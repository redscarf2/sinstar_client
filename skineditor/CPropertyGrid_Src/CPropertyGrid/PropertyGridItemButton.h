
#pragma once

class CPropertyGridItemButton : public CPropertyGridItem  
{
public:
	// ���캯��������˵����
	//     strCaption  ����������
	//     hNotifyWnd  �������Զ�����Ϣ�Ĵ��ھ��
	//     hNotifyMsg  ���Զ�����Ϣ
	CPropertyGridItemButton(CString strCaption, HWND hNotifyWnd, UINT nNotifyMsg);

	// ���캯��������˵����
	//     nID         ������ID
	//     hNotifyWnd  �������Զ�����Ϣ�Ĵ��ھ��
	//     hNotifyMsg  ���Զ�����Ϣ
	CPropertyGridItemButton(UINT nID, HWND hNotifyWnd, UINT nNotifyMsg);

	virtual ~CPropertyGridItemButton();

protected:
	// ���û������ťʱ�����ô��麯��
	virtual void OnInplaceButtonDown();

	// ���û�˫��������ʱ�����ô��麯��
	virtual void OnLButtonDblClk();

	HWND	m_hNotifyWnd;	// �����Զ�����Ϣ�Ĵ��ھ��
	UINT	m_nNotifyMsg;	// �Զ�����Ϣ
};
