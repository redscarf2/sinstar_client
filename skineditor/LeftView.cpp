// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "skineditor.h"

#include "skineditorDoc.h"
#include "LeftView.h"
#include "PropertyGridItemEx.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IMGEXT_ALL "Ƥ��֧�ָ�ʽ(*.PNG;*.BMP)|*.png; *.bmp|�����ļ�(*.*)|*.*||"
#define IMGEXT_PNG "32λPNGͼƬ(*.PNG)|*.png|�����ļ�(*.*)|*.*||"
#define IMGEXT_BMP "λͼ�ļ�(*.BMP)|*.bmp|�����ļ�(*.*)|*.*||"

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CView)

BEGIN_MESSAGE_MAP(CLeftView, CView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(PGWM_PROPERTYGRID_NOTIFY, OnPropertyGridNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView construction/destruction

CLeftView::CLeftView()
{
	// TODO: add construction code here

}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView drawing

void CLeftView::OnDraw(CDC* pDC)
{
	CSkineditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


void CLeftView::AddButtonItem(CPropertyGridItem *pGroup,LPCTSTR pszName, UINT uID, BTNINFO *pBtn,BOOL bPosReadOnly,UINT uCoor)
{
	CSkineditorDoc* pDoc = GetDocument();
	char szBuf[50];
	CPropertyGridItem * pItem=new CPropertyGridItemFileEx(pszName,pBtn->szFile,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(uID);
	pGroup->AddChildItem(pItem);
	CPropertyGridItem * pSubItem=NULL;
	if(uCoor&COOR_SRK)
	{
		sprintf(szBuf,"%d,%d,%d,%d",pBtn->rcShrink.left,pBtn->rcShrink.top,pBtn->rcShrink.right,pBtn->rcShrink.bottom);
		pSubItem=new CPropertyGridItem("����(����)",szBuf);
		pSubItem->SetID(uID+1);
		if(bPosReadOnly) pSubItem->SetReadOnly(TRUE);
		pItem->AddChildItem(pSubItem);
	}
	if(uCoor&COOR_FULL)
	{
		sprintf(szBuf,"%d,%d,%d,%d",pBtn->rcFull.left,pBtn->rcFull.top,pBtn->rcFull.right,pBtn->rcFull.bottom);
		pSubItem=new CPropertyGridItem("����(����)",szBuf);
		pSubItem->SetID(uID+2);
		if(bPosReadOnly) pSubItem->SetReadOnly(TRUE);
		pItem->AddChildItem(pSubItem);
	}
	pSubItem=new CPropertyGridItem("��ʾ",pBtn->szTip);
	pSubItem->SetID(uID+3);
	pItem->AddChildItem(pSubItem);
}

void CLeftView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CSkineditorDoc *pDoc=(CSkineditorDoc *)GetDocument();
	// TODO: You may populate your TreeView with items by directly accessing
	pDoc->SetTitle(pDoc->m_skinDesc.szName);

	CRect rc;
	GetClientRect(&rc);
	m_wndGrid.Create(rc,this,0);

	CPropertyGridItem *pGroup=NULL,*pItem=NULL,*pSubItem=NULL;
	char szBuf[100];

	pGroup = m_wndGrid.AddCategory( _T("����") );
	pItem=new CPropertyGridItem("����",pDoc->m_skinDesc.szName);
	pItem->SetID(SDID_NAME);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItem("����",pDoc->m_skinDesc.szAuthor);
	pItem->SetID(SDID_AUTHOR);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();
	//���봰��
	pGroup = m_wndGrid.AddCategory( _T("���봰��[ͨ��]") );
	pItem=new CPropertyGridItemFontEx(_T("����"), &pDoc->m_skinInput.lf);
	pItem->SetID(SIID_FONT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemFileEx("ǰ����ť",pDoc->m_skinInput.szBtnPrev,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SIID_BTNPREV);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemFileEx("�󷭰�ť",pDoc->m_skinInput.szBtnNext,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SIID_BTNNEXT);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemFileEx("���״̬",pDoc->m_skinInput.szSymbolMode,TRUE,IMGEXT_PNG,pDoc->m_strPath);
	pItem->SetID(SIID_SYMBOL);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();
	//--------------------------------------------------------
	pGroup = m_wndGrid.AddCategory( _T("���봰��[����]") );
	pItem=new CPropertyGridItemFileEx("����",pDoc->m_skinInput.loHorz.szBgImg,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SIID_BGIMG);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemBool("����",pDoc->m_skinInput.loHorz.bHeteroMorphic);
	pItem->SetID(SIID_HETEROMORPHIC);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColorEx(_T("͸��ɫ"),pDoc->m_skinInput.loHorz.crTrans);
	pItem->SetID(SIID_TRANS);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemNumber("��߳���",pDoc->m_skinInput.loHorz.nLeft);
	pItem->SetID(SIID_LEFT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemBool("�м�ƽ��",pDoc->m_skinInput.loHorz.bTile);
	pItem->SetID(SIID_TILE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemNumber("�ұ߳���",pDoc->m_skinInput.loHorz.nRight);
	pItem->SetID(SIID_RIGHT);
	pGroup->AddChildItem(pItem);

	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinInput.loHorz.rcMargin.left,
		pDoc->m_skinInput.loHorz.rcMargin.top,
		pDoc->m_skinInput.loHorz.rcMargin.right,
		pDoc->m_skinInput.loHorz.rcMargin.bottom);
	pItem=new CPropertyGridItem(_T("�߽�"),szBuf);
	pItem->SetID(SIID_MARGIN);
	pGroup->AddChildItem(pItem);

	sprintf(szBuf,"%d,%d",pDoc->m_skinInput.loHorz.nCompLeft,pDoc->m_skinInput.loHorz.nCompRight);
	pItem=new CPropertyGridItem(_T("����߽�"),szBuf);
	pItem->SetID(SIID_COMPMARGIN);
	pGroup->AddChildItem(pItem);

	sprintf(szBuf,"%d,%d",pDoc->m_skinInput.loHorz.nCandLeft,pDoc->m_skinInput.loHorz.nCandRight);
	pItem=new CPropertyGridItem(_T("����߽�"),szBuf);
	pItem->SetID(SIID_CANDMARGIN);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemNumber(_T("������"),pDoc->m_skinInput.loHorz.nCandInter);
	pItem->SetID(SIID_CANDINTER);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemNumber(_T("���봰�ڸ߶�"),pDoc->m_skinInput.loHorz.nCompHei);
	pItem->SetID(SIID_COMPHEI);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemNumber(_T("����������"),pDoc->m_skinInput.loHorz.nLineHei);
	pItem->SetID(SIID_LINEHEI);
	pGroup->AddChildItem(pItem);
	
	pGroup->Expand();

	//���봰��[����]
	pGroup = m_wndGrid.AddCategory( _T("���봰��[����]") );
	pItem=new CPropertyGridItemFileEx("����",pDoc->m_skinInput.loVert.szBgImg,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SIIDV_BGIMG);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemBool("����",pDoc->m_skinInput.loVert.bHeteroMorphic);
	pItem->SetID(SIIDV_HETEROMORPHIC);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColorEx(_T("͸��ɫ"),pDoc->m_skinInput.loVert.crTrans);
	pItem->SetID(SIIDV_TRANS);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemNumber("�ϱ߸߶�",pDoc->m_skinInput.loVert.nLeft);
	pItem->SetID(SIIDV_LEFT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemBool("�м�ƽ��",pDoc->m_skinInput.loVert.bTile);
	pItem->SetID(SIIDV_TILE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemNumber("�±߸߶�",pDoc->m_skinInput.loVert.nRight);
	pItem->SetID(SIIDV_RIGHT);
	pGroup->AddChildItem(pItem);
	
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinInput.loVert.rcMargin.left,
		pDoc->m_skinInput.loVert.rcMargin.top,
		pDoc->m_skinInput.loVert.rcMargin.right,
		pDoc->m_skinInput.loVert.rcMargin.bottom);
	pItem=new CPropertyGridItem(_T("�߽�"),szBuf);
	pItem->SetID(SIIDV_MARGIN);
	pGroup->AddChildItem(pItem);
	
	sprintf(szBuf,"%d,%d",pDoc->m_skinInput.loVert.nCompLeft,pDoc->m_skinInput.loVert.nCompRight);
	pItem=new CPropertyGridItem(_T("����߽�"),szBuf);
	pItem->SetID(SIIDV_COMPMARGIN);
	pGroup->AddChildItem(pItem);
	
	sprintf(szBuf,"%d,%d",pDoc->m_skinInput.loVert.nCandLeft,pDoc->m_skinInput.loVert.nCandRight);
	pItem=new CPropertyGridItem(_T("����߽�"),szBuf);
	pItem->SetID(SIIDV_CANDMARGIN);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemNumber(_T("������"),pDoc->m_skinInput.loVert.nCandInter);
	pItem->SetID(SIIDV_CANDINTER);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemNumber(_T("���봰�ڸ߶�"),pDoc->m_skinInput.loVert.nCompHei);
	pItem->SetID(SIIDV_COMPHEI);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemNumber(_T("����������"),pDoc->m_skinInput.loVert.nLineHei);
	pItem->SetID(SIIDV_LINEHEI);
	pGroup->AddChildItem(pItem);
	
	pGroup->Expand();
	//״̬������
	pGroup = m_wndGrid.AddCategory( _T("״̬��(ͨ��)") );
	pItem=new CPropertyGridItemBool("����",pDoc->m_skinStatus.bHeteroMorphic);
	pItem->SetID(SSID_HETEROMORPHIC);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemColorEx(_T("͸��ɫ"),pDoc->m_skinStatus.crTrans);
	pItem->SetID(SSID_TRANS);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemFontEx(_T("����"), &pDoc->m_skinStatus.lf);
	pItem->SetID(SSID_FONT);
	pGroup->AddChildItem(pItem);
	
	pItem=new CPropertyGridItemColor("״̬��ʾ",pDoc->m_skinStatus.crText);
	pItem->SetID(SSID_TEXTCOLOR);
	pGroup->AddChildItem(pItem);

	pItem=new CPropertyGridItemFileEx("�����־",pDoc->m_skinStatus.szIcon,TRUE,IMGEXT_BMP,pDoc->m_strPath);
	pItem->SetID(SSID_ICONIMG);
	pGroup->AddChildItem(pItem);
	pSubItem=new CPropertyGridItemColorEx("ͼ��͸��ɫ",pDoc->m_skinStatus.crIconTrans);
	pSubItem->SetID(SSID_ICONIMGTRANS);
	pItem->AddChildItem(pSubItem);


	pGroup->Expand();
	
	pGroup = m_wndGrid.AddCategory( _T("״̬��(����)") );
	pItem=new CPropertyGridItemFileEx("����",pDoc->m_skinStatus.szBgImgFull,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SSID_BGIMGFULL);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcDrag2.left,pDoc->m_skinStatus.rcDrag2.top,pDoc->m_skinStatus.rcDrag2.right,pDoc->m_skinStatus.rcDrag2.bottom);
	pItem=new CPropertyGridItem(_T("�϶���"),szBuf);
	pItem->SetID(SSID_DRAG2);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcIcon2.left,pDoc->m_skinStatus.rcIcon2.top,pDoc->m_skinStatus.rcIcon2.right,pDoc->m_skinStatus.rcIcon2.bottom);
	pItem=new CPropertyGridItem(_T("ͼ����"),szBuf);
	pItem->SetID(SSID_ICON2);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcText2.left,pDoc->m_skinStatus.rcText2.top,pDoc->m_skinStatus.rcText2.right,pDoc->m_skinStatus.rcText2.bottom);
	pItem=new CPropertyGridItem(_T("������ʾ"),szBuf);
	pItem->SetID(SSID_TEXT2);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();

	pGroup = m_wndGrid.AddCategory( _T("״̬��(����)") );
	pItem=new CPropertyGridItemFileEx("����",pDoc->m_skinStatus.szBgImgShrink,TRUE,IMGEXT_ALL,pDoc->m_strPath);
	pItem->SetID(SSID_BGIMGSHRINK);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcDrag.left,pDoc->m_skinStatus.rcDrag.top,pDoc->m_skinStatus.rcDrag.right,pDoc->m_skinStatus.rcDrag.bottom);
	pItem=new CPropertyGridItem(_T("�϶���"),szBuf);
	pItem->SetID(SSID_DRAG);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcIcon.left,pDoc->m_skinStatus.rcIcon.top,pDoc->m_skinStatus.rcIcon.right,pDoc->m_skinStatus.rcIcon.bottom);
	pItem=new CPropertyGridItem(_T("ͼ����"),szBuf);
	pItem->SetID(SSID_ICON);
	pGroup->AddChildItem(pItem);
	sprintf(szBuf,"%d,%d,%d,%d",pDoc->m_skinStatus.rcText.left,pDoc->m_skinStatus.rcText.top,pDoc->m_skinStatus.rcText.right,pDoc->m_skinStatus.rcText.bottom);
	pItem=new CPropertyGridItem(_T("������ʾ"),szBuf);
	pItem->SetID(SSID_TEXT);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();

	//��ť��Դ
	pGroup = m_wndGrid.AddCategory( _T("��ť��Դ") );
	AddButtonItem(pGroup,"��չ",SSID_BTNEXTENT,&pDoc->m_skinStatus.btnExtent,FALSE,COOR_SRK);
	AddButtonItem(pGroup,"����",SSID_BTNSHRINK,&pDoc->m_skinStatus.btnShrink,FALSE,COOR_FULL);
	AddButtonItem(pGroup,"ȫ��",SSID_BTNCHAR1,&pDoc->m_skinStatus.btnChar1);
	AddButtonItem(pGroup,"���",SSID_BTNCHAR2,&pDoc->m_skinStatus.btnChar2,TRUE);
	AddButtonItem(pGroup,"��¼��",SSID_BTNRECORD1,&pDoc->m_skinStatus.btnRecord1);
	AddButtonItem(pGroup,"��¼��",SSID_BTNRECORD2,&pDoc->m_skinStatus.btnRecord2,TRUE);
	AddButtonItem(pGroup,"�ʶ���",SSID_BTNREAD1,&pDoc->m_skinStatus.btnRead1);
	AddButtonItem(pGroup,"�ʶ���",SSID_BTNREAD2,&pDoc->m_skinStatus.btnRead2,TRUE);
	AddButtonItem(pGroup,"���ʿ�",SSID_BTNWORD1,&pDoc->m_skinStatus.btnWord1);
	AddButtonItem(pGroup,"���ʹ�",SSID_BTNWORD2,&pDoc->m_skinStatus.btnWord2,TRUE);
	AddButtonItem(pGroup,"��ѯ",SSID_BTNQUERY,&pDoc->m_skinStatus.btnQuery);
	AddButtonItem(pGroup,"���",SSID_BTNMAKEPHRASE,&pDoc->m_skinStatus.btnMakePhrase);
	AddButtonItem(pGroup,"�˵�",SSID_BTNMENU,&pDoc->m_skinStatus.btnMenu);
	AddButtonItem(pGroup,"����",SSID_BTNHELP,&pDoc->m_skinStatus.btnHelp);
	pGroup->Expand();

	//��������ɫ
	pGroup= m_wndGrid.AddCategory( _T("��������ɫ") );
	pItem=new CPropertyGridItemColor(_T("��ͨ����"),pDoc->m_colorComp.crNormal);
	pItem->SetID(CCID_NORMAL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����"),pDoc->m_colorComp.crEnCode);
	pItem->SetID(CCID_ENCODE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("���ʲ���"),pDoc->m_colorComp.crEnMakeup);
	pItem->SetID(CCID_ENMAKEUP);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("��������"),pDoc->m_colorComp.crEnPhonetic);
	pItem->SetID(CCID_ENPHONETIC);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("ƴ�����"),pDoc->m_colorComp.crSpResult);
	pItem->SetID(CCID_SPRESULT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("ƴ����ĸ"),pDoc->m_colorComp.crSpSpell);
	pItem->SetID(CCID_SPSPELL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("ƴ������"),pDoc->m_colorComp.crSpEdit);
	pItem->SetID(CCID_SPEDIT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("���ڻ���"),pDoc->m_colorComp.crSpLine);
	pItem->SetID(CCID_SPLINE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("��ʱƴ��ͷ"),pDoc->m_colorComp.crSpTmHead);
	pItem->SetID(CCID_SPTMHEAD);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("�ϴ�����"),pDoc->m_colorComp.crInput);
	pItem->SetID(CCID_INPUT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����ʧ��"),pDoc->m_colorComp.crSentInit);
	pItem->SetID(CCID_SENTINIT);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("���Ӽ���"),pDoc->m_colorComp.crSentNormal);
	pItem->SetID(CCID_SENTNORMAL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����ѡ��"),pDoc->m_colorComp.crSentSel);
	pItem->SetID(CCID_SENTSEL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("Uģʽ��־"),pDoc->m_colorComp.crUdHead);
	pItem->SetID(CCID_UDHEAD);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("Uģʽ����"),pDoc->m_colorComp.crUdCode);
	pItem->SetID(CCID_UDCODE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("Uģʽ����"),pDoc->m_colorComp.crUdMakeup);
	pItem->SetID(CCID_UDMAKEUP);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();
	//��������ɫ
	pGroup = m_wndGrid.AddCategory( _T("��������ɫ") );
	pItem=new CPropertyGridItemColor(_T("���"),pDoc->m_colorCand.crIndex);
	pItem->SetID(CCAID_INDEX);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("һ������"),pDoc->m_colorCand.crNormal);
	pItem->SetID(CCAID_NORMAL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("Ԥ������"),pDoc->m_colorCand.crForecast);
	pItem->SetID(CCAID_FORECAST);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("GBK����"),pDoc->m_colorCand.crInactive);
	pItem->SetID(CCAID_INACTIVE);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("�Զ�������"),pDoc->m_colorCand.crUserDef);
	pItem->SetID(CCAID_USERDEF);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("��ƴ����"),pDoc->m_colorCand.crMixSp);
	pItem->SetID(CCAID_MIXSP);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����"),pDoc->m_colorCand.crCompNormal);
	pItem->SetID(CCAID_COMPNORMAL);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("��ѯ��"),pDoc->m_colorCand.crCompWild);
	pItem->SetID(CCAID_COMPWILD);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����ͷ"),pDoc->m_colorCand.crEnHead);
	pItem->SetID(CCAID_ENHEAD);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("����"),pDoc->m_colorCand.crEnPhonetic);
	pItem->SetID(CCAID_ENPHONETIC);
	pGroup->AddChildItem(pItem);
	pItem=new CPropertyGridItemColor(_T("��ʾ��Ϣ"),pDoc->m_colorCand.crTip);
	pItem->SetID(CCAID_TIP);
	pGroup->AddChildItem(pItem);
	pGroup->Expand();
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSkineditorDoc* CLeftView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkineditorDoc)));
	return (CSkineditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView message handlers

void CLeftView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if(m_wndGrid.GetSafeHwnd())
	{
		m_wndGrid.MoveWindow(0,0,cx,cy);
	}
}

LRESULT CLeftView::OnPropertyGridNotify(WPARAM wParam, LPARAM lParam)
{
	int nAction = HIWORD(wParam);	// ���Ա��֪ͨ�¼�
//	int nID = LOWORD(wParam);		// ���Ա�Ŀؼ�ID
	CPropertyGridItem* pItem = (CPropertyGridItem*)lParam;	// �����¼���������
	DWORD dwID=pItem->GetID();
	switch(dwID)
	{
	case SIID_BGIMG:
	case SIID_BTNPREV:
	case SIID_BTNNEXT:
	case SIID_SYMBOL:
	case SIIDV_BGIMG:
	case SSID_BGIMGFULL:
	case SSID_BGIMGSHRINK:
	case SSID_ICONIMG:
	case SSID_BTNSHRINK:
	case SSID_BTNEXTENT:
	case SSID_BTNCHAR1:
	case SSID_BTNCHAR2:
	case SSID_BTNRECORD1:
	case SSID_BTNRECORD2:
	case SSID_BTNREAD1:
	case SSID_BTNREAD2:
	case SSID_BTNWORD1:
	case SSID_BTNWORD2:
	case SSID_BTNQUERY:
	case SSID_BTNMAKEPHRASE:
	case SSID_BTNMENU:
	case SSID_BTNHELP:
		{//��ʾͼƬԤ��
			CSkineditorDoc *pDoc=GetDocument();
			char szPath[MAX_PATH];
			sprintf(szPath,"%s\\%s",pDoc->m_strPath,pItem->GetValue());
			pDoc->GetPicPrevView()->m_ctrlImagePrev.SetImage(szPath);
		}
		break;
	}

	switch (nAction)
	{
	case PGN_SORTORDER_CHANGED:
		break;
	case PGN_SELECTION_CHANGED:
		break;
	case PGN_ITEMVALUE_CHANGED:
		OnItemValueChange(dwID,pItem);
		break;
	}

	return 0;
}

void CLeftView::OnItemValueChange(int nID, CPropertyGridItem *pItem)
{
	CSkineditorDoc *pDoc=(CSkineditorDoc *)GetDocument();
	CPropertyGridItemColor *pColor=NULL;
	CPropertyGridItemBool *pBool=NULL;
	CPropertyGridItemFontEx *pFont=NULL;
	CPropertyGridItemFile *pFile=NULL;
	CPropertyGridItemNumber *pNumber=NULL;
	pDoc->SetModifiedFlag(TRUE);
	switch(nID)
	{
	//��������
	case SDID_NAME:
		strcpy(pDoc->m_skinDesc.szName,pItem->GetValue());
		pDoc->SetTitle(pDoc->m_skinDesc.szName);
		break;
	case SDID_AUTHOR:
		strcpy(pDoc->m_skinDesc.szAuthor,pItem->GetValue());
		break;
	//���봰����ɫ
	case CCAID_INDEX:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crIndex=pColor->GetColor();
		break;
	case CCAID_NORMAL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crNormal=pColor->GetColor();
		break;
	case CCAID_FORECAST:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crForecast=pColor->GetColor();
		break;
	case CCAID_COMPNORMAL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crCompNormal=pColor->GetColor();
		break;
	case CCAID_COMPWILD:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crCompWild=pColor->GetColor();
		break;
	case CCAID_ENHEAD:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crEnHead=pColor->GetColor();
		break;
	case CCAID_ENPHONETIC:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crEnPhonetic=pColor->GetColor();
		break;
	case CCAID_TIP:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crTip=pColor->GetColor();
		break;
	case CCAID_INACTIVE:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crInactive=pColor->GetColor();
		break;
	case CCAID_USERDEF:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crUserDef=pColor->GetColor();
		break;
	case CCAID_MIXSP:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorCand.crMixSp=pColor->GetColor();
		break;
	//���봰����ɫ
	case CCID_NORMAL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crNormal=pColor->GetColor();
		break;
	case CCID_INPUT:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crInput=pColor->GetColor();
		break;
	case CCID_SENTINIT:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSentInit=pColor->GetColor();
		break;
	case CCID_SENTNORMAL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSentNormal=pColor->GetColor();
		break;
	case CCID_SENTSEL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSentSel=pColor->GetColor();
		break;
	case CCID_ENCODE:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crEnCode=pColor->GetColor();
		break;
	case CCID_ENMAKEUP:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crEnMakeup=pColor->GetColor();
		break;
	case CCID_ENPHONETIC:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crEnPhonetic=pColor->GetColor();
		break;
	case CCID_SPTMHEAD:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSpTmHead=pColor->GetColor();
		break;
	case CCID_SPRESULT:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSpResult=pColor->GetColor();
		break;
	case CCID_SPSPELL:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSpSpell=pColor->GetColor();
		break;
	case CCID_SPEDIT:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSpEdit=pColor->GetColor();
		break;
	case CCID_SPLINE:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crSpLine=pColor->GetColor();
		break;
	case CCID_UDHEAD:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crUdHead=pColor->GetColor();
		break;
	case CCID_UDCODE:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crUdCode=pColor->GetColor();
		break;
	case CCID_UDMAKEUP:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_colorComp.crUdMakeup=pColor->GetColor();
		break;
	//���봰��
	case SIID_FONT:
		pFont=(CPropertyGridItemFontEx*)pItem;
		pFont->GetFont(&pDoc->m_skinInput.lf);
		break;
	case SIID_BTNPREV:
		strcpy(pDoc->m_skinInput.szBtnPrev,pItem->GetValue());
		break;
	case SIID_BTNNEXT:
		strcpy(pDoc->m_skinInput.szBtnNext,pItem->GetValue());
		break;
	case SIID_SYMBOL:
		strcpy(pDoc->m_skinInput.szSymbolMode,pItem->GetValue());
		break;
	//---------------------------------------------------
	case SIID_BGIMG:
		strcpy(pDoc->m_skinInput.loHorz.szBgImg,pItem->GetValue());
		break;
	case SIID_HETEROMORPHIC:
		pBool=(CPropertyGridItemBool *)pItem;
		pDoc->m_skinInput.loHorz.bHeteroMorphic=pBool->GetBool();
		break;
	case SIID_TILE:
		pBool=(CPropertyGridItemBool *)pItem;
		pDoc->m_skinInput.loHorz.bTile=pBool->GetBool();
		break;
	case SIID_TRANS:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_skinInput.loHorz.crTrans=pColor->GetColor();
		break;
	case SIID_LEFT:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loHorz.nLeft=pNumber->GetNumber();
		break;
	case SIID_RIGHT:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loHorz.nRight=pNumber->GetNumber();
		break;
	case SIID_MARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",
				&pDoc->m_skinInput.loHorz.rcMargin.left,
				&pDoc->m_skinInput.loHorz.rcMargin.top,
				&pDoc->m_skinInput.loHorz.rcMargin.right,
				&pDoc->m_skinInput.loHorz.rcMargin.bottom);
		}
		break;
	case SIID_COMPMARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d",&pDoc->m_skinInput.loHorz.nCompLeft,&pDoc->m_skinInput.loHorz.nCompRight);
		}
		break;
	case SIID_CANDMARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d",&pDoc->m_skinInput.loHorz.nCandLeft,&pDoc->m_skinInput.loHorz.nCandRight);
		}
		break;
	case SIID_CANDINTER:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loHorz.nCandInter=pNumber->GetNumber();
		break;
	case SIID_COMPHEI:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loHorz.nCompHei=pNumber->GetNumber();
		break;
	case SIID_LINEHEI:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loHorz.nLineHei=pNumber->GetNumber();
		break;
	////////////////////////////////////////////////////
	case SIIDV_BGIMG:
		strcpy(pDoc->m_skinInput.loVert.szBgImg,pItem->GetValue());
		break;
	case SIIDV_HETEROMORPHIC:
		pBool=(CPropertyGridItemBool *)pItem;
		pDoc->m_skinInput.loVert.bHeteroMorphic=pBool->GetBool();
		break;
	case SIIDV_TILE:
		pBool=(CPropertyGridItemBool *)pItem;
		pDoc->m_skinInput.loVert.bTile=pBool->GetBool();
		break;
	case SIIDV_TRANS:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_skinInput.loVert.crTrans=pColor->GetColor();
		break;
	case SIIDV_LEFT:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loVert.nLeft=pNumber->GetNumber();
		break;
	case SIIDV_RIGHT:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loVert.nRight=pNumber->GetNumber();
		break;
	case SIIDV_MARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",
				&pDoc->m_skinInput.loVert.rcMargin.left,
				&pDoc->m_skinInput.loVert.rcMargin.top,
				&pDoc->m_skinInput.loVert.rcMargin.right,
				&pDoc->m_skinInput.loVert.rcMargin.bottom);
		}
		break;
	case SIIDV_COMPMARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d",&pDoc->m_skinInput.loVert.nCompLeft,&pDoc->m_skinInput.loVert.nCompRight);
		}
		break;
	case SIIDV_CANDMARGIN:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d",&pDoc->m_skinInput.loVert.nCandLeft,&pDoc->m_skinInput.loVert.nCandRight);
		}
		break;
	case SIIDV_CANDINTER:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loVert.nCandInter=pNumber->GetNumber();
		break;
	case SIIDV_COMPHEI:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loVert.nCompHei=pNumber->GetNumber();
		break;
	case SIIDV_LINEHEI:
		pNumber=(CPropertyGridItemNumber*)pItem;
		pDoc->m_skinInput.loVert.nLineHei=pNumber->GetNumber();
		break;

	//״̬��(ͨ��)
	case SSID_HETEROMORPHIC:
		pBool=(CPropertyGridItemBool *)pItem;
		pDoc->m_skinStatus.bHeteroMorphic=pBool->GetBool();
		break;
	case SSID_TRANS:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_skinStatus.crTrans=pColor->GetColor();
		break;
	case SSID_FONT:
		pFont=(CPropertyGridItemFontEx*)pItem;
		pFont->GetFont(&pDoc->m_skinStatus.lf);
		break;
	case SSID_TEXTCOLOR:
		{
			pColor=(CPropertyGridItemColor*)pItem;
			pDoc->m_skinStatus.crText=pColor->GetColor();		
		}
		break;
	case SSID_ICONIMG:
		pFile=(CPropertyGridItemFile*)pItem;
		strcpy(pDoc->m_skinStatus.szIcon,pFile->GetFileName());
		break;
	case SSID_ICONIMGTRANS:
		pColor=(CPropertyGridItemColor*)pItem;
		pDoc->m_skinStatus.crIconTrans=pColor->GetColor();
		break;
	//״̬��(����)
	case SSID_BGIMGFULL:
		pFile=(CPropertyGridItemFile*)pItem;
		strcpy(pDoc->m_skinStatus.szBgImgFull,pFile->GetFileName());
		break;
	case SSID_DRAG:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcDrag.left,&pDoc->m_skinStatus.rcDrag.top,&pDoc->m_skinStatus.rcDrag.right,&pDoc->m_skinStatus.rcDrag.bottom);
		}
		break;
	case SSID_ICON:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcIcon.left,&pDoc->m_skinStatus.rcIcon.top,&pDoc->m_skinStatus.rcIcon.right,&pDoc->m_skinStatus.rcIcon.bottom);
		}
		break;
	case SSID_TEXT:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcText.left,&pDoc->m_skinStatus.rcText.top,&pDoc->m_skinStatus.rcText.right,&pDoc->m_skinStatus.rcText.bottom);
		}
		break;
	//״̬��(����)
	case SSID_BGIMGSHRINK:
		pFile=(CPropertyGridItemFile*)pItem;
		strcpy(pDoc->m_skinStatus.szBgImgShrink,pFile->GetFileName());
		break;
	case SSID_DRAG2:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcDrag2.left,&pDoc->m_skinStatus.rcDrag2.top,&pDoc->m_skinStatus.rcDrag2.right,&pDoc->m_skinStatus.rcDrag2.bottom);
		}
		break;
	case SSID_ICON2:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcIcon2.left,&pDoc->m_skinStatus.rcIcon2.top,&pDoc->m_skinStatus.rcIcon2.right,&pDoc->m_skinStatus.rcIcon2.bottom);
		}
		break;
	case SSID_TEXT2:
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&pDoc->m_skinStatus.rcText2.left,&pDoc->m_skinStatus.rcText2.top,&pDoc->m_skinStatus.rcText2.right,&pDoc->m_skinStatus.rcText2.bottom);
		}
		break;
	//��ť��
	case SSID_BTNEXTENT:case SSID_BTNEXTENT+1:case SSID_BTNEXTENT+2:case SSID_BTNEXTENT+3:
		ParseButtonItem(pItem,nID-SSID_BTNEXTENT,pDoc->m_skinStatus.btnExtent);
		break;
	case SSID_BTNSHRINK:case SSID_BTNSHRINK+1:case SSID_BTNSHRINK+2:case SSID_BTNSHRINK+3:
		ParseButtonItem(pItem,nID-SSID_BTNSHRINK,pDoc->m_skinStatus.btnShrink);
		break;
	case SSID_BTNCHAR1:case SSID_BTNCHAR1+1:case SSID_BTNCHAR1+2:case SSID_BTNCHAR1+3:
		ParseButtonItem(pItem,nID-SSID_BTNCHAR1,pDoc->m_skinStatus.btnChar1);
		break;
	case SSID_BTNCHAR2:case SSID_BTNCHAR2+1:case SSID_BTNCHAR2+2:case SSID_BTNCHAR2+3:
		ParseButtonItem(pItem,nID-SSID_BTNCHAR2,pDoc->m_skinStatus.btnChar2);
		break;
	case SSID_BTNRECORD1:case SSID_BTNRECORD1+1:case SSID_BTNRECORD1+2:case SSID_BTNRECORD1+3:
		ParseButtonItem(pItem,nID-SSID_BTNRECORD1,pDoc->m_skinStatus.btnRecord1);
		break;
	case SSID_BTNRECORD2:case SSID_BTNRECORD2+1:case SSID_BTNRECORD2+2:case SSID_BTNRECORD2+3:
		ParseButtonItem(pItem,nID-SSID_BTNRECORD2,pDoc->m_skinStatus.btnRecord2);
		break;
	case SSID_BTNREAD1:case SSID_BTNREAD1+1:case SSID_BTNREAD1+2:case SSID_BTNREAD1+3:
		ParseButtonItem(pItem,nID-SSID_BTNREAD1,pDoc->m_skinStatus.btnRead1);
		break;
	case SSID_BTNREAD2:case SSID_BTNREAD2+1:case SSID_BTNREAD2+2:case SSID_BTNREAD2+3:
		ParseButtonItem(pItem,nID-SSID_BTNREAD2,pDoc->m_skinStatus.btnRead2);
		break;
	case SSID_BTNWORD1:case SSID_BTNWORD1+1:case SSID_BTNWORD1+2:case SSID_BTNWORD1+3:
		ParseButtonItem(pItem,nID-SSID_BTNWORD1,pDoc->m_skinStatus.btnWord1);
		break;
	case SSID_BTNWORD2:case SSID_BTNWORD2+1:case SSID_BTNWORD2+2:case SSID_BTNWORD2+3:
		ParseButtonItem(pItem,nID-SSID_BTNWORD2,pDoc->m_skinStatus.btnWord2);
		break;
	case SSID_BTNQUERY:case SSID_BTNQUERY+1:case SSID_BTNQUERY+2:case SSID_BTNQUERY+3:
		ParseButtonItem(pItem,nID-SSID_BTNQUERY,pDoc->m_skinStatus.btnQuery);
		break;
	case SSID_BTNMAKEPHRASE:case SSID_BTNMAKEPHRASE+1:case SSID_BTNMAKEPHRASE+2:case SSID_BTNMAKEPHRASE+3:
		ParseButtonItem(pItem,nID-SSID_BTNMAKEPHRASE,pDoc->m_skinStatus.btnMakePhrase);
		break;
	case SSID_BTNMENU:case SSID_BTNMENU+1:case SSID_BTNMENU+2:case SSID_BTNMENU+3:
		ParseButtonItem(pItem,nID-SSID_BTNMENU,pDoc->m_skinStatus.btnMenu);
		break;
	case SSID_BTNHELP:case SSID_BTNHELP+1:case SSID_BTNHELP+2:case SSID_BTNHELP+3:
		ParseButtonItem(pItem,nID-SSID_BTNHELP,pDoc->m_skinStatus.btnHelp);
		break;
	}

	//ͬ��������ѡ��ť
	switch(nID)
	{
	case SSID_BTNCHAR1R:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNCHAR2R);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnChar2.rcShrink=pDoc->m_skinStatus.btnChar1.rcShrink;
		}
		break;
	case SSID_BTNCHAR1RF:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNCHAR2RF);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnChar2.rcFull=pDoc->m_skinStatus.btnChar1.rcFull;
		}
		break;
	case SSID_BTNREAD1R:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNREAD2R);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnRead2.rcShrink=pDoc->m_skinStatus.btnRead1.rcShrink;
		}
		break;
	case SSID_BTNREAD1RF:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNREAD2RF);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnRead2.rcFull=pDoc->m_skinStatus.btnRead1.rcFull;
		}
		break;
	case SSID_BTNRECORD1R:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNRECORD2R);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnRecord2.rcShrink=pDoc->m_skinStatus.btnRecord1.rcShrink;
		}
		break;
	case SSID_BTNRECORD1RF:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNRECORD2RF);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnRecord2.rcFull=pDoc->m_skinStatus.btnRecord1.rcFull;
		}
		break;
	case SSID_BTNWORD1R:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNWORD2R);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnWord2.rcShrink=pDoc->m_skinStatus.btnWord1.rcShrink;
		}
		break;
	case SSID_BTNWORD1RF:
		{
			CPropertyGridItem *pItem2=m_wndGrid.FindItem(SSID_BTNWORD2RF);
			pItem2->SetValue(pItem->GetValue());
			pDoc->m_skinStatus.btnWord2.rcFull=pDoc->m_skinStatus.btnWord1.rcFull;
		}
		break;
	}
	BOOL bVertView=nID>=SIIDV_BGIMG && nID<=SIIDV_LINEHEI;
	pDoc->UpdateAllViews(this,MAKEWPARAM(0,bVertView?0:1));
}

void CLeftView::ParseButtonItem(CPropertyGridItem *pItem, int nType, BTNINFO &btn)
{
	switch(nType)
	{
	case 0://λͼ
		{
			CPropertyGridItemFile *pFile=(CPropertyGridItemFile*)pItem;
			strcpy(btn.szFile,pFile->GetFileName());
		}
		break;
	case 1://λ��(����)
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&btn.rcShrink.left,&btn.rcShrink.top,&btn.rcShrink.right,&btn.rcShrink.bottom);
		}
		break;
	case 2://λ��(����)
		{
			char szBuf[100];
			strcpy(szBuf,pItem->GetValue());
			sscanf(szBuf,"%d,%d,%d,%d",&btn.rcFull.left,&btn.rcFull.top,&btn.rcFull.right,&btn.rcFull.bottom);
		}
		break;
	case 3://��ʾ
		strcpy(btn.szTip,pItem->GetValue());
		break;
	}
}
