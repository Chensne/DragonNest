#pragma once

#include "SQLConnection.h"

class CDNSQLMembershipGuildWar : public CSQLConnection
{
public:
	CDNSQLMembershipGuildWar(void);
	virtual ~CDNSQLMembershipGuildWar(void);

	// 본선 보상받기 지급
	int QuerySetGuildWarRewardFinal();
};