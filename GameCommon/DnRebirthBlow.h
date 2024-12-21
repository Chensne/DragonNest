#pragma once
#include "DnBlow.h"

class CDnRebirthBlow : public CDnBlow, public TBoostMemoryPool< CDnRebirthBlow >
{
public:
	CDnRebirthBlow(DnActorHandle hActor, const char *szValue);
	virtual ~CDnRebirthBlow(void);

protected:
	bool m_bImmortal;
	int m_nCount;
	float m_fElapseTime;
private:

	UINT	m_uiRespawnHPPercent;
	UINT	m_uiRespawnMPPercent;
	UINT	m_uiRespawnImmortalTimeSec;

	enum eRebirthType
	{
		eNormal,
		eStageClear,
	};
	eRebirthType m_Type;

protected:
	void SetRebirthValue();
	void SetRebirthState();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnDefenseAttack( DnActorHandle hHitter,CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
#if !defined _GAMESERVER
	void DoRebirthAction();
#endif // !defined _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
