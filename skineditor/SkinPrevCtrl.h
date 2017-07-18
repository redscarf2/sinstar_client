#if !defined(AFX_SKINPREVCTRL_H__183B330B_4DCA_4C04_B857_6085D5E8AD5A__INCLUDED_)
#define AFX_SKINPREVCTRL_H__183B330B_4DCA_4C04_B857_6085D5E8AD5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkinPrevCtrl.h : header file
// 皮肤显示效果预览控件

/////////////////////////////////////////////////////////////////////////////
// CSkinPrevCtrl window
#include "SkinObjectEx.h"
#include <afxtempl.h>

#define UM_SKINOBJCLICKED	(WM_USER+550)
class CSkinPrevCtrl : public CStatic
{
// Construction
public:
	CSkinPrevCtrl(BOOL bObjOwner=FALSE);
	virtual ~CSkinPrevCtrl();
	
// Attributes
public:

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkinPrevCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void	OnDrawBkImage(CDC *pDC,POINT ptOffset);
	virtual POINT GetOffset();
	virtual SIZE	GetSize();

	void SetFontInfo(LOGFONT *plf);
	LOGFONT *GetFontInfo(){return &m_lf;}
	void AddObject(CSkinObject *pObj,BOOL bUpdate=FALSE);
	void SetBgImage(LPCTSTR pszImage);
	PIMAGE GetImage(){return m_pBgImage;}
	void SetObjOwnerMode(BOOL bObjOwner){m_bObjOwner=bObjOwner;}
	void RemoveObjects();
	CSkinObject * HitTest(POINT pt);
	// Generated message map functions
protected:
	//{{AFX_MSG(CSkinPrevCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	CList<CSkinObject *,CSkinObject *> m_obList;

	BOOL	m_bObjOwner;
	PIMAGE	m_pBgImage;
	LOGFONT	m_lf;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINPREVCTRL_H__183B330B_4DCA_4C04_B857_6085D5E8AD5A__INCLUDED_)
