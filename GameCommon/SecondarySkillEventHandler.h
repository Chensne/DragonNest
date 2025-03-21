
#pragma once

#if defined( PRE_ADD_SECONDARY_SKILL )

class CSecondarySkill;
class CManufactureSkill;
class CSecondarySkillRecipe;

// Interface Class
class ISecondarySkillEventHandler
{
public:

	virtual ~ISecondarySkillEventHandler(){}

	virtual bool OnCreate( CSecondarySkill* pSkill )		= 0;
	virtual bool OnDelete( CSecondarySkill* pSkill )	= 0;
	virtual bool OnAddRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash ) = 0;
	virtual bool OnDeleteRecipe( CSecondarySkillRecipe* pRecipe ) = 0;
	virtual bool OnExtractRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash ) = 0;
	virtual bool OnChangeExp( CSecondarySkill* pSkill, int iAddExp ) = 0;
	virtual bool OnChangeRecipeExp( CSecondarySkillRecipe* pRecipe, int iAddExp ) = 0;
	virtual bool OnManufacture( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe, bool bIsStart ) = 0;
	virtual bool OnCancelManufacture( CManufactureSkill* pManufactureSkill, bool bSend ) = 0;
};

// Null-Interface Class
class CSecondarySkillEventHandlerNull:public ISecondarySkillEventHandler,TBoostMemoryPool<CSecondarySkillEventHandlerNull>
{
public:

	virtual bool OnCreate( CSecondarySkill* pSkill ){ return true; }
	virtual bool OnDelete( CSecondarySkill* pSkill ){ return true; }
	virtual bool OnAddRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash ){ return true; }
	virtual bool OnDeleteRecipe( CSecondarySkillRecipe* pRecipe ){ return true; }
	virtual bool OnExtractRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash ){ return true; }
	virtual bool OnChangeExp( CSecondarySkill* pSkill, int iAddExp ){ return true; }
	virtual bool OnChangeRecipeExp( CSecondarySkillRecipe* pRecipe, int iAddExp ){ return true; }
	virtual bool OnManufacture( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe, bool bIsStart ){ return true; }
	virtual bool OnCancelManufacture( CManufactureSkill* pManufactureSkill, bool bSend ){ return true; }
};
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
