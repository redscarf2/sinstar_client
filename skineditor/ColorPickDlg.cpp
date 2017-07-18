// ColorPickDlg.cpp : implementation file
//

#include "stdafx.h"
#include "skineditor.h"
#include "ColorPickDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorPickDlg dialog


CColorPickDlg::CColorPickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorPickDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorPickDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_crPick=RGB(255,255,255);
}


void CColorPickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorPickDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorPickDlg, CDialog)
	//{{AFX_MSG_MAP(CColorPickDlg)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickDlg message handlers





void CColorPickDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);
	CWnd *pPrev=GetDlgItem(IDC_PREV);
	pPrev->GetClientRect(&rc);
	pPrev->ClientToScreen(&rc);
	ScreenToClient(&rc);
	dc.FillSolidRect(&rc,m_crPick);
}

BOOL CColorPickDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetCapture();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CColorPickDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);
	ClientToScreen(&point);
	HDC hdc=::GetDC(NULL);
	m_crPick=GetPixel(hdc,point.x,point.y);
	::ReleaseDC(NULL,hdc);
	Invalidate(FALSE);
	char szBuf[20];
	sprintf(szBuf,"%d,%d,%d",GetRValue(m_crPick),GetGValue(m_crPick),GetBValue(m_crPick));
	SetDlgItemText(IDC_VALUE,szBuf);
	SetWindowPos(&wndTopMost,point.x+16,point.y,0,0,SWP_NOSIZE);
}

void CColorPickDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	EndDialog(IDOK);
}

void CColorPickDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	ReleaseCapture();	
}

BOOL CColorPickDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CColorPickDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	EndDialog(IDCANCEL);
}
