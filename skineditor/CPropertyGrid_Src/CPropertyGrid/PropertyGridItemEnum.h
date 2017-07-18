#pragma once

class CPropertyGridItemEnum : public CPropertyGridItem
{
public:
	// 构造函数
	//     strCaption －属性名称
	CPropertyGridItemEnum(CString strCaption);

	// 构造函数
	//     nID －属性名称的资源ID
	CPropertyGridItemEnum(UINT nID);

	virtual ~CPropertyGridItemEnum();

	// 增加枚举数据项
	//     nItemValue －枚举数据项的值
	//     nItemText  －枚举数据项的文本
	void AddEnumItem(int nItemValue, CString nItemText);

	// 设置属性值
	void SetEnum(int value);

	// 获取属性值
	int GetEnum();

	// 把一个整型变量绑定到属性项
	void BindToEnum(int* pBindEnum);

protected:
	// 初始化函数
	void _Init();

protected:
	CArray<int, int>	m_lstEnumItemValues;	// 枚举值列表
	int*				m_pBindEnum;			// 绑定到属性项的整型变量

	// 当属性值由于用户输入而改变时，调用此函数
	virtual void SetValue(CString strValue);
};
