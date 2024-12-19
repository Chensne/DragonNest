#pragma once
#include "dnblow.h"


//하이랜더(HighLander)
//- 스킬 효과 ID : 143
//- 수치 단위 : float
//- 설명 
//1)적용될 경우 <효과 시간1>동안 “하이랜더” 버프 효과를 얻습니다.
//2)버프가 적용되어있는 동안 해당 버프를 지닌 플레이어의 HP가 0 또는 0이하로 내려갈 시 “하이랜더” 버프를 지우고 HP를 1로 강제 조정 후 <효과 시간2>동안 1 미만으로 HP가 내려가지 않습니다.

class CDnHighlanderBlow : public CDnBlow, 
						  public TBoostMemoryPool< CDnHighlanderBlow >
{
private:
	enum
	{
		NORMAL,
		HIGHLANDER,
	};

	int m_iState;
	

public:
	CDnHighlanderBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnHighlanderBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	float CalcDamage( float fOriginalDamage );
	float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
