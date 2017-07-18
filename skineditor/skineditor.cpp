// skineditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "skineditor.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "skineditorDoc.h"
#include "LeftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkineditorApp

BEGIN_MESSAGE_MAP(CSkineditorApp, CWinApp)
	//{{AFX_MSG_MAP(CSkineditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkineditorApp construction

CSkineditorApp::CSkineditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSkineditorApp object

CSkineditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSkineditorApp initialization

BOOL CSkineditorApp::InitInstance()
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

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Sinstar2 Skin Editor"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	char szFileName[MAX_PATH];
	GetModuleFileName(NULL,szFileName,MAX_PATH);
	char szIniFile[MAX_PATH];
	GetSystemDirectory(szIniFile,MAX_PATH);
	strcat(szIniFile,"\\sinstar2.ini");
	WritePrivateProfileString("SKIN","editor",szFileName,szIniFile);
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_SKINEDTYPE,
		RUNTIME_CLASS(CSkineditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CLeftView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{	
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWNORMAL);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CButton	m_btn;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDOK, m_btn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSkineditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSkineditorApp message handlers

#include "dirdialog.h"
void CSkineditorApp::OnFileOpen() 
{
	CDirDialog dirPick;
	dirPick.m_strWindowTitle="ѡ��Ƥ���ļ���";
	if(dirPick.DoBrowse(m_pMainWnd))
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\config.ini",dirPick.m_strPath);
		if(GetFileAttributes(szPath)!=0xFFFFFFFF)
		{
			OpenDocumentFile(szPath);
		}
		else
			AfxMessageBox(IDS_INVALIDSKINPATH,MB_OK|MB_ICONSTOP,0);
	}
}

void CSkineditorApp::OnFileNew() 
{
	CDirDialog dirPick;
	dirPick.m_strWindowTitle="ѡ��Ƥ���ļ���";
	if(dirPick.DoBrowse(m_pMainWnd))
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\config.ini",dirPick.m_strPath);
		if(GetFileAttributes(szPath)!=0xFFFFFFFF)
		{
			AfxMessageBox(IDS_EXISTSKINPATH,MB_OK|MB_ICONSTOP,0);
		}else
		{
			OpenDocumentFile(szPath);
		}
	}	
}

#include "hyperlink.h"
#pragma comment(lib,"hyperlink")
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	LINKDATA ldHome={IDC_HOME,0};
	LinkInfo_Init(m_hWnd,&ldHome,1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}