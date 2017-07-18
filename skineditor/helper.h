#pragma once
#include <windows.h>
#include <stdarg.h>
#include <wingdi.h>
#include <stdio.h>

#define GETLPCOMPSTR(lpcs) (LPTSTR)((LPBYTE)(lpcs) + (lpcs)->dwCompStrOffset)
#define GETLPRESULTSTR(lpcs) (LPTSTR)((LPBYTE)(lpcs) + (lpcs)->dwResultStrOffset)

#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

extern char * g_pszLogFile;
extern BOOL	g_bRecordLog;

void Helper_Trace(char * pszFormat,...);

void Helper_DrawBMP(HDC hdc,int x,int y,int nWid,int nHei,HBITMAP hBmp,int xSour,int ySour);

void Helper_DrawBMP2(HDC hdc,RECT rc ,HBITMAP hBmp,int xSour,int ySour);

SIZE Helper_GetBMPSize(HBITMAP hBmp);

HRGN Helper_CreateRgnFromBMP(HBITMAP hBmp,COLORREF crkey);

HBITMAP Helper_CreateBitmapFromBuffer(LPBYTE pBuffer, DWORD cbSize);

BOOL Helper_IsIMEMessage(UINT message);

void Helper_CenterWindow(HWND hWnd,UINT uFlag);

int  Helper_SetWindowRgn(HWND hWnd,HRGN hRgn,BOOL bReDraw);

void Helper_TextOut(HDC hdc,int x,int y,LPCTSTR pszText,int cbString,SIZE *pSize,int nMaxWidth);

void Helper_GetTextExtent(HDC hdc,LPCTSTR pszText,int cbString,SIZE *pSize,int nMaxWidth);

#pragma comment(lib,"version.lib")
void Helper_VersionString(DWORD dwVer,char *pszBuf);

BOOL Helper_PEVersion(LPCTSTR pszFileName,DWORD *pdwVer,char *pszName ,char *pszDesc);

BOOL Helper_ParseIniLine(LPCTSTR pszLine,LPTSTR pszKey,LPTSTR pszData);
void Helper_WriteProfileSection(LPCTSTR pszSection,LPCTSTR pszKeys,LPCTSTR pszIniFile);

DWORD Helper_GetProfileSectionString(
  LPCTSTR lpKeyName,        // points to key name
  LPCTSTR lpDefault,        // points to default string
  LPTSTR lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  LPCTSTR lpSectionData     // points to initialization data string list
);

int Helper_GetProfileSectionInt( 
  LPCTSTR lpKeyName,        // points to key name
  int nDefault,				// return value if key name is not found
  LPCTSTR lpSectionData     // points to initialization data string list
);

BOOL WritePrivateProfileInt(
  LPCTSTR lpAppName,  // pointer to section name
  LPCTSTR lpKeyName,  // pointer to key name
  int nKeyValue,   // value to write
  LPCTSTR lpFileName  // pointer to initialization filename
);

void Helper_SetWindowLayerAttributes(HWND hWnd,BYTE byTrans);

#ifdef __cplusplus
}
#endif//__cplusplus
