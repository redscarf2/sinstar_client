
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

	// �����������񣺲��ɱ༭���а�ť
	SetFlags(pgitemHasExpandButton);
}

CPropertyGridItemButton::CPropertyGridItemButton(UINT nID, HWND hNotifyWnd, UINT nNotifyMsg) 
	: CPropertyGridItem(nID)
{
	ASSERT(hNotifyWnd);

	m_hNotifyWnd = hNotifyWnd;
	m_nNotifyMsg = nNotifyMsg;

	// �����������񣺲��ɱ༭���а�ť
	SetFlags(pgitemHasExpandButton);
}

CPropertyGridItemButton::~CPropertyGridItemButton()
{

}

void CPropertyGridItemButton::OnInplaceButtonDown()
{
	// ��ָ�����ڷ�����Ϣ
	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, 0, 0);
}

void CPropertyGridItemButton::OnLButtonDblClk()
{
	// ��ָ�����ڷ�����Ϣ
	::SendMessage(m_hNotifyWnd, m_nNotifyMsg, 0, 0);
}
