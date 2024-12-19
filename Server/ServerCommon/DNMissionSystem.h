#pragma once
#include "DNEventSystem.h"

class CDNUserSession;
class CDNMissionSystem:public TBoostMemoryPool<CDNMissionSystem>
{
public:
	CDNMissionSystem( CDNUserSession *pUser );
	virtual ~CDNMissionSystem();

public:
	enum DailyMissionTypeEnum {
		Daily,
		Weekly,
		GuildWar,
		PCBang,
		GuildCommon,
		WeekendEvent,
		WeekendRepeat,
#if defined(PRE_ADD_MONTHLY_MISSION)
		Monthly,
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
		DailyMissionTypeEnum_Amount,
	};

protected:
	CDNUserSession *m_pUserSession;
	std::vector<int> m_nVecMissionGainList[EventSystem::EventTypeEnum_Amount];
	std::vector<int> m_nVecMissionAchieveList[EventSystem::EventTypeEnum_Amount];
	std::vector<int> m_nVecDailyMissionList[DailyMissionTypeEnum_Amount][EventSystem::EventTypeEnum_Amount];

#if defined(PRE_ADD_ACTIVEMISSION)
	TActiveMissionData * m_pActiveMission;
	ActiveMission::ActiveMissionState m_ActiveMissionState;
#endif

	CSyncLock	m_Lock;

protected:
	bool CheckMetaMissionAchieveEvent( TMissionData *pData );

	void RequestDailyMissionReward( int nItemID );
	void UpdateLastAchieveList( int nArrayIndex );

	void RefreshOnOffMissionList();
	void RefreshDailyMissionList();
	void RefreshWeeklyMissionList();
	void RefreshWeekendEventMissionList();
	void RefreshGuildWarMissionList();
	void RefreshPCBangMissionList();
	void RefreshGuildCommonMissionList();
	void RefreshWeekendRepeatMissionList();
#if defined( PRE_ADD_MONTHLY_MISSION )
	void RefreshMonthlyMissionList();
#endif	// #if defined( PRE_ADD_MONTHLY_MISSION )

	void OnGainMission( int nArrayIndex );
	void OnAchieveMission( int nArrayIndex );

	void OnCounterDailyMission( DailyMissionTypeEnum Type, int nArrayIndex, int nCounter );
	void OnAchieveDailyMission( DailyMissionTypeEnum Type, int nArrayIndex, bool bSecret = false );
	void OnEventPopUp( int nArrayIndex );
	void OnHelpAlarm( int nArrayIndex );

public:
	bool LoadUserData();

	TDailyMission * GetDailyMission(CDNMissionSystem::DailyMissionTypeEnum iMissionType,int nArrayIndex);
	void CheckMissionGain( EventSystem::EventTypeEnum Event );
	void CheckMissionAchieve( EventSystem::EventTypeEnum Event ,std::vector<int> &nVecAchieveList );
	void CheckDailyMissionAchieve( EventSystem::EventTypeEnum Event );
#if defined(PRE_ADD_ACTIVEMISSION)
	void ResetActiveMission();
	void SetActiveMission( TActiveMissionData *pData );	
	int CheckActiveMissionAchieve( EventSystem::EventTypeEnum Event );	
#endif
	void ApplyPCBangMissionEffect( EventSystem::EventTypeEnum Event );
	void AddGuildPoint(TDailyMission * pDailyMission);
	bool AddGuildWarPoint( TDailyMission * pDailyMission, EventSystem::EventTypeEnum Event, TMissionGroup * pMission, int nArrayIndex );	

	bool OnEvent( EventSystem::EventTypeEnum Event );
	void OnGuildWarEvent (EventSystem::EventTypeEnum Event);

	void RequestDailyMissionList( DailyMissionTypeEnum Type, int nRandomSeed, bool bSend = true );
	void RequestDailyMissionAlarm( DailyMissionTypeEnum Type );

	// 치트용으로 만들어둔다.
	void RequestForceGainMission( int nItemID );
	void RequestForceAchieveMission( int nItemID );
	void RequestForceGainDailyMission( int nCount, ... );
	void RequestForceAchieveDailyMission( int nItemID );
	void RequestForceGainWeeklyMission( int nCount, ... );
#if defined(PRE_ADD_MONTHLY_MISSION)
	void RequestForceAchieveMonthlyMission( int nItemID );
	void RequestForceGainMonthlyMission( int nCount, ...);
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
	void RequestForceAchieveWeeklyMission( int nItemID );
	void RequestForceAchieveGuildWarMission( int nItemID );
	void RequestForceAchieveGuildCommonMission( int nItemID );
	void RequestMissionReward( int nArrayIndex );

	bool bIsDailyMission( const int nItemID );
	bool bIsWeeklyMission( const int nItemID );
#if defined(PRE_MOD_MISSION_HELPER)	
	bool bIsCommonMission( const int nItemID );
	bool bCheckDailyMission( const int nItemID, DNNotifier::Type::eType eType );
#endif
	bool bIsGuildWarMission();
	bool InitGuildWarMissionList( bool bCheckAchieve );
	bool bIsAchieveMission( const int nItemID ) const;

};