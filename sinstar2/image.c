#include <windows.h>
#include "image.h"

PIMAGE	Image_Load(const char *pszFileName,IMGTYPE type)
{
	PIMAGE pRet=NULL;
	switch(type)
	{
	case IMG_BMP:
		{
			HBITMAP hBmp=(HBITMAP)LoadImage(NULL,pszFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
			if(hBmp)
			{
				pRet=(PIMAGE)malloc(sizeof(IMAGE));
				pRet->imgType=IMG_BMP;
				pRet->hBmp=hBmp;
				pRet->bTransparent=FALSE;
			}
		}
		break;
	case IMG_PNG:
		{
			HBITMAP hBmp=Bmp32_LoadPng(pszFileName);
			if(hBmp)
			{
				pRet=(PIMAGE)malloc(sizeof(IMAGE));
				pRet->imgType=IMG_PNG;
				pRet->hBmp=hBmp;
				pRet->pb.bBlend=TRUE;
				pRet->bTransparent=TRUE;
			}
		}
		break;
	default:
		break;
	}
	return pRet;
}

void	Image_Show(HDC hdc,int xDest,int yDest,int nWidth,int nHeight,PIMAGE pImg,int xSour,int ySour)
{
	if(!pImg) return;
	switch(pImg->imgType)
	{
	case IMG_BMP:
		Helper_DrawBMP(hdc,xDest,yDest,nWidth,nHeight,pImg->hBmp,xSour,ySour);
		break;
	case IMG_PNG:
		Bmp32_AlphaBlend(hdc,xDest,yDest,nWidth,nHeight,pImg->hBmp,xSour,ySour,pImg->pb);
		break;
	default:
		break;
	}
}
void	Image_Free(PIMAGE pImg)
{
	if(!pImg) return;
	DeleteObject(pImg->hBmp);
	free(pImg);
}

HRGN	Image_MakeRgn(PIMAGE pImg,DWORD dwData)
{
	HRGN hRet=0;
	if(!pImg) return 0;
	switch(pImg->imgType)
	{
	case IMG_BMP:
		hRet=Helper_CreateRgnFromBMP(pImg->hBmp,dwData);
		break;
	case IMG_PNG:
		hRet=Bmp32_MakeRgn(pImg->hBmp,(BYTE)dwData,NULL);
		break;
	default:
		break;
	}
	return hRet;
}

SIZE	Image_GetSize(PIMAGE pImg)
{
	SIZE size={0};
	if(!pImg) return size;
	size=Helper_GetBMPSize(pImg->hBmp);
	return size;
}