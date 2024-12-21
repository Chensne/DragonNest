
#include "stdafx.h"
#include "SecondarySkill.h"
#include "SecondarySkillEventHandler.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

CSecondarySkill::CSecondarySkill( int iSkillID, SecondarySkill::ExecuteType::eType ExecuteType )
:m_iSkillID(iSkillID),m_ExecuteType(ExecuteType),m_Grade(SecondarySkill::Grade::Beginning),m_iLevel(1),m_iExp(0)
{
}

void CSecondarySkill::SetExp( const int iExp, ISecondarySkillEventHandler* pEventHandler )
{
	m_iExp = iExp;
	if( pEventHandler )
		pEventHandler->OnChangeExp( this, 0 );
}

void CSecondarySkill::AddExp( const int iExp, ISecondarySkillEventHandler* pEventHandler )
{
	m_iExp += iExp;
	if( pEventHandler )
		pEventHandler->OnChangeExp( this, iExp );
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
