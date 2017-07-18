
#pragma once

class CPropertyGridItemDegree : public CPropertyGridItem  
{
public:
	//  ���캯����
	//      strCaption      ����������
	//      fDegree         ����ʼ����ֵ����λ����
	//      nSecondPrecision�����С�����ֵ�λ������Χ0-3
	CPropertyGridItemDegree(CString strCaption, double fDegree = 0, int nSecondPrecision = 2);

	//  ���캯����
	//      nID             ������ID
	//      fDegree         ����ʼ����ֵ����λ����
	//      nSecondPrecision�����С�����ֵ�λ������Χ0-3
	CPropertyGridItemDegree(UINT nID, double fDegree = 0, int nSecondPrecision = 2);

	virtual ~CPropertyGridItemDegree();

	// ��������ֵ��
	//     fDegree���µ�����ֵ����λ����
	void SetDegree(double fDegree);

	// ��ȡ����ֵ������ֵ��λ����
	double GetDegree();

	// �Ѹ�������ʽ�Ķ�ת���ɶȷ����ʽ���ı���
	//     fDegree          - ��������ʽ�Ķ�
	//     nSecondPrecision - ָʾ���С���������λ����Χ0-3
	// �������ضȷ����ʽ���ı������統�뱣����λС��ʱ��-10.13��
	// ת�����ı�������Ϊ��-10��07'48.00"����
	static CString DegreeToString(double fDegree, int nSecondPrecision = 2);

	// �Ѷȷ����ʽ���ı�ת���ɸ�������ʽ�Ķȣ�
	//     strText  - �ȷ����ʽ���ı�
	// �������ظ�������ʽ�Ķ�
	static double StringToDegree(CString strText);

	// ��һ����������󶨵�������
	void BindToDegree(double* pBindDegree);

	// �������������Сֵ
	void SetMinDegree(double fMinValue);

	// ��������������ֵ
	void SetMaxDegree(double fMaxValue);

protected:
	double	m_fDegree;			// ����ֵ����λ����
	int		m_nSecondPrecision;	// ���С�����ֵ�λ������Χ0-3

	double*	m_pBindDegree;		// �󶨵�������ĸ������

	BOOL	m_bHasMinValue;		// ��ʶ�Ƿ�����Сֵ
	BOOL	m_bHasMaxValue;		// ��ʶ�Ƿ������ֵ
	double	m_fMinValue;		// ���������Сֵ
	double	m_fMaxValue;		// ����������ֵ

	// ������ֵ�����û�������ı�ʱ�����ô˺���
	virtual void SetValue(CString strValue);

	void _Init();
};
