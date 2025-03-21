#pragma once

#include "DNGuildWar.h"

//난봉꺼 욜루 옮겨서 수정...난봉꺼 최대한 살려보장
class CDNVillageConnection;

class CDNGuildWarManager
{
private:
	DWORD m_TickCheckGuildWar;	
	
	char m_cStepIndex;			// 현재 스텝	
	bool m_bWarEvent;			// 이벤트 진행여부
	short m_wScheduleID;		// 차수	
	short m_wWinersWeightRate;	// 승리팀가중치	
	__time64_t m_tRewardExpireDate;	// 보상(농장, 제스쳐) 만료 기간 
	CDNGuildWar* m_pWarEventStep[GUILDWAR_STEP_END];

	BYTE m_cSecretTeam;			// 시크릿 여부
	int m_nSecretRandomSeed;	// 시크릿 랜덤시드	

	int m_nFinalTeamCount;		// 본선 진출팀 카운트
	bool m_bFinalTeamSetting;	// 본선 진출팀 셋팅 여부

	TGuildUID m_PreWinGuildUID;	// 지난 차수의 우승 길드UID
	DWORD m_dwPreWinSkillCoolTime; // 지난 차수의 우승스킬 쿨타임

	// DB에 저장된 각종 데이터들..
	////////////////////////////////////////////////////////////////////////////////////////
	// 예선
	LONG m_nBlueTeamPoint;		// 블루팀 포인트
	LONG m_nRedTeamPoint;		// 레드팀 포인트
	// 각 회차별 그룹
	sTournamentGroup m_bTournamentGroup[GUILDWAR_TOURNAMENT_GROUP_MAX];
	// 전체 스케쥴
	TGuildWarEventInfo m_sGuildWarSchedule[GUILDWAR_STEP_END];
	// 본선 스케쥴
	TGuildWarEventInfo m_sGuildWarFinalSchedule[GUILDWAR_FINALPART_MAX];
	// 본선 대진표
	SGuildTournamentInfo m_GuildWarFinalInfo[GUILDWAR_FINALS_TEAM_MAX]; // 대진표 순 길드 정렬
	int	m_GuildWarOpeningPoints[GUILDWAR_FINALS_TEAM_MAX]; // 각 대진표애들의 예선 합계 포인트
	////////////////////////////////////////////////////////////////////////////////////////
	// 예선 24위까지의 현황
	SGuildWarRankingInfo m_sGuildWarPointTrialRanking[GUILDWAR_TRIAL_POINT_TEAM_MAX];
	short m_wPreWinScheduleID;		// 지난 우승 차수..
	bool m_bFrinalProgress;			// 본선 진행 여부	
	bool m_bFinalStart;				// 본선이 시작이 되었는지..

	// 셋팅여부
	eGuildWarSettingStep m_eSettingStep;	// 시작시 정보 셋팅 단계.
	bool m_bSendGuildWarInfo;			// 빌리지에게 정보 요청 여부
	//DWORD m_dwSendVillageTick;			// 빌리지에게 정보를 요청한 Tick

	bool m_bCheatFlag;
	bool m_bFinalWinGuild;				// 최종 우승자가 셋팅되어 있는지.(제스쳐, 농장 보상 여부)

	bool m_bResetSchedule;				// 스케쥴을 초기화 하고 전부 다시 로딩..
#if defined(PRE_FIX_75807)
	bool m_bFarmForceHarbest;			// 우승농장 초기화
#endif //#if defined(PRE_FIX_75807)

private:
	int  FindWorldEventIndex();

	bool CheckStep(char cType);

	bool RegisterScheduleGroup(short wScheduleID, const TGuildWarEventInfo vEventInfo[]);
	bool UpdateScheduleGroup(short wScheduleID, const TGuildWarEventInfo vEventInfo[]);

	void ResetStep();
	void StartStep();
	void NextStep();
	void FinalStep();

	void Clear();

public:
	CDNGuildWarManager();
	virtual ~CDNGuildWarManager();
	
	inline void SetSendGuildWarInfo(bool bSendGuildWarInfo) { m_bSendGuildWarInfo = bSendGuildWarInfo; };
	inline void SetGuildWarSettingStep(eGuildWarSettingStep eSettingStep) { m_eSettingStep = eSettingStep; };
	inline eGuildWarSettingStep GetGuildWarSettingStep() { return m_eSettingStep;};
	inline short GetScheduleID() { return m_wScheduleID;};
	inline int GetBlueTeamPoint() { return (int)m_nBlueTeamPoint; };
	inline int GetRedTeamPoint() { return (int)m_nRedTeamPoint; };
	inline TGuildWarEventInfo* GetGuildWarSchedule() { return m_sGuildWarSchedule;};
	inline TGuildWarEventInfo* GetGuildWarFinalSchedule() { return m_sGuildWarFinalSchedule;};
	inline short GetWinersWeightRate() { return m_wWinersWeightRate; };

	int SetGuildTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo);
	inline SGuildTournamentInfo* GetGuildTournamentInfo() { return m_GuildWarFinalInfo;};

	void SetGuildWarPointTrialRanking(const MASetGuildWarPointRunningTotal* pData);
	inline SGuildWarRankingInfo* GetGuildWarPointTrialRanking() { return m_sGuildWarPointTrialRanking;};

	void SetGuildTournamentInfoWin(char cIndex, bool bWin);
	void SetGuildWarTournamentResult(GAMAPvPGuildWarResult* pGuildWarResult);
	void SendSetGuildWarFinalResult(char cWinGuildIndex=-1, char cLoseGuildIndex=-1 );
	void SetGuildWarOpenningPoint(VIMASetGuildWarFinalTeam* pData );
	void SetGuildWarTournamentPoint(GAMAPvPGuildWarScore* pGuildWarPoint);
	void SendSetGuildWarTournamentWin(char cWinGuildIndex, char cFinalPart);

	bool LoadScheduleInfo(const VIMASetGuildWarSchedule* pData);	
	bool LoadFinalScheduleInfo(MASetGuildWarFinalSchedule* pData);
	void DoUpdate(DWORD CurTick);	
	
	CDNGuildWar* GetWarEvent(char cStep);
	inline char GetStepIndex() { return m_cStepIndex;};
	bool GetRestriction(int nSubCmd);

	void SendGuildWarInfoReq(); //빌리지에 각종 정보 요청.	
	void SetGuildWarPoint(int nBlueTeam, int nRedTeam);
	void AddGuildWarPoint(char cTeamType, int nAddPoint);
	void SetGuildWarPreWinGuild(TGuildUID GuildUID);

	inline void SetPreWinGuild(TGuildUID GuildUID) { m_PreWinGuildUID = GuildUID;};
	inline TGuildUID GetPreWinGuild() { return m_PreWinGuildUID; };	// 지난 차수의 우승 길드UID
	// 우승스킬 쿨타임
	inline DWORD GetPreWinSKillCoolTime() { return m_dwPreWinSkillCoolTime; };
	inline void SetPreWinSKillCoolTime(DWORD dwCoolTime ) { m_dwPreWinSkillCoolTime = dwCoolTime;};		
	UINT GetGuildDBIDWithFinal(const UINT uiPvPIndex, const WCHAR * pGuildName);

	bool IsTrialStats();
	void SetTrialStats();
	// 대진표짜기..
	void CalcGuildWarTournament(VIMASetGuildWarFinalTeam* pData);
	// 각 차수별 길드전방 셋팅
	void SetGuildWarTournamentGroup();
	// 길드전 방 생성
	void GuildWarCreateRoom();
	// 길드전 방 종료
	void GuildWarEndRoom();

	char GetCurFinalPart();
	time_t GetCurFinalPartBeginTime();
	inline int GetFinalTeamCount() { return m_nFinalTeamCount; };
	inline bool IsFinalTeamSetting() { return m_bFinalTeamSetting; };

	void CalcTeamSecret();
	inline BYTE GetSecretTeam() {return m_cSecretTeam;}
	inline int GetSecretRandomSeed() {return m_nSecretRandomSeed;}	

	bool GetCheatFlag () {return m_bCheatFlag;}
	void SetCheatFlag (bool bFlag) {m_bCheatFlag = bFlag;}

	// 방이 뽀사지거나 게임서버가 튕겼을때 호출바람..
	void SetDropTournament(UINT  unPvPIndex);
	void CalcGuildWarTournamentWin(BYTE cGroupIndex);

	void SetDBJobSend(bool bSend);
	void SetDBJobSeq(int nJobSeq);
	void SetDBJobSuccess(bool bSuccess);
	bool GetDBJobSuccess();
	void CalcGuildWarTournamentResult();

	inline void SetFinalWinGuild(bool bFinalWin) { m_bFinalWinGuild = bFinalWin;};
	inline void SetResetSchedule(bool bReset) { m_bResetSchedule = bReset;};
	inline void SetFinalStart(bool bStart) { m_bFinalStart = bStart;};
	inline bool GetFinalStart() { return m_bFinalStart;};
	inline bool GetFinalWinGuildReward() { return m_bFinalWinGuild;};
	inline void SetPreWinScheduleID(short wScheduleID) { m_wPreWinScheduleID = wScheduleID;};
	inline short GetPreWinScheduleID() { return m_wPreWinScheduleID; };
	inline void SetFinalProgress(bool bFinalProgress) { m_bFrinalProgress = bFinalProgress;};
	inline bool GetFinalProgress() { return m_bFrinalProgress;};
};

extern CDNGuildWarManager * g_pGuildWarManager;
