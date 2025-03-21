#pragma once

#include "MultiSingleton.h"

class CTask;
class CTaskManager : public CMultiSingleton<CTaskManager, MAX_SESSION_COUNT>, public TBoostMemoryPool< CTaskManager >
{
public:
	CTaskManager( CMultiRoom *pRoom );
	~CTaskManager();

	DWORD AddTask( CTask *pTask, const char *szName, int nSecureFrame, bool bUseThread = false );
	void RemoveAllTask();
	void RemoveTask( DWORD dwTaskHandle );
	void RemoveTask( const char *szName );

	CTask *GetTask( DWORD dwTaskHandle );
	CTask *GetTask( const char *szName );

	int GetTaskCount() { return (int)m_pVecTaskList.size(); }

	void EnableDebugTaskMode( bool bEnable ) { m_bLockDebugTask = bEnable; }
	bool Excute();

protected:
	std::vector<CTask *> m_pVecTaskList;
	bool m_bLockDebugTask;


	static UINT __stdcall BeginThread( void *pParam );
};

extern CTaskManager g_TaskManager;