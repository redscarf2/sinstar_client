
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGrid.h"
#include "PropertyGrid_DrawHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceButton

CPropertyGridInplaceButton::CPropertyGridInplaceButton()
	: m_bPressed(FALSE)
	, m_bOver(FALSE)
	, m_pGrid(0)
	, m_pItem(0)
{

}

CPropertyGridInplaceButton::~CPropertyGridInplaceButton()
{

}

IMPLEMENT_DYNAMIC(CPropertyGridInplaceButton, CStatic)

BEGIN_MESSAGE_MAP(CPropertyGridInplaceButton, CStatic)
	//{{AFX_MSG_MAP(CPropertyGridInplaceButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPropertyGridInplaceButton::HideWindow()
{
	if (m_hWnd)
	{
		ShowWindow(SW_HIDE);
	}
}

void CPropertyGridInplaceButton::Create(CPropertyGridItem* pItem, CRect rect)
{
	ASSERT(pItem && pItem->GetGrid());

	m_pGrid      = pItem->GetGrid();
	rect.left    = rect.right - 15;
	rect.bottom -= 1;

	if (!m_hWnd)
	{
		CStatic::Create(_T(""), SS_NOTIFY|WS_CHILD, rect, m_pGrid);
	}

	SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);
	m_pItem = pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceButton message handlers

void CPropertyGridInplaceButton::OnPaint()
{
	CPaintDC dc(this);
	CPGClientRect rect(this);

	dc.FillSolidRect(rect, GetSysColor(COLOR_3DFACE));

    
	CPGPenDC pen (dc, GetSysColor(COLOR_BTNTEXT));
	CPGBrushDC brush (dc, GetSysColor(COLOR_BTNTEXT));
	
	if (m_pItem->GetFlags() & pgitemHasComboButton)
	{
		CPoint pt = rect.CenterPoint();

		CPoint pts[3];
		pts[0] = CPoint(pt.x -3, pt.y -1);
		pts[1] = CPoint(pt.x + 3, pt.y -1);
		pts[2] = CPoint(pt.x, pt.y +2);
		dc.Polygon(pts, 3);
	} else
	{
		ASSERT(m_pItem->GetFlags() & pgitemHasExpandButton);
        dc.Rectangle(rect.left + 3, rect.bottom -5, rect.left + 5, rect.bottom -3);
		dc.Rectangle(rect.left + 7, rect.bottom -5, rect.left + 9, rect.bottom -3);
		dc.Rectangle(rect.left + 11, rect.bottom -5, rect.left + 13, rect.bottom -3);

	}

	if (m_bPressed && m_bOver) dc.Draw3dRect(rect, GetSysColor(COLOR_3DDKSHADOW), GetSysColor(COLOR_WINDOW));
	else 
	{
		dc.Draw3dRect(rect, GetSysColor(COLOR_3DFACE), GetSysColor(COLOR_3DDKSHADOW));
		rect.DeflateRect(1, 1);
		dc.Draw3dRect(rect, GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_3DSHADOW));
	}
}

void CPropertyGridInplaceButton::OnLButtonDown(UINT, CPoint)
{
	m_bOver = m_bPressed = TRUE;
	Invalidate(FALSE);
	SetCapture();
}

void CPropertyGridInplaceButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bPressed)
	{	
		m_bPressed = FALSE;
		Invalidate(FALSE);
		ReleaseCapture();
		m_pItem->OnInplaceButtonDown();
	}
	CStatic::OnLButtonUp(nFlags, point);
}

void CPropertyGridInplaceButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bPressed)
	{
		CPGClientRect rect(this);
		if ((rect.PtInRect(point) && !m_bOver) ||
			(!rect.PtInRect(point) && m_bOver))
		{
			m_bOver = rect.PtInRect(point);
			Invalidate(FALSE);
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

void CPropertyGridInplaceButton::OnCaptureChanged(CWnd *pWnd)
{
	m_bPressed = FALSE;
	Invalidate(FALSE);

	CStatic::OnCaptureChanged(pWnd);
}

void CPropertyGridInplaceButton::DestroyItem()
{
	// reset variables to defaults.
	m_bPressed = FALSE;
	m_bOver    = FALSE;
	m_pGrid    = NULL;
	m_pItem    = NULL;

	// destroy the window.
	DestroyWindow( );
}
