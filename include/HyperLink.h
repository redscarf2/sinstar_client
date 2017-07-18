// HyperLink_C.h: interface for the CHyperLink_C module.
// before user the module, insure you have defined _WIN32_WINNT as 0x0400
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HYPERLINK_C_H__8859DE96_3C94_4734_834D_90AE2529E2BE__INCLUDED_)
#define AFX_HYPERLINK_C_H__8859DE96_3C94_4734_834D_90AE2529E2BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tagLINKDATA {
	UINT uCtrlID;				// the ID of the Link
	TCHAR	szText[MAX_PATH];	// Text to be display on the Link
	TCHAR	szURL[MAX_PATH];	// The URL of the Link
	TCHAR	szTip[MAX_PATH];	// The tooltip of the Link
}LINKDATA,*PLINKDATA;

void LinkInfo_SetStyle(COLORREF crNormal,COLORREF crHover,COLORREF crClicked,BOOL bUnderline);
void LinkInfo_Init(HWND hDlg,LINKDATA * pLinks,int cbSize);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // !defined(AFX_HYPERLINK_C_H__8859DE96_3C94_4734_834D_90AE2529E2BE__INCLUDED_)
