#pragma once
#include "DnPropState.h"

// 아이템을 떨구는 상태를 표현..
class CDnPropStateItemDrop : public CDnPropState, public TBoostMemoryPool< CDnPropStateItemDrop >
{
private:
	DnActorHandle m_hDropItemOwner;

public:
	CDnPropStateItemDrop( DnPropHandle hEntity );
	virtual ~CDnPropStateItemDrop(void);

	virtual bool OnEntry( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnProcess( LOCAL_TIME LocalTime, float fDelta );
	virtual bool OnLeave( LOCAL_TIME LocalTime, float fDelta );

	void SetDropItemOwner( DnActorHandle hOwner ) { m_hDropItemOwner = hOwner; };
};
