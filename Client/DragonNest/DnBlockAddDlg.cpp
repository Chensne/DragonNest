#include "StdAfx.h"
#include "DnBlockAddDlg.h"
#include "DnIsolate.h"
#include "DnFriendTask.h"
#include "DnPartyTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBlockAddDlg::CDnBlockAddDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pNameBox(NULL)
	, m_pOkButton(NULL)
	, m_pCancelButton(NULL)
{
}

void CDnBlockAddDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "BlockAddDlg.ui" ).c_str(), bShow );
}

void CDnBlockAddDlg::InitialUpdate()
{
	m_pNameBox		= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pOkButton		= GetControl<CEtUIButton>("ID_OK");
	m_pCancelButton	= GetControl<CEtUIButton>("ID_CANCEL");
}

void CDnBlockAddDlg::Add()
{
	const wchar_t* nameToAdd = m_pNameBox->GetText();
	if (m_pNameBox->GetTextLength() <= 0)
	{
		focus::ReleaseControl();

		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4201 ), MB_OK, MESSAGEBOX_BLOCK_ADD_USER_FAIL, this); //	UISTRING : 차단 할 캐릭터 이름을 입력해 주세요.
		return;
	}

	std::wstring rawName(m_pNameBox->GetText());
	rawName = boost::algorithm::trim_copy(rawName);

	if (rawName.compare(m_pNameBox->GetText()) != 0)
	{
		focus::ReleaseControl();

		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ), MB_OK, MESSAGEBOX_BLOCK_ADD_USER_FAIL, this); //	UISTRING : 캐릭터명에 지원되지 않는 특수 문자를 사용할 수 없습니다.
		return;
	}

	if (!__wcsicmp_l(nameToAdd, CDnActor::s_hLocalActor->GetName()))
	{
		focus::ReleaseControl();

		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4216 ), MB_OK, MESSAGEBOX_BLOCK_ADD_USER_FAIL, this);	//	UISTRING : 자기 자신은 차단할 수 없습니다
		return;
	}

	if (GetInterface().IsPVP() == false && GetInterface().IsFarm() == false&& GetPartyTask().IsPartyMember(nameToAdd))
	{
		focus::ReleaseControl();

		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4202 ), MB_OK, MESSAGEBOX_BLOCK_ADD_USER_FAIL, this); // UISTRING : 파티원은 차단할 수 없습니다.
		return;
	}

	if (GetFriendTask().IsFriend(nameToAdd))
	{
		focus::ReleaseControl();

		wchar_t wszTemp[256]={0};
		swprintf_s( wszTemp, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4204 ), nameToAdd );	// UISTRING : “%s”님이 차단되어 친구 목록에서 삭제됩니다.
		GetInterface().MessageBox(wszTemp, MB_OKCANCEL, MESSAGEBOX_BLOCK_FRIEND_DELETE, this);
		return;
	}

	GetIsolateTask().ReqIsolateAdd(nameToAdd);
}

void CDnBlockAddDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	bool bCmdProcessed = false;
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK" ) )
		{
			Add();
			bCmdProcessed = true;
		}
		else if (IsCmdControl("ID_CANCEL"))
		{
			Show(false);
			bCmdProcessed = true;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
		{
			std::wstring str = m_pNameBox->GetText();
			str = boost::algorithm::trim_copy(str);
			m_pOkButton->Enable(str.empty() == false); //m_pNameBox->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 );
			return;
		}
	}
	
	if (bCmdProcessed == false)
		CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnBlockAddDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnBlockAddDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
 		m_pNameBox->ClearText();
		RequestFocus( m_pNameBox );
		m_pOkButton->Enable(false);
	}
}

bool CDnBlockAddDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
		else if (wParam == VK_RETURN)
		{
			if (m_pOkButton->IsEnable())
			{
				if (m_pNameBox->GetTextLength() > 0)
					Add();
				return true;
			}
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnBlockAddDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_BLOCK_ADD_USER_FAIL:
		{
			m_pNameBox->SetText(m_pNameBox->GetText(), true);
			RequestFocus(m_pNameBox);
		}
		break;

	case MESSAGEBOX_BLOCK_FRIEND_DELETE:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if (IsCmdControl("ID_OK"))
				{
					GetIsolateTask().ReqIsolateAdd(m_pNameBox->GetText());
				}
				else if (IsCmdControl("ID_CANCEL"))
				{
					m_pNameBox->SetText(m_pNameBox->GetText(), true);
					RequestFocus(m_pNameBox);
				}
			}
		}
	}
}

void CDnBlockAddDlg::ReturnFocus()
{
	if (IsShow())
	{
		if (m_pNameBox && m_pNameBox->GetTextLength() > 0)
			m_pNameBox->SetText(m_pNameBox->GetText(), true);
		RequestFocus(m_pNameBox);
	}
}