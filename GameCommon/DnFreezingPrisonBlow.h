#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnCantMoveBlow;
class CDnCantActionBlow;
class CDnFrameStopBlow;

//����
//Ÿ�� : �ƹ��� �ൿ�� ������ ���Ѵ�.
//���� : �ֺ��� �ִ� ���� �������� Ÿ�ٿ� �پ��ִ� ������ �����Ͽ� ������ �������� 0 ���Ϸ� ����� ����� ���°� �����ȴ�.
//���ӽð� : ����� ���ӽð��� 0�� �ʰ��ϰ� �Ǵ� ���� ����� ���´� ���������� ���ÿ� Ÿ�ٿ��� ���̳� �������� ������.
//
//	   1. ������ : ������ HP�� �Է�
//
//	   2. ���̳ε����� : ����/���� ���ݷ��� �� %�� �������� ���� ������ ���Է� �ʿ�
//	   ��) P30
//	   M50
//	   10000(���밪���� ������)
//	   3. �ʴ� ������ : ����� ������ Ÿ�ٿ��� �������� 1�ʴ� ���ӽð����� ������. �Է� ����� ���� ����
//
//	   4. ���ӽð� : ���� ���ӽð��� ����
//	   5. ����Ʈ : ����� ���¿� ���� ������ ����ִ� ���¸� ǥ��
//	   ���̳� �������� ���� ���� ���ظ� ������ ǥ������ �� 2������ �ʿ�
class CDnFreezingPrisonBlow : public CDnBlow, 
							  public TBoostMemoryPool< CDnFreezingPrisonBlow >
{
private:
	CDnActorStatIntervalManipulator m_IntervalChecker;

	CDnCantMoveBlow*		m_pCantMoveBlow;
	CDnCantActionBlow*		m_pCantActionBlow;
	CDnFrameStopBlow*		m_pFrameStopBlow;

	float m_fIntervalDamage;
	int m_iDurability;				// ������. ��� �����Ǹ� ����ȿ���� Ǯ���� �ȴ�.
	bool m_bDurabilityDestroyed;	// �������� �� �Ǿ� �ı��Ǿ���.
	int m_iFinalDamage;				// �ı� �Ǳ� ���� ���ӽð��� �� �Ǹ� ������ �������� �ش�.
	int m_iOriginalTeam;			// �� ����ȿ���� �ߵ����� ������ ���� ��.

	float m_fOrigWight;				// ĳ������ �⺻ Weight��
	int m_iOrigPressLevel;			// ĳ������ �⺻ PressLevel��
	int m_iMaxDurability;			// ������ �ۼ�Ʈ�� ����� ���� �ʱ� ������ ��

protected:
#ifdef _GAMESERVER
	void _RequestDamage( float fDamage );
#endif

public:
	CDnFreezingPrisonBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnFreezingPrisonBlow(void);

	virtual bool OnCustomIntervalProcess( void );

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if !defined(_GAMESERVER)
	void OnHit();
	EtVector3 GetGaugePos();
#endif

#ifdef _GAMESERVER
	virtual bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
