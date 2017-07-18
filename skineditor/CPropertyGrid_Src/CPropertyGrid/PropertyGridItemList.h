
#pragma once

class CPropertyGridItemList : public CPropertyGridItem  
{
public:
	// 构造函数
	//     strCaption －属性名称
	CPropertyGridItemList(CString strCaption);

	// 构造函数
	//     nID －属性ID
	CPropertyGridItemList(UINT nID);

	virtual ~CPropertyGridItemList();

	// 设置列表索引
	void SetListIndex(int value);

	// 获取列表索引
	int GetListIndex();

	// 把一个整型变量绑定到属性项的下拉列表索引
	void BindToListIndex(int* pBindListIndex);

protected:
	int*	m_pBindListIndex;	// 绑定到属性项的整型变量

	// 当属性值由于用户输入而改变时，调用此函数
	virtual void SetValue(CString strValue);
};
