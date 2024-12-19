#pragma once

#include "DnMonsterActor.h"
#include "DnWorldProp.h"


// hit prop common module
class CDnHitPropComponent
{
private:
	DnMonsterActorHandle m_hMonster;
	const MatrixEx* m_pPropMatrixEx;
	DnPropHandle m_hProp;
	CDnDamageBase* m_pDamageBase;

	LOCAL_TIME m_LastHitSignalTime;
	int m_nLastHitSignalIndex;

public:
	CDnHitPropComponent(void);
	virtual ~CDnHitPropComponent(void);

	void Initialize( DnMonsterActorHandle hMonsterHandle, const MatrixEx* pPropMatrixEx, DnPropHandle hProp, CDnDamageBase* pDamageBase ) 
	{ 
		m_hMonster = hMonsterHandle;
		m_pPropMatrixEx = pPropMatrixEx;
		m_hProp = hProp;
		m_pDamageBase = pDamageBase;
	};

#ifdef PRE_ADD_HIT_PROP_ADD_SE
	void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
				   LOCAL_TIME SignalEndTime, int nSignalIndex, DNVector(DnActorHandle)* pvlhHittedActor = NULL );
#else
	void OnSignal( SignalTypeEnum Type, void *pPtr, LOCAL_TIME LocalTime, LOCAL_TIME SignalStartTime, 
				   LOCAL_TIME SignalEndTime, int nSignalIndex );
#endif // #ifdef PRE_ADD_HIT_PROP_ADD_SE
};
