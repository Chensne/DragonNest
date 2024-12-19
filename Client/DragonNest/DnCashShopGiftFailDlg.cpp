#include "stdafx.h"
#include "DnCashShopGiftFailDlg.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopGiftFailDlg::CDnCashShopGiftFailDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
, m_pCloseBtn( NULL )
, m_pItemsTextBox( NULL )
, m_pFailStatic( NULL )
, m_bIgnoreEnterHotkey( false )
, m_pCancelBtn( NULL )
, m_pCloseSubBtn( NULL )
, m_pOKBtn( NULL )
{
}

void CDnCashShopGiftFailDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CSCartFailed.ui" ).c_str(), bShow);
}

void CDnCashShopGiftFailDlg::InitialUpdate()
{
	m_pCloseBtn		= GetControl<CEtUIButton>("ID_BUTTON_CLOSE");
	m_pCloseBtn->Show(false);
	m_pCloseSubBtn	= GetControl<CEtUIButton>("ID_BUTTON_CLOSESUB");
	m_pItemsTextBox = GetControl<CEtUITextBox>("ID_TEXTBOX_ITEMNAME");
	m_pFailStatic	= GetControl<CEtUIStatic>("ID_STATIC_MSG");
	m_pOKBtn		= GetControl<CEtUIButton>("ID_BUTTON_OK");
	m_pCancelBtn	= GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
}

void CDnCashShopGiftFailDlg::ShowBtns(bool bShow)
{
	m_pOKBtn->Show(bShow);
	m_pCancelBtn->Show(bShow);
	m_pCloseBtn->Show(bShow);
}

void CDnCashShopGiftFailDlg::SetInfo(const std::wstring& msg, const std::vector<CASHITEM_SN>& itemList, UINT uType, int nID, CEtUICallback *pCall)
{
	if (CDnCashShopTask::IsActive() == false)
		return;

	m_pFailStatic->SetText(msg.c_str());

	m_pItemsTextBox->ClearText();
	std::vector<CASHITEM_SN>::const_iterator iter = itemList.begin();
	for (; iter != itemList.end(); ++iter)
	{
		CASHITEM_SN unableSN = *iter;
		const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(unableSN);
		if (pInfo)
			m_pItemsTextBox->AddText(pInfo->nameString.c_str());
	}

	ShowBtns(false);
	m_bIgnoreEnterHotkey = false;
	if (uType == MB_YESNO)
	{
		m_pOKBtn->Show(true);
		m_pCancelBtn->Show(true);
		m_SmartMove.SetControl( m_pCancelBtn );
		m_bIgnoreEnterHotkey = true;
	}
	else if (uType == MB_OK)
	{
		m_pCloseBtn->Show(true);
		m_SmartMove.SetControl( m_pCloseBtn );
	}
	else if (uType == MB_RETRYCANCEL )	// 버튼 이름을 "구매"가 아닌 "담기"로 바꾸기 위한 식별자로 사용합니다.
	{
		m_pOKBtn->Show(true);
		m_pOKBtn->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4612 ) );
		m_pCancelBtn->Show(true);
		m_SmartMove.SetControl( m_pCancelBtn );
		m_bIgnoreEnterHotkey = true;
	}

	m_nDialogID = nID;

	SetCallback(pCall);
}

void CDnCashShopGiftFailDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_CLOSE") 
			|| IsCmdControl("ID_BUTTON_OK")
			|| IsCmdControl("ID_BUTTON_CANCEL")
			|| IsCmdControl("ID_BUTTON_CLOSESUB")
			)
			Show(false);
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopGiftFailDlg::Show( bool bShow ) 
{
	if (bShow == m_bShow || CDnCashShopTask::IsActive() == false)
		return;

	if (bShow)
	{
		const std::wstring& staticStr = m_pFailStatic->GetText();
		if (staticStr.empty())
		{
			GetCashShopTask().HandleCashShopError(eERRCS_CART_UNABLE_BUY_DLG_INFO_FAIL, true);
			return;
		}

		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}

	CEtUIDialog::Show(bShow);
}

bool CDnCashShopGiftFailDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow())
		return false;

	if (m_bIgnoreEnterHotkey)
	{
		if (uMsg == WM_KEYDOWN)
		{
			if (m_pOKBtn->GetHotKey() == wParam)
			{
				ProcessCommand(EVENT_BUTTON_CLICKED, false, m_pCancelBtn, 0);
				return true;
			}
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}