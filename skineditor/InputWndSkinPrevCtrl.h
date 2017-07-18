// InputWndSkinPrevCtrl.h: interface for the CInputWndSkinPrevCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTWNDSKINPREVCTRL_H__C5369BA0_A4BF_4733_AF13_C678CE759AB6__INCLUDED_)
#define AFX_INPUTWNDSKINPREVCTRL_H__C5369BA0_A4BF_4733_AF13_C678CE759AB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SkinPrevCtrl.h"
#include "skindef.h"

typedef enum tagIWM{IWM_SHAPE=0,IWM_SPELL,IWM_ASSOCIATE,IWM_SENT,IWM_ENGLISH,IWM_USERDEF} IWM;

typedef struct tagBGIMGINFO
{
	int nLeft,nRight;
	BOOL bTile;
	int nCompLeft,nCompRight;
	int nCandLeft,nCandRight;
	RECT rcMargin;
}BGIMGINFO;

class CInputWndSkinPrevCtrl : public CSkinPrevCtrl  
{
	typedef struct OBJECTINFO{
		CSkinObject **pObComp;		//编码栏数据
		int			 nObComp;		//编码栏数据
		CSkinObject **pObCand;		//重码栏数据
		CSkinObjectLine obMargin[4];//4个边距对象
		int			 nObCand;		//重码栏数据
		int			 nCompWid;		//编码宽度
		int			 nCandWid;		//重码宽度
	}OBJECTINFO;

public:
	void SetTile(BOOL bTile);
	SIZE GetModeSize(IWM iwm);
	CInputWndSkinPrevCtrl();
	virtual ~CInputWndSkinPrevCtrl();
	virtual void	OnDrawBkImage(CDC *pDC,POINT ptOffset);
	virtual SIZE GetSize();

	void SetMode(IWM iwm);
	void InitObjects(LPCTSTR pszPath,SKININPUT *pSkinInput,COLORCOMP *pColorComp,COLORCAND *pColorCand);
protected:
	void FreeObjects();
	OBJECTINFO m_objInfo[IWM_USERDEF+1];
	BGIMGINFO	m_bgImgInfo;
	IWM			m_iwm;
};

class CInputWndVertSkinPrevCtrl : public CSkinPrevCtrl  
{
public:
	CInputWndVertSkinPrevCtrl(){};
	void InitObjects(LPCTSTR pszPath,SKININPUT *pSkinInput,COLORCOMP *pColorComp,COLORCAND *pColorCand);
	virtual void OnDrawBkImage(CDC *pDC,POINT ptOffset);
	void SetTile(BOOL bTile);

	CSkinObjectText	m_sotComp;
	CSkinObjectText	m_sotCand[2];
	CSkinObjectLine m_soLMargin[4];
//	CSkinObjectLine m_soLCut[2];

	BGIMGINFO	m_bgImgInfo;
};
#endif // !defined(AFX_INPUTWNDSKINPREVCTRL_H__C5369BA0_A4BF_4733_AF13_C678CE759AB6__INCLUDED_)
