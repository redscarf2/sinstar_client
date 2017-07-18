// SimButton.cpp: implementation of the CSimButton class.
//
//////////////////////////////////////////////////////////////////////
#include "sinstar2.h"
#include "SimButton.h"

void	SimBtn_Enable(SIMBTNINFO *psbi,BOOL bEnable,HWND hWnd)
{
	if(bEnable && psbi->state!=ST_DISABLE) return;
	if(!bEnable && psbi->state==ST_DISABLE) return;
	psbi->state=bEnable?ST_NORMAL:ST_DISABLE;
	if(hWnd) SimBtn_Draw(psbi,hWnd);
}

void	SimBtn_Draw(SIMBTNINFO *psbi,HWND hWnd)
{
	HDC hdc=GetDC(hWnd);
	if(psbi->pImg->bTransparent) SendMessage(hWnd,UM_DRAWBACKGND,(WPARAM)hdc,(LPARAM)&psbi->rc);
	Image_Show(hdc,psbi->rc.left,psbi->rc.top,psbi->rc.right-psbi->rc.left,psbi->rc.bottom-psbi->rc.top,
		psbi->pImg,psbi->state*(psbi->rc.right-psbi->rc.left),0);
	ReleaseDC(hWnd,hdc);
}


void	SimBtn_OnBtnLeave(SIMBTNINFO *psbi,HWND hWnd,POINT pt)
{
	psbi->state=ST_NORMAL;
	SimBtn_Draw(psbi,hWnd);
}

void	SimBtn_OnBtnEnter(SIMBTNINFO *psbi,HWND hWnd,POINT pt)
{
	if(psbi->state==ST_NORMAL) psbi->state=ST_HOVER;
	SimBtn_Draw(psbi,hWnd);
}

BOOL	SimBtn_OnMouseMove(WNDBTN *pwndBtn,HWND hWnd,POINT pt,BOOL bInWnd)
{
	if(pwndBtn->pCapture)
	{
		BOOL bIn=PtInRect(&pwndBtn->pCapture->rc,pt);
		if(bIn)
		{
			if(pwndBtn->pCapture->state!=ST_CHECKED)
			{
				pwndBtn->pCapture->state=ST_CHECKED;
				SimBtn_Draw(pwndBtn->pCapture,hWnd);
			}
		}else
		{
			if(pwndBtn->pCapture->state==ST_CHECKED) SimBtn_OnBtnLeave(pwndBtn->pCapture,hWnd,pt);
		}	
		return TRUE;
	}else
	{
		int i;
		SIMBTNINFO *pFocus=NULL;
		if(bInWnd)
		{
			for(i=0;i<pwndBtn->nCount;i++)
			{
				if(pwndBtn->pBtns[i].bVisible && PtInRect(&pwndBtn->pBtns[i].rc,pt))
				{
					pFocus=pwndBtn->pBtns+i;
					break;
				}
			}
		}
		if(pFocus!=pwndBtn->pFocus)
		{
//			Helper_Trace("\nSimBtn_OnMouseMove:Focus Change");
			if(pwndBtn->pFocus && pwndBtn->pFocus->bVisible)
				SimBtn_OnBtnLeave(pwndBtn->pFocus,hWnd,pt);
			if(pFocus && pFocus->state!=ST_DISABLE)
			{
				SimBtn_OnBtnEnter(pFocus,hWnd,pt);
				pwndBtn->pFocus=pFocus;
			}
			else
				pwndBtn->pFocus=NULL;
		}
		return pFocus!=NULL;
	}
}

BOOL	SimBtn_OnMouseUp(WNDBTN *pwndBtn,HWND hWnd,POINT pt)
{
	BOOL bRet=FALSE;
	RECT rc;
	GetClientRect(hWnd,&rc);
	if(pwndBtn->pCapture && pwndBtn->pCapture->state==ST_CHECKED)
	{
		pwndBtn->pCapture->state=ST_NORMAL;
		PostMessage(hWnd,WM_COMMAND,MAKELONG(pwndBtn->pCapture->cmdID,BN_CLICKED),0);
		bRet=TRUE;
	}
	pwndBtn->pFocus=NULL;
	pwndBtn->pCapture=NULL;
	SimBtn_OnMouseMove(pwndBtn,hWnd,pt,PtInRect(&rc,pt));
	return bRet;
}

BOOL	SimBtn_OnMouseDown(WNDBTN *pwndBtn,HWND hWnd,POINT pt)
{
	if(pwndBtn->pFocus)
	{
		pwndBtn->pFocus->state=ST_CHECKED;
		SimBtn_Draw(pwndBtn->pFocus,hWnd);
		pwndBtn->pCapture=pwndBtn->pFocus;
		return TRUE;
	}
	return FALSE;
}

void	SimBtn_Show(WNDBTN *pwndBtn,HWND hWnd,SIMBTNINFO *psbi,BOOL bVisible,BOOL bUpdate)
{
	psbi->bVisible=bVisible;
	if(pwndBtn->pFocus==psbi)
	{
		psbi->state=ST_NORMAL;
		pwndBtn->pFocus=NULL;
	}
	if(pwndBtn->pCapture==psbi)
	{
		psbi->state=ST_NORMAL;
		pwndBtn->pCapture=NULL;
	}
	if(bUpdate) InvalidateRect(hWnd,&psbi->rc,TRUE);
}

void	SimBtn_OnPaint(WNDBTN *pwndBtn,HDC hdc)
{
	int i;
	for(i=0;i<pwndBtn->nCount;i++)
	{
		if(pwndBtn->pBtns[i].bVisible)
			Image_Show(hdc,pwndBtn->pBtns[i].rc.left,pwndBtn->pBtns[i].rc.top,pwndBtn->pBtns[i].rc.right-pwndBtn->pBtns[i].rc.left,pwndBtn->pBtns[i].rc.bottom-pwndBtn->pBtns[i].rc.top,
			pwndBtn->pBtns[i].pImg,pwndBtn->pBtns[i].state*(pwndBtn->pBtns[i].rc.right-pwndBtn->pBtns[i].rc.left),0);
	}
}

BOOL	SimBtn_Load(SIMBTNINFO *psbi,LPCTSTR pszSection,LPCTSTR pszBmpPath,LPCTSTR pszIniFile,BOOL bRectInfo)
{
	char szPath[MAX_PATH];
	char szName[100];
	IMGTYPE imgType;
	if(!GetPrivateProfileString(pszSection,"bmp","",szName,100,pszIniFile)) return FALSE;
	GetPrivateProfileString(pszSection,"tip","",psbi->szTip,200,pszIniFile);
	psbi->state=ST_NORMAL;
	sprintf(szPath,"%s\\%s",pszBmpPath,szName);
	imgType=GetPrivateProfileInt(pszSection,"imgtype",0,pszIniFile);
	psbi->pImg=Image_Load(szPath,imgType);
	if(psbi->pImg==NULL) psbi->bVisible=FALSE;
	if(bRectInfo)
	{//载入位置数据
		char szRect[100];
		if(GetPrivateProfileString(pszSection,"rect","",szRect,100,pszIniFile))
			sscanf(szRect,"%d,%d,%d,%d",&psbi->rc.left,	&psbi->rc.top,&psbi->rc.right,&psbi->rc.bottom);
	}
	return psbi->pImg!=NULL;
}

