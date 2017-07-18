#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <crtdbg.h>

#include "smart.h"
#include "..\\include\\iscomm.h"

SMARTINFO g_SmartInfo={0};

void	Smart_Free()
{
	if(g_SmartInfo.cEntrys)
	{
		char i;
		free(g_SmartInfo.pcSmartHead);
		g_SmartInfo.pcSmartHead=NULL;
		free(g_SmartInfo.pcCands);
		g_SmartInfo.pcCands=NULL;
		for(i=0;i<g_SmartInfo.cEntrys;i++) if(g_SmartInfo.ppSmartLink[i]) free(g_SmartInfo.ppSmartLink[i]);
		free(g_SmartInfo.ppSmartLink);
		g_SmartInfo.ppSmartLink=NULL;
		g_SmartInfo.cEntrys=0;
	}
}

void	Smart_Load(LPCTSTR pszFileName)
{
	Smart_Free();
	g_SmartInfo.cEntrys=GetPrivateProfileInt("SMART","ENTRYS",0,pszFileName);
	if(g_SmartInfo.cEntrys)
	{
		char i,szSection[2]={0},szBuf[500]={0};
		g_SmartInfo.pcSmartHead=(char*)malloc(g_SmartInfo.cEntrys+1);
		g_SmartInfo.pcCands=(char*)calloc(g_SmartInfo.cEntrys,sizeof(char));
		g_SmartInfo.ppSmartLink=(SMARTLINK**)calloc(g_SmartInfo.cEntrys,sizeof(SMARTLINK*));
		GetPrivateProfileString("SMART","HEADER","",g_SmartInfo.pcSmartHead,g_SmartInfo.cEntrys+1,pszFileName);
		for(i=0;i<g_SmartInfo.cEntrys;i++)
		{
			char cSize=0, *p=szBuf;
			szSection[0]=g_SmartInfo.pcSmartHead[i];
			GetPrivateProfileSection(szSection,szBuf,500,pszFileName);
			while(*p)
			{//统计表项
				p+=strlen(p)+1;
				cSize++;
			}
			if(cSize)
			{
				char j=0;
				p=szBuf;
				g_SmartInfo.ppSmartLink[i]=(SMARTLINK*)calloc(cSize,sizeof(SMARTLINK));
				while(*p)
				{
					if(p[0]>='a' && p[0]<='z' && p[1]=='=')
					{
						g_SmartInfo.ppSmartLink[i][j].cComp=p[0];
						memcpy(g_SmartInfo.ppSmartLink[i][j].szCand,p+2,2);
						j++;
					}
					p+=strlen(p)+1;
				}
				g_SmartInfo.pcCands[i]=j;
			}
		}
	}
}

BOOL Smart_Test(char cHeader)
{
	char i=0;
	while(i<g_SmartInfo.cEntrys)
	{
		if(g_SmartInfo.pcSmartHead[i]==cHeader) return TRUE;
		i++;
	}
	return FALSE;
}

DWORD Smart_QueryCand(char *pszComp,char cLen)
{
	short sCount=0;
	char i=0;
	PMSGDATA pData=ISComm_GetData();
	COMFILE cf;
	CF_Attach(&cf,pData,CFM_WRITE);
	CF_WriteChar(&cf,MCR_ASSOCIATE);//type
	CF_Write(&cf,&sCount,2);
	while(i<g_SmartInfo.cEntrys)
	{
		if(g_SmartInfo.pcSmartHead[i]==pszComp[0])
		{
			if(cLen==1)
			{
				char j=0;
				while(j<g_SmartInfo.pcCands[i])
				{
					CF_WriteChar(&cf,0);//词频
					CF_WriteChar(&cf,2);//短语长度
					CF_Write(&cf,g_SmartInfo.ppSmartLink[i][j].szCand,2);//短语
					CF_WriteChar(&cf,2);//编码长度
					CF_WriteChar(&cf,g_SmartInfo.pcSmartHead[i]);
					CF_WriteChar(&cf,g_SmartInfo.ppSmartLink[i][j].cComp);
					j++;
				}
				sCount=g_SmartInfo.pcCands[i];
			}else if(cLen==2)
			{
				char j=0;
				while(j<g_SmartInfo.pcCands[i])
				{
					if(g_SmartInfo.ppSmartLink[i][j].cComp==pszComp[1])
					{
						CF_WriteChar(&cf,0);//词频
						CF_WriteChar(&cf,2);//短语长度
						CF_Write(&cf,g_SmartInfo.ppSmartLink[i][j].szCand,2);//短语
						CF_WriteChar(&cf,2);//编码长度
						CF_WriteChar(&cf,g_SmartInfo.pcSmartHead[i]);
						CF_WriteChar(&cf,g_SmartInfo.ppSmartLink[i][j].cComp);
						sCount=1;
						break;
					}
					j++;
				}
			}
			break;
		}
		i++;
	}
	if(sCount)
	{
		CF_Seek(&cf,1,SEEK_SET);
		CF_Write(&cf,&sCount,2);
	}
	CF_Detach(&cf);
	if(sCount)
		return ISACK_SUCCESS;
	else
		return ISACK_ERROR;
}