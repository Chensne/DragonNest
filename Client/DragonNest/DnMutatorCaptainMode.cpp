#include "StdAfx.h"
#include "DnMutatorCaptainMode.h"
#include "DnMonsterActor.h"
#include "DnPlayerActor.h"
#include "DnInterface.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPModeEndDlg.h"
#include "PvPScoreSystem.h"
#include "DnRoundModeHUD.h"
#include "DnLocalPlayerActor.h"
#include "PvPCaptainScoreSystem.h"
#include "DnCaptionDlg_04.h"
#include "TimeSet.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#endif

CDnMutatorCaptainMode::CDnMutatorCaptainMode( CDnPvPGameTask * pGameTask ):CDnMutatorTeamGame( pGameTask )
{
	RestartGame();

	m_hFriendlyCaptain.Identity();

	m_bFirstRound = true;
	m_bIsEndGame = false;
	m_bWarningMsg = true;
	m_bState_RoundEnd = false;
	m_bAlramCaptainCondition = false;

	m_iRound = 1;
	m_nRoundEndString = 0;
	m_nWinTeam = 0;
	m_fRoundEndCount = 0.f;
	m_fWarningMsgCount = 0.f;

}

CDnMutatorCaptainMode::~CDnMutatorCaptainMode()
{

}

IScoreSystem* CDnMutatorCaptainMode::CreateScoreSystem()
{
	return new CPvPCaptainScoreSystem();
}

bool CDnMutatorCaptainMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	CDnRoundModeHUD *pRoundModeHud = static_cast<CDnRoundModeHUD*>(GetInterface().GetHUD());
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


void CDnMutatorCaptainMode::GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec)
{
	m_bState_RoundEnd = false;

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
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
		return;
	}

	if( nRemainCountDown )
	{
		m_fCountDownTime = float(nRemainCountDown);
		
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringStarting )  , GetInterface().eCaptain);
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}
}


bool CDnMutatorCaptainMode::ProcessActorDie( DnActorHandle hKillerActor, DnActorHandle hKilledActor )
{
	if( !CDnActor::s_hLocalActor )
		return false;

	if( !hKilledActor || !hKillerActor )
		return false;

	if( hKillerActor == CDnActor::s_hLocalActor )
	{
		GetInterface().AddMessageText( 
			FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::Kill ) , hKilledActor->GetName() ).c_str(), 
			textcolor::PVP_F_PLAYER );

		TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
		if( pPvPInfo )
		{
			int nIndex = hKilledActor->GetClassID()-1;
			if( nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
				pPvPInfo->uiKOClassCount[nIndex] += 1;
		}

		return true;
	}

	if( hKilledActor->IsPlayerActor() )
	{
		CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor *>(hKilledActor.GetPointer());
		if( !pPlayerActor ) return false;

		if( pPlayerActor->IsLocalActor() )
		{
			if( pPlayerActor->IsCaptainMode() == false )
				GetInterface().OpenpRespawnGauageDlg(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::WaitRespawn ), m_pGameTask->GetPlayerDieRespawnDelta() );

			GetInterface().AddMessageText( 
				FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::Killed ), hKillerActor->GetName() ).c_str(),
				textcolor::PVP_E_PLAYER );

			TPvPGroup *pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
			if( pPvPInfo )
			{
				int nIndex = hKillerActor->GetClassID()-1;
				if( nIndex >= 0 && nIndex < PvPCommon::Common::MaxClass )
					pPvPInfo->uiKObyClassCount[nIndex] += 1;
			}
		}
		else if( hKillerActor->IsPlayerActor() )
		{
			DWORD dwColor = m_nTeam == hKilledActor->GetTeam() ? textcolor::PVP_E_PLAYER : textcolor::PVP_F_PLAYER;
			GetInterface().AddMessageText( 
				FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::SeeKill ),
				hKillerActor->GetName(),
				hKilledActor->GetName() ).c_str(), 
				dwColor );

			if( m_nTeam != hKilledActor->GetTeam() )
				pPlayerActor->SetExposure( false );
		}
	}

	return true;
}

void CDnMutatorCaptainMode::EndGame( void * pData )
{
	m_bState_RoundEnd = true;
	m_bIsEndGame = true;

	CPvPScoreSystem::SMyScore *sInfo = (CPvPScoreSystem::SMyScore*)m_pScoreSystem->GetScoreStruct(CDnActor::s_hLocalActor->GetName());
	if( !sInfo ) return;

	SCPVP_FINISH_PVPMODE * pPacket = ( SCPVP_FINISH_PVPMODE * )pData;
	if( !pPacket ) return;

	GetInterface().GetHUD()->HaltTimer();
	GetInterface().OpenFinalResultDlg();	

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );

	byte cResult = PVPResult::Win;

	if( CDnActor::s_hLocalActor )
	{
		int nResultString = CaptainModeString::UIStringVictory;
		if( pPacket->uiWinTeam == PvPCommon::Team::Max )
		{
			cResult = PVPResult::Draw;
			GetInterface().GetRespawnModeEnd()->Reset();
			nResultString = CaptainModeString::UIStringDraw;	
		}
		else if( pPacket->uiWinTeam == m_nTeam || (m_nTeam == PvPCommon::Team::Observer && pPacket->uiWinTeam == PvPCommon::Team::A) )
		{
			cResult = PVPResult::Win;
			nResultString = CaptainModeString::UIStringVictory;
		}
		else
		{
			cResult = PVPResult::Lose;
			nResultString = CaptainModeString::UIStringDefeated;
		}

#ifdef PRE_MOD_PVPOBSERVER
		// LocalPlayer가 Observer 이면서  Draw 가 아닌경우 UI처리.
		if( m_nTeam == PvPCommon::Team::Observer && pPacket->uiWinTeam != PvPCommon::Team::Max ) 
		{
			GetInterface().GetRespawnModeEnd()->SetResultStringObServer( pPacket->uiWinTeam, 
				GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringVictory )); // "승리!"
		}
		else
		{
			GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nResultString ) );
		}
#else
		GetInterface().GetRespawnModeEnd()->SetResultString2(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nResultString ) );
#endif

	}

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round, true ,cResult, true );
}

void CDnMutatorCaptainMode::_RenderGameModeScore()
{
	if( !m_pScoreSystem || m_nTeam ==0)
		return;

	CDnRoundModeHUD *pRoundModeHud = static_cast< CDnRoundModeHUD *> (GetInterface().GetHUD());	
	if( !pRoundModeHud ) return;

	UINT uiATeamScore = 0 , uiBTeamScore = 0;
	m_pScoreSystem->GetGameModeScore( uiATeamScore, uiBTeamScore );

	int iMyTeamReamin = 0;
	int iEnemyTeamReamin = 0;

	if( CDnPartyTask::IsActive() ) 
	{
		for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) 
		{
			CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);

			if( !pStruct ) continue;
			if( !pStruct->hActor ) continue;
			if( !pStruct->bCompleteBreakInto ) continue;
			if( pStruct->hActor) 
			{
				if( pStruct->hActor->GetTeam() ==  m_nTeam || ( m_nTeam == PvPCommon::Team::Observer && pStruct->hActor->GetTeam() ==  PvPCommon::Team::A ) )
				{
					if( !(pStruct->hActor->IsDie()) )
						++iMyTeamReamin;
				}					
				else
				{
					if( !(pStruct->hActor->IsDie()) )
						++iEnemyTeamReamin;
				}	

				GetInterface().SetPVPGameScore( pStruct->hActor->GetUniqueID(),
					m_pScoreSystem->GetKillCount(pStruct->hActor),
					m_pScoreSystem->GetDeathCount(pStruct->hActor),
					m_pScoreSystem->GetCaptainKillCount(pStruct->hActor),
					m_pScoreSystem->GetKillScore(pStruct->hActor),
					m_pScoreSystem->GetAssistScore(pStruct->hActor),
					m_pScoreSystem->GetTotalScore(pStruct->hActor) );
			}
		}
	}
	

	if( pRoundModeHud )
	{
		pRoundModeHud->SetWinStage( 
			(m_nTeam == PvPCommon::Team::A || m_nTeam == PvPCommon::Team::Observer) ? uiATeamScore : uiBTeamScore, 
			(m_nTeam == PvPCommon::Team::A || m_nTeam == PvPCommon::Team::Observer) ? uiBTeamScore : uiATeamScore);

		pRoundModeHud->SetMyScore( iMyTeamReamin );
		pRoundModeHud->SetEnemyScore( iEnemyTeamReamin );

		m_iRound = uiATeamScore + uiBTeamScore + 1;
	}
}


void CDnMutatorCaptainMode::RoundEnd(void * pData )
{
	SCPVP_FINISH_PVPROUND *pPacket = static_cast<SCPVP_FINISH_PVPROUND*>(pData);
	if( !pPacket ) return;
	
	int nRoundEndString =0;
	m_bState_RoundEnd = true;

	if( m_pScoreSystem )
		m_pScoreSystem->SetGameModeScore( pPacket->uiATeamScore, pPacket->uiBTeamScore );

	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , true , PVPResult::RoundEND , false );
	GetInterface().ShowRespawnGauageDlg(false);

	WCHAR  wszResultMSG[256] = L"";
	wsprintf( wszResultMSG,L"%s %d",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringRound ), m_iRound );

	if( CDnActor::s_hLocalActor )
	{
		GetInterface().GetRespawnModeEnd()->SetResultString1( wszResultMSG );
		m_bWarningMsg = true;

		if( pPacket->uiWinTeam == PvPCommon::Team::Max )
		{
			nRoundEndString = CaptainModeString::UIStringDraw;
		}
		else if( pPacket->uiWinTeam == CDnActor::s_hLocalActor->GetTeam() || (m_nTeam == PvPCommon::Team::Observer && pPacket->uiWinTeam == PvPCommon::Team::A) )
		{
			nRoundEndString = CaptainModeString::UIStringVictory;
		}
		else
		{
			nRoundEndString = CaptainModeString::UIStringDefeated;
		}
	}

	m_nWinTeam = pPacket->uiWinTeam; 
	m_nRoundEndString = nRoundEndString;

	if(GetInterface().GetHUD()->GetRemainSec()<1)
	{
		GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringTimeOver));
		SetTimeOverMessage(4.f);
		GetInterface().GetHUD()->HaltTimer();
		return;
	}

	GetInterface().GetHUD()->HaltTimer();

	if( pPacket->uiWinTeam == PvPCommon::Team::Max )
	{
		GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringDraw ) );
	}
	else
	{
		if( m_bIsEndGame == false )	
		{
#ifdef PRE_MOD_PVPOBSERVER
			// LocalPlayer가 Observer 이면서  Draw 가 아닌경우 UI처리.
			if( m_nTeam == PvPCommon::Team::Observer ) 
				GetInterface().GetRespawnModeEnd()->SetResultStringObServer( pPacket->uiWinTeam, wszResultMSG, false, true ); // "승리!"	
			else
				GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nRoundEndString ));
#else
			GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nRoundEndString ));
#endif
		}
	}
		
}

void CDnMutatorCaptainMode::OnInitializeBreakIntoActor( DnActorHandle hPlayerActor )
{
	if( hPlayerActor )
	{
		if(hPlayerActor->IsPlayerActor())
		{
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(hPlayerActor.GetPointer());
			pPlayer->ToggleCaptainMode(false);
		}
	}
}

void CDnMutatorCaptainMode::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_fCountDownTime )
	{
		m_fCountDownTime -= fDelta;

		CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
 		if( m_fCountDownTime < 0.0f)
		{
			if( GetInterface().GetHUD() )
			{
				GetInterface().GetHUD()->StartTimer();
				m_fCountDownTime = 0.0f;
			}
			if( pWorldEnv )
				pWorldEnv->ResetSaturation();
		}else
		{
			if( pWorldEnv )
				pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, 0.0f, 1000 );
		}

	}

	if(GetInterface().GetRespawnModeEnd() && m_fRoundEndCount!= 0)
		ProcessTimeOverMessage(fDelta);

	if(GetInterface().GetHUD() && GetInterface().GetHUD()->GetRemainSec()!=0) 
		ProcessWarningMessage(LocalTime,fDelta);
	
	if( GetInterface().GetHUD() )
		GetInterface().ToggleShowAllPVPPlayerInMap( GetInterface().GetHUD()->NearEndGame() );

	if(m_bState_RoundEnd)
		GetInterface().ShowRespawnGauageDlg(false);

	_RenderGameModeScore();
}

void CDnMutatorCaptainMode::ProcessTimeOverMessage(float fDelta)
{
	if(m_fRoundEndCount>0)
		m_fRoundEndCount -= fDelta;

	if(!m_nRoundEndString)
	{
		m_fRoundEndCount = 0;
		return;
	}
 
	if (m_fRoundEndCount < 1 )
	{
		if( CDnActor::s_hLocalActor )
		{
			int nMyTeam = CDnActor::s_hLocalActor->GetTeam();
			if( nMyTeam == PvPCommon::Team::Observer )
				nMyTeam = PvPCommon::Team::A;

			if( m_nWinTeam == PvPCommon::Team::Max )
			{
				GetInterface().GetRespawnModeEnd()->SetResultString1(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, CaptainModeString::UIStringDraw ));
			}
			else
			{
				int nEndString = ( m_nWinTeam == nMyTeam ) ?  CaptainModeString::UIStringVictory : CaptainModeString::UIStringDefeated;
				GetInterface().GetRespawnModeEnd()->SetResultString3(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nEndString ));
			}
		}

		m_fRoundEndCount = 0;
	}
}


void CDnMutatorCaptainMode::ProcessWarningMessage(LOCAL_TIME LocalTime, float fDelta)
{
	if( m_hFriendlyCaptain && m_hFriendlyCaptain->IsPlayerActor() )
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(m_hFriendlyCaptain.GetPointer());
		if( pPlayer && pPlayer->IsCaptainMode() && pPlayer->GetHP() < (pPlayer->GetMaxHP() / 5) )
		{
			if( m_bWarningMsg == true )
			{
				GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString(CEtUIXML::idCategory1,120066),textcolor::ORANGERED,3.f,GetInterface().GetCaptionDlg_04()->FirstCaption); // 대장이 위험하다 !!
				m_bWarningMsg = false;
			}

			m_bAlramCaptainCondition = true;
		}
		else
		{
			m_bAlramCaptainCondition = false;
		}
	}
	
	if(!m_bWarningMsg)
	{
		m_fWarningMsgCount += fDelta;

		if(m_fWarningMsgCount > 10.f)
		{
			m_bWarningMsg = true;
			m_fWarningMsgCount = 0.f;
		}
	}

}

void CDnMutatorCaptainMode::RenderCustom( float fElapsedTime )
{
#ifdef PRE_ADD_71342
	RenderCaptainPointMark();
#endif
}

void CDnMutatorCaptainMode::RenderCaptainPointMark()
{
	if( !m_bAlramCaptainCondition ) return;
	if( (CDnActor::s_hLocalActor && m_hFriendlyCaptain && m_hFriendlyCaptain != CDnActor::s_hLocalActor) == false ) return;
	if( CDnActor::s_hLocalActor->IsDie() ) return;
	if( CDnActor::s_hLocalActor->GetTeam() != m_hFriendlyCaptain->GetTeam() ) return;

	EtCameraHandle hEtCam = CEtCamera::GetActiveCamera();
	if( !hEtCam ) return;

	EtTextureHandle hTex = CDnInterface::GetInstance().GetPointMarkTex();
	EtTextureHandle hTailTex = CDnInterface::GetInstance().GetPointMarkTailTex();
	if( !hTex || !hTailTex ) return;

	EtVector3 vecDistance = *CDnActor::s_hLocalActor->GetPosition() - *m_hFriendlyCaptain->GetPosition();
	vecDistance.y = 0.f;
	if( EtVec3Length( &vecDistance ) < 750.f ) return;


	if( hEtCam->GetPosition() && hEtCam->GetDirection() )
	{
		float fDegree = 0.f;

		EtVector3 vHeadPosition = m_hFriendlyCaptain->GetHeadPosition();
		EtVector3 vActorPos = *m_hFriendlyCaptain->GetPosition();
		EtVector3 vCurlook = *hEtCam->GetDirection();

		EtVector3 vLookFromMember;
		EtVec3Normalize(&vLookFromMember, &(vHeadPosition - *hEtCam->GetPosition()));
		vLookFromMember.y = vCurlook.y;

		SUICoord screenUICoord;
		screenUICoord.SetSize( 128.f / (float)GetEtDevice()->Width(),128.f / (float)GetEtDevice()->Height());

		if( EtVec3Dot(&vCurlook, &vLookFromMember) < 0.f )
		{
			EtVector3  vCross;
			screenUICoord.fY = 1.f - screenUICoord.fHeight * 2.f;
			EtVec3Cross( &vCross, &vCurlook, &(EtVector3(0.f, 1.f, 0.f)) );
			if( vCross.y > 0.0f )
				screenUICoord.fX = 1.f - screenUICoord.fWidth;
			else
				screenUICoord.fX = 0.f;

			fDegree = EtToDegree( atan2f( -(screenUICoord.fX - 0.5f), screenUICoord.fY - 0.5f ) );
		}
		else
		{
			EtViewPort vp;
			EtMatrix* matViewProj;
			EtVector3 vScreenPos;
			float TEXTURE_HALF = screenUICoord.fWidth / 2;

			GetEtDevice()->GetViewport(&vp);
			matViewProj = hEtCam->GetViewProjMatForCull();
			EtVec3TransformCoord( &vScreenPos, &vHeadPosition, matViewProj );
			EtVec3TransformCoord( &vActorPos, &vActorPos, matViewProj );

			screenUICoord.fX = (( vScreenPos.x + 1.0f ) / 2.0f + vp.X);
			screenUICoord.fY = (( 2.0f - ( vScreenPos.y + 1.35f ) ) / 2.0f + vp.Y);

			CommonUtil::ClipNumber(screenUICoord.fX, TEXTURE_HALF, 1.f - TEXTURE_HALF);
			CommonUtil::ClipNumber(screenUICoord.fY, TEXTURE_HALF, 1.f - TEXTURE_HALF);
			CommonUtil::ClipNumber(vScreenPos.x, -1.f + TEXTURE_HALF , 1.f - TEXTURE_HALF );

			fDegree = EtToDegree( atan2f( vScreenPos.x - vActorPos.x , (0.5f - screenUICoord.fY)  - vActorPos.y ) );

			screenUICoord.fX -= TEXTURE_HALF;
			float fEndOfY = 1.f - screenUICoord.fHeight * 2.f;
			if (screenUICoord.fY > fEndOfY)
				screenUICoord.fY = fEndOfY;
		}
		
		int CAPTAIN_TEXTURE_INDEX = 9; 
		int nCountX = 4;
		DWORD dwColor = EtInterface::textcolor::WHITE;
		SUICoord UVCoord = SUICoord( 
			(128.f * (float)( CAPTAIN_TEXTURE_INDEX % nCountX)) / hTex->Width(), 
			(128.f * (float)( CAPTAIN_TEXTURE_INDEX / nCountX)) / hTex->Height(), 
			128.f / hTex->Width(), 128.f / hTex->Height() );

		SUICoord tailUVCoord = SUICoord( 0.f, 0.f, 1.f, 1.f );

		CEtSprite::GetInstance().Begin(0);
		CEtSprite::GetInstance().DrawSprite((EtTexture*)hTex->GetTexturePtr(), hTex->Width(), hTex->Height(), UVCoord, dwColor, screenUICoord, 0.f);
		CEtSprite::GetInstance().DrawSprite((EtTexture*)hTailTex->GetTexturePtr(), hTailTex->Width(), hTailTex->Height(), tailUVCoord, dwColor, screenUICoord, fDegree);
		CEtSprite::GetInstance().End();
	}
}
