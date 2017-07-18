// ProperttyGridItemEx.h: interface for the CProperttyGridItemFileEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYGRIDITEMEX_H__787E5545_3C4E_44AC_9CCE_CC2005C0BCD3__INCLUDED_)
#define AFX_PROPERTYGRIDITEMEX_H__787E5545_3C4E_44AC_9CCE_CC2005C0BCD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPropertyGridItemFileEx : public CPropertyGridItemFile  
{
public:
	CPropertyGridItemFileEx(CString strCaption, LPCTSTR lpszFileName = NULL, BOOL bOpen = TRUE, LPCTSTR pszFilter=NULL,LPCTSTR pszInitPath=NULL);
	virtual ~CPropertyGridItemFileEx();
	
	void SetInitPath(CString strInitPath){m_strInitPath=strInitPath;}
protected:
	virtual void OnInplaceButtonDown();
	CString m_strFilter;
	CString m_strInitPath;
};

class CPropertyGridItemColorEx :public CPropertyGridItemColor
{
public:
	CPropertyGridItemColorEx(CString strCaption,COLORREF cr);
	virtual ~CPropertyGridItemColorEx();
protected:
	virtual void OnInplaceButtonDown();
};

class CPropertyGridItemFontEx :public CPropertyGridItemFile
{
public:
	CPropertyGridItemFontEx(CString strCaption,LOGFONT *plf);
	void GetFont(LOGFONT *plf){memcpy(plf,&m_lf,sizeof(LOGFONT));}
	static void MakeFontStr(LOGFONT *plf,char *pszFontInfo);
	static BOOL ParseFontStr(LPCTSTR pszFontInfo,LOGFONT &lf);
protected:
	virtual void OnInplaceButtonDown();
	virtual void OnValueChanged(CString strValue);
	
	LOGFONT m_lf;
};
#endif // !defined(AFX_PROPERTYGRIDITEMEX_H__787E5545_3C4E_44AC_9CCE_CC2005C0BCD3__INCLUDED_)
