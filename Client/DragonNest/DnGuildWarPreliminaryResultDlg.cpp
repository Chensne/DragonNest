#include "StdAfx.h"
#include "DnGuildWarPreliminaryResultDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarTask.h"
#include "DnGuildTask.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryResultDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryResultDlg::CDnGuildWarPreliminaryResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pDnGuildWarPreliminaryGuildFestivalResultDlg( NULL )
, m_pDnGuildWarPreliminaryMyResultDlg( NULL )
, m_pDnGuildWarPreliminaryGuildFinalsDlg( NULL )
, m_pDnGuildWarPreliminaryGuildRankingDlg( NULL )
, m_pDnGuildWarPreliminaryPersonalRankingDlg( NULL )
, m_pDnGuildWarPreliminarySpecialRankDlg( NULL )
, m_pButtonGuildFestivalResult( NULL )
, m_pButtonMyResult( NULL )
, m_pButtonGuildFinals( NULL )
, m_pButtonGuildRanking( NULL )
, m_pButtonIndividualRanking( NULL )
, m_pButtonSpecialRank( NULL )
{
}

CDnGuildWarPreliminaryResultDlg::~CDnGuildWarPreliminaryResultDlg()
{
}

void CDnGuildWarPreliminaryResultDlg::InitialUpdate()
{
	CEtUITabDialog::InitialUpdate();

	m_pButtonGuildFestivalResult = GetControl<CEtUIRadioButton>( "ID_RBT_TAB0" );
	m_pDnGuildWarPreliminaryGuildFestivalResultDlg = new CDnGuildWarPreliminaryGuildFestivalResultDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminaryGuildFestivalResultDlg->Initialize( false );
	AddTabDialog( m_pButtonGuildFestivalResult, m_pDnGuildWarPreliminaryGuildFestivalResultDlg );

	m_pButtonMyResult = GetControl<CEtUIRadioButton>( "ID_RBT_TAB1" );
	m_pDnGuildWarPreliminaryMyResultDlg = new CDnGuildWarPreliminaryMyResultDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminaryMyResultDlg->Initialize( false );
	AddTabDialog( m_pButtonMyResult, m_pDnGuildWarPreliminaryMyResultDlg );

	m_pButtonGuildFinals = GetControl<CEtUIRadioButton>( "ID_RBT_TAB2" );
	m_pDnGuildWarPreliminaryGuildFinalsDlg = new CDnGuildWarPreliminaryGuildRankDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminaryGuildFinalsDlg->Initialize( false );
	AddTabDialog( m_pButtonGuildFinals, m_pDnGuildWarPreliminaryGuildFinalsDlg );

	m_pButtonGuildRanking = GetControl<CEtUIRadioButton>( "ID_RBT_TAB3" );
	m_pDnGuildWarPreliminaryGuildRankingDlg = new CDnGuildWarPreliminaryGuildEventRankingDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminaryGuildRankingDlg->Initialize( false );
	AddTabDialog( m_pButtonGuildRanking, m_pDnGuildWarPreliminaryGuildRankingDlg );

	m_pButtonIndividualRanking = GetControl<CEtUIRadioButton>( "ID_RBT_TAB4" );
	m_pDnGuildWarPreliminaryPersonalRankingDlg = new CDnGuildWarPreliminaryPersonalEventRankingDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminaryPersonalRankingDlg->Initialize( false );
	AddTabDialog( m_pButtonIndividualRanking, m_pDnGuildWarPreliminaryPersonalRankingDlg );

	m_pButtonSpecialRank = GetControl<CEtUIRadioButton>( "ID_RBT_TAB5" );
	m_pDnGuildWarPreliminarySpecialRankDlg = new CDnGuildWarPreliminarySpecialRankDlg( UI_TYPE_CHILD, this );
	m_pDnGuildWarPreliminarySpecialRankDlg->Initialize( false );
	AddTabDialog( m_pButtonSpecialRank, m_pDnGuildWarPreliminarySpecialRankDlg );
}

void CDnGuildWarPreliminaryResultDlg::Initialize( bool bShow )
{
	CEtUITabDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarTabDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryResultDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

		if( hNpc )
			hNpc->SetActionQueue( "Close" );

		GetInterface().CloseBlind();
		//GetInterface().CloseNpcTalkReturnDlg();
	}

	CEtUITabDialog::Show( bShow );
}

void CDnGuildWarPreliminaryResultDlg::SetGuildWarPreliminaryResultDlg()
{
	SetCheckedTab( m_pButtonGuildFestivalResult->GetTabID() );
	
	// 예선, 본선에 따라 보여지는 탭이 다름
	if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_TRIAL )			// 예선
	{
		m_pButtonGuildFestivalResult->Enable( true );
		m_pButtonMyResult->Enable( false );
		if( GetGuildWarTask().GetGuildWarFinalProgress() )
			m_pButtonGuildFinals->Enable( true );
		else
			m_pButtonGuildFinals->Enable( false );

		m_pButtonGuildRanking->Enable( false );
		m_pButtonIndividualRanking->Enable( false );
		m_pButtonSpecialRank->Enable( false );

		m_pButtonGuildFestivalResult->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126302 ) );
		m_pButtonGuildFinals->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126303 ) );
	}
	else if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_REWARD )		// 본선
	{
		m_pButtonGuildFestivalResult->Enable( true );
		if( GetGuildWarTask().GetPreliminaryResult().bJoinGuildWar )
			m_pButtonMyResult->Enable( true );
		else
			m_pButtonMyResult->Enable( false );			
		m_pButtonGuildFinals->Enable( true );
		m_pButtonGuildRanking->Enable( true );
		m_pButtonIndividualRanking->Enable( true );
		m_pButtonSpecialRank->Enable( true );

		m_pButtonGuildFestivalResult->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126069 ) );
		m_pButtonGuildFinals->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126073 ) );
	}
}

void CDnGuildWarPreliminaryResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );
	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryGuildFestivalResultDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryGuildFestivalResultDlg::CDnGuildWarPreliminaryGuildFestivalResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticBlueTeamWin( NULL )
, m_pStaticRedTeamWin( NULL )
, m_pStaticBlueTeamScore( NULL )
, m_pStaticRedTeamScore( NULL )
, m_pStaticMyScore( NULL )
, m_pStaticMyGuildScore( NULL )
{
	memset( m_pStaticInterimResult, 0, sizeof( m_pStaticInterimResult ) );
}

CDnGuildWarPreliminaryGuildFestivalResultDlg::~CDnGuildWarPreliminaryGuildFestivalResultDlg()
{
}

void CDnGuildWarPreliminaryGuildFestivalResultDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pStaticBlueTeamWin = GetControl<CEtUIStatic>( "ID_TEXT_RESULT0" );
	m_pStaticBlueTeamWin->Show( false );
	m_pStaticRedTeamWin = GetControl<CEtUIStatic>( "ID_TEXT_RESULT1" );
	m_pStaticRedTeamWin->Show( false );
	m_pStaticBlueTeamScore = GetControl<CEtUIStatic>( "ID_TEXT_BLUECOUNT" );
	m_pStaticRedTeamScore = GetControl<CEtUIStatic>( "ID_TEXT_REDCOUNT" );
	m_pStaticInterimResult[0] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT2" );
	m_pStaticInterimResult[1] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT3" );
	m_pStaticInterimResult[2] = GetControl<CEtUIStatic>( "ID_TEXT_RESULT4" );
	m_pStaticInterimResult[0]->Show( false );
	m_pStaticInterimResult[1]->Show( false );
	m_pStaticInterimResult[2]->Show( false );
	m_pStaticMyScore = GetControl<CEtUIStatic>( "ID_TEXT_MYCOUNT" );
	m_pStaticMyGuildScore = GetControl<CEtUIStatic>( "ID_TEXT_GUILDCOUNT" );
}

void CDnGuildWarPreliminaryGuildFestivalResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarResultDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryGuildFestivalResultDlg::Show( bool bShow )
{
	if( m_bShow == bShow )       
		return;
	
	if( bShow )
	{
		if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_TRIAL )			// 예선
		{
			CDnGuildWarTask::stPreliminaryStatus stPreliminaryStatus = GetGuildWarTask().GetPreliminaryStatus();
			SetFestivalResult( stPreliminaryStatus.nBluePoint, stPreliminaryStatus.nRedPoint, stPreliminaryStatus.nMyPoint, stPreliminaryStatus.nMyGuildPoint );
		}
		else if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_REWARD )		// 본선
			SetFestivalResult( GetGuildWarTask().GetPreliminaryResult().nBlueFinalPoint, GetGuildWarTask().GetPreliminaryResult().nRedFinalPoint );
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryGuildFestivalResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryGuildFestivalResultDlg::SetFestivalResult( int nBlueTeamSocre, int nRedTeamScore, int nMySocre, int nMyGuildScore )
{
	if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_TRIAL )			// 예선
	{
		CEtUIStatic* pTitle = GetControl<CEtUIStatic>( "ID_TEXT0" );
		pTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126046 ) );

		if( nBlueTeamSocre > nRedTeamScore )
		{
			m_pStaticInterimResult[0]->Show( true );
			m_pStaticInterimResult[1]->Show( false );
			m_pStaticInterimResult[2]->Show( false );
		}
		else if( nBlueTeamSocre == nRedTeamScore )
		{
			m_pStaticInterimResult[0]->Show( false );
			m_pStaticInterimResult[1]->Show( false );
			m_pStaticInterimResult[2]->Show( true );
		}
		else
		{
			m_pStaticInterimResult[0]->Show( false );
			m_pStaticInterimResult[1]->Show( true );
			m_pStaticInterimResult[2]->Show( false );
		}

		CEtUIStatic* pStaticText = GetControl<CEtUIStatic>( "ID_TEXT1" );
		pStaticText->Show( true );
		pStaticText = GetControl<CEtUIStatic>( "ID_TEXT2" );
		pStaticText->Show( true );
		pStaticText = GetControl<CEtUIStatic>( "ID_TEXT3" );
		pStaticText->Show( true );
		m_pStaticMyScore->Show( true );
		m_pStaticMyGuildScore->Show( true );
		m_pStaticMyScore->SetText( FormatW( L"%d", nMySocre ) );
		m_pStaticMyGuildScore->SetText( FormatW( L"%d", nMyGuildScore ) );
	}
	else if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_REWARD )		// 본선
	{
		CEtUIStatic* pTitle = GetControl<CEtUIStatic>( "ID_TEXT0" );
		pTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126069 ) );

		m_pStaticInterimResult[0]->Show( false );
		m_pStaticInterimResult[1]->Show( false );
		m_pStaticInterimResult[2]->Show( false );

		if( nBlueTeamSocre > nRedTeamScore )
		{
			m_pStaticBlueTeamWin->Show( true );
			m_pStaticRedTeamWin->Show( false );
		}
		else
		{
			m_pStaticBlueTeamWin->Show( false );
			m_pStaticRedTeamWin->Show( true );
		}

		CEtUIStatic* pStaticText = GetControl<CEtUIStatic>( "ID_TEXT1" );
		pStaticText->Show( false );
		pStaticText = GetControl<CEtUIStatic>( "ID_TEXT2" );
		pStaticText->Show( false );
		pStaticText = GetControl<CEtUIStatic>( "ID_TEXT3" );
		pStaticText->Show( false );
		m_pStaticMyScore->Show( false );
		m_pStaticMyGuildScore->Show( false );
	}

	m_pStaticBlueTeamScore->SetText( FormatW( L"%d", nBlueTeamSocre ) );
	m_pStaticRedTeamScore->SetText( FormatW( L"%d", nRedTeamScore ) );
}

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryMyResultDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryMyResultDlg::CDnGuildWarPreliminaryMyResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticResultWin( NULL )
, m_pStaticResultLose( NULL )
, m_pStaticWin( NULL )
, m_pStaticLose( NULL )
, m_pStaticPointCount( NULL )
, m_pStaticSpecial( NULL )
, m_pButtonReward( NULL )
{
}

CDnGuildWarPreliminaryMyResultDlg::~CDnGuildWarPreliminaryMyResultDlg()
{
}

void CDnGuildWarPreliminaryMyResultDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pStaticResultWin = GetControl<CEtUIStatic>( "ID_TEXT_RESULT0" );
	m_pStaticResultWin->Show( false );
	m_pStaticResultLose = GetControl<CEtUIStatic>( "ID_TEXT_RESULT1" );
	m_pStaticResultLose->Show( false );
	m_pStaticWin = GetControl<CEtUIStatic>( "ID_TEXT_WIN" );
	m_pStaticWin->Show( false );
	m_pStaticLose = GetControl<CEtUIStatic>( "ID_TEXT_LOSE" );
	m_pStaticLose->Show( false );
	m_pStaticPointCount = GetControl<CEtUIStatic>( "ID_TEXT_POINTCOUNT" );
	m_pStaticSpecial = GetControl<CEtUIStatic>( "ID_TEXT_SPECIAL" );
	m_pButtonReward = GetControl<CEtUIButton>( "ID_BT_REWARD" );
}

void CDnGuildWarPreliminaryMyResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarMyResultDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryMyResultDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetMyResult();

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryMyResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
		else if( IsCmdControl( "ID_BT_REWARD" ) )
		{
			CClientSessionManager::GetInstance().SendPacket( CS_GUILD, eGuild::eCSGuild::CS_GUILDWAR_COMPENSATION, NULL, 0 );
			m_pButtonReward->Enable( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryMyResultDlg::SetMyResult()
{
	if( GetGuildWarTask().GetPreliminaryResult().bMyWin )
	{
		m_pStaticResultWin->Show( true );
		m_pStaticWin->Show( true );
		m_pStaticResultLose->Show( false );
		m_pStaticLose->Show( false );
	}
	else
	{
		m_pStaticResultWin->Show( false );
		m_pStaticWin->Show( false );
		m_pStaticResultLose->Show( true );
		m_pStaticLose->Show( true );
	}

	wchar_t szTemp[128]={0};
	if( GetGuildWarTask().GetPreliminaryResult().bReward )
		swprintf_s( szTemp, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625 ), 1 );
	else
		swprintf_s( szTemp, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625 ), 0 );

	CEtUIStatic* pStatic = GetControl<CEtUIStatic>( "ID_TEXT_REWARD0" );
	if( pStatic )
		pStatic->SetText( szTemp );

	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_REWARD1" );
	if( pStatic )
		pStatic->SetText( szTemp );

	CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayerActor == NULL )
		return;

	int nSpecialPrizeCount = 0;
	std::vector<CDnGuildWarTask::stEventRankInfo>& pVecEventRankInfo = GetGuildWarTask().GetEventPersonalRankInfo();
	std::vector<CDnGuildWarTask::stEventRankInfo>::iterator iter = pVecEventRankInfo.begin();

	for( int i=0; iter != pVecEventRankInfo.end() && i < MAX_PRIZE_COUNT; iter++, i++ )
	{
		if( _tcscmp( pLocalPlayerActor->GetName(), (*iter).m_strName1.c_str() ) == 0 )
			nSpecialPrizeCount++;
	}

	std::vector<tstring>& pVecDailyBestChar = GetGuildWarTask().GetDailyBestChar();
	std::vector<tstring>::iterator iter2 = pVecDailyBestChar.begin();

	for( int i=0; iter2 != pVecDailyBestChar.end() && i < MAX_DAILY_PRIZE_COUNT; iter2++, i++ )
	{
		if( _tcscmp( pLocalPlayerActor->GetName(), (*iter2).c_str() ) == 0 )
			nSpecialPrizeCount++;
	}

	if( nSpecialPrizeCount > 0 )
	{
		swprintf_s( szTemp, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126242 ), nSpecialPrizeCount );
		m_pStaticSpecial->SetText( szTemp );
	}
	else
		m_pStaticSpecial->SetText( L"" );

	int nTotalFestivalPoint = GetGuildWarTask().GetPreliminaryResult().nFestivalPoint + GetGuildWarTask().GetPreliminaryResult().nFestivalPointAdd;
	swprintf_s( szTemp, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3791 ), nTotalFestivalPoint );

	if( GetGuildWarTask().GetPreliminaryResult().nFestivalPointAdd > 0 )
		m_pStaticPointCount->SetText( FormatW( L"%s ( %d + %d )", szTemp, GetGuildWarTask().GetPreliminaryResult().nFestivalPoint, GetGuildWarTask().GetPreliminaryResult().nFestivalPointAdd ) );
	else
		m_pStaticPointCount->SetText( szTemp );

	if( GetGuildWarTask().GetPreliminaryResult().bReward )
		m_pButtonReward->Enable( true );
	else
		m_pButtonReward->Enable( false );
}


//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryGuildRankDlgItem
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryGuildRankDlgItem::CDnGuildWarPreliminaryGuildRankDlgItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnGuildWarPreliminaryGuildRankDlgItem::~CDnGuildWarPreliminaryGuildRankDlgItem()
{
}

void CDnGuildWarPreliminaryGuildRankDlgItem::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarGuildRankListDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryGuildRankDlgItem::SetPreliminaryGuildRankInfo( CDnGuildWarTask::stJoinGuildRankInfo* pInfo, bool bBlind )
{
	if( pInfo == NULL )
		return;

	CEtUIStatic* pStatic;
	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_LISTCOUNT0" );
	if( bBlind )
		pStatic->SetText( L"" );
	else
		pStatic->SetText( FormatW( L"%d", pInfo->m_nGuildRank ) );

	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_GUILDNAME0" );
	pStatic->SetText( pInfo->m_strGuildName );
	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_GUILDMASTER0" );
	pStatic->SetText( pInfo->m_strGuildMasterName );
	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_GUILDMEMBER0" );
	pStatic->SetText( FormatW( L"%d/%d", pInfo->m_nGuildUserCount, pInfo->m_nGuildUserMax ) );

	pStatic = GetControl<CEtUIStatic>( "ID_TEXT_GUILDCOUNT0" );
	if( bBlind )
		pStatic->SetText( L"" );
	else
		pStatic->SetText( FormatW( L"%d", pInfo->m_nTotalPoints ) );
}


//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryGuildRankDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryGuildRankDlg::CDnGuildWarPreliminaryGuildRankDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticResultWin( NULL )
, m_pStaticResultLose( NULL )
, m_pGuildRankListBox( NULL )
, m_pStaticMyGuildRanking( NULL )
, m_pStaticMyGuildName( NULL )
, m_pStaticMyGuildMasterName( NULL )
, m_pStaticMyGuildUserCount( NULL )
, m_pStaticMyGuildTotalPoints( NULL )
{
}

CDnGuildWarPreliminaryGuildRankDlg::~CDnGuildWarPreliminaryGuildRankDlg()
{
}

void CDnGuildWarPreliminaryGuildRankDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pStaticResultWin = GetControl<CEtUIStatic>( "ID_TEXT_RESULT0" );
	m_pStaticResultWin->Show( false );
	m_pStaticResultLose = GetControl<CEtUIStatic>( "ID_TEXT_RESULT1" );
	m_pStaticResultLose->Show( false );

	m_pGuildRankListBox = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_LIST" );

	m_pStaticMyGuildRanking = GetControl<CEtUIStatic>( "ID_TEXT_LISTCOUNT" );
	m_pStaticMyGuildName = GetControl<CEtUIStatic>( "ID_TEXT_GUILDNAME" );
	m_pStaticMyGuildMasterName = GetControl<CEtUIStatic>( "ID_TEXT_GUILDMASTER" );
	m_pStaticMyGuildUserCount = GetControl<CEtUIStatic>( "ID_TEXT_GUILDMEMBER" );
	m_pStaticMyGuildTotalPoints = GetControl<CEtUIStatic>( "ID_TEXT_GUILDCOUNT" );
}

void CDnGuildWarPreliminaryGuildRankDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarGuildRankDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryGuildRankDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetPageUI();

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryGuildRankDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryGuildRankDlg::SetPageUI()
{
	ClearPageUI();

	if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_TRIAL )		// 예선
		SetPagePreliminaryUI();
	else if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_REWARD )	// 본선
		SetPageFinalUI();
}

void CDnGuildWarPreliminaryGuildRankDlg::ClearPageUI()
{
	m_pStaticResultWin->Show( false );
	m_pStaticResultLose->Show( false );
	m_pGuildRankListBox->RemoveAllItems();
	m_pStaticMyGuildRanking->SetText( L"" );
	m_pStaticMyGuildName->SetText( L"" );
	m_pStaticMyGuildMasterName->SetText( L"" );
	m_pStaticMyGuildUserCount->SetText( L"" );
	m_pStaticMyGuildTotalPoints->SetText( L"" );
}

bool SortByGuildName( CDnGuildWarTask::stJoinGuildRankInfo pInfoA, CDnGuildWarTask::stJoinGuildRankInfo pInfoB )
{
	return (0 > wcscmp( pInfoA.m_strGuildName.c_str(), pInfoB.m_strGuildName.c_str() ) );
}

void CDnGuildWarPreliminaryGuildRankDlg::SetPagePreliminaryUI()
{
	m_pStaticResultWin->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126304 ) );
	m_pStaticResultWin->Show( true );

	std::vector<CDnGuildWarTask::stJoinGuildRankInfo>& pVecPreliminaryGuildRanking = GetGuildWarTask().GetPreliminaryResultGuildRanking();

	if( pVecPreliminaryGuildRanking.size() == 0 )
		return;

	bool bBlind = false;
	if( (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::GuildWar_Rank_Blind ) == 1 )
		bBlind = true;

	if( bBlind )	// Blind이면 길드명 순으로 정렬
	{
		std::sort( pVecPreliminaryGuildRanking.begin(), pVecPreliminaryGuildRanking.end(), SortByGuildName );
	}

	// 1~N위 길드 정보
	std::vector<CDnGuildWarTask::stJoinGuildRankInfo>::iterator iter = pVecPreliminaryGuildRanking.begin();
	for( ; iter != pVecPreliminaryGuildRanking.end(); iter++ )
	{
		if( (*iter).m_nGuildRank > 0 )	// GuildRank가 0이면 빈 데이타로 간주 뺀다. (16강 미달시)
		{
			CDnGuildWarPreliminaryGuildRankDlgItem* pItem = m_pGuildRankListBox->AddItem<CDnGuildWarPreliminaryGuildRankDlgItem>();
			if( pItem )
				pItem->SetPreliminaryGuildRankInfo( &(*iter), bBlind );
		}
	}
}

void CDnGuildWarPreliminaryGuildRankDlg::SetPageFinalUI()
{
	m_pStaticResultWin->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126092 ) );

	std::vector<CDnGuildWarTask::stJoinGuildRankInfo>& pVecPreliminaryGuildRanking = GetGuildWarTask().GetPreliminaryResultGuildRanking();

	if( pVecPreliminaryGuildRanking.size() == 0 )
		return;

	std::vector<CDnGuildWarTask::stJoinGuildRankInfo>::iterator iter = pVecPreliminaryGuildRanking.begin();

	// 내 길드 순위 정보
	if( GetGuildWarTask().GetPreliminaryResult().bJoinGuildWar )
	{
		if( (*iter).m_nGuildRank > 0 && (*iter).m_nGuildRank <= 16 )
		{
			m_pStaticResultWin->Show( true );
			m_pStaticResultLose->Show( false );
		}
		else
		{
			m_pStaticResultWin->Show( false );
			m_pStaticResultLose->Show( true );
		}

		m_pStaticMyGuildRanking->SetText( FormatW( L"%d", (*iter).m_nGuildRank ) );
		m_pStaticMyGuildName->SetText( (*iter).m_strGuildName );
		m_pStaticMyGuildMasterName->SetText( (*iter).m_strGuildMasterName );
		m_pStaticMyGuildUserCount->SetText( FormatW( L"%d/%d", (*iter).m_nGuildUserCount, (*iter).m_nGuildUserMax ) );
		m_pStaticMyGuildTotalPoints->SetText( FormatW( L"%d", (*iter).m_nTotalPoints ) );

	}
	iter++;
	// 1~N위 길드 정보
	for( ; iter != pVecPreliminaryGuildRanking.end(); iter++ )
	{
		if( (*iter).m_nGuildRank > 0 )	// GuildRank가 0이면 빈 데이타로 간주 뺀다. (16강 미달시)
		{
			CDnGuildWarPreliminaryGuildRankDlgItem* pItem = m_pGuildRankListBox->AddItem<CDnGuildWarPreliminaryGuildRankDlgItem>();
			if( pItem )
				pItem->SetPreliminaryGuildRankInfo( &(*iter) );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryGuildEventRankingDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryGuildEventRankingDlg::CDnGuildWarPreliminaryGuildEventRankingDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	memset(m_UIGuildWarPreliminaryEventRankInfo, 0, sizeof(m_UIGuildWarPreliminaryEventRankInfo));
}

CDnGuildWarPreliminaryGuildEventRankingDlg::~CDnGuildWarPreliminaryGuildEventRankingDlg()
{
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	char szStr[64];
	for( int i=0; i<MAX_EVENT_LIST; i++ )
	{
		sprintf_s( szStr, "ID_TEXT_LISTCOUNT%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_GUILDNAME%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1 = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_GUILDMASTER%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2 = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_GUILDCOUNT%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints = GetControl<CEtUIStatic>( szStr );
	}
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarGuildEventRankDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetPageUI();

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::SetPageUI()
{
	ClearPageUI();

	std::vector<CDnGuildWarTask::stEventRankInfo>& pVecEventRankInfo = GetGuildWarTask().GetEventGuildRankInfo();

	if( pVecEventRankInfo.size() == 0 )
		return;

	std::vector<CDnGuildWarTask::stEventRankInfo>::iterator iter = pVecEventRankInfo.begin();

	for( int i=0; iter != pVecEventRankInfo.end() && i < MAX_EVENT_LIST; iter++, i++ )
	{
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank->SetText( FormatW( L"%d", (*iter).m_nRank ) );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1->SetText( (*iter).m_strName1 );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2->SetText( (*iter).m_strName2 );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints->SetText( FormatW( L"%d", (*iter).m_nTotalPoints ) );
	}
}

void CDnGuildWarPreliminaryGuildEventRankingDlg::ClearPageUI()
{
	for( int i=0; i<MAX_EVENT_LIST; i++ )
	{
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints->SetText( L"" );
	}
}


//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminaryPersonalEventRankingDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminaryPersonalEventRankingDlg::CDnGuildWarPreliminaryPersonalEventRankingDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	memset(m_UIGuildWarPreliminaryEventRankInfo, 0, sizeof(m_UIGuildWarPreliminaryEventRankInfo));
}

CDnGuildWarPreliminaryPersonalEventRankingDlg::~CDnGuildWarPreliminaryPersonalEventRankingDlg()
{
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	char szStr[64];
	for( int i=0; i<MAX_EVENT_LIST; i++ )
	{
		sprintf_s( szStr, "ID_TEXT_LISTCOUNT%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_NAME%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1 = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_GUILDNAME%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2 = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_COUNT%d", i );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints = GetControl<CEtUIStatic>( szStr );
	}
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarPersonalEventRankDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetPageUI();

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::SetPageUI()
{
	ClearPageUI();

	std::vector<CDnGuildWarTask::stEventRankInfo>& pVecEventRankInfo = GetGuildWarTask().GetEventPersonalRankInfo();

	if( pVecEventRankInfo.size() == 0 )
		return;

	std::vector<CDnGuildWarTask::stEventRankInfo>::iterator iter = pVecEventRankInfo.begin();

	for( int i=0; iter != pVecEventRankInfo.end() && i < MAX_EVENT_LIST; iter++, i++ )
	{
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank->SetText( FormatW( L"%d", (*iter).m_nRank ) );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1->SetText( (*iter).m_strName1 );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2->SetText( (*iter).m_strName2 );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints->SetText( FormatW( L"%d", (*iter).m_nTotalPoints ) );
	}
}

void CDnGuildWarPreliminaryPersonalEventRankingDlg::ClearPageUI()
{
	for( int i=0; i<MAX_EVENT_LIST; i++ )
	{
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticRank->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName1->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticName2->SetText( L"" );
		m_UIGuildWarPreliminaryEventRankInfo[i].m_pStaticPoints->SetText( L"" );
	}
}


//////////////////////////////////////////////////////////////////////////
// CDnGuildWarPreliminarySpecialRankDlg
//////////////////////////////////////////////////////////////////////////

CDnGuildWarPreliminarySpecialRankDlg::CDnGuildWarPreliminarySpecialRankDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	memset( m_pPrize, 0, sizeof( m_pPrize ) );
	memset( m_pDailyPrize, 0, sizeof( m_pDailyPrize ) );
}

CDnGuildWarPreliminarySpecialRankDlg::~CDnGuildWarPreliminarySpecialRankDlg()
{
}

void CDnGuildWarPreliminarySpecialRankDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	char szStr[64];
	for( int i=0; i<MAX_PRIZE_COUNT; i++ )
	{
		sprintf_s( szStr, "ID_TEXT_PRIZE%d", i );
		m_pPrize[i] = GetControl<CEtUIStatic>( szStr );
	}

	for( int i=0; i<MAX_DAILY_PRIZE_COUNT; i++ )
	{
		sprintf_s( szStr, "ID_TEXT_DAILYPRIZE%d", i );
		m_pDailyPrize[i] = GetControl<CEtUIStatic>( szStr );
	}
}

void CDnGuildWarPreliminarySpecialRankDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildWarGuildSpecialRankDlg.ui" ).c_str(), bShow );
}

void CDnGuildWarPreliminarySpecialRankDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
		SetPageUI();

	CEtUIDialog::Show( bShow );
}

void CDnGuildWarPreliminarySpecialRankDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) )
		{
			CDnGuildWarPreliminaryResultDlg* pDnGuildWarPreliminaryResult = dynamic_cast<CDnGuildWarPreliminaryResultDlg*>( GetParentDialog() );
			if( pDnGuildWarPreliminaryResult )
				pDnGuildWarPreliminaryResult->Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildWarPreliminarySpecialRankDlg::SetPageUI()
{
	ClearPageUI();

	std::vector<CDnGuildWarTask::stEventRankInfo>& pVecEventRankInfo = GetGuildWarTask().GetEventPersonalRankInfo();
	std::vector<CDnGuildWarTask::stEventRankInfo>::iterator iter = pVecEventRankInfo.begin();

	for( int i=0; iter != pVecEventRankInfo.end() && i < MAX_PRIZE_COUNT; iter++, i++ )
		m_pPrize[i]->SetText( (*iter).m_strName1 );

	std::vector<tstring>& pVecDailyBestChar = GetGuildWarTask().GetDailyBestChar();
	std::vector<tstring>::iterator iter2 = pVecDailyBestChar.begin();

	for( int i=0; iter2 != pVecDailyBestChar.end() && i < MAX_DAILY_PRIZE_COUNT; iter2++, i++ )
		m_pDailyPrize[i]->SetText( (*iter2) );
}

void CDnGuildWarPreliminarySpecialRankDlg::ClearPageUI()
{
	for( int i=0; i<MAX_PRIZE_COUNT; i++ )
		m_pPrize[i]->SetText( L"" );

	for( int i=0; i<MAX_DAILY_PRIZE_COUNT; i++ )
		m_pDailyPrize[i]->SetText( L"" );
}

