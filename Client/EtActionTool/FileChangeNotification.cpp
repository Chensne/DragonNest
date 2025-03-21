#include "stdafx.h"
#include "FileChangeNotification.h"

HANDLE CFileChangeNotification::s_hThreadHandle = NULL;
int CFileChangeNotification::s_nThreadStatus = CFileChangeNotification::eThread_State_Enabled;
std::wstring CFileChangeNotification::m_wszPath;
bool CFileChangeNotification::m_bSubTree = false;
DWORD CFileChangeNotification::m_FilterFlag = FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS;

CFileChangeNotification::CFileChangeNotification()
: m_dwThreadIndex( 0 )
{
}

CFileChangeNotification::~CFileChangeNotification()
{
	EndFileChangeNotification();
}

UINT __stdcall CFileChangeNotification::ChangeNotification( void *pParam )
{
	CFileChangeNotification * pThis = (CFileChangeNotification *)pParam;

	if( NULL == s_hThreadHandle )
	{
		s_nThreadStatus = eThread_State_Enable_Destroy;
		_endthreadex( 0 );
		return 0;
	}

	s_nThreadStatus = eThread_State_Executing;

	HANDLE hHandle = FindFirstChangeNotificationW( m_wszPath.c_str(), m_bSubTree, m_FilterFlag );

	if( INVALID_HANDLE_VALUE != hHandle )
	{
		do
		{
			if( WaitForMultipleObjects( 2, &hHandle, false, INFINITE ) == WAIT_OBJECT_0 )
			{
				// 파일이 변경 되었을 경우 처리를 해준다.
				break;
			}
		} while ( FindNextChangeNotification( hHandle ) );

		FindCloseChangeNotification( hHandle );
	}
	_endthreadex( 0 );

	return 0;
}

void CFileChangeNotification::StartFileChangeNotification( WCHAR * wszPath, bool bSubTree, DWORD FilterFlag )
{
	EndFileChangeNotification();

	s_hThreadHandle = (HANDLE)_beginthreadex( NULL, 65536, ChangeNotification, (void*)this, 0, &m_dwThreadIndex );
}

void CFileChangeNotification::EndFileChangeNotification()
{
	if( s_hThreadHandle )
	{
		s_nThreadStatus = eThread_State_Request_Destroy;
		while(1)
		{
			Sleep(1);
			if( eThread_State_Enable_Destroy == s_nThreadStatus )
				break;
		}

		CloseHandle( s_hThreadHandle );
		s_hThreadHandle = NULL;
		m_dwThreadIndex = 0;
	}
}