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
	SetFlags(pgitemHasComboButton); // ����������ɱ༭������Ͽ�
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
	ASSERT(count); // ��������ֵʱ���������ö��������

	// ����ö��ֵ��Ӧ���б�����
	int index;
	for (index = 0; index < count; index++)
	{
		if (m_lstEnumItemValues[index] == value)
			break;
	}

	// ���û���ҵ�������Ϊ��һ��ö������
	if (index == count)
		index = 0;

	// ��������������б����(������CStringList)��ȡָ��ö����������ı�
	POSITION pos = GetConstraints()->FindIndex(index);
	ASSERT(pos);
	CPropertyGridItem::SetValue(GetConstraints()->GetAt(pos));

	if (m_pBindEnum)
		*m_pBindEnum = m_lstEnumItemValues[index];
}

int CPropertyGridItemEnum::GetEnum()
{
	int index = GetConstraints()->FindConstraint( GetValue() );
	ASSERT(index != -1); // ��ȡ����ֵʱ���������ö��������ҵ���SetEnum�����˳�ֵ

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
