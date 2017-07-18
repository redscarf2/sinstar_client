// skineditorView.cpp : implementation of the CSkineditorView class
//

#include "stdafx.h"
#include "skineditor.h"

#include "skineditorDoc.h"
#include "skineditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIN_WID		600
#define MIN_HEI		400

/////////////////////////////////////////////////////////////////////////////
// CSkineditorView

IMPLEMENT_DYNCREATE(CSkineditorView, CFormView)

BEGIN_MESSAGE_MAP(CSkineditorView, CFormView)
	//{{AFX_MSG_MAP(CSkineditorView)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MODE, OnSelchangeTabMode)
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_SKINOBJCLICKED,OnSkinPrevClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkineditorView construction/destruction

CSkineditorView::CSkineditorView()
	: CFormView(CSkineditorView::IDD),m_spStatusShrink(TRUE),m_spStatusFull(TRUE)
{
	//{{AFX_DATA_INIT(CSkineditorView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
}

CSkineditorView::~CSkineditorView()
{
}

void CSkineditorView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkineditorView)
	DDX_Control(pDX, IDC_TAB_MODE, m_tabMode);
	DDX_Control(pDX, IDC_SP_STATUSSHRINK, m_spStatusShrink);
	DDX_Control(pDX, IDC_SP_STATUSFULL, m_spStatusFull);
	DDX_Control(pDX, IDC_SP_INPUTWND, m_spInputWnd);
	//}}AFX_DATA_MAP
}

BOOL CSkineditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CSkineditorView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ResizeParentToFit();
	m_tabMode.InsertItem(0,"普通模式");
	m_tabMode.InsertItem(1,"拼音模式");
	m_tabMode.InsertItem(2,"联想模式");
	m_tabMode.InsertItem(3,"语句模式");
	m_tabMode.InsertItem(4,"单词模式");
	m_tabMode.InsertItem(5,"自定义模式");
	InitInputWndPrev();
	InitStatusWndPrev_Shrink();
	InitStatusWndPrev_Full();
}

/////////////////////////////////////////////////////////////////////////////
// CSkineditorView diagnostics

#ifdef _DEBUG
void CSkineditorView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSkineditorView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSkineditorDoc* CSkineditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkineditorDoc)));
	return (CSkineditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkineditorView message handlers



void CSkineditorView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	if(cx<MIN_WID) cx=MIN_WID;
	if(cy<MIN_HEI) cy=MIN_HEI;
	SetScrollSizes(MM_TEXT,CSize(cx,cy));
	if(m_spInputWnd.GetSafeHwnd())
	{
		CRect rc;
		m_spInputWnd.GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_spInputWnd.SetWindowPos(NULL,0,0,cx-rc.left,rc.Height(),SWP_NOZORDER|SWP_NOMOVE);
	}
	if(m_spStatusFull.GetSafeHwnd())
	{
		CRect rc;
		m_spStatusFull.GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_spStatusFull.SetWindowPos(NULL,0,0,cx-rc.left,rc.Height(),SWP_NOZORDER|SWP_NOMOVE);
	}
}

void CSkineditorView::AddStatusButton(DWORD dwID, LPCTSTR pszPath, LPCTSTR pszName, RECT rc, SIZE sizeImage, CSkinPrevCtrl *pPrevCtrl)
{
	if(!IsRectEmpty(&rc) 
		&& rc.right<sizeImage.cx+5  
		&& rc.bottom<sizeImage.cy+5)
	{
		char szPath[MAX_PATH];
		sprintf(szPath,"%s\\%s",pszPath,pszName);
		DWORD dwAttr=GetFileAttributes(szPath);
		if(dwAttr!=0xFFFFFFFF && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
		{
			CSkinObjectButton *pButton=new CSkinObjectButton(dwID);
			pButton->SetImage(szPath);
			pButton->SetRect(rc);
			pPrevCtrl->AddObject(pButton);
		}
	}
}

void CSkineditorView::InitInputWndPrev()
{
	char szPath[MAX_PATH];
	CSkineditorDoc *pDoc=GetDocument();
	//bk image
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinInput.loHorz.szBgImg);
	m_spInputWnd.SetBgImage(szPath);
	m_spInputWnd.InitObjects(pDoc->m_strPath,&pDoc->m_skinInput,&pDoc->m_colorComp,&pDoc->m_colorCand);
	m_spInputWnd.SetMode(IWM_SHAPE);
}

void CSkineditorView::OnSelchangeTabMode(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nMode=m_tabMode.GetCurSel();
	m_spInputWnd.SetMode((IWM)nMode);
	*pResult = 0;
}

LRESULT CSkineditorView::OnSkinPrevClicked(WPARAM wParam, LPARAM lParam)
{
	CSkineditorDoc *pDoc=GetDocument();
	CPropertyGridItem *pItem=pDoc->GetLeftView()->m_wndGrid.FindItem(lParam);
	if(pItem) 
	{
		pDoc->GetLeftView()->m_wndGrid.SetFocus();
		CPropertyGridItem *pParent=pItem->GetParentItem();
		if(pParent)
		{
			if(!pParent->IsExpanded()) pParent->Expand();
		}
		pItem->Select();
	}
	return 0;
}

void CSkineditorView::InitStatusWndPrev_Shrink()
{
	char szPath[MAX_PATH];
	CSkineditorDoc *pDoc=GetDocument();
	m_spStatusShrink.RemoveObjects();
	SIZE size;
	m_spStatusShrink.SetFontInfo(&pDoc->m_skinStatus.lf);
	CFont font;
	font.CreateFontIndirect(m_spStatusShrink.GetFontInfo());
	CDC *pDC=m_spStatusShrink.GetDC();
	CFont *pOldFont=pDC->SelectObject(&font);
	//bk image
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.szBgImgShrink);
	m_spStatusShrink.SetBgImage(szPath);

	CSkinObjectFrame *pFrame=new CSkinObjectFrame(SSID_DRAG);
	pFrame->SetRect(pDoc->m_skinStatus.rcDrag);
	m_spStatusShrink.AddObject(pFrame);

	pFrame=new CSkinObjectFrame(SSID_TEXT);
	pFrame->SetRect(pDoc->m_skinStatus.rcText);
	m_spStatusShrink.AddObject(pFrame);

	CSkinObjectText *pText=new CSkinObjectText("启程拼音",SSID_TEXTCOLOR);
	pText->SetColor(pDoc->m_skinStatus.crText);
	size=pDC->GetTextExtent("启程拼音");
	RECT rc=pDoc->m_skinStatus.rcText;
	int left=(rc.right-rc.left-size.cx)/2;
	int top=(rc.bottom-rc.top-size.cy)/2;
	::InflateRect(&rc,-left,-top);
	pText->SetRect(rc);
	m_spStatusShrink.AddObject(pText);

	CSkinObjectFlag *pFlag=new CSkinObjectFlag(SSID_ICON);
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.szIcon);
	DWORD dwAttr=GetFileAttributes(szPath);
	if(dwAttr!=0xFFFFFFFF && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
		pFlag->SetFlagFile(szPath,pDoc->m_skinStatus.crIconTrans);
	else
		pFlag->SetFlagResource(IDB_FLAG,RGB(255,0,255));
	pFlag->SetRect(pDoc->m_skinStatus.rcIcon);
	m_spStatusShrink.AddObject(pFlag);

	SIZE sizeImage=m_spStatusShrink.GetSize();
	CSkinObjectButton *pButton=new CSkinObjectButton(SSID_BTNEXTENT);
	pButton->SetRect(pDoc->m_skinStatus.btnExtent.rcShrink);
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.btnExtent.szFile);
	pButton->SetImage(szPath);
	m_spStatusShrink.AddObject(pButton);
	
	AddStatusButton(SSID_BTNCHAR1,pDoc->m_strPath,pDoc->m_skinStatus.btnChar1.szFile,pDoc->m_skinStatus.btnChar1.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNRECORD1,pDoc->m_strPath,pDoc->m_skinStatus.btnRecord1.szFile,pDoc->m_skinStatus.btnRecord1.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNREAD1,pDoc->m_strPath,pDoc->m_skinStatus.btnRead1.szFile,pDoc->m_skinStatus.btnRead1.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNWORD1,pDoc->m_strPath,pDoc->m_skinStatus.btnWord1.szFile,pDoc->m_skinStatus.btnWord1.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNQUERY,pDoc->m_strPath,pDoc->m_skinStatus.btnQuery.szFile,pDoc->m_skinStatus.btnQuery.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNMAKEPHRASE,pDoc->m_strPath,pDoc->m_skinStatus.btnMakePhrase.szFile,pDoc->m_skinStatus.btnMakePhrase.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNMENU,pDoc->m_strPath,pDoc->m_skinStatus.btnMenu.szFile,pDoc->m_skinStatus.btnMenu.rcShrink,sizeImage,&m_spStatusShrink);
	AddStatusButton(SSID_BTNHELP,pDoc->m_strPath,pDoc->m_skinStatus.btnHelp.szFile,pDoc->m_skinStatus.btnHelp.rcShrink,sizeImage,&m_spStatusShrink);

	pDC->SelectObject(&pOldFont);
}

void CSkineditorView::InitStatusWndPrev_Full()
{
	char szPath[MAX_PATH];
	CSkineditorDoc *pDoc=GetDocument();
	m_spStatusFull.RemoveObjects();
	SIZE size;
	m_spStatusFull.SetFontInfo(&pDoc->m_skinStatus.lf);
	CFont font;
	font.CreateFontIndirect(m_spStatusFull.GetFontInfo());
	CDC *pDC=m_spStatusFull.GetDC();
	CFont *pOldFont=pDC->SelectObject(&font);
	//bk image
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.szBgImgFull);
	m_spStatusFull.SetBgImage(szPath);

	CSkinObjectFrame *pFrame=new CSkinObjectFrame(SSID_DRAG2);
	pFrame->SetRect(pDoc->m_skinStatus.rcDrag2);
	m_spStatusFull.AddObject(pFrame);

	pFrame=new CSkinObjectFrame(SSID_TEXT2);
	pFrame->SetRect(pDoc->m_skinStatus.rcText2);
	m_spStatusFull.AddObject(pFrame);

	CSkinObjectText *pText=new CSkinObjectText("启程拼音",SSID_TEXTCOLOR);
	pText->SetColor(pDoc->m_skinStatus.crText);
	size=pDC->GetTextExtent("启程拼音");
	RECT rc=pDoc->m_skinStatus.rcText2;
	int left=(rc.right-rc.left-size.cx)/2;
	int top=(rc.bottom-rc.top-size.cy)/2;
	::InflateRect(&rc,-left,-top);
	pText->SetRect(rc);
	m_spStatusFull.AddObject(pText);

	CSkinObjectFlag *pFlag=new CSkinObjectFlag(SSID_ICON2);
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.szIcon);
	DWORD dwAttr=GetFileAttributes(szPath);
	if(dwAttr!=0xFFFFFFFF && !(dwAttr&FILE_ATTRIBUTE_DIRECTORY))
		pFlag->SetFlagFile(szPath,pDoc->m_skinStatus.crIconTrans);
	else
		pFlag->SetFlagResource(IDB_FLAG,RGB(255,0,255));
	pFlag->SetRect(pDoc->m_skinStatus.rcIcon2);
	m_spStatusFull.AddObject(pFlag);

	SIZE sizeImage=m_spStatusFull.GetSize();
	CSkinObjectButton *pButton=new CSkinObjectButton(SSID_BTNSHRINK);
	pButton->SetRect(pDoc->m_skinStatus.btnShrink.rcFull);
	sprintf(szPath,"%s\\%s",pDoc->m_strPath,pDoc->m_skinStatus.btnShrink.szFile);
	pButton->SetImage(szPath);
	m_spStatusFull.AddObject(pButton);
	
	AddStatusButton(SSID_BTNCHAR1,pDoc->m_strPath,pDoc->m_skinStatus.btnChar1.szFile,pDoc->m_skinStatus.btnChar1.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNRECORD1,pDoc->m_strPath,pDoc->m_skinStatus.btnRecord1.szFile,pDoc->m_skinStatus.btnRecord1.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNREAD1,pDoc->m_strPath,pDoc->m_skinStatus.btnRead1.szFile,pDoc->m_skinStatus.btnRead1.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNWORD1,pDoc->m_strPath,pDoc->m_skinStatus.btnWord1.szFile,pDoc->m_skinStatus.btnWord1.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNQUERY,pDoc->m_strPath,pDoc->m_skinStatus.btnQuery.szFile,pDoc->m_skinStatus.btnQuery.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNMAKEPHRASE,pDoc->m_strPath,pDoc->m_skinStatus.btnMakePhrase.szFile,pDoc->m_skinStatus.btnMakePhrase.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNMENU,pDoc->m_strPath,pDoc->m_skinStatus.btnMenu.szFile,pDoc->m_skinStatus.btnMenu.rcFull,sizeImage,&m_spStatusFull);
	AddStatusButton(SSID_BTNHELP,pDoc->m_strPath,pDoc->m_skinStatus.btnHelp.szFile,pDoc->m_skinStatus.btnHelp.rcFull,sizeImage,&m_spStatusFull);

	pDC->SelectObject(&pOldFont);
}



void CSkineditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(LOWORD(lHint)==0)
	{
        InitInputWndPrev();
		InitStatusWndPrev_Full();
		InitStatusWndPrev_Shrink();
	}
}
