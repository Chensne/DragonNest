
#pragma once

#include "DnPartyTask.h"

class CDnPvPPartyTask:public CDnPartyTask,public TBoostMemoryPool<CDnPvPPartyTask>
{
public:

	CDnPvPPartyTask( CDNGameRoom * pRoom );
	virtual ~CDnPvPPartyTask();

	virtual void OutPartyMember( UINT nOutPartyMemberUID, UINT nNewLeaderUID, char cKickKind );
};
