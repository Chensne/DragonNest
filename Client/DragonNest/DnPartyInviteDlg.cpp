#include "StdAfx.h"
#include "DnPartyTask.h"
#include "DnPartyInviteDlg.h"
#include "DnInterface.h"
#ifdef PRE_PARTY_DB
#include "DnMainMenuDlg.h"
#include "DnFriendTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartyInviteDlg::CDnPartyInviteDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pEditBoxGuestName(NULL)
	, m_pButtonOk(NULL)
	, m_pButtonCancel(NULL)
{
#ifdef PRE_PARTY_DB
	m_pFriendComboBox = NULL;
#endif
}

CDnPartyInviteDlg::~CDnPartyInviteDlg(void)
{
}

void CDnPartyInviteDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyAddDlg.ui" ).c_str(), bShow );
}

void CDnPartyInviteDlg::InitialUpdate()
{
	m_pEditBoxGuestName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pButtonOk			= GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel		= GetControl<CEtUIButton>("ID_CANCEL");

#ifdef PRE_PARTY_DB
	m_pFriendComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_NAME");
	m_pFriendComboBox->SetEditMode(true);
#endif
}

void CDnPartyInviteDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

 	CEtUIDialog::Show( bShow );

	if( bShow )
	{
#ifdef PRE_PARTY_DB
		SetFriendComboBox();
#endif

		m_pEditBoxGuestName->ClearText();
		RequestFocus(m_pEditBoxGuestName);
	}
}

bool CDnPartyInviteDlg::DoInvite()
{
	std::wstring str(m_pEditBoxGuestName->GetText());
	if (str.empty())
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4211), MB_OK, MESSAGEBOX_PARTY_INVITE_NONE, this);	// UISTRING : 캐릭터 이름을 입력해 주세요.
		return false;
	}
	else
	{
		GetPartyTask().ReqInviteParty(str.c_str());
	}

	return true;
}

void CDnPartyInviteDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

#ifdef PRE_PARTY_DB
	if (IsCmdControl("ID_IMEEDITBOX_NAME"))
	{
		if( nCommand == EVENT_EDITBOX_IME_CHANGE )
		{
			std::wstring curText = m_pEditBoxGuestName->GetText();
			std::wstring composeStr;
			m_pEditBoxGuestName->GetIMECompositionString(composeStr);
			curText += composeStr;
			OnAddEditBoxString(curText);
			return;
		}
		else if (nCommand == EVENT_EDITBOX_CHANGE)
		{
			WCHAR ch = (WCHAR)uMsg;
			if (CommonUtil::IsCtrlChar(ch) == false)
			{
				std::wstring curText = m_pEditBoxGuestName->GetText();
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
				CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
				return;
			}

			if (m_AutoCompleteTextCache != _T(""))
			{
				m_pEditBoxGuestName->ClearText();
				m_pEditBoxGuestName->SetText(m_AutoCompleteTextCache.c_str());
				m_AutoCompleteTextCache = _T("");
			}
			return;
		}
	}
#endif

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_OK"))
		{
			if (DoInvite())
				Show(false);
			return;
		}

		if (IsCmdControl("ID_CANCEL"))
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser )
	{
		if( IsCmdControl( "ID_COMBOBOX_NAME" ) && m_pFriendComboBox->IsOpenedDropDownBox() == false )
		{
			SComboBoxItem* pItem = m_pFriendComboBox->GetSelectedItem();
			if( pItem )
			{
				m_pEditBoxGuestName->ClearText();
				m_pEditBoxGuestName->SetText( pItem->strText );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnPartyInviteDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			focus::ReleaseControl();	// todo : remove this and set ESC hotkey at ui tool next time - by kalliste
			return true;
		}
		else if (wParam == VK_RETURN)
		{
			if (DoInvite())
			{
				Show(false);
				focus::ReleaseControl();	// todo : remove this and set ESC hotkey at ui tool next time - by kalliste
			}
			return true;
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnPartyInviteDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( nID == MESSAGEBOX_PARTY_INVITE_NONE )
		{
			if( IsCmdControl("ID_OK" ) )
				RequestFocus(m_pEditBoxGuestName);
		}
	}
}

#ifdef PRE_PARTY_DB
void CDnPartyInviteDlg::SetFriendComboBox()
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
			m_AutoCompleteMgr.AddTextUnit(info.wszFriendName[0], info.wszFriendName);
			++i;
		}
	}

	m_pFriendComboBox->ClearSelectedItem();
}

void CDnPartyInviteDlg::OnAddEditBoxString(const std::wstring& strName)
{
	int curEditBoxCaretPos = m_pEditBoxGuestName->GetCaretPos();
	if (strName.size() < 1) return;

	std::wstring almost = L"";
	m_AutoCompleteMgr.GetAlmostMatchingString(almost, strName);

	if (almost != _T(""))
	{
		m_pEditBoxGuestName->ClearText();
		m_pEditBoxGuestName->SetText(almost.c_str());
		m_pEditBoxGuestName->SetSelection(m_pEditBoxGuestName->GetTextLength(), curEditBoxCaretPos);
		m_AutoCompleteTextCache = almost;
	}
	else
	{
		m_AutoCompleteTextCache = _T("");
	}
}
#endif