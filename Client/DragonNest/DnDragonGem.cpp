#include "stdafx.h"
#include "DnDragonGem.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(PRE_ADD_DRAGON_GEM)
DECL_DN_SMART_PTR_STATIC( CDnDragonGem, 2000 )

CDnDragonGem::CDnDragonGem()
: CDnUnknownRenderObject(false)
{
}

CDnDragonGem::~CDnDragonGem()
{
}

DnDragonGemHandle CDnDragonGem::CreateDragonGem(int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	if( nTableID < 1 ) 
		return CDnDragonGem::Identity();

	DnDragonGemHandle hDragonGem = (new CDnDragonGem)->GetMySmartPtr();
	if( !hDragonGem ) 
		return CDnDragonGem::Identity();

	hDragonGem->Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );
	
	return hDragonGem;
}

bool CDnDragonGem::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	/*DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTALISMANITEM );
	if( !pSox ) return false;
	if( !pSox->IsExistItem( nTableID ) ) return false;*/
	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID );

	return true;
}
#endif // PRE_ADD_DRAGON_GEM