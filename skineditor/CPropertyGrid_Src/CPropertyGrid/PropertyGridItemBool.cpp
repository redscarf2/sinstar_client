
#include "StdAfx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGriditemBool.h"

const TCHAR TRUE_VALUE[]  = _T("True");
const TCHAR FALSE_VALUE[] = _T("False");

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemBool

CPropertyGridItemBool::CPropertyGridItemBool(CString strCaption, BOOL bValue)
	: CPropertyGridItem(strCaption)
{
	_Init(bValue);
}

CPropertyGridItemBool::CPropertyGridItemBool(UINT nID, BOOL bValue)
	: CPropertyGridItem(nID)
{
	_Init(bValue);
}

CPropertyGridItemBool::~CPropertyGridItemBool(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// 

void CPropertyGridItemBool::_Init(BOOL bValue)
{
	m_pBindBool = NULL;

	SetBool(bValue);
	m_nFlags = pgitemHasComboButton | pgitemHasEdit;
	
	m_lstContraints.AddConstraint(TRUE_VALUE);
	m_lstContraints.AddConstraint(FALSE_VALUE);
}

void CPropertyGridItemBool::SetValue(CString strValue)
{	
	SetBool(strValue.CompareNoCase(TRUE_VALUE) == 0);
}

void CPropertyGridItemBool::SetBool(BOOL bValue)
{
	m_bValue = bValue;
	CPropertyGridItem::SetValue(bValue? TRUE_VALUE: FALSE_VALUE);

	if (m_pBindBool)
		*m_pBindBool = m_bValue;
}
