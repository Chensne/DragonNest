#pragma once
#include "dnblow.h"

//꼭두각시 상태효과...
class CDnPuppetBlow : public CDnBlow, 
								 public TBoostMemoryPool< CDnPuppetBlow >
{
public:
	CDnPuppetBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPuppetBlow(void);

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
#endif

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

protected:
#if defined(_GAMESERVER)
	EtVector3 m_vDeltaPos;
	DnMonsterActorHandle m_hSummonMonster;
#ifdef PRE_ADD_DECREASE_EFFECT
	bool m_bShowReduce;
public:
	void SetShowReduce( bool bShowReduce ) { m_bShowReduce = bShowReduce; }
#endif // PRE_ADD_DECREASE_EFFECT
#endif // _GAMESERVER
	std::string m_DamageChangeRateArg;	//데미지 비율 증가 수치(상태효과 설정 값.물리(134)/마법(135) 둘다 적용)
	float m_fLimitRange;				//소환액터와 꼭두각시 소환몬스터와의 제한 거리(벗어나면 상태효과 제거 됨)

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
