#include "..\include\helper.h"

char * g_pszLogFile=NULL;
BOOL	g_bRecordLog=TRUE;
/**********************************************************************/
/*                                                                    */
/* UI_IsIMEMessage(message)                                            */
/*                                                                    */
/* Any UI window should not pass the IME messages to DefWindowProc.   */
/*                                                                    */
/**********************************************************************/
BOOL Helper_IsIMEMessage(UINT message)
{
    switch(message)
    {
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:
	case WM_IME_SETCONTEXT:
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_SELECT:
	case WM_IME_CHAR:
		return TRUE;
	default:
		return FALSE;
    }
}

void Helper_DrawBMP(HDC hdc,int x,int y,int nWid,int nHei,HBITMAP hBmp,int xSour,int ySour)
{
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP holdbmp=(HBITMAP) SelectObject(memdc,hBmp);
	if(nWid==-1 || nHei==-1)
	{
		SIZE size=Helper_GetBMPSize(hBmp);
		nWid=size.cx,nHei=size.cy;
	}
	BitBlt(hdc,x,y,nWid,nHei,memdc,xSour,ySour,SRCCOPY);
	SelectObject(memdc,holdbmp);
	DeleteDC(memdc);
}

void Helper_DrawBMP2(HDC hdc,RECT rc ,HBITMAP hBmp,int xSour,int ySour)
{
	HDC memdc=CreateCompatibleDC(hdc);
	HBITMAP holdbmp=(HBITMAP) SelectObject(memdc,hBmp);
	BitBlt(hdc,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,memdc,xSour,ySour,SRCCOPY);
	SelectObject(memdc,holdbmp);
	DeleteDC(memdc);
}

SIZE Helper_GetBMPSize(HBITMAP hBmp)
{
	SIZE sizeRet={0};
	BITMAP bm={0};
	GetObject(hBmp,sizeof(bm),&bm);
	sizeRet.cx=bm.bmWidth;
	sizeRet.cy=bm.bmHeight;
	return sizeRet;
}

void Helper_Trace(char * pszFormat,...)
{
	if(g_bRecordLog)
	{
		va_list args; 
		char buf[1024];
		va_start( args, pszFormat );
		_vsnprintf(buf,sizeof(buf)-1, pszFormat,args);
		va_end (args);
		buf[sizeof(buf)-1]=0;
		OutputDebugString(buf);
		if(g_pszLogFile)
		{
			FILE *f=fopen(g_pszLogFile,"a+");
			if(f)
			{
				fprintf(f,buf);
				fclose(f);
			}
		}
	}
}

void Helper_CenterWindow(HWND hWnd,UINT uFlag)
{
	RECT rc,screenrc;
	int x,y;
	SystemParametersInfo(SPI_GETWORKAREA,
		0,
		&screenrc,
		0);
	GetWindowRect(hWnd,&rc);
	x=(screenrc.right-rc.right+rc.left)/2;
	y=(screenrc.bottom-rc.bottom+rc.top)/2;
	SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|uFlag);
}


void Rgn_AdjustColorKey(COLORREF *pcr,BOOL bBit16)
{

	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)pcr;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if ( bBit16 )
	{
		// for 16 bit
		*pcr = ((DWORD)(pClr[0] & 0xf8) >> 3) |
				((DWORD)(pClr[1] & 0xfc) << 3) |
				((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
//		*pcr = ((DWORD)(pClr[0] & 0xf8) >> 3) |
//				((DWORD)(pClr[1] & 0xf8) << 2) |
//				((DWORD)(pClr[2] & 0xf8) << 7);
	}
}

LPBITMAPINFO  Rgn_GetBitmapDibInfo(HBITMAP hBmp,LPBYTE *ppBits,BITMAP *pbm)
{
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO) + 8 );
	// create temporary dc
	HDC dc = CreateIC( "DISPLAY",NULL,NULL,NULL );
	// get extended information about image (length, compression, length of color table if exist, ...)
	GetObject(hBmp,sizeof(BITMAP),pbm);
	memset( bi, 0, sizeof(BITMAPINFO) + 8 );
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	GetDIBits( dc, hBmp, 0, pbm->bmHeight, 0, bi, DIB_RGB_COLORS );
	// allocate memory for color table
	if ( bi->bmiHeader.biBitCount == 8 ) bi=(LPBITMAPINFO)realloc(bi,sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD));
	// allocate memory for image data (colors)
	*ppBits = (LPBYTE)malloc(bi->bmiHeader.biSizeImage + 4 );
	// fill bits buffer
	GetDIBits( dc, hBmp, 0, bi->bmiHeader.biHeight, *ppBits, bi, DIB_RGB_COLORS );
	DeleteDC( dc );
	return bi;
}

#define ADD_RECTS_COUNT		40			// number of rects to be appended
#define BMPLINEWID(wid,bytes) ((wid*bytes+3)&~0x03)	//4字节对齐,将不齐的去掉

HRGN Helper_CreateRgnFromBMP(HBITMAP hBmp,COLORREF crkey)
{
	HRGN hRgn=NULL;
	LPBYTE pBits=NULL;
	BITMAP bm = { 0 };
	LPBITMAPINFO bi=Rgn_GetBitmapDibInfo(hBmp,&pBits,&bm);
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
	// BitPerPixel
	BYTE	Bpp = bi->bmiHeader.biBitCount >> 3;				// bytes per pixel
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits;
													// (after processing of current)
	DWORD	dwRectsCount = bi->bmiHeader.biHeight;	// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
	RECT	rc;
													// where mask was found
	BOOL	wasfirst = FALSE;						// set when mask has been found in current scan line
	BOOL	ismask;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)malloc(sizeof(RGNDATAHEADER) + dwRectsCount * sizeof(RECT) );
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)(pRgnData + 1);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, sizeof(RGNDATAHEADER) + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= sizeof(RGNDATAHEADER);
	pRgnData->iType		= RDH_RECTANGLES;

	Rgn_AdjustColorKey(&crkey,bi->bmiHeader.biBitCount==16);

	for ( i = 0; i < bm.bmHeight; i++ )
	{
		for ( j = 0; j < bm.bmWidth; j++ )
		{
			// get color
			switch ( bi->bmiHeader.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] == crkey);
				break;
			case 16:
				ismask = (*(LPWORD)pColor == (WORD)crkey);
				break;
			case 24:
			case 32:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) == crkey);
				break;
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( ismask )
				{
					// save current RECT
					rc.left=first,rc.right=j,rc.top=bm.bmHeight-i-1,rc.bottom=bm.bmHeight-i;
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
			rc.left=first,rc.right=j,rc.top=bm.bmHeight-i-1,rc.bottom=bm.bmHeight-i;
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

		pColor += BMPLINEWID(bm.bmWidth,Bpp)-bm.bmWidth*Bpp;
	}
	// release image data
	free( pBits);
	free(bi);
	// create region
	hRgn = ExtCreateRegion( NULL, sizeof(RGNDATAHEADER) + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	free(pRgnData);

	return hRgn;
}

//从缓冲区中创建一个位图句柄
HBITMAP Helper_CreateBitmapFromBuffer(LPBYTE pBuffer, DWORD cbSize)
{
	HBITMAP hBmp=NULL;
	HDC hdc=NULL;
	HPALETTE hPal=NULL,hOldPal=NULL;
	BITMAPFILEHEADER *pbmfHeader=(BITMAPFILEHEADER *)pBuffer;
	DWORD dwOffset=sizeof(BITMAPFILEHEADER);
	// File type should be 'BM'

	BITMAPINFOHEADER *pbmiHeader = (LPBITMAPINFOHEADER)(pBuffer+dwOffset) ;
	BITMAPINFO * pbmInfo = (LPBITMAPINFO)(pBuffer+dwOffset) ;
	// If bmiHeader.biClrUsed is zero we have to infer the number
	// of colors from the number of bits used to specify it.
	int nColors = pbmiHeader->biClrUsed ? pbmiHeader->biClrUsed : 
						1 << pbmiHeader->biBitCount;

	LPVOID lpDIBBits;
	if (pbmfHeader->bfType != ((WORD) ('M' << 8) | 'B')) return NULL;
	if( pbmInfo->bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(pbmInfo->bmiColors + pbmInfo->bmiHeader.biClrUsed) + 
			((pbmInfo->bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(pbmInfo->bmiColors + nColors);
	
	hdc=GetDC(NULL);
	// Create the palette
	if( nColors <= 256 )
	{
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
		LOGPALETTE *pLP = (LOGPALETTE *) malloc(nSize);
		int i;
		pLP->palVersion = 0x300;
		pLP->palNumEntries = nColors;
		
		for( i=0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = pbmInfo->bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = pbmInfo->bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = pbmInfo->bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = 0;
		}
		hPal=CreatePalette(pLP);
		free(pLP);
		hOldPal=(HPALETTE)SelectObject(hdc,hPal);
		RealizePalette(hdc);
	}


	hBmp = CreateDIBitmap( hdc,		// handle to device context 
				pbmiHeader,	// pointer to bitmap size and format data 
				CBM_INIT,	// initialization flag 
				lpDIBBits,	// pointer to initialization data 
				pbmInfo,	// pointer to bitmap color-format data 
				DIB_RGB_COLORS);		// color-data usage 
	if( hPal )
	{
		SelectObject(hdc,hOldPal);
		DeleteObject(hPal);
	}
	ReleaseDC(NULL,hdc);
	return hBmp;
}

int  Helper_SetWindowRgn(HWND hWnd,HRGN hRgn,BOOL bReDraw)
{
	HRGN hRgnCopy=CreateRectRgn(0,0,0,0);
	CombineRgn(hRgnCopy, hRgn, NULL, RGN_COPY);
	return SetWindowRgn(hWnd,hRgnCopy,bReDraw);
}

//绘制字符串,自动计算长度,支持字符串超长时使用...代替
//HDC hdc,int x,int y,LPCTSTR pszText,int cbString:TextOut需要的参数
//SIZE *pSize:占用长度
//int nMaxWidth:最大长度,-1代表不限
void Helper_TextOut(HDC hdc,int x,int y,LPCTSTR pszText,int cbString,SIZE *pSize,int nMaxWidth)
{
	if(cbString==-1) cbString=strlen(pszText);
	GetTextExtentPoint32(hdc,pszText,cbString,pSize);
	if(nMaxWidth!=-1 && nMaxWidth<pSize->cx)
	{
		SIZE size;
		int nTailLen=0;
		int nOffset=0;
		int i=0;
		int nWordLen=0;
		int nTailWordLen=0;
		while(i<cbString)
		{
			nWordLen=(pszText[i]&0x80)?2:1;
			i+=nWordLen;
		}
		nTailWordLen=nWordLen;
		GetTextExtentPoint(hdc,"…",2,&size);
		nTailLen=size.cx;
		GetTextExtentPoint(hdc,pszText+cbString-nWordLen,nWordLen,&size);
		nTailLen+=size.cx;
		i=0;
		pSize->cy=size.cy;
		while (i<cbString)
		{
			nWordLen=(pszText[i]&0x80)?2:1;
			GetTextExtentPoint(hdc,pszText+i,nWordLen,&size);
			if(nOffset+size.cx>nMaxWidth-nTailLen) break;
			TextOut(hdc,nOffset+x,y,pszText+i,nWordLen);
			nOffset+=size.cx;
			i+=nWordLen;
		}
		TextOut(hdc,nOffset+x,y,"…",2);
		GetTextExtentPoint(hdc,"…",2,&size);
		nOffset+=size.cx;
		TextOut(hdc,nOffset+x,y,pszText+cbString-nTailWordLen,nTailWordLen);
		GetTextExtentPoint(hdc,pszText+cbString-nTailWordLen,nTailWordLen,&size);
		nOffset+=size.cx;
		pSize->cx=nOffset-x;
	}else
	{
		TextOut(hdc,x,y,pszText,cbString);
	}
}

void Helper_GetTextExtent(HDC hdc,LPCTSTR pszText,int cbString,SIZE *pSize,int nMaxWidth)
{
	if(cbString==-1) cbString=strlen(pszText);
	GetTextExtentPoint32(hdc,pszText,cbString,pSize);
	if(nMaxWidth!=-1 && nMaxWidth<pSize->cx) pSize->cx=nMaxWidth;
}


//获取文件的版本号
void Helper_VersionString(DWORD dwVer,char *pszBuf)
{
	sprintf(pszBuf,"%u.%u.%u.%u",(dwVer>>24)&0xFF,(dwVer>>16)&0xFF,(dwVer>>8)&0xFF,dwVer&0xFF);
}

//获取一个PE文件的version + ProductName +  FileDescription
BOOL Helper_PEVersion(LPCTSTR pszFileName,DWORD *pdwVer,char *pszName ,char *pszDesc)
{
	DWORD dwResHandle;
	LPSTR lszVer = NULL; 
	UINT  cchVer = 0;  
	void *pBuf;
	BOOL bRet=FALSE;
	DWORD dwVerInfoSize=GetFileVersionInfoSize((char*)pszFileName,&dwResHandle);
	if(!dwVerInfoSize) return FALSE;
	pBuf=malloc(dwVerInfoSize);
	GetFileVersionInfo((char*)pszFileName, dwResHandle, dwVerInfoSize, pBuf); 
	if(pdwVer)
	{
		UINT nVersionLen;
		VS_FIXEDFILEINFO *pstFileVersion;
		if(VerQueryValue(pBuf, "\\", (void**)&pstFileVersion, &nVersionLen) && nVersionLen>=sizeof(VS_FIXEDFILEINFO) )
		{
			*pdwVer=MAKELONG(MAKEWORD(LOWORD(pstFileVersion->dwFileVersionLS),HIWORD(pstFileVersion->dwFileVersionLS)),
				MAKEWORD(LOWORD(pstFileVersion->dwFileVersionMS),HIWORD(pstFileVersion->dwFileVersionMS)));
		}
	}

	bRet = VerQueryValue(pBuf, "\\VarFileInfo\\Translation", (void**)&lszVer, &cchVer); 
	if(bRet&&cchVer==4)
	{
		char szLangCode[20]={0};
		char szSection[256]; 
		BOOL bOK;
		sprintf(szLangCode,"%04x",*(WORD*)lszVer);
		sprintf(szLangCode+4,"%04x",*(WORD*)(lszVer+2));
		if(pszName)
		{
			sprintf(szSection,"\\StringFileInfo\\%s\\ProductName",szLangCode);
			bOK= VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) strcpy(pszName,lszVer);
		}
		if(pszDesc)
		{
			sprintf(szSection,"\\StringFileInfo\\%s\\FileDescription",szLangCode);
			bOK = VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) strcpy(pszDesc,lszVer);
		}
	}
	free(pBuf);
	return bRet;
}

//滑过空格及制表符
static char * StrSkipSpace(const char *pszBuf)
{
	char *p=(char*)pszBuf;
	while(*p)
	{
		switch(*p)
		{
		case 0x20:
		case 0x09:
			p++;
		default:
			return p;
		}
	}
	return p;
}

BOOL Helper_ParseIniLine(LPCTSTR pszLine,LPTSTR pszKey,LPTSTR pszData)
{
	int i=0;
	char *p;
	while(pszLine[i])
	{
		if(pszLine[i]&0x80)
			i+=2;
		else if(pszLine[i]==0x20 || pszLine[i]==0x09 || pszLine[i]=='=')
			break;
		else
			i++;
	}
	if(!pszLine[i])	return FALSE;
	p=StrSkipSpace(pszLine+i);
	if(!p || p[0]!='=') return FALSE;
	strncpy(pszKey,pszLine,i);
	pszKey[i]=0;
	p=StrSkipSpace(p+1);
	strcpy(pszData,p);
	return TRUE;
}

void Helper_WriteProfileSection(LPCTSTR pszSection,LPCTSTR pszKeys,LPCTSTR pszIniFile)
{
	char szKey[100],szData[500];
	char *p=(char*)pszKeys;
	while(p[0])
	{
		if(Helper_ParseIniLine(p,szKey,szData)) WritePrivateProfileString(pszSection,szKey,szData,pszIniFile);
		p+=strlen(p)+1;
	}
}

DWORD Helper_GetProfileSectionString(
  LPCTSTR lpKeyName,        // points to key name
  LPCTSTR lpDefault,        // points to default string
  LPTSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCTSTR lpSectionData     // points to initialization data string list
)
{
	LPTSTR p=(LPTSTR)lpSectionData;
	if(p)
	{
		int nKeyLen=strlen(lpKeyName);
		while(*p)
		{
			if(strncmp(lpKeyName,p,nKeyLen)==0)
			{
				p=StrSkipSpace(p+nKeyLen);
				if(p[0]=='=')
				{
					UINT nLen=0;
					p=StrSkipSpace(p+1);
					nLen=strlen(p);
					if(nLen>=nSize) return nSize-2;
					strcpy(lpReturnedString,p);
					return nLen;
				}
			}
			p+=strlen(p)+1;
		}
	}
	if(lpDefault)
		strcpy(lpReturnedString,lpDefault);
	else
		strcpy(lpReturnedString,"");
	return strlen(lpReturnedString);
}

int Helper_GetProfileSectionInt( 
  LPCTSTR lpKeyName,        // points to key name
  int nDefault,				// return value if key name is not found
  LPCTSTR lpSectionData     // points to initialization data string list
)
{
	char szBuf[50];
	int nRet=Helper_GetProfileSectionString(lpKeyName,NULL,szBuf,50,lpSectionData);
	if(nRet==0||nRet==48) return nDefault;
	return atoi(szBuf);
}

BOOL WritePrivateProfileInt(
  LPCTSTR lpAppName,  // pointer to section name
  LPCTSTR lpKeyName,  // pointer to key name
  int nKeyValue,   // value to write
  LPCTSTR lpFileName  // pointer to initialization filename
)
{
	char szBuf[100];
	sprintf(szBuf,"%d",nKeyValue);
	return WritePrivateProfileString(lpAppName,lpKeyName,szBuf,lpFileName);
}

typedef BOOL (WINAPI *FunSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

void Helper_SetWindowLayerAttributes(HWND hWnd,BYTE byTrans)
{

	HMODULE hUser32 = GetModuleHandle("user32.dll"); 
	if(hUser32)
	{
		FunSetLayeredWindowAttributes funSetLayer=(FunSetLayeredWindowAttributes)GetProcAddress(hUser32,"SetLayeredWindowAttributes"); 
		funSetLayer(hWnd,0,byTrans,0x02);
	}
}

typedef BOOL (WINAPI *FunChangeMsgFilter)(UINT,DWORD);

BOOL Helper_ChangeWindowMessageFilter(UINT message, DWORD dwFlag)
{
	HMODULE hUser32=GetModuleHandle("user32.dll");
	if(hUser32)
	{
		FunChangeMsgFilter fMsgFilter=(FunChangeMsgFilter)GetProcAddress(hUser32,"ChangeWindowMessageFilter");
		if(!fMsgFilter) return FALSE;
		return fMsgFilter(message,dwFlag);
	}else
	{
		return FALSE;
	}
}

//计算显示指定的文本需要的空间
BOOL Helper_TextBox_CalcRect(HDC hdc,char *pszText,int nLen,SIZE *pSize,int nMaxWid,int nTabSize,int nLineInter)
{
	int i=0,iLineChar=0;
	POINT pt={0};
	SIZE  sChar={0};
	BOOL  bNewLine=FALSE;
	if(nMaxWid<50 || nTabSize>10) return FALSE;
	if(nLen==-1) nLen=strlen(pszText);
	pSize->cx=0;
	pSize->cy=0;
	while(i<nLen)
	{
		if(pszText[i]&0x80)
		{//中文
			GetTextExtentPoint(hdc,pszText+i,2,&sChar);
			if(pt.x+sChar.cx>nMaxWid)
				bNewLine=TRUE;
			else
				i+=2, iLineChar+=2, pt.x+=sChar.cx;
		}else
		{
			if(pszText[i]==0x0D)
			{//换行
				if(pszText[i+1]==0x0A)
					i+=2;
				else
					i++;
				bNewLine=TRUE;
			}else if(pszText[i]==0x0A)
			{//换行
				bNewLine=TRUE;
				i++;
			}else if(pszText[i]==0x09)//tab key
			{//制表符
				int nLeftSpace=nTabSize-iLineChar%nTabSize;
				GetTextExtentPoint(hdc,"\t",1,&sChar);
				sChar.cx*=nLeftSpace;
				i++;
				if(pt.x+sChar.cx>nMaxWid)
				{
					bNewLine=TRUE;
				}else
				{
					iLineChar+=nLeftSpace;
					pt.x+=sChar.cx;
				}
			}else
			{//谱通英文
				GetTextExtentPoint(hdc,pszText+i,1,&sChar);
				if(pt.x+sChar.cx>nMaxWid)
					bNewLine=TRUE;
				else
					i++, iLineChar++, pt.x+=sChar.cx;
			}
		}
		if(bNewLine)
		{//需要换行
			if(pSize->cx<pt.x) pSize->cx=pt.x;
			pt.y+=nLineInter+sChar.cy;
			pt.x=0;
			iLineChar=0;
			bNewLine=FALSE;
		}
	}
	if(pSize->cx<pt.x) pSize->cx=pt.x;
	pSize->cy=pt.y+sChar.cy;
	return TRUE;
}

//显示文本
void Helper_TextBox_Draw(HDC hdc,char *pszText,int nLen,POINT ptOffset,int nMaxWid,int nTabSize,int nLineInter)
{
	int i=0,iLineChar=0;
	POINT pt={0};
	SIZE  sChar={0};
	BOOL  bNewLine=FALSE;
	if(nMaxWid<50 || nTabSize>10) return;
	if(nLen==-1) nLen=strlen(pszText);
	while(i<nLen)
	{
		if(pszText[i]&0x80)
		{//中文
			GetTextExtentPoint(hdc,pszText+i,2,&sChar);
			if(pt.x+sChar.cx>nMaxWid)
				bNewLine=TRUE;
			else
			{
				TextOut(hdc,ptOffset.x+pt.x,ptOffset.y+pt.y,pszText+i,2);
				i+=2, iLineChar+=2, pt.x+=sChar.cx;
			}
		}else
		{
			if(pszText[i]==0x0D)
			{//换行
				if(pszText[i+1]==0x0A)
					i+=2;
				else
					i++;
				bNewLine=TRUE;
			}else if(pszText[i]==0x0A)
			{
				bNewLine=TRUE;
				i++;
			}else if(pszText[i]==0x09)//tab key
			{//制表符
				int nLeftSpace=nTabSize-iLineChar%nTabSize;
				GetTextExtentPoint(hdc,"\t",1,&sChar);
				sChar.cx*=nLeftSpace;
				i++;
				if(pt.x+sChar.cx>nMaxWid)
				{
					bNewLine=TRUE;
				}else
				{
					iLineChar+=nLeftSpace;
					pt.x+=sChar.cx;
				}
			}else
			{//谱通英文
				GetTextExtentPoint(hdc,pszText+i,1,&sChar);
				if(pt.x+sChar.cx>nMaxWid)
					bNewLine=TRUE;
				else
				{
					TextOut(hdc,ptOffset.x+pt.x,ptOffset.y+pt.y,pszText+i,1);
					i++, iLineChar++, pt.x+=sChar.cx;
				}
			}
		}
		if(bNewLine)
		{//需要换行
			pt.y+=nLineInter+sChar.cy;
			pt.x=0;
			iLineChar=0;
			bNewLine=FALSE;
		}
	}
}

//区域拉伸
HRGN Helper_StretchRgn(HRGN Rgn, float HScaling, float VScaling)
{
    HRGN rgn = NULL;
    DWORD length = GetRegionData(Rgn, 0, NULL);
    if (length != 0)
    {
        PRGNDATA data = (PRGNDATA)malloc(length);
        if (data != NULL)
        {
            if (GetRegionData(Rgn, length, data) != 0)
            {
                XFORM xfrom = {HScaling, 0, 0, VScaling, 0, 0};
                rgn = ExtCreateRegion(&xfrom, length, data);
            }
			free(data);
        }
    }
    return rgn;
}

BOOL Helper_IsLayerSupport()
{
	OSVERSIONINFO     ver; 
	ver.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&ver);
	return ver.dwMajorVersion>=5;
}

