
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceEdit

CPropertyGridInplaceEdit::CPropertyGridInplaceEdit()
: m_pItem(0)
, m_pGrid(0)
, m_nDefaultMaxLength(0)
{
}

CPropertyGridInplaceEdit::~CPropertyGridInplaceEdit()
{
}

IMPLEMENT_DYNAMIC(CPropertyGridInplaceEdit, CEdit)

BEGIN_MESSAGE_MAP(CPropertyGridInplaceEdit, CEdit)
	//{{AFX_MSG_MAP(CPropertyGridInplaceEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnEnSetfocus)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceEdit message handlers

void CPropertyGridInplaceEdit::SetValue(CString strValue)
{
	m_strValue = strValue;
}

void CPropertyGridInplaceEdit::HideWindow()
{
	if (m_hWnd)
	{
		ShowWindow(SW_HIDE);
	}
}

void CPropertyGridInplaceEdit::Create(CPropertyGridItem* pItem, CRect rect, UINT nMaxLength)
{
	ASSERT(pItem && pItem->GetGrid());

	m_pGrid = pItem->GetGrid();

	if (!m_hWnd)
	{
		CEdit::Create(WS_CHILD|ES_AUTOHSCROLL, rect, m_pGrid, 0);
		m_nDefaultMaxLength = GetLimitText();
		SetFont(m_pGrid->GetFont());
	}
	SetWindowText(m_strValue);
	SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_SHOWWINDOW);

	SetMargins(3, 0);
	
	m_pItem = pItem;

	// 设置编辑框的最大字符数
	if (nMaxLength == 0)
		SetLimitText(m_nDefaultMaxLength);	// 无限制
	else
		SetLimitText(nMaxLength);			// 有限制
}

HBRUSH CPropertyGridInplaceEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	COLORREF clr = ((CPropertyGridView*)m_pGrid)->m_clrBack;

	//pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(GetStyle() & ES_READONLY? GetSysColor(COLOR_GRAYTEXT): ((CPropertyGridView*)m_pGrid)->m_clrFore);
	pDC->SetBkColor(clr);

	m_brBack.DeleteObject();
	m_brBack.CreateSolidBrush(clr);
	
	return m_brBack;
}

void CPropertyGridInplaceEdit::OnEnSetfocus()
{
	m_pGrid->Invalidate(FALSE);
}

void CPropertyGridInplaceEdit::OnEnKillfocus()
{
	if (m_pItem)
	{
		m_pItem->OnValidateEdit();
		m_pGrid->Invalidate(FALSE);
	}
}

BOOL CPropertyGridInplaceEdit::PreTranslateMessage(MSG* pMsg)
{
	if ((GetFocus() == this) && (pMsg->message == WM_KEYDOWN))
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			SetWindowText(m_strValue);
			m_pGrid->SetFocus();
			return TRUE;
		} 
		else if (pMsg->wParam == VK_RETURN)
		{
			m_pGrid->SetFocus();
			return TRUE;
		}
		else if (pMsg->wParam == VK_DOWN || pMsg->wParam == VK_UP)
		{
			if (SelectConstraint(pMsg->wParam == VK_DOWN? +1: -1, FALSE))
			{
				SetSel(0, -1);
				return TRUE;
			}			
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

BOOL CPropertyGridInplaceEdit::SelectConstraint(int nDirection, BOOL bCircle)
{
	CPGItemConstraints* pList = m_pItem->GetConstraints();
	if (pList->IsEmpty())
		return FALSE;

	CString str;
	GetWindowText(str);
	int nIndex = pList->FindConstraint( str );
	nIndex+= nDirection;
	if (nIndex >= pList->GetCount()) nIndex = bCircle ? 0 : (ULONG)pList->GetCount() - 1;
	if (nIndex < 0) nIndex = bCircle ? (ULONG)pList->GetCount() - 1 : 0;

	POSITION pos = pList->FindIndex(nIndex);
	SetWindowText( pList->GetAt(pos) );
	return TRUE;
}

void CPropertyGridInplaceEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// 修改后，增加了对只读属性的判断
	// if (SelectConstraint(+1, TRUE))
	if (m_pItem != NULL && !m_pItem->GetReadOnly() && SelectConstraint(+1, TRUE))
	{
		m_pGrid->SetFocus();
		return;
	}	
	CEdit::OnLButtonDblClk(nFlags, point);
}

void CPropertyGridInplaceEdit::DestroyItem()
{
	// reset variables to defaults.
	m_pItem = NULL;
	m_pGrid = NULL;
	m_strValue.Empty();
	m_brBack.DeleteObject();

	// destroy the window.
	DestroyWindow( );
}
