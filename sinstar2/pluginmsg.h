#ifndef _PLUGINMSG_
#define _PLUGINMSG_

#define UM_PLUGIN	(WM_USER+200)

//���ͨ��WPARAMָ���������Ϣ���������뷨����
//����: SendMessage(g_hUiWnd,UM_PLUGIN,PM_GETSTATEWND,0) ��ȡ״̬�����ھ��

#define PM_GETSTATEWND	1	//�����뷨�л�ȡ״̬�����ھ��
#define PM_GETINPUTWND	2	//�����뷨�л�ȡ���봰�ھ��
#define PM_PLUGINCLOSE	3	//֪ͨ���뷨�رյ�ǰ���

#endif//_PLUGINMSG_