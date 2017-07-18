
#pragma once
#pragma warning(disable : 4996 4761)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef WINVER
#undef  WINVER
#endif // WINVER
#define WINVER 0x040A

#define NOIME
#include <windows.h>
#include "imm.h"
#pragma comment(lib,"imm32.lib")

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <crtdbg.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#include "resource.h"
#include "skin.h"
#include "tooltip.h"
#include "optip.h"
#include "uispchar.h"
#include "config.h"
#include "SoPlugin.h"

#include "..\include\isProtocol.h"
#include "..\include\iscomm.h"
#include "..\include\helper.h"
#include "..\include\htmlhelp.h"

#define CS_INPUTSTAR			(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS)
#define GCS_INPUT				(GCS_COMPREAD|GCS_COMP|GCS_CURSORPOS|GCS_DELTASTART|GCS_RESULTREAD|GCS_RESULT)
//��������
#define CLSNAME_UI			_T("ISUI")		//UI
#define CLSNAME_STATUS		_T("ISSTATUS")	//״̬��
#define CLSNAME_INPUT		_T("ISINPUT")	//���봰��
#define CLSNAME_SPCHAR		_T("ISSPCHAR")	//�����ַ�����

//�������ݰ���2���֣�
//1 ���봮���������Ǵ����룬ƴ������Ԥ����+��ǰ�ڱ༭����ƴ��=MAX_SENTLEN+7
//2 �����: ����봮�����ڴ�
#define MAX_COMP                120
#define MAX_SYLLABLES			(MAX_SENTLEN/2)
#define MAX_INPUT				255 // >(MAX_SENTLEN+7) && >6*MAX_SYLLABLES && UD_MAXCAND

#define UM_CALCSIZE	(WM_USER+10)

#define IMN_PRIV_UPDATEINPUTWND		1		//�������봰�ڴ�С
#define IMN_PRIV_INVALIDINPUTWND	(1<<1)
#define IMN_PRIV_SKINCHANGE			(1<<2)	//Ƥ��������Ϣ
#define IMN_PRIV_SETDELAY			(1<<3)
#define IMN_PRIV_KILLDELAY			(1<<4)
#define IMN_PRIV_COMMAND			(1<<5)
#define IMN_PRIV_SHOWSPCHAR			(1<<6)
#define IMN_PRIV_HIDESPCHAR			(1<<7)
#define IMN_PRIV_OPENINPUTWND		(1<<8)
#define IMN_PRIV_CLOSEINPUTWND		(1<<9)
#define IMN_PRIV_UPDATESTATUS		(1<<10)	//����״̬������
#define IMN_PRIV_SAVEUICFG			(1<<11)	//���洰�ڽ�����Ϣ

#define IMN_PRIV_PLUGINCLOSE		(1<<15)	//����˳���Ϣ

#define SKIN_RANDOM		"\\Rand_Skin\\"

//״̬����ťID
#define IDC_BTN_BASE	100

#define IDC_PREVPAGE		(IDC_BTN_BASE+0)
#define IDC_NEXTPAGE		(IDC_BTN_BASE+1)

#define INPUT_BUTTONS		2

#define IDC_VIEWMODE1	(IDC_BTN_BASE+0)
#define IDC_VIEWMODE2	(IDC_BTN_BASE+1)
#define IDC_CHARMODE1	(IDC_BTN_BASE+2)
#define IDC_CHARMODE2	(IDC_BTN_BASE+3)
#define IDC_MAKEWORD	(IDC_BTN_BASE+4)
#define IDC_RECORD1		(IDC_BTN_BASE+5)
#define IDC_RECORD2		(IDC_BTN_BASE+6)
#define IDC_SOUND1		(IDC_BTN_BASE+7)
#define IDC_SOUND2		(IDC_BTN_BASE+8)
#define IDC_MENU		(IDC_BTN_BASE+9)
#define IDC_MYHELP		(IDC_BTN_BASE+10)
#define	IDC_QUERY		(IDC_BTN_BASE+11)
#define IDC_ENGLISH1	(IDC_BTN_BASE+12)
#define IDC_ENGLISH2	(IDC_BTN_BASE+13)


#define STATUS_BUTTONS	14

typedef enum tagCOMPMODE
{
 IM_SPELL=0,	//ƴ������ģʽ
 IM_SHAPECODE,	//��������ģʽ
}COMPMODE;

typedef enum tagASTMODE
{//����ʱ����������
	AST_NULL=0,	//������
	AST_CAND,	//��������
	AST_ENGLISH,//��������
	AST_PHRASEREMIND,//������ʾ
}ASTMODE;

typedef struct tagSETTINGSG
{
	BOOL	b23CandKey;			// ;+'ѡ23����
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// ���뷨����
	BYTE	byTempSpellKey;		// ��ʱƴ������
	BYTE	byForecast;			// forecast mask
	BYTE	byLineKey[6];		// �ʻ����뷨ת����ֵ
	BYTE	byPntcRed;			// ������ɫ Red
	BYTE	byPntcGreen;		// ������ɫ Green
	BYTE	byPntcBlue;			// ������ɫ Blue
	BYTE	byTransStatus;		// ״̬����͸���� 50~255
	BYTE    byTransInput;		// ���봰��͸����50~255
	char	cPageCands;			// һ����������
	BOOL	bFastUMode;			// ����Զ���ģʽ�л�
	BOOL	bShowOpTip;			// ��ʾ������ʾ����
	char    szPhonticLeft[50];	// ����������
	char    szPhonticRight[50];	// ����������
	int		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// ��Ƶ�������� 0-��ʹ�ã��������ܵ�Ƶ���������ٵ�Ƶ
	BYTE	byAstMode;			// �������� (ASTMODE)
	BOOL	bAstSent;			// �������
	BOOL	bAutoMatch;			// ����ѡ��
	BOOL	bTTSInput;			// �����϶�
	
	BOOL	bBlendUD;			// ��ϼ����Զ�������
	BOOL	bBlendSpWord;		// �������ƴ������
	BOOL	bAutoInput;			// �����Զ�����

	BYTE	byTurnPageUpVK;		// �����Ϸ���
	BYTE	byTurnPageDownVK;	// �����·���

	BYTE	byHotKeyQuery;		// ��ѯ�ȼ�
	BYTE	byHotKeyMode;		// ���ƴ��ģʽ�л�
	BYTE	byHotKeyEn;			// Ӣ�����뿪��
	BYTE	byHotKeyMakeWord;	// ����ȼ�
	BYTE	byHotKeyShowRoot;	// �ָ�����ʾ�ȼ�
	BYTE	byHotKeyHideStatus;	// ����״̬��

	BOOL	bPYPhraseFirst;		// ƴ����������
	BOOL	bEnterClear;		// �س���ձ���
	int		nSoundAlert;		// ������ʾ
	BOOL	bDisableFnKey;		// ���ù��ܼ�
	BOOL	bAutoDot;			// ����С����
	BOOL	bAutoPrompt;		// ����������ʾ
	BOOL	bDisableDelWordCand;// ��ֹɾ����������
	char	szWebHeader[100];	// �Զ�תΪӢ������ģʽ��̧ͷ

	BOOL	bCandSelNoNum;		// ��������ѡ������
	BOOL	bOnlySimpleCode;	// ���򲻳�ȫ
	BYTE	byFadeSteps;		// ���봰�ڽ��Խ����ٶȣ�0����رոù���
	BOOL	bIptWndVertical;	// ���Ŵ��ڱ�־
	char	szPlugin[100];		// plugin name
	BOOL	bShadeInput,bShadeStatus;//shade
}SETTINGSG;

typedef struct tagSETTINGSL
{
	BOOL	bHideStatus;		// ��ǰ״̬������״̬
	BOOL	bMouseFollow;		// �����濪��
	BOOL	bEnglish;			// Ӣ�ĵ������뿪��
	BOOL	bFullStatus;		// ״̬��չ����־
	BOOL	bCharMode;			// ���ģʽ
	BOOL	bSound;				// ������־
	BOOL	bRecord;			// ��¼�������
	BOOL	bInputBig5;			// ���������־
}SETTINGSL;

typedef struct tagUIPRIV {      // IME private UI data
    HWND    hStatusWnd;         // status window
	POINT	ptStatusWnd;		// ״̬������λ��
    HWND    hInputWnd;          // ���봰��
	POINT	ptInputWnd;			// ���봰��λ��
	HWND	hSpCharWnd;			// �����ַ�����
	POINT	ptSpCharWnd;		// �����ַ�����λ��
}UIPRIV;

typedef UIPRIV      *PUIPRIV;
typedef UIPRIV FAR  *LPUIPRIV;

typedef struct tagSPELLINFO
{
	char szSpell[7];	//ƴ��
	BYTE bySpellLen;	//ƴ������
}SPELLINFO,*PSPELLINFO,FAR *LPSPELLINFO;

typedef enum tagINSTATE
{
	INST_CODING=0,	//��������״̬
	INST_ENGLISH,	//Ӣ�ĵ�������״̬
	INST_DIGITAL,	//��������״̬
	INST_USERDEF,	//�û��Զ�������״̬
	INST_LINEIME,	//�ʻ����뷨״̬
}INSTATE;

typedef enum tagSUBSTATE
{
	SBST_NORMAL=0,	//һ��״̬
	SBST_ASSOCIATE,	//������Ϣ
	SBST_SENTENCE,	//�������״̬
}SUBSTATE;

typedef struct tagCONTEXTPRIV
{
	char    szComp[MAX_COMP];		//����ı�������
	char	cCompLen;				//����ı��볤��
	char	szInput[MAX_INPUT];			//��ǰ��������,UD�ĳ��ȿ��ܳ���250
	int		nInputLen;				//��������ݵĳ���

	INSTATE	inState;				//��ǰ����״̬
	SUBSTATE sbState;				//������״̬
	BOOL	bWebMode;				//��ַ����ģʽ
//	LPBYTE  pbyData;				//������
	//��������
	short	sCurCandPage;			//��ǰ����ҳ
	short   sCandCount;				//������
	LPBYTE  *ppbyCandInfo;			//�������ݱ�
	//��ʾλ��
	RECT	rcCand[10];				//�����ڴ��ڵ�λ��
	RECT	rcHeader;				//Ӣ������Ĵ�ͷ��ʾλ��
	RECT	rcTip;					//��ʾ��Ϣλ��
	RECT	rcComp;					//������ʾλ��
	RECT	rcSent;					//�����ʾ��ʾλ��
	RECT	rcCharMode;				//���ģʽͼ����ʾλ��
	//ƴ������
	BYTE	bySyllables;			//ƴ��������
	BYTE	byCaret;				//��ǰ�༭������
	BYTE	bySyCaret;				//�����ڲ��༭���
	char	szWord[MAX_SYLLABLES][2];	//ƴ����Ӧ�ĺ���
	SPELLINFO spellData[MAX_SYLLABLES];	//������Ϣ
	BYTE	bySelect[MAX_SYLLABLES];	//�ֶ�ѡ�������־ 0-�Զ�Ԥ��,1-�ֶ�ѡ���֣��������ֶ�ѡ�����
	LPBYTE  pbyBlur[MAX_SYLLABLES];		//ƴ����ģ����
	BOOL	bPYBiHua;				//ƴ���ʻ�����״̬��־
	//�������
	LPBYTE  pbySentWord[MAX_SENTLEN];	//���ֵ�ַ
	short   sSentLen;				//���ӳ���
	short   sSentWords;				//����
	short	sSentCaret;				//��ǰ�༭λ��
	
	//Ӣ������
	LPBYTE  pbyEnAstPhrase;			//�����뵽��Ӣ�Ķ�Ӧ�Ĵ���

	//Ӣ������
	LPBYTE  pbyEnSpell;				//Ӣ��ƴд
	LPBYTE  pbyEnPhontic;			//��������

	HWND	hUiWnd;					//UIWnd
}CONTEXTPRIV,*PCONTEXTPRIV,FAR *LPCONTEXTPRIV;

//�������������
#define CPC_STATE		(1<<0)
#define CPC_CAND		(1<<1)
#define CPC_COMP		(1<<2)
#define CPC_INPUT		(1<<3)
#define CPC_ENGLISH		(1<<4)
#define CPC_SPELL		(1<<5)
#define CPC_SENTENCE	(1<<6)
#define CPC_ALL			0xFFFFFFFF

#define GETLPRESULTSTR(lpCompStr) ((LPTSTR)((LPBYTE)lpCompStr+lpCompStr->dwResultStrOffset))
#define GETLPCOMPSTR(lpCompStr) ((LPTSTR)((LPBYTE)lpCompStr+lpCompStr->dwCompStrOffset))

BOOL ClearPrivateContext(LPINPUTCONTEXT lpIMC,DWORD dwMask);
void ClearPrivateContextEx(LPCONTEXTPRIV lpCntxtPriv,DWORD dwMask);

void Config_GetIni(char *szPath);
void SoundPlay(LPCTSTR pszSound);
int  GB2GIB5(char *szBuf,int nStrLen);
void InputString(LPTSTR pszText,HWND hWnd);
BOOL MyGenerateMessageToTransKey(LPDWORD lpdwTransKey, UINT *uNumTranMsgs, UINT msg, WPARAM wParam, LPARAM lParam) ;
BOOL MyGenerateMessage(HIMC hIMC, UINT msg, WPARAM wParam, LPARAM lParam);


extern HINSTANCE	g_hInst;
extern char			g_szSkin[MAX_PATH];		//��ǰ�򿪵�skin
extern HIMAGELIST	g_himglstIcon;			//����ͼ���б�
extern BOOL			g_bCaps;				//��д״̬��־
extern HFONT		g_hPhoneticFont;		//��������
extern HWND			g_hWndToolTip;			//��ʾ��Ϣ����
extern HWND			g_hWndMsg;				//��Ϣ��ʾ����
extern COMPMODE		g_CompMode;				//����ģʽ
extern SPECIALDATA  g_SpCharData;			//���������
extern SETTINGSL	g_SettingsL;			//�ֲ�����
extern SETTINGSG	g_SettingsG;			//���뷨����

extern BOOL			g_bDelayPhontic;		//��ʱ���������־
extern BOOL			g_bTempSpell;			//��ʱƴ����־
extern char			g_szTip[500];			//��ʾ��Ϣ������
extern char			g_szDataPath[MAX_PATH];	//�������������·��
extern int			g_ImeCount;				//�򿪵����뷨������
extern char			g_szImeTitle[200];		//���뷨����
extern char			g_szPYBiHua[50];		//ƴ������ʱ�ıʻ�������
extern char			g_cHotKeyUD;			//�л����û�������������ؿ�ݼ�
extern char			g_cHotKeySent;			//�л������������������ؿ�ݼ�
extern POINT		g_ptStatusDockMode;		//״̬����ʾλ����Ϣ��-1,0,1���� (��/��)���У�(��/��) 3��״̬
extern DWORD		g_dwTimeSpan;			//������뻨��ʱ��
extern DWORD		g_dwInputCount;			//�����ַ�����
extern BOOL			g_bTipShow;				//����������ʾ��ʾ��Ϣ
extern POINT		g_ptInputPos;			//�������봰�ڵ�ǰλ��
extern HRGN			g_hRgnInput;			//���봰�ڵ�����
extern LPBYTE		g_pbyData;				//���ݽ��ջ�����
