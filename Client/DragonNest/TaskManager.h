#pragma once
#include "Singleton.h"

class CTask;
class CTaskManager : public CSingleton<CTaskManager>
{
public:
	CTaskManager();
	~CTaskManager();

	DWORD AddTask( CTask *pTask, const char *szName, int nSecureFrame, bool bUseThread = false );
	void RemoveAllTask( bool bImmediate = true );
	void RemoveTask( DWORD dwTaskHandle, bool bImmediate = true );
	void RemoveTask( const char *szName, bool bImmediate = true );

	CTask *GetTask( DWORD dwTaskHandle );
	CTask *GetTask( const char *szName );

	int GetTaskCount() { return (int)m_pVecTaskList.size(); }

	void EnableDebugTaskMode( bool bEnable ) { m_bLockDebugTask = bEnable; }
	bool Execute();
	void DebugTestLag( int nLag ) { m_nDebugTestLag = nLag;}

	LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	bool OnCloseTask();

protected:
	std::vector<CTask *> m_pVecTaskList;
	bool m_bLockDebugTask;
	int		m_nDebugTestLag;
	LOCAL_TIME m_PrevGlobalTime;


	static UINT __stdcall BeginThread( void *pParam );
};

extern CTaskManager g_TaskManager;