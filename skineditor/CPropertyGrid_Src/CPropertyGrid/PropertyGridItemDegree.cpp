
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemDegree.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemDegree::CPropertyGridItemDegree(CString strCaption, double fDegree, int nSecondPrecision) 
	: CPropertyGridItem(strCaption)
{
	_Init();

	m_nSecondPrecision = nSecondPrecision;
	SetDegree(fDegree);
}

CPropertyGridItemDegree::CPropertyGridItemDegree(UINT nID, double fDegree, int nSecondPrecision) 
	: CPropertyGridItem(nID)
{
	_Init();

	m_nSecondPrecision = nSecondPrecision;
	SetDegree(fDegree);
}

CPropertyGridItemDegree::~CPropertyGridItemDegree()
{

}

void CPropertyGridItemDegree::_Init()
{
	m_pBindDegree	= NULL;

	m_bHasMinValue	= FALSE;
	m_bHasMaxValue	= FALSE;
	m_fMinValue		= 0;
	m_fMaxValue		= 0;
}

void CPropertyGridItemDegree::SetDegree(double fDegree)
{
	m_fDegree = fDegree;

	if (m_bHasMinValue && m_fDegree < m_fMinValue)
		m_fDegree = m_fMinValue;

	if (m_bHasMaxValue && m_fDegree > m_fMaxValue)
		m_fDegree = m_fMaxValue;

	CPropertyGridItem::SetValue( DegreeToString(m_fDegree, m_nSecondPrecision) );

	if (m_pBindDegree)
		*m_pBindDegree = m_fDegree;
}

double CPropertyGridItemDegree::GetDegree()
{
	return m_fDegree;
}

void CPropertyGridItemDegree::SetValue(CString strValue)
{
	SetDegree( StringToDegree(strValue) );
}

CString CPropertyGridItemDegree::DegreeToString(double fDegree, int nSecondPrecision /* = 2 */)
{
	// 计算度分秒
	int    degree, minute;
	double second;
	double temp = fabs(fDegree)+(1E-6/3600);// 由于浮点数存在舍入误差，可能给取整计算度和分
	degree = (int)temp;						// 带来问题。例如10.7表示为 double 类型时，其值
	temp = (temp-degree) * 60;				// 为10.699...99，取整得到的度等于10，分等于41，
	minute = (int)temp;						// 秒用格式%05.2f输出得到60.00。这显然不合理。
	second = (temp-minute) * 60;			// 如果加一个很小的数，既能解决这个问题，又不会
											// 影响数值的精确度。这里加的是百万分之一秒。

	// 生成度分秒格式的文本
	CString strFormat;		// 格式字符串
	CString strMaxSecond;	// 秒的最大值(文本格式)
	if (nSecondPrecision == 0)
	{
		strFormat = _T("%d°%02d'%02.0f\"");
		strMaxSecond = _T("59");
	}
	else
	{
		strFormat.Format( _T("%%d°%%02d'%%0%d.%df\""), 3+nSecondPrecision, nSecondPrecision);
		strMaxSecond = _T("59.") + CString(_T('9'), nSecondPrecision);
	}
#ifdef _UNICODE
	char ansi_buf[256];
	::WideCharToMultiByte(CP_ACP, 0, strMaxSecond, -1, ansi_buf, 256, NULL, NULL);
	double fMaxSecond = atof(ansi_buf);		// 秒的最大值(浮点数格式)
#else
	double fMaxSecond = atof(strMaxSecond);	// 秒的最大值(浮点数格式)
#endif

	CString strText;
	strText.Format( strFormat,
					degree,
					minute,
					min(fMaxSecond,second) ); // 避免出现类似59.999"被格式化输出成60.00"的情况
	if (fDegree < 0)
		strText = _T('-') + strText;

	return strText;
}

double CPropertyGridItemDegree::StringToDegree(CString strText)
{
	// 读取数据
	int degree = 0, minute = 0;
	float second = 0;
#ifdef _UNICODE
	char ansi_buf[256];
	::WideCharToMultiByte(CP_ACP, 0, strText, -1, ansi_buf, 256, NULL, NULL);
	sscanf(ansi_buf, "%d°%d'%f", &degree, &minute, &second);
#else
	sscanf(strText, "%d°%d'%f", &degree, &minute, &second);
#endif
	double fDegree = abs(degree) + minute/60.0 + second/3600.0;
	if (strText.Find( _T('-') ) >= 0)
		fDegree = -fDegree;

	return fDegree;
}

void CPropertyGridItemDegree::BindToDegree(double* pBindDegree)
{
	m_pBindDegree = pBindDegree;
	if (m_pBindDegree)
		SetDegree(*m_pBindDegree);
}

void CPropertyGridItemDegree::SetMinDegree(double fMinValue)
{
	m_bHasMinValue = TRUE;
	m_fMinValue = fMinValue;
	if (m_fDegree < m_fMinValue)
		SetDegree(m_fMinValue);
}

void CPropertyGridItemDegree::SetMaxDegree(double fMaxValue)
{
	m_bHasMaxValue = TRUE;
	m_fMaxValue = fMaxValue;
	if (m_fDegree > m_fMaxValue)
		SetDegree(m_fMaxValue);
}
