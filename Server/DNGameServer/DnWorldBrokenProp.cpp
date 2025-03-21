#include "StdAfx.h"
#include "DnWorldBrokenProp.h"
#include "DnTableDB.h"
#include "DnWorld.h"
#include "DnWeapon.h"
#include "DnDamageBase.h"
#include "DnProjectile.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#include "DNUserSession.h"
#include "DnPropState.h"
#include "DnPropCondition.h"
#include "DnPropStateDoAction.h"
#include "DnPropStateTrigger.h"
#include "DnPropCondiDurability.h"
#include "DNLogConnection.h"
#include "DNGameTask.h"
#include "DnPlayerActor.h"
#include "DnCannonMonsterActor.h"
#include "DnMonsterActor.h"
#include "DnStateBlow.h"

CDnWorldBrokenProp::CDnWorldBrokenProp( CMultiRoom *pRoom )
: CDnWorldActProp( pRoom )
//, CDnDamageBase( DamageObjectTypeEnum::Prop )
{
	m_nDurability = 0;
	//m_nBreakActionCount = 0;
	m_bHittable = false;
	m_bNoDamage = false;

	m_nItemDropGroupTableID = 0;
	m_pLastHitObject = NULL;
	m_bBroken = false;

	m_nLastHitUniqueID = -1;

	m_iLastRandomSeed = 0;
	m_bHitted = false;
	m_pBrokenActionState = NULL;
	m_pTriggerActionState = NULL;
	m_LastDamageTime = 0;
}

CDnWorldBrokenProp::~CDnWorldBrokenProp()
{
	SAFE_DELETE_VEC( m_VecDropItemList );
	ReleasePostCustomParam();
}

bool CDnWorldBrokenProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	//*
	//DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );
	//m_nDurability = pSox->GetFieldFromLablePtr( nTableID, "_Durability" )->GetInteger();

	//m_nItemDropTableID = pSox->GetFieldFromLablePtr( nTableID, "_ItemDropTableIndex" )->GetInteger();
	//if( m_nItemDropTableID > 0 ) {
	//	CDnDropItem::CalcDropItemList( GetRoom(), m_nItemDropTableID, m_VecDropItemList );

	//	for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
	//		CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID );
	//	}
	//}
	//*/

	if( GetData() ) {
		BrokenStruct *pStruct = (BrokenStruct *)GetData();
		m_nDurability = pStruct->nDurability;

		if( -1 != m_nDurability )
		{
			if( 0 == m_nDurability ) m_nDurability = 1;
			m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
			if( m_nItemDropGroupTableID > 0 ) {
				CDnDropItem::CalcDropItemList( GetRoom(), Dungeon::Difficulty::Max, m_nItemDropGroupTableID, m_VecDropItemList );

				if (m_VecDropItemList.empty()){
					// 프랍로그
					// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, 0, nTableID);	// 스테이지 로그 090226
				}
				else {
					for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID, m_VecDropItemList[i].nEnchantID ) == false )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList[i].nItemID ) == false )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
						{
							m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
							i--;
							continue;
						}

						// 프랍로그
						// g_pLogConnection->QueryLogStage(GetRoom()->GetRoomID(), pTask->GetMapTableID(), 0, m_VecDropItemList[i].nItemID, nTableID);	// 스테이지 로그 090226
					}
				}
			}
		}
	}

	return true;
}



bool CDnWorldBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{	
	// FSM 을 BrokenProp에 맞게 초기화 해준다..
	// 내구도가 -1 인 프랍은 broken이 아님..
	if( -1 == m_nDurability )
		return true;
	
	// state 생성
	CDnPropState* pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	CDnPropState* pHitActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	CDnPropState* pItemDropState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::ITEM_DROP );
	CDnPropState* pTriggerState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::TRIGGER );
	CDnPropState* pBrokenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	static_cast<CDnPropStateDoAction*>(pHitActionState)->AddActionName( "Hit" );
	m_pBrokenActionState = static_cast<CDnPropStateDoAction*>(pBrokenActionState);
	static_cast<CDnPropStateTrigger*>(pTriggerState)->SetFuncName( "CDnWorldProp::OnBrokenProp" );
	m_pTriggerActionState = static_cast<CDnPropStateTrigger*>(pTriggerState);

	// 조건 생성
	CDnPropCondition* pHitCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::IS_HIT );
	CDnPropCondition* pNULLCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::NULL_CONDITION );
	CDnPropCondition* pDurabilityCondition = CDnPropCondition::Create( GetMySmartPtr(), CDnPropCondition::COMPARE_DURABILITY );
	static_cast<CDnPropCondiDurability*>(pDurabilityCondition)->Initialize( 0, CDnPropCondiDurability::LESS_EQUAL );

	m_pFSM->AddCondition( pHitCondition );
	m_pFSM->AddCondition( pNULLCondition );
	m_pFSM->AddCondition( pDurabilityCondition );

	m_pFSM->AddState( pNormalState );
	m_pFSM->AddState( pHitActionState );
	m_pFSM->AddState( pItemDropState );
	m_pFSM->AddState( pTriggerState);
	m_pFSM->AddState( pBrokenActionState );

	pNormalState->AddTransitState( pHitActionState, pHitCondition );
	pHitActionState->AddTransitState( pNormalState, pNULLCondition );
	pNormalState->AddTransitState( pTriggerState, pDurabilityCondition );
	pTriggerState->AddTransitState( pItemDropState, pNULLCondition );
	pItemDropState->AddTransitState( pBrokenActionState, pNULLCondition );
	
	m_pFSM->SetEntryState( pNormalState );

	bool bResult = CDnWorldActProp::Initialize( pParentSector, szPropName, vPos, vRotate, vScale );
	if( !bResult )
		return false;

	return true;
}


bool CDnWorldBrokenProp::CreateObject()
{
	bool bResult = CDnWorldActProp::CreateObject();

	if( m_pBrokenActionState )
	{
		if( -1 == m_nDurability )
			return true;

		if( bResult ) {
			char szTemp[16];
			for( int i=0; ; i++ ) {
				sprintf_s( szTemp, "Break_%c", 'a' + i );
				if( IsExistAction( szTemp ) == false ) break;

				m_pBrokenActionState->AddActionName( szTemp );
				//m_nBreakActionCount++;
			}
		}
	}

	return bResult;
}

void CDnWorldBrokenProp::UpdatePropBreakToHitter( CDnDamageBase* pHitter )
{
	if( pHitter )
	{
		DnActorHandle hActor = pHitter->GetActorHandle();
		if( hActor )
		{
			if ( hActor->IsPlayerActor() )
			{
				CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(hActor.GetPointer());
				pPlayer->UpdatePropBreak();
			}
			else
			if( CDnActorState::Cannon == hActor->GetActorType() )
			{
				CDnCannonMonsterActor* pCannonMonsterActor = static_cast<CDnCannonMonsterActor*>(hActor.GetPointer());
				DnActorHandle hPlayerActor = pCannonMonsterActor->GetMasterPlayerActor();
				if( hPlayerActor && hPlayerActor->IsPlayerActor() )
				{
					CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(hPlayerActor.GetPointer());
					pPlayer->UpdatePropBreak();
				}
			}
		}
	}
}

void CDnWorldBrokenProp::CalcDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam )
{

//	if( m_nDurability <= 0 ) return;
	if( m_bDestroy == true ) return;
	if( m_bBroken ) return;

	m_pLastHitObject = pHitter;
	m_LastDamageTime = HitParam.RemainTime;
	m_nLastHitUniqueID = HitParam.iUniqueID;

	m_nDurability -= (int)( HitParam.fDurability * 100.f );

	// NOTE: 트리거에서 IsBroken() 사용함.. 다른 함수들도 체크해봐야 할듯.
	// 트리거 관련해선 상황이 변경되자마자 곧바로 전이 체크를 돌려야 한다.
	if( m_nDurability <= 0 )
	{
		m_bBroken = true;

		//소환 몬스터일 경우 주인 액터를 찾아서 설정 해야 함.
		DnActorHandle hHitterActor;

		if (pHitter)
			hHitterActor = pHitter->GetActorHandle();

		//몬스터 액터인 경우 
		if (hHitterActor && hHitterActor->IsMonsterActor())
		{
			DnActorHandle hMasterActor;
			CDnMonsterActor* pMonsterActor = NULL;

			pMonsterActor = static_cast<CDnMonsterActor*>(hHitterActor.GetPointer());

			if (pMonsterActor)
				hMasterActor = pMonsterActor->GetSummonerPlayerActor();

			//정상적인 주인 액터를 찾았으면, hHitterActor를 주인 액터로 변경한다.
			if (hMasterActor)
				hHitterActor = hMasterActor;
		}
		
		m_pTriggerActionState->AddFuncParam( "LastBrokenPropActor", hHitterActor ? hHitterActor->GetUniqueID() : -1 );

		m_pFSM->Process( 0, 0.0f );
		
		UpdatePropBreakToHitter( pHitter );
//		SetLastAccessActor( pHitter->GetActorHandle() );
	}

	//if( m_nDurability <= 0 ) {
	//	CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnWorldProp::OnBrokenProp", m_LocalTime, 0.f );
	//	m_bBroken = true;
	//	m_nDurability = 0;

	//	if( m_nBreakActionCount > 0 ) {
	//		char szTemp[32];
	//		sprintf_s( szTemp, "Break_%c", 'a' + _rand(GetRoom())%m_nBreakActionCount );
	//		SetActionQueue( szTemp );
	//	}

	//	// 아이템 드랍 해주자~
	//	DropItems();
	//}
	//else {
	//	SetActionQueue( "Hit" );
	//}

	if( HitParam.hWeapon ) {
		if( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) {
			CDnProjectile *pProjectile = static_cast<CDnProjectile *>(HitParam.hWeapon.GetPointer());
			if( pProjectile ) pProjectile->OnDamageSuccess( CDnActor::Identity(), HitParam );
			else HitParam.hWeapon->SetDestroy();
		}
	}

	DnActorHandle hHitterActor;
	if (pHitter)
		hHitterActor = pHitter->GetActorHandle();

	if (hHitterActor && hHitterActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_179))
	{
		DnActorHandle hTargetActor;

		DNVector(DnBlowHandle) vlhBlows;
		hHitterActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_179, vlhBlows );

		int iNumBlow = (int)vlhBlows.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			DnBlowHandle hBlow = vlhBlows.at(i);
			if (hBlow)
				hBlow->OnTargetHit(hTargetActor);
		}
	}
}

void CDnWorldBrokenProp::OnDamage( CDnDamageBase *pHitter, CDnDamageBase::SHitParam &HitParam )
{
	if( -1 == m_nDurability )
		return;

	int nSeed = CRandom::Seed(GetRoom());
	_srand( GetRoom(), nSeed );
	m_iLastRandomSeed = nSeed;

	CalcDamage( pHitter, HitParam );

	// 이 부분도 MPTransAction 을 상속받고 있는지라 어쩔 수 없네..
	switch( pHitter->GetDamageObjectType() ) 
	{
		case CDnDamageBase::DamageObjectTypeEnum::Actor:
			{
				DnActorHandle hActor = pHitter->GetActorHandle();
				if( !hActor ) break;
				// Request Damage
				BYTE pBuffer[128];
				CPacketCompressStream Stream( pBuffer, 128 );

				DWORD dwUniqueID = hActor->GetUniqueID();
				DWORD dwWeaponUniqueID = -1;
				INT64 nWeaponSerialID = -1;
				bool bSendWeaponSerialID = false;

				if( HitParam.hWeapon ) {
					if( HitParam.hWeapon->GetSerialID() == -1 ) 
						dwWeaponUniqueID = HitParam.hWeapon->GetUniqueID();
					else {
						bSendWeaponSerialID = true;
						nWeaponSerialID = HitParam.hWeapon->GetSerialID();
					}
				}

				Stream.Write( &nSeed, sizeof(int) );
				Stream.Write( &dwUniqueID, sizeof(DWORD) );

				Stream.Write( &m_nDurability, sizeof(int) );
				Stream.Write( &bSendWeaponSerialID, sizeof(bool) );
				if( bSendWeaponSerialID ) Stream.Write( &nWeaponSerialID, sizeof(INT64) );
				else Stream.Write( &dwWeaponUniqueID, sizeof(DWORD) );
				Stream.Write( &HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Write( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

				OnDamageWriteAdditionalPacket( HitParam, &Stream );

				Send( eProp::SC_ONDAMAGE, &Stream );
			}
			break;
		case CDnDamageBase::DamageObjectTypeEnum::Prop:
			break;
	}

	m_bHitted = true;
}


bool CDnWorldBrokenProp::IsHittedAndReset( void )
{
	bool bResult = m_bHitted;

	m_bHitted = false;

	return bResult;
}


bool CDnWorldBrokenProp::IsHittable( CDnDamageBase *pHitter, LOCAL_TIME LocalTime, int iHitUniqueID /*= -1*/ )
{
	if( m_bBroken ) return false;

	if( false == IsShow() ) return false;
	
	if( m_pLastHitObject == pHitter )
	{
		if( -1 != m_nLastHitUniqueID )
		{
			if( m_nLastHitUniqueID == iHitUniqueID )
			{
				if( m_LastDamageTime > LocalTime ) return false;
			}
		}
		else
		if( m_LastDamageTime > LocalTime ) return false;
	}
	if( m_bNoDamage ) return false;

	return m_bHittable;
}

void CDnWorldBrokenProp::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_CanHit:
			{
				CanHitStruct *pStruct = (CanHitStruct *)pPtr;
				m_bHittable = ( pStruct->bHittable == TRUE ) ? true : false;
			}
			break;
	}
	CDnWorldActProp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );
}

// BrokenProp 은 난입시에만 맞추어주면 된다.
void CDnWorldBrokenProp::OnSyncComplete( CDNUserSession* pBreakIntoGameSession/*=NULL*/ )
{
	if( !pBreakIntoGameSession )
		return;

	CmdShow( true, pBreakIntoGameSession );
	
	CDnWorldActProp::OnSyncComplete( pBreakIntoGameSession );
}


void CDnWorldBrokenProp::_OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_nDurability = 0;
	m_bBroken = true;
	m_pBrokenActionState->UseCmdAction( true );
	m_pFSM->Process( 0, 0.0f );
}