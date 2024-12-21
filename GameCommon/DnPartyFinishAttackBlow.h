#pragma once
#include "dnblow.h"


class CDnPartyFinishAttackBlow : public CDnBlow, public TBoostMemoryPool< CDnPartyFinishAttackBlow >
{
protected:

#if defined(_GAMESERVER)
	float m_fEnemyHpRatio;
	float m_fIncreaseDamagdRatio;
	float m_fDamageRatio;
	float m_fDamageAbsolute;
	float m_fDamageRatioMax;
#endif

public:
	CDnPartyFinishAttackBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnPartyFinishAttackBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#if defined(_GAMESERVER)
	
	virtual void OnTargetHit( DnActorHandle hTargetActor );

protected:
	virtual void OnSetParentSkillInfo();

#endif



};
