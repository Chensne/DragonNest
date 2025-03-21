
#include "stdafx.h"
#include "PvPZombieMode.h"
#include "DnActor.h"
#include "DNUserSession.h"
#include "DNPvPGameRoom.h"
#include "DnPlayerActor.h"
#include "DnStateBlow.h"
#include "DNGameDataManager.h"
#include "DNDBConnection.h"
#include "DNMissionSystem.h"

CPvPZombieMode::CPvPZombieMode( CDNGameRoom* pGameRoom, const TPvPGameModeTable* pPvPGameModeTable, const MAGAPVP_GAMEMODE* pPacket )
:CPvPRoundMode( pGameRoom, pPvPGameModeTable, pPacket )
{
	memset(&m_MutationGroup, 0, sizeof(TMonsterMutationGroup));
	m_nGhoulCount = 0;
	m_bInitializeZombie = false;
}

CPvPZombieMode::~CPvPZombieMode()
{

}

void CPvPZombieMode::OnInitializeActor( CDNUserSession* pSession )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return;

	if( pSession->GetTeam() == PvPCommon::Team::Observer )
		return;

	hActor->SetTeam( PvPCommon::Team::Human );
	pSession->SetTeam( hActor->GetTeam() );

	_AddHumanBlow( hActor );
}

void CPvPZombieMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	_ProcessSelectZombie();

	CPvPRoundMode::Process( LocalTime, fDelta );

	// Playing 중일 때 상대방 나갔는지 검사
	if( bIsPlaying() )
	{
		OnCheckFinishRound( PvPCommon::FinishReason::OpponentTeamAllGone );
	}
}

void CPvPZombieMode::OnFinishProcessDie( DnActorHandle hActor )
{
	if( !hActor )
		return;

	if( bIsPlaying() != true )
		return;

	if( bIsPlayingUser( hActor) == false )
		return;

	// 리스폰위치 설정
	if( bIsZombie( hActor ) )
		_SetRespawnPosition( hActor );

	// 부활시킴
	UINT uiRespawnHPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnHPPercent			: 100;
	UINT uiRespawnMPPercent = GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnMPPercent			: 100;
	UINT uiImmortalTime		= GetPvPGameModeTable() ? GetPvPGameModeTable()->uiRespawnNoDamageTimeSec	: 5;

	char szParam[32];
	sprintf_s( szParam, "Coin/%d/%d/%d", uiRespawnHPPercent, uiRespawnMPPercent, uiImmortalTime );
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_057, uiImmortalTime*1000, szParam );
}

void CPvPZombieMode::OnCheckFinishRound( PvPCommon::FinishReason::eCode Reason )
{
	UINT uiWinTeam = PvPCommon::Team::Max;

	switch( Reason )
	{
		// 좀비,유저 한쪽이 다 나갔으면 종료 처리
		case PvPCommon::FinishReason::OpponentTeamAllGone:
		{
			if( m_bInitializeZombie == false )
				return;

			int iPlayerCount = 0;
			int iZombieCount = 0;
			for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
			{
				CDNUserSession* pSession	= GetGameRoom()->GetUserData(i);
				DnActorHandle	hActor		= pSession->GetActorHandle();
				if( bIsPlayingUser( hActor ) == false )
					continue;

				if( hActor->GetTeam() == PvPCommon::Team::A )
					++iPlayerCount;
				else if( hActor->GetTeam() == PvPCommon::Team::Zombie )
					++iZombieCount;
			}

			// 유저,좀비 둘 다 살아있음요~!
			if( iPlayerCount > 0 && iZombieCount > 0 )
				return;

			if( iPlayerCount == 0 && iZombieCount > 0 )
				uiWinTeam = PvPCommon::Team::Zombie;
			else if( iZombieCount == 0 && iPlayerCount > 0 )
				uiWinTeam = PvPCommon::Team::A;

			CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			return;
		}
		// 좀비모드에서는 TimeOver 시 유저가 무조건 승리한다.
		case PvPCommon::FinishReason::TimeOver:
		{
			uiWinTeam = PvPCommon::Team::A;
			CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			return;
		}
		// 좀비모드에서는 좀비가 다 죽어있는 상태인가만 검사한다.
		case PvPCommon::FinishReason::OpponentTeamAllDead:
		{
			if( m_bInitializeZombie == false )
				return;

			int iZombieCount = 0;
			int iZombieDeadCount = 0;
			for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
			{
				CDNUserSession* pSession	= GetGameRoom()->GetUserData(i);
				DnActorHandle	hActor		= pSession->GetActorHandle();
				if( bIsPlayingUser( hActor ) == false )
					continue;

				if( hActor->GetTeam() == PvPCommon::Team::Zombie )
				{
					++iZombieCount;
					if( hActor->IsDie() )
						++iZombieDeadCount;
				}
			}

			if( iZombieCount <= 0 )
				return;

			if( iZombieCount == iZombieDeadCount )
				uiWinTeam = PvPCommon::Team::Human;
			else
				return;

			CPvPRoundMode::_ProcessFinishRound( uiWinTeam, Reason );
			return;
		}
	}

	CPvPRoundMode::OnCheckFinishRound( Reason );
}

void CPvPZombieMode::OnFinishRound()
{
	
}

void CPvPZombieMode::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	CPvPGameMode::OnDie( hActor, hHitter );

	// 인간->좀비 이면 바로 변신시킨다
	if( hActor && hActor->IsPlayerActor() && static_cast<CDnPlayerActor*>(hActor.GetPointer())->IsTransformMode() == false )
		hActor->SetDieDelta( 0.f );
}

void CPvPZombieMode::OnLeaveUser( DnActorHandle hActor )
{
	if( hActor )
		m_mZombieActor.erase( hActor->GetUniqueID() );
	CPvPRoundMode::OnLeaveUser( hActor );
	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( pPlayer )
	{		
		CDNUserSession* pSession = pPlayer->GetUserSession();
		if( pSession->GetAddGhoulScore(GhoulMode::PointType::GhoulKill) || pSession->GetAddGhoulScore(GhoulMode::PointType::HumanKill) || pSession->GetAddGhoulScore(GhoulMode::PointType::HolyWaterUse) )
		{
			pSession->GetDBConnection()->QueryAddPVPGhoulScores(pSession);
			pSession->ClearAddGhoulScore();
		}		
	}
}

void CPvPZombieMode::OnSuccessBreakInto( CDNUserSession* pGameSession )
{
	CPvPRoundMode::OnSuccessBreakInto( pGameSession );

	// 난입 유저에게 좀비 정보 알려준다.
	pGameSession->SendPvPSelectZombie( m_mZombieActor );
}

void CPvPZombieMode::OnRebirth( DnActorHandle hActor, bool bForce/*=false*/ )
{
	TMonsterMutationData Data;
	memset(&Data, 0, sizeof(TMonsterMutationData));
	if( GetZombieActorData(Data) == false )
		_ASSERT(0);
	if( bIsZombie(hActor) )
	{
		_AddZombieBlow( hActor, Data );
	}
	else
	{
		_ChangeZombie( hActor, Data );
	}
}

void CPvPZombieMode::_ProcessSelectZombie()
{
	if( m_bInitializeZombie )
		return;

	if( bIsPlaying() == false )
		return;
	
	// 10초가 지난후부터 Zombie 설정된다.
	if( GetElapsedTimeRound() < static_cast<float>(PvPCommon::Common::RoundStartMutationTimeSec) )
		return;

	// 이미 좀비가 설정되어있으면 설정할 필요가 없다.
	if( m_mZombieActor.empty() == false )
		return;

	InitializeZombie();
	m_bInitializeZombie = true;
}

void CPvPZombieMode::_OnStartRound()
{
	CPvPRoundMode::_OnStartRound();
	AllReturnHuman();
	m_bInitializeZombie = false;
	m_mZombieActor.clear();
}

void CPvPZombieMode::_ProcessFinishRoundMode( CDNUserSession* pSession, bool bIsWin, UINT uiWinTeam, PvPCommon::FinishReason::eCode Reason )
{
	if( bIsWin )
	{					
		switch(Reason)
		{
			case PvPCommon::FinishReason::TimeOver : // 타임오버 
			{
				pSession->AddGhoulScore(GhoulMode::PointType::TimeOver);	
				pSession->GetEventSystem()->OnEvent( EventSystem::OnRoundFinished, 4, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
													   EventSystem::PvPTeam, PvPCommon::Team::Human, EventSystem::PvPWinCondition, PvPCommon::FinishReason::TimeOver,
													   EventSystem::GhoulModeWin, pSession->GetTotalGhoulScore(GhoulMode::PointType::TimeOver));
				break;
			}
			case PvPCommon::FinishReason::OpponentTeamAllDead :
			{
				if( uiWinTeam == PvPCommon::Team::Human ) // 좀비가 다 죽어있음.
				{
					pSession->AddGhoulScore(GhoulMode::PointType::HumanWin);
					pSession->GetEventSystem()->OnEvent( EventSystem::OnRoundFinished, 4, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
														   EventSystem::PvPTeam, PvPCommon::Team::Human, EventSystem::PvPWinCondition, PvPCommon::FinishReason::OpponentTeamAllDead,
														   EventSystem::GhoulModeWin, pSession->GetTotalGhoulScore(GhoulMode::PointType::HumanWin));
				}
				break;
			}
			case PvPCommon::FinishReason::OpponentTeamAllGone :
			{
				if( uiWinTeam == PvPCommon::Team::Human )
				{
					pSession->AddGhoulScore(GhoulMode::PointType::HumanWin);
					pSession->GetEventSystem()->OnEvent( EventSystem::OnRoundFinished, 4, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
														   EventSystem::PvPTeam, PvPCommon::Team::Human, EventSystem::PvPWinCondition, PvPCommon::FinishReason::OpponentTeamAllGone,
														   EventSystem::GhoulModeWin, pSession->GetTotalGhoulScore(GhoulMode::PointType::HumanWin));
				}
				else if( uiWinTeam == PvPCommon::Team::Zombie )
				{
					pSession->AddGhoulScore(GhoulMode::PointType::GhoulWin);
					pSession->GetEventSystem()->OnEvent( EventSystem::OnRoundFinished, 4, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
														   EventSystem::PvPTeam, PvPCommon::Team::Zombie, EventSystem::PvPWinCondition, PvPCommon::FinishReason::OpponentTeamAllGone,
														   EventSystem::GhoulModeWin, pSession->GetTotalGhoulScore(GhoulMode::PointType::GhoulWin));
				}
				break;
			}						
		}
		
		pSession->AddGhoulScore(GhoulMode::PointType::PlayCount);	
		// 여기서 DB에 라운드 판 저장
		pSession->GetDBConnection()->QueryAddPVPGhoulScores( pSession );
		pSession->ClearAddGhoulScore();

		// 구울 모드 라운드 횟수
		pSession->GetEventSystem()->OnEvent( EventSystem::OnRoundFinished, 2, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
											   EventSystem::PvPRoundCount, pSession->GetTotalGhoulScore(GhoulMode::PointType::PlayCount));
	}		
}

void CPvPZombieMode::_ChangeZombie( DnActorHandle hActor, const TMonsterMutationData &Data )
{
	if( bIsZombie(hActor) )
		return;

	if( !hActor || !hActor->IsPlayerActor() )
		return;	
	
	// 좀비 정보 설정
	if (AddZombie(hActor, Data ))
	{
		_AddZombieBlow( hActor, Data );

		_UpdateSelectZombieCount( hActor->GetSessionID() );
		return;
	}
	
	_DANGER_POINT();
}

void CPvPZombieMode::_AddHumanBlow( DnActorHandle hActor )
{
	if( !hActor )
		return;

	// 인간으로 변경될 때 스킬제한
	std::string strSkillVec;
	std::vector< std::pair<int,int> > vData;
	bool bRet = g_pDataManager->GetPvPGameModeSkillSetting( GetPvPGameModeTable()->iSkillSetting, hActor->GetClassID(), vData );
	if( bRet )
	{
		hActor->CmdRemoveStateEffect(STATE_BLOW::BLOW_176); // 스킬제한 블로우를 초기화 해준다.
		hActor->GetStateBlow()->Process( 0, 0.f );

		for(DWORD i=0; i<vData.size();i++)
		{
			char szSkillIndex[256];
			sprintf(szSkillIndex, "%d;", vData[i].first);
			strSkillVec += szSkillIndex;
		}
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_176, -1, strSkillVec.c_str(), true );				// 설정된 인간스킬을 제외한 스킬들은 비활성화 시킨다.
	}

	if(!(hActor->GetStateBlow()->IsApplied(STATE_BLOW::BLOW_183)))
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_183, -1, NULL, true );
}

void CPvPZombieMode::_AddZombieBlow( DnActorHandle hActor, const TMonsterMutationData &Data )
{
	{
		USES_CONVERSION;
		
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_058, -1, I2A(Data.nMutationHP), true );				// 58) 최대 HP 비율
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_002, -1, I2A(Data.nMutationStrPMax), true );			// 2) 물리 공격력 비율
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_029, -1, I2A(Data.nMutationStrMMax), true );			// 29) 마법 공격 비율
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_004, -1, I2A(Data.nMutationDefP), true );				// 4) 물리 방어력 비율
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_094, -1, I2A(Data.nMutationDefM), true );				// 94) 마법 방어 비율
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_123, -1, I2A(Data.nMutationStiff), true );			// 123) 경직력 비율 변경
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_124, -1, I2A(Data.nMutationStiffResis), true );		// 124) 경직력 저항 비율 변경
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_126, -1, I2A(Data.nMutationCriticalResis), true );	// 126) 크리티컬 저항 비율 변경
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_128, -1, I2A(Data.nMutationStunResis), true );		// 128) 스턴 저항 비율 변경
		hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_061, -1, I2A(Data.nMutationSuperArmor), true );		// 61) 슈퍼아머1 부여
		if(!(hActor->GetStateBlow()->IsApplied(STATE_BLOW::BLOW_183)))
			hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_183, -1, NULL, true );								// 183) 구울모드에서 접두어 상태효과 적용 되지 않도록 하는 상태효과
	}

	hActor->GetStateBlow()->Process( 0, 0.f );
	hActor->CmdRefreshHPSP( hActor->GetMaxHP(), hActor->GetMaxSP() );
}

bool CPvPZombieMode::bIsZombie( DnActorHandle hActor )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return false;

	std::map<DWORD,DnActorHandle>::iterator itor = m_mZombieActor.find( hActor->GetUniqueID() );
	if( itor == m_mZombieActor.end() )
		return false;

	return true;
}

void CPvPZombieMode::_UpdateSelectZombieCount( UINT uiSessionID )
{
	std::map<UINT,UINT>::iterator itor = m_mSelectZombieCount.find( uiSessionID );
	if( itor != m_mSelectZombieCount.end() )
	{
		++(*itor).second;
	}
	else
	{
		m_mSelectZombieCount.insert( std::make_pair(uiSessionID,1) );
	}
}

UINT CPvPZombieMode::_GetSelectZombieCount( UINT uiSessionID )
{
	std::map<UINT,UINT>::iterator itor = m_mSelectZombieCount.find( uiSessionID );
	if( itor != m_mSelectZombieCount.end() )
		return (*itor).second;

	return 0;
}

void CPvPZombieMode::_GetZombieCandidate( std::vector<DnActorHandle>& vResult )
{
	UINT uiMinValue = UINT_MAX;

	std::vector<DnActorHandle> vCandidate;
	for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
	{
		CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
		if( !pSession || !pSession->GetActorHandle() )
			continue;
		if( !bIsPlayingUser( pSession->GetActorHandle() ) )
			continue;
		UINT uiCount = _GetSelectZombieCount( pSession->GetSessionID() );
		if(  uiCount < uiMinValue )	
			uiMinValue = uiCount;

		vCandidate.push_back( pSession->GetActorHandle() );
	}

	for( UINT i=0 ; i<vCandidate.size() ; ++i )
	{
		if( _GetSelectZombieCount( vCandidate[i]->GetSessionID() ) == uiMinValue )
			vResult.push_back( vCandidate[i] );
	}
}

void CPvPZombieMode::InitZombieGroupData()
{
	g_pDataManager->GetGhoulGroup(GetGameRoom()->GetUserCountWithoutGM(), m_MutationGroup, m_nGhoulCount);
}

bool CPvPZombieMode::GetZombieActorData(TMonsterMutationData &Data)
{
	int nRandomOffset = _roomrand(GetGameRoom())%1000000000;
	int nTotalProb = 0;
	for (int i = 0; i < PvPCommon::Common::MonsterMutationColCount; i++)
	{
		if( nRandomOffset < m_MutationGroup.nProbability[i] + nTotalProb )
			return g_pDataManager->GetMonsterMutationData(m_MutationGroup.nMonsterID[i], Data);
		nTotalProb += m_MutationGroup.nProbability[i];
	}
	return false;
}

bool CPvPZombieMode::AddZombie(DnActorHandle hActor, const TMonsterMutationData &Data )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return false;

	std::map<DWORD,DnActorHandle>::iterator itor = m_mZombieActor.find( hActor->GetUniqueID() );
	if( itor == m_mZombieActor.end() )
	{
		m_mZombieActor.insert(std::make_pair(hActor->GetUniqueID(), hActor));

		// 좀비 팀으로 변경
		hActor->CmdChangeTeam( PvPCommon::Team::Zombie );
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->GetUserSession()->SetTeam( PvPCommon::Team::Zombie );
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->ToggleTransformMode(true, Data.nMutationID ); // 액터 좀비설정

		// 랜덤 사이즈
		_fpreset();
		int nScale = 100;
		int nMin = Data.nSizeMin;
		int nMax = Data.nSizeMax;
		if( nMin <= nMax ) {
			nScale = ( nMin + ( _rand(hActor->GetRoom())%( ( nMax + 1 ) - nMin ) ) );
		}
		float fScale = nScale/100.f;
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->SetScale( EtVector3( fScale, fScale, fScale ) );

		_ASSERT( GetGameRoom()->bIsPvPRoom() );
		static_cast<CDNPvPGameRoom*>(GetGameRoom())->SendSelectZombie( hActor, Data.nMutationID, true, false, nScale );
		return true;
	}
	return false;
}

bool CPvPZombieMode::DelZombie(DnActorHandle hActor, bool bRemoveStateBlow/*=false*/ )
{
	if( !hActor || !hActor->IsPlayerActor() )
		return false;

	std::map<DWORD,DnActorHandle>::iterator itor = m_mZombieActor.find( hActor->GetUniqueID() );
	if( itor != m_mZombieActor.end() )
	{
		// 인간 팀으로 변경
		hActor->CmdChangeTeam( PvPCommon::Team::Human );
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->GetUserSession()->SetTeam( PvPCommon::Team::Human );
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->ToggleTransformMode(false);

		// 사이즈 복구
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->SetScale( EtVector3( 1.f, 1.f, 1.f ) );

		_ASSERT( GetGameRoom()->bIsPvPRoom() );
		static_cast<CDNPvPGameRoom*>(GetGameRoom())->SendSelectZombie( hActor, -1, false, bRemoveStateBlow );
		m_mZombieActor.erase(itor);

		if( bRemoveStateBlow )
		{
			// 나머지, 다른 플레이어나 몹에 의해 걸려 있는 상태효과 모두 없앤다.
			hActor->RemoveAllBlowExpectPassiveSkill();
			hActor->GetStateBlow()->Process( 0, 0.f );
			hActor->CmdRefreshHPSP( hActor->GetMaxHP(), hActor->GetMaxSP() );
		}

		// 인간 상태효과
		_AddHumanBlow( hActor );
		return true;
	}
	return false;
}

void CPvPZombieMode::InitializeZombie()
{
	InitZombieGroupData();

	if (GetZombieCount() >= m_nGhoulCount)
		return;

	_srand( GetGameRoom(), timeGetTime() );

	for (int i = 0; i < m_nGhoulCount; i++)
	{
		TMonsterMutationData Data;
		memset(&Data, 0, sizeof(TMonsterMutationData));
		if (GetZombieActorData(Data) == false)
		{
			_DANGER_POINT();
			continue;
		}

		std::vector<DnActorHandle> vResult;
		_GetZombieCandidate( vResult );
		if( vResult.empty() )
			return;

		DnActorHandle hZombie = vResult[_roomrand(GetGameRoom())%vResult.size()];
		_ChangeZombie( hZombie, Data );
	}
}

void CPvPZombieMode::AllReturnHuman()
{
	CDNUserSession * pSession = NULL;
	for (int i = 0; i < (int)GetGameRoom()->GetUserCount(); i++)
	{
		pSession = GetGameRoom()->GetUserData(i);
		if (pSession )
		{
			if( pSession->GetTeam() == PvPCommon::Team::Observer )
				continue;

			if( DelZombie(pSession->GetActorHandle()) == false )
			{
				_AddHumanBlow( pSession->GetActorHandle() );
			}
		}
		else
			_DANGER_POINT();
	}
}


