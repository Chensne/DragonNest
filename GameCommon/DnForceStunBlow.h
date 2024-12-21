#pragma once
#include "dnblow.h"

// �̽� #12200
// ����Ȯ������ �����Ѱ�ġ(�����ǰ�� 35%)������ [��������]��ų�� ��ųȿ���� �����Ҽ��� �����ϴ�.(�׷��� ���� �ִ� ���Ͻ�ų�� �� ����;;;)
// �׼������� ������������ �������ڴ� ���������� ��쿡�� ������ ���Ͽ� �ɸ��ԵǾ� �ʹ� ������ų�� �˴ϴ�. ���� �Ʒ��� ���� ó���ؾ��Ұ� �����ϴ�.
// �������� ����Ȯ�� �ɷ�ġ�� ������� ȿ����ġ�� ������ Ȯ���� ����� [����]���·� ����� ��ųȿ���� �߰��ؾ��մϴ�.
class CDnForceStunBlow : public CDnBlow,
						 public TBoostMemoryPool< CDnForceStunBlow >
{
private:

public:
	CDnForceStunBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnForceStunBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
