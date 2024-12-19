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
#include "PerfCheck.h"
#include "DnDropItem.h"
#include "GameSendPacket.h"
#include "DnMonsterActor.h"
#include "DnStateBlow.h"
#include "MAAiScript.h"
#include "DnBlow.h"
#include "DnItemTask.h"
#include "DNLogConnection.h"
#include "DNGameDataManager.h"
#include "ScoreSystem.h"
#include "DNMissionSystem.h"
#include "DNDBConnectionManager.h"
#include "DnPlayerSpeedHackChecker.h"
#include "DnPlayerDoNotEnterChecker.h"
#include "DnPlayerPickupChecker.h"
#include "DnPlayerSkillChecker.h"
#include "DnPlayerActionChecker.h"
#include "DnPlayAniProcess.h"
#include "DNPvPPlayerAggroSystem.h"
#include "DNMonsterAggroSystem.h"
#include "DnChangeActionSetBlow.h"
#include "DnChangeActionStrProcessor.h"
#include "DNDBConnection.h"
#include "DnChangeStandActionBlow.h"
#include "DnActionSpecificInfo.h"
#include "MasterRewardSystem.h"
#if defined( PRE_ADD_SECONDARY_SKILL )
#include "SecondarySkillRepositoryServer.h"
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
#include "DnCannonMonsterActor.h"
#include "DNPvPGameRoom.h"
#include "DnApplySEWhenTargetNormalHitProcessor.h"

#include "DnBubbleSystem.h"
#include "DnObserverEventMessage.h"
#include "DnBlockBlow.h"
#include "DnParryBlow.h"
#include "DnCooltimeParryBlow.h"
#include "PvPZombieMode.h"
#include "DNGameServerScriptAPI.h"
#include "DnOrderMySummonedMonsterBlow.h"
#include "IDnSkillUsableChecker.h"
#include "DnTransformBlow.h"
#include "DnBasicBlow.h"
#include "DnCreateBlow.h"

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
#include "DnDPSReporter.h"
#endif

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
#include "DnSkillTask.h"
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

float CDnPlayerActor::s_fRecoverySPTime = 5.f;

CDnPlayerActor::CDnPlayerActor( CMultiRoom *pRoom, int nClassID )
: CDnActor( pRoom, nClassID )
{
	m_pSession = NULL;

	CDnPlayerState::Initialize( m_nClassID );
	CDnActionBase::Initialize( this );
	MACP::Initialize( this );

	m_cMovePushKeyFlag = 0;
	m_bBattleMode = true;

	m_nWorldLevel = 0;

	m_nComboDelay = 0;
	m_nComboCount = 0;
	m_nTotalComboCount = 0;
	m_fRecoverySPDelta = 0.f;
	m_bCompleteCutScene = false;
	m_bCheckCompleteCutScene = false;
	m_bSkipCutScene = false;

	m_bGhost = false;
	m_uiStateBlowProcessAfterBit = 0;

	m_afLastEquipItemSkillDelayTime = 0.0f;
	m_afLastEquipItemSkillRemainTime = 0.0f;

	m_dwSyncDatumTick			= 0;
	m_dwSyncDatumSendTick		= 0;
	m_nVoiceChannelID = 0;
	m_pPlayerSpeedHackChecker	= new CDnPlayerSpeedHackChecker( this );
	m_pPlayerDoNotEnterChecker = new CDnPlayerDoNotEnterChecker( this );
	m_pPlayerPickupChecker = new CDnPlayerPickupChecker( this );
	m_pPlayerSkillChecker = new CDnPlayerSkillChecker( this );
	m_pPlayerActionChecker = new CDnPlayerActionChecker( this );

	memset( m_bCashSelfDeleteWeapon, 0, sizeof(m_bCashSelfDeleteWeapon) );
	memset( m_bWeaponViewOrder, 0, sizeof(m_bWeaponViewOrder) );

	m_iNowMaxProjectileCount = 0;
	m_iReservedProjectileCount = 0;

	m_LastEscapeTime = 0;

	m_dwLastBasicShootActionTime = 0;
	m_dwLastBasicShootCoolTime = 0;
	m_dwLastChargeShootTime = 0;
	m_bCheckProjectileSignalTerm = true;

	m_bUpdatedProjectileInfoFromCmdAction = false;
	m_fFrameSpeed = 1.0f;

	m_bPlayerCannonMode = false;

	m_nSwapSingleSkinActorID = -1;
	m_nMonsterMutationTableID = -1;

	m_hSwapOriginalHandle.Identity();
	m_pSwapOriginalAction = NULL;
	m_nDeathCount = 0;

	m_pBubbleSystem = new BubbleSystem::CDnBubbleSystem;
	this->RegisterObserver( m_pBubbleSystem );

	m_bTempSkillAdded = false;
	m_iTempChangedJob = 0;
	m_nInvalidPlayerCheckCounter = 0;

	m_bForceEnableRideByTrigger = true;

	m_nVehicleEffectIndex = 0;
	m_bVehicleMode = false;

	m_vecEventEffectList.clear();

	for( int i=0; i<Pet::Skill::Max; i++ )
		m_bDeletedPetSkill[i] = false;
	
	m_nAllowedSkill = 0;
	m_bRefreshTransformMode = false;
	m_bTransformMode = false;
	m_bShootMode = false;

#ifdef _USE_VOICECHAT
	//	m_nVoiceUpdateCount = 0;
	m_nVoiceRotate = 0;
#endif

	m_MixedActionTimeStamp = 0;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_pTotalLevelSkillSystem = NULL;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef PRE_ADD_COSTUME_SKILL
	m_nCostumeSkillID = 0;
#endif
	m_nLastUsedSkill = 0;
}

CDnPlayerActor::~CDnPlayerActor()
{
	// 대포 모드라면 대포 소유권 해제.
	if( IsCannonMode() )
	{
		static_cast<CDnCannonMonsterActor*>( m_hCannonMonsterActor.GetPointer() )->OnMasterPlayerActorDie();
	}

	//문장 스킬효과 리셋
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( pSox )
	{
		for( int itr = 0; itr < CDnGlyph::GlyphSlotEnum_Amount; ++itr )
		{
			if( m_hGlyph[itr] )
			{
				if( CDnGlyph::PassiveSkill == pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_GlyphType" )->GetInteger() )
				{
					int nSkillID = pSox->GetFieldFromLablePtr( m_hGlyph[itr]->GetClassID(), "_SkillID" )->GetInteger();
					DnSkillHandle hSkill = FindSkill( nSkillID );

					if( hSkill )
						hSkill->DelGlyphStateEffect( m_hGlyph[itr]->GetClassID() );
				}
			}
		}
	}

	// 오라 스킬 정리. 다른 버프류와는 다르게 오라는 시전했던 캐릭터가 죽으면 다른 캐릭터에게 적용되던 것들이 풀려야 한다.
	// 오라스킬, 토글스킬이 켜져 있다면 꺼준다.
	if( IsEnabledToggleSkill() )
		OnSkillToggle( m_hToggleSkill, false );

	if( IsEnabledAuraSkill() )
		OnSkillAura( m_hAuraSkill, false );

	for( int i=0; i<2; i++ ) SAFE_RELEASE_SPTR( m_hCashWeapon[i] );

	SAFE_RELEASE_SPTR( m_hSwapOriginalHandle );
	SAFE_DELETE( m_pSwapOriginalAction );

	SAFE_DELETE( m_pPlayerSkillChecker );
	SAFE_DELETE( m_pPlayerActionChecker );
	SAFE_DELETE( m_pPlayerDoNotEnterChecker ) ;
	SAFE_DELETE( m_pPlayerPickupChecker );
	SAFE_DELETE( m_pPlayerSpeedHackChecker );

	SAFE_DELETE( m_pBubbleSystem );

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	SAFE_DELETE(m_pTotalLevelSkillSystem);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}

MAMovementBase* CDnPlayerActor::CreateMovement()
{
	MAMovementBase* pMovement = new IBoostPoolMAWalkMovement();
	return pMovement;
}

bool CDnPlayerActor::Initialize()
{
	bool bResult = CDnActor::Initialize();

	m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
	m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );

	_ASSERT( m_pAggroSystem == NULL );
	if( GetGameRoom()->bIsPvPRoom() )
	{
		m_pAggroSystem = new CDNPvPPlayerAggroSystem( GetActorHandle() );
		_ASSERT( m_pAggroSystem != NULL );
	}

	m_mapTumbleHelper.insert( make_pair(GetElementIndex("Tumble_Front"), GetElementIndex("Move_Front")) );
	m_mapTumbleHelper.insert( make_pair(GetElementIndex("Tumble_Back"), GetElementIndex("Move_Back")) );
	m_mapTumbleHelper.insert( make_pair(GetElementIndex("Tumble_Left"), GetElementIndex("Move_Left")) );
	m_mapTumbleHelper.insert( make_pair(GetElementIndex("Tumble_Right"), GetElementIndex("Move_Right")) );

	// 소서리스 차지 미사일도 액션툴과 액션시스템만 갖고 구현되어있지 않고 클라에서 처리하는 부분이 있으므로 서버도 맞춰줄 수 밖에 없다.
	// 시스템과는 약간 다르게 돌아가므로 마찬가지로 액션을 이어주고 액션의 쿨타임은 차지시간으로 설정되어있는 1.5초로 한다.
	// 대표로 Move_Front 로만 셋팅하도록 해도 된다. 핵 체크에서 예외로 확인되기만 하면 된다..
	m_mapTumbleHelper.insert( make_pair(GetElementIndex("ChargeShoot_Stand"), GetElementIndex("Charge_Front")) );

	m_pBubbleSystem->Initialize( GetActorHandle() );

	m_mapIcyFractionHitted.clear();

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	m_FrameSkipCallSkillProcess.SetFramePerSec( 10.0f );
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	m_pTotalLevelSkillSystem = new CDnTotalLevelSkillSystem(GetMySmartPtr());
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	return bResult;
}


void CDnPlayerActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	EtVector3 vPrevPos = m_Cross.m_vPosition;

	CDnActor::Process( LocalTime, fDelta );
	PROFILE_TIME_TEST_BLOCK_START( "CDnPlayerActor::Process" );

	m_pBubbleSystem->Process( LocalTime, fDelta );

	for( int i=0; i<2; i++ ) 
	{
		if( m_hCashWeapon[i] ) 
			m_hCashWeapon[i]->Process( LocalTime, fDelta );
	}

	MAPartsBody::PreProcess( LocalTime, fDelta );

	if( 0 < GetCantXZMoveSEReferenceCount() )
		m_vAniDistance.x = m_vAniDistance.z = 0.0f;

#ifdef PRE_ADD_MONSTER_CATCH
	if(!IsVehicleMode() && !m_hCatcherMonster )
#else
	if(!IsVehicleMode())
#endif // #ifdef PRE_ADD_MONSTER_CATCH
		m_pMovement->Process( LocalTime, fDelta );

	MAPartsBody::Process( m_Cross, LocalTime, fDelta );

	ProcessCombo( LocalTime, fDelta );
	ProcessRecoverySP( LocalTime, fDelta );

	m_pPlayerDoNotEnterChecker->Process( LocalTime, fDelta );
	m_pPlayerSpeedHackChecker->Process( LocalTime, fDelta );
	m_pPlayerPickupChecker->Process( LocalTime, fDelta );
	m_pPlayerSkillChecker->Process( LocalTime, fDelta );
	m_pPlayerActionChecker->Process( LocalTime, fDelta );

	ProcessCompanion( LocalTime, fDelta );
	ProcessNonLocalShootModeAction();
	if(m_bRefreshTransformMode)
		RefreshTransformMode();

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (m_pTotalLevelSkillSystem)
		m_pTotalLevelSkillSystem->Process(LocalTime, fDelta);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	PROFILE_TIME_TEST_BLOCK_END();
}

void CDnPlayerActor::CmdPickupItem( PickupItemStruct* pStruct, DnDropItemHandle hDropItem/*=CDnDropItem::Identity()*/ )
{
#if defined(_CH)
	if (m_pSession->GetFCMState() != FCMSTATE_NONE){	// 3시간 이상 게임하면 아이템 줏기 못함 090624
		// m_pSession->SendPickUp(ERROR_FCMSTATE, -1, NULL, 0);
		return;
	}
#endif

	// 옵져버는 PickUp 못함
	if( GetActorHandle() && GetActorHandle()->bIsObserver() )
		return;
	// 운영자난입도 PickUp 못함
	if( m_pSession && m_pSession->bIsGMTrace() )
		return;

	if( !CDnDropItem::IsActive(GetRoom()) ) 
		return;

	DnDropItemHandle hResult;
	if( hDropItem )
		hResult = hDropItem;
	else
	{
		_DANGER_POINT();
		return;
	}

	if( hResult ) {
		if (hResult->IsReversionItem() && hResult->IsReversionLocked() == false)
		{
			DnActorHandle hBeneficiery;
			if (hResult->GetOwnerUniqueID() != -1)
			{
				hBeneficiery = CDnActor::FindActorFromUniqueID( GetRoom(), hResult->GetOwnerUniqueID() );
				if (hBeneficiery)
					CDnItemTask::GetInstance(GetRoom()).PickUpItem( hBeneficiery, hResult, ITEMLOOTRULE_NONE );
				return;
			}
			CDnItemTask::GetInstance(GetRoom()).PickUpItem( GetMySmartPtr(), hResult, ITEMLOOTRULE_NONE );
			return;
		}
		if( hResult->GetOwnerUniqueID() != -1 ) {
			DnActorHandle hOwner = CDnActor::FindActorFromUniqueID( GetRoom(), hResult->GetOwnerUniqueID() );
			if( hOwner ) {
				CDnItemTask::GetInstance(GetRoom()).PickUpItem( hOwner, hResult, ITEMLOOTRULE_NONE );
			}
			return;
		}
		else {
			eItemRank Rank = CDnItem::GetItemRank( hResult->GetItemID() );
			eItemTypeEnum Type = CDnItem::GetItemType( hResult->GetItemID() );
			TPARTYITEMLOOTRULE LootType = ITEMLOOTRULE_NONE;
			TITEMRANK LootRank = m_pRoom->GetPartyItemLootRank();

			switch( Type ) 
			{
				case ITEMTYPE_INSTANT:
					LootType = ITEMLOOTRULE_NONE;
					break;
				default:
					if (LootRank == ITEMRANK_NONE )
						LootType = m_pRoom->GetPartyItemLootRule();
					else
						LootType = ( Rank >= LootRank ) ? ITEMLOOTRULE_RANDOM : m_pRoom->GetPartyItemLootRule();
					break;
			}
			// 돈일경우도 그냥 노말로 획득합니다.
			if( hDropItem->GetItemID() == 0 ) 
				LootType = ITEMLOOTRULE_NONE;

			TItemData *pItemData = g_pDataManager->GetItemData(hDropItem->GetItemID());
			if (pItemData)
			{
				if (pItemData->cReversion == ITEMREVERSION_BELONG)
					LootType = ITEMLOOTRULE_NONE;
			}

			// 구울모드에서는 강제로 LootType 설정
			if( GetRoom() && static_cast<CDNGameRoom*>(GetRoom())->bIsZombieMode() )
				LootType = ITEMLOOTRULE_NONE;

			switch( LootType ) 
			{
				case ITEMLOOTRULE_NONE:
				case ITEMLOOTRULE_OWNER:
					CDnItemTask::GetInstance(GetRoom()).PickUpItem( GetMySmartPtr(), hResult, LootType );
					break;
				case ITEMLOOTRULE_RANDOM:
				{
					int nLiveCount = CDnPartyTask::GetInstance(GetRoom()).GetPartyUserCount(CDNGameRoom::ePICKUPITEM);
					if( nLiveCount == 0 ) return;
					int nResultOffset = _rand(GetRoom())%nLiveCount;
					int nOffset = 0;
					DnActorHandle hResultActor;
					int nPartyCount = CDnPartyTask::GetInstance(GetRoom()).GetUserCount();
					for( int i=0; i<nPartyCount; i++ ) {
						CDNUserSession *pSession = CDnPartyTask::GetInstance(GetRoom()).GetUserData(i);
						if( !pSession ) continue;
						if( pSession->bIsGMTrace() ) continue;
#if defined _CH
						if (pSession->GetFCMState() != FCMSTATE_NONE) continue;
#endif
						DnActorHandle hActor = pSession->GetActorHandle();
						if( !hActor || hActor->IsDie() ) continue;
						if( nOffset == nResultOffset ) {
							hResultActor = hActor;
							break;
						}
						nOffset++;
					}
					if( hResultActor ) 
						CDnItemTask::GetInstance(GetRoom()).PickUpItem( hResultActor, hResult, LootType );
				}
				break;
				case ITEMLOOTRULE_LEADER:
				{
					int nPartyCount = CDnPartyTask::GetInstance(GetRoom()).GetUserCount();
					DnActorHandle hResultActor;
					for( int i=0; i<nPartyCount; i++ ) {
						CDNGameRoom::PartyStruct *pPartyStruct = CDnPartyTask::GetInstance(GetRoom()).GetPartyData(i);
						if( !pPartyStruct ) continue;
						if( pPartyStruct->bLeader ) {
								hResultActor = ( pPartyStruct->pSession ) ? pPartyStruct->pSession->GetActorHandle() : CDnActor::Identity();
						}
					}
					if( hResultActor ) 
						CDnItemTask::GetInstance(GetRoom()).PickUpItem( hResultActor, hResult, LootType );
				}
				break;
				case ITEMLOOTRULE_INORDER:
				{
					CDNUserSession *pSession = CDnPartyTask::GetInstance(GetRoom()).GetUserData(m_pRoom->GetCurrentItemLooterIdx());
					if (pSession)
					{
						DnActorHandle hActor = pSession->GetActorHandle();
						CDnItemTask::GetInstance(GetRoom()).PickUpItem( hActor, hResult, LootType );
					}
				}
				break;
			}
		}
	}
}

void CDnPlayerActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Hit:
			{
			}
			break;
		case STE_Jump:
			{
				JumpStruct *pStruct = (JumpStruct *)pPtr;
				EtVector2 vVec( 0.f, 0.f );

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

				// 점프 동기화 [2010/11/09 semozz]
				// 점프중 재 점프가 될때 기존 Velocity값을 리셋해야 하는경우
				// 클라이언트와 같이 리셋이 되어야 점프시간 동기화가 맞아진다.
				if( pStruct->bResetPrevVelocity ) {
 					SetVelocityY( 0.f );
					SetResistanceY( 0.f );
				}
				
				Jump( pStruct->fJumpVelocity, vVec );
				SetResistanceY( pStruct->fJumpResistance );
			}
			return;
		case STE_CustomAction:
			{
				CustomActionStruct *pStruct = (CustomActionStruct *)pPtr;

				ResetCustomAction();
				SetCustomAction( pStruct->szChangeAction, (float)pStruct->nChangeActionFrame );
			}
			return;
		case STE_PickupItem:
			{
			}
			return;
		case STE_RebirthAnyPlayer:
			{

				// [2011/02/11 semozz]
				// 씨드래곤맵에서는 파티 부활 방지..
				CMultiRoom *pRoom = GetRoom();
				if (!pRoom)
					break;

				eDragonNestType _dragonNestType = CDnWorld::GetInstance(pRoom).GetDragonNestType();

				RebirthAnyPlayerStruct *pStruct = (RebirthAnyPlayerStruct*)pPtr;
				CDNUserSession *pSession = NULL;
				float fMinDist = FLT_MAX;

				for( DWORD i=0; i<CDnPartyTask::GetInstance(GetRoom()).GetUserCount(); i++ ) {
					CDNGameRoom::PartyStruct *pParty = CDnPartyTask::GetInstance(GetRoom()).GetPartyData(i);
					if( !pParty || !pParty->pSession ) continue;
					if( pParty->pSession == m_pSession ) continue;
					if(  pParty->pSession->GetState() != SESSION_STATE_GAME_PLAY)
						continue;
					DnActorHandle hActor = pParty->pSession->GetActorHandle();
					if( !hActor ) continue;
					CDnPlayerActor *pPlayer = (CDnPlayerActor *)hActor.GetPointer();
					if( !pPlayer->IsDie() || !pPlayer->IsGhost() ) continue;					
					if( pParty->nUsableRebirthCoin == 0 ) continue;

					EtVector3 vVec = *GetPosition() - *hActor->GetPosition();
					float fDist = EtVec3LengthSq( &vVec );
					if( fDist < fMinDist ) {
						fMinDist = fDist;
						pSession = pParty->pSession;
					}
				}
				if( !pSession || fMinDist > (float)( pStruct->nRadius * pStruct->nRadius ) ) break;

				if( m_pSession->GetStatusData()->wCashRebirthCoin <= 0 )
				{
					GetUserSession()->SendRebirthCoin(ERROR_ITEM_REBIRTH_CASHCOIN_SHORT_FAIL, m_pPartyData->nUsableRebirthCoin, _REBIRTH_SELF, GetUserSession()->GetSessionID());
					break;
				}

				CDnItemTask::GetInstance(GetRoom()).RequestRebirthCoinUseAnyPlayer( m_pSession, pSession );
				UpdateRebirthPlayer();
			}
			break;

		case STE_CancelChangeStandActionSE:
			{
				CDnChangeStandActionBlow::ReleaseStandChangeSkill( GetActorHandle(), false );
			}
			break;
		case STE_CancelChangeStandEndActionSE:
			{
				CancelChangeStandEndActionSEStruct* pStruct = (CancelChangeStandEndActionSEStruct*)pPtr;
				CDnChangeStandActionBlow::ReleaseStandChangeSkill( GetActorHandle(), false, pStruct->szEndAction );
			}
			break;
		case STE_TriggerEvent:
			{
				TriggerEventStruct *pStruct = (TriggerEventStruct *)pPtr;
				CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "EventArea", -1 );
				CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "ActorHandle", GetSessionID() );
				CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "EventID", pStruct->nEventID );
				CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnActor::TriggerEvent", LocalTime, 0.f );
			}
			break;

		case STE_ShootCannon:
			{
				ShootCannonStruct* pStruct = (ShootCannonStruct*)pPtr;
				if( IsCannonMode() )
				{
					// 클라이언트에서 CS_CANNONROTATESYNC 패킷으로 보내준 TargetPosition 으로 발사체를 셋팅한다.
					m_hCannonMonsterActor->UseSkill( pStruct->CannonMonsterSkillID );
				}
			}
			break;

			// #29925 이 시그널에 지정된 상태효과가 없으면 지정된 액션을 실행.
		case STE_ChangeActionSECheck:
			{
				ChangeActionSECheckStruct* pStruct = (ChangeActionSECheckStruct*)pPtr;
				if( false == IsAppliedThisStateBlow( (STATE_BLOW::emBLOW_INDEX)pStruct->nStateEffectID ) )
				{
					SetActionQueue( pStruct->szChangeAction );
				}
			}
			break;

			/*
			// Note: 패킷보다 시그널이 늦게 처리되어 클라에서 발동된 액션 중 스킬이 씹히는 경우가 있으므로 
			// 클라쪽에서 패킷으로 보내도록 수정합니다..
		case STE_CanUseSkill:
			{
				CanUseSkillStruct* pStruct = (CanUseSkillStruct*)pPtr;
				m_bUseSignalSkillCheck = (pStruct->bUseSignalSkillCheck ? TRUE : FALSE);
				SetSignalSkillCheck( pStruct->CheckType, m_bUseSignalSkillCheck );
			}
			break;
			*/

		case STE_OrderMySummonedMonster:
			{
				OrderMySummonedMonsterStruct* pStruct = (OrderMySummonedMonsterStruct*)pPtr;
				OrderUseSkillToMySummonedMonster(pStruct);

			}
			break;
	}
	CDnActor::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

void CDnPlayerActor::OnDrop( float fCurVelocity )
{
	if( IsAir() ) {
		if( !IsHit() ) {

			//////////////////////////////////////////////////////////////////////////
			// #32977 - 탑 스피닝 스킬을 시작 해놓은 상태에서 탑 스피닝 액션의 시그널이 처리 되기전
			// OnDrop이 호출 되면 탑 스피닝에 있는 STE_Jump가 처리가 되지 않아서 동작이 이어지지 않게 된다.
			// 그래서 일단..
			// 현재 Frame이 0이고, STE_Jump시그널이 0프레임에 있으면 _Landing동작으로 변경을 막고
			// 현재 동작을 계속 유지 하도록 한다.
			//////////////////////////////////////////////////////////////////////////
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
			//////////////////////////////////////////////////////////////////////////

			char szStr[64];
			sprintf_s( szStr, "%s_Landing", GetCurrentAction() );
			if( IsExistAction( szStr ) )
			{
				SetActionQueue( szStr, 0, 2.f, 0.f, true, false );

				// 스킬 사용중일땐 체인액션 셋팅해준다.
				if( m_hProcessSkill )
				{
					m_hProcessSkill->AddUseActionName( szStr );
					m_hProcessSkill->OnChainInput( szStr );
				}
			}
			else {// 만약에 없을경우에 하늘에서 병신짓하구있어서 넣어놉니다. 일단은 마춰서 넣어주는거임
				if( GetVelocity()->y != 0.f )
					SetActionQueue( "Stand", 0, 0.f, 0.f, true, false );
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
		! IsFloorCollision() )
	{
		// 움직여지는 각도도 체크해서 계단등을 내려올때 떨어져보이는것 보정해보아요.
		EtVector3 vDir = *GetPosition() - *GetPrevPosition();
		EtVec3Normalize( &vDir, &vDir );
		float fDot = EtVec3Dot( &EtVector3( 0.f, 1.f, 0.f ), &vDir );
		float fAngle = EtToDegree( EtAcos( fDot ) );

		if( fCurVelocity < -5.f && fAngle > 155.f ) {
			ActionElementStruct *pStruct = GetElement( "Jump" );
			if( pStruct ) {
				SetActionQueue( "Jump", 0, 6.f, (float)pStruct->dwLength / 2.f );
			}
		}
	}
}

void CDnPlayerActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{

#if defined( PRE_ADD_SECONDARY_SKILL )
	if( GetUserSession() && GetUserSession()->GetSecondarySkillRepository() )
		static_cast<CSecondarySkillRepositoryServer*>(GetUserSession()->GetSecondarySkillRepository())->CancelManufacture();
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	if( IsCannonMode() )
	{
		HitParam.szActionName.clear();
		HitParam.vViewVec = *GetLookDir();
		HitParam.vResistance = EtVector3( 0.0f, 0.0f, 0.0f );
		HitParam.vVelocity = EtVector3( 0.0f, 0.0f, 0.0f );
	}

	int nSeed = CRandom::Seed(GetRoom());
	_srand( GetRoom(), nSeed );
	
	INT64 nTemp = GetHP();
	CDnActor::OnDamage( pHitter, HitParam, pHitStruct );
	INT64 nDamage = nTemp - GetHP();

	INT64 biKillerCharDBID = 0;
	if( GetGameRoom() )
	{
		DnActorHandle hHitter = pHitter ? pHitter->GetActorHandle() : CDnActor::Identity();
		GetGameRoom()->OnDamage( GetActorHandle(), hHitter, nDamage );	

		if( GetGameRoom()->GetGameTask() )
			GetGameRoom()->GetGameTask()->OnDamage( GetActorHandle(), hHitter, nDamage );

		if (IsDie() && hHitter && hHitter->IsPlayerActor()){
			CDnPlayerActor *pPlayer = (CDnPlayerActor *)hHitter.GetPointer();
			biKillerCharDBID = pPlayer->GetUserSession()->GetCharacterDBID();
		}
	}

	if( m_HitParam.bSuccessNormalDamage ) 
	{
		m_nComboDelay = 0;
		m_nComboCount = 0;
	}

	UpdateAttackedCPPoint( pHitter, m_HitParam.HitType );

	ResetCustomAction();
	RequestDamage( pHitter, nSeed, nDamage );

	switch( pHitter->GetDamageObjectType() ) 
	{
		case DamageObjectTypeEnum::Actor:
			{
				DnActorHandle hActor = pHitter->GetActorHandle();

				if( m_pAggroSystem )
					m_pAggroSystem->OnDamageAggro( hActor, HitParam, (int)nDamage );
			}
			break;
	}
}

void CDnPlayerActor::CmdMove( EtVector3 &vPos, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	MovePos( vPos, false );

	// #31940 패링은 상태효과가 돌면서 패링액션을 나가게 하는 것이므로 움직이면서 피격되어 패링이 나올 경우
	// 클라에서 계속 방향키를 누르고 있어서 이동 패킷이 오는경우 CS_CMDMOVE 패킷이 와서 여기서 Move_ 시리즈로 로 액션이 바뀌므로 예외처리.
	if( m_szAction == "Skill_Parrying" ||
		m_szActionQueue == "Skill_Parrying" )
		return;

	SetActionQueue( szActionName, nLoopCount, fBlendFrame );
}

void CDnPlayerActor::CmdStop( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame )
{
	MAMovementBase *pMovement = GetMovement();
	if( !pMovement ) return;

	pMovement->ResetMove();

	// #31940 패링은 상태효과가 돌면서 패링액션을 나가게 하는 것이므로 움직이면서 피격되어 패링이 나올 경우
	// 곧바로 캐릭터가 멈추면서 CS_CMDSTOP 패킷이 와서 여기서 Stand 로 액션이 바뀌므로 예외처리.
	if( m_szAction == "Skill_Parrying" ||
		m_szActionQueue == "Skill_Parrying" )
		return;

	SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame );
}

void CDnPlayerActor::CmdPassiveSkillAction( int nSkillID, const char *szActionName, int nLoopCount /*= 0*/, float fBlendFrame /*= 3.f*/, float fStartFrame /*= 0.0f*/, bool bChargeKey /*= false*/, bool bCheckOverlapAction /*= true */, bool bOnlyCheck/* = false*/ )
{
	// Note: 행동불가인 경우 패시브 스킬 나갈 수 없음.
	// SetAction() 함수에서 액션이 막히기 때문에 어차피 안나가지만 스킬 사용으로 쿨타임이 돌아가므로 시그널 처리를 막는다.
	if( GetCantActionSEReferenceCount() > 0 ) return;

	if( false == bOnlyCheck )
	{
		DnSkillHandle hSkill = FindSkill( nSkillID );
		if( !hSkill ) return;

		// Note: 발차기 같은 경우 같은 스킬이 발동 중에 끝 부분에 또 발동 되도록 액션툴에 설정되어있어서 
		//		 같은 스킬일지리도 끊기고 나가도록 합니다.
		if( m_hProcessSkill /*&& m_hProcessSkill != hSkill*/ ) 
		{
			if( false == (IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn()) )
			{
				m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
			}

			m_hProcessSkill.Identity();
		}

		// 서버쪽에서도 누르고 있는 키기 떄문에 액션 바뀌기 전엔 스킬이 계속 실행되는 것으로 처리.
		if( bChargeKey )
		{
			hSkill->SetPassiveSkillLength( -1.0f );
		}
		else
		{
			// 패시브 스킬 액션에 next 액션까지 있는 경우 감안.
			// next 액션은 클라이언트쪽에서도 하나만 기준 잡고 있기 때문에 서버에서도 하나만 기준 잡는다.
			DWORD dwActionLength = 0;
			ActionElementStruct* pElement = GetElement( szActionName );
			if( pElement )
			{
				if( pElement->dwLength <= (DWORD)fStartFrame )
					fStartFrame = pElement->dwLength*1.f;

				dwActionLength = pElement->dwLength - (DWORD)fStartFrame;
			}
			else
			{
				fStartFrame = 0.0f;
			}

			// #25042 Stand 액션이 next 액션인 경우 패시브 스킬의 액션이 아니므로 시간을 포함시키지 않는다.
			if( pElement->szNextActionName != "Stand" &&
				0 == strstr( pElement->szNextActionName.c_str(), "_Landing") )
			{
				ActionElementStruct* pNextElement = GetElement( pElement->szNextActionName.c_str() );
				if( pNextElement )
					dwActionLength += pNextElement->dwLength;
			}
			
			hSkill->SetPassiveSkillLength( (float)dwActionLength / s_fDefaultFps );

			// Active Type의 스킬(이글스 디센트) 같은 경우 Passive Type으로 스킬을 사용 할 경우도 있다.
			// 추후에 _CheckActionWithProcessPassiveActionSkill 에서 ActionLength가 진행 중인지 체크하게 되면
			// Passive Type 일 경우 ChaningPassiveSkill을 true로 만드는데 Active Type이라서 false 이다.
			// 그래서 여기서 ChaningPassiveKill을 true로 만들어 준다.
			if( hSkill->GetSkillType() == CDnSkill::Active ) // Active 일때만 설정해 줍니다.
				hSkill->SetChaningPassiveSkill( true );
		}

		// 액티브 스킬이 InputHasPassiveSkill 로 패킷이 왓을 경우 UsableCheck 를 무시해야 한다.
		if( CDnSkill::Active == hSkill->GetSkillType() )
		{
			m_bUseSignalSkillCheck = true;
			for( int i = 0; i < 3; ++i )
				SetSignalSkillCheck( i, true );
		}

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
		// 서버의 쿨타임 오차를 감안해서 0.5초의 여유를 두고 있지만,
		// 존이동할때나 기타 0.5초 이상 클라이언트와 벌어지는 다른 새로운 경우들이 생길 수 있으므로
		// 최종 스킬 사용한 타임 스탬프를 찍어두어 데이터에 지정된 스킬의 쿨타임보타 간격이 크다면 
		// 서버의 스킬 객체에 저장되어있는 쿨타임을 초기화 시켜주도록 한다. (#19737)
		hSkill->UpdateSkillCoolTimeExactly();
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

		CDnSkill::UsingResult eResult = CanExecuteSkill( hSkill );
		if( eResult == CDnSkill::UsingResult::Success )
		{
			//
			m_nLastUsedSkill = hSkill->GetClassID();
			//
			ExecuteSkill( hSkill, CDnActionBase::m_LocalTime, 0.f );
			hSkill->FromInputHasPassive();
			SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, bCheckOverlapAction );
		}
		else {
			((CDnPlayerSkillChecker*)m_pPlayerSkillChecker)->OnInvalidUseSkill( nSkillID, eResult );
		}

		// 액티브 스킬이 InputHasPassiveSkill 로 패킷이 왓을 경우 UsableCheck 를 무시한 것 다시 복구.
		if( CDnSkill::Active == hSkill->GetSkillType() )
		{
			m_bUseSignalSkillCheck = false;
			for( int i = 0; i < 3; ++i )
				SetSignalSkillCheck( i, false );
		}

		// 플레이어가 사용하는 패시브/즉시 스킬을 위해 액션 이름 기입해 줌.
		hSkill->SetPassiveSkillActionName( szActionName );
	}
	else
		SetActionQueue( szActionName, nLoopCount, fBlendFrame, fStartFrame, bCheckOverlapAction );

	m_fDownDelta = 0.f;
}

// 이쪽 패킷에서 구조체가 변경될 경우 PvP 난입시 코드도 수정이 필요하니 저에게(김밥) 알려주세요.
int CDnPlayerActor::CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit/* = false */, bool bCheckCanBegin/* = true*/ , bool bEternity /*= false*/ )
{
	if( emBlowIndex < STATE_BLOW::BLOW_NONE || emBlowIndex >= STATE_BLOW::BLOW_MAX )
	{
		if( pParentSkill )
			g_Log.Log(LogType::_ERROR, L"[CDnActor::CmdAddStateEffect] SkillID:%d, STATE_BLOW:%d\r\n", pParentSkill->iSkillID, emBlowIndex );
		else
			g_Log.Log(LogType::_ERROR, L"[CDnActor::CmdAddStateEffect] STATE_BLOW:%d\r\n", emBlowIndex);
		return -1;
	}

	int iID = CDnActor::CmdAddStateEffect( pParentSkill, emBlowIndex, nDurationTime, szParam, bOnPlayerInit, bCheckCanBegin , bEternity );
	if( -1 == iID ) 
		return -1;

	DnBlowHandle hAddedBlow = m_pStateBlow->GetStateBlowFromID( iID );

	const CPacketCompressStream* pPacket = hAddedBlow->GetPacketStream( szParam, bOnPlayerInit );
	Send( eActor::SC_CMDADDSTATEEFFECT, const_cast<CPacketCompressStream*>(pPacket) );

	if( GetGameRoom() )
		GetGameRoom()->OnCmdAddStateEffect( hAddedBlow->GetParentSkillInfo() );

	CheckAndRegisterObserverStateBlow( hAddedBlow );

	return iID;
}

void CDnPlayerActor::CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bRemoveFromServerToo /*= true */ )
{	
	if( m_pStateBlow->IsApplied( emBlowIndex ) )
	{
		if( bRemoveFromServerToo )
			CDnActor::CmdRemoveStateEffect( emBlowIndex );	

		SendRemoveStateEffect( emBlowIndex );
	}
}

void CDnPlayerActor::SendRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	BYTE pBuffer[32];
	CPacketCompressStream Stream( pBuffer, 32 );
	Stream.Write( &emBlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );

	Send( eActor::SC_CMDREMOVESTATEEFFECT, &Stream );
}

void CDnPlayerActor::SendRemoveStateEffectGraphic( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &emBlowIndex, sizeof(int) );

	Send( eActor::SC_BLOW_GRAPHIC_ERASE, &Stream );
}

void CDnPlayerActor::CmdToggleBattle( bool bBattleMode )
{
	if( bBattleMode == true ) 
	{
		if( !m_hWeapon[0] ) {
			m_bBattleMode = false;
			return;
		}
	}
	if( bBattleMode != m_bBattleMode ) 
	{
		SetBattleMode( bBattleMode );

		bool bSkipAction = false;

#if defined(PRE_ADD_50907)
		bool bSkipChangeAction = IsSkipChangeWeaponAction();
		if (bSkipChangeAction == true)
			return;
#endif // PRE_ADD_50907
		if(m_bShootMode)
		{
			if( m_bBattleMode == true ) SetActionQueue( "MOD_PullOut_Weapon" );
			else SetActionQueue( "MOD_PutIn_Weapon" ); 
			
			// ShootMode는 전투상태에만 사용하기때문에 Normal상태에서 무기를 꺼내는 행동은 GetchangeShootaction 에서 설정하지 못하기때문에 여기서 넣는다.
			bSkipAction = true;
		}
		if(!bSkipAction)
		{
			if( !IsDie() ) 
			{
				if( m_bBattleMode == true ) SetActionQueue( "PullOut_Weapon" );
				else SetActionQueue( "PutIn_Weapon" );
			}
		}
	}
}

void CDnPlayerActor::CmdAddExperience( TExpData &ExpData, int nLogCode, INT64 biFKey )
{
	int nExp = ExpData.nExperience;
	int nEventExp = ExpData.nEventExperience;
	int nPCBangExp = ExpData.nPcBangExperience;
	int nVIPExp = ExpData.nVIPExperience;
	int nPromoExp = ExpData.nPromotionExperience;

	TPlayerCommonLevelTableInfo* pPlayerCommonLevelTableInfo = g_pDataManager->GetPlayerCommonLevelTable(GetLevel());
	if( pPlayerCommonLevelTableInfo )
	{
		//여기서 1.5배 증가(한군데에서 해주는게 관리가 용이할듯....dyss)
		nExp = (int)(nExp * pPlayerCommonLevelTableInfo->fAddGainExp);
		nEventExp = (int)(nEventExp * pPlayerCommonLevelTableInfo->fAddGainExp);
		nPCBangExp = (int)(nPCBangExp * pPlayerCommonLevelTableInfo->fAddGainExp);
		nVIPExp = (int)(nVIPExp * pPlayerCommonLevelTableInfo->fAddGainExp);
		nPromoExp = (int)(nPromoExp * pPlayerCommonLevelTableInfo->fAddGainExp);
	}

#if defined( PRE_USA_FATIGUE )
	int iPwrExp = 0;
	if( nLogCode == DBDNWorldDef::CharacterExpChangeCode::DungeonMonster )
	{
		nLogCode = DBDNWorldDef::CharacterExpChangeCode::Dungeon;

		if( m_pSession && m_pSession->bIsNoFatigueEnter() == true )
		{
			int iTemp = nExp+ExpData.nItemExperience;
			nExp = (iTemp*g_pDataManager->GetNoFatigueExpRate())/100;

#if defined( _WORK )
			char szBuf[MAX_PATH];
			sprintf_s( szBuf, "북미피로도적용 Exp%d->%d", iTemp, nExp );
			std::cout << szBuf << std::endl;
#endif // #if defined( _WORK )
		}
		else
		{
			iPwrExp = ((nExp+ExpData.nItemExperience)*(g_pDataManager->GetFatigueExpRate()-g_pDataManager->GetNoFatigueExpRate()))/100;
			int iTemp = nExp+ExpData.nItemExperience;
			nExp = (iTemp*g_pDataManager->GetFatigueExpRate())/100;
#if defined( _WORK )
			char szBuf[MAX_PATH];
			sprintf_s( szBuf, "북미피로도적용 Exp%d->%d", iTemp, nExp );
			std::cout << szBuf << std::endl;
#endif // #if defined( _WORK )
		}
	}
#endif // #if defined( PRE_USA_FATIGUE )

#if defined(_CH)
	if (m_pSession->GetFCMState() == FCMSTATE_HALF){
		nExp = ExpData.nExperience / 2;
		nEventExp = (ExpData.nEventExperience > 0) ? ExpData.nEventExperience / 2 : 0;
		nPCBangExp = (ExpData.nPcBangExperience > 0) ? ExpData.nPcBangExperience / 2 : 0;
		nVIPExp = (ExpData.nVIPExperience > 0) ? ExpData.nVIPExperience / 2 : 0;
		nPromoExp = (ExpData.nPromotionExperience > 0) ? ExpData.nPromotionExperience / 2 : 0;
		ExpData.nItemExperience = (ExpData.nItemExperience>0) ? ExpData.nItemExperience/2 : 0;
		ExpData.nGuildExp = (ExpData.nGuildExp>0) ? ExpData.nGuildExp/2 : 0;
	}
	else if (m_pSession->GetFCMState() == FCMSTATE_ZERO){
		nExp = 0;
		nEventExp = 0;
		nPCBangExp = 0;
		nVIPExp = 0;
		nPromoExp = 0;
		ExpData.nItemExperience = 0;
		ExpData.nGuildExp = 0;
	}
#endif	// _CH

	nExp += ExpData.nItemExperience;
	nExp += ExpData.nGuildExp;

#if defined(PRE_ADD_WEEKLYEVENT)
	if (CDnWorld::GetInstance(GetRoom()).GetMapType() != EWorldEnum::MapSubTypeNest){
		int nThreadID = GetGameRoom()->GetServerID();

		float fEventValue = g_pDataManager->GetWeeklyEventValuef(WeeklyEvent::Player, GetClassID(), WeeklyEvent::Event_5, nThreadID);
		if (fEventValue != 0)
			nExp += (int)(nExp * fEventValue);
	}
#endif	// #if defined(PRE_ADD_WEEKLYEVENT)

	int nPrevLevel = GetLevel();
	int nTotalExp = nExp + nEventExp + nPCBangExp + nVIPExp + nPromoExp;
	AddExperience( nTotalExp, nLogCode, biFKey );		//_KR이 아니면 pcbangexp는 항상 0이다.

	// 레벨업이 되버리면 구지 보낼필요 없지 말입니다.
	if( GetLevel() == nPrevLevel ) {
		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		Stream.Write( &m_nExperience, sizeof(int) );
		Stream.Write( &nExp, sizeof(int) );		
		Stream.Write( &nEventExp, sizeof(int) );
		Stream.Write( &nPCBangExp, sizeof(int) );
#if defined(PRE_ADD_VIP)
		Stream.Write( &nVIPExp, sizeof(int) );
#endif	// #if defined(PRE_ADD_VIP)
		Stream.Write( &nPromoExp, sizeof(int) );
#if defined( PRE_USA_FATIGUE )
		Stream.Write( &iPwrExp, sizeof(int) );
#endif // #if defined( PRE_USA_FATIGUE )

		Send( eActor::SC_ADDEXP, GetMySmartPtr(), &Stream );
	}
}

void CDnPlayerActor::CmdAddCoin( INT64 nCoin, int nLogCode, int nFKey, bool bSync )
{
	INT64 nChangeCoin = nCoin;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if( nLogCode == DBDNWorldDef::CoinChangeCode::PickUp )
	{
		float fIncGoldRate = 0.0f;
		if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_266))
		{
			DNVector(DnBlowHandle) vlBlows;
			GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_266, vlBlows);
			{
				int nCount = (int)vlBlows.size();
				for (int i = 0; i < nCount; ++i)
				{
					DnBlowHandle hBlow = vlBlows[i];
					if (hBlow && hBlow->IsEnd() == false)
					{
						fIncGoldRate += hBlow->GetFloatValue();
					}
				}
			}
			nChangeCoin += (INT64)(nChangeCoin * fIncGoldRate);
			if( !m_pSession->CheckMaxCoin(nChangeCoin) )
				return;
		}
	}	
#endif
#if defined(_CH)
	if (m_pSession->GetFCMState() == FCMSTATE_HALF){
		nChangeCoin = nCoin / 2;
	}
	else if (m_pSession->GetFCMState() == FCMSTATE_ZERO){
		nChangeCoin = 0;
	}
#endif	// _CH

#if defined( _GAMESERVER )
	if( nLogCode != DBDNWorldDef::CoinChangeCode::PickUp )
		g_Log.Log(LogType::_ERROR, m_pSession, L"CmdAddCoin PrevCoin=%I64d  ChangeCoin=%I64d PickUpCoint=%I64d LogType=%d\r\n", m_pSession->GetCoin(), nChangeCoin, m_pSession->GetPickUpCoin(), nLogCode);
#endif

	if( nLogCode == DBDNWorldDef::CoinChangeCode::PickUp )
	{
		m_pSession->AddPickUpCoin( nChangeCoin );
#if defined( _WORK )
		std::cout << nCoin << " Coin 획득 => 총 " << m_pSession->GetPickUpCoin() << " Coin" << std::endl;
#endif // #if defined( _WORK )
		nLogCode = 0;	// DB에 저장하지 않기 위해 다시 초기화
	}

	// 여기 주의!! bSync 랑 AddCoin 의 bWarehouse 는 틀리지만 쨋든 패킷 보낸다,안보낸다 차이기 때문에 걍 쓴다.ㅋㅋㅋㅋ 우씨 -> 바꿨음 Send로... ㅎㅎㅎ
	if( m_pSession && m_pSession->GetItem() ){
		if (nChangeCoin > 0)
			m_pSession->AddCoin( nChangeCoin, nLogCode, nFKey, bSync );
		else if (nChangeCoin < 0)
			m_pSession->DelCoin( -nChangeCoin, nLogCode, nFKey, bSync );
	}
}

void CDnPlayerActor::OnDispatchMessage( CDNUserSession *pSession, DWORD dwActorProtocol, BYTE *pPacket )
{

	switch( dwActorProtocol ) {
		case eActor::CS_CMDMOVE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex;
				EtVector3 vPos, vXVec;
				EtVector2 vZVec, vLook;
				char cFlag;
				DWORD dwGap;
				int nMoveSpeed;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &cFlag, sizeof(char) );
				Stream.Read( &nMoveSpeed, sizeof(int) );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );
				m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );
#ifdef PRE_ADD_CHECK_MOVESPEED_HACK
				m_pPlayerSpeedHackChecker->OnSyncMoveSpeed( nMoveSpeed );
#endif
				if( CheckSkillAction(pStruct->szName.c_str() ) == true )
				{
					((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
					break;
				}

				Look( vLook, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				SetPosition( vPos );
//				SetMagnetPosition( vPos );

#ifdef _USE_VOICECHAT
				m_nVoiceRotate = (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vZVec ) ) );
				if( vZVec.x > 0.0f )
					m_nVoiceRotate = 360 - m_nVoiceRotate;
				m_nVoiceRotate = (m_nVoiceRotate + 90) % 360;
				m_pSession->SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, m_nVoiceRotate);
#endif

				m_cMovePushKeyFlag = cFlag;
				float fXSpeed = 0.f, fZSpeed = 0.f;
				if( cFlag & 0x01 ) fXSpeed = -100000.f;
				if( cFlag & 0x02 ) fXSpeed = 100000.f;
				if( cFlag & 0x04 ) fZSpeed = 100000.f;
				if( cFlag & 0x08 ) fZSpeed = -100000.f;
				vPos += ( vXVec * fXSpeed );
				vPos += ( EtVec2toVec3( vZVec ) * fZSpeed );

				CmdMove( vPos, pStruct ? pStruct->szName.c_str() : "", -1, 8.f );
			}
			break;
		case eActor::CS_CMDSTOP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				DWORD dwGap;
				bool bReset, bForce;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &bReset, sizeof(bool) );
				Stream.Read( &bForce, sizeof(bool) );

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );
				m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );

				/*
				std::string szActionName = GetCurrentAction();
				if( EtVec2Length( &( EtVector2( vPos.x, vPos.z ) - EtVector2( GetPosition()->x, GetPosition()->z ) ) ) > 100.f ) {
					if( GetState() != ActorStateEnum::Move ) szActionName = "Move_Front";
				}

				CmdMove( vPos, szActionName.c_str(), -1, CDnActionBase::m_fQueueBlendFrame );
				*/
				if( IsProcessSkill() && bForce ) CancelUsingSkill();
				SetPosition( vPos );
				OnStop( vPos );

#ifdef _USE_VOICECHAT
				m_pSession->SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, m_nVoiceRotate);
#endif
			}
			break;
		case eActor::CS_CMDACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount;
				float fBlendFrame;
 				EtVector3 vXVec, vPos;
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
				Stream.Read( &bSkillChain, sizeof(bool) );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );

				ResetMove();

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
				Look( vLook );
				SetMagnetPosition( vPos );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				if( bSkillChain )
				{
					if( false == IsValidSkillChain( GetCurrentActionIndex(), nActionIndex ) )
						return;
				}
				else
				{
					bool bCheckStandAction = CDnChangeStandActionBlow::CheckUsableAction( GetActorHandle(), true, pStruct->szName.c_str() );
		
					if( IsProcessSkill() == true && CheckSkillAction( pStruct->szName.c_str() ) == true || bCheckStandAction == false ) 
					{
						ActionElementStruct *pCurrentActionElement = GetElement( GetCurrentActionIndex() );
						if( pCurrentActionElement )
						{
							bool bCorrectInputAction = false;

							for( DWORD iSignal = 0; iSignal < pCurrentActionElement->pVecSignalList.size(); ++iSignal )
							{
								CEtActionSignal* pSignal = pCurrentActionElement->pVecSignalList.at( iSignal );
								if( STE_Input == pSignal->GetSignalIndex() )
								{
									InputStruct* pInputStruct = (InputStruct*)(pSignal->GetData());
									if( strcmp( pInputStruct->szChangeAction, pStruct->szName.c_str() ) == NULL )
									{
										bCorrectInputAction = true;
										break;
									}
								}
							}

							if( bCorrectInputAction == false )
							{
								((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
								return;
							}
						}
					}
				}

				if( m_pBasicAttackInfo && strstr(pStruct->szName.c_str(), "Attack") )
				{
					DNVector(int) nVecCheckActionList;
					nVecCheckActionList.push_back( GetCurrentActionIndex() );
					if( IsCustomAction() ) 
						nVecCheckActionList.push_back( GetCustomActionIndex() );

					// next 액션이 있는 경우엔 10 프레임 정도의 여유를 두고 검증할 리스트에 추가해준다.
					// 서버 프레임이 느려서 next action 으로 stand 잡아놓고 입력 받게 해도 서버에서 
					// next 액션 셋팅이 안되어서 핵으로 간주되는 경우가 있다.
					ActionElementStruct* pCurrentActionElement = GetElement( GetCurrentActionIndex() );
					if( pCurrentActionElement )
					{
						if( 0 < pCurrentActionElement->szNextActionName.length() )
						{
							// 클라이언트가 어떻게 조작하느냐에 따라 서버와의 프레임 격차가 커질 수 있어서 일단 프레임체크는 서버에서 할 수 없으므로 뺌.
							//if( pCurrentActionElement->dwLength - (DWORD)CDnActionBase::m_fFrame < 10 )
							{
								int iNextActionIndex = GetElementIndex( pCurrentActionElement->szNextActionName.c_str() );
								if( -1 < iNextActionIndex )
									nVecCheckActionList.push_back( iNextActionIndex );
							}
						}
					}

					// cmdaction 으로 평타 패킷을 마구 보내는 경우, 전이될 수 있는 모든 액션을 체크함으로써 기본적으로 막힐 수 있으며,
					// 실제로 타격이 되는 액션의 프레임은 조금 지나야 나오므로 cmdaction 패킷엔 임의의 프레임으로부터 액션을 시작할 수 없기 때문에
					// 이 방법이 유효하다. 하지만 바로 아래 mixedaction 은 임의로 액션 시작 프레임을 정할 수 있으므로 해당 부분을 따로 막아야 한다.
					bool bValid = false;
					for( DWORD k=0; k<nVecCheckActionList.size(); k++ ) 
					{
						map<int, vector<CDnActionSpecificInfo::S_BASIC_ATTACK_INPUT_SIGNAL_INFO> >::const_iterator iter = m_pBasicAttackInfo->mapBasicAttackInfo.find( nVecCheckActionList[k] );
						if( iter != m_pBasicAttackInfo->mapBasicAttackInfo.end() )
						{
							const vector<CDnActionSpecificInfo::S_BASIC_ATTACK_INPUT_SIGNAL_INFO>& vlInputSignals = iter->second;
							for( int i = 0; i < (int)vlInputSignals.size(); ++i )
							{
								const CDnActionSpecificInfo::S_BASIC_ATTACK_INPUT_SIGNAL_INFO& InputSignalInfo = vlInputSignals.at( i );
								if( strstr( pStruct->szName.c_str(), InputSignalInfo.strChangeActionName.c_str() ) )
								{
									bValid = true;
									break;
								}
							}
						}
					}
					if( !bValid ) 
					{
						((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
						return;
					}
				}

				// 일반 슛 액션이라면 현재 시점에 가능한 것인지 인풋 시그널 간격의 시간과 체크.
				if( m_pProjectileCountInfo ) {
					map<int, DWORD>::const_iterator iterCooltime = m_pProjectileCountInfo->mapBasicShootActionCoolTime.find( nActionIndex );
					if( m_pProjectileCountInfo->mapBasicShootActionCoolTime.end() != iterCooltime )
					{
						DWORD dwTerm = timeGetTime() - m_dwLastBasicShootActionTime;
						DWORD dwCoolTime = DWORD((float)m_dwLastBasicShootCoolTime/m_fFrameSpeed);
						if( dwTerm < dwCoolTime )
						{
							// 정해진 시간 간격 이하로 일반 공격 패킷이 왔음. 핵입니다..
	#ifndef _FINAL_BUILD
							OutputDebug( "CDnPlayerActor-CS_CMDACTION: 액션툴에서 정해진 시간 간격 이하로 일반 슛 액션이 왔음. 핵으로 판단.\n" );
	#endif // #ifndef _FINAL_BUILD
							return;
						}

						m_dwLastBasicShootActionTime = timeGetTime();
						m_dwLastBasicShootCoolTime = iterCooltime->second;
					}
				}

				if( IsCustomAction() ) ResetCustomAction();

				// 스킬 채인이 아닌 경우라면 현재 cmdaction 패킷이 스킬에서 사용되는 액션인지 체크한다. #26467
				// 위에서 스킬 체인에 대한 시그널 데이터 검증이 다 끝나야 이쪽으로 흐름이 오기 때문에 안심하고 확인하면 된다.
				if( false == bSkillChain )
					_CheckProcessSkillActioncChange( pStruct->szName.c_str() );

				CmdAction( pStruct->szName.c_str(), nLoopCount, fBlendFrame, false, false, bSkillChain );

				// CS_CMDACTION 패킷이 왔을 때 바로 업데이트 해주도록 변경.
				_UpdateMaxProjectileCount( nActionIndex );
				m_bUpdatedProjectileInfoFromCmdAction = true;

				// 서버나 클라 둘 중에 상태효과쪽에서 발생시킨 액션.
				if( bFromStateBlow )
					m_pStateBlow->OnCmdActionFromPacket( pStruct->szName.c_str() );
				//
#ifdef _USE_VOICECHAT
				m_nVoiceRotate = (int)EtToDegree( acos( EtVec2Dot( &EtVector2( 0.f, 1.f ), &vZVec ) ) );
				if( vZVec.x > 0.0f )
					m_nVoiceRotate = 360 - m_nVoiceRotate;
				m_nVoiceRotate = (m_nVoiceRotate + 90) % 360;
				m_pSession->SetVoicePos((int)vPos.x, (int)vPos.y, (int)vPos.z, m_nVoiceRotate);
#endif
			}
			break;
		case eActor::CS_CMDMIXEDACTION:
			{
				// Note: 액티브 패시브 스킬 사용 중에 일반 액션을 하게 된다면 체크해서 사용중인 스킬을 종료 시킨다.
				// 상태효과가 남아서 일반 공격에도 영향을 미치는 것을 막기 위해서. 
				if( IsProcessSkill() ) 
				{
					if( (m_hProcessSkill->GetSkillType() == CDnSkill::Passive || m_hProcessSkill->GetSkillType() == CDnSkill::AutoPassive) &&
						 m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
					{
						m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
						m_hProcessSkill.Identity();
					}
				}

				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;

				int nActionIndex, nMaintenanceBone, nActionBone;
				float fFrame, fBlendFrame;
				EtVector3 vLook;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &nActionBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nMaintenanceBone, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &fFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &fBlendFrame, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10.f );
				Stream.Read( &m_cMovePushKeyFlag, sizeof(char) );

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );
				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL ) break;

				// 기본적으로 cmdaction 으로 날아오는 Skill_ 관련 패킷은 잘못된 것이다. (스킬 체인 입력을 제외하고)
				// 현재 돌아가는 예외 액션 모음에 없다면 무시한다.
				if( CheckSkillAction(pStruct->szName.c_str()) == true )
				{
					((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
					break;
				}

				bool bCheckStandAction = CDnChangeStandActionBlow::CheckUsableAction( GetActorHandle(), true, pStruct->szName.c_str() );
				if( bCheckStandAction == false )
				{
					((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
					return;
				}
	
				// 일반 슛 액션이라면 현재 시점에 가능한 것인지 인풋 시그널 간격의 시간과 체크.
				// 새로 액션을 취하는 경우(0 프레임으로 패킷 온다.)에만 기본 공격 액션 쿨타임을 체크한다.
				// 만약 핵을 썼다면 0 프레임으로 보내면 액션 쿨타임에 걸리고, 0 프레임 이상을 보낸다면 
				// 발사체 갯수나 기타 핵관련 인증 정보를 업데이트를 해주지 않도록 한다. 그 뒤로 쏘는 발사체는 무효가 된다..
				// cmdaction 쪽은 무조건 0 프레임으로 시작하므로 MIXEDACTION 만 클라에서 보내주는 프레임을 체크하면 된다.
				bool bActionCoolTimeChecked = false;

				if( m_pProjectileCountInfo )
				{
					map<int, DWORD>::const_iterator iterCooltime = m_pProjectileCountInfo->mapBasicShootActionCoolTime.find( nActionIndex );
					if( m_pProjectileCountInfo->mapBasicShootActionCoolTime.end() != iterCooltime )
					{
						if( 0.0f == fFrame )
						{
							DWORD dwTerm = timeGetTime() - m_dwLastBasicShootActionTime;
							DWORD dwCoolTime = DWORD((float)m_dwLastBasicShootCoolTime/m_fFrameSpeed);
							if( dwTerm < dwCoolTime )
							{
								// 정해진 시간 간격 이하로 일반 공격 패킷이 왔음. 핵입니다..
#ifndef _FINAL_BUILD
								OutputDebug( "CDnPlayerActor-CS_CMDMIXEDACTION: 액션툴에서 정해진 시간 간격 이하로 일반 슛 액션이 왔음. 핵으로 판단.\n" );
#endif // #ifndef _FINAL_BUILD
								return;
							}

							m_dwLastBasicShootActionTime = timeGetTime();
							m_dwLastBasicShootCoolTime = iterCooltime->second;

							bActionCoolTimeChecked = true;
						}
					}
				}

				if( false == bActionCoolTimeChecked )
				{
					// 프레임 값이 있는 경우엔 워리어, 클러릭등이 평타 한 번 하고 이동할 때임. 이 패킷을 임의의 평타액션으로 hit 시그널 있는 프레임으로 마구 
					// 보내 계속 hit 하는 핵이 나올 수 있어 방어한다. 방법은 중간중간 CmdStop 이나 CmdMove 등으로 Stand 액션으로 변경하면서 MixedAction 을 보내면
					// 서버로서는 검증할 방법이 없으므로 쿨타임으로 체크를 한다..
					LOCAL_TIME MixedActionCoolTime = 500;
					if( CDnActionBase::m_LocalTime - m_MixedActionTimeStamp < MixedActionCoolTime )
					{
						if( CDnActionBase::m_LocalTime - m_MixedActionTimeStamp < 0 )
							m_MixedActionTimeStamp = CDnActionBase::m_LocalTime;

						// 일정 시간 간격 이상으로 들어오면 핵으로 판단.
						return;
					}
					m_MixedActionTimeStamp = CDnActionBase::m_LocalTime;
				}


				std::string szAction = pStruct->szName;
				m_szActionBoneName = GetBoneName(nActionBone);
				m_szMaintenanceBoneName = GetBoneName(nMaintenanceBone);

				if( IsCustomAction() ) ResetCustomAction();
				SetCustomAction( szAction.c_str(), fFrame );

				// MixedAction 은 OnChangeAction() 함수가 호출이 안되므로 업데이트 해주어야 함.
				//_UpdateMaxProjectileCount( nActionIndex, true );

				// 프레임 값이 0 이 아니면 플레이어의 방향만 바꿔주는 경우다.
				if( 0.0f == fFrame )
				{
					_UpdateMaxProjectileCount( nActionIndex );
				}
			}
			break;
		case eActor::CS_PROJECTILE:
			{
				// next 액션이 있고 거의 끝 프레임에 다다랐을 경우엔 액션이 바뀌기 직전이므로 클라에서 
				// 액션을 바꿔서 발사체를 쏘더라도 서버는 못따라갔을 수 있다. 한발 여유를 준다.
				// 핀포인트 샷처럼 빠르게 loop 액션을 행하는 경우 문제가 될 수 있다.
#ifdef PRE_ADD_LOOP_PROJECTILE
				ActionElementStruct* pActionElement = GetElement( m_szAction.c_str() );
				if( pActionElement && false == pActionElement->szNextActionName.empty() )
				{
					// TODO: 추가적으로 IsCustomAction() 으로 mixed 애니메이션을 체크해서 
					// 정확하게 발사체 인증 정보를 업데이트 해준다. 지금 이렇게만 해두면 뒤로 가면서 
					// 화살 쏘면 Move_Back 만 업데이트 되어 발사체 갯수는 0 이 됨.
					int iNextActionElementIndex = GetElementIndex( pActionElement->szNextActionName.c_str() );
					if( GetCurrentActionIndex() == iNextActionElementIndex  )
						_UpdateMaxProjectileCount( iNextActionElementIndex );
				}
#endif // #ifdef PRE_ADD_LOOP_PROJECTILE

				CDnProjectile* pProjectile = CDnProjectile::CreatePlayerProjectileFromClientPacket( GetMySmartPtr(), pPacket );
			}
			break;
		case eActor::CS_CMDLOOK:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector2 vLook, vZVec;
				EtVector3 vXVec;
				bool bForce;

				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &bForce, sizeof(bool) );

				Look( vLook, bForce );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );
			}
			break;
		case eActor::CS_USESKILL:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillTableID = 0;
				int nEnchantSkillID = 0;		// #38294 게임 서버에서는 사용할 일 없다. 빌리지 서버에서 ex 스킬 사용시 정보를 알려주기 위한 용도.
				BYTE cLevel;
				EtVector2 vLook, vZVec;
				EtVector3 vXVec;
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
				EtVector3 vPos;
#endif
				bool bUseApplySkillItem = false;
				bool bAutoUseFromServer = false;		// 겜서버에선 사용하지 않는 데이터.


				Stream.Read( &nSkillTableID, sizeof(int) );
				Stream.Read( &cLevel, sizeof(char) );
				Stream.Read( &bUseApplySkillItem, sizeof(bool) );
				Stream.Read( m_abSignalSkillCheck, sizeof(m_abSignalSkillCheck) );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &bAutoUseFromServer, sizeof(bool) );
				Stream.Read( &nEnchantSkillID, sizeof(int) );
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
				Stream.Read(&vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				SetPosition( vPos );
#endif
	
				Look( vLook, true );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );
				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				// 연속으로 이어서 스킬이 사용되는 경우 CanMove 가 false 인 경우가 있을 수 있는데
				// 이런 경우엔 Movable 로 스킬 발동조건을 걸어놓은 경우 씹힐 수 있기 때문에 true 로 바꿔줌.
				// 이미 클라이언트에선 사용가능 상태라서 패킷이 온 상황이다. (정상적인 경우라면) #14127
				if( false == m_bMovable )
					m_bMovable = true;
				////////////////////////////////////////////////////////////////////////////////////////////////////

				// Note: 게임서버에서는 ApplySkill이 붙은 아이템 사용을 사용한 클라쪽에서 보내기 때문에 그 시점에 이미 처리합니다..
				// 그런고로 여기선 무시. 다른 클라이언트들에게 적용시키기 위한 플래그입니다.
				if( false == bUseApplySkillItem )
				{
					bool bSkipAirCondition = false;

					if(CheckSkipAirCondition(nSkillTableID))
					{
						SetSignalSkillCheck(2,true);
						bSkipAirCondition = true;
					}
					// 상태가 Air인경우에 다음액션이 GroundMovable 조건이 있다면 클라와 서버의 갭으로 인해 Air 체크에 걸려서 취소되는 경우가 있다.

					m_bUseSignalSkillCheck = (m_abSignalSkillCheck[ 0 ] || m_abSignalSkillCheck[ 1 ] || m_abSignalSkillCheck[ 2 ]);

					if( m_hProcessSkill ) 
					{
						// #25154 오라 스킬은 오라를 껐을 때 onend 된다.
						if( false == m_hProcessSkill->IsAuraOn() )
						{
							m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
						}
						else
						{
							// #26002 오라 스킬이 종료된 거라면 자기 자신에게 적용하는 상태효과 리스트를 비우도록 한다.
							// 안그러면 다른 스킬을 사용할 때 영향을 미치게 된다. 
							// 원래 ApplyStateEffect 시그널을 사용하는 의도가 중간에 피격시 스킬 액션이 끊기면 적용이 안되게 하는 것이므로,
							// 유저가 곧바로 다른 스킬을 사용해서 자기 자신에게 적용하는 상태효과 시그널이 돌기 전에 액션이 바뀌면
							// 당연히 상태효과가 적용 안되게 되므로 액션 디자인을 할 때 이부분이 고려되어 있다는 전제 하에 이렇게 처리한다.
							if( IsEnabledAuraSkill() )
								ClearSelfStateSignalBlowQueue();
						}

						m_hProcessSkill.Identity();
					}
					
					if (GetActorHandle() && GetActorHandle()->IsAppliedThisStateBlow(STATE_BLOW::BLOW_345)) //rlkt_mechanicMODE
					{
						if (IsExistSkill(nSkillTableID, cLevel) == false)
							AddSkill(nSkillTableID, cLevel);
					}

					if (IsExistSkill(nSkillTableID, cLevel) == false) {
						((CDnPlayerSkillChecker*)m_pPlayerSkillChecker)->OnInvalidUseSkill(nSkillTableID, CDnSkill::UsingResult::NoExistSkill);
						// Hack!!!
						break;
					}
#if defined( PRE_FIX_CHANGESTAND_HACK )
					CDnChangeStandActionBlow::ReleaseStandChangeSkill( GetActorHandle(), true  );
#endif
		
					CDnSkill::UsingResult eResult = UseSkill( nSkillTableID );
					if( eResult != CDnSkill::UsingResult::Success ) 
					{
						// 쿨타임 부족으로 인해 스킬 사용 실패되면 핵으로 판단.
						// 나머지는 클라에서 스킬 사용하는 순간 이미 서버에서 맞은 상태로 판단하여 클라이언트로 결과를 통보.
						// 클라이언트 쪽에서는 쿨타임을 리셋시킨다.
						((CDnPlayerSkillChecker*)m_pPlayerSkillChecker)->OnInvalidUseSkill( nSkillTableID, eResult );
						switch( eResult )
						{
							case CDnSkill::FailedByCooltime:
								break;

							case CDnSkill::FailedByUsableChecker:
								{
#ifdef PRE_FIX_69469
									if( GetProcessSkill() ) CancelUsingSkill();
									SetAction( "Stand", 0.f, 3.f );
#endif
									const char* pCurrentAction = GetCurrentAction();
									int iCurrentActionIndex = GetElementIndex( pCurrentAction );

									BYTE pBuffer[ 32 ] = { 0 };
									CPacketCompressStream LocalStream( pBuffer, 32 );

									LocalStream.Write( &nSkillTableID, sizeof(int) );
									LocalStream.Write( &eResult, sizeof(CDnSkill::UsingResult) );
									LocalStream.Write( &iCurrentActionIndex, sizeof(int) );

									Send( eActor::SC_SKILLUSING_FAILED, &LocalStream );
								}
								break;
						}

					}

					if(bSkipAirCondition) 
						SetSignalSkillCheck(2,false); // 다시 해제해준다.


					// 만약 장비아이템 스킬이라면,
					if( m_hProcessSkill && m_hProcessSkill->IsEquipItemSkill() )
					{
						m_afLastEquipItemSkillDelayTime = m_hProcessSkill->GetDelayTime();
						m_afLastEquipItemSkillRemainTime = m_hProcessSkill->GetElapsedDelayTime();
					}


					m_bUseSignalSkillCheck = false;
					ZeroMemory( m_abSignalSkillCheck, sizeof(m_abSignalSkillCheck) );
				}

			}
			break;
		case eActor::CS_VIEWSYNC:
			{
				if( GetGameRoom() && GetGameRoom()->GetGameTask() && !GetGameRoom()->GetGameTask()->IsSyncComplete() ) break;
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos, vXVec;
				EtVector2 vZVec, vLook;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );

				Look( vLook, false );

				EtVec3Cross( &vXVec, &EtVector3( 0.f, 1.f, 0.f ), &EtVec2toVec3( vZVec ) );

				SetMoveVectorX( vXVec );
				SetMoveVectorZ( EtVec2toVec3( vZVec ) );

				m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );

				SetPosition( vPos );
				// SetMagnetPosition( vPos );
			}
			break;
		case eActor::CS_CMDTOGGLEBATTLE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				bool bBattle;
				Stream.Read( &bBattle, sizeof(bool) );

				CmdToggleBattle( bBattle );
			}
			break;
		case eActor::CS_CMDPASSIVESKILLACTION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nActionIndex, nLoopCount, nSkillID;
				BYTE cLevel;
				float fBlendFrame;
				float fStartFrame;
				EtVector3 vXVec, vPos;
				EtVector2 vLook, vZVec;
				bool bChargeKey = false;
				Stream.Read( &nSkillID, sizeof(int));
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
				Look( vLook );
				//SetMagnetPosition( vPos );
				SetPosition( vPos );

				ActionElementStruct *pStruct = GetElement( nActionIndex );
				if( pStruct == NULL )
				{
					// Hack!!
					break;
				}

				// 핵체크를 위해 커스텀 액션 정보도 필요하므로 몇 줄 아래에서 정보 빼내고 리셋합니다.
				//if( IsCustomAction() ) ResetCustomAction();

				// Tumble 류 액션은 서버에서 Move 액션 중이 아니기 때문에 아래 핵체크에 걸려 실행이 안된다.
				// Tumble 액션 요청이 여기로 왔을 때 서버에서 Stand 액션 상태인 이유는 이 패킷이 오기 전에 
				// CMDSTOP 패킷이 먼저 오기 떄문이다. Tumble 로 전이할 수 있는 시그널은 Move 액션에 있기 때문에
				// 핵체크에 걸리게 된다. 현재로썬 서버에서 판단할 수 없는 부분이므로 Tumble 류 액션은 그냥 통과시킨다.
				// 다만 클라에서 온 skillid 를 믿을 수 없으므로 Tumble 류 액션이 왔을 땐 관련된 패시브 스킬 id 로 바꿔서 
				// 흘려보내준다. 그러면, 스킬의 쿨타임이나 보유 여부는 CmdInputHasPassiveSkill 에서도 체크해서 임의로 사용하는 경우엔 걸리게 된다.
				int iCurrentActionIndex = GetCurrentActionIndex();
				vector<int> vlCheckActionIndices;
				vlCheckActionIndices.push_back( iCurrentActionIndex );

				// #23245 패링 시리즈들 처럼 상태효과에서 직접 액션을 조작하는 경우 서버쪽에서 액션 큐에만 넣고 프레임 갱신이 
				// 되기 전에 클라이언에서 이미 바뀐 액션에서의 패시브 스킬 사용 요청이 오는 경우가 있으므로 queue 된 액션까지 감안한다.
				int iQueuedActionIndex = -1;
				if( false == m_szActionQueue.empty() )
				{
					iQueuedActionIndex = GetElementIndex( m_szActionQueue.c_str() );
					vlCheckActionIndices.push_back( iQueuedActionIndex );
				}

				// mixed action 중이라면 해당 액션까지 포함.
				if( IsCustomAction() )
				{
					int iCustomActionIndex = GetCustomActionIndex();
					vlCheckActionIndices.push_back( iCustomActionIndex );
					
					ResetCustomAction();
				}

				// 현재 액션이 cmdstop 으로 인해 stand 일 때만 tumblehelper 를 사용한다.
				bool bNowStand = (0 != strstr( m_szAction.c_str(), "Stand" ));

				// 이동 중인 경우도 방향 관계없이 어떤 덤블링 액션이라도 수행할 수 있도록 Move 상태에서도 tunble helper 를 사용한다.
				// 클라이언트에서 옆으로 이동하다가 곧바로 shift+앞 방향키로 다른 방향으로 대쉬하도록 여기에 패킷이 오는 경우가 있어서 감안한다.
				// 안 그러면 현재 방향으로의 move 액션 시그널에 걸리지 않아서 핵으로 간주되어 사용되지 않는다.
				bool bNowMove = IsMove();

				bool bValid = false;
				for( int k = 0; k < (int)vlCheckActionIndices.size(); ++k )
				{
					int iCheckActionIndex = vlCheckActionIndices.at( k );

					if( bNowStand || bNowMove )
					{
						map<int, int>::iterator iterTumble = m_mapTumbleHelper.find( nActionIndex );
						if( m_mapTumbleHelper.end() != iterTumble )
						{
							// 아무 이동 액션에서 Tumble 이 가능하므로 현재 액션을 이걸로 셋팅해주어야 Tumble 이 핵 체크에서 valid 하게 떨어진다.
							iCheckActionIndex = iterTumble->second;
						}
					}

					if( !m_pPassiveSkillInfo || ( m_pPassiveSkillInfo && m_pPassiveSkillInfo->mapPassiveSkillInfo.empty() ) )
						continue;

					// 우선 패킷으로 온 액션이 현재 액션에서 inputhaspassive 스킬로 전이 될 수 있는지 확인.
					map<int, vector<CDnActionSpecificInfo::S_PASSIVESKILL_SIGNAL_INFO> >::const_iterator iter = m_pPassiveSkillInfo->mapPassiveSkillInfo.find( iCheckActionIndex );
					if( m_pPassiveSkillInfo->mapPassiveSkillInfo.end() != iter )
					{
						const vector<CDnActionSpecificInfo::S_PASSIVESKILL_SIGNAL_INFO>& vlPassiveSkillSignalInfos = iter->second;

						// 먼저 액션이름이 정확히 일치하는 것이 있다면 해당 시그널이 우선이고, 문자열 포함되는 것은 그 다음 순위이다.
						for( int i = 0; i < (int)vlPassiveSkillSignalInfos.size(); ++i )
						{
							const CDnActionSpecificInfo::S_PASSIVESKILL_SIGNAL_INFO& PassiveSkillInfo = vlPassiveSkillSignalInfos.at( i );
							if( PassiveSkillInfo.strChangeActionName == pStruct->szName ||
								PassiveSkillInfo.strEXSkillChangeActionName == pStruct->szName )
							{
								// 클라로부터 온 요청에 있는 액션 이름이 현재 액션에서 전이될 수 있는 패시브 스킬 액션이지만 
								// 보유하고 있는 패시브 스킬인지 한번 더 체크한다.
								nSkillID = PassiveSkillInfo.iSkillID;
								if( IsExistSkill( PassiveSkillInfo.iSkillID ) )
								{
									// 강화 패시브 스킬로 패시브 스킬이 강화가 된 경우. 
									// 강화 패시브 스킬의 액션이 패킷으로 와야 정상.
									DnSkillHandle hSkill = FindSkill( PassiveSkillInfo.iSkillID );
									if( hSkill->IsEnchantedSkill() )
									{
										if( PassiveSkillInfo.strEXSkillChangeActionName == pStruct->szName )
											bValid = true;
									}
									else
										bValid = true;

									// 차지샷인경우엔 액션의 쿨타임을 따로 측정한다.
									if( strstr( pStruct->szName.c_str(), "ChargeShoot_" ) )
									{
										if( timeGetTime() - m_dwLastChargeShootTime < 1500 )
										{
											// 차지샷 액션을 패시브 스킬 패킷으로 계속 날리는 핵임.
											bValid = false;
										}
										else
											m_dwLastChargeShootTime = timeGetTime();
									}
								}
								break;
							}
						}

						// 일치하는 액션 이름을 찾지 못한 경우.
						if( false == bValid )
						{
							for( int i = 0; i < (int)vlPassiveSkillSignalInfos.size(); ++i )
							{
								const CDnActionSpecificInfo::S_PASSIVESKILL_SIGNAL_INFO& PassiveSkillInfo = vlPassiveSkillSignalInfos.at( i );
								if( strstr( pStruct->szName.c_str(), PassiveSkillInfo.strChangeActionName.c_str() ) )		// 소서리스는 _Book, _Orb 이런게 붙기 때문에 포함여부로 체크한다.
								{
									// 클라로부터 온 요청에 있는 액션 이름이 현재 액션에서 전이될 수 있는 패시브 스킬 액션이지만 
									// 보유하고 있는 패시브 스킬인지 한번 더 체크한다.
									nSkillID = PassiveSkillInfo.iSkillID;
									if( IsExistSkill( PassiveSkillInfo.iSkillID ) )
									{
										// 강화 패시브 스킬로 패시브 스킬이 강화가 된 경우. 
										// 강화 패시브 스킬의 액션이 패킷으로 와야 정상.
										DnSkillHandle hSkill = FindSkill( PassiveSkillInfo.iSkillID );
										if( hSkill->IsEnchantedSkill() )
										{
											if( PassiveSkillInfo.strEXSkillChangeActionName == pStruct->szName )
												bValid = true;
										}
										else
											bValid = true;

										// 차지샷인경우엔 액션의 쿨타임을 따로 측정한다.
										if( strstr( pStruct->szName.c_str(), "ChargeShoot_" ) )
										{
											if( timeGetTime() - m_dwLastChargeShootTime < 1500 )
											{
												// 차지샷 액션을 패시브 스킬 패킷으로 계속 날리는 핵임.
												bValid = false;
											}
											else
												m_dwLastChargeShootTime = timeGetTime();
										}
									}
								}

								if( bValid )
									break;
							}
						}

						if( bValid )
							break;
					}
				}


#if defined( PRE_FIX_CHANGESTAND_HACK )
				CDnChangeStandActionBlow::ReleaseStandChangeSkill( GetActorHandle(), true  );
#endif

				if( false == bValid )
				{
					// Hack!! 현재 액션에서 전이될 수 없는,, 갖고 있는 패시브 스킬 시그널들에 없는 액션을 실행하려 함.
					// 아니면 보유하고 있지 않은 패시브 스킬의 액션을 실행하려 하거나.. 
					// 핵이다.
#ifdef PRE_FIX_SKILL_FAILED_BY_ACTION
#ifdef PRE_FIX_69469
					if( GetProcessSkill() ) CancelUsingSkill();
					SetAction( "Stand", 0.f, 3.f );
#endif
					const char* pCurrentAction = GetCurrentAction();
					int iCurrentActionIndex = GetElementIndex( pCurrentAction );

					BYTE pBuffer[ 32 ] = { 0 };
					CPacketCompressStream LocalStream( pBuffer, 32 );

					CDnSkill::UsingResult eResult = CDnSkill::FailedByInvailedAction;

					LocalStream.Write( &nSkillID, sizeof(int) );
					LocalStream.Write( &eResult, sizeof(CDnSkill::UsingResult) );
					LocalStream.Write( &iCurrentActionIndex, sizeof(int) );
					
					Send( eActor::SC_SKILLUSING_FAILED, &LocalStream );
#endif

					return;
				}
		
				// 위의 핵 테스트를 통과하면 나머 nLoopCount 나 fBlendFrame 이나 fStartFrame 등등을 그대로 사용하게 되는데
				// 이 부분이 문제될 시에는 시그널에 있는 데이터를 그대로 사용토록 한다.
				CmdPassiveSkillAction( nSkillID, pStruct->szName.c_str(), nLoopCount, fBlendFrame, fStartFrame, bChargeKey, false, bOnlyCheck );
				m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );
			}
			break;

		case eActor::CS_POSREV:
			{
				if( GetGameRoom() && GetGameRoom()->GetGameTask() && !GetGameRoom()->GetGameTask()->IsSyncComplete() ) break;
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;
				bool bMove;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &bMove, sizeof(bool) );

				_ASSERT( m_pPlayerSpeedHackChecker );
				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );
				m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );

				SetPosition( vPos );
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
			}
			break;
		case eActor::CS_ONDROP:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;
				DWORD dwGap;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &dwGap, sizeof(DWORD) );

				_ASSERT( m_pPlayerSpeedHackChecker );
				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );

				SetPosition( vPos );
				SetJumpMovement( EtVector2( 0.f, 0.f ) );
			}
			break;

		case eActor::CS_CMDREMOVESTATEEFFECT:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				STATE_BLOW::emBLOW_INDEX emBlowIndex;
				Stream.Read( &emBlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );

				// 다른 클라이언트들에게도 상태효과 제거한 클라에서 요청한 제거 패킷을 알아야 하기 때문에
				// 다시 브로드캐스팅 시킨다. 상태효과 제거 요청한 클라에선 이미 제거된 상태.
				CmdRemoveStateEffect( emBlowIndex );
				break;
			}
			break;
#if defined(PRE_FIX_57706)
		case eActor::CS_CMDREMOVESTATEEFFECTFROMID:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nServerBlowID = 0;
				Stream.Read( &nServerBlowID, sizeof(int) );

				//클라이언트만 제거 시간 변경을 알고 있기 때문에 클라이언트에서 서버로 패킷을 전송하고, 서버에서
				//브로드캐스팅 시겨 다른 클라이언트도 해당 상태효과 제거를 한다.
				CmdRemoveStateEffectFromID(nServerBlowID);
				break;
			}
			break;
#endif // PRE_FIX_57706

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

				if (GetUserSession() && GetUserSession()->GetPartyID() > 0) {
					if( nEquipIndex == CASHEQUIP_HELMET || nEquipIndex == CASHEQUIP_EARRING || nEquipIndex == HIDEHELMET_BITINDEX ) {
						for (DWORD i = 0; i < m_pSession->GetGameRoom()->GetUserCount(); i++) {
							CDNGameRoom::PartyStruct *pStruct = m_pSession->GetGameRoom()->GetPartyData(i);
							if (pStruct == NULL) continue;
							pStruct->pSession->SendPartyMemberPart(m_pSession);
						}
					}
				}
			}
			break;

		case eActor::CS_CMDPICKUPITEM:
		{
			if( IsDie() )
				break;
			if( IsInvalidPlayerChecker() ) break;

			CPacketCompressStream Stream( pPacket, 128 );

			DWORD dwUniqueID;
			int nSignalIndex;
			EtVector3 vPos;
			int	nRange = 100;

			Stream.Read( &dwUniqueID, sizeof(DWORD) );
			Stream.Read( &nSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
			Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

			CEtActionSignal *pSignal = GetSignal( m_nActionIndex, nSignalIndex );
			if( pSignal && pSignal->GetSignalIndex() == STE_PickupItem ) {
				PickupItemStruct *pSignalStruct = (PickupItemStruct *)pSignal->GetData();
				nRange = pSignalStruct->nRange;
			}

			m_pPlayerSpeedHackChecker->OnSyncPosition( vPos );
			SetPosition( vPos );

			if( GetGameRoom() )
			{
				DnDropItemHandle hDropItem = GetGameRoom()->FindDropItem( dwUniqueID );
				if( hDropItem )
				{
					((CDnPlayerPickupChecker*)m_pPlayerPickupChecker)->OnPickupDist( vPos, hDropItem );
					if( m_pSession && m_pSession->GetItem() && m_pSession->GetItem()->GetPetEquip() )//플레이어 아이템 착용 장비중 펫을 소환했다면
					{
						const TVehicle* pPet = m_pSession->GetItem()->GetPetEquip();

						// 거리측정(2배 오차 허용)
						float fDist = EtVec2Length( &(EtVector2(hDropItem->GetPosition()->x,hDropItem->GetPosition()->z)-EtVector2(vPos.x,vPos.z) ) );
						if( fDist > pPet->nRange*2.f )
							return;
					}
					else
					{
						// 거리측정(2배 오차 허용)
						float fDist = EtVec2Length( &(EtVector2(hDropItem->GetPosition()->x,hDropItem->GetPosition()->z)-EtVector2(vPos.x,vPos.z) ) );
						if( fDist > nRange*2.f )
							return;
					}

					PickupItemStruct Struct;
					Struct.nRange = nRange;
					CmdPickupItem( &Struct, hDropItem );
				}
			}
			break;
		}

		case eActor::CS_CMDESCAPE:
			{
				// 여기서 실제 먹히게할지 한번 검중.
				if( m_LastEscapeTime > 0 && CDnActionBase::m_LocalTime - m_LastEscapeTime < 1000 * 60 * 1 ) break;

				CPacketCompressStream Stream( pPacket, 128 );
				EtVector3 vPos;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				SetPosition( vPos );
				SetStaticPosition( vPos );
				SetPrevPosition( vPos );
				if( m_pPlayerSpeedHackChecker ) m_pPlayerSpeedHackChecker->ResetInvalid();
				if( m_pPlayerDoNotEnterChecker ) m_pPlayerDoNotEnterChecker->ResetInvalid();

				CmdEscape( vPos );
			}
			break;

		case eActor::CS_CANNONPOSSESS_REQ:
			{
				// 대포를 점유하겠다는 클라로부터의 요청. 
				// 요청의 결과도 보내주도록 한다.

				CPacketCompressStream Stream( pPacket, 128 );
				DWORD dwCannonMonsterActorID = 0;
				MatrixEx Cross;

				Stream.Read( &dwCannonMonsterActorID, sizeof(DWORD) );
				Stream.Read( &Cross, sizeof(MatrixEx) );

				bool bSuccess = false;

				// 대포 몬스터에게 할당.
				CDnActor *pActor = CDnActor::FindActorFromUniqueID( GetRoom(), dwCannonMonsterActorID );
				if( pActor && pActor->GetActorType() == ActorTypeEnum::Cannon )
				{
					CDnCannonMonsterActor* pCannonActor = static_cast<CDnCannonMonsterActor*>( pActor );

					// 클라에서 보내준 대포 몬스터의 근처에 이 캐릭터가 있는지 체크한다.
					// Press Circle 보다 거리가 멀면 핵일 가능성이 있다.. 약간 관용도 값을 주어도 될 듯.
					bool bValid = false;
					float fDistanceSQ = EtVec3LengthSq( &EtVector3(*pCannonActor->GetPosition() - *GetPosition()) );
					float fPressCircleDist = float(GetUnitSize() + pCannonActor->GetUnitSize());
					float fPressCircleDistSQ = fPressCircleDist * fPressCircleDist;
					if( fPressCircleDistSQ <= fDistanceSQ + 1000.0f  || fPressCircleDistSQ - 1000.f <= fDistanceSQ) 
					{
						// 이미 점유중인지.
						if( false == pCannonActor->IsPossessed() )
						{
							m_bPlayerCannonMode = true;
						
							pCannonActor->SetMasterPlayerActor( GetMySmartPtr() );
							m_hCannonMonsterActor = pCannonActor->GetMySmartPtr();
							m_Cross = Cross;
							bSuccess = true;

							// 대포 점유 성공.. 필요한 정보들과 함께 응답 보내줌..
							char acBuffer[ 64 ] = { 0 };
							CPacketCompressStream Result( acBuffer, sizeof(acBuffer) );
							Result.Write( &bSuccess, sizeof(bool) );
							Result.Write( &dwCannonMonsterActorID, sizeof(DWORD) );
							Result.Write( &m_Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
							Result.Write( &m_Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
							Result.Write( &m_Cross.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
							Result.Write( &m_Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
							Send( eActor::SC_CANNONPOSSESS_RES, &Result );

							// 대포상태에서는 밀리지 않도록 설정합니다. 대포에서 내리는 경우에는 리셋시켜줘야합니다.
							SetWeight(0.f);
							SetPressLevel(-1);

#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
							DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );

							if( pTableCannon->IsExistItem( pCannonActor->GetClassID() ) )
							{
								const char* pCannonActionName = pTableCannon->GetFieldFromLablePtr( pCannonActor->GetClassID(), "_StandName" )->GetString();
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
					}
				}

				// 대포 점유 실패.. 응답 보내줌.
				if( false == bSuccess )
				{
					char acBuffer[ 64 ] = { 0 };
					CPacketCompressStream Result( acBuffer, sizeof(acBuffer) );
					Result.Write( &bSuccess, sizeof(bool) );
					Send( eActor::SC_CANNONPOSSESS_RES, &Result );
				}
			}
			break;

		case eActor::CS_CANNONRELEASE:
			{
				DWORD dwCannonMonsterID = 0;
				MatrixEx Cross;

				CPacketCompressStream Result( pPacket, 64 );
				Result.Read( &dwCannonMonsterID, sizeof(DWORD) );
				Result.Read( &Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Result.Read( &Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Result.Read( &Cross.m_vYAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Result.Read( &Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				
				CDnActor* pActor = CDnActor::FindActorFromUniqueID( GetRoom(), dwCannonMonsterID );
				if( pActor && pActor->GetActorType() == ActorTypeEnum::Cannon )
				{
					CDnCannonMonsterActor* pCannonMonster = static_cast<CDnCannonMonsterActor*>(pActor);
					pCannonMonster->ClearMasterPlayerActor();
					m_Cross = Cross;
					
					EndCannonMode();
				}
			}
			break;

		case eActor::CS_CANNONROTATESYNC:
			{
				_ASSERT( IsCannonMode() );
				if( false == IsCannonMode() )
					break;

				CPacketCompressStream Stream( pPacket, 64 );
				DWORD dwCannonMonsterID = 0;
				EtVector3 vDirection( 0.0f, 0.0f, 0.0f );

				Stream.Read( &dwCannonMonsterID, sizeof(DWORD) );
				Stream.Read( &vDirection, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

				CDnActor* pActor = CDnActor::FindActorFromUniqueID( GetRoom(), dwCannonMonsterID );
				if( pActor && pActor->GetActorType() == ActorTypeEnum::Cannon )
				{
					CDnCannonMonsterActor* pCannonMonster = static_cast<CDnCannonMonsterActor*>(pActor);
					EtVec3Normalize( &vDirection, &vDirection );		// 압축되어 전송된 데이터이므로 정규화 한 번 해줌.
					
					pCannonMonster->SetCannonLookDirection(&vDirection); // Rotha - 대포 자체를 돌리는게 아닌 대포의 포신을 돌리도록 설정합니다. 포신은 시점만 관리합니다.


					m_Cross.m_vZAxis = pCannonMonster->GetMatEx()->m_vZAxis;
					m_Cross.MakeUpCartesianByZAxis();

					// press circle 값만큼 밀어주면 된다.
					m_Cross.m_vPosition = pCannonMonster->GetMatEx()->m_vPosition - (pCannonMonster->GetMatEx()->m_vZAxis*20.0f);
					EtVector2 vDir;
					float vDist = 0.0f;
					_ASSERT( GetPress() == CDnActorState::Press_Circle && pCannonMonster->GetPress() == CDnActorState::Press_Circle );
					if( GetPress() == CDnActorState::Press_Circle && pCannonMonster->GetPress() == CDnActorState::Press_Circle )
					{
						if( CheckPressCircle2Clrcle2( pCannonMonster->GetMySmartPtr(), GetMySmartPtr() , vDir, vDist ) ) 
						{
							m_Cross.m_vPosition.x += vDir.x*vDist;
							m_Cross.m_vPosition.z += vDir.y*vDist;
						}
					}

					// 이 타이밍에 다른 어떤 이유로 인해 "Stand" 액션을 캐릭터가 취하고 있으면
					// Stand_Cannon 로 바꿔준다.
					// 감전 액션을 하다가 풀리거나 하는 경우.
					const char* pCurrentAction = GetCurrentAction();
					if( strcmp(pCurrentAction, "Stand") == 0 )
					{
#ifdef PRE_ADD_MODIFY_PLAYER_CANNON
						DNTableFileFormat* pTableCannon = GetDNTable( CDnTableDB::TCANNON );

						if( pTableCannon->IsExistItem( pCannonMonster->GetClassID() ) )
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
				}
			}
			break;
		
		case eActor::CS_CANNONTARGETING:
			{
				if( false == IsCannonMode() )
					break;

				DWORD dwCannonMonsterID = 0;
				EtVector3 vCannonDir;			// 클라이언트에서 카메라가 바라보고 있는 방향. 결국 대포의 방향.
				EtVector3 vShootDir;
				EtVector3 vCannonGroundHitPos;

				CPacketCompressStream Stream( pPacket, 64 );
				Stream.Read( &dwCannonMonsterID, sizeof(DWORD) );
				Stream.Read( &vCannonDir, sizeof(EtVector3) );
				Stream.Read( &vShootDir, sizeof(EtVector3) );
				Stream.Read( &vCannonGroundHitPos, sizeof(EtVector3) );

				CDnActor* pActor = CDnActor::FindActorFromUniqueID( GetRoom(), dwCannonMonsterID );
				if( pActor && pActor->GetActorType() == ActorTypeEnum::Cannon )
				{
					CDnCannonMonsterActor* pCannonMonster = static_cast<CDnCannonMonsterActor*>(pActor);
					pCannonMonster->SetCannonProjectileInfo( vCannonDir, vShootDir, vCannonGroundHitPos );
				}
			}
			break;

		case eActor::CS_VEHICLE_RIDE_COMPLETE:
			{
				if(!IsVehicleMode() || !GetMyVehicleActor())
					return;

				CPacketCompressStream Stream( pPacket, 128 );
				bool bComplete = false;
				Stream.Read( &bComplete, sizeof(bool) );

				if(bComplete)
				{
					DNVector( DnBlowHandle ) vlhFrameBlows;
					DNVector( DnBlowHandle ) vlhMoveSpeedBlows;
					m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_025, vlhFrameBlows );
					m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_076, vlhMoveSpeedBlows );

					if(vlhFrameBlows.size() > 0)
					{
						for(DWORD i=0 ; i<vlhFrameBlows.size() ; i++)
						{
							if(vlhFrameBlows[i])
								GetMyVehicleActor()->CmdAddStateEffect( vlhFrameBlows[i]->GetParentSkillInfo() , vlhFrameBlows[i]->GetBlowIndex() , (int)(vlhFrameBlows[i]->GetDurationTime() * 1000) , 
								vlhFrameBlows[i]->GetValue() );
						}
					}

					if(vlhMoveSpeedBlows.size() > 0)
					{
						for(DWORD i=0 ; i<vlhMoveSpeedBlows.size() ; i++)
						{
							if(vlhMoveSpeedBlows[i])
								GetMyVehicleActor()->CmdAddStateEffect( vlhMoveSpeedBlows[i]->GetParentSkillInfo() , vlhMoveSpeedBlows[i]->GetBlowIndex() , (int)(vlhMoveSpeedBlows[i]->GetDurationTime() * 1000) , 
								vlhMoveSpeedBlows[i]->GetValue() );
						}
					}
				}

			}
			break;

		case eActor::CS_SYNCPRESSEDPOS:
			{
				if( GetGameRoom() && GetGameRoom()->GetGameTask() && !GetGameRoom()->GetGameTask()->IsSyncComplete() ) break;
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos;
				DWORD dwGap;

				Stream.Read( &dwGap, sizeof(DWORD) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				m_pPlayerSpeedHackChecker->OnSyncDatumGap( dwGap );

				SetPosition( vPos );
				// SetMagnetPosition( vPos );
			}
			break;
		case eActor::CS_UDP_PING:
		{
			CPacketCompressStream Stream( pPacket, 128 );

			DWORD dwTick = 0;
			Stream.Read( &dwTick, sizeof(dwTick) );

			m_pSession->RecvUdpPing( dwTick );
			break;
		}

		case eActor::CS_SUMMONOFF:
			{
				CPacketCompressStream Stream( pPacket, 64 );

				DWORD dwSummonMonsterUniqueID = 0;
				Stream.Read( &dwSummonMonsterUniqueID, sizeof(DWORD) );

				CDnActor* pSummonMonster = CDnActor::FindActorFromUniqueID( GetRoom(), dwSummonMonsterUniqueID );
				if (pSummonMonster)
					pSummonMonster->CmdSuicide(false, false);
			}
			break;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
// 		case eActor::CS_TOTAL_LEVEL:
// 			{
// 				int nTotalLevel = 0;
// 
// 				CPacketCompressStream Stream( pPacket, 128 );
// 				Stream.Read( &nTotalLevel, sizeof(int) );
// 
// 				UpdateTotalLevel(nTotalLevel);
// 			}
// 			break;
		case eActor::CS_ADD_TOTAL_LEVEL_SKILL:
			{
				int nSlotIndex = -1;
				int nSkillID = 0;
				bool isInitialize = false;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nSlotIndex, sizeof(int) );
				Stream.Read( &nSkillID, sizeof(int) );		
				Stream.Read( &isInitialize, sizeof(bool) );
				
				if( nSlotIndex >= TotalLevelSkill::Common::MAXSLOTCOUNT )
					break;

				if(!m_pSession->AddTotalLevelSkillData(nSlotIndex, nSkillID, isInitialize))
				{
					break;
				}
				else
					AddTotalLevelSkill(nSlotIndex, nSkillID, isInitialize);
				
				m_pSession->SendAddTotalLevelSkill(m_pSession->GetSessionID(), nSlotIndex, nSkillID, isInitialize);

			}
			break;
		case eActor::CS_REMOVE_TOTAL_LEVEL_SKILL:
			{
				int nSlotIndex = -1;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &nSlotIndex, sizeof(int) );

				if( nSlotIndex < 0 || nSlotIndex >= TotalLevelSkill::Common::MAXSLOTCOUNT )
					break;

				RemoveTotalLevelSkill(nSlotIndex);
				m_pSession->AddTotalLevelSkillData(nSlotIndex, 0);
				m_pSession->SendDelTotalLevelSkill(m_pSession->GetSessionID(), nSlotIndex);
			}
			break;
// 		case eActor::CS_TOTAL_LEVEL_SKILL_ACTIVE_LIST:
// 			{
// 				int nCount = 0;
// 				int nSlotIndex = -1;
// 				int nSkillID = 0;
// 
// 				CPacketCompressStream Stream( pPacket, 128 );
// 				Stream.Read( &nCount, sizeof(int) );
// 
// 				for (int i = 0; i < nCount; ++i)
// 				{
// 					Stream.Read( &nSlotIndex, sizeof(int) );
// 					Stream.Read( &nSkillID, sizeof(int) );
// 
// 					ActivateTotalLevelSkillSlot(nSlotIndex, true);
// 					AddTotalLevelSkill(nSlotIndex, nSkillID);
// 				}
// 			}
// 			break;
// 		case eActor::CS_TOTAL_LEVEL_SKILL_CASHSLOT_ACTIVATE:
// 			{
// 				int nSlotIndex = -1;
// 				bool bActivate = false;
// 
// 				CPacketCompressStream Stream( pPacket, 128 );
// 				Stream.Read(&nSlotIndex, sizeof(int));
// 				Stream.Read(&bActivate, sizeof(bool));
// 
// 				ActivateTotalLevelSkillCashSlot(bActivate);
// 			}
// 			break;
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_FIX_68898)
		case eActor::CS_SKIP_END_ACTION:
			{
				bool isSkipEndAction = false;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read(&isSkipEndAction, sizeof(bool));

				SetSkipEndAction(isSkipEndAction);
			}
			break;
#endif // PRE_FIX_68898
	}
}

void CDnPlayerActor::SyncClassTime( LOCAL_TIME LocalTime )
{
	MAActorRenderBase::m_LocalTime = LocalTime;
	CDnActor::SyncClassTime( LocalTime );
}

void CDnPlayerActor::ProcessDie( LOCAL_TIME LocalTime, float fDelta )
{
	// 죽지않아~
	if( !IsDie() ) {
		ToggleGhostMode( false );
		return;
	}
	if( m_fDieDelta > 0.f ) {
		m_fDieDelta -= fDelta;
		if( m_fDieDelta < 0.f ) m_fDieDelta = 0.f;
	}
	if( !m_bGhost && m_fDieDelta < m_fMaxDieDelta - 4.f ) {
		ToggleGhostMode( true );
	}

	if( m_fDieDelta <= 0.f ) {
		OnFinishProcessDie();
	}

	// 상용 아이템에 스킬 들어갈 수 있고, 추가적으로 문장에 스킬이 들어감. 일단 그냥 악세사리 루프도 돌자.
	if( m_afLastEquipItemSkillRemainTime != 0.0f )
	{
		m_afLastEquipItemSkillRemainTime -= fDelta;
		if( m_afLastEquipItemSkillRemainTime < 0.0f )
			m_afLastEquipItemSkillRemainTime = 0.0f;
	}
}

void CDnPlayerActor::OnKillMonster(DnActorHandle hMonster)
{
	if ( !m_pSession ) return;

	if ( m_pSession->GetQuest() == NULL ) return;

	if( hMonster && hMonster->IsMonsterActor() )
	{
		CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(hMonster.GetPointer());
		if ( pMonster ) 
		{
			m_pSession->GetQuest()->OnKillMonster(pMonster->GetMonsterClassID());
			UpdateKillMonster();

			if (pMonster->IsBossKillCheck())
				UpdateKillBoss();
		}
	}
}

void CDnPlayerActor::OnStateBlowProcessAfter()
{
	if( m_uiStateBlowProcessAfterBit&eStateBlowAfterProcessType::eRebirth )
	{
		m_uiStateBlowProcessAfterBit &= ~eStateBlowAfterProcessType::eRebirth;

		if( !GetGameRoom() )
		{
			_DANGER_POINT();
			return;
		}

		GetGameRoom()->OnRebirth( GetActorHandle() );
	}
	if( m_uiStateBlowProcessAfterBit & eStateBlowAfterProcessType::eDelZombie )
	{
		m_uiStateBlowProcessAfterBit &= ~eStateBlowAfterProcessType::eDelZombie;

		if( GetGameRoom() && GetGameRoom()->bIsZombieMode() )
			static_cast<CPvPZombieMode*>(static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetPvPGameMode())->DelZombie( GetActorHandle(), true );
	}
}

void CDnPlayerActor::OnAddStateBlowProcessAfterType( eStateBlowAfterProcessType Type )
{
	m_uiStateBlowProcessAfterBit |= Type;
}

void CDnPlayerActor::OnDie( DnActorHandle hHitter )
{
#if defined( PRE_ADD_SECONDARY_SKILL )
	if( GetUserSession() && GetUserSession()->GetSecondarySkillRepository() )
		static_cast<CSecondarySkillRepositoryServer*>(GetUserSession()->GetSecondarySkillRepository())->CancelManufacture();
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	CDnActor::OnDie( hHitter );

	// hitter 쪽 버블 시스템쪽에 타격한 대상이 죽었음을 알린다. /////////////////////////////////////////////
	if(hHitter && hHitter->IsPlayerActor())
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
		boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_PLAYER_KILL_TARGET ) );
		pPlayer->Notify( pEvent );
	}
	//////////////////////////////////////////////////////////////////////////

	UpdateDead();
	AddStageDeathCount();

	if(m_pRoom && !m_pRoom->bIsPvPRoom()) {
		for( DWORD i=0; i<CDnPartyTask::GetInstance(GetRoom()).GetUserCount(); i++ ) {
			CDNGameRoom::PartyStruct *pParty = CDnPartyTask::GetInstance(GetRoom()).GetPartyData(i);
			if( !pParty || !pParty->pSession ) continue;
			if( pParty->pSession == m_pSession ) continue;
			DnActorHandle hActor = pParty->pSession->GetActorHandle();
			if( !hActor ) continue;
			CDnPlayerActor *pPlayer = (CDnPlayerActor *)hActor.GetPointer();
			pPlayer->UpdatePartyMemberDead();
		}
	}

	// 현재 발동중인 스킬을 모두 종료,
	// 부활하고나면 패시브 스킬은 다시 걸어줘야 함..
	if( m_hProcessSkill )
		m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.0f );

	// 발동중인 오토 패시브 스킬이 있다면 종료
	EndAutoPassiveSkill( CDnActionBase::m_LocalTime, 0.0f );

	// 접두어 스킬 종료
	EndPrefixSystemSkill(CDnActionBase::m_LocalTime, 0.0f);

	// #14340 으로 인해.. 죽었을 때 패시브 스킬은 없애지 않는 것으로 수정됨.

	// 오라스킬, 토글스킬이 켜져 있다면 꺼준다.
	if( IsEnabledToggleSkill() )
		OnSkillToggle( m_hToggleSkill, false );

	if( IsEnabledAuraSkill() )
		OnSkillAura( m_hAuraSkill, false );

	// 나머지, 다른 플레이어나 몹에 의해 걸려 있는 상태효과 모두 없앤다.
	RemoveAllBlowExpectPassiveSkill();

	// 인벤토리 아이템 쿨타임 초기화 해준다.
	if( m_pSession && m_pSession->GetItem() )
	{
		//m_pSession->GetItem()->ResetCoolTime();
	}

	SetSP(0);

	bool bIgnoreDuration = false;

	if (bIgnoreDuration == false)
	{
		// 내구도 감소시켜준다.
		bool bDecreaseDurability = true;
		if( m_pRoom ) {
			if( m_pRoom->bIsPvPRoom() ) bDecreaseDurability = false;
			else if( m_pRoom->bIsDLRoom() ) bDecreaseDurability = false;
			else if( m_pRoom->bIsFarmRoom() ) bDecreaseDurability = false;
		}
		if( bDecreaseDurability ) OnDecreaseEquipDurability( GetDeadDurabilityRatio(), true );
	}

	if( GetGameRoom() ) 
	{
		GetGameRoom()->OnDie( GetActorHandle(), hHitter );
		if( GetGameRoom()->GetGameTask() )
			GetGameRoom()->GetGameTask()->OnDie( GetActorHandle(), hHitter );
	}

	if( IsCannonMode() )
	{
		static_cast<CDnCannonMonsterActor*>(m_hCannonMonsterActor.GetPointer())->OnMasterPlayerActorDie();
		EndCannonMode();
	}

	m_pBubbleSystem->Clear();

	if( CDnWorld::IsActive(GetRoom()) )
	{
		CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "DieActionPlayer", GetUniqueID() );
		CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnPlayerActor::OnDie", CDnActionBase::m_LocalTime, 0.f );
	}
}

void CDnPlayerActor::OnFinishProcessDie()
{
	if( GetGameRoom() )
		GetGameRoom()->OnFinishProcessDie( GetActorHandle() );
}

void CDnPlayerActor::ResetActor()
{
	CDnActor::ResetActor();

	m_cMovePushKeyFlag = 0;
	m_LastEscapeTime = 0;
	if( IsProcessSkill() ) CancelUsingSkill();

	// #26902 임시로 추가된 스킬이 있다면 삭제. 클라한테는 패킷으로 날라감.
	if( IsTempSkillAdded() )
	{
		RemoveAllTempSkill();
		EndAddTempSkillAndSendRestoreTempJobChange();
	}
	if( m_pPlayerSpeedHackChecker ) m_pPlayerSpeedHackChecker->ResetInvalid();

	map<int, DnSkillHandle>::iterator iter = m_mapEnchantSkillFromBubble.begin();
	for( iter; iter != m_mapEnchantSkillFromBubble.end();  )
	{
		SAFE_RELEASE_SPTR( iter->second );
		iter = m_mapEnchantSkillFromBubble.erase( iter );
	}

	m_MixedActionTimeStamp = 0;
}

void CDnPlayerActor::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	Sphere.Center = m_Cross.m_vPosition;
	Sphere.Center.y += 50.f;
	Sphere.fRadius = 50.f;
}

void CDnPlayerActor::SetBattleMode( bool bEnable )
{
	m_bBattleMode = bEnable;
}

bool CDnPlayerActor::IsCanBattleMode()
{
	if( IsSwapSingleSkin() ) return false;
	if( m_bBattleMode && !m_hWeapon[0] ) return false;
	if( IsSpectatorMode() ) return false;

	return m_bBattleMode;
}

int CDnPlayerActor::GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel )
{
	if( nPrevLevel == nCurLevel ) return 0;

	int nSkillPoint = 0;
	for( int i=nPrevLevel+1; i<=nCurLevel; i++ ) {
		int nItemID = ( ( GetClassID() - 1 ) * PLAYER_MAX_LEVEL ) + i;
		nSkillPoint += CPlayerLevelTable::GetInstance().GetValue( GetJobClassID(), i, CPlayerLevelTable::SkillPoint );
	}
	return nSkillPoint;
}

void CDnPlayerActor::OnBattleToggle( bool bBattle )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &bBattle, sizeof(bool) );

	Send( eActor::SC_CMDTOGGLEBATTLE, &Stream );
}

void CDnPlayerActor::OnLevelUp( int nLevel, int nLevelUpAmount )
{
	if( !IsDie() ) {
		SetHP( GetMaxHP() );
		SetSP( GetMaxSP() );
	}
	int nSkillPoint = GetLevelUpSkillPoint( nLevel - nLevelUpAmount, nLevel );
	//레벨업시에는 스킬포인트 두군데 모두 증가
	m_pSession->ChangeSkillPoint(nSkillPoint, 0, true, DBDNWorldDef::SkillPointCode::LevelUp, DualSkill::Type::Primary);
	m_pSession->ChangeSkillPoint(nSkillPoint, 0, true, DBDNWorldDef::SkillPointCode::LevelUp, DualSkill::Type::Secondary);

	m_pSession->SetLevel(m_nLevel, DBDNWorldDef::CharacterLevelChangeCode::Normal, true);
	m_pSession->SetExp(m_nExperience, DBDNWorldDef::CharacterExpChangeCode::Dungeon, 0, true);

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nLevel, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &m_nExperience, sizeof(int) );

	Send( eActor::SC_LEVELUP, &Stream );

#if !defined(PRE_DELETE_DUNGEONCLEAR)
	// 던전 저장 관련 데이타 여기서 한번 리플레쉬 해서 필요없는것들 지워주게 하자
	m_pSession->RefreshDungeonEnterLevel();
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

	// Refresh 해보고 게이트의 변경사항이 생겼으면 게이트인포를 보낸다.
	if( CDnPartyTask::IsActive(GetRoom()) ) CDnPartyTask::GetInstance(GetRoom()).UpdateGateInfo();
	
	// 레벨에 따라 스킬 소모 SP 가 다르기땜시 언제나 리플레쉬 해줘야한다.
	for( DWORD i=0; i<GetSkillCount(); i++ ) {
		DnSkillHandle hSkill = GetSkillFromIndex(i);
		if( hSkill ) hSkill->RefreshDecreaseMP();
	}

	m_pSession->NotifyGuildMemberLevelUp(nLevel);

	m_pSession->GetEventSystem()->OnEvent( EventSystem::OnLevelUp );

	// 사제 졸업
	if( m_pSession->GetMasterSystemData()->SimpleInfo.iMasterCount > 0 && nLevel >= static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::MasterSystem_GraduateLevel )) )
	{
		if( m_pSession->CheckDBConnection() )
			m_pSession->GetDBConnection()->QueryMasterSystemGraduate( m_pSession );
	}
	
	if( CDnWorld::GetInstance(GetRoom()).GetMapType() == EWorldEnum::MapTypeDungeon && m_pSession->GetPeriodExpItemRate() > 0 )
		m_pSession->SetPeriodExpItemRate();

#if defined( PRE_ADD_BESTFRIEND )
	m_pSession->BestFriendChangeLevel(nLevel, true);
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	UpdateTotalLevelByCharLevel();
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
}

void CDnPlayerActor::OnAddExperience( int nAddExperience, int nLogCode, INT64 biFKey )
{
	m_pSession->ChangeExp(nAddExperience, nLogCode, biFKey);	// db저장 안에있음
	// 경험치는 마출필요 없다.
}

void CDnPlayerActor::OnBeginStateBlow( DnBlowHandle hBlow )
{
	// 나중에 고치자..
	DNVector(DnActorHandle) hVecList;
	ScanActor( GetRoom(), m_Cross.m_vPosition, 2000.f, hVecList );
	
	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		DnActorHandle hActor = hVecList[i];
		if( !hActor ) 
			continue;

		CDNAggroSystem* pAggroSystem = hActor->GetAggroSystem();
		// 일반던젼 플레이시에는 PlayerActor 는 AggroSystem 이 없다.
		if( !pAggroSystem )
			continue;

		if( !pAggroSystem->bOnCheckPlayerBeginStateBlow( this ) )
			continue;

		pAggroSystem->OnStateBlowAggro( hBlow );
	}
}


void CDnPlayerActor::UnLockSkill( int nSkillID, INT64 nUnlockPrice/*=0*/ )
{
	// 이미 언락 되어있는 스킬이 또 들어오면 안되고..
	vector<int>::iterator iter = find( m_vlUnlockZeroLevelSkills.begin(), m_vlUnlockZeroLevelSkills.end(), nSkillID );
	_ASSERT( m_vlUnlockZeroLevelSkills.end() == iter );

	if( m_vlUnlockZeroLevelSkills.end() == iter )
	{
		m_vlUnlockZeroLevelSkills.push_back( nSkillID );

		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
		SkillInfo.iSkillID = nSkillID;
		SkillInfo.iSkillLevel = 0;
		SkillInfo.bCurrentLock = false;
		m_vlPossessedSkill.push_back( SkillInfo );

		INT64 biCurrentCoin = 0;
		INT64 biPickUpCoin = 0;
		
		if( nUnlockPrice > 0 )
		{
			biCurrentCoin = m_pSession->GetCoin();
			biPickUpCoin = m_pSession->GetPickUpCoin();
			m_pSession->SelPickUpCoin(0);
		}

		m_pSession->GetDBConnection()->QueryAddSkill(m_pSession, nSkillID, SkillInfo.iSkillLevel, 0, DBDNWorldDef::SkillChangeCode::GainByBuy, nUnlockPrice, biCurrentCoin, biPickUpCoin);		
	}
}

int CDnPlayerActor::CanAcquireSkillIfUnlock( int nSkillID )
{
	int nRetCode = ERROR_NONE;

	CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( GetPossessedSkillInfo() );
	CDnSkillTreeSystem::S_OUTPUT Output;

	TryAcquire.iCurrentCharLevel = GetLevel();
	TryAcquire.iTryAcquireSkillID = nSkillID;
	TryAcquire.iHasSkillPoint = GetAvailSkillPointByJob( nSkillID );

	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int iNeedJob = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NeedJob" )->GetInteger();
	if( IsPassJob( iNeedJob ) )
	{
		pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

		// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
		bool bAlreadyGlobalSkillAcquired = HasSameGlobalIDSkill( nSkillID );
		bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
											 (CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
											 (CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
											 true == bAlreadyGlobalSkillAcquired;

		if( CDnSkillTreeSystem::R_SUCCESS != Output.eResult &&
			false == bIgnoreParentSkillCondition )
		{
			switch( Output.eResult )
			{
					// 캐릭터 요구레벨이 모자람.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
					break;

					// 선행(부모) 스킬이 없음.
				case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
					break;

					// 부모 스킬의 레벨이 충족되지 않음.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
					break;

					// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
					break;
			}
		}
	}

	return nRetCode;
}

int CDnPlayerActor::AcquireSkill( int nSkillID )
{
	int nRetCode = ERROR_NONE;

	vector<int>::iterator iter = find( m_vlUnlockZeroLevelSkills.begin(), m_vlUnlockZeroLevelSkills.end(), nSkillID );
	_ASSERT( m_vlUnlockZeroLevelSkills.end() != iter );

	if( m_vlUnlockZeroLevelSkills.end() != iter )
	{
		// 서로 못배우게 한 스킬은 못배우게 처리.
		TSkillData* pSkillData = g_pDataManager->GetSkillData( nSkillID );
		if( IsExclusiveSkill( nSkillID, pSkillData->nExclusiveID ) )
		{
			// 클라에서 기본적으로 막기 때문에 여기까지 오면 핵이다.
			return ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE;
		}

		CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
		CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( GetPossessedSkillInfo() );
		CDnSkillTreeSystem::S_OUTPUT Output;

		TryAcquire.iCurrentCharLevel = GetLevel();
		TryAcquire.iTryAcquireSkillID = nSkillID;
		TryAcquire.iHasSkillPoint = GetAvailSkillPointByJob( nSkillID );

		DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		int iNeedJob = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NeedJob" )->GetInteger();
		if( IsPassJob( iNeedJob ) )
		{
			//TryAcquire.iJobID = iNeedJob;
			pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			std::vector<int> nNeedSPValues;
			CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>(CTaskManager::GetInstance( m_pSession->GetGameRoom() ).GetTask( "SkillTask" ));
			if (pSkillTask)
				pSkillTask->GetNeedSPValuesByJob(nSkillID, nNeedSPValues);

			std::vector<int> jobHistory;
			GetJobHistory(jobHistory);

			bool bAvailableSPByJob = false;
			if (pSkillTask)
				bAvailableSPByJob = pSkillTask->IsAvailableSPByJob(jobHistory, nNeedSPValues, this);

			if (bAvailableSPByJob == false)
				Output.eResult = CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE;
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

			// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
			bool bAlreadyGlobalSkillAcquired = HasSameGlobalIDSkill( nSkillID );
			bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
				(CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
				(CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
				true == bAlreadyGlobalSkillAcquired;

			if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult ||
				true == bIgnoreParentSkillCondition )
			{			
				// 내부에서 직접 레벨 1로 바꿈.
				//vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill.begin();
				//for( iter; m_vlPossessedSkill.end() != iter; ++iter )
				//{
				//	if( iter->iSkillID == nSkillID )
				//	{
				//		iter->iSkillLevel = 1;
				//		break;
				//	}
				//}

				bool bSuccess = AddSkill( nSkillID, 1 );
				_ASSERT( bSuccess );
				DnSkillHandle hAcquiredSkill = FindSkill( nSkillID );
				m_pSession->ChangeSkillPoint( -hAcquiredSkill->GetNowLevelSkillPoint(), nSkillID, false, 0 );

				m_pSession->GetDBConnection()->QueryModSkillLevel(m_pSession, nSkillID, 1, 0, -hAcquiredSkill->GetNowLevelSkillPoint(), DBDNWorldDef::SkillChangeCode::GainByBuy);	// db저장: 스킬포인트까지 같이 업데이트
			}
			else
			{
				switch( Output.eResult )
				{
						// 캐릭터 요구레벨이 모자람.
					case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
						nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
						break;

						// 선행(부모) 스킬이 없음.
					case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
						nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
						break;

						// 부모 스킬의 레벨이 충족되지 않음.
					case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
						nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
						break;

						// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
					case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
						nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
						break;
				}

			}
		}
	}

	return nRetCode;
}

void CDnPlayerActor::CheckAndRegisterObserverStateBlow( DnBlowHandle hBlow )
{
	switch( hBlow->GetBlowIndex() )
	{
			// 블록 상태효과
		case STATE_BLOW::BLOW_030:
			{
				CDnBlockBlow* pObservable = static_cast<CDnBlockBlow*>( hBlow.GetPointer() );
				pObservable->RegisterObserver( m_pBubbleSystem );
			}
			break;

			// 패링 상태효과
		case STATE_BLOW::BLOW_031:
			{
				CDnParryBlow* pObservable = static_cast<CDnParryBlow*>( hBlow.GetPointer() );
				pObservable->RegisterObserver( m_pBubbleSystem );
			}
			break;

			// 쿨타임 패링 상태효과.
		case STATE_BLOW::BLOW_153:
			{
				CDnCooltimeParryBlow* pObservable = static_cast<CDnCooltimeParryBlow*>( hBlow.GetPointer() );
				pObservable->RegisterObserver( m_pBubbleSystem );
			}
			break;
	}
}

void CDnPlayerActor::OnApplyPassiveSkillBlow( int iBlowID )
{
	DnBlowHandle hBlow = m_pStateBlow->GetStateBlowFromID( iBlowID );

	if( hBlow )
		CheckAndRegisterObserverStateBlow( hBlow );
}

bool CDnPlayerActor::CanAddSkill( int nSkillTableID, int nLevel /*= 1*/ )
{
	TSkillData* pSkillData = g_pDataManager->GetSkillData( nSkillTableID );
	if( pSkillData == NULL )
		return false;
	if( GetGameRoom() == NULL )
		return false;
	if( GetGameRoom()->bIsZombieMode() == true )
	{
		if( CDnSkill::Passive == pSkillData->cSkillType && CDnSkill::DurationTypeEnum::Buff == pSkillData->cDurationType )
			return false;
	}
	return true;
}

bool CDnPlayerActor::AddSkill( int nSkillTableID, int nLevel /* = 1 */, int iSkillLevelApplyType/* = CDnSkill::PVE*/ )
{
	bool bSuccess = false;

	if( 0 < nLevel )
	{
		// 스킬 레벨 데이터를 pve/pvp 인 경우와 나눠서 셋팅해준다.
		int iSkillLevelDataType = CDnSkill::PVE;
		if( GetGameRoom()->bIsPvPRoom() )
			iSkillLevelDataType = CDnSkill::PVP;

		bSuccess = MASkillUser::AddSkill( nSkillTableID, nLevel, iSkillLevelDataType );

		vector<int>::iterator iter = find( m_vlUnlockZeroLevelSkills.begin(), m_vlUnlockZeroLevelSkills.end(), nSkillTableID );
		if( m_vlUnlockZeroLevelSkills.end() != iter )
			m_vlUnlockZeroLevelSkills.erase( iter );
	}
	else
	{
		// 처음에 DB 로부터 스킬 리스트 받을 때 레벨이 0 이면 이쪽으로 들어온다.
		// 언락만 되어있고 실제로 갖고있지는 않은 스킬.
		m_vlUnlockZeroLevelSkills.push_back( nSkillTableID );

		bSuccess = true;
	}

	//if( bSuccess )
	//{
	//	bool bFinded = false;
	//	int iNumPossessed = (int)m_vlPossessedSkill.size();
	//	for( int i = 0; i < iNumPossessed; ++i )
	//	{
	//		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO& PossessedSkill = m_vlPossessedSkill.at( i );
	//		if( PossessedSkill.iSkillID == nSkillTableID )
	//		{
	//			PossessedSkill.iSkillLevel = nLevel;
	//			bFinded = true;
	//		}
	//	}

	//	if( false == bFinded )
	//	{
	//		// unlock 만 된 레벨 0 짜리 스킬도 감안하기 때문에 같이 리스트에 넣어준다.
	//		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
	//		SkillInfo.iSkillID = nSkillTableID;
	//		SkillInfo.iSkillLevel = nLevel;
	//		SkillInfo.bCurrentLock = false;
	//		m_vlPossessedSkill.push_back( SkillInfo );
	//	}
	//}

	return bSuccess;
}


bool CDnPlayerActor::ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta )
{
	ResetCustomAction();

	// 오라 스킬이 꺼지는 것으로 토글링 될 땐 m_hAuraSkill 없어짐.
	// CDnActor::OnSkillExecute( LocalTime, fDelta ); 
	DnSkillHandle hNowSkill;
	if( m_hAuraSkill )
		hNowSkill = m_hAuraSkill;

	bool bResult = CDnActor::ExecuteSkill( hSkill, LocalTime, fDelta );

	if( m_hProcessSkill )
		hNowSkill = m_hProcessSkill;
	else
	if( m_hAuraSkill )
		hNowSkill = m_hAuraSkill;

	if( bResult && hNowSkill ) 
	{
		UpdateUseSkill( hNowSkill );

		// 일단 인스턴트만 아니면 무조건 이벤트 발생시킨다. 추후에 오오라나 타임토글 같은 경우도 필요없다면
		// 이쪽에서 또 걸러준다.
		switch( hNowSkill->GetDurationType() ) {
			case CDnSkill::Instantly:
				break;
			default:
				{
					if( m_pSession && m_pSession->GetMissionSystem() )
					{
						m_pSession->GetEventSystem()->OnEvent( EventSystem::OnSkillUse, 2,
																 EventSystem::SkillID, hNowSkill->GetClassID(),
																 EventSystem::SkillLevel, hNowSkill->GetLevel() );
					}
				}
				break;
		}
	}


#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
	if( bResult && CDnDPSReporter::IsActive() )
	{
		if(CDnDPSReporter::GetInstance().IsEnabledUser( GetCharacterDBID() ))
		{
			DNVector(DnActorHandle) hVecList;
			ScanActor( GetRoom(), *GetPosition() , 500.f , hVecList );
			CDnDPSReporter::GetInstance().ReportSkillInfo( hSkill , (int)hVecList.size());
		}
	}
#endif

	return bResult;
}


void CDnPlayerActor::OnHitSuccess( LOCAL_TIME LocalTime, DnActorHandle hActor, HitStruct *pStruct )
{
	// #33265 by kalliste
	// #if defined( PRE_ADD_LOTUSGOLEM )
	// 	if( hActor->GetHitParam()->bIgnoreShowDamage == true )
	// 	{
	// 		m_nComboDelay = -1;
	// 		return;
	// 	}
	// #endif // #if defined( PRE_ADD_LOTUSGOLEM )

	// #12170 콤보 딜레이 값이 0인 hit 는 콤보 판정에 아무런 영향을 주지 않도록 처리.
	if( m_bAllowCalcCombo && 
		0 < pStruct->nComboDelay )		
	{
		if( m_nComboDelay > 0 ) {
#if defined( PRE_ADD_LOTUSGOLEM )
			if( hActor->GetHitParam()->bIgnoreShowDamage == false )
				m_nComboCount++;
#else
			m_nComboCount++;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )

			if( m_nComboCount > m_nTotalComboCount )
				m_nTotalComboCount = m_nComboCount;

			UpdateMaxCombo( m_nComboCount );
		}
		else {
			m_nComboCount = 1;
		}
		m_nComboDelay = pStruct->nComboDelay;
		if( m_nComboDelay == 0 ) {
			m_nComboCount = 0;
		}
	}

	switch( hActor->GetHitParam()->HitType ) {
		case CDnWeapon::Critical: UpdateCriticalHit(); break;
		case CDnWeapon::Stun: UpdateStunHit(); break;
	}

	CheckNormalHitSEProcessor( hActor, *hActor->GetHitParam() );

	if( hActor && hActor->IsHit() && pStruct->szTargetHitAction != NULL )
	{
		if( IsAppliedThisStateBlow(STATE_BLOW::BLOW_246) )
		{
			DNVector(DnBlowHandle) vlBlows;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_246, vlBlows );
			for( DWORD i=0; i<vlBlows.size(); i++ )
			{
				if( vlBlows[i] )
				{
					const CDnSkill::SkillInfo* pSkillInfo = vlBlows[i]->GetParentSkillInfo();
					if( pSkillInfo )
					{
						DnActorHandle hHitter = pSkillInfo->hSkillUser;
						if( hHitter == hActor )
						{
							vlBlows[i]->SetState(STATE_BLOW::STATE_END);
							SendRemoveStateEffectFromID( vlBlows[i]->GetBlowID() );
						}
					}
				}
			}
		}
	}
}

// 이 액터가 쏜 발사체가 명중되었을 때.
void CDnPlayerActor::OnHitProjectile( LOCAL_TIME LocalTime, DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam )
{
	CheckNormalHitSEProcessor( hHittedTarget, HitParam );
}

void CDnPlayerActor::CheckNormalHitSEProcessor( DnActorHandle hHittedTarget, const CDnDamageBase::SHitParam& HitParam )
{
	// #23818 무기에 붙은 아이템 접두어 스킬을 처리하기 위한 "평타" 를 구분하기 위한 코드.
	// 평타인 경우엔 평타로 대상에게 상태효과를 부여하는 발현타입이 있는지 확인해서 처리.
	if( hHittedTarget )
	{
		// 내가 스킬을 쓰고 있지 않을 때만.
		if( ( false == IsProcessSkill() || false == HitParam.bFromProjectileSkill) )
		{
			int iNumApplySEProcessor = (int)m_vlpApplySEWhenNormalHitProcessor.size();
			for( int i = 0; i < iNumApplySEProcessor; ++i )
			{
				CDnApplySEWhenTargetNormalHitProcessor* pProcessor = static_cast<CDnApplySEWhenTargetNormalHitProcessor*>( m_vlpApplySEWhenNormalHitProcessor.at( i ) );
				pProcessor->OnNormalHitSuccess( hHittedTarget );
			}
		}
	}
}

void CDnPlayerActor::OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct )
{
	CDnActor::OnHitFinish( LocalTime, pStruct );
	/*
	if( m_nComboDelay > 0 && pStruct->bFinish ) {
		for( DWORD i=0; i<m_hVecLastHitList.size(); i++ ) {
			if( m_hVecLastHitList[i] && m_hVecLastHitList[i]->IsAir() ) {
				m_nChainCount++;
				UpdateChainCombo();

				char pBuffer[2];
				CPacketCompressStream Stream( pBuffer, sizeof(pBuffer) );
				Stream.Write( &m_nChainCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Send( eActor::SC_CHAIN, &Stream );
				break;
			}
		}
	}
	*/

	int nDieCount = 0;
	for( DWORD i=0; i<m_hVecLastHitList.size(); i++ ) {
		if( m_hVecLastHitList[i] && m_hVecLastHitList[i]->IsDie() ) nDieCount++;
	}
	
	UpdateMissionByMonsterKillCount(nDieCount);
}

void CDnPlayerActor::UpdateMissionByMonsterKillCount(int nCount)
{
	if( nCount >= 3 ) UpdateGenocide();

	if( nCount >= 2 && m_pSession )
	{
		m_pSession->GetEventSystem()->OnEvent( EventSystem::OnKillMonster, 1,
			EventSystem::GenocideCount, nCount );
	}
}

void CDnPlayerActor::SaveUserData(TUserData &UserData)
{
#ifndef PRE_FIX_SKILLLIST
	memset( UserData.Skill.SkillList, 0, sizeof(UserData.Skill.SkillList) );

	if( static_cast<CDNGameRoom*>(GetRoom())->bIsPvPRoom() == false )
	{
		int iIndex = 0;
		for( UINT i=0 ; i<m_vlhSkillList.size() ; ++i )
		{
			DnSkillHandle hSkill = m_vlhSkillList[i];
			if( !hSkill )
				continue;
			UserData.Skill.SkillList[iIndex].nSkillID		= hSkill->GetClassID();
			UserData.Skill.SkillList[iIndex].cSkillLevel	= hSkill->GetLevel();
			UserData.Skill.SkillList[iIndex].nCoolTime		= static_cast<int>(hSkill->GetElapsedDelayTime()*1000);
			++iIndex;
		}
	}
#else
	memset( UserData.Skill[0].SkillList, 0, sizeof(UserData.Skill[0].SkillList) );

	if( static_cast<CDNGameRoom*>(GetRoom())->bIsPvPRoom() == false )
	{
		int iIndex = 0;
		DWORD dwNumSkill = GetSkillCount();
		for( DWORD i = 0; i < dwNumSkill; ++i )
		{
			DnSkillHandle hSkill = GetSkillFromIndex( i );
			if( !hSkill )
				continue;
			UserData.Skill[0].SkillList[iIndex].nSkillID		= hSkill->GetClassID();
			UserData.Skill[0].SkillList[iIndex].cSkillLevel	= hSkill->GetLevel();
			UserData.Skill[0].SkillList[iIndex].nCoolTime		= static_cast<int>(hSkill->GetElapsedDelayTime()*1000);
			++iIndex;
		}
	}	
#endif // #ifndef PRE_FIX_SKILLLIST

	UserData.Status.nGlyphDelayTime = (int)(m_afLastEquipItemSkillDelayTime * 1000.f);
	UserData.Status.nGlyphRemainTime = (int)(m_afLastEquipItemSkillRemainTime * 1000.f);
	if( GetUserSession() && m_nInvalidPlayerCheckCounter > 0 ) 
	{
		g_Log.Log( LogType::_HACK, GetUserSession(), L"HackChecker(DB) : CharName=%s Counter=%d\n", GetUserSession()->GetCharacterName(), m_nInvalidPlayerCheckCounter );
		m_pSession->GetDBConnection()->QueryAddAbuseMonitor( m_pSession, m_nInvalidPlayerCheckCounter, 0 );
		m_nInvalidPlayerCheckCounter = 0;
	}
}

void CDnPlayerActor::SetUserSession(CDNUserSession * pSession) 
{ 
	m_pSession = pSession; 
	m_pPartyData = pSession->GetGameRoom()->GetPartyData( m_pSession );

	// EquipDelayTime, EquipRemainTime 세팅 여기서 그냥 해준다;;
	m_afLastEquipItemSkillDelayTime = (float)m_pSession->GetGlyphDelayTime() / 1000.f;
	m_afLastEquipItemSkillRemainTime = (float)m_pSession->GetGlyphRemainTime() / 1000.f;

	// 현재 장비 슬롯에 매치되는 스킬이 있다면 쿨타임 셋팅해줌.
	if( m_ahEquipSkill )
		m_ahEquipSkill->SetOnceCoolTime( m_afLastEquipItemSkillDelayTime, m_afLastEquipItemSkillRemainTime );

	SetAccountLevel(m_pSession->GetAccountLevel());
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

void CDnPlayerActor::OnComboFinish( int nCombo )
{
	if( nCombo > 1 ) {
		UpdateCombo( nCombo );
	}
}

bool CDnPlayerActor::IsGMTrace() const
{
	if( m_pSession && m_pSession->bIsGMTrace() )
		return true;

	return false;
}

UINT CDnPlayerActor::GetSessionID()
{
	return m_pSession ? m_pSession->GetSessionID() : 0;	
}

int CDnPlayerActor::GetMoveSpeed()
{
	int nMoveSpeed = CDnActor::GetMoveSpeed();

	if( CDnWorld::IsActive(GetRoom()) && ( CDnWorld::GetInstance(GetRoom()).GetMapType() == EWorldEnum::MapTypeVillage || CDnWorld::GetInstance(GetRoom()).GetMapType() == EWorldEnum::MapTypeWorldMap ) )
		nMoveSpeed += GetSafeZoneMoveSpeed();

	if( IsTransformMode() )
	{
		nMoveSpeed = g_pDataManager->GetMonsterMutationMoveSpeed( m_nMonsterMutationTableID );
		if( m_cMovePushKeyFlag & 0x08 )
			 nMoveSpeed /= 2;
		return nMoveSpeed;
	}

	if( m_cMovePushKeyFlag & 0x08 ) nMoveSpeed /= 2;
	// #62481 월드존 이동속도를 마을과 동일하게 변경
	if( !IsGhost() && !IsBattleMode() ) nMoveSpeed = (int)( nMoveSpeed * 1.4f );
	if( IsDie() && IsGhost() ) nMoveSpeed = (int)( nMoveSpeed * 1.5f );
	return nMoveSpeed;
}

void CDnPlayerActor::OnAddSkill( DnSkillHandle hSkill, bool isInitialize/* = false*/ )
{
	CDnActor::OnAddSkill( hSkill, isInitialize );

	if( hSkill )
	{
		bool bFinded = false;
		int iNumPossessed = (int)m_vlPossessedSkill.size();
		for( int i = 0; i < iNumPossessed; ++i )
		{
			CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO& PossessedSkill = m_vlPossessedSkill.at( i );
			if( PossessedSkill.iSkillID == hSkill->GetClassID() )
			{
				PossessedSkill.iSkillLevel = hSkill->GetLevel();
				bFinded = true;
			}
		}

		if( false == bFinded )
		{
			// unlock 만 된 레벨 0 짜리 스킬도 감안하기 때문에 같이 리스트에 넣어준다.
			CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
			SkillInfo.iSkillID = hSkill->GetClassID();
			SkillInfo.iSkillLevel = hSkill->GetLevel();
			SkillInfo.bCurrentLock = false;
			m_vlPossessedSkill.push_back( SkillInfo );
		}
	}

	// 추가된 스킬이 강화 패시브 스킬인 경우.(강화 패시브 스킬을 획득 했거나 다른 플레이어가 레벨업 한 경우)
	if( hSkill->GetSkillType() == CDnSkill::EnchantPassive )
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
}


void CDnPlayerActor::OnRemoveSkill( DnSkillHandle hSkill )
{
	CDnActor::OnRemoveSkill( hSkill );

	if( hSkill )
	{
		vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill.begin();
		for( iter; iter != m_vlPossessedSkill.end(); ++iter )
		{
			if( iter->iSkillID == hSkill->GetClassID() )
			{
				m_vlPossessedSkill.erase( iter );
				break;
			}
		}
	}

	// 다른 플레이어의 패시브 강화 스킬이 레벨업 되어 이전 레벨의 스킬 객체가 삭제 루틴을 타고 이쪽으로 오는 경우.
	// 적용되고 있던 베이스 스킬의 강화 상태를 리셋으로 돌린다.
	if( CDnSkill::EnchantPassive == hSkill->GetSkillType() &&
		0 < hSkill->GetBaseSkillID() )
	{
		DnSkillHandle hBaseSkill = FindSkill( hSkill->GetBaseSkillID() );
		if( hBaseSkill )
		{
#if defined(PRE_FIX_64312)
			//소환몬스터용 스킬이 경우 바로 적용 하지 않고 담아 놓고, MAAiSkill에서 UseSkill시점에 적용 한다.
			bool isSummonMonsterSkill = false;

			isSummonMonsterSkill = hBaseSkill->IsSummonMonsterSkill();

			if (isSummonMonsterSkill == false)
				hBaseSkill->ReleaseEnchantSkill();
			else
				hBaseSkill->RemoveSummonMonsterEnchantSkill();
#else
			hBaseSkill->ReleaseEnchantSkill();
#endif // PRE_FIX_64312
		}
	}

	// 다른 플레이어의 패시브 강화 스킬의 대상이 되는 베이스 스킬이 레벨업 되어 삭제 루틴을 타고 이쪽으로 오는 경우.
	// 이 경우엔 베이스 스킬 객체를 그냥 삭제하면 되므로 따로 처리할 것은 없다.
}

void CDnPlayerActor::ProcessRecoverySP( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDie() ) {
		m_fRecoverySPDelta = 0.f;
		return;
	}
	m_fRecoverySPDelta += fDelta;

	int nValue = GetSP();
	if( m_fRecoverySPDelta >= s_fRecoverySPTime ) {
		m_fRecoverySPDelta -= s_fRecoverySPTime;
		nValue += GetRecoverySP();
	}
	else return;

	if( GetSP() >= GetMaxSP() ) return;

	nValue = min( nValue, GetMaxSP() );
	SetSP( nValue );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nValue, sizeof(int) );

	Send( eActor::SC_RECOVERYSP, &Stream );
}

void CDnPlayerActor::CmdToggleWeaponViewOrder( int nEquipIndex, bool bShowCash )
{
	SetWeaponViewOrder( nEquipIndex, bShowCash );
	RefreshWeaponViewOrder( nEquipIndex );

	if( m_pSession )
	{
		m_pSession->SetViewCashEquipBitmap(CASHEQUIP_WEAPON1 + nEquipIndex, bShowCash);
	}
}

void CDnPlayerActor::CmdTogglePartsViewOrder( int nEquipIndex, bool bShowCash )
{
	if ((nEquipIndex < 0)  || (nEquipIndex >= _countof(m_bPartsViewOrder))) 
		return;

	SetPartsViewOrder( nEquipIndex, bShowCash );
	RefreshPartsViewOrder( nEquipIndex );

	if( m_pSession )
		m_pSession->SetViewCashEquipBitmap(nEquipIndex, bShowCash);
}

void CDnPlayerActor::CmdToggleHideHelmet( bool bHideHelmet )
{
	if( m_pSession )
		m_pSession->SetViewCashEquipBitmap(HIDEHELMET_BITINDEX, bHideHelmet);
}

void CDnPlayerActor::CmdChangeJob( int nJobID )
{
	SetJobHistory( nJobID );
	OnChangeJob( nJobID );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nJobID, sizeof(int) );

	Send( eActor::SC_CHANGEJOB, m_pSession->GetSessionID(), &Stream );
}

void CDnPlayerActor::CmdEscape( EtVector3 &vPos )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

	Send( eActor::SC_CMDESCAPE, &Stream );
}

void CDnPlayerActor::ToggleGhostMode( bool bGhost )
{
	if( bGhost == m_bGhost ) return;
	m_bGhost = bGhost;

	if( bGhost == true && GetSP() > 0 )
		SetSP(0);

#ifdef PRE_FIX_GAMESERVER_USE_GHOST_MODE
	bool bGhostMode = true;
	CDnGameTask* pGameTask = m_pSession->GetGameRoom()->GetGameTask();
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP ) 
	{
		bGhostMode = false;
		CDNGameRoom* pGameRoom = GetGameRoom();
#if defined(PRE_ADD_PVP_TOURNAMENT)
		if( pGameRoom && pGameRoom->GetPvPGameMode() && (pGameRoom->GetPvPGameMode()->bIsAllKillMode() || pGameRoom->GetPvPGameMode()->bIsTournamentMode()))
#else
		if( pGameRoom && pGameRoom->GetPvPGameMode() && pGameRoom->GetPvPGameMode()->bIsAllKillMode() )
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
			bGhostMode= true;
	}

	if( !bGhostMode ) 
	{
		if( bGhost ) 
		{
			if( IsMovable() || IsStay() ) 
			{
				SetAction( "Die", 0.f, 0.f, false );
			}
		}
		else 
		{
			std::string szActionName = "Stand";
			if(m_bShootMode)
				szActionName = "MOD_Stand";

			if(IsTransformMode())
			{
				if(IsExistAction( "Summon_On" ))
					szActionName = "Summon_On";
			}
			SetAction( szActionName.c_str(), 0.f, 0.f );
		}
	}
	else 
	{
		if( bGhost ) 
		{
			SwapSingleSkin( 499 + m_nClassID );
			SetAction( "Stand", 0.f, 0.f );
		}
		else
		{
			SwapSingleSkin( -1 );
			SetAction( "Stand", 0.f, 0.f );
		}
	}
#endif

	if( m_bGhost && GetGameRoom() && GetGameRoom()->GetGameTask() )
		GetGameRoom()->GetGameTask()->OnGhost( GetActorHandle() );
}

void CDnPlayerActor::_CheckProcessSkillActioncChange( const char* pAction )
{
	// #26467 액션이 셋팅되고 다음 프레임에 갱신되므로 곧바로 스킬 사용중인지 체크해서 스킬 액션이 아니면 스킬을 종료시키도록 한다.
	if( m_hProcessSkill )
	{
		m_setUseActionName.clear();
		m_setUseActionName.insert( pAction );

		if( false == m_hProcessSkill->IsUseActionNames( m_setUseActionName ) )
		{
			if( false == (IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn()) )
			{
				m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
			}

			m_hProcessSkill.Identity();
		}
	}
}

void CDnPlayerActor::_CheckActionWithProcessPassiveActionSkill( const char* szPrevAction )
{
	// 같은 액션 반복중이면 패스
	if( szPrevAction && m_nPrevActionIndex == m_nActionIndex ) 
		return;

	// if instantly passive skill, then cancel skill. ( ex) archer's spinkick)
	// because state effect must deactivate when change to another attack action.
	if( m_hProcessSkill )
	{
		m_setUseActionName.clear();
		m_setUseActionName.insert( szPrevAction );

		// 현재 액션이 prev 액션의 next 액션이라면 스킬이 이어지는 것으로 본다.
		// 이전 액션이 현재 진행중인 스킬에서 사용하는 액션이었고 현재 액션이 이전 액션의 next 액션이 아니라면
		// 패시브 스킬이 끝난 것으로 판단한다.
		ActionElementStruct* pElement = GetElement( szPrevAction );
		bool bIsNextAction = false;
		if( pElement )
		{
			// #25154 기본 스탠드 액션은 스킬에서 지정된 next 액션이 이어지는 것으로 보지 않는다.
			// 오라 스킬 액션이 끝나고 이 함수가 호출되었을 때 현재 액션이 Stand 로 되어있는데 해당 시점에서
			// m_hProcessSkill 이 스킬이 끝난 것으로 판단되어서 NULL 로 되어야 한다. 
			// m_hProcessSkill 이 남아있으면 다른 스킬 썼을 때 강제로 onend 될 수 있기 때문에 안됨.
			// 따라서 bIsNextAction 이 false 가 되고 m_hProcessSkill->IsUseSkillActionNames() 함수 내부에서
			// 스킬 액션이 종료된 것으로 판단되어야 한다.
			bIsNextAction = ((pElement->szNextActionName != "Stand") && (pElement->szNextActionName == GetCurrentAction()));
		}

		if( false == bIsNextAction &&
			m_hProcessSkill->IsUseActionNames( m_setUseActionName ) )
		{
			// 액티브 스킬이 패시브 형태로 등록되어 사용되었을때 , GetPassiveSkillLengh() 로 알수있다.
			if( ( (m_hProcessSkill->GetSkillType() == CDnSkill::Passive || m_hProcessSkill->GetPassiveSkillLength() != 0.f ) || m_hProcessSkill->GetSkillType() == CDnSkill::AutoPassive) &&
				m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
			{
				// 패시브 스킬이 체인 입력이 들어왔을 때를 체크한다. 한번 체크되는 순간 체인 입력 플래그는 초기화된다.
				// 체인입력되는 순간 액션의 길이만큼 패시브 스킬 사용 길이가 늘어난다.
				// 이렇게 플래그와 시간 둘 다 같이 사용해야 패시브 스킬의 연속 체인이 가능해진다.
				if( false == m_hProcessSkill->CheckChainingPassiveSkill() )
				{
					if( false == m_hProcessSkill->IsChainInputAction( GetCurrentAction() ) )
					{
						m_hProcessSkill->OnEnd( MAActorRenderBase::m_LocalTime, 0.0f );
						m_hProcessSkill.Identity();
					}
				}
			}
			else if( IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn() )
			{
				// Note 한기: m_hProcessSkill 스마트 포인터는 오라 스킬 사용하는 액션이 재생되는 동안은 유효해야
				// 게임 서버에서 CDnPlayerActor::CmdStop() 쪽에서 걸러지기 때문에 겜 서버에서 해당 액션 시그널이 끝까지 
				// 처리됨. 따라서 CDnActor::OnChangeAction 쪽에서 ProcessAction 을 Identity 시킴.

				m_hProcessSkill.Identity();
				ClearSelfStateSignalBlowQueue(); // 오라 스킬의 자기 자신에게 적용하는 상태효과 타이밍 시그널에 보내주는 큐 초기화 시킴. 안그럼 다른 스킬에 영향을 준다.
			}
		}
	}
}


void CDnPlayerActor::OnChangeAction( const char* szPrevAction )
{
	// 129번 액션 이름 대체 상태효과 활성 비활성 처리. /////////////////////////////////////////////////
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_129 ) )
	{
		DNVector(DnBlowHandle) vlhChangeActionSetBlow;
		m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_129, vlhChangeActionSetBlow );

		// 액션 셋 변경 상태효과는 여러개 있을 수 있다.
		int iNumBlow = (int)vlhChangeActionSetBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			if( !vlhChangeActionSetBlow[i] )
				continue;
			CDnChangeActionSetBlow* pChangeActionSetBlow = static_cast<CDnChangeActionSetBlow*>( vlhChangeActionSetBlow.at(i).GetPointer() );
			pChangeActionSetBlow->UpdateEnable( szPrevAction, GetCurrentAction() );
			
			// 액션 변경시에 상태효과 적용 발현타입이 있다면 함수 호출해줌.
			// 클라에서 액션이 이미 변환되어 날아오기 때문에..
			CDnChangeActionStrProcessor* pProcessor = pChangeActionSetBlow->GetChangeActionStrProcessor();	// 액션 변경 발현타입이 비활성화 상태일때는 NULL 리턴됨.
			if( pProcessor && pProcessor->IsChangedActionName( GetCurrentAction() ) )
			{	
				pChangeActionSetBlow->OnChangeAction();
			}
			else
			{
				// 서버에서 자체적으로 액션이 처리되는 setaction 쪽에서도 호출해줘야함.
				pChangeActionSetBlow->OnNotChangeAction();
			}
		}
	}

	_CheckActionWithProcessPassiveActionSkill( szPrevAction );
	//CDnActor::OnChangeAction( szPrevAction );

	if( !( szPrevAction && m_nPrevActionIndex == m_nActionIndex ) ) {
		if( CDnWorld::IsActive(GetRoom()) ) {
			CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "ChangeActionPlayer", GetUniqueID() );
			CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnPlayerActor::OnChangeAction", CDnActionBase::m_LocalTime, 0.f );
		}
	}

	m_bUseSignalSkillCheck = false;
	ZeroMemory( m_abSignalSkillCheck, sizeof(m_abSignalSkillCheck) );

	// mixed 액션중일 때는 mixed 액션 패킷 왔을 때 이미 업데이트 된 상태.
	if( false == m_bUpdatedProjectileInfoFromCmdAction )
	{
		if( false == IsCustomAction() )
			_UpdateMaxProjectileCount( m_nActionIndex );
	}
	else
		m_bUpdatedProjectileInfoFromCmdAction = false;	// 이 다음에 OnChangeAction() 호출되면 업데이트 해주어야 하기 때문에 플래그를 꺼준다.

	// 액션이 바뀌면 텀 체크하도록 원상 복구 시켜줌.
	m_bCheckProjectileSignalTerm = true;

	if( false == m_mapIcyFractionHitted.empty() )
		m_mapIcyFractionHitted.clear();

	// 버블 시스템쪽에 알려줌. 아카데믹 쪽에서 쓰는 경우가..
	boost::shared_ptr<IDnObserverNotifyEvent> pEvent( IDnObserverNotifyEvent::Create( EVENT_ONCHANGEACTION ) );
	Notify( pEvent );
}

void CDnPlayerActor::OnBreakSkillSuperAmmor( int nIndex, int nOriginalSupperAmmor, int nDescreaseSupperAmmor )
{
	if( nOriginalSupperAmmor >= 200 ) {
		UpdateSuperAmmorBreak();
	}
}

void CDnPlayerActor::OnAirCombo( int nComboCount )
{
	if( nComboCount >= 2 ) {
		UpdateAirCombo();
	}
}


void CDnPlayerActor::PushSummonMonster( DnMonsterActorHandle hMonster, const SummonMonsterStruct* pSummonMonsterStruct, bool bReCreateFollowStageMonster/* = false*/ )
{
	CDnActor::PushSummonMonster( hMonster, pSummonMonsterStruct, bReCreateFollowStageMonster );

	// 스테이지 이동으로 재생성 되는 소환몹인지 여부와 관계 없이 (bReCreateFollowStageMonster)
	// 해당 그룹 아이디로 넣어준다. 스테이지 이동하면서 CDnActor::bIsCanSummonMonster() 함수에서 유효하지 않은
	// 몬스터 액터 객체 핸들은 정리된다.
	if( 0 < pSummonMonsterStruct->nGroupID )
	{
		m_mapSummonMonsterByGroup[ pSummonMonsterStruct->nGroupID ].push_back( hMonster );
		hMonster->SetSummonGroupID( pSummonMonsterStruct->nGroupID );
	}

	if( bReCreateFollowStageMonster )
	{
		// 관리되고 있는 스테이지 따라가는 몬스터가 새 스테이지에서 다시 생성되는 경우 핸들 교체.
		DWORD dwMonsterClassID = hMonster->GetMonsterClassID();
		std::list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO>::iterator iter = m_listSummonedMonstersFollowStageInfos.begin();
		for( iter; iter != m_listSummonedMonstersFollowStageInfos.end(); ++iter )
		{
			if( false == iter->bReCreatedFollowStageMonster && 
				dwMonsterClassID == iter->dwMonsterClassID )
			{
				iter->hMonster = hMonster;
				iter->hMonster->SetActionQueue( "Stand" );		// 새로 소환되는 것이 아니므로 곧바로 stand 액션.
				iter->hMonster->CmdWarp( *GetPosition(), EtVec3toVec2( *GetLookDir() ) );
				iter->bReCreatedFollowStageMonster = true;
				break;
			}
		}
	}
	else
	{
		// 추가적으로 플레이어인 경우 스테이지 이동이나 존 이동(CmdWarp) 를 하는 경우 따라가도록 설정된 소환체는
		// 또 따로 갖고 있도록 한다.
		if( pSummonMonsterStruct->bFollowSummonerStage )
		{
			S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO Info;
			Info.hMonster = hMonster;
			Info.dwMonsterClassID = hMonster->GetMonsterClassID();
#ifdef PRE_FIX_MEMOPT_SIGNALH
			CopyShallow_SummonMonsterStruct(Info.SummonMonsterSignalData, pSummonMonsterStruct);
#else
			Info.SummonMonsterSignalData = *pSummonMonsterStruct;
#endif
			Info.iRemainDestroyTime = pSummonMonsterStruct->nLifeTime;

			m_listSummonedMonstersFollowStageInfos.push_back( Info );
		}
	}
}


// 소환된 몬스터 객체가 제한 시간이 다 되었거나 HP 가 0 이 되어 죽는 경우 호출됨.
// 이 두가지 경우에 대해서는 스테이지를 따라가는 소환 몬스터들의 관리 리스트에서 제거해준다.
// 스테이지 이동시 해당 몬스터 객체가 파괴될 때는 호출되지 않는다.
void CDnPlayerActor::OnDieSummonedMonster( DnMonsterActorHandle hSummonedMonster )
{
	std::list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO>::iterator iter = m_listSummonedMonstersFollowStageInfos.begin();
	for( iter; iter != m_listSummonedMonstersFollowStageInfos.end(); ++iter )
	{
		if( hSummonedMonster == iter->hMonster )
		{
			m_listSummonedMonstersFollowStageInfos.erase( iter );
			break;
		}
	}

	int iGroupID = hSummonedMonster->GetSummonGroupID();
	if( 0 < m_mapSummonMonsterByGroup.count( iGroupID ) )
	{
		list<DnMonsterActorHandle>& listSummonedMonsters = m_mapSummonMonsterByGroup[ iGroupID ];
		list<DnMonsterActorHandle>::iterator iter = find( listSummonedMonsters.begin(), listSummonedMonsters.end(), hSummonedMonster );
		if( listSummonedMonsters.end() != iter )
			listSummonedMonsters.erase( iter );

		// 해당 그룹의 리스트가 비었으면 맵에서 제거.
		if( listSummonedMonsters.empty() )
			m_mapSummonMonsterByGroup.erase( iGroupID );
	}

#if defined(PRE_ADD_65808)
	if (hSummonedMonster)
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hSummonedMonster.GetPointer());
		if (pMonsterActor)
		{
			RemoveSummonMonsterGlyphStateEffects(pMonsterActor->GetMonsterClassID());
		}
	}
#endif // PRE_ADD_65808
}

// 스테이지에 있는 몬스터들이 삭제되기 직전에 호출된다.
// 필요한 정보를 여기서 뽑아내서 갖고 있는다.
void CDnPlayerActor::OnBeforeDestroyStageMonsters( void )
{
	std::list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO>::iterator iter = m_listSummonedMonstersFollowStageInfos.begin();
	for( iter; iter != m_listSummonedMonstersFollowStageInfos.end(); )
	{
		// 리스트에 갖고 있는 몬스터 핸들이 invalid 한 경우엔 스테이지 이동시 버리고 간다.
		if( iter->hMonster )
		{
			iter->iRemainDestroyTime = iter->hMonster->GetRemainDestroyTime();
			iter->bReCreatedFollowStageMonster = false;

			// TODO: 추가적으로 받아둘 내용이 있으면 받아둔다.
			
			++iter;
		}
		else
		{
			iter = m_listSummonedMonstersFollowStageInfos.erase( iter );
		}
	}
}

void CDnPlayerActor::OnInitializeNextStageFinished( void )
{
	// 소환된 몬스터 제거된 것들 새로 생성한다. STE_SummonMonster 시그널 그대로 돌림.
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
	std::list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO>::iterator iter = m_listSummonedMonstersFollowStageInfos.begin();
	for( iter; iter != m_listSummonedMonstersFollowStageInfos.end(); ++iter )
	{
		// 스테이지 이동에 따라 다시 생성하는 몬스터이기 때문에 PushMonster() 함수에서 액터 핸들 갱신만 이루어진다.
		// 다시 소환하는 몬스터의 지속시간을 남은 지속시간으로 셋팅해줌.
		iter->SummonMonsterSignalData.nLifeTime = (int)iter->iRemainDestroyTime;
		pTask->RequestSummonMonster( GetMySmartPtr(), &(iter->SummonMonsterSignalData), true );
	}

#ifdef PRE_ADD_WEEKLYEVENT
	RemoveEventStateBlow();
	ApplyEventStateBlow();
#endif

#if defined( PRE_FIX_70618 )
	if( IsAppliedThisStateBlow(STATE_BLOW::BLOW_078) )
		CmdRemoveStateEffect(STATE_BLOW::BLOW_078);
#endif	// #if defined( PRE_FIX_70618 )
}

bool CDnPlayerActor::_bIsMasterSystemDurabilityReward()
{
	if( GetGameRoom() && GetGameRoom()->GetMasterRewardSystem() )
	{
		if( GetGameRoom()->GetMasterRewardSystem()->bIsDurabilityReward( m_pSession ) )
		{
#if defined( _WORK )
			WCHAR wszBuf[MAX_PATH];
			wsprintf( wszBuf, L"[사제시스템] 내구도 보상 적용으로 깎이지 않음" );
			m_pSession->SendDebugChat( wszBuf );
#endif // #if defined( _WORK )
			return true;
		}
	}
	return false;
}

void CDnPlayerActor::OnRepairEquipDurability( bool bDBSave, INT64 nPriceCoin )
{
	bool bRefreshStatus = false;

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	for( int i=CDnParts::Helmet; i<=CDnParts::Ring2; i++ ) {
		DnPartsHandle hParts = GetParts( (CDnParts::PartsTypeEnum)i );
		if( !hParts ) continue;
		if( hParts->IsInfinityDurability() ) continue;
		if( hParts->GetDurability() == hParts->GetMaxDurability() ) continue;

		int nTemp = hParts->GetMaxDurability();
		hParts->SetDurability( nTemp );
		bRefreshStatus = true;

		m_pSession->GetItem()->SetEquipItemDurability( i, nTemp, true );
		if (bDBSave && m_pSession->GetItem()->GetEquip(i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(i)->wDur);
		}
	}
	for( int i=0; i<2; i++ ) {
		DnWeaponHandle hWeapon = m_hWeapon[i];
		if( !hWeapon ) continue;
		if( hWeapon->IsInfinityDurability() ) continue;
		if( hWeapon->GetDurability() == hWeapon->GetMaxDurability() ) continue;

		int nTemp = hWeapon->GetMaxDurability();
		hWeapon->SetDurability( nTemp );
		bRefreshStatus = true;

		m_pSession->GetItem()->SetEquipItemDurability( EQUIP_WEAPON1 + i, nTemp, true );
		if (bDBSave && m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->wDur);
		}
	}
	if( bRefreshStatus ) {
		RefreshState( RefreshEquip, ST_All );
	}

	if (bDBSave)
	{
		INT64 biCurrentCoin = 0;
		INT64 biPickUpCoin = 0;

		if( nPriceCoin > 0 )
		{
			biCurrentCoin = m_pSession->GetCoin();
			biPickUpCoin = m_pSession->GetPickUpCoin();
			m_pSession->SelPickUpCoin(0);
		}
		
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, nPriceCoin, VecSerialList, VecDurList, biCurrentCoin, biPickUpCoin);
	}
}

void CDnPlayerActor::OnDecreaseEquipDurability( int nValue, bool bDBSave )
{
	if( _bIsMasterSystemDurabilityReward() )
		return;

	bool bRefreshStatus = false;

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	for( int i=CDnParts::Helmet; i<=CDnParts::Ring2; i++ ) {
		DnPartsHandle hParts = GetParts( (CDnParts::PartsTypeEnum)i );
		if( !hParts ) continue;
		if( hParts->IsInfinityDurability() ) continue;
		if( hParts->GetDurability() == 0 ) continue;

		int nTemp = hParts->GetDurability() - nValue;
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
		}
		hParts->SetDurability( nTemp );

		m_pSession->GetItem()->SetEquipItemDurability( i, nTemp );
		if (bDBSave && m_pSession->GetItem()->GetEquip(i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(i)->wDur);
		}
	}
	for( int i=0; i<2; i++ ) {
		DnWeaponHandle hWeapon = m_hWeapon[i];
		if( !hWeapon ) continue;
		if( hWeapon->IsInfinityDurability() ) continue;
		if( hWeapon->GetDurability() == 0 ) continue;

		int nTemp = hWeapon->GetDurability() - nValue;
		if( nTemp <= 0 ) {
			nTemp = 0;
			bRefreshStatus = true;
		}
		hWeapon->SetDurability( nTemp );

		m_pSession->GetItem()->SetEquipItemDurability( EQUIP_WEAPON1 + i, nTemp );
		if (bDBSave && m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->wDur);
		}
	}
	if( bRefreshStatus ) {
		RefreshState( RefreshEquip, ST_All );
	}

	if (bDBSave)
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, 0, VecSerialList, VecDurList);
}

void CDnPlayerActor::OnDecreaseEquipDurability( float fValue, bool bDBSave )
{
	if( _bIsMasterSystemDurabilityReward() )
		return;

	bool bRefreshStatus = false;

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	float fTotalLevelValue = 0.0f;
	if ( m_pRoom->bIsPvPRoom() == false && IsAppliedThisStateBlow(STATE_BLOW::BLOW_259))
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
		}
		hParts->SetDurability( nTemp );
		m_pSession->GetItem()->SetEquipItemDurability( i, nTemp );
		if (bDBSave && m_pSession->GetItem()->GetEquip(i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(i)->wDur);
		}
	}
	for( int i=0; i<2; i++ ) {
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
		}

		hWeapon->SetDurability( nTemp );
		m_pSession->GetItem()->SetEquipItemDurability( EQUIP_WEAPON1 + i, nTemp );
		if (bDBSave && m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)){
			VecSerialList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->nSerial);
			VecDurList.push_back(m_pSession->GetItem()->GetEquip(EQUIP_WEAPON1 + i)->wDur);
		}
	}
	if( bRefreshStatus ) {
		RefreshState( RefreshEquip, ST_All );
	}

	if (bDBSave)
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, 0, VecSerialList, VecDurList);
}

void CDnPlayerActor::OnDecreaseInvenDurability( int nValue, bool bDBSave )
{
	if( _bIsMasterSystemDurabilityReward() )
		return;

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	for( int i=0; i<INVENTORYMAX; i++ ) {
		const TItem *pItem = m_pSession->GetItem()->GetInventory(i);
		if( !pItem ) continue;
		switch( g_pDataManager->GetItemMainType( pItem->nItemID ) ) {
			case ITEMTYPE_WEAPON:
				{
					CDnWeapon *pWeapon = static_cast<CDnWeapon*>(m_pPartyData->pInventory[i]);
					if( !pWeapon ) continue;
					if( pWeapon->IsInfinityDurability() ) continue;
					if( pWeapon->GetDurability() == 0 ) continue;

					int nTemp = pWeapon->GetDurability() - nValue;
					if( nTemp <= 0 ) nTemp = 0;

					pWeapon->SetDurability( nTemp );

					m_pSession->GetItem()->SetInvenItemDurability( i, nTemp );
					if (bDBSave && m_pSession->GetItem()->GetInventory(i)){
						VecSerialList.push_back(m_pSession->GetItem()->GetInventory(i)->nSerial);
						VecDurList.push_back(m_pSession->GetItem()->GetInventory(i)->wDur);
					}
				}
				break;
			case ITEMTYPE_PARTS:
				{
					CDnParts *pParts = static_cast<CDnParts*>(m_pPartyData->pInventory[i]);
					if( pParts->IsInfinityDurability() ) continue;
					if( pParts->GetDurability() == 0 ) continue;

					int nTemp = pParts->GetDurability() - nValue;
					if( nTemp <= 0 ) nTemp = 0;
					pParts->SetDurability( nTemp );

					m_pSession->GetItem()->SetInvenItemDurability( i, nTemp );
					if (bDBSave && m_pSession->GetItem()->GetInventory(i)){
						VecSerialList.push_back(m_pSession->GetItem()->GetInventory(i)->nSerial);
						VecDurList.push_back(m_pSession->GetItem()->GetInventory(i)->wDur);
					}
				}
				break;
			default:
				continue;
		}
	}

	if (bDBSave)
		m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, 0, VecSerialList, VecDurList);
}

void CDnPlayerActor::OnDecreaseInvenDurability( float fValue, bool bDBSave )
{
	if( _bIsMasterSystemDurabilityReward() )
		return;

	std::vector<INT64> VecSerialList;
	std::vector<USHORT> VecDurList;
	VecSerialList.clear();
	VecDurList.clear();

	for( int i=0; i<INVENTORYMAX; i++ ) {
		const TItem *pItem = m_pSession->GetItem()->GetInventory(i);
		if( !pItem ) continue;
		switch( g_pDataManager->GetItemMainType( pItem->nItemID ) ) {
			case ITEMTYPE_WEAPON:
				{
					CDnWeapon *pWeapon = static_cast<CDnWeapon*>(m_pPartyData->pInventory[i]);
					if( !pWeapon ) continue;
					if( pWeapon->IsInfinityDurability() ) continue;
					if( pWeapon->GetDurability() == 0 ) continue;

					int nTemp = (int)( pWeapon->GetDurability() - ( pWeapon->GetMaxDurability() * fValue ) );
					if( nTemp <= 0 ) nTemp = 0;

					pWeapon->SetDurability( nTemp );

					m_pSession->GetItem()->SetInvenItemDurability( i, nTemp );
					if (m_pSession->GetItem()->GetInventory(i)){
						VecSerialList.push_back(m_pSession->GetItem()->GetInventory(i)->nSerial);
						VecDurList.push_back(m_pSession->GetItem()->GetInventory(i)->wDur);
					}
				}
				break;
			case ITEMTYPE_PARTS:
				{
					CDnParts *pParts = static_cast<CDnParts*>(m_pPartyData->pInventory[i]);
					if( !pParts ) continue;
					if( pParts->IsInfinityDurability() ) continue;
					if( pParts->GetDurability() == 0 ) continue;

					int nTemp = (int)( pParts->GetDurability() - ( pParts->GetMaxDurability() * fValue ) );
					if( nTemp <= 0 ) nTemp = 0;
					pParts->SetDurability( nTemp );

					m_pSession->GetItem()->SetInvenItemDurability( i, nTemp );
					if (m_pSession->GetItem()->GetInventory(i)){
						VecSerialList.push_back(m_pSession->GetItem()->GetInventory(i)->nSerial);
						VecDurList.push_back(m_pSession->GetItem()->GetInventory(i)->wDur);
					}
				}
				break;
			default:
				continue;
		}
	}

	m_pSession->GetDBConnection()->QueryModItemDurability(m_pSession, 0, VecSerialList, VecDurList);
}

bool CDnPlayerActor::IsPenaltyStageGiveUp()
{
	if (!m_pRoom) return false;
	switch( m_pRoom->GetGameTaskType() ) {
		case GameTaskType::PvP: return false;
		case GameTaskType::DarkLair: return false;
		case GameTaskType::Farm: return false;
		default: break;
	}

	if (IsDie())
		return false;

	if (CDnWorld::IsActive(GetRoom()))
	{
		EWorldEnum::MapTypeEnum mapType = CDnWorld::GetInstance(GetRoom()).GetMapType();
		if (mapType == EWorldEnum::MapTypeDungeon)
		{
			CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
			if (pTask->GetDungeonClearState() == CDnGameTask::DCS_WarpStandBy)
				return false;
		}
		else
		{
			return false;
		}
	}

	return true;
}

// 내구도 감소가 되었으면 return true; 그렇지 않으면 return false;
bool CDnPlayerActor::OnStageGiveUp()
{
	if (IsPenaltyStageGiveUp() == false)
		return false;
	DNTableFileFormat *pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
	if( pDungeonSox->GetFieldFromLablePtr( GetGameRoom()->GetGameTask()->GetDungeonEnterTableID(), "_StageOutDurability" )->GetInteger() == 0)	
		return false;
	OnDecreaseEquipDurability( CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StageGiveupDurabilityPenalty ), true );
	OnDecreaseInvenDurability( CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StageGiveupDurabilityPenalty ), true );

	// 포기하고 가면 Equip 이야 알아서 갱신되지만 인벤토리는 그렇지 않습니다. 그래서 알려줘야 합니다.
	if( m_pSession ) {
		float fRatio = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StageGiveupDurabilityPenalty );
		m_pSession->SendDecreaseDurabilityInventory( 1, (void*)&fRatio );
	}

	return true;
}

void CDnPlayerActor::RecvPartyRefreshGateInfo( const EtVector3& Pos )
{
	SetPosition( Pos );
	SetStaticPosition( Pos );
}

bool CDnPlayerActor::AttachParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete )
{
	if( !hParts )
		return false;

	bool bResult = MAPartsBody::AttachParts( hParts, Index, bDelete );

#if defined(_GAMESERVER)
	int nSkillID = -1;
	int nSkillLevel = 0;

	if (hParts->HasPrefixSkill(nSkillID, nSkillLevel))
	{
		DnSkillHandle hSkill = CDnSkill::CreateSkill(GetMySmartPtr(), nSkillID, nSkillLevel);
		if (!hSkill)
		{
			OutputDebug("%s ==> SkillID %d, SkillLevel %d...  CreateSkill Failed!!!!\n", __FUNCTION__, nSkillID, nSkillLevel);
		}
		else 
		{
#if defined(PRE_ADD_PREFIXSKILL_PVP)
			// 스킬 레벨 데이터를 pve/pvp 인 경우와 나눠서 셋팅해준다.
			int iSkillLevelDataType = CDnSkill::PVE;
			if( GetGameRoom()->bIsPvPRoom() )
				iSkillLevelDataType = CDnSkill::PVP;

			hSkill->SelectLevelDataType( iSkillLevelDataType );
#endif // PRE_ADD_PREFIXSKILL_PVP

			if (!MASkillUser::AddPreFixSystemDefenceSkill(Index, hSkill))
			{
				SAFE_RELEASE_SPTR(hSkill);
				OutputDebug("MASkillUser::AddPreFixSystemDeffenceSkill ===> slotIndex %d, ItemID %d, SkillID %d, SkillLevel %d AddPreFixSystem Deffence skill Failed !!!\n", 
					Index, hParts->GetClassID(), nSkillID, nSkillLevel);
			}
		}
	}
#endif // _GAMESERVER

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

#if defined(_GAMESERVER)
	MASkillUser::RemovePreFixSystemDefenceSkill(Index);
#endif // _GAMESERVER

	RemoveSkillLevelUpInfo(Index);

	// Note: 파츠 분리에 실패한 경우에도 스킬은 사라질 수 있음.
	bool bResult = MAPartsBody::DetachParts( Index );
	return bResult;
}

bool CDnPlayerActor::AttachCashParts( DnPartsHandle hParts, CDnParts::PartsTypeEnum Index, bool bDelete )
{
	if( !hParts ) return false;

	bool bResult = MAPartsBody::AttachCashParts( hParts, Index, bDelete );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hParts->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfoByCashItem(Index, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

	return bResult;
}

bool CDnPlayerActor::DetachCashParts( CDnParts::PartsTypeEnum Index )
{
	RemoveSkillLevelUpInfoByCashItem(Index);

	// Note: 파츠 분리에 실패한 경우에도 스킬은 사라질 수 있음.
	bool bResult = MAPartsBody::DetachCashParts( Index );
	return bResult;
}

void CDnPlayerActor::ReplacementGlyph( DnSkillHandle hNewSkill )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.ext failed\r\n");
		return;
	}

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

bool CDnPlayerActor::AttachGlyph( DnGlyphHandle hGlyph, CDnGlyph::GlyphSlotEnum Index, bool bDelete /* = false */ )
{
	if( !MAPlateUser::AttachGlyph( hGlyph, Index, bDelete ) )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.ext failed\r\n");
		return false;
	}

	int eType = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_GlyphType" )->GetInteger();

	// 문장에 스킬이 존재 할 경우 문장이 스킬추가 인지 스킬효과추가 인지 알아 낸다.
	if( CDnItem::TemperedSkill == eType )
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
		if( !pSox ) 
		{
			g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.ext failed\r\n");
			return false;
		}

		int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID" )->GetInteger();

		DnSkillHandle hSkill = FindSkill( nSkillID );
		if( !hSkill ) return false;

		//CDnSkill에 SkillEffect를 추가해야함
		hSkill->AddGlyphStateEffect( hGlyph->GetClassID() );
	}
	else if( CDnItem::AddSKill == eType && 0 != hGlyph->GetSkillID() &&  0 != hGlyph->GetSkillLevel() )
	{
		AddSkill( hGlyph->GetSkillID(), hGlyph->GetSkillLevel() );
		DnSkillHandle hSkill = FindSkill( hGlyph->GetSkillID() );

		if( !hSkill )	return false;

		hSkill->AsEquipItemSkill();
		
		hSkill->SetEquipIndex( Index );

		m_ahEquipSkill = hSkill;

		if( 0.0f != m_afLastEquipItemSkillDelayTime )
		{
			hSkill->SetOnceCoolTime( m_afLastEquipItemSkillDelayTime, m_afLastEquipItemSkillRemainTime );
		}
	}

	return true;
}

bool CDnPlayerActor::DetachGlyph( CDnGlyph::GlyphSlotEnum Index )
{
	DnGlyphHandle hGlyph = m_hGlyph[Index];
	if( !hGlyph )	return false;

	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TGLYPHSKILL );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"GlyphSkillTable.ext failed\r\n");
		return false;
	}

	int eType = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_GlyphType")->GetInteger();

	if( CDnItem::TemperedSkill == eType )
	{
		int nSkillID = pSox->GetFieldFromLablePtr( hGlyph->GetClassID(), "_SkillID")->GetInteger();

		DnSkillHandle hSkill = FindSkill( nSkillID );
		if( !hSkill ) return false;

		hSkill->DelGlyphStateEffect( hGlyph->GetClassID() );
	}
	else if( CDnItem::AddSKill == eType && 0 != hGlyph->GetSkillID() && 0 != hGlyph->GetSkillLevel() )
	{
		DnSkillHandle hSkill = FindSkill( hGlyph->GetSkillID() );
		m_afLastEquipItemSkillDelayTime = hSkill->GetDelayTime();
		m_afLastEquipItemSkillRemainTime = hSkill->GetElapsedDelayTime();

		RemoveSkill( hGlyph->GetSkillID() );
	}

	return true;
}

void CDnPlayerActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex /*= 0*/, bool bDelete/* = false */)
{
	CDnActor::AttachWeapon( hWeapon, nEquipIndex, bDelete );

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	RefreshWeaponViewOrder( nEquipIndex );

	if( m_hCashWeapon[nEquipIndex] ) {
		m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( GetMySmartPtr(), nEquipIndex );
		LinkCashWeapon( nEquipIndex );
	}

#if defined(_GAMESERVER)
	int nSkillID = -1;
	int nSkillLevel = 0;

	if (hWeapon->HasPrefixSkill(nSkillID, nSkillLevel))
	{
		DnSkillHandle hSkill = CDnSkill::CreateSkill(GetMySmartPtr(), nSkillID, nSkillLevel);
		if (!hSkill)
		{
			OutputDebug("%s ==> SkillID %d, SkillLevel %d...  CreateSkill Failed!!!!\n", __FUNCTION__, nSkillID, nSkillLevel);
		}
		else 
		{
#if defined(PRE_ADD_PREFIXSKILL_PVP)
			// 스킬 레벨 데이터를 pve/pvp 인 경우와 나눠서 셋팅해준다.
			int iSkillLevelDataType = CDnSkill::PVE;
			if( GetGameRoom()->bIsPvPRoom() )
				iSkillLevelDataType = CDnSkill::PVP;

			hSkill->SelectLevelDataType( iSkillLevelDataType );
#endif // PRE_ADD_PREFIXSKILL_PVP

			if (!MASkillUser::AddPreFixSystemOffenceSkill(nEquipIndex, hSkill))
			{
				SAFE_RELEASE_SPTR(hSkill);
				OutputDebug("MASkillUser::AddPreFixSystemOffenceSkill ===> slotIndex %d, ItemID %d, SkillID %d, SkillLevel %d AddPreFixSystem Deffence skill Failed !!!\n", 
					nEquipIndex, hWeapon->GetClassID(), nSkillID, nSkillLevel);
			}
		}
	}
#endif // _GAMESERVER

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hWeapon->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfo(CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}

}

void CDnPlayerActor::DetachWeapon( int nEquipIndex/* = 0*/ )
{
	CDnActor::DetachWeapon( nEquipIndex );

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	RefreshWeaponViewOrder( nEquipIndex );

#if defined(_GAMESERVER)
	MASkillUser::RemovePreFixSystemOffenceSkill(nEquipIndex);
#endif // _GAMESERVER

	RemoveSkillLevelUpInfo(CDnParts::PartsTypeEnum::PartsTypeEnum_Amount+nEquipIndex);
}

void CDnPlayerActor::AttachCashWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	if( m_hCashWeapon[nEquipIndex] ) {
		DetachCashWeapon( nEquipIndex );
	}
	m_hCashWeapon[nEquipIndex] = hWeapon;
	m_bCashSelfDeleteWeapon[nEquipIndex] = bDelete;
	if( !m_hCashWeapon[nEquipIndex] ) return;
	m_hCashWeapon[nEquipIndex]->CreateObject();

	RefreshWeaponViewOrder( nEquipIndex );
	m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( GetMySmartPtr(), nEquipIndex );
	LinkCashWeapon( nEquipIndex );

	int nLevelUpSkillID = -1;
	int nLevelUpSkillLevelValue = 0;
	int nLevelUpItemSkillUsingType = 0;
	if (hWeapon->HasLevelUpInfo(nLevelUpSkillID, nLevelUpSkillLevelValue, nLevelUpItemSkillUsingType))
	{
		if (CDnItem::ItemSkillApplyType::SkillLevelUp == nLevelUpItemSkillUsingType)
			AddSkillLevelUpInfoByCashItem(CASHEQUIP_WEAPON1+nEquipIndex, nLevelUpSkillID, nLevelUpSkillLevelValue);
	}
}

void CDnPlayerActor::LinkCashWeapon( int nEquipIndex )
{
	switch( m_hCashWeapon[nEquipIndex]->GetEquipType() ) {
		case CDnWeapon::Sword:
		case CDnWeapon::Axe:
		case CDnWeapon::Hammer:
		case CDnWeapon::SmallBow:
		case CDnWeapon::BigBow:
		case CDnWeapon::CrossBow:
		case CDnWeapon::Staff:
		case CDnWeapon::Book:
		case CDnWeapon::Orb:
		case CDnWeapon::Puppet:
		case CDnWeapon::Mace:
		case CDnWeapon::Flail:
		case CDnWeapon::Wand:
		case CDnWeapon::Shield:
		case CDnWeapon::Gauntlet:
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			break;
		case CDnWeapon::Arrow:
			m_hCashWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), m_hCashWeapon[0] );
			break;
	}
}

void CDnPlayerActor::DetachCashWeapon( int nEquipIndex )
{
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

void CDnPlayerActor::ShowCashWeapon( int nEquipIndex, bool bShow )
{
	if( m_hCashWeapon[nEquipIndex] )
		m_hCashWeapon[nEquipIndex]->ShowWeapon( bShow );
}

void CDnPlayerActor::SetWeaponViewOrder( int nEquipIndex, bool bShowCash )
{
	if( nEquipIndex < 0 || nEquipIndex >= 2 ) return;
	m_bWeaponViewOrder[nEquipIndex] = bShowCash;
}

void CDnPlayerActor::RefreshWeaponViewOrder( int nEquipIndex )
{
	if( nEquipIndex < 0 || nEquipIndex >= 2 ) return;
	if( m_hWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex] ) {
		if( m_bWeaponViewOrder[nEquipIndex] ) {
			if( !m_hCashWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hCashWeapon[nEquipIndex]->CreateObject();
				m_hCashWeapon[nEquipIndex]->RecreateCashWeapon( GetMySmartPtr(), nEquipIndex );
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
				m_hWeapon[nEquipIndex]->CreateObject();
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
			m_hWeapon[nEquipIndex]->CreateObject();
			LinkWeapon( nEquipIndex );
			m_hWeapon[nEquipIndex]->ShowWeapon( true );
		}
	}
	else {
		if( nEquipIndex == 0 ) {
			if( m_hCashWeapon[nEquipIndex] && m_hCashWeapon[nEquipIndex]->IsCreateObject() ) {
				m_hCashWeapon[nEquipIndex]->FreeObject();
				m_hCashWeapon[nEquipIndex]->ShowWeapon( false );
			}
		}
	}
	SetBattleMode( IsBattleMode() );
}

void CDnPlayerActor::OnEventCP( CPTypeEnum Type, int nResult )
{
#if defined( PRE_ADD_CP_RENEWAL )
	MACP_Renewal::OnEventCP( Type, nResult );
#else	// #if defined( PRE_ADD_CP_RENEWAL )
	MACP::OnEventCP( Type, nResult );
#endif	// #if defined( PRE_ADD_CP_RENEWAL )

	switch( Type ) {
		case MACP::MaxComboCount:
		case MACP::KillBossCount:
		case MACP::SuperAmmorBreakScore:
		case MACP::GenocideScore:
		case MACP::AirComboScore:
		case MACP::RebirthPlayerScore:
		case MACP::ComboScore:
		case MACP::PartyComboScore:
		case MACP::AssistMonsterScore:
			{
				BYTE pBuffer[32];
				CPacketCompressStream Stream( pBuffer, 32 );

				Stream.Write( &Type, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Write( &nResult, sizeof(int) );

				Send( eActor::SC_CP, &Stream );
			}
			break;
	}
}

void CDnPlayerActor::UpdateAttackedCPPoint( CDnDamageBase *pHitter , CDnWeapon::HitTypeEnum eHitType )
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

DnWeaponHandle CDnPlayerActor::GetActiveWeapon( int nEquipIndex )
{
	if( m_bWeaponViewOrder[nEquipIndex] && m_hCashWeapon[nEquipIndex] ) return m_hCashWeapon[nEquipIndex];
	return CDnActor::GetActiveWeapon( nEquipIndex );
}

float CDnPlayerActor::PreCalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam, const float fDefenseRate, float fStateEffectAttackM )
{
	float fResult = CDnActor::PreCalcDamage( pHitter, HitParam, fDefenseRate, fStateEffectAttackM );
	return fResult;
}

bool CDnPlayerActor::IsDie()
{
	bool bResult = CDnPlayerState::IsDie();
	if( bResult ) return true;

	// PvP 에서는 return true 할 필요가 없습니다.
	if( IsGMTrace() && GetGameRoom() && !GetGameRoom()->bIsPvPRoom() )
		return true;

	return false;
}

bool CDnPlayerActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame , float fStartFrame , bool bCheck , bool bCheckStateEffect )
{
	if(m_bShootMode && !m_bTransformMode && m_bBattleMode)
		szActionName = GetChangeShootActionName(szActionName);

#if defined(PRE_FIX_63219)
	//#63219
	//블럭이 발동된 시점에 좌우 이동키를 누르면 MAWalkMovement에서 OnStop이 호출 되고
	//이때 CmdStop으로 "Stand"동작이 설정되어 버린다.
	//클라이언트에서 이동중 block 발동 되고 특수공격키 눌러 패시브 스킬을 사용하면
	//서버에서는 Stand동작으로 변경되 서버에서는 스킬이 동작 하지 않게 됨.
	//일단 Block동작에 Stand로 변경은 무시
	if ((strstr(m_szAction.c_str(), "Block") != NULL ||
		strstr(m_szActionQueue.c_str(), "Block") != NULL)&&
		strstr(szActionName, "Stand") != NULL)
	{
		//#68376 스탠스 오브 페이스 오류
		//Skill_StandOfFaith_EX_Block 이런 동작에서 Stand로는 전환이 되어야 한다...
		//다른 Block이 있을 수 있어서 "StandOfFaith"이 있으면 전환 하도록 수정.
		bool isSkillAction = (strstr(m_szAction.c_str(), "StandOfFaith") != NULL || strstr(m_szActionQueue.c_str(), "StandOfFaith"));
		
		if (isSkillAction == false)
			return false;
	}
#endif // PRE_FIX_63219

	return CDnActor::SetActionQueue( szActionName , nLoopCount, fBlendFrame, fStartFrame, bCheck, bCheckStateEffect );
}

void CDnPlayerActor::_UpdateMaxProjectileCount( int nActionIndex, bool bUpdateReservedCount/* = false*/ )
{
	if( NULL == m_pProjectileCountInfo )
		return;

	// 현재 액션에서 최대한 발사할 수 있는 발사체 갯수.
	m_iNowMaxProjectileCount = 0;
	map<int, int>::const_iterator iterProj = m_pProjectileCountInfo->mapMaxProjectileCountInAction.find( nActionIndex );
	if( m_pProjectileCountInfo->mapMaxProjectileCountInAction.end() != iterProj )
		m_iNowMaxProjectileCount = iterProj->second;

	// 발사체 시그널 순서대로 사용하는 무기 테이블 인덱스
	// 보통 발사체 시그널과 무기 SendAction 이 섞여있지 않다면 덱이 리셋되지 않아서 아래쪽 무기 체크하는 루프에서 계속 쌓이게 되므로
	// 여기서 한번 클리어 해준다.
	m_setWeaponIDUsingProjectileSignal.clear();
	map<int, multiset<int> >::const_iterator iterWeaponIDs = m_pProjectileCountInfo->mapUsingProjectileWeaponTableIDs.find( nActionIndex );
	if( m_pProjectileCountInfo->mapUsingProjectileWeaponTableIDs.end() != iterWeaponIDs )
		m_setWeaponIDUsingProjectileSignal = iterWeaponIDs->second;

	// 현재 액션에서 발사체가 나가는 프레임들 모음.
	// 보통 발사체 시그널과 무기 SendAction 이 섞여있지 않다면 덱이 리셋되지 않아서 아래쪽 무기 체크하는 루프에서 계속 쌓이게 되므로
	// 여기서 한번 클리어 해준다.
	m_dqProjectileSignalOffset.clear();
	map<int, deque<int> >::const_iterator iterProjOffset = m_pProjectileCountInfo->mapProjectileSignalFrameOffset.find( nActionIndex );
	if( m_pProjectileCountInfo->mapProjectileSignalFrameOffset.end() != iterProjOffset )
		m_dqProjectileSignalOffset = iterProjOffset->second;

	sort( m_dqProjectileSignalOffset.begin(), m_dqProjectileSignalOffset.end() );

	map<int, vector<CDnActionSpecificInfo::S_WEAPONACTION_INFO> >::const_iterator iterWeapon = m_pProjectileCountInfo->mapSendActionWeapon.find( nActionIndex );
	if( m_pProjectileCountInfo->mapSendActionWeapon.end() != iterWeapon )
	{
		const vector<CDnActionSpecificInfo::S_WEAPONACTION_INFO>& vlWeaponInfo = iterWeapon->second;
		for( int i = 0; i < (int)vlWeaponInfo.size(); ++i )
		{
			const CDnActionSpecificInfo::S_WEAPONACTION_INFO& Struct = vlWeaponInfo.at( i );

			// 그냥 호출하면 TDnPlayer~~::GetActiveWeapon() 함수가 호출되어 발차기 중 쏘는 액션으로 바뀔 때 발 무기가 얻어와지므로 핵체크에 걸려서 CDnPlayerActor::GetActiveWeapon() 을 호출 하돌고 변경.
			DnWeaponHandle hWeapon = CDnPlayerActor::GetActiveWeapon( Struct.iWeaponIndex );		
			if( hWeapon ) 
			{
				if( false == Struct.strActionName.empty() ) 
				{
					// 플레이어 액터인 경우현재 액션에서 쏠 수 있는 발사체 갯수에 무기의 발사체 갯수를 더해준다.
					if( hWeapon->IsExistAction( Struct.strActionName.c_str() ) )
					{
						hWeapon->SetActionQueue( Struct.strActionName.c_str() );
						if( m_ActorType <= Reserved6 )
						{
							int nWeaponActionIndex = hWeapon->GetElementIndex( Struct.strActionName.c_str() );
							int nAddCount = hWeapon->GetMaxProjectileCountInAction( nWeaponActionIndex );
							
							if( false == bUpdateReservedCount )
								m_iNowMaxProjectileCount += nAddCount;
							else
								m_iReservedProjectileCount += nAddCount;

							// 무기에서 쏘는 발사체에서 사용하는 무기 테이블 정보 추가.
							hWeapon->AddUsingProjectileWeaponTableIDs( nWeaponActionIndex, m_setWeaponIDUsingProjectileSignal );

							// 무기에서 쏘는 발사체 프레임 정보 추가.
							// 마지막 프레임 같은 곳에 무기에게 shoot 액션을 하라는 시그널을 박던가하면 이 핵 체크 루틴에 걸릴 수 있습니다.
							// 현재로썬 그럴 가능성이 없기 때문에 일단 패스.
							hWeapon->AddProjectileSignalOffset( nWeaponActionIndex, Struct.iFrame, m_dqProjectileSignalOffset );
							
							// 결과로 얻어온 것은 정렬로 가지고 있도록 합니다.
							// 프레임 순서대로 정렬해서 갖고 있다가 비교할때 사용함.
							sort( m_dqProjectileSignalOffset.begin(), m_dqProjectileSignalOffset.end() );
						}
					}
				}
			}
		}
	}
}


bool CDnPlayerActor::UseAndCheckAvailProjectileCount( void )
{
	if( m_iNowMaxProjectileCount <= 0 )
	{
		// 발사체에서 발사체 쏘는 경우처럼 예약된 발사체 갯수도 없는 경우 핵으로 판단.
		if( m_iReservedProjectileCount <= 0 )
		{
			// 핵으로 발사체를 마구 날리고 있음.
			OutputDebug( "CS_PROJECTILE: 현재 액션의 최대 갯수를 넘는 발사체 요청. 핵으로 판단됨.\n" );
			
			return false;
		}

		--m_iReservedProjectileCount;
	}

	if( 0 < m_iNowMaxProjectileCount )
		--m_iNowMaxProjectileCount;

	return true;
}

bool CDnPlayerActor::IsExclusiveSkill( int iSkillID, int iExclusiveID )
{
#ifndef PRE_FIX_SKILLLIST
	// 배운 스킬 중에 같은 스킬 못 배우게 하는 id 가 있으면 true.
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );

		// 자신의 스킬은 제외하고.
		if( hSkill->GetClassID() == iSkillID )
			continue;

		if( 0 == hSkill->GetExclusiveID() )
			continue;

		if( iExclusiveID == hSkill->GetExclusiveID() )
		{
			return true;
		}
	}
#else
	// 배운 스킬 중에 같은 스킬 못 배우게 하는 id 가 있으면 true.
	DWORD dwNumSkill = GetSkillCount();
	for( DWORD i = 0; i < dwNumSkill; ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );

		// 자신의 스킬은 제외하고.
		if( hSkill->GetClassID() == iSkillID )
			continue;

		if( 0 == hSkill->GetExclusiveID() )
			continue;

		if( iExclusiveID == hSkill->GetExclusiveID() )
		{
			return true;
		}
	}
#endif // #ifndef PRE_FIX_SKILLLIST

	return false;
}

int CDnPlayerActor::GetAvailSkillPointByJob( int iSkillID )
{
	// 현재 직업 차수
	int iJobDegree = g_pDataManager->GetJobNumber( GetJobClassID() );

	int iWholeSP = GetLevelUpSkillPoint( 1, GetLevel() );
	int iWholeAvailSPByJob = int(iWholeSP * m_pSession->GetAvailSkillPointRatioByJob( iSkillID ));

	// 스킬에 필요한 직업에 해당되는 사용한 SP 를 모은다.
	int iUsedSPByJob = 0;
	const TSkillData* pSkillDataForNeedJobID = g_pDataManager->GetSkillData( iSkillID );

#ifndef PRE_FIX_SKILLLIST
	for( int i = 0; i < (int)m_vlhSkillList.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( i );
#else
	DWORD dwNumSkill = GetSkillCount();
	for( DWORD i = 0; i < dwNumSkill; ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
		if( hSkill->GetNeedJobClassID() == pSkillDataForNeedJobID->nNeedJobID )
		{
			int iLevel = hSkill->GetLevel();
			for( int k = 0; k < iLevel; ++k )
			{
				const TSkillData* pSkillData = g_pDataManager->GetSkillData( hSkill->GetClassID() );
				iUsedSPByJob += pSkillData->vLevelDataList.at( k ).nNeedSkillPoint;
			}
		}
	}

	// 전체 사용가능 SP 보다 직업 SP 가 남은 것이 많으면 전체 사용가능 SP 가 진짜이므로 해당 포인트로 리턴.
	int iAvailPoint = iWholeAvailSPByJob - iUsedSPByJob;
	if( m_pSession->GetSkillPoint() < iAvailPoint )
		iAvailPoint = m_pSession->GetSkillPoint();
	
	return iAvailPoint;
}

int CDnPlayerActor::GetUsedSkillPointInThisJob( const int nJobID )
{
	int iUsedSPByJob = 0;

#ifndef PRE_FIX_SKILLLIST
	for( int i = 0; i < (int)m_vlhSkillList.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( i );
#else
	DWORD dwNumSkill = GetSkillCount();
	for( DWORD i = 0; i < dwNumSkill; ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
		if( hSkill->GetNeedJobClassID() == nJobID )
		{
			int iLevel = hSkill->GetLevel();
			for( int k = 0; k < iLevel; ++k )
			{
				const TSkillData* pSkillData = g_pDataManager->GetSkillData( hSkill->GetClassID() );
				iUsedSPByJob += pSkillData->vLevelDataList.at( k ).nNeedSkillPoint;
			}
		}
	}

	return iUsedSPByJob;
}

void CDnPlayerActor::OnChangePlaySpeed( DWORD dwFrame, float fSpeed )
{
	// 맞거나 해서 슈퍼아머 때문에 플레이 시간이 늘어난 경우 시그널 텀 프레임 값을
	// 같이 업데이트 해줘야 핵에 안걸린다.
	// 프레임 스피드 변경된 가운데 액션을 시작할 수도 있으므로 값을 받아놓고 체크할 때 적용하는 것으로 변경.
	//for( int i = 0; i < (int)m_dqProjectileSignalOffset.size(); ++i )
	//{
	//	int& iSignalOffsetFrame = m_dqProjectileSignalOffset.at( i );
	//	iSignalOffsetFrame = (int)((float)iSignalOffsetFrame * fSpeed );
	//}

	// 슈퍼아머로 아예 프레임이 순간적으로 일정 시간 정지되는 경우
	if( dwFrame <= 200 && fSpeed < 0.05f)
		m_bCheckProjectileSignalTerm = false;

	m_fFrameSpeed = fSpeed;
}

void CDnPlayerActor::ProcessCompanion( LOCAL_TIME LocalTime, float fDelta )
{
	CheckPetSatietyPercent();
}

void CDnPlayerActor::CheckPetSatietyPercent()
{
	if( !IsCanPetMode() )
		return;

	const TVehicle* pEquipPet = GetUserSession()->GetItem()->GetPetEquip();
	if( pEquipPet && (pEquipPet->nType & Pet::Type::ePETTYPE_SATIETY) && pEquipPet->Vehicle[Pet::Slot::Body].nItemID > 0 )
	{
		if( GetUserSession()->GetItem()->GetPetSatietyPercent() < 50.f ) // 여기에 Limit 체크 해야 합니다.
		{
			DnSkillHandle hFirstSkill = FindSkill(pEquipPet->nSkillID1);
			if(hFirstSkill && hFirstSkill->GetSkillType() == CDnSkill::Passive )
			{
				CmdForceRemoveSkill(pEquipPet->nSkillID1);
				m_bDeletedPetSkill[Pet::Skill::Primary] = true;
			}

			DnSkillHandle hSecondSkill = FindSkill(pEquipPet->nSkillID2);
			if(hSecondSkill && hSecondSkill->GetSkillType() == CDnSkill::Passive )
			{
				CmdForceRemoveSkill(pEquipPet->nSkillID2);
				m_bDeletedPetSkill[Pet::Skill::Secondary] = true;
			}
		}
		else
		{
			if( m_bDeletedPetSkill[Pet::Skill::Primary] == true )
			{
				if( !IsExistSkill( pEquipPet->nSkillID1 ) )
					CmdForceAddSkill( pEquipPet->nSkillID1 );

				m_bDeletedPetSkill[Pet::Skill::Primary] = false;
			}

			if( m_bDeletedPetSkill[Pet::Skill::Secondary] == true )
			{
				if( !IsExistSkill( pEquipPet->nSkillID2 ) )
					CmdForceAddSkill( pEquipPet->nSkillID2 );

				m_bDeletedPetSkill[Pet::Skill::Secondary] = false;
			}
		}
	}
}

CDnVehicleActor *CDnPlayerActor::GetMyVehicleActor()
{
	if(m_hVehicleActor)
	{
		return static_cast<CDnVehicleActor*>(m_hVehicleActor.GetPointer());
	}

	return NULL;
}

bool CDnPlayerActor::IsCanVehicleMode()
{
	if( IsDie() || IsGhost() || m_nTeam == PvPCommon::Team::Observer ) 
		return false;

	if( IsSpectatorMode() ) 
		return false;

	int nCurrentMapID = 0;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
	if( pGameTask ) // 게임테스크일때의 검출 
		nCurrentMapID = pGameTask->GetMapTableID();
	else
		nCurrentMapID = m_pSession->GetMapIndex();

	bool bIsCanVehicleMode = g_pDataManager->IsVehicleMode(nCurrentMapID);

#if defined( PRE_ADD_FORCE_RIDE_ENABLE_TRIGGER )
	bIsCanVehicleMode = bIsCanVehicleMode && m_bForceEnableRideByTrigger;
#endif	// #if defined( PRE_ADD_FORCE_RIDE_ENABLE_TRIGGER )

	return bIsCanVehicleMode;
}

bool CDnPlayerActor::IsCanPetMode()
{
	int nCurrentMapID = 0;

	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
	if( pGameTask ) // 게임테스크일때의 검출 
		nCurrentMapID = pGameTask->GetMapTableID();
	else
		nCurrentMapID = m_pSession->GetMapIndex();

	return g_pDataManager->IsPetMode( nCurrentMapID );
}

void CDnPlayerActor::RideVehicle(TVehicle *pInfo)
{
	if( !IsPlayerActor() || IsVehicleMode() || !pInfo )
		return;

	if( !IsCanVehicleMode())
		return;
		
	if(IsBattleMode())
		SetBattleMode(false);

	int nVehicleActorTableID = g_pDataManager->GetVehicleActorID(pInfo->Vehicle[Vehicle::Slot::Body].nItemID);

	DnActorHandle hVehicle;
	hVehicle = CreateActor( GetRoom(), nVehicleActorTableID );
	if( !hVehicle || !hVehicle->GetObjectHandle())
		return;

	CDnVehicleActor* pVehicle = (CDnVehicleActor *)hVehicle.GetPointer();
	if(!pVehicle)
		return;

	pVehicle->SetMyPlayerActor(GetActorHandle());
	pVehicle->Show( false );

	float fLandHeight = INSTANCE(CDnWorld).GetHeight( GetMatEx()->m_vPosition );

	pVehicle->SetPosition(GetMatEx()->m_vPosition);
	pVehicle->SetPrevPosition(GetMatEx()->m_vPosition);
	pVehicle->SetAddHeight( GetMatEx()->m_vPosition.y - fLandHeight );
	pVehicle->GetMatEx()->CopyRotationFromThis(GetMatEx());

	pVehicle->SetItemID(pInfo->Vehicle[Vehicle::Slot::Body].nItemID); // 자신의 아이템 아이디를 가지고 있습니다.

#ifdef PRE_ADD_VEHICLE_ACTION_STRING
	DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );

	std::string strVehicleAction = pVehicleTable->GetFieldFromLablePtr( pInfo->Vehicle[Vehicle::Slot::Body].nItemID , "_RiderString" )->GetString();
	if(!strVehicleAction.empty() || strstr(strVehicleAction.c_str() , "Vehicle_") != NULL )
		pVehicle->SetVehicleActionString(strVehicleAction.c_str());
#endif

	// 부위별 장착 // 파츠 정보를 함유하고 들어오는 경우는 장착을 시켜줍니다.
	for (int i= Vehicle::Slot::Saddle; i<Vehicle::Slot::Max; i++)
	{
		if(pInfo->Vehicle[i].nItemID != 0 && pInfo->Vehicle[i].nSerial != 0)
		{
			pVehicle->EquipItem(pInfo->Vehicle[i]);
		}
	}

	////////////////
	if(pInfo->dwPartsColor1 != 0 && pInfo->dwPartsColor1 != -1) // 설정된 색이 있는경우에는 색 설정을 해줍니다.
	{
		pVehicle->ChangeHairColor(pInfo->dwPartsColor1); // 기본 테이블에 정의된 색지정.
	}

	SetMyVehicleActor(pVehicle->GetActorHandle());
	pVehicle->InitializeRoom((CDNGameRoom*)GetRoom());
	pVehicle->Initialize();
	pVehicle->SetUniqueID( m_pSession->GetVehicleObjectID() );
		
	pVehicle->SetVehicleClassID(pInfo->Vehicle[Vehicle::Slot::Body].nItemID);
	pVehicle->RefreshState();
	pVehicle->Show(true);
	
	pVehicle->SetAttachToPlayer(true);
	pVehicle->SetProcess(true);

	SetVehicleMode(true);
	pVehicle->SetActionQueue( "Stand" );
	pVehicle->SetTeam(GetTeam());
}
  
void CDnPlayerActor::UnRideVehicle(bool bForce)
{
	if(!IsVehicleMode())
		return;

	if(GetMyVehicleActor() && !GetMyVehicleActor()->IsDestroy())
	{
		float fLandHeight = INSTANCE(CDnWorld).GetHeight( GetMyVehicleActor()->GetMatEx()->m_vPosition );
		SetPosition(GetMyVehicleActor()->GetMatEx()->m_vPosition);
		SetPrevPosition(GetMyVehicleActor()->GetMatEx()->m_vPosition);
		SetAddHeight( GetMyVehicleActor()->GetMatEx()->m_vPosition.y - fLandHeight );

		GetMyVehicleActor()->SetAttachToPlayer(false);
		GetMyVehicleActor()->Show(false);
		GetMyVehicleActor()->SetUniqueID(GetMyVehicleActor()->GetUniqueID() +1 );  
		// SetDestroy가 설정되고 프로세스 이후에 액터가 삭제되는 사이에 새로운 말을 타게되면 유니크 아이디가 겹치는 문제가 발생된다
		// < 신규 유니크아이디를 이후 프로세스에서 삭제하는경우가 생김 > 그러므로 디스트로이 하기전에 유니크아이디를 변경시켜주자.
		GetMyVehicleActor()->SetDestroy();
	}

	SetVehicleMode(false);
	Show(true);
	SetActionQueue("Stand");

	return;
}

void CDnPlayerActor::ForceUnRideVehicle()
{
	if(!IsVehicleMode())
		return;

	CDNGameRoom* pRoom = GetGameRoom();
	if (!pRoom)
		return;

	api_trigger_UnRideVehicle( pRoom, GetSessionID() );
}

void CDnPlayerActor::RemoveVehicleStateEffectImmediately(int nBlowIndex )
{
	if(IsVehicleMode() && GetMyVehicleActor())
	{
		GetMyVehicleActor()->CmdRemoveStateEffectImmediately( (STATE_BLOW::emBLOW_INDEX)nBlowIndex );
		GetMyVehicleActor()->SendRemoveStateEffect( (STATE_BLOW::emBLOW_INDEX)nBlowIndex );
	}
}

void CDnPlayerActor::SetForceEnableRide( const bool bForceEnableRide )
{
	if( false == bForceEnableRide )
		ForceUnRideVehicle();

	m_bForceEnableRideByTrigger = bForceEnableRide;

	m_pSession->SendTriggerForceEnableRide( GetSessionID(), bForceEnableRide );
}

void CDnPlayerActor::OnCannonMonsterDie( void )
{
	// 대포 모드 해제
	EndCannonMode();
	SetActionQueue( "Stand" );
}

void CDnPlayerActor::EndCannonMode()
{
	m_bPlayerCannonMode = false;
	m_hCannonMonsterActor.Identity();
	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );
	if( pSox && pSox->IsExistItem(GetClassID()))
	{
		float fWeight = pSox->GetFieldFromLablePtr( GetClassID(), "_Weight" )->GetFloat();
		int fPressLevel = pSox->GetFieldFromLablePtr( GetClassID(), "_PressLevel" )->GetInteger();

		SetWeight(fWeight);
		SetPressLevel(fPressLevel);
	}
}

void CDnPlayerActor::ProcessNonLocalShootModeAction()
{
	if(!IsLocalActor())
		return;

	if( strcmp( GetCurrentAction(), "MOD_Stand" ) == NULL ) {
		if( strstr( m_szCustomAction.c_str(), "MOD_Shoot" ) ) {
			float fFrame = ( ( CDnActionBase::m_LocalTime - m_CustomActionTime ) / 1000.f ) * CDnActionBase::m_fFPS;
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

bool CDnPlayerActor::IsTransformSkill( int nSkillID )
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

	DNTableFileFormat* pMonsterSox = GetDNTable( CDnTableDB::TMONSTER_TRANS );
	DNTableFileFormat* pMonsterSkillSox = GetDNTable( CDnTableDB::TMONSTERSKILL_TRANS );
	if(!pMonsterSox || !pMonsterSkillSox) return;

	if( IsDestroy() ) return;

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

	if(IsTransformMode() == true)
	{
		int nActorIndex = pMonsterSox->IsExistItem(m_nMonsterMutationTableID) ? pMonsterSox->GetFieldFromLablePtr( m_nMonsterMutationTableID , "_ActorTableID" )->GetInteger() : 0;
		int nSkillTableIndex = pMonsterSox->IsExistItem(m_nMonsterMutationTableID) ? pMonsterSox->GetFieldFromLablePtr( m_nMonsterMutationTableID , "_SkillTable" )->GetInteger() : 0;

		if(GetStateBlow()->IsApplied(STATE_BLOW::BLOW_176))
		{
			CmdRemoveStateEffect(STATE_BLOW::BLOW_176);
			GetStateBlow()->Process( 0, 0.f );
		}
	
		DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
		if(!pActorSox || !pActorSox->IsExistItem(nActorIndex))
			return;

		if(nActorIndex > 0)
		{
			SwapSingleSkin( nActorIndex );
			ResetCustomAction();
			SetAction( "Stand", 0.f, 0.f );
		}
		else
			return;

		if( pMonsterSkillSox && pMonsterSkillSox->IsExistItem(nSkillTableIndex))
		{
			if(!m_vecTransformSkillList.empty()) // 이미 적용된 스킬이있을때는 지워준다. < 변신했다가 또변신하는경우 >
			{
				for (int i=0; i<(int)m_vecTransformSkillList.size(); i++)
				{
					if(m_vecTransformSkillList[i] != -1 )
						RemoveSkill(m_vecTransformSkillList[i]);
				}

				m_vecTransformSkillList.clear();
			}

			for (int i=0; i<PvPCommon::Common::MonsterMutationSkillColCount; i++)
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
			std::string strSkillVec = "";
			for(DWORD i=0; i<m_vecTransformSkillList.size();i++)
			{
				char szSkillIndex[256];
				sprintf(szSkillIndex, "%d;", m_vecTransformSkillList[i]);
				strSkillVec += szSkillIndex;
			}

			m_nAllowedSkill = CmdAddStateEffect( NULL, STATE_BLOW::BLOW_176, -1, strSkillVec.c_str(), true ); // 추가된 몬스터 스킬을 제외한 모든 스킬을 비활성화 한다.
		}

	}
	else if(IsTransformMode() == false)
	{
		SwapSingleSkin( -1 );
		SetActionQueue( IsDie() ? "Die" : m_strTransformEndAction.c_str() );

		for(UINT i=0; i<m_vecTransformSkillList.size(); i++)
		{
			if(m_vecTransformSkillList[i] != -1)
				RemoveSkill(m_vecTransformSkillList[i]);
		}
	
		if(m_nAllowedSkill > 0)
		{
			CmdRemoveStateEffectFromID(m_nAllowedSkill);
			m_nAllowedSkill = 0;
		}

		m_vecTransformSkillList.clear();
	}

	SetBattleMode(true);
	RefreshState();
}

void CDnPlayerActor::ToggleTransformMode( bool bTrue,int nMonsterMutatorTableID , bool bForce, const char* strEndAction )
{
	if( m_bTransformMode == bTrue && !bForce )
		return;

	m_bTransformMode = bTrue;
	m_nMonsterMutationTableID = nMonsterMutatorTableID;
	m_bRefreshTransformMode = true;
	m_strTransformEndAction = strEndAction;
}

void CDnPlayerActor::CmdShootMode(bool bTrue)
{
	m_bShootMode = bTrue;

	if(m_bShootMode)
		CmdStop("MOD_Shoot_Stand");
	else
		CmdStop("Stand");

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	DWORD dwUniqueID = GetUniqueID();
	Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
	Stream.Write( &m_bShootMode, sizeof(bool) );

	Send( eActor::SC_CMDSHOOTMODE, &Stream );
}

void CDnPlayerActor::CmdWarp( EtVector3 &vPos, EtVector2 &vLook, CDNUserSession* pGameSession, bool bCheckPlayerFollowSummonedMonster/*=false*/ )
{
	if(IsVehicleMode() && GetMyVehicleActor() )
		GetMyVehicleActor()->CmdWarp(vPos, vLook, pGameSession);

	CDnActor::CmdWarp( vPos, vLook, pGameSession, bCheckPlayerFollowSummonedMonster );
	if( m_pPlayerSpeedHackChecker ) m_pPlayerSpeedHackChecker->ResetInvalid();

	// #32426 소환체 컨트롤 기능 - 스테이지 이동 혹은 존 이동(CmdWarp)를 할 때 따라가야 되는
	// 이 플레이어가 소환한 몬스터 객체들 체크해서 처리.
	if( bCheckPlayerFollowSummonedMonster )
	{
		list<S_FOLLOWSTAGE_SUMMONED_MONSTER_INFO>::iterator iter = m_listSummonedMonstersFollowStageInfos.begin();
		for( iter; iter != m_listSummonedMonstersFollowStageInfos.end(); )
		{
			DnMonsterActorHandle hMonsterActor = iter->hMonster;
			if( hMonsterActor && false == hMonsterActor->IsDie() )
			{
				hMonsterActor->CmdWarp( vPos, vLook, pGameSession, false );
				hMonsterActor->ResetAggro();
				hMonsterActor->CmdAction( "Stand" );
				++iter;
			}
			else
			{
				iter = m_listSummonedMonstersFollowStageInfos.erase( iter );
			}
		}
	}
}

void CDnPlayerActor::RequestCooltimeParrySuccess( int iSkillID )
{	
	BYTE pBuffer[ 32 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 32 );
	
	Stream.Write( &iSkillID, sizeof(int) );

	Send( eActor::CS_COOLTIMEPARRY_SUCCESS, &Stream );
}

bool CDnPlayerActor::IsInvalidPlayerChecker()
{
	if( !m_pSession ) return false;
	if( m_pSession->GetHackPlayRestraintValue() <= 0 ) return false;
	if( m_pSession->GetHackAbuseDBValue() + m_nInvalidPlayerCheckCounter >= m_pSession->GetHackPlayRestraintValue() ) return true;
	return false;
}

void CDnPlayerActor::SwapSingleSkin( int nChangeActorTableID )
{
	if( m_nSwapSingleSkinActorID == nChangeActorTableID ) return;

	m_nSwapSingleSkinActorID	= nChangeActorTableID;
//	FreeAction();
#ifdef PRE_FIX_MEMOPT_EXT
	if (g_pDataManager == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );
	if( m_nSwapSingleSkinActorID == -1 ) {
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szSkinName, szAniName, szActName;
		g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, "_SkinName");
		g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, "_AniName");
		g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, m_nClassID, "_ActName");
#else
		std::string szSkinName = pSox->GetFieldFromLablePtr( m_nClassID, "_SkinName" )->GetString();
		std::string szAniName = pSox->GetFieldFromLablePtr( m_nClassID, "_AniName" )->GetString();
		std::string szActName = pSox->GetFieldFromLablePtr( m_nClassID, "_ActName" )->GetString();
#endif

		SAFE_RELEASE_SPTR( GetObjectHandle() );
		FreeAction();

		LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
		LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );
		SetAction( "Stand", 0.f, 0.f );

		for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) 
		{
			DnWeaponHandle hWeapon = GetWeapon(i);
			if( hWeapon ) 
			{
				LinkWeapon( i );
			}

			hWeapon = GetCashWeapon(i);
			if( hWeapon ) 
			{
				LinkCashWeapon( i );
			}
		}

		SAFE_RELEASE_SPTR( m_hSwapOriginalHandle );
		SAFE_DELETE( m_pSwapOriginalAction );

	}
	else 
	{
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szSkinName, szAniName, szActName;
		g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, m_nSwapSingleSkinActorID, "_SkinName");
		g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, m_nSwapSingleSkinActorID, "_AniName");
		g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, m_nSwapSingleSkinActorID, "_ActName");
#else
		std::string szSkinName = pSox->GetFieldFromLablePtr( m_nSwapSingleSkinActorID, "_SkinName" )->GetString();
		std::string szAniName = pSox->GetFieldFromLablePtr( m_nSwapSingleSkinActorID, "_AniName" )->GetString();
		std::string szActName = pSox->GetFieldFromLablePtr( m_nSwapSingleSkinActorID, "_ActName" )->GetString();
#endif

		if( !m_hSwapOriginalHandle && m_hObject ) 
		{
			m_hSwapOriginalHandle = EternityEngine::CreateAniObject( GetRoom(), CEtResourceMng::GetInstance().GetFullName( m_hObject->GetSkinFileName() ).c_str(), CEtResourceMng::GetInstance().GetFullName( m_hObject->GetAniHandle()->GetFileName() ).c_str() );
		}
		SAFE_RELEASE_SPTR( GetObjectHandle() );
		if( !m_pSwapOriginalAction ) 
		{
			m_pSwapOriginalAction = new CDnActionBase;
			m_pSwapOriginalAction->LoadAction( CEtResourceMng::GetInstance().GetFullName( CDnActionBase::m_szFileName ).c_str() );
		}

		SAFE_RELEASE_SPTR( GetObjectHandle() );
		FreeAction();

		LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
		LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

		SetAction( "Stand", 0.f, 0.f );
	}

	if( m_hObject ) 
	{
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );
	}

	if(m_pBubbleSystem)
		m_pBubbleSystem->Clear();
}

void CDnPlayerActor::InitializeEnchantPassiveSkills( void )
{
	// 강화 패시브 타입의 스킬을 찾아서 베이스 스킬에 수치를 적용하도록 한다.
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i = 0; i < m_vlhSkillList.size(); i++ ) 
	{
		DnSkillHandle hSkill = m_vlhSkillList[ i ];
#else
	for( DWORD i = 0; i < GetSkillCount(); ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
		if( CDnSkill::EnchantPassive == hSkill->GetSkillType() &&
			0 < hSkill->GetBaseSkillID() )
		{
			DnSkillHandle hEnchantPassiveSkill = hSkill;
			int iBaseSkillID = hEnchantPassiveSkill->GetBaseSkillID();
			DnSkillHandle hBaseSkill = FindSkill( iBaseSkillID );

			// 임시로 방어코드 넣음. 스킬리셋 캐쉬템이 제대로 올라가면 없애야 함.
			if( hBaseSkill )
				hBaseSkill->ApplyEnchantSkill( hEnchantPassiveSkill );
		}
	}
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
			hBaseSkill->ReleaseEnchantSkill();
			hBaseSkill->ApplyEnchantSkill( hNewSkill );
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
			DnSkillHandle hSkill = m_vlhSkillList[ i ];
#else
		for( DWORD i = 0; i < GetSkillCount(); ++i )
		{
			DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
			if( hSkill->GetBaseSkillID() == hBaseSkill->GetClassID() ) 
			{
				hEnchantPassiveSkill = hSkill;
				break;
			}
		}

		if( hEnchantPassiveSkill )
			hBaseSkill->ApplyEnchantSkill( hEnchantPassiveSkill );
	}
}

// 현재 엘리멘탈로드의 아이시 프랙션에서만 예외적으로 2가지의 상태효과 필터링을 걸고 있는데
// 둘 중에 하나만 hit 되길 원하기 때문에 먼저 체크된 것은 다음 필터링에 걸리지 않도록 한다. #28747
void CDnPlayerActor::OnHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex )
{
	if( STATE_BLOW::BLOW_041 == (STATE_BLOW::emBLOW_INDEX)iBlowIndex || 
		STATE_BLOW::BLOW_144 == (STATE_BLOW::emBLOW_INDEX)iBlowIndex )
	{
		m_mapIcyFractionHitted[ dwTargetActorUniqueID ] = true;
	}
}

bool CDnPlayerActor::CheckHitSignalStateEffectFilterException( DWORD dwTargetActorUniqueID, int iBlowIndex )
{
	bool bResult = true;

	if( STATE_BLOW::BLOW_041 == (STATE_BLOW::emBLOW_INDEX)iBlowIndex || 
		STATE_BLOW::BLOW_144 == (STATE_BLOW::emBLOW_INDEX)iBlowIndex )
	{
		if( m_mapIcyFractionHitted.end() != m_mapIcyFractionHitted.find( dwTargetActorUniqueID ) )
		{
			bResult = false;
		}
	}

	return bResult;
}

bool CDnPlayerActor::IsMyRelicMonster( DnActorHandle hActor )
{
	bool bResult = false;

	if( hActor && hActor->IsMonsterActor() )
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		if( pMonsterActor->IsClericRelicMonster() )
		{
			if( GetMySmartPtr() == pMonsterActor->GetSummonerPlayerActor() )
				bResult = true;
		}
	}

	return bResult;
}

INT64 CDnPlayerActor::GetCharacterDBID()
{ 
	return m_pSession ? m_pSession->GetCharacterDBID() : 0; 
}

// #26902 
// 임시로 클라이언트에게 이 직업으로 전직할 것을 명령.
// 이 시점부터는 스킬 레벨업 및 각종 조작이 불가하다.
// 게임 서버 쪽에서도 스킬 레벨업 등의 패킷이 오면 무시하도록 한다.
// 스테이지 이동시 곧바로 리셋시킨다.
bool CDnPlayerActor::CanChangeJob( int iJobID )
{
	// 게임서버에서는 실제 전직하지 않는다. 
	// 현재 직업에서 전직이 가능한 2차 직업인지 인증만해서 클라로 보내준다.
	DNTableFileFormat* pJobTable = GetDNTable( CDnTableDB::TJOB );

	// 현재 직업의 단계값과 루트 직업을 얻어옴.
	int iNowJob = m_pSession->GetUserJob();
	int iNowJobDeep = 0;
	int iNowRootJob = 0;
	for( int i = 0; i < pJobTable->GetItemCount(); ++i )
	{
		int iItemID = pJobTable->GetItemID( i );
		if( iItemID == iNowJob )
		{
			iNowJobDeep = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
			iNowRootJob = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
			break;
		}
	}

	int iJobIDToChange = iJobID;

	// 바꾸기 원하는 직업과 단계가 같거나 큰지 확인.
	bool bResult = false;
	map<int, int> mapRootJob;
	for( int i = 0; i < pJobTable->GetItemCount(); ++i )
	{
		int iItemID = pJobTable->GetItemID( i );
		if( iItemID == iJobIDToChange )
		{
			int iJobRootToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();
			if( iNowRootJob == iJobRootToChange )
			{
				int iJobDeepToChange = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
				if( iNowJobDeep < iJobDeepToChange )
				{
					// 부모 직업도 맞아야 함.
					int iParentJobID = pJobTable->GetFieldFromLablePtr( iItemID, "_ParentJob" )->GetInteger();
					if( iParentJobID == iNowJob )
					{
						bResult = true;
					}
					else
					{
						// 바꾸고자 하는 직업의 부모 직업이 현재 직업이 아님.
						wstring wszString = FormatW(L"현재 직업에선 전직 할 수 없는 직업입니다.!!\r\n");
						m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
					}
				}
				else
				{
					// 바꾸고자하는 직업이 아래 단계임. 못바꿈.
					wstring wszString = FormatW(L"같거나 낮은 단계의 직업으로 바꿀 수 없습니다!!\r\n");
					m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
				}
			}
			else
			{
				// 바꾸고자하는 직업이 다른 클래스임. 못바꿈.
				wstring wszString = FormatW(L"다른 클래스의 직업으로 바꿀 수 없습니다!!\r\n");
				m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
			}
		}
	}

	if( false == bResult )
	{
		wstring wszString = FormatW(L"잘못된 Job ID 입니다..\r\n");
		m_pSession->SendChat(CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str());
	}

	return bResult;
}


void CDnPlayerActor::SendTempJobChange( int iJobID )
{
	BYTE pBuffer[ 32 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 32 );
	Stream.Write( &iJobID, sizeof(int) );

	Send( eActor::SC_DO_TEMP_JOBCHANGE, &Stream );

	m_iTempChangedJob = iJobID;
}



// 임시 전직을 원래대로 복구할 것을 명령.
// 이 시점부터는 스킬 레벨업 및 각종 조작이 가능하다.
void CDnPlayerActor::EndAddTempSkillAndSendRestoreTempJobChange( void )
{
	BYTE pBuffer[ 32 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 32 );
	Stream.Write( &m_iTempChangedJob, sizeof(int) );

	Send( eActor::SC_RESTORE_TEMP_JOBCHANGE, &Stream );

	m_iTempChangedJob = 0;
}


void CDnPlayerActor::AddTempSkill( int iSkillID )
{
	// 임시로 만들려고 하는 스킬이 강화 패시브 스킬인 경우 
	// 베이스가 되는 스킬을 갖고 있는지 찾아서 없다면 역시 임시 스킬로 베이스 스킬로 추가한다.
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int iNeedBaseSkillID = 0;
	if( pSkillTable->IsExistItem( iSkillID ) )
		iNeedBaseSkillID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_BaseSkillID" )->GetInteger();

	DNVector(int) vlSkillsToAdd;
	if( 0 < iNeedBaseSkillID )
	{
		// 강화 대상 스킬을 갖고 있지 않은 상태라면 이것도 새로 생성하도록 벡터에 넣어줌.
		if( false == IsExistSkill( iNeedBaseSkillID ) )
			vlSkillsToAdd.push_back( iNeedBaseSkillID );
	}
	vlSkillsToAdd.push_back( iSkillID );

	for( int i = 0; i < (int)vlSkillsToAdd.size(); ++i )
	{
		int iSkillIDToAdd = vlSkillsToAdd.at( i );
		bool bSuccess = AddSkill( iSkillIDToAdd, 1, CDnSkill::PVE );
		if( bSuccess )
		{
			DnSkillHandle hSkill = FindSkill( iSkillIDToAdd );
			if( hSkill )
			{
				hSkill->AsTempSkill();

				// 2차 전직스킬이라서 현재 사용할 수 없는 스킬이라면,
				// 현재 캐릭터 레벨 및 직업에 맞게 객체 값을 바꿔준다.
				if( GetLevel() < hSkill->GetLevelLimit() )
					hSkill->SetLevelLimit( GetLevel() );

				if( false ==  IsPassJob( hSkill->GetNeedJobClassID() ) )
					hSkill->SetNeedJobClassID( GetJobClassID() );

				// 클라로 임시 스킬 추가 보냄.
				BYTE pBuffer[ 32 ] = { 0 };
				CPacketCompressStream Stream( pBuffer, 32 );
				Stream.Write( &iSkillID, sizeof(int) );

				Send( eActor::SC_ADD_TEMP_SKILL, &Stream );

				m_bTempSkillAdded = true;
			}
		}
	}
}

void CDnPlayerActor::RemoveAllTempSkill( void )
{
#ifndef PRE_FIX_SKILLLIST
	DNVector(DnSkillHandle)::iterator iter = m_vlhSkillList.begin();
	for( iter; iter != m_vlhSkillList.end(); )
	{
		DnSkillHandle hSkill = *iter;
		if( hSkill->IsTempSkill() )
		{
			// 2차전직을 위한 임시적인 기능이므로 MASkillUser 가 아닌 여기서 직접 
			// 스킬 객체 삭제 처리를 함. 추후에 이쪽관련 추가 요청이 있는 경우 
			// MASkillUser::RemoveSkill() 를 사용해야할 수도 있음.
			int iSkillID = hSkill->GetClassID();
			OnRemoveSkill( hSkill );

			iter = m_vlhSkillList.erase( iter );

			// 클라로 임시 스킬 제거 보냄.
			BYTE pBuffer[ 32 ] = { 0 };
			CPacketCompressStream Stream( pBuffer, 32 );
			Stream.Write( &iSkillID, sizeof(int) );

			Send( eActor::SC_REMOVE_TEMP_SKILL, &Stream );
		}
		else
			++iter;
	}
#else
	vector<int> vlTempSkills;
	for( DWORD i = 0; i < GetSkillCount(); ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
		if( hSkill->IsTempSkill() )
		{
			vlTempSkills.push_back( hSkill->GetClassID() );
		}
	}

	for( int i = 0; i < (int)vlTempSkills.size(); ++i )
	{
		int iSkillID = vlTempSkills.at( i );
		RemoveSkill( iSkillID );

		// 클라로 임시 스킬 제거 보냄.
		BYTE pBuffer[ 32 ] = { 0 };
		CPacketCompressStream Stream( pBuffer, 32 );
		Stream.Write( &iSkillID, sizeof(int) );

		Send( eActor::SC_REMOVE_TEMP_SKILL, &Stream );
	}
#endif // #ifndef PRE_FIX_SKILLLIST

	m_bTempSkillAdded = false;
}

#ifdef PRE_ADD_48714
#include "DNMailSender.h"
#endif		//#ifdef PRE_ADD_48714
void CDnPlayerActor::OnInvalidPlayerChecker( int nValue )
{
	m_nInvalidPlayerCheckCounter += nValue;
	OutputDebug( "OnInvalidPlayerChecker : %d, (%d)\n", m_nInvalidPlayerCheckCounter, nValue );

	if( m_pSession && m_pSession->GetHackPlayRestraintValue() > 0 )
	{
#if defined (PRE_ADD_ABUSE_ACCOUNT_RESTRAINT)
		//제재가 들어갈경우 끊는 판단에 *2 제외
		if( m_nInvalidPlayerCheckCounter >= m_pSession->GetHackPlayRestraintValue() )
		{
			//일단은 한국에만 처리되어진다
			if (m_pSession->GetHackCharacterCntWithoutMe() > 0)
			{
				//지금현재 캐릭터를 제외한 캐릭터가 하나 있는데 하나 더 걸렸다 36080이슈에 의하여 제재처리한다.
#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100070, m_pSession->m_eSelectedLanguage);
				std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100071, m_pSession->m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100070);
				std::wstring wszRestraintReasonForDolis = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100071);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
				m_pSession->GetDBConnection()->QueryAddRestraint(m_pSession, DBDNWorldDef::RestraintTargetCode::Account, DBDNWorldDef::RestraintTypeCode::ConnectRestraint, wszRestraintReason.c_str(), wszRestraintReasonForDolis.c_str(), 9999, DBDNWorldDef::RestraintDolisReasonCode::AbuseRestraintCode);
			}
			m_pSession->DetachConnection( L"InvalidPlayerCheckCounter" );
		}
#else
#ifdef PRE_ADD_48714
		if(m_nInvalidPlayerCheckCounter >= m_pSession->GetHackPlayRestraintValue())
		{
			if (m_pSession->GetDBConnection())
			{
#if defined (_TW)
				WCHAR wszBuf[100];
				wsprintf( wszBuf, L"Invalidcount Reached limit Value");
				m_pSession->GetDBConnection()->QueryAddAbuseLog(m_pSession, ABUSE_TWN_EXTENDLOG, wszBuf);

#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 109049, m_pSession->m_eSelectedLanguage);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
				std::wstring wszRestraintReason = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 109049);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

				m_pSession->GetDBConnection()->QueryAddRestraint(m_pSession, DBDNWorldDef::RestraintTargetCode::Character, DBDNWorldDef::RestraintTypeCode::TradeRestraint, wszRestraintReason.c_str(), wszRestraintReason.c_str(), 9999, DBDNWorldDef::RestraintDolisReasonCode::AbuseTradeRestraintCode);
#endif		//#if defined (_TW)
				CDNMailSender::Process(m_pSession, AbuseLog::Common::AbuseLog_Reached_MailID);
			}
		}
#endif		//#ifdef PRE_ADD_48714

		if( m_nInvalidPlayerCheckCounter >= m_pSession->GetHackPlayRestraintValue()*2 )
		{
			m_pSession->DetachConnection( L"InvalidPlayerCheckCounter" );
		}
#endif		//#if defined (PRE_ADD_ABUSE_ACCOUNT_RESTRAINT)
	}
}


void CDnPlayerActor::OrderUseSkillToMySummonedMonster( OrderMySummonedMonsterStruct* pStruct )
{
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_215 ) )
	{
		DNVector( DnBlowHandle ) vlhBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_215, vlhBlows );
		_ASSERT( 1 == (int)vlhBlows.size() );
		if( false == vlhBlows.empty() )
		{
			// 정의된 반경 안에 내가 소환한 몬스터가 있는지 확인.
			float fRangeSQ = pStruct->fOrderRange * pStruct->fOrderRange;

			DNVector( DnMonsterActorHandle ) vlhMonsters;
			for( list<DnMonsterActorHandle>::const_iterator iter = m_listSummonMonster.begin();
				iter != m_listSummonMonster.end(); ++iter )
			{
				DnMonsterActorHandle hMonster = *iter;
				if( hMonster && false == hMonster->IsDie() )
				{
					float fDistanceWithThisMonsterSQ = EtVec3LengthSq( &EtVector3(*GetPosition() - *hMonster->GetPosition()) );
					if( fDistanceWithThisMonsterSQ < fRangeSQ )
					{
						vlhMonsters.push_back( hMonster );
					}
				}
			}

			map<int, list<DnMonsterActorHandle> >::iterator iterMap = m_mapSummonMonsterByGroup.begin();
			for( iterMap; iterMap != m_mapSummonMonsterByGroup.end(); ++iterMap )
			{
				const list<DnMonsterActorHandle>& listSummonMonster = iterMap->second;
				for( list<DnMonsterActorHandle>::const_iterator iterList = listSummonMonster.begin();
					iterList != listSummonMonster.end(); ++iterList )
				{
					DnMonsterActorHandle hMonster = *iterList;
					if( hMonster && false == hMonster->IsDie() )
					{
						float fDistanceWithThisMonsterSQ = EtVec3LengthSq( &EtVector3(*GetPosition() - *hMonster->GetPosition()) );
						if( fDistanceWithThisMonsterSQ < fRangeSQ )
						{
							vlhMonsters.push_back( hMonster );
						}
					}
				}
			}

			if( false == vlhMonsters.empty() )
			{
				CDnOrderMySummonedMonsterBlow* pBlow = static_cast<CDnOrderMySummonedMonsterBlow*>( vlhBlows.front().GetPointer() );
				int iSkillID = pBlow->GetSkillID();

				for( int i = 0; i < (int)vlhMonsters.size(); ++i )
				{
					DnMonsterActorHandle hMonster = vlhMonsters.at( i );
					if( hMonster->GetMonsterClassID() == pStruct->nMonsterID )
					{
						// TODO: AI 쪽에 문의해서 스킬 사용 예약을 해둬야 함.
						if( hMonster->IsExistSkill( iSkillID ) )
						{
							MAAiScript* pScript = static_cast<MAAiScript*>(hMonster->GetAIBase());

							if( hMonster->GetAggroTarget() )
								pScript->GetMonsterSkillAI()->AddWaitOrderCount( hMonster, iSkillID );
						}
					}
				}
			}
		}
	}
}

bool CDnPlayerActor::CheckSkipAirCondition(int iSkill)
{
	if(IsAir())
	{
		bool bHaveGroundCheck = false;
		DnSkillHandle hSkill = FindSkill(iSkill);

		if(hSkill)
		{
			if(hSkill->IsExistUsableChecker(IDnSkillUsableChecker::GROUNDMOVABLE_CHECKER))
				bHaveGroundCheck = true;
		}

		if(bHaveGroundCheck)
		{
			return true;
		}
	}

	return false;
}


bool CDnPlayerActor::HasSameGlobalIDSkill( int iSkillID )
{
	bool bResult = false;
	DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	int iGlobalSkillGroupID = pSkillTable->GetFieldFromLablePtr( iSkillID, "_GlobalSkillGroup" )->GetInteger();

	if( 0 < iGlobalSkillGroupID )
	{ 
		for( DWORD i = 0; i < GetSkillCount(); ++i )
		{
		 	DnSkillHandle hExistSkill = GetSkillFromIndex( i );
			if( hExistSkill &&
				hExistSkill->GetGlobalSkillGroupID() == iGlobalSkillGroupID )
			{
				bResult = true;
				break;
			}
		}
	}

	return bResult;
}

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
bool CDnPlayerActor::IsAllowCallSkillProcess( float fDelta )
{
	return m_FrameSkipCallSkillProcess.Update( fDelta );
}
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

void CDnPlayerActor::RemoveAllBubbles( bool bRemoveEvent )
{
	if( m_pBubbleSystem )
		m_pBubbleSystem->RemoveAllBubbles( bRemoveEvent );
}

void CDnPlayerActor::ApplyEnchantSkillOnceFromBubble( int iTargetSkillID, int iEnchantSkillID )
{
	DnSkillHandle hEnchantSkill;
	map<int, DnSkillHandle>::iterator iter = m_mapEnchantSkillFromBubble.find( iEnchantSkillID );
	if( m_mapEnchantSkillFromBubble.end() != iter )
	{
		hEnchantSkill = iter->second;
	}
	else
	{
		// pvp/pve 모두 데이터가 있어야 제대로 생성됨.
		hEnchantSkill = CDnSkill::CreateSkill( GetMySmartPtr(), iEnchantSkillID, 1 );
		m_mapEnchantSkillFromBubble.insert( make_pair(iEnchantSkillID, hEnchantSkill) );
	}

	DnSkillHandle hTargetSkill = FindSkill( iTargetSkillID );
	_ASSERT( hTargetSkill );
	_ASSERT( hEnchantSkill );

	if( hTargetSkill && hEnchantSkill )
	{
		// 이미 강화된 스킬이 대상이 될 수는 없음.
		bool bIsEnchantedAlready = hTargetSkill->IsEnchantedSkill();
		_ASSERT( false == bIsEnchantedAlready );
		if( false == bIsEnchantedAlready )
		{
			hTargetSkill->ApplyEnchantSkillOnceFromBubble( hEnchantSkill );
		}
	}
}

bool CDnPlayerActor::OnApplySpectator(bool bEnable)
{
	CDNGameRoom* pGameRoom = GetGameRoom();
	if( pGameRoom == NULL || pGameRoom->bIsPvPRoom() == false || pGameRoom->GetPvPGameMode()->bIsAllKillMode() == false )
		return false;

	if( bEnable )
	{
		if( IsProcessSkill() )
			CancelUsingSkill();
	
		if(IsBattleMode())
			SetBattleMode(false);
	}

	return true;
}

bool CDnPlayerActor::IsSpectatorMode()
{
	return IsAppliedThisStateBlow(STATE_BLOW::BLOW_230);
}

void CDnPlayerActor::ChangeSkillLevelUp(int nSkillID, int nOrigLevel)
{
	if( IsProcessSkill() )
		return;

	DnSkillHandle hSkill = FindSkill(nSkillID);
	if( hSkill )
	{
		if (hSkill->GetElapsedDelayTime() > 0.0f || hSkill->IsToggleOn())
			return;
	}
	__super::ChangeSkillLevelUp(nSkillID, nOrigLevel);

	DnSkillHandle hChangedSkill = FindSkill( nSkillID );
	if( hChangedSkill )
	{
		ReplacementGlyph( hChangedSkill );
	}
}

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
		int iSkillLevelDataType = CDnSkill::PVE;
		if( GetGameRoom()->bIsPvPRoom() )
			iSkillLevelDataType = CDnSkill::PVP;

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

void CDnPlayerActor::ApplyEventStateBlow()
{
	int nAddStateEffectIndex = -1;
	DnBlowHandle hBlow;

#ifdef PRE_ADD_WEEKLYEVENT
	if (CDnWorld::GetInstance(GetRoom()).GetMapSubType() != EWorldEnum::MapSubTypeNest && !GetGameRoom()->bIsPvPRoom() )
	{
		bool bRfreshHP = false;
		float fCurrentHpRatio = (float)GetHP() / (float)GetMaxHP();

		int nThreadID = GetGameRoom()->GetServerID();

		float fEventHp = g_pDataManager->GetWeeklyEventValuef(WeeklyEvent::Player, GetClassID(), WeeklyEvent::Event_1, nThreadID);
		if (fEventHp != 0.f)
		{
			std::string strValue;
			strValue.append(boost::lexical_cast<std::string>(fEventHp));

			nAddStateEffectIndex = CmdAddStateEffect(NULL, STATE_BLOW::BLOW_058, -1, strValue.c_str(), false, true , true );
			m_vecEventEffectList.push_back( nAddStateEffectIndex );
			bRfreshHP = true;
		}
		
		float fEventAttack = g_pDataManager->GetWeeklyEventValuef(WeeklyEvent::Player, GetClassID(), WeeklyEvent::Event_2, nThreadID);
		if (fEventAttack != 0.f)
		{
			std::string strValue;
			strValue.append(boost::lexical_cast<std::string>(fEventAttack));

			nAddStateEffectIndex = CmdAddStateEffect(NULL, STATE_BLOW::BLOW_002, -1, strValue.c_str(), false, true , true );
			m_vecEventEffectList.push_back( nAddStateEffectIndex );

			nAddStateEffectIndex = CmdAddStateEffect(NULL, STATE_BLOW::BLOW_029, -1, strValue.c_str(), false, true , true );
			m_vecEventEffectList.push_back( nAddStateEffectIndex );
		}

		float fEventDefense = g_pDataManager->GetWeeklyEventValuef(WeeklyEvent::Player, GetClassID(), WeeklyEvent::Event_3, nThreadID);
		if (fEventDefense != 0.f)
		{
			std::string strValue;
			strValue.append(boost::lexical_cast<std::string>(fEventDefense));

			nAddStateEffectIndex = CmdAddStateEffect(NULL, STATE_BLOW::BLOW_004, -1, strValue.c_str(), false, true , true );
			m_vecEventEffectList.push_back( nAddStateEffectIndex );

			nAddStateEffectIndex = CmdAddStateEffect(NULL, STATE_BLOW::BLOW_094, -1, strValue.c_str(), false, true , true );
			m_vecEventEffectList.push_back( nAddStateEffectIndex );
		}
	
		if( bRfreshHP )
		{
			GetStateBlow()->Process( 0 , 0 );
			CmdRefreshHPSP( (INT64)(GetMaxHP() * fCurrentHpRatio) , GetSP() );
		}
	}
#endif

}

void CDnPlayerActor::RemoveEventStateBlow()
{
	for( DWORD index = 0; index < m_vecEventEffectList.size() ; ++index )
	{
		CmdRemoveStateEffectFromID( m_vecEventEffectList[index] );
	}
	m_vecEventEffectList.clear();
}

bool CDnPlayerActor::CheckSkillAction( const char *szActionName )
{
	if( strstr( szActionName, "Skill_" ) 
		|| strstr( szActionName, "ChargeShoot_" ) 
		|| strstr( szActionName, "GuildSkill_" ) 
		|| strstr( szActionName, "Throw_" ) 
		|| strstr( szActionName, "Tumble_" )
		|| strstr( szActionName, "AerialEvasion" ) )
	{
		return true;
	}

	return false;
}

void CDnPlayerActor::MakeEquipAndPassiveState( CDnState &State )
{
	State.ResetState();
	State = MakeEquipState();
	
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

#ifdef PRE_ADD_COSTUME_SKILL

void CDnPlayerActor::RefreshCostumeSkill( int nSkillIndex, int nSkillLevel )
{
	if( m_nCostumeSkillID == nSkillIndex )
		return;

	if( m_nCostumeSkillID > 0 ) 
		RemoveSkill( m_nCostumeSkillID );

	if( nSkillIndex > 0 ) 
	{
		AddSkill( nSkillIndex, nSkillLevel );
		DnSkillHandle hSkill = FindSkill( nSkillIndex );
		if( hSkill ) 
		{
			if( hSkill->GetSkillType() != CDnSkill::SkillTypeEnum::Active )
			{
				RemoveSkill( nSkillIndex );
				return;
			}

			hSkill->OnBeginCoolTime();
		}
	}

	m_nCostumeSkillID = nSkillIndex;
}

#endif // PRE_ADD_COSTUME_SKILL

#ifdef PRE_ADD_VEHICLE_SPECIAL_ACTION
void CDnPlayerActor::ReportInvalidAction()
{
	if( m_pPlayerActionChecker )
		((CDnPlayerActionChecker*)m_pPlayerActionChecker)->OnInvalidAction();
}
#endif