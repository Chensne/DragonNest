
#include "stdafx.h"
#include "PvPScoreSystem.h"
#include "DnActor.h"
#include "DnSkill.h"
#include "PvPCaptainScoreSystem.h"
#include "PvPRespawnModeScoreSystem.h"
#include "PvPRoundModeScoreSystem.h"
#include "DnMonsterActor.h"
#include "PvPAllKillScoreSystem.h"

#ifdef _GAMESERVER
#include "DNUserSession.h"
#include "DNGameRoom.h"
#include "DNDBConnection.h"
#include "DNDBConnectionManager.h"
#include "PvPGameMode.h"
#include "DnPlayerActor.h"
#include "DNLogConnection.h"
#include "DNGameDataManager.h"
#include "DNPvPPlayerAggroSystem.h"
#include "DNPvPGameRoom.h"
#include "LadderStats.h"
#else
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"
#include "DnTableDB.h"
#include "DnPVPBaseHud.h"
#endif

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
#include "DnTotalLevelSkillBlows.h"
#endif
#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#endif // #if !defined( USE_BOOST_MEMPOOL )

CPvPScoreSystem::CPvPScoreSystem()
{
#ifdef _GAMESERVER
	m_bCreateXPScorePacket = false;
	memset( &m_XPScorePacket, 0, sizeof(m_XPScorePacket) );
#endif
}

void CPvPScoreSystem::ClearAllScore()
{
	SetGameModeScore( 0,0 );
	m_mScore.clear();
	m_mCount.clear();
}

void CPvPScoreSystem::OnFinishRound( DnActorHandle hActor, const bool bIsWin )
{
#ifdef _GAMESERVER

	SMyScore* pMyScore = FindMyScoreDataIfnotInsert( hActor );
	if( !pMyScore )
	{
		_DANGER_POINT();
		return;
	}

	if( bIsWin )
		++pMyScore->uiWinRound;
	else
		++pMyScore->uiLoseRound;
#endif
}

bool CPvPScoreSystem::AddUserScore( DnActorHandle hActor, const void* pData )
{
	if( !hActor || !hActor->GetName() )
	{
		_DANGER_POINT();
		return false;
	}

	const TPvPUserScore* pUserScore = static_cast<const TPvPUserScore*>(pData);

	SMyScore* pMyScore = FindMyScoreDataIfnotInsert( hActor );
	if( !pMyScore )
		return false;

	pMyScore->uiKillScore				= pUserScore->uiKillScore;
	pMyScore->uiAssistScore				= pUserScore->uiAssistScore;
	for( UINT i=0 ; i<PvPCommon::Common::MaxClass ; ++i )
	{
		pMyScore->sKillClassScore.uiScore[i]	= pUserScore->uiKillClassScore[i];
		pMyScore->sKilledClassScore.uiScore[i]	= pUserScore->uiKilledClassScore[i];
	}

	return true;
}

#if !defined( _GAMESERVER )

bool CPvPScoreSystem::SetCount( DnActorHandle hActor, const void* pData )
{
	if( !hActor || !hActor->GetName() || !hActor->IsPlayerActor() )
	{
		_DANGER_POINT();
		return false;
	}

	const TPvPUserCount* pUserCount = static_cast<const TPvPUserCount*>(pData);

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( hActor->GetName() );
	if( itor == m_mCount.end() )
	{
		m_mCount.insert( std::make_pair( hActor->GetName(), SCount(pUserCount->uiKillCount, pUserCount->uiDieCount,pUserCount->uiContinuousKillCount)) );
	}
	else
	{
		(*itor).second.uiKillCount				= pUserCount->uiKillCount;
		(*itor).second.uiDieCount				= pUserCount->uiDieCount;
		(*itor).second.uiContinuousKillCount	= pUserCount->uiContinuousKillCount;
	}
	return true;
}

#endif // #if !defined( _GAMESERVER )

IPVPGameModeScoreSystem* CPvPScoreSystem::CreatePvPGameModeScoreSystem( const UINT uiGameMode )
{
	switch( uiGameMode )
	{
		case PvPCommon::GameMode::PvP_Respawn:
			return new CPvPRespawnModeScoreSystem();
		case PvPCommon::GameMode::PvP_Round:
			return new IBoostPoolPvPRoundModeScoreSystem();
		case PvPCommon::GameMode::PvP_Captain:
			return new IBoostPoolPvPRoundModeScoreSystem();	// 대장전은 라운드 모드 스코어 시스템을 사용합니다.
		case PvPCommon::GameMode::PvP_IndividualRespawn:
			return new IPVPGameModeScoreSystem();	// 개인전(리스폰)은 게임모드 ScoreSystem 을 사용하지 않습니다. 그냥 NullInterface 로 IPvPGameModeScoreSystem() 생성합니다.
		case PvPCommon::GameMode::PvP_Zombie_Survival:
			return new IPVPGameModeScoreSystem();	// 개인전 스코어 모드사용 < 좀비모드 >
		case PvPCommon::GameMode::PvP_GuildWar:
		case PvPCommon::GameMode::PvP_Occupation:
			return new IPVPGameModeScoreSystem();
		case PvPCommon::GameMode::PvP_AllKill:
			return new CPvPAllKillModeScoreSystem();	//  라운드 모드 스코어 시스템을 사용합니다.
#if defined( PRE_ADD_RACING_MODE)
		case PvPCommon::GameMode::PvP_Racing:
			return new IPVPGameModeScoreSystem();
#endif
#if defined(PRE_ADD_PVP_TOURNAMENT)
		case PvPCommon::GameMode::PvP_Tournament:
			return new IPVPGameModeScoreSystem();
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
		case PvPCommon::GameMode::PvP_ComboExercise:
			return new IPVPGameModeScoreSystem();
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )
	}

	return NULL;
}

UINT CPvPScoreSystem::GetKillCount( DnActorHandle hActor )
{
	if( !hActor || !hActor->GetName() )
		return 0;

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( hActor->GetName() );
	if( itor == m_mCount.end() )
		return 0;

	return (*itor).second.uiKillCount;
}

UINT CPvPScoreSystem::GetKillCount( const WCHAR * pName )
{
	if (pName == NULL || pName[0] == '\0')
		return 0;

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find(pName);
	if( itor == m_mCount.end() )
		return 0;

	return (*itor).second.uiKillCount;
}

UINT CPvPScoreSystem::GetDeathCount( DnActorHandle hActor )
{
	if( !hActor || !hActor->GetName() )
		return 0;

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( hActor->GetName() );
	if( itor == m_mCount.end() )
		return 0;

	return (*itor).second.uiDieCount;
}

UINT CPvPScoreSystem::GetTotalScore( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return 0;

	SMyScore* pMyScore = FindMyScoreData( hActor->GetName() );
	if( !pMyScore )
		return 0;

	return pMyScore->uiKillScore + pMyScore->uiAssistScore;
}

UINT CPvPScoreSystem::GetKillScore( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return 0;

	SMyScore* pMyScore = FindMyScoreData( hActor->GetName() );
	if( !pMyScore )
		return 0;

	return pMyScore->uiKillScore;
}

UINT CPvPScoreSystem::GetAssistScore( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return 0;

	SMyScore* pMyScore = FindMyScoreData( hActor->GetName() );
	if( !pMyScore )
		return 0;

	return pMyScore->uiAssistScore;
}

UINT CPvPScoreSystem::GetXPScore( DnActorHandle hActor, const UINT uiWinTeam )
{
#ifdef _GAMESERVER

	if( !hActor || !hActor->IsPlayerActor() )
		return 0;

	CDNGameRoom* pGameRoom = hActor->GetGameRoom();
	if( !pGameRoom )
		return 0;
	CPvPGameMode* pPvPGameMode = pGameRoom->GetPvPGameMode();
	if( !pPvPGameMode )
		return 0;

	// PlayTime 계산
	UINT uiPlayTimeSec = pPvPGameMode->GetPlayTime( hActor->GetName() );
	if( uiPlayTimeSec == 0 )
		return 0;
	int iPlayRound = pPvPGameMode->GetPlayRound();
	if( iPlayRound <= 0 )
		return 0;

	UINT uiTotalPlayTime	= iPlayRound * pPvPGameMode->GetSelectPlayTime();
	UINT uiPlayTimePercent	= uiPlayTimeSec * 100 / uiTotalPlayTime;

	UINT uiDefaultExp	= 0;
	UINT uiBonusExp		= 0;

	uiDefaultExp = hActor->GetTeam() == uiWinTeam ? m_uiVictoryExp : m_uiDefeatExp;

#if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )
	UINT uiPenaltyPercent = 0;
	pPvPGameMode->GetFinishRoundPenalty( uiWinTeam, uiPenaltyPercent );
	uiDefaultExp = uiDefaultExp * (100 - uiPenaltyPercent) / 100;
#endif // #if defined( PRE_MOD_PVP_ROUNDMODE_PENALTY )

	if (m_uiGameMode == PvPCommon::GameMode::PvP_Occupation)
	{
		if( hActor->GetTeam() == uiWinTeam )
			uiBonusExp = GetMyOccupationScore(hActor->GetName()) * m_uiVictoryBonusRate /100;
		else
			uiBonusExp = GetMyOccupationScore(hActor->GetName()) * m_uiDefeatBonusRate / 100;
	}
	else
	{
		SMyScore* pMyScore = FindMyScoreData( hActor->GetName() );
		if( pMyScore )
		{
			// 이겼을 때
			if( hActor->GetTeam() == uiWinTeam )			
				uiBonusExp = pMyScore->GetScore() * m_uiVictoryBonusRate /100;
			// 졌을 때
			else			
				uiBonusExp = pMyScore->GetScore() * m_uiDefeatBonusRate / 100;
		}
	}
	uiDefaultExp = uiDefaultExp * uiPlayTimePercent / 100;
	uiBonusExp	 = (uiBonusExp > uiDefaultExp ) ? uiDefaultExp : uiBonusExp;

	return uiDefaultExp + uiBonusExp;

#else // #ifdef _GAMESERVER

	// 클라이언트에서는 XP를 별도의 프로토콜로 XP를 알릴 예정이다.
	return 0;

#endif // #ifdef _GAMESERVER
}

const void* CPvPScoreSystem::GetScoreStruct( const WCHAR* pwszCharName )
{
	const void* pScoreStruct = FindMyScoreData( pwszCharName );
	return pScoreStruct;
}

#ifdef _GAMESERVER

#if defined(PRE_MOD_PVP_LADDER_XP)
UINT CPvPScoreSystem::CalcXPScore( CDNUserSession* pGameSession, const UINT uiWinTeam, PvPCommon::QueryUpdatePvPDataType::eCode Type )
{
	DnActorHandle hActor = pGameSession->GetActorHandle();
	if(!hActor) return 0;

	if( Type != PvPCommon::QueryUpdatePvPDataType::FinishGameMode )
		return 0;

	// 패스워드 방은 경험치를 주지 않는다.
	bool IsPwRoom = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsPWRoom();
	if( IsPwRoom )
		return 0;

	// PvP_XP
	UINT uiAddXP = GetXPScore( hActor, uiWinTeam );

#if defined( _CH )
	if( pGameSession->GetFCMState() == FCMSTATE_HALF)
		uiAddXP /= 2;
	else if( pGameSession->GetFCMState() == FCMSTATE_ZERO)
		uiAddXP = 0;
#endif

	int nEventID = 0;
	nEventID = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->GetEventRoomIndex();
	// 이벤트 방 보상 ( 콜로세움 XP 10% )
	if( nEventID > 0 )
		uiAddXP += (uiAddXP/10);

	int nGuildRewardValue = pGameSession->GetGuildRewardItemValue( GUILDREWARDEFFECT_TYPE_EXTRAPVPEXP );
	if( nGuildRewardValue > 0 )
	{
		uiAddXP += (int)(uiAddXP * (nGuildRewardValue * 0.01));
	}

	return uiAddXP;
}
#endif

void CPvPScoreSystem::SendScore( CDNUserSession* pGameSession )
{
	UINT uiATeamScore,uiBTeamScore;
	GetGameModeScore( uiATeamScore, uiBTeamScore );

	if( m_mScore.size() > PvPCommon::Common::MaxPlayer )
	{
		_DANGER_POINT();
		return;
	}

	if( m_mCount.size() > PvPCommon::Common::MaxPlayer )
	{
		_DANGER_POINT();
		return;
	}
	
	//

	SCPVP_MODE_SCORE TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.unATeamScore	= static_cast<USHORT>(uiATeamScore);
	TxPacket.unBTeamScore	= static_cast<USHORT>(uiBTeamScore);
	TxPacket.cScoreCount	= static_cast<BYTE>(m_mScore.size());
	TxPacket.cCount			= static_cast<BYTE>(m_mCount.size());

	char* pStartBuffer	= reinterpret_cast<char*>(TxPacket.sPvPUserScore);
	char* pBuffer		= pStartBuffer;

	for( std::map<std::wstring,SMyScore>::iterator itor=m_mScore.begin() ; itor!=m_mScore.end() ; ++itor )
	{
		TPvPUserScore* pUserScore = reinterpret_cast<TPvPUserScore*>(pBuffer);

		pUserScore->uiSessionID				= (*itor).second.uiSessionID;
		pUserScore->uiKillScore				= (*itor).second.uiKillScore;
		pUserScore->uiAssistScore			= (*itor).second.uiAssistScore;
		for( UINT i=0 ; i<PvPCommon::Common::MaxClass ; ++i )
		{
			pUserScore->uiKillClassScore[i]	= (*itor).second.sKillClassScore.uiScore[i];
			pUserScore->uiKilledClassScore[i] = (*itor).second.sKilledClassScore.uiScore[i];
		}

		pBuffer += sizeof(TPvPUserScore);
	}

	for( std::map<std::wstring,SCount>::iterator itor=m_mCount.begin() ; itor!=m_mCount.end() ; ++itor )
	{
		TPvPUserCount* pCount = reinterpret_cast<TPvPUserCount*>(pBuffer);

		pCount->uiSessionID				= (*itor).second.uiSessionID;
		pCount->uiKillCount				= (*itor).second.uiKillCount;
		pCount->uiDieCount				= (*itor).second.uiDieCount;
		pCount->uiContinuousKillCount	= (*itor).second.uiContinuousKillCount;

		pBuffer += sizeof(TPvPUserCount);
	}

	int iSize = static_cast<int>(sizeof(SCPVP_MODE_SCORE)-sizeof(TxPacket.sPvPUserScore)-sizeof(TxPacket.sPvPUserCount)+(pBuffer-pStartBuffer));

	pGameSession->AddSendData( SC_PVP, ePvP::SC_MODE_SCORE, reinterpret_cast<char*>(&TxPacket), iSize );
}

void CPvPScoreSystem::SendXPScore( CDNGameRoom* pGameRoom, CDNUserSession* pGameSession )
{
	if( !m_bCreateXPScorePacket )
	{
		if( m_mScore.size() > PvPCommon::Common::MaxPlayer )
		{
			_DANGER_POINT();
			return;
		}

		int Seq;

		char* pStartBuffer	= reinterpret_cast<char*>(m_XPScorePacket.sXPScoreArr);
		char* pBuffer		= pStartBuffer;

		for( std::map<std::wstring,SMyScore>::iterator itor=m_mScore.begin() ; itor!=m_mScore.end() ; ++itor )
		{
			TPvPUserXPScore* pXPScore = reinterpret_cast<TPvPUserXPScore*>(pBuffer);

			CDNGameRoom::PartyStruct* pStruct = pGameRoom->GetPartyDatabySessionID( (*itor).second.uiSessionID, Seq );
			if( !pStruct )
				continue;

			DnActorHandle hActor = pStruct->pSession->GetActorHandle();
			if( !hActor )
				continue;

			pXPScore->uiSessionID		= (*itor).second.uiSessionID;
			if( pGameRoom->bIsLadderRoom() )
			{
#if defined(PRE_ADD_DWC)
				CDNPvPGameRoom *pPvPGameRoom = static_cast<CDNPvPGameRoom*>(pGameRoom);
				if(pPvPGameRoom->bIsDWCMatch())
				{
					if(pPvPGameRoom->GetPvPLadderMatchType() == LadderSystem::MatchType::_3vs3_DWC)
					{
						pXPScore->iAddLadderGradePoint		= pPvPGameRoom->GetDWCStatsRepositoryPtr()->GetAddGradePoint( pStruct->pSession->GetTeam() );
						pXPScore->uiResultLadderGradePoint	= pPvPGameRoom->GetDWCStatsRepositoryPtr()->GetResultGradePoint( pStruct->pSession->GetTeam() );
						pXPScore->uiGainMedalCount			= 0;
#if defined(PRE_MOD_PVP_LADDER_XP)
						pXPScore->uiAddLadderXPScore		= 0;
#endif
					}
					else
					{
						pXPScore->iAddLadderGradePoint = 0;
						pXPScore->uiResultLadderGradePoint = 0;
						pXPScore->uiGainMedalCount = 0;
						pXPScore->uiAddLadderXPScore = 0;
					}
				}
				else
				{
#endif
					pXPScore->iAddLadderGradePoint		= static_cast<CDNPvPGameRoom*>(pGameRoom)->GetLadderStatsRepositoryPtr()->GetAddGradePoint( pStruct->pSession->GetCharacterDBID() );
					pXPScore->uiResultLadderGradePoint	= static_cast<CDNPvPGameRoom*>(pGameRoom)->GetLadderStatsRepositoryPtr()->GetResultGradePoint( pStruct->pSession->GetCharacterDBID() );
					pXPScore->uiGainMedalCount			= 0;
#if defined(PRE_MOD_PVP_LADDER_XP)
					pXPScore->uiAddLadderXPScore		= (*itor).second.uiAddXP;
#endif
#if defined(PRE_ADD_DWC)
				}
#endif
			}
			else
			{
				pXPScore->uiAddXPScore		= (*itor).second.uiAddXP;
				pXPScore->uiGainMedalCount	= (*itor).second.uiGainMedalCount;
				pXPScore->uiResultXPScore	= pStruct->pSession->GetPvPData()->uiXP;
#if defined(PRE_MOD_PVP_LADDER_XP)
				pXPScore->uiAddLadderXPScore = 0;
#endif
			}
			pBuffer += sizeof(TPvPUserXPScore);
			++m_XPScorePacket.cCount;
		}

		m_bCreateXPScorePacket = true;
	}

	int iSize = static_cast<int>(sizeof(SCPVP_XPSCORE)-sizeof(m_XPScorePacket.sXPScoreArr)+m_XPScorePacket.cCount*sizeof(TPvPUserXPScore));

	pGameSession->AddSendData( SC_PVP, ePvP::SC_XPSCORE, reinterpret_cast<char*>(&m_XPScorePacket), iSize );
}

bool CPvPScoreSystem::QueryUpdatePvPData( const UINT uiWinTeam, CDNUserSession* pGameSession, PvPCommon::QueryUpdatePvPDataType::eCode Type )
{
	if( !pGameSession || !pGameSession->GetPvPData() || !g_pDBConnectionManager || !pGameSession->GetGameRoom() )
	{
		_DANGER_POINT();
		return false;
	}

	bool bIsLadderRoom = false;
	bIsLadderRoom = pGameSession->GetGameRoom()->bIsLadderRoom();

#if defined(PRE_ADD_DWC)
	if(bIsLadderRoom && static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsDWCMatch())
		return true;
	CPvPGameMode * pPvPGameMode = pGameSession->GetGameRoom()->GetPvPGameMode();
	if( pPvPGameMode && pPvPGameMode->GetPvPChannelType() == PvPCommon::RoomType::dwc )
		return true;
#endif

	// ActorHandle 유효성 검사
	DnActorHandle hActor = pGameSession->GetActorHandle();
	if( !hActor )
		return false;

	// 중복 쿼리 방지
	if( std::find( m_listQueryCharacterDBID.begin(), m_listQueryCharacterDBID.end(), pGameSession->GetCharacterDBID() ) != m_listQueryCharacterDBID.end() )
		return true;

#if defined( PRE_FIX_76282 )
	// 중복쿼리 방지로 막았더니 정상적인 유저가 결과창을 보지 못하는 버그가 발생하여 수정 함.(#81131)
	if( PvPCommon::QueryUpdatePvPDataType::Disconnect == Type )
	{
		CPvPGameMode * pPvPGameMode = pGameSession->GetGameRoom()->GetPvPGameMode();
		if( pPvPGameMode && ( pPvPGameMode->bIsZombieMode() || pPvPGameMode->bIsRacingMode() || pPvPGameMode->bIsComboExerciseMode() ) )
		{
			return true;
		}
		g_Log.Log( LogType::_ERROR, pGameSession->GetWorldSetID(), pGameSession->GetAccountDBID(), pGameSession->GetCharacterDBID(), pGameSession->GetSessionID(), L"[ADBID:%u CDBID:%u SID:%u] QueryUpdatePvPData when Disconnect!!\r\n", pGameSession->GetAccountDBID(), pGameSession->GetCharacterDBID(), pGameSession->GetSessionID() );
	}
#endif // #if defined( PRE_FIX_76282 )

	CDNDBConnection* pDBCon = pGameSession->GetDBConnection();
	if( !pDBCon)
	{
		g_Log.Log( LogType::_ERROR, pGameSession->GetWorldSetID(), pGameSession->GetAccountDBID(), pGameSession->GetCharacterDBID(), pGameSession->GetSessionID(), L"[ADBID:%u CDBID:%u SID:%u] QueryUpdatePvPData DBConnection NULL!!\r\n", pGameSession->GetAccountDBID(), pGameSession->GetCharacterDBID(), pGameSession->GetSessionID() );
		return false;
	}

	if( bIsLadderRoom == true )	
		return _QueryUpdateLadderData( pDBCon, pGameSession, uiWinTeam, Type );
	else	
		return _QueryUpdatePvPData( pDBCon, pGameSession, uiWinTeam, Type );
	return true;
}

bool CPvPScoreSystem::_QueryUpdatePvPData( CDNDBConnection* pDBCon, CDNUserSession* pGameSession, UINT uiWinTeam, PvPCommon::QueryUpdatePvPDataType::eCode Type )
{
	SMyScore*	pMyScore = FindMyScoreData( pGameSession->GetCharacterName() );
	TPvPGroup*	pPvPData = pGameSession->GetPvPData();

	DnActorHandle hActor = pGameSession->GetActorHandle();

#if defined(PRE_MOD_PVP_LADDER_XP)
	// PvP_XP
	UINT uiAddXP = CalcXPScore( pGameSession, uiWinTeam, Type );

	pPvPData->uiXP += uiAddXP;
	if( pMyScore )
		pMyScore->uiAddXP = uiAddXP;

	// PvP_Level
	pGameSession->UpdatePvPLevel();

	bool IsPwRoom = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsPWRoom();
	int nEventID = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->GetEventRoomIndex();

#else
	// PvP_XP
	UINT uiAddXP = GetXPScore( hActor, uiWinTeam );
	if( Type != PvPCommon::QueryUpdatePvPDataType::FinishGameMode )
		uiAddXP = 0;

#if defined( _CH )
	if( pGameSession->GetFCMState() == FCMSTATE_HALF)
		uiAddXP /= 2;
	else if( pGameSession->GetFCMState() == FCMSTATE_ZERO)
		uiAddXP = 0;
#endif

	// 패스워드 방은 경험치를 주지 않는다.
	bool IsPwRoom = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsPWRoom();
	if( IsPwRoom )
		uiAddXP = 0;

	int nEventID = 0;
	nEventID = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->GetEventRoomIndex();
#if defined( PRE_WORLDCOMBINE_PVP )
	// 월드통합PVP룸도 이벤트룸과 같은 보상임 혹시 보상이 틀려지면 따로 작업분리해야함
	if( pGameSession->GetGameRoom()->bIsWorldPvPRoom() )
		nEventID = 1;
#endif
	// 이벤트 방 보상 ( 콜로세움 XP 10% )
	if( nEventID > 0 )
		uiAddXP += (uiAddXP/10);

	int nGuildRewardValue = pGameSession->GetGuildRewardItemValue( GUILDREWARDEFFECT_TYPE_EXTRAPVPEXP );
	if( nGuildRewardValue > 0 )
	{
		uiAddXP += (int)(uiAddXP * (nGuildRewardValue * 0.01));
	}

	pPvPData->uiXP += uiAddXP;
	if( pMyScore )
		pMyScore->uiAddXP = uiAddXP;

	// PvP_Level
	for( UINT i=pPvPData->cLevel ; i<PvPCommon::Common::MaxRank ; ++i )
	{
		const TPvPRankTable* pPvPRankTable = g_pDataManager->GetPvPRankTable( i );
		if( !pPvPRankTable )
		{
			_DANGER_POINT();
			break;
		}

#ifdef PRE_MOD_PVPRANK
		if (pPvPRankTable->cType != PvPCommon::RankTable::ExpValue)
			continue;

		if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
			pGameSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
			pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
		else
			break;
#else		//#ifdef PRE_MOD_PVPRANK
		if( pPvPData->uiXP >= pPvPRankTable->uiXP )
#if defined(PRE_ADD_PVPLEVEL_MISSION)
			pGameSession->SetPvPLevel(static_cast<BYTE>(i+1));
#else
			pPvPData->cLevel = static_cast<BYTE>(i+1);
#endif
		else
			break;
#endif		//#ifdef PRE_MOD_PVPRANK
	}
#endif	//#if defined(PRE_MOD_PVP_LADDER_XP)

	// PvP_GamePlayTimeSec
	CPvPGameMode* pPvPGameMode = pGameSession->GetGameRoom()->GetPvPGameMode();
	if( pPvPGameMode == NULL )
	{
		_DANGER_POINT();
		return false;
	}
	pPvPData->uiPlayTimeSec += pPvPGameMode->GetPlayTime( pGameSession->GetCharacterName() );

	// 정상종료인 경우...
	if( Type == PvPCommon::QueryUpdatePvPDataType::FinishGameMode )
	{
		// 개인전은 승리만 기록한다.
		if( pPvPGameMode->GetPvPGameModeTable()->uiGameMode == PvPCommon::GameMode::PvP_IndividualRespawn )
		{
			if( uiWinTeam != PvPCommon::Team::Max )
			{
				if( uiWinTeam == hActor->GetTeam() )
					++pPvPData->uiWin;
			}
		}
		else
		{
			// PvP_Win,PvP_Lose,PvP_Draw
			if( uiWinTeam == PvPCommon::Team::Max )
				++pPvPData->uiDraw;
			else
			{
				if( uiWinTeam == hActor->GetTeam() )
					++pPvPData->uiWin;
				else
					++pPvPData->uiLose;
			}
		}
	}
	else
	{
		// PvP_GiveUpCount
		++pPvPData->uiGiveUpCount;
	}

	if( pMyScore )
	{
		// PvP_KO_*,PvP_KO_by_*
		for( UINT i=0 ; i<PvPCommon::Common::MaxClass ; ++i )
		{
			pPvPData->uiKOClassCount[i]		+= pMyScore->sKillClassScore.uiScore[i];
			pPvPData->uiKObyClassCount[i]	+= pMyScore->sKilledClassScore.uiScore[i];
		}

		// PvP_KillPoint
		pPvPData->biTotalKillPoint += pMyScore->uiKillScore;
		// PvP_AssistPoint
		pPvPData->biTotalAssistPoint += pMyScore->uiAssistScore;
	}

	bool bQuery = true;
	if( pPvPGameMode->bIsZombieMode() )
		bQuery = false;
#if defined( PRE_ADD_RACING_MODE)
	if( pPvPGameMode->bIsRacingMode() )
		bQuery = false;
#endif
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	if( pPvPGameMode->bIsComboExerciseMode() )
		bQuery = false;
#endif // #if defined( PRE_ADD_PVP_COMBOEXERCISE )

	if( bQuery && pDBCon->QueryUpdatePvPData( pGameSession->GetDBThreadID(), pGameSession, Type ) == false )
	{
		_DANGER_POINT();
		return false;
	}

	if( bQuery )
		m_listQueryCharacterDBID.push_back( pGameSession->GetCharacterDBID() );

	const TPvPGameModeTable* pPvPGameModeTable = pPvPGameMode->GetPvPGameModeTable();
	int nSubmitCount = 0;
	bool bFatigueMedal = false;
	int nGainExp = 0, nGainMedal = 0;

	// 패스워드 방은 피로도 소모도 하지 않음.
	if( !IsPwRoom && pGameSession->GetPvPFatigueOption() && pPvPGameModeTable && pPvPGameModeTable->nFatigueConTime > 0)	
	{
		// 시간은 초임..
		UINT uiPlayTime = pPvPGameMode->GetPlayTime( pGameSession->GetCharacterName() );
		nSubmitCount = uiPlayTime / (pPvPGameModeTable->nFatigueConTime*60);
		int nNeedFatigue = nSubmitCount*pPvPGameModeTable->nFatigueConValue;
		int nAllFatigue = pGameSession->GetAllFatigue();
		if( nSubmitCount > 0 && nAllFatigue > 0)
		{			
			if( nNeedFatigue > nAllFatigue && pPvPGameModeTable->nFatigueConValue ) // 피로도 충분하지 않으면 있는걸 다 까고 최소단위 보상.
			{
				nSubmitCount = (nAllFatigue/pPvPGameModeTable->nFatigueConValue)+1;
				nNeedFatigue = nAllFatigue;
			}
			float fPercent = (pPvPGameModeTable->nFatigueConExp*nSubmitCount)/1000.f;
			nGainExp = static_cast<int>(GetPlayerLevelTable().GetValue( hActor->OnGetJobClassID(), hActor->GetLevel(), CPlayerLevelTable::LevelDExp ) * fPercent);
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
			TExpData ExpData;
			ExpData.set( (float)nGainExp );
			pPlayerActor->CmdAddExperience(ExpData, DBDNWorldDef::CharacterExpChangeCode::PvP, 0);
			pGameSession->DecreaseFatigue(nNeedFatigue);
			bFatigueMedal = true;
		}
	}

	// 아이템 지급	
	if( pGameSession->GetItem() )
	{
		const TPvPGameModeTable* pGameModeTable = pPvPGameMode->GetPvPGameModeTable();
		_ASSERT( pGameModeTable );
#if defined(PRE_ADD_PVP_TOURNAMENT)
		int nRewardItemID = 0;
		int nRewardItemCount = 0;
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
#if defined(PRE_ADD_RACING_MODE)
		int iCount = 0;
		if( pPvPGameMode->bIsRacingMode() )
		{
			BYTE cRank = pPvPGameMode->GetRacingRanking(pGameSession->GetSessionID());
			if( cRank != 0 && pGameModeTable->nDefaultRewardValue > 0 )			
				iCount = pGameModeTable->nDefaultRewardValue;
			if( cRank != 0 && cRank <= 8 && cRank <= pPvPGameModeTable->vRankRewardValue.size() ) // 1위부터 8등까지만
			{
				if( pPvPGameModeTable->vRankRewardValue[cRank-1] > 0 )
					iCount += pPvPGameModeTable->vRankRewardValue[cRank-1];
			}
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		else if( pPvPGameMode->bIsTournamentMode() )
		{
			BYTE cRank = pPvPGameMode->GetTournamentRanking(pGameSession->GetSessionID());
			if( cRank != 0)
			{
				// 킬수가 0이면 보상 주지 않는다.
				if( pPvPGameModeTable->vRankRewardItem[cRank-1] > 0 && GetKillCount(pGameSession->GetActorHandle()) > 0)
				{
					nRewardItemID = pPvPGameModeTable->vRankRewardItem[cRank-1];
					if( pPvPGameModeTable->vRankRewardValue[cRank-1] > 0 )
						nRewardItemCount += pPvPGameModeTable->vRankRewardValue[cRank-1];
				}				
			}
		}
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT)
		else
			iCount = uiAddXP/pGameModeTable->uiMedalExp;
#else // #if defined(PRE_ADD_RACING_MODE)
		int iCount = uiAddXP/pGameModeTable->uiMedalExp;
#endif

		// #17383 [PVP] 킬 수가 1이상일 때 획득 메달이 0이 아닌 1로 수정 요청
		if( iCount <= 0 )
		{
			if( GetKillCount(pGameSession->GetActorHandle()) > 0 )			
				iCount = 1;
		}
#if defined(PRE_ADD_REVENGE)
		if( pMyScore && pMyScore->bRevengeSucess )	// #62999 리벤지 성공한 경우 메달 추가 획득 (이벤트방 보상에 적용됨)		
			iCount += Revenge::RevengeRewardMedalCount;
#endif
		// 패스워드 방은 메달을 주지 않는다.
		if( IsPwRoom )
			iCount = 0;

		// 이벤트방 보상( 메달획등량 50% 증가 )
		if( nEventID > 0 )
			iCount += (iCount/2);
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		if (hActor && hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_261))
		{
			DNVector(DnBlowHandle) vlBlows;
			int nTotalLevelSkillAddMedal = 0;
			hActor->GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_261, vlBlows);

			int nBlowCount = (int)vlBlows.size();
			for (int i = 0; i < nBlowCount; ++i)
			{
				DnBlowHandle hBlow = vlBlows[i];
				if (hBlow && hBlow->IsEnd() == false)
				{
					CDnAddMedalBlow* pMedalBlow = static_cast<CDnAddMedalBlow*>(hBlow.GetPointer());
					nTotalLevelSkillAddMedal += pMedalBlow->GetAddMedal();
					int nTotalLevelSkillLimitMedal = pMedalBlow->GetLimitMedal();
					if( pGameSession->GetTotalLevelSkillMedalCount() + nTotalLevelSkillAddMedal > nTotalLevelSkillLimitMedal )
					{
						nTotalLevelSkillAddMedal = nTotalLevelSkillLimitMedal - pGameSession->GetTotalLevelSkillMedalCount();
						break;
					}
				}
			}
			if(nTotalLevelSkillAddMedal > 0)
			{
				iCount += nTotalLevelSkillAddMedal;
				pGameSession->AddTotalLevelSkillMedalCount(nTotalLevelSkillAddMedal);
			}
		}		
#endif
		if( bFatigueMedal )
		{
			nGainMedal = pPvPGameModeTable->nFatigueConMedal*nSubmitCount;
			iCount += nGainMedal;
		}
		if( iCount > 0 )
		{
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			if (pPvPGameMode->GetPvPChannelType() == PvPCommon::RoomType::beginner)
				iCount *= PvPCommon::Common::PvPBeginnerMedalMultipleFactor;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
			if( pMyScore )
				pMyScore->uiGainMedalCount = iCount;
			pGameSession->GetItem()->CreateInvenItem1( pPvPGameMode->GetPvPGameModeTable()->iRewardItemID, iCount, -1, -1, DBDNWorldDef::AddMaterializedItem::PvPReward, 0 );
		}
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( Type == PvPCommon::QueryUpdatePvPDataType::FinishGameMode && nRewardItemID > 0 && nRewardItemCount > 0)
			pGameSession->GetItem()->CreateInvenItem1( nRewardItemID, nRewardItemCount, -1, -1, DBDNWorldDef::AddMaterializedItem::PvPReward, 0 );
#endif // #if defined(PRE_ADD_PVP_TOURNAMENT) 
		if( bFatigueMedal )
			pGameSession->SendPvPFatigueReward(nGainExp, nGainMedal);
	}

	_QueryPvPEndLog( pDBCon, pGameSession, pPvPGameMode, pMyScore, Type );

	return true;
}

bool CPvPScoreSystem::_QueryUpdateLadderData( CDNDBConnection* pDBCon, CDNUserSession* pGameSession, UINT uiWinTeam, PvPCommon::QueryUpdatePvPDataType::eCode Type )
{
#if defined(PRE_ADD_DWC)
	if(static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->bIsDWCMatch())
		return true;
#endif

#if defined(PRE_MOD_PVP_LADDER_XP)
	SMyScore*	pMyScore = FindMyScoreData( pGameSession->GetCharacterName() );
	TPvPGroup*	pPvPData = pGameSession->GetPvPData();

	// PvP_XP
	UINT uiAddXP = CalcXPScore( pGameSession, uiWinTeam, Type );

	pPvPData->uiXP += uiAddXP;
	if( pMyScore )
		pMyScore->uiAddXP = uiAddXP;

	// PvP_Level
	pGameSession->UpdatePvPLevel();

	if( pDBCon->QueryUpdatePvPData( pGameSession->GetDBThreadID(), pGameSession, PvPCommon::QueryUpdatePvPDataType::FinishLadder ) == false )
	{
		_DANGER_POINT();
		//return false;
	}
#endif

	LadderSystem::CStatsRepository* pStatsRepository = static_cast<CDNPvPGameRoom*>(pGameSession->GetGameRoom())->GetLadderStatsRepositoryPtr();
	bool bRet = pStatsRepository->QueryUpdateResult( pGameSession, uiWinTeam, Type );

#if !defined(PRE_MOD_PVP_LADDER_XP)
	SMyScore*		pMyScore	 = FindMyScoreData( pGameSession->GetCharacterName() );
#endif
	CPvPGameMode*	pPvPGameMode = pGameSession->GetGameRoom()->GetPvPGameMode();
	if( pPvPGameMode == NULL )
	{
		_ASSERT(0);
		return false;
	}

	m_listQueryCharacterDBID.push_back( pGameSession->GetCharacterDBID() );

	// PvP 종료시 캐릭터별 PvP 전적 로그
	_QueryPvPEndLog( pDBCon, pGameSession, pPvPGameMode, pMyScore, Type );

	return bRet;
}

void CPvPScoreSystem::_QueryPvPEndLog( CDNDBConnection* pDBCon, CDNUserSession* pGameSession, CPvPGameMode* pPvPGameMode, SMyScore* pMyScore, PvPCommon::QueryUpdatePvPDataType::eCode Type )
{
	if( pDBCon == NULL )
		return;

	INT64 biSNMain = 0;
	int nSNSub = 0;
	pGameSession->GetGameRoom()->GetPvPSN(biSNMain, nSNSub);

	DBDNWorldDef::PvPResultCode::eCode ResultCode = static_cast<DBDNWorldDef::PvPResultCode::eCode>(0);
	if( pPvPGameMode->GetWinTeam() == PvPCommon::Team::Max )
		ResultCode = DBDNWorldDef::PvPResultCode::Draw;
	else
	{
		ResultCode = (pPvPGameMode->GetWinTeam() == pGameSession->GetTeam()) ? DBDNWorldDef::PvPResultCode::Win : DBDNWorldDef::PvPResultCode::Lose;
	}

	if (Type != PvPCommon::QueryUpdatePvPDataType::FinishGameMode)
		ResultCode = DBDNWorldDef::PvPResultCode::GiveUp;
	BYTE cOccupationWinType = pPvPGameMode->GetFinishReason() == PvPCommon::FinishReason::OpponentTeamResourceLimit ? PvPCommon::OccupationLogWinType::LimitResource : PvPCommon::OccupationLogWinType::TimeOut;
	pDBCon->QueryAddPvPResultLog(pGameSession->GetDBThreadID(), pGameSession->GetWorldSetID(), pGameSession->GetAccountDBID(), biSNMain, nSNSub, pGameSession->GetCharacterDBID(), 
		pGameSession->GetTeam() == PvPCommon::Team::A ? DBDNWorldDef::PvPTeamCode::A : DBDNWorldDef::PvPTeamCode::B, pGameSession->bIsBreakIntoSession(),
		ResultCode, pPvPGameMode->GetPlayTime( pGameSession->GetCharacterName()), 
		pMyScore ? pMyScore->uiKillScore : 0, pMyScore ? pMyScore->uiAssistScore : 0, 0,
		pMyScore ? pMyScore->sKillClassScore.uiScore : NULL, pMyScore ? pMyScore->sKilledClassScore.uiScore : NULL, GetOccupationAcquireScore(pGameSession->GetActorHandle()), cOccupationWinType);
}
#endif // #ifdef _GAMESERVER

void CPvPScoreSystem::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage )
{
}

void CPvPScoreSystem::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return;

	if( hHitter && hHitter->IsMonsterActor() )
	{
		DnActorHandle hMaster = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
		if( hMaster && hMaster->IsPlayerActor() )
			hHitter = hMaster;
	}

	if( m_pGameModeScoreSystem )
		m_pGameModeScoreSystem->OnDie( hActor, hHitter );

	_UpdateCount( hActor, hHitter );

	// KillScore
	UINT uiAddKillScore = 0;
	SMyScore* pMyScore = FindMyScoreDataIfnotInsert( hHitter );
	if( pMyScore )
	{
		uiAddKillScore = GetPlayerLevelTable().GetValue( hHitter->OnGetJobClassID(), hHitter->GetLevel(), CPlayerLevelTable::KillScore );
		pMyScore->uiKillScore += uiAddKillScore;

		// ClassScore
		UINT uiActorType = hActor->GetActorType();
		if( uiActorType < PvPCommon::Common::MaxClass )
			++(pMyScore->sKillClassScore.uiScore[uiActorType]);
		else
			_DANGER_POINT();
	}

	OnNotify( hHitter, ScoreType::KillScore, uiAddKillScore );

	pMyScore = FindMyScoreDataIfnotInsert( hActor );
	if( pMyScore )
	{		
		// ClassScore
		if( hHitter )
		{
			UINT uiActorType = hHitter->GetActorType();
			if( uiActorType < PvPCommon::Common::MaxClass )
				++(pMyScore->sKilledClassScore.uiScore[uiActorType]);
			else
				_DANGER_POINT();
		}
	}
}

void CPvPScoreSystem::OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo )
{

}

void CPvPScoreSystem::OnLeaveUser( DnActorHandle hActor )
{
	if( !hActor || !hActor->GetName() || !hActor->IsPlayerActor() )
		return;

#ifdef _GAMESERVER
	CDnPlayerActor* pActor = static_cast<CDnPlayerActor*>(hActor.GetPointer() );
	if( pActor && pActor->GetUserSession() )
		m_listQueryCharacterDBID.remove( pActor->GetUserSession()->GetCharacterDBID() );
#endif

	m_mScore.erase( hActor->GetName() );
	m_mCount.erase( hActor->GetName() );
}

void CPvPScoreSystem::OnNotify( DnActorHandle hActor, const UINT uiScoreType, const UINT uiScore )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return;

	switch( m_uiGameMode )
	{
		case PvPCommon::GameMode::PvP_IndividualRespawn:
		{
			// 개인전에서는 AssistPoint 가 사용되지 않는다.
			if( uiScoreType == IScoreSystem::ScoreType::AssistScore )
				return;
			break;
		}
	}


#ifdef _GAMESERVER

	if( uiScoreType == IScoreSystem::ScoreType::AssistScore )
	{
		CDNGameRoom* pRoom = hActor->GetGameRoom();
		if( !pRoom )
		{
			_DANGER_POINT();
			return;
		}

		for( UINT i=0 ; i<pRoom->GetUserCount() ; ++i )
		{
			CDNUserSession* pSession = pRoom->GetUserData( i );
			if( !pSession )
				continue;

			pSession->SendPvPAddPoint( hActor->GetSessionID(), uiScoreType, uiScore );
		}
	}

#else

	WCHAR wszMessage[256];
	SecureZeroMemory(wszMessage,sizeof(wszMessage));
	
	switch( uiScoreType )
	{
		case ScoreType::KillScore:
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120407 ),uiScore);
			//GetInterface().AddPVPGameScore( hActor->GetUniqueID() , uiScore ) ;
			break;
		}
		case ScoreType::AssistScore:
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120408 ),uiScore);
			//GetInterface().AddPVPGameScore( hActor->GetUniqueID() , 0 ,uiScore) ;
			break;
		}
	}

	if( CDnActor::s_hLocalActor && hActor == CDnActor::s_hLocalActor )
		GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );

#endif // #ifdef _GAMESERVER
}

void CPvPScoreSystem::OnStartRound()
{
	for( std::map<std::wstring,SCount>::iterator itor=m_mCount.begin() ; itor!=m_mCount.end() ; ++itor )
	{
		(*itor).second.uiContinuousKillCount = 0;
	}
}

#if defined(PRE_ADD_REVENGE) && defined(_GAMESERVER)
void CPvPScoreSystem::OnRevengeSuccess(DnActorHandle hHitter)
{
	if( !hHitter || !hHitter->IsPlayerActor() )
		return;
	
	SMyScore* pMyScore = FindMyScoreDataIfnotInsert( hHitter );
	if( pMyScore )
		pMyScore->bRevengeSucess = true;
}
#endif

CPvPScoreSystem::SMyScore* CPvPScoreSystem::FindMyScoreData( const WCHAR* pwszName )
{
	if( !pwszName )
		return NULL;

	std::map<std::wstring,SMyScore>::iterator itor = m_mScore.find( pwszName );
	if( itor == m_mScore.end() )
		return NULL;
	
	return &itor->second;
}

CPvPScoreSystem::SMyScore* CPvPScoreSystem::FindMyScoreDataIfnotInsert( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return NULL;

	wchar_t* pwszName = hActor->GetName();
	if( !pwszName )
		return NULL;

	SMyScore* pMyScore = FindMyScoreData( pwszName );
	if( pMyScore )
		return pMyScore;
	
#ifdef _GAMESERVER
	UINT uiSessionID = hActor->GetSessionID();
	if( uiSessionID == 0 )
	{
		_DANGER_POINT();
		return NULL;
	}

	std::pair<std::map<std::wstring,SMyScore>::iterator,bool> Ret = m_mScore.insert( std::make_pair(pwszName,SMyScore(uiSessionID) ) );
#else
	std::pair<std::map<std::wstring,SMyScore>::iterator,bool> Ret = m_mScore.insert( std::make_pair(pwszName,SMyScore() ) );
#endif
	if( Ret.second )
		return &Ret.first->second;

	_DANGER_POINT();
	return NULL;
}

void CPvPScoreSystem::_UpdateCount( DnActorHandle hActor, DnActorHandle hHitter )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return;

	wchar_t* pwszName = hActor->GetName();
	if( !pwszName )
		return;

#ifdef _GAMESERVER
	UINT uiSessionID = hActor->GetSessionID();
	if( uiSessionID == 0 )
	{
		_DANGER_POINT();
		return;
	}
#endif

	// DieCount
	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( pwszName );
	if( itor == m_mCount.end() )
	{
#ifdef _GAMESERVER
		m_mCount.insert( std::make_pair(pwszName,SCount(uiSessionID,0,1,0)) );
#else
		m_mCount.insert( std::make_pair(pwszName,SCount(0,1,0)) );
#endif
	}
	else
	{
		++((*itor).second.uiDieCount);
		(*itor).second.uiContinuousKillCount = 0;	// 연속킬 리셋
	}

#if defined( _GAMESERVER )
	_UpdateAssistPoint( hActor, hHitter );
#endif // #if defined( _GAMESERVER )

	// KillCount
	if( hHitter && hHitter->GetName() && hHitter->IsPlayerActor() )
	{
		int iUpdateKillCount			= 1;
		int iUpdateContinuousKillCount	= 1;

		std::map<std::wstring,SCount>::iterator Localitor = m_mCount.find( hHitter->GetName() );
		if( Localitor == m_mCount.end() )
		{
#ifdef _GAMESERVER
			m_mCount.insert( std::make_pair(hHitter->GetName(),SCount(hHitter->GetSessionID(),1,0,1)) );
#else
			m_mCount.insert( std::make_pair(hHitter->GetName(),SCount(1,0,1)) );
#endif
		}
		else
		{
			iUpdateKillCount			= ++((*Localitor).second.uiKillCount);
			iUpdateContinuousKillCount	= ++((*Localitor).second.uiContinuousKillCount);
		}

#ifndef _GAMESERVER
		// 우선은 모드에 관계없이 나오도록 한다. 담에 바꿔야하면, 파생받아서 모드에 따라 달리 처리하면 될 듯.
		if( CDnActor::s_hLocalActor && hHitter == CDnActor::s_hLocalActor )
			GetInterface().GetHUD()->ShowKillMark( iUpdateContinuousKillCount, CDnPVPBaseHUD::KILLMARK_SHOW_TIME );

		if( iUpdateContinuousKillCount >= 5 ) {
			WCHAR wszTemp[128];
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121048 ), hHitter->GetName(), iUpdateContinuousKillCount );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
		}
#endif // #ifndef _GAMESERVER
	}
}

#if defined( _GAMESERVER )

void CPvPScoreSystem::_UpdateAssistPoint( DnActorHandle hActor, DnActorHandle hHitter )
{
	_ASSERT( dynamic_cast<CDNPvPPlayerAggroSystem*>(hActor->GetAggroSystem()) != NULL );

	CDNPvPPlayerAggroSystem* pAggroSystem = static_cast<CDNPvPPlayerAggroSystem*>(hActor->GetAggroSystem());
	pAggroSystem->Die( this, hHitter );
}

#endif // #if defined( _GAMESERVER )


UINT CPvPScoreSystem::_GetDieCount( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() || !hActor->GetName() )
		return 0;

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( hActor->GetName() );
	if( itor == m_mCount.end() )
		return 0;

	return (*itor).second.uiDieCount;
}

UINT CPvPScoreSystem::_GetContinuousKillCount( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() || !hActor->GetName() )
		return 0;

	std::map<std::wstring,SCount>::iterator itor = m_mCount.find( hActor->GetName() );
	if( itor == m_mCount.end() )
		return 0;

	return (*itor).second.uiContinuousKillCount;
}
