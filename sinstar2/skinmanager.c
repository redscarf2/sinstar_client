#include "sinstar2.h"
#include <shellapi.h>
static int g_nSortType=-1;
static int g_nSortCol=-1;

typedef struct tagSKINDATA
{
	char szPath[200],szName[50],szAuthor[50];
}SKINDATA, *PSKINDATA;

DWORD ListView_GetItemData(HWND hList,int nItem)
{
	LVITEM lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.iItem = nItem;
	lvi.mask = LVIF_PARAM;
	SendMessage(hList, LVM_GETITEM, 0, (LPARAM)&lvi);
	return (DWORD)lvi.lParam;
}

void SkinManager_List_RemoveAll(HWND hList)
{
	int i=0,nCount=ListView_GetItemCount(hList);
	for(i=0;i<nCount;i++)
	{
		free((PSKINDATA)ListView_GetItemData(hList,i));
	}
	ListView_DeleteAllItems(hList);
}

void SkinManager_List_Init(HWND hList)
{
	HANDLE hFindFile=NULL;
	WIN32_FIND_DATA wfd;
	char szFilter[MAX_PATH];
	sprintf(szFilter,"%s\\skins\\*.*",g_szDataPath);
	SkinManager_List_RemoveAll(hList);
	hFindFile=FindFirstFile(szFilter,&wfd);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	{
		char szConfig[MAX_PATH];
		char szName[50],szAuthor[50];
		int iSkin=0;
		LVITEM lv={0};
		lv.mask=LVIF_TEXT|LVIF_PARAM;
		do
		{
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY 
				&& strcmp(wfd.cFileName,".")!=0
				&& strcmp(wfd.cFileName,"..")!=0)
			{
				sprintf(szConfig,"%s\\Skins\\%s\\config.ini",g_szDataPath,wfd.cFileName);
				if(GetPrivateProfileString("desc","name","",szName,50,szConfig)
					&& GetPrivateProfileString("desc","author","",szAuthor,50,szConfig))
				{
					PSKINDATA pSkinData=(PSKINDATA)malloc(sizeof(SKINDATA));
					strcpy(pSkinData->szPath,wfd.cFileName);
					strcpy(pSkinData->szName,szName);
					strcpy(pSkinData->szAuthor,szAuthor);
					lv.iItem=iSkin;
					lv.iSubItem=0;
					lv.lParam=(LPARAM)pSkinData;
					lv.pszText=szName;
					ListView_InsertItem(hList,&lv);
					ListView_SetItemText(hList,iSkin,1,szAuthor);
					iSkin++;
				}
			}
		}while(FindNextFile(hFindFile,&wfd));
		FindClose(hFindFile);
	}
}

int CALLBACK SkinManager_CompareFunc(LPARAM lParam1, LPARAM lParam2, 
							  LPARAM lParamSort)
{
	SKINDATA *ld1=(SKINDATA*)lParam1;
	SKINDATA *ld2=(SKINDATA*)lParam2;
	int nRet=0;
	if(g_nSortCol==0)
	{
		nRet=strcmp(ld1->szName,ld2->szName)*g_nSortType;	
	}else if(g_nSortCol==1)
	{
		nRet=strcmp(ld1->szAuthor,ld2->szAuthor)*g_nSortType;	
	}
	return nRet;
}

INT_PTR CALLBACK SkinManager_DlgProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
	BOOL bRet=FALSE;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hList=GetDlgItem(hwndDlg,IDC_LIST_SKIN);
			LV_COLUMN lvCol={LVCF_TEXT|LVCF_WIDTH|LVCF_FMT,LVCFMT_LEFT,135,0};
			SetWindowLongPtr(hwndDlg,GWLP_USERDATA,lParam);
			ListView_SetExtendedListViewStyle(hList,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
			lvCol.pszText="名称";
			ListView_InsertColumn(hList,0,&lvCol);
			lvCol.pszText="作者";
			ListView_InsertColumn(hList,1,&lvCol);
			SkinManager_List_Init(hList);
			Helper_CenterWindow(hwndDlg,0);
			g_nSortType=-1;
			g_nSortCol=-1;
		}
		break;
	case WM_DESTROY:
		SkinManager_List_RemoveAll(GetDlgItem(hwndDlg,IDC_LIST_SKIN));
		break;
	case WM_COMMAND:
		if(HIWORD(wParam)==BN_CLICKED)
		{
			switch(LOWORD(wParam))
			{
			case IDC_SKIN_REFRESH:
				SkinManager_List_Init(GetDlgItem(hwndDlg,IDC_LIST_SKIN));
				break;
			case IDC_SKIN_DIR:
				{
					char szSkinDir[MAX_PATH];
					sprintf(szSkinDir,"\"%s\\skins\"",g_szDataPath);
					ShellExecute(NULL,"open",szSkinDir,NULL,NULL,SW_SHOWDEFAULT);
				}
				break;
			case IDC_SKIN_DEL:
				{
					HWND hList=GetDlgItem(hwndDlg,IDC_LIST_SKIN);
					char szSkinDir[MAX_PATH]={0};
					SHFILEOPSTRUCT fo={hwndDlg,FO_DELETE,szSkinDir,NULL,FOF_ALLOWUNDO|FOF_SILENT,0};
					int  i=0,nSel=ListView_GetSelectedCount(hList);
					int  *pnIdx=(int*)malloc(nSel*sizeof(int));
					int nIdx=ListView_GetNextItem(hList,-1,LVIS_SELECTED);
					while(nIdx!=-1)
					{
						pnIdx[i++]=nIdx;
						nIdx=ListView_GetNextItem(hList,nIdx,LVIS_SELECTED);
					}
					for(i--;i>=0;i--)
					{
						PSKINDATA pSkin=(PSKINDATA)ListView_GetItemData(hList,pnIdx[i]);
						if(_stricmp(pSkin->szPath,"default")==0)
						{
							MessageBox(GetActiveWindow(),"不能删除默认皮肤","提示",MB_OK|MB_ICONSTOP);
							continue;
						}
						if(_stricmp(pSkin->szPath,g_szSkin)==0)
						{
							MessageBox(GetActiveWindow(),"不能删除当前正在使用皮肤","提示",MB_OK|MB_ICONSTOP);
							continue;
						}
						sprintf(szSkinDir,"%s\\skins\\%s",g_szDataPath,pSkin->szPath);
						if(0==SHFileOperation(&fo))
						{
							free(pSkin);
							ListView_DeleteItem(hList,pnIdx[i]);
						}
					}
					free(pnIdx);
				}
				break;
			case IDC_SKIN_EDIT:
				{
					char szConfig[MAX_PATH];
					char szEditor[MAX_PATH]={0};
					ISComm_GetConfig(szConfig);
					GetPrivateProfileString("SKIN","editor",NULL,szEditor,MAX_PATH,szConfig);
					if(szEditor[0])
					{
						HWND hList=GetDlgItem(hwndDlg,IDC_LIST_SKIN);
						int iSkin=ListView_GetSelectionMark(hList);
						if(iSkin!=-1)
						{
							char szSkin[MAX_PATH];
							sprintf(szSkin,"\"%s\\skins\\%s\\config.ini\"",g_szDataPath,((PSKINDATA)ListView_GetItemData(hList,iSkin))->szPath);
							ShellExecute(NULL,"open",szEditor,szSkin,NULL,SW_SHOWDEFAULT);
						}
					}else
					{
						MessageBox(GetActiveWindow(),"没有找到皮肤编辑器,请先下载皮肤编辑器并至少执行一次！","提示",MB_OK|MB_ICONSTOP);
					}
				}
				break;
			case IDOK:
				{
					HWND hList=GetDlgItem(hwndDlg,IDC_LIST_SKIN);
					int iSkin=ListView_GetSelectionMark(hList);
					if(iSkin!=-1)
					{
						PSKINDATA pSkin=(PSKINDATA)ListView_GetItemData(hList,iSkin);
						SendMessage((HWND)GetWindowLongPtr(hwndDlg,GWLP_USERDATA),WM_COMMAND,IDM_SKIN_SET,(LPARAM)pSkin->szPath);
					}
				}
				break;
			case IDCANCEL:
				EndDialog(hwndDlg,IDCANCEL);
				break;
			}
		}
		break;
	case WM_NOTIFY:
		if(wParam)
		{
			NMHDR* pNMHDR = (NMHDR*)lParam;
			switch(pNMHDR->code)
			{
			case NM_DBLCLK:
				SendMessage(hwndDlg,WM_COMMAND,MAKELONG(IDOK,BN_CLICKED),0);
				EndDialog(hwndDlg,IDOK);
				break;
			case LVN_COLUMNCLICK:
				{
					NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
					if(pNMListView->iSubItem!=g_nSortCol)
						g_nSortCol=pNMListView->iSubItem,g_nSortType=1;
					else
						g_nSortType*=-1;
					ListView_SortItems(GetDlgItem(hwndDlg,IDC_LIST_SKIN),SkinManager_CompareFunc,0);

				}
				break;
			}
		}
		break;
	}
	return bRet;
}

void SkinManager(HWND hStatusWnd)
{
	DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_SKIN_MANAGER),GetActiveWindow(),SkinManager_DlgProc,(LPARAM)hStatusWnd);
}
