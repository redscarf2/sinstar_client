
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
	SetFlags(pgitemHasComboButton); // ����������ɱ༭������Ͽ�
	m_pBindListIndex = NULL;
}

CPropertyGridItemList::CPropertyGridItemList(UINT nID)
	: CPropertyGridItem(nID)
{
	SetFlags(pgitemHasComboButton); // ����������ɱ༭������Ͽ�
	m_pBindListIndex = NULL;
}

CPropertyGridItemList::~CPropertyGridItemList()
{

}

void CPropertyGridItemList::SetListIndex(int value)
{
	if (value == -1)
	{
		// ��ѡ��
		CPropertyGridItem::SetValue( _T("") );
	}
	else
	{
		// ��������������б����(������CStringList)��ȡָ�����������ַ���
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
