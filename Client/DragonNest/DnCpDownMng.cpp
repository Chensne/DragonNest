#include "StdAfx.h"
#include "DnCpDownMng.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCpDownMng::CDnCpDownMng(void)
{
	m_CurCountDlg		= NULL;
}

CDnCpDownMng::~CDnCpDownMng(void)
{
}

void CDnCpDownMng::SetCpDown(int point)
{
	CDnCpDownDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->Initialize(true);
	m_listCountDlg.push_back( pDlg );
	SUICoord parentCoord		= GetInterface().GetCpScoreDlgBaseCoord();
	pDlg->ShowCpDownCount(point, parentCoord.fX, parentCoord.fY);
	pDlg->Show(true);
	pDlg->ShowCount( true );
	m_CurCountDlg = pDlg;
}