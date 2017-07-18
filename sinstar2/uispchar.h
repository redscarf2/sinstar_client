//*******************************************************
//	特殊字符
//*******************************************************

#ifndef _UISPCHAR_H_
#define _UISPCHAR_H_
#include "simbutton.h"

//窗口尺寸
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

//特殊符号数据
typedef struct tagSPECIALDATA{
	int	 nFrameWid;			//边宽
	LAYOUTINFO layout[20];	//全部键盘布局信息
	int	 nLayoutCount;		//全部键盘布局数量
	int	 nCurLayout;		//当前键盘布局
	int	 iCurSel;			//当前选择的字
}SPECIALDATA;


#endif//_UISPCHAR_H_