#include "stdafx.h"
#include "WiseLog.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(_KR)

WiseLog::WiseLog()
: m_nThreadID( 0 ), m_hThreadHandle( INVALID_HANDLE_VALUE ), m_bRunning(false)
{
	strcpy_s(m_szThreadNick, "WiseLogThread");
	InitializeCriticalSectionAndSpinCount(&m_Lock, 2000);
	m_hThreadSignal = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_uMyOid = 0;
}

WiseLog::~WiseLog()
{
	Destroy();
}

bool WiseLog::Start(UINT uMyOid)
{
	/*
	m_uMyOid = uMyOid;

	if( m_hThreadHandle != INVALID_HANDLE_VALUE )
		return false;

	m_bRunning = true;

	m_hThreadHandle = ::CreateThread( 0, 0, _Runner, ( LPVOID )this, 0, &m_nThreadID );
	if( m_hThreadHandle == 0 )
		return false;

	return true;
}

void WiseLog::Destroy()
{
	/*
	EnterCriticalSection(&m_Lock);
	while ( m_WiseLogQueue.size() )
	{
		EnterCriticalSection(&m_Lock);
		std::string szUrl = m_WiseLogQueue.front();
		m_WiseLogQueue.pop();
		LeaveCriticalSection(&m_Lock);

		SendWebPost(szUrl.c_str());
	}
	LeaveCriticalSection(&m_Lock);

	m_bRunning = false;
	LeaveCriticalSection(&m_Lock);
	DeleteCriticalSection(&m_Lock);
	m_bRunning = false;

	if( m_hThreadHandle != INVALID_HANDLE_VALUE )
	{
		m_bRunning = false;
		//CloseHandle( m_hThreadHandle );
		//m_hThreadHandle = INVALID_HANDLE_VALUE;
	}
	*/
}

bool WiseLog::WriteToWiseLog(const char* szLogFile)
{
	/*
	EnterCriticalSection(&m_Lock);

	m_WiseLogQueue.push( std::string(szLogFile) );

	LeaveCriticalSection(&m_Lock);

	SetEvent(m_hThreadSignal);
	*/
	return true;
}

bool WiseLog::Terminate( DWORD nExitCode )
{
	bool bResult = false;

	if( m_hThreadHandle == INVALID_HANDLE_VALUE )
		return true;

	bResult	= ( ::TerminateThread( m_hThreadHandle, nExitCode ) == TRUE );
	if( bResult )
	{
		CloseHandle( m_hThreadHandle );
		m_hThreadHandle	= INVALID_HANDLE_VALUE;
	}

	return bResult;
}

DWORD WINAPI WiseLog::_Runner( LPVOID pParam )
{
	/*
	WiseLog* pInstance = static_cast<WiseLog*>( pParam );

	SetThreadName( pInstance->m_nThreadID, pInstance->m_szThreadNick);
	pInstance->Run();
	*/

	return 0;
}

void WiseLog::SetThreadName( DWORD dwThreadID, const char* szThreadName) 
{ 
	THREADNAME_INFO info; 
	info.dwType = 0x1000; 
	info.szName = szThreadName; 
	info.dwThreadID = dwThreadID; 
	info.dwFlags = 0; 

	__try 
	{ 
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), 
			(ULONG_PTR*)&info ); 
	} 
	__except (EXCEPTION_CONTINUE_EXECUTION) 
	{ 
	} 
} 

void WiseLog::Run()
{
	/*
	ResetEvent(m_hThreadSignal);
	while ( m_bRunning )
	{
		if ( WaitForSingleObject( m_hThreadSignal, 500) == WAIT_OBJECT_0 )
		{
			if ( m_bRunning == false )
				break;

			EnterCriticalSection(&m_Lock);
			while ( m_WiseLogQueue.size() )
			{
				EnterCriticalSection(&m_Lock);
				std::string szUrl = m_WiseLogQueue.front();
				m_WiseLogQueue.pop();
				LeaveCriticalSection(&m_Lock);

				SendWebPost(szUrl.c_str());
			}
			LeaveCriticalSection(&m_Lock);

			ResetEvent(m_hThreadSignal);
		}
	}
	*/
}

void WiseLog::SendWebPost(const char* szLogFile)
{
	/*
	OutputDebugStringA("SendWebPost\n");
	char szUrl[2048] = {0,};
	char szCookieValue[2048] = {0,};
	
	sprintf_s( szUrl, "http://dragonnest.nexon.com/WiseLog/%s", szLogFile );
	sprintf_s( szCookieValue, "OID=%d", m_uMyOid);
	
	InternetSetCookieA( "http://dragonnest.nexon.com", "NXCH", szCookieValue );
	HINTERNET hInternet = InternetOpenA("USER-AGENT-GAME-CLIENT", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if( hInternet == NULL )
		return;

	HINTERNET hURL = InternetOpenUrlA( hInternet, szUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hURL == NULL )
	{
		InternetCloseHandle( hInternet );
		return;
	}

	InternetCloseHandle( hURL );
	InternetCloseHandle( hInternet );
	*/
}

#endif	// #if defined(_KR)