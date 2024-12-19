#include "stdafx.h"

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

#include "DnItemCompoundProgressDlg.h"

CDnItemCompoundProgressDlg::CDnItemCompoundProgressDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
,m_pCancel( NULL )
,m_pProgressBar( NULL )
, m_fTime( 0.f )
, m_cFlag( 0 )
{

}

CDnItemCompoundProgressDlg::~CDnItemCompoundProgressDlg(void)
{

}

void CDnItemCompoundProgressDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompoundGaugeDlg.ui" ).c_str(), bShow );
}

void CDnItemCompoundProgressDlg::InitialUpdate()
{
	m_pCancel = GetControl<CEtUIButton>("ID_BT_CANCE");
	m_pProgressBar = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");	
}

void CDnItemCompoundProgressDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	switch( m_cFlag ) {
		case 1:
			{
				m_fTime -= fElapsedTime;
				if( m_fTime <= 0.f ) {
					m_fTime = 0.f;
					m_pCancel->Enable( false );
					m_pProgressBar->Enable( false );
					m_cFlag = 0;
					//SendCompleteCharmItem( m_cInvenType, m_cInvenIndex, m_biInvenSerial, 0, 0, 0 );

					Show( false );
					m_pParentDialog->Show(false);
				}
				m_pProgressBar->SetProgress( 100.f - ( 100.f / 2.f * m_fTime ) );
			}
			break;
	}
}


void CDnItemCompoundProgressDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CANCEL" ) ) 
		{
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnItemCompoundProgressDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnItemCompoundProgressDlg::ShowEx( bool bShow, float fTime )
{
	Show( bShow );

	if( bShow ) {
		m_cFlag = 1;
		m_fTime = fTime;

		m_pCancel->Show( true );
		m_pProgressBar->Show( true );
		m_pCancel->Enable( true );
	}
}

#endif
