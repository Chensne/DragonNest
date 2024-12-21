#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnAddStateOnHitBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateOnHitBlow >
{
private:
	float m_fRate;				//Ȯ����
	int m_nStateDurationTime;	//�߰��� ����ȿ�� ���� �ð�
	int m_nDestStateBlowIndex;	//������ ������ �߰��� ����ȿ�� Index

	std::string m_strStateAttribute;
	
	void SetInfo(const char* szValue);
public:
	CDnAddStateOnHitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddStateOnHitBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

#if defined(_GAMESERVER)
protected:
	//////////////////////////////////////////////////////////////////////////
	// Ȯ������� ���ʹ� ���� �ʵ��� �ϱ����� ó��.
	bool m_bCalcProb;	//Ȯ����� ����
	bool m_bOnDamaged;	//�¾Ҵ��� Ȯ��.

	bool CalcProb();
	//////////////////////////////////////////////////////////////////////////
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
