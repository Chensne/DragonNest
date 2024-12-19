#pragma once
#include "dnblow.h"

// �̽� #19619
// Ÿ�Ӹ���Ʈ���� ��ųó�� ������ ���� ��Ʈ�ñ׳��� ����ؼ� ����ȿ���� �ο��ϴ� ��ų���� ���
// ����� CanHit ���¿� ���� ����ǰ� �ȵ� ���� �ִ�. 
// �� ����ȿ���� �ش� ����ȿ���� �ɷ��ִ� ��ü�� CDnActor::IsHittable() �Լ����� CanHit ���¸� 
// �����ϰ� ������ true �� ó���ϵ��� �Ѵ�.
class CDnIgnoreCanHitBlow : public CDnBlow,
							public TBoostMemoryPool< CDnIgnoreCanHitBlow >
{
private:

public:
	CDnIgnoreCanHitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnIgnoreCanHitBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

};
