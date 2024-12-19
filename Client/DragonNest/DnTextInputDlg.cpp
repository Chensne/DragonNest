#include "StdAfx.h"
#include "DnTextInputDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnTextInputDlg::CDnTextInputDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pTextBoxText(NULL)
	, m_pEditBoxText(NULL)
{
}

CDnTextInputDlg::~CDnTextInputDlg(void)
{
}

void CDnTextInputDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "TextInputDlg.ui" ).c_str(), bShow );
}

void CDnTextInputDlg::InitialUpdate()
{
	m_pTextBoxText = GetControl<CEtUITextBox>("ID_TEXTBOX");
	m_pEditBoxText = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX");
}

void CDnTextInputDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		Show(false);
	}
}

void CDnTextInputDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_pEditBoxText->ClearText();
		RequestFocus( m_pEditBoxText );
	}
}

bool CDnTextInputDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnTextInputDlg::SetText( LPCWSTR wszText )
{
	std::wstring strText(wszText);
	if( strText.empty() )
		return;

	m_pTextBoxText->ClearText();
	m_pTextBoxText->AddText( wszText );
}

LPCWSTR CDnTextInputDlg::GetText()
{
	return m_pEditBoxText->GetText();
}