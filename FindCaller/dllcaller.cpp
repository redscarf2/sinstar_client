// dllcaller.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "dllcaller.h"
#include "sarray.hpp"
#include <tlhelp32.h>
#include <stdlib.h> 


BOOL ProcessModuleDetect(DWORD dwProcID,LPCTSTR pszModuleName,LPCTSTR pszProcFile,LPTSTR pszProcPath)
{
	BOOL bRet=FALSE;
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcID);

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return (FALSE);
	}

	me32.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First (hModuleSnap, &me32))
	{
		return (FALSE);
	}

	do
	{
		if(0==lstrcmpi(pszProcFile,me32.szModule)) strcpy(pszProcPath,me32.szExePath);
		if (0==lstrcmpi(pszModuleName, me32.szExePath)) 
			bRet=TRUE;
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	return bRet;
}

 PCALLERINFO GetCaller(LPCTSTR pszDllPath)
{
	CSArray<CALLERINFO> arrCaller;
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	CALLERINFO ci;
	char szProcPath[MAX_PATH];
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return (NULL);
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return (NULL);
	}

	do
	{
		szProcPath[0]=0;
		if(ProcessModuleDetect(pe32.th32ProcessID,pszDllPath,pe32.szExeFile,szProcPath) && pe32.th32ProcessID!=0)
		{
			strcpy(ci.szFile,pe32.szExeFile);
			strcpy(ci.szPath,szProcPath);
			ci.dwProcID=pe32.th32ProcessID;
			arrCaller.Add(ci);
		}
	} while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	int i,nProcessCount=arrCaller.GetSize();
	if(nProcessCount==0) return NULL;
	PCALLERINFO pRet=(PCALLERINFO)malloc(sizeof(CALLERINFO)*(nProcessCount+1));
	for(i=0;i<arrCaller.GetSize();i++)
	{
		memcpy(pRet+i,arrCaller.GetPtAt(i),sizeof(CALLERINFO));
	}
	pRet[i].dwProcID=0;
	pRet[i].szPath[0]=0;
	pRet[i].szFile[0]=0;
	return pRet;
}

 void FreeCallerInfo(PCALLERINFO pci)
{
	if(pci)	free(pci);
}

 BOOL Kill_Process(DWORD dwProcID)
{
	BOOL bRet=FALSE;
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcID);
	if(hProcess)
	{
		bRet=TerminateProcess(hProcess,1);
		CloseHandle(hProcess);
	}
	return bRet;
}

typedef struct tagEWPARAM
{
	HWND hWnd;
	DWORD dwProcID;
}EWPARAM;

BOOL CALLBACK MyEnumWindowsProc(
  HWND hWnd,      // handle to parent window
  LPARAM lParam   // application-defined value
)
{
	DWORD dwProcID=0;
	EWPARAM *p=(EWPARAM*)lParam;
	GetWindowThreadProcessId(hWnd,&dwProcID);
	if(dwProcID==p->dwProcID && !GetParent(hWnd) && !(GetWindowLong(hWnd,GWL_EXSTYLE)&WS_EX_TOOLWINDOW) && !(GetWindowLong(hWnd,GWL_STYLE)&WS_DISABLED))
	{
		p->hWnd=hWnd;
		return FALSE;
	}
	return TRUE;
}

 HWND GetProcessWnd(DWORD dwProcID)
{
	EWPARAM ewp={0,dwProcID};
	EnumWindows(MyEnumWindowsProc,(LPARAM)&ewp);
	return ewp.hWnd;
}
