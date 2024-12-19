#pragma once
#include "dnblow.h"

// 감전 상태효과 폭발 상태효과
class CDnElectricExplosionBlow : public CDnBlow, 
								 public TBoostMemoryPool< CDnElectricExplosionBlow >
{
private:
#ifdef _GAMESERVER
	//int m_iSkillUserAttackMMin;
	//int m_iSkillUserAttackMMax;

	int m_iOriAttackMMin;
	int m_iOriAttackMMax;
#endif

	CEtActionBase::ActionElementStruct* m_pActionElement;
	float m_fFrame;

	DWORD m_ActionDurationTime;
	vector<CEtActionSignal *> m_vlpSignalList;

public:
	CDnElectricExplosionBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnElectricExplosionBlow(void);

	virtual bool CanBegin( void );

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void SignalProcess( LOCAL_TIME LocalTime, float fDelta );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
