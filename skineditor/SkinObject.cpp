// SkinObject.cpp: implementation of the CSkinObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "skineditor.h"
#include "SkinObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinObject::CSkinObject()
{
	m_szDescript[0]=0;
	m_dwID=0;
	m_dwData=0;
	m_bSelected=0;
	::SetRect(&m_rect,0,0,0,0);
}

CSkinObject::~CSkinObject()
{

}
