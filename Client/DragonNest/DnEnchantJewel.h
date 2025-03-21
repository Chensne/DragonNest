#pragma once

#include "DnItem.h"

class CDnEnchantJewel : public CDnItem
{
public:
	CDnEnchantJewel();
	virtual ~CDnEnchantJewel();

protected:
	int m_nApplyApplicableValue;
	int m_nRequireMaxItemLevel;
	int m_nRequireEnchantLevel;
	int m_nEnchantLevel;
	bool m_bCanApplySealedItem;

public:
	virtual bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false, int nLookItemID = ITEMCLSID_NONE);

	int GetApplyApplicableValue() { return m_nApplyApplicableValue; }
	int GetRequireMaxItemLevel() { return m_nRequireMaxItemLevel; }
	int GetRequireEnchantLevel() { return m_nRequireEnchantLevel; }
	int GetEnchantLevel() { return m_nEnchantLevel; }
	bool IsCanApplySealedItem(){ return m_bCanApplySealedItem; }
};