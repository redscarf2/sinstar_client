
#pragma once

//////////////////////////////////////////////////////////////////////
// CPropertyGridItemFont is a CPropertyGridItem derived class.  
// It is used to create Font-value item in a Property Grid control.
//
class CPropertyGridItemFont : public CPropertyGridItem
{
public:
	
	// Constructs a CPropertyGridItemFont object.
	//
	CPropertyGridItemFont(
		// Caption of the item.
		CString strCaption,
		// Initial value
		LOGFONT& font);

	// Constructs a CPropertyGridItemFont object.
	//
	CPropertyGridItemFont(
		// Identifier of the item.
		UINT nID,
		// Initial value
		LOGFONT& font);

	// Destroys a CPropertyGridItemFont object.
	//
	virtual ~CPropertyGridItemFont(void);

private:

	LOGFONT		m_lfValue;
	LOGFONT*	m_pBindFont;	// 绑定到属性项的字体对象

public:

	// Call this method to change item's value
	//
	void SetFont(
		// The new value of the item. 	
		LOGFONT& font);
	
	// Call this method to get the LOGFONT value of the item.
	// 
	void GetFont(
		//Pointer to the LOGFONT structure to receive the font information.
		LOGFONT* lf);

	// 把一个字体对象绑定到属性项
	void BindToFont(LOGFONT* pBindFont)
	{
		m_pBindFont = pBindFont;
		if (m_pBindFont)
			SetFont(*m_pBindFont);
	}

	// 把字体对象转换成类似“宋体; 9pt”这样的字符串
	static CString FontToString(LOGFONT& font);

protected:
	virtual void OnInplaceButtonDown();

	// 字体对话框的回调函数
	static UINT CALLBACK FontDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CPropertyGridItemFont::GetFont(LOGFONT* lf) {
	ASSERT(lf != NULL); memcpy(lf, &m_lfValue, sizeof(LOGFONT));
}
