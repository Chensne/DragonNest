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
	// ���� �ʱ�ȭ
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
	// �̹� ����� �̺�Ʈ �������� Ȯ���Ѵ�
	time_t tCurrentTime;
	time(&tCurrentTime);

	if (pData->bForce)
	{
		m_bWarEvent = false;
		m_bCheatFlag = true;
	}
	if (!pData->bForce && tCurrentTime > pData->EventInfo[GUILDWAR_STEP_REWARD].tEndTime)
	{
		// �̹� ���� �̺�Ʈ�̰ų� ������ �ȵȰ���
		//g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] LoadScheduleInfo Failed !!! TimeOut or Schedule empty\n");		
#if defined(PRE_FIX_75807)
		// ���⼭ Ȥ�� ���� ����� ������� ���� ���縦 �ȳѾ����� ����.
		if( pData->tRewardExpireDate > 0 && !GetPreWinGuild().IsSet() )
		{			
			__time64_t tCurrentTime;
			time(&tCurrentTime);
			if( tCurrentTime < pData->tRewardExpireDate )
			{
				m_tRewardExpireDate = pData->tRewardExpireDate;	
				// ���� ���� �Ⱓ�� ���� ������ ��±�� �̾ƿ���
				m_eSettingStep = REQ_PRE_WIN;
				SendGuildWarInfoReq(); // ���� ������ ���� �������� ��û
				g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Send GetPrewinGuild Schedule Empty!!!\n");
				return true;
			}
		}
		else if( pData->tRewardExpireDate == 0 && !m_bFarmForceHarbest )		
		{
			// ���⼭ ���Ӽ��� ��³��� �ʱ�ȭ ����
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

	// �̺�Ʈ �������̸�
	if (m_bWarEvent)
	{
		// ���������� ���� �Ⱓ ������Ʈ ����
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
	else // �̺�Ʈ �������� �ƴ϶�� �ű� ���
	{
		bResult = RegisterScheduleGroup(pData->wScheduleID, pData->EventInfo);
	}

	if (bResult)
	{
		// ����� �ϵ� ������Ʈ�� �ϵ� ���⼭ ������ ���� ������Ʈ�Ѵ�.		
		m_wScheduleID = pData->wScheduleID;		
		
		m_eSettingStep = REQ_FINAL_SCHEDULE;
		SendGuildWarInfoReq(); // ���� ������ ���� �������� ��û
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
	// �̹� ������ �������̸� �������� ���õǾ� �ִ��� üũ..

	// �� �ð�üũ
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
	
	// ���� ������ ���ƶ�..
	StartStep();

	// ��ü ������ �����ֱ�
	MASetGuildWarEventTime SetGuildWarEventTime;
	memset(&SetGuildWarEventTime, 0, sizeof(SetGuildWarEventTime));
	memcpy(SetGuildWarEventTime.sGuildWarTime, m_sGuildWarSchedule, sizeof(SetGuildWarEventTime.sGuildWarTime));
	memcpy(SetGuildWarEventTime.sFinalPartTime, m_sGuildWarFinalSchedule, sizeof(SetGuildWarEventTime.sFinalPartTime));

	SetGuildWarEventTime.bFinalProgress = GetFinalProgress();
	g_pDivisionManager->SendSetGuildWarSchedule(&SetGuildWarEventTime);

	if( (m_cStepIndex != GUILDWAR_STEP_REWARD || !GetFinalStart() ) && m_wScheduleID != 1)  // �� ó�� ������ �ƴϸ�.
	{
		// ������ �ƴϰų� ���� ������ �������� ���� ���� ����� ��������..
		m_eSettingStep = REQ_PRE_WIN;
		SendGuildWarInfoReq();
		return true;
	}	
	else if( m_cStepIndex == GUILDWAR_STEP_TRIAL || m_cStepIndex == GUILDWAR_STEP_REWARD ) 
	{
		// �����̰ų� �����̸� û��, ȫ�� ����Ʈ ��������
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
	// �ݷμ��� ���������� ��û
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
				pVillageConnection->SendGetGuildWarSchedule(); // ������ ���� �������� ��û				
				m_eSettingStep = REQ_SCHEDULE;
			}
			break;
		case REQ_FINAL_SCHEDULE :
			{
				pVillageConnection->SendGetGuildWarFinalSchedule(m_wScheduleID); // ���� ������ ���� �������� ��û				
			}
			break;		
		case REQ_PRE_WIN :
			{
				pVillageConnection->SendGetGuildWarPreWinGuild();	// ����� ��������
			}
			break;
		case REQ_TEAM_POINT :
			{
				pVillageConnection->SendGetGuildWarPoint(); // û��, ȫ�� ����Ʈ ��������				
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
		// �����̰ų� �����̸� ���̸� û��, ȫ�� ����Ʈ ��������
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
	// �ڸ��� �����Դϴٿ�...�ڼ��Ѱ� [DN]�����_��������ǥ��ġ_1_.pptx ����
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

	// ���⼭ ��� ���� ������Ʈ
	MAChangeGuildInfo ChangeGuildInfo;
	memset(&ChangeGuildInfo, 0, sizeof(ChangeGuildInfo));
	// ���� ���� Ƚ�� 1ȸ �߰�~~
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
	// Reset Ȯ��
	if ( m_bResetSchedule )
	{		 
		if( g_pGuildWarManager->GetFinalProgress() && g_pGuildWarManager->GetFinalStart() )
		{
			// ���� �������̸�..�̶��� ���� �ߴ����� ����� ��� ����� ���� ����..
			g_pDivisionManager->SendGuildWarAllStop();			
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Emergence GuilWar Stop Process!!!\n");
		}
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] Reset Schedule GuilWar Progress:%d, Start:%d\n",g_pGuildWarManager->GetFinalProgress(),g_pGuildWarManager->GetFinalStart() );
		Clear(); // ���� �ʱ�ȭ�ϰ�
		SendGuildWarInfoReq(); // �ٽ� ÷���� �ε� ����
		return;
	}

	//��� ��尡 ���õǾ� ������ ������ ���޵ƴ��� ��������..�뷫 1�� ���� �ѹ���..	
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
		|| (m_bSendGuildWarInfo && CurTick > m_TickCheckGuildWar+REQ_GUILDWARINFO_TICK_MAX)) )  // Ȱ��ȭ �Ǿ� ���� �ʰ� ���������� ���� ��û���� ���� ���°ų� Ÿ�ӿ����̸�
	{		
		SendGuildWarInfoReq();
		return;
	}
	// ��±�� ����..������, ����	
	if( m_PreWinGuildUID.IsSet() && m_bFinalWinGuild && m_tRewardExpireDate > 0 )
	{
		__time64_t tCurrentTime;
		time(&tCurrentTime);
		if( tCurrentTime > m_tRewardExpireDate )
		{
			// ��±�� ����			
			MAGuildWarPreWinGuild pPacket;
			memset(&pPacket, 0, sizeof(MAGuildWarPreWinGuild));
			pPacket.GuildUID = GetPreWinGuild();
			pPacket.bPreWin = false;
			g_pDivisionManager->SendSetGuildWarPreWinGuild(&pPacket); // ��ü ���� �� �������� �뺸.

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
			// �ش� �̺�Ʈ��ü�� ȣ���Ͽ� ó��!
			pCurrentEvent->Process();

			// ��� �̺�Ʈ�� �������� ������������ ����
			if (pCurrentEvent->IsFinishPeriod())
				NextStep();
		}
	}

	// ���� �����϶���..
	if( GetFinalProgress() ) 
	{
		CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
		if( pVillageCon == NULL || pVillageCon->GetActive() == false )
			return;	//�̰� �����ؾ���?	

		// �����̰� ���� ������� �ֵ��� �ð��� �Ǹ� ��ŸƮ ��������..
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
	// ī��Ʈ ������ ������.�Ф�
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

			// �Ѵ� �� ���·� �Ǿ� ������..
			if( ATeamIndex < GUILDWAR_FINALS_TEAM_MAX && BTeamIndex < GUILDWAR_FINALS_TEAM_MAX 
				&& !m_GuildWarFinalInfo[ATeamIndex].bWin && !m_GuildWarFinalInfo[BTeamIndex].bWin )
			{
				// �׳� ���� ��������..
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

	//������ �ǳʶ��
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
		// ���� ����� ��ŵ..
		if( m_GuildWarFinalInfo[i].cMatchTypeCode == GUILDWAR_FINALPART_FINAL && m_GuildWarFinalInfo[i].bWin == true)			
			continue;			
		// ���� �������� ������ ����
		if( m_GuildWarFinalInfo[i].cMatchTypeCode != GUILDWAR_FINALPART_NONE && m_GuildWarFinalInfo[i].cMatchTypeCode != cCurFinalPart+1)		
			continue;		
		
		if( m_bTournamentGroup[nCount].ATeam.nTeamNum == 0)
		{
			m_bTournamentGroup[nCount].ATeam.nTeamNum = i+1;	
			m_bTournamentGroup[nCount].ATeam.nOpeningPoint = m_GuildWarOpeningPoints[i];
			// ������ üũ..�� ����ۿ� ������..�Ф�
			bool bDefaltWin = true;
			for( int j=1; j<= nDefaultWinMax; ++j )
			{
				if( i+j >= GUILDWAR_FINALS_TEAM_MAX )
					break;
				if( !m_GuildWarFinalInfo[i+j].GuildUID.IsSet() )
					continue;
				if( m_GuildWarFinalInfo[i+j].cMatchTypeCode != GUILDWAR_FINALPART_NONE && m_GuildWarFinalInfo[i+j].bWin == false )
					continue;
				// ������� �����ϰ� ���� ���� ������ ���� ���� ���� ����.
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
// ����� �� ����
void CDNGuildWarManager::GuildWarCreateRoom()
{	
	CDNVillageConnection* pVillageCon = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillageCon == NULL || pVillageCon->GetActive() == false )
		return;	//�̰� �����ؾ���?

	CTimeSet CurTime;
	CurTime.GetMonth();
	// MapID ������
	UINT uiMapIndex = g_pExtManager->GetGuildWarMapInfoID((char)CurTime.GetMonth());
	if( uiMapIndex == 0 )		
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! GuildWarMapID is Zero\n");
		return;
	}
	// MapID�� PvPMapTable ������	
	const TPvPMapTable* pMapTable = g_pExtManager->GetPvPMapTable(uiMapIndex);
	if( pMapTable == NULL )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] GuildWarCreateRoom Failed !!! pMapTable is NULL\n");
		return;
	}
	// PvPMapTable���� GameMode ���� ��������
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
				// ��� �������̴�..DB���� �ؾ���..
				SetGuildTournamentInfoWin(m_bTournamentGroup[i].ATeam.nTeamNum-1, true); //�̱�ɷ� ǥ��
				SendSetGuildWarFinalResult(m_bTournamentGroup[i].ATeam.nTeamNum-1);
				pVillageCon->SendSetGuildWarFinalResultDB(m_wScheduleID, m_bTournamentGroup[i].ATeam.nTeamNum, m_GuildWarFinalInfo[m_bTournamentGroup[i].ATeam.nTeamNum-1].GuildUID.nDBID, GetCurFinalPart(), true);
			}
			else
			{
				// ���⼭ �游���..
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
				sCreateRoom.sCSPVP_CREATEROOM.uiSelectWinCondition = pPvPGameModeTable->vWinCondition[0]; // ��� �����ε� ���� �𸣰ڴ�.
				sCreateRoom.sCSPVP_CREATEROOM.uiSelectPlayTimeSec = pPvPGameModeTable->vPlayTimeSec[0];
				sCreateRoom.sCSPVP_CREATEROOM.cMaxUser = pPvPGameModeTable->uiNumOfPlayersMax;
				sCreateRoom.sCSPVP_CREATEROOM.cMinUser = 0;
				sCreateRoom.sCSPVP_CREATEROOM.cRoomPWLen = 0;
				sCreateRoom.sCSPVP_CREATEROOM.cMinLevel = 1;
				sCreateRoom.sCSPVP_CREATEROOM.cMaxLevel = 100;
				sCreateRoom.sCSPVP_CREATEROOM.unRoomOptionBit |= (PvPCommon::RoomOption::BreakInto|PvPCommon::RoomOption::NoRegulation); // ����,������ ���
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
	//����̸� ��±��� ������ ����~
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
	// ���� ����� ���� �̸�..
	if( m_GuildWarFinalInfo[cIndex].cMatchTypeCode == GUILDWAR_FINALPART_FINAL && bWin )
	{
		// ���⼭ ��� ���� ������Ʈ
		MAChangeGuildInfo ChangeGuildInfo;
		memset(&ChangeGuildInfo, 0, sizeof(ChangeGuildInfo));
		// ���� ��� Ƚ�� 1ȸ �߰�~~
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

// ���� �ǻ����ų� ���Ӽ����� ƨ������ ȣ��ٶ�..
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
	// ���� �߰� ���� ����
	if( m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint != m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint )
	{
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint > m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint ? m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTournamentPoint > m_bTournamentGroup[cGroupIndex].BTeam.nTournamentPoint ? m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
	}
	// �ȵǸ� �������� ����
	else if( m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint != m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint )
	{
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint > m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint ? m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nOpeningPoint > m_bTournamentGroup[cGroupIndex].BTeam.nOpeningPoint ? m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1 : m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
	}
	else
	{
		//�̰� ���̴�..����..
		cWinGuildIndex = m_bTournamentGroup[cGroupIndex].ATeam.nTeamNum-1;
		cLoseGuildIndex = m_bTournamentGroup[cGroupIndex].BTeam.nTeamNum-1;
	}
	char cMatchTypeCode = m_GuildWarFinalInfo[cWinGuildIndex].cMatchTypeCode;
	SetGuildTournamentInfoWin(cWinGuildIndex, true); //�̱�ɷ� ǥ��
	SetGuildTournamentInfoWin(cLoseGuildIndex, false); //���ɷ� ǥ��

	SendSetGuildWarFinalResult(cWinGuildIndex, cLoseGuildIndex); // ��� �����ְ�..
	SendSetGuildWarTournamentWin(cWinGuildIndex, cMatchTypeCode); // �¸� ��� �˸�.
	//���������������..
	//CDNVillageConnection* pVillage = g_pDivisionManager->GetFirstEnableVillageServer();
	CDNVillageConnection* pVillage = g_pDivisionManager->GetVillageConnectionByVillageID( g_pDivisionManager->GetPvPLobbyVillageID() );
	if( pVillage )
	{
		//�̱�� ���� �Ѵ� DB�� ��� ����
		pVillage->SendSetGuildWarFinalResultDB(m_wScheduleID, cWinGuildIndex+1, m_GuildWarFinalInfo[cWinGuildIndex].GuildUID.nDBID, cMatchTypeCode, true);
		pVillage->SendSetGuildWarFinalResultDB(m_wScheduleID, cLoseGuildIndex+1, m_GuildWarFinalInfo[cLoseGuildIndex].GuildUID.nDBID, cMatchTypeCode, false);
	}
	else
	{
		// ��� ���� ���ְ� ��� ��ϵ� ����.�Ф�
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SetDropTournament Failed !!! WinGuildDBID:%d, LosGuildDBID:%d \n", m_GuildWarFinalInfo[cWinGuildIndex].GuildUID.nDBID,
			m_GuildWarFinalInfo[cLoseGuildIndex].GuildUID.nDBID);
	}
	//����̸� ��±��� ������ ����~
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
	// ������ ID����
	m_wScheduleID = GUILDWARSCHEDULE_DEF;

	// ���� �ʱ�ȭ
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

	// ���� � �̺�Ʈ�� ����Ǵ��� ���⼭ ���������.
	for( int i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; ++i)
	{		
		CDNGuildWar** pWarEventStep = m_pWarEventStep;

		if (!pWarEventStep[i])
			return;	

		// �ش� �̺�Ʈ��ü�� ȣ���Ͽ� ó��!
		pWarEventStep[i]->Process();

		// ��� �̺�Ʈ�� �������� ������������ ����
		if (pWarEventStep[i]->IsFinishPeriod())
			NextStep();
	}
}

void CDNGuildWarManager::NextStep()
{
	// Ȥ�� �𸣴� Ÿ��üũ �ѹ���!
	if (!CheckStep(m_cStepIndex))
		return;

	char cNextStepIndex = m_cStepIndex + 1;
	if (cNextStepIndex >= GUILDWAR_STEP_END)
	{
		// ���������� �������̸� �����Ѵ�.
		FinalStep();
		// ���� ���� ��������~
		m_eSettingStep = REQ_SCHEDULE;
		SendGuildWarInfoReq();
		return;
	}

	time_t tNextEvent = m_pWarEventStep[cNextStepIndex]->GetEventTime (GUILDWAR_EVENT_START);

	time_t tCurrentTime;
	time(&tCurrentTime);

	// ���� �̺�Ʈ �ð��� ����ð����� ������ ���� �������� �����Ѵ�.
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

		// Ÿ��üũ
		if (!CheckStep(cStep))
		{
			g_Log.Log(LogType::_FILELOG, L"CDNGuildWarManager::RegisterScheduleGroup Failed !!! Wrong Step Type (%d)\n", cStep);
			return false;
		}

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		// �ð�üũ
		if (tBeginTime < tCheckPeriod || tEndTime < tCheckPeriod || tBeginTime >= tEndTime)
		{
			ResetStep();	// ���ӵ� �Ⱓ�� �ƴ϶�� ����ó���ϰ� ����!
			g_Log.Log(LogType::_FILELOG, L"CDNGuildWarManager::RegisterScheduleGroup Failed !!! Wrong Time Schedule Begin(%d) End(%d) Check(%d)\n", tBeginTime, tEndTime, tCheckPeriod);
			return false;
		}

		m_pWarEventStep[cStep]->UpdateTime(wScheduleID, tBeginTime, tEndTime, m_wWinersWeightRate);

		// ������ �ð��� �����ؼ� ���� �Ⱓ�� ���� �˻��������� ����Ѵ�.
		tCheckPeriod  = tEndTime;
	}

	m_wScheduleID = wScheduleID;

	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] RegisterScheduleGroup Success ScheduleID:%d!!!\n", wScheduleID);

	return true;
}

bool CDNGuildWarManager::UpdateScheduleGroup(short wScheduleID, const TGuildWarEventInfo vEventInfo[])
{
	time_t tCurrentTime;
	time(&tCurrentTime);		// ����ð�

	// �����ͻ��� ���� ����
	char cStepIndex = GUILDWAR_STEP_NONE;

	time_t tCheckPeriod = 0;
	time_t tBeginTime = 0;
	time_t tEndTime = 0;

	// ���� �������϶��� ������ ������Ʈ ��������..�̷� Hell �ɰ� ����.
	if( m_cStepIndex == GUILDWAR_STEP_REWARD )
	{
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Already Reward Step On\n");
		return false;
	}

	// ������ üũ
	for (char i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
	{
		char cStep = i;

		// Ÿ��üũ
		if (!CheckStep(cStep))
		{
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Step Type (%d)\n", cStep);
			return false;
		}

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		// �ð�üũ
		if (tBeginTime < tCheckPeriod || tEndTime < tCheckPeriod || tBeginTime >= tEndTime)
		{
			g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Time Schedule Begin(%d) End(%d) Check(%d)\n", tBeginTime, tEndTime, tCheckPeriod);
			return false;
		}

		// �����ͻ� ���� ������ ���Ѵ�.
		if (tCurrentTime > tBeginTime && tCurrentTime < tEndTime)
			cStepIndex = i;
		
		// ������ �ð��� �����ؼ� ���� �Ⱓ�� ���� �˻��������� ����Ѵ�.
		tCheckPeriod  = tEndTime;
	}

	// ���� ���ܰ� ���� ������ ����ó��
	if (m_cStepIndex != cStepIndex)
	{		
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Failed !!! Wrong Step Index[%d]<->[%d]\n", m_cStepIndex, cStepIndex);
		return false;
	}

	// ������ �Է�
	for (char i=GUILDWAR_STEP_PREPARATION; i<GUILDWAR_STEP_END; i++)
	{
		char cStep = i;

		tBeginTime = vEventInfo[i].tBeginTime;
		tEndTime = vEventInfo[i].tEndTime;

		if (CheckStep(cStep))
			m_pWarEventStep[cStep]->UpdateTime(wScheduleID, tBeginTime, tEndTime, m_wWinersWeightRate);
		else
		{
			_DANGER_POINT(); // ������ �ѹ� üũ�ߴµ� Ÿ��üũ �����ϸ� ���� �ȵ�!
			return false;
		}
	}
	g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] UpdateScheduleGroup Success ScheduleID:%d!!!\n", wScheduleID);
	return true;
}


void CDNGuildWarManager::CalcTeamSecret()
{
	// ��� : 1
	// ���� : 2
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
		cTeamCode = 2;	// ������ ����
	}
	else
	{
		fSecretRate = (float)m_nBlueTeamPoint / (float)m_nRedTeamPoint;
		cTeamCode = 1; // ����� ����
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
	
	//���� ���� �ð� ���� üũ
	if( tEndTime - tCurrentTime <= GUILDWAR_SECRET_TERM_SECOND ) // 24�ð� ���Ϸ� �������� ������ ������ ����
	{
		nSecretStartRate = nSecretStartRate*0.5f;
		nSecretEndRate = nSecretEndRate*0.5f;
	}
	else if( tCurrentTime - tStartTime  < GUILDWAR_SECRET_TERM_SECOND ) // ���۵��� 24�ð� �� �������� ��ũ�� �ߵ� �ȵ�.	
		return;
	
	if (fSecretRate >= nSecretStartRate)
	{
		if (m_cSecretTeam != cTeamCode)
		{
			m_cSecretTeam = cTeamCode;	// ����

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
		m_cSecretTeam = 0;			// ����

		MASetGuildWarSecretMission packet = {0,};
		packet.cTeamCode = m_cSecretTeam;
		packet.nRandomSeed = -1;

		g_pDivisionManager->SendSetGuildWarSecretMission(&packet);
		g_Log.Log(LogType::_GUILDWAR, g_Config.nWorldSetID, 0, 0, 0, L"[GUILDWAR] SecretEnd Rate:%f!!!\n", fSecretRate );
	}
}
