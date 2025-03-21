#include "Stdafx.h"
#include "DnMasterIntroduceDlg.h"
#include "DnPupilListDlg.h"
#include "DnMasterTask.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"

#define MAX_INTRODUCTION	18

CDnMasterIntroduceDlg::CDnMasterIntroduceDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnMasterIntroduceDlg::~CDnMasterIntroduceDlg()
{
}

void CDnMasterIntroduceDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterIntroduceDlg.ui" ).c_str(), bShow );
}

void CDnMasterIntroduceDlg::InitialUpdate()
{
	CONTROL( EditBox, ID_IMEEDITBOX_TITLE)->SetMaxEditLength( MAX_INTRODUCTION );
}

void CDnMasterIntroduceDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CONTROL( EditBox, ID_IMEEDITBOX_TITLE)->SetText( L"" );
		focus::SetFocus( CONTROL( EditBox, ID_IMEEDITBOX_TITLE) );
	}

	CEtUIDialog::Show( bShow );
}

void CDnMasterIntroduceDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterIntroduceDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CANCEL" ) ) {
			Show( false );
		}
		else if( IsCmdControl( "ID_CLOSE" ) ) {
			Show( false );
		}
		else if( IsCmdControl( "ID_OK" ) ) {
			std::wstring wszString = std::wstring( CONTROL( EditBox, ID_IMEEDITBOX_TITLE)->GetText() );

			if( wszString.empty() )
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7279 ) );
			else if( !DN_INTERFACE::UTIL::CheckChat( wszString ) )
				GetMasterTask().RequestMasterIntroduction( true, wszString.c_str() );
			else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3772 ) );
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMasterIntroduceDlg::OnRecvIntroduction(  bool bRegist  )
{
	if( bRegist )
		CONTROL( EditBox, ID_IMEEDITBOX_TITLE)->SetText( GetMasterTask().GetMasterCharacterInfo().wszSelfIntroduction );
	else
		CONTROL( EditBox, ID_IMEEDITBOX_TITLE)->SetText( L"" );
}
