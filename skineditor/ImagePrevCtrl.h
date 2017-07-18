#if !defined(AFX_IMAGEPREVCTRL_H__AA2AB228_FA36_4E84_A263_7579B4FD6669__INCLUDED_)
#define AFX_IMAGEPREVCTRL_H__AA2AB228_FA36_4E84_A263_7579B4FD6669__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImagePrevCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImagePrevCtrl window
#include "image.h"

typedef enum tagBACKMODE{BM_WHITE=1,BM_BLACK,BM_TRANSGRID} BACKMODE;

class CImagePrevCtrl : public CStatic
{
// Construction
public:
	CImagePrevCtrl();

// Attributes
public:
	PIMAGE	m_pImg;
// Operations
public:
	BACKMODE	m_nBackMode;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImagePrevCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetImage(LPCTSTR pszFileName);
	virtual ~CImagePrevCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImagePrevCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	CString m_strMsg;
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPREVCTRL_H__AA2AB228_FA36_4E84_A263_7579B4FD6669__INCLUDED_)
