
#pragma once

#if defined( PRE_ADD_SECONDARY_SKILL )

#include "SecondarySkillEventHandler.h"

class CSecondarySkillEventHandlerServer:public ISecondarySkillEventHandler,public TBoostMemoryPool<CSecondarySkillEventHandlerServer>
{
public:

	CSecondarySkillEventHandlerServer( CDNUserSession* pSession );
	virtual ~CSecondarySkillEventHandlerServer(){}
	
	virtual bool OnCreate( CSecondarySkill* pSkill );
	virtual bool OnDelete( CSecondarySkill* pSkill );
	virtual bool OnAddRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash );
	virtual bool OnDeleteRecipe( CSecondarySkillRecipe* pRecipe );
	virtual bool OnExtractRecipe( CSecondarySkillRecipe* pRecipe, bool bIsCash );
	virtual bool OnChangeExp( CSecondarySkill* pSkill, int iAddExp );
	virtual bool OnChangeRecipeExp( CSecondarySkillRecipe* pRecipe, int iAddExp );
	virtual bool OnManufacture( CSecondarySkill* pSkill, CSecondarySkillRecipe* pRecipe, bool bIsStart );
	virtual bool OnCancelManufacture( CManufactureSkill* pManufactureSkill, bool bSend );

protected:

	CDNUserSession* m_pSession;
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
