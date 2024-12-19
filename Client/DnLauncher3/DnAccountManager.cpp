#include "stdafx.h"
#include "DnAccountManager.h"
#include "DnLauncherDlg.h"
#include "Settings.h"
#include "RLKTAuth.h"
#include "base64.h"
#ifdef _SKY
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
		pStatc->SetWindowText( GetWC(data.Username) );
	//Insert password
	pStatc = static_cast<CStatic*>( GetDlgItem( ID_PASSWORD ) );
	if( pStatc )
		pStatc->SetWindowText(GetWC(data.Password));
	
	//
	CButton* pButton = static_cast<CButton*>( GetDlgItem( LoadUI ) );
	if( pButton )
	{
		if( data.ShowNewUI )
			pButton->SetCheck( TRUE );
		else
			pButton->SetCheck( FALSE );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CDnAccountManagerDlg::OnClose()
{
	MessageBoxA(NULL, "Close", "CLOSED!", MB_OK);
	ExitProcess(0);
}

BOOL CDnAccountManagerDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch ( LOWORD( wParam ) )
	{
	case LoadUI:
		{
			CButton* pButton = static_cast<CButton*>( GetDlgItem( LoadUI ) );
			if( pButton )
				data.ShowNewUI = pButton->GetCheck();
		}
		break;
	case ID_PLAY:
		//save Account + Password
		CStatic* pStatc;
		wchar_t *user = new wchar_t[32];
		wchar_t *pass = new wchar_t[32];
		pStatc = static_cast<CStatic*>( GetDlgItem( ID_USERNAME ) );
		pStatc->GetWindowTextW(user,32);
		strcpy(data.Username, GetC(user));

		pStatc = static_cast<CStatic*>( GetDlgItem( ID_PASSWORD ) );
		pStatc->GetWindowTextW(pass,32);
		strcpy(data.Password,GetC(pass));
		//
#ifdef _SKY
			char buffData[128];
			wsprintfA(buffData, "login&%ws&%ws&5", user, pass);
			std::string logindata = buffData;

			bool result = gHTTP.CheckLogin(base64_encode((const unsigned char*)logindata.c_str(),logindata.size()), logindata.size());
			if(result == false)
			{
				return false;
			}
#endif
		//set new parameter
		DNPATCHINFO.SetParameter();

		//Save Config
		Settings::GetInstance().SaveSettings();
		EndDialog( TRUE );
	break;
	}

	return CDialog::OnCommand( wParam, lParam );
}	
#endif