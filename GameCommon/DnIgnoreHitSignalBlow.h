#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnIgnoreHitSignalBlow : public CDnBlow, public TBoostMemoryPool< CDnIgnoreHitSignalBlow >
{
public:
	CDnIgnoreHitSignalBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnIgnoreHitSignalBlow(void);

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual bool CanBegin( void );

#if defined(_CLIENT)
protected:
	CDnActorStatIntervalManipulator	m_IntervalChecker;
	bool m_bGraphicEffectShow;
	bool m_bTrigger;

public:
	virtual bool OnCustomIntervalProcess( void );
	void ShowGraphicEffect(bool bShow);
#endif // _CLIENT

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
