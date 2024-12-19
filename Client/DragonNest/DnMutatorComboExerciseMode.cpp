#include "StdAfx.h"


#ifdef PRE_ADD_PVP_COMBOEXERCISE

#include "TimeSet.h"
#include "TaskManager.h"
#include "DnLocalPlayerActor.h"

#include "DnInterface.h"

#include "PvPScoreSystem.h"
#include "DnComboExerciseModeHUD.h"
#include "DnMutatorComboExerciseMode.h"


CDnMutatorComboExerciseMode::CDnMutatorComboExerciseMode( CDnPvPGameTask * pGameTask ) :
CDnMutatorTeamGame( pGameTask )
{
	//RestartGame();
}

CDnMutatorComboExerciseMode::~CDnMutatorComboExerciseMode()
{
	Release();
}

IScoreSystem * CDnMutatorComboExerciseMode::CreateScoreSystem()
{ 
	return new CPvPScoreSystem();
}
	
bool CDnMutatorComboExerciseMode::Initialize( const UINT uiItemID, DNTableFileFormat*  pSox )
{
	CDnComboExerciseModeHUD * pHud = NULL;
	pHud = static_cast< CDnComboExerciseModeHUD * >( GetInterface().GetHUD() ); // PvpRespawnInGame_AllDlg.ui
	if( pHud )
	{
		pHud->SetRoomName( CDnBridgeTask::GetInstance().GetPVPRoomStatus().wszPVPRoomName.c_str() );
		//pHud->SetStage( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nObjectiveCount );
	}
	
	return true;
}


void CDnMutatorComboExerciseMode::ProcessEnterUser( DnActorHandle hActor , bool isIngameJoin, void * pData )
{
	if( !hActor)
		return;

	WCHAR wszMessage[256];
	SecureZeroMemory(wszMessage,sizeof(wszMessage));

	CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
	// isIngameJoin 이 난입을 뜻함.
	if( pPlayerActor && isIngameJoin )
	{
		if( hActor->GetTeam() != PvPCommon::Team::Observer 
			&& !CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar )
			)
		{
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, GameString::EnterGame ),pPlayerActor->GetName());
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszMessage);
		}
		// 옵져버는 보이지 않게 처리
		else
		{
			if(hActor)
			{
				hActor->Show( false );
				hActor->CmdShowExposureInfo( false );
			}
		}
	}

	GetInterface().AddPVPGameUer( hActor );

}


void CDnMutatorComboExerciseMode::GameStart( int nObjectiveUIString , float nRemainCountDown , float RemainSec , bool IsInGameJoin , float nMaxSec)
{
	if( CDnActor::s_hLocalActor )
	{	
		CTask * pTask = CTaskManager::GetInstance().GetTask("GameTask");
		if( pTask )
		{
			CDnPvPGameTask * pPvPGameTask = dynamic_cast< CDnPvPGameTask * >( pTask );
			if( pPvPGameTask &&  pPvPGameTask->IsPvPComboModeMaster() )
			{
				CDnLocalPlayerActor * pLocalPlayer = reinterpret_cast<CDnLocalPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
				pLocalPlayer->CmdMove( EtVector2(0.0f,0.0f), "Move_Front", -1, 6.0f );
				pLocalPlayer->CmdStop( "Stand", 0, 6.0f );
			}
		}
	}

	GetInterface().RestartRound();

	GetInterface().GetHUD()->SetTime( nMaxSec - RemainSec , CTimeSet().GetTimeT64_GM() , CTimeSet().GetMilliseconds()); // 시간 + 밀리세컨드
//	GetInterface().GetRespawnModeEnd()->Reset();
//	GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_ComboExercise, false , 0 , false );

	if( IsInGameJoin )
	{
		GetInterface().GetHUD()->StartTimer();
//		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
		return;
	}

	if( nRemainCountDown )
	{
		m_fCountDownTime = float(nRemainCountDown);
		GetInterface().BeginCountDown( (int)nRemainCountDown  , GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121021 )  );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );		
	}
	else
	{
		GetInterface().GetHUD()->StartTimer();
//		CDnLocalPlayerActor::LockInput( false );
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption2,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nObjectiveUIString ) , textcolor::YELLOW ,4.0f );
	}

}


#endif // PRE_ADD_PVP_COMBOEXERCISE