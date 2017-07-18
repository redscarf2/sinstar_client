#if !defined(AFX_PANELPREVFORM_H__9C93C67F_03BA_4880_9F1F_8F8F2BB32B97__INCLUDED_)
#define AFX_PANELPREVFORM_H__9C93C67F_03BA_4880_9F1F_8F8F2BB32B97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PanelPrevForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPanelPrevForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include "ImagePrevCtrl.h"

class CPanelPrevForm : public CFormView
{
protected:
	CPanelPrevForm();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPanelPrevForm)

// Form Data
public:
	CImagePrevCtrl	m_ctrlImagePrev;
	//{{AFX_DATA(CPanelPrevForm)
	enum { IDD = IDD_PANEL_PREV };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPanelPrevForm)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPanelPrevForm();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	afx_msg void OnBackMode(UINT uCmd);
	//{{AFX_MSG(CPanelPrevForm)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANELPREVFORM_H__9C93C67F_03BA_4880_9F1F_8F8F2BB32B97__INCLUDED_)
