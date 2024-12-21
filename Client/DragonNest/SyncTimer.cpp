#include "StdAfx.h"
#include "SyncTimer.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CSyncTimer::CSyncTimer()
{
	m_bStarted = false;
	m_tCurTime = 0;
	m_dwBasisTime = 0;
}

CSyncTimer::~CSyncTimer()
{
}

void CSyncTimer::SetServerTime( __time64_t _tStartTime )
{
	m_tCurTime = _tStartTime;
	m_dwBasisTime = timeGetTime();
	m_bStarted = true;
}

void CSyncTimer::Process()
{
	if( !m_bStarted ) return;

	DWORD dwDelta = timeGetTime() - m_dwBasisTime;
	if( dwDelta >= 1000 ) {

		// 혹시 프레임 튀었을때를 대비해서,
		DWORD dwSec = dwDelta / 1000;

		// 최소단위는 초
		m_tCurTime += dwSec;

		// 기준타임 갱신
		m_dwBasisTime += (dwSec * 1000);
	}
}

__time64_t CSyncTimer::GetCurTime()
{
	if( !m_bStarted ) {
		_ASSERT(0&&"SyncTimer를 초기화하지 않은채 사용하려 합니다.");
		return 0;
	}
	return m_tCurTime;
}