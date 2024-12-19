#pragma once
#include "IStateMessage.h"

class CSMScanActor : public IStateMessage,
					  public TBoostMemoryPool<CSMScanActor>
{
private:
	set<int> m_setScanedActor;

public:
	CSMScanActor( DNVector(DnActorHandle)& vlhActor );
	virtual ~CSMScanActor(void);

	const set<int>& GetScanActorSet( void ) const
	{
		return m_setScanedActor; 
	};
};
