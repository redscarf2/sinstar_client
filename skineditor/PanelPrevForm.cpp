// PanelPrevForm.cpp : implementation file
//

#include "stdafx.h"
#include "skineditor.h"
#include "PanelPrevForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPanelPrevForm

IMPLEMENT_DYNCREATE(CPanelPrevForm, CFormView)

CPanelPrevForm::CPanelPrevForm()
	: CFormView(CPanelPrevForm::IDD)
{
	//{{AFX_DATA_INIT(CPanelPrevForm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPanelPrevForm::~CPanelPrevForm()
{
}

void CPanelPrevForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPanelPrevForm)
	DDX_Control(pDX,IDC_IMAGEPREV,m_ctrlImagePrev);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPanelPrevForm, CFormView)
	//{{AFX_MSG_MAP(CPanelPrevForm)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(MAKELONG(IDC_BM_WHITE,BN_CLICKED),MAKELONG(IDC_BM_TRANSGRID,BN_CLICKED),OnBackMode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPanelPrevForm diagnostics

#ifdef _DEBUG
void CPanelPrevForm::AssertValid() const
{
	CFormView::AssertValid();
}

void CPanelPrevForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPanelPrevForm message handlers

void CPanelPrevForm::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	m_ctrlImagePrev.m_nBackMode=(BACKMODE)AfxGetApp()->GetProfileInt("settings","BackMode",3);
	CheckRadioButton(IDC_BM_WHITE,IDC_BM_TRANSGRID,IDC_BM_WHITE+m_ctrlImagePrev.m_nBackMode-1);
}

void CPanelPrevForm::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	if(cx<400) cx=400;
	if(cy<200) cy=200;
	SetScrollSizes(MM_TEXT,CSize(cx,cy));
	if(m_ctrlImagePrev.GetSafeHwnd())
	{
		m_ctrlImagePrev.SetWindowPos(NULL,0,0,cx,cy-50,SWP_NOZORDER|SWP_NOMOVE|SWP_NOCOPYBITS);
	}
}

void CPanelPrevForm::OnBackMode(UINT uCmd)
{
	UINT uCtrlID=LOWORD(uCmd);
	m_ctrlImagePrev.m_nBackMode=(BACKMODE)(uCtrlID-IDC_BM_WHITE+1);
	m_ctrlImagePrev.Invalidate();
	AfxGetApp()->WriteProfileInt("settings","BackMode",m_ctrlImagePrev.m_nBackMode);
}
