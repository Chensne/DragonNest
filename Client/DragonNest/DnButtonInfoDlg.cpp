#include "StdAfx.h"
#include "DnButtonInfoDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnButtonInfoDlg::CDnButtonInfoDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pTextBoxInfo(NULL)
{
}

CDnButtonInfoDlg::~CDnButtonInfoDlg(void)
{
}

void CDnButtonInfoDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SingleLineInfoDlg.ui" ).c_str(), bShow );
}

void CDnButtonInfoDlg::InitialUpdate()
{
	m_pTextBoxInfo = GetControl<CEtUITextBox>("ID_TEXTBOX");
}

void CDnButtonInfoDlg::SetInfo( LPCWSTR szwInfo, LPCWSTR szwKey )
{
	wchar_t szTemp[256]={0};
	swprintf_s( szTemp, 256, L"%s[%s]", szwInfo, szwKey );
	m_pTextBoxInfo->ClearText();
	m_pTextBoxInfo->AddText( szTemp, textcolor::GOLD );
}