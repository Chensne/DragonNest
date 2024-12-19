#pragma once
#include "dnblow.h"

//HP회복 상태효과(공격시)
class CDnRecoverHPByAttackBlow : public CDnBlow, 
								 public TBoostMemoryPool< CDnRecoverHPByAttackBlow >
{
protected:
	INT64 m_nLimitHP;

public:
	CDnRecoverHPByAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRecoverHPByAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	void RemoveStateBlowBySkillID(int nSkillID);
	void FindSkillBlowList(DnActorHandle hActor, int nSkillID, DNVector(DnBlowHandle)& vlBlowList);

	virtual void OnTargetHit( DnActorHandle hTargetActor );
#endif // _GAMESERVER

protected:
	virtual void OnSetParentSkillInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
