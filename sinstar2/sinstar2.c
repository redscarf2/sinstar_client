// sinstar2.cpp : Defines the entry point for the DLL application.
//

#include "sinstar2.h"
#include <time.h>

#pragma data_seg(".sinstar2")
//ʹ�ù����ڴ淽ʽʹ������Ϣ���������뷨�����й���
SETTINGSG	g_SettingsG={0};		//���뷨ȫ������
char		g_szDataPath[MAX_PATH];	//�������������·��
int			g_ImeCount=0;			//���뷨�򿪼���
char		g_szSkin[MAX_PATH]={0};	//��ǰ�򿪵�skin
#pragma data_seg()

HINSTANCE	g_hInst=NULL;
char		g_szImeTitle[200]={0};	//���뷨����	
HIMAGELIST	g_himglstIcon=0;		//����ͼ���б�
BOOL		g_bCaps=FALSE;			//��д״̬��־
HFONT		g_hPhoneticFont=NULL;	//��������
HWND		g_hWndToolTip=NULL;		//��ʾ��Ϣ����
HWND		g_hWndMsg=NULL;			//��Ϣ��ʾ����
COMPMODE	g_CompMode;				//����ģʽ
SPECIALDATA g_SpCharData={0};		//�����ַ���
SETTINGSL	g_SettingsL={0};		//��������
BOOL		g_bDelayPhontic=FALSE;	//��ʱ����������Ϣ��־
BOOL		g_bTempSpell=FALSE;		//��ʱƴ����־
BOOL		g_bTipShow=FALSE;		//����������ʾ��ʾ��Ϣ
char		g_szTip[500]={0};		//����������ʾ��ʾ��Ϣ������
char		g_szPYBiHua[50]={0};	//ƴ������ʱ�ıʻ�������

char		g_cHotKeyUD=0;			//�л����û�������������ؿ�ݼ�
char		g_cHotKeySent=0;		//�л������������������ؿ�ݼ�
POINT		g_ptStatusDockMode={0,0};//״̬����ʾλ����Ϣ��-1,0,1���� (��/��)���У�(��/��) 3��״̬
POINT		g_ptInputPos={-1,-1};	//�������봰�ڵ�ǰλ��

HRGN		g_hRgnInput=0;			//���봰�ڵ�����
//�ٶ�ͳ��
DWORD		g_dwTimeSpan=0;			//������뻨��ʱ��
DWORD		g_dwInputCount=0;		//�����ַ�����

LPBYTE		g_pbyData=NULL;			//���ݽ��ջ�����

//���ڹ��̻ص�
LRESULT WINAPI UIWndProc(HWND hWnd,	UINT message,WPARAM wParam,	LPARAM lParam);
LRESULT WINAPI InputWnd_Proc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT WINAPI StatusWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT	WINAPI SpecialCharWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

//*****************************************************
//	���ߺ�������ǰ�н���Ĵ��������ַ���
//	2004/5/21 �޸���FrantPage�������InputString()����ASCIIʧ�ܵ�����
//	������WM_IME_CHAR�����ΪIMM��ʽ����
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
// GB2312��תGBK��
// �л����񹲺͹� --> ���A���񹲺͇�
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
	//���ļ���ת����Сд
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
//	��HIMC������Ϣ
//	��Ϣ���ݱ�����lpIMC->dwNumMsgBuf��
//	ÿ�η���Ϣǰ��Ҫ������Ϣ�ռ�
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
        ImmGenerateMessage(hIMC);//����Ϣ���͵�IME��IME�پ������Լ������Ǽ�������Ӧ�ó���
		bRet=TRUE;
    }
error:
	ImmUnlockIMC(hIMC); 
	return bRet;
}

//************************************************************
//	����Ϣ�����������Ϣ
//	LPDWORD lpdwTransKey:��Ϣ������,��ϵͳ�ṩ,��һ��˫��Ϊ������Ϣ�ռ���
//	UINT *uNumTranMsgs:��ǰ��Ϣ��
//	UINT msg,WPARAM wParam,LPARAM lParam����Ϣ����
//	remark:�ú���ֻ��ImeToAsciiEx�е���
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
//	ע���û����洰����
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
//	ע������֮��ע��Ĵ�����
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
				{//ѡ��һ�����Ƥ��
					char szSkins[ID_SKIN_END-ID_SKIN_BEGIN+1][MAX_PATH];
					int nSkins=Skin_Enum(szSkins,ID_SKIN_END-ID_SKIN_BEGIN+1,g_szDataPath);
					if(nSkins==0) return FALSE;
					strcpy(g_szSkin,szSkins[rand()%nSkins]);
				}
				//��֤��ʼ��ʱ�ܹ�����һ��Ƥ��
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
			{//��������̴�����Դ
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

