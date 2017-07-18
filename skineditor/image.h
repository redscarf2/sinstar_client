#ifndef _MYIMAGE_
#define _MYIMAGE_

#include "helper.h"
#include "png32_helper.h"

typedef enum tagIMGTYPE
{
	IMG_BMP=0,	//λͼ
	IMG_PNG,	//PNG32
} IMGTYPE;


typedef struct tagIMAGE
{
	IMGTYPE imgType;
	HBITMAP hBmp;
	BOOL bTransparent;	//λͼ��ʹ�õ�͸����־
	PNGBLEND pb;		//PNGͼʹ�õĻ�ϱ�־
}IMAGE,*PIMAGE;

PIMAGE	Image_Load(const char *pszFileName,IMGTYPE type);
void	Image_Show(HDC hdc,int xDest,int yDest,int nWidth,int nHeight,PIMAGE pImg,int xSour,int ySour);
void	Image_Free(PIMAGE pImg);
HRGN	Image_MakeRgn(PIMAGE pImg,DWORD dwData);
SIZE	Image_GetSize(PIMAGE pImg);
#endif//_MYIMAGE