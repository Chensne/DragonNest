#pragma once

#include "DnMonsterActor.h"
#include "DnPartsMonsterActor.h"

class CDnBeholderMonsterActor : public CDnPartsMonsterActor
{
public:
	CDnBeholderMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnBeholderMonsterActor();

protected:
	int   m_nEyeBoneIndex;
	DnActorHandle	m_DummyBeholderEye;
public:
	virtual bool Initialize();
	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	
	// MAActorRenderBase
	virtual void PreProcess( LOCAL_TIME LocalTime, float fDelta );
};