#include "optip.h"

static char	**	g_ppszOpTipSpell=NULL;
static char	**	g_ppszOpTipShape=NULL;
static char **  g_ppszOpTipAll=NULL;
static int		g_nOpTipSpell=0;
static int		g_nOpTipShape=0;
static int		g_nOpTipAll=0;
static BOOL		g_bOpLoaded=FALSE;

BOOL Tips_IsLoaded(){return g_bOpLoaded;}
void Tips_Load()
{
	char szPath[MAX_PATH],szBuf[200],szKey[20];
	int i;
	sprintf(szPath,"%s\\tips.txt",g_szDataPath);
	g_nOpTipAll=GetPrivateProfileInt("ALL","count",0,szPath);
	g_ppszOpTipAll=(char**)malloc(sizeof(char*)*g_nOpTipAll);
	for(i=0;i<g_nOpTipAll;i++)
	{
		sprintf(szKey,"tip%d",i+1);
		GetPrivateProfileString("ALL",szKey,"",szBuf,200,szPath);
		g_ppszOpTipAll[i]=(char*)malloc(strlen(szBuf)+1);
		strcpy(g_ppszOpTipAll[i],szBuf);
	}
	
	g_nOpTipSpell=GetPrivateProfileInt("SPELL","count",0,szPath);
	g_ppszOpTipSpell=(char**)malloc(sizeof(char*)*g_nOpTipSpell);
	for(i=0;i<g_nOpTipSpell;i++)
	{
		sprintf(szKey,"tip%d",i+1);
		GetPrivateProfileString("SPELL",szKey,"",szBuf,200,szPath);
		g_ppszOpTipSpell[i]=(char*)malloc(strlen(szBuf)+1);
		strcpy(g_ppszOpTipSpell[i],szBuf);
	}
	g_nOpTipShape=GetPrivateProfileInt("SHAPE","count",0,szPath);
	g_ppszOpTipShape=(char**)malloc(sizeof(char*)*g_nOpTipShape);
	for(i=0;i<g_nOpTipShape;i++)
	{
		sprintf(szKey,"tip%d",i+1);
		GetPrivateProfileString("SHAPE",szKey,"",szBuf,200,szPath);
		g_ppszOpTipShape[i]=(char*)malloc(strlen(szBuf)+1);
		strcpy(g_ppszOpTipShape[i],szBuf);
	}
	g_bOpLoaded=TRUE;
}

void Tips_Free()
{
	int i;
	if(!g_bOpLoaded) return;
	if(g_nOpTipAll)
	{
		for(i=0;i<g_nOpTipAll;i++)
		{
			free(g_ppszOpTipAll[i]);
		}
		free(g_ppszOpTipAll);
		g_ppszOpTipAll=NULL;
		g_nOpTipAll=0;
	}
	if(g_nOpTipSpell)
	{
		for(i=0;i<g_nOpTipSpell;i++)
		{
			free(g_ppszOpTipSpell[i]);
		}
		free(g_ppszOpTipSpell);
		g_ppszOpTipSpell=NULL;
		g_nOpTipSpell=0;
	}
	if(g_nOpTipShape)
	{
		for(i=0;i<g_nOpTipShape;i++)
		{
			free(g_ppszOpTipShape[i]);
		}
		free(g_ppszOpTipShape);
		g_ppszOpTipShape=NULL;
		g_nOpTipShape=0;
	}
	g_bOpLoaded=FALSE;
}

BOOL Tips_Rand(BOOL bSpell,char *pszBuf)
{
	char *pszOpTip=NULL;
	int nIndex;
	if(g_bOpLoaded)
	{
		if(bSpell)
		{
			if(g_nOpTipSpell||g_nOpTipAll)
			{
				nIndex=rand()%(g_nOpTipSpell+g_nOpTipAll);
				if(nIndex<g_nOpTipAll)
					pszOpTip=g_ppszOpTipAll[nIndex];
				else
					pszOpTip=g_ppszOpTipSpell[nIndex-g_nOpTipAll];
			}
		}else
		{
			if(g_nOpTipShape||g_nOpTipAll)
			{
				nIndex=rand()%(g_nOpTipShape+g_nOpTipAll);
				if(nIndex<g_nOpTipAll)
					pszOpTip=g_ppszOpTipAll[nIndex];
				else
					pszOpTip=g_ppszOpTipShape[nIndex-g_nOpTipAll];
			}
		}
	}
	if(pszOpTip) strcpy(pszBuf,pszOpTip);
	return pszOpTip!=NULL;
}
