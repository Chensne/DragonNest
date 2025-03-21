#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "TimeSet.h"
#include "DnItem.h"

class CDnTimeEventTask : public CTask, public CTaskListener, public CSingleton<CDnTimeEventTask> {
public:
	CDnTimeEventTask();
	virtual ~CDnTimeEventTask();

	enum RepeatTypeEnum {
		None,
		Repeat,
		Daily,
	};
	struct TimeEventInfoStruct {
		int nEventID;
		CTimeSet BeginTime;
		CTimeSet EndTime;
		tstring szTitle;
		tstring szDescription;
		RepeatTypeEnum RepeatType;
		std::vector<CDnItem *> pVecRewardItemList;
		vector<int> VecRewardItemCountList;
		int nRewardCoin;
		INT64 nMaintenanceTime;
		INT64 nRemainTime;
		bool bDailyFlag;

		~TimeEventInfoStruct() {
			SAFE_DELETE_PVEC( pVecRewardItemList );
		};
	};

protected:
	std::vector<TimeEventInfoStruct *> m_pVecTimeEventList;
	int m_nSelectAlarmEventID;

protected:
	virtual void OnRecvTimeEventMessage( int nSubCmd, char *pData, int nSize );
	void InsertTimeEvent( int nEventID, INT64 nRemainTime, bool bCheckFlag );
	void RemoveTimeEvent( int nEventID );
	
	int FindUrgentEventID();
	void CalcRewardItemList( /*OUT */ std::vector<CDnItem *> &pVecResult,  /*OUT*/ std::vector<int> &VecItemCountResult,
							 std::vector<int> &nVecItemList, std::vector<int> &nVecCashItemList,
							 std::vector<int> &nVecRewardItemCountList, std::vector<int> &nVecRewardCashItemCountList );

	bool IsProcess();

#ifdef PRE_ADD_STAMPSYSTEM
	void OnRecvStampEventMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvStampInit( char * pData, class CDnStampEventDlg * pDlg );
	void OnRecvStampAddComplete( char * pData, class CDnStampEventDlg * pDlg );
	void OnRecvStampClearComplete( class CDnStampEventDlg * pDlg );
	void OnRecvStampChangeWeekDay( char * pData, class CDnStampEventDlg * pDlg );
	void OnRecvStampChangeTable( char * pData, class CDnStampEventDlg * pDlg );
#endif // PRE_ADD_STAMPSYSTEM


public:
	bool Initialize();
	void Finalize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnRecvTimeEventList( SCTimeEventList* pPacket );
	virtual void OnRecvTimeEventAchieve( SCTimeEventAchieve* pPacket );
	virtual void OnRecvTimeEventExpire( SCTimeEventExpire* pPacket );

	DWORD GetTimeEventCount();
	TimeEventInfoStruct *GetTimeEventInfo( DWORD dwIndex );
	TimeEventInfoStruct *FindTimeEventInfo( int nEventID );
	void SetTimeEventAlarm( int nEventID );
	int GetTimeEventAlarm() { return m_nSelectAlarmEventID; }
};

#define GetTimeEventTask()		CDnTimeEventTask::GetInstance()