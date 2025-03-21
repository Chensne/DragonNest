#include "StdAfx.h"
#include "DnPlayerFloatingChecker.h"
#include "DnWorld.h"
#include "DnPlayerActor.h"
#include "DNUserSession.h"

CDnPlayerFloatingChecker::CDnPlayerFloatingChecker( CDnPlayerActor* pActor )
: IDnPlayerChecker( pActor )
{
	m_nInvalidCount = 0;
	m_LastCheckTime = 0;
}

CDnPlayerFloatingChecker::~CDnPlayerFloatingChecker()
{
}

void CDnPlayerFloatingChecker::Process( LOCAL_TIME LocalTime, float fDelta )
{
	PROFILE_TIME_TEST_BLOCK_START( "CDnPlayerFloatingChecker" );
	if( m_LastCheckTime == 0 ) m_LastCheckTime = LocalTime;
	else if( m_LastCheckTime > LocalTime ) m_LastCheckTime = LocalTime;

	m_fVecHeightList.push_back( m_pActor->GetMatEx()->m_vPosition.y );
	if( m_fVecHeightList.size() > 20 ) m_fVecHeightList.erase( m_fVecHeightList.begin() );
	// 1초에 한번씩 체크해서
	if( LocalTime - m_LastCheckTime > 1000 ) 
	{
		m_LastCheckTime = LocalTime; // 서버가 느려지면 체크가 될 소지가 있으므려 m_nLastCheckTime -= 1000 안하고 그냥 현제 시간으로 넙니다.
		if( IsFloating() == true ) 
		{
			m_nInvalidCount++;
			// 3초동안 계속 Invalid 나올 시 체크해줍니다.
			if( m_nInvalidCount == 3 ) 
			{
				if( m_pActor && m_pActor->GetUserSession() )
					g_Log.Log( LogType::_HACK, m_pActor->GetUserSession(), L"[CDnPlayerFloatingChecker] 치트 의심 유저!!! CharName=%s", m_pActor->GetUserSession()->GetCharacterName() );

				m_nInvalidCount = 0;
			}
		}
		else m_nInvalidCount = 0;
	}
	PROFILE_TIME_TEST_BLOCK_END();
}

bool CDnPlayerFloatingChecker::IsFloating()
{
	// 2미터 이상 오차가 날 경우
	if( m_fVecHeightList.size() < 20 ) return false;

	float fAverage = 0.f;
	for( DWORD i=0; i<m_fVecHeightList.size(); i++ ) {
		fAverage += m_fVecHeightList[i];
	}
	fAverage /= (float)m_fVecHeightList.size();

	static float fLimitHeight = 200.f;
	if( m_pActor->GetVelocity()->y == 0.f && abs( m_pActor->GetPosition()->y - fAverage ) > fLimitHeight ) {
		return true;
	}
	return false;
}