// AutoUpdateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DnAutoUpdate.h"
#include "DnAutoUpdateDlg.h"
#include "CHttpDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CString				g_strHttpPath;
CString				g_strLocalPath;
vector<int>			g_vecUpdateVersion;
CString				g_strCurrentDownloadFileName;
int					g_nCurrentDownloadCount;

CHttpDownload		g_HttpDownload;
CDnAutoUpdateDlg*	g_pMainDialog;

// Download Thread
UINT threadDownloadProc( LPVOID pParam )
{
	HINTERNET hInternetSession;
	// 세션 열기
	hInternetSession = InternetOpen( NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );

	if( hInternetSession != NULL )
	{
		int nRtn = CHttpDownload::HTTPDOWNLOAD_ERROR;
		int nVersionCount = static_cast<int>( g_vecUpdateVersion.size() );
		for( int i=0; i<nVersionCount; i++ )
		{
			g_HttpDownload.Reset();

			CString strVersion;
			strVersion.Format( _T( "%08d" ), g_vecUpdateVersion[i] );

			CString strUrl;
			strUrl += g_strHttpPath;
			strUrl += strVersion;
			strUrl += _T("/");

			CString strPatchFileUrl, strPatchFileName;
			strPatchFileUrl.Format( _T( "%sPatch%08d.pak" ), strUrl.GetBuffer(), g_vecUpdateVersion[i] );
			strPatchFileName.Format( _T( "%sPatch%08d.pak.tmp" ), g_strLocalPath, g_vecUpdateVersion[i] );

			g_nCurrentDownloadCount = i + 1;
			g_strCurrentDownloadFileName = strPatchFileName;
			
			for( int i=0; i<RETRY_MAX_COUNT; i++ )
			{
				nRtn = g_HttpDownload.DownloadFile( hInternetSession, strPatchFileUrl, strPatchFileName );
				if( nRtn == CHttpDownload::HTTPDOWNLOAD_SUCCESS )
					break;
			}

			if( nRtn == CHttpDownload::HTTPDOWNLOAD_ERROR || nRtn == CHttpDownload::HTTPDOWNLOAD_TERMINATE )
				break;
		}

		InternetCloseHandle( hInternetSession );
		if( g_pMainDialog )
			g_pMainDialog->DownloadComplete( nRtn );
	}

	return 0;
}



// CDnAutoUpdateDlg dialog

CDnAutoUpdateDlg::CDnAutoUpdateDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDnAutoUpdateDlg::IDD, pParent), m_bShowWindow( FALSE ), m_bShowTrayTooltip( FALSE ), m_pThread( NULL ), m_bDownloadEnd( FALSE ), m_dwExitTick( 0 )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	g_pMainDialog = this;
}

void CDnAutoUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_PROGRESS1, m_progressCtrl );
}


BEGIN_MESSAGE_MAP(CDnAutoUpdateDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
	ON_MESSAGE( WM_NOTIFYICON_MSG, OnTrayNotifyMsg )
	ON_COMMAND( ID_AUTOUPDATE_EXIT, OnExit )
	ON_COMMAND( ID_AUTOUPDATE_PAUSE, &CDnAutoUpdateDlg::OnBnClickedButtonPause )
	ON_COMMAND( ID_AUTOUPDATE_RESUME, &CDnAutoUpdateDlg::OnBnClickedButtonResume )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED( IDC_BUTTON_PAUSE, &CDnAutoUpdateDlg::OnBnClickedButtonPause )
	ON_BN_CLICKED( IDC_BUTTON_RESUME, &CDnAutoUpdateDlg::OnBnClickedButtonResume )
	ON_BN_CLICKED( IDC_BUTTON_EXIT, &CDnAutoUpdateDlg::OnBnClickedButtonExit )
END_MESSAGE_MAP()

void CDnAutoUpdateDlg::OnDestroy()
{
	// Thread Terminate
	if( m_pThread )
	{
		g_HttpDownload.SetExit( TRUE );
		::WaitForSingleObject( m_pThread->m_hThread, INFINITE );
	}

	Shell_NotifyIcon( NIM_DELETE, &m_stNID );	// 트레이 해제

	CDialog::OnDestroy();
}
// CDnAutoUpdateDlg message handlers

void CDnAutoUpdateDlg::OnClose()
{
	ShowWindowEx( SW_HIDE );
}

BOOL CDnAutoUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// 트레이 풍선말 표시
	m_stNID.cbSize = sizeof( NOTIFYICONDATA );
	m_stNID.hWnd = this->GetSafeHwnd();;
	m_stNID.dwInfoFlags = NIIF_USER; //풍선에 뜰 아이콘을 사용자 지정 아이콘으로
	m_stNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO; 
	m_stNID.uID = IDR_MAINFRAME;
	m_stNID.uCallbackMessage = WM_NOTIFYICON_MSG;
	m_stNID.hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_MAINFRAME) );
	m_stNID.uTimeout = 4000;

	CString strInfoTitle(_T("DragonNest"));
	CString strInfo(_T("AutoUpdate Start!"));
	lstrcpyn( m_stNID.szInfoTitle, (LPCTSTR)strInfoTitle, sizeof( m_stNID.szInfoTitle ) );
	lstrcpyn( m_stNID.szInfo, (LPCTSTR)strInfo, sizeof( m_stNID.szTip ) );

	Shell_NotifyIcon( NIM_ADD, &m_stNID );

	m_TrayMenu.LoadMenu(IDR_MENU1);

	// 다운로드 쓰레드 생성
	m_pThread = AfxBeginThread( threadDownloadProc , this );

	SetTimer( 1, 500, 0 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDnAutoUpdateDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDnAutoUpdateDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDnAutoUpdateDlg::OnExit()
{
	CDialog::OnOK();
}

LRESULT CDnAutoUpdateDlg::OnTrayNotifyMsg( WPARAM wParam, LPARAM lParam )
{
	UINT uMsg = (UINT) lParam; 
	switch( uMsg ) 
	{
	case WM_LBUTTONDBLCLK:
		ShowWindowEx( SW_SHOW );
		break;
	case WM_RBUTTONUP:
		{
			CPoint pt;    
			GetCursorPos( &pt );
			m_TrayMenu.GetSubMenu( 0 )->TrackPopupMenu( TPM_RIGHTALIGN | TPM_LEFTBUTTON ,pt.x, pt.y, this );
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( !m_bShowTrayTooltip )
			{
				Shell_NotifyIcon( NIM_DELETE, &m_stNID );
				m_stNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
				Shell_NotifyIcon( NIM_ADD, &m_stNID );
				m_bShowTrayTooltip = TRUE;
			}
		}
		break;
	} 
	return TRUE;
}

void CDnAutoUpdateDlg::OnWindowPosChanging( WINDOWPOS FAR* lpwndpos )
{
	CDialog::OnWindowPosChanging(lpwndpos);

	if( m_bShowWindow )
		lpwndpos->flags |= SWP_SHOWWINDOW;
	else
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
} 

BOOL CDnAutoUpdateDlg::ShowWindowEx( int nCmdShow )
{
	m_bShowWindow = ( nCmdShow == SW_SHOW );
	return (GetSafeHwnd()) ? ShowWindow( nCmdShow ) : TRUE;
}

void CDnAutoUpdateDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
	if( (nID & 0xFFF0) == SC_MINIMIZE )	// 최소화 버튼은 걸러서 Hide 처리만 한다.
		ShowWindowEx( SW_HIDE );
	else
		CDialog::OnSysCommand( nID, lParam );
}
void CDnAutoUpdateDlg::OnBnClickedButtonPause()
{
	if( m_pThread )
		SuspendThread( m_pThread->m_hThread );
}

void CDnAutoUpdateDlg::OnBnClickedButtonResume()
{
	if( m_pThread )
		ResumeThread( m_pThread->m_hThread );
}

void CDnAutoUpdateDlg::OnBnClickedButtonExit()
{
	CDialog::OnOK();
}

void CDnAutoUpdateDlg::OnTimer( UINT_PTR nIDEvent )
{
	CDialog::OnTimer(nIDEvent);

	m_progressCtrl.SetRange32( 0, g_HttpDownload.GetTotalFileSize() );
	m_progressCtrl.SetPos( g_HttpDownload.GetTotalDownloadBytes() );
	SetDownloadInfo( g_HttpDownload.GetDownloadRate(), g_HttpDownload.GetDownloadPercent() );

	if( m_bDownloadEnd )
	{
		if( GetTickCount() - m_dwExitTick > 5000 )
			CDialog::OnOK();
	}
}

void CDnAutoUpdateDlg::SetDownloadInfo( float fRate, float fPercent )
{
	CString strState;
	if( fPercent < 10.0f )
		strState.Format( L"Download Patch : %1.0f%%\n[%4.2f KB/s]", fPercent, fRate );
	else if( fPercent < 100.0f )
		strState.Format( L"Download Patch : %2.0f%%\n[%4.2f KB/s]", fPercent, fRate );
	else
		strState.Format( L"Download Patch : %3.0f%%\n[%4.2f KB/s]", fPercent, fRate );

	GetDlgItem( IDC_STATIC_STATE )->SetWindowText( strState );
	GetDlgItem( IDC_STATIC_FILENAME )->SetWindowText( g_strCurrentDownloadFileName );
	strState.Format( L"%d / %d", g_nCurrentDownloadCount, g_vecUpdateVersion.size() );
	GetDlgItem( IDC_STATIC_DOWNLOAD_COUNT )->SetWindowText( strState );

	if( m_bShowTrayTooltip )
	{
		m_stNID.dwInfoFlags = NIIF_NONE;
		m_stNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_stNID.uTimeout = 0;
		lstrcpyn( m_stNID.szTip, (LPCTSTR)strState, sizeof( m_stNID.szTip ) );
		DWORD dwMessage = NIM_MODIFY;
		Shell_NotifyIcon( dwMessage, &m_stNID );
	}
}

void CDnAutoUpdateDlg::DownloadComplete( int nRtn )
{
	Shell_NotifyIcon( NIM_DELETE, &m_stNID );

	m_stNID.dwInfoFlags = NIIF_USER; //풍선에 뜰 아이콘을 사용자 지정 아이콘으로
	m_stNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO; 
	m_stNID.hIcon = LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_MAINFRAME) );
	m_stNID.uTimeout = 4000;

	CString strInfoTitle( _T("DragonNest") );
	CString strInfo;
	if( nRtn == CHttpDownload::HTTPDOWNLOAD_SUCCESS || nRtn == CHttpDownload::HTTPDOWNLOAD_ALREADY_DOWNLOAD )
		strInfo = _T("AutoUpdate Complete!");
	else
		strInfo = _T("AutoUpdate Failed!");

	lstrcpyn( m_stNID.szInfoTitle, (LPCTSTR)strInfoTitle, sizeof( m_stNID.szInfoTitle ) );
	lstrcpyn( m_stNID.szInfo, (LPCTSTR)strInfo, sizeof( m_stNID.szTip ) );

	Shell_NotifyIcon( NIM_ADD, &m_stNID );
	m_bShowTrayTooltip = FALSE;
	m_bDownloadEnd = TRUE;
	m_dwExitTick = GetTickCount();
}