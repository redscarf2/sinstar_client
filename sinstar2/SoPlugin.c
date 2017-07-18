// SoPlugin.cpp: implementation of the CSoPlugin class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include "SoPlugin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HWND UIWnd_GetStatusWnd(HWND);
HWND UIWnd_GetInputWnd(HWND);

typedef BOOL (*SPI_INIT)(HWND hUIWnd);
typedef BOOL (*SPI_HOTKEYINQUIRE)(UINT uKey);
typedef BOOL (*SPI_HOTKEYPROCESS)(UINT uVKey,UINT uScanCode);
typedef void (*SPI_STATECHAGNE)(UINT CompMode,UINT uState,UINT uSubState,BOOL bTempSpell);
typedef void (*SPI_COMPNAMECHAGNE)(LPCTSTR pszCompName);
typedef void (*SPI_TEXTINPUT)(LPCTSTR pszText,int nWordLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar);
typedef void (*SPI_CANDFOCUS)(LPCTSTR pszText,int nWordLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar);
typedef void (*SPI_CANDLOSEFOCUS)();

struct tagSP_INFO
{
	HINSTANCE	hPlugin;
	SPI_HOTKEYINQUIRE funHotkeyInquire;
	SPI_HOTKEYPROCESS funHotkeyProcess;
	SPI_STATECHAGNE	  funStateChange;
	SPI_COMPNAMECHAGNE funCompNameChange;
	SPI_TEXTINPUT	  funTextInput;
	SPI_CANDFOCUS	  funCandFocus;
	SPI_CANDLOSEFOCUS funCandLoseFocus;
} g_spInfo={0};

BOOL Plugin_Load(LPCTSTR pszPluginFile,HWND hUiWnd)
{
	Plugin_Free();
	g_spInfo.hPlugin=LoadLibrary(pszPluginFile);
	if(g_spInfo.hPlugin)
	{
		SPI_INIT funInit=(SPI_INIT)GetProcAddress(g_spInfo.hPlugin,"Plugin_Init");
		if(!funInit || !funInit(hUiWnd))
		{
			FreeLibrary(g_spInfo.hPlugin);
			g_spInfo.hPlugin=NULL;
		}else
		{
			g_spInfo.funHotkeyInquire=(SPI_HOTKEYINQUIRE)GetProcAddress(g_spInfo.hPlugin,"Plugin_HotkeyInquire");
			g_spInfo.funHotkeyProcess=(SPI_HOTKEYPROCESS)GetProcAddress(g_spInfo.hPlugin,"Plugin_HotkeyProcess");
			g_spInfo.funStateChange=(SPI_STATECHAGNE)GetProcAddress(g_spInfo.hPlugin,"Plugin_StateChange");
			g_spInfo.funCompNameChange=(SPI_COMPNAMECHAGNE)GetProcAddress(g_spInfo.hPlugin,"Plugin_CompNameChange");
			g_spInfo.funTextInput=(SPI_TEXTINPUT)GetProcAddress(g_spInfo.hPlugin,"Plugin_TextInput");
			g_spInfo.funCandFocus=(SPI_CANDFOCUS)GetProcAddress(g_spInfo.hPlugin,"Plugin_CandFocus");
			g_spInfo.funCandLoseFocus=(SPI_CANDFOCUS)GetProcAddress(g_spInfo.hPlugin,"Plugin_CandLoseFocus");
		}
	}
	return g_spInfo.hPlugin!=NULL;
}

void		Plugin_Free()
{
	if(g_spInfo.hPlugin)
	{
		FreeLibrary(g_spInfo.hPlugin);
		memset(&g_spInfo,0,sizeof(g_spInfo));
	}
}

BOOL		Plugin_HotkeyInquire(UINT uKey)
{
	BOOL bRet=FALSE;
	if(g_spInfo.funHotkeyInquire) bRet=g_spInfo.funHotkeyInquire(uKey);
	return bRet;
}

BOOL	Plugin_HotkeyProcess(UINT uVKey,UINT uScanCode)
{
	BOOL bRet=FALSE;
	if(g_spInfo.funHotkeyProcess) bRet=g_spInfo.funHotkeyProcess(uVKey,uScanCode);
	return bRet;
}

void		Plugin_StateChange(UINT CompMode,UINT uState,UINT uSubState,BOOL bTempSpell)
{
	if(g_spInfo.funStateChange) g_spInfo.funStateChange(CompMode,uState,uSubState,bTempSpell);
}

void		Plugin_CompNameChange(LPCTSTR pszCompName)
{
	if(g_spInfo.funCompNameChange) g_spInfo.funCompNameChange(pszCompName);
}

void		Plugin_TextInput(LPCTSTR pszText,int nWordLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar)
{
	if(g_spInfo.funTextInput) g_spInfo.funTextInput(pszText,nWordLen,pszComp,nCompLen,bWildChar);
}

void		Plugin_CandFocus(LPCTSTR pszCand,int nCandLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar)
{
	if(g_spInfo.funCandFocus) g_spInfo.funCandFocus(pszCand,nCandLen,pszComp,nCompLen,bWildChar);
}

void Plugin_CandLoseFocus()
{
	if(g_spInfo.funCandLoseFocus) g_spInfo.funCandLoseFocus();
}