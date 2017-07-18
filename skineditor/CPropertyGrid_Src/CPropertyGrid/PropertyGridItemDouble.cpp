
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemDouble.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemDouble::CPropertyGridItemDouble(CString strCaption, double value, int precision)
	: CPropertyGridItem(strCaption)
{
	_Init();

	m_nPrecision = precision;
	SetDouble(value);
}

CPropertyGridItemDouble::CPropertyGridItemDouble(UINT nID, double value, int precision)
	: CPropertyGridItem(nID)
{
	_Init();

	m_nPrecision = precision;
	SetDouble(value);
}

CPropertyGridItemDouble::~CPropertyGridItemDouble()
{

}

void CPropertyGridItemDouble::_Init()
{
	m_pBindDouble = NULL;

	m_bHasMinValue	= FALSE;
	m_bHasMaxValue	= FALSE;
	m_fMinValue		= 0;
	m_fMaxValue		= 0;
}

void CPropertyGridItemDouble::SetDouble(double value)
{
	m_fValue = value;

	if (m_bHasMinValue && m_fValue < m_fMinValue)
		m_fValue = m_fMinValue;

	if (m_bHasMaxValue && m_fValue > m_fMaxValue)
		m_fValue = m_fMaxValue;

	CString strFormat;	//¸ñÊ½×Ö·û´®
	strFormat.Format( _T("%%.%df"), m_nPrecision);
	CString strValue;
	strValue.Format(strFormat, m_fValue);
	CPropertyGridItem::SetValue(strValue);

	if (m_pBindDouble)
		*m_pBindDouble = m_fValue;
}

double CPropertyGridItemDouble::GetDouble()
{
	return m_fValue;
}

void CPropertyGridItemDouble::SetValue(CString strValue)
{
#ifdef _UNICODE
	char ansi_string[256];
	::WideCharToMultiByte(CP_ACP, 0, strValue, -1, ansi_string, 256, NULL, NULL);
	SetDouble(atof(ansi_string));
#else
	SetDouble(atof(strValue));
#endif
}

void CPropertyGridItemDouble::BindToDouble(double* pBindDouble)
{
	m_pBindDouble = pBindDouble;
	if (m_pBindDouble)
		SetDouble(*m_pBindDouble);
}

void CPropertyGridItemDouble::SetMinDouble(double fMinValue)
{
	m_bHasMinValue = TRUE;
	m_fMinValue = fMinValue;
	if (m_fValue < m_fMinValue)
		SetDouble(m_fMinValue);
}

void CPropertyGridItemDouble::SetMaxDouble(double fMaxValue)
{
	m_bHasMaxValue = TRUE;
	m_fMaxValue = fMaxValue;
	if (m_fValue > m_fMaxValue)
		SetDouble(m_fMaxValue);
}
