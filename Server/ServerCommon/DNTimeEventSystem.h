#pragma once

#include "TimeSet.h"

class CDNUserSession;

class CDNTimeEventSystem
{
public:
	CDNTimeEventSystem( CDNUserSession *pSession );
	virtual ~CDNTimeEventSystem();

	enum MaintenanceType {
		Endure,
		Accumelate,
	};
	enum RepeatType {
		None,
		Repeat,
		Daily,
	};
	struct TimeEventStruct {
		int nEventID;
		MaintenanceType Type;
		CTimeSet tBeginTime;
		CTimeSet tEndTime;
		RepeatType RepeatType;
		INT64 nMaintenanceTime;
		INT64 nRemainTime;
		int nRewardMissionID;
//		bool bDailyOnly;
		bool bDailyFlag;
	};

protected:
	CDNUserSession *m_pUserSession;
	std::vector<TimeEventStruct *> m_pVecEventList;

	DWORD m_dwPrevTime;

protected:
	void RegisterTableData();
	void UpdateEventStructFromUserData();
	void UpdateUserDataFromEventStruct( bool bLogout = false, bool bSaveDB = true );

	TimeEventStruct *GetTimeEvent( int nEventID );

	void OnAchieveTimeEvent( TimeEventStruct *pStruct );
	void OnExpireTimeEvent( TimeEventStruct *pStruct );
public:
	bool LoadUserData();
	bool SaveUserData( bool bLogout );

	virtual void Process( DWORD dwCurTick );
	void RequestSyncTimeEvent();

	void ResetDailyTimeEvent();

};