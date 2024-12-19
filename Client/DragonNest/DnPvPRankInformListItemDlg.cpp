#include "Stdafx.h"

#ifdef PRE_ADD_PVPRANK_INFORM
#include "DnPvPRankInformListItemDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"

CDnPvPRankInformListItemDlg::CDnPvPRankInformListItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pRankIcon = NULL;
	m_pRankName = NULL;
	m_pNeedExp = NULL;
	m_pBackground = NULL;

	int i = 0;
	for (; i < PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT; ++i)
	{
		m_pRewardItem[i] = NULL;
	}
}

CDnPvPRankInformListItemDlg::~CDnPvPRankInformListItemDlg()
{
	ClearListItem();
}

void CDnPvPRankInformListItemDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RankGradeListDlg.ui" ).c_str(), bShow );
}

void CDnPvPRankInformListItemDlg::InitialUpdate()
{
	m_pRankIcon = GetControl<CEtUITextureControl>("ID_TEXTUREL_RANK");
	m_pRankName = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pNeedExp = GetControl<CEtUIStatic>("ID_TEXT_EXP");
	m_pBackground = GetControl<CEtUIStatic>("ID_STATIC_BAR0");

	int i = 0;
	for (; i < PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT; ++i)
	{
		std::string btnStr = FormatA("ID_BT_ITEM%d", i);
		m_pRewardItem[i] = GetControl<CDnItemSlotButton>(btnStr.c_str());
	}
}

void CDnPvPRankInformListItemDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnPvPRankInformListItemDlg::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);
}

void CDnPvPRankInformListItemDlg::SetData(const PvPRankInfoUIDef::SRankUnit& items)
{
	GetInterface().SetPvPClassIcon(m_pRankIcon, items.level);
	m_pRankName->SetText(items.name.c_str());
	m_pRankName->SetTextColor(items.lineColor);

	if (items.needExp <= 0)
	{
		m_pNeedExp->ClearText();
	}
	else
	{
		std::wstring needExpString = AtoCommaString(items.needExp);
		m_pNeedExp->SetText(needExpString.c_str());
		m_pNeedExp->SetTextColor(items.lineColor);
	}

	if (items.lineColor == 0xFFFF0000)
	{
		m_pNeedExp->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8424)); // UISTRING : ¿µ±¤ÀÇ 1µî
		m_pNeedExp->SetTextColor(items.lineColor);
	}

	int i = 0;
	for (; i < PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT; ++i)
	{
		int maxCount = (int)items.rewards.size();
		if (i >= maxCount)
			break;

		const PvPRankInfoUIDef::SRankRewardItemUnit& unit = items.rewards[i];
		if (unit.id < 0 || unit.count <= 0)
			continue;

		TItem itemInfo;
		CDnItem::MakeItemInfo(unit.id, unit.count, itemInfo);
		CDnItem* pItem = CDnItemTask::GetInstance().CreateItem(itemInfo);

		if (pItem != NULL)
			m_pRewardItem[i]->SetItem(pItem, pItem->GetOverlapCount());
	}
}

void CDnPvPRankInformListItemDlg::ClearListItem()
{
	int i = 0;
	for (; i < PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT; ++i)
	{
		CDnItem* pItem = static_cast<CDnItem*>(m_pRewardItem[i]->GetItem());
		SAFE_DELETE(pItem);
	}
}

float CDnPvPRankInformListItemDlg::GetCurrentListUIHeight() const
{
	if (m_pBackground)
		return m_pBackground->GetBaseUICoord().fHeight;

	return -1.f;
}

#endif // PRE_ADD_PVPRANK_INFORM