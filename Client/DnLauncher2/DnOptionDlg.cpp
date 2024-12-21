#include "stdafx.h"
#include "DnOptionDlg.h"
#include "DnLauncherDlg.h"


CDnOptionDlg::CDnOptionDlg( CWnd* pParent )
: CDialog( CDnOptionDlg::IDD, pParent )
{
}

void CDnOptionDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP(CDnOptionDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CtestDlg message handlers

BOOL CDnOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CDnLauncherDlg* pLauncherDlg = static_cast<CDnLauncherDlg*>( GetParent() );

	DNOPTIONDATA.LoadConfigOption( this->m_hWnd );

	CStatic* pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_GAME_OPTION ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_GAME_OPTION + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_RESOLUTION ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_RESOLUTION + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_GRAPHIC_QUALITY ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_GRAPHIC_QUALITY + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_TEXTURE_QUALITY ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_TEXTURE_QUALITY + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_WINDOW_MODE ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_WINDOWS_MODE + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_VSYNC_MODE ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_VSYNC_MODE + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_BUTTON_OK ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_APPLY + DNPATCHINFO.GetLanguageOffset() ) );
	pStatc = static_cast<CStatic*>( GetDlgItem( IDC_OPTION_BUTTON_CANCEL ) );
	if( pStatc )
		pStatc->SetWindowText( _S( STR_OPTION_CANCEL + DNPATCHINFO.GetLanguageOffset() ) );

	//기존 파일의 해상도를 얻는다
	WCHAR wszOldRes[20]={0,};
	for( int i=0; i<(int)pLauncherDlg->m_vecDisplayMode.size(); i++ )
	{
		if(  pLauncherDlg->m_vecDisplayMode[i].x == DNOPTIONDATA.m_nWidth 
			&& pLauncherDlg->m_vecDisplayMode[i].y == DNOPTIONDATA.m_nHeight )
		{
			wsprintf( wszOldRes, L"%d X %d", pLauncherDlg->m_vecDisplayMode[i].x, pLauncherDlg->m_vecDisplayMode[i].y );
		}
	}

	CComboBox* pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_RESOLUTION_COMBO ) );
	if( pComboBox )
	{
		for(int i=0; i<(int)pLauncherDlg->m_vecDisplayMode.size(); i++ )
		{
			WCHAR wszRes[20];
			wsprintf( wszRes, L"%d X %d", pLauncherDlg->m_vecDisplayMode[i].x, pLauncherDlg->m_vecDisplayMode[i].y );
			pComboBox->AddString( wszRes );
		}
		pComboBox->SelectString( 0, wszOldRes );
	}

	pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_GRAPHICQ_COMBO ) );
	if( pComboBox )
	{
		pComboBox->AddString( _S( STR_LOW + DNPATCHINFO.GetLanguageOffset() ) );
		pComboBox->AddString( _S( STR_MID + DNPATCHINFO.GetLanguageOffset() ) );
		pComboBox->AddString( _S( STR_HIGH + DNPATCHINFO.GetLanguageOffset() ) );

		if( DNOPTIONDATA.m_nGraphicQuality == 0 )
			pComboBox->SetCurSel( 2 );
		else if( DNOPTIONDATA.m_nGraphicQuality == 1)
			pComboBox->SetCurSel( 1 );
		else if( DNOPTIONDATA.m_nGraphicQuality == 2 )
			pComboBox->SetCurSel( 0 );
	}

	pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_TEXTUREQ_COMBO ) );
	if( pComboBox )
	{
		pComboBox->AddString( _S( STR_LOW + DNPATCHINFO.GetLanguageOffset() ) );
		pComboBox->AddString( _S( STR_HIGH + DNPATCHINFO.GetLanguageOffset() ) );

		if( DNOPTIONDATA.m_nTextureQuality == 0 )
			pComboBox->SetCurSel( 1 );
		else if( DNOPTIONDATA.m_nTextureQuality == 1)
			pComboBox->SetCurSel( 0 );
	}

	CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_WINDOWED_CHECK ) );
	if( pButton )
	{
		if( DNOPTIONDATA.m_bWindow )
			pButton->SetCheck( TRUE );
		else
			pButton->SetCheck( FALSE );
	}

	pButton = static_cast<CButton*>( GetDlgItem( IDC_VSYNC_CHECK ) );
	if( pButton )
	{
		if( DNOPTIONDATA.m_bVSync )
			pButton->SetCheck( TRUE );
		else
			pButton->SetCheck( FALSE );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CDnOptionDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{
	switch ( LOWORD( wParam ) )
	{
	case IDC_WINDOWED_CHECK:
		{
			CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_WINDOWED_CHECK ) );
			if( pButton )
				DNOPTIONDATA.m_bWindow = pButton->GetCheck();
		}
		break;
	case IDC_VSYNC_CHECK:
		{
			CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_VSYNC_CHECK ) );
			if( pButton )
				DNOPTIONDATA.m_bVSync = pButton->GetCheck();
		}
		break;
	case IDC_OPTION_BUTTON_OK:
		DNOPTIONDATA.SaveConfigOption( this->m_hWnd );
		EndDialog( TRUE );
		break;
	case IDC_OPTION_BUTTON_CANCEL:
		EndDialog( FALSE );
		break;
	case IDC_GRAPHICQ_COMBO:
		{
			if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				CComboBox* pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_GRAPHICQ_COMBO ) );
				if( pComboBox )
				{
					int nItemCur = pComboBox->GetCurSel();
					if( nItemCur == 0 )
						DNOPTIONDATA.m_nGraphicQuality = 2;
					else if( nItemCur == 1)
						DNOPTIONDATA.m_nGraphicQuality = 1;
					else if( nItemCur == 2 )
						DNOPTIONDATA.m_nGraphicQuality = 0;
				}
			}
		}				
		break;
	case IDC_RESOLUTION_COMBO:
		{
			if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				CDnLauncherDlg* pLauncherDlg = static_cast<CDnLauncherDlg*>( GetParent() );
				CComboBox* pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_RESOLUTION_COMBO ) );
				if( pComboBox )
				{
					int nItemCur = pComboBox->GetCurSel();
					if( ( nItemCur >= 0) && ( nItemCur < static_cast<int>( pLauncherDlg->m_vecDisplayMode.size() ) ) )
					{
						DNOPTIONDATA.m_nWidth = pLauncherDlg->m_vecDisplayMode[nItemCur].x;
						DNOPTIONDATA.m_nHeight = pLauncherDlg->m_vecDisplayMode[nItemCur].y;
					}
				}
			}
		}
		break;
	case IDC_TEXTUREQ_COMBO:
		{
			if( HIWORD( wParam ) == CBN_SELCHANGE )
			{
				CComboBox* pComboBox = static_cast<CComboBox*>( GetDlgItem( IDC_TEXTUREQ_COMBO ) );
				if( pComboBox )
				{
					int nItemCur = pComboBox->GetCurSel();
					if( nItemCur == 0 )
						DNOPTIONDATA.m_nTextureQuality = 1;
					else if( nItemCur == 1)
						DNOPTIONDATA.m_nTextureQuality = 0;
				}
			}
		}				
		break;
	}

	return CDialog::OnCommand( wParam, lParam );
}	
