#include "sinstar2.h"
#include "udictinput.h"
#include <shellapi.h>
BYTE g_bComp[] = {      0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,        // 00-0F
                        0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,        // 10-1F
                        1,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,        // 20-2F
                        1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,        // 30-3F
                        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,        // 40-4F
                        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,        // 50-5F
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 60-6F
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 70-7F
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 80-8F
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // 90-9F
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // A0-AF
                        0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,        // B0-BF
                        1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // C0-CF
                        0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,        // D0-DF
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        // E0-EF
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};       // F0-FF


BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo,LPTSTR lpszUIClass,DWORD dwSystemInfoFlags)
{
	if(!lpIMEInfo) return FALSE;
    lpIMEInfo->dwPrivateDataSize = sizeof(CONTEXTPRIV);//ϵͳ������ΪINPUTCONTEXT.hPrivate����ռ�

    lpIMEInfo->fdwProperty = IME_PROP_KBD_CHAR_FIRST |
#ifdef _UNICODE
                             IME_PROP_UNICODE |
#endif
                             IME_PROP_SPECIAL_UI |
							 IME_PROP_END_UNLOAD ;

    lpIMEInfo->fdwConversionCaps = IME_CMODE_FULLSHAPE |
								IME_CMODE_NATIVE;

    lpIMEInfo->fdwSentenceCaps = IME_SMODE_NONE;
    lpIMEInfo->fdwUICaps = UI_CAP_2700;

	lpIMEInfo->fdwSCSCaps = 0;

    lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;

    _tcscpy(lpszUIClass,CLSNAME_UI);

    return TRUE;
}

BOOL WINAPI ImeConfigure(HKL hKL,HWND hWnd, DWORD dwMode, LPVOID lpData)
{
	//DebugLog(1,(DebugLogFile,"ImeConfigure\n"));
    switch (dwMode) {
    case IME_CONFIG_GENERAL:
        PropSheet_Show(hWnd,g_hInst,0);
        break;
    default:
        return (FALSE);
        break;
    }
    return (TRUE);
}

#define MKI_ALL (MKI_RECORD|MKI_TTSINPUT|MKI_AUTOPICK)
BYTE GetKeyinMask(BOOL bAssociate,BYTE byMask)
{
	BYTE byRet=0;
	if(g_SettingsG.bAutoMatch) byRet|=(MKI_AUTOPICK&byMask);
	if(g_SettingsL.bRecord) byRet|=(MKI_RECORD&byMask);
	if(g_SettingsG.bTTSInput) byRet|=(MKI_TTSINPUT&byMask);
	if(bAssociate)
	{
		if(g_SettingsG.bAstSent) byRet|=MKI_ASTSENT;
		switch(g_SettingsG.byAstMode)
		{
		case AST_ENGLISH:byRet|=MKI_ASTENGLISH;break;
		case AST_CAND:byRet|=MKI_ASTCAND;break;
		case AST_PHRASEREMIND:byRet|=MKI_PHRASEREMIND;break;
		default:break;
		}
	}
	return byRet;
}

void ClearPrivateContextEx(LPCONTEXTPRIV lpCntxtPriv,DWORD dwMask)
{
	if(dwMask&CPC_COMP)
	{
		memset(lpCntxtPriv->szComp,0,MAX_COMP);
		lpCntxtPriv->cCompLen=0;
	}
	if(dwMask&CPC_INPUT)
	{
		memset(lpCntxtPriv->szInput,0,MAX_SENTLEN);
		lpCntxtPriv->nInputLen=0;
	}
	if(dwMask&CPC_ENGLISH)
	{
		lpCntxtPriv->pbyEnSpell=NULL;
		lpCntxtPriv->pbyEnPhontic=NULL;
	}
	if(dwMask&CPC_STATE)
	{
		lpCntxtPriv->inState=INST_CODING;
		lpCntxtPriv->sbState=SBST_NORMAL;
		lpCntxtPriv->bWebMode=FALSE;
	}
	if(dwMask&CPC_CAND)
	{
		if(lpCntxtPriv->ppbyCandInfo)
		{
			free(lpCntxtPriv->ppbyCandInfo);
			memset(lpCntxtPriv->rcCand,0,sizeof(RECT)*10);
			lpCntxtPriv->ppbyCandInfo=NULL;
		}
		if(lpCntxtPriv->sCandCount)
		{
			lpCntxtPriv->sCandCount=0;
			lpCntxtPriv->sCurCandPage=0;
		}
		lpCntxtPriv->pbyEnAstPhrase=NULL;
	}
	if(dwMask&CPC_SENTENCE)
	{
		lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentLen=0;
		lpCntxtPriv->sSentWords=0;
		memset(lpCntxtPriv->pbySentWord,0,sizeof(LPBYTE)*MAX_SENTLEN);
	}
	if(dwMask&CPC_SPELL)
	{
		BYTE i;
		for(i=0;i<lpCntxtPriv->bySyllables;i++)
		{
			if(lpCntxtPriv->pbyBlur[i]) free(lpCntxtPriv->pbyBlur[i]);
			lpCntxtPriv->pbyBlur[i]=NULL;
		}
		memset(lpCntxtPriv->szWord,VK_SPACE,2*MAX_SYLLABLES);
		memset(lpCntxtPriv->bySelect,0,sizeof(BOOL)*MAX_SYLLABLES);
		memset(lpCntxtPriv->spellData,0,sizeof(SPELLINFO)*MAX_SYLLABLES);
		lpCntxtPriv->bySyllables=1;
		lpCntxtPriv->bySyCaret=0xFF;
		lpCntxtPriv->byCaret=0;
		lpCntxtPriv->bPYBiHua=FALSE;
	}
}

BOOL ClearPrivateContext(LPINPUTCONTEXT lpIMC,DWORD dwMask)
{
    LPCONTEXTPRIV		lpCntxtPriv=NULL;
    lpCntxtPriv = (LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
	if(!lpCntxtPriv) return FALSE;
	ClearPrivateContextEx(lpCntxtPriv,dwMask);
	Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
    ImmUnlockIMCC(lpIMC->hPrivate);
    return (TRUE);
}

BOOL CmdExecute(BYTE * pszBuf)
{
	UINT uRet=FALSE;
	char *pParam=NULL;
	char *pCmd=(char*)pszBuf+pszBuf[1]+2+1;
	if(pCmd[0]=='\"')
	{
		pParam=strstr(pCmd,"\" ");
		if(pParam) {pParam[1]=0;pParam+=2;}
	}else
	{
		pParam=strstr(pCmd," ");
		if(pParam) pParam[0]=0,pParam++;
	}
	uRet=(UINT)ShellExecute(NULL,"open",pCmd,pParam,NULL,SW_SHOWDEFAULT);
	if(uRet<=32) uRet=(UINT)ShellExecute(NULL,"explorer",pCmd,NULL,NULL,SW_SHOWDEFAULT);
	return uRet>32;
}

/**********************************************************************/
/* InitImeData()                                                     */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL PASCAL InitImeData(
    LPINPUTCONTEXT lpIMC)
{
    HIMCC               hMem;
    LPCOMPOSITIONSTRING lpCompStr;
	LPCONTEXTPRIV		lpCntxtPriv;
    DWORD               dwSize;
	
    if(!lpIMC) {
        return (FALSE);
    }

    dwSize = sizeof(COMPOSITIONSTRING) + (MAX_INPUT);	//InputStringҲ��Ҫʹ����������

    if (!lpIMC->hCompStr) {
        // it maybe free by other IME, init it
        lpIMC->hCompStr = ImmCreateIMCC(dwSize);
    } else if (hMem = ImmReSizeIMCC(lpIMC->hCompStr, dwSize)) {
        lpIMC->hCompStr = hMem;
    } else {
        ImmDestroyIMCC(lpIMC->hCompStr);
        lpIMC->hCompStr = ImmCreateIMCC(dwSize);
        return (FALSE);
    }

    lpCompStr = (LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
    if (!lpCompStr) {
        ImmDestroyIMCC(lpIMC->hCompStr);
        lpIMC->hCompStr = ImmCreateIMCC(dwSize);
        return (FALSE);
    }
	memset(lpCompStr,0,dwSize);
    lpCompStr->dwSize = dwSize;
	lpCompStr->dwCompStrOffset = sizeof(COMPOSITIONSTRING);
	lpCompStr->dwCompStrLen = 0;
	lpCompStr->dwResultStrOffset = sizeof(COMPOSITIONSTRING);
	lpCompStr->dwResultStrLen = 0;
	lpCompStr->dwCompClauseLen=0;
    ImmUnlockIMCC(lpIMC->hCompStr);

	//�����hPrivate�б���ı�������
	dwSize=sizeof(CONTEXTPRIV);
    if (!lpIMC->hPrivate) {
        // it maybe free by other IME, init it
        lpIMC->hPrivate = ImmCreateIMCC(dwSize);
	}else if(hMem = ImmReSizeIMCC(lpIMC->hPrivate, dwSize)) {
        lpIMC->hPrivate = hMem;
	}else{
		return FALSE;
	}
	lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
	memset(lpCntxtPriv,0,sizeof(CONTEXTPRIV));
	ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
	ImmUnlockIMCC(lpIMC->hPrivate);
    return (TRUE);
}

/**********************************************************************/
/* ImeSelect()                                                        */
/* Return Value:                                                      */
/*      TRUE - successful, FALSE - failure                            */
/**********************************************************************/
BOOL WINAPI ImeSelect(HIMC hIMC,BOOL fSelect)
{
    LPINPUTCONTEXT lpIMC;
	BOOL bRet=FALSE;
    if (!hIMC) {
        return (FALSE);
    }
    lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC);
    if (!lpIMC) {
        return (FALSE);
    }
	if(fSelect)
	{
		bRet=InitImeData(lpIMC);
	}else
	{
		ClearPrivateContext(lpIMC,CPC_ALL);
		if(g_bTempSpell)
		{
			g_bTempSpell=FALSE;
			g_CompMode=IM_SHAPECODE;
		}
		bRet=TRUE;
		MyGenerateMessage(hIMC,WM_IME_ENDCOMPOSITION,0,0);
	}
 	lpIMC->fOpen=fSelect;
    ImmUnlockIMC(hIMC);
    return bRet;
}

/*
ʹһ�����������ļ������ʧ���֪ͨ���뷨���µ����������ģ������ڴ���һЩ��ʼ������
HIMC hIMC :����������
BOOL fFlag : TRUE if activated, FALSE if deactivated. 
Returns TRUE if successful, FALSE otherwise. 
*/
BOOL WINAPI ImeSetActiveContext(HIMC hIMC,BOOL fFlag)
{
	Helper_Trace("\nImeSetActiveContext ,fFlag=%d",fFlag);
    return TRUE;
}

BOOL KeyIn_IsCoding(LPCONTEXTPRIV lpCntxtPriv)
{
	BOOL bOpen=FALSE;
	if(lpCntxtPriv->inState==INST_CODING)
	{//��������
		if(lpCntxtPriv->sbState==SBST_NORMAL)
		{
			if(g_CompMode==IM_SHAPECODE)
			{//����
				bOpen=lpCntxtPriv->cCompLen!=0;
			}else//if(g_CompMode==IM_SPELL)
			{//ƴ��
				bOpen=(lpCntxtPriv->bySyllables>1 || lpCntxtPriv->spellData[0].bySpellLen>0);
				if(g_bTempSpell) bOpen=TRUE;
			}
		}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
			bOpen=TRUE;
	}else if(lpCntxtPriv->inState==INST_ENGLISH)
	{//Ӣ������
		bOpen=lpCntxtPriv->cCompLen!=0;
	}else if(lpCntxtPriv->inState==INST_USERDEF || lpCntxtPriv->inState==INST_LINEIME)
	{
		bOpen=TRUE;
	}
	return bOpen;
}

//�ر����봰�ڲ�����������
void CloseInputWndAndInputComp(HIMC hIMC, LPINPUTCONTEXT lpIMC)
{
	LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
	char szInput[MAX_SYLLABLES*6+1];
	int nLen=0;
	if(g_CompMode==IM_SPELL)
	{
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_NORMAL)
		{
			BYTE i;
			lpCntxtPriv->nInputLen=0;
			for( i=0;i<lpCntxtPriv->bySyllables ;i++)
			{
				memcpy(szInput+nLen,lpCntxtPriv->spellData[i].szSpell,lpCntxtPriv->spellData[i].bySpellLen);
				nLen+=lpCntxtPriv->spellData[i].bySpellLen;
			}
		}
	}else
	{
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_NORMAL)
		{
			memcpy(szInput,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen);
			nLen=lpCntxtPriv->cCompLen;
		}
	}
	ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
	
	if(nLen)
	{//��ǰ�����������
		MyGenerateMessage(hIMC,WM_IME_ENDCOMPOSITION,0,0);	//���������ʱ���͵�WM_IME_STARTCOMPOSITION��Ϣ���
		szInput[nLen]=0;
		InputString(szInput,lpIMC->hWnd);
	}
	
	MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
	ISComm_KeyIn(".",1,g_SettingsL.bRecord?MKI_RECORD:0,lpCntxtPriv->hUiWnd);
	ImmUnlockIMCC(lpIMC->hPrivate);
}

BOOL IsDigitEx(char c)
{
	if(isdigit(c)) return TRUE;
	return c=='.';
}

//�жϵ�ǰ״̬�����ּ����ǲ����нϱ���,�Զ���״̬���ʻ�����״̬��֧�����ּ��̵�������
BOOL KeyIn_IsNumCode(LPCONTEXTPRIV lpCntxtPriv)
{
	return (lpCntxtPriv->inState==INST_USERDEF
		&& ((lpCntxtPriv->cCompLen>0 && IsDigitEx(lpCntxtPriv->szComp[0])) || lpCntxtPriv->cCompLen==0)
		);
}

/*
ϵͳ��������ӿ����ж�IME�Ƿ���ǰ��������
HIMC hIMC:����������
UINT uKey:��ֵ
LPARAM lKeyData: unknown
CONST LPBYTE lpbKeyState:����״̬,����256����״̬
return : TRUE-IME����,FALSE-ϵͳ����
ϵͳ�����ImeToAsciiEx������ֱ�ӽ�������Ϣ����Ӧ�ó���
*/
BOOL WINAPI ImeProcessKey(HIMC hIMC,UINT uKey,LPARAM lKeyData,CONST LPBYTE lpbKeyState)
{
	static BOOL bPressOther = FALSE;
	static BOOL bPressShift=FALSE;
	static BOOL bPressCtrl=FALSE;
	BOOL bRet=FALSE;
	LPINPUTCONTEXT lpIMC =NULL;
//	__asm int 3
	if(!ISComm_IsSvrWorking()) return FALSE;
	if ((lKeyData & 0x80000000) && (uKey != VK_SHIFT && uKey !=VK_CONTROL))
        return FALSE;
	
	if(!(lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC)) )
		return FALSE;
	if(uKey==VK_CAPITAL)
	{
		g_bCaps=lpbKeyState[VK_CAPITAL]&0x01;
		if(g_bCaps) CloseInputWndAndInputComp(hIMC,lpIMC);
		ImmUnlockIMC(hIMC);
		return FALSE;
	}else if(uKey==VK_SHIFT)
	{
		if(bPressCtrl)
		{
			bPressOther=TRUE;
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����SHIFT��
		{
			if(!bPressOther && bPressShift)//��SHIFT�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				BYTE byKey=(BYTE)(lKeyData>>16);
				bPressShift=FALSE; 
				if(!(lpbKeyState[VK_SPACE]&0x80) &&
					g_SettingsG.bySwitchKey==byKey)
				{ //check the scan code
					BOOL bOpen=!lpIMC->fOpen;
					ImmSetOpenStatus(hIMC,bOpen);
					if(bOpen)
					{//��������
						MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_OPENSTATUSWINDOW,0);//��ʾ״̬��
						MyGenerateMessage(hIMC,WM_IME_SETCONTEXT,1,0);//ͨ��WM_IME_SETCONTEXT��ȷ���Ƿ�����봰��
					}else
					{//�ر����룬�����ǰ����������,�򽫵�ǰ����������������༭����
						CloseInputWndAndInputComp(hIMC,lpIMC);
						MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_CLOSESTATUSWINDOW,0);
					}
				}
			}else//��ԭ״̬
			{
				bPressShift=FALSE;
				bPressOther=FALSE;
			}	
		}else
		{//����SHIFT������ʼ��״̬
			bPressShift=TRUE;
			bPressOther=FALSE;
		}
	}else if(uKey==VK_CONTROL)
	{
		if(!lpIMC->fOpen)
		{
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		if(bPressShift)
		{
			bPressOther=TRUE;
			ImmUnlockIMC(hIMC);
			return FALSE;
		}
		if(lKeyData & 0x80000000)//����Ctrl��
		{
			BYTE byKey=(BYTE)(lKeyData>>24);
			if(!bPressOther && bPressCtrl)//Ctrl�����º�û�а�������������ʾʹ�ÿ�ݹرչ���
			{
				LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
				LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
				if(g_SettingsG.byTempSpellKey==byKey)
				{//��ʱƴ����ƴ������
					if(g_CompMode==IM_SHAPECODE)
					{//�������״̬��������ʱƴ��״̬
						g_bTempSpell=TRUE;
						g_CompMode=IM_SPELL;
						Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
						ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
						g_bTipShow=TRUE;
						strcpy(g_szTip,"��ʱƴ��:����������ʾ");
						MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS|IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
					}else if(g_CompMode==IM_SPELL)
					{//ƴ������״̬
						if(g_bTempSpell && lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
						{//�˳���ʱƴ��״̬
							g_bTempSpell=FALSE;
							g_CompMode=IM_SHAPECODE;
							Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS|IMN_PRIV_CLOSEINPUTWND);
							ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
						}else if(lpCntxtPriv->sCandCount)
						{
							if(lpCntxtPriv->bPYBiHua)
							{//�˳��ʻ��븨������ģʽ
								g_szPYBiHua[0]=0;
								lpCntxtPriv->bPYBiHua=FALSE;
							}else
							{//����ʻ��븨������ģʽ
								short i,sCands=0;
								lpCntxtPriv->bPYBiHua=TRUE;
								g_szPYBiHua[0]=0;
								//��������е�������
								for(i=0;i<lpCntxtPriv->sCandCount;i++)
								{
									LPBYTE 	pbyCand=lpCntxtPriv->ppbyCandInfo[i];
									if(pbyCand[1]==2)
									{
										lpCntxtPriv->ppbyCandInfo[sCands++]=lpCntxtPriv->ppbyCandInfo[i];
									}
								}
								lpCntxtPriv->sCandCount=sCands;
								lpCntxtPriv->sCurCandPage=0;
							}
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
						}
					}
				}else if(!g_SettingsG.bDisableFnKey && ((g_SettingsG.byTempSpellKey==0 && byKey==0xC1)||g_SettingsG.byTempSpellKey!=0))
				{//���ܼ�
#ifndef SOWB
					if(!KeyIn_IsCoding(lpCntxtPriv)) 
					{
						if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_ASSOCIATE
							&& ((g_CompMode==IM_SHAPECODE &&  lpCntxtPriv->sSentLen) || (g_CompMode==IM_SPELL && lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) )
							)
						{//�����������״̬
							lpCntxtPriv->sbState=SBST_SENTENCE;
							lpCntxtPriv->sSentCaret=0;
							MyGenerateMessage(hIMC,WM_IME_STARTCOMPOSITION,0,0);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_INVALIDINPUTWND|IMN_PRIV_KILLDELAY);
							bRet=TRUE;
						}else
						{//�����û��Զ�������״̬
							ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
							lpCntxtPriv->inState=INST_USERDEF;
							Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							MyGenerateMessage(hIMC,WM_IME_STARTCOMPOSITION,0,0);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
							bRet=TRUE;
						}
					}else
					{
						if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_SENTENCE
							&& ((g_CompMode==IM_SHAPECODE &&  lpCntxtPriv->sSentLen) || (g_CompMode==IM_SPELL && lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) )
							)
						{//�������״̬
							lpCntxtPriv->sbState=SBST_ASSOCIATE;
							MyGenerateMessage(hIMC,WM_IME_ENDCOMPOSITION,0,0);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_INVALIDINPUTWND|IMN_PRIV_SETDELAY);
							bRet=TRUE;
						}else if(lpCntxtPriv->inState==INST_USERDEF)
						{//״̬��ԭ
							lpCntxtPriv->inState=INST_CODING;
							Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							lpCompStr->dwCompStrLen=0;
							MyGenerateMessage(hIMC,WM_IME_ENDCOMPOSITION,0,0);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
							bRet=TRUE;
						}
					}
#endif//!SOWB
				}
				ImmUnlockIMCC(lpIMC->hCompStr);
				ImmUnlockIMCC(lpIMC->hPrivate);
				bPressCtrl=FALSE; 
			}else//��ԭ״̬
			{
				bPressCtrl=FALSE;
				bPressOther=FALSE;
			}		
		}else
		{//����Ctrl������ʼ��״̬
			bPressCtrl=TRUE;
			bPressOther=FALSE;
		}
	}else if(!g_bCaps) 
	{
		bPressOther=TRUE;
		if(lpIMC->fOpen)
		{
			if(lpbKeyState[VK_CONTROL]&0x80 && lpbKeyState[VK_SHIFT]&0x80)
			{//Ctrl + Shift
				ImmUnlockIMC(hIMC);
				bRet=(uKey==g_SettingsG.byHotKeyQuery || (uKey>='0' && uKey<='9'));
				if(!bRet) bRet=Plugin_HotkeyInquire(uKey);
				return bRet;
			}else if(lpbKeyState[VK_CONTROL]&0x80) 
			{//Ctrl��ϼ�
				ImmUnlockIMC(hIMC);
				if(uKey==g_SettingsG.byHotKeyMode
					||uKey==g_SettingsG.byHotKeyEn
					||uKey==g_SettingsG.byHotKeyQuery
					||uKey==g_SettingsG.byHotKeyMakeWord
					||uKey==g_SettingsG.byHotKeyShowRoot
					||uKey==g_SettingsG.byHotKeyHideStatus
					||(uKey>='0' && uKey<='9') 
					||(uKey>=VK_NUMPAD0 && uKey<=VK_NUMPAD9) 
					) //�ȼ�����Ƶ���ּ�
					return TRUE;
				else
					return FALSE;
			}else if(lpbKeyState[VK_SHIFT]&0x80 && uKey==VK_SPACE)
			{//Shift + VK_SPACE:��Ӣ�ı���л�
				ImmUnlockIMC(hIMC);
				MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,g_SettingsL.bCharMode?IDC_CHARMODE1:IDC_CHARMODE2));
				return TRUE;
			}else
			{
				LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
				BOOL bCoding=KeyIn_IsCoding(lpCntxtPriv);
				if(uKey>='A' && uKey<='Z')
				{//��ĸ����
					if(g_CompMode==IM_SPELL)
					{
						bRet=TRUE;
					}else 
					{
						char cKey=uKey+0x20;	//��VKת�����ַ�
						char i=0;
						if(cKey==ISComm_GetCompInfo()->cWildChar)
						{
							bRet=TRUE;
						}else
						{
							while(ISComm_GetCompInfo()->szCode[i])
							{
								if(ISComm_GetCompInfo()->szCode[i]==cKey)
								{
									bRet=TRUE;
									break;
								}
								i++;
							}
						}
					}
				}else if(uKey>=VK_NUMPAD0 && uKey<=VK_NUMPAD9)
				{//С������������
					bRet=KeyIn_IsNumCode(lpCntxtPriv) || lpCntxtPriv->sCandCount;
				}else if(uKey>=VK_MULTIPLY && uKey<=VK_DIVIDE)
				{//С���̹�ʽ����
					bRet=KeyIn_IsNumCode(lpCntxtPriv);
				}else
				{
					bRet=g_bComp[uKey];
					if(!bRet && lpCntxtPriv->sCandCount)
					{//����Ƿ����û�����ĸ���������ؿ�ݼ�
						bRet= (uKey==g_SettingsG.by2CandVK 
							|| uKey==g_SettingsG.by3CandVK 
							|| uKey==g_SettingsG.byTurnPageDownVK 
							|| uKey==g_SettingsG.byTurnPageUpVK
							);
					}
					if(!bRet && bPressCtrl)
					{//����Ƿ����û�����ĸ��ֿ�ݼ�
						bRet=( uKey==g_SettingsG.byHotKeyEn
							|| uKey==g_SettingsG.byHotKeyMakeWord
							|| uKey==g_SettingsG.byHotKeyMode
							|| uKey==g_SettingsG.byHotKeyQuery
							|| uKey==g_SettingsG.byHotKeyShowRoot
							|| uKey==g_SettingsG.byHotKeyHideStatus
							);
					}
					if(!bRet && lpCntxtPriv->bySyCaret!=0xFF)
						bRet= uKey==VK_DELETE;
					//!������ص��л��������ǿɴ�ӡ�ַ�,��˲���Ҫ�ж�
				}
				if(!bCoding) 
				{//���Ǳ����������
					if(uKey==VK_BACK || uKey==VK_RETURN || uKey==VK_ESCAPE 
						|| uKey==VK_LEFT || uKey==VK_RIGHT 
						|| ( (uKey==VK_UP || uKey==VK_DOWN) && lpCntxtPriv->sCandCount<=g_SettingsG.cPageCands) //���뷭ҳ��������Ҫ��ҳ��ʱ��
						|| uKey==VK_HOME || uKey==VK_END )//�༭���ƶ���꣬������ǰ��
					{
						if(uKey==VK_RETURN && lpbKeyState[VK_SHIFT]&0x80)
						{//Shift+Return:�ظ�����
							bRet=TRUE;
						}else
						{//�༭���ƶ����
							bRet=FALSE;
							ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
							Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
							MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
							if(uKey==VK_ESCAPE)
							{
								DWORD dwStyle=GetWindowLongPtr(g_hWndMsg,GWL_STYLE);
								if(dwStyle&WS_VISIBLE)
								{
									ShowWindow(g_hWndMsg,SW_HIDE);
									bRet=TRUE;
								}
							}else if(uKey==VK_BACK) 
								ISComm_KeyIn("\b",1,0,lpCntxtPriv->hUiWnd);
 							else
 								ISComm_KeyIn(".",1,g_SettingsL.bRecord?MKI_RECORD:0,lpCntxtPriv->hUiWnd);
						}
					}
				}
				ImmUnlockIMCC(lpIMC->hPrivate);
				if(!bRet && !bCoding)	MyGenerateMessage(hIMC,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
			}
		}
	}

	ImmUnlockIMC(hIMC);
	return bRet;
}

//���һ�������Ƿ�Ϊ�������ַ��ͷ
BOOL KeyIn_IsWebHeader(char *pszHeader,int nLen)
{
	BOOL bRet=FALSE;
	int i=0,iBegin=0,iEnd;
	while(g_SettingsG.szWebHeader[i])
	{
		iEnd=iBegin;
		for(;;)
		{
			if(g_SettingsG.szWebHeader[iEnd]==0) break;
			if(g_SettingsG.szWebHeader[iEnd]==0x20) break;
			iEnd++;
		}
		if(nLen==iEnd-iBegin && strncmp(pszHeader,g_SettingsG.szWebHeader+iBegin,nLen)==0)
		{
			bRet=TRUE;
			break;
		}
		if(g_SettingsG.szWebHeader[iEnd]==0) break;//������ַ�������
		iBegin=iEnd+1;
	}
	return bRet;
}

//���벢������
BOOL KeyIn_InputAndAssociate(LPCONTEXTPRIV lpCntxtPriv,char *pszInput,short sLen,BYTE byMask)
{
	ISComm_KeyIn(pszInput,sLen,byMask,lpCntxtPriv->hUiWnd);
	lpCntxtPriv->sbState=SBST_ASSOCIATE;
	lpCntxtPriv->sCandCount=0;
	return TRUE;
}



//���Ŵ���
//BYTE byInput:��������
//char *pBuf:ת��������
int Symbol_Convert(BYTE byInput,char *pBuf,CONST LPBYTE lpbKeyState)
{
	int nRet=0;
	if(g_SettingsL.bCharMode)
	{
		char cType=3;
		if(byInput=='\'')
		{
			static BOOL bLeft1=TRUE;
			cType=bLeft1?1:2;
			bLeft1=!bLeft1;
		}else if(byInput=='\"')
		{
			static BOOL bLeft2=TRUE;
			cType=bLeft2?1:2;
			bLeft2=!bLeft2;
		}
		if(ISComm_SymbolConvert((char)byInput,cType)==ISACK_SUCCESS)
		{
			PMSGDATA pMsg=ISComm_GetData();
			if(pMsg->sSize<20)
			{
				nRet=pMsg->sSize-1;
				strcpy(pBuf,(char*)pMsg->byData);
				if(strncmp(pBuf+nRet-5,"$left",5)==0)
				{//���͹���ƶ�����
					nRet-=5;
					pBuf[nRet]=0;
					if(lpbKeyState[VK_SHIFT]&0x80) 	keybd_event(VK_SHIFT,MapVirtualKey(VK_SHIFT,0),KEYEVENTF_KEYUP,0);
					keybd_event(VK_LEFT,MapVirtualKey(VK_LEFT,0),0,0);
					keybd_event(VK_LEFT,MapVirtualKey(VK_LEFT,0),KEYEVENTF_KEYUP,0);
				}
			}
		}
	}
	if(!nRet)
	{
		pBuf[0]=byInput;
		nRet=1;
	}
	return nRet;
}

//ƴ�����������ƶ�
void SpellBuf_Move(LPCONTEXTPRIV lpCntxtPriv,BYTE byBegin,BYTE bySize,char cDistance)
{
	memmove(lpCntxtPriv->pbyBlur+byBegin+cDistance,lpCntxtPriv->pbyBlur+byBegin,sizeof(LPBYTE)*bySize);
	memmove(lpCntxtPriv->spellData+byBegin+cDistance,lpCntxtPriv->spellData+byBegin,sizeof(SPELLINFO)*bySize);
	memmove(lpCntxtPriv->bySelect+byBegin+cDistance,lpCntxtPriv->bySelect+byBegin,sizeof(BYTE)*bySize);
	memmove(lpCntxtPriv->szWord+byBegin+cDistance,lpCntxtPriv->szWord+byBegin,2*bySize);
}

//���ָ�����ڵ�����
void SpellBuf_ClearSyllable(LPCONTEXTPRIV lpCntxtPriv,BYTE bySyllable)
{
	memset(lpCntxtPriv->pbyBlur+bySyllable,0,sizeof(LPBYTE));
	memset(lpCntxtPriv->spellData+bySyllable,0,sizeof(SPELLINFO));
	memset(lpCntxtPriv->bySelect+bySyllable,0,sizeof(BYTE));
	memset(lpCntxtPriv->szWord+bySyllable,VK_SPACE,2);
}


//��byStartPos���ڿ�ʼԤ�⣬�����ʼ���ڵ�ǰһ���������û�ѡ��ĵ��֣�����ֻ���ΪԤ���ǰ׺����������
void KeyIn_Spell_Forecast(LPCONTEXTPRIV lpCntxtPriv,BYTE byStartPos)
{
	BYTE byEnd=byStartPos;
	while(byEnd<lpCntxtPriv->bySyllables)
	{
		//�ֱ��ҵ�Ԥ�����ʼ��ͽ�����
		BYTE byBegin=byEnd;
		while(lpCntxtPriv->bySelect[byBegin]!=0 && byBegin<lpCntxtPriv->bySyllables) byBegin++;
		byEnd=byBegin;
		while(lpCntxtPriv->bySelect[byEnd]==0 && byEnd<lpCntxtPriv->bySyllables) byEnd++;
		if(byEnd>byBegin)
		{
			char szPrefix[2]={0};
			if(byBegin>0 && lpCntxtPriv->bySelect[byBegin-1]==1)
				memcpy(szPrefix,lpCntxtPriv->szWord[byBegin-1],2);
			if(ISComm_SpellForecast(szPrefix,lpCntxtPriv->pbyBlur+byBegin,byEnd-byBegin)==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=ISComm_GetData();
				BYTE byPrefix=pMsgData->byData[0];
				BYTE *pBuf=pMsgData->byData+1;
				BYTE iPhrase,byPhrases=*pBuf++;
				char *pszWord=(char*)(lpCntxtPriv->szWord+byBegin);
				if(byPrefix==1)
				{//ǰ׺��Ч
					memcpy(pszWord,pBuf+2+2,pBuf[1]-2);
					pszWord+=pBuf[1]-2;
					pBuf+=pBuf[1]+2;
					byPhrases--;
				}
				for(iPhrase=0;iPhrase<byPhrases;iPhrase++)
				{//��������
					memcpy(pszWord,pBuf+2,pBuf[1]);
					pszWord+=pBuf[1];
					pBuf+=pBuf[1]+2;
				}
			}
		}
	}
}

//ƴ��״̬�¸��º�ѡ�ʴ���
void KeyIn_Spell_UpdateCandList(LPCONTEXTPRIV lpCntxtPriv,BYTE byCaret)
{
	DWORD dwRet=ISACK_ERROR;
	BYTE byMask=0;
	ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);
	if(lpCntxtPriv->spellData[byCaret].bySpellLen==0) return;
	if(g_SettingsG.bPYPhraseFirst) byMask=0x01;
	if(byCaret==lpCntxtPriv->bySyllables-1)
	{
		byMask|=0x02;	//�������������ѯ����
		if(lpCntxtPriv->bySyllables>=4) byMask|=0x04;	//��������������
		dwRet=ISComm_SpellQueryCandEx(lpCntxtPriv->pbyBlur,lpCntxtPriv->bySyllables,byMask);
	}else
	{
		if(byCaret==0 && lpCntxtPriv->bySyllables>=4) byMask|=0x04;//��������������
		dwRet=ISComm_SpellQueryCandEx(lpCntxtPriv->pbyBlur+byCaret,lpCntxtPriv->bySyllables-byCaret,byMask);
	}
	
	if(dwRet==ISACK_SUCCESS)
	{
		PMSGDATA pMsgData=ISComm_GetData();
		short i,sCount=0,sValidCount=0;
		LPBYTE pBuf=g_pbyData;
		memcpy(pBuf,pMsgData->byData,pMsgData->sSize);
		memcpy(&sCount,pBuf,2);
		pBuf+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
		for(i=0;i<sCount;i++)
		{
			if(byCaret==(lpCntxtPriv->bySyllables-1))
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+lpCntxtPriv->bySyllables-pBuf[1]/2),pBuf+2,pBuf[1])!=0) //ȥ����Ԥ����ͬ������
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}else
			{
				if(strncmp((char*)(lpCntxtPriv->szWord+byCaret),pBuf+2,pBuf[1])!=0)
					lpCntxtPriv->ppbyCandInfo[sValidCount++]=pBuf;
			}
			pBuf+=pBuf[1]+2;
		}
		lpCntxtPriv->sCandCount=sValidCount;
	}
}

void GetShapeComp(char *pInput,char cLen)
{
	if(ISComm_QueryComp(pInput,cLen)==ISACK_SUCCESS)
	{
		PMSGDATA pData=ISComm_GetData();
		pData->byData[pData->sSize]=0;
		sprintf(g_szTip,"��\"%s\"�ı���=%s",pInput,(char*)pData->byData);
	}else
	{
		sprintf(g_szTip,"��ѯ��\"%s\"�ı���ʧ��",pInput);
	}
}

BOOL KeyIn_Spell_GetSpellInput(LPCONTEXTPRIV lpCntxtPriv,BYTE bySpellID[MAX_SYLLABLES][2])
{
	BOOL bRet=TRUE;
	BYTE i,j,k,iWord;
	PMSGDATA pData=NULL;
	
	for(i=0,iWord=0;i<lpCntxtPriv->bySyllables;i++)
	{
		BOOL  bFind=FALSE;
		if(lpCntxtPriv->spellData[i].bySpellLen==0) continue;
		//�ҵ���ǰ�û�ѡ��ĺ��ֵ�ƴ����ͨ����������ģ��������ƥ����
		ISComm_QueryWordSpell(lpCntxtPriv->szWord[i]);//��ú��ֵ�����ƴ����ID
		pData=ISComm_GetData();
		
		for(j=0;j<pData->byData[0];j++)
		{
			short sBlur=0;
			BYTE  *pbyBlur=lpCntxtPriv->pbyBlur[i];
			memcpy(&sBlur,pbyBlur,2);//ģ��������
			pbyBlur+=4;//ģ����ID,�����������ȫƥ��������
			for(k=0;k<sBlur;k++)
			{
				if(memcmp(pData->byData+1+2*j,pbyBlur+2*k,2)==0)
				{//�õ�������ƴ��ID
					memcpy(bySpellID[iWord],pbyBlur+2*k,2);
					bFind=TRUE;
				}
			}
			if(bFind) break;
		}
		if(!bFind) bRet=FALSE;
		memcpy(lpCntxtPriv->szInput+iWord*2,lpCntxtPriv->szWord[i],2);
		iWord++;
	}
	lpCntxtPriv->nInputLen=iWord*2;
	return bRet;
}

//ƴ��״̬�½�������뵽�༭����
BOOL KeyIn_Spell_InputText(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->bySyllables<=1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;

	if(byInput==VK_SPACE)
	{
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char *pResult=GETLPRESULTSTR(lpCompStr);
		BYTE bySpellID[MAX_SYLLABLES][2];//�û�����ĺ��ֵ�ƴ��������ID
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		if(lpCntxtPriv->nInputLen)
		{
			memcpy(pResult,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
			lpCompStr->dwResultStrLen=lpCntxtPriv->nInputLen;
		}else
		{
			lpCompStr->dwResultStrLen=0;
		}
		if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
		ImmUnlockIMCC(lpIMC->hCompStr);
		//֪ͨӦ�ó����������
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);

		Plugin_TextInput(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,NULL,0,FALSE);

		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL&(~CPC_INPUT));
		//���û������ύ������������
		if(bGetSpID) ISComm_SpellMemoryEx(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,bySpellID);
		KeyIn_InputAndAssociate(lpCntxtPriv,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,GetKeyinMask(!g_bTempSpell,MKI_ALL));
		if(lpCntxtPriv->bPYBiHua)
		{//�˳��ʻ�ѡ������״̬
			lpCntxtPriv->bPYBiHua=FALSE;
			g_szPYBiHua[0]=0;
		}		
		if(g_bTempSpell) 
		{//��ʱƴ��ģʽ�Զ��������ƴ���ı���
			g_bTipShow=TRUE;
			GetShapeComp(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
		}
		if(g_bTempSpell)
		{//������������״̬
			g_bTempSpell=FALSE;
			g_CompMode=IM_SHAPECODE;
			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS);
		}
		bRet=TRUE;
	}else if ( byInput == VK_RETURN && !g_bTempSpell && !lpCntxtPriv->bPYBiHua)
	{//�س��������
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char* pResult=GETLPRESULTSTR(lpCompStr);
		BYTE i;
		lpCompStr->dwResultStrLen=0;
		for(i=0;i<lpCntxtPriv->bySyllables;i++)
		{
			if(lpCntxtPriv->spellData[i].bySpellLen)
			{
				memcpy(pResult+lpCompStr->dwResultStrLen,lpCntxtPriv->spellData[i].szSpell,lpCntxtPriv->spellData[i].bySpellLen);
				lpCompStr->dwResultStrLen+=lpCntxtPriv->spellData[i].bySpellLen;
			}
		}
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		//֪ͨӦ�ó����������
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
		ISComm_KeyIn(pResult,(short)lpCompStr->dwResultStrLen,GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT),lpCntxtPriv->hUiWnd);
		if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
		ImmUnlockIMCC(lpIMC->hCompStr);
		bRet=TRUE;
	}
	ImmUnlockIMCC(lpIMC->hPrivate);
	return bRet;
}


//ƴ��״̬���Ƶ��༭����
BOOL KeyIn_Spell_MoveCaret(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=TRUE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		switch(byInput)
		{
		case VK_LEFT:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret-1,1,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret-1);
				}
				lpCntxtPriv->byCaret--;
			}
			break;
		case VK_HOME:
			if(lpCntxtPriv->byCaret>0)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,0,lpCntxtPriv->byCaret,1);
					SpellBuf_ClearSyllable(lpCntxtPriv,0);
				}
				lpCntxtPriv->byCaret=0;
			}
			break;
		case VK_RIGHT:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,1,-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret+1);
				}
				lpCntxtPriv->byCaret++;
			}
			break;
		case VK_END:
			if(lpCntxtPriv->byCaret<lpCntxtPriv->bySyllables-1)
			{
				if(lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen==0)
				{
					SpellBuf_Move(lpCntxtPriv,lpCntxtPriv->byCaret+1,lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),-1);
					SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
				}
				lpCntxtPriv->byCaret=lpCntxtPriv->bySyllables-1;
			}
			break;
		default:
			bRet=FALSE;
			break;
		}
		if(bRet) 
		{
			if(lpCntxtPriv->bPYBiHua)
			{
				lpCntxtPriv->bPYBiHua=FALSE;
				g_szPYBiHua[0]=0;
			}
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
		}
	}else
	{
		bRet=FALSE;
		//�������
		if(byInput==VK_LEFT)
		{
			if(lpCntxtPriv->bySyCaret>0) 
				lpCntxtPriv->bySyCaret--;
			bRet=TRUE;
		}else if(byInput==VK_RIGHT)
		{
			if(lpCntxtPriv->bySyCaret<lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen)
				lpCntxtPriv->bySyCaret++;
			bRet=TRUE;
		}else if(byInput==VK_HOME)
		{
			lpCntxtPriv->bySyCaret=0;
			bRet=TRUE;
		}else if(byInput==VK_END)
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			bRet=TRUE;
		}
	}
	if(bRet) MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
	return bRet;
}

//ѡ��һ������
//BYTE byInput: Virtual Key
BOOL KeyIn_All_SelectCand(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,char cCompLen,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
	LPBYTE pResult=GETLPRESULTSTR(lpCompStr);
	if(byInput>='0' && byInput<='9' && lpCntxtPriv->sCandCount && ((byInput-'0'+9)%10)<g_SettingsG.cPageCands)
	{
		short iCand=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands+(byInput-'0'+9)%10;
		if(iCand<lpCntxtPriv->sCandCount)
		{//��Ч������
			BYTE byMask=GetKeyinMask(cCompLen==0,MKI_ALL);
			LPBYTE pData=lpCntxtPriv->ppbyCandInfo[iCand];
			bRet=TRUE;

			if(lpCntxtPriv->inState==INST_CODING)
			{//��ͨ��������
				if(lpCntxtPriv->sbState==SBST_NORMAL)
				{//��������״̬
					if(g_CompMode==IM_SPELL)
					{//ƴ������״̬
						BYTE iWord=0;
						BYTE byPhraseLen=pData[1]/2;
						BYTE byCaret=lpCntxtPriv->byCaret;
						if(byCaret==lpCntxtPriv->bySyllables-1)
						{
							if(byPhraseLen>lpCntxtPriv->bySyllables)
								byCaret=0;//��������������
							else
								byCaret-=byPhraseLen-1;
						}
						while(iWord<byPhraseLen)
						{
							memcpy(lpCntxtPriv->szWord[iWord+byCaret],pData+2+iWord*2,2);
							lpCntxtPriv->bySelect[iWord+byCaret]=byPhraseLen;
							iWord++;
						}
						if(lpCntxtPriv->bPYBiHua)
						{//�˳��ʻ�����
							lpCntxtPriv->bPYBiHua=FALSE;
							g_szPYBiHua[0]=0;
						}
						if(byPhraseLen>lpCntxtPriv->bySyllables)
						{//���������볤������
							lpCompStr->dwResultStrLen=pData[1];
							memcpy(pResult,pData+2,lpCompStr->dwResultStrLen);
							lpCntxtPriv->nInputLen=lpCompStr->dwResultStrLen;
							memcpy(lpCntxtPriv->szInput,pResult,lpCntxtPriv->nInputLen);
							Plugin_TextInput(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,NULL,0,FALSE);
							ClearPrivateContextEx(lpCntxtPriv,CPC_ALL&(~CPC_INPUT));
							//֪ͨӦ�ó����������
							g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
							if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
							MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
							MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
	
							KeyIn_InputAndAssociate(lpCntxtPriv,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,GetKeyinMask(!g_bTempSpell,MKI_ALL));
							if(g_bTempSpell) 
							{//��ʱƴ��ģʽ��������ֵı���
								g_bTipShow=TRUE;
								GetShapeComp(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
							}
							if(lpCntxtPriv->bPYBiHua)
							{//�˳��ʻ�ѡ������״̬
								lpCntxtPriv->bPYBiHua=FALSE;
								g_szPYBiHua[0]=0;
							}
							if(g_bTempSpell)
							{//�˳���ʱƴ��
								g_bTempSpell=FALSE;
								g_CompMode=IM_SHAPECODE;
								Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
								MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATESTATUS);
							}
						}else
						{
							KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
							if(byCaret+byPhraseLen<lpCntxtPriv->bySyllables)
							{
								lpCntxtPriv->byCaret+=byPhraseLen;
								KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
								MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
							}else
							{
								KeyIn_Spell_InputText(hIMC,lpIMC,lpCntxtPriv,VK_SPACE,lpbKeyState,lpdwTransKey,pMsgCnt);
							}
						}
						goto end; //ƴ������Ƚ�����
					}else
					{//��ƴ��
						if( (lpbKeyState[VK_CONTROL]&0x80) && (lpbKeyState[VK_SHIFT]&0x80) )
						{//Ctrl+Shift+���ּ�:����ɾ��,��������״̬֧��
							if(lpCntxtPriv->sCandCount>1)
							{//ֻ���ж�������ʱ�����Ч
								char *pPhrase=(char*)pData+1;
								char *pComp=(char *)pPhrase+pPhrase[0]+1;
								char cCompLen=pComp[0];
								if(cCompLen==0)
									pComp=lpCntxtPriv->szComp,cCompLen=lpCntxtPriv->cCompLen;
								else
									pComp++;
								if(pPhrase[0]>2 || !g_SettingsG.bDisableDelWordCand)
								{
									if(ISComm_PhraseDel(pComp,cCompLen,pPhrase+1,pPhrase[0],lpCntxtPriv->hUiWnd)==ISACK_SUCCESS)
									{//ɾ������ɹ�,������ʾ
										memcpy(lpCntxtPriv->ppbyCandInfo+iCand,lpCntxtPriv->ppbyCandInfo+iCand+1,(lpCntxtPriv->sCandCount-iCand-1)*sizeof(LPBYTE));
										lpCntxtPriv->sCandCount--;
										if(lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands > lpCntxtPriv->sCandCount) lpCntxtPriv->sCurCandPage--;
										MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
										goto end;
									}else
									{
										SoundPlay("error");
									}
								}else
								{
									SoundPlay("error");
								}
							}
							SoundPlay("error");
							goto end;
						}else if(pData[0]==RATE_USERCMD)
						{//����ֱͨ������
							CmdExecute(pData);
							lpCompStr->dwResultStrLen=0;
							byMask=0;
						}else
						{//��ͨ��ѡ������
							LPBYTE pComp=pData+2+pData[1];
							if(pData[0]==RATE_USERDEF)
							{//�Զ������
								if(pComp[0]!=0)	pData=pComp;
								else pData++;
								memcpy(pResult,pData+1,pData[0]);
								lpCompStr->dwResultStrLen=pData[0];
								byMask=0;
							}else
							{//�����Զ������
								memcpy(pResult,pData+2,pData[1]);
								lpCompStr->dwResultStrLen=pData[1];
								Plugin_TextInput(pResult,lpCompStr->dwResultStrLen,pData+1+pData[1]+1,pData[1+pData[1]],pData[0]==RATE_WILD);
								if(pData[0]!=RATE_FORECAST)
								{//����Ԥ��ʣ���Ƶ����
									if(lpbKeyState[VK_CONTROL] & 0x80)
									{
										ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pData+2,pData[1],RAM_FAST,lpCntxtPriv->hUiWnd);
									}else if(g_SettingsG.byRateAdjust) 
									{
										ISComm_RateAdjust((char*)pComp+1,pComp[0],(char*)pData+2,pData[1],g_SettingsG.byRateAdjust==1?RAM_AUTO:RAM_FAST,lpCntxtPriv->hUiWnd);
									}
								}else
								{//Ԥ��ʣ��Զ����
									ISComm_MakePhrase((char*)pData+2,pData[1]);
								}
							}
						}
					}
				}else //if(lpCntxtPriv->sbState==SBST_ASSOCIATE || lpCntxtPriv->sbState==SBST_SENTENCE)
				{
					if(g_SettingsG.byAstMode==AST_CAND)
					{//��ǰ�Ǵ�������
						MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_STARTCOMPOSITION, 0, 0);
						memcpy(pResult,pData+3+pData[0],pData[2]-pData[0]);
						lpCompStr->dwResultStrLen=pData[2]-pData[0];
					}else if(g_SettingsG.byAstMode==AST_ENGLISH)
					{//��ǰ��Ӣ������
						MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_STARTCOMPOSITION, 0, 0);
						memcpy(pResult,pData+1,pData[0]);
						lpCompStr->dwResultStrLen=pData[0];
						cCompLen=0;//�����м�������
					}
				}			
			}else if(lpCntxtPriv->inState==INST_ENGLISH)
			{//Ӣ�ĵ�������
				memcpy(pResult,pData+1,pData[0]);
				lpCompStr->dwResultStrLen=pData[0];
				lpCntxtPriv->inState=INST_CODING;
				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				byMask&=~MKI_ASTENGLISH;
			}else if(lpCntxtPriv->inState==INST_USERDEF)
			{//�û��Զ�������
				if(pData[0]==RATE_USERCMD)
				{//����ֱͨ������
					CmdExecute(pData);
					lpCompStr->dwResultStrLen=0;
					byMask=0;
				}else
				{//һ����Զ���
					LPBYTE pComp=pData+2+pData[1];
					if(pComp[0]!=0) 
						pData=pComp;
					else
						pData++;
					memcpy(pResult,pData+1,pData[0]);
					lpCompStr->dwResultStrLen=pData[0];
					lpCntxtPriv->inState=INST_CODING;
					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
					byMask=GetKeyinMask(FALSE,MKI_RECORD|MKI_TTSINPUT);//������
				}
			}else if(lpCntxtPriv->inState==INST_LINEIME)
			{//�ʻ�����״̬
				memcpy(pResult,pData+2,pData[1]);
				lpCompStr->dwResultStrLen=pData[1];
				lpCntxtPriv->inState=INST_CODING;
				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			}
			//֪ͨӦ�ó����������
			lpCntxtPriv->nInputLen=lpCompStr->dwResultStrLen;
			memcpy(lpCntxtPriv->szInput,pResult,lpCntxtPriv->nInputLen);

			g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
			if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
			MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);

			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL&(~CPC_INPUT));
			KeyIn_InputAndAssociate(lpCntxtPriv,(char*)pResult,(short)lpCompStr->dwResultStrLen,byMask);
		}
	}
end:
	ImmUnlockIMCC(lpIMC->hCompStr);
	if(bRet && lpCntxtPriv->bWebMode) lpCntxtPriv->bWebMode=FALSE;
	return bRet;
}


//���뷭ҳ����
//BYTE byInput:Vitual Key
BOOL KeyIn_All_TurnCandPage(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	if(lpCntxtPriv->sCandCount )
	{
		//����״̬���Զ���״ֻ̬��ʹ�����¼�ͷ��ҳ,�Ա�������������ͻ
		if(byInput==VK_DOWN || (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG.byTurnPageDownVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState!=INST_USERDEF))
		{
			if(lpCntxtPriv->sCurCandPage<(lpCntxtPriv->sCandCount+g_SettingsG.cPageCands-1)/g_SettingsG.cPageCands-1)
			{
				lpCntxtPriv->sCurCandPage++;
			}
			bRet=TRUE;
		}else if(byInput==VK_UP || (!(lpbKeyState[VK_SHIFT]&0x80) && byInput==g_SettingsG.byTurnPageUpVK&& lpCntxtPriv->sbState!=SBST_ASSOCIATE && lpCntxtPriv->inState!=INST_USERDEF))
		{
			if(lpCntxtPriv->sCurCandPage>0) 
			{
				lpCntxtPriv->sCurCandPage--;
			}
			bRet=TRUE;
		}
	}
	if(bRet){
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
		if(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_ASSOCIATE)
		{//����״̬�ķ�ҳ,�ȹر���ʱ��ʱ���ٿ���
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_KILLDELAY);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SETDELAY);
		}
		if(lpCntxtPriv->inState==INST_CODING && g_CompMode==IM_SPELL)
		{//ƴ��״̬�Զ����µ�ǰ�ַ�
			LPBYTE pCand=NULL;
			short iCand=lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands;
			short iPageEnd=iCand+g_SettingsG.cPageCands;
			if(iPageEnd>lpCntxtPriv->sCandCount-1) iPageEnd=lpCntxtPriv->sCandCount-1;
			while(iCand<=iPageEnd)
			{//�ҵ���һ���ĵ�һ������
				pCand=lpCntxtPriv->ppbyCandInfo[iCand];
				if(pCand[1]==2) break;
				iCand++;
			}
			if(iCand<=iPageEnd)
			{//����ҳ������µ�Ԥ�⣬��ͬ���ֶ�ѡ��
				memcpy(lpCntxtPriv->szWord+lpCntxtPriv->byCaret,pCand+2,2);
				lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=1;
				KeyIn_Spell_Forecast(lpCntxtPriv,lpCntxtPriv->byCaret);
				MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
			}
		}
	}

	return bRet;
}

//ƴ���ıʻ���������ѡ��״̬:�������
BOOL KeyIn_PYBiHua_ChangComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	int nLen=strlen(g_szPYBiHua);
	
	if(byInput == VK_BACK)
	{
		if(nLen>0)
		{
			g_szPYBiHua[--nLen]=0;
			bRet=TRUE;
		}else
		{//�˳��ʻ�����״̬
			if(lpCntxtPriv->bPYBiHua) lpCntxtPriv->bPYBiHua=FALSE;
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
			return TRUE;
		}
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{
		if(nLen<50)
		{
			g_szPYBiHua[nLen++]=byInput-VK_NUMPAD1+'1';
			g_szPYBiHua[nLen]=0;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->bPYBiHua=FALSE;
		g_szPYBiHua[0]=0;
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
		return TRUE;
	}
	if(bRet)
	{
		char *pszWordList=(char*)malloc(lpCntxtPriv->sCandCount*2);
		short i;
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[i];
			memcpy(pszWordList+i*2,pbyCand+2,2);
		}
		if(ISComm_SortWordByBiHua(g_szPYBiHua,nLen,pszWordList,lpCntxtPriv->sCandCount)==ISACK_SUCCESS)
		{
			PMSGDATA pData=ISComm_GetData();
			for(i=0;i<lpCntxtPriv->sCandCount;i++)
			{
				LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[i];
				memcpy(pbyCand+2,pData->byData+i*2,2);
			}
			memcpy(lpCntxtPriv->szWord+lpCntxtPriv->byCaret,pData->byData,2);//�Զ�ѡ���һ������
			lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=1;					//�趨Ϊ�Զ�ѡ�񣬲������Զ��޸�
			lpCntxtPriv->sCurCandPage=0;
		}else
		{
			SoundPlay("error");
		}
		free(pszWordList);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
	}
	return bRet;
}

//ƴ��״̬�¸ı����
BOOL KeyIn_Spell_ChangeComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	LPSPELLINFO pSpInfo=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
	char (*pComp)[2]=lpCntxtPriv->szWord+lpCntxtPriv->byCaret;
	BYTE byCaret=lpCntxtPriv->byCaret;
	if(byInput>='a' && byInput<='z')
	{
		bCompChar=TRUE;
		if(pSpInfo->bySpellLen==0)
		{
			if(byInput=='u' || byInput=='v')
			{
				if(!g_bTempSpell && lpCntxtPriv->bySyllables==1)
				{//�л����û��Զ���ģʽ
					ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
					lpCntxtPriv->inState=INST_USERDEF;
					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);

					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
				}
				bCompChar=FALSE;
			}else if(byInput=='i')
			{
				if(!g_bTempSpell && lpCntxtPriv->bySyllables==1)
				{//�л����ʻ�����״̬
					ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
					lpCntxtPriv->inState=INST_LINEIME;
					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);

					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
				}
				bCompChar=FALSE;
			}else
			{
				if(g_SettingsG.bShowOpTip && !g_bTempSpell)
				{
					g_bTipShow=TRUE;
					Tips_Rand(TRUE,g_szTip);
				}
			}
		}
		if(!bCompChar)	return lpCntxtPriv->bySyllables==1;
	}else if(byInput=='\'' && pSpInfo->bySpellLen)
	{
		bCompChar=TRUE;
	}

	if(bCompChar)
	{
		PMSGDATA pData;
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{//��ʼ��������,���ɿ�ʼ������Ϣ�Ի�ȡ������ʱ���봰�ڵ�����
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND);
		}
		pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
		if(ISComm_Spell2ID(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
		{//���µ�ǰ���ڱ༭������
			ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
			pData=ISComm_GetData();
			if(lpCntxtPriv->pbyBlur[byCaret]) free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
			memcpy(lpCntxtPriv->pbyBlur[byCaret],pData->byData,pData->sSize);
			lpCntxtPriv->bySelect[byCaret]=0;
		}else
		{//����һ��������
			pSpInfo->bySpellLen--;
			if(lpCntxtPriv->bySyllables<MAX_SYLLABLES)
			{
				//����ǰ�����������ں���һ������,������һ�����ڿռ�
				SpellBuf_Move(lpCntxtPriv,
					byCaret+1,
					lpCntxtPriv->bySyllables-(byCaret+1),
					1);
				lpCntxtPriv->bySyllables++;
				lpCntxtPriv->byCaret++;
				//��յ�ǰ��������
				SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->byCaret);
				pSpInfo++;
				pComp++;
				byCaret++;
				//���ݵ�ǰ������д��ǰ��������
				if(byInput!='\'')
				{
					pSpInfo->szSpell[pSpInfo->bySpellLen++]=byInput;
					if(ISComm_Spell2ID(pSpInfo->szSpell,1)==ISACK_SUCCESS)
					{
						pData=ISComm_GetData();
					}else
					{//����ƴ����������ͨ������ĸ�γ�ƴ��
						pData=ISComm_GetData();
						pData->byData[0]=0;
					}
					if(pData->byData[0]==0 && 
						lpCntxtPriv->bySyllables>1 &&
						byCaret==lpCntxtPriv->bySyllables-1 && 
						lpCntxtPriv->spellData[byCaret-1].bySpellLen>1 )
					{//���ڱ༭���һ�����ڣ���ǰ����Ĳ�����ĸ���ж��Ƿ���Ҫ��ǰһ�����ڽ�һ����ĸ
						SPELLINFO siPrev,siCur;
						siPrev=lpCntxtPriv->spellData[byCaret-1];
						siCur=lpCntxtPriv->spellData[byCaret];
						siCur.szSpell[0]=siPrev.szSpell[siPrev.bySpellLen-1];
						siCur.szSpell[1]=byInput;
						siCur.bySpellLen=2;
						siPrev.bySpellLen--;
						if(ISComm_Spell2ID(siPrev.szSpell,siPrev.bySpellLen)==ISACK_SUCCESS &&
							ISComm_Spell2ID(siCur.szSpell,siCur.bySpellLen)==ISACK_SUCCESS)
						{//����ĸ�ɹ�,�޸�ǰһ��������
							_ASSERT(lpCntxtPriv->pbyBlur[byCaret-1]);
							free(lpCntxtPriv->pbyBlur[byCaret-1]);	//ע���ͷ�ԭƴ��ռ�õĶ��ڴ�
							SpellBuf_ClearSyllable(lpCntxtPriv,byCaret-1);
							ISComm_SpellGetBlur(siPrev.szSpell,siPrev.bySpellLen);
							pData=ISComm_GetData();
							lpCntxtPriv->pbyBlur[byCaret-1]=(LPBYTE)malloc(pData->sSize);
							memcpy(lpCntxtPriv->pbyBlur[byCaret-1],pData->byData,pData->sSize);
							lpCntxtPriv->spellData[byCaret-1]=siPrev;
							lpCntxtPriv->spellData[byCaret]=siCur;
						}
					}
					if(ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen)==ISACK_SUCCESS)
					{
						pData=ISComm_GetData();
						lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
						memcpy(lpCntxtPriv->pbyBlur[byCaret],
							pData->byData,
							pData->sSize);
					}else
					{//�����ʱ����
						pSpInfo->bySpellLen=0;
					}
				}
			}
		}
		bRet=TRUE;
	}else if(byInput==VK_BACK)
	{
		if(pSpInfo->bySpellLen)
		{
			pSpInfo->bySpellLen--;
			free(lpCntxtPriv->pbyBlur[byCaret]);
			lpCntxtPriv->pbyBlur[byCaret]=NULL;
			lpCntxtPriv->bySelect[byCaret]=0;
			memset(lpCntxtPriv->szWord+byCaret,0x20,2);
			if(pSpInfo->bySpellLen)
			{
				PMSGDATA pData;
				ISComm_SpellGetBlur(pSpInfo->szSpell,pSpInfo->bySpellLen);
				pData=ISComm_GetData();
				lpCntxtPriv->pbyBlur[byCaret]=(LPBYTE)malloc(pData->sSize);
				memcpy(lpCntxtPriv->pbyBlur[byCaret],
					pData->byData,
					pData->sSize);
			}else if(byCaret==lpCntxtPriv->bySyllables-1 && lpCntxtPriv->bySyllables>1)
			{//ɾ�����һ�����ڵ�Ψһ��ĸ
				lpCntxtPriv->bySyllables--;
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(lpCntxtPriv->bySyllables>1)//���ٱ�֤Ҫ��һ�����ڿռ�
		{//�����ǰ�Ŀհ�����
			SpellBuf_Move(lpCntxtPriv,
				lpCntxtPriv->byCaret+1,
				lpCntxtPriv->bySyllables-(lpCntxtPriv->byCaret+1),
				-1);
			SpellBuf_ClearSyllable(lpCntxtPriv,lpCntxtPriv->bySyllables-1);
			lpCntxtPriv->bySyllables--;
			if(lpCntxtPriv->byCaret==lpCntxtPriv->bySyllables)
			{
				lpCntxtPriv->byCaret--;
				pSpInfo--;
				byCaret--;
			}
			bRet=TRUE;
		}else if(g_bTempSpell)
		{//temp spell mode will ignore VK_BACK key.
			bRet=TRUE;
		}
		if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
		{
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_ENDCOMPOSITION,0,0);
			if(!g_bTempSpell) MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_SETDELAY);
		}
	}else if(byInput==VK_ESCAPE)
	{
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_ENDCOMPOSITION,0,0);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,(g_bTempSpell?IMN_PRIV_UPDATESTATUS:0)|IMN_PRIV_SETDELAY);
		if(g_bTempSpell)
		{//restore shape code input mode
			g_bTempSpell=FALSE;
			g_CompMode=IM_SHAPECODE;
			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		}	
		bRet=TRUE;
	}

	if(bRet )
	{
		if(lpCntxtPriv->bySyllables>1) g_bTipShow=FALSE;
		ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);//�����ǰ����
		if(pSpInfo->bySpellLen)
		{
			//����Ԥ�Ȿ��ĺ�ѡ��
			KeyIn_Spell_Forecast(lpCntxtPriv,0);
			//��ȡ��ǰ�����λ�õ�����
			KeyIn_Spell_UpdateCandList(lpCntxtPriv,byCaret);
		}
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
	}
	return bRet;
}

//�������
BOOL KeyIn_Spell_Symbol(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	if(!g_bTempSpell && (lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0 || lpCntxtPriv->bySyllables==1))
	{//�������
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char *pResult=GETLPRESULTSTR(lpCompStr);
		BYTE bySpellID[MAX_SYLLABLES][2];
		BOOL bGetSpID=KeyIn_Spell_GetSpellInput(lpCntxtPriv,bySpellID);
		if(lpCntxtPriv->nInputLen)
		{
			if(bGetSpID) ISComm_SpellMemoryEx(lpCntxtPriv->szInput,lpCntxtPriv->nInputLen,bySpellID);
			memcpy(pResult,lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
			lpCompStr->dwResultStrLen=lpCntxtPriv->nInputLen;
		}else
		{
			lpCompStr->dwResultStrLen=0;
		}
		lpCompStr->dwResultStrLen+=Symbol_Convert(byInput,pResult+lpCompStr->dwResultStrLen,lpbKeyState);
		ISComm_KeyIn(pResult,(short)lpCompStr->dwResultStrLen,g_SettingsL.bRecord?MKI_RECORD:0,lpCntxtPriv->hUiWnd);
		g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
		if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
		ImmUnlockIMCC(lpIMC->hCompStr);
		
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_STARTCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY, IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);

		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		bRet=TRUE;
	}
	return bRet;
}

//ͨ����д��������λ����
BOOL KeyIn_Spell_Locate(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	if(byInput>='A' && byInput<='Z' && !lpCntxtPriv->bPYBiHua && lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen!=0)
	{//��д����,�Ǳʻ�����״̬���ǿ���״̬
		BYTE byCaret=lpCntxtPriv->byCaret+1;
		byInput+=0x20;
		while(byCaret<lpCntxtPriv->byCaret+lpCntxtPriv->bySyllables)
		{
			if(lpCntxtPriv->spellData[byCaret%lpCntxtPriv->bySyllables].szSpell[0]==byInput)
			{
				lpCntxtPriv->byCaret=byCaret%lpCntxtPriv->bySyllables;
				KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
				bRet=TRUE;
				break;
			}
			byCaret++;
		}
	}
	return bRet;
}

BOOL KeyIn_Spell_SyFix(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
						CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0) return FALSE;
	if(lpCntxtPriv->bySyCaret==0xFF)
	{
		if(!lpCntxtPriv->bPYBiHua && byInput==VK_RETURN && !(lpbKeyState[VK_SHIFT]&0x80))
		{
			lpCntxtPriv->bySyCaret=lpCntxtPriv->spellData[lpCntxtPriv->byCaret].bySpellLen;
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
			return TRUE;
		}
	}else
	{
		BOOL bRet=FALSE;
		if(byInput==VK_RETURN)
		{//�˳������޸�״̬
			if(lpCntxtPriv->sCandCount)
				lpCntxtPriv->bySyCaret=0xFF;
			else
			{
				g_bTipShow=TRUE;
				strcpy(g_szTip,"���ڴ���!");
			}
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
			return TRUE;
		}else
		{//�����޸ļ�
			BOOL bModified=FALSE;
			SPELLINFO * lpSpi=lpCntxtPriv->spellData+lpCntxtPriv->byCaret;
			if(byInput==VK_BACK)
			{
				if(lpCntxtPriv->bySyCaret>0)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret-1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->bySpellLen--;
					lpCntxtPriv->bySyCaret--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput==VK_DELETE)
			{
				if(lpCntxtPriv->bySyCaret<lpSpi->bySpellLen)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret-1);
					lpSpi->bySpellLen--;
					bModified=TRUE;
				}
				bRet=TRUE;
			}else if(byInput>='a' && byInput<='z')
			{//��������
				if(lpSpi->bySpellLen<6)
				{
					memmove(lpSpi->szSpell+lpCntxtPriv->bySyCaret+1,
						lpSpi->szSpell+lpCntxtPriv->bySyCaret,
						lpSpi->bySpellLen-lpCntxtPriv->bySyCaret);
					lpSpi->szSpell[lpCntxtPriv->bySyCaret]=byInput;
					lpSpi->bySpellLen++;
					lpCntxtPriv->bySyCaret++;
					bModified=TRUE;
				}
				bRet=TRUE;
			}
			if(bModified)
			{//��ǰ���ڱ��޸ģ���������
				g_bTipShow=FALSE;
				if(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret])
				{//ɾ��ԭ������
					free(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]);
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=NULL;
					memset(lpCntxtPriv->szWord[lpCntxtPriv->byCaret],0x20,2);
					ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);//�����ǰ����
				}
				lpCntxtPriv->bySelect[lpCntxtPriv->byCaret]=0;
				if(lpSpi->bySpellLen==0)
				{//�Զ��˳����ڱ༭״̬
					lpCntxtPriv->bySyCaret=0xFF;
				}else if(ISComm_Spell2ID(lpSpi->szSpell,lpSpi->bySpellLen)==ISACK_SUCCESS)
				{//���µ�ǰ���ڱ༭������
					PMSGDATA pData=NULL;
					ISComm_SpellGetBlur(lpSpi->szSpell,lpSpi->bySpellLen);
					pData=ISComm_GetData();
					lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret]=(LPBYTE)malloc(pData->sSize);
					memcpy(lpCntxtPriv->pbyBlur[lpCntxtPriv->byCaret],pData->byData,pData->sSize);
					//����Ԥ�Ȿ��ĺ�ѡ��
					KeyIn_Spell_Forecast(lpCntxtPriv,0);
					//��ȡ��ǰ�����λ�õ�����
					KeyIn_Spell_UpdateCandList(lpCntxtPriv,lpCntxtPriv->byCaret);
				}
				MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
			}
			return bRet;
		}
	}
	return FALSE;
}

BOOL KeyIn_Spell_Normal(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bHandle=KeyIn_Spell_SyFix(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	if(!bHandle) bHandle=KeyIn_Spell_ChangeComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	if(!bHandle) bHandle=KeyIn_Spell_InputText(	hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	if(!bHandle) bHandle=KeyIn_Spell_Locate(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	if(!bHandle) bHandle=KeyIn_Spell_Symbol(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	return bHandle;
}

int MakeText(char *text,int nSize,char *buf)
{
	int nLen=0;
	nLen+=sprintf(buf+nLen,g_SettingsG.szPhonticLeft);
	strncpy(buf+nLen,text,nSize);
	nLen+=nSize;
	nLen+=sprintf(buf+nLen,g_SettingsG.szPhonticRight);
	return nLen;
}

int MakeRTF(char *text,int nSize,char *buf)
{
	int nLen=sprintf(buf,"{\\rtf1 {\\fonttbl {\\f0 \\'cb\\'ce\\'cc\\'e5;} {\\f1 %s;}}\r\n{\\colortbl ;\\red%u\\green%u\\blue%u;}",
		ISComm_GetFlmInfo()->szAddFont,
		g_SettingsG.byPntcRed,g_SettingsG.byPntcGreen,g_SettingsG.byPntcBlue);
	nLen+=sprintf(buf+nLen,"%s\\cf1\\f1 ",g_SettingsG.szPhonticLeft);
	strncpy(buf+nLen,text,nSize);
	nLen+=nSize;
	nLen+=sprintf(buf+nLen,"\\cf0\\f0 %s}",g_SettingsG.szPhonticRight);
	return nLen;
}

int MakeHTML(char *text,int nSize,char *buf) {
	// Create temporary buffer for HTML header...
	char *ptr;
	int nLen=0;
	// Create a template string for the HTML header...
	strcpy(buf,
		"Version:0.9\r\n"
		"StartHTML:00000000\r\n"
		"EndHTML:00000000\r\n"
		"StartFragment:00000000\r\n"
		"EndFragment:00000000\r\n"
		"<html><body>\r\n"
		"<!--StartFragment -->\r\n");
	
	// Append the HTML...
	nLen=strlen(buf);
	nLen+=sprintf(buf+nLen,g_SettingsG.szPhonticLeft);
	nLen+=sprintf(buf+nLen,"<font face=\"%s\" color=\"#%02X%02X%02X\">",		
		ISComm_GetFlmInfo()->szAddFont,
		g_SettingsG.byPntcRed,g_SettingsG.byPntcGreen,g_SettingsG.byPntcBlue);
	strncpy(buf+nLen,text,nSize);
	nLen+=nSize;
	nLen+=sprintf(buf+nLen,"</font> \r\n");
	nLen+=sprintf(buf+nLen,g_SettingsG.szPhonticRight);
	// Finish up the HTML format...
	nLen+=sprintf(buf+nLen,
		"<!--EndFragment-->\r\n"
		"</body>\r\n"
		"</html>");
	
	// Now go back, calculate all the lengths, and write out the
	// necessary header information. Note, wsprintf() truncates the
	// string when you overwrite it so you follow up with code to replace
	// the 0 appended at the end with a '\r'...
	ptr = strstr(buf, "StartHTML");
	sprintf(ptr+10, "%08u", strstr(buf, "<html>") - buf);
	*(ptr+10+8) = '\r';
	
	ptr = strstr(buf, "EndHTML");
	sprintf(ptr+8, "%08u", strlen(buf));
	*(ptr+8+8) = '\r';
	
	ptr = strstr(buf, "StartFragment");
	sprintf(ptr+14, "%08u", strstr(buf, "<!--StartFrag") - buf);
	*(ptr+14+8) = '\r';
	
	ptr = strstr(buf, "EndFragment");
	sprintf(ptr+12, "%08u", strstr(buf, "<!--EndFrag") - buf);
	*(ptr+12+8) = '\r';
	return nLen;
} 

void PutEnAdd2Clip(char *pszAddInfo)
{
	static cfRtf=0;
	static cfHtml=0;
	HWND hFocus=GetFocus();
	if(cfRtf==0) cfRtf=RegisterClipboardFormat(TEXT("Rich Text Format"));
	if(cfHtml==0) cfHtml=RegisterClipboardFormat(TEXT("HTML Format"));
	if(OpenClipboard(hFocus))
	{
		int nFmtTxtLen=strlen(g_SettingsG.szPhonticLeft)+strlen(g_SettingsG.szPhonticRight);
		HGLOBAL hMemText=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,1+nFmtTxtLen+pszAddInfo[0]);
		HGLOBAL hMemRtf=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,400+nFmtTxtLen+pszAddInfo[0]);
		HGLOBAL hMemHtml=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,400+nFmtTxtLen+pszAddInfo[0]);
		LPSTR pszBuf=(LPSTR)GlobalLock(hMemText);
		MakeText(pszAddInfo+1,pszAddInfo[0],pszBuf);
		GlobalUnlock(hMemText);
		pszBuf=(LPSTR)GlobalLock(hMemRtf);
		MakeRTF(pszAddInfo+1,pszAddInfo[0],pszBuf);
		GlobalUnlock(hMemRtf);
		pszBuf=(LPSTR)GlobalLock(hMemHtml);
		MakeHTML(pszAddInfo+1,pszAddInfo[0],pszBuf);
		GlobalUnlock(hMemHtml);
		EmptyClipboard(); 
		SetClipboardData(cfRtf,hMemRtf);
		SetClipboardData(cfHtml,hMemHtml);
		SetClipboardData(CF_TEXT,hMemText);
		CloseClipboard();
		g_bDelayPhontic=TRUE;
	}
}

//Ӣ�ĵ���״̬�����봦��
BOOL KeyIn_Code_English(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	_ASSERT(lpCntxtPriv->inState==INST_ENGLISH);
	if((byInput>='A' && byInput<='Z')||(byInput>='a' && byInput<='z') || byInput==VK_BACK || byInput==VK_SPACE || byInput=='\'')
	{
		if(byInput==VK_BACK)
		{
			if(lpCntxtPriv->cCompLen) 
			{
				lpCntxtPriv->cCompLen--;
				if(lpCntxtPriv->cCompLen==0)
				{
					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_ENDCOMPOSITION,0,0);
					MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
					lpCntxtPriv->inState=INST_CODING;
					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				}
			}
		}else
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cCompLen++]=byInput;
		}
		ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);
		if(lpCntxtPriv->cCompLen)
		{//�����Ѿ��޸�
			if(ISComm_EnQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen)==ISACK_SUCCESS)
			{
				PMSGDATA pData=ISComm_GetData();
				LPBYTE  pBuf=g_pbyData;
				BYTE i,byCount=0;
				memcpy(g_pbyData,pData->byData,pData->sSize);
				lpCntxtPriv->pbyEnSpell=pBuf;
				pBuf+=1+pBuf[0];
				lpCntxtPriv->pbyEnPhontic=pBuf;
				pBuf+=1+pBuf[0];
				byCount=*pBuf++;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(byCount*sizeof(LPBYTE));
				for(i=0;i<byCount;i++)
				{
					lpCntxtPriv->ppbyCandInfo[i]=pBuf;
					pBuf+=1+pBuf[0];
				}
				lpCntxtPriv->sCandCount=byCount;
				lpCntxtPriv->sCurCandPage=0;
			}else
			{
				lpCntxtPriv->pbyEnSpell=NULL;
				lpCntxtPriv->pbyEnPhontic=NULL;
				lpCntxtPriv->pbyEnAstPhrase=NULL;
			}
			//������ʾ
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
		}
	}else if(byInput==VK_RETURN)
	{//���뵱ǰӢ��
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char * pResult=GETLPRESULTSTR(lpCompStr);
		if(lpCntxtPriv->pbyEnSpell)
		{
			memcpy(pResult,lpCntxtPriv->pbyEnSpell+1,lpCntxtPriv->pbyEnSpell[0]);
			lpCompStr->dwResultStrLen=lpCntxtPriv->pbyEnSpell[0];
		}else
		{
			memcpy(pResult,lpCntxtPriv->szComp,lpCntxtPriv->cCompLen);
			lpCompStr->dwResultStrLen=lpCntxtPriv->cCompLen;
		}
		if(g_SettingsL.bSound) ISComm_TTS(pResult,(char)lpCompStr->dwResultStrLen,MTTS_EN);
		ImmUnlockIMCC(lpIMC->hCompStr);
		if(lpbKeyState[VK_SHIFT]&0x80 && lpCntxtPriv->pbyEnPhontic && lpCntxtPriv->pbyEnPhontic[0])
		{//����������Ϣ
			PutEnAdd2Clip(lpCntxtPriv->pbyEnPhontic);
		}
		//���뵥��
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_UPDATEINPUTWND|IMN_PRIV_SETDELAY);

		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		
		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
	}else if(byInput==VK_ESCAPE)
	{//�������
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_UPDATEINPUTWND|IMN_PRIV_SETDELAY);
	}
	return TRUE;
}

void KeyIn_Sent_Input(LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	if(lpCntxtPriv->sSentLen && lpCntxtPriv->sSentCaret)
	{
		BYTE byMask=0;
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char* pResult=GETLPRESULTSTR(lpCompStr);
		if(g_SettingsL.bRecord)
			byMask|=MKI_RECORD;
		if(g_SettingsG.bTTSInput)
			byMask|=MKI_TTSINPUT;
		
		lpCompStr->dwResultStrLen=lpCntxtPriv->pbySentWord[lpCntxtPriv->sSentCaret]-lpCntxtPriv->pbySentWord[0];
		memcpy(pResult,lpCntxtPriv->pbySentWord[0],lpCompStr->dwResultStrLen);
		pResult[lpCompStr->dwResultStrLen]=0;
		ISComm_KeyIn(pResult,(short)lpCompStr->dwResultStrLen,byMask,lpCntxtPriv->hUiWnd);
		g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
		if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
		ImmUnlockIMCC(lpIMC->hCompStr);
		
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);
	}
}

//�������״̬�µ����봦��
BOOL KeyIn_All_Sentence(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	_ASSERT(lpCntxtPriv->inState==INST_CODING && lpCntxtPriv->sbState==SBST_SENTENCE);
	if(byInput==',')
	{
		if(lpCntxtPriv->sSentCaret<lpCntxtPriv->sSentWords)
			lpCntxtPriv->sSentCaret++;
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_INVALIDINPUTWND);
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->sSentCaret>0)
			lpCntxtPriv->sSentCaret--;
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_INVALIDINPUTWND);
	}else if((g_CompMode==IM_SHAPECODE && byInput==g_cHotKeySent) ||(g_CompMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentCaret==0) return FALSE;//����FALSE,�Ա��������������
	}else if(byInput=='.')
	{
		lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentWords;
		KeyIn_Sent_Input(lpIMC,lpCntxtPriv,lpdwTransKey,pMsgCnt);
	}else if(byInput==VK_SPACE)
	{
		if(lpCntxtPriv->sSentCaret==0) lpCntxtPriv->sSentCaret=lpCntxtPriv->sSentWords;
		KeyIn_Sent_Input(lpIMC,lpCntxtPriv,lpdwTransKey,pMsgCnt);
	}else if(byInput==VK_ESCAPE)
	{
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);
	}

	if(byInput>='0' && byInput<='9')
		return FALSE;//��������������ѡ��ӿڼ���ִ��
	return TRUE;
}

BOOL KeyIn_Code_IsMaxCode(LPCONTEXTPRIV lpCntxtPriv,BYTE byType)
{
	if(lpCntxtPriv->cCompLen==0) return FALSE;
	return (lpCntxtPriv->cCompLen>=ISComm_GetCompInfo()->cCodeMax &&  byType==MCR_NORMAL);
}

BOOL KeyIn_Code_IsValidComp(LPCONTEXTPRIV lpCntxtPriv,char cInput)
{
	BYTE byMask=0;
	if(lpCntxtPriv->cCompLen==MAX_COMP-1) return FALSE;
	if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
	if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
	lpCntxtPriv->szComp[lpCntxtPriv->cCompLen]=cInput;
	return ISComm_CheckComp(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen+1,byMask);
}

//��ͨ����״̬�±���ı䴦��
BOOL KeyIn_Code_ChangeComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==ISComm_GetCompInfo()->cWildChar)
	{
		bCompChar=TRUE;
	}else
	{
		char i=0;
		while(ISComm_GetCompInfo()->szCode[i])
		{
			if(ISComm_GetCompInfo()->szCode[i]==byInput)
			{
				bCompChar=TRUE;
				break;
			}
			i++;
		}
	}
	if(bCompChar)
	{
		if(lpCntxtPriv->sbState==SBST_ASSOCIATE) 
		{
			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		}

		if(KeyIn_Code_IsMaxCode(lpCntxtPriv,MCR_NORMAL) 
			&& !KeyIn_Code_IsValidComp(lpCntxtPriv,byInput)
			&& g_SettingsG.bAutoInput 
			&& !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount)
			{
				BYTE byType=lpCntxtPriv->ppbyCandInfo[0][0];
				//��ֹ��������ʱ���ִ���:��㲻�����ױ���,�˳���ǰ����,�����㶥����������
				if((byInput<'a' || byInput>'z') && !ISComm_GetCompInfo()->bSymbolFirst) return FALSE;
				if(byType!=RATE_FORECAST && (byType!=RATE_GBK || g_SettingsG.nGbkMode==2)) 
				{
					KeyIn_All_SelectCand(hIMC,lpIMC,lpCntxtPriv,'1',1,lpbKeyState,lpdwTransKey,pMsgCnt);
					bRet=TRUE;
				}
			}
			lpCntxtPriv->cCompLen=0;
		}
		
		if(lpCntxtPriv->cCompLen==0 && !bRet)
		{
			if((byInput<'a' || byInput>'z') && (byInput!=g_cHotKeyUD || !g_SettingsG.bFastUMode) )
			{//��㣺Ҫô�����Զ���ģʽ��ݼ������߲�֧�ֿ���Զ���ģʽ�л�
				if(!ISComm_GetCompInfo()->bSymbolFirst || byInput==g_cHotKeyUD) return FALSE;//���Ŷ�������
			}
			if(g_SettingsG.bShowOpTip)
			{
				g_bTipShow=TRUE;
				Tips_Rand(FALSE,g_szTip);
			}
			//��ʼ��������,���ɿ�ʼ������Ϣ�Ի�ȡ������ʱ���봰�ڵ�����
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND);
			if(byInput==g_cHotKeyUD && g_SettingsG.bFastUMode)
			{//�л����û��Զ�������״̬
				ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
				lpCntxtPriv->inState=INST_USERDEF;
				g_bTipShow=FALSE;
				Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
				MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
				return TRUE;
			}
		}
		if(lpCntxtPriv->cCompLen<MAX_COMP)
		{
			lpCntxtPriv->szComp[lpCntxtPriv->cCompLen++]=byInput;
			bRet=TRUE;
		}
	}else if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cCompLen>0) lpCntxtPriv->cCompLen--;
		if(lpCntxtPriv->bWebMode && lpCntxtPriv->cCompLen<=2) lpCntxtPriv->bWebMode=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE)
	{
		lpCntxtPriv->cCompLen=0;
		lpCntxtPriv->sbState=SBST_NORMAL;
		lpCntxtPriv->bWebMode=FALSE;
		g_bTipShow=FALSE;
		bRet=TRUE;
	}else if(byInput==VK_RETURN)
	{
		BOOL bClearComp= lpCntxtPriv->bWebMode==FALSE 
			&& ((g_SettingsG.bEnterClear && !(lpbKeyState[VK_SHIFT]&0x80))||(!g_SettingsG.bEnterClear && lpbKeyState[VK_SHIFT]&0x80 ));
		if(bClearComp)
		{//��ձ���
			lpCntxtPriv->cCompLen=0;
			lpCntxtPriv->sbState=SBST_NORMAL;
			bRet=TRUE;
		}else 
		{//��������
			if(lpCntxtPriv->bWebMode && lpbKeyState[VK_SHIFT]&0x80)
			{//��ַģʽ�´������
				lpCntxtPriv->szComp[lpCntxtPriv->cCompLen]=0;
				ShellExecute(NULL,"open",lpCntxtPriv->szComp,NULL,NULL,SW_SHOWDEFAULT);
			}else
			{
				BYTE byMask=0;
				LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
				lpCompStr->dwResultStrLen=lpCntxtPriv->cCompLen;
				memcpy(GETLPRESULTSTR(lpCompStr),lpCntxtPriv->szComp,lpCntxtPriv->cCompLen);
				
				if(g_SettingsL.bRecord)
					byMask|=MKI_RECORD;
				if(g_SettingsG.bTTSInput)
					byMask|=MKI_TTSINPUT;
				ISComm_KeyIn(GETLPRESULTSTR(lpCompStr),(short)lpCompStr->dwResultStrLen,byMask,lpCntxtPriv->hUiWnd);
				ImmUnlockIMCC(lpIMC->hCompStr);
				MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
			}
			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_ENDCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
			return TRUE;
		}
	}else if(byInput==g_cHotKeyUD)
	{//�û�������Զ���״̬�л���
		if(lpCntxtPriv->cCompLen==0 && g_SettingsG.bFastUMode)
		{//�л����û��Զ�������״̬
			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
			lpCntxtPriv->inState=INST_USERDEF;
			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND|IMN_PRIV_UPDATEINPUTWND);
			return TRUE;
		}
	}else if(lpCntxtPriv->cCompLen < MAX_COMP)
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cCompLen++]=byInput;
		if(lpCntxtPriv->bWebMode || (!g_SettingsG.bAutoInput && byInput>='a' && byInput<='z')) bRet=TRUE;
	}
	if(!lpCntxtPriv->bWebMode)
	{//������ַģʽת��
		if(!bCompChar)
		{
			if(KeyIn_IsWebHeader(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen))
			{
				lpCntxtPriv->bWebMode=TRUE;
				bRet=TRUE;
			}else if(byInput!=VK_BACK && byInput!=VK_ESCAPE && byInput!=VK_RETURN && byInput!=g_cHotKeyUD)
			{
				lpCntxtPriv->cCompLen--;
			}
		}else
		{
			lpCntxtPriv->bWebMode=KeyIn_IsWebHeader(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen);
		}
	}

	if(bRet)
	{
		LPBYTE lpbyCand=NULL;
		BYTE byType=0;
		BYTE byMask=MQC_NORMAL|g_SettingsG.byForecast;
		if(g_SettingsG.bAutoMatch) byMask|=MQC_MATCH;
		if(g_SettingsG.bBlendUD) byMask|=MQC_USERDEF;
		if(g_SettingsG.bBlendSpWord) byMask|=MQC_SPCAND;
		if(g_SettingsG.bAutoPrompt) byMask|=MQC_AUTOPROMPT;
		if(g_SettingsG.bOnlySimpleCode) byMask|=MQC_ONLYSC;
	
		ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);
		
		if(lpCntxtPriv->cCompLen==0)
		{
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_ENDCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND|IMN_PRIV_SETDELAY);
		}else
		{
			lpCntxtPriv->sbState=SBST_NORMAL;	//�˳�����״̬
			Helper_Trace("\nISComm_QueryCand hWnd=0x%08X",lpCntxtPriv->hUiWnd);
			if(ISComm_QueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen,byMask,lpCntxtPriv->hUiWnd)==ISACK_SUCCESS)
			{
				PMSGDATA pMsgData=ISComm_GetData();
				LPBYTE pbyData,pCandData;
				short i,sCount,sSingleWords=0;
				memcpy(g_pbyData,pMsgData->byData,pMsgData->sSize);
				pbyData=g_pbyData+1;
				memcpy(&sCount,pbyData,2);
				pbyData+=2;
				lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*sCount);
				lpCntxtPriv->sCandCount=0;
				pCandData=pbyData;
				//���ҳ���������
				for(i=0;i<sCount;i++)
				{
					if(pCandData[1]==2) sSingleWords++;
					pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ					
				}
				pCandData=pbyData;
				for(i=0;i<sCount;i++)
				{
					if(g_SettingsG.nGbkMode!=0 || pCandData[0]!=RATE_GBK || sSingleWords<2)
					{//GBK��ʾ���߲���GBK����
						lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCandCount++]=pCandData;
					}
					pCandData+=pCandData[1]+2;	//ƫ�ƴ�����Ϣ
					pCandData+=pCandData[0]+1;	//ƫ�Ʊ�����Ϣ
				}
			}
		}
		if(lpCntxtPriv->sCandCount)
		{
			byType=g_pbyData[0];
			lpbyCand=lpCntxtPriv->ppbyCandInfo[0];
		}else if(lpCntxtPriv->bWebMode)
		{
			g_bTipShow=TRUE;
			strcpy(g_szTip,"��ַģʽ:\n �س�=��ַ����\n Shift+�س�=���");
		}
		if((byType&MCR_AUTOSELECT ||(KeyIn_Code_IsMaxCode(lpCntxtPriv,byType) && g_SettingsG.bAutoInput)) && !lpCntxtPriv->bWebMode)
		{
			if(lpCntxtPriv->sCandCount==1 && lpbyCand[0]!=RATE_FORECAST && (lpbyCand[0]!=RATE_GBK || g_SettingsG.nGbkMode!=1))
				KeyIn_All_SelectCand(hIMC,lpIMC,lpCntxtPriv,'1',0,lpbKeyState,lpdwTransKey,pMsgCnt);
			else
			{
				if(lpCntxtPriv->sCandCount>1)
					SoundPlay("ChongMa");
				else if(lpCntxtPriv->sCandCount==0)
					SoundPlay("KongMa");
				else
					SoundPlay("LianXiang");
			}
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
		}else
		{
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
		}
	}
	return bRet;
}

BOOL KeyIn_Code_Symbol(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BYTE byMask=0;
	LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
	char *pResult=GETLPRESULTSTR(lpCompStr);
	lpCompStr->dwResultStrLen=0;
	if(lpCntxtPriv->sCandCount && 
		lpCntxtPriv->inState==INST_CODING && 
		lpCntxtPriv->sbState==SBST_NORMAL)
	{
		LPBYTE pbyCand=lpCntxtPriv->ppbyCandInfo[lpCntxtPriv->sCurCandPage*g_SettingsG.cPageCands];
		memcpy(pResult,pbyCand+2,pbyCand[1]);
		lpCompStr->dwResultStrLen+=pbyCand[1];
	}
	lpCompStr->dwResultStrLen+=Symbol_Convert(byInput,pResult+lpCompStr->dwResultStrLen,lpbKeyState);
	if(g_SettingsL.bRecord)
		byMask|=MKI_RECORD;
	if(g_SettingsG.bTTSInput)
		byMask|=MKI_TTSINPUT;
	ISComm_KeyIn(pResult,(short)lpCompStr->dwResultStrLen,byMask,lpCntxtPriv->hUiWnd);
	g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
	ImmUnlockIMCC(lpIMC->hCompStr);

	ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);

	MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
	MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
	MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
	MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);

	return TRUE;
}

BOOL KeyIn_Code_Normal(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=KeyIn_Code_ChangeComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	if(!bRet) bRet=KeyIn_Code_Symbol(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	return bRet;
}

BOOL KeyIn_RepeatInput(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
	if(lpCntxtPriv->nInputLen!=0)
	{
		lpCompStr->dwResultStrLen=lpCntxtPriv->nInputLen;
		strncpy(GETLPRESULTSTR(lpCompStr),lpCntxtPriv->szInput,lpCntxtPriv->nInputLen);
		g_dwInputCount+=lpCompStr->dwResultStrLen/2;	//�ۼ����뺺������
		if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		bRet=TRUE;
	}			
	ImmUnlockIMCC(lpIMC->hCompStr);
	return bRet;
}

//����״̬�µ����봦��,����״̬�ı䴦��
BOOL KeyIn_All_Associate(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	_ASSERT(lpCntxtPriv->sbState==SBST_ASSOCIATE);
	if((g_CompMode==IM_SHAPECODE && byInput==g_cHotKeySent) ||(g_CompMode==IM_SPELL && byInput==';'))
	{
		if(lpCntxtPriv->sSentLen)
		{//�л����������״̬
			lpCntxtPriv->sbState=SBST_SENTENCE;
			lpCntxtPriv->sSentCaret=0;
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_STARTCOMPOSITION,0,0);
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_INVALIDINPUTWND|IMN_PRIV_KILLDELAY);
			bRet=TRUE;
		}
	}

	if(!bRet)
	{
		if(g_CompMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
		else
			bRet=KeyIn_Code_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	}
	return bRet;
}

//��������
BOOL KeyIn_Digital_ChangeComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	if((byInput>='0' && byInput<='9') || ((byInput=='.' ||byInput==',') && g_SettingsG.bAutoDot) )
	{
		LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
		char *pResult=GETLPRESULTSTR(lpCompStr);
		pResult[0]=byInput;
		lpCompStr->dwResultStrLen=1;
		ImmUnlockIMCC(lpIMC->hCompStr);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_STARTCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		ISComm_KeyIn(&byInput,1,g_SettingsL.bRecord?MKI_RECORD:0,lpCntxtPriv->hUiWnd);
		bRet=TRUE;
	}else
	{
		lpCntxtPriv->inState=INST_CODING;
		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		if(g_CompMode==IM_SPELL)
			bRet=KeyIn_Spell_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
		else
			bRet=KeyIn_Code_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,pMsgCnt);
	}
	return bRet;
}

//�ʻ�����״̬
BOOL KeyIn_Line_ChangeComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	BOOL bRet=FALSE;
	BOOL bCompChar=FALSE;
	if(byInput==VK_BACK)
	{//����
		if(lpCntxtPriv->cCompLen>0)
		{
			lpCntxtPriv->cCompLen--;
			bCompChar=TRUE;
		}
		bRet=TRUE;
	}else if(byInput==VK_ESCAPE||byInput==VK_RETURN)
	{//�˳�״̬
		lpCntxtPriv->inState=INST_CODING;
		lpCntxtPriv->sbState=SBST_NORMAL;
		lpCntxtPriv->cCompLen=0;
		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);
		bRet=TRUE;
	}else if(byInput>=VK_NUMPAD1 && byInput<=VK_NUMPAD6)
	{//�༭
		lpCntxtPriv->szComp[lpCntxtPriv->cCompLen++]=byInput-VK_NUMPAD1+'1';
		lpCntxtPriv->szComp[lpCntxtPriv->cCompLen]=0;
		bCompChar=TRUE;
		bRet=TRUE;
	}
	if(bCompChar)
	{
		ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);
		if(lpCntxtPriv->cCompLen && ISComm_LineQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen)==ISACK_SUCCESS)
		{//��ѯ����
			PMSGDATA pMsgData=ISComm_GetData();
			short i,sCount=0;
			LPBYTE pBuf=g_pbyData;
			memcpy(g_pbyData,pMsgData->byData,pMsgData->sSize);
			memcpy(&sCount,pBuf,2);
			pBuf+=2;
			lpCntxtPriv->ppbyCandInfo=(LPBYTE*)malloc(sCount*sizeof(LPBYTE));
			for(i=0;i<sCount;i++)
			{
				lpCntxtPriv->ppbyCandInfo[i]=pBuf;
				pBuf+=pBuf[1]+2;
			}
			lpCntxtPriv->sCandCount=sCount;
		}
		MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
	}
	return bRet;
}


//�û��Զ�������
BOOL KeyIn_UserDef_ChangeComp(HIMC hIMC,LPINPUTCONTEXT lpIMC,LPCONTEXTPRIV lpCntxtPriv,BYTE byInput,
					  CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT *pMsgCnt)
{
	if(byInput==VK_BACK)
	{
		if(lpCntxtPriv->cCompLen>0)	lpCntxtPriv->cCompLen--;
	}else if(byInput==VK_ESCAPE)
	{
		ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
		Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
		MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);
	}else if(byInput==VK_RETURN)
	{
		if(lpCntxtPriv->cCompLen)
		{//�������
			LPCOMPOSITIONSTRING lpCompStr=(LPCOMPOSITIONSTRING)ImmLockIMCC(lpIMC->hCompStr);
			memcpy(GETLPRESULTSTR(lpCompStr),lpCntxtPriv->szComp,lpCntxtPriv->cCompLen);
			lpCompStr->dwResultStrLen=lpCntxtPriv->cCompLen;
			if(g_SettingsL.bInputBig5) GB2GIB5(GETLPRESULTSTR(lpCompStr),lpCompStr->dwResultStrLen);
			ImmUnlockIMCC(lpIMC->hCompStr);
			//֪ͨӦ�ó����������
			MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_COMPOSITION,0,GCS_INPUT);
			MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_ENDCOMPOSITION, 0, 0);
			MyGenerateMessageToTransKey(lpdwTransKey, pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE, IMN_PRIV_CLOSEINPUTWND);
			//�л�������״̬
			ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
			Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
			return TRUE;
		}
	}else
	{
		lpCntxtPriv->szComp[lpCntxtPriv->cCompLen++]=byInput;
		lpCntxtPriv->szComp[lpCntxtPriv->cCompLen]=0;
	}
	ClearPrivateContextEx(lpCntxtPriv,CPC_CAND);
	if(lpCntxtPriv->cCompLen && ISACK_SUCCESS==ISComm_UDQueryCand(lpCntxtPriv->szComp,lpCntxtPriv->cCompLen))
	{//��ȡ�Զ������,֧�ֶ������
		PMSGDATA pMsgData=ISComm_GetData();
		LPBYTE pbyData;
		short i;
		memcpy(g_pbyData,pMsgData->byData,pMsgData->sSize);
		pbyData=g_pbyData;
		pbyData+=pbyData[0]+1;//������������������Զ������ı�������
		memcpy(&lpCntxtPriv->sCandCount,pbyData,2);
		pbyData+=2;
		lpCntxtPriv->ppbyCandInfo=(LPBYTE *)malloc(sizeof(LPBYTE)*lpCntxtPriv->sCandCount);
		for(i=0;i<lpCntxtPriv->sCandCount;i++)
		{
			lpCntxtPriv->ppbyCandInfo[i]=pbyData;
			pbyData+=pbyData[1]+2;//����
			pbyData+=pbyData[0]+1;//����
		}
	}
	MyGenerateMessageToTransKey(lpdwTransKey,pMsgCnt,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_UPDATEINPUTWND);
	return TRUE;
}

/****************************************************************************************************************/
/* function:Ӧ�ó����������ӿ����������������ĵ�ת�������뷨����������ӿ���ת���û�������					*/
/* UINT uVKey:��ֵ,�����ImeInquire�ӿ���ΪfdwProperty����������IME_PROP_KBD_CHAR_FIRST,����ֽ��������ֵ		*/
/* UINT uScanCode:������ɨ���룬��ʱ��������ͬ���ļ�ֵ����ʱ��Ҫʹ��uScanCode������								*/
/* CONST LPBYTE lpbKeyState:����״̬,����256����״̬															*/
/* LPDWORD lpdwTransKey:��Ϣ����������������IMEҪ����Ӧ�ó������Ϣ����һ��˫���ǻ������������ɵ������Ϣ����	*/
/* UINT fuState:Active menu flag(come from msdn)																*/
/* HIMC hIMC:����������																							*/
/* return : ���ر�������Ϣ������lpdwTransKey�е���Ϣ����														*/
/****************************************************************************************************************/
UINT WINAPI ImeToAsciiEx (UINT uVKey,UINT uScanCode,CONST LPBYTE lpbKeyState,LPDWORD lpdwTransKey,UINT fuState,HIMC hIMC)
{
	LPINPUTCONTEXT lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC);
	UINT uMsgCount=0;
	if(lpIMC)
	{
		LPCONTEXTPRIV lpCntxtPriv=(LPCONTEXTPRIV)ImmLockIMCC(lpIMC->hPrivate);
		BYTE byInput=LOBYTE(uVKey);//��ȡVK
		BOOL bHandle=FALSE;
		//�û��ʵ��ݼ�
		if(lpbKeyState[VK_CONTROL] & 0x80 && lpbKeyState[VK_SHIFT] & 0x80)
		{
			if(byInput==g_SettingsG.byHotKeyQuery)
			{
				UserDict_Input(lpIMC->hWnd);
				bHandle=TRUE;
			}else
			{
				bHandle=Plugin_HotkeyProcess(uVKey,uScanCode);
			}
		}
		//����ʹ��VK�����ݼ������뷭ҳ��
		if(!bHandle && lpbKeyState[VK_CONTROL] & 0x80 )
		{//�����ݼ�
			bHandle=TRUE;
			if(byInput==g_SettingsG.byHotKeyMakeWord)
			{
				MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,IDM_MAKEPHRASE));
			}else if(byInput==g_SettingsG.byHotKeyQuery)
			{
				MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,IDM_QUERYCOMP));
			}else if(byInput==g_SettingsG.byHotKeyMode)
			{
				if(!g_bTempSpell) MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,IDM_IMCHANGE));
			}else if(byInput==g_SettingsG.byHotKeyShowRoot)
			{
				MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,IDM_KEYMAP));
			}else if(byInput==g_SettingsG.byHotKeyEn)
			{
				MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,g_SettingsL.bEnglish?IDC_ENGLISH1:IDC_ENGLISH2));
			}else if(byInput==g_SettingsG.byHotKeyHideStatus)
			{
				MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,MAKELONG(IMN_PRIV_COMMAND,IDM_HIDESTATUS));
			}else
			{
				bHandle=FALSE;
			}
		}
		if(!bHandle && lpCntxtPriv && lpCntxtPriv->sCandCount && lpCntxtPriv->sbState!=SBST_SENTENCE)
		{//��������
			BYTE byCandIndex=0;
			if(byInput==VK_SPACE) 
			{//�ո�
				byCandIndex='1';
			}else if((byInput>='0' && byInput<='9') || (byInput>=VK_NUMPAD0 && byInput<=VK_NUMPAD9))
			{//���ּ�
				if((lpbKeyState[VK_CONTROL]&0x80) ||
					(!(lpbKeyState[VK_SHIFT]&0x80) //δ����Shift�����
					&& !KeyIn_IsNumCode(lpCntxtPriv) //���ֲ��Ǳ���״̬
					&& (!g_SettingsG.bCandSelNoNum ||g_CompMode==IM_SPELL) //δ��ֹ����ѡ������
					)//��������ѡ������
					)
				{//����������ֵ�ASCII��
					if((byInput>=VK_NUMPAD0 && byInput<=VK_NUMPAD9))
						byCandIndex=byInput-VK_NUMPAD0+'0';
					else
						byCandIndex=byInput;
				}
			}else if(g_SettingsG.b23CandKey && !(lpbKeyState[VK_SHIFT]&0x80))
			{//SHIFT ģʽ�²�����
				UINT uVk=MapVirtualKey(uScanCode,1);
				if(uVk==g_SettingsG.by2CandVK && lpCntxtPriv->sCandCount>=2) byCandIndex='2';
				if(uVk==g_SettingsG.by3CandVK && lpCntxtPriv->sCandCount>=3) byCandIndex='3';
			}
			if(byCandIndex )
			{
				if(!((lpCntxtPriv->sbState==SBST_ASSOCIATE && g_SettingsG.byAstMode==AST_ENGLISH && !(lpbKeyState[VK_CONTROL]&0x80))//Ӣ������״ֻ̬�а���Ctrl�Ž������ѡ��
					|| (g_CompMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING && (byInput==VK_SPACE || byInput=='\'') ) )	//ƴ������״̬�µ���������ֶ�����  0xde=VkKeyScan('\'')
					)
				{
					bHandle=KeyIn_All_SelectCand(hIMC,lpIMC,lpCntxtPriv,byCandIndex,0,lpbKeyState,lpdwTransKey,&uMsgCount);
				}
			}
			if(!bHandle) bHandle=KeyIn_All_TurnCandPage(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
		}
		//����ƴ���������ƶ�
		if(!bHandle && g_CompMode==IM_SPELL && lpCntxtPriv->inState==INST_CODING)
		{
			bHandle=KeyIn_Spell_MoveCaret(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
			if(!bHandle && byInput==VK_DELETE) bHandle=KeyIn_Spell_SyFix(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);//����VK_DELETE
		}
		byInput=HIBYTE(uVKey);	//ȡ�ð�����ASCII��
		if(byInput==VK_RETURN && lpbKeyState[VK_SHIFT]&0x80)
		{
			bHandle=KeyIn_RepeatInput(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
		}
		if(!bHandle && lpCntxtPriv && byInput)
		{
			if(lpCntxtPriv->inState==INST_CODING)
			{//����״̬ת��ǰ����
				BOOL bReadyEn=FALSE;
				BOOL bReadyDgt=FALSE;
				if(lpCntxtPriv->sbState==SBST_NORMAL)
				{
					if(g_CompMode==IM_SPELL)
					{
						if(lpCntxtPriv->bySyllables==1 && lpCntxtPriv->spellData[0].bySpellLen==0)
						{
							bReadyEn=TRUE;
							bReadyDgt=TRUE;
						}
					}else
					{
						if(lpCntxtPriv->cCompLen==0)
						{
							bReadyEn=TRUE;
							bReadyDgt=TRUE;
						}
					}
				}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
				{
					bReadyEn=TRUE;
					bReadyDgt=TRUE;
					if((g_SettingsG.byAstMode==AST_CAND ||(g_SettingsG.byAstMode==AST_ENGLISH &&(lpbKeyState[VK_CONTROL]&0x80))) && lpCntxtPriv->sCandCount)
						bReadyDgt=FALSE;
				}
				if(g_bTempSpell && (bReadyEn || bReadyDgt) && (isdigit(byInput) || isupper(byInput)))
				{//temp spell mode
					SoundPlay("error");
					ImmUnlockIMCC(lpIMC->hPrivate);
					ImmUnlockIMC(hIMC);
					return 0;
				}
				if((bReadyEn || bReadyDgt) && g_bTipShow) //�ر�tip
					g_bTipShow=FALSE;
				if(bReadyEn && byInput>='A' && byInput<='Z')
				{//��д���룬���л���Ӣ��״̬
					ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
					if(g_SettingsL.bEnglish)
					{
						lpCntxtPriv->inState=INST_ENGLISH;
						byInput+=0x20;//�Զ��Ļ�Сд
						Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
						//ȷ�������봰��
						MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_STARTCOMPOSITION,0,0);
						MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_OPENINPUTWND);
					}
				}else if(bReadyDgt && byInput>='0' && byInput<='9')
				{//�������룬������������״̬
					ClearPrivateContextEx(lpCntxtPriv,CPC_ALL);
					lpCntxtPriv->inState=INST_DIGITAL;
					Plugin_StateChange(g_CompMode,lpCntxtPriv->inState,lpCntxtPriv->sbState,g_bTempSpell);
					MyGenerateMessageToTransKey(lpdwTransKey,&uMsgCount,WM_IME_NOTIFY,IMN_PRIVATE,IMN_PRIV_CLOSEINPUTWND);
				}
			}
			if(lpCntxtPriv->inState==INST_CODING)
			{
				if(lpCntxtPriv->sbState==SBST_NORMAL)
				{//��������
					if(g_CompMode==IM_SPELL)
					{//ƴ��״̬
						if(lpCntxtPriv->bPYBiHua)
						{//�ʻ��������룬��֧�����ܼ�
							BOOL bInputKey=FALSE;
							if(LOBYTE(uVKey)>=VK_NUMPAD1&&LOBYTE(uVKey)<=VK_NUMPAD5)
							{//С��������
								byInput=LOBYTE(uVKey);
								bInputKey=TRUE;
							}else
							{//���õ�ת����
								int i;
								for( i=0;i<5;i++)
								{
									if(g_SettingsG.byLineKey[i]==byInput)
									{
										byInput=VK_NUMPAD1+i;
										bInputKey=TRUE;
										break;
									}
								}
								if(byInput==VK_ESCAPE || byInput==VK_BACK) bInputKey=TRUE;
							}
							bHandle=KeyIn_PYBiHua_ChangComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
							if(!bHandle) bHandle=KeyIn_Spell_InputText(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
						}else
						{
							bHandle=KeyIn_Spell_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
						}
					}else
					{
						bHandle=KeyIn_Code_Normal(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
					}
				}else if(lpCntxtPriv->sbState==SBST_ASSOCIATE)
				{//����״̬
					bHandle=KeyIn_All_Associate(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
				}else if(lpCntxtPriv->sbState==SBST_SENTENCE)
				{//����״̬
					bHandle=KeyIn_All_Sentence(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
					if(!bHandle) KeyIn_Code_Symbol(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
				}
			}else if(lpCntxtPriv->inState==INST_ENGLISH)
			{//Ӣ�ĵ���״̬
				bHandle=KeyIn_Code_English(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
			}else if(lpCntxtPriv->inState==INST_DIGITAL)
			{//��������״̬
				bHandle=KeyIn_Digital_ChangeComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
			}else if(lpCntxtPriv->inState==INST_USERDEF)
			{//�û��Զ�������״̬
				bHandle=KeyIn_UserDef_ChangeComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
			}else if(lpCntxtPriv->inState==INST_LINEIME)
			{//�ʻ�����״̬
				BOOL bInputKey=FALSE;
				if(LOBYTE(uVKey)>=VK_NUMPAD1&&LOBYTE(uVKey)<=VK_NUMPAD6)
				{//С��������
					byInput=LOBYTE(uVKey);
					bInputKey=TRUE;
				}else
				{//���õ�ת����
					int i;
					for( i=0;i<6;i++)
					{
						if(g_SettingsG.byLineKey[i]==byInput)
						{
							byInput=VK_NUMPAD1+i;
							bInputKey=TRUE;
							break;
						}
					}
					if(byInput==VK_ESCAPE || byInput==VK_RETURN || byInput==VK_BACK) bInputKey=TRUE;
				}
				if(bInputKey) bHandle=KeyIn_Line_ChangeComp(hIMC,lpIMC,lpCntxtPriv,byInput,lpbKeyState,lpdwTransKey,&uMsgCount);
			}
			if(!bHandle) SoundPlay("error");
		}
		ImmUnlockIMCC(lpIMC->hPrivate);
		ImmUnlockIMC(hIMC);
	}
    return (uMsgCount);
}

