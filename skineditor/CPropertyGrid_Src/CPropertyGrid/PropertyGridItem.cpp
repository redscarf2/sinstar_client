
#include "StdAfx.h"

#include "PropertyGrid_Defines.h"
#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItem

CPropertyGridItem::CPropertyGridItem(CString strCaption, LPCTSTR strValue, UINT nMaxLength)
{
	Init();
	SetCaption(strCaption);

	m_nMaxLength = nMaxLength;
	m_strValue = AdjustValue(strValue);
}

CPropertyGridItem::CPropertyGridItem(UINT nID, LPCTSTR strValue, UINT nMaxLength)
{
	Init();
    m_nID = nID;
	
	CString strText; 
	if (strText.LoadString(nID))
	{
		int nIndex = strText.Find( _T('\n') ); 
		if (nIndex == -1)
		{
			m_strCaption = strText;
		} else
		{
			m_strCaption = strText.Left(nIndex);
			m_strDescription = strText.Mid(nIndex + 1);
		}
	}

	m_nMaxLength = nMaxLength;
	m_strValue = AdjustValue(strValue);
}

CPropertyGridItem::~CPropertyGridItem(void)
{
	while (!m_lstChilds.IsEmpty())
	{
		delete m_lstChilds.RemoveTail();
	}
	if ( ::IsWindow( GetInplaceButton().m_hWnd ) )
	{
		GetInplaceButton().DestroyItem( );
	}
	if ( ::IsWindow( GetInplaceList().m_hWnd ) )
	{
		GetInplaceList().DestroyItem( );
	}
	if ( ::IsWindow( GetInplaceEdit().m_hWnd ) )
	{
		GetInplaceEdit().DestroyItem( );
	}
}

void CPropertyGridItem::Init()
{
	m_bExpanded = FALSE;
	m_nIndex = -1;
	m_bVisible = FALSE;
	m_pParent = 0;
	m_pGrid = NULL;
	m_nIndent = 0;
	m_strValue = _T("");
	m_bReadOnly = FALSE;
	m_bCategory = FALSE;
	m_nID = 0;
	m_nFlags = pgitemHasEdit;

	m_dwItemData  = 0;
	m_pBindString = NULL;
}

CString CPropertyGridItem::AdjustValue(CString strValue)
{
	if (m_nMaxLength > 0 && strValue.GetLength() > (int)m_nMaxLength)
		return strValue.Left(m_nMaxLength);
	else
		return strValue;
}

CPropertyGridItem* CPropertyGridItem::AddChildItem(CPropertyGridItem* pItem)
{
	m_lstChilds.AddTail(pItem);
	pItem->m_pGrid = m_pGrid;
	ASSERT(pItem->m_pParent == NULL);
	pItem->m_pParent = this;
	pItem->m_nIndent = m_nIndent + 1;

	pItem->OnAddChildItem();

	if (m_bVisible)
	{
		if (m_bExpanded)
			m_pGrid->SetPropertySort(m_pGrid->m_properetySort, TRUE);
		else if (m_pGrid->GetSafeHwnd() && m_lstChilds.GetCount() == 1)
			// 如果父项处于折叠状态，且添加的是第一个子项，则刷新窗口以显示父项左边的"+"
			m_pGrid->Invalidate(FALSE);
	}
	else
	{
		// 如果属性表按字母排序或者不排序，此时属性分类不可见，但为属性分类添加的子项要显示出来
		if (m_bCategory && m_pGrid->m_properetySort != pgsortCategorized)
			m_pGrid->SetPropertySort(m_pGrid->m_properetySort, TRUE);
	}

	return pItem;
}

void CPropertyGridItem::Collapse()
{
	if (!m_bExpanded) return;

	if (m_bVisible)
	{
		m_pGrid->_DoCollapse(this);

		// 下面这4行代码是为了解决第30号Bug
		CPropertyGridItem* pItem = m_pGrid->GetSelectedItem();
		if (m_pGrid->m_pSelected) m_pGrid->m_pSelected->OnDeselect();
		if (pItem) pItem->OnSelect();
		m_pGrid->m_pSelected = pItem;
	}

	m_bExpanded = FALSE;
}

void CPropertyGridItem::Expand()
{
	if (m_bExpanded) return;

	if (m_bVisible)
	{
		m_pGrid->_DoExpand(this, m_nIndex);
		m_pGrid->_RefreshIndexes();

		// 下面这4行代码是为了解决第30号Bug
		CPropertyGridItem* pItem = m_pGrid->GetSelectedItem();
		if (m_pGrid->m_pSelected) m_pGrid->m_pSelected->OnDeselect();
		if (pItem) pItem->OnSelect();
		m_pGrid->m_pSelected = pItem;
	}

	m_bExpanded = TRUE;
}

BOOL CPropertyGridItem::HasParent(CPropertyGridItem* pParent)
{
	if (m_pParent == NULL) return FALSE;
	if (m_pParent == pParent) return TRUE;
	return m_pParent->HasParent(pParent);
}

void CPropertyGridItem::SetReadOnly(BOOL bReadOnly)
{
	m_bReadOnly = bReadOnly;
	if (m_pGrid && m_pGrid->GetSafeHwnd())
	{
		// 判断属性项是否处于选中状态
		if (m_pGrid->GetSelectedItem() == this)
		{
			OnDeselect();
			OnSelect();
		}
		m_pGrid->Invalidate(FALSE);
	}
}

void CPropertyGridItem::Select()
{
	ASSERT(m_pGrid);

	if (m_bVisible)
	{
		m_pGrid->SetCurSel(m_nIndex);
		m_pGrid->OnSelectionChanged();
	}
}

void CPropertyGridItem::OnDeselect()
{
	GetInplaceButton().HideWindow();
	OnValidateEdit();
}

void CPropertyGridItem::OnValidateEdit()
{
	GetInplaceEdit().HideWindow();
	
	if (GetInplaceEdit().GetSafeHwnd() && GetInplaceEdit().m_pItem == this)
	{
		CString strValue;
		GetInplaceEdit().GetWindowText(strValue);

		if (m_strValue != strValue)
		{
			OnValueChanged(strValue);
			m_pGrid->Invalidate(FALSE);
		}
	}
}

void CPropertyGridItem::SetEditText(CString str)
{
	if (!m_pGrid) return;

	if (GetInplaceEdit().GetSafeHwnd() && GetInplaceEdit().m_pItem == this)
	{
		GetInplaceEdit().SetWindowText(str);
	}
}

void CPropertyGridItem::SetValue(CString strValue)
{
	m_strValue = AdjustValue(strValue);
	SetEditText(m_strValue);

	// 更新绑定的字符串对象
	if (m_pBindString)
		*m_pBindString = m_strValue;

	// 在设置完属性值后，强制刷新窗口
	if (m_pGrid && m_pGrid->m_hWnd) m_pGrid->Invalidate(FALSE);
}

void CPropertyGridItem::OnValueChanged(CString strValue)
{
	SetValue(strValue);

	// 发送属性值改变的通知消息
	int nID = m_pGrid->GetParent()->GetDlgCtrlID();
	WPARAM wParam = MAKEWPARAM(nID, PGN_ITEMVALUE_CHANGED);
	m_pGrid->GetParent()->GetOwner()->SendMessage(PGWM_PROPERTYGRID_NOTIFY, wParam, (LPARAM)this);
}

void CPropertyGridItem::OnSelect()
{
	ASSERT(m_bVisible);

	if (!m_bReadOnly && (m_nFlags & (pgitemHasComboButton | pgitemHasExpandButton)))
	{
		GetInplaceButton().Create(this, GetItemRect());
	}
	if (m_nFlags & pgitemHasEdit)
	{
		GetInplaceEdit().SetValue(m_strValue);
		GetInplaceEdit().Create(this, GetValueRect(), m_nMaxLength);
		GetInplaceEdit().SetReadOnly(m_bReadOnly);		
	}
	else
	{
		GetInplaceEdit().HideWindow();
	}
}

void CPropertyGridItem::OnInplaceButtonDown()
{
	if ((m_nFlags & pgitemHasComboButton) && !m_lstContraints.IsEmpty())
	{	
		GetInplaceList().Create(this, GetItemRect());
		m_pGrid->Invalidate(FALSE);
	}
}

CRect CPropertyGridItem::GetValueRect()
{
	ASSERT(m_bVisible);
	CRect rc = GetItemRect();
	CRect rcValue(m_pGrid->GetDividerPos() + 1, rc.top + 1, rc.right, rc.bottom - 1);
	if (m_nFlags & (pgitemHasComboButton | pgitemHasExpandButton))
	{
		if (GetInplaceButton().m_pItem == this && GetInplaceButton().GetStyle() & WS_VISIBLE)
		{		
			rcValue.right -= 15;
		}
	}

	return rcValue;
}

CRect CPropertyGridItem::GetItemRect()
{
	CRect rc;
	m_pGrid->GetItemRect(m_nIndex, rc);
	return rc;
}

BOOL CPropertyGridItem::OnChar(UINT nChar)
{
	if (m_nFlags & pgitemHasEdit)
	{
		OnSelect();
		GetInplaceEdit().SetFocus();
		GetInplaceEdit().SetSel(0, -1);
		if (nChar != VK_TAB) GetInplaceEdit().SendMessage(WM_CHAR, nChar);
		return TRUE;
	}
	return FALSE;
}

void CPropertyGridItem::OnLButtonDblClk()
{
	if (HasChilds())
	{
		if(m_bExpanded) Collapse(); else Expand();
	} 
	else
	{
		OnSelect();
		if (m_nFlags & pgitemHasEdit)
		{
			if (!GetReadOnly() && GetInplaceEdit().SelectConstraint(+1, TRUE))
			{
				OnValidateEdit();
			}
			else
			{
				GetInplaceEdit().SetFocus();
				GetInplaceEdit().SetSel(0, -1);
			}
		}
		else if (!GetReadOnly())
		{
			CPGItemConstraints* pList = GetConstraints();
			if (pList->IsEmpty())
				return ;

			int nIndex = pList->FindConstraint(m_strValue);
			nIndex += +1;
			if (nIndex >= pList->GetCount()) nIndex = 0;

			POSITION pos = pList->FindIndex(nIndex);
			OnValueChanged(pList->GetAt(pos));
		}
	}
}

BOOL CPropertyGridItem::OnLButtonDown(UINT, CPoint point)
{
	ASSERT(GetItemRect().PtInRect(point));

	m_pGrid->SetFocus();
	Select();

//	if(point.x <= PGI_EXPAND_BORDER)
//	{
//		if(m_bExpanded) Collapse(); else Expand();
//	}

	// 原来的代码只考虑了列表项为根结点的情况，没有
	// 考虑为子节点的情况，新代码修正了这个问题
	int nOffset = 0;
	if (m_nIndent > 1)
		nOffset = (m_nIndent - 1) * PGI_EXPAND_BORDER;

	if(point.x >= nOffset && point.x <= PGI_EXPAND_BORDER + nOffset)
	{
		if(m_bExpanded) Collapse(); else Expand();
	}

	CRect rc = GetValueRect();
	if ((m_nFlags & pgitemHasEdit) && rc.PtInRect(point))
	{	
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	}

	return TRUE;
}

CPropertyGridInplaceEdit& CPropertyGridItem::GetInplaceEdit()
{
	ASSERT(m_pGrid);
	return m_pGrid->m_wndEdit;
}

CPropertyGridInplaceButton& CPropertyGridItem::GetInplaceButton()
{
	ASSERT(m_pGrid);
	return m_pGrid->m_wndButton;
}

CPropertyGridInplaceList& CPropertyGridItem::GetInplaceList()
{
	ASSERT(m_pGrid);
	return m_pGrid->m_wndListBox;
}

/////////////////////////////////////////////////////////////////////////////
// CPGListItems

void CPGListItems::Sort()
{
	CPGListItems lstSorted;
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CPropertyGridItem* pItem = GetNext(pos);

		BOOL bFound = FALSE;
		POSITION posInsert = lstSorted.GetHeadPosition();
		while (posInsert)
		{
			if (lstSorted.GetNext(posInsert)->GetCaption() > pItem->GetCaption())
			{
				bFound = TRUE;
				break;
			}
		}

		if (bFound)
		{
			if (posInsert)
				lstSorted.GetPrev(posInsert);
			else
				posInsert = lstSorted.GetTailPosition();
			lstSorted.InsertBefore(posInsert, pItem);
		}
		else
		{
			lstSorted.AddTail(pItem);
		}
	}

	RemoveAll();
	AddTail(&lstSorted);
}

CPropertyGridItem* CPGListItems::FindItem(CString strCaption)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CPropertyGridItem* pItem = GetNext(pos);
		if (pItem->GetCaption() == strCaption)
			return pItem;
		
		CPropertyGridItem* pChild = pItem->m_lstChilds.FindItem(strCaption);
		if (pChild) return pChild;
	}	
	return NULL;
}

CPropertyGridItem* CPGListItems::FindItem(UINT nID)
{
	POSITION pos = GetHeadPosition();
	while (pos)
	{
		CPropertyGridItem* pItem = GetNext(pos);
		if (pItem->GetID() == nID)
			return pItem;
		
		CPropertyGridItem* pChild = pItem->m_lstChilds.FindItem(nID);
		if (pChild) return pChild;
	}	
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CPGItemConstraints

int CPGItemConstraints::FindConstraint(CString str)
{
	POSITION pos = GetHeadPosition();
	int nIndex = 0;
	while (pos)
	{
		CString strNext = GetNext(pos);
		if (str == strNext)
			return nIndex;
		nIndex++;
	}	
	return -1;
}
