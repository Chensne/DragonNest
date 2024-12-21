
#pragma once

#include "SecondarySkill.h"

#if defined( PRE_ADD_SECONDARY_SKILL )

//========================================================================
// 제작스킬
//========================================================================

class CSecondarySkillRecipe;

class CManufactureSkill:public CSecondarySkill
{
public:

	CManufactureSkill( const int iSkillID, SecondarySkill::ExecuteType::eType ExecuteType )
		:CSecondarySkill( iSkillID, ExecuteType ),m_dwManufactureTick(0)
	{
	}
	virtual ~CManufactureSkill();

	size_t					GetRecipeCount(){ return m_vRecipe.size(); }
	CSecondarySkillRecipe*	GetRecipe( int iItemID );
	int						AddRecipe( CSecondarySkillRecipe* pRecipe );
	bool					DeleteRecipe( CSecondarySkillRecipe* pRecipe );
	void					CopyList( std::vector<CSecondarySkillRecipe*>& vData );

	bool					bIsManufacturing(){ return (m_dwManufactureTick>0); }
	DWORD					GetManufactureTick(){ return m_dwManufactureTick; }
	void					ClearManufactureTick(){ m_dwManufactureTick=0; }
	void					SetManufactureTick( DWORD dwTick ){ m_dwManufactureTick = dwTick; }

	virtual SecondarySkill::Type::eType		GetType(){ return SecondarySkill::Type::ManufactureSkill; }
	virtual size_t							GetMaxRecipeCount() = 0;		// 최대 Recipe 수
	std::vector<CSecondarySkillRecipe*>&	GetList() { return m_vRecipe; }

protected:

	std::vector<CSecondarySkillRecipe*>	m_vRecipe;
	DWORD m_dwManufactureTick;
};

//========================================================================
// 제작스킬-요리
//========================================================================

class CCookingSkill:public CManufactureSkill,public TBoostMemoryPool<CCookingSkill>
{
public:

	CCookingSkill( const int iSkillID, SecondarySkill::ExecuteType::eType ExecuteType )
		:CManufactureSkill( iSkillID, ExecuteType )
	{
	}

	virtual SecondarySkill::SubType::eType	GetSubType(){ return SecondarySkill::SubType::CookingSkill; }
	virtual size_t							GetMaxRecipeCount(){ return SecondarySkill::Max::LearnCookingRecipe; }
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
