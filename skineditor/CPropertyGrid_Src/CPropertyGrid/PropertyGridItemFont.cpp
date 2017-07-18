
#include "StdAfx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemFont.h"
#include "PropertyGrid.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyGridItemFont

CPropertyGridItemFont::CPropertyGridItemFont(CString strCaption, LOGFONT& font)
	: CPropertyGridItem(strCaption)
{
	m_pBindFont = NULL;

	SetFont(font);
	m_nFlags = pgitemHasExpandButton;

}
CPropertyGridItemFont::CPropertyGridItemFont(UINT nID, LOGFONT& font)
	: CPropertyGridItem(nID)
{
	m_pBindFont = NULL;

	SetFont(font);
	m_nFlags = pgitemHasExpandButton;
}

CPropertyGridItemFont::~CPropertyGridItemFont(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// 

void CPropertyGridItemFont::SetFont(LOGFONT& font)
{
	memcpy(&m_lfValue, &font, sizeof(LOGFONT));
	SetValue( FontToString(m_lfValue) );

	if (m_pBindFont)
		memcpy(m_pBindFont, &m_lfValue, sizeof(LOGFONT));
}

CString CPropertyGridItemFont::FontToString(LOGFONT& font)
{
	CWindowDC dc(CWnd::GetDesktopWindow());
	int nHeight = -MulDiv(font.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));

	CString strFont;
	strFont.Format(_T("%s; %ipt"), font.lfFaceName, nHeight);
	return strFont;
}

void CPropertyGridItemFont::OnInplaceButtonDown()
{
	LOGFONT lf = m_lfValue;
	CFontDialog dlg(&lf, CF_EFFECTS | CF_SCREENFONTS | CF_ENABLEHOOK);
	dlg.m_cf.lpfnHook = FontDlgProc;
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCurrentFont(&lf);
		SetFont(lf);
		OnValueChanged(m_strValue);
	}
}

UINT CALLBACK CPropertyGridItemFont::FontDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		// 初始化对话框时，隐藏跟字体颜色相关的两个控件：
		// ID为0x443的静态文本和ID为0x473的组合框
		HWND hWndStatic = ::GetDlgItem(hDlg, 0x443);
		HWND hWndCombo  = ::GetDlgItem(hDlg, 0x473);
		if (hWndStatic)
			::ShowWindow(hWndStatic, SW_HIDE);
		if (hWndCombo)
			::ShowWindow(hWndCombo, SW_HIDE);
	}

	return 0;
}
