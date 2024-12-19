#pragma once

#include "DnBlow.h"


class CDnIgnoreGravitySignalBlow : public CDnBlow, public TBoostMemoryPool< CDnIgnoreGravitySignalBlow >
{
public:
	CDnIgnoreGravitySignalBlow( DnActorHandle hActor, const char *szValue );
	virtual ~CDnIgnoreGravitySignalBlow();

public:
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnBegin( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnEnd( LOCAL_TIME LocalTime, float fDelta );
	virtual bool CanBegin( void );
};
