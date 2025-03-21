#include "StdAfx.h"
#include "DnHPChecker.h"
#include "DnActor.h"

#ifndef USE_BOOST_MEMPOOL
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif


CDnHPChecker::CDnHPChecker( DnActorHandle hActor, float fStandardRatio, int iOperator ) : IDnSkillUsableChecker( hActor ), 
																						  m_fStandardRatio( fStandardRatio ),
																						  m_iOperator( iOperator )
{
	_ASSERT( 0 <= m_iOperator && m_iOperator < OP_COUNT && "잘못된 연산자 대입" );
	m_iType = HP_CHECKER;
}

CDnHPChecker::~CDnHPChecker(void)
{
}


bool CDnHPChecker::CanUse( void )
{
	bool bResult = false;

	if( m_hHasActor )
	{
		float fCurHPPercent = ( (float)m_hHasActor->GetHP() / (float)m_hHasActor->GetMaxHP() ) * 100.0f;
		
		switch( m_iOperator )
		{
			case OP_GREATER:
				bResult = (m_fStandardRatio < fCurHPPercent);
				break;

			// float 이므로 약간 오차를 줘야 하나. 0.5 퍼센트 정도는 관용도를 줘볼까.
			case OP_EQUAL:
				bResult = (float)fabs(m_fStandardRatio - fCurHPPercent) < 0.5f;
				break;

			case OP_LESS:
				bResult = (m_fStandardRatio > fCurHPPercent);
				break;

			case OP_GREATER_EQUAL:
				bResult = (m_fStandardRatio <= fCurHPPercent );
				break;

			case OP_LESS_EQUAL:
				bResult = (m_fStandardRatio >= fCurHPPercent);
				break;
		}
	}
	return bResult;
}

IDnSkillUsableChecker* CDnHPChecker::Clone()
{
	CDnHPChecker* pNewChecker = new CDnHPChecker(m_hHasActor, m_fStandardRatio, m_iOperator);

	return pNewChecker;
}