#pragma once
#include "dnworldactprop.h"
#include "MAActorProp.h"


class CDnShooterProp : public CDnWorldActProp,
					   public MAActorProp,
					   public CDnDamageBase
{
private:

protected:
	bool InitializeTable( int nTableID );

public:
	CDnShooterProp(void);
	virtual ~CDnShooterProp(void);

	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );
};
