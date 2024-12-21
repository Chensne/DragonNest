#pragma once
#include "dnblow.h"

class CDnVarianceDamgeBlow : public CDnBlow, public TBoostMemoryPool< CDnVarianceDamgeBlow >
{
public:
	CDnVarianceDamgeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnVarianceDamgeBlow(void);

	enum VarianceType
	{
		NONE = 0,
		DIVIDE_DAMAGE,
		ZERO_DAMAGE,
		INC_DAMAGE
	};

private:
	int m_MaxDamage;		//데미지
	int m_nLimitMaxCount;	//히트 제한 인원수?
	int m_nApplyType;		//데미지 변화 종류(분배/0/증가)
	
	int m_nHitActorCount;	//실제 hit된 actor 수

#if defined(_GAMESERVER)
	void ApplyDamage(int nHitCount);
protected:
	void OnSetParentSkillInfo();
#endif // _GAMESERVER

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
