#pragma once

#include "DnWeapon.h"
class CDnDamageBase
{
public:
	enum DamageObjectTypeEnum {
		Unknown = 0,
		Actor,
		Prop,
	};

	enum DistanceTypeEnum
	{
		Melee = 0,
		Range,
	};

#ifdef _GAMESERVER
	struct SHitParam {
		std::string szActionName;
		LOCAL_TIME RemainTime;
		float fDamage;
		float fDurability;
		float fStiffProb;
		EtVector3 vVelocity;
		EtVector3 vResistance;
		DnActorHandle hHitter;
		DnWeaponHandle hWeapon;
		bool bFirstHit;
		EtVector3 vPosition;
		EtVector3 vViewVec;
		std::vector<int>	vBoneIndex;
		int nDamageType;
		CDnWeapon::HitTypeEnum HitType;
		int nSkillSuperAmmorIndex;
		int nSkillSuperAmmorDamage;
		int nSuperAmmorDelay;
		CDnState::ElementEnum HasElement;
		int iElementAttack;
		char cAttackType;
		bool bSuccessNormalDamage;
		DistanceTypeEnum	DistanceType;
		bool bIgnoreCanHit;
		bool bIgnoreParring;
		bool bFromProjectileSkill;		// #23818 스킬로 발사된 발사체의 hit 시그널로 비롯된 hitparam 인지 구분. 공격 아이템에 접두어 스킬이 붙어있을 경우 "평타 공격시" 라는 조건이 붙으므로..

		// 발사체로 쏜 것은 발사체의 객체에 발사체를 쏜 시점의 shooter 의 state 를 복사해서 갖고 있다가 
		// 데미지 계산할 때 그걸로 계산한다.
		bool bFromProjectile;
		
		// 일반 히트 시에는 -1 디폴트 값으로써, 건드릴 필요가 없으나
		// Projectile 에서 생성하여 넣어줌. IsHittable 함수에서 EndSignalTime 을
		// 구분하기 전에 UniqueID 가 같은지 우선 체크해서 다르면 무시한다.
		int iUniqueID;

		INT64 nCalcDamage; //OnTargetHit시에 맞는 액터의 데미지를 알기 위해 저장.
		int nHitLimitCount;	//Hit수 제한
#if defined( PRE_ADD_LOTUSGOLEM )
		bool bIgnoreShowDamage;	
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_MONSTER_CATCH
		bool bReleaseCatchActor;
#endif // #ifdef PRE_ADD_MONSTER_CATCH
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
		bool bStageLimit;
#endif

		int nWeightValueLimit;	// 아카데믹 포스아웃 용 [2011/07/04 semozz]
		bool bBreakSuperAmmor;	// 브레이킹 포인트(슈퍼아머브레이크)에 의해 크리티컬이 발동시 활성화.(크리티컬 저항 계산이 안되도록하기 위해)

		SHitParam() 
		: vVelocity( 0.f, 0.f, 0.f ) 
		, vResistance( 0.f, 0.f, 0.f )
		, vPosition( 0.f, 0.f, 0.f )
		, vViewVec( 0.f, 0.f, 0.f )
		, iUniqueID( -1 )
		{
			fDamage = 0.f;
			fDurability = 0.f;
			fStiffProb = 0.f;
			RemainTime = 0;
			hWeapon.Identity();
			bFirstHit = true;
			vBoneIndex.clear();
			HitType = CDnWeapon::HitTypeEnum::Normal;
			nDamageType = 0;
			nSkillSuperAmmorIndex = -1;
			nSkillSuperAmmorDamage = 0;
			HasElement = CDnState::ElementEnum_Amount;
			iElementAttack = 0;
			cAttackType = 0;
			bSuccessNormalDamage = true;
			nSuperAmmorDelay = 0;
			DistanceType	= DistanceTypeEnum::Melee;
			//nProjectileAttackMin = 0;
			//nProjectileAttackMax = 0;
			bFromProjectile = false;
			bIgnoreParring = false;
			bIgnoreCanHit = false;
			bFromProjectileSkill = false;

			nCalcDamage = 0;
			nHitLimitCount = 0;
#if defined( PRE_ADD_LOTUSGOLEM )
			bIgnoreShowDamage = false;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_MONSTER_CATCH
			bReleaseCatchActor = false;
#endif // #ifdef PRE_ADD_MONSTER_CATCH
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
			bStageLimit = false;
#endif

			nWeightValueLimit = 0;
			bBreakSuperAmmor = false;
		}
	};
#else
	struct SHitParam {
		std::string szActionName;
		DnWeaponHandle hWeapon;
		INT64 nDamage;
		CDnWeapon::HitTypeEnum HitType;
		EtVector3 vPosition;
		EtVector3 vVelocity;
		EtVector3 vResistance;
		EtVector3 vViewVec;
		float fStiffDelta;
		float fDownDelta;
		bool bFirstHit;
		bool bSuccessNormalDamage;
		bool bRecvOnDamage;
		INT64 nCurrentHP;
		DistanceTypeEnum	DistanceType;
		int nHitLimitCount;

		CDnState::ElementEnum HasElement;
#if defined( PRE_ADD_LOTUSGOLEM )
		bool bIgnoreShowDamage;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
		bool bStageLimit;
#endif
#ifdef PRE_FIX_77172
		bool bKeepJumpMovement;
#endif // PRE_FIX_77172

		// #32722 클라이언트에서 스킬 사용한다는 패킷이 도착하기 전에 서버에서 피격될 경우 클라이언트만 스킬 액션이 캔슬되는 것 수정.
		int iCanceledSkillIDInServer;

		SHitParam()
			: vVelocity( 0.f, 0.f, 0.f )
			, vResistance( 0.f, 0.f, 0.f )
			, vPosition( 0.f, 0.f, 0.f )
			, vViewVec( 0.f, 0.f, 0.f )
		{
			nDamage					= 0;
			HitType					= CDnWeapon::Normal;
			fStiffDelta				= 0.f;
			fDownDelta				= 0.f;
			bFirstHit				= false;
			bSuccessNormalDamage	= true;
			bRecvOnDamage			= true;
			DistanceType = Melee;
			nCurrentHP = 0;
			nHitLimitCount = 0;

			HasElement = CDnState::ElementEnum_Amount;
#if defined( PRE_ADD_LOTUSGOLEM )
			bIgnoreShowDamage = false;
#endif // #if defined( PRE_ADD_LOTUSGOLEM )
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
			bStageLimit = false;
#endif
#ifdef PRE_FIX_77172
			bKeepJumpMovement = false;
#endif // PRE_FIX_77172
			iCanceledSkillIDInServer = 0;
		}
	};
#endif


	CDnDamageBase() { m_DamageObjectType = Unknown; }
	CDnDamageBase( DamageObjectTypeEnum Type ) { m_DamageObjectType = Type; }
	virtual ~CDnDamageBase() {}

protected:
	DamageObjectTypeEnum m_DamageObjectType;

public:
	DamageObjectTypeEnum GetDamageObjectType() { return m_DamageObjectType; }
	void SetDamageObjectType( DamageObjectTypeEnum Type ) { m_DamageObjectType = Type; }

	virtual void			OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam ) {}
	virtual DWORD			GetDamageObjectUniqueID() { return -1; }
	virtual DnActorHandle	GetActorHandle( void ) = 0;
	virtual void			OnSetPartsHP( DnActorHandle hActor, const int iPartsTableID, const int iCurHP , const int iHitterID){}

};
