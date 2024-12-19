// DnLauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DnLauncher.h"
#include "DnLauncherDlg.h"
#include "DnHtmlView.h"
#include "DnPatchThread.h"
#include "DnOptionDlg.h"
#include "DnAccountManager.h"
#include "DnExplorerDlg.h"


#if defined(_KOR)
#include "DnLauncherDlgKOR.h"
#elif defined(_CHN)
#include "DnLauncherDlgCHN.h"
#elif defined(_JPN)
#include "DnLauncherDlgJPN.h"
#elif defined(_TWN)
#include "DnLauncherDlgTWN.h"
#elif defined(_USA)
#include "DnLauncherDlgUSA.h"
#elif defined(_SG)
#include "DnLauncherDlgSG.h"
#elif defined(_THAI)
#include "DnLauncherDlgTHAI.h"
#elif defined(_EU)
#include "DnLauncherDlgEU.h"
#elif defined(_IDN)
#include "DnLauncherDlgIDN.h"
#elif defined(_RUS)
#include "DnLauncherDlgRUS.h"
#elif defined(_KRAZ)
#include "DnLauncherDlgKRAZ.h"
#elif defined(_TEST)
#include "DnLauncherDlgTEST.h"
#else
#include "DnLauncherDlg.h"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CDnPatchDownloadThread* g_pPatchDownloadThread;
extern stDownloadInfoUpdateData	g_DownloadInfoData;
extern CString g_szCmdLine;
extern int g_nInitErrorCode;


struct DownloadStateString
{
	int nStateIndex;
	int nStringID;
};

DownloadStateString g_DownloadStateString[] = 
{
	{ PATCH_NONE,						-1	},
	{ PATCH_CHECK_VERSION,				(STR_CONFIRM_PATCH_VER)	},
	{ PATCH_CHECK_VERSION_FAILED,		(STR_CONFIRM_PATCH_VER_FAILED)	},
	{ PATCH_DOWNLOAD_DATA,				(STR_PATCH_DATA_DOWNLOADING)	},
	{ PATCH_DOWNLOAD_DATA_FAILED,		(STR_PATCH_DATA_DOWNLOADING_FAILED)	},
	{ PATCH_MD5_CHECK,					(STR_PATCH_MD5_CHECK)	},
	{ PATCH_MD5_CHECK_FAILED,			(STR_PATCH_MD5_CHECK_FAILED)	},
	{ PATCH_APPLY_PAK_FILE,				(STR_PATCH_APPLY_PAK_FILE	)	},
	{ PATCH_APPLY_PAK_FILE_FAILED,		(STR_PATCH_APPLY_PAK_FILE_FAILED)	},
	{ PATCH_APPLY_PAK_FILE_END,			-1	},
	{ PATCH_COMPLETE,					(STR_PATCH_COMPLETE)	},
	{ PATCH_LAUNCHER,					-1	},
	{ PATCH_FAILED,						(STR_PATCH_FAILED)	},
	{ PATCH_TERMINATE,					(STR_PATCH_TERMINATE)	},
};

// CDnLauncherDlg dialog
CDnLauncherDlg::CDnLauncherDlg( CWnd* pParent )
: CDialog(CDnLauncherDlg::IDD, pParent)
, m_dwWidth( 0 )
, m_dwHeight( 0 )
, m_dwFlags ( 0 )
, m_emDownloadPatchState( PATCH_NONE )
, m_nMaxCtrlNum( 0 )
, m_pDnHtmlView( NULL )
, m_pDownloadProgress( NULL )
, m_pFileCountProgress( NULL )
, m_pDownloadProgressBitmap( NULL )
, m_pFileCountProgressBitmap( NULL )
, m_nDownloadProgressMin( 0 )
, m_nDownloadProgressMax( 0 )
, m_nCurrentFileCount( 0 )
, m_nTotalFileCount( 0 )
, m_fDownloadRate( 0.0f )
, m_bDownloading( FALSE )
, m_staticBaseColor( WHITE )
, m_dcBkGrnd( NULL )
, m_pRectCtrl( NULL )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	EnumDisplayMode( m_vecDisplayMode );
	m_pntMouse.x = 0;
	m_pntMouse.y = 0;
}

CDnLauncherDlg* CDnLauncherDlg::CreateLauncherDlg()
{
#if defined(_KOR)
	return ( new CDnLauncherDlgKOR() );
#elif defined(_CHN)
	return ( new CDnLauncherDlgCHN() );
#elif defined(_JPN)
	return ( new CDnLauncherDlgJPN() );
#elif defined(_TWN)
	return ( new CDnLauncherDlgTWN() );
#elif defined(_USA)
	return ( new CDnLauncherDlgUSA() );
#elif defined(_SG)
	return ( new CDnLauncherDlgSG() );
#elif defined(_THAI)
	return ( new CDnLauncherDlgTHAI() );
#elif defined(_EU)
	return ( new CDnLauncherDlgEU() );
#elif defined(_IDN)
	return ( new CDnLauncherDlgIDN() );
#elif defined(_RUS)
	return ( new CDnLauncherDlgRUS() );
#elif defined(_KRAZ)
	return ( new CDnLauncherDlgKRAZ() );
#elif defined(_TEST)
	return ( new CDnLauncherDlgTEST() );
#else
	return (new CDnLauncherDlg());
#endif
	return NULL;
}

void CDnLauncherDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );

	// Button Ctrl
	DDX_Control( pDX, IDC_CHECK_DIRECT_GAMESTART, m_BtnCheckDirectGameStart );
	DDX_Control( pDX, IDC_BTN_CANCEL, m_BtnCancel );
	DDX_Control( pDX, IDC_BTN_GAMESTART, m_BtnStartGame );
	DDX_Control( pDX, IDC_BTN_OPTION, m_BtnGameOption );
	DDX_Control( pDX, IDC_BTN_CLOSE, m_BtnClose );
	DDX_Control( pDX, IDC_BTN_MINIMUM, m_BtnMinimum );
#ifdef _USE_PARTITION_SELECT
	DDX_Control( pDX, IDC_BTN_SELECT_PARTITION, m_BtnSelectPartition );
	DDX_Control( pDX, IDC_BTN_START_PATCH, m_BtnStartPatch );
#endif // _USE_PARTITION_SELECT

	// ProgressBar Ctrl
	DDX_Control( pDX, IDC_STATIC_PROGRESS_DOWNLOAD, m_StaticDownloadProgress );
	DDX_Control( pDX, IDC_STATIC_PROGRESS_COUNT, m_StaticFileCountProgress );

	// Static Text
	DDX_Text( pDX, IDC_LAUNCHER_VERSION, m_strLauncherVersion );
	DDX_Text( pDX, IDC_FILE_NAME, m_strFileName );
	DDX_Text( pDX, IDC_FILE_SIZE, m_strFileSize );
	DDX_Text( pDX, IDC_TIME_LEFT, m_strTimeLeft );
	DDX_Text( pDX, IDC_FILE_COUNT, m_strFileCount );
	DDX_Text( pDX, IDC_DOWNLOAD_PERCENTAGE, m_strDownloadPercentage );
	DDX_Text( pDX, IDC_DOWNLOAD_STATE, m_strDownloadState );
	DDX_Text( pDX, IDC_DIRECT_START, m_strDirectGameStart );
#ifdef _USE_PARTITION_SELECT
	DDX_Text( pDX, IDC_SELECT_PARTITION, m_strSelectPartition );
	DDX_Text( pDX, IDC_SELECT_PARTITION_NAME, m_strSelectPartitionName );
	DDX_Text( pDX, IDC_SELECT_PARTITION_GUIDE, m_strSelectPartitionGuide );
#endif // _USE_PARTITION_SELECT

	// Static Text Ctrl
	DDX_Control( pDX, IDC_LAUNCHER_VERSION, m_StaticLauncherVersion );
	DDX_Control( pDX, IDC_FILE_NAME, m_StaticFileName );
	DDX_Control( pDX, IDC_FILE_SIZE, m_StaticFileSize );
	DDX_Control( pDX, IDC_TIME_LEFT, m_StaticTimeLeft );
	DDX_Control( pDX, IDC_FILE_COUNT, m_StaticFileCount );
	DDX_Control( pDX, IDC_DOWNLOAD_PERCENTAGE, m_StaticDownloadPercentage );
	DDX_Control( pDX, IDC_DOWNLOAD_STATE, m_StaticDownloadState );
	DDX_Control( pDX, IDC_DIRECT_START, m_StaticDirectGameStart );
#ifdef _USE_PARTITION_SELECT
	DDX_Control( pDX, IDC_SELECT_PARTITION, m_StaticSelectPartition );
	DDX_Control( pDX, IDC_SELECT_PARTITION_NAME, m_StaticSelectPartitionName );
	DDX_Control( pDX, IDC_SELECT_PARTITION_GUIDE, m_StaticSelectPartitionGuide );
#endif // _USE_PARTITION_SELECT
}


BEGIN_MESSAGE_MAP( CDnLauncherDlg, CDialog )
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_STATUS_MSG, OnStatusMsg )
	// Button Ctrl Click Message
	ON_BN_CLICKED( IDC_CHECK_DIRECT_GAMESTART, &CDnLauncherDlg::OnClickedCheckDirectGamestart )
	ON_BN_CLICKED( IDC_BTN_CANCEL, &CDnLauncherDlg::OnClickButtonCancel )
	ON_BN_CLICKED( IDC_BTN_GAMESTART, &CDnLauncherDlg::OnClickButtonGameStart )
	ON_BN_CLICKED( IDC_BTN_OPTION, &CDnLauncherDlg::OnClickButtonOption )
	ON_BN_CLICKED( IDC_BTN_CLOSE, &CDnLauncherDlg::OnClickButtonClose )
	ON_BN_CLICKED( IDC_BTN_MINIMUM, &CDnLauncherDlg::OnClickMinimumBtn )
#ifdef _USE_PARTITION_SELECT
	ON_BN_CLICKED( IDC_BTN_SELECT_PARTITION, &CDnLauncherDlg::OnClickButtonSelectPartition )
	ON_BN_CLICKED( IDC_BTN_START_PATCH, &CDnLauncherDlg::OnClickButtonStartPatch )
#endif // _USE_PARTITION_SELECT
END_MESSAGE_MAP()


void CDnLauncherDlg::OnDestroy()
{
	m_vecDisplayMode.clear();
	m_Font.DeleteObject();

	SAFE_DELETE( m_pDownloadProgress );
	SAFE_DELETE( m_pFileCountProgress );
	SAFE_DELETE( m_pDownloadProgressBitmap );
	SAFE_DELETE( m_pFileCountProgressBitmap );

	// Thread Terminate
	if( g_pPatchDownloadThread )
	{
		g_pPatchDownloadThread->TerminateThread();
		g_pPatchDownloadThread->WaitForTerminate();
	}
	LogWnd::TraceLog( _T( "Thread End") );
	SAFE_DELETE( g_pPatchDownloadThread );

	LogWnd::DestroyLog();

	CDialog::OnDestroy();
}

BOOL CDnLauncherDlg::InitFail()
{
	CString strError;
	strError.Format( _S( STR_LAUNCHER_INIT_FAILED + DNPATCHINFO.GetLanguageOffset() ), g_nInitErrorCode );
	LogWnd::Log( LogLevel::Error, strError );
	AfxMessageBox( strError );
	CDialog::DestroyWindow();
	return FALSE;
}

// CDnLauncherDlg message handlers
BOOL CDnLauncherDlg::OnInitDialog()
{
	LogWnd::TraceLog( L"InitDialog Start!" );
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon( m_hIcon, TRUE );	// Set big icon
	SetIcon( m_hIcon, FALSE );	// Set small icon

	this->SetWindowText( _S( STR_DRAGONNEST_LAUNCHER + DNPATCHINFO.GetLanguageOffset() ) );
	DNOPTIONDATA.LoadConfigOption( this->m_hWnd );		// Option 읽기

	if( !InitInterface() )		// UI 초기화
	{
		g_nInitErrorCode = INIT_ERROR_INTERFACE;
		return InitFail();
	}

	if( !InitWebpage() )		// 가이드 웹페이지 초기화
	{
		g_nInitErrorCode = INIT_ERROR_WEBPAGE;
		return InitFail();
	}

#ifdef _USE_PARTITION_SELECT
#ifdef _FIRST_PATCH
	InitSelectPartition();
#else // _FIRST_PATCH
	DNOPTIONDATA.LoadPartitionOption( this->m_hWnd );

	// 파티션 선택 모드에서는 선택된 정보가 있을 경우 Patch Info 초기화 아닐 경우 클라이언트 버전만 읽음
	if( DNOPTIONDATA.m_nSelectChannelNum > 0 && DNOPTIONDATA.m_nSelectPartitionId > 0 )
	{
		if( DNPATCHINFO.SetPatchInfo() )
			InitSelectPartition();
		else	// 저장된 파티션 정보로 패치정보 설정에 실패하면 파티션 정보 날리고 다시 세팅되게 유도
		{
			DNOPTIONDATA.m_nSelectChannelNum = 0;
			DNOPTIONDATA.m_nSelectPartitionId = 0;
			DNOPTIONDATA.SavePartitionOption( this->m_hWnd );
		}
	}
	else
		DNPATCHINFO.LoadClientVersion();
#endif // _FIRST_PATCH
#endif // _USE_PARTITION_SELECT
	
	NavigateHtml();

#if defined(_KOR) || defined(_THAI) || defined(_IDN) || defined(_CHN) || defined(_KRAZ) || defined(_EU)
	m_strLauncherVersion.Format( _T("Ver.%d"), DNPATCHINFO.GetClientVersion() );	// 왼쪽 상단 버전 표기 스트링
#else // _KOR, _THAI, _IDN
	m_strLauncherVersion.Format( _T("%s Ver.%d"), _S( STR_DLG_DRAGON_NEST + DNPATCHINFO.GetLanguageOffset() ), DNPATCHINFO.GetClientVersion() );	// 왼쪽 상단 버전 표기 스트링
#endif // _KOR, _THAI, _IDN
	SetTimer( TEVENT_REFRESH_TEXTINFO, 40, NULL );		//화면 텍스트 갱신
	LogWnd::TraceLog( L"InitDialog Success!" );

#ifndef _USE_PARTITION_SELECT
	StartPatchDownloadThread();	// 파티션 선택이 아니라면 바로 패치 쓰레드 시작
#endif // _USE_PARTITION_SELECT

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDnLauncherDlg::NavigateHtml()
{
#ifdef _USE_PARTITION_SELECT
	if( DNPATCHINFO.GetGuidepageUrl().GetLength() == 0 )	// 선택했던 기록이 없으면 기본가이드 페이지를 구해서 보여줌 (엑박방지!)
	{
		if( m_pDnHtmlView )
			m_pDnHtmlView->Navigate( DNPATCHINFO.GetBaseGuidePageUrl() );

		return;
	}
#endif // _USE_PARTITION_SELECT
	if( m_pDnHtmlView )
		m_pDnHtmlView->Navigate( DNPATCHINFO.GetGuidepageUrl() );
}

BOOL CDnLauncherDlg::InitWebpage()
{
	LogWnd::TraceLog( L"InitWebpage" );
	// 가이드 웹페이지 View/Html 셋팅
	CCreateContext pContext;
	CWnd* pFrameWnd = this;
	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS( CDnView );
	CDnView *pView = (CDnView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	if( pView == NULL )
		return FALSE;
	pView->ShowWindow( SW_SHOW );

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS( CDnHtmlView );
	m_pDnHtmlView = (CDnHtmlView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	if( m_pDnHtmlView == NULL )
		return FALSE;
	m_pDnHtmlView->ShowWindow( SW_SHOW );
	m_pDnHtmlView->MoveWindow( GetControlPos( em_WebPageHtml ) );

	return TRUE;
}

BOOL CDnLauncherDlg::InitInterface()
{
	LogWnd::TraceLog( L"InitInterface" );
	// -- Background -- //
	if( !InitBackground() )
		return FALSE;

	// -- Control -- //
	InitCtrlRect();
	GetCtrlRect( m_pRectCtrl, m_nMaxCtrlNum );	// Ctrl Rect 셋팅
	InitControl();

	// -- Transparent -- //
	int window_style = GetWindowLong( m_hWnd, GWL_EXSTYLE );

	if( !( window_style & WS_EX_LAYERED ) )
		SetWindowLong( m_hWnd, GWL_EXSTYLE, window_style | WS_EX_LAYERED );

	HMODULE h_user32_dll = GetModuleHandle( L"USER32.DLL" );
	if( h_user32_dll != NULL )
	{
		BOOL (WINAPI *fp_set_layered_window_attributes)(HWND, COLORREF, BYTE, DWORD) = 
			(BOOL (WINAPI *)(HWND, COLORREF, BYTE, DWORD))GetProcAddress(h_user32_dll, 
			"SetLayeredWindowAttributes");

		if( fp_set_layered_window_attributes != NULL )	// 함수의 포인터를 이용해서 SetLayeredWindowAttributes 함수를 간접 수행한다.
			(*fp_set_layered_window_attributes)( m_hWnd, RGB( 255, 0, 255 ), 0, LWA_COLORKEY );
	}

	return TRUE;
}

BOOL CDnLauncherDlg::InitBackground()
{
	CxImage xImage;
	if( !xImage.LoadResource( FindResource( NULL, MAKEINTRESOURCE( IDR_JPG_BACK_GROUND + DNPATCHINFO.GetLanguageOffset() ), L"JPG" ), CXIMAGE_FORMAT_JPG ) )
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
	
	// set window size the same as image size ( 최상위로 설정 )
	//::SetWindowPos( this->GetSafeHwnd(), HWND_TOPMOST, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE );
	SetWindowPos( &wndTopMost, 0, 0, m_dwWidth, m_dwHeight, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );
	::SetForegroundWindow( this->GetSafeHwnd() );
	::SetActiveWindow( this->GetSafeHwnd() );

	// assign region to window
	SetWindowRgn( hRgn, FALSE );

	return TRUE;
}

void CDnLauncherDlg::InitControl()
{
	// Button Control
	m_BtnCheckDirectGameStart.MoveWindow( m_pRectCtrl[em_CheckBox_DIrectStart] );
	m_BtnCheckDirectGameStart.SetCheck( DNOPTIONDATA.m_bAutoGameStart );

	m_BtnCancel.SetSkinJpg( IDR_JPG_CANCEL + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CANCEL_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CANCEL_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnCancel.MoveWindow( m_pRectCtrl[em_BtnQuit] );
	m_BtnCancel.SetWindowText( L"" );

	m_BtnStartGame.SetSkinJpg( IDR_JPG_GAMESTART + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_GAMESTART_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_GAMESTART_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnStartGame.MoveWindow( m_pRectCtrl[em_BtnStartGame] );
	m_BtnStartGame.ShowWindow( SW_HIDE );
	m_BtnStartGame.SetWindowText( L"" );

	m_BtnGameOption.SetSkinJpg( IDR_JPG_OPTION + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_OPTION_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_OPTION_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnGameOption.MoveWindow( m_pRectCtrl[em_BtnOption] );
	m_BtnGameOption.ShowWindow( SW_HIDE );
	m_BtnGameOption.SetWindowText( L"" );

	m_BtnClose.SetSkinJpg( IDR_JPG_CLOSE + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CLOSE_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_CLOSE_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnClose.MoveWindow( m_pRectCtrl[em_BtnWinClose] );
	m_BtnClose.SetWindowText( L"" );

	m_BtnMinimum.SetSkinJpg( IDR_JPG_MINIMUM + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_MINIMUM_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_MINIMUM_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnMinimum.MoveWindow( m_pRectCtrl[em_BtnWinMiniMum] );
	m_BtnMinimum.SetWindowText( L"" );

#ifdef _USE_PARTITION_SELECT
	m_BtnSelectPartition.SetSkinJpg( IDR_JPG_SELECT_PARTITION + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_SELECT_PARTITION_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_SELECT_PARTITION_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnSelectPartition.MoveWindow( m_pRectCtrl[em_BtnSelectPartition] );
	m_BtnSelectPartition.SetWindowText( L"" );

	m_BtnStartPatch.SetSkinJpg( IDR_JPG_PARTCHSTART + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_PARTCHSTART_CLICK + DNPATCHINFO.GetLanguageOffset(), IDR_JPG_PARTCHSTART_ON + DNPATCHINFO.GetLanguageOffset() );
	m_BtnStartPatch.MoveWindow( m_pRectCtrl[em_BtnStartPatch] );
	m_BtnStartPatch.SetWindowText( L"" );
#endif // _USE_PARTITION_SELECT

	// ProgressBar Control
	m_pDownloadProgressBitmap = new CBitmap();
	HBITMAP hDownloadBitmap = (HBITMAP)::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_PROGRESS_DOWNLOAD), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	m_pDownloadProgressBitmap->Attach( hDownloadBitmap );
	m_pDownloadProgress = new CSkinProgress( &m_StaticDownloadProgress, 100, m_pDownloadProgressBitmap );
	m_pDownloadProgress->SetRange( 0, 100 );
	m_StaticDownloadProgress.MoveWindow( m_pRectCtrl[em_DownloadProgress] );
	m_StaticDownloadProgress.SetWindowText( L"" );

	m_pFileCountProgressBitmap = new CBitmap();
	HBITMAP hCountBitmap = (HBITMAP)::LoadImage( ::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_PROGRESS_COUNT), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
	m_pFileCountProgressBitmap->Attach( hCountBitmap );
	m_pFileCountProgress = new CSkinProgress( &m_StaticFileCountProgress, 100, m_pFileCountProgressBitmap );
	m_pFileCountProgress->SetRange( 0, 100 );
	m_StaticFileCountProgress.MoveWindow( m_pRectCtrl[em_FileCountProgress] );
	m_StaticFileCountProgress.SetWindowText( L"" );

	// Text Control
	MakeFont();
	SetStaticText();
}

void CDnLauncherDlg::SetStaticText()
{
	m_StaticLauncherVersion.SetTextColor( m_staticBaseColor );
	m_StaticLauncherVersion.SetFont( &m_Font );
	m_StaticLauncherVersion.MoveWindow( m_pRectCtrl[em_Text_LauncherVersion] );

	m_StaticFileName.SetTextColor( m_staticBaseColor );
	m_StaticFileName.SetFont( &m_Font );
	m_StaticFileName.MoveWindow( m_pRectCtrl[em_Text_FileName] );

	m_StaticFileSize.SetTextColor( m_staticBaseColor );
	m_StaticFileSize.SetFont( &m_Font );
	m_StaticFileSize.MoveWindow( m_pRectCtrl[em_Text_FileSize] );

	m_StaticTimeLeft.SetTextColor( m_staticBaseColor );
	m_StaticTimeLeft.SetFont( &m_Font );
	m_StaticTimeLeft.MoveWindow( m_pRectCtrl[em_Text_TimeLeft] );

	m_StaticFileCount.SetTextColor( m_staticBaseColor );
	m_StaticFileCount.SetFont( &m_Font );
	m_StaticFileCount.MoveWindow( m_pRectCtrl[em_Text_FileCount] );

	m_StaticDownloadPercentage.SetTextColor( m_staticBaseColor );
	m_StaticDownloadPercentage.SetFont( &m_Font );
	m_StaticDownloadPercentage.MoveWindow( m_pRectCtrl[em_Text_DownloadPercentage] );

	m_StaticDownloadState.SetTextColor( m_staticBaseColor );
	m_StaticDownloadState.SetFont( &m_Font );
	m_StaticDownloadState.MoveWindow( m_pRectCtrl[em_Text_DownloadState] );

	m_StaticDirectGameStart.SetTextColor( m_staticBaseColor );
	m_StaticDirectGameStart.SetFont( &m_Font );
	m_StaticDirectGameStart.MoveWindow( m_pRectCtrl[em_Text_DirectStart] );
	m_StaticDirectGameStart.SetWindowText( _S( STR_AFTER_UPDATE_ATUOSTART + DNPATCHINFO.GetLanguageOffset() ) );

#ifdef _USE_PARTITION_SELECT
	m_StaticSelectPartition.SetTextColor( WHITE );
	m_StaticSelectPartition.SetFont( &m_Font );
	m_StaticSelectPartition.MoveWindow( m_pRectCtrl[em_Text_SelectPartition] );
	m_strSelectPartition = _S( STR_SELECT_PARTITION + DNPATCHINFO.GetLanguageOffset() );

	m_StaticSelectPartitionName.SetTextColor( RED );
	m_StaticSelectPartitionName.SetFont( &m_Font );
	m_StaticSelectPartitionName.MoveWindow( m_pRectCtrl[em_Text_SelectPartitionName] );

	m_StaticSelectPartitionGuide.SetTextColor( WHITE );
	m_StaticSelectPartitionGuide.SetFont( &m_Font );
	m_StaticSelectPartitionGuide.MoveWindow( m_pRectCtrl[em_Text_SelectPartitionGuide] );
	m_strSelectPartitionGuide = _S( STR_SELECT_PARTITION_GUIDE + DNPATCHINFO.GetLanguageOffset() );
#endif // _USE_PARTITION_SELECT
}

void CDnLauncherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon( x, y, m_hIcon );
	}
	else
	{
		CDialog::OnPaint();
	}
}

HBRUSH CDnLauncherDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	HBRUSH hbr = CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	RECT rect;

	switch(nCtlColor)
	{
	case CTLCOLOR_STATIC:
//	case CTLCOLOR_EDIT:
//	case CTLCOLOR_LISTBOX:
//	case CTLCOLOR_SCROLLBAR:
		{
			pDC->SetBkMode(TRANSPARENT);
			pWnd->GetWindowRect(&rect);
			RedrawWindow(&rect);
			return (HBRUSH)GetStockObject(NULL_BRUSH);;
		}
		break;
	}

	return hbr;
}

BOOL CDnLauncherDlg::OnEraseBkgnd( CDC* pDC )
{
	BitBlt( pDC->m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0, 0, SRCCOPY );
	return FALSE;
}

HCURSOR CDnLauncherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDnLauncherDlg::OnLButtonDown( UINT nFlags, CPoint point )
{
	if( !(m_dwFlags & DRAGGING) )
	{
		m_pntMouse = point;
		m_dwFlags |= DRAGGING;
		SetCapture();
	}

	CDialog::OnLButtonDown( nFlags, point );
}

void CDnLauncherDlg::OnLButtonUp( UINT nFlags, CPoint point )
{
	if( m_dwFlags & DRAGGING )
	{
		m_dwFlags &= ~DRAGGING;
		ReleaseCapture();
	}

	CDialog::OnLButtonUp( nFlags, point );
}

void CDnLauncherDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	if ( m_dwFlags & DRAGGING )
	{
		RECT rect;
		GetWindowRect( &rect );

		rect.left += point.x - m_pntMouse.x;
		rect.top += point.y - m_pntMouse.y;

		SetWindowPos( &wndTopMost, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
		SendMessage( WM_PAINT );
	}

	CDialog::OnMouseMove( nFlags, point );
}

void CDnLauncherDlg::OnClickedCheckDirectGamestart()
{
	if( m_BtnCheckDirectGameStart.GetCheck() != DNOPTIONDATA.m_bAutoGameStart )
	{
		DNOPTIONDATA.m_bAutoGameStart = m_BtnCheckDirectGameStart.GetCheck();
		DNOPTIONDATA.SaveConfigOption( this->m_hWnd );
	}
}

void CDnLauncherDlg::OnClickButtonCancel()
{
	int nResult = AfxMessageBox( _S( STR_PATCH_CANCEL + DNPATCHINFO.GetLanguageOffset() ), MB_YESNO );
	if ( nResult == IDYES && m_emDownloadPatchState != PATCH_COMPLETE )	// 확인창 열린 상태에서 패치완료 되었을 경우 예방
	{
		if( g_pPatchDownloadThread )
		{
			g_pPatchDownloadThread->TerminateThread();
			g_pPatchDownloadThread->WaitForTerminate();

			SAFE_DELETE( g_pPatchDownloadThread );
			g_pPatchDownloadThread = NULL;
		}
		LogWnd::TraceLog(_T("★ Patch Canceled"));

#ifdef _USE_PARTITION_SELECT
		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_SHOW );
#else // _USE_PARTITION_SELECT
		CDialog::OnCancel();
#endif // _USE_PARTITION_SELECT
	}
}

void CDnLauncherDlg::EnableStartButton()
{
	m_BtnCancel.ShowWindow( SW_HIDE );
	m_BtnStartGame.ShowWindow( SW_SHOW );
	m_BtnGameOption.ShowWindow( SW_SHOW );
}

void CDnLauncherDlg::OnClickButtonGameStart()
{
#ifdef _SKY
	CDnAccountManagerDlg dlg( this );
	INT_PTR nResponse = dlg.DoModal();
//	MessageBoxA(NULL, FormatA("%d", nResponse).c_str(), "OK", MB_OK);
	if (nResponse != 1) //1 = ok , 2 = app was closed!
	{
		CDialog::OnOK();
		return;
	}
#endif

	// 최소 메모리 검사
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);

	GlobalMemoryStatusEx( &statex );
	if( statex.ullAvailPhys < 800000000 )	// 사용가능 메모리 800메가 이하 경고 메세지 출력
	{
		AfxMessageBox( _S( STR_WARNNING_AVAILABLE_MEMORY + DNPATCHINFO.GetLanguageOffset() ), MB_OK );
		LogWnd::Log( LogLevel::Error, L"Warnning! Check Available Memory. You need atleast 800MB Ram free to play the game." );
	}

	CString strExeFile = DNPATCHINFO.GetClientPath();
	strExeFile += DNCLIENT_NAME;

	DNOPTIONDATA.SaveConfigOption( this->m_hWnd );
#ifdef _USE_PARTITION_SELECT
	DNOPTIONDATA.SavePartitionOption( this->m_hWnd );
#endif // _USE_PARTITION_SELECT
#ifdef _USE_MULTILANGUAGE
	DNPATCHINFO.AddLanguageParamToTotalParam();
#endif // _USE_MULTILANGUAGE

#ifdef ACTOZ_TEST_MESSAGE
	AfxMessageBox(DNPATCHINFO.GetTotalParameter().GetBuffer(), MB_OK);
#endif // ACTOZ_TEST_MESSAGE
#ifdef _USA
	ShellExecute( m_hWnd, NULL, strExeFile.GetBuffer(), DNPATCHINFO.GetTotalParameter().GetBuffer(), NULL, SW_SHOWNORMAL );

#else
	ShellExecute( m_hWnd, NULL, strExeFile.GetBuffer(), DNPATCHINFO.GetTotalParameter().GetBuffer(), NULL, SW_SHOWNORMAL );
#endif
#ifdef _USE_AUTOUPDATE	// 오토업데이트 구동
	int nNextVersion = DNPATCHINFO.GetNextVersion();
	LogWnd::Log( LogLevel::Info, L"Check Next Version Ver:%d, Server Ver:%d", nNextVersion, DNPATCHINFO.GetServerVersion() );
	if( nNextVersion > 0 && nNextVersion > DNPATCHINFO.GetServerVersion() )
	{
		CString strAutoupdateParameter;
		strAutoupdateParameter.Format( L"%s|%s|%d-%d", DNPATCHINFO.GetPatchUrl(), DNPATCHINFO.GetClientPath(), DNPATCHINFO.GetServerVersion()+1, nNextVersion );

		CString strAutoUpdateExe = DNPATCHINFO.GetClientPath();
		strAutoUpdateExe += DNAUTOUPDATE_NAME;

		LogWnd::Log( LogLevel::Info, L"Execute AutoUpdate.exe %s", strAutoUpdateExe.GetBuffer( 0 ) );
		ShellExecute( m_hWnd, NULL, strAutoUpdateExe, strAutoupdateParameter, NULL, SW_SHOWNORMAL );
	}
#endif // _USE_AUTOUPDATE
	Sleep( 1000 );	// 1초후에 끝내자..

	CDialog::OnOK();
}

void CDnLauncherDlg::OnClickButtonOption()
{
	CDnOptionDlg dlg( this );
	INT_PTR nResponse = dlg.DoModal();
}

void CDnLauncherDlg::OnClickButtonClose()
{
	int nResult = AfxMessageBox( _S( STR_LAUNCHER_EXIT + DNPATCHINFO.GetLanguageOffset() ), MB_YESNO );
	if( nResult == IDYES )
	{
		LogWnd::TraceLog(_T("★ Launcher Closed"));
		CDialog::OnCancel();
	}
}

void CDnLauncherDlg::OnClickMinimumBtn()
{
	ShowWindow( SW_SHOWMINIMIZED );
}

#ifdef _USE_PARTITION_SELECT
void CDnLauncherDlg::OnClickButtonStartPatch()
{
	if( !m_bDownloading && DNPATCHINFO.IsSetPatchInfo() )
	{
		int nResult = AfxMessageBox( _S( STR_PATCH_START + DNPATCHINFO.GetLanguageOffset() ), MB_YESNO );
		if( nResult == IDYES )
		{
			m_BtnCancel.ShowWindow( SW_SHOW );
			m_BtnStartPatch.ShowWindow( SW_HIDE );

			StartPatchDownloadThread();
		}
	}
	else
		ErrorMessageBoxLog( _S( STR_MUST_SELECT_PARTITION + DNPATCHINFO.GetLanguageOffset() ) );
}

void CDnLauncherDlg::OnClickButtonSelectPartition()
{
	if( !m_bDownloading )
	{
		// Open Server Select Page.
		CDnSelectPartitionDlg DnSelectPartitionDlg( this );
		WCHAR szUrl[4096]={0,};
		TCHAR szCurDir[ _MAX_PATH ]={0,};
		GetCurrentDirectory( _MAX_PATH, szCurDir );
		swprintf_s( szUrl , _T("%s\\Html\\HTMLPage1.htm"), szCurDir );

		DnSelectPartitionDlg.SetURL(szUrl);
		DnSelectPartitionDlg.DoModal();
	}
}

void CDnLauncherDlg::SetSelectPartition( int nSelectChannelNum, int nSelectPartitionNum )
{
	DNOPTIONDATA.m_nSelectChannelNum = nSelectChannelNum;
	DNOPTIONDATA.m_nSelectPartitionId = DNPATCHINFO.GetPartitionId( nSelectChannelNum, nSelectPartitionNum );

	if( !DNPATCHINFO.SetPatchInfo() )
		return;

	DNOPTIONDATA.SavePartitionOption( this->m_hWnd );

#ifdef _FIRST_PATCH
	CheckVersionAndReboot();
#else
	InitSelectPartition();
#endif

	m_pDnHtmlView->Navigate( DNPATCHINFO.GetGuidepageUrl() );
}

#ifdef _FIRST_PATCH
void CDnLauncherDlg::SetOnlySelectPartition( int nSelectChannelNum, int nSelectPartitionNum )
{
	//----------------------------------------------
	// 이 함수는 무조건 파티션 선택 및 저장만 한다.

	DNOPTIONDATA.m_nSelectChannelNum = nSelectChannelNum;
	DNOPTIONDATA.m_nSelectPartitionId = DNPATCHINFO.GetPartitionId( nSelectChannelNum, nSelectPartitionNum );

	if( !DNPATCHINFO.SetPatchInfo() )
	{
		return;
	}
	DNOPTIONDATA.SavePartitionOption( this->m_hWnd );
}

void CDnLauncherDlg::CheckVersionAndReboot()
{
	// 바뀐 파티션의 버전 정보 체크
	// 현재 로클의 버전과 동일 할 경우 -> 게임 시작 버튼 활성화
	// 현재 로컬의 버전이 높을 경우 -> Full Client 설치 요청 메세지
	// 현재 로컬의 버전이 낮을 경우 -> 패치 시작 버튼 활성화	

	
	/*
					----------- 모듈패치 관련 -----------
	
	모듈패치 진행시, 버전을 비교할때 ( 모듈버전 == 서버버전 ) 을 비교합니다.
	런처패치 진행시, 버전을 비교할떄 ( 런처버전 == 서버버전 ) 을 비교합니다.
	모듈패치를 마치고, 런처가 떠있는 상황에 "파티션을 재선택"해서, [런처 재실행] --> [모듈패치]를 실행하게 되면, 모듈 버전은 이미 서버에 맞춰져있게 됩니다. 예를 들면.
	
	ex) 모듈패치만 하고, 런처의 PatchStart버튼을 누르지 않고, 파티션을 변경했을때.
	ex) Server 100 , Module 100 , Launcher 50
	- 모듈패치 진행을 위해 버전을 비교할때, 문제가 생긴다.

	이런 경우를 방지하기 위해, 파티션 변경으로 인한 재실행을 할때는, Module버전이 런처버전보다 크면, 런처버전과 동일하게 바꿔줍니다..
	어차피 [런처버전==모듈버전]의 강제성이 있기 떄문에 이런식으로 수정했습니다. ( 모듈과 런처의 버전은 항상 같아야 합니다 )
	그리고, 모듈패치는 거의 하지 않으니, 패치를 다시 한다해도 퍼포먼스에 크게 영향을 줄꺼같지는 않습니다..

	*/

	int nClientVersion = DNPATCHINFO.GetClientVersion();
	int nSereverVersion = DNPATCHINFO.GetServerVersion();
	if( nClientVersion == nSereverVersion )
	{
		LogWnd::TraceLog( _T("Current Version is latest version") );
		OnStatusMsg( PATCH_COMPLETE, 0 );

		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_HIDE );
		m_pDownloadProgress->SetRange( 0, 100 );
		m_pDownloadProgress->SetPos( 100 );
		m_pFileCountProgress->SetRange( 0, 100 );
		m_pFileCountProgress->SetPos( 100 );
		m_strDownloadPercentage.Format( _T("%d%%"), 100 );

		if(	DNOPTIONDATA.m_bAutoGameStart )	// 자동시작 체크되있을 경우 시작하도록 한다.
			OnClickButtonGameStart();
	}
	else if( nClientVersion > nSereverVersion )
	{
		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_HIDE );
		m_BtnStartGame.ShowWindow( SW_HIDE );
		AfxMessageBox( _S( STR_ERROR_CLIENT_VERSION + DNPATCHINFO.GetLanguageOffset() ), MB_OK );
	}
	else if( nClientVersion < nSereverVersion )
	{
		// 런처 리부트 시킵니다.
		int nCurModuleVersion = DNFIRSTPATCHINFO.GetLocalModuleVersion();
		int nCurClientVersion = DNPATCHINFO.GetClientVersion();
		if( nCurModuleVersion > nCurClientVersion )
			nCurModuleVersion = nCurClientVersion;

		BOOL bSaveResult = SaveModuleVersion( nCurModuleVersion );
		if(bSaveResult)
		{
			CString strParam	= DNPATCHINFO.GetTotalParameter();	// 파라매터
			CString strExeFile	= DNPATCHINFO.GetClientPath();		// 다운로드 경로
			strExeFile += DNLAUNCHER_NAME;

#ifdef _USE_COMMAND_LINE
			ShellExecute( m_hWnd, NULL, strExeFile, g_szOriginalCmdLine.GetBuffer(), NULL, SW_SHOWNORMAL );
#else
			ShellExecute( m_hWnd, NULL, strExeFile, strParam.GetBuffer(), NULL, SW_SHOWNORMAL );
#endif
			KillMyProcess(_T(DNLAUNCHER_NAME)); // old프로세스 kill.
		}
		else
		{
			ErrorMessageBoxLog( _S( STR_PATCH_FAILED+DNPATCHINFO.GetLanguageOffset() ) );
		}
	}
}

BOOL CDnLauncherDlg::SaveModuleVersion( int nVersion )
{

	//------------------------------------------
	// version.cfg에 저장..
	// 버전 변경
	CString szPath;
	szPath = DNPATCHINFO.GetClientPath();
	szPath += CLIENT_VERSION_NAME;

	char*   buffer = NULL;
	HANDLE	hFile  = INVALID_HANDLE_VALUE;
	hFile = CreateFile( szPath.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::Log( LogLevel::Error, L"Save Version File Failed!" );
		return FALSE;
	}

	char  szVersion[ 256 ]= {0,};
	DWORD dwWriteSize;

	sprintf_s( szVersion, sizeof(szVersion), "Version %d\r\nModule %d", DNPATCHINFO.GetClientVersion(), nVersion );
	WriteFile( hFile, szVersion,(int)strlen(szVersion)+1 , &dwWriteSize, NULL );
	CloseHandle(hFile);



	//------------------------------------------
	// Pak 파일에 저장.
	CString szFindPackingFile;
	szFindPackingFile =  DNPATCHINFO.GetClientPath();
	szFindPackingFile += _T("Resource00.pak");

	CEtPackingFile *pPackingFile = new CEtPackingFile();
	char strVersion[256]={0,};
	sprintf_s( strVersion, sizeof(strVersion), "version %d\r\nModule %d", DNPATCHINFO.GetClientVersion(), nVersion );

	USES_CONVERSION;
	char szTemp[ _MAX_PATH ] = {0,};
	char StrVersionName[32]= "\\version.cfg";
	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, _MAX_PATH, NULL, NULL );

	// 어쩔수 없다. 512M 넘어가는것중에 골라서 추가하자. 
	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		pPackingFile->Remove( StrVersionName );
		pPackingFile->AddFile( StrVersionName, strVersion, sizeof(strVersion) );
	}
	else 
	{
		AfxMessageBox( _T("Version can not be saved") );
		SAFE_DELETE( pPackingFile );
		return FALSE;
	}

	pPackingFile->CloseFileSystem();
	SAFE_DELETE( pPackingFile );

	return TRUE;
}
#endif

void CDnLauncherDlg::InitSelectPartition()
{
	ResetDownloadInfo();

	m_strSelectPartitionName = DNPATCHINFO.GetPartitionName();
	m_strSelectPartition = _S( STR_SELECT_PARTITION + DNPATCHINFO.GetLanguageOffset() );
	m_strSelectPartitionGuide = _S( STR_SELECT_PARTITION_GUIDE + DNPATCHINFO.GetLanguageOffset() );

	GetDlgItem( IDC_SELECT_PARTITION_NAME )->ShowWindow( SW_SHOW );
	GetDlgItem( IDC_SELECT_PARTITION_GUIDE )->ShowWindow( SW_SHOW );

	CheckVersion();
}

void CDnLauncherDlg::CheckVersion()
{
	// 바뀐 파티션의 버전 정보 체크
	// 현재 로클의 버전과 동일 할 경우 -> 게임 시작 버튼 활성화
	// 현재 로컬의 버전이 높을 경우 -> Full Client 설치 요청 메세지
	// 현재 로컬의 버전이 낮을 경우 -> 패치 시작 버튼 활성화			
	int nClientVersion = DNPATCHINFO.GetClientVersion();
	int nSereverVersion = DNPATCHINFO.GetServerVersion();

	if( nClientVersion == nSereverVersion )
	{
		LogWnd::TraceLog( _T("Current Version is latest version") );
		OnStatusMsg( PATCH_COMPLETE, 0 );

		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_HIDE );
		m_pDownloadProgress->SetRange( 0, 100 );
		m_pDownloadProgress->SetPos( 100 );
		m_pFileCountProgress->SetRange( 0, 100 );
		m_pFileCountProgress->SetPos( 100 );
		m_strDownloadPercentage.Format( _T("%d%%"), 100 );

		if(	DNOPTIONDATA.m_bAutoGameStart )	// 자동시작 체크되있을 경우 시작하도록 한다.
			OnClickButtonGameStart();
	}
	else if( nClientVersion > nSereverVersion )
	{
		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_HIDE );
		m_BtnStartGame.ShowWindow( SW_HIDE );
		AfxMessageBox( _S( STR_ERROR_CLIENT_VERSION + DNPATCHINFO.GetLanguageOffset() ), MB_OK );
	}
	else if( nClientVersion < nSereverVersion )
	{
		m_BtnCancel.ShowWindow( SW_HIDE );
		m_BtnStartPatch.ShowWindow( SW_SHOW );
		m_BtnStartPatch.EnableWindow( TRUE );
		m_BtnStartGame.ShowWindow( SW_HIDE );
		m_BtnGameOption.ShowWindow( SW_HIDE );
		OnStatusMsg( PATCH_NONE, 0 );
	}
}
#endif // _USE_PARTITION_SELECT

void CDnLauncherDlg::OnTimer( UINT_PTR nIDEvent )
{
	if( nIDEvent == TEVENT_REFRESH_TEXTINFO )	// 화면 텍스트 정보 갱신
		RefreshTextInfo();

	CDialog::OnTimer(nIDEvent);
}

void CDnLauncherDlg::StartPatchDownloadThread()
{
	if( g_pPatchDownloadThread == NULL )
	{
		g_pPatchDownloadThread = new CDnPatchDownloadThread();
		g_pPatchDownloadThread->SetWindowHandle( GetSafeHwnd() );
		g_pPatchDownloadThread->Start();
	}
}

void CDnLauncherDlg::CtlEraseBkgnd( int nCtlID )
{
	RECT rect;
	GetDlgItem( nCtlID )->GetWindowRect( &rect );
	this->ScreenToClient( &rect );
	InvalidateRect( &rect );
}

void CDnLauncherDlg::RefreshTextInfo()
{
#ifdef _KRAZ
	m_strDirectGameStart = _S( STR_AFTER_UPDATE_ATUOSTART + DNPATCHINFO.GetLanguageOffset() );
	m_strTimeLeft.Format(L"　");
#endif
	UpdateDownloadInfo();

	CString str; 
	GetDlgItem( IDC_LAUNCHER_VERSION )->GetWindowText( str );
	if( str != m_strLauncherVersion ) CtlEraseBkgnd( IDC_LAUNCHER_VERSION );

	GetDlgItem( IDC_FILE_NAME )->GetWindowText( str );
	if( str != m_strFileName ) CtlEraseBkgnd( IDC_FILE_NAME );

	GetDlgItem( IDC_FILE_SIZE )->GetWindowText( str );
	if( str != m_strFileSize ) 	CtlEraseBkgnd( IDC_FILE_SIZE );

	GetDlgItem( IDC_TIME_LEFT )->GetWindowText( str );
	if( str != m_strTimeLeft ) CtlEraseBkgnd( IDC_TIME_LEFT );
	
	GetDlgItem( IDC_FILE_COUNT )->GetWindowText( str );
	if( str != m_strFileCount ) CtlEraseBkgnd( IDC_FILE_COUNT );

	GetDlgItem( IDC_DOWNLOAD_PERCENTAGE )->GetWindowText( str );
	if( str != m_strDownloadPercentage ) CtlEraseBkgnd( IDC_DOWNLOAD_PERCENTAGE );

	GetDlgItem( IDC_DOWNLOAD_STATE )->GetWindowText( str );
	if( str != m_strDownloadState ) CtlEraseBkgnd( IDC_DOWNLOAD_STATE );

	GetDlgItem( IDC_DIRECT_START)->GetWindowText( str );
	if( str != m_strDirectGameStart) CtlEraseBkgnd( IDC_DIRECT_START);

#ifdef _USE_PARTITION_SELECT
	GetDlgItem( IDC_SELECT_PARTITION )->GetWindowText( str );
	if( str != m_strSelectPartition ) CtlEraseBkgnd( IDC_SELECT_PARTITION );
	GetDlgItem( IDC_SELECT_PARTITION_NAME )->GetWindowText( str );
	if( str != m_strSelectPartitionName ) CtlEraseBkgnd( IDC_SELECT_PARTITION_NAME );
	GetDlgItem( IDC_SELECT_PARTITION_GUIDE )->GetWindowText( str );
	if( str != m_strSelectPartitionGuide ) CtlEraseBkgnd( IDC_SELECT_PARTITION_GUIDE );
#endif // _USE_PARTITION_SELECT

	UpdateData( FALSE );
}

LRESULT CDnLauncherDlg::OnStatusMsg( WPARAM wParam, LPARAM lParam )
{
	m_emDownloadPatchState = static_cast<DownloadPatchState>(wParam);

	if( g_DownloadStateString[m_emDownloadPatchState].nStringID + DNPATCHINFO.GetLanguageOffset() != -1 )
	{
#if !defined(_KOR) || !defined(_IDN) || !defined(_CHN) || !defined(_KRAZ) || defined(_EU)
		m_strDownloadState = _S( g_DownloadStateString[m_emDownloadPatchState].nStringID + DNPATCHINFO.GetLanguageOffset() );
#endif // _KOR, _IDN
	}
	UpdateDownloadInfo();

	switch( m_emDownloadPatchState )
	{
	case PATCH_NONE:
		ResetDownloadInfo();
		m_bDownloading = FALSE;
		break;
	case PATCH_CHECK_VERSION:
		m_bDownloading = TRUE;
		break;
	case PATCH_COMPLETE:
		{
			EnableStartButton();
			SAFE_DELETE( g_pPatchDownloadThread );

			m_pDownloadProgress->SetRange( 0, 100 );
			m_pDownloadProgress->SetPos( 100 );
			m_pFileCountProgress->SetRange( 0, 100 );
			m_pFileCountProgress->SetPos( 100 );
			m_strDownloadPercentage.Format( _T("%d%%"), 100 );

			if(	DNOPTIONDATA.m_bAutoGameStart )	// 자동시작 체크되있을 경우 시작하도록 한다.
				OnClickButtonGameStart();

			m_bDownloading = FALSE;
		}
		break;
	case PATCH_FAILED:
	case PATCH_TERMINATE:
		{
			SAFE_DELETE( g_pPatchDownloadThread );
			m_bDownloading = FALSE;
		}
		break;
	case PATCH_LAUNCHER:
		{
			CDialog::DestroyWindow();
			m_bDownloading = FALSE;
		}
		break;
	}

	return 0;
}

void CDnLauncherDlg::ResetDownloadInfo()
{
	m_strFileName.Empty();
	m_strFileSize.Empty();
	m_strTimeLeft.Empty();
	m_strFileCount.Empty();
	m_strDownloadPercentage.Empty();
	m_strDownloadState.Empty();

	if( m_pDownloadProgress )
	{
		m_pDownloadProgress->SetRange( 0, 100 );
		m_pDownloadProgress->SetPos( 0 );
	}
	if( m_pFileCountProgress )
	{
		m_pFileCountProgress->SetRange( 0, 100 );
		m_pFileCountProgress->SetPos( 0 );
	}
}

void CDnLauncherDlg::UpdateDownloadInfo()
{
	// 다운로드 중 - 파일, 사이즈, 남은시간 표기
	if( m_emDownloadPatchState == PATCH_DOWNLOAD_DATA || m_emDownloadPatchState == PATCH_MD5_CHECK
		|| m_emDownloadPatchState == PATCH_APPLY_PAK_FILE )
	{
#if defined(_KOR) || defined(_IDN) || defined(_CHN) || defined(_KRAZ) || defined(_EU)
		if( g_DownloadInfoData.m_strFileName.GetLength() > 0 )
			m_strFileName = g_DownloadInfoData.m_strFileName;
		if( g_DownloadInfoData.m_nFileSize > 0 )
			m_strFileSize = GetAdjustFileSize( (double)g_DownloadInfoData.m_nFileSize );
#else // _KOR, _IDN
		if( g_DownloadInfoData.m_strFileName.GetLength() > 0 )
			m_strFileName.Format( _T("%s:%s"), _S( STR_DLG_FILENAME + DNPATCHINFO.GetLanguageOffset() ), g_DownloadInfoData.m_strFileName );
		if( g_DownloadInfoData.m_nFileSize > 0 )
			m_strFileSize.Format( _T("%s:%s"), _S( STR_DLG_FILE_SIZE + DNPATCHINFO.GetLanguageOffset() ), GetAdjustFileSize( (double)g_DownloadInfoData.m_nFileSize ) );
#endif // _KOR, _IDN
		m_fDownloadRate = g_DownloadInfoData.m_fDownloadRate;
		UpdateDownloadLeftTime();

		// 다운로드 중 - 프로그래스바
		m_nDownloadProgressMin = g_DownloadInfoData.m_nDownloadProgressMin;
		m_nDownloadProgressMax = g_DownloadInfoData.m_nDownloadProgressMax;
		m_nCurrentFileCount = g_DownloadInfoData.m_nCurrentFileCount;
		m_nTotalFileCount = g_DownloadInfoData.m_nTotalFileCount;
		UpdatProgressInfo();
	}

#if defined (_KOR) || defined(_IDN) || defined(_CHN) || defined(_KRAZ) || defined(_EU)
	if( g_DownloadStateString[m_emDownloadPatchState].nStringID != -1 )
	{
		if( m_nCurrentFileCount > 0 && m_nTotalFileCount > 0 )
		{
			CString strCount;
			strCount.Format( _T("(%d/%d)"), m_nCurrentFileCount, m_nTotalFileCount );
			m_strDownloadState.Format( _T("%s %s"), _S( g_DownloadStateString[m_emDownloadPatchState].nStringID + DNPATCHINFO.GetLanguageOffset() ), strCount );
		}
		else
		{
			m_strDownloadState = _S( g_DownloadStateString[m_emDownloadPatchState].nStringID + DNPATCHINFO.GetLanguageOffset() );
		}
	}
#endif // _KOR, _IDN

#if defined(_KOR) || defined(_THAI) || defined(_IDN) || defined(_CHN) || defined(_KRAZ) || defined(_EU)
	// 패치 데이터 설치 완료 후
	if( m_emDownloadPatchState == PATCH_APPLY_PAK_FILE_END )
		m_strLauncherVersion.Format( _T("Ver.%d"), DNPATCHINFO.GetClientVersion() );
#else // _KOR, _THAI, _IDN
	// 패치 데이터 설치 완료 후
	if( m_emDownloadPatchState == PATCH_APPLY_PAK_FILE_END )
		m_strLauncherVersion.Format( _T("%s Ver.%d"), _S( STR_DLG_DRAGON_NEST + DNPATCHINFO.GetLanguageOffset() ), DNPATCHINFO.GetClientVersion() );
#endif // _KOR, _THAI, _IDN
}

void CDnLauncherDlg::UpdatProgressInfo()
{
	if( m_pDownloadProgress == NULL || m_pFileCountProgress == NULL )
		return;

	if( m_nDownloadProgressMax == 0 || m_nTotalFileCount == 0 )		// error divide by zero
		return;

	// 다운로드 프로그래스바 & %표기 업데이트
	m_pFileCountProgress->SetRange( 0, 100 );
	int nDownloadPercentage = 0;
	if( m_emDownloadPatchState == PATCH_DOWNLOAD_DATA || PATCH_APPLY_PAK_FILE )
		nDownloadPercentage = static_cast<int>( (100 * m_nDownloadProgressMin) / m_nDownloadProgressMax );
	else if( m_emDownloadPatchState == PATCH_MD5_CHECK )
		nDownloadPercentage = 100;

	if( nDownloadPercentage < 0 ) nDownloadPercentage = 0;
	if( nDownloadPercentage > 100 ) nDownloadPercentage = 100;

	m_strDownloadPercentage.Format( _T("%d%%"), nDownloadPercentage );
	m_pDownloadProgress->SetPos( nDownloadPercentage );

	// 총 파일 수 프로그래스바 업데이트 (파일당 구간을 100으로 쪼개 50은 다운로드 50은 패킹적용에 할당)
	m_pFileCountProgress->SetRange( 0, m_nTotalFileCount * 100 );	// 파일 갯수 구간을 100으로 나눔
	int nCurrentFileCountPercentage = 0;

	if( m_emDownloadPatchState == PATCH_DOWNLOAD_DATA || m_emDownloadPatchState == PATCH_MD5_CHECK )
		nCurrentFileCountPercentage = ((m_nCurrentFileCount - 1) * 100) + (nDownloadPercentage / 2);
	else if( m_emDownloadPatchState == PATCH_APPLY_PAK_FILE )
		nCurrentFileCountPercentage = ((m_nCurrentFileCount - 1) * 100) + (nDownloadPercentage / 2) + 50;

	if( nCurrentFileCountPercentage < 0 ) nCurrentFileCountPercentage = 0;
	if( nCurrentFileCountPercentage > m_nTotalFileCount * 100 ) nCurrentFileCountPercentage = m_nTotalFileCount * 100;

	m_pFileCountProgress->SetPos( nCurrentFileCountPercentage );
	m_strFileCount.Format( _T("%d/%d"), m_nCurrentFileCount, m_nTotalFileCount );

#if defined(_KOR) || defined(_IDN) || defined(_CHN) || defined(_KRAZ) || defined(_EU)
	nDownloadPercentage = static_cast<int>( (100 * nCurrentFileCountPercentage) / ( m_nTotalFileCount * 100 ) );
	if( nDownloadPercentage < 0 ) nDownloadPercentage = 0;
	if( nDownloadPercentage > 100 ) nDownloadPercentage = 100;
	m_strDownloadPercentage.Format( _T("%d%%"), nDownloadPercentage );
	m_strFileCount = L"";
#endif // _KOR, _IDN
}

CString CDnLauncherDlg::GetAdjustFileSize( double size )
{
	CString str;

#ifdef _CHN
	if( size >= 1024 * 1024 )
		str.Format( _T("%.1f M") , size / (1024.0f * 1024.0f) );
	else if( size >= 1024 )
		str.Format( _T("%.1f K") , size / 1024.0f );
	else
		str.Format( _T("%d B") , size );
#else // _CHN
	if( size >= 1024 * 1024 )
		str.Format( _T("%.1f MB") , size / (1024.0f * 1024.0f) );
	else if( size >= 1024 )
		str.Format( _T("%.1f KB") , size / 1024.0f );
	else
		str.Format( _T("%d B") , size );
#endif // _CHN

	return str;
}

void CDnLauncherDlg::UpdateDownloadLeftTime()
{
	if( m_fDownloadRate > 0 )
	{
		// KB/s 이므로 1000으로 나눠 줌
		int nEstimateTimeLeft = static_cast<int>( static_cast<float>( m_nDownloadProgressMax - m_nDownloadProgressMin ) / m_fDownloadRate / 1000.0f );

		if( nEstimateTimeLeft > 3600 )
		{
			int nHour = nEstimateTimeLeft / 3600;
			int nMin = (nEstimateTimeLeft - (nHour * 3600)) / 60;
			int nSec = nEstimateTimeLeft - (nHour * 3600) - (nMin * 60);
			m_strTimeLeft.Format( _S( STR_DLG_TIME_HOUR_MIN_SEC + DNPATCHINFO.GetLanguageOffset() ), nHour, nMin, nSec );
		}
		else if( nEstimateTimeLeft <= 3600 && nEstimateTimeLeft > 60 )
		{
			int nMin = nEstimateTimeLeft / 60;
			int nSec = nEstimateTimeLeft - (nMin * 60);
			m_strTimeLeft.Format( _S( STR_DLG_TIME_MIN_SEC + DNPATCHINFO.GetLanguageOffset() ), nMin, nSec );
		}
		else if( nEstimateTimeLeft <= 60 && nEstimateTimeLeft >= 0)
			m_strTimeLeft.Format( _S( STR_DLG_TIME_SEC + DNPATCHINFO.GetLanguageOffset() ), nEstimateTimeLeft );
	}
}

BOOL CDnLauncherDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if(m_BtnStartGame && m_BtnStartGame.IsWindowVisible())
		{
			OnClickButtonGameStart();
			return true;
		}
		return true;
	}

	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		if(m_BtnClose && m_BtnClose.IsWindowVisible())
		{
			OnClickButtonClose();
			return true;
		}
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
