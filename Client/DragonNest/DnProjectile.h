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

	// ���� Ÿ��
	enum OrbitTypeEnum {
		Linear,        // ��ӵ� SetSpeed
		Acceleration,  // ���ӵ� SetSpeed, SetResistance
		Homing,		   // ���� SetSpeed
		TerrainLinear, // �ٴ� Ÿ�� ��ӵ� SetSpeed
		Projectile,    // ������ SetSpeed
		TerrainHoming, // �ٴ� Ÿ�� ��ӵ� ȣ�� SetSpeed
		OrbitTypeCount,
	};

	// �Ҹ�� Ÿ��
	enum DestroyOrbitTypeEnum {
		Instantly,	       // ��� �����
		FallGravity,       // �߷°� ������ ������
		RangeFallGravity,  // �⺻�����δ� FallGravity �� ������ ��Ÿ� �� �Ǹ� �ٷ� Destroy. (Ŭ���̾�Ʈ������ Destroy �׼� ����)
		DestroyTypeCount,
	};

	// �߻�ü�� ������� �� �� ���� �Ǵ� Ÿ��
	enum ValidTypeEnum {
		WeaponLength = 0x01, // ���� ���� �̻� ���ư��� ���
		Time = 0x02,		 // ���� �ð� ����
		Stick = 0x04,		 // ���̳� Prop �� ������ ���
	};

	// Ÿ�� Ÿ��
	enum TargetTypeEnum
	{
		CrossHair = 0,   // ũ�ν� ���
		Direction,	     // Actor �� View ����
		TargetPosition,  // Ÿ���� ��ġ
		Target,          // Ÿ��
		DestPosition,	 // �׼������� �ھƳ��� ������
		Shooter,		 // Ÿ�� Ÿ���� Target ���� �����Ǹ� �߻�ü�� �߻��� ���ͷ� ��. �켱 ���Ϳ����� ���Ǳ� ������ ������ �߻�ü���� �����Ǿ� ��Ŷ���� Ŭ��� ��. ���� Ŭ���̾�Ʈ������ ��Ŷ ���źο����� ó���Ǿ�����. #28352
		TargetTypeCount,
	};

	// ���ӵ�/��ӵ� Ÿ��. Orbit Ÿ���� Projectile �̰ų� Homing �� ��쿡�� ��ȿ.
	enum VelocityTypeEnum
	{
		Constant,	// ����Ʈ. ��ӵ��� ������Ÿ�� ���ư�.
		Accell,		// ������Ÿ�� ���ӵ� ����.
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

	// ���� ������ �����͸� ���� �ִ´�.
	const ProjectileStruct* m_pProjectileSignal;

#ifndef PRE_MOD_PROJECTILE_HACK
	DWORD m_dwParentProjectileID;
#endif

	DnPropHandle m_hShooterProp;

	float m_fProjectileOrbitRotateZDegree;

	// ��Ŷ �޶�� �� �� ���� ��ü�� ���� �ȵ�.
	char* m_pPacketBuffer;
	CPacketCompressStream* m_pPacketStream;

#ifndef PRE_MOD_PROJECTILE_HACK
	int m_iSignalArrayIndex;
#endif

	DnActorHandle m_hChainShooter;			// ü�� ����Ʈ�� ȿ���� �������ֱ� ���� �߻�ü�� ��� ����.

	// hit �� ����� ���󰡴� ���. (#22666)
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
	// ���� ��� �Լ��� CreateProjectile... �Լ��� ���� ������ ���� �ƴ϶� ���� new �� �����ؼ� ������ �߻�Ǿ� ���ư��� ���,
	// Initialize �Լ��� ���������� �߻�ü ���� �� ���� ������ �������� �� �Լ��� �� ȣ�����־�� �Ѵ�.
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
	// CDnLocalPlayerActor ���� ȣ��Ǵ� SignalArray �� �����ϴ� �ε���.
	void SetSignalArrayIndex( int iSignalArrayIndex ) { m_iSignalArrayIndex = iSignalArrayIndex; };
#endif

	void SetTraceHitTarget( bool bTraceHitTarget, bool bTraceHitActorHittable ) { m_bTraceHitTarget = bTraceHitTarget; m_bTraceHitActorHittable = bTraceHitActorHittable; };
	bool IsTraceHitTarget( void ) { return m_bTraceHitTarget; };
	bool IsTraceHitActorHittable( void ) { return m_bTraceHitActorHittable; };

	// �߻�ü ����, �߻�ü ��ü�κ��� ���� ��Ŷ �����.. ���..
	// �߻�ü �ñ׳ο� ������ �߰��Ǹ� ���� �������־�� �մϴ�.
	static CDnProjectile *CreateProjectile( DnActorHandle hShooter, MatrixEx &LocalCross, ProjectileStruct *pStruct, 
											EtVector3 vForceTargetPos=EtVector3(0.f,0.f,0.f), DnActorHandle hForceTargetActor=CDnActor::Identity(), const EtVector3* pForceDir = NULL, bool bCheckLocalUser = true );
	static CDnProjectile* CreateProjectileFromServerPacket( DnActorHandle hShooter, const BYTE* pPacket );
	CPacketCompressStream* GetPacketStream( void );

	virtual float GetTraceHitProjectileActorAdditionalHeight( void );
	virtual void SetDestroy( void );
	void DoDestroyAction( void );

protected:
	int m_nTargetPartsIndex;		//Parts�����ϰ�� ���õ� parts�ε���
	int m_nTargetPartsBoneIndex;	//���õ� Parts���� ���� Bone������ ���� Index..(MonsterParts�� Vector�� ������, �ٽ� _Info�� _PartsInfo�� vector�� ����)
public:
	void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; }
	int GetTargetPartsIndex() { return m_nTargetPartsIndex; }
	int GetTargetPartsBoneIndex() { return m_nTargetPartsBoneIndex; }

	void UpdateTargetPartsIndex();

protected:
	bool m_bDoNotDestroyEffect;	//hit�� �ٲ� ����Ʈ�� ���� ���� ����
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
	//�߻�ü�� ������ ������ü Y���� �ʱ�ȭ ��Ų��.
	void ChangeProjectileRotation();
	
	void SetHitActionVectorInit(bool bHitActionVectorInit) { m_bHitActionVectorInit = bHitActionVectorInit; }
	bool IsHitActionVectorInit() { return m_bHitActionVectorInit; }
#endif // PRE_ADD_55295
};


// ������ �����ϴµ� �ʿ��� ������.
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