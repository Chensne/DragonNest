#pragma once
#include "dnblow.h"


class CDnRecallSummonBlow : public CDnBlow, public TBoostMemoryPool< CDnRecallSummonBlow >
{
public:
	CDnRecallSummonBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnRecallSummonBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
protected:
	void RecallSummon(const std::list<DnMonsterActorHandle>& listSummon, DWORD dwActorClassID, EtVector3* pDestPos);
#endif // _GAMESERVER

protected:
	DWORD m_dwMonsterID;

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
