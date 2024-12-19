#pragma once


#include "DnMonsterActor.h"
#include "DnPartsMonsterActor.h"

class CDnCelberosMonsterActor : public CDnPartsMonsterActor
{
public:
	CDnCelberosMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnCelberosMonsterActor();

protected:
	enum StateBuf
	{
		IceBuf = 0,
		FireBuf,
		DarkBuf,
		MaxBufCount
	};

	bool m_bBufDieArray[MaxBufCount];

public:
	virtual bool Initialize();

	// Actor Message
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDrop( float fCurVelocity );

	virtual void OnFinishAction(const char* szPrevAction, const char *szNextAction, LOCAL_TIME time);
};