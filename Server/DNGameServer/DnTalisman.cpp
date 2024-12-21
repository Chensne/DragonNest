#include "stdafx.h"
#if defined(PRE_ADD_TALISMAN_SYSTEM)

#include "DnTalisman.h"
#include "DNTableFile.h"
#include "DnTableDB.h"

DECL_DN_SMART_PTR_STATIC( CDnTalisman, MAX_SESSION_COUNT, 100 )


CDnTalisman::CDnTalisman( CMultiRoom *pRoom )
: CDnUnknownRenderObject(pRoom, false)
, CDnItem( pRoom )
{
}

CDnTalisman::~CDnTalisman()
{
}

DnTalismanHandle CDnTalisman::CreateTalisman( CMultiRoom *pRoom, int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound )
{
	if( nTableID < 1 ) return CDnTalisman::Identity();

	DnTalismanHandle hTalisman = (new CDnTalisman(pRoom))->GetMySmartPtr();
	if( !hTalisman ) return CDnTalisman::Identity();

	hTalisman->Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound );
	return hTalisman;
}

bool CDnTalisman::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTALISMANITEM );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"TalismanTalbe.ext failed\r\n");
		return false;
	}	
	if( !pSox->IsExistItem( nTableID ) ) return false;	

	CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound );

	return true;
}

#endif // PRE_ADD_TALISMAN_SYSTEM