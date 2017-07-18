// SkinObject.h: interface for the CSkinObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINOBJECT_H__14269B0C_8CDD_4CF9_933A_CDF54C998F18__INCLUDED_)
#define AFX_SKINOBJECT_H__14269B0C_8CDD_4CF9_933A_CDF54C998F18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkinObject :public CObject
{
public:
	CSkinObject();
	virtual ~CSkinObject();
	virtual void OnDraw(CDC *pDC,POINT ptOffset,LPARAM lParam)=NULL;
	virtual void CopyData(CSkinObject &newObject)
	{
		newObject.m_bSelected=m_bSelected;
		newObject.m_rect=m_rect;
		newObject.m_dwData=m_dwData;
		strcpy(newObject.m_szDescript,m_szDescript);
	}


	void SetID(DWORD dwID){m_dwID=dwID;}
	DWORD GetID(){return m_dwID;}
	void SetData(DWORD dwData){m_dwData=dwData;}
	DWORD GetData(){return m_dwData;}
	void SetDescprit(LPCTSTR pszDesc)
	{
		ASSERT(strlen(pszDesc)<300);
		strcpy(m_szDescript,pszDesc);
	}
	LPCTSTR GetDescript(){return m_szDescript;}
	void SetRect(RECT rect)
	{
		m_rect.left=rect.left;
		m_rect.top=rect.top;
		m_rect.right=rect.right;
		m_rect.bottom=rect.bottom;
	}
	const RECT GetRect(){return m_rect;}
protected:
	BOOL	m_bSelected;
	char	m_szDescript[300];
	RECT	m_rect;
	DWORD	m_dwID;
	DWORD	m_dwData;
};

#endif // !defined(AFX_SKINOBJECT_H__14269B0C_8CDD_4CF9_933A_CDF54C998F18__INCLUDED_)
