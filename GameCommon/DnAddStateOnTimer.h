#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnAddStateOnTimerBlow : public CDnBlow, public TBoostMemoryPool< CDnAddStateOnTimerBlow >
{
private:
	float m_fRate;				//확률
	int m_nDestStateBlowIndex;	//조건이 맞을때 추가될 상태효과 Index

	std::string m_strStateAttribute;
	int m_nStateDurationTime;

	float m_fRange;		//탐색 범위
	DWORD m_TimerTime;	//시간 간격

	void SetInfo(const char* szValue);
public:
	CDnAddStateOnTimerBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAddStateOnTimerBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool OnCustomIntervalProcess( void );
	
protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
