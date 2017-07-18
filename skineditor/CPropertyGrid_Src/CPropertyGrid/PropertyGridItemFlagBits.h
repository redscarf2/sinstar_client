#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceCheckList ���ø�ѡ�б��
/////////////////////////////////////////////////////////////////////////////

class CPropertyGridInplaceCheckList : public CCheckListBox
{
public:
	CPropertyGridInplaceCheckList();
	virtual ~CPropertyGridInplaceCheckList();

	// ��������
	void Create(CPropertyGridItem* pItem, CRect rect, BOOL checkStates[]);

	// ɾ������
	void DestroyItem();

protected:
	// ����������
	void Apply();

	// ���ø�ѡ�б���ӵ����
	CPropertyGridItem*	m_pItem;

	// ��ʶ�û��Ƿ�ȡ���˲���
	BOOL	m_bCanceled;

protected:
	//{{AFX_VIRTUAL(CPropertyGridInplaceCheckList)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPropertyGridInplaceCheckList)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemFlagBits ��־λ���Ա���
/////////////////////////////////////////////////////////////////////////////

class CPropertyGridItemFlagBits : public CPropertyGridItem
{
public:
	// ���캯��
	//     strCaption ����������
	CPropertyGridItemFlagBits(CString strCaption);

	// ���캯��
	//     nID ���������Ƶ���ԴID
	CPropertyGridItemFlagBits(UINT nID);

	virtual ~CPropertyGridItemFlagBits();

	// ���ӱ�־λ������
	//     nItemValue ����־λ�������ֵ
	//     nItemText  ����־λ��������ı�
	void AddFlagItem(int nItemValue, CString nItemText);

	// ��������ֵ
	void SetFlagBits(DWORD value);

	// ��ȡ����ֵ
	DWORD GetFlagBits();

	// ��һ�����ͱ����󶨵�������
	void BindToFlagBits(DWORD* pBindFlagBits);

protected:
	// ��ʼ������
	void _Init();

	// ������������ָ���ķָ��������ַ������Ϊ�ַ����б�
	void SplitString(CString str, TCHAR separator, CStringList& splittedStrings);

protected:
	CArray<DWORD, DWORD>	m_lstFlagItemValues;	// ��־λֵ���б�

	DWORD					m_dwValue;				// ����ֵ
	DWORD*					m_pBindFlagBits;		// �󶨵�������ı���

	// ������ֵ�����û�������ı�ʱ�����ô��麯��
	virtual void SetValue(CString strValue);

	// ���û������ťʱ�����ô��麯��
	virtual void OnInplaceButtonDown();

	// ���û�˫��������ʱ�����ô��麯��
	virtual void OnLButtonDblClk();

	CPropertyGridInplaceCheckList	m_wndCheckList;	// ���ø�ѡ�б��
};
