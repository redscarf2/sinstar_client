// SimButton.h: interface for the CSimButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMBUTTON_H__0D7F126F_1D14_4E87_BFB7_F7D6D98BDA86__INCLUDED_)
#define AFX_SIMBUTTON_H__0D7F126F_1D14_4E87_BFB7_F7D6D98BDA86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "image.h"
//模拟按钮类
#define UM_DRAWBACKGND		(WM_USER+4000)	//画透明按钮前先通知窗口刷新背景 wParam:hdc,LPARAM:rcDest
enum	{ST_NORMAL=0,ST_HOVER,ST_CHECKED,ST_DISABLE} ;

typedef struct tagSIMBTNINFO{
	UINT	cmdID;	//按钮命令
	int		state;	//按钮状态
	RECT	rc;		//按钮位置
	PIMAGE  pImg;	//按钮位图
	BOOL	bVisible;//显示标志
	char	szTip[200];//提示字符串
}SIMBTNINFO;

typedef struct tagWNDBTN
{
	SIMBTNINFO *pBtns;		//按钮组
	int			nCount;		//按钮个数
	SIMBTNINFO *pFocus;		//当前焦点按钮
	SIMBTNINFO *pCapture;	//当前捕获鼠标输入按钮
}WNDBTN;

void	SimBtn_Draw(SIMBTNINFO *psbi,HWND hWnd);

void	SimBtn_Enable(SIMBTNINFO *psbi,BOOL bEnable,HWND hWnd);

void	SimBtn_OnBtnLeave(SIMBTNINFO *psbi,HWND hWnd,POINT pt);
void	SimBtn_OnBtnEnter(SIMBTNINFO *psbi,HWND hWnd,POINT pt);

BOOL	SimBtn_OnMouseMove(WNDBTN *pwndBtn,HWND hWnd,POINT pt,BOOL bInWnd);
BOOL	SimBtn_OnMouseUp(WNDBTN *pwndBtn,HWND hWnd,POINT pt);
BOOL	SimBtn_OnMouseDown(WNDBTN *pwndBtn,HWND hWnd,POINT pt);
void	SimBtn_Show(WNDBTN *pwndBtn,HWND hWnd,SIMBTNINFO *psbi,BOOL bVisible,BOOL bUpdate);
void	SimBtn_OnPaint(WNDBTN *pwndBtn,HDC hdc);
BOOL	SimBtn_Load(SIMBTNINFO *psbi,LPCTSTR pszSection,LPCTSTR pszBmpPath,LPCTSTR pszIniFile,BOOL bRectInfo);

#endif // !defined(AFX_SIMBUTTON_H__0D7F126F_1D14_4E87_BFB7_F7D6D98BDA86__INCLUDED_)
