
#pragma once

#if defined( PRE_ADD_SECONDARY_SKILL )

class CSecondarySkill;
class CSecondarySkillRecipe;

class CSecondarySkillFactory
{
public:
	static CSecondarySkill*	CreateSkill( const int iSecondarySkillID );
	static CSecondarySkillRecipe* CreateRecipe( int iSkillID, int iItemID, INT64 biItemSerial, USHORT nDurability );
};

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )
