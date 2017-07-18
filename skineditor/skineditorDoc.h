// skineditorDoc.h : interface of the CSkineditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINEDITORDOC_H__EFA10DA9_229E_46A8_8A96_2F31A86EA501__INCLUDED_)
#define AFX_SKINEDITORDOC_H__EFA10DA9_229E_46A8_8A96_2F31A86EA501__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "skindef.h"
#include "leftview.h"
#include "SkineditorView.h"
#include "PanelPrevForm.h"
#include "PanelVertForm.h"
class CSkineditorDoc : public CDocument
{
protected: // create from serialization only
	CSkineditorDoc();
	DECLARE_DYNCREATE(CSkineditorDoc)

// Attributes
public:
	CString		m_strPath;
	SKINDESC	m_skinDesc;
	SKININPUT	m_skinInput;
	SKINSTATUS	m_skinStatus;
	COLORCOMP	m_colorComp;
	COLORCAND	m_colorCand;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkineditorDoc)
	public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetImageType(LPCTSTR pszName);
	CLeftView * GetLeftView();
	CSkineditorView * GetEditView();
	CPanelPrevForm * GetPicPrevView();
	CPanelVertForm * GetVertView();
	virtual ~CSkineditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	virtual BOOL DoFileSave();
// Generated message map functions
protected:
	//{{AFX_MSG(CSkineditorDoc)
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SKINEDITORDOC_H__EFA10DA9_229E_46A8_8A96_2F31A86EA501__INCLUDED_)
