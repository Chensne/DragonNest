#pragma once
#include "dnblow.h"

// ��ų ��Ÿ�� ���� ����ȿ��
// 96��
class CDnCoolTimeBlow : public CDnBlow, 
						public TBoostMemoryPool< CDnCoolTimeBlow >
{
private:
	

public:
	CDnCoolTimeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnCoolTimeBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
