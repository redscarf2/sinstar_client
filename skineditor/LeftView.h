// LeftView.h : interface of the CLeftView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__12CA0F9B_CEC3_4857_9D13_F46805509A54__INCLUDED_)
#define AFX_LEFTVIEW_H__12CA0F9B_CEC3_4857_9D13_F46805509A54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "skindef.h"

class CSkineditorDoc;

enum {COOR_SRK=1,COOR_FULL=2,COOR_ALL=3};
class CLeftView : public CView
{
protected: // create from serialization only
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
	CSkineditorDoc* GetDocument();

	CPropertyGrid	m_wndGrid;		// 属性表对象

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	void ParseButtonItem(CPropertyGridItem *pItem,int nType,BTNINFO &btn);
	void AddButtonItem(CPropertyGridItem *pGroup,LPCTSTR pszName,UINT uID,BTNINFO *pBtn,BOOL bPosReadOnly=FALSE,UINT uCoor=COOR_ALL);
	void OnItemValueChange(int nID,CPropertyGridItem *pItem);
	//{{AFX_MSG(CLeftView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnPropertyGridNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CSkineditorDoc* CLeftView::GetDocument()
   { return (CSkineditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__12CA0F9B_CEC3_4857_9D13_F46805509A54__INCLUDED_)
