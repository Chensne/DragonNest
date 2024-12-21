#include "StdAfx.h"
#include "FrameSync.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CFrameSync::CFrameSync()
{
	Reset();
	m_nSecureFrame = 0;
	m_bActivate = false;
	m_bPause = false;
}

CFrameSync::~CFrameSync()
{
}

void CFrameSync::Begin( int nSecureFrame )
{
	m_nSecureFrame = nSecureFrame;
	m_bActivate = true;
}

void CFrameSync::End()
{
	m_bActivate = false;
}

void CFrameSync::Reset()
{
	/*
	QueryPerformanceFrequency( &m_qpFreq );
	QueryPerformanceCounter( &m_qpBaseTime );
	*/

	m_dwPovitTime = timeGetTime();
	m_SystemTime = 0;
	m_SecureCount = 0;
	m_PrevSecureCount = 0;
	m_PrevUpdateTime = 0;
	m_NextSyncTime = 0;
	m_fFPS = 0.f;

	m_LastModifySpeedTime = 0;
	m_fTimeSpeed = 1.f;
	m_RevisionModifyTime = 0;
	m_dwModifySpeedDelay = 0;
}

void CFrameSync::Pause()
{
	m_bPause = true;
}

void CFrameSync::Resume()
{
	m_bPause = false;
}

#ifndef _FINAL_BUILD
bool g_bFrameTest = false;
bool g_bPauseMode = false;
#endif
void CFrameSync::UpdateTime( int nDebugSecureCount )
{
	if( m_bActivate == false || m_bPause == true ) return;

//	LARGE_INTEGER qpTime;
//	QueryPerformanceCounter( &qpTime );

#ifndef _FINAL_BUILD
	if( !g_bFrameTest )
	{
		if( !g_bPauseMode )
		{
			if( nDebugSecureCount ) m_SystemTime += 1600;
//			else m_SystemTime = qpTime.QuadPart - m_qpBaseTime.QuadPart;
			else m_SystemTime = timeGetTime() - m_dwPovitTime;
		}
	}
#else
	if( nDebugSecureCount ) m_SystemTime += 1600;
//	else m_SystemTime = qpTime.QuadPart - m_qpBaseTime.QuadPart;
	else m_SystemTime = timeGetTime() - m_dwPovitTime;
#endif

// 고정프레임 돌릴때 쓸거당..
//	m_SystemTime += 10000000;
//	m_SystemTime += 142413156;
//	Sleep( 10 );
#ifndef _FINAL_BUILD
	if( g_bFrameTest )
	{
		m_SystemTime += 30000;
	}
#endif

	// FPS 구하기 위한 코드!!
	DWORD dwDiff = (DWORD)( GetMSTime() - m_PrevUpdateTime );
	if( dwDiff > 1000 ) {
		m_fFPS = ( m_SecureCount - m_PrevSecureCount ) / 1000.f * (float)dwDiff;
		m_PrevUpdateTime = GetMSTime();
		m_PrevSecureCount = m_SecureCount;
	}
}

float CFrameSync::GetFps()
{
	return m_fFPS;
}

void CFrameSync::CheckSecureFrameLoad( int nIgnoreFrame )
{
//	LOCAL_TIME TempTime = ( m_SystemTime * 1000 ) / m_qpFreq.QuadPart;
	LOCAL_TIME TempTime = timeGetTime() - m_dwPovitTime;

	if( TempTime - m_NextSyncTime > nIgnoreFrame ) {
		m_SecureCount = (LOCAL_TIME)( m_nSecureFrame * ( TempTime / 1000.f ) );
	}
}

bool CFrameSync::CheckSync()
{
	if( m_bActivate == false || m_bPause == true ) return false;

//	LOCAL_TIME TempTime = ( m_SystemTime * 1000 ) / m_qpFreq.QuadPart;
	LOCAL_TIME TempTime = timeGetTime() - m_dwPovitTime;

	if( m_dwModifySpeedDelay != 0 ) {
		if( TempTime - m_LastModifySpeedTime  > m_dwModifySpeedDelay ) {
			SetTimeSpeed( 1.f, 0 );
		}

	}
	// 고정 프레임일때
	if( m_nSecureFrame > 0 ) {
		if( TempTime >= m_NextSyncTime ) {
			++m_SecureCount;
			m_NextSyncTime = (LOCAL_TIME)( ( m_SecureCount * 1000.f ) / (double)m_nSecureFrame );
			return true;
		}
	}
	// 한번만 실행되게.. ( 내부 모듈 상 쓰레드를 돌린다던지 할때 )
	else if( m_nSecureFrame == 0 ) {
		m_bActivate = false;
		return true;
	}
	// 시간남으면 무조건 ( 렌더러쪽 )
	else if( m_nSecureFrame < 0 ) {
		++m_SecureCount;
		return true;
		/*
		LOCAL_TIME TempTime = ( m_SystemTime * 1000 ) / m_qpFreq.QuadPart;
		if( TempTime >= m_NextSyncTime ) {
			++m_SecureCount;
			m_NextSyncTime = (LOCAL_TIME)( 1000.f / (double)m_nSecureFrame );
			return true;
		}
		*/
	}

	return false;
}


void CFrameSync::SetTimeSpeed( float fSpeed, DWORD dwDelay )
{
//	LOCAL_TIME LocalTime = ( m_SystemTime * 1000 ) / m_qpFreq.QuadPart;
	LOCAL_TIME LocalTime = timeGetTime() - m_dwPovitTime;
	if( m_fTimeSpeed != 1.f ) {
		LOCAL_TIME TimeTemp = LocalTime - m_LastModifySpeedTime;
		m_RevisionModifyTime += (LOCAL_TIME)( TimeTemp * ( m_fTimeSpeed - 1.f ) );
	}
	m_dwModifySpeedDelay = dwDelay;
	m_fTimeSpeed = fSpeed;
	m_LastModifySpeedTime = LocalTime;
}

LOCAL_TIME CFrameSync::GetMSTime() 
{ 
//	LOCAL_TIME LocalTime = ( m_SystemTime * 1000 ) / m_qpFreq.QuadPart;
	LOCAL_TIME LocalTime = timeGetTime() - m_dwPovitTime;
	LOCAL_TIME RevTime = 0;

	if( m_fTimeSpeed != 1.f ) {
		RevTime = m_LastModifySpeedTime + (LOCAL_TIME)( ( LocalTime - m_LastModifySpeedTime ) * m_fTimeSpeed );
		LocalTime = 0;
	}
	return LocalTime + RevTime + m_RevisionModifyTime;
}