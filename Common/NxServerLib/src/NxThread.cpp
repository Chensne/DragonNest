#include "stdafx.h"
#include "NxThread.h"


NxThread::NxThread(const char* szThreadNick)
: m_nThreadID( 0 ), m_hThreadHandle( INVALID_HANDLE_VALUE )
{
	strcpy_s(m_szThreadNick, szThreadNick);
}

NxThread::~NxThread()
{
	m_bRunning = false;
}

bool
NxThread::Start()
{
	if( m_hThreadHandle != INVALID_HANDLE_VALUE )
		return false;

	m_hThreadHandle = ::CreateThread( 0, 0, _Runner, ( LPVOID )this, 0, &m_nThreadID );
	if( m_hThreadHandle == 0 )
		return false;

	m_bRunning = true;

	return true;
}

bool 
NxThread::Terminate( DWORD nExitCode )
{
	bool	bResult = false;

	if( m_hThreadHandle == INVALID_HANDLE_VALUE )
		return true;

	bResult	= ( ::TerminateThread( m_hThreadHandle, nExitCode ) == TRUE );
	m_hThreadHandle	= INVALID_HANDLE_VALUE;

	return bResult;
}

bool 
NxThread::WaitForTerminate( DWORD nTimeout )
{
	if( m_hThreadHandle == INVALID_HANDLE_VALUE )
		return false;

	return ( ::WaitForSingleObject( m_hThreadHandle, nTimeout ) == WAIT_OBJECT_0 );
}

DWORD WINAPI 
NxThread::_Runner( LPVOID pParam )
{
	NxThread* pInstance = static_cast<NxThread*>( pParam );

	SetThreadName( pInstance->m_nThreadID, pInstance->m_szThreadNick);
	pInstance->Run();

	return 0;
}


void 
NxThread::SetThreadName( DWORD dwThreadID, const char* szThreadName) 
{ 
	THREADNAME_INFO info; 
	info.dwType = 0x1000; 
	info.szName = szThreadName; 
	info.dwThreadID = dwThreadID; 
	info.dwFlags = 0; 

	__try 
	{ 
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), 
			(DWORD*)&info ); 
	} 
	__except (EXCEPTION_CONTINUE_EXECUTION) 
	{ 
	} 
} 