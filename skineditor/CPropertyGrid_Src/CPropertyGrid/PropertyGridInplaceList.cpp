
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGrid_DrawHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceList

CPropertyGridInplaceList::CPropertyGridInplaceList()
: m_pItem(0)
{
}

CPropertyGridInplaceList::~CPropertyGridInplaceList()
{
}

IMPLEMENT_DYNAMIC(CPropertyGridInplaceList, CListBox)

BEGIN_MESSAGE_MAP(CPropertyGridInplaceList, CListBox)
	//{{AFX_MSG_MAP(CPropertyGridInplaceList)
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPropertyGridInplaceList::Create(CPropertyGridItem* pItem, CRect rect)
{
	ASSERT(pItem && pItem->GetGrid());

	m_pItem = pItem;
	CRect rcValue = m_pItem->GetValueRect();
	CWnd* pParent = m_pItem->GetGrid();

	if (!m_hWnd)
	{
		CListBox::CreateEx(0, _T("LISTBOX"), _T(""), LBS_NOTIFY |WS_CHILD|WS_BORDER|WS_VSCROLL, CRect(0, 0, 0, 0), pParent, 0);
		SetOwner(pParent);
		SetFont(pParent->GetFont());
	}

	ResetContent();

	CPGItemConstraints* pList = m_pItem->GetConstraints();

	POSITION pos = pList->GetHeadPosition();
	int dx = rect.right - rcValue.left;

	CWindowDC dc(pParent);
	CPGFontDC font(&dc, pParent->GetFont());
	int nThumbLength = GetSystemMetrics(SM_CXHTHUMB);

	while (pos) 
	{
		CString str = pList->GetNext(pos);
		AddString(str);
		
		dx = __max(dx, dc.GetTextExtent(str).cx + nThumbLength * 2);
	}

	int nHeight = GetItemHeight(0);
	rect.top = rect.bottom;
	rect.bottom += nHeight * __min(10, GetCount()) + 2;
	rect.left = rect.right - __min(dx, rect.Width() - PGI_EXPAND_BORDER);
	pParent->ClientToScreen(&rect);

	// 根据系统的工作区调整列表框位置
	CRect rcWork;
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);
	if (rect.bottom > rcWork.bottom) rect.OffsetRect(0, - rect.Height() - rcValue.Height() - 3);
	if (rect.top    < rcWork.top)    rect.OffsetRect(0, rcWork.top - rect.top);
	if (rect.left   < rcWork.left)   rect.OffsetRect(rcWork.left - rect.left, 0);
	if (rect.right  > rcWork.right)  rect.OffsetRect(rcWork.right - rect.right, 0);

	// 以子窗口方式创建内置列表框，然后再执行下面这段代码，内置列表
	// 框将以弹出窗口方式显示，并得到输入焦点，但当前窗口保持不变
	SetFocus();
	::SetWindowLong(m_hWnd, GWL_HWNDPARENT, NULL);
	ModifyStyle(WS_CHILD, WS_POPUP);
	::SetWindowLong(m_hWnd, GWL_HWNDPARENT, (LONG)pParent->m_hWnd);
	SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

	// 要在显示了列表框之后再选择列表项，因为属性项处于编辑状态时，
	// 编辑控件在激活列表框时失去焦点，从而更新属性值
	SetCurSel( pList->FindConstraint( m_pItem->GetValue() ) );

	CPGMouseMonitor::SetupHook(this);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceList message handlers

void CPropertyGridInplaceList::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

	CPGMouseMonitor::SetupHook(NULL);
	DestroyItem();
}

int CPropertyGridInplaceList::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// 用鼠标点击内置列表框时，不改变当前窗口
	return MA_NOACTIVATE;
}

void CPropertyGridInplaceList::OnLButtonUp(UINT, CPoint point)
{
	CPGClientRect rc(this);
	if (rc.PtInRect(point))	Apply(); else Cancel();
}

void CPropertyGridInplaceList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPropertyGridInplaceList::Cancel(void)
{
	GetOwner()->SetFocus();
}

void CPropertyGridInplaceList::Apply(void)
{
	int nIndex = GetCurSel();
	if (nIndex != LB_ERR)
	{
		CString str;
		GetText(nIndex, str);
		
		if (str != m_pItem->GetValue())
			m_pItem->OnValueChanged(str);
	}
	GetOwner()->SetFocus();
}

BOOL CPropertyGridInplaceList::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg && pMsg->message == WM_KEYDOWN && (GetFocus() == this))
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			Cancel();
			return TRUE;
		}
		if (pMsg->wParam == VK_RETURN)
		{
			Apply();
			return TRUE;
		}
	}

	return CListBox::PreTranslateMessage(pMsg);
}

void CPropertyGridInplaceList::DestroyItem()
{
	// reset variables to defaults.
	m_pItem = NULL;

	// destroy the window.
	DestroyWindow( );
}

//////////////////////////////////////////////////////////////////////////
// CPGMouseMonitor

HHOOK CPGMouseMonitor::m_hHookMouse  = NULL;
CWnd* CPGMouseMonitor::m_pWndMonitor = NULL;

void CPGMouseMonitor::SetupHook(CWnd* pWndMonitor)
{
	if (pWndMonitor && m_hHookMouse == NULL)
	{
		m_hHookMouse = ::SetWindowsHookEx( WH_MOUSE, MouseProc, NULL, ::GetCurrentThreadId() );
	}
	if (!pWndMonitor && m_hHookMouse)
	{
		::UnhookWindowsHookEx(m_hHookMouse);
		m_hHookMouse = NULL;
	}
	m_pWndMonitor = pWndMonitor;
}

LRESULT CALLBACK CPGMouseMonitor::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HC_ACTION || !m_pWndMonitor)
		return ::CallNextHookEx(m_hHookMouse, nCode, wParam, lParam);

	CPGWindowRect rc(m_pWndMonitor);
	PMOUSEHOOKSTRUCT pInfo = (PMOUSEHOOKSTRUCT)lParam;

	// if ( !rc.PtInRect(pInfo->pt) )
	// 原来的代码会导致第40号Bug，替换成如下代码
	if ( !rc.PtInRect(pInfo->pt) && pInfo->hwnd != m_pWndMonitor->m_hWnd )
	{
		switch(wParam)
		{
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
			m_pWndMonitor->GetOwner()->SetFocus();
			return FALSE;
		}
	}

	return ::CallNextHookEx(m_hHookMouse, nCode, wParam, lParam);
}
