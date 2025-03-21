#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDwcCreateTeamDlg.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "VillageSendPacket.h"

CDnDwcCreateTeamDlg::CDnDwcCreateTeamDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
:CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true)
, m_pIMEEditBox(NULL)
, m_pInviteButton(NULL)
{
}

CDnDwcCreateTeamDlg::~CDnDwcCreateTeamDlg()
{
}

void CDnDwcCreateTeamDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("PVPTeamCreateDlg.ui").c_str(), bShow);
}

void CDnDwcCreateTeamDlg::InitialUpdate()
{
	m_pIMEEditBox	= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pInviteButton = GetControl<CEtUIButton>("ID_BT_OK");
	m_pInviteButton->Enable(false);
}

void CDnDwcCreateTeamDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if(IsCmdControl("ID_BT_OK"))
		{
			std::wstring wszIMEText = m_pIMEEditBox->GetText();
			if(!wszIMEText.empty())
			{
				std::wstring rawName = boost::algorithm::trim_copy(wszIMEText);

				if( DN_INTERFACE::UTIL::CheckAccount( wszIMEText ) || DN_INTERFACE::UTIL::CheckChat( wszIMEText ) )
				{
					// 사용할 수 없는 단어가 포함되어있습니다.
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772), MB_OK, 0, this );
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772), false);
					return;
				}
				
				if(rawName.compare(wszIMEText) != 0)
				{
					focus::ReleaseControl();
					 //	UISTRING : 캐릭터명에 지원되지 않는 특수 문자를 사용할 수 없습니다.
					GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821), MB_OK, 123, this);
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821), false);
					return;
				}
				
				if(CDnActor::s_hLocalActor)
				{
					if(!__wcsicmp_l(wszIMEText.c_str(), CDnActor::s_hLocalActor->GetName()))
					{
						focus::ReleaseControl();
						// UISTRING : 자신은 친구로 등록할 수 없습니다
						GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4311), MB_OK, 123, this );
						GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4311), false);
						return;
					}
				}

				// 문제 없다면, 서버에 전송...
				SendDWCTeamCreate(wszIMEText);
				return;
			}
		}
		else if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			this->Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDwcCreateTeamDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_YES") || IsCmdControl("ID_OK"))
		{
			switch(nID)
			{
			case 123:
				{
					m_pIMEEditBox->ClearText();
					focus::SetFocus(m_pIMEEditBox);
				}
				break;
			}
		}
	}
}

void CDnDwcCreateTeamDlg::Process( float fElapsedTime )
{
	if(!IsShow())
		return;
	
	// ime비어있으면 창설버튼 비활성화
	std::wstring wszTeamName = m_pIMEEditBox->GetText();
	m_pInviteButton->Enable(!wszTeamName.empty());

	CEtUIDialog::Process(fElapsedTime);
}

void CDnDwcCreateTeamDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{
		// 리셋
		m_pIMEEditBox->ClearText();
		RequestFocus(m_pIMEEditBox);
	}
	else
	{
		GetInterface().CloseBlind();
	}

	CEtUIDialog::Show(bShow);
}
#endif // PRE_ADD_DWC