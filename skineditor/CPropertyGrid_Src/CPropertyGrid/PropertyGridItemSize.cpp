
#include "StdAfx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemSize.h"
#include "PropertyGridItemNumber.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemSize::CPropertyGridItemSizeWidth

class CPropertyGridItemSize::CPropertyGridItemSizeWidth : public CPropertyGridItemNumber
{
public:
	CPropertyGridItemSizeWidth(CString strCaption)
		: CPropertyGridItemNumber(strCaption) {}
	
	virtual void OnValueChanged(CString strValue)
	{
		((CPropertyGridItemSize*)m_pParent)->SetWidth(strValue);      
	}
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemSize::CPropertyGridItemSizeHeight

class CPropertyGridItemSize::CPropertyGridItemSizeHeight : public CPropertyGridItemNumber
{
public:
	CPropertyGridItemSizeHeight(CString strCaption)
		: CPropertyGridItemNumber(strCaption) {}

	virtual void OnValueChanged(CString strValue)
	{
		((CPropertyGridItemSize*)m_pParent)->SetHeight(strValue);        
	}
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemSize

CPropertyGridItemSize::CPropertyGridItemSize(CString strCaption, CSize size)
	: CPropertyGridItem(strCaption)
{
	m_pBindSize = NULL;

	m_szValue = size;
	m_strValue = SizeToString(size);
}

CPropertyGridItemSize::CPropertyGridItemSize(UINT nID,  CSize size)
	: CPropertyGridItem(nID)
{
	m_pBindSize = NULL;

	m_szValue = size;
	m_strValue = SizeToString(size);
	
}

CPropertyGridItemSize::~CPropertyGridItemSize(void)
{

}

/////////////////////////////////////////////////////////////////////////////
// 

void CPropertyGridItemSize::OnAddChildItem()
{
	m_itemWidth = (CPropertyGridItemSizeWidth*)AddChildItem(new CPropertyGridItemSizeWidth(_T("Width")));
	m_itemHeight = (CPropertyGridItemSizeHeight*)AddChildItem(new CPropertyGridItemSizeHeight(_T("Height")));
	UpdateChilds();
}

CString CPropertyGridItemSize::SizeToString(CSize size)
{
	CString str;
	str.Format(_T("%i; %i"), size.cx, size.cy);
	return str;
}

CSize CPropertyGridItemSize::StringToSize(CString str)
{
	CString strWidth, strHeight;
	
	AfxExtractSubString(strWidth, str, 0, _T(';') );
	AfxExtractSubString(strHeight, str, 1, _T(';') );

	return CSize(_ttoi(strWidth), _ttoi(strHeight));
}

void CPropertyGridItemSize::SetValue(CString strValue)
{
	SetSize(StringToSize(strValue));
}

void CPropertyGridItemSize::SetSize(CSize size)
{
	m_szValue = size;
	CPropertyGridItem::SetValue(SizeToString(m_szValue));
	UpdateChilds();

	if (m_pBindSize)
		*m_pBindSize = m_szValue;
}

void CPropertyGridItemSize::UpdateChilds()
{
	m_itemWidth->SetNumber(m_szValue.cx);
	m_itemHeight->SetNumber(m_szValue.cy);
}

void CPropertyGridItemSize::SetWidth(CString strWidth)
{
	OnValueChanged(SizeToString(CSize(_ttoi(strWidth), m_szValue.cy)));

}

void CPropertyGridItemSize::SetHeight(CString strHeight)
{
	OnValueChanged(SizeToString(CSize(m_szValue.cx, _ttoi(strHeight))));
}
