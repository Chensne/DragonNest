#pragma once
#include "dnblow.h"
#include "DnObservable.h"



// 정해진 횟수만큼 무조건 가드하는 상태효과
class CDnBlockBlow : public CDnBlow,
					 public TBoostMemoryPool< CDnBlockBlow >,
					 // CDnPlayerActor 쪽에서 이 클래스로 static_cast 하여 Observer 객체를 넣는 코드가 있으므로 이 상속 코드만 제거되면 큰일남.
					 public CDnObservable 
					 //////////////////////////////////////////////////////////////////////////
{
private:
	int					m_iCanBlockCount;

public:
	CDnBlockBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnBlockBlow(void);

	void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	bool OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess );
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
public:
	static void AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
	static void RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
};
