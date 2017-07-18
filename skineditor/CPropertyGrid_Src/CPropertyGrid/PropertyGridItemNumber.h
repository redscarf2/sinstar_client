
#pragma once

//////////////////////////////////////////////////////////////////////
// CPropertyGridItemNumber is a CPropertyGridItem derived class.  
// It is used to create integer value item in a Property Grid control.
//
class CPropertyGridItemNumber : public CPropertyGridItem
{
public:

	// Constructs a CPropertyGridItemNumber object.
	//
	CPropertyGridItemNumber(
		// Caption of the item.
		CString strCaption,
		// Initial value
		int nValue = 0);

	// Constructs a CPropertyGridItemNumber object.
	//
	CPropertyGridItemNumber(
		// Identifier of the item.
		UINT nID,
		// Initial value
		int nValue = 0);
	
	// Destroys a CPropertyGridItemNumber object.
	//
	virtual ~CPropertyGridItemNumber(void);

private:

	int		m_nValue;
	int*	m_pBindNumber;	// 绑定到属性项的整型变量

	BOOL	m_bHasMinValue;	// 标识是否有最小值
	BOOL	m_bHasMaxValue;	// 标识是否有最大值
	int		m_nMinValue;	// 属性项的最小值
	int		m_nMaxValue;	// 属性项的最大值

public:

	// Call this method to change item's value
	//
	void SetNumber(
		// The new integer value of the item. 			
		int nValue);

	// Call this method to get the integer value of the item.
	// 
	int GetNumber();

	// 把一个整型变量绑定到属性项
	void BindToNumber(int* pBindNumber)
	{
		m_pBindNumber = pBindNumber;
		if (m_pBindNumber)
			SetNumber(*m_pBindNumber);
	}

	// 设置属性项的最小值
	void SetMinNumber(int nMinValue);

	// 设置属性项的最大值
	void SetMaxNumber(int nMaxValue);

protected:
	void _Init();
	virtual void SetValue(CString strValue);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CPropertyGridItemNumber::GetNumber() {
	return m_nValue;
}
