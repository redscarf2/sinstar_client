
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemList::CPropertyGridItemList(CString strCaption)
	: CPropertyGridItem(strCaption)
{
	SetFlags(pgitemHasComboButton); // 设置属性项不可编辑、有组合框
	m_pBindListIndex = NULL;
}

CPropertyGridItemList::CPropertyGridItemList(UINT nID)
	: CPropertyGridItem(nID)
{
	SetFlags(pgitemHasComboButton); // 设置属性项不可编辑、有组合框
	m_pBindListIndex = NULL;
}

CPropertyGridItemList::~CPropertyGridItemList()
{

}

void CPropertyGridItemList::SetListIndex(int value)
{
	if (value == -1)
	{
		// 不选中
		CPropertyGridItem::SetValue( _T("") );
	}
	else
	{
		// 从属性项的下拉列表对象(派生自CStringList)获取指定索引处的字符串
		POSITION pos = GetConstraints()->FindIndex(value);
		ASSERT(pos);
		CPropertyGridItem::SetValue(GetConstraints()->GetAt(pos));
	}

	if (m_pBindListIndex)
		*m_pBindListIndex = value;
}

int CPropertyGridItemList::GetListIndex()
{
	return GetConstraints()->FindConstraint( GetValue() );
}

void CPropertyGridItemList::BindToListIndex(int* pBindListIndex)
{
	m_pBindListIndex = pBindListIndex;
	if (m_pBindListIndex)
		SetListIndex(*m_pBindListIndex);
}

void CPropertyGridItemList::SetValue(CString strValue)
{
	CPropertyGridItem::SetValue(strValue);

	if (m_pBindListIndex)
		*m_pBindListIndex = GetListIndex();
}
