#include "StdAfx.h"
#include "DnPlayerActor.h"
#include "EtMatrixEx.h"
#include "DnWorld.h"
#include "MAMovementBase.h"
#include "DnWeapon.h"
#include "DnProjectile.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnSkill.h"
#include "DnTableDB.h"
#include "DnInCodeResource.h"
#include "DnInterface.h"
#include "DnitemTask.h"
#include "DnMovableChecker.h"
#include "DnGroundMovableChecker.h"
#include "DnJumpableChecker.h"
#include "ItemSendPacket.h"
#include "DnStateBlow.h"
#include "DnCreateBlow.h"
#include "DnBasicBlow.h"
#include "EtOptionController.h"
#include "GameOption.h"
#include "DnInterfaceString.h"
#include "DnPlayerCamera.h"
#include "DnPlayAniProcess.h"
#include "DnChangeActionSetBlow.h"
#include "DnChangeActionStrProcessor.h"
#include "EtDecal.h"
#include "DnSkillTask.h"
#include "DnMonsterActor.h"
#include "DnPVPGameResultDlg.h"
#include "DnCooltimeParryBlow.h"
#include "DnMainMenuDlg.h"
#include "DnChangeStandActionBlow.h"
#include "DnPvPGameTask.h"
#include "DnVillageTask.h"

#include "Timeset.h"
#include "DnGaugeDlg.h"

#include "DnPartsVehicleEffect.h"
#include "DnActorClassDefine.h"
#include "DnLocalPlayerActor.h"
#include "VillageClientSession.h"
#include "DnCharVehicleDlg.h"
#include "DnVehicleTask.h"

#include "DnMasterTask.h"
#include "DnCannonMonsterActor.h"
#include "DnGuildTask.h"

#include "DnShockInfectionBlow.h"
#include "DnTransformBlow.h"
#include "EtActionSignal.h"
#include "DnPetActor.h"
#include "SyncTimer.h"
#include "DnPetTask.h"
#include "DnMainDlg.h"

#if defined (PRE_FIX_MEMOPT_EXT)
#include "DnCommonUtil.h"
#include "DnCutSceneTask.h"
#endif
#if defined (PRE_ADD_BESTFRIEND)
#include "DnAppellationTask.h"
#endif
#if defined (PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif
#if defined (PRE_ADD_DWC)
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#include "EtActionCoreMng.h"
#include "DnBubbleSystem.h"
#include "DnRevengeTask.h"


struct CalcDecalHeight : public CalcHeightFn
{
    CalcDecalHeight() {

    }

	virtual float GetHeight( float fX, float fY, float fZ, int nTileScale )  override {
		return INSTANCE(CDnWorld).GetHeightWithProp( EtVector3(fX, fY, fZ), NULL, NULL, nTileScale );
	}
};

bool CDnPlayerActor::s_bEnableAnotherPlayer = false;
bool CDnPlayerActor::s_bHideAnotherPlayer = false;
float CDnPlayerActor::s_fHideAnotherPlayerDelta = 0.0f;
CDnPlayerActor::CDnPlayerActor( int nClassID, bool bProcess )
: CDnActor( nClassID, bProcess )
{
	CDnPlayerState::Initialize( m_nClassID );
	CDnActionBase::Initialize( this );
	SetIncreaseHeight( 15.f );
#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	SetHeadEffectAdjustHeight( 30.0f );
#endif
	m_cMovePushKeyFlag = 0;
	m_bShowHeadName = true;
	m_bBattleMode = true;
	m_nComboCount = 0;
	m_nComboDelay = 0;
	memset( m_nDefaultPartsIndex, 0, sizeof(m_nDefaultPartsIndex) );

	m_bGhost = false;
	m_nSwapSingleSkinActorID = -1;

	m_fExposureTime = 0.0f;
	m_isExposure = false;
	m_isLocalExposure = false;

	m_hSwapOriginalHandle.Identity();
	m_pSwapOriginalAction = NULL;

	m_bInitializedSkillTree = false;

	m_bChangedEquipAction = false;

	memset( m_bSelfDeleteWeapon, 0, sizeof(m_bSelfDeleteWeapon) );
	memset( m_bWeaponViewOrder, 1, sizeof(m_bWeaponViewOrder) );
	memset( m_bHideWeaponBySignalInCutScene, 0, sizeof(m_bHideWeaponBySignalInCutScene) );

	m_bPlayerAway = false;
	m_hAbsenceMarkTex = LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossSleep.dds" ).c_str(), RT_TEXTURE );

	m_bPartyLeader = false;
	m_hPartyLeaderIconTex = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Icon_RoomMaster.dds" ).c_str(), RT_TEXTURE );
	
	m_bCaptain = false;
	m_bObserver = false;
	m_bPlayerCannonMode = false;

	// Rotha 좀비 모드
	m_nTransformTableID = -1;
	m_bRefreshTransformMode = false;
	m_bTransformMode = false;
	m_bShootMode = false;
	m_nTransformMoveSpeed = 0;
	m_vecTransformSkillList.clear();

#ifdef PRE_ADD_TRANSFORM_POTION
	m_bRefreshVillageTransformMode = false;
	m_nTransformItemIndex = 0;
#endif

	m_bEnableVehicleThread = false;
	m_bVehicleMode = false;
	m_hPostParentHandle.Identity();
	memset( &m_sVehicleCompact, 0, sizeof( m_sVehicleCompact ) );
	m_nVehicleSessionID = 0;
	m_fRideDelay = -1;
	m_bForceEnableRideByTrigger = true;

	m_bShowFishingRod = false;
	m_bStartCast = false;
	m_bStartLift = false;
	m_bRecieveFishingReward = false;
	m_fFishingTimer = FISHING_EFFECT_GAP;
	m_nFishingRetCode = ERROR_FISHING_FAIL;
	m_nFishingRewardItemID = -1;

	m_bSelfDeleteVehicleEffect = false;
	m_bTogglePlayerEffect = false;

	m_bSummonPet = false;
	m_bFollowPetAni = false;
	m_bChatExpire = false;
	m_bPetSlipAni = false;
	m_bPetSummonableMap = false;
	m_fPetFollowDeltaTime = 0.0f;
	m_fPetPickUpRange = 0.0f;
	m_strPetPickUpActionName = "";
	memset( &m_sPetCompact, 0, sizeof( m_sPetCompact ) );

	m_pBubbleSystem = new BubbleSystem::CDnBubbleSystem;

	memset(&m_PlayerGuildInfo, 0, sizeof(m_PlayerGuildInfo));

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
	m_hMailBoxOpenMarkTex	= LoadResource( CEtResourceMng::GetInstance().GetFullName( "CrossMail.dds" ).c_str(), RT_TEXTURE );
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_CASH_COSTUME_AURA
	m_bEnableCostumeAura = false; 
	m_bIsCompletParts	 = false;
	m_AuraRenderType	 = RT_SCREEN;
	memset(&m_SelectedCostumeData, 0, sizeof(sCurrentAuraCostume));
	
	char* szActionName[] = { "Head", "Body", "Hand_L", "Hand_R", "Foot_L", "Foot_R", "Foot_L2", "Foot_R2" };
	char* szBoneName[]   = { "Bip01 Head", "Bip01 Pelvis", "Bip01 L Forearm", "Bip01 R Forearm", "Bip01 L Calf", "Bip01 R Calf", "Bip01 L Foot", "Bip01 R Foot" };

	for(int i = 0 ; i < eParts_Max ; ++i)
	{
		m_vecActionName.push_back(szActionName[i]);
		m_vecBoneName.push_back(szBoneName[i]);
	}
#endif // PRE_ADD_CASH_COSTUME_AURA

#ifdef PRE_ADD_VIP
	m_bVIP = false;
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_pTotalLevelSkillSystem = NULL;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_NEWCOMEBACK	
	m_bComebackLocalUser = false;
#endif // PRE_ADD_NEWCOMEBACK	

#ifdef PRE_ADD_COSTUME_SKILL
	m_nCostumeSkillID = 0;
#endif

#ifdef PRE_ADD_GM_APPELLATION
	m_bGMAppellation = false;
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_GM_APPELLAThongse
	m_hongse = false;
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_GM_APPELLAchengse
	m_jinse = false;
#endif // PRE_ADD_GM_APPELLATION

#ifdef PRE_ADD_DWC
	memset(m_wszDWCTeamName, 0, GUILDNAME_MAX);
#endif
}

CDnPlayerActor::~CDnPlayerActor()
{
	// 대포 모드라면 대포 소유권도 해제.
	if( IsCannonMode() )
	{
		CDnCannonMonsterActor *pCannonActor = dynamic_cast<CDnCannonMonsterActor *>( m_hCannonMonsterActor.GetPointer() );
		if( pCannonActor ) pCannonActor->OnMasterPlayerActorDie();
	}

	SAFE_DELETE( m_pBubbleSystem );

	SAFE_RELEASE_SPTR( m_hAbsenceMarkTex );
	if( CDnInterface::IsActive() )
		GetInterface().ShowPlayerGauge( GetActorHandle() , false );

	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		DetachCashWeapon( i );
	}

	for( int i=0; i<CDnParts::DefaultPartsTypeEnum_Amount; i++ ) SAFE_RELEASE_SPTR( m_hNudeParts[i] ); 	
	SAFE_RELEASE_SPTR( m_hSwapOriginalHandle );
	SAFE_DELETE( m_pSwapOriginalAction );

	SAFE_RELEASE_SPTR( m_hDecal );
	SAFE_RELEASE_SPTR( m_hDecalTexture );

	SAFE_RELEASE_SPTR( m_hCaptainHelmetParts );
	SAFE_RELEASE_SPTR( m_hPlayerEffect );
	SAFE_RELEASE_SPTR(m_hPostParentHandle);

	SAFE_RELEASE_SPTR( m_hPartyLeaderIconTex );

	ClearFishingRod();

	if( m_bSelfDeleteVehicleEffect ) {
		SAFE_RELEASE_SPTR( m_hPartsVehicleEffect );
		m_bSelfDeleteVehicleEffect = false;
	}

	SAFE_RELEASE_SPTR( m_hSaveMergedObject );
	SAFE_RELEASE_SPTR( m_hPet );
	SAFE_RELEASE_SPTR( m_hPetEffect );

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
	SAFE_RELEASE_SPTR( m_hMailBoxOpenMarkTex );
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_CASH_COSTUME_AURA 
	for(int i = 0 ; i < eParts_Max ; ++i)
		SAFE_RELEASE_SPTR( m_hPlayerAuraEffect[i] );
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	SAFE_DELETE(m_pTotalLevelSkillSystem);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

}

void CDnPlayerActor::AttachDecal()
{
	SGraphicOption Option;
	GetEtOptionController( )->GetGraphicOption( Option );
	if( !Option.bDecalShadow ) {
		return;
	}

	m_hDecalTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "DecalShadow.dds" ).c_str(), RT_TEXTURE );
	if( !m_hDecalTexture ) {
		return;
	}	
	SAFE_RELEASE_SPTR( m_hDecal );
	m_hDecal = (new CEtDecal)->GetMySmartPtr();
	m_hDecal->Initialize( m_hDecalTexture, m_matexWorld.m_vPosition.x, m_matexWorld.m_vPosition.z, 50.0f,
		FLT_MAX, 0.0f, 0.0f, EtColor(1,1,1,1), D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, &CalcDecalHeight(), m_matexWorld.m_vPosition.y );	

}

bool CDnPlayerActor::Initialize()
{
	bool bResult = CDnActor::Initialize();
	if( bResult == false ) return false;


	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	static char *szLabel[CDnParts::DefaultPartsTypeEnum_Amount] = { "_DefaultBody", "_DefaultLeg", "_DefaultHand", "_DefaultFoot" };

	if( CTaskManager::GetInstance().GetTask( "LoginTask" ) ) {
		for( int i=0; i<CDnParts::DefaultPartsTypeEnum_Amount; i++ ) {
			if( m_hPartsObject[CDnParts::Body + i] ) continue;

			if( pSox->IsExistItem( m_nClassID ) == false ) continue;
			int nItemID = pSox->GetFieldFromLablePtr( m_nClassID, szLabel[i] )->GetInteger();
			if( nItemID < 1 ) continue;
			DnPartsHandle hParts = CDnParts::CreateParts( nItemID, -1 );
			MAPartsBody::AttachParts( hParts, (CDnParts::PartsTypeEnum)-1, true, false );
		}
		return true;
	}

	for( int i=0; i<CDnParts::DefaultPartsTypeEnum_Amount; i++ ) {
		if( pSox->IsExistItem( m_nClassID ) == false ) continue;
		int nItemID = pSox->GetFieldFromLablePtr( m_nClassID, szLabel[i] )->GetInteger();
		if( nItemID < 1 ) continue;
		m_hNudeParts[i] = CDnParts::CreateParts( nItemID, -1 );
	}

	for( int i=CDnParts::Body; i<=CDnParts::Foot; i++ )
		AttachNudeParts( (CDnParts::PartsTypeEnum)i );

	if( m_hObject ) {
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );
		m_hPostParentHandle = GetObjectHandle()->GetParent();
	}

	m_pBubbleSystem->Initialize( GetActorHandle() );

	AttachDecal();
 

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_pTotalLevelSkillSystem = new CDnTotalLevelSkillSystem(GetMySmartPtr());
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	return true;
}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
bool CDnPlayerActor::LoadAction( const char *szFullPathName )
{
	AddUsingElement( "Dummy" );
	return CDnActor::LoadAction( szFullPathName );
}
#endif

void CDnPlayerActor::OnDie( DnActorHandle hHitter )
{
	CDnActor::OnDie( hHitter );

	// NOTE: 걸려 있는 상태효과 모두 없앤다. 부활하고나면 패시브 스킬은 다시 걸어줘야 함..
	RemoveAllBlowExpectPassiveSkill();

	// 오라스킬, 토글스킬이 켜져 있다면 꺼준다.
	if( IsEnabledToggleSkill() )
		OnSkillToggle( m_hToggleSkill, false );

	if( IsEnabledAuraSkill() )
		OnSkillAura( m_hAuraSkill, false );

	SetSP(0);

	// 내구도 감소시켜준다.
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pTask ) {
		switch( pTask->GetGameTaskType() ) 
		{
			case GameTaskType::Normal:
			{
				// 스킬 쿨 타임도 모두 리셋
				ResetSkillCoolTime();
				OnDecreaseEquipDurability( GetDeadDurabilityRatio() );
				break;
			}
			case GameTaskType::DarkLair:
				// 스킬 쿨 타임도 모두 리셋
				ResetSkillCoolTime();
				break;
			case GameTaskType::PvP:
				break;
		}
	}

	if( IsCannonMode() )
	{
		CDnCannonMonsterActor* pCannonActor = dynamic_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer());
		if( pCannonActor ) pCannonActor->OnMasterPlayerActorDie();
		EndCannonMode();
	}

	m_pBubbleSystem->Clear();
	ToggleLinkedPlayerEffect(false);
	GetInterface().UpdateMyPortrait();
}

void CDnPlayerActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	EtVector3 vPrevPos = m_matexWorld.m_vPosition;
	CDnActor::ProcessActor( LocalTime, fDelta );

	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ )
	{
		if( m_hCashWeapon[i] ) 
			m_hCashWeapon[i]->Process( LocalTime, fDelta );
	}

	PreProcess( LocalTime, fDelta );

	if( 0 < GetCantXZMoveSEReferenceCount() )
		m_vAniDistance.x = m_vAniDistance.z = 0.0f;

#ifdef PRE_ADD_MONSTER_CATCH
	if(!IsVehicleMode() && !m_hCatcherMonster )
		m_pMovement->Process( LocalTime, fDelta );
#else
	if(!IsVehicleMode())
		m_pMovement->Process( LocalTime, fDelta );
#endif // #ifdef PRE_ADD_MONSTER_CATCH
		
	
	MAPartsBody::Process( m_matexWorld, LocalTime, fDelta );
	MAPlateUser::Process( LocalTime, fDelta );

	ProcessHeadLook( m_matexWorld, fDelta, IsSignalRange( STE_HeadLook ) );

	ProcessCombo( LocalTime, fDelta );

	ProcessHidePlayer( LocalTime, fDelta );

	ProcessVisual( LocalTime, fDelta );	

	ProcessDecal();

	ProcessPVPChanges(LocalTime,fDelta);
	
	ProcessVehicleCall(LocalTime,fDelta);
	ProcessFishing( LocalTime, fDelta );

	ProcessPet( LocalTime, fDelta );
	ProcessNonLocalShootModeAction();
	if( m_bRefreshTransformMode )
		RefreshTransformMode();

#ifdef PRE_ADD_TRANSFORM_POTION
	if( m_bRefreshVillageTransformMode == true && m_nTransformItemIndex > 0 )
		ToggleVillageTransformMode( true , m_nTransformItemIndex , 0 );
#endif

	ProcessEffectSkill( LocalTime, fDelta );

	m_pBubbleSystem->Process( LocalTime, fDelta );

	// #41767 플레이어가 은신 상태효과가 사용될 경우가 있으므로 딸린 이펙트들 같이 처리.
	if ( IsAppliedThisStateBlow( STATE_BLOW::BLOW_073 ) || s_bHideAnotherPlayer )
	{
		float fAlpha = GetAlpha();
		if( fAlpha != 1.0f )
			ApplyAlphaToSignalImps( fAlpha );
	}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (m_pTotalLevelSkillSystem)
		m_pTotalLevelSkillSystem->Process(LocalTime, fDelta);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

}

void CDnPlayerActor::PreProcess( LOCAL_TIME LocalTime, float fDelta )
{
	MAPartsBody::PreProcess( LocalTime, fDelta );
}

void CDnPlayerActor::SetDestroy()
{
	CDnActor::SetDestroy();
	if( s_bHideAnotherPlayer || ( IsDeveloperAccountLevel() && !IsProcess() ) ) {
		CDnUnknownRenderObject<CDnActor>::SetDestroy();
	}
}
void CDnPlayerActor::ProcessHidePlayer( LOCAL_TIME LocalTime, float fDelta )
{
	if( !s_bEnableAnotherPlayer ) return;
	if( s_hLocalActor == this ) return;
	if( IsDeveloperAccountLevel() && !CDnActor::FindActorFromUniqueID( GetUniqueID() ) ) return;

	bool bShow = true;
	bool bCastShadow = true;
	float fAlpha = 1.f;
	if ( s_fHideAnotherPlayerDelta > 0.0f )
	{
		s_fHideAnotherPlayerDelta -= fDelta;
		if( s_fHideAnotherPlayerDelta < 0.0f ) s_fHideAnotherPlayerDelta = 0.0f;

		if( s_bHideAnotherPlayer ) {
			bCastShadow = false;
			fAlpha = s_fHideAnotherPlayerDelta;
		}
		else {
			bShow = true;
			fAlpha = 1.f - s_fHideAnotherPlayerDelta;
		}

		if( s_fHideAnotherPlayerDelta == 0.f ) {
			if( s_bHideAnotherPlayer ) bShow = false;
			else bCastShadow = true;
		}
	}
	else {
		if( s_bHideAnotherPlayer ) {
			bShow = false;
			bCastShadow = false;
			fAlpha = 0.f;
		}
		else {
			bShow = true;
			bCastShadow = true;
			fAlpha = 1.f;
		}
	}

	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );

	CDnPlayerActor *pLocalPlayer = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	if( pLocalPlayer && pGameTask && GetActorHandle() != CDnActor::s_hLocalActor && pGameTask->GetGameTaskType() == GameTaskType::PvP ) {
		if( GetTeam() != PvPCommon::Team::Observer ) {
			if( IsGhost() ) {
				CDnPvPGameTask *pPvpGameTask = (CDnPvPGameTask*)pGameTask;
				if( pPvpGameTask && pPvpGameTask->GetGameMode() != PvPCommon::GameMode::PvP_AllKill )
				{
					bShow = false;
					bCastShadow = false;
					fAlpha = 0.f;
				}	
			}
			else
			{
				if(!s_bHideAnotherPlayer && bShow != IsShow())
					bShow = IsShow(); 
				// PVP 에서는 HideAnotherPlayer를 사용하지 않는다 , 하지만 false 값으로 Process가 돌게되면 
				// bShow 는 항상 true 값이 되게되며 , PVP 에서 난입해 들어온 유저같은경우는 숨겨야 하지만 이후에 프로세스가 돌면서
				// 강제로 Show(true) 가 호출되는 경우가 생긴다.
				// 그래서 <s_bHideAnotherPlayer> 상태가 아니라면 현재의 Show 를 유지하도록 설정해둔다.
			}
		}
		else bShow = false;
	}

	if( IsShow() != bShow ) Show( bShow );
	if( IsEnableCastShadow() != bCastShadow ) EnableCastShadow( bCastShadow );
	if( GetAlpha() != fAlpha ) SetAlphaBlend( fAlpha );

}

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
bool CDnPlayerActor::PlayerSkillEffectCheck( SignalTypeEnum Type, void *pPtr )
{
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( pTask && pTask->GetGameTaskType() != GameTaskType::PvP )	
	{ 
		if( ! CGameOption::GetInstance().m_bEnablePlayerEffect )
		{
			if( CDnActor::s_hLocalActor->GetUniqueID() != this->GetUniqueID() )
				if( ! SetEnableSignal( Type, pPtr ) )		// 시그널의 bool 형 상태를 지정해주는 이유는 특정 몇몇 스킬들은 반드시 보여져야 할 필요가 있기 때문
					return false;
		}			
	}

	return true; 
}
#endif

void CDnPlayerActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{

#ifdef PRE_ADD_ENABLE_EFFECT_SIGNAL
	if( ! PlayerSkillEffectCheck( Type, pPtr ) )
		return;
#endif 

	switch( Type ) {
		case STE_Jump:
			{
				JumpStruct *pStruct = (JumpStruct *)pPtr;
				EtVector2 vVec( 0.f, 0.f );

				// 애니메이션 키 까지 이동 불가일때는 방향키 먹지 않도록 처리한다.
				if( 0 == GetCantXZMoveSEReferenceCount() )
				{
					if( !pStruct->bIgnoreJumpDir ) {
						if( m_cMovePushKeyFlag & 0x01 ) vVec.x -= 1.f;
						if( m_cMovePushKeyFlag & 0x02 ) vVec.x += 1.f;
						if( m_cMovePushKeyFlag & 0x04 ) vVec.y += 1.f;
						if( m_cMovePushKeyFlag & 0x08 ) vVec.y -= 1.f;
						EtVec2Normalize( &vVec, &vVec );
					}
				}
				
				if( pStruct->bResetPrevVelocity ) {
					SetVelocityY( 0.f );
					SetResistanceY( 0.f );
				}

				Jump( pStruct->fJumpVelocity, vVec );
				SetResistanceY( pStruct->fJumpResistance );
			}
			return;
		case STE_VelocityAccel:
			{
				VelocityAccelStruct *pStruct = (VelocityAccelStruct *)pPtr;

				MAWalkMovement *pMovement = dynamic_cast<MAWalkMovement *>(GetMovement());

				// #24949 VelocityAccel 시그널 사용하는 부분도 막도록 처리.
				EtVector3 vVelocity = *pStruct->vVelocity;
				EtVector3 vResistance = *pStruct->vResistance;
				if( 0 < GetCantXZMoveSEReferenceCount() )
				{
					vVelocity.x = vVelocity.z = 0.0f;
					vResistance.x = vResistance.z = 0.0f;
				}

				if( pMovement )
				{
					pMovement->SetVelocityByMoveVector( vVelocity );
					pMovement->SetResistance( vResistance );
				}
			}
			return;
		case STE_CustomAction:
			{
				CustomActionStruct *pStruct = (CustomActionStruct *)pPtr;

				ResetCustomAction();
				ResetMixedAnimation( false );
				// 반드시 CDnPlayerActor::CmdMixedAction 해줘야 한다. 이 시그널은 로컬플에이어에 영향받으면 안되기땜시( 페킷을 보내버린다. )
				CDnPlayerActor::CmdMixedAction( pStruct->szActionBoneName, pStruct->szMaintenanceBoneName, pStruct->szChangeAction, 0, (float)pStruct->nChangeActionFrame, (float)pStruct->nBlendFrame );
				SetCustomAction( pStruct->szChangeAction, (float)pStruct->nChangeActionFrame );
			}
			return;

		case STE_ShowWeapon:
			{
				// 플레이어인 경우 캐쉬무기까지 처리. #23810
				ShowWeaponStruct *pStruct = (ShowWeaponStruct *)pPtr;

#ifdef PRE_ADD_37745
				if( pStruct->EnableByShowOption == TRUE && ( CGameOption::GetInstance().m_bHideWeaponByBattleMode == false || m_bRTTRenderMode ) )
					return;
#else
				if( pStruct->EnableByShowOption == TRUE )
					return;
#endif
				
				bool bShow = ( pStruct->bEnable == TRUE ) ? true : false;
				ShowWeapon( pStruct->nEquipIndex, bShow );
				ShowCashWeapon( pStruct->nEquipIndex, bShow );

				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
				{
					m_bHideWeaponBySignalInCutScene[pStruct->nEquipIndex] = !pStruct->bEnable;
				}
			}
			return;
		case STE_SkillChecker:
			{
				SkillCheckerStruct* pStruct = (SkillCheckerStruct*)pPtr;
				DnSkillHandle hSkill = FindSkill(pStruct->nSkillID);
				if (hSkill)
				{
					//스킬 사용 불가능이면 설정된 동작으로 변경..
					if (CDnSkill::UsingResult::Success != hSkill->CanExecute())
					{
						CmdAction(pStruct->szChangeAction);
					}
				}
			}
			return;

			// #29925 이 시그널에 지정된 상태효과가 없으면 지정된 액션을 실행.
			// 서버와 클라 각자 따로 행한다.
		case STE_ChangeActionSECheck:
			{
				ChangeActionSECheckStruct* pStruct = (ChangeActionSECheckStruct*)pPtr;
				if( false == IsAppliedThisStateBlow( (STATE_BLOW::emBLOW_INDEX)pStruct->nStateEffectID ) )
				{
					SetActionQueue( pStruct->szChangeAction );
				}
			}
			break;

		case STE_CancelChangeStandEndActionSE:
			{
				CancelChangeStandEndActionSEStruct* pStruct = (CancelChangeStandEndActionSEStruct*)pPtr;
				if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_121 ) )
				{
					DNVector( DnBlowHandle ) vlhAllAppliedBlows;
					GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_121 , vlhAllAppliedBlows );

					for( int i = 0; i < (int)vlhAllAppliedBlows.size(); ++i )
					{
						if( vlhAllAppliedBlows[i] )
							static_cast<CDnChangeStandActionBlow*>( vlhAllAppliedBlows[i].GetPointer() )->SetEndAction( pStruct->szEndAction );
					}
				}
			}
			break;
		case STE_Destroy:
			{
				if( IsSwapSingleSkin() ) // 디스트로이 되면 안된다.
					return;
				break;
			}
	}
	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnPlayerActor::OnDrop( float fCurVelocity )
{
	if( IsAir() ) {
		if( !IsHit() ) 
		{

			// #32977 - 탑 스피닝 스킬을 시작 해놓은 상태에서 탑 스피닝 액션의 시그널이 처리 되기전
			// OnDrop이 호출 되면 탑 스피닝에 있는 STE_Jump가 처리가 되지 않아서 동작이 이어지지 않게 된다.
			// 그래서 일단..
			// 현재 Frame이 0이고, STE_Jump시그널이 0프레임에 있으면 _Landing동작으로 변경을 막고
			// 현재 동작을 계속 유지 하도록 한다.
			float fCurrentFrame = CDnActionBase::GetCurFrame();
			float fPrevFrame = CDnActionBase::m_fPrevFrame;
			ActionElementStruct *pStruct = GetElement(GetCurrentAction());
			if (pStruct)
			{
				bool hasJumpSignal = false;
				CEtActionSignal *pSignal = NULL;
				for (int i = 0; i < (int)pStruct->pVecSignalList.size(); ++i)
				{
					pSignal = pStruct->pVecSignalList[i];
					if (pSignal && pSignal->GetSignalIndex() == STE_Jump)
					{
						hasJumpSignal = true;
						break;
					}
				}

				if (hasJumpSignal)
				{
					//STE_Jump가 있고, actionQueue가 있으면 스킵..(자동으로 다음 액션으로 바뀌겠지?..)
					if (false == m_szActionQueue.empty())
					{
						return;
					}
					//현재 액션이 STE_Jump를 가지고 있고, 0프레임이면 STE_Jump 호출될 수 있도록..
					else if (false == m_szAction.empty() && fPrevFrame == 0.0f)
					{
						return;
					}
				}
			}

			char szStr[128];

			sprintf_s( szStr, "%s_Landing", GetCurrentAction() );
			if( IsExistAction( szStr ) )
			{
				SetActionQueue( szStr, 0, 0.f, 0.f, true, false );
			}
			else {// 만약에 없을경우에 하늘에서 병신짓하구있어서 넣어놉니다. 일단은 마춰서 넣어주는거임
				if( GetVelocity()->y != 0.f )
					SetActionQueue( "Stand", 0, 0.f, 0.f, true, false );
			}

			if( !GetLastFloorForceVelocity() ) {
				TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition );

				CreateEnviEffectParticle( ConditionEnum::Landing, TileType, &m_matexWorld );
				CreateEnviEffectSound( ConditionEnum::Landing, TileType, m_matexWorld.m_vPosition );
			}
			SetMovable( false );
		}
		else {
			std::string szAction;
			float fBlendFrame = 2.f;
			// 떨어지는 속도가 10이상이면 bigBounce로 한번 더 띄어준다.
			if( fCurVelocity < -6.f && m_HitParam.vVelocity.y != 0.f && abs(m_HitParam.vVelocity.y) > 0.1f ) {
				if( m_HitParam.vVelocity.y > 0.f ) {
					m_HitParam.vVelocity.y *= 0.6f;
					SetVelocityY( m_HitParam.vVelocity.y );
				}
				else { // 가속도가 처음부터 바닥으로 향해있는 경우에는 뒤집어줘야한다.
					m_HitParam.vVelocity.y *= -0.6f;
					if( m_HitParam.vResistance.y > 0.f )
						m_HitParam.vResistance.y *= -1.f;
					SetVelocityY( m_HitParam.vVelocity.y );

					if( m_HitParam.vVelocity.y > 0 && m_HitParam.vResistance.y <= 0 )
						SetResistanceY( -15.0f );
					else
						SetResistanceY( m_HitParam.vResistance.y );

				}
				szAction = "Hit_AirBounce";
			}
			else {
				szAction = "Down_SmallBounce";
				fBlendFrame = 0.f;
			}

			SetActionQueue( szAction.c_str(), 0, fBlendFrame, 0.f, true, false );


			TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition );

			CreateEnviEffectParticle( ConditionEnum::Down, TileType, &m_matexWorld );
			CreateEnviEffectSound( ConditionEnum::Down, TileType, m_matexWorld.m_vPosition );
		}
	}
}

void CDnPlayerActor::OnStop( EtVector3 &vPosition )
{
	if( IsProcessSkill() ) return;
	CmdStop( "Stand" );
}

void CDnPlayerActor::OnFall( float fCurVelocity )
{
	// #31056 Move 이고 Air 이면 공중에서 이동하는 액션이므로 Fall 처리 하지 않는다. (높은 곳에서 떨어질 때 탑스피닝 사용)
	if( !(IsMove() && IsAir()) &&
		(IsStay() || IsMove()) &&
		!IsFloorCollision() )
	{
		// 움직여지는 각도도 체크해서 계단등을 내려올때 떨어져보이는것 보정해보아요.
		EtVector3 vDir = *GetPosition() - *GetPrevPosition();
		EtVec3Normalize( &vDir, &vDir );
		float fDot = EtVec3Dot( &EtVector3( 0.f, 1.f, 0.f ), &vDir );
		float fAngle = EtToDegree( EtAcos( fDot ) );

		if( fCurVelocity < -5.f && fAngle > 155.f ) {

			std::string szJump = "Jump";
		
			if(!IsBattleMode())
				szJump = "Normal_Jump";

			ActionElementStruct *pStruct = GetElement( szJump.c_str() );
			if( pStruct ) {
				SetActionQueue( szJump.c_str(), 0, 6.f, (float)pStruct->dwLength / 2.f );
				SetMovable( false );
			}
		}
	}
}

void CDnPlayerActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	CDnActor::OnDamage( pHitter, HitParam );
	
	// ScoreSystem
	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	DnActorHandle hHitter = pHitter ? pHitter->GetActorHandle() : CDnActor::Identity();
	if( pGameTask && HitParam.bRecvOnDamage )
	{		
		pGameTask->OnDamage( GetActorHandle(), hHitter, HitParam.nDamage );
	}

	// Note : 데미지 출력
	//	
	EtVector3 vPos = GetHeadPosition();

	// 렐릭이 때렸다면 소환한 액터를 hitter 로 설정한다.
	if( hHitter && hHitter->IsMonsterActor() )
	{
		DnActorHandle hMaster = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
		if( hMaster && hMaster->IsPlayerActor() )
			hHitter = hMaster;
	}

	bool bMeOrParty = (GetMySmartPtr() == CDnActor::s_hLocalActor) || (hHitter == CDnActor::s_hLocalActor);	
	bool bHitOrDamage = (GetTeam() != CDnActor::s_hLocalActor->GetTeam());

	GetInterface().SetDamage( 
		vPos, 
		(int)HitParam.nDamage, 
		HitParam.HitType==CDnWeapon::Critical, 
		HitParam.HitType==CDnWeapon::CriticalRes, 
		bHitOrDamage, 
		bMeOrParty, 
		HitParam.HasElement,
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
		HitParam.bStageLimit
#else
		false
#endif
		);
	GetInterface().OnDamage( GetMySmartPtr() );

	if( !(GetMySmartPtr() == CDnActor::s_hLocalActor) )
	{	
		SetExposure(CDnActor::s_hLocalActor && pHitter && pHitter->GetActorHandle() == CDnActor::s_hLocalActor);
	}

	ResetCustomAction();
	ResetMixedAnimation();
}

void CDnPlayerActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MovePos( vPos, false );
	SetActionQueue( szActionName, nLoopCount, fBlendFrame );
}


void CDnPlayerActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bResetStop, bool bForce )
{
	MAMovementBase *pMovement = GetMovement();
	if( !pMovement ) return;

	pMovement->ResetMove();
	SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame );

}

void CDnPlayerActor::CmdLook( EtVector2 &vVec, bool bForce )
{
	Look( vVec, bForce );
}

void CDnPlayerActor::CmdToggleBattle( bool bBattleMode )
 {
	if( bBattleMode == m_bBattleMode ) return;
	if( bBattleMode && !IsCanBattleMode() ) return;

	SetBattleMode( bBattleMode );

	if( !IsDie() && !IsHit() ) {
		
		bool bSkipAction = false;

#if defined(PRE_ADD_50907)
		bool bSkipChangeAction = IsSkipChangeWeaponAction();
		if (bSkipChangeAction == true)
			return;
#endif // PRE_ADD_50907
		if(IsShootMode())
		{
			// ShootMode는 전투상태에만 사용하기때문에 Normal상태에서 무기를 꺼내는 행동은 GetchangeShootaction 에서 설정하지 못하기때문에 여기서 넣는다.
			if( m_bBattleMode == true ) SetActionQueue( "MOD_PullOut_Weapon", 0, 0.f, 0.f, false, false );
			else SetActionQueue( "MOD_PutIn_Weapon", 0, 0.f, 0.f, false, false ); 
			bSkipAction = true;
		}
		if(!bSkipAction)
		{
			if( m_bBattleMode == true ) SetActionQueue( "PullOut_Weapon", 0, 0.f, 0.f, false, false );
			else SetActionQueue( "PutIn_Weapon", 0, 0.f, 0.f, false, false );
		}

		if( IsCustomAction() ) {
			ResetCustomAction();
			ResetMixedAnimation();
		}
	}
}

void CDnPlayerActor::CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bChargeKey, bool bCheckOverlapAction, bool bOnlyCheck )
{
	if( false == bOnlyCheck )
	{
		DnSkillHandle hSkill = FindSkill( nSkillID );
		if( !hSkill ) return;

		ExecuteSkill( hSkill, CDnActionBase::m_LocalTime, 0.0f, true, false, false );

		// 플레이어가 사용하는 패시브/즉시 스킬을 위해 액션 이름 기입해 줌.
		hSkill->SetPassiveSkillActionName( szActionName );
	}

	SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, bCheckOverlapAction );
	m_fDownDelta = 0.f;
}


void CDnPlayerActor::CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash )
{
	if( bShowCash && !m_hCashWeapon[nEquipIndex] ) return;
	if( !bShowCash && !m_hWeapon[nEquipIndex] ) return;

	SetWeaponViewOrder( nEquipIndex, bShowCash );
	RefreshWeaponViewOrder( nEquipIndex );
}

void CDnPlayerActor::CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash )
{
	if( bShowCash && !m_hCashPartsObject[nEquipIndex] ) return;
	if( !bShowCash )
	{
		// 모자는 디폴트파츠처럼 일반템이 없어도 전환가능하게한다.
		// 디폴트 파츠가 있는 거라 좀 다르게 체크한다.
		if( nEquipIndex >= CDnParts::CashBody && nEquipIndex <= CDnParts::CashFoot )
		{
			if( !m_hPartsObject[nEquipIndex+2] ) return;
			// 누드파츠로도 바꿀 수 있게 해달라고 한다.
			//if( IsNudeParts( m_hPartsObject[nEquipIndex+2] ) ) return;
		}
		if( nEquipIndex >= CDnParts::CashNecklace && nEquipIndex <= CDnParts::CashRing2 )
		{
			if( !m_hPartsObject[nEquipIndex+2] ) return;
		}
	}

	SetPartsViewOrder( nEquipIndex, bShowCash );
	RefreshPartsViewOrder( nEquipIndex );
}

void CDnPlayerActor::CmdToggleHideHelmet( bool bHideHelmet )
{
	if( !m_hPartsObject[CDnParts::Helmet] && !m_hCashPartsObject[CDnParts::CashHelmet] ) return;

	SetHideHelmet( bHideHelmet );
	RefreshHideHelmet( true );
}

void CDnPlayerActor::CmdWarp( EtVector3 &vPos, EtVector2 &vLook )
{
	if( IsVehicleMode() && GetMyVehicleActor() )
	{
		GetMyVehicleActor()->SetPosition( vPos );
		GetMyVehicleActor()->SetPrevPosition( vPos );

		if( EtVec2LengthSq( &vLook ) > 0.f )
		{
			GetMyVehicleActor()->Look( vLook );
			CDnLocalPlayerActor::ResetCameraLook( true );
		}

		if( abs( CDnWorld::GetInstance().GetHeight( vPos ) - vPos.y ) > 5.f )
		{
			GetMyVehicleActor()->SetVelocityY( -3.0f );
		}
		GetMyVehicleActor()->SetResistanceY( -18.0f );

		if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
		{
			if( IsLocalActor() )
			{
				GetMyVehicleActor()->CmdStop( "Stand" );
			}
		}

		return; // return;
	}

	CDnActor::CmdWarp( vPos , vLook );  // CDnActor::CmdWarp

	if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
	{
		if( IsLocalActor() )
		{
			if( EtVec2LengthSq( &vLook ) > 0.f )
			{
				CDnLocalPlayerActor::ResetCameraLook( true );
			}
		
			CmdStop( "Stand", 0, 3.f, 0.f, false, true );
		}
	}
}

void CDnPlayerActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_CMDMOVE:
			{
				if(IsVehicleMode() || IsCallingVehicle()) // 탈것을 타거나 부르는 도중에 캐릭터가 CMDMOVE 패킷을 받을 일도 없고 받아야 하지 않지만 
					break;                                // PositionRevision 에 걸려서 이패킷을 보내는 경우가있다. 받아야 하지 않아야 할경우에 패킷 딜레이에 걸려서 받는경우가 
				                                          // 생기며 이럴때는 말에 타고있는데 엄한 액션이 실행되는 경우가 있으니 막아두자.

				CPacketCompressStream Stream( pPacket, 128 );

				int			nActionIndex;
				EtVector3	vPos, vXVec;
				EtVector2	vZVec, vLook;
				char		cFlag;
				DWORD		dwGap;
				int			nMoveSpeed;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &cFlag, sizeof(char) );
				Stream.Read( &nMoveSpeed, sizeof(int) );

				if( !IsBattleMode() ) {
					if( cFlag ) Look( vZVec, false );
				}
				else Look( vLook, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetMagnetPosition( vPos );

				m_cMovePushKeyFlag = cFlag;
				float fXSpeed = 0.f, fZSpeed = 0.f;
				if( cFlag & 0x01 ) fXSpeed = -100000.f;
				if( cFlag & 0x02 ) fXSpeed = 100000.f;
				if( cFlag & 0x04 ) fZSpeed = 100000.f;
				if( cFlag & 0x08 ) fZSpeed = -100000.f;
				vPos += ( vXVec * fXSpeed );
				vPos += ( EtVec2toVec3( vZVec ) * fZSpeed );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;
				CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
				if( pTask ) {
					if( nActionIndex == m_nActionIndex ) {
						if( EtVec3Length( &(EtVector3)(m_matexWorld.m_vPosition - vPos) ) < 100.f ) break;
					}
				}
				CmdMove( vPos, pStruct->szName.c_str(), -1, 8.f );
			}
			break;
		case eActor::SC_CMDMOVEPOS:
			{
				
				CPacketCompressStream Stream( pPacket, 128 );

				int			nActionIndex;
				EtVector3	vPos, vXVec, vTargetPos;
				EtVector2	vZVec;
				DWORD		dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vTargetPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				Look( vZVec, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetMagnetPosition( vPos );

				m_cMovePushKeyFlag = 0;

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;
				CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
				if( pTask ) {
					if( nActionIndex == m_nActionIndex ) {
						MovePos( vPos, false );
						break;
					}
				}
				CmdMove( vTargetPos, pStruct->szName.c_str(), -1, 8.f );
			}
			break;
		case eActor::SC_CMDSTOP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3	vPos;
				bool		bResetStop;
				DWORD		dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &bResetStop, sizeof(bool) );

				//죽은 상태에서는 스탑이 와도 해당 애니메이션은 그대로 출력
				if( !IsDie() || IsGhost() )
				{			
					std::string szActionName = GetCurrentAction();
					if( EtVec2Length( &( EtVector2( vPos.x, vPos.z ) - EtVector2( GetPosition()->x, GetPosition()->z ) ) ) > 100.f ) 
					{
						if( GetState() != ActorStateEnum::Move ) szActionName = "Move_Front";
					}
					CmdMove( vPos, szActionName.c_str(), -1, CDnActionBase::m_fQueueBlendFrame );
				}
				if( bResetStop ) 
					OnStopReady();
			}
			break;
		case eActor::SC_CMDACTION:
			{
				if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage ) {
					if( CDnPartyTask::IsActive() && !CDnPartyTask::GetInstance().IsSyncComplete() ) break;
				}
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				float fBlendFrame;
				EtVector3 vPos, vXVec;
				EtVector2 vLook, vZVec;
				DWORD dwGap;
				bool bFromStateBlow = false;
				bool bSkillChain = false;
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &m_cMovePushKeyFlag, sizeof(char) );
				Stream.Read( &bFromStateBlow, sizeof(bool) );
				Stream.Read( &bSkillChain, sizeof(bool) );						// 클라에서는 사용하지 않음.

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				ResetMove();

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
				Look( vLook, true );
				SetMagnetPosition( vPos );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				if( IsCustomAction() ) {
					ResetCustomAction();
					ResetMixedAnimation();
				}

				CmdAction( pStruct->szName.c_str(), nLoopCount, fBlendFrame, false, false, false );

				// 서버나 클라 둘 중에 상태효과쪽에서 발생시킨 액션.
				if( bFromStateBlow )
					m_pStateBlow->OnCmdActionFromPacket( pStruct->szName.c_str() );
			}
			break;
		case eActor::SC_CMDMIXEDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nMaintenanceBone, nActionBone;
				float fFrame, fBlendFrame;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nActionBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nMaintenanceBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &m_cMovePushKeyFlag, sizeof(char) );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				std::string szAction = pStruct->szName;
				m_szActionBoneName = GetBoneName(nActionBone);
				m_szMaintenanceBoneName = GetBoneName(nMaintenanceBone);
				if( IsCustomAction() ) {
					ResetCustomAction();
					bool bBlend = true;
					if( nActionIndex < (int)m_nVecAniIndexList.size() ) {
						bBlend = !IsExistMixedAnimation( nActionBone, m_nVecAniIndexList[nActionIndex] );
					}
					ResetMixedAnimation( bBlend );
				}
				CmdMixedAction( m_szActionBoneName.c_str(), m_szMaintenanceBoneName.c_str(), szAction.c_str(), 0, fFrame, fBlendFrame );
				SetCustomAction( szAction.c_str(), fFrame );
			}
			break;
		case eActor::SC_PROJECTILE:
			{
				CDnProjectile* pProjectile = CDnProjectile::CreateProjectileFromServerPacket( GetMySmartPtr(), pPacket );
				if( pProjectile )
					OnSkillProjectile( pProjectile );
			}
			break;
		case eActor::SC_CMDLOOK:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector2 vLook, vZVec;
				EtVector3 vXVec;

				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );                         
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				CmdLook( vLook );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
			}
			break;

		case eActor::SC_POSREV:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				bool bMove;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &bMove, sizeof(bool) );

				SetMagnetPosition( vPos );
				if( bMove ) {
					EtVector3 vXVec;
					EtVector2 vZVec;

					Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

					EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

					SetMoveVectorX( vXVec );
					SetMoveVectorZ( EtVec2toVec3( vZVec ) );

					if( EtVec3LengthSq( GetMovePos() ) > 0.f ) {
						float fXSpeed = 0.f, fZSpeed = 0.f;
						if( m_cMovePushKeyFlag & 0x01 ) fXSpeed = -100000.f;
						if( m_cMovePushKeyFlag & 0x02 ) fXSpeed = 100000.f;
						if( m_cMovePushKeyFlag & 0x04 ) fZSpeed = 100000.f;
						if( m_cMovePushKeyFlag & 0x08 ) fZSpeed = -100000.f;
						vPos += ( vXVec * fXSpeed );
						vPos += ( EtVec2toVec3( vZVec ) * fZSpeed );
						MovePos( vPos, false );
					}
				}

				OutputDebug( "PosRev\n" );
			}
			break;
		case eActor::SC_VIEWSYNC:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos, vXVec;
				EtVector2 vZVec, vLook;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				Look( vLook, true );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetMagnetPosition( vPos );

				OutputDebug( "ViewSync\n" );

			}
			break;
		case eActor::SC_CMDTOGGLEBATTLE:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				
				bool bBattle;
				Stream.Read( &bBattle, sizeof(bool) );

				CmdToggleBattle( bBattle );
			}
			break;
		case eActor::SC_BLOW_GRAPHIC_ERASE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int emBlowIndex;
				Stream.Read( &emBlowIndex, sizeof(int) );

				DNVector(DnBlowHandle) vBlow;
				GatherAppliedStateBlowByBlowIndex( (STATE_BLOW::emBLOW_INDEX)emBlowIndex, vBlow );

				for( int itr = 0; itr < (int)vBlow.size(); ++itr )
					vBlow[itr]->DetachGraphicEffectDefaultType();
			}
			break;
		case eActor::SC_CMDPASSIVESKILLACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount, nSkillID;
				BYTE cLevel;
				float fBlendFrame;
				float fStartFrame;
				bool bChargeKey;
				EtVector3 vPos, vXVec;
				EtVector2 vLook, vZVec;
				Stream.Read( &nSkillID, sizeof(int) );
				Stream.Read( &cLevel, sizeof(char) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nLoopCount, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &fStartFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.0f );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &m_cMovePushKeyFlag, sizeof(char) );
				Stream.Read( &bChargeKey, sizeof(bool) );

				bool bOnlyCheck = false;
				Stream.Read( &bOnlyCheck, sizeof(bool) );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				ResetMove();

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
				Look( vLook, true );
				SetMagnetPosition( vPos );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				if( IsCustomAction() ) {
					ResetCustomAction();
					ResetMixedAnimation();
				}

				AddSkill( nSkillID, cLevel );

				CmdPassiveSkillAction( nSkillID, pStruct->szName.c_str(), nLoopCount, fBlendFrame, fStartFrame, bChargeKey, false, bOnlyCheck );
			}
			break;
		case eActor::SC_LEVELUP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nLevel;

				Stream.Read( &nLevel, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

				int nLevelAmount = nLevel - GetLevel();
				SetLevel( nLevel );
				OnLevelUp( nLevel, nLevelAmount );

				CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
				if( pTask ) pTask->UpdatePartyMemberInfo();
			}
			break;
		case eActor::SC_CHANGEJOB:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nJob;
				Stream.Read( &nJob, sizeof(int) );
				SetJobHistory( nJob );
				OnChangeJob( nJob );

				CDnPartyTask* pTask = (CDnPartyTask*)CTaskManager::GetInstance().GetTask( "PartyTask" );
				if( pTask )
					pTask->UpdatePartyMemberInfo();
			}
			break;
		case eActor::SC_ADDCOIN:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				INT64 nTotalCoin;
				INT64 nAddCoin;
				Stream.Read( &nTotalCoin, sizeof(INT64) );
				Stream.Read( &nAddCoin, sizeof(INT64) );

				CDnItemTask::GetInstance().OnAddCoin(this);
				CDnItemTask::GetInstance().SetCoin( nTotalCoin );
				CDnItemTask::GetInstance().AddCoin( nAddCoin );
			}
			break;
		case eActor::SC_USESKILL:
			{
				// 이 패킷의 내용이 바뀌면 게임 서버의 MASkillUser::ProcessAutoPassiveSkill() 에서 클라로 쏘는 패킷도 같이 변경되어야 합니다.
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillTableID = 0;
				int nEnchantSkillID = 0;
				BYTE cLevel;
				EtVector2 vLook, vZVec;
				EtVector3 vXVec;
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
				EtVector3 vPos;
#endif
				bool bUseApplySkillItem = false;
				char cUseSignalSkill = -1;
				bool bAutoUseFromServer = false;
				bool abSignalSkillCheck[ 3 ] = { false };

				Stream.Read( &nSkillTableID, sizeof(int) );
				Stream.Read( &cLevel, sizeof(char) );
				Stream.Read( &bUseApplySkillItem, sizeof(bool) );
				Stream.Read( abSignalSkillCheck, sizeof(abSignalSkillCheck) );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &bAutoUseFromServer, sizeof(bool) );
				Stream.Read( &nEnchantSkillID, sizeof(int) );
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				SetMagnetPosition( vPos );
#endif

				Look( vLook, true );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				// Note: ApplySkill 아이템인 경우 스킬 사용으로 패킷이 날아옵니다..
				if( bUseApplySkillItem )
				{
					UseItemSkill( nSkillTableID, cLevel, CDnItem::ItemSkillApplyType::ApplySkill );
				}
				else
				{
					bool bExistSkill = IsExistSkill( nSkillTableID, cLevel );
					bool bAddSuccess = true;
					if( false == bExistSkill )
					{
						// PVE/PVP 나뉘어진 상태임. 구분해서 넣어준다.
						// 스킬 레벨 데이터 디폴트는 무조건 PVE 
						CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
						bool bIsPVPGame = ((pGameTask) && (pGameTask->GetGameTaskType() == GameTaskType::PvP));
						int iSkillLevelDataType = CDnSkill::PVE;
						if( bIsPVPGame )
						{
							iSkillLevelDataType = CDnSkill::PVP;
						}

						//UseSkill에 Level정보는 레벨업이 된 최종 값이 담겨 온다.
						//그래서 LevelUp정보를 찾아서 원래 레벨을 재계산 해야함.
						int nLevelUpValue = GetSkillLevelUpValue(nSkillTableID);
						cLevel -= nLevelUpValue;

						bAddSuccess = AddSkill( nSkillTableID, cLevel, iSkillLevelDataType );

						// #38294 빌리지 서버 ex 스킬 액션 동기 맞추기 용. 그 이상의 의미는 없음.
						if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
						{
							if( 0 < nEnchantSkillID )
								AddSkill( nEnchantSkillID, 1, iSkillLevelDataType );
						}
					}
					
					if( bExistSkill || bAddSuccess )
					{
						UseSkill( nSkillTableID, false, bAutoUseFromServer );
					}
					else
					{
						OutputDebug( "SkillIndex : %d, SkillLevel %d 다른 플레이어가 사용한 스킬 패킷, 스킬 생성 실패!\n", nSkillTableID, cLevel );
					}
				}
			}
			break;
		case eActor::SC_RECOVERYSP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nValue;
				Stream.Read( &nValue, sizeof(int) );

				SetSP( nValue );
			}
			break;
		case eActor::CS_ONDROP:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );

				SetMagnetPosition( vPos );
				SetJumpMovement( EtVector2( 0.f, 0.f ) );
			}
			break;
		case eActor::CS_CMDTOGGLEWEAPONORDER:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nEquipIndex;
				bool bViewCash;

				Stream.Read( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &bViewCash, sizeof(bool) );

				CmdToggleWeaponViewOrder( nEquipIndex, bViewCash );
			}
			break;
		case eActor::CS_CMDTOGGLEPARTSORDER:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nEquipIndex;
				bool bViewCash;

				Stream.Read( &nEquipIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &bViewCash, sizeof(bool) );

				if( nEquipIndex == HIDEHELMET_BITINDEX ) CmdToggleHideHelmet( bViewCash );
				else CmdTogglePartsViewOrder( nEquipIndex, bViewCash );
			}
			break;

		case eActor::SC_SKILLUSING_FAILED:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int iSkillID = 0;
				int iServerActionID = 0;
				CDnSkill::UsingResult eFailResult = CDnSkill::Failed;

				Stream.Read( &iSkillID, sizeof(int) );
				Stream.Read( &eFailResult, sizeof(CDnSkill::UsingResult) );
				Stream.Read( &iServerActionID, sizeof(int) );

				// 스킬 사용 취소.
				CancelUsingSkill();

				// usable checker 에 걸려 사용 실패된 스킬의 쿨타임을 회복시켜준다.
				switch ( eFailResult )
				{
					case CDnSkill::FailedByCooltime:
						break;

					case CDnSkill::FailedByUsableChecker:
					case CDnSkill::FailedByInvailedAction:
						{

#ifdef PRE_FIX_69469
							SetActionQueue( "Stand" , 0,  0.f, 0.f );
							if( IsLocalActor() )
							{
								DnSkillHandle hExistSkill = FindSkill( iSkillID );
								if( hExistSkill ) hExistSkill->ResetCoolTime();
							}
#else

							ActionElementStruct* pActionElement = GetElement( iServerActionID );
							if( pActionElement )
								SetActionQueue( pActionElement->szName.c_str() );

#ifdef PRE_FIX_DISSOLVE_SKILL_FAILED
							if( IsLocalActor() )
							{
								DnSkillHandle hExistSkill = FindSkill( iSkillID );
								if( hExistSkill )
								{
									for( DWORD i = 0; i < hExistSkill->GetStateEffectCount(); ++i )
									{
										CDnSkill::StateEffectStruct* pSE = hExistSkill->GetStateEffectFromIndex( i );
										if( STATE_BLOW::BLOW_069 == pSE->nID || STATE_BLOW::BLOW_155 == pSE->nID )
										{
											hExistSkill->ResetCoolTime();
											break;
										}
									}
								}
							}
#endif
#endif

						}
						break;
				}
			}
			break;
		case eActor::SC_CMDESCAPE:
			{
				CPacketCompressStream Stream( pPacket, 32 );

				EtVector3 vPos;
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				SetPosition( vPos );
				SetPrevPosition( vPos );
				SetActionQueue( "Stand" );

				CmdWarp( vPos, EtVec3toVec2( *GetMoveVectorZ() ) );
			}
			break;
		case eActor::SC_CANNONPOSSESS_RES:
			{
				// 대포를 점유하겠다는 다른 클라로부터의 요청에 대한 결과.
				// 대포 소유에 성공한 경우에 대포모드로 전환.
				bool bSuccess  = false;
				DWORD dwCannonMonsterID = 0;
				MatrixEx Cross;

				CPacketCompressStream Stream( pPacket, 64 );
				Stream.Read( &bSuccess, sizeof(bool) );

				// 로컬플레이어에서 패킷 보내고 기다리는 플래그를 응답이 왔으므로 초기화 시켜준다.
				ResetWaitCannonPossess();

				if( bSuccess )
				{
					Stream.Read( &dwCannonMonsterID, sizeof(DWORD) );
					Stream.Read( &Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
					Stream.Read( &Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
					Stream.Read( &Cross.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
					Stream.Read( &Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
					ToggleCannonMode( true, dwCannonMonsterID, Cross );
				}
			}
			break;

		case eActor::SC_CANNONRELEASE:
			{
				DWORD dwCannonMonsterID = 0;
				MatrixEx Cross;

				CPacketCompressStream Stream( pPacket, 64 );
				Stream.Read( &dwCannonMonsterID, sizeof(DWORD) );
				Stream.Read( &Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Stream.Read( &Cross.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Stream.Read( &Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

				ToggleCannonMode( false, dwCannonMonsterID, Cross );
			}
			break;

		case eActor::SC_CANNONROTATESYNC:
			{
				_ASSERT( m_bPlayerCannonMode );
				if( false == m_bPlayerCannonMode )
					break;
				
				CPacketCompressStream Stream( pPacket, 64 );
				DWORD dwCannonMonsterID = 0;
				EtVector3 vDirection( 0.0f, 0.0f, 0.0f );

				Stream.Read( &dwCannonMonsterID, sizeof(DWORD) );
				Stream.Read( &vDirection, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

				CDnActor* pActor = CDnActor::FindActorFromUniqueID( dwCannonMonsterID );
				if( pActor )
				{
					CDnCannonMonsterActor* pCannonMonster = dynamic_cast<CDnCannonMonsterActor*>(pActor);
					if( pCannonMonster ) {
						EtVec3Normalize( &vDirection, &vDirection );		// 압축되어 전송된 데이터이므로 정규화 한 번 해줌.
						pCannonMonster->SetCannonLookDirection(&vDirection); // Rotha 추가
					}
				}
			}
			break;

			// 게임 서버에서 대포 몬스터가 발사체를 쏠 때 발사체 패킷을 보내주기 때문에 
			// 타겟팅 관련해서 클라이언트가 따로 할 일은 없다.
		case eActor::CS_CANNONTARGETING:
			break;

		case eActor::SC_COOLTIMEPARRY_SUCCESS:
			{
				CPacketCompressStream Stream( pPacket, 32 );

				int iSkillID = 0;
				Stream.Read( &iSkillID, sizeof(int) );

				// 쿨타임 패링 상태효과를 찾아서 패링이 성공했음을 알린다.
				DNVector(DnBlowHandle) vlhParryBlows;
				m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_153, vlhParryBlows );

				int iNumParryBlow = (int)vlhParryBlows.size();
				for( int i = 0; i < iNumParryBlow; ++i )
				{
					DnBlowHandle hParryBlow = vlhParryBlows.at( i );
					if( !hParryBlow )
						continue;

					// 쿨타임 패링 상태효과가 하나만 있는 게 맞다. 사실 현재 여러개 있으면 문제 있는 거임.
					const CDnSkill::SkillInfo* pSkillInfo = hParryBlow->GetParentSkillInfo();
					if( iSkillID == pSkillInfo->iSkillID )
					{
						static_cast<CDnCooltimeParryBlow*>(hParryBlow.GetPointer())->OnSuccess();
					}
				}
			}
			break;
		case eActor::SC_FISHINGROD_CAST:
			{
#ifdef PRE_ADD_CASHFISHINGITEM
				CPacketCompressStream Stream( pPacket, 32 );

				int nFishingRodItemID = 0;
				Stream.Read( &nFishingRodItemID, sizeof(int) );

				CreateFishingRod( nFishingRodItemID );
#else // PRE_ADD_CASHFISHINGITEM
				CreateFishingRod( "Fishing Rod.skn", "Fishing Rod.ani", "Fishing Rod.act" );
#endif // PRE_ADD_CASHFISHINGITEM
				ShowFishingRod( true );
				SetFishingRodAction( "Fishing Rod_Casting" );
			}
			break;
		case eActor::SC_FISHINGROD_LIFT:
			{
				SetFishingRodAction( "Fishing Rod_Lift." );
			}
			break;
		case eActor::SC_FISHINGROD_HIDE:
			{
				ShowFishingRod( false );
			}
			break;
		case eActor::SC_SYNCPRESSEDPOS:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				SetMagnetPosition( vPos );

				OutputDebug( "SYNCPRESSEDPOS\n" );
			}
			break;

			// 감전 전이 상태효과로 내가 가진 감전 전이 스킬로 특정 액터에게서
			// 특정 액터에게로 감전이 전이 되었다고 서버로부터 패킷이 옴.
		case eActor::SC_SHOCK_INFECTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				DWORD dwTargetActorUniqueID = 0;
				DWORD dwActorToInfectionUniqueID = 0;
				int iServerBlowID = 0;

				Stream.Read( &dwTargetActorUniqueID, sizeof(dwTargetActorUniqueID) );
				Stream.Read( &dwActorToInfectionUniqueID, sizeof(dwActorToInfectionUniqueID) );
				Stream.Read( &iServerBlowID, sizeof(iServerBlowID) );

				CDnShockInfectionBlow* pShockInfectionBlow = NULL;
				DNVector(DnBlowHandle) vlhHandles;

				// 쇼크 트랜지션에서만 쓰이는 상태효과이므로 반드시 하나만 있어야 한다.
				m_pStateBlow->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_168, vlhHandles);
				_ASSERT( 1 == vlhHandles.size() );
				if (!vlhHandles.empty())
					pShockInfectionBlow = dynamic_cast<CDnShockInfectionBlow*>(vlhHandles[0].GetPointer());

				if( pShockInfectionBlow )
				{
					DnActorHandle hTargetActor = CDnActor::FindActorFromUniqueID( dwTargetActorUniqueID );
					DnActorHandle hActorToInfection = CDnActor::FindActorFromUniqueID( dwActorToInfectionUniqueID );
					if( hTargetActor && hActorToInfection )
					{
						pShockInfectionBlow->DoShockInfection( hTargetActor, hActorToInfection );
					}
				}

			}
			break;
		case eActor::SC_CMDSHOOTMODE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				DWORD dwUniqueID = 0;
				bool bEnableShootMode = false;
				Stream.Read( &dwUniqueID, sizeof(dwUniqueID) );
				Stream.Read( &bEnableShootMode, sizeof(bool) );

				CDnActor* pActor = CDnActor::FindActorFromUniqueID( dwUniqueID );
				if( pActor && pActor->IsPlayerActor())
				{
					CDnPlayerActor* pPlayerActor = (CDnPlayerActor*)pActor;
					pPlayerActor->CmdShootMode(bEnableShootMode);
				}
				break;
			}
		case eActor::SC_ADDBUBBLE:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				m_pBubbleSystem->CreateBubbleFromPacketStream( &Stream );
			}	
			break;

		case eActor::SC_REMOVEBUBBLE:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int iBubbleTypeID = 0;
				int iRemoveCount = 0;
				Stream.Read( &iBubbleTypeID, sizeof(int) );
				Stream.Read( &iRemoveCount, sizeof(int) );
				m_pBubbleSystem->RemoveBubbleByTypeID( iBubbleTypeID, iRemoveCount );
			}
			break;

		case eActor::SC_REFRESH_BUBBLE_DURATIONTIME:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				m_pBubbleSystem->RefreshBubbleDurationTimeFromPacketStream( &Stream );
			}
			break;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		case eActor::SC_TOTAL_LEVEL:
			{
				int nTotalLevel = 0;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nTotalLevel, sizeof(int) );

				UpdateTotalLevel(nTotalLevel);
			}
			break;
		case eActor::SC_ADD_TOTAL_LEVEL_SKILL:
			{
				int nSlotIndex = -1;
				int nSkillID = 0;
				bool isInitialize = false;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nSlotIndex, sizeof(int) );
				Stream.Read( &nSkillID, sizeof(int) );
				Stream.Read( &isInitialize, sizeof(bool) );

				AddTotalLevelSkill(nSlotIndex, nSkillID, isInitialize);

			}
			break;
		case eActor::SC_REMOVE_TOTAL_LEVEL_SKILL:
			{
				int nSlotIndex = -1;
				
				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nSlotIndex, sizeof(int) );
				
				RemoveTotalLevelSkill(nSlotIndex);
			}
			break;
		case eActor::SC_TOTAL_LEVEL_SKILL_ACTIVE_LIST:
			{
				int nCount = 0;
				int nSlotIndex = -1;
				int nSkillID = 0;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nCount, sizeof(int) );

				for (int i = 0; i < nCount; ++i)
				{
					Stream.Read( &nSlotIndex, sizeof(int) );
					Stream.Read( &nSkillID, sizeof(int) );
					AddTotalLevelSkill(nSlotIndex, nSkillID, true);
				}
			}
			break;
		case eActor::SC_TOTAL_LEVEL_SKILL_CASHSLOT_ACTIVATE:
			{
				int nSlotIndex = -1;
				bool bActivate = false;
				__time64_t tExpireDate = 0;				

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read(&nSlotIndex, sizeof(int));
				Stream.Read(&bActivate, sizeof(bool));
				Stream.Read(&tExpireDate, sizeof(__time64_t));				

				ActivateTotalLevelSkillCashSlot(nSlotIndex, bActivate, tExpireDate);
			}
			break;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
	}
	CDnActor::OnDispatchMessage( dwActorProtocol, pPacket );
}

void CDnPlayerActor::SyncClassTime( LOCAL_TIME LocalTime )
{
	MAActorRenderBase::m_LocalTime = LocalTime;
	CDnActor::SyncClassTime( LocalTime );
}

void CDnPlayerActor::ProcessDie( LOCAL_TIME LocalTime, float fDelta )
{
	if( !IsDie() ) {
		if( m_bGhost ) ToggleGhostMode( false );
		if( m_fDieDelta >= m_fMaxDieDelta - 4.f )
		{
			if( strcmp( GetCurrentAction(), "Die" ) == 0 )
			{
				SetAction( "Stand", 0.f, 0.f );
			}
			m_fDieDelta = 0;
		}

		return;
	}

	if( m_fDieDelta > 0.f ) {
		m_fDieDelta -= fDelta;
		if( m_fDieDelta < 0.f ) m_fDieDelta = 0.f;
	}
	if( !m_bGhost ) {
		if( m_fDieDelta < m_fMaxDieDelta - 4.f ) {
			ToggleGhostMode( true );
		}
	}
}

void CDnPlayerActor::ToggleGhostMode( bool bGhost, bool bShowEffect )
{
#ifdef _SHADOW_TEST
	if( m_bIsShadowActor ) return;
#endif
	if( m_bObserver ) return;
	if( m_bGhost == bGhost ) return;
	m_bGhost = bGhost;

	if( bGhost == true && GetSP() > 0 )
		SetSP(0);

	if( bShowEffect ) {
		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle ) {
			hHandle->SetPosition( m_matexWorld.m_vPosition );
			hHandle->SetActionQueue( bGhost ?  "SwapGhost" : "Rebirth" );
			EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );
			
			pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
			pResult->vRotate = EtVector3( 0, 0, 0);
			pResult->bLinkObject = false;
			pResult->bDefendenceParent = false;
		}
	}

	bool bGhostMode = true;
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP ) 
	{
		bGhostMode = false;
		CDnPvPGameTask *pPvpGameTask = (CDnPvPGameTask*)pGameTask;
		if( pPvpGameTask && pPvpGameTask->GetGameMode() == PvPCommon::GameMode::PvP_AllKill )
			bGhostMode= true;
#if defined(PRE_ADD_PVP_TOURNAMENT)		
		else if( pPvpGameTask && pPvpGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Tournament )
			bGhostMode= true;
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
	}

	if( IsLocalActor() && bGhost ) 
	{
		GetInterface().BackupMyPortrait();
	}

	if( !bGhostMode ) {
		if( bGhost ) {
			Show( false );
			if( IsMovable() || IsStay() ) {
				SetAction( "Die", 0.f, 0.f, false );
			}
		}
		else {
			Show( true );
			std::string szActionName = "Stand";
			if(m_bShootMode)
				szActionName = "MOD_Stand";
			
			if(IsTransformMode())
			{
				if(IsExistAction( "Summon_On" ))
					szActionName = "Summon_On";
				ShowParts(false);
			}
			SetAction( szActionName.c_str(), 0.f, 0.f );

			if( IsLocalActor() ) {
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) return;
				CDnPlayerCamera *pPlayerCamera = dynamic_cast<CDnPlayerCamera*>(hCamera.GetPointer());
				if( !pPlayerCamera ) return;
				if( pPlayerCamera->GetAttachActor() != CDnActor::s_hLocalActor ) {
					pPlayerCamera->AttachActor( CDnActor::s_hLocalActor );
					pPlayerCamera->ResetCamera();
				}
			}
		}
	}
	else {
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );

		if( bGhost ) {
			SwapSingleSkin( 499 + m_nClassID );
			ResetCustomAction();
			ResetMixedAnimation( false );
			SetAction( "Stand", 0.f, 0.f );
		}
		else {
			SwapSingleSkin( -1 );
			SetAction( "Stand", 0.f, 0.f );
			if( IsDeveloperAccountLevel() == false )
				SetAlphaBlend( 1.0 );
			CmdToggleBattle( true );
		}
	}

	if( pGameTask ) pGameTask->OnGhost( GetActorHandle() );
	
	// 죽어서 월드존으로 돌아간 경우 죽어있는 상태에서 캐릭터 Initialize 하는 경우가있고 죽은상태에서는 말을 못타게된다.
	// 그래서 부활을 해준이후에 탈것아이템이 존재하지만 탈것을 못탄경우에는 다시 태워준다.
	if( IsCanVehicleMode() && !IsVehicleMode() && GetVehicleInfo().Vehicle[Vehicle::Slot::Body].nItemID > 0)
		RideVehicle( GetVehicleInfo() , true );
}

void CDnPlayerActor::SwapSingleSkin(int nChangeActorTableID, bool bSaveMergedObject)
{
	if (m_nSwapSingleSkinActorID == nChangeActorTableID) return;
	DebugLog("CDnPlayerActor::SwapSingleSkin start");

	m_nSwapSingleSkinActorID = nChangeActorTableID;

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TACTOR);
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable(CDnTableDB::TFILE);
#endif
	if (m_nSwapSingleSkinActorID == -1)
	{
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szSkinName, szAniName, szActName;
		CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, "_SkinName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, "_AniName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nClassID, "_ActName", pFileNameSox);
#else
		std::string szSkinName = pSox->GetFieldFromLablePtr(m_nClassID, "_SkinName")->GetString();
		std::string szAniName = pSox->GetFieldFromLablePtr(m_nClassID, "_AniName")->GetString();
		std::string szActName = pSox->GetFieldFromLablePtr(m_nClassID, "_ActName")->GetString();
#endif

		ReleaseSignalImp();
		SAFE_RELEASE_SPTR(GetObjectHandle());
		FreeAction();

		RecomposeAction();

		LoadSkin(CEtResourceMng::GetInstance().GetFullName(szSkinName).c_str(), CEtResourceMng::GetInstance().GetFullName(szAniName).c_str());
		LoadAction(CEtResourceMng::GetInstance().GetFullName(szActName).c_str());
		SetAction("Stand", 0.f, 0.f);

		// 구울모드에서 컴바인 한번이라도 덜 호출하기 위해 아래처럼 검사해서 처리한다. 칼리는 왼손무기때문에 무조건 TDnPlayerKali쪽으로 타도록 한다.
		bool bCallNormalFunction = true;
		if (bSaveMergedObject && m_hSaveMergedObject)
		{
			SetCombineObject(m_hSaveMergedObject);
			bCallNormalFunction = false;
#ifdef PRE_ADD_KALI
			if (m_nClassID - 1 == CDnActorState::Kali) bCallNormalFunction = true;
#endif
#ifdef PRE_ADD_ASSASSIN
			if (m_nClassID - 1 == CDnActorState::Assassin) bCallNormalFunction = true;
#endif
#ifdef PRE_ADD_LENCEA
			if (m_nClassID - 1 == CDnActorState::Lencea) bCallNormalFunction = true;
#endif
#ifdef PRE_ADD_MACHINA
			if (m_nClassID - 1 == CDnActorState::Machina) bCallNormalFunction = true;
#endif
		}
		else
		{
			CombineParts();
		}

		ShowParts(true);

		for (int i = 0; i<CDnWeapon::EquipSlot_Amount; i++) {
			DnWeaponHandle hWeapon = GetWeapon(i);
			if (hWeapon)
			{
				LinkWeapon(i);
				if (bCallNormalFunction) ShowWeapon(i, true);
				else CDnActor::ShowWeapon(i, true);
			}
			hWeapon = GetCashWeapon(i);
			if (hWeapon)
			{
				LinkCashWeapon(i);
				if (bCallNormalFunction) ShowCashWeapon(i, true);
				else CDnPlayerActor::ShowCashWeapon(i, true);
			}

			RefreshWeaponViewOrder(i);
		}

#ifdef PRE_ADD_CASH_COSTUME_AURA
		LoadCostumeAuraEffect(true);
#endif
		SAFE_RELEASE_SPTR(m_hSwapOriginalHandle);
		SAFE_DELETE(m_pSwapOriginalAction);
	}
	else
	{

		if (bSaveMergedObject)
			m_hSaveMergedObject = GetCombineObject();

#ifdef PRE_FIX_MEMOPT_EXT
		std::string szSkinName, szAniName, szActName;
		CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nSwapSingleSkinActorID, "_SkinName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nSwapSingleSkinActorID, "_AniName", pFileNameSox);
		CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nSwapSingleSkinActorID, "_ActName", pFileNameSox);
#else
		std::string szSkinName = pSox->GetFieldFromLablePtr(m_nSwapSingleSkinActorID, "_SkinName")->GetString();
		std::string szAniName = pSox->GetFieldFromLablePtr(m_nSwapSingleSkinActorID, "_AniName")->GetString();
		std::string szActName = pSox->GetFieldFromLablePtr(m_nSwapSingleSkinActorID, "_ActName")->GetString();
#endif

		if (!m_hSwapOriginalHandle && m_hObject) {
			m_hSwapOriginalHandle = EternityEngine::CreateAniObject(CEtResourceMng::GetInstance().GetFullName(m_hObject->GetSkinFileName()).c_str(), CEtResourceMng::GetInstance().GetFullName(m_hObject->GetAniHandle()->GetFileName()).c_str());
		}
		SAFE_RELEASE_SPTR(GetObjectHandle());
		if (!m_pSwapOriginalAction) {
			m_pSwapOriginalAction = new CDnActionBase;
			m_pSwapOriginalAction->LoadAction(CEtResourceMng::GetInstance().GetFullName(CDnActionBase::m_szFileName).c_str());
		}

		SAFE_RELEASE_SPTR(GetObjectHandle());
		FreeAction();

		char szTempActionName[128] = { 0, };
		_GetFullFileName(szTempActionName, _countof(szTempActionName), szActName.c_str());
		_strlwr(szTempActionName);
		AddUsingElement(szTempActionName);
		RefreshUsingElement();

		LoadSkin(CEtResourceMng::GetInstance().GetFullName(szSkinName).c_str(), CEtResourceMng::GetInstance().GetFullName(szAniName).c_str());
		LoadAction(CEtResourceMng::GetInstance().GetFullName(szActName).c_str());

		ShowParts(false);
		for (int i = 0; i<CDnWeapon::EquipSlot_Amount; i++)
		{
			DnWeaponHandle hWeapon = GetWeapon(i);
			if (hWeapon)
				LinkWeapon(i);

			ShowWeapon(i, false);
			ShowCashWeapon(i, false);
		}

		SetAction("Stand", 0.f, 0.f);

#ifdef PRE_ADD_CASH_COSTUME_AURA
		LoadCostumeAuraEffect(false);
#endif

	}

	if (m_hObject) {
		m_hObject->SetCollisionGroup(COLLISION_GROUP_DYNAMIC(1));
		m_hObject->SetTargetCollisionGroup(COLLISION_GROUP_STATIC(1) | COLLISION_GROUP_DYNAMIC(2) | COLLISION_GROUP_DYNAMIC(3));
	}

	if (IsVehicleMode() && GetMyVehicleActor() && !IsGhost() && !IsDie() && !IsDestroy())
	{
		GetMyVehicleActor()->LinkPlayerToVehicle();
		GetMyVehicleActor()->SetActionQueue("Stand");
	}

	if (IsLocalActor())
	{
		CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>(this);
		pLocalPlayer->RemoveAllBubbles(false);

#ifdef PRE_FIX_PROTRAIT_FOR_GHOST
		GetInterface().UpdateMyPortrait();
#endif
	}

	DebugLog("CDnPlayerActor::SwapSingleSkin end");
}


void CDnPlayerActor::ResetActor()
{
	CDnActor::ResetActor();

	if(IsVehicleMode() && GetMyVehicleActor())
		GetMyVehicleActor()->SetActionQueue("Stand");

	m_cMovePushKeyFlag = 0;
} 

void CDnPlayerActor::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	Sphere.Center = m_matexWorld.m_vPosition;
	Sphere.Center.y += 50.f;
	Sphere.fRadius = 50.f;
}

std::string CDnPlayerActor::CheckBattleModeAction( const char *szActionName )
{
	std::string szAction;

	if( IsGhost() && !IsObserver() ) return std::string(szActionName);
	if( IsSwapSingleSkin() ) return std::string(szActionName);

	if( !IsBattleMode() && !IsVehicleMode() ) 
	{
		if( strstr( szActionName, "Normal_" ) == NULL ) {
			if( strcmp( szActionName, "Stand" ) == NULL || strstr( szActionName, "Move_") || strstr( szActionName, "Jump_" ) )
			{
				szAction = "Normal_";
			}
		}
		szAction += szActionName;
	}
	else 
		szAction = szActionName;

	return szAction.c_str();
}


void CDnPlayerActor::SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop )
{
	if(IsVehicleMode() && GetMyVehicleActor() && strstr(szActionName,"Vehicle_") == NULL)
		return;

	if( !IsSwapSingleSkin() && m_bBattleMode && m_bShootMode && strcmp(szActionName,"Stand") == NULL) // 특정 액션의 nextAction이 지정된경우 액션큐를 통하지않고 바로 설정되는 경우가있다.
		szActionName = GetChangeShootActionName(szActionName);

	CDnActor::SetAction( CheckBattleModeAction( CheckChangeActionBlow( szActionName ) ).c_str(), fFrame, fBlendFrame, bLoop );
}

bool CDnPlayerActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bCheck, bool bCheckStateEffect )
{
	if( IsDie() && strstr( GetCurrentAction(), "Die" ) )
		return false;

	if(IsVehicleMode() && GetMyVehicleActor() && !GetMyVehicleActor()->IsDestroy())
	{
		if(strstr(szActionName,"Vehicle_") == NULL)
			return false;
	}
	else if(!IsVehicleMode() && strstr(szActionName,"Vehicle_Call") == NULL && strstr(szActionName,"Vehicle_") != NULL )
		return false;

	if(m_bShootMode && !m_bTransformMode && m_bBattleMode)
		szActionName = GetChangeShootActionName(szActionName);

	return CDnActor::SetActionQueue( CheckBattleModeAction( CheckChangeActionBlow( szActionName ) ).c_str(), nLoopCount, fBlendFrame, fStartFrame, bCheck, bCheckStateEffect );
}

void CDnPlayerActor::OnChangeAction( const char *szPrevAction )
{
	// 129번 액션 이름 대체 상태효과 활성 비활성 처리.
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_129 ) )
	{
		DNVector(DnBlowHandle) vlhChangeActionSetBlow;
		m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_129, vlhChangeActionSetBlow );
		
		// 액션 셋 변경 상태효과는 여러개 있을 수 있다.
		int iNumBlow = (int)vlhChangeActionSetBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			CDnChangeActionSetBlow* pChangeActionSetBlow = static_cast<CDnChangeActionSetBlow*>( vlhChangeActionSetBlow.at( i ).GetPointer() );
			pChangeActionSetBlow->UpdateEnable( szPrevAction, GetCurrentAction() );
		}
	}

	CDnActor::OnChangeAction( szPrevAction );

	// 129번 상태효과에서 사용하게 되었으므로 액션이 한번 바뀌면 리셋해준다.
	// 다음에 CmdAction 이 호출되어 무기 액션으로 바뀌게 되면 플래그 다시 켜짐.
	m_bChangedEquipAction = false;
}

char *CDnPlayerActor::GetLinkWeaponBoneName( int nEquipIndex, bool bBattleMode )
{
	static char szBoneName[32] = {0,};
	if( bBattleMode ) {
		sprintf_s( szBoneName, "~BoxBone%02d", nEquipIndex + 1 );
	}
	else {
		sprintf_s( szBoneName, "~BoxBone%02d_1", nEquipIndex + 1 );
	}
	return szBoneName;
}

#ifdef PRE_ADD_37745
void CDnPlayerActor::SetBattleMode( bool bEnable , bool bForce /* = false */  )
#else
void CDnPlayerActor::SetBattleMode( bool bEnable )
#endif
{
	m_bBattleMode = bEnable;
	
#ifdef PRE_ADD_37745
	if(!m_bRTTRenderMode && 
		CGameOption::IsActive() &&
		CGameOption::GetInstance().m_bHideWeaponByBattleMode && !m_bRTTRenderMode && !bForce && IsSwapSingleSkin() == false )
	{
		ShowWeapon( 0, bEnable );
		ShowCashWeapon( 0, bEnable );
	}
#endif

	if(m_bShootMode && GetActorType() == CDnActor::Warrior)
	{
		if(m_bBattleMode)
		{
			if( m_hWeapon[0] ) 
				m_hWeapon[0]->ChangeLinkBone("Bip01 L Hand");
		}
		else
		{
			if( m_hWeapon[0] ) 
				m_hWeapon[0]->ChangeLinkBone(GetLinkWeaponBoneName( 0 , m_bBattleMode ));
		}
		return;
	}
	switch( GetActorType() ) {
		case CDnActor::Warrior:
		case CDnActor::Cleric:
		case CDnActor::Soceress:
#ifdef PRE_ADD_ACADEMIC
		case CDnActor::Academic: // #35786 관련 아카데믹 추가 < 나중에 구현방식이 기존 캐릭터와 틀리면 아쳐처럼 예외처리 해줍시다. >
#endif
#if defined(PRE_ADD_ASSASSIN)
		case CDnActor::Assassin:
		case CDnActor::Lencea:
#endif	// #if defined(PRE_ADD_ASSASSIN)
			for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
				const char *szBoneName = GetLinkWeaponBoneName( i , m_bBattleMode );
				if( m_hWeapon[i] ) m_hWeapon[i]->ChangeLinkBone( szBoneName );
				if( m_hCashWeapon[i] ) m_hCashWeapon[i]->ChangeLinkBone( szBoneName );
			}			
			break;

		//relk
		case CDnActor::Machina:
			for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
				const char *szBoneName = GetLinkWeaponBoneName( i , m_bBattleMode );
				if( m_hWeapon[0] ) m_hWeapon[0]->ChangeLinkBone("~BoxBone02_1");
				if( m_hWeapon[1] ) m_hWeapon[1]->ChangeLinkBone("~BoxBone01_1");
				if( m_hCashWeapon[0] ) m_hCashWeapon[0]->ChangeLinkBone("~BoxBone02_1");
				if( m_hCashWeapon[1] ) m_hCashWeapon[1]->ChangeLinkBone("~BoxBone01_1");
			}			
			break;

		case CDnActor::Archer:
#if defined(PRE_ADD_KALI)
		case CDnActor::Kali:
			// 칼리의 경우 왼손무기 복제처리때문에 워려나 아카데믹과 달리 ~BoxBone02_1 가지고 있다.
			// 그래서 ChangeLinkBone 처리를 할 경우 플레이어 ani파일을 쓰면서 동시에 특정 본에 링크되어버리는 것.
			// 그래서 주무기만 ChangeLinkBone 처리를 해야한다.
#endif // PRE_ADD_KALI
			for( int i=0; i<1; i++ ) {
				const char *szBoneName = GetLinkWeaponBoneName( i , m_bBattleMode );
				if( m_hWeapon[i] ) m_hWeapon[i]->ChangeLinkBone( szBoneName );
				if( m_hCashWeapon[i] ) m_hCashWeapon[i]->ChangeLinkBone( szBoneName );
			}
			break;
	}
}

bool CDnPlayerActor::IsCanBattleMode()
{
	if(IsObserver()) return false;
	if(IsSwapSingleSkin()) return false;
	if(IsVehicleMode()) return false;
	if( IsSpectatorMode() ) return false;
	return true;
}

bool CDnPlayerActor::IsNudeParts( DnPartsHandle hParts ) const
{
	for( int i=0; i<CDnParts::DefaultPartsTypeEnum_Amount; i++ ) {
		if( m_hNudeParts[i] == hParts ) return true;
	}
	return false;
}

bool CDnPlayerActor::IsDefaultParts(ITEMCLSID classId) const
{
	for( int i=0; i<CDnParts::DefaultPartsTypeEnum_Amount; i++ )
	{
		if( m_nDefaultPartsIndex[i] == classId )
			return true;
	}
	return false;
}

void CDnPlayerActor::AttachNudeParts( CDnParts::PartsTypeEnum Index )
{
	if( Index < CDnParts::Body || Index > CDnParts::Foot ) return;
	if( !m_hPartsObject[Index] ) {
		MAPartsBody::AttachParts( m_hNudeParts[Index - CDnParts::Body], Index );
	}
}

void CDnPlayerActor::DetachNudeParts( CDnParts::PartsTypeEnum Index )
{
	if( Index < CDnParts::Body || Index > CDnParts::Foot ) return;
	if( m_hPartsObject[Index] == m_hNudeParts[Index - CDnParts::Body] ) {
		MAPartsBody::DetachParts( Index );
	}
}

bool CDnPlayerActor::AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete, bool bUseCombine )
{
	if( !hParts ) return false;
	DetachNudeParts( hParts->GetPartsType() );
	bool bResult = MAPartsBody::AttachParts( hParts, Index, bDelete, bUseCombine );

	if( bResult == false ) AttachNudeParts( hParts->GetPartsType() );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hParts->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfo(Index, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

	return bResult;
}

bool CDnPlayerActor::DetachParts( CDnParts::PartsTypeEnum Index )
{
	bool bResult = MAPartsBody::DetachParts( Index );
	if( bResult ) {
		AttachNudeParts( Index );
	}

	RemoveSkillLevelUpInfo(Index);

	return bResult;
}

void CDnPlayerActor::OnLevelUp( int nLevel, int nLevelUpAmount )
{
	if( !IsDie() ) {
		SetHP( GetMaxHP() );
		SetSP( GetMaxSP() );
	}
	if( GetAccountLevel() < AccountLevel_New || ( GetAccountLevel() >= AccountLevel_New && IsLocalActor() ) ) {
		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle ) {
			hHandle->SetPosition( m_matexWorld.m_vPosition );
			hHandle->SetActionQueue( "LevelUp" );
			EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );

			pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
			pResult->vRotate = EtVector3( 0, 0, 0);
			pResult->bLinkObject = true;
			pResult->bDefendenceParent = true;
		}
	}
	// 레벨에 따라 스킬 소모 SP 가 다르기땜시 언제나 리플레쉬 해줘야한다.
	for( DWORD i=0; i<GetSkillCount(); i++ ) {
		DnSkillHandle hSkill = GetSkillFromIndex(i);
		if( hSkill ) hSkill->RefreshDecreaseMP();
	}

	// 파티 정보도 갱신해줘야 할것들이 있으므로(EXP따위) Refresh해준다.
	CDnPartyTask::GetInstance().UpdatePartyMemberInfo();
}


void CDnPlayerActor::OnRefreshState()
{
	if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) {
		SetHP( GetMaxHP() );
		SetSP( GetMaxSP() );
	}
}

void CDnPlayerActor::OnComboCount( int nValue, int nDelay )
{
	m_nComboCount = nValue;
	m_nComboDelay = nDelay;
}

int CDnPlayerActor::GetMoveSpeed()
{
	int nMoveSpeed = CDnActor::GetMoveSpeed();

	if( CDnWorld::IsActive() && ( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage || CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap ) )
		nMoveSpeed += GetSafeZoneMoveSpeed();

	if(IsTransformMode())
	{
		nMoveSpeed = m_nTransformMoveSpeed;
		if( m_cMovePushKeyFlag & 0x08 ) nMoveSpeed /= 2;
		return nMoveSpeed;
	}

	if( m_cMovePushKeyFlag & 0x08 ) nMoveSpeed /= 2;

	// #62481 월드존 이동속도를 마을과 동일하게 변경.
	if( !IsGhost() && !IsBattleMode() ) nMoveSpeed = (int)( nMoveSpeed * 1.4f );
	if( IsDie() && IsGhost() ) nMoveSpeed = (int)( nMoveSpeed * 1.5f );
	return nMoveSpeed;
}

bool CDnPlayerActor::UseItemFromItemID( int nItemID, char cType )
{
	CDnItem *pItem = NULL;
	switch( cType )
	{
	case ITEMPOSITION_INVEN:		pItem = CDnItemTask::GetInstance().FindItem( nItemID, ST_INVENTORY );		break;
	case ITEMPOSITION_CASHINVEN:	pItem = CDnItemTask::GetInstance().FindItem( nItemID, ST_INVENTORY_CASH );	break;
	}
	if( pItem == NULL ) return false;
	if( MIInventoryItem::Unusable == 
		pItem->GetItemCondition() ||
		MIInventoryItem::NotEnoughCondition == pItem->GetItemCondition() ) return false;

	return UseItemFromSlotIndex( pItem->GetSlotIndex(), cType );
}

bool CDnPlayerActor::IsUsableItem( CDnItem* pItem )
{
	// Disable된 아이템 사용 못하게 하기. 우선 임시처리다.
	if( pItem == NULL || GetItemTask().IsDisableItemInCharInven( pItem )  )
		return false;

	if (CTaskManager::GetInstance().GetTask( "VillageTask" ))
	{
		eItemTypeEnum type = pItem->GetItemType();
		switch( type ) {
			case ITEMTYPE_FATIGUEUP: return true;
			case ITEMTYPE_NORMAL: return true;
			case ITEMTYPE_COOKING: return true;
			case ITEMTYPE_HAIRDYE: return true;
			case ITEMTYPE_SKINDYE: return true;
			case ITEMTYPE_EYEDYE: return true;
			case ITEMTYPE_FACIAL: return true;
			case ITEMTYPE_HAIRDRESS: return true;
			case ITEMTYPE_RANDOM: return true;
			case ITEMTYPE_SKILLBOOK: return true;
			case ITEMTYPE_WORLDMSG: return true;
			case ITEMTYPE_RESET_SKILL: 
			case ITEMTYPE_INFINITY_RESET_SKILL:
				if( IsProcessSkill() ) return false;
				return true;
			case ITEMTYPE_PETALTOKEN: return true;
			case ITEMTYPE_REMOTE_ENCHANT: 
				return true;
			case ITEMTYPE_REPAIR_EQUIPITEM:
				return true;
			case ITEMTYPE_REMOTE_ITEMCOMPOUND:
				return true;
			case ITEMTYPE_REMOTE_WAREHOUSE:
				return true;
			case ITEMTYPE_COSTUMEMIX: return true;
			case ITEMTYPE_COSTUMEDESIGN_MIX: return true;
#ifdef PRE_ADD_COSRANDMIX
			case ITEMTYPE_COSTUMERANDOM_MIX: return true;
#endif
			case ITEMTYPE_VEHICLEPARTS:
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID()))
						return false;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();
					if(equipType == Vehicle::Parts::Hair && IsVehicleMode())
						return true;
				}
			case ITEMTYPE_VEHICLEHAIRCOLOR:
				{
					if(IsVehicleMode())
						return true;
					else
						return false;
				}
			case ITEMTYPE_PETPARTS:
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem( pItem->GetClassID() ) )
						return false;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();
					if( IsSummonPet() && ( equipType == Pet::Parts::PetAccessory1 || equipType == Pet::Parts::PetAccessory1 ) )
						return true;
				}
			case ITEMTYPE_PETCOLOR_BODY:
			case ITEMTYPE_PETCOLOR_TATOO:
				{
					if( IsSummonPet() )
						return true;
					else
						return false;
				}
			case ITEMTYPE_APPELLATION: return true;
			case ITEMTYPE_RETURN_HOME:	return true;
			case ITEMTYPE_PERIOD_APPELLATION: return true;
			case ITEMTYPE_INCREASE_LIFE: return true;
			case ITEMTYPE_REBIRTH_COIN: return true;
			case ITEMTYPE_ULTIMATEFATIGUEUP: return true;
			case ITEMTYPE_REBIRTH_COIN_EX: return true;
			case ITEMTYPE_INVENTORY_SLOT_EX: return true;
			case ITEMTYPE_WAREHOUSE_SLOT_EX: return true;
#if defined(PRE_PERIOD_INVENTORY)
			case ITEMTYPE_PERIOD_INVENTORY: return true;
			case ITEMTYPE_PERIOD_WAREHOUSE: return true;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
			case ITEMTYPE_PERIOD_PLATE_EX: return true;
			case ITEMTYPE_EXPAND_SKILLPAGE: return true;
			case ITEMTYPE_PET_EXP : 
				{
					if( IsSummonPet() )
						return true;
					else
						return false;
				}
			case ITEMTYPE_SOURCE:
				return true;
			case ITEMTYPE_GLOBAL_PARTY_BUFF:
				return true;
			case ITEMTYPE_TRANSFORMPOTION:
			case ITEMTYPE_HEAD_SCALE_POTION:
				return true;
			case ITEMTYPE_PET_FOOD:
				return true;
#if defined(PRE_ADD_EXPUP_ITEM)
			case ITEMTYPE_EXPUP_ITEM: return true;
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
			case ITEMTYPE_TOTALSKILLLEVEL_SLOT: return true;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
			case ITEMTYPE_ADD_QUEST: return true;
#endif // PRE_ADD_REMOTE_QUEST
#ifdef PRE_ADD_PVP_EXPUP_ITEM
			case ITEMTYPE_PVPEXPUP: return true;
#endif
#ifdef PRE_ADD_NEW_MONEY_SEED
			case ITEMTYPE_NEWMONEY_SEED: return true;
#endif // PRE_ADD_NEW_MONEY_SEED
		}
		return false;
	}

	return true;
}

bool CDnPlayerActor::UseItemFromSlotIndex( int nSlotIndex, char cType )
{
	if (CDnPartyTask::IsActive())
	{
		if (CDnPartyTask::GetInstance().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
			return false;
	}

	CDnItem *pItem = NULL;
	switch( cType )
	{
	case ITEMPOSITION_INVEN:		pItem = GetItemTask().GetCharInventory().GetItem( nSlotIndex );	break;
	case ITEMPOSITION_CASHINVEN:	pItem = GetItemTask().GetCashInventory().GetItem( nSlotIndex );	break;
	}
	if( pItem == NULL || IsUsableItem(pItem) == false)
		return false;

	// 직업 제한 체크 
	std::vector<int> nVecJobList;
	this->GetJobHistory( nVecJobList );
	if( !pItem->IsPermitPlayer( nVecJobList ) )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5069 ) );
		return false;
	}

	// 레벨 제한 체크. 모든 사용하는 아이템에 레벨 제한을 체크함. (현승씨 요청)
	if( GetLevel() < pItem->GetLevelLimit() )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ) );
		return false;
	}

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	if(pItem->IsUseLimited())
	{
		return false;
	}
#endif

#ifdef PRE_ADD_NAMEDITEM_SYSTEM
	bool bExpireDateCheck = true;
	if( pItem->GetExpireDate() )
	{
		const time_t tNowTime = CSyncTimer::GetInstance().GetCurTime();
		if( tNowTime > *(pItem->GetExpireDate()) )
			bExpireDateCheck = false;
	}

	if( pItem->GetItemType() == eItemTypeEnum::ITEMTYPE_GLOBAL_PARTY_BUFF && (bExpireDateCheck == false) )
	{
		CDnInterface::GetInstance().ShowCaptionDialog(CDnInterface::typeCaption1, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7803 ), textcolor::YELLOW);
		return false;
	}
#endif

#ifdef PRE_ADD_TRANSFORM_POTION
	if( pItem->GetItemType() == ITEMTYPE_TRANSFORMPOTION )
	{
		if( IsLocalActor() && IsVehicleMode() )
		{
			CDnCharStatusDlg * pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			CDnCharVehicleDlg * pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();

			if( pCharVehicleDlg )
				pCharVehicleDlg->ForceUnRideVehicle();
		}
	}
#endif

	// 물약등 현제 상태가 사용 불가(전투상태등) 일 경우에 여기서 그냥 리턴합니다.
	if( pItem->GetItemCondition() != MIInventoryItem::Usable ) return false;

	bool bRet = false;
	switch( cType )
	{
	case ITEMPOSITION_INVEN:		bRet = GetItemTask().GetCharInventory().RequestUseItem( nSlotIndex, cType );	break;
	case ITEMPOSITION_CASHINVEN:	bRet = GetItemTask().GetCashInventory().RequestUseItem( nSlotIndex, cType );	break;
	}
	return bRet;
}

void CDnPlayerActor::RenderHeadIcon( float fElapsedTime )
{
	// 헤드아이콘 찍기(라디오메세지)
	SAABox box;
	GetBoundingBox(box);
	float fHeight = box.Max.y - box.Min.y + 5.f;
	EtVector3 vPos = m_matexWorld.m_vPosition;
	vPos.y += (fHeight * 1.3f);
	m_HeadIcon.RenderHeadIcon( vPos, fElapsedTime );
}

#ifdef PRE_FIX_MAIL_HEADICON
void CDnPlayerActor::RenderMailBoxMark(int nRenderLayer)
{
	#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
	if (m_isMailBoxOpenByShortCutKey == true &&
		m_hMailBoxOpenMarkTex)
	{
		int nAlpha = (int)(GetAlphaLayer( AL_APPEAR ) * 255);
		nAlpha = EtClamp(nAlpha, 0, 255);

		int STATE_MARK_SCALE_TIME  = 250;
		float STATE_MARK_SCALE_RANGE  = 0.1f;

		EtVector2 vSize;
		vSize.x = m_hMailBoxOpenMarkTex->Width() * 0.5f;
		vSize.y = m_hMailBoxOpenMarkTex->Height() * 0.5f;

		int nTemp = (DWORD)CDnActionBase::m_LocalTime%(STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME);
		if( nTemp < STATE_MARK_SCALE_TIME ) {
			vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * nTemp;
		}
		else {
			vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * ( (STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME) - nTemp );
		}

		AddHeadNameElement( nRenderLayer, m_hMailBoxOpenMarkTex, vSize, D3DCOLOR_ARGB(nAlpha,255,255,255), 17.f );
	}
	#endif // PRE_ADD_MAILBOX_OPEN
}
#endif

void CDnPlayerActor::RenderCustom( float fElapsedTime )
{
	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( m_bShowHeadName && IsShow() && IsProcess() )
	{
		if ( s_bHideAnotherPlayer && s_hLocalActor.GetPointer() != this )
			return;

		CalcCustomRenderDepth();
		RenderHeadIcon( fElapsedTime );

#ifdef PRE_FIX_MAIL_HEADICON
#else
	#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
		if (m_isMailBoxOpenByShortCutKey == true &&
			m_hMailBoxOpenMarkTex)
		{
			int nAlpha = (int)(GetAlphaLayer( AL_APPEAR ) * 255);
			nAlpha = EtClamp(nAlpha, 0, 255);

			int STATE_MARK_SCALE_TIME  = 250;
			float STATE_MARK_SCALE_RANGE  = 0.1f;

			EtVector2 vSize;
			vSize.x = m_hMailBoxOpenMarkTex->Width() * 0.5f;
			vSize.y = m_hMailBoxOpenMarkTex->Height() * 0.5f;

			int nTemp = (DWORD)CDnActionBase::m_LocalTime%(STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME);
			if( nTemp < STATE_MARK_SCALE_TIME ) {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * nTemp;
			}
			else {
				vSize *= 1.f + ( STATE_MARK_SCALE_RANGE / (float)STATE_MARK_SCALE_TIME ) * ( (STATE_MARK_SCALE_TIME+STATE_MARK_SCALE_TIME) - nTemp );
			}

			AddHeadNameElement( 1, m_hMailBoxOpenMarkTex, vSize, D3DCOLOR_ARGB(nAlpha,255,255,255), 17.f );
		}
	#endif // PRE_ADD_MAILBOX_OPEN
#endif // PRE_FIX_MAIL_HEADICON

		if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP ) {

			// #41767 은신 상태효과 보유중일 때는 캐시 파츠 이펙트 안보이게 해달라고 함.
			if( false == IsAppliedThisStateBlow( STATE_BLOW::BLOW_073 ) || IsLocalActor() )
			{
				DrawPVPCharInfo( fElapsedTime );
				CDnActor::RenderCustom( fElapsedTime );
			}
			else
			{
				GetInterface().ShowPlayerGauge( GetActorHandle(), false);
			}

			return;
		}

		bool bPartyMember = CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsPartyMember( GetUniqueID() );
		if( bPartyMember ) {
			if( CGameOption::GetInstance().bPartyUserName == false ) return;
		}
		else {
			if( CGameOption::GetInstance().bUserName == false ) return;
		}

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
		CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pVillageTask && pVillageTask->GetVillageType() == CDnVillageTask::PvPVillage ) bShowPVPLevelMark = true;
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
				AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, textcolor::NAVY, textcolor::WHITE );
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

#ifdef PRE_FIX_MAIL_HEADICON
		int nLastHeadLineIndex = 0;
		// 길드마크
		if( CGameOption::GetInstance().bGuildName == true && CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark( m_GuildSelfView ) )
		{
			nLastHeadLineIndex = 1;
			AddHeadNameElement(nLastHeadLineIndex, GetGuildTask().GetGuildMarkTexture( m_GuildSelfView ), 8.f, 8.f, 1, 1, 0, 0xFFFFFFFF, fTextureOffset );
		}

		// 길드
		if( CGameOption::GetInstance().bGuildName == true && m_GuildSelfView.IsSet() ) {
			nLastHeadLineIndex = 1;
			WCHAR wszGuildName[GUILDNAME_MAX+2];
			swprintf_s( wszGuildName, GUILDNAME_MAX+2, L"<%s>", m_GuildSelfView.wszGuildName );
			AddHeadNameElement( nLastHeadLineIndex, wszGuildName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
		}

#ifdef PRE_ADD_DWC
		// DWC 팀 이름
		if( GetDWCTask().IsDWCRankSession() && GetDWCTask().IsDWCChar() && IsJoinGuild() == false )
		{
			nLastHeadLineIndex = 1;
			WCHAR wszDWCTeamName[GUILDNAME_MAX+2] = { 0 , };
			if(wcslen(m_wszDWCTeamName))
			{
				swprintf_s( wszDWCTeamName, GUILDNAME_MAX+2, L"<%s>", m_wszDWCTeamName);
				AddHeadNameElement( nLastHeadLineIndex, wszDWCTeamName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
			}			
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

			nLastHeadLineIndex = 2;
			AddHeadNameElement( nLastHeadLineIndex, m_hAbsenceMarkTex, vSize, D3DCOLOR_ARGB(nAlpha,255,255,255), 15.f );
		}

		if (nLastHeadLineIndex < 2)
		{
			nLastHeadLineIndex = 2;
			RenderMailBoxMark(nLastHeadLineIndex);
		}
#else // PRE_FIX_MAIL_HEADICON
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
							if( GetDWCTask().IsDWCRankSession() && GetDWCTask().IsDWCChar() && IsJoinGuild() == false )
							{
								WCHAR wszDWCTeamName[GUILDNAME_MAX+2] = { 0 , };
								if(wcslen(m_wszDWCTeamName))
								{
									swprintf_s( wszDWCTeamName, GUILDNAME_MAX+2, L"<%s>", m_wszDWCTeamName);
									AddHeadNameElement( 1, wszDWCTeamName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
								}
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
#endif // PRE_FIX_MAIL_HEADICON
	}
	else {
		if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
		{
			GetInterface().ShowPlayerGauge( GetActorHandle() , false );
		}
	}
	CDnActor::RenderCustom( fElapsedTime );

#ifdef RENDER_PRESS_hayannal2009
	RenderPress();
#endif
}

#if defined( PRE_ADD_REVENGE )
void CDnPlayerActor::DrawRevengeName( const UINT eRevenge )
{
	DWORD dwFontColor, dwFontColorS;
	if( Revenge::RevengeTarget::eRevengeTarget_Target == eRevenge )
	{
		dwFontColor = textcolor::PVP_REVENGE_TARGET;
		dwFontColorS = textcolor::PVP_E_PLAYER_SH;
	}
	else
	{
		dwFontColor = EtInterface::textcolor::PVP_REVENGE_ME;
		dwFontColorS = textcolor::PVP_E_PLAYER_SH;
	}

	float fTextureOffset = 0.0f;
	if( CEtFontMng::s_bUseUniscribe ) fTextureOffset = -2.0f;

#ifdef PRE_MOD_PVPRANK
	if( GetPvPLevel() > 0 )
		AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 1, 0xFFFFFFFF, fTextureOffset );
#else
	if( GetPvPLevel() > 1 )
		AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 2, 0xFFFFFFFF, fTextureOffset );
#endif

	AddHeadNameElement( 0, GetPvPLevelName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );
	AddHeadNameElement( 0, GetName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );

	if( CGameOption::GetInstance().bGuildName == true && CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark( m_GuildSelfView ) )
		AddHeadNameElement( 1, GetGuildTask().GetGuildMarkTexture( m_GuildSelfView ), 8.f, 8.f, 1, 1, 0, 0xFFFFFFFF, fTextureOffset );

	if( CGameOption::GetInstance().bGuildName == true && m_GuildSelfView.IsSet() ) {
		WCHAR wszGuildName[GUILDNAME_MAX+2];
		swprintf_s( wszGuildName, GUILDNAME_MAX+2, L"<%s>", m_GuildSelfView.wszGuildName );
		AddHeadNameElement( 1, wszGuildName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
	}
}
#endif	// #if defined( PRE_ADD_REVENGE )

void CDnPlayerActor::DrawPVPName( bool isMyTeam, bool isSimple, float fElapsedTime )
{
	DWORD dwFontColor, dwFontColorS;
	DWORD dwFontColorA, dwFontColorAS;
	GetHeadIconFontColor( this, MAHeadNameRender::Normal, dwFontColor, dwFontColorS );
	GetHeadIconFontColor( this, MAHeadNameRender::Appellation, dwFontColorA, dwFontColorAS );

	if( isMyTeam && isSimple ) {
		if(GetName()) 
		{
			AddHeadNameElement( 0, /*(WCHAR*)DN_INTERFACE::STRING::GetClassString( GetClassID() )*/GetName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );
		}
		else
		{
			AddHeadNameElement( 0, /*(WCHAR*)DN_INTERFACE::STRING::GetClassString( GetClassID() )*/GetJobName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );
		}// 혹시모르니 넣어둡니다.
		return;
	}

	float fTextureOffset = 0.0f;
	if( CEtFontMng::s_bUseUniscribe ) fTextureOffset = -2.0f;

#ifdef PRE_MOD_PVPRANK
	if( GetPvPLevel() > 0 )
		AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 1, 0xFFFFFFFF, fTextureOffset );
#else
	if( GetPvPLevel() > 1 )
		AddHeadNameElement( 0, GetInterface().GetPVPIconBigTex(), 8.f, 8.f, 8, 8, GetPvPLevel() - 2, 0xFFFFFFFF, fTextureOffset );
#endif

	if( isMyTeam ) {
		if( IsSetAppellationIndex() )
			AddHeadNameElement( 0, (WCHAR*)m_wszAppellationName.c_str(), s_nFontIndex, 16, dwFontColorA, dwFontColorAS );
	}
	else 
	{
		AddHeadNameElement( 0, GetPvPLevelName(), s_nFontIndex, 16, dwFontColor, dwFontColorS );
	}

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
		
		WCHAR wszTemp[GUILDNAME_MAX] = { 0 , };
		if(isMyTeam)
		{
			wcscpy(wszTemp, GetDWCTask().GetMyDWCTeamName());
		}
		else
		{
			wcscpy(wszTemp, GetDWCTask().GetEnemyTeamName());
		}
	
		swprintf_s( wszDWCTeamName, GUILDNAME_MAX+2, L"<%s>", wszTemp);
		AddHeadNameElement( 1, wszDWCTeamName, s_nFontIndex, 16, dwFontColor, dwFontColorS );
	}
#endif
}



void CDnPlayerActor::DrawPVPCharInfo( float fElapsedTime )
{
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pGameTask || pGameTask->GetGameTaskType() != GameTaskType::PvP ) return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor *pLocalPlayerActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());

	float fDistance = 0.0f;
	bool bSameTeam = (pLocalPlayerActor->GetTeam() == GetTeam());

#if defined( PRE_ADD_REVENGE )
	bool bRevengeUser = false;
	CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
	if( NULL != pRevengeTask && CDnBridgeTask::GetInstance().GetSessionID() != GetUniqueID() )
	{
		UINT eRevengeState = Revenge::RevengeTarget::eRevengeTarget_None;
		pRevengeTask->GetRevengeUserID( GetUniqueID(), eRevengeState );

		if( Revenge::RevengeTarget::eRevengeTarget_Target == eRevengeState 
			|| Revenge::RevengeTarget::eRevengeTarget_Me == eRevengeState )
		{
			bRevengeUser = true;
			fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );
			if( 1000.f > fDistance )
				DrawRevengeName( eRevengeState );
		}
	}
#endif	// #if defined( PRE_ADD_REVENGE )

	// PVP에서 보이는 플레이어게이지는 옵션에 영향받지 않는다.
	if(pLocalPlayerActor->IsObserver())
	{
		if( ((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_IndividualRespawn) 
		{
#if defined( PRE_ADD_REVENGE )
			if( false == bRevengeUser )
#endif	// #if defined( PRE_ADD_REVENGE )
				DrawPVPName( false, false, fElapsedTime ); // 개인전에는 따로 팀이존재하지않는다.
		}
		else
		{
#if defined( PRE_ADD_REVENGE )
			if( false == bRevengeUser )
#endif	// #if defined( PRE_ADD_REVENGE )
				DrawPVPName( GetTeam() == PvPCommon::Team::A, false, fElapsedTime );
		}
		
		bool bShowGauge = false;
		fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );

		if( fDistance < 550.f )
			bShowGauge = true;

		if( !((CDnPvPGameTask*)pGameTask)->IsLadderMode() && GetActorHandle() != pLocalPlayerActor->GetActorHandle())
			GetInterface().ShowPlayerGauge( GetActorHandle(), bShowGauge); 

		return; 
	}

	if( !bSameTeam )
	{ 
		if(CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_bPVPShowHp)
		{
			bool bShowPVPName = false;
			fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );
			if( fDistance < 550.f )
				bShowPVPName = true;

			if(bShowPVPName || m_isExposure)
			{
#if defined( PRE_ADD_REVENGE )
				if( false == bRevengeUser )
#endif	// #if defined( PRE_ADD_REVENGE )
					DrawPVPName( false, false, fElapsedTime );
			}
		}
		else 
		{
			if(m_isExposure)
			{
#if defined( PRE_ADD_REVENGE )
				if( false == bRevengeUser )
#endif	// #if defined( PRE_ADD_REVENGE )
					DrawPVPName( false, false, fElapsedTime );
			}
		}
	}
	else //같은 팀
	{ 
		bool bShowDetailInfo = false;
		bool bShowGauge = false;
		fDistance = EtVec3Length( &(*(pLocalPlayerActor->GetPosition()) -  *(GetPosition())) );
		if( fDistance < 1000.f ) {
			bShowDetailInfo = true;
			if( fDistance < 500.f && !IsDie() ) bShowGauge = true;
		}

		if( pLocalPlayerActor->IsDie() && pLocalPlayerActor->IsGhost() ) {
			DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
			if( hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera ) {
				DnActorHandle hAttachActor = ((CDnPlayerCamera*)hCamera.GetPointer())->GetAttachActor();
				if( hAttachActor != CDnActor::s_hLocalActor ) {
					if( hAttachActor == GetMySmartPtr() ) {
						bShowDetailInfo = true;
						bShowGauge = true;
					}
					else {
						bShowDetailInfo = false;
						bShowGauge = false;
					}
				}
			}
		}

#if defined( PRE_ADD_REVENGE )
		if( false == bRevengeUser )
#endif	// #if defined( PRE_ADD_REVENGE )
			DrawPVPName( true, !bShowDetailInfo, fElapsedTime );

		if( CDnBridgeTask::IsActive() && CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_bPVPShowHp )
			GetInterface().ShowPlayerGauge( GetActorHandle(), bShowGauge );
	}

	if( IsCaptainMode() )
		GetInterface().ShowPlayerGauge( GetActorHandle(), m_isExposure );
}

void CDnPlayerActor::ProcessCombo( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nComboDelay > 0 ) m_nComboDelay -= (int)( fDelta * 1000 );
	if( m_nComboDelay < 0 ) {
		OnComboFinish( m_nComboCount );
		m_nComboCount = 0;
		m_nComboDelay = 0;
	}
}

void CDnPlayerActor::EnableAnotherPlayers( bool bEnable )
{
	s_bEnableAnotherPlayer = bEnable;

	s_bHideAnotherPlayer = false; 
	s_fHideAnotherPlayerDelta = 0.f;
}

void CDnPlayerActor::HideAnotherPlayers(bool bHide, float fTime)
{
	if( !s_bEnableAnotherPlayer ) return;
	if( s_bHideAnotherPlayer == bHide ) return;
	s_bHideAnotherPlayer = bHide; 
	s_fHideAnotherPlayerDelta = fTime;
}

void CDnPlayerActor::ProcessVisual( LOCAL_TIME LocalTime, float fDelta)
{
	CDnActor::ProcessVisual( LocalTime, fDelta );

	// Process Exposure
	m_fExposureTime -= fDelta;
	if( m_fExposureTime < 0 ) {
		m_fExposureTime = 0.0f;
		m_isExposure= false;
		m_isLocalExposure = false;
	}

	if( IsShow() ) 
	{
		float fDistance = 0.0f;
		CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );

		// PVP 가 아닌 필드에서만 적용한다.
		if( CDnActor::s_hLocalActor &&  ( pGameTask && pGameTask->GetGameTaskType() != GameTaskType::PvP ) ) {
			if( GetActorHandle() == CDnActor::s_hLocalActor ) {
				GetInterface().ShowPlayerGauge( GetActorHandle(), CGameOption::GetInstance().bMyHPBar );
			}
			else {
				bool bShowGauge = false;
				fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );
				if( fDistance < 750.f && CGameOption::GetInstance().bPartyHPBar )
					bShowGauge = true;
				GetInterface().ShowPlayerGauge( GetActorHandle(), bShowGauge );
			}

		}
		else if(CDnActor::s_hLocalActor &&  ( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP)) // pvp 게임중일때는
		{
			if(CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_bPVPShowHp) // 브릿지 테스크에 저장되어있는 pvp 정보를 받아서 설정 해 줍니다.
			{
				if( GetActorHandle() == CDnActor::s_hLocalActor) 
				{
					if(!IsDie() && !IsGhost())
					{
						GetInterface().ShowPlayerGauge( GetActorHandle(), true);
					}
					else
					{
						GetInterface().ShowPlayerGauge( GetActorHandle(), false);
					}

				}
				else 
				{
					bool bShowGauge = false;
					fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );
					if( fDistance < 550.f || m_isExposure )
						bShowGauge = true;
					GetInterface().ShowPlayerGauge( GetActorHandle(), bShowGauge );

				}
			}
#if defined( PRE_ADD_REVENGE )
			else
			{
				CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
				if( NULL != pRevengeTask && CDnBridgeTask::GetInstance().GetSessionID() != GetUniqueID() )
				{
					UINT eRevengeState = Revenge::RevengeTarget::eRevengeTarget_None;
					pRevengeTask->GetRevengeUserID( GetUniqueID(), eRevengeState );

					bool bShowGauge = false;
					fDistance = EtVec3Length( &(*(CDnActor::s_hLocalActor->GetPosition()) - *(GetPosition())) );

					if( Revenge::RevengeTarget::eRevengeTarget_Target == eRevengeState
						|| Revenge::RevengeTarget::eRevengeTarget_Me == eRevengeState )
					{
						if( 550.f > fDistance )
							bShowGauge = true;
					}

					GetInterface().ShowPlayerGauge( GetActorHandle(), bShowGauge );
				}
			}
#endif	// #if defined( PRE_ADD_REVENGE )
		}

		if( m_bTogglePlayerEffect && m_hPlayerEffect)
		{
			if( !IsVehicleMode() )
			{
				EtMatrix AxisMat = GetBoneMatrix("Bip01");
				m_hPlayerEffect->SetPosition( (EtVector3)&AxisMat._41 );
			}
		}

#ifdef PRE_ADD_CASH_COSTUME_AURA 
		ShowCostumeAura();
#endif

	}
	else 
	{
		GetInterface().ShowPlayerGauge( GetActorHandle(), false );
	}
}

void CDnPlayerActor::SetExposure( bool isLocal)
{
	m_fExposureTime = 3.0f;
	m_isExposure= true;

	if( isLocal )
		m_isLocalExposure = true;
}


void CDnPlayerActor::SetDefaultPartsInfo( int *pPartsIndex )
{
	memcpy( m_nDefaultPartsIndex, pPartsIndex, sizeof(m_nDefaultPartsIndex) );
}

int CDnPlayerActor::GetDefaultPartsInfo( const CDnParts::PartsTypeEnum Index )
{
	ASSERT( Index >= CDnParts::Body );

	int nPartsIndex = Index - CDnParts::Body;
	
	if( 0 > nPartsIndex || _countof(m_nDefaultPartsIndex) <= nPartsIndex )
		return 0;

	return m_nDefaultPartsIndex[ nPartsIndex ];
}

int CDnPlayerActor::GetNudePartsInfo( CDnParts::PartsTypeEnum Index )
{
	ASSERT( Index >= CDnParts::Body && Index-CDnParts::Body < _countof(m_nDefaultPartsIndex) );
	
	int nClassID = -1;
	if( m_hNudeParts[ Index - CDnParts::Body ] ) {
		nClassID = m_hNudeParts[ Index - CDnParts::Body ]->GetClassID();
	}
	return nClassID;
}

#ifdef PRE_ADD_BESTFRIEND
void CDnPlayerActor::SetAppellationName( WCHAR * bfName )
{
	if( m_nCoverAppellationIndex != -1 )
		m_wszAppellationName = CDnAppellationTask::GetInstance().GetAppellationName( m_nCoverAppellationIndex, this );	
}
#endif

void CDnPlayerActor::SetAppellationIndex( int nCoverIndex, int nIndex )
{
	CDnPlayerState::SetAppellationIndex( nCoverIndex, nIndex );
	m_wszAppellationName.clear();

#ifdef PRE_ADD_BESTFRIEND
	if( nCoverIndex == -1 )
		m_wszAppellationName.assign( _T("") );
	else
		m_wszAppellationName = CDnAppellationTask::GetInstance().GetAppellationName( nCoverIndex, this );	
#else
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TAPPELLATION );

	int nID = -1;
	if( m_nCoverAppellationIndex == -1 )
		nID = pSox->GetItemID( m_nAppellationIndex );
	else
		nID = pSox->GetItemID( m_nCoverAppellationIndex );

	if( nID == -1 ) return;

	m_wszAppellationName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger() );
#endif

	DNTableFileFormat* pDnt = GetDNTable( CDnTableDB::TAPPELLATION );
	if( pDnt == NULL )
		return;

#ifdef PRE_ADD_NEWCOMEBACK	
	// 귀환자 칭호를 적용할때만 귀환자처리를 하도록 함.	
	bool bComeback = false;
	int nItemID = pDnt->GetItemID( nIndex );
	if( nItemID != -1 && nIndex != -1 ) 
	{		
		DNTableCell * pCell = pDnt->GetFieldFromLablePtr( nItemID, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::ComeBack )	
			bComeback = true;
	}
	
	if( CDnActor::s_hLocalActor && ( CDnActor::s_hLocalActor.GetHandle() == GetActorHandle().GetHandle() ) )
		GetInterface().SetComebackLocalUser( bComeback );	
	
	// 커버칭호 -
	nItemID = pDnt->GetItemID( nCoverIndex );
	if( nItemID != -1 && nCoverIndex != -1 ) 
	{		
		DNTableCell * pCell = pDnt->GetFieldFromLablePtr( nItemID, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::ComeBack )	
		{
			m_bComebackLocalUser = true;
		//	GetInterface().SetComebackAppellation( true );
		}
		else
		{
			m_bComebackLocalUser = false;
		//	GetInterface().SetComebackAppellation( false );
		}
	}
	else
		m_bComebackLocalUser = false;
	
	// CDnPartyMemberDlg - 파티맴버의 귀환자처리.
	if( bComeback || m_bComebackLocalUser )
		GetInterface().GetMainMenuDialog()->SetComebackAppellation( GetUniqueID(), true ); 
	else
		GetInterface().GetMainMenuDialog()->SetComebackAppellation( GetUniqueID(), false );
#endif // PRE_ADD_NEWCOMEBACK	
#ifdef PRE_ADD_GM_APPELLATION
	m_bGMAppellation = false;
	int nItemIndex = pDnt->GetItemID( nIndex );
	if( nItemIndex != -1 && nIndex != -1 ) 
	{		
		DNTableCell * pCell = pDnt->GetFieldFromLablePtr( nItemIndex, "_Type" );	
		if( pCell && pCell->GetInteger() == AppellationType::Type::eCode::GM )	
			m_bGMAppellation = true;
	}
#endif // PRE_ADD_GM_APPELLATION
#ifdef PRE_ADD_GM_APPELLAThongse
m_hongse = false;
if (nItemIndex != -1 && nIndex != -1)
{
	DNTableCell * pCell = pDnt->GetFieldFromLablePtr(nItemIndex, "_Type");
	if (pCell && pCell->GetInteger() == AppellationType::Type::eCode::hong)
		m_hongse = true;
}
#endif // PRE_ADD_GM_APPELLATION
#ifdef PRE_ADD_GM_APPELLAchengse
m_jinse = false;
if (nItemIndex != -1 && nIndex != -1)
{
	DNTableCell * pCell = pDnt->GetFieldFromLablePtr(nItemIndex, "_Type");
	if (pCell && pCell->GetInteger() == AppellationType::Type::eCode::jin)
		m_jinse = true;
}
#endif // PRE_ADD_GM_APPELLATION
}

void CDnPlayerActor::OnDecreaseEquipDurability( int nValue )
{
	if( CDnMasterTask::IsActive() && GetMasterTask().IsPlayWithPupil( GetActorHandle() ) )
		return;

	bool bRefreshStatus = false;
	for( int i=CDnParts::Helmet; i<=CDnParts::Ring2; i++ ) {
		DnPartsHandle hParts = GetParts( (CDnParts::PartsTypeEnum)i );
		if( !hParts ) continue;
		if( hParts->IsInfinityDurability() ) continue;
		if( hParts->GetDurability() == 0 ) continue;

		int nTemp = hParts->GetDurability() - nValue;
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
			hParts->OnEmptyDurability();
		}
		hParts->SetDurability( nTemp );
	}
	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		DnWeaponHandle hWeapon = m_hWeapon[i];
		if( !hWeapon ) continue;
		if( hWeapon->IsInfinityDurability() ) continue;
		if( hWeapon->GetDurability() == 0 ) continue;

		int nTemp = hWeapon->GetDurability() - nValue;
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
			hWeapon->OnEmptyDurability();
		}
		hWeapon->SetDurability( nTemp );
	}
	if( bRefreshStatus ) {
		RefreshState( RefreshEquip, ST_All );
	}

	if( GetMySmartPtr() == CDnActor::s_hLocalActor )
		GetInterface().RefreshPlayerDurabilityIcon();
}

void CDnPlayerActor::OnDecreaseEquipDurability( float fValue )
{
	if( CDnMasterTask::IsActive() && GetMasterTask().IsPlayWithPupil( GetActorHandle() ) )
		return;

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	float fTotalLevelValue = 0.0f;

	bool isPvP = GetSkillTask().QueryPVEOrPVP() == CDnSkill::PVP;
	if ( isPvP == false && IsAppliedThisStateBlow(STATE_BLOW::BLOW_259))
	{
		DNVector(DnBlowHandle) vlBlows;
		GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_259, vlBlows);
		{
			int nCount = (int)vlBlows.size();
			for (int i = 0; i < nCount; ++i)
			{
				DnBlowHandle hBlow = vlBlows[i];
				if (hBlow && hBlow->IsEnd() == false)
				{
					fTotalLevelValue += hBlow->GetFloatValue();
				}
			}
		}
	}
#endif

	bool bRefreshStatus = false;
	for( int i=CDnParts::Helmet; i<=CDnParts::Ring2; i++ ) {
		DnPartsHandle hParts = GetParts( (CDnParts::PartsTypeEnum)i );
		if( !hParts ) continue;
		if( hParts->IsInfinityDurability() ) continue;
		if( hParts->GetDurability() == 0 ) continue;

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		int nTemp = (int)( hParts->GetDurability() - (( hParts->GetMaxDurability() * fValue ) * (1 - fTotalLevelValue)) );
#else
		int nTemp = (int)( hParts->GetDurability() - ( hParts->GetMaxDurability() * fValue ) );
#endif
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
			hParts->OnEmptyDurability();
		}
		hParts->SetDurability( nTemp );
	}
	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		DnWeaponHandle hWeapon = m_hWeapon[i];
		if( !hWeapon ) continue;
		if( hWeapon->IsInfinityDurability() ) continue;
		if( hWeapon->GetDurability() == 0 ) continue;

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		int nTemp = (int)( hWeapon->GetDurability() - (( hWeapon->GetMaxDurability() * fValue ) * (1 - fTotalLevelValue)) );
#else
		int nTemp = (int)( hWeapon->GetDurability() - ( hWeapon->GetMaxDurability() * fValue ) );
#endif
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
			hWeapon->OnEmptyDurability();
		}

		hWeapon->SetDurability( nTemp );
	}
	if( bRefreshStatus ) {
		RefreshState( RefreshEquip, ST_All );
	}

	if( GetMySmartPtr() == CDnActor::s_hLocalActor )
		GetInterface().RefreshPlayerDurabilityIcon();
}

void CDnPlayerActor::LinkCashWeapon( int nEquipIndex )
{
	switch( m_hCashWeapon[nEquipIndex]->GetEquipType() ) {
		case CDnWeapon::Sword:
		case CDnWeapon::Axe:
		case CDnWeapon::Hammer:
		case CDnWeapon::Staff:
		case CDnWeapon::Book:
		case CDnWeapon::Orb:
		case CDnWeapon::Puppet:
		case CDnWeapon::Mace:
		case CDnWeapon::Flail:
		case CDnWeapon::Wand:
		case CDnWeapon::Shield:
		case CDnWeapon::Cannon:
		case CDnWeapon::BubbleGun:
		case CDnWeapon::Chakram:
		case CDnWeapon::Fan:
		case CDnWeapon::Spear:
		//case CDnWeapon::KnuckleGear:
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			break;
		case CDnWeapon::Gauntlet:
		case CDnWeapon::Glove:
		case CDnWeapon::Charm:
		case CDnWeapon::Bracelet:
		case CDnWeapon::Claw:
		case CDnWeapon::KnuckleGear:
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex, "" );
			break;
		case CDnWeapon::SmallBow:
		case CDnWeapon::BigBow:
		case CDnWeapon::CrossBow:
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			if( nEquipIndex == 0 && m_hCashWeapon[0] && m_hCashWeapon[1] ) {
				m_hCashWeapon[1]->LinkWeapon( GetMySmartPtr(), m_hCashWeapon[0] );
			}
			break;
		case CDnWeapon::Arrow:
			if( !m_hCashWeapon[0] ) break;
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), m_hCashWeapon[0] );
			break;
	}
}
#pragma message("    ---> check dnplayeractor.cpp linkcashweapon")

void CDnPlayerActor::AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete, bool bRefreshWeaponOrder )
{
	if( m_hCashWeapon[nEquipIndex] != hWeapon ) {
		if( m_hCashWeapon[nEquipIndex] ) {
			DetachCashWeapon( nEquipIndex );
		}
		m_hCashWeapon[nEquipIndex] = hWeapon;
		m_bCashSelfDeleteWeapon[nEquipIndex] = bDelete;
		if( m_hCashWeapon[nEquipIndex] ) {
			m_hCashWeapon[nEquipIndex]->CreateObject( this );
		}
	}

	if( !m_hCashWeapon[nEquipIndex] ) return;
	RefreshWeaponViewOrder( nEquipIndex );
	m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( this, nEquipIndex );
	LinkCashWeapon( nEquipIndex );
	SetBattleMode( IsBattleMode() );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hWeapon->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfoByCashItem(CASHEQUIP_WEAPON1+nEquipIndex, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

#ifdef PRE_ADD_CASH_COSTUME_AURA
	ComputeCostumeWeaponCount_ByAttach(hWeapon);
#endif
}

void CDnPlayerActor::DetachCashWeapon( int nEquipIndex )
{
#ifdef PRE_ADD_CASH_COSTUME_AURA
	ComputeCostumeWeaponCount_ByDetach(nEquipIndex);
#endif

	if( !m_hCashWeapon[nEquipIndex] ) return;

	m_hCashWeapon[nEquipIndex]->FreeObject();
	m_hCashWeapon[nEquipIndex]->UnlinkWeapon();

	if( m_bCashSelfDeleteWeapon[nEquipIndex] ) {
		SAFE_RELEASE_SPTR( m_hCashWeapon[nEquipIndex] );
		m_bCashSelfDeleteWeapon[nEquipIndex] = false;
	}
	m_hCashWeapon[nEquipIndex].Identity();

	RefreshWeaponViewOrder( nEquipIndex );

	RemoveSkillLevelUpInfoByCashItem(CASHEQUIP_WEAPON1+nEquipIndex);
}

bool CDnPlayerActor::AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete/* = false*/, bool bUseCombine/* = true*/ )
{
	if (!hParts) return false;

	bool bResult = MAPartsBody::AttachCashParts( hParts, Index, bDelete, bUseCombine );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hParts->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfoByCashItem(Index, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

#ifdef PRE_ADD_CASH_COSTUME_AURA 
	ComputeCostumePartsCount_ByAttach( hParts );
#endif

	return bResult;
}

bool CDnPlayerActor::DetachCashParts( CDnParts::PartsTypeEnum Index )
{
#ifdef PRE_ADD_CASH_COSTUME_AURA 
	ComputeCostumePartsCount_ByDetach(Index);
#endif

	RemoveSkillLevelUpInfoByCashItem(Index);

	// Note: 파츠 분리에 실패한 경우에도 스킬은 사라질 수 있음.
	bool bResult = MAPartsBody::DetachCashParts( Index );

	return bResult;
}

void CDnPlayerActor::ShowCashWeapon( int nEquipIndex, bool bShow )
{
	if( m_hCashWeapon[nEquipIndex] )
		m_hCashWeapon[nEquipIndex]->ShowWeapon( bShow );
}

void CDnPlayerActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	CDnActor::AttachWeapon( hWeapon, nEquipIndex, bDelete );

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	if( IsSwapSingleSkin() )
		hWeapon->ShowWeapon( false );
	else
		RefreshWeaponViewOrder( nEquipIndex );

	if( m_hCashWeapon[nEquipIndex] ) {
		m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( this, nEquipIndex );
		LinkCashWeapon( nEquipIndex );
	}
	SetBattleMode( IsBattleMode() );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hWeapon->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfo(CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

}

void CDnPlayerActor::DetachWeapon( int nEquipIndex )
{
	if( nEquipIndex >= 0 && nEquipIndex < 2 ) {
		m_szLastWeaponAction[nEquipIndex].clear();
		if( m_hWeapon[nEquipIndex] ) {
			m_szLastWeaponAction[nEquipIndex] = m_hWeapon[nEquipIndex]->GetCurrentDefaultAction();
		}

		CDnActor::DetachWeapon( nEquipIndex );
	}

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907
	
	RefreshWeaponViewOrder( nEquipIndex );

	RemoveSkillLevelUpInfo(CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex);
}

void CDnPlayerActor::RefreshWeaponViewOrder( int nEquipIndex )
{
	if( m_hWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex] ) {
		if( m_bWeaponViewOrder[nEquipIndex] ) {
			if( !m_hCashWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hCashWeapon[nEquipIndex]->CreateObject( this );
				m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( this, nEquipIndex );
				LinkCashWeapon( nEquipIndex );
				m_hCashWeapon[nEquipIndex]->ShowWeapon( true );
			}

			if( m_hWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hWeapon[nEquipIndex]->FreeObject();
				m_hWeapon[nEquipIndex]->ShowWeapon( false );
			}
		}
		else {
			if( !m_hWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hWeapon[nEquipIndex]->CreateObject( this );
				LinkWeapon( nEquipIndex );
				m_hWeapon[nEquipIndex]->ShowWeapon( true );
			}

			if( m_hCashWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hCashWeapon[nEquipIndex]->FreeObject();
				m_hCashWeapon[nEquipIndex]->ShowWeapon( false );
			}
		}
	}
	else if( m_hWeapon[nEquipIndex] && !m_hCashWeapon[nEquipIndex] ) {
		if( !m_hWeapon[nEquipIndex]->IsCreateObject() ) {
			m_hWeapon[nEquipIndex]->CreateObject( this );
			LinkWeapon( nEquipIndex );
			m_hWeapon[nEquipIndex]->ShowWeapon( true );
		}
	}
	else {
		bool bMainWeapon = false;
		if( nEquipIndex == 0 && GetActorType() != CDnActor::Soceress ) bMainWeapon = true;
		if( nEquipIndex == 1 && GetActorType() == CDnActor::Soceress ) bMainWeapon = true;
		if( bMainWeapon ) {
			if( m_hCashWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hCashWeapon[nEquipIndex]->FreeObject();
				m_hCashWeapon[nEquipIndex]->ShowWeapon( false );
			}
		}
		else {
			if( m_hCashWeapon[nEquipIndex] ) {
				m_hCashWeapon[nEquipIndex]->ReleaseSignalImp();
				m_hCashWeapon[nEquipIndex]->ResetDefaultAction( nEquipIndex );
			}
		}
	}

	// 아쳐같은경우 보조무기를 비전투 중일때 PullOut/PullIn 액션의 시그널을 이용해서 보조무기를 Hide 시켜준다.
	// 하지만 탈것에는 따로 무기빼는 액션이 없고 그렇다고 모든 액션에 시그널 박기도 지저분 하기 때문에 , 리프래쉬 해줄때 강제로 보조무기를 숨기도록 설정한다. 
	if(IsVehicleMode() && GetActorType() == CDnActorState::Archer && nEquipIndex == 1)
	{
		if(m_hCashWeapon[nEquipIndex] )
			m_hCashWeapon[nEquipIndex]->ShowRenderBase(false);

		if( m_hWeapon[nEquipIndex])
			m_hWeapon[nEquipIndex]->ShowRenderBase(false);
	}

	SetBattleMode( IsBattleMode() );

}

void CDnPlayerActor::SetGuildSelfView( const TGuildSelfView &GuildSelfView )
{
	m_GuildSelfView.Set(GuildSelfView);
}

void CDnPlayerActor::SetGuildView( const TGuildView &GuildView )
{
	m_GuildSelfView.Set(GuildView, m_GuildSelfView.btGuildRole);
}

void CDnPlayerActor::SetJobHistory( const std::vector<int> &nVecList )
{
	CDnPlayerState::SetJobHistory( nVecList );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	
	int nItemID = GetJobClassID();	
	if( pSox->IsExistItem( nItemID ) ) {		
		m_wszJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_JobName" )->GetInteger() );	
	}	
}

void CDnPlayerActor::SetJobHistory( int nValue )
{
	CDnPlayerState::SetJobHistory( nValue );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	
	int nItemID = GetJobClassID();	
	if( pSox->IsExistItem( nItemID ) ) {		
		m_wszJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_JobName" )->GetInteger() );	
	}	
}

void CDnPlayerActor::PopJobHistory( int nValue )
{
	CDnPlayerState::PopJobHistory( nValue );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TJOB );	
	int nItemID = GetJobClassID();	
	if( pSox->IsExistItem( nItemID ) ) {		
		m_wszJobName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_JobName" )->GetInteger() );	
	}	
}

void CDnPlayerActor::OnChangeJob( int nJobID )
{	
	CDnPlayerState::OnChangeJob( nJobID );	

	if( GetAccountLevel() < AccountLevel_New || ( GetAccountLevel() >= AccountLevel_New && IsLocalActor() ) ) {
		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();	
		if( hHandle ) {		
			hHandle->SetPosition( m_matexWorld.m_vPosition );		
			hHandle->SetActionQueue( "LevelUp" );		
			EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );		
			pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );		
			pResult->vRotate = EtVector3( 0, 0, 0);		
			pResult->bLinkObject = true;		
			pResult->bDefendenceParent = true;
		}
	}
}

void CDnPlayerActor::SetPvPLevel( char cLevel ) 
{
	CDnPlayerState::SetPvPLevel( cLevel );

	m_wszPvPLevelName.clear();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
	m_wszPvPLevelName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( m_cPvPLevel, "PvPRankUIString" )->GetInteger() );
}

void CDnPlayerActor::SetAbsence(bool bAbsence)
{
	m_bPlayerAway = bAbsence;
}

void CDnPlayerActor::ProcessDecal()
{
	if( m_hDecal ) {
		float fAlpha = GetAlpha();
		m_hDecal->Update( m_matexWorld.m_vPosition.x, m_matexWorld.m_vPosition.z, 50.0f, 0.0f, 
			EtColor(1,1,1, fAlpha), &CalcDecalHeight(), m_matexWorld.m_vPosition.y, FLT_MAX );
	}
}

void CDnPlayerActor::PreThreadRelease()
{
	CDnActor::PreThreadRelease();
	if( CDnInterface::IsActive() )
		GetInterface().ShowPlayerGauge( GetMySmartPtr(), false );
}

void CDnPlayerActor::Show( bool bShow )
{
	if( IsShow() == bShow ) 
		return;

	CDnActor::Show( bShow );

	if( IsSwapSingleSkin() )
		return;

	ShowParts( bShow );
	for( int i=0; i<2; i++ ) 
	{
#ifdef PRE_ADD_37745
		if( !m_bRTTRenderMode && CGameOption::GetInstance().m_bHideWeaponByBattleMode && i == 0)
			continue;
#endif
		CDnActor::ShowWeapon( i, bShow );
		CDnPlayerActor::ShowCashWeapon( i , bShow );
	}

	if( m_hPlayerEffect )
		m_hPlayerEffect->Show( bShow );

#ifdef PRE_ADD_CASH_COSTUME_AURA

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( CGameOption::IsActive() && (CGameOption::GetInstance().GetCurEffectQuality() != CGameOption::Effect_Low || !IsGhost()) )
#else
	if( !IsGhost() )
#endif // PRE_MOD_OPTION_EFFECT_QUALITY
	{
		if(m_bEnableCostumeAura)
		{
			for(int i = 0 ; i < eParts_Max ; ++i )
			{
				if( m_hPlayerAuraEffect[i] )
					m_hPlayerAuraEffect[i]->Show( bShow );
			}
		}
	}
#endif // PRE_ADD_CASH_COSTUME_AURA
}

void CDnPlayerActor::ShowWeapon( int nEquipIndex, bool bShow )
{
	if(IsSwapSingleSkin())
		bShow = false;

	CDnActor::ShowWeapon(nEquipIndex,bShow);
}

void CDnPlayerActor::ReleaseWeaponSignalImp()
{
	CDnActor::ReleaseWeaponSignalImp();

	for( DWORD i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		if( m_hCashWeapon[i] ) {
			m_hCashWeapon[i]->ReleaseSignalImp();
			m_hCashWeapon[i]->ResetDefaultAction( i );
		}
	}
}

void CDnPlayerActor::OnInitializePVPRoundRestart( void )
{
	CDnActor::OnInitializePVPRoundRestart();
	ToggleCaptainMode(false);
}

void CDnPlayerActor::ProcessPVPChanges(LOCAL_TIME LocalTime, float fDelta)
{
	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( !pGameTask || pGameTask->GetGameTaskType() != GameTaskType::PvP )
		return;

	if(IsDie() || IsGhost() || IsTransformMode())
	{
		GetInterface().SetPVPGameUerState( GetActorHandle() , CDnPVPGameResultDlg::eUserState::KILLED_S );
	}
	else if(CDnActor::s_hLocalActor && GetActorHandle() == CDnActor::s_hLocalActor)
	{
		GetInterface().SetPVPGameUerState( GetActorHandle() , CDnPVPGameResultDlg::eUserState::MINE_S );
	}
	else 
	{
		GetInterface().SetPVPGameUerState( GetActorHandle() , CDnPVPGameResultDlg::eUserState::Normal_S );
	}

	if(IsCaptainMode())
	{
		if(GetCurrentHairLevel()==0)
			RefreshHairLevel(1);
	}
}

void CDnPlayerActor::ToggleLinkedPlayerEffect( bool bEnable , const char *strEffectName )
{
	m_bTogglePlayerEffect = bEnable;
	SAFE_RELEASE_SPTR( m_hPlayerEffect );

	if(bEnable)
	{
		if(!m_hPlayerEffect)
		{
			m_hPlayerEffect = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		}

		if(m_hPlayerEffect)
		{
			if(IsVehicleMode())
				m_hPlayerEffect->SetParentActor(GetMyVehicleActor()->GetActorHandle());

			ActionElementStruct* pActionElement = m_hPlayerEffect->GetElement( "Loop_Release" );
			if( pActionElement )
				pActionElement->szNextActionName = strEffectName;

			m_hPlayerEffect->SetActionQueue( "Loop_Release" ); 
		}
	}
}

void CDnPlayerActor::ToggleCaptainMode( bool bEnable, bool bForce )
{
	if(m_bCaptain == bEnable && (bForce == false) )
		return;

	m_bCaptain = bEnable;

	SAFE_RELEASE_SPTR( m_hCaptainHelmetParts );
	if(m_bCaptain)
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS);
		int CAPTAIN_MODE_HELMET_RED_ITEM_ID  = 150000;
		int	CAPTAIN_MODE_HELMET_BLUE_ITEM_ID  = 150001;

		int nTableID = 0;
		char *pSkinName ="";
		char szStr[256] ="";
		sprintf_s( szStr, "_Player%d_SkinName", this->GetClassID() );

		if( (CDnActor::s_hLocalActor && GetTeam() == CDnActor::s_hLocalActor->GetTeam()) 
			|| (CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer && GetTeam() ==  PvPCommon::Team::A) )
		{
			nTableID = CAPTAIN_MODE_HELMET_BLUE_ITEM_ID; 
		}
		else
		{
			nTableID = CAPTAIN_MODE_HELMET_RED_ITEM_ID;
		}

#ifdef PRE_FIX_MEMOPT_EXT
		DNTableCell* pSkinNameField = pSox->GetFieldFromLablePtr(nTableID, szStr);
		if (pSkinNameField)
			pSkinName = CommonUtil::GetFileNameFromFileEXT(pSkinNameField->GetInteger());
#else
		pSkinName = pSox->GetFieldFromLablePtr(nTableID, szStr)->GetString();
#endif

		m_hCaptainHelmetParts = (new CDnSimpleParts)->GetMySmartPtr();
		m_hCaptainHelmetParts->LinkPartsToObject(pSkinName, m_szAniFileName.c_str(),m_hObject);
		RefreshHairLevel(1);
		GetInterface().UpdateMyPortrait();
	} 
	else
	{
		RefreshHairLevel(0);
		GetInterface().UpdateMyPortrait();
	}

	ToggleLinkedPlayerEffect( bEnable, "Loop_Captain" );

	if( IsLocalActor() )
		GetInterface().GetPlayerGauge()->SetPartyMaster( bEnable );

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	GetInterface().SetPartyPlayerGaugeAttributes( GetUniqueID() , bEnable , true , false );
	GetInterface().UpdatePartyPlayerGaugePortrait( GetUniqueID() );
#endif
}

void CDnPlayerActor::CmdShootMode(bool bTrue)
{
	m_bShootMode = bTrue;

	if(m_bShootMode)
		CmdStop("MOD_Shoot_Stand");
	else
		CmdStop("Stand");

	if(m_bShootMode && GetActorType() == CDnActor::Warrior && m_bBattleMode)
	{
		if( m_hWeapon[0] ) 
			m_hWeapon[0]->ChangeLinkBone("Bip01 L Hand");
	}

	CDnPlayerActor::CmdToggleWeaponViewOrder( 0, false );
}

void CDnPlayerActor::ProcessNonLocalShootModeAction()
{
	if(!IsLocalActor())
		return;
	
	if( strcmp( GetCurrentAction(), "MOD_Stand" ) == NULL ) {
		if( strstr( m_szCustomAction.c_str(), "MOD_Shoot" ) ) {
			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFps;
			CmdStop( "MOD_Shoot_Stand", 0, 8.f, fFrame );
			ResetCustomAction();
		}
	}
}

const char *CDnPlayerActor::GetChangeShootActionName(const char *szActionName) // 작업중.
{
	if(strcmp(szActionName,"Stand") == NULL)
	{
		szActionName = "MOD_Stand";
	}
	if(strcmp(szActionName,"Move_Front") == NULL)
	{
		szActionName = "MOD_Move_Front";
	}
	if(strcmp(szActionName,"Move_Back") == NULL)
	{
		szActionName = "MOD_Move_Back";
	}
	if(strcmp(szActionName,"Move_Left") == NULL)
	{
		szActionName = "MOD_Move_Left";
	}
	if(strcmp(szActionName,"Move_Right") == NULL)
	{
		szActionName = "MOD_Move_Right";
	}
	if(strcmp(szActionName,"Jump") == NULL)
	{
		szActionName = "MOD_Jump";
	}
	if(strstr(szActionName,"Attack1"))
	{
		szActionName = "MOD_Shoot_Stand";
	}

	return szActionName;
}

bool CDnPlayerActor::IsTransformModeSkill( int nSkillID )
{
	for( DWORD i=0; i<m_vecTransformSkillList.size(); i++ )
	{
		if( m_vecTransformSkillList[i] == nSkillID )
			return true;
	}

	return false;
}

void CDnPlayerActor::RefreshTransformMode()
{
	m_bRefreshTransformMode = false;

	if(IsDestroy()) return;

	DNTableFileFormat* pMonsterSox = GetDNTable( CDnTableDB::TMONSTER_TRANS );
	DNTableFileFormat* pMonsterSkillSox = GetDNTable( CDnTableDB::TMONSTERSKILL_TRANS );

	if(!pMonsterSox || !pMonsterSkillSox)
		return;

	if( IsProcessSkill() == true )
	{
		bool bCancelSkill = true;

		if(GetStateBlow()->IsApplied(STATE_BLOW::BLOW_232))
		{
			DNVector(DnBlowHandle) vlBlows;
			GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_232, vlBlows);

			for (DWORD i = 0; i < vlBlows.size(); i++)
			{
				if( vlBlows[i] )
				{
					CDnTransformBlow *pTransformBlow = static_cast<CDnTransformBlow*>( vlBlows[i].GetPointer() );
					if( pTransformBlow && pTransformBlow->GetParentSkillInfo()->iSkillID == m_hProcessSkill->GetClassID() )
					{
						bCancelSkill = false;
						break;
					}
				}
			}
		}

		if( bCancelSkill == true )
		{
			CancelUsingSkill();
			SetAction("Stand",0.f,0.f);
		}
	}
	else
	{
		SetAction("Stand",0.f,0.f);
	}

	if( m_bTransformMode ) 
	{
		if( pMonsterSox->IsExistItem(m_nTransformTableID) == false ) 
			return;

		int nActorIndex = pMonsterSox->GetFieldFromLablePtr( m_nTransformTableID , "_ActorTableID" )->GetInteger();
		int nSkillTableIndex = pMonsterSox->GetFieldFromLablePtr( m_nTransformTableID , "_SkillTable" )->GetInteger();
		m_nTransformMoveSpeed = pMonsterSox->GetFieldFromLablePtr( m_nTransformTableID, "_MoveSpeed" )->GetInteger();

		DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
		if(!pActorSox || !pActorSox->IsExistItem(nActorIndex))
			return;
		if(nActorIndex > 0)
		{
			SwapSingleSkin( nActorIndex , true);
			ShowParts(false);
			for( int i=0; i<2; i++ ) ShowWeapon( i, false );
			ResetCustomAction();
			ResetMixedAnimation( false );
			SetAction( "Stand", 0.f, 0.f );
		}
		else
			return;

		if( pMonsterSkillSox && pMonsterSkillSox->IsExistItem(nSkillTableIndex))
		{
			if(!m_vecTransformSkillList.empty()) // 이미 적용된 스킬이있을때는 지워준다. < 변신했다가 또변신하는경우 >
			{
				for(int i=0; i<(int)m_vecTransformSkillList.size(); i++)
				{
					if(m_vecTransformSkillList[i] != -1 )
						RemoveSkill(m_vecTransformSkillList[i]);
				}

				m_vecTransformSkillList.clear();
			}

			for(int i=0; i<PvPCommon::Common::MonsterMutationSkillColCount; i++)
			{
				char szStr[256];
				int nSkillIndex = -1;
				int nSkillLevel = -1;
				sprintf_s( szStr, "_SkillIndex%d", i+1 );
				nSkillIndex = pMonsterSkillSox->GetFieldFromLablePtr( nSkillTableIndex, szStr )->GetInteger();
				sprintf_s( szStr, "_SkillLevel%d", i+1 );
				nSkillLevel = pMonsterSkillSox->GetFieldFromLablePtr( nSkillTableIndex, szStr )->GetInteger();

				if(nSkillIndex != -1 && nSkillLevel != -1)
				{
					m_vecTransformSkillList.push_back(nSkillIndex);
					AddSkill(nSkillIndex,nSkillLevel);
				}
				else
					break;
			}
		}

		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle )
		{
			std::string nEffectName = pMonsterSox->GetFieldFromLablePtr( m_nTransformTableID , "_EffectByStart" )->GetString();
			hHandle->SetPosition( m_matexWorld.m_vPosition );
			if( nEffectName.empty() == false )
				hHandle->SetActionQueue( nEffectName.c_str() ); 
		}
	}
	else 
	{
		SwapSingleSkin( -1 , true );
		SetActionQueue( IsDie() ? "Die" : m_strTransformEndAction.c_str() );

		for(int i=0; i<(int)m_vecTransformSkillList.size(); i++)
		{
			if(m_vecTransformSkillList[i] != -1 )
				RemoveSkill(m_vecTransformSkillList[i]);
		}

		m_vecTransformSkillList.clear();

		DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
		if( hHandle ) 
		{
			std::string nEffectName = pMonsterSox->GetFieldFromLablePtr( m_nTransformTableID , "_EffectByEnd" )->GetString();
			hHandle->SetPosition( m_matexWorld.m_vPosition );
			if( nEffectName.empty() == false )
				hHandle->SetActionQueue( nEffectName.c_str() ); 
		}

		m_nTransformTableID = -1;

		if( IsCaptainMode() == true )
			ToggleCaptainMode( IsCaptainMode() , true );

	}

	SetBattleMode(true);
	RefreshState();

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	GetInterface().UpdatePartyPlayerGaugePortrait( GetUniqueID() );
#endif
}

void CDnPlayerActor::ToggleTransformMode(bool bEnable,int nMonsterMutatorTableID , bool bForce, const char* strEndAction)
{
	if( m_bTransformMode == bEnable && !bForce )
		return;

	if( bEnable == true )
		m_nTransformTableID = nMonsterMutatorTableID;
	
	m_bTransformMode = bEnable;
	m_bRefreshTransformMode = true;
	m_strTransformEndAction = strEndAction;
}

#ifdef PRE_ADD_TRANSFORM_POTION
void CDnPlayerActor::SetRefrehVillageTransformMode( int nIndex )
{
	m_nTransformItemIndex = nIndex; 
	m_bRefreshVillageTransformMode = true;
}

void CDnPlayerActor::ToggleVillageTransformMode( bool bEnable , int nItemIndex , int nDurationTime )
{
	m_bRefreshVillageTransformMode = false;

	if( IsDestroy() )
		return;

	if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) 
	{
#ifdef PRE_ADD_HEAD_SCALE_POTION
		if( CDnItem::GetItemType( nItemIndex ) == ITEMTYPE_HEAD_SCALE_POTION )
		{
			if( bEnable )
			{
				if( m_nTransformItemIndex > 0 && nItemIndex != m_nTransformItemIndex && CDnItem::GetItemType( m_nTransformItemIndex ) == ITEMTYPE_TRANSFORMPOTION )
					ToggleVillageTransformMode( false, 0, 0 );

				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				int nRate = pSox->GetFieldFromLablePtr( nItemIndex, "_TypeParam1" )->GetInteger();
				float fRate = nRate * 0.01f;
				SetHeadScale( fRate );
				m_nTransformItemIndex = nItemIndex;
			}
			else
			{
				SetHeadScale( 1.0f );
				m_nTransformItemIndex = 0;
			}

			DnEtcHandle hCommonEffect = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();	
			if( hCommonEffect ) 
			{
				hCommonEffect->SetPosition( *( GetPosition() ) );
				hCommonEffect->SetActionQueue( bEnable ? "SwapGhost" : "Rebirth" );
			}
			return;
		}
#endif

		if( bEnable )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( pSox && pSox->IsExistItem( nItemIndex ) )
			{
				const int TRANSFORM_GHOST_INDEX = 500; // 유령의 경우 클래스별로 액터가 존재해서 따로 예외처리해줍니다.

				int nActorIndex = pSox->GetFieldFromLablePtr( nItemIndex, "_TypeParam1" )->GetInteger();
				int nSkillIndex = pSox->GetFieldFromLablePtr( nItemIndex, "_SkillID" )->GetInteger();
				if( nDurationTime > 0 )
				{
					DelEffectSkill( nItemIndex );
					AddEffectSkill( nItemIndex, nSkillIndex, nDurationTime * 1000 , false , false );
				}

				SetBattleMode( false );

				if( nActorIndex == TRANSFORM_GHOST_INDEX )
				{
					SwapSingleSkin( 499 + GetClassID() , true );
				}
				else
				{
					SwapSingleSkin( nActorIndex , true );
				}
			}
			m_nTransformItemIndex = nItemIndex;
		}
		else
		{
			DelEffectSkill( m_nTransformItemIndex );
			SwapSingleSkin( -1 , true );
			m_nTransformItemIndex = 0;
		}

		DnEtcHandle hCommonEffect = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();	
		if( hCommonEffect ) 
		{
			hCommonEffect->SetPosition( *( GetPosition() ) );
			hCommonEffect->SetActionQueue( bEnable ?  "SwapGhost" : "Rebirth" );
		}
#ifdef PRE_ADD_HEAD_SCALE_POTION
		SetHeadScale( 1.0f );
#endif
	}
}
#endif

void CDnPlayerActor::ClearFishingRod()
{
	if( m_hFishingRodParts )
	{
		m_hFishingRodParts->FreeObject();
		SAFE_RELEASE_SPTR( m_hFishingRodParts );
	}

	m_bShowFishingRod = false;
}

#ifdef PRE_ADD_CASHFISHINGITEM
void CDnPlayerActor::CreateFishingRod( int nFishingRodItemID )
{
	if( nFishingRodItemID <= 0 )
		return;

	DNTableFileFormat* pSoxFishingRod = GetDNTable( CDnTableDB::TFISHINGROD );
	if( pSoxFishingRod == NULL )
	{
		DN_ASSERT( 0, "Invalid pSoxFishingRod" );
		return;
	}

	std::string szFileName = pSoxFishingRod->GetFieldFromLablePtr( nFishingRodItemID, "_fishingrodfilename" )->GetString();

	if( szFileName.length() > 0 )
	{
		std::string szSkinFileName = szFileName + ".skn";
		std::string szAniFileName = szFileName + ".ani";
		std::string szActFileName = szFileName + ".act";

		ClearFishingRod();

		m_hFishingRodParts = (new CDnSimpleParts)->GetMySmartPtr();

		if( m_hFishingRodParts )
		{
			if( !m_hFishingRodParts->CreateObject( szSkinFileName.c_str(), szAniFileName.c_str() ) )
				ClearFishingRod();

			if( !m_hFishingRodParts->LoadAction( szActFileName.c_str() ) )
				ClearFishingRod();
		}
	}
}
#else // PRE_ADD_CASHFISHINGITEM
void CDnPlayerActor::CreateFishingRod( const char* pszSkinName, const char* pszAniName, const char* pszActName )
{
	ClearFishingRod();

	m_hFishingRodParts = (new CDnSimpleParts)->GetMySmartPtr();

	if( m_hFishingRodParts )
	{
		if( !m_hFishingRodParts->CreateObject( pszSkinName, pszAniName ) )
			ClearFishingRod();

		if( !m_hFishingRodParts->LoadAction( pszActName ) )
			ClearFishingRod();
	}
}
#endif // PRE_ADD_CASHFISHINGITEM

void CDnPlayerActor::ShowFishingRod( bool bShowFishingRod )
{
	if( m_bShowFishingRod == bShowFishingRod )
		return;

	m_bShowFishingRod = bShowFishingRod;

	if( m_bShowFishingRod && m_hFishingRodParts )
	{
		m_hFishingRodParts->LinkPartsToBone( GetActorHandle(), "Bip01 L Hand" );
		m_hFishingRodParts->ShowRenderBase( true );
	}
	else if( !m_bShowFishingRod && m_hFishingRodParts )
	{
		m_hFishingRodParts->UnlinkParts();
		m_hFishingRodParts->ShowRenderBase( false );

		StopHeadEffect( HeadEffect_Fishing );
		m_fFishingTimer = FISHING_EFFECT_GAP;
		m_bStartCast = false;
		m_bStartLift = false;
		m_nFishingRetCode = ERROR_FISHING_FAIL;
		m_nFishingRewardItemID = -1;
		m_bRecieveFishingReward = false;
	}
}

void CDnPlayerActor::SetFishingRodAction( const char* pszActionName )
{
	if( !m_bShowFishingRod )
		return;

	if( m_hFishingRodParts )
		m_hFishingRodParts->SetActionQueue( pszActionName );
}

void CDnPlayerActor::SetFishingReward( int nRetCode, int nRewardItemID )
{
	m_nFishingRetCode = nRetCode;
	m_nFishingRewardItemID = nRewardItemID;
	m_bRecieveFishingReward = true;
}

void CDnPlayerActor::ProcessFishing( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_bShowFishingRod || !m_hFishingRodParts )
		return;

	m_hFishingRodParts->Process( LocalTime, fDelta );

	if( GetCurrentActionIndex() == GetElementIndex( "Fishing_casting" ) 
		|| GetCurrentActionIndex() == GetElementIndex( "Fishing_Stand" )
		|| GetCurrentActionIndex() == GetElementIndex( "Fishing_Stand2" ) )	// 낚시증 이펙트 셋팅
	{
		if( GetCurrentActionIndex() == GetElementIndex( "Fishing_Stand" ) && 
			m_hFishingRodParts->GetCurrentActionIndex() != m_hFishingRodParts->GetElementIndex( "Fishing Rod_Stand" ) )
			SetFishingRodAction( "Fishing Rod_Stand" );
		else if( GetCurrentActionIndex() == GetElementIndex( "Fishing_Stand2" ) &&
			m_hFishingRodParts->GetCurrentActionIndex() != m_hFishingRodParts->GetElementIndex( "Fishing Rod_Stand2" ) )
			SetFishingRodAction( "Fishing Rod_Stand2" );

		if( !m_bStartCast )
		{
			StopHeadEffect( HeadEffect_Fishing );

			m_fFishingTimer -= fDelta;
			if( m_fFishingTimer <= 0.0f )
			{
				SetHeadEffect( EffectType_Fishing, EffectState_Start );
				m_fFishingTimer = FISHING_EFFECT_GAP;
				m_bStartCast = true;
			}
		}
	}
	else if( GetCurrentActionIndex() == GetElementIndex( "Fishing_Lift" ) )	// lift동작 시 이펙트 삭제
	{
		if( !m_bStartLift && IsEffectPlaying() == HeadEffect_Fishing )
		{
			StopHeadEffect( HeadEffect_Fishing );
			m_bStartLift = true;
		}
	}
	else
		m_bStartCast = false;

	if( m_bStartLift )	// 낚시결과 이펙트 셋팅
	{
		int nCurrentIndex = GetCurrentActionIndex();
		int nElementIndex = GetElementIndex( "Fishing_Normal Stand2" );

		if( nCurrentIndex == nElementIndex && m_bRecieveFishingReward )
		{
			if( m_nFishingRetCode == ERROR_NONE && m_nFishingRewardItemID != ERROR_FISHING_FAIL )	// 낚시 성공 시
			{
				TItemInfo itemInfo;
				if( CDnItem::MakeItemInfo( m_nFishingRewardItemID, 1, itemInfo ) )
				{
					CDnItem* pItem = GetItemTask().CreateItem( itemInfo );
					if( pItem )
					{
						if( pItem->GetItemType() == ITEMTYPE_RANDOM )
							SetHeadEffect( EffectType_Fishing, EffectState_VeryGood );
						else
						{
							if( pItem->GetItemRank() < ITEMRANK_A )
								SetHeadEffect( EffectType_Fishing, EffectState_Normal );
							else
								SetHeadEffect( EffectType_Fishing, EffectState_Good );
						}
					}
					SAFE_DELETE( pItem );
				}
			}
			else if( m_nFishingRetCode == ERROR_FISHING_FAIL )	// 낚시 실패 시
			{
				SetHeadEffect( EffectType_Fishing, EffectState_Fail );
			}

			m_nFishingRetCode = ERROR_FISHING_FAIL;
			m_nFishingRewardItemID = -1;
			m_bStartLift = false;
			m_bRecieveFishingReward = false;
		}
	}
}

void CDnPlayerActor::SummonPet( int nPetItemID, LPCWSTR strPetName, bool bSummonAni )
{
	if( nPetItemID == -1 )
	{
		if( m_hPet && bSummonAni )
			m_hPet->SetAction( "Summon_Off", 0, 3.0f );
		else
			SAFE_RELEASE_SPTR( m_hPet );

		m_bSummonPet = false;
		m_fPetFollowDeltaTime = 0.0f;
		m_fPetPickUpRange = 0.0f;
		m_strPetPickUpActionName = "";
	}
	else
	{
		DNTableFileFormat*  pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
		if( !pVehicleTable || !pVehicleTable->IsExistItem(nPetItemID) )
			return;

		int nPetActorTableID = pVehicleTable->GetFieldFromLablePtr( nPetItemID, "_VehicleActorID" )->GetInteger();
		int nPetMoveSpeed = pVehicleTable->GetFieldFromLablePtr( nPetItemID, "_VehicleDefaultSpeed" )->GetInteger();

		if( m_hPet )
			SAFE_RELEASE_SPTR( m_hPet );

		m_hPet = CreateActor( nPetActorTableID, false, false , false );

		if( m_hPet )
		{
			m_hPet->Initialize();

			CDnPetActor *pPetActor = dynamic_cast<CDnPetActor*>(m_hPet.GetPointer());
			if(!pPetActor)
				return ;

			pPetActor->SetPetInfo( m_sPetCompact );
			pPetActor->SetMyMaster( this );
			pPetActor->SetDefaultMesh();
			
			m_hPet->SetMoveSpeed( nPetMoveSpeed );
			m_hPet->SetPosition( *( GetPosition() ) );
			m_hPet->Show( true );
			m_hPet->SetMaxHP( 10000 );
			m_hPet->SetHP( 10000 );
			m_hPet->SetName( strPetName );
			m_hPet->ShowHeadName( true );
			m_hPet->SetTeam( GetTeam() );

			EtVector3 vZVec = *GetPosition() - *( m_hPet->GetPosition() );

			if( vZVec.x == 0 && vZVec.y == 0 && vZVec.z == 0 )
				vZVec = *GetLookDir();
			
			EtVec3Normalize( &vZVec, &vZVec );
			m_hPet->CmdLook( EtVec3toVec2( vZVec ), false );

			if( bSummonAni )
				m_hPet->SetAction( "Summon_On", 0, 3.0f );
			else
				m_hPet->SetAction( "Stand", 0, 3.0f );

			m_fPetFollowDeltaTime = 0.0f;
			m_fPetPickUpRange = static_cast<float>( pVehicleTable->GetFieldFromLablePtr( nPetItemID, "_Range" )->GetInteger() );
			m_strPetPickUpActionName = pVehicleTable->GetFieldFromLablePtr( nPetItemID, "_Petacteffect" )->GetString();

			pPetActor->ChangeColor( ePetParts::PET_PARTS_BODY, GetPetInfo().dwPartsColor1 );
			pPetActor->ChangeColor( ePetParts::PET_PARTS_NOSE, GetPetInfo().dwPartsColor2 );

			m_hPet->SetProcess(true);

			m_bSummonPet = true;
			m_bChatExpire = false;

			// 스킬 셋팅
			if( m_sPetCompact.nSkillID1 > 0 )
				AddSkill( m_sPetCompact.nSkillID1, 1 );
			if( m_sPetCompact.nSkillID2 > 0 )
				AddSkill( m_sPetCompact.nSkillID2, 1 );

			if( IsLocalActor() )
				GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
		}

		// Create Effect Act
		if( !m_hPetEffect )
		{
			m_hPetEffect = (new CDnEtcObject)->GetMySmartPtr();

			if( !m_hPetEffect->Initialize( NULL, NULL, "PetCommonEffect.act" ) )
				SAFE_RELEASE_SPTR( m_hPetEffect );
		}
	}

	RefreshState( CDnActorState::RefreshEquip );
	ResetStateEffects();
}

bool CDnPlayerActor::ProcessPet( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_bSummonPet || m_hPet == NULL ) return false;
	if( !CDnVehicleTask::IsActive() ) return false;

	if( m_hPet->GetCurrentActionIndex() == m_hPet->GetElementIndex( "Summon_On" ) || 
		m_hPet->GetCurrentActionIndex() == m_hPet->GetElementIndex( "Summon_Off" ) )
		return false;

	if( !m_bChatExpire )	// 만료기간 지났는지 체크
	{
		const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();

		TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( GetPetInfo().Vehicle[Pet::Slot::Body].nSerial );
		if( pPetCompact && !pPetCompact->Vehicle[Pet::Slot::Body].bEternity )
		{
			if( pNowTime >= pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )
			{
				GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9222 ), false );
				m_bChatExpire = true;
			}
		}
	}

	if( m_hPet->GetState() & ActorStateEnum::Attack )
		m_bFollowPetAni = false;
	else
		ProcessFollowPet( LocalTime, fDelta );

	return true;
}

void CDnPlayerActor::ProcessFollowPet( LOCAL_TIME LocalTime, float fDelta )
{
	bool bStopFollow = false;
	float fMinLength = 100.0f;
	float fMinLengthMargin = 50.0f;
	float fLength;
	EtVector3 vStart, vEnd;
	EtVector2 vLook;

	vStart = *( m_hPet->GetPosition() );
	vEnd =	*( GetPosition() );
	vLook.x = vEnd.x - vStart.x;
	vLook.y = vEnd.z - vStart.z;
	fLength = EtVec2Length( &vLook );
	vLook /= fLength;

	if( fLength > 1000.0f )	// 특정거리 이상 벌어지면 강제 소환
	{
		m_hPet->ResetMove();
		m_hPet->SetPosition( *GetPosition() );
		m_hPet->CmdStop( "Stand" );
	}
	else
	{
		if( !m_bPetSlipAni && fLength > 500.0f )
			m_bPetSlipAni = true;

		int nCurrentIndex = m_hPet->GetCurrentActionIndex();
		int nSlipActionIndex = m_hPet->GetElementIndex( "Landing" );
		if( nCurrentIndex == nSlipActionIndex )
			return;

		int nStandGap = 0;
		if( strstr( m_hPet->GetCurrentAction(), "Stand_" ) || strstr( m_hPet->GetCurrentAction(), "Social_" ) )
			nStandGap = 100;

		if( fLength < fMinLength + fMinLengthMargin && !IsAir() )
		{
			bStopFollow = true;
			m_bFollowPetAni = false;
		}
		else if( fLength >= fMinLength + fMinLengthMargin + nStandGap )
		{
			m_fPetFollowDeltaTime += fDelta;
			if( m_fPetFollowDeltaTime >= 0.4f )
			{
				m_fPetFollowDeltaTime -= 0.4f;

				EtVector3 vXVec, vZVec;

				vZVec = *GetPosition() - *( m_hPet->GetPosition() );
				EtVec3Normalize( &vZVec, &vZVec );

				m_hPet->CmdLook( EtVec3toVec2( vZVec ), false );

				EtVec3Cross( &vXVec, &vZVec, &EtVector3( 0.f, 1.f, 0.f ) );
				vXVec.y = vZVec.y = 0.f;
				EtVec3Normalize( &vXVec, &vXVec );
				EtVec3Normalize( &vZVec, &vZVec );
				m_hPet->SetMoveVectorX( vXVec );
				m_hPet->SetMoveVectorZ( vZVec );

				EtVector3 vMovePos = vStart;
				vMovePos.x += vLook.x * ( fLength - fMinLength + fMinLengthMargin );
				vMovePos.z += vLook.y * ( fLength - fMinLength + fMinLengthMargin );

				if( !m_bFollowPetAni )
				{
					m_hPet->SetActionQueue( "Move_Front", -1, 3.f, 0.f, false );
					m_bFollowPetAni = true;
				}

				m_hPet->MovePos( vMovePos, false );
			}
		}

		if( bStopFollow && m_hPet->IsMove() )
		{
			if( m_bPetSlipAni )
				m_hPet->CmdStop( "Landing" );
			else
				m_hPet->CmdStop( "Stand" );

			m_bPetSlipAni = false;
		}
	}
}

int CDnPlayerActor::GetPetClassType()
{
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if(!pVehicleTable || !pVehicleTable->IsExistItem(m_sPetCompact.Vehicle[Pet::Slot::Body].nItemID))
		return -1;

	int PetClassType = -1;
	PetClassType = pVehicleTable->GetFieldFromLablePtr( m_sPetCompact.Vehicle[Pet::Slot::Body].nItemID , "_VehicleClassID" )->GetInteger();

	return PetClassType;
}

CDnPetActor* CDnPlayerActor::GetMyPetActor()
{
	if( m_hPet )
	{
		return dynamic_cast<CDnPetActor*>(m_hPet.GetPointer());
	}
	return NULL;
}

bool CDnPlayerActor::IsInPetSummonableMap()
{
	m_bPetSummonableMap = false;
	int nCurrentMapID = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox || !pSox->IsExistItem(CGlobalInfo::GetInstance().m_nCurrentMapIndex) )
		return m_bPetSummonableMap;

	m_bPetSummonableMap = pSox->GetFieldFromLablePtr( nCurrentMapID, "_PetMode" )->GetInteger() ? true : false;

	return m_bPetSummonableMap;
}

void CDnPlayerActor::HideSummonPet( bool bHide )
{
	if( m_hPet == NULL ) return; 

	m_hPet->Show( !bHide );
	m_hPet->SetProcess( !bHide );
}

void CDnPlayerActor::SetPetEffectAction( const char* szActionName )
{
	if( m_hPetEffect && m_hPetEffect->GetCurrentActionIndex() == -1 && m_hPetEffect->GetElementIndex( szActionName ) != -1 )
	{
		m_hPetEffect->SetPosition( *( m_hPet->GetPosition() ) );
		m_hPetEffect->SetParentActor( m_hPet );

		// 액션인덱스에 헤드이펙트 디파인오프셋 넣고, 시그널 인덱스에 이펙트타입 넣어서 관리한다.
		EtcObjectSignalStruct *pResult = m_hPet->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, m_hPetEffect );
		pResult->bLinkObject = true;
		pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
		pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
		pResult->bDefendenceParent = false;
		pResult->szBoneName[0] = 0;

		m_hPetEffect->SetActionQueue( szActionName );
	}
}

void CDnPlayerActor::AddPetNotEatItemList(DWORD dropItemUniqueID)
{
	if (IsLocalActor() == false)
		return;

	if (m_hPet && IsSummonPet())
	{
		CDnPetActor* pPetActor = static_cast<CDnPetActor*>(m_hPet.GetPointer());
		if (pPetActor == NULL) return;

		pPetActor->AddNotEatItemList(dropItemUniqueID);
	}
}

bool CDnPlayerActor::AttachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex,
									 const EffectOutputInfo* pEffectInfo, /*OUT*/ DnEtcHandle& hEtcObject, /*OUT*/ DNVector(DnEtcHandle)& vlhDummyBoneEtcObjects,
									  bool bAllowSameSkillDuplicate/* = false*/ )
{
	bool bRetValue = false;
	if( CDnPetTask::IsActive() && GetPetTask().IsPetSkill( ParentSkillInfo.iSkillID ) )
	{
		if( IsSummonPet() && m_hPet )
			bRetValue = m_hPet->AttachSEEffect( ParentSkillInfo, emBlowIndex, nIndex, pEffectInfo, hEtcObject, vlhDummyBoneEtcObjects, bAllowSameSkillDuplicate );
	}
	else
	{
		bRetValue = CDnActor::AttachSEEffect( ParentSkillInfo, emBlowIndex, nIndex, pEffectInfo, hEtcObject, vlhDummyBoneEtcObjects, bAllowSameSkillDuplicate );
	}

	return bRetValue;
}

bool CDnPlayerActor::DetachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex, 
									const EffectOutputInfo* pEffectInfo )
{
	bool bRetValue = false;
	if( CDnPetTask::IsActive() && GetPetTask().IsPetSkill( ParentSkillInfo.iSkillID ) )
	{
		if( IsSummonPet() && m_hPet )
			bRetValue = m_hPet->DetachSEEffect( ParentSkillInfo, emBlowIndex, nIndex, pEffectInfo );
	}
	else
	{
		bRetValue = CDnActor::DetachSEEffect( ParentSkillInfo, emBlowIndex, nIndex, pEffectInfo );
	}

	return bRetValue;
}

bool CDnPlayerActor::ChangeSocialTexture( int nSocialIndex, int nFrameIndex )
{
	if(CDnActor::s_hLocalActor) // 액터에서 확인후에 업데이트를  해줍니다.
	{
		CDnLocalPlayerActor *pLocalPlayerActor = (CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer();
		if( pLocalPlayerActor->GetFollowObserverUniqueID() == GetUniqueID() && pLocalPlayerActor->IsFollowObserver() )
			GetInterface().UpdateTargetPortrait( GetActorHandle() );
	}

	bool bResult = MAPartsBody::ChangeSocialTexture( nSocialIndex, nFrameIndex );
	return bResult;
}


#ifdef PRE_ADD_VIP
bool CDnPlayerActor::IsAlwaysVIPHeadRender() const
{
	return (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon || 
			CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap);
}
#endif

void CDnPlayerActor::OnResetAttachWeaponAction( DnWeaponHandle hWeapon, int nEquipIndex )
{
	if( !hWeapon ) return;

	if( !IsBattleMode() ) {
		if( nEquipIndex >= 0 && nEquipIndex < 2 ) {
			if( !m_szLastWeaponAction[nEquipIndex].empty() && !strstr( GetCurrentAction(), "_Weapon" ) && !strstr( GetCurrentAction(), "Stand" ) ) {
				if( hWeapon->IsExistAction( m_szLastWeaponAction[nEquipIndex].c_str() ) ) {
					hWeapon->SetAction( m_szLastWeaponAction[nEquipIndex].c_str(), 0.f, 0.f );
					return;
				}
			}
		}
		if( !strstr( GetCurrentAction(), "Social_Sit01" ) ) {
			// 소서리스 책 따위는 닫혀잇는 상태를 Normal_Stand 로 해놓아서.. 이쪽에서 채크해줘야 인형이 서있거나 책이 펴져있는 상태로 시작하지않습니다.
#ifdef PRE_FIX_BOW_ACTION_MISMATCH
			if (hWeapon->IsEnableNormalStandAction())
#else
			if( hWeapon->IsExistAction( "Normal_Stand" ) )
#endif
				hWeapon->SetAction( "Normal_Stand", 0.f, 0.f );
		}
	}
}

void CDnPlayerActor::SetPartsVehicleEffect(DnPartsVehicleEffectHandle hHandle, bool bDelete)
{
	if( m_hPartsVehicleEffect && m_bSelfDeleteVehicleEffect ) {
		SAFE_RELEASE_SPTR( m_hPartsVehicleEffect );
		m_bSelfDeleteVehicleEffect = false;
	}

	m_hPartsVehicleEffect = hHandle;
	m_bSelfDeleteVehicleEffect = bDelete;
	if(IsVehicleMode() && GetMyVehicleActor() && hHandle == CDnPartsVehicleEffect::Identity())
		GetMyVehicleActor()->ReleaseSignalImp();
	
	if(IsVehicleMode() && GetMyVehicleActor())
	{
		GetMyVehicleActor()->CmdStop("Stand",0,10.f);
		if(IsLocalActor() && CDnActor::s_hLocalActor)
		{
			((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->ReleaseAllButton();
			CDnCharStatusDlg * pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			CDnCharVehicleDlg * pCharVehicleDlg = pCharStatusDlg->GetVehicleDlg();
			if( pCharVehicleDlg )
			{
				pCharVehicleDlg->SetAvatarAction("Stand");
			}
		}
	}
	// 탈것 이펙트가 존재하는경우 애니메이션이 도중 체인지가 되기때문에 스탠드 모션을 한번잡아준다. 추가로 이펙트 모션자체가 스탠드 상태에 존재하기때문에
	// 스탠드모션이 되어야만 이펙트가 보이게 되므로 유의하자.
}


int CDnPlayerActor::GetVehicleClassType()
{
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if(!pVehicleTable || !pVehicleTable->IsExistItem(m_sVehicleCompact.Vehicle[Vehicle::Slot::Body].nItemID))
		return -1;

	int VehicleClassType = -1;
	VehicleClassType = pVehicleTable->GetFieldFromLablePtr( m_sVehicleCompact.Vehicle[Vehicle::Slot::Body].nItemID , "_VehicleClassID" )->GetInteger();

	return VehicleClassType;
}

CDnVehicleActor* CDnPlayerActor::GetMyVehicleActor()
{
	if(m_hVehicleActor)
		return static_cast<CDnVehicleActor*>(m_hVehicleActor.GetPointer());

	return NULL;
}

bool CDnPlayerActor::IsCanVehicleMode()
{
	if( IsSwapSingleSkin() || IsDie() || IsGhost() || m_nTeam == PvPCommon::Team::Observer ) 
		return false;

	if( IsSpectatorMode() ) 
		return false;

	int nCurrentMapID = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if(!pSox || !pSox->IsExistItem(CGlobalInfo::GetInstance().m_nCurrentMapIndex))
		return false;

	int bVehicleMode = 0;
	bVehicleMode = pSox->GetFieldFromLablePtr( nCurrentMapID, "_VehicleMode" )->GetInteger();

#if defined( PRE_ADD_FORCE_RIDE_ENABLE_TRIGGER )
	bVehicleMode = bVehicleMode && m_bForceEnableRideByTrigger;
#endif	// #if defined( PRE_ADD_FORCE_RIDE_ENABLE_TRIGGER )

	return bVehicleMode ? true : false;
}

void CDnPlayerActor::SetForceEnableRide( const bool bForceEnableRide )
{
	m_bForceEnableRideByTrigger = bForceEnableRide;
}

void CDnPlayerActor::CallVehicle()
{
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if(!pVehicleTable || !pVehicleTable->IsExistItem(m_sVehicleCompact.Vehicle[Vehicle::Slot::Body].nItemID))
		return;

	int nSummonTime = -1;
	nSummonTime = pVehicleTable->GetFieldFromLablePtr( m_sVehicleCompact.Vehicle[Vehicle::Slot::Body].nItemID, "_VehicleSummonTime" )->GetInteger();
	if(nSummonTime == -1)
		return;

	m_fRideDelay = (float)nSummonTime / 1000.f;
	SetActionQueue("Vehicle_Call");

	if(GetActorHandle() == CDnActor::s_hLocalActor)
		CDnLocalPlayerActor::LockInput(true);
}

void CDnPlayerActor::CancelCallVehicle()
{
	m_fRideDelay = -1;
	if(GetActorHandle() == CDnActor::s_hLocalActor)
		CDnLocalPlayerActor::LockInput(false);
}

void CDnPlayerActor::ProcessVehicleCall(LOCAL_TIME LocalTime, float fDelta)
{
	if(m_fRideDelay != -1)
	{
		if(m_fRideDelay >= 0)
		{
			m_fRideDelay -= fDelta;
		}
		else
		{
			if(GetActorHandle() == CDnActor::s_hLocalActor)
				CDnLocalPlayerActor::LockInput(false);

			if(!IsVehicleMode())
			{
				if(!IsDestroy() && GetObjectHandle() && !IsDie() && !IsGhost() && IsShow() && !m_bEnableVehicleThread)
				{
					InsertRideVehicleThread(GetActorHandle());
				}
			}
		}
	}
}

void CDnPlayerActor::InsertRideVehicleThread( DnActorHandle hActor )
{
	DWORD dwUniqueID = hActor->GetUniqueID();

	if(IsLocalActor()) // 내꺼 탈때는 남의 패킷까지 신경안써도 된다 내가타는 쓰레드 호출중에 로딩패킷 왔다갔다하면 자꾸 밀려서 타지못하는 경우가 생긴다.
	{
		RideVehicle(GetVehicleInfo());
	}
	else
	{
		m_bEnableVehicleThread = true;
		CDnLoadingTask::GetInstance().InsertLoadObject( 
			OnLoadRideVehicle, 
			OnLoadRideVehicleUniqueID,
			&CVillageClientSession::OnCheckLoadingPacket,
			NULL,
			this, (void*)&dwUniqueID, sizeof(DWORD),0);
	}
}

int __stdcall CDnPlayerActor::OnLoadRideVehicleUniqueID( void *pParam, int nSize ) 
{ 
	return *((DWORD *)pParam); 
}

bool __stdcall CDnPlayerActor::OnLoadRideVehicle( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
{
	CDnPlayerActor* pTemp = (CDnPlayerActor*)pThis;
	if( !pTemp ) return false;

	pTemp->RideVehicle(pTemp->GetVehicleInfo());
	return true;
}

void CDnPlayerActor::RideVehicle(TVehicleCompact sInfo , bool bForce)
{
	if(!IsPlayerActor() || IsVehicleMode() || IsDie() || !m_hObject || IsDestroy())
	{
		m_bEnableVehicleThread = false;
		return;
	}

	if(!IsCanVehicleMode())
	{
		m_bEnableVehicleThread = false;
		return;
	}

	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
	if(!pVehicleTable || !pVehicleTable->IsExistItem(sInfo.Vehicle[Vehicle::Slot::Body].nItemID))
	{
		m_bEnableVehicleThread = false;
		return;
	}

	int nVehicleActorTableID = pVehicleTable->GetFieldFromLablePtr( sInfo.Vehicle[Vehicle::Slot::Body].nItemID , "_VehicleActorID" )->GetInteger();

	bool bIsLocal = false;
	if(GetActorHandle() == CDnActor::s_hLocalActor) // 로컬 플레이어면 = 로컬 탈것
	{
		bIsLocal = true;
	}
	
	MatrixEx PlayerCross = *GetMatEx();
	
	DnVehicleHandle hVehicle = CreateActor( nVehicleActorTableID , bIsLocal ,false , false);
	if(!hVehicle || !hVehicle->GetObjectHandle() ) // 액터 생성 실패
	{
		MessageBox( NULL, L"Actor File Loading Failed", L"Error", MB_OK );
		m_bEnableVehicleThread = false;
		return;
	}

	if( IsBattleMode() )
		SetBattleMode(false);

	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && GameTaskType::PvP == pGameTask->GetGameTaskType() && PvPCommon::GameMode::PvP_GuildWar == ((CDnPvPGameTask *)pGameTask)->GetGameMode()  )
		CmdToggleBattle( false );
	
	for(int i=0;i<CDnWeapon::EquipSlot_Amount;i++)
	{
		if(GetWeapon(i))
			OnResetAttachWeaponAction(GetWeapon(i),i);
		
		if(m_hCashWeapon[i])
			OnResetAttachWeaponAction(m_hCashWeapon[i],i);
	}

	SetMyVehicleActor(hVehicle->GetActorHandle());
	hVehicle->SetMyPlayerActor(GetActorHandle());

	hVehicle->SetUniqueID(GetVehicleSessionID());
	hVehicle->SetLocalVehicle(bIsLocal); // 내가 로컬 탈것인가를 알고있습니다.
	hVehicle->Initialize();
	hVehicle->LinkPlayerToVehicle();
	hVehicle->SetItemID(sInfo.Vehicle[Vehicle::Slot::Body].nItemID); // 자신의 아이템 아이디를 가지고 있습니다.

#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	std::string strVehicleAction = pVehicleTable->GetFieldFromLablePtr( sInfo.Vehicle[Vehicle::Slot::Body].nItemID , "_RiderString" )->GetString();
	
	if(!strVehicleAction.empty() && strstr(strVehicleAction.c_str() , "Vehicle_") != NULL )
		hVehicle->SetVehicleActionString(strVehicleAction.c_str());
#endif
	
	// 부위별 장착 // 파츠 정보를 함유하고 들어오는 경우는 장착을 시켜줍니다.
	for(int i= Vehicle::Slot::Saddle; i<Vehicle::Slot::Max; i++)
	{
		if(sInfo.Vehicle[i].nItemID != 0 && sInfo.Vehicle[i].nSerial != 0)
		{
			hVehicle->EquipItem(sInfo.Vehicle[i]);
		}
	}

	if(sInfo.dwPartsColor1 != 0 && sInfo.dwPartsColor1 != -1) 
		hVehicle->ChangeHairColor(sInfo.dwPartsColor1); // 설정된 색이 있는경우에는 색 설정을 해줍니다.
		
	hVehicle->SetVehicleClassID(sInfo.Vehicle[Vehicle::Slot::Body].nItemID);
	hVehicle->RefreshState();

	float fLandHeight = INSTANCE(CDnWorld).GetHeight( PlayerCross.m_vPosition );
	hVehicle->SetPosition(PlayerCross.m_vPosition);
	hVehicle->SetPrevPosition(PlayerCross.m_vPosition);
	hVehicle->SetAddHeight( PlayerCross.m_vPosition.y - fLandHeight );
	hVehicle->GetMatEx()->CopyRotationFromThis(&PlayerCross);

	if(CDnActor::s_hLocalActor && GetActorHandle() == CDnActor::s_hLocalActor)
	{
		CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera(CDnCamera::PlayerCamera).GetPointer(); 
		// 컷신이나 , NPC 카메라에 붙어있는 경우가 있으니 , GetActiveCamera를 사용할때 오류가 발생한다. 주의

		if(pCamera && pCamera->GetCameraType() == CDnCamera::PlayerCamera )
		{
			pCamera->DetachActor();
			pCamera->AttachActor( hVehicle->GetActorHandle() );
			pCamera->SetZoomDistance(CAMERA_VEHICLE_ZOOM_MIN ,CAMERA_VEHICLE_ZOOM_MAX);
		}
		hVehicle->InitializeSession(CDnActor::s_hLocalActor->GetSession()); // Rotha 세션동기화 해야됩니다 그래야 SC_ 메세지가 정상동작.
	}

	hVehicle->SetAttachToPlayer(true);
	SetVehicleMode(true);
	SetIncreaseHeight(-35.f); // HeadName 
#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	SetHeadEffectAdjustHeight( 0.0f );
#endif

	hVehicle->SetActionQueue( "Stand" );
	hVehicle->SetSoftAppear( true );
	hVehicle->SetProcess(true);

	DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
	if( hHandle && !bForce && !s_bHideAnotherPlayer) {
		hHandle->SetPosition( m_matexWorld.m_vPosition );
		hHandle->SetActionQueue( "SummonVehicle" );
	}

	m_fRideDelay = -1;
	m_bEnableVehicleThread = false;

	LinkCashParts( CDnParts::CashFairy );
	if(m_hPlayerEffect)
		m_hPlayerEffect->SetParentActor(hVehicle);

	return;
}

void CDnPlayerActor::UnRideVehicle(bool bIgnoreEffect,bool bIgnoreAction)
{
	if(!IsVehicleMode() || !GetMyVehicleActor())
		return;
		
	float fLandHeight = INSTANCE(CDnWorld).GetHeight( GetMyVehicleActor()->GetMatEx()->m_vPosition );
	MatrixEx *Cross = GetMyVehicleActor()->GetMatEx();
	SetPosition(GetMyVehicleActor()->GetMatEx()->m_vPosition);
	SetPrevPosition(GetMyVehicleActor()->GetMatEx()->m_vPosition);
	SetAddHeight( GetMyVehicleActor()->GetMatEx()->m_vPosition.y - fLandHeight );

	GetMyVehicleActor()->SetAttachToPlayer(false);
	GetMyVehicleActor()->SetDestroy();
	GetObjectHandle()->SetParent(GetPostParentHandle(),-1);
	GetMatEx()->CopyRotationFromThis(Cross);

	SetIncreaseHeight(15.f); // HeadName 디폴트값
	SetVehicleMode(false);
#ifdef PRE_MOD_HEAD_EFFECT_HEIGHT
	SetHeadEffectAdjustHeight( 30.0f );
#endif

	if(!bIgnoreAction)
		SetActionQueue("Stand");

	if(IsBattleMode())
		CmdToggleBattle(false);

	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && GameTaskType::PvP == pGameTask->GetGameTaskType() && PvPCommon::GameMode::PvP_GuildWar == ((CDnPvPGameTask *)pGameTask)->GetGameMode()  )
		CmdToggleBattle( true );

	if(GetActorHandle() == CDnActor::s_hLocalActor)
	{
		CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera(CDnCamera::PlayerCamera).GetPointer(); 
		// 컷신이나 , NPC 카메라에 붙어있는 경우가 있으니 , GetActiveCamera를 사용할때 오류가 발생한다. 주의

		if(pCamera && pCamera->GetCameraType() == CDnCamera::PlayerCamera)
		{
			pCamera->DetachActor();
			pCamera->AttachActor( GetActorHandle() );
			pCamera->ResetZoomDistance();
		}
	}

	DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
	if( hHandle && !bIgnoreEffect && !s_bHideAnotherPlayer) {
		hHandle->SetPosition( m_matexWorld.m_vPosition );
		hHandle->SetActionQueue( "SummonVehicle" );
	}

	LinkCashParts( CDnParts::CashFairy );
	if(m_hPlayerEffect)
		m_hPlayerEffect->SetParentActor(GetActorHandle());

	return;
}


DnWeaponHandle CDnPlayerActor::GetActiveWeapon( int nEquipIndex )
{
	if( m_bWeaponViewOrder[nEquipIndex] && m_hCashWeapon[nEquipIndex] ) return m_hCashWeapon[nEquipIndex];
	return CDnActor::GetActiveWeapon( nEquipIndex );
}

void CDnPlayerActor::SetWeaponFPS( float fValue )
{
	CDnActor::SetWeaponFPS( fValue );

	for( DWORD i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		if( m_hCashWeapon[i] ) m_hCashWeapon[i]->CDnActionBase::SetFPS( fValue );
	}
}

void CDnPlayerActor::ToggleCannonMode( bool bCannonMode, DWORD dwCannonMonsterID, const MatrixEx& Cross )
{
	m_bPlayerCannonMode = bCannonMode;
	
	// 대포 몬스터에게 할당.
	m_hCannonMonsterActor = CDnActor::FindActorFromUniqueID( dwCannonMonsterID );
	if( m_hCannonMonsterActor )
	{
		CDnCannonMonsterActor* pCannonActor = dynamic_cast<CDnCannonMonsterActor*>( m_hCannonMonsterActor.GetPointer() );
		m_bPlayerCannonMode = bCannonMode;
		if( bCannonMode )
		{
			if( pCannonActor ) pCannonActor->SetMasterPlayerActor( GetMySmartPtr() );

			EtVector3 vPos = Cross.m_vPosition;
			MovePos( vPos, false );

			SetWeight(0.f);
			SetPressLevel(-1);
			m_hObject->SetCalcPositionFlag( -1 ); // 액션해도 움직이지 않도록 설정
		}
		else
		{
			if( pCannonActor ) pCannonActor->ClearMasterPlayerActor();
			m_matexWorld = Cross;
			EndCannonMode();
		}
	}
}

void CDnPlayerActor::OnCannonMonsterDie( void )
{
	EndCannonMode();

	SetActionQueue( "Stand" );
}

void CDnPlayerActor::EndCannonMode( void )
{
	m_bPlayerCannonMode = false;
	m_hCannonMonsterActor.Identity();

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
}


bool CDnPlayerActor::CanAddSkill( int nSkillTableID, int nLevel /*= 1*/ )
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	if( pSkillTable == NULL )
		return false;
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && ((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival) 
	{
		CDnSkill::SkillTypeEnum  eSkillType		= (CDnSkill::SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillTableID, "_SkillType" )->GetInteger();
		CDnSkill::DurationTypeEnum eDurationType = (CDnSkill::DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillTableID, "_DurationType" )->GetInteger();
		if( CDnSkill::Passive == eSkillType && CDnSkill::DurationTypeEnum::Buff == eDurationType )
			return false;
	}
	return true;
}

void CDnPlayerActor::OnAddSkill( DnSkillHandle hSkill, bool isInitialize/* = false*/ )
{
	MASkillUser::OnAddSkill( hSkill, isInitialize );

	// 추가된 스킬이 강화 패시브 스킬인 경우.(강화 패시브 스킬을 획득 했거나 다른 플레이어가 레벨업 한 경우)
	if( CDnSkill::EnchantPassive == hSkill->GetSkillType() )
	{
		int iBaseSkillID = hSkill->GetBaseSkillID();
		DnSkillHandle hBaseSkill = FindSkill( iBaseSkillID );
		if( hBaseSkill )
		{
#if defined(PRE_FIX_64312)
			//소환몬스터용 스킬이 경우 바로 적용 하지 않고 담아 놓고, MAAiSkill에서 UseSkill시점에 적용 한다.
			bool isSummonMonsterSkill = false;

			isSummonMonsterSkill = hBaseSkill->IsSummonMonsterSkill();

			if (isSummonMonsterSkill == false)
				hBaseSkill->ApplyEnchantSkill( hSkill );
			else
				hBaseSkill->AddSummonMonsterEnchantSkill(hSkill);
#else
			hBaseSkill->ApplyEnchantSkill( hSkill );
#endif // PRE_FIX_64312
		}
	}
	else
	{
		// 추가된 스킬이 갖고 있는 강화 패시브 스킬의 베이스 스킬인 경우. (다른 플레이어의 강화된 스킬이 레벨업 된 경우)
		CheckAndApplyEnchantPassiveSkill( hSkill );
	}

#if defined(PRE_FIX_CLIENT_MEMOPTIMIZE)
	if( hSkill->IsUseInVillage() ) {
		const set<string> &setActionList = hSkill->GetUseActionSet();
		for( set<string>::const_iterator iter = setActionList.begin(); iter != setActionList.end(); iter++ ) {
			RefreshUsingElement( (*iter).c_str() );
		}
	}
#endif
}

void CDnPlayerActor::OnRemoveSkill( DnSkillHandle hSkill )
{
	MASkillUser::OnRemoveSkill( hSkill );

	// 다른 플레이어의 패시브 강화 스킬이 레벨업 되어 이전 레벨의 스킬 객체가 삭제 루틴을 타고 이쪽으로 오는 경우.
	// 적용되고 있던 베이스 스킬의 강화 상태를 리셋으로 돌린다.
	if( CDnSkill::EnchantPassive == hSkill->GetSkillType() &&
		0 < hSkill->GetBaseSkillID() )
	{
		DnSkillHandle hBaseSkill = FindSkill( hSkill->GetBaseSkillID() );
		if( hBaseSkill )
		{
			hBaseSkill->ReleaseEnchantSkill();
		}
	}

	// 다른 플레이어의 패시브 강화 스킬의 대상이 되는 베이스 스킬이 레벨업 되어 삭제 루틴을 타고 이쪽으로 오는 경우.
	// 이 경우엔 베이스 스킬 객체를 그냥 삭제하면 되므로 따로 처리할 것은 없다.
}

void CDnPlayerActor::OnReplacementSkill( DnSkillHandle hLegacySkill, DnSkillHandle hNewSkill )
{
	MASkillUser::OnReplacementSkill( hLegacySkill, hNewSkill );

	// 로컬 플레이어의 패시브 강화 스킬이 레벨업 되어 이전 레벨의 스킬 객체의 교체 루틴을 타고 이쪽으로 오는 경우.
	// 적용되고 있던 베이스 스킬의 강화 상태를 리셋으로 돌리고 레벨업된 새로운 강화 스킬의 것을 적용시킨다.
	if( CDnSkill::EnchantPassive == hLegacySkill->GetSkillType() &&
		0 < hLegacySkill->GetBaseSkillID() )
	{
		DnSkillHandle hBaseSkill = FindSkill( hLegacySkill->GetBaseSkillID() );
		if( hBaseSkill )
		{
			_ASSERT( hLegacySkill->GetBaseSkillID() == hNewSkill->GetBaseSkillID() );
#if defined(PRE_FIX_64312)
			//소환몬스터용 스킬이 경우 바로 적용 하지 않고 담아 놓고, MAAiSkill에서 UseSkill시점에 적용 한다.
			bool isSummonMonsterSkill = false;

			isSummonMonsterSkill = hBaseSkill->IsSummonMonsterSkill();

			if (isSummonMonsterSkill == false)
			{
				hBaseSkill->ReleaseEnchantSkill();
				hBaseSkill->ApplyEnchantSkill( hNewSkill );
			}
			else
			{
				hBaseSkill->RemoveSummonMonsterEnchantSkill();
				hBaseSkill->AddSummonMonsterEnchantSkill(hNewSkill);
			}
#else
			hBaseSkill->ReleaseEnchantSkill();
			hBaseSkill->ApplyEnchantSkill( hNewSkill );
#endif // PRE_FIX_64312
		}
	}
	else
	{
		// 로컬 플레이어의 패시브 강화 스킬의 대상이 되는 베이스 스킬이 레벨업 되어 교체 루틴을 타고 이쪽으로 오는 경우.
		// 이 경우엔 베이스 스킬 객체는 SkillTask 에서 이 루틴이 끝난 후 그냥 삭제될 것이므로 놔두고 
		// 새로 레벨업 된 베이스 스킬에 강화 스킬을 적용시켜 주면 된다.
		CheckAndApplyEnchantPassiveSkill( hNewSkill );
	}
}

void CDnPlayerActor::CheckAndApplyEnchantPassiveSkill( DnSkillHandle hBaseSkill )
{
	if( hBaseSkill )
	{
		DnSkillHandle hEnchantPassiveSkill;
#ifndef PRE_FIX_SKILLLIST
		for( DWORD i = 0; i < m_vlhSkillList.size(); i++ ) 
		{
			DnSkillHandle hSkill = m_vlhSkillList.at( i );
#else
		for( DWORD i = 0; i < GetSkillCount(); ++i )
		{
			DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
			// 강화패시브 스킬이 실제로 언락되어 획득 중일 때만.
			if( hSkill->GetBaseSkillID() == hBaseSkill->GetClassID() &&
				hSkill->IsAcquired() ) 
			{
				hEnchantPassiveSkill = hSkill;
				break;
			}
		}

#if defined(PRE_FIX_64312)
		if (hEnchantPassiveSkill)
		{
			//소환몬스터용 스킬이 경우 바로 적용 하지 않고 담아 놓고, MAAiSkill에서 UseSkill시점에 적용 한다.
			bool isSummonMonsterSkill = false;

			isSummonMonsterSkill = hBaseSkill->IsSummonMonsterSkill();

			if (isSummonMonsterSkill == false)
				hBaseSkill->ApplyEnchantSkill( hEnchantPassiveSkill );
			else
				hBaseSkill->AddSummonMonsterEnchantSkill(hEnchantPassiveSkill);
		}
#else
		if( hEnchantPassiveSkill )
			hBaseSkill->ApplyEnchantSkill( hEnchantPassiveSkill );
#endif // PRE_FIX_64312
	}
}

#ifdef PRE_ADD_MONSTER_CATCH
void CDnPlayerActor::OnReleaseFromMonster( DnActorHandle hCatcherMonsterActor )
{
	ReleaseCatcherMonsterActor();
}
#endif // #ifdef PRE_ADD_MONSTER_CATCH

bool CDnPlayerActor::IsJump()
{
	ActionElementStruct *pStruct = GetElement(GetCurrentAction());

	if( !pStruct )
		return false;

	CEtActionSignal *pSignal = NULL;
	for (int i = 0; i < (int)pStruct->pVecSignalList.size(); ++i)
	{
		pSignal = pStruct->pVecSignalList[i];
		if (pSignal && pSignal->GetSignalIndex() == STE_Jump)
			return true;
	}

	return false;
}


#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
void CDnPlayerActor::RecomposeAction()
{
	std::string szClassStr;
	std::vector<std::string> szVecActionElement;

	switch( GetClassID() ) {
		case 1: szClassStr = "warrior"; break;
		case 2: szClassStr = "archer"; break;
		case 3: szClassStr = "soceress"; break;
		case 4: szClassStr = "cleric"; break;
		case 5: szClassStr = "academic"; break;
		case 6: szClassStr = "kali"; break;
		case 7: szClassStr = "assassin"; break;
		case 8: szClassStr = "lencea"; break;
		case 9: szClassStr = "machina"; break;
	}

	// 현제 서버 위치에 따라 틀려진다.
	CTask *pTitleTask = CTaskManager::GetInstance().GetTask( "TitleTask" );
	CTask *pLoginTask = CTaskManager::GetInstance().GetTask( "LoginTask" );
	CTask *pVillageTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
	CTask *pPvPLobbyTask = CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( ( pTitleTask || pLoginTask ) && !pVillageTask && !pGameTask && !pPvPLobbyTask ) {
		szVecActionElement.push_back( FormatA( "%s_login.act", szClassStr.c_str() ) );
	}
	else if( !pTitleTask && !pLoginTask && pVillageTask && !pGameTask && !pPvPLobbyTask ) {
		// 기본적으로 읽혀야 하는 액션
		szVecActionElement.push_back( FormatA( "%s_basic.act", szClassStr.c_str() ) );
	}
	else if( !pTitleTask && !pLoginTask && ( ( !pVillageTask && pGameTask ) || ( pVillageTask && pPvPLobbyTask ) ) ) {
		// 기본적으로 읽혀야 하는 액션
		szVecActionElement.push_back( FormatA( "%s_basic.act", szClassStr.c_str() ) );

		bool bUseSkill = true;
		if( pGameTask ) {
			switch( pGameTask->GetGameTaskType() ) {
				case GameTaskType::PvP:
					szVecActionElement.push_back( FormatA( "%s_skill.act", szClassStr.c_str() ) );
					szVecActionElement.push_back( FormatA( "%s_subskill.act", szClassStr.c_str() ) );
					szVecActionElement.push_back( FormatA( "%s_damaged.act", szClassStr.c_str() ) );
					switch( ((CDnPvPGameTask*)pGameTask)->GetGameMode() ) {
						case PvPCommon::GameMode::PvP_GuildWar: 
							szVecActionElement.push_back( FormatA( "%s_subskill_guild.act", szClassStr.c_str() ) );
							break;
						case PvPCommon::GameMode::PvP_Zombie_Survival:
							szVecActionElement.push_back( FormatA( "%s_subskill_ghuolmod.act", szClassStr.c_str() ) );
							bUseSkill = false;
							break;
					}
					break;
				case GameTaskType::Farm:
					//RLKT ADDON
					szVecActionElement.push_back(FormatA("%s_skill.act", szClassStr.c_str()));
					szVecActionElement.push_back(FormatA("%s_subskill.act", szClassStr.c_str()));
					szVecActionElement.push_back(FormatA("%s_damaged.act", szClassStr.c_str()));
					szVecActionElement.push_back( FormatA( "%s_lifesystem.act", szClassStr.c_str() ) );
					bUseSkill = false;
					break;
				default:
					szVecActionElement.push_back( FormatA( "%s_skill.act", szClassStr.c_str() ) );
					szVecActionElement.push_back( FormatA( "%s_subskill.act", szClassStr.c_str() ) );
					szVecActionElement.push_back( FormatA( "%s_damaged.act", szClassStr.c_str() ) );
					break;
			}
		}
		else {
			szVecActionElement.push_back( FormatA( "%s_damaged.act", szClassStr.c_str() ) );
		}

		if( bUseSkill ) {
			// 직업별로 읽혀야 하는 액션
			std::string szJobElement;
			std::vector<int> nVecJobHistoryList;

			GetJobHistory( nVecJobHistoryList );

			for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) {
				CDnPartyTask::PartyStruct *pStruct = CDnPartyTask::GetInstance().GetPartyData(i);
				if( pStruct->cClassID != GetClassID() ) continue;

				for( DWORD j=0; j<pStruct->nVecJobHistoryList.size(); j++ ) {
					if( std::find( nVecJobHistoryList.begin(), nVecJobHistoryList.end(), pStruct->nVecJobHistoryList[j] ) == nVecJobHistoryList.end() )
						nVecJobHistoryList.push_back( pStruct->nVecJobHistoryList[j] );
				}
			}

			// 직업 추가되면 넣어야합니다. 소문자로만 인식하니 주의 해 주세요~

			for( DWORD j=0; j<nVecJobHistoryList.size(); j++ ) 
			{
				switch( nVecJobHistoryList[j] ) {
					// 워리어
					case 11: szJobElement = "warrior_skill_1st_swordmaster.act"; break;
					case 12: szJobElement = "warrior_skill_1st_mercenary.act"; break;
					case 23: szJobElement = "warrior_skill_2st_gladiator.act"; break;
					case 24: szJobElement = "warrior_skill_2st_moonlord.act"; break;
					case 25: szJobElement = "warrior_skill_2st_barbarian.act"; break;
					case 26: szJobElement = "warrior_skill_2st_destroyer.act"; break;

					// 아쳐
					case 14: szJobElement = "archer_skill_1st_bowmaster.act"; break;
					case 15: szJobElement = "archer_skill_1st_acrobat.act"; break;
					case 29: szJobElement = "archer_skill_2st_sniper.act"; break;
					case 30: szJobElement = "archer_skill_2st_artillery.act"; break;
					case 31: szJobElement = "archer_skill_2st_tempest.act"; break;
					case 32: szJobElement = "archer_skill_2st_windwalker.act"; break;

					// 소서
					case 17: szJobElement = "soceress_skill_1st_elementallord.act"; break;
					case 18: szJobElement = "soceress_skill_1st_forceuser.act"; break;
					case 35: szJobElement = "soceress_skill_2st_saleana.act"; break;
					case 36: szJobElement = "soceress_skill_2st_elestra.act"; break;
					case 37: szJobElement = "soceress_skill_2st_smasher.act"; break;
					case 38: szJobElement = "soceress_skill_2st_majesty.act"; break;

					// 클레릭
					case 20: szJobElement = "cleric_skill_1st_paladin.act"; break;
					case 22: szJobElement = "cleric_skill_1st_priest.act"; break;
					case 41: szJobElement = "cleric_skill_2st_guardian.act"; break;
					case 42: szJobElement = "cleric_skill_2st_crusades.act"; break;
					case 43: szJobElement = "cleric_skill_2st_saint.act"; break;
					case 44: szJobElement = "cleric_skill_2st_inquisitor.act"; break;

					//아카데믹
					case 46: szJobElement = "academic_skill_1st_engineer.act"; break;
					case 47: szJobElement = "academic_skill_2st_shootingstar.act"; break;
					case 48: szJobElement = "academic_skill_2st_gearmaster.act"; break;
					case 49: szJobElement = "academic_skill_1st_alchemist.act"; break;
					case 50: szJobElement = "academic_skill_2st_adept.act"; break;
					case 51: szJobElement = "academic_skill_2st_physician.act"; break;

					//칼리
					case 54: szJobElement = "kali_skill_1st_scremer.act"; break;
					case 55: szJobElement = "kali_skill_2st_darksummoner.act"; break;
					case 56: szJobElement = "kali_skill_2st_souleater.act"; break;
					case 57: szJobElement = "kali_skill_1st_dancer.act"; break;
					case 58: szJobElement = "kali_skill_2nd_bladedancer.act"; break;
					case 59: szJobElement = "kali_skill_2nd_spiritdancer.act"; break;

					//어쌔씬
					case 62: szJobElement = "assassin_skill_1st_chaser.act"; break;
					case 63: szJobElement = "assassin_skill_2st_ripper.act"; break;
					case 64: szJobElement = "assassin_skill_2st_raven.act"; break;
					case 67: szJobElement = "assassin_skill_1st_bringer.act"; break;
					case 68: szJobElement = "assassin_skill_2st_lightfury.act"; break;
					case 69: szJobElement = "assassin_skill_2st_abysswalker.act"; break;

					//lencea
					case 72: szJobElement = "lencea_skill_1st_piercer.act"; break;
					case 73: szJobElement = "lencea_skill_2st_flurry.act"; break;
					case 74: szJobElement = "lencea_skill_2st_stingbreezer.act"; break;

						
					//machina
					case 77: szJobElement = "machina_skill_1st_patrona.act"; break;
					case 78: szJobElement = "machina_skill_2st_defensio.act"; break;
					case 79: szJobElement = "machina_skill_2st_ruina.act"; break;

						
					//dark avenger
					case 75: szJobElement = "warrior_skill_1st_avenger.act"; break;
					case 76: szJobElement = "warrior_skill_2st_darkavenger.act"; break;

					//Silver Hentai
					case 80: szJobElement = "archer_skill_1st_hunter.act"; break;
					case 81: szJobElement = "archer_skill_2st_silverfox.act"; break;


				}
				if( szJobElement.empty() ) continue;
				if( std::find( szVecActionElement.begin(), szVecActionElement.end(), szJobElement ) != szVecActionElement.end() ) continue;
				szVecActionElement.push_back( szJobElement );
			}
		}
	}

	CDnCutSceneTask* pCutSceneTask = static_cast<CDnCutSceneTask*>(CTaskManager::GetInstance().GetTask( "CutSceneTask" ));
	if( pCutSceneTask && 
		false == pCutSceneTask->IsPlayEnd() ) 
	{
		szVecActionElement.push_back( FormatA( "%s_event.act", szClassStr.c_str() ) );
	}

	PROFILE_TICK_TEST_BLOCK_START( "RecomposeAction" );
	SetUsingElement( szVecActionElement );
	RefreshUsingElement();
	PROFILE_TICK_TEST_BLOCK_END();
}
#endif

// 액터가 다른 쓰레드 객체 로딩을 기다리고 있는 상태라면 리턴값 주도록 합시다 
// 쓰레드가 밀리면서 액터가 죽은다음에 호출될수 있으므로 ProcessClass 할때 SetDestroy 에 의한 액터 삭제를 미뤄줍니다
bool CDnPlayerActor::IsWaitingThread() 
{
	return m_bEnableVehicleThread;
}

void CDnPlayerActor::SyncEffectSkill()
{
	deque<S_EFFECT_SKILL>::iterator iter = m_dqEffectSkill.begin();
	for( iter; iter != m_dqEffectSkill.end(); )
	{
		S_EFFECT_SKILL& UsedItem = *iter;

		int nSize = m_pStateBlow->GetNumStateBlow();
		for ( int i = 0; i < nSize; i++ )
		{
			DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if(hBlow && !hBlow->IsFromSourceItem() && pSkillInfo && pSkillInfo->iSkillID == UsedItem.iSkillID )
			{
				hBlow->FromSourceItem();
				if( false == UsedItem.bUsedInGameServer )
				{ // CheckSourceItemBeginStuffs() 가 상태효과가 먼저들어와서 수행이 된 상태이므로 체력을 설정해 준다. 
					SetHP( GetMaxHP() );
					SetSP( GetMaxSP() );
				}
			}
		}

		++iter;
	}
}

void CDnPlayerActor::ProcessEffectSkill( LOCAL_TIME LocalTime, float fDelta )
{
	deque<S_EFFECT_SKILL>::iterator iter = m_dqEffectSkill.begin();
	for( iter; iter != m_dqEffectSkill.end(); )
	{
		S_EFFECT_SKILL& UsedItem = *iter;

#ifdef PRE_SOURCEITEM_TIMECHEAT
		if( IsLocalActor() )
		{
			CDnLocalPlayerActor *pLocal = static_cast<CDnLocalPlayerActor*>(this);
			if ( pLocal->m_bSourceItemTimeCheat && UsedItem.nEffectSkillLeftTime > 10000)
				UsedItem.nEffectSkillLeftTime = 10000;
		}
#endif

		UsedItem.nEffectSkillLeftTime -= int(fDelta * 1000.0f);
		if( UsedItem.nEffectSkillLeftTime < 0 && UsedItem.bEternity == false )
		{
			// 장비창에 달려 있다면 제거한다. 현재는 하나밖에 없지만 추후에 이런게 여러개 추가될 수 있음. 
			// 따라서 따로 아이템 구분을 위한 인자가 없다..
			if( IsLocalActor() )
				GetItemTask().RemoveUsedSourceItemReferenceObjectAndIcon( UsedItem.iItemID );

			iter = m_dqEffectSkill.erase( iter );
		}
		else
		{
			++iter;
		}
	}
}

void CDnPlayerActor::AddEffectSkill( int iItemID, int iSkillID, int iDurationTime, bool bUsedInGameServer, bool bEternity )
{
	S_EFFECT_SKILL UsedSourceItem;
	UsedSourceItem.iItemID = iItemID;
	UsedSourceItem.iSkillID = iSkillID;
	UsedSourceItem.bUsedInGameServer = bUsedInGameServer;
	UsedSourceItem.bEternity = bEternity;

	if( bEternity )
		UsedSourceItem.nEffectSkillLeftTime = -1;
	else
		UsedSourceItem.nEffectSkillLeftTime = iDurationTime;

	m_dqEffectSkill.push_back( UsedSourceItem );
}

void CDnPlayerActor::ClearEffectSkill( void )
{
	m_dqEffectSkill.clear();
}

void CDnPlayerActor::DelEffectSkill( int iItemID )
{
	deque<S_EFFECT_SKILL>::iterator iter = m_dqEffectSkill.begin();
	for( iter; iter != m_dqEffectSkill.end(); )
	{
		S_EFFECT_SKILL& UsedItem = *iter;
		if( iItemID == UsedItem.iItemID )
			iter = m_dqEffectSkill.erase( iter );
		else
			++iter;
	}
}

const CDnPlayerActor::S_EFFECT_SKILL* CDnPlayerActor::GetEffectSkillFromIndex( int iIndex ) 
{ 
	const S_EFFECT_SKILL* pResult = NULL;

	if( iIndex < (int)m_dqEffectSkill.size() )
	{
		pResult = &m_dqEffectSkill.at( iIndex );
	}

	return pResult;
}

void CDnPlayerActor::RemoveAllBubbles( bool bRemoveEvent )
{
	if( m_pBubbleSystem )
	{
		m_pBubbleSystem->RemoveAllBubbles( bRemoveEvent );
	}
}

#if defined(PRE_FIX_44486)
bool CDnPlayerActor::CheckSkillLevelUp(int nSkillID)
{
	//#44486 스킬 레벨업 아이템 장/탁찰 시 기존에 스킬이 사용중이면 아이템 장착 안되도록...
	DnSkillHandle hSkill = FindSkill(nSkillID);
	DnSkillHandle hProcessSkill = GetProcessSkill();

	//현재 사용 중인 스킬과 같은 스킬인지
	if (hProcessSkill && hProcessSkill == hSkill) return false;

	//스킬이 유효 하고
	if (hSkill && hSkill->IsAcquired())
	{
		if (hSkill->GetElapsedDelayTime() > 0.0f || hSkill->IsToggleOn())
			return false;
	}

	return true;
}
#endif // PRE_FIX_44486

bool CDnPlayerActor::OnApplySpectator(bool bEnable)
{
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
#ifdef PRE_ADD_PVP_TOURNAMENT
	if (pGameTask == NULL || pGameTask->GetGameTaskType() != GameTaskType::PvP)
		return false;

	CDnPvPGameTask* pPvPGameTask = static_cast<CDnPvPGameTask*>(pGameTask);
	if (pPvPGameTask == false)
		return false;

	byte curGameMode = pPvPGameTask->GetGameMode();
	if (curGameMode != PvPCommon::GameMode::PvP_AllKill &&
		curGameMode != PvPCommon::GameMode::PvP_Tournament)
	{
		return false;
	}
#else
	if( !(pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP && ((CDnPvPGameTask*)pGameTask)->GetGameMode() == PvPCommon::GameMode::PvP_AllKill ) ) 
		return false;
#endif

	if( bEnable )
	{
		if( IsProcessSkill() ) 
			CancelUsingSkill();

		if( IsBattleMode() )
			SetBattleMode(false);
	}
	else
	{
		CmdToggleBattle( true );
	}

	return true;
}

bool CDnPlayerActor::IsSpectatorMode()
{
	return IsAppliedThisStateBlow(STATE_BLOW::BLOW_230); 
}

DnBlowHandle CDnPlayerActor::CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkillInfo, 
												STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit/* = false*/ )
{
	DnBlowHandle hBlow = __super::CmdAddStateEffect( pParentSkillInfo, emBlowIndex, nDurationTime, szParam, bOnPlayerInit );

	if( (false == m_dqEffectSkill.empty()) &&
		pParentSkillInfo && hBlow )
	{
		// #45646 근원 아이템은 장비와 비슷한 개념이므로 최대 HP/MP 로 맞춰준다.
		// 아이템을 게임서버에서 사용했을 때 말고 마을에서 사용하고 게임서버로 나갔을 때만 최대치로 해주는데
		// 이거에 대한 판단은 게임서버에서 하고 SC_REFRESHHPSP 패킷으로 게임서버에서 날려준 것으로
		// 동기를 맞춘다.
		int iNumAppliedSourceItems = (int)m_dqEffectSkill.size();
		for( int i = 0; i < iNumAppliedSourceItems; ++i )
		{
			if( pParentSkillInfo->iSkillID == m_dqEffectSkill.at( i ).iSkillID )
			{
				hBlow->FromSourceItem();
				break;
			}
		}
	}

	return hBlow;
}


#ifdef PRE_ADD_CASH_COSTUME_AURA
void CDnPlayerActor::ProcessAdditionalAura( LOCAL_TIME LocalTime, float fDelta )
{
	for(int i = 0 ; i < eParts_Max ; ++i)
		if(m_hPlayerAuraEffect[i])  
			m_hPlayerAuraEffect[i]->Process(LocalTime, fDelta);
}

void CDnPlayerActor::LoadCostumeAuraEffect(bool bEnable, int nItemID)
{
	if( nItemID == -1 && bEnable )
	{
		if( m_SelectedCostumeData.nItemID != 0 )
			nItemID = m_SelectedCostumeData.nItemID;
		else
			return;
	}

	m_bEnableCostumeAura = bEnable;

	for(int i = 0 ; i < eParts_Max ; ++i )
	{
		if( m_bEnableCostumeAura )
		{
			if( !m_hPlayerAuraEffect[i] )
			{
				m_hPlayerAuraEffect[i] = CDnInCodeResource::GetInstance().CreateCostumeAura(nItemID);
				EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, m_hPlayerAuraEffect[i] );
				pResult->bLinkObject = true;
				pResult->bDefendenceParent = true;
			}

			if( m_hPlayerAuraEffect[i] )
			{
				if( m_vecActionName[i].empty() )
					continue;

				ActionElementStruct* pActionElement = m_hPlayerAuraEffect[i]->GetElement( m_vecActionName[i].c_str() );
				if( pActionElement )
				{
					if(m_AuraRenderType == RT_SCREEN)
						m_hPlayerAuraEffect[i]->SetRTTRenderMode(false, m_pRTTRenderer);
					else
						m_hPlayerAuraEffect[i]->SetRTTRenderMode(true, m_pRTTRenderer);
					
					m_hPlayerAuraEffect[i]->SetActionQueue( pActionElement->szName.c_str() );
				}
			}
		}
		else
		{
			SAFE_RELEASE_SPTR( m_hPlayerAuraEffect[i] );
		}
	}
}

void CDnPlayerActor::ShowCostumeAura()
{
	//if( CTaskManager::GetInstance().GetTask("LoginTask") )
	//	return;

	if( !m_bEnableCostumeAura )
		return;

	for( int i = 0 ; i < eParts_Max ; ++i )
	{
		if( m_hPlayerAuraEffect[i] == NULL )
			continue;

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
		if( CGameOption::IsActive() && (CGameOption::GetInstance().GetCurEffectQuality() == CGameOption::Effect_Low || IsGhost()) ) 
#else
		if( IsGhost() ) 
#endif
		{
			m_hPlayerAuraEffect[i]->Show(false);
		}
		else
		{
			if( m_vecBoneName[i].empty() )
				continue;

			MatrixEx ChangeMat;
			EtMatrix PlayerBoneMat = GetBoneMatrix( m_vecBoneName[i].c_str() );
			memcpy( &ChangeMat.m_vXAxis,	&PlayerBoneMat._11, sizeof(EtVector3) );
			memcpy( &ChangeMat.m_vYAxis,	&PlayerBoneMat._21, sizeof(EtVector3) );
			memcpy( &ChangeMat.m_vZAxis,	&PlayerBoneMat._31, sizeof(EtVector3) );
			memcpy( &ChangeMat.m_vPosition, &PlayerBoneMat._41, sizeof(EtVector3) );

			m_hPlayerAuraEffect[i]->SetPosition( ChangeMat.m_vPosition );
			m_hPlayerAuraEffect[i]->GetObjectCross()->CopyRotationFromThis(&ChangeMat);
			m_hPlayerAuraEffect[i]->Show(true);
		}
	}
}

void CDnPlayerActor::ComputeRTTModeCostumeAura(DnActorHandle hPlayer)
{
	if(!hPlayer) return;

	CDnPlayerActor *pActor = (CDnPlayerActor *)hPlayer.GetPointer();
	if(!pActor) return;

	//----------------------------------------------------------
	CDnPlayerActor *pTempPlayer = new CDnPlayerActor(this->GetClassID(), false);
	if(!pTempPlayer) return;

	for( int i = 0; i < CDnParts::CashPartsTypeEnum_Amount; i++ )
	{
		DnPartsHandle hParts = pActor->GetCashParts( (CDnParts::PartsTypeEnum)i );
		if( !hParts ) continue;
		int nItemID = hParts->GetClassID();
		if( nItemID <= 0) continue;

		TItemInfo ItemInfo;
		if( CDnItem::MakeItemInfo(nItemID, 1, ItemInfo) )
		{
			CDnItem *pItem = CDnItemTask::GetInstance().CreateItem(ItemInfo);
			if( !pItem ) continue;
			DnPartsHandle hParts = ((CDnParts*)pItem)->GetMySmartPtr();
			pTempPlayer->AttachCashParts(hParts, hParts->GetPartsType(), true, false);
		}
	}

	for(int j = 0 ; j < CDnWeapon::EquipSlot_Amount ; ++j)
	{
		DnWeaponHandle hOriginalWeapon = pActor->GetCashWeapon(j);
		if( !hOriginalWeapon ) continue;
		
		int nWeaponID = hOriginalWeapon->GetClassID();
		if( nWeaponID <= 0 ) continue;

		DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon(nWeaponID, 0);
		if(!hWeapon) continue;

		pTempPlayer->AttachCashWeapon(hWeapon, j, true, false);
	}

	ComputeMyEquipCostumeAura(pTempPlayer);
	SAFE_DELETE(pTempPlayer);
}

void CDnPlayerActor::ComputeRTTModeCostumeAura()
{
	// 콜로세움 프리뷰 / 캐시샵 프리뷰에서 사용함.
	// 본인것만 보면 되기 떄문에 아이템 테스크 사용함.
	// CDnRenderAvatarNew::SetActor( int nClassID, bool bShowWeapon )함수로 액터를 설정한다면, 이 함수를 호출해야한다.

	CDnItemTask *pItemTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
	if(!pItemTask) return;

	CDnPlayerActor *pPlayer = new CDnPlayerActor(this->GetClassID(), false);
	if(!pPlayer) return;
	
	for(int i=0; i<CDnParts::CashPartsTypeEnum_Amount; i++)
	{
		CDnItem *pItem = pItemTask->GetCashEquipItem(i);

		if(!pItem) continue;
		
		if(pItem->GetItemType() == ITEMTYPE_PARTS)
		{
			DnPartsHandle hParts = CDnParts::CreateParts( pItem->GetClassID(), 0, 0, 0, 0, 0, false, pItem->GetLookItemID());
			if( hParts )
				pPlayer->AttachCashParts(hParts, hParts->GetPartsType(),true,false);
		}
	}

	for(int j = 0 ; j < CDnWeapon::EquipSlot_Amount ; ++j)
	{
		CDnItem *pItem = pItemTask->GetCashEquipItem(j + CASHEQUIP_WEAPON1);

		if(!pItem) continue;
		
		if(pItem->GetItemType() == ITEMTYPE_WEAPON)
		{
			DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon( pItem->GetClassID(), 0, 0, 0, 0, 0, false, false, true, ITEMCLSID_NONE );
			if( hWeapon )
				pPlayer->AttachCashWeapon(hWeapon, j, true, false);
		}
	}

	ComputeMyEquipCostumeAura(pPlayer);
	SAFE_DELETE(pPlayer);
}

bool CDnPlayerActor::ComputeMyEquipCostumeAura(CDnPlayerActor *pPlayer)
{
	// Function : 내가 착용하고 있는 아이템을 기준으로, 코스튬 오오라를 검색
	if(pPlayer == NULL) pPlayer = this;
	
	//-----------------------------------------------------------
	// 1. Parts 저장하기.
	int						nEffectActNumber = -1;
	CDnItem*				pCashItem		 = NULL;
	std::vector<CDnItem*>	vecTempItemList;
	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if(pSox == NULL) return false;

	for( int i = 0 ; i < CDnParts::CashPartsTypeEnum_Amount ; ++i )
	{
		pCashItem = pPlayer->GetCashParts((CDnParts::PartsTypeEnum) i);
		if(!pCashItem) 
			continue;

		nEffectActNumber = pSox->GetFieldFromLablePtr(pCashItem->GetClassID(), "_SetEffect_ActName")->GetInteger();
		if(nEffectActNumber < 0)
			continue;

		CDnParts* pParts = dynamic_cast<CDnParts*>(pCashItem);
		if( pParts->GetSetItemID() > 0 && nEffectActNumber > 0 )
		{
			vecTempItemList.push_back(pCashItem);
		}
	}

	//-----------------------------------------------------------
	// 2. 무기 저장하기
	DnWeaponHandle			hWeapon;
	std::vector<CDnWeapon*> vecCashWeaponList;

	pSox = GetDNTable( CDnTableDB::TWEAPON );
	if(!pSox) return false;

	for( int i = 0 ; i < CDnWeapon::EquipSlot_Amount ; ++i )
	{
		hWeapon = pPlayer->GetCashWeapon(i);
		if(!hWeapon) 
			continue;

		nEffectActNumber = pSox->GetFieldFromLablePtr(hWeapon->GetClassID(), "_SetEffect_ActName")->GetInteger();
		if(nEffectActNumber < 0)
			continue;

		if( hWeapon->GetSetItemID() > 0 && nEffectActNumber > 0 )
		{
			vecCashWeaponList.push_back(hWeapon);
		}
	}
	if( vecTempItemList.empty() && vecCashWeaponList.empty() )
		return false;


	//-----------------------------------------------------------
	// 3. 착용중인 아이템중 가장 많은 Rank의 아이템을 기준으로, 세트아이템 여부를 체크한다.
	std::vector<CDnItem*> vecItemRankGroup[ITEMRANK_MAX];
	
	// 3.1 Parts아이템들 Rank별로 정리.
	for(int i = 0 ; i < (int)vecTempItemList.size() ; ++i)
	{
		eItemRank ItemRank = vecTempItemList[i]->GetItemRank();
		vecItemRankGroup[ItemRank].push_back( vecTempItemList[i] );
	}

	// 3.2 CashWeapon아이템들 Rank별로 정리.
	for(int i = 0 ; i < (int)vecCashWeaponList.size() ; ++i)
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
		eItemRank nRankType = (eItemRank)pSox->GetFieldFromLablePtr( vecCashWeaponList[i]->GetClassID(), "_Rank" )->GetInteger();
		if(nRankType < 0)
			continue;
		
		vecItemRankGroup[nRankType].push_back( vecCashWeaponList[i] );
	}

	// 3.3 같은 Rank가 가장 많은 벡터를 선택
	int nPivotRank		= 0;
	int nResultItemRank = -1;
	for( int i = 0 ; i < ITEMRANK_MAX; ++i )
	{
		if(vecItemRankGroup[i].empty())
			continue;

		// Pivot을 지정.
		if(i == 0)
		{
			nPivotRank = (int)vecItemRankGroup[i].size();
			nResultItemRank = i;
			continue;
		}

		if( nPivotRank < (int)vecItemRankGroup[i].size() )
		{		
			nPivotRank = (int)vecItemRankGroup[i].size();
			nResultItemRank = i;
		}
	}
	if(nResultItemRank == -1)
		return false;
	
	//-----------------------------------------------------------
	// 4. 현재 세트 아이템의 파츠의 MAX 갯수를 알아낸다. 세트아이템에 붙는 효과는 최대 12개.
	char szLabel[32];	
	int  nNumNeedSet, nMaxPartsCount = 0;
	pSox = GetDNTable( CDnTableDB::TSETITEM );
	if(pSox == NULL)
		return false;

	for( int j = 0 ; j < 12 ; j++ )
	{
		CDnParts* pPart = (CDnParts*)vecItemRankGroup[nResultItemRank][0];
		sprintf_s( szLabel, "_NeedSetNum%d", j+1 );
		nNumNeedSet = pSox->GetFieldFromLablePtr( pPart->GetSetItemID(), szLabel )->GetInteger();
		if( nNumNeedSet > 0 )
		{
			nMaxPartsCount = nNumNeedSet;
		}
	}

	if(nMaxPartsCount == (int)vecItemRankGroup[nResultItemRank].size())
	{
		std::vector<CDnItem*>::iterator it = vecItemRankGroup[nResultItemRank].begin();
		LoadCostumeAuraEffect(true, (*it)->GetClassID());
	}
	else
		LoadCostumeAuraEffect(false);


	return true;
}

void CDnPlayerActor::ComputeCostumePartsCount_ByAttach( DnPartsHandle pParts )
{
	if( !CTaskManager::GetInstance().GetTask("ItemTask") )
		return;

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( CGameOption::IsActive() && (CGameOption::GetInstance().GetCurEffectQuality() == CGameOption::Effect_Low || IsGhost()) ) 
#else
	if( IsGhost() )
#endif
	{
		OutputDebug("이펙트 옵션 '하' - 오오라를 보이지 않는다.\n");
		return;
	}
	
	int nSetItemID	= -1;
	int nItemID		= -1;
	if( ((CDnItem*)pParts)->GetItemType() == ITEMTYPE_PARTS)
	{
		nItemID		= ((CDnParts*)pParts)->GetClassID();
		nSetItemID  = ((CDnParts*)pParts)->GetSetItemID();
	}
	if( nSetItemID == 0 ) 
		return;

	int nMaxPartsCount		= GetSetItemMaxPartsCnt((int)CDnTableDB::TPARTS, nItemID, nSetItemID); // 파츠의 풀파츠 갯수.
	int nNumEquipedSetItem  = GetMyEquipSetItemPartsCnt((int)CDnTableDB::TPARTS, nItemID, nSetItemID); // 내가 장착하고 있는 파츠의 갯수.

	(nMaxPartsCount == nNumEquipedSetItem) ? m_bIsCompletParts = true : m_bIsCompletParts = false;
#ifdef _WORK
	//(m_bIsCompletParts)? OutputDebug("CDnPlayerActor::FullParts(Aura)\n") : OutputDebug("CDnPlayerActor::NotYet(Aura)\n");
#endif


	if(m_bIsCompletParts)
	{
		m_SelectedCostumeData.nItemID	 = m_nEquipeditemID;
		m_SelectedCostumeData.nSetItemID = nSetItemID;

		LoadCostumeAuraEffect(true, m_nEquipeditemID);
	}
}

void CDnPlayerActor::ComputeCostumePartsCount_ByDetach( int nSlotIndex )
{
	if( !CTaskManager::GetInstance().GetTask("ItemTask") )
		return;

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( CGameOption::IsActive() && (CGameOption::GetInstance().GetCurEffectQuality() == CGameOption::Effect_Low || IsGhost()) ) 
#else
	if( IsGhost() )
#endif
	{
		OutputDebug("이펙트 옵션 '하' - 오오라를 보이지 않는다.\n");
		return;
	}

	if( m_hCashPartsObject[nSlotIndex] == NULL )
		return;

	int nItemID		= m_hCashPartsObject[nSlotIndex]->GetClassID();
	int nSetItemID	= m_hCashPartsObject[nSlotIndex]->GetSetItemID();
	if( nSetItemID == 0 ) 
		return;

	int nMaxPartsCount		= GetSetItemMaxPartsCnt((int)CDnTableDB::TPARTS, nItemID, nSetItemID);
	int nNumEquipedSetItem	= GetMyEquipSetItemPartsCnt((int)CDnTableDB::TPARTS, nItemID, nSetItemID, true); // 내가 장착하고 있는 파츠의 갯수.

	(nMaxPartsCount == nNumEquipedSetItem) ? m_bIsCompletParts = true : m_bIsCompletParts = false;
	if( !m_bIsCompletParts )
		LoadCostumeAuraEffect(false);
}

void CDnPlayerActor::ComputeCostumeWeaponCount_ByAttach(DnWeaponHandle pParts)
{
	if( !CTaskManager::GetInstance().GetTask("ItemTask") )
		return;

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( CGameOption::IsActive() && (CGameOption::GetInstance().GetCurEffectQuality() == CGameOption::Effect_Low || IsGhost()) ) 
#else
	if( IsGhost() )
#endif
	{
		OutputDebug("이펙트 옵션 '하' - 오오라를 보이지 않는다.\n");
		return;
	}

	CDnItem* pItem  = pParts;
	int nSetItemID	= -1;
	int nItemID		= -1;
	
	CDnWeapon* pWeapon = dynamic_cast<CDnWeapon*>(pItem);
	if(pWeapon)
	{
		nSetItemID  = pWeapon->GetSetItemID();
		nItemID		= pWeapon->GetClassID();
	}
	if( nSetItemID == 0 ) 
		return;

	int nMaxPartsCount		= GetSetItemMaxPartsCnt((int)CDnTableDB::TWEAPON, nItemID, nSetItemID);
	int nNumEquipedSetItem  = GetMyEquipSetItemPartsCnt((int)CDnTableDB::TWEAPON, nItemID, nSetItemID); // 내가 장착하고 있는 파츠의 갯수.

	(nMaxPartsCount == nNumEquipedSetItem) ? m_bIsCompletParts = true : m_bIsCompletParts = false;
#ifdef _WORK
	//(m_bIsCompletParts)? OutputDebug("CDnPlayerActor::FullParts(Aura)\n") : OutputDebug("CDnPlayerActor::NotYet(Aura)\n");
#endif

	if(m_bIsCompletParts)
	{
		m_SelectedCostumeData.nItemID	 = m_nEquipeditemID;
		m_SelectedCostumeData.nSetItemID = nSetItemID;

		LoadCostumeAuraEffect(true, m_nEquipeditemID);
	}
}

void CDnPlayerActor::ComputeCostumeWeaponCount_ByDetach(int nSlotIndex )
{
	if( !CTaskManager::IsActive() || !CDnTableDB::IsActive() )
		return;

	if( !CTaskManager::GetInstance().GetTask("ItemTask") )
		return;

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if( CGameOption::IsActive() && CGameOption::GetInstance().GetCurEffectQuality() == CGameOption::Effect_Low || IsGhost() ) 
#else
	if( IsGhost() )
#endif
	{
		OutputDebug("이펙트 옵션 '하' - 오오라를 보이지 않는다.\n");
		return;
	}

	if( m_hCashWeapon[nSlotIndex] == NULL )
		return;

	int nItemID		= m_hCashWeapon[nSlotIndex]->GetClassID();
	int nSetItemID	= m_hCashWeapon[nSlotIndex]->GetSetItemID();
	if( nSetItemID == 0 ) 
		return;

	int nMaxPartsCount	   = GetSetItemMaxPartsCnt((int)CDnTableDB::TWEAPON, nItemID, nSetItemID);
	int nNumEquipedSetItem = GetMyEquipSetItemPartsCnt((int)CDnTableDB::TWEAPON, nItemID, nSetItemID, true); // 내가 장착하고 있는 파츠의 갯수.

	(nMaxPartsCount == nNumEquipedSetItem) ? m_bIsCompletParts = true : m_bIsCompletParts = false;
	if( !m_bIsCompletParts )
		LoadCostumeAuraEffect(false);
}

int CDnPlayerActor::GetSetItemMaxPartsCnt(int nTableEnum, int nCurrentItemID, int nCurrentSetItemID)
{
	std::string			szEffectActName;
	DNTableFileFormat*	pSox = GetDNTable( (CDnTableDB::TableEnum)nTableEnum );
	if(!pSox)
		return 0;

	int nEffectString = pSox->GetFieldFromLablePtr(nCurrentItemID, "_SetEffect_ActName")->GetInteger();
	if(nEffectString <= 0)
		return 0;

	szEffectActName = CDnTableDB::GetInstance().GetFileName( nEffectString );
	if(szEffectActName.empty())
		return 0;

	// 세트 아이템을 모두 장착하고 있고, 다른 종류의 셋트 아이템을 입으면 패스.
	if( m_bIsCompletParts && m_SelectedCostumeData.nSetItemID != nCurrentSetItemID ) 
		return 0;

	// 현재 세트 아이템의 파츠의 MAX 갯수를 알아낸다. 세트아이템에 붙는 효과는 최대 12개.
	char szLabel[32];
	int  nNumNeedSet;
	int  nMaxPartsCount = 0;
	DNTableFileFormat* pSetItemSox = GetDNTable( CDnTableDB::TSETITEM );
	if(!pSetItemSox)
		return 0;
	for( int j=0; j<12; j++ )
	{
		sprintf_s( szLabel, "_NeedSetNum%d", j+1 );
		nNumNeedSet = pSetItemSox->GetFieldFromLablePtr( nCurrentSetItemID, szLabel )->GetInteger();
		if( nNumNeedSet > 0 )
			nMaxPartsCount = nNumNeedSet;
	}

	return nMaxPartsCount;
}

int CDnPlayerActor::GetMyEquipSetItemPartsCnt(int nTableEnum, int nItemID, int nSetItemID, bool bIsDetach)
{
	// 장착된 템 중 현재 세트아이템에 해당되는 개수 구한다.
	int	nNumEquipedSetItem  = 0;
	std::vector<CDnItem*>	vecParts;
	std::vector<CDnWeapon*>	vecWeapon;

	// 입고있는 파츠 저장.
	for(int i = 0 ; i < CDnParts::CashPartsTypeEnum_Amount ; ++i)
	{
		if(m_hCashPartsObject[i])
		{
			if(bIsDetach)
			{
				if( m_hCashPartsObject[i]->GetClassID() == nItemID )
					continue;
			}
			vecParts.push_back(m_hCashPartsObject[i]);
		}
	}

	// 입고있는 무기 저장.
	for(int i = 0 ; i < CDnWeapon::EquipSlot_Amount ; ++i)
	{
		if(m_hCashWeapon[i])
		{
			if(bIsDetach)
			{
				if( m_hCashWeapon[i]->GetClassID() == nItemID )
					continue;
			}
			vecWeapon.push_back(m_hCashWeapon[i]);
		}
	}
	if(vecParts.empty() && vecWeapon.empty())
		return 0;


	DNTableFileFormat* pSox = NULL;
	switch(CDnItem::GetItemType(nItemID))
	{
	case ITEMTYPE_PARTS:
		{
			pSox = GetDNTable(CDnTableDB::TPARTS);
		}
		break;

	case ITEMTYPE_WEAPON:
		{
			pSox = GetDNTable(CDnTableDB::TWEAPON);
		}
		break;
	}
	if(!pSox) 
		return 0;
	int nEffectActName = pSox->GetFieldFromLablePtr(nItemID, "_SetEffect_ActName")->GetInteger();


	pSox = GetDNTable(CDnTableDB::TPARTS);
	if(!pSox)
		return 0;
	CDnItem* pEquipedItem = NULL;
	for( int i = 0 ; i < (int)vecParts.size() ; ++i )
	{
		pEquipedItem = vecParts[i];
		if( !pEquipedItem ) 
			continue;

		int nCurrentEffectActName = pSox->GetFieldFromLablePtr(pEquipedItem->GetClassID() , "_SetEffect_ActName")->GetInteger();
		if( nCurrentEffectActName != nEffectActName )
			continue;

		if( pEquipedItem->GetItemType() == ITEMTYPE_PARTS )
		{
			CDnParts* pParts = dynamic_cast<CDnParts*>(pEquipedItem);
			if( pParts->GetSetItemID() == nSetItemID )
			{
				++nNumEquipedSetItem;
				m_nEquipeditemID = pParts->GetClassID();
			}
		}
	}


	CDnWeapon* pWeapon = NULL;
	pSox = GetDNTable( CDnTableDB::TWEAPON );
	if(!pSox) return 0;
	for( int i = 0 ; i < (int)vecWeapon.size() ; ++i )
	{
		pWeapon = vecWeapon[i];
		if( !pWeapon ) 
			continue;

		int nCurrentEffectActName = pSox->GetFieldFromLablePtr(pWeapon->GetClassID() , "_SetEffect_ActName")->GetInteger();
		if( nCurrentEffectActName != nEffectActName )
			continue;

		if( pWeapon->GetItemType() == ITEMTYPE_WEAPON )
		{
			if( pWeapon->GetSetItemID() == nSetItemID )
			{
				++nNumEquipedSetItem;
				m_nEquipeditemID = pWeapon->GetClassID();
			}
		}
	}
	return nNumEquipedSetItem; // 세트 아이템 풀파츠 갯수.
}

#endif


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
void CDnPlayerActor::UpdateTotalLevel(int nLevel)
{
	if (m_pTotalLevelSkillSystem)
	{
		m_pTotalLevelSkillSystem->SetTotalLevel(nLevel);
	}
}

void CDnPlayerActor::UpdateTotalLevelByCharLevel()
{
	if (m_pTotalLevelSkillSystem)
		m_pTotalLevelSkillSystem->UpdateTotalLevel();
}

void CDnPlayerActor::AddTotalLevelSkill(int nSlotIndex, int nSkillID, bool isInitialize/* = false*/)
{
	if (m_pTotalLevelSkillSystem)
	{
		DnSkillHandle hSkill = m_pTotalLevelSkillSystem->FindTotalLevelSkill(nSkillID);
		if (!hSkill)
		{
			RemoveTotalLevelSkill(nSlotIndex);
			return;
		}

		//만약 해당 슬롯이 활성화가 되지 않은 경우는 추가 하지 않도록 한다..
		bool isActivateSlot = m_pTotalLevelSkillSystem->IsActivateSlot(nSlotIndex);
		if (isActivateSlot == false)
			return;

		//PVE/PVP설정..
		// 스킬 레벨 데이터를 pve/pvp 인 경우와 나눠서 셋팅해준다.
		int iSkillLevelDataType = GetSkillTask().QueryPVEOrPVP();

		hSkill->SelectLevelDataType( iSkillLevelDataType );

		m_pTotalLevelSkillSystem->AddTotalLevelSkill(nSlotIndex, hSkill, isInitialize);
	}
}
void CDnPlayerActor::RemoveTotalLevelSkill(int nSlotIndex)
{
	if (m_pTotalLevelSkillSystem)
	{
		m_pTotalLevelSkillSystem->RemoveTotallevelSkill(nSlotIndex);
	}
}

void CDnPlayerActor::ActivateTotalLevelSkillSlot(int nSlotIndex, bool bActivate)
{
	if (m_pTotalLevelSkillSystem)
		m_pTotalLevelSkillSystem->ActivateTotalLevelSkillSlot(nSlotIndex, bActivate);
}

void CDnPlayerActor::ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate)
{
	if (m_pTotalLevelSkillSystem)
		m_pTotalLevelSkillSystem->ActivateTotalLevelSkillCashSlot(nSlotIndex, bActivate, tExpireDate);
}

void CDnPlayerActor::OnLevelChange()
{
	UpdateTotalLevelByCharLevel();
}

#endif // PRE_ADD_TOTAL_LEVEL_SKILL


#ifdef PRE_ADD_COSTUME_SKILL

void CDnPlayerActor::RefreshCostumeSkill( int nSkillIndex, int nSkillLevel )
{
	if( m_nCostumeSkillID == nSkillIndex )
		return;

	if( IsLocalActor() == true ) // LocalPlayerActor
	{
		CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstancePtr()->GetTask( "SkillTask" ) );
		if( !pSkillTask ) 
			return;

		if( m_nCostumeSkillID > 0 ) 
		{
			for(int i=0; i<DualSkill::Type::MAX; i++)
				pSkillTask->DelSkill( m_nCostumeSkillID, i );
		}

		if( nSkillIndex > 0 ) 
		{
			for(int i=0; i<DualSkill::Type::MAX; i++)
				pSkillTask->AddSkill( nSkillIndex, nSkillLevel, i );

			DnSkillHandle hSkill = FindSkill( nSkillIndex );
			if( !hSkill ) 
				return;

			if( hSkill->GetSkillType() != CDnSkill::SkillTypeEnum::Active )
			{
#if !defined(_FINAL_BUILD)
				MessageBox( NULL, L"CostumeSkill Failed By SkillType ( active skill type fail ) ", L"Error", MB_OK );
#endif
				for(int i=0; i<DualSkill::Type::MAX; i++)
					pSkillTask->DelSkill( nSkillIndex, i );
				
				return;
			}

			if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
				hSkill->OnBeginCoolTime();

			CDnCharStatusDlg * pCharStatusDlg = (CDnCharStatusDlg *)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
			if( pCharStatusDlg )
			{
				pCharStatusDlg->RefreshCustumeSkill( hSkill );
				GetInterface().GetMainBarDialog()->OnRefreshQuickSlot();
			}
		}
	}
	else // PlayerActor
	{
		if( m_nCostumeSkillID > 0 ) 
			RemoveSkill( m_nCostumeSkillID );

		if( nSkillIndex > 0 ) 
		{
			AddSkill( nSkillIndex , nSkillLevel );

			DnSkillHandle hSkill = FindSkill( nSkillIndex );
			if( !hSkill ) 
				return;

			if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
				hSkill->OnBeginCoolTime();
		}
	}

	m_nCostumeSkillID = nSkillIndex;
}
#endif // PRE_ADD_COSTUME_SKILL

void CDnPlayerActor::MakeComparePotentialItemState(CDnState& State, CDnItem* pCurrItemState, CDnItem* pNewItemState)
{
	State.ResetState();
	State = MakeComparePotentialState(pCurrItemState, pNewItemState);

	CDnState BUFF_STATE;
	BUFF_STATE.ResetState();

	if( m_pStateBlow )
	{
		int nSize = m_pStateBlow->GetNumStateBlow();
		for ( int i = 0 ; i < nSize ; i++ )
		{
			DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);
			if( hBlow && 
				CDnCreateBlow::IsBasicBlow( hBlow->GetBlowIndex() ) == true &&
				hBlow->GetParentSkillInfo() && 
				hBlow->GetParentSkillInfo()->eSkillType == CDnSkill::SkillTypeEnum::Passive && 
				hBlow->GetParentSkillInfo()->eDurationType == CDnSkill::DurationTypeEnum::Buff )
			{
				CDnBasicBlow *pBlow = static_cast<CDnBasicBlow*>(hBlow.GetPointer());
				if( pBlow )
				{
					CDnState *pState = const_cast<CDnState*>(pBlow->GetState());
					BUFF_STATE.MergeState( *pState, pState->GetValueType() );
				}
			}
		}
	}

	State.MergeState( BUFF_STATE, ValueTypeAbsolute );
	State.CalculateRatioValue( BUFF_STATE );
}

#ifdef PRE_ADD_DWC
void CDnPlayerActor::SetDWCTeamName(WCHAR* wszTeamName)
{
	if(wszTeamName == NULL)
		memset(m_wszDWCTeamName, 0, sizeof(m_wszDWCTeamName));
	else
		wcscpy(m_wszDWCTeamName, wszTeamName);
}
#endif