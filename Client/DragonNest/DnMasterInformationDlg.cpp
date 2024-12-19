#include "Stdafx.h"
#include "DnMasterInformationDlg.h"
#include "DnMasterTask.h"
#include "DnInterfaceString.h"
#include "DnWorldData.h"
#include "DnWorldDataManager.h"
#include "DnInterface.h"
#include "DnPartyTask.h"

CDnMasterInformationDlg::CDnMasterInformationDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnMasterInformationDlg::~CDnMasterInformationDlg()
{
}

void CDnMasterInformationDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterInformationDlg.ui" ).c_str(), bShow );
}

void CDnMasterInformationDlg::InitialUpdate()
{

}

void CDnMasterInformationDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CONTROL( Button, ID_BT_CHAT )->Enable( false );
		CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );

		CONTROL( Static, ID_TEXT_NAME )->SetText( L"" );
		CONTROL( Static, ID_TEXT_CLASS )->SetText( L"" );
		CONTROL( Static, ID_TEXT_LEVEL )->SetText( L"" );
		CONTROL( Static, ID_TEXT_MAP )->SetText( L"" );
		CONTROL( Static, ID_TEXT_RESPECT )->SetText( L"" );
		CONTROL( Static, ID_TEXT_REPUTE )->SetText( L"" );
	}

	CEtUIDialog::Show( bShow );
}

void CDnMasterInformationDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterInformationDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_CLOSE" ) ) {
			Show( false );
		}
		else if( IsCmdControl( "ID_BT_CHAT") ) {
			GetInterface().OpenPrivateChatDialog( CONTROL( Static, ID_TEXT_NAME )->GetText() );
		}
		else if( IsCmdControl( "ID_BT_JOINPARTY") ) {
			GetPartyTask().ReqInviteParty( CONTROL( Static, ID_TEXT_NAME )->GetText() );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMasterInformationDlg::RefreshInformation()
{
	const TMyMasterInfoWithLocation & MyMasterInfo = GetMasterTask().GetMyMasterInfo();

	CONTROL( Static, ID_TEXT_NAME )->SetText( MyMasterInfo.wszCharName );
	CONTROL( Static, ID_TEXT_CLASS )->SetText( DN_INTERFACE::STRING::GetJobString( MyMasterInfo.cJob ) );
	CONTROL( Static, ID_TEXT_LEVEL )->SetIntToText( MyMasterInfo.cLevel );

	if( MyMasterInfo.Location.cServerLocation )
	{
		CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( MyMasterInfo.Location.nMapIdx );

		if( pWorldData )
		{
			std::wstring wszWorldName;

			pWorldData->GetMapName(wszWorldName);

			if( eLocationState::_LOCATION_VILLAGE == MyMasterInfo.Location.cServerLocation )
			{
				WCHAR wszTemp[256];
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4307 ), wszWorldName.c_str(), MyMasterInfo.Location.nChannelID );
				wszWorldName = std::wstring(wszTemp);
			}

			CONTROL( Static, ID_TEXT_MAP )->SetText( wszWorldName );
		}
		
		CONTROL( Button, ID_BT_CHAT )->Enable( true );
		CONTROL( Button, ID_BT_JOINPARTY )->Enable( true );
	}
	else
		CONTROL( Static, ID_TEXT_MAP )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4305 ) );

	CONTROL( Static, ID_TEXT_RESPECT )->SetIntToText( MyMasterInfo.iRespectPoint );
	CONTROL( Static, ID_TEXT_REPUTE )->SetIntToText( MyMasterInfo.iFavorPoint );
}
