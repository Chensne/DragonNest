#pragma once

#include "DnItem.h"

class CDnPotentialJewel : public CDnItem
{
public:
	CDnPotentialJewel();
	virtual ~CDnPotentialJewel();

protected:
	int m_nApplyApplicableValue;
	int m_nPotentialNo;
	bool m_bErasable;
	bool m_bCanApplySealedItem;
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	INT64 m_nChargeCoin;
	bool  m_bIsRemovePotentialItem;
#endif

public:
	virtual bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE );

	int		GetApplyApplicableValue() { return m_nApplyApplicableValue; }
	int		GetPotentialNo() { return m_nPotentialNo; }
	bool	IsErasable() { return m_bErasable; }
	bool	IsCanApplySealedItem() { return m_bCanApplySealedItem; }
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	INT64	GetChargeCoin()			{ return m_nChargeCoin; }
	bool	IsRemovePotentialItem() { return m_bIsRemovePotentialItem; }
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#if defined(PRE_ADD_REMOVE_PREFIX)
protected:
	bool m_IsSuffix;
public:
	bool IsSuffix() { return m_IsSuffix; }
#endif // PRE_ADD_REMOVE_PREFIX
};