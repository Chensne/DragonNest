#pragma once
#include "dnblow.h"

class CDnAdditionalStateInfoBlow : public CDnBlow
{
public:
	

private:
	STATE_BLOW::emBLOW_INDEX m_DestStateIndex;	//���� ����ȿ��
	CDnSkill::StateEffectStruct m_TargetStateInfo;	//�߰��� ����ȿ��

public:
	CDnAdditionalStateInfoBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnAdditionalStateInfoBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	void SetInfo(const char* szValue);

	STATE_BLOW::emBLOW_INDEX GetDestStateIndex() { return m_DestStateIndex; }
	CDnSkill::StateEffectStruct GetTargetStateInfo() { return m_TargetStateInfo; }

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
