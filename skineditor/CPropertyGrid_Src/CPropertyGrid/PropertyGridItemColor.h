
#pragma once

//////////////////////////////////////////////////////////////////////
// CPropertyGridItemColor is a CPropertyGridItem derived class.  
// It is used to create RGB-value item in a Property Grid control.
//
class CPropertyGridItemColor : public CPropertyGridItem
{
public:

	// Constructs a CPropertyGridItemColor object.
	//
	CPropertyGridItemColor(
		// Caption of the item
		CString strCaption,
		// Initial value
		COLORREF clr = 0);

	// Constructs a CPropertyGridItemColor object.
	//
	CPropertyGridItemColor(
		// Identifier of the item
		UINT nID,
		// Initial value
		COLORREF clr = 0);

	// Destroys a CPropertyGridItemColor object.
	//
	virtual ~CPropertyGridItemColor(void);

private:

	COLORREF	m_clrValue;
	COLORREF*	m_pBindColor;	// 绑定到属性项的颜色对象

public:

	// Call this method to change item's value
	//
	void SetColor(
		// The new RGB-value of the item. 	
		COLORREF clr);

	// Call this method to get the RGB value of the item.
	// 
	COLORREF GetColor();

	// The StringToRGB function converts a string to a COLORREF.
	static COLORREF StringToRGB(
		// String that is converted to a COLORREF
		CString str);

	// The RGBToString function converts a COLORREF to a string.
	static CString RGBToString(
		// COLORREF that is converted to a string
		COLORREF clr);

	// 把一个颜色对象绑定到属性项
	void BindToColor(COLORREF* pBindColor)
	{
		m_pBindColor = pBindColor;
		if (m_pBindColor)
			SetColor(*m_pBindColor);
	}

protected:

	virtual BOOL OnDrawItemValue(CDC& dc, CRect rcValue);
	virtual CRect GetValueRect();
	virtual void SetValue(CString strValue);
	virtual void OnInplaceButtonDown();

};

//////////////////////////////////////////////////////////////////////

AFX_INLINE COLORREF CPropertyGridItemColor::GetColor() {
	return m_clrValue;
}
