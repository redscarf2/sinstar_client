#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemEnum.h"

CPropertyGridItemEnum::CPropertyGridItemEnum(CString strCaption)
	: CPropertyGridItem(strCaption)
{
	_Init();
}

CPropertyGridItemEnum::CPropertyGridItemEnum(UINT nID)
	: CPropertyGridItem(nID)
{
	_Init();
}

CPropertyGridItemEnum::~CPropertyGridItemEnum()
{
}

void CPropertyGridItemEnum::_Init()
{
	SetFlags(pgitemHasComboButton); // 设置属性项不可编辑、有组合框
	m_pBindEnum = NULL;
}

void CPropertyGridItemEnum::AddEnumItem(int nItemValue, CString nItemText)
{
	m_lstEnumItemValues.Add(nItemValue);
	GetConstraints()->AddConstraint(nItemText);
}

void CPropertyGridItemEnum::SetEnum(int value)
{
	int count = (int)m_lstEnumItemValues.GetSize();
	ASSERT(count); // 设置属性值时，必须存在枚举数据项

	// 查找枚举值对应的列表索引
	int index;
	for (index = 0; index < count; index++)
	{
		if (m_lstEnumItemValues[index] == value)
			break;
	}

	// 如果没有找到，则设为第一个枚举数据
	if (index == count)
		index = 0;

	// 从属性项的下拉列表对象(派生自CStringList)获取指定枚举数据项的文本
	POSITION pos = GetConstraints()->FindIndex(index);
	ASSERT(pos);
	CPropertyGridItem::SetValue(GetConstraints()->GetAt(pos));

	if (m_pBindEnum)
		*m_pBindEnum = m_lstEnumItemValues[index];
}

int CPropertyGridItemEnum::GetEnum()
{
	int index = GetConstraints()->FindConstraint( GetValue() );
	ASSERT(index != -1); // 获取属性值时，必须存在枚举数据项，且调用SetEnum设置了初值

	return m_lstEnumItemValues[index];
}

void CPropertyGridItemEnum::BindToEnum(int* pBindEnum)
{
	m_pBindEnum = pBindEnum;
	if (m_pBindEnum)
		SetEnum(*m_pBindEnum);
}

void CPropertyGridItemEnum::SetValue(CString strValue)
{
	CPropertyGridItem::SetValue(strValue);

	if (m_pBindEnum)
		*m_pBindEnum = GetEnum();
}
