
#include "stdafx.h"
#include "PvPGameMode.h"
#include "DnActor.h"
#include "DnPlayerActor.h"
#include "DnMonsterActor.h"
#include "DNGameRoom.h"
#include "TaskManager.h"
#include "DnPvPGameTask.h"
#include "DNUserSession.h"
#include "PvPScoreSystem.h"
#include "PvPGameModeTimeOverChecker.h"
#include "PvPGameModeTeamMemberChecker.h"
#include "PvPGameModeNoMoreBreakIntoChecker.h"
#include "PvPGameModeZombieChecker.h"
#include "DNMasterConnectionManager.h"
#include "DNLogConnection.h"
#include "DNMissionSystem.h"
#include "DNPvPGameRoom.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"
#include "PvPCaptainScoreSystem.h"
#include "PvPZombieScoreSystem.h"
#include "PvPGameModeLadderNoGameChecker.h"
#include "PvPGuildWarScoreSystem.h"
#include "PvPOccupationScoreSystem.h"
#include "PvPZombieMode.h"
#if defined(PRE_ADD_REVENGE)
#include "RevengeSystem.h"
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
#include "PvPComboExerciseScoreSystem.h"
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
#if defined(PRE_ADD_DWC)
#include "LadderStats.h"
#endif

CPvPGameMode::CPvPGameMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:m_pGameRoom(pGameRoom),m_pScoreSystem(NULL),m_uiWinTeam(PvPCommon::Team::Max),m_FinishReason(PvPCommon::FinishReason::Max),m_bStartGameModeFlag(false),m_bFinishGameModeFlag(false)
,m_uiCurRound(1),m_pPvPGameModeTable(pPvPGameModeTable),m_uiWinCondition(pPacket->uiWinCondition),m_uiPlayTimeSec(pPacket->uiPlayTimeSec)
,m_uiGameModeCheck(pPacket->unGameModeCheck),m_fStartDelta(static_cast<float>(PvPCommon::Common::GameModeStartDelta)),m_uiStartTick(0)
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
,m_cPvPChannelType(static_cast<char>(pPacket->cPvPChannelType))
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#if defined(PRE_ADD_REVENGE)
,m_pRevengeSystem(NULL)
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
,m_nStartPlayer(pPacket->nStartPlayer)
#endif
{
	_Create();
	bool bCheck = (pPvPGameModeTable->uiGameMode == PvPCommon::GameMode::PvP_GuildWar && pPacket->bIsGuildWar) ? true : false;
	_CreateChecker(bCheck);
#if defined(PRE_ADD_PVP_TOURNAMENT)
	m_uiWinSessionID = 0;
#endif
	m_fElapsedTimeRound		= 0.f;
	m_bRoundCountingTrigger	= false;
	m_FinishDetailReason = PvPCommon::FinishDetailReason::None;
#if defined( PRE_WORLDCOMBINE_PVP )	
	m_bSendStartMsg =false;
	m_nSendStarMsgUserSessionID = 0;
	m_fWorldPvPRoomWaitStartDelta = 0.f;
	m_bWaitStartCount = false;
	m_bStartSetting = false;
#endif
}

CPvPGameMode::~CPvPGameMode()
{
	SAFE_DELETE( m_pScoreSystem );
	for( UINT i=0 ; i<m_vGameModeChecker.size() ; ++i )
		delete m_vGameModeChecker[i];
#if defined(PRE_ADD_REVENGE)
	SAFE_DELETE( m_pRevengeSystem );
#endif
}

void CPvPGameMode::OnInitializeActor( CDNUserSession* pSession )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	if( hActor )
	{
		hActor->SetTeam( pSession->GetTeam() );
		hActor->SetMaxDieDelta( 10.f );
	}
}

void CPvPGameMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	// 1.시작시간 동기화
#if defined( PRE_WORLDCOMBINE_PVP )
	bool bSendStartTick = true;
	if( GetGameRoom()->bIsWorldPvPRoom() )
	{
		if( GetGameRoom()->bIsWorldPvPRoomStart() == false )
			bSendStartTick = false;
	}
	
	if( bSendStartTick )
		pGameSession->SendPvPModeStartTick( m_uiStartTick );
#else
	pGameSession->SendPvPModeStartTick( m_uiStartTick );
#endif
	DnActorHandle hActor = pGameSession->GetActorHandle();
	if( hActor )
	{
		// 쿨타임
		_SetSkillCoolTimeRule( hActor );

		// 2.HP/SP
		_OnRefreshHPSP( hActor );
	}
	// 3.스코어 동기화
	_SendPVPModeScore( pGameSession );
	// 4.게임모드가 종료된다음에 난입한 경우라면 종료처리.
	if( m_bFinishGameModeFlag )
	{
		pGameSession->SetPvPGameModeFinish( true );
		_FinishGameMode( pGameSession, m_FinishReason );
	}
	// 5.옵져버체크
	_ASSERT( m_pGameRoom != NULL );
	_ASSERT( m_pGameRoom->bIsPvPRoom() == true );
	static_cast<CDNPvPGameRoom*>(m_pGameRoom)->CmdObserverAddStateEffect( hActor );

	// 난입성공했다는 이벤트 트리거 발생
	GetGameRoom()->GetWorld()->OnTriggerEventCallback( "CPvPGameMode::OnSuccessBreakInto", 0, 0 );
}

void CPvPGameMode::OnRebirth( DnActorHandle hActor, bool bForce/*=false*/ )
{
	// 1. 게임이 끝났다면..
	if( m_bFinishGameModeFlag || bForce )
	{
		// 2. 무적상태효과 걸어준다.
		if( hActor )
			hActor->CDnActor::AddStateBlow( STATE_BLOW::BLOW_099, NULL, -1, "-1" );
	}
}

void CPvPGameMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	_ASSERT( m_pGameRoom );
	if( hHitter && hHitter->IsMonsterActor() )
	{		
		DnActorHandle hMaster = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
		if( hMaster && hMaster->IsPlayerActor() )
			hHitter = hMaster;
	}

	// ScoreSystem
	m_pScoreSystem->OnDie( hActor, hHitter );
	_OnKillPlayer( hHitter );
	// Mission Event
	if( hActor && hActor->IsPlayerActor() && hHitter && hHitter->IsPlayerActor() )
	{

#if defined(PRE_ADD_REVENGE)
		bool bRevengeSuccess = false;
		CDNUserSession *pGameSession = GetGameRoom()->GetUserSession( hActor->GetSessionID() );
		CDNUserSession *pHitterSession = GetGameRoom()->GetUserSession( hHitter->GetSessionID() );

		if( pGameSession && pHitterSession )
		{
			if( m_pGameRoom->GetEventRoomIndex() > 0 && m_pRevengeSystem )
			{
				bRevengeSuccess = m_pRevengeSystem->CheckRevenge(pGameSession, pHitterSession);
				if(bRevengeSuccess)
					((CPvPScoreSystem *)m_pScoreSystem)->OnRevengeSuccess(hHitter);
			}
		}
#else	//#if defined(PRE_ADD_REVENGE)
		CDNUserSession *pGameSession = GetGameRoom()->GetUserSession( hActor->GetSessionID() );
#endif	// #if defined(PRE_ADD_REVENGE)
		CPvPScoreSystem::SMyScore *pMyScore = ((CPvPScoreSystem *)m_pScoreSystem)->FindMyScoreData( pGameSession->GetCharacterName() );	// pvp안에서는 강제 캐스팅 허용
		UINT uiActorSumKOby = 0;	// 죽은 플레이어의 데스 총합
		for( UINT i=0 ; i<PvPCommon::Common::MaxClass ; ++i ) {
			uiActorSumKOby += pGameSession->GetPvPData()->uiKObyClassCount[i];
			uiActorSumKOby += pMyScore->sKilledClassScore.uiScore[hHitter->GetClassID()-1];
		}
		if( GetGameRoom()->bIsLadderRoom() == false )
		{
#if defined(PRE_ADD_REVENGE)
			pGameSession->GetEventSystem()->OnEvent( EventSystem::OnDie, 4,
				EventSystem::KObyClassIDinPvP, hHitter->GetClassID(),
				EventSystem::KObyClassCountinPvP, pGameSession->GetPvPData()->uiKObyClassCount[hHitter->GetClassID()-1] + pMyScore->sKilledClassScore.uiScore[hHitter->GetClassID()-1],
				EventSystem::KObyCountinPvP, uiActorSumKOby, EventSystem::PvPRevengeSuccess, bRevengeSuccess );
#else
			pGameSession->GetEventSystem()->OnEvent( EventSystem::OnDie, 3,
				EventSystem::KObyClassIDinPvP, hHitter->GetClassID(),
				EventSystem::KObyClassCountinPvP, pGameSession->GetPvPData()->uiKObyClassCount[hHitter->GetClassID()-1] + pMyScore->sKilledClassScore.uiScore[hHitter->GetClassID()-1],
				EventSystem::KObyCountinPvP, uiActorSumKOby );
#endif
		}
#if defined(PRE_ADD_REVENGE)
#else
		CDNUserSession *pHitterSession = GetGameRoom()->GetUserSession( hHitter->GetSessionID() );
#endif
		CPvPScoreSystem::SMyScore *pHitterScore = ((CPvPScoreSystem *)m_pScoreSystem)->FindMyScoreData( pHitterSession->GetCharacterName() );	// pvp안에서는 강제 캐스팅 허용
		UINT uiHitterSumKO = 0;		// 죽인 플레이어의 킬 총합
		for( UINT i=0 ; i<PvPCommon::Common::MaxClass ; ++i ) {
			uiHitterSumKO += pHitterSession->GetPvPData()->uiKOClassCount[i];
			uiHitterSumKO += pHitterScore->sKillClassScore.uiScore[i];
		}

		if( GetGameRoom()->bIsLadderRoom() == false )
		{
			if( GetGameRoom()->bIsZombieMode() == true )
			{
				CPvPZombieMode* pCaptainMode = static_cast<CPvPZombieMode*>(this);

				bool bZombieKill = pCaptainMode->bIsZombie( hActor );
				if( bZombieKill )
				{
#if defined(PRE_ADD_REVENGE)
					pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 4,
						EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
						EventSystem::PvPTeam, PvPCommon::Team::Human, EventSystem::KOCountinPvP, 
						pHitterSession->GetTotalGhoulScore(GhoulMode::PointType::GhoulKill), EventSystem::PvPRevengeSuccess, bRevengeSuccess );
#else
					pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 3,
						EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
						EventSystem::PvPTeam, PvPCommon::Team::Human, EventSystem::KOCountinPvP, pHitterSession->GetTotalGhoulScore(GhoulMode::PointType::GhoulKill));
#endif
				}		
				else
				{
#if defined(PRE_ADD_REVENGE)
					pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 4,
						EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
						EventSystem::PvPTeam, PvPCommon::Team::Zombie, EventSystem::KOCountinPvP, 
						pHitterSession->GetTotalGhoulScore(GhoulMode::PointType::HumanKill), EventSystem::PvPRevengeSuccess, bRevengeSuccess);
#else
					pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 3,
						EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
						EventSystem::PvPTeam, PvPCommon::Team::Zombie, EventSystem::KOCountinPvP, pHitterSession->GetTotalGhoulScore(GhoulMode::PointType::HumanKill));
#endif
				}
			}
			else						
			{
#if defined(PRE_ADD_REVENGE)
				pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 4,
					EventSystem::KOClassIDinPvP, hActor->GetClassID(),
					EventSystem::KOClassCountinPvP, pHitterSession->GetPvPData()->uiKOClassCount[hActor->GetClassID()-1] + pHitterScore->sKillClassScore.uiScore[hActor->GetClassID()-1],
					EventSystem::KOCountinPvP, uiHitterSumKO, EventSystem::PvPRevengeSuccess, bRevengeSuccess );
#else
				pHitterSession->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 3,
					EventSystem::KOClassIDinPvP, hActor->GetClassID(),
					EventSystem::KOClassCountinPvP, pHitterSession->GetPvPData()->uiKOClassCount[hActor->GetClassID()-1] + pHitterScore->sKillClassScore.uiScore[hActor->GetClassID()-1],
					EventSystem::KOCountinPvP, uiHitterSumKO );
#endif
			}			
		}
	}
}

void CPvPGameMode::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage )
{
	m_pScoreSystem->OnDamage( hActor, hHitter, iDamage );
}

void CPvPGameMode::OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo )
{
	m_pScoreSystem->OnCmdAddStateEffect( pSkillInfo );
}

void CPvPGameMode::OnLeaveUser( DnActorHandle hActor )
{
	if( !hActor )
		return;
	// PlayTime
	if( hActor->GetName() )
		m_mPlayTimeDelta.erase( hActor->GetName() );
	// ScoreSystem
	m_pScoreSystem->OnLeaveUser( hActor );
}

bool CPvPGameMode::bIsPlayingUser( DnActorHandle hActor )
{
	if( !hActor )
		return false;

	if( hActor->bIsObserver() )
		return false;

	return true;
}

bool CPvPGameMode::OnTryAcquirePoint(DnActorHandle hActor, int nAreaID, LOCAL_TIME LocalTime)
{
	return m_pScoreSystem->OnTryAcquirePoint(hActor, nAreaID, LocalTime);
}

bool CPvPGameMode::OnAcquirePoint(int nTeam, const WCHAR * pName, bool bSNatched)
{
	return m_pScoreSystem->OnAcquirePoint(nTeam, pName, bSNatched);
}

void CPvPGameMode::_Create()
{
	m_uiVictoryExp			= 0;
	m_uiVictoryBonusRate	= 0;
	m_uiDefeatExp			= 0;
	m_uiDefeatBonusRate		= 0;

	int iIndex = m_pPvPGameModeTable->GetWinconditionIndex( m_uiWinCondition );
	if( iIndex < 0 )
	{
		_DANGER_POINT();
		return;
	}

	m_uiVictoryExp			= m_pPvPGameModeTable->vVictoryExp[iIndex];
	m_uiVictoryBonusRate	= m_pPvPGameModeTable->vVictoryBonusRate[iIndex];
	m_uiDefeatExp			= m_pPvPGameModeTable->vDefeatExp[iIndex];
	m_uiDefeatBonusRate		= m_pPvPGameModeTable->vDefeatBonusRate[iIndex];
}
void CPvPGameMode::_CreateChecker(bool bIsGuildWar)
{
	// Checker 중 가장 먼저 등록되어야 한다.!!!
	if( m_pGameRoom->bIsLadderRoom() )
		m_vGameModeChecker.push_back( new CPvPGameModeLadderNoGameChecker(this) );	

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	if( PvPCommon::GameMode::PvP_ComboExercise != m_pPvPGameModeTable->uiGameMode )
	{
		m_vGameModeChecker.push_back( new CPvPGameModeTimeOverChecker(this) );
	}
#else
	m_vGameModeChecker.push_back( new CPvPGameModeTimeOverChecker(this) );
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

#if defined(PRE_ADD_RACING_MODE)
	if (bIsGuildWar == false && m_pPvPGameModeTable->uiGameMode != PvPCommon::GameMode::PvP_Racing )		//시스템 길드전 and Racing 모드일 경우에는 인원이 없더라도 진행이 된다
#else
	if (bIsGuildWar == false)		//시스템에 의한 길드전일 경우에는 인원이 없더라도 진행이 된다
#endif // #if defined(PRE_ADD_RACING_MODE)
	{
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		if( PvPCommon::GameMode::PvP_ComboExercise != m_pPvPGameModeTable->uiGameMode )		
			m_vGameModeChecker.push_back( new CPvPGameModeTeamMemberChecker(this) );		
#else
		m_vGameModeChecker.push_back( new CPvPGameModeTeamMemberChecker(this) );
#endif
	}

	m_vGameModeChecker.push_back( new CPvPGameModeZombieChecker(this) );	

	switch( m_pPvPGameModeTable->uiGameMode )
	{
 		case PvPCommon::GameMode::PvP_Respawn:
		case PvPCommon::GameMode::PvP_IndividualRespawn:
		{
			m_vGameModeChecker.push_back( new CPvPGameModeNoMoreBreakIntoChecker(this) );
			break;
		}
	}
}

void CPvPGameMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
#if defined( PRE_WORLDCOMBINE_PVP )
	if( !m_bStartGameModeFlag && GetGameRoom()->bIsWorldPvPRoom() )
	{
		if( !CheckWorldPvPRoomStart(fDelta) )
			return;		
	}
#endif
	if( !m_bStartGameModeFlag )
	{
		m_bStartGameModeFlag	= true;
		m_bRoundCountingTrigger	= false;
		m_fStartDelta			= static_cast<float>(PvPCommon::Common::GameModeStartDelta);
		m_uiStartTick			= timeGetTime()+(PvPCommon::Common::GameModeStartDelta*1000);	// 게임 시작 시간은 GameModeStartDelta 만큼 더해준다.
		m_fTimeOver				= static_cast<float>(m_uiPlayTimeSec);
		m_fElapsedTimeRound		= 0.f;

		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);

			// SESSION_STATE_GAME_PLAY 인 유저에게만 보내준다.
			if( pPartyStruct->pSession && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
			{
				pPartyStruct->pSession->SendPvPModeStartTick( m_uiStartTick );

				DnActorHandle hActor = pPartyStruct->pSession->GetActorHandle();
				if( hActor )
				{
					_SetSkillCoolTimeRule( hActor );
					_OnRefreshHPSP( hActor );
				}
			}
		}
		
		_SetPvPStartLog();
	}

	if( m_fStartDelta > 0.f )
	{
		if( m_bRoundCountingTrigger == false && bIsRoundMode() == true )
		{
			GetGameRoom()->GetWorld()->OnTriggerEventCallback( "CPvPRoundMode::OnRoundCounting", LocalTime, fDelta );
			m_bRoundCountingTrigger = true;
		}
		m_fStartDelta -= fDelta;
	}

	if( m_fStartDelta <= 0.f )
	{
		m_fTimeOver			-= fDelta;
		m_fElapsedTimeRound	+= fDelta;
		_ProcessPlayTime( fDelta );
		_ProcessChecker();
	}
}

void CPvPGameMode::_SetPvPStartLog()
{
	// Log
	if( m_uiCurRound++ == 1 )
	{
		// PvP 시작 로그
		BYTE cThreadID;
		CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
		if( pDBCon )
		{
			CDNPvPGameRoom* pPvPGameRoom = static_cast<CDNPvPGameRoom*>(GetGameRoom());
			INT64 biSNMain = 0; 
			int nSNSub = 0;
			pPvPGameRoom->GetPvPSN(biSNMain, nSNSub);
			CPvPGameMode* pPvPGameMode = GetGameRoom()->GetPvPGameMode();
			DBDNWorldDef::PvPModeCode::eCode ModeCode = static_cast<DBDNWorldDef::PvPModeCode::eCode>(0);
			switch (pPvPGameMode->GetPvPGameModeTable()->uiGameMode)
			{
			case PvPCommon::GameMode::PvP_Respawn:				ModeCode = DBDNWorldDef::PvPModeCode::Respawn;				break;
			case PvPCommon::GameMode::PvP_Round:			ModeCode = DBDNWorldDef::PvPModeCode::Round;				break;
			case PvPCommon::GameMode::PvP_Captain:				ModeCode = DBDNWorldDef::PvPModeCode::Captain;				break;
			case PvPCommon::GameMode::PvP_IndividualRespawn:	ModeCode = DBDNWorldDef::PvPModeCode::IndividualRespawn;	break;
			case PvPCommon::GameMode::PvP_Zombie_Survival:	ModeCode = DBDNWorldDef::PvPModeCode::Zombie;	break;
			case PvPCommon::GameMode::PvP_GuildWar:	ModeCode = DBDNWorldDef::PvPModeCode::GuildWar;	break;
			case PvPCommon::GameMode::PvP_Occupation:	ModeCode = DBDNWorldDef::PvPModeCode::Occupation;	break;
			case PvPCommon::GameMode::PvP_AllKill: ModeCode = DBDNWorldDef::PvPModeCode::AllKill; break;
#if defined(PRE_ADD_RACING_MODE)
			case PvPCommon::GameMode::PvP_Racing: ModeCode = DBDNWorldDef::PvPModeCode::Racing; break;
#endif // #if defined( PRE_ADD_RACING_MODE)
#if defined(PRE_ADD_PVP_TOURNAMENT)
			case PvPCommon::GameMode::PvP_Tournament: ModeCode = DBDNWorldDef::PvPModeCode::Tournament; break;
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			case PvPCommon::GameMode::PvP_ComboExercise: ModeCode = DBDNWorldDef::PvPModeCode::ComboExercise; break;
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
			}

			if( GetGameRoom()->bIsLadderRoom() )
			{
				switch( static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetPvPLadderMatchType() )
				{
					case LadderSystem::MatchType::_1vs1:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::Ladder1vs1;
						break;
					}
					case LadderSystem::MatchType::_2vs2:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::Ladder2vs2;
						break;
					}
					case LadderSystem::MatchType::_3vs3:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::Ladder3vs3;
						break;
					}
					case LadderSystem::MatchType::_4vs4:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::Ladder4vs4;
						break;
					}
#if defined(PRE_ADD_DWC)
					case LadderSystem::MatchType::_3vs3_DWC:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::DWCRegular;
						break;
					}
					case LadderSystem::MatchType::_3vs3_DWC_PRACTICE:
					{
						ModeCode = DBDNWorldDef::PvPModeCode::DWCPractice;
						break;
					}
#endif
				}
			}

			if( ModeCode <= DBDNWorldDef::PvPModeCode::Round )
				g_Log.Log( LogType::_ERROR, L"CPvPGameMode::_SetPvPStartLog() PvPMode 로그 추가해주세요.\r\n");

			pDBCon->QueryAddPvPStartLog( cThreadID, GetGameRoom()->GetWorldSetID(), 0, biSNMain, nSNSub, pPvPGameRoom->GetPvPMaxUser(), ModeCode, pPvPGameMode->GetSelectWinCondition(), pPvPGameRoom->bIsBreakIntoRoom(), 
				pPvPGameRoom->bIsDropItemRoom(), pPvPGameRoom->bIsPWRoom(), GetGameRoom()->m_iMapIdx, pPvPGameRoom->GetRoomMasterCharacterDBID(), pPvPGameRoom->GetCreateRoomCharacterDBID());

#if defined( PRE_PVP_GAMBLEROOM )
#endif
		}
	}
}

void CPvPGameMode::_ProcessChecker()
{
	for( UINT i=0 ; i<m_vGameModeChecker.size() ; ++i )
		m_vGameModeChecker[i]->Check();
}

void CPvPGameMode::_ProcessPlayTime( const float fDelta )
{
	if( m_bFinishGameModeFlag )
		return;

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		
		if( pPartyStruct->pSession && pPartyStruct->pSession->GetState() == SESSION_STATE_GAME_PLAY )
		{
			if( bIsPlayingUser( pPartyStruct->pSession->GetActorHandle()) )
			{
				const wchar_t* pwszName = pPartyStruct->pSession->GetCharacterName();
				if( pwszName )
				{
					std::map<std::wstring,float>::iterator itor =  m_mPlayTimeDelta.find( pwszName );
					if( itor == m_mPlayTimeDelta.end() )
					{
						m_mPlayTimeDelta.insert( std::make_pair(pwszName,fDelta) );
					}
					else
					{
						(*itor).second += fDelta;
					}
				}
			}
		}
	}
}

void CPvPGameMode::_OnRefreshHPSP( DnActorHandle hActor )
{
	CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
	pPlayer->CmdRefreshHPSP( hActor->GetMaxHP()*m_pPvPGameModeTable->uiRespawnHPPercent/100, hActor->GetMaxSP()*m_pPvPGameModeTable->uiRespawnMPPercent/100 );
}

bool CPvPGameMode::InitializeBase( const UINT uiWinCondition )
{
	switch( m_pPvPGameModeTable->uiGameMode )
	{
		case PvPCommon::GameMode::PvP_Captain:
		{
			m_pScoreSystem = new CPvPCaptainScoreSystem();
			break;
		}
		case PvPCommon::GameMode::PvP_Zombie_Survival:
		{
			m_pScoreSystem = new CPvPZombieScoreSystem();
			break;
		}
		case PvPCommon::GameMode::PvP_GuildWar:
		{
			m_pScoreSystem = new CPvPGuildWarScoreSystem();
			break;
		}
		case PvPCommon::GameMode::PvP_Occupation:
		{
			m_pScoreSystem = new CPvPOccupationScoreSystem();
			break;
		}
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		case PvPCommon::GameMode::PvP_ComboExercise:
		{
			m_pScoreSystem = new CPvPComboExerciseScoreSystem();
			break;
		}
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
		
		default:
		{
			m_pScoreSystem = new IBoostPoolPvPScoreSystem();
			break;
		}
	}
#if defined(PRE_ADD_REVENGE)
	// #70076 미션룸 중 개인전 및 리스폰 모드에서만 리벤지 적용
	if( m_pGameRoom->GetEventRoomIndex() > 0 && 
		(m_pPvPGameModeTable->uiGameMode == PvPCommon::GameMode::PvP_Respawn || m_pPvPGameModeTable->uiGameMode == PvPCommon::GameMode::PvP_IndividualRespawn ))
	{
		m_pRevengeSystem = new CRevengeSystem(this);
	}
#endif

	return m_pScoreSystem->InitializeBase( m_pPvPGameModeTable, uiWinCondition );
}

UINT CPvPGameMode::OnCheckZeroUserWinTeam( const bool bCheckBreakIntoUser/*=true*/ )
{
	UINT uiCheckBit = 0;

	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);

		if( pPartyStruct->pSession )
		{
			// 플레이중인 유저 검사
			if( bCheckBreakIntoUser )
			{
				if( !bIsPlayingUser( pPartyStruct->pSession->GetActorHandle() ) )
					continue;
			}
			else
			{
				if( pPartyStruct->pSession->GetState() != SESSION_STATE_GAME_PLAY )
					continue;
			}

			if( pPartyStruct->pSession->GetTeam() == PvPCommon::Team::A )
				uiCheckBit |= 1;
			else if( pPartyStruct->pSession->GetTeam() == PvPCommon::Team::B )
				uiCheckBit |= 2;

			if( uiCheckBit == 3 )
				return PvPCommon::Team::Max;
		}
	}

	if( uiCheckBit&1 )
		return PvPCommon::Team::A;
	else if( uiCheckBit&2 )
		return PvPCommon::Team::B;

	return PvPCommon::Team::Max;
}

UINT CPvPGameMode::OnCheckFinishWinTeam()
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	UINT uiATeamScore,uiBTeamScore;
	GetGameModeScore( uiATeamScore, uiBTeamScore );

	if( uiATeamScore > uiBTeamScore )
		uiWinTeam = PvPCommon::Team::A;
	else if( uiBTeamScore > uiATeamScore )
		uiWinTeam = PvPCommon::Team::B;

	return uiWinTeam;
}

UINT CPvPGameMode::GetPlayTime( const wchar_t* pwszName )
{
	if( !pwszName )
		return 0;

	std::map<std::wstring,float>::iterator itor = m_mPlayTimeDelta.find( pwszName );
	if( itor == m_mPlayTimeDelta.end() )
		return 0;

	return static_cast<UINT>((*itor).second);
}

// PvPGameMode 가 시작된지 uiSec 이내인지 체크
bool CPvPGameMode::bIsInPlayTime( const UINT uiSec )
{
	if( GetRemainStartTick() || GetTimeOver() <= 0.f )	
	{
		_DANGER_POINT();
		return false;
	}
	
	UINT uiCurSec = static_cast<UINT>(GetTimeOver());
	if( GetSelectPlayTime() - uiCurSec <= uiSec )
		return true;

	return false;
}

bool CPvPGameMode::bIsPlaying()
{
	if( bIsStarting() )
		return false;
	if( bIsFinishFlag() )
		return false;
	return true;
}

UINT CPvPGameMode::GetPvPStartStateEffectDurationTick()
{
	return GetRemainStartTick();
}

void CPvPGameMode::FinishGameMode( const UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason )
{
	// 마스터서버에 PvPGameMode 끝났음을 알림.
	if( g_pMasterConnectionManager )
	{
		CDNPvPGameRoom* pPvpGameRoom = static_cast<CDNPvPGameRoom*>(m_pGameRoom);
		if( pPvpGameRoom->bIsLadderRoom() == true )
			g_pMasterConnectionManager->SendPvPLadderRoomSync( GetGameRoom()->m_iWorldID, pPvpGameRoom->GetLadderRoomIndex(0), pPvpGameRoom->GetLadderRoomIndex(1), LadderSystem::RoomState::GameFinished );
		else
			g_pMasterConnectionManager->SendPvPCommand( GetGameRoom()->m_iWorldID, PvPCommon::GAMA_Command::FinishGameMode, GetGameRoom()->GetPvPIndex(), GetGameRoom()->GetRoomID() );
	}

	m_bFinishGameModeFlag	= true;
	m_uiWinTeam				= uiWinTeam;
	m_FinishReason			= Reason;

#if defined(PRE_ADD_DWC)
	CDNPvPGameRoom* pPvpGameRoom = static_cast<CDNPvPGameRoom*>(m_pGameRoom);

	if( (pPvpGameRoom->bIsLadderRoom() && pPvpGameRoom->bIsDWCMatch()) || GetPvPChannelType() == PvPCommon::RoomType::dwc )
	{
		_FinishDWCGameMode( Reason );
	}
	else
	{
#endif	//#if defined(PRE_ADD_DWC)
		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
			if( pPartyStruct->pSession )
				_FinishGameMode( pPartyStruct->pSession, Reason );
		}
#if defined(PRE_ADD_DWC)
	}
#endif	//#if defined(PRE_ADD_DWC)
	//DB에서 저장안하는 애는 여기서 보낸다.	
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		if( pPartyStruct->pSession )
		{
			if( static_cast<CPvPScoreSystem*>(m_pScoreSystem)->bIsQueryCharacterDBID( pPartyStruct->pSession->GetCharacterDBID() ) == false )
				SendFinishGameMode( pPartyStruct->pSession );
		}
	}
#if defined( PRE_WORLDCOMBINE_PVP )
//	if( GetGameRoom()->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::GMRoom )
//	{
//		m_bFinishGameModeFlag	= false;
//		m_uiWinTeam				= 0;
//	}
#endif
}

void CPvPGameMode::SendFinishGameMode( CDNUserSession* pGameSession )
{
	// 1. PvP모드 결과를 알려준다.
	pGameSession->SendPvPModeFinish( this );
	// 2. XP 결과 공유해준다.
	m_pScoreSystem->SendXPScore( GetGameRoom(), pGameSession );
	// 3. 게임모드 정상종료
	pGameSession->SetPvPGameModeFinish( true );
}

// 상위에서 pGameSession Valid 검사함.
void CPvPGameMode::_FinishGameMode( CDNUserSession* pGameSession, PvPCommon::FinishReason::eCode Reason )
{
#if defined(PRE_ADD_DWC)
	if(static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsDWCMatch() || GetPvPChannelType() == PvPCommon::RoomType::dwc)
		return;
#endif

	if( pGameSession->GetState() == SESSION_STATE_GAME_PLAY )
	{
		// 1. 무적Blow 걸어준다.
		DnActorHandle hActor = pGameSession->GetActorHandle();
		if( hActor )
			hActor->CDnActor::AddStateBlow( STATE_BLOW::BLOW_099, NULL, -1, "-1" );

		if( Reason == PvPCommon::FinishReason::LadderNoGame )
		{
#if defined( _WORK )
			std::cout << "[Ladder] NoGame!!!" << std::endl;
#endif // #if defined( _WORK )
			static_cast<CPvPScoreSystem*>(m_pScoreSystem)->PushQueryCharacterDBID( pGameSession->GetCharacterDBID() );
			SendFinishGameMode( pGameSession );
			return;
		}

		bool bIsLadderRoom = GetGameRoom()->bIsLadderRoom();

		if( bIsPlayingUser( pGameSession->GetActorHandle() ) && !pGameSession->GetPvPGameModeFinish() )
		{
			if( bIsLadderRoom == false )
			{
				if( pGameSession->GetGameRoom() && static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->GetEventRoomIndex() > 0 && bIsLadderRoom == false )
				{
					pGameSession->GetEventSystem()->OnEvent( EventSystem::OnPvPEventFinished );
				}
			}
			else
			{
				pGameSession->GetEventSystem()->OnEvent( EventSystem::OnLadderFinished );
			}

			// 2. DB 저장
			if( m_pScoreSystem->QueryUpdatePvPData( m_uiWinTeam, pGameSession, PvPCommon::QueryUpdatePvPDataType::FinishGameMode ) == false )
			{
				SendFinishGameMode( pGameSession );
			}
			else
				_OnFinishToUpdatePeriodic( pGameSession );
		}
		//else
		//{
			// 3.DB에 저장할 필요 없는 유저는 바로 종료함.
			//SendFinishGameMode( pGameSession );
			//static_cast<CPvPScoreSystem*>(m_pScoreSystem)->PushQueryCharacterDBID( pGameSession->GetCharacterDBID() );
		//}

		if( hActor && bIsLadderRoom == false )
		{
			// OnMissionEvent
			if( m_uiWinTeam != PvPCommon::Team::Max )
			{
				if( hActor->GetTeam() == m_uiWinTeam )
				{
					pGameSession->GetEventSystem()->OnEvent( EventSystem::OnPvPVictory );
				}
				else
				{
					if( m_pPvPGameModeTable->uiGameMode != PvPCommon::GameMode::PvP_IndividualRespawn)
						pGameSession->GetEventSystem()->OnEvent( EventSystem::OnPvPLoss );
				}
			}
			pGameSession->GetEventSystem()->OnEvent( EventSystem::OnPvPFinished );
		}
	}
}

#if defined(PRE_ADD_DWC)
void CPvPGameMode::_FinishDWCGameMode( PvPCommon::FinishReason::eCode Reason )
{
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		if( pPartyStruct->pSession)
		{
			CDNUserSession *pGameSession = pPartyStruct->pSession;

			if( pGameSession->GetState() == SESSION_STATE_GAME_PLAY )
			{
				// 1. 무적Blow 걸어준다.
				DnActorHandle hActor = pGameSession->GetActorHandle();
				if( hActor )
					hActor->CDnActor::AddStateBlow( STATE_BLOW::BLOW_099, NULL, -1, "-1" );
			}
			
			static_cast<CPvPScoreSystem*>(m_pScoreSystem)->PushQueryCharacterDBID( pGameSession->GetCharacterDBID() );
		}
	}

	bool bRet = false;
	// 실제 래더 포인트 매칭만 디비 저장
	if( GetGameRoom()->bIsLadderRoom() && static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetPvPLadderMatchType() ==  LadderSystem::MatchType::_3vs3_DWC )
	{
		LadderSystem::CDWCStatsRepository*	pStatsRepository = static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetDWCStatsRepositoryPtr();	
		if(pStatsRepository)
		{
			pStatsRepository->QueryUpdateResult( m_uiWinTeam, PvPCommon::QueryUpdatePvPDataType::FinishGameMode );
			bRet = true;
		}
	}

	//일반,연습모드나 업데이트 쿼리 실패하면 종료 처리
	if(!bRet)
		SendFinishDWCGameMode();
}

void CPvPGameMode::SendFinishDWCGameMode()
{
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNGameRoom::PartyStruct* pPartyStruct = GetGameRoom()->GetPartyData(i);
		if( pPartyStruct->pSession )
			SendFinishGameMode( pPartyStruct->pSession );
	}
}

#endif

void CPvPGameMode::GetGameModeScore( UINT& uiATeamScore, UINT& uiBTeamScore ) const
{
	uiATeamScore = uiBTeamScore = 0;
	m_pScoreSystem->GetGameModeScore( uiATeamScore, uiBTeamScore );
}

void CPvPGameMode::_SendPVPModeScore( CDNUserSession* pGameSession )
{
	m_pScoreSystem->SendScore( pGameSession );
}

void CPvPGameMode::_SetRespawnPosition( DnActorHandle hActor, const bool bIsStartPosition/*=false*/ )
{
	if( !hActor )
		return;
	CTaskManager* pTaskMng = GetGameRoom()->GetTaskMng();
	if( pTaskMng )
	{
		CDnGameTask* pGameTask = (CDnGameTask*)(pTaskMng->GetTask("GameTask"));
		if( pGameTask )
		{
			((CDnPvPGameTask*)m_pGameRoom->GetGameTask())->OnInitializeRespawnPosition( hActor, bIsStartPosition );
			hActor->CmdWarp();
		}
	}
}

void CPvPGameMode::_SetSkillCoolTimeRule( DnActorHandle hActor )
{
	if( hActor )
	{
		if( GetGameRoom()->bIsLadderRoom() || bIsAllKillMode() == true
	#ifdef PRE_ADD_PVP_TOURNAMENT
			|| bIsTournamentMode()
	#endif
			)
		{
			hActor->ResetLadderSkillCoolTime();
		}
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		else if( bIsComboExerciseMode() )
		{
			hActor->ResetSkillCoolTime();
		}
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
		else
		{
			hActor->ResetPvPSkillCoolTime();
		}
	}
}

void CPvPGameMode::_OnFinishToUpdatePeriodic( CDNUserSession* pGameSession )
{
	if( m_pPvPGameModeTable == NULL )
		return;

	DnActorHandle hActor = pGameSession->GetActorHandle();
	if( !hActor )
		return;

	switch (m_pPvPGameModeTable->uiGameMode) // PvPCommon::GameMode
	{
	case PvPCommon::GameMode::PvP_Occupation:
		{
			// 플레이 수
			pGameSession->IncCommonVariableData(CommonVariable::Type::PvPOccupation_PlayCount);

			// 승리 수
			if (hActor->GetTeam() == m_uiWinTeam)
				pGameSession->IncCommonVariableData(CommonVariable::Type::PvPOccupation_WinCount);	
		}
		break;
	case PvPCommon::GameMode::PvP_AllKill:
		{
			// 플레이 수
			pGameSession->IncCommonVariableData(CommonVariable::Type::PvPAllKill_PlayCount);

			// 승리 수
			if (hActor->GetTeam() == m_uiWinTeam)
				pGameSession->IncCommonVariableData(CommonVariable::Type::PvPAllKill_WinCount);	
		}
		break;
	}
}

void CPvPGameMode::_OnKillPlayer( DnActorHandle hHitter )
{
	if( m_pPvPGameModeTable == NULL )
		return;

	if( !hHitter )
		return;

	CDNUserSession* pGameSession = m_pGameRoom->GetUserSession(hHitter->GetSessionID());
	if( !pGameSession )
		return;

	switch (m_pPvPGameModeTable->uiGameMode) // PvPCommon::GameMode
	{
		case PvPCommon::GameMode::PvP_Occupation:
		{
			// 킬수
			pGameSession->IncCommonVariableData(CommonVariable::Type::PvPOccupation_KillCount);
		}
		break;
		case PvPCommon::GameMode::PvP_AllKill:
		{
			// 킬수
			pGameSession->IncCommonVariableData(CommonVariable::Type::PvPAllKill_KillCount);
		}
		break;
	}
}

#if defined( PRE_WORLDCOMBINE_PVP )
bool CPvPGameMode::CheckWorldPvPRoomStart( float fDelta )
{
	if( !m_bSendStartMsg )
	{
		if( CheckWorldPvPRoomMinMemberCount() && !m_bWaitStartCount )
		{
			if( GetGameRoom()->GetBreakIntoUserCount() > 0 )
				return false;

			for (DWORD i= 0; i<GetGameRoom()->GetUserCount(); i++)
			{
				CDNGameRoom::PartyStruct *pStruct = GetGameRoom()->GetPartyData(i);
				if(pStruct == NULL) 
					continue;

				if( pStruct->pSession )
				{
					if(GetGameRoom()->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::GMRoom )
					{
						if( pStruct->pSession->GetAccountLevel() >= AccountLevel_New && pStruct->pSession->GetAccountLevel() <= AccountLevel_Developer )
						{
							pStruct->pSession->SendWorldPvPRoomStartMsg( true, (UINT)(GetGameRoom()->GetInstanceID()) );				
							m_bSendStartMsg = true;
							m_nSendStarMsgUserSessionID = pStruct->pSession->GetSessionID();
							break;
						}
					}
					else
					{
						if( pStruct->pSession->GetTeam() != PvPCommon::Team::Observer )
						{
							pStruct->pSession->SendWorldPvPRoomStartMsg( true, (UINT)(GetGameRoom()->GetInstanceID()) );				
							m_bSendStartMsg = true;
							m_nSendStarMsgUserSessionID = pStruct->pSession->GetSessionID();
							break;
						}
					}					
				}
			}			
		}
	}
	else
	{
		if( CheckWorldMaxMemberCount() && !m_bWaitStartCount )
		{
			for (DWORD i= 0; i<GetGameRoom()->GetUserCount(); i++)
			{
				CDNGameRoom::PartyStruct *pStruct = GetGameRoom()->GetPartyData(i);
				if(pStruct == NULL) 
					continue;

				if( pStruct->pSession )
				{
					if( pStruct->pSession->GetSessionID() == m_nSendStarMsgUserSessionID )
					{
						pStruct->pSession->SendWorldPvPRoomStartMsg( false, (UINT)(GetGameRoom()->GetInstanceID()) );						
						break;
					}
				}
			}
			
			m_bWaitStartCount = true;			
			m_fWorldPvPRoomWaitStartDelta = WorldPvPMissionRoom::Common::WaitStartWorldPvPRoomSecond;
			m_nSendStarMsgUserSessionID = 0;
		}

		if( m_bWaitStartCount )
		{
			if( m_fWorldPvPRoomWaitStartDelta < 0 )
			{				
				if( CheckWorldPvPRoomMinMemberCount() )
				{					
					GetGameRoom()->SetWorldPvPRoomStart(true);
					m_bSendStartMsg = false;
				}
				else
				{
					m_bSendStartMsg = false;
					m_bWaitStartCount = false;
				}
			}
			else
				m_fWorldPvPRoomWaitStartDelta -= fDelta;
		}			
	}

	if( GetGameRoom()->bIsWorldPvPRoomStart() == false )
		return false;	

	if( bIsStartSetting() == false )
		return false;	

	return true;
}

bool CPvPGameMode::CheckWorldPvPRoomMinMemberCount()
{
	if( GetGameRoom() )
	{		
		if(m_nStartPlayer <= (int)(GetGameRoom()->GetLiveUserCount()) )
		{
			return true;				
		}		
	}
	return false;
}

bool CPvPGameMode::CheckWorldMaxMemberCount()
{
	if( GetGameRoom() && GetGameRoom()->GetWorldPvPRoomReqType() == WorldPvPMissionRoom::Common::MissionRoom )
	{		
		if(GetGameRoom()->GetPvPMaxUser() <= (int)(GetGameRoom()->GetLiveUserCount()) )
		{
			return true;				
		}		
	}
	return false;
}
#endif