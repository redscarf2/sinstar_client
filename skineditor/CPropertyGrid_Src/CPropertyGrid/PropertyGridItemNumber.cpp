
#include "StdAfx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemNumber.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemNumber

CPropertyGridItemNumber::CPropertyGridItemNumber(CString strCaption, int nValue)
	: CPropertyGridItem(strCaption)
{
	_Init();
	SetNumber(nValue);
}
CPropertyGridItemNumber::CPropertyGridItemNumber(UINT nID, int nValue)
	: CPropertyGridItem(nID)
{
	_Init();
	SetNumber(nValue);
}

CPropertyGridItemNumber::~CPropertyGridItemNumber(void)
{
}

/////////////////////////////////////////////////////////////////////////////
//

void CPropertyGridItemNumber::_Init()
{
	m_pBindNumber	= NULL;

	m_bHasMinValue	= FALSE;
	m_bHasMaxValue	= FALSE;
	m_nMinValue		= 0;
	m_nMaxValue		= 0;
}

void CPropertyGridItemNumber::SetValue(CString strValue)
{	
	SetNumber(_ttol(strValue));
}

void CPropertyGridItemNumber::SetNumber(int nValue)
{
	m_nValue = nValue;

	if (m_bHasMinValue && m_nValue < m_nMinValue)
		m_nValue = m_nMinValue;

	if (m_bHasMaxValue && m_nValue > m_nMaxValue)
		m_nValue = m_nMaxValue;
	
	CString strValue;
	strValue.Format(_T("%i"), m_nValue);
	CPropertyGridItem::SetValue(strValue);

	if (m_pBindNumber)
		*m_pBindNumber = m_nValue;
}

void CPropertyGridItemNumber::SetMinNumber(int nMinValue)
{
	m_bHasMinValue = TRUE;
	m_nMinValue = nMinValue;
	if (m_nValue < m_nMinValue)
		SetNumber(m_nMinValue);
}

void CPropertyGridItemNumber::SetMaxNumber(int nMaxValue)
{
	m_bHasMaxValue = TRUE;
	m_nMaxValue = nMaxValue;
	if (m_nValue > m_nMaxValue)
		SetNumber(m_nMaxValue);
}
