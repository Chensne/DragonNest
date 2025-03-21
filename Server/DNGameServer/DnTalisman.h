#pragma once
#if defined(PRE_ADD_TALISMAN_SYSTEM)

#include "DnUnknownRenderObject.h"
#include "DnItem.h"

class CDnTalisman : public CDnUnknownRenderObject< CDnTalisman, MAX_SESSION_COUNT >, public CDnItem
{
public:
	CDnTalisman(CMultiRoom *pRoom);
	virtual ~CDnTalisman();

public:
	static DnTalismanHandle CreateTalisman( CMultiRoom *pRoom, int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false );

public:
	virtual bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false );	
};

#endif // PRE_ADD_TALISMAN_SYSTEM