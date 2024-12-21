#include "StdAfx.h"
#include "DnProbabilityChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

CDnProbabilityChecker::CDnProbabilityChecker( DnActorHandle hActor, float fProbability ) : IDnSkillUsableChecker( hActor ),
																						   m_fProbability( fProbability )
{
	_ASSERT( 0.0f <= m_fProbability && m_fProbability <= 1.0f );
	m_fProbability = fProbability * 10000.0f;
	m_iType = PROB_CHECKER;
}

CDnProbabilityChecker::~CDnProbabilityChecker(void)
{

}


bool CDnProbabilityChecker::CanUse( void )
{
	bool bResult = false;

	// 서버에서만 확률을 체크한다.
#ifdef _GAMESERVER
	bResult = (_rand(m_hHasActor->GetRoom()) % 10000 <= (int)m_fProbability);
#else
	bResult = true;
#endif

	return bResult;
}

IDnSkillUsableChecker* CDnProbabilityChecker::Clone()
{
	CDnProbabilityChecker* pNewChecker = new CDnProbabilityChecker(m_hHasActor, m_fProbability);

	return pNewChecker;
}