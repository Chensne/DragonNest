#pragma once
#include "dnblow.h"


//MP사용량 변경(UseMpRatio)
//- 스킬 효과 ID : 139
//- 수치 단위 : float
//- 설명 
//1) 적용될 경우 <효과 시간>동안 해당 버프,디버프를 가진 유저가 스킬 사용 시 MP소비량을 <기존 소비량>*<효과 비율>로 조정합니다.

class CDnChangeMPConsumeRatioBlow : public CDnBlow, 
								    public TBoostMemoryPool< CDnChangeMPConsumeRatioBlow >
{
private:
	

public:
	CDnChangeMPConsumeRatioBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnChangeMPConsumeRatioBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual int OnUseMP( int iMPDelta );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
