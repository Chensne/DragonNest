#pragma once

class CFileChangeNotification
{
public:
	enum{
		eThread_State_Enabled = 0,		// 스레드가 활성화 가능 한 상태
		eThread_State_Executing,		// 스레드가 동작 하는 상태
		eThread_State_Request_Destroy,	// 스레드 종료 요청 상태
		eThread_State_Enable_Destroy,	// 스레드가 종료 될 수 있는 상태
	};

public:
	CFileChangeNotification();
	virtual ~CFileChangeNotification();

protected:
	static HANDLE	s_hThreadHandle;
	static int		s_nThreadStatus;
	UINT			m_dwThreadIndex;

	static std::wstring m_wszPath;
	static bool			m_bSubTree;
	static DWORD		m_FilterFlag;

	static UINT __stdcall ChangeNotification( void *pParam );

public:
	void StartFileChangeNotification( WCHAR * wszPath, bool bSubTree = false, DWORD FilterFlag = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS );
	void EndFileChangeNotification();
};