#pragma once
#include "dnblow.h"

class CDnDarkTransformationBlow : public CDnBlow, public TBoostMemoryPool< CDnDarkTransformationBlow >
{

protected:
	int m_nTransformActorID;
	int m_nOriginalActorID;
	int m_nWeapon[2];
	int m_nUnk;

	DnWeaponHandle m_hBackupWeapon[2];

	DnActorHandle m_hCurrentActor;

#if defined(_GAMESERVER)
	int m_nTransformSkillID;
	bool m_bOrderUseSkill;
#endif

public:
	CDnDarkTransformationBlow(DnActorHandle hActor, const char* szValue);
	virtual ~CDnDarkTransformationBlow(void);
	virtual void OnBegin(LOCAL_TIME LocalTime, float fDelta);
	virtual void Process(LOCAL_TIME LocalTime, float fDelta);
	virtual void OnEnd(LOCAL_TIME LocalTime, float fDelta);

};
