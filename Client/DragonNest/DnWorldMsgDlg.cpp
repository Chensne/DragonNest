#include "StdAfx.h"
#include "DnWorldMsgDlg.h"
#include "DnInterface.h"
#include "DnInterfaceString.h"
#include "VillageSendPacket.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldMsgDlg::CDnWorldMsgDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticTitle(NULL)
, m_pEditBoxMsg(NULL)
, m_pButtonOK(NULL)
, m_nType(0)
, m_biItemSerial(0)
{
}

CDnWorldMsgDlg::~CDnWorldMsgDlg(void)
{
}

void CDnWorldMsgDlg::Initialize( bool bShow )
{
#ifdef PRE_ADD_WORLD_MSG_RED
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "WorldMsgLineEditDlg.ui" ).c_str(), bShow );
#else // PRE_ADD_WORLD_MSG_RED
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "WorldMsgDlg.ui" ).c_str(), bShow );
#endif // PRE_ADD_WORLD_MSG_RED
}

void CDnWorldMsgDlg::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_STATIC_TITLE");
#ifdef PRE_ADD_WORLD_MSG_RED
	m_pEditBoxMsg = GetControl<CEtUILineIMEEditBox>("ID_LINEIMEEDITBOX_INPUT");
	m_pEditBoxMsg->SetMaxEditLength( MAX_WORLD_MSG_CHAR + 1 );
#else // PRE_ADD_WORLD_MSG_RED
	m_pEditBoxMsg = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_INPUT");
#endif // PRE_ADD_WORLD_MSG_RED
	m_pButtonOK = GetControl<CEtUIButton>("ID_BUTTON_OK");
}

void CDnWorldMsgDlg::Process( float fElapsedTime )
{
	if (m_pButtonOK)
	{
		if( m_pEditBoxMsg->GetTextLength() > 0 )
			m_pButtonOK->Enable( true );
		else
			m_pButtonOK->Enable( false );
	}

	CEtUIDialog::Process(fElapsedTime);
}

void CDnWorldMsgDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_OK") )
		{
#ifdef PRE_ADD_WORLD_MSG_RED
			wchar_t wszTemp[CHATLENMAX] = {0, };
			m_pEditBoxMsg->GetOriginTextCopy( wszTemp, CHATLENMAX );

			std::wstring szStr = wszTemp;
#else // PRE_ADD_WORLD_MSG_RED
			std::wstring szStr = m_pEditBoxMsg->GetText();
#endif // PRE_ADD_WORLD_MSG_RED
			if( DN_INTERFACE::UTIL::CheckChat( szStr ) )
			{
				Show(false);
				CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
				if (pInvenDlg)
					pInvenDlg->ShowTab( ST_INVENTORY_CASH );
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ), MB_OK, 0, this );
				return;
			}

			eChatType eType = (eChatType)0;
			if( m_nType == 1 ) eType = CHATTYPE_WORLD;
			else if( m_nType == 2 ) eType = CHATTYPE_CHANNEL;
#ifdef PRE_ADD_WORLD_MSG_RED
			else if( m_nType == 3 ) eType = CHATTYPE_WORLD_POPMSG;
			SendChatMsg( eType, wszTemp, m_biItemSerial );
#else // PRE_ADD_WORLD_MSG_RED
			SendChatMsg( eType, m_pEditBoxMsg->GetText(), m_biItemSerial );
#endif // PRE_ADD_WORLD_MSG_RED
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if (pInvenDlg)
				pInvenDlg->ShowTab( ST_INVENTORY_CASH );
			Show(false);

			return;
		}

		if( IsCmdControl("ID_BUTTON_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		focus::ReleaseControl();
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnWorldMsgDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pButtonOK->Enable( false );
		m_pEditBoxMsg->ClearText();
		RequestFocus( m_pEditBoxMsg );

		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		pInvenDlg->ShowTab( ST_INVENTORY );
	}

	CDnLocalPlayerActor::LockInput(bShow);
	CDnLocalPlayerActor::StopAllPartyPlayer();

	CEtUIDialog::Show( bShow );
}

void CDnWorldMsgDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			RequestFocus( m_pEditBoxMsg );
		}
	}
}

void CDnWorldMsgDlg::AppendChatEditBox( LPCWSTR wszString, bool bFocus )
{
	if( m_pEditBoxMsg->GetTextLength() + lstrlenW(wszString) >= CHATLENMAX )
		return;

	if( bFocus == false )
		focus::ReleaseControl();

	m_pEditBoxMsg->AddText( wszString, bFocus );
}

void CDnWorldMsgDlg::SetInfo( int nType, INT64 biItemSerial )
{
	m_nType = nType;
	if( m_nType == 1 ) m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114029 ) );
	else if( m_nType == 2 ) m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114030 ) );
#ifdef PRE_ADD_WORLD_MSG_RED
	else if( m_nType == 3 ) m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114029 ) );
#endif // PRE_ADD_WORLD_MSG_RED
	m_biItemSerial = biItemSerial;
}