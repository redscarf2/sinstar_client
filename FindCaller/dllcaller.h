#ifndef _DLLCALLER_
#define _DLLCALLER_


#ifdef __cplusplus
extern "C"{
#endif//__cplusplus

typedef struct tagCALLERINFO
{
	DWORD dwProcID;
	char szFile[MAX_PATH];
	char szPath[MAX_PATH];
}CALLERINFO,*PCALLERINFO;

PCALLERINFO GetCaller(LPCTSTR pszDllPath);
void FreeCallerInfo(PCALLERINFO pci);
BOOL Kill_Process(DWORD dwProcID);
HWND GetProcessWnd(DWORD dwProcID);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif//_DLLCALLER_