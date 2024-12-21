#pragma once
#include "dnblow.h"


// Rotha
// 화상 상태에 걸린 상황에서 , 화염공격을 맞게되면 크리티컬 확률을 증가시켜줌 < 비율증가및 절대비율 증가 >.

class CDnBurnCriticalBlow : public CDnBlow, public TBoostMemoryPool< CDnBurnCriticalBlow >
{
public:
	CDnBurnCriticalBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnBurnCriticalBlow(void);
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	void SetBlowInfo();

	int m_nMultiplyRatio;   // 기존의 확률의 곱.
	int m_nAddValueRatio; // 크리값 절대치를 더해줌

	int GetMultiplyRatio(){return m_nMultiplyRatio;}
	int GetAddValueRatio(){return m_nAddValueRatio;}
#endif

protected:

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
