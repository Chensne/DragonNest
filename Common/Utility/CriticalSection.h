#pragma once

class CSyncLock
{
private:
	CRITICAL_SECTION m_Lock;

public:
	CSyncLock()
	{
		BOOL bResult = InitializeCriticalSectionAndSpinCount(&m_Lock, 2000);
		ASSERT( bResult != FALSE );
	}

	~CSyncLock()
	{
		DeleteCriticalSection(&m_Lock);
	}

	/*LARGE_INTEGER liFrequency;
	LARGE_INTEGER liStartTime;
	LARGE_INTEGER liCurTime;*/

	inline void Lock()
	{
//#ifdef _RDEBUG
//		QueryPerformanceFrequency(&liFrequency);
//		QueryPerformanceCounter(&liStartTime);	
//#endif
		EnterCriticalSection(&m_Lock);
//#ifdef _RDEBUG
//		QueryPerformanceCounter(&liCurTime);
//		double dwTime = (double)1000.0 * ( liCurTime.QuadPart - liStartTime.QuadPart ) / ( liFrequency.QuadPart );
//		static double minTime = 20.0;
//		if( dwTime >= minTime ) {
//			char szProfileTestStr[256];
//			sprintf_s( szProfileTestStr, "Profiling Code Block / %s, %.2f ms \n", "Lock Delay", dwTime );
//			OutputDebugStringA( szProfileTestStr );
//		}
//#endif
	}

	inline void UnLock()
	{
		LeaveCriticalSection(&m_Lock);
		
		
	}

#if defined(_DEBUG)
	inline bool IsLock() const
	{
		// 현재 스레드가 이미 임계구역에 진입해 있는 상태인지 체크 (20091110 b4nfter)
		return((0 < m_Lock.RecursionCount) && (::GetCurrentThreadId() == HandleToUlong(m_Lock.OwningThread)));
	}
#endif // #if defined(_DEBUG)

};
  

//----------------------------------------------------------------------------

template < class _T >
class ScopeLock
{
public:
	ScopeLock( _T& LockObj, bool bEnable = true )
		: m_MonitorObj( &LockObj ), m_bEnable( (&LockObj) ? (bEnable):(false) )
	{
		if( m_bEnable ) {
			m_MonitorObj->Lock();
		}
	}
	ScopeLock( _T* LockObj, bool bEnable = true )	// 클라에서도 사용합니다. by realgaia
		: m_MonitorObj( LockObj ), m_bEnable( (LockObj)?(bEnable):(false) )
	{
		// 생성자 인터페이스 추가 (20091110 b4nfter)
		if( m_bEnable ) {
			m_MonitorObj->Lock();
		}
	}

	~ScopeLock()
	{
		if( m_bEnable ) {
			m_MonitorObj->UnLock();
		}
	}

private:
	_T*		m_MonitorObj;
	bool		m_bEnable;
};
