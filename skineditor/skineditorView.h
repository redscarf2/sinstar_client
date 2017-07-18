// skineditorView.h : interface of the CSkineditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINEDITORVIEW_H__50412C4F_D278_48FD_9AAA_DA3B80FE2610__INCLUDED_)
#define AFX_SKINEDITORVIEW_H__50412C4F_D278_48FD_9AAA_DA3B80FE2610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "skinPrevCtrl.h"
#include "inputwndskinprevctrl.h"

class CSkineditorView : public CFormView
{
protected: // create from serialization only
	CSkineditorView();
	DECLARE_DYNCREATE(CSkineditorView)

public:
	//{{AFX_DATA(CSkineditorView)
	enum { IDD = IDD_SKINEDITOR_FORM };
	CTabCtrl	m_tabMode;
	CSkinPrevCtrl	m_spStatusShrink;
	CSkinPrevCtrl	m_spStatusFull;
	CInputWndSkinPrevCtrl	m_spInputWnd;
	//}}AFX_DATA
	
// Attributes
public:
	CSkineditorDoc* GetDocument();
// Operations
public:
	void InitStatusWndPrev_Shrink();
	void InitStatusWndPrev_Full();
	void InitInputWndPrev();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkineditorView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSkineditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	void AddStatusButton(DWORD dwID,LPCTSTR pszPath,LPCTSTR pszName,RECT rc,SIZE sizeImage,CSkinPrevCtrl * pPrevCtrl);
	afx_msg LRESULT OnSkinPrevClicked(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CSkineditorView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in skineditorView.cpp
inline CSkineditorDoc* CSkineditorView::GetDocument()
   { return (CSkineditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINEDITORVIEW_H__50412C4F_D278_48FD_9AAA_DA3B80FE2610__INCLUDED_)
