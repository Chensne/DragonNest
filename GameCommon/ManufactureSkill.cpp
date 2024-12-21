
#include "Stdafx.h"
#include "ManufactureSkill.h"
#include "SecondarySkillRecipe.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

CManufactureSkill::~CManufactureSkill()
{
	for( UINT i=0 ; i<m_vRecipe.size() ; ++i )
		delete m_vRecipe[i];
}

CSecondarySkillRecipe* CManufactureSkill::GetRecipe( int iItemID )
{
	for( UINT i=0 ; i<m_vRecipe.size() ; ++i )
	{
		if( m_vRecipe[i]->GetItemID() == iItemID )
			return m_vRecipe[i];
	}

	return NULL;
}

int CManufactureSkill::AddRecipe( CSecondarySkillRecipe* pRecipe )
{
	if( static_cast<size_t>(GetMaxRecipeCount()) <= m_vRecipe.size() )
	{
		_ASSERT( 0 );
		return ERROR_SECONDARYSKILL_ADDRECIPE_MAX;
	}
  
	// 중복검사
	if( GetRecipe( pRecipe->GetItemID() ) )
	{
		_ASSERT(0);
		return ERROR_SECONDARYSKILL_ADDRECIPE_DUPLICATE;
	}

	m_vRecipe.push_back( pRecipe );
	return ERROR_NONE;
}

bool CManufactureSkill::DeleteRecipe( CSecondarySkillRecipe* pRecipe )
{
	for( UINT i=0 ; i<m_vRecipe.size() ; ++i )
	{
		if( m_vRecipe[i] == pRecipe )
		{
			m_vRecipe.erase( m_vRecipe.begin()+i );
			return true;
		}
	}

	_ASSERT(0);
	return false;
}

void CManufactureSkill::CopyList( std::vector<CSecondarySkillRecipe*>& vData )
{
	if( m_vRecipe.empty() )
		return;

	vData.reserve( m_vRecipe.size() );
	vData = m_vRecipe;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
