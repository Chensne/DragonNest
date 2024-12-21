#include "StdAfx.h"
#include "DnJumpableChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

#ifndef _GAMESERVER
bool CDnJumpableChecker::s_bUseSignalSkillCheck = false;
bool CDnJumpableChecker::s_PrimUseSignalSkillCheck = false;
LOCAL_TIME CDnJumpableChecker::s_EndTime = 0;
#endif


CDnJumpableChecker::CDnJumpableChecker( DnActorHandle hActor ) : IDnSkillUsableChecker( hActor )
{
	m_iType = JUMPABLE_CHECKER;
}

CDnJumpableChecker::~CDnJumpableChecker(void)
{
}


#ifndef _GAMESERVER
void CDnJumpableChecker::SetUseSignalSkillCheck( bool bUseSignalSkillCheck, LOCAL_TIME SignalEndTime )
{
	s_bUseSignalSkillCheck = bUseSignalSkillCheck;	
	s_EndTime = SignalEndTime;
}
#endif


bool CDnJumpableChecker::CanUse( void )
{
	bool bResult = false;

	if( m_hHasActor )
	{
#ifndef _GAMESERVER
		if (s_PrimUseSignalSkillCheck)
			return true;

		if( s_bUseSignalSkillCheck )
			return true;
#else
		if( m_hHasActor && m_hHasActor->IsUseSignalSkillCheck() && m_hHasActor->IsAllowedSkillCheck(1) )
			return true;
#endif

		// 정환씨와 협의해서 Attack 상태일때는 안되도록 수정.
		if( !m_hHasActor->IsAttack() && m_hHasActor->IsAir() && !m_hHasActor->IsHit() && !m_hHasActor->IsDown() )
			bResult = true;
#if defined(_GAMESERVER)
		else
		{
			//
			if ((m_hHasActor->GetLocalTime() - m_hHasActor->GetStateChangeTime()) <= 200)
				bResult = true;
		}
#endif // _GAMESERVER
	}

	return bResult;
}

IDnSkillUsableChecker* CDnJumpableChecker::Clone()
{
	CDnJumpableChecker* pNewChecker = new CDnJumpableChecker(m_hHasActor);

	return pNewChecker;
}