#include "stdafx.h"
#include "Thread.h"


Thread::Thread()
: m_nThreadID( 0 ), m_hHandleThread( INVALID_HANDLE_VALUE )
{
}

Thread::~Thread()
{
	m_bThreadLoop = false;
}

bool
Thread::Start()
{
	if( m_hHandleThread != INVALID_HANDLE_VALUE )
		return false;

	m_hHandleThread = ::CreateThread( 0, 0, _Runner, ( LPVOID )this, 0, &m_nThreadID );
	if( m_hHandleThread == 0 )
		return false;

	m_bThreadLoop = true;

	return true;
}

bool 
Thread::Terminate( DWORD nExitCode )
{
	bool	ret;

	if( m_hHandleThread == INVALID_HANDLE_VALUE )
		return true;

	ret				= ( ::TerminateThread( m_hHandleThread, nExitCode ) == TRUE );
	m_hHandleThread	= INVALID_HANDLE_VALUE;

	return ret;
}

bool 
Thread::WaitForTerminate( DWORD nTimeout )
{
	if( m_hHandleThread == INVALID_HANDLE_VALUE )
		return false;

	return ( ::WaitForSingleObject( m_hHandleThread, nTimeout ) == WAIT_OBJECT_0 );
}

DWORD WINAPI 
Thread::_Runner( LPVOID pParam )
{
	Thread* pInstance = static_cast<Thread*>( pParam );

	pInstance->Run();

	return 0;
}