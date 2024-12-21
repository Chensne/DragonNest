#pragma once
#include "dnblow.h"
#include "DnActorStatIntervalManipulator.h"


class CDnBreakSuperArmorBlow : public CDnBlow, public TBoostMemoryPool< CDnBreakSuperArmorBlow >
{
public:
	CDnBreakSuperArmorBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnBreakSuperArmorBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if !defined(_GAMESERVER)
public:
	virtual bool OnCustomIntervalProcess( void );
	void ShowGraphicEffect(bool bShow);

protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;
	bool m_bGraphicEffectShow;
	bool m_bTrigger;
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
