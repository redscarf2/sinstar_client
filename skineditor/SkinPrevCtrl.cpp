// SkinPrevCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SkinPrevCtrl.h"
#include "drawtransback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinPrevCtrl

CSkinPrevCtrl::CSkinPrevCtrl(BOOL bObjOwner)
{
	m_pBgImage=NULL;
	memset(&m_lf,0,sizeof(m_lf));
	strcpy(m_lf.lfFaceName,"ËÎÌו");
	m_lf.lfHeight=-12;
	m_bObjOwner=bObjOwner;
}

CSkinPrevCtrl::~CSkinPrevCtrl()
{
	if(m_pBgImage) Image_Free(m_pBgImage);
	RemoveObjects();
}


BEGIN_MESSAGE_MAP(CSkinPrevCtrl, CStatic)
	//{{AFX_MSG_MAP(CSkinPrevCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinPrevCtrl message handlers

void CSkinPrevCtrl::SetBgImage(LPCTSTR pszImage)
{
	char szExt[15];
	_splitpath(pszImage,NULL,NULL,NULL,szExt);

	if(m_pBgImage) Image_Free(m_pBgImage);
	m_pBgImage=NULL;
	m_pBgImage=Image_Load(pszImage,stricmp(szExt,".bmp")==0?IMG_BMP:IMG_PNG);

	Invalidate();
}

BOOL CSkinPrevCtrl::OnEraseBkgnd(CDC* pDC) 
{
	RECT rc;
	GetClientRect(&rc);
	DrawTransBack(pDC,rc);
	return TRUE;
}

void CSkinPrevCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	POINT pt=GetOffset();
	dc.SetBkMode(TRANSPARENT);
	CFont font;
	font.CreateFontIndirect(&m_lf);
	CFont *pOldFont=dc.SelectObject(&font);
	OnDrawBkImage(&dc,pt);
	POSITION pos=m_obList.GetHeadPosition();
	while(pos)
	{
		CSkinObject *pSkinObj=m_obList.GetNext(pos);
		pSkinObj->OnDraw(&dc,pt,0);
	}
	dc.SelectObject(pOldFont);
}

void CSkinPrevCtrl::OnDrawBkImage(CDC *pDC,POINT ptOffset)
{
	if(m_pBgImage)
	{
		SIZE size=Image_GetSize(m_pBgImage);
		Image_Show(pDC->GetSafeHdc(),ptOffset.x,ptOffset.y,size.cx,size.cy,m_pBgImage,0,0);
	}
}

POINT CSkinPrevCtrl::GetOffset()
{
	CRect rc;
	GetClientRect(&rc);
	POINT pt={0,0};
	SIZE size=GetSize();
	pt.x=(rc.right-size.cx)/2;
	pt.y=(rc.bottom-size.cy)/2;
	return pt;
}

SIZE CSkinPrevCtrl::GetSize()
{
	SIZE size={0};
	if(m_pBgImage) size=Image_GetSize(m_pBgImage);
	return size;
}

void CSkinPrevCtrl::AddObject(CSkinObject *pObj,BOOL bUpdate)
{
	m_obList.AddTail(pObj);
	if(bUpdate) Invalidate(TRUE);
}

void CSkinPrevCtrl::SetFontInfo(LOGFONT *plf)
{
	memcpy(&m_lf,plf,sizeof(LOGFONT));
}	

CSkinObject * CSkinPrevCtrl::HitTest(POINT pt)
{
	POSITION pos=m_obList.GetTailPosition();
	POINT ptOffset=GetOffset();
	pt.x-=ptOffset.x;
	pt.y-=ptOffset.y;
	while(pos)
	{
		CSkinObject *pObj=m_obList.GetPrev(pos);
		if(PtInRect(&pObj->GetRect(),pt))
		{
			return pObj;
		}
	}
	return NULL;
}

void CSkinPrevCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSkinObject *pObj=HitTest(point);
	if(pObj) GetParent()->SendMessage(UM_SKINOBJCLICKED,GetDlgCtrlID(),pObj->GetID());
}

void CSkinPrevCtrl::RemoveObjects()
{
	if(m_bObjOwner)
	{
		POSITION pos=m_obList.GetHeadPosition();
		while(pos)
		{
			CSkinObject *pObj=(CSkinObject*)m_obList.GetNext(pos);
			delete pObj;
		}
	}
	m_obList.RemoveAll();
}
