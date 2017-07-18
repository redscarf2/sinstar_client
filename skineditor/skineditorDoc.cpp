// skineditorDoc.cpp : implementation of the CSkineditorDoc class
//

#include "stdafx.h"
#include "skineditor.h"

#include "skineditorDoc.h"
#include "propertygriditemex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkineditorDoc

IMPLEMENT_DYNCREATE(CSkineditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CSkineditorDoc, CDocument)
	//{{AFX_MSG_MAP(CSkineditorDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkineditorDoc construction/destruction

CSkineditorDoc::CSkineditorDoc()
{
	// TODO: add one-time construction code here
	strcpy(m_skinDesc.szName,"启程皮肤");
	strcpy(m_skinDesc.szAuthor,"匿名");
	memset(&m_colorComp,0,sizeof(m_colorComp));
	memset(&m_colorCand,0,sizeof(m_colorCand));
	memset(&m_skinInput,0,sizeof(m_skinInput));
	memset(&m_skinStatus,0,sizeof(m_skinStatus));
}

CSkineditorDoc::~CSkineditorDoc()
{
}


/////////////////////////////////////////////////////////////////////////////
// CSkineditorDoc diagnostics

#ifdef _DEBUG
void CSkineditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSkineditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkineditorDoc commands

COLORREF Skin_LoadColor(LPCTSTR pszIniFile,LPCTSTR pszSection,LPCTSTR pszEntry,COLORREF crDef)
{
	int  r,g,b;
	char szBuf[50]={0};
	GetPrivateProfileString(pszSection,pszEntry,"",szBuf,50,pszIniFile);
	if(szBuf[0])
	{
		sscanf(szBuf,"%d,%d,%d",&r,&g,&b);
		return RGB(r,g,b);
	}else
	{
		return crDef;
	}
}

void	Skin_SaveColor(LPCTSTR pszIniFile,LPCSTR pszSection,LPCTSTR pszEntry,COLORREF cr)
{
	char szBuf[50];
	sprintf(szBuf,"%d,%d,%d",GetRValue(cr),GetGValue(cr),GetBValue(cr));
	WritePrivateProfileString(pszSection,pszEntry,szBuf,pszIniFile);
}

BOOL Skin_LoadRect(LPCTSTR pszIniFile,LPCTSTR pszSection,LPCTSTR pszEntry,RECT &rc)
{
	char szBuf[50]={0};
	GetPrivateProfileString(pszSection,pszEntry,"",szBuf,50,pszIniFile);
	if(!szBuf[0]) return FALSE;
	sscanf(szBuf,"%d,%d,%d,%d",&rc.left,&rc.top,&rc.right,&rc.bottom);
	return TRUE;
}

void Skin_SaveRect(LPCTSTR pszIniFile,LPCTSTR pszSection,LPCTSTR pszEntry,RECT rc)
{
	char szBuf[50];
	sprintf(szBuf,"%d,%d,%d,%d",rc.left,rc.top,rc.right,rc.bottom);
	WritePrivateProfileString(pszSection,pszEntry,szBuf,pszIniFile);
}

BOOL Skin_LoadBtn(LPCTSTR pszIniFile,LPCTSTR pszButton,BTNINFO &btn)
{
	char szFile[MAX_PATH];
	if(!GetPrivateProfileString(pszButton,"bmp",NULL,szFile,MAX_PATH,pszIniFile)) return FALSE;
	strcpy(btn.szFile,szFile);
	GetPrivateProfileString(pszButton,"tip",NULL,btn.szTip,MAX_PATH,pszIniFile);
	Skin_LoadRect(pszIniFile,pszButton,"rect",btn.rcShrink);
	btn.rcFull=btn.rcShrink;
	Skin_LoadRect(pszIniFile,pszButton,"rect2",btn.rcFull);
	return TRUE;
}

void Skin_SaveBtn(LPCTSTR pszIniFile,LPCTSTR pszButton,BTNINFO btn)
{
	if(btn.szFile[0])
	{
		WritePrivateProfileString(pszButton,"bmp",btn.szFile,pszIniFile);
		BOOL bBmpFile=stricmp(btn.szFile+strlen(btn.szFile)-3,"bmp")==0;
		WritePrivateProfileString(pszButton,"imgtype",bBmpFile?"0":"1",pszIniFile);
		WritePrivateProfileString(pszButton,"tip",btn.szTip,pszIniFile);
		Skin_SaveRect(pszIniFile,pszButton,"rect",btn.rcShrink);
		if(!EqualRect(&btn.rcFull,&btn.rcShrink)) 
			Skin_SaveRect(pszIniFile,pszButton,"rect2",btn.rcFull);
		else
			WritePrivateProfileString(pszButton,"rect2",NULL,pszIniFile);
	}
}


BOOL CSkineditorDoc::OnOpenDocument(LPCTSTR pszConfig) 
{
	m_strPath=pszConfig;
	m_strPath=m_strPath.Left(m_strPath.GetLength()-strlen("config.ini")-1);//获得皮肤所在的目录
	SetCurrentDirectory(m_strPath);
	DWORD dwRet=GetFileAttributes(pszConfig);
	if(GetFileAttributes(pszConfig)!=0xFFFFFFFF)
	{
		char szBuf[200];
		memset(&m_skinInput.lf,0,sizeof(LOGFONT));
		memset(&m_skinStatus.lf,0,sizeof(LOGFONT));
		//皮肤属性
		GetPrivateProfileString("desc","name","",m_skinDesc.szName,50,pszConfig);
		GetPrivateProfileString("desc","author","",m_skinDesc.szAuthor,50,pszConfig);
		
		//输入窗口部分[通用]
		GetPrivateProfileString("InputWindow","font_name","宋体",m_skinInput.lf.lfFaceName,LF_FACESIZE,pszConfig);
		m_skinInput.lf.lfHeight=GetPrivateProfileInt("InputWindow","font_height",16,pszConfig);
		GetPrivateProfileString("InputWindow","font","",szBuf,200,pszConfig);
		if(szBuf[0]) CPropertyGridItemFontEx::ParseFontStr(szBuf,m_skinInput.lf);
		m_skinInput.lf.lfHeight=-abs(m_skinInput.lf.lfHeight);
		GetPrivateProfileString("InputWindow","charmode",NULL,m_skinInput.szSymbolMode,MAX_PATH,pszConfig);
		GetPrivateProfileString("Btn_Input_PrevPage","bmp",NULL,m_skinInput.szBtnPrev,MAX_PATH,pszConfig);
		GetPrivateProfileString("Btn_Input_NextPage","bmp",NULL,m_skinInput.szBtnNext,MAX_PATH,pszConfig);
		//输入窗口横排部分
		GetPrivateProfileString("InputWindow","image",NULL,m_skinInput.loHorz.szBgImg,MAX_PATH,pszConfig);
		m_skinInput.loHorz.nLeft=GetPrivateProfileInt("InputWindow","left",10,pszConfig);
		m_skinInput.loHorz.bTile=GetPrivateProfileInt("InputWindow","tile",1,pszConfig);
		m_skinInput.loHorz.nRight=GetPrivateProfileInt("InputWindow","right",10,pszConfig);

		m_skinInput.loHorz.bHeteroMorphic=GetPrivateProfileInt("InputWindow","heteromorphic",0,pszConfig);
		if(m_skinInput.loHorz.bHeteroMorphic)
			m_skinInput.loHorz.crTrans=Skin_LoadColor(pszConfig,"InputWindow","color_trans",-1);
		else
			m_skinInput.loHorz.crTrans=-1;

		Skin_LoadRect(pszConfig,"InputWindow","margin",m_skinInput.loHorz.rcMargin);

		GetPrivateProfileString("InputWindow","margin_comp","0,0",szBuf,100,pszConfig);
		sscanf(szBuf,"%d,%d",&m_skinInput.loHorz.nCompLeft,&m_skinInput.loHorz.nCompRight);
		GetPrivateProfileString("InputWindow","margin_cand","0,0",szBuf,100,pszConfig);
		sscanf(szBuf,"%d,%d",&m_skinInput.loHorz.nCandLeft,&m_skinInput.loHorz.nCandRight);
		m_skinInput.loHorz.nCandInter=GetPrivateProfileInt("InputWindow","cand_inter",5,pszConfig);
		m_skinInput.loHorz.nCompHei=GetPrivateProfileInt("InputWindow","comp_hei",0,pszConfig);
		m_skinInput.loHorz.nLineHei=GetPrivateProfileInt("InputWindow","line_hei",0,pszConfig);
		//输入窗口竖排部分
		GetPrivateProfileString("InputWindow2","image",NULL,m_skinInput.loVert.szBgImg,MAX_PATH,pszConfig);
		m_skinInput.loVert.nLeft=GetPrivateProfileInt("InputWindow2","top",10,pszConfig);
		m_skinInput.loVert.bTile=GetPrivateProfileInt("InputWindow2","tile",1,pszConfig);
		m_skinInput.loVert.nRight=GetPrivateProfileInt("InputWindow2","bottom",10,pszConfig);
		
		m_skinInput.loVert.bHeteroMorphic=GetPrivateProfileInt("InputWindow2","heteromorphic",0,pszConfig);
		if(m_skinInput.loVert.bHeteroMorphic)
			m_skinInput.loVert.crTrans=Skin_LoadColor(pszConfig,"InputWindow2","color_trans",-1);
		else
			m_skinInput.loVert.crTrans=-1;
		
		Skin_LoadRect(pszConfig,"InputWindow2","margin",m_skinInput.loVert.rcMargin);
		
		GetPrivateProfileString("InputWindow2","margin_comp","0,0",szBuf,100,pszConfig);
		sscanf(szBuf,"%d,%d",&m_skinInput.loVert.nCompLeft,&m_skinInput.loVert.nCompRight);
		GetPrivateProfileString("InputWindow2","margin_cand","0,0",szBuf,100,pszConfig);
		sscanf(szBuf,"%d,%d",&m_skinInput.loVert.nCandLeft,&m_skinInput.loVert.nCandRight);
		m_skinInput.loVert.nCandInter=GetPrivateProfileInt("InputWindow2","cand_inter",5,pszConfig);
		m_skinInput.loVert.nCompHei=GetPrivateProfileInt("InputWindow2","comp_hei",0,pszConfig);
		m_skinInput.loVert.nLineHei=GetPrivateProfileInt("InputWindow2","line_hei",0,pszConfig);

		//状态栏窗口部分
		GetPrivateProfileString("StatusWindow","imageFull",NULL,m_skinStatus.szBgImgFull,MAX_PATH,pszConfig);
		GetPrivateProfileString("StatusWindow","imageShrink",NULL,m_skinStatus.szBgImgShrink,MAX_PATH,pszConfig);
		if(m_skinStatus.bHeteroMorphic=GetPrivateProfileInt("StatusWindow","heteromorphic",0,pszConfig))
			m_skinStatus.crTrans=Skin_LoadColor(pszConfig,"StatusWindow","color_trans",-1);
		else
			m_skinStatus.crTrans=-1;

		Skin_LoadRect(pszConfig,"StatusWindow","rect_text",m_skinStatus.rcText);
		if(!Skin_LoadRect(pszConfig,"StatusWindow","rect_text2",m_skinStatus.rcText2))
			m_skinStatus.rcText2=m_skinStatus.rcText;

		Skin_LoadRect(pszConfig,"StatusWindow","rect_drag",m_skinStatus.rcDrag);
		if(!Skin_LoadRect(pszConfig,"StatusWindow","rect_drag2",m_skinStatus.rcDrag2))
			m_skinStatus.rcDrag2=m_skinStatus.rcDrag;

		Skin_LoadRect(pszConfig,"StatusWindow","rect_icon",m_skinStatus.rcIcon);
		if(!Skin_LoadRect(pszConfig,"StatusWindow","rect_icon2",m_skinStatus.rcIcon2))
			m_skinStatus.rcIcon2=m_skinStatus.rcIcon;

		GetPrivateProfileString("StatusWindow","font_name","宋体",m_skinStatus.lf.lfFaceName,LF_FACESIZE,pszConfig);
		m_skinStatus.lf.lfHeight=GetPrivateProfileInt("StatusWindow","font_height",-12,pszConfig);
		GetPrivateProfileString("StatusWindow","font","",szBuf,200,pszConfig);
		if(szBuf[0]) CPropertyGridItemFontEx::ParseFontStr(szBuf,m_skinStatus.lf);
		m_skinStatus.lf.lfHeight=-abs(m_skinStatus.lf.lfHeight);

		m_skinStatus.crText=Skin_LoadColor(pszConfig,"StatusWindow","color_text",RGB(0,0,0));

		if(GetPrivateProfileString("StatusWindow","flag_icon",NULL,m_skinStatus.szIcon,MAX_PATH,pszConfig))
		{
			m_skinStatus.crIconTrans=Skin_LoadColor(pszConfig,"StatusWindow","flag_mask",RGB(255,0,255));
		}
		
		Skin_LoadBtn(pszConfig,"Btn_Status_ViewMode1",m_skinStatus.btnExtent);
		Skin_LoadBtn(pszConfig,"Btn_Status_ViewMode2",m_skinStatus.btnShrink);
		Skin_LoadBtn(pszConfig,"Btn_Status_CharMode1",m_skinStatus.btnChar1);
		Skin_LoadBtn(pszConfig,"Btn_Status_CharMode2",m_skinStatus.btnChar2);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Record1",m_skinStatus.btnRecord1);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Record2",m_skinStatus.btnRecord2);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Sound1",m_skinStatus.btnRead1);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Sound2",m_skinStatus.btnRead2);
		Skin_LoadBtn(pszConfig,"Btn_Staus_English1",m_skinStatus.btnWord1);
		Skin_LoadBtn(pszConfig,"Btn_Staus_English2",m_skinStatus.btnWord2);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Query",m_skinStatus.btnQuery);
		Skin_LoadBtn(pszConfig,"Btn_Staus_MakeWord",m_skinStatus.btnMakePhrase);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Menu",m_skinStatus.btnMenu);
		Skin_LoadBtn(pszConfig,"Btn_Staus_Help",m_skinStatus.btnHelp);

		//载入编码颜色配置
		m_colorComp.crNormal=Skin_LoadColor(pszConfig,"Color_Comp","normal",RGB(0,0,0));
		m_colorComp.crInput=Skin_LoadColor(pszConfig,"Color_Comp","input",RGB(128,0,0));
		m_colorComp.crSentSel=Skin_LoadColor(pszConfig,"Color_Comp","sent_sel",RGB(255,0,0));
		m_colorComp.crSentNormal=Skin_LoadColor(pszConfig,"Color_Comp","sent_normal",RGB(0,0,0));
		m_colorComp.crSentInit=Skin_LoadColor(pszConfig,"Color_Comp","sent_init",RGB(100,100,100));
		m_colorComp.crEnCode=Skin_LoadColor(pszConfig,"Color_Comp","en_code",RGB(0,0,0));
		m_colorComp.crEnMakeup=Skin_LoadColor(pszConfig,"Color_Comp","en_makeup",RGB(255,0,0));
		m_colorComp.crEnPhonetic=Skin_LoadColor(pszConfig,"Color_Comp","en_phonetic",RGB(0,0,255));
		m_colorComp.crSpTmHead=Skin_LoadColor(pszConfig,"Color_Comp","spell_tmhead",RGB(255,0,0));
		m_colorComp.crSpResult=Skin_LoadColor(pszConfig,"Color_Comp","spell_result",RGB(0,0,0));
		m_colorComp.crSpSpell=Skin_LoadColor(pszConfig,"Color_Comp","spell_spell",RGB(0,0,255));
		m_colorComp.crSpEdit=Skin_LoadColor(pszConfig,"Color_Comp","spell_edit",RGB(255,0,0));
		m_colorComp.crSpLine=Skin_LoadColor(pszConfig,"Color_Comp","spell_line",RGB(255,0,0));
		m_colorComp.crUdHead=Skin_LoadColor(pszConfig,"Color_Comp","userdef_head",RGB(255,0,0));
		m_colorComp.crUdCode=Skin_LoadColor(pszConfig,"Color_Comp","userdef_code",RGB(0,0,0));
		m_colorComp.crUdMakeup=Skin_LoadColor(pszConfig,"Color_Comp","userdef_makeup",RGB(0,0,255));
		//载入重码颜色
		m_colorCand.crIndex=Skin_LoadColor(pszConfig,"Color_Cand","index",RGB(50,50,50));
		m_colorCand.crNormal=Skin_LoadColor(pszConfig,"Color_Cand","normal",RGB(0,0,0));
		m_colorCand.crForecast=Skin_LoadColor(pszConfig,"Color_Cand","association",RGB(0,0,255));
		m_colorCand.crCompNormal=Skin_LoadColor(pszConfig,"Color_Cand","comp_nomal",RGB(0,0,255));
		m_colorCand.crCompWild=Skin_LoadColor(pszConfig,"Color_Cand","comp_wild",RGB(255,0,0));
		m_colorCand.crEnHead=Skin_LoadColor(pszConfig,"Color_Cand","en_head",RGB(128,0,0));
		m_colorCand.crEnPhonetic=Skin_LoadColor(pszConfig,"Color_Cand","en_phonetic",RGB(0,0,255));
		m_colorCand.crTip=Skin_LoadColor(pszConfig,"Color_Cand","tip",RGB(0,0,0));
		m_colorCand.crUserDef=Skin_LoadColor(pszConfig,"Color_Cand","userdef",m_colorCand.crForecast);
		m_colorCand.crInactive=Skin_LoadColor(pszConfig,"Color_Cand","inactive",RGB(192,192,192));
		m_colorCand.crMixSp=Skin_LoadColor(pszConfig,"Color_Cand","mixsp",RGB(192,192,192));
	}else
	{
		char szPath[MAX_PATH];
		_splitpath(pszConfig,NULL,szPath,NULL,NULL);
		szPath[strlen(szPath)-1]=0;
		CString strPath=szPath;
		if(!strPath.IsEmpty())
		{
			int pos=strPath.ReverseFind('\\');
			strcpy(m_skinDesc.szName,szPath+pos+1);
		}
	}
	return TRUE;
}

BOOL CSkineditorDoc::OnSaveDocument(LPCTSTR pszConfig) 
{
	char szBuf[200];
	//皮肤属性
	WritePrivateProfileString("desc","name",m_skinDesc.szName,pszConfig);
	WritePrivateProfileString("desc","author",m_skinDesc.szAuthor,pszConfig);

	//输入窗口部分[通用]
	WritePrivateProfileString("InputWindow","font_name",m_skinInput.lf.lfFaceName,pszConfig);
	WritePrivateProfileInt("InputWindow","font_height",-abs(m_skinInput.lf.lfHeight),pszConfig);
	CPropertyGridItemFontEx::MakeFontStr(&m_skinInput.lf,szBuf);
	WritePrivateProfileString("InputWindow","font",szBuf,pszConfig);
	WritePrivateProfileString("InputWindow","charmode",m_skinInput.szSymbolMode,pszConfig);
	WritePrivateProfileString("Btn_Input_PrevPage","bmp",m_skinInput.szBtnPrev,pszConfig);
	WritePrivateProfileString("Btn_Input_PrevPage","imgtype",GetImageType(m_skinInput.szBtnPrev)?"1":"0",pszConfig);
	WritePrivateProfileString("Btn_Input_NextPage","bmp",m_skinInput.szBtnNext,pszConfig);
	WritePrivateProfileString("Btn_Input_NextPage","imgtype",GetImageType(m_skinInput.szBtnNext)?"1":"0",pszConfig);
	//输入窗口横排模式
	WritePrivateProfileString("InputWindow","image",m_skinInput.loHorz.szBgImg,pszConfig);
	WritePrivateProfileString("InputWindow","imgtype",GetImageType(m_skinInput.loHorz.szBgImg)?"1":"0",pszConfig);
	WritePrivateProfileInt("InputWindow","left",m_skinInput.loHorz.nLeft,pszConfig);
	WritePrivateProfileString("InputWindow","tile",m_skinInput.loHorz.bTile?"1":"0",pszConfig);
	WritePrivateProfileInt("InputWindow","right",m_skinInput.loHorz.nRight,pszConfig);

	WritePrivateProfileString("InputWindow","heteromorphic",m_skinInput.loHorz.bHeteroMorphic?"1":"0",pszConfig);
	if(m_skinInput.loHorz.bHeteroMorphic && m_skinInput.loHorz.crTrans!=-1)
		Skin_SaveColor(pszConfig,"InputWindow","color_trans",m_skinInput.loHorz.crTrans);

	Skin_SaveRect(pszConfig,"InputWindow","margin",m_skinInput.loHorz.rcMargin);

	sprintf(szBuf,"%d,%d",m_skinInput.loHorz.nCompLeft,m_skinInput.loHorz.nCompRight);
	WritePrivateProfileString("InputWindow","margin_comp",szBuf,pszConfig);
	sprintf(szBuf,"%d,%d",m_skinInput.loHorz.nCandLeft,m_skinInput.loHorz.nCandRight);
	WritePrivateProfileString("InputWindow","margin_cand",szBuf,pszConfig);
	WritePrivateProfileInt("InputWindow","cand_inter",m_skinInput.loHorz.nCandInter,pszConfig);
	WritePrivateProfileInt("InputWindow","comp_hei",m_skinInput.loHorz.nCompHei,pszConfig);
	WritePrivateProfileInt("InputWindow","line_hei",m_skinInput.loHorz.nLineHei,pszConfig);
	//输入窗口竖排部分
	WritePrivateProfileString("InputWindow2","image",m_skinInput.loVert.szBgImg,pszConfig);
	WritePrivateProfileString("InputWindow2","imgtype",GetImageType(m_skinInput.loVert.szBgImg)?"1":"0",pszConfig);
	WritePrivateProfileInt("InputWindow2","top",m_skinInput.loVert.nLeft,pszConfig);
	WritePrivateProfileString("InputWindow2","tile",m_skinInput.loVert.bTile?"1":"0",pszConfig);
	WritePrivateProfileInt("InputWindow2","bottom",m_skinInput.loVert.nRight,pszConfig);
	
	WritePrivateProfileString("InputWindow2","heteromorphic",m_skinInput.loVert.bHeteroMorphic?"1":"0",pszConfig);
	if(m_skinInput.loVert.bHeteroMorphic && m_skinInput.loVert.crTrans!=-1)
		Skin_SaveColor(pszConfig,"InputWindow2","color_trans",m_skinInput.loVert.crTrans);
	
	Skin_SaveRect(pszConfig,"InputWindow2","margin",m_skinInput.loVert.rcMargin);
	
	sprintf(szBuf,"%d,%d",m_skinInput.loVert.nCompLeft,m_skinInput.loVert.nCompRight);
	WritePrivateProfileString("InputWindow2","margin_comp",szBuf,pszConfig);
	sprintf(szBuf,"%d,%d",m_skinInput.loVert.nCandLeft,m_skinInput.loVert.nCandRight);
	WritePrivateProfileString("InputWindow2","margin_cand",szBuf,pszConfig);
	WritePrivateProfileInt("InputWindow2","cand_inter",m_skinInput.loVert.nCandInter,pszConfig);
	WritePrivateProfileInt("InputWindow2","comp_hei",m_skinInput.loVert.nCompHei,pszConfig);
	WritePrivateProfileInt("InputWindow2","line_hei",m_skinInput.loVert.nLineHei,pszConfig);

	//状态栏窗口部分
	WritePrivateProfileString("StatusWindow","imageFull",m_skinStatus.szBgImgFull,pszConfig);
	WritePrivateProfileString("StatusWindow","imageShrink",m_skinStatus.szBgImgShrink,pszConfig);
	WritePrivateProfileString("StatusWindow","imgtype",GetImageType(m_skinStatus.szBgImgFull)?"1":"0",pszConfig);


	WritePrivateProfileString("StatusWindow","heteromorphic",m_skinStatus.bHeteroMorphic?"1":"0",pszConfig);
	if(m_skinStatus.bHeteroMorphic && m_skinStatus.crTrans!=-1)
		Skin_SaveColor(pszConfig,"StatusWindow","color_trans",m_skinStatus.crTrans);

	Skin_SaveRect(pszConfig,"StatusWindow","rect_text",m_skinStatus.rcText);
	if(!EqualRect(&m_skinStatus.rcText2,&m_skinStatus.rcText))
		Skin_SaveRect(pszConfig,"StatusWindow","rect_text2",m_skinStatus.rcText2);
	else
		WritePrivateProfileString("StatusWindow","rect_text2",NULL,pszConfig);
	Skin_SaveRect(pszConfig,"StatusWindow","rect_drag",m_skinStatus.rcDrag);
	if(!EqualRect(&m_skinStatus.rcDrag2,&m_skinStatus.rcDrag))
		Skin_SaveRect(pszConfig,"StatusWindow","rect_drag2",m_skinStatus.rcDrag2);
	else
		WritePrivateProfileString("StatusWindow","rect_drag2",NULL,pszConfig);
	Skin_SaveRect(pszConfig,"StatusWindow","rect_icon",m_skinStatus.rcIcon);
	if(!EqualRect(&m_skinStatus.rcIcon2,&m_skinStatus.rcIcon))
		Skin_SaveRect(pszConfig,"StatusWindow","rect_icon2",m_skinStatus.rcIcon2);
	else
		WritePrivateProfileString("StatusWindow","rect_icon2",NULL,pszConfig);

	WritePrivateProfileString("StatusWindow","font_name",m_skinStatus.lf.lfFaceName,pszConfig);
	WritePrivateProfileInt("StatusWindow","font_height",-abs(m_skinStatus.lf.lfHeight),pszConfig);
	CPropertyGridItemFontEx::MakeFontStr(&m_skinStatus.lf,szBuf);
	WritePrivateProfileString("StatusWindow","font",szBuf,pszConfig);

	Skin_SaveColor(pszConfig,"StatusWindow","color_text",m_skinStatus.crText);

	WritePrivateProfileString("StatusWindow","flag_icon",m_skinStatus.szIcon,pszConfig);
	if(m_skinStatus.szIcon[0]) Skin_SaveColor(pszConfig,"StatusWindow","flag_mask",m_skinStatus.crIconTrans);

	Skin_SaveBtn(pszConfig,"Btn_Status_ViewMode1",m_skinStatus.btnExtent);
	Skin_SaveBtn(pszConfig,"Btn_Status_ViewMode2",m_skinStatus.btnShrink);
	Skin_SaveBtn(pszConfig,"Btn_Status_CharMode1",m_skinStatus.btnChar1);
	Skin_SaveBtn(pszConfig,"Btn_Status_CharMode2",m_skinStatus.btnChar2);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Record1",m_skinStatus.btnRecord1);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Record2",m_skinStatus.btnRecord2);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Sound1",m_skinStatus.btnRead1);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Sound2",m_skinStatus.btnRead2);
	Skin_SaveBtn(pszConfig,"Btn_Staus_English1",m_skinStatus.btnWord1);
	Skin_SaveBtn(pszConfig,"Btn_Staus_English2",m_skinStatus.btnWord2);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Query",m_skinStatus.btnQuery);
	Skin_SaveBtn(pszConfig,"Btn_Staus_MakeWord",m_skinStatus.btnMakePhrase);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Menu",m_skinStatus.btnMenu);
	Skin_SaveBtn(pszConfig,"Btn_Staus_Help",m_skinStatus.btnHelp);

	//保存编码颜色配置
	Skin_SaveColor(pszConfig,"Color_Comp","normal",m_colorComp.crNormal);
	Skin_SaveColor(pszConfig,"Color_Comp","input",m_colorComp.crInput);
	Skin_SaveColor(pszConfig,"Color_Comp","sent_sel",m_colorComp.crSentSel);
	Skin_SaveColor(pszConfig,"Color_Comp","sent_normal",m_colorComp.crSentNormal);
	Skin_SaveColor(pszConfig,"Color_Comp","sent_init",m_colorComp.crSentInit);
	Skin_SaveColor(pszConfig,"Color_Comp","en_code",m_colorComp.crEnCode);
	Skin_SaveColor(pszConfig,"Color_Comp","en_makeup",m_colorComp.crEnMakeup);
	Skin_SaveColor(pszConfig,"Color_Comp","en_phonetic",m_colorComp.crEnPhonetic);
	Skin_SaveColor(pszConfig,"Color_Comp","spell_tmhead",m_colorComp.crSpTmHead);
	Skin_SaveColor(pszConfig,"Color_Comp","spell_result",m_colorComp.crSpResult);
	Skin_SaveColor(pszConfig,"Color_Comp","spell_spell",m_colorComp.crSpSpell);
	Skin_SaveColor(pszConfig,"Color_Comp","spell_edit",m_colorComp.crSpEdit);
	Skin_SaveColor(pszConfig,"Color_Comp","spell_line",m_colorComp.crSpLine);
	Skin_SaveColor(pszConfig,"Color_Comp","userdef_head",m_colorComp.crUdHead);
	Skin_SaveColor(pszConfig,"Color_Comp","userdef_code",m_colorComp.crUdCode);
	Skin_SaveColor(pszConfig,"Color_Comp","userdef_makeup",m_colorComp.crUdMakeup);
	//保存重码颜色
	Skin_SaveColor(pszConfig,"Color_Cand","index",m_colorCand.crIndex);
	Skin_SaveColor(pszConfig,"Color_Cand","normal",m_colorCand.crNormal);
	Skin_SaveColor(pszConfig,"Color_Cand","association",m_colorCand.crForecast);
	Skin_SaveColor(pszConfig,"Color_Cand","comp_nomal",m_colorCand.crCompNormal);
	Skin_SaveColor(pszConfig,"Color_Cand","comp_wild",m_colorCand.crCompWild);
	Skin_SaveColor(pszConfig,"Color_Cand","en_head",m_colorCand.crEnHead);
	Skin_SaveColor(pszConfig,"Color_Cand","en_phonetic",m_colorCand.crEnPhonetic);
	Skin_SaveColor(pszConfig,"Color_Cand","tip",m_colorCand.crTip);
	Skin_SaveColor(pszConfig,"Color_Cand","userdef",m_colorCand.crUserDef);
	Skin_SaveColor(pszConfig,"Color_Cand","inactive",m_colorCand.crInactive);
	Skin_SaveColor(pszConfig,"Color_Cand","mixsp",m_colorCand.crMixSp);
	

	SetModifiedFlag(FALSE);     // back to unmodified
	return TRUE;
}

//重载该消息防止出现文件选择对话框.
void CSkineditorDoc::OnFileSave() 
{
	DoSave(GetPathName());	
}

//重载该虚函数防止出现文件选择对话框.
BOOL CSkineditorDoc::DoFileSave()
{
	DoSave(GetPathName());
	return TRUE;
}

CLeftView * CSkineditorDoc::GetLeftView()
{
	CView *pView;
	POSITION pos=GetFirstViewPosition();
	while(pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CLeftView))) break;
	}
	ASSERT(pView);
	return (CLeftView*)pView;
}

CSkineditorView * CSkineditorDoc::GetEditView()
{
	CView *pView;
	POSITION pos=GetFirstViewPosition();
	while(pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CSkineditorView))) break;
	}
	ASSERT(pView);
	return (CSkineditorView*)pView;
}

CPanelPrevForm * CSkineditorDoc::GetPicPrevView()
{
	CView *pView;
	POSITION pos=GetFirstViewPosition();
	while(pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CPanelPrevForm))) break;
	}
	ASSERT(pView);
	return (CPanelPrevForm*)pView;
}

CPanelVertForm * CSkineditorDoc::GetVertView()
{
	CView *pView;
	POSITION pos=GetFirstViewPosition();
	while(pos)
	{
		pView=GetNextView(pos);
		if(pView->IsKindOf(RUNTIME_CLASS(CPanelVertForm))) break;
	}
	ASSERT(pView);
	return (CPanelVertForm*)pView;
}


//判断文件名的扩展名是否为png
BOOL CSkineditorDoc::GetImageType(LPCTSTR pszName)
{
	return stricmp(pszName+strlen(pszName)-3,"png")==0;
}	
