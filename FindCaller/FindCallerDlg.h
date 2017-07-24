// FindCallerDlg.h : header file
//

#if !defined(AFX_FINDCALLERDLG_H__714D7958_48C8_4D59_A8F7_6FF15D5C2531__INCLUDED_)
#define AFX_FINDCALLERDLG_H__714D7958_48C8_4D59_A8F7_6FF15D5C2531__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFindCallerDlg dialog

class CFindCallerDlg : public CDialog
{
// Construction
public:
	CFindCallerDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	//{{AFX_DATA(CFindCallerDlg)
	enum { IDD = IDD_FINDCALLER_DIALOG };
	CListCtrl	m_lstProcess;
	//}}AFX_DATA
	CImageList	m_imgLst;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindCallerDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFindCallerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAbout();
	afx_msg void OnProcActive();
	afx_msg void OnProcRefresh();
	afx_msg void OnDestroy();
	afx_msg void OnProcKill();
	afx_msg void OnBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDCALLERDLG_H__714D7958_48C8_4D59_A8F7_6FF15D5C2531__INCLUDED_)
