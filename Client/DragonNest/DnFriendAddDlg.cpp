#include "StdAfx.h"
#include "DnFriendAddDlg.h"
#include "DnFriendTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFriendAddDlg::CDnFriendAddDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pEditBoxFriendName(NULL)
	, m_pComboBoxGroup(NULL)
	, m_pOkButton(NULL)
{
}

CDnFriendAddDlg::~CDnFriendAddDlg(void)
{
}

void CDnFriendAddDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "FriendAddDlg.ui" ).c_str(), bShow );
}

void CDnFriendAddDlg::InitialUpdate()
{
	m_pEditBoxFriendName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pComboBoxGroup = GetControl<CEtUIComboBox>("ID_COMBOBOX_GROUP");
	m_pOkButton				= GetControl<CEtUIButton>("ID_OK");
}

bool CDnFriendAddDlg::DoAddFriend()
{
	std::wstring strName( m_pEditBoxFriendName->GetText() );
	if (strName.empty() == false)
	{
		std::wstring rawName = boost::algorithm::trim_copy(strName);

		if (rawName.compare(strName) != 0)
		{
			focus::ReleaseControl();
			GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ), MB_OK, MSGBOXID_CANNOT_ADDSELF, this); //	UISTRING : 캐릭터명에 지원되지 않는 특수 문자를 사용할 수 없습니다.
			return false;
		}

		if (!__wcsicmp_l(strName.c_str(), CDnActor::s_hLocalActor->GetName()))
		{
			focus::ReleaseControl();
			GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4311), MB_OK, MSGBOXID_CANNOT_ADDSELF, this );	// UISTRING : 자신은 친구로 등록할 수 없습니다
			return false;
		}

		int nGroupIndex(0);
		if( m_pComboBoxGroup->GetSelectedValue( nGroupIndex ) )
			GetFriendTask().RequestFriendAdd( nGroupIndex, strName.c_str() );
	}
	else
	{
		// Note : 이름을 입력하라는 메세지 박스 출력
		//
		RequestFocus(m_pEditBoxFriendName);
		return false;
	}

	return true;
}

void CDnFriendAddDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK" ) )
		{
			if (DoAddFriend())
				Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
		{
			std::wstring str = m_pEditBoxFriendName->GetText();
			str = boost::algorithm::trim_copy(str);
			m_pOkButton->Enable(str.empty() == false); //m_pEditBoxFriendName->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 );
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnFriendAddDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnFriendAddDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_pOkButton->Enable(false);
		SetFriendGroup();
		m_pEditBoxFriendName->ClearText();
		RequestFocus( m_pEditBoxFriendName );
	}
}

void CDnFriendAddDlg::SetFriendGroup()
{
	// Note : Friend Task에서 그룹 정보를 받아서 셋팅한다.
	//
	m_pComboBoxGroup->RemoveAllItems();
	m_pComboBoxGroup->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1346/*그룹없음*/ ), NULL, 0 );

	const CDnFriendTask::FRIENDGROUPINFO_VEC &vecFriendGroupInfo = GetFriendTask().GetFriendGroupInfoList();
	for( int i=0; i<(int)vecFriendGroupInfo.size(); i++ )
	{
		m_pComboBoxGroup->AddItem( vecFriendGroupInfo[i].wszGroupName.c_str(), NULL, vecFriendGroupInfo[i].nGroupDBID, true );
	}
}

bool CDnFriendAddDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
				if (DoAddFriend())
					Show(false);
				return true;
			}
		}
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnFriendAddDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (nID == MSGBOXID_CANNOT_ADDSELF)
		{
			if (IsCmdControl("ID_OK" ))
			{
				m_pEditBoxFriendName->ClearText();
				RequestFocus(m_pEditBoxFriendName);
			}
		}
	}
}