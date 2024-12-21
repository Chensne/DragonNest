#include "stdafx.h"
#include "DnAccountManager.h"
#include "DnLauncherDlg.h"


CDnAccountManagerDlg::CDnAccountManagerDlg( CWnd* pParent )
: CDialog( CDnAccountManagerDlg::IDD, pParent )
{
}

void CDnAccountManagerDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP(CDnAccountManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


const wchar_t *GetWC(const char *c)
{
    const size_t cSize = strlen(c)+1;
    wchar_t wc[512];
    mbstowcs (wc, c, cSize);

    return wc;
}

const char *GetC(const wchar_t *wc)
{
	 char output[256];
	//const WCHAR* wc = L"Hellow World" ;
	sprintf(output, "%ws", wc );
	return output;
}
// CtestDlg message handlers

BOOL CDnAccountManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CDnLauncherDlg* pLauncherDlg = static_cast<CDnLauncherDlg*>( GetParent() );

	DNOPTIONDATA.LoadConfigOption( this->m_hWnd );

	CStatic* pStatc;// = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_GAME_OPTION ) );
	//Insert username
	pStatc = static_cast<CStatic*>( GetDlgItem( ID_USERNAME ) );
	if( pStatc )
		pStatc->SetWindowText( GetWC(DNOPTIONDATA.username) );
	//Insert password
	pStatc = static_cast<CStatic*>( GetDlgItem( ID_PASSWORD ) );
	if( pStatc )
		pStatc->SetWindowText( GetWC(DNOPTIONDATA.password) );

	//
	CButton* pButton = static_cast<CButton*>( GetDlgItem( LoadUI ) );
	if( pButton )
	{
		if( DNOPTIONDATA.m_bNewUI )
			pButton->SetCheck( TRUE );
		else
			pButton->SetCheck( FALSE );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CDnAccountManagerDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch ( LOWORD( wParam ) )
	{
	case LoadUI:
		{
			CButton* pButton = static_cast<CButton*>( GetDlgItem( LoadUI ) );
			if( pButton )
				DNOPTIONDATA.m_bNewUI = pButton->GetCheck();
		}
		break;
	case ID_PLAY:
		//save Account + Password
		CStatic* pStatc;
		wchar_t *user = new wchar_t[32];
		wchar_t *pass = new wchar_t[32];
		pStatc = static_cast<CStatic*>( GetDlgItem( ID_USERNAME ) );
		pStatc->GetWindowTextW(user,32);
		strcpy(DNOPTIONDATA.username,GetC(user));

		pStatc = static_cast<CStatic*>( GetDlgItem( ID_PASSWORD ) );
		pStatc->GetWindowTextW(pass,32);
		strcpy(DNOPTIONDATA.password,GetC(pass));
		//set new parameter
		DNPATCHINFO.SetParameter();

		//Save Config
		DNOPTIONDATA.SaveConfigOption( this->m_hWnd );
		EndDialog( TRUE );
	break;
	}

	return CDialog::OnCommand( wParam, lParam );
}	
