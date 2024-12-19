#pragma once

#include "DnNormalMonsterActor.h"
class CDnDefenseMonsterActor : public CDnNormalMonsterActor
{
public:
	CDnDefenseMonsterActor( CMultiRoom *pRoom, int nClassID );
	virtual ~CDnDefenseMonsterActor();

protected:
	int m_nDefenseProb;
	bool m_bCrashDefense;

protected:
	virtual void DefenseWeapon( bool bCrash );

public:
	// Actor Message
	virtual bool CheckDamageAction( DnActorHandle hActor );

	// ActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	virtual void OnDamageWriteAdditionalPacket( CPacketCompressStream *pStream );
};

class IBoostPoolDnDefenseMonsterActor : public CDnDefenseMonsterActor, public TBoostMemoryPool< IBoostPoolDnDefenseMonsterActor >
{
public:
	IBoostPoolDnDefenseMonsterActor( CMultiRoom *pRoom, int nClassID ):CDnDefenseMonsterActor( pRoom, nClassID ){}
	virtual ~IBoostPoolDnDefenseMonsterActor(){}
};
