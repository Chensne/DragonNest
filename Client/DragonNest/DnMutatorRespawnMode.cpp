#include "StdAfx.h"
#include "DnMutatorRespawnMode.h"
#include "DnMonsterActor.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPModeEndDlg.h"
#include "PvPScoreSystem.h"
#include "DnMutatorGame.h"
#include "DnCaptionDlg_04.h"
#include "DnPVPGameResultAllDlg.h"
#include "Timeset.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#endif

CDnMutatorRespawnMode::CDnMutatorRespawnMode( CDnPvPGameTask * pGameTask ):CDnMutatorTeamGame( pGameTask )
{
	RestartGame();
}

CDnMutatorRespawnMode::~CDnMutatorRespawnMode()
{
	
}

IScoreSystem* CDnMutatorRespawnMode::CreateScoreSystem()
{
	return new IBoostPoolPvPScoreSystem();
}


bool CDnMutatorRespawnMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{


	if( GetInterface().GetHUD())
	{
		GetInterface().GetHUD()->SetRoomName(CDnBridgeTask::GetInstance().GetPVPRoomStatus().wszPVPRoomName.c_str() );

		int iUIstring = 0;

		if ( pSox ) 
			 iUIstring = pSox->GetFieldFromLablePtr( uiItemID  , "WinCondition_UIString" )->GetInteger();

		WCHAR wszWinCon[256];
		wsprintf(wszWinCon,L"%d%s",CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIstring ));
		GetInterface().GetHUD()->SetWinCondition( wszWinCon  );
	}	

	return InitializeBase( uiItemID, pSox );
}

void CDnMutatorRespawnMode::GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec)
{

	//자기 팀 셋팅
	if( CDnActor::s_hLocalActor )
	{
		m_nTeam = CDnActor::s_hLocalActor->GetTeam();
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
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringStarting )  );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}

#ifdef PRE_MOD_PVPOBSERVER
	CDnRespawnModeHUD * pHud = dynamic_cast<CDnRespawnModeHUD * >( GetInterface().GetHUD() );
	if( pHud )
		pHud->GameStart();
#endif // PRE_MOD_PVPOBSERVER


}


bool CDnMutatorRespawnMode::ProcessActorDie( DnActorHandle hKillerActor, DnActorHandle hKilledActor )
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
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::Kill ),pPlayer->GetName());

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
			GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::WaitRespawn ), m_pGameTask->GetPlayerDieRespawnDelta() );

			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::Killed ),pPlayer->GetName());

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

			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::SeeKill ),pPlayer->GetName(),pOtherPlayer->GetName());

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
			GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::WaitRespawn ),m_pGameTask->GetPlayerDieRespawnDelta());
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

void CDnMutatorRespawnMode::EndGame( void * pData )
{

	CPvPScoreSystem::SMyScore * sInfo = NULL;
	sInfo = (CPvPScoreSystem::SMyScore*)m_pScoreSystem->GetScoreStruct(CDnActor::s_hLocalActor->GetName());

	SCPVP_FINISH_PVPMODE * pPacket = NULL;	

	pPacket = ( SCPVP_FINISH_PVPMODE * )pData;

	GetInterface().GetHUD()->HaltTimer();
	GetInterface().OpenFinalResultDlg();

	byte cResult = PVPResult::Win;

	if(IsIndividualMode())
	{

		WCHAR wzStrName[128];
		SecureZeroMemory(wzStrName,sizeof(wzStrName));

		
		if(pPacket->uiWinTeam == PvPCommon::Team::Max) // 무승부 메세지가 날라온경우
		{
			cResult = PVPResult::Draw;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringTimeOver ) );
			GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString(CEtUIXML::idCategory1,RespawnModeString::UIStringIndividualDraw),textcolor::YELLOW,3.f,GetInterface().GetCaptionDlg_04()->SecondCaption); 
			GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Respawn  , true ,cResult, true );
			return;
		}

		if(CDnActor::s_hLocalActor && GetInterface().GetPVPGameResultAllDlg()->GetBestUserName() == CDnActor::s_hLocalActor->GetName())
			cResult = PVPResult::Win;   
		else
			cResult = PVPResult::Lose;  
		
		wsprintf(wzStrName,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringIndividualVictory ),GetInterface().GetPVPGameResultAllDlg()->GetBestUserName().c_str());
		GetInterface().GetRespawnModeEnd()->SetResultString2(wzStrName);
		
		GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Respawn  , true ,cResult, true );
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
				GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringVictory ) ); // "승리!"	
		}
	
		//--------------------------------------------------------
		// PVP 참여플레이어의 경우 기존그대로.
		else
		{
			if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리 
			{
				if( PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() )
				{
					cResult = PVPResult::Win;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringVictory ) );
				}
				else
				{
					cResult = PVPResult::Lose;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDefeated ) );
				}
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
			{
				if( PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
				{
					cResult = PVPResult::Win;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringVictory ) );			
				}
				else
				{
					cResult = PVPResult::Lose;
					GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDefeated ) );
				}
			}

			if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
			{
				cResult = PVPResult::Draw;
				GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDraw ) );
			}

		}

	}

#else	
	if( pPacket-> uiWinTeam == PvPCommon::Team::A ) //Ateam 승리 
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::A == CDnActor::s_hLocalActor->GetTeam() || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
		{
			cResult = PVPResult::Win;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringVictory ) );
		}
		else
		{
			cResult = PVPResult::Lose;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDefeated ) );
		}
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::B ) //Bteam 승리
	{
		if( CDnActor::s_hLocalActor &&   PvPCommon::Team::B == CDnActor::s_hLocalActor->GetTeam() )
		{
			cResult = PVPResult::Win;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringVictory ) );			
		}
		else
		{
			cResult = PVPResult::Lose;
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDefeated ) );
		}
	}

	if( pPacket-> uiWinTeam == PvPCommon::Team::Max ) //비김
	{
		cResult = PVPResult::Draw;
		GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, RespawnModeString::UIStringDraw ) );
	}
#endif // PRE_MOD_PVPOBSERVER

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Respawn  , true ,cResult, true );
	
}

void CDnMutatorRespawnMode::_RenderGameModeScore()
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
					m_pScoreSystem->GetTotalScore(pStruct->hActor) );
			}
	}
}


void CDnMutatorRespawnMode::PushUsedRespawnArea( const std::string& name )
{
	m_UsedRespawnAreaList.push_back( name );
}

bool CDnMutatorRespawnMode::bIsUsedRespawnArea( const std::string& name )
{
	std::list<std::string>::iterator itor = std::find( m_UsedRespawnAreaList.begin(), m_UsedRespawnAreaList.end(), name );
	if( itor == m_UsedRespawnAreaList.end() )
		return false;
	return true;
}
