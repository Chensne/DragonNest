
#pragma once

#include "Timer.h"
class CFrameSync;
class CTask
{
public:
	CTask();
	virtual ~CTask();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}
	virtual UINT __stdcall BeginThread( void *pParam ) { return 0; }

	void DestroyTask( bool bDelete ) { m_bDestroyTask = true; m_bDeleteThis = bDelete; }

	void SetLocalTime( LOCAL_TIME LocalTime ) { m_LocalTime = LocalTime; }
	LOCAL_TIME GetLocalTime() { return m_LocalTime; }
	void SetPrevLocalTime( LOCAL_TIME LocalTime ) { m_PrevLocalTime = LocalTime; }
	LOCAL_TIME GetPrevLocalTime() { return m_PrevLocalTime; }

	void SetDeltaTime( float fValue ) { m_fDelta = fValue; }
	float GetDeltaTime() { return m_fDelta; }

	void EnableTaskProcess( bool bEnable );
	bool IsEnableTaskProcess() { return m_bEnableTaskProcess; }

	const char *GetTaskName() { return m_szTaskName.c_str(); }
	void SetTaskName( const char *szName ) { m_szTaskName = szName; }

	void ResetTimer();

	virtual LRESULT WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) { return 0; }

	void SetLocalTimeSpeed( float fSpeed, DWORD dwDelay = 0 );

	unsigned GetThreadID() { return m_dwThreadID; }

	virtual bool IsBusy() {return false;}

	virtual bool OnCloseTask() { return false; }

public:
	DWORD m_dwHandle;
	bool m_bUseThread;
	HANDLE m_hThreadHandle;
	unsigned m_dwThreadID;
	bool m_bEnableTaskProcess;
	std::string m_szTaskName;
	/*bool m_bProcessThread;*/

	LOCAL_TIME m_LocalTime;
	LOCAL_TIME m_PrevLocalTime;
	float m_fDelta;

	bool m_bDestroyTask;
	bool m_bDeleteThis;
	bool m_bResetTimer;

	CFrameSync *m_pFrameSync;

	HANDLE	m_hEndRequest;
};
