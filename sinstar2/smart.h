#ifndef _SINSMART_
#define _SINSMART_

typedef struct tagSMARTLINK
{
	char cComp;
	char szCand[2];
}SMARTLINK;

typedef struct tagSMARTINFO
{
	char cEntrys;
	char *pcSmartHead;
	char *pcCands;
	SMARTLINK ** ppSmartLink;
}SMARTINFO;

extern SMARTINFO g_SmartInfo;

void	Smart_Free();
void	Smart_Load(LPCTSTR pszFileName);
DWORD Smart_QueryCand(char *pszComp,char cLen);
BOOL Smart_Test(char cHeader);

#endif//_SINSMART_