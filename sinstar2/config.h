#ifndef _CONFIG_
#define _CONFIG_

#define  NOPLUGIN	"不使用插件"

void Config_Load();
void Config_Save_Global(LPCTSTR pszIniFile);
void Config_Load_Global(LPCTSTR pszIniFile);
void Config_SaveInt(LPCTSTR pszKey,UINT nValue);

INT_PTR	PropSheet_Show(HWND hwndOwner, HINSTANCE hInst, int nStartPage);


#endif//_CONFIG_