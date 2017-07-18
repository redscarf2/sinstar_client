#ifndef _PNG32_HELPER_
#define _PNG32_HELPER_


/* Inhibit C++ name-mangling for libpng functions but not for system calls. */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct tagPNGBLEND
{
	BOOL bBlend;				//blend flag: true - blend image with background
	COLORREF cr;				//blend color
}PNGBLEND;


HBITMAP __cdecl Bmp32_LoadPng(LPCTSTR pszFileName);
HBITMAP __cdecl Bmp32_LoadPngData(LPBYTE lpData,DWORD dwSize);
HRGN	__cdecl Bmp32_MakeRgn(HBITMAP hBmp,BYTE byBlur,RECT *pLimitRect);
BOOL __cdecl Bmp32_AlphaBlend(  HDC hdcDest,int nXOriginDest,int nYOriginDest,int nWidth,int nHeight,HBITMAP hbmp32,int nXOriginSrc, int nYOriginSrc,PNGBLEND blend);

#ifdef __cplusplus
}
#endif

#endif//_PNG32_HELPER_