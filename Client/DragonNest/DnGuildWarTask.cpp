#include "StdAfx.h"
#include "DnGuildWarTask.h"
#include "DnInterface.h"
#include "DnGuildWarRoomDlg.h"
#include "DnGuildTask.h"
#include "DnTableDB.h"
#include "IDnSkillUsableChecker.h"
#include "IDnSkillProcessor.h"
#include "DnUIString.h"
#include "DnSkillTreeDlg.h"
#include "DnMainMenuDlg.h"
#include "DnSkillTask.h"
#include "DnGuildWarSituationMng.h"
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#include "DnMutatorGuildWar.h"
#include "DnPlayerActor.h"
#include "DnLocalPlayerActor.h"
#include "DnGuildWarFinalTournamentListDlg.h"
#include "SyncTimer.h"
#include "DnInterfaceString.h"
#include "DnOccupationTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildWarTask::CDnGuildWarTask()
: CTaskListener( false )
, m_nGuildWarState( PvPCommon::OccupationSystemState::None )
, m_nGuildWarSkillUsedPoint( 0 )
, m_nGuildWarSkillLeavePoint( MAX_GUILD_WAR_SKILL_POINT )
, m_biGuildWarFestivalPoint( 0 )
, m_CurGuildWarFinalRoundStep( GUILDWAR_FINALPART_NONE )
, m_CurGuildWarStep( GUILDWAR_STEP_NONE )
, m_bGuildWarFinalProgress( false )
, m_bSetForceUpdateAlarm( false )
{
}

CDnGuildWarTask::~CDnGuildWarTask()
{
	m_vecPreliminaryResultGuildRanking.clear();
	m_vecEventGuildRankInfo.clear();
	m_vecEventPersonalRankInfo.clear();
	m_vecGuildWarShopItemInfo.clear();
}

bool CDnGuildWarTask::Initialize()
{
#if defined(PRE_FIX_NEXTSKILLINFO)
	InitSkillLevelTableIDList();
#endif // PRE_FIX_NEXTSKILLINFO
	return true;
}

void CDnGuildWarTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( nMainCmd == SC_GUILD )
	{
		switch( nSubCmd )
		{
			// 데이터를 서버에서 받아 창을 연다
			case eGuild::SC_GUILDWAR_TRIAL_RESULT_UIOPEN: OnRecvGuildWarTrialResultOpenMsg(); break;
			case eGuild::SC_GUILDWAR_TOURNAMENT_INFO_UIOPEN: OnRecvGuildWarTournamentOpenMsg(); break;
			case eGuild::SC_GUILDWAR_STATUS_OPEN: OnRecvGuildWarStatusOpenMsg( (SCGuildWarStatus*)pData ); break;
			case eGuild::SC_GUILDWAR_TRIAL_RESULT_OPEN: OnRecvGuildWarTrialResultInfoMsg( (SCGuildWarTrialResult*)pData ); break;
			case eGuild::SC_GUILDWAR_TOURNAMENT_INFO_OPEN: OnRecvGuildWarTournamentInfoMsg( (SCGuildTournamentInfo*)pData ); break;
			case eGuild::SC_GUILDWAR_FESTIVAL_POINT: OnRecvGuildWarFestivalPoint( (SCGuildWarFestivalPoint*)pData ); break;
			case eGuild::SC_GUILDWAR_COMPENSATION: OnRecvGuildWarGetReward( (SCGuildWarCompensation*)pData ); break;
			case eGuild::SC_GUILDWAR_VOTE: OnRecvGuildWarVote( (SCGuildWarVote*)pData ); break;
			case eGuild::SC_GUILDWAR_VOTE_TOP: OnRecvGuildWarVoteTop( (SCGuildWarVoteTop*)pData ); break;
			case eGuild::SC_GUILDWAR_EVENT_TIME: OnRecvGuildWarEventTime( (SCGuildWarEventTime*)pData ); break;
			case eGuild::SC_GUILDWAR_BUYED_ITEM_COUNT_LIST: OnRecvGuildWarShopItemInfoList( (SCBuyedItemCount*)pData ); break;
			case eGuild::SC_GUILDWAR_BUYED_ITEM_COUNT: OnRecvGuildWarShopItemInfo( (SCBuyedItemCount*)pData ); break;
		}
	}
	else if( nMainCmd == SC_PVP )
	{
		switch (nSubCmd)
		{
			case ePvP::SC_PVP_MEMBERINDEX:
				{
					SCPvPMemberIndex * pPacket = (SCPvPMemberIndex*)pData;

					if (pPacket->nRetCode == ERROR_NONE)
					{
						if( GetInterface().GetGuildWarRoomDlg() )
							GetInterface().GetGuildWarRoomDlg()->RefreshSlot( pPacket );
						else if( GetInterface().GetGuildWarSituation() )
							GetInterface().GetGuildWarSituation()->MoveUser( pPacket );
					}
					else
					{
						if( GetInterface().GetGuildWarRoomDlg() )
							GetInterface().GetGuildWarRoomDlg()->RevertSlot();

						GetInterface().ServerMessageBox(pPacket->nRetCode);		//정상이 아닌경우 메세지 출력. 게임도중에는 달라질 수 있음 기획자랑 협의바람(게임중에는 채팅메세지처리같은)
					}
					break;
				}

			case ePvP::SC_PVP_MEMBERGRADE:
				{
					SCPvPMemberGrade * pPacket = (SCPvPMemberGrade*)pData;

					if (pPacket->nRetCode == ERROR_NONE)
					{
						CDnGuildWarSituationMng * pMng = GetInterface().GetGuildWarSituation();
						if( pMng )
							pMng->SetUserState( pPacket->nSessionID, pPacket->uiUserState );
					}
					else if( pPacket->nRetCode == ERROR_PVP_GUILDWAR_SECONDARYMASTER_ASIGNFAIL )
						GetInterface().MessageBox( 126222 );	// 부마스터는 최대 3명까지 위임할 수 있습니다.
					else
						GetInterface().ServerMessageBox(pPacket->nRetCode);
					break;
				}
			case ePvP::SC_PVP_OCCUPATION_MODESTATE:		OnRecvGuildWarModeState( (SCPvPOccupationModeState*)pData );	break;
		}
	}
}

void CDnGuildWarTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if (m_GuildWarEventTime.empty() == false)
	{
		const __time64_t tCurTime = CSyncTimer::GetInstance().GetCurTime();

		bool bGWStepProcessd = false;
		std::map<eGuildWarStepType, SGuildWarEventTime>::const_iterator iter = m_GuildWarEventTime.begin();
		for (; iter != m_GuildWarEventTime.end(); ++iter)
		{
			const SGuildWarEventTime& curTime = (*iter).second;
			if (curTime.term.tBeginTime <= tCurTime && curTime.term.tEndTime > tCurTime)
			{
				const eGuildWarStepType& step = (*iter).first;
				if (m_CurGuildWarStep != step || m_bSetForceUpdateAlarm)
				{
					m_CurGuildWarStep = step;
					if (m_CurGuildWarStep == GUILDWAR_STEP_REWARD)
					{
						std::map<eGuildFinalPart, SGuildWarEventTime>::const_iterator roundIter = m_GuildWarFinalRoundTime.begin();
						for (; roundIter != m_GuildWarFinalRoundTime.end(); ++roundIter)
						{
							const SGuildWarEventTime& curRoundTime = (*roundIter).second;
							if (curRoundTime.term.tBeginTime <= tCurTime && curRoundTime.term.tEndTime > tCurTime)
							{
								const eGuildFinalPart& curRoundStep = (*roundIter).first;
								if (curRoundStep != m_CurGuildWarFinalRoundStep)
								{
									m_CurGuildWarFinalRoundStep = curRoundStep;
									break;
								}
							}
						}
					}

					bool bShow = (m_CurGuildWarStep != GUILDWAR_STEP_NONE && m_CurGuildWarStep != GUILDWAR_STEP_END);
					GetInterface().RefreshGuildWarMark(bShow, true);

					m_bSetForceUpdateAlarm = false;
				}

				bGWStepProcessd = true;
				break;
			}
		}

		if (bGWStepProcessd == false && m_CurGuildWarStep != GUILDWAR_STEP_NONE)
		{
			m_CurGuildWarStep = GUILDWAR_STEP_NONE;
			m_CurGuildWarFinalRoundStep = GUILDWAR_FINALPART_NONE;

			GetInterface().RefreshGuildWarMark(false, false);
		}
	}
}

void CDnGuildWarTask::GetGuildWarEventTime(std::wstring& startTime, std::wstring& endTime, eGuildWarStepType type) const
{
	startTime.clear();
	endTime.clear();

	std::map<eGuildWarStepType, SGuildWarEventTime>::const_iterator iter = m_GuildWarEventTime.find(type);
	if (iter != m_GuildWarEventTime.end())
	{
		const SGuildWarEventTime& eventTime = (*iter).second;
		startTime	= eventTime.timeString[GUILDWAR_EVENT_START];
		endTime		= eventTime.timeString[GUILDWAR_EVENT_END];
	}
}

void CDnGuildWarTask::GetGuildWarFinalRoundTime(std::wstring& startTime, std::wstring& endTime, eGuildFinalPart step) const
{
	startTime.clear();
	endTime.clear();

	std::map<eGuildFinalPart, SGuildWarEventTime>::const_iterator iter = m_GuildWarFinalRoundTime.find(step);
	if (iter != m_GuildWarFinalRoundTime.end())
	{
		const SGuildWarEventTime& eventTime = (*iter).second;
		startTime	= eventTime.timeString[GUILDWAR_EVENT_START];
		endTime		= eventTime.timeString[GUILDWAR_EVENT_END];
	}
}

void CDnGuildWarTask::UseGuildWarSkillPoint()
{
	m_nGuildWarSkillUsedPoint++;
	m_nGuildWarSkillLeavePoint--;

	if( m_nGuildWarSkillLeavePoint < 0 )
		m_nGuildWarSkillLeavePoint = 0;

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg && pSkillTreeDlg->IsShow() )
		pSkillTreeDlg->UpdateGuildWarSkillPointInfo();
}

void CDnGuildWarTask::OnRecvGuildWarTrialResultOpenMsg()
{
	GetInterface().OpenGuildWarPreliminaryResult();
}

void CDnGuildWarTask::OnRecvGuildWarTournamentOpenMsg()
{
	GetInterface().OpenGuildWarFinalTournamentList();
}

void CDnGuildWarTask::OnRecvGuildWarStatusOpenMsg( SCGuildWarStatus* pPacket )
{
	_ASSERT( pPacket );

	// Clear Data
	m_stPreliminaryStatus.nBluePoint = 0;
	m_stPreliminaryStatus.nRedPoint = 0;
	m_stPreliminaryStatus.nMyPoint = 0;
	m_stPreliminaryStatus.nMyGuildPoint = 0;

	m_stPreliminaryStatus.nBluePoint = pPacket->nBluePoint;
	m_stPreliminaryStatus.nRedPoint = pPacket->nRedPoint;
	m_stPreliminaryStatus.nMyPoint = pPacket->nMyPoint;
	m_stPreliminaryStatus.nMyGuildPoint = pPacket->nMyGuildPoint;

	m_vecPreliminaryResultGuildRanking.clear();
	for( int i=0; i<pPacket->nRankingCount; i++ )
	{
		stJoinGuildRankInfo stGuildInfo;
		stGuildInfo.m_nGuildRank = pPacket->sGuildWarPointTrialRanking[i].wRanking;
		stGuildInfo.m_nGuildUserCount = pPacket->sGuildWarPointTrialRanking[i].wCurGuildUserCount;
		stGuildInfo.m_nGuildUserMax = pPacket->sGuildWarPointTrialRanking[i].wMaxGuildUserCount;
		stGuildInfo.m_nTotalPoints = pPacket->sGuildWarPointTrialRanking[i].nTotalPoint;
		stGuildInfo.m_strGuildMasterName = pPacket->sGuildWarPointTrialRanking[i].wszGuildMasterName;
		stGuildInfo.m_strGuildName = pPacket->sGuildWarPointTrialRanking[i].wszGuildName;

		m_vecPreliminaryResultGuildRanking.push_back( stGuildInfo );
	}

	GetInterface().SetGuildWarPreliminaryResult();
}

void CDnGuildWarTask::OnRecvGuildWarTrialResultInfoMsg( SCGuildWarTrialResult* pPacket )
{
	_ASSERT( pPacket );

	if( !CDnGuildTask::IsActive() ) return;
	TGuild* pGuild = GetGuildTask().GetGuildInfo();

	tstring strMyGuildName;
	if( pGuild && pGuild->IsSet() )
		strMyGuildName = pGuild->GuildView.wszGuildName;

	// Clear Data
	m_vecPreliminaryResultGuildRanking.clear();
	m_vecEventGuildRankInfo.clear();
	m_vecEventPersonalRankInfo.clear();
	m_vecDailyBestChar.clear();

	m_stPreliminaryResult.nBlueFinalPoint = 0;
	m_stPreliminaryResult.nRedFinalPoint = 0;
	m_stPreliminaryResult.bJoinGuildWar = 0;
	m_stPreliminaryResult.bReward = false;
	m_stPreliminaryResult.bMyWin = false;

	m_stPreliminaryResult.nBlueFinalPoint = pPacket->nBluePoint;
	m_stPreliminaryResult.nRedFinalPoint = pPacket->nRedPoint;
	m_stPreliminaryResult.bJoinGuildWar = pPacket->bGuildWar;

	stJoinGuildRankInfo stGuildInfo;

	if( pGuild && pGuild->IsSet() && m_stPreliminaryResult.bJoinGuildWar )	 // 길드전 참가 길드만 내 길드 정보 있음
	{
		int nGuildUserCount = 0;
		int nGuildUserMax = 0;
		nGuildUserMax = pGuild->wGuildSize;
		nGuildUserCount = GetGuildTask().GetGuildMemberCount();

		m_stPreliminaryResult.bReward = pPacket->bReward;
		if( ( pGuild->GuildView.cTeamColorCode == 1 && pPacket->nBluePoint > pPacket->nRedPoint )
			|| ( pGuild->GuildView.cTeamColorCode == 2 && pPacket->nBluePoint < pPacket->nRedPoint ) )
			m_stPreliminaryResult.bMyWin = true;
		else
			m_stPreliminaryResult.bMyWin = false;

		stGuildInfo.m_nGuildRank = pPacket->sMyGuildWarRankingInfo.wRanking;
		stGuildInfo.m_nGuildUserCount = nGuildUserCount;
		stGuildInfo.m_nGuildUserMax = nGuildUserMax;
		stGuildInfo.m_nTotalPoints = pPacket->sMyGuildWarRankingInfo.nTotalPoint;
		stGuildInfo.m_strGuildMasterName = GetGuildTask().GetGuildMasterName();
		stGuildInfo.m_strGuildName = strMyGuildName;

		m_stPreliminaryResult.nFestivalPoint = pPacket->nFestivalPoint;
		m_stPreliminaryResult.nFestivalPointAdd = pPacket->nFestivalPointAdd;
	}

	m_vecPreliminaryResultGuildRanking.push_back( stGuildInfo );

	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; i++ )
	{
		stGuildInfo.m_nGuildRank = pPacket->sGuildWarRankingInfo[i].wRanking;
		stGuildInfo.m_nGuildUserCount = pPacket->sGuildWarRankingInfo[i].wCurGuildUserCount;
		stGuildInfo.m_nGuildUserMax = pPacket->sGuildWarRankingInfo[i].wMaxGuildUserCount;
		stGuildInfo.m_nTotalPoints = pPacket->sGuildWarRankingInfo[i].nTotalPoint;
		stGuildInfo.m_strGuildMasterName = pPacket->sGuildWarRankingInfo[i].wszGuildMasterName;
		stGuildInfo.m_strGuildName = pPacket->sGuildWarRankingInfo[i].wszGuildName;

		m_vecPreliminaryResultGuildRanking.push_back( stGuildInfo );
	}

	stEventRankInfo stEventInfo;

	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; i++ )
	{
		stEventInfo.m_nRank = 1;
		stEventInfo.m_nTotalPoints = pPacket->sGuildWarMissionGuildRankingInfo[i].nTotalPoint;
		stEventInfo.m_strName1 = pPacket->sGuildWarMissionGuildRankingInfo[i].wszGuildName;
		stEventInfo.m_strName2 = pPacket->sGuildWarMissionGuildRankingInfo[i].wszGuildMasterName;

		m_vecEventGuildRankInfo.push_back( stEventInfo );
	}

	if( m_stPreliminaryResult.bJoinGuildWar )
	{
		for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; i++ )
		{
			stEventInfo.m_nRank = pPacket->sMyGuildWarMissionGuildRankingInfo[i].wRanking;
			stEventInfo.m_nTotalPoints = pPacket->sMyGuildWarMissionGuildRankingInfo[i].nTotalPoint;
			stEventInfo.m_strName1 = strMyGuildName;
			stEventInfo.m_strName2 = GetGuildTask().GetGuildMasterName();

			m_vecEventGuildRankInfo.push_back( stEventInfo );
		}
	}

	for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; i++ )
	{
		stEventInfo.m_nRank = 1;
		stEventInfo.m_nTotalPoints = pPacket->sGuildWarMissionRankingInfo[i].nTotalPoint;
		stEventInfo.m_strName1 = pPacket->sGuildWarMissionRankingInfo[i].wszCharName;
		stEventInfo.m_strName2 = pPacket->sGuildWarMissionRankingInfo[i].wszGuildName;

		m_vecEventPersonalRankInfo.push_back( stEventInfo );
	}

	if( m_stPreliminaryResult.bJoinGuildWar )
	{
		for( int i=0; i<GUILDWAR_RANKINGTYPE_MAX; i++ )
		{
			stEventInfo.m_nRank = pPacket->sMyGuildWarMissionRaningInfo[i].wRanking;
			stEventInfo.m_nTotalPoints = pPacket->sMyGuildWarMissionRaningInfo[i].nTotalPoint;
			stEventInfo.m_strName1 = CDnActor::s_hLocalActor->GetName();
			stEventInfo.m_strName2 = strMyGuildName;

			m_vecEventPersonalRankInfo.push_back( stEventInfo );
		}
	}

	for( int i=0; i<GUILDWAR_DAILY_AWARD_MAX; i++ )
		m_vecDailyBestChar.push_back( pPacket->sGuildWarDailyAward[i].wszCharName );	
	
	GetInterface().SetGuildWarPreliminaryResult();
}

void CDnGuildWarTask::OnRecvGuildWarTournamentInfoMsg( SCGuildTournamentInfo* pPacket )
{
	_ASSERT( pPacket );

	// Clear Data
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; i++ )
	{
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildUID.Reset();
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_strGuildName = L"";
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildSelfView.Reset();
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_bWin = false;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_cMatchTypeCode = 0;
	}

	m_stGuildTournamentInfo.m_cMatchTypeCode = 0;
	m_stGuildTournamentInfo.m_tStartTime = 0;
	m_stGuildTournamentInfo.m_bPopularityVote = false;


	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; i++ )
	{
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildUID = pPacket->TournamentInfo[i].GuildUID;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_strGuildName = pPacket->TournamentInfo[i].wszGuildName;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildSelfView.GuildUID = pPacket->TournamentInfo[i].GuildUID;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildSelfView.wGuildMark = pPacket->TournamentInfo[i].wGuildMark;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildSelfView.wGuildMarkBG = pPacket->TournamentInfo[i].wGuildMarkBG;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_tGuildSelfView.wGuildMarkBorder = pPacket->TournamentInfo[i].wGuildMarkBorder;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_bWin = pPacket->TournamentInfo[i].bWin;
		m_stGuildTournamentInfo.m_TournamentGuild[i].m_cMatchTypeCode = pPacket->TournamentInfo[i].cMatchTypeCode;
	}

	m_stGuildTournamentInfo.m_cMatchTypeCode = pPacket->cMatchTypeCode;
	m_stGuildTournamentInfo.m_tStartTime = pPacket->tStartTime;
	m_stGuildTournamentInfo.m_bPopularityVote = pPacket->bPopularityVote;

	GetInterface().SetGuildWarFinalTournamentList();
}

void CDnGuildWarTask::OnRecvGuildWarModeState( SCPvPOccupationModeState* pPacket )
{
	_ASSERT( pPacket );

	m_nGuildWarState = pPacket->nState;
	if( pPacket->nState == PvPCommon::OccupationSystemState::Wait )	// 최초 진입시 스킬포인트 초기화
	{
		ResetGuildWarSkillPoint();
	}
	else if( pPacket->nState == PvPCommon::OccupationSystemState::End )
	{
		ResetGuildWarSkillPoint();
	}
}

void CDnGuildWarTask::OnRecvGuildWarShopItemInfoList( SCBuyedItemCount* pPacket )
{
	_ASSERT( pPacket );
	m_vecGuildWarShopItemInfo.clear();

	for( int i=0; i<pPacket->count; i++ )
	{
		stGuildWarShopItemInfo sGuildWarShopItemInfo;
		sGuildWarShopItemInfo.m_nItemID = pPacket->items[i].id;
		sGuildWarShopItemInfo.m_nCurrentBuyCount = pPacket->items[i].count;

		m_vecGuildWarShopItemInfo.push_back( sGuildWarShopItemInfo );
	}
}

void CDnGuildWarTask::OnRecvGuildWarShopItemInfo( SCBuyedItemCount* pPacket )
{
	_ASSERT( pPacket );

	for( int i=0; i<pPacket->count; i++ )
	{
		bool bExistItem = false;
		for( int j=0; j<(int)m_vecGuildWarShopItemInfo.size(); j++ )
		{
			if( m_vecGuildWarShopItemInfo[j].m_nItemID == pPacket->items[i].id )
			{
				m_vecGuildWarShopItemInfo[j].m_nCurrentBuyCount = m_vecGuildWarShopItemInfo[j].m_nCurrentBuyCount + pPacket->items[i].count;
				bExistItem = true;
				break;
			}
		}

		if( !bExistItem )
		{
			stGuildWarShopItemInfo sGuildWarShopItemInfo;
			sGuildWarShopItemInfo.m_nItemID = pPacket->items[i].id;
			sGuildWarShopItemInfo.m_nCurrentBuyCount = pPacket->items[i].count;

			m_vecGuildWarShopItemInfo.push_back( sGuildWarShopItemInfo );			
		}
	}
}

int CDnGuildWarTask::GetGuildWarShopItemCurrentBuyCount( int nItemID )
{
	int nItemCurrentBuyCount = 0;
	for( int i=0; i<static_cast<int>( m_vecGuildWarShopItemInfo.size() ); i++ )
	{
		if( m_vecGuildWarShopItemInfo[i].m_nItemID == nItemID )
		{
			nItemCurrentBuyCount = m_vecGuildWarShopItemInfo[i].m_nCurrentBuyCount;
			break;
		}
	}

	return nItemCurrentBuyCount;
}

void CDnGuildWarTask::OnRecvGuildWarFestivalPoint( SCGuildWarFestivalPoint *pPacket )
{
	_ASSERT( pPacket );
	m_biGuildWarFestivalPoint = pPacket->biGuildWarFestivalPoint;
}

void CDnGuildWarTask::OnRecvGuildWarGetReward( SCGuildWarCompensation* pPacket )
{
	_ASSERT( pPacket );
	if( pPacket->nRetCode == ERROR_NONE )
	{
		GetInterface().OpenMovieAlarmDlg( NULL, 2.0f, 0, 0, 0, 126246 );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126246 ), false );		
	}
}

void CDnGuildWarTask::SendGuildWarVote( int nGuildIndex )
{
	ASSERT( nGuildIndex >= 0 && nGuildIndex < GUILDWAR_FINALS_TEAM_MAX );

	if( nGuildIndex < 0 || nGuildIndex >= GUILDWAR_FINALS_TEAM_MAX )
		return;

	CSGuildWarVote stGuildWarVote;
	stGuildWarVote.GuildUID = m_stGuildTournamentInfo.m_TournamentGuild[nGuildIndex].m_tGuildUID;

	CClientSessionManager::GetInstance().SendPacket( CS_GUILD, eGuild::CS_GUILDWAR_VOTE, (char*)&stGuildWarVote, int(sizeof(stGuildWarVote)) );
}

void CDnGuildWarTask::OnRecvGuildWarVote( SCGuildWarVote* pPacket )
{
	_ASSERT( pPacket );
	if( pPacket->nRetCode == ERROR_NONE )
	{
		m_stGuildTournamentInfo.m_bPopularityVote = false;
		CDnGuildWarFinalTournamentListDlg* pDnGuildWarFinalTournamentListDlg = GetInterface().GetGuildWarFinalTournamentList();
		if( pDnGuildWarFinalTournamentListDlg )
			pDnGuildWarFinalTournamentListDlg->SetTournamentUI();

		WCHAR wszTemp[1024] = {0, };
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126249 ), pPacket->nAddFestivalPoint );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
	}
}

void CDnGuildWarTask::OnRecvGuildWarVoteTop( SCGuildWarVoteTop* pPacket )
{
	_ASSERT( pPacket );

	WCHAR wszTemp[1024] = {0, };
	if( pPacket->nVoteCount == 0 )
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126248 ) );
	else
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126247 ), pPacket->wszGuildName, pPacket->nVoteCount );

	GetInterface().MessageBox( wszTemp );
}

void CDnGuildWarTask::OnRecvGuildWarEventTime( SCGuildWarEventTime* pPacket )
{
	if (pPacket == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_GuildWarEventTime.clear();

	SGuildWarEventTime eventTime;
	eventTime.term = pPacket->tGuildWarTime[GUILDWAR_STEP_PREPARATION - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarEventTime.insert(std::make_pair(GUILDWAR_STEP_PREPARATION, eventTime));

	eventTime.term = pPacket->tGuildWarTime[GUILDWAR_STEP_TRIAL - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarEventTime.insert(std::make_pair(GUILDWAR_STEP_TRIAL, eventTime));

	eventTime.term = pPacket->tGuildWarTime[GUILDWAR_STEP_REWARD - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarEventTime.insert(std::make_pair(GUILDWAR_STEP_REWARD, eventTime));

	m_GuildWarFinalRoundTime.clear();

	eventTime.term = pPacket->tFinalPartTime[GUILDWAR_FINALPART_FINAL - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarFinalRoundTime.insert(std::make_pair(GUILDWAR_FINALPART_FINAL, eventTime));


	eventTime.term = pPacket->tFinalPartTime[GUILDWAR_FINALPART_4 - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarFinalRoundTime.insert(std::make_pair(GUILDWAR_FINALPART_4, eventTime));

	eventTime.term = pPacket->tFinalPartTime[GUILDWAR_FINALPART_8 - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarFinalRoundTime.insert(std::make_pair(GUILDWAR_FINALPART_8, eventTime));

	eventTime.term = pPacket->tFinalPartTime[GUILDWAR_FINALPART_16 - 1];
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_START], eventTime.term.tBeginTime);
	DN_INTERFACE::STRING::GetTimeText(eventTime.timeString[GUILDWAR_EVENT_END], eventTime.term.tEndTime);
	m_GuildWarFinalRoundTime.insert(std::make_pair(GUILDWAR_FINALPART_16, eventTime));

	m_CurGuildWarFinalRoundStep = GUILDWAR_FINALPART_NONE;
	m_CurGuildWarStep			= GUILDWAR_STEP_NONE;

	m_bGuildWarFinalProgress = pPacket->bFinalProgress;

}

//////////////////////////////////////////////////////////////////////////
// CDnGuildWarSkill
//////////////////////////////////////////////////////////////////////////

DnSkillHandle CDnGuildWarSkill::CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel )
{
#if defined(_DEBUG)
	if( false == ValidateSkillData( iSkillTableID, iLevel ) )
		return DnSkillHandle();
#endif

	// 레벨 0으로 생성 요청 들어오면 언락만 된 미획득 스킬임.
	bool bAcquired = (iLevel != 0);
	if( false == bAcquired )
		iLevel = 1;		// 레벨 0으로는 생성할 수 없으므로 1로 셋팅

	CDnGuildWarSkill* pNewSkill = NULL;

	// 스킬 발동 조건, 발동 프로세서들을 달아준다.
	// 각각 5개씩 있고 파라메터는 전부 합쳐 10개임. 변경될 가능성도 있다.
	pNewSkill = new CDnGuildWarSkill( hActor );

	// Initialize 
	bool bResult = pNewSkill->Initialize( iSkillTableID, iLevel );
	if( false == bResult )
	{
		SAFE_DELETE( pNewSkill );
		return CDnSkill::Identity();
	}

	//pNewSkill->SetSkillLevelIDOffset( iSkillLevelTableIDOffset );

	if( bAcquired )
	{
		pNewSkill->SetHasActor( hActor );
		pNewSkill->Acquire();
	}

	return pNewSkill->GetMySmartPtr();
}

bool CDnGuildWarSkill::Initialize( int iSkillTableID, int iLevel )
{
	m_iSkillID = iSkillTableID;

	if( false == LoadSkillLevelData( iSkillTableID, iLevel ) )
		return false;

	// 초기화 이후에 따로 모아놓을 정보들.
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );

	// 아이콘 이미지 셋
	SetIconImageIndex( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_IconImageIndex" )->GetInteger() );

	// 초기화했을 땐 PVE 기준으로 초기화 해준다. 디폴트 값.
	m_iSelectedSkillLevelDataApplyType = PVE;
	m_bEnableTooltip = ( pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillTip" )->GetInteger() ) ? true : false;

	return true;
}

bool CDnGuildWarSkill::LoadSkillLevelData( int iSkillTableID, int iLevel )
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );

	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillTableID, vlSkillLevelList ) == 0 ) 
		return false;

	if ( vlSkillLevelList.empty() )
		return false;

	int iSkillLevelDataApplyType = PVP;
	// 해당 대상의 데이터가 없는 경우엔 그냥 아무것도 안하고 리턴하면 된다.
	if( vlSkillLevelList.empty() )
		return true;

	int iSkillLevelTableIDOffset = vlSkillLevelList.front();
	this->SetSkillLevelIDOffset( iSkillLevelTableIDOffset, iSkillLevelDataApplyType );

	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iNowLevel == iLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
	}

	if( -1 == iSkillLevelTableID )
		return false;

#if defined(PRE_FIX_NEXTSKILLINFO)
	//NextLevel이 연속으로 있지 않을 수 있다.. 그래서 여기서 다음 레벨 데이타 테이블 ID를 찾아 놓는다.
	int iMinSkillLevelTableID = -1;
	int iNextSkillLevelTableID = -1;

	int nMinLevel = 9999;	//일단 이정도 크기로 레벨 최대 수치를 정해놓고..
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iTempLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iTempLevel == (iLevel + 1) )
			iNextSkillLevelTableID = vlSkillLevelList.at( i );

		//제일 낮은 스킬 레벨 정보를 설정한다..
		if (nMinLevel > iTempLevel)
		{
			nMinLevel = iTempLevel;
			iMinSkillLevelTableID = vlSkillLevelList.at(i);
		}		
	}
#endif // PRE_FIX_NEXTSKILLINFO

	// 스킬 발동 조건, 발동 프로세서들을 달아준다.
	// 각각 5개씩 있고 파라메터는 전부 합쳐 10개임. 변경될 가능성도 있다.
	char caLabel[ 32 ];
	int iCheckerParamOffset = 0;
	int iProcessorParamOffset = 0;
	for( int i = 0; i < 5; ++i )
	{
		// 발동조건 객체 이름을 찾는다. 파라메터 필드가 비어있으면 생성 함수들에서 NULL 리턴됨
		sprintf_s( caLabel, "_UsableChecker%d", i + 1 );
		int iUsableChecker = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLabel )->GetInteger();

		sprintf_s( caLabel, "_Processor%d", i + 1 );
		int iProcessor = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLabel )->GetInteger();

		int iOffsetCheck = iCheckerParamOffset;
		IDnSkillUsableChecker* pUsableChecker = IDnSkillUsableChecker::Create( m_hActor, iUsableChecker, iSkillLevelTableID, &iCheckerParamOffset );

		if( NULL != pUsableChecker )
		{
			if( (iCheckerParamOffset - iOffsetCheck) != pUsableChecker->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d 의 파라메터 개수가 잘못되었습니다.\n", iSkillLevelTableID );
				_ASSERT( !"스킬 레벨 테이블 파라메터 잘못됨. OutputDebug 출력 확인!" );
			}

			this->AddUsableCheckers( pUsableChecker, iSkillLevelDataApplyType );
		}

		// 클라에서는 사용하지 않음. 지역변수로 그냥 넣어준다.
		set<string> setUseActionName;
		iOffsetCheck = iProcessorParamOffset;
		IDnSkillProcessor* pSkillProcessor = IDnSkillProcessor::Create( m_hActor, iProcessor, iSkillLevelTableID, &iProcessorParamOffset, setUseActionName );

		if( NULL != pSkillProcessor )
		{
			if( (iProcessorParamOffset - iOffsetCheck) != pSkillProcessor->GetNumArgument() )
			{
				OutputDebug( "[SkillLevelTable Error!] %d 의 파라메터 개수가 잘못되었습니다.\n", iSkillLevelTableID );
				_ASSERT( !"스킬 레벨 테이블 파라메터 잘못됨. OutputDebug 출력 확인!" );
			}

			this->AddProcessor( pSkillProcessor, iSkillLevelDataApplyType );
		}
	}

	// skill table
	int iStringID = 0;
	iStringID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NameID" )->GetInteger();
	m_strName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );
	m_strStaticName = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_StaticName" )->GetString();

	char caLable[ 64 ];
	ZeroMemory( caLable, sizeof(caLable) );
	for( int i = 0; i < 2; ++i )
	{
		sprintf_s( caLable,  "_NeedWeaponType%d", i+1 );
		int iEquipType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		if( -1 != iEquipType )
			m_aeNeedEquipType[ i ] = (CDnWeapon::EquipTypeEnum)iEquipType;
		else
			m_aeNeedEquipType[ i ] = CDnWeapon::EquipTypeEnum_Amount;
	}

	//m_iMaxLevel = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_MaxLevel" )->GetInteger();

	// 스킬 테이블의 최대 레벨은 신뢰할 수 없다. -_-
	// 실제 갯수로 업데이트. 이전에 초기화 되어 이미 값이 있다면 값이 다른 경우 assert!!
	if( 0 == m_iMaxLevel )
	{
		m_iMaxLevel = (int)vlSkillLevelList.size();
	}
	else
	{
		_ASSERT( m_iMaxLevel == (int)vlSkillLevelList.size() );
	}

	m_eSkillType = (SkillTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillType" )->GetInteger();
	m_eDurationType = (DurationTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DurationType" )->GetInteger();
	m_eTargetType = (TargetTypeEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_TargetType" )->GetInteger();
	m_iBuffIconImageIndex = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BuffIconImageIndex" )->GetInteger();

	m_iDissolvable = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Dissolvable" )->GetInteger();
	m_iDuplicateCount = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectAmassCount" )->GetInteger();
	m_iSkillDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_SkillDuplicate" )->GetInteger();
	m_iEffectDuplicateMethod = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_EffectDuplicate" )->GetInteger();
	m_iNeedJobClassID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_NeedJob" )->GetInteger();
	//m_eElement = (CDnState::ElementEnum)pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_Element" )->GetInteger();
	if( (CDnState::ElementEnum)-1 == m_eElement )
		m_eElement = CDnState::ElementEnum_Amount;

	m_iUnlockSkillBookItemID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_UnlockSkillBookItemID" )->GetInteger();
	m_iUnlockPrice = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_UnlockPrice" )->GetInteger();
	m_iExclusiveID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_DuplicatedSkillType" )->GetInteger();
	m_iBaseSkillID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BaseSkillID" )->GetInteger();

	// skill level table
	m_iSkillLevelID = iSkillLevelTableID;
	m_iLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();

	if( m_iLevel < m_iMaxLevel )
#if defined(PRE_FIX_NEXTSKILLINFO)
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#else
		m_iNextLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

	else
		m_iNextLevelSkillPoint = 0;

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_iFirstLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iMinSkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#else
	m_iFirstLevelSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID-(m_iLevel-1), "_NeedSkillPoint" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO

	m_iNeedItemID[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedItem" )->GetInteger();
	m_iNeedItemDecreaseCount[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_NeedItemDecreaseCount" )->GetInteger();
	m_iIncreaseRange[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_AddRange" )->GetInteger();
	m_iDecreaseHP[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseHP" )->GetInteger();
	m_iDecreaseMP[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DecreaseSP" )->GetInteger();
	m_iLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_LevelLimit" )->GetInteger();
	m_fDelayTime[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DelayTime" )->GetInteger() / 1000.f;
	m_fOriginalDelayTime[ iSkillLevelDataApplyType ] = m_fDelayTime[ iSkillLevelDataApplyType ];
	m_iCPScore = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_CPScore" )->GetInteger();

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_iFirstLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iMinSkillLevelTableID, "_LevelLimit" )->GetInteger();
#else
	m_iFirstLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID-(m_iLevel-1), "_LevelLimit" )->GetInteger();
#endif // PRE_FIX_NEXTSKILLINFO


	m_fHPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_HPConsumeType" )->GetFloat();
	m_fMPConsumeType[ iSkillLevelDataApplyType ] = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SPConsumeType" )->GetFloat();

	// 스킬 설명의 기본 틀은 안바뀌고 인자값만 바뀔 것이다. 따라서 인자값만 모드별로 나눈다.
	m_iSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

	char *szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
	MakeUIStringUseVariableParam( m_strSkillExplainString[ iSkillLevelDataApplyType ], m_iSkillExplainStringID, szParam );

	m_EffectOutputIDs = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_StateEffectTableID" )->GetString();

	// 스킬 레벨 테이블에서 같은 스킬은 레벨 데이터가 연속해서 있다는 가정 하에.
	if( m_iLevel < m_iMaxLevel )
	{
#if defined(PRE_FIX_NEXTSKILLINFO)
		DNTableCell* pSoxField = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationID" );
		if ( pSoxField )
		{
			m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationID" )->GetInteger();

			szParam = pSkillLevelTable->GetFieldFromLablePtr( iNextSkillLevelTableID, "_SkillExplanationIDParam" )->GetString();
			MakeUIStringUseVariableParam( m_strNextSkillExplainString[iSkillLevelDataApplyType ], m_iNextSkillExplainStringID, szParam );
		}
		else
		{
			OutputDebug("스킬레벨 테이블에서 같은 스킬은 레벨데이터가 연속이 아니네???\n");
			m_iNextSkillExplainStringID = -1;
		}
#else
		DNTableCell* pSoxField = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationID" );
		if ( pSoxField )
		{
			m_iNextSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationID" )->GetInteger();

			szParam = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+1, "_SkillExplanationIDParam" )->GetString();
			MakeUIStringUseVariableParam( m_strNextSkillExplainString[iSkillLevelDataApplyType ], m_iNextSkillExplainStringID, szParam );
		}
		else
		{
			OutputDebug("스킬레벨 테이블에서 같은 스킬은 레벨데이터가 연속이 아니네???\n");
			m_iNextSkillExplainStringID = -1;
		}
#endif // PRE_FIX_NEXTSKILLINFO
	}
	else
		m_iNextSkillExplainStringID = -1;

	// 상태 효과 정보 로딩
	StateEffectStruct StateEffect;
	for( int i = 0; i < 5; ++i ) 
	{
		sprintf_s( caLable, "_EffectClass%d", i + 1 );
		StateEffect.nID = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();

		if( StateEffect.nID < 1 ) 
			continue;

		sprintf_s( caLable, "_EffectClass%dApplyType", i + 1 );
		int iApplyType = pSkillTable->GetFieldFromLablePtr( iSkillTableID, caLable )->GetInteger();
		bool bApplyAll = (StateEffectApplyType::ApplyAll == iApplyType);		// 모두 적용임. 타겟만 다르게 해서 똑같은 상태효과 2개를 추가해준다.

		if( bApplyAll )
			StateEffect.ApplyType = StateEffectApplyType::ApplySelf;
		else
			StateEffect.ApplyType = (StateEffectApplyType)iApplyType;

		sprintf_s( caLable, "_EffectClassValue%d", i + 1 );
		StateEffect.szValue = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetString();

		sprintf_s( caLable, "_EffectClassValue%dDuration", i + 1 );
		StateEffect.nDurationTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, caLable )->GetInteger();

		m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );

		// 모두 적용이면 타겟으로 바꿔서 똑같이 한 번 더 넣어줌.
		if( bApplyAll )
		{
			StateEffect.ApplyType = StateEffectApplyType::ApplyTarget;
			m_vlStateEffectList[ iSkillLevelDataApplyType ].push_back( StateEffect );
		}
	}

	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillID = m_iSkillID;
	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillLevelID = m_iSkillLevelID;
	m_SkillInfo[ iSkillLevelDataApplyType ].eDurationType = m_eDurationType;
	m_SkillInfo[ iSkillLevelDataApplyType ].eSkillType = m_eSkillType;
	m_SkillInfo[ iSkillLevelDataApplyType ].iSkillBufIcon = pSkillTable->GetFieldFromLablePtr( iSkillTableID, "_BuffIconImageIndex" )->GetInteger();
	m_SkillInfo[ iSkillLevelDataApplyType ].effectOutputIDs = m_EffectOutputIDs;
	m_SkillInfo[ iSkillLevelDataApplyType ].iBuffIconImageIndex = m_iBuffIconImageIndex;
	m_SkillInfo[ iSkillLevelDataApplyType ].eTargetType = m_eTargetType;
	if( m_hActor )
		m_SkillInfo[ iSkillLevelDataApplyType ].iSkillUserTeam = m_hActor->GetTeam();

#if defined(PRE_FIX_NEXTSKILLINFO)
	m_SkillInfo[ iSkillLevelDataApplyType ].nSkillLevel = m_iLevel;
#endif // PRE_FIX_NEXTSKILLINFO

	UpdateRangeChecker( iSkillLevelDataApplyType );

	RefreshDecreaseMP( iSkillLevelDataApplyType );

	if( 0.0f == m_fHPConsumeType[ iSkillLevelDataApplyType ] )
	{
		m_iNeedHP[ iSkillLevelDataApplyType ] = m_iDecreaseHP[ iSkillLevelDataApplyType ];
	}
	else
	{
		if( m_hActor )
			m_iNeedHP[ iSkillLevelDataApplyType ] = int((float)m_hActor->GetMaxHP() * m_fHPConsumeType[ iSkillLevelDataApplyType ]);
	}

	return true;
}

MIInventoryItem::ItemConditionEnum CDnGuildWarSkill::GetItemCondition()
{
	MIInventoryItem::ItemConditionEnum eItemCondition =  ItemConditionEnum::Usable;

	if( m_hActor )
	{
		if( m_hActor->IsDie() ) 
			return ItemConditionEnum::Unusable;

		// 플레이어가 획득하지 않은 스킬은 사용 불가.
		if( !IsAcquired() )
			return ItemConditionEnum::Unusable;
	}

	if( CanExecuteGuildWarSkill() == CDnSkill::UsingResult::Failed )
		return ItemConditionEnum::Unusable;

	bool bCheckResource = false;
	if( CDnOccupationTask::IsActive() )
	{
		DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );
		if( pSkillLevelTable == NULL )
			return eItemCondition;
		int nItemID = pSkillLevelTable->GetItemIDFromField( "_SkillIndex", GetClassID() );
		int nNeedResource = pSkillLevelTable->GetFieldFromLablePtr( nItemID, "_DecreaseTIC" )->GetInteger();

		int nCurrentResource = GetOccupationTask().GetResource( (PvPCommon::Team::eTeam)CDnActor::s_hLocalActor->GetTeam() ); 
		if( nCurrentResource >= nNeedResource )	// 사용 가능한 자원이 있는지 검사
			bCheckResource = true;
	}
	if( !bCheckResource )
		eItemCondition = ItemConditionEnum::Unusable;

	return eItemCondition;
}

CDnSkill::UsingResult CDnGuildWarSkill::CanExecuteGuildWarSkill()
{
	UsingResult eResult = UsingResult::Failed;

	if( !m_hActor )
		return UsingResult::Failed;

	// 대포 모드일 땐 플레이어는 스킬 사용 못한다.
	if( m_hActor->IsPlayerActor() )
	{
		if( static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->IsCannonMode() )
			return UsingResult::Failed;
	}

	// 잔여 SP 체크, 체력 체크, 레벨 체크 등등
	// 오라나 토글은 현재 활성화 중이라면 MP 상관 없이 끌 수 있다.
	if( !IsToggleOn() && !IsAuraOn() )
		if( m_hActor->GetSP() < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// 워리어의 릴리브같은 스킬은 수면, 스턴 중일때도 사용가능해야 한다.
	// 테이블에 공격불가 무시 발현타입을 추가할까하다가 현재 릴리브에서만 의미있는 것이므로
	// 다른 곳에서 어떤 식으로 쓰이게 될지 좀 더 지켜보고 규격화 시키도록 한다.
	// 우선 Dissolve 상태효과 있는 스킬은 행동 불가 체크를 건너뛴다.
	// 스킬에 해당하는 액션의 State 시그널에 반드시 IgnorectCantAction 이 켜져 있어야 스킬 액션이 나간다.
	bool bPassCheckCantAction = false;
	for( DWORD i = 0; i < GetStateEffectCount(); ++i )
	{
		CDnSkill::StateEffectStruct* pSE = GetStateEffectFromIndex( i );
		if( STATE_BLOW::BLOW_069 == pSE->nID )
		{
			bPassCheckCantAction = true;
			break;
		}
	}

	if( false == bPassCheckCantAction )
	{
		if( 0 < m_hActor->GetCantActionSEReferenceCount() )
			return UsingResult::Failed;
	}

	// 스킬 사용불가 상태효과가 있으면 mp 소모하는 스킬은 사용할 수 없다.
	if( 0 < m_hActor->GetCantUseSkillSEReferenceCount() )
		if( 0 < m_iNeedMP[ m_iSelectedSkillLevelDataApplyType ] )
			return UsingResult::Failed;

	// HP
	if( m_hActor->GetHP() < m_iNeedHP[ m_iSelectedSkillLevelDataApplyType ] )
		return UsingResult::Failed;

	if( m_hActor->GetLevel() < m_iLevelLimit )
		return UsingResult::Failed;

	// 필요 무기가 있다면 장착했는지 확인. 최대 2개임. 둘 중 하나만 충족되도 스킬 사용 가능.
	if( false == IsSatisfyWeapon() )
		return UsingResult::Failed;

	// 직업 체크. 
	// 현재 마을에서는 시야에 들어온 유저의 직업 풀 히스토리가 있지 않고 최근직업만 셋팅되므로 
	// 마을에서 만난 다른 유저들의 직업까지는 체크하지 않습니다.
	// 던전에서는 파티원의 풀 히스토리를 받아오기 때문에 모두 체크함.
	bool bNeedCheckJob = true;
	if( CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType() )
		if( NULL == dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer()) )
			bNeedCheckJob = false;

	if( bNeedCheckJob && m_hActor->GetClassID() <= CDnActor::Reserved6  )
	{
		CDnPlayerActor* pActor = dynamic_cast<CDnPlayerActor*>(m_hActor.GetPointer());
		if( NULL == pActor )
			return UsingResult::Failed;
		if( 0 != m_iNeedJobClassID )
		{
			if( pActor->IsPassJob( m_iNeedJobClassID ) == false ) 
				return UsingResult::Failed;
		}

		// 죽었으면 불가
		if( m_hActor->IsDie() ) 
			return UsingResult::Failed;
		// 배틀 모드가 아니면 스킬 발동 불가!
		if( !pActor->IsBattleMode() )
		{
			if( !CDnSkillTask::IsActive() || !GetSkillTask().IsGuildWarSkill( GetClassID() ) )
				return UsingResult::Failed;
		}
	}

	// 쿨타임이 끝나지 않았다면 스킬 발동 불가
	switch( m_eDurationType )
	{
		case Instantly:
		case Buff:
		case Debuff:
		case SummonOnOff:
			{
				if( m_fCoolTime > 0.0f )
					return UsingResult::FailedByCooltime;
			}
			break;

		case TimeToggle: 
			break;

		case ActiveToggle: 
		case ActiveToggleForSummon:
			break;

		case Aura: 
			break;
	}

	if( m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].empty() )
		eResult = UsingResult::Success;
	else
	{
		int iNumChecker = (int)m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].size();

		for( int iChecker = 0; iChecker < iNumChecker; ++iChecker )
		{
			IDnSkillUsableChecker* pChecker = m_vlpUsableCheckers[ m_iSelectedSkillLevelDataApplyType ].at( iChecker );

			if( static_cast<CDnPlayerActor*>(m_hActor.GetPointer())->IsVehicleMode() && pChecker->GetType() == IDnSkillUsableChecker::GROUNDMOVABLE_CHECKER )
				eResult = UsingResult::Success;
			else
			{
				if( false == pChecker->CanUse() )
				{
					eResult = UsingResult::FailedByUsableChecker;
					break;
				}
				else
					eResult = UsingResult::Success;
			}
		}
	}

	return eResult;
}

void CDnGuildWarTask::ResetGuildWarSkillPoint()
{
	m_nGuildWarSkillUsedPoint = 0;
	m_nGuildWarSkillLeavePoint = MAX_GUILD_WAR_SKILL_POINT;
	GetSkillTask().LoadGuildWarSkillList( (PvPCommon::Team::eTeam)CDnActor::s_hLocalActor->GetTeam() );
}


#if defined(PRE_FIX_NEXTSKILLINFO)
SKILL_LEVEL_INFO CDnGuildWarTask::ms_SkillLevelTableInfo;
SKILL_LEVEL_INFO* CDnGuildWarTask::GetSkillLevelTableIDList(int nSkillID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return &ms_SkillLevelTableInfo;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
		return &findIter->second;

	return &ms_SkillLevelTableInfo;
}

void CDnGuildWarTask::InitSkillLevelTableIDList()
{
	DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILL );
	DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TBATTLEGROUNDSKILLLEVEL );

	if (pSkillTable == NULL || pSkillLevelTable == NULL)
		return;

	int nSkillCount = pSkillTable->GetItemCount();
	for (int i = 0; i < nSkillCount; ++i)
	{
		int nSkillID = pSkillTable->GetItemID(i);

		//int nSkillID = pSkillTable->GetFieldFromLablePtr(nItemID, "id")->GetInteger();

		std::vector<int> vlSkillLevelList;
		pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList );

		// pve, pvp 대상인지 확인하여 걸러냄.
		vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
		for( iterLevelList; iterLevelList != vlSkillLevelList.end(); ++iterLevelList)
		{
			int iSkillLevelTableID = *iterLevelList;
			int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
			int iSkillLevel = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_SkillLevel" )->GetInteger();

			AddSkillLevelTableID(nSkillID, iSkillLevel, iSkillLevelTableID, iApplyType);
		}
	}
}

void CDnGuildWarTask::AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType)
{
	if (nApplyType >= 2 || nApplyType < 0)
		return;

	SKILL_LEVEL_TABLEID_LIST::iterator findIter = m_SkillLevelTableIDList[nApplyType].find(nSkillID);
	if (findIter != m_SkillLevelTableIDList[nApplyType].end())
	{
		//기존 리스트가 존재 하면 그 리스트에 추가 한다..
		findIter->second.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);
	}
	else
	{
		SKILL_LEVEL_INFO skillLevelInfo;
		skillLevelInfo.AddSkillLevelTableID(nSkillLevel, nSkillLevelTableID);

		m_SkillLevelTableIDList[nApplyType].insert(std::make_pair(nSkillID, skillLevelInfo));
	}
}
#endif // PRE_FIX_NEXTSKILLINFO
