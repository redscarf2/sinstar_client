
#pragma once

class CPropertyGridItemDegree : public CPropertyGridItem  
{
public:
	//  构造函数：
	//      strCaption      －属性名称
	//      fDegree         －初始属性值，单位：度
	//      nSecondPrecision－秒的小数部分的位数，范围0-3
	CPropertyGridItemDegree(CString strCaption, double fDegree = 0, int nSecondPrecision = 2);

	//  构造函数：
	//      nID             －属性ID
	//      fDegree         －初始属性值，单位：度
	//      nSecondPrecision－秒的小数部分的位数，范围0-3
	CPropertyGridItemDegree(UINT nID, double fDegree = 0, int nSecondPrecision = 2);

	virtual ~CPropertyGridItemDegree();

	// 设置属性值：
	//     fDegree－新的属性值，单位：度
	void SetDegree(double fDegree);

	// 获取属性值，返回值单位：度
	double GetDegree();

	// 把浮点数形式的度转换成度分秒格式的文本：
	//     fDegree          - 浮点数形式的度
	//     nSecondPrecision - 指示秒的小数点后保留几位，范围0-3
	// 函数返回度分秒格式的文本。例如当秒保留两位小数时，-10.13度
	// 转换成文本后内容为“-10°07'48.00"”。
	static CString DegreeToString(double fDegree, int nSecondPrecision = 2);

	// 把度分秒格式的文本转换成浮点数形式的度：
	//     strText  - 度分秒格式的文本
	// 函数返回浮点数形式的度
	static double StringToDegree(CString strText);

	// 把一个浮点变量绑定到属性项
	void BindToDegree(double* pBindDegree);

	// 设置属性项的最小值
	void SetMinDegree(double fMinValue);

	// 设置属性项的最大值
	void SetMaxDegree(double fMaxValue);

protected:
	double	m_fDegree;			// 属性值，单位：度
	int		m_nSecondPrecision;	// 秒的小数部分的位数，范围0-3

	double*	m_pBindDegree;		// 绑定到属性项的浮点变量

	BOOL	m_bHasMinValue;		// 标识是否有最小值
	BOOL	m_bHasMaxValue;		// 标识是否有最大值
	double	m_fMinValue;		// 属性项的最小值
	double	m_fMaxValue;		// 属性项的最大值

	// 当属性值由于用户输入而改变时，调用此函数
	virtual void SetValue(CString strValue);

	void _Init();
};
