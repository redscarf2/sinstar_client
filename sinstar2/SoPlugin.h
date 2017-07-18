// SoPlugin.h: interface for the CSoPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOPLUGIN_H__F58EFA5D_1C65_4458_8561_00C7D702EF04__INCLUDED_)
#define AFX_SOPLUGIN_H__F58EFA5D_1C65_4458_8561_00C7D702EF04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "pluginmsg.h"

BOOL	Plugin_Load(LPCTSTR pszPluginFile,HWND hUiWnd);
void	Plugin_Free();
BOOL	Plugin_HotkeyInquire(UINT uKey);
BOOL	Plugin_HotkeyProcess(UINT uVKey,UINT uScanCode);
void	Plugin_StateChange(UINT CompMode,UINT uState,UINT uSubState,BOOL bTempSpell);
void	Plugin_CompNameChange(LPCTSTR pszCompName);
void	Plugin_TextInput(LPCTSTR pszText,int nWordLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar);
void	Plugin_CandFocus(LPCTSTR pszCand,int nWordLen,LPCTSTR pszComp,int nCompLen,BOOL bWildChar);
void	Plugin_CandLoseFocus();

#endif // !defined(AFX_SOPLUGIN_H__F58EFA5D_1C65_4458_8561_00C7D702EF04__INCLUDED_)
