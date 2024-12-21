#include "StdAfx.h"
#include "DnPlayerActionChecker.h"
#include "DnPlayerActor.h"
#include "DNUserSession.h"
#include "DnGameTask.h"

CDnPlayerActionChecker::CDnPlayerActionChecker( CDnPlayerActor *pPlayer )
: IDnPlayerChecker( pPlayer )
{
	m_nInvalidCount = 0;
	m_LastCheckTime = 0;
}

CDnPlayerActionChecker::~CDnPlayerActionChecker()
{
}

bool CDnPlayerActionChecker::IsInvalidPlayer()
{
	return ( m_nInvalidCount >= 10 ) ? true : false;
}

void CDnPlayerActionChecker::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nInvalidCount <= 0 ) return;
	if( timeGetTime() - m_LastCheckTime > 5000 ) 
	{
		m_nInvalidCount = 0;
	}
}

void CDnPlayerActionChecker::OnInvalidAction()
{
	m_nInvalidCount++;
	m_LastCheckTime = timeGetTime();

	if( m_pActor && m_pActor->GetUserSession() ) 
	{
		if( m_nInvalidCount >= 10 )
		{
			g_Log.Log(LogType::_HACK, m_pActor->GetUserSession(), 
				L"HackChecker(Action(Invalid Action)) : CharName=%s, InvalidCount=%d, Action=%S, State=%d ", 
				m_pActor->GetUserSession()->GetCharacterName(), 
				m_nInvalidCount, 
				m_pActor->GetCurrentAction(), 
				m_pActor->GetState() );
		}
	}
}
