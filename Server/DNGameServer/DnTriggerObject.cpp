#include "StdAfx.h"
#include "DnTriggerObject.h"
#include "DnTriggerElement.h"
#if defined( PRE_TRIGGER_UNITTEST_LOG )
#include "DNGameRoom.h"
#include "DnWorld.h"
#include "EtWorldSector.h"
#include <../NetworkLib/Log.h>
extern CLog g_TriggerUnitTestLog;
#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )
#if defined( PRE_TRIGGER_LOG )
#include "EtWorldSector.h"
#include "EtWorldGrid.h"
#include "DnWorld.h"
#include "DNGameRoom.h"
#include "DNRUDPGameServer.h"
#endif // #if defined( PRE_TRIGGER_LOG )

CDnTriggerObject::CDnTriggerObject( CEtTrigger *pTrigger )
: CEtTriggerObject( pTrigger )
{
#if defined( PRE_TRIGGER_LOG )
	m_pGameRoom = NULL;
	m_bLog		= false;

	if( pTrigger && pTrigger->GetSector() && pTrigger->GetSector()->GetParentGrid() && pTrigger->GetSector()->GetParentGrid()->GetWorld() )
	{
		m_pGameRoom = static_cast<CDNGameRoom*>(((CDnWorld*)pTrigger->GetSector()->GetParentGrid()->GetWorld())->GetCurrentTask());
		if( m_pGameRoom )
		{
#if !defined( _FINAL_BUILD )
			// 모든 서버만 로그 남겨본다.
			m_bLog = true;
#else // #if !defined( _FINAL_BUILD )
			// 1번 서버만 로그 남겨본다.
			m_bLog = ( m_pGameRoom->GetGameServer() && m_pGameRoom->GetGameServer()->GetServerID() == 1 ) ? true : false;
#endif // #if !defined( _FINAL_BUILD )
			if( m_bLog )
			{
				QueryPerformanceFrequency(&m_liFrequency);
			}
		}
	}
#endif // #if defined( PRE_TRIGGER_LOG )
}

CDnTriggerObject::~CDnTriggerObject()
{
#if defined( PRE_TRIGGER_UNITTEST_LOG )

	if( m_dTime > 0.f )
	{
		char szLog[1024];
		int iMapIndex = 0;
		if( GetTrigger() && GetTrigger()->GetSector() && GetTrigger()->GetSector()->GetParentGrid() && GetTrigger()->GetSector()->GetParentGrid()->GetWorld() )
		{
			CDNGameRoom* pGameRoom = (((CDnWorld*)GetTrigger()->GetSector()->GetParentGrid()->GetWorld())->GetCurrentTask());
			if( pGameRoom )
				iMapIndex = pGameRoom->m_iMapIdx;
		}
		
		sprintf( szLog, "[%.3f ms] MapIndex=%d Categoty:%s TriggerName:%s\r\n", m_dTime, iMapIndex, GetCategory(), GetTriggerName() );
		g_TriggerUnitTestLog.LogA( szLog );
	}

#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )
}

CEtTriggerElement *CDnTriggerObject::AllocTriggerElement()
{
	return new CDnTriggerElement( this );
}

#if defined( PRE_TRIGGER_LOG )

bool CDnTriggerObject::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_bLog )
	{
		return CEtTriggerObject::Process( LocalTime, fDelta );
	}

	QueryPerformanceCounter(&m_liStartTime);	
	bool bRet = CEtTriggerObject::Process( LocalTime, fDelta );
	QueryPerformanceCounter(&m_liCurTime);
	double dElapsed = (1000 * ( m_liCurTime.QuadPart - m_liStartTime.QuadPart ) / static_cast<double>( m_liFrequency.QuadPart ));

	m_pGameRoom->m_dTriggerTime += dElapsed;

	return bRet;
}

bool CDnTriggerObject::ProcessQueue( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_bLog )
	{
		return CEtTriggerObject::ProcessQueue( LocalTime, fDelta );
	}

	QueryPerformanceCounter(&m_liStartTime);	
	bool bRet = CEtTriggerObject::ProcessQueue( LocalTime, fDelta );
	QueryPerformanceCounter(&m_liCurTime);
	double dElapsed = (1000 * ( m_liCurTime.QuadPart - m_liStartTime.QuadPart ) / static_cast<double>( m_liFrequency.QuadPart ));

	m_pGameRoom->m_dTriggerTime += dElapsed;

	return bRet;
}

#endif // #if defined( PRE_TRIGGER_LOG )
