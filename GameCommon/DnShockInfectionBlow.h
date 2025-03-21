#pragma once
#include "dnblow.h"

#ifndef _GAMESERVER
class CDnLocalPlayerActor;
#endif

class CDnShockInfectionBlow : public CDnBlow, public TBoostMemoryPool< CDnShockInfectionBlow >
{
protected:
	float m_fShockProb;				//번개화살 발사 확률
	float m_fLimitRadius;		//번개화살을 발사할 액터 선택 범위
	int m_nShockDurationTime;		//감전 효과 지속 시간
	float m_fHitApplyPercent;	//발사체 hitPercent..

	ProjectileStruct m_ProjectileSignalInfo;
	//EtVector3 m_OffSet;
	//EtVector3 m_Direction;
	//EtVector3 m_DestPos;

	//int m_nOrbitType;
	//int m_nTargetType;
	//int m_VelocityType;
	//float m_fProjectileSpeed;
	//int m_nProjectileValidTime;

#ifdef _GAMESERVER
	CDnSkill::StateEffectStruct m_AddtionalSE;	//감전 상태효과 정보
	DWORD m_dwCoolTime;
	LOCAL_TIME m_LastActivateTime;				// 마지막으로 감전 전이가 되었던 시간.
#else
	CDnLocalPlayerActor* m_pLocalPlayerActor;
	DnSkillHandle m_hParentSkillForCoolTime;
#endif // #ifdef _GAMESERVER

	//bool m_bCreateProjectile;

#if defined(_GAMESERVER)
	void CreateProjectileOnServer( DnActorHandle hTargetActor );
#else
	void CreateProjectileOnClient();
#endif // _GAMESERVER

public:
	CDnShockInfectionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnShockInfectionBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	//void SetCreateProjectile(bool bCreateProjectile) { m_bCreateProjectile = bCreateProjectile; }

#if defined(_GAMESERVER)
	void DoShockInfection( DnActorHandle hHitTargetActor );
	virtual void OnTargetHit( DnActorHandle hTargetActor );
#else
	void DoShockInfection( DnActorHandle hTargetActor, DnActorHandle hActorToInfection );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
