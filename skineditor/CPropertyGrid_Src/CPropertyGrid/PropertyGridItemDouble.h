
#pragma once

class CPropertyGridItemDouble : public CPropertyGridItem  
{
public:
	//  构造函数
	//      strCaption－属性名称；
	//      value     －初始属性值
	//      precision －指示小数点后保留几位，缺省为4位，该参数只影响显示。
	CPropertyGridItemDouble(CString strCaption, double value = 0, int precision = 4);

	//  构造函数
	//      nID       －属性ID；
	//      value     －初始属性值
	//      precision －指示小数点后保留几位，缺省为4位，该参数只影响显示。
	CPropertyGridItemDouble(UINT nID, double value = 0, int precision = 4);

	virtual ~CPropertyGridItemDouble();

	// 设置属性值：value－新的属性值
	void SetDouble(double value);

	// 获取属性值
	double GetDouble();

	// 把一个浮点变量绑定到属性项
	void BindToDouble(double* pBindDouble);

	// 设置属性项的最小值
	void SetMinDouble(double fMinValue);

	// 设置属性项的最大值
	void SetMaxDouble(double fMaxValue);

protected:
	double	m_fValue;		// 属性值
	int		m_nPrecision;	// 小数点位数
	double*	m_pBindDouble;	// 绑定到属性项的浮点变量

	BOOL	m_bHasMinValue;	// 标识是否有最小值
	BOOL	m_bHasMaxValue;	// 标识是否有最大值
	double	m_fMinValue;	// 属性项的最小值
	double	m_fMaxValue;	// 属性项的最大值

	// 当属性值由于用户输入而改变时，调用此函数
	virtual void SetValue(CString strValue);

	void _Init();
};
