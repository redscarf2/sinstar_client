// FindCallerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FindCaller.h"
#include "FindCallerDlg.h"
#include "aboutdlg.h"
#include "dllcaller.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindCallerDlg dialog

CFindCallerDlg::CFindCallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindCallerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindCallerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFindCallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindCallerDlg)
	DDX_Control(pDX, IDC_LIST_PROCESS, m_lstProcess);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindCallerDlg, CDialog)
	//{{AFX_MSG_MAP(CFindCallerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_PROC_ACTIVE, OnProcActive)
	ON_BN_CLICKED(IDC_PROC_REFRESH, OnProcRefresh)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PROC_KILL, OnProcKill)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindCallerDlg message handlers

BOOL CFindCallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef _WIN64
	CString strTitle;
	GetWindowText(strTitle);
	strTitle += _T("[x64]");
	SetWindowText(strTitle);
#endif

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	m_imgLst.Create(16,16,ILC_COLORDDB|ILC_MASK,0,0);
	m_imgLst.Add(theApp.LoadIcon(IDR_MAINFRAME));
	m_lstProcess.SetImageList(&m_imgLst,LVSIL_SMALL);
	m_lstProcess.InsertColumn(0,"名称",0,100);
	m_lstProcess.InsertColumn(1,"进程ID",0,100);
	m_lstProcess.InsertColumn(2,"完整路径",0,280);
	if(theApp.m_strFile.IsEmpty())
	{
		CString str=theApp.GetProfileString("rem","dll",NULL);
		SetDlgItemText(IDC_DLLPATH,str);
	}else
	{
		SetDlgItemText(IDC_DLLPATH,theApp.m_strFile);
	}
	OnProcRefresh();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFindCallerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindCallerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFindCallerDlg::OnAbout() 
{
	CAboutDlg about;
	about.DoModal();
}

void CFindCallerDlg::OnProcActive() 
{
	int iProc=m_lstProcess.GetSelectionMark();
	if(iProc!=-1)
	{
		DWORD dwProcID=m_lstProcess.GetItemData(iProc);
		HWND   hWnd=GetProcessWnd(dwProcID);
		if(!hWnd)
			MessageBox("没有找到进程窗口","提示",MB_OK|MB_ICONSTOP);
		else
		{
			if(::IsIconic(hWnd)) ::PostMessage(hWnd,WM_SYSCOMMAND,SC_RESTORE,0);
			::BringWindowToTop(hWnd);
			::FlashWindow(hWnd,TRUE);
		}
	}
}


void CFindCallerDlg::OnProcRefresh() 
{
	char szDllPath[MAX_PATH],szBuf[50];
	GetDlgItemText(IDC_DLLPATH,szDllPath,MAX_PATH);
	theApp.WriteProfileString("rem","dll",szDllPath);
	m_lstProcess.DeleteAllItems();
	m_imgLst.SetImageCount(1);
	if(szDllPath[0])
	{
		PCALLERINFO pci=GetCaller(szDllPath);
		if(pci)
		{
			HICON hIcon;
			int i=0,iImage;
			while(pci[i].szFile[0])
			{
				iImage=0;
				if(ExtractIconEx(pci[i].szPath,0,NULL,&hIcon,1)==1)
				{
					iImage=m_imgLst.Add(hIcon);
					DeleteObject(hIcon);
				}
				sprintf(szBuf,"0x%08X",pci[i].dwProcID);
				m_lstProcess.InsertItem(i,pci[i].szFile,iImage);
				m_lstProcess.SetItemText(i,1,szBuf);
				m_lstProcess.SetItemText(i,2,pci[i].szPath);
				m_lstProcess.SetItemData(i,pci[i].dwProcID);
				i++;
			}
			FreeCallerInfo(pci);
		}
	}
}

void CFindCallerDlg::OnProcKill() 
{
	int iProc=m_lstProcess.GetSelectionMark();
	if(iProc!=-1)
	{
		char szName[100],szBuf[200];
		m_lstProcess.GetItemText(iProc,0,szName,100);
		sprintf(szBuf,"确定终止进程[%s]吗?",szName);
		if(MessageBox(szBuf,"提示",MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		{
			DWORD dwProcID=m_lstProcess.GetItemData(iProc);
			if(Kill_Process(dwProcID))
			{
				m_lstProcess.DeleteItem(iProc);
			}else
			{
				sprintf(szBuf,"终止进程[%s]失败!",szName);
				MessageBox(szBuf,"提示",MB_OK|MB_ICONSTOP);
			}
		}
	}
}


void CFindCallerDlg::OnDestroy() 
{
	m_imgLst.DeleteImageList();
	CDialog::OnDestroy();	
}

void CFindCallerDlg::OnBrowse() 
{
	CFileDialog	openDlg(TRUE,NULL,NULL,0,"DLL,IME FILE|*.dll;*.ime||");
	if(openDlg.DoModal()==IDOK)
	{
		SetDlgItemText(IDC_DLLPATH,openDlg.GetPathName());
		OnProcRefresh();
	}
}
