#include "stdafx.h"
#include "DnOccupationModeHUD.h"
#include "DnOccupationModeClimaxDlg.h"
#include "DnOccupationModeNoticeDlg.h"
#include "Timeset.h"
#include "DnTableDB.h"
#include "DnBridgeTask.h"
#include "DnPvPKillListDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnOccupationModeHUD::CDnOccupationModeHUD( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnPVPBaseHUD( dialogType, pParentDialog, nID, pCallback )
, m_pStaticClimaxString( NULL )
, m_pStaticClimaxBackground( NULL )
, m_pStaticClimaxMinute( NULL )
, m_pStaticClimaxSecond( NULL )
, m_pStaticClimaxMilliSecond( NULL )
, m_pStaticClimaxSemicolon1( NULL )
, m_pStaticClimaxSemicolon2( NULL )
, m_pStaticClimaxTimeBackground( NULL )
, m_pBlueBar( NULL )
, m_pRedBar( NULL )
, m_pNoticeDlg( NULL )
, m_pClimaxDlg( NULL )
, m_bClimaxModeStart( false )
, m_bClimaxModeEffect( false )
, m_nGoalScore( 0 )
, m_nClimaxTime( 0 )
#ifdef PRE_MOD_PVPOBSERVER		
, m_bFirstObserver( false )
#endif // PRE_MOD_PVPOBSERVER
{
}
CDnOccupationModeHUD::~CDnOccupationModeHUD()
{
	SAFE_DELETE( m_pNoticeDlg );
	SAFE_DELETE( m_pClimaxDlg );
}

void CDnOccupationModeHUD::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvPFlagDlg.ui" ).c_str(), bShow );
}

void CDnOccupationModeHUD::InitialUpdate()
{
	CDnPVPBaseHUD::InitialUpdate();

	CDnClimaxEffect::m_pBaseTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_TIMEBAR");
	CDnClimaxEffect::m_pScaleTexture = GetControl<CEtUITextureControl>("ID_TEXTUREL_TIMEBARAFTER1");

	CDnClimaxEffect::m_hClimaxTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_TimeBar.dds" ).c_str(), RT_TEXTURE );
	CDnClimaxEffect::m_pBaseTexture->SetTexture( m_hClimaxTexture );
	CDnClimaxEffect::m_pScaleTexture->SetTexture( m_hClimaxTexture );
	CDnClimaxEffect::m_pBaseTexture->Show( false );
	CDnClimaxEffect::m_pScaleTexture->Show( false );

	CDnClimaxEffect::Init( 0.f, 1.f, 1.5f, 1.5f, 0.0f, true );

	m_pRoomName = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ROOMNAME");
	m_pMYTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_MYTEAM");
	m_pEnemyTeam = GetControl<CEtUIStatic>("ID_PVP_RESPAWN_ENEMY");

#ifdef PRE_MOD_PVPOBSERVER	
	m_bFirstObserver = true;
#endif // PRE_MOD_PVPOBSERVER

	m_pStaticClimaxString = GetControl<CEtUIStatic>("ID_TEXT_NOTICE");
	m_pStaticClimaxBackground = GetControl<CEtUIStatic>("ID_STATIC_NOTICEBAR0");
	m_pStaticClimaxString->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120109 ) );	// 클라이막스 돌입! 리스폰 시간 감소 점수 획득 2배

	m_pStaticClimaxMinute = GetControl<CEtUIStatic>("ID_CLIMAXTIME_MINUTE");
	m_pStaticClimaxSecond = GetControl<CEtUIStatic>("ID_CLIMAXTIME_SECOND");
	m_pStaticClimaxMilliSecond = GetControl<CEtUIStatic>("ID_CLIMAXTIME_MILISECOND");
	m_pStaticClimaxSemicolon1 = GetControl<CEtUIStatic>("ID_CLIMAXTIME0");
	m_pStaticClimaxSemicolon2 = GetControl<CEtUIStatic>("ID_CLIMAXTIME1");
	m_pStaticClimaxTimeBackground = GetControl<CEtUIStatic>("ID_STATIC_TIMEBAR");

	m_pBlueBar = GetControl<CDnExpGauge>("ID_PROGRESSBAR_BLUE");
	m_pRedBar = GetControl<CDnExpGauge>("ID_PROGRESSBAR_RED");
	m_pBlueBar->UseGaugeEndElement( 0 );
	m_pRedBar->UseGaugeEndElement( 0 );
	m_pRedBar->SetFlipH();
	m_pBlueBar->SetProgress( 0.f );
	m_pRedBar->SetProgress( 0.f );

	m_pNoticeDlg = new CDnOccupationModeNoticeDlg;
	m_pNoticeDlg->Initialize( false );
	m_pClimaxDlg = new CDnOccupationModeClimaxDlg;
	m_pClimaxDlg->Initialize( false );

	m_nGoalScore = CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount;

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );

	int nBattleGroundID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "_BattleGroundID" )->GetInteger();;

	pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );
	if( !pSox ) return;

	m_nClimaxTime = pSox->GetFieldFromLablePtr(nBattleGroundID , "_ClimaxTime")->GetInteger() / 1000;

	StartClimaxMode( false );
}

void CDnOccupationModeHUD::Process( float fElapsedTime )
{
	CDnPVPBaseHUD::Process( fElapsedTime );

	ProcessScore();
	ProcessClimax( fElapsedTime );

	if( CDnClimaxEffect::IsShow() )
		CDnClimaxEffect::Process( fElapsedTime );

#ifdef PRE_MOD_PVPOBSERVER
	if( CEtUIDialog::IsShow() && m_bFirstObserver )
	{		
		if( SetTextObserverTeam( m_pMYTeam, m_pEnemyTeam, 7856, 7857 ) ) // "질서의 드래곤", "혼돈의 드래곤"	
			m_bFirstObserver = false;
	}
#endif // PRE_MOD_PVPOBSERVER

}

void CDnOccupationModeHUD::ProcessScore()
{
	float fBluePer = ((float)m_nMyTeam_Score / (float)m_nGoalScore ) * 100.0f;
	float fRedPer = ((float)m_nEnemyTeam_Score / (float)m_nGoalScore ) * 100.0f;

	m_pBlueBar->SetProgress( fBluePer );
	m_pRedBar->SetProgress( fRedPer );
}

void CDnOccupationModeHUD::ProcessClimax( float fElapsedTime )
{
	if( false == m_bClimaxModeStart && 0 != m_fRemainSec )
	{
		if( m_fRemainSec - CLIMAX_EFFECT_TIME < m_nClimaxTime && !m_bClimaxModeEffect )
			StartClimaxEffect();
	}
}

void CDnOccupationModeHUD::StartClimaxMode( bool bStart )
{
	m_pTimer_Min->Show( !bStart );
	m_pTimer_Sec->Show( !bStart );
	m_pTimer_Semi->Show( !bStart );

	m_pStaticClimaxString->Show( bStart );
	m_pStaticClimaxBackground->Show( bStart );

	m_pStaticClimaxMinute->Show( bStart );
	m_pStaticClimaxSecond->Show( bStart );
	m_pStaticClimaxMilliSecond->Show( bStart );
	m_pStaticClimaxSemicolon1->Show( bStart );
	m_pStaticClimaxSemicolon2->Show( bStart );
	m_pStaticClimaxTimeBackground->Show( bStart );

	m_bClimaxModeStart = bStart;

	if( bStart )
		CDnClimaxEffect::Start();
}

void CDnOccupationModeHUD::StartClimaxEffect()
{
	m_pClimaxDlg->StartClimaxEffect();
	m_bClimaxModeEffect = true;
}

void CDnOccupationModeHUD::UpdateTimer()
{
	CDnPVPBaseHUD::UpdateTimer();

	m_pStaticClimaxMinute->SetText( m_pTimer_Min->GetText() );
	m_pStaticClimaxSecond->SetText( m_pTimer_Sec->GetText() );

	WCHAR wszSec[10];
	int nMSec = 100 - (CTimeSet().GetMilliseconds() / 10);
	wsprintf(wszSec,L"%.2d",nMSec);
	m_pStaticClimaxMilliSecond->SetText( wszSec );

	if( 0 >= m_fRemainSec )
		m_pStaticClimaxMilliSecond->SetText( L"00" );
}


#ifdef PRE_MOD_PVPOBSERVER

void CDnOccupationModeHUD::ShowOccupationInfo( DnActorHandle hActor, std::wstring & wszFlagName, int nBlueStringID, int nRedStringID, int nObserverBlueStringID, int nObserverRedStringID )
{
	if( !m_pKillListDlg ) return;
	m_pKillListDlg->AddOccupationInfo( hActor, wszFlagName );

	if( !CDnActor::s_hLocalActor || !hActor->IsPlayerActor()) return;

	int eTeam = 0;
	int nStringID = 0;

	// Observer 인 경우.
	if(CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
	{
		eTeam = hActor->GetTeam();		
		nStringID = ( eTeam == PvPCommon::Team::A ) ? nObserverBlueStringID : nObserverRedStringID;		
	}	

	// PVP 참여 유저인 경우.
	else
	{
		eTeam = ( CDnActor::s_hLocalActor->GetTeam() == hActor->GetTeam() ) ? PvPCommon::Team::A : PvPCommon::Team::B;
		nStringID = ( CDnActor::s_hLocalActor->GetTeam() == hActor->GetTeam() ) ? nBlueStringID : nRedStringID;
	}

	m_pNoticeDlg->ShowNotice( eTeam, nStringID );
}

#else

void CDnOccupationModeHUD::ShowOccupationInfo( DnActorHandle hActor, std::wstring & wszFlagName, int nBlueStringID, int nRedStringID )
{
	if( !m_pKillListDlg ) return;
	m_pKillListDlg->AddOccupationInfo( hActor, wszFlagName );

	if( !CDnActor::s_hLocalActor || !hActor->IsPlayerActor()) return;

	if(CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
		return;

	int eTeam = ( CDnActor::s_hLocalActor->GetTeam() == hActor->GetTeam() ) ? PvPCommon::Team::A : PvPCommon::Team::B;
	int nStringID = ( CDnActor::s_hLocalActor->GetTeam() == hActor->GetTeam() ) ? nBlueStringID : nRedStringID;

	m_pNoticeDlg->ShowNotice( eTeam, nStringID );
}
#endif // PRE_MOD_PVPOBSERVER

