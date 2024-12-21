#pragma once

#include "DnItem.h"

#ifdef PRE_ADD_SECONDARY_SKILL

class CDnSecondarySkillRecipeItem : public CDnItem
{
public:
	CDnSecondarySkillRecipeItem();
	virtual ~CDnSecondarySkillRecipeItem();

public:
	int GetDurability() const { return m_nDurability; }
	void SetDurability( int nValue ) { m_nDurability = nValue; }
	virtual void GetTItemInfo( TItemInfo &Info );

protected:
	int m_nDurability;
};

#endif // PRE_ADD_SECONDARY_SKILL