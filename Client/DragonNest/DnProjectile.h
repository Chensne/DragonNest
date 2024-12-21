#pragma once

#include "DnUnknownRenderObject.h"
#include "EtMatrixEx.h"
#include "DnActionBase.h"
#include "DnActor.h"
#include "Timer.h"
#include "DnWeapon.h"

#ifdef PRE_ADD_MARK_PROJECTILE
enum eProjectileMarkType
{
	PROJECTILE_MARK_DECAL = 0,
	PROJECTILE_MARK_FX,
	PROJECTILE_MARK_PARTICLE,
};
#endif // PRE_ADD_MARK_PROJECTILE


class CDnProjectile;
class CDnWeapon;
class IDnOrbitProcessor;

class CDnProjectile : public CDnWeapon
{
public:
	CDnProjectile( DnActorHandle hActor, bool bProcess = true, bool bIncreaseUniqueID = true );
	virtual ~CDnProjectile();

	// 궤적 타입
	enum OrbitTypeEnum {
		Linear,        // 등속도 SetSpeed
		Acceleration,  // 가속도 SetSpeed, SetResistance
		Homing,		   // 유도 SetSpeed
		TerrainLinear, // 바닥 타고 등속도 SetSpeed
		Projectile,    // 포물선 SetSpeed
		TerrainHoming, // 바닥 타고 등속도 호밍 SetSpeed
		OrbitTypeCount,
	};

	// 소멸시 타입
	enum DestroyOrbitTypeEnum {
		Instantly,	       // 즉시 사라짐
		FallGravity,       // 중력값 받으며 떨어짐
		RangeFallGravity,  // 기본적으로는 FallGravity 와 같으나 사거리 다 되면 바로 Destroy. (클라이언트에서는 Destroy 액션 실행)
		DestroyTypeCount,
	};

	// 발사체가 대미지를 줄 수 없게 되는 타입
	enum ValidTypeEnum {
		WeaponLength = 0x01, // 무기 길이 이상 날아갔을 경우
		Time = 0x02,		 // 일정 시간 이후
		Stick = 0x04,		 // 땅이나 Prop 에 박히는 경우
	};

	// 타겟 타입
	enum TargetTypeEnum
	{
		CrossHair = 0,   // 크로스 헤어
		Direction,	     // Actor 의 View 방향
		TargetPosition,  // 타겟의 위치
		Target,          // 타겟
		DestPosition,	 // 액션툴에서 박아놓은 목적지
		Shooter,		 // 타겟 타입은 Target 으로 지정되며 발사체를 발사한 액터로 됨. 우선 몬스터에서만 사용되기 때문에 서버의 발사체에서 생성되어 패킷으로 클라로 옴. 따라서 클라이언트에서는 패킷 수신부에서만 처리되어있음. #28352
		TargetTypeCount,
	};

	// 가속도/등속도 타입. Orbit 타입이 Projectile 이거나 Homing 인 경우에만 유효.
	enum VelocityTypeEnum
	{
		Constant,	// 디폴트. 등속도로 프로젝타일 날아감.
		Accell,		// 프로젝타일 가속도 먹음.
		VelocityTypeCount,
	};

	
protected:
	IDnOrbitProcessor* m_pOrbitProcessor;

	MatrixEx m_matExWorld;	
	MatrixEx m_OffsetCross;
	EtVector3 m_vPrevPos;
	EtVector3 m_vStartPos;
	LOCAL_TIME m_InvalidLocalTime;
	LOCAL_TIME m_CreateLocalTime;
	
	OrbitTypeEnum m_OrbitType;
	DestroyOrbitTypeEnum m_DestroyOrbitType;
	TargetTypeEnum m_TargetType; ValidTypeEnum m_ValidType;

	CDnActor::SHitParam m_HitParam;

	bool m_bValidDamage;
	int m_nStateCondition;
	float m_fThickness;

	bool m_bStick;
	float m_fGravity;

	float m_fSpeed;
	float m_fResistance;
	int m_nValidTime;
	int m_nDestroyOrbitTimeGap;
	bool m_bFirstProcess;

	HitStruct m_HitStruct;

	EtLineTrailHandle m_hLineTrail;
	EtVector3 m_vTrailOffset;

	EtLineHandle m_hLine;
	float m_fLineEffectLifeTime;

	DnActorHandle m_hTargetActor;
	EtVector3 m_vTargetPosition;

	bool m_bOnCollisionCalled;
	bool m_bPierce;

	VelocityTypeEnum m_VelocityType;
	float m_fElapsedTime;

	int m_nMaxHitCount;

	bool m_bDirectionFollowView;

	bool m_bHasHitAction;
	bool m_bHasHitSignalInHitAction;
	bool m_bHitFXUseWorldAxis;
	int m_iShootActionIndex;
	int m_iHitActionIndex;
	int m_iDestroyActionIndex;

	bool m_bHasDestroyAction;

	// 생성 정보의 포인터를 물고 있는다.
	const ProjectileStruct* m_pProjectileSignal;

#ifndef PRE_MOD_PROJECTILE_HACK
	DWORD m_dwParentProjectileID;
#endif

	DnPropHandle m_hShooterProp;

	float m_fProjectileOrbitRotateZDegree;

	// 패킷 달라고 할 때 까지 객체는 생성 안됨.
	char* m_pPacketBuffer;
	CPacketCompressStream* m_pPacketStream;

#ifndef PRE_MOD_PROJECTILE_HACK
	int m_iSignalArrayIndex;
#endif

	DnActorHandle m_hChainShooter;			// 체인 라이트닝 효과를 전달해주기 위해 발사체를 쏘는 액터.

	// hit 된 대상을 따라가는 기능. (#22666)
	bool m_bTraceHitTarget;
	bool m_bTraceHitActorHittable;
	DnActorHandle m_hTraceActor;

	char m_cShooterType;
	DWORD m_dwShooterUniqueID;
	INT64 m_nShooterSerialID;
	int m_nShooterActionIndex;
	int m_nShooterSignalIndex;

protected:

	void ProcessOrbit( LOCAL_TIME LocalTime, float fDelta );
	void ProcessValid( LOCAL_TIME LocalTime, float fDelta );
	void ProcessDamage( LOCAL_TIME LocalTime, float fDelta );
	void ProcessDestroyOrbit( LOCAL_TIME LocalTime, float fDelta );

	void StopParticleFXList();

	DnPropHandle _CheckPropCollision( EtVector3& vCenter, float fLength );

public:
	OrbitTypeEnum GetOrbitType() { return m_OrbitType; }
	DestroyOrbitTypeEnum GetDestroyOrbitType() { return m_DestroyOrbitType; } 
	TargetTypeEnum GetTargetType() { return m_TargetType; } 

	MatrixEx *GetMatEx() { return &m_matExWorld; }
	
	void SetSpeed( float fValue ) { m_fSpeed = fValue; }
	float GetSpeed() { return m_fSpeed; }
	void SetResistance( float fValue ) { m_fResistance = fValue; }
	float GetResistance() { return m_fResistance; }
	void SetValidTime( int nValidTime ) 
	{ 
		m_nValidTime = nValidTime; 
		if( 0 < nValidTime )
			m_ValidType = (ValidTypeEnum)( m_ValidType | Time ); 
	};
	int GetValidTime( void ) { return m_nValidTime; };

	void SetShooterType( DnActorHandle hActor, int nActionIndex, int nSignalIndex );
	void SetShooterType( DnWeaponHandle hWeapon, int nActionIndex, int nSignalIndex );

	EtVector3 *GetTargetPosition() { return &m_vTargetPosition; }
	DnActorHandle GetTargetActor() { return m_hTargetActor; }

	void SetTargetActor( DnActorHandle hActor, bool bUpdateTargetPartsIndex = true );// { m_hTargetActor = hActor; };
	void SetTargetPosition( EtVector3 &vPos ) { m_vTargetPosition = vPos; };

	void SetValidType(ValidTypeEnum ValidType) { m_ValidType = ValidType; }
	void SetDestroyOrbitTime( int nValue ) { m_nDestroyOrbitTimeGap = nValue; }

	bool Initialize( MatrixEx& Offset, OrbitTypeEnum OrbitType, DestroyOrbitTypeEnum DestroyType, TargetTypeEnum TargetType );
	// 정적 멤버 함수인 CreateProjectile... 함수를 통해 생성한 것이 아니라 직접 new 로 생성해서 실제로 발사되어 날아가는 경우,
	// Initialize 함수와 마찬가지로 발사체 생성 후 설정 끝나고 마지막에 이 함수를 꼭 호출해주어야 한다.
	bool PostInitialize( void );

	bool AttachTrail( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment, EtVector3 &vOffset );

	void AttachLine( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fUMoveSpeed );
	void SetChainShooter( DnActorHandle hChainShooter ) { m_hChainShooter = hChainShooter; };

	void OnDamageSuccess( DnActorHandle hDamagedActor, CDnActor::SHitParam &HitParam );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	void OnCollisionWithActor( bool bHitFromServer = false );
	void OnCollisionWithProp( void );
	void OnCollisionWithGround( void );

	void OnChangedNextActionToHit( void );


	void SetPierce( bool bPierce ) { m_bPierce = bPierce; };
	bool GetPierce( void ) { return m_bPierce; };

	void SetVelocityType( VelocityTypeEnum VelocityType ) { m_VelocityType = VelocityType; };
	VelocityTypeEnum GetVelocityType( void ) { return m_VelocityType; };

	void SetMaxHitCount( int iMaxHitCount ) { m_nMaxHitCount = iMaxHitCount; };
	int GetMaxHitCount( void ) { return m_nMaxHitCount; };

	void SetInvalid( LOCAL_TIME InvalidTime ) { m_InvalidLocalTime = InvalidTime; m_bValidDamage = false; }

	void SetDirectionFollowView( bool bDirectionFollowView ) { m_bDirectionFollowView = bDirectionFollowView; }

	MatrixEx *GetObjectCross() override;
	static void NonScaledMatrixValidCheck( EtMatrix &EtMatrix );

	void SetProjectileSignal( const ProjectileStruct* pProjectileStruct ) { m_pProjectileSignal = pProjectileStruct; };
	const ProjectileStruct* GetProjectileSignal( void ) const { return m_pProjectileSignal; };

	void SetHitFXUseWorldAxis( bool bHitFXUseWorldAxis ) { m_bHitFXUseWorldAxis = bHitFXUseWorldAxis; }

	void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);

#ifndef PRE_MOD_PROJECTILE_HACK
	void SetParentProjectileID( DWORD dwProjectileID ) { m_dwParentProjectileID = dwProjectileID; };
	DWORD GetParentProjectileID( void ) { return m_dwParentProjectileID; };
#endif

	void SetShooterProp( DnPropHandle hProp ) { m_hShooterProp = hProp; }

	void SetProjectileOrbitRotateZ( float fProjectileOrbitRotateZ ) { m_fProjectileOrbitRotateZDegree = fProjectileOrbitRotateZ; };
	float GetProjectileOrbitRotateZ( void ) { return m_fProjectileOrbitRotateZDegree; };

#ifndef PRE_MOD_PROJECTILE_HACK
	// CDnLocalPlayerActor 에서 호출되는 SignalArray 값 셋팅하는 인덱스.
	void SetSignalArrayIndex( int iSignalArrayIndex ) { m_iSignalArrayIndex = iSignalArrayIndex; };
#endif

	void SetTraceHitTarget( bool bTraceHitTarget, bool bTraceHitActorHittable ) { m_bTraceHitTarget = bTraceHitTarget; m_bTraceHitActorHittable = bTraceHitActorHittable; };
	bool IsTraceHitTarget( void ) { return m_bTraceHitTarget; };
	bool IsTraceHitActorHittable( void ) { return m_bTraceHitActorHittable; };

	// 발사체 생성, 발사체 객체로부터 전송 패킷 만들기.. 등등..
	// 발사체 시그널에 내용이 추가되면 같이 수정해주어야 합니다.
	static CDnProjectile *CreateProjectile( DnActorHandle hShooter, MatrixEx &LocalCross, ProjectileStruct *pStruct, 
											EtVector3 vForceTargetPos=EtVector3(0.f,0.f,0.f), DnActorHandle hForceTargetActor=CDnActor::Identity(), const EtVector3* pForceDir = NULL, bool bCheckLocalUser = true );
	static CDnProjectile* CreateProjectileFromServerPacket( DnActorHandle hShooter, const BYTE* pPacket );
	CPacketCompressStream* GetPacketStream( void );

	virtual float GetTraceHitProjectileActorAdditionalHeight( void );
	virtual void SetDestroy( void );
	void DoDestroyAction( void );

protected:
	int m_nTargetPartsIndex;		//Parts몬스터일경우 선택된 parts인덱스
	int m_nTargetPartsBoneIndex;	//선택된 Parts에서 실제 Bone정보를 가진 Index..(MonsterParts를 Vector로 가지고, 다시 _Info가 _PartsInfo를 vector로 가짐)
public:
	void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; }
	int GetTargetPartsIndex() { return m_nTargetPartsIndex; }
	int GetTargetPartsBoneIndex() { return m_nTargetPartsBoneIndex; }

	void UpdateTargetPartsIndex();

protected:
	bool m_bDoNotDestroyEffect;	//hit로 바뀔때 이펙트를 제거 할지 여부
public:
	void SetDoNotDestroyEffect(bool bDoNotDestroy) { m_bDoNotDestroyEffect = bDoNotDestroy; }
	bool IsDoNotDestroyEffect() { return m_bDoNotDestroyEffect; }

#if defined(PRE_FIX_52329)
protected:
	int m_nIgnoreHitType;

public:
	void SetIgnoreHitType(int nType) { m_nIgnoreHitType = nType; }
	int GetIgnoreHitType() { return m_nIgnoreHitType; }
#endif // PRE_FIX_52329

#if defined(PRE_ADD_55295)
protected:
	bool m_bHitActionVectorInit;

public:
	//발사체의 방향은 유지한체 Y벡터 초기화 시킨다.
	void ChangeProjectileRotation();
	
	void SetHitActionVectorInit(bool bHitActionVectorInit) { m_bHitActionVectorInit = bHitActionVectorInit; }
	bool IsHitActionVectorInit() { return m_bHitActionVectorInit; }
#endif // PRE_ADD_55295
};


// 궤적을 결정하는데 필요한 정보들.
struct S_PROJECTILE_PROPERTY
{
	CDnProjectile::OrbitTypeEnum eOrbitType;
	CDnProjectile::TargetTypeEnum eTargetType;
	CDnProjectile::VelocityTypeEnum eVelocityType;
	float fSpeed;
	float fResistance;
	float fProjectileOrbitRotateZ;

	S_PROJECTILE_PROPERTY( void ) : eOrbitType( CDnProjectile::OrbitTypeCount ),
									eTargetType( CDnProjectile::TargetTypeCount ), 
									eVelocityType( CDnProjectile::VelocityTypeCount ),
									fSpeed( 0.0f ),
									fResistance( 0.0f ),
									fProjectileOrbitRotateZ( 0.0f ) {}
};