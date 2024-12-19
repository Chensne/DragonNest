#pragma once

#include "DNGuildWar.h"

//������ ��� �Űܼ� ����...������ �ִ��� �������
class CDNVillageConnection;

class CDNGuildWarManager
{
private:
	DWORD m_TickCheckGuildWar;	
	
	char m_cStepIndex;			// ���� ����	
	bool m_bWarEvent;			// �̺�Ʈ ���࿩��
	short m_wScheduleID;		// ����	
	short m_wWinersWeightRate;	// �¸�������ġ	
	__time64_t m_tRewardExpireDate;	// ����(����, ������) ���� �Ⱓ 
	CDNGuildWar* m_pWarEventStep[GUILDWAR_STEP_END];

	BYTE m_cSecretTeam;			// ��ũ�� ����
	int m_nSecretRandomSeed;	// ��ũ�� �����õ�	

	int m_nFinalTeamCount;		// ���� ������ ī��Ʈ
	bool m_bFinalTeamSetting;	// ���� ������ ���� ����

	TGuildUID m_PreWinGuildUID;	// ���� ������ ��� ���UID
	DWORD m_dwPreWinSkillCoolTime; // ���� ������ ��½�ų ��Ÿ��

	// DB�� ����� ���� �����͵�..
	////////////////////////////////////////////////////////////////////////////////////////
	// ����
	LONG m_nBlueTeamPoint;		// ����� ����Ʈ
	LONG m_nRedTeamPoint;		// ������ ����Ʈ
	// �� ȸ���� �׷�
	sTournamentGroup m_bTournamentGroup[GUILDWAR_TOURNAMENT_GROUP_MAX];
	// ��ü ������
	TGuildWarEventInfo m_sGuildWarSchedule[GUILDWAR_STEP_END];
	// ���� ������
	TGuildWarEventInfo m_sGuildWarFinalSchedule[GUILDWAR_FINALPART_MAX];
	// ���� ����ǥ
	SGuildTournamentInfo m_GuildWarFinalInfo[GUILDWAR_FINALS_TEAM_MAX]; // ����ǥ �� ��� ����
	int	m_GuildWarOpeningPoints[GUILDWAR_FINALS_TEAM_MAX]; // �� ����ǥ�ֵ��� ���� �հ� ����Ʈ
	////////////////////////////////////////////////////////////////////////////////////////
	// ���� 24�������� ��Ȳ
	SGuildWarRankingInfo m_sGuildWarPointTrialRanking[GUILDWAR_TRIAL_POINT_TEAM_MAX];
	short m_wPreWinScheduleID;		// ���� ��� ����..
	bool m_bFrinalProgress;			// ���� ���� ����	
	bool m_bFinalStart;				// ������ ������ �Ǿ�����..

	// ���ÿ���
	eGuildWarSettingStep m_eSettingStep;	// ���۽� ���� ���� �ܰ�.
	bool m_bSendGuildWarInfo;			// ���������� ���� ��û ����
	//DWORD m_dwSendVillageTick;			// ���������� ������ ��û�� Tick

	bool m_bCheatFlag;
	bool m_bFinalWinGuild;				// ���� ����ڰ� ���õǾ� �ִ���.(������, ���� ���� ����)

	bool m_bResetSchedule;				// �������� �ʱ�ȭ �ϰ� ���� �ٽ� �ε�..
#if defined(PRE_FIX_75807)
	bool m_bFarmForceHarbest;			// ��³��� �ʱ�ȭ
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

	void SendGuildWarInfoReq(); //�������� ���� ���� ��û.	
	void SetGuildWarPoint(int nBlueTeam, int nRedTeam);
	void AddGuildWarPoint(char cTeamType, int nAddPoint);
	void SetGuildWarPreWinGuild(TGuildUID GuildUID);

	inline void SetPreWinGuild(TGuildUID GuildUID) { m_PreWinGuildUID = GuildUID;};
	inline TGuildUID GetPreWinGuild() { return m_PreWinGuildUID; };	// ���� ������ ��� ���UID
	// ��½�ų ��Ÿ��
	inline DWORD GetPreWinSKillCoolTime() { return m_dwPreWinSkillCoolTime; };
	inline void SetPreWinSKillCoolTime(DWORD dwCoolTime ) { m_dwPreWinSkillCoolTime = dwCoolTime;};		
	UINT GetGuildDBIDWithFinal(const UINT uiPvPIndex, const WCHAR * pGuildName);

	bool IsTrialStats();
	void SetTrialStats();
	// ����ǥ¥��..
	void CalcGuildWarTournament(VIMASetGuildWarFinalTeam* pData);
	// �� ������ ������� ����
	void SetGuildWarTournamentGroup();
	// ����� �� ����
	void GuildWarCreateRoom();
	// ����� �� ����
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

	// ���� �ǻ����ų� ���Ӽ����� ƨ������ ȣ��ٶ�..
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
