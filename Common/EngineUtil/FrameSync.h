#pragma once

#include "Timer.h"

class CFrameSync:public TBoostMemoryPool<CFrameSync>
{
public:
	CFrameSync();
	~CFrameSync();

	void Begin( int nSecureFrame );
	void End();
	void Pause();
	void Resume();
	void Reset();

	void UpdateTime( int nDebugSecureCount = 0 );
	bool CheckSync();
	void CheckSecureFrameLoad( int nIgnoreFrame );

	LOCAL_TIME GetMSTime();

	float GetFps();
	void SetTimeSpeed( float fSpeed, DWORD dwDelay = 0 );

protected:
	LOCAL_TIME m_SystemTime;
	LOCAL_TIME m_NextSyncTime;
	LOCAL_TIME m_PrevUpdateTime;
	/*
	LARGE_INTEGER m_qpFreq;
	LARGE_INTEGER m_qpBaseTime;
	LARGE_INTEGER m_qpTime;
	LARGE_INTEGER m_qpPrevTime;
	*/

	int m_nSecureFrame;
	LOCAL_TIME m_SecureCount;
	LOCAL_TIME m_PrevSecureCount;
	bool m_bActivate;
	bool m_bPause;

	float m_fFPS;

	float m_fTimeSpeed;
	LOCAL_TIME m_LastModifySpeedTime;
	LOCAL_TIME m_RevisionModifyTime;
	DWORD m_dwModifySpeedDelay;
	DWORD m_dwPovitTime;
};


class CFrameSkip
{
public:
	CFrameSkip()	{ Clear(); }
	virtual ~CFrameSkip() { }

	//! 초기화 하는 함수.. 
	void		Clear()  
	{
		SetFramePerSec( 60.0f );  
		m_fTimer = 0.0f;	
	}

	//! 프레임율을 설정한다.
	void		SetFramePerSec( float fFps )
	{
		m_fSecPerFrame = 1.0f/fFps;
		m_fTimer = 0.0f;
	}

	/*! Update(float dt) 함수가 루프안에서 호출될때,
	이 함수는 설정된 고정 프레임율 만큼만 true를 리턴한다. */
	bool		Update( float fDt )
	{
		m_fTimer += fDt;		
		if( m_fTimer < 0 )
			return false;		

		// 한프레임에 해당하는 시간을 뺀다.
		m_fTimer -= m_fSecPerFrame;
		return true;
	}

	//!	경우에 따라 스킵 되야 된다면 false 를 리턴한다.	
	bool		IsFrameSkip() { return (m_fTimer >= 0); }

protected:
	float		m_fTimer; 
	float		m_fSecPerFrame;
};