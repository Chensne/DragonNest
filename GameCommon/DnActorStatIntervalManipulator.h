#pragma once

#include "DnBlow.h"




// 현재는 게임서버와 클라이언트에서 #define 걸어 놓고 같이 쓰고 있지만 나중엔 어떻게 될지 모르겠다.
class CDnActorStatIntervalManipulator
{
private:
	DnBlowHandle			m_hParentBlow;
	DnActorHandle			m_hActor;

	LOCAL_TIME				m_LocalStartTime;
	LOCAL_TIME				m_LocalElapsedTime;

	DWORD					m_dwInterval;
	LOCAL_TIME				m_LastIntervalTime;

private:
	// 반드시 아래쪽 인자주는 생성자로 생성해 주세요.
	CDnActorStatIntervalManipulator( void ) {};

	//void _ProcessInterval( int& iProcessManipulateCount, int iDestProcessCount );

public:
	CDnActorStatIntervalManipulator( DnActorHandle hActor, DnBlowHandle hParentBlow );
	~CDnActorStatIntervalManipulator(void);

	void OnBegin( LOCAL_TIME LocalTime, DWORD dwInterval );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
};
