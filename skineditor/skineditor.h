// skineditor.h : main header file for the SKINEDITOR application
//

#if !defined(AFX_SKINEDITOR_H__6B2CAF2A_18E5_484B_88B4_CB2CC160B213__INCLUDED_)
#define AFX_SKINEDITOR_H__6B2CAF2A_18E5_484B_88B4_CB2CC160B213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSkineditorApp:
// See skineditor.cpp for the implementation of this class
//

class CSkineditorApp : public CWinApp
{
public:
	CSkineditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkineditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSkineditorApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINEDITOR_H__6B2CAF2A_18E5_484B_88B4_CB2CC160B213__INCLUDED_)
