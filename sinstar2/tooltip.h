#pragma once

#define CLSNAME_TOOLTIP "mytooltip"

typedef struct tagTOOLTIP
{
	UINT	uID;
	char	szText[1601];
	HFONT	hFont;
}TOOLTIP,FAR *LPTOOLTIP;

BOOL ToolTipWnd_Register(HINSTANCE hInstance);
void ToolTipWnd_Unregister(HINSTANCE hInstance);

