#include "StdAfx.h"
#include "DnPartsHelmet.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPartsHelmet::CDnPartsHelmet()
{
	m_nHairShowLevel = 0;
	m_bAttachHeadBone = false;
}

CDnPartsHelmet::~CDnPartsHelmet()
{
}

bool CDnPartsHelmet::Initialize( int nPartsTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	if( CDnParts::Initialize( nPartsTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPARTS );
	if( !pSox ) return false;

	int nLookPartsId = (nLookItemID != ITEMCLSID_NONE) ? nLookItemID : nPartsTableID;
	m_nHairShowLevel = (DWORD)pSox->GetFieldFromLablePtr( nLookPartsId, "_HelmetShowLevel" )->GetInteger();
	m_bAttachHeadBone = ( pSox->GetFieldFromLablePtr( nLookPartsId, "_AttachHeadBone" )->GetInteger() ) ? true : false;

	return true;
}


bool CDnPartsHelmet::IsLoadPartsAni()
{
	if( IsCashItem() && IsAttachHeadBone() ) return true;
	return CDnParts::IsLoadPartsAni();
}