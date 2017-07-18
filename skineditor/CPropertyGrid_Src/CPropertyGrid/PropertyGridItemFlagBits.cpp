#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemFlagBits.h"
#include "PropertyGrid_DrawHelpers.h"

// ��־λ�ķָ���
const TCHAR	FLAG_BITS_SEPARATOR = _T('|');

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceCheckList
/////////////////////////////////////////////////////////////////////////////

CPropertyGridInplaceCheckList::CPropertyGridInplaceCheckList()
	: m_pItem(0)
{
}

CPropertyGridInplaceCheckList::~CPropertyGridInplaceCheckList()
{
}

BEGIN_MESSAGE_MAP(CPropertyGridInplaceCheckList, CCheckListBox)
	//{{AFX_MSG_MAP(CPropertyGridInplaceCheckList)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPropertyGridInplaceCheckList::Create(CPropertyGridItem* pItem, CRect rect, BOOL checkStates[])
{
	ASSERT(pItem && pItem->GetGrid());

	m_pItem = pItem;
	m_bCanceled = FALSE;
	CRect rcValue = m_pItem->GetValueRect();
	CWnd* pParent = m_pItem->GetGrid();

	if (!m_hWnd)
	{
		CCheckListBox::Create(WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS, CRect(0, 0, 0, 0), pParent, 0);
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
		int index = AddString(str);
		SetCheck(index, checkStates[index]);
		
		dx = __max(dx, dc.GetTextExtent(str).cx + nThumbLength * 2);
	}

	int nHeight = GetItemHeight(0);
	rect.top = rect.bottom;
	rect.bottom += nHeight * __min(10, GetCount()) + 2;
	rect.left = rect.right - __min(dx, rect.Width() - PGI_EXPAND_BORDER);
	pParent->ClientToScreen(&rect);

	// ����ϵͳ�Ĺ����������б��λ��
	CRect rcWork;
	::SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcWork, 0);
	if (rect.bottom > rcWork.bottom) rect.OffsetRect(0, - rect.Height() - rcValue.Height() - 3);
	if (rect.top    < rcWork.top)    rect.OffsetRect(0, rcWork.top - rect.top);
	if (rect.left   < rcWork.left)   rect.OffsetRect(rcWork.left - rect.left, 0);
	if (rect.right  > rcWork.right)  rect.OffsetRect(rcWork.right - rect.right, 0);

	// ���Ӵ��ڷ�ʽ���������б��Ȼ����ִ��������δ��룬�����б�
	// ���Ե������ڷ�ʽ��ʾ�����õ����뽹�㣬����ǰ���ڱ��ֲ���
	SetFocus();
	::SetWindowLong(m_hWnd, GWL_HWNDPARENT, NULL);
	ModifyStyle(WS_CHILD, WS_POPUP);
	::SetWindowLong(m_hWnd, GWL_HWNDPARENT, (LONG)pParent->m_hWnd);
	SetWindowPos(0, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);

	CPGMouseMonitor::SetupHook(this);
}

void CPropertyGridInplaceCheckList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_ESCAPE)
	{
		m_bCanceled = TRUE;
		GetOwner()->SetFocus();
	}

	CCheckListBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPropertyGridInplaceCheckList::OnKillFocus(CWnd* pNewWnd) 
{
	CCheckListBox::OnKillFocus(pNewWnd);

	if (!m_bCanceled)
		Apply();

	CPGMouseMonitor::SetupHook(NULL);
	DestroyItem();
}

int CPropertyGridInplaceCheckList::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	// ����������ø�ѡ�б��ʱ�����ı䵱ǰ����
	return MA_NOACTIVATE;
}

void CPropertyGridInplaceCheckList::Apply()
{
	CString strNewValue;
	CPGItemConstraints* pList = m_pItem->GetConstraints();
	for (int index = 0; index < GetCount(); index++)
	{
		if ( GetCheck(index) )
		{
			POSITION pos = pList->FindIndex(index);
			CString text = pList->GetAt(pos);

			if (strNewValue.IsEmpty())
				strNewValue = text;
			else
				strNewValue += FLAG_BITS_SEPARATOR + text;
		}
	}

	if (strNewValue != m_pItem->GetValue())
		m_pItem->OnValueChanged(strNewValue);
}

void CPropertyGridInplaceCheckList::DestroyItem()
{
	m_pItem = NULL;
	DestroyWindow( );
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemFlagBits
/////////////////////////////////////////////////////////////////////////////

CPropertyGridItemFlagBits::CPropertyGridItemFlagBits(CString strCaption)
	: CPropertyGridItem(strCaption)
{
	_Init();
}

CPropertyGridItemFlagBits::CPropertyGridItemFlagBits(UINT nID)
	: CPropertyGridItem(nID)
{
	_Init();
}

CPropertyGridItemFlagBits::~CPropertyGridItemFlagBits()
{
}

void CPropertyGridItemFlagBits::_Init()
{
	SetFlags(pgitemHasComboButton); // ����������ɱ༭����������ť

	m_dwValue = 0;
	m_pBindFlagBits = NULL;
}

void CPropertyGridItemFlagBits::AddFlagItem(int nItemValue, CString nItemText)
{
	ASSERT( nItemText.GetLength() ); // ����Ϊ��
	ASSERT( nItemText.Find(FLAG_BITS_SEPARATOR) == -1 ); // ���ܰ����ָ���
	m_lstFlagItemValues.Add(nItemValue);
	GetConstraints()->AddConstraint(nItemText);
}

void CPropertyGridItemFlagBits::SetFlagBits(DWORD value)
{
	int count = (int)m_lstFlagItemValues.GetSize();
	ASSERT(count); // ��������ֵʱ��������ڱ�־λ������

	m_dwValue = value;
	CString strValue;
	for (int index = 0; index < count; index++)
	{
		if (m_dwValue & m_lstFlagItemValues[index])
		{
			POSITION pos = GetConstraints()->FindIndex(index);
			CString text = GetConstraints()->GetAt(pos);

			if (strValue.IsEmpty())
				strValue = text;
			else
				strValue += FLAG_BITS_SEPARATOR + text;
		}
	}

	CPropertyGridItem::SetValue(strValue);

	if (m_pBindFlagBits)
		*m_pBindFlagBits = m_dwValue;
}

DWORD CPropertyGridItemFlagBits::GetFlagBits()
{
	int count = (int)m_lstFlagItemValues.GetSize();
	ASSERT(count); // ��ȡ����ֵʱ��������ڱ�־λ������

	return m_dwValue;
}

void CPropertyGridItemFlagBits::BindToFlagBits(DWORD* pBindFlagBits)
{
	m_pBindFlagBits = pBindFlagBits;
	if (m_pBindFlagBits)
		SetFlagBits(*m_pBindFlagBits);
}

void CPropertyGridItemFlagBits::SetValue(CString strValue)
{
	int count = (int)m_lstFlagItemValues.GetSize();
	ASSERT(count);

	CStringList lstValue;
	SplitString(strValue, FLAG_BITS_SEPARATOR, lstValue);

	// �����־λ
	DWORD dwValue = 0;
	for (int index = 0; index < count; index++)
	{
		POSITION pos = GetConstraints()->FindIndex(index);
		CString text = GetConstraints()->GetAt(pos);
		if ( lstValue.Find(text) )
			dwValue |= m_lstFlagItemValues[index];
	}

	SetFlagBits(dwValue);
}

void CPropertyGridItemFlagBits::OnInplaceButtonDown()
{
	int count = (int)m_lstFlagItemValues.GetSize();
	ASSERT(count); // ��������ֵʱ��������ڱ�־λ������

	BOOL* checkStates = new BOOL [count];
	for (int index = 0; index < count; index++)
	{
		if (m_dwValue & m_lstFlagItemValues[index])
			checkStates[index] = TRUE;
		else
			checkStates[index] = FALSE;
	}

	m_wndCheckList.Create(this, GetItemRect(), checkStates);
	delete [] checkStates;
}

void CPropertyGridItemFlagBits::OnLButtonDblClk()
{
	// �������д�ú�����˫���ᵼ���б���������Ϊ����ֵ
}

void CPropertyGridItemFlagBits::SplitString(CString str, TCHAR separator, CStringList& splittedStrings)
{
	CString tempStr = str;
	int index;
	while ((index = tempStr.Find(separator)) != -1)
	{
		splittedStrings.AddTail( tempStr.Left(index) );
		tempStr = tempStr.Mid(index + 1);
	}
	splittedStrings.AddTail( tempStr );
}
