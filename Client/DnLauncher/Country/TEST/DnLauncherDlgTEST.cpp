#include "stdafx.h"
#include "DnLauncherDlgTEST.h"
#include "SHFolderDialog.h"

#if defined(_TEST)


CDnLauncherDlgTEST::CDnLauncherDlgTEST( CWnd* pParent )
: CDnLauncherDlg( pParent )
{
	m_nMaxCtrlNum = em_MaxNumEx;
	m_pRectCtrl = new CRect[em_MaxNumEx];
}

void CDnLauncherDlgTEST::DoDataExchange( CDataExchange* pDX )
{
	CDnLauncherDlg::DoDataExchange( pDX );

	DDX_Control( pDX, IDC_BTN_OPENPATH, m_BtnOpenPath );	
	DDX_Text( pDX, IDC_CLIENT_PATH, m_strClientPath );
	DDX_Control( pDX, IDC_CLIENT_PATH, m_StaticClientPath );
}


BEGIN_MESSAGE_MAP( CDnLauncherDlgTEST, CDnLauncherDlg )
	ON_WM_DESTROY()
	ON_BN_CLICKED( IDC_BTN_OPENPATH, &CDnLauncherDlgTEST::OnClickButtonOpenPath )
END_MESSAGE_MAP()


BOOL CDnLauncherDlgTEST::OnInitDialog()
{
	m_strClientPath = DNPATCHINFO.GetClientPath();

	return CDnLauncherDlg::OnInitDialog();
}

void CDnLauncherDlgTEST::InitControl()
{
	CDnLauncherDlg::InitControl();

	m_BtnOpenPath.SetSkinJpg( IDR_JPG_OPEN_PATH, IDR_JPG_OPEN_PATH_CLICK, IDR_JPG_OPEN_PATH_ON );
	m_BtnOpenPath.MoveWindow( m_pRectCtrl[em_BtnOpenPath] );
	m_BtnOpenPath.SetWindowText( L"" );
	
	m_StaticClientPath.SetTextColor( WHITE );
	m_StaticClientPath.SetFont( &m_ClientPathFont );
	m_StaticClientPath.MoveWindow( m_pRectCtrl[em_Text_ClientPath] );
}

void CDnLauncherDlgTEST::MakeFont()
{
	m_Font.CreateFont( 11, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("µ¸¿òÃ¼") );
	m_ClientPathFont.CreateFont( 15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("arial") );
}

void CDnLauncherDlgTEST::RefreshTextInfo()
{
	CString str;
	GetDlgItem( IDC_CLIENT_PATH )->GetWindowText( str );
	if( str != m_strClientPath ) CtlEraseBkgnd( IDC_CLIENT_PATH );

	CDnLauncherDlg::RefreshTextInfo();
}

void CDnLauncherDlgTEST::OnDestroy()
{
	SAFE_DELETE_ARRAY( m_pRectCtrl );
	m_ClientPathFont.DeleteObject();

	CDnLauncherDlg::OnDestroy();
}

void CDnLauncherDlgTEST::OnClickButtonOpenPath()
{
	if( !m_bDownloading )
	{
		CString strSelectedPath;
		if( CSHFolderDialog::BrowseForFolder( strSelectedPath, _T("Choose a Folder:"), m_hWnd, 0, 0, NULL ) == IDOK )
		{
			DNPATCHINFO.SetClientPath( strSelectedPath );
			DNPATCHINFO.LoadClientVersion();
			m_strLauncherVersion.Format( _T("%s Ver.%d"), _S( STR_DLG_DRAGON_NEST ), DNPATCHINFO.GetClientVersion() );
			m_strClientPath = strSelectedPath;
			ResetDownloadInfo();
			CheckVersion();
		}
	}
}

#endif // _TEST