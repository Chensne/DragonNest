#pragma once
#include "DnBlow.h"
#ifdef _GAMESERVER
#include "DnActorStatIntervalManipulator.h"
#endif

//지속 HP 증가 비율
//효과시간 동안 2초마다 <효과수치*최대HP>만큼의 현재HP가 증가한다.
//최대HP이상으로는 변화하지 않는다.
class CDnRtContinueHPIncBlow : public CDnBlow, public TBoostMemoryPool< CDnRtContinueHPIncBlow >
{
private:
#ifdef _GAMESERVER
	CDnActorStatIntervalManipulator m_IntervalChecker;
#endif

	bool m_bShowValue;

public:
	CDnRtContinueHPIncBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRtContinueHPIncBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool OnCustomIntervalProcess( void );
#endif

protected:
	virtual void OnSetParentSkillInfo();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
