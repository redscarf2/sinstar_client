#pragma once

#include <commctrl.h>	//HIMAGELIST操作需要
#include "image.h"
#include "simbutton.h"

typedef struct tagINPUTLAYOUT
{
	PIMAGE		pImgBack;		//背景图
	SIZE		size;			//背景图大小
	union{
		int		nLeft;			//左边宽度
		int		nTop;			//上边高度
	};
	union{
		int		nRight;			//右边宽度
		int		nBottom;		//下边高度
	};
	BOOL		bTile;			//平铺模式
	HRGN		hRgnLeft;		//异形模式下窗口的左边的HRGN
	HRGN		hRgnRight;		//异形模式下窗口的右边的HRGN
	PRGNDATA	pRgnDataBody;	//异形模式下窗口的中间部分的HRGN数据,只在中间部分不规则的情况下使用
	DWORD		dwRgnDataLen;
	RECT		rcBody;			//中间区域的矩形
	RECT		rcMargin;		//4个方向的边距
	int			nCandInter;		//重码间隔
	int			nCompHei;		//编码区高度
	int			nLineHei;		//分隔线高度
	int			nCompLeft;		//编码窗口左边附加空白
	int			nCompRight;		//编码窗口右边附加空白
	int			nCandLeft;		//重码窗口左边附加空白
	int			nCandRight;		//重码窗口右边附加空白
}INPUTLAYOUT,*PINPUTLAYOUT;

typedef struct tagSKININPUT
{
	HFONT		hFont;			//显示字体
	WNDBTN		wndBtn;			//input window buttons:prevpage,nextpage
// 	PNGINFO	 *	hBmp32CharMode;	//标点模式显示的PNG图
	HBITMAP		hBmp32CharMode;	//标点模式显示的32位图
	INPUTLAYOUT   modeHerizontal;
	INPUTLAYOUT	  modeVertical;
}SKINIINPUT,*PSKININPUT,FAR *LPSKININPUT;

typedef struct tagCOLORCOMP
{
	COLORREF crNormal;
	COLORREF crInput;
	COLORREF crSentSel;
	COLORREF crSentNormal;
	COLORREF crSentInit;
	COLORREF crEnCode;
	COLORREF crEnMakeup;
	COLORREF crEnPhonetic;
	COLORREF crSpTmHead;	
	COLORREF crSpResult;
	COLORREF crSpSpell;
	COLORREF crSpEdit;
	COLORREF crSpLine;
	COLORREF crUdHead;
	COLORREF crUdCode;
	COLORREF crUdMakeup;
}COLORCOMP;

typedef struct tagCOLORCAND
{
	COLORREF crIndex;
	COLORREF crNormal;
	COLORREF crAssociation;
	COLORREF crCompNormal;
	COLORREF crCompWild;
	COLORREF crEnHead;
	COLORREF crEnPhonetic;
	COLORREF crTip;
	COLORREF crInactive;
	COLORREF crUserDef;
	COLORREF crMixSp;
}COLORCAND;

typedef struct tagSKINSTATUS
{
	char		szSkinName[50];
	PIMAGE		pImgBackShrink;	//背景图关闭状态
	HRGN		hRgnShrink;
	SIZE		sizeShrink;
	PIMAGE		pImgBackFull;	//背景图完整状态
	HRGN		hRgnFull;
	SIZE		sizeFull;
	RECT		rcIcon;			//编码标志区(紧缩)
	RECT		rcIcon2;		//编码标志区(完整)
	RECT		rcText;			//文字区(紧缩)
	RECT		rcText2;		//文字区(完整)
	RECT		rcDrag;			//窗口拖动区(紧缩)
	RECT		rcDrag2;		//窗口拖动区(完整)
	HFONT		hFont;			//显示字体
	HFONT		hFont2;			//竖排文字时使用的字体
	COLORREF	crText;			//文字颜色
	WNDBTN		wndBtn;			//状态栏按钮

	HIMAGELIST  hIconList;		//图标列表:包含形码与拼音２个１６*１６的图标(可以没有，没有时使用默认图标)
}SKINSTATUS,*PSKINSTATUS,FAR *LPSKINSTATUS;

typedef struct tagSKINSPCHAR
{
	HFONT	hFont;
	HFONT	hFontBig;
	HBITMAP hBmpHeader;
	WNDBTN	wndBtn;		//特殊字符窗口按钮
}SKINSPCHAR,FAR *LPSKINSPCHAR;

extern SKINIINPUT	g_SkinInput;
extern SKINSTATUS	g_SkinStatus;
extern SKINSPCHAR	g_SkinSpChar;
extern COLORCOMP	g_ColorComp;		//编码窗口颜色配置
extern COLORCAND	g_ColorCand;		//重码窗口颜色配置
void Skin_GetBtnRect(LPCTSTR pszConfig,BOOL bFullMode);
BOOL Skin_Load(LPCTSTR pszSkinName);
void Skin_Free();
BOOL Skin_LoadSpCharWnd(LPCTSTR pszPath,LPCTSTR pszConfig);
void Skin_FreeSpCharWnd();
int Skin_Enum(char szSkins[][MAX_PATH],int nSize,LPCTSTR pszSkinPath);

