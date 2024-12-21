#pragma once

#include "DnNormalMonsterActor.h"
class CDnDefenseMonsterActor : public CDnNormalMonsterActor
{
public:
	CDnDefenseMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnDefenseMonsterActor();

protected:
	bool m_bCrashDefense;
	EtColor m_OrignalShieldColor;

protected:
	virtual void DefenseWeapon( bool bCrash );
	void ProcessShieldState( DnWeaponHandle hWeapon, LOCAL_TIME LocalTime );

public:
	// Actor Message
	virtual void OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	// ActionBase
//	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	virtual void OnDamageReadAdditionalPacket( CPacketCompressStream *pStream );
};