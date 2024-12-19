
#pragma once

class CDnPlayerActor;

namespace HackPenanty
{
	struct Common
	{
		enum
		{
			MaxRate				= 10000,
			SpeedHack			= 500,
			SpeedHackAllowCount	= 3,
			PosHack				= 100,
			PosHackAllowCount	= 5,
		};
	};
};

class IDnPlayerChecker
{
public:
	IDnPlayerChecker( CDnPlayerActor* pActor ):m_pActor(pActor){}
	virtual ~IDnPlayerChecker(){}

	virtual bool OnSyncDatumGap( const DWORD dwGap ) { return true; }
	virtual bool OnSyncPosition( const EtVector3& vPos ){ return true; }
	virtual bool OnSyncMoveSpeed( const int nMoveSpeed ){ return true; }
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}

	virtual bool IsInvalidPlayer() { return false; }
	virtual void ResetInvalid() {}
protected:

	CDnPlayerActor* m_pActor;
};
