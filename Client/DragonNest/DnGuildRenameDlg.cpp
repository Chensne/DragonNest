#include "StdAfx.h"
#include "DnGuildRenameDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "ItemSendPacket.h"
#include "DnMainMenuDlg.h"
#include "DNCountryUnicodeSet.h"
#include "DnGuildRenameConfirmDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRenameDlg::CDnGuildRenameDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pEditBoxGuildName(NULL)
, m_cInvenType(0)
, m_cInvenIndex(0)
, m_biItemSerial(0)
, m_pButtonOK(NULL)
, m_pConfirmDlg(NULL)
{
}

CDnGuildRenameDlg::~CDnGuildRenameDlg(void)
{
	SAFE_DELETE(m_pConfirmDlg);
}

void CDnGuildRenameDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSGuildRename.ui" ).c_str(), bShow );
}

void CDnGuildRenameDlg::InitialUpdate()
{	
	m_pEditBoxGuildName = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_INPUT");
	m_pButtonOK = GetControl<CEtUIButton>("ID_BT_YES");

	m_pConfirmDlg = new CDnGuildRenameConfirmDlg(UI_TYPE_MODAL, NULL, 0, this);
	m_pConfirmDlg->Initialize(false); 
}

void CDnGuildRenameDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_YES") )
		{
			std::wstring szName = m_pEditBoxGuildName->GetText();
			
			if (!g_CountryUnicodeSet.Check(szName.c_str())) 
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4302 ), MB_OK, 0, this );	// UISTING : 허용 되지 않는 문자가 포함되었습니다.
				return;
			}

			if( DN_INTERFACE::UTIL::CheckAccount( szName ) || DN_INTERFACE::UTIL::CheckChat( szName ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}
			
			m_pConfirmDlg->SetGuildName( szName.c_str() );
			m_pConfirmDlg->Show(true);
			return;
		}

		if( IsCmdControl("ID_BT_NO") || IsCmdControl("ID_BT_CLOSE") )  
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE || nCommand == EVENT_EDITBOX_IME_CHANGE )
	{
		if( IsCmdControl("ID_IMEEDITBOX_INPUT") )
		{
			bool bChangeable = true;			

			std::wstring szName = m_pEditBoxGuildName->GetText();
			szName = boost::algorithm::trim_copy(szName);
			if (szName.empty())
				bChangeable = false;
			if( m_pEditBoxGuildName->GetTextLength() + CEtUIIME::GetCompStringLength() > 0 && bChangeable )
				m_pButtonOK->Enable( true );
			else
				m_pButtonOK->Enable( false );
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGuildRenameDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{		
		m_pButtonOK->Enable(false);
		m_pEditBoxGuildName->ClearText();
		RequestFocus( m_pEditBoxGuildName );		
	}
	else {
		if( m_pConfirmDlg && m_pConfirmDlg->IsShow() ) 
			m_pConfirmDlg->Show( false );
	}
	CEtUIDialog::Show( bShow );
}

void CDnGuildRenameDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxGuildName );
		}
		else if( IsCmdControl( "ID_BT_YES") )
		{				
			SendGuildRename(m_cInvenType, m_cInvenIndex, m_biItemSerial, m_pEditBoxGuildName->GetText());
			m_pConfirmDlg->Show(false);
		}
		else if( IsCmdControl( "ID_BT_NO") || IsCmdControl( "ID_BUTTON_CLOSE") )
		{
			// 창을 완전 닫기?
			m_pConfirmDlg->Show(false);
		}
	}
}

void CDnGuildRenameDlg::SetInfo( char cInvenType, BYTE cInvenIndex, INT64 biItemSerial )
{
	m_cInvenType = cInvenType;	
	m_cInvenIndex = cInvenIndex;
	m_biItemSerial = biItemSerial;
}