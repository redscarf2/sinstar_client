//***********************************************************************
//	这个文件处理UI消息，导出UIWndProc接口,一些和界面相关的工具函数也在此
//	文件中定义。
//***********************************************************************
#include "sinstar2.h"
#include "uispchar.h"
#include "udictinput.h"
#include "ui.h"
#include <math.h>
//#include "indicml.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TI_DELAYINPUTWND	100
#define TI_DELAYPASTE		200

void StatusWnd_EnsureVisible(HWND hWnd,int nWidth,int nHeight);

void PASCAL UIWnd_UpdateCheckButton()
{
	g_SkinStatus.wndBtn.pBtns[IDC_VIEWMODE1-IDC_BTN_BASE].bVisible=!g_SettingsL.bFullStatus;
	g_SkinStatus.wndBtn.pBtns[IDC_VIEWMODE2-IDC_BTN_BASE].bVisible=g_SettingsL.bFullStatus;
	g_SkinStatus.wndBtn.pBtns[IDC_CHARMODE1-IDC_BTN_BASE].bVisible=g_SettingsL.bCharMode;
	g_SkinStatus.wndBtn.pBtns[IDC_CHARMODE2-IDC_BTN_BASE].bVisible=!g_SettingsL.bCharMode;
	g_SkinStatus.wndBtn.pBtns[IDC_RECORD1-IDC_BTN_BASE].bVisible=g_SettingsL.bRecord;
	g_SkinStatus.wndBtn.pBtns[IDC_RECORD2-IDC_BTN_BASE].bVisible=!g_SettingsL.bRecord;
	g_SkinStatus.wndBtn.pBtns[IDC_SOUND1-IDC_BTN_BASE].bVisible=g_SettingsL.bSound;
	g_SkinStatus.wndBtn.pBtns[IDC_SOUND2-IDC_BTN_BASE].bVisible=!g_SettingsL.bSound;
	g_SkinStatus.wndBtn.pBtns[IDC_ENGLISH1-IDC_BTN_BASE].bVisible=g_SettingsL.bEnglish;
	g_SkinStatus.wndBtn.pBtns[IDC_ENGLISH2-IDC_BTN_BASE].bVisible=!g_SettingsL.bEnglish;
}


/**********************************************************************/
/* 显示状态栏                                                       */
/**********************************************************************/
void PASCAL UIWnd_OpenStatus( HWND hUIWnd ,BOOL bShow)
{
    HGLOBAL        hUIPrivate;
    LPUIPRIV       lpUIPrivate;
    HIMC           hIMC;
    int            nShowCmd=bShow?SW_SHOWNOACTIVATE:SW_HIDE;

    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate)           // can not darw status window
       return;
    
    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate)          // can not draw status window
       return;

    hIMC = (HIMC)GetWindowLongPtr(hUIWnd, IMMGWLP_IMC);
    if (!hIMC) {
       nShowCmd = SW_HIDE;
    } 
    else if (!ImmLockIMC(hIMC))
	{
        nShowCmd = SW_HIDE;
    }else
	{
		ImmUnlockIMC(hIMC);
	}
	
	if(!lpUIPrivate->hStatusWnd)
	{
		SIZE size=g_SettingsL.bFullStatus?g_SkinStatus.sizeFull:g_SkinStatus.sizeShrink;
		HRGN hRgn=g_SettingsL.bFullStatus?g_SkinStatus.hRgnFull:g_SkinStatus.hRgnShrink;
        lpUIPrivate->hStatusWnd = CreateWindowEx(
									Helper_IsLayerSupport()?0x80000:0,//0x80000=WS_EX_LAYERED
                                     CLSNAME_STATUS, NULL, 
                                     WS_POPUP|WS_DISABLED,
                                     lpUIPrivate->ptStatusWnd.x,lpUIPrivate->ptStatusWnd.y,
                                     size.cx,size.cy,
                                     hUIWnd, (HMENU)NULL, g_hInst, NULL);
		
		if(Helper_IsLayerSupport())
		{
			Helper_SetWindowLayerAttributes(lpUIPrivate->hStatusWnd,(BYTE)ceil(2.55*g_SettingsG.byTransStatus));
		}
		if(hRgn) Helper_SetWindowRgn(lpUIPrivate->hStatusWnd,hRgn,FALSE);
		StatusWnd_EnsureVisible(lpUIPrivate->hStatusWnd,0,0);//计算靠边类型
	}

	ShowWindow( lpUIPrivate->hStatusWnd, nShowCmd);
    GlobalUnlock(hUIPrivate);
    return;
}


/**********************************************************************/
/* 显示状态栏                                                       */
/**********************************************************************/
void PASCAL UIWnd_OpenSpChar( HWND hUIWnd ,BOOL bShow)
{
    HGLOBAL        hUIPrivate;
    LPUIPRIV       lpUIPrivate;
    HIMC           hIMC;
    int            nShowCmd=bShow?SW_SHOWNOACTIVATE:SW_HIDE;

    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate)           // can not darw status window
       return;
    

    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate)          // can not draw status window
       return;
    

    hIMC = (HIMC)GetWindowLongPtr(hUIWnd, IMMGWLP_IMC);
    if (!hIMC) {
       nShowCmd = SW_HIDE;
    } 
    else if (!ImmLockIMC(hIMC))
	{
        nShowCmd = SW_HIDE;
    }else
	{
		ImmUnlockIMC(hIMC);
	}
	
	if(!lpUIPrivate->hSpCharWnd )
	{
        lpUIPrivate->hSpCharWnd = CreateWindowEx(
									0,
                                     CLSNAME_SPCHAR, NULL, 
                                     WS_POPUP|WS_DISABLED|WS_DLGFRAME,
                                     lpUIPrivate->ptSpCharWnd.x,lpUIPrivate->ptSpCharWnd.y,
                                     SPCHAR_WID,SPCHAR_HEI,
                                     hUIWnd, (HMENU)NULL, g_hInst, NULL);
	}

	ShowWindow( lpUIPrivate->hSpCharWnd, nShowCmd);
	if(bShow) SendMessage( lpUIPrivate->hSpCharWnd,UM_SETLAYOUT,g_SpCharData.nCurLayout,0);
    GlobalUnlock(hUIPrivate);
    return;
}

/**********************************************************************/
/* 显示状态栏                                                       */
/**********************************************************************/
void PASCAL UIWnd_OpenInput( HWND hUIWnd ,BOOL bShow,BOOL bCalcSize)
{
    HGLOBAL        hUIPrivate;
    LPUIPRIV       lpUIPrivate;
    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate)           // can not darw status window
       return;
    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate )          // can not draw status window
       return;
	if(!lpUIPrivate->hInputWnd)
	{
		POINT pt=g_SettingsL.bMouseFollow?g_ptInputPos:lpUIPrivate->ptInputWnd;
        lpUIPrivate->hInputWnd = CreateWindowEx(
									WS_EX_TOPMOST|Helper_IsLayerSupport()?0x80000:0,//0x80000=WS_EX_LAYERED
                                     CLSNAME_INPUT, NULL, 
                                     WS_POPUP|WS_DISABLED,
                                     pt.x ,pt.y ,
                                     0,0,
                                     hUIWnd, (HMENU)NULL, g_hInst, NULL);
		if(Helper_IsLayerSupport())
		{
			Helper_SetWindowLayerAttributes(lpUIPrivate->hInputWnd,(BYTE)ceil(2.55*g_SettingsG.byTransInput));
		}

	}
	if(bShow && bCalcSize) SendMessage(lpUIPrivate->hInputWnd,UM_CALCSIZE,0,0);
	SendMessage(lpUIPrivate->hInputWnd,bShow?UM_SHOWWINDOW:UM_HIDEWINDOW,0,0);

    GlobalUnlock(hUIPrivate);
}

/**********************************************************************/
/* UIWnd_GetStatusWnd                                                         */
/* Return Value :                                                     */
/*      window handle of candidatte                                   */
/**********************************************************************/
HWND PASCAL UIWnd_GetStatusWnd(
    HWND hUIWnd)                // UI window
{
    HGLOBAL  hUIPrivate;
    LPUIPRIV lpUIPrivate;
    HWND     hWnd;

    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate) {          // can not darw candidate window
        return (HWND)NULL;
    }

    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate) {         // can not draw candidate window
        return (HWND)NULL;
    }

    hWnd = lpUIPrivate->hStatusWnd;

    GlobalUnlock(hUIPrivate);
    return (hWnd);
}

/**********************************************************************/
/* UIWnd_GetInputWnd                                                         */
/* Return Value :                                                     */
/*      window handle of candidatte                                   */
/**********************************************************************/
HWND PASCAL UIWnd_GetInputWnd(
    HWND hUIWnd)                // UI window
{
    HGLOBAL  hUIPrivate;
    LPUIPRIV lpUIPrivate;
    HWND     hWnd;

    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate) {          // can not darw candidate window
        return (HWND)NULL;
    }

    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate) {         // can not draw candidate window
        return (HWND)NULL;
    }

    hWnd = lpUIPrivate->hInputWnd;

    GlobalUnlock(hUIPrivate);
    return (hWnd);
}

/**********************************************************************/
/* UIPaint()                                                          */
/**********************************************************************/
LRESULT PASCAL UIWnd_Paint(HWND   hUIWnd)
{
    PAINTSTRUCT ps;
	MSG sMsg;
    // for safety
    BeginPaint(hUIWnd, &ps);
    EndPaint(hUIWnd, &ps);

    // some application will not remove the WM_PAINT messages
    PeekMessage(&sMsg, hUIWnd, WM_PAINT, WM_PAINT, PM_REMOVE|PM_NOYIELD);
    return (0L);
}

//从配置文件中获取输入窗口位置,确保位置合法
void UI_Config_Load(LPUIPRIV pUIPriv)
{
	BOOL bRet=FALSE;
	char szConfig[MAX_PATH], szBuf[100];
	RECT rcWorkArea;
	// read config info from register table
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);

	Config_GetIni(szConfig);

	pUIPriv->ptInputWnd.x=pUIPriv->ptInputWnd.y=-1;
	if(GetPrivateProfileString("WIN_POS","InputWndPos",NULL,szBuf,100,szConfig))
	{
		sscanf(szBuf,"%d,%d",&pUIPriv->ptInputWnd.x,&pUIPriv->ptInputWnd.y);
	}
	
	pUIPriv->ptStatusWnd.x=-1;
	if(GetPrivateProfileString("WIN_POS","StatusWndPos",NULL,szBuf,100,szConfig))
	{
		sscanf(szBuf,"%d,%d",&pUIPriv->ptStatusWnd.x,&pUIPriv->ptStatusWnd.y);
	}
	g_ptStatusDockMode.x=g_ptStatusDockMode.y=1;
	if(GetPrivateProfileString("WIN_POS","StatusWndDock",NULL,szBuf,100,szConfig))
	{
		sscanf(szBuf,"%d,%d",&g_ptStatusDockMode.x,&g_ptStatusDockMode.y);
	}
	
	pUIPriv->ptSpCharWnd.x=-1;
	if(GetPrivateProfileString("WIN_POS","SpCharWndPos",NULL,szBuf,100,szConfig))
	{
		sscanf(szBuf,"%d,%d",&pUIPriv->ptSpCharWnd.x,&pUIPriv->ptSpCharWnd.y);
	}

	if(pUIPriv->ptInputWnd.x<0 
		|| pUIPriv->ptInputWnd.y<0 
		|| pUIPriv->ptInputWnd.x>rcWorkArea.right-g_SkinInput.modeHerizontal.size.cx-100
		|| pUIPriv->ptInputWnd.y>rcWorkArea.bottom-g_SkinInput.modeHerizontal.size.cy-10)
	{
		pUIPriv->ptInputWnd.x=(rcWorkArea.right-g_SkinInput.modeHerizontal.size.cx)/2-50;
		pUIPriv->ptInputWnd.y=rcWorkArea.bottom-g_SkinInput.modeHerizontal.size.cy-10;
	}
//	g_ptInputPos=pUIPriv->ptInputWnd;

	if(!PtInRect(&rcWorkArea,pUIPriv->ptStatusWnd))
	{
		if(g_SettingsL.bFullStatus)
		{
			pUIPriv->ptStatusWnd.x=rcWorkArea.right-g_SkinStatus.sizeFull.cx;
			pUIPriv->ptStatusWnd.y=rcWorkArea.bottom-g_SkinStatus.sizeFull.cy;;
		}else
		{
			pUIPriv->ptStatusWnd.x=rcWorkArea.right-g_SkinStatus.sizeShrink.cx;
			pUIPriv->ptStatusWnd.y=rcWorkArea.bottom-g_SkinStatus.sizeShrink.cy;;
		}
	}
	if(!PtInRect(&rcWorkArea,pUIPriv->ptSpCharWnd))
	{
		pUIPriv->ptSpCharWnd.x=rcWorkArea.right-SPCHAR_WID;
		pUIPriv->ptSpCharWnd.y=rcWorkArea.bottom-SPCHAR_HEI;
	}
}
 
void UI_Config_Save(LPUIPRIV pUIPriv)
{
	char szConfig[MAX_PATH],szBuf[100];
	Config_GetIni(szConfig);//don't care about whether the config file is exist.
	// write config info to config file
	sprintf(szBuf,"%d,%d",pUIPriv->ptInputWnd.x,pUIPriv->ptInputWnd.y);
	WritePrivateProfileString("WIN_POS","InputWndPos",szBuf,szConfig);
	sprintf(szBuf,"%d,%d",pUIPriv->ptStatusWnd.x,pUIPriv->ptStatusWnd.y);
	WritePrivateProfileString("WIN_POS","StatusWndPos",szBuf,szConfig);
	sprintf(szBuf,"%d,%d",pUIPriv->ptSpCharWnd.x,pUIPriv->ptSpCharWnd.y);
	WritePrivateProfileString("WIN_POS","SpCharWndPos",szBuf,szConfig);
	sprintf(szBuf,"%d,%d",g_ptStatusDockMode.x,g_ptStatusDockMode.y);
	WritePrivateProfileString("WIN_POS","StatusWndDock",szBuf,szConfig);
}

void Spchar_Load()
{
	char szConfig[MAX_PATH],szSection[20];
	int i;
	sprintf(szConfig,"%s\\%s",g_szDataPath,"spchar.ini");

	g_SpCharData.nLayoutCount=GetPrivateProfileInt("projects","layouts",0,szConfig);
	if(g_SpCharData.nLayoutCount>20)
	{//error
		g_SpCharData.nLayoutCount=0;
		return;
	}
	for(i=0;i<g_SpCharData.nLayoutCount;i++)
	{
		sprintf(szSection,"layout%d",i+1);
		GetPrivateProfileString(szSection,"name","",g_SpCharData.layout[i].szName,50,szConfig);
		GetPrivateProfileString(szSection,"chars","",g_SpCharData.layout[i].szSpChar,200,szConfig);
		g_SpCharData.layout[i].nWords=strlen(g_SpCharData.layout[i].szSpChar)/2;
	}
	g_SpCharData.iCurSel=-1;
	g_SpCharData.nCurLayout=0;
}

/**********************************************************************/
/* UIWnd_Create()                                                    */
/**********************************************************************/
int PASCAL UIWnd_Create( HWND hUIWnd)
{
    HGLOBAL hUIPrivate;
	LPUIPRIV pUIPriv;
    // create storage for UI setting
    hUIPrivate = GlobalAlloc(GHND, sizeof(UIPRIV));
    if (!hUIPrivate) {     
		Helper_Trace("\nUIWnd_Create GlobalAlloc Error!!!");
        return -1;
    }
    SetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE, (LONG_PTR)hUIPrivate);

    // set the default position for UI window, it is hide now
    SetWindowPos(hUIWnd, NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE);

    ShowWindow(hUIWnd, SW_SHOWNOACTIVATE);

	pUIPriv=(LPUIPRIV)GlobalLock(hUIPrivate);

	Helper_Trace("\nUIWnd_Create hUIWnd=0x%08X",hUIWnd);
	Helper_ChangeWindowMessageFilter(ISComm_GetCommMsgID(),MSGFLT_ADD);
	if(!ISComm_Login(hUIWnd))
	{
		Helper_Trace("\n启动服务器失败");
		GlobalUnlock(hUIPrivate);
		GlobalFree(hUIPrivate);
		return -1;
	}	
	UI_Config_Load(pUIPriv);
	//检查皮肤是否在输入法关闭时发生了更新，如果发生更改则重新载入皮肤。
	if(strcmp(g_SkinStatus.szSkinName,g_szSkin)!=0) 
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
		Skin_Load(g_szSkin);
		Skin_GetBtnRect(szPath,g_SettingsL.bFullStatus);
		UIWnd_UpdateCheckButton();
	}
	UIWnd_UpdateCheckButton();
	{//插件初始化
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\plugin\\%s",g_szDataPath,g_SettingsG.szPlugin);
		Plugin_Load(szPath,hUIWnd);
	}
	Spchar_Load();
	//提示信息窗口
	g_hWndToolTip=CreateWindowEx(    0,
                                     CLSNAME_TOOLTIP, NULL, 
                                     WS_POPUP|WS_DISABLED,
                                     0,0,
                                     0,0,
                                     hUIWnd, (HMENU)NULL, g_hInst, NULL);		

	//消息显示窗口
	g_hWndMsg=CreateWindowEx(    0,
                                     CLSNAME_TOOLTIP, NULL, 
                                     WS_POPUP|WS_DISABLED,
                                     0,0,
                                     0,0,
                                     hUIWnd, (HMENU)NULL, g_hInst, NULL);	

	GlobalUnlock(hUIPrivate);
    return 0;
}

BOOL IsIMEOpen(HIMC hIMC)
{
	BOOL fOpen;
	LPINPUTCONTEXT lpIMC;
	if (!(lpIMC = ImmLockIMC(hIMC)))
		return FALSE;

	fOpen = lpIMC->fOpen;
	ImmUnlockIMC(hIMC);
	return fOpen;
}


/**********************************************************************/
/* DestroyUIWindow()                                                  */
/**********************************************************************/
void PASCAL UIWnd_Destroy(HWND hUIWnd)
{
    HGLOBAL  hUIPrivate;
    LPUIPRIV lpUIPrivate;
	//在程序退出时调用Helper_Trace输出信息会导致GoSurf浏览器出错
    hUIPrivate = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
    if (!hUIPrivate) {     
        return;
    }

    lpUIPrivate = (LPUIPRIV)GlobalLock(hUIPrivate);
    if (!lpUIPrivate) {    
        return;
    }
    // status window need to be destroyed
    if (IsWindow(lpUIPrivate->hStatusWnd)) DestroyWindow(lpUIPrivate->hStatusWnd);
    // input window need to be destroyed
    if (IsWindow(lpUIPrivate->hInputWnd))  DestroyWindow(lpUIPrivate->hInputWnd);
     // spchar window need to be destroyed
	if (IsWindow(lpUIPrivate->hSpCharWnd)) DestroyWindow(lpUIPrivate->hSpCharWnd);
	
	if(g_hRgnInput){ DeleteObject(g_hRgnInput); g_hRgnInput=0;}
	if(IsWindow(g_hWndToolTip))	DestroyWindow(g_hWndToolTip);
	g_hWndToolTip=NULL;
	if(IsWindow(g_hWndMsg))	DestroyWindow(g_hWndMsg);
	g_hWndMsg=NULL;
	// write config to register
	if(g_ImeCount<3) //防止程序出错时设置信息未能正确保存
		UI_Config_Save(lpUIPrivate);

    GlobalUnlock(hUIPrivate);
    // free storage for UI settings
    GlobalFree(hUIPrivate);

 	ISComm_Logout(hUIWnd);

	Plugin_Free();
}

/**********************************************************************/
/*                                                                    */
/* NotifyHandle()                                                     */
/*                                                                    */
/* Handle WM_IME_NOTIFY messages.                                     */
/*                                                                    */
/**********************************************************************/

LONG UIWnd_Notify(HIMC hIMC, HWND hUIWnd, WPARAM wParam, LPARAM lParam)
{
	LONG lRet = 1L;

    switch (wParam)
    {
	case IMN_OPENSTATUSWINDOW:
		UIWnd_OpenStatus(hUIWnd,!g_SettingsL.bHideStatus);
		break;
	case IMN_CLOSESTATUSWINDOW:
		UIWnd_OpenStatus(hUIWnd,FALSE);
		KillTimer(hUIWnd,TI_DELAYINPUTWND);
		break;
    case IMN_OPENCANDIDATE:
        break;
    case IMN_CLOSECANDIDATE:
        break;
    case IMN_CHANGECANDIDATE:
        break;
	case IMN_SETOPENSTATUS: 
		UIWnd_OpenStatus(hUIWnd,!g_SettingsL.bHideStatus);
		break;
	case IMN_SETCONVERSIONMODE:
		break;
	case IMN_SETSENTENCEMODE:
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		if(hIMC)
		{
			LPINPUTCONTEXT lpIMC=ImmLockIMC(hIMC);
			if(lpIMC)
			{
				if(g_SettingsL.bMouseFollow && (lpIMC->cfCompForm.ptCurrentPos.x!=0 || lpIMC->cfCompForm.ptCurrentPos.y!=0))
				{//光标跟随
					HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
					LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hUIPriv);
					switch(lpIMC->cfCompForm.dwStyle&0x0F)
					{
					case CFS_DEFAULT://for excel
						GetCaretPos(&g_ptInputPos);
						break;
					case CFS_POINT:
					case CFS_FORCE_POSITION:
						g_ptInputPos=lpIMC->cfCompForm.ptCurrentPos;
						break;
					case CFS_RECT:
						g_ptInputPos.x=lpIMC->cfCompForm.rcArea.left;
						g_ptInputPos.y=lpIMC->cfCompForm.rcArea.top;
					default:
						g_ptInputPos=lpIMC->cfCompForm.ptCurrentPos;
						break;
					}
					MapWindowPoints(lpIMC->hWnd,NULL,&g_ptInputPos,1);//ClientToScreen(lpIMC->hWnd,&g_ptInputPos);
					GlobalUnlock(hUIPriv);
					if(IsWindow(lpUIPriv->hInputWnd) 
						&& IsWindowVisible(lpUIPriv->hInputWnd))
						SendMessage(lpUIPriv->hInputWnd,UM_CALCSIZE,1,0);
				}
			}
			ImmUnlockIMC(hIMC);
		}
		Helper_Trace("\n#########IMN_SETCOMPOSITIONWINDOW");
		break;
	case IMN_SETSTATUSWINDOWPOS:
		Helper_Trace("\n=============IMN_SETSTATUSWINDOWPOS");
		break;
	case IMN_PRIVATE:
		{
			WORD wType=LOWORD(lParam);
			if(wType&IMN_PRIV_INVALIDINPUTWND)
			{
				HWND hInputWnd=UIWnd_GetInputWnd(hUIWnd);
				if(hInputWnd) InvalidateRect(hInputWnd,NULL,TRUE);
			}
			if(wType&IMN_PRIV_SHOWSPCHAR)
				UIWnd_OpenSpChar(hUIWnd,TRUE);
			if(wType&IMN_PRIV_HIDESPCHAR)
				UIWnd_OpenSpChar(hUIWnd,FALSE);
			if(wType&IMN_PRIV_COMMAND)
			{
				HWND hStatusWnd=UIWnd_GetStatusWnd(hUIWnd);
				SendMessage(hStatusWnd,WM_COMMAND,HIWORD(lParam),0);
			}
			if(wType&IMN_PRIV_SETDELAY)
				SetTimer(hUIWnd,TI_DELAYINPUTWND,5000,NULL);
			if(wType&IMN_PRIV_KILLDELAY)
				KillTimer(hUIWnd,TI_DELAYINPUTWND);
			if(wType&IMN_PRIV_OPENINPUTWND)
			{
				UIWnd_OpenInput(hUIWnd,TRUE,TRUE);
				KillTimer(hUIWnd,TI_DELAYINPUTWND);
			}
			if(wType&IMN_PRIV_UPDATEINPUTWND)
			{
				HWND hInputWnd=UIWnd_GetInputWnd(hUIWnd);
				if(hInputWnd) SendMessage(hInputWnd,UM_CALCSIZE,0,0);
			}
			if(wType&IMN_PRIV_CLOSEINPUTWND)
			{
				Helper_Trace("\nIMN_PRIVATE IMN_PRIV_CLOSEINPUTWND");
				KillTimer(hUIWnd,TI_DELAYINPUTWND);
				UIWnd_OpenInput(hUIWnd,FALSE,FALSE);
			}
			if(wType&IMN_PRIV_UPDATESTATUS)
			{
				HWND hStatusWnd=UIWnd_GetStatusWnd(hUIWnd);
				if(hStatusWnd) InvalidateRect(hStatusWnd,NULL,TRUE);
			}
			if(wType&IMN_PRIV_SKINCHANGE)
			{
				HGLOBAL hPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
				LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hPriv);
				if(g_SettingsL.bFullStatus)
				{
					SetWindowPos(lpUIPriv->hStatusWnd,NULL,0,0,g_SkinStatus.sizeFull.cx,g_SkinStatus.sizeFull.cy,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
					if(g_SkinStatus.hRgnFull) 
						Helper_SetWindowRgn(lpUIPriv->hStatusWnd,g_SkinStatus.hRgnFull,TRUE);
					else
						SetWindowRgn(lpUIPriv->hStatusWnd,0,TRUE);
				}else
				{
					SetWindowPos(lpUIPriv->hStatusWnd,NULL,0,0,g_SkinStatus.sizeShrink.cx,g_SkinStatus.sizeShrink.cy,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
					if(g_SkinStatus.hRgnShrink) 
						Helper_SetWindowRgn(lpUIPriv->hStatusWnd,g_SkinStatus.hRgnShrink,TRUE);
					else
						SetWindowRgn(lpUIPriv->hStatusWnd,0,TRUE);
				}
				InvalidateRect(lpUIPriv->hStatusWnd,NULL,TRUE);
				StatusWnd_EnsureVisible(lpUIPriv->hStatusWnd,0,0);
				if(lpUIPriv->hInputWnd)
				{
					if(!g_SkinInput.modeHerizontal.hRgnLeft && !g_SkinInput.modeHerizontal.hRgnRight &&g_hRgnInput)
					{
						SetWindowRgn(lpUIPriv->hInputWnd,0,TRUE);
						DeleteObject(g_hRgnInput);
						g_hRgnInput=0;
					}
					SendMessage(lpUIPriv->hInputWnd,UM_CALCSIZE,0,1);
				}
				UI_Config_Save(lpUIPriv);	//保存设置
				GlobalUnlock(hPriv);
			}
			if(wType&IMN_PRIV_SAVEUICFG)
			{
				HGLOBAL hPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
				LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hPriv);
				UI_Config_Save(lpUIPriv);	//保存设置
				GlobalUnlock(hPriv);
			}
			if(wType&IMN_PRIV_PLUGINCLOSE)
			{
				Plugin_Free();
			}
			break;
		}
		lRet=1;
		break;
	default:
		lRet=0;
		break;
    }
    return lRet;
}

LONG OnServerNotify(HWND hUIWnd,WPARAM wp,LPARAM lp,HIMC hIMC)
{
	LONG lRet=0;
	PMSGDATA pMsg=ISComm_OnSeverNotify(hUIWnd,wp,lp);
	if(wp==NT_COMPINFO)
	{
		char szHotKeyFile[MAX_PATH];
		char szBuf[10]={0};
		HWND hWndStatus=UIWnd_GetStatusWnd(hUIWnd);
		int i=0;
		
		if(ImageList_GetImageCount(g_himglstIcon)==3) ImageList_Remove(g_himglstIcon,2);
		if(ISComm_GetCompInfo()->hIcon)
			ImageList_AddMasked(g_himglstIcon,ISComm_GetCompInfo()->hIcon,ISComm_GetCompInfo()->crIconKey);
		if(hWndStatus) InvalidateRect(hWndStatus,NULL,TRUE);
		//加载特定的自定义状态及语句输入状态开关
		sprintf(szHotKeyFile,"%s\\hotkey_%s.txt",g_szDataPath,ISComm_GetCompInfo()->szName);
		GetPrivateProfileString("hotkey","umode","",szBuf,2,szHotKeyFile);
		g_cHotKeyUD=szBuf[0];
		if(g_cHotKeyUD==0)
		{
			g_cHotKeyUD=ISComm_GetCompInfo()->cWildChar;
		}
		
		g_SettingsG.bFastUMode=1;
		if(GetPrivateProfileString("settings","FastUMode",NULL,szBuf,10,szHotKeyFile))
			g_SettingsG.bFastUMode=atoi(szBuf);
		else if(g_cHotKeyUD<'a' || g_cHotKeyUD>'z') //不是字母，默认关闭
			g_SettingsG.bFastUMode=0;
		
		szBuf[0]=0;
		if(GetPrivateProfileString("hotkey","sentence","",szBuf,2,szHotKeyFile))
		{
			g_cHotKeySent=szBuf[0];
		}else
		{//默认设置为“；”，如果是编码则取消
			g_cHotKeySent=';';
			while(ISComm_GetCompInfo()->szCode[i])
			{
				if(ISComm_GetCompInfo()->szCode[i]==';')
				{
					g_cHotKeySent=0;
					break;
				}
				i++;
			}
		}
		Plugin_CompNameChange(ISComm_GetCompInfo()->szName);
		lRet=1;
	}else if(wp==NT_FLMINFO)
	{
		LOGFONT lf={0};
		GetObject(g_SkinInput.hFont,sizeof(LOGFONT),&lf);
		if(g_hPhoneticFont) DeleteObject(g_hPhoneticFont);
		lf.lfCharSet=DEFAULT_CHARSET;
		g_hPhoneticFont=NULL;
		if(ISComm_GetFlmInfo()->szAddFont[0])
		{//需要特殊字体
			strcpy(lf.lfFaceName,ISComm_GetFlmInfo()->szAddFont);
			g_hPhoneticFont=CreateFontIndirect(&lf);
		}
		lRet=1;
	}else if(wp==NT_COMPERROR)
	{//编码错误消息
		HWND hWndStatus=UIWnd_GetStatusWnd(hUIWnd);
		if(hWndStatus) InvalidateRect(hWndStatus,NULL,TRUE);
		lRet=1;
	}else if(wp==NT_UDICTINFO)
	{//用户词典名称
		Helper_Trace("\n用户词典=%s",ISComm_GetUserDict());
		if(g_hWndUDict) PostMessage(g_hWndUDict,UM_UDICT_CHANGED,0,0);
		lRet=1;
	}else if(wp==NT_SERVEREXIT)
	{//服务器退出
		HWND hStatusWnd=UIWnd_GetStatusWnd(hUIWnd);
		if(hStatusWnd) InvalidateRect(hStatusWnd,NULL,TRUE);
		if(hIMC) ImeSelect(hIMC,FALSE);
		lRet=1;
	}else if(wp==NT_SKINCHANGE)
	{//皮肤变更
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
		Skin_Load(g_szSkin);
		Skin_GetBtnRect(szPath,g_SettingsL.bFullStatus);
		UIWnd_UpdateCheckButton();
		PostMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SKINCHANGE);
		lRet=1;
	}else if(wp==NT_KEYIN)
	{//输入时返回的联想数据
		LPINPUTCONTEXT lpIMC=NULL;
		if(hIMC) lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
		if(lpIMC && lpIMC->fOpen)//有数据才处理
		{
			LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
			Helper_Trace("\n=====servernotify keyin");
			if(INST_CODING== lpCntxtPriv->inState && SBST_ASSOCIATE==lpCntxtPriv->sbState)
			{//保证当前状态是等待联想数据状态
				Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
				if(pMsg->sSize)
				{
					LPBYTE pbyData=NULL;
					memcpy(g_pbyData,pMsg->byData,pMsg->sSize);
					
					Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
					pbyData=g_pbyData;
					if(pbyData[0]==MKI_ASTCAND)
					{//诩组联想
						short iCand=0,sCount=0;
						Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
						pbyData++;
						memcpy(&sCount,pbyData,2);
						pbyData+=2;
						lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
						for(iCand=0;iCand<sCount;iCand++)
						{//枚举所有联想词组：词头长度(1BYTE)+词组
							lpCntxtPriv->ppbyCandInfo[iCand]=pbyData;
							pbyData+=pbyData[2]+3;
						}
						lpCntxtPriv->sCandCount=sCount;
					}
					if(pbyData-g_pbyData<pMsg->sSize && pbyData[0]==MKI_ASTENGLISH)
					{//英文联想
						BYTE i;
						Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
						pbyData++;
						lpCntxtPriv->pbyEnAstPhrase=pbyData;
						pbyData+=1+pbyData[0];
						lpCntxtPriv->sCandCount=*pbyData++;
						lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(lpCntxtPriv->sCandCount*sizeof(LPBYTE));
						for(i=0;i<lpCntxtPriv->sCandCount;i++)
						{
							lpCntxtPriv->ppbyCandInfo[i]=pbyData;
							pbyData+=pbyData[0]+1;
							pbyData+=pbyData[0]+1;						
						}
						if(!g_SettingsG.bTTSInput && g_SettingsL.bSound && lpCntxtPriv->sCandCount)
						{
							ISComm_TTS((LPCSTR)lpCntxtPriv->ppbyCandInfo[0]+1,lpCntxtPriv->ppbyCandInfo[0][0],MTTS_EN);
						}
					}
					if(pbyData-g_pbyData<pMsg->sSize && pbyData[0]==MKI_PHRASEREMIND)
					{//已有词组提示
						Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
						g_bTipShow=TRUE;
						strcpy(g_szTip,"系统词组:");
						strncpy(g_szTip+9,pbyData+2,pbyData[1]);
						g_szTip[9+pbyData[1]]=0;
						pbyData+=2+pbyData[1];
					}
					if(pbyData-g_pbyData<pMsg->sSize && pbyData[0]==MKI_ASTSENT)
					{//句子联想
						short sLen=0,iWord=0,sOffset=0;
						Helper_Trace("\n=====servernotify keyin  : FILE=%s,line=%d",__FILE__,__LINE__);
						pbyData++;
						memcpy(&sLen,pbyData,2);
						pbyData+=2;
						while(sOffset<sLen)
						{
							lpCntxtPriv->pbySentWord[iWord++]=pbyData;
							sOffset+=(pbyData[0]&0x80)?2:1;
							pbyData+=(pbyData[0]&0x80)?2:1;
						}
						lpCntxtPriv->pbySentWord[iWord]=pbyData;
						lpCntxtPriv->sSentWords=iWord;
						lpCntxtPriv->sSentCaret=0;
						lpCntxtPriv->sSentLen=sLen;
					}
				}
				if(g_bTipShow || lpCntxtPriv->sCandCount || lpCntxtPriv->sSentLen)
				{//有联想词组或有联想句子
					Helper_Trace("\n=====Update input window");
					SendMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
					SendMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SETDELAY);
				}else
				{//关闭窗口
					Helper_Trace("\n=====Close Input Window");
					SendMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
				}
			}else
			{
				Helper_Trace("\n=====IME state is not waitting for notify,inState=%d,sbState=%d",lpCntxtPriv->inState,lpCntxtPriv->sbState);
			}
			ImmUnlockIMCC(lpIMC->hPrivate);
		}else
		{
			Helper_Trace("\n=====Close Input Window lpIMC=0");
			SendMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
		}
		if(hIMC) ImmUnlockIMC(hIMC);
		lRet=1;
	}
	return lRet;
}

BOOL KeyIn_IsCoding(LPCONTEXTPRIV lpCntxtPriv);

/**********************************************************************/
/*                                                                    */
/* UIWndProc()                                                   */
/*                                                                    */
/* IME UI window procedure                                            */
/*                                                                    */
/**********************************************************************/
LRESULT WINAPI UIWndProc(HWND hUIWnd,	UINT message,WPARAM wParam,	LPARAM lParam)
{
	static DWORD dwInputTime=0;
    LONG           lRet = 0L;
    HIMC hIMC = (HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
	Helper_Trace("\n###########HUIWND=%08X,msgID=%d,wParam=%d,lParam=%d",hUIWnd,message,wParam,lParam);
	if(message==ISComm_GetCommMsgID())
		return OnServerNotify(hUIWnd,wParam,lParam,hIMC);
    //**************************************************************
    // Even if there is no current UI. these messages should not be pass to 
    // DefWindowProc().
    //**************************************************************
    if (!hIMC)
    { 
		Helper_Trace("\nUIWndProc Can't Get hIMC Msg=%08X,wp=%08X,lp=%08X",message,wParam,lParam);
    }
    switch (message)
    {
	case WM_CREATE://做窗口的初始化工作
		lRet=UIWnd_Create(hUIWnd);
		break;
	case WM_DESTROY:
		UIWnd_Destroy(hUIWnd);
		break;
    case WM_PAINT:
        lRet= UIWnd_Paint(hUIWnd);
		break;
	case WM_IME_NOTIFY:
		lRet = UIWnd_Notify(hIMC, hUIWnd,wParam,lParam);
		break; 
    case WM_IME_STARTCOMPOSITION:
		{//间隔大于10秒时，忽略时间间隔
			DWORD dwTime=GetTickCount();
			if(dwTime-dwInputTime>10000)
				dwInputTime=dwTime;
		}
		break;
    case WM_IME_ENDCOMPOSITION:
		{//计算出输入花费的时间
			DWORD dwEndTime=GetTickCount();
			g_dwTimeSpan+=dwEndTime-dwInputTime;
			dwInputTime=dwEndTime;
		}
		if(g_bDelayPhontic)
		{
			g_bDelayPhontic=FALSE;
			SetTimer(hUIWnd,TI_DELAYPASTE,300,NULL);
		}
	case WM_IME_COMPOSITION:
		break;
	case WM_IME_SELECT:
		if(wParam && hIMC){
			LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
			if(lpIMC)
			{
				LPCONTEXTPRIV lpCntxePriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
				if(lpCntxePriv)
				{
					lpCntxePriv->hUiWnd=hUIWnd;
					ImmUnlockIMCC(lpIMC->hPrivate);
				}
				ImmUnlockIMC(hIMC);
			}
		}
		break;
	case WM_IME_SETCONTEXT:
		if(hIMC)
		{//为输入法设置了一个新的上下文,更新状态
			Helper_Trace("\nUIWndProc WM_IME_SETCONTEXT");
			g_bCaps=GetKeyState(VK_CAPITAL)&0x0001;
			ImmSetOpenStatus(hIMC,wParam);
			if(wParam)
			{
				if(hIMC)
				{
					LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
					if(lpIMC)
					{
						LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
						//更新了目标窗口，新窗口可能不能正常光标跟随
						g_ptInputPos.x=g_ptInputPos.y=-1;
						UIWnd_OpenInput(hUIWnd,KeyIn_IsCoding(lpCntxtPriv),FALSE);
						lpCntxtPriv->hUiWnd=hUIWnd;
						ImmUnlockIMCC(lpIMC->hPrivate);
						Helper_Trace("\n##########WM_IME_SETCONTEXT,hWnd=0x%08X",lpIMC->hWnd);
					}
					ImmUnlockIMC(hIMC);
				}
				UIWnd_OpenStatus(hUIWnd,!g_SettingsL.bHideStatus);
			}else
			{
				UIWnd_OpenStatus(hUIWnd,FALSE);
				UIWnd_OpenInput(hUIWnd,FALSE,FALSE);
				UIWnd_OpenSpChar(hUIWnd,FALSE);
			}
			lRet=1;
		}
		break;
    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
	case WM_TIMER:
		if(wParam==TI_DELAYINPUTWND && hIMC)
		{//延时关闭输入窗口
			POINT pt={0};
			HWND  hWndInput=UIWnd_GetInputWnd(hUIWnd);
			RECT  rcInput;
			GetCursorPos(&pt);
			GetWindowRect(hWndInput,&rcInput);
			if(!PtInRect(&rcInput,pt))
			{
				LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
				UIWnd_OpenInput(hUIWnd,FALSE,FALSE);
				//将联想状态还原为编码状态，以便于数字及符号输入
				ClearPrivateContext(lpIMC,CPC_ALL&~CPC_INPUT);
				ImmUnlockIMC(hIMC);
				KillTimer(hUIWnd,wParam);
			}
		}else if(wParam==TI_DELAYPASTE)
		{//延时模拟按下Ctrl+V,不延时在Word中不能正确输入
			KillTimer(hUIWnd,wParam);
			keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),0,0);
			keybd_event('V',MapVirtualKey('V',0),0,0);
			keybd_event('V',MapVirtualKey('V',0),KEYEVENTF_KEYUP,0);
			keybd_event(VK_CONTROL,MapVirtualKey(VK_CONTROL,0),KEYEVENTF_KEYUP,0);
		}
		lRet=1;
		break;
	case UM_PLUGIN://插件定义消息
		switch(wParam)
		{
		case PM_GETSTATEWND:
			lRet=(LRESULT)UIWnd_GetStatusWnd(hUIWnd);			
			break;
		case PM_GETINPUTWND:
			lRet=(LRESULT)UIWnd_GetInputWnd(hUIWnd);
			break;
		case PM_PLUGINCLOSE:
			Plugin_Free();
			lRet=1;
			break;
		}
		break;
	default:
		if(!Helper_IsIMEMessage(message))
			lRet=DefWindowProc(hUIWnd,message,wParam,lParam);
		break;
    }
    return lRet;
}

