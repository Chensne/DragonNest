#include "stdafx.h"
#include "DnMutatorGuildWar.h"
#include "DnMonsterActor.h"
#include "DnLocalPlayerActor.h"
#include "DnInterface.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPModeEndDlg.h"
#include "PvPScoreSystem.h"
#include "DnMutatorGame.h"
#include "DnCaptionDlg_04.h"
#include "Timeset.h"
#include "DnWorld.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "DnWorldSector.h"
#include "DnWorldData.h"
#include "PvPSendPacket.h"
#include "DnTableDB.h"
#include "DnGuildWarModeHUD.h"
#include "DnGuildWarResultDlg.h"
#include "DnMainMenuDlg.h"
#include "DnCharStatusDlg.h"
#include "DnCharVehicleDlg.h"
#include "DnGuildZoneMapDlg.h"
#include "PvPGuildWarScoreSystem.h"
#include "DnGuildWarSituationMng.h"
#include "DnStateBlow.h"
#include "DnOccupationTask.h"
#include "TaskManager.h"
#include "DnItemTask.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnMutatorGuildWarMode::CDnMutatorGuildWarMode( CDnPvPGameTask * pGameTask )
: CDnMutatorTeamGame( pGameTask )
{
	RestartGame();

	m_bIsTeamHairColorMode = true;
	m_fRespawnTime = 0.f;
	m_bRespawnMessage = false;
	m_pOccupationTask = NULL;
}

CDnMutatorGuildWarMode::~CDnMutatorGuildWarMode()
{
	if( !m_pOccupationTask ) return;
	m_pOccupationTask->Finalize();
	m_pOccupationTask->DestroyTask( true );
	m_pOccupationTask = NULL;
}

IScoreSystem* CDnMutatorGuildWarMode::CreateScoreSystem()
{
	return new CPvPGuildWarScoreSystem();
}


bool CDnMutatorGuildWarMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	if( GetInterface().GetHUD() && pSox )
	{
		GetInterface().GetHUD()->SetRoomName(CDnBridgeTask::GetInstance().GetPVPRoomStatus().wszPVPRoomName.c_str() );

		int iUIstring = pSox->GetFieldFromLablePtr( uiItemID  , "WinCondition_UIString" )->GetInteger();

		WCHAR wszWinCon[256];
		wsprintf(wszWinCon,L"%d%s",CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIstring ));
		GetInterface().GetHUD()->SetWinCondition( wszWinCon  );
	}	

	return InitializeBase( uiItemID, pSox );
}

void CDnMutatorGuildWarMode::GameStart( int nObjectiveUIString, float nRemainCountDown, float RemainSec, bool IsInGameJoin, float nMaxSec )
{
	//자기 팀 셋팅
	if( CDnActor::s_hLocalActor )
	{
		m_nTeam = CDnActor::s_hLocalActor->GetTeam();
	}
	if( CDnOccupationTask::IsActive() )
	{
		GetOccupationTask().SetBreakInfoUser( IsInGameJoin );
		GetOccupationTask().SetScoreSystem( m_pScoreSystem );
	}
	// Set GuildWarSkill UI
	int nTeam = CDnActor::s_hLocalActor->GetTeam();
	DWORD dwPartyCount = GetPartyTask().GetPartyCount();

	for( DWORD itr = 0; itr < dwPartyCount; itr++ )
	{
		CDnPartyTask::PartyStruct *pInfo = GetPartyTask().GetPartyData(itr);
		if( !pInfo || pInfo->usTeam != nTeam || !pInfo->hActor || pInfo->hActor->GetTeam() == PvPCommon::Team::Observer ) continue;

		if( pInfo->hActor->GetUniqueID() == CDnActor::s_hLocalActor->GetUniqueID() )	// 본인일 경우
		{
			GetInterface().SetGuildWarSkillUI( pInfo->uiPvPUserState, true );
			break;
		}
	}

	//시간을 셋팅하고
	GetInterface().GetHUD()->SetTime( nMaxSec - RemainSec , CTimeSet().GetTimeT64_GM() , CTimeSet().GetMilliseconds()); // 시간 + 밀리세컨드

	if( IsInGameJoin )
	{
		GetInterface().GetHUD()->StartTimer();
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
		return;
	}

	if( nRemainCountDown )
	{
		m_fCountDownTime = float(nRemainCountDown);
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringStarting )  );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}
}

void CDnMutatorGuildWarMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnMutatorTeamGame::Process( LocalTime, fDelta );

	ProcessObserver();

	if( m_bRespawnMessage )
	{
		m_fRespawnTime += fDelta;

		if( m_fRespawnTime > m_pGameTask->GetPlayerDieRespawnDelta() )
		{
			GetInterface().CloseMessageBox();
			m_bRespawnMessage = false;
		}
	}
}

bool CDnMutatorGuildWarMode::ProcessActorDie( DnActorHandle hKillerActor, DnActorHandle hKilledActor )
{
	CDnMonsterActor *pMonster = NULL;
	CDnPlayerActor *pPlayer = NULL;
	CDnPlayerActor *pOtherPlayer = NULL;

	WCHAR wszMessage[256];

	SecureZeroMemory(wszMessage,sizeof(wszMessage));

	if( CDnActor::s_hLocalActor && hKillerActor == CDnActor::s_hLocalActor ) //로컬플레이어가 살인자일때 
	{
		pMonster = dynamic_cast<CDnMonsterActor *>(hKilledActor.GetPointer());

		if( pMonster ) //피해자가 몬스터
		{
			return true;
		}		

		pPlayer = dynamic_cast<CDnPlayerActor *>(hKilledActor.GetPointer());

		if( pPlayer ) //피해자가 플레이어
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::Kill ),pPlayer->GetName());

			GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );

			// 여기서 로컬의 킬카운트 증가.
			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
			int nIndex = pPlayer->GetClassID()-1;
			if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
				pPvPInfo->uiKOClassCount[nIndex] += 1;
			return true;
		}
	}

	pMonster = NULL;
	pPlayer = NULL;

	if( (pPlayer = dynamic_cast<CDnPlayerActor *>(hKillerActor.GetPointer())))//다른 플레이어가 살인자 
	{
		if( CDnActor::s_hLocalActor && hKilledActor == CDnActor::s_hLocalActor )// 로컬 플레이어가 피해자 
		{
			RequestImmediateRebirth();

			GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::WaitRespawn ), m_pGameTask->GetPlayerDieRespawnDelta(), false );

			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::Killed ),pPlayer->GetName());

			GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );

			// 여기서 로컬의 데스카운트 증가.
			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
			int nIndex = pPlayer->GetClassID()-1;
			if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
				pPvPInfo->uiKObyClassCount[nIndex] += 1;
			return true;
		}

		if( pOtherPlayer = dynamic_cast<CDnPlayerActor *>(hKilledActor.GetPointer()) )//다른 플레이어가 피해자 
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::SeeKill ),pPlayer->GetName(),pOtherPlayer->GetName());

			if( m_nTeam ==  pOtherPlayer->GetTeam() )//피해자가 우리팀 실점
			{
				GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );
			}
			else										//득점
			{
				pOtherPlayer->SetExposure( false );
				GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );
			}			
		}		

		pMonster = dynamic_cast<CDnMonsterActor *>(hKilledActor.GetPointer());

		if( pMonster ) //몬스터가 피해자 
		{
			return true;
		}
	}

	pMonster = NULL;
	pPlayer = NULL;

	if(  (pMonster = dynamic_cast<CDnMonsterActor *>(hKillerActor.GetPointer())) )//몬스터가 살인자 
	{
		if( CDnActor::s_hLocalActor && hKilledActor == CDnActor::s_hLocalActor )// 로컬 플레이어가 피해자 
		{
			RequestImmediateRebirth();

			GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::WaitRespawn ), m_pGameTask->GetPlayerDieRespawnDelta(), false );
			return true;
		}

		pPlayer = dynamic_cast<CDnPlayerActor *>(hKilledActor.GetPointer());

		if( pPlayer ) //플레이어가 피해자 
		{
			return true;
		}
	}

	WarningLog("Nobody Killed ,Nobody Kill ");
	return false;
}

void CDnMutatorGuildWarMode::EndGame( void * pData )
{
	CPvPScoreSystem::SMyScore * sInfo = NULL;
	sInfo = (CPvPScoreSystem::SMyScore*)m_pScoreSystem->GetScoreStruct(CDnActor::s_hLocalActor->GetName());

	SCPVP_FINISH_PVPMODE * pPacket = NULL;	

	pPacket = ( SCPVP_FINISH_PVPMODE * )pData;

	GetInterface().GetHUD()->HaltTimer();

	if( CDnPartyTask::IsActive() )
	{
		CDnGuildWarResultDlg::STeamData sData;

		for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) {
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
			if( !pStruct ) continue;
			if( !pStruct->hActor ) continue;
			if( pStruct->hActor) 
			{

				GetInterface().SetPVPGameScore( pStruct->hActor->GetUniqueID(),
					m_pScoreSystem->GetKillCount(pStruct->hActor),
					m_pScoreSystem->GetDeathCount(pStruct->hActor),
					0,                                                // 대장이 없기때문에 0 을 보내줘요
					m_pScoreSystem->GetKillScore(pStruct->hActor),
					m_pScoreSystem->GetAssistScore(pStruct->hActor),
					m_pScoreSystem->GetTotalScore(pStruct->hActor),
					m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor) );

				if( PvPCommon::Team::A == pStruct->hActor->GetTeam() )
				{
					sData.nBlueOccupation += m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor);
					sData.nBlueTeamKill += m_pScoreSystem->GetKillCount(pStruct->hActor);
				}
				else if( PvPCommon::Team::B == pStruct->hActor->GetTeam() )
				{
					sData.nRedOccupation += m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor);
					sData.nRedTeamKill += m_pScoreSystem->GetKillCount(pStruct->hActor);
				}
			}
		}
		if( CDnOccupationTask::IsActive() )
		{
			sData.nBlueTeamResource = GetOccupationTask().GetTotalResource(PvPCommon::Team::A);
			sData.nRedTeamResource = GetOccupationTask().GetTotalResource(PvPCommon::Team::B);
		}
		sData.bBlueBossKill = m_pScoreSystem->IsBlueBossKill();
		sData.bRedBossKill = m_pScoreSystem->IsRedBossKill();

		TPvPOccupationTeamScoreInfo sBlueScore, sRedScore;

		m_pScoreSystem->GetOccupationTeamScore( PvPCommon::Team::A, sBlueScore );
		m_pScoreSystem->GetOccupationTeamScore( PvPCommon::Team::B, sRedScore );

		sData.nBlueTeamKill = sBlueScore.nTeamKillScore;
		sData.nBlueOccupation = sBlueScore.nTeamAcquireScore;

		sData.nRedTeamKill = sRedScore.nTeamKillScore;
		sData.nRedOccupation = sRedScore.nTeamAcquireScore;

		sData.nBlueTeamScore = sData.nBlueTeamResource + sData.nBlueTeamKill + sData.nBlueOccupation;
		sData.nRedTeamScore = sData.nRedTeamResource + sData.nRedTeamKill + sData.nRedOccupation;

		if( GetInterface().GetGuildWarResultDlg() )
			GetInterface().GetGuildWarResultDlg()->SetData( sData );

		//길드전 전용 메뉴 셋팅
		if( GetInterface().GetHUD() )
		{
			int nRedTeam = 0, nBlueTeam = 0;
			std::vector<CDnOccupationZone *> & vFlag = GetOccupationTask().GetFlagVector();
			for( int itr = 0; itr < (int)vFlag.size(); ++itr )
			{
				if( PvPCommon::OccupationState::Own  & vFlag[itr]->m_eState )
				{
					if( PvPCommon::Team::A == vFlag[itr]->m_nOwnedTemID )
						++nBlueTeam;
					else if( PvPCommon::Team::B == vFlag[itr]->m_nOwnedTemID )
						++nRedTeam;
				}
			}

			CDnGuildWarModeHUD * pHUD = dynamic_cast<CDnGuildWarModeHUD *>(GetInterface().GetHUD());

			pHUD->SetRedPoint( nRedTeam, sData.nRedTeamScore );
			pHUD->SetBluePoint( nBlueTeam, sData.nBlueTeamScore );
		}
	}

	GetInterface().OpenFinalResultDlg();

	byte cResult = PVPResult::Draw;

	if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리 
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
		{
			cResult = PVPResult::Win;
			if( PvPCommon::FinishReason::OpponentTeamBossDead == pPacket->Reason )
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringBossVictory ) );
			else
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringTimeVictory ) );
		}
		else
		{
			cResult = PVPResult::Lose;
			if( PvPCommon::FinishReason::OpponentTeamBossDead == pPacket->Reason )
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringBossDefeated ) );
			else
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringTimeDefeated ) );
		}

		//return;
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
		{
			cResult = PVPResult::Win;
			if( PvPCommon::FinishReason::OpponentTeamBossDead == pPacket->Reason )
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringBossVictory ) );
			else
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringTimeVictory ) );
		}
		else
		{
			cResult = PVPResult::Lose;
			if( PvPCommon::FinishReason::OpponentTeamBossDead == pPacket->Reason )
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringBossDefeated ) );
			else
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringTimeDefeated ) );
		}

		//return;
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
	{
		cResult = PVPResult::Draw;
		GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GuildWarModeString::UIStringDraw ) );
		//return;
	}

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_GuildWar  , true ,cResult, true );
}

void CDnMutatorGuildWarMode::_RenderGameModeScore()
{
	if( !m_pScoreSystem || m_nTeam ==0)
		return;

	UINT uiATeamScore, uiBTeamScore;
	m_pScoreSystem->GetGameModeScore( uiATeamScore, uiBTeamScore );

	int iMyScore		= (m_nTeam == PvPCommon::Team::A) ? uiATeamScore : uiBTeamScore;
	int iEnemyScore		= (m_nTeam == PvPCommon::Team::A) ? uiBTeamScore : uiATeamScore;

	if(m_nTeam == PvPCommon::Team::Observer)
	{
		iMyScore = uiATeamScore;
		iEnemyScore = uiBTeamScore;
	}

	if( GetInterface().GetHUD() )
	{
		GetInterface().GetHUD()->SetMyScore( iMyScore );
		GetInterface().GetHUD()->SetEnemyScore( iEnemyScore );
	}

	//개인 스코어 셋팅

	if( CDnPartyTask::IsActive() )
	{
		CDnGuildWarResultDlg::STeamData sData;

		for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) {
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
			if( !pStruct ) continue;
			if( !pStruct->hActor ) continue;
			if( pStruct->hActor) 
			{

				GetInterface().SetPVPGameScore( pStruct->hActor->GetUniqueID(),
					m_pScoreSystem->GetKillCount(pStruct->hActor),
					m_pScoreSystem->GetDeathCount(pStruct->hActor),
					0,                                                // 대장이 없기때문에 0 을 보내줘요
					m_pScoreSystem->GetKillScore(pStruct->hActor),
					m_pScoreSystem->GetAssistScore(pStruct->hActor),
					m_pScoreSystem->GetTotalScore(pStruct->hActor),
					m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor) );

				if( PvPCommon::Team::A == pStruct->hActor->GetTeam() )
				{
					sData.nBlueOccupation += m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor);
					sData.nBlueTeamKill += m_pScoreSystem->GetKillCount(pStruct->hActor);
				}
				else if( PvPCommon::Team::B == pStruct->hActor->GetTeam() )
				{
					sData.nRedOccupation += m_pScoreSystem->GetOccupationAcquireScore(pStruct->hActor);
					sData.nRedTeamKill += m_pScoreSystem->GetKillCount(pStruct->hActor);
				}
			}
		}
		if( CDnOccupationTask::IsActive() )
		{
			sData.nBlueTeamResource = GetOccupationTask().GetTotalResource(PvPCommon::Team::A);
			sData.nRedTeamResource = GetOccupationTask().GetTotalResource(PvPCommon::Team::B);
		}
		sData.bBlueBossKill = m_pScoreSystem->IsBlueBossKill();
		sData.bRedBossKill = m_pScoreSystem->IsRedBossKill();

		TPvPOccupationTeamScoreInfo sBlueScore, sRedScore;

		m_pScoreSystem->GetOccupationTeamScore( PvPCommon::Team::A, sBlueScore );
		m_pScoreSystem->GetOccupationTeamScore( PvPCommon::Team::B, sRedScore );

		sData.nBlueTeamKill = sBlueScore.nTeamKillScore;
		sData.nBlueOccupation = sBlueScore.nTeamAcquireScore;

		sData.nRedTeamKill = sRedScore.nTeamKillScore;
		sData.nRedOccupation = sRedScore.nTeamAcquireScore;

		sData.nBlueTeamScore = sData.nBlueTeamResource + sData.nBlueTeamKill + sData.nBlueOccupation;
		sData.nRedTeamScore = sData.nRedTeamResource + sData.nRedTeamKill + sData.nRedOccupation;

		if( GetInterface().GetGuildWarResultDlg() )
			GetInterface().GetGuildWarResultDlg()->SetData( sData );

		//길드전 전용 메뉴 셋팅
		if( GetInterface().GetHUD() )
		{
			int nRedTeam = 0, nBlueTeam = 0;
			std::vector<CDnOccupationZone *> & vFlag = GetOccupationTask().GetFlagVector();
			for( int itr = 0; itr < (int)vFlag.size(); ++itr )
			{
				if( PvPCommon::OccupationState::Own  & vFlag[itr]->m_eState )
				{
					if( PvPCommon::Team::A == vFlag[itr]->m_nOwnedTemID )
						++nBlueTeam;
					else if( PvPCommon::Team::B == vFlag[itr]->m_nOwnedTemID )
						++nRedTeam;
				}
			}
			CDnGuildWarModeHUD * pHUD = dynamic_cast<CDnGuildWarModeHUD *>(GetInterface().GetHUD());

			pHUD->SetRedPoint( nRedTeam, sData.nRedTeamScore );
			pHUD->SetBluePoint( nBlueTeam, sData.nBlueTeamScore );
		}
	}
}

void CDnMutatorGuildWarMode::PushUsedRespawnArea( const std::string& name )
{
	m_UsedRespawnAreaList.push_back( name );
}

bool CDnMutatorGuildWarMode::bIsUsedRespawnArea( const std::string& name )
{
	std::list<std::string>::iterator itor = std::find( m_UsedRespawnAreaList.begin(), m_UsedRespawnAreaList.end(), name );
	if( itor == m_UsedRespawnAreaList.end() )
		return false;
	return true;
}

void CDnMutatorGuildWarMode::ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin, void * pData )
{
	CDnMutatorTeamGame::ProcessEnterUser( hActor, isIngameJoin, pData );

	if( CDnActor::s_hLocalActor->GetTeam() != hActor->GetTeam() )
		return;

	CDnGuildWarSituationMng * pSituation = GetInterface().GetGuildWarSituation();

	if( pSituation )
		pSituation->AddUser( hActor, pData );
}

void CDnMutatorGuildWarMode::OnPostInitializeStage()
{
	m_pOccupationTask = new CDnOccupationTask;
	m_pOccupationTask->Initialize( PvPCommon::GameMode::PvP_GuildWar );

	CTaskManager::GetInstance().AddTask( m_pOccupationTask, "OccupationTask", -1, false );

	CDnGuildWarZoneMapDlg * pMapDlg = GetInterface().GetGuildWarZoneMapDlg();
	if( !pMapDlg ) return;
	pMapDlg->PostInitialize();
	pMapDlg->Show( true );
}

void CDnMutatorGuildWarMode::RequestImmediateRebirth()
{
	std::vector<CDnItem *> vecItem;
	GetItemTask().FindItemFromItemType( ITEMTYPE_IMMEDIATEREBIRTH, ITEM_SLOT_TYPE::ST_INVENTORY, vecItem );
	if( !vecItem.empty() )
	{
#if defined( _FINAL_BUILD )
		if( vecItem[0]->GetCoolTime() == 0.0f )
#else
		if( CGlobalValue::GetInstance().m_bIgnoreCoolTime == true || vecItem[0]->GetCoolTime() == 0.0f )
#endif // #if defined( _FINAL_BUILD )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7544 ), MB_YESNO, 0, this );	//당신은 소생의 비약을 가지고 있습니다. 소생의 비약을 사용하여 현 위치에서 부활하시겠습니까?
			m_fRespawnTime = 0.f;
			m_bRespawnMessage = true;
		}
	}
}

void CDnMutatorGuildWarMode::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg  )
{
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strcmp( pControl->GetControlName(), "ID_YES") == 0 )
		{
			if( !CDnActor::s_hLocalActor && !CDnActor::s_hLocalActor->IsDie() ) return;

			std::vector<CDnItem *> vecItem;
			GetItemTask().FindItemFromItemType( ITEMTYPE_IMMEDIATEREBIRTH, ITEM_SLOT_TYPE::ST_INVENTORY, vecItem );
			if( vecItem.empty() || !vecItem[0] ) return;

			CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
			if( !pLocalActor ) return;

			//pLocalActor->UseItemFromSlotIndex( vecItem[0]->GetSlotIndex(), eItemPositionType::ITEMPOSITION_INVEN );
			GetItemTask().GetCharInventory().RequestUseItem( vecItem[0]->GetSlotIndex(), ITEMPOSITION_INVEN );
		}
		m_bRespawnMessage = false;
	}
}

void CDnMutatorGuildWarMode::ProcessObserver()
{
	// GM 옵저버는 성문을 부순다.
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * pLocalActor = dynamic_cast<CDnLocalPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
	if( pLocalActor && pLocalActor->IsObserver() )
	{
		CPvPGuildWarScoreSystem * pScore = dynamic_cast<CPvPGuildWarScoreSystem *>(m_pScoreSystem);

		for( int nTeam = 0; nTeam < PvPCommon::TeamIndex::Max; ++nTeam )
		{
			for( int nCount = 0; nCount < PvPCommon::Common::DoorCount; ++nCount )
			{
				int nDoorID = pScore->GetDoorID( nTeam, nCount );

				CDnMonsterActor * pMonster = GetDoorActor( nDoorID );

				if( !pMonster || !pMonster->CheckAnimation("Die") ) continue;

				pMonster->CmdAction( "Die" );
			}
		}
	}
}

CDnMonsterActor * CDnMutatorGuildWarMode::GetDoorActor( int nDoorID )
{
	for( int itr = 0; itr < (int)CDnActor::s_pVecProcessList.size(); ++itr )
	{
		if( CDnActor::s_pVecProcessList[itr] && CDnActor::s_pVecProcessList[itr]->IsMonsterActor() )
		{
			CDnMonsterActor * pMonster = reinterpret_cast<CDnMonsterActor *>(CDnActor::s_pVecProcessList[itr]);

			if( pMonster && pMonster->GetMonsterClassID() == nDoorID )
				return pMonster;
		}
	}

	return NULL;
}

