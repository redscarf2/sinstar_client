
#include "stdafx.h"

#include "PropertyGrid_Resource.h"
#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGrid_DrawHelpers.h"
#include "PropertyGridItem.h"
#include "PropertyGrid.h"

const UINT PGV_HIT_SPLITTER = 0x100;

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridView

CPropertyGridView::CPropertyGridView()
{
	m_properetySort = pgsortCategorized;

	m_dDivider = .5;
	m_bTracking = FALSE;

	LOGFONT lfIcon;
	VERIFY( ::SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof( lfIcon ), &lfIcon, 0 ) );
	lfIcon.lfWeight = FW_NORMAL;
	lfIcon.lfItalic = FALSE;
	VERIFY( m_fontNormal.CreateFontIndirect(&lfIcon  ) );

	lfIcon.lfWeight = FW_BOLD;
	VERIFY( m_fontBold.CreateFontIndirect(&lfIcon  ) );

	m_pSelected = NULL;
}

CPropertyGridView::~CPropertyGridView()
{	
	while (!m_lstCategories.IsEmpty())
	{
		delete m_lstCategories.RemoveTail();
	}
}

IMPLEMENT_DYNAMIC(CPropertyGridView, CListBox)

BEGIN_MESSAGE_MAP(CPropertyGridView, CListBox)
	//{{AFX_MSG_MAP(CPropertyGridView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelectionChanged)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridView message handlers

void CPropertyGridView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (GetScrollBarCtrl(SB_VERT) == pScrollBar)
	{
		CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
		Invalidate(FALSE);

		CWnd* pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			if (pWnd->IsWindowVisible())
				pWnd->Invalidate(FALSE);
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}
	else
	{
		CListBox::OnVScroll(nSBCode, nPos, pScrollBar);
	}
}

BOOL CPropertyGridView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	BOOL bResult = CWnd::OnMouseWheel(nFlags, zDelta, pt);
	Invalidate(FALSE);

	CWnd* pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		if (pWnd->IsWindowVisible())
			pWnd->Invalidate(FALSE);
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	return bResult;
}

void CPropertyGridView::PreSubclassWindow()
{
	CListBox::PreSubclassWindow();
	ASSERT((GetStyle() & (LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT)) == (LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_NOINTEGRALHEIGHT));
	ASSERT((GetStyle() & WS_BORDER) == 0);
	ASSERT((GetExStyle() & WS_EX_STATICEDGE) == WS_EX_STATICEDGE);
}

void CPropertyGridView::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	CPropertyGridItem* pItem = (CPropertyGridItem*)lpDrawItemStruct->itemData;

	COLORREF clrWindow = m_clrBack;

	if (pItem == NULL)
		return;

	CPGWindowRect rcWindow(this);
	CRect rc = lpDrawItemStruct->rcItem;

	CPGBufferDC dc(lpDrawItemStruct->hDC, rc);
	CPGFontDC font(&dc, &m_fontNormal);

	dc.FillSolidRect(rc, clrWindow);
	dc.SetBkMode(TRANSPARENT);

	CRect rcCaption(rc.left, rc.top, rc.left + int(m_dDivider * rcWindow.Width()), rc.bottom);

	if (pItem->IsCategory())
	{
		dc.FillSolidRect(rc, m_clrLine);
		CPGFontDC font(&dc, &m_fontBold);

		dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));		
		CPGEmptyRect rcDrawText;
		dc.DrawText( pItem->GetCaption(), rcDrawText,  DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);

		CRect rcText(CPoint(PGI_EXPAND_BORDER - 1, rc.top + 1) , CSize(rcDrawText.Width() + 4, rc.Height() - 2));

		dc.DrawText( pItem->GetCaption(),   rcText,  DT_SINGLELINE|DT_VCENTER|DT_RIGHT);

		if ((lpDrawItemStruct->itemAction | ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))
		{		
			dc.SetTextColor(0);		
			dc.DrawFocusRect(rcText);
		}		
	} 
	else
	{
		dc.SetTextColor(pItem->m_bReadOnly? GetSysColor(COLOR_GRAYTEXT): m_clrFore);

		CRect rcValue(rc);
		rcValue.left = rcCaption.right + 4;
		rcValue.bottom -= 1;

		if (!pItem->OnDrawItemValue(dc, rcValue))
		{
			dc.DrawText( pItem->m_strValue, rcValue,  DT_SINGLELINE|DT_VCENTER);
		}

		BOOL bSelected = ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED));
		BOOL bFocused = bSelected && GetFocus() && ((GetFocus() == this) || (GetFocus()->GetParent() == this) || (GetFocus()->GetOwner() == this));
		if (bFocused)
		{
			dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			dc.FillSolidRect(rcCaption, ::GetSysColor(COLOR_HIGHLIGHT));
		} 
		else if (bSelected)
		{
			dc.FillSolidRect(rcCaption, m_clrLine);
		}	
		dc.FillSolidRect(rc.left, rc.top, PGI_EXPAND_BORDER, rc.Height(), m_clrLine);
		CPGPenDC pen(dc, m_clrLine);
		dc.MoveTo(0, rc.bottom - 1); dc.LineTo(rc.right, rc.bottom - 1);
		dc.MoveTo(rcCaption.right, rc.top); dc.LineTo(rcCaption.right, rc.bottom - 1);
	
		CRect rcText(rc);
		rcText.left = pItem->m_nIndent * PGI_EXPAND_BORDER + 3;
		rcText.right = rcCaption.right - 1;
		rcText.bottom -= 1;
		dc.DrawText( pItem->GetCaption(),   rcText,  DT_SINGLELINE|DT_VCENTER);
	}	

	if (pItem->HasChilds())
	{
		CRgn rgn;
		rgn.CreateRectRgnIndirect(&rcCaption);
		dc.SelectClipRgn(&rgn);

		CRect rcSign(CPoint(PGI_EXPAND_BORDER / 2 - 5, rc.CenterPoint().y - 4), CSize(9, 9));

		if (pItem->m_nIndent > 1)
			rcSign.OffsetRect((pItem->m_nIndent - 1) * PGI_EXPAND_BORDER, 0);

		CPoint pt = rcSign.CenterPoint();

		CPGBrushDC brush (dc,  pItem->IsCategory()? m_clrLine: clrWindow);
		CPGPenDC pen (dc,  m_clrFore);
		dc.Rectangle(rcSign);

		dc.MoveTo(pt.x - 2, pt.y);
		dc.LineTo(pt.x + 3, pt.y);

		if(!pItem->m_bExpanded)
		{
			dc.MoveTo(pt.x, pt.y - 2);
			dc.LineTo(pt.x, pt.y + 3);
		}

		dc.SelectClipRgn(NULL);
	}
}

BOOL CPropertyGridView::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void CPropertyGridView::OnPaint()
{
	CPaintDC dc(this); 	
	CPGClientRect rc(this);
	CPGBufferDC buffer(dc, rc);

	buffer.FillSolidRect(rc, m_clrBack);
	CWnd::DefWindowProc(WM_PAINT, (WPARAM)buffer.m_hDC, 0);
}

void CPropertyGridView::OnNcPaint()
{
	CWindowDC dc(this);
	CPGWindowRect rc(this);
	rc.OffsetRect(-rc.TopLeft());

	// 当窗口大小发生改变，使得列表项消失时，下面这段代码会产生异常。
	// 由于这段代码看上去无关紧要，因此予以注释。
// #ifdef _DEBUG
//	CPGClientRect rcClient(this);
//	ASSERT(rcClient.Height() == rc.Height() - 2);
// #endif

	CWnd::DefWindowProc(WM_PRINT, (WPARAM)dc.m_hDC, PRF_NONCLIENT);
	dc.Draw3dRect(rc, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW) );
}

CPropertyGridItem* CPropertyGridView::AddCategory(CString strCaption)
{
	CPropertyGridItem* pCategory = new CPropertyGridItem(strCaption);
	pCategory->m_pGrid = this;
	pCategory->m_bCategory = TRUE;
	pCategory->m_nFlags = 0;

	m_lstCategories.AddTail(pCategory);
	SetPropertySort(m_properetySort, TRUE);

	return pCategory;	
}

void CPropertyGridView::ResetContent()
{
	// 在删除属性项对象之前，强制触发内置编辑框的EN_KILLFOCUS消息
	if (m_pSelected)
	{
		m_pSelected->OnDeselect();
		m_pSelected = NULL;
	}

	for (int i = 0; i < GetCount(); i++)
	{
		CPropertyGridItem* pItem = (CPropertyGridItem*)GetItemDataPtr(i);
		ASSERT(pItem);
		ASSERT(pItem->m_nIndex == i);
		pItem->m_bVisible = FALSE;
	}
	CListBox::ResetContent();

	SetFont(&m_fontNormal, FALSE);

	CWindowDC dc(this);
	CPGFontDC font(&dc, &m_fontNormal);	
	SetItemHeight(0, dc.GetTextExtent(_T(" ")).cy + 3);
}

void CPropertyGridView::SetPropertySort(PG_PROPERTYSORT sort, BOOL bRrefresh)
{
	if (sort == m_properetySort && !bRrefresh)
		return;

	SetRedraw(FALSE); // 禁止窗口重画

	CPropertyGridItem* pSelected = GetSelectedItem();
	ResetContent();

	if (sort == pgsortCategorized)
	{
		POSITION pos = m_lstCategories.GetHeadPosition();
		while (pos)
		{
			CPropertyGridItem* pCategory = m_lstCategories.GetNext(pos);
			InsertItem(pCategory, GetCount());
		}
	} 
	else if (sort == pgsortAlphabetical || sort == pgsortNoSort)  
	{
		CPGListItems lstItems;
		POSITION pos = m_lstCategories.GetHeadPosition();
		while (pos)
		{
			CPropertyGridItem* pCategory = m_lstCategories.GetNext(pos);			
			lstItems.AddTail(&pCategory->m_lstChilds);
		}

		if (sort != pgsortNoSort)
			lstItems.Sort();

		pos = lstItems.GetHeadPosition();
		while (pos)
		{
			CPropertyGridItem* pItem = lstItems.GetNext(pos);						
			InsertItem(pItem, GetCount());
		}
	} 
	else
	{
		ASSERT(FALSE);
	}

	_RefreshIndexes();

	if (pSelected)
	{
		pSelected->Select();
	}

	SetRedraw(TRUE); // 允许窗口重画

	// 下面这行代码会导致第31号Bug，需要去掉
	// OnSelectionChanged();

	if (sort != m_properetySort)
	{
		m_properetySort = sort;		
		((CPropertyGrid*)GetParent())->OnSortChanged();
	}
}

int CPropertyGridView::GetDividerPos()
{
	CPGWindowRect rcWindow(this);
	return int(m_dDivider * rcWindow.Width());
}

int  CPropertyGridView::InsertItem(CPropertyGridItem* pItem, int nIndex)
{
	nIndex = InsertString(nIndex, _T(""));
	ASSERT(nIndex != -1);

	SetItemDataPtr(nIndex, pItem);
	pItem->m_bVisible = TRUE;

	int nItemsInserted = 1;

	if (pItem->m_bExpanded)
	{
		nItemsInserted += _DoExpand(pItem, nIndex);		
	}

	return nItemsInserted;
}

int CPropertyGridView::_DoExpand(CPropertyGridItem* pItem, int nIndex)
{	
	int nStart = nIndex;
	POSITION pos = pItem->GetFirstChild();

	while(pos != NULL)
	{
		CPropertyGridItem* pChild = pItem->GetNextChild(pos);

		nIndex += InsertItem(pChild, nIndex + 1);		
	}	
	return nIndex - nStart;
}

void CPropertyGridView::_RefreshIndexes()
{	
	int i;
	for (i = 0; i < GetCount(); i++)
	{
		CPropertyGridItem* pItem = (CPropertyGridItem*)GetItemDataPtr(i);
		ASSERT(pItem);
		pItem->m_nIndex = i;
	}
}

void CPropertyGridView::_DoCollapse(CPropertyGridItem* pItem)
{	
	ASSERT(pItem);
	ASSERT(pItem->m_bExpanded);
	int nIndex = pItem->m_nIndex + 1;

	while(nIndex < GetCount())
	{
		CPropertyGridItem* pChild = (CPropertyGridItem*)GetItemDataPtr(nIndex);
		ASSERT(pChild);
		if (!pChild || !pChild->HasParent(pItem))
			break;

		pChild->m_bVisible = FALSE;
		DeleteString(nIndex);
	}	
	_RefreshIndexes();
}

CPropertyGridItem* CPropertyGridView::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetCount())
		return 0;

	CPropertyGridItem* pItem = (CPropertyGridItem*)GetItemDataPtr(nIndex);

	ASSERT(pItem);

	// 下面这行代码会导致第29号Bug，需要去掉
	// ASSERT(pItem->m_nIndex == nIndex);

	return pItem;
}

void CPropertyGridView::SwitchExpandState(int nItem)
{
	CPropertyGridItem* pItem = GetItem(nItem);
	if (!pItem) return;

	if(pItem->m_bExpanded) pItem->Collapse(); else pItem->Expand();
}

void CPropertyGridView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (HitTest(point) == PGV_HIT_SPLITTER)
	{
		SetFocus();
		SetCapture();
		if (m_pSelected) m_pSelected->OnValidateEdit();

		m_bTracking = TRUE;
		return;
	}

	CPropertyGridItem* pItem = ItemFromPoint(point);
	if (pItem && pItem->OnLButtonDown(nFlags, point))
		return;

	CListBox::OnLButtonDown(nFlags, point);		
}

CPropertyGridItem* CPropertyGridView::ItemFromPoint(CPoint point)
{
	BOOL bOutside = FALSE;
	int nIndex = CListBox::ItemFromPoint(point, bOutside);
	if (nIndex != -1 && !bOutside)
	{
		return GetItem(nIndex);
	}
	return NULL;
}

void CPropertyGridView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		CPGWindowRect rcWindow(this);
		m_dDivider = (double)point.x / rcWindow.Width();
		m_dDivider = __max(m_dDivider, .1);
		m_dDivider = __min(m_dDivider, .85);
		Invalidate(FALSE);
	}
	else
	{
	
		ShowToolTip(point);
	}

	CListBox::OnMouseMove(nFlags, point);
}

void CPropertyGridView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = FALSE;
	}

	CListBox::OnLButtonUp(nFlags, point);
}

void CPropertyGridView::OnCaptureChanged(CWnd *pWnd)
{
	m_bTracking = FALSE;

	CListBox::OnCaptureChanged(pWnd);
}

void CPropertyGridView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDblClk(nFlags, point);

	CPropertyGridItem* pItem = ItemFromPoint(point);

	if (pItem)
	{
		pItem->OnLButtonDblClk();
	}
}

CPropertyGridItem* CPropertyGridView::GetSelectedItem()
{
	return GetItem(GetCurSel());
}

BOOL CPropertyGridView::PreTranslateMessage(MSG* pMsg)
{
	if (GetFocus() == this && (pMsg->message == WM_KEYDOWN))
	{
		if (pMsg->wParam == VK_RETURN )
		{	
			SwitchExpandState(GetCurSel());
			return TRUE;
		} 
		else if (pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_ADD)
		{
			CPropertyGridItem* pItem = GetSelectedItem();
			if (pItem && pItem->HasChilds())
			{
				if (!pItem->m_bExpanded)
				{
					pItem->Expand();
					return TRUE;
				}
				if (pMsg->wParam == VK_ADD) return TRUE;				
			}
		} 
		else if (pMsg->wParam == VK_LEFT || pMsg->wParam == VK_SUBTRACT)
		{
			CPropertyGridItem* pItem = GetSelectedItem();
			if (pItem && pItem->HasChilds())
			{
				if (pItem->m_bExpanded)
				{
					pItem->Collapse();
					return TRUE;
				}
				if (pMsg->wParam == VK_SUBTRACT)  return TRUE;
			} 
		}
	}

	if (GetFocus() == this && ((pMsg->message == WM_CHAR) ||
		(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )))
	{
		CPropertyGridItem* pItem = GetSelectedItem();
		if (pItem)
		{
			if (pItem->OnChar((UINT)pMsg->wParam))
				return TRUE;			
		}
	}

	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible())
	{
		RelayToolTipEvent(pMsg);
	}

	return CListBox::PreTranslateMessage(pMsg);
}

void CPropertyGridView::OnSelectionChanged()
{
	CPropertyGridItem* pItem = GetSelectedItem();

	if (m_pSelected) m_pSelected->OnDeselect();
	if (pItem) pItem->OnSelect();

	m_pSelected = pItem;

	((CPropertyGrid*)GetParent())->OnSelectionChanged(m_pSelected);

	Invalidate(FALSE);
}

int CPropertyGridView::HitTest(CPoint point)
{
	CPGWindowRect rcWindow(this);

	int nWidth =  int(m_dDivider * rcWindow.Width());
	if (point.x > nWidth - 4 && point.x <= nWidth + 2 && GetCount() > 0)
		return PGV_HIT_SPLITTER;

	return -1;
}

BOOL CPropertyGridView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT)
	{
		CPoint point;

		GetCursorPos(&point);
		ScreenToClient(&point);

		if (HitTest(point) == PGV_HIT_SPLITTER)
		{
			static HCURSOR hcurHSplitBar = ::LoadCursor( AfxGetInstanceHandle(), MAKEINTRESOURCE(PG_IDC_HSPLITBAR) );
			ASSERT(hcurHSplitBar);
			SetCursor(hcurHSplitBar);
			return TRUE;
		}
	}

	return CListBox::OnSetCursor(pWnd, nHitTest, message);
}

void CPropertyGridView::OnSize(UINT nType, int cx, int cy)
{
	CListBox::OnSize(nType, cx, cy);

	// OnSelectionChanged();
	// 原来的代码会导致第32号Bug，替换成下列代码
	CPropertyGridItem* pItem = GetSelectedItem();
	if (m_pSelected) m_pSelected->OnDeselect();
	if (pItem) pItem->OnSelect();
	m_pSelected = pItem;
}

// ToolTips routings

void CPropertyGridView::_ShowToolTip(CRect rcBound, CRect rcText)
{
	TOOLINFO ti = { sizeof (TOOLINFO),  TTF_ABSOLUTE|TTF_TRACK|TTF_TRANSPARENT, m_hWnd, 1};
	ti.lpszText = (LPTSTR)(LPCTSTR)m_strTipText;
	m_wndTip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
	m_wndTip.SetToolInfo(&ti);
	m_wndTip.Activate(TRUE);

	m_wndTip.SendMessage(TTM_TRACKPOSITION, 0, MAKELONG(rcText.left, rcText.top));
	m_wndTip.SendMessage(TTM_TRACKACTIVATE,(WPARAM)TRUE,(LPARAM)&ti);

	m_rcToolTip = rcBound;

	TRACKMOUSEEVENT tme = {sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0};
	_TrackMouseEvent (&tme);
}

CSize CPropertyGridView::_GetTextExtent(CString str)
{
	CWindowDC dc(this);
	CPGFontDC font(&dc, GetFont());
	return dc.GetTextExtent(str);
}

void CPropertyGridView::ShowToolTip(CPoint pt)
{
	if (!m_wndTip.GetSafeHwnd())
	{
		m_wndTip.Create(this);
		m_wndTip.SetFont(GetFont(), FALSE);
	}

	CPropertyGridItem* pItem = ItemFromPoint(pt);

	if (!pItem || pItem->IsCategory())
		return;

	CPGWindowRect rcWindow(this);
	CRect rc = pItem->GetItemRect();

	CRect rcCaption(rc.left, rc.top, rc.left + int(m_dDivider * rcWindow.Width()), rc.bottom);

	if (rcCaption.PtInRect(pt))
	{
		// if (m_strTipText == pItem->GetCaption())
		// 原来的判断条件有问题，改进如下
		if (m_strTipText == pItem->GetCaption() && m_wndTip.IsWindowVisible())
			return;

		m_strTipText = pItem->GetCaption();

		ClientToScreen(&rcCaption);

		CRect rcCaptionText(rcCaption);
		rcCaptionText.left += pItem->m_nIndent * PGI_EXPAND_BORDER + 3 - 3;
		rcCaptionText.top--;	

		if (_GetTextExtent(m_strTipText).cx + 3 > rcCaptionText.Width())
		{
			_ShowToolTip(rcCaption, rcCaptionText);
		}
	}
	else
	{
		// if (m_strTipText == pItem->GetValue())
		// 原来的判断条件有问题，改进如下
		if (m_strTipText == pItem->GetValue() && m_wndTip.IsWindowVisible())
			return;

		m_strTipText = pItem->GetValue();

/*		if (GetFocus() && GetFocus()->IsKindOf(RUNTIME_CLASS(CPropertyGridInplaceEdit)))
		{		
			if (((CPropertyGridInplaceEdit*)(GetFocus()))->GetItem() == pItem)
				return;
		}	*/

		// 上述代码判断如果属性项正处于编辑状态，则不显示工具提示。
		// 由于自定义的扩展属性项使用了自定义内置编辑控件，上述代码
		// 不起作用，改进如下：
		CWnd* pWnd = GetFocus();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CEdit)) && IsChild(pWnd) && pItem == m_pSelected)
			return;

		CRect rcValue(rc.left + int(m_dDivider * rcWindow.Width()), rc.top, rc.right, rc.bottom);
		ClientToScreen(&rcValue);

		CRect rcValueText = pItem->GetValueRect();
		rcValueText.top -= 2;	
		ClientToScreen(&rcValueText);

		if (_GetTextExtent(m_strTipText).cx + 3 > rcValueText.Width())
		{
			_ShowToolTip(rcValue, rcValueText);					
		}	
	}
}

void CPropertyGridView::RelayToolTipEvent(MSG* pMsg)
{
	if (m_wndTip.GetSafeHwnd() && m_wndTip.IsWindowVisible())
	{
		CPoint pt;
		GetCursorPos(&pt);

		if (!m_rcToolTip.PtInRect(pt))
		{
			m_strTipText = _T("");
			m_wndTip.Activate(FALSE);
		}
			
		switch (pMsg->message)
		{
			case WM_MOUSEWHEEL:
				m_strTipText = _T("");
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN: 
			case WM_MBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				m_wndTip.Activate(FALSE);
		}
	}
}
