#ifndef ISCOMM_H
#define ISCOMM_H

#include "commfile.h"

//����֮��ͨѶʹ�õ��ڴ��ļ�ӳ�������
#define NAME_REQ_MAPFILE	"_sinstar2 req map file"	//�ͻ�����������������������ͨ��
#define MAX_BUF_REQ_MAP		1<<12						//4K						
#define MAX_BUF_REQ			(MAX_BUF_REQ_MAP)									
#define NAME_ACK_MAPFILE	"_sinstar2 ack map file"	//��������ͻ��˷���ָʾ������ͨ��
#define MAX_BUF_ACK_MAP		1<<15						//32K
#define MAX_BUF_ACK			((MAX_BUF_ACK_MAP)-sizeof(HWND))

//����֮��ͨѶʹ�õ�ͬ���¼�����,�ͻ���ͨ������lock��Mutex����֤�ܹ��������д��Ȩ�ޣ��Է�ֹ����ͻ���ͬʱд��
#define NAME_REQ_SYNCOMMUTEX	"_sinstar2 req synchronization mutex"

#define MSG_NAME_SINSTAR2		"_sinstar2 communicate msg"	//ͨѶʱʹ�õ���Ϣ���ƣ�ȡ���ϰ汾�̶�����ϢID�������������������


#pragma pack(push,1)

typedef struct tagMSGDATA{
	short	sSize;		//�������ݳ���
	BYTE	byData[1];	//����������ʼ��ַ
}MSGDATA,*PMSGDATA;

	
typedef struct tagCOMPINFO
{
	char szName[50];	//����
	char szCode[50];	//��Ԫ
	char cWildChar;		//���ܼ�
	char cCodeMax;		//����볤
	char szUrl[50];		//�����ַ
	DWORD dwWords;		//�Ѿ������ַ���
	BOOL bSymbolFirst;	//����ױ���
	DWORD dwUnused:4;	//������ʹ�õı�־�������뷨���û������
	DWORD dwReserved:28;	//����,��ʼ��Ϊ0

	COLORREF crIconKey;	//����ͼ���͸��ɫ
	HBITMAP	hIcon;		//����ͼ��
}COMPINFO,*PCOMPINFO;

typedef struct tagFLMINFO
{
	char szName[50];
	char szAddition[50];
	char szAddFont[LF_FACESIZE];
}FLMINFO,*PFLMINFO;
#pragma pack(pop)

#define CISIZE_BASE		164		//����������Ϣ������

#define MAX_SENTLEN		50		//�����󳤶�
#define MAX_WORDLEN		30		//������󳤶�
#define MAX_COMPLEN		20		//ϵͳ֧�ֵ�����볤

#define RATE_USERDEF	0xFF	//�Զ������ʹ�õĴ�Ƶ
#define RATE_FORECAST	0xFE	//����Ԥ��õ��Ĵ�ʹ�õĴ�Ƶ
#define RATE_GBK		0xFD	//GBK flag
#define RATE_USERCMD	0xFC	//Command ID
#define RATE_MIXSP		0xFB	//��ƴ����
#define RATE_WILD		0xFA	//���ܼ���ѯ���
#define RATE_ASSOCIATE	0xF9	//��������

//ϵͳ��Ӧ
#define ISACK_SUCCESS	0
#define ISACK_DELAY		1	
#define ISACK_UNKNOWN	100		//����ʶ�����Ϣ
#define ISACK_ERRCLIENT	101		//����Ŀͻ����ھ��
#define ISACK_SVRCANCEL	102		//�������쳣�˳�
#define ISACK_UNINIT	103		//ͨѶû��������ʼ��
#define ISACK_ERRBUF	104		//���ݳ��ȴ���
#define ISACK_ERROR		105		//һ���Դ���
	
//��ȡ��������ⷵ��ֵ
#define MCR_NORMAL		0		//������ȡ����
#define MCR_ASSOCIATE	1		//������,����Ԥ�⼰������ʾ
#define MCR_LONGERCOMP	(1<<1)	//�����Ե�ǰ����Ϊǰ׺�ĳ��Զ������
#define MCR_MIXSP		(1<<2)	//��ϼ�ƴ
#define MCR_USERDEF		(1<<3)	//����Զ�������
#define MCR_SPCAND		(1<<4)	//����ƴ������
#define MCR_WILD		(1<<6)	//���ܼ��Ĳ�ѯ
#define MCR_AUTOSELECT	(1<<7)	//Ψһ������

//����mask
#define MKI_ASTCAND		1		//��������
#define MKI_ASTSENT		(1<<1)	//��������
#define MKI_ASTENGLISH	(1<<2)	//Ӣ������
#define MKI_RECORD		(1<<3)	//��¼����
#define MKI_AUTOPICK	(1<<4)	//����ѡ��ʱ����������ӵĿ���
#define MKI_TTSINPUT	(1<<5)	//�ʶ���������
#define MKI_PHRASEREMIND (1<<6)	//��������

//��ѯ����mask
#define MQC_NORMAL		0		//��ѯ��ͨ����
#define MQC_FORECAST	1		//��ѯ�����Ԥ����
#define MQC_FCNOCAND	(1<<1)	//�޺�ѡ��ʱԤ��
#define MQC_MATCH		(1<<2)	//�Զ�����ƥ���������Ϊ��ѡ��
#define MQC_ONLYSC		(1<<3)	//���򲻳�ȫ
#define MQC_USERDEF		(1<<4)	//ͬʱ��ѯ�Զ������
#define MQC_AUTOPROMPT	(1<<5)	//���ܱ�����ʾ
#define MQC_SPCAND		(1<<6)	//����Զ�������

//TTSѡ��
#define MTTS_EN			1		//ʹ��Ӣ������	1
#define MTTS_CH			2		//ʹ����������

//rateAdjust mode
#define RAM_AUTO		0		//���ܵ�Ƶ
#define RAM_FAST		1		//���ٵ�Ƶ

#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

const UINT ISComm_GetCommMsgID();
BOOL ISComm_GetConfig(LPSTR pszConfig);
BOOL ISComm_GetSvrPath(LPSTR pszPath);
BOOL ISComm_SetSvrPath(LPSTR pszPath);
BOOL ISComm_GetDataPath(LPSTR pszPath);

DWORD ISComm_SendMsg(DWORD dwType,LPVOID pData,short sSize,HWND hWnd);
DWORD ISComm_PostMsg(DWORD dwType,LPVOID pData,short sSize,HWND hWnd);
PMSGDATA ISComm_GetData();

BOOL ISComm_Login(HWND hWnd);
void  ISComm_Logout(HWND hWnd);

PCOMPINFO ISComm_GetCompInfo();
PFLMINFO  ISComm_GetFlmInfo();
char *    ISComm_GetUserDict();

PMSGDATA ISComm_OnSeverNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

DWORD ISComm_KeyIn(LPCSTR pszText,short sSize,BYTE byMask,HWND hWnd);

DWORD ISComm_QueryCand(LPCSTR pszComp,char cCompLen,BYTE byMask,HWND hWnd);
DWORD ISComm_QueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_Forecast(LPCSTR pszComp,char cCompLen);

DWORD ISComm_SpellQueryComp(LPCSTR pszPhrase,char cPhraseLen);
DWORD ISComm_SpellForecast(char szPrefix[2],LPBYTE *pbyBlurs,BYTE bySyllables);
DWORD ISComm_Spell2ID(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellGetBlur(LPCSTR pszSpell,char cCompLen);
DWORD ISComm_SpellQueryCandEx(LPBYTE *pbyBlur,BYTE bySyllables,BYTE byFlag);
DWORD ISComm_En2Ch(LPCSTR pszWord,char cWordLen);
DWORD ISComm_Ch2En(LPCSTR pszWord,char cWordLen);
DWORD ISComm_UDQueryCand(LPCSTR pszComp,char cCompLen);

DWORD ISComm_TTS(LPCSTR pszText,char cTextLen,BYTE byMask);
DWORD ISComm_RateAdjust(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,BYTE byMode,HWND hWnd);
DWORD ISComm_EnQueryCand(LPCSTR pszText,char cTextLen);
DWORD ISComm_MakePhrase(LPCSTR pszText,char cTextLen);
DWORD ISComm_ShowServer(LPCSTR pszPageName,char cTextLen);
DWORD ISComm_ServerVersion();
DWORD ISComm_PhraseDel(LPCSTR pszComp,char cCompLen,LPCSTR pszPhrase,char cPhraseLen,HWND hWnd);
DWORD ISComm_LineQueryCand(LPCSTR pszComp,char cCompLen);
int ISComm_PhraseRate(LPCSTR pszPhrase,char cPhraseLen);

DWORD ISComm_QueryUserDict(LPCTSTR pszkey,char cKeyLen);
DWORD ISComm_SortWordByBiHua(LPCTSTR pszBiHua,char cBiHuaLen,LPCTSTR pszWordList,short nWords);

DWORD ISComm_Blur_Set(BOOL bBlur);
DWORD ISComm_Blur_Add(char *pszSpell1,char *pszSpell2);
DWORD ISComm_Blur_Del(char *pszSpell1,char *pszSpell2);
DWORD ISComm_Blur_Query();
DWORD ISComm_Blur_DelAll();

DWORD ISComm_QueryWordSpell(char szWord[2]);
DWORD ISComm_SpellMemoryEx(LPCSTR pszText,char cTextLen,BYTE (*pbySpellID)[2]);
DWORD ISComm_SymbolConvert(char cSymbol,char cType);

DWORD ISComm_UserDict_List();
DWORD ISComm_UserDict_Open(LPCTSTR pszUserDict);
DWORD ISComm_UserDict_Max();
DWORD ISComm_UserDict_Item(DWORD dwItem);
DWORD ISComm_UserDict_Locate(LPCTSTR pszkey,char cKeyLen);

DWORD ISComm_Comp_List();
DWORD ISComm_Comp_Open(LPCTSTR pszComp);
BOOL ISComm_IsSvrWorking();
DWORD ISComm_SkinChange(HWND hUIWnd);

BOOL  ISComm_CheckComp(LPCTSTR pszComp,char cComplen,BYTE byMask);
DWORD ISComm_Bldsp_Get(BOOL *pbCe2,BOOL *pbCe3,BOOL *pbCa4);
DWORD ISComm_Bldsp_Set(BYTE byMask,BOOL bCe2,BOOL bCe3,BOOL bCa4);

BOOL  ISComm_SvrTray_Get();
void  ISComm_SvrTray_Set(BOOL bTray);

LPCTSTR ISComm_Svr_Pages();

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//ISCOMM_H