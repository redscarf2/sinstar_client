
#include "StdAfx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemColor.h"
#include "PropertyGrid.h"
#include "PropertyGrid_DrawHelpers.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemColor

CPropertyGridItemColor::CPropertyGridItemColor(CString strCaption, COLORREF clr)
	: CPropertyGridItem(strCaption)
{
	m_pBindColor = NULL;

	SetColor(clr);
	m_nFlags = pgitemHasEdit | pgitemHasExpandButton;
}

CPropertyGridItemColor::CPropertyGridItemColor(UINT nID, COLORREF clr)
	: CPropertyGridItem(nID)
{
	m_pBindColor = NULL;

	SetColor(clr);
	m_nFlags = pgitemHasEdit | pgitemHasExpandButton;
}

CPropertyGridItemColor::~CPropertyGridItemColor(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// 

BOOL CPropertyGridItemColor::OnDrawItemValue(CDC& dc, CRect rcValue)
{
	COLORREF clr = dc.GetTextColor();
	CRect rcSample(rcValue.left - 2, rcValue.top + 1, rcValue.left + 18, rcValue.bottom - 1);
	CPGPenDC pen(dc, clr);
	CPGBrushDC brush(dc, m_clrValue);
	dc.Rectangle(rcSample);

	CRect rcText(rcValue);
	rcText.left += 25;

	dc.DrawText( m_strValue, rcText,  DT_SINGLELINE|DT_VCENTER);

	return TRUE;
}

CRect CPropertyGridItemColor::GetValueRect()
{
	CRect rcValue(CPropertyGridItem::GetValueRect());
	rcValue.left += 25;
	return rcValue;
}

COLORREF CPropertyGridItemColor::StringToRGB(CString str)
{	
	CString strRed, strGreen, strBlue;
	
	AfxExtractSubString(strRed,   str, 0, _T(';') );
	AfxExtractSubString(strGreen, str, 1, _T(';') );
	AfxExtractSubString(strBlue,  str, 2, _T(';') );

	return RGB( __max(__min(_ttoi(strRed),   255), 0),	// ��֤R��G��B����
				__max(__min(_ttoi(strGreen), 255), 0),	// ��ɫ����λ��
				__max(__min(_ttoi(strBlue),  255), 0));	// 0��255֮��
}

CString CPropertyGridItemColor::RGBToString(COLORREF clr)
{
	CString str;
	str.Format(_T("%i; %i; %i"), GetRValue(clr), GetGValue(clr), GetBValue(clr));
	return str;
}

void CPropertyGridItemColor::SetValue(CString strValue)
{	
	SetColor(StringToRGB(strValue));
}

void CPropertyGridItemColor::SetColor(COLORREF clr)
{
	m_clrValue = clr;
	CPropertyGridItem::SetValue(RGBToString(clr));

	if (m_pBindColor)
		*m_pBindColor = m_clrValue;
}

void CPropertyGridItemColor::OnInplaceButtonDown()
{
	// ��������ڱ༭״̬ʱ�����ť����������ֵ��û���£�
	// ������ɫ�Ի���ĳ�ʼ��ɫֵʹ�õĻ����޸�ǰ������ֵ��
	// ����취�ǰ����뽹�㴫�����Ա�ǿ�Ƹ�������ֵ��
	m_pGrid->SetFocus();

	CColorDialog dlg( m_clrValue);
	if ( dlg.DoModal( ) == IDOK )
		OnValueChanged( RGBToString( dlg.GetColor( ) ) );
}
