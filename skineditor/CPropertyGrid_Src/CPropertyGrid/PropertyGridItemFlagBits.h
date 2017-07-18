#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridInplaceCheckList 内置复选列表框
/////////////////////////////////////////////////////////////////////////////

class CPropertyGridInplaceCheckList : public CCheckListBox
{
public:
	CPropertyGridInplaceCheckList();
	virtual ~CPropertyGridInplaceCheckList();

	// 创建窗口
	void Create(CPropertyGridItem* pItem, CRect rect, BOOL checkStates[]);

	// 删除窗口
	void DestroyItem();

protected:
	// 更新属性项
	void Apply();

	// 内置复选列表框的拥有者
	CPropertyGridItem*	m_pItem;

	// 标识用户是否取消了操作
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
// CPropertyGridItemFlagBits 标志位属性表项
/////////////////////////////////////////////////////////////////////////////

class CPropertyGridItemFlagBits : public CPropertyGridItem
{
public:
	// 构造函数
	//     strCaption －属性名称
	CPropertyGridItemFlagBits(CString strCaption);

	// 构造函数
	//     nID －属性名称的资源ID
	CPropertyGridItemFlagBits(UINT nID);

	virtual ~CPropertyGridItemFlagBits();

	// 增加标志位数据项
	//     nItemValue －标志位数据项的值
	//     nItemText  －标志位数据项的文本
	void AddFlagItem(int nItemValue, CString nItemText);

	// 设置属性值
	void SetFlagBits(DWORD value);

	// 获取属性值
	DWORD GetFlagBits();

	// 把一个整型变量绑定到属性项
	void BindToFlagBits(DWORD* pBindFlagBits);

protected:
	// 初始化函数
	void _Init();

	// 辅助函数，按指定的分隔符，把字符串拆分为字符串列表
	void SplitString(CString str, TCHAR separator, CStringList& splittedStrings);

protected:
	CArray<DWORD, DWORD>	m_lstFlagItemValues;	// 标志位值的列表

	DWORD					m_dwValue;				// 属性值
	DWORD*					m_pBindFlagBits;		// 绑定到属性项的变量

	// 当属性值由于用户输入而改变时，调用此虚函数
	virtual void SetValue(CString strValue);

	// 当用户点击按钮时，调用此虚函数
	virtual void OnInplaceButtonDown();

	// 当用户双击属性项时，调用此虚函数
	virtual void OnLButtonDblClk();

	CPropertyGridInplaceCheckList	m_wndCheckList;	// 内置复选列表框
};
