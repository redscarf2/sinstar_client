// register.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include "register.h"
#include <stdio.h>
#include <imm.h>
#include <TCHAR.h>
#include <ShellAPI.h>
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"imm32.lib")
#include <winver.h>
#pragma comment(lib,"version.lib")

#define MAX_STRLEN		100

#define NAME_MAPFILE _T("_setoutsoft comm map file")

TCHAR g_szPath[MAX_PATH]={0};	//程序启动位置

//定义一个结构，以便初始化对话框
typedef struct tagDLGPARAM
{
	HINSTANCE hInst;	//程序实例
	TCHAR szIme[100];	//输入法文件名
	TCHAR szName[100];	//安装到注册表中的名称
}DLGPARAM;


//获取一个PE文件的version + ProductName +  FileDescription
BOOL Helper_PEVersion(LPCTSTR pszFileName,DWORD *pdwVer,TCHAR *pszName ,TCHAR *pszDesc)
{
	DWORD dwResHandle;
	LPTSTR lszVer = NULL; 
	UINT  cchVer = 0;  
	void *pBuf;
	BOOL bRet=FALSE;
	DWORD dwVerInfoSize=GetFileVersionInfoSize((TCHAR*)pszFileName,&dwResHandle);
	if(!dwVerInfoSize) return FALSE;
	pBuf=malloc(dwVerInfoSize);
	GetFileVersionInfo((TCHAR*)pszFileName, dwResHandle, dwVerInfoSize, pBuf); 
	if(pdwVer)
	{
		UINT nVersionLen;
		VS_FIXEDFILEINFO *pstFileVersion;
		if(VerQueryValue(pBuf, _T("\\"), (void**)&pstFileVersion, &nVersionLen) && nVersionLen>=sizeof(VS_FIXEDFILEINFO) )
		{
			*pdwVer=MAKELONG(MAKEWORD(LOWORD(pstFileVersion->dwFileVersionLS),HIWORD(pstFileVersion->dwFileVersionLS)),
				MAKEWORD(LOWORD(pstFileVersion->dwFileVersionMS),HIWORD(pstFileVersion->dwFileVersionMS)));
		}
	}

	bRet = VerQueryValue(pBuf, _T("\\VarFileInfo\\Translation"), (void**)&lszVer, &cchVer); 
	if(bRet&&cchVer==4)
	{
		TCHAR szLangCode[20]={0};
		TCHAR szSection[256]; 
		BOOL bOK;
		_stprintf(szLangCode,_T("%04x"),*(WORD*)lszVer);
		_stprintf(szLangCode+4,_T("%04x"),*(WORD*)(lszVer+2));
		if(pszName)
		{
			_stprintf(szSection,_T("\\StringFileInfo\\%s\\ProductName"),szLangCode);
			bOK= VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) _tcscpy(pszName,lszVer);
		}
		if(pszDesc)
		{
			_stprintf(szSection,_T("\\StringFileInfo\\%s\\FileDescription"),szLangCode);
			bOK = VerQueryValue(pBuf, szSection, (void**)&lszVer, &cchVer); 
			if(bOK) _tcscpy(pszDesc,lszVer);
		}
	}
	free(pBuf);
	return bRet;
}
//获取文件的版本号
void Helper_VersionString(DWORD dwVer,TCHAR *pszBuf)
{
	_stprintf(pszBuf,_T("%u.%u.%u.%u"),(dwVer>>24)&0xFF,(dwVer>>16)&0xFF,(dwVer>>8)&0xFF,dwVer&0xFF);
}

#include <AclAPI.h>
BOOL Helper_SetFileACL(LPCTSTR pszPath)
{
	HANDLE hDir = CreateFile(pszPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
	if(hDir == INVALID_HANDLE_VALUE)
		return FALSE; 

	ACL* pOldDACL;
	SECURITY_DESCRIPTOR* pSD = NULL;
	GetSecurityInfo(hDir, SE_FILE_OBJECT , DACL_SECURITY_INFORMATION,NULL, NULL, &pOldDACL, NULL, (void**)&pSD);

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_USERS,0,0,0,0,0,0,&pSid);

	EXPLICIT_ACCESS ea={0};
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1,&ea,pOldDACL,&pNewDACL);

	if(pNewDACL)
		SetSecurityInfo(hDir,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);

	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);
	LocalFree(pOldDACL);
	CloseHandle(hDir);

	return TRUE;

}


BOOL Helper_SetFileACLEx(LPCTSTR pszPath,BOOL bSubFile)
{
	SECURITY_DESCRIPTOR* pSD = NULL;

	PSID pSid = NULL;
	SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid(&authNt,2,SECURITY_BUILTIN_DOMAIN_RID,DOMAIN_ALIAS_RID_USERS,0,0,0,0,0,0,&pSid);

	EXPLICIT_ACCESS ea={0};
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.ptstrName = (LPTSTR)pSid;

	ACL* pNewDACL = 0;
	DWORD err = SetEntriesInAcl(1,&ea,NULL,&pNewDACL);

	if(pNewDACL)
	{
		HANDLE hFile = CreateFile(pszPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
		if(hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
		CloseHandle(hFile);
		if(bSubFile)
		{
			WIN32_FIND_DATA wfd={0};
			TCHAR szPath[MAX_PATH],szFilter[MAX_PATH];
			_stprintf(szFilter,_T("%s\\*.*"),pszPath);
			HANDLE hFind=FindFirstFile(szFilter,&wfd);
			if(hFind)
			{
				do 
				{
					if(!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					{
						_stprintf(szPath,_T("%s\\%s"),pszPath,wfd.cFileName);
						HANDLE hFile = CreateFile(szPath,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
						if(hFile != INVALID_HANDLE_VALUE) SetSecurityInfo(hFile,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL, NULL, pNewDACL, NULL);
						CloseHandle(hFile);
					}
				} while (FindNextFile(hFind,&wfd));
				FindClose(hFind);
			}
		}
	}
	FreeSid(pSid);
	LocalFree(pNewDACL);
	LocalFree(pSD);

	return TRUE;

}


//找到输入法程序文件
BOOL FindImeFile(TCHAR *pszImeFile)
{
	WIN32_FIND_DATA fd={0};
	HANDLE hFind=0;
	TCHAR szPath[MAX_PATH];
	_stprintf(szPath,_T("%s\\*.ime"),g_szPath);
	hFind=FindFirstFile(szPath,&fd);
	if(INVALID_HANDLE_VALUE!=hFind)
	{
		FindNextFile(hFind,&fd);
		_tcscpy(pszImeFile,fd.cFileName);
		pszImeFile[_tcslen(pszImeFile)-4]=0;//去掉扩展名
		FindClose(hFind);
	}
	if(INVALID_HANDLE_VALUE==hFind) return FALSE;
	_stprintf(szPath,_T("%s\\x64\\*.ime"),g_szPath);
	hFind=FindFirstFile(szPath,&fd);
	if(INVALID_HANDLE_VALUE!=hFind)
	{
		FindNextFile(hFind,&fd);
		_tcscpy(pszImeFile,fd.cFileName);
		pszImeFile[_tcslen(pszImeFile)-4]=0;//去掉扩展名
		FindClose(hFind);
	}
	if(INVALID_HANDLE_VALUE==hFind) return FALSE;
	return TRUE;
}

//从输入文件中获取文件属性
BOOL QueryImeProp(TCHAR *pszIme,TCHAR *pszImeProp)
{
	TCHAR szImeFile[MAX_PATH],szName[100];
	_stprintf(szImeFile,_T("%s\\%s.ime"),g_szPath,pszIme);
	if(!Helper_PEVersion(szImeFile,NULL,NULL,szName)) return FALSE;
	_stprintf(pszImeProp,_T("中文(简体) %s"),szName);
	return TRUE;
}

void VerDot2Space(TCHAR *pszVer)
{
	while(*pszVer)
	{
		if(*pszVer == '.') *pszVer=0x20;
		pszVer++;
	}
}

BOOL CheckVersion(TCHAR *pszVerNew,TCHAR *pszVerCur)
{
	DWORD dwVerCur=0,dwVerNew=0;
	BYTE byVer1,byVer2,byVer3,byVer4;
	VerDot2Space(pszVerNew);
	VerDot2Space(pszVerCur);
	_stscanf(pszVerCur,_T("%u %u %u %u"),&byVer1,&byVer2,&byVer3,&byVer4);
	dwVerCur=MAKELONG(MAKEWORD(byVer4,byVer3),MAKEWORD(byVer2,byVer1));
	_stscanf(pszVerNew,_T("%u %u %u %u"),&byVer1,&byVer2,&byVer3,&byVer4);
	dwVerNew=MAKELONG(MAKEWORD(byVer4,byVer3),MAKEWORD(byVer2,byVer1));
	return (dwVerCur<dwVerNew);
}

BOOL Sinstar_Update(LPCTSTR pszIme)
{
	TCHAR szPath1[MAX_PATH],szPath2[MAX_PATH],szSysPath[MAX_PATH];
	TCHAR szMutex[300];
	::GetSystemDirectory(szSysPath,MAX_PATH);
	_stprintf(szMutex,_T("mutex_sinstar2_%s"),pszIme);
	HANDLE hMutex=CreateMutex(NULL,FALSE,szMutex);
	if(!hMutex)
	{
		MessageBox(GetActiveWindow(),_T("mutex create error"),_T("?"),MB_OK);
		return 0;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if(MessageBox(GetActiveWindow(),_T("输入法正在使用，不能更新，查看哪些程序在使用吗？"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		{
			STARTUPINFO si={sizeof(STARTUPINFO),0};
			PROCESS_INFORMATION pi={0};
			_stprintf(szPath1,_T("findcaller.exe %s\\%s.ime"),szSysPath,pszIme);
			if(CreateProcess(_T("findcaller.exe"),szPath1,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}else
			{
				MessageBox(GetActiveWindow(),_T("启动程序失败"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
		return 0;
	}
	CloseHandle(hMutex);
	_stprintf(szPath1,_T("%s\\%s.ime"),g_szPath,pszIme);
	_stprintf(szPath2,_T("%s\\%s.ime"),szSysPath,pszIme);
	BOOL bRet=CopyFile(szPath1,szPath2,FALSE);
	if(!bRet) MessageBox(GetActiveWindow(),_T("复制文件失败！"),_T("提示"),MB_OK|MB_ICONSTOP);

	//update information of update
	TCHAR szDate[200],szConfig[MAX_PATH],szVerCur[20],szVerNew[20];
	DWORD dwVerNew=0;
	Helper_PEVersion(szPath1,&dwVerNew,NULL,NULL);
	Helper_VersionString(dwVerNew,szVerNew);

	_stprintf(szConfig,_T("%s\\sinstar2.ini"),szSysPath);
	GetPrivateProfileString(_T("update"),_T("version"),_T("0.0.0.0"),szVerCur,20,szConfig);
	if(CheckVersion(szVerNew,szVerCur))
	{
		Helper_VersionString(dwVerNew,szVerNew);
		WritePrivateProfileString(_T("update"),_T("version"),szVerNew,szConfig);		
	}
	SYSTEMTIME time;
	GetSystemTime(&time);
	_stprintf(szDate,_T("%d-%d-%d"),time.wMonth,time.wDay,time.wYear);
	WritePrivateProfileString(_T("update"),_T("date"),szDate,szConfig);
	return bRet;
}

BOOL Sinstar_Uninstall(LPCTSTR pszIme)
{
		// TODO: Place code here.
	TCHAR szSysPath[MAX_PATH];
	TCHAR szPath[MAX_PATH],szMutex[300];
	::GetSystemDirectory(szSysPath,MAX_PATH);
	_stprintf(szPath,_T("%s\\%s\\uninstime.dll"),g_szPath,pszIme);
	if(GetFileAttributes(szPath)==0xFFFFFFFF)
	{
		MessageBox(GetActiveWindow(),_T("没有找到输入法卸载模块"),_T("提示"),MB_OK|MB_ICONSTOP);
		return 0;
	}
	_stprintf(szMutex,_T("mutex_sinstar2_%s"),pszIme);
	HANDLE hMutex=CreateMutex(NULL,FALSE,szMutex);
	if(!hMutex)
	{
		MessageBox(GetActiveWindow(),_T("mutex create error"),_T("?"),MB_OK);
		return 0;
	}
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		CloseHandle(hMutex);
		if(MessageBox(GetActiveWindow(),_T("输入法正在使用，不能卸载，查看哪些程序在使用吗？"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
		{
			STARTUPINFO si={sizeof(STARTUPINFO),0};
			PROCESS_INFORMATION pi={0};
			_stprintf(szPath,_T("findcaller.exe %s\\%s.ime"),szSysPath,pszIme);
			if(CreateProcess(_T("findcaller.exe"),szPath,NULL,NULL,FALSE,0,NULL,NULL,&si,&pi))
			{
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}else
			{
				MessageBox(GetActiveWindow(),_T("启动程序失败"),_T("提示"),MB_OK|MB_ICONSTOP);
			}
		}
		return 0;
	}
	CloseHandle(hMutex);
	if(MessageBox(GetActiveWindow(),_T("确定卸载吗？"),_T("提示"),MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL) return 0;

	STARTUPINFO    si={0};    
	PROCESS_INFORMATION pi;         
	si.cb  = sizeof(si);  
	si.dwFlags = 0; 
	si.wShowWindow  = SW_HIDE;
	//以后台启动方式创建服务器进程
	TCHAR szCmdLine[300];
	_stprintf(szCmdLine,_T("%s /c %s.ime"),szPath,pszIme);
	if(!CreateProcess(NULL,szCmdLine,NULL,NULL,FALSE,CREATE_NEW_PROCESS_GROUP,NULL,NULL,&si,&pi))
	{
		MessageBox(GetActiveWindow(),_T("Create Process Error"),_T("?"),MB_OK);
		return 0;
	}
	WaitForSingleObject(pi.hProcess,INFINITE);
	//删除输入法文件
	_stprintf(szPath,_T("%s\\%s.ime"),szSysPath,pszIme);
	DeleteFile(szPath);
	//从路径配置文件中删除当前输入法条目
	_stprintf(szPath,_T("%s\\sinstar2.ini"),szSysPath);
	WritePrivateProfileString(_T("client"),pszIme,NULL,szPath);//delete item from ini file
	//检查是否需要删除路径配置文件
	_stprintf(szPath,_T("%s\\%s\\register.log"),g_szPath,pszIme);
	int bMySvr=GetPrivateProfileInt(_T("log"),_T("mysvr"),0,szPath);
	if(bMySvr)
	{
		//退出服务器
		HANDLE hMapData=OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE,FALSE,NAME_MAPFILE);
		if(hMapData)
		{
			HWND hWndSvr=0;
			LPVOID pData=MapViewOfFile(hMapData,FILE_MAP_READ|FILE_MAP_WRITE,0,0,0);
			memcpy(&hWndSvr,pData,sizeof(HWND));
			UnmapViewOfFile(pData);
			CloseHandle(hMapData);
			if(IsWindow(hWndSvr))
			{
				DWORD dwProcID=0;
				GetWindowThreadProcessId(hWndSvr,&dwProcID);
				PostMessage(hWndSvr,WM_QUIT,0,0);
				if(dwProcID!=0)
					WaitForSingleObject((HANDLE)dwProcID,INFINITE);
				else
					Sleep(500);
			}
		}
		//删除安装时产生的log文件
		DeleteFile(szPath);
		//删除路径配置文件
		_stprintf(szPath,_T("%s\\sinstar2.ini"),szSysPath);
		DeleteFile(szPath);
	}
	return TRUE;
}

BOOL Is64OS()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
} 

BOOL MyCopyFile(LPCTSTR pszSour,LPCTSTR pszDest)
{
	TCHAR szSour[512]={0},szDest[512]={0};
	_tcscpy(szSour,pszSour);
	_tcscpy(szDest,pszDest);

	SHFILEOPSTRUCT fileOp={0};
	fileOp.wFunc=FO_COPY;
	fileOp.pFrom=szSour;
	fileOp.pTo=szDest;
	fileOp.fFlags=FOF_NOCONFIRMATION|FOF_SILENT|FOF_FILESONLY;
	int nRet=SHFileOperation(&fileOp);
	return nRet==0;
}

BOOL Sinstar_Install(LPCTSTR pszImeName,LPCTSTR pszIme)
{
	TCHAR szSysPath[MAX_PATH],szPathCfg[MAX_PATH];
	TCHAR szPath1[300],szPath2[300],szBuf[MAX_PATH];
	TCHAR szSysWow64[MAX_PATH];

	GetSystemDirectory(szSysPath,MAX_PATH);
	if(Is64OS())
	{
		GetSystemWow64Directory(szSysWow64,MAX_PATH);
	}
	

	//step1:验证文件有效性
	_stprintf(szPath1,_T("%s\\%s.ime"),g_szPath,pszIme);
	if(GetFileAttributes(szPath1)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("输入法目录下没有找到输入法文件：%s.ime"),pszIme);
		MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	_stprintf(szPath1,_T("%s\\server\\isserver.exe"),g_szPath);
	if(GetFileAttributes(szPath1)==0xFFFFFFFF)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("服务器目录下没有找到isserver.exe"));
		MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	
	//step2:获得路径配置文件
	_stprintf(szPathCfg,_T("%s\\sinstar2.ini"),szSysPath);
	//step3:复制输入法文件
	BOOL bInstalled=GetPrivateProfileString(_T("client"),pszIme,_T(""),szBuf,MAX_PATH,szPathCfg);
	if(bInstalled)
	{
		TCHAR szMsg[100];
		_stprintf(szMsg,_T("系统中已经安装了[%s],重新安装吗?"),pszImeName);
		if(MessageBox(GetActiveWindow(),szMsg,_T("提示"),MB_OKCANCEL|MB_ICONSTOP)==IDCANCEL)
			return FALSE;
	}

	if(Is64OS())
	{
		//复制64位版本到系统目录
		_stprintf(szPath1,_T("%s\\x64\\%s.ime"),g_szPath,pszIme);
// 		_stprintf(szPath2,_T("%s\\%s.ime"),szSysPath,pszIme);
		MyCopyFile(szPath1,szSysPath);
		//复制32位版本到wow64目录
		_stprintf(szPath1,_T("%s\\%s.ime"),g_szPath,pszIme);
// 		_stprintf(szPath2,_T("%s\\%s.ime"),szSysWow64,pszIme);
		MyCopyFile(szPath1,szSysWow64);
	}else
	{
		_stprintf(szPath1,_T("%s\\%s.ime"),g_szPath,pszIme);
		_stprintf(szPath2,_T("%s\\%s.ime"),szSysPath,pszIme);
		CopyFile(szPath1,szPath2,FALSE);
	}
	//注册输入法文件
	_stprintf(szPath1,_T("%s\\%s"),g_szPath,pszIme);
	WritePrivateProfileString(_T("client"),pszIme,szPath1,szPathCfg);

	//step4:配置服务程序
	GetPrivateProfileString(_T("server"),_T("exe"),_T(""),szPath1,MAX_PATH,szPathCfg);
	if(GetFileAttributes(szPath1)==0xFFFFFFFF)
	{//没有安装服务器
		_stprintf(szPath1,_T("%s\\server\\isserver.exe"),g_szPath);
		WritePrivateProfileString(_T("server"),_T("exe"),szPath1,szPathCfg);
		_stprintf(szPath1,_T("%s\\server"),g_szPath);
		WritePrivateProfileString(_T("server"),_T("data"),szPath1,szPathCfg);
		//todo:log action
		_stprintf(szPath1,_T("%s\\%s\\register.log"),g_szPath,pszIme);
		WritePrivateProfileString(_T("log"),_T("mysvr"),_T("1"),szPath1);
	}
	//step5:安装输入法
	_stprintf(szPath1,_T("%s\\%s.ime"),szSysPath,pszIme);
	ImmInstallIME(szPath1,pszImeName);	//输入法注册时直接使用32位版本

	//step6:修改配置文件属性
	_stprintf(szPath1,_T("%s\\%s\\config.ini"),g_szPath,pszIme);
	Helper_SetFileACL(szPath1);
	_stprintf(szPath1,_T("%s\\server\\config.ini"),g_szPath);
	Helper_SetFileACLEx(szPath1,TRUE);

	return TRUE;
}

void Helper_CenterWindow(HWND hWnd,UINT uFlag)
{
	RECT rc,screenrc;
	int x,y;
	SystemParametersInfo(SPI_GETWORKAREA,
		0,
		&screenrc,
		0);
	GetWindowRect(hWnd,&rc);
	x=(screenrc.right-rc.right+rc.left)/2;
	y=(screenrc.bottom-rc.bottom+rc.top)/2;
	SetWindowPos(hWnd,NULL,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER|uFlag);
}


BOOL CALLBACK DlgProc_Option(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	static BOOL bCancel=FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			DLGPARAM *pdlgParam=(DLGPARAM*)lParam;
			SetDlgItemText(hwndDlg,IDC_IME,pdlgParam->szIme);
			SetDlgItemText(hwndDlg,IDC_NAME,pdlgParam->szName);
			HICON hIcon=LoadIcon(pdlgParam->hInst,MAKEINTRESOURCE(IDI_FLAG));
			SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			SendMessage(hwndDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			Helper_CenterWindow(hwndDlg,0);
		}
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_INSTALL:
				{
					TCHAR szIme[50],szName[100];
					GetDlgItemText(hwndDlg,IDC_IME,szIme,50);
					GetDlgItemText(hwndDlg,IDC_NAME,szName,100);
					EndDialog(hwndDlg,Sinstar_Install(szName,szIme));
				}
				break;
			case IDC_UNINSTALL:
				{
					TCHAR szIme[50];
					GetDlgItemText(hwndDlg,IDC_IME,szIme,50);
					EndDialog(hwndDlg,Sinstar_Uninstall(szIme));
				}
				break;
			case IDC_UPDATE:
				{
					TCHAR szIme[50];
					GetDlgItemText(hwndDlg,IDC_IME,szIme,50);
					EndDialog(hwndDlg,Sinstar_Update(szIme));
				}
				break;
			case IDCANCEL:
				bCancel=TRUE;
				EndDialog(hwndDlg,IDCANCEL);
				break;
			}
		}
		break;
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	int nRet=0;
	DLGPARAM dlgParam={hInstance,0};
#ifdef _DEBUG
	GetCurrentDirectory(MAX_PATH,g_szPath);
#else//_DEBUG
	//获得程序启动目录
	TCHAR szFilePath[MAX_PATH];
	int nLen=GetModuleFileName(NULL,szFilePath,MAX_PATH);
	while(szFilePath[nLen]!='\\') nLen--;
	szFilePath[nLen]=0;
	_tcscpy(g_szPath,szFilePath);
#endif//_DEBUG
	if(!FindImeFile(dlgParam.szIme))
	{
		MessageBox(GetActiveWindow(),_T("没有找到输入法文件"),_T("提示"),MB_OK|MB_ICONSTOP);
		return 1;
	}
	if(!QueryImeProp(dlgParam.szIme,dlgParam.szName))
	{
		MessageBox(GetActiveWindow(),_T("查询输入法文件信息失败"),_T("提示"),MB_OK|MB_ICONSTOP);
		return 1;
	}
	if(__argc==1)
	{
		nRet=DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_OPTION),GetActiveWindow(),DlgProc_Option,(LPARAM)&dlgParam);
	}else if(__argc==2 && (__targv[1][0]==_T('/') || __targv[1][0]==_T('-')))
	{
		switch(__targv[1][1])
		{
		case _T('i'):case _T('I'):
			nRet=Sinstar_Install(dlgParam.szName,dlgParam.szIme);
			break;
		case _T('u'):case _T('U'):
			nRet=Sinstar_Uninstall(dlgParam.szIme);
			break;
		case _T('c'):case _T('C'):
			nRet=Sinstar_Update(dlgParam.szIme);
			break;
		default:
			MessageBox(GetActiveWindow(),_T("无效的参数"),_T("提示"),MB_OK|MB_ICONSTOP);
			nRet=1;
			break;
		}
	}
	return nRet;
}

