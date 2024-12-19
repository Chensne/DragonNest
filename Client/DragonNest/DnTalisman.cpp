#include "stdafx.h"
#include "DnTalisman.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_TALISMAN_SYSTEM)
DECL_DN_SMART_PTR_STATIC( CDnTalisman, 2000 )

CDnTalisman::CDnTalisman()
: CDnUnknownRenderObject(false)
{
}

CDnTalisman::~CDnTalisman()
{
}

bool CDnTalisman::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTALISMANITEM );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( nTableID ) ) return false;

	m_ItemType = static_cast<eTalismanType>(pSox->GetFieldFromLablePtr(nTableID, "_Type")->GetInteger());
	m_nPeriod  = pSox->GetFieldFromLablePtr(nTableID, "_Period")->GetInteger();

	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );

	return true;
}

DnTalismanHandle CDnTalisman::CreateTalisman( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	if( nTableID < 1 ) return CDnTalisman::Identity();

	DnTalismanHandle hTalisman = (new CDnTalisman)->GetMySmartPtr();
	if( !hTalisman ) return CDnTalisman::Identity();

	hTalisman->Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );
	return hTalisman;	
}

#endif // PRE_ADD_TALISMAN_SYSTEM