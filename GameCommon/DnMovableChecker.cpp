#include "StdAfx.h"
#include "DnMovableChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

#ifndef _GAMESERVER
bool CDnMovableChecker::s_bUseSignalSkillCheck = false;
bool CDnMovableChecker::s_PrimUseSignalSkillCheck = false;
LOCAL_TIME CDnMovableChecker::s_EndTime = 0;
#endif

CDnMovableChecker::CDnMovableChecker( DnActorHandle hActor ) : IDnSkillUsableChecker( hActor )
{
	m_iType = MOVABLE_CHECKER;
}

CDnMovableChecker::~CDnMovableChecker(void)
{
}


#ifndef _GAMESERVER
void CDnMovableChecker::SetUseSignalSkillCheck( bool bUseSignalSkillCheck, LOCAL_TIME SignalEndTime )
{
	s_bUseSignalSkillCheck = bUseSignalSkillCheck;
	s_EndTime = SignalEndTime;
}
#endif


bool CDnMovableChecker::CanUse( void )
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
		if( m_hHasActor && m_hHasActor->IsUseSignalSkillCheck() && m_hHasActor->IsAllowedSkillCheck(0) )
			return true;
#endif

		if( !m_hHasActor->IsProcessSkill() && (m_hHasActor->GetStateEffect() & CDnActorState::Cant_Move) == CDnActorState::Cant_Move )
		{
			bResult = true;
		}
		else if( m_hHasActor->IsMovable() )
		{
			bResult = true;
		}
	}	

	return bResult;
}

IDnSkillUsableChecker* CDnMovableChecker::Clone()
{
	CDnMovableChecker* pNewChecker = new CDnMovableChecker(m_hHasActor);

	return pNewChecker;
}