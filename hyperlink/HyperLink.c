// HyperLink_C.cpp: implementation of the CHyperLink_C Module.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <ShellAPI.h>
#include <commctrl.h>
#include "..\include\HyperLink.h"

// All these should be defined in newer version windows header files

#ifndef		IDC_HAND
#define		IDC_HAND		MAKEINTRESOURCE(32649)
#endif

#ifndef TTS_BALLOON
#define TTS_BALLOON 0x40
#endif

#ifndef WM_MOUSELEAVE
#define WM_MOUSELEAVE                   0x02A3
#endif//WM_MOUSELEAVE
//

typedef struct tagLINKINFO {
	WNDPROC	wpOrigProc;			// We need to set cursor to be HAND when mouse if over the Link. To remember the original button Control WinProc
	TCHAR	szText[MAX_PATH];	// Text to be display on the Link
	TCHAR	szURL[MAX_PATH];	// The URL of the Link
	HWND	hWndTT;				// the Tooltip Window
	BOOL	bHover;				// The flag indicates whether the Link is capturing cursor
	BOOL	bClicked;			// The flag indicates whether the Link has been clicked
} LINKINFO,*PLINKINFO;

typedef struct tagLINKSTYLE{
	COLORREF crNormal;
	COLORREF crHover;
	COLORREF crClicked;
	BOOL	 bUnderline;
}LINKSTYLE;

typedef BOOL (WINAPI *SETLAYEREDWINDOW)(HWND,COLORREF,BYTE,DWORD); 


static LINKSTYLE s_hlStyle={
	RGB(0x00,0x00,0xCC),
	RGB(0xFF,0x99,0x00),
	RGB(0x66,0x00,0x66),
	TRUE,
};

static HCURSOR	s_HandCursor=0;


static LRESULT CALLBACK Link_WinProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet=0;
	PLINKINFO pLinkInfo=(PLINKINFO)GetWindowLongPtr(hWnd,GWLP_USERDATA);
	WNDPROC	wpOrigProc=pLinkInfo->wpOrigProc;
	if(pLinkInfo->hWndTT)
	{
		MSG msg={hWnd,uMsg,wParam,lParam};
		SendMessage(pLinkInfo->hWndTT,TTM_RELAYEVENT,0,(LPARAM)&msg);
	}
	if(uMsg==WM_PAINT)
	{
		PAINTSTRUCT ps={0};
		HDC hdc=BeginPaint(hWnd,&ps);
		HGDIOBJ hFont=(HGDIOBJ)SendMessage(GetParent(hWnd),WM_GETFONT,0,0);
		HGDIOBJ hOldFont=SelectObject(hdc,hFont);
		if(pLinkInfo->bHover)
			SetTextColor(hdc,s_hlStyle.crHover);
		else if(pLinkInfo->bClicked)
			SetTextColor(hdc,s_hlStyle.crClicked);
		else
			SetTextColor(hdc,s_hlStyle.crNormal);
		SetBkMode(hdc,TRANSPARENT);
		TextOut(hdc,0,0,pLinkInfo->szText,strlen(pLinkInfo->szText));
		if(pLinkInfo->bHover && s_hlStyle.bUnderline)
		{
			SIZE size;
			HGDIOBJ hPen=CreatePen(PS_SOLID,1,s_hlStyle.crHover);
			HGDIOBJ hOldPen=SelectObject(hdc,hPen);
			GetTextExtentPoint(hdc,pLinkInfo->szText,strlen(pLinkInfo->szText),&size);
			MoveToEx(hdc,0,size.cy-1,NULL);
			LineTo(hdc,size.cx,size.cy-1);
			SelectObject(hdc,hOldPen);
			DeleteObject(hPen);
		}
		SelectObject(hdc,hOldFont);
		EndPaint(hWnd,&ps);
		lRet=1;
	}else if(uMsg==WM_DESTROY)
	{
		if(pLinkInfo->hWndTT) DestroyWindow(pLinkInfo->hWndTT);
		free(pLinkInfo);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
		SetWindowLongPtr(hWnd,GWLP_WNDPROC,(LONG_PTR)wpOrigProc);
	}else if(uMsg==WM_MOUSEMOVE)
	{
		if(!pLinkInfo->bHover)
		{
			pLinkInfo->bHover = TRUE;
			InvalidateRect(hWnd,NULL,TRUE);
			SetTimer(hWnd,100,100,NULL);
		}
	}else if(uMsg==WM_MOUSELEAVE)
	{
		pLinkInfo->bHover=FALSE;
		InvalidateRect(hWnd,NULL,TRUE);
	}else if(uMsg==WM_LBUTTONDOWN)
	{
		pLinkInfo->bClicked=TRUE;
		SetFocus(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		ShellExecute(NULL,"open",pLinkInfo->szURL,NULL,NULL,SW_SHOWDEFAULT);
	}else if(uMsg==WM_ERASEBKGND)
	{
		HDC hdc = (HDC) wParam;
		HBRUSH hbr=(HBRUSH)SendMessage(GetParent(hWnd),WM_CTLCOLORSTATIC ,wParam,(LPARAM)hWnd);
		RECT rc;
		GetClientRect(hWnd,&rc);
		FillRect(hdc,&rc,hbr);
		lRet=1;
	}else if(uMsg==WM_SETCURSOR)
	{
		if(s_HandCursor==0)
		{
			s_HandCursor=LoadCursor(NULL, IDC_HAND);
		}
		SetCursor(s_HandCursor);
		lRet=1;
	}else if(uMsg==WM_TIMER)
	{
		RECT rc;
		POINT pt;
		GetWindowRect(hWnd,&rc);
		GetCursorPos(&pt);
		if(!PtInRect(&rc,pt))
		{
			KillTimer(hWnd,100);
			SendMessage(hWnd,WM_MOUSELEAVE,0,0);
		}
	}
	
	if(!lRet) lRet=CallWindowProc(wpOrigProc,hWnd,uMsg,wParam,lParam);
	return lRet;
}

HWND LinkInfo_AddTip(HWND hWnd,LPCSTR pszTip)
{
	HWND hWndTT = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP|TTS_ALWAYSTIP, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);
	if(hWndTT)
	{
		TOOLINFO ti;					// struct specifying info about tool in ToolTip control
		/* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT;;
		ti.hwnd = hWnd;
		ti.hinst = NULL;
		ti.uId = 1;
		ti.lpszText = (char*)pszTip;
		// ToolTip control will cover the whole window (Button/Link) Client Area
		GetClientRect(hWnd, &ti.rect);
    
		/* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
		SendMessage(hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
		SendMessage(hWndTT, TTM_SETMAXTIPWIDTH, 0, 500);	// We want to display Multiline Tooltip
	}
	return hWndTT;
}

void LinkInfo_SetStyle(COLORREF crNormal,COLORREF crHover,COLORREF crClicked,BOOL bUnderline)
{
	s_hlStyle.crNormal=crNormal;
	s_hlStyle.crHover=crHover;
	s_hlStyle.crClicked=crClicked;
	s_hlStyle.bUnderline=bUnderline;
}

void LinkInfo_Init(HWND hDlg,LINKDATA * pLinks,int cbSize)
{
	int i;
	char szBuf[MAX_PATH];
	DWORD dwStyle;


	s_HandCursor=LoadCursor(NULL, IDC_HAND);

	for(i=0;i<cbSize;i++)
	{
		HWND hLink=GetDlgItem(hDlg,pLinks[i].uCtrlID);
		if(hLink)
		{
			PLINKINFO pLinkInfo=(PLINKINFO)calloc(sizeof(LINKINFO),1);
			GetWindowText(hLink,szBuf,MAX_PATH);
			if(pLinks[i].szText[0])
				strcpy(pLinkInfo->szText,pLinks[i].szText);
			else
				strcpy(pLinkInfo->szText,szBuf);
			if(pLinks[i].szURL[0])
				strcpy(pLinkInfo->szURL,pLinks[i].szURL);
			else
				strcpy(pLinkInfo->szURL,szBuf);
			if(pLinks[i].szTip[0])
				pLinkInfo->hWndTT=LinkInfo_AddTip(hLink,pLinks[i].szTip);
			else
				pLinkInfo->hWndTT=LinkInfo_AddTip(hLink,szBuf);
			pLinkInfo->wpOrigProc=(WNDPROC)GetWindowLongPtr(hLink,GWLP_WNDPROC);
			SetWindowLongPtr(hLink,GWLP_USERDATA,(LONG_PTR)pLinkInfo);
			dwStyle=GetWindowLong(hLink,GWL_STYLE);
			dwStyle|=SS_NOTIFY;
			SetWindowLongPtr(hLink,GWL_STYLE,dwStyle);
			SetWindowLongPtr(hLink,GWLP_WNDPROC,(LONG_PTR)Link_WinProc);
		}
	}
}

