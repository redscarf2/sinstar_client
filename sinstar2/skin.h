#pragma once

#include <commctrl.h>	//HIMAGELIST������Ҫ
#include "image.h"
#include "simbutton.h"

typedef struct tagINPUTLAYOUT
{
	PIMAGE		pImgBack;		//����ͼ
	SIZE		size;			//����ͼ��С
	union{
		int		nLeft;			//��߿��
		int		nTop;			//�ϱ߸߶�
	};
	union{
		int		nRight;			//�ұ߿��
		int		nBottom;		//�±߸߶�
	};
	BOOL		bTile;			//ƽ��ģʽ
	HRGN		hRgnLeft;		//����ģʽ�´��ڵ���ߵ�HRGN
	HRGN		hRgnRight;		//����ģʽ�´��ڵ��ұߵ�HRGN
	PRGNDATA	pRgnDataBody;	//����ģʽ�´��ڵ��м䲿�ֵ�HRGN����,ֻ���м䲿�ֲ�����������ʹ��
	DWORD		dwRgnDataLen;
	RECT		rcBody;			//�м�����ľ���
	RECT		rcMargin;		//4������ı߾�
	int			nCandInter;		//������
	int			nCompHei;		//�������߶�
	int			nLineHei;		//�ָ��߸߶�
	int			nCompLeft;		//���봰����߸��ӿհ�
	int			nCompRight;		//���봰���ұ߸��ӿհ�
	int			nCandLeft;		//���봰����߸��ӿհ�
	int			nCandRight;		//���봰���ұ߸��ӿհ�
}INPUTLAYOUT,*PINPUTLAYOUT;

typedef struct tagSKININPUT
{
	HFONT		hFont;			//��ʾ����
	WNDBTN		wndBtn;			//input window buttons:prevpage,nextpage
// 	PNGINFO	 *	hBmp32CharMode;	//���ģʽ��ʾ��PNGͼ
	HBITMAP		hBmp32CharMode;	//���ģʽ��ʾ��32λͼ
	INPUTLAYOUT   modeHerizontal;
	INPUTLAYOUT	  modeVertical;
}SKINIINPUT,*PSKININPUT,FAR *LPSKININPUT;

typedef struct tagCOLORCOMP
{
	COLORREF crNormal;
	COLORREF crInput;
	COLORREF crSentSel;
	COLORREF crSentNormal;
	COLORREF crSentInit;
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

typedef struct tagCOLORCAND
{
	COLORREF crIndex;
	COLORREF crNormal;
	COLORREF crAssociation;
	COLORREF crCompNormal;
	COLORREF crCompWild;
	COLORREF crEnHead;
	COLORREF crEnPhonetic;
	COLORREF crTip;
	COLORREF crInactive;
	COLORREF crUserDef;
	COLORREF crMixSp;
}COLORCAND;

typedef struct tagSKINSTATUS
{
	char		szSkinName[50];
	PIMAGE		pImgBackShrink;	//����ͼ�ر�״̬
	HRGN		hRgnShrink;
	SIZE		sizeShrink;
	PIMAGE		pImgBackFull;	//����ͼ����״̬
	HRGN		hRgnFull;
	SIZE		sizeFull;
	RECT		rcIcon;			//�����־��(����)
	RECT		rcIcon2;		//�����־��(����)
	RECT		rcText;			//������(����)
	RECT		rcText2;		//������(����)
	RECT		rcDrag;			//�����϶���(����)
	RECT		rcDrag2;		//�����϶���(����)
	HFONT		hFont;			//��ʾ����
	HFONT		hFont2;			//��������ʱʹ�õ�����
	COLORREF	crText;			//������ɫ
	WNDBTN		wndBtn;			//״̬����ť

	HIMAGELIST  hIconList;		//ͼ���б�:����������ƴ����������*������ͼ��(����û�У�û��ʱʹ��Ĭ��ͼ��)
}SKINSTATUS,*PSKINSTATUS,FAR *LPSKINSTATUS;

typedef struct tagSKINSPCHAR
{
	HFONT	hFont;
	HFONT	hFontBig;
	HBITMAP hBmpHeader;
	WNDBTN	wndBtn;		//�����ַ����ڰ�ť
}SKINSPCHAR,FAR *LPSKINSPCHAR;

extern SKINIINPUT	g_SkinInput;
extern SKINSTATUS	g_SkinStatus;
extern SKINSPCHAR	g_SkinSpChar;
extern COLORCOMP	g_ColorComp;		//���봰����ɫ����
extern COLORCAND	g_ColorCand;		//���봰����ɫ����
void Skin_GetBtnRect(LPCTSTR pszConfig,BOOL bFullMode);
BOOL Skin_Load(LPCTSTR pszSkinName);
void Skin_Free();
BOOL Skin_LoadSpCharWnd(LPCTSTR pszPath,LPCTSTR pszConfig);
void Skin_FreeSpCharWnd();
int Skin_Enum(char szSkins[][MAX_PATH],int nSize,LPCTSTR pszSkinPath);

