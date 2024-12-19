#pragma once

#include "Task.h"
#include "DnHangCrashTask.h"
#include "Singleton.h"

class CDnHangMonitorObject;

class CDnHangCrashTask : public CTask, public CSingleton<CDnHangCrashTask> {
public:
	CDnHangCrashTask( DWORD dwDetectTime = 60000 );
	virtual ~CDnHangCrashTask();

protected:
	DWORD m_dwDetectTime;
	std::vector<CDnHangMonitorObject *> m_pVecMonitorList;

public:
	bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	void InsertMonitorObject( CDnHangMonitorObject *pObject );
	void RemoveMonitorObject( CDnHangMonitorObject *pObject );
};

class CDnHangMonitorObject
{
public:
	CDnHangMonitorObject() {
		m_dwLastUpdateTime = 0;
		m_bPause = false;
		if( CDnHangCrashTask::IsActive() ) {
			CDnHangCrashTask::GetInstance().InsertMonitorObject( this );
		}
	}
	virtual ~CDnHangMonitorObject() {
		if( CDnHangCrashTask::IsActive() ) {
			CDnHangCrashTask::GetInstance().RemoveMonitorObject( this );
		}
	}

	void UpdateHang();
	DWORD GetLastUpdateTime() { return m_dwLastUpdateTime; }
	bool IsPauseHang() { return m_bPause; }

	void PauseHang() { m_bPause = true; }
	void ResumeHang() { m_bPause = false; m_dwLastUpdateTime = 0; }

protected:
	DWORD m_dwLastUpdateTime;
	bool m_bPause;
};

class CDnMonitorObjectPauseHelper
{
public:
	CDnMonitorObjectPauseHelper( CDnHangMonitorObject *pObject ) { m_pObject = pObject; pObject->PauseHang(); }
	~CDnMonitorObjectPauseHelper() { m_pObject->ResumeHang(); }

protected:
	CDnHangMonitorObject *m_pObject;
};