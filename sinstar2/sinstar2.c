// sinstar2.cpp : Defines the entry point for the DLL application.
//

#include "sinstar2.h"
#include <time.h>

#pragma data_seg(".sinstar2")
//使用共享内存方式使设置信息在所有输入法进程中共享
SETTINGSG	g_SettingsG={0};		//输入法全局设置
char		g_szDataPath[MAX_PATH];	//程序的数据所在路径
int			g_ImeCount=0;			//输入法打开计数
char		g_szSkin[MAX_PATH]={0};	//当前打开的skin
#pragma data_seg()

HINSTANCE	g_hInst=NULL;
char		g_szImeTitle[200]={0};	//输入法名称	
HIMAGELIST	g_himglstIcon=0;		//编码图标列表
BOOL		g_bCaps=FALSE;			//大写状态标志
HFONT		g_hPhoneticFont=NULL;	//音标字体
HWND		g_hWndToolTip=NULL;		//提示信息窗口
HWND		g_hWndMsg=NULL;			//消息显示窗口
COMPMODE	g_CompMode;				//编码模式
SPECIALDATA g_SpCharData={0};		//特殊字符表
SETTINGSL	g_SettingsL={0};		//本地设置
BOOL		g_bDelayPhontic=FALSE;	//延时输入音标信息标志
BOOL		g_bTempSpell=FALSE;		//临时拼音标志
BOOL		g_bTipShow=FALSE;		//在重码区显示提示信息
char		g_szTip[500]={0};		//在重码区显示提示信息缓冲区
char		g_szPYBiHua[50]={0};	//拼音输入时的笔画辅助码

char		g_cHotKeyUD=0;			//切换到用户定义的与编码相关快捷键
char		g_cHotKeySent=0;		//切换到语句输入的与编码相关快捷键
POINT		g_ptStatusDockMode={0,0};//状态栏显示位置信息，-1,0,1代表 (上/左)，中，(下/右) 3种状态
POINT		g_ptInputPos={-1,-1};	//保存输入窗口当前位置

HRGN		g_hRgnInput=0;			//输入窗口的区域
//速度统计
DWORD		g_dwTimeSpan=0;			//输入编码花费时间
DWORD		g_dwInputCount=0;		//输入字符数量

LPBYTE		g_pbyData=NULL;			//数据接收缓冲区

//窗口过程回调
LRESULT WINAPI UIWndProc(HWND hWnd,	UINT message,WPARAM wParam,	LPARAM lParam);
LRESULT WINAPI InputWnd_Proc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT WINAPI StatusWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT	WINAPI SpecialCharWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

//*****************************************************
//	工具函数：向当前有焦点的窗口输入字符串
//	2004/5/21 修改在FrantPage等软件中InputString()输入ASCII失败的问题
//	将利用WM_IME_CHAR输入改为IMM方式输入
//******************************************************
void InputString(LPTSTR pszText,HWND hWnd)
{
	HWND hFocus=hWnd?hWnd:GetFocus();
	HIMC hIMC=ImmGetContext(hFocus);
	if(hIMC)
	{
		LPINPUTCONTEXT lpIMC=NULL;
		if(lpIMC=ImmLockIMC(hIMC))
		{
			if(lpIMC->fOpen&&strlen(pszText)<MAX_INPUT)
			{
				LPCOMPOSITIONSTRING lpCompStr=NULL;
				if(lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr))
				{
					BYTE byBack[MAX_INPUT];
					DWORD	dwOldOffset,dwOldLen;
					//copy ime data to back buffer
					memcpy(byBack,GETLPCOMPSTR(lpCompStr),MAX_INPUT);
					dwOldLen=lpCompStr->dwResultStrLen;
					dwOldOffset=lpCompStr->dwResultStrOffset;
					//set ime data
					lpCompStr->dwResultStrOffset=lpCompStr->dwCompStrOffset;
					strcpy(GETLPRESULTSTR(lpCompStr),pszText);
					lpCompStr->dwResultStrLen=strlen(pszText);
					ImmUnlockIMCC(lpIMC->hCompStr);
					ImmUnlockIMC(hIMC);
					MyGenerateMessage(hIMC,WM_IME_STARTCOMPOSITION, 0, 0);
					MyGenerateMessage(hIMC,WM_IME_COMPOSITION, 0, GCS_INPUT);
					MyGenerateMessage(hIMC,WM_IME_ENDCOMPOSITION, 0, 0);
					//data restore
					lpIMC=ImmLockIMC(hIMC);
					lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
					lpCompStr->dwResultStrOffset=dwOldOffset;
					lpCompStr->dwResultStrLen=dwOldLen;
					memcpy(GETLPCOMPSTR(lpCompStr),byBack,MAX_INPUT);
					ImmUnlockIMCC(lpIMC->hCompStr);
				}
			}else
			{
				int i=0;
				while(pszText[i])
				{
					if(pszText[i]&0x80)
					{
						WPARAM wParam=MAKEWORD(pszText[i+1],pszText[i]);
						SendMessage(hFocus,WM_IME_CHAR,wParam,0);
						i+=2;
					}else
					{
						SendMessage(hFocus,WM_CHAR,pszText[i],0);
						i++;
					}
				}
			}
			ImmUnlockIMC(hIMC);
		}
	}
	ImmReleaseContext(hFocus,hIMC);
}

void SoundPlay(LPCTSTR pszSound)
{
	if(g_SettingsG.nSoundAlert==1)
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\sound\\%s.wav",g_szDataPath,pszSound);
		PlaySound(szPath,NULL,SND_ASYNC|SND_FILENAME);
	}else if(g_SettingsG.nSoundAlert==2)
	{
		MessageBeep(1000);
	}
}

//----------------------------------------------------------------------------
// GB2312码转GBK码
// 中华人民共和国 --> 中華人民共和國
int  GB2GIB5(char *szBuf,int nStrLen)
{
	int nRet=0;
	if(nStrLen==-1) nStrLen=strlen(szBuf);
	if(nStrLen>0)
	{
		DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
		int nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
		if(nReturn)
		{
			char *pcBuf = (char*)malloc(nReturn + 1);
			wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
			LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
			if(nReturn<=nStrLen)
			{
				strncpy(szBuf, pcBuf, nReturn);
				nRet=nReturn;
			}
			free(pcBuf);
		}
	}
	return nRet;
}

void GetImeTitle()
{
	char szPath[MAX_PATH],*p=g_szImeTitle;
	GetModuleFileName(g_hInst,szPath,MAX_PATH);
	_splitpath(szPath,NULL,NULL,g_szImeTitle,NULL);
	//将文件名转换成小写
	while(*p)
	{
		if(*p>='A'&& *p<='Z') *p+=0x20;
		p++;
	}
}

void Config_GetIni(char *szPath)
{
	sprintf(szPath,"%s\\config.ini",g_szDataPath);
}

//*****************************************************************
//	向HIMC发送消息
//	消息数据保存在lpIMC->dwNumMsgBuf中
//	每次发消息前需要分配消息空间
//*****************************************************************
BOOL MyGenerateMessage(HIMC hIMC, UINT msg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet=FALSE;
    LPINPUTCONTEXT lpIMC= ImmLockIMC(hIMC);
	if(lpIMC == NULL)
		return FALSE;    
    
    if (IsWindow(lpIMC->hWnd))
    {
        LPTRANSMSG lpTransMsg;
        if (!(lpIMC->hMsgBuf = ImmReSizeIMCC(lpIMC->hMsgBuf,(lpIMC->dwNumMsgBuf+1) * sizeof(TRANSMSG))))
            goto error;
		
        if (!(lpTransMsg = (LPTRANSMSG)ImmLockIMCC(lpIMC->hMsgBuf)))
            goto error;
		
        lpTransMsg += (lpIMC->dwNumMsgBuf);
        lpTransMsg->message=msg;
		lpTransMsg->wParam=wParam;
		lpTransMsg->lParam=lParam;
        lpIMC->dwNumMsgBuf++;

        ImmUnlockIMCC(lpIMC->hMsgBuf);
        ImmGenerateMessage(hIMC);//将消息发送到IME，IME再决定是自己处理还是继续发给应用程序
		bRet=TRUE;
    }
error:
	ImmUnlockIMC(hIMC); 
	return bRet;
}

//************************************************************
//	向消息缓冲区填充消息
//	LPDWORD lpdwTransKey:消息缓冲区,由系统提供,第一个双字为可用消息空间数
//	UINT *uNumTranMsgs:当前消息数
//	UINT msg,WPARAM wParam,LPARAM lParam：消息数据
//	remark:该函数只在ImeToAsciiEx中调用
//************************************************************
BOOL MyGenerateMessageToTransKey(LPDWORD lpdwTransKey,
								 UINT *uNumTranMsgs,
								 UINT msg, 
								 WPARAM wParam, 
								 LPARAM lParam) 
{
    LPTRANSMSGLIST lpTransMsg=(LPTRANSMSGLIST)lpdwTransKey;
	TRANSMSG transMsg={msg,wParam,lParam};
	if (lpTransMsg->uMsgCount <= *uNumTranMsgs)
        return FALSE;
	lpTransMsg->TransMsg[*uNumTranMsgs]=transMsg;
	(*uNumTranMsgs)++;
    return TRUE;
}

//************************************************************
//	注册用户界面窗口类
//************************************************************
BOOL ImeClass_Register( HINSTANCE hInstance )
{
    WNDCLASSEX wc;
	
    //
    // register class of UI window.
    //
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_INPUTSTAR | CS_IME;
    wc.lpfnWndProc    = UIWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 2 * sizeof(LONG_PTR);
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon          = NULL;
    wc.lpszMenuName   = (LPTSTR)NULL;
    wc.lpszClassName  = CLSNAME_UI;
    wc.hbrBackground  = NULL;
    wc.hIconSm        = NULL;
	
    if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
        return FALSE;
	
    //
    // register class of input window.
    //
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_INPUTSTAR | CS_IME ;
	if(Helper_IsLayerSupport() && g_SettingsG.bShadeInput)	wc.style|=0x00020000;

    wc.lpfnWndProc    = InputWnd_Proc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(LONG_PTR);
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon          = NULL;
    wc.lpszMenuName   = (LPSTR)NULL;
    wc.lpszClassName  = CLSNAME_INPUT;
    wc.hbrBackground  = NULL;
    wc.hIconSm        = NULL;	

    if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
        return FALSE;
    //
    // register class of status window.
    //
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_INPUTSTAR | CS_IME;
	if(Helper_IsLayerSupport() && g_SettingsG.bShadeStatus)	wc.style|=0x00020000;
    wc.lpfnWndProc    = StatusWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(LONG_PTR);
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon          = NULL;
    wc.lpszMenuName   = (LPSTR)NULL;
    wc.lpszClassName  = CLSNAME_STATUS;
    wc.hbrBackground  = NULL;
    wc.hIconSm        = NULL;
	
    if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
        return FALSE;

    //
    // register class of spchar window.
    //
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style          = CS_INPUTSTAR | CS_IME;
    wc.lpfnWndProc    = SpecialCharWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon          = NULL;
    wc.lpszMenuName   = (LPSTR)NULL;
    wc.lpszClassName  = CLSNAME_SPCHAR;
    wc.hbrBackground  = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hIconSm        = NULL;
	
    if( !RegisterClassEx( (LPWNDCLASSEX)&wc ) )
        return FALSE;
	return ToolTipWnd_Register(hInstance);
}

//**************************************************************
//	注销输入之星注册的窗口类
//**************************************************************
void ImeClass_Unregister(HINSTANCE hInstance)
{
	UnregisterClass(CLSNAME_UI    ,hInstance);
	UnregisterClass(CLSNAME_INPUT    ,hInstance);
	UnregisterClass(CLSNAME_STATUS,hInstance);
	UnregisterClass(CLSNAME_SPCHAR,hInstance);
	ToolTipWnd_Unregister(hInstance);
}

BOOL GetMyPath()
{
	char szConfig[MAX_PATH];
	if(ISComm_GetConfig(szConfig))
	{
		GetPrivateProfileString("client",g_szImeTitle,"",g_szDataPath,MAX_PATH,szConfig);
		Helper_Trace("\ndatapath=%s",g_szDataPath);
		if(g_szDataPath[0]==0)
		{
			Helper_Trace("\nCan't Find data path");
			return FALSE;
		}else
		{
			if(!g_szSkin[0])
			{
				char szSkinPath[MAX_PATH];
				strcpy(g_szSkin,"Default");
				Config_GetIni(szConfig);
				GetPrivateProfileString("IME","skin","Default",g_szSkin,50,szConfig);
				if(strcmp(g_szSkin,SKIN_RANDOM)==0)
				{//选择一款随机皮肤
					char szSkins[ID_SKIN_END-ID_SKIN_BEGIN+1][MAX_PATH];
					int nSkins=Skin_Enum(szSkins,ID_SKIN_END-ID_SKIN_BEGIN+1,g_szDataPath);
					if(nSkins==0) return FALSE;
					strcpy(g_szSkin,szSkins[rand()%nSkins]);
				}
				//保证初始化时能够载入一个皮肤
				sprintf(szSkinPath,"%s\\Skins\\%s\\config.ini",g_szDataPath,g_szSkin);
				if(GetPrivateProfileInt("InputWindow","left",-1,szSkinPath)==-1) strcpy(g_szSkin,"Default");
			}
			return TRUE;
		}
	}else
	{
		return FALSE;
	}
}


HANDLE g_hMutex=NULL;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
			g_pszLogFile="c:\\sinstar2.log";
#endif//_DEBUG
			g_hInst=(HINSTANCE)hModule;
			GetImeTitle();
			srand(time(NULL));
			if(!GetMyPath()) return FALSE;
			g_pbyData=(LPBYTE)malloc(MAX_BUF_ACK);
			if(!g_pbyData)
			{
				Helper_Trace("\nrequired memory can't be located!");
				return FALSE;
			}
			Config_Load();
			{
				char szMutexName[200];
				sprintf(szMutexName,"mutex_sinstar2_%s",g_szImeTitle);
				g_hMutex=CreateMutex(NULL,FALSE,szMutexName);
				Helper_Trace("\nCreate Mutex:%s",szMutexName);
			}
			if(!ImeClass_Register((HINSTANCE)hModule)) return FALSE;
			if(!Skin_Load(g_szSkin))
			{
				BOOL bSkinLoaded=FALSE;
				if(stricmp(g_szSkin,"default")!=0)
				{
					Skin_Free();
					strcpy(g_szSkin,"default");
					bSkinLoaded=Skin_Load(g_szSkin);
				}
				if(!bSkinLoaded)
				{
					Helper_Trace("\nload default skin error!");
					return FALSE;
				}
			}
			{//加载软键盘窗口资源
				char szConfig[MAX_PATH],szPath[MAX_PATH];
				sprintf(szConfig,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
				Skin_GetBtnRect(szConfig,g_SettingsL.bFullStatus);
				sprintf(szPath,"%s\\spskin",g_szDataPath);
				sprintf(szConfig,"%s\\spskin\\config.ini",g_szDataPath);
				if(!Skin_LoadSpCharWnd(szPath,szConfig))
				{
					Helper_Trace("\nload softkeybroad error!");
					return FALSE;
				}
			}
			g_himglstIcon=ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,0,0);
			{
				HBITMAP hIcon=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_COMPICON));
				ImageList_AddMasked(g_himglstIcon,hIcon,RGB(255,0,255));
				DeleteObject(hIcon);
			}
			if(g_SettingsG.bShowOpTip) Tips_Load();
			g_ImeCount++;
			break;
		case DLL_PROCESS_DETACH:
			g_ImeCount--;
			CloseHandle(g_hMutex);
			if(g_pbyData)
			{
				free(g_pbyData);
				g_pbyData=NULL;
			}
			if(g_hPhoneticFont) DeleteObject(g_hPhoneticFont);
			Tips_Free();
			ImeClass_Unregister((HINSTANCE)hModule);
			ImageList_Destroy(g_himglstIcon);
			Skin_Free();
			Skin_FreeSpCharWnd();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
    }
    return TRUE;
}

