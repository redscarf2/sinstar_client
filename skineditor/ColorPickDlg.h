#if !defined(AFX_COLORPICKDLG_H__379006E7_8B67_4E52_A72D_4B25D599D705__INCLUDED_)
#define AFX_COLORPICKDLG_H__379006E7_8B67_4E52_A72D_4B25D599D705__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPickDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPickDlg dialog

class CColorPickDlg : public CDialog
{
// Construction
public:
	CColorPickDlg(CWnd* pParent = NULL);   // standard constructor
	COLORREF	m_crPick;
// Dialog Data
	//{{AFX_DATA(CColorPickDlg)
	enum { IDD = IDD_COLORPICK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPickDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKDLG_H__379006E7_8B67_4E52_A72D_4B25D599D705__INCLUDED_)
