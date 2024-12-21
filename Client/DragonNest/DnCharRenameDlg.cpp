#include "StdAfx.h"
#include "DnCharRenameDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "ItemSendPacket.h"
#include "DNCountryUnicodeSet.h"
#include "DnInterface.h"
#include "DnPlayerActor.h"
#include "ItemSendPacket.h"
#include "strsafe.h"
#include "DnPartyTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharRenameDlg::CDnCharRenameDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pEditBox = NULL;
	m_pButtonOK = NULL;
	m_pItem = NULL;
}

CDnCharRenameDlg::~CDnCharRenameDlg(void)
{
}

void CDnCharRenameDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CSCharRename.ui" ).c_str(), bShow );
}

void CDnCharRenameDlg::InitialUpdate()
{	
	m_pEditBox = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_INPUT");
	m_pButtonOK = GetControl<CEtUIButton>("ID_BT_OK");
}

void CDnCharRenameDlg::EnableControl( bool bEnable )
{
	GetControl( "ID_IMEEDITBOX_INPUT" )->Enable( bEnable );
	GetControl( "ID_BT_OK" )->Enable( bEnable );
	GetControl( "ID_BT_CANCEL" )->Enable( bEnable );
	GetControl( "ID_BT_CLOSE" )->Enable( bEnable );

	if( bEnable ) {
		bool bChangeable = true;
		std::wstring szName = m_pEditBox->GetText();
		szName = boost::algorithm::trim_copy(szName);
		if (szName.empty())
			bChangeable = false;
		if( m_pEditBox->GetTextLength() + CEtUIIME::GetCompStringLength() >= CHARNAMEMIN && bChangeable )
			m_pButtonOK->Enable( true );
		else
			m_pButtonOK->Enable( false );
	}
}

void CDnCharRenameDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_OK") )
		{
			std::wstring szName = m_pEditBox->GetText();
			if( szName.empty() ) {
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100013 ), MB_OK );
				return;
			}
			if( szName.size() < CHARNAMEMIN ) {
#ifdef PRE_FIX_CHARRENAME_MSG
				std::wstring msg;
				msg = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 100303), CHARNAMEMIN);
				GetInterface().MessageBox(msg.c_str(), MB_OK);
#else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100303 ), MB_OK );
#endif
				return;
			}

			DWORD dwCheckType = ALLOW_STRING_DEFAULT;
#if defined (_US)
			dwCheckType = ALLOW_STRING_CHARACTERNAME_ENG;
#endif
			if (g_CountryUnicodeSet.Check(szName.c_str(), dwCheckType) == false)
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4821 ), MB_OK );
				return;
			}

			if( DN_INTERFACE::UTIL::CheckAccount( szName ) || DN_INTERFACE::UTIL::CheckChat( szName ) )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 84 ), MB_OK );
				return;
			}

			if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3797 ) );
				return;
			}

			if( GetPartyTask().GetPartyRole() != CDnPartyTask::SINGLE )
			{
				GetInterface().ServerMessageBox( 463 );	// 파티 상태에서는 사용이 불가능한 아이템 입니다.
				return;
			}

			WCHAR wszStr[256];
			swprintf_s( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4584 ), szName.c_str() );
			GetInterface().MessageBox( wszStr, MB_YESNO, 0, this );
			return;
		}

		if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )  
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

			std::wstring szName = m_pEditBox->GetText();
			szName = boost::algorithm::trim_copy(szName);
			if (szName.empty())
				bChangeable = false;
			if( m_pEditBox->GetTextLength() + CEtUIIME::GetCompStringLength() >= CHARNAMEMIN && bChangeable )
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

void CDnCharRenameDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pEditBox->ClearText();
		EnableControl( true );
		RequestFocus( m_pEditBox );
	}
	CEtUIDialog::Show( bShow );
}

void CDnCharRenameDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) {
		if( IsCmdControl( "ID_YES" ) ) {
			char cType = 0;
			WCHAR wszCharName[NAMELENMAX];
			cType = ( m_pItem->IsCashItem() ) ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
			StringCchCopyW( wszCharName, NAMELENMAX, m_pEditBox->GetText() );
			SendCharRename( cType, m_pItem->GetSlotIndex(), m_pItem->GetSerialID(), wszCharName );
			EnableControl( false );
		}
		else if( IsCmdControl( "ID_NO" ) ) {
			EnableControl( true );
			RequestFocus( m_pEditBox );
		}
	}
}

void CDnCharRenameDlg::SetCharRenameItem( CDnItem *pItem )
{
	m_pItem = pItem;
}