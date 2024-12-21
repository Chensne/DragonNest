#include "StdAfx.h"
#include "DnCashShopRecvGiftDlg.h"
#include "DnCashShopRecvGiftItemDlg.h"
#include "DnCashShopTask.h"
#include "DnCashShopPayItemDlg.h"
#include "DnItemTask.h"
#include "DnCommonUtil.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnDataManager.h"
#if defined(PRE_ADD_68286)
#include "TaskManager.h"
#include "DnCommonTask.h"
#endif

#if defined(PRE_ADD_68286)
#include "DnLocalPlayerActor.h"
#include "DnCashShopDlg.h"
#endif

#if defined(PRE_RECEIVEGIFTALL)
#include "CashShopSendPacket.h"
#include "DnItemTask.h"
#include "DNGestureTask.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
void CDnCashShopRecvGiftDlg::SCashShopGiftSlotUnit::Set(CASHITEM_SN sn, ITEMCLSID itemId)
{
	const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(sn);
	if (pItemInfo == NULL)
		return;

	pName->SetText(pItemInfo->nameString.c_str());

	ITEMCLSID slotItemID = itemId;
	if (itemId == ITEMCLSID_NONE || CDnCashShopTask::GetInstance().IsPackageItem(sn))
		slotItemID = pItemInfo->presentItemId;

	if (slotItemID != ITEMCLSID_NONE)
	{
		SAFE_DELETE(pItem);
		TItemInfo itemInfo;
		if (CDnItem::MakeItemInfo(slotItemID, pItemInfo->count, itemInfo))
		{
			pItem = GetItemTask().CreateItem(itemInfo);
			if (pItem)
			{
				pItem->SetCashItemSN(sn);
				pItem->SetEternityItem(CDnCashShopTask::GetInstance().IsItemPermanent(sn));
				pSlotBtn->SetItem(pItem, pItem->GetOverlapCount());
			}
		}
	}

	std::wstring str;
	str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4625), pItemInfo->count);	// UISTRING : %d 개
	pCount->SetText(str.c_str());

	if (pItemInfo->period > 0)
	{
		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4613), pItemInfo->period);	// UISTRING : (%d 일)
		pPeriod->SetTextColor(textcolor::YELLOW);
		pPeriod->SetText(str.c_str());
	}
	else
	{
		pPeriod->Show(false);
	}

#ifdef PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030002;	// UISTRING : %s페탈
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4647;	// UISTRING : %d페탈
#endif // PRE_MOD_PETAL_WRITE
#else // PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030001;	// UISTRING : %s캐시
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4642;	// UISTRING : %d캐시
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_PETALSHOP

#ifdef PRE_MOD_PETAL_WRITE
	str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), DN_INTERFACE::UTIL::GetAddCommaString( pItemInfo->price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
	str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), pItemInfo->price );
#endif // PRE_MOD_PETAL_WRITE
	
	pPrice->SetText(str.c_str());

	if (pItemInfo->type == eCSType_Costume)
	{
		std::wstring strAbility = GetCashShopTask().GetAbilityString( itemId );
		if( strAbility.length() > 0 )
		{
			pAbility->SetTextWithEllipsis( strAbility.c_str(), L"..." );
			std::wstring strRenderText = pAbility->GetRenderText();
			if( wcscmp( strAbility.c_str(), strRenderText.c_str() ) != 0 )
				pAbility->SetTooltipText( strAbility.c_str() );
			else
				pAbility->SetTooltipText( L"" );
		}
		else
			pAbility->SetTooltipText( L"" );
	}
	else
		pAbility->SetTooltipText( L"" );
}

void CDnCashShopRecvGiftDlg::SCashShopGiftInfoUnit::Set(const SCashShopRecvGiftBasicInfo& info)
{
	giftDBID = info.giftDBID;

#if defined(PRE_ADD_GIFT_RETURN)
	pSender->SetTextColor(textcolor::WHITE);
	if( info.giftReturn )
	{
		pSender->SetTextColor(textcolor::RED);
	}
#endif
	pSender->SetText(info.name.c_str());
	pRecvDate->SetText(info.recvDate.c_str());
	pExpireDate->SetText(info.expireDate.c_str());
}

CDnCashShopRecvGiftDlg::CDnCashShopRecvGiftDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
				:CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pPrevBtn			= NULL;
	m_pNextBtn			= NULL;
	m_pPageNum			= NULL;

	m_pSenderMemoName	= NULL;
	m_pSenderMemoBox	= NULL;

	m_pRecvBtn			= NULL;
	//m_pDelBtn			= NULL;
	m_pCloseBtn			= NULL;
#if defined(PRE_ADD_GIFT_RETURN)
	m_pNonAcceptBtn		= NULL;	
#endif	
	m_pReplyEditBox		= NULL;
	m_pInfoBaseBoard	= NULL;
#if defined(PRE_RECEIVEGIFTALL)
	m_pButtonTakeAll	= NULL;
#endif

	int i = 0;
	for (; i < MAX_GIFT_IMPRESSION_RADIO_BTN; ++i)
	{
		m_pImpressionBtn[i] = NULL;
		m_pImpressionIcon[i] = NULL;
	}

	m_CurrentImprsnId			= 0;
	m_IndexSelected				= -1;

	for (i = 0; i < 3; ++i)
		m_pReactStatic[i] = NULL;
	m_pReactTitle		= NULL;
	m_pReplyTitle		= NULL;
	m_pReplyBackGround	= NULL;

	m_RefreshServerInfoTimer	= 0;
	m_bRequesting				= false;
	m_CurrentPage				= 1;
	m_MaxPage					= 0;	
}

void CDnCashShopRecvGiftDlg::Initialize(bool bShow)
{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_CSPresentClip.ui").c_str(), bShow);
#else
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPresentClip.ui").c_str(), bShow);
#endif // PRE_ADD_CASHSHOP_RENEWAL
}

void CDnCashShopRecvGiftDlg::InitialUpdate()
{
	char szControlName[32]={0};

	int i = 0;
	for (i = 0; i < MAX_GIFT_RECV_INFO; ++i)
	{
		SCashShopGiftInfoUnit unit;
		sprintf_s(szControlName, 32, "ID_STATIC_NAME%d", i);
		unit.pSender = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_RDATE%d", i);
		unit.pRecvDate = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_EDATE%d", i);
		unit.pExpireDate = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_SELECT%d", i);
		unit.pSelected = GetControl<CEtUIStatic>(szControlName);
		unit.pSelected->Show(false);

		m_SenderInfoList.push_back(unit);
	}

	m_SlotUnit.pBase	= GetControl<CEtUIStatic>("ID_STATIC_PBASE0");
	m_SlotUnit.pName	= GetControl<CEtUIStatic>("ID_STATIC_ITEMNAME0");
	m_SlotUnit.pCount	= GetControl<CEtUIStatic>("ID_STATIC_COUNT0");
	m_SlotUnit.pAbility = GetControl<CEtUIStatic>("ID_STATIC_ABILTY0");
	m_SlotUnit.pPeriod	= GetControl<CEtUIStatic>("ID_STATIC_PERIOD0");
	m_SlotUnit.pPrice	= GetControl<CEtUIStatic>("ID_STATIC_PRICE0");

	m_pPrevBtn		= GetControl<CEtUIButton>("ID_BUTTON_PREV");
	m_pNextBtn		= GetControl<CEtUIButton>("ID_BUTTON_NEXT");
	m_pPageNum		= GetControl<CEtUIStatic>("ID_STATIC_COUNT");
	m_pInfoBaseBoard = GetControl<CEtUIStatic>("ID_STATIC8");

	m_pSenderMemoName	= GetControl<CEtUIStatic>("ID_STATIC_SEND");
	m_pSenderMemoBox	= GetControl<CEtUITextBox>("ID_TEXTBOX_MEMO");

	m_pRecvBtn			= GetControl<CEtUIButton>("ID_BUTTON_TAKE");
	//m_pDelBtn			= GetControl<CEtUIButton>("ID_BUTTON_DELET");
	m_pCloseBtn			= GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
#if defined(PRE_ADD_GIFT_RETURN)
	m_pNonAcceptBtn		= GetControl<CEtUIButton>("ID_BUTTON_NONACCEPT");
	if( m_pNonAcceptBtn )
		m_pNonAcceptBtn->Enable(false);
#endif

	m_pReplyEditBox		= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_MEMO");
	std::string impControlId;
	for (i = 0; i < MAX_GIFT_IMPRESSION_RADIO_BTN; ++i)
	{
		impControlId = FormatA("ID_RADIOBUTTON_REACT%d", i);
		m_pImpressionBtn[i] = GetControl<CEtUIRadioButton>(impControlId.c_str());

		impControlId = FormatA("ID_STATIC_REACTICON%d", i);
		m_pImpressionIcon[i] = GetControl<CEtUIStatic>(impControlId.c_str());
		m_pImpressionIcon[i]->SetButton(true);
	}

	std::string str;
	for (i = 0; i < 3; ++i)
	{
		str = FormatA("ID_STATIC_REACT%d", i);
		m_pReactStatic[i] = GetControl<CEtUIStatic>(str.c_str());
	}
	m_pReactTitle		= GetControl<CEtUIStatic>("ID_STATIC1");
	m_pReplyTitle		= GetControl<CEtUIStatic>("ID_STATIC5");
	m_pReplyBackGround	= GetControl<CEtUIStatic>("ID_STATIC2");

#ifdef PRE_RECEIVEGIFTALL
	m_pButtonTakeAll = GetControl<CEtUIButton>("ID_BUTTON_TAKEALL");
	m_pButtonTakeAll->Enable(false);
	m_pButtonTakeAll->Show(true);
#endif // PRE_RECEIVEGIFTALL

}

void CDnCashShopRecvGiftDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BUTTON_ITEM"))
		return;

	CDnItemSlotButton *pItemSlotBtn(NULL);
	pItemSlotBtn = static_cast<CDnItemSlotButton*>(pControl);

	pItemSlotBtn->SetSlotType(ST_INVENTORY);
	pItemSlotBtn->SetSlotIndex(0);

	m_SlotUnit.pSlotBtn = pItemSlotBtn;
}

#define MAX_GIFT_LIST_REQUEST_TIMER 10

void CDnCashShopRecvGiftDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		if (GetCashShopTask().IsRequestingGiftBasicInfoes() == true)
			return;

		Clear();
		UpdateInfo();
	}
#if defined(PRE_ADD_68286)
	else
	{
		if (CDnCashShopTask::IsActive() && GetCashShopTask().IsOpenCashShop() == false)
		{
			CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pTask)
				pTask->SendWindowState(WINDOW_NONE);
			CDnLocalPlayerActor::LockInput(false);
		}
	}
#endif

	CDnCustomDlg::Show(bShow);
}

void CDnCashShopRecvGiftDlg::Clear()
{
	int i = 0;
	for (; i < (int)m_SenderInfoList.size(); ++i)
		m_SenderInfoList[i].Clear();

	m_SlotUnit.Clear();
	m_pSenderMemoName->ClearText();
	m_pSenderMemoBox->ClearText();
	m_pReplyEditBox->ClearText();

	//m_pImpressionBtn[0]->SetChecked(true);
	//m_CurrentImprsnId = m_pImpressionBtn[0]->GetTabID();
	for (; i < MAX_GIFT_IMPRESSION_RADIO_BTN; ++i)
	{
		if (m_pImpressionBtn[i]->IsChecked())
			m_pImpressionBtn[i]->SetChecked(false);
	}
	m_CurrentImprsnId = -1;
	m_IndexSelected = -1;

	m_CurrentPage = 1;
}

void CDnCashShopRecvGiftDlg::UpdateInfo()
{
	const RECV_GIFT_LIST& giftList = GetCashShopTask().GetRecvGiftList();

	UpdatePageControllers(giftList);

	int i = 0;
	for (; i < (int)m_SenderInfoList.size(); ++i)
		m_SenderInfoList[i].Clear();

	for (i = 0; i < (int)m_SenderInfoList.size(); ++i)
	{
		SCashShopGiftInfoUnit& unit = m_SenderInfoList[i];
		int index = (m_CurrentPage - 1) * MAX_GIFT_RECV_INFO + i;
		if (index < 0 || index >= (int)giftList.size())
			break;

		unit.Set(giftList[index]);
	}
}

void CDnCashShopRecvGiftDlg::UpdatePage()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_CurrentPage, m_MaxPage);
	m_pPageNum->SetText(wszPage);
}

void CDnCashShopRecvGiftDlg::UpdatePageControllers(const RECV_GIFT_LIST& itemList)
{
	int listSize = (int)itemList.size();

#ifdef PRE_RECEIVEGIFTALL
	bool bIsEnable = false;
	if(listSize > 1) bIsEnable = true;
	m_pButtonTakeAll->Enable(bIsEnable); // 아이템이 1개있으면 비활성화
#endif // PRE_RECEIVEGIFTALL

	m_MaxPage = listSize / MAX_GIFT_RECV_INFO;
	if ((listSize % MAX_GIFT_RECV_INFO) != 0)
		m_MaxPage++;

	if (m_MaxPage < m_CurrentPage)
		m_CurrentPage = m_MaxPage;

	UpdatePage();

	m_pPrevBtn->Enable(m_CurrentPage > 1);
	m_pNextBtn->Enable(m_CurrentPage < m_MaxPage);
}

void CDnCashShopRecvGiftDlg::PrevPage()
{
	const RECV_GIFT_LIST& itemList = GetCashShopTask().GetRecvGiftList();

	m_CurrentPage--;
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	//	Set Page Static
	UpdatePageControllers(itemList);
	UpdateInfo();
}

void CDnCashShopRecvGiftDlg::NextPage()
{
	const RECV_GIFT_LIST& itemList = GetCashShopTask().GetRecvGiftList();

	m_CurrentPage++;
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	//	Set Page Static
	UpdatePageControllers(itemList);
	UpdateInfo();
}

void CDnCashShopRecvGiftDlg::Process(float fElapsedTime)
{
	if (m_bShow == false)
		return;

	const std::wstring& str = m_pPageNum->GetText();
	m_pPrevBtn->Enable(str.empty() == false);
	m_pNextBtn->Enable(str.empty() == false);

	if ((m_IndexSelected < 0 || m_IndexSelected >= MAX_GIFT_RECV_INFO) && m_pReplyBackGround && m_pReplyBackGround->IsEnable() == true)
		SetReactionUI(false);

	CEtUIDialog::Process(fElapsedTime);
}

void CDnCashShopRecvGiftDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show(false);
			return;
		}

		if( IsCmdControl("ID_BUTTON_PREV" ) )
		{
			PrevPage();
			return;
		}

		if( IsCmdControl("ID_BUTTON_NEXT" ) )
		{
			NextPage();
			return;
		}

#ifdef PRE_RECEIVEGIFTALL
		if( IsCmdControl("ID_BUTTON_TAKEALL") )
		{
			std::wstring testStr(m_pReplyEditBox->GetText());
			bool bProhibition = DN_INTERFACE::UTIL::CheckChat(testStr);
			if(bProhibition)
			{
				std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4577); // UISTRING : 메모에 금칙어가 포함되어 선물을 받을 수 없습니다
				GetInterface().MessageBox(str.c_str(), MB_OK);
				return;
			}

			TReceiveGiftData				GiftData;
			std::vector<TReceiveGiftData>	vGiftDataList;		
			for(int i = 0 ; i < (int)m_SenderInfoList.size() ; ++i)
			{
				if( i > GIFTPAGEMAX-1 )
					break;

				const SCashShopGiftInfoUnit&	  unit  = m_SenderInfoList[i];
				const SCashShopRecvGiftBasicInfo* pInfo = GetCashShopTask().GetRecvGiftInfo(unit.giftDBID);
				if(!pInfo) 
					continue;
				
				GiftData.nGiftDBID		= pInfo->giftDBID;
				GiftData.cPayMethodCode = pInfo->payMethodCode;
				_wcscpy(GiftData.wszReplyMessage, _countof(GiftData.wszReplyMessage), testStr.c_str(), (int)wcslen(testStr.c_str()));
				
				std::wstring wstrEmoticon;
				std::map<int,std::vector<int>> GiftReactInfo = GetCashShopTask().GetGiftReactInfo();
				if(GiftReactInfo.empty()) continue;
				
				if(m_CurrentImprsnId == -1) // #78252 이모티콘을 선택하지 않았으면, 자동으로 '만족'선택
					m_CurrentImprsnId = 0;

				std::map<int, std::vector<int> >::const_iterator iter = GiftReactInfo.find(m_CurrentImprsnId + 1);
				if (iter != GiftReactInfo.end() && CDnActor::s_hLocalActor) {
					const std::vector<int>& reacts = (*iter).second;
					int index = _rand() % (int)reacts.size();
					std::wstring formatStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, reacts[index]);
					wstrEmoticon = FormatW(formatStr.c_str(), CDnActor::s_hLocalActor->GetName());
				}
				_wcscpy(GiftData.wszEmoticonTitle, _countof(GiftData.wszEmoticonTitle), wstrEmoticon.c_str(), (int)wcslen(wstrEmoticon.c_str()));
				
				vGiftDataList.push_back(GiftData);
			}

			if( !vGiftDataList.empty() )
			{
				SendCashShopRecvGiftAll((char)vGiftDataList.size(), &vGiftDataList[0]);
				GetCashShopTask().SetSendItemSize((int)vGiftDataList.size());
			}
			
			return;
		}
#endif

		if (IsCmdControl("ID_BUTTON_TAKE"))
		{
			//if (m_CurrentGiftDBID != 0)
			std::wstring testStr(m_pReplyEditBox->GetText());
			bool bProhibition = DN_INTERFACE::UTIL::CheckChat(testStr);
			if (bProhibition)
			{
				std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4577); // UISTRING : 메모에 금칙어가 포함되어 선물을 받을 수 없습니다
				GetInterface().MessageBox(str.c_str(), MB_OK);
				return;
			}

			if (m_IndexSelected >= 0 && m_IndexSelected < MAX_GIFT_RECV_INFO)
			{
				const SCashShopGiftInfoUnit& unit = m_SenderInfoList[m_IndexSelected];
				const SCashShopRecvGiftBasicInfo* pInfo = GetCashShopTask().GetRecvGiftInfo(unit.giftDBID);
				char cPayMethodCode = 0;
				if (pInfo) cPayMethodCode = pInfo->payMethodCode;
				
				GetCashShopTask().RequestCashShopRecvGift(unit.giftDBID, m_CurrentImprsnId, m_pReplyEditBox->GetText(), cPayMethodCode);
			}
			else
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4790), MB_OK); // UISTRING : 받을 선물을 선택해 주세요
			}
		}
#if defined(PRE_ADD_GIFT_RETURN)
		if (IsCmdControl("ID_BUTTON_NONACCEPT"))
		{
			if (m_IndexSelected >= 0 && m_IndexSelected < MAX_GIFT_RECV_INFO)
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4594), MB_YESNO, 0, this ); // UISTRING : 정말로 선물을 보낸사람에게 되돌려 보내시겠습니까? 되돌려 보낸 선물은 다시 받을 수 없습니다
			}
			return;
		}
#endif

		const std::string& cmdName = GetCmdControlName();
		std::string::size_type numberPos = cmdName.find("ID_STATIC_REACTICON");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_STATIC_REACTICON");
			if (pieceLen < cmdName.size())
			{
				int num = atoi(&(cmdName[pieceLen]));
				if (num >= 0 && num < MAX_GIFT_IMPRESSION_RADIO_BTN)
				{
					m_pImpressionBtn[num]->SetChecked(true);
					m_CurrentImprsnId = m_pImpressionBtn[num]->GetTabID();
				}
			}
		}
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
		UI_CONTROL_TYPE type = pControl->GetType();
		if (type == UI_CONTROL_RADIOBUTTON)
		{
			CEtUIRadioButton* pBtn = static_cast<CEtUIRadioButton*>(pControl);
			m_CurrentImprsnId = pBtn->GetTabID();
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopRecvGiftDlg::SetReactionUI(bool bEnable)
{
	if (bEnable == false)
	{
		m_CurrentImprsnId = -1;
		m_pReplyEditBox->Enable(false);
		int i = 0;
		for (; i < MAX_GIFT_IMPRESSION_RADIO_BTN; ++i)
		{
			m_pImpressionIcon[i]->Enable(false);
			m_pImpressionBtn[i]->Enable(false);
			if (m_pImpressionBtn[i]->IsChecked())
				m_pImpressionBtn[i]->SetChecked(false);
		}

		for (i = 0; i < 3; ++i)
			m_pReactStatic[i]->Enable(false);
		m_pReactTitle->Enable(false);
		m_pReplyTitle->Enable(false);
		m_pReplyBackGround->Enable(false);
	}
	else
	{
		if (m_CurrentImprsnId < 0)
		{
			m_pImpressionBtn[0]->SetChecked(true);
			m_CurrentImprsnId = m_pImpressionBtn[0]->GetTabID();
		}

		m_pReplyEditBox->Enable(true);
		int i = 0;
		for (; i < MAX_GIFT_IMPRESSION_RADIO_BTN; ++i)
		{
			m_pImpressionIcon[i]->Enable(true);
			if (m_pImpressionBtn[i]->IsChecked())
				m_CurrentImprsnId = m_pImpressionBtn[i]->GetTabID();
			m_pImpressionBtn[i]->Enable(true);
		}

		for (i = 0; i < 3; ++i)
			m_pReactStatic[i]->Enable(true);
		m_pReactTitle->Enable(true);
		m_pReplyTitle->Enable(true);
		m_pReplyBackGround->Enable(true);
	}
}

bool CDnCashShopRecvGiftDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	if( !m_bShow )
	{
		return false;
	}

	if( !drag::IsValid() )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		switch( uMsg )
		{
		case WM_LBUTTONDOWN:
			{
				if (m_pInfoBaseBoard && m_pInfoBaseBoard->IsInside(fMouseX, fMouseY))
				{
					m_SlotUnit.Clear();
					m_pSenderMemoName->ClearText();
					m_pSenderMemoBox->ClearText();
					m_IndexSelected = -1;
#if defined(PRE_ADD_GIFT_RETURN)
					if( m_pNonAcceptBtn )
						m_pNonAcceptBtn->Enable(false);
#endif
					int i = 0;
					for (; i < (int)m_SenderInfoList.size(); ++i)
						m_SenderInfoList[i].pSelected->Show(false);

					for (i = 0; i < (int)m_SenderInfoList.size(); ++i)
					{
						if (m_SenderInfoList[i].IsEmpty() == false && m_SenderInfoList[i].IsInside(fMouseX, fMouseY))
						{
							const SCashShopRecvGiftBasicInfo* pInfo = GetCashShopTask().GetRecvGiftInfo(m_SenderInfoList[i].giftDBID);
							if (pInfo)
							{
								m_SlotUnit.Set(pInfo->sn, pInfo->itemId);
								m_SlotUnit.Show(true);

								SetReactionUI(GetCashShopTask().IsRecvGiftReplyAble(pInfo->payMethodCode));

#if defined(PRE_ADD_GIFT_RETURN)							
								if( m_pNonAcceptBtn && IsNonAccept(pInfo->payMethodCode) && !pInfo->giftReturn )								
									m_pNonAcceptBtn->Enable(true);
								m_pSenderMemoName->SetTextColor(textcolor::WHITE);
								
								if( pInfo->giftReturn ) // 반송된 선물
								{
									m_pSenderMemoName->SetTextColor(textcolor::RED);
									SetReactionUI(false);
								}
#endif
								std::wstring str;
								m_pSenderMemoName->SetText(pInfo->name.c_str());

								if (pInfo->payMethodCode == CDnCashShopTask::eRGT_QUEST)	// Quest
								{
									int nQuestIdx = _wtoi(pInfo->memo.c_str());
									if (nQuestIdx > 0)
									{
										Journal* pJournalData = g_DataManager.GetJournalData(nQuestIdx);
										if (pJournalData)
											m_pSenderMemoBox->SetText(pJournalData->wszQuestTitle.c_str());
									}
								}
								else
								{
#if defined(PRE_ADD_GIFT_RETURN)
									if( pInfo->giftReturn )
									{
										str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4595 ), pInfo->name.c_str()); // UISTRING %s님으로부터 반송된 선물입니다.
										m_pSenderMemoBox->SetText(str.c_str());
									}
									else
#endif

										m_pSenderMemoBox->SetText(pInfo->memo.c_str());
								}
							}							
							m_SenderInfoList[i].pSelected->Show(true);
							m_IndexSelected = i;
							return true;
						}
					}
				}
			}
		}
	}

	return CDnCustomDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}

#if defined(PRE_ADD_GIFT_RETURN)
bool CDnCashShopRecvGiftDlg::IsNonAccept(int nPayMethodCode)
{
	return nPayMethodCode >= 4 ? false : true;  // 4.DNA포인트 6.퀘스트, 7.미션, 8.레벨업
}
#endif

// CEtUICallback
void CDnCashShopRecvGiftDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
#if defined(PRE_ADD_GIFT_RETURN)
			if (m_IndexSelected >= 0 && m_IndexSelected < MAX_GIFT_RECV_INFO)
			{
				const SCashShopGiftInfoUnit& unit = m_SenderInfoList[m_IndexSelected];				
				GetCashShopTask().RequestCashShopGiftReturn(unit.giftDBID);
			}
#endif
		}
	}
}