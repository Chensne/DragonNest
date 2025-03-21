#include "stdafx.h"

#if defined(PRE_ADD_DWC)
#include "DnDwcAddMemberDlg.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "VillageSendPacket.h"

CDnDwcAddMemberDlg::CDnDwcAddMemberDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
:CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
, m_pIMEEditBox(NULL)
, m_pInviteButton(NULL)
, m_pComboBox(NULL)
{
}

CDnDwcAddMemberDlg::~CDnDwcAddMemberDlg()
{
}

void CDnDwcAddMemberDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("PVPTeamAddDlg.ui").c_str(), bShow);
}

void CDnDwcAddMemberDlg::InitialUpdate()
{
	m_pIMEEditBox	= GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pInviteButton = GetControl<CEtUIButton>("ID_BT_OK");
	m_pComboBox		= GetControl<CEtUIComboBox>("ID_COMBOBOX_NAME");
	m_pComboBox->Show(false);
}

void CDnDwcAddMemberDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if(nCommand == EVENT_BUTTON_CLICKED)
	{
		if(IsCmdControl("ID_BT_OK"))
		{
			std::wstring wszIMEText = m_pIMEEditBox->GetText();
			if(!wszIMEText.empty())
			{
				std::wstring rawName = boost::algorithm::trim_copy(wszIMEText);

				if( DN_INTERFACE::UTIL::CheckAccount( wszIMEText ) || DN_INTERFACE::UTIL::CheckChat( wszIMEText ) )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
					return;
				}

				if(rawName.compare(wszIMEText) != 0)
				{
					focus::ReleaseControl();
					//	UISTRING : 캐릭터명에 지원되지 않는 특수 문자를 사용할 수 없습니다.
					GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ), MB_OK, 123, this);
					return;
				}

				if(!__wcsicmp_l(wszIMEText.c_str(), CDnActor::s_hLocalActor->GetName()))
				{
					focus::ReleaseControl();
					// UISTRING : 자신은 친구로 등록할 수 없습니다
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4311), MB_OK, 123, this );
					return;
				}
			}

			// 팀 초대 패킷
			SendDWCInviteMember(wszIMEText);

			this->Show(false);
		}
		else if(IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE"))
		{
			this->Show(false);
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDwcAddMemberDlg::Process( float fElapsedTime )
{
	if(!IsShow())
		return;
	
	// ime비어있으면 비활성..
	std::wstring wszIMEText = m_pIMEEditBox->GetText();
	m_pInviteButton->Enable( !wszIMEText.empty() );

	CEtUIDialog::Process(fElapsedTime);
}

void CDnDwcAddMemberDlg::Show( bool bShow )
{
	if(m_bShow == bShow)
		return;

	if(bShow)
	{
		m_pIMEEditBox->ClearText();
		focus::SetFocus(m_pIMEEditBox);
	}

	CEtUIDialog::Show(bShow);
}
#endif // PRE_ADD_DWC