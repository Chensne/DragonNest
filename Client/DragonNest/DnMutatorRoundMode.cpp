#include "StdAfx.h"
#include "DnMutatorRoundMode.h"
#include "DnMonsterActor.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPModeEndDlg.h"
#include "PvPScoreSystem.h"
#include "DnRoundModeHUD.h"
#include "DnLocalPlayerActor.h"
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#include "TimeSet.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#endif

CDnMutatorRoundMode::CDnMutatorRoundMode( CDnPvPGameTask * pGameTask ):CDnMutatorTeamGame( pGameTask )
{
	RestartGame();

	m_bFirstRound = true;
	m_iRound = 1;

}

CDnMutatorRoundMode::~CDnMutatorRoundMode()
{

}

IScoreSystem* CDnMutatorRoundMode::CreateScoreSystem()
{
	return new IBoostPoolPvPScoreSystem();
}

bool CDnMutatorRoundMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	CDnRoundModeHUD * pRoundModeHud = NULL;

	pRoundModeHud = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());

	if( pRoundModeHud )
	{
		pRoundModeHud->SetRoomName(CDnBridgeTask::GetInstance().GetPVPRoomStatus().wszPVPRoomName.c_str() );
		pRoundModeHud->SetStage( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount );

		int iUIstring = 0;
		if ( pSox ) 
			iUIstring = pSox->GetFieldFromLablePtr( uiItemID  , "WinCondition_UIString" )->GetInteger();

		WCHAR wszWinCon[256];
		wsprintf(wszWinCon,L"%d%s",CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIstring ));
		GetInterface().GetHUD()->SetWinCondition( wszWinCon  );
	}

	return InitializeBase( uiItemID, pSox );
}

void CDnMutatorRoundMode::GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec)
{

	//자기 팀 셋팅
	if( CDnActor::s_hLocalActor )
	{
		m_nTeam = CDnActor::s_hLocalActor->GetTeam();
	}

	GetInterface().RestartRound();

	//시간을 셋팅하고
	GetInterface().GetHUD()->SetTime( nMaxSec - RemainSec , CTimeSet().GetTimeT64_GM() , CTimeSet().GetMilliseconds()); // 시간 + 밀리세컨드
	GetInterface().GetRespawnModeEnd()->Reset();
	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , false , 0 , false );

	//m_iRound++;

	if( IsInGameJoin )
	{
		GetInterface().GetHUD()->StartTimer();
		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
		return;
	}

	if( nRemainCountDown )
	{
		m_fCountDownTime = float(nRemainCountDown);
		//GetInterface().BeginCountDown( nRemainCountDown  , L"");
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringStarting )  );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );		

	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}

#ifdef PRE_ADD_PVP_DUAL_INFO
	CDnPvPGameTask* pGameTask = (CDnPvPGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if(pGameTask && pGameTask->GetLadderType() == LadderSystem::MatchType::_1vs1 )
	{
		DnActorHandle hBlueActor;
		DnActorHandle hRedActor;
		
		for( int itr = 0; itr < (int)CDnActor::s_pVecProcessList.size(); ++itr )
		{
			if( CDnActor::s_pVecProcessList[itr] && CDnActor::s_pVecProcessList[itr]->IsPlayerActor() )
			{
				if( CDnActor::s_pVecProcessList[itr]->GetTeam() == PvPCommon::Team::A )
					hBlueActor = CDnActor::s_pVecProcessList[itr]->GetActorHandle();

				if( CDnActor::s_pVecProcessList[itr]->GetTeam() == PvPCommon::Team::B )
					hRedActor = CDnActor::s_pVecProcessList[itr]->GetActorHandle();
			}
		}

		if( hBlueActor && hRedActor )
		{
			CDnRoundModeHUD *pRoundModeHUD = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());
			if( pRoundModeHUD )
			{
				if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::B ) // B팀이면 
				{
					pRoundModeHUD->SetDualInfo(
						hRedActor->GetName(),
						hRedActor->OnGetJobClassID(),
						hBlueActor->GetName(),
						hBlueActor->OnGetJobClassID() );
				}
				else // 관전자 A팀 등등 해당
				{
					pRoundModeHUD->SetDualInfo(
						hBlueActor->GetName(),
						hBlueActor->OnGetJobClassID(),
						hRedActor->GetName(),
						hRedActor->OnGetJobClassID() );
				}

				pRoundModeHUD->OrderShowDualInfo();
			}
		}
	}
#endif
}


bool CDnMutatorRoundMode::ProcessActorDie( DnActorHandle hKillerActor, DnActorHandle hKilledActor )
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
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::Kill ),pPlayer->GetName());

			GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );

			CDnPvPGameTask* pGameTask = (CDnPvPGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask && pGameTask->IsLadderMode() == false )
			{
				TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
				int nIndex = pPlayer->GetClassID()-1;
				if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
					pPvPInfo->uiKOClassCount[nIndex] += 1;
			}

			return true;
		}

	}

	pMonster = NULL;
	pPlayer = NULL;

	if( (pPlayer = dynamic_cast<CDnPlayerActor *>(hKillerActor.GetPointer())))//다른 플레이어가 살인자 
	{
		if( CDnActor::s_hLocalActor && hKilledActor == CDnActor::s_hLocalActor )// 로컬 플레이어가 피해자 
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::Killed ),pPlayer->GetName());
			GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );

			CDnPvPGameTask* pGameTask = (CDnPvPGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask && pGameTask->IsLadderMode() == false )
			{
				TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
				int nIndex = pPlayer->GetClassID()-1;
				if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
					pPvPInfo->uiKObyClassCount[nIndex] += 1;
			}

			return true;
		}

		if( pOtherPlayer = dynamic_cast<CDnPlayerActor *>(hKilledActor.GetPointer()) )//다른 플레이어가 피해자 
		{

			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::SeeKill ),pPlayer->GetName(),pOtherPlayer->GetName());

			if( m_nTeam ==  pOtherPlayer->GetTeam() )//피해자가 우리팀 실점
			{
				GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );

			}else										//득점
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
			//GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::WaitRespawn ),m_pGameTask->GetPlayerDieRespawnDelta());
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

void CDnMutatorRoundMode::EndGame( void * pData )
{
	CPvPScoreSystem::SMyScore * sInfo = NULL;
	sInfo = (CPvPScoreSystem::SMyScore*)m_pScoreSystem->GetScoreStruct(CDnActor::s_hLocalActor->GetName());

	SCPVP_FINISH_PVPMODE * pPacket = NULL;	

	pPacket = ( SCPVP_FINISH_PVPMODE * )pData;

	GetInterface().GetHUD()->HaltTimer();

	GetInterface().OpenFinalResultDlg();	

	byte cResult = PVPResult::Win;

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );

	if(pPacket->Reason == PvPCommon::FinishReason::LadderNoGame ) // 레더인경우만 , 레더는일단 라운드모드를 쓰기때문에 이곳에만 정의
	{
		cResult = PVPResult::Draw;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringRoundCancel ) );
		GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true ,cResult, true );
		return;
	}

#ifdef PRE_MOD_PVPOBSERVER

	if( CDnActor::s_hLocalActor )
	{
		//--------------------------------------------------------
		// LocalPlayer가 Observer 이면서  Draw 가 아닌경우 UI처리.
		//
		if( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer && 
			pPacket->uiWinTeam != PvPCommon::Team::Max ) 
		{
			GetInterface().GetRespawnModeEnd()->SetResultStringObServer( pPacket->uiWinTeam, 
				GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory) ); // "승리!"
		}

		//--------------------------------------------------------
		// PVP 참여 플레이어의 경우 기존그대로 처리.
		else
		{
			if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리
			{
				if( PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() )
				{
					cResult = PVPResult::Win;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
					//GetInterface().GetHUD()->AddMyScore( 1 );
				}
				else
				{
					cResult = PVPResult::Lose;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
					//GetInterface().GetHUD()->AddEnemyScore( 1 );
				}		
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
			{
				if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
				{
					cResult = PVPResult::Win;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
					//GetInterface().GetHUD()->AddMyScore( 1 );
				}
				else
				{
					cResult = PVPResult::Lose;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
					//GetInterface().GetHUD()->AddEnemyScore( 1 );
				}		
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
			{
				cResult = PVPResult::Draw;
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDraw ) );		
			}
		}

	}

#else
	if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
		{
			cResult = PVPResult::Win;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
			//GetInterface().GetHUD()->AddMyScore( 1 );
		}
		else
		{
			cResult = PVPResult::Lose;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
			//GetInterface().GetHUD()->AddEnemyScore( 1 );
		}		
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
		{
			cResult = PVPResult::Win;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
			//GetInterface().GetHUD()->AddMyScore( 1 );
		}
		else
		{
			cResult = PVPResult::Lose;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
			//GetInterface().GetHUD()->AddEnemyScore( 1 );
		}		
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
	{
		cResult = PVPResult::Draw;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDraw ) );		
	}

#endif // PRE_MOD_PVPOBSERVER

	

#ifdef PRE_ADD_PVP_DUAL_INFO
	CDnRoundModeHUD *pRoundModeHUD = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());
	if( pRoundModeHUD )
		pRoundModeHUD->ClearDualInfo();
#endif

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true ,cResult, true );
}

void CDnMutatorRoundMode::_RenderGameModeScore()
{
	if( !m_pScoreSystem || m_nTeam ==0)
		return;

	UINT uiATeamScore, uiBTeamScore;
	m_pScoreSystem->GetGameModeScore( uiATeamScore, uiBTeamScore );


	int iMyScore		= (m_nTeam == PvPCommon::Team::A) ? uiATeamScore : uiBTeamScore;
	int iEnemyScore		= (m_nTeam == PvPCommon::Team::A) ? uiBTeamScore : uiATeamScore;


	if(m_nTeam == PvPCommon::Team::Observer) // 내가 만약 관전자라면
	{
		iMyScore = uiATeamScore;
		iEnemyScore = uiBTeamScore;
	}

	CDnRoundModeHUD * pRoundModeHud = NULL;

	pRoundModeHud = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());	

	//개인 스코어 셋팅 와 팀 스코어 셋팅

	int iMyTeamReamin = 0;
	int iEnemyTeamReamin = 0;

	if( CDnPartyTask::IsActive() ) {
		for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) {
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
			if( !pStruct ) continue;
			if( !pStruct->hActor ) continue;
			if( !pStruct->bCompleteBreakInto ) continue;
			if( pStruct->hActor) 
			{
				if(m_nTeam != PvPCommon::Team::Observer) // 관전자가 아닐때
				{
					if( pStruct->hActor->GetTeam() ==  m_nTeam )
					{
						if( !(pStruct->hActor->IsDie()) )
							++iMyTeamReamin;
					}					
					else
					{
						if( !(pStruct->hActor->IsDie()) )
							++iEnemyTeamReamin;
					}	
				}
				else if(m_nTeam == PvPCommon::Team::Observer) // 관전자 일때
				{
					if( pStruct->hActor->GetTeam() ==  PvPCommon::Team::A )
					{
						if( !(pStruct->hActor->IsDie()) )
							++iMyTeamReamin;
					}					
					else if( pStruct->hActor->GetTeam() ==  PvPCommon::Team::B )
					{
						if( !(pStruct->hActor->IsDie()) )
							++iEnemyTeamReamin;
					}
				}					

				GetInterface().SetPVPGameScore( pStruct->hActor->GetUniqueID(),
					m_pScoreSystem->GetKillCount(pStruct->hActor),
					m_pScoreSystem->GetDeathCount(pStruct->hActor),
					0,                                                // 대장이 없기때문에 0 을 보내줘요
					m_pScoreSystem->GetKillScore(pStruct->hActor),
					m_pScoreSystem->GetAssistScore(pStruct->hActor),
					m_pScoreSystem->GetTotalScore(pStruct->hActor) );
			}
		}
	}

	if( pRoundModeHud )
	{
		pRoundModeHud->SetWinStage( iMyScore , iEnemyScore );
		pRoundModeHud->SetMyScore( iMyTeamReamin );
		pRoundModeHud->SetEnemyScore( iEnemyTeamReamin );

		m_iRound = iMyScore + iEnemyScore + 1;
	}
}

void CDnMutatorRoundMode::RoundEnd(void * pData )
{
	WCHAR  wszResultMSG[256] = L"";

	SCPVP_FINISH_PVPROUND* pPacket = static_cast<SCPVP_FINISH_PVPROUND*>(pData);

	GetInterface().GetHUD()->HaltTimer();

//	m_IsRoundLock = true;

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );

#ifdef PRE_ADD_PVP_DUAL_INFO
	CDnRoundModeHUD *pRoundModeHUD = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());
	if( pRoundModeHUD )
		pRoundModeHUD->ClearDualInfo();
#endif
	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true , PVPResult::RoundEND , false );

	wsprintf( wszResultMSG,L"%s %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringRound ) , m_iRound );

#ifdef PRE_MOD_PVPOBSERVER

	if( CDnActor::s_hLocalActor )
	{
		//--------------------------------------------------------
		// LocalPlayer가 Observer 이면서  Draw 가 아닌경우 UI처리.
		//
		if( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer && 
			pPacket->uiWinTeam != PvPCommon::Team::Max ) 
		{
			GetInterface().GetRespawnModeEnd()->SetResultStringObServer( pPacket->uiWinTeam, wszResultMSG, false, true ); // "승리!"	
		}

		//--------------------------------------------------------
		// PVP 참여플레이어의 경우 기존그대로.
		else
		{
			GetInterface().GetRespawnModeEnd()->SetResultString1( wszResultMSG );

			if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리
			{
				if( CDnActor::s_hLocalActor &&   PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
					GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
				else
					GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );		

				return;
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
			{
				if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
					GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
				else
					GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
				return;
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
			{
				//m_iRound--;
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDraw ));
				return;
			}	
		}
	}

#else

	GetInterface().GetRespawnModeEnd()->SetResultString1( wszResultMSG );

	if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
			GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
		else
			GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );		

		return;
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
			GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringVictory ) );
		else
			GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDefeated ) );
		return;
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
	{
		//m_iRound--;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RoundModeString::UIStringDraw ));
		return;
	}	

#endif // PRE_MOD_PVPOBSERVER

}

