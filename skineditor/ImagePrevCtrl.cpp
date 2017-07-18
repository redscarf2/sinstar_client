// ImagePrevCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ImagePrevCtrl.h"
#include "drawtransback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImagePrevCtrl

CImagePrevCtrl::CImagePrevCtrl()
{
	m_pImg=NULL;
	m_nBackMode=BM_TRANSGRID;
}

CImagePrevCtrl::~CImagePrevCtrl()
{
	if(m_pImg) Image_Free(m_pImg);
}


BEGIN_MESSAGE_MAP(CImagePrevCtrl, CStatic)
	//{{AFX_MSG_MAP(CImagePrevCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImagePrevCtrl message handlers

void CImagePrevCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	RECT rc;
	GetClientRect(&rc);
	if(m_pImg)
	{
		SIZE size=Image_GetSize(m_pImg);
		if(size.cx>rc.right) size.cx=rc.right;
		if(size.cy>rc.bottom) size.cy=rc.bottom;
		Image_Show(dc.GetSafeHdc(),(rc.right-size.cx)/2,(rc.bottom-size.cy)/2,size.cx,size.cy,m_pImg,0,0);
	}else
	{
		LOGFONT lf={0};
		strcpy(lf.lfFaceName,"宋体");
		lf.lfHeight=30;
		CFont font;
		font.CreateFontIndirect(&lf);
		CFont *pOldFont=dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);
		CString msg=m_strMsg.IsEmpty()?"图片资源显示":m_strMsg;
		dc.SetTextColor(RGB(255,255,255));
		dc.DrawText(msg,-1,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		OffsetRect(&rc,1,1);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(msg,-1,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		dc.SelectObject(pOldFont);
	}
}

BOOL CImagePrevCtrl::SetImage(LPCTSTR pszFileName)
{
	char szExt[15];
	_splitpath(pszFileName,NULL,NULL,NULL,szExt);
	if(m_pImg) Image_Free(m_pImg);
	m_pImg=Image_Load(pszFileName,stricmp(szExt,".bmp")==0?IMG_BMP:IMG_PNG);
	if(!m_pImg) m_strMsg="open image error";
	Invalidate(TRUE);
	return m_pImg!=NULL;
}

BOOL CImagePrevCtrl::OnEraseBkgnd(CDC* pDC) 
{
	RECT rc;
	GetClientRect(&rc);
	switch(m_nBackMode)
	{
	case BM_TRANSGRID:
		DrawTransBack(pDC,rc);
		break;
	case BM_WHITE:
		pDC->FillSolidRect(&rc,RGB(255,255,255));
		break;
	case BM_BLACK:
		pDC->FillSolidRect(&rc,RGB(0,0,0));
		break;
	}
	return TRUE;
}
