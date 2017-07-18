
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
	LOGFONT*	m_pBindFont;	// �󶨵���������������

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

	// ��һ���������󶨵�������
	void BindToFont(LOGFONT* pBindFont)
	{
		m_pBindFont = pBindFont;
		if (m_pBindFont)
			SetFont(*m_pBindFont);
	}

	// ���������ת�������ơ�����; 9pt���������ַ���
	static CString FontToString(LOGFONT& font);

protected:
	virtual void OnInplaceButtonDown();

	// ����Ի���Ļص�����
	static UINT CALLBACK FontDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};

//////////////////////////////////////////////////////////////////////

AFX_INLINE void CPropertyGridItemFont::GetFont(LOGFONT* lf) {
	ASSERT(lf != NULL); memcpy(lf, &m_lfValue, sizeof(LOGFONT));
}
