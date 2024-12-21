#pragma once
#include "dnblow.h"

class CDnChangeMAttackDamageBlow : public CDnBlow,
								   public TBoostMemoryPool< CDnChangeMAttackDamageBlow >
{
private:

public:
	CDnChangeMAttackDamageBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeMAttackDamageBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	// 마법 데미지 변경 상태효과가 중복되었을 경우 값이 1.2 형식으로 들어가 있기 때문에
	// 두번째부터는 1을 빼줘서 더해줘야한다.
	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
