#pragma once

#include "DnPlayerChecker.h"

class CDnPlayerPickupChecker :public IDnPlayerChecker, public TBoostMemoryPool<CDnPlayerPickupChecker>
{
public:
	CDnPlayerPickupChecker( CDnPlayerActor *pPlayer );
	virtual ~CDnPlayerPickupChecker();

protected:
	int m_nInvalidCount;

public:
	bool IsInvalidPlayer();
	void OnPickupDist( EtVector3 &vPos, DnDropItemHandle hDropItem );
};