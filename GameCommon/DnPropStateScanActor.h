#pragma once
#include "DnPropState.h"

// 프랍 주변으로 ScanActor 를 하고 있는 상태
class CDnPropStateScanActor : public CDnPropState, 
							  public TBoostMemoryPool<CDnPropStateScanActor>
{
private:
	float m_fScanRange;	

public:
	CDnPropStateScanActor( DnPropHandle hEntity );
	virtual ~CDnPropStateScanActor(void);

	void SetRange( float fRange ) { m_fScanRange = fRange; };

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );
};
