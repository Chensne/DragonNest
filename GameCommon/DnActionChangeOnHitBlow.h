#pragma once
#include "dnblow.h"

// 179) 이 상태효과가 들어간 후 최초로 대상에 hit 되면 상태효과로 정의된 액션으로 전이.
// 허리케인 댄스(해당 스킬의 첫 액션에 부여된 공격이 명중시 자동으로 다음 액션으로 전환합니다.)
class CDnActionChangeOnHitBlow : public CDnBlow, 
								 public TBoostMemoryPool< CDnActionChangeOnHitBlow >
{
private:
	string m_strActionNameToChange;
#if defined(PRE_FIX_55461)
	string m_strLimitAction;	//OnTargetHit시 해당 액터의 동작이 같은 경우에만 발동 되도록 제한을 둠
#endif // PRE_FIX_55461


public:
	CDnActionChangeOnHitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnActionChangeOnHitBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual void OnTargetHit( DnActorHandle hTargetActor );
#endif // #ifdef _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
