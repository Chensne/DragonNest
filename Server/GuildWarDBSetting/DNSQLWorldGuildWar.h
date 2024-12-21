#pragma once

#include "SQLConnection.h"

class CDNSQLWorldGuildWar : public CSQLConnection
{
public:
	CDNSQLWorldGuildWar(void);
	virtual ~CDNSQLWorldGuildWar(void);

	// 스케쥴 셋팅
	int QueryAddGuildWarSchedule( std::vector<std::string>& vSplit, int nScheduleID );
	// 길드전예선내용 초기화(주의..미션점수, 통계 초기화됨)
	int QueryInitGuildWarTrial();
	// 인기투표초기화
	int QueryInitGuildWarVote(const char* strCharName);
	// 예선통계내기
	int QuerySetGuildWarStats();
	// 예선 보상 초기화(개인)
	int QueryInitGuildWarRewardCharacter(const char* strCharName);
	// 예선 보상 초기화(길드)
	int QueryInitGuildWarRewardGuild(const char* strGuildName);
	// 본선 보상받기 지급
	int QuerySetGuildWarRewardFinal();
};