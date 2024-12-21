#pragma once
#include "dnblow.h"

// ����ȭ ����ȿ��
// ȿ�� �ε���: 73
// ȿ�� ��ġ: ��ġ�� ���� ����
// ȿ�� ����:
// ȿ���ð� ���� �������� ����� ����ȭ(���İ� 100%) ��Ų��.
// ȿ���ð��� �����ų� ������ �����ϴ� ȿ���� ������ ����ȭ ȿ���� �������.
// ����ȭ ���¿��� �̵��� Stand ���� �ൿ(����, ��ų���, �����ۻ��)�� �ϸ� �� �׼� ������ ����� �����ϰ� ���̴� ����(���İ� 50%����)�� ���̰� �׼��� ������ �ٽ� ����ȭ�� �ȴ�.
// ����ȭ ���¿��� ������ ���� ��쿡�� ������ ���� ���� ����� �����ϰ� �����ٰ� ��������� ������ �ٽ� ����ȭ�� �ȴ�.
// ����ȭ ���¿��� ���ܷ� [�����]ȿ���� �ɷ��� ��� �ش� ������� ���� [�����̻� ����Ʈ]�� ���������� �ʰ� ��� �����ȴ�. (�׿��� �����̻� ����Ʈ�� ��� ���� ������ ����.)
// ����ȭ ���� �߿��� [�׸���]�� ��µ��� �ʴ´�.
// ����ȭ ���¶� ���� Sound�� ��� ��µȴ�.
// �����ڰ� ������ ��� ����/������� ������Ƿ� ����ȭȿ���� �������.
class CDnInvisibleBlow : public CDnBlow,
						 public TBoostMemoryPool<CDnInvisibleBlow>
{
private:
#ifndef _GAMESERVER
	float m_fDestAlpha;
	float m_fNowAlpha;
	bool m_bEnded;
#endif

public:
	CDnInvisibleBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnInvisibleBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
