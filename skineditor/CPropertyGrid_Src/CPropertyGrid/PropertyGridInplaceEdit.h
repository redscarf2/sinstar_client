
#pragma once

// class forwards.
class CPropertyGridView;
class CPropertyGridItem;

//////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceEdit is a CEdit derived class. 
// It is an internal class used by Property Grid control
//
class CPropertyGridInplaceEdit : public CEdit
{
	DECLARE_DYNAMIC(CPropertyGridInplaceEdit)

public:
	CPropertyGridInplaceEdit();
	virtual ~CPropertyGridInplaceEdit();

	virtual void SetValue(CString strValue);
	virtual void HideWindow();
	// �༭�������ַ�����0��ʾ������
	virtual void Create(CPropertyGridItem* pItem, CRect rect, UINT nMaxLength);
	virtual void DestroyItem();

	CPropertyGridItem* GetItem();

private:

    CString              m_strValue;
    CBrush               m_brBack;
    CWnd*                m_pGrid;
    CPropertyGridItem*   m_pItem;

	UINT	m_nDefaultMaxLength;	// �༭������ַ�����ȱʡֵ

protected:

    //{{AFX_VIRTUAL(CPropertyGridInplaceEdit)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

	virtual BOOL SelectConstraint(int nDirection, BOOL bCircle);

    //{{AFX_MSG(CPropertyGridInplaceEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnEnKillfocus();
	afx_msg void OnEnSetfocus();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    //}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	friend class CPropertyGridItem;
};

AFX_INLINE CPropertyGridItem* CPropertyGridInplaceEdit::GetItem() {
	return m_pItem;
}
