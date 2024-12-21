#pragma once

#include "DnBlow.h"


class CDnIgnoreRestoreHPBlow : public CDnBlow, public TBoostMemoryPool< CDnIgnoreRestoreHPBlow >
{
public:
	CDnIgnoreRestoreHPBlow( DnActorHandle hActor, const char *szValue );
	virtual ~CDnIgnoreRestoreHPBlow();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual bool CanBegin( void );

	int GetAllowSkillID() { return m_nAllowSkillID; }

protected:
	int m_nAllowSkillID;
};

