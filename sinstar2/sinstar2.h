
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
//窗口类名
#define CLSNAME_UI			_T("ISUI")		//UI
#define CLSNAME_STATUS		_T("ISSTATUS")	//状态栏
#define CLSNAME_INPUT		_T("ISINPUT")	//输入窗口
#define CLSNAME_SPCHAR		_T("ISSPCHAR")	//特殊字符窗口

//编码数据包括2部分：
//1 编码串：形码中是纯编码，拼音中是预测结果+当前在编辑的字拼音=MAX_SENTLEN+7
//2 结果串: 与编码串共用内存
#define MAX_COMP                120
#define MAX_SYLLABLES			(MAX_SENTLEN/2)
#define MAX_INPUT				255 // >(MAX_SENTLEN+7) && >6*MAX_SYLLABLES && UD_MAXCAND

#define UM_CALCSIZE	(WM_USER+10)

#define IMN_PRIV_UPDATEINPUTWND		1		//计算输入窗口大小
#define IMN_PRIV_INVALIDINPUTWND	(1<<1)
#define IMN_PRIV_SKINCHANGE			(1<<2)	//皮肤更改消息
#define IMN_PRIV_SETDELAY			(1<<3)
#define IMN_PRIV_KILLDELAY			(1<<4)
#define IMN_PRIV_COMMAND			(1<<5)
#define IMN_PRIV_SHOWSPCHAR			(1<<6)
#define IMN_PRIV_HIDESPCHAR			(1<<7)
#define IMN_PRIV_OPENINPUTWND		(1<<8)
#define IMN_PRIV_CLOSEINPUTWND		(1<<9)
#define IMN_PRIV_UPDATESTATUS		(1<<10)	//更新状态栏窗口
#define IMN_PRIV_SAVEUICFG			(1<<11)	//保存窗口界面信息

#define IMN_PRIV_PLUGINCLOSE		(1<<15)	//插件退出消息

#define SKIN_RANDOM		"\\Rand_Skin\\"

//状态栏按钮ID
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
 IM_SPELL=0,	//拼音输入模式
 IM_SHAPECODE,	//形码输入模式
}COMPMODE;

typedef enum tagASTMODE
{//输入时的联想类型
	AST_NULL=0,	//无联想
	AST_CAND,	//词组联想
	AST_ENGLISH,//单词联想
	AST_PHRASEREMIND,//词组提示
}ASTMODE;

typedef struct tagSETTINGSG
{
	BOOL	b23CandKey;			// ;+'选23重码
	BYTE	by2CandVK;			// 2 Cand key
	BYTE	by3CandVK;			// 3 Cand Key
	BYTE	bySwitchKey;		// 输入法开关
	BYTE	byTempSpellKey;		// 临时拼音开关
	BYTE	byForecast;			// forecast mask
	BYTE	byLineKey[6];		// 笔画输入法转换键值
	BYTE	byPntcRed;			// 音标颜色 Red
	BYTE	byPntcGreen;		// 音标颜色 Green
	BYTE	byPntcBlue;			// 音标颜色 Blue
	BYTE	byTransStatus;		// 状态窗口透明度 50~255
	BYTE    byTransInput;		// 输入窗口透明度50~255
	char	cPageCands;			// 一屏的重码数
	BOOL	bFastUMode;			// 快捷自定义模式切换
	BOOL	bShowOpTip;			// 显示操作提示开关
	char    szPhonticLeft[50];	// 音标左数据
	char    szPhonticRight[50];	// 音标右数据
	int		nGbkMode;			// GBK Show Mode

	BYTE	byRateAdjust;		// 词频调整类型 0-不使用，１－智能调频，２－快速调频
	BYTE	byAstMode;			// 联想类型 (ASTMODE)
	BOOL	bAstSent;			// 语句联想
	BOOL	bAutoMatch;			// 智能选词
	BOOL	bTTSInput;			// 语音较对
	
	BOOL	bBlendUD;			// 混合检索自定义编码表
	BOOL	bBlendSpWord;		// 混合输入拼音单字
	BOOL	bAutoInput;			// 重码自动上屏

	BYTE	byTurnPageUpVK;		// 重码上翻键
	BYTE	byTurnPageDownVK;	// 重码下翻键

	BYTE	byHotKeyQuery;		// 查询热键
	BYTE	byHotKeyMode;		// 五笔拼音模式切换
	BYTE	byHotKeyEn;			// 英文输入开关
	BYTE	byHotKeyMakeWord;	// 造词热键
	BYTE	byHotKeyShowRoot;	// 字根表显示热键
	BYTE	byHotKeyHideStatus;	// 隐藏状态栏

	BOOL	bPYPhraseFirst;		// 拼音词组优先
	BOOL	bEnterClear;		// 回车清空编码
	int		nSoundAlert;		// 声音提示
	BOOL	bDisableFnKey;		// 禁用功能键
	BOOL	bAutoDot;			// 智能小数点
	BOOL	bAutoPrompt;		// 智能重码提示
	BOOL	bDisableDelWordCand;// 禁止删除单字重码
	char	szWebHeader[100];	// 自动转为英文输入模式的抬头

	BOOL	bCandSelNoNum;		// 禁用数字选择重码
	BOOL	bOnlySimpleCode;	// 出简不出全
	BYTE	byFadeSteps;		// 编码窗口渐显渐隐速度，0代表关闭该功能
	BOOL	bIptWndVertical;	// 坚排窗口标志
	char	szPlugin[100];		// plugin name
	BOOL	bShadeInput,bShadeStatus;//shade
}SETTINGSG;

typedef struct tagSETTINGSL
{
	BOOL	bHideStatus;		// 当前状态栏隐藏状态
	BOOL	bMouseFollow;		// 鼠标跟随开关
	BOOL	bEnglish;			// 英文单词输入开关
	BOOL	bFullStatus;		// 状态栏展开标志
	BOOL	bCharMode;			// 标点模式
	BOOL	bSound;				// 发音标志
	BOOL	bRecord;			// 记录输入语句
	BOOL	bInputBig5;			// 繁体输出标志
}SETTINGSL;

typedef struct tagUIPRIV {      // IME private UI data
    HWND    hStatusWnd;         // status window
	POINT	ptStatusWnd;		// 状态栏窗口位置
    HWND    hInputWnd;          // 输入窗口
	POINT	ptInputWnd;			// 输入窗口位置
	HWND	hSpCharWnd;			// 特殊字符窗口
	POINT	ptSpCharWnd;		// 特殊字符窗口位置
}UIPRIV;

typedef UIPRIV      *PUIPRIV;
typedef UIPRIV FAR  *LPUIPRIV;

typedef struct tagSPELLINFO
{
	char szSpell[7];	//拼音
	BYTE bySpellLen;	//拼音长度
}SPELLINFO,*PSPELLINFO,FAR *LPSPELLINFO;

typedef enum tagINSTATE
{
	INST_CODING=0,	//编码输入状态
	INST_ENGLISH,	//英文单词输入状态
	INST_DIGITAL,	//数字输入状态
	INST_USERDEF,	//用户自定义输入状态
	INST_LINEIME,	//笔画输入法状态
}INSTATE;

typedef enum tagSUBSTATE
{
	SBST_NORMAL=0,	//一般状态
	SBST_ASSOCIATE,	//联想信息
	SBST_SENTENCE,	//语句输入状态
}SUBSTATE;

typedef struct tagCONTEXTPRIV
{
	char    szComp[MAX_COMP];		//输入的编码数据
	char	cCompLen;				//输入的编码长度
	char	szInput[MAX_INPUT];			//当前输入数据,UD的长度可能超过250
	int		nInputLen;				//输入的数据的长度

	INSTATE	inState;				//当前输入状态
	SUBSTATE sbState;				//输入子状态
	BOOL	bWebMode;				//网址输入模式
//	LPBYTE  pbyData;				//数据区
	//重码数据
	short	sCurCandPage;			//当前重码页
	short   sCandCount;				//重码数
	LPBYTE  *ppbyCandInfo;			//重码数据表
	//显示位置
	RECT	rcCand[10];				//重码在窗口的位置
	RECT	rcHeader;				//英文联想的词头显示位置
	RECT	rcTip;					//提示信息位置
	RECT	rcComp;					//编码显示位置
	RECT	rcSent;					//语句提示显示位置
	RECT	rcCharMode;				//标点模式图标显示位置
	//拼音数据
	BYTE	bySyllables;			//拼音音节数
	BYTE	byCaret;				//当前编辑的音节
	BYTE	bySyCaret;				//音节内部编辑光标
	char	szWord[MAX_SYLLABLES][2];	//拼音对应的汉字
	SPELLINFO spellData[MAX_SYLLABLES];	//音节信息
	BYTE	bySelect[MAX_SYLLABLES];	//手动选择重码标志 0-自动预测,1-手动选择单字，其它－手动选择词组
	LPBYTE  pbyBlur[MAX_SYLLABLES];		//拼音的模糊音
	BOOL	bPYBiHua;				//拼音笔画辅助状态标志
	//智能语句
	LPBYTE  pbySentWord[MAX_SENTLEN];	//单字地址
	short   sSentLen;				//句子长度
	short   sSentWords;				//词数
	short	sSentCaret;				//当前编辑位置
	
	//英文联想
	LPBYTE  pbyEnAstPhrase;			//与联想到的英文对应的词组

	//英文输入
	LPBYTE  pbyEnSpell;				//英文拼写
	LPBYTE  pbyEnPhontic;			//音标数据

	HWND	hUiWnd;					//UIWnd
}CONTEXTPRIV,*PCONTEXTPRIV,FAR *LPCONTEXTPRIV;

//清除缓冲区数据
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
extern char			g_szSkin[MAX_PATH];		//当前打开的skin
extern HIMAGELIST	g_himglstIcon;			//编码图标列表
extern BOOL			g_bCaps;				//大写状态标志
extern HFONT		g_hPhoneticFont;		//音标字体
extern HWND			g_hWndToolTip;			//提示信息窗口
extern HWND			g_hWndMsg;				//消息显示窗口
extern COMPMODE		g_CompMode;				//编码模式
extern SPECIALDATA  g_SpCharData;			//软键盘数据
extern SETTINGSL	g_SettingsL;			//局部设置
extern SETTINGSG	g_SettingsG;			//输入法设置

extern BOOL			g_bDelayPhontic;		//延时输入音标标志
extern BOOL			g_bTempSpell;			//临时拼音标志
extern char			g_szTip[500];			//提示信息缓冲区
extern char			g_szDataPath[MAX_PATH];	//程序的数据所在路径
extern int			g_ImeCount;				//打开的输入法计数器
extern char			g_szImeTitle[200];		//输入法名称
extern char			g_szPYBiHua[50];		//拼音输入时的笔画辅助码
extern char			g_cHotKeyUD;			//切换到用户定义的与编码相关快捷键
extern char			g_cHotKeySent;			//切换到语句输入的与编码相关快捷键
extern POINT		g_ptStatusDockMode;		//状态栏显示位置信息，-1,0,1代表 (上/左)，中，(下/右) 3种状态
extern DWORD		g_dwTimeSpan;			//输入编码花费时间
extern DWORD		g_dwInputCount;			//输入字符数量
extern BOOL			g_bTipShow;				//在重码区显示提示信息
extern POINT		g_ptInputPos;			//保存输入窗口当前位置
extern HRGN			g_hRgnInput;			//输入窗口的区域
extern LPBYTE		g_pbyData;				//数据接收缓冲区
