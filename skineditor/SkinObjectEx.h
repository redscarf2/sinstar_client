// SkinObjectEx.h: interface for the CSkinObjectEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINOBJECTEX_H__64CB34C0_3579_4B63_8D09_7DA7CEFC9FF8__INCLUDED_)
#define AFX_SKINOBJECTEX_H__64CB34C0_3579_4B63_8D09_7DA7CEFC9FF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SkinObject.h"
#include "image.h"
#include "skindef.h"

class CSkinObjectButton : public CSkinObject  
{
public:
	CSkinObjectButton(DWORD dwID);
	virtual ~CSkinObjectButton();
	void SetImage(LPCTSTR pszImageFile,int nImages=3);
	SIZE GetSize();

	void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam);
private:
	PIMAGE	m_pImg;
	int		m_nImages;
};

class CSkinObjectText : public CSkinObject  
{
public:
	CSkinObjectText(){};
	CSkinObjectText(LPCTSTR pszText,DWORD dwID)
	{
		SetText(pszText);
		SetID(dwID);
	}

	void SetText(LPCTSTR pszText);
	void SetColor(COLORREF cr);
	void SetFont(LPCTSTR lfFaceName,int lfHeight);

	virtual void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam);
private:
	COLORREF m_cr;
	LOGFONT m_lf;
};

class CSkinObjectLine : public CSkinObject
{
public:
	CSkinObjectLine(DWORD dwID=0)
	{
		SetID(dwID);
		m_ps=PS_SOLID;
	}
	
	void SetColor(COLORREF cr){m_cr=cr;}
	void SetPoints(POINT pt1,POINT pt2)
	{
		m_pt1=pt1;
		m_pt2=pt2;
	}
	void SetPenStyle(int ps){m_ps=ps;}
	virtual void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)
	{
		CPen pen;
		pen.CreatePen(m_ps,1,m_cr);
		CPen *pOldPen=pDC->SelectObject(&pen);
		CPoint pt=m_pt1;
		pt.Offset(ptOffset);
		pDC->MoveTo(pt);
		pt=m_pt2;
		pt.Offset(ptOffset);
		pDC->LineTo(pt);
		pDC->SelectObject(pOldPen);
	}
private:
	COLORREF m_cr;
	POINT	m_pt1,m_pt2;
	int		m_ps;
};

class CSkinObjectFrame : public CSkinObject
{
public:
	CSkinObjectFrame(DWORD dwID=0)
	{
		SetID(dwID);
	}
	
	virtual void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)
	{
		CPen pen;
		pen.CreatePen(PS_DOT,1,RGB(0,0,0));
		CPen *pOldPen=pDC->SelectObject(&pen);
		POINT pts[5]=
		{
			{m_rect.left+ptOffset.x,m_rect.top+ptOffset.y},
			{m_rect.right+ptOffset.x,m_rect.top+ptOffset.y},
			{m_rect.right+ptOffset.x,m_rect.bottom+ptOffset.y},
			{m_rect.left+ptOffset.x,m_rect.bottom+ptOffset.y},
			{m_rect.left+ptOffset.x,m_rect.top+ptOffset.y},
		};
		pDC->Polyline(pts,5);
		pDC->SelectObject(pOldPen);
	}

};

class CSkinObjectFlag : public CSkinObject
{
public:
	CSkinObjectFlag(DWORD dwID){
		SetID(dwID);
		m_imgList.Create(16,16,ILC_COLORDDB|ILC_MASK,0,2);
	}
	virtual ~CSkinObjectFlag()
	{
		m_imgList.DeleteImageList();
	}

	virtual void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)
	{
		m_imgList.Draw(pDC,0,CPoint(m_rect.left+ptOffset.x,m_rect.top+ptOffset.y),ILD_TRANSPARENT);
	}
	
	void SetFlagFile(LPCTSTR pszFileName,COLORREF crMask)
	{
		HBITMAP hBmp=(HBITMAP)LoadImage(NULL,pszFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(hBmp)
		{
			RemoveImageList();
			m_imgList.Add(CBitmap::FromHandle(hBmp),crMask);
			DeleteObject(hBmp);
		}
	}
	void SetFlagResource(UINT uResID,COLORREF crMask)
	{
		CBitmap bmp;
		bmp.LoadBitmap(uResID);
		if(bmp.m_hObject)
		{
			RemoveImageList();
			m_imgList.Add(&bmp,crMask);			
		}
	}
private:
	void RemoveImageList()
	{
		int nImages=m_imgList.GetImageCount();
		for(int i=nImages-1;i>=0;i--)
		{
			m_imgList.Remove(i);
		}
	}
	CImageList m_imgList;
};
#endif // !defined(AFX_SKINOBJECTEX_H__64CB34C0_3579_4B63_8D09_7DA7CEFC9FF8__INCLUDED_)
