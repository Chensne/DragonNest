#include "Stdafx.h"

#include "DnGuildRewardItem.h"
#include "DnInterfaceString.h"
#include "DnUIString.h"
#include "TimeSet.h"

#include "DnGuildTask.h"

DWORD CalcTotalDays(DBTIMESTAMP &Date)
{
	//				 01  02  03  04  05  06  07  08  09  10  11  12
	int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	DWORD total = 0;

	total = (Date.year - 1) * 365 + (Date.year - 1) / 4 - (Date.year - 1) / 100 + (Date.year - 1) / 400;
	if (!(Date.year % 4) && (Date.year % 100) || !(Date.year % 400))
		months[1]++;

	for (int i = 0; i < Date.month - 1; ++i)
		total += months[i];

	total += Date.day;

	return total;
}

//////////////////////////////////////////////////////////////////////////
CDnGuildRewardItem::CDnGuildRewardItem( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
:CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_IsAvailableReward = false;
	m_IsPurchased = false;
	m_isAppliedRewardBetterThan = false;

	m_pMasterLimit = NULL;
	m_pLevelLimit = NULL;
	m_pRewardInfo = NULL;
	m_pPeriodInfo = NULL;
	m_pGold = NULL;
	m_pSilver = NULL;
	m_pBronze = NULL;

	m_pTempItem = NULL;
	m_pItemSlot = NULL;
}

CDnGuildRewardItem::~CDnGuildRewardItem(void)
{
	SAFE_DELETE(m_pTempItem);
}

void CDnGuildRewardItem::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildRewardListDlg.ui" ).c_str(), bShow );
}

void CDnGuildRewardItem::InitialUpdate()
{
	m_pMasterLimit = GetControl<CEtUIStatic>( "ID_STATIC_GUILDMASTER" );
	m_pLevelLimit = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pRewardInfo = GetControl<CEtUIStatic>( "ID_TEXT_CONTENTS" );
	m_pPeriodInfo = GetControl<CEtUIStatic>( "ID_TEXT_TIME" );
	m_pGold = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pSilver = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pBronze = GetControl<CEtUIStatic>( "ID_BRONZE" );

	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_BUTTON_REWARD");

	if (m_pItemSlot)
		m_pItemSlot->SetShowTooltip(false);
}

void CDnGuildRewardItem::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	__super::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnGuildRewardItem::Process( float fElapsedTime )
{
	UpdateInfo();
	__super::Process(fElapsedTime);
}

void CDnGuildRewardItem::UpdateInfo()
{
// 	m_IsActivated = isActivated;
// 	m_IsPurchased = isPurchased;

	//값이 아직 설정되지 않은경우 스킵..
	if (m_GuildRewardInfo.ID == -1)
		return;

	if (CDnGuildTask::IsActive() == false)
		return;

	//m_isAppliedRewardBetterThan = GetGuildTask().IsAppliedRewardByBetterThan(m_GuildRewardInfo);
	m_IsPurchased = GetGuildTask().IsPurchased(m_GuildRewardInfo);
	m_IsAvailableReward = GetGuildTask().IsAvailableReward(m_GuildRewardInfo);

	wchar_t buffer[256] = {0, };
	DWORD dwTextColor = 0xFFFFFFFF;
	std::wstring msg;

	//아이템 아이콘 활성/비활성 설정..
	if (m_pItemSlot)
		m_pItemSlot->Enable(m_IsAvailableReward || m_IsPurchased);

	bool isFunctionality = m_GuildRewardInfo.IsFunctionality();
	//if (isFunctionality)
	{
		//길드 레벨 제한으로 구매 불가능일 경우 어두운 색
		if (!m_IsAvailableReward)
			dwTextColor = 0xFFC0C0C0;

		//구입한 보상이면 밝은 색
		if (m_IsPurchased)
			dwTextColor = 0xFFFFFF80;
	}
	//길드장 제한 표시... 기능성 아이템만..
	if (m_pMasterLimit)
		m_pMasterLimit->Show(m_GuildRewardInfo._GuildMasterLimit == 1 && isFunctionality);

	//길드 제한 레벨 표시
	//swprintf_s( buffer, _countof(buffer), L"레벨 %02d", m_GuildRewardInfo._NeedGuildLevel);
	msg = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3347), m_GuildRewardInfo._NeedGuildLevel);
	if (m_pLevelLimit)
	{
		m_pLevelLimit->SetTextColor(dwTextColor);
		m_pLevelLimit->SetText(msg.c_str());
	}

	//길드 보상 내용
	MakeUIStringUseVariableParam( msg, m_GuildRewardInfo._NameID, (char*)m_GuildRewardInfo._NameIDParam.c_str() );
	if (m_pRewardInfo)
	{
		m_pRewardInfo->SetTextColor(dwTextColor);
		m_pRewardInfo->SetText(msg.c_str());
	}

	//보상 기간 표시
	if (m_pPeriodInfo)
	{
		//기능성 아이템 상관없이 보상기간 있으면 표시 하도록..
		bool isPeriodReward = m_GuildRewardInfo._Period != 0;
		//기간제 보상 또는 현재 구입한 항목
		m_pPeriodInfo->Show(isPeriodReward || m_IsPurchased);

		std::wstring str;

		if (isPeriodReward)
		{
			if (m_IsPurchased)
			{
				__time64_t today;
				time( &today );	//현재 시간..

				__time64_t expireDate = GetGuildTask().GetRemainPeriod(m_GuildRewardInfo);

				DBTIMESTAMP expireTime, todayTime;
				CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( expireDate, &expireTime );
				CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( today, &todayTime );
				DWORD expireDays = CalcTotalDays(expireTime);
				DWORD toDays = CalcTotalDays(todayTime);

				DWORD remainDays = expireDays - toDays;

				//std::wstring wszDate = FormatW(L" (%d일 남음)", remainDays);
				//str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 703);
				//str += wszDate;
				str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5160), remainDays);	//적용(%d일 남음)
			}
			else
				str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5161), m_GuildRewardInfo._Period);

			m_pPeriodInfo->SetTextColor(dwTextColor);
			m_pPeriodInfo->SetText(str.c_str());
		}
		else if (m_IsPurchased)
		{
			//str = L"적용 중";
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3391);
			m_pPeriodInfo->SetTextColor(dwTextColor);
			m_pPeriodInfo->SetText(str.c_str());
		}
	}

	SetNeedGoldInfo(dwTextColor);
}


void CDnGuildRewardItem::SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info)
{
	m_GuildRewardInfo = info;

	SetRewardItemInfo(info);

	UpdateInfo();
}

void CDnGuildRewardItem::SetNeedGoldInfo(DWORD dwColor)
{
	INT64 nCoin = m_GuildRewardInfo._NeedGold;
	INT64 nGold = nCoin/10000;
	INT64 nSilver = (nCoin%10000)/100;
	INT64 nBronze = nCoin%100;
	std::wstring strString;

	if (m_pGold)
	{
		m_pGold->SetTextColor(dwColor);
		m_pGold->SetInt64ToText( nGold );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nGold, strString );
		m_pGold->SetTooltipText( strString.c_str() );
	}

	if (m_pSilver)
	{
		m_pSilver->SetTextColor(dwColor);
		m_pSilver->SetInt64ToText( nSilver );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nSilver, strString );
		m_pSilver->SetTooltipText( strString.c_str() );
	}

	if (m_pBronze)
	{
		m_pBronze->SetTextColor(dwColor);
		m_pBronze->SetInt64ToText( nBronze );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nBronze, strString );
		m_pBronze->SetTooltipText( strString.c_str() );
	}
}

void CDnGuildRewardItem::SetRewardItemInfo( const GuildReward::GuildRewardInfo &info )
{
	if (m_pTempItem == NULL)
	{
		m_pTempItem = CDnItem::CreateItem( info._TooltipItemID, 0 );

		if (m_pItemSlot && m_pTempItem)
			m_pItemSlot->SetItem( m_pTempItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	}

	if (m_pTempItem == NULL)
		return;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if (pSox)
	{
		int nIconIndex = -1;
		eItemRank itemRank = ITEMRANK_D;
		eItemTypeEnum itemType = ITEMTYPE_NORMAL;

		const DNTableCell* pIconImageField = pSox->GetFieldFromLablePtr( info._TooltipItemID, "_IconImageIndex" );
		if (pIconImageField)
			nIconIndex = pIconImageField->GetInteger();

		const DNTableCell* pRankField = pSox->GetFieldFromLablePtr( info._TooltipItemID, "_Rank" );
		if (pRankField)
			itemRank = (eItemRank)pRankField->GetInteger();

		const DNTableCell* pItmeTypeField = pSox->GetFieldFromLablePtr( info._TooltipItemID, "_Type" );
		if (pItmeTypeField)
			itemType = (eItemTypeEnum)pItmeTypeField->GetInteger();


		m_pTempItem->ChangeIconIndex(nIconIndex);
		m_pTempItem->ChangeRank(itemRank);
		m_pTempItem->ChangeItemType(itemType);


		if (m_pItemSlot)
			m_pItemSlot->SetItem(m_pTempItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}
}