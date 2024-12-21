#pragma once
#include "dnblow.h"
#include "DnObservable.h"



class CDnFreezingShieldBlow : public CDnBlow, public TBoostMemoryPool< CDnFreezingShieldBlow >
{
private:
	int m_nDuration; //내구도
	float m_fRate;			//마법공격력 비율
	float m_fLimitRadius;	//공격 반경
	int m_nFreezingTime;	//공격(결빙)지속 시간

	std::string m_strStateAttribute;	//결빙 설정값.

	void SetInfo(const char* szValue);

public:
	CDnFreezingShieldBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFreezingShieldBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
