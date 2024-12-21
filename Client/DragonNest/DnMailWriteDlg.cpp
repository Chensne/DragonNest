#include "StdAfx.h"
#include "DnMailWriteDlg.h"
#include "DnItem.h"
#include "DnTradeTask.h"
#include "DnMainMenuDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMoneyInputDlg.h"
#include "DnMailSendConfirmDlg.h"
#include "DnFriendTask.h"
#include "DnInterfaceString.h"
#include "DnStoreConfirmExDlg.h"
#include "DnRestraintTask.h"
#include "DnMainMenuDlg.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//#define MAX_LINE_EDITBOX_TEXT 256

#include "DnItemSlotButton.h"

CDnMailWriteDlg::CDnMailWriteDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallBack )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallBack )
{
	m_pConfirmDlg		= NULL;
	m_pMoneyInputDlg	= NULL;

	memset(m_AttachSlotTypes, 0, sizeof(m_AttachSlotTypes));
	m_pEnableSendCountIcon = NULL;
	m_PremiumNoteStatic = NULL;
	m_PremiumStatic = NULL;

	m_PremiumBtn = NULL;
	m_pButtonMoneyDel = NULL;
	m_pButtonMoneyInput = NULL;
	m_pButtonSend = NULL;
	m_pCoinB = NULL;
	m_pCoinG = NULL;
	m_pCoinS = NULL;
	m_pCoinTaxB = NULL;
	m_pCoinTaxG = NULL;
	m_pCoinTaxS = NULL;
	m_pEditBoxName = NULL;
	m_pEditBoxTitle = NULL;
	m_pEnableSendCountStatic = NULL;
	m_pFriendComboBox = NULL;
	m_pLineEditBox = NULL;
	m_pNormalLetterPaper = NULL;
	m_pPremiumLetterPaper = NULL;
	m_pPremiumLineEditBox = NULL;
	m_pSplitConfirmExDlg = NULL;

	m_bLock = false;
}

CDnMailWriteDlg::~CDnMailWriteDlg()
{
	SAFE_DELETE(m_pMoneyInputDlg);
	SAFE_DELETE(m_pConfirmDlg);
	SAFE_DELETE(m_pSplitConfirmExDlg);
}

void CDnMailWriteDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MailWriteDlg.ui").c_str(), bShow);
}

void CDnMailWriteDlg::InitialUpdate()
{
	m_pButtonSend		= GetControl<CEtUIButton>("ID_BUTTON_SEND");
	m_pButtonMoneyInput = GetControl<CEtUIButton>("ID_BUTTON_MONEY");
	m_pButtonMoneyDel	= GetControl<CEtUIButton>("ID_BUTTON_REMOVE");

	m_pEditBoxName		= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pEditBoxTitle		= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_TITLE");
	m_pLineEditBox		= GetControl<CEtUILineIMEEditBox>("ID_EDITBOX");
	m_pLineEditBox->SetMaxEditLength(MAILTEXTLENMAX);
	m_pPremiumLineEditBox = GetControl<CEtUILineIMEEditBox>("ID_EDITBOX_PREMIUM");
	m_pPremiumLineEditBox->SetMaxEditLength(MAILTEXTLENMAX);

	m_pCoinG			= GetControl<CEtUIStatic>("ID_GOLD");
	m_pCoinS			= GetControl<CEtUIStatic>("ID_SILVER");
	m_pCoinB			= GetControl<CEtUIStatic>("ID_BRONZE");

	m_pCoinTaxG			= GetControl<CEtUIStatic>("ID_TAX_GOLD");
	m_pCoinTaxS			= GetControl<CEtUIStatic>("ID_TAX_SILVER");
	m_pCoinTaxB			= GetControl<CEtUIStatic>("ID_TAX_BRONZE");

	m_PremiumBtn		= GetControl<CEtUICheckBox>("ID_CHECKBOX_PREMIUM");
	m_PremiumBtn->Enable(false);

	m_PremiumStatic		= GetControl<CEtUIStatic>("ID_STATIC_PREMIUM");
	m_PremiumNoteStatic = GetControl<CEtUIStatic>("ID_STATIC3");

	m_pEnableSendCountStatic = GetControl<CEtUIStatic>("ID_STATIC_ENABLESENDCOUNT");
	m_pEnableSendCountIcon = GetControl<CEtUIStatic>("ID_STATIC4");
	m_pNormalLetterPaper = GetControl<CEtUIStatic>("ID_MAIL_PAGE");
	m_pPremiumLetterPaper = GetControl<CEtUIStatic>("ID_MAIL_PREPAGE");

	m_pFriendComboBox	= GetControl<CEtUIComboBox>("ID_COMBOBOX_NAME");
	m_pFriendComboBox->SetEditMode(true);

	m_pMoneyInputDlg	= new CDnMoneyInputDlg(UI_TYPE_MODAL, NULL, MONEY_INPUT_DIALOG, this);
	m_pMoneyInputDlg->Initialize(false);

	m_pConfirmDlg		= new CDnMailSendConfirmDlg(UI_TYPE_MODAL, NULL, SEND_CONFIRM_DIALOG, this);
	m_pConfirmDlg->Initialize(false);

	m_pSplitConfirmExDlg = new CDnStoreConfirmExDlg( UI_TYPE_MODAL, NULL, ITEM_ATTACH_CONFIRM_DIALOG, this );
	m_pSplitConfirmExDlg->Initialize( false );

	m_vecSlotButton.resize(MAILATTACHITEMMAX);

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

	for (i = 0; i < MAILATTACHITEMMAX; ++i)
	{
		std::string temp;
		temp = FormatA("ID_ATTACH_ITEM%d", i);
		CDnItemSlotButton *pItemSlotButton = GetControl<CDnItemSlotButton>(temp.c_str());
		pItemSlotButton->SetSlotType(ST_EXCHANGE);
		pItemSlotButton->ResetSlot();
		pItemSlotButton->Show(m_AttachSlotTypes[i] == eSLOT_NORMAL);
		m_ItemSlotList.push_back(pItemSlotButton);

		m_vecSlotButton[i] = NULL;

		temp = FormatA("ID_ATTACH_BACK%d", i);
		CEtUIStatic* pBGStatic = GetControl<CEtUIStatic>(temp.c_str());
		pBGStatic->Show(m_AttachSlotTypes[i] == eSLOT_NORMAL);

		m_AttachSlotBGList.push_back(pBGStatic);
	}

	m_vecSlotButton.resize(MAILATTACHITEMMAX);
}

void CDnMailWriteDlg::SetName( const wchar_t *szName )
{
	_ASSERT(szName != NULL);
	m_pEditBoxName->SetText(szName);
}

void CDnMailWriteDlg::SetTitle(const wchar_t* szTitle)
{
	_ASSERT(szTitle != NULL);
	m_pEditBoxTitle->SetTextWithEllipsis(szTitle, _T("..."));
}

MONEY_TYPE CDnMailWriteDlg::CalcAttachAmount() const
{
	MONEY_TYPE amount = m_MoneyCache.GetMoney();

	std::vector<CDnItemSlotButton*>::const_iterator iter = m_ItemSlotList.begin();
	for (; iter != m_ItemSlotList.end(); ++iter)
	{
		const CDnItemSlotButton* curSlot = *iter;
		if (curSlot)
		{
			const CDnItem *pItem = static_cast<const CDnItem*>(curSlot->GetItem());
			if( pItem )
				amount += (pItem->GetItemAmount() * curSlot->GetRenderCount());
		}
	}

	return amount;
}

void CDnMailWriteDlg::SetFriendComboBox()
{
	int i = 0;
	const CDnFriendTask::FRIEND_MAP& friendList = GetFriendTask().GetFriendList();
	m_pFriendComboBox->RemoveAllItems();

	CDnFriendTask::FRIEND_MAP::const_iterator iter = friendList.begin();
	for (; iter != friendList.end(); ++iter)
	{
		CDnFriendTask::FRIENDINFO_MAP *pFriendInfo = iter->second;
		if( pFriendInfo == NULL)
			continue;

		CDnFriendTask::FRIENDINFO_MAP::const_iterator fmIter = pFriendInfo->begin();
		for (; fmIter != pFriendInfo->end(); ++fmIter)
		{
			const CDnFriendTask::SFriendInfo& info = fmIter->second;
			m_pFriendComboBox->AddItem(info.wszFriendName.c_str(), NULL, i);
#ifdef PRE_PARTY_DB
			m_AutoCompleteMgr.AddTextUnit(info.wszFriendName[0], info.wszFriendName);
#else
			m_FriendNameList.insert(make_pair(info.wszFriendName[0], info.wszFriendName));
#endif
			++i;
		}
	}
	
	m_pFriendComboBox->ClearSelectedItem();
}

TAX_TYPE CDnMailWriteDlg::CalcTax()
{
	return (CommonUtil::CalcTax(TAX_POSTDEFAULT, CDnActor::s_hLocalActor->GetLevel(), CalcAttachAmount()) + 
		CommonUtil::CalcTax(TAX_POST, CDnActor::s_hLocalActor->GetLevel(), CalcAttachAmount()));
}

void CDnMailWriteDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		TAX_TYPE totalTax = CalcTax();
		SetTax(totalTax);
		SetFriendComboBox();

		SetPremium(false);

		CheckPremiumMail();
	}

	CEtUIDialog::Show(bShow);
}

void CDnMailWriteDlg::OnAddEditBoxString(const std::wstring& strName)
{
	int curEditBoxCaretPos = m_pEditBoxName->GetCaretPos();
	//m_pEditBoxName->GetIMECompositionString(strName);
	if (strName.size() < 1) return;

#ifdef PRE_PARTY_DB
	std::wstring almost = L"";
	m_AutoCompleteMgr.GetAlmostMatchingString(almost, strName);
#else
	std::pair<std::multimap<wchar_t, std::wstring>::iterator, std::multimap<wchar_t, std::wstring>::iterator> range;
	range = m_FriendNameList.equal_range(strName[0]);

	std::wstring almost = _T("");
	std::wstring::size_type longest = 0;
	std::multimap<wchar_t, std::wstring>::iterator it = range.first;
	for (; it != range.second; ++it)
	{
		std::wstring& friendName = (*it).second;
		if (friendName == strName)
			return;
		std::wstring::size_type size = friendName.find(strName);
		if (size != std::wstring::npos && size >= longest)
			almost = friendName;
	}
#endif

	if (almost != _T(""))
	{
		m_pEditBoxName->ClearText();
		m_pEditBoxName->SetText(almost.c_str());
		m_pEditBoxName->SetSelection(m_pEditBoxName->GetTextLength(), curEditBoxCaretPos);
		m_AutoCompleteTextCache = almost;
		//m_pEditBoxName->CancelIMEComposition();
	}
	else
	{
		m_AutoCompleteTextCache = _T("");
	}
}

bool CDnMailWriteDlg::IsDuplicateAttachItem(INT64 serialItemId) const
{
	std::vector<CDnItemSlotButton*>::const_iterator iter = m_ItemSlotList.begin();
	for (; iter != m_ItemSlotList.end(); ++iter)
	{
		const CDnItemSlotButton* pSlot = *(iter);
		if (pSlot && pSlot->IsEmptySlot() == false)
		{
			const CDnItem* pCurItem = static_cast<const CDnItem*>(pSlot->GetItem());
			if (pCurItem)
			{
				if (pCurItem->GetSerialID() == serialItemId)
					return true;
			}
		}
	}

	return false;
}

void CDnMailWriteDlg::GetEmptyAttachSlot(CDnItemSlotButton** ppEmptySlot)
{
	if( m_PremiumBtn->IsChecked() )
	{
		std::vector<CDnItemSlotButton*>::iterator iter = m_ItemSlotList.begin();
		for (; iter != m_ItemSlotList.end(); ++iter)
		{
			CDnItemSlotButton* pSlot = *(iter);
			if (pSlot)
			{
				if (pSlot->IsEmptySlot() == false || pSlot->IsShow() == false)
				{
					continue;
				}
				else
				{
					*ppEmptySlot = pSlot;
					break;
				}
			}
		}
	}
	else
	{
		std::vector<CDnItemSlotButton*>::iterator iter = m_ItemSlotList.begin();
		for (; iter != m_ItemSlotList.end(); ++iter)
		{
			CDnItemSlotButton* pSlot = *(iter);
			if (pSlot)
			{
				if (pSlot->IsShow() == true)
				{
					*ppEmptySlot = pSlot;
					break;
				}
			}
		}
	}
}

bool CDnMailWriteDlg::AddAutoAttachItem(CDnSlotButton* pFromSlot, bool bItemSplit)
{
	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		return false;
	}

	CDnItem* pItem = static_cast<CDnItem*>(pInvenItem);

#ifdef PRE_ADD_ONESTOP_TRADECHECK
	if (pItem && CDnTradeTask::IsActive() && CDnTradeTask::GetInstance().IsTradable(CDnTradeTask::eTRADECHECK_MAIL, *pItem) == false)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK );	// UISTRING : 거래가 불가능한 아이템입니다
		return false;
	}
#endif

	if (IsAttachableItem(pItem) == false)
	{
		return false;
	}

	if (pItem->GetOverlapCount() > 1 && bItemSplit)
	{
		m_pSplitConfirmExDlg->SetItem(pFromSlot, CDnInterface::MAIL_WRITE);
		m_pSplitConfirmExDlg->Show(true);

		return true;
	}

	CDnItemSlotButton* pEmptySlot = NULL;
	GetEmptyAttachSlot(&pEmptySlot);
	if (pEmptySlot == NULL)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1971), MB_OK); // UISTRING : 빈 슬롯이 없어 아이템을 첨부할 수 없습니다
		return false;
	}

	return AddAttachItem(pFromSlot, pEmptySlot, CDnSlotButton::ITEM_ORIGINAL_COUNT);
}

bool CDnMailWriteDlg::IsAttachableItem(CDnItem* pItem) const
{
	if (IsDuplicateAttachItem(pItem->GetSerialID()))
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1970 ), MB_OK); // UISTRING : 이미 첨부한 아이템입니다
		return false;
	}

	if (pItem->IsCashItem())
	{
		if (pItem->IsEternityItem() == false)
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4082 ), MB_OK ); // UISTRING : 기간제 아이템은 거래할 수 없습니다
			return false;
		}

		if (pItem->GetReversion() == CDnItem::Trade && pItem->GetSealCount() > 0)
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3627 ), MB_OK ); // UISTRING : 해당 무기한 아이템은 거래소 등록만 가능합니다.
			return false;
		}

		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다.
		return false;
	}
	else
	{
		if (pItem->GetReversion() == CDnItem::Belong && pItem->IsSoulbBound())
		{
			eItemTypeEnum type = pItem->GetItemType();
			if (type == ITEMTYPE_WEAPON || 
				type == ITEMTYPE_PARTS ||
				type == ITEMTYPE_GLYPH ||
				type == ITEMTYPE_POTENTIAL_JEWEL)
			{
				if (pItem->GetSealCount() > 0)
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3626 ), MB_OK ); // UISTRING : 재봉인 가능한 귀속 아이템을 거래 하려면 봉인의 인장으로 봉인해야 합니다
					return false;
				}
			}

			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
			return false;
		}
		else if (pItem->GetReversion() == CDnItem::GuildBelong)
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
			return false;
		}
	}

	eItemTypeEnum itemType = pItem->GetItemType();
	if (itemType == ITEMTYPE_QUEST)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1978 ), MB_OK ); // UISTRING : 퀘스트 아이템은 첨부할 수 없습니다.
		return false;
	}

	return true;
}

bool CDnMailWriteDlg::AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot, int itemCount)
{
	if (pFromSlot == NULL || pAttachSlot == NULL)
	{
		_ASSERT(0);
		return false;
	}

	pFromSlot->EnableSplitMode(itemCount);
	MIInventoryItem* pInvenItem = pFromSlot->GetItem();
	if (pInvenItem == NULL || pInvenItem->GetType() != MIInventoryItem::Item)
	{
		_ASSERT(0);
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	CDnItem *pItem = static_cast<CDnItem*>(pFromSlot->GetItem());

	if (IsAttachableItem(pItem) == false)
	{
		pFromSlot->DisableSplitMode(true);
		return false;
	}

	if (pAttachSlot->GetItem() != NULL)
		RemoveAttachItem(pAttachSlot, true);

	int idx = GetSlotIndex(pAttachSlot);
 
 	m_vecSlotButton[idx] = pFromSlot;
 	m_vecSlotButton[idx]->SetRegist(true);

	pAttachSlot->SetItem(pItem, itemCount);

	SetTax(CalcTax());
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	return true;
}

bool CDnMailWriteDlg::RestoreFromSlot(int idx, bool bAttachCancel)
{
	if (idx < 0 || idx >= MAILATTACHITEMMAX)
	{
		_ASSERT(0);
		return false;
	}

	if (m_vecSlotButton[idx] != NULL)
	{
		m_vecSlotButton[idx]->SetRegist(false);
		m_vecSlotButton[idx]->DisableSplitMode(bAttachCancel);
		m_vecSlotButton[idx] = NULL;
	}

	return true;
}

bool CDnMailWriteDlg::RestoreFromSlot(CDnItemSlotButton* pAttachSlot, bool bAttachCancel)
{
	int idx = GetSlotIndex(pAttachSlot);
	return RestoreFromSlot(idx, bAttachCancel);
}

bool CDnMailWriteDlg::RemoveAttachItem(CDnItemSlotButton* pAttachSlot, bool bAttachCancel)
{
	if (pAttachSlot == NULL ||
		RestoreFromSlot(pAttachSlot, bAttachCancel) == false)
	{
		_ASSERT(0);
		return false;
	}

	pAttachSlot->ResetSlot();
	SetTax(CalcTax());
	return true;
}

bool CDnMailWriteDlg::RemoveAttachItem_All(bool bAttachCancel)
{
	int i = 0;
	for (; i < MAILATTACHITEMMAX; i++)
	{
		RestoreFromSlot(i, bAttachCancel);
		m_ItemSlotList[i]->ResetSlot();
	}

	SetTax(CalcTax());
	return true;
}

int CDnMailWriteDlg::GetSlotIndex(const CDnItemSlotButton* pSlot) const
{
	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		if (m_ItemSlotList[i] == pSlot)
			return i;
	}
	
	return -1;
}

void CDnMailWriteDlg::CopyMailContentsToBuffer(LPWSTR pBuffer, UINT bufferCount)
{
	if (m_PremiumBtn->IsChecked())
		m_pPremiumLineEditBox->GetTextCopy(pBuffer, bufferCount);
	else
		m_pLineEditBox->GetTextCopy(pBuffer, bufferCount);
}

void CDnMailWriteDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	if (IsLocked())
		return;

	SetCmdControlName(pControl->GetControlName());

	if (IsCmdControl("ID_IMEEDITBOX_NAME"))
	{
		if( nCommand == EVENT_EDITBOX_IME_CHANGE )
		{
			std::wstring curText = m_pEditBoxName->GetText();
			std::wstring composeStr;
			m_pEditBoxName->GetIMECompositionString(composeStr);
			curText += composeStr;
			OnAddEditBoxString(curText);
			return;
		}
		else if (nCommand == EVENT_EDITBOX_CHANGE)
		{
			WCHAR ch = (WCHAR)uMsg;
			if (CommonUtil::IsCtrlChar(ch) == false)
			{
				std::wstring curText = m_pEditBoxName->GetText();
				OnAddEditBoxString(curText);
			}
			return;
		}
		else if (nCommand == EVENT_EDITBOX_ESCAPE)
		{
			CDnMainMenuDlg* pDlg = GetInterface().GetMainMenuDialog();
			if (pDlg)
				pDlg->CloseMenuDialog();
			return;
		}
		else if (nCommand != EVENT_EDITBOX_CHANGE)
		{
			if (nCommand == EVENT_EDITBOX_FOCUS || nCommand == EVENT_EDITBOX_RELEASEFOCUS)
			{
				CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
				return;
			}

			if (m_AutoCompleteTextCache != _T(""))
			{
				m_pEditBoxName->ClearText();
				m_pEditBoxName->SetText(m_AutoCompleteTextCache.c_str());
				m_AutoCompleteTextCache = _T("");
			}
			return;
		}
	}

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_MONEY"))
		{
			m_pMoneyInputDlg->Show(true);
			return;
		}

		if (IsCmdControl("ID_BUTTON_REMOVE"))
		{
			m_pCoinG->ClearText();
			m_pCoinS->ClearText();
			m_pCoinB->ClearText();

			m_MoneyCache.Clear();
			m_TaxCache.Clear();

			RemoveAttachItem_All(true);
			return;
		}

		if( IsCmdControl( "ID_BUTTON_SEND" ) )
		{
			std::wstring str = m_pEditBoxName->GetText();
			if( str.empty() )
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1969), MB_OK, ERROR_NO_NAME_MSGBOX, this);	//	UISTRING : 받는 사람을 입력해 주세요.
				return;
			}

			if (!__wcsicmp_l(str.c_str(), CDnActor::s_hLocalActor->GetName()))
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1984), MB_OK, ERROR_NO_TITLE_MSGBOX, this);	//	UISTRING : 자신에게는 우편을 보낼 수 없습니다
				return;
			}

			// 거래 제재 검사
			if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
				return;

			str = m_pEditBoxTitle->GetText();
			if( str.empty() )
			{
				if (IsAnyAttach())
				{
					m_pEditBoxTitle->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1985));	// UISTRING : 첨부된 우편
				}
				else
				{
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1927), MB_OK, ERROR_NO_TITLE_MSGBOX, this);
					return;
				}
			}

			if (min( GetItemTask().GetCoin(), 1000000000 ) < (UINT)m_MoneyCache.GetMoney())
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1929), MB_OK);
				return;
			}

			if (GetItemTask().GetCoin() < (UINT)m_TaxCache.GetMoney())
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1966), MB_OK);	// UISTRING : 수수료가 부족하여 편지를 보낼 수 없습니다
				return;
			}

			bool bTitleProhibition		= DN_INTERFACE::UTIL::CheckChat( str );

			wchar_t strTemp[MAILTEXTLENMAX]={0};
			CopyMailContentsToBuffer(strTemp, MAILTEXTLENMAX);
			str = strTemp;
			bool bContentProhibition	= DN_INTERFACE::UTIL::CheckChat( str );

			if (bTitleProhibition || bContentProhibition)
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1977), MB_OK);	// UISTRING : 제목이나 내용에 금칙어가 있어서 보낼 수 없습니다
				return;
			}

			if (m_PremiumBtn->IsChecked() == false && GetTradeTask().GetTradeMail().GetDaySendEnableCount() <= 0)
			{
				GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1991), MB_OK);	// UISTRING : 오늘 보낼 수 있는 횟수가 다 되어 메일을 보낼 수 없습니다
				return;
			}

			m_pConfirmDlg->SetTax(m_TaxCache.GetMoney());
			m_pConfirmDlg->Show(true, m_PremiumBtn->IsChecked(), m_pEditBoxName->GetText());
		}

		if (strstr(pControl->GetControlName(), "ID_ATTACH_ITEM"))
		{
			CDnSlotButton* pDragButton = (CDnSlotButton*)drag::GetControl();
			CDnItemSlotButton* pPressedButton = (CDnItemSlotButton*)pControl;

			if (uMsg == WM_RBUTTONUP)
			{
				RemoveAttachItem(pPressedButton, true);
				return;
			}

			if (pDragButton)
			{
				if (pDragButton != pPressedButton)
				{
					//pDragButton->DisableSplitMode(false);
					if (AddAttachItem(pDragButton, pPressedButton, CDnSlotButton::ITEM_ORIGINAL_COUNT) == false)
						drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();
				}
			}
		}
	}
	else if ((nCommand == EVENT_COMBOBOX_SELECTION_CHANGED) && bTriggeredByUser)
	{
		if (IsCmdControl("ID_COMBOBOX_NAME") && m_pFriendComboBox->IsOpenedDropDownBox() == false)
		{
			SComboBoxItem* pItem = m_pFriendComboBox->GetSelectedItem();
			if (pItem)
			{
				m_pEditBoxName->ClearText();
				m_pEditBoxName->SetText(pItem->strText);
			}
		}
	}
	else if( (nCommand == EVENT_CHECKBOX_CHANGED) && bTriggeredByUser )
	{
		if (IsCmdControl("ID_CHECKBOX_PREMIUM"))
			SetPremium(((CEtUICheckBox*)pControl)->IsChecked());
	}
	else if (nCommand == EVENT_EDITBOX_ESCAPE)
	{
		CDnMainMenuDlg* pDlg = GetInterface().GetMainMenuDialog();
		if (pDlg)
			pDlg->CloseMenuDialog();
		return;
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMailWriteDlg::ShowAttachSlot(eAttachSlotType type)
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
		{
			if (bShow == false)
				RemoveAttachItem(pBtn, true);

			pBtn->Show(bShow);
		}

		if (pBG)
			pBG->Show(bShow);
	}
}

void CDnMailWriteDlg::Process( float fElapsedTime )
{
	m_pButtonMoneyDel->Enable(IsAnyAttach());

	CEtUIDialog::Process(fElapsedTime);
}

void CDnMailWriteDlg::Clear(bool bAttachCancel)
{
	m_pEditBoxName->ClearText();
	m_pEditBoxTitle->ClearText();
	m_pLineEditBox->ClearText();
	m_pPremiumLineEditBox->ClearText();

	m_pCoinG->ClearText();
	m_pCoinS->ClearText();
	m_pCoinB->ClearText();

	m_pCoinTaxG->ClearText();
	m_pCoinTaxS->ClearText();
	m_pCoinTaxB->ClearText();

	m_MoneyCache.Clear();
	m_TaxCache.Clear();

	RemoveAttachItem_All(bAttachCancel);

	CheckPremiumMail();
}

void CDnMailWriteDlg::SetTax(const TAX_TYPE& amount)
{
	m_TaxCache.SetMoney(amount);

	m_pCoinTaxG->SetIntToText(m_TaxCache.GetG());
	m_pCoinTaxS->SetIntToText(m_TaxCache.GetS());
	m_pCoinTaxB->SetIntToText(m_TaxCache.GetB());
}

void CDnMailWriteDlg::SetCoin(const INT64& amount)
{
	INT64 nCoin = GetItemTask().GetCoin();
	if (nCoin < m_MoneyCache.GetMoney())
		m_MoneyCache.SetMoney(nCoin);
	else
		m_MoneyCache.SetMoney(amount);

	m_pCoinG->SetInt64ToText(m_MoneyCache.GetG());
	m_pCoinS->SetInt64ToText(m_MoneyCache.GetS());
	m_pCoinB->SetInt64ToText(m_MoneyCache.GetB());
}

void CDnMailWriteDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (nID == MONEY_INPUT_DIALOG)
		{
			if( IsCmdControl( "ID_BUTTON_OK" ) )
			{
				INT64 nCoin = min( m_pMoneyInputDlg->GetMoney(), 1000000000 );
				SetCoin( nCoin );
				SetTax(CalcTax());
			}

			m_pMoneyInputDlg->Show( false );
			return;
		}
		else if (nID == SEND_CONFIRM_DIALOG)
		{
			if (IsCmdControl("ID_SEND"))
			{
				CDnTradeMail::SSendMailInfo sendMailInfo;

				wchar_t strTemp[MAILTEXTLENMAX]={0};
				CopyMailContentsToBuffer( strTemp, MAILTEXTLENMAX );
				sendMailInfo.strText.assign( strTemp, MAILTEXTLENMAX-1 );

				sendMailInfo.strReceiver = m_pEditBoxName->GetText();
				sendMailInfo.strTitle = m_pEditBoxTitle->GetText();
				
				// #82082
				int nLen = (int)sendMailInfo.strReceiver.length();
				if ( nLen <= 0 || nLen == 1 || sendMailInfo.strReceiver[nLen - 1] == _T(' ') || sendMailInfo.strReceiver[0] == _T(' ') )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100003), MB_OK ); // UISTRING : 등록되지 않은 아이디입니다.
					return;
				}

				if (m_ItemSlotList.size() != MAILATTACHITEMMAX || sendMailInfo.attachItems.size() != MAILATTACHITEMMAX)
				{
					_ASSERT(0);
					return;
				}

				int i = 0;
				for (; i < MAILATTACHITEMMAX; ++i)
				{
					CDnItemSlotButton* pItemSlot = m_ItemSlotList[i];
					if (pItemSlot)
					{
						CDnTradeMail::SSendAttachItemInfo& info = sendMailInfo.attachItems[i];
						CDnItem *pItem = static_cast<CDnItem*>(pItemSlot->GetItem());
						if( pItem )
						{
							info.index			= pItem->GetSlotIndex();
							info.itemClassID	= pItem->GetClassID();
							info.count			= pItemSlot->GetRenderCount();
							info.biSerial		= pItem->GetSerialID();
						}
						else
						{
							info.index = -1;
						}
					}
					else
					{
						_ASSERT(0);
					}
				}

				sendMailInfo.money		= m_MoneyCache.GetMoney();
				sendMailInfo.tax		= m_TaxCache.GetMoney();
				sendMailInfo.isPremium	= m_PremiumBtn->IsChecked() ? MailType::Premium : MailType::Normal;
				GetTradeTask().GetTradeMail().RequestMailSend(sendMailInfo);
				m_pConfirmDlg->Show(false);
			}
			else if (IsCmdControl("ID_CANCEL") ||
					 IsCmdControl("ID_CLOSE"))
			{
				m_pConfirmDlg->Show(false);
			}
		}
		else if (nID == ERROR_NO_NAME_MSGBOX ||
				 nID == ERROR_CANNOT_SEND_SELF)
		{
			if (IsCmdControl("ID_OK"))
				RequestFocus(m_pEditBoxName);
		}
		else if (nID == ERROR_NO_TITLE_MSGBOX)
		{
			if (IsCmdControl("ID_OK"))
				RequestFocus(m_pEditBoxTitle);
		}
		else if (nID == ITEM_ATTACH_CONFIRM_DIALOG)
		{
			if (IsCmdControl("ID_OK"))
			{
				CDnItem* pItem = m_pSplitConfirmExDlg->GetItem();
				if (pItem)
				{
					int nCount = m_pSplitConfirmExDlg->GetItemCount();

					if (m_pSplitConfirmExDlg->GetConfirmType() == CDnInterface::MAIL_WRITE)
					{
						if (nCount == 0)
						{
							GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1988), MB_OK ); // UISTRING : 0개는 첨부할 수 없습니다.
							return;
						}
						else if (nCount < 0)
						{
							CDnSlotButton* pFromSlot = m_pSplitConfirmExDlg->GetFromSlot();
							if (pFromSlot != NULL)
								pFromSlot->DisableSplitMode(true);
							m_pSplitConfirmExDlg->Show(false);
							return;
						}

						CDnItemSlotButton* pEmptySlot = NULL;
						GetEmptyAttachSlot(&pEmptySlot);
						if (pEmptySlot == NULL)
						{
							GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1971), MB_OK); // UISTRING : 빈 슬롯이 없어 아이템을 첨부할 수 없습니다
							return;
						}

						CDnSlotButton* pFromSlot = m_pSplitConfirmExDlg->GetFromSlot();
						if (pFromSlot != NULL)
						{
							AddAttachItem(pFromSlot, pEmptySlot, nCount);
							//pFromSlot->EnableSplitMode(nCount);
						}
					}

					m_pSplitConfirmExDlg->Show(false);
				}
				else
				{
					_ASSERT(0);
				}
			}
			else if (IsCmdControl("ID_CANCEL"))
			{
				CDnSlotButton* pFromSlot = m_pSplitConfirmExDlg->GetFromSlot();
				if (pFromSlot != NULL)
				{
					pFromSlot->DisableSplitMode(true);
					pFromSlot->SetRegist(false);
				}

				m_pSplitConfirmExDlg->Show(false);
			}
		}
	}
}

bool CDnMailWriteDlg::IsAnyAttach() const
{
	if (m_MoneyCache.GetMoney() > 0 )
		return true;

	int i = 0;
	for (; i < MAILATTACHITEMMAX; ++i)
	{
		CDnItemSlotButton* pItemSlot = m_ItemSlotList[i];
		if (pItemSlot)
		{
			if (pItemSlot->IsEmptySlot() == false)
				return true;
		}
	}

	return false;
}

void CDnMailWriteDlg::EnableButtons(bool bEnable)
{
	m_pButtonSend->Enable(bEnable);
	m_pButtonMoneyInput->Enable(bEnable);
	if (bEnable)
		m_pButtonMoneyDel->Enable(IsAnyAttach());
	else
		m_pButtonMoneyDel->Enable(false);
}

void CDnMailWriteDlg::CheckPremiumMail()
{
	m_nPremiumLetterItemCache.clear();

	CDnItemTask::GetInstance().FindItemFromItemType( ITEMTYPE_PREMIUM_POST, ST_INVENTORY, m_nPremiumLetterItemCache );
	if (m_nPremiumLetterItemCache.empty())
		CDnItemTask::GetInstance().FindItemFromItemType( ITEMTYPE_PREMIUM_POST, ST_INVENTORY_CASH, m_nPremiumLetterItemCache );

	m_PremiumBtn->Enable(IsEnablePremiumMail());
}

bool CDnMailWriteDlg::IsEnablePremiumMail() const
{
	// todo by kalliste : check mail cash item
	return (m_nPremiumLetterItemCache.empty() == false);
}

void CDnMailWriteDlg::SetDaySendEnableCount(bool bPremium)
{
	m_pEnableSendCountStatic->Enable(bPremium == false);

	SUIControlProperty* pProp = m_pEnableSendCountStatic->GetProperty();
	if (pProp == NULL)
		return;

	int count = GetTradeTask().GetTradeMail().GetDaySendEnableCount();
	DWORD color = (count == 0) ? EtInterface::textcolor::RED : pProp->StaticProperty.dwFontColor;
	m_pEnableSendCountStatic->SetTextColor(color);

	std::wstring countStr;
	countStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 114041), count); // UISTRING : 오늘 보낼 수 있는 우편 : %d 개
	m_pEnableSendCountStatic->SetText(countStr.c_str());
}

void CDnMailWriteDlg::ShowLetterPaper(bool bPremium)
{
	m_pPremiumLetterPaper->Show(bPremium);
	m_pNormalLetterPaper->Show(bPremium == false);
}

void CDnMailWriteDlg::SetPremium(bool bPremium)
{
	if (bPremium)
	{
		ShowAttachSlot(eSLOT_PREMIUM);
		SetDaySendEnableCount(true);
		ShowLetterPaper(true);
		if (m_pLineEditBox->IsShow())
		{
			m_pPremiumLineEditBox->ClearText();
			wchar_t text[MAILTEXTLENMAX] = {0,};
			m_pLineEditBox->GetTextCopy(text, MAILTEXTLENMAX);
			std::wstring str(text);
			RemoveStringW(str, std::wstring(L"\n"));
			m_pPremiumLineEditBox->SetTextBuffer(str.c_str());
			m_pPremiumLineEditBox->Show(true);
			m_pLineEditBox->Show(false);
		}
	}
	else
	{
		ShowAttachSlot(eSLOT_NORMAL);
		SetDaySendEnableCount(false);
		ShowLetterPaper(false);
		if (m_PremiumBtn->IsChecked())
			m_PremiumBtn->SetChecked(false);
		if (m_pPremiumLineEditBox->IsShow())
		{
			m_pLineEditBox->ClearText();
			wchar_t text[MAILTEXTLENMAX] = {0,};
			m_pPremiumLineEditBox->GetTextCopy(text, MAILTEXTLENMAX);
			std::wstring str(text);
			RemoveStringW(str, std::wstring(L"\n"));
			m_pLineEditBox->SetTextBuffer(str.c_str());
			m_pLineEditBox->Show(true);
			m_pPremiumLineEditBox->Show(false);
		}
	}
}

bool CDnMailWriteDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_TAB )
		{
			if (focus::IsSameControl(m_pEditBoxName))
			{
				RequestFocus(m_pEditBoxTitle);
				return true;
			}
		}
	}

	return CDnCustomDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}