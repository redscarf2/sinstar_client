// FindCaller.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FindCaller.h"
#include "FindCallerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindCallerApp

BEGIN_MESSAGE_MAP(CFindCallerApp, CWinApp)
	//{{AFX_MSG_MAP(CFindCallerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindCallerApp construction

CFindCallerApp::CFindCallerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFindCallerApp object

CFindCallerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFindCallerApp initialization

BOOL CFindCallerApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey("SetoutSoft");
	CCommandLineInfo cmdLineInfo;
	ParseCommandLine(cmdLineInfo);
	if(cmdLineInfo.m_nShellCommand==CCommandLineInfo::FileOpen)
		m_strFile=cmdLineInfo.m_strFileName;

	CFindCallerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
