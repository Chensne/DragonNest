#pragma once
#include "dnblow.h"


// #71659 [����Ʈ �巡��] ��ųȿ�� - �ִ����� ���� ȿ�� ����
class CDnDamageLimitBlow : public CDnBlow, public TBoostMemoryPool< CDnDamageLimitBlow >
{
protected:

#ifdef _GAMESERVER
	bool m_bOrderUseSkill;
	int m_nSkillIndex;
	float m_fDamageLimit;
	float m_fSumDamage;
	LOCAL_TIME m_tSkillUseDelay;
#endif

public:
	CDnDamageLimitBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDamageLimitBlow(void);

	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );

#ifdef _GAMESERVER
	virtual bool CanBegin( void );
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif

};
