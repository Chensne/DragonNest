#pragma once

#include "Singleton.h"

class CPseudoRandom;
class CDNMissionScheduler : public CSingleton<CDNMissionScheduler>
{
public:
	CDNMissionScheduler();
	virtual ~CDNMissionScheduler();

protected:
	std::vector<int> m_nVecDailyMissionList[10][PLAYER_MAX_LEVEL][DAILYMISSIONMAX];
	std::vector<int> m_nVecWeeklyMissionList[10][PLAYER_MAX_LEVEL][WEEKLYMISSIONMAX];
#if defined(PRE_ADD_MONTHLY_MISSION)
	std::vector<int> m_nVecMonthlyMissionList[10][PLAYER_MAX_LEVEL][MONTHLYMISSIONMAX];
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

protected:

public:
	bool Initialize();

	void RefreshDailyMissionList( int nRandomSeed, TMissionGroup *pMission, int nLevel, int nClassID );
	void RefreshWeeklyMissionList( int nRandomSeed, TMissionGroup *pMission, int nLevel, int nClassID );
#if defined(PRE_ADD_MONTHLY_MISSION)
	void RefreshMonthlyMissionList( int nRandomSeed, TMissionGroup *pMission, int nLevel, int nClassID );
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
	void RefreshWeekendEventMissionList( TMissionGroup *pMission );
	void RefreshWeekendRepeatMissionList( TMissionGroup *pMission );

#if defined( PRE_ADD_STAMPSYSTEM )
	bool IsDailyMissionGroup( int nLevel, int nClassID, int nMissionGroupID, int nMissionID ) const;
#endif // #if defined( PRE_ADD_STAMPSYSTEM )
};