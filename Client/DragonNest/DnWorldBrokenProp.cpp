#include "StdAfx.h"
#include "DnWorldBrokenProp.h"
#include "DnTableDB.h"
#include "DnWorld.h"
#include "DnWeapon.h"
#include "DnProjectile.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnPropState.h"
#include "DnPropStateDoAction.h"
#include "DnPropCondition.h"
#include "DnPropCondiDurability.h"
#include "DnLocalPlayerActor.h"
#include "DnCannonMonsterActor.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnWorldBrokenProp::CDnWorldBrokenProp()
//: CDnDamageBase( DamageObjectTypeEnum::Prop )
{
	m_nDurability = 0;
	m_nBreakActionCount = 0;
	m_nHitParticle = -1;
	m_fSoundRange = 0.f;
	m_fRollOff = 0.f;
	m_nItemDropGroupTableID = 0;

	m_iLastRandomSeed = 0;
	m_bHitted = false;
	m_pBrokenActionState = NULL;
	m_bBroken = false;
	m_bLifeTimeEnd = false;
}

CDnWorldBrokenProp::~CDnWorldBrokenProp()
{
	if( m_nHitParticle != -1 ) EternityEngine::DeleteParticleData( m_nHitParticle );
	for( DWORD i=0; i<m_nVecHitSoundList.size(); i++ ) {
		CEtSoundEngine::GetInstance().RemoveSound( m_nVecHitSoundList[i] );
	}

	ReleasePostCustomParam();
}

bool CDnWorldBrokenProp::InitializeTable( int nTableID )
{
	if( CDnWorldActProp::InitializeTable( nTableID ) == false ) return false;

	LoadHitEffectData( nTableID );
	m_bIsStaticCollision = false;

	if( GetData() ) {
		BrokenStruct *pStruct = (BrokenStruct *)GetData();
		m_nDurability = pStruct->nDurability;
	
		if( -1 != m_nDurability )
		{
			if( 0 == m_nDurability ) m_nDurability = 1;
			m_nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
			if( m_nItemDropGroupTableID > 0 ) {
				CDnDropItem::CalcDropItemList( -1, m_nItemDropGroupTableID, m_VecDropItemList );

				for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
					if( CDnDropItem::PreInitializeItem( m_VecDropItemList[i].nItemID ) == false ) {
						m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
						i--;
					}
				}
			}
		}
	}

	return true;
}

void CDnWorldBrokenProp::LoadHitEffectData( int nTableID )
{
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	CTask *pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
	if( pTask ) return;
#endif

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPROP );

	// Particle
#ifdef PRE_FIX_MEMOPT_EXT
	std::string szFileName;
	CommonUtil::GetFileNameFromFileEXT(szFileName, pSox, nTableID, "_Particle");
	m_nHitParticle = EternityEngine::LoadParticleData( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );

	// Sound
	szFileName = "";
	CommonUtil::GetFileNameFromFileEXT(szFileName, pSox, nTableID, "_Sound");
#else
	CFileNameString szFileName = pSox->GetFieldFromLablePtr( nTableID, "_Particle" )->GetString();
	m_nHitParticle = EternityEngine::LoadParticleData( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	// Sound
	szFileName = pSox->GetFieldFromLablePtr( nTableID, "_Sound" )->GetString();
#endif
	if( !szFileName.empty() ) {
		CFileNameString szTemp;
		bool bFind;
		for( int j=0; ; j++ ) {
			szTemp = CEtResourceMng::GetInstance().GetFullNameRandom( szFileName, j, &bFind );
			if( bFind == false ) break;
			int nIndex = CEtSoundEngine::GetInstance().LoadSound( szTemp.c_str(), true, false );
			if( nIndex == -1 ) break;
			m_nVecHitSoundList.push_back( nIndex );
		}
	}

	m_fSoundRange = pSox->GetFieldFromLablePtr( nTableID, "_Sound_Range" )->GetFloat() * 100.f;
	m_fRollOff = pSox->GetFieldFromLablePtr( nTableID, "_Sound_Rolloff" )->GetFloat();
}

bool CDnWorldBrokenProp::Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale )
{
	if( -1 == m_nDurability )
		return true;

	// FSM 을 BrokenProp에 맞게 초기화 해준다... 클라에서는 게임서버와 다르게 Trigger State 가 빠지게 된다.
	CDnPropState* pNormalState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::NORMAL );
	CDnPropState* pHitActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	CDnPropState* pItemDropState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::ITEM_DROP );
	CDnPropState* pBrokenActionState = CDnPropState::Create( GetMySmartPtr(), CDnPropState::DO_ACTION );
	static_cast<CDnPropStateDoAction*>(pHitActionState)->AddActionName( "Hit" );
	m_pBrokenActionState = static_cast<CDnPropStateDoAction*>(pBrokenActionState);

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
	m_pFSM->AddState( pBrokenActionState );

	pNormalState->AddTransitState( pHitActionState, pHitCondition );
	pHitActionState->AddTransitState( pNormalState, pNULLCondition );
	pBrokenActionState->AddTransitState( pItemDropState, pNULLCondition );		// 아이템 드랍은 박살난 다음에 곧바로 전이 됨
	//pNormalState->AddTransitState( pItemDropState, pDurabilityCondition );
	//pItemDropState->AddTransitState( pBrokenActionState, pNULLCondition );

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


void CDnWorldBrokenProp::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnWorldActProp::Process( LocalTime, fDelta );

	// 소환된 프랍인 경우(포스미러) 서버로부터 CmdAction() 이나 OnDamage() 로 Break 액션을 실행하게 되어있으나 
	// 다시 hit 가 되어 break 액션을 실행못하는 경우가 있어서 보험 들어놓음.
	if( m_bLifeTimeEnd )
	{
		if( NULL == strstr( GetCurrentAction(), "Break" ) )
			if( IsExistAction( "Break_a" ) )
				SetActionQueue( "Break_a" );
	}
}


void CDnWorldBrokenProp::OnDamage( CDnDamageBase *pHitter, CDnActor::SHitParam &HitParam )
{

	if( m_bDestroy == true ) return;

	// 반응성 때문에 어쩔 수 없이.. 이 함수는 매 프레임 콜이 아니라 메시지 온 시점에 호출 되므로..
	if( m_nDurability <= 0 )
	{
		if( m_bBroken )
			return;

		//m_IntersectionType = IntersectionEnum::NoIntersectioin;
		CDnWorld::GetInstance().CheckAndRemoveVisibleProp( this );
		
		// MultiDurability 프랍인 경우 클라에서 fsm 을 안쓰므로 m_pBrokenActionState 가 NULL 임.
		// 앞으로 제작되는 프랍은 클라이언트는 서버에서 액션 동기만 받아서 처리한다.
		if( m_pBrokenActionState )
		{
			m_pFSM->ChangeState( m_pBrokenActionState, 0, 0.0f );
		}

		m_bBroken = true;

		//if( CDnActor::s_hLocalActor && pHitter && pHitter->GetActorHandle() == CDnActor::s_hLocalActor ) 
		//{
		//	CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		//	pPlayer->UpdatePropBreak();
		//}
		UpdatePropBreakToHitter( pHitter );

		//m_pFSM->Process( 0, 0.0f );

		//if( m_nBreakActionCount > 0 ) {
		//	char szTemp[16];
		//	sprintf_s( szTemp, "Break_%c", 'a' + _rand()%m_nBreakActionCount );
		//	SetActionQueue( szTemp );
		}

	//	DropItems();
	//}
	//else {
	//	SetActionQueue( "Hit" );
	//}

	// Hit Particle & Sound
	MatrixEx Cross;
	Cross = m_matExWorld;
	Cross.m_vPosition = HitParam.vPosition;
	Cross.m_vZAxis = -HitParam.vViewVec;
	float fLength = EtVec3Length( &EtVector3( m_matExWorld.m_vPosition - HitParam.vPosition ) ) * 2.f;
	Cross.MakeUpCartesianByZAxis();

	SSegment Segment;
	SCollisionResponse Response;

	Segment.vDirection = Cross.m_vZAxis * fLength;
	Segment.vOrigin = HitParam.vPosition;

	if( HitParam.hWeapon && ( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) && m_Handle &&
		CDnWorldProp::GetObjectHandle()->FindSegmentCollision( Segment, Response ) == true ) {
		Cross.MoveLocalZAxis( Response.fContactTime * fLength );
	}
	else {
		bool bCheck = false;
		if( m_Handle ) {
			SSphere Sphere;
			GetBoundingSphere( Sphere );

			float fLengthInner = Sphere.fRadius * 2.f;
			SSegment SegmentInner;
			SegmentInner.vOrigin = HitParam.vPosition + ( HitParam.vViewVec * Sphere.fRadius );
			SegmentInner.vOrigin.y += 50.f;
			SegmentInner.vDirection = -HitParam.vViewVec * fLengthInner;
			SCollisionResponse ResponseInner;
			if( m_Handle->FindSegmentCollision( SegmentInner, ResponseInner ) == true ) {
				Cross.m_vPosition = SegmentInner.vOrigin + ( SegmentInner.vDirection * ResponseInner.fContactTime );
				bCheck = true;
			}
		}

		if( !bCheck ) {
			SOBB Obb;
			GetBoundingBox( Obb );

			Cross.m_vPosition = m_matExWorld.m_vPosition;
			Cross.m_vPosition.y = HitParam.vPosition.y + 50.f;

			Cross.MoveLocalZAxis( -( ( Obb.Extent[1] + Obb.Extent[2] ) / 2.f ) );
		}
	}

	int nSoundIndex = -1;
	if( !m_nVecHitSoundList.empty() ) nSoundIndex = m_nVecHitSoundList[ _rand()%m_nVecHitSoundList.size() ];

	if( m_nHitParticle != -1 )
		EternityEngine::CreateBillboardEffect( m_nHitParticle, Cross );
	if( nSoundIndex != -1 ) {
		CEtSoundChannel *pChannel = CEtSoundEngine::GetInstance().PlaySound( "3D", nSoundIndex, false, true );
		if( pChannel ) {
			pChannel->SetVolume( 1.f );
			pChannel->SetPosition( Cross.m_vPosition ); 
			pChannel->SetRollOff( 3, 0.f, 1.f, m_fSoundRange * m_fRollOff, 1.f, m_fSoundRange, 0.f );

			pChannel->Resume();
		}
	}
	if( HitParam.hWeapon ) {
		if( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) {
			CDnProjectile *pProjectile = dynamic_cast<CDnProjectile *>(HitParam.hWeapon.GetPointer());
			if( pProjectile ) pProjectile->OnDamageSuccess( CDnActor::Identity(), HitParam );
			else HitParam.hWeapon->SetDestroy();
		}
	}

	m_bHitted = true;
}

void CDnWorldBrokenProp::OnDispatchMessage( DWORD dwProtocol, BYTE *pPacket )
{
	switch( dwProtocol ) {
		case eProp::SC_ONDAMAGE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSeed;
				DWORD dwWeaponUniqueID = -1, dwUniqueID;
				INT64 nWeaponSerialID = -1;
				bool bSendWeaponSerialID;

				CDnActor::SHitParam HitParam;
				Stream.Read( &nSeed, sizeof(int) );
				Stream.Read( &dwUniqueID, sizeof(DWORD) );

				Stream.Read( &m_nDurability, sizeof(int) );
				Stream.Read( &bSendWeaponSerialID, sizeof(bool) );
				if( bSendWeaponSerialID ) Stream.Read( &nWeaponSerialID, sizeof(INT64) );
				else Stream.Read( &dwWeaponUniqueID, sizeof(DWORD) );
				Stream.Read( &HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &HitParam.vViewVec, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				OnDamageReadAdditionalPacket( HitParam, &Stream );

				DnActorHandle hHitter = CDnActor::FindActorFromUniqueID( dwUniqueID );
				if( bSendWeaponSerialID ) {
					if( nWeaponSerialID != -1 ) {
						HitParam.hWeapon = CDnWeapon::FindWeaponFromSerialID( nWeaponSerialID );
					}
				}
				else {
					if( dwWeaponUniqueID != -1 ) {
						HitParam.hWeapon = CDnWeapon::FindWeaponFromUniqueID( dwWeaponUniqueID );
					}
				}

				_srand( nSeed );
				m_iLastRandomSeed = nSeed;
				OnDamage( hHitter, HitParam );
			}
			break;
	}

	CDnWorldActProp::OnDispatchMessage( dwProtocol, pPacket );
}



bool CDnWorldBrokenProp::IsHittedAndReset( void )
{
	bool bResult = m_bHitted;

	m_bHitted = false;

	return bResult;
}

void CDnWorldBrokenProp::_OnLifeTimeEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// break 액션에 반드시 destroy 시그널이 있다는 것을 가정한다.
	m_nDurability = 0;
	m_bBroken = true;
	m_bLifeTimeEnd = true;
	
	//m_pFSM->ChangeState( m_pBrokenActionState, LocalTime, fDelta );
}

void CDnWorldBrokenProp::UpdatePropBreakToHitter( CDnDamageBase* pHitter )
{
	//	CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
	//	pPlayer->UpdatePropBreak();

	if( CDnActor::s_hLocalActor && pHitter ) 
	{
		DnActorHandle hActor = pHitter->GetActorHandle();
		if( hActor )
		{
			if( hActor == CDnActor::s_hLocalActor )
			{
				CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				pPlayer->UpdatePropBreak();
			}
			else
			if( CDnActorState::Cannon == hActor->GetActorType() )
			{
				CDnCannonMonsterActor* pCannonMonsterActor = static_cast<CDnCannonMonsterActor*>(hActor.GetPointer());
				DnActorHandle hPlayerActor = pCannonMonsterActor->GetMasterPlayerActor();
				if( hPlayerActor && hPlayerActor == CDnActor::s_hLocalActor )
				{
					CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					pPlayer->UpdatePropBreak();
				}
			}
		}
	}
}

//void CDnWorldBrokenProp::DropItems()
//{
//	CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
//
//	EtVector3 vPos = m_matExWorld.m_vPosition;
////	vPos.y = CDnWorld::GetInstance().GetHeight( vPos );
//
//	// 돈 구한담에
//	int nTotalCoin = 0;
//	for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
//		if( m_VecDropItemList[i].nItemID == 0 ) {
//			CDnDropItem::DropCoin( vPos, m_VecDropItemList[i].nCount );
//			nTotalCoin += m_VecDropItemList[i].nCount;
//			m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
//			i--;
//		}
//	}
//
//	int nCoin = nTotalCoin / pTask->GetPartyCount();
//	CDnItemTask::GetInstance().AddCoin( nCoin );
//
//	// 아이템 뿌려준다.
//	for( DWORD i=0; i<m_VecDropItemList.size(); i++ ) {
//		int nRotate = (int)( ( ( ( GetUniqueID() * 10 ) + 360 ) / (float)m_VecDropItemList.size() ) * i );
//		CDnDropItem::DropItem( vPos, m_VecDropItemList[i].dwUniqueID, m_VecDropItemList[i].nItemID, m_VecDropItemList[i].nSeed, m_VecDropItemList[i].nCount, nRotate );
//	}
//	SAFE_DELETE_VEC( m_VecDropItemList );
//}