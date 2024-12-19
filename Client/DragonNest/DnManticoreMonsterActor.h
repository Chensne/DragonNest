#pragma once


#include "DnMonsterActor.h"
#include "DnPartsMonsterActor.h"

class CDnManticoreMonsterActor : public CDnPartsMonsterActor
{
public:
	CDnManticoreMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnManticoreMonsterActor();

protected:
	LOCAL_TIME m_nIgnoreNormalSuperArmorTime;
public:
	virtual bool Initialize();

	
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDrop( float fCurVelocity );	
};