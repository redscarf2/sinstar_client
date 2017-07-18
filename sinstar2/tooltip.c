//输入法状态栏提示窗口,消息类型兼容Windows的tooptip控件
#include "sinstar2.h"
#include "tooltip.h"
#define  TIMER_RELAYSHOW	100
#define  TIMER_RELAYHIDE	200
#define  TIMER_TRACE		300

#define  TS_RELAYSHOW		500
#define  TS_RELAYHIDE		5*1000

#define  TT_BORDER			5		//边距		
#define  TT_LINEINTER		5		//行距

LRESULT WINAPI ToolTipWnd_Proc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static BOOL bTrace=FALSE;
	LRESULT lRet=0;
	switch(message)
	{
	case WM_CREATE:
		{
			HGLOBAL hData=GlobalAlloc(0,sizeof(TOOLTIP));
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			LOGFONT lf={0};
			lf.lfCharSet=DEFAULT_CHARSET;
			strcpy(lf.lfFaceName,"宋体");
			lf.lfHeight=12;
			lpData->hFont=CreateFontIndirect(&lf);
			lpData->szText[0]=0;
			GlobalUnlock(hData);
			SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)hData);
		}
		break;
	case WM_DESTROY:
		{
			HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			if(lpData)
			{
				DeleteObject(lpData->hFont);
				GlobalUnlock(hData);
				GlobalFree(hData);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(hWnd,&ps);
			HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			RECT rc;
			POINT pt={TT_BORDER,TT_BORDER};
			HGDIOBJ hOldFont=SelectObject(hdc,lpData->hFont);
			HBRUSH hBrush=CreateSolidBrush(GetSysColor(COLOR_INFOBK));
			HGDIOBJ hOldBr=SelectObject(hdc,hBrush);
			GetClientRect(hWnd,&rc);
			SetBkMode(hdc,TRANSPARENT);
			Rectangle(hdc,0,0,rc.right,rc.bottom);
			Helper_TextBox_Draw(hdc,lpData->szText,strlen(lpData->szText),pt,500,4,TT_LINEINTER);
			SelectObject(hdc,hOldBr);
			DeleteObject(hBrush);
			SelectObject(hdc,hOldFont);
			GlobalUnlock(hData);
			EndPaint(hWnd,&ps);
		}
		break;
	case TTM_RELAYEVENT:
		{
			switch(wParam)
			{
			case WM_LBUTTONDOWN:
			case WM_MOUSEMOVE:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				{
					HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
					LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);

					KillTimer(hWnd,TIMER_RELAYSHOW);
					ShowWindow(hWnd,SW_HIDE);
					lpData->uID=-1;
					GlobalUnlock(hData);
				}
				break;
			}
		}
		break;
	case TTM_ADDTOOL:
		{
			HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
			UINT uID=wParam;
			char *pszTxt=(char *)lParam;
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			if(uID!=lpData->uID)
			{
				KillTimer(hWnd,TIMER_RELAYSHOW);
				lpData->uID=uID;
				strcpy(lpData->szText,pszTxt);
				SetTimer(hWnd,TIMER_RELAYSHOW,TS_RELAYSHOW,NULL);
			}
			GlobalUnlock(hData);
		}
		break;
	case TTM_SETTOOLINFO:
		{
			char *pszText=(char*)lParam;
			int nLen=strlen(pszText);			
			if(nLen<1600)
			{
				HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
				LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
				HDC hdc=GetDC(hWnd);
				SIZE size;
				HGDIOBJ hOldFont=SelectObject(hdc,lpData->hFont);
				strcpy(lpData->szText,pszText);
				Helper_TextBox_CalcRect(hdc,lpData->szText,strlen(lpData->szText),&size,500,4,TT_LINEINTER);
				size.cx+=2*TT_BORDER;
				size.cy+=2*TT_BORDER;
								
				SetWindowPos(hWnd,HWND_TOPMOST,0,0,size.cx,size.cy,SWP_NOACTIVATE);
				Helper_CenterWindow(hWnd,SWP_NOACTIVATE);
				if(IsWindowVisible(hWnd))
				{
					KillTimer(hWnd,TIMER_RELAYHIDE);
					InvalidateRect(hWnd,NULL,TRUE);
				}else
				{
					ShowWindow(hWnd,SW_SHOWNOACTIVATE);
				}
				{
					RECT rc;
					POINT pt;
					GetWindowRect(hWnd,&rc);
					GetCursorPos(&pt);
					if(PtInRect(&rc,pt))
					{
						bTrace=TRUE;
						SetTimer(hWnd,TIMER_TRACE,100,NULL);
					}else
					{
						SetTimer(hWnd,TIMER_RELAYHIDE,TS_RELAYHIDE,NULL);
					}
				}
				SelectObject(hdc,hOldFont);
				ReleaseDC(hWnd,hdc);
				GlobalUnlock(hData);
			}
		}
		break;
	case WM_TIMER:
		{
			HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			if(wParam==TIMER_RELAYSHOW && lpData->szText[0])
			{
				HDC hdc=GetDC(hWnd);
				SIZE size;
				POINT pt;
				RECT  rcWorkArea;
				HGDIOBJ hOldFont=SelectObject(hdc,lpData->hFont);
				SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
				GetCursorPos(&pt);
				pt.x+=16,pt.y+=16;
				GetTextExtentPoint(hdc,lpData->szText,strlen(lpData->szText),&size);
				size.cx+=2*TT_BORDER;
				size.cy+=2*TT_BORDER;
				SelectObject(hdc,hOldFont);
				ReleaseDC(hWnd,hdc);
				if(pt.x+size.cx>rcWorkArea.right) pt.x=rcWorkArea.right-size.cx;
				SetWindowPos(hWnd,HWND_TOPMOST,pt.x,pt.y,size.cx,size.cy,SWP_NOACTIVATE);
				ShowWindow(hWnd,SW_SHOWNOACTIVATE);
				SetTimer(hWnd,TIMER_RELAYHIDE,TS_RELAYHIDE,NULL);
				KillTimer(hWnd,wParam);
			}else if(wParam==TIMER_RELAYHIDE)
			{
				ShowWindow(hWnd,SW_HIDE);
				KillTimer(hWnd,wParam);
			}else if(wParam==TIMER_TRACE)
			{
				RECT rcWnd;
				POINT pt;
				GetCursorPos(&pt);
				GetWindowRect(hWnd,&rcWnd);
				if(!PtInRect(&rcWnd,pt))
				{
					bTrace=FALSE;
					KillTimer(hWnd,TIMER_TRACE);
					ShowWindow(hWnd,SW_HIDE);
				}
			}
			GlobalUnlock(hData);
		}
		break;
	case WM_SETCURSOR:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(hWnd,&pt);
			SendMessage(hWnd,HIWORD(lParam),0,MAKELONG(pt.x,pt.y));
			SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
			if(!bTrace)
			{
				bTrace=TRUE;
				KillTimer(hWnd,TIMER_RELAYHIDE);
				SetTimer(hWnd,TIMER_TRACE,100,NULL);
			}
		}
		break;

	case WM_LBUTTONDOWN:
		ShowWindow(hWnd,SW_HIDE);
		break;
	case WM_RBUTTONDOWN:
		if(OpenClipboard(hWnd))
		{
			HGLOBAL hData=(HGLOBAL)GetWindowLongPtr(hWnd,GWLP_USERDATA);
			LPTOOLTIP lpData=(LPTOOLTIP)GlobalLock(hData);
			int nLen=strlen(lpData->szText);
			EmptyClipboard(); 
			if(nLen)
			{
				HGLOBAL hMem=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE ,nLen+1);
				LPSTR pMem=GlobalLock(hMem);
				strcpy(pMem,lpData->szText);
				GlobalUnlock(hMem);
				SetClipboardData(CF_TEXT,hMem);
			}
			GlobalUnlock(hData);
			CloseClipboard();
		}
		break;
	default:
		lRet=DefWindowProc(hWnd,message,wParam,lParam);
		break;
	}
	return lRet;
}

BOOL ToolTipWnd_Register(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
	
    //
    // register class of UI window.
    //
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc    = ToolTipWnd_Proc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon          = NULL;
    wc.lpszMenuName   = (LPTSTR)NULL;
    wc.lpszClassName  = CLSNAME_TOOLTIP;
    wc.hbrBackground  =  0;
    wc.hIconSm        = NULL;
	
    return RegisterClassEx( (LPWNDCLASSEX)&wc );
}

void ToolTipWnd_Unregister(HINSTANCE hInstance)
{
	UnregisterClass(CLSNAME_TOOLTIP,hInstance);
}