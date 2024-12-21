#include "stdafx.h"
#include "DnLauncherDlgTHAI.h"
#include "../../DnHtmlView.h"


#if defined(_THAI)


CDnLauncherDlgTHAI::CDnLauncherDlgTHAI( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_ExMaxNum;
	m_pRectCtrl = new CRect[em_ExMaxNum];
}

void CDnLauncherDlgTHAI::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	// Static Text Ctrl
	DDX_Control( pDX, IDC_HOMEPAGE_LINK, m_StaticHomepageLink );
}

BEGIN_MESSAGE_MAP( CDnLauncherDlgTHAI, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED( IDC_HOMEPAGE_LINK, &CDnLauncherDlgTHAI::OnClickedHomepageLink )
END_MESSAGE_MAP()

void CDnLauncherDlgTHAI::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );

	CDnLauncherDlg::OnDestroy();
	m_FontStatic.DeleteObject();
}

BOOL CDnLauncherDlgTHAI::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
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

void CDnLauncherDlgTHAI::MakeFont()
{
	m_Font.CreateFont( 14, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("tahoma") );

	m_FontStatic.CreateFont( 14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("tahoma") );
}

void CDnLauncherDlgTHAI::SetStaticText()
{
	CDnLauncherDlg::SetStaticText();

	m_StaticLauncherVersion.SetFont( &m_FontStatic );

	m_StaticHomepageLink.SetTextColor( RGB( 25, 115, 255 ) );
	m_StaticHomepageLink.SetFont( &m_Font );
	m_StaticHomepageLink.MoveWindow( m_pRectCtrl[em_StaticText_HomepageLink] );
	m_StaticHomepageLink.SetWindowText( _S( STR_HOMEPAGE_LINK ) );
}

BOOL CDnLauncherDlgTHAI::InitWebpage()
{
	BOOL bRtn = CDnLauncherDlg::InitWebpage();
	m_pDnHtmlView->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED );

	return bRtn;
}

BOOL CDnLauncherDlgTHAI::InitBackground()
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

void CDnLauncherDlgTHAI::OnClickedHomepageLink()
{
	GetDlgItem( IDC_HOMEPAGE_LINK )->Invalidate();
	ShellExecute( m_hWnd, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
}

#endif // _THAI