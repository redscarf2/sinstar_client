// FindCaller.h : main header file for the FINDCALLER application
//

#if !defined(AFX_FINDCALLER_H__F94728A7_E773_4B54_BD9D_3C714C174547__INCLUDED_)
#define AFX_FINDCALLER_H__F94728A7_E773_4B54_BD9D_3C714C174547__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFindCallerApp:
// See FindCaller.cpp for the implementation of this class
//

class CFindCallerApp : public CWinApp
{
public:
	CFindCallerApp();
	
	CString m_strFile;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindCallerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFindCallerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CFindCallerApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDCALLER_H__F94728A7_E773_4B54_BD9D_3C714C174547__INCLUDED_)
