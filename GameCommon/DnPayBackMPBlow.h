#pragma once
#include "dnblow.h"

//효과 Index : 68
//
//수치 Type : float
//
//설정 : MP 소모 스킬 사용 시 <효과 수치1>에 설정된 확률로 <효과 수치2>*<사용한 MP> 만큼 마나를 충전 or 소비 합니다.
//
//	 사용 방식 : 
//- 버프 또는 패시브 스킬로 일정 확률로 사용된 스킬의 MP 소모량을 줄여주거나
//- 디버프를 걸어 상대의 MP 소비량을 늘릴 수 있습니다.(예상)
class CDnPayBackMPBlow : public CDnBlow, public TBoostMemoryPool< CDnPayBackMPBlow >
{
private:
	float m_fApplyProb;
	float m_fConsumeDecreaseProb;

#ifndef _GAMESERVER
	bool m_bShowActivateEffect;
	LOCAL_TIME m_ShowActivateEffectTime;
	LOCAL_TIME m_PrevFrameLocalTime;
#endif

public:
	CDnPayBackMPBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPayBackMPBlow();

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#ifdef _GAMESERVER
	virtual int OnUseMP( int iMPDelta );
#else
	void OnActivate( void );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
