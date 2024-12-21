#include "StdAfx.h"
#include "DnPotentialJewel.h"
#include "DnTableDB.h"
#include "DnActor.h"


CDnPotentialJewel::CDnPotentialJewel()
{
	m_nApplyApplicableValue = 0;
	m_nPotentialNo = 0;
	m_bErasable = false;
	m_bCanApplySealedItem = false;

#if defined(PRE_ADD_REMOVE_PREFIX)
	m_IsSuffix = false;
#endif // PRE_ADD_REMOVE_PREFIX

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	m_nChargeCoin = 0;
	m_bIsRemovePotentialItem = false;
#endif
}

CDnPotentialJewel::~CDnPotentialJewel()
{
}

bool CDnPotentialJewel::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID )
{
	if( CDnItem::Initialize( nTableID, nSeed, cOptionIndex, cEnchantLevel, cPotentialIndex, cSealCount, bSoulBound, nLookItemID ) == false )
		return false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPOTENTIALJEWEL );
	if( !pSox ) return false;

	m_nApplyApplicableValue = pSox->GetFieldFromLablePtr( nTableID, "_ApplicableValue" )->GetInteger();
	m_nPotentialNo = pSox->GetFieldFromLablePtr( nTableID, "_PotentialNo" )->GetInteger();
	m_bErasable = ( pSox->GetFieldFromLablePtr( nTableID, "_IsErasable" )->GetInteger() == TRUE ) ? true : false;
	m_bCanApplySealedItem = ( pSox->GetFieldFromLablePtr( nTableID, "_CanApplySealedItem" )->GetInteger() == TRUE ) ? true : false;

#if defined(PRE_ADD_REMOVE_PREFIX)
	m_IsSuffix = ( pSox->GetFieldFromLablePtr( nTableID, "_Issuffix" )->GetInteger() == TRUE ) ? true : false;
#endif // PRE_ADD_REMOVE_PREFIX

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	m_nChargeCoin = pSox->GetFieldFromLablePtr( nTableID, "_Charge" )->GetInteger();
	m_bIsRemovePotentialItem = (pSox->GetFieldFromLablePtr( nTableID, "_PopupType" )->GetInteger()) ? true : false;
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

	return true;
}