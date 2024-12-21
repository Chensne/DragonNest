#pragma once
#include "dnblow.h"


// 105
// ���� �и� ���� ���� ����
class CDnConsumeBubbleBlow : public CDnBlow,
	public TBoostMemoryPool< CDnConsumeBubbleBlow >
{
private:
	float m_nTime;
	float m_nProcessTime;
	int m_nBubbleID;
public:
	CDnConsumeBubbleBlow(DnActorHandle hActor, const char* szValue);
	virtual ~CDnConsumeBubbleBlow(void);
	
	virtual void OnBegin(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnEnd(LOCAL_TIME LocalTime, float fDelta);

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
