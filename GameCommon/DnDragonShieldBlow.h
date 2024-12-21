#pragma once
#include "dnblow.h"

//  #79363 드래곤버프 효과 제작
class CDnDragonShieldBlow : public CDnBlow, public TBoostMemoryPool< CDnDragonShieldBlow >
{
protected:
#if defined( _GAMESERVER )
	float m_fActivateCooltime;

	int m_nRestartCooltime;
	int m_nProbablity;
	std::string m_strEffectName;

	int m_nSubSkillIndex;
	CDnSkill::SkillInfo m_sSubSkillInfo;
	std::vector<CDnSkill::StateEffectStruct> m_vecSubSkillEffectList;
#endif

public:
	CDnDragonShieldBlow( DnActorHandle hActor, const char* szValue );
	virtual ~CDnDragonShieldBlow(void);

#ifdef _GAMESERVER
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual float OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam );
#endif
};
