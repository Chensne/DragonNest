#pragma once
#include "dnblow.h"

//중첩시 추가 액션 상태효과
class CDnPileAddEffectBlow : public CDnBlow, 
								 public TBoostMemoryPool< CDnPileAddEffectBlow >
{
public:
	CDnPileAddEffectBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPileAddEffectBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDuplicate( const STATE_BLOW& StateBlowInfo );
	virtual void OnSetParentSkillInfo();
	virtual void SignalProcess( LOCAL_TIME LocalTime, float fDelta );

	//상태효과가 중첩 카운트 수치에 도달 하면 활성화 여부 설정.
	void SetActivateBlow(bool bActivate) { m_bActivatedBlow = bActivate; }
	bool IsActivatedBlow() { return m_bActivatedBlow; }

private:
	std::string m_szTargetAction;

	CEtActionBase::ActionElementStruct* m_pActionElement;
	float m_fFrame;
	DWORD m_ActionDurationTime;
	vector<CEtActionSignal *> m_vlpSignalList;

	bool m_bActivatedBlow;		//중첩 설정 수치에 도달 했을때 활성화 됨..

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
