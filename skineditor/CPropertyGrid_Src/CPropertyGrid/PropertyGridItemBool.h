
#pragma once

//////////////////////////////////////////////////////////////////////
// CPropertyGridItemBool is a CPropertyGridItem derived class.  
// It is used to create Boolean-value item in a Property Grid control.
//
class CPropertyGridItemBool : public CPropertyGridItem
{
public:

	// Constructs a CPropertyGridItemBool object.
	//
	CPropertyGridItemBool(
		// Caption of the item.
		CString strCaption,
		// Initial value
		BOOL bValue = FALSE);

	// Constructs a CPropertyGridItemBool object.
	//
	CPropertyGridItemBool(
		// Identifier of the item.
		UINT nID,
		// Initial value
		BOOL bValue = FALSE);

	// Destroys a CPropertyGridItemBool object.
	//
	virtual ~CPropertyGridItemBool(void);

	// Call this method to change item's value
	//
	void SetBool(
		// The new BOOL-value of the item. 	
		BOOL bValue);
	
	// Call this method to get the Boolean value of the item.
	// 
	BOOL GetBool();

	// 把一个布尔变量绑定到属性项
	void BindToBool(BOOL* pBindBool)
	{
		m_pBindBool = pBindBool;
		if (m_pBindBool)
			SetBool(*m_pBindBool);
	}

protected:
	void SetValue(CString strValue);
	void _Init(BOOL bValue);

private:
	BOOL	m_bValue;
	BOOL*	m_pBindBool;	// 绑定到属性项的布尔变量
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE BOOL CPropertyGridItemBool::GetBool() {
	return m_bValue;
}
