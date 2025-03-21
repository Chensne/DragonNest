#pragma once

class CDNGuildWarManager
{
private:
	char m_cStepIndex;				// 현재 진행중인 길드전 스텝
	int m_nBlueTeamPoint;			// 블루팀 점수
	int m_nRedTeamPoint;			// 레드팀 점수
	short m_wScheduleID;			// 스케쥴 ID
	short m_wWinersWeightRate;		// 승리팀 가중치

	bool m_bGuildWarStats;			// 길드전 예선 결과 로딩 완료 여부	

	// 현재 진행중인 차수 본선 스케쥴.	
	char m_cGuildWarFinalPart;
	__time64_t m_tGuildWarFinalPartBeginTime;

	TGuildUID m_PreWinGuildUID;	// 지난 차수의 우승 길드UID
	DWORD m_dwPreWinSkillCoolTime;	// 우승 스킬 쿨타임

	// 전체 스케쥴
	TGuildWarEventInfo m_sGuildWarSchedule[GUILDWAR_STEP_END-1];
	// 본선 스케쥴
	TGuildWarEventInfo m_sGuildWarFinalSchedule[GUILDWAR_FINALPART_MAX-1];

	// 변하지 않는 순위 정보
	// 본선 진출 길드
	SGuildWarRankingInfo	m_sGuildWarRankingInfo[GUILDWAR_FINALS_TEAM_MAX];	
	int						m_nGuildWarTrialRankingCount;
	SGuildWarRankingInfo	m_sGuildWarTrialRankingInfo[GUILDWAR_TRIAL_POINT_TEAM_MAX];
	bool					m_bFinalProgress;

	// 길드 순위
	SGuildWarMissionGuildRankingInfo m_sGuildWarMissionGuildRankingInfo[GUILDWAR_RANKINGTYPE_MAX];	// 전체 길드 순위
	
	// 개인 순위
	INT64				m_nGuildWarMissionRankingCharacterDBID[GUILDWAR_RANKINGTYPE_MAX];	// 개인별 순위에 든 넘의 CharacterDBID(점수 계산용)
	INT64				m_nGuildWarDailyAwardCharacterDBID[GUILDWAR_DAILY_AWARD_MAX];		// 일자별 시상에 든 넘의 CharacterDBID(점수 계산용)

	SGuildWarMissionRankingInfo m_sGuildWarMissionRankingInfo[GUILDWAR_RANKINGTYPE_MAX];		// 개인별 순위	

	// 일자별 시상
	SGuildWarDailyAward			m_sGuildWarDailyAward[GUILDWAR_DAILY_AWARD_MAX];	

	// 본선 대진표
	SGuildTournamentInfo		m_sGuildWarFinalInfo[GUILDWAR_FINALS_TEAM_MAX]; // 대진표 순 길드 정렬

	// 인기투표 1위 정보
	bool m_bGetGuildWarVote;					// 정보 꺼내왔는지 여부
	WCHAR m_wszVoteGuildName[GUILDNAME_MAX];	// 길드 이름
	int m_nVoteCount;						// 총득표수

	BYTE m_cSecretTeam;
	int m_nSecretRandomSeed;

	int m_vSecretMissionID[GUILDWARMISSION_MAXGROUP];

public:
	CDNGuildWarManager();
	virtual ~CDNGuildWarManager();

	void Reset();
	inline short GetScheduleID() { return m_wScheduleID; };
	inline char GetStepIndex() { return m_cStepIndex; };
	void SetStepIndex (char cStepIndex) {m_cStepIndex = cStepIndex;}
	bool GetRestriction(int nSubCmd, TGuildUID GuildUID);
	void SetGuildWarStep(char cStepIndex, short wScheduleID, short wWinersWeightRate );
	inline bool GetGuildWarStats() { return m_bGuildWarStats;};

	inline void SetTeamPoint(int nBlueTeamPoint, int nRedTeamPoint) { m_nBlueTeamPoint=nBlueTeamPoint; m_nRedTeamPoint=nRedTeamPoint; CalcTeamSecret();};
	inline int GetBlueTeamPoint() { return m_nBlueTeamPoint; };
	inline int GetRedTeamPoint() { return m_nRedTeamPoint; };
	inline void AddBlueTeamPoint(int nAddPoint) { m_nBlueTeamPoint += nAddPoint; CalcTeamSecret();};
	inline void AddRedTeamPoint(int nAddPoint) { m_nRedTeamPoint += nAddPoint; CalcTeamSecret();};

	inline void SetGuildWarVoteCount(int nVoteCount) { m_nVoteCount=nVoteCount;};
	inline int GetGuildWarVoteCount() { return m_nVoteCount; };
	inline void SetGuildWarVote(bool bVote) { m_bGetGuildWarVote = bVote;};
	inline bool GetGuildWarVote() { return m_bGetGuildWarVote;};
	inline WCHAR* GetGuildWarVoteGuildName() { return m_wszVoteGuildName; };
	inline void SetGuildWarVoteGuildName(WCHAR* wszGuildName) { memcpy(m_wszVoteGuildName, wszGuildName, sizeof(m_wszVoteGuildName)); };

	bool IsGuildWarFinal(TGuildUID nGuildUID);	
	
	bool IsGuildWarTrialWIn(char cTeamType);

	int CalcGuildWarFestivalPoint(char cTeamType, INT64 nCharacterDBID, int nGuildWarPoint);
	int GetGuildAwardCount(INT64 nCharacterDBID);

	// 길드 스케쥴
	void SetGuildWarSechdule(MASetGuildWarEventTime* SetGuildWarEventTime);
	inline TGuildWarEventInfo* GetGuildWarSchedule() { return m_sGuildWarSchedule;};
	inline TGuildWarEventInfo* GetGuildWarFinalSchedule() { return m_sGuildWarFinalSchedule;};

	// 지난 차수 우승길드
	inline TGuildUID GetPreWinGuildUID() { return m_PreWinGuildUID; };
	inline void SetPreWinGuildUID(TGuildUID GuildUID) { m_PreWinGuildUID = GuildUID;};
	inline void ResetPreWinGuildUID() { m_PreWinGuildUID.Reset(); };

	// 우승스킬 쿨타임
	inline DWORD GetPreWinSKillCoolTime() { return m_dwPreWinSkillCoolTime; };
	inline void SetPreWinSKillCoolTime(DWORD dwCoolTime ) { m_dwPreWinSkillCoolTime = dwCoolTime;};	
	
	// 본선 진출 길드
	void SetGuildWarRankingInfo(TAGetGuildWarPointGuildTotal* pGetGuildWarPointGuildTotal );
	inline SGuildWarRankingInfo* GetGuildWarRankingInfo() { return m_sGuildWarRankingInfo; };
	int GetGuildWarPoint(UINT nGuildDBID);

	void SetGuildWarTrialRankingInfo(MASetGuildWarPointRunningTotal* pSetGuildWarPointRunningTotal );
	inline SGuildWarRankingInfo* GetGuildWarTrialRankingInfo() { return m_sGuildWarTrialRankingInfo; };	
	inline int GetGuildWarTrialRankingCount() { return m_nGuildWarTrialRankingCount;};
	inline bool GetFinalProgress() { return m_bFinalProgress; };

	// 부문별 길드 순위 셋팅
	void SetGuildWarMissionGuildRankingInfo(TAGetGuildWarPointGuildPartTotal* pGetGuildWarPointGuildPartTotal);
	inline SGuildWarMissionGuildRankingInfo* GetGuildWarMissionGuildRankingInfo() { return m_sGuildWarMissionGuildRankingInfo; };

	// 부문별 개인 순위 셋팅
	void SetGuildWarMissionRankingInfo(TAGetGuildWarPointPartTotal* pGetGuildWarPointPartTotal);
	inline SGuildWarMissionRankingInfo* GetGuildWarMissionRankingInfo() { return m_sGuildWarMissionRankingInfo; };

	// 날짜별 시상
	void SetGuildDailyAward(TAGetGuildWarPointDaily* pGetGuildWarPointDaily);
	inline SGuildWarDailyAward* GetGuildDailyAward() { return m_sGuildWarDailyAward; };	

	// 대진표	
	void SetGuildTournamentInfo(SGuildTournamentInfo* pGuildTournamentInfo);
	inline SGuildTournamentInfo* GetGuildTournamentInfo() { return m_sGuildWarFinalInfo;};
	// 본선 결과
	void SetGuildTournamentResult(MAVISetGuildWarFinalResult* pGuildTournamentResult);

	// 본선 스케쥴	
	void SetGuildWarFinalPart(char cGuildFinalPart, __time64_t tFinalPartBeginTime);
	inline char GetGuildWarFinalPart() { return m_cGuildWarFinalPart;};
	inline __time64_t GetGuildWarFinalPartBeginTime() { return m_tGuildWarFinalPartBeginTime; };	

	// 시크릿 정보
	bool IsSecretMission(BYTE cTeamCode, int nMissionID);
	void SetGuildSecretMission(MASetGuildWarSecretMission* pSecret);
	void CalcTeamSecret();

	// 길드마크
	void OnRecvMaGuildChangeMark( MAGuildChangeMark* pPacket );

	// 길드명 변경
	void OnRecvMaGuildChangeName( MAGuildChangeName* pPacket );	
};

extern CDNGuildWarManager * g_pGuildWarManager;

