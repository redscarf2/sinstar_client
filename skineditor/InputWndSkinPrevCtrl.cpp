// InputWndSkinPrevCtrl.cpp: implementation of the CInputWndSkinPrevCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "skineditor.h"
#include "InputWndSkinPrevCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputWndSkinPrevCtrl::CInputWndSkinPrevCtrl()
{
	//shape
	m_objInfo[IWM_SHAPE].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SHAPE].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SHAPE].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SHAPE].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SHAPE].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SHAPE].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SHAPE].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SHAPE].obMargin[3].SetColor(RGB(255,0,0));

	m_objInfo[IWM_SHAPE].nObComp=2;
	m_objInfo[IWM_SHAPE].pObComp=new CSkinObject* [2];
	m_objInfo[IWM_SHAPE].pObComp[0]=new CSkinObjectText("code",CCID_NORMAL);
	m_objInfo[IWM_SHAPE].pObComp[1]=new CSkinObjectButton(SIID_SYMBOL);

	m_objInfo[IWM_SHAPE].nObCand=12;
	m_objInfo[IWM_SHAPE].pObCand=new CSkinObject* [12];
	m_objInfo[IWM_SHAPE].pObCand[0]=new CSkinObjectButton(SIID_BTNPREV);
	m_objInfo[IWM_SHAPE].pObCand[1]=new CSkinObjectText("1:",CCAID_INDEX);
	m_objInfo[IWM_SHAPE].pObCand[2]=new CSkinObjectText("重码",CCAID_NORMAL);
	m_objInfo[IWM_SHAPE].pObCand[3]=new CSkinObjectText("2:",CCAID_INDEX);
	m_objInfo[IWM_SHAPE].pObCand[4]=new CSkinObjectText("GBK重码",CCAID_INACTIVE);
	m_objInfo[IWM_SHAPE].pObCand[5]=new CSkinObjectText("3:",CCAID_INDEX);
	m_objInfo[IWM_SHAPE].pObCand[6]=new CSkinObjectText("预测",CCAID_FORECAST);
	m_objInfo[IWM_SHAPE].pObCand[7]=new CSkinObjectText("4:",CCAID_INDEX);
	m_objInfo[IWM_SHAPE].pObCand[8]=new CSkinObjectText("自定义",CCAID_USERDEF);
	m_objInfo[IWM_SHAPE].pObCand[9]=new CSkinObjectText("5:",CCAID_INDEX);
	m_objInfo[IWM_SHAPE].pObCand[10]=new CSkinObjectText("混拼",CCAID_MIXSP);
	m_objInfo[IWM_SHAPE].pObCand[11]=new CSkinObjectButton(SIID_BTNNEXT);

	//spell
	m_objInfo[IWM_SPELL].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SPELL].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SPELL].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SPELL].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SPELL].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SPELL].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SPELL].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SPELL].obMargin[3].SetColor(RGB(255,0,0));

	m_objInfo[IWM_SPELL].nObComp=5;
	m_objInfo[IWM_SPELL].pObComp=new CSkinObject* [5];
	m_objInfo[IWM_SPELL].pObComp[0]=new CSkinObjectText("t:",CCID_SPTMHEAD);
	m_objInfo[IWM_SPELL].pObComp[1]=new CSkinObjectText("拼",CCID_SPRESULT);
	m_objInfo[IWM_SPELL].pObComp[2]=new CSkinObjectText("音",CCID_SPEDIT);
	m_objInfo[IWM_SPELL].pObComp[3]=new CSkinObjectLine(CCID_SPLINE);	
	m_objInfo[IWM_SPELL].pObComp[4]=new CSkinObjectText("ying",CCID_SPSPELL);

	m_objInfo[IWM_SPELL].nObCand=12;
	m_objInfo[IWM_SPELL].pObCand=new CSkinObject* [12];
	m_objInfo[IWM_SPELL].pObCand[0]=new CSkinObjectButton(SIID_BTNPREV);
	m_objInfo[IWM_SPELL].pObCand[1]=new CSkinObjectText("1:",CCAID_INDEX);
	m_objInfo[IWM_SPELL].pObCand[2]=new CSkinObjectText("重码",CCAID_NORMAL);
	m_objInfo[IWM_SPELL].pObCand[3]=new CSkinObjectText("2:",CCAID_INDEX);
	m_objInfo[IWM_SPELL].pObCand[4]=new CSkinObjectText("GBK重码",CCAID_INACTIVE);
	m_objInfo[IWM_SPELL].pObCand[5]=new CSkinObjectText("3:",CCAID_INDEX);
	m_objInfo[IWM_SPELL].pObCand[6]=new CSkinObjectText("预测",CCAID_FORECAST);
	m_objInfo[IWM_SPELL].pObCand[7]=new CSkinObjectText("4:",CCAID_INDEX);
	m_objInfo[IWM_SPELL].pObCand[8]=new CSkinObjectText("自定义",CCAID_USERDEF);
	m_objInfo[IWM_SPELL].pObCand[9]=new CSkinObjectText("5:",CCAID_INDEX);
	m_objInfo[IWM_SPELL].pObCand[10]=new CSkinObjectText("混拼",CCAID_MIXSP);
	m_objInfo[IWM_SPELL].pObCand[11]=new CSkinObjectButton(SIID_BTNNEXT);

	//IWM_ASSOCIATE
	m_objInfo[IWM_ASSOCIATE].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ASSOCIATE].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ASSOCIATE].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ASSOCIATE].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ASSOCIATE].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ASSOCIATE].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ASSOCIATE].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ASSOCIATE].obMargin[3].SetColor(RGB(255,0,0));

	m_objInfo[IWM_ASSOCIATE].nObComp=2;
	m_objInfo[IWM_ASSOCIATE].pObComp=new CSkinObject* [2];
	m_objInfo[IWM_ASSOCIATE].pObComp[0]=new CSkinObjectText("启程",CCID_INPUT);
	m_objInfo[IWM_ASSOCIATE].pObComp[1]=new CSkinObjectText("语句联想测试",CCID_SENTINIT);
	m_objInfo[IWM_ASSOCIATE].nObCand=8;
	m_objInfo[IWM_ASSOCIATE].pObCand=new CSkinObject* [8];
	m_objInfo[IWM_ASSOCIATE].pObCand[0]=new CSkinObjectText("1:",CCAID_INDEX);
	m_objInfo[IWM_ASSOCIATE].pObCand[1]=new CSkinObjectText("软件",CCAID_NORMAL);
	m_objInfo[IWM_ASSOCIATE].pObCand[2]=new CSkinObjectText("2:",CCAID_INDEX);
	m_objInfo[IWM_ASSOCIATE].pObCand[3]=new CSkinObjectText("输入法",CCAID_NORMAL);
	m_objInfo[IWM_ASSOCIATE].pObCand[4]=new CSkinObjectText("3:",CCAID_INDEX);
	m_objInfo[IWM_ASSOCIATE].pObCand[5]=new CSkinObjectText("输入之星",CCAID_NORMAL);
	m_objInfo[IWM_ASSOCIATE].pObCand[6]=new CSkinObjectText("4:",CCAID_INDEX);
	m_objInfo[IWM_ASSOCIATE].pObCand[7]=new CSkinObjectText("五笔",CCAID_NORMAL);
	
	//IWM_SENT
	m_objInfo[IWM_SENT].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SENT].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SENT].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SENT].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_SENT].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SENT].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SENT].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SENT].obMargin[3].SetColor(RGB(255,0,0));
	m_objInfo[IWM_SENT].nObComp=3;
	m_objInfo[IWM_SENT].pObComp=new CSkinObject* [3];
	m_objInfo[IWM_SENT].pObComp[0]=new CSkinObjectText("启程",CCID_INPUT);
	m_objInfo[IWM_SENT].pObComp[1]=new CSkinObjectText("语句",CCID_SENTSEL);
	m_objInfo[IWM_SENT].pObComp[2]=new CSkinObjectText("联想测试",CCID_SENTNORMAL);
	m_objInfo[IWM_SENT].nObCand=1;
	m_objInfo[IWM_SENT].pObCand=new CSkinObject* [1];
	m_objInfo[IWM_SENT].pObCand[0]=new CSkinObjectText("语句联想模式",CCAID_TIP);
	
	//IWM_ENGLISH
	m_objInfo[IWM_ENGLISH].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ENGLISH].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ENGLISH].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ENGLISH].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_ENGLISH].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ENGLISH].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ENGLISH].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ENGLISH].obMargin[3].SetColor(RGB(255,0,0));
	m_objInfo[IWM_ENGLISH].nObComp=3;
	m_objInfo[IWM_ENGLISH].pObComp=new CSkinObject* [3];
	m_objInfo[IWM_ENGLISH].pObComp[0]=new CSkinObjectText("tes",CCID_ENCODE);
	m_objInfo[IWM_ENGLISH].pObComp[1]=new CSkinObjectText("t",CCID_ENMAKEUP);
	m_objInfo[IWM_ENGLISH].pObComp[2]=new CSkinObjectText("[test]",CCID_ENPHONETIC);
	m_objInfo[IWM_ENGLISH].nObCand=6;
	m_objInfo[IWM_ENGLISH].pObCand=new CSkinObject* [6];
	m_objInfo[IWM_ENGLISH].pObCand[0]=new CSkinObjectText("1:",CCAID_INDEX);
	m_objInfo[IWM_ENGLISH].pObCand[1]=new CSkinObjectText("测试",CCAID_NORMAL);
	m_objInfo[IWM_ENGLISH].pObCand[2]=new CSkinObjectText("2:",CCAID_INDEX);
	m_objInfo[IWM_ENGLISH].pObCand[3]=new CSkinObjectText("测验",CCAID_NORMAL);
	m_objInfo[IWM_ENGLISH].pObCand[4]=new CSkinObjectText("3:",CCAID_INDEX);
	m_objInfo[IWM_ENGLISH].pObCand[5]=new CSkinObjectText("检验",CCAID_NORMAL);
	
	//IWM_USERDEF
	m_objInfo[IWM_USERDEF].obMargin[0].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_USERDEF].obMargin[1].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_USERDEF].obMargin[2].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_USERDEF].obMargin[3].SetPenStyle(PS_DASHDOT);
	m_objInfo[IWM_USERDEF].obMargin[0].SetColor(RGB(255,0,0));
	m_objInfo[IWM_USERDEF].obMargin[1].SetColor(RGB(255,0,0));
	m_objInfo[IWM_USERDEF].obMargin[2].SetColor(RGB(255,0,0));
	m_objInfo[IWM_USERDEF].obMargin[3].SetColor(RGB(255,0,0));
	m_objInfo[IWM_USERDEF].nObComp=3;
	m_objInfo[IWM_USERDEF].pObComp=new CSkinObject* [3];
	m_objInfo[IWM_USERDEF].pObComp[0]=new CSkinObjectText("u:",CCID_UDHEAD);
	m_objInfo[IWM_USERDEF].pObComp[1]=new CSkinObjectText("mai",CCID_UDCODE);
	m_objInfo[IWM_USERDEF].pObComp[2]=new CSkinObjectText("l",CCID_UDMAKEUP);
	m_objInfo[IWM_USERDEF].nObCand=2;
	m_objInfo[IWM_USERDEF].pObCand=new CSkinObject* [2];
	m_objInfo[IWM_USERDEF].pObCand[0]=new CSkinObjectText("1:",CCAID_INDEX);
	m_objInfo[IWM_USERDEF].pObCand[1]=new CSkinObjectText("setoutsoft@qq.com",CCAID_NORMAL);

}

CInputWndSkinPrevCtrl::~CInputWndSkinPrevCtrl()
{
	FreeObjects();
}



void CInputWndSkinPrevCtrl::InitObjects(LPCTSTR pszPath,SKININPUT *pSkinInput,COLORCOMP *pColorComp,COLORCAND *pColorCand)
{
	SetFontInfo(&pSkinInput->lf);
	m_bgImgInfo.nLeft=pSkinInput->loHorz.nLeft;
	m_bgImgInfo.nRight=pSkinInput->loHorz.nRight;
	m_bgImgInfo.bTile=pSkinInput->loHorz.bTile;
	m_bgImgInfo.nCandLeft=pSkinInput->loHorz.nCandLeft;
	m_bgImgInfo.nCandRight=pSkinInput->loHorz.nCandRight;
	m_bgImgInfo.nCompLeft=pSkinInput->loHorz.nCompLeft;
	m_bgImgInfo.nCompRight=pSkinInput->loHorz.nCompRight;
	m_bgImgInfo.rcMargin=pSkinInput->loHorz.rcMargin;

	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->loHorz.szBgImg);
	SetBgImage(szPath);

	CDC *pDC=GetDC();
	CFont font;
	font.CreateFontIndirect(&m_lf);
	CFont *pOldFont=pDC->SelectObject(&font);

	SIZE size;
	SIZE sizeBg=Image_GetSize(GetImage());
	int nFontHei=pDC->GetTextExtent("A").cy;
	CSkinObjectText *pText=NULL;
	CSkinObjectButton *pButton=NULL;
	CSkinObjectLine	 *pLine=NULL;

	int nRoomHei=sizeBg.cy-m_bgImgInfo.rcMargin.top-m_bgImgInfo.rcMargin.bottom;
	int nTopTmp;
	//普通形码状态
	int nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	int nOffset=m_bgImgInfo.rcMargin.left+m_bgImgInfo.nCompLeft;
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObComp[0];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	pText->SetColor(pColorComp->crNormal);
	m_objInfo[IWM_SHAPE].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;

	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	
	pButton=(CSkinObjectButton*)m_objInfo[IWM_SHAPE].pObCand[0];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->szBtnPrev);
	pButton->SetImage(szPath);
	size=pButton->GetSize();
	nTopTmp=m_bgImgInfo.rcMargin.top+(nRoomHei/2-size.cy)/2+nRoomHei/2;
	pButton->SetRect(CRect(CPoint(nOffset,nTopTmp),size));
	nOffset+=size.cx;

	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[1];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[2];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[3];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[4];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crInactive);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[5];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[6];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crForecast);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[7];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[8];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crUserDef);

	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[9];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SHAPE].pObCand[10];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crMixSp);


	pButton=(CSkinObjectButton*)m_objInfo[IWM_SHAPE].pObCand[11];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->szBtnNext);
	pButton->SetImage(szPath);
	size=pButton->GetSize();
	nTopTmp=m_bgImgInfo.rcMargin.top+(nRoomHei/2-size.cy)/2+nRoomHei/2;
	pButton->SetRect(CRect(CPoint(nOffset,nTopTmp),size));
	nOffset+=size.cx;

	m_objInfo[IWM_SHAPE].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;

	pButton=(CSkinObjectButton*)m_objInfo[IWM_SHAPE].pObComp[1];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->szSymbolMode);
	pButton->SetImage(szPath,2);
	size=pButton->GetSize();
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-size.cy)/2;
	pButton->SetRect(CRect(CPoint(nOffset-size.cx,nTop),size));

	size=GetModeSize(IWM_SHAPE);
	m_objInfo[IWM_SHAPE].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_SHAPE].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_SHAPE].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_SHAPE].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------

	/////////////////////////init spell objects//////////////////////////////////
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompLeft;

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObComp[0];
	pText->SetColor(pColorComp->crSpTmHead);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObComp[1];
	pText->SetColor(pColorComp->crSpResult);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObComp[2];
	pText->SetColor(pColorComp->crSpEdit);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pLine=(CSkinObjectLine*)m_objInfo[IWM_SPELL].pObComp[3];
	pLine->SetColor(pColorComp->crSpLine);
	pLine->SetPoints(CPoint(nOffset-size.cx,nTop+size.cy),CPoint(nOffset,nTop+size.cy));
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObComp[4];
	pText->SetColor(pColorComp->crSpSpell);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_SPELL].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;
	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	
	pButton=(CSkinObjectButton*)m_objInfo[IWM_SPELL].pObCand[0];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->szBtnPrev);
	pButton->SetImage(szPath);
	size=pButton->GetSize();
	nTopTmp=m_bgImgInfo.rcMargin.top+(nRoomHei/2-size.cy)/2+nRoomHei/2;
	pButton->SetRect(CRect(CPoint(nOffset,nTopTmp),size));
	nOffset+=size.cx;

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[1];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[2];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[3];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[4];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crInactive);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[5];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[6];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crForecast);
	nOffset+=pSkinInput->loHorz.nCandInter;

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[7];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[8];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crUserDef);

	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[9];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_SPELL].pObCand[10];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crMixSp);

	pButton=(CSkinObjectButton*)m_objInfo[IWM_SPELL].pObCand[11];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->szBtnNext);
	pButton->SetImage(szPath);
	size=pButton->GetSize();
	nTopTmp=m_bgImgInfo.rcMargin.top+(nRoomHei/2-size.cy)/2+nRoomHei/2;
	pButton->SetRect(CRect(CPoint(nOffset,nTopTmp),size));
	nOffset+=size.cx;

	m_objInfo[IWM_SPELL].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;

	size=GetModeSize(IWM_SPELL);
	m_objInfo[IWM_SPELL].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_SPELL].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_SPELL].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_SPELL].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------
	///////////////IWM_ASSOCIATE///////////////////////////////
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompLeft;
	
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObComp[0];
	pText->SetColor(pColorComp->crInput);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObComp[1];
	pText->SetColor(pColorComp->crSentInit);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_ASSOCIATE].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;
	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[0];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[1];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[2];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[3];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[4];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[5];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[6];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ASSOCIATE].pObCand[7];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	m_objInfo[IWM_ASSOCIATE].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;

	size=GetModeSize(IWM_ASSOCIATE);
	m_objInfo[IWM_ASSOCIATE].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_ASSOCIATE].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_ASSOCIATE].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_ASSOCIATE].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------
	/////////////IWM_SENT//////////////////////////////////
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompLeft;
	
	pText=(CSkinObjectText*)m_objInfo[IWM_SENT].pObComp[0];
	pText->SetColor(pColorComp->crInput);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_SENT].pObComp[1];
	pText->SetColor(pColorComp->crSentSel);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_SENT].pObComp[2];
	pText->SetColor(pColorComp->crSentNormal);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_SENT].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;
	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	pText=(CSkinObjectText*)m_objInfo[IWM_SENT].pObCand[0];
	pText->SetColor(pColorCand->crTip);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_SENT].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;
	
	size=GetModeSize(IWM_SENT);
	m_objInfo[IWM_SENT].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_SENT].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_SENT].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_SENT].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------
	///////////////////////IWM_ENGLISH///////////////////////////////
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompLeft;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObComp[0];
	pText->SetColor(pColorComp->crEnCode);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObComp[1];
	pText->SetColor(pColorComp->crEnMakeup);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObComp[2];
	pText->SetColor(pColorComp->crEnPhonetic);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_ENGLISH].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;
	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[0];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[1];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[2];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[3];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);
	nOffset+=pSkinInput->loHorz.nCandInter;
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[4];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crIndex);
	pText=(CSkinObjectText*)m_objInfo[IWM_ENGLISH].pObCand[5];
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText->SetColor(pColorCand->crNormal);

	m_objInfo[IWM_ENGLISH].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;

	size=GetModeSize(IWM_ENGLISH);
	m_objInfo[IWM_ENGLISH].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_ENGLISH].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_ENGLISH].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_ENGLISH].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------
	////////////////////////IWM_USERDEF////////////////////////////
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompLeft;
	
	pText=(CSkinObjectText*)m_objInfo[IWM_USERDEF].pObComp[0];
	pText->SetColor(pColorComp->crUdHead);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_USERDEF].pObComp[1];
	pText->SetColor(pColorComp->crUdCode);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_USERDEF].pObComp[2];
	pText->SetColor(pColorComp->crUdMakeup);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_USERDEF].nCompWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCompRight;
	//cand
	nTop=m_bgImgInfo.rcMargin.top+(nRoomHei/2-nFontHei)/2+nRoomHei/2;
	nOffset=m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandLeft;
	pText=(CSkinObjectText*)m_objInfo[IWM_USERDEF].pObCand[0];
	pText->SetColor(pColorCand->crIndex);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	pText=(CSkinObjectText*)m_objInfo[IWM_USERDEF].pObCand[1];
	pText->SetColor(pColorCand->crNormal);
	size=pDC->GetTextExtent(pText->GetDescript());
	pText->SetRect(CRect(CPoint(nOffset,nTop),size));
	nOffset+=size.cx;
	m_objInfo[IWM_USERDEF].nCandWid=nOffset-m_bgImgInfo.rcMargin.left+pSkinInput->loHorz.nCandRight;

	size=GetModeSize(IWM_USERDEF);
	m_objInfo[IWM_USERDEF].obMargin[0].SetPoints(CPoint(0,m_bgImgInfo.rcMargin.top),CPoint(size.cx,m_bgImgInfo.rcMargin.top));		// -------
	m_objInfo[IWM_USERDEF].obMargin[1].SetPoints(CPoint(m_bgImgInfo.rcMargin.left,0),CPoint(m_bgImgInfo.rcMargin.left,size.cy));	// |
	m_objInfo[IWM_USERDEF].obMargin[2].SetPoints(CPoint(size.cx-m_bgImgInfo.rcMargin.right,0),CPoint(size.cx-m_bgImgInfo.rcMargin.right,size.cy));	// |
	m_objInfo[IWM_USERDEF].obMargin[3].SetPoints(CPoint(0,size.cy-m_bgImgInfo.rcMargin.bottom),CPoint(size.cx,size.cy-m_bgImgInfo.rcMargin.bottom));		// -------

	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
}

void CInputWndSkinPrevCtrl::OnDrawBkImage(CDC *pDC,POINT ptOffset)
{
	if(!m_pBgImage) return;
	SIZE size1=Image_GetSize(m_pBgImage);
	SIZE size=GetSize();
	if(size1.cx!=size.cx)
	{
		int x=0;
		Image_Show(pDC->m_hDC,ptOffset.x+x,ptOffset.y,m_bgImgInfo.nLeft,size.cy,m_pBgImage,0,0);
		x+=m_bgImgInfo.nLeft;

		int nSeg=size1.cx-(m_bgImgInfo.nLeft+m_bgImgInfo.nRight);
		int nRight=size.cx-m_bgImgInfo.nRight;
		if(m_bgImgInfo.bTile)
		{
			while(x<nRight)
			{
				int nWid=nSeg;
				if(x+nWid>nRight) nWid=nRight-x;
				Image_Show(pDC->m_hDC,x+ptOffset.x,ptOffset.y,nWid,size.cy,m_pBgImage,m_bgImgInfo.nLeft,0);
				x+=nWid;
			}
		}else
		{
			HDC hmemdc=CreateCompatibleDC(pDC->m_hDC);
			HGDIOBJ hOldBmp=SelectObject(hmemdc,m_pBgImage->hBmp);
			if(m_pBgImage->imgType==IMG_BMP)
			{//bmp
				pDC->SetStretchBltMode(COLORONCOLOR);
				StretchBlt(pDC->m_hDC,ptOffset.x+x,ptOffset.y,nRight-x,size.cy,hmemdc,x,0,nSeg,size.cy,SRCCOPY);
			}else
			{//PNG
				BLENDFUNCTION bf={AC_SRC_OVER,0,255,1};
				AlphaBlend(pDC->m_hDC,ptOffset.x+x,ptOffset.y,nRight-x,size.cy,hmemdc,x,0,nSeg,size.cy,bf);
			}
			SelectObject(hmemdc,hOldBmp);
			DeleteDC(hmemdc);
			x=nRight;
		}
		Image_Show(pDC->m_hDC,x+ptOffset.x,ptOffset.y,m_bgImgInfo.nRight,size.cy,m_pBgImage,size1.cx-m_bgImgInfo.nRight,0);
	}else
	{
		CSkinPrevCtrl::OnDrawBkImage(pDC,ptOffset);
	}
}

void CInputWndSkinPrevCtrl::SetMode(IWM iwm)
{
	m_iwm=iwm;
	m_obList.RemoveAll();
    int i;
	for(i=0;i<m_objInfo[iwm].nObComp;i++)
	{
		AddObject(m_objInfo[iwm].pObComp[i]);
	}
	for( i=0;i<m_objInfo[iwm].nObCand;i++)
	{
		AddObject(m_objInfo[iwm].pObCand[i]);
	}
	for( i=0;i<4;i++)
	{
		AddObject(m_objInfo[iwm].obMargin+i);
	}
	Invalidate();
}

SIZE CInputWndSkinPrevCtrl::GetSize()
{
	return GetModeSize(m_iwm);
}

SIZE CInputWndSkinPrevCtrl::GetModeSize(IWM iwm)
{
	SIZE size=CSkinPrevCtrl::GetSize();
	int nWid1=m_objInfo[iwm].nCompWid+m_bgImgInfo.rcMargin.left+m_bgImgInfo.rcMargin.right;
	int nWid2=m_objInfo[iwm].nCandWid+m_bgImgInfo.rcMargin.left+m_bgImgInfo.rcMargin.right;
	int nWid=nWid1>nWid2?nWid1:nWid2;
	if(size.cx<nWid) size.cx=nWid;
	return size;
}

void CInputWndSkinPrevCtrl::FreeObjects()
{
	for(int i=IWM_SHAPE;i<=IWM_USERDEF;i++)
	{
        int j;
		for(j=0;j<m_objInfo[i].nObComp;j++)
		{
			delete m_objInfo[i].pObComp[j];
		}
		delete []m_objInfo[i].pObComp;
		for(j=0;j<m_objInfo[i].nObCand;j++)
		{
			delete m_objInfo[i].pObCand[j];
		}
		delete []m_objInfo[i].pObCand;
	}
}

void CInputWndSkinPrevCtrl::SetTile(BOOL bTile)
{
	m_bgImgInfo.bTile=bTile;
	Invalidate();
}

void CInputWndVertSkinPrevCtrl::InitObjects(LPCTSTR pszPath,SKININPUT *pSkinInput,COLORCOMP *pColorComp,COLORCAND *pColorCand)
{
	SetFontInfo(&pSkinInput->lf);
	m_bgImgInfo.nLeft=pSkinInput->loVert.nTop;
	m_bgImgInfo.nRight=pSkinInput->loVert.nBottom;
	m_bgImgInfo.bTile=pSkinInput->loVert.bTile;
	m_bgImgInfo.nCandLeft=pSkinInput->loVert.nCandLeft;
	m_bgImgInfo.nCandRight=pSkinInput->loVert.nCandRight;
	m_bgImgInfo.nCompLeft=pSkinInput->loVert.nCompLeft;
	m_bgImgInfo.nCompRight=pSkinInput->loVert.nCompRight;
	m_bgImgInfo.rcMargin=pSkinInput->loVert.rcMargin;

	char szPath[MAX_PATH];
	sprintf(szPath,"%s\\%s",pszPath,pSkinInput->loVert.szBgImg);
	SetBgImage(szPath);

	RemoveObjects();

	CDC *pDC=GetDC();
	CFont font;
	font.CreateFontIndirect(&m_lf);
	CFont *pOldFont=pDC->SelectObject(&font);
	
	SIZE sizeBg=GetSize();
	int nFontHei=pDC->GetTextExtent("A").cy;
	POINT pt1,pt2;
	//margin line
	//top line
	pt1.x=0,pt2.x=sizeBg.cx; 
	pt2.y=pt1.y=m_bgImgInfo.rcMargin.top;
	m_soLMargin[0].SetPoints(pt1,pt2);
	m_soLMargin[0].SetPenStyle(PS_DASHDOT);
	m_soLMargin[0].SetColor(RGB(255,0,0));
	AddObject(m_soLMargin+0);
	//bottom line
	pt2.y=pt1.y=sizeBg.cy-m_bgImgInfo.rcMargin.bottom;
	m_soLMargin[1].SetPoints(pt1,pt2);
	m_soLMargin[1].SetPenStyle(PS_DASHDOT);
	m_soLMargin[1].SetColor(RGB(255,0,0));
	AddObject(m_soLMargin+1);
	//left line
	pt1.x=pt2.x=m_bgImgInfo.rcMargin.left;
	pt1.y=0,pt2.y=sizeBg.cy;
	m_soLMargin[2].SetPoints(pt1,pt2);
	m_soLMargin[2].SetPenStyle(PS_DASHDOT);
	m_soLMargin[2].SetColor(RGB(255,0,0));
	AddObject(m_soLMargin+2);
	//right line
	pt1.x=pt2.x=sizeBg.cx-m_bgImgInfo.rcMargin.right;
	m_soLMargin[3].SetPoints(pt1,pt2);
	m_soLMargin[3].SetPenStyle(PS_DASHDOT);
	m_soLMargin[3].SetColor(RGB(255,0,0));
	AddObject(m_soLMargin+3);
	//cut line
// 	pt1.x=0,pt2.x=sizeBg.cx;
// 	pt1.y=pt2.y=m_bgImgInfo.nLeft;
// 	m_soLCut[0].SetPoints(pt1,pt2);
// 	m_soLCut[0].SetPenStyle(PS_DASHDOT);
// 	m_soLCut[0].SetColor(RGB(255,0,0));
// 	AddObject(m_soLCut+0);
// 	pt1.y=pt2.y=m_bgImgInfo.nRight;
// 	m_soLCut[1].SetPoints(pt1,pt2);
// 	m_soLCut[1].SetPenStyle(PS_DASHDOT);
// 	m_soLCut[1].SetColor(RGB(255,0,0));
// 	AddObject(m_soLCut+1);
	//comp
	m_sotComp.SetText("comp");
	m_sotComp.SetColor(pColorComp->crNormal);
	pt1.x=pSkinInput->loVert.rcMargin.left+pSkinInput->loVert.nCompLeft;
	pt1.y=pSkinInput->loVert.rcMargin.top;
	pt2.x=pt1.x+pDC->GetTextExtent("comp",4).cx;
	pt2.y=pt1.y+pSkinInput->loVert.nCompHei;
	m_sotComp.SetRect(CRect(pt1,pt2));
	AddObject(&m_sotComp);
	//cand
	m_sotCand[0].SetText("1. 重码");
	m_sotCand[0].SetColor(pColorCand->crNormal);
	pt1.x=pSkinInput->loVert.rcMargin.left+pSkinInput->loVert.nCandLeft;
	pt1.y=pt2.y+pSkinInput->loVert.nLineHei;
	pt2.x=pt1.x+pDC->GetTextExtent("1: 重码",7).cx;
	pt2.y=pt1.y+nFontHei;
	m_sotCand[0].SetRect(CRect(pt1,pt2));
	AddObject(m_sotCand);
	m_sotCand[1].SetText("2. 重码");
	m_sotCand[1].SetColor(pColorCand->crNormal);
	pt1.y=pt2.y+pSkinInput->loVert.nCandInter;
	pt2.x=pt1.x+pDC->GetTextExtent("2: 重码",7).cx;
	pt2.y=pt1.y+nFontHei;
	m_sotCand[1].SetRect(CRect(pt1,pt2));
	AddObject(m_sotCand+1);

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
	ReleaseDC(pDC);
}

void CInputWndVertSkinPrevCtrl::SetTile(BOOL bTile)
{
	m_bgImgInfo.bTile=bTile;
	Invalidate();
}

void CInputWndVertSkinPrevCtrl::OnDrawBkImage(CDC *pDC,POINT ptOffset)
{
	if(!m_pBgImage) return;
	SIZE size1=Image_GetSize(m_pBgImage);
	SIZE size=GetSize();
	if(size1.cy!=size.cy)
	{
		int y=0;
		Image_Show(pDC->m_hDC,ptOffset.x+y,ptOffset.y,size.cx,m_bgImgInfo.nLeft,m_pBgImage,0,0);
		y+=m_bgImgInfo.nLeft;
		
		int nSeg=size1.cy-(m_bgImgInfo.nLeft+m_bgImgInfo.nRight);
		int nBottom=size.cy-m_bgImgInfo.nRight;
		if(m_bgImgInfo.bTile)
		{//tile
			while(y<nBottom)
			{
				int nHei=nSeg;
				if(y+nHei>nBottom) nHei=nBottom-y;
				Image_Show(pDC->m_hDC,ptOffset.x,y+ptOffset.y,size.cx,nHei,m_pBgImage,0,m_bgImgInfo.nLeft);
				y+=nHei;
			}
		}else
		{//Stretch
			HDC hmemdc=CreateCompatibleDC(pDC->m_hDC);
			HGDIOBJ hOldBmp=SelectObject(hmemdc,m_pBgImage->hBmp);
			if(m_pBgImage->imgType==IMG_BMP)
			{//bmp
				pDC->SetStretchBltMode(COLORONCOLOR);
				StretchBlt(pDC->m_hDC,ptOffset.x,ptOffset.y+y,size.cx,nBottom-y,hmemdc,0,y,size.cx,nSeg,SRCCOPY);
			}else
			{//PNG
				BLENDFUNCTION bf={AC_SRC_OVER,0,255,1};
				AlphaBlend(pDC->m_hDC,ptOffset.x,ptOffset.y+y,size.cx,nBottom-y,hmemdc,0,y,size.cx,nSeg,bf);
			}
			SelectObject(hmemdc,hOldBmp);
			DeleteDC(hmemdc);
			y=nBottom;
		}
		Image_Show(pDC->m_hDC,ptOffset.x,y+ptOffset.y,size.cx,m_bgImgInfo.nRight,m_pBgImage,0,size1.cy-m_bgImgInfo.nRight);
	}else
	{
		CSkinPrevCtrl::OnDrawBkImage(pDC,ptOffset);
	}
}
