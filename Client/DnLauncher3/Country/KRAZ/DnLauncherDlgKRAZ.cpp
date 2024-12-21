#include "stdafx.h"
#include "DnLauncherDlgKRAZ.h"
#include "../../DnHtmlView.h"


#if defined(_KRAZ)

extern CDnPatchDownloadThread* g_pPatchDownloadThread;

CDnLauncherDlgKRAZ::CDnLauncherDlgKRAZ( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_ExMaxNum;
	m_pRectCtrl = new CRect[em_ExMaxNum];
	m_staticColor = RGB( 170, 180, 188 );
	m_ColorPercentage = RGB( 179, 255, 0 );
}

void CDnLauncherDlgKRAZ::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	// Static Text Ctrl
	DDX_Control( pDX, IDC_TEXT_CURRENT_INFO, m_StaticCurrentInfo );
	DDX_Control( pDX, IDC_TEXT_FILE_NAME, m_StaticTextFileName );
	DDX_Control( pDX, IDC_TEXT_FILE_SIZE, m_StaticTextFileSize );
	DDX_Control( pDX, IDC_TEXT_TIME_LEFT, m_StaticTextTimeLeft );
	DDX_Control( pDX, IDC_HOMEPAGE_LINK, m_StaticHomepageLink );
}

BEGIN_MESSAGE_MAP( CDnLauncherDlgKRAZ, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED( IDC_BTN_CANCEL, &CDnLauncherDlgKRAZ::OnClickButtonCancel )
	ON_BN_CLICKED( IDC_BTN_GAMESTART, &CDnLauncherDlgKRAZ::OnClickButtonGameStart )
	ON_BN_CLICKED( IDC_BTN_CLOSE, &CDnLauncherDlgKRAZ::OnClickButtonClose )
	ON_BN_CLICKED( IDC_HOMEPAGE_LINK, &CDnLauncherDlgKRAZ::OnClickedHomepageLink )
END_MESSAGE_MAP()


void CDnLauncherDlgKRAZ::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
	m_12Font.DeleteObject();
	m_11FontStatic.DeleteObject();
	m_12FontStatic.DeleteObject();
}

BOOL CDnLauncherDlgKRAZ::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
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

void CDnLauncherDlgKRAZ::MakeFont()
{
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("����") );

	m_12Font.CreateFont( 12, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("����") );

	m_11FontStatic.CreateFont( 11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("����") );

	m_12FontStatic.CreateFont( 12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("����") );
}

void CDnLauncherDlgKRAZ::SetStaticText()
{
	CDnLauncherDlg::SetStaticText();

	m_StaticLauncherVersion.SetTextColor( m_staticColor );
	m_StaticLauncherVersion.SetFont( &m_11FontStatic );
	m_StaticFileName.SetTextColor( m_staticColor );
	m_StaticFileName.SetFont( &m_12FontStatic );
	m_StaticDirectGameStart.SetTextColor( m_staticColor );
	m_StaticDirectGameStart.SetFont( &m_11FontStatic );
	m_StaticDownloadPercentage.SetTextColor( m_ColorPercentage );
	m_StaticDownloadPercentage.SetFont( &m_12Font );
	m_StaticDownloadState.SetFont( &m_12Font );

	CString strText;
	m_StaticCurrentInfo.SetTextColor( m_staticColor );
	m_StaticCurrentInfo.SetFont( &m_12FontStatic );
	m_StaticCurrentInfo.MoveWindow( m_pRectCtrl[em_StaticText_CurrentInfo] );
	strText.Format( _T("%s :"), _S( STR_PATCH_CURRENT_INFO ) );
	m_StaticCurrentInfo.SetWindowText( strText );

	m_StaticTextFileName.SetTextColor( m_staticColor );
	m_StaticTextFileName.SetFont( &m_12FontStatic );
	m_StaticTextFileName.MoveWindow( m_pRectCtrl[em_StaticText_FileName] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILENAME ) );
	m_StaticTextFileName.SetWindowText( strText );

	m_StaticTextFileSize.SetTextColor( m_staticColor );
	m_StaticTextFileSize.SetFont( &m_11FontStatic );
	m_StaticTextFileSize.MoveWindow( m_pRectCtrl[em_StaticText_FileSize] );
	strText.Format( _T("%s :"), _S( STR_DLG_FILE_SIZE ) );
	m_StaticTextFileSize.SetWindowText( strText );

	m_StaticTextTimeLeft.SetTextColor( m_staticColor );
	m_StaticTextTimeLeft.SetFont( &m_11FontStatic );
	m_StaticTextTimeLeft.MoveWindow( m_pRectCtrl[em_StaticText_TimeLeft] );
	strText.Format( _T("%s :"), _S( STR_DLG_DOWNLOAD_TIME_LEFT ) );
	m_StaticTextTimeLeft.SetWindowText( strText );

	m_StaticHomepageLink.SetTextColor( m_staticColor );
	m_StaticHomepageLink.SetFont( &m_11FontStatic );
	m_StaticHomepageLink.MoveWindow( m_pRectCtrl[em_StaticText_HomepageLink] );
	m_StaticHomepageLink.SetWindowText( _S( STR_HOMEPAGE_LINK ) );
}

BOOL CDnLauncherDlgKRAZ::InitWebpage()
{
	BOOL bRtn = CDnLauncherDlg::InitWebpage();
	m_pDnHtmlView->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED );

	return bRtn;
}

BOOL CDnLauncherDlgKRAZ::InitBackground()
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

void CDnLauncherDlgKRAZ::OnClickButtonCancel()
{
	int nResult = AfxMessageBox( STR_PATCH_CANCEL, MB_YESNO );
	if ( nResult == IDYES && m_emDownloadPatchState != PATCH_COMPLETE )	// Ȯ��â ���� ���¿��� ��ġ�Ϸ� �Ǿ��� ��� ����
	{
		if( g_pPatchDownloadThread )
		{
			g_pPatchDownloadThread->TerminateThread();
			g_pPatchDownloadThread->WaitForTerminate();

			SAFE_DELETE( g_pPatchDownloadThread );
			g_pPatchDownloadThread = NULL;
		}
		LogWnd::TraceLog(_T("�� Patch Canceled"));

#ifdef _USE_PARTITION_SELECT
		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_SHOW );
#else // _USE_PARTITION_SELECT
		CDialog::OnCancel();
#endif // _USE_PARTITION_SELECT
	}
}

void CDnLauncherDlgKRAZ::OnClickButtonGameStart()
{
	CDnLauncherDlg::OnClickButtonGameStart();
}

void CDnLauncherDlgKRAZ::OnClickButtonClose()
{
	int nResult = AfxMessageBox( STR_LAUNCHER_EXIT, MB_YESNO );

	if( nResult == IDYES )
	{
		LogWnd::TraceLog(_T("�� Launcher Closed"));
		CDialog::OnCancel();
	}
}

void CDnLauncherDlgKRAZ::OnClickedHomepageLink()
{
	GetDlgItem( IDC_HOMEPAGE_LINK )->Invalidate();
	ShellExecute( m_hWnd, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
}

#endif // _KRAZ