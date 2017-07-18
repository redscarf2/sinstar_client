#if !defined(AFX_BMPCOMBINEDLG_H__A1503F42_75D7_423F_AFFF_19EEB26F457D__INCLUDED_)
#define AFX_BMPCOMBINEDLG_H__A1503F42_75D7_423F_AFFF_19EEB26F457D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BmpCombineDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBmpCombineDlg dialog

class CBmpCombineDlg : public CDialog
{
// Construction
public:
	CBmpCombineDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBmpCombineDlg)
	enum { IDD = IDD_BMPCOMBINE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpCombineDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBmpCombineDlg)
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnBrowse3();
	afx_msg void OnBrowse4();
	afx_msg void OnCombine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPCOMBINEDLG_H__A1503F42_75D7_423F_AFFF_19EEB26F457D__INCLUDED_)
