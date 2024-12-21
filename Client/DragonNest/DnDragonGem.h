#pragma once
#include "DnUnknownRenderObject.h"
#include "DnItem.h"

#if defined(PRE_ADD_DRAGON_GEM)
class CDnDragonGem : public CDnUnknownRenderObject< CDnDragonGem >, public CDnItem
{
public:
	CDnDragonGem();
	virtual ~CDnDragonGem();

	enum eDragonGemType {
		GEM_DEFENSE = 0,
		GEM_ATTACK,
		GEM_SKILL,
		GEM_NEW //not used?
	};

protected:

public:
	virtual	bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );
	static	DnDragonGemHandle CreateDragonGem( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );
};
#endif // PRE_ADD_DRAGON_GEM