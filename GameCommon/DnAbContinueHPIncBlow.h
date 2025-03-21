#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


// 지속 HP 증가 절대치 - 11번
class CDnAbContinueHPIncBlow : public CDnBlow, public TBoostMemoryPool< CDnAbContinueHPIncBlow >
{
private:
	CDnActorStatIntervalManipulator m_IntervalChecker;

	bool m_bShowValue;

#if defined(PRE_ADD_56253)
	float m_fLimitMinValue;		//HP 최소 제한 수치(이 제한 수치를 벗어 나면 이 상태효과 스킬을 종료 시킨다.)
#endif // PRE_ADD_56253

public:
	CDnAbContinueHPIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAbContinueHPIncBlow(void);

	virtual float GetStatDelta( void ) { return m_fValue; };

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual bool OnCustomIntervalProcess( void );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

protected:
	virtual void OnSetParentSkillInfo();
};
