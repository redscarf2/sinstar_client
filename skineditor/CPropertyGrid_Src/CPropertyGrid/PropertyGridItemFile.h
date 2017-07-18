
#pragma once

class CPropertyGridItemFile : public CPropertyGridItem  
{
public:
	// ���캯��
	//     strCaption   ����������
	//     lpszFileName ����ʼ�ļ���
	//     bOpen        �������Ǵ��ļ�(TRUE)���Ǳ����ļ�(FALSE)
	CPropertyGridItemFile(CString strCaption, LPCTSTR lpszFileName = NULL, BOOL bOpen = TRUE);

	// ���캯��
	//     nID          ������ID
	//     lpszFileName ����ʼ�ļ���
	//     bOpen        �������Ǵ��ļ�(TRUE)���Ǳ����ļ�(FALSE)
	CPropertyGridItemFile(UINT nID, LPCTSTR lpszFileName = NULL, BOOL bOpen = TRUE);

	virtual ~CPropertyGridItemFile();

	// �����ļ���
	void SetFileName(CString strFileName);

	// ��ȡ�ļ���
	CString GetFileName();

	// ��һ���ַ�������󶨵�������
	void BindToFileName(CString* pBindFileName);

protected:
	BOOL		m_bOpen;		// �����Ǵ��ļ�(TRUE)���Ǳ����ļ�(FALSE)
	CString*	m_pBindFileName;// �󶨵���������ַ�������

	// ������ֵ�����û�������ı�ʱ�����ô˺���
	virtual void SetValue(CString strValue);

	// ���û������ťʱ�����ô��麯��
	virtual void OnInplaceButtonDown();
};
