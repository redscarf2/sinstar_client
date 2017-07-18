#include "sinstar2.h"
#include "udictinput.h"

HWND g_hWndUDict=0;
static char g_szUserDict[1000];

static HHOOK g_hhk;

LRESULT CALLBACK UserDict_GetMsgProc(
  int nCode,      // hook code
  WPARAM wParam,  // current-process flag
  LPARAM lParam   // address of structure with message data
)
{
	if(nCode==HC_ACTION)
	{
		MSG *pmsg=(MSG*)lParam;
		if(pmsg->message==WM_KEYDOWN && pmsg->hwnd==GetDlgItem(g_hWndUDict,IDC_EDIT_KEY))
		{
			if(pmsg->wParam==VK_UP||pmsg->wParam==VK_DOWN)
			{
				pmsg->hwnd=GetDlgItem(g_hWndUDict,IDC_LIST_KEY);
			}
		}else if((pmsg->message==WM_CHAR || pmsg->message==WM_IME_CHAR) && pmsg->hwnd==GetDlgItem(g_hWndUDict,IDC_LIST_KEY))
		{
			HWND hEdit=GetDlgItem(g_hWndUDict,IDC_EDIT_KEY);
			SetFocus(hEdit);
			SendMessage(hEdit,EM_SETSEL,-1,0);
			pmsg->hwnd=hEdit;
		}
	}
	return CallNextHookEx(g_hhk,nCode,wParam,lParam);
}

INT_PTR CALLBACK UserDict_DlgProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	BOOL bRet=FALSE;
	static BOOL bListSet=FALSE;
	static BOOL bEditChange=FALSE;
	static HBRUSH hbr_normal=0;
	static HBRUSH hbr_select=0;
	if(!hbr_normal) hbr_normal=CreateSolidBrush(RGB(255,255,255));
	if(!hbr_select) hbr_select=CreateSolidBrush(RGB(0,0,255));
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTitle1[200],szTitle2[200];
			RECT rcList;
			LVCOLUMN lvCol={LVCF_WIDTH|LVCF_FMT,LVCFMT_LEFT,0};
			HWND hList=GetDlgItem(hwndDlg,IDC_LIST_KEY);

			GetWindowText(hwndDlg,szTitle1,200);
			sprintf(szTitle2,szTitle1,ISComm_GetUserDict());
			SetWindowText(hwndDlg,szTitle2);

			ListView_SetItemCount(hList,lParam);
			GetClientRect(hList,&rcList);
			lvCol.cx=rcList.right;
			ListView_InsertColumn(hList,0,&lvCol);
			Helper_CenterWindow(hwndDlg,0);
			g_hWndUDict=hwndDlg;
			g_hhk=SetWindowsHookEx(WH_GETMESSAGE,UserDict_GetMsgProc,g_hInst,GetCurrentThreadId());
		}
		break;
	case WM_DESTROY:
		DeleteObject(hbr_normal);
		hbr_normal=0;
		DeleteObject(hbr_select);
		hbr_select=0;
		g_hWndUDict=0;
		UnhookWindowsHookEx(g_hhk);
		break;
	case UM_UDICT_CHANGED:
		MessageBox(hwndDlg,"服务器用户词典更改，本窗口将自动关闭","提示",MB_OK|MB_ICONWARNING);
		EndDialog(hwndDlg,IDCANCEL);
		break;
	case WM_COMMAND:
		{
			if(HIWORD(wParam)==BN_CLICKED)
			{
				switch(LOWORD(wParam))
				{
				case IDOK:
					GetDlgItemText(hwndDlg,IDC_CONVERT,g_szUserDict,1000);
					EndDialog(hwndDlg,IDOK);
					break;
				case IDCANCEL:
					EndDialog(hwndDlg,IDCANCEL);
					break;
				case IDC_COPY:
					if(OpenClipboard(hwndDlg)){
						HGLOBAL hMem=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,1000);
						LPSTR pszBuf=(LPSTR)GlobalLock(hMem);
						GetDlgItemText(hwndDlg,IDC_CONVERT,pszBuf,1000);
						GlobalUnlock(hMem);
						EmptyClipboard(); 
						SetClipboardData(CF_TEXT,hMem);
						CloseClipboard();
					}
					break;
				}
			}else if(HIWORD(wParam)==EN_CHANGE)
			{
				if(!bListSet && LOWORD(wParam)==IDC_EDIT_KEY)
				{//不是由于用户选择列表中的项导致的窗口数据变化
					char szText[40];
					GetDlgItemText(hwndDlg,IDC_EDIT_KEY,szText,40);
					if(ISComm_UserDict_Locate(szText,strlen(szText))==ISACK_SUCCESS)
					{
						PMSGDATA pData=ISComm_GetData();
						DWORD dwIndex=-1;
						memcpy(&dwIndex,pData->byData,4);
						if(dwIndex!=-1)
						{
							HWND hList=GetDlgItem(hwndDlg,IDC_LIST_KEY);
							int nTopItem=ListView_GetTopIndex(hList);
							int nPageItems=ListView_GetCountPerPage(hList);

							bEditChange=TRUE;
							ListView_SetItemState(hList,dwIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
							ListView_SetSelectionMark(hList,dwIndex);
							if(nTopItem>(int)dwIndex)
							{
								ListView_EnsureVisible(hList,dwIndex,FALSE);
							}else if( nTopItem+nPageItems-1<(int)dwIndex)
							{
								int nEndItem=dwIndex+nPageItems-1;
								if(nEndItem<0) nEndItem=0;
								if(nEndItem>ListView_GetItemCount(hList)-1) nEndItem=ListView_GetItemCount(hList)-1;
								ListView_EnsureVisible(hList,nEndItem,FALSE);
							}
							bEditChange=FALSE;
						}else
						{
							SoundPlay("error");
						}
					}
				}
			}
		}
		break;
	case WM_NOTIFY:
		if(wParam)
		{
			NMHDR* pNMHDR = (NMHDR*)lParam;
			switch(pNMHDR->code)
			{
			case LVN_GETDISPINFO:
				{
					LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
					LV_ITEM* pItem= &(pDispInfo)->item;
					if (pItem->mask & LVIF_TEXT)
					{
						if(ISComm_UserDict_Item(pItem->iItem)==ISACK_SUCCESS)
						{
							PMSGDATA pData=ISComm_GetData();
							lstrcpyn(pItem->pszText,(char*)pData->byData,pItem->cchTextMax);
						}else
						{
							strcpy(pItem->pszText,"error");
						}
					}
				}
				break;
			case LVN_ITEMCHANGED:
				{
					NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
					if(pNMListView->iItem!=-1 && pNMListView->uOldState==0 && pNMListView->uChanged&LVIF_STATE)
					{
						char szText[40];
						ListView_GetItemText(pNMListView->hdr.hwndFrom,pNMListView->iItem,0,szText,40);
						if(ISComm_QueryUserDict(szText,(char)strlen(szText))==ISACK_SUCCESS)
						{
							PMSGDATA pData=ISComm_GetData();
							char *pbuf=(char*)pData->byData;
							if(strcmp(szText,pbuf)==0)
							{
								pbuf+=strlen(pbuf)+1;
								SetDlgItemText(hwndDlg,IDC_CONVERT,pbuf);
							}else
							{
								SetDlgItemText(hwndDlg,IDC_CONVERT,"error");
							}
						}
					}
				}
				break;
			case NM_DBLCLK:
				if(pNMHDR->idFrom==IDC_LIST_KEY)
				{//模拟回车键
					PostMessage(hwndDlg,WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),0);
				}
				break;
			}
		}
		break;
	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
			lpmis->itemHeight=20;
		}
		break;
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
			SetBkMode(lpdis->hDC,TRANSPARENT);
			if(lpdis->itemState & ODS_SELECTED)
			{
				FillRect(lpdis->hDC,&lpdis->rcItem,hbr_select);
				if(GetFocus()==lpdis->hwndItem) 
					SetTextColor(lpdis->hDC,RGB(255,0,0));
				else
					SetTextColor(lpdis->hDC,RGB(255,255,255));
			}else
			{
				FillRect(lpdis->hDC,&lpdis->rcItem,hbr_normal);
				SetTextColor(lpdis->hDC,RGB(0,0,0));
			}
			if(lpdis->itemID!=-1)
			{
				char szText[40];
				ListView_GetItemText(lpdis->hwndItem,lpdis->itemID,0,szText,40);
				DrawText(lpdis->hDC,szText,-1,&lpdis->rcItem,DT_SINGLELINE|DT_LEFT|DT_VCENTER);
			}
		}
		break;
	}
	return bRet;
}

BOOL UserDict_Input(HWND hWnd)
{
	if(g_hWndUDict) return FALSE;
	if(ISComm_UserDict_Max()==ISACK_SUCCESS)
	{
		DWORD dwItems=0;
		PMSGDATA pMsg=ISComm_GetData();
		memcpy(&dwItems,pMsg->byData,4);
		g_szUserDict[0]=0;
		if(DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_UDICT_INPUT),GetActiveWindow(),UserDict_DlgProc,dwItems)==IDOK)
		{
			if(g_szUserDict[0])
			{
				InputString(g_szUserDict,hWnd);
			}
		}
	}
	return TRUE;
}