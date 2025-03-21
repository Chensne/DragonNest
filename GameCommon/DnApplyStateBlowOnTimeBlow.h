#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnApplyStateBlowOnTimeBlow : public CDnBlow, public TBoostMemoryPool< CDnApplyStateBlowOnTimeBlow >
{
public:
	CDnApplyStateBlowOnTimeBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnApplyStateBlowOnTimeBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;

	DnSkillHandle m_hSkill;	//적용할 상태효과의 스킬 핸들

	bool OnCustomIntervalProcess( void );
	void ApplyStateEffect();
#endif // _GAMESERVER

protected:
	int m_nTime;	//시간 간격 (1000 => 1초)
	float m_fRate;	//확률
	int m_nSkillID;	//적용할 상태효과의 스킬ID
	int m_nSkillLevel;	//적용할 상태효과의 스킬 레벨

	void SetInfo(const char* szValue);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);

#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
