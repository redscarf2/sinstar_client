
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
	// ����ȷ���
	int    degree, minute;
	double second;
	double temp = fabs(fDegree)+(1E-6/3600);// ���ڸ������������������ܸ�ȡ������Ⱥͷ�
	degree = (int)temp;						// �������⡣����10.7��ʾΪ double ����ʱ����ֵ
	temp = (temp-degree) * 60;				// Ϊ10.699...99��ȡ���õ��Ķȵ���10���ֵ���41��
	minute = (int)temp;						// ���ø�ʽ%05.2f����õ�60.00������Ȼ������
	second = (temp-minute) * 60;			// �����һ����С���������ܽ��������⣬�ֲ���
											// Ӱ����ֵ�ľ�ȷ�ȡ�����ӵ��ǰ����֮һ�롣

	// ���ɶȷ����ʽ���ı�
	CString strFormat;		// ��ʽ�ַ���
	CString strMaxSecond;	// ������ֵ(�ı���ʽ)
	if (nSecondPrecision == 0)
	{
		strFormat = _T("%d��%02d'%02.0f\"");
		strMaxSecond = _T("59");
	}
	else
	{
		strFormat.Format( _T("%%d��%%02d'%%0%d.%df\""), 3+nSecondPrecision, nSecondPrecision);
		strMaxSecond = _T("59.") + CString(_T('9'), nSecondPrecision);
	}
#ifdef _UNICODE
	char ansi_buf[256];
	::WideCharToMultiByte(CP_ACP, 0, strMaxSecond, -1, ansi_buf, 256, NULL, NULL);
	double fMaxSecond = atof(ansi_buf);		// ������ֵ(��������ʽ)
#else
	double fMaxSecond = atof(strMaxSecond);	// ������ֵ(��������ʽ)
#endif

	CString strText;
	strText.Format( strFormat,
					degree,
					minute,
					min(fMaxSecond,second) ); // �����������59.999"����ʽ�������60.00"�����
	if (fDegree < 0)
		strText = _T('-') + strText;

	return strText;
}

double CPropertyGridItemDegree::StringToDegree(CString strText)
{
	// ��ȡ����
	int degree = 0, minute = 0;
	float second = 0;
#ifdef _UNICODE
	char ansi_buf[256];
	::WideCharToMultiByte(CP_ACP, 0, strText, -1, ansi_buf, 256, NULL, NULL);
	sscanf(ansi_buf, "%d��%d'%f", &degree, &minute, &second);
#else
	sscanf(strText, "%d��%d'%f", &degree, &minute, &second);
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
