#include "StdAfx.h"
#include "DnPvPGameTask.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "DnActorClassDefine.h"
#include "DnPlayerActor.h"
#include "DnItemTask.h"
#include "DnStateBlow.h"
#include "GameSendPacket.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"

#include "GlobalValue.h"
#include "DnTableDB.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnCommonDef.h"
#include "DnRespawnModeHUD.h"
#include "PvPScoreSystem.h"
#include "DnDropItem.h"
#include "GameOption.h"
#include "SystemSendPacket.h"
#include "DnCaptionDlg_04.h"
#include "DnPlayerCamera.h"
#include "navigationmesh.h"
#include "DnRadioMsgTask.h"
#include "DnMutatorRoundMode.h"
#include "DnMutatorRespawnMode.h"
#include "DnMutatorCaptainMode.h"
#include "DnMainFrame.h"
#include "DnRevengeTask.h"

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
#include "DnMainDlg.h"
#include "DnChatTabDlg.h"
#include "DnChatDlg.h"
#endif

#ifdef PRE_MOD_PVPOBSERVER
#include "DnPvPObserverOrderListDlg.h"
#endif
#include "DnAllKillModeHUD.h"

#include "DnMutatorZombieMode.h"

#include "DnMutatorGuildWar.h"
#include "DnGuildWarModeHUD.h"
#include "DnGuildZoneMapDlg.h"
#include "DnOccupationTask.h"
#include "DnMutatorOccupationMode.h"
#include "DnMutatorAllKillMode.h"

#include "DnMutatorRacingMode.h"
#ifdef PRE_ADD_PVP_TOURNAMENT
#include "DnMutatorPVPTournamentMode.h"
#include "DnPVPTournamentDataMgr.h"
#include "DnPVPTournamentModeHud.h"
#endif

#ifdef PRE_ADD_PVP_COMBOEXERCISE
#include "PvPSendPacket.h"
#include "DnPartyTask.h"
#include "DnMutatorComboExerciseMode.h"
#include "DnComboExerciseModeHUD.h"
#endif // PRE_ADD_PVP_COMBOEXERCISE


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnPvPGameTask::CDnPvPGameTask()
{
	m_GameTaskType = GameTaskType::PvP;
	m_fPlayerDieRespawnDelta = CDnActor::s_fDieDelta;
	m_pMutatorClass = NULL;
	
	nObjectiveUIString = 0;
	m_cGameMode = 0;
	m_iKOItemDropTableID = 0;
	m_fKickElapsedTime = 0.0f;
	m_nWaringNum = 0;
	m_lKickTime = 0;
	m_dwKey = 0;
	m_nMedalExp = 0;

	m_isInGameJoin = false;
	m_IsStartRegulation = false;
	m_nLadderType = 0;

	m_nBattleGroundID = 0;
	m_nNextObservingPartyPlayerIndex = 0;

#ifdef PRE_MOD_PVPOBSERVER	
	m_nCrrObservingPlayerIndex = -1;
	m_uiSessionBattlePlayer[0] = m_uiSessionBattlePlayer[1] = 0;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_COMBOEXERCISE
	m_bPvPComboModeMaster = false; // 콤보연습모드 방장여부.
#endif // PRE_ADD_PVP_COMBOEXERCISE

	// pvp 게임 중엔 몬스터 등장으로 인한 전투 BGM 켜고 끄기를 수행하지 않는다. (#10800)
	CDnWorld::GetInstance().SetProcessBattleBGM( false );
}

CDnPvPGameTask::~CDnPvPGameTask()
{
	//PVP게임후에는 파티가 존재 할수없다.
	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( pPartyTask )
		pPartyTask->RequestInitParty();
	SAFE_DELETE(m_pMutatorClass);

	CDnWorld::GetInstance().SetProcessBattleBGM( true );
	if(GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival && CDnRadioMsgTask::IsActive() )
		GetRadioMsgTask().ClearTransformSound();
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	GetInterface().ResetPartyPlayerGaugeList();
	GetInterface().ResetPartyPlayerGaugeAttributes( true , true );
#endif

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
	GetInterface().ShowPvpPartyControl( false );
#endif

#if defined( PRE_ADD_REVENGE )
	CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
	if( NULL != pRevengeTask )
		pRevengeTask->ClearData();
#endif	// #if defined( PRE_ADD_REVENGE )
}

//blondy
bool CDnPvPGameTask::Initialize()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );

	if( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID  != 0 )
	{		
		if ( pSox ) 
		{
			m_nLadderType = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "LadderType" )->GetInteger();
			if( pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "Respawn_Time" ) && pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "Respawn_Time" )->GetInteger() > 0 )
				m_fPlayerDieRespawnDelta = float(pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "Respawn_Time" )->GetInteger());
		
			m_cGameMode = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "GamemodeID" )->GetInteger();
			m_nBattleGroundID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "_BattleGroundID" )->GetInteger();
			m_iKOItemDropTableID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "KOItemDropTableID" )->GetInteger();
			m_IsStartRegulation = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "StartRegulation" )->GetInteger()?true:false;
			m_nMedalExp = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "MedalExp" )->GetInteger();
			_ASSERT(m_nMedalExp&&"pvp 메달경험치가 0입니다. 나눗셈에 사용되니 PvpGameModeTable 데이터를 확인해주세요.");
			
			switch( m_cGameMode )
			{
			case PvPCommon::GameMode::PvP_Respawn:
				m_pMutatorClass = new CDnMutatorRespawnMode(this); 
				break;
			case PvPCommon::GameMode::PvP_Round:
				m_pMutatorClass = new CDnMutatorRoundMode(this);
				break;
			case PvPCommon::GameMode::PvP_Captain:
				m_pMutatorClass = new CDnMutatorCaptainMode(this); // 새로운 캡틴모드를 사용
				break;
			case PvPCommon::GameMode::PvP_IndividualRespawn:
				m_pMutatorClass = new CDnMutatorRespawnMode(this); // 개인전은 리스폰 모드를 사용합니다.
				
				if(m_pMutatorClass)
				{
					SetPVPShowHelmetMode(true); // 개인전은 뚜껑을 보여주게 됩니다.
					CDnMutatorRespawnMode *pRespawnMode = dynamic_cast<CDnMutatorRespawnMode*>(m_pMutatorClass);
					pRespawnMode->SetHairColorMode(false);          // 머리색 설정 사용안함 : Default 는 True ( TeamGame ) 입니다.
					pRespawnMode->SetIndividualMode(true);          // 나는 개인전 입니다.
			
				}
				break;
			case PvPCommon::GameMode::COOP_Defense:
				m_pMutatorClass = new CDnMutatorRespawnMode(this);
				break;
			case PvPCommon::GameMode::COOP_Survival:
				m_pMutatorClass = new CDnMutatorRespawnMode(this);
				break;
			case PvPCommon::GameMode::PvP_Zombie_Survival:
				m_pMutatorClass = new CDnMutatorZombieMode(this);

				if(m_pMutatorClass)
				{
					SetPVPShowHelmetMode(true);                    // 개인전은 뚜껑을 보여주게 됩니다.
					CDnMutatorZombieMode *pZombieMode = dynamic_cast<CDnMutatorZombieMode*>(m_pMutatorClass);
					pZombieMode->SetHairColorMode(false);          // 머리색 설정 사용안함 : Default 는 True ( TeamGame ) 입니다.
				}
				break;
			case PvPCommon::GameMode::PvP_GuildWar :
				m_pMutatorClass = new CDnMutatorGuildWarMode(this);
				break;
			case PvPCommon::GameMode::PvP_Occupation:
				m_pMutatorClass = new CDnMutatorOccupationMode(this);
				break;
			case PvPCommon::GameMode::PvP_AllKill:
	#ifdef _TEST_CODE_KAL
				m_pMutatorClass = new CDnMutatorPVPTournamentMode(this);
	#else
				m_pMutatorClass = new CDnMutatorAllKillMode(this);
	#endif
				break;
#if defined( PRE_ADD_RACING_MODE )
			case PvPCommon::GameMode::PvP_Racing:
				m_pMutatorClass = new CDnMutatorRacingMode( this );
				if(m_pMutatorClass)
				{
					SetPVPShowHelmetMode(true); // 개인전은 뚜껑을 보여주게 됩니다.
					CDnMutatorRacingMode * pRacingMode = dynamic_cast<CDnMutatorRacingMode*>(m_pMutatorClass);
					if (pRacingMode)
					{
						pRacingMode->SetHairColorMode(false);          // 머리색 설정 사용안함 : Default 는 True ( TeamGame ) 입니다.
						pRacingMode->SetIndividualMode(true);          // 나는 개인전 입니다.
					}
				}
				break;
#endif	// #if defined( PRE_ADD_RACING_MODE )

#ifdef PRE_ADD_PVP_TOURNAMENT
			case PvPCommon::GameMode::PvP_Tournament:
				{
					m_pMutatorClass = new CDnMutatorPVPTournamentMode(this);
				}
				break;
#endif

#ifdef PRE_ADD_PVP_COMBOEXERCISE
			case PvPCommon::GameMode::PvP_ComboExercise:
				m_pMutatorClass = new CDnMutatorComboExerciseMode(this);			
				m_bPvPComboModeMaster = false; // 콤보연습모드 방장여부.				
				break;
#endif // PRE_ADD_PVP_COMBOEXERCISE

			}			
		}		
	}

	InitializeInterface();

	if( m_pMutatorClass )
		m_pMutatorClass->Initialize( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID, pSox );
	if(GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival)
	{
		GetRadioMsgTask().LoadTransformSound();
	}
#ifdef PRE_MOD_PVPOBSERVER		
	m_uiSessionBattlePlayer[0] = m_uiSessionBattlePlayer[1] = 0;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
	if( IsShowPartyGauge() == true )
		GetInterface().ShowPvpPartyControl( true );
#endif

	return CDnGameTask::Initialize();
}

void CDnPvPGameTask::SetSyncComplete( bool bFlag )
{
	CDnGameTask::SetSyncComplete( bFlag );	
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	GetInterface().RefreshPvpPlayerGauge();
#endif
}

void CDnPvPGameTask::OnInitializeStartPosition( CDnPartyTask::PartyStruct *pStruct, const int iPartyIndex, EtVector3& vStartPos, float& fRotate, const UINT uiSessionID )
{
	DnActorHandle hActor = pStruct->hActor;
	if( hActor )
	{
		hActor->SetTeam( pStruct->usTeam );

		CEtWorldEventControl*	pControl	= NULL;
		CEtWorldSector*			pSector		= NULL;
		CEtWorldEventArea*		pArea		= NULL;

		std::vector<CEtWorldEventArea*> vRespawnArea;

		for( DWORD i=0; i<m_pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
		{
			pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
			pControl = pSector->GetControlFromUniqueID( ETE_PvPRespawnArea );
			if( !pControl ) 
				continue;

			for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
			{
				pArea = pControl->GetAreaFromIndex(j);
				if( pArea )
				{
					PvPRespawnAreaStruct* pStruct = reinterpret_cast<PvPRespawnAreaStruct*>(pArea->GetData());

					bool bTeamCheck = false;
					if( hActor->GetTeam() == PvPCommon::Team::Observer || pStruct->nTeam == hActor->GetTeam() )
						bTeamCheck = true;

					if( m_cGameMode == PvPCommon::GameMode::PvP_IndividualRespawn ||
						 m_cGameMode == PvPCommon::GameMode::PvP_Racing )
					{
						vRespawnArea.push_back( pArea );
						continue;
					}

#ifdef PRE_ADD_PVP_TOURNAMENT
					if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
						bTeamCheck = true;
#endif
					if( m_cGameMode == PvPCommon::GameMode::PvP_Zombie_Survival )
					{
						bTeamCheck = (pStruct->nTeam == PvPCommon::Team::Human);
					}
					if( pStruct && pStruct->IsStartArea && bTeamCheck )
						vRespawnArea.push_back( pArea );
				}
			}
		}

		if( !vRespawnArea.empty() )
		{
#ifdef PRE_ADD_PVP_TOURNAMENT
			CEtWorldEventArea* pRespawnArea = NULL;
			if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
			{
				if (vRespawnArea.size() < 2)
				{
					pRespawnArea = vRespawnArea[0];
				}
				else
				{
					bool teamA = false;
					const CDnPVPTournamentDataMgr* pMgr = GetTournamentDataMgr();
					if (pMgr != NULL)
					{
						const PvPTournamentUIDef::SMatchUserInfo* pInfo = pMgr->GetSlotInfoBySessionID(uiSessionID);
						if (pInfo)
							teamA = (pInfo->commonIndex % 2 == 0);
					}

					pRespawnArea = (teamA) ? vRespawnArea[0] : vRespawnArea[1];
				}
			}
			else
			{
				_srand( uiSessionID );
				int RandomValue = _rand()%(int)vRespawnArea.size();
				pRespawnArea = vRespawnArea[RandomValue];
			}
#else
			_srand( uiSessionID );
			int RandomValue = _rand()%(int)vRespawnArea.size();
			CEtWorldEventArea* pRespawnArea = vRespawnArea[RandomValue];
#endif

			if( m_cGameMode == PvPCommon::GameMode::PvP_IndividualRespawn ||
				m_cGameMode == PvPCommon::GameMode::PvP_Racing)
			{
				CDnMutatorRespawnMode* pRespawnMode = dynamic_cast<CDnMutatorRespawnMode*>(m_pMutatorClass);
				if( pRespawnMode == NULL || pRespawnMode->IsIndividualMode() == false )
				{
					_ASSERT(0);
				}
				else
				{
					if( pRespawnMode->bIsUsedRespawnArea( pRespawnArea->GetName() ) == true )
					{
						for( UINT i=0 ; i<vRespawnArea.size() ; ++i )
						{
							if( pRespawnMode->bIsUsedRespawnArea( vRespawnArea[i]->GetName() ) == false )
							{
								pRespawnArea = vRespawnArea[i];
								break;
							}
						}
					}
					pRespawnMode->PushUsedRespawnArea( pRespawnArea->GetName() );
				}
			}

			EtVector3 vRandPos = GetGenerationRandomPosition( pRespawnArea->GetOBB() );


			vStartPos = vRandPos;
			vStartPos.y = CDnWorld::GetInstance().GetHeight( vStartPos.x, vStartPos.z ) + pRespawnArea->GetMin()->y;
			fRotate		= pRespawnArea->GetRotate();
		}
	}
}


bool CDnPvPGameTask::OnInitializeBreakIntoActor( CDnPartyTask::PartyStruct* pStruct, const int iVectorIndex )
{
	_srand( m_nStageRandomSeed );
	DnActorHandle hActor = InitializePlayerActor( pStruct ,iVectorIndex, false, true );

	if( !hActor )
		return false;

	hActor->Show( false );
	hActor->SetProcess( !pStruct->bGMTrace );

	if( m_pMutatorClass )
	{
		m_pMutatorClass->ProcessEnterUser( hActor, true, pStruct );
		m_pMutatorClass->OnInitializeBreakIntoActor( hActor );
	}

	return true;
}

//blondy
void CDnPvPGameTask::OnRecvFinishPvPMode(  void * pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvFinishPvPMode");

	if( m_pMutatorClass )
		m_pMutatorClass->EndGame( pData );

	CDnLocalPlayerActor::StopAllPartyPlayer();
	
}

void CDnPvPGameTask::OnRecvFinishPvPRound( void* pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvFinishPvPRound");

	//SCPVP_FINISH_PVPROUND* pPacket = static_cast<SCPVP_FINISH_PVPROUND*>(pData);

	if( m_pMutatorClass )
	{
		m_pMutatorClass->RoundEnd( pData );		
	}

	// DropItem 제거
	std::vector<CDnDropItem*> vDropItem;
	vDropItem.reserve( CDnDropItem::s_pVecProcessList.size() );
	vDropItem = CDnDropItem::s_pVecProcessList;
	SAFE_DELETE_PVEC( vDropItem );

	for( UINT i=0 ; i<CDnDropItem::s_pVecProcessList.size() ; ++i )
		CDnDropItem::s_pVecProcessList[i]->SetDestroy();

	
	if( CDnPartyTask::IsActive() ) 
	{
		for( UINT i=0 ; i<GetPartyTask().GetPartyCount() ; ++i) 
		{
			CDnPartyTask::PartyStruct* pStruct = GetPartyTask().GetPartyData(i);
			if( pStruct->hActor )
			{
				pStruct->hActor->RemoveAllBlowExpectPassiveSkill();
			}
		}
	}	
}


void CDnPvPGameTask::OnRecvStartPVPRound( void* pData )
{
	FUNC_LOG();

	DebugLog( "CDnPVPGameTask::OnRecvStartPVPRound" );

	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask )
	{
		_ASSERT( !"CDnPvPGameTask::OnRecvStartPVPRound() -> Can't find party task" );
		return;
	}

	if (m_pMutatorClass == NULL || m_pMutatorClass->GetScoreSystem() == NULL)
	{
		_ASSERT(0&&"CDnPvPGameTask::OnRecvStartPVPRound() -> Can't find MutatorClass or ScoreSystem");
		return;
	}

	m_pMutatorClass->GetScoreSystem()->OnStartRound();

	DWORD dwNumPartyMember = pPartyTask->GetPartyCount();
	for( DWORD dwMember = 0; dwMember < dwNumPartyMember; ++dwMember )
	{
		CDnPartyTask::PartyStruct* pMemberData = pPartyTask->GetPartyData( dwMember );

		if( pMemberData && pMemberData->hActor )
			pMemberData->hActor->OnInitializePVPRoundRestart();
	}

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if(hCamera)
	{
		CDnPlayerCamera *pPlayerCamera = dynamic_cast<CDnPlayerCamera*>(hCamera.GetPointer());
		if(pPlayerCamera)
		{
			if(CDnActor::s_hLocalActor)
			{
				if( pPlayerCamera->GetAttachActor() != CDnActor::s_hLocalActor ) {
					pPlayerCamera->AttachActor( CDnActor::s_hLocalActor );
					pPlayerCamera->ResetCamera();
				}
			}
		}
	}

	CDnLocalPlayerActor::LockInput( true );	

	// 프로젝타일 없앤다
	CDnWeapon::ReleaseClass( CDnWeapon::Projectile );
}


void CDnPvPGameTask::OnRecvModeStartTick( SCPVP_MODE_STARTTICK * pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvModeStartTick");

	//흑백 처리
	CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
	if( pWorldEnv )
		pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, 0.0f, 3000000 );

	
	if( CDnActor::s_hLocalActor )
	{
		CDnActor::s_hLocalActor->ResetActor();
		if( IsLadderMode() || GetGameMode() == PvPCommon::GameMode::PvP_AllKill
#ifdef PRE_ADD_PVP_TOURNAMENT
			|| GetGameMode() == PvPCommon::GameMode::PvP_Tournament
#endif
			)
			CDnActor::s_hLocalActor->ResetLadderSkillCoolTime();
		else
			CDnActor::s_hLocalActor->ResetPvPSkillCoolTime();

#ifdef PRE_ADD_PVP_COMBOEXERCISE
		// 콤보연습모드 - 스킬쿨타임초기화.
		// 위의 디파인이 엉켜있어서 한번더 초기화 하도록했습니다.
		if( GetGameMode() == PvPCommon::GameMode::PvP_ComboExercise )
			CDnActor::s_hLocalActor->ResetSkillCoolTime();
#endif // PRE_ADD_PVP_COMBOEXERCISE


		CDnLocalPlayerActor::LockInput( false );	
	}

	if( pData->uiStartTick > pData -> uiCurTick )//카운트 다운 중에 들어 왔다
	{
		UINT uiTimeGap = pData->uiStartTick - pData -> uiCurTick;
		if( m_pMutatorClass )
			m_pMutatorClass->GameStart(nObjectiveUIString , (float)uiTimeGap/1000 , 0 , false , (float)CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nTimeSec );
		
		return;		

	}

	if( pData->uiStartTick <= pData -> uiCurTick )//중간 난입
	{
		m_isInGameJoin = true;

		if( pWorldEnv )
			pWorldEnv->ResetSaturation();

		UINT uiTimeGap;
		uiTimeGap =  pData -> uiCurTick - pData->uiStartTick;

		if( m_pMutatorClass )
			m_pMutatorClass->GameStart(nObjectiveUIString , 0 , (float)uiTimeGap/1000 , true , (float)CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nTimeSec );	

		return;
	}

}

void CDnPvPGameTask::OnRecvModeScrore( SCPVP_MODE_SCORE * pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvModeScrore");

	if( !m_pMutatorClass )
	{
		_DANGER_POINT();
		return;
	}
	
	IScoreSystem* pScoreSystem = m_pMutatorClass->GetScoreSystem();
	if( !pScoreSystem )
	{
		_DANGER_POINT();
		return;
	}

	pScoreSystem->ClearAllScore();
	pScoreSystem->SetGameModeScore( pData->unATeamScore, pData->unBTeamScore );

	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask )
	{
		_DANGER_POINT();
		return;
	}

	char* pBuffer = reinterpret_cast<char*>(pData->sPvPUserScore);

	for( UINT i=0 ; i<pData->cScoreCount ; ++i )
	{
		TPvPUserScore* pUserScore = reinterpret_cast<TPvPUserScore*>(pBuffer);
		pBuffer += sizeof(TPvPUserScore);

		CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyDataFromSessionID( pUserScore->uiSessionID );
		if( !pPartyStruct )
		{
			_DANGER_POINT();
			continue;
		}

		if( !pScoreSystem->AddUserScore( pPartyStruct->hActor, pUserScore ) )
			continue;
	}

	for( UINT i=0 ; i<pData->cCount ; ++i )
	{
		TPvPUserCount* pUserCount = reinterpret_cast<TPvPUserCount*>(pBuffer);
		pBuffer += sizeof(TPvPUserCount);

		CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyDataFromSessionID( pUserCount->uiSessionID );
		if( !pPartyStruct )
		{
			_DANGER_POINT();
			continue;
		}

		if( !pScoreSystem->SetCount( pPartyStruct->hActor, pUserCount ) )
			continue;
	}
}

void CDnPvPGameTask::OnRecvCaptainKillCount( SCPVP_CAPTAINKILL_COUNT * pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvCaptainKillCount");

	if( !m_pMutatorClass )
	{
		_DANGER_POINT();
		return;
	}

	IScoreSystem* pScoreSystem = m_pMutatorClass->GetScoreSystem();
	if( !pScoreSystem )
	{
		_DANGER_POINT();
		return;
	}

	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask )
	{
		_DANGER_POINT();
		return;
	}

	char* pBuffer = reinterpret_cast<char*>(pData->sCaptainKillCount);

	for( UINT i=0 ; i<pData->cCount ; ++i )
	{
		TPvPCaptainKillCount* pData = reinterpret_cast<TPvPCaptainKillCount*>(pBuffer);
		pBuffer += sizeof(TPvPCaptainKillCount);

		CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyDataFromSessionID( pData->uiSessionID );
		if( !pPartyStruct )
		{
			_DANGER_POINT();
			continue;
		}

		if( !pScoreSystem->SetCaptainKillCount( pPartyStruct->hActor, pData ) )
			continue;
	}
}

void CDnPvPGameTask::OnRecvPvPAddPoint( char* pData )
{
	FUNC_LOG();

	DebugLog("CDnPvPGameTask::OnRecvModeScrore");

	if( !m_pMutatorClass )
	{
		_DANGER_POINT();
		return;
	}

	IScoreSystem* pScoreSystem = m_pMutatorClass->GetScoreSystem();
	if( !pScoreSystem || !pScoreSystem->IsPvPScoreSystem() )
	{
		_DANGER_POINT();
		return;
	}

	CPvPScoreSystem* pPvPScoreSystem = static_cast<CPvPScoreSystem*>(pScoreSystem);

	SCPVP_ADDPOINT* pPacket = reinterpret_cast<SCPVP_ADDPOINT*>(pData);

	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask )
	{
		_DANGER_POINT();
		return;
	}
	
	CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyDataFromSessionID( pPacket->uiSessionID );
	if( !pPartyStruct )
	{
		_DANGER_POINT();
		return;
	}

	CPvPScoreSystem::SMyScore* pScore = pPvPScoreSystem->FindMyScoreDataIfnotInsert( pPartyStruct->hActor );
	if( !pScore )
	{
		_DANGER_POINT();
		return;
	}

	switch( pPacket->uiScoreType )
	{
		case IScoreSystem::ScoreType::AssistScore:
		{
			pScore->uiAssistScore += pPacket->uiAddPoint;
			break;
		}
	}

	pPvPScoreSystem->OnNotify( pPartyStruct->hActor, pPacket->uiScoreType, pPacket->uiAddPoint );
}

void CDnPvPGameTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnGameTask::Process( LocalTime, fDelta );

	if( m_pMutatorClass )
		m_pMutatorClass->Process( LocalTime, fDelta );

#ifndef _WORK // 개발환경에서 강퇴 하지 않음
	ProcessAbsenceKick( LocalTime , fDelta );
#endif

}

void CDnPvPGameTask::ProcessAbsenceKick( LOCAL_TIME LocalTime, float fDelta )
{
	// 올킬전은 자리비움:강퇴 방지.
	if( GetGameMode() == PvPCommon::GameMode::PvP_AllKill )
		return;
#ifdef PRE_ADD_PVP_TOURNAMENT
	if (GetGameMode() == PvPCommon::GameMode::PvP_Tournament)
		return;
#endif

	if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->IsPlayerActor())
	{
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if(pPlayer && ( pPlayer->IsObserver() || pPlayer->GetTeam() == PvPCommon::Team::Observer || pPlayer->IsDeveloperAccountLevel() ) ) 
			return;
	}

	LOCAL_TIME lTime = GetEventMouseTime( IR_MOUSE_MOVE );
	DWORD dwKey = GetEventKeyCount();

	if( m_lKickTime == lTime )
	{
		m_fKickElapsedTime+= fDelta;
	}
	else
	{
		m_nWaringNum = 0;
		m_fKickElapsedTime =0.0f;
	}

	if( m_dwKey != dwKey )
	{
		m_nWaringNum = 0;
		m_fKickElapsedTime =0.0f;
	}

	m_dwKey = dwKey;
	m_lKickTime = lTime;

	if( (m_nWaringNum == 0) && m_fKickElapsedTime > (fInGameKickTime-30.0f)   )
	{
		WCHAR wsWarnningMSG[100];
		SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));
		wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),30);
		DebugLog("30s");
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
		m_nWaringNum = 1;

	}

	if( (m_nWaringNum == 1) && m_fKickElapsedTime > (fInGameKickTime - 10.0f)  )
	{
		WCHAR wsWarnningMSG[100];
		SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));
		wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),10);
		DebugLog("10s");
		GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
		m_nWaringNum = 2;
	}

	if( (m_nWaringNum == 2) && m_fKickElapsedTime > fInGameKickTime  )
	{		
		CGlobalInfo::GetInstance().SetGlobalMessageCode( 1 );
		SendMovePvPGameToPvPLobby();
		DebugLog("0s");
		m_nWaringNum = 0;
		m_fKickElapsedTime =0.0f;
	}
}

void CDnPvPGameTask::ProcessExitUser( DnActorHandle hActor )
{
	if( m_pMutatorClass )
		m_pMutatorClass->OnLeaveUser( hActor );
}

bool CDnPvPGameTask::InitializeAllPlayerActor()
{
	bool returnVal = false;
	returnVal = CDnGameTask::InitializeAllPlayerActor();

	if( m_pMutatorClass && CDnActor::s_hLocalActor )
		m_pMutatorClass->ProcessEnterUser( CDnActor::s_hLocalActor );

	ScopeLock<CSyncLock> Lock( m_ConnectListLock );
	for( DWORD i=0; i<m_VecConnectList.size(); i++ ) {
		if( m_pMutatorClass )
			m_pMutatorClass->ProcessEnterUser( m_VecConnectList[i].hActor , m_isInGameJoin );		
	}

	return returnVal;

}

void CDnPvPGameTask::OnRecvRespawnPoint ( SCPVP_RESPAWN_POINT * pData )
{
#ifndef _FINAL_BUILD

	for(UINT i=0;i < CGlobalValue::GetInstance().m_vRespawnInfo.size();i++ )
	{
		if( CGlobalValue::GetInstance().m_vRespawnInfo[i].Position == pData->Position  )
		{
			CGlobalValue::GetInstance().m_vRespawnInfo[i].iBasePreferPoint = pData->iBasePreferPoint;
			CGlobalValue::GetInstance().m_vRespawnInfo[i].iPoint = pData->iPoint;
			CGlobalValue::GetInstance().m_vRespawnInfo[i].unTeam = pData->unTeam;
			return;
		}
	}

	char				szName[32] = "";

	sprintf(szName,"P %d",CGlobalValue::GetInstance().m_vRespawnInfo.size() + 1);

	_strcpy(pData->szName, _countof(pData->szName), szName, (int)strlen(szName));
	
	CGlobalValue::GetInstance().m_vRespawnInfo.push_back( *pData );
#endif
}

void CDnPvPGameTask::OnRecvPartyReadyGate( SCGateInfo *pPacket )
{
	CDnGameTask::OnRecvPartyReadyGate( pPacket );	
}

void CDnPvPGameTask::OnRecvBreakIntoSuccess( SCPVP_SUCCESSBREAKINTO *pData )
{
	CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyDataFromSessionID( pData->uiSessionID );
	if( pStruct ) 
	{
		pStruct->bCompleteBreakInto = true;
		if( pStruct->hActor ) {
			pStruct->hActor->Show( !pStruct->bGMTrace );
			pStruct->hActor->SetProcess( !pStruct->bGMTrace );
		}
	}
}


void CDnPvPGameTask::OnRecvPvPSelectCaptain(SCPVP_SELECTCAPTAIN *pData)
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID(pData->uiSessionID);
	if(hActor && hActor->IsPlayerActor() && hActor->GetTeam() != PvPCommon::Team::Observer ) 
	{
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(hActor.GetPointer());
		pActor->ToggleCaptainMode(true);

		if(pActor->GetTeam() == CDnActor::s_hLocalActor->GetTeam())
		{
			WCHAR wszCaptainMsg[MAX_PATH];
			WCHAR wszDefendMsg[MAX_PATH];

			wsprintf(wszCaptainMsg,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120054),pActor->GetName());
			wsprintf(wszDefendMsg,L"\n %s",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120055));
			wcscat(wszCaptainMsg,wszDefendMsg);

			GetInterface().GetCaptionDlg_04()->SetCaption(wszCaptainMsg,textcolor::ORANGERED,10.f,GetInterface().GetCaptionDlg_04()->FirstCaption);
			// 요기서 캡션을 설정해주어야 대장이 바뀔때도 대장이 누군지 등등 메세지가 뜨겠습니다.

			if( GetGameMode() == PvPCommon::GameMode::PvP_Captain )
			{
				CDnMutatorCaptainMode *pCaptainMode = static_cast<CDnMutatorCaptainMode *>(m_pMutatorClass);
				if( pCaptainMode )
					pCaptainMode->OnSelectFriendlyCaptain( hActor );
			}
		}
	}
}

void CDnPvPGameTask::OnRecvPvPSelectZombie(SCPVP_SELECTZOMBIE *pData)
{
	for(int i=0; i<pData->cCount ;i ++)
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID(pData->Zombies[i].uiSessionID);
		if(hActor && hActor->IsPlayerActor())
		{
			CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(hActor.GetPointer());
			if(pActor && pActor->IsObserver() == false )
			{
				pActor->ToggleTransformMode( pData->Zombies[i].bZombie , pData->Zombies[i].nMonsterMutationTableID , true ); // 좀비로 만들어줍니다.
				pActor->SetTeam(PvPCommon::Team::Zombie);
				float fScale = pData->Zombies[i].iScale/100.f;
				pActor->SetScale( EtVector3( fScale,fScale,fScale ) );
				if( pData->Zombies[i].bRemoveStateBlow == true )
					hActor->RemoveAllBlowExpectPassiveSkill();
			}
		}
	}
}

void CDnPvPGameTask::OnRecvPVPAddGhoulScore(SCPVP_HOLYWATER_KILLCOUNT *pData)
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pData->nSessionID );
	DnActorHandle hHitter = CDnActor::FindActorFromUniqueID( pData->nKillerSessionID );

	if( hActor && hHitter )
	{
		OnDie( hActor , hHitter );
	}
}

void CDnPvPGameTask::OnRecvPvPFatigueReward( SCPVP_FATIGUE_REWARD *pData )
{
	// 시스템 메세지 창에 얻은거 표시 해주고 피로도 옵션 끄자.
	if( pData->nGainExp	)
	{
		std::wstring str = FormatW(L"[%s] ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126182 )); // UISTRING : 콜로세움 피로도 소모
		str += FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126191 ), pData->nGainExp);	// UISTRING : 경험치 %d 획득		
		
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", str.c_str(), false );
	}
	if( pData->nGainMedal )
	{
		std::wstring str = FormatW(L"[%s] ", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126182 )); // UISTRING : 콜로세움 피로도 소모

		str += FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121069 ), pData->nGainMedal);	// UISTRING : 메달 %d개 획득
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", str.c_str(), false );		
	}	
}

void CDnPvPGameTask::OnRecvPvPTryAcquirePoint( SCPvPTryAcquirePoint * pData )
{
	if( pData->nRetCode == ERROR_NONE )
	{
		if( CDnOccupationTask::IsActive() )
			GetOccupationTask().TryAcquirePoint( pData->nAreaID );
	}
}

void CDnPvPGameTask::OnRecvPvPOccupationPointState( SCPvPOccupationState * pData )
{
	if( CDnOccupationTask::IsActive() )
		GetOccupationTask().OccupationPoint( pData );
}

void CDnPvPGameTask::OnRecvPvPOccupationTeamState( SCPvPOccupationTeamState * pData)
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_GuildWar )
	{
		if( pData->nTeam == CDnActor::s_hLocalActor->GetTeam() )
			(dynamic_cast<CDnGuildWarModeHUD*>(GetInterface().GetHUD()))->SetResourcePoint( pData->nResource );
	}
	if( CDnOccupationTask::IsActive() )
		GetOccupationTask().SetResource( pData );
	if( m_cGameMode == PvPCommon::GameMode::PvP_Occupation )
	{
		CDnMutatorOccupationMode * pMutator = dynamic_cast<CDnMutatorOccupationMode *>(m_pMutatorClass);
		pMutator->CheckResource();
	}
}

void CDnPvPGameTask::OnRecvOccupationScore( TPvPOccupationScore * pData )
{
	FUNC_LOG();
	DebugLog("CDnPvPGameTask::OnRecvOccupationScore");

	if( !m_pMutatorClass )
	{
		_DANGER_POINT();
		return;
	}

	IScoreSystem* pScoreSystem = m_pMutatorClass->GetScoreSystem();
	if( !pScoreSystem )
	{
		_DANGER_POINT();
		return;
	}

	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
	if( !pPartyTask )
	{
		_DANGER_POINT();
		return;
	}

	for( int itr = 0; itr < pData->cCount; ++itr )
	{
		CDnPartyTask::PartyStruct* pPartyStruct = pPartyTask->GetPartyDataFromSessionID( pData->Info[itr].nSessionID );

		if( !pPartyStruct )
			continue;	//망

		if (!pPartyStruct->hActor)
			continue;

		for( int jtr = 0; jtr < pData->Info[itr].nAcquireCount; ++jtr )
		{
			pScoreSystem->OnAcquirePoint( pPartyStruct->hActor->GetTeam(), pPartyStruct->hActor->GetName(), false, true );
		}

		for( int jtr = 0; jtr < pData->Info[itr].nStealAquireCount; ++jtr )
			pScoreSystem->OnAcquirePoint( pPartyStruct->hActor->GetTeam(), pPartyStruct->hActor->GetName(), true, true );
	}

	if( PvPCommon::GameMode::PvP_GuildWar == m_cGameMode )
	{
		for( int itr = 0; itr < pData->TeamScoreInfo[PvPCommon::TeamIndex::A].nTeamKillScore; ++itr )
			pScoreSystem->OnOccupationTeamScore( PvPCommon::Team::A, PvPCommon::OccupationScoreType::Kill );

		for( int itr = 0; itr < pData->TeamScoreInfo[PvPCommon::TeamIndex::A].nTeamAcquireScore; ++itr )
			pScoreSystem->OnOccupationTeamScore( PvPCommon::Team::A, PvPCommon::OccupationScoreType::Acquire );

		for( int itr = 0; itr < pData->TeamScoreInfo[PvPCommon::TeamIndex::B].nTeamKillScore; ++itr )
			pScoreSystem->OnOccupationTeamScore( PvPCommon::Team::B, PvPCommon::OccupationScoreType::Kill );

		for( int itr = 0; itr < pData->TeamScoreInfo[PvPCommon::TeamIndex::B].nTeamAcquireScore; ++itr )
			pScoreSystem->OnOccupationTeamScore( PvPCommon::Team::B, PvPCommon::OccupationScoreType::Acquire );
	}
}

void CDnPvPGameTask::OnRecvConcentrateOrder( SCConcentrateOrder *pData )
{
	CDnGuildWarZoneMapDlg * pGuildWarZoneMap = GetInterface().GetGuildWarZoneMapDlg();

	if( !pGuildWarZoneMap ) return;

	CDnPartyTask::PartyStruct * pParty = GetPartyTask().GetPartyDataFromSessionID( pData->nSessionID );
	if( !pParty ) return;

	if (!CDnLocalPlayerActor::s_hLocalActor) return;
	CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
	if( !pLocalActor ) return;
	if( pLocalActor->GetTeam() != pParty->usTeam ) return;

	pGuildWarZoneMap->OnMousePointer( pData->vPosition );
}

void CDnPvPGameTask::OnRecvOccupationClimaxMode()
{
	if( m_cGameMode != PvPCommon::GameMode::PvP_Occupation )
		return;

	CDnMutatorOccupationMode * pMutator = dynamic_cast<CDnMutatorOccupationMode *>(m_pMutatorClass);
	pMutator->ClimaxModeStart();
}

//===============================================================================================================================
// ScoreSystem
//===============================================================================================================================

void CDnPvPGameTask::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage )
{
	if( m_pMutatorClass )
		m_pMutatorClass->OnDamage( hActor, hHitter, iDamage );
}

void CDnPvPGameTask::OnDie( DnActorHandle hActor, DnActorHandle hHitter )
{
	float fDieDelta = GetPlayerDieRespawnDelta();
	hActor->SetMaxDieDelta( fDieDelta );
	hActor->SetDieDelta( fDieDelta );
	if( m_pMutatorClass )
		m_pMutatorClass->OnDie( hActor, hHitter );
}

void CDnPvPGameTask::OnCmdAddStateEffect( const CDnSkill::SkillInfo* pSkillInfo )
{
	if( m_pMutatorClass )
		m_pMutatorClass->OnCmdAddStateEffect( pSkillInfo );
}

void CDnPvPGameTask::ShowDebugInfo()
{
#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bEnableRespawnInfo )
	{
		for(UINT i=0;i < CGlobalValue::GetInstance().m_vRespawnInfo.size();i++ )
		{
			char szBuf[256];

			sprintf_s( szBuf,"%s: Base = %d , Point = %d , Team = %d "
				,CGlobalValue::GetInstance().m_vRespawnInfo[i].szName
				,CGlobalValue::GetInstance().m_vRespawnInfo[i].iBasePreferPoint
				,CGlobalValue::GetInstance().m_vRespawnInfo[i].iPoint
				,CGlobalValue::GetInstance().m_vRespawnInfo[i].unTeam);

			EternityEngine::DrawText2D( EtVector2( 0.f, 0.2f + (0.03f * i ) ), szBuf, D3DCOLOR_ARGB(255,255,0,0) );

		}
	}
#endif
}


void CDnPvPGameTask::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	CDnGameTask::OnInputReceive( nReceiverState, LocalTime );

#ifdef PRE_MOD_PVPOBSERVER
	if( (nReceiverState & IR_MOUSE_LB_UP) || (nReceiverState & IR_MOUSE_RB_UP) )
	{
		if( !CDnMouseCursor::GetInstance().IsShowCursor() )
		{
			if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->IsPlayerActor() )
			{
				CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );

				if( pLocalPlayer->IsObserver() || pLocalPlayer->IsSpectatorMode() || pLocalPlayer->IsGhost() )
				{
					RotateObservingCamera( ( ( (nReceiverState & IR_MOUSE_LB_UP) == IR_MOUSE_LB_UP ) ? false : true ), false );
					if( pLocalPlayer->IsObserver() )
					{				
						GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121085),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption); 
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
						if( pLocalPlayer->IsDeveloperAccountLevel() )
						{
							GetInterface().ResetPartyPlayerGaugeAttributes( false , true );
							GetInterface().SetPartyPlayerGaugeAttributes( pLocalPlayer->GetFollowObserverUniqueID(), true, false, true );
						}
#endif
					}
					else
					{
	#ifdef PRE_FIX_PVP_SPECTATOR_MOVE
						if (pLocalPlayer->IsMove())
							pLocalPlayer->CmdStop("Stand");
	#endif

						GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121084),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption); // "마우스 좌,우 버튼을 누르면 다른캐릭터의 시점으로 이동합니다."
					}
				}
			}
		}
	}
#endif // #ifdef PRE_MOD_PVPOBSERVER

	if( nReceiverState & IR_KEY_DOWN ) 
	{
		if( CDnActor::s_hLocalActor )
		{
			CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );

			if( PvPCommon::GameMode::PvP_GuildWar == m_cGameMode && !pLocalPlayer->IsObserver() )
				return;

			if( pLocalPlayer->IsObserver() )
			{
#ifdef PRE_MOD_PVPOBSERVER
				if(IsPushKey(DIK_TAB) )
				{
					RotatePartyPlayerCamera(true);
					GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121084),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption);
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
					if( pLocalPlayer->IsObserver() && pLocalPlayer->IsDeveloperAccountLevel() )
					{
						GetInterface().ResetPartyPlayerGaugeAttributes( false , true );
					}
#endif
				}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
				if( pLocalPlayer->IsDeveloperAccountLevel() && IsPushKey(DIK_X) )
				{
					GetInterface().ShowPartyPlayerGaugeList( !GetInterface().IsShowPartyPlayerGaugeList() );
				}

				if( pLocalPlayer->IsDeveloperAccountLevel() && IsPushKey(DIK_Z) )
				{
					GetInterface().GetMainBarDialog()->Show(!GetInterface().GetMainBarDialog()->IsShow());
					GetInterface().GetChatTabDialog()->GetCurrentChatDlg()->Show(!GetInterface().GetChatTabDialog()->GetCurrentChatDlg()->IsShow());
				}

				if( pLocalPlayer->IsDeveloperAccountLevel() && IsPushKey(DIK_C) )
				{
					CDnAllKillModeHUD *pAllKillModeHUD = dynamic_cast< CDnAllKillModeHUD *> (GetInterface().GetHUD());
					if( pAllKillModeHUD )
					{
						pAllKillModeHUD->GetObserverOrderListDlg()->ForceView( !pAllKillModeHUD->GetObserverOrderListDlg()->IsShow() );
					}
				}

				if( pLocalPlayer->IsDeveloperAccountLevel() && IsPushKey(DIK_LSHIFT) )
				{
					const int IMPROVED_MOVESPEED = 800;
					if( CDnActor::s_hLocalActor )
					{
						if( CDnActor::s_hLocalActor->GetMoveSpeed() >= IMPROVED_MOVESPEED ) 
						{
							CDnActor::s_hLocalActor->RefreshState( CDnActorState::RefreshAll , CDnActorState::ST_MoveSpeed );
						}
						else
						{
							CDnActor::s_hLocalActor->SetMoveSpeed( IMPROVED_MOVESPEED );
						}
					}
				}
#endif

#else
				if( IsPushKey(DIK_SPACE) )
				{
					RotatePartyPlayerCamera();
					GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121085),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption); 
				}
				else if(IsPushKey(DIK_TAB) )
				{
					RotatePartyPlayerCamera(true);
					GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121084),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption);
				}
#endif // PRE_MOD_PVPOBSERVER
				
				return;
			}
			if( pLocalPlayer->IsSpectatorMode() )
			{
				if( IsPushKey(DIK_TAB) )
				{
					RotatePartyPlayerCamera();
	#ifdef PRE_FIX_PVP_SPECTATOR_MOVE
					if (pLocalPlayer->IsMove())
						pLocalPlayer->CmdStop("Stand");
	#endif
					GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 109045 ),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption); 
				}
				return;
			}
#ifndef PRE_MOD_PVPOBSERVER
			if( pLocalPlayer->IsGhost() )
			{
				if( IsPushKey( DIK_SPACE ) )
				{
					RotatePartyPlayerCamera();
	#ifdef PRE_FIX_PVP_SPECTATOR_MOVE
					if (pLocalPlayer->IsMove())
						pLocalPlayer->CmdStop("Stand");
	#endif
					GetInterface().GetCaptionDlg_04()->SetCaption(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121084),textcolor::ORANGERED,2.f,GetInterface().GetCaptionDlg_04()->FirstCaption);
				}
				return;
			}
#endif

		}
	}
}


void CDnPvPGameTask::ResetPlayerCamera()
{
	if(!CDnActor::s_hLocalActor)
		return;
	
	CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor*>((CDnActor*)m_hLocalPlayerActor);
	CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera(CDnCamera::PlayerCamera).GetPointer(); 
	if(pCamera && pCamera->GetCameraType() == CDnCamera::PlayerCamera )
	{
		pCamera->DetachActor();
		pCamera->AttachActor( pPlayer->GetActorHandle() );
	}

	pPlayer->SetFollowObserver(false,0);
}



#ifdef PRE_MOD_PVPOBSERVER
void CDnPvPGameTask::RotateObservingCamera( bool bNext, bool bFreeView )
{
	CDnPartyTask *pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( !pPartyTask ) return;
	if( (( pPartyTask->GetPartyCount() - pPartyTask->GetObserverCount() ) <= 0) ) return;
	if( !CDnActor::s_hLocalActor ) return;
	if( !m_hPlayerCamera ) return;

	CDnPlayerCamera *pPlayerCamera = static_cast<CDnPlayerCamera*>(m_hPlayerCamera.GetPointer());
	CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );


	// 관전타겟플레이어 검출.
	CDnPlayerActor * pObservingTarget = NULL;
	int nPartSize = (int)pPartyTask->GetPartyCount();
	int targetIndex = m_nCrrObservingPlayerIndex;

	int nSign = 1;
	if( !bNext )
		nSign = -1;	

	for( int i=0; i<nPartSize; ++i )
	{
		targetIndex += nSign;

		if( targetIndex < 0 )
			targetIndex = nPartSize - 1;
		else if( targetIndex >= nPartSize )
			targetIndex = 0;
	
		if( GetObservingTargetActor( pPartyTask, targetIndex, &pObservingTarget ) )
		{
			if( pObservingTarget && pObservingTarget->GetActorHandle() != pPlayerCamera->GetAttachActor() )
				break;
		}
	}

	// 카메라설정.
	if( !bFreeView )
	{
		if( pObservingTarget && pObservingTarget->GetActorHandle() != pPlayerCamera->GetAttachActor() ) 
		{
			bool bUseObservation = false;

			pPlayerCamera->DetachActor();
			pPlayerCamera->AttachActor( pObservingTarget->GetActorHandle() );

			if( pLocalPlayer->IsObserver() ) bUseObservation = true;
			if( pLocalPlayer->IsSpectatorMode() ) bUseObservation = true; 
			if( bUseObservation )
			{
				if(pObservingTarget->GetActorHandle() == pLocalPlayer->GetActorHandle() )
				{
					pLocalPlayer->SetFollowObserver( false, 0 );
				}
				else
				{
					pLocalPlayer->SetFollowObserver( true, pObservingTarget->GetUniqueID() );
				}

				GetInterface().UpdateTargetPortrait( pObservingTarget->GetActorHandle() ); // 붙을때 바로 갱신해주도록 하자

				m_nCrrObservingPlayerIndex = targetIndex;
			}
		}
	}
	else
	{
		if( pPlayerCamera->GetAttachActor() )
		{
			pLocalPlayer->SetPosition(*pPlayerCamera->GetAttachActor()->GetPosition());
			pLocalPlayer->GetMatEx()->CopyRotationFromThis(pPlayerCamera->GetAttachActor()->GetMatEx());
		}

		pPlayerCamera->DetachActor();
		pPlayerCamera->AttachActor(pLocalPlayer->GetActorHandle());

		if(pLocalPlayer->IsFollowObserver())
		{
			pLocalPlayer->SetFollowObserver( false, 0 );
		}
	}
}
#endif // PRE_MOD_PVPOBSERVER

void CDnPvPGameTask::RotatePartyPlayerCamera(bool bFreeView)
{
	CDnPartyTask *pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if( !pPartyTask ) return;
	if( (( pPartyTask->GetPartyCount() - pPartyTask->GetObserverCount() ) <= 0) ) return;
	if( !CDnActor::s_hLocalActor ) return;
	if( !m_hPlayerCamera ) return;

	CDnPlayerCamera *pPlayerCamera = static_cast<CDnPlayerCamera*>(m_hPlayerCamera.GetPointer());
	CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>((CDnActor*)m_hLocalPlayerActor);

	bool bFindActor = false;
	bool bFindNextActor = false;

	for(int i=0; i < (int)pPartyTask->GetPartyCount(); i++ )
	{
		if( i < m_nNextObservingPartyPlayerIndex )
			continue;

		CDnPartyTask::PartyStruct *pPartyStruct(NULL);
		pPartyStruct = pPartyTask->GetPartyData(i);
		if( !pPartyStruct ) 
			continue;

		if( !pPartyStruct->hActor || !pPartyStruct->hActor->IsPlayerActor() || pPartyStruct->usTeam == PvPCommon::Team::Observer ) 
			continue;

		CDnPlayerActor *pTargetActor = static_cast<CDnPlayerActor*>(pPartyStruct->hActor.GetPointer());
		if( !pTargetActor->IsLocalActor() && ( pTargetActor->IsDie() || pTargetActor->IsSpectatorMode() ) )
			continue; 
		if( !bFreeView && pTargetActor->IsLocalActor() && !pTargetActor->IsSpectatorMode() && pTargetActor->IsDie() )
			continue;

		if(!pLocalPlayer->IsObserver())
		{
			if( GetGameMode() != PvPCommon::GameMode::PvP_IndividualRespawn 
				&& GetGameMode() != PvPCommon::GameMode::PvP_Zombie_Survival )
			{
				if( pLocalPlayer->GetTeam() != pTargetActor->GetTeam() ) 
					continue;
			}
		}
		
		if( bFindActor )
		{
			m_nNextObservingPartyPlayerIndex = i;
			bFindNextActor = true;
			break;
		}

		if( !bFreeView)
		{
			if( pTargetActor->GetActorHandle() != pPlayerCamera->GetAttachActor() ) 
			{
				bool bUseObservation = false;

				pPlayerCamera->DetachActor();
				pPlayerCamera->AttachActor( pTargetActor->GetActorHandle() );
				
				if( pLocalPlayer->IsObserver() ) bUseObservation = true;
				if( pLocalPlayer->IsSpectatorMode() ) bUseObservation = true; 
				if( bUseObservation )
				{
					if(pTargetActor->GetActorHandle() == pLocalPlayer->GetActorHandle() )
						pLocalPlayer->SetFollowObserver(false,0);
					else
						pLocalPlayer->SetFollowObserver( true,pTargetActor->GetUniqueID() );
					GetInterface().UpdateTargetPortrait( pTargetActor->GetActorHandle() ); // 붙을때 바로 갱신해주도록 하자
				}
			}
		}
		else
		{
			if( pPlayerCamera->GetAttachActor() )
			{
				pLocalPlayer->SetPosition(*pPlayerCamera->GetAttachActor()->GetPosition());
				pLocalPlayer->GetMatEx()->CopyRotationFromThis(pPlayerCamera->GetAttachActor()->GetMatEx());
			}
			
			pPlayerCamera->DetachActor();
			pPlayerCamera->AttachActor(pLocalPlayer->GetActorHandle());

			if(pLocalPlayer->IsFollowObserver())
				pLocalPlayer->SetFollowObserver(false,0);
		}

		bFindActor = true;
	}

	if(!bFindNextActor)
		m_nNextObservingPartyPlayerIndex = 0;
}


bool CDnPvPGameTask::OnPostInitializeStage()
{
	char szLabel[64]= "";

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEMDROP );
	if( pSox->IsExistItem( m_iKOItemDropTableID ) )
	{
		for( DWORD i=0; i<20; i++ ) 
		{		
			sprintf_s( szLabel, "_Item%dIndex", i + 1 );
			int nIndex = pSox->GetFieldFromLablePtr( m_iKOItemDropTableID, szLabel )->GetInteger();
			if( nIndex < 1 ) continue;

			CDnDropItem::PreInitializeItem( nIndex  ); 
		}		
	}

	//에어리어에 있는것 프리로드

	for( DWORD i=0; i<CDnWorld::GetInstance().GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		CEtWorldSector*			pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
		CEtWorldEventControl*	pControl = pSector->GetControlFromUniqueID( ETE_PvPRespawnItemArea );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			CEtWorldEventArea* pArea = pControl->GetAreaFromIndex(j);
			if( pArea )
			{
				PvPRespawnItemAreaStruct * PvPRespawnItemArea = (PvPRespawnItemAreaStruct*)pArea->GetData();

				if( pSox->IsExistItem( PvPRespawnItemArea->nItemDropTableID ) )
				{
					for( DWORD i=0; i<20; i++ ) 
					{		
						sprintf_s( szLabel, "_Item%dIndex", i + 1 );
						int nIndex = pSox->GetFieldFromLablePtr( PvPRespawnItemArea->nItemDropTableID, szLabel )->GetInteger();
						if( nIndex < 1 ) continue;
						CDnDropItem::PreInitializeItem( nIndex  ); 
					}
				}
			}
		}
	}

	if (m_pMutatorClass)
		m_pMutatorClass->OnPostInitializeStage();
	return true;
}

DnActorHandle CDnPvPGameTask::InitializePlayerActor( CDnPartyTask::PartyStruct *pStruct ,int nPartyIndex , bool bLocalPlayer, bool bIsBreakInto/*=false*/ )
{
	DnActorHandle hActor;
	if( pStruct && pStruct->usTeam != PvPCommon::Team::Observer )
	{
		hActor = CDnGameTask::InitializePlayerActor( pStruct, nPartyIndex, bLocalPlayer, bIsBreakInto );
		if(GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival)
		{
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
			if(pPlayer)
				pPlayer->CmdShootMode(true);// 슛모드는 트리거로 설정해서 난입해서 들어오는경우는 모르고있다 , 구울모드는 100% 슛모드인상태이므로 걸어주자.
		}
	}
	else if(pStruct && pStruct->usTeam == PvPCommon::Team::Observer ) 
	{
		if(pStruct->nSessionID == CDnBridgeTask::GetInstance().GetSessionID()) // 캐릭터만 관전자 설정 / 타유저는 생성치않는다.
		{
			hActor = CDnGameTask::InitializePlayerActor( pStruct, nPartyIndex, bLocalPlayer );
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(hActor.GetPointer());
			if(pPlayer)
				pPlayer->SetObserver(true); // 생성시 옵져버로 설정해둡니다.
		}
	}

	if(hActor && pStruct)
	{
		CDnPlayerActor *pActor = (CDnPlayerActor *)hActor.GetPointer();

		if(pActor)
			pActor->SetPartsColor( MAPartsBody::HairColor, pStruct->dwHairColor );
	}

	if( bLocalPlayer )
	{
		CDnLocalPlayerActor::LockInput( true );
		if(m_cGameMode == PvPCommon::GameMode::PvP_Zombie_Survival)
			CDnLocalPlayerActor::LockItemMove(true);
	}
	return hActor;
}

void CDnPvPGameTask::OnRecvAllKillModeShowSelectPlayer( SCPVP_ALLKILL_SHOW_SELECTPLAYER * pData )
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		if( pData->bSelectPlayer )
		{
			CDnAllKillModeHUD *pAllKillHUD = static_cast<CDnAllKillModeHUD*>(GetInterface().GetHUD());
			pAllKillHUD->ShowOrderListDlg( true );
		}
		else
		{
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121129 ) , textcolor::ORANGERED ,10.0f );
		}

		GetInterface().ShowPVPModeEndDialog( PvPCommon::GameMode::PvP_Round  , false , 0 , false );
	}
}

void CDnPvPGameTask::OnRecvAllKillModeActivePlayer( SCPVP_ALLKILL_ACTIVEPLAYER * pData )
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		CDnAllKillModeHUD *pAllKillHUD = static_cast<CDnAllKillModeHUD*>(GetInterface().GetHUD());
		pAllKillHUD->SelectOrderListPlayer( pData->uiActivePlayerSessionID );
	}
}

void CDnPvPGameTask::OnRecvAllKillModeSelectGroupCaptain( SCPVP_ALLKILL_GROUPCAPTAIN * pData )
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		CDnAllKillModeHUD *pAllKillHUD = static_cast<CDnAllKillModeHUD*>(GetInterface().GetHUD());
		pAllKillHUD->SelectOrderListCaptain( pData->uiGroupCaptainSessionID );
	}
}

void CDnPvPGameTask::OnRecvAllKillContinousWinCount( SCPVP_ALLKILL_CONTINUOUSWIN *pData )
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		CDnMutatorAllKillMode *pMutatorAllKill = static_cast<CDnMutatorAllKillMode*>(m_pMutatorClass);
		pMutatorAllKill->SetContinousWinCount( pData->uiSessionID , pData->uiCount );
	}
}

void CDnPvPGameTask::OnRecvAllKillFinishDetailReason( SCPVP_ALLKILL_FINISHDETAILREASON *pData )
{
	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		CDnMutatorAllKillMode *pMutatorAllKill = static_cast<CDnMutatorAllKillMode*>(m_pMutatorClass);

		switch(pData->Reason)
		{
		case PvPCommon::FinishDetailReason::SuperiorHPWin:
			pMutatorAllKill->SetFinishDetailReason(121130);
			break;
		case PvPCommon::FinishDetailReason::SuperiorContinuousWin:
			pMutatorAllKill->SetFinishDetailReason(121131);
			break;
		case PvPCommon::FinishDetailReason::HPnContinuousWinDraw:
			pMutatorAllKill->SetFinishDetailReason(121132);
			break;
		}
	}
#ifdef PRE_ADD_PVP_TOURNAMENT
	else if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		CDnMutatorPVPTournamentMode* pMutatorTournament = static_cast<CDnMutatorPVPTournamentMode*>(m_pMutatorClass);

		switch(pData->Reason)
		{
		case PvPCommon::FinishDetailReason::SuperiorHPWin:
			pMutatorTournament->SetFinishDetailReason(121130);
			break;
		case PvPCommon::FinishDetailReason::SuperiorContinuousWin:
			pMutatorTournament->SetFinishDetailReason(121131);
			break;
		case PvPCommon::FinishDetailReason::HPnContinuousWinDraw:
			pMutatorTournament->SetFinishDetailReason(121132);
			break;
		}
	}
#endif
}

void CDnPvPGameTask::OnRecvAllKillBattlePlayer( SCPVP_ALLKILL_BATTLEPLAYER *pData )
{
#ifdef PRE_ADD_PVP_TOURNAMENT
	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		OnRecvPVPTournamentBattlePlayer(pData);
		return;
	}
#endif

	if( m_cGameMode == PvPCommon::GameMode::PvP_AllKill )
	{
		WCHAR wszMsg[256] = {0,};

		CDnPartyTask::PartyStruct *pStruct_TeamA = CDnPartyTask::GetInstance().GetPartyDataFromSessionID(pData->uiSessionIDArr[0]);
		CDnPartyTask::PartyStruct *pStruct_TeamB = CDnPartyTask::GetInstance().GetPartyDataFromSessionID(pData->uiSessionIDArr[1]);

#ifdef PRE_MOD_PVPOBSERVER
		m_uiSessionBattlePlayer[0] = m_uiSessionBattlePlayer[1] = 0;
#endif // PRE_MOD_PVPOBSERVER

		if( pStruct_TeamA && pStruct_TeamB && pStruct_TeamA->hActor && pStruct_TeamB->hActor )
		{
#ifdef PRE_MOD_PVPOBSERVER
			m_uiSessionBattlePlayer[0] = pData->uiSessionIDArr[0];
			m_uiSessionBattlePlayer[1] = pData->uiSessionIDArr[1];
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_DUAL_INFO
			CDnAllKillModeHUD *pAllKillHUD = static_cast<CDnAllKillModeHUD*>(GetInterface().GetHUD());

			if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == pStruct_TeamB->hActor->GetTeam() ) // B팀이면 
			{
				pAllKillHUD->SetDualInfo(
					pStruct_TeamB->wszCharacterName,
					pStruct_TeamB->hActor->OnGetJobClassID(),
					pStruct_TeamA->wszCharacterName,
					pStruct_TeamA->hActor->OnGetJobClassID());
			}
			else // 관전자 A팀 등등 해당
			{
				pAllKillHUD->SetDualInfo(
					pStruct_TeamA->wszCharacterName,
					pStruct_TeamA->hActor->OnGetJobClassID(),
					pStruct_TeamB->wszCharacterName,
					pStruct_TeamB->hActor->OnGetJobClassID());
			}
#else
			wsprintf( wszMsg , L"%s VS %s" , pStruct_TeamA->wszCharacterName , pStruct_TeamB->wszCharacterName );
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  wszMsg , textcolor::YELLOW ,5.0f );
#endif
		}
	}
}

#ifdef PRE_ADD_PVP_TOURNAMENT
void CDnPvPGameTask::OnRecvPVPTournamentBattlePlayer(SCPVP_ALLKILL_BATTLEPLAYER *pData)
{
	std::wstring msg;
	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
#ifdef PRE_MOD_PVPOBSERVER
		m_uiSessionBattlePlayer[0] = pData->uiSessionIDArr[0];
		m_uiSessionBattlePlayer[1] = pData->uiSessionIDArr[1];
#endif // PRE_MOD_PVPOBSERVER

		CDnPVPTournamentDataMgr* pMgr = GetTournamentDataMgr();
		if (pMgr == NULL)
		{
			_ASSERT(0);
			return;
		}

		std::vector<UINT> sessionIds;
		sessionIds.push_back(pData->uiSessionIDArr[0]);
		sessionIds.push_back(pData->uiSessionIDArr[1]);
		pMgr->SetGameMatchCurrentUser(sessionIds);

		GetInterface().UpdatePVPTournamentGameMatchUserList();

		const SMatchUserInfo* pLeftUserInfo = pMgr->GetSlotInfoBySessionID(pData->uiSessionIDArr[0]);
		const SMatchUserInfo* pRightUserInfo = pMgr->GetSlotInfoBySessionID(pData->uiSessionIDArr[1]);
		if (pLeftUserInfo == NULL || pRightUserInfo == NULL)
		{
			_ASSERT(0);
			return;
		}

#ifdef PRE_ADD_PVP_DUAL_INFO
		CDnPVPTournamentModeHUD* pTournamentHud = static_cast<CDnPVPTournamentModeHUD*>(GetInterface().GetHUD());
		if (pTournamentHud)
		{
			pTournamentHud->SetVersusUserName(pLeftUserInfo->playerName.c_str(), pRightUserInfo->playerName.c_str());
			pTournamentHud->SetDualInfo(
				pLeftUserInfo->playerName.c_str(),
				pLeftUserInfo->nJobID,
				pRightUserInfo->playerName.c_str(),
				pRightUserInfo->nJobID);
		}
#else
		msg = FormatW(L"%s VS %s", pLeftUserInfo->playerName.c_str(), pRightUserInfo->playerName.c_str());
		GetInterface().ShowCaptionDialog(CDnInterface::typeCaption4, msg.c_str(), textcolor::YELLOW, 5.0f);
#endif
	}
}

CDnPVPTournamentDataMgr* CDnPvPGameTask::GetTournamentDataMgr() const
{
	if (CDnBridgeTask::IsActive())
	{
		CDnPVPTournamentDataMgr& dataMgr = CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr();
		return (&dataMgr);
	}

	return NULL;
}

void CDnPvPGameTask::OnRecvPVPTournamentGameMatchList(SCPvPTournamentMatchList* pData)
{
	if (pData == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		CDnPVPTournamentDataMgr* pDataMgr = GetTournamentDataMgr();
		pDataMgr->SetGameMatchUserList(*pData);
	}

	GetInterface().UpdatePVPTournamentGameMatchUserList();
}

void CDnPvPGameTask::OnRecvPVPTournamentDefaultWin(SCPvPTournamentDefaultWin* pData)
{
	if (pData == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		CDnMutatorPVPTournamentMode* pMutatorTournament = dynamic_cast<CDnMutatorPVPTournamentMode*>(m_pMutatorClass);
		if (NULL == pMutatorTournament)
			return;

		pMutatorTournament->OnDefaultWin(*pData);
	}
}

void CDnPvPGameTask::OnRecvPVPTournamentIdleTick(SCPvPTournamentIdleTick* pData)
{
	if (pData == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		GetInterface().ShowPVPModeEndDialog(PvPCommon::GameMode::PvP_Tournament, false, 0, false);

		CDnPVPTournamentDataMgr* pDataMgr = GetTournamentDataMgr();

		if (pData->uiIdleTick > 0)
		{
			GetInterface().BeginCountDown(int(pData->uiIdleTick * 0.001f), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120211)); // UISTRING : 다음 라운드를 시작합니다
			return;		
		}
	}
}

void CDnPvPGameTask::OnRecvPVPTournamentTop4(SCPvPTournamentTop4* pData)
{
	if (pData == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_cGameMode == PvPCommon::GameMode::PvP_Tournament)
	{
		CDnPVPTournamentDataMgr* pDataMgr = GetTournamentDataMgr();
		if (pDataMgr == NULL)
		{
			_ASSERT(0);
			return;
		}

		pDataMgr->SetFinalReportUser_WinnersRank(pData->uiSessionID, 4);
	}
}

#endif // PRE_ADD_PVP_TOURNAMENT

#if defined( PRE_ADD_RACING_MODE )
void CDnPvPGameTask::OnRecvRacingRapTime( SCPVP_RACING_RAPTIME * pData )
{
	if( m_cGameMode != PvPCommon::GameMode::PvP_Racing )
		return;

	CDnMutatorRacingMode *pMutatorRacing = dynamic_cast<CDnMutatorRacingMode*>(m_pMutatorClass);
	if( NULL == pMutatorRacing )
		return;

	pMutatorRacing->SetLapTime( pData );
}

void CDnPvPGameTask::OnRecvRacingFirstEnd( SCPVP_RACING_FIRST_END * pData )
{
	if( m_cGameMode != PvPCommon::GameMode::PvP_Racing )
		return;

	CDnMutatorRacingMode *pMutatorRacing = dynamic_cast<CDnMutatorRacingMode*>(m_pMutatorClass);
	if( NULL == pMutatorRacing )
		return;

	bool bLocalPlayerGoal = false;
	if( CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor * pLocalActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
		if( pLocalActor )
		{
			if( pData->uiSessionID == pLocalActor->GetUniqueID() )
				bLocalPlayerGoal = true;
		}
	}

	if( true == bLocalPlayerGoal )
		pMutatorRacing->LocalPlayerGoal();

	CDnPartyTask::PartyStruct * pPartyData = GetPartyTask().GetPartyDataFromSessionID( pData->uiSessionID );
	if( NULL == pPartyData || !pPartyData->hActor )
		return;

	WCHAR wszString[256] = {0,};
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000023354 ), pPartyData->hActor->GetName() );	// UISTRING : %s" 님이 목표지점에 도착하였습니다
	GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3, wszString, textcolor::YELLOW, 5.0f );
}
#endif	// #if defined( PRE_ADD_RACING_MODE )


#ifdef PRE_MOD_PVPOBSERVER
bool CDnPvPGameTask::GetObservingTargetActor( CDnPartyTask *pPartyTask, int tergetIndex, CDnPlayerActor ** pObservingTarget )
{
	CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>((CDnActor*)m_hLocalPlayerActor);

	CDnPartyTask::PartyStruct *pPartyStruct = pPartyTask->GetPartyData( tergetIndex );
	if( !pPartyStruct ) 
		return false;
	
	if( (( pPartyTask->GetPartyCount() - pPartyTask->GetObserverCount() ) <= 0) )
		return false;
	if( !CDnActor::s_hLocalActor || !m_hPlayerCamera )
		return false;
	
	if( !pPartyStruct->hActor || !pPartyStruct->hActor->IsPlayerActor() || pPartyStruct->usTeam == PvPCommon::Team::Observer ) 
		return false;

	CDnPlayerActor *pTargetActor = static_cast<CDnPlayerActor*>(pPartyStruct->hActor.GetPointer());
	if( !pTargetActor->IsLocalActor() && ( pTargetActor->IsDie() || pTargetActor->IsSpectatorMode() ) )
		return false;
	if( pTargetActor->IsLocalActor() && !pTargetActor->IsSpectatorMode() && pTargetActor->IsDie() )
		return false;
	if(!pLocalPlayer->IsObserver())
	{
		if( GetGameMode() != PvPCommon::GameMode::PvP_IndividualRespawn 
			&& GetGameMode() != PvPCommon::GameMode::PvP_Zombie_Survival 
#ifdef PRE_ADD_PVP_TOURNAMENT
			&& GetGameMode() != PvPCommon::GameMode::PvP_Tournament
#endif
			)
		{
			if( pLocalPlayer->GetTeam() != pTargetActor->GetTeam() ) 
				return false;
		}
	}

	// AllKill Mode 의 경우 TargetActor의 SpectatorMode 설정이 제대로 이루어지지 않는 문제가 있다.
	// 따라서 현재 경기중인 유저가 아닌 경우 제외하도록 한다.	
	if( ( pLocalPlayer->IsSpectatorMode() ||  pLocalPlayer->IsObserver() ) && ( GetGameMode() == PvPCommon::GameMode::PvP_AllKill 
#ifdef PRE_ADD_PVP_TOURNAMENT		
		|| GetGameMode() == PvPCommon::GameMode::PvP_Tournament 
#endif		
		) )
	{
		bool bExit = true;
		for( int i=0; i<2; ++i )
		{
			if( m_uiSessionBattlePlayer[ i ] != 0 &&
				m_uiSessionBattlePlayer[ i ] == pPartyStruct->nSessionID )
			{
				bExit = false;
				break;
			}
		}

		if( bExit )
			return false;
	}	

	*pObservingTarget = pTargetActor;
	return true;
}

bool CDnPvPGameTask::IsMatchedBattlePlayer( UINT uiSessionID )
{
	return ( uiSessionID == m_uiSessionBattlePlayer[0] || uiSessionID == m_uiSessionBattlePlayer[1] );
}

#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_WORLDCOMBINE_PVP

void CDnPvPGameTask::OnRecvWorldPVPRoomStartMsg( WorldPvPMissionRoom::SCWorldPvPRoomStartMsg* pPacket )
{
	if( pPacket == NULL )
	{
		_ASSERT(0);
		return;
	}

	GetInterface().SetWorldPVPStartButtonInfo( pPacket->bShow, pPacket->uiPvPIndex );
}

void CDnPvPGameTask::OnRecvWorldPVPRoomStartResult( WorldPvPMissionRoom::SCWorldPvPRoomStartReturn* pPacket )
{
	if( pPacket == NULL )
	{
		_ASSERT(0);
		return;
	}

	if( pPacket->nRet != ERROR_NONE )
	{
		int nErrorMsgId = 0;
/*		switch( pPacket->nRet )
		{
		case ERROR_NOTEXIST_INVITEUSER:
			nErrorMsgId = 7820;
			break;
		case ERROR_ALREADY_JOINCHANNEL:
			nErrorMsgId = 7821;
			break;
		}
*/
		if( nErrorMsgId > 0 )
		{
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
		}
	}
}

void CDnPvPGameTask::OnRecvWorldPVPRoomJoinResult( WorldPvPMissionRoom::SCWorldPvPRoomJoinResult* pPacket )
{
	if( pPacket == NULL )
	{
		_ASSERT(0);
		return;
	}

	if( pPacket->nRet != ERROR_NONE )
	{
		int nErrorMsgId = 0;
/*		switch( pPacket->nRet )
		{
		case ERROR_NOTEXIST_INVITEUSER:
			nErrorMsgId = 7820;
			break;
		case ERROR_ALREADY_JOINCHANNEL:
			nErrorMsgId = 7821;
			break;
		}
*/
		if( nErrorMsgId > 0 )
		{
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, _T(""), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorMsgId ), false );
		}
	}
}

void CDnPvPGameTask::OnRecvWorldPVPRoomAllKillTeamInfo( WorldPvPMissionRoom::SCWorldPvPRoomAllKillTeamInfo* pPacket )
{
	if( pPacket == NULL )
	{
		_ASSERT(0);
		return;
	}

	for( int i=0; i<PARTYMAX; i++ )
	{
		if( pPacket->nSessionID[i] > 0 )
		{
			GetInterface().ChangePVPScoreTeam( pPacket->nSessionID[i], pPacket->nTeam[i] );
		}
	}
}

void CDnPvPGameTask::OnRecvWorldPVPRoomTournamentUserInfo( WorldPvPMissionRoom::SCWorldPvPRoomTournamentUserInfo* pPacket )
{
	if( pPacket == NULL )
	{
		_ASSERT(0);
		return;
	}

	WCHAR wszPlayerName[NAMELENMAX];
	int nPosition=0;

	CDnPVPTournamentDataMgr* pMgr = GetTournamentDataMgr();

	if( !pMgr )
		return;

	for(int i =0 ;i < pPacket->cUserCount ;i++)
	{
		SecureZeroMemory(wszPlayerName,sizeof(wszPlayerName));
		memcpy(wszPlayerName,pPacket->UserInfoList.wszBuffer +nPosition , pPacket->UserInfoList.cCharacterNameLen[i] * sizeof(WCHAR));
		nPosition += pPacket->UserInfoList.cCharacterNameLen[i];

		if(pPacket->UserInfoList.cTeamIndex[i] != PvPCommon::Team::Observer)
		{
			SMatchUserInfo info;
			info.commonIndex = pPacket->UserInfoList.cTeamIndex[i];
			info.nJobID = pPacket->UserInfoList.iJobID[i];
			info.cLevel = pPacket->UserInfoList.cLevel[i];
			info.uiUserState = pPacket->UserInfoList.uiUserState[i];
			info.cPVPLevel = pPacket->UserInfoList.cPvPLevel[i];
			info.playerName = wszPlayerName;
			info.uiSessionID = pPacket->UserInfoList.uiSessionID[i];			
			info.bIsNeedUpdate = true;

			bool bSet = pMgr->SetInfo(info);
			if (bSet == false)
			{
				ErrorLog("CDnPVPGameRoomDlg::InsertPlayer:: Not insert player because no empty slot.");
				return;
			}		
		}
	}	
}

#endif // PRE_WORLDCOMBINE_PVP


#ifdef PRE_ADD_PVP_COMBOEXERCISE

void CDnPvPGameTask::OnRecvPVPComboModeMaster( SCPvPComboExerciseRoomMasterInfo * pData )
{
	// 콤보연습모드 방장여부.
	CTask * pTask = CTaskManager::GetInstance().GetTask("BridgeTask");
	if( pTask )
	{
		m_bPvPComboModeMaster = ( pData->uiRoomMasterSessionID == ((CDnBridgeTask *)pTask)->GetSessionID() );

		CDnPVPBaseHUD * pHud = GetInterface().GetHUD();
		if( pHud )
			(( CDnComboExerciseModeHUD * )pHud)->ShowMaster( m_bPvPComboModeMaster );
	}
}

void CDnPvPGameTask::OnRecvPVPComboModeChangeMaster( SCPvPComboExerciseChangeRoomMaster * pData )
{
	// 콤보연습모드 방장여부.
	CTask * pTask = CTaskManager::GetInstance().GetTask("BridgeTask");
	if( pTask )
	{
		int sessionID = ((CDnBridgeTask *)pTask)->GetSessionID();
		m_bPvPComboModeMaster = ( pData->uiRoomMasterSessionID == sessionID );

		// 변경된 방장이름 출력.
		if( CDnPartyTask::IsActive() )
		{
			CDnPartyTask::PartyStruct * pParty = GetPartyTask().GetPartyDataFromSessionID( pData->uiRoomMasterSessionID );
			if( pParty->hActor )
			{
				std::wstring str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120131 ), pParty->hActor->GetName() ); // "방장이 [%s]님으로 변경되었습니다."
				GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption3, str.c_str(), textcolor::YELLOW );				
			}
		}

		// 방장에게만 몬스터소환UI 출력.
		CDnPVPBaseHUD * pHud = GetInterface().GetHUD();
		if( pHud )
			(( CDnComboExerciseModeHUD * )pHud)->ShowMaster( m_bPvPComboModeMaster );
	}
}

void CDnPvPGameTask::OnRecvPVPComboModeMonsterGen( SCPvPComboExerciseRecallMonsterResult * pData )
{
	if( pData->nRet == ERROR_NONE )
	{
		// 소환 메세지출력.
		GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption3, 7966, textcolor::YELLOW, 3.0f );
	}
}

void CDnPvPGameTask::SummonDummyMonster()
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	if (pLocalActor != NULL)
	{
		// 이동중이면 소환불가.
		//if ((CDnActor::s_hLocalActor->IsMove() && CDnActor::s_hLocalActor->IsMovable()) || 
		//	pLocalActor->IsAutoRun() || pLocalActor->IsFollowing() )

		// "Stand" 상태 일때만 소환가능.
		std::string::size_type pos = CDnActor::s_hLocalActor->GetCurrentPlayAction().find( "Stand" );
		if( pos != std::string::npos )
		{
			// Packet 전송.
			SendPvPSummonMonster();
		}			
		//
		else
		{			
			// "이동 중에는 사용할 수 없습니다."
			GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7938 ) , textcolor::YELLOW , 1.0f );
		}
	}	

}

#endif // PRE_ADD_PVP_COMBOEXERCISE

bool CDnPvPGameTask::IsShowPartyGauge()
{
#ifdef PRE_ADD_2vs2_LADDER
	if( ( GetLadderType() >= LadderSystem::MatchType::_2vs2 ) )
		return true;
#endif

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
	switch( GetGameMode() )
	{
	case PvPCommon::GameMode::PvP_Respawn:
	case PvPCommon::GameMode::PvP_Round:
	case PvPCommon::GameMode::PvP_Occupation:
	case PvPCommon::GameMode::PvP_Captain:
		return true;
	}
#endif

	return false;
}

bool CDnPvPGameTask::IsUseRegulation()
{ 
	if( CDnBridgeTask::IsActive() && CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_bDisablePVPRegulation == true ) 
		return false;

	return m_IsStartRegulation; 
}