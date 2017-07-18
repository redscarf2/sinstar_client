
#pragma once

class CPropertyGridItemList : public CPropertyGridItem  
{
public:
	// ���캯��
	//     strCaption ����������
	CPropertyGridItemList(CString strCaption);

	// ���캯��
	//     nID ������ID
	CPropertyGridItemList(UINT nID);

	virtual ~CPropertyGridItemList();

	// �����б�����
	void SetListIndex(int value);

	// ��ȡ�б�����
	int GetListIndex();

	// ��һ�����ͱ����󶨵�������������б�����
	void BindToListIndex(int* pBindListIndex);

protected:
	int*	m_pBindListIndex;	// �󶨵�����������ͱ���

	// ������ֵ�����û�������ı�ʱ�����ô˺���
	virtual void SetValue(CString strValue);
};
