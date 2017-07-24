//除uiWnd外所有窗口处理
//提示信息显示在编码窗口中
//输入法创建的所有窗口（不含对话框）都应该是WS_DISABLE的，也就是说不能响应鼠标消息
//为了响应鼠标消息需要在WM_SETCURSOR中获取鼠标按钮状态来模拟
#include "sinstar2.h"
#include "uispchar.h"
#include "ui.h"
#include "udictinput.h"
#include <shellapi.h>
#include <math.h>
#include <helper.h>

//编码窗口显示隐藏动画使用的定时器
#define TIMER_FADE		900
//状态栏窗口模式切换动画使用的定时器
#define TIMER_ANIMATOR	900
#define	ANIMATE_STEPS	10

#define STATUSCMD_ICON		1
#define STATUSCMD_TEXT		2
#define STATUSCMD_DRAG		3

#define STATUSCMD_NODEF		-1

#define MAX_UDICTKEY	40	//最大关键字长度
#define MAX_UDICTPHRASE	1000


#define MAX_CANDWID			200	//应该可以显示"中华人民共和国"这个串
#define MAX_SENTHEAD		150	//句子联想时上次输入的字符显示长度

static char  s_szSkins[ID_SKIN_END-ID_SKIN_BEGIN+1][MAX_PATH]={0};
static int   s_nSkins=0;
static char  s_szBatch[20][MAX_PATH]={0},s_szUDict[20][MAX_PATH]={0},s_szComp[20][MAX_PATH]={0};
static BOOL  s_bInitBatch=FALSE,s_bInitDict=FALSE,s_bInitComp=FALSE,s_bInitSpChar=FALSE;
static BOOL  s_bInputVertical=FALSE;

char g_szLine[][2]={"-","|","/","\\","<","*"};

int  RectWidth(RECT *pRect) {return pRect->right-pRect->left;}
int  RectHeight(RECT *pRect) {return pRect->bottom-pRect->top;}

LRESULT WINAPI CompWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	if(message==WM_IME_NOTIFY && wParam == IMN_SETCOMPOSITIONWINDOW) 
	{
		HWND hUIWnd = GetWindow(hWnd, GW_OWNER);
		if(IsWindow(hUIWnd)) SendMessage(hUIWnd,message,wParam,lParam);
	}

	return 0;
}

LRESULT WINAPI CandWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	return 0;
}

void VSentTextout(HDC hdc,LPCONTEXTPRIV lpCntxtPriv,RECT *lprc,int nLineInter,COLORREF crSel,COLORREF crLeft)
{
	POINT ptOffset={lprc->left,lprc->top};
	int i=0;
	SIZE size;
	int nLineHei;
	GetTextExtentPoint(hdc,"A",1,&size);
	nLineHei=size.cy;
	SetTextColor(hdc,crSel);
	while(i<lpCntxtPriv->sSentCaret)
	{
		GetTextExtentPoint(hdc,lpCntxtPriv->pbySentWord[i],lpCntxtPriv->pbySentWord[i+1]-lpCntxtPriv->pbySentWord[i],&size);
		if(ptOffset.x+size.cx>lprc->right)
		{
			ptOffset.x=lprc->left;
			ptOffset.y+=nLineHei+nLineInter;
		}
		TextOut(hdc,ptOffset.x,ptOffset.y,lpCntxtPriv->pbySentWord[i],lpCntxtPriv->pbySentWord[i+1]-lpCntxtPriv->pbySentWord[i]);
		ptOffset.x+=size.cx;
		i++;
	}
	SetTextColor(hdc,crLeft);
	while(i<lpCntxtPriv->sSentWords)
	{
		GetTextExtentPoint(hdc,lpCntxtPriv->pbySentWord[i],lpCntxtPriv->pbySentWord[i+1]-lpCntxtPriv->pbySentWord[i],&size);
		if(ptOffset.x+size.cx>lprc->right)
		{
			ptOffset.x=lprc->left;
			ptOffset.y+=nLineHei+nLineInter;
		}
		TextOut(hdc,ptOffset.x,ptOffset.y,lpCntxtPriv->pbySentWord[i],lpCntxtPriv->pbySentWord[i+1]-lpCntxtPriv->pbySentWord[i]);
		ptOffset.x+=size.cx;
		i++;
	}
}

int VSentTextoutPoint(HDC hdc,LPCONTEXTPRIV lpCntxtPriv,int nMaxWid,int nLineInter)
{
	POINT ptOffset={0};
	int i=0;
	SIZE size;
	int nLineHei;
	GetTextExtentPoint(hdc,"A",1,&size);
	nLineHei=size.cy;
	while(i<lpCntxtPriv->sSentCaret)
	{
		GetTextExtentPoint(hdc,lpCntxtPriv->pbySentWord[i],lpCntxtPriv->pbySentWord[i+1]-lpCntxtPriv->pbySentWord[i],&size);
		if(ptOffset.x+size.cx>nMaxWid)
		{
			ptOffset.x=0;
			ptOffset.y+=nLineHei+nLineInter;
		}
		ptOffset.x+=size.cx;
		i++;
	}
	return ptOffset.y+nLineHei;
}

void StatusWnd_UpdateTransDegree(HWND hWndStatus)
{//透明度发生更改，更新输入窗口
	if(Helper_IsLayerSupport())
	{
		HWND hWndOwner=GetWindow(hWndStatus,GW_OWNER);
		HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hWndOwner,IMMGWLP_PRIVATE);
		LPUIPRIV lpUIPriv = (LPUIPRIV)GlobalLock(hUIPriv);
		Helper_SetWindowLayerAttributes(lpUIPriv->hInputWnd,(BYTE)ceil(2.55*g_SettingsG.byTransInput));
		GlobalUnlock(hUIPriv);
		Helper_SetWindowLayerAttributes(hWndStatus,(BYTE)ceil(2.55*g_SettingsG.byTransStatus));
	}
}

void StatusWnd_UpdateInputPageCands(HWND hWndStatus)
{//重码数发生更改，更新输入窗口。
	HWND hWndOwner=GetWindow(hWndStatus,GW_OWNER);
	HIMC hIMC=(HIMC)GetWindowLongPtr(hWndOwner,IMMGWLP_IMC);
	if(hIMC)
	{
		HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hWndOwner,IMMGWLP_PRIVATE);
		LPUIPRIV lpUIPriv = (LPUIPRIV)GlobalLock(hUIPriv);
		LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
		LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
		lpCntxtPriv->sCurCandPage=0;
		ImmUnlockIMCC(lpIMC->hPrivate);
		ImmUnlockIMC(hIMC);
		if(lpUIPriv->hInputWnd) SendMessage(lpUIPriv->hInputWnd,UM_CALCSIZE,0,0);
		GlobalUnlock(hUIPriv);
	}
}

void ContextMenu(HWND hWnd,HWND hWndMsg,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)(LOWORD(lParam)),(short)(HIWORD(lParam))};
	HMENU hMenu=LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_SYSMENU));
	HMENU hSubMenu=GetSubMenu(hMenu,0);
	s_bInitBatch=FALSE,s_bInitDict=FALSE,s_bInitComp=FALSE,s_bInitSpChar=FALSE;
	s_nSkins=0;
	SetCursor(LoadCursor(NULL,IDC_ARROW));
	ClientToScreen(hWnd,&pt);

	CheckMenuItem(hSubMenu,IDM_HIDESTATUS,(g_SettingsL.bHideStatus?MF_CHECKED:0)|MF_BYCOMMAND);
	CheckMenuItem(hSubMenu,IDM_MOUSEFOLLOW,(g_SettingsL.bMouseFollow?MF_CHECKED:0)|MF_BYCOMMAND);
	CheckMenuItem(hSubMenu,IDM_INPUTBIG5,(g_SettingsL.bInputBig5?MF_CHECKED:0)|MF_BYCOMMAND);
	TrackPopupMenu(hSubMenu,TPM_RIGHTALIGN|TPM_BOTTOMALIGN,pt.x,pt.y,0,hWndMsg,NULL);
	DestroyMenu(hMenu);
}

//更新输入窗口的指定部分背景
void InputWnd_PaintBackgnd(HWND hWnd,HDC hdc,LPRECT pRect)
{
	INPUTLAYOUT *pLayout=NULL;
	RECT rcWnd,rc1,rc2,rc3;
	GetClientRect(hWnd,&rcWnd);
	rc1=rc2=rc3=rcWnd;
	pLayout=s_bInputVertical?(&g_SkinInput.modeVertical):(&g_SkinInput.modeHerizontal);

	if(s_bInputVertical)
	{
		rc1.bottom=rc2.top=pLayout->nTop;
		rc2.bottom=rc3.top=rcWnd.bottom-pLayout->nBottom;
	}else
	{
		rc1.right=rc2.left=pLayout->nLeft;
		rc2.right=rc3.left=rcWnd.right-pLayout->nRight;
	}
	if(pRect)
	{
		IntersectRect(&rc1,&rc1,pRect);
		IntersectRect(&rc2,&rc2,pRect);
		IntersectRect(&rc3,&rc3,pRect);
	}

	if(g_hRgnInput)
	{//判断是否是异形窗口皮肤,是异形窗口时根据窗口的区域进行剪裁
		HRGN hRgnTmp=0;
		RECT rcBound;
		if(!IsRectEmpty(&rc1))
		{
			hRgnTmp=CreateRectRgnIndirect(&rc1);
			CombineRgn(hRgnTmp,g_hRgnInput,hRgnTmp,RGN_AND);
			GetRgnBox(hRgnTmp,&rcBound);
			IntersectRect(&rc1,&rc1,&rcBound);
			DeleteObject(hRgnTmp);
		}
		if(!IsRectEmpty(&rc3))
		{
			hRgnTmp=CreateRectRgnIndirect(&rc3);
			CombineRgn(hRgnTmp,g_hRgnInput,hRgnTmp,RGN_AND);
			GetRgnBox(hRgnTmp,&rcBound);
			IntersectRect(&rc3,&rc3,&rcBound);
			DeleteObject(hRgnTmp);
		}
		if(!IsRectEmpty(&rc2))
		{
			hRgnTmp=CreateRectRgnIndirect(&rc2);
			CombineRgn(hRgnTmp,g_hRgnInput,hRgnTmp,RGN_AND);
			GetRgnBox(hRgnTmp,&rcBound);
			IntersectRect(&rc2,&rc2,&rcBound);
			DeleteObject(hRgnTmp);
		}
	}
	if(s_bInputVertical)
	{
		if(!IsRectEmpty(&rc1))
		{//上边部分
			Image_Show(hdc,rc1.left,rc1.top,RectWidth(&rc1),RectHeight(&rc1),
				pLayout->pImgBack,
				rc1.left,rc1.top);
		}
		if(!IsRectEmpty(&rc2))
		{//中间部分
			if(pLayout->bTile)
			{//在y方向上采用平铺
				int nInterS=pLayout->size.cy-pLayout->nTop-pLayout->nBottom;
				int y=rc2.top;
				while(y<rc2.bottom)
				{
					int nHei=nInterS;
					if(nHei>rc2.bottom-y) nHei=rc2.bottom-y;
					Image_Show(hdc,rc2.left,y,RectWidth(&rc2),nHei,pLayout->pImgBack,rc2.left,pLayout->nTop);
					y+=nHei;
				}
			}else
			{//在y方向上拉伸
				RECT rcSour;
				HDC hmemdc=CreateCompatibleDC(hdc);
				HGDIOBJ hOldBmp=SelectObject(hmemdc,pLayout->pImgBack->hBmp);
				SetStretchBltMode(hdc,COLORONCOLOR);
				rcSour.left=rc2.left,rcSour.right=rc2.right;
				rcSour.top=pLayout->nTop+(rc2.top-pLayout->nTop)*(pLayout->size.cy-pLayout->nTop-pLayout->nBottom)/(rcWnd.bottom-pLayout->nTop-pLayout->nBottom);
				rcSour.bottom=pLayout->nTop+(rc2.bottom-pLayout->nTop)*(pLayout->size.cy-pLayout->nTop-pLayout->nBottom)/(rcWnd.bottom-pLayout->nTop-pLayout->nBottom);

				StretchBlt(hdc,
					rc2.left,rc2.top,RectWidth(&rc2),RectHeight(&rc2),
					hmemdc,
					rcSour.left,rcSour.top,RectWidth(&rcSour),RectHeight(&rcSour),
					SRCCOPY);
				SelectObject(hmemdc,hOldBmp);
				DeleteDC(hmemdc);
			}
		}
		if(!IsRectEmpty(&rc3))
		{//下边部分,注意与图片大小对齐
			Image_Show(hdc,rc3.left,rc3.top,RectWidth(&rc3),RectHeight(&rc3),
				pLayout->pImgBack,
				rc3.left,rc3.top+pLayout->size.cy-rcWnd.bottom);
		}
	}else
	{
		if(!IsRectEmpty(&rc1))
		{//左边部分
			Image_Show(hdc,rc1.left,rc1.top,rc1.right-rc1.left,rc1.bottom-rc1.top,
				pLayout->pImgBack,
				rc1.left,rc1.top);
		}
		if(!IsRectEmpty(&rc2))
		{//中间部分
			if(pLayout->bTile)
			{//在x方向上采用平铺
				int nInterS=pLayout->size.cx-pLayout->nLeft-pLayout->nRight;
				int x=rc2.left;
				while(x<rc2.right)
				{
					int nWid=nInterS;
					if(nWid>rc2.right-x) nWid=rc2.right-x;
					Image_Show(hdc,x,rc2.top,nWid,rc2.bottom-rc2.top,pLayout->pImgBack,pLayout->nLeft,rc2.top);
					x+=nWid;
				}
			}else
			{//在x方向上拉伸
				RECT rcSour;
				HDC hmemdc=CreateCompatibleDC(hdc);
				HGDIOBJ hOldBmp=SelectObject(hmemdc,pLayout->pImgBack->hBmp);
				SetStretchBltMode(hdc,COLORONCOLOR);
				rcSour.left=pLayout->nLeft+(rc2.left-pLayout->nLeft)*(pLayout->size.cx-pLayout->nLeft-pLayout->nRight)/(rcWnd.right-pLayout->nLeft-pLayout->nRight);
				rcSour.right=pLayout->nLeft+(rc2.right-pLayout->nLeft)*(pLayout->size.cx-pLayout->nLeft-pLayout->nRight)/(rcWnd.right-pLayout->nLeft-pLayout->nRight);
				rcSour.top=rc2.top,rcSour.bottom=rc2.bottom;
				StretchBlt(hdc,
					rc2.left,rc2.top,rc2.right-rc2.left,rc2.bottom-rc2.top,
					hmemdc,
					rcSour.left,rcSour.top,RectWidth(&rcSour),RectHeight(&rcSour),
					SRCCOPY);
				SelectObject(hmemdc,hOldBmp);
				DeleteDC(hmemdc);
			}
		}
		if(!IsRectEmpty(&rc3))
		{//右边部分,注意与图片大小对齐
			Image_Show(hdc,rc3.left,rc3.top,rc3.right-rc3.left,rc3.bottom-rc3.top,
				pLayout->pImgBack,
				rc3.left+pLayout->size.cx-rcWnd.right,rc3.top);
		}
	}
}

//区域拉伸
HRGN StretchRgn(PRGNDATA pRgnData, DWORD dwDataLen,float HScaling, float VScaling)
{
    HRGN rgn = NULL;
	XFORM xfrom = {HScaling, 0, 0, VScaling, 0, 0};
	LPRECT prc=(LPRECT)pRgnData->Buffer;
    rgn = ExtCreateRegion(&xfrom, dwDataLen, pRgnData);
    return rgn;
}

void InputWnd_SetRgn(HWND hWnd,BOOL bVertical)
{
	INPUTLAYOUT *pLayout=NULL;
	RECT rcWnd;
	GetClientRect(hWnd,&rcWnd);
	pLayout=bVertical?(&g_SkinInput.modeVertical):(&g_SkinInput.modeHerizontal);
	if(pLayout->hRgnLeft || pLayout->hRgnRight)
	{
		HRGN hRgnTemp=CreateRectRgn(0,0,0,0);
		if(!g_hRgnInput) g_hRgnInput=CreateRectRgn(0,0,0,0);
		if(pLayout->hRgnLeft) CombineRgn(g_hRgnInput,pLayout->hRgnLeft,0,RGN_COPY);
		if(pLayout->hRgnRight)
		{
			CombineRgn(hRgnTemp,pLayout->hRgnRight,0,RGN_COPY);
			if(bVertical)
				OffsetRgn(hRgnTemp,0,rcWnd.bottom-pLayout->size.cy);
			else
				OffsetRgn(hRgnTemp,rcWnd.right-pLayout->size.cx,0);
			CombineRgn(g_hRgnInput,g_hRgnInput,hRgnTemp,RGN_OR);
		}
		if(bVertical)
		{
			if(pLayout->pRgnDataBody)
			{
				if(pLayout->bTile)
				{//区域平铺
					int nOffset=pLayout->nTop;
					int nInter=pLayout->size.cy-pLayout->nTop-pLayout->nBottom;
					HRGN hRgnBody=ExtCreateRegion(NULL,pLayout->dwRgnDataLen,pLayout->pRgnDataBody);
					OffsetRgn(hRgnBody,0,pLayout->nTop);
					CombineRgn(hRgnTemp,hRgnBody,0,RGN_COPY);
					OffsetRgn(hRgnBody,0,nInter);
					nOffset+=nInter;
					while(nOffset<rcWnd.bottom-pLayout->nBottom)
					{
						CombineRgn(hRgnTemp,hRgnTemp,hRgnBody,RGN_OR);
						OffsetRgn(hRgnBody,0,nInter);
						nOffset+=nInter;
					}
					SetRectRgn(hRgnBody,0,pLayout->nTop,rcWnd.right,rcWnd.bottom-pLayout->nBottom);
					CombineRgn(hRgnTemp,hRgnTemp,hRgnBody,RGN_AND);
					DeleteObject(hRgnBody);
				}else
				{//区域拉伸
					hRgnTemp=StretchRgn(pLayout->pRgnDataBody,pLayout->dwRgnDataLen,
						1.0f,
						(float)(rcWnd.bottom-pLayout->nTop-pLayout->nBottom)/(pLayout->size.cy-pLayout->nTop-pLayout->nBottom));
					OffsetRgn(hRgnTemp,0,pLayout->nTop);
				}
			}else
				SetRectRgn(hRgnTemp,pLayout->rcBody.left,pLayout->nTop,pLayout->rcBody.right,rcWnd.bottom-pLayout->nBottom);
		}else
		{
			if(pLayout->pRgnDataBody)
			{
				if(pLayout->bTile)
				{//区域平铺
					int nOffset=pLayout->nLeft;
					int nInter=pLayout->size.cx-pLayout->nLeft-pLayout->nRight;
					HRGN hRgnBody=ExtCreateRegion(NULL,pLayout->dwRgnDataLen,pLayout->pRgnDataBody);
					OffsetRgn(hRgnBody,pLayout->nLeft,0);
					CombineRgn(hRgnTemp,hRgnBody,0,RGN_COPY);
					OffsetRgn(hRgnBody,nInter,0);
					nOffset+=nInter;
					while(nOffset<rcWnd.right-pLayout->nRight)
					{
						CombineRgn(hRgnTemp,hRgnTemp,hRgnBody,RGN_OR);
						OffsetRgn(hRgnBody,nInter,0);
						nOffset+=nInter;
					}
					SetRectRgn(hRgnBody,pLayout->nLeft,0,rcWnd.right-pLayout->nRight,rcWnd.bottom);
					CombineRgn(hRgnTemp,hRgnTemp,hRgnBody,RGN_AND);
					DeleteObject(hRgnBody);
				}else
				{//区域拉伸
					hRgnTemp=StretchRgn(pLayout->pRgnDataBody,pLayout->dwRgnDataLen,
						(float)(rcWnd.right-pLayout->nLeft-pLayout->nRight)/(pLayout->size.cx-pLayout->nLeft-pLayout->nRight),
						1.0f);
					OffsetRgn(hRgnTemp,pLayout->nLeft,0);
				}
			}else
				SetRectRgn(hRgnTemp,pLayout->nLeft,pLayout->rcBody.top,rcWnd.right-pLayout->nRight,pLayout->rcBody.bottom);
		}
		CombineRgn(g_hRgnInput,g_hRgnInput,hRgnTemp,RGN_OR);
		DeleteObject(hRgnTemp);
		Helper_SetWindowRgn(hWnd,g_hRgnInput,FALSE);
	}else 
	{
		if(g_hRgnInput)
		{
			DeleteObject(g_hRgnInput);
			g_hRgnInput=0;
		}
		SetWindowRgn(hWnd,0,FALSE);
	}
}

//************************************
// Method:    InputWnd_CalcCandHorz
// FullName:  InputWnd_CalcCandHorz
// Access:    public 
// Returns:   int
// Qualifier: 计算横排重码窗口需要的宽度
// Parameter: HWND hWnd
// Parameter: HDC hdc
// Parameter: LPCONTEXTPRIV lpCntxtPriv
//************************************
int InputWnd_CalcCandHorz(HWND hWnd,HDC hdc,LPCONTEXTPRIV lpCntxtPriv)
{
	INPUTLAYOUT *pLayout=&g_SkinInput.modeHerizontal;
	int nOffset=pLayout->rcMargin.left+pLayout->nCandLeft;
	int nTop=0;
	int nCeiling=pLayout->rcMargin.top+pLayout->nCompHei+pLayout->nLineHei;
	int nFloor=pLayout->size.cy-pLayout->rcMargin.bottom;
	int nHeight=nFloor-nCeiling;
	SIMBTNINFO *pBtn=NULL;
	SIZE size;
	GetTextExtentPoint(hdc,"A",1,&size);
	nTop=nCeiling+(nHeight-size.cy)/2;

	pBtn=g_SkinInput.wndBtn.pBtns+IDC_PREVPAGE-IDC_BTN_BASE;
	pBtn->bVisible=FALSE;
	pBtn=g_SkinInput.wndBtn.pBtns+IDC_NEXTPAGE-IDC_BTN_BASE;
	pBtn->bVisible=FALSE;

	if(lpCntxtPriv->sCandCount)
	{//计算重码需要长度
		LPBYTE pbyCand;
		char szBuf[100];
		int i,iCand=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands;
		int nCands=min(g_SettingsG.cPageCands,lpCntxtPriv->sCandCount-iCand);
		LPRECT pRcCand=NULL;

		if(lpCntxtPriv->sCurCandPage>0)
		{//向前翻页按钮
			pBtn=g_SkinInput.wndBtn.pBtns+IDC_PREVPAGE-IDC_BTN_BASE;
			pBtn->bVisible=TRUE;
			OffsetRect(&pBtn->rc,nOffset-pBtn->rc.left,nCeiling+(nFloor-nCeiling-RectHeight(&pBtn->rc))/2-pBtn->rc.top);
			nOffset+=RectWidth(&pBtn->rc);
		}
		
		if(lpCntxtPriv->inState==INST_CODING &&
			lpCntxtPriv->sbState!=SBST_NORMAL && 
			g_SettingsG.byAstMode==AST_ENGLISH && 
			lpCntxtPriv->sCandCount)
		{//计算英文联想的词头宽度
			lpCntxtPriv->rcHeader.left=nOffset;
			lpCntxtPriv->rcHeader.top=nTop;
			GetTextExtentPoint(hdc,(LPSTR)lpCntxtPriv->pbyEnAstPhrase+1,lpCntxtPriv->pbyEnAstPhrase[0],&size);
			nOffset+=size.cx;
			GetTextExtentPoint(hdc,":",1,&size);
			nOffset+=size.cx;
			lpCntxtPriv->rcHeader.right=nOffset;
			lpCntxtPriv->rcHeader.bottom=nTop+size.cy;
			nOffset+=g_SkinInput.modeHerizontal.nCandInter;
		}

		for(i=0,pRcCand=lpCntxtPriv->rcCand; i<nCands; i++,iCand++,pRcCand++)
		{
			pRcCand->left=nOffset;
			pRcCand->top=nTop;
			pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
			szBuf[0]='0'+(1+i)%10;
			szBuf[1]='.';
			GetTextExtentPoint(hdc,szBuf,2,&size);
			nOffset+=size.cx;
			if(lpCntxtPriv->inState==INST_CODING)
			{
				if(lpCntxtPriv->sbState==SBST_NORMAL)
				{//编码状态
					if(g_CompMode==IM_SPELL)
					{//拼音
						Helper_GetTextExtent(hdc,(LPCSTR)pbyCand+2,pbyCand[1],&size,MAX_CANDWID);
						nOffset+=size.cx;
					}else
					{//形码	：rate+len+phrase+len+comp
						BYTE byRate=*pbyCand++;
						Helper_GetTextExtent(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
						nOffset+=size.cx;
						pbyCand+=pbyCand[0]+1;
						if(pbyCand[0])
						{//有编码信息
							if(byRate==RATE_WILD)
							{//万能键,显示全编码
								GetTextExtentPoint(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size);
								nOffset+=size.cx;
							}else if(byRate==RATE_ASSOCIATE)
							{//自动联想,显示联想到的编码
								GetTextExtentPoint(hdc,(LPSTR)pbyCand+1+lpCntxtPriv->cCompLen,pbyCand[0]-lpCntxtPriv->cCompLen,&size);
								nOffset+=size.cx;
							}
						}
					}
				}else 
				{
					if(g_SettingsG.byAstMode==AST_CAND)
					{//词组联想:level+rate+len+phrase
						Helper_GetTextExtent(hdc,(LPSTR)pbyCand+3+pbyCand[0],pbyCand[2]-pbyCand[0],&size,MAX_CANDWID);
						nOffset+=size.cx;
					}else if(g_SettingsG.byAstMode==AST_ENGLISH)
					{//英文联想:len+spell+len+phonetic
						HGDIOBJ hOldFont=0;
						Helper_GetTextExtent(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
						nOffset+=size.cx;
						pbyCand+=1+pbyCand[0];
						if(pbyCand[0])
						{
							GetTextExtentPoint(hdc,"[",1,&size);
							nOffset+=size.cx;
							if(g_hPhoneticFont) hOldFont=SelectObject(hdc,g_hPhoneticFont);
							Helper_GetTextExtent(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
							nOffset+=size.cx;
							if(g_hPhoneticFont) SelectObject(hdc,hOldFont);
							GetTextExtentPoint(hdc,"]",1,&size);
							nOffset+=size.cx;
						}
					}			
				}
			}else if(lpCntxtPriv->inState==INST_ENGLISH)
			{//英文状态:len+phrase
				Helper_GetTextExtent(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
				nOffset+=size.cx;
			}else if(lpCntxtPriv->inState==INST_USERDEF)
			{//用户自定义状态:len+phrase+complen+comp
				Helper_GetTextExtent(hdc,(LPSTR)pbyCand+2,pbyCand[1],&size,MAX_CANDWID);
				nOffset+=size.cx;
			}else if(lpCntxtPriv->inState==INST_LINEIME)
			{//笔画输入状态:rate+len+phrase
				Helper_GetTextExtent(hdc,(LPSTR)pbyCand+2,pbyCand[1],&size,MAX_CANDWID);
				nOffset+=size.cx;
			}
			pRcCand->right=nOffset;
			pRcCand->bottom=nTop+size.cy;

			nOffset+=g_SkinInput.modeHerizontal.nCandInter;
		}
		nOffset-=g_SkinInput.modeHerizontal.nCandInter;
		
		if(lpCntxtPriv->sCurCandPage<(lpCntxtPriv->sCandCount+g_SettingsG.cPageCands-1)/g_SettingsG.cPageCands-1)
		{//下翻页按钮
			pBtn=g_SkinInput.wndBtn.pBtns+IDC_NEXTPAGE-IDC_BTN_BASE;
			pBtn->bVisible=TRUE;
			OffsetRect(&pBtn->rc,nOffset-pBtn->rc.left,nCeiling+(nFloor-nCeiling-RectHeight(&pBtn->rc))/2-pBtn->rc.top);
			nOffset+=RectWidth(&pBtn->rc);
		}
	}else if(g_bTipShow)
	{
		if(lpCntxtPriv->sbState!=SBST_NORMAL
			|| lpCntxtPriv->bWebMode 
			|| (g_bTempSpell&&lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0))
		{//非编码输入过程中没有重码时在重码栏显示提示信息
			lpCntxtPriv->rcTip.left=nOffset;
			lpCntxtPriv->rcTip.top=nTop;
			nOffset+=GetTextExtentPoint(hdc,g_szTip,strlen(g_szTip),&size);
			nOffset+=size.cx;
			lpCntxtPriv->rcTip.right=nOffset;
			lpCntxtPriv->rcTip.bottom=nTop+size.cy;
		}
	}
	nOffset+=g_SkinInput.modeHerizontal.rcMargin.right+g_SkinInput.modeHerizontal.nCandRight;
	return nOffset;
}

//************************************
// Method:    InputWnd_CalcCandVert
// FullName:  InputWnd_CalcCandVert
// Access:    public 
// Returns:   int
// Qualifier: 计算竖排重码窗口需要的高度
// Parameter: HWND hWnd
// Parameter: HDC hdc
// Parameter: LPCONTEXTPRIV lpCntxtPriv
//************************************
int InputWnd_CalcCandVert(HWND hWnd,HDC hdc,LPCONTEXTPRIV lpCntxtPriv)
{
	INPUTLAYOUT *pLayout=&g_SkinInput.modeVertical;
	int nOffset=pLayout->rcMargin.top+pLayout->nCompHei+pLayout->nLineHei;
	int nLeft=pLayout->rcMargin.left+pLayout->nCandLeft;
	int nRight=pLayout->size.cx-pLayout->rcMargin.right-pLayout->nCandRight;
	SIZE size;
	int nOffsetX=0;
	int nMinHei=nOffset;
	GetTextExtentPoint(hdc,"A",1,&size);
	nMinHei+=g_SettingsG.cPageCands*size.cy+(g_SettingsG.cPageCands-1)*pLayout->nCandInter;
	lpCntxtPriv->rcComp.left=pLayout->rcMargin.left+pLayout->nCompLeft;
	lpCntxtPriv->rcComp.right=pLayout->size.cx-(pLayout->rcMargin.right+pLayout->nCompRight);
	lpCntxtPriv->rcComp.top=pLayout->rcMargin.top+(pLayout->nCompHei-size.cy)/2;
	lpCntxtPriv->rcComp.bottom=lpCntxtPriv->rcComp.top+size.cy;
	
	g_SkinInput.wndBtn.pBtns[IDC_PREVPAGE-IDC_BTN_BASE].bVisible=FALSE;
	g_SkinInput.wndBtn.pBtns[IDC_NEXTPAGE-IDC_BTN_BASE].bVisible=FALSE;

	//计算重码需要长度
	if(lpCntxtPriv->sCandCount)
	{
		int i,iCand=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands;
		int nCands=min(g_SettingsG.cPageCands,lpCntxtPriv->sCandCount-iCand);
		LPRECT pRcCand=lpCntxtPriv->rcCand;
		char szBuf[100];
		for(i=0,pRcCand=lpCntxtPriv->rcCand;i<nCands;i++,iCand++,pRcCand++)
		{
			LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
			nOffsetX=nLeft;
			pRcCand->top=nOffset;
			pRcCand->left=nOffsetX;
			szBuf[0]='0'+(1+i)%10;
			szBuf[1]='.';
			GetTextExtentPoint(hdc,szBuf,2,&size);
			nOffset+=size.cy;
			pRcCand->bottom=nOffset;

			nOffsetX+=size.cx;
			if(lpCntxtPriv->inState==INST_CODING)
			{
				BYTE byRate=*pbyCand++;
				GetTextExtentPoint(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size);
				nOffsetX+=size.cx;
				pbyCand+=pbyCand[0]+1;
				if(pbyCand[0])
				{//有编码信息
					if(byRate==RATE_WILD)
					{//万能键,显示全编码
						GetTextExtentPoint(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size);
						nOffsetX+=size.cx;
					}else if(byRate==RATE_ASSOCIATE)
					{//自动联想,显示联想到的编码
						GetTextExtentPoint(hdc,(LPSTR)pbyCand+1+lpCntxtPriv->cCompLen,pbyCand[0]-lpCntxtPriv->cCompLen,&size);
						nOffsetX+=size.cx;
					}
				}
			}else if(lpCntxtPriv->inState==INST_ENGLISH)
			{//英文状态:len+phrase
				GetTextExtentPoint(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size);
				nOffsetX+=size.cx;
			}else if(lpCntxtPriv->inState==INST_USERDEF)
			{//用户自定义状态:len+phrase+complen+comp
				GetTextExtentPoint(hdc,(LPSTR)pbyCand+2,pbyCand[1],&size);
				nOffsetX+=size.cx;
			}
			if(nOffsetX<nRight)
				pRcCand->right=nOffsetX;
			else
				pRcCand->right=-(nOffsetX-nRight);
			nOffset+=pLayout->nCandInter;
		}
		nOffset-=pLayout->nCandInter;
		nOffsetX=nRight;
		//更新翻页按钮状态
		{
			int nPages=(lpCntxtPriv->sCandCount+g_SettingsG.cPageCands-1)/g_SettingsG.cPageCands;
			SIMBTNINFO *pBtnPrev=g_SkinInput.wndBtn.pBtns+IDC_PREVPAGE-IDC_BTN_BASE;
			SIMBTNINFO *pBtnNext=g_SkinInput.wndBtn.pBtns+IDC_NEXTPAGE-IDC_BTN_BASE;
			if(lpCntxtPriv->sCurCandPage<nPages-1)
			{
				pBtnNext->bVisible=TRUE;
				OffsetRect(&pBtnNext->rc,
					nOffsetX-RectWidth(&pBtnNext->rc)-pBtnNext->rc.left,
					pLayout->rcMargin.top+(pLayout->nCompHei-RectHeight(&pBtnNext->rc))/2-pBtnNext->rc.top);
				nOffsetX-=RectWidth(&pBtnNext->rc);
			}
			if(lpCntxtPriv->sCurCandPage>0)
			{
				pBtnPrev->bVisible=TRUE;
				OffsetRect(&pBtnPrev->rc,
					nOffsetX-RectWidth(&pBtnPrev->rc)-pBtnPrev->rc.left,
					pLayout->rcMargin.top+(pLayout->nCompHei-RectHeight(&pBtnNext->rc))/2-pBtnPrev->rc.top);
				nOffsetX-=RectWidth(&pBtnPrev->rc);
			}
		}
	}else if(lpCntxtPriv->sbState!=SBST_NORMAL && lpCntxtPriv->sSentLen)
	{//计算语句提示显示位置
		lpCntxtPriv->rcSent.left=nLeft;
		lpCntxtPriv->rcSent.right=nRight;
		lpCntxtPriv->rcSent.top=nOffset;
		nOffset+=VSentTextoutPoint(hdc,lpCntxtPriv,nRight-nLeft,pLayout->nCandInter);
		lpCntxtPriv->rcSent.bottom=nOffset;
	}
	if(g_bTipShow)
	{//显示操作提示及提示信息
		nOffset+=pLayout->nCandInter;
		lpCntxtPriv->rcTip.left=nLeft;
		lpCntxtPriv->rcTip.top=nOffset;
		Helper_TextBox_CalcRect(hdc,g_szTip,-1,&size,nRight-nLeft,4,pLayout->nCandInter);
		nOffset+=size.cy;
		lpCntxtPriv->rcTip.bottom=nOffset;
		lpCntxtPriv->rcTip.right=nLeft+size.cx;
		if(nOffset<pLayout->size.cy-pLayout->rcMargin.bottom)
		{//自动将提示信息放在重码窗口的底部
			OffsetRect(&lpCntxtPriv->rcTip,0,pLayout->size.cy-pLayout->rcMargin.bottom-nOffset);
			nOffset=pLayout->size.cy-pLayout->rcMargin.bottom;
		}
	}
	if(g_SkinInput.hBmp32CharMode)
	{//有标点图标
		SIZE szIcon=Helper_GetBMPSize(g_SkinInput.hBmp32CharMode);
		if(nOffsetX==0) nOffsetX=nRight;
		SetRect(&lpCntxtPriv->rcCharMode,
			nOffsetX-szIcon.cx/2,
			pLayout->rcMargin.top+(pLayout->nCompHei-szIcon.cy)/2,
			nOffsetX,
			pLayout->rcMargin.top+(pLayout->nCompHei-szIcon.cy)/2+szIcon.cy);
		nOffsetX-=szIcon.cx/2;
	}
	if(nOffset<nMinHei) nOffset=nMinHei;
	nOffset+=pLayout->rcMargin.bottom;
	return nOffset;
}


//************************************
// Method:    InputWnd_CalcCompWid
// FullName:  InputWnd_CalcCompWid
// Access:    public 
// Returns:   int
// Qualifier: 计算编码窗口需要的宽度
// Parameter: HWND hWnd
// Parameter: HDC hdc
// Parameter: LPCONTEXTPRIV lpCntxtPriv
//************************************
int InputWnd_CalcCompWid(HWND hWnd,HDC hdc,LPCONTEXTPRIV lpCntxtPriv)
{
	int nOffset=g_SkinInput.modeHerizontal.rcMargin.left+g_SkinInput.modeHerizontal.nCandLeft;
	SIZE size;
	GetTextExtentPoint(hdc,"A",1,&size);
	lpCntxtPriv->rcComp.left=nOffset;
	lpCntxtPriv->rcComp.top=g_SkinInput.modeHerizontal.rcMargin.top+(g_SkinInput.modeHerizontal.nCompHei-size.cy)/2;
	lpCntxtPriv->rcComp.bottom=lpCntxtPriv->rcComp.top+size.cy;
	//计算编码需要长度
	if(lpCntxtPriv->inState==INST_CODING)
	{
		if(lpCntxtPriv->sbState==SBST_NORMAL)
		{//一般状态
			if(g_CompMode==IM_SPELL)
			{//拼音状态
				if(g_bTempSpell)
				{
					GetTextExtentPoint(hdc,"t:",2,&size);
					nOffset+=size.cx;
				}
				GetTextExtentPoint(hdc,(char*)lpCntxtPriv->szWord,lpCntxtPriv->bySyllables*2,&size);
				nOffset+=size.cx;
				if(!lpCntxtPriv->bPYBiHua)
				{
					GetTextExtentPoint(hdc,"      ",6,&size);
					nOffset+=size.cx;
				}else
				{
					GetTextExtentPoint(hdc,lpCntxtPriv->spellData[lpCntxtPriv->byCaret].szSpell,lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen,&size);
					nOffset+=size.cx;
					if(strlen(g_szPYBiHua)<5)
					{
						GetTextExtentPoint(hdc,"+     ",6,&size);
						nOffset+=size.cx;
					}else
					{
						GetTextExtentPoint(hdc,"+",1,&size);
						nOffset+=size.cx;
						GetTextExtentPoint(hdc,g_szPYBiHua,strlen(g_szPYBiHua),&size);
						nOffset+=size.cx;
					}
				}
				if(g_bTipShow && !g_bTempSpell)
				{//显示tip(不在临时拼音状态)
					nOffset+=g_SkinInput.modeHerizontal.nCandInter*2;
					lpCntxtPriv->rcTip.left=nOffset;
					lpCntxtPriv->rcTip.top=lpCntxtPriv->rcComp.top;
					GetTextExtentPoint(hdc,g_szTip,strlen(g_szTip),&size);
					nOffset+=size.cx;
					lpCntxtPriv->rcTip.right=nOffset;
					lpCntxtPriv->rcTip.bottom=lpCntxtPriv->rcComp.bottom;
				}
			}else
			{
				GetTextExtentPoint(hdc,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size);
				nOffset+=size.cx;
				if(g_bTipShow && !lpCntxtPriv->bWebMode)
				{//显示tip(不是网址模式)
					if(ISComm_GetCompInfo()->cCodeMax>(char)lpCntxtPriv->cCompLen)
					{
						GetTextExtentPoint(hdc," ",1,&size);//保留显示剩下的编码的空间
						nOffset+=size.cx*(ISComm_GetCompInfo()->cCodeMax-lpCntxtPriv->cCompLen);
					}
					nOffset+=g_SkinInput.modeHerizontal.nCandInter*2;
					lpCntxtPriv->rcTip.left=nOffset;
					lpCntxtPriv->rcTip.top=lpCntxtPriv->rcComp.top;
					GetTextExtentPoint(hdc,g_szTip,strlen(g_szTip),&size);
					nOffset+=size.cx;
					lpCntxtPriv->rcTip.right=nOffset;
					lpCntxtPriv->rcTip.bottom=lpCntxtPriv->rcComp.bottom;
				}
			}
		}else
		{//联想或者语句输入状态
			GetTextExtentPoint(hdc,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,&size);
			if(size.cx>MAX_SENTHEAD) size.cx=MAX_SENTHEAD;	//拼音输入时可能一次输入了一个较长的短语
			nOffset+=size.cx;
			if(g_SettingsG.bAstSent)
			{
				if(lpCntxtPriv->sSentLen)
				{
					GetTextExtentPoint(hdc,(LPSTR)lpCntxtPriv->pbySentWord[0],lpCntxtPriv->sSentLen,&size);
					nOffset+=size.cx;
				}
			}			
		}
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//英文单词输入状态
		if(lpCntxtPriv->pbyEnSpell)
		{//有英文单词
			HGDIOBJ hOldFont=0;
			GetTextExtentPoint(hdc,(LPSTR)lpCntxtPriv->pbyEnSpell+1,lpCntxtPriv->pbyEnSpell[0],&size);
			nOffset+=size.cx;
			if(lpCntxtPriv->pbyEnPhontic[0])
			{
				GetTextExtentPoint(hdc,"[",1,&size);
				nOffset+=size.cx;
				if(g_hPhoneticFont) hOldFont=SelectObject(hdc,g_hPhoneticFont);
				GetTextExtentPoint(hdc,(LPSTR)lpCntxtPriv->pbyEnPhontic+1,lpCntxtPriv->pbyEnPhontic[0],&size);
				nOffset+=size.cx;
				if(g_hPhoneticFont) SelectObject(hdc,hOldFont);
				GetTextExtentPoint(hdc,"]",1,&size);
				nOffset+=size.cx;
			}
		}else
		{//没有单词，只是字母组合
			GetTextExtentPoint(hdc,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size);
			nOffset+=size.cx;
		}
	}else if(lpCntxtPriv->inState==INST_USERDEF)
	{//用户自定义输入状态
		GetTextExtentPoint(hdc,"u:",2,&size);//显示"u:"字头
		nOffset+=size.cx;
		if(lpCntxtPriv->cCompLen)
		{
			GetTextExtentPoint(hdc,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size);
			nOffset+=size.cx;
			if(g_pbyData[0]>lpCntxtPriv->cCompLen)
			{//检测后续编码占用的空间
				LPBYTE pbyComp=g_pbyData;
				GetTextExtentPoint(hdc,(LPSTR)pbyComp+1+lpCntxtPriv->cCompLen,pbyComp[0]-lpCntxtPriv->cCompLen,&size);
				nOffset+=size.cx;
			}
		}
	}else if(lpCntxtPriv->inState==INST_LINEIME)
	{//笔画输入状态
		LPTSTR pszComp=lpCntxtPriv->szComp;
		BYTE i;
		GetTextExtentPoint(hdc,"i:",2,&size);//显示"i:"字头
		nOffset+=size.cx;
		for(i=0;i<lpCntxtPriv->cCompLen;i++)
		{
			GetTextExtentPoint(hdc,g_szLine[pszComp[i]-'1'],1,&size);
			nOffset+=size.cx+2;
		}
	}
	lpCntxtPriv->rcComp.right=nOffset;
	if(g_SkinInput.hBmp32CharMode)
	{
		SIZE szIcon=Helper_GetBMPSize(g_SkinInput.hBmp32CharMode);
		nOffset+=szIcon.cx;
	}
	nOffset+=g_SkinInput.modeHerizontal.rcMargin.right+g_SkinInput.modeHerizontal.nCompRight;
	return nOffset;
}

//************************************
// Method:    InputWnd_OnCalcSize
// FullName:  InputWnd_OnCalcSize
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: HWND hWnd
// Parameter: WPARAM wParam :为1代表不重新计算尺寸只移动位置
// Parameter: LPARAM lParam :为1代表强制更新窗口区域
//************************************
void InputWnd_OnCalcSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
	HIMC hIMC=(HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
	if(hIMC){
		HGLOBAL hUiPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
		LPUIPRIV lpUiPriv=(LPUIPRIV)GlobalLock(hUiPriv);
		LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
		if(lpIMC)
		{//可能获取不到IMC, 这里直接退出
			LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
			POINT pt=g_SettingsL.bMouseFollow?g_ptInputPos:lpUiPriv->ptInputWnd;
			HDC hdc=GetDC(hWnd);
			HGDIOBJ hOldFont=SelectObject(hdc,g_SkinInput.hFont);
			RECT rcWorkArea,rcWnd;
			SIZE sizeWnd;
			int nLineHei=16;
			//获得字体高度
			if(lpIMC->lfFont.A.lfHeight!=0)
				nLineHei=abs(lpIMC->lfFont.A.lfHeight);
			else if(lpIMC->lfFont.A.lfWidth!=0)
				nLineHei=2*abs(lpIMC->lfFont.A.lfWidth);
			if(wParam==0)
			{//需要计算窗口大小
				s_bInputVertical=FALSE;
				if(g_SettingsG.bIptWndVertical 
					&& g_SkinInput.modeVertical.pImgBack
					&& g_CompMode==IM_SHAPECODE
					&& (lpCntxtPriv->sbState==SBST_NORMAL || (lpCntxtPriv->sCandCount==0 || lpCntxtPriv->sSentLen==0)))
					s_bInputVertical=TRUE;

				memset(lpCntxtPriv->rcCand,0,10*sizeof(RECT));
				SetRect(&lpCntxtPriv->rcHeader,0,0,0,0);
				SetRect(&lpCntxtPriv->rcTip,-1,-1,-1,-1);
				SetRect(&lpCntxtPriv->rcSent,0,0,0,0);
				SetRect(&lpCntxtPriv->rcCharMode,0,0,0,0);
				if(s_bInputVertical)
				{
					SIZE size;
					GetTextExtentPoint(hdc,"A",1,&size);
					sizeWnd.cx=g_SkinInput.modeVertical.size.cx;
					sizeWnd.cy=InputWnd_CalcCandVert(hWnd,hdc,lpCntxtPriv);
					sizeWnd.cy=max(sizeWnd.cy,g_SkinInput.modeVertical.size.cy); 
				}else
				{
					int nCandWid=InputWnd_CalcCandHorz(hWnd,hdc,lpCntxtPriv);
					int nCompWid= InputWnd_CalcCompWid(hWnd,hdc,lpCntxtPriv);
					sizeWnd.cx=max(nCandWid,nCompWid);
					sizeWnd.cx=max(sizeWnd.cx,g_SkinInput.modeHerizontal.size.cx);
					sizeWnd.cy=g_SkinInput.modeHerizontal.size.cy;
					if(g_SkinInput.hBmp32CharMode)
					{//有标点图标
						int nRight=sizeWnd.cx-g_SkinInput.modeHerizontal.rcMargin.right-g_SkinInput.modeHerizontal.nCompRight;
						SIZE szIcon=Helper_GetBMPSize(g_SkinInput.hBmp32CharMode);
						SetRect(&lpCntxtPriv->rcCharMode,
							nRight-szIcon.cx/2,
							g_SkinInput.modeHerizontal.rcMargin.top+(g_SkinInput.modeHerizontal.nCompHei-szIcon.cy)/2,
							nRight,
							g_SkinInput.modeHerizontal.rcMargin.top+(g_SkinInput.modeHerizontal.nCompHei-szIcon.cy)/2+szIcon.cy);
					}
				}

			}else
			{//位置改变
				GetClientRect(hWnd,&rcWnd);
				sizeWnd.cx=rcWnd.right;
				sizeWnd.cy=rcWnd.bottom;
			}
			SelectObject(hdc,hOldFont);
			ReleaseDC(hWnd,hdc);

			SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
			if(pt.x==-1 && pt.y==-1)
			{
				GetCaretPos(&pt);
				if(pt.x==0 && pt.y==0)
				{
					pt=lpUiPriv->ptInputWnd;
				}
				else
				{
					ClientToScreen(lpIMC->hWnd,&pt);
				}
			}
			pt.y+=nLineHei*15/10;
			if(pt.x+sizeWnd.cx>rcWorkArea.right) 
				pt.x=rcWorkArea.right-sizeWnd.cx;
			if(pt.y+sizeWnd.cy>rcWorkArea.bottom)
				pt.y-=sizeWnd.cy+nLineHei*2;
			GetClientRect(hWnd,&rcWnd);// Get window's size of original state
			SetWindowPos(hWnd,NULL,pt.x,pt.y,sizeWnd.cx,sizeWnd.cy,SWP_NOZORDER|SWP_NOACTIVATE);
			if(lParam || rcWnd.right!=sizeWnd.cx || rcWnd.bottom!=sizeWnd.cy)
				InputWnd_SetRgn(hWnd,s_bInputVertical);

			ImmUnlockIMCC(lpIMC->hPrivate);
			ImmUnlockIMC(hIMC);

			InvalidateRect(hWnd,NULL,TRUE);
		}

		GlobalUnlock(hUiPriv);

	}
}


void InputWnd_PaintCand(HWND hWnd,HDC hdc,LPCONTEXTPRIV lpCntxtPriv)
{
	SIZE size;
	char szBuf[100];
	short iCand=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands;
	short i=0,nCands=min(lpCntxtPriv->sCandCount-iCand,g_SettingsG.cPageCands);
	RECT *pRcCand=pRcCand=lpCntxtPriv->rcCand;
	int nOffsetX=0;
	BYTE *pbyCand;
	if(!s_bInputVertical
		&& lpCntxtPriv->inState==INST_CODING
		&& lpCntxtPriv->sbState!=SBST_NORMAL
		&& g_SettingsG.byAstMode==AST_ENGLISH
		&& lpCntxtPriv->sCandCount)
	{//显示英文联想的词头
		SetTextColor(hdc,g_ColorCand.crEnHead);
		nOffsetX=lpCntxtPriv->rcHeader.left;
		Helper_TextOut(hdc,nOffsetX,lpCntxtPriv->rcHeader.top,(LPSTR)lpCntxtPriv->pbyEnAstPhrase+1,lpCntxtPriv->pbyEnAstPhrase[0],&size,-1);
		nOffsetX+=size.cx;
		Helper_TextOut(hdc,nOffsetX,lpCntxtPriv->rcHeader.top,":",1,&size,-1);
	}
	for(; i<nCands; i++,iCand++,pRcCand++)
	{//显示重码
		nOffsetX=pRcCand->left;
		pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
		szBuf[0]='0'+(1+i)%10;
		szBuf[1]='.';
		SetTextColor(hdc,g_ColorCand.crIndex);
		Helper_TextOut(hdc,nOffsetX,pRcCand->top,szBuf,2,&size,-1);
		nOffsetX+=size.cx;
		SetTextColor(hdc,g_ColorCand.crNormal);
		if(lpCntxtPriv->inState==INST_CODING)
		{
			if(lpCntxtPriv->sbState==SBST_NORMAL)
			{//编码状态：rate+len+phrase+len+comp
				if(g_CompMode==IM_SPELL)
				{//拼音
					Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPCSTR)pbyCand+2,pbyCand[1],&size,MAX_CANDWID);
					nOffsetX+=size.cx;
				}else
				{//形码			
					BYTE byRate=*pbyCand++;
					int nMaxWid=MAX_CANDWID;
					if(pRcCand->right<0)
					{//pRcCand->right<0代表完整显示该重码还缺少abs(pRcCand->right)个象素。
						GetTextExtentPoint(hdc,(LPSTR)pbyCand+1,pbyCand[0],&size);
						nMaxWid=size.cx+pRcCand->right;
					}
					switch(byRate)
					{//通过词频来区分重码类型
					case RATE_GBK:	//set a inactive color for GBK Cand
						SetTextColor(hdc,g_SettingsG.nGbkMode==1?g_ColorCand.crInactive:g_ColorCand.crNormal);	
						break;
					case RATE_USERDEF:case RATE_USERCMD:
						SetTextColor(hdc,g_ColorCand.crUserDef);
						break;
					case RATE_FORECAST:
						SetTextColor(hdc,g_ColorCand.crAssociation);
						break;
					case RATE_MIXSP:
						SetTextColor(hdc,g_ColorCand.crMixSp);
						break;
					default:
						SetTextColor(hdc,g_ColorCand.crNormal);
						break;
					}
					Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+1,pbyCand[0],&size,nMaxWid);
					nOffsetX+=size.cx;
					pbyCand+=pbyCand[0]+1;
					if(pbyCand[0])
					{//有编码信息
						if(byRate==RATE_WILD)
						{//万能键,显示全编码
							BYTE iComp;
							for( iComp=0;iComp<pbyCand[0];iComp++)
							{//逐编码输出，以便设置万能键的颜色
								if(lpCntxtPriv->szComp[iComp]==ISComm_GetCompInfo()->cWildChar)
									SetTextColor(hdc,g_ColorCand.crCompWild);
								else
									SetTextColor(hdc,g_ColorCand.crCompNormal);
								Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPCSTR)pbyCand+1+iComp,1,&size,-1);
								nOffsetX+=size.cx;
							}
						}else if(byRate==RATE_ASSOCIATE)
						{//自动联想,显示联想到的编码
							SetTextColor(hdc,g_ColorCand.crCompNormal);
							Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+1+lpCntxtPriv->cCompLen,pbyCand[0]-lpCntxtPriv->cCompLen,&size,-1);
							nOffsetX+=size.cx;
						}
					}
				}
			}else 
			{
				if(g_SettingsG.byAstMode==AST_CAND)
				{//词组联想:level+rate+len+phrase
					Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+3+pbyCand[0],pbyCand[2]-pbyCand[0],&size,MAX_CANDWID);
					nOffsetX+=size.cx;
				}else if(g_SettingsG.byAstMode==AST_ENGLISH)
				{//英文联想:len+spell+len+phonetic
					HGDIOBJ hOldFont=0;
					SetTextColor(hdc,g_ColorCand.crNormal);
					Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
					nOffsetX+=size.cx;
					pbyCand+=1+pbyCand[0];
					if(pbyCand[0])
					{
						SetTextColor(hdc,g_ColorCand.crEnPhonetic);
						Helper_TextOut(hdc,nOffsetX,pRcCand->top,"[",1,&size,-1);
						nOffsetX+=size.cx;
						if(g_hPhoneticFont) hOldFont=SelectObject(hdc,g_hPhoneticFont);
						Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+1,pbyCand[0],&size,MAX_CANDWID);
						nOffsetX+=size.cx;
						if(g_hPhoneticFont) SelectObject(hdc,hOldFont);
						Helper_TextOut(hdc,nOffsetX,pRcCand->top,"]",1,&size,-1);
						nOffsetX+=size.cx;
					}
				}			
			}
		}else if(lpCntxtPriv->inState==INST_ENGLISH)
		{//英文状态:len+phrase
			int nMaxWid=MAX_CANDWID;
			if(pRcCand->right<0)
			{
				nMaxWid=g_SkinInput.modeVertical.size.cx-g_SkinInput.modeVertical.rcMargin.right-nOffsetX;
			}
			Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+1,pbyCand[0],&size,nMaxWid);
			//nOffsetX+=size.cx;
		}else if(lpCntxtPriv->inState==INST_USERDEF)
		{//用户自定义输入:len+phrase
			int nMaxWid=MAX_CANDWID;
			if(pRcCand->right<0)
			{
				nMaxWid=g_SkinInput.modeVertical.size.cx-g_SkinInput.modeVertical.rcMargin.right-nOffsetX;
			}
			Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+2,pbyCand[1],&size,nMaxWid);
			//nOffsetX+=size.cx;
		}else if(lpCntxtPriv->inState==INST_LINEIME)
		{//笔画输入状态:rate+len+phrase
			Helper_TextOut(hdc,nOffsetX,pRcCand->top,(LPSTR)pbyCand+2,pbyCand[1],&size,MAX_CANDWID);
			//nOffsetX+=size.cx;
		}
	}
	if(s_bInputVertical && lpCntxtPriv->sbState!=SBST_NORMAL)
	{//显示语句联想
		COLORREF crSel,crLeft;
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
		{
			crSel=crLeft=g_ColorComp.crSentInit;
		}else
		{
			crSel=g_ColorComp.crSentSel;
			crLeft=g_ColorComp.crSentNormal;
		}
		VSentTextout(hdc,lpCntxtPriv,&lpCntxtPriv->rcSent,g_SkinInput.modeVertical.nCandInter,crSel,crLeft);
	}
}

void InputWnd_OnLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)LOWORD(lParam),(short)HIWORD(lParam)};
	if(!SimBtn_OnMouseDown(&g_SkinInput.wndBtn,hWnd,pt) && !g_SettingsL.bMouseFollow)
	{
		SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam);
	}
}


void InputWnd_OnMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)(LOWORD(lParam)),(short)(HIWORD(lParam))};
	DWORD dwClick=GetWindowLongPtr(hWnd,GWLP_USERDATA);
	BOOL bInWnd=TRUE;
	if(g_hRgnInput) bInWnd=PtInRegion(g_hRgnInput,pt.x,pt.y);
	else
	{
		RECT rc;
		GetClientRect(hWnd,&rc);
		bInWnd=PtInRect(&rc,pt);
	}
	
	if(dwClick)
	{
		ClientToScreen(hWnd,&pt);
		SetWindowPos(hWnd,NULL,pt.x-(short)LOWORD(dwClick),pt.y-(short)HIWORD(dwClick),0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}else if(SimBtn_OnMouseMove(&g_SkinInput.wndBtn,hWnd,pt,bInWnd))
	{
		SendMessage(g_hWndToolTip,TTM_ADDTOOL,g_SkinInput.wndBtn.pFocus->cmdID,(LPARAM)g_SkinInput.wndBtn.pFocus->szTip);
		SetCursor(LoadCursor(g_hInst,MAKEINTRESOURCE(IDC_MYHAND)));
	}else
	{
		RECT rc;
		GetClientRect(hWnd,&rc);
		if(!PtInRect(&rc,pt))
		{
			Plugin_CandLoseFocus();
			ReleaseCapture();
		}else
		{
			HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
			HIMC hIMC=(HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
			if(hIMC)
			{
				LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
				LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
				int i,iCand=-1;
				if(lpCntxtPriv->sCandCount)
				{
					for( i=0;i<g_SettingsG.cPageCands;i++)
					{
						if(PtInRect(lpCntxtPriv->rcCand+i,pt))
						{
							iCand=i;
							break;
						}
					}
					if(iCand!=-1)
					{
						BYTE *pbyCand=NULL,*pbyComp;
						iCand+=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands;
						pbyCand=lpCntxtPriv->ppbyCandInfo[iCand];
						pbyComp=pbyCand+2+pbyCand[0];
						Plugin_CandFocus((LPCTSTR)pbyCand+2,pbyCand[1],pbyComp+1,pbyComp[0],pbyCand[0]==RATE_WILD);
					}
				}
				ImmUnlockIMCC(lpIMC->hPrivate);
			}
			SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(g_SettingsL.bMouseFollow?IDC_ARROW:IDC_SIZEALL)));
		}
	}
}

void InputWnd_OnLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD dwClick=GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(dwClick)
	{
		RECT rc;
		HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
		HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
		LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hUIPriv);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
		GetWindowRect(hWnd,&rc);
		lpUIPriv->ptInputWnd.x=rc.left;
		lpUIPriv->ptInputWnd.y=rc.top;
		GlobalUnlock(hUIPriv);
		PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
	}else
	{
		POINT pt={(short)LOWORD(lParam),(short)HIWORD(lParam)};
		SimBtn_OnMouseUp(&g_SkinInput.wndBtn,hWnd,pt);
		InputWnd_OnMouseMove(hWnd,wParam,lParam);
	}
}

void DrawChar(HDC hdc,int x,int y, char c)
{
	if(c!=' ')
	{
		TextOut(hdc,x,y,&c,1);
	}else
	{//draw a space like |_|
		SIZE size;
		GetTextExtentPoint(hdc,&c,1,&size);
		MoveToEx(hdc,x,y+size.cy-4,NULL);
		LineTo(hdc,x,y+size.cy-1);
		LineTo(hdc,x+size.cx-1,y+size.cy-1);
		LineTo(hdc,x+size.cx-1,y+size.cy-4);
	}
}

void DrawCompText(HDC hdc,int x,int y,LPCTSTR pszText,char cbString,LPSIZE pSize,int nMaxWidth)
{
	int nTailLen=0;
	int nOffset=x;
	SIZE size;
	int i=0;
	HPEN hPen=CreatePen(PS_SOLID,1,GetTextColor(hdc));
	HGDIOBJ hOldPen=SelectObject(hdc,hPen);
	if(cbString==-1) cbString=strlen(pszText);
	GetTextExtentPoint32(hdc,pszText,cbString,pSize);
	if(nMaxWidth!=-1 && nMaxWidth<pSize->cx)
	{
		GetTextExtentPoint(hdc,"…",2,&size);
		nTailLen=size.cx;
		GetTextExtentPoint(hdc,pszText+cbString-1,1,&size);
		nTailLen+=size.cx;
	}else
	{
		nMaxWidth=pSize->cx;
	}

	while (i<cbString)
	{
		GetTextExtentPoint(hdc,pszText+i,1,&size);
		if(nOffset+size.cx>x+nMaxWidth-nTailLen) break;
		DrawChar(hdc,nOffset,y,pszText[i]);
		nOffset+=size.cx;
		i++;
	}
	if(nTailLen)
	{
		TextOut(hdc,nOffset,y,"…",2);
		GetTextExtentPoint(hdc,"…",2,&size);
		nOffset+=size.cx;
		DrawChar(hdc,nOffset,y,pszText[cbString-1]);
		GetTextExtentPoint(hdc,pszText+cbString-1,1,&size);
		nOffset+=size.cx;
	}
	pSize->cx=nOffset-x;
	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
}

void InputWnd_PaintComp(HWND hWnd,HDC hdc,LPCONTEXTPRIV lpCntxtPriv)
{
	SIZE size;
	int nOffset=lpCntxtPriv->rcComp.left;
	int nTop=lpCntxtPriv->rcComp.top;
	SetTextColor(hdc,g_ColorComp.crNormal);
	if(lpCntxtPriv->inState==INST_CODING)
	{//编码输入
		if(lpCntxtPriv->sbState==SBST_NORMAL)
		{//编码状态
			if(g_CompMode==IM_SPELL)
			{//拼音输入
				POINT pt1,pt2;
				HPEN hPen=CreatePen(PS_SOLID,1,g_ColorComp.crSpLine);
				HGDIOBJ hOldPen=SelectObject(hdc,hPen);
				if(g_bTempSpell)
				{//临时拼音头显示
					SetTextColor(hdc,g_ColorComp.crSpTmHead);
					Helper_TextOut(hdc,nOffset,nTop,"t:",2,&size,-1);
					nOffset+=size.cx;
				}
				//分３段显示预测结果
				SetTextColor(hdc,g_ColorComp.crSpResult);
				Helper_TextOut(hdc,nOffset,nTop,(LPTSTR)lpCntxtPriv->szWord,lpCntxtPriv->byCaret*2,&size,-1);
				nOffset+=size.cx;
				pt1.x=nOffset;
				SetTextColor(hdc,g_ColorComp.crSpEdit);
				Helper_TextOut(hdc,nOffset,nTop,(LPTSTR)lpCntxtPriv->szWord[lpCntxtPriv->byCaret],2,&size,-1);		
				nOffset+=size.cx;
				pt2.x=nOffset;
				pt1.y=pt2.y=nTop+size.cy+1;
				SetTextColor(hdc,g_ColorComp.crSpResult);
				Helper_TextOut(hdc,nOffset,nTop,(LPTSTR)(lpCntxtPriv->szWord+lpCntxtPriv->byCaret+1),
					(lpCntxtPriv->bySyllables-lpCntxtPriv->byCaret-1)*2,&size,-1);
				nOffset+=size.cx;
				//显示当前音节的拼音
				SetTextColor(hdc,g_ColorComp.crSpSpell);
				if(lpCntxtPriv->bySyCaret==0xFF)
				{
					Helper_TextOut(hdc,nOffset,nTop,
						lpCntxtPriv->spellData[lpCntxtPriv->byCaret].szSpell,
						lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen,&size,-1);
					nOffset+=size.cx;
				}else
				{
					SPELLINFO *lpSpi=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
					Helper_TextOut(hdc,nOffset,nTop,
						lpSpi->szSpell,
						lpCntxtPriv->bySyCaret,&size,-1);
					nOffset+=size.cx;
					pt1.y=nTop;
					pt1.x=pt2.x=nOffset;
					Helper_TextOut(hdc,nOffset,nTop,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret,&size,-1);
					nOffset+=size.cx;
				}
				if(lpCntxtPriv->bPYBiHua)
				{//显示笔画码
					int i=0;
					Helper_TextOut(hdc,nOffset,nTop,"+",1,&size,-1);
					nOffset+=size.cx;
					while(g_szPYBiHua[i])
					{
						Helper_TextOut(hdc,nOffset,nTop,g_szLine[g_szPYBiHua[i]-'1'],1,&size,-1);
						nOffset+=size.cx;
						i++;
					}
				}
				MoveToEx(hdc,pt1.x,pt1.y,NULL);
				LineTo(hdc,pt2.x,pt2.y);
				
				SelectObject(hdc,hOldPen);
				DeleteObject(hPen);
			}else
			{//形码输入
				SetTextColor(hdc,g_ColorComp.crNormal);
				Helper_TextOut(hdc,nOffset,nTop,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size,RectWidth(&lpCntxtPriv->rcComp));
			}
		}else
		{//联想状态|sentence input
			char *pResult=lpCntxtPriv->szInput;
			SetTextColor(hdc,g_ColorComp.crInput);
			Helper_TextOut(hdc,nOffset,nTop,pResult,lpCntxtPriv->nInputLen,&size,MAX_SENTHEAD);
			nOffset+=size.cx;
			if(lpCntxtPriv->sSentLen && !s_bInputVertical)
			{//语句联想显示
				int nSel=lpCntxtPriv->pbySentWord[lpCntxtPriv->sSentCaret]-lpCntxtPriv->pbySentWord[0];
				SetTextColor(hdc,g_ColorComp.crSentInit);
				if(lpCntxtPriv->sbState==SBST_SENTENCE)	SetTextColor(hdc,g_ColorComp.crSentSel);
				Helper_TextOut(hdc,nOffset,nTop,(LPSTR)lpCntxtPriv->pbySentWord[0],nSel,&size,-1);
				nOffset+=size.cx;
				if(lpCntxtPriv->sbState==SBST_SENTENCE)	SetTextColor(hdc,g_ColorComp.crSentNormal);
				Helper_TextOut(hdc,nOffset,nTop,(LPSTR)lpCntxtPriv->pbySentWord[0]+nSel,lpCntxtPriv->sSentLen-nSel,&size,-1);
			}
		}
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//英文单词输入
		if(lpCntxtPriv->pbyEnSpell)
		{//有英文单词
			HGDIOBJ hOldFont=0;
			SetTextColor(hdc,g_ColorComp.crEnCode);
			DrawCompText(hdc,nOffset,nTop,(LPSTR)lpCntxtPriv->pbyEnSpell+1,lpCntxtPriv->cCompLen,&size,-1);
			nOffset+=size.cx;
			SetTextColor(hdc,g_ColorComp.crEnMakeup);
			DrawCompText(hdc,nOffset,nTop,(LPSTR)lpCntxtPriv->pbyEnSpell+1+lpCntxtPriv->cCompLen,lpCntxtPriv->pbyEnSpell[0]-lpCntxtPriv->cCompLen,&size,-1);
			nOffset+=size.cx;
			if(lpCntxtPriv->pbyEnPhontic[0])
			{
				SetTextColor(hdc,g_ColorComp.crEnPhonetic);
				Helper_TextOut(hdc,nOffset,nTop,"[",1,&size,-1);
				nOffset+=size.cx;
				if(g_hPhoneticFont) hOldFont=SelectObject(hdc,g_hPhoneticFont);
				Helper_TextOut(hdc,nOffset,nTop,(LPSTR)lpCntxtPriv->pbyEnPhontic+1,lpCntxtPriv->pbyEnPhontic[0],&size,MAX_CANDWID);
				nOffset+=size.cx;
				if(g_hPhoneticFont) SelectObject(hdc,hOldFont);
				Helper_TextOut(hdc,nOffset,nTop,"]",1,&size,-1);
			}
		}else
		{//没有英文单词
			SetTextColor(hdc,g_ColorComp.crNormal);
			DrawCompText(hdc,nOffset,nTop,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size,-1);
			nOffset+=size.cx;
		}
	}else if(lpCntxtPriv->inState==INST_USERDEF)
	{//用户自定义输入
		LPBYTE pbyCand=lpCntxtPriv->sCandCount?lpCntxtPriv->ppbyCandInfo[0]:NULL;
		SetTextColor(hdc,g_ColorComp.crUdHead);
		Helper_TextOut(hdc,nOffset,nTop,"u:",2,&size,-1);
		nOffset+=size.cx;
		if(lpCntxtPriv->cCompLen)
		{
			SetTextColor(hdc,g_ColorComp.crUdCode);
			DrawCompText(hdc,nOffset,nTop,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,&size,RectWidth(&lpCntxtPriv->rcComp)-size.cx);
			nOffset+=size.cx;
			if(g_pbyData[0]>lpCntxtPriv->cCompLen)
			{//从服务器传回有编码信息，显示后续编码
				LPBYTE pbyComp=g_pbyData;
				SetTextColor(hdc,g_ColorComp.crUdMakeup);
				Helper_TextOut(hdc,nOffset,nTop,pbyComp+1+lpCntxtPriv->cCompLen,pbyComp[0]-lpCntxtPriv->cCompLen,&size,-1);
				nOffset+=size.cx;
			}
		}else
		{
			SetTextColor(hdc,g_ColorComp.crSentInit);
			Helper_TextOut(hdc,nOffset,nTop,"自定义输入状态",14,&size,-1);
		}
	}else if(lpCntxtPriv->inState==INST_LINEIME)
	{//笔画输入法状态
		SetTextColor(hdc,g_ColorComp.crUdHead);
		Helper_TextOut(hdc,nOffset,nTop,"i:",2,&size,-1);
		nOffset+=size.cx;
		if(lpCntxtPriv->cCompLen)
		{
			BYTE i;
			LPTSTR pComp=lpCntxtPriv->szComp;
			for(i=0;i<lpCntxtPriv->cCompLen;i++)
			{
				SetTextColor(hdc,pComp[i]<'6'?g_ColorComp.crUdCode:g_ColorComp.crUdHead);
				Helper_TextOut(hdc,nOffset,nTop,g_szLine[pComp[i]-'1'],1,&size,-1);
				nOffset+=size.cx+2;
			}
		}else
		{
			SetTextColor(hdc,g_ColorComp.crSentInit);
			Helper_TextOut(hdc,nOffset,nTop,"笔画输入法状态",14,&size,-1);
		}
	}
	if(g_SkinInput.hBmp32CharMode)
	{//先显示标点输入状态
		SIZE szIcon=Helper_GetBMPSize(g_SkinInput.hBmp32CharMode);
		PNGBLEND bf;
		bf.bBlend=TRUE;
		Bmp32_AlphaBlend(hdc,lpCntxtPriv->rcCharMode.left,
			lpCntxtPriv->rcCharMode.top,
			szIcon.cx/2,
			szIcon.cy,
			g_SkinInput.hBmp32CharMode,
			g_SettingsL.bCharMode?0:szIcon.cx/2,0,
			bf);
	}
}

static enum{FS_HIDING=-1,FS_NOSTATE=0,FS_SHOWING=1} s_FadeState=FS_NOSTATE;	//渐显隐状态，FS_HIDING：正在隐藏; FS_NOSTATE：无状态; FS_SHOWING：正在显示

LRESULT WINAPI InputWnd_Proc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	LRESULT lRet=0;
	switch(message)
	{
	case WM_TIMER:
		if(wParam==TIMER_FADE)
		{
			BYTE byStep=(BYTE)GetWindowLongPtr(hWnd,0);
			BYTE byTrans=(BYTE)ceil(2.55*g_SettingsG.byTransInput)*byStep/g_SettingsG.byFadeSteps;
			Helper_SetWindowLayerAttributes(hWnd,byTrans);
			SetWindowLongPtr(hWnd,0,byTrans);
			if(s_FadeState==FS_HIDING)
			{
				if(byStep==0)
				{
					KillTimer(hWnd,wParam);
					ShowWindow(hWnd,SW_HIDE);
					s_FadeState=FS_NOSTATE;
				}else
				{
					SetWindowLongPtr(hWnd,0,byStep-1);
				}
			}else//if(s_FadeState==FS_SHOWING
			{
				if(g_SettingsG.byFadeSteps==byStep)
				{
					KillTimer(hWnd,wParam);
					s_FadeState=FS_NOSTATE;
				}else
				{
					SetWindowLongPtr(hWnd,0,byStep+1);
				}
			}
		}
		break;
	case UM_HIDEWINDOW:
		if(IsWindowVisible(hWnd))
		{
			if(Helper_IsLayerSupport() && g_SettingsG.byFadeSteps!=0 && s_FadeState!=FS_HIDING)
			{
				BYTE byStep=g_SettingsG.byFadeSteps-1;
				if(s_FadeState==FS_SHOWING)
				{//正在显示，关闭显示定时器
					byStep=(BYTE)GetWindowLongPtr(hWnd,0);
				}
				s_FadeState=FS_HIDING;
				SetWindowLongPtr(hWnd,0,byStep);
				SetTimer(hWnd,TIMER_FADE,10,NULL);
				SendMessage(hWnd,WM_TIMER,TIMER_FADE,0);
			}else
			{
				ShowWindow(hWnd,SW_HIDE);
			}
		}
		break;
	case UM_SHOWWINDOW:
		if(Helper_IsLayerSupport() && g_SettingsG.byFadeSteps!=0 && s_FadeState!=FS_SHOWING)
		{
			BYTE byStep=1;
			if(s_FadeState==FS_HIDING)
			{//正在隐藏，关闭隐藏定时器
				byStep=(BYTE)GetWindowLongPtr(hWnd,0)+1;
			}else if(IsWindowVisible(hWnd))
			{//窗口已经可见
				break;
			}
			s_FadeState=FS_SHOWING;
			SetWindowLongPtr(hWnd,0,byStep);
			SetTimer(hWnd,TIMER_FADE,10,NULL);
			SendMessage(hWnd,WM_TIMER,TIMER_FADE,0);
		}
		ShowWindow(hWnd,SW_SHOWNOACTIVATE);
		break;
	case UM_DRAWBACKGND:
		{
			HDC hdc=(HDC)wParam;
			LPRECT pRect=(LPRECT)lParam;
			InputWnd_PaintBackgnd(hWnd,hdc,pRect);
		}
		break;
	case WM_ERASEBKGND:
		{
			HDC hdc = (HDC) wParam;
			InputWnd_PaintBackgnd(hWnd,hdc,NULL);
			lRet=1;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(hWnd,&ps);
			HGDIOBJ hOldFont=SelectObject(hdc,g_SkinInput.hFont);
			HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
			HIMC hIMC=(HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
			SetBkMode(hdc,TRANSPARENT);
			if(hIMC)
			{
				LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
				LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
				InputWnd_PaintComp(hWnd,hdc,lpCntxtPriv);
				InputWnd_PaintCand(hWnd,hdc,lpCntxtPriv);
				if(g_bTipShow && lpCntxtPriv->rcTip.left!=-1)
				{//显示提示信息
					POINT pt={lpCntxtPriv->rcTip.left,lpCntxtPriv->rcTip.top};
					SetTextColor(hdc,g_ColorCand.crTip);
					if(s_bInputVertical)
						Helper_TextBox_Draw(hdc,g_szTip,-1,pt,RectWidth(&lpCntxtPriv->rcTip),4,g_SkinInput.modeVertical.nCandInter);
					else
						TextOut(hdc,pt.x,pt.y,g_szTip,strlen(g_szTip));
				}
				ImmUnlockIMCC(lpIMC->hPrivate);
				ImmUnlockIMC(hIMC);
			}
			SimBtn_OnPaint(&g_SkinInput.wndBtn,hdc);
			SelectObject(hdc,hOldFont);
			EndPaint(hWnd,&ps);
			lRet=1;
		}
		break;
	case WM_SETCURSOR:
		SetCapture(hWnd);
		break;
	case WM_RBUTTONUP:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_MOUSEMOVE,0);//relayevent
		ContextMenu(hWnd,UIWnd_GetStatusWnd(GetWindow(hWnd,GW_OWNER)),wParam,lParam);
		break;
	case WM_LBUTTONDOWN:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_MOUSEMOVE,0);//relayevent
		InputWnd_OnLButtonDown(hWnd,wParam,lParam);
		break;
	case WM_LBUTTONUP:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_MOUSEMOVE,0);//relayevent
		InputWnd_OnLButtonUp(hWnd,wParam,lParam);
		break;
	case WM_MOUSEMOVE:
		{
			static LPARAM lp=-1;
			if(lp!=lParam)
			{
				lp=lParam;
				SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_MOUSEMOVE,0);//relayevent
				InputWnd_OnMouseMove(hWnd,wParam,lParam);
			}
		}
		break;
	case UM_CALCSIZE:
		InputWnd_OnCalcSize(hWnd,wParam,lParam);
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_PREVPAGE:
				{
					HIMC hIMC=(HIMC)GetWindowLongPtr(GetWindow(hWnd,GW_OWNER),IMMGWLP_IMC);
					if(hIMC)
					{
						LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
						LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
						if(lpCntxtPriv->sCurCandPage>0)
						{
							lpCntxtPriv->sCurCandPage--;
						}
						ImmUnlockIMCC(lpIMC->hPrivate);
						ImmUnlockIMC(hIMC);
						SendMessage(hWnd,UM_CALCSIZE,0,0);
					}
				}
				break;
			case IDC_NEXTPAGE:
				{
					HIMC hIMC=(HIMC)GetWindowLongPtr(GetWindow(hWnd,GW_OWNER),IMMGWLP_IMC);
					if(hIMC)
					{
						LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
						LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
						if(lpCntxtPriv->sCurCandPage<(lpCntxtPriv->sCandCount+g_SettingsG.cPageCands-1)/g_SettingsG.cPageCands-1)
						{
							lpCntxtPriv->sCurCandPage++;
						}
						ImmUnlockIMCC(lpIMC->hPrivate);
						ImmUnlockIMC(hIMC);
						SendMessage(hWnd,UM_CALCSIZE,0,0);
					}
				}
				break;
			}
		}
		break;
	default:
		if(!Helper_IsIMEMessage(message))
			lRet=DefWindowProc(hWnd,message,wParam,lParam);
		break;
	}
	return lRet;
}

void Rect_ScreenToClient(HWND hWnd,LPRECT pRect)
{
	POINT pt1={pRect->left,pRect->top};
	POINT pt2={pRect->right,pRect->bottom};
	ScreenToClient(hWnd,&pt1);
	ScreenToClient(hWnd,&pt2);
	SetRect(pRect,pt1.x,pt1.y,pt2.x,pt2.y);
}

void Rect_ClientToScreen(HWND hWnd,LPRECT pRect)
{
	POINT pt1={pRect->left,pRect->top};
	POINT pt2={pRect->right,pRect->bottom};
	ClientToScreen(hWnd,&pt1);
	ClientToScreen(hWnd,&pt2);
	SetRect(pRect,pt1.x,pt1.y,pt2.x,pt2.y);
}

//获得与鼠标点击位置相应的命令：优先级：文本区<拖动区<图标区
int StatusWnd_CmdFromPt(POINT pt)
{
	int nRet=STATUSCMD_NODEF;
	if(g_SettingsL.bFullStatus)
	{
		if(PtInRect(&g_SkinStatus.rcText2,pt))
			nRet=STATUSCMD_TEXT;
		if(PtInRect(&g_SkinStatus.rcDrag2,pt))
			nRet=STATUSCMD_DRAG;
		if(PtInRect(&g_SkinStatus.rcIcon2,pt))
			nRet=STATUSCMD_ICON;
	}else
	{
		if(PtInRect(&g_SkinStatus.rcText,pt))
			nRet=STATUSCMD_TEXT;
		if(PtInRect(&g_SkinStatus.rcDrag,pt))
			nRet=STATUSCMD_DRAG;
		if(PtInRect(&g_SkinStatus.rcIcon,pt))
			nRet=STATUSCMD_ICON;
	}
	return nRet;
}

void StatusWnd_UpdateDockMode(HWND hWnd)
{
	RECT rcWnd,rcWorkArea;
	GetWindowRect(hWnd,&rcWnd);
	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);

	//计算靠边类型
	if(rcWnd.left==0)
		g_ptStatusDockMode.x=-1;
	else if(rcWnd.right==rcWorkArea.right)
		g_ptStatusDockMode.x=1;
	else
		g_ptStatusDockMode.x=0;
	
	if(rcWnd.top==0)
		g_ptStatusDockMode.y=-1;
	else if(rcWnd.bottom==rcWorkArea.bottom)
		g_ptStatusDockMode.y=1;
	else
		g_ptStatusDockMode.y=0;
}

void StatusWnd_OnLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)(LOWORD(lParam)),(short)(HIWORD(lParam))};
	if(!SimBtn_OnMouseDown(&g_SkinStatus.wndBtn,hWnd,pt))
	{
		int nCmd=StatusWnd_CmdFromPt(pt);
		switch(nCmd)
		{
		case STATUSCMD_ICON:
			SendMessage(hWnd,WM_COMMAND,IDM_IMCHANGE,0);
			break;
		case STATUSCMD_DRAG:
			SetWindowLongPtr(hWnd,GWLP_USERDATA,lParam);
			break;
		}
	}
}

void StatusWnd_OnLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)(LOWORD(lParam)),(short)(HIWORD(lParam))};
	LONG dwClick=GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(dwClick!=0)
	{
		RECT rc;
		HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
		HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
		LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hUIPriv);
		SetWindowLongPtr(hWnd,GWLP_USERDATA,0);
		GetWindowRect(hWnd,&rc);
		lpUIPriv->ptStatusWnd.x=rc.left;
		lpUIPriv->ptStatusWnd.y=rc.top;
		GlobalUnlock(hUIPriv);
		StatusWnd_UpdateDockMode(hWnd);
		PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
	}else
	{
		SimBtn_OnMouseUp(&g_SkinStatus.wndBtn,hWnd,pt);
	}
}

#define SIZE_MAGNETIC	5

void StatusWnd_OnMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT pt={(short)(LOWORD(lParam)),(short)(HIWORD(lParam))};
	DWORD dwClick=GetWindowLongPtr(hWnd,GWLP_USERDATA);
	if(dwClick)
	{
		RECT rcWorkArea,rcWnd;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
		GetClientRect(hWnd,&rcWnd);
		ClientToScreen(hWnd,&pt);
		pt.x-=(short)LOWORD(dwClick);
		pt.y-=(short)HIWORD(dwClick);
		if(pt.x-rcWorkArea.left<=SIZE_MAGNETIC) pt.x=rcWorkArea.left;
		if(pt.y-rcWorkArea.top<SIZE_MAGNETIC) pt.y=rcWorkArea.top;
		if(rcWorkArea.right-pt.x-rcWnd.right<SIZE_MAGNETIC) pt.x=rcWorkArea.right-rcWnd.right;
		if(rcWorkArea.bottom-pt.y-rcWnd.bottom<SIZE_MAGNETIC) pt.y=rcWorkArea.bottom-rcWnd.bottom;
		SetWindowPos(hWnd,NULL,pt.x,pt.y,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}else
	{
		BOOL bInWnd=TRUE;
		if(g_SkinStatus.hRgnFull && g_SkinStatus.hRgnShrink)
		{
			bInWnd=PtInRegion(g_SettingsL.bFullStatus?g_SkinStatus.hRgnFull:g_SkinStatus.hRgnShrink,pt.x,pt.y);
		}else
		{
			RECT rc;
			GetClientRect(hWnd,&rc);
			bInWnd=PtInRect(&rc,pt);
		}
		if(SimBtn_OnMouseMove(&g_SkinStatus.wndBtn,hWnd,pt,bInWnd))
		{
			SendMessage(g_hWndToolTip,TTM_ADDTOOL,g_SkinStatus.wndBtn.pFocus->cmdID,(LPARAM)g_SkinStatus.wndBtn.pFocus->szTip);
			SetCursor(LoadCursor(g_hInst,MAKEINTRESOURCE(IDC_MYHAND)));
		}else
		{
			if(!bInWnd)
			{
				ReleaseCapture();
			}else
			{
				int nCmd=StatusWnd_CmdFromPt(pt);
				char szBuf[100];
				switch(nCmd)
				{
				case STATUSCMD_ICON:
					if(g_CompMode==IM_SPELL)
						sprintf(szBuf,"编码->%s Ctrl+Q",ISComm_GetCompInfo()->szName);
					else
						strcpy(szBuf,"编码->启程拼音 Ctrl+Q");
					SendMessage(g_hWndToolTip,TTM_ADDTOOL,STATUSCMD_ICON,(LPARAM)szBuf);
					SetCursor(LoadCursor(g_hInst,MAKEINTRESOURCE(IDC_MYHAND)));
					break;
				case STATUSCMD_TEXT:
					SendMessage(g_hWndToolTip,TTM_ADDTOOL,STATUSCMD_TEXT,(LPARAM)"显示当前输入状态");
					SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
					break;
				case STATUSCMD_DRAG:
					SendMessage(g_hWndToolTip,TTM_ADDTOOL,STATUSCMD_DRAG,(LPARAM)"移动鼠标调整显示位置");
					SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL)));
					break;
				case STATUSCMD_NODEF:
					SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
					break;
				}
			}
		}
	}
}

void DrawTextExtent(HDC hdc,LPCTSTR pszText,int nLen,RECT *pRect)
{
	int   nWid=RectWidth(pRect);
	int   nHei=RectHeight(pRect);
	if(nLen==-1) nLen=strlen(pszText);
	if(nWid>nHei)
	{//横向排列
		if(nWid>50)
		{
			HGDIOBJ hOldFont=SelectObject(hdc,g_SkinStatus.hFont);
			DrawText(hdc,pszText,nLen,pRect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
			SelectObject(hdc,hOldFont);
		}
	}else if(nHei>50)
	{//竖向排列，使用旋转字体
		SIZE  size;
		POINT pt;
		HGDIOBJ hOldFont=SelectObject(hdc,g_SkinStatus.hFont2);
		HRGN hRgnClip=CreateRectRgnIndirect(pRect);
		SelectClipRgn(hdc,hRgnClip);
		DeleteObject(hRgnClip);
		GetTextExtentPoint(hdc,pszText,nLen,&size);
		pt.x=pRect->right-(nWid-size.cy)/2;
		pt.y=pRect->top+(nHei-size.cx)/2;
		TextOut(hdc,pt.x,pt.y,pszText,nLen);
		SelectClipRgn(hdc,NULL);
		SelectObject(hdc,hOldFont);
	}
}

void StatusWnd_OnPaint(HWND hWnd,HDC hdc)
{
	HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
	HGLOBAL hUIPriv=(HGLOBAL)GetWindowLongPtr(hUIWnd,IMMGWLP_PRIVATE);
	LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hUIPriv);
	RECT rcIcon=g_SettingsL.bFullStatus?g_SkinStatus.rcIcon2:g_SkinStatus.rcIcon;
	RECT rcText=g_SettingsL.bFullStatus?g_SkinStatus.rcText2:g_SkinStatus.rcText;
	SetBkMode(hdc,TRANSPARENT);
	if(g_SettingsL.bFullStatus)
		Image_Show(hdc,0,0,g_SkinStatus.sizeFull.cx,g_SkinStatus.sizeFull.cy,g_SkinStatus.pImgBackFull,0,0);
	else
		Image_Show(hdc,0,0,g_SkinStatus.sizeShrink.cx,g_SkinStatus.sizeShrink.cy,g_SkinStatus.pImgBackShrink,0,0);
	SimBtn_OnPaint(&g_SkinStatus.wndBtn,hdc);

	SetTextColor(hdc,g_SkinStatus.crText);
	if(g_CompMode==IM_SPELL)
	{
		DrawTextExtent(hdc,g_bTempSpell?"启程拼音*":"启程拼音",-1,&rcText);
		if(g_SkinStatus.hIconList)
			ImageList_Draw(g_SkinStatus.hIconList,1,hdc,rcIcon.left,rcIcon.top,ILD_TRANSPARENT);
		else
			ImageList_Draw(g_himglstIcon,1,hdc,rcIcon.left,rcIcon.top,ILD_TRANSPARENT);
	}else
	{
		DrawTextExtent(hdc,ISComm_GetCompInfo()->szName,-1,&rcText);
		if(ImageList_GetImageCount(g_himglstIcon)==3)//存在编码图标
			ImageList_Draw(g_himglstIcon,2,hdc,rcIcon.left,rcIcon.top,ILD_TRANSPARENT);
		else if(g_SkinStatus.hIconList)
			ImageList_Draw(g_SkinStatus.hIconList,0,hdc,rcIcon.left,rcIcon.top,ILD_TRANSPARENT);
		else
			ImageList_Draw(g_himglstIcon,0,hdc,rcIcon.left,rcIcon.top,ILD_TRANSPARENT);
	}
	GlobalUnlock(hUIPriv);
}

void StatusWnd_EnsureVisible(HWND hWnd,int nWidth,int nHeight)
{
	POINT pt;
	HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
	RECT rcWorkArea,rcWnd,rcInter;
	POINT ptOldStatusDockMode=g_ptStatusDockMode;
	HGLOBAL hUIPriv = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
	LPUIPRIV lpUIPriv = (LPUIPRIV)GlobalLock(hUIPriv);

	SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
	GetWindowRect(hWnd,&rcWnd);
	if(nWidth!=0)
		rcWnd.right=rcWnd.left+nWidth;
	else
		nWidth=rcWnd.right-rcWnd.left;
	if(nHeight!=0)
		rcWnd.bottom=rcWnd.top+nHeight;
	else
		nHeight=rcWnd.bottom-rcWnd.top;

	IntersectRect(&rcInter,&rcWnd,&rcWorkArea);
	pt.x=rcWnd.left,pt.y=rcWnd.top;

	if(!EqualRect(&rcInter,&rcWnd))
	{
		if(rcWnd.right>rcWorkArea.right)
			pt.x=rcWorkArea.right-(rcWnd.right-rcWnd.left);
		if(rcWnd.bottom>rcWorkArea.bottom)
			pt.y=rcWorkArea.bottom-(rcWnd.bottom-rcWnd.top);
		if(rcWnd.left<rcWorkArea.left)
			pt.x=rcWorkArea.left;
		if(rcWnd.top<rcWorkArea.top)
			pt.y=rcWorkArea.top;
		lpUIPriv->ptStatusWnd=pt;
	}
	StatusWnd_UpdateDockMode(hWnd);//计算停靠模式

	if((ptOldStatusDockMode.x!=0 && ptOldStatusDockMode.x!=g_ptStatusDockMode.x)
	   ||(ptOldStatusDockMode.y!=0 && ptOldStatusDockMode.y!=g_ptStatusDockMode.y))
	{//恢复停靠模式
		if(ptOldStatusDockMode.x==-1) 
			pt.x=0;
		else if(ptOldStatusDockMode.x==1) 
			pt.x=rcWorkArea.right-RectWidth(&rcWnd);
		if(ptOldStatusDockMode.y==-1) 
			pt.y=0;
		else if(ptOldStatusDockMode.y==1) 
			pt.y=rcWorkArea.bottom-RectHeight(&rcWnd);
		lpUIPriv->ptStatusWnd=pt;
		g_ptStatusDockMode=ptOldStatusDockMode;
	}
	SetWindowPos(hWnd,NULL,pt.x,pt.y,nWidth,nHeight,SWP_NOZORDER|SWP_NOACTIVATE);
	GlobalUnlock(hUIPriv);
}

#define MK_SIMULATE	0x1000

void StatusWnd_PostMouseMove(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd,&pt);
	PostMessage(hWnd,WM_MOUSEMOVE,MK_SIMULATE,MAKELONG(pt.x,pt.y));
}

int GetClipboardText(HWND hWnd,char *pszBuf,int nBufSize)
{				
	int nRet=0;
	if(OpenClipboard(hWnd))
	{//剪贴板数据优先
		HGLOBAL hglb = GetClipboardData(CF_TEXT); 
		if(hglb)
		{
			LPSTR lpstr = (char *)GlobalLock(hglb); 
			int nLen=strlen(lpstr);
			if(lpstr && nLen<nBufSize)
			{//清除两边的空格
				while(*lpstr)
				{
					if(*lpstr == ' ' || *lpstr=='\t') lpstr++;
					else if(*lpstr == 0x0d) { if(lpstr[1]==0x0a) lpstr+=2;else lpstr++;}
					else break;
				}
				nLen=strlen(lpstr);
				while(nLen)
				{
					if(lpstr[nLen-1]==' ' || lpstr[nLen-1]=='\t'){ nLen--;lpstr[nLen]=0;}
					else if(lpstr[nLen-1]==0x0a) { if(nLen>1 && lpstr[nLen-2]==0x0d){nLen-=2;lpstr[nLen]=0;}else break;}
					else break;
				}
				strcpy(pszBuf,lpstr);
				nRet=strlen(pszBuf);
			}
			GlobalUnlock(hglb); 
		}
		CloseClipboard(); 
	}
	return nRet;
}

int GetInputText(HWND hWnd,char *pszBuf,int nBufSize)
{
	int nRet=0;
	HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
	HIMC hIMC=(HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
	if(hIMC)
	{
		LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
		LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState!=SBST_NORMAL)
		{
			if(lpCntxtPriv->nInputLen && lpCntxtPriv->nInputLen<nBufSize)
			{
				memcpy(pszBuf,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
				pszBuf[lpCntxtPriv->nInputLen]=0;
				nRet=lpCntxtPriv->nInputLen;
			}
		}
		ImmUnlockIMCC(lpIMC->hPrivate);
		ImmUnlockIMC(hIMC);
	}	
	return nRet;
}

LRESULT WINAPI StatusWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	static BOOL bTrace=FALSE;
	LRESULT lRet=0;
	switch(message)
	{
	case WM_INITMENUPOPUP:
		{
			HMENU hMenu=(HMENU)wParam;
			if(GetMenuContextHelpId(hMenu)!=0) break;
			switch(LOWORD(lParam))
			{
			case 1://批量设置
				if(!s_bInitBatch)
				{
					WIN32_FIND_DATA wfd;
					HANDLE hFindFile;
					char  szPath[MAX_PATH],szIniFile[MAX_PATH];
					sprintf(szPath,"%s\\Batch\\*.ini",g_szDataPath);
					hFindFile=FindFirstFile(szPath,&wfd);
					if(hFindFile!=INVALID_HANDLE_VALUE)
					{
						char szType[50];
						int i=0;
						DeleteMenu(hMenu,0,MF_BYPOSITION);
						do{
							sprintf(szIniFile,"%s\\Batch\\%s",g_szDataPath,wfd.cFileName);
							if(GetPrivateProfileString("info","type",NULL,szType,50,szIniFile))
							{
								InsertMenu(hMenu,i,MF_BYPOSITION|MF_STRING,ID_BATCH_SETTINGS1+i,szType);
								strcpy(s_szBatch[i++],wfd.cFileName);
								if(i>=20) break;
							}
						}while(FindNextFile(hFindFile,&wfd));
						FindClose(hFindFile);
					}else
					{
						EnableMenuItem(hMenu,0,MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
					}	
					s_bInitBatch=TRUE;
				}
				break;
			case 6://特殊符号
				if(!s_bInitSpChar)
				{
					if(g_SpCharData.nLayoutCount>0) 
					{
						int i=0;
						DeleteMenu(hMenu,0,MF_BYPOSITION);
						for(i=0;i<g_SpCharData.nLayoutCount;i++)
						{
							InsertMenu(hMenu,i,MF_BYPOSITION|MF_STRING,ID_SPCHAR_FIRST+i,g_SpCharData.layout[i].szName);
						}
					}else
					{
						EnableMenuItem(hMenu,0,MF_BYPOSITION|MF_GRAYED);
					}
					s_bInitSpChar=TRUE;
				}
				break;
			case 7://皮肤
				if(s_nSkins==0)
				{
					char szPath[MAX_PATH];
					int nSkins=Skin_Enum(s_szSkins,ID_SKIN_END-ID_SKIN_BEGIN+1,g_szDataPath);
					if(g_SettingsG.bIptWndVertical) CheckMenuItem(hMenu,ID_SKIN_VERTICAL,MF_CHECKED |MF_BYCOMMAND);
					if(nSkins)
					{
						char szName[50],szAuthor[50],szItem[100];
						char szCurSkin[MAX_PATH];
						UINT uFlags;
						int i;
						sprintf(szPath,"%s\\config.ini",g_szDataPath);
						GetPrivateProfileString("IME","skin","",szCurSkin,MAX_PATH,szPath);
						for(i=0;i<nSkins;i++)
						{
							uFlags=MF_BYPOSITION|MF_STRING;
							sprintf(szPath,"%s\\Skins\\%s\\config.ini",g_szDataPath,s_szSkins[i]);
							GetPrivateProfileString("desc","name","",szName,50,szPath);
							GetPrivateProfileString("desc","author","",szAuthor,50,szPath);
							sprintf(szItem,"%s[%s]",szName,szAuthor);
							if(_stricmp(szCurSkin,s_szSkins[i])==0) 
								uFlags|=MF_CHECKED; 
							InsertMenu(hMenu,5+i,uFlags,ID_SKIN_BEGIN+i,szItem);
						}
						if(strcmp(szCurSkin,SKIN_RANDOM)==0)
						{
							CheckMenuItem(hMenu,ID_SKIN_RANDOM,MF_CHECKED |MF_BYCOMMAND);
						}
					}
					s_nSkins=nSkins;
				}
				break;
			case 8://用户词典
				if(!s_bInitDict && ISComm_UserDict_List()==ISACK_SUCCESS)
				{
					PMSGDATA pData=ISComm_GetData();
					BYTE i,byCount=pData->byData[0];
					char *pBuf=(char*)pData->byData+1;
					char szItem[100];
					for(i=0;i<byCount;i++)
					{
						char *pName=pBuf+1;
						char *pType=pName+strlen(pName)+1;
						sprintf(szItem,"%s[%s]",pName,pType);
						InsertMenu(hMenu,i+4,MF_BYPOSITION|MF_STRING|(pBuf[0]?MF_CHECKED:0),ID_UDICT_1+i,szItem);
						strcpy(s_szUDict[i],pName);
						pBuf=pType+strlen(pType)+1;
					}
					s_bInitDict=TRUE;
				}
				break;
			case 9://编码切换
				if(!s_bInitComp && ISComm_Comp_List()==ISACK_SUCCESS)
				{
					PMSGDATA pData=ISComm_GetData();
					BYTE i,byCount=pData->byData[0];
					char *pBuf=(char*)pData->byData+1;
					char szItem[100];
					DeleteMenu(hMenu,0,MF_BYPOSITION);
					for(i=0;i<byCount && i<(ID_COMP_END-ID_SKIN_BEGIN+1);i++)
					{
						char *pName=pBuf+1;
						char *pType=pName+strlen(pName)+1;
						sprintf(szItem,"%s[%s]",pName,pType);
						InsertMenu(hMenu,i+1,MF_BYPOSITION|MF_STRING|(pBuf[0]?MF_CHECKED:0),ID_COMP_FIRST+i,szItem);
						strcpy(s_szComp[i],pName);
						pBuf=pType+strlen(pType)+1;
					}
					s_bInitComp=TRUE;
				}
				break;
			case 10://形码混输
				{
					BOOL bCe2=0,bCe3=0,bCa4=0;
					CheckMenuItem(hMenu,ID_BLEND_SP_WORD,MF_BYCOMMAND|(g_SettingsG.bBlendSpWord?MF_CHECKED:0));
					ISComm_Bldsp_Get(&bCe2,&bCe3,&bCa4);
					CheckMenuItem(hMenu,ID_BLEND_SP_CE2,MF_BYCOMMAND|(bCe2?MF_CHECKED:0));
					CheckMenuItem(hMenu,ID_BLEND_SP_CE3,MF_BYCOMMAND|(bCe3?MF_CHECKED:0));
					CheckMenuItem(hMenu,ID_BLEND_SP_CA4,MF_BYCOMMAND|(bCa4?MF_CHECKED:0));
					CheckMenuItem(hMenu,ID_BLEND_USERDEF,MF_BYCOMMAND|(g_SettingsG.bBlendUD?MF_CHECKED:0));
				}
				break;
			case 12://数据管理
				{
					LPCTSTR pszPages=ISComm_Svr_Pages();
					if(pszPages)
					{
						UINT uID=IDM_SVR_PAGE1;
						int nPos=1;
						while(*pszPages && uID<IDM_SVR_PAGE50)
						{
							if(*pszPages=='\n')
								InsertMenu(hMenu,nPos++,MF_BYPOSITION|MF_SEPARATOR,uID++,"");
							else
								InsertMenu(hMenu,nPos++,MF_BYPOSITION|MF_STRING,uID++,pszPages);
							pszPages+=strlen(pszPages)+1;
						}
					}
					CheckMenuItem(hMenu,IDM_SVR_TRAY,MF_BYCOMMAND|(ISComm_SvrTray_Get()?MF_CHECKED:0));
				}
				break;
			}
			SetMenuContextHelpId(hMenu,1);
		}
		break;
	case WM_TIMER:
		if(wParam==TIMER_ANIMATOR)
		{
			DWORD dwStep=GetWindowLongPtr(hWnd,0);
			int nWidth=(g_SkinStatus.sizeFull.cx-g_SkinStatus.sizeShrink.cx)*dwStep/ANIMATE_STEPS+g_SkinStatus.sizeShrink.cx;
			int nHeight=(g_SkinStatus.sizeFull.cy-g_SkinStatus.sizeShrink.cy)*dwStep/ANIMATE_STEPS+g_SkinStatus.sizeShrink.cy;
			StatusWnd_EnsureVisible(hWnd,nWidth,nHeight);
			if(g_SettingsL.bFullStatus)
			{
				if(dwStep==ANIMATE_STEPS)
				{
					KillTimer(hWnd,TIMER_ANIMATOR);
					StatusWnd_PostMouseMove(hWnd);
				}else
					SetWindowLongPtr(hWnd,0,dwStep+1);
			}else
			{
				if(dwStep==0)
				{
					KillTimer(hWnd,TIMER_ANIMATOR);
					StatusWnd_PostMouseMove(hWnd);
				}else
					SetWindowLongPtr(hWnd,0,dwStep-1);
			}
		}
		break;
	case UM_DRAWBACKGND:
		{
			HDC hdc=(HDC)wParam;
			LPRECT pRect=(LPRECT)lParam;
			Image_Show(hdc,pRect->left,pRect->top,pRect->right-pRect->left,pRect->bottom-pRect->top,g_SettingsL.bFullStatus?g_SkinStatus.pImgBackFull:g_SkinStatus.pImgBackShrink,pRect->left,pRect->top);
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(hWnd,&ps);
			StatusWnd_OnPaint(hWnd,hdc);
			EndPaint(hWnd,&ps);
			lRet=1;
		}
		break;
	case WM_DISPLAYCHANGE:
		StatusWnd_EnsureVisible(hWnd,0,0);
		g_SettingsL.bMouseFollow=TRUE;
		Config_SaveInt("MouseFollow",g_SettingsL.bMouseFollow);
		PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
		break;
	case WM_SETCURSOR:
		SetCapture(hWnd);
		break;
	case WM_LBUTTONDOWN:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_LBUTTONDOWN,0);//relayevent
		StatusWnd_OnLButtonDown(hWnd,wParam,lParam);
		break;
	case WM_LBUTTONUP:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_LBUTTONUP,0);//relayevent
		StatusWnd_OnLButtonUp(hWnd,wParam,lParam);
		break;
	case WM_MOUSEMOVE:
		{
			static LPARAM lp=-1;
			if(lp!=lParam || MK_SIMULATE==wParam)
			{
				lp=lParam;
				SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_MOUSEMOVE,0);//relayevent
				StatusWnd_OnMouseMove(hWnd,wParam,lParam);
			}
		}
		break;
	case WM_RBUTTONUP:
		SendMessage(g_hWndToolTip,TTM_RELAYEVENT,WM_RBUTTONUP,0);//relayevent
		ContextMenu(hWnd,hWnd,wParam,lParam);
		break;
	case WM_COMMAND:
		{
			UINT uRet=LOWORD(wParam);
			switch(uRet)
			{
			case IDC_VIEWMODE1:
				{
					char szSkinConfig[MAX_PATH];
					sprintf(szSkinConfig,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
					g_SettingsL.bFullStatus=TRUE;
					Skin_GetBtnRect(szSkinConfig,TRUE);
					SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_VIEWMODE1-IDC_BTN_BASE,FALSE,FALSE);
					SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_VIEWMODE2-IDC_BTN_BASE,TRUE,TRUE);
					if(g_SkinStatus.hRgnFull)
						Helper_SetWindowRgn(hWnd,g_SkinStatus.hRgnFull,TRUE);
					else
						SetWindowRgn(hWnd,0,TRUE);
					SetTimer(hWnd,TIMER_ANIMATOR,10,NULL);
					SetWindowLongPtr(hWnd,0,1);
					SendMessage(hWnd,WM_TIMER,TIMER_ANIMATOR,0);
					Config_SaveInt("FullStatus",g_SettingsL.bFullStatus);
					PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
				}
				break;
			case IDC_VIEWMODE2:
				{
					char szSkinConfig[MAX_PATH];
					sprintf(szSkinConfig,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
					g_SettingsL.bFullStatus=FALSE;
					Skin_GetBtnRect(szSkinConfig,FALSE);
					
					SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_VIEWMODE2-IDC_BTN_BASE,FALSE,FALSE);
					SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_VIEWMODE1-IDC_BTN_BASE,TRUE,TRUE);
					if(g_SkinStatus.hRgnShrink)
						Helper_SetWindowRgn(hWnd,g_SkinStatus.hRgnShrink,TRUE);
					else
						SetWindowRgn(hWnd,0,TRUE);
					SetTimer(hWnd,TIMER_ANIMATOR,10,NULL);
					SetWindowLongPtr(hWnd,0,ANIMATE_STEPS-1);
					SendMessage(hWnd,WM_TIMER,TIMER_ANIMATOR,0);
					
					Config_SaveInt("FullStatus",g_SettingsL.bFullStatus);
					PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SAVEUICFG);
				}
				break;
			case IDC_CHARMODE1:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_CHARMODE1-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_CHARMODE2-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bCharMode=FALSE;
				Config_SaveInt("CharMode",g_SettingsL.bCharMode);
				PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
				break;
			case IDC_CHARMODE2:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_CHARMODE2-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_CHARMODE1-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bCharMode=TRUE;
				Config_SaveInt("CharMode",g_SettingsL.bCharMode);
				PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
				break;
			case IDC_RECORD1:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_RECORD1-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_RECORD2-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bRecord=FALSE;
				Config_SaveInt("Record",g_SettingsL.bRecord);
				break;
			case IDC_RECORD2:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_RECORD2-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_RECORD1-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bRecord=TRUE;
				Config_SaveInt("Record",g_SettingsL.bRecord);
				break;
			case IDC_SOUND1:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_SOUND1-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_SOUND2-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bSound=FALSE;
				Config_SaveInt("Sound",g_SettingsL.bSound);
				break;
			case IDC_SOUND2:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_SOUND2-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_SOUND1-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bSound=TRUE;
				Config_SaveInt("Sound",g_SettingsL.bSound);
				break;
			case IDC_ENGLISH1:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_ENGLISH1-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_ENGLISH2-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bEnglish=FALSE;
				Config_SaveInt("English",g_SettingsL.bEnglish);
				break;
			case IDC_ENGLISH2:
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_ENGLISH2-IDC_BTN_BASE,FALSE,FALSE);
				SimBtn_Show(&g_SkinStatus.wndBtn,hWnd,g_SkinStatus.wndBtn.pBtns+IDC_ENGLISH1-IDC_BTN_BASE,TRUE,TRUE);
				g_SettingsL.bEnglish=TRUE;
				Config_SaveInt("English",g_SettingsL.bEnglish);
				break;
			case IDC_MAKEWORD:
				SendMessage(hWnd,WM_COMMAND,IDM_MAKEPHRASE,0);
				break;
			case IDC_QUERY:
				SendMessage(hWnd,WM_COMMAND,IDM_QUERYCOMP,1);
				break;
			case IDC_MYHELP:
				SendMessage(hWnd,WM_COMMAND,IDM_HELP,0);
				break;
			case IDC_MENU:
				{
					POINT pt;
					GetCursorPos(&pt);
					ScreenToClient(hWnd,&pt);
					PostMessage(hWnd,WM_RBUTTONUP,0,MAKELONG(pt.x,pt.y));
				}
				break;
			case IDM_CONFIG:
				{
					char szPlugin[50]={0};
					BYTE byTransStatus=g_SettingsG.byTransStatus;
					BYTE byTransInput=g_SettingsG.byTransInput;
					char cPageCands=g_SettingsG.cPageCands;
					strcpy(szPlugin,g_SettingsG.szPlugin);
					PropSheet_Show(GetActiveWindow(),g_hInst,6);
					if((byTransStatus!=g_SettingsG.byTransStatus ||byTransInput!=g_SettingsG.byTransInput || 0==g_SettingsG.byFadeSteps) && Helper_IsLayerSupport())//透明度发生更改，更新窗口透明度
						StatusWnd_UpdateTransDegree(hWnd);
					if(cPageCands!=g_SettingsG.cPageCands)						//重码数发生更改，更新输入窗口。
						StatusWnd_UpdateInputPageCands(hWnd);
					if(strcmp(g_SettingsG.szPlugin,NOPLUGIN)==0)
					{
						strcpy(g_SettingsG.szPlugin,"");
						Plugin_Free();
					}else if(strcmp(szPlugin,g_SettingsG.szPlugin)!=0)
					{
						char szPath[MAX_PATH];
						sprintf(szPath,"%s\\plugin\\%s",g_szDataPath,g_SettingsG.szPlugin);
						Plugin_Load(szPath,GetWindow(hWnd,GW_OWNER));
					}
					if(!Tips_IsLoaded() && g_SettingsG.bShowOpTip) Tips_Load();
					else if(Tips_IsLoaded() && !g_SettingsG.bShowOpTip) Tips_Free();
				}
				break;
			case IDM_OPENSERVER:
				ISComm_ShowServer(NULL,0);
				break;
			case IDM_KEYMAP:
				ISComm_SendMsg(CT_SHOWKEYMAP,0,0,0);
				break;
			case IDM_SVR_TRAY:
				{
					BOOL bTray=!ISComm_SvrTray_Get();
					ISComm_SvrTray_Set(bTray);
				}
				break;
			case IDM_MAKEPHRASE:
				if (OpenClipboard(hWnd)) 
				{ 
					HGLOBAL hglb = GetClipboardData(CF_TEXT); 
					if(hglb)
					{
						LPSTR lpstr = (char *)GlobalLock(hglb); 
						if(lpstr)
						{
							char szBuf[200];
							if(ISComm_MakePhrase(lpstr,strlen(lpstr))==ISACK_SUCCESS)
							{
								PMSGDATA pMsg=ISComm_GetData();
								if(pMsg->byData[0]==1)
									sprintf(szBuf,"词\"%s\"已经存在",lpstr);
								else
									sprintf(szBuf,"词\"%s\"加入词库",lpstr);
							}else
							{
								sprintf(szBuf,"造词\"%s\"失败",lpstr);
							}
							SendMessage(g_hWndMsg,TTM_SETTOOLINFO,0,(LPARAM)szBuf);
						}
						GlobalUnlock(hglb); 
					}
					CloseClipboard(); 
				} 
				break;
			case IDM_IMCHANGE:
				if(!g_bTempSpell)
				{
					HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
					HIMC hIMC=(HIMC)GetWindowLongPtr(hUIWnd,IMMGWLP_IMC);
					if(hIMC)
					{
						LPINPUTCONTEXT lpIMC=(LPINPUTCONTEXT)ImmLockIMC(hIMC);
						ClearPrivateContext(lpIMC,CPC_ALL);
						g_bTipShow=FALSE;
						ImmUnlockIMC(hIMC);
						if(g_CompMode==IM_SPELL)
							g_CompMode=IM_SHAPECODE;
						else
							g_CompMode=IM_SPELL;
						Config_SaveInt("CompMode",g_CompMode);
						PostMessage(hUIWnd,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
						InvalidateRect(hWnd,NULL,TRUE);
					}
				}else
				{
					char szBuf[200]="临时拼音模式不能切换！请先退出临时拼音";
					SendMessage(g_hWndMsg,TTM_SETTOOLINFO,0,(LPARAM)szBuf);
				}
				break;
			case IDM_QUERYCOMP:
				{
					char szBuf[MAX_INPUT]={0};
					int  nGet=0;
					if(lParam)
					{//按钮查询，剪贴板优先
						nGet=GetClipboardText(hWnd,szBuf,MAX_INPUT);
						if(!nGet) nGet=GetInputText(hWnd,szBuf,MAX_INPUT);
					}else
					{//快捷键查询，当前输入优先
						nGet=GetInputText(hWnd,szBuf,MAX_INPUT);
						if(!nGet) nGet=GetClipboardText(hWnd,szBuf,MAX_INPUT);
					}
					if(nGet)
					{
						char szRet[128+300+300+MAX_UDICTPHRASE]={0};//关键词+拼音+五笔+用户词典
						char *p=szRet;
						if(szBuf[0]&0x80)
						{//中文关键词，可以查询编码数据
							p+=sprintf(p,"关键词=%s",szBuf);
							if(ISComm_QueryComp(szBuf,nGet)==ISACK_SUCCESS)
							{
								PMSGDATA pData=ISComm_GetData();
								pData->byData[pData->sSize]=0;
								p+=sprintf(p,"\r\n%s=%s",ISComm_GetCompInfo()->szName,(char*)pData->byData);
							}else
							{
								p+=sprintf(p,"\r\n%s=查询失败",ISComm_GetCompInfo()->szName);
							}
							
							if(ISComm_SpellQueryComp(szBuf,nGet)==ISACK_SUCCESS)
							{
								PMSGDATA pData=ISComm_GetData();
								short i,sCount=0;
								BYTE *pbyData=pData->byData;
								p+=sprintf(p,"\r\n拼音=");
								memcpy(&sCount,pbyData,2);
								pbyData+=2;
								if(sCount>10) sCount=10;//只取前面10个拼音
								for(i=0;i<sCount;i++)
								{
									char cSize=pbyData[0];
									pbyData++;
									memcpy(p,pbyData,cSize);
									pbyData+=cSize;
									p+=cSize;
									*p++=' ';
								}
								*p=0;
							}else
							{
								p+=sprintf(p,"\r\n拼音=查询失败",szBuf);
							}
						}else
						{//查询英文单词
							p+=sprintf(p,"\r\n单词=%s",szBuf);
							if(ISComm_En2Ch(szBuf,nGet)==ISACK_SUCCESS)
							{
								PMSGDATA pData=ISComm_GetData();
								LPBYTE pbyData=pData->byData;
								BYTE i=0,byItems=*pbyData++;
								pbyData+=pbyData[0]+1;//skip phonetic
								p+=sprintf(p,"\r\n中文释意");
								while(i<byItems)
								{
									p+=sprintf(p,"\r\n\t%d:",i+1);
									strncpy(p,pbyData+1,pbyData[0]);p+=pbyData[0];pbyData+=pbyData[0]+1;
									i++;
								}
								*p=0;
							}else
							{
								p+=sprintf(p,"\r\n外文词库查询失败!");
							}
						}
						if(ISComm_QueryUserDict(szBuf,nGet)==ISACK_SUCCESS)
						{//查询用户词典成功
							PMSGDATA pData=ISComm_GetData();
							if(pData->sSize<1000+100)//关键词长度最大100，数据最大长度1000
							{
								char *pbuf=(char*)pData->byData;
								p+=sprintf(p,"\r\n用户词典[%s]:%s",ISComm_GetUserDict(),pbuf);
								pbuf+=strlen(pbuf)+1;
								p+=sprintf(p,"\r\n\t%s",pbuf);
								if(OpenClipboard(hWnd))
								{//将用户词典内容保存到剪贴板
									int nLen=strlen(pbuf);
									HGLOBAL hMem=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE ,nLen+1);
									LPSTR pMem=GlobalLock(hMem);
									strcpy(pMem,pbuf);
									GlobalUnlock(hMem);
									EmptyClipboard(); 
									SetClipboardData(CF_TEXT,hMem);
									CloseClipboard();
								}
							}
						}
						SendMessage(g_hWndMsg,TTM_SETTOOLINFO,0,(LPARAM)szRet);
					}else
					{
						SoundPlay("error");
					}
			}
			break;
			case IDM_HELP:
				{
					char path[MAX_PATH];
					sprintf(path,"%s\\%s.chm>main",g_szDataPath,g_szImeTitle);
					HtmlHelp(NULL,path,HH_DISPLAY_TOPIC,0);
				}
				break;
			case IDM_SPEED:
				{
					char szBuf[100]="还没有输入数据";
					if(g_dwTimeSpan>0) sprintf(szBuf,"输入汉字: %d 个\n打字速度:%d 字/分钟",g_dwInputCount,g_dwInputCount*60000/g_dwTimeSpan);
					MessageBox(GetActiveWindow(),szBuf,"打字速度",MB_OK|MB_ICONEXCLAMATION);
				}
				break;
			case IDM_HIDESTATUS:
				g_SettingsL.bHideStatus=!g_SettingsL.bHideStatus;
				if(!g_SettingsL.bHideStatus)
					ShowWindow(hWnd,SW_SHOWNOACTIVATE);
				else
					ShowWindow(hWnd,SW_HIDE);
				Config_SaveInt("HideStatus",g_SettingsL.bHideStatus);
				break;
			case IDM_MOUSEFOLLOW:
				{
					HWND hUIWnd=GetWindow(hWnd,GW_OWNER);
					HWND hInputWnd=UIWnd_GetInputWnd(hUIWnd);
					g_SettingsL.bMouseFollow=!g_SettingsL.bMouseFollow;
					Config_SaveInt("MouseFollow",g_SettingsL.bMouseFollow);
					if(!g_SettingsL.bMouseFollow)
					{
						HGLOBAL hUIPriv = (HGLOBAL)GetWindowLongPtr(hUIWnd, IMMGWLP_PRIVATE);
						LPUIPRIV lpUIPriv=(LPUIPRIV)GlobalLock(hUIPriv);
						SetWindowPos(hInputWnd,HWND_TOPMOST,lpUIPriv->ptInputWnd.x,lpUIPriv->ptInputWnd.y,0,0,SWP_NOSIZE|SWP_NOACTIVATE);
						GlobalUnlock(hUIPriv);
					}
				}
				break;
			case IDM_INPUTBIG5:
				g_SettingsL.bInputBig5=!g_SettingsL.bInputBig5;
				Config_SaveInt("InputBig5",g_SettingsL.bInputBig5);
				break;
			case IDM_SKIN_SET:
				{
					char *pSkin=(char*)lParam;
					if(!_stricmp(pSkin,g_szSkin)==0)
					{
						Skin_Free();
						if(Skin_Load(pSkin))
						{
							char szPath[MAX_PATH];
							strcpy(g_szSkin,pSkin);
							sprintf(szPath,"%s\\skins\\%s\\config.ini",g_szDataPath,g_szSkin);
							Skin_GetBtnRect(szPath,g_SettingsL.bFullStatus);
							UIWnd_UpdateCheckButton();
							Config_GetIni(szPath);
							WritePrivateProfileString("IME","skin",g_szSkin,szPath);
							PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SKINCHANGE);
							ISComm_SkinChange(GetWindow(hWnd,GW_OWNER));
						}else
						{
							Skin_Load(g_szSkin);
							UIWnd_UpdateCheckButton();
							MessageBox(GetActiveWindow(),"载入皮肤失败!","提示",MB_ICONERROR|MB_OK);
						}
					}
				}
				break;
			}
			if(uRet>=ID_SPCHAR_FIRST && uRet<=ID_SPCHAR_END)
			{//特殊符号
				g_SpCharData.nCurLayout=uRet-ID_SPCHAR_FIRST;
				g_SpCharData.iCurSel=-1;
				PostMessage(GetWindow(hWnd,GW_OWNER),WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SHOWSPCHAR);
			}else if(uRet>=ID_SKIN_BEGIN && uRet<=ID_SKIN_END)
			{//指定皮肤
				SendMessage(hWnd,WM_COMMAND,IDM_SKIN_SET,(LPARAM)s_szSkins[uRet-ID_SKIN_BEGIN]);
			}else if(uRet==ID_SKIN_VERTICAL)
			{//重要坚排
				HWND hWndInput=UIWnd_GetInputWnd(GetWindow(hWnd,GW_OWNER));
				g_SettingsG.bIptWndVertical=!g_SettingsG.bIptWndVertical;
				Config_SaveInt("InputWndVertical",g_SettingsG.bIptWndVertical);
				if(hWndInput) SendMessage(hWndInput,UM_CALCSIZE,0,0);
			}else if(uRet==ID_SKIN_RANDOM)
			{//随机选择皮肤
				char szPath[MAX_PATH];
				int iSkin=rand()%s_nSkins;
				SendMessage(hWnd,WM_COMMAND,IDM_SKIN_SET,(LPARAM)s_szSkins[iSkin]);
				Config_GetIni(szPath);
				WritePrivateProfileString("IME","skin",SKIN_RANDOM,szPath);
			}else if(uRet==ID_SKIN_MANAGER)
			{//皮肤管理器
				void SkinManager(HWND );
				SkinManager(hWnd);
			}else if(uRet==ID_SKIN_WEB)
			{
				char szConfig[MAX_PATH];
				char szURL[200]={0};
				ISComm_GetConfig(szConfig);
				GetPrivateProfileString("SKIN","webshare","http://soskin.ys168.com/",szURL,200,szConfig);
				ShellExecute(NULL,"open",szURL,NULL,NULL,SW_SHOWDEFAULT);
			}else if(uRet>=ID_BATCH_SETTINGS1 && uRet<=ID_BATCH_SETTINGS20)
			{//批量设置
				char szSettings[1<<15];//缓冲区,最大32K
				char szIniFile[MAX_PATH];
				sprintf(szIniFile,"%s\\Batch\\%s",g_szDataPath,s_szBatch[uRet-ID_BATCH_SETTINGS1]);
				if(GetPrivateProfileSection("IME",szSettings,1<<15,szIniFile))
				{
					char szConfig[MAX_PATH];
					char cPageCands=g_SettingsG.cPageCands;
					BYTE byTransStatus=g_SettingsG.byTransStatus;
					BYTE byTransInput=g_SettingsG.byTransInput;
					BOOL bBlur=GetPrivateProfileInt("BLUR","switch",TRUE,szIniFile);
					Config_GetIni(szConfig);
					Config_Save_Global(szConfig);
					Helper_WriteProfileSection("IME",szSettings,szConfig);
					Config_Load_Global(szConfig);
					
					if(byTransStatus!=g_SettingsG.byTransStatus || byTransInput!=g_SettingsG.byTransInput)//透明度发生更改，更新窗口透明度
						StatusWnd_UpdateTransDegree(hWnd);
					if(cPageCands!=g_SettingsG.cPageCands)						//重码数发生更改，更新输入窗口。
						StatusWnd_UpdateInputPageCands(hWnd);
					
					ISComm_Blur_Set(bBlur);
					if(GetPrivateProfileSection("BLUR_DATA",szSettings,1<<15,szIniFile))
					{
						char szSpell1[50],szSpell2[50];
						char *p=szSettings;
						ISComm_Blur_DelAll();
						while(p[0])
						{
							if(Helper_ParseIniLine(p,szSpell1,szSpell2))
							{
								ISComm_Blur_Add(szSpell1,szSpell2);
							}
							p+=strlen(p)+1;
						}
					}
				}
			}else if(uRet==ID_UDICT_INPUT)
			{//打开词典输入界面
				HIMC hIMC=(HIMC)GetWindowLongPtr(GetWindow(hWnd,GW_OWNER),IMMGWLP_IMC);
				if(hIMC)
				{
					HWND hImeWnd=NULL;
					LPINPUTCONTEXT lpIMC=ImmLockIMC(hIMC);
					hImeWnd=lpIMC->hWnd;
					ImmUnlockIMC(hIMC);
					UserDict_Input(hImeWnd);
				}
			}else if(uRet>=ID_UDICT_CLOSE && uRet<=ID_UDICT_20)
			{//选择用户词典
				if(uRet==ID_UDICT_CLOSE)
					ISComm_UserDict_Open("");
				else
					ISComm_UserDict_Open(s_szUDict[uRet-ID_UDICT_1]);
			}else if(uRet>=ID_COMP_FIRST && uRet<=ID_COMP_END)
			{
				ISComm_Comp_Open(s_szComp[uRet-ID_COMP_FIRST]);
			}else if(uRet==ID_BLEND_SP_WORD)
			{
				g_SettingsG.bBlendSpWord=!g_SettingsG.bBlendSpWord;
				Config_SaveInt("BlendSpWord",g_SettingsG.bBlendSpWord);
			}else if(uRet==ID_BLEND_SP_CE2)
			{
				BOOL bValid=0;
				ISComm_Bldsp_Get(&bValid,NULL,NULL);
				bValid=!bValid;
				ISComm_Bldsp_Set(BLDSP_CE2,bValid,0,0);
			}else if(uRet==ID_BLEND_SP_CE3)
			{
				BOOL bValid=0;
				ISComm_Bldsp_Get(NULL,&bValid,NULL);
				bValid=!bValid;
				ISComm_Bldsp_Set(BLDSP_CE3,0,bValid,0);				
			}else if(uRet==ID_BLEND_SP_CA4)
			{
				BOOL bValid=0;
				ISComm_Bldsp_Get(NULL,NULL,&bValid);
				bValid=!bValid;
				ISComm_Bldsp_Set(BLDSP_CA4,0,0,bValid);
			}else if(uRet==ID_BLEND_USERDEF)
			{
				g_SettingsG.bBlendUD=!g_SettingsG.bBlendUD;
				Config_SaveInt("BlendUD",g_SettingsG.bBlendUD);
			}else if(uRet>=IDM_SVR_PAGE1 && uRet<IDM_SVR_PAGE50)
			{
				LPCTSTR pszPages=ISComm_Svr_Pages();
				UINT uID=IDM_SVR_PAGE1;
				while(uID<uRet)
				{
					pszPages+=strlen(pszPages)+1;
					uID++;
				}
				ISComm_ShowServer(pszPages,strlen(pszPages));
			}
		}
		break;
	case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			if(pnmh->code==TTN_NEEDTEXT)
			{
				LPNMTTDISPINFO pttn=(LPNMTTDISPINFO)lParam;
				POINT pt;
				int nCmd;
				pttn->hinst=NULL;
				GetCursorPos(&pt);
				ScreenToClient(hWnd,&pt);
				nCmd=StatusWnd_CmdFromPt(pt);
				if(nCmd!=STATUSCMD_NODEF) sprintf(pttn->szText,"%d",nCmd);
			}
		}
		break;
	default:
		if(!Helper_IsIMEMessage(message))
			lRet=DefWindowProc(hWnd,message,wParam,lParam);
		break;
	}
	return lRet;
}

