#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DnSkill.h"

#if defined(PRE_FIX_NEXTSKILLINFO)
#include "DnTableDB.h"
#endif // PRE_FIX_NEXTSKILLINFO

#define MAX_GUILD_WAR_SKILL_POINT 8

class CDnGuildWarTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnGuildWarTask>
{
public:
	CDnGuildWarTask();
	virtual ~CDnGuildWarTask();

public:
	bool Initialize();
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

public:
	struct stPreliminaryStatus	// 길드전 예선 현황
	{
		int		nBluePoint;			// 청팀 점수	
		int		nRedPoint;			// 홍팀 점수	
		int		nMyPoint;			// 내 점수
		int		nMyGuildPoint;		// 길드 현재 점수

		stPreliminaryStatus() : nBluePoint( 0 ), nRedPoint( 0 ), nMyPoint( 0 ), nMyGuildPoint( 0 ) {}
	};

	struct stPreliminaryResult		// 길드전 예선 결과
	{
		int		nBlueFinalPoint;	// 청팀 최종 점수	
		int		nRedFinalPoint;		// 홍팀 최종 점수
		bool	bMyWin;				// 길드전 내 결과
		bool	bJoinGuildWar;		// 길드전 참여 여부
		bool	bReward;			// 보상받기 여부
		int		nFestivalPoint;		// 길드전 결과 내 획득 포인트
		int		nFestivalPointAdd;	// 길드전 결과 내 획득 포인트 추가 포인트

		stPreliminaryResult() : nBlueFinalPoint( 0 ), nRedFinalPoint( 0 ), bMyWin( false ), bJoinGuildWar( false ), bReward( false ), nFestivalPoint( 0 ), nFestivalPointAdd( 0 ) {}
	};

	struct stJoinGuildRankInfo			// 길드전 예선 결과 - 길드 순위 정보
	{
		int		m_nGuildRank;			// 순위
		int		m_nGuildUserCount;		// 인원수
		int		m_nGuildUserMax;		// 최대 길드 인원수
		int		m_nTotalPoints;			// 총점
		tstring m_strGuildName;			// 길드명
		tstring m_strGuildMasterName;	// 길드장

		stJoinGuildRankInfo() : m_nGuildRank( 0 ), m_nGuildUserCount( 0 ), m_nGuildUserMax( 0 ), m_nTotalPoints( 0 ), m_strGuildName( L"" ), m_strGuildMasterName( L"" ) {}
	};

	struct stEventRankInfo			// 길드전 예선 결과 - 부문별 순위 정보
	{
		int		m_nRank;			// 순위
		int		m_nTotalPoints;		// 총점
		tstring m_strName1;			// 이름1
		tstring m_strName2;			// 이름2

		stEventRankInfo() : m_nRank( 0 ), m_nTotalPoints( 0 ), m_strName1( L"" ), m_strName2( L"" ) {}
	};

	struct stTournamanetGuild
	{
		TGuildUID		m_tGuildUID;
		tstring			m_strGuildName;
		TGuildSelfView	m_tGuildSelfView;
		bool			m_bWin;
		char			m_cMatchTypeCode;

		stTournamanetGuild() : m_strGuildName( L"" ), m_bWin( false ), m_cMatchTypeCode( 0 ) {}
	};

	struct stGuildTournamentInfo
	{
		stTournamanetGuild	m_TournamentGuild[GUILDWAR_FINALS_TEAM_MAX];
		char				m_cMatchTypeCode;				// 1=결승, 2=4강, 3=8강, 4=16강
		__time64_t			m_tStartTime;					// 이게 0이면 현재 진행중이고 값이 있으면 이때 시작하는 것입니다.
		bool				m_bPopularityVote;				// 인기투표 활성화, 비활성화

		stGuildTournamentInfo() : m_cMatchTypeCode( 0 ), m_tStartTime( 0 ), m_bPopularityVote( false ) {}
	};

	struct SGuildWarEventTime
	{
		SGuildWarTime term;
		std::wstring timeString[GUILDWAR_EVENT_MAX];

		SGuildWarEventTime() { memset(&term, 0, sizeof(term)); }
	};

	struct stGuildWarShopItemInfo		// 길드전 상점 구매 내역 기록
	{
		int			m_nItemID;
		int			m_nCurrentBuyCount;

		stGuildWarShopItemInfo() : m_nItemID( -1 ), m_nCurrentBuyCount( 0 ) {}
	};

public:
	stPreliminaryStatus& GetPreliminaryStatus() { return m_stPreliminaryStatus; }
	stPreliminaryResult& GetPreliminaryResult() { return m_stPreliminaryResult; }
	std::vector<stJoinGuildRankInfo>& GetPreliminaryResultGuildRanking() { return m_vecPreliminaryResultGuildRanking; }
	std::vector<stEventRankInfo>& GetEventGuildRankInfo() { return m_vecEventGuildRankInfo; }
	std::vector<stEventRankInfo>& GetEventPersonalRankInfo() { return m_vecEventPersonalRankInfo; }
	std::vector<tstring>& GetDailyBestChar() { return m_vecDailyBestChar; }
	stGuildTournamentInfo& GetGuildTournamentInfo() { return m_stGuildTournamentInfo; }
	void SetGuildWarState( int nGuildWarState ) { m_nGuildWarState = nGuildWarState; }
	int GetGuildWarState() { return m_nGuildWarState; }
	int GetGuildWarSkillUsedPoint() { return m_nGuildWarSkillUsedPoint; }
	int GetGuildWarSkillLeavePoint() { return m_nGuildWarSkillLeavePoint; }
	void UseGuildWarSkillPoint();
	INT64 GetGuildWarFestivalPoint() { return m_biGuildWarFestivalPoint; }
	eGuildWarStepType	GetCurrentGuildWarEventTimeType() const { return m_CurGuildWarStep; }
	eGuildFinalPart		GetCurrentGuildWarFinalRoundStep() const { return m_CurGuildWarFinalRoundStep; }
	void GetGuildWarEventTime(std::wstring& startTime, std::wstring& endTime, eGuildWarStepType type) const;
	void GetGuildWarFinalRoundTime(std::wstring& startTime, std::wstring& endTime, eGuildFinalPart step) const;
	void SetForceUpdateAlarm(bool bSet)	{ m_bSetForceUpdateAlarm = bSet; }
	bool GetGuildWarFinalProgress() { return m_bGuildWarFinalProgress; }

	void SendGuildWarVote( int nGuildIndex );
	int GetGuildWarShopItemCurrentBuyCount( int nItemID );
	void ResetGuildWarSkillPoint();

private:
	void OnRecvGuildWarTrialResultOpenMsg();
	void OnRecvGuildWarTournamentOpenMsg();
	void OnRecvGuildWarStatusOpenMsg( SCGuildWarStatus* pPacket );
	void OnRecvGuildWarTrialResultInfoMsg( SCGuildWarTrialResult* pPacket );
	void OnRecvGuildWarTournamentInfoMsg( SCGuildTournamentInfo* pPacket );
	void OnRecvGuildWarModeState( SCPvPOccupationModeState* pPacket );
	void OnRecvGuildWarFestivalPoint( SCGuildWarFestivalPoint* pPacket );
	void OnRecvGuildWarGetReward( SCGuildWarCompensation* pPacket );
	void OnRecvGuildWarVote( SCGuildWarVote* pPacket );
	void OnRecvGuildWarVoteTop( SCGuildWarVoteTop* pPacket );
	void OnRecvGuildWarEventTime( SCGuildWarEventTime* pPacket );
	void OnRecvGuildWarShopItemInfoList( SCBuyedItemCount* pPacket );
	void OnRecvGuildWarShopItemInfo( SCBuyedItemCount* pPacket );

protected:
	stPreliminaryStatus					m_stPreliminaryStatus;					// 길드전 예선 현황 데이터
	stPreliminaryResult					m_stPreliminaryResult;					// 길드전 결과 데이터
	std::vector<stJoinGuildRankInfo>	m_vecPreliminaryResultGuildRanking;		// 0:내 길드 1~ 실제 길드 순위
	std::vector<stEventRankInfo>		m_vecEventGuildRankInfo;				// 부분별 길드 랭킹
	std::vector<stEventRankInfo>		m_vecEventPersonalRankInfo;				// 부분별 개인 랭킹
	std::vector<tstring>				m_vecDailyBestChar;						// 일자별 최고 점수상
	stGuildTournamentInfo				m_stGuildTournamentInfo;				// 길드전 토너먼트 데이터
	std::vector<stGuildWarShopItemInfo> m_vecGuildWarShopItemInfo;				// 길드전 상점 구매 내역 기록
	int									m_nGuildWarState;						// 길드전 상태
	int									m_nGuildWarSkillUsedPoint;				// 길드전 사용 스킬 포인트
	int									m_nGuildWarSkillLeavePoint;				// 길드전 남은 스킬 포인트

	INT64								m_biGuildWarFestivalPoint;				// 길드축제 포인트
	std::map<eGuildWarStepType, SGuildWarEventTime>	m_GuildWarEventTime;
	std::map<eGuildFinalPart, SGuildWarEventTime>	m_GuildWarFinalRoundTime;
	eGuildWarStepType								m_CurGuildWarStep;
	eGuildFinalPart									m_CurGuildWarFinalRoundStep;
	bool											m_bSetForceUpdateAlarm;
	bool											m_bGuildWarFinalProgress;

#if defined(PRE_FIX_NEXTSKILLINFO)
public:
	SKILL_LEVEL_INFO* GetSkillLevelTableIDList(int nSkillID, int nApplyType);

protected:
	static SKILL_LEVEL_INFO ms_SkillLevelTableInfo;
	SKILL_LEVEL_TABLEID_LIST m_SkillLevelTableIDList[2];

	void InitSkillLevelTableIDList();
	void AddSkillLevelTableID(int nSkillID, int nSkillLevel, int nSkillLevelTableID, int nApplyType);

#endif // PRE_FIX_NEXTSKILLINFO
};

#define GetGuildWarTask()		CDnGuildWarTask::GetInstance()



// CDnGuildWarSkill

class CDnGuildWarSkill : public CDnSkill
{
public:
	CDnGuildWarSkill( DnActorHandle hActor ) : CDnSkill( hActor ) {}
	~CDnGuildWarSkill() {}

public:
	static DnSkillHandle CreateSkill( DnActorHandle hActor, int iSkillTableID, int iLevel );
	bool Initialize( int iSkillTableID, int iLevel );
	bool LoadSkillLevelData( int iSkillTableID, int iLevel );
	virtual MIInventoryItem::ItemConditionEnum GetItemCondition();
	CDnSkill::UsingResult CanExecuteGuildWarSkill();
};

