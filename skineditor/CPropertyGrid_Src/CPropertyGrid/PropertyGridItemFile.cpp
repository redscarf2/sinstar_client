
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemFile.h"
#include "PropertyGrid.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemFile::CPropertyGridItemFile(CString strCaption, LPCTSTR lpszFileName, BOOL bOpen)
	: CPropertyGridItem(strCaption, lpszFileName)
{
	// ����������ɱ༭���а�ť
	SetFlags(pgitemHasEdit | pgitemHasExpandButton);

	m_bOpen = bOpen;
	m_pBindFileName = NULL;
}

CPropertyGridItemFile::CPropertyGridItemFile(UINT nID, LPCTSTR lpszFileName, BOOL bOpen)
	: CPropertyGridItem(nID, lpszFileName)
{
	// ����������ɱ༭���а�ť
	SetFlags(pgitemHasEdit | pgitemHasExpandButton);

	m_bOpen = bOpen;
	m_pBindFileName = NULL;
}

CPropertyGridItemFile::~CPropertyGridItemFile()
{

}

void CPropertyGridItemFile::SetFileName(CString strFileName)
{
	CPropertyGridItem::SetValue(strFileName);

	if (m_pBindFileName)
		*m_pBindFileName = strFileName;
}

CString CPropertyGridItemFile::GetFileName()
{
	return GetValue();
}

void CPropertyGridItemFile::BindToFileName(CString* pBindFileName)
{
	m_pBindFileName = pBindFileName;
	if (m_pBindFileName)
		SetFileName(*m_pBindFileName);
}

void CPropertyGridItemFile::SetValue(CString strValue)
{
	SetFileName(strValue);
}

void CPropertyGridItemFile::OnInplaceButtonDown()
{
	// ��������ڱ༭״̬ʱ�����ť����������ֵ��û���£�
	// �����ļ��Ի���ĳ�ʼ�ļ���ʹ�õĻ����޸�ǰ������ֵ��
	// ����취�ǰ����뽹�㴫�����Ա�ǿ�Ƹ�������ֵ��
	m_pGrid->SetFocus();

	DWORD dwFlags;
	if (m_bOpen)
		dwFlags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	else
		dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	CFileDialog dlg(m_bOpen, NULL, GetValue(), dwFlags);
	if (dlg.DoModal() == IDOK)
	{
		OnValueChanged( dlg.GetPathName() );
	}
}
