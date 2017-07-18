// DrawTransBack.cpp: implementation of the DrawTransBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawTransBack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define BLOCK_SIZE 10

void DrawTransBack(CDC *pDC,RECT rc)
{
	int y=0;
	int iRow=0;
	CBrush br[2];
	br[0].CreateSolidBrush(RGB(202,202,202));
	br[1].CreateSolidBrush(RGB(238,238,238));

	while(y<rc.bottom)
	{
		int x=0;
		int ibr=iRow%2;
		while(x<rc.right)
		{
			int nWid=BLOCK_SIZE;
			if(x+nWid>rc.right) nWid=rc.right-x;
			int nHei=BLOCK_SIZE;
			if(y+nHei>rc.bottom) nHei=rc.bottom-y;
			pDC->FillRect(CRect(CPoint(x,y),CSize(nWid,nHei)),br+ibr%2);
			ibr++;
			x+=BLOCK_SIZE;
		}
		y+=BLOCK_SIZE;
		iRow++;
	}
}
