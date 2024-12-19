#include "StdAfx.h"
#include "DnMailReadDlg.h"
#include "DnInterfaceString.h"
#include "DnItemTask.h"
#ifdef PRE_ADD_CADGE_CASH
#include "DnCashShopTask.h"
#include "DnMainMenuDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnCashShopDlg.h"
#endif // PRE_ADD_CADGE_CASH

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#include "DnItemSlotButton.h"
#include "DnInterface.h"

CDnMailReadDlg::CDnMailReadDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallBack )
{
	m_pStaticName		= NULL;
	m_pStaticTitle		= NULL;
	m_pStaticSendDay	= NULL;

	m_pTextBoxText		= NULL;

	m_pCoinG			= NULL;
	m_pCoinS			= NULL;
	m_pCoinB			= NULL;

	m_pButtonGetAttach	= NULL;
	m_pButtonReply		= NULL;
	m_pButtonDelete		= NULL;

	memset(m_AttachSlotTypes, 0, sizeof(m_AttachSlotTypes));
	m_pPremiumLetterPaper	= NULL;
	m_pNormalLetterPaper	= NULL;
	m_pNoticeLetterPaper	= NULL;
#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeLetterPaper		= NULL;
	m_pBuyButton			= NULL;
#endif // PRE_ADD_CADGE_CASH

	m_MailKind = MailType::NormalMail;

	//m_CurMailID			= -1;
	m_bLock				= false;
}

void CDnMailReadDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MailReadDlg.ui").c_str(), bShow);
}

void CDnMailReadDlg::InitialUpdate()
{
	m_pStaticName		= GetControl<CEtUIStatic>("ID_STATIC_NAME");
	m_pStaticTitle		= GetControl<CEtUIStatic>("ID_STATIC_TITLE");
	m_pStaticSendDay	= GetControl<CEtUIStatic>("ID_SENDDAY");
	m_pTextBoxText		= GetControl<CEtUITextBox>("ID_TEXTBOX");
	m_pPremiumLetterPaper = GetControl<CEtUIStatic>("ID_MAIL_PREPAGE");
	m_pNormalLetterPaper = GetControl<CEtUIStatic>("ID_MAIL_PAGE");
	m_pNoticeLetterPaper = GetControl<CEtUIStatic>("ID_MAIL_NOTICE");
#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeLetterPaper	= GetControl<CEtUIStatic>("ID_MAIL_ASKPAGE");
	m_pBuyButton		= GetControl<CEtUIButton>("ID_BT_ASK");
#endif // PRE_ADD_CADGE_CASH
	m_pCoinG			= GetControl<CEtUIStatic>("ID_GOLD");
	m_pCoinS			= GetControl<CEtUIStatic>("ID_SILVER");
	m_pCoinB			= GetControl<CEtUIStatic>("ID_BRONZE");

	m_pButtonReply		= GetControl<CEtUIButton>("ID_BUTTON_RE");
	m_pButtonDelete		= GetControl<CEtUIButton>("ID_BUTTON_DEL");
	m_pButtonGetAttach	= GetControl<CEtUIButton>("ID_BUTTON_GET");

	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (i == 2)
		{
			m_AttachSlotTypes[i] = eSLOT_NORMAL;
			continue;
		}

		m_AttachSlotTypes[i] = eSLOT_PREMIUM;
	}

	for (i = 0; i < MAILATTACHITEMMAX; i++)
	{
		std::string temp;
		temp = FormatA("ID_ATTACH_ITEM%d", i);
		CDnItemSlotButton *pItemSlotButton = GetControl<CDnItemSlotButton>(temp.c_str());
		pItemSlotButton->SetSlotType(ST_EXCHANGE);
		pItemSlotButton->ResetSlot();
		pItemSlotButton->Show(m_AttachSlotTypes[i] == eSLOT_NORMAL);
		m_ItemSlotList.push_back(pItemSlotButton);

		temp = FormatA("ID_ATTACH_BACK%d", i);
		CEtUIStatic* pBGStatic = GetControl<CEtUIStatic>(temp.c_str());
		pBGStatic->Show(m_AttachSlotTypes[i] == eSLOT_NORMAL);

		m_AttachSlotBGList.push_back(pBGStatic);
	}

#if defined(PRE_ADD_ENGLISH_STRING)
#else
	m_pTextBoxText->EnableWorkBreak(false);
#endif
}

bool CDnMailReadDlg::IsAttachEmpty() const
{
	if (m_MoneyCache.GetMoney() > 0)
	{
		return false;
	}

	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (m_ItemSlotList[i])
		{
			if (m_ItemSlotList[i]->IsEmptySlot() == false)
				return false;
		}
		else
		{
			_ASSERT(0);
			return true;
		}
	}

	return true;
}

void CDnMailReadDlg::EnableButtons(bool bEnable)
{
	if (m_pButtonGetAttach == NULL ||
		m_pButtonReply == NULL ||
		m_pButtonDelete == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (bEnable)
		m_pButtonGetAttach->Enable(!IsAttachEmpty());
	else
		m_pButtonGetAttach->Enable(false);

	m_pButtonReply->Enable(bEnable);
	m_pButtonDelete->Enable(bEnable);

	int i = 0;
	for (; i < MAILATTACHITEMMAX; i++)
	{
		if (i > int(m_ItemSlotList.size()))
		{
			_ASSERT(0);
			break;
		}

		CDnItemSlotButton* pSlotBtn = m_ItemSlotList[i];
		if (pSlotBtn)
			pSlotBtn->Enable(bEnable);
	}
}

void CDnMailReadDlg::Process(float fElapsedTime)
{
	if (IsLocked())
		return;

	if (m_pButtonGetAttach)
		m_pButtonGetAttach->Enable(!IsAttachEmpty());

	m_pButtonReply->Enable(m_MailKind == MailType::NormalMail);

	CEtUIDialog::Process(fElapsedTime);
}

int CDnMailReadDlg::GetAttachItemSlotIndex(INT64 serial) const
{
	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (m_ItemSlotList[i])
		{
			CDnItem* pItem = static_cast<CDnItem*>(m_ItemSlotList[i]->GetItem());
			if (pItem && pItem->GetSerialID() == serial)
				return m_ItemSlotList[i]->GetSlotIndex();
		}
	}

	return -1;
}

void CDnMailReadDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if (IsLocked())
		return;

	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_GET"))
		{
			if (GetMailID() != -1)
			{
				CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
				mailTask.RequestMailAttach(GetMailID());
			}
			else
			{
				_ASSERT(0);
			}
			return;
		}
		else if (IsCmdControl("ID_BUTTON_RE"))
		{
			std::wstring title = FormatW(_T("%s%s"), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1965), m_pStaticTitle->GetText());	// UISTRING : ´äÀå:
			GetInterface().OpenMailDialog(true, m_pStaticName->GetText(), title);
		}
		else if (IsCmdControl("ID_BUTTON_DEL"))
		{
			std::wstring str;
			str = FormatW(_T("%s\n%s"), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1960), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1961));
			GetInterface().MessageBox(str.c_str(), MB_YESNO, MESSAGEBOX_DEL_CONFIRM, this);
			return;
		}
#ifdef PRE_ADD_CADGE_CASH
		else if( IsCmdControl( "ID_BT_ASK" ) )
		{
			if( GetInterface().GetCashShopDlg() )
				GetInterface().GetCashShopDlg()->CadgeButtonClick();
			if( GetInterface().GetMainMenuDialog() )
				GetInterface().GetMainMenuDialog()->CloseMenuDialog();

			CDnLocalPlayerActor::LockInput( false );
			CDnCashShopTask::GetInstance().RequestCashShopOpen();
			return;
		}
#endif // PRE_ADD_CADGE_CASH
		if (strstr(pControl->GetControlName(), "ID_ATTACH_ITEM"))
		{
			CDnSlotButton* pDragButton = (CDnSlotButton*)drag::GetControl();
			CDnItemSlotButton* pPressedButton = (CDnItemSlotButton*)pControl;

			if (uMsg == WM_RBUTTONUP)
			{
				CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();

				if (pPressedButton)
				{
					CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
					if (pPressedItem)
					{
						int slotIdx = GetAttachItemSlotIndex(pPressedItem->GetSerialID());
						if (GetMailID() != -1 && (slotIdx >= 0 && slotIdx < MAILATTACHITEMMAX))
						{
							mailTask.RequestMailAttach(GetMailID(), pPressedItem->GetSerialID(), slotIdx);
						}
						else
						{
							_ASSERT(0);
						}
					}
				}

				return;
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMailReadDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	CEtUIDialog::Show(bShow);
}

void CDnMailReadDlg::SetCoin(const INT64& amount)
{
	m_MoneyCache.SetMoney(amount);

	m_pCoinG->SetInt64ToText(m_MoneyCache.GetG());
	m_pCoinS->SetInt64ToText(m_MoneyCache.GetS());
	m_pCoinB->SetInt64ToText(m_MoneyCache.GetB());
}

bool CDnMailReadDlg::IsNeedSingleSlot(bool bPremium) const
{
	return (bPremium == false && m_MailKind != MailType::MissionMail);
}

void CDnMailReadDlg::Update(const CDnTradeMail::SReadMailInfo& info)
{
	m_MailKind = info.mailKind;
	//m_CurMailID = info.nMailID;

	if (info.bNewFlag)
	{
		int iTitleUIStringID = _wtoi( info.strTitle.c_str() );
		int iContentUIStringID = _wtoi( info.strText.c_str() );

		if (info.mailKind == MailType::AdminMail)
		{
			int iSenderUIStringID = _wtoi( info.strSender.c_str() );
			if (iSenderUIStringID == 0)
				m_pStaticName->SetText(info.strSender);
			else
			{
				const wchar_t* wszSenderUI = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iSenderUIStringID);
				m_pStaticName->SetTextWithEllipsis( wszSenderUI, L"...");
			}
		}
		else
			m_pStaticName->SetText(info.strSender);

		m_pStaticTitle->SetText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iTitleUIStringID) );

		const wchar_t* pContentStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, iContentUIStringID);
		m_pTextBoxText->ClearText();
		if (info.bPremium)
			m_pTextBoxText->SetText(pContentStr, textcolor::MAIL_PREMIUM_READ);
		else
			m_pTextBoxText->SetText(pContentStr, textcolor::MAIL_READ);
	}
	else
	{
		m_pStaticName->SetText(info.strSender);
		m_pStaticTitle->SetText(info.strTitle);
		m_pTextBoxText->ClearText();

		if (info.bPremium)
			m_pTextBoxText->SetText(info.strText.c_str(), textcolor::MAIL_PREMIUM_READ);
		else
			m_pTextBoxText->SetText(info.strText.c_str(), textcolor::MAIL_READ);
	}


	std::wstring str;
	CommonUtil::GetDateString(CommonUtil::DATESTR_FULL, str, info.date);

	m_pStaticSendDay->SetText(str.c_str());

	SetCoin(info.money);

	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (m_ItemSlotList[i])
			m_ItemSlotList[i]->ResetSlot();
		else
			_ASSERT(0);
	}

	if (info.attachItems.size() > MAILATTACHITEMMAX)
	{
		_ASSERT(0);
		return;
	}

	bool bAttachItem = false;

	SetPremium(info.bPremium);
	if (IsNeedSingleSlot(info.bPremium))
	{
		const TItemInfo& attachItem = info.attachItems[0];
		if (attachItem.Item.nItemID > 0)
		{
			TItemInfo infoForCreate = attachItem;
			CDnItem* pNewItem = GetItemTask().CreateItem( infoForCreate );
			CDnItemSlotButton* pSlot = m_ItemSlotList[MAIL_ATTACH_SINGLE_SLOT_INDEX];
			pSlot->SetItem(pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
			pSlot->SetSlotIndex(infoForCreate.cSlotIndex);
			bAttachItem = true;
		}
	}
	else
	{
		std::vector<TItemInfo>::const_iterator iter = info.attachItems.begin();
		for (; iter < info.attachItems.end(); ++iter)
		{
			const TItemInfo& attachItem = (*iter);
			if (attachItem.Item.nItemID > 0 && (attachItem.cSlotIndex >= 0 && attachItem.cSlotIndex < MAILATTACHITEMMAX))
			{
				TItemInfo infoForCreate = attachItem;
				CDnItem* pNewItem = GetItemTask().CreateItem( infoForCreate );
				CDnItemSlotButton* pSlot = m_ItemSlotList[attachItem.cSlotIndex];
				pSlot->SetItem(pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
				pSlot->SetSlotIndex(infoForCreate.cSlotIndex);
				bAttachItem = true;
			}
		}
	}

#ifdef PRE_ADD_CADGE_CASH
	if( m_MailKind == MailType::NormalMail )
	{
		m_pNoticeLetterPaper->Show( false );
		m_pCadgeLetterPaper->Show( false );
		m_pBuyButton->Show( false );
	}
	else if( m_MailKind == MailType::Cadge )
	{
		m_pPremiumLetterPaper->Show( false );
		m_pNormalLetterPaper->Show( false );
		m_pNoticeLetterPaper->Show( false );
		m_pCadgeLetterPaper->Show( true );
		m_pBuyButton->Show( true );

		if( info.bCadgeComplete )
		{
			m_pBuyButton->Enable( false );
			m_pBuyButton->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4915 ) );
		}
		else
		{
			m_pBuyButton->Enable( true );
			m_pBuyButton->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4914 ) );
		}
	}
	else
	{
		m_pPremiumLetterPaper->Show( false );
		m_pNormalLetterPaper->Show( false );
		m_pCadgeLetterPaper->Show( false );
		m_pBuyButton->Show( false );
		m_pNoticeLetterPaper->Show( true );
	}
#else // PRE_ADD_CADGE_CASH
	if (m_MailKind != MailType::NormalMail)
	{
		m_pPremiumLetterPaper->Show(false);
		m_pNormalLetterPaper->Show(false);
		m_pNoticeLetterPaper->Show(true);
	}
	else
	{
		m_pNoticeLetterPaper->Show(false);
	}
#endif // PRE_ADD_CADGE_CASH

	m_pButtonReply->Enable(m_MailKind == MailType::NormalMail);
	m_pButtonGetAttach->Enable(bAttachItem || info.money > 0);
}

int CDnMailReadDlg::GetMailIndex() const
{
	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	int idx = mailTask.GetMailBoxIndex(GetMailID());
	if (idx <= 0 || idx > mailTask.GetTotalMailCount())
	{
		_ASSERT(0);
		return -1;
	}

	return idx;
}

void CDnMailReadDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	if (IsLocked())
		return;

	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (nID == MESSAGEBOX_DEL_CONFIRM)
		{
			if (IsCmdControl("ID_YES"))
			{
				int deleteIds[MAILPAGEMAX];
				ZeroMemory(deleteIds, sizeof(deleteIds));
				deleteIds[0] = GetMailID();

				GetTradeTask().GetTradeMail().RequestMailDelete(deleteIds, 1);
			}
		}
	}
}

void CDnMailReadDlg::Clear()
{
	m_pStaticName->ClearText();
	m_pStaticTitle->ClearText();
	m_pStaticSendDay->ClearText();

	m_pTextBoxText->ClearText();

	ClearAttach();

	//m_CurMailID	= -1;
}

void CDnMailReadDlg::ClearAttach()
{
	m_pCoinG->ClearText();
	m_pCoinS->ClearText();
	m_pCoinB->ClearText();
	m_MoneyCache.SetMoney(0);

	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (m_ItemSlotList[i])
			m_ItemSlotList[i]->ResetSlot();
		else
			_ASSERT(0);
	}
}

int CDnMailReadDlg::RemoveAttachItem(INT64 attachItemSerial)
{
	int i = 0;
	int restCount = 0;

	for (; i < MAILATTACHITEMMAX; ++i)
	{
		CDnItem* pItem = static_cast<CDnItem*>(m_ItemSlotList[i]->GetItem());
		if (pItem && pItem->GetSerialID() == attachItemSerial)
		{
			m_ItemSlotList[i]->ResetSlot();
		}
		else
		{
			if (m_ItemSlotList[i]->IsEmptySlot() == false)
				restCount++;
		}
	}

	return restCount;
}

int	CDnMailReadDlg::GetMailID() const
{
	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	return mailTask.GetCurReadMailID(); 
}

bool CDnMailReadDlg::IsUpdated() const
{
	return (GetMailID() != -1);
}

void CDnMailReadDlg::SetPremium(bool bSet)
{
	m_pPremiumLetterPaper->Show(bSet);
	m_pNormalLetterPaper->Show(!bSet);

	if (IsNeedSingleSlot(bSet))
		ShowAttachSlot(eSLOT_NORMAL);
	else
		ShowAttachSlot(eSLOT_PREMIUM);
}

void CDnMailReadDlg::ShowAttachSlot(eAttachSlotType type)
{
	if ((int)m_ItemSlotList.size() != MAILATTACHITEMMAX || 
		(int)m_AttachSlotBGList.size() != MAILATTACHITEMMAX ||
		_countof(m_AttachSlotTypes) != MAILATTACHITEMMAX)
	{
		_ASSERT(0);
		return;
	}

	int idx = 0;
	for (; idx < MAILATTACHITEMMAX; ++idx)
	{
		CDnItemSlotButton* pBtn = m_ItemSlotList[idx];
		CEtUIStatic* pBG = m_AttachSlotBGList[idx];
		bool bShow = false;
		if (type == eSLOT_PREMIUM)
			bShow = (m_AttachSlotTypes[idx] == eSLOT_PREMIUM || m_AttachSlotTypes[idx] == eSLOT_NORMAL);
		else if (type == eSLOT_NORMAL)
			bShow = (m_AttachSlotTypes[idx] == eSLOT_NORMAL);

		if (pBtn)
			pBtn->Show(bShow);
		if (pBG)
			pBG->Show(bShow);
	}
}
