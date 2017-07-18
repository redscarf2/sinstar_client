//*************************************************************
//	ÌØÊâ×Ö·û´¦Àí
//*************************************************************
#include "sinstar2.h"
#include "uispchar.h"
#include "simbutton.h"


void   DrawSpecialChar(HDC hdc,int x,int y,char *pszText,BOOL bCheck)
{
	RECT rc;
	HBRUSH	hbrush;
	HFONT oldfont=(HFONT)SelectObject(hdc,g_SkinSpChar.hFont);
	if(bCheck) 
		hbrush=CreateSolidBrush(RGB(0,0,255));
	else
		hbrush=CreateSolidBrush(RGB(255,255,255));
	rc.left=2+x*20;
	rc.right=rc.left+20;
	rc.top=20+y*20;
	rc.bottom=rc.top+20;
	InflateRect(&rc,-1,-1);
	FillRect(hdc,&rc,hbrush);
	DeleteObject(hbrush);
	DrawText(hdc,pszText,pszText[0]<0?2:1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	SelectObject(hdc,oldfont);
}

void DrawSpecialCharBig(HDC hdc,char *pszText)
{
	RECT rc={230,90,280,140};
	HFONT oldfont=(HFONT)SelectObject(hdc,g_SkinSpChar.hFontBig);
	Rectangle(hdc,230,90,280,140);
	if(pszText)
	{
		int len=pszText[0]<0?2:1;
		DrawText(hdc,pszText,len,&rc,DT_CENTER|DT_SINGLELINE|DT_VCENTER);
	}
	SelectObject(hdc,oldfont);
}


LRESULT	WINAPI SpecialCharWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	POINT pt;
	if(Helper_IsIMEMessage(message)) return 0;

	switch(message)
	{
	case WM_CREATE:
		{
			g_SpCharData.nFrameWid=GetSystemMetrics(SM_CXDLGFRAME);
		}
		break;
	case WM_LBUTTONDBLCLK://
		{
			int x,y;
			LAYOUTINFO *pli=g_SpCharData.layout+g_SpCharData.nCurLayout;
			pt.x=(short)LOWORD(lParam);
			pt.y=(short)HIWORD(lParam);
			x=(pt.x-2)/20;
			y=(pt.y-20)/20;
			if(pt.x>2&&pt.y>20&&x<11&&y<6&&y*11+x<pli->nWords)//click word
			{
				char buf[3]={0};
				g_SpCharData.iCurSel=y*11+x;
				strncpy(buf,pli->szSpChar+g_SpCharData.iCurSel*2,2);
				InputString(buf,0);
			}
			return TRUE;
		}
		break;
	case WM_LBUTTONDOWN:
		pt.x=LOWORD(lParam);
		pt.y=HIWORD(lParam);
		if(!SimBtn_OnMouseDown(&g_SkinSpChar.wndBtn,hWnd,pt))
		{
			if(pt.y<SPDRAG_HEI)
			{
				SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam);
			}else
			{
				int x,y;
				LAYOUTINFO *pli=g_SpCharData.layout+g_SpCharData.nCurLayout;
				x=(pt.x-2)/20;
				y=(pt.y-20)/20;
				if(pt.x>2&&pt.y>20&&x<11&&y<6&&y*11+x<pli->nWords)//click word
				{
					HDC hdc=GetDC(hWnd);
					SetBkMode(hdc,TRANSPARENT);
					DrawSpecialChar(hdc,g_SpCharData.iCurSel%11,g_SpCharData.iCurSel/11,pli->szSpChar+g_SpCharData.iCurSel*2,FALSE);
					g_SpCharData.iCurSel=y*11+x;
					DrawSpecialChar(hdc,x,y,pli->szSpChar+g_SpCharData.iCurSel*2,TRUE);
					DrawSpecialCharBig(hdc,pli->szSpChar+g_SpCharData.iCurSel*2);
					ReleaseDC(hWnd,hdc);
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		pt.x=(short)LOWORD(lParam);
		pt.y=(short)HIWORD(lParam);
		if(GetWindowLongPtr(hWnd,GWLP_USERDATA))
		{
			RECT rc;
			HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
			HGLOBAL hPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
			LPUIPRIV lpUiPriv=(LPUIPRIV)GlobalLock(hPriv);
			GetWindowRect(hWnd,&rc);
			lpUiPriv->ptSpCharWnd.x=rc.left,lpUiPriv->ptSpCharWnd.y=rc.top;
			GlobalUnlock(hPriv);
			SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
			PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
		}else
		{
			SimBtn_OnMouseUp(&g_SkinSpChar.wndBtn,hWnd,pt);
		}
		break;
	case WM_RBUTTONUP: 
		SendMessage(hWnd,WM_COMMAND,MAKELONG(IDC_EXIT,BN_CLICKED),0);
		break;
	case WM_MOUSEMOVE:
		{
			LONG  lPrev=GetWindowLongPtr(hWnd,GWLP_USERDATA);
			POINT ptPrev={(short)LOWORD(lPrev),(short)HIWORD(lPrev)};
			RECT rc;
			pt.x=(short)LOWORD(lParam);
			pt.y=(short)HIWORD(lParam);
			GetClientRect(hWnd,&rc);
			if(lPrev || !SimBtn_OnMouseMove(&g_SkinSpChar.wndBtn,hWnd,pt,PtInRect(&rc,pt)))
			{
				if(pt.y<SPDRAG_HEI)//ÍÏ¶¯Ìõ
					SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL)));
				else
					SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
				if(lPrev)
				{
					ClientToScreen(hWnd,&pt);
					SetWindowPos(hWnd,NULL,pt.x-ptPrev.x,pt.y-ptPrev.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
				}
				if(!PtInRect(&rc,pt) && lPrev==0)
				{
					SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
					ReleaseCapture();
				}
			}else
			{
				SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
			}
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			switch(HIWORD(wParam))
			{
			case BN_CLICKED:
				switch(LOWORD(wParam)) 
				{
				case IDC_EXIT:
					ReleaseCapture();
					ShowWindow(hWnd,SW_HIDE);
					break;
				case IDC_INSERT:
					if(g_SpCharData.iCurSel!=-1)
					{
						char buf[3]={0};
						LAYOUTINFO *pli=g_SpCharData.layout+g_SpCharData.nCurLayout;
						strncpy(buf,pli->szSpChar+g_SpCharData.iCurSel*2,2);
						InputString(buf,0);
					}
					break;
				case IDC_NEXT:
					SendMessage(hWnd,UM_SETLAYOUT,g_SpCharData.nCurLayout+1,0);
					break;
				case IDC_PREV:
					SendMessage(hWnd,UM_SETLAYOUT,g_SpCharData.nCurLayout-1,0);
					break;
				}
				break;
			}
		}
		return 0;
	case WM_SETCURSOR:
		{
			SetCapture(hWnd);
		}
		return TRUE;
	case UM_SETLAYOUT:
		{
			int nIndex=(int)wParam;
			if(nIndex==-1) nIndex=g_SpCharData.nLayoutCount-1;
			else if(nIndex==g_SpCharData.nLayoutCount) nIndex=0;
			g_SpCharData.nCurLayout=nIndex;
			g_SpCharData.iCurSel=-1;
			InvalidateRect(hWnd,NULL,TRUE);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT psstru;
			HDC hdc=BeginPaint(hWnd,&psstru);
			RECT	headrc={0,0,SPCHAR_WID,SPDRAG_HEI};
			char	title[100];
			int		i;
			LAYOUTINFO *pli=g_SpCharData.layout+g_SpCharData.nCurLayout;
			HFONT oldfont=(HFONT)SelectObject(hdc,g_SkinSpChar.hFont);
			Helper_DrawBMP2(hdc,headrc,g_SkinSpChar.hBmpHeader,0,0);
			SetBkMode(hdc,TRANSPARENT);
			sprintf(title,"·ûºÅ¼üÅÌ:%s",pli->szName);
			DrawText(hdc,title,strlen(title),&headrc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			//draw special char table
			for(i=0;i<12;i++)
			{
				MoveToEx(hdc,i*20+2,20,NULL);
				LineTo(hdc,i*20+2,140);
			}
			for(i=0;i<7;i++)
			{
				MoveToEx(hdc,2,i*20+20,NULL);
				LineTo(hdc,222,i*20+20);
			}
			
			i=0;
			for(i=0;i<pli->nWords;i++)
			{
				DrawSpecialChar(hdc,i%11,i/11,pli->szSpChar+i*2,i==g_SpCharData.iCurSel);
			}

			if(g_SpCharData.iCurSel!=-1)
				DrawSpecialCharBig(hdc,pli->szSpChar+g_SpCharData.iCurSel*2);
			else
				DrawSpecialCharBig(hdc,NULL);
			SimBtn_OnPaint(&g_SkinSpChar.wndBtn,hdc);
			SelectObject(hdc,oldfont);
			EndPaint(hWnd,&psstru);
		}
		return 0;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}