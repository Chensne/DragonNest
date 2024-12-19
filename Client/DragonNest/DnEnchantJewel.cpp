#include "StdAfx.h"
#include "DnEnchantJewel.h"
#include "DnTableDB.h"
#include "DnActor.h"

CDnEnchantJewel::CDnEnchantJewel()
{
	m_nApplyApplicableValue = 0;
	m_nRequireMaxItemLevel = 0;
	m_nRequireEnchantLevel = 0;
	m_nEnchantLevel = 0;
	m_bCanApplySealedItem = false;
}

CDnEnchantJewel::~CDnEnchantJewel()
{
}

bool CDnEnchantJewel::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	if( CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANTJEWEL );
	if( !pSox ) return false;

	m_nApplyApplicableValue = pSox->GetFieldFromLablePtr( nTableID, "_ApplicableValue" )->GetInteger();
	m_nRequireMaxItemLevel = pSox->GetFieldFromLablePtr( nTableID, "_RequireMaxItemLevel" )->GetInteger();
	m_nRequireEnchantLevel = pSox->GetFieldFromLablePtr( nTableID, "_RequireEnchantLevel" )->GetInteger();
	m_nEnchantLevel = pSox->GetFieldFromLablePtr( nTableID, "_EnchantLevel" )->GetInteger();
	m_bCanApplySealedItem = ( pSox->GetFieldFromLablePtr( nTableID, "_CanApplySealedItem" )->GetInteger() == TRUE ) ? true : false;

	return true;
}