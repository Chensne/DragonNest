#include "Stdafx.h"

#ifdef PRE_ADD_BESTFRIEND

#include "TaskManager.h"
#include "DnPartyTask.h"

#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"

#include "DnBestFriendDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


namespace
{
	wchar_t * g_strCountDown = NULL; // "��ģ �ı� ���� �� �Դϴ�. (%�� %�ð� %�� ����)"
};

CDnBestFriendDlg::CDnBestFriendDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
 : CEtUIDialog( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pStaticTex0( NULL )
, m_pStaticTex1( NULL )
, m_pStaticTex2( NULL )
, m_pStaticTex3( NULL )
, m_pStaticTex4( NULL )
, m_pStaticTex5( NULL )
, m_pStaticTex6( NULL )
, m_pStaticCond( NULL )
, m_pStaticTex7( NULL )
, m_pStaticTex8( NULL )	
, m_pStaticDate( NULL )
, m_pStaticDay( NULL )
, m_pStaticName( NULL ) 
, m_pStaticLevel( NULL )
, m_pStaticJob( NULL ) 
, m_pStaticLocation( NULL )
, m_pStaticMemo( NULL )
, m_pStaticFriendMemo( NULL )
, m_pStatiBrokeupTime( NULL )
, m_pStaticBrokeupNotice( NULL )
, m_pBtnBrokeup( NULL )
, m_pBtnBrokeupCancel( NULL )
, m_pBtnParty( NULL )
, m_pBtnMemo( NULL )
, m_biCharacterDBID( 0 )
, m_pStrCountDown( NULL )
, m_CountDown(0.0f)
, m_timeOneMin(0.0f)
, m_bBFState(true)
, m_bBFBrokeUpCancel(false)
, m_bAllowRequestBFInfo(true)
, m_TimeAllowBFInfo(0.0f)
{	
}

void CDnBestFriendDlg::ReleaseDlg()
{		
	delete [] m_pStrCountDown;
	m_pStrCountDown = NULL;

	m_vStaticEnable.clear();
}


void CDnBestFriendDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("BFDlg.ui").c_str(), bShow );
}


void CDnBestFriendDlg::InitialUpdate()
{
	m_pStrCountDown = new wchar_t[256];
	memset( m_pStrCountDown, 0, sizeof(wchar_t)*256);
	g_strCountDown = const_cast< wchar_t * >( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4330 ) ); // "��ģ �ı� ���� �� �Դϴ�. (%�� %�ð� %�� ����)"

	GetControl<CEtUIButton>("ID_BT_BROKEUPAGREE")->Show( false ); // ���X - ��ģ�ı⵿��.

	m_pStaticTex0 = GetControl< CEtUIStatic >("ID_TEXT0");
	m_pStaticTex1 = GetControl< CEtUIStatic >("ID_TEXT1");
	m_pStaticTex6 = GetControl< CEtUIStatic >("ID_TEXT6");
	m_pStaticCond = GetControl< CEtUIStatic >("ID_TEXT_CONDITION");
	m_pStaticTex7 = GetControl< CEtUIStatic >("ID_TEXT7");
	m_pStaticTex8 = GetControl< CEtUIStatic >("ID_TEXT8");	

	m_pStaticDate = GetControl< CEtUIStatic >("ID_TEXT_DAY");     // ��ģ������¥.
	m_pStaticDay = GetControl< CEtUIStatic >("ID_TEXT_DAYCOUNT"); // ��ģ���� ���κ��� ������¥.
	m_pStaticName = GetControl< CEtUIStatic >("ID_TEXT_NAME"); 
	m_pStaticLevel = GetControl< CEtUIStatic >("ID_TEXT_LEVEL");
	m_pStaticJob = GetControl< CEtUIStatic >("ID_TEXT_CLASS");
	m_pStaticLocation = GetControl< CEtUIStatic >("ID_TEXT_CONDITION");
	m_pStaticMemo = GetControl< CEtUIStatic >("ID_TEXT_MYMESSAGE");
	m_pStaticFriendMemo = GetControl< CEtUIStatic >("ID_TEXT_BFMESSAGE");
	m_pBtnBrokeup = GetControl<CEtUIButton>("ID_BT_BROKEUP");       // ��ģ�ı�.
	m_pBtnBrokeupAgreement = GetControl<CEtUIButton>("ID_BT_BROKEUPAGREE"); // ��ģ�ı⵿��. 

	// ��ģ�ı��� ��� Static��.
	m_pStatiBrokeupTime = GetControl< CEtUIStatic >("ID_TEXT_BROKEUPTIME");
	m_pStaticBrokeupNotice = GetControl< CEtUIStatic >("ID_TEXT_BROKEUPNOTICE");	
	m_pBtnBrokeupCancel = GetControl<CEtUIButton>("ID_BT_BROKEUPCANCEL"); // ��ģ�ı����.

	// ��ģ�ı�ÿ� Enable��ų Static��. 
	m_pStaticTex2 = GetControl< CEtUIStatic >("ID_TEXT2");
	m_pStaticTex3 = GetControl< CEtUIStatic >("ID_TEXT3");
	m_pStaticTex4 = GetControl< CEtUIStatic >("ID_TEXT4");
	m_pStaticTex5 = GetControl< CEtUIStatic >("ID_TEXT5");
	m_vStaticEnable.push_back( m_pStaticTex2 );
	m_vStaticEnable.push_back( m_pStaticTex3 );
	m_vStaticEnable.push_back( m_pStaticTex4 );
	m_vStaticEnable.push_back( m_pStaticTex5 );
	m_vStaticEnable.push_back( m_pStaticName);
	m_vStaticEnable.push_back( m_pStaticLevel);
	m_vStaticEnable.push_back( m_pStaticJob); 

	m_pBtnParty = GetControl<CEtUIButton>("ID_BT_PARTY"); // ��Ƽ��û.
	m_pBtnMemo = GetControl<CEtUIButton>("ID_BT_MYMESSAGE"); // �޸��Է�.

}


void CDnBestFriendDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	// ��ģ�ıⳲ���ð� ī��Ʈ�ٿ�.
	if( m_CountDown > 0.0f )
	{
		m_CountDown -= fElapsedTime;
		m_timeOneMin += fElapsedTime;

		if( m_timeOneMin >= 60.0f )
		{
			m_timeOneMin = 0.0f;

			int nCD = (int)m_CountDown;

			// "��ģ �ı� ���� �� �Դϴ�. (%d�� %d�ð� %d�� ����)"
			swprintf_s( m_pStrCountDown, 256, g_strCountDown, 
				        nCD / 86400,
						(nCD % 86400) / 3600,
						( (nCD % 86400) % 3600 ) / 60 );
			m_pStatiBrokeupTime->SetText( m_pStrCountDown );
		}
	}


	// ��ģ������û.
	if( !m_bAllowRequestBFInfo )
	{
		m_TimeAllowBFInfo += fElapsedTime;
		if( m_TimeAllowBFInfo >= 60.0f )
		{
			m_TimeAllowBFInfo = 0.0f;			
			m_bAllowRequestBFInfo = true;		
		}	
	}

}


void CDnBestFriendDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// ��ģ�ı�.
		if( IsCmdControl("ID_BT_BROKEUP") )
		{
			GetInterface().MessageBox( 4339 , MB_YESNO , 1 , this );
		}

		// ��ģ�ı����.
		else if( IsCmdControl("ID_BT_BROKEUPCANCEL") )
		{			
			GetItemTask().RequestBrokeupOrCancelBF( false );
		}
	
		// ��Ƽ��û.
		else if( IsCmdControl("ID_BT_PARTY") )
		{
			InvitePartyBF();
		}
		
		// �޸�.
		else if( IsCmdControl("ID_BT_MYMESSAGE") )
		{
			GetInterface().OpenBFMemo( true );
		}
	}


	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnBestFriendDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		// ��ģ������û.
		if( m_bAllowRequestBFInfo && IsShow() )
			RequestBFInfo();

		EnableCtrByGameTask();		
	}
	else
	{
		m_bAllowRequestBFInfo = true;
	}
	
}


void CDnBestFriendDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == 1 ) 
	{	
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 ) 
			{
				// ��ģ�ı�.
				GetItemTask().RequestBrokeupOrCancelBF( true );
			}
		}		
	}

}


// ��ģ����.
void CDnBestFriendDlg::SetBFData( struct TBestFriendInfo & bfInfo )
{
	wchar_t buf[16] = {0,};

	tm* Tm;
	Tm = _localtime64( &bfInfo.tStartDate );
	wchar_t str[128] = {0,};	
	swprintf_s( str, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4347 ), 
		Tm->tm_year+1900, Tm->tm_mon+1, Tm->tm_mday );
	m_pStaticDate->SetText( str ); 
	m_pStaticDay->SetText( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4348 ), bfInfo.nDay ) );
	m_pStaticName->SetText( bfInfo.wszName );
	m_pStaticLevel->SetText( _itow( bfInfo.cLevel, buf, 10 ) );
	m_pStaticJob->SetText( DN_INTERFACE::STRING::GetJobString( bfInfo.cJob ) );

	std::wstring strLocation;
	DN_INTERFACE::STRING::GetLocationText( strLocation, bfInfo.Location );
	m_pStaticLocation->SetText( strLocation );
	
	m_pStaticMemo->SetText( bfInfo.wszMyMemo );
	m_pStaticFriendMemo->SetText( bfInfo.wszFriendMemo );

	// ��ģDBID.
	m_biCharacterDBID = bfInfo.biCharacterDBID;

	// ��ģ����.
	if( bfInfo.bStatus == 0 )
	{
		m_bBFState = true;
		SetCtrlBFInfo( true );
		SetCtrlDestroyedBFInfo( false );		
	}

	// �ı����.
	else if( bfInfo.bStatus == 1 )
	{
		m_bBFState = false;
		SetCtrlBFInfo( false );
		SetCtrlDestroyedBFInfo( true );

		// �ı���Ұ��� ��ưó�� - "�ı��û"�� ������� "�ı����"�� �� �ִ�.
		m_bBFBrokeUpCancel = bfInfo.bCancel == 1 ? true : false;
		m_pBtnBrokeupCancel->Enable( m_bBFBrokeUpCancel );

		m_CountDown = (float)bfInfo.biWaitingTimeForDelete; // ��ģ�ıⳲ���ð�.
		m_timeOneMin = 60.0f;
	}

}

// ��ģ���¿����� ��Ʈ�����ó��.
void CDnBestFriendDlg::SetCtrlBFInfo( bool bShow )
{
	// ��ģ���� ��Ʈ�ѵ�.	
	m_pStaticTex0->Show( bShow );
	m_pStaticTex1->Show( bShow );
	m_pStaticTex6->Show( bShow );
	m_pStaticCond->Show( bShow );
	m_pStaticTex7->Show( bShow );
	m_pStaticTex8->Show( bShow );

	m_pStaticDate->Show( bShow );
	m_pStaticDay->Show( bShow );
	m_pStaticMemo->Show( bShow );
	m_pStaticFriendMemo->Show( bShow );
	m_pBtnBrokeup->Show( bShow );

	for( int i=0; i<(int)m_vStaticEnable.size(); ++i )
		m_vStaticEnable[ i ]->Enable( bShow );

	m_pBtnParty->Enable( bShow );
	m_pBtnMemo->Enable( bShow );
}


// �ı���� ��Ʈ�ѵ�.	
void CDnBestFriendDlg::SetCtrlDestroyedBFInfo( bool bShow )
{
	m_pStatiBrokeupTime->Show( bShow );
	m_pStaticBrokeupNotice->Show( bShow );
	m_pBtnBrokeupCancel->Show( bShow );
}

// �޸����.
void CDnBestFriendDlg::SetMemo( bool bFromMe, WCHAR * pStrMemo )
{
	// ���޸�.
	if (bFromMe)
		m_pStaticMemo->SetText( pStrMemo );
	else
		m_pStaticFriendMemo->SetText( pStrMemo );		
}

// ��Ƽ�ʴ�.
void CDnBestFriendDlg::InvitePartyBF()
{
	GetPartyTask().ReqInviteParty( m_pStaticName->GetText() );
}

// ��ģ�ı� or �ı����.
void CDnBestFriendDlg::BrokeupOrCancelBF( bool bCancel, WCHAR * strName )
{	
	// ��ģ�ı�.
	if( bCancel )
	{
		m_bBFState = false;
		SetCtrlBFInfo( false );
		SetCtrlDestroyedBFInfo( true );

		// ��ģ�ı�������� [�ı����] ��ư�� ��Ȱ��.
		m_bBFBrokeUpCancel = _wcsicmp( m_pStaticName->GetText(), strName ) == 0 ? false : true;
		m_pBtnBrokeupCancel->Enable( m_bBFBrokeUpCancel );

		m_CountDown = 86400 * 4 - 1; // ��ģ�ıⳲ���ð� - 4��.
		m_timeOneMin = 60.0f;
	}

	// �ı����.
	else
	{
		m_bBFState = true;
		SetCtrlBFInfo( true );
		SetCtrlDestroyedBFInfo( false );
	}
	
	
}


// GameTask �� ��� ��ģDlg �ϴ��� Button���� ��� ��Ȱ����Ű��,
// VillageTask �� ��� ���� ���·� ���� ��Ų��.
void CDnBestFriendDlg::EnableCtrByGameTask()
{
	// GameTask.
	CTask * pTask = CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pTask )
	{
		// ��Ƽ��û - ���������� ��Ƽ�����ΰ�쿡 Ȱ��ȭ ��Ŵ.
		if( CDnPartyTask::PartyRoleEnum::LEADER == GetPartyTask().GetPartyRole() ||
			CDnPartyTask::PartyRoleEnum::MEMBER == GetPartyTask().GetPartyRole() )
			m_pBtnParty->Enable( true );
		else
			m_pBtnParty->Enable( false );

		m_pBtnMemo->Enable( false ); //  �޸��Է�
		m_pBtnBrokeup->Enable( false ); //  ��ģ�ı�
		m_pBtnBrokeupAgreement->Enable( false ); //  ��ģ�ı⵿��
		m_pBtnBrokeupCancel->Enable( false ); //  �ı����
	}
	
	// VillageTask.
	else
	{
		pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pTask )
		{
			m_pBtnParty->Enable( m_bBFState );
			m_pBtnMemo->Enable( m_bBFState );
		
			m_pBtnBrokeup->Enable( true ); //  ��ģ�ı�
			m_pBtnBrokeupAgreement->Enable( true ); //  ��ģ�ı⵿��

			m_pBtnBrokeupCancel->Enable( m_bBFBrokeUpCancel );
		}	
	}

}


// ��ģ������û.
void CDnBestFriendDlg::RequestBFInfo()
{	
	GetItemTask().RequestBFInfo();	
	m_bAllowRequestBFInfo = false;
}


#endif