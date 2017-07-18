#ifndef _MYIMAGE_
#define _MYIMAGE_

#include "helper.h"
#include "png32_helper.h"

typedef enum tagIMGTYPE
{
	IMG_BMP=0,	//位图
	IMG_PNG,	//PNG32
} IMGTYPE;


typedef struct tagIMAGE
{
	IMGTYPE imgType;
	HBITMAP hBmp;
	BOOL bTransparent;	//位图中使用的透明标志
	PNGBLEND pb;		//PNG图使用的混合标志
}IMAGE,*PIMAGE;

PIMAGE	Image_Load(const char *pszFileName,IMGTYPE type);
void	Image_Show(HDC hdc,int xDest,int yDest,int nWidth,int nHeight,PIMAGE pImg,int xSour,int ySour);
void	Image_Free(PIMAGE pImg);
HRGN	Image_MakeRgn(PIMAGE pImg,DWORD dwData);
SIZE	Image_GetSize(PIMAGE pImg);
#endif//_MYIMAGE