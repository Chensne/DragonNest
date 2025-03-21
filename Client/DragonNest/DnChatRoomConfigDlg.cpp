#include "StdAfx.h"
#include "DnChatRoomConfigDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "DnChatRoomTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnChatRoomConfigDlg::CDnChatRoomConfigDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnChatRoomCreateDlg( dialogType, pParentDialog, nID, pCallback )
{
}

CDnChatRoomConfigDlg::~CDnChatRoomConfigDlg(void)
{
}

void CDnChatRoomConfigDlg::InitialUpdate()
{
	CDnChatRoomCreateDlg::InitialUpdate();

	GetControl<CEtUIStatic>("ID_STATIC12")->Show( false );
	GetControl<CEtUIStatic>("ID_STATIC13")->Show( false );

	GetControl<CEtUIStatic>("ID_STATIC_TITLE")->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8128 ) );
	m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3533 ) );
}

void CDnChatRoomConfigDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RequestFocus( m_pEditBoxName );
	}

	CEtUIDialog::Show( bShow );
}

void CDnChatRoomConfigDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CREATE") )
		{
			std::wstring szName = m_pEditBoxName->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( szName ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			std::wstring wszPR0 = m_pEditBoxPR0->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( wszPR0 ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			std::wstring wszPR1 = m_pEditBoxPR1->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( wszPR1 ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			std::wstring wszPR2 = m_pEditBoxPR2->GetText();
			if( DN_INTERFACE::UTIL::CheckChat( wszPR2 ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			if( m_pCheckBoxSecret->IsChecked() )
			{
				std::wstring wszPassword = m_pEditBoxPassword->GetText();
				if( wszPassword.empty() || ((int)wszPassword.size() < CHATROOMPASSWORDMAX) )
				{
					GetInterface().MessageBox( MESSAGEBOX_35, MB_OK, 0, this );
					return;
				}
			}

			int nType = 0, nAllow = 0;
			m_pComboRoomType->GetSelectedValue( nType );
			m_pComboCondition->GetSelectedValue( nAllow );
			GetChatRoomTask().RequestConfigChatRoom( szName.c_str(), m_pEditBoxPassword->GetText(), nType, nAllow, wszPR0.c_str(), wszPR1.c_str(), wszPR2.c_str() );
			Show( false );
			return;
		}
	}

	CDnChatRoomCreateDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChatRoomConfigDlg::SetInfo( TChatRoomView &View, int nAllow, LPCWSTR pwszPassword )
{
	m_pComboRoomType->SetSelectedByValue( View.m_nChatRoomType );
	m_pComboCondition->SetSelectedByValue( nAllow );

	m_pCheckBoxSecret->SetChecked( View.m_bHasPassword );
	m_pEditBoxPassword->SetText( pwszPassword );

	m_pEditBoxPR0->SetText( View.m_wszChatRoomPRLine1 );
	m_pEditBoxPR1->SetText( View.m_wszChatRoomPRLine2 );
	m_pEditBoxPR2->SetText( View.m_wszChatRoomPRLine3 );

	m_pEditBoxName->SetText( View.m_wszChatRoomName );
}