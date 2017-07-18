// PropertyGridItemEx.cpp: implementation of the CProperttyGridItemFileEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PropertyGridItemEx.h"
#include "resource.h"
#include "ColorPickDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPropertyGridItemFileEx::CPropertyGridItemFileEx(CString strCaption, LPCTSTR lpszFileName , BOOL bOpen,LPCTSTR pszFilter,LPCTSTR pszInitPath)
:CPropertyGridItemFile(strCaption,lpszFileName,bOpen)
{
	m_strFilter=pszFilter?pszFilter:"所有文件(*.*)|*.*||";
	m_strInitPath=pszInitPath;
}

CPropertyGridItemFileEx::~CPropertyGridItemFileEx()
{

}

void CPropertyGridItemFileEx::OnInplaceButtonDown()
{
	CFileDialog fileDlg(m_bOpen,NULL,GetFileName(),OFN_HIDEREADONLY,m_strFilter);
	if(!GetFileName().IsEmpty())
	{
		fileDlg.m_ofn.lpstrInitialDir=m_strInitPath;
	}
	if(fileDlg.DoModal()==IDOK)
	{
		if(!m_strInitPath.IsEmpty())
		{
			CopyFile(fileDlg.GetPathName(),m_strInitPath+"\\"+fileDlg.GetFileName(),FALSE);
		}
		SetFileName(fileDlg.GetFileName());
		CWnd *pGrid=GetGrid();
		pGrid->GetParent()->GetParent()->PostMessage(PGWM_PROPERTYGRID_NOTIFY,MAKELONG(pGrid->GetDlgCtrlID(),PGN_ITEMVALUE_CHANGED),(LPARAM)this);
	}
}


//////////////////////////////////////////////////////////////////////////
CPropertyGridItemColorEx::CPropertyGridItemColorEx(CString strCaption, COLORREF cr)
:CPropertyGridItemColor(strCaption,cr)
{
}

CPropertyGridItemColorEx::~CPropertyGridItemColorEx()
{

}
void CPropertyGridItemColorEx::OnInplaceButtonDown()
{
	CColorPickDlg colorPickDlg;
	if(colorPickDlg.DoModal()==IDOK)
	{
		SetColor(colorPickDlg.m_crPick);
		CWnd *pGrid=GetGrid();
		pGrid->GetParent()->GetParent()->PostMessage(PGWM_PROPERTYGRID_NOTIFY,MAKELONG(pGrid->GetDlgCtrlID(),PGN_ITEMVALUE_CHANGED),(LPARAM)this);
	}
}

//////////////////////////////////////////////////////////////////////////
CPropertyGridItemFontEx::CPropertyGridItemFontEx(CString strCaption,LOGFONT *plf)
:CPropertyGridItemFile(strCaption,"")
{
	char szFontInfo[200];
	MakeFontStr(plf,szFontInfo);
	SetValue(szFontInfo);
	memcpy(&m_lf,plf,sizeof(LOGFONT));
}

BOOL CPropertyGridItemFontEx::ParseFontStr(LPCTSTR pszFontInfo,LOGFONT &lf)
{
	CStringArray strArr;
	char szFontInfo[200];
	char szName[50],szValue[50];
	strcpy(szFontInfo,pszFontInfo);
	char *p=szFontInfo;
	for(;;)
	{
		char *pEnd=strstr(p,";");
		if(!pEnd){ strArr.Add(p); break;}
		pEnd[0]=0;
		strArr.Add(p);
		p=pEnd+1;
	}
	for(int i=0;i<strArr.GetSize();i++)
	{
		CString str=strArr[i];
		str.Replace("="," ");
		sscanf(str,"%s %s",szName,szValue);
		if(strcmp(szName,"face")==0)
		{
			strcpy(lf.lfFaceName,szValue);
		}else if(strcmp(szName,"size")==0)
		{
			lf.lfHeight=atoi(szValue);
		}else if(strcmp(szName,"bold")==0)
		{
			lf.lfWeight=atoi(szValue)?700:400;
		}else if(strcmp(szName,"italic")==0)
		{
			lf.lfItalic=atoi(szValue);
		}
	}
	return TRUE;
}


void CPropertyGridItemFontEx::MakeFontStr(LOGFONT *plf, char *pszFontInfo)
{
	sprintf(pszFontInfo,"face=%s;size=%d;bold=%d;italic=%d",plf->lfFaceName,-abs(plf->lfHeight),plf->lfWeight==700?1:0,plf->lfItalic);
}

void CPropertyGridItemFontEx::OnValueChanged(CString strValue)
{
	ParseFontStr(strValue,m_lf);
	CPropertyGridItemFile::OnValueChanged(strValue);
}

void CPropertyGridItemFontEx::OnInplaceButtonDown()
{
	ParseFontStr(GetValue(),m_lf);
	CFontDialog fontPick(&m_lf,CF_SCREENFONTS);
	if(fontPick.DoModal()==IDOK)
	{
		char szValue[200];
		fontPick.GetCurrentFont(&m_lf);
		MakeFontStr(&m_lf,szValue);
		SetValue(szValue);
		OnValueChanged(szValue);
	}
}

