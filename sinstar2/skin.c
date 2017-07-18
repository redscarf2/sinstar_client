#include "sinstar2.h"
#include "skin.h"
#include "..\include\helper.h"

#define		MAX_SECTION		(1<<13)	//配置文件段最大值，暂时设定为8K

SKINIINPUT	g_SkinInput={0};		//输入窗口配置
SKINSTATUS	g_SkinStatus={0};		//状态窗口配置
SKINSPCHAR	g_SkinSpChar={0};		//特殊字符窗口配置
COLORCOMP	g_ColorComp={0};		//编码窗口颜色配置
COLORCAND	g_ColorCand={0};		//重码窗口颜色配置

SIMBTNINFO	g_stBtn[STATUS_BUTTONS]=
{//状态栏按钮
	{IDC_VIEWMODE1},
	{IDC_VIEWMODE2},
	{IDC_CHARMODE1},
	{IDC_CHARMODE2},
	{IDC_MAKEWORD},
	{IDC_RECORD1},
	{IDC_RECORD2},
	{IDC_SOUND1	},
	{IDC_SOUND2	},
	{IDC_MENU	},
	{IDC_MYHELP	},
	{IDC_QUERY	},
	{IDC_ENGLISH1},
	{IDC_ENGLISH2},
};

char g_szBtnName[][50]=
{
	"Btn_Status_ViewMode1",
	"Btn_Status_ViewMode2",
	"Btn_Status_CharMode1",
	"Btn_Status_CharMode2",
	"Btn_Staus_MakeWord",
	"Btn_Staus_Record1",
	"Btn_Staus_Record2",
	"Btn_Staus_Sound1",
	"Btn_Staus_Sound2",
	"Btn_Staus_Menu",
	"Btn_Staus_Help",
	"Btn_Staus_Query",
	"Btn_Staus_English1",
	"Btn_Staus_English2",
};

SIMBTNINFO g_iwBtn[INPUT_BUTTONS]=
{
	{IDC_PREVPAGE	},
	{IDC_NEXTPAGE	},
};

SIMBTNINFO	g_spBtn[4]=
{
	{IDC_INSERT},
	{IDC_EXIT},
	{IDC_PREV},
	{IDC_NEXT},
};

void StrReplace(char *pstr,char cOrig,char cNew)
{
	char *p=pstr;
	while(*p)
	{
		if(*p&0x80)
			p+=2;
		else
		{
			if(*p==cOrig)
				*p=cNew;
			p++;
		}
	}
}

void Skin_ParseFontStr(LPCTSTR pszFontInfo,LOGFONT *plf)
{
	char szFontInfo[200]={0};
	char *p=szFontInfo;
	char szName[50],szValue[50];

	strcpy(szFontInfo,pszFontInfo);
	StrReplace(szFontInfo,';',0);

	while(*p)
	{
		StrReplace(p,'=',' ');
		sscanf(p,"%s %s",szName,szValue);
		if(strcmp(szName,"face")==0)
		{
			strcpy(plf->lfFaceName,szValue);
		}else if(strcmp(szName,"size")==0)
		{
			plf->lfHeight=-abs(atoi(szValue));
		}else if(strcmp(szName,"bold")==0)
		{
			plf->lfWeight=atoi(szValue)?700:400;
		}else if(strcmp(szName,"italic")==0)
		{
			plf->lfItalic=atoi(szValue);
		}
		p+=strlen(p)+1;
	}
}

HFONT Skin_LoadFont(LPCTSTR pszIniData)
{
	LOGFONT lf={0};
	char szBuf[200]={0};
	lf.lfCharSet=DEFAULT_CHARSET;
	Helper_GetProfileSectionString("font_name","宋体",lf.lfFaceName,LF_FACESIZE,pszIniData);
	lf.lfHeight=-abs(Helper_GetProfileSectionInt("font_height",12,pszIniData));
	Helper_GetProfileSectionString("font","",szBuf,200,pszIniData);
	if(szBuf[0]) Skin_ParseFontStr(szBuf,&lf);
	return CreateFontIndirect(&lf);
}

COLORREF Skin_LoadColor(LPCTSTR pszEntry,COLORREF crDef,LPCTSTR pszIniData)
{
	int  r,g,b;
	char szBuf[50]={0};
	Helper_GetProfileSectionString(pszEntry,"",szBuf,50,pszIniData);
	if(szBuf[0])
	{
		sscanf(szBuf,"%d,%d,%d",&r,&g,&b);
		return RGB(r,g,b);
	}else
	{
		return crDef;
	}
}

//将PNG格式的背景图转换为位图格式
void Skin_CvtPng2Bmp(PIMAGE pImg,COLORREF crBkgnd)
{
	if(pImg->imgType=IMG_PNG)
	{
		BITMAP bm;
		HBITMAP hBmp,hOldBmp;
		HDC hdc=GetDC(NULL);
		HDC hmemdc=CreateCompatibleDC(hdc);
		PNGBLEND blend={FALSE,crBkgnd};
		GetObject(pImg->hBmp,sizeof(BITMAP),&bm);
		hBmp=CreateCompatibleBitmap(hdc,bm.bmWidth,bm.bmHeight);
		hOldBmp=(HBITMAP)SelectObject(hmemdc,hBmp);
		Bmp32_AlphaBlend(hmemdc,0,0,bm.bmWidth,bm.bmHeight,pImg->hBmp,0,0,blend);
		SelectObject(hmemdc,hOldBmp);
		DeleteDC(hmemdc);
		ReleaseDC(NULL,hdc);
		DeleteObject(pImg->hBmp);
		pImg->hBmp=hBmp;
		pImg->imgType=IMG_BMP;
		pImg->bTransparent=FALSE;
	}
}

void Skin_LoadCompColor(LPCTSTR pszIniData)
{
	g_ColorComp.crNormal=Skin_LoadColor("normal",RGB(0,0,0),pszIniData);
	g_ColorComp.crInput=Skin_LoadColor("input",RGB(128,0,0),pszIniData);
	g_ColorComp.crSentSel=Skin_LoadColor("sent_sel",RGB(255,0,0),pszIniData);
	g_ColorComp.crSentNormal=Skin_LoadColor("sent_normal",RGB(0,0,0),pszIniData);
	g_ColorComp.crSentInit=Skin_LoadColor("sent_init",RGB(100,100,100),pszIniData);
	g_ColorComp.crEnCode=Skin_LoadColor("en_code",RGB(0,0,0),pszIniData);
	g_ColorComp.crEnMakeup=Skin_LoadColor("en_makeup",RGB(255,0,0),pszIniData);
	g_ColorComp.crEnPhonetic=Skin_LoadColor("en_phonetic",RGB(0,0,255),pszIniData);
	g_ColorComp.crSpTmHead=Skin_LoadColor("spell_tmhead",RGB(255,0,0),pszIniData);
	g_ColorComp.crSpResult=Skin_LoadColor("spell_result",RGB(0,0,0),pszIniData);
	g_ColorComp.crSpSpell=Skin_LoadColor("spell_spell",RGB(0,0,255),pszIniData);
	g_ColorComp.crSpEdit=Skin_LoadColor("spell_edit",RGB(255,0,0),pszIniData);
	g_ColorComp.crSpLine=Skin_LoadColor("spell_line",RGB(255,0,0),pszIniData);
	g_ColorComp.crUdHead=Skin_LoadColor("userdef_head",RGB(255,0,0),pszIniData);
	g_ColorComp.crUdCode=Skin_LoadColor("userdef_code",RGB(0,0,0),pszIniData);
	g_ColorComp.crUdMakeup=Skin_LoadColor("userdef_makeup",RGB(0,0,255),pszIniData);
}

void Skin_LoadCandColor(LPCTSTR pszIniData)
{
	g_ColorCand.crIndex=Skin_LoadColor("index",RGB(50,50,50),pszIniData);
	g_ColorCand.crNormal=Skin_LoadColor("normal",RGB(0,0,0),pszIniData);
	g_ColorCand.crAssociation=Skin_LoadColor("association",RGB(0,0,255),pszIniData);
	g_ColorCand.crCompNormal=Skin_LoadColor("comp_nomal",RGB(0,0,255),pszIniData);
	g_ColorCand.crCompWild=Skin_LoadColor("comp_wild",RGB(255,0,0),pszIniData);
	g_ColorCand.crEnHead=Skin_LoadColor("en_head",RGB(128,0,0),pszIniData);
	g_ColorCand.crEnPhonetic=Skin_LoadColor("en_phonetic",RGB(0,0,255),pszIniData);
	g_ColorCand.crTip=Skin_LoadColor("tip",RGB(0,0,0),pszIniData);
	g_ColorCand.crUserDef=Skin_LoadColor("userdef",g_ColorCand.crAssociation,pszIniData);
	g_ColorCand.crMixSp=Skin_LoadColor("mixsp",g_ColorCand.crAssociation,pszIniData);
	g_ColorCand.crInactive=Skin_LoadColor("inactive",RGB(192,192,192),pszIniData);
}

BOOL Skin_LoadSpCharWnd(LPCTSTR pszPath,LPCTSTR pszConfig)
{
	char szFileName[MAX_PATH]={0},szFullPath[MAX_PATH]={0};
	LOGFONT lf={0};
	int i;
	g_SkinSpChar.wndBtn.pBtns=g_spBtn;
	g_SkinSpChar.wndBtn.nCount=4;
	g_SkinSpChar.wndBtn.pCapture=g_SkinSpChar.wndBtn.pFocus=NULL;
	SimBtn_Load(g_spBtn+0,"Btn_Insert",pszPath,pszConfig,TRUE);
	SimBtn_Load(g_spBtn+1,"Btn_Exit",pszPath,pszConfig,TRUE);
	SimBtn_Load(g_spBtn+2,"Btn_Back",pszPath,pszConfig,TRUE);
	SimBtn_Load(g_spBtn+3,"Btn_Next",pszPath,pszConfig,TRUE);
	for(i=0;i<4;i++) g_spBtn[i].bVisible=TRUE;

	GetPrivateProfileString("SpCharWnd","head",NULL,szFileName,MAX_PATH,pszConfig);
	sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
	g_SkinSpChar.hBmpHeader =(HBITMAP)LoadImage(NULL,szFullPath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(!g_SkinSpChar.hBmpHeader) return FALSE;
	GetPrivateProfileString("SpCharWnd","font","宋体",lf.lfFaceName,LF_FACESIZE,pszConfig);
	lf.lfCharSet=DEFAULT_CHARSET;
	lf.lfHeight=GetPrivateProfileInt("SpCharWnd","fontNormal",12,pszConfig);
	g_SkinSpChar.hFont=CreateFontIndirect(&lf);
	lf.lfHeight=GetPrivateProfileInt("SpCharWnd","fontBig",24,pszConfig);
	g_SkinSpChar.hFontBig=CreateFontIndirect(&lf);

	return TRUE;
}


BOOL Skin_LoadInputLayout(INPUTLAYOUT *pLayout,LPCTSTR pszPath,LPCTSTR pszIniData,BOOL bVertical)
{
	char szFileName[MAX_PATH]={0},szFullPath[MAX_PATH]={0},szBuf[100];
	LOGFONT lf={0};
	BOOL bHeteromorphic=FALSE;
	IMGTYPE imgType=Helper_GetProfileSectionInt("imgtype",0,pszIniData);
	
	Helper_GetProfileSectionString("image",NULL,szFileName,MAX_PATH,pszIniData);
	sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
	pLayout->pImgBack=Image_Load(szFullPath,imgType);
	if(!pLayout->pImgBack)
	{
		Helper_Trace("skin error: load input window image error");
		return FALSE;
	}
	pLayout->size=Image_GetSize(pLayout->pImgBack);
	if(bVertical)
	{
		pLayout->nTop=Helper_GetProfileSectionInt("top",10,pszIniData);
		pLayout->nBottom=Helper_GetProfileSectionInt("bottom",10,pszIniData);
	}else
	{
		pLayout->nLeft=Helper_GetProfileSectionInt("left",10,pszIniData);
		pLayout->nRight=Helper_GetProfileSectionInt("right",10,pszIniData);
	}
	pLayout->bTile=Helper_GetProfileSectionInt("tile",1,pszIniData);
	if(pLayout->size.cx<=pLayout->nLeft+pLayout->nRight)
	{
		Helper_Trace("skin error:width settings error");
		return FALSE;
	}
	
	bHeteromorphic=Helper_GetProfileSectionInt("heteromorphic",0,pszIniData);
	if(bHeteromorphic)
	{//获取异形窗口的区域
		HRGN hRgn=0,hRgnBody=0;
		if(imgType==IMG_BMP)
		{
			COLORREF crTrans=Skin_LoadColor("color_trans",RGB(255,0,255),pszIniData);
			hRgn=Image_MakeRgn(pLayout->pImgBack,crTrans);
		}else
		{
			hRgn=Image_MakeRgn(pLayout->pImgBack,30);
			Skin_CvtPng2Bmp(pLayout->pImgBack,Skin_LoadColor("color_trans",RGB(255,255,255),pszIniData));
		}
		if(hRgn)
		{
			HRGN hRgnTemp;
			if(pLayout->nLeft)
			{
				pLayout->hRgnLeft=CreateRectRgn(0,0,0,0);
				if(bVertical)
					hRgnTemp=CreateRectRgn(0,0,pLayout->size.cx,pLayout->nTop);
				else
					hRgnTemp=CreateRectRgn(0,0,pLayout->nLeft,pLayout->size.cy);
				CombineRgn(pLayout->hRgnLeft,hRgn,hRgnTemp,RGN_AND);
				DeleteObject(hRgnTemp);
			}
			if(pLayout->nRight)
			{
				pLayout->hRgnRight=CreateRectRgn(0,0,0,0);
				if(bVertical)
					hRgnTemp=CreateRectRgn(0,pLayout->size.cy-pLayout->nBottom,pLayout->size.cx,pLayout->size.cy);
				else
					hRgnTemp=CreateRectRgn(pLayout->size.cx-pLayout->nRight,0,pLayout->size.cx,pLayout->size.cy);
				CombineRgn(pLayout->hRgnRight,hRgn,hRgnTemp,RGN_AND);
				DeleteObject(hRgnTemp);
			}
			hRgnBody=CreateRectRgn(0,0,0,0);
			if(bVertical)
				hRgnTemp=CreateRectRgn(0,pLayout->nTop,pLayout->size.cx,pLayout->size.cy-pLayout->nBottom);
			else
				hRgnTemp=CreateRectRgn(pLayout->nLeft,0,pLayout->size.cx-pLayout->nRight,pLayout->size.cy);
			CombineRgn(hRgnBody,hRgn,hRgnTemp,RGN_AND);
			DeleteObject(hRgnTemp);
			GetRgnBox(hRgnBody,&pLayout->rcBody);
			hRgnTemp=CreateRectRgnIndirect(&pLayout->rcBody);
			if(!EqualRgn(hRgnTemp,hRgnBody))
			{
				if(bVertical)
					OffsetRgn(hRgnBody,0,-pLayout->nTop);
				else
					OffsetRgn(hRgnBody,-pLayout->nLeft,0);
				pLayout->dwRgnDataLen=GetRegionData(hRgnBody, 0, NULL);
				pLayout->pRgnDataBody=(PRGNDATA)malloc(pLayout->dwRgnDataLen);
				GetRegionData(hRgnBody, pLayout->dwRgnDataLen, pLayout->pRgnDataBody);
			}
			DeleteObject(hRgnBody);
			DeleteObject(hRgnTemp);
			DeleteObject(hRgn);
		}
	}
	Helper_GetProfileSectionString("margin","0,0,0,0",szBuf,100,pszIniData);
	sscanf(szBuf,"%d,%d,%d,%d",&pLayout->rcMargin.left,&pLayout->rcMargin.top,&pLayout->rcMargin.right,&pLayout->rcMargin.bottom);
	Helper_GetProfileSectionString("margin_comp","0,0",szBuf,100,pszIniData);
	sscanf(szBuf,"%d,%d",&pLayout->nCompLeft,&pLayout->nCompRight);
	Helper_GetProfileSectionString("margin_cand","0,0",szBuf,100,pszIniData);
	sscanf(szBuf,"%d,%d",&pLayout->nCandLeft,&pLayout->nCandRight);

	pLayout->nCandInter=Helper_GetProfileSectionInt("cand_inter",5,pszIniData);
	pLayout->nCompHei=Helper_GetProfileSectionInt("comp_hei",0,pszIniData);
	pLayout->nLineHei=Helper_GetProfileSectionInt("line_hei",0,pszIniData);
	if(pLayout->nCompHei==0)
	{//横排窗口没有该参数
		pLayout->nCompHei=(pLayout->size.cy-pLayout->rcMargin.top-pLayout->rcMargin.bottom)/2;
	}
	return TRUE;
}

BOOL Skin_LoadInputWnd(LPCTSTR pszPath,LPCTSTR pszConfig,char *pszIniBuf)
{
	char szFileName[MAX_PATH],szFullPath[MAX_PATH];
	int i;
	if(!GetPrivateProfileSection("InputWindow",pszIniBuf,MAX_SECTION,pszConfig)) return FALSE;
	//载入输入窗口的通用数据
	Helper_GetProfileSectionString("charmode",NULL,szFileName,MAX_PATH,pszIniBuf);
	sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
	g_SkinInput.hBmp32CharMode=Bmp32_LoadPng(szFullPath);
	g_SkinInput.hFont=Skin_LoadFont(pszIniBuf);
	g_SkinInput.wndBtn.pBtns=g_iwBtn;
	g_SkinInput.wndBtn.nCount=INPUT_BUTTONS;
	g_SkinInput.wndBtn.pCapture=g_SkinInput.wndBtn.pFocus=NULL;
	SimBtn_Load(g_iwBtn+0,"Btn_Input_PrevPage",pszPath,pszConfig,FALSE);
	SimBtn_Load(g_iwBtn+1,"Btn_Input_NextPage",pszPath,pszConfig,FALSE);
	for(i=0;i<INPUT_BUTTONS;i++)
	{
		SIZE size=Image_GetSize(g_iwBtn[i].pImg);
		g_iwBtn[i].bVisible=FALSE;
		SetRect(&g_iwBtn[i].rc,0,0,size.cx/3,size.cy);
	}

	if(!Skin_LoadInputLayout(&g_SkinInput.modeHerizontal,pszPath,pszIniBuf,FALSE)) return FALSE;
	//载入纵排窗口的布局信息
	if(GetPrivateProfileSection("InputWindow2",pszIniBuf,MAX_SECTION,pszConfig))
		Skin_LoadInputLayout(&g_SkinInput.modeVertical,pszPath,pszIniBuf,TRUE);
	return TRUE;
}

void Skin_GetBtnRect(LPCTSTR pszConfig,BOOL bFullMode)
{
	int i;
	char szBuf[100];
	for(i=0;i<STATUS_BUTTONS;i++)
	{
		if(bFullMode)
			GetPrivateProfileString(g_szBtnName[i],"rect2","",szBuf,100,pszConfig);
		if(!bFullMode || szBuf[0]==0)
			GetPrivateProfileString(g_szBtnName[i],"rect","",szBuf,100,pszConfig);
		if(szBuf[0])
			sscanf(szBuf,"%d,%d,%d,%d",&g_stBtn[i].rc.left,&g_stBtn[i].rc.top,&g_stBtn[i].rc.right,&g_stBtn[i].rc.bottom);
	}
}

BOOL Skin_LoadStatusWnd(LPCTSTR pszPath,LPCTSTR pszConfig,char * pszIniData)
{
	char szFileName[MAX_PATH]={0},szFullPath[MAX_PATH]={0};
	char szRect[50];
	LOGFONT lf={0};
	int i;
	IMGTYPE imgType;
	BOOL bHeteromorphic;
	g_SkinStatus.wndBtn.pBtns=g_stBtn;
	g_SkinStatus.wndBtn.nCount=STATUS_BUTTONS;
	g_SkinStatus.wndBtn.pCapture=g_SkinStatus.wndBtn.pFocus=NULL;
	for(i=0;i<STATUS_BUTTONS;i++)
	{
		if(!SimBtn_Load(g_stBtn+i,g_szBtnName[i],pszPath,pszConfig,FALSE))
		{
			Helper_Trace("skin warning: button %d config infomation missed",i);
		}
		g_stBtn[i].bVisible=TRUE;
	}
	if(!GetPrivateProfileSection("StatusWindow",pszIniData,MAX_SECTION,pszConfig)) return FALSE;

	imgType=Helper_GetProfileSectionInt("imgtype",0,pszIniData);
	Helper_GetProfileSectionString("imageShrink",NULL,szFileName,MAX_PATH,pszIniData);
	sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
	g_SkinStatus.pImgBackShrink=Image_Load(szFullPath,imgType);
	if(!g_SkinStatus.pImgBackShrink)
	{
		Helper_Trace("skin load:status back shrink");
		return FALSE;
	}
	g_SkinStatus.sizeShrink=Image_GetSize(g_SkinStatus.pImgBackShrink);
	Helper_GetProfileSectionString("imageFull",NULL,szFileName,MAX_PATH,pszIniData);
	sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
	g_SkinStatus.pImgBackFull=Image_Load(szFullPath,imgType);
	if(!g_SkinStatus.pImgBackFull)
	{
		Helper_Trace("skin load:status back full");
		return FALSE;
	}

	g_SkinStatus.sizeFull=Image_GetSize(g_SkinStatus.pImgBackFull);

	bHeteromorphic=Helper_GetProfileSectionInt("heteromorphic",0,pszIniData);
	if(bHeteromorphic)
	{//获取异形窗口的区域
		if(imgType==IMG_BMP)
		{
			COLORREF crTrans=Skin_LoadColor("color_trans",RGB(255,0,255),pszIniData);
			g_SkinStatus.hRgnShrink=Image_MakeRgn(g_SkinStatus.pImgBackShrink,crTrans);
			g_SkinStatus.hRgnFull=Image_MakeRgn(g_SkinStatus.pImgBackFull,crTrans);			
		}else
		{
			COLORREF crBlend=Skin_LoadColor("color_trans",RGB(255,255,255),pszIniData);
			g_SkinStatus.hRgnShrink=Image_MakeRgn(g_SkinStatus.pImgBackShrink,30);
			g_SkinStatus.hRgnFull=Image_MakeRgn(g_SkinStatus.pImgBackFull,30);
			Skin_CvtPng2Bmp(g_SkinStatus.pImgBackShrink,crBlend);
			Skin_CvtPng2Bmp(g_SkinStatus.pImgBackFull,crBlend);
		}
	}

	Helper_GetProfileSectionString("rect_text",NULL,szRect,50,pszIniData);
	sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcText.left,&g_SkinStatus.rcText.top,&g_SkinStatus.rcText.right,&g_SkinStatus.rcText.bottom);
	Helper_GetProfileSectionString("rect_icon",NULL,szRect,50,pszIniData);
	sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcIcon.left,&g_SkinStatus.rcIcon.top,&g_SkinStatus.rcIcon.right,&g_SkinStatus.rcIcon.bottom);
	if(Helper_GetProfileSectionString("rect_drag",NULL,szRect,50,pszIniData))
		sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcDrag.left,&g_SkinStatus.rcDrag.top,&g_SkinStatus.rcDrag.right,&g_SkinStatus.rcDrag.bottom);
	else
		g_SkinStatus.rcDrag=g_SkinStatus.rcText;

	if(Helper_GetProfileSectionString("rect_text2",NULL,szRect,50,pszIniData))
		sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcText2.left,&g_SkinStatus.rcText2.top,&g_SkinStatus.rcText2.right,&g_SkinStatus.rcText2.bottom);
	else
		g_SkinStatus.rcText2=g_SkinStatus.rcText;

	if(Helper_GetProfileSectionString("rect_icon2",NULL,szRect,50,pszIniData))
		sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcIcon2.left,&g_SkinStatus.rcIcon2.top,&g_SkinStatus.rcIcon2.right,&g_SkinStatus.rcIcon2.bottom);
	else
		g_SkinStatus.rcIcon2=g_SkinStatus.rcIcon;
	
	if(Helper_GetProfileSectionString("rect_drag2",NULL,szRect,50,pszIniData))
		sscanf(szRect,"%d,%d,%d,%d",&g_SkinStatus.rcDrag2.left,&g_SkinStatus.rcDrag2.top,&g_SkinStatus.rcDrag2.right,&g_SkinStatus.rcDrag2.bottom);
	else
		g_SkinStatus.rcDrag2=g_SkinStatus.rcDrag;

	g_SkinStatus.hFont=Skin_LoadFont(pszIniData);
	GetObject(g_SkinStatus.hFont,sizeof(LOGFONT),&lf);
	lf.lfEscapement=2700;
	g_SkinStatus.hFont2=CreateFontIndirect(&lf);

	g_SkinStatus.crText=Skin_LoadColor("color_text",RGB(0,0,0),pszIniData);

	if(Helper_GetProfileSectionString("flag_icon",NULL,szFileName,MAX_PATH,pszIniData))
	{//加载皮肤定义的图标
		HBITMAP hBmp=NULL;
		COLORREF crFlagMask=Skin_LoadColor("flag_mask",RGB(255,0,255),pszIniData);
		sprintf(szFullPath,"%s\\%s",pszPath,szFileName);
		hBmp=(HBITMAP)LoadImage(NULL,szFullPath,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(hBmp)
		{
			g_SkinStatus.hIconList=ImageList_Create(16,16,ILC_COLORDDB|ILC_MASK,0,0);
			ImageList_AddMasked(g_SkinStatus.hIconList,hBmp,crFlagMask);
			DeleteObject(hBmp);
		}
	}
	return TRUE;
}

void Skin_FreeLayout(INPUTLAYOUT *pLayout)
{
	if(pLayout->pImgBack) Image_Free(pLayout->pImgBack);
	pLayout->pImgBack=NULL;
	if(pLayout->hRgnLeft) DeleteObject(pLayout->hRgnLeft);
	pLayout->hRgnLeft=NULL;
	if(pLayout->hRgnRight) DeleteObject(pLayout->hRgnRight);
	pLayout->hRgnRight=NULL;
	if(pLayout->pRgnDataBody) free(pLayout->pRgnDataBody);
	pLayout->pRgnDataBody=NULL;
	pLayout->dwRgnDataLen=0;
}

//释放当前皮肤资源
void Skin_Free()
{
	int i;
	//input
	Skin_FreeLayout(&g_SkinInput.modeHerizontal);
	Skin_FreeLayout(&g_SkinInput.modeVertical);
	if(g_SkinInput.hBmp32CharMode) DeleteObject(g_SkinInput.hBmp32CharMode);
	g_SkinInput.hBmp32CharMode=NULL;
	if(g_SkinInput.hFont) DeleteObject(g_SkinInput.hFont);
	g_SkinInput.hFont=NULL;
	
	//status
	if(g_SkinStatus.pImgBackShrink) Image_Free(g_SkinStatus.pImgBackShrink);
	g_SkinStatus.pImgBackShrink=NULL;
	if(g_SkinStatus.pImgBackFull) Image_Free(g_SkinStatus.pImgBackFull);
	g_SkinStatus.pImgBackFull=NULL;
	if(g_SkinStatus.hRgnFull) DeleteObject(g_SkinStatus.hRgnFull);
	g_SkinStatus.hRgnFull=NULL;
	if(g_SkinStatus.hRgnShrink) DeleteObject(g_SkinStatus.hRgnShrink);
	g_SkinStatus.hRgnShrink=NULL;
	if(g_SkinStatus.hFont) DeleteObject(g_SkinStatus.hFont);
	g_SkinStatus.hFont=NULL;
	if(g_SkinStatus.hFont2) DeleteObject(g_SkinStatus.hFont2);
	g_SkinStatus.hFont2=NULL;
	if(g_SkinStatus.hIconList) ImageList_Destroy(g_SkinStatus.hIconList);
	g_SkinStatus.hIconList=NULL;

	for(i=0;i<STATUS_BUTTONS;i++)
	{
		if(g_stBtn[i].pImg)
		{
			Image_Free(g_stBtn[i].pImg);
			g_stBtn[i].pImg=NULL;
		}
		SetRect(&g_stBtn[i].rc,0,0,0,0);
		g_stBtn[i].bVisible=FALSE;
	}
	for(i=0;i<INPUT_BUTTONS;i++)
	{
		if(g_iwBtn[i].pImg)
		{
			Image_Free(g_iwBtn[i].pImg);
			g_iwBtn[i].pImg=NULL;
		}
		SetRect(&g_iwBtn[i].rc,0,0,0,0);
		g_iwBtn[i].bVisible=FALSE;
	}
}

void Skin_FreeSpCharWnd()
{
	int i;
	if(g_SkinSpChar.hBmpHeader) DeleteObject(g_SkinSpChar.hBmpHeader);
	g_SkinSpChar.hBmpHeader=NULL;
	if(g_SkinSpChar.hFont) DeleteObject(g_SkinSpChar.hFont);
	g_SkinSpChar.hFont=NULL;
	if(g_SkinSpChar.hFontBig) DeleteObject(g_SkinSpChar.hFontBig);
	g_SkinSpChar.hFontBig=NULL;
	for(i=0;i<4;i++)
	{
		if(g_spBtn[i].pImg)
		{
			Image_Free(g_spBtn[i].pImg);
			g_spBtn[i].pImg=NULL;
		}
	}
}

//载入一个皮肤资源
BOOL Skin_Load(LPCTSTR pszSkinName)
{
	char szPath[MAX_PATH],szConfig[MAX_PATH];
	char szIniData[MAX_SECTION];
	Skin_Free();
	sprintf(szPath,"%s\\Skins\\%s",g_szDataPath,pszSkinName);
	sprintf(szConfig,"%s\\config.ini",szPath);

	if(!Skin_LoadInputWnd(szPath,szConfig,szIniData)) return FALSE;
	if(!Skin_LoadStatusWnd(szPath,szConfig,szIniData)) return FALSE;
	if(GetPrivateProfileSection("Color_Comp",szIniData,MAX_SECTION,szConfig))
		Skin_LoadCompColor(szIniData);
	if(GetPrivateProfileSection("Color_Cand",szIniData,MAX_SECTION,szConfig))
		Skin_LoadCandColor(szIniData);
	strcpy(g_SkinStatus.szSkinName,pszSkinName);
	return TRUE;
}

int Skin_Enum(char szSkins[][MAX_PATH],int nSize,LPCTSTR pszDataPath)
{
	char szPath[MAX_PATH];
	HANDLE hFindFile=NULL;
	WIN32_FIND_DATA wfd;
	int nSkins=0;
	sprintf(szPath,"%s\\skins\\*.*",pszDataPath);
	hFindFile=FindFirstFile(szPath,&wfd);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		char szConfig[MAX_PATH];
		char szName[50],szAuthor[50];
		do
		{
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY 
				&& strcmp(wfd.cFileName,".")!=0
				&& strcmp(wfd.cFileName,"..")!=0)
			{
				sprintf(szConfig,"%s\\Skins\\%s\\config.ini",pszDataPath,wfd.cFileName);
				if(GetPrivateProfileString("desc","name","",szName,50,szConfig)
					&& GetPrivateProfileString("desc","author","",szAuthor,50,szConfig))
				{

					strcpy(szSkins[nSkins],wfd.cFileName);
					if(nSkins>=nSize-1) break;
					nSkins++;
				}
			}
		}while(FindNextFile(hFindFile,&wfd));
		FindClose(hFindFile);
	}
	return nSkins;
}