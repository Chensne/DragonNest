#pragma once
#include "DnDefenseMonsterActor.h"

class CDnLamiaMonsterActor : public CDnDefenseMonsterActor
{
public:
	CDnLamiaMonsterActor( int nClassID, bool bProcess = true );
	virtual ~CDnLamiaMonsterActor();

protected:
	DnWeaponHandle m_hSubWeapon[2];

protected:
	virtual void DefenseWeapon( bool bCrash );

public:
	virtual void AttachWeapon( DnWeaponHandle hWeapon, int nEquipIndex = 0, bool bDelete = false );
	virtual void DetachWeapon( int nEquipIndex = 0 );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual void OnDamageReadAdditionalPacket( CPacketCompressStream *pStream );
};