#ifndef _PNG32_HELPER_
#define _PNG32_HELPER_


/* Inhibit C++ name-mangling for libpng functions but not for system calls. */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tagPNGINFO
{
	int nWidth;					//width
	int nHeight;				//height
	unsigned char **ppbyRow;	//data:rgba
}PNGINFO;

typedef struct tagPNGBLEND
{
	BOOL bBlend;				//blend flag: true - blend image with background
	COLORREF cr;				//blend color
}PNGBLEND;

PNGINFO * Png32_Load(const char * pszFileName);
PNGINFO *Png32_LoadFromMem(unsigned char *pbyData,DWORD dwSize);
void	Png32_Free(PNGINFO * pPngInfo);
void	Png32_Show(HDC hdc,int xDest,int yDest,int nWidth,int nHeight, PNGINFO * pPngInfo,int xSour,int ySour);
void	Png32_ShowEx(HDC hdc,int xDest,int yDest,int nWidth,int nHeight, PNGINFO * pPngInfo,int xSour,int ySour,PNGBLEND *pBlend);
HRGN	Png32_MakeRgn(PNGINFO *pPngInfo);
HRGN	Png32_MakeRgnEx(PNGINFO *pPngInfo,BYTE byBlur);

HBITMAP Bmp32_LoadPng(const char * pszFileName);
HBITMAP Bmp32_LoadPngData(LPBYTE lpData,DWORD dwSize);
HRGN	Bmp32_MakeRgn(HBITMAP hBmp,BYTE byBlur);
BOOL Bmp32_AlphaBlend(  HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidth,int nHeight,HBITMAP hbmp32,int nXOriginSrc, int nYOriginSrc,PNGBLEND blend);

#ifdef __cplusplus
}
#endif

#endif//_PNG32_HELPER_