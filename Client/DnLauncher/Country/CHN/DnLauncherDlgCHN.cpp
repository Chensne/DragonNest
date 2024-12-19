#include "stdafx.h"
#include "DnLauncherDlgCHN.h"
#include "../../DnExplorerDlg.h"

#if defined(_CHN)


CDnLauncherDlgCHN::CDnLauncherDlgCHN( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_ExMaxNum;
	m_pRectCtrl = new CRect[em_ExMaxNum];
	m_staticColor = RGB( 170, 180, 188 );
	m_ColorPercentage = RGB( 179, 255, 0 );
}

void CDnLauncherDlgCHN::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	// Static Text Ctrl
	DDX_Control( pDX, IDC_TEXT_CURRENT_INFO, m_StaticCurrentInfo );
	DDX_Control( pDX, IDC_TEXT_FILE_NAME, m_StaticTextFileName );
	DDX_Control( pDX, IDC_TEXT_FILE_SIZE, m_StaticTextFileSize );
	DDX_Control( pDX, IDC_TEXT_TIME_LEFT, m_StaticTextTimeLeft );
	DDX_Control( pDX, IDC_HOMEPAGE_LINK, m_StaticHomepageLink );
	DDX_Control( pDX, IDC_TOTAL_FILE, m_StaticTotalFile );
	DDX_Control( pDX, IDC_CURRENT_FILE, m_StaticCurrentFile );
}

BEGIN_MESSAGE_MAP( CDnLauncherDlgCHN, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED( IDC_BTN_GAMESTART, &CDnLauncherDlgCHN::OnClickButtonGameStart )
	ON_BN_CLICKED( IDC_BTN_CLOSE, &CDnLauncherDlgCHN::OnClickButtonClose )
	ON_BN_CLICKED( IDC_HOMEPAGE_LINK, &CDnLauncherDlgCHN::OnClickedHomepageLink )
END_MESSAGE_MAP()

BOOL CDnLauncherDlgCHN::OnInitDialog()
{
	UpdateRegister();
	return CDnLauncherDlg::OnInitDialog();
}

void CDnLauncherDlgCHN::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
	m_15Font.DeleteObject();
	m_14FontStatic.DeleteObject();
	m_15FontStatic.DeleteObject();
}

BOOL CDnLauncherDlgCHN::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
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

void CDnLauncherDlgCHN::MakeFont()
{
	if( !m_Font.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("MYSH") ) )
		m_Font.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("simsun") );

	if( !m_15Font.CreateFont( 15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("MYSH") ) )
		m_15Font.CreateFont( 15, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("simsun") );

	if( !m_14FontStatic.CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("MYSH") ) )
		m_14FontStatic.CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("simsun") );

	if( !m_15FontStatic.CreateFont( 15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("MYSH") ) )
		m_15FontStatic.CreateFont( 15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("simsun") );
}

void CDnLauncherDlgCHN::InitControl()
{
	CDnLauncherDlg::InitControl();

	m_BtnGameOption.ShowWindow( SW_SHOW );
}

void CDnLauncherDlgCHN::OnClickButtonGameStart()
{
	if( DNPATCHINFO.GetPartitionOpen() == 0 )
	{
		CDnRestrictiveGuideDlg RestrictiveGuideDlg( this );
		RestrictiveGuideDlg.SetURL( L"http://dn.sdo.com/project/launcher/news.html" );

		if( RestrictiveGuideDlg.DoModal() == WM_CLOSE_LAUNCHER )	// 종료요청이 왔을 경우 런처 종료
			EndDialog( IDOK );

		return;
	}

	CDnLauncherDlg::OnClickButtonGameStart();
}

void CDnLauncherDlgCHN::OnClickButtonClose()
{
	int nResult = AfxMessageBox( STR_PATCH_CLOSE, MB_YESNO );

	if( nResult == IDYES )
	{
		LogWnd::TraceLog(_T("★ Launcher Closed"));
		CDialog::OnCancel();
	}
}

void CDnLauncherDlgCHN::CheckVersion()
{
	CDnLauncherDlg::CheckVersion();

	m_BtnGameOption.ShowWindow( SW_SHOW );
}

void CDnLauncherDlgCHN::UpdateRegister()
{
	//-- 레지스트리가 등록되어있는 경로를 찾는다.
	const TCHAR RegClientPath[256] = _T( "SOFTWARE\\SNDA\\DN" );

	HKEY h_Key = NULL;
	int ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, RegClientPath, 0, KEY_ALL_ACCESS, &h_Key );

	///-- 현재 프로그램이 실행되고 있는 모듈경로를 찾는다.
	TCHAR  strClientRoot[MAX_PATH] =  {0,};
	GetModuleFileName( NULL, strClientRoot, sizeof( strClientRoot ) );

	TCHAR aFilePath[MAX_PATH] = {0,};
	TCHAR aFilePath2[MAX_PATH] = {0,};
	TCHAR aFileName[MAX_PATH] = {0,};
	TCHAR aFileEx[MAX_PATH] = {0,};

	if( ret == ERROR_FILE_NOT_FOUND )
	{
		///-- 레지스트리 등록 경로가 존재하지 않으면...
		///-- 레지스트리 등록 경로를 생성한다.
		RegCreateKeyEx( HKEY_LOCAL_MACHINE, RegClientPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &h_Key, NULL );

		if( h_Key )
		{
			DWORD dSize = sizeof( strClientRoot );
			RegSetValueEx( h_Key, _T("Loader"), 0, (DWORD)REG_SZ, (LPBYTE)strClientRoot, dSize );
		}

		AfxMessageBox( STR_UPDATE_REGISTRE, MB_OK );
	}

	if( h_Key )
	{
		//-- 레지스트리에 저장된 모듈파일의 이름을 불러온다.
		TCHAR aBuffer[MAX_PATH] = {0,};
		DWORD dSize = sizeof( aBuffer );
		DWORD dDataType = 0;
		int ret = RegQueryValueEx( h_Key, _T("Loader"), 0, &dDataType, (LPBYTE)aBuffer, &dSize );

		//-- 실행모듈과 레지스트리에 저장된 값이 틀릴경우
		//-- 레지스트리에 다시 저장한다.
		if( _tcsicmp( strClientRoot, aBuffer ) != 0 )
		{
			DWORD dSize = sizeof( strClientRoot );
			RegSetValueEx( h_Key, _T("Loader"), 0, (DWORD)REG_SZ, (LPBYTE)strClientRoot, dSize );
			AfxMessageBox( STR_UPDATE_CHANGE_PATH );	
		}
	}

	if( h_Key != NULL )
		RegCloseKey( h_Key );
}

void CDnLauncherDlgCHN::SetStaticText()
{
	CDnLauncherDlg::SetStaticText();

	m_StaticLauncherVersion.SetTextColor( m_staticColor );
	m_StaticLauncherVersion.SetFont( &m_14FontStatic );
	m_StaticFileName.SetTextColor( m_staticColor );
	m_StaticFileName.SetFont( &m_15FontStatic );
	m_StaticDirectGameStart.SetTextColor( m_staticColor );
	m_StaticDirectGameStart.SetFont( &m_14FontStatic );
	m_StaticDownloadPercentage.SetTextColor( m_ColorPercentage );
	m_StaticDownloadPercentage.SetFont( &m_15Font );
	m_StaticDownloadState.SetFont( &m_15Font );

	m_StaticSelectPartitionName.SetTextColor( m_ColorPercentage );

	CString strText;
	m_StaticCurrentInfo.SetTextColor( m_staticColor );
	m_StaticCurrentInfo.SetFont( &m_15FontStatic );
	m_StaticCurrentInfo.MoveWindow( m_pRectCtrl[em_StaticText_CurrentInfo] );
	strText.Format( _T("%s :"), _S( STR_PATCH_CURRENT_INFO ) );
	m_StaticCurrentInfo.SetWindowText( strText );

	m_StaticTextFileName.SetTextColor( m_staticColor );
	m_StaticTextFileName.SetFont( &m_15FontStatic );
	m_StaticTextFileName.MoveWindow( m_pRectCtrl[em_StaticText_FileName] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILENAME ) );
	m_StaticTextFileName.SetWindowText( strText );

	m_StaticTextFileSize.SetTextColor( m_staticColor );
	m_StaticTextFileSize.SetFont( &m_14FontStatic );
	m_StaticTextFileSize.MoveWindow( m_pRectCtrl[em_StaticText_FileSize] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILE_SIZE ) );
	m_StaticTextFileSize.SetWindowText( strText );

	m_StaticTextTimeLeft.SetTextColor( m_staticColor );
	m_StaticTextTimeLeft.SetFont( &m_14FontStatic );
	m_StaticTextTimeLeft.MoveWindow( m_pRectCtrl[em_StaticText_TimeLeft] );
	strText.Format( _T("%s :"), _S( STR_DLG_DOWNLOAD_TIME_LEFT ) );
	m_StaticTextTimeLeft.SetWindowText( strText );

	m_StaticHomepageLink.SetTextColor( m_staticColor );
	m_StaticHomepageLink.SetFont( &m_14FontStatic );
	m_StaticHomepageLink.MoveWindow( m_pRectCtrl[em_StaticText_HomepageLink] );
	m_StaticHomepageLink.SetWindowText( _S( STR_HOMEPAGE_LINK ) );

	m_StaticTotalFile.SetTextColor( m_staticColor );
	m_StaticTotalFile.SetFont( &m_14FontStatic );
	m_StaticTotalFile.MoveWindow( m_pRectCtrl[em_StaticText_TotalFile] );
	m_StaticTotalFile.SetWindowText( _S( STR_PATCH_TOTAL_FILE ) );

	m_StaticCurrentFile.SetTextColor( m_staticColor );
	m_StaticCurrentFile.SetFont( &m_14FontStatic );
	m_StaticCurrentFile.MoveWindow( m_pRectCtrl[em_StaticText_CurrentFile] );
	m_StaticCurrentFile.SetWindowText( _S( STR_PATCH_CURRENT_FILE ) );
}

BOOL CDnLauncherDlgCHN::InitWebpage()
{
	BOOL bRtn = CDnLauncherDlg::InitWebpage();
	m_pDnHtmlView->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED );

	return bRtn;
}

BOOL CDnLauncherDlgCHN::InitBackground()
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

void CDnLauncherDlgCHN::OnClickedHomepageLink()
{
	GetDlgItem( IDC_HOMEPAGE_LINK )->Invalidate();
	ShellExecute( m_hWnd, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
}

#endif // _CHN