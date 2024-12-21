#include "StdAfx.h"
#include "DnSecondarySkillRecipeItem.h"


#ifdef PRE_ADD_SECONDARY_SKILL

CDnSecondarySkillRecipeItem::CDnSecondarySkillRecipeItem() : m_nDurability( 0 )
{
}

CDnSecondarySkillRecipeItem::~CDnSecondarySkillRecipeItem()
{
}

void CDnSecondarySkillRecipeItem::GetTItemInfo( TItemInfo &Info )
{
	// 이건 다른 창에 등록하는 복사용아이템이긴 하지만, cSlotIndex 역시 필요하다.
	// 패킷요청시 이 복사용아이템의 정보를 사용할때도 있으니..
	Info.cSlotIndex = m_nSlotIndex;
	Info.Item.nItemID = m_nClassID;
	Info.Item.wCount = m_nOverlapCount;
	Info.Item.cLevel = m_cEnchantLevel;
	Info.Item.cPotential = m_cPotentialIndex;
	Info.Item.cOption = m_cOptionIndex;
	Info.Item.cSealCount = m_cSealCount;
	Info.Item.bSoulbound = (m_Reversion == ITEMREVERSION_BELONG) ? m_bSoulBound : true;
	Info.Item.nSerial = m_nSerialID;
	Info.Item.wDur = m_nDurability;
}
#endif // PRE_ADD_SECONDARY_SKILL