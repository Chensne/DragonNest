#pragma once
#include "dnblow.h"


// ��� ���Խÿ� ���� ��������..
// ������ ���Ұ� ����ȿ�� ����.
class CDnCantUseItemBlow : public CDnBlow, public TBoostMemoryPool< CDnCantUseItemBlow >
{
private:
	

public:
	CDnCantUseItemBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCantUseItemBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
