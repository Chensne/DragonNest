#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnAddStateOnHitBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateOnHitBlow >
{
private:
	float m_fRate;				//확률값
	int m_nStateDurationTime;	//추가될 상태효과 지속 시간
	int m_nDestStateBlowIndex;	//조건이 맞을때 추가될 상태효과 Index

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
	// 확률계산이 액터당 되지 않도록 하기위한 처리.
	bool m_bCalcProb;	//확률계산 여부
	bool m_bOnDamaged;	//맞았는지 확인.

	bool CalcProb();
	//////////////////////////////////////////////////////////////////////////
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
