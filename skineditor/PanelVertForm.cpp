// PanelVertForm.cpp : implementation file
//

#include "stdafx.h"
#include "skineditor.h"
#include "skineditorDoc.h"
#include "PanelVertForm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanelVertForm

IMPLEMENT_DYNCREATE(CPanelVertForm, CFormView)

CPanelVertForm::CPanelVertForm()
	: CFormView(CPanelVertForm::IDD)
{
	//{{AFX_DATA_INIT(CPanelVertForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPanelVertForm::~CPanelVertForm()
{
}

void CPanelVertForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanelVertForm)
	DDX_Control(pDX, IDC_SP_IPTWNDVERT, m_ctrlSkinPrev);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanelVertForm, CFormView)
	//{{AFX_MSG_MAP(CPanelVertForm)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_SKINOBJCLICKED,OnSkinPrevClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanelVertForm diagnostics

#ifdef _DEBUG
void CPanelVertForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CPanelVertForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSkineditorDoc* CPanelVertForm::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkineditorDoc)));
	return (CSkineditorDoc*)m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPanelVertForm message handlers


LRESULT CPanelVertForm::OnSkinPrevClicked(WPARAM wParam, LPARAM lParam)
{
	CSkineditorDoc *pDoc=GetDocument();
	CPropertyGridItem *pItem=pDoc->GetLeftView()->m_wndGrid.FindItem(lParam);
	if(pItem) 
	{
		pDoc->GetLeftView()->m_wndGrid.SetFocus();
		CPropertyGridItem *pParent=pItem->GetParentItem();
		if(pParent)
		{
			if(!pParent->IsExpanded()) pParent->Expand();
		}
		pItem->Select();
	}
	return 0;
}

void CPanelVertForm::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(HIWORD(lHint)==0)
	{
		char szPath[MAX_PATH];
		CSkineditorDoc *pDoc=GetDocument();
		//bk image
		sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinInput.loVert.szBgImg);
		m_ctrlSkinPrev.InitObjects(pDoc->m_strPath,&pDoc->m_skinInput,&pDoc->m_colorComp,&pDoc->m_colorCand);
		m_ctrlSkinPrev.SetTile(pDoc->m_skinInput.loVert.bTile);
	}
}
