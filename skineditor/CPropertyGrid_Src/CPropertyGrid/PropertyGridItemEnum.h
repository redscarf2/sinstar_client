#pragma once

class CPropertyGridItemEnum : public CPropertyGridItem
{
public:
	// ���캯��
	//     strCaption ����������
	CPropertyGridItemEnum(CString strCaption);

	// ���캯��
	//     nID ���������Ƶ���ԴID
	CPropertyGridItemEnum(UINT nID);

	virtual ~CPropertyGridItemEnum();

	// ����ö��������
	//     nItemValue ��ö���������ֵ
	//     nItemText  ��ö����������ı�
	void AddEnumItem(int nItemValue, CString nItemText);

	// ��������ֵ
	void SetEnum(int value);

	// ��ȡ����ֵ
	int GetEnum();

	// ��һ�����ͱ����󶨵�������
	void BindToEnum(int* pBindEnum);

protected:
	// ��ʼ������
	void _Init();

protected:
	CArray<int, int>	m_lstEnumItemValues;	// ö��ֵ�б�
	int*				m_pBindEnum;			// �󶨵�����������ͱ���

	// ������ֵ�����û�������ı�ʱ�����ô˺���
	virtual void SetValue(CString strValue);
};
