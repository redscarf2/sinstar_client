
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
	// 设置属性项可编辑、有按钮
	SetFlags(pgitemHasEdit | pgitemHasExpandButton);

	m_bOpen = bOpen;
	m_pBindFileName = NULL;
}

CPropertyGridItemFile::CPropertyGridItemFile(UINT nID, LPCTSTR lpszFileName, BOOL bOpen)
	: CPropertyGridItem(nID, lpszFileName)
{
	// 设置属性项可编辑、有按钮
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
	// 当属性项处于编辑状态时点击按钮，由于属性值还没更新，
	// 导致文件对话框的初始文件名使用的还是修改前的属性值。
	// 解决办法是把输入焦点传给属性表，强制更新属性值。
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
