#include "StdAfx.h"
#include "DnPlayerSkillChecker.h"
#include "DnDropItem.h"
#include "DnPlayerActor.h"
#include "DNUserSession.h"

CDnPlayerSkillChecker::CDnPlayerSkillChecker( CDnPlayerActor *pPlayer )
: IDnPlayerChecker( pPlayer )
{
	m_nInvalidCount = 0;
	m_LastCheckTime = 0;
}

CDnPlayerSkillChecker::~CDnPlayerSkillChecker()
{
}

bool CDnPlayerSkillChecker::IsInvalidPlayer()
{
	return ( m_nInvalidCount >= 3 ) ? true : false;
}

void CDnPlayerSkillChecker::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nInvalidCount <= 0 ) return;
	if( timeGetTime() - m_LastCheckTime > 10000 ) {
		m_nInvalidCount = 0;
	}
}

void CDnPlayerSkillChecker::OnInvalidUseSkill( int nSkillID, CDnSkill::UsingResult Result )
{
	int nInvalidValue = 0;
	switch( Result ) {
		case CDnSkill::UsingResult::FailedByCooltime: 
			nInvalidValue = 1; 
			break;
		case CDnSkill::UsingResult::NoExistSkill:
			nInvalidValue = 2;
			break;
		case CDnSkill::UsingResult::Hack:
			nInvalidValue = 5;
			break;
		case CDnSkill::UsingResult::FailedByUsableChecker: 
			break;
	}
	m_nInvalidCount += nInvalidValue;
	m_LastCheckTime = timeGetTime();

	if( m_pActor && m_pActor->GetUserSession() ) {
		if( m_nInvalidCount >= 10 ) 
		{
			g_Log.Log( LogType::_HACK, m_pActor->GetUserSession(), L"HackChecker(Skill) : CharName=%s InvalidCount=%d, SkillID=%d, Result=%d\n", m_pActor->GetUserSession()->GetCharacterName(), m_nInvalidCount, nSkillID, Result );
//			m_pActor->OnInvalidPlayerChecker( 1 );
		}
	}
}
