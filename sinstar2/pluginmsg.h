#ifndef _PLUGINMSG_
#define _PLUGINMSG_

#define UM_PLUGIN	(WM_USER+200)

//插件通过WPARAM指定下面的消息类型与输入法交互
//例如: SendMessage(g_hUiWnd,UM_PLUGIN,PM_GETSTATEWND,0) 获取状态栏窗口句柄

#define PM_GETSTATEWND	1	//从输入法中获取状态栏窗口句柄
#define PM_GETINPUTWND	2	//从输入法中获取输入窗口句柄
#define PM_PLUGINCLOSE	3	//通知输入法关闭当前插件

#endif//_PLUGINMSG_