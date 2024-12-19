#include "Stdafx.h"

#include "DnGuildRewardBuyConfirmDlg.h"
#include "DnInterfaceString.h"
#include "DnUIString.h"
#include "DnGuildTask.h"
#include "DnItemTask.h"

const int ICON_TEXTURE_SIZE = 32;

CDnGuildRewardBuyConfirmDlg::CDnGuildRewardBuyConfirmDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
:CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pWarringMsg = NULL;
	m_pItemSlot = NULL;
	m_pTempItem = NULL;

	m_pRewardInfo = NULL;
	m_pPeriodInfo = NULL;
	m_pGold = NULL;
	m_pSilver = NULL;
	m_pBronze = NULL;
}

CDnGuildRewardBuyConfirmDlg::~CDnGuildRewardBuyConfirmDlg(void)
{
	SAFE_DELETE(m_pTempItem);
}

void CDnGuildRewardBuyConfirmDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildRewardMessageBoxDlg.ui" ).c_str(), bShow );
}

void CDnGuildRewardBuyConfirmDlg::InitialUpdate()
{
	m_pWarringMsg = GetControl<CEtUIStatic>( "ID_TEXT_ASK" );
	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_BUTTON_ITEM");

	m_pRewardInfo = GetControl<CEtUIStatic>( "ID_TEXT_EXP" );
	m_pPeriodInfo = GetControl<CEtUIStatic>( "ID_TEXT_TIME" );
	m_pGold = GetControl<CEtUIStatic>( "ID_GOLD" );
	m_pSilver = GetControl<CEtUIStatic>( "ID_SILVER" );
	m_pBronze = GetControl<CEtUIStatic>( "ID_BRONZE" );

// 	if (m_pItemSlot)
// 		m_pItemSlot->SetShowTooltip(false);
}

void CDnGuildRewardBuyConfirmDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK" ) ) 
		{
			//길드 보상 아이템 구입 패킷 전송....
			GetGuildTask().RequestBuyGuildRewardItem(m_GuildRewardInfo.ID);
			return;
		}
		else if( IsCmdControl("ID_CANCEL" )  || IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show(false);
			return;
		}
	}

	__super::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnGuildRewardBuyConfirmDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnGuildRewardBuyConfirmDlg::Process( float fElapsedTime )
{
	__super::Process(fElapsedTime);
}


void CDnGuildRewardBuyConfirmDlg::SetGuildRewardInfo(const GuildReward::GuildRewardInfo &info)
{
	m_GuildRewardInfo = info;

	//길드 보상 내용
	std::wstring msg;
	MakeUIStringUseVariableParam( msg, m_GuildRewardInfo._NameID, (char*)m_GuildRewardInfo._NameIDParam.c_str() );
	if (m_pRewardInfo)
	{
		m_pRewardInfo->SetText(msg.c_str());
	}

	SetRewardItemInfo(info);

	//보상 기간 표시
	if (m_pPeriodInfo)
	{
		bool isPeriodReward = m_GuildRewardInfo._Period != 0;

		if (isPeriodReward)
		{
			std::wstring str;
			str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5161), m_GuildRewardInfo._Period);
			m_pPeriodInfo->SetText(str.c_str());
		}

		//기간제 보상일때만 표시..
		m_pPeriodInfo->Show(isPeriodReward);
	}
	
	SetNeedGoldInfo();

	//경고 메시지 표시..
	int nUIStringTableID = 0;
	bool isPeriodReward = m_GuildRewardInfo._Period != 0;	//기간제 보상인지..
		
	//같은 카테고리에 보상 아이템ID를 얻어서
	int AppliedRewardItemID = GetGuildTask().GetPurchasedItemID(m_GuildRewardInfo);

	if (AppliedRewardItemID == 0)
	{
		nUIStringTableID = 5163;		//새로 구입하는 경우
	}
	else
	{
		if (isPeriodReward)
		{
			//같은 카테고리에 같은 ID의 보상이 적용되어 있는지 확인..
			//같은 카테고리에 같은 ID 보상이 적용되어 있으면 기간 연장
			if (AppliedRewardItemID == m_GuildRewardInfo.ID)
				nUIStringTableID = 5165;		//기간 연장..
			else	//
				nUIStringTableID = 5164;		//보상 대체
		}
		else
			nUIStringTableID = 5164;		//보상 대체.
	}

	if (nUIStringTableID != 0)
		m_pWarringMsg->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nUIStringTableID));
	else
		m_pWarringMsg->SetText(L"");
}

void CDnGuildRewardBuyConfirmDlg::SetNeedGoldInfo()
{
	INT64 nCoin = m_GuildRewardInfo._NeedGold;
	INT64 nGold = nCoin/10000;
	INT64 nSilver = (nCoin%10000)/100;
	INT64 nBronze = nCoin%100;
	std::wstring strString;

	if (m_pGold)
	{
		//m_pGold->SetTextColor(dwColor);
		m_pGold->SetInt64ToText( nGold );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nGold, strString );
		m_pGold->SetTooltipText( strString.c_str() );
	}

	if (m_pSilver)
	{
		//m_pSilver->SetTextColor(dwColor);
		m_pSilver->SetInt64ToText( nSilver );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nSilver, strString );
		m_pSilver->SetTooltipText( strString.c_str() );
	}

	if (m_pBronze)
	{
		//m_pBronze->SetTextColor(dwColor);
		m_pBronze->SetInt64ToText( nBronze );
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nBronze, strString );
		m_pBronze->SetTooltipText( strString.c_str() );
	}
}

void CDnGuildRewardBuyConfirmDlg::SetRewardItemInfo( const GuildReward::GuildRewardInfo &info )
{
	SAFE_DELETE(m_pTempItem);

	if (m_pTempItem == NULL)
	{
		TItemInfo itemInfo;
		memset( &itemInfo, 0, sizeof(TItemInfo));
		itemInfo.Item.nItemID = info._TooltipItemID;	
		
		m_pTempItem = GetItemTask().CreateItem( itemInfo );//CDnItem::CreateItem( info._TooltipItemID, 0 );

		if (m_pItemSlot && m_pTempItem)
			m_pItemSlot->SetItem( m_pTempItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	}
}
