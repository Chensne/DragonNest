#include "stdafx.h"
#include "DnLauncherDlgEU.h"
#include "../../DnHtmlView.h"


#if defined(_EU)

CDnLauncherDlgEU::CDnLauncherDlgEU( CWnd* pParent )
: CDnLauncherDlg( pParent )
, m_eSelectLanguage( em_NONE )
{
	m_nMaxCtrlNum = em_ExMaxNum;
	m_pRectCtrl = new CRect[em_ExMaxNum];
	m_staticGrayColor = RGB( 150, 150, 150 );
	m_staticBaseColor = RGB( 0, 0, 0 );
	m_staticOrangeColor = RGB( 255, 52, 1 );
}

void CDnLauncherDlgEU::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	// Static Text
	DDX_Text( pDX, IDC_HOMEPAGE_LINK, m_strHomepageLink );
	DDX_Text( pDX, IDC_TOTAL_PROGRESS, m_strStaticTotalProgress );
	DDX_Text( pDX, IDC_CURRENT_PROGRESS, m_strStaticCurrentProgress );
	DDX_Text( pDX, IDC_TEXT_CURRENT_INFO, m_strStaticCurrentInfo );
	DDX_Text( pDX, IDC_TEXT_FILE_NAME, m_strStaticFileName );
	DDX_Text( pDX, IDC_TEXT_FILE_SIZE, m_strStaticTextFileSize );
	DDX_Text( pDX, IDC_TEXT_TIME_LEFT, m_strStaticTextTimeLeft );
	DDX_Text( pDX, IDC_TEXT_SELECT_LANGUAGE, m_strStaticTextSelectLanguage );

	// Static Text Ctrl
	DDX_Control( pDX, IDC_TEXT_CURRENT_INFO, m_StaticCurrentInfo );
	DDX_Control( pDX, IDC_TEXT_FILE_NAME, m_StaticTextFileName );
	DDX_Control( pDX, IDC_TEXT_FILE_SIZE, m_StaticTextFileSize );
	DDX_Control( pDX, IDC_TEXT_TIME_LEFT, m_StaticTextTimeLeft );
	DDX_Control( pDX, IDC_HOMEPAGE_LINK, m_StaticHomepageLink );
	DDX_Control( pDX, IDC_TOTAL_PROGRESS, m_StaticTotalProgress );
	DDX_Control( pDX, IDC_CURRENT_PROGRESS, m_StaticCurrentProgress );
	DDX_Control( pDX, IDC_TEXT_SELECT_LANGUAGE, m_StaticTextSelectLanguage );
	DDX_Control( pDX, IDC_BTN_ENG, m_BtnENG );
	DDX_Control( pDX, IDC_BTN_FRA, m_BtnFRA );
	DDX_Control( pDX, IDC_BTN_GER, m_BtnGER );
	DDX_Control( pDX, IDC_BTN_SPA, m_BtnSPA );
}

BEGIN_MESSAGE_MAP( CDnLauncherDlgEU, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED( IDC_HOMEPAGE_LINK, &CDnLauncherDlgEU::OnClickedHomepageLink )
	ON_BN_CLICKED( IDC_BTN_ENG, &CDnLauncherDlgEU::OnClickedENG )
	ON_BN_CLICKED( IDC_BTN_FRA, &CDnLauncherDlgEU::OnClickedFRA )
	ON_BN_CLICKED( IDC_BTN_GER, &CDnLauncherDlgEU::OnClickedGER )
	ON_BN_CLICKED( IDC_BTN_SPA, &CDnLauncherDlgEU::OnClickedSPA )
END_MESSAGE_MAP()


void CDnLauncherDlgEU::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
	m_NormalFont.DeleteObject();
	m_BoldFont.DeleteObject();

	if( m_hFont )
		RemoveFontMemResourceEx( m_hFont );
}

BOOL CDnLauncherDlgEU::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
	CPoint pt;
	CRect rc;

	GetCursorPos( &pt );
	GetDlgItem( IDC_HOMEPAGE_LINK )->GetWindowRect( rc );

	if( rc.PtInRect( pt ) )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor( MAKEINTRESOURCE( IDC_HAND ) ) );
		return TRUE;
	}

	return CDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CDnLauncherDlgEU::MakeFont()
{
	HINSTANCE hResInstance = AfxGetResourceHandle( );

	HRSRC res = FindResource( hResInstance, MAKEINTRESOURCE( IDR_FONT ), L"BINARY" );
	if( res )
	{
		HGLOBAL mem = LoadResource( hResInstance, res );
		void *data = LockResource( mem );
		size_t len = SizeofResource( hResInstance, res );

		DWORD nFonts;
		m_hFont = AddFontMemResourceEx( data, len, NULL, &nFonts );
	}

	if( !m_NormalFont.CreateFont( 13, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Handel Gothic ITC Std Medium") ) )
		m_NormalFont.CreateFont( 15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("arial") );

	if( !m_BoldFont.CreateFont( 12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Handel Gothic ITC Std Medium") ) )
		m_BoldFont.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("arial") );
}

void CDnLauncherDlgEU::SetStaticText()
{
	CDnLauncherDlg::SetStaticText();

	m_StaticLauncherVersion.SetTextColor( m_staticGrayColor );
	m_StaticLauncherVersion.SetFont( &m_NormalFont );

	m_StaticHomepageLink.SetTextColor( m_staticBaseColor );
	m_StaticHomepageLink.SetFont( &m_BoldFont );
	m_StaticHomepageLink.MoveWindow( m_pRectCtrl[em_StaticText_HomepageLink] );
	m_StaticHomepageLink.SetWindowText( _S( STR_HOMEPAGE_LINK + DNPATCHINFO.GetLanguageOffset() ) );

	m_StaticTotalProgress.SetTextColor( m_staticGrayColor );
	m_StaticTotalProgress.SetFont( &m_NormalFont );
	m_StaticTotalProgress.MoveWindow( m_pRectCtrl[em_StaticText_TotalProgress] );
	m_StaticTotalProgress.SetWindowText( _S( STR_TOTAL_PATCH + DNPATCHINFO.GetLanguageOffset() ) );

	m_StaticCurrentProgress.SetTextColor( m_staticGrayColor );
	m_StaticCurrentProgress.SetFont( &m_NormalFont );
	m_StaticCurrentProgress.MoveWindow( m_pRectCtrl[em_StaticText_CurrentProgress] );
	m_StaticCurrentProgress.SetWindowText( _S( STR_CURRENT_PATCH + DNPATCHINFO.GetLanguageOffset() ) );

	m_StaticTextSelectLanguage.SetTextColor( m_staticGrayColor );
	m_StaticTextSelectLanguage.SetFont( &m_NormalFont );
	m_StaticTextSelectLanguage.MoveWindow( m_pRectCtrl[em_StaticText_SelectLanguage] );
	m_StaticTextSelectLanguage.SetWindowText( _S( STR_SELECT_LANGUAGE + DNPATCHINFO.GetLanguageOffset() ) );

	CString strText;
	m_StaticCurrentInfo.SetTextColor( m_staticGrayColor );
	m_StaticCurrentInfo.SetFont( &m_NormalFont );
	m_StaticCurrentInfo.MoveWindow( m_pRectCtrl[em_StaticText_CurrentInfo] );
	strText.Format( _T("%s :"), _S( STR_PATCH_CURRENT_INFO + DNPATCHINFO.GetLanguageOffset() ) );
	m_StaticCurrentInfo.SetWindowText( strText );

	m_StaticTextFileName.SetTextColor( m_staticGrayColor );
	m_StaticTextFileName.SetFont( &m_NormalFont );
	m_StaticTextFileName.MoveWindow( m_pRectCtrl[em_StaticText_FileName] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILENAME + DNPATCHINFO.GetLanguageOffset() ) );
	m_StaticTextFileName.SetWindowText( strText );

	m_StaticTextFileSize.SetTextColor( m_staticGrayColor );
	m_StaticTextFileSize.SetFont( &m_NormalFont );
	m_StaticTextFileSize.MoveWindow( m_pRectCtrl[em_StaticText_FileSize] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILE_SIZE + DNPATCHINFO.GetLanguageOffset() ) );
	m_StaticTextFileSize.SetWindowText( strText );

	m_StaticTextTimeLeft.SetTextColor( m_staticGrayColor );
	m_StaticTextTimeLeft.SetFont( &m_NormalFont );
	m_StaticTextTimeLeft.MoveWindow( m_pRectCtrl[em_StaticText_TimeLeft] );
	strText.Format( _T("%s :"), _S( STR_DLG_DOWNLOAD_TIME_LEFT + DNPATCHINFO.GetLanguageOffset() ) );
	m_StaticTextTimeLeft.SetWindowText( strText );

	m_StaticFileName.SetFont( &m_BoldFont );
	m_StaticFileSize.SetFont( &m_BoldFont );
	m_StaticTimeLeft.SetFont( &m_BoldFont );
	m_StaticFileCount.SetFont( &m_BoldFont );
	m_StaticDownloadPercentage.SetTextColor( m_staticOrangeColor );
	m_StaticDownloadPercentage.SetFont( &m_BoldFont );
	m_StaticDownloadState.SetFont( &m_BoldFont );
	m_StaticDirectGameStart.SetFont( &m_BoldFont );
}

BOOL CDnLauncherDlgEU::InitWebpage()
{
	BOOL bRtn = CDnLauncherDlg::InitWebpage();
	m_pDnHtmlView->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED );

	return bRtn;
}

BOOL CDnLauncherDlgEU::InitBackground()
{
	CxImage xImage;
	if( !xImage.LoadResource( FindResource( NULL, MAKEINTRESOURCE( IDR_JPG_BACK_GROUND ), L"PNG" ), CXIMAGE_FORMAT_PNG ) )
		return FALSE;

	HBITMAP hBmp = xImage.MakeBitmap();
	if( hBmp == NULL )
		return FALSE;

	// set window size
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	m_dwWidth	= bmp.bmWidth;		// bitmap width
	m_dwHeight	= bmp.bmHeight;		// bitmap height
	// alpha color 
	HRGN hRgn = CreateRgnFromFile( hBmp, RGB( 255, 0, 255 ) );
	// build memory dc for background
	CDC* dc = GetDC();
	m_dcBkGrnd = CreateCompatibleDC( dc->m_hDC );
	ReleaseDC( dc );
	// select background image
	SelectObject( m_dcBkGrnd, hBmp );
	// set window size the same as image size
	//SetWindowPos( NULL, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE );
	SetWindowPos( &wndTopMost, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
	::SetForegroundWindow( this->GetSafeHwnd() );
	::SetActiveWindow( this->GetSafeHwnd() );
	// assign region to window
	SetWindowRgn( hRgn, FALSE );

	return TRUE;
}

void CDnLauncherDlgEU::InitControl()
{
	CDnLauncherDlg::InitControl();

	m_BtnENG.SetSkinJpg( IDR_JPG_ENG, 0, IDR_JPG_ENG_ON, IDR_JPG_ENG_CLICK );
	m_BtnENG.MoveWindow( m_pRectCtrl[em_BtnENG] );
	m_BtnENG.SetWindowText( L"" );

	m_BtnGER.SetSkinJpg( IDR_JPG_GER, 0, IDR_JPG_GER_ON, IDR_JPG_GER_CLICK );
	m_BtnGER.MoveWindow( m_pRectCtrl[em_BtnGER] );
	m_BtnGER.SetWindowText( L"" );

	m_BtnFRA.SetSkinJpg( IDR_JPG_FRA, 0, IDR_JPG_FRA_ON, IDR_JPG_FRA_CLICK );
	m_BtnFRA.MoveWindow( m_pRectCtrl[em_BtnFRA] );
	m_BtnFRA.SetWindowText( L"" );

	m_BtnSPA.SetSkinJpg( IDR_JPG_SPA, 0, IDR_JPG_SPA_ON, IDR_JPG_SPA_CLICK );
	m_BtnSPA.MoveWindow( m_pRectCtrl[em_BtnSPA] );
	m_BtnSPA.SetWindowText( L"" );

	if( DNPATCHINFO.GetLanguageParam() == L"ENG" )
		SetSelectLanguageIndex( em_ENG );
	else if( DNPATCHINFO.GetLanguageParam() == L"GER" )
		SetSelectLanguageIndex( em_GER );
	else if( DNPATCHINFO.GetLanguageParam() == L"FRA" )
		SetSelectLanguageIndex( em_FRA );
	else if( DNPATCHINFO.GetLanguageParam() == L"ESP" )
		SetSelectLanguageIndex( em_ESP );
}

void CDnLauncherDlgEU::OnClickedHomepageLink()
{
	GetDlgItem( IDC_HOMEPAGE_LINK )->Invalidate();
	ShellExecute( m_hWnd, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
}

void CDnLauncherDlgEU::OnClickedENG()
{
	SetSelectLanguageIndex( em_ENG );
}

void CDnLauncherDlgEU::OnClickedFRA()
{
	SetSelectLanguageIndex( em_FRA );
}

void CDnLauncherDlgEU::OnClickedGER()
{
	SetSelectLanguageIndex( em_GER );
}

void CDnLauncherDlgEU::OnClickedSPA()
{
	SetSelectLanguageIndex( em_ESP );
}

void CDnLauncherDlgEU::SetSelectLanguageIndex( emLanguage eLanguage )
{
	if( m_eSelectLanguage == eLanguage )
		return;

	m_BtnENG.EnableWindow( true );
	m_BtnGER.EnableWindow( true );
	m_BtnFRA.EnableWindow( true );
	m_BtnSPA.EnableWindow( true );

	switch( eLanguage )
	{
		case em_ENG:
			{
				m_BtnENG.EnableWindow( false );
				DNPATCHINFO.SetLanguageParam( L"ENG" );
				DNOPTIONDATA.SetLanguageID( LANG_ENGLISH );
			}
			break;
		case em_GER:
			{
				m_BtnGER.EnableWindow( false );
				DNPATCHINFO.SetLanguageParam( L"GER" );
				DNOPTIONDATA.SetLanguageID( LANG_GERMAN );
			}
			break;
		case em_FRA:
			{
				m_BtnFRA.EnableWindow( false );
				DNPATCHINFO.SetLanguageParam( L"FRA" );
				DNOPTIONDATA.SetLanguageID( LANG_FRENCH );
			}
			break;
		case em_ESP:
			{
				m_BtnSPA.EnableWindow( false );
				DNPATCHINFO.SetLanguageParam( L"ESP" );
				DNOPTIONDATA.SetLanguageID( LANG_SPANISH );
			}
			break;
	}

	DNPATCHINFO.SetLocaleGuidePage();
	m_eSelectLanguage = eLanguage;
	NavigateHtml();
	DNOPTIONDATA.SaveConfigOption( this->m_hWnd );

	ResetInitCtrlRect();
	GetCtrlRect( m_pRectCtrl, m_nMaxCtrlNum );
 
	m_StaticLauncherVersion.MoveWindow( m_pRectCtrl[em_Text_LauncherVersion] );
	m_StaticFileName.MoveWindow( m_pRectCtrl[em_Text_FileName] );
	m_StaticFileSize.MoveWindow( m_pRectCtrl[em_Text_FileSize] );
	m_StaticTimeLeft.MoveWindow( m_pRectCtrl[em_Text_TimeLeft] );
	m_StaticFileCount.MoveWindow( m_pRectCtrl[em_Text_FileCount] );
	m_StaticDownloadPercentage.MoveWindow( m_pRectCtrl[em_Text_DownloadPercentage] );
	m_StaticDownloadState.MoveWindow( m_pRectCtrl[em_Text_DownloadState] );
	m_StaticDirectGameStart.MoveWindow( m_pRectCtrl[em_Text_DirectStart] );

	m_StaticHomepageLink.MoveWindow( m_pRectCtrl[em_StaticText_HomepageLink] );
	m_StaticTotalProgress.MoveWindow( m_pRectCtrl[em_StaticText_TotalProgress] );
	m_StaticCurrentProgress.MoveWindow( m_pRectCtrl[em_StaticText_CurrentProgress] );
	m_StaticCurrentInfo.MoveWindow( m_pRectCtrl[em_StaticText_CurrentInfo] );
	m_StaticTextFileName.MoveWindow( m_pRectCtrl[em_StaticText_FileName] );
	m_StaticTextFileSize.MoveWindow( m_pRectCtrl[em_StaticText_FileSize] );
	m_StaticTextTimeLeft.MoveWindow( m_pRectCtrl[em_StaticText_TimeLeft] );
}

void CDnLauncherDlgEU::RefreshTextInfo()
{
	m_strDirectGameStart = _S( STR_AFTER_UPDATE_ATUOSTART + DNPATCHINFO.GetLanguageOffset() );
	m_strHomepageLink = _S( STR_HOMEPAGE_LINK + DNPATCHINFO.GetLanguageOffset() );
	m_strStaticTotalProgress = _S( STR_TOTAL_PATCH + DNPATCHINFO.GetLanguageOffset() );
	m_strStaticCurrentProgress = _S( STR_CURRENT_PATCH + DNPATCHINFO.GetLanguageOffset() );
	m_strStaticCurrentInfo.Format( _T("%s :"), _S( STR_PATCH_CURRENT_INFO + DNPATCHINFO.GetLanguageOffset() ) );
	m_strStaticFileName.Format( _T("%s :"), _S( STR_DLG_FILENAME + DNPATCHINFO.GetLanguageOffset() ) );
	m_strStaticTextFileSize.Format( _T("%s :"), _S( STR_DLG_FILE_SIZE + DNPATCHINFO.GetLanguageOffset() ) );
	m_strStaticTextTimeLeft.Format( _T("%s :"), _S( STR_DLG_DOWNLOAD_TIME_LEFT + DNPATCHINFO.GetLanguageOffset() ) );
	m_strStaticTextSelectLanguage = _S( STR_SELECT_LANGUAGE + DNPATCHINFO.GetLanguageOffset() );

	CString str;
	GetDlgItem( IDC_DIRECT_START )->GetWindowText( str );
	if( str != m_strDirectGameStart ) CtlEraseBkgnd( IDC_DIRECT_START );
	GetDlgItem( IDC_HOMEPAGE_LINK )->GetWindowText( str );
	if( str != m_strHomepageLink ) CtlEraseBkgnd( IDC_HOMEPAGE_LINK );
	GetDlgItem( IDC_TOTAL_PROGRESS )->GetWindowText( str );
	if( str != m_strStaticTotalProgress ) CtlEraseBkgnd( IDC_TOTAL_PROGRESS );
	GetDlgItem( IDC_CURRENT_PROGRESS )->GetWindowText( str );
	if( str != m_strStaticCurrentProgress ) CtlEraseBkgnd( IDC_CURRENT_PROGRESS );
	GetDlgItem( IDC_TEXT_CURRENT_INFO )->GetWindowText( str );
	if( str != m_strStaticCurrentInfo ) CtlEraseBkgnd( IDC_TEXT_CURRENT_INFO );
	GetDlgItem( IDC_TEXT_FILE_NAME )->GetWindowText( str );
	if( str != m_strStaticFileName ) CtlEraseBkgnd( IDC_TEXT_FILE_NAME );
	GetDlgItem( IDC_TEXT_FILE_SIZE )->GetWindowText( str );
	if( str != m_strStaticTextFileSize ) CtlEraseBkgnd( IDC_TEXT_FILE_SIZE );
	GetDlgItem( IDC_TEXT_TIME_LEFT )->GetWindowText( str );
	if( str != m_strStaticTextTimeLeft ) CtlEraseBkgnd( IDC_TEXT_TIME_LEFT );
	GetDlgItem( IDC_TEXT_SELECT_LANGUAGE )->GetWindowText( str );
	if( str != m_strStaticTextSelectLanguage ) CtlEraseBkgnd( IDC_TEXT_SELECT_LANGUAGE );

	m_BtnCancel.SetSkinJpg( IDR_JPG_CANCEL + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CANCEL_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CANCEL_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnCancel.Invalidate( TRUE );

	m_BtnStartGame.SetSkinJpg( IDR_JPG_GAMESTART + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_GAMESTART_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_GAMESTART_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnStartGame.Invalidate( TRUE );

	CDnLauncherDlg::RefreshTextInfo();
}

#endif // _EU