#include "StdAfx.h"
#include "DnActor.h"
#include "DnWorld.h"
#include "DnWorldGrid.h"
#include "MAActorRenderBase.h"
#include "MAMovementBase.h"
#include "DnWeapon.h"
#include "EtActionSignal.h"
#include "VelocityFunc.h"
#include "DnWorldBrokenProp.h"
#include "DnWorldSector.h"
#include "DnProjectile.h"
#include "DnMonsterActor.h"
#include "TaskManager.h"
#include "DnPlayerActor.h"
#include "PerfCheck.h"
#include "DnStateBlow.h"
#include "DnBlow.h"
#include "DnGameTask.h"
#include "DnItemTask.h"
#include "DNUserData.h"
#include "DNUserSession.h"
#include "DnWorldTrapProp.h"
#include "DnChangeActionStrProcessor.h"
//#include "DNUserConnection.h"
// Siva Shaodw Test 임시
#ifdef _SHADOW_TEST
#include "DNProtocol.h"
#include "GameSendPacket.h"
bool g_bEnableShadow[MAX_SESSION_COUNT] = { 0, };
#endif
#include "EtComputeDist.h"
#include "DNAggroSystem.h"
#include "DnParryBlow.h"
#include "DnCooltimeParryBlow.h"
#include "DnChargerBlow.h"
#include "DnBuffProp.h"
#include "DnWorldBrokenBuffProp.h"
#include "DnReverseTeamBlow.h"
#include "DnHighLanderBlow.h"
#include "DnCannonMonsterActor.h"
#include "DnPlayerSpeedHackChecker.h"
#include "DnFrostbiteBlow.h"

#include "DnChangeActionSetBlow.h"
#include "DnInvincibleAtBlow.h"
#include "DnBreakSuperArmorBlow.h"
#include "DnBurnCriticalBlow.h"
#include "DnDisableActionBlow.h"

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
#include "DnDPSReporter.h"
#endif

#include "DnManaShieldBlow.h"
#include "DnCurseBlow.h"
#include "DNGameDataManager.h"
#include "DnStateBlowSignalProcessor.h"
#include "DnInvincibleTypeBlow.h"
#include "DnImmuneByTypeBlow.h"
#include "DnComboDamageLimitBlow.h"
#include "DnTransmitDamageBlow.h"
#include "DnChangeStandActionBlow.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "DnTotalLevelSkillBlows.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#if defined(PRE_FIX_68898)
#include "DnPartialPlayProcessor.h"
#endif // PRE_FIX_68898

#include "SmartPtrDef.h"

#include "DnBubbleSystem.h"
#include "DnObserverEventMessage.h"


DECL_DN_SMART_PTR_STATIC( CDnActor, MAX_SESSION_COUNT, 100 )

STATIC_DECL_INIT( CDnActor, CEtOctree<DnActorHandle> *, s_pOctree ) = { NULL, };
STATIC_DECL_INIT( CDnActor, DWORD, s_dwUniqueCount ) = { 0, };
STATIC_DECL_INIT( CDnActor, CDnActor::mapActorSearch, s_dwMapActorSearch );

float CDnActor::s_fDieDelta = 20.f;

float CEtOctreeNode<DnActorHandle>::s_fMinRadius = 1000.0f;

#define DOWN_DELAY_RANDOM_RANGE 0.2f // 20프로 길거나 짥거나~

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
STATIC_DECL_INIT( CDnActor, CFrameSkip, s_MonsterProcess );
STATIC_DECL_INIT( CDnActor, float, s_fMonsterProcessDelta );
STATIC_DECL_INIT( CDnActor, CFrameSkip, s_NpcProcess );
STATIC_DECL_INIT( CDnActor, float, s_fNpcProcessDelta );
#endif

CDnActor::CDnActor( CMultiRoom *pRoom, int nClassID )
	: CDnUnknownRenderObject( pRoom, true )
	, m_vStaticPosition( 0.f, 0.f, 0.f )
	, m_vPrevPosition( 0.f, 0.f, 0.f )
{
	CDnActorState::Initialize( this );
	m_nClassID = nClassID;
	m_fAddHeight = 0.f;

	m_bShow = true;
	m_fDownDelta = 0.f;
	m_fDieDelta = 0.f;
	m_fMaxDieDelta = CDnActor::s_fDieDelta;
	m_fStiffDelta = 0.f;
	m_fLastDownRatio = 1.f;
	m_LastHitSignalTime = 0;
	m_nLastHitSignalIndex = -1;
	m_nLastDamageHitterActionIndex = 0;
	m_nTeam = 0;
	m_dwUniqueID = -1;

	m_pAggroSystem = NULL;

	m_bModifyPlaySpeed = false;
	m_PlaySpeedStartTime = 0;
	m_dwPlaySpeedTime = 0;

	m_bEnableNormalSuperAmmor = false;
	m_nNormalSuperAmmorTime = 0;
	m_fBreakNormalSuperAmmorDurability = 0.f;

	m_HitCheckType = HitCheckTypeEnum::BoundingBox;
	memset( m_bSelfDeleteWeapon, 0, sizeof( m_bSelfDeleteWeapon ) );

	m_pStateBlow = new CDnStateBlow( GetMySmartPtr() );

	memset( m_nSkillSuperAmmorValue, 0, sizeof(m_nSkillSuperAmmorValue) );
	memset( m_nLastUpdateSkillSuperAmmorValue, 0, sizeof(m_nLastUpdateSkillSuperAmmorValue) );
	m_nSkillSuperAmmorTime = 0;
	m_fSkillSuperAmmorDamageDecreaseProb = 0.f;

	m_bIngnoreNormalSuperArmor = false;
	InsertOctreeNode();

	m_bUseSignalSkillCheck = false;
	ZeroMemory( m_abSignalSkillCheck, sizeof(m_abSignalSkillCheck) );

	m_iCantMoveReferenceCount = 0;
	m_iCantActionReferenceCount = 0;
	m_iCantUseSkillReferenceCount = 0;
	m_iCantAttackReferenceCount = 0;

	m_iCantXZMoveSE = 0;

#ifdef _SHADOW_TEST
	m_bIsShadowActor = false;
	m_bEnableShadowActor = true;
#endif

	m_iLastDamage = 0;

	m_bOnSignalFromChargerSE = false;
	
	m_bAllowCalcCombo = true;
	m_bOctreeUpdate = true;

	m_bToggleIngnoreHit = false;

	m_dwGenTick = timeGetTime();

	m_iFrameStopRefCount = 0;
	m_fPlaySpeed = 0.0f;
	m_iLastProcessPressCount = 0;

	m_bCompleteKill_AfterProcessStateBlow = false;

	m_pStateBlowSignalProcessor = new CDnStateBlowSignalProcessor();

	m_bChangeWeaponLocked = false;
	m_bSkipChangeWeaponAction = false;
	m_bSkipOnAttatchDetachWeapon = false;

#if defined(PRE_ADD_50907)
	m_ChangeWeaponRefCount = 0;
	m_bOrigWeaponWhenChangeWeaponSelfDelete = false;
#endif // PRE_ADD_50907

#if defined(PRE_ADD_50907)
	m_DisarmamentRefCount = 0;
	m_bOrigWeaponWhenDisarmamentSelfDelete = false;
#endif // PRE_ADD_50907

#if defined(PRE_FIX_50482)
	m_nChangeTeamRefCount = 0;
#endif // PRE_FIX_50482

#if defined(PRE_FIX_59347)
	m_bApplyPartsDamage = false;
#endif // PRE_FIX_59347

#ifdef PRE_ADD_GRAVITY_PROPERTY
	m_fGravityEnd = 0.0f;
#endif // PRE_ADD_GRAVITY_PROPERTY

	// Rotate.
	m_dwRotateStartTime = 0;
	m_dwRotateTime = 0;	
	m_bRotate = false;;
	m_bRotLeft = false;	  // 회전방향.	
	m_fSpeedRot = 0.0f;    // 회전속도.
	m_fStartSpeed = 0.0f;  // 시작속도.
	m_fEndSpeed = 0.0f;    // 종료속도.
	m_vRotAxis.x = m_vRotAxis.y = m_vRotAxis.z = 0.0f; // 회전축좌표.
}

CDnActor::~CDnActor()
{
	RemoveUniqueSearchMap( GetRoom(), this );
	FreeAction();
	for( int i=0; i<2; i++ ) SAFE_RELEASE_SPTR( m_hWeapon[i] );
	RemoveOctreeNode();
	SAFE_DELETE_VEC( m_VecPreActionState );
	SAFE_DELETE_VEC( m_VecPreActionCustomState );
	SAFE_DELETE(m_pStateBlow);
	SAFE_DELETE( m_pAggroSystem );

	SAFE_DELETE(m_pStateBlowSignalProcessor);
}

void CDnActor::InsertOctreeNode()
{
	if( STATIC_INSTANCE(s_pOctree) ) {
		SSphere Sphere;
		GetBoundingSphere( Sphere );
		m_pCurrentNode = STATIC_INSTANCE(s_pOctree)->Insert( GetMySmartPtr(), Sphere );
	}
}

void CDnActor::RemoveOctreeNode()
{
	if( STATIC_INSTANCE(s_pOctree) ) {
		if( !STATIC_INSTANCE(s_pOctree)->Remove( GetMySmartPtr(), m_pCurrentNode ) ) {
			STATIC_INSTANCE(s_pOctree)->Remove( GetMySmartPtr(), NULL );
		}
		m_pCurrentNode = NULL;
	}
}

void CDnActor::SetUniqueID( DWORD dwUniqueID )
{
	RemoveUniqueSearchMap( GetRoom(), this );
	m_dwUniqueID = dwUniqueID;
	InsertUniqueSearchMap( GetRoom(), this );
}

void CDnActor::InsertUniqueSearchMap( CMultiRoom *pRoom, CDnActor *pActor )
{
	if( pActor->GetUniqueID() == -1 ) return;
	STATIC_INSTANCE_(s_dwMapActorSearch).insert( make_pair( pActor->GetUniqueID(), pActor->GetMySmartPtr() ) );
}

void CDnActor::RemoveUniqueSearchMap( CMultiRoom *pRoom, CDnActor *pActor )
{
	if( pActor->GetUniqueID() == -1 ) return;
	std::map<DWORD, DnActorHandle>::iterator it;
	it = STATIC_INSTANCE_(s_dwMapActorSearch).find( pActor->GetUniqueID() );
	if( it != STATIC_INSTANCE_(s_dwMapActorSearch).end() ) {
		STATIC_INSTANCE_(s_dwMapActorSearch).erase( it );
	}
}

// Static Func

void CDnActor::InitializeNextStage( CMultiRoom* pRoom  )
{
	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(pRoom);

	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) 
	{
		if( !STATIC_INSTANCE_(s_pVecProcessList)[i]->IsPlayerActor() ) 
		{
			//g_Log.Log( LogType::_INVALIDACTOR, "[%d] MapIdx:%d ClassID:%d LocalTime=%I64d\r\n", g_Config.nManagedID, pGameRoom->m_iMapIdx, STATIC_INSTANCE_(s_pVecProcessList)[i]->GetClassID(), pGameTask ? pGameTask->m_pFrameSync->GetMSTime() : 0 );
			STATIC_INSTANCE_(s_pVecProcessList)[i]->Release();
			i--;
		}
	}
}

bool CDnActor::InitializeClass( CMultiRoom *pRoom )
{
	if( !CDnWorld::IsActive(pRoom) ) return false;

	DNVector(DnActorHandle) VecList;
	GetActorList( pRoom, VecList );
	
	int iNumActor = (int)VecList.size();
	for( int i = 0; i < iNumActor; ++i )
	{
		DnActorHandle hActor = VecList.at( i );

		CDnActor::ActorTypeEnum ActorType = hActor->GetActorType();
		if( ActorType > -1 && ActorType <= ActorTypeEnum::Reserved6 )
			hActor->OnInitializeNextStage();
	}
	float fCenter, fSize;
	CDnWorld::GetInstance(pRoom).CalcWorldSize( fCenter, fSize );

	SAFE_DELETE( STATIC_INSTANCE_(s_pOctree) );
	STATIC_INSTANCE_(s_pOctree) = new CEtOctree<DnActorHandle>( false );
	STATIC_INSTANCE_(s_pOctree)->Initialize( EtVector3( 0.f, fCenter, 0.f ), fSize );
	STATIC_INSTANCE_(CDnActor::s_dwUniqueCount) = 0;

#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	STATIC_INSTANCE_(s_MonsterProcess).SetFramePerSec( 10.f );
	STATIC_INSTANCE_(s_fMonsterProcessDelta) = 0.f;
	STATIC_INSTANCE_(s_NpcProcess).SetFramePerSec( 1.f );
	STATIC_INSTANCE_(s_fNpcProcessDelta) = 0.f;
#endif
	return true;
}

void CDnActor::ProcessClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta )
{
#ifdef PRE_FIX_GAMESERVER_OPTIMIZE
	float fResultDelta[2] = { fDelta, fDelta };
	bool bProcessSkip[2] = { false, false };
	if( STATIC_INSTANCE_(s_MonsterProcess).Update( fDelta ) ) {
		STATIC_INSTANCE_(s_fMonsterProcessDelta) += fDelta;
		fResultDelta[0] = STATIC_INSTANCE_(s_fMonsterProcessDelta);
		bProcessSkip[0] = false;
		STATIC_INSTANCE_(s_fMonsterProcessDelta) = 0.f;
	}
	else {
		bProcessSkip[0] = true;
		STATIC_INSTANCE_(s_fMonsterProcessDelta) += fDelta;
	}

	if( STATIC_INSTANCE_(s_NpcProcess).Update( fDelta ) ) {
		STATIC_INSTANCE_(s_fNpcProcessDelta) += fDelta;
		fResultDelta[1] = STATIC_INSTANCE_(s_fNpcProcessDelta);
		bProcessSkip[1] = false;
		STATIC_INSTANCE_(s_fNpcProcessDelta) = 0.f;
	}
	else {
		bProcessSkip[1] = true;
		STATIC_INSTANCE_(s_fNpcProcessDelta) += fDelta;
	}


	float fTempDelta;
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		CDnActor *pActor = STATIC_INSTANCE_(s_pVecProcessList)[i];
		pActor->SyncClassTime( LocalTime );

		fTempDelta = fDelta;

		if( !pActor->IsPlayerActor() ) {
			if( pActor->IsMonsterActor() ) {

#ifndef PRE_FIX_MOMSTER_PROCESS_DELTA
				// 몬스터와 플레이어의 프레임율이 달라서 민감한 부분에서 클라와 서버의 싱크가 달라지게됩니다
				// 특정 Delta값을 이용한 보간 계산이나 Delta 값을 사용하는 부분에서 미묘하게 싱크가 틀어져서 몬스터는 프레임 싱크 걸지않도록 설정합니다.
				fTempDelta = fResultDelta[0];
				if( bProcessSkip[0] ) continue;
#endif
			}
			if( pActor->IsNpcActor() ) {
				fTempDelta = fResultDelta[1];
				if( bProcessSkip[1] ) continue;
			}
		}
		/*
		if( bProcessSkip == true && !STATIC_INSTANCE_(s_pVecProcessList)[i]->IsPlayerActor() ) {
			continue;
		}
		if( !STATIC_INSTANCE_(s_pVecProcessList)[i]->IsPlayerActor() ) {
			fTempDelta = fResultDelta;
		}
		else fTempDelta = fDelta;
		*/

		STATIC_INSTANCE_(s_pVecProcessList)[i]->Process( LocalTime, fTempDelta );

		SSphere Sphere;
		STATIC_INSTANCE_(s_pVecProcessList)[i]->GetBoundingSphere( Sphere );
		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->m_bOctreeUpdate )
			STATIC_INSTANCE_(s_pVecProcessList)[i]->m_pCurrentNode = STATIC_INSTANCE_(s_pOctree)->Update( STATIC_INSTANCE_(s_pVecProcessList)[i]->GetMySmartPtr(), Sphere, STATIC_INSTANCE_(s_pVecProcessList)[i]->m_pCurrentNode );

		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->IsDestroy() ) {
			STATIC_INSTANCE_(s_pVecProcessList)[i]->Release();
			i--;
		}
	}
#else
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		STATIC_INSTANCE_(s_pVecProcessList)[i]->SyncClassTime( LocalTime );
		STATIC_INSTANCE_(s_pVecProcessList)[i]->Process( LocalTime, fDelta );

		SSphere Sphere;
		STATIC_INSTANCE_(s_pVecProcessList)[i]->GetBoundingSphere( Sphere );
		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->m_bOctreeUpdate )
			STATIC_INSTANCE_(s_pVecProcessList)[i]->m_pCurrentNode = STATIC_INSTANCE_(s_pOctree)->Update( STATIC_INSTANCE_(s_pVecProcessList)[i]->GetMySmartPtr(), Sphere, STATIC_INSTANCE_(s_pVecProcessList)[i]->m_pCurrentNode );

		if( STATIC_INSTANCE_(s_pVecProcessList)[i]->IsDestroy() ) {
			STATIC_INSTANCE_(s_pVecProcessList)[i]->Release();
			i--;
		}
	}
#endif
}

void CDnActor::ProcessAIClass( CMultiRoom *pRoom, LOCAL_TIME LocalTime, float fDelta )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) 
	{
		STATIC_INSTANCE_(s_pVecProcessList)[i]->ProcessAI( LocalTime, fDelta );

	}
}

void CDnActor::ReleaseClass( CMultiRoom *pRoom )
{
	SAFE_DELETE( STATIC_INSTANCE_(s_pOctree) );
	SAFE_DELETE_MAP( STATIC_INSTANCE_(s_dwMapActorSearch) );
	DeleteAllObject( pRoom );

	STATIC_INSTANCE_(s_pVecProcessList).clear();
}

int CDnActor::ScanActor( CMultiRoom *pRoom, EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList, bool bInside )
{
	SSphere Sphere;
	Sphere.Center = vPos;
	Sphere.fRadius = fRadius;
	STATIC_INSTANCE_(s_pOctree)->Pick( Sphere, VecList, bInside );
	return (int)VecList.size();
}

int CDnActor::ScanActorByActorSize( CMultiRoom *pRoom, EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList, bool bInside, bool bActorSize )
{
	SSphere Sphere;
	Sphere.Center = vPos;
	Sphere.fRadius = fRadius;
	STATIC_INSTANCE_(s_pOctree)->Pick( Sphere, VecList, bInside, bActorSize );
	return (int)VecList.size();
}

void CDnActor::GetActorList(CMultiRoom *pRoom, DNVector(DnActorHandle) &VecList)
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) 
	{
		DnActorHandle hActor = STATIC_INSTANCE_(s_pVecProcessList)[i]->GetMySmartPtr();
		VecList.push_back(hActor);
	}
}

void CDnActor::GetOpponentActorList(CMultiRoom *pRoom, int nTeam, DNVector(DnActorHandle) &VecList, EtVector3* pPos/*=NULL*/, float fMaxRange/*=0.f*/ )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) 
	{
		DnActorHandle hActor = STATIC_INSTANCE_(s_pVecProcessList)[i]->GetMySmartPtr();
		if( hActor->GetTeam() != nTeam )
		{
			bool bPush = true;
			if( pPos && fMaxRange > 0.f )
			{
				if( EtVec3LengthSq( &(*pPos-*hActor->GetPosition()) ) > fMaxRange*fMaxRange )
					bPush = false;
			}

			if( bPush )
				VecList.push_back(hActor);
		}
	}
}

DnActorHandle CDnActor::FindActorFromUniqueID( CMultiRoom *pRoom, DWORD dwUniqueID )
{
	std::map<DWORD, DnActorHandle>::iterator it = STATIC_INSTANCE_(s_dwMapActorSearch).find( dwUniqueID );
	if( it != STATIC_INSTANCE_(s_dwMapActorSearch).end() ) return it->second;
	return CDnActor::Identity();
}

DnActorHandle CDnActor::FindActorFromName( CMultiRoom *pRoom, TCHAR *szName )
{
	for( DWORD i=0; i<STATIC_INSTANCE_(s_pVecProcessList).size(); i++ ) {
		if( !wcscmp( STATIC_INSTANCE_(s_pVecProcessList)[i]->GetName(), szName ) ) return STATIC_INSTANCE_(s_pVecProcessList)[i]->GetMySmartPtr();
	}
	return CDnActor::Identity();
}

void CDnActor::ProcessReservedRemoveBlows( void )
{
	if( false == m_vlReservedRemoveBlows.empty() )
	{
		for( int i = 0; i < (int)m_vlReservedRemoveBlows.size(); ++i )
		{
			DnBlowHandle hBlow = m_vlReservedRemoveBlows.at( i );
			if( hBlow )
				this->CmdRemoveStateEffectFromID( hBlow->GetBlowID() );
		}

		m_vlReservedRemoveBlows.clear();
	}
}

void CDnActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	UpdateStealMagicBuff();

	int nState = GetState();
	m_bEnableNormalSuperAmmor = false;
	CDnActionBase::ProcessAction( LocalTime, fDelta );

	if( 0 < GetCantMoveSEReferenceCount() )
		m_vAniDistance.x = m_vAniDistance.z = 0.0f;

	if( !IsNpcActor() )
	{
		PROFILE_TIME_TEST( OnSkillProcess( LocalTime, fDelta ) );
	}

	ProcessDown( LocalTime, fDelta );
	ProcessDie( LocalTime, fDelta );
	ProcessStiff( LocalTime, fDelta );
	ProcessPlaySpeed( LocalTime, fDelta );

	ProcessRotate( LocalTime, fDelta ); // Rotate;

	PROFILE_TIME_TEST( ProcessState( LocalTime, fDelta ) );
	PROFILE_TIME_TEST( ProcessPress() );

	if (m_pStateBlowSignalProcessor && !IsDie() )
		m_pStateBlowSignalProcessor->Process(LocalTime, fDelta);

	//상태효과 229번에서 등록된 상태효과 적용용 함수..
	ApplySkillStateEffect();

	for( int i=0; i<2; i++ )
	{
		if( m_hWeapon[i] ) 
			m_hWeapon[i]->Process( LocalTime, fDelta );
	}

#ifdef PRE_ADD_GRAVITY_PROPERTY
	if( m_fGravityEnd > 0.0f )
	{
		m_fGravityEnd -= fDelta;
		if( m_fGravityEnd <= 0.0f )
		{
			SetVelocityY( 0.01f );
			SetResistanceY( -30.0f );
			SetAppliedYDistance( false );
			m_fGravityEnd = 0.0f;
		}
	}
#endif // PRE_ADD_GRAVITY_PROPERTY

#ifdef _SHADOW_TEST
	if( g_bEnableShadow[GetRoom()->GetRoomID()] && m_bEnableShadowActor && GetGameRoom() ) {
		char pBuf[256];
		CPacketCompressStream Stream( pBuf, 256 );
		int nIndex = GetElementIndex( m_szAction.c_str() );
		float fFrame = GetCurFrame();
		EtVector3 vMoveX = *GetMoveVectorX();
		EtVector3 vMoveZ = *GetMoveVectorZ();
		Stream.Write( &nIndex, sizeof(int) );
		Stream.Write( &fFrame, sizeof(float) );
		Stream.Write( &m_Cross.m_vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
		Stream.Write( &m_Cross.m_vXAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Stream.Write( &m_Cross.m_vZAxis, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Stream.Write( &vMoveX, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
		Stream.Write( &vMoveZ, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

		bool bMoveTarget = ( GetMoveTarget() ) ? true : false;
		bool bLookTarget = ( GetLookTarget() ) ? true : false;
		Stream.Write( &bMoveTarget, sizeof(bool) );
		if( bMoveTarget ) {
			DnActorHandle hMoveTarget = GetMoveTarget();
			Stream.Write( hMoveTarget->GetPosition(), sizeof(EtVector3) );
		}

		Stream.Write( &bLookTarget, sizeof(bool) );
		if( bLookTarget ) {
			DnActorHandle hLookTarget = GetLookTarget();
			Stream.Write( hLookTarget->GetPosition(), sizeof(EtVector3) );
		}

		for( DWORD i=0; i<GetGameRoom()->GetUserCount(); i++ ) {
			SendActorShadowMsg( GetGameRoom()->GetUserData(i), GetUniqueID(), eActor::SC_SHADOW, (BYTE*)Stream.GetBuffer(), Stream.Tell() );
		}
	}
#endif
	// 마지막으로 받았던 데미지 초기화. (화염 상태효과에서 쓰임)
	m_iLastDamage = 0;
}

void CDnActor::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	MAActorRenderBase *pRenderBase = static_cast<MAActorRenderBase *>(this);
	if( pRenderBase ) {
		if( pRenderBase->GetBoundingSphere( Sphere ) == false ) {
			Sphere.Center = m_Cross.m_vPosition;
			Sphere.fRadius = (float)GetUnitSize();
		}
	}
	else {
		Sphere.Center = m_Cross.m_vPosition;
		Sphere.fRadius = (float)GetUnitSize();
	}

	if (bActorSize == true)
	{
		Sphere.Center = FindAutoTargetPos();
		Sphere.fRadius = (float)GetUnitSize();
	}
}

void CDnActor::GetBoundingBox( SAABox &Box )
{
	MAActorRenderBase *pRenderBase = static_cast<MAActorRenderBase *>(this);
	if( pRenderBase ) {
		pRenderBase->GetBoundingBox( Box );
	}
	else {
		Box.Min = m_Cross.m_vPosition + EtVector3( -50.f, -50.f, -50.f );
		Box.Max = m_Cross.m_vPosition + EtVector3( 50.f, 50.f, 50.f );
	}
}


void CDnActor::AddStateEffectQueue( const CDnSkill::SkillInfo& ParentSkillInfo, const CDnSkill::StateEffectStruct& StateEffectInfo )
{
	m_dqApplySelfStateBlowQ.push_back( S_NO_PACKET_SELF_STATEBLOW(ParentSkillInfo, StateEffectInfo) );
}

void CDnActor::ClearSelfStateSignalBlowQueue( bool bItemSkill/* = false*/ )
{
	deque<S_NO_PACKET_SELF_STATEBLOW>::iterator iter = m_dqApplySelfStateBlowQ.begin();
	for( iter; iter != m_dqApplySelfStateBlowQ.end(); )
	{
		if( bItemSkill == iter->ParentSkillInfo.bIsItemSkill )
		{
			iter = m_dqApplySelfStateBlowQ.erase( iter );
		}
		else
		{
			iter++;
		}
	}
}


void CDnActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
	switch( Type ) {
		case STE_Hit:
			{
				if( IsDie() ) break;

				if (ProcessIgnoreHitSignal() == true)
					break;

				HitStruct *pStruct = (HitStruct *)pPtr;
				
				DNVector(DnActorHandle) hVecList;
				DNVector(DnActorHandle) hVecActorToApplyStateEffect;

				int nWeaponLength = 0;
				bool bOnDamageCalled = false;
				if( pStruct->bIncludeWeaponLength && GetWeapon() ) nWeaponLength = GetWeapon()->GetWeaponLength();

				MatrixEx CrossTemp = m_Cross;

				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;
				// 스케일된 몬스터의 경우 히트 시그널의 중심축이 항상 액터의 원점 위치가 맞는 것은 아니기 때문에 
				// 오프셋에도 정확하게 스케일 값을 적용시켜 줘야 한다.(#18971)
				EtVector3 vOffset = *pStruct->vOffset * GetScale()->y;
				CrossTemp.MoveLocalZAxis( vOffset.z );
				CrossTemp.MoveLocalXAxis( vOffset.x );
				CrossTemp.MoveLocalYAxis( vOffset.y );
				
				// 특정 본과 링크된 상태라면..
				if( pStruct->szLinkBoneName && strlen(pStruct->szLinkBoneName) > 0 )
				{
					int nBoneIndex = GetBoneIndex( pStruct->szLinkBoneName );
					if( -1 != nBoneIndex )
					{
						EtMatrix matBoneWorld = GetBoneMatrix( pStruct->szLinkBoneName );
						EtMatrixMultiply( (EtMatrix*)&CrossTemp, (EtMatrix*)&CrossTemp, &matBoneWorld );
					}
				}
				// 몬스터는 스케일에따라 보정해줘야한다.
				float fDistanceMax = pStruct->fDistanceMax * GetScale()->y;
				float fDistanceMin = pStruct->fDistanceMin * GetScale()->y;
				float fHeightMax = pStruct->fHeightMax * GetScale()->y;

				EtVector3 vPos = CrossTemp.m_vPosition;

				float fDistance = max( fDistanceMax, fHeightMax - pStruct->fHeightMin ) + nWeaponLength;
				float fXZDistanceSQ = fDistanceMax + nWeaponLength;
				float fXZDistanceMinSQ = fDistanceMin; // Min 은 Weapon 길이 영향안받습니다.

				fXZDistanceSQ *= fXZDistanceSQ;
				fXZDistanceMinSQ *= fXZDistanceMinSQ;

				ScanActor( GetRoom(), vPos, fDistance, hVecList );

				//#53454 꼭두각시 소환액터인 경우, 꼭두각시 주인?은 HitList에서 제외 시킨다.
#if defined(PRE_FIX_61382)
				ExceptionHitList2(hVecList, m_Cross, GetMySmartPtr(), pStruct, hVecActorToApplyStateEffect, 0, fDistance, m_vPrevPosition);
#else
				ExceptionHitList(hVecList, m_Cross, GetMySmartPtr(), pStruct);
#endif // PRE_FIX_61382

				EtVector3 vDir;
				EtVector3 vZVec = m_Cross.m_vZAxis;

				if( pStruct->fCenterAngle != 0.f ) {
					EtMatrix matRotate;
					EtMatrixRotationY( &matRotate, EtToRadian( pStruct->fCenterAngle ) );
					EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );

#if defined(PRE_FIX_63356)
					//#63356 서클봄버의 경우 히트 영역이 180도 회전이 된 경우..
					//vPos는 현재 캐릭터의 방향에서 옵셋 값만큼 이동이 되고, 히트 영역은 180도 회전이 되는 경우 이므로
					//허용 각도 계산에서 이 회전 값을 적용 시켜야 한다.
					CrossTemp.m_vZAxis = vZVec;
#endif // PRE_FIX_63356
				}
				SAABox Box;
				float fDot = 0.0f;
				SHitParam HitParam;

				bool bHit = false;
				bool bFirstHit = true;
				if( m_LastHitSignalTime > LocalTime ) bFirstHit = false;
				if( m_nLastHitSignalIndex != nSignalIndex ) {
					bFirstHit = true;
					m_nLastHitSignalIndex = nSignalIndex;
				}

				HitParam.szActionName = pStruct->szTargetHitAction;
				HitParam.fDamage = pStruct->fDamageProb * 0.01f;
				HitParam.fDurability = pStruct->fDurabilityDamageProb * 0.01f;
				HitParam.vVelocity = *pStruct->vVelocity;
				HitParam.vResistance = *pStruct->vResistance;
				HitParam.hHitter = GetMySmartPtr();
				HitParam.hWeapon = GetActiveWeapon(0);
				HitParam.vPosition = vPos;
				HitParam.fStiffProb = pStruct->fStiffProb * 0.01f;
				HitParam.RemainTime = SignalEndTime;
				HitParam.nDamageType = pStruct->nDamageType;
				HitParam.nSkillSuperAmmorIndex = pStruct->nApplySuperAmmorIndex - 1;
				HitParam.nSkillSuperAmmorDamage = pStruct->nApplySuperAmmorDamage;
				HitParam.cAttackType	= (char)pStruct->nAttackType;
				HitParam.DistanceType	= (pStruct->nDistanceType == 0) ? CDnDamageBase::DistanceTypeEnum::Melee : CDnDamageBase::DistanceTypeEnum::Range;
				HitParam.HasElement = CalcHitElementType( ( pStruct->bUseSkillApplyWeaponElement == TRUE ) ? true : false );
				HitParam.bIgnoreCanHit = ( pStruct->bIgnoreCanHit == TRUE );
				HitParam.bIgnoreParring = ( pStruct->bIgnoreParring == TRUE );
				HitParam.bReleaseCatchActor = ( pStruct->bReleaseCatchedActor == TRUE );
				HitParam.nWeightValueLimit = pStruct->nWeightValueLimit;

			
				bool isFirstHitter = true;

 				HitParam.nHitLimitCount = pStruct->nHitLimitCount;
				
				bool isHitLimited = false;
				HitLimitCountInfo* pHitLimitCountInfo = NULL;

				if (HitParam.nHitLimitCount != 0)
				{
					//HitLimitCount정보가 등록되어 있는게 있는지 확인..
					HIT_LIMIT_COUNT_INFO_LIST::iterator findIter = m_HitLimitCountInfoList.find(nSignalIndex);
					if (findIter != m_HitLimitCountInfoList.end())
						pHitLimitCountInfo = &findIter->second;
					else
					{
						//새로운 정보 추가 하고, 해당 리스트의 정보의 포인터 얻는다..
						HitLimitCountInfo newHitLimitCountInfo(HitParam.nHitLimitCount, HitParam.nHitLimitCount);
						m_HitLimitCountInfoList.insert(HIT_LIMIT_COUNT_INFO_LIST::value_type(nSignalIndex, newHitLimitCountInfo));
						
						HIT_LIMIT_COUNT_INFO_LIST::iterator findIter = m_HitLimitCountInfoList.find(nSignalIndex);
						if (findIter != m_HitLimitCountInfoList.end())
							pHitLimitCountInfo = &findIter->second;
					}
				}

				// Note: 여기서 Clear 하면 이전에 때렸던 애들의 히트시그널 종료 시간이 복구되지 않은 상태로 넘어가버릴 수 있습니다. 
				// 슈퍼아머 있는 애가 맞았을 때 FPS가 1.8이 되어 EndTime 이 뒤로 밀린 채로 ProcessPlaySpeed 에서 제대로 복구 시켜주려면 
				// 마지막으로 맞은 애들을 리스트로 복구시키기 전에 날리면 안됩니다.
				m_hVecLastHitList.clear();
				// Actor 체크
				for( DWORD i=0; i<hVecList.size(); i++ ) 
				{
					if( !hVecList[i] )
						continue;

					// Hit수 제한
					//최대 Hit수가 설정되어 있고, Hit수가 최대 Hit수를 넘어 가면 멈춘다.
					isHitLimited = (pHitLimitCountInfo && (pHitLimitCountInfo->nHitLimitCount != 0 && pHitLimitCountInfo->nHitCount <= 0));

					if (isHitLimited)
						break;

					// 차져 상태효과라면 해당 상태효과의 ID 를 유니크 아이디를 사용한다.
					// 쇼크 오브 렐릭처럼 같은 액션으로 차져 상태효과의 인스턴스 갯수대로 동시에 여러 히트 시그널을 발동시키기 위해서.
					if( m_hChargerBlowCalledOnSignal )
					{
						if( !hVecList[i]->IsHittable( GetMySmartPtr(), LocalTime, pStruct, m_hChargerBlowCalledOnSignal->GetBlowID() ) ) 
							continue;

						// 여기서 hitparam 의 유니크 id 를 셋팅해줘야 추후에 ondamage 에서 이 id 별로 정리되어 추후 ishittable 호출시 사용하게 된다.
						HitParam.iUniqueID = m_hChargerBlowCalledOnSignal->GetBlowID();
					}
					else
					{
						if( !hVecList[i]->IsHittable( GetMySmartPtr(), LocalTime, pStruct ) ) 
							continue;
					}

					switch( hVecList[i]->GetHitCheckType() ) {
						case HitCheckTypeEnum::BoundingBox: 
							{
								HitParam.vPosition = vPos;
								vDir = *hVecList[i]->GetPosition() - vPos;
								vDir.y = 0.f;
								
								hVecList[i]->GetBoundingBox( Box );

								if( SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
								if( SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

								EtVec3Normalize( &vDir, &vDir );
								fDot = EtVec3Dot( &vZVec, &vDir );
								if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) continue;

								if( Box.Min.y < vPos.y + pStruct->fHeightMin && 
									Box.Max.y < vPos.y + pStruct->fHeightMin ) continue;
								if( Box.Min.y > vPos.y + fHeightMax && 
									Box.Max.y > vPos.y + fHeightMax ) continue;

								HitParam.vViewVec = -vDir;
							}
							break;
						case HitCheckTypeEnum::Collision:
							{
								SCollisionCapsule				Capsule;
								SCollisionResponse				CollisionResult;
								DNVector(SCollisionResponse)	vCollisionResult;

								Capsule.Segment.vOrigin = vPos;
								float fHeight = fHeightMax - pStruct->fHeightMin;
								Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( pStruct->fHeightMin + ( fHeight / 2.f ) );
								Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
								Capsule.fRadius = ( fDistanceMax + nWeaponLength );

								EtVector3 vDestPos;
								if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) continue;
								if( CollisionResult.pCollisionPrimitive )
								{
									for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
									{
										if( vCollisionResult[k].pCollisionPrimitive ) {
											vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
											if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < vPos.y + pStruct->fHeightMin ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}
											if( Box.Min.y > vPos.y + fHeightMax && Box.Max.y > vPos.y + fHeightMax ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}

											DNVector(EtVector3) vPointList;
											vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
											if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
												vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {
#else
											if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
												Box.GetVertices( vPointList );
											}
											bool bCheck = false;
											for( DWORD m=0; m<vPointList.size(); m++ ) {
#if defined(PRE_FIX_63356)
												//#63356 서클봄버의 경우 히트 영역이 180도 회전이 된 경우..
												//vPos는 현재 캐릭터의 방향에서 옵셋 값만큼 이동이 되고, 히트 영역은 180도 회전이 되는 경우 이므로
												//허용 각도 계산에서 이 회전 값을 적용 시켜야 한다.
												if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == true ) {
#else
												if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], pStruct->fAngle) == true ) {
#endif // PRE_FIX_63356
													bCheck = true;
													break;
												}
											}
											if( bCheck == false ) {
												vCollisionResult.erase( vCollisionResult.begin() + k );
												k--;
												continue;
											}
											HitParam.vBoneIndex.push_back( hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive ) );
										}
									}
									if( vCollisionResult.empty() ) continue;

									GetCenterPos( *CollisionResult.pCollisionPrimitive, vDestPos );
									HitParam.vPosition = vDestPos;
								}
								else
								{
									ASSERT( 0 );
								}

								if( pStruct->fDistanceMin > 100.f ) 
								{
									vCollisionResult.clear();
									Capsule.fRadius = pStruct->fDistanceMin;
									if( hVecList[i]->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == true ) {
										if( CollisionResult.pCollisionPrimitive )
										{
											for( UINT k=0 ; k<vCollisionResult.size() ; ++k )
											{
												if( vCollisionResult[k].pCollisionPrimitive ) {
													vCollisionResult[k].pCollisionPrimitive->GetBoundingBox( Box );
													if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < vPos.y + pStruct->fHeightMin ) {
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}
													if( Box.Min.y > vPos.y + fHeightMax && Box.Max.y > vPos.y + fHeightMax ) {
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}

													DNVector(EtVector3) vPointList;
													vPointList.push_back( Box.GetCenter() );
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ||
														vCollisionResult[k].pCollisionPrimitive->Type == CT_CAPSULE ) {

#else
													if( vCollisionResult[k].pCollisionPrimitive->Type == CT_BOX ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														Box.GetVertices( vPointList );

													}
													bool bCheck = false;
													for( DWORD m=0; m<vPointList.size(); m++ ) {
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														// 빼야하는 영역에서 한점이라도 안 걸리면 hit 영역과 걸쳐 있는 놈이므로 빼지 않는다...
#if defined(PRE_FIX_63356)
														if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == false ) {
#else
														if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], pStruct->fAngle) == false ) {
#endif // PRE_FIX_63356
#else
#if defined(PRE_FIX_63356)
														if( hVecList[i]->CheckCollisionHitCondition(vPos, CrossTemp, vPointList[m], pStruct->fAngle) == true ) {
#else
														if( hVecList[i]->CheckCollisionHitCondition(vPos, m_Cross, vPointList[m], pStruct->fAngle) == true ) {
#endif // PRE_FIX_63356
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
															bCheck = true;
															break;
														}
													}
													
#ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
													if( bCheck == true ) {
#else
													if( bCheck == false ) {
#endif // #ifdef PRE_FIX_COLMESH_RECTANGLE_HITSIGNAL
														vCollisionResult.erase( vCollisionResult.begin() + k );
														k--;
														continue;
													}
													int nBoneIndex = hVecList[i]->GetObjectHandle()->GetParentBoneIndex( vCollisionResult[k].pCollisionPrimitive );
													std::vector<int>::iterator it = std::find( HitParam.vBoneIndex.begin(), HitParam.vBoneIndex.end(), nBoneIndex );
													if( it != HitParam.vBoneIndex.end() ) {
														HitParam.vBoneIndex.erase( it );
													}
												}
											}
										}
									}
									if( HitParam.vBoneIndex.empty() ) continue;
								}

								HitParam.vViewVec = vPos - vDestPos;
								EtVec3Normalize( &HitParam.vViewVec, &HitParam.vViewVec );
							}
							break;
					}

					HitParam.bFirstHit = bFirstHit;
					bFirstHit = false;
					bHit = true;

					if( !HitParam.szActionName.empty() ) {
						m_hVecLastHitList.push_back( hVecList[i] );

						// 프레임이 빨라졌을 경우에 중복으로 액터가 들어올 수 있다.
						if( m_hVecLastHitListByRemainTime.end() == find( m_hVecLastHitListByRemainTime.begin(), m_hVecLastHitListByRemainTime.end(), hVecList[i] ) )
						{
							m_hVecLastHitListByRemainTime.push_back( hVecList[i] );
						}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
						// HitSignal에서 스킬 여부 상관없이 접두사 스킬 발동
						// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨
						// #40186 접미사? 발동 조건 변경 (데미지 비율이 0인 경우 발동 되지 않도록함.)
						if (HitParam.bFirstHit && 
							pStruct->fDamageProb != 0.0f &&
							!IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) && 
							isFirstHitter)
						{
							ProcessPrefixOffenceSkill_New();
							isFirstHitter = false;
						}
#else
						//////////////////////////////////////////////////////////////////////////
						// 접두어 공격용 스킬 발동 준비 작업..
						// 평타일때만 접두어 시스템 공격스킬 발동
						// 평타이고, Hit프레임에 처음 들어 왔고, 첫번째 맞는 녀석일때
						
						bool isCanUsePrefixSkill = CanUsePrefixSkill();

						if (isCanUsePrefixSkill && HitParam.bFirstHit && isFirstHitter)
						{
							OutputDebug("CDnActor::OnSignal -> STE_Hit start %d current %d end %d\n", (int)SignalStartTime, (int)LocalTime, (int)SignalEndTime);

							//여기서는 자신에게 적용 하는 상태 효과만 적용시키고,
							//맞는 녀석에게 적용해야할 상태 효과는 Target->OnDamage에서 처리 되도록한다.??
							ProcessPrefixOffenceSkill( 1.0f );

							isFirstHitter = false;
						}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

#ifdef PRE_ADD_MONSTER_CATCH
						if( HitParam.bReleaseCatchActor )
						{
							if( IsMonsterActor() )
							{
								static_cast<CDnMonsterActor*>(this)->ReleaseThisActor( hVecList[ i ] );
							}
						}
#endif // #ifdef PRE_ADD_MONSTER_CATCH

						hVecList[i]->OnDamage( GetMySmartPtr(), HitParam, pStruct );

						if( 0 != pStruct->StateEffectFilter )
						{
							OnHitSignalStateEffectFilterException( hVecList[ i ]->GetUniqueID(), pStruct->StateEffectFilter );
						}

						bOnDamageCalled = true;

						//Hit수 증가
						if (pHitLimitCountInfo)
							pHitLimitCountInfo->nHitCount--;
					}

					hVecActorToApplyStateEffect.push_back( hVecList[i] );

#ifdef PRE_ADD_TRIGGER_BY_HIT_CONDITION
					if( pStruct->szTriggerEventParameter != NULL && IsPlayerActor() == false && hVecList[i]->IsMonsterActor() )
					{
						CDnMonsterActor* pTrigerTargetMonsterActor = static_cast<CDnMonsterActor*>( hVecList[i].GetPointer());
						if( pTrigerTargetMonsterActor )
						{
							std::string strParam = pStruct->szTriggerEventParameter;
							std::vector<std::string> tokens;
							TokenizeA(strParam, tokens, ";");
							if (tokens.size() == 2)
							{
								if( pTrigerTargetMonsterActor->GetMonsterClassID() == atoi(tokens[0].c_str()) )
								{
									CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "EventArea", pTrigerTargetMonsterActor->GetBirthAreaHandle() );
									CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "ActorHandle", pTrigerTargetMonsterActor->GetUniqueID() );
									CDnWorld::GetInstance(GetRoom()).InsertTriggerEventStore( "EventID", atoi(tokens[1].c_str()) );
									CDnWorld::GetInstance(GetRoom()).OnTriggerEventCallback( "CDnActor::TriggerEvent", CDnActionBase::m_LocalTime, 0.f );
								}
							}
						}
					}
#endif

				}

				bool bIsFarm = false;
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
				if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
					bIsFarm = true;

				if( false == bIsFarm && IsProcessSkill() ) 
				{
					int iNumActorApplyStateEffect = (int)hVecActorToApplyStateEffect.size();
					m_hProcessSkill->CheckTargetCount( iNumActorApplyStateEffect );
					for( int i = 0; i < iNumActorApplyStateEffect; ++i )
					{
						// 대상이 얼음감옥 상태일때는 상태효과 적용 무시
						if (hVecActorToApplyStateEffect[ i ]->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							continue;

						// 스킬 대상 설정이 아군인가 타겟인가에 따라 상태효과 적용을 구분한다.
						switch( m_hProcessSkill->GetTargetType() )
						{
							case CDnSkill::Enemy:
							case CDnSkill::Self:
								if( GetTeam() == hVecActorToApplyStateEffect[ i ]->GetTeam() )
									continue;
								break;

							case CDnSkill::Friend:
							case CDnSkill::Party:
								if( GetTeam() != hVecActorToApplyStateEffect[ i ]->GetTeam() )
									continue;
								break;

							// 스킬 적용 대상이 아군/적군 전부 다 라면 상태효과 적용 쪽에서 적용 여부를 구분해야 한다.
							case CDnSkill::All:
								break;
						}

						// 상태이상 Add 시에 지속 구분 인덱스 처리 필요.
						// 해당 Actor의 지속효과 구분 인덱스를 구분하여 성공한 경우에 상태이상 추가 시킴.
						map<int, bool> mapDuplicateResult;
						CDnSkill::CanApply eResult = CDnSkill::CanApplySkillStateEffect( hVecActorToApplyStateEffect[i], m_hProcessSkill, mapDuplicateResult, true );
						if( CDnSkill::CanApply::Fail != eResult )
						{
							for( DWORD k = 0; k < m_hProcessSkill->GetStateEffectCount(); k++ ) 
							{
								CDnSkill::StateEffectStruct *pLocalStruct = m_hProcessSkill->GetStateEffectFromIndex(k);

								if (pStruct->szSkipStateBlows && CDnSkill::IsSkipStateBlow(pStruct->szSkipStateBlows, (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID))
									continue;

								// 아군까지 힐 시켜주는 스킬의 경우엔 Self 힐과 Target 힐 두 개의 상태효과가 선언되어있따.
								// Self 상태효과는 자신에게 이미 적용되었고 여긴 Hit 시그널 판정되는 곳이기 떄문에 Target 만 적용된다.
								switch( pLocalStruct->ApplyType )
								{
									case CDnSkill::ApplySelf:
										continue;
										break;

									case CDnSkill::ApplyTarget:
										break;

									case CDnSkill::ApplyEnemy:
										if( GetTeam() == hVecActorToApplyStateEffect[ i ]->GetTeam() )
											continue;
										break;

									case CDnSkill::ApplyFriend:
										if( GetTeam() != hVecActorToApplyStateEffect[ i ]->GetTeam() )
											continue;
										break;
								}

								// 같은 스킬 중첩일 경우엔 스킬 효과 중에 확률 체크하는 것들은 이미 CanApplySkillStateEffect 에서 확률체크되고
								// 통과된 상태이다. 따라서 여기선 확률 체크 된건지 확인하고 된거라면 다시 확률 체크 안하도록 함수 호출 해준다.
								bool bAllowAddThisSE = true;
								bool bCheckCanBegin = true;
								if( CDnSkill::ApplyDuplicateSameSkill == eResult )
								{
									map<int, bool>::iterator iter = mapDuplicateResult.find( pLocalStruct->nID );
									// 맵에 없는 경우 현재 액터가 상태효과에 걸려있지 않으므로 그냥 정상적으로 상태효과 추가 루틴 실행.
									if( mapDuplicateResult.end() != iter )
									{
										// 같은 스킬의 확률있는 상태효과가 현재 걸려있어서 CanAdd 를 호출해보았으나 실패했음.
										// 이런 경우엔 상태효과 추가하지 않는다.
										if( false == (iter->second) )
											bAllowAddThisSE = false;
										else
											// 이미 CanAdd 를 통과한 상태이므로 CmdAddStateEffect 호출 시 따로 체크하지 않도록 해준다.
											bCheckCanBegin = false;
									}
								}

								if( bAllowAddThisSE )
								{
									// #72931 스크리머 저주 쿨타임 공유 처리 -> 나중에 이런식으로 쓰는거 많아지면 일반화해야함
									bool bShareCurseCoolTime = false;
									if( pLocalStruct->nID == STATE_BLOW::BLOW_244 )
									{
										DNVector(DnBlowHandle) vlhBlows;
										GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_244, vlhBlows );
										if( static_cast<int>( vlhBlows.size() ) > 0 )
										{
											CDnCurseBlow* pDnCurseBlow = static_cast<CDnCurseBlow*>( vlhBlows[0].GetPointer() );
											if( pDnCurseBlow )
											{
												pLocalStruct->szValue += ";";
												pLocalStruct->szValue += FormatA( "%f", pDnCurseBlow->GetCoolTime() );
												bShareCurseCoolTime = true;
											}
										}
									}

									// 여기서 등록되어 있는 제거 되어야할 상태효과들 없앤다. [2010/12/08 semozz]
									hVecActorToApplyStateEffect[i]->RemoveResetStateBlow();

									hVecActorToApplyStateEffect[i]->CmdAddStateEffect( m_hProcessSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
																					   pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, bCheckCanBegin );

									if( bShareCurseCoolTime )	// #72931 쿨타임 붙였던거 다시 제거
									{
										std::string::size_type delimiterindex = pLocalStruct->szValue.rfind( ";" );
										if( delimiterindex != std::string::npos )
										{
											pLocalStruct->szValue.erase( delimiterindex, pLocalStruct->szValue.length() - delimiterindex );
										}
									}
									
									// 원래는 OnDamage() 호출되면 안쪽에서 DamageRemainTime 을 셋팅해준다. 하지만
									// 힐처럼 데미지 없이 상태효과만 추가시키기 위한 hit 시그널도 있으므로 OnDamage() 호출되지 않았더라도
									// 여기서 LastDamageTime 이 셋팅 안되어있는 녀석은 셋팅해주도록 한다.
									// 이렇게 해야 hit 시그널에 한방만 맞는다.
									if( !bOnDamageCalled )
									{
										hVecActorToApplyStateEffect[ i ]->SetDamageRemainTime( GetUniqueID(), HitParam.RemainTime );
										hVecActorToApplyStateEffect[ i ]->SetLastDamageHitUniqueID( GetUniqueID(), HitParam.iUniqueID );
									}
								}

							}
						}
						// 스킬 사용 끝났으면 리셋리스트 초기화 [2010/12/09 semozz]
						hVecActorToApplyStateEffect[i]->InitStateBlowIDToRemove();
					}
				}

				if( bHit ) {
					m_LastHitSignalTime = SignalEndTime;
					OnHitFinish( LocalTime, pStruct );
				}
				else m_LastHitSignalTime = -1;

				// 프랍 체크
				// Note: 프랍체크는 상태효과만 있는 Heal 같은 것은 체크하지 않도록 합니다. HitAction 이 없는 것으로 구분.
				if( false == HitParam.szActionName.empty() 
					//필터가 적용되어 있지 않을때만 프랍에 데미지를 줄 수 있다.
					&& pStruct->StateEffectFilter == 0 
					)
				{
					DNVector(CEtWorldProp*) pVecProp;
					CDnWorld* pWorld = CDnWorld::GetInstancePtr(GetRoom());

					pWorld->ScanProp( vPos, fDistance, pVecProp );

					for( DWORD i=0; i<pVecProp.size(); i++ ) 
					{
						if( false == ((CDnWorldProp*)pVecProp[i])->IsBrokenType() )
							continue;

						if( !((CDnWorldBrokenProp*)pVecProp[i])->IsHittable( GetMySmartPtr(), LocalTime ) ) 
							continue;

						pVecProp[i]->GetBoundingBox( Box );

						if( SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
						if( SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

						vDir = ((CDnWorldProp*)pVecProp[i])->GetMatEx()->m_vPosition - vPos;

						EtVec3Normalize( &vDir, &vDir );
						fDot = EtVec3Dot( &vZVec, &vDir );
						if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) 
							continue;

						if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < m_Cross.m_vPosition.y + pStruct->fHeightMin ) 
							continue;
						if( Box.Min.y > vPos.y + fHeightMax && Box.Max.y > m_Cross.m_vPosition.y + fHeightMax ) 
							continue;

						HitParam.vViewVec = -vDir;
						((CDnWorldProp*)pVecProp[i])->OnDamage( GetMySmartPtr(), HitParam );
					}
				}

				//////////////////////////////////////////////////////////////////////////
				// 접두어 공격용 스킬 끝..
				InitPrefixOffenceSkills();
				//////////////////////////////////////////////////////////////////////////

//				PROFILE_TICK_TEST_BLOCK_END();
			}
			break;
		case STE_VelocityAccel:
			{
				VelocityAccelStruct *pStruct = (VelocityAccelStruct *)pPtr;

				MAMovementBase *pMovement = GetMovement();
				
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
					pMovement->SetVelocity( vVelocity );
					pMovement->SetResistance( vResistance );
				}
			}
			break;
		case STE_State:
			{
				if( IsCustomProcessSignal() ) break;
				StateStruct *pStruct = (StateStruct *)pPtr;

				int nState = CDnActorState::s_nActorStateIndex[pStruct->nStateOne] | 
							CDnActorState::s_nActorStateIndex[pStruct->nStateTwo] | 
							CDnActorState::s_nActorStateIndex[pStruct->nStateThree];

				SetState( (ActorStateEnum)nState );
			}
			break;
		case STE_CustomState:
			{
				if( IsCustomProcessSignal() ) break;
				CustomStateStruct *pStruct = (CustomStateStruct *)pPtr;

				int nState = CDnActorState::s_nActorCustomStateIndex[pStruct->nStateOne] | 
					CDnActorState::s_nActorCustomStateIndex[pStruct->nStateTwo] | 
					CDnActorState::s_nActorCustomStateIndex[pStruct->nStateThree];
				SetCustomState( (ActorCustomStateEnum)nState );

			}
			break;
		case STE_CanHit:
			{
				// 결빙 상태가 되었을때 현재 진행중인 액션에서 프레임이 스탑되는 경우가있는데 
				// 특정 액션의 STE_CanHit 가 False 인 상태일때는 결빙상태에서도 Hit 가 들어가지 않던 문제가 있어서 결빙 상태에서는 무조건
				// 맞도록 해준다.
				if(GetStateBlow()->IsApplied( STATE_BLOW::BLOW_041)) 
				{
					SetHittable(true);
					break;
				}

				CanHitStruct *pStruct = (CanHitStruct *)pPtr;
				SetHittable( ( pStruct->bHittable == TRUE ) ? true : false );
			}
			break;
		case STE_CanMove:
			{
				if( CDnActorState::Cant_Move != (m_StateEffect & CDnActorState::Cant_Move) )
				{
					CanMoveStruct *pStruct = (CanMoveStruct *)pPtr;
					SetMovable( ( pStruct->bCanMove == TRUE ) ? true : false );
				}
			}
			break;
		case STE_SendAction_Weapon:
			{
				SendAction_WeaponStruct *pStruct = (SendAction_WeaponStruct *)pPtr;

				if( 2 <= pStruct->nWeaponIndex )	// 보조무기용 인덱스이다.
					break;

				if( !GetActiveWeapon( pStruct->nWeaponIndex ) ) break;
				if( pStruct->szActionName ) 
				{
					// 플레이어 액터인 경우현재 액션에서 쏠 수 있는 발사체 갯수에 무기의 발사체 갯수를 더해준다.
					if( GetActiveWeapon( pStruct->nWeaponIndex )->IsExistAction( pStruct->szActionName ) )
					{
						GetActiveWeapon( pStruct->nWeaponIndex )->SetActionQueue( pStruct->szActionName );
					}
				}

			}
			break;
		case STE_MotionSpeed:
			{
				if( IsModifyPlaySpeed() ) break;
				MotionSpeedStruct *pStruct = (MotionSpeedStruct *)pPtr;
				float fIncreadSpeed = 1.f / pStruct->fFrame * 60.f;
				DWORD dwTime = (DWORD)( ( SignalEndTime - SignalStartTime ) * fIncreadSpeed );
				float fSpeed = 1.f / 60.f * pStruct->fFrame;
				SetPlaySpeed( dwTime, fSpeed );
			}
			break;
		case STE_NormalSuperAmmor:
			{
				NormalSuperAmmorStruct *pStruct = (NormalSuperAmmorStruct *)pPtr;
				m_bEnableNormalSuperAmmor = ( pStruct->bEnable == TRUE ) ? true : false;
				m_nNormalSuperAmmorTime = pStruct->nTime;
				m_fBreakNormalSuperAmmorDurability = pStruct->fBreakDurability;

				if ( m_bIngnoreNormalSuperArmor )
				{
					m_bEnableNormalSuperAmmor = false;
				}
			}
			break;
		case STE_ResetVelocity:
			{
				ResetVelocityStruct *pStruct = (ResetVelocityStruct *)pPtr;
				if( pStruct->bXAxis ) {
					SetVelocityX( 0.f );
					SetResistanceX( 0.f );
				}
				if( pStruct->bYAxis ) {
					SetVelocityY( 0.f );
					SetResistanceY( 0.f );
				}
				if( pStruct->bZAxis ) {
					SetVelocityZ( 0.f );
					SetResistanceZ( 0.f );
				}
			}
			break;
		case STE_Projectile:
			{
				ProjectileStruct *pStruct = (ProjectileStruct *)pPtr;

				OutputDebug("%s m_bOnSignalFromChargerSE: %d   m_ActorType: %d", __FUNCTION__, m_bOnSignalFromChargerSE, m_ActorType);
				// Note: 플레이어인 경우엔 모든 프로젝타일 패킷을 클라로부터 받아서 생성합니다.
				// Direction 타입이라도 각 로컹 카메라 방향에 따른 설정도 있기 때문에..
				if( false == m_bOnSignalFromChargerSE )
					if( m_ActorType <= CDnActorState::Reserved6 )
						break;

				MatrixEx LocalCross = *GetMatEx();
				CDnProjectile *pProjectile = CDnProjectile::CreateProjectile( GetRoom(), GetMySmartPtr(), LocalCross, pStruct );
				if( pProjectile == NULL ) break;
				pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

				//bool bActorAttachWeapon = false;
				//if( pStruct->nWeaponTableID == 0 && GetWeapon(1) ) bActorAttachWeapon = true;
				if( m_bOnSignalFromChargerSE && m_hChargetDestActor )
				{
					DNVector(DnBlowHandle) vlhChargerBlows;
					m_hChargetDestActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_062, vlhChargerBlows );	// 1개 이상 중복되지 않는 게 좋은데 기획에서 어찌 할지.
					for( int i = 0; i < (int)vlhChargerBlows.size(); ++i )
					{
						if( vlhChargerBlows[i] )
							static_cast<CDnChargerBlow*>( vlhChargerBlows.at( i ).GetPointer() )->OnProjectileSignal( pProjectile );
					}
				}

				if( false == m_bOnSignalFromChargerSE )
				{
					OnProjectile( pProjectile, pStruct, LocalCross, nSignalIndex );
					OnSkillProjectile( pProjectile );
				}
			}
			break;
		case STE_SkillSuperAmmor:
			{
				SkillSuperAmmorStruct *pStruct = (SkillSuperAmmorStruct *)pPtr;

				m_nSkillSuperAmmorValue[0] = (int)( GetSuperAmmor() * pStruct->fSuperAmmorOneProb );
				m_nSkillSuperAmmorValue[1] = (int)( GetSuperAmmor() * pStruct->fSuperAmmorTwoProb );
				m_nSkillSuperAmmorValue[2] = (int)( GetSuperAmmor() * pStruct->fSuperAmmorThreeProb );
				m_nSkillSuperAmmorValue[3] = (int)( GetSuperAmmor() * pStruct->fSuperAmmorFourProb );
				memcpy( m_nLastUpdateSkillSuperAmmorValue, m_nSkillSuperAmmorValue, sizeof(m_nSkillSuperAmmorValue) );
				m_nSkillSuperAmmorTime = pStruct->nTime;
				m_fSkillSuperAmmorDamageDecreaseProb = pStruct->fDamageDecreaseProb;
			}
			break;
		case STE_SummonMonster:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonMonsterStruct* pStruct = (SummonMonsterStruct *)pPtr;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));
				if( pGameTask )
				{
					// 내부적으로 값을 바꿔서 사용하기 때문에 반드시 복사해서 사용한다.
#ifdef PRE_FIX_MEMOPT_SIGNALH
					SummonMonsterStruct Struct;
					CopyShallow_SummonMonsterStruct(Struct, pStruct);
#else
					SummonMonsterStruct Struct = *pStruct;
#endif
					// 167번 소환 몬스터 스킬레벨 강제 셋팅 상태효과 처리. ///////////////////
					if( IsPlayerActor() )
					{
						if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_167 ) )
						{
							DNVector(DnBlowHandle) vlBlows;
							GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_167, vlBlows );
							if( false == vlBlows.empty() )
							{
								Struct.nForceSkillLevel = (int)vlBlows.front()->GetFloatValue();
							}
						}
					}
					//////////////////////////////////////////////////////////////////////////

					if(Struct.PositionCheck == TRUE )
					{
						MatrixEx Cross = *GetMatEx();
						Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
						Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
						Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

						if( INSTANCE(CDnWorld).IsOnNavigationMesh( Cross.GetPosition() ) == false )
							break;
					}
			
					pGameTask->RequestSummonMonster( GetActorHandle(), &Struct );
				}
			}
			break;
		//case STE_SummonProp:
		//	{
		//		SummonPropStruct *pStruct = (SummonPropStruct *)pPtr;

		//		MatrixEx Cross = m_Cross;
		//		Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
		//		Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
		//		Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

		//		float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
		//		EtVector3 vRotate;
		//		vRotate = *pStruct->vRotate;
		//		vRotate.y += EtToDegree( acos(fDot) );

		//		CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

		//		DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, Cross.m_vPosition, vRotate, *pStruct->vScale );
		//	}
		//	break;



		// ****************************************************************************************************************
		// 프랍 소환 시그널에서 소환되는 프랍 클래스들은 반드시 void ReleasePostCustomParam() 를 구현해야 함!! 
		// 타입에 맞춰서 프랍 struct 를 삭제하지 않으면 boost::pool 에서 메모리 courrupt
		// ****************************************************************************************************************
		case STE_SummonChestProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonChestPropStruct *pStruct = (SummonChestPropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				ChestStruct* pChestPropInfo = new ChestStruct;
				pChestPropInfo->nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				pChestPropInfo->nNeedKeyID = pStruct->nNeedKeyID;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_Chest, Cross.m_vPosition, vRotate, *pStruct->vScale, pStruct->nLifeTime, (void*)pChestPropInfo );

				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		case STE_SummonBrokenProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonBrokenPropStruct *pStruct = (SummonBrokenPropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				BrokenStruct* pBrokenPropInfo = new BrokenStruct;
				pBrokenPropInfo->nDurability = pStruct->nDurability;
				pBrokenPropInfo->nItemDropGroupTableID = pStruct->nItemDropGroupTableID;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_Broken, Cross.m_vPosition, vRotate, *pStruct->vScale, pStruct->nLifeTime, (void*)pBrokenPropInfo );
				
				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		case STE_SummonBrokenDamageProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonBrokenDamagePropStruct *pStruct = (SummonBrokenDamagePropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				BrokenDamageStruct* pBrokenDamagePropInfo = new BrokenDamageStruct;
				pBrokenDamagePropInfo->nMonsterTableID = 102;	// goblin
				pBrokenDamagePropInfo->nDurability = pStruct->nDurability;
				pBrokenDamagePropInfo->nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				pBrokenDamagePropInfo->nSkillTableID = pStruct->nSkillTableID;
				pBrokenDamagePropInfo->nSkillLevel = pStruct->nSkillLevel;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_BrokenDamage, Cross.m_vPosition, vRotate, 
																		*pStruct->vScale, pStruct->nLifeTime, (void*)pBrokenDamagePropInfo );
				if( !hProp )
					break;

				MAActorProp* pActorProp = dynamic_cast<MAActorProp*>(hProp.GetPointer());
				_ASSERT( pActorProp );
				if( pActorProp )
					pActorProp->CopyActorStateFromThis( GetMySmartPtr() );

				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		case STE_SummonHitMoveDamageBrokenProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonHitMoveDamageBrokenPropStruct *pStruct = (SummonHitMoveDamageBrokenPropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				HitMoveDamageBrokenStruct* pHitMoveDamageBrokenPropInfo = new HitMoveDamageBrokenStruct;
				pHitMoveDamageBrokenPropInfo->nMonsterTableID = 102;	// goblin
				pHitMoveDamageBrokenPropInfo->nDurability = pStruct->nDurability;
				pHitMoveDamageBrokenPropInfo->nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				pHitMoveDamageBrokenPropInfo->MovingAxis = pStruct->MovingAxis;
				pHitMoveDamageBrokenPropInfo->fDefaultAxisMovingSpeed = pStruct->fDefaultAxisMovingSpeed;
				pHitMoveDamageBrokenPropInfo->fMaxAxisMoveDistance = pStruct->fMaxAxisMoveDistance;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_HitMoveDamageBroken, Cross.m_vPosition, vRotate, 
																	   *pStruct->vScale, pStruct->nLifeTime, (void*)pHitMoveDamageBrokenPropInfo );
				if( !hProp )
					break;

				MAActorProp* pActorProp = dynamic_cast<MAActorProp*>(hProp.GetPointer());
				_ASSERT( pActorProp );
				if( pActorProp )
					pActorProp->CopyActorStateFromThis( GetMySmartPtr() );

				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		case STE_SummonBuffProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonBuffPropStruct *pStruct = (SummonBuffPropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				BuffStruct* pBuffPropInfo = new BuffStruct;
				pBuffPropInfo->nMonsterTableID = 102; // goblin
				pBuffPropInfo->nSkillTableID = pStruct->nSkillTableID;
				pBuffPropInfo->nSkillLevel = pStruct->nSkillLevel;
				pBuffPropInfo->nTeam = pStruct->nTeam;
				pBuffPropInfo->fCheckRange = pStruct->fCheckRange;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_Buff, Cross.m_vPosition, vRotate, *pStruct->vScale, pStruct->nLifeTime, (void*)pBuffPropInfo );

				if( !hProp )
					break;

				MAActorProp* pActorProp = dynamic_cast<MAActorProp*>(hProp.GetPointer());
				_ASSERT( pActorProp );
				if( pActorProp )
					pActorProp->CopyActorStateFromThis( GetMySmartPtr() );

				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				static_cast<CDnBuffProp*>(hProp.GetPointer())->SetSummoner( GetMySmartPtr() );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		case STE_SummonBuffBrokenProp:
			{
				// #48747 아카데믹 같이 소환체로 공격을 하는 경우엔 스핵을 써서 걸려 있으면 소환도 안되어야 함..
				if( IsPlayerActor() && ((CDnPlayerActor*)this)->IsInvalidPlayerChecker() ) break;

				SummonBuffBrokenPropStruct *pStruct = (SummonBuffBrokenPropStruct *)pPtr;

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vPosition->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vPosition->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vPosition->z;

				float fDot = EtVec3Dot( &EtVector3( 0.f, 0.f, 1.f ), &Cross.m_vZAxis );
				EtVector3 vRotate;
				vRotate = *pStruct->vRotate;
				vRotate.y += EtToDegree( acos(fDot) );

				BuffBrokenStruct* pBuffBrokenPropInfo = new BuffBrokenStruct;
				pBuffBrokenPropInfo->nMonsterTableID = 102; // goblin
				pBuffBrokenPropInfo->nDurability = pStruct->nDurability;
				pBuffBrokenPropInfo->nItemDropGroupTableID = pStruct->nItemDropGroupTableID;
				pBuffBrokenPropInfo->nSkillTableID = pStruct->nSkillTableID;
				pBuffBrokenPropInfo->nSkillLevel = pStruct->nSkillLevel;
				pBuffBrokenPropInfo->nTeam = pStruct->nTeam;
				pBuffBrokenPropInfo->fCheckRange = pStruct->fCheckRange;

				CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));

				DnPropHandle hProp = pGameTask->RequestGenerationProp( pStruct->nSummonPropTableID, PTE_BuffBroken, Cross.m_vPosition, vRotate, *pStruct->vScale, pStruct->nLifeTime, (void*)pBuffBrokenPropInfo );

				if( !hProp )
					break;

				MAActorProp* pActorProp = dynamic_cast<MAActorProp*>(hProp.GetPointer());
				_ASSERT( pActorProp );
				if( pActorProp )
					pActorProp->CopyActorStateFromThis( GetMySmartPtr() );

				if( -1 < pStruct->nLifeTime )
					hProp->SetLifeTime( pStruct->nLifeTime );

				static_cast<CDnWorldBrokenBuffProp*>(hProp.GetPointer())->SetSummoner( GetMySmartPtr() );

				hProp->SetMasterActor(GetMySmartPtr());
			}
			break;
		// ****************************************************************************************************************
		// 프랍 소환 시그널에서 소환되는 프랍 클래스들은 반드시 void ReleasePostCustomParam() 를 구현해야 함!! 
		// ****************************************************************************************************************
		case STE_DropItem:
			{
				DropItemStruct *pStruct = (DropItemStruct*)pPtr;

				DNVector(CDnItem::DropItemStruct) VecList;
				CDnDropItem::CalcDropItemList( GetRoom(), pStruct->nDropItemTableID, VecList );

				MatrixEx Cross = m_Cross;
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vOffset->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vOffset->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vOffset->z;
				CDnItemTask *pTask = (CDnItemTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "ItemTask" );

				int nSeed = 0;
				for( DWORD i=0; i<VecList.size(); i++ ) 
				{
#if defined(PRE_FIX_46730)
					//bFixPosition이 true로 설정되면 nSeed값을 0으로 설정해서 아이템 위치 보정을 하지 않도록 한다.
					if (( pStruct->bFixPosition?true:false) == true)
						nSeed = 0;
					else
						nSeed = VecList[i].nSeed;
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[i].dwUniqueID, Cross.m_vPosition, VecList[i].nItemID, nSeed, VecList[i].nCount, 0, -1, VecList[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[i].dwUniqueID, Cross.m_vPosition, VecList[i].nItemID, nSeed, VecList[i].nCount, 0 );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#else
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[i].dwUniqueID, Cross.m_vPosition, VecList[i].nItemID, VecList[i].nSeed, VecList[i].nCount, 0, -1, VecList[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
					pTask->RequestDropItem( VecList[i].dwUniqueID, Cross.m_vPosition, VecList[i].nItemID, VecList[i].nSeed, VecList[i].nCount, 0 );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
#endif // PRE_FIX_46730
				}
			}
			break;
			// 한기 2009.7.27 
			// 자신에게 사용하는 효과 적용 시간 관련 (#1911)
		case STE_ApplyStateEffect:
			{
				// 스킬을 사용한 순간 상태효과 적용되기 전에 액터가 죽으면 상태효과는 추가되지 않으므로 
				// 아래로 상태효과 루틴을 진행시키지 않는다.
				if( GetHP() <= 0 )
				{
					if( m_hAuraSkill && m_hAuraSkill->IsAuraOn() )
					{
						OnSkillAura( m_hAuraSkill, false );
					}

					ClearSelfStateSignalBlowQueue();

					break;
				}

				if( false == m_dqApplySelfStateBlowQ.empty() )
				{
					ApplyStateEffectStruct* pStruct = (ApplyStateEffectStruct*)pPtr;
					if( pStruct->StateEffectIndex < (int)m_dqApplySelfStateBlowQ.size() )
					{
						const S_NO_PACKET_SELF_STATEBLOW& StateBlowInfo = m_dqApplySelfStateBlowQ.at( pStruct->StateEffectIndex );
						ApplyStateEffectSignalProcess(StateBlowInfo, pStruct);
					}
					else
					{
						OutputDebug( "[Error!!]CDnActor::OnSignal, case STE_ApplyStateEffect: 스킬에 들어있는 상태효과 갯수(%d)를 벗어난 적용 상태효과 인덱스(%d)임.\n", 
							(int)m_dqApplySelfStateBlowQ.size(), pStruct->StateEffectIndex );
						_ASSERT( !"case STE_ApplyStateEffect: 스킬에 들어있는 상태효과 갯수를 벗어난 적용 상태효과 인덱스임" );
					}
				}
			}
			break;
		case STE_Parring:
			{
				ParringStruct* pStruct = (ParringStruct*)pPtr;

				DNVector(DnBlowHandle) vlhParryBlows;
				m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_031, vlhParryBlows );
				
				int iNumParryBlow = (int)vlhParryBlows.size();
				for( int i = 0; i < iNumParryBlow; ++i )
				{
					DnBlowHandle hParryBlow = vlhParryBlows.at( i );
					if( !hParryBlow )
						continue;

					const CDnSkill::SkillInfo* pParentSkillInfo = hParryBlow->GetParentSkillInfo();
					if( pParentSkillInfo && pStruct->nSkillID == pParentSkillInfo->iSkillID )
					{
						CDnParryBlow* pParryBlow = static_cast<CDnParryBlow*>(hParryBlow.GetPointer());

						if( false == pParryBlow->IsEnabled() )
						{
							pParryBlow->EnableParrying( pStruct->fProb );
							pParryBlow->SetParringActionName( pStruct->szParringActionName );
						}
					}
				}
			}
			break;
		case STE_CooltimeParring:
			{
				CooltimeParringStruct* pStruct = (CooltimeParringStruct*)pPtr;

				DNVector(DnBlowHandle) vlhParryBlows;
				m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_153, vlhParryBlows );

				int iNumParryBlow = (int)vlhParryBlows.size();
				for( int i = 0; i < iNumParryBlow; ++i )
				{
					DnBlowHandle hParryBlow = vlhParryBlows.at( i );
					if( !hParryBlow )
						continue;

					const CDnSkill::SkillInfo* pParentSkillInfo = hParryBlow->GetParentSkillInfo();
					if( pParentSkillInfo && pStruct->nSkillID == pParentSkillInfo->iSkillID )
					{
						CDnCooltimeParryBlow* pParryBlow = static_cast<CDnCooltimeParryBlow*>(hParryBlow.GetPointer());

						if( false == pParryBlow->IsEnabled() )
						{
							pParryBlow->EnableParrying( pStruct->fProb );
							pParryBlow->SetParringActionName( pStruct->szParringActionName );
						}
					}
				}
			}
			break;
		case STE_Gravity:
			{
				GravityStruct *pStruct = (GravityStruct *)pPtr;

				MatrixEx CrossOffset = m_Cross;
				CrossOffset.MoveLocalXAxis( pStruct->vOffset->x );
				CrossOffset.MoveLocalYAxis( pStruct->vOffset->y );
				CrossOffset.MoveLocalZAxis( pStruct->vOffset->z );

				DNVector(DnActorHandle) hVecList;
				CDnActor::ScanActor( GetRoom(), CrossOffset.m_vPosition, pStruct->fDistanceMax, hVecList );

#ifdef PRE_ADD_GRAVITY_PROPERTY
				EtVector3 vPos = CrossOffset.m_vPosition;

				float fXZDistanceMaxSQ = pStruct->fDistanceMax;
				float fXZDistanceMinSQ = pStruct->fDistanceMin;

				fXZDistanceMaxSQ *= fXZDistanceMaxSQ;
				fXZDistanceMinSQ *= fXZDistanceMinSQ;
		
				SAABox Box;
				float fDot = 0.0f;
				EtVector3 vZVec = m_Cross.m_vZAxis;

				if( pStruct->fCenterAngle != 0.f ) 
				{
					EtMatrix matRotate;
					EtMatrixRotationY( &matRotate, EtToRadian( pStruct->fCenterAngle ) );
					EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );
				}
#endif // PRE_ADD_GRAVITY_PROPERTY

				for( DWORD i=0; i<hVecList.size(); i++ ) {
					if( !hVecList[i] ) continue;
					if( hVecList[i] == GetMySmartPtr() ) continue;
					if( hVecList[i]->IsDie() ) continue;
					if( hVecList[i]->GetWeight() == 0.f ) continue;
					if( hVecList[i]->IsNpcActor() ) continue;
					if( hVecList[i]->ProcessIgnoreGravitySignal() ) continue;

					bool bHittable = false;
					switch( pStruct->nTargetType ) {
						case 0: // Enemy
							if( hVecList[i]->GetTeam() == GetTeam() ) break;
							bHittable = true;
							break;
						case 1: // Friend
							if( hVecList[i]->GetTeam() != GetTeam() ) break;
							bHittable = true;
							break;
						case 2: // All
							bHittable = true;
							break;
					}
					if( !bHittable ) continue;

					bHittable = true;
					switch( pStruct->nApplyTargetState ) {
						case 0:	// Normal
							if( hVecList[i]->GetState() == CDnActorState::Down ) break;
							bHittable = true;
							break;
						case 1:	// Down
							if( !( hVecList[i]->GetState() & CDnActorState::Down ) ) break;
							bHittable = true;
							break;
						case 2:	// Normal + Down
							bHittable = true;
							break;
					}
					if( !bHittable ) continue;

#ifdef PRE_ADD_GRAVITY_PROPERTY
					bool bCheckArea = true;
					DnActorHandle hActor = hVecList[i];
					EtVector3 vDirActorToPos;
					vDirActorToPos = *hActor->GetPosition() - vPos;
					vDirActorToPos.y = 0.f;

					hActor->GetBoundingBox( Box );
					
					float fSQDistance = SquaredDistance( vPos, Box );

					if( fSQDistance > fXZDistanceMaxSQ ) 
						bCheckArea = false;
					
					if( fSQDistance <= fXZDistanceMinSQ )
						bCheckArea = false;

					if( pStruct->fAngle != 0.0f )
					{
						EtVec3Normalize( &vDirActorToPos, &vDirActorToPos );
						fDot = EtVec3Dot( &vZVec, &vDirActorToPos );
						if( EtToDegree( acos( fDot ) ) > pStruct->fAngle )
							bCheckArea = false;
					}

					if( pStruct->fHeightMax == 0.0f )
						pStruct->fHeightMax = 3000.0f;

					if( Box.Min.y < vPos.y + pStruct->fHeightMin && Box.Max.y < vPos.y + pStruct->fHeightMin ) 
						bCheckArea = false;

					if( Box.Min.y > vPos.y + pStruct->fHeightMax && Box.Max.y > vPos.y + pStruct->fHeightMax ) 
						bCheckArea = false;

					if( !bCheckArea )
					{
						hActor->SetVelocityZ( 0.0f );
						hActor->SetResistanceZ( 0.0f );
						hActor->ResetLook();
						continue;
					}
#endif // PRE_ADD_GRAVITY_PROPERTY

					MAMovementBase *pMovement = hVecList[i]->GetMovement();
					if( !pMovement ) continue;

					EtVector3 vDir = *hVecList[i]->GetPosition() - CrossOffset.m_vPosition;
					float fDistance = EtVec3Length( &vDir );
					float fDistance2 = EtVec3Length( &EtVector3( vDir.x, 0.f, vDir.z ) );
					EtVec3Normalize( &vDir, &vDir );

#ifdef PRE_ADD_GRAVITY_PROPERTY
					float fDistanceMaxToMin = pStruct->fDistanceMax - pStruct->fDistanceMin;
					float fTemp = 0.0f;
					if( fDistanceMaxToMin > 0.0f )
						fTemp = pStruct->fMinVelocity + ( ( (pStruct->fMaxVelocity - pStruct->fMinVelocity) / fDistanceMaxToMin ) * ( fDistanceMaxToMin - fDistance ) );

					float fHeight = CrossOffset.m_vPosition.y - hVecList[i]->GetPosition()->y;
					float fHeightMaxToMin = pStruct->fHeightMax - pStruct->fHeightMin;
					float fTempVertical = 0.0f;
					if( fHeightMaxToMin > 0.0f )
						fTempVertical = pStruct->fMinVelocity_Vertical + ( ( (pStruct->fMaxVelocity_Vertical - pStruct->fMinVelocity_Vertical) / fHeightMaxToMin ) * ( fHeightMaxToMin - fHeight ) );
#else // PRE_ADD_GRAVITY_PROPERTY
					float fTemp = pStruct->fMinVelocity + ( ( (pStruct->fMaxVelocity - pStruct->fMinVelocity) / pStruct->fDistanceMax ) * ( pStruct->fDistanceMax - fDistance ) );
#endif // PRE_ADD_GRAVITY_PROPERTY
					EtVector3 vVel;
					if( pMovement ) {
						EtVector3 vCurLook = *pMovement->GetLookDir();
						pMovement->Look( EtVec3toVec2(vDir), true );
						pMovement->SetVelocityZ( -fTemp );
						pMovement->SetResistanceZ( fTemp * 2.f );
#ifdef PRE_ADD_GRAVITY_PROPERTY
						if( fTempVertical > 0.0f )
						{
							pMovement->SetVelocityY( fTempVertical );
							pMovement->SetResistanceY( fTempVertical * 2.f );
							pMovement->SetAppliedYDistance( true );
							hVecList[i]->SetGravityEnd( 0.2f + ( float(SignalEndTime - SignalStartTime) / 1000.0f ) );
						}
#endif // PRE_ADD_GRAVITY_PROPERTY
						pMovement->Look( EtVec3toVec2(vCurLook), true );
					}
				}
			}
			break;
		case STE_MoveY:
			{
				if( m_pMovement )
				{
					MoveYStruct* pStruct = (MoveYStruct*)pPtr;
					float fYDelta = pStruct->fVelocityY * ((float(SignalEndTime - SignalStartTime) / 1000.0f));
					float fWholeMoveYDistance = (0.0f < fYDelta) ? fYDelta : -fYDelta;
					m_pMovement->SetMoveYDistancePerSec( pStruct->fVelocityY, fWholeMoveYDistance, (pStruct->bMaintainYPos == TRUE) ? true : false );
				}
			}
			break;
		// Signal Rotate.
		case STE_Rotate:
			{
				
				RotateStruct * pStruct = static_cast< RotateStruct * >( pPtr );															
				float fLength = (float)( SignalEndTime - SignalStartTime ) * (CDnActionBase::m_fFPS / 60.0f); 
				DWORD dwTime = (DWORD)( fLength - 16.6666f );
				SetRotate( dwTime, pStruct->fStartSpeed, pStruct->fEndSpeed, *(pStruct->vAxis), pStruct->bLeft == TRUE ? true : false );
			
			}
			break;
	}

}

LOCAL_TIME CDnActor::CheckRemainFrameTime()
{
	float fCurFrame = GetCurFrame();
	int nCurActionIndex = GetCurrentActionIndex();
	ActionElementStruct *pStruct = GetElement(nCurActionIndex);
	LOCAL_TIME remainTime = 0;

	if (pStruct)
		remainTime = pStruct->dwLength - (LOCAL_TIME)fCurFrame;

	return remainTime;
}

bool CDnActor::SetActionQueue( const char *szActionName, int nLoopCount, float fBlendFrame, float fStartFrame, bool bCheck, bool bCheckStateEffect )
{
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_224 ) )
	{
		DNVector(DnBlowHandle) vlhDisableActionSetBlow;
		m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_224, vlhDisableActionSetBlow );

		// 액션 셋 변경 상태효과는 여러개 있을 수 있다.
		int iNumBlow = (int)vlhDisableActionSetBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			CDnDisableActionBlow* pDisableActionSetBlow = static_cast<CDnDisableActionBlow*>( vlhDisableActionSetBlow.at( i ).GetPointer() );
			if(pDisableActionSetBlow->IsMatchedAction(szActionName))
				return false;
		}
	}

	const char *szName = szActionName;

	if( bCheck ) {
		if( m_szActionQueue.empty() ) {
			if( strcmp( szName, m_szAction.c_str() ) == NULL ) 
			{
				LOCAL_TIME remainTime = CheckRemainFrameTime();
				
				//현재 동작이 거의 끝까지 갔다면 동작 끝났다고 보고 다음 동작 예약한다..
				if (remainTime >= 20)
					return false;
			}
		}
		else {
			if( strcmp( szName, m_szActionQueue.c_str() ) == NULL ) 
			{
				LOCAL_TIME remainTime = CheckRemainFrameTime();

				//현재 동작이 거의 끝까지 갔다면 동작 끝났다고 보고 다음 동작 예약한다..
				if (remainTime >= 20)
					return false;
			}
		}
	}

	int nCurrentActionIndex = GetElementIndex( szName );
	if( bCheckStateEffect && nCurrentActionIndex > -1 )
	{
		int nActionState = GetState( nCurrentActionIndex );

		// State에 따라 세부적으로 나뉜다~
		// 공격불가일 땐 공격 state 인 액션은 사용 못함.
		if( (nActionState & ActorStateEnum::Attack) && !( nActionState & ActorStateEnum::IgnoreCantAction) )
		{
			if( (GetStateEffect() & CDnActorState::Cant_AttackAction) == CDnActorState::Cant_AttackAction ) 
			{
				if( !IsDie() || IsGMTrace() ) 
					return false;
			}
		}
	}

	CDnActionBase::SetActionQueue( szName, nLoopCount, fBlendFrame, fStartFrame );
	if( nCurrentActionIndex != -1 ) {
		for( int i=(int)m_VecPreActionState[nCurrentActionIndex].size()-1; i>=0; i-- ) {
			if( (int)fStartFrame >= m_VecPreActionState[nCurrentActionIndex][i].nOffset ) {
				SetState( (CDnActorState::ActorStateEnum)m_VecPreActionState[nCurrentActionIndex][i].nState );
				break;
			}
		}
		for( int i=(int)m_VecPreActionCustomState[nCurrentActionIndex].size()-1; i>=0; i-- ) {
			if( (int)fStartFrame >= m_VecPreActionCustomState[nCurrentActionIndex][i].nOffset ) {
				SetCustomState( (CDnActorState::ActorCustomStateEnum)m_VecPreActionCustomState[nCurrentActionIndex][i].nState );
				break;
			}
		}
		for( int i=(int)m_VecPreCanMoveState[nCurrentActionIndex].size()-1; i>=0; i-- ) {
			if( (int)fStartFrame >= m_VecPreCanMoveState[nCurrentActionIndex][i].nOffset ) {
				SetMovable( ( m_VecPreCanMoveState[nCurrentActionIndex][i].nState == TRUE ) ? true : false );
				break;
			}
		}
	}

	return true;
}

void CDnActor::SetCustomAction( const char *szActionName, float fFrame )
{
	if( m_hToggleSkill && m_hToggleSkill->IsToggleOn() )
	{
		CDnChangeActionStrProcessor* pChangeAction = static_cast<CDnChangeActionStrProcessor*>( m_hToggleSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		if( pChangeAction ) {
			pChangeAction->GetChangeActionName( szActionName );
		}
	}
	CDnActionBase::SetCustomAction( szActionName, fFrame );
}

void CDnActor::ProcessDown( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDie() ) return;
	if( GetState() == CDnActorState::Down && m_fDownDelta > 0.f ) 
	{
		m_fDownDelta -= ( fDelta * ( ( 1.f / 60.f ) * CDnActionBase::GetFPS() ) );
		if( m_fDownDelta <= 0.f ) {
			m_fDownDelta = 0.f;
			m_fLastDownRatio = 1.f;
			SetActionQueue( "Down_StandUp", 0, 0.f, 0.f, true, false );
		}
	}
}

void CDnActor::ProcessStiff( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDie() ) return;

	if( IsStiff() ) {
		if( m_fStiffDelta == 0.f ) return;
		m_fStiffDelta -= ( fDelta * ( ( 1.f / 60.f ) * CDnActionBase::GetFPS() ) );
		if( m_fStiffDelta <= 0.f ) {
			m_fStiffDelta = 0.f;
			ActionElementStruct *pStruct = GetElement( m_szAction.c_str() );
			if( pStruct ) SetActionQueue( pStruct->szNextActionName.c_str(), 0, 3.f );
			else SetActionQueue( "Stand", 0, 3.f );
		}
	}
	else m_fStiffDelta = 0.f;
}

void CDnActor::ProcessDie( LOCAL_TIME LocalTime, float fDelta )
{
	if( !IsDie() ) return;
	m_fDieDelta -= fDelta;
	if( m_fDieDelta <= 0.f ) {
		SetDestroy();
		return;
	}
}

void CDnActor::RequestKillAfterProcessStateBlow(DnActorHandle hHitter)
{
	m_bCompleteKill_AfterProcessStateBlow = true;
	m_hCompleteKillActor = hHitter;
}

void CDnActor::ExecuteKillAfterProcessStateBlow()
{
	if(m_bCompleteKill_AfterProcessStateBlow)
	{
		SetHP( 0 );
		RequestHPMPDelta( CDnState::ElementEnum_Amount, -( GetMaxHP() *2 ), GetUniqueID() );
		m_bCompleteKill_AfterProcessStateBlow = false;

		//최종 Die호출
		Die(m_hCompleteKillActor);
		m_hCompleteKillActor.Identity();
	}
}

void CDnActor::CmdRefreshHPSP( INT64 nHP, int nSP )
{
	SetHP( min( nHP, GetMaxHP() ) );
	SetSP( min( nSP, GetMaxSP() ) );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &m_nHP, sizeof(INT64) );
	Stream.Write( &m_nSP, sizeof(int) );
	
	//printf("[RLKT_DEBUG][%s] CurHP: %lld FuncRecvCurHP: %lld SP: %d\n", __FUNCTION__, m_nHP, nHP , m_nSP);

	Send( eActor::SC_REFRESHHPSP, &Stream );
}

void CDnActor::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame, bool bCheckOverlapAction, bool bFromStateBlow/* = false*/, bool bSkillChain/* = false*/ )
{
	SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, bCheckOverlapAction );

	// Note: 라이징 슬래시 처럼 입력에 따라 여러 액션이 나뉘어 나가는 경우 스킬의 
	// 상태효과가 지속되어야 하므로 스킬 쪽에 알려주도록 한다.
	// 서버로는 CmdActon 이 호출될 때 패킷에 같이 날아감.
	if( bSkillChain )
	{
		if( m_hProcessSkill )
			m_hProcessSkill->OnChainInput( szActionName );
	}
}

int CDnActor::CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit/* = false*/, bool bCheckCanBegin/* = true*/ ,  bool bEternity /*= false*/ )
{
	if( emBlowIndex < STATE_BLOW::BLOW_NONE || emBlowIndex >= STATE_BLOW::BLOW_MAX )
	{
		if( pParentSkillInfo )
			g_Log.Log(LogType::_ERROR, L"[CDnActor::CmdAddStateEffect] SkillID:%d, STATE_BLOW:%d\r\n", pParentSkillInfo->iSkillID, emBlowIndex );
		else
			g_Log.Log(LogType::_ERROR, L"[CDnActor::CmdAddStateEffect] STATE_BLOW:%d\r\n", emBlowIndex);
		return -1;
	}

	// [2011/03/18 semozz]
	// 상태효과 강제 Begin을 위해 코드 수정
	int nBlowID = AddStateBlow( emBlowIndex, pParentSkillInfo, nDurationTime, szParam, bOnPlayerInit, bCheckCanBegin , bEternity );
	
	DnBlowHandle hBlow = GetStateBlowFromID(nBlowID);
	if (pParentSkillInfo && pParentSkillInfo->bItemPrefixSkill)
		ForceBeginStateBlow(hBlow);

	return nBlowID;
}

void CDnActor::CmdModifyStateEffect( int iBlowID, STATE_BLOW& StateBlowInfo )
{
	DnBlowHandle hBlow = m_pStateBlow->GetStateBlowFromID( iBlowID );

	if( hBlow )
	{
		char acBuffer[ 32 ];
		CPacketCompressStream Stream( acBuffer, sizeof(acBuffer) );

		Stream.Write( &iBlowID, sizeof(int) );
		Stream.Write( &StateBlowInfo.fDurationTime, sizeof(float) );

		Send( eActor::SC_CMDMODIFYSTATEEFFECT, &Stream );	
	}
}

DnBlowHandle CDnActor::GetStateBlowFromID( int nStateBlowID )
{
	return m_pStateBlow->GetStateBlowFromID( nStateBlowID ); 
}

void CDnActor::CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bRemoveFromServerToo/* = true*/ )
{
	DelStateBlow( emBlowIndex );
}

void CDnActor::CmdRemoveStateEffectImmediately( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	if( !m_pStateBlow ) 
		return;

	m_pStateBlow->RemoveImediatlyStateEffectByBlowIndex( emBlowIndex );
}

void CDnActor::CmdRemoveStateEffectFromID( int nID )
{
#ifdef PRE_FIX_REMOVE_STATE_EFFECT_PACKET
	// 여기다 넣게되면 퍼포먼스 부하가 있을것 같기때문에 일단은 넣지않습니다.
	// 왠만한경우에서는 블로우 아이디를 찾은이후에 넣어주기때문에 굳이 검사할 필요는 없어보입니다.
	// if( m_pStateBlow->IsExistStateBlowFromBlowID( nID ) == false )
	//	return;
#endif

	m_pStateBlow->RemoveStateBlowFromID( nID );
	SendRemoveStateEffectFromID(nID);
}

void CDnActor::ClearTriggerBuff()
{
	for( int itr = 0; itr < (int)m_vTriggerBuff.size(); ++itr )
		CmdRemoveStateEffectFromID( m_vTriggerBuff[itr] );

	m_vTriggerBuff.clear();
}

void CDnActor::CmdWarp()
{
	CmdWarp( *GetPosition(), EtVec3toVec2( *GetLookDir() ) );
}

void CDnActor::CmdWarp( EtVector3 &vPos, EtVector2 &vLook, CDNUserSession* pGameSession/*=NULL*/, bool bCheckPlayerFollowSummonedMonster/*=false*/ )
{
	SetPosition( vPos );
	SetPrevPosition( vPos );
	Look( vLook, true );
	if( abs( CDnWorld::GetInstance(GetRoom()).GetHeight( vPos ) - vPos.y ) > 5.f ) {
		SetVelocityY( -3.0f );
		SetResistanceY( -18.0f );
	}

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

	// pGameSession 이 있는 경우는 해당 세션에게만 패킷 보낸다.
	if( pGameSession )
		Send( pGameSession, eActor::SC_CMDWARP, GetUniqueID(), &Stream );
	else
		Send( eActor::SC_CMDWARP, &Stream );
}

bool CDnActor::CheckDamageAction( DnActorHandle hActor )
{
	MAMovementBase *pMovement = GetMovement();
	// 모양세 셋팅
	if( ( IsAir() ) ||
		( IsHit() && IsDown() && !IsStun() ) 
		|| GetState() == ActorStateEnum::Down ) 
	{
			if( IsStandHit( m_HitParam.szActionName.c_str() ) ) 
			{
				m_HitParam.szActionName = "Hit_AirBounce";
				m_HitParam.vVelocity.y = pMovement->GetLastVelocity()->y * 0.8f;
				m_HitParam.vResistance.y = pMovement->GetResistance()->y;

				if( m_HitParam.vVelocity.y > 0.f && m_HitParam.vResistance.y >= 0.f )
				{
					m_HitParam.vResistance.y = -18.f;
				}
			}
	}

	return true;
}

void CDnActor::CheckDamageVelocity( DnActorHandle hActor )
{
	MAMovementBase *pMovement = GetMovement();

	// 바라보기 셋팅
	EtVector2 vViewVec = EtVec3toVec2( m_HitParam.vViewVec );
	EtVec2Normalize( &vViewVec, &vViewVec );
	if( EtVec2LengthSq( &vViewVec ) > 0.f ) {
		pMovement->Look( vViewVec );
	}

	// 만약에 전혀 띠워져있는 상태가 아님에두 Air 인 Hit 모션이 들어왔을 경우를 대비해서 여기서 임의의 수치로 띄어주기로 하자
	// (ex. 전사의 누워있는놈 때리기의 경우 스턴으로 맞을경우 떠야하는데 y값으로 가속도를 주지 않기 때문에 떠있는상태로 에니가 유지되는 경우가 있다. )
	ActorStateEnum HitState = (ActorStateEnum)GetState( m_HitParam.szActionName.c_str() );
	if( ( HitState & ActorStateEnum::Air ) && m_HitParam.vVelocity.y == 0.f ) {
		m_HitParam.vVelocity.y = 3.f;
		m_HitParam.vResistance.y = -18.f;
	}
	if( m_HitParam.vVelocity.y == 0.f && pMovement->GetVelocity() && pMovement->GetVelocity()->y != 0.f && 
		( GetAddHeight() != 0.f ) || ( m_HitParam.szActionName.empty() && ( GetState() & ActorStateEnum::Air ) ) ) {
		m_HitParam.vVelocity.y = pMovement->GetVelocity()->y;
		m_HitParam.vResistance.y = pMovement->GetResistance()->y;
	}

	// Weight 가 0이면 안뜨는놈이다. 그러나 시그널중에 Air 상태로 되있게되면 위의 예외처리루틴을 무시하게되기땜에
	// 바닥에서 Air 로 유지되는 놈들이 생길수가 있으니 0인놈은 상태가 절대 Air 로 되면 안된다.
	if( GetWeight() == 0.f ) 
	{
		// 외딴섬(301) 덩쿨 트랩처럼 무게값 0.0 으로 바꾸는 경우 대포같은 거 맞아서 
		// 공중에 뜨는 넉백 액션이 나올 경우 땅에 착지하지 못하는 버그가 생길 수 있으므로 수정. (#12253)
		if( IsAir( m_HitParam.szActionName.c_str() ) )
			m_HitParam.szActionName.assign( GetCurrentAction() );

		m_HitParam.vVelocity = EtVector3( 0.f, 0.f, 0.f );
		m_HitParam.vResistance = EtVector3( 0.f, 0.f, 0.f );
	}
	pMovement->SetVelocity( m_HitParam.vVelocity );
	pMovement->SetResistance( m_HitParam.vResistance );
}

int GetGaussianRandom( int nMin, int nMax, CMultiRoom *pRoom )
{
	float f1 = ( _rand(pRoom) % 10001 ) / 10000.f;
	float f2 = ( _rand(pRoom) % 10001 ) / 10000.f;

	int nResult = (int)( ((nMin+nMax)/2.f) + ( ( sqrt(-2.f*log(f1)) * cos(2.f*3.1415926f*f2) ) * ((nMax-nMin)/6.f) ) );

	if( nResult < nMin || nResult > nMax )
		nResult = (nMin + nMax) / 2; 

	return nResult;
};

float GetGaussianRandom( float fMin, float fMax, CMultiRoom *pRoom, float fPrecision = 0.01f )
{
	float f1 = ( _rand(pRoom) % 10001 ) / 10000.f;
	float f2 = ( _rand(pRoom) % 10001 ) / 10000.f;

	float fResult = ((fMin+fMax)/2.f) + ( ( sqrt(-2.f*log(f1)) * cos(2.f*3.1415926f*f2) ) * ((fMax-fMin)/6.f) );

	if( fResult < fMin || fResult > fMax )
		fResult = (fMin + fMax) / 2; 

	return fResult;
};


CDnState::ElementEnum CDnActor::CalcHitElementType( bool bUseSkillApplyWeaponElement, CDnState::ElementEnum eSkillElement, bool bUsingSkill )
{
	// #33312 특정 상황에서 강제로 셋팅한 속성이 있다면 해당 속성으로 처리.
	if( false == m_vlForceSettedHitElement.empty() )
		return static_cast<CDnState::ElementEnum>(m_vlForceSettedHitElement.back());

	bool bProcessSkill = (m_hProcessSkill ? true : false);
	ElementEnum eProcessSkillElement = CDnState::ElementEnum_Amount;
	if( m_hProcessSkill )
		eProcessSkillElement = m_hProcessSkill->GetElement();

	// 외부에서 스킬 사용 정보를 갖고 호출해줄 때(스킬로 나간 프로젝타일)
	if( bUsingSkill )
	{
		bProcessSkill = bUsingSkill;
		eProcessSkillElement = eSkillElement;
	}

	ElementEnum Type = CDnState::ElementEnum_Amount;
	if( bProcessSkill ) 
	{
		if( eProcessSkillElement == CDnState::ElementEnum_Amount ) 
		{
			if( bUseSkillApplyWeaponElement ) 
			{
				if( IsPlayerActor() )
				{
					Type = ( CDnActor::GetWeapon() ) ? CDnActor::GetWeapon()->GetElementType() : CDnState::ElementEnum_Amount;
				}
				else if( IsMonsterActor() )
				{
					Type = static_cast<CDnMonsterActor*>(this)->GetElementType();
				}
			}
		}
		else
		{
			Type = eProcessSkillElement;
		}
	}
	else
	{
		if( m_ActorType <= ActorTypeEnum::Reserved6 )
			Type = ( CDnActor::GetWeapon() ) ? CDnActor::GetWeapon()->GetElementType() : CDnState::ElementEnum_Amount;
		else
			Type = static_cast<CDnMonsterActor*>(this)->GetElementType();
	}

	return Type;
}


// 상태효과에 관련된 부분은 #pragma region 으로 접힘 가능하게 설정합니다
// Visual Studio 옵션에서 폰트옵션에 "축소 가능 택스트" 부분에 색지정을 해두시면 가독성이 높아집니다.
float CDnActor::PreCalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam, const float fDefenseRate/*=1.f*/, float fStateEffectAttackM /*= -1.0f */ )
{
	float fResult = 0.f;
	float fHitPer = HitParam.fDamage;
	float fAttackPower = 0.0f;
	
	int nDefense = 0;
	float fDefenseWeight = 0.0f;

	float fElementWeight = 1.f;
	float fGausianResultProb = 1.f;

	DnActorHandle hHitter = pHitter->GetActorHandle();
	CDnState *pState = NULL;
	CDnStateBlow *pHitterStateBlow = hHitter->GetStateBlow();

	bool bCalcDamageFromStateEffect = ( fStateEffectAttackM != -1.0f );

	if( bCalcDamageFromStateEffect == true )
	{
		if( hHitter )
			pState = static_cast<CDnActorState *>(hHitter.GetPointer());

		fAttackPower = fStateEffectAttackM;
		nDefense = GetDefenseM();
	}
	else
	{
		if( hHitter && !HitParam.bFromProjectile ) 
		{ 
			pState = static_cast<CDnActorState *>(hHitter.GetPointer());
		}
		else 
		{
			if( HitParam.hWeapon )
			{
				CDnProjectile *pHitterProjectile = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() );
				if( pHitterProjectile )
				{
					pState = pHitterProjectile->GetShooterStateSnapshot().get();
#if defined(PRE_ADD_PROJECTILE_SE_INFO)					
					CDnStateBlow *pProjectileStateBlow = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() )->GetShooterStateBlow().get();
					if( pProjectileStateBlow )
						pHitterStateBlow = pProjectileStateBlow;
#endif
				}
			}

#if !defined(PRE_ADD_PROJECTILE_SE_INFO)
			if (hHitter&& hHitter->IsApplyPrefixOffenceSkill())
				pState = static_cast<CDnActorState *>(hHitter.GetPointer());
#endif

			if( hHitter && pState == NULL )
				pState = static_cast<CDnActorState *>(hHitter.GetPointer());
		}
	}


	if( bCalcDamageFromStateEffect == false )
	{
		int nAttack[2] = { 0, };

		switch( HitParam.cAttackType ) 
		{
		case 0: // 물리
			{
				nAttack[0] = pState->GetAttackPMin();
				nAttack[1] = pState->GetAttackPMax();

#pragma region ++         [   ADD DAMAGE BY SELF ( BLOW_109 )    ]               ++

				if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_109 ) )
				{
					int aiAdditionalPAttack[ 2 ] = { 0 };
					DNVector(DnBlowHandle) vlhBlows;
					GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_109, vlhBlows );
					int iNumBlow = (int)vlhBlows.size();
					for( int i = 0; i < iNumBlow; ++i )
					{
						aiAdditionalPAttack[ 0 ] += int((float)GetAttackPMin() * vlhBlows.at(i)->GetFloatValue());
						aiAdditionalPAttack[ 1 ] += int((float)GetAttackPMax() * vlhBlows.at(i)->GetFloatValue());
					}

					nAttack[ 0 ] += aiAdditionalPAttack[ 0 ];
					nAttack[ 1 ] += aiAdditionalPAttack[ 1 ];
				}

#pragma endregion

				nDefense = GetDefenseP();
			}
			break;

		case 1: // 마법
		case 2: //rlkt_test
		{
				nAttack[0] = pState->GetAttackMMin();
				nAttack[1] = pState->GetAttackMMax();

#pragma region ++         [   ADD DAMAGE BY HITTER ( BLOW_180 )    ]               ++

				if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_180) )
				{
					CDnActorState* pActorState = static_cast<CDnActorState *>(hHitter.GetPointer());
					CDnState* pSkillState = pActorState ? pActorState->GetStateStep(1) : NULL;

					if (pSkillState)
					{
						nAttack[0] += pSkillState->GetAttackPMin();
						nAttack[1] += pSkillState->GetAttackPMax();
					}
				}

#pragma endregion

#pragma region ++         [   ADD DAMAGE BY SELF ( BLOW_110 )    ]               ++

				if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_110 ) )
				{
					int aiAdditionalMAttack[ 2 ] = { 0 };
					DNVector(DnBlowHandle) vlhBlows;
					GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_110, vlhBlows );
					int iNumBlow = (int)vlhBlows.size();
					for( int i = 0; i < iNumBlow; ++i )
					{
						aiAdditionalMAttack[ 0 ] += int((float)GetAttackMMin() * vlhBlows.at(i)->GetFloatValue());
						aiAdditionalMAttack[ 1 ] += int((float)GetAttackMMax() * vlhBlows.at(i)->GetFloatValue());
					}

					nAttack[ 0 ] += aiAdditionalMAttack[ 0 ];
					nAttack[ 1 ] += aiAdditionalMAttack[ 1 ];
				}

#pragma endregion

				nDefense = GetDefenseM();
			}
			break;
		}

#pragma region ++         [   ADD DAMAGE BY HITTER ( BLOW_213 )    ]               ++
		
		if( pHitterStateBlow && m_pStateBlow->IsApplied(STATE_BLOW::BLOW_213) )
		{
			int nAddAttack[2] = { 0, };
			int HitterAttackP[2] = {pState->GetAttackPMin(), pState->GetAttackPMax()};

			DNVector(DnBlowHandle) vlhBlows;
			pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_213, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				nAddAttack[ 0 ] += int((float)HitterAttackP[0] * vlhBlows.at(i)->GetFloatValue());
				nAddAttack[ 1 ] += int((float)HitterAttackP[1] * vlhBlows.at(i)->GetFloatValue());
			}

			nAttack[ 0 ] += nAddAttack[ 0 ];
			nAttack[ 1 ] += nAddAttack[ 1 ];
		}

#pragma endregion

#pragma region ++         [   ADD DAMAGE BY HITTER ( BLOW_214 )    ]               ++

		if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_214) )
		{
			int nAddAttack[2] = { 0, };
			int HitterAttackM[2] = {pState->GetAttackMMin(), pState->GetAttackMMax()};

			DNVector(DnBlowHandle) vlhBlows;
			pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_214, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				nAddAttack[ 0 ] += int((float)HitterAttackM[0] * vlhBlows.at(i)->GetFloatValue());
				nAddAttack[ 1 ] += int((float)HitterAttackM[1] * vlhBlows.at(i)->GetFloatValue());
			}

			nAttack[ 0 ] += nAddAttack[ 0 ];
			nAttack[ 1 ] += nAddAttack[ 1 ];
		}

#pragma endregion

#pragma region ++         [   ADD DAMAGE BY HITTER ( BLOW_221 )    ]               ++

		if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_221))
		{
			int nAddAttack[2] = { 0, };
			int HitterAttackValue[2] = {0, };
			
			switch( HitParam.cAttackType ) 
			{
			case 0: //물리 공격일때.
				HitterAttackValue[0] = pState->GetAttackPMin();
				HitterAttackValue[1] = pState->GetAttackPMax();
				break;
			case 1: //마법 공격일때.
				HitterAttackValue[0] = pState->GetAttackMMin();
				HitterAttackValue[1] = pState->GetAttackMMax();
				break;
			}
			
			DNVector(DnBlowHandle) vlhBlows;
			pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_221, vlhBlows );
			int iNumBlow = (int)vlhBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				nAddAttack[ 0 ] += int((float)HitterAttackValue[0] * vlhBlows.at(i)->GetFloatValue());
				nAddAttack[ 1 ] += int((float)HitterAttackValue[1] * vlhBlows.at(i)->GetFloatValue());
			}

			nAttack[ 0 ] += nAddAttack[ 0 ];
			nAttack[ 1 ] += nAddAttack[ 1 ];
		}

#pragma endregion


		float fFinalDamageValue[4] = 
		{ 
			CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FinalDamageValue1 ), 
			CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FinalDamageValue2 ), 
			CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FinalDamageValue3 ),
			CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FinalDamageValue4 ),
		};

		float fFValue = GetFinalDamageConstant();

		float fFinalDamage1 = (float)( pState->GetFinalDamage() / fFValue ) * fFinalDamageValue[3];
		float fFinalDamage2 = powf( (float)( pState->GetFinalDamage() / fFValue ), fFinalDamageValue[1] ) * fFinalDamageValue[2];
		float fResultFinalDamage = min( max(fFinalDamage1, fFinalDamage2), fFinalDamageValue[2] );

		// 184, 185번 공격자 파이널 데미지 물공/마공 구분 적용 상태효과
		float fResultAdditionalFinalDamage = 0.0f;
		
		if( 0 == HitParam.cAttackType ) // 물리 공격
		{

#pragma region ++         [   fResultAdditionalFinalDamage BY HITTER  ( BLOW_184 )    ]               ++

			if( pHitterStateBlow && pHitterStateBlow->IsApplied( STATE_BLOW::BLOW_184 ) )
			{
				DNVector(DnBlowHandle) vlhFinalPDamageBlows;
				pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_184, vlhFinalPDamageBlows );
				int iNumBlow = (int)vlhFinalPDamageBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					DnBlowHandle hBlow = vlhFinalPDamageBlows.at( i );
					fResultAdditionalFinalDamage += hBlow->GetFloatValue();
				}
			}

#pragma endregion

		}
		else if (1 == HitParam.cAttackType || 2 == HitParam.cAttackType) // 마법 공격// rlkt_test
		{

#pragma region ++         [   fResultAdditionalFinalDamage BY HITTER  ( BLOW_185 )    ]               ++

			if( pHitterStateBlow && pHitterStateBlow->IsApplied( STATE_BLOW::BLOW_185 ) )
			{
				DNVector(DnBlowHandle) vlhFinalPDamageBlows;
				pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_185, vlhFinalPDamageBlows );
				int iNumBlow = (int)vlhFinalPDamageBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					DnBlowHandle hBlow = vlhFinalPDamageBlows.at( i );
					fResultAdditionalFinalDamage += hBlow->GetFloatValue();
				}
			}

#pragma endregion

		}

		float fGausianResult = (float)GetGaussianRandom( nAttack[0], nAttack[1], GetRoom() );
		fGausianResultProb = fGausianResult / nAttack[1];
		fAttackPower = fGausianResult * fHitPer;

		float fDamageRatioBlowValue = 0.f;

#pragma region ++         [   fDamageRatioBlowValue BY HITTER  ( BLOW_050 )    ]               ++
		
		if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_050))
		{
			DNVector(DnBlowHandle) vlhDamageRatioBlows;
			pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_050, vlhDamageRatioBlows );
			int iNumBlow = (int)vlhDamageRatioBlows.size();
			for( int i = 0; i < iNumBlow; ++i )
			{
				DnBlowHandle hBlow = vlhDamageRatioBlows.at( i );
				fDamageRatioBlowValue += hBlow->GetFloatValue();
			}
		}

#pragma endregion


#if defined(PRE_FIX_65287)
		//발사체에 의해서 FinalDamageRate값이 설정되어 있는 경우는 이 설정값을 사용 하도록 한다.
		//스킬이 끝나고 나면 50번 상태효과가 사라진 다음이라 이 설정값을 사용 해야 함.
		float fShooterFinalDamageRate = 0.0f;
		if (HitParam.bFromProjectile)
		{
			CDnProjectile* pProjectile = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() );

			fShooterFinalDamageRate = pProjectile ? pProjectile->GetShooterFinalDamageRate() : 0.0f;

			if (fShooterFinalDamageRate != 0.0f)
				fDamageRatioBlowValue = fShooterFinalDamageRate;
		}
#endif // PRE_FIX_65287
		

		fAttackPower = fAttackPower * ( 1.f + fResultFinalDamage + fDamageRatioBlowValue + fResultAdditionalFinalDamage );

#pragma region ++         [   fAttackPower BY HITTER  ( BLOW_074 )    ]               ++
		// 74번 상태효과. 무조건 최대 데미지로 계산됨..
		if( pHitterStateBlow && pHitterStateBlow->IsApplied( STATE_BLOW::BLOW_074 ) ) 
			fAttackPower = (float)nAttack[1];
#pragma endregion


#pragma region ++         [   fAttackPower BY HITTER  ( BLOW_209 )    ]               ++		
		// 신규 캐릭터(아카데믹 관련)
		// 기본 공격력 비율 증가..(일반공격일경우만....) // 스킬을 사용중이지 않고 , 발사체가 아니거나 , 발사체라면 스킬사용 발사체가 아닐때 
		// 일단 아카데믹 인경우만 사용가능 하도록 설정이 되어있는 상태이다 , 다른클래스에 적용 할 때는 예외상황을 좀더 테스트 해주세요 
		// < Ex> 스탠스류 활성화 되어있는 상태에서 예외 상황 확인했으나 처리하지 않은 상태 > 
		if (false == hHitter->IsProcessSkill() && (( HitParam.bFromProjectile && !HitParam.bFromProjectileSkill ) || !HitParam.bFromProjectile ) && 
			!hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_121) && !hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_129) //해킹스텐스류)
			)
		{
			if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_209) )
			{
				DNVector(DnBlowHandle) vlBlowList;
				float fAddtionalAttackPower = 0.f;
				pHitterStateBlow->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_209, vlBlowList);

				for (int i=0 ; i< (int)vlBlowList.size(); ++i )
				{
					if(vlBlowList[i]->GetFloatValue() > 0.f)
					{
						fAddtionalAttackPower = fAttackPower * vlBlowList[i]->GetFloatValue(); 
						break;
					}
				}

				fAttackPower += fAddtionalAttackPower;
			}
		}
#pragma endregion

		if( fAttackPower < 0.f ) 
			fAttackPower = 0.f;
	}

	//rlkt_damage!
	CDnSkill::SkillInfo *pDmgSkillInfo = NULL;
	DnSkillHandle hDmgSkill;
	
	if (HitParam.bFromProjectile && HitParam.hWeapon)
		hDmgSkill = static_cast<CDnProjectile*>(HitParam.hWeapon.GetPointer())->GetParentSkill();
	else
		hDmgSkill = hHitter->GetProcessSkill();

	bool bIsPVP = GetGameRoom()->bIsPvPRoom();

	if (hDmgSkill)
	{
		pDmgSkillInfo = const_cast<CDnSkill::SkillInfo*>(hDmgSkill->GetInfo());
		if (bIsPVP)
		{
			CDNGameDataManager::TPVPSkills* nSkillInfo = NULL; 
			nSkillInfo = g_pDataManager->GetPVPSkillPDamage(pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);
			if (nSkillInfo)
			{		
				printf("hDmgSkill PVP OK atak: %.2f skill:%d lvl:%d\n", fAttackPower, pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);
				fAttackPower = (float)(fAttackPower*(float)(nSkillInfo->fPDamage + 1.f));//test
				printf("hDmgSkill PVP AFTER OK multi:%.4f atak: %.2f skill:%d lvl:%d\n", nSkillInfo->fPDamage, fAttackPower, pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);
			}
		} else {
			printf("hDmgSkill OK atak: %.2f skill:%d lvl:%d\n", fAttackPower, pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);
			TSkillLevelData *pData = g_pDataManager->GetSkillLevelData(pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);
			if (pData)
			{
				if (pData->nPhyDamage > 0.01f && fAttackPower > 0.001f)
					fAttackPower = fAttackPower + (float)(fAttackPower*(float)(pData->nPhyDamage + 1.f));//test
			}
			printf("hDmgSkill AFTER OK atak: %.2f skill:%d lvl:%d\n", fAttackPower, pDmgSkillInfo->iSkillID, pDmgSkillInfo->iLevel);

		}
	} 

#pragma region ++         [   nDefense BY SELF  ( BLOW_101 ~ BLOW_108 )    ]               ++		
	// [OBT] 추가 스킬 효과 구현 - 방어력 변경 (#7839) ///////////////////////////////////////
	STATE_BLOW::emBLOW_INDEX AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_NONE;
	switch( HitParam.DistanceType )
	{
		case CDnDamageBase::DistanceTypeEnum::Melee:
			{	
				if( 0 == HitParam.cAttackType )
				{
					// 물리 밀리 방어력 절대 변경 : 101
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_101 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_101;
					else
					// 물리 밀리 방어력 비율 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_102 ) )						
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_102;
				}
				else
				if( 1 == HitParam.cAttackType || 2 == HitParam.cAttackType) //rlkt_test
				{
					// 마법 밀리 방어력 절대 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_105 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_105;
					else
					// 마법 밀리 방어력 비율 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_106 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_106;
				}
			}
			break;

		case CDnDamageBase::DistanceTypeEnum::Range:
			{
				if( 0 == HitParam.cAttackType )
				{
					// 물리 레인지 방어력 절대 변경 : 101
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_103 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_103;
					else
					// 물리 레인지 방어력 비율 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_104 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_104;
				}
				else
				if( 1 == HitParam.cAttackType || 2 == HitParam.cAttackType) //rlkt_test
				{
					// 마법 레인지 방어력 절대 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_107 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_107;
					else
					// 마법 레인지 방어력 비율 변경
					if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_108 ) )
						AppliedDefenseChangeBlowIndex = STATE_BLOW::BLOW_108;
				}
			}
			break;
	}

	if( STATE_BLOW::BLOW_NONE != AppliedDefenseChangeBlowIndex )
	{
		switch( AppliedDefenseChangeBlowIndex )
		{
			// 절대값 변경
			case STATE_BLOW::BLOW_101: 
			case STATE_BLOW::BLOW_103:
			case STATE_BLOW::BLOW_105:
			case STATE_BLOW::BLOW_107:
				{
					DNVector(DnBlowHandle) vlhDefenseAbChangeBlow;
					GetStateBlow()->GetStateBlowFromBlowIndex( AppliedDefenseChangeBlowIndex, vlhDefenseAbChangeBlow );
					int iNumBlow = (int)vlhDefenseAbChangeBlow.size();
					for( int i = 0; i < iNumBlow; ++i )
					{
						DnBlowHandle hBlow = vlhDefenseAbChangeBlow.at( i );
						int iValue = (int)hBlow->GetFloatValue();
						nDefense += iValue;
					}
				}
				break;

			// 비율 변경
			case STATE_BLOW::BLOW_102:
			case STATE_BLOW::BLOW_104:
			case STATE_BLOW::BLOW_106:
			case STATE_BLOW::BLOW_108:
				{
					DNVector(DnBlowHandle) vlhDefenseRtChangeBlow;
					GetStateBlow()->GetStateBlowFromBlowIndex( AppliedDefenseChangeBlowIndex, vlhDefenseRtChangeBlow );
					float fChangeRatio = 1.0f;		// 복수개가 있는 경우를 감안해서 100%를 넘어가는 수치만 누적시켜준다.
					int iNumBlow = (int)vlhDefenseRtChangeBlow.size();
					for( int i = 0; i < iNumBlow; ++i )
					{
						DnBlowHandle hBlow = vlhDefenseRtChangeBlow.at( i );
						fChangeRatio += (hBlow->GetFloatValue()-1.0f);
					}

					nDefense += int( (float)nDefense*fChangeRatio );
				}
				break;
		}
	}
#pragma endregion


	float fDefensePower = 1.f;

#if defined(PRE_ADD_SKILLBUF_RENEW)

	//모든 공격력 계산에는 방어력이 적용 된다.

	fDefenseWeight = hHitter->GetDefenseConstant();

	fDefensePower = nDefense / fDefenseWeight;
	if( fDefensePower < 0.f ) fDefensePower = 0.f;
	else if( fDefensePower > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax ) ) 
		fDefensePower = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax );

	fDefensePower = 1.f - fDefensePower;

	//크리티컬인 경우 AttackPower를 설정된 상수값을 적용시킨다.
	if (HitParam.HitType == CDnWeapon::Critical)
	{
		//#57510 PvP에서 크리티컬 데미지 1.5
		float fCriticalIncValue = 1.0f;

		if( GetGameRoom()->bIsPvPRoom() == true )
			fCriticalIncValue = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalIncrease_PVP );
		else
			fCriticalIncValue = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalIncrease );

		fAttackPower = fAttackPower * fCriticalIncValue;
	}

#else // PRE_ADD_SKILLBUF_RENEW

	if( HitParam.HitType == CDnWeapon::Critical ) 
	{
		fDefensePower = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalIncrease );
	}
	else 
	{
		fDefenseWeight = hHitter->GetDefenseConstant();

		fDefensePower = nDefense / fDefenseWeight;
		if( fDefensePower < 0.f ) fDefensePower = 0.f;
		else if( fDefensePower > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax ) ) 
			fDefensePower = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DefenseMax );

		fDefensePower = 1.f - fDefensePower;
	}

#endif // PRE_ADD_SKILLBUF_RENEW


	fResult = fAttackPower * fDefensePower;

#pragma region ++         [   fResult *= fChangeRatio BY SELF  ( BLOW_134, BLOW_135 )    ]               ++

	// 물리/마법 데미지 변경 #12354, #12353 ///////////////////////////////////////////
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_134 ) && 0 == HitParam.cAttackType )
	{

		DNVector( DnBlowHandle ) vlhBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_134, vlhBlows );
		
		float fChangeRatio = 1.0f;		// 복수개가 있는 경우를 감안해서 100%를 넘어가는 수치만 누적시켜준다.
#ifdef PRE_ADD_BUFF_STATE_LIMIT
		float fBuffStateRatioValue = 1.f;
#endif
		for( int i = 0; i < (int)vlhBlows.size(); ++i )
		{
			DnBlowHandle hBlow = vlhBlows.at( i );
			float fValue = hBlow->GetFloatValue();
			fChangeRatio += (fValue-1.0f);
#ifdef PRE_ADD_BUFF_STATE_LIMIT
			if( hBlow->GetAddBlowStateType() == CDnActorState::AddBlowStateType::Equip_Buff_Level )
				fBuffStateRatioValue += (fValue-1.0f);
#endif
		}


#ifdef PRE_ADD_BUFF_STATE_LIMIT
		float fMinLimit = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Change_DamageTaken_Physical_Min );
		float fMaxLimit = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Change_DamageTaken_Physical_Max );
		
		if(IsPlayerActor())
		{
			if( fChangeRatio < fMinLimit ) //#77817 유저가 받는 데미지 일경우에는 총 데미지 감소량을 기준으로 설정.
				fChangeRatio = fMinLimit;
		}
		else if(IsMonsterActor())
		{
			if( fBuffStateRatioValue > fMaxLimit )
				fChangeRatio += -fBuffStateRatioValue + fMaxLimit;
		}
#else
		float fDamageChangeClampLowestRatio = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DamageChangeClampLowestRatio );
		if( fChangeRatio < fDamageChangeClampLowestRatio )
			fChangeRatio = fDamageChangeClampLowestRatio;
#endif

		if(fChangeRatio < 0) 
			fChangeRatio = 0.f;

		fResult *= fChangeRatio;


	}
	else if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_135 ) && 1 == HitParam.cAttackType 
			|| 	IsAppliedThisStateBlow(STATE_BLOW::BLOW_135) && 2 == HitParam.cAttackType) //rlkt_test
	{

		DNVector( DnBlowHandle ) vlhBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_135, vlhBlows );
		
		float fChangeRatio = 1.f;
#ifdef PRE_ADD_BUFF_STATE_LIMIT
		float fBuffStateRatioValue = 1.f;

#endif
		for( int i = 0; i < (int)vlhBlows.size(); ++i )
		{
			DnBlowHandle hBlow = vlhBlows.at( i );
			float fValue = hBlow->GetFloatValue();
			fChangeRatio += (fValue-1.0f);

#ifdef PRE_ADD_BUFF_STATE_LIMIT
			if( hBlow->GetAddBlowStateType() == CDnActorState::AddBlowStateType::Equip_Buff_Level )
				fBuffStateRatioValue += (fValue-1.0f);
#endif
		}

#ifdef PRE_ADD_BUFF_STATE_LIMIT
		float fMinLimit = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Change_DamageTaken_Magical_Min );
		float fMaxLimit = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Change_DamageTaken_Magical_Max );

		if(IsPlayerActor())
		{
			if( fChangeRatio < fMinLimit ) //#77817 유저가 받는 데미지 일경우에는 총 데미지 감소량을 기준으로 설정.
				fChangeRatio = fMinLimit;
		}
		else if(IsMonsterActor())
		{
			if( fBuffStateRatioValue > fMaxLimit )
				fChangeRatio += -fBuffStateRatioValue + fMaxLimit;
		}
#else
		float fDamageChangeClampLowestRatio = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::DamageChangeClampLowestRatio );
		if( fChangeRatio < fDamageChangeClampLowestRatio )
			fChangeRatio = fDamageChangeClampLowestRatio;
#endif


		if(fChangeRatio < 0) 
			fChangeRatio = 0.f;

		fResult *= fChangeRatio;

	}

#pragma endregion


#pragma region ++         [   fResult += fResult * fChangeRatio BY SELF  ( BLOW_268 )    ]               ++
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (IsAppliedThisStateBlow( STATE_BLOW::BLOW_268 ))
	{
		DNVector( DnBlowHandle ) vlhBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_268, vlhBlows );

		float fChangeRatio = 0.0f;
		for( int i = 0; i < (int)vlhBlows.size(); ++i )
		{
			DnBlowHandle hBlow = vlhBlows.at( i );
			if (hBlow && hBlow->IsEnd() == false)
			{
				CDnDamageChagneBlow* pDamageChageBlow = static_cast<CDnDamageChagneBlow*>(hBlow.GetPointer());
				if (pDamageChageBlow && pDamageChageBlow->IsActivated() == true && pDamageChageBlow->GetLeftCoolTime() > 0.0f)
				{
					fChangeRatio += pDamageChageBlow->GetDamageRate();
				}
			}
		}

		if (fChangeRatio >= 0.0f)
		{
			fResult += fResult * fChangeRatio;
		}
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL
#pragma endregion


	if( HitParam.HasElement != CDnActorState::ElementEnum_Amount ) 
	{
		fElementWeight = 1.f;
		fElementWeight = ( ( 1.f + pState->GetElementAttack( HitParam.HasElement ) ) * ( 1.f - GetElementDefense( HitParam.HasElement ) ) );
#ifndef PRE_ADD_BUFF_STATE_LIMIT
		fElementWeight = max( fElementWeight, CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::ElementMinRevision ) );
#endif
		fResult *= fElementWeight;
	}

	// 크리티컬 저항 계산해서 대미지 깍아준다. (브레이킹 포인트(슈퍼아머 브레이크)가 설정되어 있지 않아야함)
	if( HitParam.HitType == CDnWeapon::Critical && false == HitParam.bBreakSuperAmmor) 
	{
		float fCriticalValue = hHitter->GetCriticalConstant();
		float fCriticalResistProb = GetCriticalResistance() / fCriticalValue;
		if( fCriticalResistProb > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalResistanceMax ) )
			fCriticalResistProb = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalResistanceMax );
		if( _rand(GetRoom())%100 < (int)( fCriticalResistProb * 100.f ) ) {
#if defined(PRE_FIX_51491)
			//크리티컬로 발생했던 데미지 가산 값을 없앤다..
			//#57510 PvP에서 크리티컬 데미지 1.5

			float fCriticalIncValue = 1.0f;
			if( GetGameRoom()->bIsPvPRoom() == true )
				fCriticalIncValue = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalIncrease_PVP );
			else
				fCriticalIncValue = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalIncrease );

			fResult = fResult / fCriticalIncValue;
#else
			fResult *= CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalResistance );
#endif // PRE_FIX_51491

			m_HitParam.HitType = CDnWeapon::CriticalRes;
		}
	}

#pragma region ++         [   pComboDamageLimitBlow BY SELF  ( BLOW_242 )    ]               ++
	// #52332 일정 HIT수 이후에 데미지 감소되는 효과 구현
	// 242번 상태효과 적용되어 있는 경우, 지금 데미지 계산에 사용된 스킬/액터를 이용해서 Hit카운트 제한값을 갱신한다..
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_242 ) )
	{
		CDnSkill::SkillInfo *pSkillInfo = NULL;
		DnSkillHandle hSkill;

		if (HitParam.bFromProjectile && HitParam.hWeapon)
			hSkill = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() )->GetParentSkill();
		else
			hSkill = hHitter->GetProcessSkill();
		
		if (hSkill)
			pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hSkill->GetInfo());
		
		DNVector(DnBlowHandle) vlBlows;
		m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_242, vlBlows );
		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
			{
				CDnComboDamageLimitBlow* pComboDamageLimitBlow = dynamic_cast<CDnComboDamageLimitBlow*>(hBlow.GetPointer());
				if (pComboDamageLimitBlow)
					pComboDamageLimitBlow->UpdateHitCount(pSkillInfo, HitParam);
			}
		}
	}
#pragma endregion

	// 상태효과에서 추가 데미지를 줄 수 있음. 
	fResult += m_pStateBlow->OnCalcDamage( fResult, m_HitParam );

#if defined(PRE_FIX_59680)
	//서먼 퍼펫의 OnCalcDamage를 호출 할때 주인 액터의 StateBlow의 OnCalcDamage도 호출이 되어야 주인 액터 데미지 적용될때 효과가 정상 적용된다.
	//클레릭의 홀리쉴드의 경우 자신에게 CDnAddStateOnHitBlow(178) 상태효과가 적용되고, 이후에 서먼퍼펫이 소환 될경우, 서먼퍼펫은 이 상태효과를
	//가지고 있지 않다. 그래서 서먼퍼펫으로 데미지가 클레릭에 적용되지만 데미지에 의한 HP증가 상태효과는 발동되지 않는다.
	if (IsMonsterActor())
	{
		DnActorHandle hSummonerActor;
		CDnStateBlow *pOwnerStateBlow = NULL;

		CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(m_MySmartPtr.GetPointer());
		if (pMonsterActor && pMonsterActor->IsSummonedMonster() && pMonsterActor->IsPuppetSummonMonster())
			hSummonerActor = pMonsterActor->GetSummonerPlayerActor();

		if(hSummonerActor)
			pOwnerStateBlow = hSummonerActor->GetStateBlow();

		if (pOwnerStateBlow)
			pOwnerStateBlow->OnCalcDamage(fResult, m_HitParam);
	}
#endif // PRE_FIX_59680

	// OnTargetHit시 데미지확인을 위해 [2011/02/15 semozz]
	m_HitParam.nCalcDamage = (int)fResult;
	hHitter->GetStateBlow()->OnTargetHit( GetMySmartPtr() );

#pragma region ++         [   fResult -= fAbsorbDamage BY SELF  ( BLOW_243 )    ]               ++
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_243 ) )
	{
		float fAbsorbDamage = 0.f;
		int nAbsorbSP = 0;

		DNVector( DnBlowHandle ) vlhManaShieldBlow;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_243, vlhManaShieldBlow );
		for( DWORD n=0; n<vlhManaShieldBlow.size(); n++ )
		{
			CDnManaShieldBlow* pManShieldBlow = static_cast<CDnManaShieldBlow*>(vlhManaShieldBlow[n].GetPointer());
			pManShieldBlow->CalcManaShield( fResult , fAbsorbDamage , nAbsorbSP );
		}

		if( nAbsorbSP > GetSP() )
		{
			fAbsorbDamage -= (fAbsorbDamage * (nAbsorbSP - GetSP()) / nAbsorbSP);
			nAbsorbSP = GetSP();
		}

		if( fAbsorbDamage > fResult )
			fAbsorbDamage = fResult;

		fResult -= fAbsorbDamage;

		SetSP( GetSP() -nAbsorbSP );
		RequestHPMPDelta( ElementEnum::ElementEnum_Amount, -nAbsorbSP, UINT_MAX, true , true );
	}
#pragma endregion

#pragma region ++         [   fResult = 0.0f; BY SELF  ( BLOW_051 )    ]               ++
#if defined(PRE_FIX_61382)
	if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_051))
	{
		DNVector(DnBlowHandle) vlhTransmitBlows;
		GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_051, vlhTransmitBlows );

		int iNumBlow = (int)vlhTransmitBlows.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			CDnTransmitDamageBlow* pTransmitDamageBlow = static_cast<CDnTransmitDamageBlow*>(vlhTransmitBlows[i].GetPointer());
			
			//데미지 전달되는 액터의 히트 여부를 확인 해서 히트 가능일때만 데미지 전달한다.
			DnActorHandle hTargetActor = pTransmitDamageBlow->GetActorHandle();
			
			bool isEnableTransmitDamage = false;

			if (hTargetActor)
			{
				if (hTargetActor->IsMonsterActor())
				{
					CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hTargetActor.GetPointer());
					isEnableTransmitDamage = pMonsterActor ? pMonsterActor->IsEnableNoDamage() == false : true;
				}
				else
					isEnableTransmitDamage = true;				
			}

			//데미지 전달 가능 하면 데미지 전달 시킨다.
			if (isEnableTransmitDamage == true)
				pTransmitDamageBlow->TransmitDamage( fResult , m_HitParam );
		}

		//데미지 전달 하고 나서 데미지 0으로 리셋 시킴..
		fResult = 0.0f;
	}
#endif // PRE_FIX_61382
#pragma endregion

#pragma region ++         [  STAGE DAMAGE LIMIT  ]               ++
#ifdef PRE_ADD_STAGE_DAMAGE_LIMIT
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );
	if( pTask && pTask->GetStageDamageLimit()->fDamageLimit > 0.f )
	{
		if( hHitter )
		{
			CDnPlayerActor *pPlayerActor = NULL;

			if( hHitter->IsPlayerActor() )
			{
				pPlayerActor = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			}
			else if( hHitter->IsMonsterActor() )
			{
				CDnMonsterActor *pMonsterActor = static_cast<CDnMonsterActor*>(hHitter.GetPointer());
				if( pMonsterActor && pMonsterActor->GetSummonerPlayerActor() )
					pPlayerActor = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			}

			if( pPlayerActor )
			{
				bool bNotifyInterface = false;

				float fBaseAttackPower = 0.f;
				if( HitParam.cAttackType == 0)
				{
					fBaseAttackPower = pState->GetAttackPMax() * fGausianResultProb * fHitPer;
					bNotifyInterface = pPlayerActor->GetDamageLimitInfo()->bPhysicalAttack;
				}
				else if (HitParam.cAttackType == 1)
				{
					fBaseAttackPower = pState->GetAttackMMax() * fGausianResultProb * fHitPer;
					bNotifyInterface = pPlayerActor->GetDamageLimitInfo()->bMagicalAttack;
				}
				else if (HitParam.cAttackType == 2) //rlkt_test
				{
					fBaseAttackPower = pState->GetAttackMMax() * fGausianResultProb * fHitPer;
					bNotifyInterface = pPlayerActor->GetDamageLimitInfo()->bMagicalAttack;
				}

				if( fResult > (fBaseAttackPower * pTask->GetStageDamageLimit()->fDamageLimit) )
				{
					fResult = (fBaseAttackPower * pTask->GetStageDamageLimit()->fDamageLimit);
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
					if( bNotifyInterface == true )
						HitParam.bStageLimit = true;
#endif
				}
			}
		}
	}
#endif
#pragma endregion


	if( fResult < 0.f ) 
		fResult = 0.f;

	return fResult;
}


float CDnActor::CalcDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	float fResult = PreCalcDamage( pHitter, HitParam );

#if defined( NODAMAGE ) || defined( STRESS_TEST )
	return fResult;
#endif

#if defined(PRE_ADD_MISSION_COUPON)
	INT64 nSaveHP = m_nHP;
	SetHP((INT64)( m_nHP - fResult ));
	m_nHP = (INT64)( nSaveHP - fResult );
#else
	m_nHP = (INT64)( m_nHP - fResult );
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (IsAppliedThisStateBlow( STATE_BLOW::BLOW_268 ))
	{
		float fHpRate = 1.0f;
		if (m_nMaxHP > 0)
			fHpRate = (float)m_nHP / (float)m_nMaxHP;

		DNVector( DnBlowHandle ) vlhBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_268, vlhBlows );

		float fChangeRatio = 0.0f;
		for( int i = 0; i < (int)vlhBlows.size(); ++i )
		{
			DnBlowHandle hBlow = vlhBlows.at( i );
			if (hBlow && hBlow->IsEnd() == false)
			{
				CDnDamageChagneBlow* pDamageChageBlow = static_cast<CDnDamageChagneBlow*>(hBlow.GetPointer());
				if (pDamageChageBlow)
				{
					float fHPLimit = pDamageChageBlow->GetHpLimit();

					if (fHpRate < fHPLimit &&
						pDamageChageBlow->IsActivated() == false &&
						pDamageChageBlow->GetLeftCoolTime() == 0.0f)
					{
						pDamageChageBlow->ApplyDamageChange();
					}
				}
			}
		}
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	return fResult;
}

void CDnActor::Die( DnActorHandle hHitter )
{
	// 몬스터 막타친 것도 CP 점수 처리해야해서 이곳에서 대체 시켜 준다.
	if( hHitter && hHitter->IsMonsterActor() && IsMonsterActor() )
	{
		DnActorHandle hSummonMasterPlayerActor = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
		if( hSummonMasterPlayerActor )
			hHitter = hSummonMasterPlayerActor;
	}

#if defined( PRE_ADD_58761 )
	// 죽었을떄 로그 남기기	
	CDNGameRoom *pRoom = GetGameRoom();	
	if( pRoom && CDnWorld::GetInstance(pRoom).GetMapSubType() == EWorldEnum::MapSubTypeNest)
	{
		if( hHitter && hHitter->IsMonsterActor() && IsPlayerActor() )
		{
			DnMonsterActorHandle hMonster = hHitter;
			int nSkillID = 0;
			if(hMonster->GetProcessSkill())
				nSkillID = hMonster->GetProcessSkill()->GetClassID();
			CDNUserSession *pSession = pRoom->GetUserSession( GetSessionID() );
			if (pSession)
				pRoom->NestDeathLog( pSession, hMonster->GetMonsterClassID(), nSkillID, pSession->GetUserJob(), pSession->GetLevel());
			else
				_DANGER_POINT();
		}
	}
#endif
#if defined(PRE_FIX_44884)
	bool bStateBlowProcessed = true;

	if (IsDie())
		bStateBlowProcessed = m_pStateBlow->OnDie( hHitter );

	if (IsDie())
		OnDie(hHitter);
#else
	OnDie( hHitter );
	bool bStateBlowProcessed = m_pStateBlow->OnDie( hHitter );
#endif // PRE_FIX_44884

	bool bCheckDie = false;
	
	if( false == bStateBlowProcessed )
	{
		if( IsStandHit( m_HitParam.szActionName.c_str() ) )
			m_HitParam.szActionName = "Die", bCheckDie = true;
		if( IsStun( m_HitParam.szActionName.c_str() ) && !IsAir( m_HitParam.szActionName.c_str() ) ) 
			m_HitParam.szActionName = "Die", bCheckDie = true;

		if( !bCheckDie ) {
			// 홀드 스킬 때문에 Stay 조건도 포함.. 안그러면 Die 액션이 발동이 안되어서 hold 맞고 한방에 죽을 경우 클라에서 "stand" 액션으로 
			// OnDamage 패킷을 받아 선 채로 죽게 됨..
			if( IsStay( m_HitParam.szActionName.c_str() ) )
				m_HitParam.szActionName = "Die", bCheckDie = true;
			
			if( IsAir( m_HitParam.szActionName.c_str() ) ) {
				if( IsExistAction( "Die_Air" ) ) m_HitParam.szActionName = "Die_Air", bCheckDie = true;
			}
			if( IsDown( m_HitParam.szActionName.c_str() ) ) {
				if( IsExistAction( "Die_Down" ) ) m_HitParam.szActionName = "Die_Down", bCheckDie = true;
			}

			if( !bCheckDie ) m_HitParam.szActionName = "Die";
		}
	}
	else
	{
		// 몬스터가 죽으면서 사용하는 스킬 때문에 Die 액션이 아니라 그냥 stand 시킴. (#16331)
		m_HitParam.szActionName = "Stand";
	}

	m_bEnableNormalSuperAmmor = false;

	if (hHitter)
	{
		if (hHitter->IsMonsterActor())
		{
			CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(hHitter.GetPointer());
			if (pMonster && pMonster->IsCannonMonsterActor())
			{
				CDnCannonMonsterActor* pCannonMonster = static_cast<CDnCannonMonsterActor*>(hHitter.GetPointer());
				if (pCannonMonster && pCannonMonster->GetMasterPlayerActor() && pCannonMonster->GetMasterPlayerActor()->IsPlayerActor())
				{
					CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(pCannonMonster->GetMasterPlayerActor().GetPointer());
					if( pPlayerActor )
						pPlayerActor->OnKillMonster(GetMySmartPtr());
				}
			}
		}
		else if (hHitter->IsPlayerActor())
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			if( pPlayerActor )
				pPlayerActor->OnKillMonster(GetMySmartPtr());
		}
	}	

	// 각 파티원들에게 몬스터가 죽었다고 처리해줌.
	// NOTE: 독걸리고 죽는 순간에 클라이언트를 끄니깐 서버가 죽는다. pTask가 쓰레기값으로 나옴..
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance(GetRoom()).GetTask( "GameTask" );

	if ( pTask )
	{
		for( DWORD i=0; i< pTask->GetRoom()->GetUserCount(); i++ ) 
		{
			CDNGameRoom::PartyStruct *pStruct = pTask->GetRoom()->GetPartyData(i);
			if (!pStruct) 
				continue;
			if( !IsMonsterActor() )
				continue;
			CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(this);
			if( pStruct->pSession && pStruct->pSession->GetQuest() )
				pStruct->pSession->GetQuest()->OnDieMonster(pMonster->GetMonsterClassID());
		}
	}
}



void CDnActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam, HitStruct *pHitStruct )
{
	//여기 들어 온 시점은 IsHitable에서 체크 되고 들어 왔음.
	//데미지 처리 전에 상태효과를 제거..
	RemoveStateEffectByHitSignal(pHitStruct);

	DnActorHandle hHitterActor = pHitter->GetActorHandle();

	if( hHitterActor && hHitterActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) == false )
	{
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
		hHitterActor->ApplyPrefixOffenceSkillToTarget_New(GetActorHandle());
#else
		hHitterActor->ApplyPrefixOffenceSkillToTarget(GetActorHandle());
#endif
	}

	m_HitParam = HitParam;
	DnActorHandle hHitter = pHitter->GetActorHandle();
	CDnStateBlow *pHitterStateBlow = hHitter->GetStateBlow();

#if defined(PRE_ADD_PROJECTILE_SE_INFO)	
	if( hHitter && HitParam.bFromProjectile && HitParam.hWeapon )
	{
		CDnStateBlow *pProjectileStateBlow = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() )->GetShooterStateBlow().get();
		if( pProjectileStateBlow )
			pHitterStateBlow = pProjectileStateBlow;
	}
#endif

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
	if( CDnDPSReporter::IsActive() && hHitter && hHitter->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( hHitter.GetPointer());

		if(CDnDPSReporter::GetInstance().IsEnabledUser(pPlayerActor->GetCharacterDBID()))
			CDnDPSReporter::GetInstance().StartReport();
	}
	float fDPS_DAMAGE = 0;
#endif

	CDnWeapon::HitTypeEnum HitType = CDnWeapon::Normal;
	// 명중률 계산
	SetDamageRemainTime( hHitter->GetUniqueID(), HitParam.RemainTime , hHitter->GetCurrentActionIndex() );
	SetLastDamageHitUniqueID( hHitter->GetUniqueID(), HitParam.iUniqueID );
	m_fStiffDelta = 0.f;
	if( m_fDownDelta > 0.f ) m_fLastDownRatio *= 0.5f;
	m_fDownDelta = 0.f;

	bool bAir = false;
	if( IsAir() && IsHit() ) bAir = true;

	bool bHitSuccess = CheckDamageAction( hHitter );


	// 물리공격으로 마법공격으로 변경.. [2011/02/21 semozz]
	if (hHitter && hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_180))
	{
		if (0 == m_HitParam.cAttackType)
		{
			//BLOW_180이 공격자에 적용되어있으면 모든 물리 공격을 마법 공격으로 변경한다.
			m_HitParam.cAttackType = 1;
		}
	}

	if( pHitterStateBlow && pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_182) )
	{
		//속성 공격 설정이 되저 있지 않을때만 변경??
		if( m_HitParam.HasElement == CDnActorState::ElementEnum_Amount ) 
		{
			DNVector(DnBlowHandle) vlChangeElementBlows;
			pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_182, vlChangeElementBlows );
			CDnState::ElementEnum eElementType = CDnState::ElementEnum_Amount;
			if (!vlChangeElementBlows.empty())
			{
				eElementType = CDnState::ElementEnum(int(vlChangeElementBlows.at(0)->GetFloatValue()));
			}

			if (eElementType != CDnActorState::ElementEnum_Amount)
				m_HitParam.HasElement = eElementType;
		}
	}

	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_149 ) == false  ) // 얼음감옥 상태효과 걸렸을 때는 OnDefenseAttack 을 받아서 처리하는 패링, 블록 상태효과 처리를 하지 않음.
	{
		CDnState* pAttackerState = NULL;
		if( hHitter && !HitParam.bFromProjectile )
		{ 
			pAttackerState = static_cast<CDnActorState *>(hHitter.GetPointer());
		}
		else
		{
			if( HitParam.hWeapon )
				pAttackerState = static_cast<CDnProjectile*>( HitParam.hWeapon.GetPointer() )->GetShooterStateSnapshot().get();

#if !defined(PRE_ADD_PROJECTILE_SE_INFO)
			if (hHitter&& hHitter->IsApplyPrefixOffenceSkill())
				pAttackerState = static_cast<CDnActorState *>(hHitter.GetPointer());
#endif

			if( hHitter && pAttackerState == NULL )
				pAttackerState = static_cast<CDnActorState *>(hHitter.GetPointer());
		}
		
#ifdef PRE_ADD_MONSTER_CATCH
		bool bCheckOnDefense = true;
		if( IsPlayerActor() )
		{
			// 몬스터에 잡힌 상태에서는 스킬 관련 내용들 무시.
			if( static_cast<CDnPlayerActor*>(this)->IsCatchedByMonster() || m_HitParam.bReleaseCatchActor )
				bCheckOnDefense = false;
		}

		// 때린 녀석인 hActor 에서 state 를 가져다 사용하면 스킬 액션이 이미 끝난 상태일 수 있기 때문에 평소 공격력/방어력일 수 있으므로 
		// 발사체에 실어서 보내는 것까지 고려된 pAttackerState 를 사용한다. 
		// pAttackerState 를 함수 내부에서 사용할 경우, 반드시 내부에서 NULL 포인터 체크를 해야 함.
		if( bCheckOnDefense )
			bHitSuccess = m_pStateBlow->OnDefenseAttack( hHitter, pAttackerState, m_HitParam, bHitSuccess );
#else
		bHitSuccess = m_pStateBlow->OnDefenseAttack( hHitter, pAttackerState, m_HitParam, bHitSuccess );
#endif 

	}

	// 데미지 반사 상태효과에 의해 hitter 가 몬스터인 경우 Die 처리되어 객체가 destroy 될 수 있다.
	// 플레이어인 경우 객체가 파괴되지 않으므로 CheckDamageVelocity() 함수가 호출되어 적절하게
	// 피격액션이 나올 수 있도록 처리한다.#23559
	// 몬스터한테 맞자마자 데미지 반사로 몬스터가 죽으면 속도 값이 정확히 셋팅 안되어 동일한 문제가 나올 수 있지만 
	// 우선 플레이어쪽만 처리해둔다.
	if( hHitter->IsMonsterActor() )
	{
		if( !hHitter || hHitter->IsDie() )
			return;
	}

	bool isBreakSuperArmorBlow = false;

	if( bHitSuccess ) {
		// 경직 시간 계산
		float fStiffResult = 0.f;
		if( GetStiffResistance() > 0 ) fStiffResult = ( hHitter->GetStiff() * m_HitParam.fStiffProb ) / GetStiffResistance();
		fStiffResult *= CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffRevision );

		if( fStiffResult < 0.001f )
			fStiffResult = 0.001f; // CPacketCompressStream::FLOAT_SHORT 을 사용하기때문에 범위를 벗어나는 작은 값이 되지않도록 설정한다.

		if( fStiffResult <= 0.f )
		{
			fStiffResult = 0.05f;
		}
		else
		{
			float fStiffMax = 0.f; 
			
#if defined PRE_ADD_PVP_STIFF_MAX
			if( GetGameRoom()->bIsPvPRoom() == true )
				fStiffMax = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffMax_PVP );
			else
				fStiffMax = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffMax );
#else
			fStiffMax = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffMax );
#endif
			if( fStiffResult > fStiffMax ) 
				fStiffResult = fStiffMax;
		}

		m_fStiffDelta = s_fMaxStiffTime * fStiffResult;

		// 다운딜레이 랜덤값 설정
		if( !IsDown() ) {
			m_fLastDownRatio = 1.f;
		}

		float fTemp = ( 3.f * ( 1.f - GetDownDelayProb() ) ) * m_fLastDownRatio;
		int nTemp = (int)( fTemp * DOWN_DELAY_RANDOM_RANGE * 100.f );
		if( nTemp == 0 ) m_fDownDelta = fTemp;
		else m_fDownDelta = fTemp - DOWN_DELAY_RANDOM_RANGE + ( _rand(GetRoom())%( nTemp * 2 ) / 100.f );
		if( m_fDownDelta >= 3.f ) m_fDownDelta = 3.f;
		if( m_fDownDelta <= 0.01f ) m_fDownDelta = 0.01f;

		// 크리티컬 계산
		float fCritical = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalValue );
		float fCriticalValue = hHitter->GetLevelWeightValue() * fCritical;
		float fCriticalProb = 0.f;
		if( fCriticalValue > 0.f )
			fCriticalProb = hHitter->GetCritical() / fCriticalValue;
		else
			g_Log.Log(LogType::_SESSIONCRASH, L"fCriticalValue = 0.f ClassID=%d\n", hHitter->GetClassID() );


		float fAddCiriticalValue = 0.0f;

		if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_250))
		{
			DNVector( DnBlowHandle ) vlhCriticalIncBlows;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_250, vlhCriticalIncBlows );
			int nListCount = (int)vlhCriticalIncBlows.size();
			for (int i = 0; i < nListCount; ++i)
			{
				DnBlowHandle hBlow = vlhCriticalIncBlows[i];
				if (hBlow)
					fAddCiriticalValue += hBlow->GetFloatValue();
			}
		}


		if( pHitterStateBlow )
		{
			if( pHitterStateBlow->IsApplied(STATE_BLOW::BLOW_251) )
			{
				DNVector(DnBlowHandle) vlhCriticalIncBlows;
				pHitterStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_251, vlhCriticalIncBlows );
				int nListCount = (int)vlhCriticalIncBlows.size();
				for (int i = 0; i < nListCount; ++i)
				{
					DnBlowHandle hBlow = vlhCriticalIncBlows[i];
					if (hBlow)
						fAddCiriticalValue += hBlow->GetFloatValue();
				}
			}
		}

		fCriticalProb += fAddCiriticalValue;

		if( fCriticalProb > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalMax ) )
			fCriticalProb = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalMax );

		bool bBreakSuperAmmor = false;
		//슈퍼아머브레이크 상태효과 걸려 있고, 확률이 맞으면 크리티컬로 변경.( 면역이 아니어야 한다..)
		if (hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_162) && !IsImmuned(STATE_BLOW::BLOW_162))
		{
			DNVector( DnBlowHandle ) vlhBreakSuperArmors;
			hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_162, vlhBreakSuperArmors );

			float fRate = 0.0f;
			DnSkillHandle hSkill;

			if (!vlhBreakSuperArmors.empty())
			{
				const CDnSkill::SkillInfo* pSkillInfo = vlhBreakSuperArmors.at(0)->GetParentSkillInfo();
				if (pSkillInfo)
					hSkill = hHitter->FindSkill(pSkillInfo->iSkillID);

				if (hSkill && CDnSkill::UsingResult::Success == hSkill->CanExecute())
				{
					fRate = vlhBreakSuperArmors.at(0)->GetFloatValue();
				}
			}

			bBreakSuperAmmor = rand() % 10000 <= (fRate * 10000.0f);

			if (bBreakSuperAmmor)
			{
				fCriticalProb = 1.0f;
				m_HitParam.bBreakSuperAmmor = true;
				
				if (hSkill && CDnSkill::Passive == hSkill->GetSkillType())
				{
					// PassiveSkill 쿨타임 표시를 위한 패킷 전송...
					BYTE pBuffer[128];
					CPacketCompressStream Stream( pBuffer, 128 );

					DWORD dwUniqueID = hHitter ? hHitter->GetUniqueID() : -1;
					int nSkillID = hSkill->GetClassID();

					Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
					Stream.Write( &nSkillID, sizeof(nSkillID));

					hHitter->Send(eActor::SC_PASSIVESKILL_COOLTIME, &Stream);

					//실제 쿨타임 시작.
					hSkill->OnBeginCoolTime();
				}

				//슈퍼아머브레이크 이펙트 표시 패킷 전송..
				{
					//이펙트 표시용 패킷을 전송..
					BYTE pBuffer[128];
					CPacketCompressStream Stream( pBuffer, 128 );

					DWORD dwUniqueID = hHitter ? hHitter->GetUniqueID() : -1;
					STATE_BLOW::emBLOW_INDEX blowIndex = STATE_BLOW::BLOW_162;
					bool bShowEffect = true;
					Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
					Stream.Write( &blowIndex, sizeof(blowIndex));
					Stream.Write( &bShowEffect, sizeof(bShowEffect));

					hHitter->Send(eActor::SC_SHOW_STATE_EFFECT, &Stream);
				}

			}
		}
		//162번 상태효과에 의해 슈퍼 아머가 깨졌는지 확인
		if (bBreakSuperAmmor)
			isBreakSuperArmorBlow = true;
	
		//79번 상태효과가 걸려 있으면 무조건 슈퍼 아머 깨짐..
		if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_079))
			isBreakSuperArmorBlow = true;
		
		EtVector3 vVelocity = m_HitParam.vVelocity;

		// 스턴 계산
		int nStunProb = (int)( CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunMax ) * 100.f );
		if( GetStunResistance() > 0 ) {
			float fLocalTemp = powf( (float)( hHitter->GetStun() / GetStunResistance() ), CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunRevisioin ) ) * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunRevision2 );
			if( fLocalTemp > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunMax ) )
				fLocalTemp = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunMax );
			nStunProb = (int)( fLocalTemp * 100.f );
		}

		bool isAppliedForceSturn = false;

		// 타격자가 강제 스턴 상태효과가 있는 상황이라면 해당 상태효과 수치로 있는 확률 값 (0.0~1.0)을 더한다.
		if( hHitter && hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_133 ) )
		{
			DNVector( DnBlowHandle ) vlhStunBlow;
			hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_133, vlhStunBlow );
			for( int i = 0; i < (int)vlhStunBlow.size(); ++i )
				nStunProb += int(vlhStunBlow.at(i)->GetFloatValue() * 100.0f);

			isAppliedForceSturn = true;
		}

//		if( hHitter->GetStun() > 0 ) nStunProb = (int)( ( ( 1.f - ( hHitter->GetStun() / GetStunResistance() ) ) * 100 ) * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunRevisioin ) );
		nStunProb = (int)( nStunProb * m_HitParam.fDamage );
		if( _rand(GetRoom())%100 < nStunProb ) {
			if( IsStandHit( m_HitParam.szActionName.c_str() ) ) {
				m_HitParam.szActionName = "Stun";
			}
			else if( IsAir( m_HitParam.szActionName.c_str() ) ) {
				if( strstr( m_HitParam.szActionName.c_str(), "High" ) == NULL ) {
					m_HitParam.szActionName = "Hit_AirUp";
				}
				vVelocity = m_HitParam.vVelocity * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StunVelocityRevision ) * ( 1.f + pHitStruct->fStunVelocityRevision );
			}
			else if( IsDown( m_HitParam.szActionName.c_str() ) ) {
				m_HitParam.szActionName = "Hit_AirUp";
			}
			
			
			//스턴이 되더라도 크리티컬이 발생되어야 함.
			//그래서 STATE_BLOW::BLOW_133 상태효과가 다면 Stun으로 무조건 설정하고
			//STATE_BLOW::BLOW_133상태효과가 있고, hitType이 설정되어 있으면 hitType을 Sturn으로 바꾸지 말고 유지
			if (!isAppliedForceSturn)
				HitType = CDnWeapon::Stun;
			else if (HitType == CDnWeapon::Normal)
				HitType = CDnWeapon::Stun;

			m_fStiffDelta = 0.f;
		}

		if ( m_HitParam.HasElement == CDnActorState::Fire && IsAppliedThisStateBlow(STATE_BLOW::BLOW_042)) // 히트 속성이 화염이며 , 화상에 걸려있는 상태이라면
		{
			if (hHitter && hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_222)) // 때린녀석이 화염속성 크리 상태효과를 들고있다면.
			{
				DNVector( DnBlowHandle ) vBurnCriticalBlows;
				hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_222, vBurnCriticalBlows );

				if (!vBurnCriticalBlows.empty())
				{
					if(vBurnCriticalBlows.at(0))
					{
						CDnBurnCriticalBlow *pBurnCriticalBlow = static_cast<CDnBurnCriticalBlow*>(vBurnCriticalBlows.at(0).GetPointer());

						if(pBurnCriticalBlow->GetMultiplyRatio() > 0)
							fCriticalProb = fCriticalProb + ( fCriticalProb * ( (float)pBurnCriticalBlow->GetMultiplyRatio() * 1/100 ) );

						if(pBurnCriticalBlow->GetAddValueRatio() > 0)
							fCriticalProb = fCriticalProb + (pBurnCriticalBlow->GetAddValueRatio() / fCriticalValue);


						if( fCriticalProb > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalMax ) )
							fCriticalProb = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalMax );
					}
				}
			}
		}

		//스턴확률 계산 이후 크리티컬 확률 계산으로 변경.
		if( _rand(GetRoom())%100 < (int)( fCriticalProb * 100.f ) ) {
			vVelocity = m_HitParam.vVelocity * CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CriticalVelocityRevision ) * ( 1.f + pHitStruct->fCriticalVelocityRevision );
			HitType = CDnWeapon::Critical;

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
			//크리티컬인 경우 통합레벨스킬의 상태효과 
			if (hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_267))
			{
				DNVector( DnBlowHandle ) vlhBlows;
				hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_267, vlhBlows );
				
				int nCount = (int)vlhBlows.size();
				for( int i = 0; i < (int)vlhBlows.size(); ++i )
				{
					CDnAddCriticalRateBlow* pBlow = static_cast<CDnAddCriticalRateBlow*>(vlhBlows[i].GetPointer());
					if (pBlow && pBlow->IsActivated() == false && pBlow->GetLeftCoolTime() == 0.0f)
						pBlow->ApplyCriticalIncBlow();
				}
			}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

			//rlkt_mastery!
			//printf("%s, OnCriticalHit Passive %d \n ", __FUNCTION__, 7529);
			//mastery 1 dark avenger 
		/*	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			boost::shared_ptr<IDnObserverNotifyEvent> pEvent(IDnObserverNotifyEvent::Create(EVENT_ONCRITICALHIT));
			pEvent->SetSkillID(hHitter->GetProcessSkill()->GetClassID());
			pPlayer->Notify(pEvent);*/
			CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			if (pPlayer && pPlayer->IsPlayerActor()) {
				
				printf("%s Notify EVENT_ONCRITICALHIT\n",__FUNCTION__);

				boost::shared_ptr<IDnObserverNotifyEvent> pEvent(IDnObserverNotifyEvent::Create(EVENT_ONCRITICALHIT));
				pEvent->SetSkillID(pPlayer->GetLastUsedSkill());
				pPlayer->Notify(pEvent);

				//boost::shared_ptr<IDnObserverNotifyEvent> pEvent(IDnObserverNotifyEvent::Create(EVENT_ONCRITICALHIT));
				////pEvent->SetSkillID(pPlayer->GetLastUsedSkill());
				//pPlayer->Notify(pEvent);
			}
		/*	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hHitter.GetPointer());
			shared_ptr<IDnObserverNotifyEvent> pNotifyEvent(new CDnOnCriticalHitMessage);
			shared_ptr<CDnOnCriticalHitMessage> pOnCriticalEvent = shared_polymorphic_downcast<CDnOnCriticalHitMessage>(pNotifyEvent);
			pOnCriticalEvent->SetSkillID(hHitter->GetProcessSkill()->GetClassID());
			pPlayer->Notify(pNotifyEvent);*/

		}

		// 에어 콤보 계산
		if( IsAir() && IsHit() ) {
			m_nAirComboCount++;
			hHitter->OnAirCombo( m_nAirComboCount );
		}
		else m_nAirComboCount = 0;

		m_HitParam.vVelocity = vVelocity;
	}
	else HitType = CDnWeapon::Defense;

	m_HitParam.HitType = HitType;
	// 데미지 계산
	switch( HitType ) {
		case CDnWeapon::Normal:
		case CDnWeapon::Critical:
		case CDnWeapon::Stun:
		case CDnWeapon::CriticalRes:
			{
				// 무게에 따라 가속도 줄여준다.
				if( GetWeight() > 0.f )
					m_HitParam.vVelocity.y /= GetWeight();
				if( GetWeight() > 1.f ) {
					float fTemp = m_HitParam.vVelocity.z;
					float fTest = CalcMovement( fTemp, 1.f, FLT_MAX, FLT_MIN, m_HitParam.vResistance.z );
					if( m_HitParam.vVelocity.z * fTest > 0.f ) {
						m_HitParam.vVelocity.x /= powf( 1.1f, GetWeight() );
						m_HitParam.vVelocity.z /= powf( 1.1f, GetWeight() );
					}
				}

				// 대미지 상관관계 체크해요
				int nDamageType = m_HitParam.nDamageType;
				
				// #13885 이슈 관련.
				if( hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_086 ) )
				{
					// 팀 반전 상태효과 하나만 적용된다.
					DNVector( DnBlowHandle ) vlReverseTeamBlows;
					hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_086, vlReverseTeamBlows );
					_ASSERT( false == vlReverseTeamBlows.empty() );
					if( false == vlReverseTeamBlows.empty() )
					{
						DnBlowHandle hBlow = vlReverseTeamBlows.front();
						CDnReverseTeamBlow* pReverseTeamBlow = static_cast<CDnReverseTeamBlow*>(hBlow.GetPointer());
						if( CDnReverseTeamBlow::HIT_SIGNAL_TARGET_CHANGE == pReverseTeamBlow->GetType() )
						{
							switch( nDamageType )
							{
								case 0: // Enemy -> Friend
									nDamageType = 1;
									break;

								case 1: // Friend -> Enemy
									nDamageType = 0; // #50166 이슈관련 Hittalbe에서만 처리한다.
									break;

								case 2: // All -> All
									break;
							}
						}
						else 
						if( CDnReverseTeamBlow::HIT_SIGNAL_TARGET_ALL == pReverseTeamBlow->GetType() )
						{
							// 타격대상 all 로 바꿔줌.
							nDamageType = 2;
						}
					}
				}

				//rlkt_damage
				bool bCalcDamage = true;
				float fDamage = 0.f;
				switch( nDamageType )
				{
					case 0:	// Enemy
						if( GetTeam() == hHitter->GetTeam() ) bCalcDamage = false;
						break;
					case 1: // Friend
						if( GetTeam() != hHitter->GetTeam() ) bCalcDamage = false;
						break;
					case 2: // All
						break;
				}

				if( bCalcDamage ) 
				{
					fDamage = CalcDamage( hHitter, m_HitParam );
#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
					fDPS_DAMAGE = fDamage;
#endif
				}

				// 죽었나 체크
				if( GetHP() <= 0.f )
					Die( hHitter );

				if (fDamage > 0.0f &&	//데미지가 있고
					GetHP() > 0.0f &&	//사망이 아니고
					!IsAppliedThisStateBlow(STATE_BLOW::BLOW_183)) //접두어 상태효과 무시(구울모드) 아닌경우)
				{
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
					ProcessPrefixDefenceSkill_New(hHitterActor);
#else				
					ProcessPrefixDefenceSkill(hHitterActor);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
				}

				bool bSuccessNormalDamage = true;
				int nSuperAmmorTime = 0;

				if( !IsDie() ) {
					// Skill Super Ammor Check
//					if( m_HitParam.nSkillSuperAmmorIndex == -1 || ( m_HitParam.nSkillSuperAmmorIndex != -1 && m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] > 0 ) {
					bool bApplySuperAmmor = false;
					if( m_HitParam.nSkillSuperAmmorIndex != -1 && m_HitParam.nSkillSuperAmmorIndex < 0)
						break;
					if( m_HitParam.nSkillSuperAmmorIndex == -1 ) {
						bApplySuperAmmor = true;
					}
					else if( m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] > 0 ) {
						int nAmmorDamage = m_HitParam.nSkillSuperAmmorDamage;// * (( HitType == CDnWeapon::Critical ) ? 2 : 1);

						// 슈퍼 아머 보호 상태 효과가 적용 되어 있으면 슈퍼 아머 계산 안함.[2010/12/28 semozz]
						bool isApplySuperArmorShield = false;

						isApplySuperArmorShield = this->IsAppliedThisStateBlow(STATE_BLOW::BLOW_156);

						// [2011/02/09 semozz]
						// 가디언 포스의 Shield상태효과가 적용 되어 있을때도 슈퍼아머는 깨지지 않는다.
						// 슈퍼아머 보호관련이 우선순위가 가장 높다.
						if (!isApplySuperArmorShield)
							isApplySuperArmorShield = this->IsAppliedThisStateBlow(STATE_BLOW::BLOW_055);

						//슈퍼아머 브레이크 상태효과 적용되어 있으면 무조건 슈퍼아머 0으로
						if (isBreakSuperArmorBlow)
						{
							m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] = 0;
						}
						else if (!isApplySuperArmorShield)						
						{
							//////////////////////////////////////////////////////////////////////////
							// 142번 슈퍼아머 공격력 비율 상태효과
							if( hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_142 ) )
							{
								DNVector(DnBlowHandle) vlBlows;
								float fValue = 1.0f;
								hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_142, vlBlows );
								for( int i = 0; i < (int)vlBlows.size(); ++i )
								{
									DnBlowHandle hBlow = vlBlows.at( i );
									fValue += (hBlow->GetFloatValue() - 1.0f);
								}
								nAmmorDamage = int((float)nAmmorDamage * fValue);
							}
							//////////////////////////////////////////////////////////////////////////

							m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] -= nAmmorDamage;
						}

						if( m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] <= 0 ) {
							m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] = 0;
							hHitter->OnBreakSkillSuperAmmor( m_HitParam.nSkillSuperAmmorIndex, m_nLastUpdateSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex], nAmmorDamage );
							OnBreakSkillSuperAmmor( m_HitParam );

						}
						else bApplySuperAmmor = true;
					}
					if( bApplySuperAmmor ) {
						bSuccessNormalDamage = false;
						nSuperAmmorTime = m_nSkillSuperAmmorTime;
						// 대미지 감쇠시켜준다.
						SetHP( GetHP() + (INT64)( fDamage * m_fSkillSuperAmmorDamageDecreaseProb ) );
#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
						fDPS_DAMAGE += (int)( fDamage * m_fSkillSuperAmmorDamageDecreaseProb );
#endif
					}

					// Normal Super Ammor Check
					if( m_bEnableNormalSuperAmmor == true ) {
						if( m_HitParam.fDurability >= m_fBreakNormalSuperAmmorDurability )
							m_bEnableNormalSuperAmmor = false;
						else {
							bSuccessNormalDamage = false;
							nSuperAmmorTime = m_nNormalSuperAmmorTime;
						}
					}
					if( pHitStruct->nSuperAmmorTime > 0 ) nSuperAmmorTime = pHitStruct->nSuperAmmorTime;
				}

				bool bSuperAmmorReset = true;

				//////////////////////////////////////////////////////////////////////////
				// 아카데믹 포스 아웃 스킬 처리 [2011/07/04 semozz]
				// 무게값 제한 값이 있고, 설정된 무게값 보다 액터의 무게가 적다면 슈퍼아머 상관 없이 물리값 적용
				if( 0 < m_HitParam.nWeightValueLimit )
				{
					if( (float)m_HitParam.nWeightValueLimit >= GetWeight() )
					{
						//물리값 적용을 위해서 bSuccessNormalDamage는 true로 변경하고.
						bSuccessNormalDamage = true;
						//슈퍼아머 값 리셋은 안되도록한다..
						bSuperAmmorReset = false;
						//슈퍼아머가 깨지면(0) hit동작을 하고, 슈퍼아머가 깨지지 않으면 Hit동작을 제거 한다.
						//#39137 몬스터가 죽은 경우는 스킵
						if (!IsDie() && m_HitParam.nSkillSuperAmmorIndex>=0 && m_nSkillSuperAmmorValue[m_HitParam.nSkillSuperAmmorIndex] > 0)
							m_HitParam.szActionName.clear();
					}                            
				}
				//////////////////////////////////////////////////////////////////////////

				//이 상태효과가 걸려 있는 상태에서 피격 동작을 하면 Escape에서 설정된 피격 동작이 중간에 취소됨.
				if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_218))
				{
#if defined(PRE_FIX_50007)
					//#50007 Escape중에도 죽는 동작으로는 변경이 되어야 함.(Die동작이 아닌 경우만 Hit동작 제거)
					if (strstr(m_HitParam.szActionName.c_str(), "Die") == NULL)
#endif // PRE_FIX_50007
						m_HitParam.szActionName.clear();
				}

				m_HitParam.bSuccessNormalDamage = bSuccessNormalDamage;
				m_HitParam.nSuperAmmorDelay = nSuperAmmorTime;

				if( bSuccessNormalDamage ) {
					m_nNormalSuperAmmorTime = 0;

					if (bSuperAmmorReset)
						memset( m_nSkillSuperAmmorValue, 0, sizeof(m_nSkillSuperAmmorValue) );

					CheckDamageVelocity( hHitter );

					// 맞았을때 리셋할것들 리셋
					MAMovementBase *pMovement = GetMovement();
					if( pMovement ) {
						pMovement->ResetMove();
						pMovement->ResetLook();
					}

					if( !m_HitParam.szActionName.empty() ) 
					{
#ifdef PRE_FIX_81750
						CDnChangeStandActionBlow::ReleaseStandChangeSkill( GetActorHandle(), true );
#endif
						SetActionQueue( m_HitParam.szActionName.c_str(), 0, 3.f, 0.f, false );
					}
				}
				else {
					int nDelay = (int)( nSuperAmmorTime * ( m_fStiffDelta / s_fMaxStiffTime ) );
					if( nDelay > 0 ) {
						SetPlaySpeed( (DWORD)( nSuperAmmorTime * ( m_fStiffDelta / s_fMaxStiffTime ) ), 0.03f );
						if( m_HitParam.hWeapon && m_HitParam.hWeapon->GetEquipType() != CDnWeapon::Arrow ) 
						{
#ifdef PRE_FIX_HITSTIFF
							// 프레임 변경 상태효과가 우선 처리대상이므로 상태효과 적용중이라면 SetPlaySpeed() 함수 무시.
							if( false == (0 < hHitter->GetFrameStopRefCount() || hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 )) )
							{
								hHitter->SetPlaySpeed( 100, 0.03f );
							}
#else
							hHitter->SetPlaySpeed( 100, 0.03f );
#endif // #ifdef PRE_FIX_HITSTIFF
								
						}
					}
					m_HitParam.szActionName.clear();
				}
			}
			break;
		case CDnWeapon::Defense: 
			m_HitParam.vVelocity *= 0.5f;
			m_HitParam.vVelocity.y = 0.f;
			if( m_HitParam.hWeapon && m_HitParam.hWeapon->GetEquipType() != CDnWeapon::Arrow ) 
			{
//#ifdef PRE_FIX_HITSTIFF
				// 프레임 변경 상태효과가 우선 처리대상이므로 상태효과 적용중이라면 SetPlaySpeed() 함수 무시.
				if( false == (0 < hHitter->GetFrameStopRefCount() || hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 )) )
				{
					hHitter->SetPlaySpeed( 100, 0.03f );
				}
//#else
//				hHitter->SetPlaySpeed( 100, 0.03f );
//#endif // #ifdef PRE_FIX_HITSTIFF
			}

			CheckDamageVelocity( hHitter );
			// 에뉘 시키구
			if( !m_HitParam.szActionName.empty() )
				SetActionQueue( m_HitParam.szActionName.c_str(), 0, 3.f, 0.f, false );
			break;
	}

//	OutputDebug( "데미지 : %s, %.2f, %.2f ( %.2f, %.2f )\n", m_HitParam.szActionName.c_str(), m_HitParam.vVelocity.y, m_HitParam.vResistance.y, GetVelocity() ? GetVelocity()->y :0.f, GetResistance() ? GetResistance()->y : 0.f);

	// 무기 소리, 파티클 등을 처리하기 위해 콜해준다.
	if( m_HitParam.hWeapon ) {
		m_HitParam.hWeapon->OnHitSuccess( GetMySmartPtr(), HitType, m_HitParam.vPosition, m_HitParam.bFirstHit );
		if( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) {
			CDnProjectile *pProjectile = static_cast<CDnProjectile *>(HitParam.hWeapon.GetPointer());
			if( pProjectile ) pProjectile->OnDamageSuccess( GetMySmartPtr(), m_HitParam );
			else HitParam.hWeapon->SetDestroy();
		}
	}


	// 스킬 시전중 피격시 스킬 취소. 
	if( false == m_HitParam.szActionName.empty() )
	{
		// 스킬 액션 중 피격시 스킬 취소에 대한 것은 IsProcessSkill() 로 체크하지 않고 객체의 존재 유무만 체크한다.
		// IsFinished() 로 내부에서 체크하면 PlayAniProcess 내부에서 GetCurrentAction() 이 액션 큐 값을 우선해서 
		// 얻어오기 때문에 위에서 피격액션이 SetActionQueue로 셋팅되면 스킬이 끝난 것으로 판단되어 아래 구문이 실행되지 않는다.
		if( m_hProcessSkill )
		{
			// 오라 스킬 시전 중.. 그러니까 상태효과 다 들어가기 이전에 타격되었다면 오라 스킬이 온전히 사용된 것이 아님
			if( IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn() )
			{
				//#41352 오라 스킬 취소될때 클라이언트로 취소 패킷을 전송해야함..
				CmdFinishAuraSkill(m_hAuraSkill->GetClassID());

				OnSkillAura( m_hAuraSkill, false );
				m_hProcessSkill.Identity();

				OutputDebug( "### 오라스킬 피격당해서 캔슬됨!!\n" );
			}
			else
			{
				m_hCanceledSkill = m_hProcessSkill;
				m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
				m_hProcessSkill.Identity();
			}
		}
	}

	// 현재 오토 패시브 스킬은 피격되었을 때 밖에 발동될 때가 없음.
	UseAutoPassiveSkill( 0, 0.0f );

	if( hHitter ) hHitter->OnHitSuccess( CDnActionBase::m_LocalTime, GetMySmartPtr(), pHitStruct );

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
	if( CDnDPSReporter::IsActive() && hHitter && hHitter->IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( hHitter.GetPointer());
		if(CDnDPSReporter::GetInstance().IsEnabledUser(pPlayerActor->GetCharacterDBID()))
		{
			DNVector(DnActorHandle) hVecList;
			ScanActor( GetRoom(), *hHitter->GetPosition() , 500.f , hVecList );

			CDnDPSReporter::GetInstance().ApplyAreaMonsterCount( (int)hVecList.size() );
			CDnDPSReporter::GetInstance().ApplyBaseData( &m_HitParam , fDPS_DAMAGE );
			CDnDPSReporter::GetInstance().ApplyTargetData(GetActorHandle());
			CDnDPSReporter::GetInstance().EndReport();
		}
	}
#endif
}

void CDnActor::RequestDamage( CDnDamageBase *pHitter, int nSeed, INT64 nDamage )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	char cHitterType = pHitter->GetDamageObjectType();
	DWORD dwUniqueID = pHitter->GetDamageObjectUniqueID();
	ASSERT( dwUniqueID != -1 && "잘못된 DamageBase 아이디" );
	bool bSendWeaponSerialID = false;
	DWORD dwWeaponUniqueID = -1;
	INT64 nWeaponSerialID = -1;

	int nActionIndex = GetElementIndex( m_HitParam.szActionName.c_str() );
	if( m_HitParam.hWeapon ) {
		if( m_HitParam.hWeapon->GetSerialID() == -1 ) 
			dwWeaponUniqueID = m_HitParam.hWeapon->GetUniqueID();
		else {
			bSendWeaponSerialID = true;
			nWeaponSerialID = m_HitParam.hWeapon->GetSerialID();
		}
	}
	INT64 nCurrentHP = GetHP();

	Stream.Write( &nSeed, sizeof(int) );
	Stream.Write( &cHitterType, sizeof(char) );
	Stream.Write( &dwUniqueID, sizeof(DWORD) );
	Stream.Write( &bSendWeaponSerialID, sizeof(bool) );
	if( bSendWeaponSerialID ) Stream.Write( &nWeaponSerialID, sizeof(INT64) );
	else Stream.Write( &dwWeaponUniqueID, sizeof(DWORD) );
	Stream.Write( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &nDamage, sizeof(INT64) );
	Stream.Write( &nCurrentHP, sizeof(INT64) ); // 일단 넣어놉니다.
	Stream.Write( &m_HitParam.HitType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( ( GetVelocity() ) ? GetVelocity() : &EtVector3( 0.f, 0.f, 0.f ), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( ( GetResistance() ) ? GetResistance() : &EtVector3( 0.f, 0.f, 0.f ), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &m_HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( &EtVec3toVec2(m_HitParam.vViewVec), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
	Stream.Write( &m_HitParam.bFirstHit, sizeof(bool) );
	//Stream.Write( &m_HitParam.nBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &m_fStiffDelta, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10000.f );
	Stream.Write( &m_fDownDelta, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1000.f );
	Stream.Write( &m_HitParam.nSuperAmmorDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	Stream.Write( &m_HitParam.bSuccessNormalDamage, sizeof(bool) );
	Stream.Write( &m_HitParam.DistanceType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

	// #32722 클라이언트에서 스킬 사용한다는 패킷이 도착하기 전에 서버에서 피격될 경우 클라이언트만 스킬 액션이 캔슬되는 것 수정.
	int iCanceledSkillID = 0;
	if( IsPlayerActor() )
	{
		if( m_hCanceledSkill )
		{
			iCanceledSkillID = m_hCanceledSkill->GetClassID();
			m_hCanceledSkill.Identity();
		}
		else
		if( IsDie() )	// 스킬 쓰다가 죽은 경우엔 CanceledSkillID 를 -1 로 보내줘서 클라쪽에서 알 수 있도록 한다.
		{
			iCanceledSkillID = -1;
		}
	}
	Stream.Write( &iCanceledSkillID, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	//////////////////////////////////////////////////////////////////////////

	// 임시
	// ToDo : 준영씨께서 나중에 봐주셔야 함. 임시로 안죽게만 막아둠.
	int nSuperArmorValue = 0;
	// 플레이어의 슈퍼아머가 33000 까지 되는 경우가 있어 압축하지 않고 서버에서 보냅니다. (#10065)
	Stream.Write( (m_HitParam.nSkillSuperAmmorIndex >= 0 && m_HitParam.nSkillSuperAmmorIndex <= 3) ? &m_nSkillSuperAmmorValue[ m_HitParam.nSkillSuperAmmorIndex ] : &nSuperArmorValue, sizeof(int) );
	Stream.Write( (m_HitParam.nSkillSuperAmmorIndex >= 0 && m_HitParam.nSkillSuperAmmorIndex <= 3) ? &m_nLastUpdateSkillSuperAmmorValue[ m_HitParam.nSkillSuperAmmorIndex ] : &nSuperArmorValue, sizeof(int) );
	//Stream.Write( (m_HitParam.nSkillSuperAmmorIndex >= 0 && m_HitParam.nSkillSuperAmmorIndex <= 3) ? &m_nSkillSuperAmmorValue[ m_HitParam.nSkillSuperAmmorIndex ] : &nSuperArmorValue, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
	//Stream.Write( (m_HitParam.nSkillSuperAmmorIndex >= 0 && m_HitParam.nSkillSuperAmmorIndex <= 3) ? &m_nLastUpdateSkillSuperAmmorValue[ m_HitParam.nSkillSuperAmmorIndex ] : &nSuperArmorValue, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

	// Combo 싱쿠

	Stream.Write( &m_HitParam.HasElement, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

#if defined( PRE_ADD_LOTUSGOLEM )
	Stream.Write( &m_HitParam.bIgnoreShowDamage, sizeof(bool) );
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	Stream.Write( &m_HitParam.bStageLimit, sizeof(bool) );
#endif

	switch( pHitter->GetDamageObjectType() ) {
		case DamageObjectTypeEnum::Actor:
			{
				DnActorHandle hActor = pHitter->GetActorHandle();
				if( !hActor ) break;
				if( dwUniqueID >= 100000 ) {
					CDnPlayerActor *pPlayer = ( hActor->IsPlayerActor() ) ? static_cast<CDnPlayerActor*>(hActor.GetPointer()) : NULL;
					if( !pPlayer ) break;
					int nComboCount = pPlayer->GetComboCount();
					int nComboDelay = pPlayer->GetComboDelay();
					Stream.Write( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
					Stream.Write( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				}
				// (#11415) 이슈 관련, 소환된 렐릭이 공격하는 것도 콤보 카운트에 포함시킨다. 
				// 직접 플레이어에게서 소환된 몬스터 타입인지 구분해서 맞다면 패킷에 실어서 보낸다.
				// 클라쪽에서도 마찬가지로 판단.
				else if( hActor->IsMonsterActor() )
				{
					CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
				
					// 렐릭(소환몹)이 hit 했을 경우 소환한 플레이어에게 콤보 업데이트 해준다. (#11415)
					DnActorHandle hSummonMasterPlayer = pMonsterActor->GetSummonerPlayerActor();
					if( hSummonMasterPlayer && hSummonMasterPlayer->IsPlayerActor() )
					{
						CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hSummonMasterPlayer.GetPointer());
						int nComboCount = pPlayerActor->GetComboCount();
						int nComboDelay = pPlayerActor->GetComboDelay();
						Stream.Write( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
						Stream.Write( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
					}
					else
					if( CDnActorState::Cannon == pMonsterActor->GetActorType() )
					{
						// 대포 몬스터가 hit 했을 경우 마스터 액터의 콤보 카운트를 업데이트 해준다. (#25778)
						CDnCannonMonsterActor* pCannonMonster = static_cast<CDnCannonMonsterActor*>( pMonsterActor );
						DnActorHandle hPlayerActor = pCannonMonster->GetMasterPlayerActor();

						int nComboCount = 0;
						int nComboDelay = 0;
						if( hPlayerActor && hPlayerActor->IsPlayerActor() )
						{
							CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hPlayerActor.GetPointer());
							int nComboCount = pPlayerActor->GetComboCount();
							int nComboDelay = pPlayerActor->GetComboDelay();
						}
						Stream.Write( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
						Stream.Write( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
					}
				}
			}
			break;
		case DamageObjectTypeEnum::Prop:
			break;
	}
	OnDamageWriteAdditionalPacket( &Stream );

	Send( eActor::SC_ONDAMAGE, &Stream );

	// #31022 차져 상태효과에서 돌린 액션의 hit 시그널로 맞은 경우 마지막 데미지로 기록하지 않는다. 
	// 화염 상태효과에서 퍼가서 쓰는데 영향을 주지 않기 위해..
	DnActorHandle hHitter = pHitter->GetActorHandle();
	if( hHitter && false == hHitter->IsOnSignalFromChargerSE() )
		m_iLastDamage = nDamage;
}


void CDnActor::OnRequestHPMPDelta( /*IN OUT*/ INT64& nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta /* = false */ )
{
	if( false == bIsMPDelta && 0 < nHPMPDelta )
	{
		// 141번 치료비율변경 상태효과.
		if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_141 ) )
		{
			DNVector(DnBlowHandle) vlBlows;
			float fValue = 1.0f;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_141, vlBlows );
			for( int i = 0; i < (int)vlBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlBlows.at( i );
				fValue += (hBlow->GetFloatValue() - 1.0f);
			}

			nHPMPDelta = INT64((float)nHPMPDelta * fValue);
		}
	}
}


// 서버쪽 HP/MP 는 바뀌지 않으므로 반드시 따로 서버의 HP도 조절해줄 것!
#ifdef PRE_FIX_77172
void CDnActor::RequestHPMPDelta( ElementEnum eElement, INT64 nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta /*= false */, 
								bool bShowValue, CDnDamageBase::SHitParam* pHitParam/* = NULL*/, bool bKeepJumpMovement /*=false*/ )
#else // PRE_FIX_77172
void CDnActor::RequestHPMPDelta( ElementEnum eElement, INT64 nHPMPDelta, const DWORD dwHitterUniqueID, bool bIsMPDelta /*= false */, bool bShowValue, CDnDamageBase::SHitParam* pHitParam/* = NULL*/ )
#endif // PRE_FIX_77172
{
	OnRequestHPMPDelta( nHPMPDelta, dwHitterUniqueID, bIsMPDelta );

#if defined(PRE_FIX_59680)
	//서먼 퍼펫의 TransmitDamageBlow(51)에 의해서 정상적인 Hit시그널 처리가 안되는 경우 RecoverHPByAttack(248) 상태효과가 정상 처리되지 않는 현상 발생됨.
	//51번 상태효과에서 주인 액터로 데미지 처리를 이쪽 함수 호출로 하는데, 여기서 HitParam값에 데미지를 설정 해 놓아야 248번 상태효과에서 정상적으로
	//HP회복을 할 수 있다.(248번 상태효과에서 TargetActor가 SummonMonster일 경우 주인 액터의 HitParam 정보를 얻어 와야 함.)
	if (bIsMPDelta == false)
	{
		m_HitParam.nCalcDamage = -nHPMPDelta;
	}
#endif // PRE_FIX_59680

	BYTE pBuffer[ 32 ];
	CPacketCompressStream Stream( pBuffer, 32 );

	Stream.Write( &eElement, sizeof(ElementEnum), CPacketCompressStream::INTEGER_CHAR );
	Stream.Write( &nHPMPDelta, sizeof(INT64) );
	Stream.Write( &dwHitterUniqueID, sizeof(DWORD) );
	Stream.Write( &bIsMPDelta, sizeof(bool) );
	Stream.Write( &bShowValue, sizeof(bool) );

#if defined(PRE_FIX_59308)
	int nHitType = pHitParam ? pHitParam->HitType : CDnWeapon::Normal;
	Stream.Write( &nHitType, sizeof(int) );
#endif // PRE_FIX_59308
#ifdef PRE_FIX_77172
	Stream.Write( &bKeepJumpMovement, sizeof(bool) );
#endif // PRE_FIX_77172

	Send( eActor::SC_SETHPMP_DELTA, &Stream );
}


// 확률 발동으로 어떤 일을 수행하는 상태효과가 서버로부터 발동되었다고 클라한테 알려줌. 현재 서버에서 클라로 쏘기만 할때 사용됨.
// 현재 페이백 마나에서 사용중.
void CDnActor::RequestSEProbSuccess( int iSkillID, STATE_BLOW::emBLOW_INDEX eBlowIndex )
{
	BYTE pBuffer[ 16 ];
	CPacketCompressStream Stream( pBuffer, 16 );

	Stream.Write( &iSkillID, sizeof(int) );
	Stream.Write( &eBlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );

	Send( eActor::SC_STATEEFFECT_PROB_SUCCESS, &Stream );
}

void CDnActor::RequestCooltimeReset( int iSkillID )
{
	BYTE pBuffer[ 4 ];

	CPacketCompressStream Stream( pBuffer, 16 );

	Stream.Write( &iSkillID, sizeof(int) );

	Send( eActor::SC_COOLTIME_RESET, &Stream );
}

bool CDnActor::Compare_PreStateOrder( CDnActor::PreStateStruct &a, CDnActor::PreStateStruct &b )
{
	return ( a.nOffset < b.nOffset ) ? true : false;
}

void CDnActorActionStateCache::LoadAction( CDnActor* pActor, const char* szFullPathName,								\
										   std::vector<CEtActionBase::ActionElementStruct*>* pVecActionElementList,		\
										   std::vector<std::vector<CDnActor::PreStateStruct>>& ActionState,				\
										   std::vector<std::vector<CDnActor::PreStateStruct>>& ActionCustomState,		\
										   std::vector<std::vector<CDnActor::PreStateStruct>>& CanMoveState )
{
	// 캐쉬 되어 있는 정보가 있는지 확인
	{
		//CPerformanceLog log("loadaction-cache");

		std::map<std::string,SData>::iterator itor;
		{
			ScopeLock<CSyncLock> lock(m_Lock);
			itor = m_mRepository.find( szFullPathName );
		}

		if( itor != m_mRepository.end() )
		{
			ActionState.reserve( (*itor).second.VecPreActionState.size() );
			ActionCustomState.reserve( (*itor).second.VecPreActionCustomState.size() );
			CanMoveState.reserve( (*itor).second.VecPreCanMoveState.size() );

			ActionState			= (*itor).second.VecPreActionState;
			ActionCustomState	= (*itor).second.VecPreActionCustomState;
			CanMoveState		= (*itor).second.VecPreCanMoveState;
			return;
		}
	}


	SData data;

	int nValue = 0;
	std::vector<CDnActor::PreStateStruct> VecList;
	CEtActionSignal *pSignal;
	for( DWORD i=0; i< pVecActionElementList->size(); i++ ) 
	{
		// STE_State
		VecList.clear();
		for( int j=0; ; j++ ) 
		{
			nValue = 0;
			pSignal = pActor->GetSignal( i, STE_State, j );
			if( pSignal ) 
			{
				StateStruct *pStruct = (StateStruct *)pSignal->GetData();
				nValue = CDnActorState::s_nActorStateIndex[pStruct->nStateOne] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateTwo] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateThree];
			}
			else 
				break;

			VecList.push_back( CDnActor::PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		
		if( VecList.empty() ) 
			VecList.push_back( CDnActor::PreStateStruct( nValue, 0 ) );
		else 
			std::sort( VecList.begin(), VecList.end(), pActor->Compare_PreStateOrder );

		data.VecPreActionState.push_back( VecList );

		// STE_CustomState
		VecList.clear();
		for( int j=0; ; j++ ) 
		{
			nValue = 0;
			pSignal = pActor->GetSignal( i, STE_CustomState, j );
			if( pSignal ) {
				CustomStateStruct *pStruct = (CustomStateStruct *)pSignal->GetData();
				nValue = CDnActorState::s_nActorStateIndex[pStruct->nStateOne] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateTwo] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateThree];
			}
			else 
				break;

			VecList.push_back( CDnActor::PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		if( VecList.empty() ) 
			VecList.push_back( CDnActor::PreStateStruct( nValue, 0 ) );
		else 
			std::sort( VecList.begin(), VecList.end(), pActor->Compare_PreStateOrder );

		data.VecPreActionCustomState.push_back( VecList );

		// STE_CanMove
		VecList.clear();
		for( int j=0; ; j++ ) 
		{
			nValue = 0;
			pSignal = pActor->GetSignal( i, STE_CanMove, j );
			if( pSignal ) 
			{
				CanMoveStruct *pStruct = (CanMoveStruct *)pSignal->GetData();
				nValue = pStruct->bCanMove;
			}
			else 
				break;

			VecList.push_back( CDnActor::PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		if( VecList.empty() ) 
			VecList.push_back( CDnActor::PreStateStruct( nValue, 0 ) );
		else 
			std::sort( VecList.begin(), VecList.end(), pActor->Compare_PreStateOrder );

		data.VecPreCanMoveState.push_back( VecList );
	}

	// 캐쉬정보 기록
	{
		ScopeLock<CSyncLock> lock(m_Lock);
		m_mRepository.insert( std::make_pair(szFullPathName, data) );
	}

	ActionState.reserve( data.VecPreActionState.size() );
	ActionCustomState.reserve( data.VecPreActionCustomState.size() );
	CanMoveState.reserve( data.VecPreCanMoveState.size() );

	ActionState			= data.VecPreActionState;
	ActionCustomState	= data.VecPreActionCustomState;
	CanMoveState		= data.VecPreCanMoveState;
}

bool CDnActor::LoadAction( const char *szFullPathName )
{
	bool bResult = CDnActionBase::LoadAction( szFullPathName );
	if( !bResult ) return false;

	CDnActorActionStateCache::GetInstance().LoadAction( this, szFullPathName, m_pVecActionElementList, m_VecPreActionState, m_VecPreActionCustomState, m_VecPreCanMoveState );

	return bResult;
}

void CDnActor::FreeAction()
{
	CDnActionBase::FreeAction();
	SAFE_DELETE_VEC( m_VecPreActionState );
}

void CDnActor::LinkWeapon( int nEquipIndex )
{
	switch( m_hWeapon[nEquipIndex]->GetEquipType() ) {
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
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			break;
		case CDnWeapon::Arrow:
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), m_hWeapon[0] );
			break;
	}
}

void CDnActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
#if defined(PRE_ADD_50907)
	if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_237))
	{
		if (IsChangeWeaponLock() == true)
		{
			//무기 해제 상태효과가 적용 된 시점인데 무기 장착이 되는 경우라면 이 시점에 변경은 안되도록 스킵하고,
			//OrigWeapon 무기 정보를 갱신한다.
			SetOrigWeaponWhenDisarmament(hWeapon, bDelete);
			return;
		}
	}
#endif // PRE_ADD_50907

	if( m_hWeapon[nEquipIndex] ) {
		DetachWeapon( nEquipIndex );
	}
	m_hWeapon[nEquipIndex] = hWeapon;
	m_bSelfDeleteWeapon[nEquipIndex] = bDelete;
	m_hWeapon[nEquipIndex]->CreateObject();

	LinkWeapon( nEquipIndex );

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	MASkillUser::OnAttachWeapon( m_hWeapon[ nEquipIndex ], nEquipIndex );
}

void CDnActor::DetachWeapon( int nEquipIndex )
{
	if( !m_hWeapon[nEquipIndex] ) return;

#if defined(PRE_ADD_50907)
	if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_237))
	{
		if (IsChangeWeaponLock() == true)
			return;
	}
#endif // PRE_ADD_50907

	m_hWeapon[nEquipIndex]->FreeObject();
	m_hWeapon[nEquipIndex]->UnlinkWeapon();

	if( m_bSelfDeleteWeapon[nEquipIndex] ) {
		SAFE_RELEASE_SPTR( m_hWeapon[nEquipIndex] );
		m_bSelfDeleteWeapon[nEquipIndex] = false;
	}
	m_hWeapon[nEquipIndex].Identity();

#if defined(PRE_ADD_50907)
	if (IsSkipOnAttatchDetachWeapon() == true)
		return;
#endif // PRE_ADD_50907

	MASkillUser::OnDetachWeapon( m_hWeapon[ nEquipIndex ], nEquipIndex );
}

void CDnActor::ShowWeapon( int nEquipIndex, bool bShow )
{
	if( m_hWeapon[nEquipIndex] )
		m_hWeapon[nEquipIndex]->ShowWeapon( bShow );
}

bool CDnActor::IsStay( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stay ) ? true : false;
	return false;
}

bool CDnActor::IsMove( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Move ) ? true : false;
	return false;
}

bool CDnActor::IsAttack( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Attack ) ? true : false;
	return false;
}

bool CDnActor::IsHit( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Hit ) ? true : false;
	return false;
}

bool CDnActor::IsAir( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Air ) ? true : false;
	return false;
}

bool CDnActor::IsDown( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Down ) ? true : false;
	return false;
}

bool CDnActor::IsStun( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stun ) ? true : false;
	return false;
}

bool CDnActor::IsStiff( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stiff ) ? true : false;
	return false;
}

bool CDnActor::IsFly( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_Fly ) ? true : false;
	return false;
}

bool CDnActor::IsGround( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_Ground ) ? true : false;
	return false;
}

bool CDnActor::IsStandHit( const char *szActionName )
{
	int nIndex;
	int nState = 0;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) nState = m_VecPreActionState[nIndex][0].nState;
	else return false;
	if( nState == ActorStateEnum::Hit || nState == ( ActorStateEnum::Hit | ActorStateEnum::Stiff ) ) return true;
	return false;
}

int CDnActor::GetState( const char *szActionName )
{
	int nIndex;
	if( ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return m_VecPreActionState[nIndex][0].nState;
	return 0;
}

int CDnActor::GetState( int nElementIndex )
{
	if( nElementIndex < 0 || nElementIndex >= (int)m_VecPreActionState.size() ) return 0;
	return m_VecPreActionState[nElementIndex][0].nState;
}


// SetPlaySpeed 와는 별개로 프레임 변경, 프레임 정지 상태효과가 Begin/End 될 때 마다 업데이트 되는 함수이다.
// 적용된 프레임 변경 상태효과의 값들을 모아 한꺼번에 값을 더해서 셋팅해준다.
// FrameStop 상태효과가 있다면 아예 0 프레임으로 처리.
// 만약 상태효과가 비어있다면 60 프레임으로 복구시켜준다.
// SetPlaySpeed 와는 별개로 돌아가지만 ProcessPlaySpeed() 에서 종료된 후에 이 함수를 한번 더 호출해서 상태효과들로
// 영향을 받는 최종 프레임을 다시 업데이트 해주어야 한다. 따라서 상태효과쪽에서는 SetPlaySpeed 함수를 사용해서는 안된다.
void CDnActor::UpdateFPS( void )
{
	// 프랍 액터는 process 를 돌지 않기 때문에 playspeed 조절하면 안된다. #26354 
	if( ActorTypeEnum::PropActor == GetActorType() )
		return;

	float fFrameRatio = 1.0f;
	if( m_bModifyPlaySpeed )
	{
		fFrameRatio = m_fPlaySpeed;

		float fPrevFPS = CDnActionBase::m_fFPS;
		CDnActionBase::SetFPS( 60.f * fFrameRatio );

		for( DWORD i=0; i<m_hVecLastHitListByRemainTime.size(); i++ )
		{
			if( m_hVecLastHitListByRemainTime[i] ) 
			{
				// 먼저 60 프레임 기준으로 원상 복구 시킨 후
				float fFrame = ( ( m_hVecLastHitListByRemainTime[i]->GetDamageRemainTime( GetUniqueID() ) - CDnActionBase::m_LocalTime ) / 1000.f ) * fPrevFPS;

				// 새로 적용될 프레임 스피드로 RemainTime 재 계산.
				m_hVecLastHitListByRemainTime[i]->SetDamageRemainTime( GetUniqueID(), CDnActionBase::m_LocalTime + (LOCAL_TIME)( fFrame / ( CDnActionBase::m_fFPS) * 1000.f ) );
			}
		}
	}
	else
	{
		ResetPlaySpeed();

		// 프레임 정지 상태효과. 하나라도 걸려있다면 모든 프레임 정지.
		if( 0 < GetFrameStopRefCount() )
		{
			float fPrevFPS = CDnActionBase::m_fFPS;
			CDnActionBase::SetFPS( 0.0f );
			fFrameRatio = 0.0f;
		}
		else
		if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 ) ||
			IsAppliedThisStateBlow( STATE_BLOW::BLOW_144 ) || 
			IsAppliedThisStateBlow( STATE_BLOW::BLOW_220)
			)
		{
#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
			float fBuffFrameRatio = 0.0f;
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT
			// 프레임 변경 상태효과들의 값을 모아서 적용.
			DNVector( DnBlowHandle ) vlhFrameBlows;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_025, vlhFrameBlows );
			for( int i = 0; i < (int)vlhFrameBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlhFrameBlows.at( i );
#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
				if( hBlow->GetAddBlowStateType() == AddBlowStateType::Equip_Buff_Level )
					fBuffFrameRatio += hBlow->GetFloatValue();
				else
					fFrameRatio += hBlow->GetFloatValue();
#else // PRE_ADD_SLOW_DEBUFF_LIMIT
				fFrameRatio += hBlow->GetFloatValue();
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT
			}

			// #27320 동상 상태효과도 프레임 느려짐의 효과가 있다.
			vlhFrameBlows.clear();
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_144, vlhFrameBlows );
			for( int i = 0; i < (int)vlhFrameBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlhFrameBlows.at( i );
#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
				if( hBlow->GetAddBlowStateType() == AddBlowStateType::Equip_Buff_Level )
					fBuffFrameRatio += static_cast<CDnFrostbiteBlow*>(hBlow.GetPointer())->GetFrameBlowArg();
				else
					fFrameRatio += static_cast<CDnFrostbiteBlow*>(hBlow.GetPointer())->GetFrameBlowArg();
#else // PRE_ADD_SLOW_DEBUFF_LIMIT
				fFrameRatio += static_cast<CDnFrostbiteBlow*>(hBlow.GetPointer())->GetFrameBlowArg();
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT
			}

			vlhFrameBlows.clear();
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_220, vlhFrameBlows );
			for( int i = 0; i < (int)vlhFrameBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlhFrameBlows.at( i );
#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
				if( hBlow->GetAddBlowStateType() == AddBlowStateType::Equip_Buff_Level )
					fBuffFrameRatio += hBlow->GetFloatValue();
				else
					fFrameRatio += hBlow->GetFloatValue();
#else // PRE_ADD_SLOW_DEBUFF_LIMIT
				fFrameRatio += hBlow->GetFloatValue();
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT
			}

#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
			if( GetGameRoom() && GetGameRoom()->bIsPvPRoom() )
			{
				if( fBuffFrameRatio < CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FrameSpeedRatio_Limit_Min ) )
					fBuffFrameRatio = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FrameSpeedRatio_Limit_Min );
				else if( fBuffFrameRatio > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FrameSpeedRatio_Limit_Max ) )
					fBuffFrameRatio = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::FrameSpeedRatio_Limit_Max );
			}

			fFrameRatio += fBuffFrameRatio;
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT

			if( fFrameRatio < 0.0f )
				fFrameRatio = 0.0f;

			float fPrevFPS = CDnActionBase::m_fFPS;
			CDnActionBase::SetFPS( 60.0f * fFrameRatio );

			for( DWORD i=0; i<m_hVecLastHitListByRemainTime.size(); i++ )
			{
				if( m_hVecLastHitListByRemainTime[i] ) 
				{
					// 먼저 60 프레임 기준으로 원상 복구 시킨 후
					float fFrame = ( ( m_hVecLastHitListByRemainTime[i]->GetDamageRemainTime( GetUniqueID() ) - CDnActionBase::m_LocalTime ) / 1000.f ) * fPrevFPS;
		
					// 새로 적용될 프레임 스피드로 RemainTime 재 계산.
					m_hVecLastHitListByRemainTime[i]->SetDamageRemainTime( GetUniqueID(), CDnActionBase::m_LocalTime + (LOCAL_TIME)( fFrame / ( CDnActionBase::m_fFPS ) * 1000.f ) );
				}
			}
		}
		else
		{
			fFrameRatio = 1.0f;

			float fPrevFPS = CDnActionBase::m_fFPS;
			
			// 원상복구
			CDnActionBase::SetFPS( 60.0f );

			for( DWORD i=0; i<m_hVecLastHitListByRemainTime.size(); i++ )
			{
				if( m_hVecLastHitListByRemainTime[i] )
				{
					float fFrame = ( ( m_hVecLastHitListByRemainTime[i]->GetDamageRemainTime( GetUniqueID() ) - CDnActionBase::m_LocalTime ) / 1000.f ) * fPrevFPS;
					if( m_hVecLastHitListByRemainTime[i] )
						m_hVecLastHitListByRemainTime[i]->SetDamageRemainTime( GetUniqueID(), CDnActionBase::m_LocalTime + (LOCAL_TIME)( fFrame / ( CDnActionBase::m_fFPS ) * 1000.f ) );
				}
			}

			m_hVecLastHitListByRemainTime.clear();
		}
	}

	if( IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(this);
		pPlayerActor->OnChangePlaySpeed( 0, fFrameRatio );
	}
}

void CDnActor::AddedFrameStop( void ) 
{ 
	++m_iFrameStopRefCount;

	// #28345 몬스터인 경우 AI 를 꺼주지 않으면 프레임 멈춰진 동안 액션이 새로 셋팅되면서
	// 0 프레임으로 바뀌어버리는 문제가 생김.
	if( IsMonsterActor() )
	{
		static_cast<CDnMonsterActor*>(this)->SetAIState( MAAiReceiver::Disable, false );
	}

	UpdateFPS(); 
};

void CDnActor::RemovedFrameStop( void ) 
{
	m_iFrameStopRefCount--; 
	if( m_iFrameStopRefCount < 0 )
		m_iFrameStopRefCount = 0;

	// #28345 몬스터인 경우 AI 를 꺼주지 않으면 프레임 멈춰진 동안 액션이 새로 셋팅되면서
	// 0 프레임으로 바뀌어버리는 문제가 생김.
	if( IsMonsterActor() )
	{
		if( 0 == m_iFrameStopRefCount )
		{
			static_cast<CDnMonsterActor*>(this)->SetAIState( MAAiReceiver::Threat, false );
		}
	}

	UpdateFPS(); 
};


void CDnActor::SetPlaySpeed( DWORD dwFrame, float fSpeed )
{
	//// 프레임 변경 상태효과가 우선 처리대상이므로 상태효과 적용중이라면 SetPlaySpeed() 함수 무시.
	//if( 0 < GetFrameStopRefCount() || IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 ) )
	//	return;

	if( m_bModifyPlaySpeed == true ) 
		return;

	// 프랍 액터는 process 를 돌지 않기 때문에 playspeed 조절하면 안된다. #26354 
	if( ActorTypeEnum::PropActor == GetActorType() )
		return;

	m_bModifyPlaySpeed = true;
	m_PlaySpeedStartTime = CDnActionBase::m_LocalTime;
	m_dwPlaySpeedTime = dwFrame;

	m_fPlaySpeed = fSpeed;
	UpdateFPS();
}


void CDnActor::ProcessPlaySpeed( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bModifyPlaySpeed == false ) return;
	if( m_PlaySpeedStartTime == 0 ) m_PlaySpeedStartTime = LocalTime;
	if( LocalTime - m_PlaySpeedStartTime > m_dwPlaySpeedTime ) 
	{
		//m_bModifyPlaySpeed = false; // ResetPlaySpeed 함수 내부에서 호출됨.
		ResetPlaySpeed();
		UpdateFPS();
		return;
	}
}


// #15065 이슈 관련.
// 결빙 시 프레임 멈추는 것 SetPlaySpeed 함수 호출할 때 이미 타격때문에 잠깐 PlaySpeed 조정되는 타이밍일 때가 있어서 
// 함수호출이 씹힐 때가 있다. CDnFreezingBlow::OnBegin() 에서 SetPlaySpeed() 호출하기 전에 리셋시키고 호출한다.
void CDnActor::ResetPlaySpeed()
{
	if( m_bModifyPlaySpeed == false ) 
		return;

	for( DWORD i=0; i<m_hVecLastHitListByRemainTime.size(); i++ ) {
		if( m_hVecLastHitListByRemainTime[i] ) {
			float fFrame = ( ( m_hVecLastHitListByRemainTime[i]->GetDamageRemainTime( GetUniqueID() ) - CDnActionBase::m_LocalTime ) / 1000.f ) * CDnActionBase::m_fFPS;
			m_hVecLastHitListByRemainTime[i]->SetDamageRemainTime( GetUniqueID(), CDnActionBase::m_LocalTime + (LOCAL_TIME)( fFrame / 60.f * 1000.f ) );
		}
	}

	CDnActionBase::SetFPS( 60.f );

	if( IsPlayerActor() )
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(this);
		pPlayerActor->OnChangePlaySpeed( 0, 60.0f / CDnActionBase::m_fFPS );
	}

	// Note: SignalEndTime 이 복구 시켜준 놈들은 여기서 날립니다.
	//m_hVecLastHitListByRemainTime.clear();

	m_bModifyPlaySpeed = false;
}



void CDnActor::SyncClassTime( LOCAL_TIME LocalTime )
{
	CDnActionBase::m_LocalTime = LocalTime;
	MASkillUser::m_LocalTime = LocalTime;
}

void CDnActor::OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash )
{
	if( !IsDie() && IsHit() && ( IsAir() || IsDown() ) && !IsStun() ) {
		float fDecrease = 0.85f;

		if( GetVelocity()->x != 0.f ) {
			SetVelocityX( -GetVelocity()->x * fDecrease );
			SetResistanceX( -GetResistance()->x );
		}

		if( GetVelocityValue()->z < -120.f ) {
			float fVelocityValue = GetVelocityValue()->z; // 일정 이상의 충격일 경우에는 삭감해준다. 지금상태에선 300 이 최적..
			if( fVelocityValue < -300.f ) fVelocityValue = -300.f;
			float fYVel = 5.f + ( -fVelocityValue * 0.01f );
//			if( fYVel > 8.f ) fYVel = 8.f;
//			if( fVelocityValue >
//			OutputDebug( "PrevVel Z : %.2f PrevVel Y : %.2f - %.2f\n", GetVelocity()->z, GetVelocity()->y, GetVelocityValue()->z );
			SetVelocityZ( -GetVelocity()->z * ( 1.35f - ( 1.f / 4.f * ( -fVelocityValue * 0.01f  ) ) ) );
			SetResistanceZ( -GetResistance()->z );

			SetVelocityY( fYVel );
			SetResistanceY( -18.f );

//			OutputDebug( "Vel Z : %.2f Vel Y : %.2f\n", GetVelocity()->z, GetVelocity()->y );
//			OutputDebug( "Res Z : %.2f Res Y : %.2f\n", GetResistance()->z, GetResistance()->y );

			EtVector3 vLook;
			vLook = -m_Cross.m_vZAxis;
			vLook.y = 0.f;
			EtVec3Normalize( &vLook, &vLook );
			Look( EtVector2( vLook.x, vLook.z ) );
			if( IsDie() ) SetActionQueue( "Die_Air", 0, 0.f );
			else SetActionQueue( "Hit_AirUp", 0, 0.f );
			if( m_HitParam.hWeapon ) {
				m_HitParam.hWeapon->OnHitSuccess( GetMySmartPtr(), CDnWeapon::Stun, vPosition, true );
			}
		}
		else {
			if( GetVelocity()->z != 0.f ) {
				SetVelocityZ( -GetVelocity()->z * fDecrease );
				SetResistanceZ( -GetResistance()->z );
			}
		}

	}
}

bool CDnActor::IsHittable( DnActorHandle hHitter, LOCAL_TIME LocalTime, HitStruct *pHitSignal, int iHitUniqueID /*= -1*/ )
{
#if !defined( _FINAL_BUILD )
	if( bIsIgnoreHit() ) return false;
#endif // #if !defined( _FINAL_BUILD )

	bool isSelfCheck = false;
	isSelfCheck = pHitSignal ? (pHitSignal->isSelfCheck?true:false) : false;

	//자신 체크가 설정되어 있지 않을때만(false가 기본. 특정(왁스 스킬...)에서만 true로 설정.
	//true로 설정되어 있으면 자신도 Hit처리..
	if (false == isSelfCheck)
		if( hHitter == GetMySmartPtr() ) return false;
	
	if( IsDie() ) return false;
	
	CDnActorState::ActorTypeEnum actorType = GetActorType();

	if( actorType == CDnActorState::Npc ) return false;

	//ActorType [0:All/1:User/2:Monster]
	if (pHitSignal)
	{
		switch(pHitSignal->nActorType)
		{
		case 1:	//User(적용 대상이 유저인데 현재 actor가 유저가 아니라면)
			{
				if (!IsPlayerActor())
					return false;
			}
			break;
		case 2:	//Monster(적용 대상이 몬스터 인데 몬스터가 아니라면)
			{
				if (!IsMonsterActor())
					return false;
			}
			break;
		}
	}

	if( GetLastDamageHitUniqueID( hHitter->GetUniqueID() ) != -1 )
	{
		if( GetLastDamageHitUniqueID( hHitter->GetUniqueID() ) == iHitUniqueID )
			if( GetDamageRemainTime( hHitter->GetUniqueID() ) >= LocalTime ) 
				return false;
	}
	else if( GetDamageRemainTime( hHitter->GetUniqueID() ) >= LocalTime ) 
	{
		bool bIsChangedByInputAction = false;

		// #69997 이전 액션에서 발생한 히트시그널이 다음액션에도 영향을 미치는 부분 수정.
		if( m_nLastDamageHitterActionIndex > 0 && m_nLastDamageHitterActionIndex != hHitter->GetCurrentActionIndex() )
		{
			if( hHitter->GetProcessSkill() )
			{
				bIsChangedByInputAction = hHitter->GetProcessSkill()->IsChainInputAction( hHitter->GetCurrentAction() );
			}
		}

		if( bIsChangedByInputAction == false && hHitter->IsOnSignalFromChargerSE() == false )
		{
			return false;
		}
	}

	// hit 시그널 구간과 팀만 제대로 체크된다면 149번 얼음감옥 상태효과가 있다면 무조건 맞도록 된다. //
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_149 ) && pHitSignal )
	{
		//얼음감옥이더라도 상태효과 필터가 적용 되어 있으면 먼저 필터 적용 체크
		if( pHitSignal->StateEffectFilter != 0 )
		{
			// 현재 엘리멘탈로드의 아이시 프랙션에서만 예외적으로 2가지의 상태효과 필터링을 걸고 있는데
			// 둘 중에 하나만 hit 되길 원하기 때문에 먼저 체크된 것은 다음 필터링에 걸리지 않도록 한다. #28747
			// 예외 조건에 걸리지 않으면 일반적으로 처리함.
			if( false == hHitter->CheckHitSignalStateEffectFilterException( GetUniqueID(), pHitSignal->StateEffectFilter) )
			{
				return false;
			}
			else
			if( false == m_pStateBlow->IsApplied( (STATE_BLOW::emBLOW_INDEX)pHitSignal->StateEffectFilter ) )
			{
				return false;
			}
		}

		int nOriginalTeam = GetOriginalTeam();
		int nHitterTeam = hHitter->GetTeam();
		//얼음 감옥 상태에서는 적에게는 맞으면 안되고, 아군에게는 맞아야 한다.??
		if (nOriginalTeam != nHitterTeam)
			return false;
		else
		{
			//적에게 때리는 경우 인데.. 자신 이라면
			if (pHitSignal->nTargetType == 0 && GetMySmartPtr() == hHitter->GetMySmartPtr() ) 
				return false;
			
			//얼음 감옥 상태에서는 아군에게는 무조건 맞아야 한다..
			return true;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	// ignorecanhit 가 꺼져있고 무적 상태효과가 있으면 맞지 않는다.
	if( pHitSignal )
	{
		if( pHitSignal->bIgnoreCanHit == TRUE )
		{
			DNVector(DnBlowHandle) vBlows;
			m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_099, vBlows );
			for( int i = 0; i < (int)vBlows.size(); ++i )
			{
				if( vBlows[i]->GetFloatValue() == -1 )
					return false;
			}
		}
		else if( m_pStateBlow->IsApplied(STATE_BLOW::BLOW_099) )
		{
			return false;
		}

#if !defined(PRE_FIX_73833)
		// 무게값 제한 값이 있다면 지정된 무게값 이하가 되었을 때만 맞는다.
		if( 0 < pHitSignal->nWeightValueLimit )
		{
			if( (float)pHitSignal->nWeightValueLimit < GetWeight() )
				return false;
		}
#endif
	}
	else if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_099 ) )
		return false;

	if( hHitter->GetProcessSkill() )
	{
		bool bResist = false;
		if( !IsHittableSkill( hHitter->GetProcessSkill()->GetClassID() , bResist ) )
		{
			if( bResist )
			{
				SendAddSEFail( CDnStateBlow::ADD_FAIL_BY_IMMUNE , STATE_BLOW::BLOW_154 );
			}

			return false;
		}
	}

#if defined(PRE_ADD_50923)
	//무적 상태효과 Type별 적용이 되어 있으면
	if ( m_pStateBlow->IsApplied(STATE_BLOW::BLOW_239))
	{
		DNVector(DnBlowHandle) vResult;
		m_pStateBlow->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_239, vResult);

		//무적 Type 값들을 리스트로 담아 놓는다.
		std::map<int, int> invincibleTypeList;
		for( UINT i=0 ; i<vResult.size() ; ++i ) 
		{
			DnBlowHandle hBlow = vResult.at( i );
			if( !hBlow )
				continue;

			CDnInvincibleTypeBlow* pInvincibleTypeBlow = static_cast<CDnInvincibleTypeBlow*>(hBlow.GetPointer());
			if (NULL == pInvincibleTypeBlow)
				continue;

			int nInvincibleTypeValue = pInvincibleTypeBlow->GetInvincibleTypeValue();
			invincibleTypeList.insert(std::make_pair(nInvincibleTypeValue, nInvincibleTypeValue));
		}

		if (hHitter->IsAppliedThisStateBlow(STATE_BLOW::BLOW_240))
		{
			DNVector(DnBlowHandle) vResultImmunByType;
			CDnStateBlow *pHitterStateBlow = hHitter->GetStateBlow(); 
			if (pHitterStateBlow)
				pHitterStateBlow->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_240, vResultImmunByType);

			for( UINT i=0 ; i<vResultImmunByType.size() ; ++i ) 
			{
				DnBlowHandle hBlow = vResultImmunByType.at( i );
				if( !hBlow )
					continue;

				CDnImmuneByType* pImmuneTypeBlow = static_cast<CDnImmuneByType*>(hBlow.GetPointer());
				if (NULL == pImmuneTypeBlow)
					continue;

				int nImmuneTypeValue = pImmuneTypeBlow->GetImmuneTypeValue();
				
				std::map<int, int>::iterator findIter = invincibleTypeList.find(nImmuneTypeValue);
				if (findIter != invincibleTypeList.end())
					invincibleTypeList.erase(findIter);
			}

			//무적 Type 무시 상태효과로 전부 제거가 되어야 Hit될 수 있다.
			//리스트가 남아 있다면 무시 상태효과가 없으므로 Hit될 수 없다.
			if (invincibleTypeList.empty() == false)
				return false;
		}
		//무적 상태효과 Type 무시 상태효과가 Hitter에 적용 되어 있지 않으면 Hit될 수 없다.
		else
			return false;
	}
#endif // PRE_ADD_50923

	// 액션툴에서 상태효과 필터링이 지정되어있으면 체크한다.
	if( pHitSignal )
	{
#ifdef PRE_ADD_HITDIRECTION
		EtVector3 vDir = *GetPosition() - *hHitter->GetPosition();
		EtVec3Normalize( &vDir, &vDir );
		switch( pHitSignal->CheckHitDirection )
		{
		// front
		case 1:
			{
				// 피격자가 때린 놈을 바라보고 있는 경우 hit 처리.
				float fDot = EtVec3Dot( &vDir, &(m_Cross.m_vZAxis) );
				if( fDot >= -0.1f )	// 범위는 피격자(일반적인 경우 플레이어)에게 유리하게 설정.
					return false;
			}
			break;
		// back
		case 2:
			{
				// 피격자가 때린 놈을 등지고 있는 경우 hit 처리.
				float fDot = EtVec3Dot( &vDir, &(m_Cross.m_vZAxis) );
				if( fDot < 0.1f )
					return false;
			}
			break;
		}
#endif // #ifdef PRE_ADD_HITDIRECTION

#if defined(PRE_ADD_57090)
		//////////////////////////////////////////////////////////////////////////
		//#57090 특정 대상이 공격당하지 않게 하는 기능 - 설정된 상태효과가 적용 되어 있으면 히트 되지 않음.
		//StateEffectFilter보다 우선 검사...
		std::vector<string> vlTokens;
		string strArgument = pHitSignal->szUnStateEffectFilter ? pHitSignal->szUnStateEffectFilter : "";
		TokenizeA( strArgument, vlTokens, ";" );

		if (vlTokens.empty() == false)
		{
			int nTokenSize = (int)vlTokens.size();
			for (int i = 0; i < nTokenSize; ++i)
			{
				STATE_BLOW::emBLOW_INDEX emBlowIndex = (STATE_BLOW::emBLOW_INDEX)atoi(vlTokens[i].c_str());
				if( true == m_pStateBlow->IsApplied( emBlowIndex ) )
				{
					return false;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
#endif // PRE_ADD_57090

		if( pHitSignal->StateEffectFilter != 0 )
		{
			// 현재 엘리멘탈로드의 아이시 프랙션에서만 예외적으로 2가지의 상태효과 필터링을 걸고 있는데
			// 둘 중에 하나만 hit 되길 원하기 때문에 먼저 체크된 것은 다음 필터링에 걸리지 않도록 한다. #28747
			// 예외 조건에 걸리지 않으면 일반적으로 처리함.
			if( false == hHitter->CheckHitSignalStateEffectFilterException( GetUniqueID(), pHitSignal->StateEffectFilter) )
			{
				return false;
			}
			else
			if( false == m_pStateBlow->IsApplied( (STATE_BLOW::emBLOW_INDEX)pHitSignal->StateEffectFilter ) )
			{
				return false;
			}
		}

#ifdef PRE_ADD_MONSTER_CATCH
		// 몬스터에 잡힌건지 체크하는 플래그가 켜져 있다면 몬스터에게 잡힌 경우만 구분.
		if( TRUE == pHitSignal->bIsCatchedActor )
		{
			if( IsPlayerActor() )
			{
				CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(this);
				if( false == pPlayerActor->IsCatchedByMonster() )
				{
					return false;
				}
			}
		}
#endif // #ifdef PRE_ADD_MONSATER_CATCH
		
		// 주변에 내가 소환한 렐릭 몬스터인지 아닌지 구분해줌. 쇼크 오브 렐릭 스킬에서 사용함.
		if( TRUE == pHitSignal->bUseMyRelicFilter )
		{
			bool bIsMyRelicMonster = hHitter->IsMyRelicMonster( GetMySmartPtr() );
			if( bIsMyRelicMonster )
				return true;
			else
				return false;
		}

		// #13885 이슈 관련.
		int nTargetType = pHitSignal->nTargetType;
		if( hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_086 ) )
		{
			// 팀 반전 상태효과 하나만 적용된다.
			DNVector( DnBlowHandle ) vlReverseTeamBlows;
			hHitter->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_086, vlReverseTeamBlows );
			_ASSERT( false == vlReverseTeamBlows.empty() );
			if( false == vlReverseTeamBlows.empty() )
			{
				DnBlowHandle hBlow = vlReverseTeamBlows.front();
				CDnReverseTeamBlow* pReverseTeamBlow = static_cast<CDnReverseTeamBlow*>(hBlow.GetPointer());
				if( CDnReverseTeamBlow::HIT_SIGNAL_TARGET_CHANGE == pReverseTeamBlow->GetType() )
				{
					switch( nTargetType )
					{
						case 0: // Enemy -> Friend
							nTargetType = 1;
							break;

						case 1: // Friend -> Enemy
							{
								// #50166 폭발관련 시그널은 기존상태를 유지하도록 한다.
								if(pHitSignal->fDamageProb > 0) 
									nTargetType = 1;
								else
									nTargetType = 0;
							}
							break;

						case 2: // All -> All
							{
								// #51871 이슈관련 All 타입일 경우에도 데미지가 있는경우 영향을 끼치지 않도록 설정합니다.
								if(pHitSignal->fDamageProb > 0) 
									nTargetType = 1;
							}
							break;
					}
				}
				else
				if( CDnReverseTeamBlow::HIT_SIGNAL_TARGET_ALL == pReverseTeamBlow->GetType() )
				{
					// 타격대상 all 로 바꿔줌.
					nTargetType = 2;
				}
			}
		}

		switch( nTargetType )
		{
			case 0: // Enemy
				if( GetTeam() == hHitter->GetTeam() ) return false; 
				if( GetMySmartPtr() == hHitter->GetMySmartPtr() ) return false;
				if( false == pHitSignal->bIgnoreCanHit &&
					false == IsAppliedThisStateBlow( STATE_BLOW::BLOW_138 ) )
				{
					if( !CDnActorState::IsHittable() )
						return false;
				}
				break;

			case 1: // Friend
				
				if( GetTeam() != hHitter->GetTeam() ) return false;
				
#ifdef PRE_FIX_63315
				// #63315 - TargetType 이 Friend 일 경우에도 Hittable 검사하도록 추가.
				if( false == pHitSignal->bIgnoreCanHit &&
					false == IsAppliedThisStateBlow( STATE_BLOW::BLOW_138 ) )
				{
					if( !CDnActorState::IsHittable() )
						return false;
				}
#endif // PRE_FIX_63315
				break;

			case 2: // All
				// CanHit 시그널이 false 일 때 hit 시그널 targettype 이 all 이라도 적군은 hit 되지 않도록 수정. (#10570)
				if( GetTeam() != hHitter->GetTeam() )
				{
					// Hit 시그널에 CanHit 무시하는 플래그 켜져 있으면 무조건 맞을 수 있는 상태로 판단.
					// 138번 상태효과 걸렸을 때도 마찬가지. (#19619)
					if( false == pHitSignal->bIgnoreCanHit &&
						false == IsAppliedThisStateBlow( STATE_BLOW::BLOW_138 ) )
					{
						if( !CDnActorState::IsHittable() )
							return false;
					}
				}
				break;
		}
	
	
		switch( pHitSignal->nStateCondition )
		{
			case 0:	// Normal
				if( GetState() == CDnActorState::Down ) return false;
				break;
			case 1:	// Down
				if( !( GetState() & CDnActorState::Down ) ) return false;
				break;
			case 2:	// Normal + Down
				break;
			case 3:	// All
				break;
		}
	}
	return true;
}

bool CDnActor::IsHittableSkill( int iSkillIndex , bool &bSendResist )
{
	// 특정 스킬에 대한 무적 처리 [2010/11/22 semozz]
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_154 ) )
	{
		DNVector(DnBlowHandle) vResult;
		m_pStateBlow->GetStateBlowFromBlowIndex(STATE_BLOW::BLOW_154, vResult);

		for( UINT i=0 ; i<vResult.size() ; ++i ) 
		{
			DnBlowHandle hBlow = vResult.at( i );
			if( hBlow )
			{
				CDNInvincibleAtBlow* pInvincibleAtBlow = static_cast<CDNInvincibleAtBlow*>(hBlow.GetPointer());
				if ( pInvincibleAtBlow && pInvincibleAtBlow->IsInvincibleAt(iSkillIndex) )
				{
					bSendResist = pInvincibleAtBlow->IsEnableSendResist();
					return false;
				}
			}
		}
	}

	return true;
}

void CDnActor::Show( bool bShow )
{
	m_bShow = bShow;
}

void CDnActor::ResetActor()
{
	Show( true );
	m_fDownDelta = 0.f;
	m_fDieDelta = 0.f;
	m_fStiffDelta = 0.f;
	m_mapLastDamageTime.clear();
	m_mapLastHitUniqueID.clear();
	m_nLastDamageHitterActionIndex = 0;
	m_LastHitSignalTime = 0;
	m_nLastHitSignalIndex = -1;

	m_bModifyPlaySpeed = false;
	m_PlaySpeedStartTime = 0;
	m_dwPlaySpeedTime = 0;

	m_bEnableNormalSuperAmmor = false;
	m_nNormalSuperAmmorTime = 0;
	m_fBreakNormalSuperAmmorDurability = 0.f;
//	CDnActionBase::SetFPS( 60.f );
	CDnActionBase::ResetActionBase();
	MAActorRenderBase::ResetActorRenderBase();

	ResetMove();
	ResetLook();

	SetVelocity( EtVector3( 0.f, 0.f, 0.f ) );
	SetResistance( EtVector3( 0.f, 0.f, 0.f ) );
	SetAddHeight( 0.f );

	SetMagnetLength( 0.f );
	SetMagnetDir( EtVector2( 0.f, 0.f ) );

	m_vlForceSettedHitElement.clear();

	if( IsCustomAction() ) ResetCustomAction();
	SetActionQueue( "Stand", 0, 0.f, 0.f, false, false );
}

void CDnActor::ProcessCollision( EtVector3 &vMove )
{
	m_pMovement->PushAndCollisionCheck( vMove );
}

void CDnActor::ProcessState( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActorState::ProcessState( LocalTime, fDelta );

	// #48950 슈머아머가 깨진 경우엔 MoveY 시그널 값 리셋해준다.
	if( IsHit() )
	{
		ResetMoveYDistance();
	}

	if( m_pStateBlow &&
		false == IsNpcActor() )
	{
		m_pStateBlow->Process( LocalTime, fDelta );
		ProcessReservedRemoveBlows();
		ExecuteKillAfterProcessStateBlow();
	}
}

void CDnActor::SendAddSEFail( int iAddSEResult, STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	BYTE pBuffer[ 16 ];
	CPacketCompressStream Stream( pBuffer, 16 );

	Stream.Write( &iAddSEResult, sizeof(int) );

	Send( eActor::SC_ADD_STATE_EFFECT_DENIED, &Stream );
}

void CDnActor::PushSummonMonster( DnMonsterActorHandle hMonster, const SummonMonsterStruct* pSummonMonsterStruct, bool bReCreateFollowStageMonster/* = false*/ )
{
	//if( bReCreateFollowStageMonster )
	//	return;

	if( 0 == pSummonMonsterStruct->nGroupID )
	{
		m_listSummonMonster.push_back( hMonster );
	}
}

bool CDnActor::bIsCanSummonMonster( const SummonMonsterStruct* pSummonMonsterStruct )
{
	// 0 이면 개체수 제한을 하지 않겠다는 것임~!
	if( pSummonMonsterStruct->nMaxCount == 0 )
		return true;

	// 그룹 ID 가 정의되어 있다면 그룹 map 의 리스트를 찾아서 처리.
	int iCount = 0;
	if( IsPlayerActor() && 0 < pSummonMonsterStruct->nGroupID )
	{
		if( 0 < m_mapSummonMonsterByGroup.count( pSummonMonsterStruct->nGroupID ) )
		{
			std::list<DnMonsterActorHandle>& listSummonedGroupMonster = m_mapSummonMonsterByGroup[ pSummonMonsterStruct->nGroupID ];
			for( std::list<DnMonsterActorHandle>::iterator itor=listSummonedGroupMonster.begin(); itor!=listSummonedGroupMonster.end(); )
			{
				DnMonsterActorHandle hMonster = *itor;
				if( hMonster )
				{
					++iCount;
					++itor;
				}
				else
					itor = listSummonedGroupMonster.erase( itor );
			}

			if( iCount < pSummonMonsterStruct->nMaxCount )
				return true;
		}
		else
			return true;
	}
	else
	{
		for( std::list<DnMonsterActorHandle>::iterator itor=m_listSummonMonster.begin() ; itor!=m_listSummonMonster.end() ; )
		{
			DnMonsterActorHandle hMonster = *itor;
			if( hMonster )
			{
				if( hMonster->GetMonsterClassID() == pSummonMonsterStruct->MonsterID )
					++iCount;

				++itor;
			}
			else
				itor = m_listSummonMonster.erase( itor );
		}

		if( iCount < pSummonMonsterStruct->nMaxCount )
			return true;
	}

	return false;
}

int CDnActor::AddStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex, const CDnSkill::SkillInfo* pParentSkill, 
							int nDurationTime, const char *szParam, bool bOnPlayerInit/* = false*/, bool bCheckCanBegin/* = true*/ , bool bEternity /*=false*/ )
{
	// 부활 제외하고 죽었을 땐 상태효과 추가 안됨
	// PvP 라운드모드 라운드 재시작시 죽은상태에서 상태효과를 걸어줄때 bOnPlayerInit = true 로 넘어와서 Die 체크 안함. by 김밥
	if( false == bOnPlayerInit )
		if( IsDie() && STATE_BLOW::emBLOW_INDEX::BLOW_057 != emBlowIndex ) return -1;

	if( !m_pStateBlow ) return -1;
	int iAddSEResult = m_pStateBlow->CanAddThisBlow( pParentSkill, emBlowIndex );
	switch( iAddSEResult ) 
	{	
		case CDnStateBlow::ADD_FAIL_BY_IMMUNE:
			{
				// 실패한 이유에 따라..
				SendAddSEFail( iAddSEResult, emBlowIndex );
				return -1;
			}
			break;

		case CDnStateBlow::ADD_FAIL_BY_DIED:
			OutputDebug( "[Add State Effect Failed by die]\n" );
			return -1;

		case CDnStateBlow::ADD_FAIL_BY_INVINCIBLE:
			OutputDebug( "[Add State Effect Failed by Invincible]\n" );
			return -1;

		case CDnStateBlow::ADD_FAIL_BY_REVIVAL:
			OutputDebug( "[Add State Effect Failed by Revival blow!]\n" );
			return -1;
		case CDnStateBlow::ADD_FAIL_BY_GUILDBLOW_PRIORITY:
			return -1;
		case CDnStateBlow::ADD_FAIL_BY_PROB_SKILL_INVINCIBLE:
			OutputDebug( "[Add State Effect Failed by Prob Skill Invincible]\n" );
			return -1;
		case CDnStateBlow::ADD_FAIL_BY_COMBOLIMITBLOW:
			OutputDebug( "[Add State Effect Failed by ComboLimitBlow]\n" );
			return -1;
	}

	//속도를 변경하는 효과 적용될때 Accel효과 이미 적용 되어 있다면
	//Accel효과는 제거(STATE_END?)된다..
	switch(emBlowIndex)
	{
	case STATE_BLOW::BLOW_076:
	case STATE_BLOW::BLOW_025:
	case STATE_BLOW::BLOW_144:
		{
			if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_220))
			{
				DNVector(DnBlowHandle) vlhFrameBlows;
				GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_220, vlhFrameBlows );
				for( int i = 0; i < (int)vlhFrameBlows.size(); ++i )
				{
					DnBlowHandle hBlow = vlhFrameBlows.at(i);
					if (hBlow)
						hBlow->SetState(STATE_BLOW::STATE_END);
				}
			}
		}
		break;
	}

#if defined(PRE_ADD_50923)
	//240상태효과는 하나씩만 적용되어야 한다. 새로운 240번 상태효과가 들어 오면 기존거
	//제거 하도록한다.
	if (emBlowIndex == STATE_BLOW::BLOW_240)
	{
		if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_240))
		{
			DNVector(DnBlowHandle) vlhFrameBlows;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_240, vlhFrameBlows );
			for( int i = 0; i < (int)vlhFrameBlows.size(); ++i )
			{
				DnBlowHandle hBlow = vlhFrameBlows.at(i);
				if (hBlow)
					hBlow->SetState(STATE_BLOW::STATE_END);
			}
		}
	}
#endif // PRE_ADD_50923

	DnBlowHandle hBlow = m_pStateBlow->CreateStateBlow( GetMySmartPtr(), pParentSkill, emBlowIndex, nDurationTime, szParam );
	if( !hBlow ) 
		return -1;

	// 확률이 있는 상태효과는 확률에 걸리면 상태효과 안 걸림.
	// 클라쪽에도 왜 상태효과 추가가 실패했는지 이유는 알아야 하므로 이유를 패킷으로 보내줌.
	if( bCheckCanBegin && (false == hBlow->CanBegin()) )
	{
		SendAddSEFail( CDnStateBlow::ADD_FAIL_BY_PROBABILITY, emBlowIndex );
		//hBlow->Release();

#if defined(PRE_FIX_49208)
		//#49208 이미 적용된 상태효과가 OnBegin이 호출 된 상태에서 Duplicated된 상태효과를 여기에서 바로 Delete해 버리면 reference카운트가 어긋나게 됨.
		if (hBlow->IsDuplicated() == false)
#endif // PRE_FIX_49208
			SAFE_RELEASE_SPTR( hBlow );

		OutputDebug( "[Add State Effect Failed by probability fail!] BlowIndex: %d\n", emBlowIndex );
		return -1;
	}
	else
	{
		if( hBlow->IsDuplicated() )
			return hBlow->GetBlowID();
		else
		{
			hBlow->SetParentSkillInfo( pParentSkill );
			//hBlow->SetParentSkillID( iParentSkillID );
			//hBlow->SetParentSkillLevelID( iParentSkillLevelID );

			// 무적 상태효과 들어가면 독, 불 상태효과는 제거한다..
			if( hBlow->GetBlowIndex() == STATE_BLOW::BLOW_099 )
			{
				// 화상과 중독 상태효과를 제거.
				if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_042 ) ||
					IsAppliedThisStateBlow( STATE_BLOW::BLOW_044 ) )
				{
					DNVector(DnBlowHandle) vlBlows;
					GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_042, vlBlows );
					GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_044, vlBlows );
					for( int i = 0; i < (int)vlBlows.size(); ++i )
					{
						DnBlowHandle hBlow = vlBlows.at( i );
						int iBlowID = hBlow->GetBlowID();

						CmdRemoveStateEffectFromID( iBlowID );
					}
				}
			}

			hBlow->SetEternity( bEternity );

			return m_pStateBlow->AddStateBlow( hBlow );
		}
	}
}

void CDnActor::DelStateBlow( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	if( !m_pStateBlow ) return;
	m_pStateBlow->RemoveStateBlowByBlowDefineIndex( emBlowIndex );
}

int CDnActor::RemoveStateBlowByBlowDefineIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	if( !m_pStateBlow )	return -1;
	return m_pStateBlow->RemoveStateBlowByBlowDefineIndex( emBlowIndex );
}

void CDnActor::RemoveStateBlowFromID( int nStateBlowID )
{
	if( !m_pStateBlow ) return;
	m_pStateBlow->RemoveStateBlowFromID( nStateBlowID );
}

void CDnActor::RemoveAllBlow()
{
	DNVector(DnBlowHandle) vlAppliedStateBlow;
	GetAllAppliedStateBlow( vlAppliedStateBlow );
	int iNumStateBlow = (int)vlAppliedStateBlow.size();
	for( int iBlow = 0; iBlow < iNumStateBlow; ++iBlow )
		m_pStateBlow->RemoveStateBlowFromID( vlAppliedStateBlow.at( iBlow )->GetBlowID() );

	ApplyPassiveSkills();
}

void CDnActor::RemoveAllBlowExpectPassiveSkill()
{
	DNVector(DnBlowHandle) vlAppliedStateBlow;
	GetAllAppliedStateBlow( vlAppliedStateBlow );
	int iNumStateBlow = (int)vlAppliedStateBlow.size();
	for( int iBlow = 0; iBlow < iNumStateBlow; ++iBlow )
	{
		DnBlowHandle hBlow = vlAppliedStateBlow.at( iBlow );
		if( !hBlow )
			continue;
		if( bIsPassiveSkill( hBlow ) )
			continue;

		// 근원 아이템의 확률 스킬 무시 상태효과는 죽어도 남아있어야 한다.
		if( IsShouldRemainBlowWhenDie( hBlow ) )
			continue;

		m_pStateBlow->RemoveStateBlowFromID( hBlow->GetBlowID() );
	}
}

bool CDnActor::IsShouldRemainBlowWhenDie( DnBlowHandle hBlow )
{
	bool bResult = false;
	
	if( hBlow )
	{
		if( hBlow->IsFromSourceItem() )
			return true;

		if( hBlow->IsEternity() )
			return true;
	}

	return bResult;
}

int CDnActor::GetNumAppliedStateBlow( void )
{ 
	return m_pStateBlow->GetNumStateBlow();
}

DnBlowHandle CDnActor::GetAppliedStateBlow( int iIndex )
{ 
	return m_pStateBlow->GetStateBlow( iIndex ); 
}

bool CDnActor::IsAppliedThisStateBlow( STATE_BLOW::emBLOW_INDEX iBlowIndex )
{
	if( !m_pStateBlow )
		return false;

	return m_pStateBlow->IsApplied( iBlowIndex );
}


void CDnActor::GetAllAppliedStateBlow(DNVector(DnBlowHandle)& /*IN OUT*/ out)
{
	int nSize = m_pStateBlow->GetNumStateBlow();
	if( 0 < nSize ) out.reserve( out.capacity() + nSize ); // 20080916 성능개선
	for ( int i = 0 ; i < nSize ; i++ )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);
		if( hBlow && STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			out.push_back(hBlow);
	}

}

void CDnActor::GetAllAppliedStateBlowBySkillID(int nSkillID, DNVector(DnBlowHandle)& /*IN OUT*/ out)
{
	int nSize = m_pStateBlow->GetNumStateBlow();
	if( 0 < nSize ) out.reserve( out.capacity() + nSize ); // 20080916 성능개선

	for ( int i = 0 ; i < nSize ; i++ )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);
		if( hBlow )
		{
			const CDnSkill::SkillInfo* pSkillinfo = hBlow->GetParentSkillInfo();
			if( pSkillinfo && pSkillinfo->iSkillID == nSkillID &&
				STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			{
				out.push_back(hBlow);
			}
		}
	}

}


void CDnActor::GatherAppliedStateBlowByBlowIndex( STATE_BLOW::emBLOW_INDEX BlowIndex, DNVector(DnBlowHandle)& /*IN OUT*/ out )
{
	int iNumStateBlow = m_pStateBlow->GetNumStateBlow();
	for( int i = 0; i < iNumStateBlow; ++i )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow( i );
		if( hBlow && hBlow->GetBlowIndex() == BlowIndex &&
			STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			out.push_back( hBlow );
	}
}



void CDnActor::OnSkillProjectile( CDnProjectile *pProjectile )
{
	MASkillUser::OnSkillProjectile( pProjectile );

	if( pProjectile )
	{
		if( IsProcessSkill()  || IsEnabledToggleSkill() )
		{		
			DnSkillHandle hSkill;
			if( IsProcessSkill() )
				hSkill = m_hProcessSkill;
			else
			if( IsEnabledToggleSkill() )
				hSkill = GetEnabledToggleSkill();

			_ASSERT( hSkill && "프로젝타일을 쏜 스킬이 없다는?" );

			pProjectile->SetParentSkill( hSkill );
			
			for( DWORD i = 0; i < hSkill->GetStateEffectCount(); i++ )
			{
				CDnSkill::StateEffectStruct *pStruct = hSkill->GetStateEffectFromIndex(i);
				if( pStruct->ApplyType == CDnSkill::ApplySelf ) continue;
				pProjectile->AddStateEffect( *pStruct );
			}

			if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_167 ) )
			{
				DNVector(DnBlowHandle) vlBlows;
				GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_167, vlBlows );
				if( false == vlBlows.empty() )
				{
					int iForceSkillLevel = (int)vlBlows.front()->GetFloatValue();
					pProjectile->SetSummonMonsterForceSkillLevel( iForceSkillLevel );
				}
			}

			//////////////////////////////////////////////////////////////////////////
			//애어로 샤워 스킬에 사용될 상태효과 처리를 위해서 발사체가 생성 될때 현재 스킬이 시전 중인 경우 해당 스킬에 발사체를 담아 놓는다.
			//현재 사용중인 스킬이 242상태효과를 가지고 있는 스킬이면
			if (hSkill)
			{
				bool bExistState = false;
				int nStateEffectCount = hSkill->GetStateEffectCount();
				for (int i = 0; i < nStateEffectCount; ++i)
				{
					CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
					if (pSE && (STATE_BLOW::emBLOW_INDEX)pSE->nID == STATE_BLOW::BLOW_242)
					{
						bExistState = true;
						break;
					}
				}

				if (bExistState == true)
				{
					LOCAL_TIME skillStartTime = hSkill->GetSkillStartTime();
					pProjectile->SetSkillStartTime(skillStartTime);	//발사체에도 스킬 시작 시간을 설정 해놓는다.
					hSkill->AddProjectile(pProjectile);
				}
			}
		}

#if defined(PRE_FIX_68645)
		//129번 상태효과가 있고, 강제 속성 설정이 있는 경우 발사체에 설정 해 놓는다.
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
				
				if (pChangeActionSetBlow->IsEnable() == true)
				{
					CDnChangeActionStrProcessor* pProcessor = pChangeActionSetBlow->GetChangeActionStrProcessor();	// 액션 변경 발현타입이 비활성화 상태일때는 NULL 리턴됨.
					if( pProcessor && m_vlForceSettedHitElement.empty() == false)
					{	
						ElementEnum forceElement = (ElementEnum)m_vlForceSettedHitElement[0];

						pProjectile->SetForceHitElement(forceElement);
					}
				}
			}
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // PRE_FIX_68645
	}
}

void CDnActor::OnInitializeNextStage( void )
{
	// 진행중인 패시브 액션 스킬은 꺼버림
	if( m_hProcessSkill )
	{
		if( (m_hProcessSkill->GetSkillType() == CDnSkill::Passive || m_hProcessSkill->GetSkillType() == CDnSkill::AutoPassive) &&
			 m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
		{
			m_hProcessSkill->OnEnd( 0, 0.0f );
			m_hProcessSkill.Identity();
		}
	}

	// 진행중인 오라 스킬 역시 꺼버림
	if( m_hAuraSkill )
	{
		OnSkillAura( m_hAuraSkill, false );
	}

	// 결빙 상태효과 걸려 있는채로 이동되는 경우는 거의 없겠지만 만약 그렇다면 제거함.
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_041 ) )
		m_pStateBlow->RemoveStateBlowByBlowDefineIndex( STATE_BLOW::BLOW_041 );

	RemoveNonAvailableStateBlow();

	// Note: 맵 이동될 때 호출됨. 마지막으로 때렸던 놈들 리스트를 이곳에서 초기화해주지 않으면 이전 스테이지에서
	// 때렸던 애들이 남아있을 수 있다. 그런 상황에서 Hit 프로세스가 함 돌아가면 뻑남.
	m_hVecLastHitList.clear();
	m_hVecLastHitListByRemainTime.clear();

	m_iCantMoveReferenceCount = 0;
	m_iCantActionReferenceCount = 0;

	// 마지막 타격된 히트시그널 관련 정보 업데이트
	m_mapLastDamageTime.clear();
	m_mapLastHitUniqueID.clear();
	m_nLastDamageHitterActionIndex = 0;

	RefreshState();
}

// 여기 함수 내용이 바뀌면 클라이언트에서도 SC_START_PVPROUND 패킷 처리부분 변경되어야한다.
void CDnActor::OnInitializePVPRoundRestart( void )
{
	// 진행중인 패시브 액션이 있다면 종료.
	if( m_hProcessSkill )
	{
		if( (m_hProcessSkill->GetSkillType() == CDnSkill::Passive || m_hProcessSkill->GetSkillType() == CDnSkill::AutoPassive) &&
			m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
		{
			m_hProcessSkill->OnEnd( 0, 0.0f );
			m_hProcessSkill.Identity();
		}
	}

    // 진행중인 AutoPassive 스킬 종료.
	EndAutoPassiveSkill( CDnActionBase::m_LocalTime, 0.0f );

	// 아이템스킬 종료
	if( m_hItemSkill )
	{
		if( m_hItemSkill->IsFinished() )
		{
			m_hItemSkill->OnEnd( 0, 0.f );

			deque<DnSkillHandle>::iterator iter = find( m_dqhItemSkillList.begin(), m_dqhItemSkillList.end(), m_hItemSkill );
			_ASSERT( m_dqhItemSkillList.end() != iter );
			m_dqhItemSkillList.erase( iter );

			SAFE_RELEASE_SPTR( m_hItemSkill );
		}
	}

	// 진행중인 오라 스킬이 있다면 종료.
	if( m_hAuraSkill )
	{
		OnSkillAura( m_hAuraSkill, false );
	}

	// 진행중인 토글 스킬이 있다면 종료.
	if( m_hToggleSkill )
	{
		OnSkillToggle( m_hToggleSkill, false );
	}

	// 모든 상태효과 리셋
	RemoveAllBlowExpectPassiveSkill();

	if( IsPlayerActor() )
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(this);
		pPlayer->RemoveAllBubbles( false );
	}


	m_hVecLastHitList.clear();
	m_hVecLastHitListByRemainTime.clear();

	// 마지막 때렸던 히트시그널 정보 업데이트
	m_mapLastDamageTime.clear();
	m_mapLastHitUniqueID.clear();
	m_nLastDamageHitterActionIndex = 0;

	for( std::list<DnMonsterActorHandle>::iterator itor=m_listSummonMonster.begin() ; itor!=m_listSummonMonster.end() ; itor++ )
	{
		DnMonsterActorHandle hMonster = *itor;
		if( hMonster ) hMonster->CmdSuicide( false, false );
	}
	SAFE_DELETE_VEC( m_listSummonMonster );

	std::list<DnMonsterActorHandle> suicideMonsterList;

	map<int, list<DnMonsterActorHandle> >::iterator iter = m_mapSummonMonsterByGroup.begin();
	for( iter; iter != m_mapSummonMonsterByGroup.end(); ++iter )
	{	
		// 제한 시간 다 되어 혼자 사라진 소환 몬스터는 리스트에 invalid한 핸들만 남아 있으므로 감안.
		list<DnMonsterActorHandle>& listSummonedMonsters = iter->second;
		list<DnMonsterActorHandle>::iterator iterList = listSummonedMonsters.begin();
		for( iterList; iterList != listSummonedMonsters.end(); )
		{
			if( *iterList )
			{
				DnMonsterActorHandle hMonster = *iterList;
				//CmdSuicide함수 내부에서 PlayerActor::OnDieSummonedMonster호출됨.
				//이 함수에서 m_mapSummonMonsterByGroup/listSummonMonster리스트에서 제거함.
				//그래서 CmdSuicide될 녀석들을 리스트에 담아서 아래쪽에서 처리 하도록 함..
				//hMonster->CmdSuicide( false, false );
				suicideMonsterList.push_back(hMonster);

				++iterList;
			}
			else
			{
				iterList = listSummonedMonsters.erase( iterList );
			}

			
		}
	}

	//위에서 리스트에 담아 놓은 CmdSuicide될 녀석들 처리한다.
	for( std::list<DnMonsterActorHandle>::iterator itor=suicideMonsterList.begin() ; itor!=suicideMonsterList.end() ; itor++ )
	{
		DnMonsterActorHandle hMonster = *itor;
		if( hMonster ) hMonster->CmdSuicide( false, false );
	}

	m_mapSummonMonsterByGroup.clear();
}


void CDnActor::ResetStateEffect( int nSkillDurationType )
{
	m_pStateBlow->ResetStateBlowBySkillType( nSkillDurationType );
}


void CDnActor::ProcessPress()
{
	if( m_nPressLevel == 0 ) return;
	if( IsDie() ) return;
	if( IsFly() ) return;
	if( IsUnderGround() ) return;

	DNVector(DnActorHandle) hVecList;
	if( m_iLastProcessPressCount > 0 )
		hVecList.reserve( m_iLastProcessPressCount );
	EtVector3 vAniDist = *GetPosition();
	vAniDist.x -= GetAniDistance()->x;
	vAniDist.z -= GetAniDistance()->z;

	float fScanRadius = 0.0f;
	if( m_Press == Press_Circle ) fScanRadius = (float)GetUnitSize() + 50.f;
	else if( m_Press == Press_Capsule ) fScanRadius = (float)GetUnitSize()*2.0f + 50.f;

	int nCount = ScanActor( GetRoom(), vAniDist, fScanRadius, hVecList );
	if( nCount > 0 ) {
		for( int i=0; i<nCount; i++ ) {
			if( hVecList[i] == GetMySmartPtr() ) continue;
			if( hVecList[i]->IsDie() ) continue;
			if( hVecList[i]->GetPressLevel() > m_nPressLevel ) continue;
			if( hVecList[i]->GetPressLevel() == -1 ) continue;
			if( m_Press == Press_NoneSameCircle && hVecList[i]->m_Press == Press_NoneSameCircle ) continue;

			ProcessPress( hVecList[i] );
		}
		m_iLastProcessPressCount = nCount;
	}
}

void CDnActor::ProcessPress( DnActorHandle hTarget )
{
	EtVector2 Dir;
	float Dist = 0.0f;
	bool bCheckPress = false;
	if( GetPress() == CDnActorState::Press_Circle && hTarget->GetPress() == CDnActorState::Press_Circle ) {
		if( CheckPressCircle2Clrcle2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			bCheckPress = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Capsule && hTarget->GetPress() == CDnActorState::Press_Capsule ) {
		if( CheckPressCapsule2Capsule2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			bCheckPress = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Capsule && hTarget->GetPress() == CDnActorState::Press_Circle ) {
		if( CheckPressCapsule2Circle2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			bCheckPress = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Circle && hTarget->GetPress() == CDnActorState::Press_Capsule ) {
		if( CheckPressCapsule2Circle2( hTarget, GetMySmartPtr(), Dir, Dist ) ) {
			hTarget->MoveToWorld( -Dir * Dist );
			bCheckPress = true;
		}
	}

	if( bCheckPress ) {
		if( hTarget->IsPlayerActor() ) {
			CDnPlayerActor *pPlayer = (CDnPlayerActor *)hTarget.GetPointer();
			if( pPlayer->GetPlayerSpeedHackChecker() )
				((CDnPlayerSpeedHackChecker*)pPlayer->GetPlayerSpeedHackChecker())->SetCheckPress( true, Dist );
		}
	}
}

bool CDnActor::CheckPressCircle2Clrcle2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist )
{
	EtVector2 vDir;
	int nSumSize;
	vDir = EtVec3toVec2( *hActor2->GetPosition() - *hActor1->GetPosition() );
	nSumSize = hActor1->GetUnitSize() + hActor2->GetUnitSize();
	float fLength = D3DXVec2Length( &vDir );
	if( fLength > (float)nSumSize ) return false;

	D3DXVec2Normalize( &Dir, &vDir );
	Dist = (float)( nSumSize - fLength );
	return true;
}

bool CDnActor::CheckPressCapsule2Capsule2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist )
{
	// 자신의 월드상 캡슐영역 구하고(직선)
	int nSizeX = hActor1->GetUnitSizeParam1();
	int nSizeZ = hActor1->GetUnitSizeParam2();
	EtVector3 vCapsule1, vCapsule2;
	int nSumSize = 0;
	if( nSizeX < nSizeZ ) {
		vCapsule1.x = vCapsule2.x = vCapsule1.y = vCapsule2.y = 0.0f;
		vCapsule1.z = (float)(nSizeZ - nSizeX);
		vCapsule2.z = -vCapsule1.z;
		nSumSize += nSizeX;
	}
	else {
		vCapsule1.z = vCapsule2.z = vCapsule1.y = vCapsule2.y = 0.0f;
		vCapsule1.x = (float)(nSizeX - nSizeZ);
		vCapsule2.x = -vCapsule1.x;
		nSumSize += nSizeZ;
	}
	EtMatrix matWorld = *hActor1->GetMatEx();
	EtVec3TransformCoord( &vCapsule1, &vCapsule1, &matWorld );
	EtVec3TransformCoord( &vCapsule2, &vCapsule2, &matWorld );
	EtVector2 vLine1, vLine2;
	vLine1 = EtVec3toVec2( vCapsule1 );
	vLine2 = EtVec3toVec2( vCapsule2 );

	// 타겟의 월드상 캡슐영역 구하고(직선)
	int nTargetSizeX = hActor2->GetUnitSizeParam1();
	int nTargetSizeZ = hActor2->GetUnitSizeParam2();
	EtVector3 vTargetCapsule1, vTargetCapsule2;
	if( nTargetSizeX < nTargetSizeZ ) {
		vTargetCapsule1.x = vTargetCapsule2.x = vTargetCapsule1.y = vTargetCapsule2.y = 0.0f;
		vTargetCapsule1.z = (float)(nTargetSizeZ - nTargetSizeX);
		vTargetCapsule2.z = -vTargetCapsule1.z;
		nSumSize += nTargetSizeX;
	}
	else {
		vTargetCapsule1.z = vTargetCapsule2.z = vTargetCapsule1.y = vTargetCapsule2.y = 0.0f;
		vTargetCapsule1.x = (float)(nTargetSizeX - nTargetSizeZ);
		vTargetCapsule2.x = -vTargetCapsule1.x;
		nSumSize += nTargetSizeZ;
	}
	EtMatrix matTargetWorld = *hActor2->GetMatEx();
	EtVec3TransformCoord( &vTargetCapsule1, &vTargetCapsule1, &matTargetWorld );
	EtVec3TransformCoord( &vTargetCapsule2, &vTargetCapsule2, &matTargetWorld );
	EtVector2 vTargetLine1, vTargetLine2;
	vTargetLine1 = EtVec3toVec2( vTargetCapsule1 );
	vTargetLine2 = EtVec3toVec2( vTargetCapsule2 );

	if( hActor1 == hActor2 ) return false;

	// 직선과 직선 거리를 구한 후
	EtVector2 vPressDir;
	float fLength = DistLine2DToLine2D( vLine1, vLine2, vTargetLine1, vTargetLine2, vPressDir );
	if( fLength > 0.0f ) {
		// 교차하지 않는다면, PressDir이 제대로 들어있을 것이다.
		// 그리고 이땐 일반적인 점과 직선의 거리기 때문에 SumSize와 비교판단을 수행한다.
		if( fLength > (float)nSumSize ) return false;
		Dir = vPressDir;
		Dist = (float)( nSumSize - fLength );
	}
	else {
		// 교차하면
		// 첫번째 선분과 두번째 선분의 양 끝점을 가지고 거리를 잰 후
		EtVector2 vDir1, vDir2;
		float fDist1 = DistPointToLine2D( vTargetLine1, vLine1, vLine2, vDir1 );
		float fDist2 = DistPointToLine2D( vTargetLine2, vLine1, vLine2, vDir2 );

		// 거리가 짧은 것을 저장. 방향은 반대로 저장.
		if( fDist1 < fDist2 ) {
			vPressDir = -vDir1;
			fLength = fDist1;
		}
		else {
			vPressDir = -vDir2;
			fLength = fDist2;
		}

		// 교차한다면 SumSize와의 거리비교는 필요없다.
		// 이미 교차될정도로 겹쳤다면, 최소거리밖으로 밀어내는 방법 외엔 없다.
		Dir = vPressDir;
		Dist = (float)( nSumSize + fLength );
	}

	return true;
}

bool CDnActor::CheckPressCapsule2Circle2( DnActorHandle hActor1, DnActorHandle hActor2, EtVector2 &Dir, float &Dist )
{
	int nSizeX = hActor1->GetUnitSizeParam1();
	int nSizeZ = hActor1->GetUnitSizeParam2();
	EtVector3 vCapsule1, vCapsule2;
	int nSumSize;
	if( nSizeX < nSizeZ ) {
		vCapsule1.x = vCapsule2.x = vCapsule1.y = vCapsule2.y = 0.0f;
		vCapsule1.z = (float)(nSizeZ - nSizeX);
		vCapsule2.z = -vCapsule1.z;
		nSumSize = nSizeX + hActor2->GetUnitSize();
	}
	else {
		vCapsule1.z = vCapsule2.z = vCapsule1.y = vCapsule2.y = 0.0f;
		vCapsule1.x = (float)(nSizeX - nSizeZ);
		vCapsule2.x = -vCapsule1.x;
		nSumSize = nSizeZ + hActor2->GetUnitSize();
	}
	EtMatrix matWorld = *hActor1->GetMatEx();
	EtVec3TransformCoord( &vCapsule1, &vCapsule1, &matWorld );
	EtVec3TransformCoord( &vCapsule2, &vCapsule2, &matWorld );
	EtVector2 vLine1, vLine2;
	vLine1 = EtVec3toVec2( vCapsule1 );
	vLine2 = EtVec3toVec2( vCapsule2 );

	EtVector2 vTargetPos = EtVec3toVec2( *hActor2->GetPosition() );
	EtVector2 vPressDir;
	float fLength = DistPointToLine2D( vTargetPos, vLine1, vLine2, vPressDir );
	if( fLength > (float)nSumSize ) return false;

	Dir = vPressDir;
	Dist = (float)( nSumSize - fLength );
	return true;
}

bool CDnActor::bIsObserver()
{
	if( GetGameRoom() && GetGameRoom()->bIsPvPRoom() )
	{
		if( GetTeam() == PvPCommon::Team::Observer )
			return true;
	}

	return false;
}

float CDnActor::SquaredDistance( const EtVector3& vPos, const SAABox& BBox, bool bNear )
{
	float fSQDist = 0.0f;

	for( int i = 0; i < 3; ++i )
	{
		// y 축은 무시
		if( 1 == i )
			continue;

		float v = vPos[ i ];
		
		if( bNear ) {
			if( v < BBox.Min[ i ] ) 
				fSQDist += (BBox.Min[ i ] - v) * (BBox.Min[ i ] - v);
			
			if( v > BBox.Max[ i ] )
				fSQDist += (v - BBox.Max[ i ]) * (v - BBox.Max[ i ]);
		}
		else {
			if( v > BBox.Min[ i ] ) 
				fSQDist += (BBox.Min[ i ] - v) * (BBox.Min[ i ] - v);

			if( v < BBox.Max[ i ] )
				fSQDist += (v - BBox.Max[ i ]) * (v - BBox.Max[ i ]);
		}
	}

	return fSQDist;
}


void CDnActor::CmdChangeTeam( int nTeam )
{
	SetTeam( nTeam );

	printf("CDnActor::CmdChangeTeam %d\n",nTeam);

	char pBuffer[32];
	CPacketCompressStream Stream( pBuffer, sizeof(pBuffer) );
	Stream.Write( &m_nTeam, sizeof(int) );

	Send( eActor::SC_CMDCHANGETEAM, &Stream );
}

void CDnActor::OnHitFinish( LOCAL_TIME LocalTime, HitStruct *pStruct )
{
	if( !pStruct->vSelfVelocity || !pStruct->vSelfResistance ) return;
	if( GetVelocity() == NULL || GetResistance() == NULL ) return;
	if( EtVec3LengthSq( pStruct->vSelfVelocity ) == 0.f && EtVec3LengthSq( pStruct->vSelfResistance ) == 0.f ) return;

	EtVector3 vVelocity = *pStruct->vSelfVelocity;
	EtVector3 vResistance = *pStruct->vSelfResistance;

	// y 값이 기존에 있는데 여기서 덮어씌우면 문제가 생길수있으므로 y 는 보정해준다.
	if( GetVelocity()->y != 0.f && pStruct->vSelfVelocity->y == 0.f ) {
		vVelocity.y = GetVelocity()->y;
		vResistance.y = GetResistance()->y;
	}

	CmdForceVelocity( vVelocity, vResistance );
}

void CDnActor::CmdForceVelocity( EtVector3 &vVelocity, EtVector3 &vResistance )
{
	SetVelocity( vVelocity );
	SetResistance( vResistance );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( ( GetVelocity() ) ? GetVelocity() : &EtVector3( 0.f, 0.f, 0.f ), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
	Stream.Write( ( GetResistance() ) ? GetResistance() : &EtVector3( 0.f, 0.f, 0.f ), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

	Send( eActor::SC_CMDFORCEVELOCITY, &Stream );
}

void CDnActor::CmdChatBalloon( LPCWSTR wszMessage )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	int nLength = static_cast<int>(wcslen( wszMessage ) * 2);
	if( nLength > 120 ) nLength = 120;

	Stream.Write( &nLength, sizeof(int) );
	Stream.Write( wszMessage, nLength );

	Send( eActor::SC_CMDCHATBALLOON, &Stream );
}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDnActor::CmdChatBalloon( int nUIStringIndex )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nUIStringIndex, sizeof(int) );	

	Send( eActor::SC_CMDCHATBALLOON_AS_INDEX, &Stream );
}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

void CDnActor::CmdShowExposureInfo( bool bShow )
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &bShow, sizeof(bool) );

	Send( eActor::SC_SHOWEXPOSUREINFO, &Stream );
}

void CDnActor::CmdForceAddSkill( int nSkillID )
{
	AddSkill( nSkillID );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nSkillID, sizeof(int) );
	Send( eActor::SC_FORCE_ADD_SKILL, &Stream );
}

void CDnActor::CmdForceRemoveSkill( int nSkillId )
{
	RemoveSkill( nSkillId );

	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );

	Stream.Write( &nSkillId, sizeof(int) );
	Send( eActor::SC_FORCE_REMOVE_SKILL, &Stream );
}

void CDnActor::OnProjectile( CDnProjectile *pProjectile, ProjectileStruct* pStruct, MatrixEx& LocalCross, int nSignalIndex )
{
	int iShootActionIndex = GetElementIndex( GetCurrentAction() );

	if( -1 != iShootActionIndex )
		pProjectile->SetShootActionIndex( iShootActionIndex );

	pProjectile->SetSignalArrayIndex( nSignalIndex );

	boost::shared_ptr<ProjectileStruct> pProjectileStruct = boost::shared_ptr<ProjectileStruct>(new ProjectileStruct);
#ifdef PRE_FIX_MEMOPT_SIGNALH
	CopyShallow_ProjectileStruct(*pProjectileStruct, pStruct);
#else
	*pProjectileStruct = *pStruct;
#endif

	pProjectile->SetProjectileSignal( pProjectileStruct );
}

void CDnActor::CmdSyncBlow( CDNUserSession* pGameSession )
{
	for( int i=0 ; i<GetNumAppliedStateBlow() ; ++i )
	{
		DnBlowHandle hBlow = GetAppliedStateBlow( i );
		if( hBlow )
		{
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if( !pSkillInfo )
			{
				_DANGER_POINT();
				continue;
			}

			// #24143 패시브 버프 스킬의 상태효과인 경우엔 동기를 맞추지 않습니다.
			// 패시브 스킬은 클라이언트가 난입했을 때 받게 되는 다른 유저들의 스킬리스트를 
			// 초기화 할때 자동으로 적용됩니다.
			if( CDnSkill::Passive == pSkillInfo->eSkillType &&
				CDnSkill::Buff == pSkillInfo->eDurationType &&
				CDnSkill::ApplySelf == pSkillInfo->eTargetType )
			{
				continue;
			}

			const CPacketCompressStream* pPacket = hBlow->GetPacketStream();
			Send( pGameSession, eActor::SC_CMDADDSTATEEFFECT, GetUniqueID(), const_cast<CPacketCompressStream*>(pPacket) );
		}
	}	
}

void CDnActor::OnDie( DnActorHandle hHitter )
{
	m_fDieDelta = m_fMaxDieDelta;
}

void CDnActor::UseMP( int iMPDelta )
{
	int iResult = m_pStateBlow->OnUseMP( iMPDelta );
	SetSP( GetSP() + iResult );

	RequestHPMPDelta( ElementEnum::ElementEnum_Amount, iResult, UINT_MAX, true );
}

void CDnActor::RequestDamageFromStateBlow( DnBlowHandle hFromBlow, int iDamage, CDnDamageBase::SHitParam* pHitParam/* = NULL*/ )
{
	// 무적 상태효가 걸려있으면 무시.
	if( hFromBlow && false == IsAppliedThisStateBlow( STATE_BLOW::BLOW_099 ) )
	{
		// 하이랜더 상태효과가 있는 경우 죽지 않아야 한다.
		float fDamage = (float)iDamage;
		if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_143 ) )
		{
			// IsAppliedThisStateBlow() 함수에선 true 가 리턴되지만 지속시간이 다 된
			// 상태효과는 GatherAppliedStateBlowByBlowIndex() 에서 얻어와지지 않으므로 리스트가 비어있을 수 도 있다.
			DNVector( DnBlowHandle ) vlhHighLanderBlow;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_143, vlhHighLanderBlow );
			if( 0 < (int)vlhHighLanderBlow.size() )
			{
				CDnHighlanderBlow* pHighlanderBlow = static_cast<CDnHighlanderBlow*>(vlhHighLanderBlow.front().GetPointer());
				fDamage += pHighlanderBlow->CalcDamage( fDamage );
			}
		}

		CDnSkill::SkillInfo* pParentSkillInfo = const_cast<CDnSkill::SkillInfo*>(hFromBlow->GetParentSkillInfo());
		DWORD dwHitterUniqueID = 0;
		ElementEnum eElement = ElementEnum::ElementEnum_Amount;
		if( pParentSkillInfo )
		{
			dwHitterUniqueID = pParentSkillInfo->hSkillUser ? pParentSkillInfo->hSkillUser->GetUniqueID() : -1;
			eElement =  pParentSkillInfo->eSkillElement;
		}

		if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_243 ) )
		{
			float fAbsorbDamage = 0.f;
			int nAbsorbSP = 0;

			DNVector( DnBlowHandle ) vlhManaShieldBlow;
			GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_243, vlhManaShieldBlow );
			for( DWORD n=0; n<vlhManaShieldBlow.size(); n++ )
			{
				CDnManaShieldBlow* pManShieldBlow = static_cast<CDnManaShieldBlow*>(vlhManaShieldBlow[n].GetPointer());
				pManShieldBlow->CalcManaShield( fDamage , fAbsorbDamage , nAbsorbSP );
			}

			if( nAbsorbSP > GetSP() )
			{
				fAbsorbDamage -= (fAbsorbDamage * (nAbsorbSP - GetSP()) / nAbsorbSP);
				nAbsorbSP = GetSP();
			}

			if( fAbsorbDamage > fDamage )
				fAbsorbDamage = fDamage;

			fDamage -= fAbsorbDamage;
			SetSP( GetSP() -nAbsorbSP );
			RequestHPMPDelta( ElementEnum::ElementEnum_Amount, -nAbsorbSP, UINT_MAX, true , true );
		}

#if defined(PRE_FIX_59347)
		//파츠 몬스터 데미지 보정?을 위해서
#ifdef PRE_FIX_67656
		ApplyPartsDamage(fDamage, pParentSkillInfo->hSkillUser);
#else
		if (GetApplyPartsDamage() == true)
			ApplyPartsDamage(fDamage, pParentSkillInfo->hSkillUser);
#endif

		//속성 표시
		if (pHitParam)
			eElement = pHitParam->HasElement;
#endif // PRE_FIX_59347

		SetHP( GetHP()-(INT64)fDamage );
#ifdef PRE_FIX_77172
		if( hFromBlow->GetBlowIndex() == STATE_BLOW::BLOW_042 )
			RequestHPMPDelta( eElement, -(INT64)fDamage, dwHitterUniqueID, false, true, pHitParam, true );
		else
			RequestHPMPDelta( eElement, -(INT64)fDamage, dwHitterUniqueID, false, true, pHitParam );
#else // PRE_FIX_77172
		RequestHPMPDelta( eElement, -(INT64)fDamage, dwHitterUniqueID, false, true, pHitParam );
#endif // PRE_FIX_77172

#ifdef PRE_ADD_EXPORT_DPS_INFORMATION
		if( CDnDPSReporter::IsActive() && pParentSkillInfo && pParentSkillInfo->hSkillUser && pParentSkillInfo->hSkillUser->IsPlayerActor() )
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>( pParentSkillInfo->hSkillUser.GetPointer());

			if(CDnDPSReporter::GetInstance().IsEnabledUser( pPlayerActor->GetCharacterDBID() ))
			{
				CDnDPSReporter::GetInstance().StartReport();
				DNVector(DnActorHandle) hVecList;
				ScanActor( GetRoom(), *GetActorHandle()->GetPosition() , 500.f , hVecList );
				CDnDPSReporter::GetInstance().ApplyAreaMonsterCount( (int)hVecList.size() );
				CDnDPSReporter::GetInstance().ApplyDotDamageData( (int)fDamage , GetActorHandle() , hFromBlow );
				CDnDPSReporter::GetInstance().EndReport();
			}
		}
#endif

	}
}

void CDnActor::CmdFreezingPrisonDurablity(int nStateBlowID, float fDuabilityRate, bool bShowGauge)
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	Stream.Write( &nStateBlowID, sizeof(nStateBlowID) );
	Stream.Write( &fDuabilityRate, sizeof(fDuabilityRate) );
	Stream.Write( &bShowGauge, sizeof(bShowGauge) );

	Send( eActor::SC_FREEZINGPRISON_DURABILITY, &Stream );
}

void CDnActor::SendRemoveStateEffectFromID( int nID )
{
	char acBuffer[ 32 ];
	CPacketCompressStream Stream( acBuffer, sizeof(acBuffer) );
	Stream.Write( &nID, sizeof(nID) );

	Send( eActor::SC_CMDREMOVESTATEEFFECTFROMID, &Stream );
}

void CDnActor::RemoveResetStateBlow()
{
	// [2010/12/15 semozz]
	// 이 함수 호출후에 새로운 상태 효과들로 대체되어야 하기때문에
	// 여기서는 바로 상태 효과들을 제거 해야한다.

	std::map<int, int>::iterator iter = m_vlStateBlowIDToRemove.begin();
	for (; iter != m_vlStateBlowIDToRemove.end(); ++iter)
	{
		//즉시 상태효과 제거 하고
		m_pStateBlow->RemoveImediatlyStateEffectFromID(iter->second);
	
		//클라이언트로 상태효과 제거 패킷 보냄.
		SendRemoveStateEffectFromID(iter->second);
	}

	m_vlStateBlowIDToRemove.clear();
}


void CDnActor::ForceKnockBack(DnActorHandle hHitter, CDnDamageBase::SHitParam& HitParam)
{
	m_HitParam = HitParam;
	
	int nSeed = CRandom::Seed(GetRoom());
	_srand( GetRoom(), nSeed );

	CDnWeapon::HitTypeEnum HitType = CDnWeapon::Normal;
	
	//////////////////////////////////////////////////////////////////////////
	// 경직 시간 계산
	float fStiffResult = 0.f;
	if( GetStiffResistance() > 0 ) fStiffResult = ( hHitter->GetStiff() * m_HitParam.fStiffProb ) / GetStiffResistance();
	fStiffResult *= CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffRevision );

	if( fStiffResult <= 0.f ) fStiffResult = 0.05f;
	else if( fStiffResult > CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffMax ) ) 
		fStiffResult = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::StiffMax );

	m_fStiffDelta = s_fMaxStiffTime * fStiffResult;

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 다운딜레이 랜덤값 설정
	if( !IsDown() ) {
		m_fLastDownRatio = 1.f;
	}

	float fTemp = ( 3.f * ( 1.f - GetDownDelayProb() ) ) * m_fLastDownRatio;
	int nTemp = (int)( fTemp * DOWN_DELAY_RANDOM_RANGE * 100.f );
	if( nTemp == 0 ) m_fDownDelta = fTemp;
	else m_fDownDelta = fTemp - DOWN_DELAY_RANDOM_RANGE + ( _rand(GetRoom())%( nTemp * 2 ) / 100.f );
	if( m_fDownDelta >= 3.f ) m_fDownDelta = 3.f;
	if( m_fDownDelta <= 0.1f ) m_fDownDelta = 0.01f;
	//////////////////////////////////////////////////////////////////////////

	m_HitParam.HitType = HitType;
	// 데미지 계산
	switch( HitType ) 
	{
	case CDnWeapon::Normal:
	case CDnWeapon::Critical:
	case CDnWeapon::Stun:
	case CDnWeapon::CriticalRes:
		{
			// 무게에 따라 가속도 줄여준다.
			if( GetWeight() > 0.f )
				m_HitParam.vVelocity.y /= GetWeight();
			if( GetWeight() > 1.f ) 
			{
				float fTemp = m_HitParam.vVelocity.z;
				float fTest = CalcMovement( fTemp, 1.f, FLT_MAX, FLT_MIN, m_HitParam.vResistance.z );
				if( m_HitParam.vVelocity.z * fTest > 0.f ) 
				{
					m_HitParam.vVelocity.x /= powf( 1.1f, GetWeight() );
					m_HitParam.vVelocity.z /= powf( 1.1f, GetWeight() );
				}
			}

			bool bSuccessNormalDamage = true;
			int nSuperAmmorTime = 0;

			m_HitParam.bSuccessNormalDamage = bSuccessNormalDamage;
			m_HitParam.nSuperAmmorDelay = nSuperAmmorTime;

			memset( m_nSkillSuperAmmorValue, 0, sizeof(m_nSkillSuperAmmorValue) );
			OnBreakSkillSuperAmmor( m_HitParam );

			if( bSuccessNormalDamage ) {
				CheckDamageVelocity( hHitter );

				// 맞았을때 리셋할것들 리셋
				MAMovementBase *pMovement = GetMovement();
				if( pMovement ) {
					pMovement->ResetMove();
					pMovement->ResetLook();
				}

				if( !m_HitParam.szActionName.empty() ) {
					SetActionQueue( m_HitParam.szActionName.c_str(), 0, 3.f, 0.f, false );
				}
			}
		}
		break;
	}
	
	INT64 nDamage = 0;

	RequestDamage(hHitter, nSeed, nDamage);
}

void CDnActor::RemoveNonAvailableStateBlow()
{
	if (NULL == m_pStateBlow)
		return;

	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(GetRoom());
	CDNUserSession *pUserSession = pGameRoom ? pGameRoom->GetUserSession(GetSessionID()) : NULL;

	if (!pUserSession)
		return;

	const TMapInfo* pMapData = g_pDataManager->GetMapInfo( pUserSession->GetMapIndex() );
	if (!pMapData)
		return;
	
	//월드 맵일 경우는 스킵
	if (GlobalEnum::MAP_WORLDMAP == pMapData->MapType)
		return;

	// 현재 유저가 속한 맵의 AllowType을 얻어 온다..
	int nAllowMapType = pMapData->nAllowMapType;

	
	int nSize = m_pStateBlow->GetNumStateBlow();
	for ( int i = 0 ; i < nSize ; i++ )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);

		if( hBlow && STATE_BLOW::STATE_END != hBlow->GetBlowState() )
		{
			//상태효과중 아이템 스킬에 의한 상태효과인지 확인해서
			//사용된 아아템 ID를 얻어 온다.
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if (pSkillInfo && pSkillInfo->bIsItemSkill)
			{
				int nItemID = pSkillInfo->nItemID;

				const TItemData* pItemData = g_pDataManager->GetItemData(nItemID);
				if (pItemData)
				{
					//현재 맵의 AllowMapType과 아이템의 AllowMapType 설정이 다르다면
					if ((nAllowMapType & pItemData->nAllowMapType) == 0)
					{
						CmdRemoveStateEffectFromID(hBlow->GetBlowID());
					}
				}
			}
		}
	}
}

void CDnActor::RemovedActivatedStateBlow( bool bIgnoreItemSkill )
{
	if (NULL == m_pStateBlow)
		return;

	CDNGameRoom* pGameRoom = static_cast<CDNGameRoom*>(GetRoom());
	CDNUserSession *pUserSession = pGameRoom ? pGameRoom->GetUserSession(GetSessionID()) : NULL;

	if (!pUserSession)
		return;

	int nSize = m_pStateBlow->GetNumStateBlow();
	for ( int i = 0 ; i < nSize ; i++ )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);

		if( hBlow && STATE_BLOW::STATE_END != hBlow->GetBlowState() )
		{
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if( bIgnoreItemSkill == true && pSkillInfo && pSkillInfo->bIsItemSkill )
				continue;

			if ( hBlow->IsFromSourceItem() )
				continue;

			if( hBlow->IsEternity() )
				continue;

			if ( !bIsPassiveSkill( hBlow ) )
			{
				CmdRemoveStateEffectFromID(hBlow->GetBlowID());
			}
		}
	}

}

void CDnActor::FindSkillBySkillType(CDnSkill::SkillTypeEnum eSkillType, DNVector(DnSkillHandle)& vlSkillList)
{
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) 
	{
		// 토글이나 오라 스킬이라면 별도의 리스트에서 삭제
		DnSkillHandle hSkill = m_vlhSkillList.at(i);
		if(!hSkill)
			continue;

		if (eSkillType == hSkill->GetSkillType())
			vlSkillList.push_back(hSkill);
	}
#else
	DWORD dwNumSkill = GetSkillCount();
	for( DWORD i = 0; i < dwNumSkill; ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
		if( !hSkill )
			continue;

		if( eSkillType == hSkill->GetSkillType() )
			vlSkillList.push_back( hSkill );
	}
#endif // #ifndef PRE_FIX_SKILLLIST
}

void CDnActor::OnLoopAction( float fFrame, float fPrevFrame )
{
	/*
	if( m_nActionIndex == -1 || m_nVecAniIndexList[m_nActionIndex] == -1 ) return;
	MAActorRenderBase *pRenderBase = static_cast<MAActorRenderBase *>(this);
	if( !pRenderBase ) return;

	ActionElementStruct *pStruct = GetElement( m_nActionIndex );

	EtVector3 vDist;
	pRenderBase->CalcAniDistance( m_nVecAniIndexList[m_nActionIndex], min( (float)pStruct->dwLength, fFrame ), fPrevFrame, vDist );
	pRenderBase->AddAniDistance( vDist );
	*/
}

void CDnActor::CmdUpdateStateBlow(int nBlowID)
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	DWORD dwUniqueID = GetUniqueID();
	Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
	Stream.Write( &nBlowID, sizeof(nBlowID) );

	Send( eActor::SC_CMDUPDATESTATEBLOW, &Stream );
}

void CDnActor::CmdFinishAuraSkill(DWORD nSkillID)
{
	BYTE pBuffer[128];
	CPacketCompressStream Stream( pBuffer, 128 );
	DWORD dwUniqueID = GetUniqueID();
	Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
	Stream.Write( &nSkillID, sizeof(nSkillID) );

	Send( eActor::SC_FINISH_AURASKILL, &Stream );
}


bool CDnActor::IsImmuned(STATE_BLOW::emBLOW_INDEX blowIndex)
{
	CDnStateBlow *pStateBlow = GetStateBlow();
	if (!pStateBlow)
		return false;

	return pStateBlow->IsImmuned(blowIndex);
}

void CDnActor::SetAction( const char *szActionName, float fFrame, float fBlendFrame, bool bLoop /* = false */ )
{
	CDnActionBase::SetAction( CheckChangeActionBlow( szActionName ), fFrame, fBlendFrame, bLoop );

	//동작 바뀔때 HitLimitCount정보 초기화
	m_HitLimitCountInfoList.clear();
}


const char *CDnActor::CheckChangeActionBlow( const char *szActionName )
{
	const char *szResultActionName = szActionName;
	// Move 액션도 바뀌어야할 때가 있으므로 어쩔 수 없이 SetAction 함수도 변경... ㅠㅠ
	// 129번 액션 이름 대체 상태효과 ///////////////////////////////////////////////////////////////////
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
			if( STATE_BLOW::STATE_END != pChangeActionSetBlow->GetBlowState() )
			{
				CDnChangeActionStrProcessor* pProcessor = pChangeActionSetBlow->GetChangeActionStrProcessor();
				if( pProcessor )
				{
					// 게임서버 덤프 확인용.
					string strNowActionName( szResultActionName );
					const char* pChangeActionName = pProcessor->GetChangeActionName( strNowActionName );
					if( pChangeActionName )
						szResultActionName = pChangeActionName;
				}
			}
		}
	}

	// 121번 스탠드 액션 변경 상태효과가 있는 경우. 해당 액션으로 변경해 줌. ///////////////////////////
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_121 ) )
	{
		if( 0 == strcmp(szResultActionName, "Stand") )
		{
			DNVector(DnBlowHandle) vlhChangeStandActionBlow;
			m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_121, vlhChangeStandActionBlow );
			if( false == vlhChangeStandActionBlow.empty() )
			{
				_ASSERT( 1 == vlhChangeStandActionBlow.size() );
				if( STATE_BLOW::STATE_END != vlhChangeStandActionBlow.front()->GetBlowState() )
					szResultActionName = vlhChangeStandActionBlow.front()->GetValue();
			}

		}
	}

	return szResultActionName;
}

void CDnActor::ForceBeginStateBlow(DnBlowHandle hBlow)
{
	if (hBlow)
	{
		hBlow->OnBegin( CDnActionBase::m_LocalTime, 0.0f );
		OnBeginStateBlow( hBlow );

		// 곧바로 end 로 셋팅하는 blow 도 있기 때문에 체크.
		if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			hBlow->SetState( STATE_BLOW::STATE_DURATION );
	}
}

bool CDnActor::CanUsePrefixSkill()
{
	bool isCanUse = false;

	// 평타이거나 스킬에 접두어 스킬 발동 플래그가 설정 되어 있으면 발동됨. [2011/01/31 semozz]
	// [2011/02/11 semozz]
	// 접두어 스킬 발동 플래그 체크기능 제거
	// [2011/03/14 semozz]
	// 해킹스텐스류 스킬은 ChangeStandAction상태 효과가 적용 되어 있어서 m_hProcessSkill이 없지만 스킬 사용 상태이다.
	// ChangeStandAction상태효과가 있으면 평타가 아니다..라고 일단은 판단한다..
	// 접두어 상태효과 무시하는 상태효과가 있으면 적용 안됨 [2011/03/23 semozz]

	isCanUse = (false == IsProcessSkill() && //평타
		!IsAppliedThisStateBlow(STATE_BLOW::BLOW_121) && !IsAppliedThisStateBlow(STATE_BLOW::BLOW_129) && //해킹스텐스류
		!IsAppliedThisStateBlow(STATE_BLOW::BLOW_183) //접두어 무시
		/* || IsPrefixTriggerSkill()*/);

	return isCanUse;
}

void CDnActor::AddStateBlowSignal(DnBlowHandle hBlow)
{
	if (m_pStateBlowSignalProcessor && hBlow)
		m_pStateBlowSignalProcessor->AddBlow(hBlow);
}

void CDnActor::RemoveStateBlowSignal(DnBlowHandle hBlow)
{
	if (m_pStateBlowSignalProcessor && hBlow)
		m_pStateBlowSignalProcessor->RemoveBlow(hBlow);
}

void CDnActor::ReserveStealMagicBuff(DnActorHandle hActor, CDnSkill::SkillInfo *pSkillInfo, int nDurationTime, STATE_BLOW::emBLOW_INDEX blowIndex, const char* szValue)
{
	StateBlowInfo newBlowInfo;
	newBlowInfo.blowIndex = blowIndex;
	newBlowInfo.strValue = szValue ? szValue : "";

	STEAL_MAGIC_BUFF_INFO_LIST::iterator findIter = m_StealMagicBuffAddList.find(hActor);
	if (findIter != m_StealMagicBuffAddList.end())
	{
		findIter->second.stateBlowList.push_back(newBlowInfo);
	}
	else
	{
		StealMagicBuffInfo newInfo;
		newInfo.nDurationTime = nDurationTime;
		if (pSkillInfo)
			newInfo.skillInfo = *pSkillInfo;

		newInfo.stateBlowList.push_back(newBlowInfo);

		m_StealMagicBuffAddList.insert(STEAL_MAGIC_BUFF_INFO_LIST::value_type(hActor, newInfo));
	}
}

void CDnActor::UpdateStealMagicBuff()
{
	//기존에 적용된 스틸매직에의한 상태효과가 있다면 제거하고..
	if (!m_StealMagicBuffList.empty())
	{
		//지워지는 상태효과중 스틸매직에 의한 상태효과가 지워질때 리스트에 담아 놓은 상태효과를 제거 한다.
		STEAL_MAGIC_BUFF_LIST::iterator firstIter = m_StealMagicBuffList.begin();
		if (firstIter != m_StealMagicBuffList.end())
		{
			std::list<int>::iterator iter = firstIter->second.begin();
			for (; iter != firstIter->second.end(); )
			{
				int nBlowID = (*iter);
				
				std::map<int, int>::iterator findIter = m_RemovedStealMagicStateBlowIDList.find(nBlowID);
				if (findIter != m_RemovedStealMagicStateBlowIDList.end())
				{
					iter = firstIter->second.erase(iter);
					continue;
				}

				++iter;
			}
		}
	}

	m_RemovedStealMagicStateBlowIDList.clear();

	//스틸매직에 의한 상태효과 예약된게 없다면 건너뛰고...
	if (m_StealMagicBuffAddList.empty())
		return;

	//기존에 적용된 스틸매직에의한 상태효과가 있다면 제거하고..
	if (!m_StealMagicBuffList.empty())
	{
		//아직 적용 되어 있는 상태효과 제거 한다..
		STEAL_MAGIC_BUFF_LIST::iterator firstIter = m_StealMagicBuffList.begin();
		if (firstIter != m_StealMagicBuffList.end())
		{
			std::list<int>::iterator iter = firstIter->second.begin();
			std::list<int>::iterator endIter = firstIter->second.end();
			for (; iter != endIter; ++iter)
			{
				int nBlowID = (*iter);
				
				//즉시 상태효과 제거 하고
				m_pStateBlow->RemoveImediatlyStateEffectFromID(nBlowID);

				//클라이언트로 상태효과 제거 패킷 보냄.
				SendRemoveStateEffectFromID(nBlowID);
			}

			firstIter->second.clear();
		}

		m_StealMagicBuffList.clear();
	}

	//예약된 스틸매직 상태효과 적용
	//랜덤하게 하나 골라 내자..
	int nCount = (int)m_StealMagicBuffAddList.size();
	int nSelectIndex = rand() % nCount;

	STEAL_MAGIC_BUFF_INFO_LIST::iterator selectedIter = m_StealMagicBuffAddList.begin();
	for (int i = 0; i < nSelectIndex; ++i)
		++selectedIter;

	if (selectedIter != m_StealMagicBuffAddList.end())
	{
		StealMagicBuffInfo &buffInfo = selectedIter->second;

		STATE_BLOW_INFO_LIST::iterator iter = selectedIter->second.stateBlowList.begin();
		STATE_BLOW_INFO_LIST::iterator endIter = selectedIter->second.stateBlowList.end();
		
		for (; iter != endIter; ++iter)
		{
			int nBlowID = CmdAddStateEffect(&buffInfo.skillInfo, iter->blowIndex, buffInfo.nDurationTime, iter->strValue.c_str(), false, false);
			
			AddStealMagicStateBlow(buffInfo.skillInfo.iSkillID, nBlowID);
		}
	}
	m_StealMagicBuffAddList.clear();
}

void CDnActor::AddStealMagicStateBlow(int nSkillID, int nBlowID)
{
	STEAL_MAGIC_BUFF_LIST::iterator findIter = m_StealMagicBuffList.find(nSkillID);
	if (findIter != m_StealMagicBuffList.end())
	{
		findIter->second.push_back(nBlowID);
	}
	else
	{
		std::list<int> blowIDList;
		blowIDList.push_back(nBlowID);

		m_StealMagicBuffList.insert(STEAL_MAGIC_BUFF_LIST::value_type(nSkillID, blowIDList));
	}
}

void CDnActor::RemoveStealMagicStateBlow(int nDeletedBlowID)
{
	//스틸매직에 의해 추가된 상태효과 정보가 있다면
	if (!m_StealMagicBuffList.empty())
	{
		//지금은 한 액터에서만 스틸매직을 하기때문에 begin에 있는것만 확인..
		STEAL_MAGIC_BUFF_LIST::iterator firstIter = m_StealMagicBuffList.begin();
		if (firstIter != m_StealMagicBuffList.end())
		{
			std::list<int>::iterator iter = firstIter->second.begin();
			std::list<int>::iterator endIter = firstIter->second.end();
			for (; iter != firstIter->second.end(); )
			{
				// 지금 제거되는 상태효과의 BlowID와 같은 녀석을 발견 하면 리스트에서 제거 한다.
				if (nDeletedBlowID == (*iter))
				{
					m_RemovedStealMagicStateBlowIDList.insert(std::map<int, int>::value_type(nDeletedBlowID, nDeletedBlowID));
					break;
				}

				++iter;
			}
		}
	}

}


void CDnActor::OnEndStateBlow( DnBlowHandle hBlow )
{

	//상태효과 제거 될때 스틸매직에 의한 상태효과이면 리스트에서 제거 하기 위한 코드
	if (!hBlow)
		return;

	int nDeletedBlowID = hBlow->GetBlowID();
	RemoveStealMagicStateBlow(nDeletedBlowID);

}

#ifdef PRE_ADD_MONSTER_CATCH
bool CDnActor::CatchCalcSuperArmor( DnActorHandle hCatcherActor, int iSuperArmorDamage )
{
	bool bResult = false;

	// 슈퍼아머 데미지를 적용하고 0 이하면 잡힘.
	// 데미지 적용하고 슈퍼아머가 남아있다면 프레임 느리게 하고 패스. 현재 슈퍼아머는 0만 사용함.
	m_nSkillSuperAmmorValue[ 0 ] -= iSuperArmorDamage;
	if( m_nSkillSuperAmmorValue[ 0 ] < 0 )
	{
		bResult = true;
	}
	else
	{
		// 우선 프레임 느려지는 건 뺀다.
		//// 슈퍼 아머로 버팀.
		//int iSuperArmorTime = m_nSkillSuperAmmorTime;
		//int iDelay = (int)( iSuperArmorTime * ( m_fStiffDelta / s_fMaxStiffTime ) );
		//if( iDelay > 0 ) 
		//{
		//	SetPlaySpeed( (DWORD)( iSuperArmorTime * ( m_fStiffDelta / s_fMaxStiffTime ) ), 0.03f );
		//}
	}

	return bResult;
}
#endif // #ifdef PRE_ADD_MONSTER_CATCH

bool CDnActor::CheckCollisionHitCondition(const EtVector3& vObjPos, const MatrixEx& objCross, EtVector3 &vTargetPos, float angleAllow)
{
	EtVector3 vDir = vTargetPos - vObjPos;
	vDir.y = 0.f;

	EtVec3Normalize(&vDir, &vDir);

	EtVector3 vZVec = objCross.m_vZAxis;
	float fDeg = EtToDegree(acos(EtVec3Dot(&vZVec, &vDir)));
	if (fDeg > angleAllow)
		return false;

	return true;
}

DnMonsterActorHandle CDnActor::FindOldSummonMonster(const SummonMonsterStruct* pSummonMonsterStruct)
{
	DnMonsterActorHandle hSelectedSummon;

	LOCAL_TIME tempTime = 0x7FFFFFFFFFFFFFFF;
	if( 0 < pSummonMonsterStruct->nGroupID )
	{
		if( 0 < m_mapSummonMonsterByGroup.count( pSummonMonsterStruct->nGroupID ) )
		{
			std::list<DnMonsterActorHandle>& listSummonedGroupMonster = m_mapSummonMonsterByGroup[ pSummonMonsterStruct->nGroupID ];
			for( std::list<DnMonsterActorHandle>::iterator itor=listSummonedGroupMonster.begin(); itor!=listSummonedGroupMonster.end(); ++itor)
			{
				DnMonsterActorHandle hMonster = *itor;

				//Group이 설정되어 있는 경우는 MonsterID를 비교 하지 않고 그룹내에 있는 소환 몬스터중에서 소환 시간이 제일 오래된 녀석을 선택 하도록 한다.
				if (!hMonster)
					continue;

				LOCAL_TIME requestSummonTime = hMonster->GetRequestSummonTime();
				if( requestSummonTime < tempTime)
				{
					hSelectedSummon = hMonster;
					tempTime = requestSummonTime;
				}
			}
		}
	}
	else
	{
		for( std::list<DnMonsterActorHandle>::iterator itor=m_listSummonMonster.begin() ; itor!=m_listSummonMonster.end(); ++itor)
		{
			DnMonsterActorHandle hMonster = *itor;
			if (!hMonster || hMonster->GetMonsterClassID() != pSummonMonsterStruct->MonsterID)
				continue;

			LOCAL_TIME requestSummonTime = hMonster->GetRequestSummonTime();
			if( requestSummonTime < tempTime)
			{
				hSelectedSummon = hMonster;
				tempTime = requestSummonTime;
			}
		}
	}

	return hSelectedSummon;
}

void CDnActor::RequestActionChange(int nActionIndex)
{
	BYTE pBuffer[ 16 ];
	CPacketCompressStream Stream( pBuffer, 16 );

	Stream.Write( &nActionIndex, sizeof(int) );
	
	Send( eActor::SC_ACTION_CHANGE, &Stream );
}

const std::list<DnMonsterActorHandle>& CDnActor::GetSummonedMonsterList( void )
{
	for( std::list<DnMonsterActorHandle>::iterator itor=m_listSummonMonster.begin() ; itor!=m_listSummonMonster.end() ; )
	{
		if( !(*itor) )
		{
			itor = m_listSummonMonster.erase( itor );
		}
		else
		{
			++itor;
		}
	}

	return m_listSummonMonster;
}

const std::map<int, std::list<DnMonsterActorHandle> >& CDnActor::GetGroupingSummonedMonsterList( void )
{
	map<int, list<DnMonsterActorHandle> >::iterator iter = m_mapSummonMonsterByGroup.begin();
	for( iter; iter != m_mapSummonMonsterByGroup.end(); ++iter )
	{	
		list<DnMonsterActorHandle>& listSummonedMonsters = iter->second;
		list<DnMonsterActorHandle>::iterator iterList = listSummonedMonsters.begin();
		for( iterList; iterList != listSummonedMonsters.end(); )
		{
			if( !(*iterList) )
			{
				iterList = listSummonedMonsters.erase( iterList );
			}
			else
			{
				++iterList;
			}
		}
	}

	return m_mapSummonMonsterByGroup;
}

void CDnActor::SendChainAttackProjectile(DnActorHandle hRootAttacker, DWORD dwPrevAttackerActorUniqueID, int iActionIndex, int iProjectileSignalArrayIndex, DnActorHandle hActorToAttack, int iSkillID)
{
	if (!hRootAttacker || !hActorToAttack)
		return;

	BYTE pBuffer[ 256 ];
	CPacketCompressStream Stream( pBuffer, 256 );

	//
	DWORD dwRootAttackerID = hRootAttacker->GetUniqueID();
	DWORD dwActorToAttackID = hActorToAttack->GetUniqueID();

	Stream.Write(&dwRootAttackerID, sizeof(DWORD));
	Stream.Write(&dwPrevAttackerActorUniqueID, sizeof(DWORD));
	Stream.Write(&dwActorToAttackID, sizeof(DWORD));

	Stream.Write(&iActionIndex, sizeof(int));
	Stream.Write(&iProjectileSignalArrayIndex, sizeof(int));
	Stream.Write(&iSkillID, sizeof(int));
		
	Send( eActor::SC_CHAINATTACK_PROJECTILE, &Stream );
}

void CDnActor::RemoveStateEffectByHitSignal(HitStruct* pHitStruct)
{
	if (NULL == pHitStruct)
		return;

	//RemoveStateIndex기능 막음 요청으로 인해 RemoveStateIndex는 무시 하도록 수정함.
#if defined(PRE_FIX_51988)
	if (pHitStruct->szRemoveStateIndexList == NULL)
		return;
#else
	if (pHitStruct->RemoveStateIndex == 0 && pHitStruct->szRemoveStateIndexList == NULL)
		return;
#endif // PRE_FIX_51988

#if defined(PRE_FIX_51988)
	std::string str = pHitStruct->szRemoveStateIndexList ? pHitStruct->szRemoveStateIndexList : "";
#else
	std::string str = FormatA("%d;%s", pHitStruct->RemoveStateIndex, pHitStruct->szRemoveStateIndexList ? pHitStruct->szRemoveStateIndexList : "");
#endif // PRE_FIX_51988
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	for (int i = 0; i < (int)tokens.size(); ++i)
	{

		STATE_BLOW::emBLOW_INDEX eBlowIndex = (STATE_BLOW::emBLOW_INDEX)(atoi(tokens[i].c_str()));

		if (eBlowIndex == STATE_BLOW::emBLOW_INDEX::BLOW_NONE)
			continue;

		//패킷 보내고...
		SendRemoveStateEffect(eBlowIndex);

		//여기서는 직접 바로 지운다...
		m_pStateBlow->RemoveImediatlyStateEffectByBlowIndex(eBlowIndex);
	}
}

bool CDnActor::ProcessIgnoreHitSignal()
{
	bool bIgnoreHitSignal = false;
	DnBlowHandle hIgnoreHitSignalBlow;
	if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_228))
	{
		DNVector(DnBlowHandle) vecBlowList;
		GatherAppliedStateBlowByBlowIndex(STATE_BLOW::BLOW_228, vecBlowList);
		for (int i = 0; i < (int)vecBlowList.size(); ++i)
		{
			DnBlowHandle hBlow = vecBlowList[i];
			if (hBlow && hBlow->CanBegin())
			{
				bIgnoreHitSignal = true;
				hIgnoreHitSignalBlow = hBlow;
				break;
			}
		}
	}

	//히트 시그널 무시가 설정이 되면 해당 상태효과 이펙트 표시용 패킷 날리고 더이상 데미지 처리 안하도록 한다.
	if (hIgnoreHitSignalBlow && bIgnoreHitSignal == true)
	{
		BYTE pBuffer[128];
		CPacketCompressStream Stream( pBuffer, 128 );

		DWORD dwUniqueID = GetUniqueID();
		STATE_BLOW::emBLOW_INDEX stateBlowIndex = hIgnoreHitSignalBlow->GetBlowIndex();
		bool bShowEffect = true;

		Stream.Write( &dwUniqueID, sizeof(dwUniqueID) );
		Stream.Write( &stateBlowIndex, sizeof(stateBlowIndex));
		Stream.Write( &bShowEffect, sizeof(bShowEffect));

		Send(eActor::SC_SHOW_STATE_EFFECT, &Stream);

		return true;
	}

	return false;
}

bool CDnActor::ProcessIgnoreGravitySignal()
{
	bool bIgnoreGravitySignal = false;
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_273 ) )
	{
		DNVector(DnBlowHandle) vecBlowList;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_273, vecBlowList );
		for( int i=0; i<(int)vecBlowList.size(); ++i )
		{
			DnBlowHandle hBlow = vecBlowList[i];
			if( hBlow && hBlow->CanBegin() )
			{
				bIgnoreGravitySignal = true;
				break;
			}
		}
	}

	return bIgnoreGravitySignal;
}

void CDnActor::AddSkillStateEffect(DnSkillHandle hSkill)
{
	m_ApplySkillStateEffectList.push_back(hSkill);
}

void CDnActor::ApplySkillStateEffect()
{
	if (m_ApplySkillStateEffectList.empty())
		return;

	std::vector<DnSkillHandle>::iterator iter = m_ApplySkillStateEffectList.begin();
	for (; iter != m_ApplySkillStateEffectList.end(); ++iter)
	{
		DnSkillHandle hSkill = *(iter);
		if (hSkill)
		{
			DWORD stateCount = hSkill->GetStateEffectCount();
			CDnStateBlow *pStateBlow = GetStateBlow();
			const CDnSkill::SkillInfo* skillInfo = hSkill->GetInfo();
			if (pStateBlow)
			{
				for (DWORD dwIndex = 0; dwIndex < stateCount; ++dwIndex)
				{
					CDnSkill::StateEffectStruct *pStateEffect = hSkill->GetStateEffectFromIndex(dwIndex);
					if (pStateEffect == NULL || pStateEffect->ApplyType != CDnSkill::StateEffectApplyType::ApplySelf)
						continue;

					CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;
					eResult = pStateBlow->CanApplySkillStateEffect(skillInfo, *pStateEffect);

					if (CDnSkill::CanApply::Fail != eResult)
					{
						RemoveResetStateBlow();

						int nBlowID = CmdAddStateEffect( skillInfo, (STATE_BLOW::emBLOW_INDEX)pStateEffect->nID, 
							pStateEffect->nDurationTime, pStateEffect->szValue.c_str(), false, false );

						//지속 시간이 없는 상태효과인 경우 스킬에 등록.
						if (-1 != nBlowID && pStateEffect->nDurationTime == -1)
						{
							hSkill->AddSignalApplyNoPacketStateEffectHandle(nBlowID);
						}
					}
				}
			}
		}
	}

	m_ApplySkillStateEffectList.clear();
}

void CDnActor::SendProbInvincibleSuccess( void )
{
	BYTE pBuffer[ 4 ] = { 0 };
	CPacketCompressStream Stream( pBuffer, 4 );

	Send( eActor::SC_PROBINVINCIBLE_SUCCESS, &Stream );
}

EtVector3 CDnActor::FindAutoTargetPos()
{
	EtVector3 vTargetPosition = *GetPosition();

	if( !m_FindAutoTargetName.empty())
	{
		int nBoneIndex = GetBoneIndex( m_FindAutoTargetName.c_str() );
		if( nBoneIndex != -1 )
		{
			EtMatrix boneMatrix = GetBoneMatrix(m_FindAutoTargetName.c_str());
			vTargetPosition = *(EtVector3*)&boneMatrix._41;
		}
	}

	return vTargetPosition;
}

void CDnActor::ApplyStateEffectSignalProcess( const S_NO_PACKET_SELF_STATEBLOW &StateBlowInfo, ApplyStateEffectStruct* pStruct )
{
	// StateBlow의 사용 여부 확인 먼저 [2010/11/11 semozz]
	if (false == StateBlowInfo.bUsed)
	{
		CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

		// [2010/11/11 semozz]
		// 여기 등록되어 있는 스킬의 StateBlow도 사용 가능한지 확인 후 CmdAddStateEffect를 호출 해야 한다.
		// CDnSkill의 OnBegin에서 체크 시점에서는 사용 가능이지만, 여기 호출 전에 다른 StateBlow가 등록 되어 있을 수 있다.
		// #20008의 힐을 동시에 사용시 이런 문제점이 발생.
		eResult = m_pStateBlow->CanApplySkillStateEffect(&StateBlowInfo.ParentSkillInfo, StateBlowInfo.StateEffect);

		if( CDnSkill::CanApply::Fail != eResult )
		{
			// [2010/12/08 semozz]
			// 스킬 사용 시점의 CanApplySkillStateEffect함수에서 리셋 시킬 상태들 리스트에 담아 놓은것들을
			// 여기서 제거 한다.
			RemoveResetStateBlow();

			// Note 한기: 스킬이 발동될 시에 자신에게 스킬 액션 시간 동안 즉시 걸리는 상태효과는 
			// 해제 시점을 스킬 객체가 알고 있으므로 스킬 객체쪽의 nopacket 상태효과. 리스트에 넣어준다.
			// nopacket 상태효과는 클라이언트에서도 스킬이 실행되면 알아서 추가하고 빼준다.
			// 그냥 자신에게 거는 버프 거는 지속시간이 있는 상태효과는 지속시간이 정해져 있으므로 시간 다 되면
			// 알아서 제거되므로 기존 시스템을 그대로 사용해서 CmdAddStateEffect 시킨다. 패킷도 알아서 쏴줌. 이 경우엔 클라에서 상태효과 발동시키지 않고
			// 서버의 패킷을 기다린다.
			if( StateBlowInfo.StateEffect.ApplyType == CDnSkill::ApplySelf && 
				StateBlowInfo.StateEffect.nDurationTime == -1 /*&&						// 버프/디버프/오라 등등도 될 수 있기 때문에 이 조건은 빠집니다. 
															  CDnSkill::Instantly == StateBlowInfo.ParentSkillInfo.eDurationType*/ )
			{
				// [2010/12/09 semozz]
				// 상태효과 추가 패킷을 보내야 한다.
				int iBlowID = CmdAddStateEffect( &StateBlowInfo.ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)StateBlowInfo.StateEffect.nID, 
					StateBlowInfo.StateEffect.nDurationTime, StateBlowInfo.StateEffect.szValue.c_str() );

				if( -1 != iBlowID )
				{
					if( m_hProcessSkill )
					{
						//_ASSERT( m_hProcessSkill && -1 != iBlowID );
						// 블록 상태효과 (30번)은 지속시간이 없는 컨셉으로 변경되었으므로 예외로 여기에 추가시켜주지 않는다.
						// 추가 시키면 액션이 끝나는 순간 같이 종료됨. 2010.09.20
						if( !((-1 == StateBlowInfo.StateEffect.nDurationTime) && 
							(STATE_BLOW::BLOW_030 == StateBlowInfo.StateEffect.nID)) )
							m_hProcessSkill->AddSignalApplyNoPacketStateEffectHandle( iBlowID );
					}
					else
						if( m_hAuraSkill && CDnSkill::Aura == StateBlowInfo.ParentSkillInfo.eDurationType )
						{
							m_hAuraSkill->AddSignalApplyNoPacketStateEffectHandle( iBlowID );
						}
#ifdef _DEBUG
						else
							_ASSERT( !"상태효과 설정한 시그널 타이밍 추가 실패!!" );
#endif

				}
			}
			else
			{
				// 지속시간이 있는 self 상태효과
				int iBlowID = CmdAddStateEffect( &StateBlowInfo.ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)StateBlowInfo.StateEffect.nID, 
					StateBlowInfo.StateEffect.nDurationTime, StateBlowInfo.StateEffect.szValue.c_str() );
			}
		}
		// 스킬 사용 끝났으면 리셋리스트 초기화 [2010/12/09 semozz]
		InitStateBlowIDToRemove();

		// 스킬 사용을 못해도 사용한걸로 해야하는지는 확인 해봐야 할듯 [2010/11/11 semozz]
		const_cast<S_NO_PACKET_SELF_STATEBLOW&>(StateBlowInfo).Used();
	}
	else
	{
		OutputDebug( "[Error!!] 상태효과 적용 시그널에서 이미 사용한 상태효과 인덱스(%d) 사용!!\n", pStruct->StateEffectIndex );
		_ASSERT( !"상태효과 적용 시그널에서 이미 사용한 상태효과 인덱스 사용!!" );
	}
}

#if defined(PRE_FIX_51048)
void CDnActor::RemoveDebufAction(LOCAL_TIME LocalTime, float fDelta)
{
	//Freezing
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_041 ) )
	{
		DNVector(DnBlowHandle) vlBlows;
		GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_041, vlBlows );
		
		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
				hBlow->RemoveDebufAction(LocalTime, fDelta);
		}
	}

	//Frostbite
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_144 ) )
	{
		DNVector(DnBlowHandle) vlBlows;
		GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_144, vlBlows );

		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
				hBlow->RemoveDebufAction(LocalTime, fDelta);
		}
	}

	//ElectricShock
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_043 ) )
	{
		DNVector(DnBlowHandle) vlBlows;
		GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_043, vlBlows );

		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
				hBlow->RemoveDebufAction(LocalTime, fDelta);
		}
	}

	//Escape
	if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_218 ) )
	{
		DNVector(DnBlowHandle) vlBlows;
		GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_218, vlBlows );

		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
				hBlow->RemoveDebufAction(LocalTime, fDelta);
		}
	}

}
#endif // PRE_FIX_51048

void CDnActor::SetState(ActorStateEnum State)
{
#if defined(PRE_FIX_59939)
	//매 프레임 SetState가 호출 되지만 저주상태효과(244)에서 자체 쿨타임 제어하기에 그냥 호출 하도록 한다...
	//
	__super::SetState(State);

	bool isAttackState = IsAttack();
	if (isAttackState)
		OnAttackChange();

#else
	bool isPreAttackState = IsAttack();

	__super::SetState(State);

	bool isNewAttackState = IsAttack();
	if (isPreAttackState == false && isNewAttackState == true)
		OnAttackChange();
#endif // PRE_FIX_59939
}

void CDnActor::OnAttackChange()
{
	if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_244))
	{
		DNVector(DnBlowHandle) vlBlows;
		GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_244, vlBlows );

		int nBlowCount = (int)vlBlows.size();
		for (int i = 0; i < nBlowCount; ++i)
		{
			DnBlowHandle hBlow = vlBlows[i];
			if (hBlow)
			{
				CDnCurseBlow* pCurseBlow = dynamic_cast<CDnCurseBlow*>(hBlow.GetPointer());
				if (pCurseBlow)
					pCurseBlow->OnAttackChange();
			}
		}
	}
}

int CDnActor::ExceptionHitList(DNVector(DnActorHandle) &hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct)
{
	std::map<DWORD, DnActorHandle> hOwnerActorList;		//꼭두각시 상태효과를 가지고 있는 주인액터 후보? 리스트
	std::map<DWORD, DnActorHandle> eraseActorList;		//제거 되어야 할 액터 리스트
	std::vector<CDnMonsterActor*> hSummonActorList;		//소환된 몬스터 액터 리스트

	DNVector(DnActorHandle) hNewVecList;				//새로운 액터 리스트

	int nWeaponLength = 0;
	bool bOnDamageCalled = false;
	if (hHiterActor)
	{
		if( pStruct->bIncludeWeaponLength && hHiterActor->GetWeapon() ) 
			nWeaponLength = hHiterActor->GetWeapon()->GetWeaponLength();
	}

	MatrixEx CrossTemp = Cross;

	// 스케일된 몬스터의 경우 히트 시그널의 중심축이 항상 액터의 원점 위치가 맞는 것은 아니기 때문에 
	// 오프셋에도 정확하게 스케일 값을 적용시켜 줘야 한다.(#18971)
	EtVector3 vScale = EtVector3(1.0f, 1.0f, 1.0f);
	if (hHiterActor)
		vScale = *hHiterActor->GetScale();

	EtVector3 vOffset = *pStruct->vOffset * vScale.y;

	CrossTemp.MoveLocalZAxis( vOffset.z );
	CrossTemp.MoveLocalXAxis( vOffset.x );
	CrossTemp.MoveLocalYAxis( vOffset.y );
	
	// 특정 본과 링크된 상태라면..
	if( hHiterActor &&
		pStruct->szLinkBoneName && strlen(pStruct->szLinkBoneName) > 0 )
	{
		int nBoneIndex = hHiterActor->GetBoneIndex( pStruct->szLinkBoneName );
		if( -1 != nBoneIndex )
		{
			EtMatrix matBoneWorld = hHiterActor->GetBoneMatrix( pStruct->szLinkBoneName );
			EtMatrixMultiply( (EtMatrix*)&CrossTemp, (EtMatrix*)&CrossTemp, &matBoneWorld );
		}
	}
	// 몬스터는 스케일에따라 보정해줘야한다.
	float fDistanceMax = pStruct->fDistanceMax * vScale.y;
	float fDistanceMin = pStruct->fDistanceMin * vScale.y;
	float fHeightMax = pStruct->fHeightMax * vScale.y;
	///////////////////////////////////////////////

	EtVector3 vPos = CrossTemp.m_vPosition;
	
	float fDistance = max( fDistanceMax, fHeightMax - pStruct->fHeightMin ) + nWeaponLength;
	float fXZDistanceSQ = fDistanceMax + nWeaponLength;
	float fXZDistanceMinSQ = fDistanceMin; // Min 은 Weapon 길이 영향안받습니다.

	fXZDistanceSQ *= fXZDistanceSQ;
	fXZDistanceMinSQ *= fXZDistanceMinSQ;

	EtVector3 vDir;
	EtVector3 vZVec = Cross.m_vZAxis;

	if( pStruct->fCenterAngle != 0.f ) 
	{
		EtMatrix matRotate;
		EtMatrixRotationY( &matRotate, EtToRadian( pStruct->fCenterAngle ) );
		EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );
	}
	SAABox Box;
	float fDot = 0.0f;

#if defined(PRE_FIX_59680)
	int nMyTeamID = -1;
	if (hHiterActor)
	{
		nMyTeamID = hHiterActor->GetTeam();
		if (hHiterActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
			nMyTeamID = hHiterActor->GetOriginalTeam();
	}
#endif // PRE_FIX_59680

	int nListCount = (int)hVecList.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnActorHandle hActor = hVecList[i];
		if (!hActor)
			continue;

		switch( hActor->GetHitCheckType() ) 
		{
		case HitCheckTypeEnum::BoundingBox: 
			{
				vDir = *hActor->GetPosition() - vPos;
				vDir.y = 0.f;

				hActor->GetBoundingBox( Box );

				if( SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
				if( SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

				EtVec3Normalize( &vDir, &vDir );
				fDot = EtVec3Dot( &vZVec, &vDir );
				if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) continue;

				if( Box.Min.y < vPos.y + pStruct->fHeightMin && 
					Box.Max.y < vPos.y + pStruct->fHeightMin ) continue;
				if( Box.Min.y > vPos.y + fHeightMax && 
					Box.Max.y > vPos.y + fHeightMax ) continue;
			}
			break;
		case HitCheckTypeEnum::Collision:
			{
				SCollisionCapsule				Capsule;
				SCollisionResponse				CollisionResult;
				DNVector(SCollisionResponse)	vCollisionResult;
				//
				Capsule.Segment.vOrigin = vPos;
				float fHeight = fHeightMax - pStruct->fHeightMin;
				Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( pStruct->fHeightMin + ( fHeight / 2.f ) );
				Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
				Capsule.fRadius = ( fDistanceMax + nWeaponLength );

				//int nParentBoneIndex = -1;
				EtVector3 vDestPos;
				if( hActor->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) 
					continue;
			}
		}

		
		//꼭두각시 상태효과가 있는 주인액터 후보 리스트를 만든다..
		if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_247))
		{
			hOwnerActorList.insert(std::make_pair(hActor->GetUniqueID(), hActor));
		}
		//꼭두각시로 소환된 몬스터 리스트를 만든다..
		else if (hActor->IsMonsterActor())
		{
			CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
			if (pMonsterActor && 
				pMonsterActor->IsSummonedMonster() &&
				pMonsterActor->IsPuppetSummonMonster())
			{
				
#if defined(PRE_FIX_59680)
				//렐릭오브힐 스킬의 경우 렐릭은 같은 팀으로 설정이됨.
				//렐릭의 Hit시그널 처리시 서먼 퍼펫이 리스트 추가가 되고, 아래에서 주인 액터가 제외 되어서 힐이 정상 처리 되지 않는 문제가 발생함.
				//같은 팀의 서먼퍼펫은 이 리스트에 제외 되어야한다.
				int nTargetTeam = hActor->GetTeam();
				if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
					nTargetTeam = hActor->GetOriginalTeam();

				//발사체에서 Hiter액터가 없을 수 있다. Prop인 경우(Hiter액터가 없는 경우는 추가)
				if (hHiterActor)
				{
					//서먼 퍼펫이 다른 팀인 경우 서먼액터 리스트에 추가 하고
					//같은 팀인 경우 지울 리스트에 서머퍼펫 액터를 추가해서 최종 리스트에 지워 지도록 한다.
					if (nMyTeamID != nTargetTeam)
						hSummonActorList.push_back(pMonsterActor);
					else
						eraseActorList.insert(std::make_pair(hActor->GetUniqueID(), hActor));
				}
				else
					hSummonActorList.push_back(pMonsterActor);
#else
				hSummonActorList.push_back(pMonsterActor);
#endif // PRE_FIX_59680
			}
		}
	}

	//소환 몬스터 액터가 있는 경우
	//소환 몬스터 액터의 주인 액터가 존재 하는 지 확인 해서, 주인 액터가 존재 하면
	//지울리스트에 주인 액터를 추가 해 놓는다.
	int nSummonActorList = (int)hSummonActorList.size();
	for (int iIndex = 0; iIndex < nSummonActorList; ++iIndex)
	{
		CDnMonsterActor* pSummonMonsterActor = hSummonActorList[iIndex];

		//소환액터의 주인 액터를 확인 한다.
		DnActorHandle hOwnerActor = pSummonMonsterActor->GetSummonerPlayerActor();
		DWORD dwOwnerUniqueID = hOwnerActor ? hOwnerActor->GetUniqueID() : -1;
		if (dwOwnerUniqueID == -1)
			continue;

		//주인후보 액터 리스트에서 액터를 찾는다.
		std::map<DWORD, DnActorHandle>::iterator findIter = hOwnerActorList.find(dwOwnerUniqueID);
		if (findIter != hOwnerActorList.end())
		{
			//삭제 액터 리스트에 추가 한다.
			eraseActorList.insert(std::make_pair(dwOwnerUniqueID, hOwnerActor));
		}
	}


	nListCount = (int)hVecList.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnActorHandle hActor = hVecList[i];
		if (!hActor)
			continue;

		//원래 액터 리스트를 돌면서, 지워야 할 액터 리스트에 찾지 못 하면 새로운 리스트에 추가 한다.
		DWORD dwOwnerUniqueID = hActor->GetUniqueID();
		std::map<DWORD, DnActorHandle>::iterator findIter = eraseActorList.find(dwOwnerUniqueID);
		if (findIter == eraseActorList.end())
			hNewVecList.push_back(hActor);
	}

	std::swap(hVecList, hNewVecList);

	return (int)hVecList.size();
}

//스캔된 액터 리스트중 Hit영역에 있는 녀석들만 리스트를 만든다.
void CDnActor::CheckHitAreaActorList(DNVector(DnActorHandle)& hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct, int nCheckType, float fScanLength, EtVector3& vPrePos)
{
	DNVector(DnActorHandle) hNewVecList;				//새로운 액터 리스트

	int nWeaponLength = 0;
	bool bOnDamageCalled = false;
	if (hHiterActor)
	{
		if( pStruct->bIncludeWeaponLength && hHiterActor->GetWeapon() ) 
			nWeaponLength = hHiterActor->GetWeapon()->GetWeaponLength();
	}

	MatrixEx CrossTemp = Cross;

	// 스케일된 몬스터의 경우 히트 시그널의 중심축이 항상 액터의 원점 위치가 맞는 것은 아니기 때문에 
	// 오프셋에도 정확하게 스케일 값을 적용시켜 줘야 한다.(#18971)
	EtVector3 vScale = EtVector3(1.0f, 1.0f, 1.0f);
	if (hHiterActor)
		vScale = *hHiterActor->GetScale();

	EtVector3 vOffset = *pStruct->vOffset * vScale.y;

	CrossTemp.MoveLocalZAxis( vOffset.z );
	CrossTemp.MoveLocalXAxis( vOffset.x );
	CrossTemp.MoveLocalYAxis( vOffset.y );

	// 특정 본과 링크된 상태라면..
	if( pStruct->szLinkBoneName && strlen(pStruct->szLinkBoneName) > 0 )
	{
		int nBoneIndex = hHiterActor->GetBoneIndex( pStruct->szLinkBoneName );
		if( -1 != nBoneIndex )
		{
			EtMatrix matBoneWorld = hHiterActor->GetBoneMatrix( pStruct->szLinkBoneName );
			EtMatrixMultiply( (EtMatrix*)&CrossTemp, (EtMatrix*)&CrossTemp, &matBoneWorld );
		}
	}
	// 몬스터는 스케일에따라 보정해줘야한다.
	float fDistanceMax = pStruct->fDistanceMax * vScale.y;
	float fDistanceMin = pStruct->fDistanceMin * vScale.y;
	float fHeightMax = pStruct->fHeightMax * vScale.y;
	///////////////////////////////////////////////

	EtVector3 vPos = CrossTemp.m_vPosition;

	float fDistance = max( fDistanceMax, fHeightMax - pStruct->fHeightMin ) + nWeaponLength;
	float fXZDistanceSQ = fDistanceMax + nWeaponLength;
	float fXZDistanceMinSQ = fDistanceMin; // Min 은 Weapon 길이 영향안받습니다.

	fXZDistanceSQ *= fXZDistanceSQ;
	fXZDistanceMinSQ *= fXZDistanceMinSQ;

	EtVector3 vDir;
	EtVector3 vZVec = Cross.m_vZAxis;

	if( pStruct->fCenterAngle != 0.f ) 
	{
		EtMatrix matRotate;
		EtMatrixRotationY( &matRotate, EtToRadian( pStruct->fCenterAngle ) );
		EtVec3TransformNormal( &vZVec, &vZVec, &matRotate );
	}
	SAABox Box;
	float fDot = 0.0f;

	int nListCount = (int)hVecList.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnActorHandle hActor = hVecList[i];
		if (!hActor)
			continue;

		if( !hActor->GetObjectHandle() )
			continue;

		switch( hActor->GetHitCheckType() ) 
		{
		case HitCheckTypeEnum::BoundingBox: 
			{
				switch(nCheckType)
				{
				case 0:	//CDnActor의 OnSignal에 있는 체크 방식...		
					{
						vDir = *hActor->GetPosition() - vPos;
						vDir.y = 0.f;

						hActor->GetBoundingBox( Box );

						if( SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
						if( SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

						EtVec3Normalize( &vDir, &vDir );
						fDot = EtVec3Dot( &vZVec, &vDir );
						if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) continue;

						if( Box.Min.y < vPos.y + pStruct->fHeightMin && 
							Box.Max.y < vPos.y + pStruct->fHeightMin ) continue;
						if( Box.Min.y > vPos.y + fHeightMax && 
							Box.Max.y > vPos.y + fHeightMax ) continue;
					}
					break;
				case 1:	//Projectile의 첫번째 체크 방식.
					{
						float fPropContactDistance = FLT_MAX;
						float fNowFrameActorContactDistanceSQ = FLT_MAX;

						vDir = *hActor->GetPosition() - vPos;
						vDir.y = 0.f;

						hActor->GetBoundingBox( Box );

						if( CDnActor::SquaredDistance( vPos, Box ) > fXZDistanceSQ ) continue;
						if( CDnActor::SquaredDistance( vPos, Box, false ) < fXZDistanceMinSQ ) continue;

						EtVec3Normalize( &vDir, &vDir );
						fDot = EtVec3Dot( &vZVec, &vDir );
						if( EtToDegree( acos( fDot ) ) > pStruct->fAngle ) continue;

						if( Box.Min.y < vPos.y + pStruct->fHeightMin && 
							Box.Max.y < vPos.y + pStruct->fHeightMin ) continue;
						if( Box.Min.y > vPos.y + pStruct->fHeightMax && 
							Box.Max.y > vPos.y + pStruct->fHeightMax ) continue;
						fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( vPos - vPrePos ) );
						if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;
					}
					break;
				case 2: //Projectile의 두번째 체크 방식
					{
						bool bResult = false;
						float fPropContactDistance = FLT_MAX;
						float fNowFrameActorContactDistanceSQ = FLT_MAX;

						SCollisionResponse				Response;
						DNVector(SCollisionResponse)	vResponse;

						float fThickness = max( fabs( pStruct->fHeightMin ), fabs( pStruct->fHeightMax ) );
						
						SSegment Segment;
						Segment.vOrigin = vPrePos;
						Segment.vDirection = Cross.m_vZAxis * fScanLength;

						SCollisionCapsule Capsule;
						Capsule.Segment = Segment;
						Capsule.fRadius = fThickness;

						
						if( fThickness == 0.f ) bResult = hActor->GetObjectHandle()->FindSegmentCollision( Segment, Response );
						else bResult = hActor->GetObjectHandle()->FindCapsuleCollision( Capsule, Response );

						fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( *hActor->GetPosition() - vPrePos ) );
						if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;

						if (bResult == false)
							continue;
					}
					break;
				}
			}
			break;
		case HitCheckTypeEnum::Collision:
			{
				switch(nCheckType)
				{
				case 0://CDnActor의 OnSignal 체크 방식..
					{
						SCollisionCapsule				Capsule;
						SCollisionResponse				CollisionResult;
						DNVector(SCollisionResponse)	vCollisionResult;
						//
						Capsule.Segment.vOrigin = vPos;
						float fHeight = fHeightMax - pStruct->fHeightMin;
						Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( pStruct->fHeightMin + ( fHeight / 2.f ) );
						Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
						Capsule.fRadius = ( fDistanceMax + nWeaponLength );

						//int nParentBoneIndex = -1;
						EtVector3 vDestPos;
						if( hActor->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) 
							continue;
					}
					break;
				case 1://Projectile의 첫번째 체크 방식..
					{
						SCollisionCapsule				Capsule;
						SCollisionResponse				CollisionResult;
						DNVector(SCollisionResponse)	vCollisionResult;
						//
						Capsule.Segment.vOrigin = vPos;
						float fHeight = pStruct->fHeightMax - pStruct->fHeightMin;
						Capsule.Segment.vOrigin.y = Capsule.Segment.vOrigin.y - ( pStruct->fHeightMin + ( fHeight / 2.f ) );
						Capsule.Segment.vDirection = EtVector3( 0.f, fHeight / 2.f, 0.f );
						Capsule.fRadius = pStruct->fDistanceMax;

						//int nParentBoneIndex = -1;
						EtVector3 vDestPos;
						if( hActor->GetObjectHandle()->CEtCollisionEntity::FindCapsuleCollision( Capsule, CollisionResult, &vCollisionResult ) == false ) 
							continue;
					}
					break;
				case 2://Projectile의 두번째 체크 방식..
					{
						float fPropContactDistance = FLT_MAX;
						float fNowFrameActorContactDistanceSQ = FLT_MAX;

						bool bResult = false;
						float fThickness = max( fabs( pStruct->fHeightMin ), fabs( pStruct->fHeightMax ) );
						SSegment Segment;
						Segment.vOrigin = vPrePos;
						Segment.vDirection = Cross.m_vZAxis * fScanLength;

						SCollisionCapsule Capsule;
						Capsule.Segment = Segment;
						Capsule.fRadius = fThickness;

						SCollisionResponse				Response;
						DNVector(SCollisionResponse)	vResponse;

						int nParentBoneIndex = -1;
						EtVector3 vDestPos;
						if( fThickness == 0.f ) 
							bResult = hActor->GetObjectHandle()->FindSegmentCollision( Segment, Response, &vResponse );
						else
							bResult = hActor->GetObjectHandle()->FindCapsuleCollision( Capsule, Response, &vResponse );
						
						if (bResult)
						{
							fNowFrameActorContactDistanceSQ = EtVec3LengthSq( &EtVector3( Segment.vDirection * Response.fContactTime ) );
							if( fNowFrameActorContactDistanceSQ > fPropContactDistance ) continue;
						}
						else
							continue;
					}
					break;
				}
			}
		}

		hNewVecList.push_back(hActor);
	}

	std::swap(hVecList, hNewVecList);
}

int CDnActor::ExceptionHitList2(DNVector(DnActorHandle) &hVecList, MatrixEx& Cross, DnActorHandle hHiterActor, HitStruct* pStruct, 
								DNVector(DnActorHandle)& hAddStateEffectActorList, 
								int nCheckType, float fScanLength, EtVector3& vPrePos)
{
	//히트 영역에 있는 액터들만 리스트에 담는다...
	CDnActor::CheckHitAreaActorList(hVecList, Cross, hHiterActor, pStruct, nCheckType, fScanLength, vPrePos);

	int nMyTeamID = -1;
	if (hHiterActor)
	{
		nMyTeamID = hHiterActor->GetTeam();
		if (hHiterActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
			nMyTeamID = hHiterActor->GetOriginalTeam();
	}

	struct _PuppetNOwnerInfo
	{
		DWORD dwOwnerID;
		CDnMonsterActor* pPuppetMonster;
		DnActorHandle hPuppetActor;
		DnActorHandle hOwnerActor;
	};

	std::map<DWORD, _PuppetNOwnerInfo> PuppetnOnwerInfoList;

	DNVector(DnActorHandle) hNewVecList;				//새로운 액터 리스트
	
	EtVector3 vPos = *hHiterActor->GetPosition();

	int nListCount = (int)hVecList.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnActorHandle hActor = hVecList[i];
		if (!hActor)
			continue;

		bool bSeperated = false;

		//꼭두각시 상태효과가 있는 주인액터 후보 리스트를 만든다..
		if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_247))
		{
			bSeperated = true;
			DWORD dwOwnerUniqueID = hActor->GetUniqueID();

			std::map<DWORD, _PuppetNOwnerInfo>::iterator findIter = PuppetnOnwerInfoList.find(dwOwnerUniqueID);
			if (findIter == PuppetnOnwerInfoList.end())
			{
				_PuppetNOwnerInfo _info;
				_info.dwOwnerID = dwOwnerUniqueID;
				_info.hOwnerActor = hActor;

				PuppetnOnwerInfoList.insert(std::make_pair(dwOwnerUniqueID, _info));
			}
			else
			{
				_PuppetNOwnerInfo& _info = findIter->second;
				_info.hOwnerActor = hActor;
			}
		}
		//꼭두각시로 소환된 몬스터 리스트를 만든다..
		else if (hActor->IsMonsterActor())
		{
			CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
			if (pMonsterActor && 
				pMonsterActor->IsSummonedMonster() &&
				pMonsterActor->IsPuppetSummonMonster())
			{
				bSeperated = true;

				DnActorHandle hOwnerActor = pMonsterActor->GetSummonerPlayerActor();
				DWORD dwOwnerUniqueID = hOwnerActor ? hOwnerActor->GetUniqueID() : -1;

				std::map<DWORD, _PuppetNOwnerInfo>::iterator findIter = PuppetnOnwerInfoList.find(dwOwnerUniqueID);
				if (findIter == PuppetnOnwerInfoList.end())
				{
					_PuppetNOwnerInfo _info;
					_info.dwOwnerID = dwOwnerUniqueID;
					_info.hPuppetActor = hActor;
					_info.pPuppetMonster = pMonsterActor;

					PuppetnOnwerInfoList.insert(std::make_pair(dwOwnerUniqueID, _info));
				}
				else
				{
					_PuppetNOwnerInfo& _info = findIter->second;
					_info.hPuppetActor = hActor;
					_info.pPuppetMonster = pMonsterActor;
				}
			}
		}

		//주인 액터/ 소환 액터 리스트에 포함되지 않는 녀석들은 새로운 리스트에 미리 담아 놓는다.
		if (bSeperated == false)
			hNewVecList.push_back(hActor);
	}


	std::map<DWORD, _PuppetNOwnerInfo>::iterator iter = PuppetnOnwerInfoList.begin();
	std::map<DWORD, _PuppetNOwnerInfo>::iterator endIter = PuppetnOnwerInfoList.end();
	for (; iter != endIter; ++iter)
	{
		_PuppetNOwnerInfo& _info = iter->second;

		if (_info.hOwnerActor && _info.hPuppetActor)
		{
			/*
			//제일 가까운 액터를 추가 한다.
			//주인 액터가 제외 되는 경우 제외된 액터 리스트에 추가 해 놓는다.(상태효과 추가를 위해)
			EtVector3 vOwnerDiff = (*_info.hOwnerActor->GetPosition()) - vPos;
			EtVector3 vPuppetDiff = (*_info.hPuppetActor->GetPosition()) - vPos;

			float fOwnerLength = EtVec3LengthSq(&vOwnerDiff);
			float fPuppetLength = EtVec3LengthSq(&vPuppetDiff);
			*/

			int nTargetTeam = _info.hOwnerActor->GetTeam();
			if (_info.hOwnerActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
				nTargetTeam = _info.hOwnerActor->GetOriginalTeam();
			
			/*
			//같은 팀인 경우 거리에 상관없이 주인 액터를 추가
			//주인 액터 거리가 더 멀다면 소환 액터를 리스트에 추가 하고, 제외된 액터 리스트에 주인 액터를 추가
			if (nMyTeamID != nTargetTeam && fOwnerLength > fPuppetLength)
			*/


			//거리 상관 없이 같은 히트 범위에 있으면 소환 액터를 히트 리스트에 추가 한다.
			if (hHiterActor &&
				nMyTeamID != nTargetTeam)
			{
				hNewVecList.push_back(_info.hPuppetActor);

				hAddStateEffectActorList.push_back(_info.hOwnerActor);
			}
			else
			{
				hNewVecList.push_back(_info.hOwnerActor);
			}
			
		}
		//소환 액터/주인 액터 쌍이 아닌 경우(두 액터중 하나만 있는경우) 리스트에 추가
		else
		{
			DnActorHandle hActor = _info.hOwnerActor ? _info.hOwnerActor : _info.hPuppetActor;
			if (hActor)
				hNewVecList.push_back(hActor);
		}
	}

	std::swap(hVecList, hNewVecList);

	return (int)hVecList.size();	
}

void CDnActor::ScanActorByStateIndex(DNVector(DnActorHandle) &Veclist, STATE_BLOW::emBLOW_INDEX blowIndex)
{
	DNVector(DnActorHandle) scanActorList;
	ScanActor(GetRoom(), *GetPosition(), FLT_MAX, scanActorList);
	int nListCount = (int)scanActorList.size();
	for (int i = 0; i < nListCount; ++i)
	{
		DnActorHandle hActor = scanActorList[i];
		if (hActor && hActor->IsDie() == false)
		{
			DNVector(DnBlowHandle) blowList;
			if (hActor->IsAppliedThisStateBlow(blowIndex))
				hActor->GetStateBlow()->GetStateBlowFromBlowIndex(blowIndex, blowList);

			int nBlowCount = (int)blowList.size();
			for (int j = 0; j < nBlowCount; ++j)
			{
				DnBlowHandle hBlow = blowList[j];
				if (hBlow && hBlow->IsEnd() == false)
				{
					//해당 액터의 낙인 상태효과의 스킬 시전자가 자신인 경우 리스트에 담는다.
					CDnSkill::SkillInfo *pSkillInfo = const_cast<CDnSkill::SkillInfo*>(hBlow->GetParentSkillInfo());
					DnActorHandle hSkillActor = GetMySmartPtr();
					if (IsMonsterActor())
					{
						CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hSkillActor.GetPointer());
						if (pMonsterActor)
							hSkillActor = pMonsterActor->GetSummonerPlayerActor();
					}

					if (pSkillInfo && pSkillInfo->hSkillUser == hSkillActor)
						Veclist.push_back(hActor);
				}
			}
		}
	}
}



// Signal Rotate.
void CDnActor::SetRotate( DWORD dwTime, float fStart, float fEnd, EtVector3 & vPos, bool bLeft )
{
	m_bRotate = true;
	m_dwRotateStartTime = CDnActionBase::m_LocalTime;
	m_dwRotateTime = dwTime;
	m_fStartSpeed = fStart;  m_fEndSpeed = fEnd;
	m_vRotAxis = vPos;  m_bRotLeft = bLeft;
}

// Rotate.
void CDnActor::ProcessRotate( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_bRotate )
		return;

	if( (LocalTime - m_dwRotateStartTime) > m_dwRotateTime )
	{
		m_bRotate = false;
		return;
	}

	if( m_dwRotateStartTime == 0 ) m_dwRotateStartTime = LocalTime;

	// 속도( 회전각량 )	
	if( m_fStartSpeed == m_fEndSpeed )
	{
		m_fSpeedRot = m_fStartSpeed;
	}
	else
	{
		/*float frame = pObject->GetCurFrame();
		float w = ( frame - m_pSignal->GetStartFrame() ) / ( m_pSignal->GetEndFrame() - m_pSignal->GetStartFrame() );		
		m_fSpeedRot = m_fStartSpeed + ( ( m_fEndSpeed - m_fStartSpeed ) * w );		*/	
		m_fSpeedRot = m_fStartSpeed;
	}

	// 방향.
	if( m_bRotLeft )
		m_fSpeedRot = -m_fSpeedRot;

	m_fSpeedRot *= fDelta;

	//// 회전축좌표이동.
	m_Cross.MoveLocalXAxis( m_vRotAxis.x );
	m_Cross.MoveLocalYAxis( m_vRotAxis.y );
	m_Cross.MoveLocalZAxis( m_vRotAxis.z );

	// 회전.
	m_Cross.RotateYaw( (-m_fSpeedRot) );
	
}

#if defined(PRE_FIX_61382)
//꼭두각시 몬스터일 경우 주인 액터 반환.
DnActorHandle CDnActor::GetOwnerActorHandle(DnActorHandle hActor)
{
	DnActorHandle hOwnerActor = hActor;

	if (hActor && hActor->IsMonsterActor())
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		if (pMonsterActor && pMonsterActor->IsPuppetSummonMonster())
			hOwnerActor = pMonsterActor->GetSummonerPlayerActor();
	}

	return hOwnerActor;
}
#endif // PRE_FIX_61382

#if defined(PRE_ADD_65808)
void CDnActor::AddSummonMonsterGlyphInfo(int monsterID, int glyphID)
{
	std::map<int, std::list<int>>::iterator findIter = m_SummonMonsterGlyphInfo.find(monsterID);
	if (findIter != m_SummonMonsterGlyphInfo.end())
	{
		findIter->second.push_back(glyphID);
	}
	else
	{
		std::list<int> glyphIDList;
		glyphIDList.push_back(glyphID);

		m_SummonMonsterGlyphInfo.insert(std::make_pair(monsterID, glyphIDList));
	}
}

void CDnActor::AddSummonMonsterGlyphStateEffectID(int monsterID, int glyphID, DWORD dwMonsterUniqueID, int stateEffectID)
{
	std::map<int, std::map<int, std::list<_StateEffectInfo>>>::iterator findIter = m_SummonMonsterGlyphStateEffectIDs.find(monsterID);

	//해당 몬스터 정보가 없다면
	if (findIter == m_SummonMonsterGlyphStateEffectIDs.end())
	{
		//새로운 glyphID, stateEffectIDList를 만들어 등록한다.
		std::map<int, std::list<_StateEffectInfo>> newList;
		std::list<_StateEffectInfo> stateEffectList;
		_StateEffectInfo info;
		info.nStateEffectID = stateEffectID;
		info.dwUniqueID = dwMonsterUniqueID;

		stateEffectList.push_back(info);

		newList.insert(std::make_pair(glyphID, stateEffectList));

		m_SummonMonsterGlyphStateEffectIDs.insert(std::make_pair(monsterID, newList));
	}
	else
	{
		std::map<int, std::list<_StateEffectInfo>>& oldList = findIter->second;
		
		std::map<int, std::list<_StateEffectInfo>>::iterator iter = oldList.find(glyphID);
		if (iter != oldList.end())
		{
			//이미 glyphID가 등록 되어 있으면 기존 리스트에 stateEffectID를 등록한다.
			_StateEffectInfo info;
			info.nStateEffectID = stateEffectID;
			info.dwUniqueID = dwMonsterUniqueID;

			iter->second.push_back(info);
		}
		else
		{
			//몬스터는 등록 되어 있는데, glyphID가 등록 되어 있지 않으면..
			//상태효과 리스트 만들고,
			std::list<_StateEffectInfo> stateEffectList;

			_StateEffectInfo info;
			info.nStateEffectID = stateEffectID;
			info.dwUniqueID = dwMonsterUniqueID;

			stateEffectList.push_back(info);

			//glyphID에 상태효과 리스트를 등록 한다.
			oldList.insert(std::make_pair(glyphID, stateEffectList));
		}
	}
}

void CDnActor::RemoveSummonMonsterGlyphInfo(int monsterID, int glyphID)
{
	std::map<int, std::list<int>>::iterator findIter = m_SummonMonsterGlyphInfo.find(monsterID);
	if (findIter != m_SummonMonsterGlyphInfo.end())
	{
		std::list<int>& oldList = findIter->second;
		std::list<int>::iterator iter = oldList.begin();
		std::list<int>::iterator endIter = oldList.end();

		int nGlyphID = 0;
		for (; iter != endIter; ++iter)
		{
			nGlyphID = (*iter);

			if (nGlyphID == glyphID)
			{
				oldList.erase(iter);
				break;
			}
		}
	}

	//소환몬스터중에 monsterID에 해당하는 몬스터들을 찾는다..
	std::list<DnMonsterActorHandle> monsterActorList;
	FindSummonMonster(monsterID, monsterActorList);

	std::list<DnMonsterActorHandle>::iterator iter = monsterActorList.begin();
	std::list<DnMonsterActorHandle>::iterator endIter = monsterActorList.end();
	for (; iter != endIter; ++iter)
	{
		DnMonsterActorHandle hMonsterActor = (*iter);

		//몬스터 액터 리스트를 순회 하는동안 정보가 지워 지면 안된다..
		RemoveSummonMonsterStateEffectByGlyph(hMonsterActor, monsterID, glyphID);
	}

	//실제 정보는 여기서 제거 되도록 한다.
	RemoveSummonMonsterGlyphStateEffects(monsterID);
}

void CDnActor::RemoveSummonMonsterStateEffectByGlyph(DnMonsterActorHandle hMonsterActor, int monsterID, int glyphID)
{
	std::map<int, std::map<int, std::list<_StateEffectInfo>>>::iterator findIter = m_SummonMonsterGlyphStateEffectIDs.find(monsterID);

	if (findIter != m_SummonMonsterGlyphStateEffectIDs.end())
	{
		std::map<int, std::list<_StateEffectInfo>>& oldList = findIter->second;

		std::map<int, std::list<_StateEffectInfo>>::iterator glyphIter = oldList.find(glyphID);
		if (glyphIter != oldList.end())
		{
			std::list<_StateEffectInfo>& stateEffectIDList = glyphIter->second;

			if (stateEffectIDList.empty() == false)
			{
				std::list<_StateEffectInfo>::iterator iter = stateEffectIDList.begin();
				std::list<_StateEffectInfo>::iterator endIter = stateEffectIDList.end();

				for (; iter != endIter; ++iter)
				{
					_StateEffectInfo& info = (*iter);

					if (hMonsterActor && hMonsterActor->GetUniqueID() == info.dwUniqueID)
						hMonsterActor->CmdRemoveStateEffectFromID(info.nStateEffectID);
				}
			}
		}
	}
}

//실제 몬스터가 죽었을때 PlayerActor::OnDieSummonMonster함수에서 호출 됨.
//몬스터가 제거 될 예정이므로 상태효과를 찾아 일일이 지울 필요는 없을듯..
//설정된 정보만 제거..
void CDnActor::RemoveSummonMonsterGlyphStateEffects(int monsterID)
{
	std::map<int, std::map<int, std::list<_StateEffectInfo>>>::iterator findIter = m_SummonMonsterGlyphStateEffectIDs.find(monsterID);
	if (findIter != m_SummonMonsterGlyphStateEffectIDs.end())
	{
		m_SummonMonsterGlyphStateEffectIDs.erase(findIter);
	}
}

bool CDnActor::GetSummonMonsterGlyphInfo(int monsterID, std::list<int>& glyphIDs)
{
	std::map<int, std::list<int>>::iterator findIter = m_SummonMonsterGlyphInfo.find(monsterID);
	if (findIter != m_SummonMonsterGlyphInfo.end())
	{
		std::list<int>& oldList = findIter->second;
		std::list<int>::iterator iter = oldList.begin();
		std::list<int>::iterator endIter = oldList.end();
		
		int nGlyphID = 0;
		for (; iter != endIter; ++iter)
		{
			nGlyphID = (*iter);

			glyphIDs.push_back(nGlyphID);
		}

		return true;
	}

	return false;	
}

void CDnActor::FindSummonMonster(int monsterID, std::list<DnMonsterActorHandle>& monsterActorList)
{
	std::map<int, std::list<DnMonsterActorHandle> >::iterator iter = m_mapSummonMonsterByGroup.begin();
	std::map<int, std::list<DnMonsterActorHandle> >::iterator endIter = m_mapSummonMonsterByGroup.end();

	for (; iter != endIter; ++iter)
	{
		std::list<DnMonsterActorHandle>::iterator monsterIter = iter->second.begin();
		std::list<DnMonsterActorHandle>::iterator monsterEndIter = iter->second.end();
		for (; monsterIter != monsterEndIter; ++monsterIter)
		{
			DnMonsterActorHandle hMonster = *monsterIter;

			if (!hMonster)
				continue;

			if (hMonster->GetMonsterClassID() == monsterID)
			{
				monsterActorList.push_back(hMonster);
			}
		}
	}
	
	{
		for( std::list<DnMonsterActorHandle>::iterator itor=m_listSummonMonster.begin() ; itor!=m_listSummonMonster.end(); ++itor)
		{
			DnMonsterActorHandle hMonster = *itor;
			if (!hMonster || hMonster->GetMonsterClassID() != monsterID)
				continue;

			monsterActorList.push_back(hMonster);
		}
	}
}
#endif // PRE_ADD_65808


#if defined(PRE_FIX_68898)
void CDnActor::SetSkipEndAction(bool isSkipEndAction)
{
	if (IsProcessSkill() && m_hProcessSkill)
	{
		CDnPartialPlayProcessor* pPartialPlayProcess =  static_cast<CDnPartialPlayProcessor*>(m_hProcessSkill->GetProcessor( IDnSkillProcessor::PARTIAL_PLAY_ANI ));
		if (pPartialPlayProcess)
			pPartialPlayProcess->SetSkipEndAction(isSkipEndAction);
	}
}
#endif // PRE_FIX_68898


void CDnActor::ResetDamageRemainTime()
{
	m_mapLastDamageTime.clear();
	m_nLastDamageHitterActionIndex = 0;
}

LOCAL_TIME CDnActor::GetDamageRemainTime( DWORD dwActorUniqueID ) 
{ 
	map<DWORD, LOCAL_TIME>::iterator iter = m_mapLastDamageTime.find( dwActorUniqueID );
	if( m_mapLastDamageTime.end() != iter )
		return iter->second;
	else
		return 0;
}

void CDnActor::SetDamageRemainTime( DWORD dwActorUniqueID, LOCAL_TIME LastDamageTime, int nCurrentActionIndex ) 
{ 
	m_mapLastDamageTime[ dwActorUniqueID ] = LastDamageTime; 
	m_nLastDamageHitterActionIndex = nCurrentActionIndex;
}

void CDnActor::SetLastDamageHitUniqueID( DWORD dwActorUniqueID, int iLastHitUniqueID ) 
{ 
	m_mapLastHitUniqueID[ dwActorUniqueID ] = iLastHitUniqueID; 
}

int CDnActor::GetLastDamageHitUniqueID( DWORD dwActorUniqueID ) 
{
	if( m_mapLastHitUniqueID.end() != m_mapLastHitUniqueID.find( dwActorUniqueID ) )
		return m_mapLastHitUniqueID[ dwActorUniqueID ];
	else
		return -1;
};