#include "StdAfx.h"
#include "DnGameRadioMsgOptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameRadioMsgOptDlg::CDnGameRadioMsgOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: BaseClass( dialogType, pParentDialog, nID, pCallback )
{
}

CDnGameRadioMsgOptDlg::~CDnGameRadioMsgOptDlg()
{
}

void CDnGameRadioMsgOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameRadioMsgOptDlg.ui" ).c_str(), bShow );
}

void CDnGameRadioMsgOptDlg::InitialUpdate()
{
	// 어차피 영문이라 따로 스트링 등록 안하고 여기서 처리하겠다.
	char szName[32];
	WCHAR wszText[4];
	for( int i = 0; i < 10; ++i ) {
		sprintf_s( szName, _countof(szName), "ID_STATIC_BOX%d", i );
		swprintf_s( wszText, _countof(wszText), L"F%d", i+1 );
		GetControl<CEtUIStatic>(szName)->SetText( wszText );
	}
}

void CDnGameRadioMsgOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		//if( IsCmdControl( "ID_BUTTON_CLOSE" ) )
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGameRadioMsgOptDlg::ExportSetting()
{
}

void CDnGameRadioMsgOptDlg::ImportSetting()
{
}

bool CDnGameRadioMsgOptDlg::IsChanged()
{
	return false;
}
