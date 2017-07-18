
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
	int*	m_pBindNumber;	// �󶨵�����������ͱ���

	BOOL	m_bHasMinValue;	// ��ʶ�Ƿ�����Сֵ
	BOOL	m_bHasMaxValue;	// ��ʶ�Ƿ������ֵ
	int		m_nMinValue;	// ���������Сֵ
	int		m_nMaxValue;	// ����������ֵ

public:

	// Call this method to change item's value
	//
	void SetNumber(
		// The new integer value of the item. 			
		int nValue);

	// Call this method to get the integer value of the item.
	// 
	int GetNumber();

	// ��һ�����ͱ����󶨵�������
	void BindToNumber(int* pBindNumber)
	{
		m_pBindNumber = pBindNumber;
		if (m_pBindNumber)
			SetNumber(*m_pBindNumber);
	}

	// �������������Сֵ
	void SetMinNumber(int nMinValue);

	// ��������������ֵ
	void SetMaxNumber(int nMaxValue);

protected:
	void _Init();
	virtual void SetValue(CString strValue);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE int CPropertyGridItemNumber::GetNumber() {
	return m_nValue;
}
