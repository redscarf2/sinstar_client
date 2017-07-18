
#pragma once

class CPropertyGridItemButton : public CPropertyGridItem  
{
public:
	// 构造函数，参数说明：
	//     strCaption  －属性名称
	//     hNotifyWnd  －接收自定义消息的窗口句柄
	//     hNotifyMsg  －自定义消息
	CPropertyGridItemButton(CString strCaption, HWND hNotifyWnd, UINT nNotifyMsg);

	// 构造函数，参数说明：
	//     nID         －属性ID
	//     hNotifyWnd  －接收自定义消息的窗口句柄
	//     hNotifyMsg  －自定义消息
	CPropertyGridItemButton(UINT nID, HWND hNotifyWnd, UINT nNotifyMsg);

	virtual ~CPropertyGridItemButton();

protected:
	// 当用户点击按钮时，调用此虚函数
	virtual void OnInplaceButtonDown();

	// 当用户双击属性项时，调用此虚函数
	virtual void OnLButtonDblClk();

	HWND	m_hNotifyWnd;	// 接收自定义消息的窗口句柄
	UINT	m_nNotifyMsg;	// 自定义消息
};
