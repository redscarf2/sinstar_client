//*******************************************************
//	�����ַ�
//*******************************************************

#ifndef _UISPCHAR_H_
#define _UISPCHAR_H_
#include "simbutton.h"

//���ڳߴ�
#define SPCHAR_WID		290
#define SPCHAR_HEI		150

#define SPDRAG_HEI		19

#define IDC_EXIT		100
#define IDC_INSERT		101
#define IDC_PREV		102
#define IDC_NEXT		103

#define UM_SETLAYOUT	(WM_USER+50)

typedef struct tagLAYOUTINFO{
	char szName[50];
	char szSpChar[200];
	int	 nWords;
}LAYOUTINFO;

//�����������
typedef struct tagSPECIALDATA{
	int	 nFrameWid;			//�߿�
	LAYOUTINFO layout[20];	//ȫ�����̲�����Ϣ
	int	 nLayoutCount;		//ȫ�����̲�������
	int	 nCurLayout;		//��ǰ���̲���
	int	 iCurSel;			//��ǰѡ�����
}SPECIALDATA;


#endif//_UISPCHAR_H_