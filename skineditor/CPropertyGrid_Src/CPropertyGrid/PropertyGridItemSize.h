
#pragma once

//////////////////////////////////////////////////////////////////////
// CPropertyGridItemSize is a CPropertyGridItem derived class.  
// It is used to create size-value item in a Property Grid control.
//
class CPropertyGridItemSize : public CPropertyGridItem
{
public:
	// Constructs a CPropertyGridItemSize object.
	//
	CPropertyGridItemSize(
		// Caption of the item.
		CString strCaption,
		// Initial value
		CSize size = 0);

	// Constructs a CPropertyGridItemSize object.
	//
	CPropertyGridItemSize(
		// Identifier of the item.
		UINT nID,
		// Initial value
		CSize size = 0);

	// Destroys a CPropertyGridItemSize object.
	//
	virtual ~CPropertyGridItemSize(void);

private:
	
	class CPropertyGridItemSizeWidth;
	class CPropertyGridItemSizeHeight;

    CSize                        m_szValue;
	CSize*                       m_pBindSize;	// �󶨵��������CSize����
    CPropertyGridItemSizeWidth*  m_itemWidth;
    CPropertyGridItemSizeHeight* m_itemHeight;

public:
	
	// Call this method to change item's value
	//
	void SetSize(
		// The new Size value of the item. 					
		CSize size);

	// Call this method to get the size value of the item.
	// 
	CSize GetSize();

	// ��һ��CSize����󶨵�������
	void BindToSize(CSize* pBindSize)
	{
		m_pBindSize = pBindSize;
		if (m_pBindSize)
			SetSize(*m_pBindSize);
	}

private:

	void SetWidth(CString strWidth);
	void SetHeight(CString strHeight);

	CString SizeToString(CSize size);
	CSize StringToSize(CString str);

	virtual void OnAddChildItem();
	virtual void SetValue(CString strValue);
	
	void UpdateChilds();

	friend class CPropertyGridItemSizeWidth;
	friend class CPropertyGridItemSizeHeight;

};

//////////////////////////////////////////////////////////////////////

AFX_INLINE CSize CPropertyGridItemSize::GetSize() {
	return m_szValue;
}
