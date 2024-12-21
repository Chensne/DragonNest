#pragma once

#include "TimeSet.h"
#include "DNUserQuest.h"

class CDNPeriodQuest
{
public:
	CDNPeriodQuest();
	~CDNPeriodQuest();

	void	Init();

	void	Set(int nItemID, int nType, time_t tStartDate, time_t tEndDate, int nScheduleID, int nSelectCount);
	void	AddQuestID(int nQuestID);
	bool	CanAcceptDate() const;
	int		SelectQuestID(int nKey) const;
	bool	CheckSelectedQuestID( int nKey, int nSelectedID, UINT uiOffset ) const;

	const std::vector<int>& GetQuestIDs() const {return m_vecQuestIDs;}

	int		GetScheduleID() const {return m_nScheduleID;}

	time_t	GetEventTime(byte cIndex) const {return m_EventTime[cIndex];}
	void	SetEventTime(byte cIndex, time_t tEvent) {m_EventTime[cIndex] = tEvent;}


	bool	GetEventComplete(byte cIndex) {return m_EventComplete[cIndex];}
	void	SetEventComplete(byte cIndex, bool bFlag) {m_EventComplete[cIndex] = bFlag;}

	bool	IsType(int nType) const {return nType == m_nType;}
	int		GetType() const {return m_nType;}

	bool	GetActivate() const {return m_bActivate;}
	void	SetActivate(bool bActivate) {m_bActivate = bActivate;}
	int		GetItemID() const {return m_nItemID;}

	bool	GetOnEvent() const {return m_bOnEvent;}
	void	SetOnEvent(bool bEvent) {m_bOnEvent = bEvent;}

protected:
	int		m_nItemID;
	int		m_nType;
	int		m_nScheduleID;
	bool	m_bActivate;
	bool	m_bOnEvent;

	time_t	m_EventTime[PERIODQUEST_TIME_MAX];
	bool	m_EventComplete[PERIODQUEST_TIME_MAX];

	std::vector<int> m_vecQuestIDs;	// Äù½ºÆ® ID ±×·ì
	int		m_nSelectCount;
};

struct TWorldQuestInfo
{
	char cWorldSetID;
	int nScheduleID;
	int nCount;
};


class CDNPeriodQuestSystem
{
public:
	CDNPeriodQuestSystem();
	virtual ~CDNPeriodQuestSystem();

	bool	Initialize();
	void	BuildCompletedPeriodQuests(CDNUserQuest* pQuest);
	void	GetResetQuestList(CDNUserQuest* pQuest, int nType, std::vector<int>& vOutQuestList);
	void	MarkingSelectQuest(CDNUserQuest* pQuest);
	void	RefreshEventQuest(CDNUserQuest* pQuest, int nCount, const TCompleteEventQuest* pEventQuest);
	void	RefreshPeriodQuest(int nType, CDNUserQuest* pQuest);

	bool	FindEventScheduleID(int nQuestID, int& nScheduleID, __time64_t& tExpireDate);

	void	DoUpdate(DWORD CurTick);

	bool	CheckValidTime(time_t timeStart, time_t timeEnd);
	bool	SetActivateQuest(int nScheduleID, bool bFlag, time_t timeStart, time_t timeEnd);

	void	LoadWorldQuestInfo(char cWorldSetID);
	void	UpdateWorldEvent(char cWorldSetID, int nScheduleID, int nCount, bool bCheckNotice = false);
	int		GetWorldEventCount(char cWorldSetID, int nScheduleID);
	int		GetWorldEventStep(char cWorldSetID, int nScheduleID);
	bool	SetWorldEventTime(int nScheduleID, time_t tCollectStartDate, time_t tCollectEndDate, time_t tRewardDate);
	bool	IsPeriodQuest(int questID) const;

private:
	bool	RegisterBaseQuest(int nItemID, int nType, std::vector<string>& vQuestIDs, std::vector<string>& vCompleteDate, int nScheduleID, int nSelectCount);
	bool	IsValidType(int nType) const;
	bool	IsPeriodType(int nType) const;

	void	RefreshQuestKey(int nPeriodQuestType);

	void	OnStartEvent(CDNPeriodQuest* pQuest);
	void	OnEndEvent(CDNPeriodQuest* pQuest);

#if defined(_GAMESERVER)
	void	SendAssginPeriodQuestForGameServer(int nQuestID, bool bFlag);
	void	SendNoticePeriodQuestForGameServer(char cWorldSetID, int nItemID, int nNoticeCount);
#endif

private:
	std::vector <CDNPeriodQuest> m_vBaseQuest;

	std::vector<int> m_vQuestIDs[PERIODQUEST_RESET_MAX];
	int		m_nKeys[PERIODQUEST_RESET_MAX];				

	time_t	m_Time;
	tm *	m_pTime;
	int		m_nResetHour;
	int		m_nPeriodQuestCount;
	
	DWORD	m_TickCheckQuest;

	int		m_nWorldQuestIndex;

	// Key : <WorldSetID, VecData>
	typedef std::map <char, std::vector<TWorldQuestInfo>> MWorldQuest;
	typedef MWorldQuest::iterator MWorldQuestItor;
	MWorldQuest m_mWorldEventQuest;
	bool	m_bInitFlag;

	std::set<int> m_PeriodQuestIDs;
};

extern CDNPeriodQuestSystem * g_pPeriodQuestSystem;
