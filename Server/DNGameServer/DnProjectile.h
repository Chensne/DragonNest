#pragma once

#include "DnUnknownRenderObject.h"
#include "EtMatrixEx.h"
#include "DnActionBase.h"
#include "DnActor.h"
#include "Timer.h"
#include "DnWeapon.h"
#include "DnCalcCatmullrom.h"
#include "DnSkill.h"

class CDnProjectile;
class CDnWeapon;
class IDnOrbitProcessor;

class CDnProjectile : public CDnWeapon, public TBoostMemoryPool<CDnProjectile>
{
public:
	CDnProjectile( CMultiRoom *pRoom, DnActorHandle hActor, bool bProcess = true, bool bIncreaseUniqueID = true );
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
		CrossHair = 0,  // ũ�ν� ���
		Direction,	    // Actor �� View ����
		TargetPosition, // Ÿ���� ��ġ
		Target,         // Ÿ��
		DestPosition,   // �׼������� �ھƳ��� ������
		Shooter,		// Ÿ�� Ÿ���� Target ���� �����Ǹ� �߻�ü�� �߻��� ���ͷ� ��. �켱 ���Ϳ����� ����ϱ� ������ �ű������ �۾���.
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

	static int s_iHitUniqueID;

	int m_iHitUniqueID;

	DnActorHandle m_hShooter;
	MatrixEx m_Cross;
	MatrixEx m_OffsetCross;
	EtVector3 m_vPrevPos;
	EtVector3 m_vStartPos;
	EtVector3 m_vHitPos;
	LOCAL_TIME m_InvalidLocalTime;
	LOCAL_TIME m_CreateLocalTime;
	
	OrbitTypeEnum m_OrbitType;
	DestroyOrbitTypeEnum m_DestroyOrbitType;
	TargetTypeEnum m_TargetType; 
	ValidTypeEnum m_ValidType;

	CDnDamageBase::SHitParam m_HitParam;

	bool m_bValidDamage;
	bool m_bFallGravity;

	bool m_bStick;
	float m_fGravity;

	float m_fSpeed;
	float m_fResistance;
	int m_nValidTime;
	int m_nDestroyOrbitTimeGap;
	bool m_bFirstProcess;

	DnSkillHandle m_hParentSkill;

	std::vector<CDnSkill::StateEffectStruct> m_VecStateEffectList;
	std::vector<CDnSkill::StateEffectStruct> m_ComboLimitStateEffectList;

	HitStruct m_HitStruct;
	DnActorHandle m_hTargetActor;
	EtVector3 m_vTargetPosition;

	CDnCalcCatmullRom m_CatmullRom;
	CDnHoming m_Homing;

	bool m_bOnCollisionCalled;

	bool m_bPierce;

	LOCAL_TIME			m_LastHitSignalEndTime;

	// ������ ������Ÿ���� ��� �� �� hit �� �༮���� ���� �������� ���� �ʵ��� �Ѵ�..
	vector<DnActorHandle> m_VecHittedActor;

	CDnSkill::SkillInfo m_ParentSkillInfo;

	bool m_bHasHitAction;		// hit �׼�(����)�� ���� �ִ� ������Ÿ���� 
	bool m_bHasHitSignalInHitAction;	// Hit �׼ǿ��� Hit �ñ׳��� ���� �ִ°�.
	bool m_bHasHitSignalInShootAction;	// Shoot �׼ǿ��� Hit �ñ׳��� ���� �ִ°�.
	bool m_bHitSignalProcessed;
	bool m_bBombHitSignalStarted;	// 2009.7.29 �ѱ� - ���߽� ��Ʈ �ñ׳��� �ڿ� �ִ� ��찡 �����Ƿ� �ڿ� �ִ� ���̽��� �־ ��Ʈ �ñ׳� ���� �ÿ� �÷��� ����.
	bool m_bHitActionStarted;
	int m_iShootActionIndex;
	int m_iHitActionIndex;
	int m_iDestroyActionIndex;
	
	bool m_bHasDestroyAction;

	VelocityTypeEnum m_VelocityType;

	LOCAL_TIME m_nLastHitSignalIndex;
	
	float m_fElapsedTime;
	int m_nMaxHitCount;		// ������(bPierce == true) �� ��쿡�� ��ȿ�� �ִ� ��Ʈ ��.
	map<CDnActor*, int> m_mapMaxHitCount;

	// ���� ������ �����͸� ���� �ִ´�.
	int m_iShooterShootActionIndex;	// �� ������Ÿ���� �߻��� �׼� �ε���
	int m_iSignalArrayIndex;	// �� ������Ÿ�� �ñ׳��� array index
	boost::shared_ptr<ProjectileStruct> m_pProjectileSignal;
	boost::shared_ptr<CDnState> m_pShooterState;
#ifdef PRE_ADD_PROJECTILE_SE_INFO
	boost::shared_ptr<CDnStateBlow> m_pShooterStateBlow;
#endif

	// ü�� ���� ����ȿ�������� ���� ���� ����ȿ�� �ɷ��� �� ������Ÿ���� �� ����
	DnActorHandle m_hPrevAttacker;
	// ü�� ���� ����ȿ������ ���� �ִ� ��ų ���̺� ���ǵǾ��ִ� hit prob ��.
	float m_fHitApplyPercent;
	// 

	// ���� �����ϴ� �߻�ü�� ���� hit �ñ׳��� ����ϰ� �ִ���. (ex) summon blackhole
	bool m_bProcessingBombHitSignal;

	// �������� �� �߻�ü��� ����ص״ٰ� �浹 üũ���� ����.
	DnPropHandle m_hShooterProp;

	bool m_bFromCharger;

	float m_fProjectileOrbitRotateZDegree;

	// ��Ŷ �޶�� �� �� ���� ��Ŷ ��Ʈ�� ��ü�� ���� �ȵ�.
	char* m_pPacketBuffer;
	CPacketCompressStream* m_pPacketStream;

	// hit �� ����� ���󰡴� ���. (#22666)
	bool m_bTraceHitTarget;
	bool m_bTraceHitActorHittable;
	DnActorHandle m_hTraceActor;
	//////////////////////////////////////////////////////////////////////////

	// �ܺο��� ������ �������� ����. ���� �������� ����. (������ �߻�ü ��Ŷ������ ����..)
	EtVector3 m_vForceDir;
	bool m_bUseForceDir;

	char m_cShooterType;
	DWORD m_dwShooterUniqueID;
	INT64 m_nShooterSerialID;
	int m_nShooterActionIndex;
	int m_nShooterSignalIndex;

	ElementEnum m_eForceHitElement;

	// #41382 ���� ��ȯ �߻�ü�� ��� 167�� ����ȿ���� ��ȯ�� ������ ��ų���� ���� ���� ���� �ִٸ� �����Ѵ�.
	// �̹� �߻�ü�� ���� ������ ��ȯ�Ǵ� ���������� 167�� ����ȿ���� ���ŵ� ��Ȳ�� �� �ִ�. 
	// (�̵��ϰų� �°ų��ϼ� ��ų �׼��� ����Ǿ� �ڱ� �ڽſ��� �ο��� ����ȿ���� ���ŵ� ���)
	int m_iSummonMonsterForceSkillLevel;

protected:
	void MakeHitParam( CDnDamageBase::SHitParam &HitParam );

	void ProcessOrbit( LOCAL_TIME LocalTime, float fDelta );
	void ProcessValid( LOCAL_TIME LocalTime, float fDelta );
	bool ProcessDamage( LOCAL_TIME LocalTime, float fDelta, 
						LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, 
						int nSignalIndex, BOOL bUseHitSignalArea );

	DnPropHandle _CheckPropCollision( EtVector3& vCenter, float fLength, float& fPropContactDistance, float& fPropContactTime );
	void ProcessDestroyOrbit( LOCAL_TIME LocalTime, float fDelta );

	void CheckAndApplyDamage( DnActorHandle hActor, bool bValidDamageGoOn = true, int* nHitCount = NULL );

	bool SetTraceActor( DnActorHandle hActor, bool bValidDamageGoOn );

public:
	OrbitTypeEnum GetOrbitType() { return m_OrbitType; }
	DestroyOrbitTypeEnum GetDestroyOrbitType() { return m_DestroyOrbitType; } 
	TargetTypeEnum GetTargetType() { return m_TargetType; } 

	MatrixEx *GetMatEx() { return &m_Cross; }
	
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

	EtVector3 *GetTargetPosition() { return &m_vTargetPosition; }
	DnActorHandle GetTargetActor() { return m_hTargetActor; }

	void SetTargetActor( DnActorHandle hActor, bool bUpdateTargetPartsIndex = true );// { m_hTargetActor = hActor; };
	void SetTargetPosition( EtVector3 &vPos ) { m_vTargetPosition = vPos; }

	void SetValidType(ValidTypeEnum ValidType) { m_ValidType = ValidType; }
	void SetDestroyOrbitTime( int nValue ) { m_nDestroyOrbitTimeGap = nValue; }

	bool Initialize( MatrixEx &Offset, OrbitTypeEnum OrbitType, DestroyOrbitTypeEnum DestroyType, TargetTypeEnum TargetType );
	// ���� ��� �Լ��� CreateProjectile... �Լ��� ���� ������ ���� �ƴ϶� ���� new �� �����ؼ� ������ �߻�Ǿ� ���ư��� ���,
	// Initialize �Լ��� ���������� �߻�ü ���� �� ���� ������ �������� �� �Լ��� �� ȣ�����־�� �Ѵ�.
	bool PostInitialize( void );		

	void OnDamageSuccess( DnActorHandle hDamagedActor, CDnDamageBase::SHitParam &HitParam );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	void SetParentSkill( DnSkillHandle hParentSkill ) 
	{ 
		m_hParentSkill = hParentSkill;
		if (hParentSkill)
			m_ParentSkillInfo = *(hParentSkill->GetInfo());
		m_ParentSkillInfo.hSkillUser = m_hShooter; 
	};
	void SetParentSkillInfo( CDnSkill::SkillInfo& SkillInfo ) 
	{ 
		m_ParentSkillInfo = SkillInfo; 
		m_ParentSkillInfo.hSkillUser = m_hShooter; 
		m_iShooterShootActionIndex = m_ParentSkillInfo.iProjectileShootActionIndex;
		m_iSignalArrayIndex = m_ParentSkillInfo.iProjectileSignalArrayIndex;
		m_hPrevAttacker = m_ParentSkillInfo.hPrevAttacker;
	}

	void AddStateEffect( CDnSkill::StateEffectStruct &Struct );

	void OnCollisionWithActor( void );
	void OnCollisionWithGround( void );
	void OnCollisionWithProp( void );
	void OnChangedNextActionToHit( void );			// �׼� ������ shoot �׼ǿ��� next �׼��� hit �� �����س��Ƽ� �ڵ����� �����ϴ� �߻�ü�� ��쿣 �׼��� �ٲ� �������� ó���� �͵� ó�����ش�.

	void SetPierce( bool bPierce ) { m_bPierce = bPierce; };
	bool GetPierce( void ) { return m_bPierce; };

	void FromSkill( bool bFromSkill ) { m_HitParam.bFromProjectileSkill = bFromSkill; };
	bool IsFromSkill() { return m_HitParam.bFromProjectileSkill; }

	void SetVelocityType( VelocityTypeEnum VelocityType ) { m_VelocityType = VelocityType; };
	VelocityTypeEnum GetVelocityType( void ) { return m_VelocityType; };

	void SetMaxHitCount( int iMaxHitCount ) { m_nMaxHitCount = iMaxHitCount; };
	int GetMaxHitCount( void ) { return m_nMaxHitCount; };

	virtual void OnChangeAction( const char *szPrevAction );
	virtual void OnFinishAction( const char* szPrevAction, LOCAL_TIME time );

	void SetProjectileSignal( boost::shared_ptr<ProjectileStruct>& pProjectileStruct ) { m_pProjectileSignal = std::move(pProjectileStruct); };
	boost::shared_ptr<ProjectileStruct>& GetProjectileSignal( void ) { return m_pProjectileSignal; };
	
	void SetShootActionIndex( int iActionIndex ) { m_iShooterShootActionIndex = iActionIndex; };
	void SetSignalArrayIndex( int iSignalArrayIndex ) { m_iSignalArrayIndex = iSignalArrayIndex; };

	// ��ų�� �߻�Ǵ� ��쿡�� ������ �д�.
	void SetShooterStateSnapshot( boost::shared_ptr<CDnState>& pShooterState ) { m_pShooterState = std::move(pShooterState); };
	boost::shared_ptr<CDnState>& GetShooterStateSnapshot( void ) { return m_pShooterState; };
#ifdef PRE_ADD_PROJECTILE_SE_INFO
	void SetShooterStateBlow( boost::shared_ptr<CDnStateBlow>& pShooterStateBlow ) { 
        m_pShooterStateBlow = std::move(pShooterStateBlow);};
	boost::shared_ptr<CDnStateBlow>& GetShooterStateBlow( void ) { return m_pShooterStateBlow; };
#endif

	void SetHitApplyPercent( float fHitApplyPercent ) { m_fHitApplyPercent = fHitApplyPercent; };
	
	DnActorHandle GetShooterActor( void ) { return m_hShooter; };
	DnSkillHandle GetParentSkill( void ) { return m_hParentSkill; };

	void SetShooterProp( DnPropHandle hProp ) { m_hShooterProp = hProp; };

	void FromCharger( void ) { m_bFromCharger = true; };
	bool IsFromCharger( void ) { return m_bFromCharger; };

	void SetProjectileOrbitRotateZ( float fProjectileOrbitRotateZ ) { m_fProjectileOrbitRotateZDegree = fProjectileOrbitRotateZ; };
	float GetProjectileOrbitRotateZ( void ) { return m_fProjectileOrbitRotateZDegree; };

	void SetTraceHitTarget( bool bTraceHitTarget, bool bTraceHitActorHittable ) { m_bTraceHitTarget = bTraceHitTarget; m_bTraceHitActorHittable = bTraceHitActorHittable; };
	bool IsTraceHitTarget( void ) { return m_bTraceHitTarget; };
	bool IsTraceHitActorHittable( void ) { return m_bTraceHitActorHittable; };

	void SetForceDir( const EtVector3& vForceDir ) { m_vForceDir = vForceDir; m_bUseForceDir = true; };
	bool IsUsedForceDir( void ) { return m_bUseForceDir; };
	const EtVector3& GetForceDir( void ) { return m_vForceDir; };

	void SetShooterType( DnActorHandle hActor, int nActionIndex, int nSignalIndex );
	void SetShooterType( DnWeaponHandle hWeapon, int nActionIndex, int nSignalIndex );
	// �߻�ü ����, �߻�ü ��ü�κ��� ���� ��Ŷ �����.. ���..
	// �߻�ü �ñ׳ο� ������ �߰��Ǹ� ���� �������־�� �մϴ�.
	static CDnProjectile *CreateProjectile( CMultiRoom *pRoom, DnActorHandle hShooter, MatrixEx &LocalCross, ProjectileStruct *pStruct, EtVector3* pForceTargetPos=NULL, 
											DnActorHandle hForceTarget=CDnActor::Identity(), EtVector3* pForceDir = NULL );
	static CDnProjectile* CreatePlayerProjectileFromClientPacket( DnActorHandle hShooter, const BYTE* pPacket );
	CPacketCompressStream* GetPacketStream( void );

	const std::vector<CDnSkill::StateEffectStruct>& GetStateEffectList() { return m_VecStateEffectList; }
	void ApplyParentProjectile(CDnProjectile* pParentProjectile);

protected:
	int m_nTargetPartsIndex;		//Parts�����ϰ�� ���õ� parts�ε���
	int m_nTargetPartsBoneIndex;	//���õ� Parts���� ���� Bone������ ���� Index..(MonsterParts�� Vector�� ������, �ٽ� _Info�� _PartsInfo�� vector�� ����)
public:
	void SetTargetPartsIndex(int nPartsIndex, int nBoneIndex) { m_nTargetPartsIndex = nPartsIndex; m_nTargetPartsBoneIndex = nBoneIndex; }
	int GetTargetPartsIndex() { return m_nTargetPartsIndex; }
	int GetTargetPartsBoneIndex() { return m_nTargetPartsBoneIndex; }

	void UpdateTargetPartsIndex();

	void SetForceHitElement( ElementEnum eElement ) { m_eForceHitElement = eElement; };

private:
	// ����ȿ�� �������� Ȯ���Ѵ�.
	void GetChainAttackInfo(const CDnSkill::StateEffectStruct& stateEffectStruct, float& fRange, int& nMaxCount);
	//���� ��븦 �˻�
	DnActorHandle FindNextChainActor(int iRootAttackerTeam, DnActorHandle hActor, DnActorHandle hPrevActor, float fRange);
	void CreateChainAttackProjectile(DnActorHandle hRootAttacker, DnActorHandle hActor, DnActorHandle hActorToAttack, ProjectileStruct* pProjectileSignalInfo, CDnSkill::SkillInfo& parentSkillInfo);

public:
	void SetSummonMonsterForceSkillLevel( int iLevel ) { m_iSummonMonsterForceSkillLevel = iLevel; };

#if defined(PRE_FIX_52329)
protected:
	int m_nIgnoreHitType;	//

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

//////////////////////////////////////////////////////////////////////////
//#56880
//��Ʈ �� ���� ���� �߻�ü ��ų�� ���� �ð��� ���� �ϵ��� �Ѵ�.
protected:
	LOCAL_TIME m_SkillStartTime;

public:
	void SetSkillStartTime(LOCAL_TIME startTime) { m_SkillStartTime = startTime; }
	LOCAL_TIME GetSkillStartTime() { return m_SkillStartTime; }
//////////////////////////////////////////////////////////////////////////

#if defined(PRE_FIX_59238)
//��������� ���� ���Ͱ� ���ÿ� ��Ʈ ���� �ʵ��� �ϱ� ���� �۾�
//�߻�ü���� ��Ʈ �� ���͸� ����Ʈ�� ��� ���´�.
protected:
	std::map<DWORD, DnActorHandle> m_HittedActorList;

public:
	void AddHittedActor(DnActorHandle hHittedActor);
	bool IsHittable(DnActorHandle hActor);
#endif // PRE_FIX_59238

#if defined(PRE_FIX_59336)
	void ApplyComboLimitStateEffect( DnActorHandle hActor );
#endif // PRE_FIX_59336

	void ApplySkillStateEffect(DnActorHandle hActor);

#if defined(PRE_FIX_65287)
	//��ų���� 50�� ����ȿ���� ��ų ������ �ڽſ��� ���� �� ���� ���¿��� �߻�ü�� �߻�ǰ�,
	//�߻�ü���� �Ǵ� �ð��� ��ų ���� �ð� ���� ��� �� ���, ��ų �������� ����ȿ���� �������,
	//�߻�ü�� ������ ��꿡 �� �̻� ������ �� �ִ� ��찡 �߻��ȴ�.
	//�߻�ü �����ؼ� �߻�ü �߻� ������ ���� ������ ���� �ϴ� ��������, 50�� ����ȿ���� �ִ� ���
	//����ȿ�� �������� �ջ��ؼ� �߻�ü�� ���� �� ����, �߻�ü Hitó�� �� �� ������ �̿��ϵ��� �Ѵ�.
protected:
	float m_fShooterFinalDamageRate;

public:
	void SetShooterFinalDamageRate(float fRate) { m_fShooterFinalDamageRate = fRate; }
	float GetShooterFinalDamageRate() { return m_fShooterFinalDamageRate; }

#endif // PRE_FIX_65287
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