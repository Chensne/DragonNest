#pragma once
#include "dnblow.h"


// Ŭ����(��ũ)�� ����Ʈ ��ο쿡���� ���̴� �� ������ ����ȿ��
class CDnClericMentalChargeBlow : public CDnBlow, public TBoostMemoryPool< CDnClericMentalChargeBlow >
{
private:
	

public:
	CDnClericMentalChargeBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnClericMentalChargeBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
