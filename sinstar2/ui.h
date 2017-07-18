#pragma once

void PASCAL UIWnd_OpenInput( HWND hUIWnd ,BOOL bShow,BOOL bCalcSize);
void PASCAL UIWnd_OpenStatus( HWND hUIWnd ,BOOL bShow);
void PASCAL UIWnd_UpdateCheckButton();
HWND PASCAL UIWnd_GetInputWnd(  HWND hUIWnd) ;
HWND PASCAL UIWnd_GetStatusWnd( HWND hUIWnd)  ;

#define UM_HIDEWINDOW		(WM_USER+509)
#define UM_SHOWWINDOW		(WM_USER+510)