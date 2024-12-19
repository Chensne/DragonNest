// TorrentTestDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TorrentTest.h"
#include "TorrentTestDlg.h"
#include "WLibTorrent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTorrentTestDlg 대화 상자




CTorrentTestDlg::CTorrentTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTorrentTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTorrentTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTorrentTestDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTorrentTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CTorrentTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTorrentTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTorrentTestDlg::OnBnClickedButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CTorrentTestDlg 메시지 처리기
void CTorrentTestDlg::OnDestroy()
{
	m_vecPeers.clear();
}

BOOL CTorrentTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem( IDC_LIST2 );
	if( pListCtrl )
	{
		pListCtrl->InsertColumn( 0, L"IP", LVCFMT_CENTER, 140 );
		pListCtrl->InsertColumn( 1, L"Port", LVCFMT_CENTER, 80 );
		pListCtrl->InsertColumn( 2, L"Seed", LVCFMT_CENTER, 60 );
		pListCtrl->InsertColumn( 3, L"Upload", LVCFMT_CENTER, 100 );
		pListCtrl->InsertColumn( 4, L"Download", LVCFMT_CENTER, 100 );
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTorrentTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTorrentTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTorrentTestDlg::OnBnClickedOk()
{
	CString strFilter = L"Torrent Files (*.torrent)|*.torrent||";
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, strFilter );
	if( IDOK == dlg.DoModal() )
	{
		m_strTorrentName = dlg.GetPathName();
	}

	if( m_strTorrentName.GetLength() == 0 )
		return;

	if( WLibTorrent::GetInstance()->InitSession( L"test" ) )
	{
		if( WLibTorrent::GetInstance()->AddTorrent( m_strTorrentName.GetBuffer( 0 ), m_strSavePath.GetBuffer( 0 ) ) )
		{
			MessageBox( L"Success AddTorrent" );
		}
	}

	GetDlgItem( IDC_STATIC_NAME )->SetWindowText( m_strTorrentName );
	CProgressCtrl* pProgressBar = (CProgressCtrl*)GetDlgItem( IDC_PROGRESS1 );
	if( pProgressBar )
		pProgressBar->SetRange( 1,100 );

	SetTimer( PROGRESS_TIMER, 100, NULL ); // TimerID = 1000; TimerTick = 100ms
}

void CTorrentTestDlg::OnTimer( UINT_PTR nIDEvent )
{
	if( nIDEvent == PROGRESS_TIMER )
	{
		float fPercent = 0.0f;
		torrent_status TorrentStatus;
		libtorrent::torrent_handle* pTorrentHandle = WLibTorrent::GetInstance()->GetTorrentHandle( m_strTorrentName.GetBuffer(0) );
		if( pTorrentHandle )
		{
			TorrentStatus = pTorrentHandle->status();
			fPercent = TorrentStatus.progress_ppm / 10000.f;

			CString strState;
			switch( TorrentStatus.state )
			{
				case torrent_status::queued_for_checking:
					strState = L"queued_for_checking";
					break;
				case torrent_status::checking_files:
					strState = L"checking_files";
					break;
				case torrent_status::downloading_metadata:
					strState = L"downloading_metadata";
					break;
				case torrent_status::downloading:
					strState = L"downloading";
					break;
				case torrent_status::finished:
					strState = L"finished";
					break;
				case torrent_status::seeding:
					strState = L"seeding";
					break;
				case torrent_status::allocating:
					strState = L"allocating";
					break;
				case torrent_status::checking_resume_data:
					strState = L"checking_resume_data";
					break;
			}

			GetDlgItem( IDC_STATIC_STATE )->SetWindowText( strState );
		}

		int nPercent = static_cast<int>( fPercent );
		if( nPercent > 100 )
			nPercent = 100;

		CProgressCtrl* pProgressBar = (CProgressCtrl*)GetDlgItem( IDC_PROGRESS1 );
		if( pProgressBar )
			pProgressBar->SetPos( nPercent );

		CString strPercent;
		strPercent.Format( L"%.2f%%", fPercent );
		GetDlgItem( IDC_STATIC_PERCENT )->SetWindowText( strPercent );

		if( TorrentStatus.state != torrent_status::queued_for_checking && TorrentStatus.state != torrent_status::checking_files )
		{
			CString strDownloadInfo;
			CString strDownloadRate;
			CString strConnectionInfo;

			strDownloadInfo.Format( L"Total Download : %s | Total Upload : %s", AddSuffix( TorrentStatus.total_download ), AddSuffix( TorrentStatus.total_upload ) );
			strDownloadRate.Format( L"Download Rate : %s | Upload Rate : %s", AddSuffix( TorrentStatus.download_rate, "/s" ), AddSuffix( TorrentStatus.upload_rate, "/s" ) );
			strConnectionInfo.Format( L"Peers : %d | Seeds : %d", TorrentStatus.num_peers, TorrentStatus.num_seeds );

			GetDlgItem( IDC_STATIC_DOWNLOAD )->SetWindowText( strDownloadInfo );
			GetDlgItem( IDC_STATIC_DOWNLOAD_RATE )->SetWindowText( strDownloadRate );
			GetDlgItem( IDC_STATIC_CONNECTION )->SetWindowText( strConnectionInfo );
		}

		// Set Peers, Seeds Info
		std::vector<peer_info> peers;
		if( TorrentStatus.state != torrent_status::seeding )
		{
			pTorrentHandle->get_peer_info( peers );
		}

		if( CheckPeers( peers ) )	// Update Peers
		{
			CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem( IDC_LIST2 );
			pListCtrl->DeleteAllItems();

			CString strIP;
			CString strPort;
			if( pListCtrl && !m_vecPeers.empty() )
			{
				int nIndex = 0;
				for( std::vector<peer_info>::const_iterator i = m_vecPeers.begin(); i != m_vecPeers.end(); ++i )
				{
					if( i->flags & (peer_info::handshake | peer_info::connecting | peer_info::queued) )
						continue;
					boost::system::error_code ec;

					wchar_t wChar[250];
					MultiByteToWideChar( CP_ACP, 0, i->ip.address().to_string( ec ).c_str(), -1, wChar, 250 );
					strIP.Format( L"%-22s", wChar );
					strPort.Format( L"%-5d", i->ip.port() );
					pListCtrl->InsertItem( nIndex, strIP );
					pListCtrl->SetItemText( nIndex, 1, strPort );
					if( i->flags & peer_info::seed )
						pListCtrl->SetItemText( nIndex, 2, L"Seed" );
					pListCtrl->SetItemText( nIndex, 3, AddSuffix( i->up_speed, "/s" ) );
					pListCtrl->SetItemText( nIndex, 4, AddSuffix( i->down_speed, "/s" ) );

					nIndex++;

/*					snprintf(str, sizeof(str)
					, "%s%s (%s|%s) %s%s (%s|%s) %s%3d (%3d) %3d %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c %c%c%c%c%c%c "
					, esc("32"), add_suffix(i->down_speed, "/s").c_str()
					, add_suffix(i->total_download).c_str(), add_suffix(i->download_rate_peak, "/s").c_str()
					, esc("31"), add_suffix(i->up_speed, "/s").c_str(), add_suffix(i->total_upload).c_str()
					, add_suffix(i->upload_rate_peak, "/s").c_str(), esc("0")
*/
				}
			}
		}
		peers.clear();

//		if( nPercent == 100 )
//			KillTimer( PROGRESS_TIMER );
	}
}

bool CTorrentTestDlg::CheckPeers( std::vector<peer_info>& peers )
{
	if( m_vecPeers.size() != peers.size() )
	{
		m_vecPeers = peers;
		return true;
	}

	bool bSamePeers = true;
	for( std::vector<peer_info>::const_iterator i = m_vecPeers.begin(); i != m_vecPeers.end(); ++i )
	{
		bool bFindIP = false;
		for( std::vector<peer_info>::const_iterator i2 = peers.begin(); i2 != peers.end(); ++i2 )
		{
			if( i->ip.address() == i2->ip.address() )
			{
				bFindIP = true;
				break;
			}
		}

		if( !bFindIP )
		{
			bSamePeers = false;
			break;
		}
	}

	if( !bSamePeers )
	{
		m_vecPeers = peers;
		return true;	
	}
	
	return false;
}

CString CTorrentTestDlg::AddSuffix( float val, char const* suffix )
{
	CString strAddSuffix;
	WCHAR* chrPrefix[] = { L"kB", L"MB", L"GB", L"TB"};
	const int nNumPrefix = sizeof(chrPrefix) / sizeof(WCHAR*);
	for( int i=0; i<nNumPrefix; ++i )
	{
		val /= 1000.f;
		if( std::fabs(val) < 1000.f )
		{
			strAddSuffix.Format( L"%.2f", val );
			strAddSuffix += chrPrefix[i];
			if( suffix ) strAddSuffix += suffix;
			break;
		}
	}

	return strAddSuffix;
}

void CTorrentTestDlg::OnBnClickedButton1()
{
	CString strFileName;
	CString strMakeFileName;

	CString strFilter = L"Make Files (*.*)|*.*||";
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, strFilter );
	if( IDOK == dlg.DoModal() )
	{
		strFileName = dlg.GetPathName();
	}

	if( strFileName.GetLength() == 0 )
		return;

	std::vector<std::wstring> vecWstrWebSeeds;
	std::vector<std::wstring> vecWstrTrackers;
	strMakeFileName = strFileName + L".torrent";

//	vecWstrTrackers.push_back( L"http://172.24.32.20:6881/announce" );

	bool bMake = WLibTorrent::GetInstance()->MakeTorrent( strFileName, vecWstrWebSeeds, vecWstrTrackers, strMakeFileName, L"Test" );

//	std::wstring strTestWebSeed = L"http://dn-nexon14.ktics.co.kr/ReleaseBuild/Patch/00000352/Patch00000352.pak";
//	vecWstrWebSeeds.push_back( strTestWebSeed );

//	bool bMake = WLibTorrent::GetInstance()->MakeTorrent( L"C:\\libtorrent\\Example\\TorrentTest\\Patch00000352.pak", vecWstrWebSeeds, vecWstrTrackers, 
//															L"C:\\libtorrent\\Example\\TorrentTest\\Patch00000352.pak.torrent", L"Test" );
}

void CTorrentTestDlg::OnBnClickedButton2()
{
	ITEMIDLIST* pildBrowse;
	TCHAR pszPathname[MAX_PATH];
	BROWSEINFO bInfo;
	memset( &bInfo, 0, sizeof( bInfo ) );
	bInfo.hwndOwner = GetSafeHwnd();
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = pszPathname;
	bInfo.lpszTitle = _T("디렉토리를 선택하세요");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn = NULL;
	bInfo.lParam = (LPARAM)(LPCTSTR)"C:\\";
	bInfo.lParam = (LPARAM)NULL;
	pildBrowse = ::SHBrowseForFolder(&bInfo);
	if( pildBrowse )
	{
		SHGetPathFromIDList( pildBrowse, pszPathname );

		std::vector<std::wstring> vecWstrWebSeeds;
		std::vector<std::wstring> vecWstrTrackers;
		CString strMakeFileName;
		CString strFileName = pszPathname;
		strMakeFileName = strFileName + L".torrent";
		bool bMake = WLibTorrent::GetInstance()->MakeTorrent( strFileName, vecWstrWebSeeds, vecWstrTrackers, strMakeFileName, L"Test" );
	}
}


void CTorrentTestDlg::OnBnClickedButton3()
{
	ITEMIDLIST* pildBrowse;
	TCHAR pszPathname[MAX_PATH];
	BROWSEINFO bInfo;
	memset( &bInfo, 0, sizeof( bInfo ) );
	bInfo.hwndOwner = GetSafeHwnd();
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = pszPathname;
	bInfo.lpszTitle = _T("디렉토리를 선택하세요");
	bInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	bInfo.lpfn = NULL;
	bInfo.lParam = (LPARAM)(LPCTSTR)"C:\\";
	bInfo.lParam = (LPARAM)NULL;
	pildBrowse = ::SHBrowseForFolder(&bInfo);
	if( pildBrowse )
	{
		SHGetPathFromIDList( pildBrowse, pszPathname );
		m_strSavePath = pszPathname;
		GetDlgItem( IDC_STATIC_PATH )->SetWindowText( m_strSavePath );
	}
}
