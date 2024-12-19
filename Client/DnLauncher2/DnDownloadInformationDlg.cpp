#include "stdafx.h"
#include "DnDownloadInformationDlg.h"
#include "DnPatchThread.h"


#ifdef _USE_BITTORRENT

extern stDownloadInfoUpdateData	g_DownloadInfoData;

IMPLEMENT_DYNAMIC(CDnDownloadInformationDlg, CDialog)

CDnDownloadInformationDlg::CDnDownloadInformationDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDnDownloadInformationDlg::IDD, pParent)
{

}

CDnDownloadInformationDlg::~CDnDownloadInformationDlg()
{
}

void CDnDownloadInformationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	// Static Text
	DDX_Text( pDX, IDC_STATIC_FILE, m_strTextFile );
	DDX_Text( pDX, IDC_STATIC_STATUS, m_strTextStatus );
	DDX_Text( pDX, IDC_STATIC_DOWNLOAD, m_strTextDownload );
	DDX_Text( pDX, IDC_STATIC_UPLOAD, m_strTextUpload );
	DDX_Text( pDX, IDC_STATIC_PEERS, m_strTextPeers );
	DDX_Text( pDX, IDC_STATIC_SEEDS, m_strTextSeeds );
}


BEGIN_MESSAGE_MAP(CDnDownloadInformationDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OK, &CDnDownloadInformationDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDnDownloadInformationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem( IDC_LIST_PEER );
	if( pListCtrl )
	{
		pListCtrl->InsertColumn( 0, L"IP", LVCFMT_CENTER, 140 );
		pListCtrl->InsertColumn( 1, L"Port", LVCFMT_CENTER, 80 );
		pListCtrl->InsertColumn( 2, L"Download", LVCFMT_CENTER, 100 );
		pListCtrl->InsertColumn( 3, L"Upload", LVCFMT_CENTER, 100 );
		pListCtrl->InsertColumn( 4, L"Seed", LVCFMT_CENTER, 60 );
		pListCtrl->InsertColumn( 5, L"ID", LVCFMT_CENTER, 300 );
	}

	m_strTextFile.Empty();
	m_strTextStatus.Empty();
	m_strTextDownload.Empty();
	m_strTextUpload.Empty();
	m_strTextPeers.Empty();
	m_strTextSeeds.Empty();

	SetTimer( UPDATE_TIMER, 1000, NULL );
	return TRUE;
}

void CDnDownloadInformationDlg::OnDestroy()
{
	m_vecPeers.clear();
}

void CDnDownloadInformationDlg::OnBnClickedOk()
{
	OnOK();
}

void CDnDownloadInformationDlg::CtlEraseBkgnd( int nCtlID )
{
	RECT rect;
	GetDlgItem( nCtlID )->GetWindowRect( &rect );
	this->ScreenToClient( &rect );
	InvalidateRect( &rect );
}

void CDnDownloadInformationDlg::RefreshTextInfo()
{
	CString str; 
	GetDlgItem( IDC_STATIC_FILE )->GetWindowText( str );
	if( str != m_strTextFile ) CtlEraseBkgnd( IDC_STATIC_FILE );

	GetDlgItem( IDC_STATIC_STATUS )->GetWindowText( str );
	if( str != m_strTextStatus ) CtlEraseBkgnd( IDC_STATIC_STATUS );

	GetDlgItem( IDC_STATIC_DOWNLOAD )->GetWindowText( str );
	if( str != m_strTextDownload ) 	CtlEraseBkgnd( IDC_STATIC_DOWNLOAD );

	GetDlgItem( IDC_STATIC_UPLOAD )->GetWindowText( str );
	if( str != m_strTextUpload ) CtlEraseBkgnd( IDC_STATIC_UPLOAD );

	GetDlgItem( IDC_STATIC_PEERS )->GetWindowText( str );
	if( str != m_strTextPeers ) CtlEraseBkgnd( IDC_STATIC_PEERS );

	GetDlgItem( IDC_STATIC_SEEDS )->GetWindowText( str );
	if( str != m_strTextSeeds ) CtlEraseBkgnd( IDC_STATIC_SEEDS );

	UpdateData( FALSE );
}

void CDnDownloadInformationDlg::OnTimer( UINT_PTR nIDEvent )
{
	if( g_DownloadInfoData.m_pTorrentHandle == NULL )
		return;

	if( nIDEvent == UPDATE_TIMER )
	{
		torrent_status torrentStatus = g_DownloadInfoData.m_pTorrentHandle->status();
		float fPercent = torrentStatus.progress_ppm / 10000.f;
		m_strTextFile.Format( L"File : %s (%.2f%%)", g_DownloadInfoData.m_strFileName, fPercent );

		CString strStatus;
		switch( torrentStatus.state )
		{
			case torrent_status::queued_for_checking:
				strStatus = L"queued_for_checking";
				break;
			case torrent_status::checking_files:
				strStatus = L"checking_files";
				break;
			case torrent_status::downloading_metadata:
				strStatus = L"downloading_metadata";
				break;
			case torrent_status::downloading:
				strStatus = L"downloading";
				break;
			case torrent_status::finished:
				strStatus = L"finished";
				break;
			case torrent_status::seeding:
				strStatus = L"seeding";
				break;
			case torrent_status::allocating:
				strStatus = L"allocating";
				break;
			case torrent_status::checking_resume_data:
				strStatus = L"checking_resume_data";
				break;
		}

		m_strTextStatus.Format( L"Status : %s", strStatus );

		if( torrentStatus.state != torrent_status::queued_for_checking && torrentStatus.state != torrent_status::checking_files )
		{
			m_strTextDownload.Format( L"Download : %s", AddSuffix( torrentStatus.download_rate, "/s" ) );
			m_strTextUpload.Format( L"Download : %s", AddSuffix( torrentStatus.upload_rate, "/s" ) );
			m_strTextPeers.Format( L"Peers : %d", torrentStatus.num_peers );
			m_strTextSeeds.Format( L"Seeds : %d", torrentStatus.num_seeds );
		}

		// Set Peers, Seeds Info
		std::vector<peer_info> peers;
		if( torrentStatus.state != torrent_status::seeding )
		{
			g_DownloadInfoData.m_pTorrentHandle->get_peer_info( peers );
		}

		if( CheckPeers( peers ) )	// Update Peers
		{
			CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem( IDC_LIST_PEER );

			CString strIP;
			CString strPort;
			CString strClient;
			boost::system::error_code ec;

			if( pListCtrl && !m_vecPeers.empty() )
			{
				std::vector<int> vecRemoveIndex;
				for( int i=0; i<pListCtrl->GetItemCount(); i++ )
				{
					bool bExist = false;
					CString strListIP = pListCtrl->GetItemText( i, 0 );
					for( std::vector<peer_info>::const_iterator i = m_vecPeers.begin(); i != m_vecPeers.end(); i++ )
					{
						wchar_t wChar[250];
						MultiByteToWideChar( CP_ACP, 0, i->ip.address().to_string( ec ).c_str(), -1, wChar, 250 );
						strIP.Format( L"%-22s", wChar );

						if( strListIP == strIP )
						{
							bExist = true;
							break;
						}
					}
					
					if( !bExist )
						vecRemoveIndex.push_back( i );
				}

				for( int i=0; i<static_cast<int>( vecRemoveIndex.size() ); i++ )
				{
					pListCtrl->DeleteItem( vecRemoveIndex[i] );
				}
				vecRemoveIndex.clear();

				for( std::vector<peer_info>::const_iterator iter = m_vecPeers.begin(); iter != m_vecPeers.end(); iter++ )
				{
					if( iter->flags & (peer_info::handshake | peer_info::connecting | peer_info::queued) )
						continue;

					wchar_t wChar[250];
					MultiByteToWideChar( CP_ACP, 0, iter->ip.address().to_string( ec ).c_str(), -1, wChar, 250 );
					strIP.Format( L"%-22s", wChar );
					strPort.Format( L"%-5d", iter->ip.port() );

					int nItemIndex = -1;
					for( int i=0; i<pListCtrl->GetItemCount(); i++ )
					{
						CString strListIP = pListCtrl->GetItemText( i, 0 );
						if( strListIP == strIP )
						{
							nItemIndex = i;
							break;
						}
					}

					if( nItemIndex == -1 )
					{
						nItemIndex = pListCtrl->GetItemCount();
						pListCtrl->InsertItem( nItemIndex, strIP );
					}
					else
					{
						pListCtrl->SetItemText( nItemIndex, 0, strIP );
					}

					pListCtrl->SetItemText( nItemIndex, 1, strPort );
					pListCtrl->SetItemText( nItemIndex, 2, AddSuffix( iter->down_speed, "/s" ) );
					pListCtrl->SetItemText( nItemIndex, 3, AddSuffix( iter->up_speed, "/s" ) );

					if( iter->flags & peer_info::seed )
						pListCtrl->SetItemText( nItemIndex, 4, L"Seed" );

					MultiByteToWideChar( CP_ACP, 0, iter->client.c_str(), -1, wChar, 250 );
					strClient.Format( L"%-22s", wChar );
					pListCtrl->SetItemText( nItemIndex, 5, strClient );
				}
			}
		}
		peers.clear();
		RefreshTextInfo();
	}
}

bool CDnDownloadInformationDlg::CheckPeers( std::vector<peer_info>& peers )
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

#endif // _USE_BITTORRENT