#include "StdAfx.h"
#include "DnLocalPlayerActor.h"
#include "EtMatrixEx.h"
#include "DnWorld.h"
#include "InputWrapper.h"
#include "DnCamera.h"
#include "DnPlayerCamera.h"
#include "DnProjectile.h"
#include "DnSkill.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnDropItem.h"
#include "VillageSendPacket.h"
#include "DnVillageTask.h"
#include "DnItemTask.h"
#include "DnPartyTask.h"
#include "DnSkillTask.h"
#include "DnNPCActor.h"
#include "NpcSendPacket.h"
#include "DnMouseCursor.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnWorldOperationProp.h"
#include "DnWorldChestProp.h"
#include "DnWorldNpcProp.h"
#include "EtWorldEventArea.h"
#include "DnStateBlow.h"
#include "GameOption.h"
#include "DnCommonTask.h"
#include "DnMovableChecker.h"
#include "DnJumpableChecker.h"
#include "DnGroundMovableChecker.h"
#include "ItemSendPacket.h"
#include "GameSendPacket.h"
#include "EtDrawQuad.h"
#include "EtPostProcessFilter.h"
#include "DnMainDlg.h"
#include "DnMainMenuDlg.h"
#include "DnSkillTask.h"
#include "DnDamageReaction.h"
#include "DNGestureTask.h"
#include "EtActionSignal.h"
#include "SystemSendPacket.h"
#include "DnGaugeDlg.h"
#include "navigationmesh.h"
#include "DnChatTabDlg.h"
#include "DnChatOption.h"
#include "DnQuickSlotDlg.h"
#include "DnRadioMsgTask.h"
#include "DnFarmGameTask.h"
#include "DnLifeSkillPlantTask.h"
#include "DnLifeTooltipDlg.h"
#include "DnLifeConditionDlg.h"
#include "DnCashShopTask.h"
#ifdef _USE_VOICECHAT
#include "DNVoiceChatTask.h"
#endif
#include "EtConvexVolume.h"
#include "DnCommonUtil.h"
#include "DnCannonMonsterActor.h"
#include "DnCannonCamera.h"
#include "DnLifeSkillFishingTask.h"
#include "DnGuildTask.h"
#include "SyncTimer.h"
#include "DnDisableActionBlow.h"
#include "DnPvPGameTask.h"
#include "DnMutatorGuildWar.h"
#include "DnOccupationTask.h"
#include "DnHideMonsterActor.h"

#if defined(PRE_ADD_ENGLISH_STRING)
#include "DnInterfaceString.h"
#endif // #if defined(PRE_ADD_ENGLISH_STRING)

//#define HACKTEST_MIXEDACTION

#include "DnPetActor.h"
#include "SyncTimer.h"
#include "DnPetTask.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
#include "DnItemCompoundNotifyDlg.h"
#endif 

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

int CDnLocalPlayerActor::s_nSpecialCommandDelay = 150;
int CDnLocalPlayerActor::s_nDoubleCommandDelay = 200;
UINT CDnLocalPlayerActor::ms_nLastHasNpcUID = 0xffffffff;
int CDnLocalPlayerActor::s_nVillageMaximumMoveSendTime = 1000;
int CDnLocalPlayerActor::s_nGameMaximumMoveSendTime = 500;
float CDnLocalPlayerActor::s_fLastSaturationColor;
bool CDnLocalPlayerActor::s_bShowCrosshair = true;

float s_fCrossHairHeightRatio = 0.46f;

#define REFRESH_VIEWVEC_ANGLE 0.995f
#define REFRESH_VIEWVEC_TICK 200
#define REFRESH_SKILL_VIEWVEC_TICK 300

#define ENTER_GATE_CANCEL_LIMIT_TIME 2.f

CDnLocalPlayerActor::CrossHairType CDnLocalPlayerActor::s_CrossHairTypeList[] = {
	{ CrossHairType::Normal, "CrossHair.dds" },
	{ CrossHairType::Enemy, "CrossHairTarget.dds" },
	{ CrossHairType::Npc, "CrossHairTalk.dds" },
	{ CrossHairType::Blue, "Cross01_blue.dds" },
	{ CrossHairType::NotTalk, "CrossHairNotTalk.dds" },
	{ CrossHairType::Player, "CrossHairPlayer.dds" },
	{ CrossHairType::Lever, "CrossLever.dds" },
	{ CrossHairType::Chest_UnLock, "CrossOpen.dds" },
	{ CrossHairType::Chest_Lock, "CrossClose.dds" },
	{ CrossHairType::CursorMode, "CrossHairNon.dds" },
	{ CrossHairType::Pickup, "CrossPick.dds" },
	{ CrossHairType::Seed, "LifeCrossHair08.dds" },
	{ CrossHairType::Water, "LifeCrossHair01.dds" },
	{ CrossHairType::NotWater, "CrossHairNotTalk.dds" },
	{ CrossHairType::Harvest, "LifeCrossHair02.dds" },
	{ CrossHairType::NotHarvest, "CrossHairNotTalk.dds" },
	{ CrossHairType::CannonPick, "CrossPick.dds" },
	{ CrossHairType::CannonTarget, "CrossHairTarget_Cannon.dds" },
	{ CrossHairType::FlagTarget, "GuildCrossHair.dds" }
	//{ CrossHairType::Ride, "CrossHairRide.dds" }, //rlkt_ride
};

CDnLocalPlayerActor::CDnLocalPlayerActor( int nClassID, bool bProcess )
	: CDnPlayerActor( nClassID, bProcess ), CInputReceiver(true)
	, m_vPrevMoveVector( 0.f, 0.f )
{
	memset( m_LastPushDirKeyTime, 0, sizeof(m_LastPushDirKeyTime) );
	memset( m_LastReleaseDirKeyTime, 0, sizeof(m_LastPushDirKeyTime) );
	memset( m_LastPrevPushDirKeyTime, 0, sizeof(m_LastPrevPushDirKeyTime) );
	m_LastSendMoveMsg = 0;

	m_cLastPushDirKey = 0;
	m_cLastPushDirKeyIndex = 0;
	m_cLastSpecialCommand = -1;
	m_bResetMoveMsg = true;
	m_bLockInput = false;
	m_bStandingBy = false;
	m_bProcessInputSignal = false;
	m_cAutoRun = 0;
	m_bApplyInputProcess = true;
	m_bCheckInputSignal = true;

	memset( m_hTargetCross, 0, sizeof(m_hTargetCross) );
	int nCount = sizeof(s_CrossHairTypeList) / sizeof(CrossHairType);
	for( int i=0; i<nCount; i++ ) {
		m_hTargetCross[s_CrossHairTypeList[i].nType] = LoadResource( CEtResourceMng::GetInstance().GetFullName( s_CrossHairTypeList[i].szFileName ).c_str(), RT_TEXTURE );
	}

#ifdef _SHADOW_TEST
	m_bEnableShadowActor = true;
#endif

	m_nWarpLazyCount = -1;
	m_WarpPos = EtVector3(0,0,0);
	m_WarpLook = EtVector2(0,0);
	m_bClashHit = false;

	// 스핵관련
	m_dwSyncDatumTick = 0;

	m_afLastEquipItemSkillDelayTime = 0.0f;
	m_afLastEquipItemSkillRemainTime = 0.0f;
	
	// Account Imp
	m_bAccountHide = false;

	m_pDamageReaction = NULL;

	m_bCancelStageMove = false;
	m_CancelStageMoveDelta = 0.f;
	m_CancelStageMoveDir = 0.f;

//	m_pLastValidCell = NULL;
	m_vLastValidPos = EtVector3( 0.f, 0.f, 0.f );
	m_bIgnoreRefreshViewCamera = false;

	m_fPartyLeaderSleepCheckTimer = 0.f;
	m_LastEscapeTime = 0;
	m_fPosHistoryDelta = 0.f;
	m_bWaitCannonPossessRes = false;
	m_bNeedSendCannonTargeting = false;
	m_LastSendCannonRotateSyncTime = 0;
	m_bCannonRotated = false;
	SecureZeroMemory( &m_vNowCannonGroundHitPos, sizeof(EtVector3) );

	m_fForceSyncViewTime = 0.0f;

	m_nPressedCount = 0;

	m_dwPartyMemberMarkerColor = 0xffffffff;
	m_dwStigmaActorMarkerColor = 0xffffffff;

	m_bInputHasPassiveSignalNotCheckPushKey = false;

	m_bPlayerAwayForIdle = false;
	m_bLockItemMove = false;

	m_bFollowObserver = false;
	m_nObserverUniqueID = 0;

	m_LastAimTarget.Reset();
	SetCurrentGhostType(Ghost::Type::Normal);

	m_vecBackUpEventSlotItem.clear();
	m_fNormalProjectileAdditionalRange = 0.0f;
	m_LastUpdatePickupPet = 0;

#ifdef PRE_SOURCEITEM_TIMECHEAT
	m_bSourceItemTimeCheat = false;
#endif
	m_bCurrentNpcOutlineState = false;
	m_cLastReleaseDirKey = 0;
	m_eSeedState = Farm::AreaState::NONE;
	m_fDelta = 0.0f;
	m_fFollowDeltaTime = 0.0f;
	m_fIdleMotionTimer = 0.0f;
	m_fNpcOutlineCheckDelayTime = 0.0f;
#ifdef PRE_ADD_MAINQUEST_UI
	m_bHideNpc = false;
	m_fHideNpcDelta = .0f;
#endif
}

CDnLocalPlayerActor::~CDnLocalPlayerActor()
{
	for( int i=0; i<CrossHairType::MaxCount; i++ ) 
		SAFE_RELEASE_SPTR( m_hTargetCross[i] );

	//문장 스킬효과 리셋
	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
	if( pSkillTask ) {
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );

		if( pSox ) 
		{
			for( int itr = 0 ; itr < CDnGlyph::GlyphSlotEnum_Amount; ++itr ) 
			{
				if ( m_hGlyph[itr] ) 
				{
					if( CDnGlyph::PassiveSkill == pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_GlyphType" )->GetInteger() )
					{
						int nSkillID = pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_SkillID" )->GetInteger();
						DnSkillHandle hSkill = pSkillTask->FindSkill( nSkillID );

						if( hSkill )
							hSkill->DelGlyphStateEffect( m_hGlyph[itr]->GetClassID() );
					}
				}
			}
		}
	}

	SAFE_RELEASE_SPTR( m_hCannonCamera );

	SAFE_DELETE( m_pDamageReaction );

	SAFE_RELEASE_SPTR( m_hNpcOutline );
}

bool CDnLocalPlayerActor::Initialize()
{
	m_pDamageReaction = new CDnDamageReaction;
	m_pDamageReaction->Initialize();
	CDnPlayerActor::Initialize();

#ifdef _SHADOW_TEST
	if( m_bIsShadowActor ) {
		if( m_hObject ) m_hObject->EnableCollision( false );
	}
#endif

	m_hNpcOutline = CEtOutlineObject::Create( CEtObject::Identity() );
	static EtColor vColor(0.2f, 0.5f, 1.0f, 1.0f);
	m_hNpcOutline->SetColor( vColor );
	m_hNpcOutline->SetWidth( 0.7f );
	m_fNpcOutlineCheckDelayTime = 0.0f;
	m_bCurrentNpcOutlineState = false;

	return true;
}

void CDnLocalPlayerActor::InitializeIdleMotion()
{
	DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TJOB);
	if (!pSox) return;
	int nJobID = GetJobClassID();
	if (pSox->IsExistItem(nJobID))
	{
		DNTableCell* pField = pSox->GetFieldFromLablePtr(nJobID, "_IdleMotionStartTime");
		if (pField)
			m_IdleMotionInfo.fStartTime = pField->GetFloat();

		pField = pSox->GetFieldFromLablePtr(nJobID, "_IdleMotionMinTime");
		if (pField)
			m_IdleMotionInfo.fCoolTimeMin = pField->GetFloat();

		pField = pSox->GetFieldFromLablePtr(nJobID, "_IdleMotionMaxTime");
		if (pField)
			m_IdleMotionInfo.fCoolTimeMax = pField->GetFloat();

		pField = pSox->GetFieldFromLablePtr(nJobID, "_IdleMotionName");
		if (pField)
			m_IdleMotionInfo.motionName = pField->GetString();
	}
}

void CDnLocalPlayerActor::ProcessPartyAbsenceCheck(float fDelta)
{
	bool bAbsenceChecker = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().GetPartyCount() > 1)
	{
		if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon ||
			CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap)
		{
			CInputDevice::GetInstance().HoldAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY, false);
			bAbsenceChecker = true;
			if (m_bPlayerAway == false)
			{
				const float sec = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerAbsenceTime);
				if (CInputDevice::GetInstance().GetAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY) > sec)
				{
					m_bPlayerAway = true;
					GetPartyTask().ReqPartyAbsence(true);
					m_fPartyLeaderSleepCheckTimer = 0.f;
				}
			}
			else
			{
				if (CInputDevice::GetInstance().GetAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY) <= 0.f)
				{
					m_bPlayerAway = false;
					GetPartyTask().ReqPartyAbsence(false);
					CInputDevice::GetInstance().ResetAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY);
				}
				else if (GetPartyTask().IsLocalActorMaster())
				{
					const float partyLeaderAbsenceSec = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PartyLeaderAbsenceTime);
					if (m_fPartyLeaderSleepCheckTimer > 0.f)
					{
						m_fPartyLeaderSleepCheckTimer -= fDelta;
						if (m_fPartyLeaderSleepCheckTimer < 0.f)
						{
							if (GetPartyTask().DelegatePartyLeader(true) == false)
								m_fPartyLeaderSleepCheckTimer = partyLeaderAbsenceSec;
							else
								m_fPartyLeaderSleepCheckTimer = 0.f;
						}
					}
					else
					{
						m_fPartyLeaderSleepCheckTimer = partyLeaderAbsenceSec;
					}
				}
			}
		}
	}

	if (bAbsenceChecker == false)
	{
		m_bPlayerAway = false;
		CInputDevice::GetInstance().ResetAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY);
		CInputDevice::GetInstance().HoldAbsenceCheckTimer(CInputDevice::eABSENCE_PARTY, true);
	}
}

void CDnLocalPlayerActor::ProcessIdleMotion(float fDelta)
{
	if (m_IdleMotionInfo.IsValid() == false)
		return;

	if (CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().GetFishingState() != STATE_NONE)
		return;

	if (IsBattleMode())
	{
		if (m_bPlayerAwayForIdle == true)
		{
			m_bPlayerAwayForIdle = false;
			CInputDevice::GetInstance().ResetAbsenceCheckTimer(CInputDevice::eABSENCE_IDLEMOTION);
			m_fIdleMotionTimer = 0.f;
		}

		return;
	}

	if (m_bPlayerAwayForIdle == false)
	{
		if (CInputDevice::GetInstance().GetAbsenceCheckTimer(CInputDevice::eABSENCE_IDLEMOTION) > m_IdleMotionInfo.fStartTime)
		{
			m_bPlayerAwayForIdle = true;

			CmdAction(m_IdleMotionInfo.motionName.c_str());
			// 현재 JobTable의 _IdleMotion 관련 값들이 타입은 float이지만 초단위 정수로 들어가 있어서 정수 난수로 돌립니다. by kalliste
			m_fIdleMotionTimer = (float)GetRandom().rand((int)m_IdleMotionInfo.fCoolTimeMin, (int)m_IdleMotionInfo.fCoolTimeMax);
		}
	}
	else
	{
		if (CInputDevice::GetInstance().GetAbsenceCheckTimer(CInputDevice::eABSENCE_IDLEMOTION) <= 0.f)
		{
			m_bPlayerAwayForIdle = false;
			CInputDevice::GetInstance().ResetAbsenceCheckTimer(CInputDevice::eABSENCE_IDLEMOTION);
			m_fIdleMotionTimer = 0.f;
			return;
		}

		if (m_fIdleMotionTimer <= 0.f)
		{
			CmdAction(m_IdleMotionInfo.motionName.c_str());
			m_fIdleMotionTimer = (float)GetRandom().rand((int)m_IdleMotionInfo.fCoolTimeMin, (int)m_IdleMotionInfo.fCoolTimeMax);
		}

		m_fIdleMotionTimer -= fDelta;
	}
}

extern bool g_bNaviDraw;
void CDnLocalPlayerActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_bProcessInputSignal = false;
	m_fDelta = fDelta;
	EtMatrix matWorld = *m_matexWorld;

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera )
	{	
		switch( hCamera->GetCameraType() )
		{
			case CDnCamera::PlayerCamera:
				{
					if( /*!IsDie() && !CDnMouseCursor::GetInstance().IsShowCursor() && */!IsLockInput() ) 
						ProcessInput( LocalTime, fDelta );

					if( hCamera->IsFreeze(1) && !IsSignalRange( STE_FreezeCamera ) ) {
						hCamera->SetFreeze( false, 1 );
					}
				}
				break;

			case CDnCamera::CannonCamera:
				{
					if( hCamera->IsFreeze(1) && false == IsSignalRange( STE_FreezeCamera ) )
					{
						hCamera->SetFreeze( false, 1 );
					}
				}
				break;
		}
		if( !hCamera->IsFreeze(1) ) m_bIgnoreRefreshViewCamera = false;
	}

	if(hCamera && !IsVehicleMode()) RefreshMoveVector( hCamera ); // 탈것 상태에서는 돌려줄필요가 없습니다.
		CDnPlayerActor::Process( LocalTime, fDelta );
	if( !IsVehicleMode() && !IsCallingVehicle() ) 
	{
		// 캐릭터가 액션중 바뀌는 View 방향을 체크해서 Refresh 해주는 함수
		ProcessActionSyncView();
		// 게임서버에서 CmdMove 대신 작은 양의 패킷사이즈로 일정시간마다 싱크를 마줘주는 함수
		// 원래 Village 두 동일하게 먹어두 되는데 빌리지에서 다른 유저 Enter 시 CmdMove 패킷으로
		// 보내지 않으면 이상해지므로 겜서버에서만 POSREV 로 마춘다.
		ProcessPositionRevision();
	}
	if( m_fForceSyncViewTime > 0.0f )
		m_fForceSyncViewTime -= fDelta;

	ProcessLastValidNavCell( LocalTime, fDelta );
	ProcessSyncPressedPos();

	if( hCamera ) {
		if( !IsHit() && !IsDie() ) hCamera->SetFreeze( false );
		if( IsGhost() ) hCamera->SetFreeze( false );
		if(!IsVehicleMode()) hCamera->PostProcess( LocalTime, fDelta ); // 탈것 상태에서는 돌려줄필요가 없습니다.
	}

	// 마우스로 카메라를 조작했을 때만 처리된다. 서버로 패킷도 보냄. 다른 유저들에게 브로드캐스팅..
	if( IsCannonMode() )
		ProcessCannonRotateSync();

	ProcessCrosshair();

	if (m_bCancelStageMove)
	{
		CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
		if( pTask ) 
		{
			if (pTask->IsLocalActorEnterGateReady() && m_CancelStageMoveDelta < ENTER_GATE_CANCEL_LIMIT_TIME)
			{
				//	todo : frame filtering
				EtVector2 vMoveVec( 0.f, m_CancelStageMoveDir );
				vMoveVec *= fDelta * (float)GetMoveSpeed();
				m_CancelStageMoveDelta += fDelta;
				OutputDebug("CancelStageMoveDelta:%f\n", m_CancelStageMoveDelta);

				if(IsVehicleMode() && GetMyVehicleActor())
					GetMyVehicleActor()->CmdMove( vMoveVec, m_CancelStageMoveActionName.c_str(), -1, 6.f );
				else
					CmdMove( vMoveVec, m_CancelStageMoveActionName.c_str(), -1, 6.f );

			}
			else
			{
				if (pTask->IsWaitingPartyFindDlgOpen() && m_CancelStageMoveDelta < ENTER_GATE_CANCEL_LIMIT_TIME)
				{
					CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
					if( pMainMenuDlg )
					{
						pMainMenuDlg->ToggleShowDialog( CDnMainMenuDlg::COMMUNITY_DIALOG );
						pMainMenuDlg->SwapPartyDialog();
					}
					pTask->SetWaitingPartyFindDlgOpen(false);
				}

				m_bCancelStageMove = false;
				m_CancelStageMoveActionName.clear();
				m_CancelStageMoveDelta = 0.f;
				m_CancelStageMoveDir = 0.f;
				CmdStop( "Stand", 0, 6.f );

				LockInput(false);
			}
		}
	}

	if( m_nWarpLazyCount > 0 ) {
		m_nWarpLazyCount--;
		if( m_nWarpLazyCount == 0 ) {
			CDnPlayerActor::CmdWarp(m_WarpPos, m_WarpLook);
			m_WarpPos = EtVector3(0,0,0);
			CDnGameTask* pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pTask ) pTask->ResetPlayerCamera();
		}
	}

	// 상용 아이템에 스킬 들어갈 수 있고, 추가적으로 문장에 스킬이 들어감. 일단 그냥 악세사리 루프도 돌자.
	if( m_afLastEquipItemSkillRemainTime != 0.0f )
	{
		m_afLastEquipItemSkillRemainTime -= fDelta;
		if( m_afLastEquipItemSkillRemainTime < 0.0f )
			m_afLastEquipItemSkillRemainTime = 0.0f;
	}

	ProcessAccountImp( LocalTime, fDelta );

	if( m_pDamageReaction ) {
		m_pDamageReaction->Process( fDelta, (int)GetHP(), GetHPPercent() );
	}

	ProcessFollow( fDelta );

	SetInteractivePos( GetPosition(), fDelta );
	UpdatePositioinHistory( fDelta );

	if( !CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
	{
		ProcessPartyAbsenceCheck(fDelta);
	}
	ProcessIdleMotion(fDelta);

	// 시그널 스킬 사용 제한시간 체크 및 초기화. 해당 시그널의 영역이 지나면 초기화 시킨다.
	if( CDnMovableChecker::GetUseSkillSignalEnd() < LocalTime )
	{
		CDnMovableChecker::SetUseSignalSkillCheck( false, 0 );
		CDnMovableChecker::SetPrimUseSignalSkillCheck( false );
	}
	
	if( CDnJumpableChecker::GetUseSkillSignalEnd() < LocalTime )
	{
		CDnJumpableChecker::SetUseSignalSkillCheck( false, 0 );
		CDnJumpableChecker::SetPrimUseSignalSkillCheck( false );
	}

	if( CDnGroundMovableChecker::GetUseSkillSignalEnd() < LocalTime )
	{
		CDnGroundMovableChecker::SetUseSignalSkillCheck( false, 0 );
		CDnGroundMovableChecker::SetPrimUseSignalSkillCheck( false );
	}
	//////////////////////////////////////////////////////////////////////////
		
#ifdef _USE_VOICECHAT
	if( CDnVoiceChatTask::IsActive() ) {
		if( CDnPartyTask::IsActive() && GetPartyTask().GetPartyCount() > 1 ) {
			EtVector2 vLook( m_matexWorld.m_vZAxis.x, m_matexWorld.m_vZAxis.z );
			GetVoiceChatTask().SetRotation( vLook );
		}
	}
#endif

#ifdef PRE_MOD_NAVIGATION_PATH
	if( g_bNaviDraw )
	{
		MAWalkMovement* pWalkMovement = dynamic_cast<MAWalkMovement*>(GetMovement());
		if( pWalkMovement )
		{
			WAYPOINT_LIST& fur_waypoint_list = pWalkMovement->GetWayPoints();
			WAYPOINT_LIST::iterator it = fur_waypoint_list.begin();
			
			int i = 0;
			for( ; it != fur_waypoint_list.end(); it++, i++ )
			{
				WAYPOINT& point = *it;
				EtVector3 vPos1, vPos2;
				vPos1 = point.Position;

				WAYPOINT_LIST::iterator nextIt = it;
				++nextIt;
				if ( nextIt == fur_waypoint_list.end() )
					break;

				vPos2 = (*nextIt).Position;
				if( i % 2 == 0 )
					EternityEngine::DrawLine3D( vPos1, vPos2, 0xffff0000 );
				else
					EternityEngine::DrawLine3D( vPos1, vPos2, 0xff0000ff );
			}

			WAYPOINT_LIST& losWaypointList = pWalkMovement->GetLOSWayPoints();
			it = losWaypointList.begin();
			
			i = 0;
			for( ; it != losWaypointList.end(); it++, i++ )
			{
				WAYPOINT& point = *it;
				EtVector3 vPos1, vPos2;
				vPos1 = point.Position;

				WAYPOINT_LIST::iterator nextIt = it;
				++nextIt;
				if ( nextIt == losWaypointList.end() )
					break;

				vPos2 = (*nextIt).Position;
				if( i%2 == 0 )
					EternityEngine::DrawLine3D( vPos1, vPos2, 0xffffff00 );
				else
					EternityEngine::DrawLine3D( vPos1, vPos2, 0xffffff00 );
			}
		}
	}
#endif // PRE_MOD_NAVIGATION_PATH

	ProcessNpcOutline( fDelta );

#ifdef PRE_ADD_SHUTDOWN_CHILD
	if (CDnBridgeTask::IsActive())
		CDnBridgeTask::GetInstance().ProcessShutDownSystem(fDelta);
#endif

#ifdef PRE_ADD_MAINQUEST_UI
	ProcessNpcHide( LocalTime, fDelta );
#endif
}

void CDnLocalPlayerActor::ProcessLastValidNavCell( LOCAL_TIME LocalTime, float fDelta )
{
	if( !CDnWorld::IsActive() ) return;
	NavigationMesh *pNavMesh = CDnWorld::GetInstance().GetNavMesh( GetMatEx()->m_vPosition );
	if( !pNavMesh ) return;

	NavigationCell *pCell = pNavMesh->FindCell( m_matexWorld.m_vPosition );
	if( pCell ) {
		m_vLastValidPos = m_matexWorld.m_vPosition;
	}
}

void CDnLocalPlayerActor::ProcessSyncPressedPos()
{
	if( IsHit() ) return;
	if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg < 400 ) return;
	if( m_nPressedCount < 10 ) return;

	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	DWORD dwGap = GetSyncDatumGap();

	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

	Send( eActor::CS_SYNCPRESSEDPOS, &Stream );

	m_nPressedCount = 0;
}

#ifdef PRE_ADD_MAINQUEST_UI
void CDnLocalPlayerActor::ProcessNpcHide( LOCAL_TIME LocalTime, float fDelta )
{
	if(!CDnActor::s_hLocalActor)
		return;
	 
	bool bShow = true;
	bool bShowShadow = true;
	float fAlpha = 1.0f;

	if(m_fHideNpcDelta > 0.0f)
	{
		m_fHideNpcDelta -= fDelta;
		if(m_fHideNpcDelta < 0.0f) m_fHideNpcDelta = 0.0f;

		if(m_bHideNpc)
		{
			bShowShadow = false;
			fAlpha = m_fHideNpcDelta;
		}
		else
		{
			bShowShadow = true;
			fAlpha = 1.0f - m_fHideNpcDelta;
		}

		if(m_fHideNpcDelta == 0.0f)
		{
			if(m_bHideNpc) bShow = false;
			else bShowShadow = true;
		}
	}
	else
	{
		if(m_bHideNpc)
		{
			bShowShadow = bShow = false;
			fAlpha = 0.0f;
		}
		else
		{
			bShow = bShowShadow = true;
			fAlpha = 1.0f;
		}
	}

	DNVector(DnActorHandle) hVecList;
	int nCount = CDnActor::ScanActor( *CDnActor::s_hLocalActor->GetPosition(), 100000.0f, hVecList);

	for(int i = 0 ; i < nCount ; ++i)
	{
		if( hVecList[i] == false ) 
			continue;
		
		if( hVecList[i] == CDnActor::s_hLocalActor->GetMySmartPtr() ) 
			continue;
		
		if( hVecList[i]->IsNpcActor() == false ) 
			continue;
		
		if( hVecList[i]->IsFromMapTriggerHideNpc() ) 
			continue;		
		
		if( hVecList[i]->IsShow() != bShow )					hVecList[i]->Show(bShow);
		if( hVecList[i]->IsEnableCastShadow() != bShowShadow )	hVecList[i]->EnableCastShadow(bShowShadow);
		if( hVecList[i]->GetAlpha() != fAlpha )					hVecList[i]->SetAlphaBlend(fAlpha);
	}
}
#endif // PRE_ADD_MAINQUEST_UI

void CDnLocalPlayerActor::ProcessNpcOutline( float fDelta )
{
	if( !CDnActor::s_hLocalActor ) return;

	m_fNpcOutlineCheckDelayTime -= fDelta;
	if( m_fNpcOutlineCheckDelayTime > 0.0f ) return;

	DNVector(DnActorHandle) hVecList;
	int nCount = CDnActor::ScanActor( *CDnActor::s_hLocalActor->GetPosition(), 110.0f, hVecList );
	float fShortestDistanceSQ = FLT_MAX;
	DnActorHandle hShortestDistActor;
	for( int i=0; i<nCount; i++ ) {
		if( !hVecList[i] ) continue;
		if( hVecList[i] == CDnActor::s_hLocalActor->GetMySmartPtr() ) continue;
		if( !hVecList[i]->IsNpcActor() ) continue;
		if( !hVecList[i]->IsShow() ) continue;

		EtVector3 vDistance = (*CDnActor::s_hLocalActor->GetPosition()) - (*hVecList[i]->GetPosition());
		float fLengthSQ = EtVec3LengthSq( &vDistance );
		if( fLengthSQ < fShortestDistanceSQ )
		{
			fShortestDistanceSQ = fLengthSQ;
			hShortestDistActor = hVecList[i];
		}
	}
	bool bOutline = false;
	if( hShortestDistActor && fShortestDistanceSQ < FLT_MAX ) {
		if( CanTalkNpc( hShortestDistActor ) == char(true) ) {
			EtObjectHandle hObject;
			if( hShortestDistActor->GetObjectHandle() ) {
				hObject = hShortestDistActor->GetObjectHandle();
			}
			else {
				CDnActor *pActor = hShortestDistActor.GetPointer();
				if( ((CDnNPCActor*)pActor)->IsNpcProp() ) {
					DnPropHandle hProp = ((CDnNPCActor*)pActor)->GetPropHandle();
					if( hProp && hProp->GetObjectHandle() )
						hObject = hProp->GetObjectHandle();
				}
			}
			if( hObject ) {
				if( m_hNpcOutline->GetObject() && m_hNpcOutline->GetObject() != hObject ) {
					// 두 npc가 붙어있어서 A에서 B로 라인이펙트가 바로 이동할 경우 부드럽게 처리하기 위해 이렇게 한다.
					m_hNpcOutline->SetObject( CEtObject::Identity() );
					m_fNpcOutlineCheckDelayTime = 0.5f;
				}
				else {
					m_hNpcOutline->SetObject( hObject );
					m_hNpcOutline->Show( true, 2.0f ); 
					bOutline = true;
					m_bCurrentNpcOutlineState = true;
					m_hActorNpcOutline = hShortestDistActor;
				}
			}
		}
	}
	if( !bOutline ) {
		m_hNpcOutline->Show( false, 2.0f );
		m_bCurrentNpcOutlineState = false;
	}

	m_fNpcOutlineCheckDelayTime = 0.1f;
}

void CDnLocalPlayerActor::RefreshMoveVector( DnCameraHandle hCamera )
{
	if( EtVec3LengthSq( GetMovePos() ) > 0.f ) return;
	// 이동할때 케릭의 EtMatrixEx 값으로 하는게 아니구
	// 카메라의 Vector 값으로 해야하기 떔시 MoveVector 를 셋해준다.
	EtVector3 vXVec, vZVec;
	/*
	if( m_hFollowActor ) { // Follow 시에는 그냥 Look Vector 와 동일하게 MoveVector 를 맞춥니다.
		vZVec = *GetLookDir();
		EtVec3Cross( &vXVec, &vZVec, &EtVector3( 0.f, 1.f, 0.f ) );
		vXVec.y = vZVec.y = 0.f;
	}
	else {
	}
	*/
	vXVec = hCamera->GetMatEx()->m_vXAxis;
	vZVec = hCamera->GetMatEx()->m_vZAxis;
	vXVec.y = vZVec.y = 0.f;
	EtVec3Normalize( &vXVec, &vXVec );
	EtVec3Normalize( &vZVec, &vZVec );
	SetMoveVectorX( vXVec );
	SetMoveVectorZ( vZVec );
}

void CDnLocalPlayerActor::ProcessActionSyncView()
{
	EtVector2 vCurZVec;
	if( m_fForceSyncViewTime > 0.0f ) {
		// 시그널 사용해서 짧은 시간동안만 동기화맞추는거로 처리하니, 유저입력에 의해 카메라가 변경되었는지를 판단할 필요가 없는거 같다.
		vCurZVec = EtVector2( GetMoveVectorZ()->x, GetMoveVectorZ()->z );
		if( EtVec2Dot( &vCurZVec, &m_vPrevMoveVector ) >= REFRESH_VIEWVEC_ANGLE + 0.002f ) return;
		if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg < 75 ) return;
	}
	else {
		if( m_bResetMoveMsg == false ) return;
		if( IsStay() || IsHit() ) return;
		vCurZVec = EtVector2( GetMoveVectorZ()->x, GetMoveVectorZ()->z );
		if( EtVec2Dot( &vCurZVec, &m_vPrevMoveVector ) >= REFRESH_VIEWVEC_ANGLE ) return;
		if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg < 100 ) return;
	}

	m_vPrevMoveVector = vCurZVec;
	m_LastSendMoveMsg = CDnActionBase::m_LocalTime;

	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	DWORD dwGap = GetSyncDatumGap();

	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

	Send( eActor::CS_VIEWSYNC, &Stream );
}

void CDnLocalPlayerActor::ProcessPositionRevision( bool bForce )
{
	bool bSendRevision = false;
	if( !bForce ) {
		if( IsHit() ) return;
		if( IsStay() && IsLockInput() ) return;

#ifdef PRE_ADD_MONSTER_CATCH
		if( m_hCatcherMonster ) return;		// 잡힌 중에도 보내지 않음.
#endif // #ifdef PRE_ADD_MONSTER_CATCH

		int nMoveSendTime = s_nGameMaximumMoveSendTime;
		CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pTask ) nMoveSendTime = s_nVillageMaximumMoveSendTime;
		if( IsStay() ) nMoveSendTime = 10000;

		if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg < nMoveSendTime ) return;
		bSendRevision = true;
	}
	else bSendRevision = true;

	if( bSendRevision ) {
		BYTE pBuffer[128] = { 0, };
		CPacketCompressStream Stream( pBuffer, 128 );
		DWORD dwGap	= GetSyncDatumGap();
		int nMoveSpeed = CDnActor::GetMoveSpeed();

		CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pTask ) {
			if( EtVec3Length( GetMovePos() ) == 0.f ) {
				int nTemp = m_nActionIndex;
				Stream.Write( &dwGap, sizeof(DWORD) );
				Stream.Write( &nTemp, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );
				Stream.Write( &nMoveSpeed, sizeof(int) );

				Send( eActor::CS_CMDMOVE, &Stream );
			}
			else {
				/*
				Stream.Write( &dwGap, sizeof(DWORD) );
				Stream.Write( &m_nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Write( GetMovePos(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				Send( eActor::CS_CMDMOVEPOS, &Stream );
				*/
			}
		}

		pTask = CTaskManager::GetInstance().GetTask( "GameTask" );
		if( pTask ) {
			bool bMove	= IsMove();
			EtVector3 vPos = EtVec3LengthSq(&m_WarpPos) > 0 ? m_WarpPos : *GetPosition();
			Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Write( &dwGap, sizeof(DWORD) );
			Stream.Write( &bMove, sizeof(bool) );
			if( bMove ) 
				Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

			Send( eActor::CS_POSREV, &Stream );
		}

		m_LastSendMoveMsg = CDnActionBase::m_LocalTime;

//		OutputDebug( "CmdPosRev\n" );

	}
}

DnPropHandle CDnLocalPlayerActor::GetLastAimProp()
{
	return m_LastAimTarget.hProp;
} 

DnActorHandle CDnLocalPlayerActor::GetLastAimActor()
{
	return m_LastAimTarget.hActor;
}

void CDnLocalPlayerActor::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime /* = -1 */ )
{
	if( IsLockInput() || CGlobalInfo::GetInstance().IsPlayingCutScene() ) return;
	if( nReceiverState & CInputReceiver::IR_MOUSE_LB_DOWN )
	{
		switch ( m_LastAimTarget.TargetType )
		{
		case CrossHairType::Normal:
			break;
		case CrossHairType::Enemy:
			break;
		case CrossHairType::Npc:
			{
				if ( CDnMouseCursor::GetInstance().IsShowCursor() ) break;
				DnActorHandle hAimActor = GetLastAimActor();
				if( !hAimActor ) break;

				char cResult = CanTalkNpc( hAimActor );

				if( cResult != (char)true ) {
					if( cResult == -1 ) 
						GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3, 100030, textcolor::RED, 4.0f );
					break;
				}
				if( CDnPartyTask::GetInstance().IsRequestEnteredGate() || CDnPartyTask::GetInstance().GetEnteredGateIndex() != INVALID_GATE_INDEX )
					break;
#ifdef PRE_ADD_SECONDARY_SKILL
				if( GetLifeSkillFishingTask().IsRequestFishingReady() || GetLifeSkillFishingTask().IsNowFishing() )
					break;
#endif // PRE_ADD_SECONDARY_SKILL

				CDnNPCActor* pNPC = dynamic_cast<CDnNPCActor*>(hAimActor.GetPointer());
				CDnNPCActor::NpcData& data = pNPC->GetNpcData();

				bool bSkipNpcMessage = false;

				if(pNPC->GetNPCJobType() == CDnNPCActor::emJobType::typeCompleteQuest)
				{
					CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
					if( pTask ) bSkipNpcMessage = pTask->OpenCompleteNpcTalk(pNPC->GetNpcData().nNpcID);
				}

				// 사용자 정의에 의해 NPCTALK를 할때 NpcTalkStringGroup에 해당 스트링을 추가하세요!
				if(!bSkipNpcMessage)
				{
					CDnCommonTask* pCommonTask = dynamic_cast<CDnCommonTask*>( CTaskManager::GetInstance().GetTask( "CommonTask" ) );
					if(pCommonTask)
					{
						if( !pCommonTask->IsRequestNpcTalk() )
						{
							SendNpcTalk(pNPC->GetUniqueID(), NpcTalkStringGroup[NPCTALK_START], data.wszTalkFileName);
							pCommonTask->SetNpcTalkRequestWait();
						}
					}
				}

				LockInput(true);


				if( IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsMovable() 
					&& (GetMyVehicleActor()->IsMove() || strstr( GetMyVehicleActor()->GetCurrentAction(), "Turn" ) ) ) // Turn 동작은 Stay 형태로 구분되니 따로걸러줍니다.
				{
					GetMyVehicleActor()->CmdStop("Stand",0,10.f);
				}

				bool bBattleMode = false;
				if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) {
					bBattleMode = IsBattleMode();
				}

				if( IsMove() && IsMovable() ) CmdStop( bBattleMode ? "PutIn_Weapon" : "Stand", 0, bBattleMode ? 0.f : 3.f );
				if( bBattleMode ) CmdToggleBattle( false );

				ResetAutoRun();
				CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
				if( pTask ) pTask->BeginNpcTalk( hAimActor );
			}
			break;
		case CrossHairType::Player:
			{
				if( !CDnMouseCursor::GetInstance().IsShowCursor() )
				{
					// Note : 다이렉트 커뮤니티 UI를 띄운다.
					//
					DnActorHandle hAimActor = GetLastAimActor();
					if( hAimActor ) {
						CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hAimActor.GetPointer());
						if( pPlayer && ( ( pPlayer->GetAccountLevel() < AccountLevel_New ) || ( pPlayer->GetAccountLevel() == AccountLevel_QA ) ) ) {
							GetInterface().OpenDirectDialog( GetLastAimActor()->GetUniqueID(), 
								GetLastAimActor()->GetLevel(),
								pPlayer->GetJobName(),
								GetLastAimActor()->GetName() );

							StopAllPartyPlayer();

						}
					}
				}
			}
			break;
		case CrossHairType::Seed:
		case CrossHairType::Water:
		case CrossHairType::NotWater:
		case CrossHairType::Harvest:
		case CrossHairType::NotHarvest:
			{

				if( IsVehicleMode() && GetMyVehicleActor() )
				{
					CDnVehicleActor * pVehicleActor = GetMyVehicleActor();
					if( ( !pVehicleActor->IsStay() && !pVehicleActor->IsMove() ) || IsAir() )	break;
				}
				else
					if( ( !IsStay() && !IsMove() ) || IsHit() || IsAir() && IsDie() ) break;

				DnPropHandle hProp = GetLastAimProp();
				if( !hProp ) break;
				GetLifeSkillPlantTask().PropOperation( hProp );
			}
			break;
		case CrossHairType::Lever:
		case CrossHairType::Pickup:
		case CrossHairType::Chest_UnLock:
		//case CrossHairType::UnkTarget:
			{
				bool bChest_OpenCheck = true;
				if( ( !IsStay() && !IsMove() ) || IsHit() || IsAir() && IsDie() ) bChest_OpenCheck = false;

				if( IsCanVehicleMode() && IsVehicleMode() && GetMyVehicleActor() && !GetMyVehicleActor()->IsAir() && !GetMyVehicleActor()->IsMove() && GetMyVehicleActor()->IsStay())
					bChest_OpenCheck = true;

				if(!bChest_OpenCheck)
					break;

				DnPropHandle hProp = GetLastAimProp();
				if( !hProp ) break;

				hProp->CmdOperation();

				if( hProp->GetPropType() != PTE_KeepOperation )
					StopAllPartyPlayer();
			}
			break;
		case CrossHairType::Chest_Lock:
			{
				// 못연다구 뭔가 나와주게 한다.
			}
			break;

		case CrossHairType::CannonPick:
			{
				// 대포 발사 모드로 전환.
				if( ( !IsStay() && !IsMove() ) || IsHit() || IsAir() && IsDie() ) 
					break;

				if( false == m_bWaitCannonPossessRes )
					SendPossessCannonReq();

				// TODO: 서버로부터 승인 응답 받고 대포 포지션으로 캐릭터를 옮기고 액션을 멈추던가 해야함. 안 그럼 서버랑 위치 틀어짐.
				//StopAllPartyPlayer();
			}
			break;

		case CrossHairType::FlagTarget:
			{
				if( IsVehicleMode() && GetMyVehicleActor() )
				{
					CDnVehicleActor * pVehicleActor = GetMyVehicleActor();
					if( ( !pVehicleActor->IsStay() && !pVehicleActor->IsMove() ) || IsAir() )	break;
				}
				else if( ( !IsStay() && !IsMove() ) || IsHit() || IsAir() || IsDie() || !IsMovable() ) break;

				DnPropHandle hProp = GetLastAimProp();
				if( !hProp ) break;

				if( CDnOccupationTask::IsActive() )
					GetOccupationTask().FlagOperation( hProp ); 
			}
			break;
		}
	}

	// 드랍 아이템 이름 표시
	if( IsPushKey( IW(IW_TOGGLEDROPITEM) ) ) {
		if( CDnDropItem::IsActive() ) {
#ifdef PRE_MOD_ALWAYS_SHOW_DROP_ITEM
#else
			CDnDropItem::ToggleToolTip();
#endif 
		}
		ReleasePushKey( IW(IW_TOGGLEDROPITEM) );
	}

	// 마우스 감도
	if( IsPushKey( IW(IW_DECREASEMOUSE) ) ) {
		float fValue = CGameOption::GetInstance().GetMouseSendsitivity();
		fValue -= 0.01f;
		if( fValue < 0.01f ) fValue = 0.01f;
		CGameOption::GetInstance().SetMouseSensitivity( fValue );
		CGameOption::GetInstance().ApplyControl();
		ReleasePushKey( IW(IW_DECREASEMOUSE) );

		int nValue = (int)( CGameOption::GetInstance().GetMouseSendsitivity() * 10000.f ) + 10;
		WCHAR wszTemp[256];
		wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100055 ), (int)( nValue / 100.f ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
	}
	if( IsPushKey( IW(IW_INCREASEMOUSE) ) ) {
		float fValue = CGameOption::GetInstance().GetMouseSendsitivity();
		fValue += 0.01f;
		if( fValue > 1.f ) fValue = 1.f;
		CGameOption::GetInstance().SetMouseSensitivity( fValue );
		CGameOption::GetInstance().ApplyControl();
		ReleasePushKey( IW(IW_INCREASEMOUSE) );

		int nValue = (int)( CGameOption::GetInstance().GetMouseSendsitivity() * 10000.f ) + 10;
		WCHAR wszTemp[256];
		wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100055 ), (int)( nValue / 100.f ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
	}
	if( IsPushKey( g_UIWrappingKeyData[IW_PVPTAB] ) && m_bCurrentNpcOutlineState ) {
		ReleasePushKey( g_UIWrappingKeyData[IW_PVPTAB] );
		m_bCurrentNpcOutlineState = false;
		m_LastAimTarget.hActor = m_hActorNpcOutline;
		m_LastAimTarget.TargetType = CrossHairType::Npc;
		int nTempReceiverState = CInputReceiver::IR_MOUSE_LB_DOWN;
		OnInputReceive( nTempReceiverState, LocalTime );
		return;
	}

	// 자동 달리기 - 라이트 클릭 유지시
	/*
	if( nReceiverState & CInputReceiver::IR_MOUSE_RB_DOWN ) {
		if( IsBattleMode() ) m_cAutoRun = 1;
		else {
			if( strcmp( GetCurrentAction(), "Normal_Stand" ) == NULL ) {
				m_cAutoRun = 2;
			}
		}
	}
	if( nReceiverState & CInputReceiver::IR_MOUSE_RB_UP ) m_cAutoRun = 0;
	*/
	//////////////////////////////////////
	if( !CEtUIControl::IsFocusEditBox() && 
		( ( nReceiverState & CInputReceiver::IR_KEY_DOWN ) || 
		  ( nReceiverState & CInputReceiver::IR_JOYPAD_DOWN )) ) 
	{
		if( IsPushKey( DIK_ESCAPE ) ) {
			if( IsLockInput() && EtInterface::g_bEtUIRender ) LockInput( false );
		}
		// 자동 달리기 - E키
		if( IsPushKey( IW(IW_TOGGLEAUTORUN) ) ) 
		{
			if( m_cAutoRun == 0 ) 
			{
				if( IsMovable() && !IsAir() || (IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsMovable())) 
				{
					ResetAutoRun();
					m_cAutoRun = 2;
					m_bResetMoveMsg = true;
					ReleaseAllButton();
				}
			}
			else if( m_cAutoRun == 2 ) 
			{
				m_cAutoRun = 0;
			}
		}

//RLKT_TEST!
		bool bPushTemp = false;
#ifndef _FINAL_BUILD 
		if( IsPushKey( DIK_F1 ) )
		{
			CGlobalValue::GetInstance().m_bShowMonsterAction = !CGlobalValue::GetInstance().m_bShowMonsterAction;
			
			// 몬스터액션이 ON상황에만, 정보를 갱신한다.
			(CGlobalValue::GetInstance().m_bShowMonsterAction) ? CGlobalValue::GetInstance().m_bSetMonster = true :
																 CGlobalValue::GetInstance().m_bSetMonster = false;

			ReleasePushKey( DIK_F3 );
		}
		if( IsPushKey( DIK_F3 ) ) {
			CDnPlayerCamera::s_fCenterPushWidth -= 1.f;
			ReleasePushKey( DIK_F3 );
			bPushTemp = true;
		}
		if( IsPushKey( DIK_F4 ) ) {
			CDnPlayerCamera::s_fCenterPushWidth += 1.f;
			ReleasePushKey( DIK_F4 );
			bPushTemp = true;
		}

		if( IsPushKey( DIK_F5 ) ) {
			CDnPlayerCamera::s_fCenterPushHeight -= 1.f;
			ReleasePushKey( DIK_F5 );
			bPushTemp = true;
		}
		if( IsPushKey( DIK_F6 ) ) {
			CDnPlayerCamera::s_fCenterPushHeight += 1.f;
			ReleasePushKey( DIK_F6 );
			bPushTemp = true;
		}
		if( bPushTemp ) {
			WCHAR wszTemp[256] = { 0, };
			char szTemp[256];
			sprintf_s( szTemp, "크로스헤어 옵셋 : 가로: %.1f, 세로 : %.3f", CDnPlayerCamera::s_fCenterPushWidth, s_fCrossHairHeightRatio );
			MultiByteToWideChar( CP_ACP, 0, szTemp, -1, wszTemp, 256 );
			CDnInterface::GetInstance().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp );
		}
#endif
	}
}

void CDnLocalPlayerActor::ProcessDie( LOCAL_TIME LocalTime, float fDelta )
{
	CDnPlayerActor::ProcessDie( LocalTime, fDelta );

	/*
	bool bGhostMode = true;
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->bIsPvPGameTask() ) bGhostMode = false;

	if( !bGhostMode ) {
		if( IsDie() ) {
			if( !bGhostMode && m_fDieDelta == 0.f && !IsShow() && !IsStandingBy() ) {
				SetStandingBy( true );
			}
		}
		else {
			if( IsStandingBy() ) {
				SetStandingBy( false );
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) return;
				CDnPlayerCamera *pPlayerCamera = dynamic_cast<CDnPlayerCamera*>(hCamera.GetPointer());
				if( !pPlayerCamera ) return;
				if( pPlayerCamera->GetAttachActor() != CDnActor::s_hLocalActor ) {
					pPlayerCamera->AttachActor( CDnActor::s_hLocalActor );
				}
			}
		}
	}
	*/
}

 
// mixed action 핵 테스트 코드
#ifdef HACKTEST_MIXEDACTION
string g_szActionBoneName; 
string g_szMaintenanceBoneName;
string g_szCurAction = "Attack1_Mace";
float g_fTempFrame = 13.0f;
float g_fBlendFrame = 8.0f;
DWORD g_dwTimeStamp = 0;
DWORD g_dwTimeGap = 100;
bool g_bTest = true;
#endif // #ifdef HACKTEST_MIXEDACTION

#include <mmsystem.h>

void CDnLocalPlayerActor::ProcessInput( LOCAL_TIME LocalTime, float fDelta )
{
	if(IsVehicleMode()) return;
	if(IsFollowObserver() ) return;

	// mixed action 핵 테스트 코드
#ifdef HACKTEST_MIXEDACTION
	if( g_bTest )
	{
		if( false == g_szActionBoneName.empty() )
		{
			if( timeGetTime() - g_dwTimeStamp > g_dwTimeGap )
			{
				ResetMixedAnimation( false );
				if( !g_szActionBoneName.empty() && !g_szMaintenanceBoneName.empty() )
					CmdMixedAction( g_szActionBoneName.c_str(), g_szMaintenanceBoneName.c_str(), g_szCurAction.c_str(), 0, g_fTempFrame, g_fBlendFrame );
				SetCustomAction( g_szCurAction.c_str(), g_fTempFrame );

				g_dwTimeStamp = timeGetTime();
				return;
			}
		}
	}
#endif // #ifdef HACKTEST_MIXEDACTION

	std::string szAction;
	std::string szCurAction = GetCurrentAction();

	bool bMovable = false;
	m_bApplyInputProcess = true;
	EtVector2 vMoveVec( 0.f, 0.f );

	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();
	
	// 자동달리기는 커서가 보여도 되야하기땜에..위로 뺀다.
	if( IsMovable() && !IsHit() && !IsDown() ) {
		if( m_cAutoRun == 2 ) {
			szAction = "Move_Front";
			vMoveVec.y = GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), LocalTime );
			bMovable = true;
		}
	}

	if( GetInterface().IsFocusEditBox() ) {
		if( IsMove() && IsMovable() && !IsAutoRun() && !IsFollowing() ) {
			CmdStop( "Stand" );
		}
		m_bApplyInputProcess = false;
	}
	if( m_bApplyInputProcess ) {
		if( IsMovable() && !IsHit() && !IsDown() ) {
			if( IsPushKey( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ) ) ) {
				if( GetState() == ActorStateEnum::Air ) {
					// 애니메이션 키까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
					if( 0 == GetCantXZMoveSEReferenceCount() )
					{
						MoveJumpZ( GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), LocalTime ) * GetMoveSpeed() * 2.f );
					}
				}
				else {	
					szAction = "Move_Front";
					vMoveVec.y = GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), LocalTime );
					bMovable = true;
				}
				ResetAutoRun();
			}
			if( IsPushKey( IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ) ) ) {
				if( GetState() == ActorStateEnum::Air ) {
					// 애니메이션 키까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
					if( 0 == GetCantXZMoveSEReferenceCount() )
					{
						MoveJumpZ( GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ), LocalTime ) * GetMoveSpeed() * -2.f );
					}
				}
				else {
					szAction = "Move_Back";
					vMoveVec.y = -GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ), LocalTime );
					bMovable = true;
				}
				ResetAutoRun();
			}
			if( IsPushKey( IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) ) ) {
				if( GetState() == ActorStateEnum::Air ) {
					// 애니메이션 키까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
					if( 0 == GetCantXZMoveSEReferenceCount() )
						MoveJumpX( GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ), LocalTime ) * GetMoveSpeed() * -2.f );
				}
				else {
					szAction = "Move_Left";
					vMoveVec.x = -GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ), LocalTime );
					bMovable = true;
				}
				ResetAutoRun();
			}
			if( IsPushKey( IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) ) ) {
				if( GetState() == ActorStateEnum::Air ) {
					// 애니메이션 키까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
					if( 0 == GetCantXZMoveSEReferenceCount() )
						MoveJumpX( GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ), LocalTime ) * GetMoveSpeed() * 2.f );
				}
				else {
					szAction = "Move_Right";
					vMoveVec.x = GetPushKeyDelta( IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ), LocalTime );
					bMovable = true;
				}
				ResetAutoRun();
			}
			if( !IsDie() && ( IsStay() || IsMove() ) && 
				( IsPushKey( IW( IW_TOGGLEBATTLE ) ) ) && 
				(GetPushKeyTime( IW( IW_TOGGLEBATTLE ) ) == LocalTime ) && !( GetAsyncKeyState( VK_MENU ) & 0x8000 ) ) 
			{
				ReleasePushKey( IW( IW_TOGGLEBATTLE ) );
				ReleaseJoypadButton( IW_PAD( IW_TOGGLEBATTLE ) );
	
				// #29900 관련 [2011/03/28 semozz]
				// 스탠스오브페이스 상태효과가 적용 되어 있을때는 전투모드 변경 불가
				if (!IsAppliedThisStateBlow(STATE_BLOW::BLOW_121))
				{
					if( IsCanBattleMode() )
					{
						if( IsMove() ) 
							CmdStop( "Stand" );

						bool bBattleMode = IsBattleMode();
						CmdToggleBattle( !bBattleMode );
						bMovable = false;

						if( !IsBattleMode() ) {
							WCHAR wszStr[128];
							swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 202 ), g_szKeyString[IW( IW_TOGGLEBATTLE )] );
							GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
						}
					}
					else {
						if(!m_hWeapon[0]) // 주무기를 장착하지 않아서 전투모드를 할수 없는경우에 메세지 표기
							CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100056 ), textcolor::YELLOW, 4.0f );
					}

					ResetAutoRun();
				}
			}
		}
		else {
			ResetPushKeyDelta( IW( IW_MOVEFRONT ), LocalTime );
			ResetPushKeyDelta( IW( IW_MOVEBACK ), LocalTime );
			ResetPushKeyDelta( IW( IW_MOVELEFT ), LocalTime );
			ResetPushKeyDelta( IW( IW_MOVERIGHT ), LocalTime );
		}

		//if( !CDnMouseCursor::GetInstance().IsShowCursor() ) {
		//	if( IsBattleMode() && OnSkillInput( LocalTime, fDelta ) ) {
		//		if( IsCustomAction() ) {
		//			ResetCustomAction();
		//			ResetMixedAnimation();
		//		}
		//		ResetAutoRun();
		//		return;
		//	}
		//}
//		if( m_cAutoRun == 2 && !( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 ) ) ResetAutoRun();

		ProcessSpecialCommand( LocalTime );

		if( szCurAction.empty() ) szAction = "Stand";
		else {
			if( IsMovable() && bMovable == false && m_szActionQueue.empty() && strstr( szCurAction.c_str(), "Move" ) != NULL )
				szAction = "Stand";
		}

		std::string szCompareAction = szAction;
		if(m_bShootMode)
			szCompareAction = GetChangeShootActionName(szAction.c_str());
		if( szCurAction.empty() || ( !szAction.empty() && strcmp( szCurAction.c_str(), szCompareAction.c_str() ) != NULL ) ) {
			if( !IsCustomAction() && !m_szMaintenanceBoneName.empty() && !m_szActionBoneName.empty() && !szCurAction.empty() ) {
				float fBlendFrame = 1.f;
				ActionElementStruct *pStruct = GetElement( m_nActionIndex );			
				if( pStruct ) fBlendFrame = (float)pStruct->dwBlendFrame;

				ResetMixedAnimation( false );
				if( !m_szActionBoneName.empty() && !m_szMaintenanceBoneName.empty() )
				{
					CmdMixedAction( m_szActionBoneName.c_str(), m_szMaintenanceBoneName.c_str(), szCurAction.c_str(), 0, CDnActionBase::m_fFrame, fBlendFrame );
						
					// mixed action 핵 테스트 코드
#ifdef HACKTEST_MIXEDACTION
					g_szActionBoneName = m_szActionBoneName;
					g_szMaintenanceBoneName = m_szMaintenanceBoneName;
#endif // #ifdef HACKTEST_MIXEDACTION
				}
				SetCustomAction( szCurAction.c_str(), CDnActionBase::m_fFrame );
			}
		}
	}

	if( bMovable ) {
		EtVec2Normalize( &vMoveVec, &vMoveVec );
		float fSpeed = (float)GetMoveSpeed();
		vMoveVec *= ( fDelta * fSpeed );
		CmdMove( vMoveVec, szAction.c_str(), -1, 6.f );

		if( GetInterface().GetMainBarDialog() ) {
			GetInterface().GetMainBarDialog()->ShowMinimapOption( false );
#ifdef PRE_ADD_AUTO_DICE
			GetInterface().GetMainBarDialog()->ShowAutoDiceOption( false );
#endif
		}
		/////////////////////////////////////// 위대하신 C9 님과 같이..
		if( CDnMouseCursor::IsActive() && CDnMouseCursor::GetInstance().IsShowCursor() ) {
			if( CDnMouseCursor::GetInstance().GetShowCursorRefCount() == 0 ) {
				CDnMouseCursor::GetInstance().ShowCursor( false, true );
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();
				focus::ReleaseControl();
			}
		}
		///////////////////////////////////////
		GetInterface().CloseDirectDialog();
	}
	else if( strcmp( szAction.c_str(), "Stand" ) == NULL && !m_bShootMode )
	{
//		if( !MAWalkMovement::m_bEnableNaviMode )
		if( EtVec3LengthSq( GetMovePos() ) == 0.f ) {
			if( strstr( m_szCustomAction.c_str(), "Shoot" ) ) {
				szAction = "Shoot_Stand";
				float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;
				ResetCustomAction();
				ResetMixedAnimation( false );
				CmdStop( szAction.c_str(), 0, 6.f, fFrame );
			}
			else {
				CmdStop( szAction.c_str(), 0, 6.f );
			}
		}
	}
	else if(strcmp( szAction.c_str(), "Stand" ) == NULL && m_bShootMode)
	{
		if( EtVec3LengthSq( GetMovePos() ) == 0.f ) {
			if( strstr( m_szCustomAction.c_str(), "MOD_Shoot" ) ) {
				szAction = "MOD_Shoot_Stand";
				float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;
				ResetCustomAction();
				ResetMixedAnimation( false );
				CmdStop( szAction.c_str(), 0, 6.f, fFrame );
			}
			else {
				CmdStop( szAction.c_str(), 0, 6.f );
			}
		}
	}

	if( bMovable || IsFollowing() ) {
		if( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHATROOM_DIALOG ) && GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHATROOM_DIALOG )->IsShow() )
			GetInterface().GetMainMenuDialog()->ShowChatRoomCreateDlg( false );
		GetInterface().ShowChatRoomPassWordDlg( false );
	}

	if(m_bShootMode)
		ChangeShootModeInputAction();
}

void CDnLocalPlayerActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
 	if(IsVehicleMode()) 
	{
		CDnPlayerActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
		return;
	}

	switch( Type ) {
		case STE_Input:
			{
				if( !m_bCheckInputSignal ) return;
				if( CGlobalInfo::GetInstance().IsPlayingCutScene() ) return;
				if( IsPushMouseButton(2) ) return;
				if( CDnMouseCursor::GetInstance().IsShowCursor() || IsLockInput() ) return;
				if( IsDie() && !IsGhost() ) return;
				if( IsFollowObserver() ) return;

				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				//if( !hCamera || hCamera->GetCameraType() != CDnCamera::PlayerCamera ) return;
				// 대포 모드일 때도 InputSignal 처리해준다.
				if( !hCamera || 
					(hCamera->GetCameraType() != CDnCamera::PlayerCamera && 
					 hCamera->GetCameraType() != CDnCamera::CannonCamera) )
					 return;

				InputStruct *pStruct = (InputStruct *)pPtr;

				if( ((CDnPlayerCamera*)hCamera.GetPointer())->IsFreeYaw() && pStruct->nButton < 3 ) return;

				bool isInverseMode = false;
				//입력 반전 효과가 적용 되어 있는 경우
				isInverseMode = CInputDevice::IsInverseKeyboard();

				bool bCheck = false;
				m_bProcessInputSignal = true;
				if( pStruct->nButton <= WM_BUTTON ) {
					bool bReleaseButton = false;

					//입력 반전 효과가 적용 되어 있는 경우
					int nMouseButton = pStruct->nButton;
					if (isInverseMode == true)
					{
						switch(nMouseButton)
						{
						case LM_BUTTON: nMouseButton = RM_BUTTON; break;
						case RM_BUTTON: nMouseButton = LM_BUTTON; break;
						}
					}

					switch( pStruct->nEventType ) {
						case 0:	// Push
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								LOCAL_TIME TimeTemp;
								switch( nMouseButton ) {
									case LM_BUTTON: TimeTemp = GetEventMouseTime( IR_MOUSE_LB_DOWN ); break;
									case RM_BUTTON: TimeTemp = GetEventMouseTime( IR_MOUSE_RB_DOWN ); break;
									case WM_BUTTON: TimeTemp = GetEventMouseTime( IR_MOUSE_WB_DOWN ); break;
								}
								// DirectInput 정확도가 떨어져서 여기서 좀 보정해준다.
								TimeTemp += (int)( m_fDelta * 1000 );
								if( TimeTemp >= SignalStartTime ) {
									bCheck = true;
									bReleaseButton = true;
								}
							}
							break;
						case 1:	// Charge
							bCheck = IsPushMouseButton( (BYTE)nMouseButton );
							if( bCheck ) {
								CInputReceiver::IR_STATE State;
								switch( nMouseButton ) {
									case LM_BUTTON: State = IR_MOUSE_LB_DOWN; break;
									case RM_BUTTON: State = IR_MOUSE_RB_DOWN; break;
									case WM_BUTTON: State = IR_MOUSE_WB_DOWN; break;
								}
								if( GetEventMouseTime( State ) < SignalStartTime )
									SetEventMouseTime( State, SignalStartTime );
							}
							break;
						case 2:	// Release
							if( !IsPushMouseButton( (BYTE)nMouseButton ) )
							{
								if( !IsPushMouseButton( (BYTE)nMouseButton ) )
								{
									int nState[2] = { 0, };
									switch( nMouseButton ) {
										case LM_BUTTON: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
										case RM_BUTTON: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
										case WM_BUTTON:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
									}
									LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
									LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );

									int nResult = (int)( ReleaseTime - PushTime );
									if( pStruct->nMinTime == -1 && pStruct->nMaxTime == -1 ) {
										bCheck = true;
										break;
									}
									if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) {
										SetEventMouseTime( (IR_STATE)nState[1], LocalTime );
										bCheck = true;
									}
								}
							}
							break;
						case 3:	// DoublePush
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								int nState[2] = { 0, };
								switch( nMouseButton ) {
									case LM_BUTTON: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
									case RM_BUTTON: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
									case WM_BUTTON:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
								}
								LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
								LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );
								int nResult = (int)( ReleaseTime - PushTime );
								if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) bCheck = true;
							}
							bReleaseButton = true;
							break;
						case 4: // Press
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								bCheck = true;
								/*
								LOCAL_TIME TimeTemp;
								switch( pStruct->nButton ) {
									case 0: TimeTemp = GetEventMouseTime( IR_MOUSE_LB_DOWN ); break;
									case 1: TimeTemp = GetEventMouseTime( IR_MOUSE_RB_DOWN ); break;
									case 2: TimeTemp = GetEventMouseTime( IR_MOUSE_WB_DOWN ); break;
								}

								if( TimeTemp >= CDnActionBase::m_ActionTime ) {
									bCheck = true;
//									bReleaseButton = true;
								}
								*/
							}
							break;
						case 5: // DoublePush or Shift+Push
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								int nState[2] = { 0, };
								switch( nMouseButton ) {
									case LM_BUTTON: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
									case RM_BUTTON: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
									case WM_BUTTON:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
								}
								LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
								LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );
								int nResult = (int)( ReleaseTime - PushTime );
								if( ( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) || IsPushKey( DIK_LSHIFT ) ) bCheck = true;
							}
							bReleaseButton = true;
							break;
					}
					if( bCheck ) {
						// 특정 스킬을 보유하고 있어야 하는 경우.
						if( 0 < pStruct->nNeedSkillIndex )
						{
							DnSkillHandle hSkill = FindSkill( pStruct->nNeedSkillIndex );
							if( !hSkill ) return;
						}

						m_bCheckInputSignal = false;
						if( IsCustomAction() ) {
							ResetCustomAction();
							ResetMixedAnimation();
						}
						if( pStruct->szActionBoneName && pStruct->szMaintenanceBoneName && strlen( pStruct->szActionBoneName ) && strlen( pStruct->szMaintenanceBoneName ) ) {
							if( pStruct->nEventType != 2 ) m_VecMixedAni.clear();
							// 아쳐 4연발을 위한..
							/*
							CmdMixedAction( pStruct->szActionBoneName, pStruct->szMaintenanceBoneName, pStruct->szChangeAction, (float)pStruct->nChangeActionFrame, (float)pStruct->nBlendFrame );
							SetCustomAction( pStruct->szChangeAction, (float)pStruct->nChangeActionFrame );
							*/
							std::string szAction = pStruct->szChangeAction;
							SetCustomAction( pStruct->szChangeAction, (float)pStruct->nChangeActionFrame );
							if( m_szCustomAction.empty() ) {
								
#if defined(PRE_FIX_68898)
								if (pStruct->isSkipEndAction == TRUE)
								{
									SetSkipEndAction(pStruct->isSkipEndAction == TRUE);
								}
#endif // PRE_FIX_68898

								CmdAction( m_szAction.c_str(), m_nLoopCount, m_fBlendFrame, true, false, ( pStruct->bSkillChain == TRUE ) );
							}
							else CmdMixedAction( pStruct->szActionBoneName, pStruct->szMaintenanceBoneName, pStruct->szChangeAction, 0, (float)pStruct->nChangeActionFrame, (float)pStruct->nBlendFrame );
						}
						else {

#if defined(PRE_FIX_68898)
							if (pStruct->isSkipEndAction == TRUE)
							{
								SetSkipEndAction(pStruct->isSkipEndAction == TRUE);
							}
#endif // PRE_FIX_68898
							CmdAction( pStruct->szChangeAction, 0, (float)pStruct->nBlendFrame, false, false, ( pStruct->bSkillChain == TRUE ) );
						}

						if( bReleaseButton ) 
						{
							ReleasePushButton( (BYTE)nMouseButton );
						}
						if( !IsMovable() ) {
							SetMovable( false );
							ResetMove();
						}
					}
				}
				else if( pStruct->nButton < 10 ) {
					int nButton = pStruct->nButton;
					/*
					if( nButton >= 3 && nButton <= 6 ) {
						EtVector3 vCamView = hCamera->GetMatEx()->m_vZAxis;
						EtVector3 vActorView = *GetLookDir();
						vCamView.y = 0.f;
						vActorView.y = 0.f;
						EtVec3Normalize( &vCamView, &vCamView );
						EtVec3Normalize( &vActorView, &vActorView );
						float fDot = EtVec3Dot( &vCamView, &vActorView );
						if( fDot <= 0.f ) {
							switch( nButton ) {
								case 3: nButton = 4; break;
								case 4: nButton = 3; break;
								case 5: nButton = 6; break;
								case 6: nButton = 5; break;
							}
						}
					}
					*/

					// 이동 불가일때는 방향키 먹지 않도록 처리한다.
					if( 0 < GetCantMoveSEReferenceCount() )
					{
						if( /*3*/Front_Key <= nButton && nButton <= Right_Key )
							return;
					}
					// 점프는 행동 불가일 때로 바꿈. 
					if( 0 < GetCantActionSEReferenceCount() )
					{
						if( Jump_Key == nButton )
							return;
					}

					int nMapButton;
					switch( nButton ) {
						case 3:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVEFRONT ) : IW( IW_MOVEBACK );	break;
						case 4:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVEBACK ) : IW( IW_MOVEFRONT );		break;
						case 5:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVELEFT ) : IW( IW_MOVERIGHT );		break;
						case 6:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVERIGHT ) : IW( IW_MOVELEFT );	break;
 						case 7: nMapButton = IW( IW_JUMP );			break;
						case 8: nMapButton = IW( IW_PICKUP );		break;
						case 9: nMapButton = IW( IW_REBIRTH );		break;
					}
					bool bReleaseButton = false;
					switch( pStruct->nEventType ) {
						case 0:	// Push
							if( GetPushKeyTime( nMapButton ) >= SignalStartTime ) 
							{
								if(IsObserver() && (nButton==8 || nButton ==7))
									break;
								// #17911 수정
								if( 8 == nButton ) {
									if( IsPushJoypadButton( IW_PAD(IW_PICKUP) ) && GetPushJoypadButtonTime( IW_PAD(IW_PICKUP) ) < SignalStartTime )
										break;
								}
	
								bCheck = IsPushKey( nMapButton );
								bReleaseButton = true;
							}

							break;

						case 1:	// Charge
							bCheck = IsPushKey( nMapButton );
							if( bCheck ) SetEventKeyTime( nMapButton, SignalStartTime );
							break;

						case 2:	// Release
							if( !IsPushKey( nMapButton ) ) {

								if( !IsPushKey( nMapButton ) )
								{
									LOCAL_TIME PushTime = m_LastPushDirKeyTime[ nButton - 3 ];
									LOCAL_TIME ReleaseTime = m_LastReleaseDirKeyTime[ nButton - 3 ];
									int nResult = (int)( ReleaseTime - PushTime );
									if( pStruct->nMinTime == -1 && pStruct->nMaxTime == -1 ) {
										bCheck = true;
										break;
									}
									if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) bCheck = true;
								}
							}
							break;

						case 3:	// DoublePush
							if( CheckSpecialCommand( LocalTime, nButton + 7 ) ) {
								bCheck = true;
								bReleaseButton = true;
							}
							break;

						case 4: // Press
							bCheck = IsPushKey( nMapButton );
							break;

						case 5: // DoublePush or Shift+Push
							if( CheckSpecialCommand( LocalTime, nButton + 7 ) || ( IsPushKey( DIK_LSHIFT ) && IsPushKey( nMapButton ) ) ) {
								bCheck = true;
								bReleaseButton = true;
							}
							break;
					}

					if( bCheck ) {
						// 특정 스킬을 보유하고 있어야 하는 경우.
						if( 0 < pStruct->nNeedSkillIndex )
						{
							DnSkillHandle hSkill = FindSkill( pStruct->nNeedSkillIndex );
							if( !hSkill ) return;
						}

						if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_224 ) )
						{
							DNVector(DnBlowHandle) vlhDisableActionSetBlow;
							m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_224, vlhDisableActionSetBlow );

							// 액션 셋 변경 상태효과는 여러개 있을 수 있다.
							int iNumBlow = (int)vlhDisableActionSetBlow.size();
							for( int i = 0; i < iNumBlow; ++i )
							{
								CDnDisableActionBlow* pDisableActionSetBlow = static_cast<CDnDisableActionBlow*>( vlhDisableActionSetBlow.at( i ).GetPointer() );
								if(pDisableActionSetBlow->IsMatchedAction(pStruct->szChangeAction))
									return;
							}
						}

						m_bCheckInputSignal = false;
						DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
						if( hCamera ) {
							EtVector3 vZVec;
							EtVec3Cross( &vZVec, &hCamera->GetMatEx()->m_vXAxis, &EtVector3( 0.f, 1.f, 0.f ) );

							Look( EtVec3toVec2( vZVec ) );
							RefreshMoveVector( hCamera );
						}

#if defined(PRE_FIX_68898)
						if (pStruct->isSkipEndAction == TRUE)
						{
							SetSkipEndAction(pStruct->isSkipEndAction == TRUE);
						}
#endif // PRE_FIX_68898

						CmdAction( pStruct->szChangeAction, 0, (float)pStruct->nBlendFrame, false, false, ( pStruct->bSkillChain == TRUE ) );

						if( nButton == 7 ) {
							ReleasePushKey( nMapButton );
							ReleaseJoypadButton( IW_PAD( IW_JUMP ) );
						}

						if( nButton == 9 )
							ReleaseJoypadButton( IW_PAD( IW_REBIRTH ) );

						if( !IsMovable() ) {
							SetMovable( false );
							ResetMove();
						}

						if( IsCustomAction() ) {
							ResetCustomAction();
							ResetMixedAnimation();
						}
					}
				}

				if( bCheck )
				{
					if( IsProcessSkill() ) 
					{
						// Note: 액션 패시브 스킬 사용 중에 일반 액션을 하게 된다면 체크해서 사용중인 스킬을 종료 시킨다.
						// 상태효과가 남아서 일반 공격에도 영향을 미치는 것을 막기 위해서.
						if( (m_hProcessSkill->GetSkillType() == CDnSkill::Passive || m_hProcessSkill->GetSkillType() == CDnSkill::AutoPassive) &&
							m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
						{
							m_hProcessSkill->OnEnd( LocalTime, 0.f );
							m_hProcessSkill.Identity();
						}
					}
				}
			}
			break;
		case STE_InputHasPassiveSkill:
			{
				if( !m_bCheckInputSignal ) return;
				if( CGlobalInfo::GetInstance().IsPlayingCutScene() ) return;
				if( IsPushMouseButton(2) ) return;
				if( CDnMouseCursor::GetInstance().IsShowCursor() || IsLockInput() ) return;
				if( !IsBattleMode() ) return;
				if( IsDie() ) return;
			
				// Note: 행동불가인 경우 패시브 스킬 나갈 수 없음.
				// SetAction() 함수에서 액션이 막히기 때문에 어차피 안나가지만 스킬 사용으로 쿨타임이 돌아가므로 시그널 처리를 막는다.
				if( GetCantActionSEReferenceCount() > 0 ) return;

				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera || hCamera->GetCameraType() != CDnCamera::PlayerCamera ) return;

				InputHasPassiveSkillStruct *pStruct = (InputHasPassiveSkillStruct *)pPtr;

 				DnSkillHandle hSkill = FindSkill( pStruct->nSkillIndex );
				if( !hSkill ) return;

				// Note 한기: 새로 추가된 SkillLevel 값이 셋팅하기 전엔 0이 될 수도 있어서 0인 경우엔 그냥 1로 판단함.
				if( 0 == pStruct->nSkillLevel )
					pStruct->nSkillLevel = 1;
				
				if( hSkill->GetLevel() < pStruct->nSkillLevel )
					return;

				// #45326 이 스킬에 대해 조건 체크 무시 플래그가 켜져 있다면 스킬 사용 조건 무시하도록 처리.
				if( TRUE == pStruct->bCanUseSkill )
					BeginCanUsePassiveSkill( SignalEndTime );

				if( CDnSkill::UsingResult::Success != hSkill->CanExecute() )
				{
					if( TRUE == pStruct->bCanUseSkill )
						EndCanUsePassiveSkill();

					return;
				}

				if( TRUE == pStruct->bCanUseSkill )
					EndCanUsePassiveSkill();

				// 방향키 누르고 있으면 사용하는 패시브 스킬.
				bool bChargeKey = false;

				bool isInverseMode = false;
				//입력 반전 효과가 적용 되어 있는 경우
				isInverseMode = CInputDevice::IsInverseKeyboard();

				if( pStruct->nButton < 3 ) {
					bool bCheck = false;
					bool bReleaseButton = false;

					//입력 반전 효과가 적용 되어 있는 경우
					int nMouseButton = pStruct->nButton;
					if (isInverseMode == true)
					{
						switch(nMouseButton)
						{
						case LM_BUTTON: nMouseButton = RM_BUTTON; break;
						case RM_BUTTON: nMouseButton = LM_BUTTON; break;
						}
					}

					switch( pStruct->nEventType ) {
						case 0:	// Push
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								LOCAL_TIME TimeTemp;
								switch( nMouseButton ) {
									case 0: TimeTemp = GetEventMouseTime( IR_MOUSE_LB_DOWN ); break;
									case 1: TimeTemp = GetEventMouseTime( IR_MOUSE_RB_DOWN ); break;
									case 2: TimeTemp = GetEventMouseTime( IR_MOUSE_WB_DOWN ); break;
								}
								if( TimeTemp >= SignalStartTime ) {
									bCheck = true;
									bReleaseButton = true;
								}
								else
								{
									//#66175 시그널 시간 간격이 짧아서 정확히 입력이 힘듬..
									//0.3초 정도면 봐준다?
									if ( (SignalStartTime - TimeTemp) <= 300 )
									{
										bCheck = true;
										bReleaseButton = true;
									}
								}
							}
							break;
						case 1:	// Charge
							bCheck = IsPushMouseButton( (BYTE)nMouseButton );
							bChargeKey = true;
							break;
						case 2:	// Release
							if( !IsPushMouseButton( (BYTE)nMouseButton ) )
							{
								if( !IsPushMouseButton( (BYTE)nMouseButton ) ) {
									int nState[2] = { 0, };
									switch( nMouseButton ) {
										case 0: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
										case 1: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
										case 2:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
									}
									LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
									LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );

									if( pStruct->nMinTime == -1 && pStruct->nMaxTime == -1 ) {
										bCheck = true;
										break;
									}
									int nResult = (int)( ReleaseTime - PushTime );
									if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) {
										SetEventMouseTime( (IR_STATE)nState[1], LocalTime );
										bCheck = true;
									}
								}
							}
							break;
						case 3:	// DoublePush
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								int nState[2] = { 0, };
								switch( nMouseButton ) {
									case 0: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
									case 1: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
									case 2:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
								}
								LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
								LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );
								int nResult = (int)( ReleaseTime - PushTime );
								if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) bCheck = true;
							}
							bReleaseButton = true;
							break;
						case 4: // Press
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								/*
								LOCAL_TIME TimeTemp;
								switch( pStruct->nButton ) {
									case 0: TimeTemp = GetEventMouseTime( IR_MOUSE_LB_DOWN ); break;
									case 1: TimeTemp = GetEventMouseTime( IR_MOUSE_RB_DOWN ); break;
									case 2: TimeTemp = GetEventMouseTime( IR_MOUSE_WB_DOWN ); break;
								}

								if( TimeTemp >= CDnActionBase::m_ActionTime ) {
									bCheck = true;
//									bReleaseButton = true;
								}
								*/
								bCheck = true;
							}
							break;
						case 5:	// DoublePush or Shift+Push
							if( IsPushMouseButton( (BYTE)nMouseButton ) ) {
								int nState[2] = { 0, };
								switch( nMouseButton ) {
									case 0: nState[0] = IR_MOUSE_LB_UP; nState[1] = IR_MOUSE_LB_DOWN;  break;
									case 1: nState[0] = IR_MOUSE_RB_UP; nState[1] = IR_MOUSE_RB_DOWN;  break;
									case 2:	nState[0] = IR_MOUSE_WB_UP; nState[1] = IR_MOUSE_WB_DOWN;  break;
								}
								LOCAL_TIME PushTime = GetEventMouseTime( (IR_STATE)nState[1] );
								LOCAL_TIME ReleaseTime = GetEventMouseTime( (IR_STATE)nState[0] );
								int nResult = (int)( ReleaseTime - PushTime );
								if( ( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) || IsPushKey( DIK_LSHIFT ) ) bCheck = true;
							}
							bReleaseButton = true;
							break;
					}
					if( bCheck ) {
						m_bCheckInputSignal = false;
						if( IsCustomAction() ) {
							ResetCustomAction();
							ResetMixedAnimation();
						}
						
						const char* pPassiveSkillActionName = pStruct->szChangeAction;
						// 액션 패시브 스킬이 강화스킬 획득으로 인해 강화 되었을 경우 따로 지정된 액션으로 셋팅한다.
						if( hSkill->IsEnchantedSkill() )
							pPassiveSkillActionName = pStruct->szEXSkillChangeAction;

						ActionElementStruct* pActionElement = GetElement( pPassiveSkillActionName );
						if( pActionElement )
						{
							if( IsProcessSkill() )
							{
								//if( !hSkill->CanExecute() ) return;
								m_hProcessSkill->OnEnd( LocalTime, 0.f );
								m_hProcessSkill.Identity();
							}

							if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_121))
							{
								DNVector( DnBlowHandle ) vlhAllAppliedBlows;
								GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_121 , vlhAllAppliedBlows );

								for( int i = 0; i < (int)vlhAllAppliedBlows.size(); ++i )
								{
									if( vlhAllAppliedBlows[i]->IsBegin() == true )
										return; // 스탠드 체인지형 스킬의 구조적인 문제 커밋 로그 확인해주세요
								}
							}

							// 패시브 스킬 액션에 next 액션까지 있는 경우 감안.
							// next 액션은 클라이언트쪽에서도 하나만 기준 잡고 있기 때문에 서버에서도 하나만 기준 잡는다.
							DWORD dwActionLength = 0;
							if( pActionElement )
								dwActionLength = pActionElement->dwLength - pStruct->nChangeActionFrame;

							// #25042 Stand 액션이 next 액션인 경우 패시브 스킬의 액션이 아니므로 시간을 포함시키지 않는다.
							if( pActionElement->szNextActionName != "Stand" &&
								0 == strstr( pActionElement->szNextActionName.c_str(), "_Landing") )
							{
								ActionElementStruct* pNextElement = GetElement( pActionElement->szNextActionName.c_str() );
								if( pNextElement )
									dwActionLength += pNextElement->dwLength;
							}

							hSkill->SetPassiveSkillLength( (float)dwActionLength / s_fDefaultFps );
							m_bInputHasPassiveSignalNotCheckPushKey = (pStruct->bNotCheckPushKey == TRUE) ? true : false;

							if( TRUE == pStruct->bCanUseSkill )
								BeginCanUsePassiveSkill( SignalEndTime );

							CmdPassiveSkillAction( hSkill->GetClassID(), pPassiveSkillActionName, 0, (float)pStruct->nBlendFrame, (float)pStruct->nChangeActionFrame, 
												   false, false, ( pStruct->bOnlyCheck ) ? true : false );

							if( TRUE == pStruct->bCanUseSkill )
								EndCanUsePassiveSkill();
						}
						else
						{
							_ASSERT( !"패시브 스킬 사용 실패!" );
							OutputDebug( "[패시브 스킬 사용 실패] %s 존재하지 않는 액션입니다\n", pStruct->szChangeAction );
						}

						if( bReleaseButton ) ReleasePushButton( (BYTE)nMouseButton );
						if( !IsMovable() ) {
							SetMovable( false );
							ResetMove();
						}
					}
				}
				else if( pStruct->nButton < 10 ) {
					int nButton = pStruct->nButton;
					/*
					if( nButton >= 3 && nButton <= 6 ) {
						EtVector3 vCamView = hCamera->GetMatEx()->m_vZAxis;
						EtVector3 vActorView = *GetLookDir();
						vCamView.y = 0.f;
						vActorView.y = 0.f;
						EtVec3Normalize( &vCamView, &vCamView );
						EtVec3Normalize( &vActorView, &vActorView );
						float fDot = EtVec3Dot( &vCamView, &vActorView );
						if( fDot <= 0.f ) {
							switch( nButton ) {
								case 3: nButton = 4; break;
								case 4: nButton = 3; break;
								case 5: nButton = 6; break;
								case 6: nButton = 5; break;
							}
						}
					}
					*/

					int nMapButton;
					switch( nButton ) {
						case 3:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVEFRONT ) : IW( IW_MOVEBACK );	break;
						case 4:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVEBACK ) : IW( IW_MOVEFRONT );		break;
						case 5:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVELEFT ) : IW( IW_MOVERIGHT );		break;
						case 6:	nMapButton = (isInverseMode == false) ?  IW( IW_MOVERIGHT ) : IW( IW_MOVELEFT );	break;
						case 7: nMapButton = IW( IW_JUMP );			break;
						case 8: nMapButton = IW( IW_PICKUP );		break;
						case 9: nMapButton = IW( IW_REBIRTH );		break;
					}

					bool bCheck = false;
					bool bReleaseButton = false;
					switch( pStruct->nEventType ) {
						case 0:	// Push
							if( GetPushKeyTime( nMapButton ) >= SignalStartTime ) {
								bCheck = IsPushKey( nMapButton );
								bReleaseButton = true;
							}
							break;
						case 1:	// Charge
							bCheck = IsPushKey( nMapButton );
							bChargeKey = true;
							break;
						case 2:	// Release
							if( !IsPushKey( nMapButton ) ) {
								int nArrayIndex =  nButton - 3;
								if( !(nArrayIndex >= 0 && nArrayIndex < 4) )
									break;

								LOCAL_TIME PushTime = m_LastPushDirKeyTime[ nArrayIndex ];
								LOCAL_TIME ReleaseTime = m_LastReleaseDirKeyTime[ nArrayIndex ];
								int nResult = (int)( ReleaseTime - PushTime );
								if( pStruct->nMinTime == -1 && pStruct->nMaxTime == -1 ) {
									bCheck = true;
									break;
								}
								if( nResult >= pStruct->nMinTime && (DWORD)nResult <= (DWORD)pStruct->nMaxTime ) 
									bCheck = true;
							}
							break;
						case 3:	// DoublePush
							if( CheckSpecialCommand( LocalTime, nButton + 7 ) ) {
								bCheck = true;
								bReleaseButton = true;
							}
							break;
						case 4:	// Press
							bCheck = IsPushKey( nMapButton );
							/*
							if( GetPushKeyTime( nMapButton ) >= CDnActionBase::m_ActionTime ) {
								bCheck = IsPushKey( nMapButton );
								bReleaseButton = true;
							}
							*/
							break;
						case 5: // DoublePush or Shift+Push
							if( CheckSpecialCommand( LocalTime, nButton + 7 ) || ( IsPushKey( DIK_LSHIFT ) && IsPushKey( nMapButton ) ) ) {
								bCheck = true;
								bReleaseButton = true;
							}
							break;
					}

					if( bCheck ) 
					{
						m_bCheckInputSignal = false;
						if( IsProcessSkill() )
						{
							//if( !hSkill->CanExecute() ) return;
							m_hProcessSkill->OnEnd( LocalTime, 0.f );
							m_hProcessSkill.Identity();
						}

						if( bChargeKey )
						{
							hSkill->SetPassiveSkillLength( -1.0f );
						}
						else
						{
							// 패시브 스킬 액션에 next 액션까지 있는 경우 감안.
							// next 액션은 클라이언트쪽에서도 하나만 기준 잡고 있기 때문에 서버에서도 하나만 기준 잡는다.
							DWORD dwActionLength = 0;
							ActionElementStruct* pElement = GetElement( pStruct->szChangeAction );
							if( pElement )
								dwActionLength = pElement->dwLength;

							// #25042 Stand 액션이 next 액션인 경우 패시브 스킬의 액션이 아니므로 시간을 포함시키지 않는다.
							if( pElement && pElement->szNextActionName != "Stand" &&
								0 == strstr( pElement->szNextActionName.c_str(), "_Landing") )
							{
								ActionElementStruct* pNextElement = GetElement( pElement->szNextActionName.c_str() );
								if( pNextElement )
									dwActionLength += pNextElement->dwLength;
							}

							hSkill->SetPassiveSkillLength( (float)dwActionLength / s_fDefaultFps );
						}
						
						// Note: 한기 - bChargeKey 서버쪽에도 대시처럼 누르고 있는 키로 발동된 패시브 스킬인지 보내줌.
						// 액션 바뀌기 전엔 스킬이 계속 실행되는 것으로 처리.
						DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
						if( hCamera ) {
							EtVector3 vZVec;
							EtVec3Cross( &vZVec, &hCamera->GetMatEx()->m_vXAxis, &EtVector3( 0.f, 1.f, 0.f ) );

							Look( EtVec3toVec2( vZVec ) );
							RefreshMoveVector( hCamera );
						}
						
						m_bInputHasPassiveSignalNotCheckPushKey = (pStruct->bNotCheckPushKey == TRUE) ? true : false;

						if( TRUE == pStruct->bCanUseSkill )
							BeginCanUsePassiveSkill( SignalEndTime );

						CmdPassiveSkillAction( hSkill->GetClassID(), pStruct->szChangeAction, 0, (float)pStruct->nBlendFrame, 
											   (float)pStruct->nChangeActionFrame, bChargeKey, false, ( pStruct->bOnlyCheck ) ? true : false );

						if( TRUE == pStruct->bCanUseSkill )
							EndCanUsePassiveSkill();

						if( nButton == 7 ) {
							ReleasePushKey( nMapButton );
							ReleaseJoypadButton( IW_PAD( IW_JUMP ) );
						}
						if( !IsMovable() ) {
							SetMovable( false );
							ResetMove();
						}

						if( IsCustomAction() ) {
							ResetCustomAction();
							ResetMixedAnimation();
						}
					}
				}
			}
			break;

		case STE_CanMove:
			{
//				if( IsCustomProcessSignal() ) return;

				if( CDnActorState::Cant_Move == (m_StateEffect & CDnActorState::Cant_Move) )
					return;

				//if( IsProcessSkill() ) return;
				CanMoveStruct *pStruct = (CanMoveStruct *)pPtr;

				SetMovable( ( pStruct->bCanMove == TRUE ) ? true : false );
				if( pStruct->bCanMove == TRUE ) {
					if( pStruct->szMaintenanceBoneName ) {
						m_szMaintenanceBoneName = pStruct->szMaintenanceBoneName; 
					}
					else m_szMaintenanceBoneName.clear();
					if( pStruct->szActionBoneName ) {
						m_szActionBoneName = pStruct->szActionBoneName;
					}
					else m_szActionBoneName.clear();
				}
			}
			return;
		case STE_CameraEffect_RadialBlur:
			{
				CameraEffect_RadialBlurStruct *pStruct = (CameraEffect_RadialBlurStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				float fLength = (float)( SignalEndTime - SignalStartTime );
				if( IsModifyPlaySpeed() ) {
					float fTemp = CDnActionBase::m_fFps / 60.f;
					fLength *= fTemp;
				}
				// 16.6666
				DWORD dwTime = (DWORD)( fLength - 16.6666f );
				pStruct->nCameraEffectRefIndex = hCamera->RadialBlur( dwTime, *pStruct->vCenter, pStruct->fBlurSize, pStruct->fBeginRatio, pStruct->fEndRatio );
			}
			break;
		case STE_FreezeCamera:
			{
				FreezeCameraStruct *pStruct = (FreezeCameraStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( hCamera )
				{
					switch( hCamera->GetCameraType() )
					{
						case CDnCamera::PlayerCamera:
							{
								CDnPlayerCamera *pCamera = static_cast<CDnPlayerCamera *>(hCamera.GetPointer());
								if( pCamera && pCamera->GetAttachActor() == GetMySmartPtr() ) {
									pCamera->SetFreeze( pStruct->fResistanceRatio, 1 );
									m_bIgnoreRefreshViewCamera = ( pStruct->bIgnoreRefreshPlayerView == TRUE ) ? true : false;
								}
							}
							break;

						case CDnCamera::CannonCamera:
							{
								hCamera->SetFreeze( pStruct->fResistanceRatio, 1 );
							}
							break;
					}
				}
			}
			break;

		case STE_CanUseSkill:
			{
				CanUseSkillStruct* pStruct = (CanUseSkillStruct*)pPtr;
				bool bUseSignalSkillCheck = (pStruct->bUseSignalSkillCheck ? TRUE : FALSE);

				switch( pStruct->CheckType )
				{
					// Movable
					case 0:
						CDnMovableChecker::SetUseSignalSkillCheck( bUseSignalSkillCheck, SignalEndTime );
						break;

					// Jumpable
					case 1:
						CDnJumpableChecker::SetUseSignalSkillCheck( bUseSignalSkillCheck, SignalEndTime );
						break;

					// GroundMovableChecker
					case 2:
						CDnGroundMovableChecker::SetUseSignalSkillCheck( bUseSignalSkillCheck, SignalEndTime );
						break;
				}

				m_bUseSignalSkillCheck = bUseSignalSkillCheck;
			}
			break;
		case STE_PickupItem:
			{
				PickupItemStruct* pStruct = reinterpret_cast<PickupItemStruct*>(pPtr);

				DNVector(DnDropItemHandle) hVecList;
				int nCount = CDnDropItem::ScanItem( m_matexWorld.m_vPosition, (float)pStruct->nRange, hVecList );
				float fMinDist = FLT_MAX;
				DnDropItemHandle hResult;
				for( int i=0; i<nCount; i++ ) 
				{
					float fValue = EtVec3LengthSq( &( m_matexWorld.m_vPosition - *hVecList[i]->GetPosition() ) );
					if( fMinDist > fValue ) 
					{
						fMinDist = fValue;
						hResult = hVecList[i];
					}
				}

				if( !hResult )
				{
					return;
				}

				// Packet
				DWORD dwUniqueID = hResult->GetUniqueID();

				BYTE pBuffer[128];
				CPacketCompressStream Stream( pBuffer, 128 );

				Stream.Write( &dwUniqueID, sizeof(DWORD) );
				Stream.Write( &nSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Write( &m_matexWorld.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
//				Stream.Write( &pStruct->nRange, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

				Send( eActor::SC_CMDPICKUPITEM, &Stream );

				return;
			}
		case STE_SyncChangeAction:
			{
				SyncChangeActionStruct *pStruct = (SyncChangeActionStruct *)pPtr;
				if( _rand()%100 < pStruct->nRandom ) {
					if( CheckSyncChangeActionCondition( pStruct->nConditionOne, pStruct->nOperatorOne, pStruct->szValueOne ) )
						CmdAction( pStruct->szActionName );
				}
			}
			break;

		case STE_CannonTargeting:
			{
				CannonTargetingStruct* pStruct = (CannonTargetingStruct*)pPtr;
				if( IsCannonMode() )
				{
					// 빠르게 마우스를 조작할 때 최신 마우스 위치로 카메라가 업데이트 되기 전에 시그널 처리가 먼저되어
					// 패킷을 보내는 경우가 있어 여기서 플래그를 켜고 추후에 RotateSync() 함수에서 처리한다.
					m_bNeedSendCannonTargeting = true;

					//// 현재 대포가 가리키고 있는 방향을 기준으로 서버로 TargetPosition 을 계산해서 보내준다. 
					//// 이것에 대해서는 플레이어가 쏘는 액션 시작하는 순간 카메라 이동 입력을 잠궈서 대포 이동 못하게 하고
					//// 서버로 패킷을 쏴주는 시그널을 만들어서 처리하도록 한다.

					//// 서버로 보내기 전에 대포의 타겟을 최종적으로 확정.
					//CDnCannonMonsterActor* pCannonMonsterActor = static_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer());
					//pCannonMonsterActor->GetCannonGroundHitPosition( m_hCannonCamera, m_vNowCannonGroundHitPos );
					//
					//// 우선은 발사체 시그널에 있는 방향을 그대로 사용..
					//DWORD dwCannonMonsterActorID = m_hCannonMonsterActor->GetUniqueID();
					//BYTE pBuffer[ 64 ] = { 0 };
					//CPacketCompressStream Stream( pBuffer, 64 );
					//Stream.Write( &dwCannonMonsterActorID, sizeof(DWORD) );
					//Stream.Write( &(m_hCannonCamera->GetMatEx()->m_vZAxis), sizeof(EtVector3) );				// 현재 카메라가 바라보고 있는 월드축 기준 방향.
					//Stream.Write( &static_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer())->GetNowShootDir(), sizeof(EtVector3) );	// 대포 기준 로컬좌표계의 발사 방향.
					//Stream.Write( &m_vNowCannonGroundHitPos, sizeof(EtVector3) );

					//Send( eActor::CS_CANNONTARGETING, &Stream );

				}
			}
			break;

		case STE_ShootCannon:
			{
				// 몬스터가 사용한 스킬은 패킷으로 따로 서버로부터 온다.
				//ShootCannonStruct* pStruct = (ShootCannonStruct*)pPtr;
				//if( IsCannonMode() )
				//{
				//	m_hCannonMonsterActor->UseSkill( pStruct->CannonMonsterSkillID );
				//}
			}
			break;
	}

	CDnPlayerActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnLocalPlayerActor::CmdMove( EtVector2 &vDir, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MAMovementBase *pMovement = GetMovement();
	if( !pMovement ) return;
	pMovement->MoveX( vDir.x );
	pMovement->MoveZ( vDir.y );

	bool bAllow = IsAllowMovement();
	SetActionQueue( szActionName, nLoopCount, fBlendFrame );
	if( bAllow ) return;

	
	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	int	nActionIndex = GetElementIndex( szActionName );
	DWORD dwGap	= GetSyncDatumGap();
	int nMoveSpeed = CDnActor::GetMoveSpeed();

	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );
	Stream.Write( &nMoveSpeed, sizeof(int) );

	Send( eActor::CS_CMDMOVE, &Stream );
}

void CDnLocalPlayerActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	CDnPlayerActor::CmdMove( vPos, szActionName, nLoopCount, fBlendFrame );

	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	int		nActionIndex	= GetElementIndex( szActionName );
	DWORD	dwGap			= GetSyncDatumGap();

	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( GetMovePos(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

	Send( eActor::CS_CMDMOVEPOS, &Stream );
	OutputDebug( "CmdMovePos : %d\n", CDnActionBase::m_LocalTime );
}

void CDnLocalPlayerActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce )
{
	/*
	if( strstr( m_szCustomAction.c_str(), "Shoot" ) ) {
		float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;
		CDnPlayerActor::CmdStop( "Shoot_Stand", 0, 6.f, fFrame );
		ResetCustomAction();
		ResetMixedAnimation();
	}
	else CDnPlayerActor::CmdStop( szActionName, nLoopCount, fBlendFrame, fStartFrame );
	*/
	CDnPlayerActor::CmdStop( szActionName, nLoopCount, fBlendFrame, fStartFrame, bResetStop, bForce );

	if( IsProcessSkill() == false )
		bForce = true; 
	// 게임서버는 클라이언트와 프로세스 프레임이 다르기때문에 클라이언트에서는 스킬이 끝났지만
	// 게임서버는 끝나지 않은 상황이 생긴다 그래서 클라이언트 기준으로 스킬이 끝났으면 bForce 값으로 Stop을 보낸다.
	// 이렇게 하지않으면 동기가 미묘하게 틀려지는 상황에서 클라이언트는 정지된 상태인데 서버는 움직이는 상황이 발생한다.

	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	DWORD dwGap = GetSyncDatumGap();

	Stream.Write( &dwGap, sizeof(DWORD) );
	EtVector3 vPos = EtVec3LengthSq(&m_WarpPos) > 0 ? m_WarpPos : *GetPosition();
	Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &bResetStop, sizeof(bool) );
	Stream.Write( &bForce, sizeof(bool) );

	Send( eActor::CS_CMDSTOP, &Stream );
	m_cMovePushKeyFlag = 0;
	//if( !IsBattleMode() ) m_cAutoRun = 0;
//	OutputDebug( "CmdStop\n" );
}

void CDnLocalPlayerActor::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame, bool bCheckOverlapAction, 
									 bool bFromStateBlow/* = false*/, bool bSkillChain/* = false*/ )
{
	CDnPlayerActor::CmdAction( szActionName, nLoopCount, fBlendFrame, bCheckOverlapAction, bFromStateBlow, bSkillChain );

	DWORD dwGap = GetSyncDatumGap();
	m_cMovePushKeyFlag = 0;

	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK) ) || m_cAutoRun == 2 ) m_cMovePushKeyFlag |= 0x04;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT) ) ) m_cMovePushKeyFlag |= 0x08;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT) ) ) m_cMovePushKeyFlag |= 0x01;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT) ) ) m_cMovePushKeyFlag |= 0x02;

	// 아쳐 4연발을 위한..
	int nActionIndex = ( m_szActionQueue.empty() ) ? GetElementIndex( szActionName ) : GetElementIndex( m_szActionQueue.c_str() );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );
	Stream.Write( &bFromStateBlow, sizeof(bFromStateBlow) );
	Stream.Write( &bSkillChain, sizeof(bSkillChain) );

	Send( eActor::CS_CMDACTION, &Stream );

	m_bResetMoveMsg = true;
	m_vPrevMoveVector = EtVec3toVec2( *GetMoveVectorZ() );
	m_LastSendMoveMsg = CDnActionBase::m_LocalTime;
//	OutputDebug( "CmdAction\n" );

	// Note: 라이징 슬래시 처럼 입력에 따라 여러 액션이 나뉘어 나가는 경우 스킬의 
	// 상태효과가 지속되어야 하므로 스킬 쪽에 알려주도록 한다.
	// 서버로도 같이 SkillChain 플래그를 같이 보낸다.
	if( m_hProcessSkill )
	{
		if( m_hProcessSkill->GetSkillType() == CDnSkill::Active &&
			m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
		{
			if( bSkillChain )
			{
				m_hProcessSkill->OnChainInput( szActionName );
			}
		}
	}
}

void CDnLocalPlayerActor::CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount, float fFrame, float fBlendFrame )
{
	CDnPlayerActor::CmdMixedAction( szActionBone, szMaintenanceBone, szActionName, nLoopCount, fFrame, fBlendFrame );

	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	m_cMovePushKeyFlag = 0;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK) ) ) m_cMovePushKeyFlag |= 0x04;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT) ) ) m_cMovePushKeyFlag |= 0x08;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT) ) ) m_cMovePushKeyFlag |= 0x01;
	if( IsPushKey( IW((isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT) ) ) m_cMovePushKeyFlag |= 0x02;

	int nActionIndex = GetElementIndex( szActionName );
	int nMaintenanceBoneIndex = GetBoneIndex( szMaintenanceBone );
	int nActionBoneIndex = GetBoneIndex( szActionBone );

	int nBlendAniIndex = m_nAniIndex;
	if( !m_VecMixedAni.empty() ) nBlendAniIndex = m_VecMixedAni[0].nBlendAniIndex;

	if( nActionIndex == -1 ) {
		assert(0);
	}
	if( nActionBoneIndex == -1 || nMaintenanceBoneIndex == -1 ) return;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	DWORD dwGap = GetSyncDatumGap();

	Stream.Write( &dwGap, sizeof(DWORD) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nActionBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &nMaintenanceBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );

	Send( eActor::CS_CMDMIXEDACTION, &Stream );

	m_bResetMoveMsg = true;
//	OutputDebug( "CmdMixedAction\n" );
}

void CDnLocalPlayerActor::CmdLook( EtVector2 &vVec, bool bForce )
{
	CDnPlayerActor::CmdLook( vVec, bForce );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &bForce, sizeof(bool) );

	Send( eActor::CS_CMDLOOK, &Stream );
//	OutputDebug( "CmdLook\n" );
}

bool CDnLocalPlayerActor::IsCanBattleMode()
{ 

	bool bCanBattleMode = false;
	bCanBattleMode = CDnPlayerActor::IsCanBattleMode();
	
	if(!m_hWeapon[0])
		bCanBattleMode = false;

	return bCanBattleMode;
}

bool CDnLocalPlayerActor::IsCanToggleBattleMode()
{
#if defined(PRE_ADD_50907)
	//무기 해제 상태효과로 전투 모드 변경이 될 수 있음.
	if (GetDisarmamentRefCount() > 0 && IsSkipChangeWeaponAction() == true) return true;
#endif // PRE_ADD_50907

	if( m_hProcessSkill ) return false;
	
	return true;
}

void CDnLocalPlayerActor::CmdToggleBattle( bool bBattleMode )
{
	if( bBattleMode == m_bBattleMode ) return;
		
	if( !IsCanToggleBattleMode() ) return;
	CDnPlayerActor::CmdToggleBattle( bBattleMode );
	GetInterface().ToggleButtonBattleMode( bBattleMode );
#ifdef PRE_FIX_CHARSTATUS_REFRESH
	GetInterface().OnRefreshLocalPlayerStatus();
#endif
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		((CDnPlayerCamera*)hCamera.GetPointer())->LockFreeYaw( false );
		((CDnPlayerCamera*)hCamera.GetPointer())->SetSmoothAnglePower( bBattleMode ? 20.0f : 10.0f );
		((CDnPlayerCamera*)hCamera.GetPointer())->SetSmoothZoomPower( bBattleMode ? 20.0f : 10.0f );
	}

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &m_bBattleMode, sizeof(bool) );

	Send( eActor::CS_CMDTOGGLEBATTLE, &Stream );
}

void CDnLocalPlayerActor::CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, 
												 bool bChargeKey, bool bCheckOverlapAction, bool bOnlyCheck/* = false*/ )
{
	// 핵 테스트
	//if( nSkillID == 3015 && (strcmp(szActionName, "Attack_SideKick") == 0) )
	//{
	//	CmdAction( "Attack4_Wand" );
	//	CmdPassiveSkillAction( nSkillID, "Skill_RelicOfMiracle_End_1" );
	//	return;
	//}
	CDnPlayerActor::CmdPassiveSkillAction( nSkillID, szActionName, nLoopCount, fBlendFrame, fStartFrame, bChargeKey, bCheckOverlapAction, bOnlyCheck );
	m_cMovePushKeyFlag = 0;

	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	if( false == m_bInputHasPassiveSignalNotCheckPushKey )
	{
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK) ) ) m_cMovePushKeyFlag |= 0x04;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT) ) ) m_cMovePushKeyFlag |= 0x08;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT) ) ) m_cMovePushKeyFlag |= 0x01;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT) ) ) m_cMovePushKeyFlag |= 0x02;
	}
	else
	{
		// 한번 시그널에서 저장되었던 변수는 다시 초기화 시켜준다.
		m_bInputHasPassiveSignalNotCheckPushKey = false;
	}

	int nActionIndex = GetElementIndex( szActionName );
	DnSkillHandle hSkill = FindSkill( nSkillID );
	BYTE cLevel = 1;
	if( hSkill ) cLevel = (BYTE)hSkill->GetLevel();

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nSkillID, sizeof(int) );
	Stream.Write( &cLevel, sizeof(char) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
	Stream.Write( &fStartFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.0f );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &m_cMovePushKeyFlag, sizeof(char) );
	Stream.Write( &bChargeKey, sizeof(bool) );
	Stream.Write( &bOnlyCheck, sizeof(bool) );

	Send( eActor::CS_CMDPASSIVESKILLACTION, &Stream );

	m_bResetMoveMsg = true;
	m_vPrevMoveVector = EtVec3toVec2( *GetMoveVectorZ() );
	m_LastSendMoveMsg = CDnActionBase::m_LocalTime;

	if( hSkill->HasCoolTime()  ) {
		CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
		if( pMainDlg ) {
			pMainDlg->AddPassiveSkill( hSkill );
		}
	}

//	OutputDebug( "CmdPassiveSkillAction\n" );
}


void CDnLocalPlayerActor::CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bSendServer )
{
	if( false == bSendServer )
		CDnActor::CmdRemoveStateEffect( emBlowIndex, bSendServer );
	else
	{
		BYTE pBuffer[32];
		CPacketCompressStream Stream( pBuffer, 32 );
		Stream.Write( &emBlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );

		Send( eActor::CS_CMDREMOVESTATEEFFECT, &Stream );
//		OutputDebug( "CmdRemoveStateEffect\n" );
	}
}

#if defined(PRE_FIX_57706)
void CDnLocalPlayerActor::CmdRemoveStateEffectByServerBlowID( int nServerBlowID )
{
	BYTE pBuffer[32];
	CPacketCompressStream Stream( pBuffer, 32 );
	Stream.Write( &nServerBlowID, sizeof(int) );

	Send( eActor::CS_CMDREMOVESTATEEFFECTFROMID, &Stream );
}
#endif // PRE_FIX_57706


void CDnLocalPlayerActor::CmdWarp( EtVector3 &vPos, EtVector2 &vLook )
{
	m_nWarpLazyCount = 2; // 한프레임 뒤에 호출되기 위함.	
	m_WarpPos = vPos;
	m_WarpLook = vLook;
	m_LastSendMoveMsg = CDnActionBase::m_LocalTime;
	if( CDnWorld::GetInstance().GetEnvironment() ) {
		//blondy 크래쉬남...
		if( CDnWorld::GetInstance().GetEnvironment()->GetTransitionFilter() )
			CDnWorld::GetInstance().GetEnvironment()->GetTransitionFilter()->Capture();
		//blondy end
	}
}

void CDnLocalPlayerActor::CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash )
{
	bool bPrevShow = m_bWeaponViewOrder[nEquipIndex];
	CDnPlayerActor::CmdToggleWeaponViewOrder( nEquipIndex, bShowCash );
	if( bPrevShow == m_bWeaponViewOrder[nEquipIndex] ) return;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &m_bWeaponViewOrder[nEquipIndex], sizeof(bool) );

	Send( eActor::CS_CMDTOGGLEWEAPONORDER, &Stream );
}

void CDnLocalPlayerActor::CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash )
{
	bool bPrevShow = m_bPartsViewOrder[nEquipIndex];
	CDnPlayerActor::CmdTogglePartsViewOrder( nEquipIndex, bShowCash );
	if( bPrevShow == m_bPartsViewOrder[nEquipIndex] ) return;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &m_bPartsViewOrder[nEquipIndex], sizeof(bool) );

	Send( eActor::CS_CMDTOGGLEPARTSORDER, &Stream );
}

void CDnLocalPlayerActor::CmdToggleHideHelmet( bool bHideHelmet )
{
	bool bPrevHide = m_bHideHelmet;
	CDnPlayerActor::CmdToggleHideHelmet( bHideHelmet );
	if( bPrevHide == m_bHideHelmet ) return;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nIndex = HIDEHELMET_BITINDEX;
	Stream.Write( &nIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &m_bHideHelmet, sizeof(bool) );

	Send( eActor::CS_CMDTOGGLEPARTSORDER, &Stream );
}

bool CDnLocalPlayerActor::CheckAttributeEscape()
{
	if( EtVec3LengthSq( &m_vLastValidPos ) == 0.f ) return false;
	if( !CDnWorld::IsActive() ) return false;
	NavigationMesh *pNavMesh = CDnWorld::GetInstance().GetNavMesh( m_matexWorld.m_vPosition );
	if( !pNavMesh ) return false;

	NavigationCell *pCell = pNavMesh->FindCell( m_matexWorld.m_vPosition );
	if( pCell ) return false;

	bool bClash = true;
	char cAttr2 = CDnWorld::GetInstance().GetAttribute( GetMatEx()->m_vPosition );
	if( ( cAttr2 & 0x0f ) == 1 || ( cAttr2 & 0x0f ) == 2 ) { // 0x01, 0x02 는 충돌체크
		char cHiAttr = ( cAttr2 & 0xf0 ) >> 4;
		if( cHiAttr != 0 ) {		// 대각선 블럭인 경우.
			if( !CheckDiagonalBlock( GetMatEx()->m_vPosition.x, GetMatEx()->m_vPosition.z ) ) { // 대각선 블럭 내부인경우
				bClash = false;
			}
		}
		else {				// 대각선 블럭이 아닌 경우.
			bClash = false;
		}
	}
	if( bClash ) return false;

	/*
	SetActionQueue( "Stand" );
	m_matExWorld.m_vPosition = m_vPrevPos = m_vStaticPos = m_vLastValidPos;

	CmdWarp( m_matExWorld.m_vPosition, EtVec3toVec2( *GetMoveVectorZ() ) );
	*/

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &m_vLastValidPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

	Send( eActor::CS_CMDESCAPE, &Stream );
	return true;
}

bool CDnLocalPlayerActor::CheckCollisionEscape()
{
	if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) {
#ifndef _WORK
		if( m_LastEscapeTime > 0 && CDnActionBase::m_LocalTime - m_LastEscapeTime < 1000 * 60 * 2 ) return false; // 쿨은 2분
#endif
		m_LastEscapeTime = CDnActionBase::m_LocalTime;

		Send( eActor::CS_CMDESCAPE, NULL );
	}
	else {
#ifndef _WORK
		if( m_LastEscapeTime > 0 && CDnActionBase::m_LocalTime - m_LastEscapeTime < 1000 * 60 * 1 ) return false; // 쿨은 1분
#endif
		if( EtVec3LengthSq( &m_vLastValidPos ) == 0.f ) return false;
		if( !IsExpectCollisionEscape() ) return false;
		m_LastEscapeTime = CDnActionBase::m_LocalTime;

		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		Stream.Write( &m_vLastValidPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

		Send( eActor::CS_CMDESCAPE, &Stream );
	}
	return true;
}

bool CDnLocalPlayerActor::IsExpectCollisionEscape()
{
	if( m_vVecPosHistory.size() < 10 ) return false;

	float fLength = 0.f;
	for( DWORD i=1; i<m_vVecPosHistory.size(); i++ ) {
		fLength += EtVec3Length( &EtVector3( m_vVecPosHistory[0] - m_vVecPosHistory[i] ) );
	}
	fLength /= (float)m_vVecPosHistory.size();
	
	if( fLength > 100.f ) return false;

	return true;
}

void CDnLocalPlayerActor::UpdatePositioinHistory( float fDelta )
{
	m_fPosHistoryDelta -= fDelta;
	if( m_fPosHistoryDelta <= 0.f ) m_fPosHistoryDelta = 1.f;

	m_vVecPosHistory.push_back( m_matexWorld.m_vPosition );
	if( m_vVecPosHistory.size() > 10 ) m_vVecPosHistory.erase( m_vVecPosHistory.begin() );
}

void CDnLocalPlayerActor::CmdEscape()
{
#if defined(PRE_FIX_48724)
	const char* szCurrentAction = GetCurrentAction();
	if (szCurrentAction && strstr(szCurrentAction, "Stun") != NULL)
		return;
#endif // PRE_FIX_48724

	// 속성에 꼇을경우
	if( CheckAttributeEscape() ) return;

	// 마을일경우
	CheckCollisionEscape();
}

bool CDnLocalPlayerActor::IsAllowMovement()
{
	CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
	if( pTask ) {
		if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg >= s_nVillageMaximumMoveSendTime ) m_bResetMoveMsg = true;
	}

	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	EtVector2 vCurZVec( GetMoveVectorZ()->x, GetMoveVectorZ()->z );
	if( m_bResetMoveMsg == false ) {
		if( ( IsPushKey( IW((isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT) )	)	== ( (m_cMovePushKeyFlag & 0x01) == 0x01 ) &&
			( IsPushKey( IW((isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT) ) )	== ( (m_cMovePushKeyFlag & 0x02) == 0x02 ) &&
			( ( IsPushKey( IW((isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK)	)	== ( (m_cMovePushKeyFlag & 0x04) == 0x04 ) || ( m_cAutoRun == 2 ) == ( (m_cMovePushKeyFlag & 0x04) == 0x04 ) ) ) &&
			( IsPushKey( IW((isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT) )	)	== ( (m_cMovePushKeyFlag & 0x08) == 0x08 ) &&
			GetState() == ActorStateEnum::Move && EtVec2Dot( &vCurZVec, &m_vPrevMoveVector ) >= REFRESH_VIEWVEC_ANGLE ) return true;
		if( m_LastSendMoveMsg == 0 || CDnActionBase::m_LocalTime - m_LastSendMoveMsg < REFRESH_VIEWVEC_TICK ) {
			if( m_LastSendMoveMsg == 0 ) m_LastSendMoveMsg = ( CDnActionBase::m_LocalTime == 0 ) ? 1 : CDnActionBase::m_LocalTime;
			return true;
		}
	}
	m_bResetMoveMsg = false;

	m_vPrevMoveVector = vCurZVec;

	m_cMovePushKeyFlag = 0;
	if( m_bApplyInputProcess ) {
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT) ) ) m_cMovePushKeyFlag |= 0x08;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK) ) || m_cAutoRun == 2 ) m_cMovePushKeyFlag |= 0x04;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT) ) ) m_cMovePushKeyFlag |= 0x01;
		if( IsPushKey( IW((isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT) ) ) m_cMovePushKeyFlag |= 0x02;
	}

	m_LastSendMoveMsg = ( CDnActionBase::m_LocalTime == 0 ) ? 1 : CDnActionBase::m_LocalTime;
	m_nPressedCount = 0;

	return false;
}

void CDnLocalPlayerActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	if( !IsBattleMode() && IsCanBattleMode() ) CmdToggleBattle( true );
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		hCamera->SetFreeze( true );
		CDnPlayerCamera *pCamera = static_cast<CDnPlayerCamera *>(hCamera.GetPointer());
		pCamera->LockFreeYaw( false );
	}

	CDnPlayerActor::OnDamage( pHitter, HitParam );
	m_bResetMoveMsg = true;

	// 프리카메라일경우 맞아도 키가 릴리즈되지 않도록 합니다. 불편해서..
	bool bReleaseMouseButton = true;
	if( hCamera && hCamera->GetCameraType() == CDnCamera::FreeCamera ) bReleaseMouseButton = false;

	if( bReleaseMouseButton && !m_HitParam.szActionName.empty() ) {
		ReleasePushButton(0);
		ReleasePushButton(1);
	}

	UpdateAttackedCPPoint( pHitter, m_HitParam.HitType );

	if( m_pDamageReaction && pHitter && pHitter->GetActorHandle() && m_HitParam.nDamage != 0 && m_HitParam.bRecvOnDamage ) 
	{
		m_pDamageReaction->SetHitterHandle( pHitter->GetActorHandle(), m_HitParam.DistanceType );
	}
}

void CDnLocalPlayerActor::GuildWarAlarm()
{
	if( m_pDamageReaction ) 
		m_pDamageReaction->GuildWarAlarm();
}

void CDnLocalPlayerActor::OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor )
{
	if( hActor->GetTeam() != GetTeam() )
		GetInterface().ShowEnemyGauge( hActor, false );

	if( hActor ) {
		bool bResult;
		switch( hActor->GetHitParam()->HitType ) {
			case CDnWeapon::Critical:	bResult = UpdateCriticalHit(); break;
			case CDnWeapon::Stun:		bResult = UpdateStunHit(); break;
		}

		if( hActor->IsDie() ) {
			UpdateKillMonster();
		}
	}
}

void CDnLocalPlayerActor::ProcessSpecialCommand( LOCAL_TIME LocalTime )
{
	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	for( DWORD i=0; i<GetEventKeyCount(); i++ ) {
		BYTE cKey = GetEventKey(i);
		if( cKey == IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ) && IsPushKey( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ) ) ) {
			m_LastPushDirKeyTime[0] = LocalTime;
			m_cLastPushDirKey = IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK );
			m_cLastPushDirKeyIndex = 0;

//			m_LastPushDirKeyTime[1] = m_LastPushDirKeyTime[2] = m_LastPushDirKeyTime[3] = 0;
		}
		else if( cKey == IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ) ) {
			m_LastReleaseDirKeyTime[0] = LocalTime;
			m_cLastReleaseDirKey = IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK );
			m_LastPrevPushDirKeyTime[0] = m_LastPushDirKeyTime[0];
		}

		if( cKey == IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ) && IsPushKey( IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ) ) ) {
			m_LastPushDirKeyTime[1] = LocalTime;
			m_cLastPushDirKey = IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT );
			m_cLastPushDirKeyIndex = 1;

//			m_LastPushDirKeyTime[0] = m_LastPushDirKeyTime[2] = m_LastPushDirKeyTime[3] = 0;
		}
		else if( cKey == IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ) ) {
			m_LastReleaseDirKeyTime[1] = LocalTime;
			m_cLastReleaseDirKey = IW( (isInverseMode == false) ? IW_MOVEBACK :IW_MOVEFRONT );
			m_LastPrevPushDirKeyTime[1] = m_LastPushDirKeyTime[1];
		}

		if( cKey == IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) && IsPushKey( IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) ) ) {
			m_LastPushDirKeyTime[2] = LocalTime;
			m_cLastPushDirKey = IW( (isInverseMode == false) ? IW_MOVELEFT :IW_MOVERIGHT );
			m_cLastPushDirKeyIndex = 2;

//			m_LastPushDirKeyTime[0] = m_LastPushDirKeyTime[1] = m_LastPushDirKeyTime[3] = 0;
		}
		else if( cKey == IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) ) {
			m_LastReleaseDirKeyTime[2] = LocalTime;
			m_cLastReleaseDirKey = IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT );
			m_LastPrevPushDirKeyTime[2] = m_LastPushDirKeyTime[2];
		}

		if( cKey == IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) && IsPushKey( IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) ) ) {
			m_LastPushDirKeyTime[3] = LocalTime;
			m_cLastPushDirKey = IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT );
			m_cLastPushDirKeyIndex = 3;

//			m_LastPushDirKeyTime[0] = m_LastPushDirKeyTime[1] = m_LastPushDirKeyTime[2] = 0;
		}
		else if( cKey == IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) ) {
			m_LastReleaseDirKeyTime[3] = LocalTime;
			m_cLastReleaseDirKey = IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT );
			m_LastPrevPushDirKeyTime[3] = m_LastPushDirKeyTime[3];
		}
	}
}

bool CDnLocalPlayerActor::CheckSpecialCommand( LOCAL_TIME LocalTime, char cCommandIndex )
{
	bool isInverseMode = false;
	//입력 반전 효과가 적용 되어 있는 경우
	isInverseMode = CInputDevice::IsInverseKeyboard();

	char s_cReleaseKeyList[4][3] = {
		{ IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ), IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ), IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) },
		{ IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ), IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) },
		{ IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ), IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) },
		{ IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ), IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ), IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) },
	};
	if( m_cLastPushDirKeyIndex + 10 == cCommandIndex && m_cLastPushDirKey == m_cLastReleaseDirKey && 
		IsPushKey( m_cLastPushDirKey ) && !IsPushKey( s_cReleaseKeyList[m_cLastPushDirKeyIndex][0] ) && !IsPushKey( s_cReleaseKeyList[m_cLastPushDirKeyIndex][1] ) && !IsPushKey( s_cReleaseKeyList[m_cLastPushDirKeyIndex][2] ) &&
		LocalTime - m_LastPrevPushDirKeyTime[m_cLastPushDirKeyIndex] < s_nDoubleCommandDelay ) {
			char cFlag = m_cLastPushDirKeyIndex + 10;
			m_cLastPushDirKeyIndex = -1;
			m_cLastReleaseDirKey = -1;
			return true;
	}

	if( IsPushKey( m_cLastPushDirKey ) && LocalTime - m_LastPushDirKeyTime[m_cLastPushDirKeyIndex] < s_nSpecialCommandDelay ) {
		if( IsPushMouseButton(0) ) {
			if( m_cLastPushDirKeyIndex == cCommandIndex && GetEventMouseTime( (isInverseMode == false) ? IR_MOUSE_LB_DOWN : IR_MOUSE_RB_DOWN ) >= m_LastPushDirKeyTime[m_cLastPushDirKeyIndex] ) {
				char cFlag = m_cLastPushDirKeyIndex;
				m_cLastPushDirKeyIndex = -1;
				return true;
			}
		}
		if( IsPushMouseButton(1) ) {
			if( m_cLastPushDirKeyIndex + 4 == cCommandIndex && GetEventMouseTime( (isInverseMode == false) ? IR_MOUSE_RB_DOWN : IR_MOUSE_LB_DOWN ) >= m_LastPushDirKeyTime[m_cLastPushDirKeyIndex] ) {
				char cFlag = m_cLastPushDirKeyIndex + 4;
				m_cLastPushDirKeyIndex = -1;
				return true;
			}
		}
	}
	if( IsPushMouseButton(1) ) {
		m_cLastPushDirKeyIndex = -1;
		if( !IsPushKey( IW( (isInverseMode == false) ? IW_MOVEFRONT : IW_MOVEBACK ) ) && !IsPushKey( IW( (isInverseMode == false) ? IW_MOVEBACK : IW_MOVEFRONT ) ) && !IsPushKey( IW( (isInverseMode == false) ? IW_MOVELEFT : IW_MOVERIGHT ) ) && !IsPushKey( IW( (isInverseMode == false) ? IW_MOVERIGHT : IW_MOVELEFT ) ) ) {
			if( cCommandIndex == 8 ) return true;
		}
		else {
			if( cCommandIndex == 9 ) return true;
		}
	}
	return false;
}



// #15557 이슈 관련. 몬스터가 발사체에서 발사체를 쏘는 경우.
// 서버쪽의 CDnMonsterActor::SendProjectileFromProjectile() 함수와 클라 이 함수의 내용이 같다.
// 서버에서 몹이 발사체에서 발사체를 쏘는 경우 해당 정보를 파티원들이 쏘는 발사체와 동일하게 처리한다.
#ifdef PRE_MOD_PROJECTILE_HACK
void CDnLocalPlayerActor::OnProjectile( CDnProjectile *pProjectile )
{
	CPacketCompressStream* pPacketStream = pProjectile->GetPacketStream();
	_ASSERT( pPacketStream );
	if( pPacketStream )
		Send( eActor::CS_PROJECTILE, pPacketStream );
}
#else
void CDnLocalPlayerActor::OnProjectile( CDnProjectile *pProjectile, int nSignalIndex )
{
	pProjectile->SetSignalArrayIndex( nSignalIndex );
	CPacketCompressStream* pPacketStream = pProjectile->GetPacketStream();
	_ASSERT( pPacketStream );
	if( pPacketStream )
		Send( eActor::CS_PROJECTILE, pPacketStream );
}
#endif

bool CDnLocalPlayerActor::ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, 
										bool bCheckValid /*= true*/, bool bAutoUsedFromServer /*= false*/, bool bSendPacketToServer /*= true */ )
{
	_ASSERT( hSkill && "CDnLocalPlayerActor::OnSkillExecute() -> Invalid hSkill" );

	if( CDnPlayerActor::ExecuteSkill( hSkill, LocalTime, fDelta, bCheckValid, bAutoUsedFromServer, bSendPacketToServer ) == false ) 
		return false;

#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bShowSkillDPS )
	{
		CGlobalValue::GetInstance().m_bShowDPS = true;
		CGlobalValue::GetInstance().m_nSumDPS = 0; // 누적데미지
		CGlobalValue::GetInstance().m_fDPSTime = 0.f;
	}
#endif

	UpdateUseSkill( m_hProcessSkill );
	bool bUseVilage = ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) ? true : false;
	// 아이템 스킬은 패킷을 보내지 않는다.
	// 아이템 사용 패킷이 날아가기 때문에 서버에서도 아이템에 스킬이 붙어있으면 알아서 발동된다..
	// siva : 마을의 경우는 동기를 마춰야하기때문에 Return 하면안됩니다.
	if( !bUseVilage && hSkill->IsItemSkill() ) {
		return true;	
	}

	DnActorHandle hResultActor;
	bool bCheckTarget = CalcCrosshairOffset( NULL, &m_Crosshair, NULL, &hResultActor );
	if( bCheckTarget == false ) {
		if( hSkill->GetTargetType() == CDnSkill::Friend &&
			hSkill->GetDurationType() == CDnSkill::Buff ) {
			bCheckTarget = true;
			hResultActor = GetMySmartPtr();
		}
	}
	if( bCheckTarget ) {
		m_hSkillTarget = hResultActor;
		m_vSkillTargetPos = *hResultActor->GetPosition();
		m_vSkillTargetPos.y += hResultActor->GetHeight() / 2.f;
	}
	else {
		m_hSkillTarget.Identity();
		m_vSkillTargetPos = m_Crosshair.m_vPosition;
	}

	// CmdPassiveSkillAction 함수로 부터 호출된 경우 패시브던 액티브이던 타입에 관계없이 스킬 실행.
	// 서버로 패시브 스킬 사용 패킷이 따로 가므로 패시브 스킬 사용 시 여기서 보내지 않는다.
	// 액티브 스킬을 InputHasPassiveSkill 시그널에서 사용할 경우 서버쪽에서 사용중이던 스킬을 중지시키고 
	// 패킷 받은 스킬을 실행하는데 UsableChecker 때문에 실패할 수 있다.
	if( false == bSendPacketToServer )
		return true;
	else	// 이하는 원래 있던 서버로 보내는 코드.
	if( hSkill->GetSkillType() == CDnSkill::Passive ) 
	{
		if( m_hProcessSkill->GetNumChecker() == 0 || m_hProcessSkill->GetNumProcessor() == 0 )
			return true;
	}

	m_bResetMoveMsg = true;
	ReleasePushButton(0);
	ReleasePushButton(1);
	ResetCustomAction();
	ResetMixedAnimation();
	ResetAutoRun();
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
	DnCameraHandle hPlayerCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
	if( hPlayerCamera )
	{
		CDnPlayerCamera* pPlayerCamera = static_cast<CDnPlayerCamera*>(hPlayerCamera.GetPointer());
		if( CDnCamera::GetActiveCamera() == hPlayerCamera )
			pPlayerCamera->SyncActorView();
	}
#endif

	// 서버에서 자동실행된 패시브 스킬은 클라가 패킷을 보내면 안된다.
	if( false == bAutoUsedFromServer )
	{
		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		int nSkillID = hSkill->GetClassID();
		int nEnchatSkillID = hSkill->GetEnchantedSkillID();
		BYTE cLevel = (BYTE)hSkill->GetLevel();
		bool bUseApplySkillItem = false;		// ApplySkill 아이템인 경우 아이템 사용으로 스킬 패킷을 쏴준다. 여기선 그냥 스킬이므로 false
		bool bAutoUseFromServer = false;

		// 스킬 사용 가능 상태 만들어주는 시그널의 상태를 서버로 그대로 보냄.
		bool abUseSkillSignalCheck[ 3 ] = { false };
		abUseSkillSignalCheck[ 0 ] = CDnMovableChecker::GetUseSignalSkillCheck();
		abUseSkillSignalCheck[ 1 ] = CDnJumpableChecker::GetUseSignalSkillCheck();
		abUseSkillSignalCheck[ 2 ] = CDnGroundMovableChecker::GetUseSignalSkillCheck();

		Stream.Write( &nSkillID, sizeof(int) );
		Stream.Write( &cLevel, sizeof(char) );
		Stream.Write( &bUseApplySkillItem, sizeof(bool) );
		Stream.Write( abUseSkillSignalCheck, sizeof(abUseSkillSignalCheck) );
		Stream.Write( &EtVec3toVec2( *GetMoveVectorZ() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
		Stream.Write( &EtVec3toVec2( *GetLookDir() ), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
		Stream.Write( &bAutoUseFromServer, sizeof(bool) );
		Stream.Write( &nEnchatSkillID , sizeof(int) );
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
		Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
#endif

		Send( eActor::CS_USESKILL, &Stream );
	}
	else
	{
		// 서버에서 자동으로 실행된 스킬은 반드시 오토 패시브여야만 말이 되고, 
		// 오토 패시브 스킬 사용한 것 쿨타임을 UI 로 표시해준다.
		_ASSERT( CDnSkill::AutoPassive == hSkill->GetSkillType() );
		if( hSkill->HasCoolTime()  )
		{
			CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
			if( pMainDlg )
			{
				pMainDlg->AddPassiveSkill( hSkill );
			}
		}
	}

	return true;
}

void CDnLocalPlayerActor::ProcessCrosshair()
{
	bool bForceCrossHair = false; // 특정모드일때 강제로 크로스헤어를 설정하는 경우

	if( IsCannonMode() )
	{
		// 대포 모드이고 현재 대포 카메라가 활성화 되었을 때만 대포 크로스헤어를 표시해준다.
		if( CDnCamera::GetActiveCamera()->GetCameraType() == CDnCamera::CannonCamera )
			bForceCrossHair = true;
	}

	if( CDnCamera::GetActiveCamera() ) 
	{
		if( CDnCamera::GetActiveCamera()->GetCameraType() != CDnCamera::PlayerCamera && !bForceCrossHair) 
		{
			m_LastAimTarget.TargetType = CrossHairType::Normal;
			m_LastAimTarget.Reset();
			return;
		}
	}

#ifdef PRE_TEST_ANIMATION_UI
	CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
	if (pMainDlg && pMainDlg->IsShowAniTestDlg())
		return;
#endif

	// Cross Draw
	if( ( !IsDie() ) && ( m_hTargetCross[0]->IsReady() ) )
	{
		int nCrossHairIndex = CrossHairType::Normal;

		MatrixEx Cross;
		DnActorHandle hResultTarget;
		DnActorHandle hResultActor;
		DnPropHandle hResultProp;

		float fResultLengthTarget = 9999999.0f;
		bool bCheckTarget = CalcCrosshairOffset( NULL, &m_Crosshair, &fResultLengthTarget, &hResultTarget );

		float fResultLengthActor = 9999999.0f;
		int nResultTargetActorType;
		bool bCheckTargetActor = CalcCrosshairOffsetActor( 150.0f, NULL, &m_Crosshair, &fResultLengthActor, &hResultActor, &nResultTargetActorType );

		float fResultLengthProp = 9999999.0f;
		int nResultTargetPropType;
		bool bCheckTargetProp = CalcCrosshairOffsetProp( 150.f, NULL, &m_Crosshair, &fResultLengthProp, &hResultProp, &nResultTargetPropType );

		if( !s_bShowCrosshair ) return;
		// 적이든 npc 든 안걸렷다. 노말 크로스헤어
		if ( (!bCheckTarget && !bCheckTargetActor && !bCheckTargetProp) || bForceCrossHair)
		{
			nCrossHairIndex = CrossHairType::Normal;

			if(IsCannonMode())
				nCrossHairIndex = CrossHairType::CannonTarget;

			m_LastAimTarget.Reset();
		}
		else
		{

#ifndef _FINAL_BUILD
			// 몬스터 액션 보기 ( #46533 [스냅샷개선] 몬스터 액션 확인용 치트키 )
			if( CGlobalValue::GetInstance().m_bSetMonster )
			{
				DnActorHandle hCurrentMonster = GetLastAimActor();
				if(hCurrentMonster)
				{
					CGlobalValue::GetInstance().m_dwSetMonsterID = hCurrentMonster->GetUniqueID();
				}
				else
				{
					CGlobalValue::GetInstance().m_bShowMonsterAction = false;
					CGlobalValue::GetInstance().m_dwSetMonsterID = -1;
				}

				CGlobalValue::GetInstance().m_bSetMonster = false;
			}
#endif

			float fMin = min( min( fResultLengthTarget, fResultLengthActor ), fResultLengthProp );
			if( bCheckTarget && fMin == fResultLengthTarget ) {
				if( hResultTarget && hResultTarget->IsShowExposureInfo() ) {

					// 대포 타입 액터이고, 일정 거리(2m) 이상 가깝고, 대포가 바라보고 있는 방향과 90도 이하의 각이라면,
					// 액터타입이 대포형이라면 대포모드용 크로스 헤어를 보여준다.
					float fDistanceSQ = EtVec3LengthSq( &EtVector3(*hResultTarget->GetPosition() - m_matexWorld.m_vPosition) );
					if( fDistanceSQ <= 40000.0f &&
						hResultTarget->GetActorType() == CDnActorState::Cannon )
					{
	
						float fDot = EtVec3Dot( &EtVector3(CDnCamera::GetActiveCamera()->GetMatEx()->m_vZAxis.x,0.f,CDnCamera::GetActiveCamera()->GetMatEx()->m_vZAxis.z), 
							&EtVector3(hResultTarget->GetMatEx()->m_vZAxis.x,0.f,hResultTarget->GetMatEx()->m_vZAxis.z) );
						float fDegree = EtToDegree( EtAcos(fDot) );
						
						// Rotha 대포를 앞에서 집는다면 이동 갭이 커지기때문에 , 대포는 후면에서만 클릭이 가능하도록 설정합니다.
						// 카메라 각도와 대포의 각도를 조사한뒤 , 카메라각도 앵글이 대포가 바라보는 앵글 좌우 75도를 넘지 않을때만 클릭가능하도록 설정합니다.
						// 즉 카메라가 바라보는 시점과 대포가 바라보는 시점이 비슷해져야하고 그 비슷한 경우는 대포의 뒤에 있을때만 가능하게 됩니다.
								
						if(fDegree < 60.f) 
						{
							// 대포 몬스터라면 대포 크로스 헤어로 바꿔줌.
							nCrossHairIndex = CrossHairType::CannonPick;
							m_LastAimTarget.hActor = hResultTarget;
						}
					}
					else
					// 투명 상태효과를 갖고 있다면 크로스헤어 반응 없음.
					if( false == hResultTarget->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_073 )
#ifdef PRE_FIX_GUIDEDARROW_ON_PVPTOURNAMENT
						&& false == hResultTarget->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_230 )
#endif
						)
					{
						nCrossHairIndex = CrossHairType::Enemy;
						m_LastAimTarget.hActor = hResultTarget;
						GetInterface().ShowEnemyGauge( GetLastAimActor(), true );
					}
				}
			}
			else if( bCheckTargetActor && fMin == fResultLengthActor ) 
			{
				if( !( hResultActor && hResultActor->IsNpcActor() && !hResultActor->IsShow() ) )
				{
					nCrossHairIndex = nResultTargetActorType;
					m_LastAimTarget.hActor = hResultActor;
				}
			}
			else if( bCheckTargetProp && fMin == fResultLengthProp ) 
			{
				switch( nResultTargetPropType ) {
					case CrossHairType::Npc:
						{
							CDnWorldNpcProp *pNpcProp = dynamic_cast<CDnWorldNpcProp *>(hResultProp.GetPointer());
							if( pNpcProp && pNpcProp->IsShow() )
								m_LastAimTarget.hActor = pNpcProp->GetActorHandle();
							else
								nResultTargetPropType = CrossHairType::Normal;
						}
						break;
					case CrossHairType::Seed:
						{
							m_eSeedState = GetLifeSkillPlantTask().GetPropState( hResultProp );

							if( Farm::AreaState::GROWING == m_eSeedState || Farm::AreaState::COMPLETED == m_eSeedState )
							{
								DnPropHandle hProp = GetLastAimProp();
								if( hProp )
								{
									CDnLifeSkillPlantTask::SPlantTooltip sInfo;
									sInfo = GetLifeSkillPlantTask().PlantInfo( hProp );
									if( !CDnMouseCursor::GetInstance().IsShowCursor() ) 
									{
										GetInterface().SetFarmTooltip( sInfo );
										GetInterface().GetLifeTooltipDlg()->CenterTooltip();
									}

									if( Farm::AreaState::GROWING == m_eSeedState )
									{
#if defined( PRE_REMOVE_FARM_WATER )
										nResultTargetPropType = CrossHairType::NotHarvest;
#else
										if( CDnLifeSkillPlantTask::eWater_Enable == GetLifeSkillPlantTask().UseWater(hProp) )
											nResultTargetPropType = CrossHairType::Water;
										else
											nResultTargetPropType = CrossHairType::NotWater;
#endif	// PRE_REMOVE_FARM_WATER
									}
									else if( Farm::AreaState::COMPLETED == m_eSeedState )
									{
										if( CDnLifeSkillPlantTask::eHarvest_Enable == GetLifeSkillPlantTask().UseHarvest(hProp) )
											nResultTargetPropType = CrossHairType::Harvest;
										else
											nResultTargetPropType = CrossHairType::NotHarvest;
									}
								}
							}

							m_LastAimTarget.hProp = hResultProp;
						}
						break;

					case CrossHairType::FlagTarget :
						{
							if( CDnOccupationTask::IsActive() )
							{
								if( GetOccupationTask().IsFlagOperation( hResultProp ) )
									m_LastAimTarget.hProp = hResultProp;
								else
									nResultTargetPropType = CrossHairType::Normal;
							}
						}
						break;
					default:
						m_LastAimTarget.hProp = hResultProp;
						break;
				}
				nCrossHairIndex = nResultTargetPropType;
			}
		}


		if( CDnMouseCursor::GetInstance().IsShowCursor() ) 
			nCrossHairIndex = CrossHairType::CursorMode;


		DrawCrosshair( nCrossHairIndex );

		/*
		m_LastAimTarget.TargetType = nCrossHairIndex;
		EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_hTargetCross[ nCrossHairIndex ]->GetTexturePtr() );
		*/
	}
}

void CDnLocalPlayerActor::DrawCrosshair( int nCrossHairIndex ) 
{
	EtVector4 Vertices[ 4 ];
	EtVector2 TexCoord[ 4 ];
	int nWidth, nHeight;
	int nTexWidth, nTexHeight;

	nWidth = CEtDevice::GetInstance().Width();
	nHeight = CEtDevice::GetInstance().Height();
	nTexWidth = m_hTargetCross[nCrossHairIndex]->Width();
	nTexHeight = m_hTargetCross[nCrossHairIndex]->Height();

	Vertices[0] = EtVector4( nWidth * 0.5f - ( nTexWidth / 2 ), nHeight * s_fCrossHairHeightRatio - ( nTexHeight / 2 ), 0.0f, 0.0f );
	Vertices[1] = EtVector4( nWidth * 0.5f + ( nTexWidth / 2 ), nHeight * s_fCrossHairHeightRatio - ( nTexHeight / 2 ), 0.0f, 0.0f );
	Vertices[2] = EtVector4( nWidth * 0.5f + ( nTexWidth / 2 ), nHeight * s_fCrossHairHeightRatio + ( nTexHeight / 2 ), 0.0f, 0.0f );
	Vertices[3] = EtVector4( nWidth * 0.5f - ( nTexWidth / 2 ), nHeight * s_fCrossHairHeightRatio + ( nTexHeight / 2 ), 0.0f, 0.0f );
	TexCoord[ 0 ] = EtVector2( 0.0f, 0.0f );
	TexCoord[ 1 ] = EtVector2( 1.0f, 0.0f );
	TexCoord[ 2 ] = EtVector2( 1.0f, 1.0f );
	TexCoord[ 3 ] = EtVector2( 0.0f, 1.0f );

	if( nCrossHairIndex == CrossHairType::Npc && GetLastAimActor()  ) 
	{
		if( GetLastAimActor()->GetActorType() != CDnActor::Npc )
		{
			nCrossHairIndex = CrossHairType::Normal;
		}
		else if( CanTalkNpc( GetLastAimActor() ) != (char)true )
		{
			nCrossHairIndex = CrossHairType::NotTalk;
		}
	}

	if( CDnMouseCursor::GetInstance().GetShowCursorRefCount() && CDnMouseCursor::GetInstance().IsShowCursor() ) 
		m_LastAimTarget.TargetType = CrossHairType::CursorMode;
	else {
		m_LastAimTarget.TargetType = nCrossHairIndex;
		if( EtInterface::g_bEtUIRender )
			EternityEngine::DrawQuad2DWithTex( Vertices, TexCoord, m_hTargetCross[ nCrossHairIndex ]->GetTexturePtr() );
	}
}

char CDnLocalPlayerActor::CanTalkNpc( DnActorHandle hActor )
{
	if( IsAttack() || IsHit() || IsDown() || IsAir() ) return false;
	if( IsMove() && !IsMovable() ) return false;
	if(IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsAir()) return false;

	if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsOpenCashShop())
		return false;

	if (CDnInterface::GetInstance().IsShowItemChoiceDialog())
		return false;

	CDnNPCActor* pNPC = dynamic_cast<CDnNPCActor*>(hActor.GetPointer());
	if ( !pNPC ) return false;

	if( !pNPC->CanTalk() ) return -1;
	if( CDnCamera::GetActiveCamera() ) {
		if( CDnCamera::GetActiveCamera()->GetCameraType() != CDnCamera::PlayerCamera ) return false;
	}

	return true;
}

bool CDnLocalPlayerActor::CalcCrosshairOffsetActor( float fTargetLength, OUT EtVector3 *pvStartPos, OUT MatrixEx *pResultCross, 
											   OUT float *pfResultLength, OUT DnActorHandle *phResultHandle, OUT int* pResultTargetType )
{
	int nWidth = CEtDevice::GetInstance().Width() / 2;
	int nHeight = (int)( CEtDevice::GetInstance().Height() * s_fCrossHairHeightRatio );
	EtVector3 vOrig, vDir;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();

	if( !hCamera ) return false;
	hCamera->GetCameraHandle()->CalcPositionAndDir( nWidth, nHeight, vOrig, vDir );

	CDnPlayerCamera *pCamera = dynamic_cast<CDnPlayerCamera *>(hCamera.GetPointer());
	if( pCamera ) vOrig += vDir * pCamera->GetDistance();

	EtVector3 vTemp;
	vTemp = vOrig + ( vDir * ( fTargetLength / 2.f ) );
	
	DNVector(DnActorHandle) hVecList;
	std::vector<DnActorHandle> hVecNpcList;

	int nCount = ScanActor( vTemp, fTargetLength / 2.f, hVecList );

	float fMinDistance = FLT_MAX;
	float fDist = 0.f;
	DnActorHandle hResult;
	SSegment Segment;
	Segment.vOrigin = vOrig;
	Segment.vDirection = vDir * fTargetLength;
	for( int i=0; i<nCount; i++ ) 
	{
		if( !hVecList[i] ) continue;
		if( hVecList[i] == GetMySmartPtr() ) continue;
		if( hVecList[i]->GetActorType() == CDnActorState::Vehicle) continue;
		if( (hVecList[i]->GetActorType() != CDnActorState::Npc) && (hVecList[i]->GetActorType() > Reserved6) ) continue;

		if( hVecList[i]->IsDie() && !hVecList[i]->IsShow() ) continue;

		MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(hVecList[i].GetPointer());
		if( !pRender->GetObjectHandle() ) continue;
		if( pRender->GetObjectHandle()->TestSegmentCollision( Segment ) ) 
		{
			// npc 는 우선순위로 처리 하기때문에 따로 담아둔다.
			if ( hVecList[i]->GetActorType() == CDnActorState::Npc )
			{
				hVecNpcList.push_back(hVecList[i]);
			}

			// npc 가 아닌 것들 중에서 가장 가까운 녀석을 찾아 준다.
			else
			{
				fDist = EtVec3Length( &( *hVecList[i]->GetPosition() - vOrig ) );
				if( fDist < fMinDistance ) {
					fMinDistance = fDist;
					hResult = hVecList[i];
				}
			}
		}
	}

	// npc 만 모아놓은 목록에서 다시 최소거리에 있는 녀석을 구한다.
	fMinDistance = FLT_MAX;
	for ( size_t i = 0 ; i < hVecNpcList.size() ; i++ )
	{
		CDnNPCActor *pNpc = dynamic_cast<CDnNPCActor *>(hVecNpcList[i].GetPointer());
		if( !pNpc ) continue;
		if( !pNpc->IsEnableOperator() ) continue;

		fDist = EtVec3Length( &( *hVecNpcList[i]->GetPosition() - vOrig ) );

		if( fDist < fMinDistance ) {
			fMinDistance = fDist;
			hResult = hVecNpcList[i];
		}
	}

	if( hResult ) 
	{
		float fTemp = EtVec3Length( &( *hResult->GetPosition() - vOrig ) );
		if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		if( phResultHandle ) *phResultHandle = hResult;
	}

	else 
	{
		EtVector3 vPickPos;
		if( CDnWorld::GetInstance().Pick( vOrig, vDir, vPickPos ) == true ) {
			float fTemp = EtVec3Length( &( vPickPos - vOrig ) );
			if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		}
	}


	if( pvStartPos ) 
	{
		if( pResultCross ) 
		{
			pResultCross->m_vPosition = *pvStartPos;
			pResultCross->m_vZAxis = ( vOrig + ( vDir * fTargetLength ) ) - *pvStartPos;
			EtVec3Normalize( &pResultCross->m_vZAxis, &pResultCross->m_vZAxis );
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vYAxis, &pResultCross->m_vYAxis );
			EtVec3Cross( &pResultCross->m_vXAxis, &pResultCross->m_vYAxis, &pResultCross->m_vZAxis );
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
		}
	}
	else 
	{
		if( pResultCross ) 
		{
			pResultCross->m_vPosition = vOrig;
			pResultCross->m_vZAxis = vDir;
			EtVec3Normalize( &pResultCross->m_vZAxis, &pResultCross->m_vZAxis );
			pResultCross->m_vXAxis = *GetMoveVectorX();
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vYAxis, &pResultCross->m_vYAxis );
			pResultCross->MoveLocalZAxis( fTargetLength );
		}
	}

	if( !hResult ) 
		return false;
	
	if( pfResultLength )
	{
		*pfResultLength = fTargetLength;
	}

	if( pResultTargetType )
	{
		if( hResult->GetActorType() != CDnActorState::Npc )
		{
			CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pTask ) {
				if( pTask->IsCombat() ) return false;
				if( pTask->GetGameTaskType() == GameTaskType::PvP ) return false;
			}

			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(hResult.GetPointer());
			
			//RLKT FIX CROSHAIR
			if (pTask)
			{
				if (pPlayer->IsBattleMode() && pTask->GetGameTaskType() == GameTaskType::Farm)
				return false; //rlkt
			}

			if( pPlayer && ( ( pPlayer->GetAccountLevel() < AccountLevel_New ) || ( pPlayer->GetAccountLevel() == AccountLevel_QA ) ) ) {
				*pResultTargetType = CrossHairType::Player;
				return true;
			}
			else return false;
		}
		*pResultTargetType = CrossHairType::Npc;
	}

	return true;
}

bool CDnLocalPlayerActor::CalcCrosshairOffset( EtVector3 *pvStartPos, MatrixEx *pResultCross, float *pfResultLength, DnActorHandle *phResultHandle, float fExpectationMinDistance )
{
	bool bEnemy = true;
	float fTargetLength = 0.f;

	if( m_hWeapon[0] )
	{
		fTargetLength += (float)m_hWeapon[0]->GetWeaponLength();
		fTargetLength += m_fNormalProjectileAdditionalRange;
	}

	// 스킬 Ready 상태일경우 스킬의 사정거리 추가.
	if( IsProcessSkill() ) {
		DnSkillHandle hSkill = m_hProcessSkill;
		if( hSkill ) {
			fTargetLength += hSkill->GetIncreaseRange();
			if( hSkill->GetTargetType() == CDnSkill::Friend ) bEnemy = false;
		}
	}
	// 화살일 경우엔 화살 자체사정거리 추가
	else if( m_hWeapon[1] && m_hWeapon[1]->GetEquipType() == CDnWeapon::Arrow ) {
		fTargetLength += m_hWeapon[1]->GetWeaponLength();
	}

	int nWidth = CEtDevice::GetInstance().Width() / 2;
	int nHeight = (int)( CEtDevice::GetInstance().Height() * s_fCrossHairHeightRatio );
	EtVector3 vOrig, vDir;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();

	if( !hCamera ) return false;
	hCamera->GetCameraHandle()->CalcPositionAndDir( nWidth, nHeight, vOrig, vDir );

	CDnPlayerCamera *pCamera = dynamic_cast<CDnPlayerCamera *>(hCamera.GetPointer());
	if( pCamera ) vOrig += vDir * pCamera->GetDistance();

	EtVector3 vTemp;
	vTemp = vOrig + ( vDir * ( fTargetLength / 2.f ) );
	DNVector(DnActorHandle) hVecList;
	int nCount = ScanActor( vTemp, fTargetLength / 2.f, hVecList );

	float fMinDistance = FLT_MAX;
	float fDist = 0.f;
	DnActorHandle hResult;
	SSegment Segment;
	Segment.vOrigin = vOrig;
	Segment.vDirection = vDir * fTargetLength;
	for( int i=0; i<nCount; i++ ) {
		if( !hVecList[i] ) continue;
		if( hVecList[i] == GetMySmartPtr() ) continue;

		// 대포인 경우엔 적인지 팀인지 상관없이 체크 처리.
		if( hVecList[ i ]->GetActorType() != CDnActorState::Cannon )
		{
			if( bEnemy ) {
				if( hVecList[i]->GetTeam() == GetTeam() ) continue;
			}
			else {
				if( hVecList[i]->GetTeam() != GetTeam() ) continue;
			}
		}

		if( hVecList[i]->IsDie() ) continue;

		bool bValid = true;
		switch( hVecList[i]->GetActorType() ) {
			case CDnActorState::Npc:
			case CDnActorState::Vehicle:
			case CDnActorState::Pet:
				bValid = false;
				break;
			case CDnActorState::SimpleRush:
				if( dynamic_cast<CDnHideMonsterActor*>(hVecList[i].GetPointer()) ) bValid = false;
				break;
		}
		if( !bValid ) continue;

		MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(hVecList[i].GetPointer());
		if( !pRender->GetObjectHandle() ) continue;
		if( pRender->GetObjectHandle()->TestSegmentCollision( Segment ) ) {
			SSphere Sphere;
			fDist = EtVec3Length( &( *hVecList[i]->GetPosition() - vOrig ) );
//			hVecList[i]->GetBoundingSphere( Sphere );
//			OutputDebug( "Length : %.2f, %.2f, %.2f\n", fDist, fTargetLength, Sphere.fRadius );
//			if( fDist > fTargetLength + ( Sphere.fRadius / 2.f ) ) continue;
			if( fExpectationMinDistance > 0.f && fDist < fExpectationMinDistance ) continue;
			if( fDist < fMinDistance ) {
				fMinDistance = fDist;
				hResult = hVecList[i];
			}
		}
	}
	if( hResult ) {
		float fTemp = EtVec3Length( &( *hResult->GetPosition() - vOrig ) );
		if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		if( phResultHandle ) *phResultHandle = hResult;
	}
	else {
		EtVector3 vPickPos;
		if( CDnWorld::GetInstance().Pick( vOrig, vDir, vPickPos ) == true ) {
			float fTemp = EtVec3Length( &( vPickPos - vOrig ) );
			if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		}
	}


	if( pvStartPos ) {
		if( pResultCross ) {
			pResultCross->m_vPosition = *pvStartPos;
			pResultCross->m_vZAxis = ( vOrig + ( vDir * fTargetLength ) ) - *pvStartPos;
			EtVec3Normalize( &pResultCross->m_vZAxis, &pResultCross->m_vZAxis );
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vYAxis, &pResultCross->m_vYAxis );
			EtVec3Cross( &pResultCross->m_vXAxis, &pResultCross->m_vYAxis, &pResultCross->m_vZAxis );
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
		}
	}
	else {
		if( pResultCross ) {
			pResultCross->m_vPosition = vOrig;
			pResultCross->m_vZAxis = vDir;
			EtVec3Normalize( &pResultCross->m_vZAxis, &pResultCross->m_vZAxis );
			pResultCross->m_vXAxis = *GetMoveVectorX();
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vYAxis, &pResultCross->m_vYAxis );
			pResultCross->MoveLocalZAxis( fTargetLength );
		}
	}

	if ( hResult )
	{
		if( pfResultLength ) *pfResultLength = fTargetLength;
	}

	return ( hResult ) ? true : false;
}

bool CDnLocalPlayerActor::CalcCrosshairOffsetProp( float fTargetLength, EtVector3 *pvStartPos, MatrixEx *pResultCross, float *pfResultLength, DnPropHandle *phResultHandle, int* pResultTargetType )
{
	int nWidth = CEtDevice::GetInstance().Width() / 2;
	int nHeight = (int)( CEtDevice::GetInstance().Height() * s_fCrossHairHeightRatio );
	EtVector3 vOrig, vDir;
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();

	if( !hCamera ) return false;
	hCamera->GetCameraHandle()->CalcPositionAndDir( nWidth, nHeight, vOrig, vDir );

	CDnPlayerCamera *pCamera = dynamic_cast<CDnPlayerCamera *>(hCamera.GetPointer());
	if( pCamera ) vOrig += vDir * pCamera->GetDistance();

	EtVector3 vTemp;
	vTemp = vOrig + ( vDir * ( fTargetLength / 2.f ) );
	DNVector(DnPropHandle) hVecList;
	int nCount = CDnWorld::GetInstance().ScanProp( vTemp, fTargetLength / 2.f, PTE_Chest, hVecList );
	nCount += CDnWorld::GetInstance().ScanProp( vTemp, fTargetLength / 2.f, PTE_Operation, hVecList );
	nCount += CDnWorld::GetInstance().ScanProp( vTemp, fTargetLength / 2.f, PTE_KeepOperation, hVecList );
	nCount += CDnWorld::GetInstance().ScanProp( vTemp, fTargetLength / 2.f, PTE_OperationDamage, hVecList );
	nCount += CDnWorld::GetInstance().ScanProp( vTemp, fTargetLength, PTE_Npc, hVecList );

	float fMinDistance = FLT_MAX;
	float fDist = 0.f;
	DnPropHandle hResult;
	SSegment Segment;
	Segment.vOrigin = vOrig;
	Segment.vDirection = vDir * fTargetLength;
	SOBB Box;
	EtObjectHandle hObject;
	for( int i=0; i<nCount; i++ ) 
	{
		if( !hVecList[i] ) continue;
		hObject = hVecList[i]->GetObjectHandle();
		if( !hObject ) continue;

		if( hObject->GetCollisionPrimitiveCount() == 0 ) {
			hObject->GetBoundingBox( Box );
			if( TestSegmentToOBB( Segment, Box ) == true ) {
				fDist = EtVec3Length( &( hVecList[i]->GetMatEx()->m_vPosition - vOrig ) );
				if( fDist < fMinDistance ) {
					fMinDistance = fDist;
					hResult = hVecList[i];
				}
			}

		}
		else {
			if( hObject->TestSegmentCollision( Segment ) ) {
				fDist = EtVec3Length( &( hVecList[i]->GetMatEx()->m_vPosition - vOrig ) );
				if( fDist < fMinDistance ) {
					fMinDistance = fDist;
					hResult = hVecList[i];
				}
			}
		}
	}
	if( hResult ) {
		float fTemp = EtVec3Length( &( *hResult->GetPosition() - vOrig ) );
		if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		if( phResultHandle ) *phResultHandle = hResult;
	}
	else {
		EtVector3 vPickPos;
		if( CDnWorld::GetInstance().Pick( vOrig, vDir, vPickPos ) == true ) {
			float fTemp = EtVec3Length( &( vPickPos - vOrig ) );
			if( fTemp <= fTargetLength ) fTargetLength = fTemp;
		}
	}


	if( pvStartPos ) {
		if( pResultCross ) {
			pResultCross->m_vPosition = *pvStartPos;
			pResultCross->m_vZAxis = ( vOrig + ( vDir * fTargetLength ) ) - *pvStartPos;
			EtVec3Normalize( &pResultCross->m_vZAxis, &pResultCross->m_vZAxis );
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
			EtVec3Cross( &pResultCross->m_vXAxis, &pResultCross->m_vYAxis, &pResultCross->m_vZAxis );
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
		}
	}
	else {
		if( pResultCross ) {
			pResultCross->m_vPosition = vOrig;
			pResultCross->m_vZAxis = vDir;
			pResultCross->m_vXAxis = *GetMoveVectorX();
			EtVec3Cross( &pResultCross->m_vYAxis, &pResultCross->m_vZAxis, &pResultCross->m_vXAxis );
			EtVec3Normalize( &pResultCross->m_vXAxis, &pResultCross->m_vXAxis );
			pResultCross->MoveLocalZAxis( fTargetLength );
		}
	}

	if( !hResult ) 
		return false;

	if( pfResultLength )
	{
		*pfResultLength = fTargetLength;
	}

	if( pResultTargetType )
	{
		switch( hResult->GetPropType() ) {
			/*
			case PTE_Chest: 
				{
					// TODO: 여기서 열수 있나 없나 체크해서 Lock/Unlock 리턴 틀리게 해준다... 
					// 특정 아이템을 갖고 있는지 클라에서만 체크해주도록 한다.
					CDnWorldChestProp* pChestProp = dynamic_cast<CDnWorldChestProp*>(hResult.GetPointer());
					if( !pChestProp ) return false;
					int iState = pChestProp->GetOperateState();
					switch( iState )
					{
						case CDnWorldChestProp::CLOSED:
							if( pChestProp->CanOpen() )
								*pResultTargetType = CrossHairType::Chest_UnLock;
							else
								*pResultTargetType = CrossHairType::Chest_Lock;
							break;

						case CDnWorldChestProp::OPEN_SUCCESS:
							*pResultTargetType = CrossHairType::Normal;
							break;
					}
				}
				break;
				*/
			case PTE_Operation:
			case PTE_Chest:
			case PTE_KeepOperation:
			case PTE_OperationDamage:
				{
					CDnWorldOperationProp *pOperProp = dynamic_cast<CDnWorldOperationProp *>(hResult.GetPointer());
					if( !pOperProp ) return false;
					// 일단 아예 안바뀌게 해논다
					// 나중에 바뀌는걸 원하면 조작 못하는 아이콘 따위로 바꿔주고 return 안하게 하믄 된다.
					if( !pOperProp->IsEnableOperator() ) return false;

					*pResultTargetType = pOperProp->GetCrosshairType();
				}
				break;
			case PTE_Npc:
				{
					CDnWorldNpcProp *pNpcProp = dynamic_cast<CDnWorldNpcProp *>(hResult.GetPointer());
					if( !pNpcProp ) return false;
					if( !pNpcProp->GetActorHandle() ) return false;

					*pResultTargetType = CrossHairType::Npc;

				}
				break;

		}
	}

	return true;
}


void CDnLocalPlayerActor::ResetActor()
{
	CDnPlayerActor::ResetActor();

	memset( m_LastPushDirKeyTime, 0, sizeof(m_LastPushDirKeyTime) );
	memset( m_LastReleaseDirKeyTime, 0, sizeof(m_LastPushDirKeyTime) );
	memset( m_LastPrevPushDirKeyTime, 0, sizeof(m_LastPrevPushDirKeyTime) );
	m_LastSendMoveMsg = 0;
	m_cLastPushDirKey = 0;
	m_cLastPushDirKeyIndex = 0;
	m_cLastSpecialCommand = -1;
	m_vLastValidPos = EtVector3( 0.f, 0.f, 0.f );

	m_bResetMoveMsg = true;

	//blondy
	ResetAutoRun();
	//blondy end 

	OutputDebug( "ResetActor!!\n" );

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		hCamera->RemoveAllCameraEffect();
	}

	m_LastEscapeTime = 0;
	m_fPosHistoryDelta = 0.f;

	m_bIgnoreRefreshViewCamera = false;
	/*
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	CDnPlayerCamera *pCamera = dynamic_cast<CDnPlayerCamera *>(hCamera.GetPointer());
	if( pCamera ) pCamera->ResetCamera();
	*/
	m_LastUpdatePickupPet = 0;
	m_nVecRequestPickupItemList.clear();
}


void CDnLocalPlayerActor::LockInput( bool bLock )
{ 
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if ( pLocalPlayer )
	{
		pLocalPlayer->m_bLockInput = bLock;
		if( bLock ) pLocalPlayer->ReleaseAllButton();
	}
}

bool CDnLocalPlayerActor::IsLockInput()
{ 
	if( !CDnActor::s_hLocalActor ) return false;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if ( pLocalPlayer )
	{
		return pLocalPlayer->m_bLockInput;
	}
	return false;
}

void CDnLocalPlayerActor::LockItemMove(bool bLock)
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if ( pLocalPlayer )
		pLocalPlayer->m_bLockItemMove = bLock;
}

bool CDnLocalPlayerActor::IsLockItemMove()
{
	if( !CDnActor::s_hLocalActor ) return false;
	CDnLocalPlayerActor* pLocalPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if ( pLocalPlayer )
		return pLocalPlayer->m_bLockItemMove;

	return false;
}


void CDnLocalPlayerActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	CDnPlayerActor::AttachWeapon( hWeapon, nEquipIndex, bDelete );

	if( !IsBattleMode() && IsCanBattleMode() ) CmdToggleBattle( true );
	if( hWeapon )
		_OnAttachEquip( hWeapon.GetPointer(), CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex );
}

void CDnLocalPlayerActor::DetachWeapon( int nEquipIndex )
{
#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() != false)
	{
#endif // PRE_ADD_50907

	if( m_hWeapon[ nEquipIndex ] ) {
		_OnDetachEquip( m_hWeapon[ nEquipIndex ].GetPointer(), CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex );

		CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
		if( pItemTask && !pItemTask->IsRequestRepair() ) {
			if( GetCashWeapon( nEquipIndex ) && IsViewWeaponOrder( nEquipIndex ) == false )
				CmdToggleWeaponViewOrder( nEquipIndex, true );
		}
	}

#if defined(PRE_ADD_50907)
	}
#endif // PRE_ADD_50907

	CDnPlayerActor::DetachWeapon( nEquipIndex );

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	if( IsBattleMode() && !IsCanBattleMode() ) CmdToggleBattle( false );

}

void CDnLocalPlayerActor::AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete, bool bRefreshWeaponOrder )
{
	CDnPlayerActor::AttachCashWeapon( hWeapon, nEquipIndex, bDelete, bRefreshWeaponOrder );

	if( bRefreshWeaponOrder ) {
		CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
		if( pItemTask && !pItemTask->IsRequestRepair() ) {
			if( IsViewWeaponOrder( nEquipIndex ) == false )
				CmdToggleWeaponViewOrder( nEquipIndex, true );
		}
	}
}

void CDnLocalPlayerActor::DetachCashWeapon( int nEquipIndex )
{
	CDnPlayerActor::DetachCashWeapon( nEquipIndex );
}


void CDnLocalPlayerActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_LEVELUP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nLevel, nExperience;

				Stream.Read( &nLevel, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nExperience, sizeof(int) );

				if( nLevel < m_nLevel ) m_nLevel = 1; // 서버에서 보내주는 레벨을 믿고 가도록 설정. 기존에는 경험치기반으로 계산이 들어갔다.

				SetExperience( nExperience );

				CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
				if( pTask ) pTask->UpdatePartyMemberInfo();

				// 스킬 레벨업에 따른 SP 갱신과 트리 상태 갱신
				GetInterface().RefreshSkillDialog();

				// 문장 커버 갱신
				GetInterface().RefreshGlyphCover();

#ifdef PRE_ADD_LEVELUP_GUIDE
				GetInterface().ShowLevelUpGuide(true, m_nLevel);
#endif
				GetPetTask().DoPetChat( PET_CHAT_USER_LEVEL_UP );
				
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND			
				CDnItemCompoundNotifyDlg* pItemCompoundNotifyDlg = static_cast<CDnItemCompoundNotifyDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_NOTIFY_DIALOG ) );
				if( pItemCompoundNotifyDlg && !pItemCompoundNotifyDlg->IsShow() )
				{
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
					int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CompoundLimitMaxLevel) ;
					if( CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() >= nLimitLevel )
#endif				
					pItemCompoundNotifyDlg->Show( true );
				}
#endif

#ifdef PRE_ADD_STAMPSYSTEM
				GetInterface().AddStamp();
				GetInterface().ShowStampDlg();
#endif // PRE_ADD_STAMPSYSTEM

			}
			return;

		case eActor::SC_ADDEXP:
		{
			CPacketCompressStream Stream(pPacket, 128);

			int nAddExperience = 0, nExperience = 0, nEventExperience = 0, nPcBangExp = 0, nVIPExp = 0, nPromotionExp = 0, nEmpowermentExp = 0;

			Stream.Read(&nExperience, sizeof(int));			//받은 현재의 전체 경험치
			Stream.Read(&nAddExperience, sizeof(int));		//몬스터 죽였을때 몬스터 기본 경험치
			Stream.Read(&nEventExperience, sizeof(int));		//몬스터 기본경험치에 추가 이벤트분
			Stream.Read(&nPcBangExp, sizeof(int));			//몬스터 기본경험치에 추가 피씨방분
#if defined(PRE_ADD_VIP)
			Stream.Read(&nVIPExp, sizeof(int));				//몬스터 기본경험치에 추가 VIP분
			Stream.Read(&nPromotionExp, sizeof(int));				//프로모션 추가분(만랩보너스 등)
#else
			Stream.Read(&nPromotionExp, sizeof(int));				//프로모션 추가분(만랩보너스 등)
#endif	// #if defined(PRE_ADD_VIP)
#if defined( PRE_USA_FATIGUE )
			Stream.Read(&nEmpowermentExp, sizeof(int));		// 이건 클라이언트 표시용이다. 이미 nAddExperience에 추가되어있는 상태.
#endif	// #if defined( PRE_USA_FATIGUE )

				// #12074 이슈. 잘못된 패킷 올 가능성 없으니 우선 풀어두기로 합니다.
				//if( GetExperience() + nAddExperience + nEventExperience + nPcBangExp != nExperience ) {
				//	WriteLog( 1, ", SC_ADDEXP! - CurExp : %d, AddExp : %d, EventExp : %d, PCBangExp : %d\n", GetExperience(), nAddExperience, nEventExperience, nPcBangExp );
				//	return;
				//}

			AddExperience(nAddExperience + nEventExperience + nPcBangExp + nVIPExp + nPromotionExp, 0, (INT64)0);

			if (GetExperience() != nExperience) {
				SetExperience(nExperience);
			}

			CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask("PartyTask");
			if (pTask) pTask->UpdatePartyMemberInfo();

			// #36808 획득 경험치가 0일 경우엔 표시하지 않는다.
			if (nAddExperience + nEventExperience + nPcBangExp + nVIPExp + nPromotionExp == 0)
				return;

			std::vector<std::wstring> wszVecSub;
#if defined(PRE_ADD_ENGLISH_STRING)
			std::wstring wszMsg = FormatW(L"%s %s. ", DN_INTERFACE::UTIL::GetAddCommaString(nAddExperience).c_str(), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2360));

#if defined( PRE_USA_FATIGUE )
			if (nEmpowermentExp) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116032), DN_INTERFACE::UTIL::GetAddCommaString(nEmpowermentExp).c_str()));
#endif	// #if defined( PRE_USA_FATIGUE )

			if (nPcBangExp) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6203), DN_INTERFACE::UTIL::GetAddCommaString(nPcBangExp).c_str()));
#ifdef PRE_ADD_VIP
			if (nVIPExp) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020028), DN_INTERFACE::UTIL::GetAddCommaString(nVIPExp).c_str()));
#endif
			if (nEventExperience) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116031), DN_INTERFACE::UTIL::GetAddCommaString(nEventExperience).c_str()));
			if (nPromotionExp) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1374), DN_INTERFACE::UTIL::GetAddCommaString(nPromotionExp).c_str()));
#else // #if defined(PRE_ADD_ENGLISH_STRING)
			//	std::wstring wszMsg = FormatW( L"%d", nAddExperience );
			std::wstring wszMsg = FormatW(L"%d %s ", nAddExperience, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2360));

#if defined( PRE_USA_FATIGUE )
			if (nEmpowermentExp) wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116032), nEmpowermentExp));
#endif	// #if defined( PRE_USA_FATIGUE )
			//rlkt_Test fix exp :)
			if (nPcBangExp) {
				if (wcsstr(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6203), L"%s"))
				{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6203), FormatW(L"%d",nPcBangExp).c_str()));
				}else{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 6203), nPcBangExp));
				}
			}
#ifdef PRE_ADD_VIP
			if (nVIPExp)
			{
				if (wcsstr(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020028), L"%s"))
				{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020028), FormatW(L"%d", nVIPExp).c_str()));
				}else {
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2020028), nVIPExp));
				}
			}
#endif
			if (nEventExperience)
			{
				if (wcsstr(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116031), L"%s"))
				{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116031), FormatW(L"%d", nEventExperience).c_str()));
				}else{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 116031), nEventExperience));
				}
			}
			if (nPromotionExp)
			{
				if (wcsstr(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1374), L"%s"))
				{
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1374), FormatW(L"%d", nPromotionExp).c_str() ));
				}else {
					wszVecSub.push_back(FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1374), nPromotionExp));
				}
			}
#endif // #if defined(PRE_ADD_ENGLISH_STRING)

				if( !wszVecSub.empty() ) {
#ifdef PRE_MOD_IMPROVEMENT_EXP_SHOW
					wszMsg += L" (";
#else
					wszMsg += L"(";				
#endif 
					for( DWORD i=0; i<wszVecSub.size(); i++ ) {
						wszMsg += L"+" + wszVecSub[i];
						if( i != wszVecSub.size() - 1 ) wszMsg += L", ";
					}
					wszMsg += L")";
				}

				GetInterface().AddMessageExp( 0, wszMsg.c_str() );
			}
			return;
		case eActor::SC_REFRESHHPSP:
			{
				bool bDie = IsDie();
				CDnActor::OnDispatchMessage( dwActorProtocol, pPacket );

				if( bDie && !IsDie() ) {
					CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
					if (pWorldEnv)
					{
						pWorldEnv->ResetSaturation();
						if (GetCurrentGhostType() == Ghost::Type::PartyRestore)
							pWorldEnv->ControlColorFilter(CDnWorldEnvironment::Highlight, GetLastHighLightColor(), 1000);
					}
				}
				if( !bDie && IsDie() ) {
						SetWorldColorFilterOnDie();
				}
				if (bDie && IsDie() && IsSetGhostColorFilter() == eGhostScreen_None)
					SetWorldColorFilterOnDie();
			}
			return;
		case eActor::SC_CP:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nType, nValue;
				Stream.Read( &nType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nValue, sizeof(int) );

				if( MACP::MaxComboCount != nType 
					&& MACP::KillBossCount != nType )
					m_Data.nAccumulationCP += nValue;

				OnEventCP( (CPTypeEnum)nType, nValue );

			}
			break;

		case eActor::SC_COOLTIME_RESET :
			{
				CPacketCompressStream Stream( pPacket, 4 );

				int iSkillID;
				Stream.Read( &iSkillID, sizeof(int) );

				CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
				DnSkillHandle hSkill = pSkillTask->FindSkill( iSkillID );
				if( hSkill ) hSkill->SetResetSkill();

#ifndef _FINAL_BUILD
				std::wstring wszSkillName = hSkill->GetName();
				GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"Debug", FormatW( L" [%s] Skill Cooltime Reset" , wszSkillName.c_str()).c_str() );
#endif
			}
			break;

		case eActor::SC_PASSIVESKILL_COOLTIME:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				DWORD dwUniqueID = 0;
				int nSkillID = 0;
				
				Stream.Read( &dwUniqueID, sizeof(dwUniqueID) );
				Stream.Read( &nSkillID, sizeof(nSkillID) );
				
				DnSkillHandle hSkill = FindSkill(nSkillID);

				if( hSkill && hSkill->HasCoolTime()  ) {
					hSkill->OnBeginCoolTime();

					CDnMainDlg *pMainDlg = (CDnMainDlg*)GetInterface().GetMainBarDialog();
					if( pMainDlg ) {
						pMainDlg->AddPassiveSkill( hSkill );
					}
				}
			}
			break;

		// #26902
		// 2차 전직 스킬을 임시로 사용해볼 수 있는 상태로 만들어주기 위해 퀘스트에서 사용되는 api 에서 호출되는 함수에서
		// 클라이언트로 보내주는 패킷. 아래 패킷들이 몇개씩 셋트로 오게됨.
		// 게임서버에서 존이동 했을 때에도 임시 전직/스킬 모두 초기화 해주어야 한다.
		case eActor::SC_DO_TEMP_JOBCHANGE:
			{
				// 임시로 이 직업 코드로 전직.
				CPacketCompressStream Stream( pPacket, 32 );

				int iJobID;
				Stream.Read( &iJobID, sizeof(int) );

				// 스킬 트리 싹 지웠다가
				vector<int> vlJobHistory;
				GetJobHistory( vlJobHistory );
				for( int i = 0; i < (int)vlJobHistory.size(); ++i )
					GetSkillTask().RemoveJobSkillTree( vlJobHistory.at( i ) );

				SetJobHistory( iJobID );

				// 다시 생성.
				vlJobHistory.clear();
				GetJobHistory( vlJobHistory );
				for( int i = 0; i < (int)vlJobHistory.size(); ++i )
					GetSkillTask().AddJobSkillTree( vlJobHistory.at( i ) );
				
				OnChangeJob( iJobID );

				// #40097 메모리 최적화 작업에 따른 액션 파일 나눠진거에 대응.
				// 바뀐 직업의 액션 시그널에 박혀 있는 리소스를 로드하기 위해 호출해줌.
				RecomposeAction();

				//// 파티에 있는 정보도 찾아서 바꿔준다.
				//CDnPartyTask* pTask = (CDnPartyTask*)CTaskManager::GetInstance().GetTask( "PartyTask" );
				//if( pTask )
				//	pTask->UpdatePartyMemberInfo();
			}
			break;

		case eActor::SC_RESTORE_TEMP_JOBCHANGE:
			{
				// 임시로 추가되었던 직업 코드 해제.
				CPacketCompressStream Stream( pPacket, 32 );

				int iJobID;
				Stream.Read( &iJobID, sizeof(int) );

				// 스킬 트리 싹 지웠다가
				vector<int> vlJobHistory;
				GetJobHistory( vlJobHistory );
				for( int i = 0; i < (int)vlJobHistory.size(); ++i )
					GetSkillTask().RemoveJobSkillTree( vlJobHistory.at( i ) );

				PopJobHistory( iJobID );

				// 다시 생성.
				vlJobHistory.clear();
				GetJobHistory( vlJobHistory );
				for( int i = 0; i < (int)vlJobHistory.size(); ++i )
					GetSkillTask().AddJobSkillTree( vlJobHistory.at( i ) );
				
				OnChangeJob( iJobID );

				// #40097 메모리 최적화 작업에 따른 액션 파일 나눠진거에 대응.
				// 바뀐 직업의 액션 시그널에 박혀 있는 리소스를 로드하기 위해 호출해줌.
				RecomposeAction();
			}
			break;

		case eActor::SC_ADD_TEMP_SKILL:
			{
				// 임시 스킬로 생성해서 추가.
				int iSkillID = 0;
				CPacketCompressStream Stream( pPacket, 32 );
				Stream.Read( &iSkillID, sizeof(int) );

				//bool bSuccess = AddSkill( iSkillID, 1, CDnSkill::PVE );
				CDnSkillTask::GetInstance().AddTempSkill( iSkillID );
				//if( bSuccess )
				//{
				//	DnSkillHandle hSkill = FindSkill( iSkillID );
				//	if( hSkill )
				//	{
				//		//m_vlhLocalPlayerOpenedSkillList.push_back(  );
				//		//hCheatAcquiredSkill->SetHasActor( CDnActor::s_hLocalActor );
				//		//hCheatAcquiredSkill->Acquire();

				//		hSkill->AsTempSkill();

				//		// 2차 전직스킬이라서 현재 사용할 수 없는 스킬이라면,
				//		// 현재 캐릭터 레벨 및 직업에 맞게 객체 값을 바꿔준다.
				//		if( GetLevel() < hSkill->GetLevelLimit() )
				//			hSkill->SetLevelLimit( GetLevel() );

				//		if( false == IsPassJob( hSkill->GetNeedJobClassID() ) )
				//			hSkill->SetNeedJobClassID( GetJobClassID() );
				//	}
				//}
			}
			break;

		case eActor::SC_REMOVE_TEMP_SKILL:
			{
				// 임시 스킬로 생성했던 것 제거.
				int iSkillID = 0;
				CPacketCompressStream Stream( pPacket, 32 );
				Stream.Read( &iSkillID, sizeof(int) );

				CDnSkillTask::GetInstance().RemoveTempSkill( iSkillID );

				//// TODO: 강화 패시브 스킬인 경우 삭제될 때 문제 없는지 확인...
				//DnSkillHandle hSkill = FindSkill( iSkillID );
				//if( hSkill )
				//{
				//	_ASSERT( hSkill->IsTempSkill() );
				//	RemoveSkill( iSkillID );
				//}
			}
			break;
		case eActor::SC_UDP_PING:
			{
				DWORD dwTick = 0;
				{
					CPacketCompressStream Stream( pPacket, 32 );
					Stream.Read( &dwTick, sizeof(DWORD) );
				}

				BYTE pBuffer[128] = { 0, };
				CPacketCompressStream Stream( pBuffer, 128 );

				Stream.Write( &dwTick, sizeof(dwTick) );

				Send( eActor::CS_UDP_PING, &Stream );
				break;
			}
		case eActor::SC_GHOST_TYPE:
			{
				Ghost::Type::eCode ghostType;
				CPacketCompressStream Stream( pPacket, 32 );
				Stream.Read( &ghostType, sizeof(int) );

				SetCurrentGhostType(ghostType);
			}
			break;
	}
	CDnPlayerActor::OnDispatchMessage( dwActorProtocol, pPacket );
}

void CDnLocalPlayerActor::OnDrop( float fCurVelocity )
{
	if( IsAir() && !IsHit() ) {
		m_bResetMoveMsg = true;

		BYTE pBuffer[128] = { 0, };
		CPacketCompressStream Stream( pBuffer, 128 );
		DWORD dwGap = GetSyncDatumGap();

		Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
		Stream.Write( &dwGap, sizeof(DWORD) );

		Send( eActor::CS_ONDROP, &Stream );

	}
	CDnPlayerActor::OnDrop( fCurVelocity );
}

void CDnLocalPlayerActor::OnFall( float fCurVelocity )
{
	if( ( IsStay() || IsMove() ) && !IsFloorCollision() ) {
		if( fCurVelocity < -5.f ) {
			m_bResetMoveMsg = true;
		}
	}
	CDnPlayerActor::OnFall( fCurVelocity );
}

void CDnLocalPlayerActor::RenderGateName()
{
	// Note : 포털 게이트 이름 출력
	//
	DWORD dwCount = CDnWorld::GetInstance().GetGateCount();
	CDnWorld::GateStruct *pGateStruct(NULL);

	SOBB *pOBB(NULL);

	// 게이트 이름 출력
	for( DWORD i=0; i<dwCount; i++ )
	{
		pGateStruct = CDnWorld::GetInstance().GetGateStructFromIndex( i );
		if( !pGateStruct ) continue;
		if( !pGateStruct->pGateArea ) continue;
		if( pGateStruct->PermitFlag != CDnWorld::PermitEnter ) continue;

		pOBB = pGateStruct->pGateArea->GetOBB();
		if( !pOBB ) continue;

		EtVector3 vGatePos = pOBB->Center;
		EtVector3 vPlayerPos = m_matexWorld.m_vPosition;
		EtVector3 ScreenPos = ( vGatePos - vPlayerPos );

		float fTemp = sqrtf( powf(ScreenPos.x,2)+powf(ScreenPos.z,2) );
		if( 1000.0f > fTemp )
		{
			if( pGateStruct->hGateProp ) {
				vGatePos = pGateStruct->hGateProp->GetMatEx()->m_vPosition;
				if( pGateStruct->hGateProp->GetObjectHandle() ) {
					EtObjectHandle hHandle = pGateStruct->hGateProp->GetObjectHandle();

					EtVector3 vPos = hHandle->GetMesh()->GetDummyPosByName( "#Plane02" );
					if( EtVec3LengthSq( &vPos ) > 0.f ) {
						vGatePos += vPos;
					}
				}
			}
			else {
				vGatePos.y = -1400.0f;
			}

			float fScale = 1.0f;
			SUICoord Coord;
#ifdef PRE_FIX_RENDER_GATENAME_POS
			std::wstring wszTitleName = CDnWorld::GetInstance().GetGateTitleName( pGateStruct->cGateIndex );
			if( wszTitleName.empty() )
				wszTitleName = pGateStruct->szMapName.c_str();
			CEtFontMng::GetInstance().CalcTextRect( s_nFontIndex, 16, wszTitleName.c_str(), DT_VCENTER | DT_CENTER, Coord, -1 );
#else
			CEtFontMng::GetInstance().CalcTextRect( s_nFontIndex, 16, pGateStruct->szMapName.c_str(), DT_VCENTER | DT_CENTER, Coord, -1 );
#endif

			DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
			vGatePos -= hCamera->GetMatEx()->m_vXAxis * ( ( Coord.fWidth * 0.5f ) * ( GetEtDevice()->Width() * fScale ) );

#ifdef PRE_FIX_RENDER_GATENAME_POS
#else
			std::wstring wszTitleName = CDnWorld::GetInstance().GetGateTitleName( pGateStruct->cGateIndex );
			if( wszTitleName.empty() )
				wszTitleName = pGateStruct->szMapName.c_str();
#endif

			SFontDrawEffectInfo FontEffectInfo;
			FontEffectInfo.dwFontColor = textcolor::FONT_ORANGE;
			FontEffectInfo.nDrawType = SFontDrawEffectInfo::SHADOW;
			FontEffectInfo.nWeight = 2;
			FontEffectInfo.dwEffectColor = 0xff000000;
			CEtFontMng::GetInstance().DrawTextW3D( s_nFontIndex, 16, wszTitleName.c_str(), vGatePos, FontEffectInfo, -1, fScale );
		}
	}
}

void CDnLocalPlayerActor::RenderPartyMemberMarker(float fElapsedTime)
{
	CDnPartyTask* pTask = (CDnPartyTask*)CTaskManager::GetInstance().GetTask("PartyTask");
	if (pTask && CDnActor::s_hLocalActor)
	{
		int nPartyCount = pTask->GetPartyCount();
		if (nPartyCount <= 1)
			return;

		int i = 0;
		for (; i<nPartyCount; ++i)
		{
			CDnPartyTask::PartyStruct* pStruct = pTask->GetPartyData(i);
			if (!pStruct || pStruct->bGMTrace)
				continue;
			if (pStruct->nSessionID != CDnActor::s_hLocalActor->GetUniqueID())
			{
				EtVector3 vMemberPos;
				DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pStruct->nSessionID );
				vMemberPos = (!hActor && hActor->GetPosition()) ? pStruct->vPos : *hActor->GetPosition();

				if (hActor && hActor->IsProcess() == false)
					continue;

				EtCameraHandle hEtCam = CEtCamera::GetActiveCamera();
				if (hEtCam)
				{
					EtVector3* pLocalActorPos = CDnActor::s_hLocalActor->GetPosition();
					if (pLocalActorPos != NULL && GetPosition() && hEtCam->GetPosition())
					{
						bool bShowMemberTag = false;
						float distanceFromLocalActor = EtVec3Length(&(*pLocalActorPos - vMemberPos));
						if (distanceFromLocalActor > CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PartyMarkerDistance))
							bShowMemberTag = true;

						float fRate = 0.8f;
						bool bActorChatBalloon = false;
						if (hActor && hActor->GetChatBalloon().IsRenderChatBalloon())
						{
							m_dwPartyMemberMarkerColor = CommonUtil::BlendColor(fElapsedTime, m_dwPartyMemberMarkerColor, 0x11000000, fRate);
							bActorChatBalloon = true;
						}
						else
						{
							if (m_dwPartyMemberMarkerColor != 0xffffffff)
								m_dwPartyMemberMarkerColor = CommonUtil::BlendColor(fElapsedTime, m_dwPartyMemberMarkerColor, 0xffffffff, fRate);
						}

						bool bSightOff = false;
						if ((bShowMemberTag == false && GetPosition()) || bActorChatBalloon)
						{
							CEtConvexVolume Frustum;
							Frustum.Initialize(*hEtCam->GetViewProjMatForCull());

							SAABox AABB;
							GetBoundingBox(AABB);
							const EtVector3 vDiff = (vMemberPos - *GetPosition());
							AABB.Min += vDiff;
							AABB.Max += vDiff;
							bShowMemberTag = (Frustum.TesToBox( AABB.GetCenter(), AABB.GetExtent()) == false);
							if (bShowMemberTag == true)
								m_dwPartyMemberMarkerColor = 0xffffffff;
						}

						if (bShowMemberTag)
						{
							EtTextureHandle hTex = CDnInterface::GetInstance().GetPartyMemberMarkerTex();

							SUICoord screenUICoord, UVCoord;
							EtVector3 vScreenPos, vTemp2;
							DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
							if (!hCamera)
								break;

							float heightRatio = 1.0f;
							EtVector2 vSize(78.f, 78.f);

							EtVector3 vTemp = vMemberPos;
							vTemp.y += 8.f;
							bool bBackOfCam = false;
							if (CDnActor::s_hLocalActor && hEtCam->GetPosition() && hEtCam->GetDirection())
							{
								EtVector3 vCurlook, vLookFromMember, vCross;
								vCurlook = *hEtCam->GetDirection();
								EtVec3Normalize(&vLookFromMember, &(vTemp - *hEtCam->GetPosition()));
								vLookFromMember.y = vCurlook.y;
								if (EtVec3Dot(&vCurlook, &vLookFromMember) < 0.f)
									bBackOfCam = true;

								screenUICoord.SetSize(vSize.x / (float)GetEtDevice()->Width(), vSize.y / (float)GetEtDevice()->Height());

								if (bBackOfCam)
								{
									screenUICoord.fY = 1.f - screenUICoord.fHeight * 2.f;
									EtVec3Cross( &vCross, &vCurlook, &(EtVector3(0.f, 1.f, 0.f)) );
									if( vCross.y > 0.0f )
										screenUICoord.fX = 1.f - screenUICoord.fWidth;
									else
										screenUICoord.fX = 0.f;
								}
								else
								{
									EtViewPort vp;
									EtMatrix* matViewProj;
									GetEtDevice()->GetViewport(&vp);
									matViewProj = hEtCam->GetViewProjMatForCull();
									EtVec3TransformCoord( &vScreenPos, &vTemp, matViewProj );

									screenUICoord.fX = (( vScreenPos.x + 1.0f ) / 2.0f + vp.X);
									screenUICoord.fY = (( 2.0f - ( vScreenPos.y + 1.35f ) ) / 2.0f + vp.Y);

									CommonUtil::ClipNumber(screenUICoord.fX, screenUICoord.fWidth * 0.5f, 1.f - screenUICoord.fWidth * 0.5f);
									CommonUtil::ClipNumber(screenUICoord.fY, screenUICoord.fHeight * 0.5f, 1.f - screenUICoord.fHeight * 0.5f);

									screenUICoord.fX -= screenUICoord.fWidth / 2.f;
									//screenUICoord.fY -= screenUICoord.fHeight;
									float fEndOfY = 1.f - screenUICoord.fHeight * 2.f;
									if (screenUICoord.fY > fEndOfY)
										screenUICoord.fY = fEndOfY;
								}
							}

							SUICoord stringUICoord(screenUICoord);

							int nFontIndex = INT_MAX;
							SFontDrawEffectInfo Info;
							Info.nDrawType = SFontDrawEffectInfo::SHADOW;
							Info.nWeight = 2;
							Info.dwFontColor = textcolor::PARTYMEMBERNAME;
							Info.dwEffectColor = textcolor::PARTYMEMBERNAME_S;
							Info.dwEffectColor = (Info.dwEffectColor & 0x00ffffff) | (m_dwPartyMemberMarkerColor & 0xff000000);
							Info.dwFontColor = (Info.dwFontColor & 0x00ffffff) | (m_dwPartyMemberMarkerColor & 0xff000000);
							SUICoord FontCoord;
							CEtFontMng::GetInstance().CalcTextRect( nFontIndex, 16, pStruct->wszCharacterName, DT_VCENTER, FontCoord, -1 );
							stringUICoord.fWidth = FontCoord.fWidth;
							stringUICoord.fHeight = FontCoord.fHeight;
							stringUICoord.fX -= (stringUICoord.fWidth - screenUICoord.fWidth) * 0.5f;
							stringUICoord.fY -= (stringUICoord.fHeight);
							if (stringUICoord.fX + stringUICoord.fWidth> 1.f)
								stringUICoord.fX = 1.f - stringUICoord.fWidth;
							if (stringUICoord.fX < 0.f)
								stringUICoord.fX = 0.f;

							CEtFontMng::GetInstance().DrawTextW( nFontIndex, 16, pStruct->wszCharacterName, DT_VCENTER, stringUICoord, -1, Info, false);
							
							int nIndex = pStruct->cClassID - 1;
							int nCountX = 3;
							int nCountY = 3;
							UVCoord = SUICoord((vSize.x * (float)(nIndex % nCountX)) / hTex->Width(), (vSize.y * (float)(nIndex / nCountY)) / hTex->Height(), vSize.x / hTex->Width(), vSize.y / hTex->Width());

							CEtSprite::GetInstance().Begin(0);
							CEtSprite::GetInstance().DrawSprite((EtTexture*)hTex->GetTexturePtr(), hTex->Width(), hTex->Height(), UVCoord, m_dwPartyMemberMarkerColor, screenUICoord, 0.f);
							CEtSprite::GetInstance().End();
						}
					}
				}
			}
		}
	}
}

bool CDnLocalPlayerActor::IsPartyLeader() const
{
	return (CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsLocalActorMaster());
}


void CDnLocalPlayerActor::RenderCustom( float fElapsedTime )
{
	RenderGateName();

	if( IsShow() && m_bShowHeadName ) 
	{
		CalcCustomRenderDepth();
		RenderHeadIcon( fElapsedTime );

		DWORD dwFontColor, dwFontColorS;
		DWORD dwFontColorA, dwFontColorAS;
		GetHeadIconFontColor( this, MAHeadNameRender::Normal, dwFontColor, dwFontColorS );
		GetHeadIconFontColor( this, MAHeadNameRender::Appellation, dwFontColorA, dwFontColorAS );

		float fTextureOffset = 0.0f;
		if( CEtFontMng::s_bUseUniscribe ) fTextureOffset = -2.0f;

#ifdef PRE_ADD_VIP
		if (IsVIP())
			AddHeadNameElement( 0, GetInterface().GetVIPIconTex(), 8.f, 8.f, 1, 1, 0, 0xFFFFFFFF, fTextureOffset );
#endif
		if (IsPartyLeader())
			AddHeadNameElement( 0, m_hPartyLeaderIconTex, 8.f, 8.f, 1, 1, 0, 0xFFFFFFFF, fTextureOffset );

#ifdef PRE_MOD_PVPRANK
#ifdef PRE_ADD_SHOWOPTION_PVPMARK
		bool bShowPVPLevelMark = CGameOption::GetInstance().bPVPLevelMark;
		CDnVillageTask *pVillage = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pVillage && pVillage->GetVillageType() == CDnVillageTask::PvPVillage ) bShowPVPLevelMark = true;
		CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
		if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP ) bShowPVPLevelMark = true;
		if( GetPvPLevel() > 0 && bShowPVPLevelMark )
#else
		if( GetPvPLevel() > 0 )
#endif
			AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 1, 0xFFFFFFFF, fTextureOffset );
#else
		if( GetPvPLevel() > 1 )
			AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 2, 0xFFFFFFFF, fTextureOffset );
#endif

#ifdef PRE_ADD_NEWCOMEBACK
		if( IsSetAppellationIndex() )
		{	
			if( GetComebackLocalUser() )
			{	
				dwFontColorA = textcolor::NAVY;
				dwFontColorAS = textcolor::WHITE;
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, dwFontColorA, dwFontColorAS );
			}
#ifdef PRE_ADD_GM_APPELLATION
			else if( m_bGMAppellation )
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, D3DCOLOR_ARGB( 0xFF, 0xCC, 0x00, 0xCC ), textcolor::WHITE );

#endif
#ifdef PRE_ADD_GM_APPELLAThongse
			else if (m_hongse)
				AddHeadNameElement(0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, D3DCOLOR_ARGB(0xFF, 0xFF, 0x00, 0x00), textcolor::hongse);
#endif
#ifdef PRE_ADD_GM_APPELLAchengse
			else if (m_jinse)
				AddHeadNameElement(0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, D3DCOLOR_ARGB(0xFF, 0xFF, 0xD7, 0x00), textcolor::chengse);
#endif // PRE_ADD_GM_APPELLATION
			else
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, dwFontColorA, dwFontColorAS );
		}
#else
		// 호칭
		if( IsSetAppellationIndex() )
		{
#ifdef PRE_ADD_GM_APPELLATION
			if( m_bGMAppellation )
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, D3DCOLOR_ARGB( 0xFF, 0xCC, 0x00, 0xCC ), textcolor::WHITE );
			else
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, dwFontColorA, dwFontColorAS );
#else // PRE_ADD_GM_APPELLATION
			AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, dwFontColorA, dwFontColorAS );
#endif // PRE_ADD_GM_APPELLATION
		}
#endif // PRE_ADD_NEWCOMEBACK

		// 이름
		AddHeadNameElement( 0, GetName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );

		// 길드전 예선 표시
		if( CDnGuildTask::IsActive() && CGameOption::GetInstance().bGuildName == true && m_GuildSelfView.IsSet() && GetGuildTask().GetCurrentGuldWarScheduleID() > 0 ) {
			if( m_GuildSelfView.wWarSchduleID == GetGuildTask().GetCurrentGuldWarScheduleID() && m_GuildSelfView.cTeamColorCode != 0 ) {
				if( GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_PREPARATION || GetGuildTask().GetCurrentGuildWarEventStep() == GUILDWAR_STEP_TRIAL ) {
					AddHeadNameElement( 0, GetInterface().GetGuildPreliminariesTex(), 8.f, 8.f, 2, 2, m_GuildSelfView.cTeamColorCode, 0xFFFFFFFF, fTextureOffset );
				}
			}
		}

		// 길드마크
		if( CGameOption::GetInstance().bGuildName == true && CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark( m_GuildSelfView ) )
			AddHeadNameElement( 1, GetGuildTask().GetGuildMarkTexture( m_GuildSelfView ), 8.f, 8.f, 1, 1, 0, 0xFFFFFFFF, fTextureOffset );

		// 길드
		if( CGameOption::GetInstance().bGuildName == true && m_GuildSelfView.IsSet() ) {
			WCHAR wszGuildName[GUILDNAME_MAX+2];
			swprintf_s( wszGuildName, GUILDNAME_MAX+2, L"<%s>", m_GuildSelfView.wszGuildName );
			AddHeadNameElement( 1, wszGuildName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
		}

#ifdef PRE_ADD_DWC
		// DWC 팀 이름
		if( GetDWCTask().IsDWCRankSession() && GetDWCTask().IsDWCChar() && GetDWCTask().HasDWCTeam() && IsJoinGuild() == false )
		{
			WCHAR wszDWCTeamName[GUILDNAME_MAX+2] = { 0 , };
			swprintf_s( wszDWCTeamName, GUILDNAME_MAX+2, L"<%s>", GetDWCTask().GetMyDWCTeamName() );
			AddHeadNameElement( 1, wszDWCTeamName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
		}
#endif

		if (m_bPlayerAway)
		{
			int STATE_MARK_SCALE_TIME  = 250;
			float STATE_MARK_SCALE_RANGE  = 0.1f;

			EtVector2 vSize;
			vSize.x = m_hAbsenceMarkTex->Width() * 0.5f;
			vSize.y = m_hAbsenceMarkTex->Height() * 0.5f;

			int nTemp = (DWORD)CDnActionBase::m_LocalTime%(STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME);
			if( nTemp < STATE_MARK_SCALE_TIME ) {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * nTemp;
			}
			else {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * ( (STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME) - nTemp );
			}

			int nAlpha = (int)(GetAlphaLayer( AL_APPEAR ) * 255);
			nAlpha = EtClamp(nAlpha, 0, 255);

			AddHeadNameElement( 2, m_hAbsenceMarkTex, vSize, D3DCOLOR_ARGB(nAlpha,255,255,255), 15.f );
		}
	}
	CDnActor::RenderCustom( fElapsedTime );

	if( m_pDamageReaction ) {
		m_pDamageReaction->Render( fElapsedTime, (int)GetHP(), GetHPPercentFloat(), *GetPosition() );
	}

#ifdef RENDER_PRESS_hayannal2009
	RenderPress();

//#define _VOICE_TEST
#ifdef _VOICE_TEST
	if( s_bEnableRenderPress )
	{
		EtVector3 vPos[2];
		vPos[0] = GetMatEx()->m_vPosition;
		vPos[1] = GetMatEx()->m_vPosition + ( *GetMoveVectorZ() * 100.f );
		for( int i=0; i<2; i++ ) vPos[i].y += 50.f;
		EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xffffffff );
		vPos[0] = GetMatEx()->m_vPosition;
		vPos[1] = GetMatEx()->m_vPosition + ( *GetMoveVectorX() * 100.f );
		for( int i=0; i<2; i++ ) vPos[i].y += 50.f;
		EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xffffffff );
	}
#endif
#endif

	if( CDnInterface::GetInstance().IsOpenBlind() == false && 
		CDnInterface::GetInstance().IsPVP() == false && 
		CDnInterface::GetInstance().IsPVPLobby() == false &&
		CDnInterface::GetInstance().IsFarm() == false
		)
		RenderPartyMemberMarker(fElapsedTime);

	
	//화면 밖으로 나간 낙인 액터 표시..
	if( CDnInterface::GetInstance().IsOpenBlind() == false && 
		CDnInterface::GetInstance().IsPVPLobby() == false &&
		CDnInterface::GetInstance().IsFarm() == false
		)
		RenderStigmaActorMarker(fElapsedTime);

	// test - bintitle.
	//char strPos[64] = {0,};
	//EtVector3 & vPos = CDnCamera::GetActiveCamera()->GetMatEx()->GetPosition();
	//sprintf_s( strPos, "%f, %f, %f", vPos.x, vPos.y, vPos.z );
	//EternityEngine::DrawText2D( EtVector2(0.6f, 0.2f), strPos );
}

int CDnLocalPlayerActor::GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel )
{
	if( nPrevLevel == nCurLevel ) return 0;

	int nSkillPoint = 0;
	for( int i=nPrevLevel+1; i<=nCurLevel; i++ ) {
		nSkillPoint += CPlayerLevelTable::GetInstance().GetValue( GetJobClassID(), i, CPlayerLevelTable::SkillPoint );
	}
	return nSkillPoint;
}

void CDnLocalPlayerActor::SetWorldColorFilterOnDie()
{
	CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
	if (pWorldEnv)
	{
		float fSaturationFilterValue = 0.f;
		if (GetCurrentGhostType() == Ghost::Type::PartyRestore)
		{
			fSaturationFilterValue = 0.3f;

			EtVector4* pColorCache = pWorldEnv->GetColorHighLight();
			if (pColorCache)
			{
				m_LastHighLightColor = *pColorCache;
				pWorldEnv->ControlColorFilter(CDnWorldEnvironment::Highlight, EtVector4(0.f, 0.f, 30.f, 0.f), 3000);
			}
		}

		pWorldEnv->ControlColorFilter( CDnWorldEnvironment::Saturation, fSaturationFilterValue, 1500 );
	}
}

CDnLocalPlayerActor::eGhostScreenType CDnLocalPlayerActor::IsSetGhostColorFilter() const
{
	CDnWorldEnvironment* pWorldEnv = CDnWorld::GetInstance().GetEnvironment();
	if (pWorldEnv)
	{
		const EtVector4* pHighLight = pWorldEnv->GetColorHighLight();
		if (pHighLight && ((*pHighLight) == m_PartyRestoreHighLightColor))
			return eGhostScreen_PartyRestore;

		float fCurSaturation = pWorldEnv->GetColorSaturation();
		if (fCurSaturation == 0.f)
			return eGhostScreen_Normal;
	}

	return eGhostScreen_None;
}

void CDnLocalPlayerActor::OnDie( DnActorHandle hHitter )
{
	bool bNowCannonMode = IsCannonMode();

	CDnPlayerActor::OnDie( hHitter );

	if( bNowCannonMode )
	{
		EndLocalPlayerCannonMode();
	}

	UpdateDead();
	SetWorldColorFilterOnDie();

	// 메인메뉴에서 닫아야하는 창들 닫는다. 호칭창 등.
	if( GetInterface().GetMainMenuDialog() ) {
		GetInterface().GetMainMenuDialog()->OnDie();
	}

	GetPetTask().DoPetChat( PET_CHAT_USER_DIE );
}

void CDnLocalPlayerActor::OnLevelUp( int nLevel, int nLevelUpAmount )
{
	CDnPlayerActor::OnLevelUp( nLevel, nLevelUpAmount );

	// 로컬 플레이어인 경우엔 보유하고 있지 않은 스킬들까지 전부 툴팁의 
	// 내용이 갱신되어야 하므로 모두 업데이트~
	GetSkillTask().RefreshAllSkillsDecreaseMP();

	// 레벨 올라간만큼 SP 포인트 누적
	int nSkillPoint = GetLevelUpSkillPoint( nLevel - nLevelUpAmount, nLevel );

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	if( CDnItemTask::GetInstance().GetExistLevelUpBox( CDnActor::s_hLocalActor->GetLevel() ) )
	{
		CDnMainDlg* pMainBarDlg = GetInterface().GetMainBarDialog();
		if( pMainBarDlg && pMainBarDlg->IsShow() )
		{
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7765 ), false);	// UISTRING : 레벨업 축하!! 레벨업 보상 상자를 꼭 열어보세요!
			pMainBarDlg->OnLevelUpBoxNotify( true );	
		}
	}

	//RLKT_INSTANT SPECIALIZATION for garcon
	if (nLevel >= 15)
	{
		//GetInterface().ShowInstantSpecializationBtn(true);
	}
#endif 

	for(int nPage=0;nPage<DualSkill::Type::MAX;nPage++)
		GetSkillTask().AddSkillPoint( nSkillPoint , nPage );

	// 월드맵 갱신
	if( GetInterface().GetMainMenuDialog() ) {
		GetInterface().GetMainMenuDialog()->OnLevelUp( nLevel );
	}
	// 던전 입장 알리미 관련 Refresh
	CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( pTask ) {
		pTask->CheckDungeonOpenNotice( nLevel, -1 );
	}
	// 제스처 리프레쉬
	GetGestureTask().RefreshGestureList( nLevel, true );

	CDnMainDlg* pMainBarDlg = GetInterface().GetMainBarDialog();
	if( pMainBarDlg ) 
		pMainBarDlg->OnLevelUp( nLevel );
}

void CDnLocalPlayerActor::OnComboCount( int nValue, int nDelay )
{
	CDnPlayerActor::OnComboCount( nValue, nDelay );
	GetInterface().SetCombo( m_nComboCount, m_nComboDelay );
}

void CDnLocalPlayerActor::OnComboFinish( int nCombo )
{
	/*
	if( nCombo > 1 ) {
		UpdateCombo( nCombo );
	}
	*/
}

void CDnLocalPlayerActor::OnBeginProcessSignal() 
{
	m_bCheckInputSignal = true;
}

void CDnLocalPlayerActor::OnEndProcessSignal() 
{
	m_bCheckInputSignal = true;
}

void CDnLocalPlayerActor::OnChangeAction( const char *szPrevAction )
{
	CDnPlayerActor::OnChangeAction( szPrevAction );

	// 시그널로 체크하는 스킬 사용 조건 초기화
	m_bUseSignalSkillCheck = false;
	CDnMovableChecker::SetUseSignalSkillCheck( false, 0 );
	CDnGroundMovableChecker::SetUseSignalSkillCheck( false, 0 );
	CDnJumpableChecker::SetUseSignalSkillCheck( false, 0 );

	CDnMovableChecker::SetPrimUseSignalSkillCheck( false );
	CDnJumpableChecker::SetPrimUseSignalSkillCheck( false );
	CDnGroundMovableChecker::SetPrimUseSignalSkillCheck( false );


	if(IsVehicleMode() && m_cAutoRun )
	{
#ifdef PRE_ADD_VEHICLE_ACTION_STRING
		std::string strStand , strJumpStand , strJump , strMoveFront;
		
		strStand = GetMyVehicleActor()->GetVehicleActionString() + "Stand";
		strMoveFront = GetMyVehicleActor()->GetVehicleActionString() + "Move_Front";
		strJump = GetMyVehicleActor()->GetVehicleActionString() + "Jump";
		strJumpStand = GetMyVehicleActor()->GetVehicleActionString() + "Jump_Stand";

		switch( m_cAutoRun ) {
			case 1:
				{
					if( strcmp( szPrevAction, GetCurrentAction() ) && (!strcmp( GetCurrentAction(), strStand.c_str() ) || !strcmp( GetCurrentAction(), strJumpStand.c_str() ))  )
						m_cAutoRun = 2;
				}
				break;
			case 2:
				{
					if( !strcmp( GetCurrentAction(), strJump.c_str() ) ) {
						m_cAutoRun = 1;
					}
					else if( strcmp( GetCurrentAction(), strMoveFront.c_str() ) && strcmp( GetCurrentAction(), strStand.c_str() ) )
						ResetAutoRun();
				}
				break;
		}
		return;
#else
		switch( m_cAutoRun ) {
			case 1:
				{
					if( strcmp( szPrevAction, GetCurrentAction() ) && (!strcmp( GetCurrentAction(), "Vehicle_Stand" ) || !strcmp( GetCurrentAction(), "Vehicle_Jump_Stand" ))  )
						m_cAutoRun = 2;
				}
				break;
			case 2:
				{
					if( !strcmp( GetCurrentAction(), "Vehicle_Jump" ) ) {
						m_cAutoRun = 1;
					}
					else if( strcmp( GetCurrentAction(), "Vehicle_Move_Front" ) && strcmp( GetCurrentAction(), "Vehicle_Stand" ) )
						ResetAutoRun();
				}
				break;
		}
		return;
#endif
	}

	if( m_cAutoRun ) 
	{
		switch( m_cAutoRun ) 
		{
			case 1:
				{
					if( strcmp( szPrevAction, GetCurrentAction() ) && ( !strcmp( GetCurrentAction(), "Stand" ) || !strcmp( GetCurrentAction(), "Normal_Stand" ) || !strcmp( GetCurrentAction(), "MOD_Stand" ) ) )
						m_cAutoRun = 2;
				}
				break;
			case 2:
				{
					if( !strcmp( GetCurrentAction(), "Jump" ) || !strcmp( GetCurrentAction(), "Normal_Jump" ) || !strcmp( GetCurrentAction(), "MOD_Jump" )) 
					{
						m_cAutoRun = 1;
					}
					else if( strcmp( GetCurrentAction(), "MOD_Stand" ) && strcmp( GetCurrentAction(), "MOD_Move_Front" ) && 
						strcmp( GetCurrentAction(), "Stand" ) && strcmp( GetCurrentAction(), "Normal_Stand" ) && 
						strcmp( GetCurrentAction(), "Move_Front" ) && strcmp( GetCurrentAction(), "Normal_Move_Front" ) )
					{
						ResetAutoRun();
					}

				}
				break;
		}
	}


	if( m_bClashHit && ( !strcmp( GetCurrentAction(), "Stand" ) || !strcmp( GetCurrentAction(), "Normal_Stand" ) ) ) 
	{
		CmdStop( m_szAction.c_str(), 0, 0.f, 0.f, false );
	}

	if( IsAir() && m_szActionQueue.empty() && GetAddHeight() == 0.f )
	{
		if( strstr( GetCurrentAction(), "Jump_Attack" ) && !strstr( GetCurrentAction(), "_Landing" ) )
		{
			char szStr[64];
			sprintf_s( szStr, "%s_Landing", GetCurrentAction() );
			if( IsExistAction( szStr ) )
			{
				SetActionQueue( szStr, 0, 3.f, 0.f, true, false );
			}
			else 
			{
				SetActionQueue( "Stand", 0, 3.f, 0.f, true, false );
			}
		}
	}

	m_bClashHit = false;
}

void CDnLocalPlayerActor::ResetAutoRun()
{
	if( m_cAutoRun == 2 ) {
		m_cAutoRun = 0;
	}
	m_hFollowActor.Identity();
	m_vFollowPosition = EtVector3( 0.f, 0.f, 0.f );
	ResetMove();
	ResetLook();

	if(IsVehicleMode() && GetMyVehicleActor())
	{
		GetMyVehicleActor()->ResetMove();
		GetMyVehicleActor()->ResetLook();
	}

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
		((CDnPlayerCamera*)hCamera.GetPointer())->FollowCam( false );
	}
}

void CDnLocalPlayerActor::FollowActor( DnActorHandle hFollowActor )
{
	m_vFollowPosition = EtVector3( 0.f, 0.f, 0.f );

	m_hFollowActor = hFollowActor;
	m_fFollowDeltaTime = 0.0f;
}

#ifdef PRE_MOD_NAVIGATION_PATH
void CDnLocalPlayerActor::AutoMovingToPosition( EtVector3 &vPos )
{
	m_bAutoMoving = true;
	m_bEnableNaviMode = false;
	AutoMoving( vPos, 50.0f, "Move_Front", true );
}
#endif // PRE_MOD_NAVIGATION_PATH

void CDnLocalPlayerActor::FollowPosition( EtVector3 &vPos )
{
	ResetAutoRun();
	m_hFollowActor.Identity();

	m_vFollowPosition = vPos;
	m_fFollowDeltaTime = 0.f;
}

bool CDnLocalPlayerActor::IsFollowing()
{
	return ( m_hFollowActor || EtVec3LengthSq( &m_vFollowPosition ) > 0.f );
}

void CDnLocalPlayerActor::ProcessFollow( float fDelta )
{
	bool bStopFollow = false;
	if( m_hFollowActor || EtVec3LengthSq( &m_vFollowPosition ) > 0.f )
	{
		float fMinLength = 50.0f;
		float fMinLengthMargin = 20.0f;
		float fLength;
		EtVector3 vStart, vEnd;
		EtVector2 vLook;

		if(IsVehicleMode())
			fMinLength = 200.f;

		vStart = *GetPosition();
		vEnd = ( m_hFollowActor ) ? *m_hFollowActor->GetPosition() : m_vFollowPosition;
		vLook.x = vEnd.x - vStart.x;
		vLook.y = vEnd.z - vStart.z;
		fLength = EtVec2Length( &vLook );
		vLook /= fLength;

		if( fLength < fMinLength && IsMove() && !IsAir() 
			|| (fLength < fMinLength && IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsMove() && !GetMyVehicleActor()->IsAir()) )
		{
			bStopFollow = true;
		}
		else if( fLength >= fMinLength + fMinLengthMargin && IsMovable() && !IsAir() 
			|| ( fLength >= fMinLength + fMinLengthMargin && IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsMovable() && !GetMyVehicleActor()->IsAir()) )
		{
			m_fFollowDeltaTime += fDelta;
			if( m_fFollowDeltaTime >= 1.0f )	// 1초에 한번씩 길찾기 하도록 한다
			{
				m_fFollowDeltaTime -= 1.0f;
				if( m_hFollowActor )
				{
					MoveTargetNavi( m_hFollowActor, fMinLength, "Move_Front" );
				}
				else 
				{
					MoveTargetNavi( m_vFollowPosition, fMinLength, "Move_Front" );
				}

				// MoveTargetNavi 를 콜하면 m_bEnableNaviMode 가 켜진 후 WalkMovement 쪽에서 이동을 시킵니다.
				// 하지만 이건 로컬만 돌고 동기를 마추지 않기때문에 오로지 ViewSync 덕분에 동기가 맞는것처럼 보이는데요.. ( 에니메이션 싱크가 맞지않습니다. )
				// 여기서 MoveTargetNavi 를 하면 OnMoveNavi 가 호출되므로 현제 이동해야할 위치를 얻고 바로 꺼주는겁니다.
				ResetNaviMode();
			}
		}
	}
#ifdef PRE_MOD_NAVIGATION_PATH
	else if( !m_bAutoMoving ) {
#else // PRE_MOD_NAVIGATION_PATH
	else {
#endif // PRE_MOD_NAVIGATION_PATH
		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();

		if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera && ((CDnPlayerCamera*)hCamera.GetPointer())->IsFollowCam() ) {
			bStopFollow = true;
		}
	}
	if( bStopFollow ) {
		OutputDebug( "StopFollow\n" );
		CmdStop( "Stand" );

		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
		if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			((CDnPlayerCamera*)hCamera.GetPointer())->FollowCam( false );
		}
		m_vFollowPosition = EtVector3( 0.f, 0.f, 0.f );
	}
}

void CDnLocalPlayerActor::OnMoveNavi( EtVector3 &vPosition )
{
	if( ( m_hFollowActor || EtVec3LengthSq( &m_vFollowPosition ) > 0.f ) && IsMovable() && !IsAir() 
		|| (IsVehicleMode() && GetMyVehicleActor() && GetMyVehicleActor()->IsMovable() && !GetMyVehicleActor()->IsAir()) ) 
	{
		/* // 오토런으로 셋팅할 경우
		EtVector3 vDir = vPosition - *GetPosition();
		EtVec3Normalize( &vDir, &vDir );

		m_cAutoRun = 2;
		ReleaseAllButton();
		Look( EtVec3toVec2( vDir ), false );

		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
		if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			((CDnPlayerCamera*)hCamera.GetPointer())->FollowCam( true );
		}
		*/
		// CmdMovePos 추가해서 구현
		EtVector3 vXVec, vZVec;

		vZVec = vPosition - *GetPosition();
		EtVec3Normalize( &vZVec, &vZVec );

		if(IsVehicleMode() && GetMyVehicleActor())
			GetMyVehicleActor()->Look(EtVec3toVec2( vZVec ), false);
		else
			Look( EtVec3toVec2( vZVec ), false );

		EtVec3Cross( &vXVec, &vZVec, &EtVector3( 0.f, 1.f, 0.f ) );
		vXVec.y = vZVec.y = 0.f;
		EtVec3Normalize( &vXVec, &vXVec );
		EtVec3Normalize( &vZVec, &vZVec );
		SetMoveVectorX( vXVec );
		SetMoveVectorZ( vZVec );

		EtVector3 vTempPos = vPosition;
		vTempPos += vZVec * 100.f; // 패킷이 느리게 가거나 했을 때 멈췄다 이동하게 되는게 어색하므로 약간의 값을 더 주도록 합니다.

		if(IsVehicleMode() && GetMyVehicleActor())
		{
			GetMyVehicleActor()->SetMoveVectorX( vXVec );
			GetMyVehicleActor()->SetMoveVectorZ( vZVec );
			GetMyVehicleActor()->CmdMove( vTempPos, "Move_Front", -1, 3.f );
		}
		else
		{
			CmdMove( vTempPos, "Move_Front", -1, 3.f );
		}

		DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
		if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
			((CDnPlayerCamera*)hCamera.GetPointer())->FollowCam( true );
		}
	}
}

#ifdef PRE_MOD_NAVIGATION_PATH
void CDnLocalPlayerActor::OnAutoMoving( EtVector3& vPosition, EtVector3& vCurrentPosition )
{
	EtVector3 vXVec, vZVec;

	vZVec = vPosition - vCurrentPosition;
	EtVec3Normalize( &vZVec, &vZVec );
		Look( EtVec3toVec2( vZVec ), false );

	
	// 흔들리는 문제 있음

//	EtVector2 vec2 = EtVec3toVec2( vZVec );
//	OutputDebug( "Pos(%d, %d, %d) Tar(%d, %d) Look (%d, %d) \n", (*GetPosition()).x, (*GetPosition()).y, (*GetPosition()).z,
//		vPosition.x, vPosition.y, vPosition.z, vec2.x, vec2.y );


	CmdMove( vPosition, "Move_Front", -1, 3.f );

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera )
		((CDnPlayerCamera*)hCamera.GetPointer())->FollowCam( true );
}
#endif // PRE_MOD_NAVIGATION_PATH

void CDnLocalPlayerActor::OnStop( EtVector3 &vPosition )
{
	if( m_hFollowActor || EtVec3LengthSq( &m_vFollowPosition ) > 0.f )
	{
		const float fMinLength = 50.0f;
		const float fMinLengthMargin = 20.0f;
		float fLength;
		EtVector3 vStart, vEnd;
		EtVector2 vLook;

		vStart = *GetPosition();
		vEnd = ( m_hFollowActor ) ? *m_hFollowActor->GetPosition() : m_vFollowPosition;
		vLook.x = vEnd.x - vStart.x;
		vLook.y = vEnd.z - vStart.z;
		fLength = EtVec2Length( &vLook );
		vLook /= fLength;
		if( fLength >= fMinLength + fMinLengthMargin && IsMovable() && !IsAir() )
		{
			m_fFollowDeltaTime = 0.f;
			if( m_hFollowActor )
				MoveTargetNavi( m_hFollowActor, fMinLength, "Move_Front" );
			else MoveTargetNavi( m_vFollowPosition, fMinLength, "Move_Front" );
			ResetNaviMode();
			OutputDebug( "OnStop Follow Succ\n" );
			return;
		}
	}
	OutputDebug( "OnStop Follow Fail\n" );
	CDnPlayerActor::OnStop( vPosition );
}

void CDnLocalPlayerActor::StopAllPartyPlayer()
{	
	if( s_hLocalActor ) {
		s_hLocalActor->ResetCustomAction();
		s_hLocalActor->ResetMixedAnimation( false );
		((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->ResetAutoRun();

		if( !s_hLocalActor->IsDie() && ( s_hLocalActor->IsMove() || s_hLocalActor->IsStay() ) ) {
			s_hLocalActor->CmdAction( "Stand" );		// #22468 대쉬 스킬 중에는 CmdStop 으로 액션이 바뀌지 않아서 먼저 Stand 로 바꾼 후 CmdStop 을 호출해주도록 한다.
			s_hLocalActor->CmdStop( "Stand", 0, 3.f, 0.f, true );
			((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->OnStopReady();
		}

		if(((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->IsVehicleMode() && ((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->GetMyVehicleActor())
		{
			((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->GetMyVehicleActor()->CmdStop("Stand", 0, 3.f, 0.f, true);
		}

		((CDnLocalPlayerActor*)s_hLocalActor.GetPointer())->ReleaseAllButton();
	}
}

void CDnLocalPlayerActor::ResetCameraLook( bool bResetWhenActivated )
{
	DnCameraHandle hPlayerCamera = CDnCamera::FindCamera( CDnCamera::PlayerCamera );
	if( hPlayerCamera )
	{
		CDnPlayerCamera* pPlayerCamera = static_cast<CDnPlayerCamera*>(hPlayerCamera.GetPointer());
		if( CDnCamera::GetActiveCamera() == hPlayerCamera )
		{
			pPlayerCamera->ResetLook();
		}
		else if( bResetWhenActivated = true ) // 현재 카메라가 PlayerCamera가 아닐때 예약해놓는 용도
		{
			pPlayerCamera->ResetLookWhenActivated();
		}
	}
}

void CDnLocalPlayerActor::OnBlindClosed()
{
	bool bOpenRebirthCaption = false;
	if( IsDie() && IsGhost() && !GetInterface().IsShowRebirthCationDlg() && CDnItemTask::GetInstance().IsCanRebirth() )
		bOpenRebirthCaption = true;

	if (GetCurrentGhostType() == Ghost::Type::PartyRestore)
		bOpenRebirthCaption = true;

	if (bOpenRebirthCaption)
		GetInterface().OpenRebirthCaptionDlg();

	if( IsDie() && IsGhost() && !GetInterface().IsShowRebirthFailDlg() ) {
		GetInterface().RefreshRebirthInterface();
	}
}

void CDnLocalPlayerActor::OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash )
{
	CDnPlayerActor::OnClash( vPosition, bFloorClash );
	if( IsHit() && ( IsAir() || IsDown() ) && !IsStun() ) m_bClashHit = true;
	if (bFloorClash == false)
		m_CancelStageMoveDelta = ENTER_GATE_CANCEL_LIMIT_TIME;
}

bool CDnLocalPlayerActor::AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete, bool bUseCombine )
{
	if( !hParts )
		return false;

	bool bResult = CDnPlayerActor::AttachParts( hParts, Index, bDelete, bUseCombine );

	return bResult;
}

bool CDnLocalPlayerActor::DetachParts( CDnParts::PartsTypeEnum Index )
{
	// 무기와 달리 파츠는 일반파츠가 빠지더라도 강제로 캐시를 보이게 하지 않는다. 5파츠 뿐만 아니라 반지도 포함이다.
	//if( m_hPartsObject[ Index ] ) {
	//	CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
	//	if( pItemTask && !pItemTask->IsRequestRepair() ) {
	//		if( GetCashParts( (CDnParts::PartsTypeEnum)(Index-2) ) && IsViewPartsOrder( Index-2 ) == false )
	//			CmdTogglePartsViewOrder( Index-2, true );
	//	}
	//}

	// Note: 파츠 분리에 실패한 경우에도 스킬은 사라질 수 있음.
	bool bResult = CDnPlayerActor::DetachParts( Index );
	return bResult;
}

bool CDnLocalPlayerActor::AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete, bool bUseCombine )
{
	bool bResult = CDnPlayerActor::AttachCashParts( hParts, Index, bDelete, bUseCombine );

	// 무기와 달리 파츠는 캐시장비를 장착하더라도 강제로 캐시를 보이게 하지 않는다. 5파츠 뿐만 아니라 반지도 포함이다.
	// 이런식으로 바뀌면서 무기에 있던 bRefreshOrder 는 안쓰게 되었다.
	//if( bRefreshPartsOrder ) {		
	//	CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
	//	if( pItemTask && !pItemTask->IsRequestRepair() ) {
	//		if( IsViewPartsOrder( Index ) == false )
	//			CmdTogglePartsViewOrder( Index, true );
	//	}
	//}

	return bResult;
}

bool CDnLocalPlayerActor::DetachCashParts( CDnParts::PartsTypeEnum Index )
{
	// Note: 파츠 분리에 실패한 경우에도 스킬은 사라질 수 있음.
	bool bResult = CDnPlayerActor::DetachCashParts( Index );
	return bResult;
}

void CDnLocalPlayerActor::ReplacementGlyph( DnSkillHandle hNewSkill )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return;

	int iSkillID = hNewSkill->GetClassID();
	
	for( int itr = 0; itr < CDnGlyph::GlyphSlotEnum_Amount; ++itr )
	{
		if( m_hGlyph[itr] )
		{
			int eType = pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_GlyphType" )->GetInteger();
			int iGlyphSkillID = pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_SkillID" )->GetInteger();

			if( CDnGlyph::PassiveSkill == eType && iSkillID == iGlyphSkillID )
				hNewSkill->AddGlyphStateEffect( m_hGlyph[itr]->GetClassID() );
		}
	}
}



// 이미 문장은 장착 되었고 다음 페이지의 정보를 받는경우  < OnRecvSkillList >
// 다음 페이지의 스킬에만 문장 스킬을 적용 시켜주기만 하면된다. 

bool CDnLocalPlayerActor::ApplyGlyphToExpendedSkillPage( DnGlyphHandle hGlyph , int nSkillPage )
{
	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
	
	if( !hGlyph || !pSkillTask )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return false;

	int eType = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_GlyphType" )->GetInteger();

	if( CDnGlyph::PassiveSkill == eType )
	{
		int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

		DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( nSkillID , nSkillPage );
		if( !hSkill )
			return false;
		
		hSkill->AddGlyphStateEffect( hGlyph->GetClassID() );
	}
	else if( CDnGlyph::ActiveSkill == eType && 0 != hGlyph->GetSkillID() &&  0 != hGlyph->GetSkillLevel() )
	{
		pSkillTask->AddSkill( hGlyph->GetSkillID(), hGlyph->GetSkillLevel() , nSkillPage );

		// SkillTask 에 있는 스킬리스트가 MASkillUser에게 나중에 셋팅됨.
		DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( hGlyph->GetSkillID() , nSkillPage );
		if( !hSkill )
			return false;

		hSkill->AsEquipItemSkill();

		m_ahEquipSkill = hSkill;

		if( 0.0f != m_afLastEquipItemSkillDelayTime )
		{
			hSkill->SetOnceCoolTime( m_afLastEquipItemSkillDelayTime, m_afLastEquipItemSkillRemainTime );
		}

		GetInterface().RefreshPlateDialog();
	}

	return true;
}

bool CDnLocalPlayerActor::AttachGlyph( DnGlyphHandle hGlyph, CDnGlyph::GlyphSlotEnum Index, bool bDelete /* = false  */ )
{
	if( !MAPlateUser::AttachGlyph( hGlyph, Index, bDelete ) )
		return false;

#if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	if( GLYPH_CASH1 <= Index && GLYPH_CASH3 >= Index )
#else
	if( GLYPH_SPECIALSKILL < Index && GLYPHMAX > Index )
#endif	// #if defined(PRE_ADD_DRAGON_FELLOWSHIP_GLYPH)
	{
		int nCashIndex = Index - GLYPH_CASH1;
		if( false == CDnItemTask::GetInstance().IsCashGlyphOpen( nCashIndex ) )
			return false;
	}

	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return false;

	int eType = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_GlyphType" )->GetInteger();

	// 문장에 스킬이 존재 할 경우 문장이 스킬추가 인지 스킬효과추가 인지 알아 낸다.
	if( CDnGlyph::PassiveSkill == eType )
	{
		int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

		bool bSuccessed = false;

		for(int nPage = DualSkill::Type::Primary; nPage < DualSkill::Type::MAX; nPage++ )
		{
			DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( nSkillID , nPage );
			if( hSkill )
			{
				hSkill->AddGlyphStateEffect( hGlyph->GetClassID() );
				bSuccessed = true;
			}
		}

		if( !bSuccessed ) 
			return false;
	}
	else if( CDnGlyph::ActiveSkill == eType && 0 != hGlyph->GetSkillID() &&  0 != hGlyph->GetSkillLevel() )
	{

		bool bSuccessed = false;
		
		for(int nPage = DualSkill::Type::Primary; nPage < DualSkill::Type::MAX; nPage++ )
		{
			pSkillTask->AddSkill( hGlyph->GetSkillID(), hGlyph->GetSkillLevel() , nPage );

			DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( hGlyph->GetSkillID() , nPage );
			if( hSkill )
			{
				hSkill->AsEquipItemSkill();
				m_ahEquipSkill = hSkill;

				if( 0.0f != m_afLastEquipItemSkillDelayTime )
				{
					hSkill->SetOnceCoolTime( m_afLastEquipItemSkillDelayTime, m_afLastEquipItemSkillRemainTime );
				}
	
				bSuccessed = true;
			}
		}

		if( !bSuccessed ) 
			return false;
	
		GetInterface().RefreshPlateDialog();
	}

	return true;
}

bool CDnLocalPlayerActor::DetachGlyph( CDnGlyph::GlyphSlotEnum Index )
{
	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );

	DnGlyphHandle hGlyph = m_hGlyph[Index];
	if( !hGlyph )	return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) return false;

	int eType = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_GlyphType" )->GetInteger();

	if( CDnGlyph::PassiveSkill == eType )
	{
		int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

		bool bSuccessed = false;
		for(int nPage = DualSkill::Type::Primary; nPage < DualSkill::Type::MAX; nPage++ )
		{
			DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( nSkillID , nPage );

			if(hSkill)
			{
				bSuccessed = true;
				hSkill->DelGlyphStateEffect( hGlyph->GetClassID() );
			}
		}

		if( !bSuccessed ) 
			return false;
	}
	else if( CDnGlyph::ActiveSkill == eType && 0 != hGlyph->GetSkillID() && 0 != hGlyph->GetSkillLevel() )
	{

		bool bSuccessed = false;
		for(int nPage = DualSkill::Type::Primary; nPage < DualSkill::Type::MAX; nPage++ )
		{
			DnSkillHandle hSkill = pSkillTask->FindSkillBySkillPage( hGlyph->GetSkillID() , nPage );
			if( hSkill )
			{
				m_afLastEquipItemSkillDelayTime = hSkill->GetDelayTime();
				m_afLastEquipItemSkillRemainTime = hSkill->GetElapsedDelayTime();

				pSkillTask->DelSkill( hGlyph->GetSkillID() , nPage );

				bSuccessed = true;
			}
		}

		if( !bSuccessed ) 
			return false;

		GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
	}

	if( !MAPlateUser::DetachGlyph(Index) )	return false;

	return true;
}

void CDnLocalPlayerActor::_OnAttachEquip( CDnItem* pItem, int Index )
{
	if( CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+NUM_WEAPON <= Index )
		return;

	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );

	// #36908, #36693, #37625 
	// 발사체 자체에 추가 사거리 정보가 있거나,
	// RangeFallGravity 발사체를 쏘는 시그널이 있는 경우 GlobalWeightTable 에서 추가 사거리 가중치값을 얻어와서 적용.
	m_fNormalProjectileAdditionalRange = 0.0f;
	float fLongestAdditionalRange = 0.0f;
	if( m_hWeapon[ 0 ] )
	{
		ActionElementStruct* pAction = m_hWeapon[ 0 ]->GetElement( "Shoot" );
		if( pAction )
		{
			for( int i = 0; i < (int)pAction->pVecSignalList.size(); ++i )
			{
				CEtActionSignal* pSignal = pAction->pVecSignalList.at( i );
				if( SignalTypeEnum::STE_Projectile == pSignal->GetSignalIndex() )
				{
					fLongestAdditionalRange = 0.0f;
					ProjectileStruct* pProjectileSignal = reinterpret_cast<ProjectileStruct*>(pSignal->GetData());
					if( CDnProjectile::RangeFallGravity == pProjectileSignal->nDestroyOrbitType )
					{
						// 기존 사거리에 더하게 되므로.. 값이 그대로 곱하는 것 기준으로 들어가 있다.
						fLongestAdditionalRange += (float)m_hWeapon[ 0 ]->GetWeaponLength() * 
							(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::RangeFallGravityAdditionalProjectileRange ) - 1.0f);
					}
		
					if( TRUE == pProjectileSignal->bIncludeMainWeaponLength &&
						-1 != pProjectileSignal->nProjectileIndex )
					{
						DnWeaponHandle hProjectileWeapon = CDnWeapon::GetSmartPtr( pProjectileSignal->nProjectileIndex );
						fLongestAdditionalRange += (float)hProjectileWeapon->GetWeaponLength();
					}
				}

				if( m_fNormalProjectileAdditionalRange < fLongestAdditionalRange )
					m_fNormalProjectileAdditionalRange = fLongestAdditionalRange;
			}
		}
	}
}


void CDnLocalPlayerActor::_OnDetachEquip( CDnItem* pItem, int Index )
{
	CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
	m_fNormalProjectileAdditionalRange = 0.0f;
}

void CDnLocalPlayerActor::SetEquipCoolTime( const int aiDelayTime, const int aiRemainTime, int iEquipCount )
{
	m_afLastEquipItemSkillDelayTime = (float)aiDelayTime / 1000.0f;
	m_afLastEquipItemSkillRemainTime = (float)aiRemainTime / 1000.0f;

	// 현재 장비 슬롯에 매치되는 스킬이 있다면 쿨타임 셋팅해줌.
	if( m_ahEquipSkill )
		m_ahEquipSkill->SetOnceCoolTime( m_afLastEquipItemSkillDelayTime, m_afLastEquipItemSkillRemainTime );
}


void CDnLocalPlayerActor::OnEventCP( CPTypeEnum Type, int nResult )
{
#if defined( PRE_ADD_CP_RENEWAL )
	MACP_Renewal::OnEventCP( Type, nResult );
#else	// #if defined( PRE_ADD_CP_RENEWAL )
	MACP::OnEventCP( Type, nResult );
#endif	// #if defined( PRE_ADD_CP_RENEWAL )
	switch( Type ) {
		case MACP::MaxComboCount: break;
		case MACP::KillBossCount: break;
		case MACP::KillMonsterScore:	GetInterface().SetCpJudge( CDnInterface::CpType_FinishAttack, nResult ); break;
		case MACP::CriticalHitScore:	GetInterface().SetCpJudge( CDnInterface::CpType_Critical, nResult ); break;
		case MACP::StunHitScore:		GetInterface().SetCpJudge( CDnInterface::CpType_Stun, nResult ); break;
		case MACP::SuperAmmorBreakScore: GetInterface().SetCpJudge( CDnInterface::CpType_SuperArmorAttack, nResult ); break;
		case MACP::BrokenShieldScore:	GetInterface().SetCpJudge( CDnInterface::CpType_BrokenShield, nResult ); break;
		case MACP::GenocideScore:		GetInterface().SetCpJudge( CDnInterface::CpType_Genocide, nResult ); break;
		case MACP::AirComboScore:		GetInterface().SetCpJudge( CDnInterface::CpType_AirCombo, nResult ); break;
		case MACP::PartyComboScore:		GetInterface().SetCpJudge( CDnInterface::CpType_SkillCombo, nResult ); break;
		case MACP::RebirthPlayerScore:	GetInterface().SetCpJudge( CDnInterface::CpType_Rescue, nResult ); break;
		case MACP::UseSkillScore:		GetInterface().SetCpJudge( CDnInterface::CpType_SkillScore, nResult ); break;
		case MACP::ComboScore:			GetInterface().SetCpJudge( CDnInterface::CpType_Combo, nResult ); break;
		case MACP::PropBreakScore:		GetInterface().SetCpJudge( CDnInterface::CpType_PropBreak, nResult ); break;
		case MACP::GetItemScore:		GetInterface().SetCpJudge( CDnInterface::CpType_GetItem, nResult ); break;
		case MACP::AttackedCount: break;
		case MACP::AttackedHitScore:
		case MACP::AttackedCriticalHitScore:
		case MACP::AttackedStunHitScore:
			GetInterface().SetCpDown( nResult );
			break;
		case MACP::DieCount: GetInterface().SetCpDown( nResult ); break;
		case MACP::AssistMonsterScore:	GetInterface().SetCpJudge( CDnInterface::CpType_AssistKillMonster, nResult ); break;
	}
	OutputDebug( "CP : %d - %d\n", Type, m_Data.nAccumulationCP );
}

void CDnLocalPlayerActor::UpdateAttackedCPPoint( CDnDamageBase *pHitter , CDnWeapon::HitTypeEnum eHitType )
{
	bool bIsSameTeam = false;
	if( pHitter && pHitter->GetActorHandle() )
	{
		if( GetTeam() == pHitter->GetActorHandle()->GetTeam() )
		{
			bIsSameTeam = true;
		}
	}

	if( bIsSameTeam == false )
	{
		switch( eHitType ) 
		{
		case CDnWeapon::Normal: 
		case CDnWeapon::CriticalRes: 
			{
				UpdateAttackedHit();
			}
			break;

		case CDnWeapon::Critical: 
			{
				UpdateAttackedCriticalHit(); 
			}
			break;
		case CDnWeapon::Stun: 
			{
				UpdateAttackedStunHit();
			}
			break;
		}
	}
}

void CDnLocalPlayerActor::ProcessAccountImp( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDeveloperAccountLevel() == false ) return;

	SetAlphaBlend( ( m_bAccountHide == true ) ? 0.5f : 1.f );
}

void CDnLocalPlayerActor::SetInteractivePos( EtVector3 *vPos, float fDelta )
{
	EtTerrainHandle hTerrain = CEtTerrainArea::GetTerrainArea( vPos->x, vPos->z );
	if( hTerrain ) {
		hTerrain->SetInteractivePos( vPos, fDelta );
	}
}

void CDnLocalPlayerActor::OnChangeJob( int nJobID )
{
	CDnPlayerActor::OnChangeJob( nJobID );

	// #26902 임시로 2차 전직을 사용할 수 있게 해주는 기능이 생겨서 직업이 롤백되는 경우가 생겼으므로
	// 현재 내 로컬 플레이어가 직업을 새로 얻었을 경우에만 스킬트리에 새 직업의 스킬트리를 추가해준다.
	if( IsPassJob( nJobID ) )
		GetSkillTask().AddJobSkillTree( nJobID );
	else
		GetSkillTask().RemoveJobSkillTree( nJobID );

	GetInterface().RefreshSkillDialog();

	UpdateSPUsingLimitByJob();
}

void CDnLocalPlayerActor::OnCalcPlayerState()
{
#ifdef PRE_FIX_CHARSTATUS_REFRESH
	GetInterface().OnRefreshLocalPlayerStatus();
#endif
}

bool CDnLocalPlayerActor::ChangeSocialTexture( int nSocialIndex, int nFrameIndex )
{
	bool bResult = CDnPlayerActor::ChangeSocialTexture( nSocialIndex, nFrameIndex );
	
	if( IsFollowObserver() )
		bResult = false;

	if( bResult ) {
		CDnInterface::GetInstance().UpdateMyPortrait();
	}
	return bResult;
}

void CDnLocalPlayerActor::OnCancelStage(const EtVector3& targetDir)
{
	m_bCancelStageMove = true;
	m_CancelStageMoveDelta = 0.f;

	EtVector3* pLookDir = GetLookDir();
	if (pLookDir)
	{
		float ret = EtVec3Dot(pLookDir, &targetDir);
		m_CancelStageMoveActionName = (ret < 0) ? "Move_Back" : "Move_Front";
		m_CancelStageMoveDir = (ret < 0) ? -1.f : 1.f;

		LockInput(true);
	}
}

#ifdef PRE_ADD_VIP
bool CDnLocalPlayerActor::IsVIP() const
{
	return m_VIPDataMgr.IsVIP();
}

time_t CDnLocalPlayerActor::GetVIPTime()
{
	time_t tTime;

	if( NULL != m_VIPDataMgr.GetVIPExpireDate() )
		tTime = *(m_VIPDataMgr.GetVIPExpireDate());

	return tTime;
}
#endif

#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)
bool CDnLocalPlayerActor::IsVipFarm()
{
#if defined(_CH) || defined(_TW)
	if( m_VIPDataMgr.IsVIP() )
		return true;
#else
	time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
	time_t tFarmVIPTime = CDnItemTask::GetInstance().GetVipFarmTime();
#if defined(PRE_ADD_VIP)
	time_t tVIPTime = *m_VIPDataMgr.GetVIPExpireDate();
#endif

	if( pNowTime < tFarmVIPTime 
#if defined(PRE_ADD_VIP)
		|| pNowTime < tVIPTime 
#endif
		)
		return true;
#endif

	return false;
}
#endif	//#if defined(PRE_ADD_VIP_FARM) || defined(PRE_ADD_FARM_DOWNSCALE)


#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
void CDnLocalPlayerActor::UseNpcVoicePlayer( CDnItem* pNpcVoicePlayerItem )
{
	_ASSERT( pNpcVoicePlayerItem );
	
	// 로드된 사운드 파일의 인덱스
	vector<int> vlLoadedNpcVoice;
	for( int i = 0; i < 2; ++i )
	{
		int nNpcID = pNpcVoicePlayerItem->GetTypeParam( i );

		DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TNPC );
		for( int k = 1; k <= 4; k++ ) 
		{
			char szLabel[128] = { 0,} ;
#ifdef PRE_FIX_MEMOPT_EXT
			if (!pSox)
				break;

			std::string szSoundFile;
			sprintf_s( szLabel, "_BeginTalk%d", k );
			CommonUtil::GetFileNameFromFileEXT(szSoundFile, pSox, nNpcID, szLabel);
#else
			sprintf_s( szLabel, "_BeginTalk%d", k );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
#endif
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), false, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 파일 : %S\n", nNpcID , szSoundFile.c_str() );
					{
						ScopeLock< CSyncLock > Lock( g_pEtRenderLock );
						GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
					}
#endif
					continue;
				}

				vlLoadedNpcVoice.push_back( nSoundIdx );
			}
		}

		for( int k = 1; k <= 4; k++ ) 
		{
			char szLabel[128] = { 0,} ;
#ifdef PRE_FIX_MEMOPT_EXT
			std::string szSoundFile;
			sprintf_s( szLabel, "_FinishTalk%d", k );
			CommonUtil::GetFileNameFromFileEXT(szSoundFile, pSox, nNpcID, szLabel);
#else
			sprintf_s( szLabel, "_FinishTalk%d", k );
			if ( pSox->GetFieldFromLablePtr( nNpcID, szLabel ) == NULL )
				continue;

			std::string szSoundFile = pSox->GetFieldFromLablePtr( nNpcID, szLabel )->GetString();
#endif
			if ( !szSoundFile.empty() )
			{
				int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szSoundFile.c_str(), false, false );
				if( nSoundIdx == -1 ) {
#ifndef _FINAL_BUILD
					WCHAR wszStr[256];
					swprintf_s( wszStr, L"사운드 파일을 찾을 수 없습니다. : NpcID : %d, 파일 : %S\n", nNpcID, szSoundFile.c_str() );
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", wszStr, false);
#endif
					continue;
				}

				vlLoadedNpcVoice.push_back( nSoundIdx );
			}
		}
	}

#ifdef PRE_FIX_MEMOPT_EXT
	if (vlLoadedNpcVoice.empty())
		return;
#endif

	int iPlayIndex = _rand() % (int)vlLoadedNpcVoice.size();
	if( iPlayIndex < -1 )
		return;

	if( m_hNowPlayingNpcVoice )
	{
		if( m_hNowPlayingNpcVoice->IsPlay() )
		{
			return;
		}
		else
		{
			m_hNowPlayingNpcVoice.Identity();
		}
	}

	// 3d 사운드가 아니니까 일단 걍 플레이해 보자.
	m_hNowPlayingNpcVoice = CEtSoundEngine::GetInstance().PlaySound__( "2D", vlLoadedNpcVoice.at(iPlayIndex), false/*, true*/ );

}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

void CDnLocalPlayerActor::UpdateSPUsingLimitByJob( void )
{
	// 직업 테이블에서 sp 사용 제한 정보를 얻어옴.
	m_vlSPUSingLimitByJob.clear();

	vector<int> vlJobHistory;
	GetJobHistory( vlJobHistory );

	// 1차일때는 하나만, 2차일때는 2개 얻어온다.
	DNTableFileFormat*  pJobTable = GetDNTable( CDnTableDB::TJOB );
	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < (int)vlJobHistory.size(); ++i )
	{
		sprintf_s( acBuffer, "_MaxSPJob%d", i );
		float fSPUsingLimit = pJobTable->GetFieldFromLablePtr( vlJobHistory.back(), acBuffer )->GetFloat();

		m_vlSPUSingLimitByJob.push_back( fSPUsingLimit );
	}
}

int CDnLocalPlayerActor::GetSPUsingLimitByJobArrayIndex( int iJobArrayIndex )
{
	if( iJobArrayIndex < 0 )
		return 0;

	int iResult = 0;

	if( iJobArrayIndex < (int)m_vlSPUSingLimitByJob.size() )
	{
		// 비율로 직업별 사용할 수 있는 포인트 계산.
		int iWholeSP = GetSkillTask().GetWholeUsedSkillPoint() + GetSkillTask().GetSkillPoint();
		iResult = int(iWholeSP * m_vlSPUSingLimitByJob.at( iJobArrayIndex ));
	}

	return iResult;
}

int CDnLocalPlayerActor::GetJobArrayIndex( const int nJobID )
{
	for( DWORD itr = 0; itr < (int)m_nVecJobHistoryList.size(); ++itr )
	{
		if( nJobID == m_nVecJobHistoryList.at( itr ) )
			return itr;
	}

	return -1;
}

bool CDnLocalPlayerActor::CheckSyncChangeActionOperatorString( int nOperator, char *szValue1, char *szValue2 )
{
	if( nOperator == 0 ) return true;

	int nResult = strcmp( szValue1, szValue2 );
	switch( nOperator ) {
		case 1: return ( nResult == 0 ) ? true : false;
		case 2: return ( nResult != 0 ) ? true : false;
		case 3: return ( nResult > 0 ) ? true : false;
		case 4: return ( nResult >= 0 ) ? true : false;
		case 5: return ( nResult < 0 ) ? true : false;
		case 6: return ( nResult <= 0 ) ? true : false;
	}
	return false;
}

bool CDnLocalPlayerActor::CheckSyncChangeActionOperatorInt( int nOperator, int nValue1, int nValue2 )
{
	if( nOperator == 0 ) return true;

	switch( nOperator ) {
		case 1: return ( nValue1 == nValue2 ) ? true : false;
		case 2: return ( nValue1 != nValue2 ) ? true : false;
		case 3: return ( nValue1 > nValue2 ) ? true : false;
		case 4: return ( nValue1 >= nValue2 ) ? true : false;
		case 5: return ( nValue1 < nValue2 ) ? true : false;
		case 6: return ( nValue1 <= nValue2 ) ? true : false;
	}
	return false;
}

bool CDnLocalPlayerActor::CheckSyncChangeActionOperatorFloat( int nOperator, float fValue1, float fValue2 )
{
	if( nOperator == 0 ) return true;

	switch( nOperator ) {
		case 1: return ( fValue1 == fValue2 ) ? true : false;
		case 2: return ( fValue1 != fValue2 ) ? true : false;
		case 3: return ( fValue1 > fValue2 ) ? true : false;
		case 4: return ( fValue1 >= fValue2 ) ? true : false;
		case 5: return ( fValue1 < fValue2 ) ? true : false;
		case 6: return ( fValue1 <= fValue2 ) ? true : false;
	}
	return false;
}

bool CDnLocalPlayerActor::CheckSyncChangeActionCondition( int nCondition, int nOperator, char *szValue )
{
	if( nCondition == 0 ) return true;

	char cValueType = 0;
	int nCondtionValue = 0;
	float fConditionValue = 0.f;
	std::string szConditionValue;

	switch( nCondition ) {
		case 1:	// Current Job
			cValueType = 0;
			nCondtionValue = GetJobClassID();
			break;
	}

	switch( cValueType ) {
		case 0: return CheckSyncChangeActionOperatorInt( nOperator, nCondtionValue, atoi( szValue ) );
		case 1: return CheckSyncChangeActionOperatorFloat( nOperator, fConditionValue, (float)atof( szValue ) );
		case 2: return CheckSyncChangeActionOperatorString( nOperator, (char*)szConditionValue.c_str(), szValue );
	}

	return true;
}

void CDnLocalPlayerActor::ProcessPVPChanges(LOCAL_TIME LocalTime, float fDelta)
{
	CDnPlayerActor::ProcessPVPChanges(LocalTime,fDelta);

	bool bUseObservation = false;

	if(IsObserver()) bUseObservation = true;
	if(IsSpectatorMode()) bUseObservation = true;
	if( bUseObservation )
	{
		if(IsObserver())
		{
			if(!IsGhost())
				SetHP(0);
			if(IsShow())
				Show(false); 

			if( GetInterface().GetChatTabDialog() ) { // 대상이 자신일때만 
				if(!GetInterface().GetChatTabDialog()->IsCustomizeChatMode())
					GetInterface().GetChatTabDialog()->CustomizeChatMode(true,CHAT_PARTY,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121083),chatcolor::OBSERVER); // True
			}
		}

		ProcessAttachObserver(); // 옵져버 따라가기

	}
	else
	{
		if( GetInterface().GetChatTabDialog() ) { // 대상이 자신일때만 
			if(GetInterface().GetChatTabDialog()->IsCustomizeChatMode())
				GetInterface().GetChatTabDialog()->CustomizeChatMode(false,CHAT_PARTY,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121083),chatcolor::OBSERVER); // false
		}
	}
}

void CDnLocalPlayerActor::SetFollowObserver(bool bTrue,int nUniqueID)
{
	m_bFollowObserver = bTrue; 
	m_nObserverUniqueID = nUniqueID;

	if(IsObserver() )
		GetInterface().GetPlayerGauge()->Show(bTrue);

	if(bTrue == false)
		CDnInterface::GetInstance().UpdateMyPortrait();
}


void CDnLocalPlayerActor::ProcessAttachObserver()
{
	if(!m_bFollowObserver || (m_nObserverUniqueID==0))
	{
		if(IsObserver() && GetInterface().GetPlayerGauge() && GetInterface().GetPlayerGauge()->IsShow())
			GetInterface().GetPlayerGauge()->Show(false);
		return;
	}

	CDnActor *pActor = CDnActor::FindActorFromUniqueID(m_nObserverUniqueID);
	if(!pActor) // 액터 생성 실패 !
		return;

	if(GetInterface().GetPlayerGauge() && !GetInterface().GetPlayerGauge()->IsShow())
	{
		if(!GetInterface().GetPlayerGauge()->IsShow())
			GetInterface().GetPlayerGauge()->Show(true);

		GetInterface().UpdateTargetPortrait(pActor->GetActorHandle());
	}

	if( IsObserver() )
		SetPosition(*pActor->GetPosition());
}

void CDnLocalPlayerActor::SendPossessCannonReq( void )
{
	// 대포 뒤에 붙여준다. 그리고 이동불가.
	// 지금은 일단 대충 적당히 붙여보지만.. 나중엔 더미 오브젝트나 본이 있어야 할 듯 싶다.
	DnActorHandle hCannonMonsterActor = GetLastAimActor();
	if(!hCannonMonsterActor)
		return;

	EtVector3 vCannonDir = hCannonMonsterActor->GetMatEx()->m_vZAxis;
	EtVec3Normalize( &vCannonDir, &vCannonDir );

	MatrixEx Cross = m_matexWorld;

	// 바운딩 박스 말고 Press 로 밀어내야 한다. 
	// 대포쪽으로 밀어주고 press 값 얻어온다.
	// 양쪽 다 press circle 이 있는 것으로 가정한다.
	Cross.m_vPosition = hCannonMonsterActor->GetMatEx()->m_vPosition - (vCannonDir*20.0f);
	EtVector2 vDir;
	float vDist = 0.0f;
	_ASSERT( GetPress() == CDnActorState::Press_Circle && hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle );
	if( GetPress() == CDnActorState::Press_Circle && hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle )
	{
		if( CheckPressCircle2Clrcle2( hCannonMonsterActor, GetMySmartPtr() , vDir, vDist ) ) 
		{
			Cross.m_vPosition.x += vDir.x*vDist; //MoveToWorld( vDir * vDist );
			Cross.m_vPosition.z += vDir.y*vDist;
		}
	}

	// 캐릭터가 바라보는 방향도 대포의 방향으로 셋팅.
	CmdLook( EtVector2(vCannonDir.x, vCannonDir.z), true );

	// 서버로 요청을 보냄.
	DWORD dwCannonActorID = hCannonMonsterActor->GetUniqueID();

	BYTE pBuffer[128] = { 0, };
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &dwCannonActorID, sizeof(DWORD) );
	Stream.Write( &Cross, sizeof(MatrixEx) );		// 위치도 그냥 서버에서 직접 계산하는게 나을라나..
	Send( eActor::CS_CANNONPOSSESS_REQ, &Stream );

	// 서버로부터 응답을 기다리는 플래그를 켜둠.
	m_bWaitCannonPossessRes = true;
}


void CDnLocalPlayerActor::EndLocalPlayerCannonMode( void )
{
	// TODO: 각종 UI 숨김.
	

	m_bPlayerCannonMode = false;

	// 대포 잡은 플레이어가 죽었을 경우 상위 클래스인 CDnPlayerActor::OnDie() 로 부터 호출되어질 수 있는데
	// 그런 경우엔 CDnPlayerActor 쪽에서 m_hCannonMonsterActor 핸들 변수가 정리된 상황이기 때문에 이 시점에서는 invalid 하다.
	// 그럴땐 그냥 넘어가고 나머지 일들 처리.
	if( m_hCannonMonsterActor )
	{
		CDnCannonMonsterActor* pCannonMonster = dynamic_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer());
		if( pCannonMonster ) pCannonMonster->ClearMasterPlayerActor();
	}

	CDnGameTask* pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pTask )
	{
		DnCameraHandle hCamera = pTask->GetPlayerCamera();
		CDnCamera::SetActiveCamera( hCamera );
		pTask->ResetPlayerCamera();
	}

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );
	if( pSox && pSox->IsExistItem(GetClassID()))
	{
		float fWeight = pSox->GetFieldFromLablePtr( GetClassID(), "_Weight" )->GetFloat();
		int fPressLevel = pSox->GetFieldFromLablePtr( GetClassID(), "_PressLevel" )->GetInteger();
	
		if (fWeight == 0.0f) //rlkt_weight
			fWeight = 1.0f;

		SetWeight(fWeight);
		SetPressLevel(fPressLevel);
	}
	m_hObject->SetCalcPositionFlag( CALC_POSITION_Y ); // 정상화

	m_hCannonMonsterActor.Identity();
}

void CDnLocalPlayerActor::ExitCannonMode( void )
{	
	if( IsCannonMode() )
		ToggleCannonMode( false, m_hCannonMonsterActor->GetUniqueID(), m_matexWorld );
}



void CDnLocalPlayerActor::ToggleCannonMode( bool bCannonMode, DWORD dwCannonMonsterID, const MatrixEx& Cross )
{
	if( m_bPlayerCannonMode == bCannonMode )
		return;	

	if( bCannonMode )
	{
		StopAllPartyPlayer();

		if(!IsBattleMode())
			CmdToggleBattle(true);
		

		// 각종 UI 보여줌.

		// 대포 뒤에 붙여준다. 그리고 이동불가.
		// 지금은 일단 대충 적당히 붙여보지만.. 나중엔 더미 오브젝트나 본이 있어야 할 듯 싶다.
		m_bPlayerCannonMode = true;
		m_hCannonMonsterActor = CDnActor::FindActorFromUniqueID( dwCannonMonsterID );

		if (!m_hCannonMonsterActor)
		{
			std::wstring str = FormatW(L"INVALID CANNON MONSTERID : %ld", dwCannonMonsterID);
			_ASSERT(0 && str.c_str());
			return;
		}

		CDnCannonMonsterActor* pCannonMonster = dynamic_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer());
		if( pCannonMonster ) pCannonMonster->SetMasterPlayerActor( GetMySmartPtr() );

		EtVector3 vCannonDir = m_hCannonMonsterActor->GetMatEx()->m_vZAxis;
		EtVec3Normalize( &vCannonDir, &vCannonDir );

		// 바운딩 박스 말고 Press 로 밀어내야 한다. 
		// 대포쪽으로 밀어주고 press 값 얻어온다.
		// 양쪽 다 press circle 이 있는 것으로 가정한다.
		m_matexWorld.m_vPosition = m_hCannonMonsterActor->GetMatEx()->m_vPosition - (vCannonDir*20.0f);
		EtVector2 vDir;
		float vDist = 0.0f;
		_ASSERT( GetPress() == CDnActorState::Press_Circle && m_hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle );
		if( GetPress() == CDnActorState::Press_Circle && m_hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle )
		{
			if( CheckPressCircle2Clrcle2( m_hCannonMonsterActor, GetMySmartPtr() , vDir, vDist ) ) 
			{
				m_matexWorld.m_vPosition.x += vDir.x*vDist; //MoveToWorld( vDir * vDist );
				m_matexWorld.m_vPosition.z += vDir.y*vDist;
			}
		}

		// 캐릭터가 바라보는 방향도 대포의 방향으로 셋팅.
		CmdLook( EtVector2(vCannonDir.x, vCannonDir.z), true );

		// 대포 전용 카메라로 변경 및 카메라가 바라보는 방향을 현재 대포방향으로 수정된 캐릭터의 방향으로 바라보도록..
		if( !m_hCannonCamera )
		{
			m_hCannonCamera = (new CDnCannonCamera)->GetMySmartPtr();
		}
		
		// 플레이어 카메라 정보 그대로 셋팅
		CDnGameTask* pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

		CDnCamera::SetActiveCamera( m_hCannonCamera );

		if( pTask )
		{
			DnCameraHandle hPlayerCamera = pTask->GetPlayerCamera();
			m_hCannonCamera->SetCameraInfo( *hPlayerCamera->GetCameraInfo() );
		}

		CDnCannonCamera *pCannonCamera = static_cast<CDnCannonCamera *>(m_hCannonCamera.GetPointer());
		pCannonCamera->AttachActor( GetMySmartPtr() );
		pCannonCamera->AttachCannonMonster( m_hCannonMonsterActor );	
		
		// 대포를 잡은상태에서는 밀리지 않도록 설정합니다.
		SetWeight(0.f);
		SetPressLevel(-1);
		m_hObject->SetCalcPositionFlag( -1 ); // 액션해도 움직이지 않도록 설정합니다.

		// 대포 액션으로 변경.
#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
		DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );

		if( pTableCannon->IsExistItem(pCannonMonster->GetClassID()) )
		{
			const char* pCannonActionName = pTableCannon->GetFieldFromLablePtr( pCannonMonster->GetClassID(), "_StandName" )->GetString();
			CmdAction(pCannonActionName);
		}
		else
		{
			CmdAction( "Stand_Cannon" );
		}
#else
		// 대포 액션으로 변경.
		CmdAction( "Stand_Cannon" );
#endif
	}
	else
	{
		DWORD dwCannonActorID = ( m_hCannonMonsterActor ) ? m_hCannonMonsterActor->GetUniqueID() : -1;

		EndLocalPlayerCannonMode();
		
		// 대포모드 해제,, 서버로 대포를 놨다고 패킷을 보내줌.

		BYTE pBuffer[64] = { 0, };
		CPacketCompressStream Stream( pBuffer, 64 );

		Stream.Write( &dwCannonActorID, sizeof(DWORD) );
		Stream.Write( &m_matexWorld.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
		Stream.Write( &m_matexWorld.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Stream.Write( &m_matexWorld.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Stream.Write( &m_matexWorld.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Send( eActor::CS_CANNONRELEASE, &Stream );
		
		// 대포 액션으로 종료.
		CmdAction( "Stand" );
	}
}


void CDnLocalPlayerActor::OnCannonCameraProcessEnd( void )
{
	if( IsCannonMode() )
	{
		// 서버로 보내기 전에 대포의 타겟을 최종적으로 확정.
		if( m_bNeedSendCannonTargeting /*&&
			0.0f == m_hCannonCamera->GetFreezeResistanceRatio()*/ )	// 카메라가 완전히 멈췄을 때 해당 방향으로 패킷 보냄.
		{
			m_bNeedSendCannonTargeting = false;

			CDnCannonMonsterActor* pCannonMonsterActor = dynamic_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer());

			// 게임서버로 현재 카메라의 look 방향 벡터를 보낼 것이기 때문에 클라이언트에서도 동일한 조건으로 발사 위치를 업데이트 해두어야 동기가 맞는다.
			if( pCannonMonsterActor ) {
				pCannonMonsterActor->ProcessCannonLook( true );
				pCannonMonsterActor->GetCannonGroundHitPosition( m_hCannonCamera, m_vNowCannonGroundHitPos );
			}

			// 우선은 발사체 시그널에 있는 방향을 그대로 사용..
			DWORD dwCannonMonsterActorID = m_hCannonMonsterActor->GetUniqueID();
			BYTE pBuffer[ 64 ] = { 0 };
			CPacketCompressStream Stream( pBuffer, 64 );
			Stream.Write( &dwCannonMonsterActorID, sizeof(DWORD) );
			Stream.Write( &(m_hCannonCamera->GetMatEx()->m_vZAxis), sizeof(EtVector3) );				// 현재 카메라가 바라보고 있는 월드축 기준 방향.
			Stream.Write( &static_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer())->GetNowShootDir(), sizeof(EtVector3) );	// 대포 기준 로컬좌표계의 발사 방향.
			Stream.Write( &m_vNowCannonGroundHitPos, sizeof(EtVector3) );

			Send( eActor::CS_CANNONTARGETING, &Stream );
		}

		// 플레이어의 위치라 대포 잡은 상태에서 밀리는 경우가 있어서 대포 쏠 때마다 다시 보정해 준다.

		// 바운딩 박스 말고 Press 로 밀어내야 한다. 
		// 대포쪽으로 밀어주고 press 값 얻어온다.
		// 양쪽 다 press circle 이 있는 것으로 가정한다.
		EtVector3 vCannonDir = m_hCannonMonsterActor->GetMatEx()->m_vZAxis;
		EtVec3Normalize( &vCannonDir, &vCannonDir );
		m_matexWorld.m_vPosition = m_hCannonMonsterActor->GetMatEx()->m_vPosition - (vCannonDir*20.0f);
		EtVector2 vDir;
		float vDist = 0.0f;
		_ASSERT( GetPress() == CDnActorState::Press_Circle && m_hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle );
		if( GetPress() == CDnActorState::Press_Circle && m_hCannonMonsterActor->GetPress() == CDnActorState::Press_Circle )
		{
			if( CheckPressCircle2Clrcle2( m_hCannonMonsterActor, GetMySmartPtr() , vDir, vDist ) ) 
			{
				m_matexWorld.m_vPosition.x += vDir.x*vDist; //MoveToWorld( vDir * vDist );
				m_matexWorld.m_vPosition.z += vDir.y*vDist;
			}
		}
	}
}


void CDnLocalPlayerActor::ProcessCannonRotateSync( void )
{
	//if( !m_bPlayerCannonMode ) 이 함수 호출 전에 체크하고 호출함.
	//	return;

	if( !m_hCannonMonsterActor )
		return;

	if( false == m_bCannonRotated )
		return;

	if( CDnActionBase::m_LocalTime - m_LastSendCannonRotateSyncTime < 100 )
		return;

	// 낙하지점 예측 계산을 새로 해주어야 함.
	// 크로스헤어가 보고 있는 방향으로 쏴준다.

	m_LastSendCannonRotateSyncTime = CDnActionBase::m_LocalTime;

	BYTE pBuffer[ 64 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 64 );
	DWORD dwCannonMonsterID = m_hCannonMonsterActor->GetUniqueID();
	Stream.Write( &dwCannonMonsterID, sizeof(DWORD) );
	Stream.Write( &(m_hCannonCamera->GetMatEx()->m_vZAxis), sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
	Send( eActor::CS_CANNONROTATESYNC, &Stream );

	//OutputDebug( "[회전 동기 발송]\n" );

	// 이 타이밍에 다른 어떤 이유로 인해 "Stand" 액션을 캐릭터가 취하고 있으면
	// Stand_Cannon 로 바꿔준다.
	// 감전 액션을 하다가 풀리거나 하는 경우.
	const char* pCurrentAction = GetCurrentAction();
	if( strcmp(pCurrentAction, "Stand") == 0 )
	{
#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
		DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );

		if( pTableCannon->IsExistItem(m_hCannonMonsterActor->GetClassID()) )
		{
			const char* pCannonActionName = pTableCannon->GetFieldFromLablePtr( m_hCannonMonsterActor->GetClassID(), "_StandName" )->GetString();
			CmdAction(pCannonActionName);
		}
		else
		{
			CmdAction( "Stand_Cannon" );
		}
#else
		// 대포 액션으로 변경.
		CmdAction( "Stand_Cannon" );
#endif
	}

	m_bCannonRotated = false;
}

void CDnLocalPlayerActor::OnCannonMonsterDie( void )
{
	CDnPlayerActor::OnCannonMonsterDie();

	EndLocalPlayerCannonMode();

	SetActionQueue( "Stand" );
}
/* //rlkt_later
void CDnLocalPlayerActor::ProcessSkillReplace()
{
	CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
	if (pMainBarDlg)
	{
		CEtUIDialogGroup *pQuickSlotDlgGroup = pMainBarDlg->GetQuickSlotDialogGroup();
		if (pQuickSlotDlgGroup)
		{
			CDnQuickSlotDlg *pEventQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pQuickSlotDlgGroup->GetDialog(CDnMainDlg::QUICKSLOT_EVENT));

			pEventQuickSlotDlg->GetItemList(m_vecBackUpEventSlotItem);
			pEventQuickSlotDlg->ResetAllSlot();

			for (DWORD i = 0; i<m_vecTransformSkillList.size(); i++)
			{
				if (m_vecTransformSkillList[i] != -1)
				{
					MIInventoryItem * pFindSkill = FindSkill(m_vecTransformSkillList[i]);
					if (pFindSkill)
						pEventQuickSlotDlg->SetEventSlot(pFindSkill);
				}
			}
		}
	}
}
*/
void CDnLocalPlayerActor::RefreshTransformMode()
{
	CDnPlayerActor::RefreshTransformMode();

	if(!GetInterface().GetMainBarDialog())
		return;

	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !(pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && ((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival) ) 
	{
		if( m_vecTransformSkillList.empty() == false )
		{
			GetInterface().GetMainBarDialog()->LockQuickSlot(IsTransformMode());
			GetInterface().GetMainBarDialog()->SwapEventSlot(IsTransformMode());
		}

		LockItemMove(IsTransformMode());
	}

	if( IsTransformMode() )
	{
		CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
		if(pMainBarDlg)
		{
			CEtUIDialogGroup *pQuickSlotDlgGroup = pMainBarDlg->GetQuickSlotDialogGroup();
			if(pQuickSlotDlgGroup)
			{
				CDnQuickSlotDlg *pEventQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pQuickSlotDlgGroup->GetDialog( CDnMainDlg::QUICKSLOT_EVENT ));

				pEventQuickSlotDlg->GetItemList(m_vecBackUpEventSlotItem);
				pEventQuickSlotDlg->ResetAllSlot();

				for(DWORD i=0; i<m_vecTransformSkillList.size(); i++)
				{
					if(m_vecTransformSkillList[i] != -1)
					{
						MIInventoryItem * pFindSkill = FindSkill( m_vecTransformSkillList[i] );
						if(pFindSkill)
							pEventQuickSlotDlg->SetEventSlot(pFindSkill); 
					}
				}
			}
		}
	}
	else
	{
		CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
		if(pMainBarDlg && pMainBarDlg->GetQuickSlotTabIndex() == CDnMainDlg::QUICKSLOT_EVENT)
		{
			CEtUIDialogGroup *pQuickSlotDlgGroup = pMainBarDlg->GetQuickSlotDialogGroup();
			if(pQuickSlotDlgGroup)
			{
				CDnQuickSlotDlg *pEventQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pQuickSlotDlgGroup->GetDialog( CDnMainDlg::QUICKSLOT_EVENT ));
				pEventQuickSlotDlg->ResetAllSlot(); // 기본적으로 플레이어 상태가 되었을때는 모두 삭제.

				if( false == m_vecBackUpEventSlotItem.empty() )
				{
					for(DWORD i=0; i<m_vecBackUpEventSlotItem.size(); i++)
					{
						CDnItem *pFindItem = GetItemTask().FindItem( m_vecBackUpEventSlotItem[i] , ITEM_SLOT_TYPE::ST_INVENTORY ); // 스킬은 넣지 않는다. 아이템만 넣음.
						if(pFindItem)
							pEventQuickSlotDlg->SetEventSlot(pFindItem);
					}

					m_vecBackUpEventSlotItem.clear();
				}
			}
		}
	}

	GetRadioMsgTask().EnableTransformSound(m_nTransformTableID,IsTransformMode());
	GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
}

void CDnLocalPlayerActor::ChangeShootModeInputAction()
{
	if( IsSwapSingleSkin() ) return;
	if( IsCustomAction() && m_bShootMode ) {
		if( strstr( GetCurrentAction(), "MOD_Move" ) || strcmp( GetCurrentAction(), "MOD_Stand" ) == NULL ) {
			std::string szMixedAction;
			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;

			if( strstr( m_szCustomAction.c_str(), "Stand" ) && !strstr( GetCurrentAction(), "Stand" ) ) {
				if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = "MOD_Shoot_Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = "MOD_Shoot_Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = "MOD_Shoot_Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = "MOD_Shoot_Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Front" ) && !strstr( GetCurrentAction(), "Front" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = "MOD_Shoot_Stand";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = "MOD_Shoot_Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = "MOD_Shoot_Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = "MOD_Shoot_Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Back" ) && !strstr( GetCurrentAction(), "Back" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = "MOD_Shoot_Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = "MOD_Shoot_Front";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = "MOD_Shoot_Left";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = "MOD_Shoot_Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Left" ) && !strstr( GetCurrentAction(), "Left" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = "MOD_Shoot_Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = "MOD_Shoot_Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = "MOD_Shoot_Back";
				else if( strstr( GetCurrentAction(), "Right" ) ) szMixedAction = "MOD_Shoot_Right";
			}
			else if( strstr( m_szCustomAction.c_str(), "Right" ) && !strstr( GetCurrentAction(), "Right" ) ) {
				if( strstr( GetCurrentAction(), "Stand" ) ) szMixedAction = "MOD_Shoot_Stand";
				else if( strstr( GetCurrentAction(), "Front" ) ) szMixedAction = "MOD_Shoot_Front";
				else if( strstr( GetCurrentAction(), "Back" ) ) szMixedAction = "MOD_Shoot_Back";
				else if( strstr( GetCurrentAction(), "Left" ) ) szMixedAction = "MOD_Shoot_Left";
			}
			if( !szMixedAction.empty() ) {
				ResetMixedAnimation();
				CmdMixedAction( m_szActionBoneName.c_str(), m_szMaintenanceBoneName.c_str(), szMixedAction.c_str(), 0, fFrame, 6.f );
				SetCustomAction( szMixedAction.c_str(), fFrame );
			}
		}
	}
}

#ifdef PRE_ADD_MONSTER_CATCH
void CDnLocalPlayerActor::OnReleaseFromMonster( DnActorHandle hCatcherMonsterActor )
{
	CDnPlayerActor::OnReleaseFromMonster( hCatcherMonsterActor );

	//ProcessPositionRevision( true );
}
#endif // #ifdef PRE_ADD_MONSTER_CATCH

void CDnLocalPlayerActor::ChangeSkillLevelUp(int nSkillID, int nOrigLevel)
{
	GetSkillTask().ChangeSkillLevelUp(nSkillID, nOrigLevel);

}


void CDnLocalPlayerActor::RideVehicle(TVehicleCompact sInfo , bool bForce )
{
	CDnPlayerActor::RideVehicle(sInfo,bForce);
	SendCompleteRideVehicle(true);
#ifdef PRE_FIX_CHARSTATUS_REFRESH
	GetInterface().OnRefreshLocalPlayerStatus();
#endif
}

void CDnLocalPlayerActor::UnRideVehicle(bool bIgnoreEffect , bool bIgnoreAction )
{
	CDnPlayerActor::UnRideVehicle(bIgnoreEffect,bIgnoreAction);
#ifdef PRE_FIX_CHARSTATUS_REFRESH
	GetInterface().OnRefreshLocalPlayerStatus();
#endif
}

void CDnLocalPlayerActor::SendCompleteRideVehicle(bool bComplete) // 탈것을 정상적으로 탔다고 메세지를 보내준다
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &bComplete, sizeof(bool) );
	Send( eActor::CS_VEHICLE_RIDE_COMPLETE, &Stream );
}

bool CDnLocalPlayerActor::ProcessPet( LOCAL_TIME LocalTime, float fDelta )
{
	if( CDnPlayerActor::ProcessPet( LocalTime, fDelta ) == false ) return false;

	PROFILE_TIME_TEST( ProcessPickupPet( LocalTime, fDelta ) );
	return true;
}

void CDnLocalPlayerActor::ProcessPickupPet( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hPet ) return;
	if( ( m_sPetCompact.nType & Pet::Type::ePETTYPE_SATIETY ) && !GetPetTask().EnableSatietyPickup() ) return;	// 만복도 펫은 만복도 설정에 따라 줍기 On/Off

	if( m_LastUpdatePickupPet == 0 ) m_LastUpdatePickupPet = LocalTime;
	if( LocalTime - m_LastUpdatePickupPet < 300 ) return;

	m_LastUpdatePickupPet = LocalTime;

	// 내 자신만 동작하는 기능 (아이템 먹기, 물약 자동 사용)
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );

	// 아이템 먹기
	CDnPetActor* pPetActor = static_cast<CDnPetActor*>(m_hPet.GetPointer());
	if( pPetActor == NULL ) return;
	DNVector(DnDropItemHandle) hVecList;
	DNVector(DnDropItemHandle) hVecWantList;
	int nCount = CDnDropItem::ScanItem( *(pPetActor->GetPosition()), m_fPetPickUpRange, hVecList );
	if( nCount > 0 )
	{
		bool bInventoryFull = false;
		if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY ) )
			bInventoryFull = true;

		float fMinDist = FLT_MAX;
		DnDropItemHandle hResult;
		bool bRequestPickupItem = false;
		int nFailPickUpItemCount = 0;	// 수집하고 싶은 아이템 갯 수
		for( int i=0; i<nCount; i++ )
		{
			hResult = hVecList[i];
			if( !hResult ) continue;
			
			// 수집하지 않을 아이템 패스
			if( hResult->GetItemID() != 0 && !pSox->IsExistItem( hResult->GetItemID() ) ) continue;	// 돈 아닌데 존재하지 않는 아이템일 경우 패스
			
			CDnItem::ItemSkillApplyType SkillApplyType = (CDnItem::ItemSkillApplyType)pSox->GetFieldFromLablePtr( hResult->GetItemID(), "_SkillUsingType" )->GetInteger();	// 물약 패스
			eItemTypeEnum eItemType = CDnItem::GetItemType( hResult->GetItemID() );
			if( eItemType != ITEMTYPE_SOURCE && SkillApplyType != CDnItem::ItemSkillApplyType::None ) continue;	// 근원 아이템은 습득
			if( eItemType != ITEMTYPE_GLOBAL_PARTY_BUFF && SkillApplyType != CDnItem::ItemSkillApplyType::None ) continue;

			eItemRank ItemRank = (eItemRank)pSox->GetFieldFromLablePtr( hResult->GetItemID(), "_Rank" )->GetInteger();

			if( hResult->GetItemID() != 0 )	// 옵션에 설정된 먹지 않을 것들 패스
			{
				if( !GetPetTask().GetPetOption().m_bGetItem ) continue;
				else if( ItemRank == ITEMRANK_D && !GetPetTask().GetPetOption().m_bGetNormalItem ) continue;
				else if( ItemRank == ITEMRANK_C && !GetPetTask().GetPetOption().m_bGetMagicItem ) continue;
				else if( ItemRank == ITEMRANK_B && !GetPetTask().GetPetOption().m_bGetRareItem ) continue;
				else if( ItemRank == ITEMRANK_A && !GetPetTask().GetPetOption().m_bGetEpicItem ) continue;
				else if( ItemRank == ITEMRANK_S && !GetPetTask().GetPetOption().m_bGetUniqueItem ) continue;
				else if( ItemRank == ITEMRANK_SS && !GetPetTask().GetPetOption().m_bGetLegendItem ) continue;
			}

			if( pPetActor->IsNotEatItemList( hResult->GetUniqueID() ) )	// 먹지 않기로 한 아이템 패스
				continue;
			hVecWantList.push_back( hResult );

			// 수집할 아이템에 대한 검사
			if( bInventoryFull )
			{
				if( CDnItemTask::IsActive() && hResult->GetItemID() != 0 )
				{
					std::vector<CDnItem *> vecInvenItemList;
					int nCount = CDnItemTask::GetInstance().GetCharInventory().FindItemList( hResult->GetItemID(), -1, vecInvenItemList );

					// 기존 자리에 들어가는 거라면, OverlapCount를 검사해서 증가인지 판단한다.
					bool bAddOverlap = false;
					int nInvenItemListCount = static_cast<int>( vecInvenItemList.size() );
					if( nCount > 0 && nInvenItemListCount > 0 )
					{
						int nAvailableOverlapCount =  ( nInvenItemListCount * vecInvenItemList[0]->GetMaxOverlapCount() ) - nCount;
						if( nAvailableOverlapCount > 0 && hResult->GetOverlapCount() <= nAvailableOverlapCount )
							bAddOverlap = true;
					}

					if( !bAddOverlap )
					{
						nFailPickUpItemCount++;
						continue;
					}
				}
			}
			if( std::find( m_nVecRequestPickupItemList.begin(), m_nVecRequestPickupItemList.end(), hResult->GetUniqueID() ) != m_nVecRequestPickupItemList.end() )
				continue;

			DWORD dwUniqueID = hResult->GetUniqueID();
			BYTE pBuffer[128];
			CPacketCompressStream Stream( pBuffer, 128 );

			int nSignalIndex = 4;

			Stream.Write( &dwUniqueID, sizeof(DWORD) );
			Stream.Write( &nSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Write( pPetActor->GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

			Send( eActor::SC_CMDPICKUPITEM, &Stream );
			m_nVecRequestPickupItemList.push_back( hResult->GetUniqueID() );

			bRequestPickupItem = true;
			break;
		}

		if( bRequestPickupItem )
		{
			SetPetEffectAction( m_strPetPickUpActionName.c_str() );
		}

		if( bInventoryFull && nFailPickUpItemCount > 0 )	// 수집하고 싶었지만 인벤토리가 다 찾을 경우
		{
			// 이미 스캔했던 아이템인지 확인 해서 메세지 출력
			if( pPetActor->HaveAnyDoNotGetItem( hVecWantList ) )
			{
				WCHAR wszMsg[128];
				swprintf_s( wszMsg, _countof(wszMsg), L"\n\n%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory2, 405 ) );
				GetInterface().ShowItemAlarmDialog( wszMsg, NULL, 0, textcolor::WHITE, 2.0f );
			}
		}

		hVecList.clear();
		hVecWantList.clear();
	}
}

bool CDnLocalPlayerActor::OnApplySpectator(bool bEnable)
{
	if( CDnPlayerActor::OnApplySpectator(bEnable) == true )
	{
		CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera(CDnCamera::PlayerCamera).GetPointer(); 
		if(pCamera && pCamera->GetCameraType() == CDnCamera::PlayerCamera )
		{
			pCamera->DetachActor();
			pCamera->AttachActor( GetActorHandle() );
		}
	}

	SetFollowObserver(false,0);
	return true;
}

void CDnLocalPlayerActor::BeginCanUsePassiveSkill( LOCAL_TIME SignalEndTime )
{
#if defined(PRE_FIX_66175)
	//InputHasPassiveSkill 시그널의 bCanUseSkill이 true로 설정이 되면
	//스킬 사용 여부 체크 하기전 BeginCanUsePassiveSkill/EndCanUsePassiveSkill호출로 CanUseSkill시그널에 설정된 값이 변경이 되어 버린다.
	//InputHasPassiveSkill 시그널의 bCanuseSkill이 CanUseSkill시그널 동작에 영향을 주지 않게 하도록 함수 추가

	CDnMovableChecker::SetPrimUseSignalSkillCheck( true );
	CDnJumpableChecker::SetPrimUseSignalSkillCheck( true );
	CDnGroundMovableChecker::SetPrimUseSignalSkillCheck( true );
#else
	CDnMovableChecker::SetUseSignalSkillCheck( true, SignalEndTime );
	CDnJumpableChecker::SetUseSignalSkillCheck( true, SignalEndTime );
	CDnGroundMovableChecker::SetUseSignalSkillCheck( true, SignalEndTime );
#endif // PRE_FIX_66175
	m_bUseSignalSkillCheck = true;
}

void CDnLocalPlayerActor::EndCanUsePassiveSkill( void )
{
#if defined(PRE_FIX_66175)
	CDnMovableChecker::SetPrimUseSignalSkillCheck( false );
	CDnJumpableChecker::SetPrimUseSignalSkillCheck( false );
	CDnGroundMovableChecker::SetPrimUseSignalSkillCheck( false );
#else
	CDnMovableChecker::SetUseSignalSkillCheck( false, 0 );
	CDnJumpableChecker::SetUseSignalSkillCheck( false, 0 );
	CDnGroundMovableChecker::SetUseSignalSkillCheck( false, 0 );
#endif // PRE_FIX_66175
	m_bUseSignalSkillCheck = false;
}

#if defined(PRE_FIX_45899)
void CDnLocalPlayerActor::DisableSkillByItemMove(int nSkillID)
{
	DnSkillHandle hSkill = FindSkill(nSkillID);
	if (hSkill)
		hSkill->DisableSkillByItemMove(true);
}
#endif // PRE_FIX_45899

int CDnLocalPlayerActor::GetPlayerGuildRewardEffectValue(int nType)
{
	return GetGuildTask().GetGuildRewardEffectValue(nType);
}

void CDnLocalPlayerActor::RenderStigmaActorMarker(float fElapsedTime)
{
	DNVector(DnActorHandle) StigmaActorList;
	ScanActorByStateIndex(StigmaActorList, STATE_BLOW::BLOW_246);
	
	int nStigmaActorCount = (int)StigmaActorList.size();
	if (nStigmaActorCount == 0)
		return;

	for (int i = 0; i<nStigmaActorCount; ++i)
	{
		DnActorHandle hActor = StigmaActorList[i];
		if (hActor && hActor->IsDie() == false)
		{
			EtVector3 vMemberPos = *hActor->GetPosition();

			if (hActor && hActor->IsProcess() == false)
				continue;

			EtCameraHandle hEtCam = CEtCamera::GetActiveCamera();
			if (hEtCam)
			{
				EtVector3* pLocalActorPos = CDnActor::s_hLocalActor->GetPosition();
				if (pLocalActorPos != NULL && GetPosition() && hEtCam->GetPosition())
				{
					bool bShowMemberTag = false;
					float distanceFromLocalActor = EtVec3Length(&(*pLocalActorPos - vMemberPos));
					if (distanceFromLocalActor > CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PartyMarkerDistance))
						bShowMemberTag = true;

					float fRate = 0.8f;
					bool bActorChatBalloon = false;
					if (m_dwStigmaActorMarkerColor != 0xffffffff)
						m_dwStigmaActorMarkerColor = CommonUtil::BlendColor(fElapsedTime, m_dwStigmaActorMarkerColor, 0xffffffff, fRate);
					
					bool bSightOff = false;
					if ((bShowMemberTag == false && GetPosition()) || bActorChatBalloon)
					{
						CEtConvexVolume Frustum;
						Frustum.Initialize(*hEtCam->GetViewProjMatForCull());

						SAABox AABB;
						GetBoundingBox(AABB);
						const EtVector3 vDiff = (vMemberPos - *GetPosition());
						AABB.Min += vDiff;
						AABB.Max += vDiff;
						bShowMemberTag = (Frustum.TesToBox( AABB.GetCenter(), AABB.GetExtent()) == false);
						if (bShowMemberTag == true)
							m_dwStigmaActorMarkerColor = 0xffffffff;
					}

					if (bShowMemberTag)
					{
						EtTextureHandle hTex = CDnInterface::GetInstance().GetStigmaActorMarketTex();
						if (!hTex)
							continue;

						SUICoord screenUICoord, UVCoord;
						EtVector3 vScreenPos, vTemp2;
						DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
						if (!hCamera)
							break;

						float heightRatio = 1.0f;
						EtVector2 vSize(78.f, 78.f);

						EtVector3 vTemp = vMemberPos;
						vTemp.y += 8.f;
						bool bBackOfCam = false;
						if (CDnActor::s_hLocalActor && hEtCam->GetPosition() && hEtCam->GetDirection())
						{
							EtVector3 vCurlook, vLookFromMember, vCross;
							vCurlook = *hEtCam->GetDirection();
							EtVec3Normalize(&vLookFromMember, &(vTemp - *hEtCam->GetPosition()));
							vLookFromMember.y = vCurlook.y;
							if (EtVec3Dot(&vCurlook, &vLookFromMember) < 0.f)
								bBackOfCam = true;

							screenUICoord.SetSize(vSize.x / (float)GetEtDevice()->Width(), vSize.y / (float)GetEtDevice()->Height());

							if (bBackOfCam)
							{
								screenUICoord.fY = 1.f - screenUICoord.fHeight * 2.f;
								EtVec3Cross( &vCross, &vCurlook, &(EtVector3(0.f, 1.f, 0.f)) );
								if( vCross.y > 0.0f )
									screenUICoord.fX = 1.f - screenUICoord.fWidth;
								else
									screenUICoord.fX = 0.f;
							}
							else
							{
								EtViewPort vp;
								EtMatrix* matViewProj;
								GetEtDevice()->GetViewport(&vp);
								matViewProj = hEtCam->GetViewProjMatForCull();
								EtVec3TransformCoord( &vScreenPos, &vTemp, matViewProj );

								screenUICoord.fX = (( vScreenPos.x + 1.0f ) / 2.0f + vp.X);
								screenUICoord.fY = (( 2.0f - ( vScreenPos.y + 1.35f ) ) / 2.0f + vp.Y);

								CommonUtil::ClipNumber(screenUICoord.fX, screenUICoord.fWidth * 0.5f, 1.f - screenUICoord.fWidth * 0.5f);
								CommonUtil::ClipNumber(screenUICoord.fY, screenUICoord.fHeight * 0.5f, 1.f - screenUICoord.fHeight * 0.5f);

								screenUICoord.fX -= screenUICoord.fWidth / 2.f;
								//screenUICoord.fY -= screenUICoord.fHeight;
								float fEndOfY = 1.f - screenUICoord.fHeight * 2.f;
								if (screenUICoord.fY > fEndOfY)
									screenUICoord.fY = fEndOfY;
							}
						}

						int nIndex = 0;
						int nCountX = 1;
						int nCountY = 1;
						UVCoord = SUICoord((vSize.x * (float)(nIndex % nCountX)) / hTex->Width(), (vSize.y * (float)(nIndex / nCountY)) / hTex->Height(), vSize.x / hTex->Width(), vSize.y / hTex->Width());

						CEtSprite::GetInstance().Begin(0);
						CEtSprite::GetInstance().DrawSprite((EtTexture*)hTex->GetTexturePtr(), hTex->Width(), hTex->Height(), UVCoord, m_dwStigmaActorMarkerColor, screenUICoord, 0.f);
						CEtSprite::GetInstance().End();
					}
				}
			}
		}
	}
}

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
void CDnLocalPlayerActor::OpenMailBoxByShortCutKey()
{
	bool isOpen = true;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &isOpen, sizeof(bool) );
	Send( eActor::CS_MAILBOX_OPEN, &Stream );
}

void CDnLocalPlayerActor::CloseMailBoxByShortCutKey()
{
	bool isOpen = false;

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &isOpen, sizeof(bool) );
	Send( eActor::CS_MAILBOX_CLOSE, &Stream );
	
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	GetInterface().GetMainBarDialog()->CloseBlinkMenuButton(CDnMainMenuDlg::MAIL_DIALOG );
#endif
}
#endif // PRE_ADD_MAILBOX_OPEN

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
void CDnLocalPlayerActor::UpdateTotalLevelUI()
{
	if (m_pTotalLevelSkillSystem)
	{
		std::map<int, bool>& activateList = m_pTotalLevelSkillSystem->GetSlotActivateList();

		std::map<int, bool>::iterator iter = activateList.begin();
		std::map<int, bool>::iterator endIter = activateList.end();

		for (; iter != endIter; ++iter)
		{
			CDnTotalLevelSkillSystem::_TotalLevelSkillSlotInfo* pInfo = m_pTotalLevelSkillSystem->GetSlotInfo(iter->first);
			if (pInfo == NULL)
				continue;

			if (pInfo->m_isCashSlot)
				ActivateTotalLevelSkillCashSlot(iter->first, iter->second, pInfo->m_ExpireDate);
			else
				ActivateTotalLevelSkillSlot(iter->first, iter->second);
		}
	}

	GetInterface().UpdateTotalLevelSkill();	
}
void CDnLocalPlayerActor::UpdateTotalLevel(int nLevel)
{
	CDnPlayerActor::UpdateTotalLevel(nLevel);

	//UI 갱신..
	UpdateTotalLevelUI();
	
}

void CDnLocalPlayerActor::UpdateTotalLevelByCharLevel()
{
	CDnPlayerActor::UpdateTotalLevelByCharLevel();

	//UI 갱신..
	UpdateTotalLevelUI();
}

void CDnLocalPlayerActor::AddTotalLevelSkill(int nSlotIndex, int nSkillID, bool isInitialize/* = false*/)
{
	CDnPlayerActor::AddTotalLevelSkill(nSlotIndex, nSkillID, isInitialize);

	//만약 해당 슬롯이 활성화가 되지 않은 경우는 추가 하지 않도록 한다..
	bool isActivateSlot = m_pTotalLevelSkillSystem->IsActivateSlot(nSlotIndex);
	if (isActivateSlot == false)
		return;

	//UI 갱신..
	if (m_pTotalLevelSkillSystem)
	{
		DnSkillHandle hSkill = m_pTotalLevelSkillSystem->FindTotalLevelSkill(nSkillID);
		
		GetInterface().AddTotalLevelSkill(nSlotIndex, hSkill);
	}
}

void CDnLocalPlayerActor::RemoveTotalLevelSkill(int nSlotIndex)
{
	CDnPlayerActor::RemoveTotalLevelSkill(nSlotIndex);

	//UI 갱신..
	GetInterface().RemoveTotalLevelSkill(nSlotIndex);	
}

void CDnLocalPlayerActor::ActivateTotalLevelSkillSlot(int nSlotIndex, bool bActivate)
{
	CDnPlayerActor::ActivateTotalLevelSkillSlot(nSlotIndex, bActivate);

	//UI 갱신
	GetInterface().ActivateTotalLevelSkillSlot(nSlotIndex, bActivate);
}

void CDnLocalPlayerActor::ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate)
{
	CDnPlayerActor::ActivateTotalLevelSkillCashSlot(nSlotIndex, bActivate, tExpireDate);

	//UI 갱신
	GetInterface().ActivateTotalLevelSkillCashSlot(nSlotIndex, bActivate, tExpireDate);
}

void CDnLocalPlayerActor::RequestAddTotalLevelSkill(int nSlotIndex, int nSkillID)
{
	BYTE pBuffer[128];
	bool isInitialize = false;

	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &nSlotIndex, sizeof(int) );
	Stream.Write( &nSkillID, sizeof(int) );
	Stream.Write( &isInitialize, sizeof(bool) );
	
	Send( eActor::CS_ADD_TOTAL_LEVEL_SKILL, &Stream );
}

void CDnLocalPlayerActor::RequestRemoveTotalLevelSkill(int nSlotIndex)
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &nSlotIndex, sizeof(int) );
	
	Send( eActor::CS_REMOVE_TOTAL_LEVEL_SKILL, &Stream );
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_NEWCOMEBACK
void CDnLocalPlayerActor::ShowComebackUserEffect()
{
	DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
	if( hHandle )
	{			
		hHandle->SetPosition( m_matexWorld.m_vPosition );
		hHandle->SetActionQueue( "LevelUp" );

		EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );
		pResult->vOffset = EtVector3( 0.0f, 0.0f, 0.0f );
		pResult->vRotate = EtVector3( 0.0f, 0.0f, 0.0f );
		pResult->bLinkObject = true;
		pResult->bDefendenceParent = true;
	}
	
}
#endif // PRE_ADD_NEWCOMEBACK

#if defined(PRE_FIX_68898)
void CDnLocalPlayerActor::SetSkipEndAction(bool isSkipEndAction)
{
	CDnActor::SetSkipEndAction(isSkipEndAction);

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &isSkipEndAction, sizeof(bool) );

	Send( eActor::CS_SKIP_END_ACTION, &Stream );
}
#endif // PRE_FIX_68898

#if defined(PRE_ADD_TALISMAN_SYSTEM) 
bool CDnLocalPlayerActor::AttachTalisman( DnTalismanHandle hTalisman, int Index, float fRatio, bool bDelete/* = false*/)
{
	if( !MATalismanUser::AttachTalisman(hTalisman, Index, fRatio, bDelete) )
		return false;

	return true;
}

bool CDnLocalPlayerActor::DetachTalisman( int Index )
{
	if( !MATalismanUser::DetachTalisman(Index) )
		return false;

	return true;
}
#endif

#ifdef PRE_ADD_MAINQUEST_UI
void CDnLocalPlayerActor::SetHideNpc(bool bFlag, float fDelta)
{
	m_bHideNpc = bFlag;
	m_fHideNpcDelta = fDelta;
}
#endif // PRE_ADD_MAINQUEST_UI