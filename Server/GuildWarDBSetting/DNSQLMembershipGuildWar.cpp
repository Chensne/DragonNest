#include "Stdafx.h"
#include "DNSQLMembershipGuildWar.h"

CDNSQLMembershipGuildWar::CDNSQLMembershipGuildWar(void): CSQLConnection()
{
}

CDNSQLMembershipGuildWar::~CDNSQLMembershipGuildWar(void)
{
}

// 본선 보상받기 지급
int CDNSQLMembershipGuildWar::QuerySetGuildWarRewardFinal()
{
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SendGuildWarMakeGiftQueue}" );

	return CommonReturnValueQuery(m_wszQuery);
}