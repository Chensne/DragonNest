#include "StdAfx.h"
#include "DnComboMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnComboMng::CDnComboMng(void)
{
}

CDnComboMng::~CDnComboMng(void)
{
}

void CDnComboMng::SetCombo( int nCombo, int nDelay )
{
	if( nCombo > 1 )
	{
		SetDelayTime( 0.0f );
	}

	CDnComboDlg *pDlg = m_CountDlgMemPool.Allocate();
	pDlg->Initialize( true );
	m_listCountDlg.push_back( pDlg );
	pDlg->SetCombo( nCombo, nDelay );
	pDlg->ShowCount( true );
	pDlg->Show( true );
}