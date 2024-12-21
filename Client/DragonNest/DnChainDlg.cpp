#include "StdAfx.h"
#include "DnChainDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChainDlg::CDnChainDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCountDlg( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnChainDlg::~CDnChainDlg(void)
{
}

void CDnChainDlg::Initialize( bool bShow )
{
	if( !m_pCountCtl )
	{
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChainDlg.ui" ).c_str(), bShow );
	}
}

void CDnChainDlg::InitialUpdate()
{
	m_pCountCtl = GetControl<CDnChainCount>("ID_CHAIN_COUNT");
	m_pCountCtl->Show(false);
}

void CDnChainDlg::Render( float fElapsedTime )
{
	if( GetInterface().IsOpenBlind() )
		return;

	CEtUIDialog::Render( fElapsedTime );
}

void CDnChainDlg::SetChain( int nChain )
{
	m_pCountCtl->SetValue( nChain );
	m_fDelayTime = 1.0f;

	//SUICoord dlgCoord;
	//GetDlgCoord( dlgCoord );
	//dlgCoord.fWidth = m_pChainCount->GetControlWidth();
	//dlgCoord.fHeight = m_pChainCount->GetControlHeight();
	//SetDlgCoord( dlgCoord );
}