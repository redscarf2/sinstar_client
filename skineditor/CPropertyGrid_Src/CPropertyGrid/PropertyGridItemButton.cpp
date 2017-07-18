
#include "stdafx.h"

#include "PropertyGridInplaceEdit.h"
#include "PropertyGridInplaceButton.h"
#include "PropertyGridInplaceList.h"
#include "PropertyGridItem.h"
#include "PropertyGridItemButton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemButton::CPropertyGridItemButton(CString strCaption, HWND hNotifyWnd, UINT nNotifyMsg) 
	: CPropertyGridItem(strCaption)
{
	ASSERT(hNotifyWnd);

	m_hNotifyWnd = hNotifyWnd;
	m_nNotifyMsg = nNotifyMsg;

	// 设置属性项风格：不可编辑，有按钮
	SetFlags(pgitemHasExpandButton);
}

CPropertyGridItemButton::CPropertyGridItemButton(UINT nID, HWND hNotifyWnd, UINT nNotifyMsg) 
	: CPropertyGridItem(nID)
{
	ASSERT(hNotifyWnd);

	m_hNotifyWnd = hNotifyWnd;
	m_nNotifyMsg = nNotifyMsg;

	// 设置属性项风格：不可编辑，有按钮
	SetFlags(pgitemHasExpandButton);
}

CPropertyGridItemButton::~CPropertyGridItemButton()
{

}

void CPropertyGridItemButton::OnInplaceButtonDown()
{
	// 向指定窗口发送消息
	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, 0, 0);
}

void CPropertyGridItemButton::OnLButtonDblClk()
{
	// 向指定窗口发送消息
	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, 0, 0);
}
