#pragma once

#include "DnWorldOperationProp.h"
#include "DnDamageBase.h"
#include "MAActorProp.h"
#include "DnHitPropComponent.h"


class CDnWorldOperationDamageProp : public CDnWorldOperationProp, 
									public CDnDamageBase, 
									public MAActorProp, 
									public TBoostMemoryPool< CDnWorldOperationDamageProp >
{
public:
	CDnWorldOperationDamageProp( CMultiRoom *pRoom );
	virtual ~CDnWorldOperationDamageProp();

protected:
	virtual bool InitializeTable( int nTableID );

protected:

	CDnHitPropComponent m_HitComponent;

public:
	virtual bool Initialize( CEtWorldSector *pParentSector, const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale );

	// CDnActionBase
	virtual void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, LOCAL_TIME SignalEndTime, int nSignalIndex );

	virtual DWORD GetDamageObjectUniqueID() { return GetUniqueID(); }

	// from CDnDamageBase
	virtual DnActorHandle GetActorHandle( void ) { return m_hMonster; };
};