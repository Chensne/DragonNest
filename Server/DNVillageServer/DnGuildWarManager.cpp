#include "StdAfx.h"
#include "DNGuildWarManager.h"
#include "DNGameDataManager.h"
#include "DNDBConnectionManager.h"
#include "EtUIXML.h"
#include "DNUserItem.h"
#include "DNGuildBase.h"

extern TVillageConfig g_Config;
CDNGuildWarManager * g_pGuildWarManager = NULL;

CDNGuildWarManager::CDNGuildWarManager()
{
	m_PreWinGuildUID.Reset();
	Reset();
}

CDNGuildWarManager::~CDNGuildWarManager()
{
	
}

void CDNGuildWarManager::Reset()
{
	memset(&m_sGuildWarRankingInfo, 0, sizeof(m_sGuildWarRankingInfo));
	m_nGuildWarTrialRankingCount = 0;
	memset(&m_sGuildWarTrialRankingInfo, 0, sizeof(m_sGuildWarTrialRankingInfo));
	m_bFinalProgress = false;
	memset(&m_nGuildWarMissionRankingCharacterDBID,0, sizeof(m_nGuildWarMissionRankingCharacterDBID));
	memset(&m_nGuildWarDailyAwardCharacterDBID,0, sizeof(m_nGuildWarDailyAwardCharacterDBID));

	memset(&m_sGuildWarMissionGuildRankingInfo, 0, sizeof(m_sGuildWarMissionGuildRankingInfo));
	memset(&m_sGuildWarMissionRankingInfo, 0, sizeof(m_sGuildWarMissionRankingInfo));
	memset(&m_sGuildWarDailyAward, 0, sizeof(m_sGuildWarDailyAward));

	memset(m_wszVoteGuildName, 0, sizeof(m_wszVoteGuildName));
	memset(&m_sGuildWarFinalInfo, 0, sizeof(m_sGuildWarFinalInfo));

	memset(&m_sGuildWarSchedule, 0, sizeof(m_sGuildWarSchedule));
	memset(&m_sGuildWarFinalSchedule, 0, sizeof(m_sGuildWarFinalSchedule));

	m_cGuildWarFinalPart = GUILDWAR_FINALPART_16;
	m_tGuildWarFinalPartBeginTime = 0;
	m_bGetGuildWarVote = false;
	m_nVoteCount = 0;
	m_cStepIndex = 0;
	m_nBlueTeamPoint = 0;
	m_nRedTeamPoint = 0;
	m_wScheduleID = GUILDWARSCHEDULE_DEF;
	m_wWinersWeightRate = 0;
	m_bGuildWarStats = false;
	m_bGuildWarStats = 0;
	m_cSecretTeam = 0;
	m_nSecretRandomSeed = 0;
	memset (m_vSecretMissionID, 0x00, sizeof(m_vSecretMissionID));
	m_dwPreWinSkillCoolTime = 0;
}

bool CDNGuildWarManager::GetRestriction(int nSubCmd, TGuildUID GuildUID)
{
	if( m_cStepIndex == GUILDWAR_STEP_NONE)	
		return false;		// 제약 없음

	switch (nSubCmd)
	{
	case eGuild::CS_INVITEGUILDMEMBREQ: // 가입
		{
			if (m_cStepIndex == GUILDWAR_STEP_PREPARATION)
				return false;
			// 보상 및 본선 기간에는 16강 진출길드만 가입 안되게 막음
			if( m_cStepIndex == GUILDWAR_STEP_REWARD )
			{				
				if( GetFinalProgress() == false )
					return false;	// 제약 없음.

				for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i )
				{
					if( !m_sGuildWarFinalInfo[i].GuildUID.IsSet() )
						continue;
					if( m_sGuildWarFinalInfo[i].GuildUID == GuildUID )
						return true;
				}
				return false;
			}
		}
		break;	
	case eGuild::CS_LEAVEGUILDMEMB:	// 탈퇴
	case eGuild::CS_EXILEGUILDMEMB:	// 추방	
		{
			if (m_cStepIndex == GUILDWAR_STEP_TRIAL || m_cStepIndex == GUILDWAR_STEP_REWARD)
				return false;
		}
		break;

	case eGuild::CS_DISMISSGUILD: // 해체
		{
			if (m_cStepIndex == GUILDWAR_STEP_END)
				return false;
		}
	}
	return true;
}

void CDNGuildWarManager::SetGuildWarSechdule(MASetGuildWarEventTime* SetGuildWarEventTime)
{
	memcpy(m_sGuildWarSchedule, &SetGuildWarEventTime->sGuildWarTime[GUILDWAR_STEP_PREPARATION], sizeof(m_sGuildWarSchedule));
	memcpy(m_sGuildWarFinalSchedule, &SetGuildWarEventTime->sFinalPartTime[GUILDWAR_FINALPART_FINAL], sizeof(m_sGuildWarFinalSchedule));
	m_bFinalProgress = SetGuildWarEventTime->bFinalProgress;
}

int CDNGuildWarManager::GetGuildWarPoint(UINT nGuildDBID)
{
	int nRank = GUILDWAR_FINALS_TEAM_MAX+1;
	for (int i=0; i<GUILDWAR_FINALS_TEAM_MAX; i++)
	{
		if (m_sGuildWarRankingInfo[i].nGuildDBID == nGuildDBID)
		{
			nRank = m_sGuildWarRankingInfo[i].wRanking;
			break;
		}
	}

	
	TGuildWarPoint* pWarPoint = g_pDataManager->GetGuildWarPointData(nRank);
	if (!pWarPoint)
		return GUILDWARPOINT_DEF;

	return pWarPoint->nRewardGuildPoint;
}

// 본선 진출 길드
void CDNGuildWarManager::SetGuildWarRankingInfo(TAGetGuildWarPointGuildTotal* pGetGuildWarPointGuildTotal )
{	for( int i=0; i<pGetGuildWarPointGuildTotal->nTotalCount; ++i)
	{
		m_sGuildWarRankingInfo[i].wRanking = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wRanking;
		m_sGuildWarRankingInfo[i].nTotalPoint = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nOpeningPointSum;
		m_sGuildWarRankingInfo[i].wCurGuildUserCount = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wGuildMemberCount;
		memcpy(m_sGuildWarRankingInfo[i].wszGuildName, pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wszGuildName, sizeof(m_sGuildWarRankingInfo[i].wszGuildName));
		memcpy(m_sGuildWarRankingInfo[i].wszGuildMasterName, pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].wszMasterName, sizeof(m_sGuildWarRankingInfo[i].wszGuildMasterName));

		m_sGuildWarRankingInfo[i].wMaxGuildUserCount = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nMaxMemberCount;
		m_sGuildWarRankingInfo[i].nGuildDBID = pGetGuildWarPointGuildTotal->sGuildWarPointGuildTotal[i].nGuildDBID;	
	}	
	// 셋팅 완료..이게 마지막임.
	m_bGuildWarStats = true;
}
// 부문별 길드 순위 셋팅
void CDNGuildWarManager::SetGuildWarMissionGuildRankingInfo(TAGetGuildWarPointGuildPartTotal* pGetGuildWarPointGuildPartTotal)
{
	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; ++i)
	{
		if( pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].nPoint > 0 )
		{
			memcpy(m_sGuildWarMissionGuildRankingInfo[i].wszGuildName, pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].wszGuildName
				, sizeof(m_sGuildWarMissionGuildRankingInfo[i].wszGuildName));
			memcpy(m_sGuildWarMissionGuildRankingInfo[i].wszGuildMasterName, pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].wszCharName
				, sizeof(m_sGuildWarMissionGuildRankingInfo[i].wszGuildMasterName));
			m_sGuildWarMissionGuildRankingInfo[i].nTotalPoint = pGetGuildWarPointGuildPartTotal->GuildWarPointPartTotal[i].nPoint;
		}
	}
}
// 부문별 개인 순위 셋팅
void CDNGuildWarManager::SetGuildWarMissionRankingInfo(TAGetGuildWarPointPartTotal* pGetGuildWarPointPartTotal)
{
	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; ++i)
	{
		if( pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].nPoint > 0 )
		{
			memcpy(m_sGuildWarMissionRankingInfo[i].wszGuildName, pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].wszGuildName
				, sizeof(m_sGuildWarMissionGuildRankingInfo[i].wszGuildName));
			memcpy(m_sGuildWarMissionRankingInfo[i].wszCharName, pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].wszCharName
				, sizeof(m_sGuildWarMissionGuildRankingInfo[i].wszGuildMasterName));
			m_sGuildWarMissionRankingInfo[i].nTotalPoint = pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].nPoint;
			m_nGuildWarMissionRankingCharacterDBID[i] = pGetGuildWarPointPartTotal->GuildWarPointPartTotal[i].biCharacterDBID;
		}
	}
}
// 날짜별 시상
void CDNGuildWarManager::SetGuildDailyAward(TAGetGuildWarPointDaily* pGetGuildWarPointDaily)
{
	for( int i=0; i<GUILDWAR_DAILY_AWARD_MAX; ++i)
	{
		memcpy(m_sGuildWarDailyAward[i].wszCharName, pGetGuildWarPointDaily->sGuildWarPointDaily[i].wszCharName, sizeof(m_sGuildWarDailyAward[i].wszCharName));
		m_sGuildWarDailyAward[i].nGuildUID = TGuildUID(g_Config.nWorldSetID, pGetGuildWarPointDaily->sGuildWarPointDaily[i].nGuildDBID);
		m_nGuildWarDailyAwardCharacterDBID[i] = pGetGuildWarPointDaily->sGuildWarPointDaily[i].biCharacterDBID;
	}
}

void CDNGuildWarManager::SetGuildWarStep(char cStepIndex, short wScheduleID, short wWinersWeightRate )
{
	m_cStepIndex = cStepIndex;
	m_wScheduleID = wScheduleID;
	m_wWinersWeightRate = wWinersWeightRate;	
}

void CDNGuildWarManager::SetGuildTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo)
{
	memcpy(m_sGuildWarFinalInfo, pGuildTournamentInfo, sizeof(m_sGuildWarFinalInfo));	
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetGuildTournamentInfo !!\r\n");
}
void CDNGuildWarManager::SetGuildTournamentResult(MAVISetGuildWarFinalResult* pGuildTournamentResult)
{
	if( pGuildTournamentResult->cWinGuildIndex >= 0 && pGuildTournamentResult->cWinGuildIndex < GUILDWAR_FINALS_TEAM_MAX)
	{
		m_sGuildWarFinalInfo[pGuildTournamentResult->cWinGuildIndex].bWin = true;
		m_sGuildWarFinalInfo[pGuildTournamentResult->cWinGuildIndex].cMatchTypeCode = pGuildTournamentResult->cMatchTypeCode;
	}

	if( pGuildTournamentResult->cLoseGuildIndex >= 0 && pGuildTournamentResult->cLoseGuildIndex < GUILDWAR_FINALS_TEAM_MAX)
	{
		m_sGuildWarFinalInfo[pGuildTournamentResult->cLoseGuildIndex].bWin = false;
		m_sGuildWarFinalInfo[pGuildTournamentResult->cLoseGuildIndex].cMatchTypeCode = pGuildTournamentResult->cMatchTypeCode;
	}	
}

void CDNGuildWarManager::SetGuildWarTrialRankingInfo(MASetGuildWarPointRunningTotal* pSetGuildWarPointRunningTotal )
{
	m_nGuildWarTrialRankingCount = 0;
	memset(m_sGuildWarTrialRankingInfo, 0, sizeof(m_sGuildWarTrialRankingInfo));

	for( int i=0; i<GUILDWAR_TRIAL_POINT_TEAM_MAX; ++i )
	{
		if( pSetGuildWarPointRunningTotal->sGuildWarPointRunningTotal[i].wRanking == 0)
			break;
		memcpy(&m_sGuildWarTrialRankingInfo[i], &pSetGuildWarPointRunningTotal->sGuildWarPointRunningTotal[i], sizeof(SGuildWarRankingInfo));
		++m_nGuildWarTrialRankingCount;
	}
}

void CDNGuildWarManager::SetGuildWarFinalPart(char cGuildFinalPart, __time64_t tFinalPartBeginTime)
{
	m_cGuildWarFinalPart = cGuildFinalPart;
	m_tGuildWarFinalPartBeginTime = tFinalPartBeginTime;
}

bool CDNGuildWarManager::IsGuildWarFinal(TGuildUID nGuildUID)
{
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i )
	{
		if( m_sGuildWarFinalInfo[i].GuildUID == nGuildUID )
			return true;
	}
	return false;
}

int CDNGuildWarManager::CalcGuildWarFestivalPoint(char cTeamType, INT64 nChracterDBID, int nGuildWarPoint)
{	
	//캐릭터가 획득한 미션 포인트 + ( 캐릭터가 획득한 미션 포인트 * 내가 수상한 특별상 총 개수 * 0.1)  * 길드 예선전 승리 했을 경우 x2
	int nAwardCount = GetGuildAwardCount(nChracterDBID);
	bool bWinTeam = IsGuildWarTrialWIn(cTeamType);
	if( bWinTeam )
		return (int)((nGuildWarPoint + (nGuildWarPoint*nAwardCount*0.1f))*m_wWinersWeightRate);
	return (int)(nGuildWarPoint + (nGuildWarPoint*nAwardCount*0.1f));	
}

bool CDNGuildWarManager::IsGuildWarTrialWIn(char cTeamType)
{
	bool bWinTeam = false;	
	if( cTeamType == GUILDWAR_TEAM_BLUE)	
		bWinTeam = m_nBlueTeamPoint > m_nRedTeamPoint ? true : false;
	else
		bWinTeam = m_nRedTeamPoint > m_nBlueTeamPoint ? true : false;
	return bWinTeam;
}

int CDNGuildWarManager::GetGuildAwardCount(INT64 nChracterDBID)
{
	int nAwardCount = 0;	
	// 부문별
	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; ++i )
	{
		if( m_nGuildWarMissionRankingCharacterDBID[i] == nChracterDBID )
			++nAwardCount;
	}
	for( int i=0; i<GUILDWAR_DAILY_AWARD_MAX; ++i )
	{
		if( m_nGuildWarDailyAwardCharacterDBID[i] == nChracterDBID )
			++nAwardCount;
	}
	return nAwardCount;
}

void CDNGuildWarManager::SetGuildSecretMission(MASetGuildWarSecretMission* pSecret)
{
	m_cSecretTeam = pSecret->cTeamCode;
	m_nSecretRandomSeed = pSecret->nRandomSeed;
	if( m_nSecretRandomSeed == -1 ) // 해제
	{
		memset (m_vSecretMissionID, 0x00, sizeof(m_vSecretMissionID));
		return;
	}

	g_pDataManager->GetRandomGuildMission(m_nSecretRandomSeed, m_vSecretMissionID);
}

bool CDNGuildWarManager::IsSecretMission (BYTE cTeamCode, int nMissionID)
{
	if (m_cSecretTeam != cTeamCode)
		return false;

	for (int i=0; i<GUILDWARMISSION_MAXGROUP; i++)
	{
		if (m_vSecretMissionID[i] == nMissionID)
			return true;
	}

	return false;
}

void CDNGuildWarManager::CalcTeamSecret()
{
	if (m_nBlueTeamPoint == m_nRedTeamPoint)
	{
		m_cSecretTeam = 0;
		return;
	}

	float fSecretRate = 0.0f;
	if (m_nBlueTeamPoint > m_nRedTeamPoint)
		fSecretRate = (float)m_nRedTeamPoint / (float)m_nBlueTeamPoint;
	else
		fSecretRate = (float)m_nBlueTeamPoint / (float)m_nRedTeamPoint;
	
	if (fSecretRate <= 0)
	{
		DN_ASSERT( false, "fSecretRate <= 0" );
		return;
	}

	fSecretRate = 1.0f - fSecretRate;

	// 마스터 서버가 시크릿 여부를 판단 못할 경우를 대비하여 시크릿 중지여부 판단한다.
	if (fSecretRate < 0.02f)		
		m_cSecretTeam = 0;			// 해제
}

void CDNGuildWarManager::OnRecvMaGuildChangeMark( MAGuildChangeMark* pPacket )
{
	for( UINT i=0 ; i<_countof(m_sGuildWarFinalInfo) ; ++i )
	{
		if( m_sGuildWarFinalInfo[i].GuildUID.IsSet() == false )
			continue;
		if( m_sGuildWarFinalInfo[i].GuildUID != pPacket->GuildUID )
			continue;
	
		m_sGuildWarFinalInfo[i].wGuildMark			= pPacket->wMark;
		m_sGuildWarFinalInfo[i].wGuildMarkBG		= pPacket->wMarkBG;
		m_sGuildWarFinalInfo[i].wGuildMarkBorder	= pPacket->wMarkBorder;
		break;
	}
}

// 길드명 변경
void CDNGuildWarManager::OnRecvMaGuildChangeName( MAGuildChangeName* pPacket )
{
	for( UINT i=0 ; i<_countof(m_sGuildWarFinalInfo) ; ++i )
	{
		if( m_sGuildWarFinalInfo[i].GuildUID.IsSet() == false )
			continue;
		if( m_sGuildWarFinalInfo[i].GuildUID != pPacket->GuildUID )
			continue;

		memcpy(m_sGuildWarFinalInfo[i].wszGuildName, pPacket->wszGuildName, sizeof(m_sGuildWarFinalInfo[i].wszGuildName));		
		// 이럼 통계 다시 뽑을까나?
		// 예선 통계 정보 가져오기..
		// 순서, QueryGetGuildWarPointPartTotal, QueryGetGuildWarPointGuildPartTotal, QueryGetGuildWarPointDaily, QueryGetGuildWarPointGuildTotal
		if ( g_pDBConnectionManager )
		{
			BYTE cThreadID;
			CDNDBConnection* pDBCon = g_pDBConnectionManager->GetDBConnection( cThreadID );
			if( pDBCon )
			{
				pDBCon->QueryGetGuildWarPointPartTotal(cThreadID, g_Config.nWorldSetID, 0, 'A', 0);
				g_Log.Log(LogType::_GUILDWAR,g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] ChangeGuildName and retry stats !!\r\n");
			}
		}
		break;
	}
}
