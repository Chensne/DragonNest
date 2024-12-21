#include "StdAfx.h"


#ifdef PRE_ADD_BESTFRIEND

#include "DnItemTask.h"
#include "DnBestFriendRewardProgressDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnBestFriendRewardProgressDlg::CDnBestFriendRewardProgressDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pProgressBarTime( NULL )
, m_fTimer(0.0f)
,m_GiftSerial(-1)
,m_ItemID(-1)
{	
}

CDnBestFriendRewardProgressDlg::~CDnBestFriendRewardProgressDlg(void)
{	
}

void CDnBestFriendRewardProgressDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RandomItemDlg.ui" ).c_str(), bShow );
}

void CDnBestFriendRewardProgressDlg::InitialUpdate()
{
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnBestFriendRewardProgressDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() ) 
		return;

	if( m_fTimer >= 3.0f ) 
	{
		Accept(); // 수락.
		Show( false );
	}
	m_pProgressBarTime->SetProgress( ( m_fTimer / 3.0f ) * 100.0f );

	m_fTimer += fElapsedTime;
}

void CDnBestFriendRewardProgressDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// 거절.
		if( IsCmdControl("ID_BUTTON_CANCEL" ) )
		{			
			Cancel();
			Show(false);
		}		
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnBestFriendRewardProgressDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		m_fTimer = 0.f;		
		m_GiftSerial = -1;
		m_ItemID = -1;
	}

	CEtUIDialog::Show( bShow );
}


// 수락.
void CDnBestFriendRewardProgressDlg::Accept()
{
	if( m_GiftSerial != -1 && m_ItemID != -1 )
		GetItemTask().SendBFItemComplet( m_GiftSerial, m_ItemID );
}


// 취소.
void CDnBestFriendRewardProgressDlg::Cancel()
{
	GetItemTask().RequestBFItemCancel();
}

#endif