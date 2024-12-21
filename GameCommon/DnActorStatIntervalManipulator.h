#pragma once

#include "DnBlow.h"




// ����� ���Ӽ����� Ŭ���̾�Ʈ���� #define �ɾ� ���� ���� ���� ������ ���߿� ��� ���� �𸣰ڴ�.
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
	// �ݵ�� �Ʒ��� �����ִ� �����ڷ� ������ �ּ���.
	CDnActorStatIntervalManipulator( void ) {};

	//void _ProcessInterval( int& iProcessManipulateCount, int iDestProcessCount );

public:
	CDnActorStatIntervalManipulator( DnActorHandle hActor, DnBlowHandle hParentBlow );
	~CDnActorStatIntervalManipulator(void);

	void OnBegin( LOCAL_TIME LocalTime, DWORD dwInterval );
	void Process( LOCAL_TIME LocalTime, float fDelta );
	void OnEnd( LOCAL_TIME LocalTime, float fDelta );
};
