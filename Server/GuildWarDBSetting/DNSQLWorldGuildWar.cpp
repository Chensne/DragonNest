#include "Stdafx.h"
#include "DNSQLWorldGuildWar.h"
#include "TimeSet.h"

CDNSQLWorldGuildWar::CDNSQLWorldGuildWar(void): CSQLConnection()
{

}

CDNSQLWorldGuildWar::~CDNSQLWorldGuildWar(void)
{

}

// 스케쥴 셋팅
int CDNSQLWorldGuildWar::QueryAddGuildWarSchedule( std::vector<std::string>& vSplit, int nScheduleID )
{
	CTimeSet tPreparation(vSplit[0].c_str(), true);
	CTimeSet tTrial(vSplit[1].c_str(), true);
	CTimeSet tReward(vSplit[2].c_str(), true);


	CTimeSet tPreparationEnd(tTrial);
	tPreparationEnd.AddSecond(-30);
	CTimeSet tTrialEnd(tReward);
	tTrialEnd.AddSecond(-30);
	CTimeSet tRewardEnd(vSplit[3].c_str(), true);	

	if (CheckConnect() < ERROR_NONE){		
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[8];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarSchedule('%d',?,?,?,?,?,?,2)}", nScheduleID);
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		DBTIMESTAMP temp[6] = {0,};
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		
		temp[0] = tPreparation.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[0], sizeof(DBTIMESTAMP), SqlLen[0].Get(SQL_TIMESTAMP_LEN));
		temp[1] = tPreparationEnd.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[1], sizeof(DBTIMESTAMP), SqlLen[1].Get(SQL_TIMESTAMP_LEN));
		temp[2] = tTrial.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[2], sizeof(DBTIMESTAMP), SqlLen[2].Get(SQL_TIMESTAMP_LEN));
		temp[3] = tTrialEnd.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[3], sizeof(DBTIMESTAMP), SqlLen[3].Get(SQL_TIMESTAMP_LEN));
		temp[4] = tReward.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[4], sizeof(DBTIMESTAMP), SqlLen[4].Get(SQL_TIMESTAMP_LEN));
		temp[5] = tRewardEnd.GetDbTimeStamp();
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[5], sizeof(DBTIMESTAMP), SqlLen[5].Get(SQL_TIMESTAMP_LEN));		

		RetCode = SQLExecute(m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	CTimeSet tFinalStart = tReward;
	CTimeSet tFinalEnd = tReward;

	//tFinalEnd에서 10분 더하기..(통계낼 시간 여유 주자)
	tFinalEnd.AddSecond(10*60);

	// 본선 기간 셋팅
	for( int i=4; i>0; --i)
	{
		CDNSqlLen SqlLentemp[2];

		memset(&m_wszQuery, 0, sizeof(m_wszQuery));
		swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarFinalSchedule('%d','%d',?,?)}", nScheduleID, i);

		tFinalStart = tFinalEnd;
		tFinalStart.AddSecond(60);
		tFinalEnd.AddSecond(38*60);

		RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
		if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
		{
			DBTIMESTAMP temp[6] = {0,};
			int nNo = 1;
			SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		
			temp[0] = tFinalStart.GetDbTimeStamp();
			SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[0], sizeof(DBTIMESTAMP), SqlLentemp[0].Get(SQL_TIMESTAMP_LEN));
			temp[1] = tFinalEnd.GetDbTimeStamp();
			SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &temp[1], sizeof(DBTIMESTAMP), SqlLentemp[1].Get(SQL_TIMESTAMP_LEN));			

			RetCode = SQLExecute(m_hstmt);
			if (CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
				return ERROR_DB;
		}
	}	
	return nResult;
}

// 길드전예선내용 초기화(주의..미션점수, 통계 초기화됨)
int CDNSQLWorldGuildWar::QueryInitGuildWarTrial()
{
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelGuildWarOpeningPointRecord}" );

	return CommonReturnValueQuery(m_wszQuery);
}
// 인기투표초기화
int CDNSQLWorldGuildWar::QueryInitGuildWarVote(const char* strCharName)
{
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_CheatGuildWarPopularityVoteClear(N'%S')}", strCharName );

	return CommonReturnValueQuery(m_wszQuery);
}
// 예선통계내기
int CDNSQLWorldGuildWar::QuerySetGuildWarStats()
{
	int nRetCode = ERROR_DB;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarOpeningPointTotal}" );

	nRetCode = CommonReturnValueQuery(m_wszQuery);
	if( nRetCode != ERROR_NONE)
		return nRetCode;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarOpeningPointDailyTotal}" );

	nRetCode = CommonReturnValueQuery(m_wszQuery);
	if( nRetCode != ERROR_NONE)
		return nRetCode;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarOpeningPointGuildTotal}" );

	return CommonReturnValueQuery(m_wszQuery);
}

// 예선 보상 초기화(개인)
int CDNSQLWorldGuildWar::QueryInitGuildWarRewardCharacter(const char* strCharName)
{
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_CheatGuildWarRewardForCharactercClear(N'%S')}", strCharName );

	return CommonReturnValueQuery(m_wszQuery);
}

// 예선 보상 초기화(길드)
int CDNSQLWorldGuildWar::QueryInitGuildWarRewardGuild(const char* strGuildName)
{
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_CheatGuildWarRewardForGuildClear(N'%S')}", strGuildName );

	return CommonReturnValueQuery(m_wszQuery);
}

// 본선 보상받기 지급
int CDNSQLWorldGuildWar::QuerySetGuildWarRewardFinal()
{
	int nRetCode = ERROR_DB;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SendGuildWarPointQueue}" );

	nRetCode = CommonReturnValueQuery(m_wszQuery);
	if( nRetCode != ERROR_NONE)
		return nRetCode;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SendGuildWarSystemMailQueue}" );

	return CommonReturnValueQuery(m_wszQuery);
}