
#pragma once

class CPropertyGridItemDouble : public CPropertyGridItem  
{
public:
	//  ���캯��
	//      strCaption���������ƣ�
	//      value     ����ʼ����ֵ
	//      precision ��ָʾС���������λ��ȱʡΪ4λ���ò���ֻӰ����ʾ��
	CPropertyGridItemDouble(CString strCaption, double value = 0, int precision = 4);

	//  ���캯��
	//      nID       ������ID��
	//      value     ����ʼ����ֵ
	//      precision ��ָʾС���������λ��ȱʡΪ4λ���ò���ֻӰ����ʾ��
	CPropertyGridItemDouble(UINT nID, double value = 0, int precision = 4);

	virtual ~CPropertyGridItemDouble();

	// ��������ֵ��value���µ�����ֵ
	void SetDouble(double value);

	// ��ȡ����ֵ
	double GetDouble();

	// ��һ����������󶨵�������
	void BindToDouble(double* pBindDouble);

	// �������������Сֵ
	void SetMinDouble(double fMinValue);

	// ��������������ֵ
	void SetMaxDouble(double fMaxValue);

protected:
	double	m_fValue;		// ����ֵ
	int		m_nPrecision;	// С����λ��
	double*	m_pBindDouble;	// �󶨵�������ĸ������

	BOOL	m_bHasMinValue;	// ��ʶ�Ƿ�����Сֵ
	BOOL	m_bHasMaxValue;	// ��ʶ�Ƿ������ֵ
	double	m_fMinValue;	// ���������Сֵ
	double	m_fMaxValue;	// ����������ֵ

	// ������ֵ�����û�������ı�ʱ�����ô˺���
	virtual void SetValue(CString strValue);

	void _Init();
};
