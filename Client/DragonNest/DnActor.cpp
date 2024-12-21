#include "StdAfx.h"
#include "DnActor.h"
#include "DnWorld.h"
#include "DnWorldGrid.h"
#include "MAActorRenderBase.h"
#include "MAMovementBase.h"
#include "MAFaceAniBase.h"
#include "EtSoundChannel.h"
#include "DnWeapon.h"
#include "EtActionSignal.h"
#include "EtActionCoreMng.h"
#include "VelocityFunc.h"
#include "DnWorldBrokenProp.h"
#include "DnWorldSector.h"
#include "DnProjectile.h"
#include "DnMonsterActor.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "MATransAction.h"
#include "DnStateBlow.h"
#include "DnBlow.h"
#include "DnSkill.h"
#include "DnTableDB.h"
#include "DnWorldTrapProp.h"
#include "DnActorClassDefine.h"
#include "DnPvPGameTask.h"
#include "DnQuestTask.h"
#include "GameOption.h"
#include "DnChangeActionStrProcessor.h"
#include "EtComputeDist.h"
#include "DnLocalPlayerActor.h"
#include "DnChargerBlow.h"
#include "DnChangeActionSetBlow.h"
#include "DnPAyBackMPBlow.h"
#include "DnSkillTask.h"
#include "DnFrostbiteBlow.h"
#include "DnVillageTask.h"
#include "DnFreezingPrisonBlow.h"
#include "DnProbInvincibleAtBlow.h"
#include "DnDurabilityGaugeMng.h"
#include "DnShockInfectionBlow.h"
#include "DnAddDamageOnCriticalBlow.h"
#include "DnDisableActionBlow.h"
#include "navigationmesh.h"
#include "DnStateBlowSignalProcessor.h"

#ifdef PRE_FIX_MEMOPT_SIGNALH
#include "DnCommonUtil.h"
#endif

#if defined(PRE_FIX_68898)
#include "DnPartialPlayProcessor.h"
#endif // PRE_FIX_68898

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

DECL_DN_SMART_PTR_STATIC( CDnActor, 500 )

CEtOctree<CDnActor*> *CDnActor::s_pOctree = NULL;
float CEtOctreeNode<CDnActor*>::s_fMinRadius = 1000.0f;
DnActorHandle CDnActor::s_hLocalActor;
DWORD CDnActor::s_dwUniqueCount = 0;
int CDnActor::s_nFontIndex = 5;
float CDnActor::s_fDieDelta = 20.f;
std::map<DWORD, DnActorHandle> CDnActor::s_dwMapActorSearch;
CSyncLock CDnActor::s_LockSearchMap;
std::vector<CDnPlayerActor *> CDnActor::s_pVecDeleteList;

const char* CDnActor::s_acEffectBoneName[ CDnActor::Max_FX_Dummy_Bone ] = { "#FX_01", "#FX_02", "#FX_03", "#FX_04", "#FX_05", 
																			"#FX_06", "#FX_07", "#FX_08", "#FX_09", "#FX_10",
																			"#FX_11", "#FX_12", "#FX_13", "#FX_14" };	//접두어용 추가

#ifdef PRE_ADD_GRAVITY_PROPERTY
float SquaredDistance( const EtVector3& vPos, const SAABox& BBox, bool bNear )
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
#endif // PRE_ADD_GRAVITY_PROPERTY

#ifdef RENDER_PRESS_hayannal2009
bool CDnActor::s_bEnableRenderPress = false;
#endif

CDnActor::CDnActor( int nClassID, bool bProcess )
	: CDnUnknownRenderObject( bProcess )
	, m_vStaticPos( 0.f, 0.f, 0.f )
	, m_vPrevPos( 0.f, 0.f, 0.f )	
{
	m_nClassID = nClassID;
	m_fAddHeight = 0.f;
	m_dwUniqueID = -1;

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	m_pCheckPreSignalFunc = CDnActor::CheckPreSignal;
#endif
	
#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	// Environment Effect Load
	MAEnvironmentEffect::Initialize( m_nClassID );
#endif
	CDnActorState::Initialize( this );
	MAFaceAniBase::Initialize( m_nClassID );

	m_bShow = true;
	m_fDownDelta = 0.f;
	m_fDieDelta = 0.f;
	m_fMaxDieDelta = CDnActor::s_fDieDelta;
	m_fStiffDelta = 0.f;	
	m_LastDamageTime = 0;
	m_LastHitSignalTime = 0;
	m_nLastHitSignalIndex = -1;
	m_nTeam = 0;

	m_bModifyPlaySpeed = false;
	m_PlaySpeedStartTime = 0;
	m_dwPlaySpeedTime = 0;

	m_nSuperAmmorTime = 0;
	m_bShowHeadName = false;
	memset( m_bHideExposureInfo, 0, sizeof(m_bHideExposureInfo) );

	m_HitCheckType = HitCheckTypeEnum::BoundingBox;
	m_DamageCheckType = DamageCheckTypeEnum::DifferentTeam;
	memset( m_bSelfDeleteWeapon, 0, sizeof(m_bSelfDeleteWeapon) );

	m_pStateBlow = new CDnStateBlow( GetMySmartPtr() );

	m_bIngnoreNormalSuperArmor = false;

	m_pCurrentNode = NULL;
	if( bProcess ) InsertOctreeNode();
	

	m_bUseSignalSkillCheck = false;
	
	m_iCantMoveReferenceCount = 0;
	m_iCantActionReferenceCount = 0;
	m_iCantUseSkillReferenceCount = 0;
	m_iCantAttackReferenceCount = 0;

	m_iCantXZMoveSE = 0;

#ifdef _SHADOW_TEST
	m_bEnableShadowActor = false;
	m_bIsShadowActor = false;
#endif 	
	m_nCurrSuperArmor = 0;
	m_nMaxSuperArmor = 0;
	m_SoftAppearStep = AS_NONE;
	m_SoftTime = 0;

	m_bOnSignalFromChargerSE = false;
	m_iFrameStopRefCount = 0;

	// SetPlaySpeed() 함수에 들어온 fSpeed 인자값을 저장해두는 역할.
	m_fPlaySpeed = 0.0f;
	m_lBreakIntoLoadingFlag = false;

	m_pStateBlowSignalProcessor = new CDnStateBlowSignalProcessor();

	CDnHeadEffectRender::Initialize( GetMySmartPtr() );

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

#if defined(PRE_FIX_57706)
	m_nStickAniDlgRefCount = 0;
#endif // PRE_FIX_57706

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

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
	m_isMailBoxOpenByShortCutKey = false;
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_MAINQUEST_UI
	m_bFromMapTriggerHide = false;
#endif

	//rlkt_aura
	m_hRlktAura = CEtOutlineObject::Create(CEtObject::Identity());
	static EtColor vColor(0.2f, 0.5f, 1.0f, 1.0f);
	m_hRlktAura->SetColor(vColor);
	m_hRlktAura->SetWidth(1.3f);
	m_fRlktAuraDelayTime = 0.0f;
	m_bProcessRlktAura = false;
}

CDnActor::~CDnActor()
{
#ifdef _SHADOW_TEST
	SAFE_RELEASE_SPTR( m_hShadowActor );
#endif	
	RemoveUniqueSearchMap( this );
	ReleaseSignalImp();
	FreeAction();
	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		DetachWeapon( i );
	}
	RemoveOctreeNode();
	for( DWORD i=0; i<m_VecPreActionState.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecPreActionState[i] );
	}
	for( DWORD i=0; i<m_VecPreActionCustomState.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecPreActionCustomState[i] );
	}
	for( DWORD i=0; i<m_VecPreCanMoveState.size(); i++ ) {
		SAFE_DELETE_VEC( m_VecPreCanMoveState[i] );
	}

	SAFE_DELETE(m_pStateBlow);

	for ( std::map< int , EtSwordTrailHandle >::iterator it = m_TrailList.begin(); it != m_TrailList.end(); ++it) {
		SAFE_RELEASE_SPTR( it->second );
	}
	m_TrailList.clear();

	SAFE_DELETE(m_pStateBlowSignalProcessor);

	SAFE_RELEASE_SPTR(m_hRlktAura);
}

void CDnActor::InsertOctreeNode()
{
	if( s_pOctree ) {
		if( m_pCurrentNode == NULL ) {
			SSphere Sphere;
			GetBoundingSphere( Sphere );
			m_pCurrentNode = s_pOctree->Insert( this, Sphere );
		}
	}
}

void CDnActor::RemoveOctreeNode()
{
	if( s_pOctree ) {
		if( m_pCurrentNode ) {
			s_pOctree->Remove( this, m_pCurrentNode );
			m_pCurrentNode = NULL;
		}
	}
}

bool CDnActor::Initialize()
{
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	// Environment Effect Load
	MAEnvironmentEffect::Initialize( m_nClassID );
#endif

#ifdef _SHADOW_TEST
	if( m_bEnableShadowActor ) {
		m_hShadowActor = CreateActor( m_nClassID, false, false, false );
		m_hShadowActor->EnableShadowActor( false );
		m_hShadowActor->Initialize();
		m_hShadowActor->SetAlphaBlend( 0.5f );
		m_hShadowActor->SetMaxHP( 10000 );
		m_hShadowActor->SetHP( 10000 );
		m_hShadowActor->SetShadowActor( true );
	}
#endif	
	InitializeHeadLook( GetObjectHandle() );

	if( IsPlayerActor() )
		MakePassiveSkillActionInfo();

	return true;
}

// Static Func
bool CDnActor::InitializeClass()
{
	if( !CDnWorld::GetInstance().IsActive() ) return false;

	float fCenter, fSize;
	CDnWorld::GetInstance().CalcWorldSize( fCenter, fSize );

	SAFE_DELETE( s_pOctree );
	s_pOctree = new CEtOctree<CDnActor*>;
	s_pOctree->Initialize( EtVector3( 0.f, fCenter, 0.f ), fSize );

	s_dwUniqueCount = 0;

	CDnStateBlow::InitializeClass();

	return true;
}

bool CDnActor::OnReleaseActorCallback( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
{
	CDnActor *pActor = (CDnActor*)pThis;
	SAFE_DELETE( pActor );
	return true;
}

int CDnActor::OnReleaseActorUniqueID( void *pParam, int nSize )
{
	return *((int*)pParam);
}

bool CDnActor::OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize )
{
	return false;
}

void CDnActor::ProcessClass( LOCAL_TIME LocalTime, float fDelta )
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		s_pVecProcessList[i]->SyncClassTime( LocalTime );
		s_pVecProcessList[i]->Process( LocalTime, fDelta );

		SSphere Sphere;
		s_pVecProcessList[i]->GetBoundingSphere( Sphere );
		s_pVecProcessList[i]->m_pCurrentNode = s_pOctree->Update( s_pVecProcessList[i], Sphere, s_pVecProcessList[i]->m_pCurrentNode );

		if( s_pVecProcessList[i]->IsDestroy() ) {
			bool bEnableDestroy = true;
			if( s_pVecProcessList[i]->IsPlayerActor() ) {
				MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody*>(s_pVecProcessList[i]);
				if( pPartsBody && pPartsBody->IsCombiningParts() ) {
					bEnableDestroy = false;
				}
			}

			if(s_pVecProcessList[i]->IsWaitingThread()) // 이 액터 자체가 다른 쓰레드와 엮여있을경우 호출이 끝난이후에 삭제 해 주도록 한다.'
				continue;

			if( bEnableDestroy ) 
			{
				if( s_pVecProcessList[i]->IsNpcActor() )
				{
					CDnGameTask* pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
					if( pGameTask )
						pGameTask->RemoveNpcList( s_pVecProcessList[i]->GetActorHandle() );
				}

#ifdef PRE_MOD_PRELOAD_MONSTER
				bool bCheck = false;
				if( s_pVecProcessList[i]->IsMonsterActor() )
				{
					CDnGameTask* pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
					if( pGameTask && pGameTask->IsNoPreloadMonsterMap() && pGameTask->IsSyncComplete() )
					{
						CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor *>( s_pVecProcessList[i]->GetActorHandle().GetPointer() );
						if( pMonster && pMonster->GetTeam() != 3 )
						{
							//rlkt_test
							if (pMonster->GetGrade() == CDnMonsterState::Boss || pMonster->GetGrade() == CDnMonsterState::BossHP4  )
								bCheck = true;
						}
					}
				}
#endif
				s_pVecProcessList[i]->Release();
				i--;
#ifdef PRE_MOD_PRELOAD_MONSTER
				if( bCheck )
				{
					if( CEtActionCoreMng::IsActive() ) CEtActionCoreMng::GetInstance().FlushWaitDelete();
					CEtResource::FlushWaitDelete();
				}
#endif
			}
			else {
				// 액터 클래스에서 PlayerActor로 가지고 있는게 좀 별로이긴 하나 계속해서 dynamic_cast 하는건 더 별로라 한번 변환후 push해두도록 하겠다.
				CDnPlayerActor *pPlayerActor = dynamic_cast<CDnPlayerActor*>(s_pVecProcessList[i]);
				if( pPlayerActor ) {
					s_pVecDeleteList.push_back( pPlayerActor );		// 이미 leave와 동시에 SetDestroy호출로 인해 UniqueID는 -1로 설정, Find맵에선 이미 빠져있다.
					s_pVecProcessList[i]->SetProcess( false );
					i--;
				}

				// 더 정확하게 하려면 LoadingTask에 쌓여있는 명령들을 뒤져서 해당 액터가 계속해서 보내둔 장비교환 명령들을 찾아 제거해야하지만,
				// LoadingTask에 쌓여있는 명령에 타입같은 걸 넣어두지 않았기때문에, 입장중 로딩인지 장비교환인지를 명확하게 구분하기 어렵다.
				// 사이즈로 구분할 수도 있긴 하지만, 입장 후 보낸 장비교환일수도 있기 때문에 그냥 다 찾아 지우기도 애매하다.
				// 그런데 어차피 생각해보면,
				// 이 큐에 쌓여있는 명령들을 꺼내와 수행할때 세션ID를 가지고 주변액터 Find할텐데,
				// 이미 Find리스트에서 빠져있기때문에, 액터 찾는데서 아무것도 못찾고 넘어갈거다.(더이상 컴바인 수행되지 않는다.)
			}
		}
	}
	for( DWORD i=0; i<s_pVecDeleteList.size(); i++ ) {
		if( s_pVecDeleteList[i]->IsCombiningParts() ) {
			// 끝날때까지 기다리는 수밖에...
		}
		else {
			delete s_pVecDeleteList[i];
			s_pVecDeleteList.erase( s_pVecDeleteList.begin() + i );
			--i;
		}
	}
}

void CDnActor::ProcessFlushPacketQueue()
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		s_pVecProcessList[i]->FlushPacketQueue( -1 );
	}
}

void CDnActor::ReleaseClass()
{
	SAFE_DELETE( s_pOctree );
	SAFE_DELETE_MAP( s_dwMapActorSearch );

 	DeleteAllProcessObject();
	DeleteAllActor();

	CDnStateBlow::ReleaseClass();
}

int CDnActor::ScanActor( EtVector3 &vPos, float fRadius, DNVector(CDnActor*) &VecList )
{
	SSphere Sphere;
	Sphere.Center = vPos;
	Sphere.fRadius = fRadius;
	s_pOctree->Pick( Sphere, VecList );
	return (int)VecList.size();
}

int CDnActor::ScanActor( EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList )
{
	SSphere Sphere;
	Sphere.Center = vPos;
	Sphere.fRadius = fRadius;

	DNVector(CDnActor*) VecNonSmartPtrList;
	VecNonSmartPtrList.reserve( 200 );
	s_pOctree->Pick( Sphere, VecNonSmartPtrList );

	VecList.clear();
	for each( CDnActor *pActor in VecNonSmartPtrList ) 
	{
		VecList.push_back( pActor->GetMySmartPtr() );
	}
	return (int)VecList.size();
}

int CDnActor::ScanActorByActorSize( EtVector3 &vPos, float fRadius, DNVector(DnActorHandle) &VecList, bool bInside/* = false*/, bool bActorSize/* = false*/ )
{
	SSphere Sphere;
	Sphere.Center = vPos;
	Sphere.fRadius = fRadius;

	DNVector(CDnActor*) VecNonSmartPtrList;
	s_pOctree->Pick( Sphere, VecNonSmartPtrList, bInside, bActorSize );

	VecList.clear();
	for each( CDnActor *pActor in VecNonSmartPtrList ) 
	{
		VecList.push_back( pActor->GetMySmartPtr() );
	}
	return (int)VecList.size();
}

DnActorHandle CDnActor::FindActorFromUniqueID( DWORD dwUniqueID )
{
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	std::map<DWORD, DnActorHandle>::iterator it = s_dwMapActorSearch.find( dwUniqueID );
	if( it != s_dwMapActorSearch.end() ) {
		if( !it->second->IsProcess() ) return CDnActor::Identity();
		return it->second;
	}
	return CDnActor::Identity();

}

DnActorHandle CDnActor::FindActorFromName( TCHAR *szName )
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) {
		if( s_pVecProcessList[i]->IsPetActor() ) continue;
		if( !__wcsicmp_l( s_pVecProcessList[i]->GetName(), szName ) ) return s_pVecProcessList[i]->GetMySmartPtr();
	}
	return CDnActor::Identity();
}

DnActorHandle CDnActor::FindActorFromUniqueIDBySmartPtrList( DWORD dwUniqueID )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
	DnActorHandle hResultActor = CDnActor::Identity();
	int nCount = CDnActor::GetItemCount();
	for( int i=0; i<nCount; i++ ) {
		DnActorHandle hActor = CDnActor::GetItem(i);
		if( hActor->GetUniqueID() == dwUniqueID ) {
			hResultActor = hActor;
			break;
		}
	}
	return hResultActor;
}

void CDnActor::AllMonsterKill()
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) 
	{
		DnActorHandle hActor = s_pVecProcessList[i]->GetMySmartPtr();
		
		if ( hActor->GetActorType() > Reserved6 ) 
		{
			hActor->SetHP(0);
			hActor->SetDieDelta(3.0f);
		}
	}
}

void CDnActor::SetAllMonsterDebugDrawing(bool bEnableCollision, bool bEnableBound)
{
	ScopeLock<CSyncLock> Lock(s_Lock);
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) 
	{
		DnActorHandle hActor = s_pVecProcessList[i]->GetMySmartPtr();
		CDnActor* pActor = hActor->GetMySmartPtr();

		MAActorRenderBase* pRenderBase = dynamic_cast<MAActorRenderBase*>(pActor);
		if ( pRenderBase && pRenderBase->GetObjectHandle() )
		{
			pRenderBase->GetObjectHandle()->ShowBoundingBox(bEnableBound );
			pRenderBase->GetObjectHandle()->ShowCollisionPrimitive(bEnableCollision );
		}
	}
}

// 상속받아서 Process를 사용하는 경우 CDnActor::Process 호출 이후 프로세스에서 포지션 갱신이나 위치이동 처리를 포함한 경우 이펙트 싱크가 어긋나기때문에
// ProcessActor() 를 호출해주시고 포지션 갱신이 모두 완료된 이후 시점에서 에 ProcessVisual을 호출해주세요.
void CDnActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessActor( LocalTime , fDelta );
	ProcessVisual( LocalTime, fDelta );
}

void CDnActor::ProcessActor( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActionBase::ProcessAction( LocalTime, fDelta );

	if( 0 < GetCantXZMoveSEReferenceCount() )
		m_vAniDistance.x = m_vAniDistance.z = 0.0f;

	OnSkillProcess( LocalTime, fDelta );

	ProcessDown( LocalTime, fDelta );
	ProcessDie( LocalTime, fDelta );
	ProcessStiff( LocalTime, fDelta );
	ProcessPlaySpeed( LocalTime, fDelta );
	ProcessState( LocalTime, fDelta );
	ProcessPress();
	ProcessSoftAppear( LocalTime );

	ProcessRotate( LocalTime, fDelta ); // Rotate.

	if (m_pStateBlowSignalProcessor && !IsDie())
		m_pStateBlowSignalProcessor->Process(LocalTime, fDelta);

	for( int i=0; i<CDnWeapon::EquipSlot_Amount; i++ )
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
	//rlkt_aura
	ProcessRlktAura(fDelta);
}

void CDnActor::ProcessVisual( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActionSignalImp::Process( LocalTime, fDelta );
	m_bHideExposureInfo[0] = IsSignalRange( STE_HideExposureInfo );

	CDnHeadEffectRender::Process( LocalTime, fDelta );

#ifdef _SHADOW_TEST
	if( !CGlobalValue::GetInstance().m_bEnableShadow ) 
	{
		if( m_hShadowActor ) 
			m_hShadowActor->Show( false );
		return;
	}
	else 
	{
		if( m_hShadowActor ) 
			m_hShadowActor->Show( true );
	}
	if( m_bEnableShadowActor && m_hShadowActor ) 
	{
		m_hShadowActor->Process( LocalTime, fDelta );
		m_hShadowActor->SetAlphaBlend( 0.5f );
	}
#endif
}

void CDnActor::SetPosition( EtVector3 &vPos )
{
	m_matexWorld.m_vPosition = vPos;
	m_vStaticPos = vPos;
}

void CDnActor::SetStaticPosition( EtVector3 &vPos )
{
	m_vStaticPos = vPos;
}

EtVector3 *CDnActor::GetStaticPosition()
{
	return &m_vStaticPos;
}

void CDnActor::SetPrevPosition( EtVector3 &vPos )
{
	m_vPrevPos = vPos;
}

EtVector3 *CDnActor::GetPrevPosition()
{
	return &m_vPrevPos;
}

void CDnActor::GetBoundingSphere( SSphere &Sphere, bool bActorSize/* = false*/ )
{
	MAActorRenderBase *pRenderBase = dynamic_cast<MAActorRenderBase *>(this);
	if( pRenderBase ) {
		if( pRenderBase->GetBoundingSphere( Sphere ) == false ) {
			Sphere.Center = m_matexWorld.m_vPosition;
			Sphere.fRadius = (float)GetUnitSize();
		}
	}
	else {
		Sphere.Center = m_matexWorld.m_vPosition;
		Sphere.fRadius = (float)GetUnitSize();
	}

	if (bActorSize = true)
	{
		Sphere.Center = FindAutoTargetPos();
		Sphere.fRadius = (float)GetUnitSize();
	}
}

void CDnActor::GetBoundingBox( SAABox &Box )
{
	MAActorRenderBase *pRenderBase = dynamic_cast<MAActorRenderBase *>(this);
	if( pRenderBase ) {
		if( pRenderBase->GetBoundingBox( Box ) == false ) {
			Box.Min = m_matexWorld.m_vPosition + EtVector3( -50.f, -50.f, -50.f );
			Box.Max = m_matexWorld.m_vPosition + EtVector3( 50.f, 50.f, 50.f );
		}
	}
	else {
		Box.Min = m_matexWorld.m_vPosition + EtVector3( -50.f, -50.f, -50.f );
		Box.Max = m_matexWorld.m_vPosition + EtVector3( 50.f, 50.f, 50.f );
	}
}

EtVector3 CDnActor::GetHeadPosition()
{
	EtVector3 vPos = m_matexWorld.m_vPosition;
	EtMatrix HeadMat = GetBoneMatrix("Bip01 Head");
	if(HeadMat._42 != 0.f ) {
		vPos = *(EtVector3*)&HeadMat._41;
	}
	else {
		SAABox box;
		GetBoundingBox(box);
		float fHeight = box.Max.y - box.Min.y;
		vPos.y += fHeight;
	}
	return vPos;
}

EtVector3 CDnActor::GetBodyPosition()
{
	EtVector3 vPos = m_matexWorld.m_vPosition;
	EtMatrix HeadMat = GetBoneMatrix("Bip01");
	if(HeadMat._42 != 0.f ) {
		vPos = *(EtVector3*)&HeadMat._41;
	}
	else {
		SAABox box;
		GetBoundingBox(box);
		float fHeight = ( box.Max.y - box.Min.y ) / 2.f;
		vPos.y += fHeight;
	}
	return vPos;
}

#ifdef _SOCIAL_NEXT_ACTION_KALLISTE
void CDnActor::SetSocialCtrl(int nCondition, const char* szNextActionName)
{
	m_SocialActCtrler.Clear();
	m_SocialActCtrler.nCondition = nCondition;
	m_SocialActCtrler.szNextActionName = szNextActionName;
}
#endif

void CDnActor::OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex )
{
#ifdef _SHADOW_TEST
	if( IsShadowActor() && Type != STE_MoveY ) return;
#endif _SHADOW_TEST

	CDnActionSignalImp::OnSignal( Type, pPtr, LocalTime, SignalStartTime, SignalEndTime, nSignalIndex );

	switch( Type ) {
		case STE_Hit:
			{
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

				if( CDnWeapon::EquipSlot_Amount == pStruct->nWeaponIndex )
					break;

				if( !GetActiveWeapon( pStruct->nWeaponIndex ) ) break;
				if( pStruct->szActionName ) {
					if( GetActiveWeapon( pStruct->nWeaponIndex )->IsExistAction( pStruct->szActionName ) ) {
						// 소서리스 무기처럼 자체 애니가 있을경우 그걸 블렌드 하면 애니가 튀는 현상이 나온다.
						// 주로 PullOut, PutIn 애니에서 나는거라 이때는 블렌드값을 0.0으로 설정해준다.
						// 이렇게 처리해도 캐시무기-일반무기 타입다를때는 애니 틀어져서 튀는 현상이 발생하기도 한다. 애니팀에서도 어쩔수 없다고 한다.
						float fBlendFrame = 3.0f;
						if( strstr( GetActiveWeapon( pStruct->nWeaponIndex )->GetCurrentAction(), "_Weapon" ) ) fBlendFrame = 0.0f;
						GetActiveWeapon( pStruct->nWeaponIndex )->SetActionQueue( pStruct->szActionName, pStruct->nLoopCount, fBlendFrame );
					}
				}

			}
			break;
		case STE_EnvironmentEffect:
			{
				// #41767 플레이어인 경우 은신 상태효과가 있으면 EnvironmentEfefct 시그널 처리 안함.
				if( IsPlayerActor() &&
					IsAppliedThisStateBlow( STATE_BLOW::BLOW_073 ) )
					return;

				EnvironmentEffectStruct *pStruct = (EnvironmentEffectStruct *)pPtr;
				MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(this);
				if( !pRender ) break;

				MatrixEx Cross;
				if( strlen(pStruct->szBoneName) ) {
					EtMatrix mat = pRender->GetBoneMatrix( pStruct->szBoneName );
					memcpy( &Cross.m_vXAxis, &mat._11, sizeof(EtVector3) );
					memcpy( &Cross.m_vYAxis, &mat._21, sizeof(EtVector3) );
					memcpy( &Cross.m_vZAxis, &mat._31, sizeof(EtVector3) );
					memcpy( &Cross.m_vPosition, &mat._41, sizeof(EtVector3) );
				}
				else {
					Cross = m_matexWorld;
				}
				Cross.m_vPosition += Cross.m_vXAxis * pStruct->vOffset->x;
				Cross.m_vPosition += Cross.m_vYAxis * pStruct->vOffset->y;
				Cross.m_vPosition += Cross.m_vZAxis * pStruct->vOffset->z;
				Cross.RotateYaw( -pStruct->vRotate->y );
				Cross.RotatePitch( pStruct->vRotate->x );
				Cross.RotateRoll( pStruct->vRotate->z );

				ConditionEnum Condition = (ConditionEnum)pStruct->nCondition;
				float fHeight;
				TileTypeEnum TileType = CDnWorld::GetInstance().GetTileType( m_matexWorld.m_vPosition, &fHeight );

				if( TileType == TileTypeEnum::Water ) Cross.m_vPosition.y = fHeight;

				CreateEnviEffectParticle( Condition, TileType, &Cross );
				CreateEnviEffectSound( Condition, TileType, Cross.m_vPosition );
			}
			break;
		case STE_ShowWeapon:
			{
				ShowWeaponStruct *pStruct = (ShowWeaponStruct *)pPtr;

				ShowWeapon( pStruct->nEquipIndex, ( pStruct->bEnable == TRUE ) ? true : false );
			}
			break;
		case STE_AlphaBlending:
			{
				MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(this);
				AlphaBlendingStruct *pStruct = (AlphaBlendingStruct *)pPtr;
				float fValue = 1.f / ( SignalEndTime - SignalStartTime - 16.6666f ) * ( LocalTime - SignalStartTime );
				fValue = EtClamp( fValue, 0.0f, 1.0f );
				float fCurAlpha = pStruct->fStartAlpha + ( ( pStruct->fEndAlpha - pStruct->fStartAlpha ) * fValue );
				fCurAlpha = EtClamp( fCurAlpha, 0.0f, 1.0f );
				pRender->SetAlphaBlend( fCurAlpha ,  AL_SIGNAL );
			}
			break;
		case STE_CameraEffect_Shake:
			{
				if( !IsShow() ) break;
				CameraEffect_ShakeStruct *pStruct = (CameraEffect_ShakeStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				if( s_hLocalActor ) {
					float fDistance = EtVec3Length( &EtVector3( *GetPosition() - *s_hLocalActor->GetPosition() ) );
					if( fDistance <= pStruct->fRange ) {
						float fRolloffDist = pStruct->fRange * pStruct->fRolloff;
						float fRatio = 1.f;

						if( fDistance > fRolloffDist ) 
							fRatio = 1.f - 1.f / ( pStruct->fRange - fRolloffDist ) * ( fDistance - fRolloffDist );

						float fLength = (float)( SignalEndTime - SignalStartTime );
						if( IsModifyPlaySpeed() ) {
							float fTemp = CDnActionBase::m_fFps / 60.f;
							fLength *= fTemp;
						}
						DWORD dwTime = (DWORD)( ( fLength - 16.6666f ) * fRatio );
						pStruct->nCameraEffectRefIndex = hCamera->Quake( dwTime, pStruct->fBeginRatio, pStruct->fEndRatio, pStruct->fShakeValue * fRatio );
					}
				}
			}
			break;
		case STE_CameraEffect_KeyFrame:
			{
				if( !IsShow() ) break;
				CameraEffect_KeyFrameStruct * pStruct = (CameraEffect_KeyFrameStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				if( s_hLocalActor )
				{
					pStruct->nCameraEffectRefIndex = hCamera->KeyFrame( pStruct->strCamFile );
				}
			}
			break;
		case STE_CameraEffect_Movement:
			{
				if( !IsShow() ) break;
				CameraEffect_MovementStruct * pStruct = (CameraEffect_MovementStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				if( s_hLocalActor )
				{
					float fLength = (float)( SignalEndTime - SignalStartTime );
					if( IsModifyPlaySpeed() )
					{
						float fTemp = CDnActionBase::m_fFps / 60.0f;
						fLength *= fTemp;
					}
					DWORD dwTime = (DWORD)( fLength - 16.6666f );
					pStruct->nCameraEffectRefIndex = hCamera->Movement( dwTime, *(pStruct->vDir), pStruct->fSpeedBegin, pStruct->fSpeedEnd );
				}
			}
			break;
		case STE_CameraEffect_Swing:
			{
				if( !IsShow() ) break;	
				CameraEffect_SwingStruct * pStruct = (CameraEffect_SwingStruct *)pPtr;
				DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
				if( !hCamera ) break;
				if( hCamera->GetCameraType() != CDnCamera::PlayerCamera ) break;
				if( hCamera->FindCameraEffect( pStruct->nCameraEffectRefIndex ) ) break;

				if( s_hLocalActor )
				{
					float fLength = (float)( SignalEndTime - SignalStartTime );
					if( IsModifyPlaySpeed() ){
						float fTemp = CDnActionBase::m_fFps / 60.0f;
						fLength *= fTemp;
					}
					DWORD dwTime = (DWORD)( fLength - 16.6666f );
					pStruct->nCameraEffectRefIndex = hCamera->Swing( dwTime, pStruct->fAngle, pStruct->fSpeed, pStruct->bSmooth );
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
				OutputDebug( "Motion Speed : %d %d~%d : %d\n", nSignalIndex, (int)SignalStartTime, (int)SignalEndTime, (int)LocalTime );
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
				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
					break;

				// 게임에서는 현재 플레이어의 모든 발사체는 클라에서 게임서버로 보내므로 플레이어 자기 자신의 발사체 시그널 이외에는 처리하지 않는다.
				// (무기의 발사체 시그널도 마찬가지)
				if( GetActorType() <= CDnActorState::Reserved6 )
				{
					// 차져 상태효과 같은경우 로컬기준이 아닌 다른 객체의 기준으로 발사체 시그널이 수행될수 있는데
					// 이렇게되면 로컬이 아니기때문에 여기서 리턴되어 버리면 이펙트나 발사체의 표현이 수행되지않는다 , 그래서 표현을 위해서 여기서 차져로 인해 발생한 경우
					// 발사체를 표현하도록 한다 , 하지만 서버로 패킷은 보내지 않는다.
					if( CDnActor::s_hLocalActor != GetMySmartPtr() && !m_bOnSignalFromChargerSE )
						break;
				}

				ProjectileStruct *pStruct = (ProjectileStruct *)pPtr;

				//////////////////////////////////////////////////////////////////////////
				//낙인 효과가 있는 액터의 위치로 날아 가도록한다.
				DNVector(DnActorHandle) StigmaActorList;
				if (pStruct->nTargetStateIndex != 0)
				{
					ScanActorByStateIndex(StigmaActorList, STATE_BLOW::BLOW_246);
				}

				//낙인 대상이 있는 경우만 처리
				if (StigmaActorList.empty() == false)
				{
					int nStigmaActorCount = (int)StigmaActorList.size();
					for (int i = 0; i < nStigmaActorCount; ++i)
					{
						//타겟 액터..
						DnActorHandle hTargetActor = StigmaActorList[i];

						MatrixEx LocalCross = *GetMatEx();
						CDnProjectile *pProjectile = CDnProjectile::CreateProjectile( GetMySmartPtr(), LocalCross, pStruct, EtVector3(0.0f, 0.0f, 0.0f), hTargetActor );
						if( pProjectile == NULL ) break;
						pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

						if( m_bOnSignalFromChargerSE && m_hChargetDestActor )
						{
							DNVector(DnBlowHandle) vlhChargerBlows;
							m_hChargetDestActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_062, vlhChargerBlows );	// 1개 이상 중복되지 않는 게 좋은데 기획에서 어찌 할지.
							for( int i = 0; i < (int)vlhChargerBlows.size(); ++i )
								static_cast<CDnChargerBlow*>( vlhChargerBlows.at( i ).GetPointer() )->OnProjectileSignal( pProjectile );
						}

						// 차져 상태효과는 서버/클라 따로 발사체를 날리므로 서버로 패킷 보내지 않는다.
						if( false == m_bOnSignalFromChargerSE )
						{
#ifdef PRE_MOD_PROJECTILE_HACK
							OnProjectile( pProjectile );
#else
							OnProjectile( pProjectile, nSignalIndex );
#endif
							OnSkillProjectile( pProjectile );
						}

						if( GetActorType() == PropActor )
						{
							CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( this );
							pProjectile->SetShooterProp( pMonsterActor->GetPropHandle() );
						}

						pProjectile->ShowWeapon( IsShow() );
					}
				}
				//낙인 대상이 없으면 기존과 같이 처리
				else
				{
					//#52808 - 추가 요청 (낙인용 발사체 설정이 2인 경우 낙인 대상이 없으면 발사체 생성 안됨.
					if (pStruct->nTargetStateIndex == 2)
						break;

					MatrixEx LocalCross = *GetMatEx();
					CDnProjectile *pProjectile = CDnProjectile::CreateProjectile( GetMySmartPtr(), LocalCross, pStruct );
					if( pProjectile == NULL ) break;
					pProjectile->SetShooterType( GetMySmartPtr(), m_nActionIndex, nSignalIndex );

					if( m_bOnSignalFromChargerSE && m_hChargetDestActor )
					{
						DNVector(DnBlowHandle) vlhChargerBlows;
						m_hChargetDestActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_062, vlhChargerBlows );	// 1개 이상 중복되지 않는 게 좋은데 기획에서 어찌 할지.
						for( int i = 0; i < (int)vlhChargerBlows.size(); ++i )
							static_cast<CDnChargerBlow*>( vlhChargerBlows.at( i ).GetPointer() )->OnProjectileSignal( pProjectile );
					}

					// 차져 상태효과는 서버/클라 따로 발사체를 날리므로 서버로 패킷 보내지 않는다.
					if( false == m_bOnSignalFromChargerSE )
					{
#ifdef PRE_MOD_PROJECTILE_HACK
						OnProjectile( pProjectile );
#else
						OnProjectile( pProjectile, nSignalIndex );
#endif
						OnSkillProjectile( pProjectile );
					}

					if( GetActorType() == PropActor )
					{
						CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( this );
						pProjectile->SetShooterProp( pMonsterActor->GetPropHandle() );
					}

					pProjectile->ShowWeapon( IsShow() );
					//rlkt_archerprojectile
					if (pStruct->nWeaponTableID == 2022)
					{
						pStruct->nWeaponTableID = 2023;
			
					}
					//
				}
			}
			break;
		case STE_CanRotate:
			{
				CanRotateStruct *pStruct = (CanRotateStruct *)pPtr;
				SetRotatable( ( pStruct->bRotate == TRUE ) ? true : false );
			}
			break;
		case STE_ObjectVisible:
			{
				ObjectVisibleStruct *pStruct = (ObjectVisibleStruct *)pPtr;
				Show( ( pStruct->bShow == TRUE ) ? true : false );
			}
			break;
		case STE_ShowSwordTrail:
			{
				ShowSwordTrailStruct *pStruct = (ShowSwordTrailStruct *)pPtr;
				if( pStruct->nWeaponIndex < CDnWeapon::EquipSlot_Amount ) { // 0 , 1
					if( GetActiveWeapon( pStruct->nWeaponIndex ) )
						GetActiveWeapon( pStruct->nWeaponIndex )->ShowTrail( SignalEndTime - LocalTime, pStruct );
				}
				else if( pStruct->nWeaponIndex >= 4 ) {	// 4, 5, 6, 7

					EtTextureHandle hTex = ( pStruct->nTextureIndex != -1 ) ? CEtResource::GetSmartPtr(  pStruct->nTextureIndex ) : CEtTexture::Identity();
					EtTextureHandle hNormalTex = ( pStruct->nNormalTextureIndex != -1 ) ? CEtResource::GetSmartPtr(  pStruct->nNormalTextureIndex ) : CEtTexture::Identity();

					if( m_TrailList.count( pStruct->nWeaponIndex ) == 0 ) {
						float fLifeTime = ( pStruct->fLifeTime == 0.f ) ? 0.2f : pStruct->fLifeTime;
						float fMinSegment = ( pStruct->fMinSegment == 0.f ) ? 5.0f : pStruct->fMinSegment;
						m_TrailList[ pStruct->nWeaponIndex ] = EternityEngine::CreateSwordTrail( GetObjectHandle(), hTex, hNormalTex, fLifeTime, fMinSegment );						
					}
					else {						
						m_TrailList[ pStruct->nWeaponIndex ]->ChangeTexture( hTex, hNormalTex );						
					}
					m_TrailList[ pStruct->nWeaponIndex ]->SetBlendOP( (EtBlendOP)( pStruct->nBlendOP + 1 ) );
					m_TrailList[ pStruct->nWeaponIndex ]->SetSrcBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nSrcBlend, 1, 0, 4 ) + 1 ) );
					m_TrailList[ pStruct->nWeaponIndex ]->SetDestBlend( (EtBlendMode)( GetValue2ExceptionSwap( pStruct->nDestBlend, 1, 0, 5 ) + 1 ) );
					m_TrailList[ pStruct->nWeaponIndex ]->AddPoint( pStruct->nWeaponIndex - 4, (int)(SignalEndTime - LocalTime) );
				}
			}
			break;
		case STE_ChangeWeaponLink:
			{
				if( CGlobalInfo::GetInstance().IsPlayingCutScene() )
				{
					ChangeWeaponLinkStruct* pStruct = (ChangeWeaponLinkStruct *)pPtr;
					if( pStruct->szLinkBoneName ) {
						if( GetActiveWeapon( pStruct->nWeaponIndex ) )
							GetActiveWeapon( pStruct->nWeaponIndex )->ChangeLinkBone( pStruct->szLinkBoneName );
					}
				}
			}
			break;
		case STE_SocialAction:
			{
				if( !IsShow() ) break;
				SocialActionStruct *pStruct = (SocialActionStruct *)pPtr;

				if( CGlobalInfo::GetInstance().IsPlayingCutScene() && (pStruct->nRandom < 100) )
					break;

				if( _rand()%100 < pStruct->nRandom ) 
				{
					SetFaceAction( pStruct->szActionName );
#ifdef _SOCIAL_NEXT_ACTION_KALLISTE
					if (pStruct->nCondition != 0)
						SetSocialCtrl(pStruct->nCondition, pStruct->szNextActionName);
					else
						m_SocialActCtrler.Clear();
#endif
				}
			}
			break;
		case STE_HeadLook:
			{
				HeadLookStruct *pStruct = (HeadLookStruct *)pPtr;
				OnSignalHeadLook( pStruct );
			}
			break;
		case STE_Destroy:
			{
				SetDestroy();
			}
			break;
		case STE_Gravity:
			{
				if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) break;
				GravityStruct *pStruct = (GravityStruct *)pPtr;

				MatrixEx CrossOffset = m_matexWorld;
				CrossOffset.MoveLocalXAxis( pStruct->vOffset->x );
				CrossOffset.MoveLocalYAxis( pStruct->vOffset->y );
				CrossOffset.MoveLocalZAxis( pStruct->vOffset->z );

				DNVector(DnActorHandle) hVecList;
				CDnActor::ScanActor( CrossOffset.m_vPosition, pStruct->fDistanceMax, hVecList );

#ifdef PRE_ADD_GRAVITY_PROPERTY
				EtVector3 vPos = CrossOffset.m_vPosition;

				float fXZDistanceMaxSQ = pStruct->fDistanceMax;
				float fXZDistanceMinSQ = pStruct->fDistanceMin;

				fXZDistanceMaxSQ *= fXZDistanceMaxSQ;
				fXZDistanceMinSQ *= fXZDistanceMinSQ;
	
				SAABox Box;
				float fDot = 0.0f;
				EtVector3 vZVec = m_matexWorld.m_vZAxis;

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
					DnActorHandle hActor = hVecList[i];
					EtVector3 vDirActorToPos;
					vDirActorToPos = *hActor->GetPosition() - vPos;
					vDirActorToPos.y = 0.f;

					hActor->GetBoundingBox( Box );

					bool bCheckArea = true;
					float fSQDistance = SquaredDistance( vPos, Box, true );

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
						hVecList[i]->SetVelocityZ( 0.0f );
						hVecList[i]->SetResistanceZ( 0.0f );
						hVecList[i]->ResetLook();
						continue;
					}
#endif // PRE_ADD_GRAVITY_PROPERTY

					MAMovementBase *pMovement = (hVecList[i]->GetMovement());
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
					if( pMovement ) 
					{
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
					// 조금 애매하긴한데, 액션툴에서는 기획자가 구간을 보기 편하게 MoveY 시그널이 영역을 갖고 있지만 
					// 사실은 액션당 같은 방향은 한번만 셋팅되도록 처리된다. 내부적으로 Y 방향이 달라지면 변경은 되지만
					// 같은 방향은 계속 이쪽에서 Movement 쪽으로 함수가 호출되더라도 한번만 등록된다.
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
				float fLength = (float)( SignalEndTime - SignalStartTime ) * (CDnActionBase::m_fFps / 60.0f); 
				DWORD dwTime = (DWORD)( fLength - 16.6666f );
				SetRotate( dwTime, pStruct->fStartSpeed, pStruct->fEndSpeed, *(pStruct->vAxis), pStruct->bLeft == TRUE ? true : false );
			}
			break;
		case STE_ChangeWeapon:
			{
				//무기해제 상태효과가 적용 되어 있으면 무시됨..
				if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_237))
					break;

				ChangeWeaponStruct* pStruct = (ChangeWeaponStruct*)pPtr;
				if (pStruct == NULL)
					break;

				DnWeaponHandle hNewWeapon = CDnWeapon::CreateWeapon(pStruct->nWeaponID, 0);
				if (hNewWeapon)
					AttachWeapon(hNewWeapon, pStruct->nWeaponIndex, true);
			}
			break;
		//rlkt_test aura on tumble.
		case STE_RLKT_Aura:
		{
					if (m_hRlktAura)
					{
						m_hRlktAura->Show(false);
					}

					m_hRlktAura = CEtOutlineObject::Create(GetAniObjectHandle());

					m_fRlktAuraDelayTime = 1.f;	
					m_hRlktAura->SetColor(EtColor(((float)rand() / (float)(RAND_MAX)) * 1.0f, ((float)rand() / (float)(RAND_MAX)) * 1.0f, ((float)rand() / (float)(RAND_MAX)) * 1.0f, 1.0));
					m_hRlktAura->Show(true, 1.0f);
					m_bProcessRlktAura = true;

					//hOutline->SetWidth(((float)rand() / (float)(RAND_MAX)) * 2.0f);
					//hOutline->ProcessIntensity(2.0f);
					//if (pStruct->nType == 1)
					//	hOutline->SkipProcessIntensity();

					//				hOutline->SetType( (CEtOutlineObject::OutlineTypeEnum)pStruct->nType );
					//				pStruct->nOutlineIndex = hOutline->GetMyIndex();
					
					//hOutline->Release();
		}
			break;
	}
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
				return false;
		}
		else {
			if( strcmp( szName, m_szActionQueue.c_str() ) == NULL ) 
				return false;
		}
	}
	
	int nCurrentActionIndex = GetElementIndex( szName );
	if( bCheckStateEffect )
	{
		int nActionState = GetState( szName );

		// State에 따라 세부적으로 나뉜다~
		// 공격불가일 땐 공격 state 인 액션은 사용 못함.
		if( (nActionState & ActorStateEnum::Attack) && !( nActionState & ActorStateEnum::IgnoreCantAction) )
		{
			if( (GetStateEffect() & CDnActorState::Cant_AttackAction) == CDnActorState::Cant_AttackAction ) 
			{
				if( !IsDie() ) 
					return false;
			}
		}
	}


#ifdef PACKET_DELAY
	/*
	if( dynamic_cast<CDnLocalPlayerActor*>(this) == NULL )
		CDnActionBase::SetActionQueue( szName, nLoopCount, fBlendFrame, fStartFrame );
	else CDnActionBase::SetActionQueueDelay( szName, nLoopCount, fBlendFrame, fStartFrame, 100 );
	*/
	CDnActionBase::SetActionQueue( szName, nLoopCount, fBlendFrame, fStartFrame );
#else
  	CDnActionBase::SetActionQueue( szName, nLoopCount, fBlendFrame, fStartFrame );
#endif //PACKET_DELAY
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
	const char *szName = szActionName;
	if( m_hToggleSkill && m_hToggleSkill->IsToggleOn() )
	{
		CDnChangeActionStrProcessor* pChangeAction = static_cast<CDnChangeActionStrProcessor*>( m_hToggleSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		if( pChangeAction ) {
			szName = pChangeAction->GetChangeActionName( szActionName );
		}
	}
	CDnActionBase::SetCustomAction( szName, fFrame );
}

void CDnActor::ProcessDown( LOCAL_TIME LocalTime, float fDelta )
{
	if( IsDie() ) return;
	if( GetState() == CDnActorState::Down && m_fDownDelta > 0.f ) 
	{
		m_fDownDelta -= ( fDelta * ( ( 1.f / 60.f ) * CDnActionBase::GetFPS() ) );
		if( m_fDownDelta <= 0.f ) {
			m_fDownDelta = 0.f;
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

void CDnActor::CmdAction( const char *szActionName, int nLoopCount, float fBlendFrame, bool bCheckOverlapAction, bool bFromStateBlow/* = false*/, bool bSkillChain/* = false*/  )
{
	SetActionQueue( szActionName, nLoopCount, fBlendFrame, 0.f, bCheckOverlapAction );
}

bool CDnActor::_CheckEffectDuplicate( DnSkillHandle hNewSkill )
{
	bool bResult = true;

	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	// TOOD: 이펙트 출력 구분 인덱스에 따라 출력 구분 적용
	// 현재 돌아가고 있는 StateBlow 를 전부 체크
	int iNumStateBlow = m_pStateBlow->GetNumStateBlow();

	for( int iBlow = 0; iBlow < iNumStateBlow; ++iBlow )
	{
		DnBlowHandle hExistingBlow = m_pStateBlow->GetStateBlow( iBlow );

		int iExistingSkillID = hExistingBlow->GetParentSkillInfo()->iSkillID;
		int iExistingSkillLevelID = hExistingBlow->GetParentSkillInfo()->iSkillLevelID;
		if( iExistingSkillID < 1 || iExistingSkillLevelID < 1 ) continue;

		int iExistingEffectDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iExistingSkillID, "_EffectDuplicate" )->GetInteger();

		int iEffectDuplicateMethod = hNewSkill->GetEffectDuplicateMethod();

		if( 0 != iEffectDuplicateMethod &&
			iEffectDuplicateMethod == iExistingEffectDuplicateMethod )
		{
			// 이펙트 출력 대체 시켜야 함.
			// 여기서 해당 기존 이펙트 출력을 삭제하면 호출한 함수쪽에서 새로 이펙트를 추가할 것임.
			STATE_BLOW::emBLOW_INDEX ExistingBlowIndex = hExistingBlow->GetBlowIndex();

			bool bValid = TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + ExistingBlowIndex, hExistingBlow->GetBlowID() );
			assert( bValid && "화면에 표시되고 있는 Blow Effect 이지만 TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct> 에 없습니다." );
			if( bValid )
				TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + ExistingBlowIndex, hExistingBlow->GetBlowID() );
		}
	}

	return bResult;
}



bool CDnActor::AttachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex,
							   const EffectOutputInfo* pEffectInfo, /*OUT*/ DnEtcHandle& hEtcObject, /*OUT*/ DNVector(DnEtcHandle)& vlhDummyBoneEtcObjects,
							   bool bAllowSameSkillDuplicate/* = false*/ )
{
	bool bResult = false;

	std::vector<std::string> infoTokens1;
	std::vector<std::string> infoTokens2;
	std::string delimiters = ";";

	TokenizeA(ParentSkillInfo.effectOutputIDs, infoTokens1, delimiters);
	TokenizeA(ParentSkillInfo.debuffEffectOutputIDs, infoTokens2, delimiters);
	
	if (infoTokens1.size() > 1 || infoTokens2.size() > 2)
	{
		bAllowSameSkillDuplicate = true;
	}

	switch( pEffectInfo->iOutputType )
	{
		case EffectOutputInfo::ATTACH:
			{
				if( TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex ) ) {
					TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex );
					m_setNowPlayingEffectSkill.erase( ParentSkillInfo );
				}

				if( bAllowSameSkillDuplicate || m_setNowPlayingEffectSkill.find( ParentSkillInfo ) == m_setNowPlayingEffectSkill.end() )
				{
					hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
					if( hEtcObject->Initialize( pEffectInfo->strSkinFileName.c_str(), pEffectInfo->strAniFileName.c_str(), pEffectInfo->strActFileName.c_str() ) == false ) {
						SAFE_RELEASE_SPTR( hEtcObject );
					}
					else 
					{
						*hEtcObject->GetMatEx() = m_matexWorld;

						// Summon_On 액션이 있다면 실행시켜 주자.
						if( hEtcObject->IsExistAction( "Summon_On" ) )
							hEtcObject->SetActionQueue( "Summon_On" );
						else
							hEtcObject->SetActionQueue( "Idle" );

						hEtcObject->AsStateEffect();		// 상태효과 이펙트로 사용됨
						hEtcObject->SetParentActor( GetMySmartPtr() );
						EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex, hEtcObject );
						pResult->bDefendenceParent = true;
						pResult->bLinkObject = true;
						pResult->vOffset = EtVector3( 0.f, -50.f, 0.f );
						pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
						sprintf_s( pResult->szBoneName, "Bip01" );

						m_setNowPlayingEffectSkill.insert( ParentSkillInfo );						
						bResult = true;
					}
				}
			}
			break;

		case EffectOutputInfo::DUMMY_BONE_ATTACH:
			{	
				if( bAllowSameSkillDuplicate || m_setNowPlayingEffectSkill.find( ParentSkillInfo ) == m_setNowPlayingEffectSkill.end() )
				{
					//bool bAttached = false;
					int nNumBone = (int)pEffectInfo->vlDummyBoneIndices.size();
					for( int nBone = 0; nBone < nNumBone; ++nBone )
					{
						if( TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone ) ) 
						{
							TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone );
							m_setNowPlayingEffectSkill.erase( ParentSkillInfo );
						}

						int iBoneIndex = (int)pEffectInfo->vlDummyBoneIndices.at( nBone );
						_ASSERT( iBoneIndex < Max_FX_Dummy_Bone );
						if( iBoneIndex >= Max_FX_Dummy_Bone )
						{
							OutputDebug( "[CDnActor::AttachSEEffect] 상태효과 이펙트 더미 본에 붙이기 실패했습니다. 본 인덱스 확인! BlowIndex: %d, 현재: %d, 최대: %d\n", 
										 emBlowIndex, iBoneIndex, Max_FX_Dummy_Bone );

							continue;
						}

						// 이 더미본이 이 액터에게 존재한다면 이 이펙트를 붙이고 그렇지 않다면 붙이지 않는다.
						const char* pDummyBoneName = s_acEffectBoneName[ iBoneIndex ];
						bool bExistBone = true;
						GetBoneMatrix( pDummyBoneName, &bExistBone );
						if( false == bExistBone )
						{
							OutputDebug( "[CDnActor::AttachSEEffect] 상태효과 이펙트 더미 본 %s에 붙이기 실패했습니다. 본 인덱스 확인! BlowIndex: %d, 현재: %d\n", 
										  pDummyBoneName, emBlowIndex, iBoneIndex );

							continue;
						}

						hEtcObject = (new CDnEtcObject)->GetMySmartPtr();
						if( hEtcObject->Initialize( pEffectInfo->strSkinFileName.c_str(), pEffectInfo->strAniFileName.c_str(), pEffectInfo->strActFileName.c_str() ) == false ) {
							SAFE_RELEASE_SPTR( hEtcObject );
							continue;
						}
						else 
						{
							*hEtcObject->GetMatEx() = m_matexWorld;
							
							// Summon_On 액션이 있다면 실행시켜 주자.
							if( hEtcObject->IsExistAction( "Summon_On" ) )
								hEtcObject->SetActionQueue( "Summon_On" );
							else
								hEtcObject->SetActionQueue( "Idle" );

							hEtcObject->AsStateEffect();		// 상태효과 이펙트로 사용됨
							hEtcObject->SetParentActor( GetMySmartPtr() );
							EtcObjectSignalStruct *pResult = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, 
																																		 (nIndex*100)+nBone, hEtcObject );

							pResult->bDefendenceParent = true;
							pResult->bLinkObject = true;
							pResult->vOffset = EtVector3( 0.f, 0.f, 0.f );
							pResult->vRotate = EtVector3( 0.f, 0.f, 0.f );
							_ASSERT( nBone < (int)pEffectInfo->vlDummyBoneIndices.size() );
							_strcpy( pResult->szBoneName, _countof(pResult->szBoneName), s_acEffectBoneName[ iBoneIndex ], (int)strlen(s_acEffectBoneName[ iBoneIndex ]));

							// 현재 플레이어 캐릭터에게 쓰이는 버프 아이템은 한번만 나오고 안나오도록 이펙트 설정되어있음. (#11171)
							if( false == IsPlayerActor() )
								m_pStateBlow->AddEffectAttachedBone( pEffectInfo->vlDummyBoneIndices.at(nBone), hEtcObject );

							bResult = true;
						}

						if( hEtcObject )
							vlhDummyBoneEtcObjects.push_back( hEtcObject );
					}

					if( bResult )
						m_setNowPlayingEffectSkill.insert( ParentSkillInfo );
				}
			}
			break;
	}

	return bResult;
}



bool CDnActor::DetachSEEffect( const CDnSkill::SkillInfo& ParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nIndex, 
							   const EffectOutputInfo* pEffectInfo )
{
	bool bResult = false;

	switch( pEffectInfo->iOutputType )
	{
		case EffectOutputInfo::ATTACH:
			if( TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex ) ) 
			{
				DnEtcHandle hEffectHandle = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex );
				if( hEffectHandle ) {
					if( hEffectHandle->IsExistAction( "Summon_Off" ) )
						hEffectHandle->SetActionQueue( "Summon_Off" );		// Destroy 시그널 반드시 존재해야 객체 소멸됨 
					else
						TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex );
				}
				else 
					TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, nIndex );

				m_setNowPlayingEffectSkill.erase( ParentSkillInfo );
				bResult = true;
			}
			break;

		case EffectOutputInfo::DUMMY_BONE_ATTACH:
			{
				int nNumBone = (int)pEffectInfo->vlDummyBoneIndices.size();
				for( int nBone = 0; nBone < nNumBone; ++nBone )
				{
					if( TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone ) ) 
					{
						DnEtcHandle hEffectHandle = TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::GetSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone );

						// 현재 캐릭터에게 쓰이는 버프 아이템은 한번만 나오고 안나오도록 이펙트 설정되어있음. (#11171)
						if( false == IsPlayerActor() )
							m_pStateBlow->DelEffectAttachedBone( pEffectInfo->vlDummyBoneIndices.at(nBone), hEffectHandle );

						if( hEffectHandle ) {
							if( hEffectHandle->IsExistAction( "Summon_Off" ) )
								hEffectHandle->SetActionQueue( "Summon_Off" );	// Destroy 시그널 반드시 존재해야 객체 소멸됨 
							else
								TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone );
						}
						else 
							TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + emBlowIndex, (nIndex*100)+nBone );

						bResult = true;
					}
				}

				if( bResult )
					m_setNowPlayingEffectSkill.erase( ParentSkillInfo );
			}
			break;
	}

	return bResult;
}


bool CDnActor::DetachSEEffectSkillInfo( const CDnSkill::SkillInfo& ParentSkillInfo )
{
	bool bExist = (m_setNowPlayingEffectSkill.find( ParentSkillInfo ) != m_setNowPlayingEffectSkill.end());
	if( bExist )
		m_setNowPlayingEffectSkill.erase( ParentSkillInfo );

	return bExist;
}


DnBlowHandle CDnActor::CmdAddStateEffect( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szParam, bool bOnPlayerInit /*= false*/ )
{
	DnBlowHandle hAddedBlow;

	int nIndex = AddStateBlow( pParentSkillInfo, emBlowIndex, nDurationTime, szParam, bOnPlayerInit );
	
	if( nIndex != -1 )
		hAddedBlow = GetStateBlowFromID( nIndex );
	
	//if( nIndex != -1 && hHitter ) {
	//	hAddedBlow = GetStateBlowFromID( nIndex );

		//DnSkillHandle hSkill = hHitter->FindSkill( pParentSkillInfo->iSkillID );
		//if( !hSkill ) return hAddedBlow;

		//bool bShowEffect = _CheckEffectDuplicate( hSkill );

		//if( bShowEffect )
		//{
		//	DnEtcHandle hHandle = AttachSEEffect( *hSkill->GetInfo(), emBlowIndex, nIndex, hSkill->GetEffectSkinName(), hSkill->GetEffectAniName(), hSkill->GetEffectActName() );
		//	//_ASSERT( hHandle && "상태효과의 Effect action 파일 처리 실패" );
		//}
	//}

	// [2011/03/18 semozz]
	// 상태효과 강제 Begin
	if ( pParentSkillInfo && pParentSkillInfo->bItemPrefixSkill )
		ForceBeginStateBlow(hAddedBlow);

	return hAddedBlow;
}

void CDnActor::CmdRemoveStateEffect( STATE_BLOW::emBLOW_INDEX emBlowIndex, bool bSendServer/* = false*/ )
{
	RemoveStateBlowByBlowDefineIndex( emBlowIndex );
}

void CDnActor::CmdRemoveStateEffectImmediately( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	m_pStateBlow->RemoveImediatlyStateEffectByBlowIndex( emBlowIndex );
}

void CDnActor::CmdMixedAction( const char *szActionBone, const char *szMaintenanceBone, const char *szActionName, int nLoopCount, float fFrame, float fBlendFrame )
{
	const char *szName = szActionName;
	if( m_hToggleSkill && m_hToggleSkill->IsToggleOn() )
	{
		CDnChangeActionStrProcessor* pChangeAction = static_cast<CDnChangeActionStrProcessor*>( m_hToggleSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		if( pChangeAction ) {
			szName = pChangeAction->GetChangeActionName( szActionName );
		}
	}

	int nElementIndex = GetElementIndex( szName );
	if( nElementIndex == -1 ) return;
	int nAniIndex = m_nVecAniIndexList[nElementIndex];
	if( nAniIndex == -1 ) return;

	int nNextActionBlendFrame = GetElement(nElementIndex)->dwBlendFrame;
	AddMixedAnimation( szActionBone, szMaintenanceBone, nAniIndex, fFrame, fBlendFrame, (float)nNextActionBlendFrame, nLoopCount );
}

void CDnActor::OnBeginStateBlow( DnBlowHandle hBlow )
{
}

void CDnActor::OnEndStateBlow( DnBlowHandle hBlow )
{
	//// 결빙 상태효과는 부서지는 액션이 따로 있기 때문에 제외
	//if( STATE_BLOW::BLOW_041 == hBlow->GetBlowIndex() )
	//	return;

	//if( TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::IsExistSignalHandle( STATEBLOWEFFECT_ETCOFFSET + hBlow->GetBlowIndex(), hBlow->GetBlowID() ) )
	//{
	//	TSmartPtrSignalImp<DnEtcHandle,EtcObjectSignalStruct>::RemoveSignalHandle( STATEBLOWEFFECT_ETCOFFSET + hBlow->GetBlowIndex(), hBlow->GetBlowID() );
	//	//m_setNowPlayingEffectSkill.erase( *hBlow->GetParentSkillInfo() );
	//}
}

#ifdef PRE_FIX_77172
void CDnActor::CheckDamageVelocity( DnActorHandle hActor, bool bKeepJumpMovement )
#else // PRE_FIX_77172
void CDnActor::CheckDamageVelocity( DnActorHandle hActor )
#endif // PRE_FIX_77172
{
	MAMovementBase *pMovement = GetMovement();

	EtVector2 vViewVec = EtVec3toVec2( m_HitParam.vViewVec );
	EtVec2Normalize( &vViewVec, &vViewVec );
	if( EtVec2LengthSq( &vViewVec ) > 0.f ) {
		pMovement->Look( vViewVec );
	}

#ifdef PRE_FIX_77172
	EtVector2 vJumpMovement = EtVector2( 0.0f, 0.0f );
	if( bKeepJumpMovement && pMovement->GetJumpMovement() )
	{
		vJumpMovement.x = pMovement->GetJumpMovement()->x;
		vJumpMovement.y = pMovement->GetJumpMovement()->y;
	}
#endif // PRE_FIX_77172

#ifdef PRE_FIX_STUCK_LANDING_ACTION
	if( m_HitParam.vVelocity.y == 0.f && pMovement->GetVelocity() && pMovement->GetVelocity()->y != 0.f && 
		( GetAddHeight() != 0.f ) || ( m_HitParam.szActionName.empty() && ( GetState() & ActorStateEnum::Air ) ) ) 
	{
		m_HitParam.vVelocity.y = pMovement->GetVelocity()->y;
		m_HitParam.vResistance.y = pMovement->GetResistance()->y;
	}
#endif

	pMovement->SetVelocity( m_HitParam.vVelocity );
	pMovement->SetResistance( m_HitParam.vResistance );

#ifdef PRE_FIX_77172
	if( bKeepJumpMovement )
		pMovement->SetJumpMovement( vJumpMovement );
#endif // PRE_FIX_77172
}

void CDnActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	if( IsDie() ) return;

	SetHP( HitParam.nCurrentHP );

	DnActorHandle hHitter = pHitter ? pHitter->GetActorHandle() : CDnActor::Identity();
	m_HitParam = HitParam;
	OnDamageActor( hHitter, HitParam );

	if( GetHP() < 0 ) 
		SetHP( 0 );
}

void CDnActor::OnDamageActor( DnActorHandle hHitter, SHitParam &HitParam )
{
	if( !HitParam.szActionName.empty() )
		SetActionQueue( HitParam.szActionName.c_str(), 0, 3.f, 0.f, false );
	switch( HitParam.HitType ) {
		case CDnWeapon::Normal:
		case CDnWeapon::Critical:
		case CDnWeapon::CriticalRes:
		case CDnWeapon::Stun:
			if( HitParam.bSuccessNormalDamage ) 
			{
#ifdef PRE_FIX_77172
				CheckDamageVelocity( hHitter, HitParam.bKeepJumpMovement );
#else // PRE_FIX_77172
				CheckDamageVelocity( hHitter );
#endif // PRE_FIX_77172
				MAMovementBase *pMovement = GetMovement();
				if( pMovement ) {
					pMovement->ResetMove();
					pMovement->ResetLook();
				}
			}
			else 
			{
				if( m_nSuperAmmorTime > 0 ) 
				{
					SetPlaySpeed( (DWORD)( m_nSuperAmmorTime * ( m_fStiffDelta / s_fMaxStiffTime ) ), 0.03f );
					if( m_HitParam.hWeapon && m_HitParam.hWeapon->GetEquipType() != CDnWeapon::Arrow )
					{
						if( hHitter )
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
				}
			}

#if defined(PRE_FIX_44884)
			//RequestHPMPDelta로 처리될때 먼저 OnDie가 호출되어 상태효과가 제거 되어 버린다.
			//상태효과의 OnDie호출을 해서 UsingSkillWhenDieBlow의 OnDie호출이 되도록 하고 나서
			if( IsDie() )
			{
				m_pStateBlow->OnDie( hHitter );				
			}

			//정말 죽었는지 확인 해서 OnDie호출 하도록 수정
			if (IsDie())
			{
				OnDie( hHitter );
			}
#else
			if( IsDie() )
			{
				OnDie( hHitter );
				m_pStateBlow->OnDie( hHitter );
			}
#endif // PRE_FIX_44884
			break;
		case CDnWeapon::Defense:
			if( m_HitParam.hWeapon && m_HitParam.hWeapon->GetEquipType() != CDnWeapon::Arrow ) 
			{
				if( hHitter )
				{
					// 프레임 변경 상태효과가 우선 처리대상이므로 상태효과 적용중이라면 SetPlaySpeed() 함수 무시.
					if( false == (0 < hHitter->GetFrameStopRefCount() || hHitter->IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 )) )
					{
						hHitter->SetPlaySpeed( 100, 0.03f );
					}
				}
			}

			CheckDamageVelocity( hHitter );
			break;
	}
	// 무기 소리, 파티클 등을 처리하기 위해 콜해준다.
	if( m_HitParam.hWeapon ) 
	{
		m_HitParam.hWeapon->OnHitSuccess( GetMySmartPtr(), m_HitParam.HitType, m_HitParam.vPosition, m_HitParam.bFirstHit );

		if( HitParam.hWeapon->GetWeaponType() & CDnWeapon::Projectile ) {
			CDnProjectile *pProjectile = dynamic_cast<CDnProjectile *>(HitParam.hWeapon.GetPointer());
			if( pProjectile ) pProjectile->OnDamageSuccess( GetMySmartPtr(), m_HitParam );
			else HitParam.hWeapon->SetDestroy();
		}
	}

	// 스킬 시전중 피격시 스킬 취소. 
	// 서버에서 스킬 시작 직전에 피격된 것이 클라에서 스킬 실행중에 온다면 서버에선 스킬 잘 실행되고 클라에서만 스킬 액션이 
	// 캔슬되므로 서버에서 피격되었을 때 스킬 사용중이었는지 패킷으로 받아둬서 체크해서 처리.
	if( false == HitParam.szActionName.empty() )
	{
		if( IsProcessSkill() )
		{
			// 오라 스킬 시전 중.. 그러니까 상태효과 다 들어가기 이전에 타격되었다면 오라 스킬이 온전히 사용된 것이 아님
			if( IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn() )
			{
				OnSkillAura( m_hAuraSkill, false );
				m_hProcessSkill.Identity();
			}
			else
			{
				m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
				m_hProcessSkill.Identity();
			}
		}
	}

	if( hHitter )
	{
		if( hHitter->IsMonsterActor() )
		{
			DnActorHandle hSummonMasterPlayerActor = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
			if( hSummonMasterPlayerActor )
				hHitter = hSummonMasterPlayerActor;

		}
		hHitter->OnHitSuccess( CDnActionBase::m_LocalTime, GetMySmartPtr() );
	}

#ifndef _FINAL_BUILD
	if( CGlobalValue::IsActive() && CGlobalValue::GetInstance().m_bShowDamage == true )
	{
		TCHAR *szTemp[ CDnWeapon::HitTypeEnum::Count ] = { L"Normal", L"Critical", L"Stun", L"Defense", L"CriticalRes" };
		std::wstring wzDebugMessage = FormatW( L"%s - Damage(%I64d), Stiff(%.2f), Down(%.2f), %s\n", GetName(), HitParam.nDamage, m_fStiffDelta, m_fDownDelta, szTemp[HitParam.HitType] );
		GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"Debug", wzDebugMessage.c_str(), true, false, true, D3DCOLOR_XRGB(255,255,255), D3DCOLOR_ARGB(128, 0, 0, 255) );
	}
#endif
}

bool CDnActor::Compare_PreStateOrder( CDnActor::PreStateStruct &a, CDnActor::PreStateStruct &b )
{
	return ( a.nOffset < b.nOffset ) ? true : false;
}


bool CDnActor::LoadAction( const char *szFullPathName )
{
	bool bResult = CDnActionSignalImp::LoadAction( szFullPathName );
	if( !bResult ) return false;

	PreCalcStateList();

	return bResult;
}

void CDnActor::PreCalcStateList()
{
	int nValue = 0;
	std::vector<PreStateStruct> VecList;
	CEtActionSignal *pSignal;
	for( DWORD i=0; i<m_pVecActionElementList->size(); i++ ) {
		// STE_State
		VecList.clear();
		for( int j=0; ; j++ ) {
			nValue = 0;
			pSignal = GetSignal( i, STE_State, j );
			if( pSignal ) {
				StateStruct *pStruct = (StateStruct *)pSignal->GetData();
				nValue = CDnActorState::s_nActorStateIndex[pStruct->nStateOne] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateTwo] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateThree];
			}
			else break;

			VecList.push_back( PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		if( VecList.empty() ) VecList.push_back( PreStateStruct( nValue, 0 ) );
		else std::sort( VecList.begin(), VecList.end(), Compare_PreStateOrder );

		m_VecPreActionState.push_back( VecList );

		// STE_CustomState
		VecList.clear();
		for( int j=0; ; j++ ) {
			nValue = 0;
			pSignal = GetSignal( i, STE_CustomState, j );
			if( pSignal ) {
				CustomStateStruct *pStruct = (CustomStateStruct *)pSignal->GetData();
				nValue = CDnActorState::s_nActorStateIndex[pStruct->nStateOne] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateTwo] | 
					CDnActorState::s_nActorStateIndex[pStruct->nStateThree];
			}
			else break;

			VecList.push_back( PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		if( VecList.empty() ) VecList.push_back( PreStateStruct( nValue, 0 ) );
		else std::sort( VecList.begin(), VecList.end(), Compare_PreStateOrder );

		m_VecPreActionCustomState.push_back( VecList );

		// STE_CanMove
		VecList.clear();
		for( int j=0; ; j++ ) {
			nValue = 0;
			pSignal = GetSignal( i, STE_CanMove, j );
			if( pSignal ) {
				CanMoveStruct *pStruct = (CanMoveStruct*)pSignal->GetData();
				nValue = pStruct->bCanMove;
			}
			else break;

			VecList.push_back( PreStateStruct( nValue, pSignal->GetStartFrame() ) );
		}
		if( VecList.empty() ) VecList.push_back( PreStateStruct( nValue, 0 ) );
		else std::sort( VecList.begin(), VecList.end(), Compare_PreStateOrder );

		m_VecPreCanMoveState.push_back( VecList );
	}
}

void CDnActor::FreeAction()
{
	CDnActionSignalImp::FreeAction();
	SAFE_DELETE_VEC( m_VecPreActionState );
	SAFE_DELETE_VEC( m_VecPreActionCustomState );
	SAFE_DELETE_VEC( m_VecPreCanMoveState );
}

void CDnActor::LinkWeapon( int nEquipIndex )
{
	switch( m_hWeapon[nEquipIndex]->GetEquipType() ) {
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
		case CDnWeapon::Scimiter:
		case CDnWeapon::Dagger:
		case CDnWeapon::Crook:
		case CDnWeapon::Spear:
		//case CDnWeapon::KnuckleGear:
		//case CDnWeapon::LE_UNK:
		//case CDnWeapon::MC_UNK:
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			break;
		case CDnWeapon::Gauntlet:
		case CDnWeapon::Glove:
		case CDnWeapon::Charm:
		case CDnWeapon::Bracelet:
		case CDnWeapon::Claw:
		case CDnWeapon::KnuckleGear:
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex, "" );
			break;
		case CDnWeapon::SmallBow:
		case CDnWeapon::BigBow:
		case CDnWeapon::CrossBow:
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), nEquipIndex );
			if( nEquipIndex == 0 && m_hWeapon[0] && m_hWeapon[1] ) {
				m_hWeapon[1]->LinkWeapon( GetMySmartPtr(), m_hWeapon[0] );
			}
			break;
		case CDnWeapon::Arrow:
			if( !m_hWeapon[0] ) break;
			m_hWeapon[nEquipIndex]->LinkWeapon( GetMySmartPtr(), m_hWeapon[0] );
			break;
	}
}

void CDnActor::AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex, bool bDelete )
{
	if (nEquipIndex < 0 || nEquipIndex >= CDnWeapon::EquipSlot_Amount)
		return;

#if defined(PRE_ADD_50907)
	if (GetDisarmamentRefCount() > 0 )
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

	if( m_hWeapon[nEquipIndex] != hWeapon ) {
		if( m_hWeapon[nEquipIndex] ) {
			DetachWeapon( nEquipIndex );
		}
		m_hWeapon[nEquipIndex] = hWeapon;
		m_bSelfDeleteWeapon[nEquipIndex] = bDelete;
		if( m_hWeapon[nEquipIndex] ) m_hWeapon[nEquipIndex]->CreateObject( this );
	}

	if( !m_hWeapon[nEquipIndex] ) return;
	if( m_hWeapon[nEquipIndex]->IsExistAction( "Idle" ) ) m_hWeapon[nEquipIndex]->SetAction( "Idle", 0.f, 0.f );
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
	if (GetDisarmamentRefCount() > 0)
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

void CDnActor::ReleaseWeaponSignalImp()
{
	for( DWORD i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		if( m_hWeapon[i] ) {
			m_hWeapon[i]->ReleaseSignalImp();
			m_hWeapon[i]->ResetDefaultAction( i );
		}
	}
}

bool CDnActor::IsStay( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stay ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Stay ) ? true : false;
}

bool CDnActor::IsMove( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Move ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Move ) ? true : false;
}

bool CDnActor::IsAttack( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Attack ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Attack ) ? true : false;
}

bool CDnActor::IsHit( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Hit ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Hit ) ? true : false;
}

bool CDnActor::IsAir( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Air ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Air ) ? true : false;
}

bool CDnActor::IsDown( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Down ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Down ) ? true : false;
}

bool CDnActor::IsStun( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stun ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Stun ) ? true : false;
}

bool CDnActor::IsStiff( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionState[nIndex][0].nState & ActorStateEnum::Stiff ) ? true : false;
	return ( CDnActorState::GetState() & ActorStateEnum::Stiff ) ? true : false;
}

bool CDnActor::IsFly( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_Fly ) ? true : false;
	return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_Fly ) ? true : false;
}

bool CDnActor::IsGround( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_Ground ) ? true : false;
	return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_Ground ) ? true : false;
}


bool CDnActor::IsUnderGround( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_UnderGround ) ? true : false;
	return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_UnderGround ) ? true : false;
}

bool CDnActor::IsLifeSkill( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return ( m_VecPreActionCustomState[nIndex][0].nState & ActorCustomStateEnum::Custom_LifeSkill ) ? true : false;
	return ( CDnActorState::GetCustomState() & ActorCustomStateEnum::Custom_LifeSkill ) ? true : false;
}

bool CDnActor::IsStandHit( const char *szActionName )
{
	int nIndex;
	int nState = 0;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) nState = m_VecPreActionState[nIndex][0].nState;
	else nState = CDnActorState::GetState();

	if( nState == ActorStateEnum::Hit || nState == ( ActorStateEnum::Hit | ActorStateEnum::Stiff ) ) return true;
	return false;
}

int CDnActor::GetState( const char *szActionName )
{
	int nIndex;
	if( szActionName && ( nIndex = GetElementIndex( szActionName ) ) != -1 ) return m_VecPreActionState[nIndex][0].nState;
	return CDnActorState::GetState();
}

// SetPlaySpeed 와는 별개로 프레임 변경, 프레임 정지 상태효과가 Begin/End 될 때 마다 업데이트 되는 함수이다.
// 적용된 프레임 변경 상태효과의 값들을 모아 한꺼번에 값을 더해서 셋팅해준다.
// FrameStop 상태효과가 있다면 아예 0 프레임으로 처리.
// 만약 상태효과가 비어있다면 60 프레임으로 복구시켜준다.
// SetPlaySpeed 와는 별개로 돌아가지만 ProcessPlaySpeed() 에서 종료된 후에 이 함수를 한번 더 호출해서 상태효과들로
// 영향을 받는 최종 프레임을 다시 업데이트 해주어야 한다. 따라서 상태효과쪽에서는 SetPlaySpeed 함수를 사용해서는 안된다.
void CDnActor::UpdateFPS( void )
{
	if( m_bModifyPlaySpeed )		// SetPlaySpeed 함수가 호출된 상황이라면 해당 값이 우선이 된다.
	{
		CDnActionBase::SetFPS( 60.f * m_fPlaySpeed );
		SetWeaponFPS( 60.f * m_fPlaySpeed );
	}
	else
	{
		ResetPlaySpeed();

		// 프레임 정지 상태효과. 하나라도 걸려있다면 모든 프레임 정지.
		if( 0 < GetFrameStopRefCount() )
		{
			CDnActionBase::SetFPS( 0.0f );
			SetWeaponFPS( 0.0f );
		}
		else
		if( IsAppliedThisStateBlow( STATE_BLOW::BLOW_025 ) ||
			IsAppliedThisStateBlow( STATE_BLOW::BLOW_144 ) || 
			IsAppliedThisStateBlow( STATE_BLOW::BLOW_220)
			)
		{
			// 프레임 변경 상태효과들의 값을 모아서 적용.
			float fFrameRatio = 1.0f;
#ifdef PRE_ADD_SLOW_DEBUFF_LIMIT
			float fBuffFrameRatio = 0.0f;
#endif // PRE_ADD_SLOW_DEBUFF_LIMIT
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
			CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
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

			if( CDnActionBase::GetFPS() != (60.0f * fFrameRatio) )
				SyncBindActionObjectFPS( 60.0f * fFrameRatio );

			CDnActionBase::SetFPS( 60.0f * fFrameRatio );
			SetWeaponFPS( 60.0f * fFrameRatio );
		}
		else
		{
			// 원상복구
			CDnActionBase::SetFPS( 60.0f );
			SetWeaponFPS( 60.0f );
		}
	}
}

void CDnActor::AddedFrameStop( void ) 
{ 
	++m_iFrameStopRefCount;

	UpdateFPS(); 
};

void CDnActor::RemovedFrameStop( void ) 
{
	m_iFrameStopRefCount--; 
	if( m_iFrameStopRefCount < 0 ) 
		m_iFrameStopRefCount = 0;

	UpdateFPS(); 
};


void CDnActor::SetPlaySpeed( DWORD dwFrame, float fSpeed )
{
	if( m_bModifyPlaySpeed == true ) return;
	m_bModifyPlaySpeed = true;
	m_PlaySpeedStartTime = CDnActionBase::m_LocalTime;
	m_dwPlaySpeedTime = dwFrame;

	m_fPlaySpeed = fSpeed;
	UpdateFPS();
	//CDnActionBase::SetFPS( 60.f * fSpeed );
	//SetWeaponFPS( 60.f * fSpeed );
}


//void CDnActor::ModifyPlaySpeedDuration( DWORD dwNewTimeGap )
//{
//	if( m_bModifyPlaySpeed )
//		m_dwPlaySpeedTime = dwNewTimeGap;
//}


void CDnActor::ProcessPlaySpeed( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_bModifyPlaySpeed == false ) return;
	if( m_PlaySpeedStartTime == 0 ) m_PlaySpeedStartTime = LocalTime;
	if( LocalTime - m_PlaySpeedStartTime > m_dwPlaySpeedTime ) {
		//float fPrevFPS = CDnActionBase::GetFPS();
		//CDnActionBase::SetFPS( 60.f );
		//SetWeaponFPS( 60.f );

		//m_bModifyPlaySpeed = false;		// ResetPlaySpeed 함수 내부에서 호출됨.
		//OnRestorePlaySpeed( fPrevFPS );
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

	float fPrevFPS = CDnActionBase::GetFPS();
	CDnActionBase::SetFPS( 60.f );
	SetWeaponFPS( 60.f );
	m_bModifyPlaySpeed = false;
	OnRestorePlaySpeed( fPrevFPS );	
}

void CDnActor::SetWeaponFPS( float fValue )
{
	for( DWORD i=0; i<CDnWeapon::EquipSlot_Amount; i++ ) {
		if( m_hWeapon[i] ) m_hWeapon[i]->CDnActionBase::SetFPS( fValue );
	}
}


void CDnActor::SyncClassTime( LOCAL_TIME LocalTime )
{
	CDnActionBase::m_LocalTime = LocalTime;
	MASkillUser::m_LocalTime = LocalTime;
	MAActorRenderBase::m_LocalTime = LocalTime;
	MAActorRenderBase::MAFaceAniBase::m_LocalTime = LocalTime;
}


void CDnActor::OnClash( EtVector3 &vPosition, OnClashFloorCheckType bFloorClash )
{
	if( IsDie() ) return;
	if( IsHit() && ( IsAir() || IsDown() ) && !IsStun() ) {
		float fDecrease = 0.85f;

		if( GetVelocity()->x != 0.f ) {
			SetVelocityX( -GetVelocity()->x * fDecrease );
			SetResistanceX( -GetResistance()->x );
		}

		if( GetVelocityValue()->z < -120.f ) {
			float fVelocityValue = GetVelocityValue()->z; // 일정 이상의 충격일 경우에는 삭감해준다. 지금상태에선 300 이 최적..
			if( fVelocityValue < -300.f ) fVelocityValue = -300.f;
			float fYVel = 5.f + ( -fVelocityValue * 0.01f );

			SetVelocityZ( -GetVelocity()->z * ( 1.35f - ( 1.f / 4.f * ( -fVelocityValue * 0.01f  ) ) ) );
			SetResistanceZ( -GetResistance()->z );

			SetVelocityY( fYVel );
			SetResistanceY( -18.f );

			EtVector3 vLook;
			vLook = -m_matexWorld.m_vZAxis;
			vLook.y = 0.f;
			EtVec3Normalize( &vLook, &vLook );
			Look( EtVector2( vLook.x, vLook.z ) );
			if( IsDie() ) SetActionQueue( "Die_Air", 0, 0.f );
			else SetActionQueue( "Hit_AirUp", 0, 0.f );
			if( m_HitParam.hWeapon ) {
				m_HitParam.hWeapon->OnHitSuccess( GetMySmartPtr(), CDnWeapon::Stun, vPosition, true );
			}
			OutputDebug( "OnClash : 튕겼다!! %.2f\n", fVelocityValue );
		}
		else {
			if( GetVelocity()->z != 0.f ) {
				SetVelocityZ( -GetVelocity()->z * fDecrease );
				SetResistanceZ( -GetResistance()->z );
			}
			OutputDebug( "OnClash : 안튕겼다!! %.2f\n", GetVelocity()->z );
		}

	}
}

bool CDnActor::IsHittable( DnActorHandle hHitter, LOCAL_TIME LocalTime, HitStruct *pHitSignal )
{
	bool isSelfCheck = false;
	isSelfCheck = pHitSignal ? (pHitSignal->isSelfCheck?true:false) : false;

	//자신 체크가 설정되어 있지 않을때만(false가 기본. 특정(왁스 스킬...)에서만 true로 설정.
	//true로 설정되어 있으면 자신도 Hit처리..
	if (false == isSelfCheck)
		if( GetMySmartPtr() == hHitter->GetMySmartPtr() ) return false;

	switch( hHitter->GetDamageCheckType() ) {
		case DamageCheckTypeEnum::DifferentTeam:
			if( GetTeam() == hHitter->GetTeam() ) return false;
			break;
		case DamageCheckTypeEnum::Anyone_AllDamage:
			break;
		case DamageCheckTypeEnum::Anyone_DifferentTeam:
			break;
	}
	
	if( !CDnActorState::IsHittable() ) return false;
	if( IsDie() ) return false;
	if( GetDamageRemainTime() > LocalTime ) return false;

	if( pHitSignal ) {
		switch( pHitSignal->nStateCondition ) {
			case 0:	// Normal
				if( GetState() == CDnActorState::Down ) return false;
				break;
			case 1:	// Down
				if( !( GetState() & CDnActorState::Down ) ) return false;
				break;
			case 2:	// Normal + Down
				break;
		}
	}
	return true;
}

void CDnActor::Show( bool bShow )
{
	m_bShow = bShow;
	MAActorRenderBase *pRender = dynamic_cast<MAActorRenderBase *>(this);
	if( pRender ) pRender->ShowRenderBase( bShow );
}

void CDnActor::ResetActor()
{
	Show( true );
	m_fDownDelta = 0.f;
	m_fDieDelta = 0.f;
	m_fStiffDelta = 0.f;
	m_LastDamageTime = 0;
	m_LastHitSignalTime = 0;
	m_nLastHitSignalIndex = -1;

	m_bModifyPlaySpeed = false;
	m_PlaySpeedStartTime = 0;
	m_dwPlaySpeedTime = 0;

	m_nSuperAmmorTime = 0;
	CDnActionBase::ResetActionBase();
	MAActorRenderBase::ResetActorRenderBase();

	ResetMove();
	ResetLook();

	SetVelocity( EtVector3( 0.f, 0.f, 0.f ) );
	SetResistance( EtVector3( 0.f, 0.f, 0.f ) );
	SetAddHeight( 0.f );

	SetMagnetLength( 0.f );
	SetMagnetDir( EtVector2( 0.f, 0.f ) );
	if( IsCustomAction() ) {
		m_VecMixedAni.clear();
		ResetCustomAction();
	}
	ReleaseSignalImp( CDnActionSignalImp::All ^ CDnActionSignalImp::EtcObject );
	if( IsProcessSkill() ) CancelUsingSkill();
	SetActionQueue( "Stand", 0, 0.f, 0.f, false, false );

#if !defined( PRE_ADD_ACTION_OBJECT_END_ACTION )
	m_mapBindingActionObjects.clear();
#endif

	m_vecBindActionObjectHandle.clear();
}

void CDnActor::ProcessCollision( EtVector3 &vMove )
{
	PushAndCollisionCheck( vMove );
}

void CDnActor::SetAddHeight( float fValue )
{
	m_fAddHeight = fValue;
}

void CDnActor::ProcessState( LOCAL_TIME LocalTime, float fDelta )
{
	CDnActorState::ProcessState( LocalTime, fDelta );

	// #48950 슈머아머가 깨진 경우엔 MoveY 시그널 값 리셋해준다.
	if( IsHit() )
	{
		ResetMoveYDistance();
	}

	m_pStateBlow->Process( LocalTime, fDelta );
}

int CDnActor::AddStateBlow( const CDnSkill::SkillInfo* pParentSkill, STATE_BLOW::emBLOW_INDEX emBlowIndex, int nDurationTime, const char *szValue, bool bOnPlayerInit/* = false*/ )
{
	// 부활 제외하고 죽었을 땐 상태효과 추가 안됨
	if( false == bOnPlayerInit )
		if( IsDie() && STATE_BLOW::emBLOW_INDEX::BLOW_057 != emBlowIndex ) 
			return -1;

	if( !m_pStateBlow ) 
		return -1;
	
	int iAddSEResult = m_pStateBlow->CanAddThisBlow( pParentSkill, emBlowIndex );
	switch( iAddSEResult )
	{
		case CDnStateBlow::ADD_FAIL_BY_IMMUNE:		// 면역 판단은 겜서버에서만 함.
		case CDnStateBlow::ADD_FAIL_BY_DIED:
			return -1;
			break;
	}

	if( pParentSkill && CDnSkillTask::IsActive() && GetSkillTask().IsGuildWarSkill( pParentSkill->iSkillID ) )	// 길드전 스킬일 경우 중복되는 StateBlow에 대해 검사 후 제거 해 준다.
	{
		DNVector(DnBlowHandle) vlhStateBlows;
		m_pStateBlow->GetStateBlowFromBlowIndex( emBlowIndex, vlhStateBlows );
		for( UINT i=0; i<vlhStateBlows.size(); i++ )
		{
			m_pStateBlow->RemoveStateBlowFromID( vlhStateBlows.at( i )->GetBlowID() );
		}
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

	DnBlowHandle hBlow = m_pStateBlow->CreateStateBlow( GetMySmartPtr(), pParentSkill, emBlowIndex, nDurationTime, szValue );
	if( !hBlow ) 
		return -1;

	if( hBlow->IsDuplicated() )
	{
		return hBlow->GetBlowID();
	}
	else
	{
		hBlow->SetParentSkillInfo( pParentSkill );
		return m_pStateBlow->AddStateBlow( hBlow );
	}
}

int CDnActor::RemoveStateBlowByBlowDefineIndex( STATE_BLOW::emBLOW_INDEX emBlowIndex )
{
	return m_pStateBlow->RemoveStateBlowByBlowDefineIndex( emBlowIndex );
}


DnBlowHandle CDnActor::GetStateBlowFromID( int nStateBlowID )
{
	return m_pStateBlow->GetStateBlowFromID( nStateBlowID );
}


void CDnActor::RemoveStateBlowFromID( int nStateBlowID )
{
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

void CDnActor::ResetProcessSkill()
{
	if( IsProcessSkill() )
	{
		if( m_hProcessSkill )
		{
			m_hProcessSkill->OnEnd( CDnActionBase::m_LocalTime, 0.f );
			m_hProcessSkill.Identity();
		}
	}
}

int CDnActor::GetNumAppliedStateBlow( void )
{ 
	return m_pStateBlow->GetNumStateBlow();
}



DnBlowHandle CDnActor::GetAppliedStateBlow( int iIndex )
{ 
	return m_pStateBlow->GetStateBlow( iIndex ); 
}

void CDnActor::GetAllAppliedStateBlow( DNVector(DnBlowHandle)& /*IN OUT*/ out )
{
	int nSize = m_pStateBlow->GetNumStateBlow();
	if( 0 < nSize ) out.reserve( out.capacity() + nSize );
	for ( int i = 0 ; i < nSize ; i++ )
	{
		DnBlowHandle hBlow = m_pStateBlow->GetStateBlow(i);
		if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
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
		if( hBlow->GetBlowIndex() == BlowIndex &&
			STATE_BLOW::STATE_END != hBlow->GetBlowState() )
			out.push_back( hBlow );
	}
}



void CDnActor::OnDispatchMessage( DWORD dwActorProtocol, BYTE *pPacket )
{
	switch( dwActorProtocol ) {
		case eActor::SC_PARTSDAMAGE:
		{
			int iPartsTableID, iHP, iHitterID;

			CPacketCompressStream Stream( pPacket, 128 );
			Stream.Read( &iPartsTableID, sizeof(int) );
			Stream.Read( &iHP, sizeof(int) );
			Stream.Read( &iHitterID , sizeof(int) );
			OnSetPartsHP( GetActorHandle(), iPartsTableID, iHP ,iHitterID );

			break;
		}
		case eActor::SC_ONDAMAGE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSeed;
				char cHitterType;
				DWORD dwUniqueID, dwWeaponUniqueID = -1;
				int nActionIndex;
				bool bSendWeaponSerialID;
				INT64 nWeaponSerialID = -1;
				SHitParam HitParam;
				EtVector3 vPos;
				EtVector2 vVeiwVec;

				Stream.Read( &nSeed, sizeof(int) );
				Stream.Read( &cHitterType, sizeof(char) );
				Stream.Read( &dwUniqueID, sizeof(DWORD) );
				Stream.Read( &bSendWeaponSerialID, sizeof(bool) );
				if( bSendWeaponSerialID ) Stream.Read( &nWeaponSerialID, sizeof(INT64) );
				else Stream.Read( &dwWeaponUniqueID, sizeof(DWORD) );
				Stream.Read( &nActionIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &HitParam.nDamage, sizeof(INT64) );
				Stream.Read( &HitParam.nCurrentHP, sizeof(INT64) );
				Stream.Read( &HitParam.HitType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &HitParam.vVelocity, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &HitParam.vResistance, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &HitParam.vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vVeiwVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
				HitParam.vViewVec = EtVec2toVec3( vVeiwVec );
				Stream.Read( &HitParam.bFirstHit, sizeof(bool) );
				//Stream.Read( &HitParam.nBoneIndex, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &m_fStiffDelta, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 10000.f );
				Stream.Read( &m_fDownDelta, sizeof(float), CPacketCompressStream::FLOAT_SHORT, 1000.f );
				Stream.Read( &m_nSuperAmmorTime, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				Stream.Read( &HitParam.bSuccessNormalDamage, sizeof(bool) );
				Stream.Read( &HitParam.DistanceType, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &HitParam.iCanceledSkillIDInServer, sizeof(int), CPacketCompressStream::INTEGER_SHORT ); 	// #32722 클라이언트에서 스킬 사용한다는 패킷이 도착하기 전에 서버에서 피격될 경우 클라이언트만 스킬 액션이 캔슬되는 것 수정.

				// 플레이어의 슈퍼아머가 33000 까지 되는 경우가 있어 압축하지 않고 서버에서 보냅니다. (#10065)
				Stream.Read( &m_nCurrSuperArmor, sizeof(int) );
				Stream.Read( &m_nMaxSuperArmor, sizeof(int) );
				//Stream.Read( &m_nCurrSuperArmor, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
				//Stream.Read( &m_nMaxSuperArmor, sizeof(int), CPacketCompressStream::INTEGER_SHORT );

				Stream.Read( &HitParam.HasElement, sizeof(int), CPacketCompressStream::INTEGER_CHAR );

#if defined( PRE_ADD_LOTUSGOLEM )
				Stream.Read( &HitParam.bIgnoreShowDamage, sizeof(bool) );
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
				Stream.Read( &HitParam.bStageLimit, sizeof(bool) );
#endif

				if( nActionIndex != -1 && GetElement( nActionIndex ) )
					HitParam.szActionName = GetElement( nActionIndex )->szName;

				if( IsPlayerActor() )
				{
					// #32722 클라이언트에서 스킬 사용한다는 패킷이 도착하기 전에 서버에서 피격될 경우 클라이언트만 스킬 액션이 캔슬되는 것 수정.
					// 서버에서까지 실제로 스킬이 사용되어진 상태에서 피격된 경우에만 클라에서도 맞춰서 스킬 액션을 캔슬시켜준다.
					if( nActionIndex != -1 &&
						0 == HitParam.iCanceledSkillIDInServer )			// 서버에서 피격시 스킬 사용중이 아니었고.
					{
						//#41353 오토패시브 스킬일 경우 서버에서 UseSkill -> OnDamage 패킷 순으로 보냄.
						//여기에 HitParma.szActionName에는 오토패시브 스킬용 액션이 저장되어 있어서 오토패시브 스킬인 경우 클리어 하면 스킬동작이 안 나옴.
						if( m_hProcessSkill && m_hProcessSkill->GetSkillType() != CDnSkill::SkillTypeEnum::AutoPassive)		// 클라이언트에서 스킬이 사용중이었다면.
						{
							// 클라이언트에서 피격액션을 실행하지 않는다. 스킬이 캔슬되지 않도록..
							HitParam.szActionName.clear();
						}
					}
				}

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

				CDnDamageBase *pHitter = NULL;
				switch( cHitterType ) {
					case DamageObjectTypeEnum::Actor:
						{
							int nComboCount;
							int nComboDelay;
							if( dwUniqueID >= 100000 ) 
							{
								Stream.Read( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
								Stream.Read( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
							}
							DnActorHandle hHitter = FindActorFromUniqueID( dwUniqueID );
							pHitter = hHitter;

#ifdef PRE_ADD_LOTUSGOLEM
							if (HitParam.bIgnoreShowDamage)
								break;
#endif

							if( !HitParam.hWeapon && hHitter ) {
								HitParam.hWeapon = ( hHitter->GetWeapon(1) ) ? hHitter->GetWeapon(1) : hHitter->GetWeapon(0);
							}

							if( hHitter && dwUniqueID >= 100000 ) {
								hHitter->OnComboCount( nComboCount, nComboDelay );
							}

							// 렐릭(소환몹)이 hit 했을 경우 소환한 플레이어에게 콤보 업데이트 해준다. (#11415)
							if( hHitter && dwUniqueID < 100000 && hHitter->IsMonsterActor() ) 
							{
								// 렐릭에서 소환된 몬스터라면, 소환한 원래 주인의 콤보를 올려준다.
								DnActorHandle hSummonMasterPlayerActor = static_cast<CDnMonsterActor*>(hHitter.GetPointer())->GetSummonerPlayerActor();
								if( hSummonMasterPlayerActor && hSummonMasterPlayerActor->IsPlayerActor() )
								{
									Stream.Read( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
									Stream.Read( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
									hSummonMasterPlayerActor->OnComboCount( nComboCount, nComboDelay );
								}
							}

							// 대포가 때린 거라면, 그리고 대포를 잡은 플레이어가 로컬 플레이어인 경우 콤포 카운트 업데이트
							if( hHitter && CDnActorState::Cannon == hHitter->GetActorType() )
							{
								Stream.Read( &nComboCount, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
								Stream.Read( &nComboDelay, sizeof(int), CPacketCompressStream::INTEGER_SHORT );
								hHitter->OnComboCount( nComboCount, nComboDelay );
							}
						}
						break;
					case DamageObjectTypeEnum::Prop:
						{
							DnPropHandle hHitter = CDnWorldProp::FindPropFromUniqueID( dwUniqueID );
							pHitter = dynamic_cast<CDnDamageBase*>(hHitter.GetPointer());
						}
						break;
				}

				OnDamageReadAdditionalPacket( &Stream );
				
				//#65346
				//히트가 Self체크 설정 된 발사체의 지속적인 히트가 될때 위치값 오차로 랙처럼 보이는 현상이 발생됨.
				//자신의 히트일 경우는 위치값 설정 하지 않도록
				if( dwUniqueID != m_dwUniqueID )
				{
					bool bSafetyHit = false;
					if( cHitterType == DamageObjectTypeEnum::Actor && HitParam.nDamage == 0 && nActionIndex == -1 )
					{
						DnActorHandle hHitter = FindActorFromUniqueID( dwUniqueID );
						if( hHitter && hHitter->GetTeam() == GetTeam() )
							bSafetyHit = true;
					}

					if( !bSafetyHit )
						SetPosition( vPos );
				}

				OutputDebug( "OnDamage : %s, %.2f, %.2f\n", HitParam.szActionName.c_str(), HitParam.vVelocity.y, HitParam.vResistance.y );
				_srand( nSeed );
				
				m_pStateBlow->OnCalcDamage( 0.0f, HitParam );

				OnDamage( pHitter, HitParam );

			}
			break;
		case eActor::SC_CMDADDSTATEEFFECT:
			{
				CPacketCompressStream PacketStream( pPacket, 128 );

				int nDurationTime;
				CDnSkill::SkillInfo ParentSkillInfo;
				DWORD dwHitterUniqueID;
				std::string szParam;
				STATE_BLOW::emBLOW_INDEX emState;
				BYTE cItemSkill,cOnInit;
				int nServerBlowID = 0;
				std::string szEffectOutputIDs;
				CDnSkill::TargetTypeEnum eTargetType = CDnSkill::TargetTypeEnum::Self;
				bool bEternity = false;

				PacketStream.Read( &dwHitterUniqueID, sizeof(DWORD) );
				PacketStream.Read( &cItemSkill, sizeof(BYTE) );
				PacketStream.Read( &ParentSkillInfo.iSkillID, sizeof(int) );
				PacketStream.Read( &ParentSkillInfo.iSkillLevelID, sizeof(int) );
				PacketStream.Read( &emState, sizeof(int) );
				PacketStream.Read( &nDurationTime, sizeof(int) );

				//LOCAL_TIME ServerStartTime = 0;
				//Stream.Read( &ServerStartTime, sizeof(__int64) );
				ReadStdString( szParam, &PacketStream );

				PacketStream.Read( &cOnInit, sizeof(BYTE) );
				PacketStream.Read( &nServerBlowID, sizeof(int) );
				ReadStdString( szEffectOutputIDs, &PacketStream);
				
				PacketStream.Read( &ParentSkillInfo.bFromInputHasPassiveSignal, sizeof(bool) );
				PacketStream.Read( &ParentSkillInfo.bFromBuffProp, sizeof(bool) );
				PacketStream.Read( &ParentSkillInfo.eTargetType, sizeof(CDnSkill::TargetTypeEnum) );

				// Blow 강제 Begin 여부 [2011/03/18 semozz]
				PacketStream.Read( &ParentSkillInfo.bItemPrefixSkill, sizeof(ParentSkillInfo.bItemPrefixSkill) );

				// 길드전 스킬(성문 관련) 면역 처리 무시 하기 위한 코드 추가 [2011/04/14 semozz]
				PacketStream.Read( &ParentSkillInfo.bIgnoreImmune, sizeof(ParentSkillInfo.bIgnoreImmune));

#if defined(PRE_FIX_NEXTSKILLINFO)
				PacketStream.Read(&ParentSkillInfo.nSkillLevel, sizeof(ParentSkillInfo.nSkillLevel));
#endif // PRE_FIX_NEXTSKILLINFO

				PacketStream.Read( &bEternity, sizeof(bool) );

#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
				PacketStream.Read(&ParentSkillInfo.iAppliedEnchantSkillID, sizeof(int));
				bool bAppliedEnchantSkill = (ParentSkillInfo.iAppliedEnchantSkillID > 0);
#endif

				DNTableFileFormat*  pSkillTable = NULL;
				DNTableFileFormat*  pSkillLevelTable = NULL;

				if( CDnSkillTask::IsActive() && GetSkillTask().IsGuildWarSkill( ParentSkillInfo.iSkillID ) )	// 길드전 스킬일 경우 길드전 테이블을 읽는다.
				{
					pSkillTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
					pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
				}
				else
				{
					pSkillTable = GetDNTable( CDnTableDB::TSKILL );
					pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );
				}

				ParentSkillInfo.hSkillUser		= FindActorFromUniqueID( dwHitterUniqueID );
				ParentSkillInfo.bIsItemSkill	= (cItemSkill == 0) ? false : true;

				if( ParentSkillInfo.iSkillID != 0 && pSkillTable != NULL && pSkillLevelTable != NULL )
				{
					ParentSkillInfo.eSkillType		= (CDnSkill::SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillID, "_SkillType" )->GetInteger();
#ifdef PRE_FIX_SYNC_ENCHANT_SKILL
					const int nSkillIdApplied = (bAppliedEnchantSkill) ? ParentSkillInfo.iAppliedEnchantSkillID : ParentSkillInfo.iSkillID;

					ParentSkillInfo.eDurationType	= (CDnSkill::DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( nSkillIdApplied, "_DurationType" )->GetInteger();
#else
					ParentSkillInfo.eDurationType	= (CDnSkill::DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillID, "_DurationType" )->GetInteger();
#endif
					ParentSkillInfo.iDissolvable	= pSkillTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillID, "_Dissolvable" )->GetInteger();

					if (szEffectOutputIDs.empty())
						ParentSkillInfo.effectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillLevelID, "_StateEffectTableID" )->GetString();
					else
						ParentSkillInfo.effectOutputIDs = szEffectOutputIDs;

#ifdef PRE_FIX_SYNC_ENCHANT_SKILL

					ParentSkillInfo.iBuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( nSkillIdApplied, "_BuffIconImageIndex" )->GetInteger();
					ParentSkillInfo.iDebuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( nSkillIdApplied, "_DeBuffIconImageIndex" )->GetInteger();
					ParentSkillInfo.debuffEffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillLevelID, "_StateEffectOtherID" )->GetString();

#else // PRE_FIX_SYNC_ENCHANT_SKILL
															ParentSkillInfo.iBuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillID, "_BuffIconImageIndex" )->GetInteger();
										#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
															ParentSkillInfo.iDebuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillID, "_DeBuffIconImageIndex" )->GetInteger();

															if (szEffectOutputIDs.empty())
																ParentSkillInfo.debuffEffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( ParentSkillInfo.iSkillLevelID, "_StateEffectOtherID" )->GetString();
										#endif
#endif // PRE_FIX_SYNC_ENCHANT_SKILL

					if( ParentSkillInfo.hSkillUser )
						ParentSkillInfo.iSkillUserTeam = ParentSkillInfo.hSkillUser->GetTeam();
					else
						ParentSkillInfo.iSkillUserTeam = m_nTeam;		// 스킬 사용자가 없으면 팀은 그냥 자기 자신의 팀으로 셋팅해줌.

					vector<int> vlSkillLevelList;
					//if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", ParentSkillInfo.iSkillID, vlSkillLevelList ) != 0 )
					GetSkillTask().GetSkillLevelList( ParentSkillInfo.iSkillID, vlSkillLevelList, CDnSkill::PVE );		// pve 기준으로 처리하면 된다.
					if( false == vlSkillLevelList.empty() )
					{
						ParentSkillInfo.iSkillLevelIDOffset = vlSkillLevelList.front();
					}
				}

#if defined(PRE_FIX_NEXTSKILLINFO)
				//몬스터 경우 SC_USESKILL패킷 받기 전에 상태효과 추가 패킷을 받을 수 있다.
				//서버에서는 몬스터 생성시 스킬 정보가 추가 되어 있지만 클라이언트는 스킬 정보가 없다.
				//이 시점에 상태효과 추가 될때 해당 스킬이 없을 수 있어서, 추가 해준다.... 몬스터 스킬은 PVE만??
				if( ParentSkillInfo.hSkillUser == GetActorHandle() && IsMonsterActor() == true)
				{
					AddSkill( ParentSkillInfo.iSkillID, ParentSkillInfo.nSkillLevel );
				}
#endif // PRE_FIX_NEXTSKILLINFO

#if defined( PRE_FIX_BREAKINTO_BLOW_SYNC )
				if( ParentSkillInfo.eDurationType == CDnSkill::DurationTypeEnum::Aura )
				{
					DnSkillHandle hSkill = FindSkill( ParentSkillInfo.iSkillID );
					if( hSkill && hSkill->IsAuraOn() == false )
					{
						hSkill->EnableAura( true );
					}
				}
				else if( ParentSkillInfo.eDurationType == CDnSkill::DurationTypeEnum::ActiveToggle 
					|| ParentSkillInfo.eDurationType == CDnSkill::DurationTypeEnum::ActiveToggleForSummon
					|| ParentSkillInfo.eDurationType == CDnSkill::DurationTypeEnum::TimeToggle )
				{
					DnSkillHandle hSkill = FindSkill( ParentSkillInfo.iSkillID );
					if( hSkill && hSkill->IsToggleOn() == false )
					{
						hSkill->EnableToggle( true );
					}
				}
#endif
	
				DnBlowHandle hAddedBlow = CmdAddStateEffect( ( ParentSkillInfo.iSkillID == 0 ) ? NULL : &ParentSkillInfo, emState, nDurationTime, szParam.c_str(), cOnInit==1 ? true : false );
				if( hAddedBlow )
				{
					hAddedBlow->SetServerBlowID( nServerBlowID );
					
					// 상태효과 별로 추가적인 패킷 내용이 더 있다면 알아서 처리하도록 넘겨준다.
					hAddedBlow->OnReceiveAddPacket( PacketStream );
					hAddedBlow->SetEternity( bEternity );

					CDnGameTask* pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
					if( pGameTask )
						pGameTask->OnCmdAddStateEffect( &ParentSkillInfo );
				}
			}
			break;
		case eActor::SC_CMDREMOVESTATEEFFECT:
			{
				CPacketCompressStream Stream( pPacket, 32 );

				STATE_BLOW::emBLOW_INDEX BlowIndex = STATE_BLOW::BLOW_NONE;
				Stream.Read( &BlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );
				
				if( m_pStateBlow->IsApplied( BlowIndex ) )
				{
					// [2010/12/17 semozz]
					// 패킷으로 받은 지울 상태 효과들은 즉시 제거 하도록한다.
					m_pStateBlow->RemoveImediatlyStateEffectByBlowIndex(BlowIndex);
				}
			}
			break;

		case eActor::SC_CMDREMOVESTATEEFFECTFROMID:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nID = 0;

				Stream.Read( &nID, sizeof(int) );

				// [2010/12/17 semozz]
				// 패킷으로 받은 지울 상태 효과들은 즉시 제거 하도록한다.
				m_pStateBlow->RemoveImediatlyStateEffectByServerID(nID);
			}
			break;

		case eActor::SC_CMDMODIFYSTATEEFFECT:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				int nServerBlowID = 0;
				float fDurationTime = 0.0f;

				Stream.Read( &nServerBlowID, sizeof(int) );
				Stream.Read( &fDurationTime, sizeof(float) );

				DnBlowHandle hBlow = m_pStateBlow->GetStateBlowFromServerID( nServerBlowID );
				if( hBlow )
					hBlow->SetDurationTime( fDurationTime );
			}
			break;
		case eActor::SC_CMDSUICIDE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				bool bDropItem, bDropExp;
				Stream.Read( &bDropItem, sizeof(bool) );
				Stream.Read( &bDropExp, sizeof(bool) );

				CmdSuicide( bDropItem, bDropExp );
			}
			break;
		case eActor::SC_SETHPMP_DELTA:
			{
				if(IsDie())
					break;

				CPacketCompressStream Stream( pPacket, 32 );

				ElementEnum eElement = ElementEnum::ElementEnum_Amount;
				INT64 nHPMPDelta = 0;
				DWORD dwUniqueID;
				bool bIsMPDelta = false;
				bool bShowValue;

				Stream.Read( &eElement, sizeof(ElementEnum), CPacketCompressStream::INTEGER_CHAR );
				Stream.Read( &nHPMPDelta, sizeof(INT64) );
				Stream.Read( &dwUniqueID, sizeof(DWORD) );
				Stream.Read( &bIsMPDelta, sizeof(bool) );
				Stream.Read( &bShowValue, sizeof(bool) );

#if defined(PRE_FIX_59308)
				int nHitType = CDnWeapon::Normal;
				Stream.Read( &nHitType, sizeof(int) );
#endif // PRE_FIX_59308
#ifdef PRE_FIX_77172
				bool bKeepJumpMovement = false;
				Stream.Read( &bKeepJumpMovement, sizeof(bool) );
#endif // PRE_FIX_77172

				if( false == bIsMPDelta )
				{
					if( nHPMPDelta < 0 )
					{
						DnActorHandle hHitter = FindActorFromUniqueID( dwUniqueID );
						CDnDamageBase::SHitParam HitParam;

						HitParam.bRecvOnDamage	= false;
#if defined(PRE_FIX_59308)
						HitParam.HitType = (CDnWeapon::HitTypeEnum)nHitType;
#endif // PRE_FIX_59308
						HitParam.HasElement = eElement;
						HitParam.nDamage = -nHPMPDelta;
						HitParam.vPosition = *GetPosition();
						HitParam.vViewVec = ( GetLookDir() ) ? *GetLookDir() : EtVector3( 0.f, 0.f, 0.f );

						// Note: 공중에 떠 있는 경우 디폴트 히트 파라메터 생성자에서 y 속도가 0.0f 로 될 수 있으므로 
						// 영향을 미치지 않도록 현재 원래 속도를 셋팅해 줍니다.
						HitParam.vVelocity = ( GetVelocity() ) ? *GetVelocity() : EtVector3( 0.0f, 0.0f, 0.0f );
						HitParam.vResistance = ( GetResistance() ) ? *GetResistance() : EtVector3( 0.0f, 0.0f, 0.0f );
#ifdef PRE_FIX_77172
						HitParam.bKeepJumpMovement = bKeepJumpMovement;
#endif // PRE_FIX_77172
						bool bCheckActorDie = false;
						INT64 iAfterDamageHP = GetHP() - HitParam.nDamage;
						if( iAfterDamageHP <= 0 )
						{
							//아래 OnDamage에서 137번 상태효과에 의해 HP가 1로 설정 되지만 마지막 코드에 의해서 다시 HP를 0으로 설정해서 몬스터가
							//죽을때 사용해야 하는 스킬을 사용 하지 못하고 바로 제거 되는 경우가 발생. 137번 상태효과가 있으면 bCheckActorDie
							//플래그를 설정 하지 않도록 해서, HP를 1을 유지 할 수 있게 한다.
							//Die액션도 137번 상태효과가 없을때만 설정 될 수 있도록 한다.
							if (IsAppliedThisStateBlow(STATE_BLOW::BLOW_137) == false)
							{
								string strDieAction;
								GetProperlyDieAction( strDieAction );
								_ASSERT( !strDieAction.empty() );
								if( !strDieAction.empty() )
									HitParam.szActionName = strDieAction;

								bCheckActorDie = true;
							}

							HitParam.nDamage = GetHP();
							OutputDebug( "Die By StateEffect : %s\n", HitParam.szActionName.c_str() );
						}

						CDnDamageBase* pHitter = NULL;
						if( hHitter )
							pHitter = dynamic_cast<CDnDamageBase*>(hHitter.GetPointer());
						
						// Note: SC_ONDAMAGE 에 현재 HP 추가되었으므로 여기서도 추가해 줌.
						HitParam.nCurrentHP = iAfterDamageHP;
						OnDamage( pHitter, HitParam );

						if( bCheckActorDie && !IsDie() )
							SetHP(0);
					}
					else
					{
						// 회복은 그냥 hp 회복시켜 줌
						SetHP( GetHP() + nHPMPDelta );					
						if( bShowValue && nHPMPDelta > 0 ) {

							CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(this);
							bool bSameTeam = (GetTeam() == CDnActor::s_hLocalActor->GetTeam());

							GetInterface().SetRecovery( GetHeadPosition(), (int)nHPMPDelta, 0, ( pMonsterActor != NULL || !bSameTeam ),		// 몬스터 이거나, 팀이 다르면 적이다.
																										(GetMySmartPtr() == CDnActor::s_hLocalActor)  );
#ifdef _RDEBUG
							g_Log.Log(LogType::_FILELOG, "[DMGERR] SC_SETHPMP_DELTA : false == bIsMPDelta / nHPMPDelta(%d)\n", nHPMPDelta);
#endif
						}				
						if( GetMaxHP() < GetHP() )
							SetHP( GetMaxHP() );
					}
				}
				else
				{
					int iNewSP = GetSP() + (int)nHPMPDelta;
					if( iNewSP > GetMaxSP() )
						iNewSP = GetMaxSP();
					else
					if( iNewSP < 0 )
						iNewSP = 0;

					if( bShowValue && nHPMPDelta > 0 ) 
					{
						GetInterface().SetRecovery( GetHeadPosition(), 0, (int)nHPMPDelta, false, (GetMySmartPtr() == CDnActor::s_hLocalActor)  );
					}

					SetSP( iNewSP );
				}
			}
			break;

		case eActor::SC_STATEEFFECT_PROB_SUCCESS:
			{
				CPacketCompressStream Stream( pPacket, 16 );
				int iSkillID = 0;
				STATE_BLOW::emBLOW_INDEX eBlowIndex = STATE_BLOW::BLOW_NONE;
				
				Stream.Read( &iSkillID, sizeof(int) );
				Stream.Read( &eBlowIndex, sizeof(STATE_BLOW::emBLOW_INDEX) );

				DNVector( DnBlowHandle ) vlhBlows;
				m_pStateBlow->GetStateBlowFromBlowIndex( eBlowIndex, vlhBlows );
				for( int i = 0; i < (int)vlhBlows.size(); ++i )
				{
					DnBlowHandle hBlow = vlhBlows.at( i );

					const CDnSkill::SkillInfo* pParentSkillInfo = hBlow->GetParentSkillInfo();
					if( pParentSkillInfo && iSkillID == pParentSkillInfo->iSkillID )
					{
						// 페이백 마나 상태효과인 경우.
						if( STATE_BLOW::BLOW_068 == eBlowIndex )
						{
							CDnPayBackMPBlow* pPayBackMPBlow = static_cast<CDnPayBackMPBlow*>(hBlow.GetPointer());

							// 클라쪽에서 몇 초간 이펙트를 보여준다.
							pPayBackMPBlow->OnActivate();
						}
					}
				}
			}
			break;

		case eActor::SC_ADD_STATE_EFFECT_DENIED:
			{
				CPacketCompressStream Stream( pPacket, 16 );

				int iAddSEResult = 0;
				Stream.Read( &iAddSEResult, sizeof(int) );
				ASSERT( (iAddSEResult != CDnStateBlow::ADD_SUCCESS) && "상태효과 추가 성공했는데 실패 패킷 날아옴" );
				switch( iAddSEResult )
				{
					case CDnStateBlow::ADD_FAIL_BY_IMMUNE:
#ifdef PRE_ADD_DECREASE_EFFECT
					case CDnStateBlow::ADD_DECREASE_EFFECT_BY_IMMUNE:
#endif // PRE_ADD_DECREASE_EFFECT
						{
							// 면역 띄움
							SAABox box;
							GetBoundingBox(box);
							float fHeight = box.Max.y - box.Min.y;// + 10.f;
#ifdef PRE_ADD_DECREASE_EFFECT
							if( fHeight == 0.0f && GetObjectHandle() )
							{
								((MAActorRenderBase*)this)->Process( m_matexWorld, 0, 0.0f );
								GetBoundingBox(box);
								fHeight = box.Max.y - box.Min.y;
							}
#endif // PRE_ADD_DECREASE_EFFECT
							EtVector3 vPos = m_matexWorld.m_vPosition;
							vPos.y += fHeight;

							if( CDnActor::s_hLocalActor ) 
							{
								bool bPartyMember = !IsMonsterActor() && (GetMySmartPtr() != CDnActor::s_hLocalActor) && (GetTeam() == CDnActor::s_hLocalActor->GetTeam());
								if(  !bPartyMember )	 
								{		
									if( CGameOption::GetInstance().bMyCombatInfo ) 
									{
#ifdef PRE_ADD_DECREASE_EFFECT
										if( iAddSEResult == CDnStateBlow::ADD_FAIL_BY_IMMUNE )
											GetInterface().SetResist( vPos );
										else if( iAddSEResult == CDnStateBlow::ADD_DECREASE_EFFECT_BY_IMMUNE )
											GetInterface().SetDecreaseEffect( vPos );
#else // PRE_ADD_DECREASE_EFFECT
										GetInterface().SetResist( vPos );
#endif // PRE_ADD_DECREASE_EFFECT
									}								
								}
							}
						}
						break;
					case CDnStateBlow::ADD_FAIL_BY_DIED:
						break;
					case CDnStateBlow::ADD_FAIL_BY_PROBABILITY:
						OutputDebug( "[Add State Effect Failed by probability fail!]\n" );
						break;
				}
			}
			break;

			/*
		case eActor::SC_POSITION:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3	vPosition;
				EtVector2	vLook;

				Stream.Read( &vPosition, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				SetPosition( vPosition );
				Look( vLook );

				break;
			}
			*/
		case eActor::SC_CMDWARP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3	vPos;
				EtVector2	vLook;

				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );

				CmdWarp( vPos, vLook );
			}
			break;
		case eActor::SC_PLAYERDIE:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				DWORD dwHitterUniqueID;
				Stream.Read( &dwHitterUniqueID, sizeof(DWORD) );

				DnActorHandle hHitter = FindActorFromUniqueID( dwHitterUniqueID );

				// ScoreSystem
				CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( pGameTask )
					pGameTask->OnDie( GetActorHandle(), hHitter );

				break;
			}
			
		case eActor::SC_INVALID_PROJECTILE:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				
				DWORD dwUniqueID = 0;
				Stream.Read( &dwUniqueID, sizeof(DWORD) );

				DnWeaponHandle hProjectile = CDnProjectile::FindWeaponFromUniqueID( dwUniqueID );
				if( hProjectile )
				{
					CDnProjectile* pProjectile = dynamic_cast<CDnProjectile*>( hProjectile.GetPointer() );
					if( pProjectile )
					{
						pProjectile->SetInvalid( CDnActionBase::m_LocalTime );
					}
				}
			}
			break;
		case eActor::SC_CMDCHANGETEAM:
			{
				CPacketCompressStream Stream( pPacket, 32 );

				int nTeam;
				Stream.Read( &nTeam, sizeof(int) );

				SetTeam( nTeam ); 
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
				GetInterface().RefreshPvpPlayerGauge();
#endif

			}
			break;
		case eActor::SC_CMDFORCEVELOCITY:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				EtVector3 vPos, vVel, vRes;
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vVel, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vRes, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				SetPosition( vPos );
				SetVelocity( vVel );
				SetResistance( vRes );
			}
			break;
		case eActor::SC_CMDCHATBALLOON:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				int nLength;
				WCHAR wszChat[64] = { 0, };

				Stream.Read( &nLength, sizeof(int) );
				Stream.Read( wszChat, nLength );

				if( __wcsicmp_l( L"HIDE_TRIGGER_CHATBALLOON", wszChat ) == 0 )
				{
					StopHeadEffect( CDnHeadEffectRender::HeadEffect_ChatBalloon );
					break;
				}

#if defined(PRE_ADD_MULTILANGUAGE)
				_CmdChatBaloon(wszChat);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
#ifdef PRE_ADD_CASH_AMULET
				if(GetChatBalloon().IsEnableCustomChatBallon())
				{
					GetChatBalloon().StopChatBalloonCustom();
					GetChatBalloon().ClearCustomChatBalloonText();
				}	

				if(CGameOption::GetInstance().m_bShowCashChatBalloon && IsPlayerActor())
				{
					CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(GetActorHandle().GetPointer());
					DnPartsHandle hCashAmulet = pPlayer->GetCashParts(CDnParts::CashNecklace);

					if(hCashAmulet)
					{
						int nItemID = hCashAmulet->GetClassID();
						DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCASHCHATBALLOONTABLE );

						if(pSox && pSox->IsExistItem(nItemID))
							GetChatBalloon().SetChatBalloonCustom( pSox->GetFieldFromLablePtr( nItemID, "_UIResourceName1" )->GetString() , wszChat , chatroomcolor::NORMAL, GetTickCount(), 1 );
						else
							SetChatBalloonText( wszChat, GetTickCount(), 1, true );

					}
					else
						SetChatBalloonText( wszChat, GetTickCount(), 1, true );
						
				}
				else
					SetChatBalloonText( wszChat, GetTickCount(), 1, true );

#else
				SetChatBalloonText( wszChat, GetTickCount(), 1, true );
#endif
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
			}
			break;
#if defined(PRE_ADD_MULTILANGUAGE)
		case eActor::SC_CMDCHATBALLOON_AS_INDEX:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				int nUIStringIndex;
				Stream.Read( &nUIStringIndex, sizeof(int) );

				if( nUIStringIndex == 0 )
				{
					StopHeadEffect( CDnHeadEffectRender::HeadEffect_ChatBalloon );
					break;
				}

				std::wstring wstrString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );
				if (wstrString.empty() == false && wstrString.size() <= 60)
					_CmdChatBaloon(wstrString.c_str());
			}
			break;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
		case eActor::SC_SHOWEXPOSUREINFO:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				bool bShow;
				Stream.Read( &bShow, sizeof(bool) );

				CmdShowExposureInfo( bShow );
			}
			break;

		case eActor::SC_CMDSHOW:
			{
				Show( true );

				if( CDnQuestTask::IsActive() && IsNpcActor() )
				{
					CDnQuestTask *pTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask("QuestTask"));
					pTask->SetRefreshNpcQuestState(true);
				}
				break;
			}
		case eActor::SC_CMDHIDE:
			{
				Show( false );
				ReleaseSignalImp();
#ifdef PRE_ADD_MAINQUEST_UI
				m_bFromMapTriggerHide = true;
#endif
				
				if( CDnQuestTask::IsActive() && IsNpcActor() )
				{
					CDnQuestTask *pTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask("QuestTask"));
					pTask->SetRefreshNpcQuestState(true);
				}
				break;
			}
		case eActor::SC_REFRESHHPSP:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				INT64 nHP;
				int nSP;
				Stream.Read( &nHP, sizeof(INT64) );
				Stream.Read( &nSP, sizeof(int) );

				m_pStateBlow->Process( 0, 0.f );

				SetHP( nHP );
				SetSP( nSP );
			}
			break;

		case eActor::SC_FINISH_PROCESSSKILL:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillID;
				Stream.Read( &nSkillID, sizeof(int) );

				if( IsProcessSkill() )
				{
					DnSkillHandle hSkill = GetProcessSkill();
					if( hSkill && hSkill->GetClassID() == nSkillID )
						ResetProcessSkill();
				}
				break;
			}

#ifdef _SHADOW_TEST
		case eActor::SC_SHADOW:
			{
				if( m_bEnableShadowActor == false ) break;

				CPacketCompressStream Stream( pPacket, 256 );
				int nActionIndex;
				float fFrame;
				EtVector3 vPos;
				EtVector3 vViewX, vViewZ;
				EtVector3 vMoveX, vMoveZ;

				Stream.Read( &nActionIndex, sizeof(int) );
				Stream.Read( &fFrame, sizeof(float) );
				Stream.Read( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Read( &vViewX, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Stream.Read( &vViewZ, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Stream.Read( &vMoveX, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );
				Stream.Read( &vMoveZ, sizeof(EtVector3), CPacketCompressStream::VECTOR3_SHORT );

				bool bMoveTarget;
				bool bLookTarget;
				Stream.Read( &bMoveTarget, sizeof(bool) );
				if( bMoveTarget ) {
					EtVector3 vPos;
					Stream.Read( &vPos, sizeof(EtVector3) );
				}
				Stream.Read( &bLookTarget, sizeof(bool) );
				if( bLookTarget ) {
					EtVector3 vPos;
					Stream.Read( &vPos, sizeof(EtVector3) );
				}
				if( m_hShadowActor ) {
					MatrixEx *pCross = m_hShadowActor->GetMatEx();
					pCross->m_vPosition = vPos;
					pCross->m_vXAxis = vViewX;
					pCross->m_vZAxis = vViewZ;
					pCross->MakeUpCartesianByXAxis();
					m_hShadowActor->SetMoveVectorX( vMoveX );
					m_hShadowActor->SetMoveVectorZ( vMoveZ );

					if( nActionIndex != -1 && m_hShadowActor->GetElement( nActionIndex ) ) {
						char *szActionName = (char *)m_hShadowActor->GetElement( nActionIndex )->szName.c_str();
						if( fFrame < 0.f ) fFrame = 0.f;
						m_hShadowActor->SetAction( szActionName, fFrame, 0.f );
					}
				}

			}
			break;
#endif
		case eActor::SC_FREEZINGPRISON_DURABILITY:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nStateBlowID = -1;
				float fDurabilityRate = 0.0f;
				bool bShowGauge = false;

				Stream.Read(&nStateBlowID, sizeof(nStateBlowID));
				Stream.Read(&fDurabilityRate, sizeof(fDurabilityRate));
				Stream.Read(&bShowGauge, sizeof(bShowGauge));

				//히트 이펙트 보여 주기
				// 최초 fDurabilityRate가 100.0f일 경우는 Hit액션 안하도록 추가[2010/12/16 semozz]
				if (bShowGauge && 
					fDurabilityRate != 100.0f &&
					fDurabilityRate > 0.0f)
				{
					DnBlowHandle hBlow = m_pStateBlow->GetStateBlowFromServerID( nStateBlowID );
					CDnFreezingPrisonBlow* pFreezingPrisonBlow = hBlow.GetPointer() ? static_cast<CDnFreezingPrisonBlow*>( hBlow.GetPointer() ) : NULL;
					if (pFreezingPrisonBlow)
						pFreezingPrisonBlow->OnHit();
				}

				//Guage UI표시
//				pFreezingPrisonBlow->ShowGuage(fDurabilityRate, bShowGauge);
				GetInterface().ShowDurabilityGauge(GetActorHandle(), nStateBlowID, fDurabilityRate, bShowGauge);

				OutputDebug("SC_FREEZINGPRISON_DURABILITY : ActorHandle 0x%x, BlowID : %d, Durability : %f\n", this, nStateBlowID, fDurabilityRate);
			}
			break;

		case eActor::SC_SHOW_STATE_EFFECT:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				DWORD dwUniqueID = 0;
				STATE_BLOW::emBLOW_INDEX BlowIndex;
				bool bShowGraphicEffect = false;
				Stream.Read( &dwUniqueID, sizeof(dwUniqueID) );
				Stream.Read( &BlowIndex, sizeof(BlowIndex) );
				Stream.Read( &bShowGraphicEffect, sizeof(bShowGraphicEffect));

				//DnBlowHandle hBlowHandle = m_pStateBlow->GetStateBlowFromServerID( BlowServerID );
				DNVector(DnBlowHandle) vlhHandles;
				m_pStateBlow->GetStateBlowFromBlowIndex(BlowIndex, vlhHandles);
				if (!vlhHandles.empty())
				{
					vlhHandles[0]->ShowGraphicEffect(bShowGraphicEffect);
				}
			}
			break;


		case eActor::SC_FINISH_AURASKILL:
			{
				CPacketCompressStream Stream( pPacket, 32 );
				DWORD dwUniqueID = -1;
				DWORD dwSkillID = -1;
				Stream.Read( &dwUniqueID, sizeof(dwUniqueID) );
				Stream.Read( &dwSkillID, sizeof(dwSkillID) );

				if (dwUniqueID == GetUniqueID())
				{
					CmdFinishAuraSkill(dwSkillID);
				}
			}
			break;
		case eActor::SC_ACTION_CHANGE:
			{
				CPacketCompressStream Stream( pPacket, 16 );
				int nActionIndex = -1;

				Stream.Read( &nActionIndex, sizeof(nActionIndex) );
				ActionChange(nActionIndex);
			}
			break;

		case eActor::SC_CHAINATTACK_PROJECTILE:
			{
				CPacketCompressStream Stream( pPacket, 256 );

				CreateChainAttackProjectile(Stream);
			}
			break;

		case eActor::SC_PROBINVINCIBLE_SUCCESS:
			{
				// 패킷 내용은 따로 없다. 그냥 resist!! 표시.
				// 본인 포함 같은 파티원들에게 면역을 띄워준다.
				SAABox box;
				GetBoundingBox(box);
				float fHeight = box.Max.y - box.Min.y;
				EtVector3 vPos = m_matexWorld.m_vPosition;
				vPos.y += fHeight;

				if( CDnActor::s_hLocalActor ) 
				{
					CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(this);
					bool bPartyMember = (pMonsterActor == NULL) &&		// 몬스터가 아니고
						(GetTeam() == CDnActor::s_hLocalActor->GetTeam());	// 팀이 같으면 파티원 이다.
					if(  bPartyMember )
					{
						if( CGameOption::GetInstance().bMyCombatInfo ) {
							GetInterface().SetResist( vPos );
						}								
					}
				}

				// 이펙트 출력을 위해 상태효과 쪽에도 알림.
				DNVector(DnBlowHandle) vlhBlow;
				m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_226, vlhBlow );
				if( false == vlhBlow.empty() )
				{
					CDnProbInvincibleAtBlow* pBlow = static_cast<CDnProbInvincibleAtBlow*>(vlhBlow.front().GetPointer());
					pBlow->OnSuccess();
				}
			}
			break;

		case eActor::SC_FORCE_ADD_SKILL:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillID;
				Stream.Read( &nSkillID , sizeof(int) );
				AddSkill(nSkillID);
			}
			break;

		case eActor::SC_FORCE_REMOVE_SKILL:
			{
				CPacketCompressStream Stream( pPacket, 128 );

				int nSkillID;
				Stream.Read( &nSkillID , sizeof(int) );
				RemoveSkill(nSkillID);
			}
			break;

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
		case eActor::SC_MAILBOX_OPEN:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				bool isOpen = false;

				Stream.Read(&isOpen, sizeof(bool));
				
				SetMailBoxOpenByShortCutKey(isOpen);
			}
			break;
		case eActor::SC_MAILBOX_CLOSE:
			{
				CPacketCompressStream Stream( pPacket, 128 );
				bool isOpen = false;

				Stream.Read(&isOpen, sizeof(bool));

				SetMailBoxOpenByShortCutKey(isOpen);
			}
			break;
#endif // PRE_ADD_MAILBOX_OPEN

#if defined(PRE_FIX_68898)
		case eActor::SC_SKIP_END_ACTION:
			{
				bool isSkipEndAction = false;

				CPacketCompressStream Stream( pPacket, 128 );
				Stream.Read( &isSkipEndAction, sizeof(bool) );

				if (isSkipEndAction == true)
					SetSkipEndAction(isSkipEndAction);
			}
			break;
#endif // PRE_FIX_68898
	}
}

void CDnActor::CmdWarp( EtVector3 &vPos, EtVector2 &vLook )
{	
	SetPosition( vPos );
	SetPrevPosition( vPos );
	if( EtVec2LengthSq( &vLook ) > 0.f )
		Look( vLook );

	if( abs( CDnWorld::GetInstance().GetHeight( vPos ) - vPos.y ) > 5.f ) {
		SetVelocityY( -3.0f );
		SetResistanceY( -18.0f );
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

	// 결빙 상태효과 걸려 있는채로 맵 이동되는 경우는 거의 없겠지만 만약 그렇다면 제거함.
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_041 ) )
		m_pStateBlow->RemoveStateBlowByBlowDefineIndex( STATE_BLOW::BLOW_041 );

	m_iCantActionReferenceCount = 0;
	m_iCantMoveReferenceCount = 0;

	// 무기 CreateObject 해줍니다. ( 라이트 효과때문에 EtcObject 를 생성하게 해뒀더니만 CDnEtcObject::ReleaseClass 하면서 삭제되버립니다. )
	for( int i=0; i<2; i++ ) {
		if( m_hWeapon[i] ) {
			m_hWeapon[i]->LoadEtcObject();
		}
	}
}


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
		OnSkillAura( m_hAuraSkill, false );

	// 토글 스킬 켜 놓은 것이 있으면 종료.
	if( m_hToggleSkill )
		OnSkillToggle( m_hToggleSkill, false );

	// 모든 상태효과 리셋
	RemoveAllBlowExpectPassiveSkill();
	
	if( IsPlayerActor() )
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(this);
		pPlayer->RemoveAllBubbles( false );
	}
}


void CDnActor::GetProperlyDieAction( /*IN OUT*/ string& strDieAction )
{
	strDieAction.assign( "Die" );
	const char* pCurrentAction = GetCurrentAction();
	if( pCurrentAction )
	{
		bool bCheckDie = false;
		if( IsStandHit( pCurrentAction ) )
			strDieAction = "Die", bCheckDie = true;
		if( IsStun( pCurrentAction ) && !IsAir( strDieAction.c_str() ) ) 
			strDieAction = "Die", bCheckDie = true;

		if( !bCheckDie ) {
			if( IsAir( pCurrentAction ) ) {
				if( IsExistAction( "Die_Air" ) ) strDieAction = "Die_Air";
			}
			if( IsDown( pCurrentAction ) ) {
				if( IsExistAction( "Die_Down" ) ) strDieAction = "Die_Down";
			}
		}
	}
}

void CDnActor::SetProcess( bool bFlag )
{
	if( IsProcess() == bFlag ) return;

	CDnUnknownRenderObject::SetProcess( bFlag );

	if( IsProcess() ) InsertOctreeNode();
	else RemoveOctreeNode();
}

CDnRenderBase *CDnActor::GetRenderBase()
{
	CDnRenderBase *pRender = dynamic_cast<CDnRenderBase *>(this);
	return pRender;
}

void CDnActor::ProcessPress()
{
	if( m_nPressLevel == 0 ) return;
	if( IsDie() ) return;
	if( IsFly() ) return;
	if( IsUnderGround() ) return;

	DNVector(DnActorHandle) hVecList;
	EtVector3 vAniDist = *GetPosition();
	vAniDist.x -= GetAniDistance()->x;
	vAniDist.z -= GetAniDistance()->z;

	float fScanRadius = 0.0f;
	if( m_Press == Press_Circle ) fScanRadius = (float)GetUnitSize() * 2.f;
	else if( m_Press == Press_Capsule ) fScanRadius = (float)GetUnitSize()*2.0f + 50.f;

	int nCount = ScanActor( vAniDist, fScanRadius, hVecList );
	if( nCount > 0 ) {
		for( int i=0; i<nCount; i++ ) {
			if( !hVecList[i] ) continue;
			if( hVecList[i] == GetMySmartPtr() ) continue;
			if( hVecList[i]->IsDie() ) continue;
			if( hVecList[i]->GetPressLevel() > m_nPressLevel ) continue;
			if( hVecList[i]->GetPressLevel() == -1 ) continue;
			if( abs( GetPosition()->y - hVecList[i]->GetPosition()->y ) > GetHeight() ) continue;
			if( m_Press == Press_NoneSameCircle && hVecList[i]->m_Press == Press_NoneSameCircle ) continue;

			ProcessPress( hVecList[i] );
		}
	}
}

void CDnActor::ProcessPress( DnActorHandle hTarget )
{
	bool bCheckPressed = false;
	EtVector2 Dir;
	float Dist = 0.0f;
	if( GetPress() == CDnActorState::Press_Circle && hTarget->GetPress() == CDnActorState::Press_Circle ) {
		if( CheckPressCircle2Clrcle2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			if( CDnActor::s_hLocalActor && hTarget == CDnActor::s_hLocalActor ) bCheckPressed = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Capsule && hTarget->GetPress() == CDnActorState::Press_Capsule ) {
		if( CheckPressCapsule2Capsule2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			if( CDnActor::s_hLocalActor && hTarget == CDnActor::s_hLocalActor ) bCheckPressed = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Capsule && hTarget->GetPress() == CDnActorState::Press_Circle ) {
		if( CheckPressCapsule2Circle2( GetMySmartPtr(), hTarget, Dir, Dist ) ) {
			hTarget->MoveToWorld( Dir * Dist );
			if( CDnActor::s_hLocalActor && hTarget == CDnActor::s_hLocalActor ) bCheckPressed = true;
		}
	}
	else if( GetPress() == CDnActorState::Press_Circle && hTarget->GetPress() == CDnActorState::Press_Capsule ) {
		if( CheckPressCapsule2Circle2( hTarget, GetMySmartPtr(), Dir, Dist ) ) {
			hTarget->MoveToWorld( -Dir * Dist );
			if( CDnActor::s_hLocalActor && hTarget == CDnActor::s_hLocalActor ) bCheckPressed = true;
		}
	}
	if( bCheckPressed ) {
		((CDnLocalPlayerActor*)hTarget.GetPointer())->AddPressedCount( (Dist > 10.0f) ? 10 : 1 );
	}
}

void CDnActor::ProcessSoftAppear( LOCAL_TIME LocalTime )
{
	if( m_SoftAppearStep == AS_NONE || m_SoftAppearStep == AS_SUSPEND ) {
		return;
	}
	if( !IsShow() ) {
		return;
	}
	const int APPEAR_TIME = 500;
	const int DISAPPEAR_TIME = 500;

	if( m_SoftTime == 0 ) m_SoftTime = LocalTime;

	if( m_SoftAppearStep == AS_APPEAR ) {
		float fAlpha = (float)(LocalTime - m_SoftTime) / APPEAR_TIME;
		if( fAlpha > 1.0f ) {
			SetAlphaBlend( 1.0f , AL_APPEAR );
			m_SoftAppearStep = AS_SUSPEND;
			m_SoftTime = 0;
		}
		else {
			SetAlphaBlend( fAlpha , AL_APPEAR );
		}
	}
	else if( m_SoftAppearStep == AS_DISAPPEAR ) {
		float fAlpha = 1.0f - (float)(LocalTime - m_SoftTime) / DISAPPEAR_TIME;
		if( fAlpha < 0.0f ) {
			SetAlphaBlend( 0.0f , AL_APPEAR );
			m_SoftAppearStep = AS_NONE;
			m_SoftTime = 0;
			CDnUnknownRenderObject<CDnActor>::SetDestroy();
		}
		else {
			SetAlphaBlend(  fAlpha , AL_APPEAR );
		}
	}
}

void CDnActor::SetDestroy()
{
	if( m_SoftAppearStep != AS_NONE  ) {
		if( m_SoftAppearStep != AS_DISAPPEAR )
			m_SoftTime = 0;
		m_SoftAppearStep = AS_DISAPPEAR;
		SetUniqueID( -1 );
	}
	else {
		CDnUnknownRenderObject<CDnActor>::SetDestroy();
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

#ifdef RENDER_PRESS_hayannal2009
void CDnActor::RenderPress()
{
	if( !s_bEnableRenderPress )
		return;

	EtMatrix matWorld = *GetMatEx();
	unsigned short Indices[] = {
		0, 1, 1, 2, 2, 0,
		0, 2, 2, 3, 3, 0,
		0, 3, 3, 4, 4, 0,
		0, 4, 4, 5, 5, 0,
		0, 5, 5, 6, 6, 0,
		0, 6, 6, 7, 7, 0,
		0, 7, 7, 8, 8, 0,
		0, 8, 8, 9, 9, 0,
	};

	if( m_Press == Press_Circle ) {
		// DrawLine3D으로 그려야 와이어로 나온다.
		for( int i = 0; i < 8 * 3; ++i )
			EternityEngine::DrawLine3D( m_PressVertices1[Indices[i*2]].Position, m_PressVertices1[Indices[i*2+1]].Position, m_PressVertices1[Indices[i]].dwColor, &matWorld );
	}
	else if( m_Press == Press_Capsule ) {
		for( int i = 0; i < 8 * 3; ++i )
			EternityEngine::DrawLine3D( m_PressVertices1[Indices[i*2]].Position, m_PressVertices1[Indices[i*2+1]].Position, m_PressVertices1[Indices[i]].dwColor, &matWorld );
		for( int i = 0; i < 8 * 3; ++i )
			EternityEngine::DrawLine3D( m_PressVertices2[Indices[i*2]].Position, m_PressVertices2[Indices[i*2+1]].Position, m_PressVertices2[Indices[i]].dwColor, &matWorld );
		for( int i = 0; i < 2 * 3; ++i )
			EternityEngine::DrawLine3D( m_PressVertices3[Indices[i*2]].Position, m_PressVertices3[Indices[i*2+1]].Position, m_PressVertices3[Indices[i]].dwColor, &matWorld );
	}

//#define _VOICE_TEST
#ifdef _VOICE_TEST
	EtMatrix matWorldPos;
	EtMatrixIdentity( &matWorldPos );
	matWorldPos._41 = matWorld._41;
	matWorldPos._42 = matWorld._42 + 20.0f;
	matWorldPos._43 = matWorld._43;

	SPrimitiveDraw3D WorldVertices1[6];
	WorldVertices1[ 0 ].Position = EtVector3( 0.0f, 0.0f, 0.0f );
	WorldVertices1[ 1 ].Position = EtVector3( 60.0f, 0.0f, 0.0f );
	WorldVertices1[ 2 ].Position = EtVector3( 0.0f, 0.0f, 0.0f );
	WorldVertices1[ 3 ].Position = EtVector3( 0.0f, 60.0f, 0.0f );
	WorldVertices1[ 4 ].Position = EtVector3( 0.0f, 0.0f, 0.0f );
	WorldVertices1[ 5 ].Position = EtVector3( 0.0f, 0.0f, 60.0f );
	WorldVertices1[ 0 ].dwColor = 0xFFFF0000;
	WorldVertices1[ 1 ].dwColor = 0xFFFF0000;
	WorldVertices1[ 2 ].dwColor = 0xFF00FF00;
	WorldVertices1[ 3 ].dwColor = 0xFF00FF00;
	WorldVertices1[ 4 ].dwColor = 0xFF0000FF;
	WorldVertices1[ 5 ].dwColor = 0xFF0000FF;

	//GetEtDevice()->SetWorldTransform( &matWorldPos );
	//GetEtDevice()->DrawPrimitiveUP( PT_LINELIST, 3, WorldVertices1, sizeof( SPrimitiveDraw3D ) );

	EternityEngine::DrawLine3D( WorldVertices1[0].Position, WorldVertices1[1].Position, WorldVertices1[0].dwColor, &matWorldPos );
	EternityEngine::DrawLine3D( WorldVertices1[2].Position, WorldVertices1[3].Position, WorldVertices1[2].dwColor, &matWorldPos );
	EternityEngine::DrawLine3D( WorldVertices1[4].Position, WorldVertices1[5].Position, WorldVertices1[4].dwColor, &matWorldPos );

	EtVector3 vPos[2];
	vPos[0] = GetMatEx()->m_vPosition;
	vPos[1] = GetMatEx()->m_vPosition + ( GetMatEx()->m_vZAxis * 100.f );
	for( int i=0; i<2; i++ ) vPos[i].y += 50.f;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xffffffff );
	vPos[0] = GetMatEx()->m_vPosition;
	vPos[1] = GetMatEx()->m_vPosition + ( GetMatEx()->m_vXAxis * 100.f );
	for( int i=0; i<2; i++ ) vPos[i].y += 50.f;
	EternityEngine::DrawLine3D( vPos[0], vPos[1], 0xffffffff );
#endif
}
#endif

void CDnActor::AddBindeActionObjectHandle( DnEtcHandle hHandle )
{ 
	m_vecBindActionObjectHandle.push_back(hHandle);
}

void CDnActor::ReleaseBindActionObject(const char* szPrevAction)
{

#if defined( PRE_ADD_ACTION_OBJECT_END_ACTION )
	for( DWORD i=0; i<m_vecBindActionObjectHandle.size(); i++ )
	{
		if( m_vecBindActionObjectHandle[i] && m_vecBindActionObjectHandle[i]->IsDestroy() == false )
		{
			ActionElementStruct *pStruct = GetElement( szPrevAction );
			if( pStruct && strcmp ( GetCurrentAction() , pStruct->szNextActionName.c_str() ) == NULL )
			{
				m_vecBindActionObjectHandle[i]->SetDestroy();
			}
			else if( m_vecBindActionObjectHandle[i]->IsExistAction( "End_Action" ) == true )
			{
				if( strcmp( m_vecBindActionObjectHandle[i]->GetCurrentAction(), "End_Action" ) != NULL )
					m_vecBindActionObjectHandle[i]->SetActionQueue( "End_Action" );
			}
			else
			{
				m_vecBindActionObjectHandle[i]->SetDestroy();
			}
		}
	}
#else
	// 액션 오브젝트를 만든 액션이 끊기면 생성되었던 액션 오브젝트도 같이 사라지게 처리. (#14162)
	int nActionIndex = GetElementIndex( szPrevAction );
	if( m_mapBindingActionObjects.end() != m_mapBindingActionObjects.find( nActionIndex ) )
	{
		vector<CDnActionSignalImp*>& vlEtcObjects = m_mapBindingActionObjects[ nActionIndex ];
		int iNumActionObjects = (int)vlEtcObjects.size();
		for( int i = 0; i < iNumActionObjects; ++i )
		{
			CDnActionSignalImp* pActionObject = vlEtcObjects.at( i );
			if( pActionObject )
				pActionObject->ReleaseSignalImp( CDnActionSignalImp::EtcObject );
		}
		vlEtcObjects.clear();
	}
#endif

	m_vecBindActionObjectHandle.clear();
}

void CDnActor::SyncBindActionObjectFPS( float fFPS )
{
	for( DWORD i=0; i<m_vecBindActionObjectHandle.size(); i++ )
	{
		if( m_vecBindActionObjectHandle[i] )
			m_vecBindActionObjectHandle[i]->CDnActionBase::SetFPS( fFPS );
	}
}

void CDnActor::OnChangeAction( const char *szPrevAction )
{
	ReleaseBindActionObject(szPrevAction);

	if( szPrevAction == NULL )
		return;

	CDnActionSignalImp::OnChangeAction( szPrevAction );
#ifdef _SOCIAL_NEXT_ACTION_KALLISTE
	if (m_SocialActCtrler.IsEmpty() == false)
	{
		if (m_SocialActCtrler.nCondition == 1)	// FrameEnd - todo : to string
		{
			SetFaceAction(m_SocialActCtrler.szNextActionName.c_str());
			m_SocialActCtrler.Clear();
		}
	}
#endif
	// 같은 액션 반복중이면 패스
	if( szPrevAction && strcmp(GetCurrentAction(), szPrevAction) == 0 )
		return;

	if( m_hProcessSkill )
	{
		// if instantly passive skill, then cancel skill. ( ex) archer's spinkick)
		// because state effect must be deactivate when change to another attack action.
		set<string> setPrevAction;
		setPrevAction.insert( szPrevAction );

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
			m_hProcessSkill->IsUseActionNames( setPrevAction ) )
		{
			// 액티브 스킬이 패시브 형태로 등록되어 사용되었을때 , GetPassiveSkillLengh() 로 알수있다.
			if( ( m_hProcessSkill->GetPassiveSkillLength() != 0.f || m_hProcessSkill->GetSkillType() == CDnSkill::Passive ) &&
				m_hProcessSkill->GetDurationType() == CDnSkill::Instantly )
			{
				m_hProcessSkill->OnEnd( MAActorRenderBase::m_LocalTime, 0.0f );
				m_hProcessSkill.Identity();
			}
			else if( IsEnabledAuraSkill() && m_hProcessSkill->IsAuraOn() )
			{
				// Note 한기: m_hProcessSkill 스마트 포인터는 오라 스킬 사용하는 액션이 재생되는 동안은 유효해야
				// 게임 서버에서 CDnPlayerActor::CmdStop() 쪽에서 걸러지기 때문에 겜 서버에서 해당 액션 시그널이 끝까지 
				// 처리됨. 따라서 CDnActor::OnChangeAction 쪽에서 ProcessAction 을 Identity 시킴.
				m_hProcessSkill.Identity();
			}
		}
	}
}

void CDnActor::OnDie( DnActorHandle hHitter )
{
	m_fDieDelta = m_fMaxDieDelta;
}


void CDnActor::InsertUniqueSearchMap( CDnActor *pActor )
{
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	if( pActor->GetUniqueID() == -1 ) return;
	s_dwMapActorSearch.insert( make_pair( pActor->GetUniqueID(), pActor->GetMySmartPtr() ) );
}

void CDnActor::RemoveUniqueSearchMap( CDnActor *pActor )
{
	ScopeLock<CSyncLock> Lock(s_LockSearchMap);
	if( pActor->GetUniqueID() == -1 ) return;
	std::map<DWORD, DnActorHandle>::iterator it;
	it = s_dwMapActorSearch.find( pActor->GetUniqueID() );
	if( it != s_dwMapActorSearch.end() ) {
		s_dwMapActorSearch.erase( it );
	}
}

void CDnActor::SetUniqueID( DWORD dwUniqueID )
{ 
	RemoveUniqueSearchMap( this );
	m_dwUniqueID = dwUniqueID;
	InsertUniqueSearchMap( this );
}

void CDnActor::RenderCustom( float fElapsedTime )
{
	if( m_bShowHeadName && IsShow() ) {
		SAABox box;
		GetBoundingBox(box);
		float fHeight = box.Max.y - box.Min.y + GetIncreaseHeight();

		EtVector3 vPos = m_matexWorld.m_vPosition;
		vPos.y += fHeight;

		EtMatrix HeadMat = GetBoneMatrix("Bip01 Head");
		EtVector3 vHeadPos = *(EtVector3*)&HeadMat._41;
		m_ChatBalloon.RenderChatBalloon( vPos, vHeadPos, fElapsedTime );

	}
	MAHeadNameRender::Render();
}

void CDnActor::CalcCustomRenderDepth()
{
	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if( hCamera ) {
		SAABox box;
		GetBoundingBox(box);
		float fHeight = box.Max.y - box.Min.y + m_fIncreaseHeight;
		EtVector3 vPos = m_matexWorld.m_vPosition;
		vPos.y += fHeight;
		m_fCustomRenderDepth = EtVec3Dot(&vPos, &hCamera->GetMatEx()->m_vZAxis);
	}
}

bool CDnActor::bIsAppliedSkill( const int iSkillIndex )
{
	if( iSkillIndex == 0 )
		return true;

	for( int i=0 ; i<GetNumAppliedStateBlow() ; ++i )
	{
		DnBlowHandle hBlow = GetAppliedStateBlow( i );
		if( hBlow )
		{
			const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
			if( !pSkillInfo )
				continue;

			if( pSkillInfo->iSkillID == iSkillIndex )
				return true;
		}
	}

	return false;
}
 

bool CDnActor::IsAppliedThisStateBlow( STATE_BLOW::emBLOW_INDEX iBlowIndex )
{
	if( !m_pStateBlow )
		return false;

	return m_pStateBlow->IsApplied( iBlowIndex );
}


void CDnActor::PreThreadRelease()
{
	SetProcess( false );
	EraseCustomRenderList();
}

void CDnActor::InitializeNextStage()
{
	for( DWORD i=0; i<s_pVecProcessList.size(); i++ ) 
	{
		if( !s_pVecProcessList[i]->IsPlayerActor() ) 
		{
			s_pVecProcessList[i]->Release();
			i--;
		}
	}
}

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
void CDnActor::CheckPreSignal( ActionElementStruct *pElement, int nElementIndex, CEtActionSignal *pSignal, int nSignalIndex, CEtActionBase *pActionBase )
{
	switch( pSignal->GetSignalIndex() ) {
		case STE_Sound:
			{
				std::string szTemp = pElement->szName;
				ToLowerA( szTemp );
				if( strstr( szTemp.c_str(), "skill_" ) ) {
					SoundStruct *pStruct = (SoundStruct *)pSignal->GetData();
					pStruct->bLoadingComplete = false;
					pStruct->nVecSoundIndex = new std::vector<int>;
					return;
				}
			}
			break;
		case STE_ActionObject:
			break;
		case STE_Particle:
			break;
		case STE_FX:
			break;
		case STE_Decal:
			break;
		case STE_Projectile:
			break;
#ifdef PRE_ADD_MARK_PROJECTILE
		case STE_MarkProjectile:
			break;
#endif // PRE_ADD_MARK_PROJECTILE
	}
	CDnActionBase::CheckPreSignal( pElement, nElementIndex, pSignal, nSignalIndex, pActionBase );
}
#endif

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
void CDnActor::FlushSoundSignal()
{
	for( DWORD i=0; i<m_pVecActionElementList->size(); i++ ) {
		ActionElementStruct *pElement = (*m_pVecActionElementList)[i];

		std::string szTemp = pElement->szName;
		ToLowerA( szTemp );
		if( strstr( szTemp.c_str(), "skill_" ) ) {
			for( DWORD j=0; j<pElement->pVecSignalList.size(); j++ ) {
				CEtActionSignal *pSignal = pElement->pVecSignalList[j];
				if( pSignal->GetSignalIndex() == STE_Sound ) {
					CheckPostSignal( pElement, i, pSignal, j, NULL );
					CheckPreSignal( pElement, i, pSignal, j, NULL );
				}
			}
		}
	}
}

void CDnActor::PreloadSkillSoundSignal()
{
	for( DWORD i=0; i<m_pVecActionElementList->size(); i++ ) {
		ActionElementStruct *pElement = (*m_pVecActionElementList)[i];

		std::string szTemp = pElement->szName;
		ToLowerA( szTemp );
		if( strstr( szTemp.c_str(), "skill_" ) && IsUsingSkillAction( pElement->szName.c_str() ) ) {
			for( DWORD j=0; j<pElement->pVecSignalList.size(); j++ ) {
				CEtActionSignal *pSignal = pElement->pVecSignalList[j];
				if( pSignal->GetSignalIndex() == STE_Sound ) {
					LoadSoundStruct( (SoundStruct*)pSignal->GetData() );
				}
			}
		}
	}
}
#endif

void CDnActor::DeleteAllActor()
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
	for( int i=0; i<CDnActor::GetItemCount(); i++ ) {
		DnActorHandle hActor = CDnActor::GetItem(i);
		if( !hActor ) continue;
		if( !hActor->IsPlayerActor() ) continue;
		if( ((CDnPlayerActor*)hActor.GetPointer())->IsDeveloperAccountLevel() ) {
			SAFE_RELEASE_SPTR( hActor );
			i--;
		}
	}
}

void CDnActor::MakePassiveSkillActionInfo()
{
	for( int i = 0; i < (int)GetElementCount(); ++i )
	{
		CEtActionBase::ActionElementStruct* pElement = GetElement( i );
		for( int k = 0; k < (int)pElement->pVecSignalList.size(); ++k )
		{
			CEtActionSignal* pSignal = pElement->pVecSignalList.at(k);
			if( STE_InputHasPassiveSkill == pSignal->GetSignalIndex() )
			{	
				// 보유하고 있는 스킬의 액션 이름을 모아두기 위해 패시브 스킬 인덱스 기준으로 액션 인덱스를 모아둔다
				InputHasPassiveSkillStruct* pInfo = (InputHasPassiveSkillStruct*)pSignal->GetData();
				if( false == pInfo->bOnlyCheck )
					m_mapPassiveSkillActions[ pInfo->nSkillIndex ].insert( pInfo->szChangeAction );
			}
		}
	}
}

void CDnActor::FillPassiveSkillActions( int iSkillID, set<string>& setUseActionNames )
{
	map<int, set<string> >::iterator iter = m_mapPassiveSkillActions.find( iSkillID );
	if( m_mapPassiveSkillActions.end() != iter )
	{
		const set<string>& setActions = iter->second;
		set<string>::const_iterator iterSet = setActions.begin();
		for( iterSet; iterSet != setActions.end(); ++iterSet )
		{
			setUseActionNames.insert( *iterSet );
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
#else
	for( DWORD i = 0; i < GetSkillCount(); ++i )
	{
		DnSkillHandle hSkill = GetSkillFromIndex( i );
#endif // #ifndef PRE_FIX_SKILLLIST
		if(!hSkill)
			continue;

		if (eSkillType == hSkill->GetSkillType())
			vlSkillList.push_back(hSkill);
	}
}

void CDnActor::OnLoopAction( float fFrame, float fPrevFrame )
{

}



bool CDnActor::CheckAnimation( const char *szActionName )
{
	const char *szName = szActionName;

	if( m_szActionQueue.empty() ) {
		if( strcmp( szName, m_szAction.c_str() ) == NULL ) 
			return false;
	}
	else {
		if( strcmp( szName, m_szActionQueue.c_str() ) == NULL ) 
			return false;
	}

	return true;
}

void CDnActor::CmdFinishAuraSkill(DWORD dwSkillID)
{
	DnSkillHandle hSkill = FindSkill(dwSkillID);
	if (hSkill)
	{
		// [2011/03/22 semozz]
		// 현재 스킬이 오라 스킬과 같은 스킬이면 현재 스킬도 초기화.
		if (m_hProcessSkill == hSkill)
			m_hProcessSkill.Identity();

		OnSkillAura(hSkill, false);
	}
}

const char *CDnActor::CheckChangeActionBlow( const char* szActionName )
{
	const char *szResultActionName = szActionName;
//	std::string szResultAction = szActionName;
	// 129번 액션 이름 대체 상태효과 ///////////////////////////////////////////////////////////////////
	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_129 ) )
	{
		DNVector(DnBlowHandle) vlhChangeActionSetBlow;
		m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_129, vlhChangeActionSetBlow );

		// 액션 셋 변경 상태효과는 여러개 있을 수 있다.
		int iNumBlow = (int)vlhChangeActionSetBlow.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			CDnChangeActionSetBlow* pChangeActionSetBlow = static_cast<CDnChangeActionSetBlow*>( vlhChangeActionSetBlow.at(i).GetPointer() );
			if( STATE_BLOW::STATE_END != pChangeActionSetBlow->GetBlowState() )
			{
				CDnChangeActionStrProcessor* pProcessor = pChangeActionSetBlow->GetChangeActionStrProcessor();
				if( pProcessor )
				{
					// #31697 플레이어인 경우 특정 무기 액션으로 바뀌어있다면 복구 시켜서 호출해준다.
					if( this->IsPlayerActor() )
					{
						CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(this);
						if( pPlayerActor->IsChangedEquipAction() )
						{
							szResultActionName = pPlayerActor->GetOriginalNoneActionEquipAction();
						}
					}
					//////////////////////////////////////////////////////////////////////////

					string strNowActionName(szResultActionName);
					const char* pChangeActionName = pProcessor->GetChangeActionName( strNowActionName );
					if( pChangeActionName )
					{
						szResultActionName = pChangeActionName;

						// 액션이 바뀌었다면 루프 빠져나옴. 다음 129번 상태효과를 체크한다.
						// 같은 액션을 바꾸는 129번 상태효과가 여러개 있는 경우는 지원하지 않는다.
						break;
					}
					else
					{
						// #32234 바뀌지 않았다면 원래 액션으로 복귀.
						szResultActionName = szActionName;
					}
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	if( m_pStateBlow->IsApplied( STATE_BLOW::BLOW_121 ) )
	{
		if( 0 == strcmp(szActionName, "Stand") )
		{
			DNVector(DnBlowHandle) vlhChangeStandActionBlow;
			m_pStateBlow->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_121, vlhChangeStandActionBlow );
			if( false == vlhChangeStandActionBlow.empty() )
			{
				// 무조건 한개만 취급한다.
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
	if (!hBlow)
		return;
	
	if( hBlow->IsUseTableDefinedGraphicEffect() )
		hBlow->AttachGraphicEffectDefaultType();

	hBlow->OnBegin(CDnActionBase::m_LocalTime, 0.0f);
	OnBeginStateBlow( hBlow );

	// 곧바로 end 로 셋팅하는 blow 도 있기 때문에 체크.
	if( STATE_BLOW::STATE_END != hBlow->GetBlowState() )
		hBlow->SetState( STATE_BLOW::STATE_DURATION );
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


void CDnActor::ResetStateEffects()
{
	m_setNowPlayingEffectSkill.clear();

	if (m_pStateBlow)
		m_pStateBlow->RestoreAllBlowGraphicEffect();
}

void CDnActor::ActionChange(int nActionIndex)
{
	if (nActionIndex == -1)
		return;

	ActionElementStruct *pActionElement = GetElement(nActionIndex);
	if (pActionElement == NULL)
		return;

	std::string szAction = pActionElement->szName;
	CmdAction( szAction.c_str(), 0, 3.0f, true, true, false );

	// #37325 [왁스맞고 일어나지못함] 다운이 포함된 액션일 경우 <m_fDownDelta 가 포함되어야 하지만 일반적으로 Hit에 의해 계산될때는 설정이 되나 , 
	// 이경우에는 m_fDownDelta 가 설정이 안되는 경우가있다. 0이면 ProcessDown() 함수가 동작이 안한다, 다운을 포함한 액션일경우에 다운델타를 체크 없을경우 넣어준다.
	if((GetState() & CDnActorState::Down) && GetDownRemainDelta() <= 0) 
		SetDownRemainDelta(0.1f); // 일단 예외적인 경우니 바로 일어나게 한다.
}


void CDnActor::CreateChainAttackProjectile(CPacketCompressStream& Stream)
{
	DWORD dwRootAttackerID = 0;
	DWORD dwPrevAttackerActorUniqueID = 0;
	DWORD dwActorToAttackID = 0;

	int iActionIndex = -1;
	int iProjectileSignalArrayIndex = -1;
	int iSkillID = -1;


	Stream.Read(&dwRootAttackerID, sizeof(DWORD));
	Stream.Read(&dwPrevAttackerActorUniqueID, sizeof(DWORD));
	Stream.Read(&dwActorToAttackID, sizeof(DWORD));

	Stream.Read(&iActionIndex, sizeof(int));
	Stream.Read(&iProjectileSignalArrayIndex, sizeof(int));
	Stream.Read(&iSkillID, sizeof(int));

	//1. 발사체 시그널 정보 설정.
	//////////////////////////////////////////////////////////////////////////
	DnActorHandle hRootAttacker = FindActorFromUniqueID(dwRootAttackerID);
	DnActorHandle hPrevActor = FindActorFromUniqueID(dwPrevAttackerActorUniqueID);
	DnActorHandle hActorToAttack = FindActorFromUniqueID(dwActorToAttackID);

	//PrevActor는 없을 수 있음..
	if (!hRootAttacker || /*!hPrevActor ||*/ !hActorToAttack)
		return;

	int iRootAttackerTeam = hRootAttacker->GetTeam();

	
	ProjectileStruct projectileSignalInfo;
	bool bFound = false;

	CEtActionSignal *pSignal = hRootAttacker->GetSignal( iActionIndex, iProjectileSignalArrayIndex );
	if( pSignal && pSignal->GetSignalIndex() == STE_Projectile ) {
#ifdef PRE_FIX_MEMOPT_SIGNALH
		CopyShallow_ProjectileStruct(projectileSignalInfo, static_cast<ProjectileStruct*>(pSignal->GetData()));
#else
		projectileSignalInfo = *(static_cast<ProjectileStruct*>(pSignal->GetData()));
#endif
		bFound = true;
	}
	//////////////////////////////////////////////////////////////////////////
	
	//Signal정보를 못 찾으면 발사체 생성 못하도록...(클라이언트 덤프 과련)
	if (bFound == false)
		return;
	
	//3. 발사체 정보 설정 하고 생성
	//////////////////////////////////////////////////////////////////////////
	ProjectileStruct* pProjectileSignalInfo = &projectileSignalInfo;

	// 프로젝타일 발사 관련 기타 설정들.
	DnSkillHandle hSkill = hRootAttacker->FindSkill( iSkillID );

	// 호밍으로 타겟을 설정토록 정해준다.
	pProjectileSignalInfo->nOrbitType = CDnProjectile::Homing;
	pProjectileSignalInfo->nTargetType = CDnProjectile::Target;
	pProjectileSignalInfo->VelocityType = CDnProjectile::Accell;
	pProjectileSignalInfo->fSpeed = 1000.0f;//CHAINATTACK_PROJECTILE_SPEED;
	pProjectileSignalInfo->nValidTime = 5000;


	CDnProjectile* pProjectile = CDnProjectile::CreateProjectile( hRootAttacker, *GetMatEx(), pProjectileSignalInfo, EtVector3( 0.0f, 0.0f, 0.0f), hActorToAttack );
	if( pProjectile )
	{
		pProjectile->SetTargetActor( hActorToAttack );
		pProjectile->SetChainShooter( GetMySmartPtr() );
	}

	
	//////////////////////////////////////////////////////////////////////////
}

void CDnActor::RequestSummonOff(DWORD dwSummonMonsterUniqueID)
{
	BYTE pBuffer[64] = { 0, };
	CPacketCompressStream Stream( pBuffer, 64 );

	Stream.Write( &dwSummonMonsterUniqueID, sizeof(DWORD) );
	Send( eActor::CS_SUMMONOFF, &Stream );
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

#if defined(PRE_ADD_MULTILANGUAGE)
void CDnActor::_CmdChatBaloon(const WCHAR * pwszMessage)
{
#ifdef PRE_ADD_CASH_AMULET
	if(GetChatBalloon().IsEnableCustomChatBallon())
	{
		GetChatBalloon().StopChatBalloonCustom();
		GetChatBalloon().ClearCustomChatBalloonText();
	}	

	if(CGameOption::GetInstance().m_bShowCashChatBalloon && IsPlayerActor())
	{
		CDnPlayerActor* pPlayer = static_cast<CDnPlayerActor*>(GetActorHandle().GetPointer());
		DnPartsHandle hCashAmulet = pPlayer->GetCashParts(CDnParts::CashNecklace);

		if(hCashAmulet)
		{
			int nItemID = hCashAmulet->GetClassID();
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCASHCHATBALLOONTABLE );

			if(pSox && pSox->IsExistItem(nItemID))
				GetChatBalloon().SetChatBalloonCustom( pSox->GetFieldFromLablePtr( nItemID, "_UIResourceName1" )->GetString() , pwszMessage , chatroomcolor::NORMAL, GetTickCount(), 1 );
			else
				SetChatBalloonText( pwszMessage, GetTickCount(), 1, true );

		}
		else
			SetChatBalloonText( pwszMessage, GetTickCount(), 1, true );

	}
	else
		SetChatBalloonText( pwszMessage, GetTickCount(), 1, true );

#else
	SetChatBalloonText( pwszMessage, GetTickCount(), 1, true );
#endif
}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

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


void CDnActor::ScanActorByStateIndex(DNVector(DnActorHandle) &Veclist, STATE_BLOW::emBLOW_INDEX blowIndex)
{
	DNVector(DnActorHandle) scanActorList;
	ScanActor(*GetPosition(), FLT_MAX, scanActorList);
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
					if (pSkillInfo && pSkillInfo->hSkillUser == GetMySmartPtr())
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
	m_matexWorld.MoveLocalXAxis( m_vRotAxis.x );
	m_matexWorld.MoveLocalYAxis( m_vRotAxis.y );
	m_matexWorld.MoveLocalZAxis( m_vRotAxis.z );

	// 회전.
	m_matexWorld.RotateYaw( (-m_fSpeedRot) );
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

#if defined(PRE_FIX_61382)
//꼭두각시 몬스터일 경우 주인 액터에 상태효과 이펙트를 표시 해야 함.
DnActorHandle CDnActor::GetOwnerActorHandle(DnActorHandle hActor)
{
	DnActorHandle hOwnerActor = hActor;

	if (hActor && hActor->IsMonsterActor())
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
		if (pMonsterActor && pMonsterActor->IsPuppetSummonMonster() && pMonsterActor->GetSummonerPlayerActor())
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

	//클라이언트는 상태효과 제거 하지 않는다.(서버 패킷 받아서 제거....)
	DnMonsterActorHandle hMonsterActor;
	RemoveSummonMonsterStateEffectByGlyph(hMonsterActor, monsterID, glyphID);

	//실제 정보 제거.
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
// 				std::list<_StateEffectInfo>::iterator iter = stateEffectIDList.begin();
// 				std::list<_StateEffectInfo>::iterator endIter = stateEffectIDList.end();
// 
// 				for (; iter != endIter; ++iter)
// 				{
// 					_StateEffectInfo& info = (*iter);
// 
// 					if (hMonsterActor && hMonsterActor->GetUniqueID() == info.dwUniqueID)
// 						hMonsterActor->CmdRemoveStateEffectFromID(info.nStateEffectID);
// 				}
			}
		}
	}
}

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


void CDnActor::ProcessRlktAura(float fDelta)
{
	if (!CDnActor::s_hLocalActor) return;
	if (!m_bProcessRlktAura) return;

	m_fRlktAuraDelayTime -= fDelta;
	if (m_fRlktAuraDelayTime > 0.0f)
		return;
	
	m_bProcessRlktAura = false;
	m_hRlktAura->Show(false);
}


void CDnActor::TransformToActorID(int nActorIndex)
{
	if (m_nSwapActorID == nActorIndex) return;
	m_nSwapActorID = nActorIndex;

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TACTOR);

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable(CDnTableDB::TFILE);
	std::string szSkinName, szAniName, szActName;
	CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, nActorIndex, "_SkinName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, nActorIndex, "_AniName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szActName, pSox, nActorIndex, "_ActName", pFileNameSox);
#else
	std::string szSkinName = pSox->GetFieldFromLablePtr(m_nSwapActorID, "_SkinName")->GetString();
	std::string szAniName = pSox->GetFieldFromLablePtr(m_nSwapActorID, "_AniName")->GetString();
	std::string szActName = pSox->GetFieldFromLablePtr(m_nSwapActorID, "_ActName")->GetString();
#endif

	SAFE_RELEASE_SPTR(GetObjectHandle());


	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(this);
	if (pPartsBody)
		pPartsBody->LoadSkin(CEtResourceMng::GetInstance().GetFullName(szSkinName).c_str(), CEtResourceMng::GetInstance().GetFullName(szAniName).c_str());
	
	LoadAction(CEtResourceMng::GetInstance().GetFullName(szActName).c_str());

	if (m_hObject)
	{
		m_hObject->SetCollisionGroup(COLLISION_GROUP_DYNAMIC(1));
		m_hObject->SetTargetCollisionGroup(COLLISION_GROUP_STATIC(1) | COLLISION_GROUP_DYNAMIC(2) | COLLISION_GROUP_DYNAMIC(3));
	}

	//if (IsExistAction("Summon_On") == true)
	//	SetAction("Summon_On", 0.f, 0.f);
	//else
	//	SetAction("Stand", 0.f, 0.f);
		

	if (pPartsBody)
		pPartsBody->ShowParts(false);

	for (int i = 0; i<CDnWeapon::EquipSlot_Amount; i++)
	{
		DnWeaponHandle hWeapon = GetWeapon(i);
		if (hWeapon)
			LinkWeapon(i);

		ShowWeapon(i, false);
	}
}


void CDnActor::TransformToNormal()
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TACTOR);
	DNTableFileFormat*  pFileNameSox = GetDNTable(CDnTableDB::TFILE);

	std::string szSkinName, szAniName, szActName;
	CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nClassID, "_SkinName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nClassID, "_AniName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nClassID, "_ActName", pFileNameSox);

	SAFE_RELEASE_SPTR(GetObjectHandle());

	MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>(this);
	if (pPartsBody)
	{
		pPartsBody->LoadSkin(CEtResourceMng::GetInstance().GetFullName(szSkinName).c_str(), CEtResourceMng::GetInstance().GetFullName(szAniName).c_str());
		pPartsBody->CombineParts();
		pPartsBody->ShowParts(true);
	}

	//SetAction("Stand", 0.f, 0.f);

	m_nSwapActorID = -1;
}