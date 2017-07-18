// BmpCombineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "skineditor.h"
#include "BmpCombineDlg.h"
#include "helper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4) 
/////////////////////////////////////////////////////////////////////////////
// CBmpCombineDlg dialog


CBmpCombineDlg::CBmpCombineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpCombineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBmpCombineDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBmpCombineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpCombineDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBmpCombineDlg, CDialog)
	//{{AFX_MSG_MAP(CBmpCombineDlg)
	ON_BN_CLICKED(IDC_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BROWSE2, OnBrowse2)
	ON_BN_CLICKED(IDC_BROWSE3, OnBrowse3)
	ON_BN_CLICKED(IDC_BROWSE4, OnBrowse4)
	ON_BN_CLICKED(IDC_COMBINE, OnCombine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpCombineDlg message handlers

void CBmpCombineDlg::OnBrowse1() 
{
	CFileDialog openDlg(TRUE,NULL,NULL,0,"位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");
	if(openDlg.DoModal()==IDOK) SetDlgItemText(IDC_BITMAP1,openDlg.GetPathName());
}

void CBmpCombineDlg::OnBrowse2() 
{
	CFileDialog openDlg(TRUE,NULL,NULL,0,"位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");
	if(openDlg.DoModal()==IDOK) SetDlgItemText(IDC_BITMAP2,openDlg.GetPathName());
}

void CBmpCombineDlg::OnBrowse3() 
{
	CFileDialog openDlg(TRUE,NULL,NULL,0,"位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");
	if(openDlg.DoModal()==IDOK) SetDlgItemText(IDC_BITMAP3,openDlg.GetPathName());	
}

void CBmpCombineDlg::OnBrowse4() 
{
	CFileDialog openDlg(TRUE,NULL,NULL,0,"位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");
	if(openDlg.DoModal()==IDOK) SetDlgItemText(IDC_BITMAP4,openDlg.GetPathName());	
}

void CBmpCombineDlg::OnCombine() 
{
	char szFile1[MAX_PATH]={0};	
	char szFile2[MAX_PATH]={0};
	char szFile3[MAX_PATH]={0};
	char szFile4[MAX_PATH]={0};

	GetDlgItemText(IDC_BITMAP1,szFile1,MAX_PATH);
	GetDlgItemText(IDC_BITMAP2,szFile2,MAX_PATH);
	GetDlgItemText(IDC_BITMAP3,szFile3,MAX_PATH);
	GetDlgItemText(IDC_BITMAP4,szFile4,MAX_PATH);

	HBITMAP hBmp1=(HBITMAP)::LoadImage(NULL,szFile1,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	HBITMAP hBmp2=(HBITMAP)::LoadImage(NULL,szFile2,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	HBITMAP hBmp3=(HBITMAP)::LoadImage(NULL,szFile3,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

	if(hBmp1 && hBmp2 && hBmp3)
	{
		SIZE size1=Helper_GetBMPSize(hBmp1);
		SIZE size2=Helper_GetBMPSize(hBmp2);
		SIZE size3=Helper_GetBMPSize(hBmp3);
		if(size1.cx!=size2.cx || size1.cy!=size2.cy ||size2.cx!=size3.cx ||size2.cy!=size3.cy)
		{
			MessageBox("3个子图的大小不一致！","提示",MB_OK|MB_ICONSTOP);
		}else
		{
			CDC *pDC=GetDesktopWindow()->GetDC();
			CDC memdc;
			memdc.CreateCompatibleDC(pDC);
			CBitmap bmpMem;
			bmpMem.CreateCompatibleBitmap(pDC,size1.cx*3,size1.cy);
			CBitmap *pOldBmp=memdc.SelectObject(&bmpMem);
			Helper_DrawBMP(memdc.m_hDC,size1.cx*0,0,size1.cx,size1.cy,hBmp1,0,0);
			Helper_DrawBMP(memdc.m_hDC,size1.cx*1,0,size1.cx,size1.cy,hBmp2,0,0);
			Helper_DrawBMP(memdc.m_hDC,size1.cx*2,0,size1.cx,size1.cy,hBmp3,0,0);
			memdc.SelectObject(pOldBmp);
			
			BITMAPINFO bmi={0};
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
			bmi.bmiHeader.biWidth = size1.cx*3;
			bmi.bmiHeader.biHeight = size1.cy;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biCompression = BI_RGB; 
			
			GetDIBits(pDC->m_hDC,bmpMem,0,size1.cy,NULL,&bmi,DIB_RGB_COLORS);

			LPBYTE pBits=(LPBYTE)malloc(bmi.bmiHeader.biSizeImage);
			GetDIBits(pDC->m_hDC,bmpMem,0,size1.cy,pBits,&bmi,DIB_RGB_COLORS);
			
			FILE *f=fopen(szFile4,"wb");
			if(f)
			{
				BITMAPFILEHEADER FileHeader={0x4d42,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER),0,0,sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)};
				fwrite(&FileHeader,sizeof(BITMAPFILEHEADER),1,f);
				bmi.bmiHeader.biCompression=BI_RGB;
				fwrite(&bmi.bmiHeader,sizeof(BITMAPINFOHEADER),1,f);
				fwrite(pBits,bmi.bmiHeader.biSizeImage,1,f);
				fclose(f);
			}
			free(pBits);

			GetDesktopWindow()->ReleaseDC(pDC);

			MessageBox("合并完成");
		}
	}else
	{
		MessageBox("请先指定3个子位图","提示",MB_OK|MB_ICONWARNING);
	}

	if(hBmp1) DeleteObject(hBmp1);
	if(hBmp2) DeleteObject(hBmp2);
	if(hBmp3) DeleteObject(hBmp3);
}
