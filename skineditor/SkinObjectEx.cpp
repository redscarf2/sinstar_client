// SkinObjectEx.cpp: implementation of the CSkinObjectEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkinObjectEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinObjectButton::CSkinObjectButton(DWORD dwID)
{
	SetID(dwID);
	m_pImg=NULL;
	m_nImages=0;
}

CSkinObjectButton::~CSkinObjectButton()
{
	if(m_pImg) Image_Free(m_pImg);
}

void CSkinObjectButton::SetImage(LPCTSTR pszImageFile,int nImages)
{
	char szExt[15];
	_splitpath(pszImageFile,NULL,NULL,NULL,szExt);
	if(m_pImg) Image_Free(m_pImg);
	m_pImg=NULL;
	m_pImg=Image_Load(pszImageFile,stricmp(szExt,".bmp")==0?IMG_BMP:IMG_PNG);
	m_nImages=nImages;
	SetDescprit(pszImageFile);
}


SIZE CSkinObjectButton::GetSize()
{
	SIZE size={0};
	if(!m_pImg) return size;
	size=Image_GetSize(m_pImg);
	size.cx/=m_nImages;
	return size;
}

void CSkinObjectButton::OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)
{
	if(m_pImg)
	{
		Image_Show(pDC->m_hDC,m_rect.left+ptOffset.x,m_rect.top+ptOffset.y,m_rect.right-m_rect.left,m_rect.bottom-m_rect.top,m_pImg,0,0);
	}
}

//////////////////////////////////////////////////////////////////////
void CSkinObjectText::SetText(LPCTSTR pszText)
{
	SetDescprit(pszText);
}

void CSkinObjectText::SetColor(COLORREF cr)
{
	m_cr=cr;
}

void CSkinObjectText::SetFont(LPCTSTR lfFaceName,int lfHeight)
{
	strcpy(m_lf.lfFaceName,lfFaceName);
	m_lf.lfHeight=lfHeight;
}

void CSkinObjectText::OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)
{
	pDC->SetTextColor(m_cr);
	pDC->TextOut(ptOffset.x+m_rect.left,ptOffset.y+m_rect.top,GetDescript());
}


////////////////////////////////////////////////////////////////////////
