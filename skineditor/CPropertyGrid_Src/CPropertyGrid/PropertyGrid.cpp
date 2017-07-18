
#include "stdafx.h"

#include "PropertyGrid_Defines.h"
#include "PropertyGrid_Resource.h"
#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGrid.h"
#include "PropertyGrid_DrawHelpers.h"

const UINT SPLITTER_HEIGHT = 3;
const UINT HIT_SPLITTER    = 1;
const UINT TOOLBAR_HEIGHT  = 25;

/////////////////////////////////////////////////////////////////////////////
// CPropertyGrid

CPropertyGrid::CPropertyGrid()
: m_bHelpVisible(TRUE)
, m_bToolBarVisible(FALSE)
, m_nHelpHeight(58)
{

}

CPropertyGrid::~CPropertyGrid()
{

}

IMPLEMENT_DYNAMIC(CPropertyGrid, CWnd)

BEGIN_MESSAGE_MAP(CPropertyGrid, CWnd)
	//{{AFX_MSG_MAP(CPropertyGrid)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()	
	ON_COMMAND(PG_IDC_SORT_ALPHABETIC, OnSortAlphabetic)
	ON_COMMAND(PG_IDC_SORT_CATEGORIZED, OnSortCategorized)
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolTipNotify )
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CPropertyGrid::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW));
	CPGEmptyRect rc;
	
	if (!CreateEx(0, pszCreateClass, _T(""), WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD|WS_GROUP, rc, pParentWnd, nID))
		return FALSE;

	if (!m_wndView.CreateEx(WS_EX_STATICEDGE, _T("LISTBOX"), NULL, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_CHILD|WS_VISIBLE|LBS_NOTIFY|LBS_OWNERDRAWFIXED| LBS_NOINTEGRALHEIGHT| WS_VSCROLL|WS_TABSTOP, rc, this, 0))
		return FALSE;
	
	m_wndView.ResetContent();

	SetWindowPos(0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);
	SetStandardColors();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyGrid message handlers

BOOL CPropertyGrid::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CPropertyGrid::OnPaint()
{
	CPaintDC dcPaint(this); 
	CPGClientRect rc(this);
	CPGBufferDC dc(dcPaint, rc);
	
	dc.FillSolidRect(rc, GetSysColor(COLOR_3DFACE));

	if (m_bHelpVisible)
	{
		CPGPenDC pen(dc, GetSysColor(COLOR_3DSHADOW));
		CPGBrushDC brush(dc, m_clrHelpBack);
		
		CRect rcHelp(rc);
		rcHelp.top = rc.bottom - m_nHelpHeight;
		dc.Rectangle(rcHelp);


		CPGFontDC font(&dc, &m_wndView.m_fontBold);
		CPropertyGridItem* pItem = m_wndView.GetSelectedItem();
		if (pItem)
		{
			dc.SetTextColor(m_clrHelpFore);
			dc.SetBkColor(m_clrHelpBack);
			CRect rcCaption(rcHelp);
			rcCaption.DeflateRect(6, 3);
			dc.DrawText(pItem->GetCaption(), rcCaption, DT_SINGLELINE);
			
			CString strDesc = pItem->GetDescription();
			if (!strDesc.IsEmpty())
			{
				CPGFontDC font(&dc, &m_wndView.m_fontNormal);
				CPGEmptyRect rcCaption;
				dc.DrawText(pItem->GetCaption(), rcCaption, DT_SINGLELINE|DT_CALCRECT);
				CRect rcDesc(rcHelp);
				rcDesc.DeflateRect(6, 3 + rcCaption.Height() + 3, 6, 3);
				dc.DrawText(strDesc, rcDesc, DT_WORDBREAK);
			}
		}		
	}
}

void CPropertyGrid::OnSetFocus(CWnd*)
{
	m_wndView.SetFocus();
}

void CPropertyGrid::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	
	if (!m_wndView.GetSafeHwnd())
		return;

    Reposition(cx, cy);
}

int CPropertyGrid::HitTest(CPoint pt)
{
	if (!m_bHelpVisible)
		return -1;

	CPGClientRect rc(this);
	CRect rcSplitter(CPoint(rc.left, rc.bottom - SPLITTER_HEIGHT - m_nHelpHeight), CSize(rc.Width(), SPLITTER_HEIGHT));
	
	if (rcSplitter.PtInRect(pt))
		return HIT_SPLITTER;

	return -1;
}

BOOL CPropertyGrid::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		GetCursorPos(&point);
		ScreenToClient(&point);

		if (HitTest(point) == HIT_SPLITTER)
		{
			static HCURSOR hcurVSplitBar = ::LoadCursor( AfxGetInstanceHandle(), MAKEINTRESOURCE(PG_IDC_VSPLITBAR) );
			ASSERT(hcurVSplitBar);
			SetCursor(hcurVSplitBar);
			return TRUE;
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CPropertyGrid::OnInvertTracker(CRect rect)
{
	ASSERT_VALID(this);
	ASSERT(!rect.IsRectEmpty());
	ASSERT((GetStyle() & WS_CLIPCHILDREN) == 0);

	// pat-blt without clip children on
	CDC* pDC = GetDC();

	CBrush brush(GetSysColor(COLOR_3DFACE));
	CBrush* pBrush = (CBrush*)pDC->SelectObject(&brush);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT);
	pDC->SelectObject(pBrush);
	
	ReleaseDC(pDC);
}

void CPropertyGrid::OnSelectionChanged(CPropertyGridItem* pItem)
{
	CPGClientRect rc(this);
	CRect rcHelp(rc);
	rcHelp.top = rc.bottom - m_nHelpHeight;
	InvalidateRect(rcHelp, FALSE);

	if (pItem)
	{
		// 发送属性项选中的通知消息
		int nID = GetDlgCtrlID();
		WPARAM wParam = MAKEWPARAM(nID, PGN_SELECTION_CHANGED);
		GetOwner()->SendMessage(PGWM_PROPERTYGRID_NOTIFY, wParam, (LPARAM)pItem);
	}
}

void CPropertyGrid::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (HitTest(point) == HIT_SPLITTER)
	{
		SetCapture();
		CPGClientRect rc(this);

		ModifyStyle(WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, FALSE);

		m_rectTracker.SetRect(0, point.y, rc.Width(), point.y + 3);

		OnInvertTracker(m_rectTracker);

		BOOL bAccept = FALSE;
		while (GetCapture() == this)
		{
			MSG msg;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;
			
			if (msg.message == WM_MOUSEMOVE)
			{
				point = CPoint(msg.lParam);
				point.y = __min(point.y, rc.Height() - 20);
				point.y = __max(point.y, 20 + TOOLBAR_HEIGHT);

				if (m_rectTracker.top != point.y)
				{
					OnInvertTracker(m_rectTracker);
					m_rectTracker.OffsetRect(0, point.y - m_rectTracker.top);
					OnInvertTracker(m_rectTracker);
				}
			}
			else if (msg.message == WM_KEYDOWN &&  msg.wParam == VK_ESCAPE)	break;
			else if (msg.message ==WM_LBUTTONUP)
			{
				bAccept = TRUE;
				break;
			}
			else  ::DispatchMessage(&msg);
		}

		ReleaseCapture();

		if (bAccept)
		{
			m_nHelpHeight = rc.bottom - m_rectTracker.top -2;
			Reposition(rc.Width(), rc.Height());
		} 
		Invalidate(FALSE);
		ModifyStyle(0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS, FALSE);
		return;
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CPropertyGrid::Reposition(int cx, int cy)
{
	CRect rcView(0, 0, cx, cy);

	if (m_bHelpVisible)
	{
		rcView.bottom -= m_nHelpHeight + SPLITTER_HEIGHT;
	}
	if (m_bToolBarVisible)
	{
		ASSERT(m_wndToolbar.GetSafeHwnd());
		CRect rcToolBar(1, 1, cx - 1, TOOLBAR_HEIGHT - 1);
		m_wndToolbar.MoveWindow(rcToolBar);

		rcView.top += TOOLBAR_HEIGHT;
	}

	m_wndView.MoveWindow(rcView);
}

void CPropertyGrid::CreateToolbar()
{
	m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_BORDER|WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS);
	m_wndToolbar.LoadToolBar(PG_IDR_PROPGRID_TBAR);
}

void CPropertyGrid::ShowToolBar(BOOL bShow)
{
	if (bShow && !m_wndToolbar.GetSafeHwnd()) CreateToolbar();
	else 
	{
		if (m_wndToolbar.GetSafeHwnd())
			m_wndToolbar.ShowWindow(bShow? SW_SHOW: SW_HIDE);
	}
	
	m_bToolBarVisible = bShow;
	
	CPGClientRect rc(this);
	Reposition(rc.Width(), rc.Height());
	RefreshToolBarButtons();
}

void CPropertyGrid::RefreshToolBarButtons()
{
	if (m_bToolBarVisible) 
	{
		m_wndToolbar.GetToolBarCtrl().SetState(PG_IDC_SORT_CATEGORIZED, TBSTATE_ENABLED | (m_wndView.m_properetySort == pgsortCategorized? TBSTATE_CHECKED: 0));
		m_wndToolbar.GetToolBarCtrl().SetState(PG_IDC_SORT_ALPHABETIC, TBSTATE_ENABLED | (m_wndView.m_properetySort == pgsortAlphabetical? TBSTATE_CHECKED: 0));
	}
}

void CPropertyGrid::ShowHelp(BOOL bShow)
{
	m_bHelpVisible = bShow;

	CPGClientRect rc(this);
	Reposition(rc.Width(), rc.Height());

}
void CPropertyGrid::OnSortAlphabetic()
{
	SetPropertySort(pgsortAlphabetical);
}

void CPropertyGrid::OnSortCategorized()
{
	SetPropertySort(pgsortCategorized);
}

void CPropertyGrid::OnSortChanged()
{
	RefreshToolBarButtons();
	OnSelectionChanged(NULL);

	// 发送属性项排序改变的通知消息
	int nID = GetDlgCtrlID();
	WPARAM wParam = MAKEWPARAM(nID, PGN_SORTORDER_CHANGED);
	GetOwner()->SendMessage(PGWM_PROPERTYGRID_NOTIFY, wParam, (LPARAM)0);
}

void CPropertyGrid::SetStandardColors()
{
	m_clrHelpBack       = ::GetSysColor( COLOR_3DFACE );
	m_clrHelpFore       = ::GetSysColor( COLOR_BTNTEXT );
	m_wndView.m_clrLine = ::GetSysColor( COLOR_SCROLLBAR );
	m_wndView.m_clrBack = ::GetSysColor( COLOR_WINDOW );
	m_wndView.m_clrFore = ::GetSysColor( COLOR_WINDOWTEXT );

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW| RDW_INVALIDATE | RDW_ERASE);
}

void CPropertyGrid::SetCustomColors(
	COLORREF clrHelpBack, COLORREF clrHelpFore,	COLORREF clrViewLine,
	COLORREF clrViewBack, COLORREF clrViewFore )
{
	m_clrHelpBack = clrHelpBack;
	m_clrHelpFore = clrHelpFore;
	m_wndView.m_clrLine = clrViewLine;
	m_wndView.m_clrBack = clrViewBack;
	m_wndView.m_clrFore = clrViewFore;

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW| RDW_INVALIDATE | RDW_ERASE);
}

void CPropertyGrid::ResetContent()
{
	m_wndView.ResetContent();

	m_wndView.m_pSelected = NULL;
	
	while ( !m_wndView.m_lstCategories.IsEmpty( ) )
	{
		CPropertyGridItem* pGridItem =
			m_wndView.m_lstCategories.RemoveTail( );
		
		if(pGridItem) { delete pGridItem; pGridItem = NULL; }
	}

	CPGClientRect rc(this);
	CRect rcHelp(rc);
	rcHelp.top = rc.bottom - m_nHelpHeight;
	InvalidateRect(rcHelp, FALSE);
}

void CPropertyGrid::ExpandAllCategories()
{
	CPGListItems* pList = &GetGridView().m_lstCategories;
	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CPropertyGridItem* pCategory = pList->GetNext(pos);
		pCategory->Expand();
	}
}

void CPropertyGrid::CollapseAllCategories()
{
	CPGListItems* pList = &GetGridView().m_lstCategories;
	POSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CPropertyGridItem* pCategory = pList->GetNext(pos);
		pCategory->Collapse();
	}
}

BOOL CPropertyGrid::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT nID = (UINT)pNMHDR->idFrom;

	if (pTTT->uFlags & TTF_IDISHWND)
		return FALSE;

	// nID是工具栏按钮的ID值
	pTTT->lpszText = MAKEINTRESOURCE(nID);
	pTTT->hinst = AfxGetInstanceHandle();
	return TRUE;
}

void CPropertyGrid::OnContextMenu(CWnd* pWnd, CPoint point) 
{
}
