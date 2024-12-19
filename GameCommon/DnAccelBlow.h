#pragma once
#include "DnBlow.h"
#include "DnActorStatIntervalManipulator.h"

class CDnBasicBlow;

class CDnAccelBlow : public CDnBlow, public TBoostMemoryPool< CDnAccelBlow >
{
private:
	float m_fStartValue;
	float m_fEndValue;
	float m_fIncValue;

	CDnBasicBlow* m_pMoveSpeedBlow;

	CDnActorStatIntervalManipulator	m_IntervalChecker;

	void SetInfo(const char* szValue);

public:
	CDnAccelBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnAccelBlow(void);

public:
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual bool CanBegin( void );
	virtual bool OnCustomIntervalProcess( void );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
