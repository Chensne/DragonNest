#include "StdAfx.h"


#ifdef PRE_ADD_BESTFRIEND

#include "DnItemTask.h"
#include "DnBestFriendProgressDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnBestFriendProgressDlg::CDnBestFriendProgressDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pText( NULL )
, m_pCancelButton( NULL )
, m_pProgressBarTime( NULL )
, m_fTimer(0.0f)
, m_pReqData(NULL)
{	
}

CDnBestFriendProgressDlg::~CDnBestFriendProgressDlg(void)
{	
	SAFE_DELETE( m_pReqData );
}

void CDnBestFriendProgressDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "AcceptDlg.ui" ).c_str(), bShow );
}

void CDnBestFriendProgressDlg::InitialUpdate()
{
	m_pText = GetControl<CEtUIStatic>("ID_TEXT");		
	m_pProgressBarTime = GetControl<CEtUIProgressBar>("ID_PROGRESSBAR_TIME");
}

void CDnBestFriendProgressDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() ) 
		return;
	
	if( m_fTimer >= 7.0f ) 
	{
		Refusal(); // 거절.
		Show( false );
	}
	m_pProgressBarTime->SetProgress( ( m_fTimer / 7.0f ) * 100.0f );

	m_fTimer += fElapsedTime;
}

void CDnBestFriendProgressDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// 수락.
		if( IsCmdControl("ID_OK" ) )
		{
			Accept();
			Show(false);
		}

		// 거절.
		else if( IsCmdControl("ID_CANCEL" ) )
		{			
			Refusal();
			Show(false);
		}		
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnBestFriendProgressDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		m_fTimer = 0.f;
		SAFE_DELETE( m_pReqData );
	}

	CEtUIDialog::Show( bShow );
}


void CDnBestFriendProgressDlg::SetData( BestFriend::SCRegistReq * pData, bool bOpen )
{
	SAFE_DELETE( m_pReqData );
	m_pReqData = new BestFriend::SCRegistReq;
	m_pReqData->nFromAccountDBID = pData->nFromAccountDBID;	
	m_pReqData->biFromCharacterDBID = pData->biFromCharacterDBID;
	_wcscpy( m_pReqData->wszFromName, _countof(m_pReqData->wszFromName), pData->wszFromName, (int)wcslen(pData->wszFromName) );

	wchar_t str[64]={0,};
	swprintf_s( str, 64, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4338), pData->wszFromName );
	m_pText->SetText( str ); // "[%s]님이 절친 수락을 요청중입니다. 수락 하시겠습니까?"


	// 절친요청 거절.
	if( bOpen == false )
		Refusal();
}


// 수락.
void CDnBestFriendProgressDlg::Accept()
{
	if( m_pReqData )
		GetItemTask().RequestAcceptCancelBF( true, m_pReqData->nFromAccountDBID, m_pReqData->biFromCharacterDBID, m_pReqData->wszFromName );
}


// 거절.
void CDnBestFriendProgressDlg::Refusal()
{
	if( m_pReqData )
		GetItemTask().RequestAcceptCancelBF( false, m_pReqData->nFromAccountDBID, m_pReqData->biFromCharacterDBID, m_pReqData->wszFromName );
}

#endif