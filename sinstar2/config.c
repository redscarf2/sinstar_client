#include "sinstar2.h"
#include <commdlg.h>
#include <shellapi.h>

#include "..\include\hyperlink.h"

static BOOL s_bFirstInit=TRUE;

BOOL WINAPI PropPage_Proc_Default(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	switch(message)
	{
	case WM_INITDIALOG:
		{
			if(s_bFirstInit)
			{
				RECT rcOwner,rcProp;
				HWND hPropSheet=GetParent(hWnd);
				HWND hPropOwner=GetWindow(hPropSheet,GW_OWNER);
				GetWindowRect(hPropOwner,&rcOwner);
				GetWindowRect(hPropSheet,&rcProp);
				SetWindowPos(hPropSheet,NULL,
					rcOwner.left+(rcOwner.right-rcOwner.left-rcProp.right+rcProp.left)/2,
					rcOwner.top+(rcOwner.bottom-rcOwner.top-rcProp.bottom+rcProp.top)/2,
					0,0,SWP_NOZORDER|SWP_NOSIZE);
				s_bFirstInit=FALSE;
			}
			return TRUE;
		}
		break;
	case WM_COMMAND:
		PropSheet_Changed(GetParent(hWnd),hWnd);
		break;
	case WM_NOTIFY:
		switch(((LPNMHDR)lParam)->code)
		{
		case PSN_HELP:
			{
				TCITEM tcItem;
				char szTabName[50];
				char szPath[MAX_PATH];
				HWND hTabWnd=PropSheet_GetTabControl(GetParent(hWnd));
				int iPage=TabCtrl_GetCurSel(hTabWnd);
				tcItem.mask=TCIF_TEXT;
				tcItem.pszText=szTabName;
				tcItem.cchTextMax=50;
				TabCtrl_GetItem(hTabWnd,iPage,&tcItem);
				sprintf(szPath,"%s\\%s.chm::/使用方法/系统配置.htm#%s",g_szDataPath,g_szImeTitle,szTabName);
				HtmlHelp(NULL,szPath,HH_DISPLAY_TOPIC,0);
				break;
			}
		}
		break;
	}
	return FALSE;
}

INT_PTR WINAPI PropPage_Proc_Other(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	static HBRUSH hBr=NULL;
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			int i;
			for(i=0;i<6;i++)
			{
				char szBuf[2]={g_SettingsG.byLineKey[i],0};
				SetDlgItemText(hWnd,IDC_LINEKEY1+i,szBuf);
				ImmAssociateContext(GetDlgItem(hWnd,IDC_LINEKEY1+i),(HIMC)NULL);//禁止在此显示输入法
			}
			SetDlgItemText(hWnd,IDC_PHONTIC_LEFT,g_SettingsG.szPhonticLeft);
			SetDlgItemText(hWnd,IDC_PHONTIC_RIGHT,g_SettingsG.szPhonticRight);
			CheckDlgButton(hWnd,IDC_SHOWTIP,g_SettingsG.bShowOpTip);//操作提示
			CheckDlgButton(hWnd,IDC_TTS_INPUT,g_SettingsG.bTTSInput);
			CheckRadioButton(hWnd,IDC_SOUND_DISABLE,IDC_SOUND_BEEP,g_SettingsG.nSoundAlert+IDC_SOUND_DISABLE);
			CheckDlgButton(hWnd,IDC_DELWORD_DISABLE,g_SettingsG.bDisableDelWordCand);
			SetWindowLongPtr(hWnd,GWLP_USERDATA,RGB(g_SettingsG.byPntcRed,g_SettingsG.byPntcGreen,g_SettingsG.byPntcBlue));
		}
		break;
	case WM_DESTROY:
		if(hBr) DeleteObject(hBr);
		hBr=NULL;
		break;
	case WM_CTLCOLORSTATIC:
		{
			HWND hCtrl=(HWND)lParam;
			if(GetDlgCtrlID(hCtrl)==IDC_CR_PNTC)
			{
				DWORD crPhontic=GetWindowLongPtr(hWnd,GWLP_USERDATA);
				if(hBr) DeleteObject(hBr);
				hBr=CreateSolidBrush(crPhontic);
				bRet=(int)hBr;
			}
			break;
		}
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_BTN_PHONTICCOLOR:
				{
					static COLORREF acrCustClr[16]; 
					CHOOSECOLOR cc={0};
					cc.lStructSize=sizeof(cc);
					cc.hwndOwner=hWnd;
					cc.Flags=CC_RGBINIT;
					cc.lpCustColors = (LPDWORD) acrCustClr;
					cc.rgbResult=RGB(g_SettingsG.byPntcRed,g_SettingsG.byPntcGreen,g_SettingsG.byPntcBlue);
					if(ChooseColor(&cc));
					{
						SetWindowLongPtr(hWnd,GWLP_USERDATA,cc.rgbResult);
						InvalidateRect(GetDlgItem(hWnd,IDC_CR_PNTC),NULL,TRUE);
					}
				}
				break;
			}
		}else if(HIWORD(wParam)==EN_SETFOCUS)
		{
			UINT uID=LOWORD(wParam);
			if(uID>=IDC_LINEKEY1 && uID<=IDC_LINEKEY6)
			{
				PostMessage((HWND)lParam,EM_SETSEL,0,-1);
			}
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			{
				int i;
				DWORD crPhontic=GetWindowLongPtr(hWnd,GWLP_USERDATA);
				g_SettingsG.bShowOpTip=IsDlgButtonChecked(hWnd,IDC_SHOWTIP);
				g_SettingsG.bTTSInput=IsDlgButtonChecked(hWnd,IDC_TTS_INPUT);
				if(IsDlgButtonChecked(hWnd,IDC_SOUND_DISABLE)) g_SettingsG.nSoundAlert=0;
				if(IsDlgButtonChecked(hWnd,IDC_SOUND_WAV)) g_SettingsG.nSoundAlert=1;
				if(IsDlgButtonChecked(hWnd,IDC_SOUND_BEEP)) g_SettingsG.nSoundAlert=2;
				g_SettingsG.bDisableDelWordCand=IsDlgButtonChecked(hWnd,IDC_DELWORD_DISABLE);
				GetDlgItemText(hWnd,IDC_PHONTIC_LEFT,g_SettingsG.szPhonticLeft,50-1);
				GetDlgItemText(hWnd,IDC_PHONTIC_RIGHT,g_SettingsG.szPhonticRight,50-1);
				for(i=0;i<6;i++)
				{
					char szBuf[2];
					GetDlgItemText(hWnd,IDC_LINEKEY1+i,szBuf,2);
					g_SettingsG.byLineKey[i]=szBuf[0];
				}
				g_SettingsG.byPntcRed=GetRValue(crPhontic);
				g_SettingsG.byPntcGreen=GetGValue(crPhontic);
				g_SettingsG.byPntcBlue=GetBValue(crPhontic);
			}
			break;
		}
	}
	return bRet;
}

INT_PTR WINAPI PropPage_Proc_WndEffect(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	static HBRUSH hBr=NULL;
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			HWND hWndSpin;
			//transparent degree
			hWndSpin=GetDlgItem(hWnd,IDC_SPIN_TRANS_STATUS);
			SendMessage(hWndSpin,UDM_SETBUDDY,(WPARAM)GetDlgItem(hWnd,IDC_TRANS_STATUS),0);
			SendMessage(hWndSpin,UDM_SETRANGE,FALSE,MAKELONG(100,50));
			SendMessage(hWndSpin,UDM_SETPOS,0,MAKELONG(g_SettingsG.byTransStatus,0));
			hWndSpin=GetDlgItem(hWnd,IDC_SPIN_TRANS_INPUT);
			SendMessage(hWndSpin,UDM_SETBUDDY,(WPARAM)GetDlgItem(hWnd,IDC_TRANS_INPUT),0);
			SendMessage(hWndSpin,UDM_SETRANGE,FALSE,MAKELONG(100,50));
			SendMessage(hWndSpin,UDM_SETPOS,0,MAKELONG(g_SettingsG.byTransInput,0));
			//fade
			SetDlgItemInt(hWnd,IDC_FADE_STEPS,g_SettingsG.byFadeSteps,FALSE);
			ImmAssociateContext(GetDlgItem(hWnd,IDC_FADE_STEPS),(HIMC)NULL);
			//shade
			CheckDlgButton(hWnd,IDC_SHADE_INPUT,g_SettingsG.bShadeInput);
			CheckDlgButton(hWnd,IDC_SHADE_STATUS,g_SettingsG.bShadeStatus);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_FADE_STEP_MENU)
		{
			RECT rc;
			UINT uRet=0;
			HMENU hMenu=LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_FADESTEPS));
			HWND hMenuBtn=GetDlgItem(hWnd,IDC_FADE_STEP_MENU);
			BYTE bySteps=0;
			GetWindowRect(hMenuBtn,&rc);
			uRet=TrackPopupMenu(GetSubMenu(hMenu,0),TPM_RIGHTALIGN|TPM_TOPALIGN|TPM_RETURNCMD,rc.right,rc.bottom,0,hWnd,NULL);
			switch(uRet)
			{
			case IDM_FS_NOFADE:bySteps=0;break;
			case IDM_FS_FAST:bySteps=5;break;
			case IDM_FS_MIDDLE:bySteps=10;break;
			case IDM_FS_SLOW:bySteps=15;break;
			case IDM_FS_SLOWEST:bySteps=20;break;
			default:bySteps=-1;break;
			}
			if(bySteps!=(BYTE)-1) SetDlgItemInt(hWnd,IDC_FADE_STEPS,bySteps,FALSE);
			DestroyMenu(hMenu);
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			{
				//hemitransparent
				g_SettingsG.byTransStatus=(BYTE)GetDlgItemInt(hWnd,IDC_TRANS_STATUS,NULL,FALSE);
				g_SettingsG.byTransInput=(BYTE)GetDlgItemInt(hWnd,IDC_TRANS_INPUT,NULL,FALSE);
				//fade
				g_SettingsG.byFadeSteps=GetDlgItemInt(hWnd,IDC_FADE_STEPS,NULL,FALSE);
				if(g_SettingsG.byFadeSteps>20) g_SettingsG.byFadeSteps=20;
				//shade
				g_SettingsG.bShadeInput=IsDlgButtonChecked(hWnd,IDC_SHADE_INPUT);
				g_SettingsG.bShadeStatus=IsDlgButtonChecked(hWnd,IDC_SHADE_STATUS);
			}
			break;
		}
	}
	return bRet;
}

INT_PTR WINAPI PropPage_Proc_About(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	if(message==WM_INITDIALOG)
	{
		char szBuf[50],szImePath[MAX_PATH];
		DWORD dwVer;
		LINKDATA url[]={{IDC_HOMESITE,0},{IDC_EMAIL,0}};
	
		LinkInfo_Init(hWnd,url,2);

		if(ISComm_ServerVersion()==ISACK_SUCCESS)
		{
			PMSGDATA pData=ISComm_GetData();
			memcpy(&dwVer,pData->byData,4);
			Helper_VersionString(dwVer,szBuf);
			SetDlgItemText(hWnd,IDC_VER_SVR,szBuf);
		}
		GetModuleFileName(g_hInst,szImePath,MAX_PATH);
		Helper_PEVersion(szImePath,&dwVer,NULL,NULL);
		Helper_VersionString(dwVer,szBuf);
		SetDlgItemText(hWnd,IDC_VER_IME,szBuf);
	}
	return PropPage_Proc_Default(hWnd,message,wParam,lParam);
}


INT_PTR WINAPI PropPage_Proc_Plugin(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	if(message==WM_INITDIALOG)
	{
		char szFilter[MAX_PATH];
		WIN32_FIND_DATA wfd;
		HANDLE hFindFile;
		HWND hListPlugin=GetDlgItem(hWnd,IDC_LIST_PLUGIN);
		
		SendMessage(hListPlugin,CB_ADDSTRING,0,(LPARAM)NOPLUGIN);
		
		sprintf(szFilter,"%s\\plugin\\*.dll",g_szDataPath);
		hFindFile=FindFirstFile(szFilter,&wfd);
		if(hFindFile!=INVALID_HANDLE_VALUE)
		{
			do{
				SendMessage(hListPlugin,CB_ADDSTRING,0,(LPARAM)wfd.cFileName);
			}while(FindNextFile(hFindFile,&wfd));
			FindClose(hFindFile);
		}
		if(g_SettingsG.szPlugin[0])
		{
			SendMessage(hListPlugin,CB_SELECTSTRING,0,(LPARAM)g_SettingsG.szPlugin);
			SendMessage(hWnd,WM_COMMAND,MAKELONG(IDC_LIST_PLUGIN,CBN_SELCHANGE),(LPARAM)hListPlugin);
		}else
		{
			SendMessage(hListPlugin,CB_SETCURSEL,0,0);
		}
	}else if(message==WM_COMMAND && HIWORD(wParam)==CBN_SELCHANGE)
	{
		char szPlugin[50]={0},szPath[MAX_PATH],szBuf[50],szName[100],szDesc[200];
		DWORD dwVer;
		GetWindowText((HWND)lParam,szPlugin,50);
		sprintf(szPath,"%s\\plugin\\%s",g_szDataPath,szPlugin);
		if(Helper_PEVersion(szPath,&dwVer,szName,szDesc))
		{
			Helper_VersionString(dwVer,szBuf);
			SetDlgItemText(hWnd,IDC_VERSION,szBuf);
			SetDlgItemText(hWnd,IDC_NAME,szName);
			SetDlgItemText(hWnd,IDC_DESCRIPT,szDesc);
		}else
		{
			SetDlgItemText(hWnd,IDC_VERSION,"");
			SetDlgItemText(hWnd,IDC_NAME,"");
			SetDlgItemText(hWnd,IDC_DESCRIPT,"");
		}
	}else if(message==WM_NOTIFY && ((LPNMHDR)lParam)->code==PSN_APPLY)
	{
		GetDlgItemText(hWnd,IDC_LIST_PLUGIN,g_SettingsG.szPlugin,50);
	}

	return PropPage_Proc_Default(hWnd,message,wParam,lParam);
}

INT_PTR WINAPI PropPage_Proc_Custom(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			char szBuf[50]={0};
			HWND hWndTpKey=GetDlgItem(hWnd,IDC_TP_KEY);
			int  iTpMode=2;
			switch(g_SettingsG.byTempSpellKey)
			{
			case 0xc0:iTpMode=0;break;
			case 0xc1:iTpMode=1;break;
			default:g_SettingsG.byTempSpellKey=0;break;
			}
			if(g_SettingsG.bySwitchKey==0x36)
				CheckDlgButton(hWnd,IDC_SW_RSHIFT,TRUE);
			else if(g_SettingsG.bySwitchKey==0x2a)
				CheckDlgButton(hWnd,IDC_SW_LSHIFT,TRUE);
			else
				CheckDlgButton(hWnd,IDC_SW_DISABLE,TRUE);
			CheckRadioButton(hWnd,IDC_ENTER_CLEAR,IDC_ENTER_INPUT,g_SettingsG.bEnterClear?IDC_ENTER_CLEAR:IDC_ENTER_INPUT);

			CheckDlgButton(hWnd,IDC_FAST_UMODE,g_SettingsG.bFastUMode);
			CheckDlgButton(hWnd,IDC_AUTO_INPUT,g_SettingsG.bAutoInput);
			CheckDlgButton(hWnd,IDC_PYCAND_PHRASEFIRST,g_SettingsG.bPYPhraseFirst);
			SendMessage(hWndTpKey,CB_ADDSTRING,0,(LPARAM)"左Ctrl");
			SendMessage(hWndTpKey,CB_ADDSTRING,0,(LPARAM)"右Ctrl");
			SendMessage(hWndTpKey,CB_ADDSTRING,0,(LPARAM)"禁用");
			SendMessage(hWndTpKey,CB_SETCURSEL,iTpMode,0);
			ImmAssociateContext(hWndTpKey,(HIMC)NULL);
#ifndef SOWB
			CheckDlgButton(hWnd,IDC_DISABLEFNKEY,g_SettingsG.bDisableFnKey);
			sprintf(szBuf,"禁止辅助功能键:%s",g_SettingsG.byTempSpellKey==0xC1?"左Ctrl":"右Ctrl");
			SetDlgItemText(hWnd,IDC_DISABLEFNKEY,szBuf);
#endif//SOWB				

			szBuf[0]=g_cHotKeyUD;
			szBuf[1]=0;
			SetDlgItemText(hWnd,IDC_FASTKEY_UMODE,szBuf);
			ImmAssociateContext(GetDlgItem(hWnd,IDC_FASTKEY_UMODE),(HIMC)NULL);

			szBuf[0]=g_cHotKeySent;
			szBuf[1]=0;
			SetDlgItemText(hWnd,IDC_FASTKEY_SENTENCE,szBuf);
			ImmAssociateContext(GetDlgItem(hWnd,IDC_FASTKEY_SENTENCE),(HIMC)NULL);

			if(!g_SettingsG.bFastUMode) EnableWindow(GetDlgItem(hWnd,IDC_FASTKEY_UMODE),FALSE);

		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==CBN_SELCHANGE)
		{
#ifndef SOWB
			int iSel=SendMessage((HWND)lParam,CB_GETCURSEL,0,0);
			switch(iSel)
			{
			case 0:case 2:
				SetDlgItemText(hWnd,IDC_DISABLEFNKEY,"禁止辅助功能键:右Ctrl");
				break;
			case 1:
				SetDlgItemText(hWnd,IDC_DISABLEFNKEY,"禁止辅助功能键:左Ctrl");
				break;
			}
#endif//SOWB
		}else if(HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_FAST_UMODE)
		{
			EnableWindow(GetDlgItem(hWnd,IDC_FASTKEY_UMODE),IsDlgButtonChecked(hWnd,IDC_FAST_UMODE));
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			{
				char szBuf[2]={0};
				g_SettingsG.bFastUMode=IsDlgButtonChecked(hWnd,IDC_FAST_UMODE);
				g_SettingsG.bEnterClear=IsDlgButtonChecked(hWnd,IDC_ENTER_CLEAR);
				g_SettingsG.bAutoInput=IsDlgButtonChecked(hWnd,IDC_AUTO_INPUT);
				g_SettingsG.bPYPhraseFirst=IsDlgButtonChecked(hWnd,IDC_PYCAND_PHRASEFIRST);
				g_SettingsG.bDisableFnKey=IsDlgButtonChecked(hWnd,IDC_DISABLEFNKEY);
				GetDlgItemText(hWnd,IDC_FASTKEY_UMODE,szBuf,2);
				g_cHotKeyUD=szBuf[0];
				GetDlgItemText(hWnd,IDC_FASTKEY_SENTENCE,szBuf,2);
				g_cHotKeySent=szBuf[0];
				if(IsDlgButtonChecked(hWnd,IDC_SW_RSHIFT))
					g_SettingsG.bySwitchKey=0x36;
				else if(IsDlgButtonChecked(hWnd,IDC_SW_LSHIFT))
					g_SettingsG.bySwitchKey=0x2a;
				else
					g_SettingsG.bySwitchKey=0;
				switch(SendMessage(GetDlgItem(hWnd,IDC_TP_KEY),CB_GETCURSEL,0,0))
				{
				case 0:g_SettingsG.byTempSpellKey=0xC0;break;
				case 1:g_SettingsG.byTempSpellKey=0xC1;break;
				default:g_SettingsG.byTempSpellKey=0;break;
				}
			}
			break;
		}
	}
	return bRet;
}

//将VK转换成HotKey
WORD VK2HotKey(BYTE byVK)
{
	if(byVK>=0x30) return (WORD)byVK;
	else return MAKEWORD(byVK,0x08);
}

INT_PTR WINAPI PropPage_Proc_HotKey(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			HWND hHotKey;
			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_SWITCH_MODE);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyMode),0);

			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_SWITCH_EN);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyEn),0);
			
			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_QUERY);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyQuery),0);
			
			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_MAKEWORD);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyMakeWord),0);
			
			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_SHOWROOT);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyShowRoot),0);

			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_HIDESTATUS);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byHotKeyHideStatus),0);
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			g_SettingsG.byHotKeyMode=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_SWITCH_MODE),HKM_GETHOTKEY,0,0);
			g_SettingsG.byHotKeyEn=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_SWITCH_EN),HKM_GETHOTKEY,0,0);
			g_SettingsG.byHotKeyQuery=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_QUERY),HKM_GETHOTKEY,0,0);
			g_SettingsG.byHotKeyMakeWord=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_MAKEWORD),HKM_GETHOTKEY,0,0);
			g_SettingsG.byHotKeyShowRoot=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_SHOWROOT),HKM_GETHOTKEY,0,0);
			g_SettingsG.byHotKeyHideStatus=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_HIDESTATUS),HKM_GETHOTKEY,0,0);
			break;
		}
	}
	return bRet;
}



INT_PTR WINAPI PropPage_Proc_CandMode(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			HWND hHotKey;
			HWND hWndSpin=GetDlgItem(hWnd,IDC_SPIN_PAGECANDS);
			CheckDlgButton(hWnd,IDC_23CANDKEY,g_SettingsG.b23CandKey);

			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_2CAND);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.by2CandVK),0);
			if(!g_SettingsG.b23CandKey) EnableWindow(hHotKey,FALSE);
			
			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_3CAND);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.by3CandVK),0);
			if(!g_SettingsG.b23CandKey) EnableWindow(hHotKey,FALSE);
			if(!g_SettingsG.b23CandKey) EnableWindow(GetDlgItem(hWnd,IDC_CANDSEL_NONUM),FALSE);

			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_CAND_PAGEUP);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byTurnPageUpVK),0);

			hHotKey=GetDlgItem(hWnd,IDC_HOTKEY_CAND_PAGEDOWN);
			ImmAssociateContext(hHotKey,(HIMC)NULL);
			SendMessage(hHotKey,HKM_SETHOTKEY,VK2HotKey(g_SettingsG.byTurnPageDownVK),0);

			SendMessage(hWndSpin,UDM_SETBUDDY,(WPARAM)GetDlgItem(hWnd,IDC_PAGECANDS),0);
			SendMessage(hWndSpin,UDM_SETRANGE,FALSE,MAKELONG(10,3));
			SendMessage(hWndSpin,UDM_SETPOS,1,g_SettingsG.cPageCands);

			CheckDlgButton(hWnd,IDC_CANDSEL_NONUM,g_SettingsG.bCandSelNoNum);
			CheckDlgButton(hWnd,IDC_ONLYSIMPLECODE,g_SettingsG.bOnlySimpleCode);
			
			CheckRadioButton(hWnd,IDC_GBK_HIDE,IDC_GBK_SHOWA,IDC_GBK_HIDE+g_SettingsG.nGbkMode);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_23CANDKEY)
		{//允许使用快捷23重码
			BOOL b23CandKey=IsDlgButtonChecked(hWnd,IDC_23CANDKEY);
			EnableWindow(GetDlgItem(hWnd,IDC_HOTKEY_2CAND),b23CandKey);
			EnableWindow(GetDlgItem(hWnd,IDC_HOTKEY_3CAND),b23CandKey);
			EnableWindow(GetDlgItem(hWnd,IDC_CANDSEL_NONUM),b23CandKey);
			if(!b23CandKey) CheckDlgButton(hWnd,IDC_CANDSEL_NONUM,FALSE);
		}else if(HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_CANDSEL_NONUM)
		{//不使用数字键选择重码
			BOOL bNoNumCand=IsDlgButtonChecked(hWnd,IDC_CANDSEL_NONUM);
			if(bNoNumCand) 	SendMessage(GetDlgItem(hWnd,IDC_SPIN_PAGECANDS),UDM_SETPOS,0,MAKELONG(3,0));
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			{
				g_SettingsG.b23CandKey=IsDlgButtonChecked(hWnd,IDC_23CANDKEY);
				g_SettingsG.by2CandVK=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_2CAND),HKM_GETHOTKEY,0,0);
				g_SettingsG.by3CandVK=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_3CAND),HKM_GETHOTKEY,0,0);
				g_SettingsG.byTurnPageUpVK=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_CAND_PAGEUP),HKM_GETHOTKEY,0,0);
				g_SettingsG.byTurnPageDownVK=(BYTE)SendMessage(GetDlgItem(hWnd,IDC_HOTKEY_CAND_PAGEDOWN),HKM_GETHOTKEY,0,0);
				g_SettingsG.cPageCands=(char)SendMessage(GetDlgItem(hWnd,IDC_SPIN_PAGECANDS),UDM_GETPOS,0,0);
				g_SettingsG.bCandSelNoNum=IsDlgButtonChecked(hWnd,IDC_CANDSEL_NONUM);
				g_SettingsG.bOnlySimpleCode=IsDlgButtonChecked(hWnd,IDC_ONLYSIMPLECODE);
				if(IsDlgButtonChecked(hWnd,IDC_GBK_HIDE)) g_SettingsG.nGbkMode=0;
				else if(IsDlgButtonChecked(hWnd,IDC_GBK_SHOWI)) g_SettingsG.nGbkMode=1;
				else g_SettingsG.nGbkMode=2;
			}
			break;
		}
	}
	return bRet;
}

INT_PTR WINAPI PropPage_Proc_Associate(
			HWND hWnd ,
			UINT message ,
			WPARAM wParam ,
			LPARAM lParam )
{
	BOOL bRet=PropPage_Proc_Default(hWnd,message,wParam,lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		{
			UINT uID;
			CheckDlgButton(hWnd,IDC_AUTOPROMPT,g_SettingsG.bAutoPrompt);
			CheckDlgButton(hWnd,IDC_AUTODOT,g_SettingsG.bAutoDot);
			CheckDlgButton(hWnd,IDC_AUTOMATCH,g_SettingsG.bAutoMatch);
			CheckDlgButton(hWnd,IDC_ASTSENT,g_SettingsG.bAstSent);
			CheckRadioButton(hWnd,IDC_ASTNULL,IDC_PHRASEREMIND,IDC_ASTNULL+g_SettingsG.byAstMode);
			if(g_SettingsG.byForecast==MQC_FORECAST)
				uID=IDC_FORECAST_ALWAYS;
			else if(g_SettingsG.byForecast==MQC_FCNOCAND)
				uID=IDC_FORECAST_NOCAND;
			else
				uID=IDC_FORECAST_DISABLE;
			CheckRadioButton(hWnd,IDC_FORECAST_DISABLE,IDC_FORECAST_NOCAND,uID);
			CheckRadioButton(hWnd,IDC_RATE_DISABLE,IDC_RATE_QUICK,g_SettingsG.byRateAdjust+IDC_RATE_DISABLE);
			SetDlgItemText(hWnd,IDC_WEB_HEADER,g_SettingsG.szWebHeader);
			ImmAssociateContext(GetDlgItem(hWnd,IDC_WEB_HEADER),(HIMC)NULL);
		}
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)  // type of notification message             
		{
		case PSN_APPLY:
			g_SettingsG.bAutoPrompt=IsDlgButtonChecked(hWnd,IDC_AUTOPROMPT);
			g_SettingsG.bAutoDot=IsDlgButtonChecked(hWnd,IDC_AUTODOT);
			g_SettingsG.bAutoMatch=IsDlgButtonChecked(hWnd,IDC_AUTOMATCH);

			if(IsDlgButtonChecked(hWnd,IDC_RATE_DISABLE))
				g_SettingsG.byRateAdjust=0;
			else if(IsDlgButtonChecked(hWnd,IDC_RATE_AUTO))
				g_SettingsG.byRateAdjust=1;
			else// if(IsDlgButtonChecked(hWnd,IDC_RATE_QUICK))
				g_SettingsG.byRateAdjust=2;
			
			g_SettingsG.bAstSent=IsDlgButtonChecked(hWnd,IDC_ASTSENT);
			if(IsDlgButtonChecked(hWnd,IDC_ASTENGLISH))
				g_SettingsG.byAstMode=AST_ENGLISH;
			else if(IsDlgButtonChecked(hWnd,IDC_ASTCAND))
				g_SettingsG.byAstMode=AST_CAND;
			else if(IsDlgButtonChecked(hWnd,IDC_PHRASEREMIND))
				g_SettingsG.byAstMode=AST_PHRASEREMIND;
			else
				g_SettingsG.byAstMode=AST_NULL;

			if(IsDlgButtonChecked(hWnd,IDC_FORECAST_ALWAYS))
				g_SettingsG.byForecast=MQC_FORECAST;
			else if(IsDlgButtonChecked(hWnd,IDC_FORECAST_NOCAND))
				g_SettingsG.byForecast=MQC_FCNOCAND;
			else
				g_SettingsG.byForecast=0;

			GetDlgItemText(hWnd,IDC_WEB_HEADER,g_SettingsG.szWebHeader,100);
			break;
		}
	}
	return bRet;
}

void FillInPropertyPage( HINSTANCE hInst,PROPSHEETPAGE* psp, int idDlg,  DLGPROC pfnDlgProc)
{
    psp->dwSize = sizeof(PROPSHEETPAGE);
    psp->dwFlags = PSP_HASHELP ;
    psp->hInstance = hInst;
    psp->pszTemplate = MAKEINTRESOURCE(idDlg);
    psp->pszIcon = NULL;
    psp->pfnDlgProc = pfnDlgProc;
    psp->lParam = 0;
}

int CALLBACK PropSheet_Proc(
    HWND hwndDlg,
    UINT uMsg,
    LPARAM lParam
)
{
	if(uMsg==PSCB_INITIALIZED)
	{
		SetWindowLongPtr(hwndDlg,GWL_EXSTYLE,GetWindowLongPtr(hwndDlg,GWL_EXSTYLE)&(~WS_EX_CONTEXTHELP));
	}
	return 0;
}

INT_PTR	PropSheet_Show(HWND hwndOwner, HINSTANCE hInst, int nStartPage)
{
    PROPSHEETPAGE psp[8];
    PROPSHEETHEADER psh;
	INT_PTR nRet=-1;

	FillInPropertyPage(hInst, &psp[0], IDD_PROP_CUSTOM, PropPage_Proc_Custom);
	FillInPropertyPage(hInst, &psp[1], IDD_PROP_HOTKEY, PropPage_Proc_HotKey);
	FillInPropertyPage(hInst, &psp[2], IDD_PROP_ASSOCIATE, PropPage_Proc_Associate);
	FillInPropertyPage(hInst, &psp[3], IDD_PROP_CANDMODE, PropPage_Proc_CandMode);
	FillInPropertyPage(hInst, &psp[4], IDD_PROP_OTHER,  PropPage_Proc_Other);
	FillInPropertyPage(hInst, &psp[5], IDD_PROP_PLUGIN,  PropPage_Proc_Plugin);
	FillInPropertyPage(hInst, &psp[6], IDD_PROP_ABOUT, PropPage_Proc_About);
	FillInPropertyPage(hInst, &psp[7], IDD_PROP_WNDEFFECT, PropPage_Proc_WndEffect);
    
    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_HASHELP|PSH_USEPAGELANG|PSH_USECALLBACK|PSH_USEHICON  ;
    psh.hwndParent = hwndOwner;
    psh.pszCaption = (LPSTR) TEXT("系统设置");
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE)-(Helper_IsLayerSupport()?0:1);
    psh.nStartPage = nStartPage;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
	psh.pfnCallback =PropSheet_Proc;
	psh.hIcon=LoadIcon(hInst,MAKEINTRESOURCE(IDI_SETTINGS));
	s_bFirstInit=TRUE;
    
	nRet=PropertySheet(&psh);
	if(nRet!=-1)
	{
		char szConfig[MAX_PATH],szBuf[10];
		Config_GetIni(szConfig);
		Config_Save_Global(szConfig);
		sprintf(szConfig,"%s\\hotkey_%s.txt",g_szDataPath,ISComm_GetCompInfo()->szName);
		//保存编码相关的设置
		szBuf[0]=g_cHotKeyUD,szBuf[1]=0;
		WritePrivateProfileString("hotkey","umode",szBuf,szConfig);
		szBuf[0]=g_cHotKeySent,szBuf[1]=0;
		WritePrivateProfileString("hotkey","sentence",szBuf,szConfig);
		sprintf(szBuf,"%s",g_SettingsG.bFastUMode?"1":"0");
		WritePrivateProfileString("settings","FastUMode",szBuf,szConfig);
	}
	return nRet;
}

void Config_Load_Global(LPCTSTR pszSectionData)
{
	char szBuf[100];
	Helper_GetProfileSectionString("PhonticLeft","[",g_SettingsG.szPhonticLeft,50,pszSectionData);
	Helper_GetProfileSectionString("PhonticRight","]",g_SettingsG.szPhonticRight,50,pszSectionData);
	g_SettingsG.byRateAdjust=Helper_GetProfileSectionInt("RateAdjust",1,pszSectionData);
	g_SettingsG.b23CandKey=Helper_GetProfileSectionInt("23CandKey",FALSE,pszSectionData);

	g_SettingsG.by2CandVK=Helper_GetProfileSectionInt("2CandVK",VkKeyScan(';'),pszSectionData);
	g_SettingsG.by3CandVK=Helper_GetProfileSectionInt("3CandVK",VkKeyScan('\''),pszSectionData);
	g_SettingsG.byTurnPageUpVK=Helper_GetProfileSectionInt("TurnPageUpVK",VkKeyScan('-'),pszSectionData);
	g_SettingsG.byTurnPageDownVK=Helper_GetProfileSectionInt("TurnPageDownVK",VkKeyScan('='),pszSectionData);

	g_SettingsG.byHotKeyQuery=Helper_GetProfileSectionInt("HotKey_Query",VkKeyScan('/'),pszSectionData);
	g_SettingsG.byHotKeyMode=Helper_GetProfileSectionInt("HotKey_Switch_Mode",VkKeyScan('q'),pszSectionData);	
	g_SettingsG.byHotKeyEn=Helper_GetProfileSectionInt("HotKey_Switch_En",VkKeyScan('e'),pszSectionData);	
	g_SettingsG.byHotKeyMakeWord=Helper_GetProfileSectionInt("HotKey_MakeWord",VkKeyScan('m'),pszSectionData);
	g_SettingsG.byHotKeyShowRoot=Helper_GetProfileSectionInt("HotKey_ShowRoot",VkKeyScan('k'),pszSectionData);	
	g_SettingsG.byHotKeyHideStatus=Helper_GetProfileSectionInt("HotKey_HideStatus",VkKeyScan('y'),pszSectionData);	

	
	g_SettingsG.byAstMode=Helper_GetProfileSectionInt("Associate",AST_ENGLISH,pszSectionData);
	g_SettingsG.bAstSent=Helper_GetProfileSectionInt("SentAssociate",1,pszSectionData);
	g_SettingsG.bySwitchKey=Helper_GetProfileSectionInt("SwitchKey",0x36,pszSectionData);//右SHIFT
	g_SettingsG.byTempSpellKey=Helper_GetProfileSectionInt("TempSpellKey",0xC1,pszSectionData);//右Ctrl
	Helper_GetProfileSectionString("LineKey1","g",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[0]=szBuf[0];
	Helper_GetProfileSectionString("LineKey2","h",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[1]=szBuf[0];
	Helper_GetProfileSectionString("LineKey3","t",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[2]=szBuf[0];
	Helper_GetProfileSectionString("LineKey4","y",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[3]=szBuf[0];
	Helper_GetProfileSectionString("LineKey5","n",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[4]=szBuf[0];
	Helper_GetProfileSectionString("LineKey6","z",szBuf,2,pszSectionData);
	g_SettingsG.byLineKey[5]=szBuf[0];
	g_SettingsG.byPntcRed=Helper_GetProfileSectionInt("ColorPhontic_Red",0,pszSectionData);
	g_SettingsG.byPntcGreen=Helper_GetProfileSectionInt("ColorPhontic_Green",0,pszSectionData);
	g_SettingsG.byPntcBlue=Helper_GetProfileSectionInt("ColorPhontic_Blue",0,pszSectionData);
	g_SettingsG.byForecast=Helper_GetProfileSectionInt("Forecast",MQC_FORECAST,pszSectionData);
	g_SettingsG.byTransStatus=Helper_GetProfileSectionInt("TransStatusP",80,pszSectionData);
	if(g_SettingsG.byTransStatus<50) g_SettingsG.byTransStatus=50;
	if(g_SettingsG.byTransStatus>100) g_SettingsG.byTransStatus=100;
	g_SettingsG.byTransInput=Helper_GetProfileSectionInt("TransInputP",80,pszSectionData);
	if(g_SettingsG.byTransInput<50) g_SettingsG.byTransInput=50;
	if(g_SettingsG.byTransInput>100) g_SettingsG.byTransInput=100;

	g_SettingsG.cPageCands=Helper_GetProfileSectionInt("PageCands",5,pszSectionData);
	if(g_SettingsG.cPageCands<3 || g_SettingsG.cPageCands>10) g_SettingsG.cPageCands=5;
	g_SettingsG.bShowOpTip=Helper_GetProfileSectionInt("ShowTip",1,pszSectionData);
	g_SettingsG.bAutoMatch=Helper_GetProfileSectionInt("AutoMatch",1,pszSectionData);
	g_SettingsG.nGbkMode=Helper_GetProfileSectionInt("GBKMODE",1,pszSectionData);
	g_SettingsG.bTTSInput=Helper_GetProfileSectionInt("TTSInput",0,pszSectionData);
	g_SettingsG.bBlendUD=Helper_GetProfileSectionInt("BlendUD",1,pszSectionData);
	g_SettingsG.bBlendSpWord=Helper_GetProfileSectionInt("BlendSpWord",0,pszSectionData);
	g_SettingsG.bAutoInput=Helper_GetProfileSectionInt("AutoInput",1,pszSectionData);

	g_SettingsG.bPYPhraseFirst=Helper_GetProfileSectionInt("PYPhraseFirst",1,pszSectionData);
	g_SettingsG.bEnterClear=Helper_GetProfileSectionInt("EnterClear",1,pszSectionData);
	g_SettingsG.nSoundAlert=Helper_GetProfileSectionInt("SoundAlert",1,pszSectionData);

	g_SettingsG.bDisableFnKey=Helper_GetProfileSectionInt("DisableFnKey",1,pszSectionData);
	g_SettingsG.bAutoDot=Helper_GetProfileSectionInt("AutoDot",1,pszSectionData);
	g_SettingsG.bAutoPrompt=Helper_GetProfileSectionInt("AutoPrompt",0,pszSectionData);

	g_SettingsG.bDisableDelWordCand=Helper_GetProfileSectionInt("DisableDelWordCand",1,pszSectionData);
	g_SettingsG.bCandSelNoNum=Helper_GetProfileSectionInt("CandSelNoNum",0,pszSectionData);

	Helper_GetProfileSectionString("WebHeader","www. http mail bbs. ftp:",g_SettingsG.szWebHeader,100,pszSectionData);
	g_SettingsG.byFadeSteps=Helper_GetProfileSectionInt("FadeSteps",5,pszSectionData);

	Helper_GetProfileSectionString("Plugin","",g_SettingsG.szPlugin,100,pszSectionData);
	g_SettingsG.bOnlySimpleCode=Helper_GetProfileSectionInt("OnlySimpleCode",0,pszSectionData);
	g_SettingsG.bIptWndVertical=Helper_GetProfileSectionInt("InputWndVertical",0,pszSectionData);
	g_SettingsG.bShadeInput=Helper_GetProfileSectionInt("ShadeInput",0,pszSectionData);
	g_SettingsG.bShadeStatus=Helper_GetProfileSectionInt("ShadeStatus",0,pszSectionData);
}

#define SIZE_INI (1<<13)

void Config_Load()
{
	BOOL bRet=FALSE;
	char szConfig[MAX_PATH],szBuf[1<<13];
	szConfig[0]=0,szBuf[0]=0;
	// read config info from config.ini
	Config_GetIni(szConfig);
	GetPrivateProfileSection("IME",szBuf,SIZE_INI,szConfig);
	g_SettingsL.bCharMode=Helper_GetProfileSectionInt("CharMode",TRUE,szBuf);
	g_SettingsL.bFullStatus=Helper_GetProfileSectionInt("FullStatus",TRUE,szBuf);
	g_SettingsL.bSound=Helper_GetProfileSectionInt("Sound",TRUE,szBuf);
	g_SettingsL.bRecord=Helper_GetProfileSectionInt("Record",TRUE,szBuf);
	g_SettingsL.bEnglish=Helper_GetProfileSectionInt("English",FALSE,szBuf);
	g_SettingsL.bMouseFollow=Helper_GetProfileSectionInt("MouseFollow",TRUE,szBuf);
	g_SettingsL.bHideStatus=Helper_GetProfileSectionInt("HideStatus",0,szBuf);
	g_SettingsL.bInputBig5=Helper_GetProfileSectionInt("InputBig5",0,szBuf);
	g_CompMode=Helper_GetProfileSectionInt("CompMode",IM_SPELL,szBuf);
	//获取设置信息,只在第一次启动时从注册表中读取
	if(g_ImeCount==0) Config_Load_Global(szBuf);
}
 
void Config_Save_Global(LPCTSTR pszIniFile)
{
	char szBuf[100]={0};

	WritePrivateProfileString("IME","PhonticLeft",g_SettingsG.szPhonticLeft,pszIniFile);
	WritePrivateProfileString("IME","PhonticRight",g_SettingsG.szPhonticRight,pszIniFile);
	WritePrivateProfileInt("IME","RateAdjust",g_SettingsG.byRateAdjust,pszIniFile);
	WritePrivateProfileInt("IME","23CandKey",g_SettingsG.b23CandKey,pszIniFile);

	WritePrivateProfileInt("IME","2CandVK",g_SettingsG.by2CandVK,pszIniFile);
	WritePrivateProfileInt("IME","3CandVK",g_SettingsG.by3CandVK,pszIniFile);
	WritePrivateProfileInt("IME","TurnPageUpVK",g_SettingsG.byTurnPageUpVK,pszIniFile);
	WritePrivateProfileInt("IME","TurnPageDownVK",g_SettingsG.byTurnPageDownVK,pszIniFile);

	WritePrivateProfileInt("IME","HotKey_Query",g_SettingsG.byHotKeyQuery,pszIniFile);
	WritePrivateProfileInt("IME","HotKey_Switch_Mode",g_SettingsG.byHotKeyMode,pszIniFile);
	WritePrivateProfileInt("IME","HotKey_Switch_En",g_SettingsG.byHotKeyEn,pszIniFile);
	WritePrivateProfileInt("IME","HotKey_MakeWord",g_SettingsG.byHotKeyMakeWord,pszIniFile);
	WritePrivateProfileInt("IME","HotKey_ShowRoot",g_SettingsG.byHotKeyShowRoot,pszIniFile);
	WritePrivateProfileInt("IME","HotKey_HideStatus",g_SettingsG.byHotKeyHideStatus,pszIniFile);

	WritePrivateProfileInt("IME","Associate",g_SettingsG.byAstMode,pszIniFile);
	WritePrivateProfileInt("IME","SentAssociate",g_SettingsG.bAstSent,pszIniFile);
	WritePrivateProfileInt("IME","SwitchKey",g_SettingsG.bySwitchKey,pszIniFile);
	WritePrivateProfileInt("IME","TempSpellKey",g_SettingsG.byTempSpellKey,pszIniFile);
	WritePrivateProfileInt("IME","Forecast",g_SettingsG.byForecast,pszIniFile);
	WritePrivateProfileInt("IME","PageCands",g_SettingsG.cPageCands,pszIniFile);
	WritePrivateProfileInt("IME","ShowTip",g_SettingsG.bShowOpTip,pszIniFile);
	WritePrivateProfileInt("IME","AutoMatch",g_SettingsG.bAutoMatch,pszIniFile);

	szBuf[0]=g_SettingsG.byLineKey[0];
	WritePrivateProfileString("IME","LineKey1",szBuf,pszIniFile);
	szBuf[0]=g_SettingsG.byLineKey[1];
	WritePrivateProfileString("IME","LineKey2",szBuf,pszIniFile);
	szBuf[0]=g_SettingsG.byLineKey[2];
	WritePrivateProfileString("IME","LineKey3",szBuf,pszIniFile);
	szBuf[0]=g_SettingsG.byLineKey[3];
	WritePrivateProfileString("IME","LineKey4",szBuf,pszIniFile);
	szBuf[0]=g_SettingsG.byLineKey[4];
	WritePrivateProfileString("IME","LineKey5",szBuf,pszIniFile);
	szBuf[0]=g_SettingsG.byLineKey[5];
	WritePrivateProfileString("IME","LineKey6",szBuf,pszIniFile);
	
	WritePrivateProfileInt("IME","ColorPhontic_Red",g_SettingsG.byPntcRed,pszIniFile);
	WritePrivateProfileInt("IME","ColorPhontic_Green",g_SettingsG.byPntcGreen,pszIniFile);
	WritePrivateProfileInt("IME","ColorPhontic_Blue",g_SettingsG.byPntcBlue,pszIniFile);
	WritePrivateProfileInt("IME","TransStatusP",g_SettingsG.byTransStatus,pszIniFile);
	WritePrivateProfileInt("IME","TransInputP",g_SettingsG.byTransInput,pszIniFile);
	WritePrivateProfileInt("IME","GBKMODE",g_SettingsG.nGbkMode,pszIniFile);
	WritePrivateProfileInt("IME","TTSInput",g_SettingsG.bTTSInput,pszIniFile);
	WritePrivateProfileInt("IME","AutoInput",g_SettingsG.bAutoInput,pszIniFile);
	WritePrivateProfileInt("IME","PYPhraseFirst",g_SettingsG.bPYPhraseFirst,pszIniFile);
	WritePrivateProfileInt("IME","EnterClear",g_SettingsG.bEnterClear,pszIniFile);
	WritePrivateProfileInt("IME","SoundAlert",g_SettingsG.nSoundAlert,pszIniFile);
	WritePrivateProfileInt("IME","DisableFnKey",g_SettingsG.bDisableFnKey,pszIniFile);
	WritePrivateProfileInt("IME","AutoDot",g_SettingsG.bAutoDot,pszIniFile);
	WritePrivateProfileInt("IME","AutoPrompt",g_SettingsG.bAutoPrompt,pszIniFile);
	WritePrivateProfileInt("IME","DisableDelWordCand",g_SettingsG.bDisableDelWordCand,pszIniFile);
	WritePrivateProfileInt("IME","CandSelNoNum",g_SettingsG.bCandSelNoNum,pszIniFile);
	WritePrivateProfileString("IME","WebHeader",g_SettingsG.szWebHeader,pszIniFile);
	WritePrivateProfileInt("IME","FadeSteps",g_SettingsG.byFadeSteps,pszIniFile);
	WritePrivateProfileString("IME","Plugin",g_SettingsG.szPlugin,pszIniFile);
	WritePrivateProfileInt("IME","OnlySimpleCode",g_SettingsG.bOnlySimpleCode,pszIniFile);

	WritePrivateProfileInt("IME","ShadeInput",g_SettingsG.bShadeInput,pszIniFile);
	WritePrivateProfileInt("IME","ShadeStatus",g_SettingsG.bShadeStatus,pszIniFile);

}

void Config_SaveInt(LPCTSTR pszKey,UINT nValue)
{
	char szConfig[MAX_PATH];
	Config_GetIni(szConfig);//don't care about whether the config file is exist.
	WritePrivateProfileInt("IME",pszKey,nValue,szConfig);
}
