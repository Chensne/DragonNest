#include "StdAfx.h"
#include "DnChainMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChainMng::CDnChainMng(void)
{
}

CDnChainMng::~CDnChainMng(void)
{
}

void CDnChainMng::SetChain( int nChain )
{
	CDnChainDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetChain( nChain );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}