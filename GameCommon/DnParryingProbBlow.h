#pragma once
#include "dnblow.h"


//ȿ�� Index : 56
//
//��ġ Type : float
//
//���� : <ȿ�� �ð�>���� <ȿ�� ��ġ>��ŭ �и� Ȯ�� ����
//
//	 ��� ��� : Ư�� ���ٽ��� �����̳� ���� �Ǵ� �нú� ��ų�� ���� �и� Ȯ�� ���


class CDnParryingProbBlow : public CDnBlow, public TBoostMemoryPool< CDnParryingProbBlow >
{
private:
	

public:
	CDnParryingProbBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnParryingProbBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
