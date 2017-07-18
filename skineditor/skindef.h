#ifndef _SKINDEFINE_
#define _SKINDEFINE_

enum{
	SDID_NAME=1,
	SDID_AUTHOR
};

typedef struct tagSKINDESC
{
	char szName[100];
	char szAuthor[100];
}SKINDESC;

enum{
	SSID_BGIMGFULL=1000,
	SSID_BGIMGSHRINK,
	SSID_HETEROMORPHIC,
	SSID_TRANS,

	SSID_ICON,
	SSID_ICON2,
	SSID_TEXT,
	SSID_TEXT2,
	SSID_DRAG,
	SSID_DRAG2,

	SSID_FONT,
	SSID_TEXTCOLOR,
	SSID_ICONIMG,
	SSID_ICONIMGTRANS,

	SSID_BTNSHRINK,
	SSID_BTNSHRINKR,
	SSID_BTNSHRINKRF,
	SSID_BTNSHRINKT,
	SSID_BTNEXTENT,
	SSID_BTNEXTENTR,
	SSID_BTNEXTENTRF,
	SSID_BTNEXTENTT,

	SSID_BTNCHAR1,
	SSID_BTNCHAR1R,
	SSID_BTNCHAR1RF,
	SSID_BTNCHAR1T,
	SSID_BTNCHAR2,
	SSID_BTNCHAR2R,
	SSID_BTNCHAR2RF,
	SSID_BTNCHAR2T,
	SSID_BTNRECORD1,
	SSID_BTNRECORD1R,
	SSID_BTNRECORD1RF,
	SSID_BTNRECORD1T,
	SSID_BTNRECORD2,
	SSID_BTNRECORD2R,
	SSID_BTNRECORD2RF,
	SSID_BTNRECORD2T,
	SSID_BTNREAD1,
	SSID_BTNREAD1R,
	SSID_BTNREAD1RF,
	SSID_BTNREAD1T,
	SSID_BTNREAD2,
	SSID_BTNREAD2R,
	SSID_BTNREAD2RF,
	SSID_BTNREAD2T,
	SSID_BTNWORD1,
	SSID_BTNWORD1R,
	SSID_BTNWORD1RF,
	SSID_BTNWORD1T,
	SSID_BTNWORD2,
	SSID_BTNWORD2R,
	SSID_BTNWORD2RF,
	SSID_BTNWORD2T,
	SSID_BTNQUERY,
	SSID_BTNQUERYR,
	SSID_BTNQUERYRF,
	SSID_BTNQUERYT,
	SSID_BTNMAKEPHRASE,
	SSID_BTNMAKEPHRASER,
	SSID_BTNMAKEPHRASERF,
	SSID_BTNMAKEPHRASET,
	SSID_BTNMENU,
	SSID_BTNMENUR,
	SSID_BTNMENURF,
	SSID_BTNMENUT,
	SSID_BTNHELP,
	SSID_BTNHELPR,
	SSID_BTNHELPRF,
	SSID_BTNHELPT,
};

typedef struct tagBTNINFO
{
	char szFile[MAX_PATH];//ͼƬ�ļ���,ֻ֧����չ��ΪPNG��BMP���ļ�
	RECT rcShrink;	//����״̬�µİ�ťλ��
	RECT rcFull;	//����״̬�µİ�ťλ��
	char szTip[100];//��ť��ʾ��
}BTNINFO;

typedef struct tagSKINSTATUS
{
	char		szBgImgFull[MAX_PATH];
	char		szBgImgShrink[MAX_PATH];
	BOOL		bHeteroMorphic;
	COLORREF	crTrans;
	RECT		rcIcon;			//�����־��(����)
	RECT		rcIcon2;		//�����־��(����)
	RECT		rcText;			//������(����)
	RECT		rcText2;		//������(����)
	RECT		rcDrag;			//�����϶���(����)
	RECT		rcDrag2;		//�����϶���(����)
	LOGFONT		lf;				//���嶨����Ϣ
	COLORREF	crText;			//������ɫ
	char		szIcon[MAX_PATH];
	COLORREF	crIconTrans;

	BTNINFO		btnExtent;
	BTNINFO		btnShrink;
	BTNINFO		btnChar1;
	BTNINFO		btnChar2;
	BTNINFO		btnRecord1;
	BTNINFO		btnRecord2;
	BTNINFO		btnRead1;
	BTNINFO		btnRead2;
	BTNINFO		btnWord1;
	BTNINFO		btnWord2;
	BTNINFO		btnQuery;
	BTNINFO		btnMakePhrase;
	BTNINFO		btnMenu;
	BTNINFO		btnHelp;

}SKINSTATUS,*PSKINSTATUS,FAR *LPSKINSTATUS;

enum {
	SIID_FONT=2000,
	SIID_BTNPREV,
	SIID_BTNNEXT,
	SIID_SYMBOL,
};

enum{
	SIID_BGIMG=2100,
	SIID_TILE,
	SIID_HETEROMORPHIC,
	SIID_TRANS,
	SIID_LEFT,
	SIID_RIGHT,
	SIID_MARGIN,
	SIID_COMPMARGIN,
	SIID_CANDMARGIN,
	SIID_CANDINTER,
	SIID_COMPHEI,
	SIID_LINEHEI,
};

enum{
	SIIDV_BGIMG=2200,
	SIIDV_TILE,
	SIIDV_HETEROMORPHIC,
	SIIDV_TRANS,
	SIIDV_LEFT,
	SIIDV_RIGHT,
	SIIDV_MARGIN,
	SIIDV_COMPMARGIN,
	SIIDV_CANDMARGIN,
	SIIDV_CANDINTER,
	SIIDV_COMPHEI,
	SIIDV_LINEHEI,
};

typedef struct tagINPUTLAYOUT
{
	char		szBgImg[MAX_PATH];
	BOOL		bTile;
	BOOL		bHeteroMorphic;	
	COLORREF	crTrans;
	union{
		int		nLeft;			//��߿���
		int		nTop;			//�ϱ߸߶�
	};
	union{
		int		nRight;			//�ұ߿���
		int		nBottom;		//�±߸߶�
	};
	RECT		rcMargin;		//4������ı߾�
	int			nCompLeft;		//���봰����߸��ӿհ�
	int			nCompRight;		//���봰���ұ߸��ӿհ�
	int			nCandLeft;		//���봰����߸��ӿհ�
	int			nCandRight;		//���봰���ұ߸��ӿհ�
	int			nCandInter;		//������
	int			nCompHei;		//�������߶�
	int			nLineHei;		//�ָ��߸߶�
}INPUTLAYOUT,*PINPUTLAYOUT;

typedef struct tagSKININPUT
{
	//���봰�ڵ�ͨ����Ϣ
	LOGFONT		lf;						//���嶨����Ϣ
	char		szBtnPrev[MAX_PATH];	//��ǰ��ҳ��ť
	char		szBtnNext[MAX_PATH];	//���ҳ��ť
	char		szSymbolMode[MAX_PATH];//���ģʽ��ʾ��PNGͼ
	//����ģʽ��ʹ�õ�������Ϣ
	INPUTLAYOUT loHorz,loVert;
}SKININPUT,*PSKININPUT,FAR *LPSKININPUT;


enum{
	CCID_NORMAL=3000,
	CCID_INPUT,
	CCID_SENTINIT,
	CCID_SENTNORMAL,
	CCID_SENTSEL,
	CCID_ENCODE,
	CCID_ENMAKEUP,
	CCID_ENPHONETIC,
	CCID_SPTMHEAD,
	CCID_SPRESULT,
	CCID_SPSPELL,
	CCID_SPEDIT,
	CCID_SPLINE,
	CCID_UDHEAD,
	CCID_UDCODE,
	CCID_UDMAKEUP,
};

typedef struct tagCOLORCOMP
{
	COLORREF crNormal;
	COLORREF crInput;
	COLORREF crSentInit;
	COLORREF crSentNormal;
	COLORREF crSentSel;
	COLORREF crEnCode;
	COLORREF crEnMakeup;
	COLORREF crEnPhonetic;
	COLORREF crSpTmHead;	
	COLORREF crSpResult;
	COLORREF crSpSpell;
	COLORREF crSpEdit;
	COLORREF crSpLine;
	COLORREF crUdHead;
	COLORREF crUdCode;
	COLORREF crUdMakeup;
}COLORCOMP;

enum{
	CCAID_INDEX=4000,
	CCAID_NORMAL,
	CCAID_FORECAST,
	CCAID_COMPNORMAL,
	CCAID_COMPWILD,
	CCAID_ENHEAD,
	CCAID_ENPHONETIC,
	CCAID_TIP,
	CCAID_INACTIVE,
	CCAID_USERDEF,
	CCAID_MIXSP,
};
typedef struct tagCOLORCAND
{
	COLORREF crIndex;
	COLORREF crNormal;
	COLORREF crForecast;
	COLORREF crCompNormal;
	COLORREF crCompWild;
	COLORREF crEnHead;
	COLORREF crEnPhonetic;
	COLORREF crTip;
	COLORREF crInactive;
	COLORREF crUserDef;
	COLORREF crMixSp;
}COLORCAND;


#endif//_SKINDEFINE