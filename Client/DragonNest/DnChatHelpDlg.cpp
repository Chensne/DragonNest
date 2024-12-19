#include "StdAfx.h"
#include "DnChatHelpDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChatHelpDlg::CDnChatHelpDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pTextBoxHelp(NULL)
{
}

CDnChatHelpDlg::~CDnChatHelpDlg(void)
{
}

void CDnChatHelpDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ChatHelpDlg.ui" ).c_str(), bShow );
}

void CDnChatHelpDlg::InitialUpdate()
{
	m_pTextBoxHelp = GetControl<CEtUITextBox>("ID_TEXTBOX_HELP");
}

void CDnChatHelpDlg::AddChatHelpText( LPCWSTR wszKey1, LPCWSTR wszKey2, LPCWSTR wszText, int nChatType, DWORD dwColor )
{
	if( !m_pTextBoxHelp )
		return;

	SLineData sLineData;
	sLineData.m_nData = nChatType;

	m_pTextBoxHelp->SetLineData( sLineData );

	WCHAR wszTemp[128];
	swprintf_s(wszTemp, L"%s,%s %s", wszKey1, wszKey2, wszText);
	m_pTextBoxHelp->AddText( wszTemp, dwColor );
	//m_pTextBoxHelp->AppendText( wszKey, dwColor, UITEXT_RIGHT );
}