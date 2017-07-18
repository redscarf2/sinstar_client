#if !defined(AFX_PANELVERTFORM_H__130E9B22_34E6_40B2_8003_E853D83D1414__INCLUDED_)
#define AFX_PANELVERTFORM_H__130E9B22_34E6_40B2_8003_E853D83D1414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanelVertForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanelVertForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "InputWndSkinPrevCtrl.h"

class CPanelVertForm : public CFormView
{
protected:
	CPanelVertForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPanelVertForm)

// Form Data
public:
	//{{AFX_DATA(CPanelVertForm)
	enum { IDD = IDD_PANEL_VERT };
	CInputWndVertSkinPrevCtrl	m_ctrlSkinPrev;
	//}}AFX_DATA

// Attributes
public:
	CSkineditorDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelVertForm)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPanelVertForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	afx_msg LRESULT OnSkinPrevClicked(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CPanelVertForm)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in skineditorView.cpp
inline CSkineditorDoc* CPanelVertForm::GetDocument()
{ return (CSkineditorDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANELVERTFORM_H__130E9B22_34E6_40B2_8003_E853D83D1414__INCLUDED_)
