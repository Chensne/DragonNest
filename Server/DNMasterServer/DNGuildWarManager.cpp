#include "StdAfx.h"
#include "Util.h"
#include "DNGuildWarManager.h"
#include "DNDivisionManager.h"
#include "DNVillageConnection.h"
#include "MtRandom.h"
#include "TimeSet.h"
#include "DNExtManager.h"
#include "DNPvP.h"

CDNGuildWarManager * g_pGuildWarManager = NULL;

extern TMasterConfig g_Config;

CDNGuildWarManager::CDNGuildWarManager()
{	
	memset (m_pWarEventStep, 0x00, sizeof(m_pWarEventStep));
	m_pWarEventStep[GUILDWAR_STEP_PREPARATION]	= new CDNGuildWarPreparation;
	m_pWarEventStep[GUILDWAR_STEP_TRIAL]		= new CDNGuildWarTrial;
	m_pWarEventStep[GUILDWAR_STEP_REWARD]		= new CDNGuildWarReward;	
	Clear();
}

void CDNGuildWarManager::Clear()
{
	// 스텝 초기화
	ResetStep();	

	m_bSendGuildWarInfo = false;
	m_eSettingStep = REQ_NONE;

	m_wWinersWeightRate = 0;
	m_wScheduleID = GUILDWARSCHEDULE_DEF;
	m_TickCheckGuildWar = 0;

	memset(&m_GuildWarFinalInfo, 0, sizeof(m_GuildWarFinalInfo));
	memset(&m_sGuildWarSchedule, 0, sizeof(m_sGuildWarSchedule));
	memset(&m_sGuildWarFinalSchedule, 0, sizeof(m_sGuildWarFinalSchedule));
	memset(&m_bTournamentGroup, 0, sizeof(m_bTournamentGroup));
	memset(&m_GuildWarOpeningPoints, 0, sizeof(m_GuildWarOpeningPoints));
	m_nFinalTeamCount = 0;
	m_bFinalTeamSetting = false;

	m_cSecretTeam = 0;

	m_nSecretRandomSeed = 0;

	m_bCheatFlag = false;

	m_nBlueTeamPoint = 0;
	m_nRedTeamPoint = 0;
	m_dwPreWinSkillCoolTime = 0;
	m_bFinalWinGuild = false;
	m_bResetSchedule = false;
	m_bFinalStart = false;

	memset(&m_sGuildWarPointTrialRanking, 0, sizeof(m_sGuildWarPointTrialRanking));
	m_wPreWinScheduleID = 0;	
	m_bFrinalProgress = false;
	m_tRewardExpireDate = 0;
	m_PreWinGuildUID.Reset();
#if defined(PRE_FIX_75807)	
	m_bFarmForceHarbest = false;
#endif //#if defined(PRE_FIX_75807)
}

CDNGuildWarManager::~CDNGuildWarManager()
{	
	CDNGuildWar** pWarEventStep = m_pWarEventStep;
	for (int j=GUILDWAR_STEP_NONE; j<GUILDWAR_STEP_END; j++)
		if (pWarEventStep[j]) SAFE_DELETE(pWarEventStep[j])	
}

bool CDNGuildWarManager::LoadScheduleInfo(const VIMASetGuildWarSchedule* pData)
{
	// 이미 종료된 이벤트 정보인지 확인한다
	time_t tCurrentTime;
	time(&tCurrentTime);

	if (pData->bForce)
	{
		m_bWarEvent = false;
		m_bCheatFlag = true;
	}
	if (!pData->bForce && tCurrentTime > pData->EventInfo[GUILDWAR_STEP_REWARD].tEndTime)
	{
		// 이미 지난 이벤트이거나 셋팅이 안된거임
		//g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadScheduleInfo Failed !!! TimeOut or Schedule empty\n");		
#if defined(PRE_FIX_75807)
		// 여기서 혹시 지난 길드전 우승자의 값이 현재를 안넘었는지 본다.
		if( pData->tRewardExpireDate > 0 && !GetPreWinGuild().IsSet() )
		{			
			__time64_t tCurrentTime;
			time(&tCurrentTime);
			if( tCurrentTime < pData->tRewardExpireDate )
			{
				m_tRewardExpireDate = pData->tRewardExpireDate;	
				// 아직 보상 기간이 남아 있으면 우승길드 뽑아오기
				m_eSettingStep = REQ_PRE_WIN;
				SendGuildWarInfoReq(); // 본선 스케쥴 정보 가져오기 요청
				g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Send GetPrewinGuild Schedule Empty!!!\n");
				return true;
			}
		}
		else if( pData->tRewardExpireDate == 0 && !m_bFarmForceHarbest )		
		{
			// 여기서 게임서버 우승농장 초기화 진행
			MAGuildWarPreWinGuild GuildWarPreWinGuild;
			GuildWarPreWinGuild.GuildUID.Reset();
			GuildWarPreWinGuild.bPreWin = false;
			m_bFarmForceHarbest = true;
			g_pDivisionManager->SendSetGuildWarPreWinGuildGameServer(&GuildWarPreWinGuild);			
		}
		m_bSendGuildWarInfo = false;
#endif //#if defined(PRE_FIX_75807)
		return true;
	}
	if( pData->tRewardExpireDate > 0)
		m_tRewardExpireDate = pData->tRewardExpireDate;	

	//////////////////////////////////////////////////////////////////////////////////////////////////
	bool bResult = false;
	m_wWinersWeightRate	= pData->wWinersWeightRate;

	memcpy(m_sGuildWarSchedule, pData->EventInfo, sizeof(m_sGuildWarSchedule));

	// 이벤트 진행중이면
	if (m_bWarEvent)
	{
		// 동일차수에 한해 기간 업데이트 가능
		if (m_wScheduleID != pData->wScheduleID)
		{
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadScheduleInfo Failed !!! Wrong ScheduleID Old(%d)<->New(%d) \n", m_wScheduleID, pData->wScheduleID);
			return false;
		}
		else
		{
			bResult = UpdateScheduleGroup(pData->wScheduleID, pData->EventInfo);
		}
	}
	else // 이벤트 진행중이 아니라면 신규 등록
	{
		bResult = RegisterScheduleGroup(pData->wScheduleID, pData->EventInfo);
	}

	if (bResult)
	{
		// 등록을 하든 업데이트를 하든 여기서 정보를 새로 업데이트한다.		
		m_wScheduleID = pData->wScheduleID;		
		
		m_eSettingStep = REQ_FINAL_SCHEDULE;
		SendGuildWarInfoReq(); // 본선 스케쥴 정보 가져오기 요청
		SetFinalProgress(pData->bFinalProgress);
		return true;
	}	
	return false;	
}

bool CDNGuildWarManager::LoadFinalScheduleInfo(MASetGuildWarFinalSchedule* pData)
{	
	__time64_t tCurrentTime;
	time(&tCurrentTime);

	//////////////////////////////////////////////////////////////////////////////////////////////////	
	__time64_t tCheckPeriod = 0;
	// 이미 본선이 진행중이며 스케쥴이 셋팅되어 있는지 체크..

	// 각 시간체크
	for( int i=GUILDWAR_FINALPART_MAX-1; i>GUILDWAR_FINALPART_NONE; --i)
	{
		if( tCheckPeriod > pData->GuildWarFinalSchedule[i].tBeginTime || pData->GuildWarFinalSchedule[i].tBeginTime > pData->GuildWarFinalSchedule[i].tEndTime )
		{			
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadFinalScheduleInfo Failed !!! Wrong Time Schedule Begin(%d) End(%d) Check(%d)\n"
				, pData->GuildWarFinalSchedule[i].tBeginTime, pData->GuildWarFinalSchedule[i].tEndTime, tCheckPeriod);
			return false;
		}
		tCheckPeriod = pData->GuildWarFinalSchedule[i].tBeginTime;		
	}
	CDNGuildWarReward* pGuildWarReward = (CDNGuildWarReward*)m_pWarEventStep[GUILDWAR_STEP_REWARD];
	if( !pGuildWarReward )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadFinalScheduleInfo Failed !!! CDNGuildWarReward Null\n");
		return false;
	}
	memcpy(m_sGuildWarFinalSchedule, pData->GuildWarFinalSchedule, sizeof(m_sGuildWarFinalSchedule));
	pGuildWarReward->SetFinalScheduleInfo(pData);

	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadFinalScheduleInfo ScheduleID:%d\n", m_wScheduleID);
	
	// 본선 스케쥴 돌아라..
	StartStep();

	// 전체 스케쥴 보내주기
	MASetGuildWarEventTime SetGuildWarEventTime;
	memset(&SetGuildWarEventTime, 0, sizeof(SetGuildWarEventTime));
	memcpy(SetGuildWarEventTime.sGuildWarTime, m_sGuildWarSchedule, sizeof(SetGuildWarEventTime.sGuildWarTime));
	memcpy(SetGuildWarEventTime.sFinalPartTime, m_sGuildWarFinalSchedule, sizeof(SetGuildWarEventTime.sFinalPartTime));

	SetGuildWarEventTime.bFinalProgress = GetFinalProgress();
	g_pDivisionManager->SendSetGuildWarSchedule(&SetGuildWarEventTime);

	if( (m_cStepIndex != GUILDWAR_STEP_REWARD || !GetFinalStart() ) && m_wScheduleID != 1)  // 맨 처음 차수가 아니면.
	{
		// 본선이 아니거나 아직 시작을 안했으면 지난 차수 우승팀 가져오기..
		m_eSettingStep = REQ_PRE_WIN;
		SendGuildWarInfoReq();
		return true;
	}	
	else if( m_cStepIndex == GUILDWAR_STEP_TRIAL || m_cStepIndex == GUILDWAR_STEP_REWARD ) 
	{
		// 예선이거나 본선이면 청팀, 홍팀 포인트 가져오기
		m_eSettingStep = REQ_TEAM_POINT;
		SendGuildWarInfoReq();
		return true;
	}
	m_bSendGuildWarInfo = false;
	m_eSettingStep = REQ_ALL_COMPLETE;
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadFinalScheduleInfo REQ_ALL_COMPLETE !!!\n");
	return true;
}

void CDNGuildWarManager::SendGuildWarInfoReq()
{
	// 콜로세움 빌리지한테 요청
	CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillageConnection == NULL || pVillageConnection->GetActive() == false )
		return;

	if( pVillageConnection )
	{
		switch(m_eSettingStep)
		{
		case REQ_NONE :
		case REQ_SCHEDULE :
			{
				pVillageConnection->SendGetGuildWarSchedule(); // 스케쥴 정보 가져오기 요청				
				m_eSettingStep = REQ_SCHEDULE;
			}
			break;
		case REQ_FINAL_SCHEDULE :
			{
				pVillageConnection->SendGetGuildWarFinalSchedule(m_wScheduleID); // 본선 스케쥴 정보 가져오기 요청				
			}
			break;		
		case REQ_PRE_WIN :
			{
				pVillageConnection->SendGetGuildWarPreWinGuild();	// 우승팀 가져오기
			}
			break;
		case REQ_TEAM_POINT :
			{
				pVillageConnection->SendGetGuildWarPoint(); // 청팀, 홍팀 포인트 가져오기				
			}
			break;		
		}
		m_bSendGuildWarInfo = true;
		m_TickCheckGuildWar = timeGetTime();
	}
}

void CDNGuildWarManager::SetGuildWarPoint(int nBlueTeam, int nRedTeam)
{
	InterlockedExchange(&m_nBlueTeamPoint, nBlueTeam);
	InterlockedExchange(&m_nRedTeamPoint, nRedTeam);

	CalcTeamSecret();

	m_bSendGuildWarInfo = false;
	m_eSettingStep = REQ_ALL_COMPLETE;
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetGuildWarPoint REQ_ALL_COMPLETE !!!\n");
}

void CDNGuildWarManager::AddGuildWarPoint(char cTeamType, int nAddPoint)
{
	if( cTeamType == GUILDWAR_TEAM_BLUE) 	
		InterlockedExchangeAdd(&m_nBlueTeamPoint, nAddPoint);
	else if( cTeamType == GUILDWAR_TEAM_RED )
		InterlockedExchangeAdd(&m_nRedTeamPoint, nAddPoint);

	CalcTeamSecret();
}

void CDNGuildWarManager::SetGuildWarPreWinGuild(TGuildUID GuildUID)
{
	m_PreWinGuildUID = GuildUID;

	if( m_cStepIndex == GUILDWAR_STEP_TRIAL || m_cStepIndex == GUILDWAR_STEP_REWARD ) 
	{
		// 예선이거나 본선이면 전이면 청팀, 홍팀 포인트 가져오기
		m_eSettingStep = REQ_TEAM_POINT;
		SendGuildWarInfoReq();
		return;
	}
	m_bSendGuildWarInfo = false;
	m_eSettingStep = REQ_ALL_COMPLETE;
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetGuildWarPreWinGuild REQ_ALL_COMPLETE !!!\n");
}

UINT CDNGuildWarManager::GetGuildDBIDWithFinal(const UINT uiPvPIndex, const WCHAR * pGuildName)
{
	if (pGuildName == NULL)
		return 0;

	for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i)
	{
		if( m_bTournamentGroup[i].ATeam.nTeamNum > 0 && m_bTournamentGroup[i].BTeam.nTeamNum > 0)
		{
			if (wcscmp(m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].wszGuildName, pGuildName) == 0 && m_bTournamentGroup[i].unPvPIndex == uiPvPIndex)
				return m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].GuildUID.nDBID;

			if (wcscmp(m_GuildWarFinalInfo[m_bTournamentGroup[i].BTeam.nTeamNum-1].wszGuildName, pGuildName) == 0 && m_bTournamentGroup[i].unPvPIndex == uiPvPIndex)
				return m_GuildWarFinalInfo[m_bTournamentGroup[i].BTeam.nTeamNum-1].GuildUID.nDBID;
		}
	}

	return 0;
}

bool CDNGuildWarManager::IsTrialStats()
{	
	if( m_cStepIndex == GUILDWAR_STEP_REWARD)
	{
		CDNGuildWarReward* pReward = (CDNGuildWarReward*)m_pWarEventStep[m_cStepIndex];
		return pReward->m_bTrialStatsRequest;		
	}
	return false;
}

void CDNGuildWarManager::SetTrialStats()
{
	CDNGuildWarReward* pReward = (CDNGuildWarReward*)m_pWarEventStep[GUILDWAR_STEP_REWARD];
	pReward->m_bTrialStatsRequest = true;
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetTrialStats!!!\n");
}

void CDNGuildWarManager::CalcGuildWarTournament(VIMASetGuildWarFinalTeam* pData)
{	
	// 자리는 고정입니다요...자세한건 [DN]길드전_본선대진표배치_1_.pptx 참조
	int MatchSequence[]={0,8,14,6,4,12,10,2,3,11,13,5,7,15,9,1};
	
	sTournamentGroup Groups[GUILDWAR_TOURNAMENT_GROUP_MAX];
	memset(Groups, 0, sizeof(Groups));

	for( int i=0; i<pData->nCount; ++i )
	{
		int nGroupIndex = MatchSequence[i]/2;
		if(  MatchSequence[i]%2 ) 			
			Groups[nGroupIndex].BTeam.nTeamNum = i+1;
		else
			Groups[nGroupIndex].ATeam.nTeamNum = i+1;
	}

	for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i )
	{
		if( Groups[i].ATeam.nTeamNum )
		{
			//m_bTournamentGroup[i].ATeam.nTeamNum = (i*2)+1;
			//m_bTournamentGroup[i].ATeam.nOpeningPoint = pData->nTotalPoint[Groups[i].ATeam.nTeamNum-1];
			m_GuildWarFinalInfo[i*2].GuildUID = pData->GuidUID[Groups[i].ATeam.nTeamNum-1];
			m_GuildWarOpeningPoints[i*2] = pData->nTotalPoint[Groups[i].ATeam.nTeamNum-1];			
			memcpy(m_GuildWarFinalInfo[i*2].wszGuildName, pData->wszGuildName[Groups[i].ATeam.nTeamNum-1], sizeof(m_GuildWarFinalInfo[Groups[i].ATeam.nTeamNum-1].wszGuildName));			
		}
		if( Groups[i].BTeam.nTeamNum )
		{
			//m_bTournamentGroup[i].BTeam.nTeamNum = (i*2)+2;
			//m_bTournamentGroup[i].BTeam.nOpeningPoint = pData->nTotalPoint[Groups[i].BTeam.nTeamNum-1];
			m_GuildWarFinalInfo[(i*2)+1].GuildUID = pData->GuidUID[Groups[i].BTeam.nTeamNum-1];
			m_GuildWarOpeningPoints[(i*2)+1] = pData->nTotalPoint[Groups[i].BTeam.nTeamNum-1];			
			memcpy(m_GuildWarFinalInfo[(i*2)+1].wszGuildName, pData->wszGuildName[Groups[i].BTeam.nTeamNum-1], sizeof(m_GuildWarFinalInfo[Groups[i].BTeam.nTeamNum-1].wszGuildName));			
		}
	}
	m_nFinalTeamCount = pData->nCount;
	SetGuildWarTournamentGroup();
	m_bFinalTeamSetting = true;

	// 여기서 길드 정보 업데이트
	MAChangeGuildInfo ChangeGuildInfo;
	memset(&ChangeGuildInfo, 0, sizeof(ChangeGuildInfo));
	// 본선 진출 횟수 1회 추가~~
	ChangeGuildInfo.btGuildUpdate = GUILDUPDATE_TYPE_GUILDWAR;
	ChangeGuildInfo.Int1 = 1;
	for(int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( !m_GuildWarFinalInfo[i].GuildUID.IsSet() )
			continue;
		ChangeGuildInfo.GuildUID = m_GuildWarFinalInfo[i].GuildUID;
		g_pDivisionManager->SendChangeGuildInfo(&ChangeGuildInfo);
	}	
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] CalcGuildWarTournament Success TeamCount:%d !!!\n", m_nFinalTeamCount);
}

void CDNGuildWarManager::DoUpdate(DWORD CurTick)
{	
	// Reset 확인
	if ( m_bResetSchedule )
	{		 
		if( g_pGuildWarManager->GetFinalProgress() && g_pGuildWarManager->GetFinalStart() )
		{
			// 본선 진행중이면..이때는 강제 중단으로 여기고 모든 길드전 강제 종료..
			g_pDivisionManager->SendGuildWarAllStop();			
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Emergence GuilWar Stop Process!!!\n");
		}
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Reset Schedule GuilWar Progress:%d, Start:%d\n",g_pGuildWarManager->GetFinalProgress(),g_pGuildWarManager->GetFinalStart() );
		Clear(); // 전부 초기화하고
		SendGuildWarInfoReq(); // 다시 첨부터 로딩 ㄱㄱ
		return;
	}

	//우승 길드가 셋팅되어 있으면 보상이 지급됐는지 가져오자..대략 1분 마다 한번씩..	
	DWORD dwTick = timeGetTime();
	static DWORD SendTick = 60*1000;
	if( GetPreWinGuild().IsSet() && !GetFinalWinGuildReward() && SendTick+(60*1000) < dwTick)
	{
		//CDNVillageConnection *pVillageConnection = g_pDivisionManager->GetFirstEnableVillageServer();	
		CDNVillageConnection* pVillageConnection = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
		if( pVillageConnection )
		{
			pVillageConnection->SendGetGuildWarPreWindGuildReward(GetPreWinScheduleID(), GetPreWinGuild().nDBID);
		}
		SendTick = dwTick;
	}

	if ( m_eSettingStep != REQ_ALL_COMPLETE && (!m_bSendGuildWarInfo
		|| (m_bSendGuildWarInfo && CurTick > m_TickCheckGuildWar+REQ_GUILDWARINFO_TICK_MAX)) )  // 활성화 되어 있지 않고 빌리지에게 정보 요청하지 않은 상태거나 타임오바이면
	{		
		SendGuildWarInfoReq();
		return;
	}
	// 우승길드 해제..제스쳐, 농장	
	if( m_PreWinGuildUID.IsSet() && m_bFinalWinGuild && m_tRewardExpireDate > 0 )
	{
		__time64_t tCurrentTime;
		time(&tCurrentTime);
		if( tCurrentTime > m_tRewardExpireDate )
		{
			// 우승길드 해제			
			MAGuildWarPreWinGuild pPacket;
			memset(&pPacket, 0, sizeof(MAGuildWarPreWinGuild));
			pPacket.GuildUID = GetPreWinGuild();
			pPacket.bPreWin = false;
			g_pDivisionManager->SendSetGuildWarPreWinGuild(&pPacket); // 전체 게임 및 빌리지에 통보.

			m_PreWinGuildUID.Reset();
			m_tRewardExpireDate = 0;
			g_pGuildWarManager->SetFinalWinGuild(false);
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] PreWinGuild ExpireDate!!!\n");
		}
	}

	if (m_bWarEvent && CheckStep(m_cStepIndex) && m_eSettingStep == REQ_ALL_COMPLETE)
	{
		CDNGuildWar** pWarEventStep = m_pWarEventStep;

		CDNGuildWar* pCurrentEvent = pWarEventStep[m_cStepIndex];

		if (pCurrentEvent)
		{
			// 해당 이벤트객체를 호출하여 처리!
			pCurrentEvent->Process();

			// 모든 이벤트가 끝났을때 다음스텝으로 진행
			if (pCurrentEvent->IsFinishPeriod())
				NextStep();
		}
	}

	// 본선 진행일때만..
	if( GetFinalProgress() ) 
	{
		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
		if( pVillageCon == NULL || pVillageCon->GetActive() == false )
			return;	//이거 어케해야함?	

		// 본선이고 방이 만들어진 애들은 시간이 되면 스타트 날려주자..
		CDNPvP* pPvP = NULL;
		VIMAPVP_START Packet;
		memset(&Packet, 0, sizeof(Packet));
		Packet.sCSPVP_START.unCheck = PvPCommon::Check::AllCheck;

		for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i )
		{
			if( m_bTournamentGroup[i].ATeam.nTeamNum && m_bTournamentGroup[i].BTeam.nTeamNum 
				&& m_bTournamentGroup[i].unPvPIndex && m_bTournamentGroup[i].dwStartTick > 0)
			{
				if( CurTick > m_bTournamentGroup[i].dwStartTick+GUILDWAR_FINAL_ROOMSTRAT_TICK_MAX )
				{
					if( g_pDivisionManager->GetGameConnectionCount() <= 0 )
					{				
						g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] StartPvP Failed !!! Not GameConnection \n");
						return;
					}

					pPvP = g_pDivisionManager->GetPvPRoomByIdx(m_bTournamentGroup[i].unPvPIndex);
					if( pPvP )
					{
						short wRet = pPvP->StartPvP(pVillageCon, &Packet);
						if( wRet != ERROR_NONE )
							g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] StartPvP Failed !!! %d \n", wRet);
						else
						{
							g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] StartPvP Success PvPIndex:%u !!! \n", m_bTournamentGroup[i].unPvPIndex);
							m_bTournamentGroup[i].dwStartTick = 0;							
						}
					}
				}
			}
		}
	}	
}

CDNGuildWar* CDNGuildWarManager::GetWarEvent(char cStep)
{	
	if (!CheckStep(cStep))
		return NULL;

	return m_pWarEventStep[cStep];
}

int CDNGuildWarManager::SetGuildTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo)
{
	// 카운트 셀려고 루프돔.ㅠㅠ
	int nCount = 0;
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( pGuildTournamentInfo[i].GuildUID.IsSet() )
			++nCount;
	}
	if( nCount > 0)	
	{
		memcpy(m_GuildWarFinalInfo, pGuildTournamentInfo, sizeof(m_GuildWarFinalInfo));
		m_nFinalTeamCount = nCount;			
		SetGuildWarTournamentGroup();
		m_bFinalTeamSetting = true;
	}	
	return nCount;
}

void CDNGuildWarManager::SetGuildWarPointTrialRanking(const MASetGuildWarPointRunningTotal* pData)
{
	memcpy(m_sGuildWarPointTrialRanking, pData, sizeof(m_sGuildWarPointTrialRanking));
}

void CDNGuildWarManager::CalcGuildWarTournamentResult()
{
	for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i)
	{
		if( m_bTournamentGroup[i].ATeam.nTeamNum > 0 && m_bTournamentGroup[i].BTeam.nTeamNum > 0)
		{
			BYTE ATeamIndex = m_bTournamentGroup[i].ATeam.nTeamNum-1;
			BYTE BTeamIndex = m_bTournamentGroup[i].BTeam.nTeamNum-1;

			// 둘다 진 상태로 되어 있으면..
			if( ATeamIndex < GUILDWAR_FINALS_TEAM_MAX && BTeamIndex < GUILDWAR_FINALS_TEAM_MAX 
				&& !m_GuildWarFinalInfo[ATeamIndex].bWin && !m_GuildWarFinalInfo[BTeamIndex].bWin )
			{
				// 그냥 결판 내버리자..
				CalcGuildWarTournamentWin(i);
			}
		}
	}
}

void CDNGuildWarManager::SetGuildWarTournamentGroup()
{
	memset(m_bTournamentGroup, 0, sizeof(m_bTournamentGroup));

	char cCurFinalPart = GetCurFinalPart();
	int nCount = 0;	
	int nDefaultWinMax = 0;

	//부전승 건너띄기
	switch(cCurFinalPart)
	{
	case GUILDWAR_FINALPART_NONE :
	case GUILDWAR_FINALPART_16 :
		 nDefaultWinMax = 1;
		 break;
	case GUILDWAR_FINALPART_8 : nDefaultWinMax = 3; break;
	case GUILDWAR_FINALPART_4 : nDefaultWinMax = 7; break;
	case GUILDWAR_FINALPART_FINAL : nDefaultWinMax = 15; break;
	}	
	
	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i )
	{
		if( !m_GuildWarFinalInfo[i].GuildUID.IsSet() )
			continue;
		if( m_GuildWarFinalInfo[i].cMatchTypeCode != GUILDWAR_FINALPART_NONE && m_GuildWarFinalInfo[i].bWin == false )
			continue;
		// 최종 우승자 스킵..
		if( m_GuildWarFinalInfo[i].cMatchTypeCode == GUILDWAR_FINALPART_FINAL && m_GuildWarFinalInfo[i].bWin == true)			
			continue;			
		// 현재 진행중인 차수와 대조
		if( m_GuildWarFinalInfo[i].cMatchTypeCode != GUILDWAR_FINALPART_NONE && m_GuildWarFinalInfo[i].cMatchTypeCode != cCurFinalPart+1)		
			continue;		
		
		if( m_bTournamentGroup[nCount].ATeam.nTeamNum == 0)
		{
			m_bTournamentGroup[nCount].ATeam.nTeamNum = i+1;	
			m_bTournamentGroup[nCount].ATeam.nOpeningPoint = m_GuildWarOpeningPoints[i];
			// 부전승 체크..이 방법밖에 없을라나..ㅠㅠ
			bool bDefaltWin = true;
			for( int j=1; j<= nDefaultWinMax; ++j )
			{
				if( i+j >= GUILDWAR_FINALS_TEAM_MAX )
					break;
				if( !m_GuildWarFinalInfo[i+j].GuildUID.IsSet() )
					continue;
				if( m_GuildWarFinalInfo[i+j].cMatchTypeCode != GUILDWAR_FINALPART_NONE && m_GuildWarFinalInfo[i+j].bWin == false )
					continue;
				// 결승전을 제외하고 왼쪽 팀이 오른쪽 팀과 붙을 일은 없다.
				if( cCurFinalPart != GUILDWAR_FINALPART_FINAL && i < GUILDWAR_TOURNAMENT_GROUP_MAX && i+j >= GUILDWAR_TOURNAMENT_GROUP_MAX )
					break;
				bDefaltWin = false;
				break;
			}			
			if( bDefaltWin )			
				++nCount;

			m_GuildWarFinalInfo[i].cMatchTypeCode = cCurFinalPart;
			m_GuildWarFinalInfo[i].bWin = false;
		}
		else if( m_bTournamentGroup[nCount].BTeam.nTeamNum == 0)
		{
			m_bTournamentGroup[nCount].BTeam.nTeamNum = i+1;
			m_bTournamentGroup[nCount].BTeam.nOpeningPoint = m_GuildWarOpeningPoints[i];
			++nCount;

			m_GuildWarFinalInfo[i].cMatchTypeCode = cCurFinalPart;
			m_GuildWarFinalInfo[i].bWin = false;
		}
	}	
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetGuildWarTournamentGroup FinalPart:%d !!!\n", cCurFinalPart);
}
// 길드전 방 생성
void CDNGuildWarManager::GuildWarCreateRoom()
{	
	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillageCon == NULL || pVillageCon->GetActive() == false )
		return;	//이거 어케해야함?

	CTimeSet CurTime;
	CurTime.GetMonth();
	// MapID 얻어오기
	UINT uiMapIndex = g_pExtManager->GetGuildWarMapInfoID((char)CurTime.GetMonth());
	if( uiMapIndex == 0 )		
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! GuildWarMapID is Zero\n");
		return;
	}
	// MapID로 PvPMapTable 얻어오기	
	const TPvPMapTable* pMapTable = g_pExtManager->GetPvPMapTable(uiMapIndex);
	if( pMapTable == NULL )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! pMapTable is NULL\n");
		return;
	}
	// PvPMapTable에서 GameMode 정보 가져오기
	const TPvPGameModeTable* pPvPGameModeTable = g_pExtManager->GetPvPGameModeTable( pMapTable->vGameModeTableID[0] );
	if( pPvPGameModeTable == NULL )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! pPvPGameModeTable is NULL\n");
		return;
	}

	for( int i=0; i<GUILDWAR_FINALS_TEAM_MAX/2; ++i)
	{
		if( m_bTournamentGroup[i].ATeam.nTeamNum )
		{
			if( m_bTournamentGroup[i].BTeam.nTeamNum == 0)
			{
				// 요건 부전승이다..DB저장 해야함..
				SetGuildTournamentInfoWin(m_bTournamentGroup[i].ATeam.nTeamNum-1, true); //이긴걸로 표시
				SendSetGuildWarFinalResult(m_bTournamentGroup[i].ATeam.nTeamNum-1);
				pVillageCon->SendSetGuildWarFinalResultDB(m_wScheduleID, m_bTournamentGroup[i].ATeam.nTeamNum, m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].GuildUID.nDBID, GetCurFinalPart(), true);
			}
			else
			{
				// 여기서 방만들기..
				VIMAPVP_CREATEROOM sCreateRoom;
				memset(&sCreateRoom, 0, sizeof(sCreateRoom));
				sCreateRoom.unVillageChannelID = g_pDivisionManager->GetPvPLobbyChannelID();
				sCreateRoom.uiVillageMapIndex = g_pDivisionManager->GetPvPLobbyMapIndex();
				sCreateRoom.cGameMode =  PvPCommon::GameMode::PvP_GuildWar;
				sCreateRoom.nGuildDBID[0] = m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].GuildUID.nDBID;
				sCreateRoom.nGuildQualifyingScore[0] = m_GuildWarOpeningPoints[m_bTournamentGroup[i].ATeam.nTeamNum-1];
				sCreateRoom.nGuildDBID[1] = m_GuildWarFinalInfo[m_bTournamentGroup[i].BTeam.nTeamNum-1].GuildUID.nDBID;
				sCreateRoom.nGuildQualifyingScore[1] = m_GuildWarOpeningPoints[m_bTournamentGroup[i].BTeam.nTeamNum-1];				

				sCreateRoom.sCSPVP_CREATEROOM.uiMapIndex = uiMapIndex; 
				sCreateRoom.sCSPVP_CREATEROOM.uiGameModeTableID = pMapTable->vGameModeTableID[0];
				sCreateRoom.sCSPVP_CREATEROOM.uiSelectWinCondition = pPvPGameModeTable->vWinCondition[0]; // 요거 점수인데 에라 모르겠다.
				sCreateRoom.sCSPVP_CREATEROOM.uiSelectPlayTimeSec = pPvPGameModeTable->vPlayTimeSec[0];
				sCreateRoom.sCSPVP_CREATEROOM.cMaxUser = pPvPGameModeTable->uiNumOfPlayersMax;
				sCreateRoom.sCSPVP_CREATEROOM.cMinUser = 0;
				sCreateRoom.sCSPVP_CREATEROOM.cRoomPWLen = 0;
				sCreateRoom.sCSPVP_CREATEROOM.cMinLevel = 1;
				sCreateRoom.sCSPVP_CREATEROOM.cMaxLevel = 100;
				sCreateRoom.sCSPVP_CREATEROOM.unRoomOptionBit |= (PvPCommon::RoomOption::BreakInto|PvPCommon::RoomOption::NoRegulation); // 난입,무조정 허용
				sCreateRoom.sCSPVP_CREATEROOM.uiEventItemID = 0;
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				sCreateRoom.sCSPVP_CREATEROOM.cRoomType = PvPCommon::RoomType::regular;
#endif		//#ifdef PRE_ADD_COLOSSEUM_BEGINNER
				wsprintf(sCreateRoom.sCSPVP_CREATEROOM.wszBuf, L"%s vs %s", m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].wszGuildName,
					m_GuildWarFinalInfo[m_bTournamentGroup[i].BTeam.nTeamNum-1].wszGuildName);			

				sCreateRoom.sCSPVP_CREATEROOM.cRoomNameLen = (BYTE)wcslen(sCreateRoom.sCSPVP_CREATEROOM.wszBuf);					
				short nRetCode = g_pDivisionManager->CreatePvPRoom( pVillageCon, &sCreateRoom, &m_bTournamentGroup[i].unPvPIndex );
				if( nRetCode != ERROR_NONE )
					g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! CreatePvPRoom %s \n", sCreateRoom.sCSPVP_CREATEROOM.wszBuf);
				else
				{
					m_bTournamentGroup[i].dwStartTick = timeGetTime();
					g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom !!! CreatePvPRoom PvPIndex:%u %s \n", m_bTournamentGroup[i].unPvPIndex, sCreateRoom.sCSPVP_CREATEROOM.wszBuf);
				}
			}
		}
	}
}

void CDNGuildWarManager::SetGuildWarTournamentResult(GAMAPvPGuildWarResult* pGuildWarResult)
{
	char cWinGuildIndex =-1, cLoseGuildIndex = -1;
	for(char i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{
		if( !m_GuildWarFinalInfo[i].GuildUID.IsSet() )
			continue;

		if( m_GuildWarFinalInfo[i].GuildUID.nDBID == pGuildWarResult->nWinGuildDBID )	
		{			
			SetGuildTournamentInfoWin(i, true);
			cWinGuildIndex = i;
			continue;
		}
		if( m_GuildWarFinalInfo[i].GuildUID.nDBID == pGuildWarResult->nLoseGuildDBID )
		{
			SetGuildTournamentInfoWin(i, false);
			cLoseGuildIndex = i;
			continue;
		}
	}
	SendSetGuildWarFinalResult(cWinGuildIndex, cLoseGuildIndex);
	SendSetGuildWarTournamentWin(cWinGuildIndex, m_GuildWarFinalInfo[cWinGuildIndex].cMatchTypeCode);
	//결승이면 우승길드로 셋팅해 주자~
	if( m_GuildWarFinalInfo[cWinGuildIndex].cMatchTypeCode == GUILDWAR_FINALPART_FINAL )
	{
		SetPreWinGuild( m_GuildWarFinalInfo[cWinGuildIndex].GuildUID);
		SetPreWinScheduleID(m_wScheduleID);
	}
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] TournamentResult !!! FinalPart:%d, WinGuildDBID:%d, LosGuildDBID:%d \n", GetCurFinalPart(),
		pGuildWarResult->nWinGuildDBID, pGuildWarResult->nLoseGuildDBID);
}

void CDNGuildWarManager::SendSetGuildWarFinalResult(char cWinGuildIndex, char cLoseGuildIndex )
{
	MAVISetGuildWarFinalResult FinalResult;
	memset(&FinalResult, 0, sizeof(FinalResult));
	FinalResult.cMatchTypeCode = GetCurFinalPart();
	FinalResult.cWinGuildIndex = cWinGuildIndex;
	FinalResult.cLoseGuildIndex = cLoseGuildIndex;

	g_pDivisionManager->SendSetGuildWarFinalResult(&FinalResult);
}

void CDNGuildWarManager::SetGuildTournamentInfoWin(char cIndex, bool bWin)
{
	if( cIndex < 0 || cIndex >= GUILDWAR_FINALS_TEAM_MAX )
		return;
	m_GuildWarFinalInfo[cIndex].bWin = bWin;
	// 만약 결승전 승자 이면..
	if( m_GuildWarFinalInfo[cIndex].cMatchTypeCode == GUILDWAR_FINALPART_FINAL && bWin )
	{
		// 여기서 길드 정보 업데이트
		MAChangeGuildInfo ChangeGuildInfo;
		memset(&ChangeGuildInfo, 0, sizeof(ChangeGuildInfo));
		// 본선 우승 횟수 1회 추가~~
		ChangeGuildInfo.btGuildUpdate = GUILDUPDATE_TYPE_GUILDWAR;
		ChangeGuildInfo.Int2 = 1;
		ChangeGuildInfo.GuildUID = m_GuildWarFinalInfo[cIndex].GuildUID;
		g_pDivisionManager->SendChangeGuildInfo(&ChangeGuildInfo);		
	}
}

void CDNGuildWarManager::SetGuildWarOpenningPoint(VIMASetGuildWarFinalTeam* pData )
{
	for( int i=0; i<pData->nCount; ++i)
	{
		for( int j=0; j<GUILDWAR_FINALS_TEAM_MAX; ++j)
		{
			if( pData->GuidUID[i] == m_GuildWarFinalInfo[j].GuildUID )
			{
				m_GuildWarOpeningPoints[j] = pData->nTotalPoint[i];
				break;
			}		
		}	
	}
}
void CDNGuildWarManager::SetGuildWarTournamentPoint(GAMAPvPGuildWarScore* pGuildWarPoint)
{
	for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i )
	{
		if( m_bTournamentGroup[i].ATeam.nTeamNum
			&& m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].GuildUID.nDBID == pGuildWarPoint->nGuildDBID)
		{
			m_bTournamentGroup[i].ATeam.nTournamentPoint = pGuildWarPoint->nScore;
			break;
		}
		if( m_bTournamentGroup[i].BTeam.nTeamNum
			&& m_GuildWarFinalInfo[m_bTournamentGroup[i].BTeam.nTeamNum-1].GuildUID.nDBID == pGuildWarPoint->nGuildDBID)
		{
			m_bTournamentGroup[i].BTeam.nTournamentPoint = pGuildWarPoint->nScore;
			break;
		}
	}
}

void CDNGuildWarManager::SendSetGuildWarTournamentWin(char cWinGuildIndex, char cFinalPart)
{
	if( cWinGuildIndex < 0 || cWinGuildIndex >= GUILDWAR_FINALS_TEAM_MAX )
		return;

	MASetGuildWarTournamentWin SetGuildWarTournamentWin;
	memset(&SetGuildWarTournamentWin, 0, sizeof(SetGuildWarTournamentWin));

	SetGuildWarTournamentWin.cMatchTypeCode = cFinalPart;	
	memcpy(SetGuildWarTournamentWin.wszGuildName, m_GuildWarFinalInfo[cWinGuildIndex].wszGuildName, sizeof(SetGuildWarTournamentWin.wszGuildName) );

	g_pDivisionManager->SendSetGuildWarTournamentWin(&SetGuildWarTournamentWin);
}

char CDNGuildWarManager::GetCurFinalPart()
{
	CDNGuildWarReward* pGuildWarReward = (CDNGuildWarReward*)m_pWarEventStep[GUILDWAR_STEP_REWARD];
	if( pGuildWarReward )	
		return pGuildWarReward->GetCurFinalPart();
	return 0;
}

time_t CDNGuildWarManager::GetCurFinalPartBeginTime()
{
	CDNGuildWarReward* pGuildWarReward = (CDNGuildWarReward*)m_pWarEventStep[GUILDWAR_STEP_REWARD];
	if( pGuildWarReward )	
		return pGuildWarReward->GetCurFinalPartBeginTime();
	return 0;
}

void CDNGuildWarManager::SetDBJobSend(bool bSend)
{
	CDNGuildWar* pGuildWar = m_pWarEventStep[m_cStepIndex];
	if( pGuildWar )	
		pGuildWar->SetDBJobSend(bSend);	
}

void CDNGuildWarManager::SetDBJobSeq(int nJobSeq)
{
	CDNGuildWar* pGuildWar = m_pWarEventStep[m_cStepIndex];
	if( pGuildWar )	
		pGuildWar->SetDBJobSeq(nJobSeq);
}

void CDNGuildWarManager::SetDBJobSuccess(bool bSuccess)
{
	CDNGuildWar* pGuildWar = m_pWarEventStep[m_cStepIndex];
	if( pGuildWar )	
		pGuildWar->SetDBJobSuccess(bSuccess);
}

bool CDNGuildWarManager::GetDBJobSuccess()
{
	CDNGuildWar* pGuildWar = m_pWarEventStep[m_cStepIndex];
	if( pGuildWar )	
		return pGuildWar->GetDBJobSuccess();
	return false;
}

// 방이 뽀사지거나 게임서버가 튕겼을때 호출바람..
void CDNGuildWarManager::SetDropTournament(UINT  unPvPIndex)
{
	for( int i=0; i<GUILDWAR_TOURNAMENT_GROUP_MAX; ++i )
	{
		if( m_bTournamentGroup[i].unPvPIndex == unPvPIndex )
		{			
			CalcGuildWarTournamentWin(i);			
			break;
		}
	}
}

void CDNGuildWarManager::CalcGuildWarTournamentWin(BYTE cGroupIndex)
{
	if( cGroupIndex >= GUILDWAR_TOURNAMENT_GROUP_MAX)
		return;

	char cWinGuildIndex = -1, cLoseGuildIndex = -1;
	// 먼저 중간 점수 보고
	if( m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint != m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint )
	{
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint > m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint ? m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint > m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint ? m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
	}
	// 안되면 예선점수 보고
	else if( m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint != m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint )
	{
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint > m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint ? m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint > m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint ? m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
	}
	else
	{
		//이건 운이다..된장..
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
	}
	char cMatchTypeCode = m_GuildWarFinalInfo[cWinGuildIndex].cMatchTypeCode;
	SetGuildTournamentInfoWin(cWinGuildIndex, true); //이긴걸로 표시
	SetGuildTournamentInfoWin(cLoseGuildIndex, false); //진걸로 표시

	SendSetGuildWarFinalResult(cWinGuildIndex, cLoseGuildIndex); // 결과 보내주고..
	SendSetGuildWarTournamentWin(cWinGuildIndex, cMatchTypeCode); // 승리 길드 알림.
	//보상지급해줘야함..
	//CDNVillageConnection* pVillage = g_pDivisionManager->GetFirstEnableVillageServer();
	CDNVillageConnection* pVillage = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillage )
	{
		//이긴넘 진넘 둘다 DB에 결과 저장
		pVillage->SendSetGuildWarFinalResultDB(m_wScheduleID, cWinGuildIndex+1, m_GuildWarFinalInfo[cWinGuildIndex].GuildUID.nDBID, cMatchTypeCode, true);
		pVillage->SendSetGuildWarFinalResultDB(m_wScheduleID, cLoseGuildIndex+1, m_GuildWarFinalInfo[cLoseGuildIndex].GuildUID.nDBID, cMatchTypeCode, false);
	}
	else
	{
		// 요건 보상도 못주고 결과 기록도 못함.ㅠㅠ
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetDropTournament Failed !!! WinGuildDBID:%d, LosGuildDBID:%d \n", m_GuildWarFinalInfo[cWinGuildIndex].GuildUID.nDBID,
			m_GuildWarFinalInfo[cLoseGuildIndex].GuildUID.nDBID);
	}
	//결승이면 우승길드로 셋팅해 주자~
	if( cMatchTypeCode == GUILDWAR_FINALPART_FINAL )
	{
		g_pGuildWarManager->SetPreWinGuild( m_GuildWarFinalInfo[cWinGuildIndex].GuildUID);
		g_pGuildWarManager->SetPreWinScheduleID(m_wScheduleID);
	}
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetDropTournament !!! WinGuildDBID:%d, LosGuildDBID:%d \n", m_GuildWarFinalInfo[cWinGuildIndex].GuildUID.nDBID,
		m_GuildWarFinalInfo[cLoseGuildIndex].GuildUID.nDBID);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Function
bool CDNGuildWarManager::CheckStep(char cStep)
{
	if (GUILDWAR_STEP_NONE < cStep && cStep < GUILDWAR_STEP_END)
		return true;

	return false;
}


void CDNGuildWarManager::ResetStep()
{	
	// 스케쥴 ID리셋
	m_wScheduleID = GUILDWARSCHEDULE_DEF;

	// 스텝 초기화
	m_cStepIndex = GUILDWAR_STEP_NONE;
	m_bWarEvent = false;

	for (int i=GUILDWAR_STEP_NONE; i<GUILDWAR_STEP_END; i++)
	{
		if (m_pWarEventStep[i])
			m_pWarEventStep[i]->Reset();
	}
}

void CDNGuildWarManager::StartStep()
{
	m_bWarEvent = true;
	m_cStepIndex = GUILDWAR_STEP_PREPARATION;

	// 현재 어떤 이벤트가 진행되는지 여기서 맞춰버리자.
	for( int i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; ++i)
	{		
		CDNGuildWar** pWarEventStep = m_pWarEventStep;

		if (!pWarEventStep[i])
			return;	

		// 해당 이벤트객체를 호출하여 처리!
		pWarEventStep[i]->Process();

		// 모든 이벤트가 끝났을때 다음스텝으로 진행
		if (pWarEventStep[i]->IsFinishPeriod())
			NextStep();
	}
}

void CDNGuildWarManager::NextStep()
{
	// 혹시 모르니 타입체크 한번더!
	if (!CheckStep(m_cStepIndex))
		return;

	char cNextStepIndex = m_cStepIndex + 1;
	if (cNextStepIndex >= GUILDWAR_STEP_END)
	{
		// 다음스텝이 마지막이면 종료한다.
		FinalStep();
		// 다음 차수 가져오기~
		m_eSettingStep = REQ_SCHEDULE;
		SendGuildWarInfoReq();
		return;
	}

	time_t tNextEvent = m_pWarEventStep[cNextStepIndex]->GetEventTime (GUILDWAR_EVENT_START);

	time_t tCurrentTime;
	time(&tCurrentTime);

	// 다음 이벤트 시간이 현재시각보다 많으면 다음 스텝으로 진행한다.
	if (tCurrentTime > tNextEvent)
		m_cStepIndex++;
}

void CDNGuildWarManager::FinalStep()
{
	Clear();
	//ResetStep();
}

bool CDNGuildWarManager::RegisterScheduleGroup(short wScheduleID, const TGuildWarEventInfo vEventInfo[])
{
	ResetStep();

	time_t tCheckPeriod = 0;
	time_t tBeginTime = 0;
	time_t tEndTime = 0;

	for (char i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
	{
		char cStep = i;

		// 타입체크
		if (!CheckStep(cStep))
		{
			g_Log.Log(LogType::_FILELOG, L"CDNGuildWarManager::RegisterScheduleGroup Failed !!! Wrong Step Type (%d)\n", cStep);
			return false;
		}

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		// 시간체크
		if (tBeginTime < tCheckPeriod || tEndTime < tCheckPeriod || tBeginTime >= tEndTime)
		{
			ResetStep();	// 연속된 기간이 아니라면 에러처리하고 리셋!
			g_Log.Log(LogType::_FILELOG, L"CDNGuildWarManager::RegisterScheduleGroup Failed !!! Wrong Time Schedule Begin(%d) End(%d) Check(%d)\n", tBeginTime, tEndTime, tCheckPeriod);
			return false;
		}

		m_pWarEventStep[cStep]->UpdateTime(wScheduleID, tBeginTime, tEndTime, m_wWinersWeightRate);

		// 마지막 시간을 저장해서 다음 기간에 대한 검사조건으로 사용한다.
		tCheckPeriod  = tEndTime;
	}

	m_wScheduleID = wScheduleID;

	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] RegisterScheduleGroup Success ScheduleID:%d!!!\n", wScheduleID);

	return true;
}

bool CDNGuildWarManager::UpdateScheduleGroup(short wScheduleID, const TGuildWarEventInfo vEventInfo[])
{
	time_t tCurrentTime;
	time(&tCurrentTime);		// 현재시간

	// 데이터상의 현재 스텝
	char cStepIndex = GUILDWAR_STEP_NONE;

	time_t tCheckPeriod = 0;
	time_t tBeginTime = 0;
	time_t tEndTime = 0;

	// 본선 진행중일때는 스케쥴 업데이트 하지말자..이럼 Hell 될것 같음.
	if( m_cStepIndex == GUILDWAR_STEP_REWARD )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Already Reward Step On\n");
		return false;
	}

	// 데이터 체크
	for (char i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
	{
		char cStep = i;

		// 타입체크
		if (!CheckStep(cStep))
		{
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Step Type (%d)\n", cStep);
			return false;
		}

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		// 시간체크
		if (tBeginTime < tCheckPeriod || tEndTime < tCheckPeriod || tBeginTime >= tEndTime)
		{
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Time Schedule Begin(%d) End(%d) Check(%d)\n", tBeginTime, tEndTime, tCheckPeriod);
			return false;
		}

		// 데이터상 현재 스텝을 구한다.
		if (tCurrentTime > tBeginTime && tCurrentTime < tEndTime)
			cStepIndex = i;
		
		// 마지막 시간을 저장해서 다음 기간에 대한 검사조건으로 사용한다.
		tCheckPeriod  = tEndTime;
	}

	// 현재 스텝과 맞지 않으면 에러처리
	if (m_cStepIndex != cStepIndex)
	{		
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Step Index[%d]<->[%d]\n", m_cStepIndex, cStepIndex);
		return false;
	}

	// 데이터 입력
	for (char i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
	{
		char cStep = i;

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		if (CheckStep(cStep))
			m_pWarEventStep[cStep]->UpdateTime(wScheduleID, tBeginTime, tEndTime, m_wWinersWeightRate);
		else
		{
			_DANGER_POINT(); // 위에서 한번 체크했는데 타입체크 실패하면 말이 안됨!
			return false;
		}
	}
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Success ScheduleID:%d!!!\n", wScheduleID);
	return true;
}


void CDNGuildWarManager::CalcTeamSecret()
{
	// 블루 : 1
	// 레드 : 2
	if (m_nBlueTeamPoint == m_nRedTeamPoint)
	{
		m_cSecretTeam = 0;
		return;
	}

	BYTE cTeamCode = 0;
	float fSecretRate = 0.0f;
	if (m_nBlueTeamPoint > m_nRedTeamPoint)
	{
		fSecretRate = (float)m_nRedTeamPoint / (float)m_nBlueTeamPoint;
		cTeamCode = 2;	// 레드팀 혜택
	}
	else
	{
		fSecretRate = (float)m_nBlueTeamPoint / (float)m_nRedTeamPoint;
		cTeamCode = 1; // 블루팀 혜택
	}

	if (fSecretRate < 0)
	{
		DN_ASSERT( false, "fSecretRate < 0" );
		return;
	}

	fSecretRate = 1.0f - fSecretRate;
	float nSecretStartRate = GUILDWAR_SECRET_SATRT_RATE;
	float nSecretEndRate = GUILDWAR_SECRET_END_RATE;

	time_t tCurrentTime;
	time(&tCurrentTime);
	
	time_t tStartTime = m_pWarEventStep[GUILDWAR_STEP_TRIAL]->GetEventTime(GUILDWAR_EVENT_START);
	time_t tEndTime = m_pWarEventStep[GUILDWAR_STEP_TRIAL]->GetEventTime(GUILDWAR_EVENT_END);
	
	//예선 종료 시간 먼저 체크
	if( tEndTime - tCurrentTime <= GUILDWAR_SECRET_TERM_SECOND ) // 24시간 이하로 남았으면 비율을 반으로 줄임
	{
		nSecretStartRate = nSecretStartRate*0.5f;
		nSecretEndRate = nSecretEndRate*0.5f;
	}
	else if( tCurrentTime - tStartTime  < GUILDWAR_SECRET_TERM_SECOND ) // 시작된지 24시간 안 지났으면 시크릿 발동 안됨.	
		return;
	
	if (fSecretRate >= nSecretStartRate)
	{
		if (m_cSecretTeam != cTeamCode)
		{
			m_cSecretTeam = cTeamCode;	// 적용

			CMtRandom rand;
			rand.srand( timeGetTime() );
			m_nSecretRandomSeed = rand.rand();

			MASetGuildWarSecretMission packet = {0,};
		
			packet.cTeamCode = m_cSecretTeam;
			packet.nRandomSeed = m_nSecretRandomSeed;
			g_pDivisionManager->SendSetGuildWarSecretMission(&packet);
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SecretStart Seed:%d, Team:%d, Rate:%f!!!\n", packet.nRandomSeed, packet.cTeamCode, fSecretRate );
		}
	}
	else if (fSecretRate <= nSecretEndRate && m_cSecretTeam != 0)		
	{
		m_cSecretTeam = 0;			// 해제

		MASetGuildWarSecretMission packet = {0,};
		packet.cTeamCode = m_cSecretTeam;
		packet.nRandomSeed = -1;

		g_pDivisionManager->SendSetGuildWarSecretMission(&packet);
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SecretEnd Rate:%f!!!\n", fSecretRate );
	}
}
