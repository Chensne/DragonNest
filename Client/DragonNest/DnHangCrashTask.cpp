#include "StdAfx.h"
#include "DnHangCrashTask.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnHangCrashTask::CDnHangCrashTask( DWORD dwDetectTime )
{
	m_dwDetectTime = dwDetectTime;
}

CDnHangCrashTask::~CDnHangCrashTask()
{
}

bool CDnHangCrashTask::Initialize()
{
	return true;
}

void CDnHangCrashTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnHangMonitorObject *pObject;
	bool bHangUp = false;
	for( DWORD i=0; i<m_pVecMonitorList.size(); i++ ) {
		pObject = m_pVecMonitorList[i];
		if( pObject->IsPauseHang() ) continue;
		if( pObject->GetLastUpdateTime() == 0 ) continue;
		int nInterval = (int)( timeGetTime() - pObject->GetLastUpdateTime() );
		if( nInterval <= 0 ) continue;
		if( nInterval >= (int)m_dwDetectTime ) {
			bHangUp = true;
			break;
		}
	}
	if( bHangUp ) {
		/* // 현제 프리징 되는거 잡아야 하기때문에 주석처리합니다.
#ifndef _FINAL_BUILD
		if( CGlobalValue::GetInstance().m_bDebugging ) {		// 디버깅 오래 잡고 있을땐 어서트로 대신한다.
			ASSERT( false );
			return;
		}
#endif
		*/
		BYTE *pForceCrash = NULL;
		*pForceCrash = 0xFF;
	}
	Sleep(1000);
}

void CDnHangCrashTask::InsertMonitorObject( CDnHangMonitorObject *pObject )
{
	for( DWORD i=0; i<m_pVecMonitorList.size(); i++ ) {
		if( m_pVecMonitorList[i] == pObject ) return;
	}

	m_pVecMonitorList.push_back( pObject );
}

void CDnHangCrashTask::RemoveMonitorObject( CDnHangMonitorObject *pObject )
{
	for( DWORD i=0; i<m_pVecMonitorList.size(); i++ ) {
		if( m_pVecMonitorList[i] == pObject ) {
			m_pVecMonitorList.erase( m_pVecMonitorList.begin() + i );
			i--;
		}
	}
}


void CDnHangMonitorObject::UpdateHang() 
{
	m_dwLastUpdateTime = timeGetTime();
}
