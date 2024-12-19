#include "StdAfx.h"
#include "DnMutatorAllKillMode.h"
#include "DnMonsterActor.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPModeEndDlg.h"
#include "PvPScoreSystem.h"
#include "DnAllKillModeHUD.h"
#include "DnLocalPlayerActor.h"
#include "TimeSet.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#endif

CDnMutatorAllKillMode::CDnMutatorAllKillMode( CDnPvPGameTask * pGameTask ):CDnMutatorTeamGame( pGameTask )
{
	RestartGame();

	m_bFirstRound = true;
	m_iRound = 1;

	m_iEnemyUserCount = 0;
	m_iFriendyUserCount = 0;


	m_iFinishDetailReason = 0;
	m_fFinishDetailReasonShowDelay = -1.f;
}

CDnMutatorAllKillMode::~CDnMutatorAllKillMode()
{

}

IScoreSystem* CDnMutatorAllKillMode::CreateScoreSystem()
{
	return new IBoostPoolPvPScoreSystem();
}

bool CDnMutatorAllKillMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());

	if( pAllKillModeHUD )
	{
		pAllKillModeHUD->SetRoomName(CDnBridgeTask::GetInstance().GetPVPRoomStatus().wszPVPRoomName.c_str() );
		pAllKillModeHUD->ResetStage();

		int iUIstring = 0;
		if ( pSox ) 
			iUIstring = pSox->GetFieldFromLablePtr( uiItemID  , "WinCondition_UIString" )->GetInteger();

		WCHAR wszWinCon[256];
		wsprintf(wszWinCon,L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iUIstring ));
		GetInterface().GetHUD()->SetWinCondition( wszWinCon  );
	}

	return InitializeBase( uiItemID, pSox );
}

void CDnMutatorAllKillMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnMutatorTeamGame::Process(LocalTime , fDelta );

	if( m_fFinishDetailReasonShowDelay >= 0 )
	{
		m_fFinishDetailReasonShowDelay -= fDelta;
	}
	else if( m_fFinishDetailReasonShowDelay != -1.f )
	{
		if(m_iFinishDetailReason != 0)
			GetInterface().GetRespawnModeEnd()->SetResultString3( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_iFinishDetailReason  ) );

		m_fFinishDetailReasonShowDelay = -1.f;
		m_iFinishDetailReason = 0;
	}

}

void CDnMutatorAllKillMode::GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec)
{
	if( CDnActor::s_hLocalActor )
		m_nTeam = CDnActor::s_hLocalActor->GetTeam();

	GetInterface().RestartRound();

	GetInterface().GetHUD()->SetTime( nMaxSec - RemainSec , CTimeSet().GetTimeT64_GM() , CTimeSet().GetMilliseconds()); // 시간 + 밀리세컨드
	GetInterface().GetRespawnModeEnd()->Reset();
	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , false , 0 , false );

	if( IsInGameJoin )
	{
		GetInterface().GetHUD()->StartTimer();
		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );

#ifdef PRE_ADD_PVP_DUAL_INFO
		CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
		if( pAllKillModeHUD )
			pAllKillModeHUD->OrderShowDualInfo( true );
#endif
		return;
	}

	if( nRemainCountDown )
	{
		m_fCountDownTime = float(nRemainCountDown);
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringStarting )  );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );		

	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}

	CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
	if( pAllKillModeHUD )
	{
		pAllKillModeHUD->ShowAllKilledDlg( false );
#ifdef PRE_ADD_PVP_DUAL_INFO
		pAllKillModeHUD->OrderShowDualInfo();
#endif
	}
}


bool CDnMutatorAllKillMode::ProcessActorDie( DnActorHandle hKillerActor, DnActorHandle hKilledActor )
{
	WCHAR wszMessage[256];
	SecureZeroMemory(wszMessage,sizeof(wszMessage));

	if( !hKilledActor || !hKillerActor )
		return false;

	if( CDnActor::s_hLocalActor && hKillerActor == CDnActor::s_hLocalActor ) //로컬플레이어가 살인자일때 
	{
		if( hKilledActor->IsPlayerActor() ) 
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::Kill ),hKilledActor->GetName());
			GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );

			// 여기서 로컬의 킬카운트 증가.
			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
			int nIndex = hKillerActor->GetClassID()-1;
			if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
				pPvPInfo->uiKOClassCount[nIndex] += 1;
			return true;
		}
	}

	if( hKillerActor && hKillerActor->IsPlayerActor() ) // 다른 플레이어가 살인자 
	{
		if( CDnActor::s_hLocalActor && hKilledActor == CDnActor::s_hLocalActor ) // 로컬 플레이어가 피해자 
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::Killed ),hKillerActor->GetName());
			GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );

			// 여기서 로컬의 데스카운트 증가.
			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
			int nIndex = hKillerActor->GetClassID()-1;
			if( pPvPInfo && nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
				pPvPInfo->uiKObyClassCount[nIndex] += 1;
			return true;
		}

		if( hKilledActor->IsPlayerActor() ) //다른 플레이어가 피해자 
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::SeeKill ),hKillerActor->GetName(),hKilledActor->GetName());

			if( m_nTeam ==  hKilledActor->GetTeam() ) //피해자가 우리팀 실점
			{
				GetInterface().AddMessageText( wszMessage , textcolor::PVP_E_PLAYER );

			}
			else										//득점
			{
				CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hKilledActor.GetPointer());
				pPlayer->SetExposure( false );
				GetInterface().AddMessageText( wszMessage , textcolor::PVP_F_PLAYER );
			}			
		}		

		if( hKilledActor->IsMonsterActor() )
			return true;
	}

	return false;
}

void CDnMutatorAllKillMode::_RenderGameModeScore()
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

	int iMyTeamReamin = 0;
	int iEnemyTeamReamin = 0;
	int iEnemyUserCount = 0;
	int iFriendUserCount = 0;

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
						iFriendUserCount++;

						if( !(pStruct->hActor->IsDie()) )
							++iMyTeamReamin;
					}					
					else
					{
						iEnemyUserCount++;

						if( !(pStruct->hActor->IsDie()) )
							++iEnemyTeamReamin;
					}	
				}
				else if(m_nTeam == PvPCommon::Team::Observer) // 관전자 일때
				{
					if( pStruct->hActor->GetTeam() ==  PvPCommon::Team::A )
					{
						iFriendUserCount++;

						if( !(pStruct->hActor->IsDie()) )
							++iMyTeamReamin;
					}					
					else if( pStruct->hActor->GetTeam() ==  PvPCommon::Team::B )
					{
						iEnemyUserCount++;

						if( !(pStruct->hActor->IsDie()) )
							++iEnemyTeamReamin;
					}
				}					

				GetInterface().SetPVPGameScore( pStruct->hActor->GetUniqueID(),
					m_pScoreSystem->GetKillCount(pStruct->hActor),
					m_pScoreSystem->GetDeathCount(pStruct->hActor),
					0,                                              
					m_pScoreSystem->GetKillScore(pStruct->hActor),
					m_pScoreSystem->GetAssistScore(pStruct->hActor),
					m_pScoreSystem->GetTotalScore(pStruct->hActor) );
			}
		}
	}

	if( m_iEnemyUserCount <= iEnemyUserCount )
		m_iEnemyUserCount = iEnemyUserCount;

	if( m_iFriendyUserCount <= iFriendUserCount )
		m_iFriendyUserCount = iFriendUserCount;

	CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());	

	if( pAllKillModeHUD )
	{
		pAllKillModeHUD->SetMyScore( iMyTeamReamin );
		pAllKillModeHUD->SetEnemyScore( iEnemyTeamReamin );

		m_iRound = iMyScore + iEnemyScore + 1;
	}
}

void CDnMutatorAllKillMode::EndGame( void * pData )
{
	CPvPScoreSystem::SMyScore * sInfo = NULL;
	sInfo = (CPvPScoreSystem::SMyScore*)m_pScoreSystem->GetScoreStruct(CDnActor::s_hLocalActor->GetName());
	SCPVP_FINISH_PVPMODE *pPacket = (SCPVP_FINISH_PVPMODE*)pData;

	if(!pPacket)
		return;

	GetInterface().GetHUD()->HaltTimer();
	GetInterface().OpenFinalResultDlg();	

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );


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
				GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringVictory ) ); // "승리!"	
		}

		//--------------------------------------------------------
		// PVP 참여플레이어의 경우 기존그대로.
		else
		{
			byte cResult = PVPResult::Win;

			if( m_nTeam == pPacket->uiWinTeam  || pPacket->uiWinTeam == PvPCommon::Team::A )
			{
				cResult = PVPResult::Win;
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringVictory ) );
			}
			else if( pPacket->uiWinTeam == PvPCommon::Team::Max ) //비김
			{
				cResult = PVPResult::Draw;
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringDraw ) );		
			}
			else
			{
				cResult = PVPResult::Lose;
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringDefeated ) );
			}				
		}

		GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true ,PVPResult::Win, true );
	}

#else

	byte cResult = PVPResult::Win;

	if( m_nTeam == pPacket->uiWinTeam  || ( m_nTeam == PvPCommon::Team::Observer && pPacket->uiWinTeam == PvPCommon::Team::A) )
	{
		cResult = PVPResult::Win;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringVictory ) );
	}
	else if( pPacket->uiWinTeam == PvPCommon::Team::Max ) //비김
	{
		cResult = PVPResult::Draw;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringDraw ) );		
	}
	else
	{
		cResult = PVPResult::Lose;
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringDefeated ) );
	}		

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true ,cResult, true );

#endif // PRE_MOD_PVPOBSERVER 


#ifdef PRE_ADD_PVP_DUAL_INFO
	CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
	if( pAllKillModeHUD )
		pAllKillModeHUD->ClearDualInfo();
#endif

}

void CDnMutatorAllKillMode::RoundEnd(void * pData )
{
	if(!CDnActor::s_hLocalActor)
		return;

	WCHAR  wszResultMSG[256] = L"";
	SCPVP_FINISH_PVPROUND* pPacket = static_cast<SCPVP_FINISH_PVPROUND*>(pData);

	GetInterface().GetHUD()->HaltTimer();

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );

#ifdef PRE_ADD_PVP_DUAL_INFO
	CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
	if( pAllKillModeHUD )
		pAllKillModeHUD->ClearDualInfo();
#endif
	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true , PVPResult::RoundEND , false );

	int nWinMessageString = 0;

	if( m_nTeam == pPacket->uiWinTeam || ( m_nTeam == PvPCommon::Team::Observer && pPacket->uiWinTeam == PvPCommon::Team::A ) )
	{
		nWinMessageString = AllKillModeString::UIStringVictory;
	}
	else if( pPacket->uiWinTeam == PvPCommon::Team::Max )
	{
		nWinMessageString = AllKillModeString::UIStringDraw;
		GetInterface().GetRespawnModeEnd()->SetResultString3( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringPerpectDraw ) );
	}
	else
	{
		nWinMessageString = AllKillModeString::UIStringDefeated;
	}

	wsprintf( wszResultMSG,L"%s %d %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringRound ) , m_iRound ,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nWinMessageString ) );
	GetInterface().GetRespawnModeEnd()->SetResultString1( wszResultMSG );
}


void CDnMutatorAllKillMode::SetContinousWinCount( UINT nUserSession , UINT nWinCount )
{
	CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyDataFromSessionID(nUserSession);
	if(pStruct && pStruct->hActor )
	{
		WCHAR  wszResultMSG[256] = L"";

		UINT iRemainUserCount = 0;
		
		if( pStruct->hActor->GetTeam() == m_nTeam || ( m_nTeam == PvPCommon::Team::Observer && pStruct->hActor->GetTeam() == PvPCommon::Team::A) )
			iRemainUserCount = m_iEnemyUserCount;
		else
			iRemainUserCount = m_iFriendyUserCount;

		if( nWinCount > 1 )
		{
#ifdef _RU
			wsprintf( wszResultMSG,L"%s %s %d", pStruct->wszCharacterName , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringKillStreak ) , nWinCount );
#else
			wsprintf( wszResultMSG,L"%s %d %s", pStruct->wszCharacterName , nWinCount , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringKillStreak ) );
#endif
		}
		else
			wsprintf( wszResultMSG,L"%s %s", pStruct->wszCharacterName , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, AllKillModeString::UIStringVictory ) );

		if(nWinCount >= iRemainUserCount)
		{
			if( m_pScoreSystem && m_nTeam != 0)
			{
				CDnAllKillModeHUD *pAllKillModeHUD = static_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
				if( pAllKillModeHUD )
					pAllKillModeHUD->ShowAllKilledDlg( true );
			}
		}
			
		GetInterface().GetRespawnModeEnd()->SetResultString3( wszResultMSG );
	}
}

void CDnMutatorAllKillMode::SetFinishDetailReason( int nUIStringIndex )
{
	m_iFinishDetailReason = nUIStringIndex;
	m_fFinishDetailReasonShowDelay = 3.f;
}
