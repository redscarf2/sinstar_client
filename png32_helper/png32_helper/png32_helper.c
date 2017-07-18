//32PNG图片处理
//修改为数据载入时做一次颜色的乘法。
//注：图象失真：1、数据载入时做乘法会丢失低字节数据；2、在做/255的计算时采用了只取高字节的方式，等价于/256会导致失真。
//2008年4月17日：增加从内存载入图象的方法。
//2008年10月8日：增加将32位PNG图转换为32位内存位置的接口Bmp32_*

#include <windows.h>
#include <wingdi.h>
#include <tchar.h>
#include "png32_helper.h"
#include "../png/png.h"
#pragma  comment(lib,"msimg32")

#ifndef WIDTHBYTES
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#endif

#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 1
#endif

#define ADD_RECTS_COUNT 40
#define ASM_CORE

typedef struct tagMEMFILE
{
	unsigned char *pbyData;	//data
	unsigned char *pbyCur;	//cur data
	DWORD dwRemain;			//valid data size
}MEMFILE,*PMEMFILE;

void __cdecl MemFile_Init(PMEMFILE pMemFile,unsigned char *pbyData,DWORD dwSize)
{
	pMemFile->pbyData=pMemFile->pbyCur=pbyData;
	pMemFile->dwRemain=dwSize;
}

void __cdecl MemFile_Read(png_structp png_ptr, png_bytep pbyData, png_size_t nLength)
{
	PMEMFILE pMemFile=(PMEMFILE)png_ptr->io_ptr;
	if(pMemFile->dwRemain>=nLength)
	{
		memcpy(pbyData,pMemFile->pbyCur,nLength);
		pMemFile->pbyCur+=nLength;
		pMemFile->dwRemain-=nLength;
	}else
	{
		png_error(png_ptr,"io error");
	}
}


BOOL Png32_Mem2PngInfo(LPBYTE lpData,DWORD dwSize, png_structp *ppPng,png_infop *ppInfo)
{
	MEMFILE memfile;
	// create read struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	// create info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	// create private read struct
	MemFile_Init(&memfile,lpData,dwSize);
	// init io mode
	png_set_read_fn(png_ptr,&memfile,MemFile_Read);
	// read entire image (high level)
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
	if(info_ptr->valid & PNG_INFO_IDAT && info_ptr->pixel_depth==32)
	{
		*ppPng=png_ptr;
		*ppInfo=info_ptr;
		return TRUE;
	}else
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return FALSE;
	}
}


BOOL Bmp32_File2PngInfo(LPCTSTR pszFileName, png_structp *ppPng,png_infop *ppInfo)
{

#pragma warning (push)
#pragma warning (disable : 4996)
	FILE* file = _tfopen(pszFileName,_T("rb"));
#pragma warning (pop)

	// unable to open
	if (file)
	{
		// create read struct
		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		// create info struct
		png_infop info_ptr = png_create_info_struct(png_ptr);
		png_init_io(png_ptr, file);
		// read entire image (high level)
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
		fclose(file);
		if(info_ptr->valid & PNG_INFO_IDAT && info_ptr->pixel_depth==32)
		{
			*ppPng=png_ptr;
			*ppInfo=info_ptr;
			return TRUE;
		}else
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		}
	}
	return FALSE;
}

HBITMAP __cdecl Bmp32_LoadFromPngInfo(png_infop info_ptr)
{
	HBITMAP hRet=NULL;
	if(info_ptr->valid & PNG_INFO_IDAT && info_ptr->pixel_depth==32)
	{
		int i,j;
		HDC hdc=GetDC(NULL),hmemdc=0;
		HGDIOBJ hOldBmp=0;
		BITMAPINFO bmi={0};
		LPBYTE pBitsDest=NULL;
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = info_ptr->width;
		bmi.bmiHeader.biHeight = info_ptr->height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		
		// Create the surface.
		hmemdc=CreateCompatibleDC(hdc);
		// Get Dest Rectangle memory
		hRet = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,(void **)& pBitsDest, NULL, 0);
		hOldBmp=SelectObject(hmemdc,hRet);
		
		//do pre-muliple
		for(i=(int)info_ptr->height-1;i>=0;i--)
		{
			BYTE *pLine=info_ptr->row_pointers[i];
			for(j=0;j<(int)info_ptr->width;j++)
			{
				*pBitsDest++=pLine[2]*pLine[3]/255;
				*pBitsDest++=pLine[1]*pLine[3]/255;
				*pBitsDest++=pLine[0]*pLine[3]/255;
				*pBitsDest++=pLine[3];
				pLine+=4;
			}
		}
		SelectObject(hmemdc,hOldBmp);
		DeleteDC(hmemdc);
		ReleaseDC(NULL,hdc);		
	}
	return hRet;
}


HBITMAP __cdecl Bmp32_LoadPng(LPCTSTR pszFileName)
{
	HBITMAP hRet=0;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	if(Bmp32_File2PngInfo(pszFileName,&png_ptr,&info_ptr))
	{
		hRet=Bmp32_LoadFromPngInfo(info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	}
	return hRet;
}

HBITMAP __cdecl Bmp32_LoadPngData(LPBYTE lpData,DWORD dwSize)
{
	HBITMAP hRet=0;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	if(Png32_Mem2PngInfo(lpData,dwSize,&png_ptr,&info_ptr))
	{
		hRet=Bmp32_LoadFromPngInfo(info_ptr);
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	}
	return hRet;
}

//从一个32的位图中提供的alpha通道创建区域
HRGN	__cdecl Bmp32_MakeRgn(HBITMAP hBmp,BYTE byBlur,RECT *pLimitRect)
{
	HRGN hRgn=NULL;
	// (after processing of current)
	BITMAP bm = { 0 };
	GetObject(hBmp,sizeof(BITMAP),&bm);
	if(bm.bmBitsPixel ==32)
	{
		RECT rcBmp={0,0,bm.bmWidth,bm.bmHeight};
		HDC hdc=GetDC(NULL);
		INT		i, j;									// current position in mask image
		INT		first = 0;								// left position of current scan line
		RECT	rc;
		DWORD	dwRectsCount=bm.bmHeight;				// number of rects in allocated buffer
		LPRECT  pRects=NULL;
		// where mask was found
		BOOL	wasfirst = FALSE;						// set when mask has been found in current scan line
		BOOL	ismask;									// set when current color is mask color
		LPBYTE pBits=NULL,p=NULL;
		RGNDATAHEADER* pRgnData=NULL;
		LPBITMAPINFO pbi = (LPBITMAPINFO) calloc(sizeof(BITMAPINFO) + 8 ,1);
		pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		GetDIBits( hdc, hBmp, 0, bm.bmHeight, 0, pbi, DIB_RGB_COLORS );
		pBits=(LPBYTE)malloc(pbi->bmiHeader.biSizeImage);
		GetDIBits( hdc, hBmp, 0, bm.bmHeight, pBits, pbi, DIB_RGB_COLORS );
		ReleaseDC(NULL,hdc);


		// allocate memory for region data
		// region data here is set of regions that are rectangles with height 1 pixel (scan line)
		// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
		pRgnData = (RGNDATAHEADER*)calloc(sizeof(RGNDATAHEADER) + dwRectsCount * sizeof(RECT) ,1);
		// get pointer to RECT table
		pRects = (LPRECT)(pRgnData + 1);
		// fill it by default
		pRgnData->dwSize	= sizeof(RGNDATAHEADER);
		pRgnData->iType		= RDH_RECTANGLES;

		p=pBits;
		if(!pLimitRect) pLimitRect=&rcBmp;
		for ( i = bm.bmHeight-pLimitRect->top-1; i>=bm.bmHeight-pLimitRect->bottom;i-- )
		{
			p=pBits+i*bm.bmWidth*4+pLimitRect->left*4;
			for ( j = pLimitRect->left; j < pLimitRect->right; j++ )
			{
				ismask=p[3]<=byBlur;
				p+=4;
				if ( wasfirst )
				{
					if ( ismask )
					{
						// save current RECT
						rc.left=first,rc.right=j,rc.top=(bm.bmHeight-i),rc.bottom=(bm.bmHeight-i+1);
						pRects[ pRgnData->nCount++ ] = rc;
						// if buffer full reallocate it with more room
						if ( pRgnData->nCount >= dwRectsCount )
						{
							dwRectsCount += ADD_RECTS_COUNT;
							pRgnData=(RGNDATAHEADER*)realloc(pRgnData,sizeof(RGNDATAHEADER) + dwRectsCount * sizeof(RECT) );
							pRects = (LPRECT)(pRgnData + 1);
						}
						wasfirst = FALSE;
					}
				}else if ( !ismask )		// set wasfirst when mask is found
				{
					first = j;
					wasfirst = TRUE;
				}
			}

			if ( wasfirst && !ismask )
			{
				// save current RECT
				rc.left=first,rc.right=j,rc.top=(bm.bmHeight-i),rc.bottom=(bm.bmHeight-i+1);
				pRects[ pRgnData->nCount++ ] = rc;
				// if buffer full reallocate it with more room
				if ( pRgnData->nCount >= dwRectsCount )
				{
					dwRectsCount += ADD_RECTS_COUNT;
					pRgnData=(RGNDATAHEADER*)realloc(pRgnData,sizeof(RGNDATAHEADER) + dwRectsCount * sizeof(RECT) );
					pRects = (LPRECT)(pRgnData + 1);
				}
				wasfirst = FALSE;
			}
		}
		free( pBits);
		free(pbi);
		// create region
		hRgn = ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
		// release region data
		free(pRgnData);

	}
	return hRgn;
}

BOOL __cdecl Bmp32_AlphaBlend(  HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidth,int nHeight,HBITMAP hbmp32,int nXOriginSrc, int nYOriginSrc,PNGBLEND blend)
{
	BITMAP bm;
	BLENDFUNCTION bf={AC_SRC_OVER,0,255,AC_SRC_ALPHA};
	HDC	hmemdc=CreateCompatibleDC(hdcDest);
	HGDIOBJ hOldBmp=SelectObject(hmemdc,hbmp32);
	BOOL bRet=FALSE;
	GetObject(hbmp32,sizeof(BITMAP),&bm);
	if(nWidth+nXOriginSrc>bm.bmWidth)	nWidth=bm.bmWidth-nXOriginSrc;
	if(nHeight+nYOriginSrc>bm.bmHeight)	nHeight=bm.bmHeight-nYOriginSrc;
	if(!blend.bBlend)
	{//使用双缓冲来将图与纯色背景混合
		HBRUSH hbr=CreateSolidBrush(blend.cr);
		HDC hmemdc2=CreateCompatibleDC(hdcDest);
		HBITMAP hMemBmp=CreateCompatibleBitmap(hdcDest,nWidth,nHeight);
		HGDIOBJ hOldBmp2=SelectObject(hmemdc2,hMemBmp);
		RECT rc={0,0,nWidth,nHeight};
		FillRect(hmemdc2,&rc,hbr);
		bRet=AlphaBlend(hmemdc2,0,0,nWidth,nHeight,hmemdc,nXOriginSrc,nYOriginSrc,nWidth,nHeight,bf);
		if(bRet) BitBlt(hdcDest,nXOriginDest,nYOriginDest,nWidth,nHeight,hmemdc2,0,0,SRCCOPY);
		SelectObject(hmemdc2,hOldBmp2);
		DeleteObject(hMemBmp);
		DeleteDC(hmemdc2);
		DeleteObject(hbr);
	}else
	{
		bRet=AlphaBlend(hdcDest,nXOriginDest,nYOriginDest,nWidth,nHeight,hmemdc,nXOriginSrc,nYOriginSrc,nWidth,nHeight,bf);
	}
	SelectObject(hmemdc,hOldBmp);
	DeleteDC(hmemdc);
	return bRet;
}
