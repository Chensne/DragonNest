#include "Stdafx.h"
#include "DnPupilInformationDlg.h"
#include "DnMasterTask.h"
#include "DnInterfaceString.h"
#include "DnWorldData.h"
#include "DnWorldDataManager.h"
#include "DnInterface.h"
#include "DnPartyTask.h"

CDnPupilInformationDlg::CDnPupilInformationDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
	memset( m_pStaticMasterName, NULL, sizeof(CEtUIStatic *) * MasterSystem::Max::MasterCount );
}

CDnPupilInformationDlg::~CDnPupilInformationDlg()
{
}

void CDnPupilInformationDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PupilInformationDlg.ui" ).c_str(), bShow );
}

void CDnPupilInformationDlg::InitialUpdate()
{
	char szControlName[32];
	for( int itr = 0; itr < MasterSystem::Max::MasterCount; ++itr )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_TEXT_MASTERNAME%d", itr);
		m_pStaticMasterName[itr] = GetControl<CEtUIStatic>(szControlName);
	}
}

void CDnPupilInformationDlg::Show(bool bShow)
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
	}

	CEtUIDialog::Show( bShow );
}

void CDnPupilInformationDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnPupilInformationDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
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

void CDnPupilInformationDlg::RefreshInformation()
{
	const TClassmateInfoWithLocation & ClassmateInfo = GetMasterTask().GetClassmateInfo();

	CONTROL( Static, ID_TEXT_NAME )->SetText( ClassmateInfo.wszCharName );
	CONTROL( Static, ID_TEXT_CLASS )->SetText( DN_INTERFACE::STRING::GetJobString( ClassmateInfo.cJob ) );
	CONTROL( Static, ID_TEXT_LEVEL )->SetIntToText( ClassmateInfo.cLevel );
	
	if( ClassmateInfo.Location.cServerLocation )
	{
		CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( ClassmateInfo.Location.nMapIdx );

		if( pWorldData )
		{
			std::wstring wszWorldName;

			pWorldData->GetMapName(wszWorldName);

			if( eLocationState::_LOCATION_VILLAGE == ClassmateInfo.Location.cServerLocation )
			{
				WCHAR wszTemp[256];
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4307 ), wszWorldName.c_str(), ClassmateInfo.Location.nChannelID );
				wszWorldName = std::wstring(wszTemp);
			}

			CONTROL( Static, ID_TEXT_MAP )->SetText( wszWorldName );
		}

		CONTROL( Button, ID_BT_CHAT )->Enable( true );
		CONTROL( Button, ID_BT_JOINPARTY )->Enable( true );
	}
	else
		CONTROL( Static, ID_TEXT_MAP )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4305 ) );

	for( BYTE itr = 0; itr < ClassmateInfo.cMasterCount; ++itr )
	{
		m_pStaticMasterName[itr]->SetText( L"" );
		m_pStaticMasterName[itr]->SetText( ClassmateInfo.wszMasterCharName[itr] );
	}
}
