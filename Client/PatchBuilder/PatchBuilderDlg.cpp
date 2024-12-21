#include "stdafx.h"
#include "PatchBuilder.h"
#include "PatchBuilderDlg.h"
#include "ExportBrowseFolder.h"
#include "NewProfile.h"
#include "RegProfile.h"
#include "SundriesFunc.h"
#include "tinyxml.h"
#include "EtFileSystem.h"
#include "MD5Checksum.h"
#include "DnAttachFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPatchBuilderDlg ��ȭ ����
#define RMAKE_RESULT_NAME "rmakeresult.txt"

bool IsPatchFile( const char *pFileName )
{
	char szCheckFile[ _MAX_PATH ];
	strcpy_s( szCheckFile, _MAX_PATH, pFileName );
	_strlwr_s( szCheckFile, _MAX_PATH );
	if( strncmp( szCheckFile, "mapdata", strlen( "mapdata" ) ) == 0 )
	{
		return true;
	}
	if( strncmp( szCheckFile, "\\mapdata", strlen( "\\mapdata" ) ) == 0 )
	{
		return true;
	}
	if( strncmp( szCheckFile, "resource", strlen( "resource" ) ) == 0 )
	{
		return true;
	}
	if( strncmp( szCheckFile, "\\resource", strlen( "\\resource" ) ) == 0 )
	{
		return true;
	}

	return false;
}

CPatchBuilderDlg::CPatchBuilderDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPatchBuilderDlg::IDD, pParent)
, m_szSVNUrl( _T("") )
, m_szPatchFolder( _T("") )
, m_nLastRevision( 0 )
, m_nStartRevision( 0 )
, m_szRMakeCmd( _T("") )
, m_szCountryCode( _T("") )
, m_szSVNUpdateFolder( _T("") )
, m_bMakeFullVersion( FALSE )
, m_bCopyServerData( FALSE )
, m_szSVNUrlServer( _T("") )
, m_bRollBack( FALSE )
, m_pDnAttachFile( NULL )
, m_szManualPatchExe( _T("") )
, m_bManualPatch( FALSE )
, m_nManualPatchTargetStartVer( 0 )
, m_nManualPatchTargetEndVer( 0 )
, m_bManualTargetVer( FALSE )
, m_bUseRTPatch( FALSE )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurrentVersion = 0;
	m_nDefaultProfile = 0;
	m_bAutoStart = FALSE;
}

void CPatchBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SVN_URL, m_szSVNUrl);
	DDX_Text(pDX, IDC_PATCH_FOLDER, m_szPatchFolder);
	DDX_Control(pDX, IDC_SELECT_PROFILE, m_SelectProfile);
	DDX_Text(pDX, IDC_CURRENT_REVISION, m_nLastRevision);
	DDX_Text(pDX, IDC_BASE_REVISION, m_nStartRevision);
	DDX_Text(pDX, IDC_RMAKE_FOLDER, m_szRMakeCmd);
	DDX_Text(pDX, IDC_COUNTRY_CODE, m_szCountryCode);
	DDX_Text(pDX, IDC_SVN_UPDATE_FOLDER, m_szSVNUpdateFolder);
	DDX_Check(pDX, IDC_FULL_VERSION, m_bMakeFullVersion);
	DDX_Check(pDX, IDC_COPY_SERVER_DATA, m_bCopyServerData);
	DDX_Text(pDX, IDC_SVN_URL_SERVER, m_szSVNUrlServer);
	DDX_Check(pDX, IDC_ROLLBACK, m_bRollBack);
	DDX_Text(pDX, IDC_MANUAL_FOLDER, m_szManualPatchExe);
	DDX_Check(pDX, IDC_MANUAL_PATCH, m_bManualPatch);
	DDX_Check(pDX, IDC_CHECK_MANUAL_TARGET_VER, m_bManualTargetVer);
	DDX_Text(pDX, IDC_EDIT_MANUALPATCH_TARGET_START_VER, m_nManualPatchTargetStartVer);
	DDX_Text(pDX, IDC_EDIT_MANUALPATCH_TARGET_END_VER, m_nManualPatchTargetEndVer);
	DDX_Check(pDX, IDC_CHECK_USE_RTPATCH, m_bUseRTPatch);
}

BEGIN_MESSAGE_MAP(CPatchBuilderDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PATCH_FOLDER_BROWSE, &CPatchBuilderDlg::OnBnClickedPatchFolderBrowse)
	ON_BN_CLICKED(ID_ADD_PROFILE, &CPatchBuilderDlg::OnBnClickedAddProfile)
	ON_BN_CLICKED(ID_MODIFY_PROFILE, &CPatchBuilderDlg::OnBnClickedModifyProfile)
	ON_BN_CLICKED(ID_DELETE_PROFILE, &CPatchBuilderDlg::OnBnClickedDeleteProfile)
	ON_CBN_SELCHANGE(IDC_SELECT_PROFILE, &CPatchBuilderDlg::OnCbnSelchangeSelectProfile)
	ON_BN_CLICKED(ID_BUILD, &CPatchBuilderDlg::OnBnClickedBuild)
	ON_BN_CLICKED(IDC_RMAKE_FOLDER_BROWSE, &CPatchBuilderDlg::OnBnClickedRmakeFolderBrowse)
	ON_BN_CLICKED(IDC_UPDATE_FOLDER_BROWSE, &CPatchBuilderDlg::OnBnClickedUpdateFolderBrowse)
	ON_BN_CLICKED(IDC_FULL_VERSION, &CPatchBuilderDlg::OnBnClickedFullVersion)
	ON_BN_CLICKED(IDC_COPY_SERVER_DATA, &CPatchBuilderDlg::OnBnClickedCopyServerData)
	ON_BN_CLICKED(ID_EXIT, &CPatchBuilderDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ROLLBACK, &CPatchBuilderDlg::OnBnClickedRollback)
	ON_BN_CLICKED(IDC_MANUAL_PATCH, &CPatchBuilderDlg::OnBnClickedManualPatch)
	ON_BN_CLICKED(IDC_IDC_MANUAL_FOLDER_BROWSE2, &CPatchBuilderDlg::OnBnClickedIdcManualFolderBrowse2)
	ON_BN_CLICKED(IDC_CHECK_MANUAL_TARGET_VER, &CPatchBuilderDlg::OnBnClickedCheckManualTargetVer)
END_MESSAGE_MAP()


BOOL CPatchBuilderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	GetModuleRoot();
	SetCurrentDirectoryA(m_szModuleRoot);
	DeleteFile(".\\rmakeresult.txt");

	RefreshProfileList();				//������Ʈ������ ����� ������ ������ �о��
	SetCurProfile( m_nDefaultProfile );	//�ҷ��� �ش� ������ ���� 

	if( m_nCurrentVersion <= 1 )
	{
		GetDlgItem( IDC_BASE_REVISION )->EnableWindow( false );
		GetDlgItem( IDC_FULL_VERSION )->EnableWindow( false );
		GetDlgItem( IDC_COPY_SERVER_DATA )->EnableWindow( false );
	}

	if( m_bAutoStart )
		PostMessage( WM_COMMAND, MAKEWPARAM( ID_BUILD, 1 ) );

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CPatchBuilderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPatchBuilderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPatchBuilderDlg::OnBnClickedUpdateFolderBrowse()
{
	CFolderDialog Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		m_szSVNUpdateFolder = Dlg.GetFolderPath();
		UpdateData( FALSE );
	}
}

void CPatchBuilderDlg::OnBnClickedPatchFolderBrowse()
{
	CFolderDialog Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		m_szPatchFolder = Dlg.GetFolderPath();
		UpdateData( FALSE );
	}
}

void CPatchBuilderDlg::OnBnClickedRmakeFolderBrowse()
{
	CFileDialog FileDlg( TRUE, "*.exe", "RMakeCmd.exe" );
	if( FileDlg.DoModal() == IDOK )
	{
		m_szRMakeCmd = FileDlg.m_ofn.lpstrFile;
		UpdateData( FALSE );
	}
}

void CPatchBuilderDlg::OnBnClickedFullVersion()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeFullVersion", m_bMakeFullVersion );
}

void CPatchBuilderDlg::OnBnClickedCopyServerData()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeServerData", m_bCopyServerData );
}

void CPatchBuilderDlg::OnBnClickedRollback()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "RollBack", m_bRollBack);
}

void CPatchBuilderDlg::OnBnClickedManualPatch()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "ManualPatch", m_bManualPatch);

	if( m_bManualPatch )
		GetDlgItem( IDC_CHECK_MANUAL_TARGET_VER )->EnableWindow( true );
	else
		GetDlgItem( IDC_CHECK_MANUAL_TARGET_VER )->EnableWindow( false );
}

void CPatchBuilderDlg::OnBnClickedCheckManualTargetVer()
{
	UpdateData( TRUE );

	if( !m_bManualPatch )
		return;

	if( m_bManualTargetVer )
	{
		GetDlgItem( IDC_EDIT_MANUALPATCH_TARGET_START_VER )->EnableWindow( true );
		GetDlgItem( IDC_EDIT_MANUALPATCH_TARGET_END_VER )->EnableWindow( true );
	}
	else
	{
		GetDlgItem( IDC_EDIT_MANUALPATCH_TARGET_START_VER )->EnableWindow( false );
		GetDlgItem( IDC_EDIT_MANUALPATCH_TARGET_END_VER )->EnableWindow( false );
	}
}

void CPatchBuilderDlg::RefreshProfileList()
{
	m_SelectProfile.ResetContent();
	int i;
	for( i = 0; i < CRegProfile::GetInstance().GetProfileCount(); i++ )
	{
		SProfile *pProfile = CRegProfile::GetInstance().GetProfile( i );
		m_SelectProfile.AddString( pProfile->szProfileName.c_str() );
	}
}

void CPatchBuilderDlg::SetCurProfile( int nIndex )	//�������� ������ ���´�
{
	m_SelectProfile.SetCurSel( nIndex );

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nIndex );
	if( !pProfile )
		return;

	m_szSVNUrl = pProfile->szSVNUrl.c_str();
	m_szSVNUpdateFolder = pProfile->szSVNUpdateFolder.c_str();
	m_szPatchFolder = pProfile->szPatchFolder.c_str();
	m_szRMakeCmd = pProfile->szRMakeCmd.c_str();
	m_szCountryCode = pProfile->szCountryCode.c_str();
	m_szSVNUrlServer = pProfile->szSVNUrlServer.c_str();
	m_szManualPatchExe = pProfile->szManualPatchExe.c_str();

	CheckSVNInfo( nIndex );
	CheckPatchFolder();

#ifdef _ROLLBACK_PATCH
	LoadPatchInfo();
#endif // _ROLLBACK_PATCH

	UpdateData( FALSE );
}

void CPatchBuilderDlg::OnBnClickedAddProfile()	// �������߿� �ƹ��ų� �����ϸ� �ش� ����� �������� �ε� �Ѵ�. 
{
	CNewProfile Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		UpdateData( TRUE );

		SProfile TempProfile;
		TempProfile.szProfileName = Dlg.m_szProfileName.GetBuffer();
		TempProfile.szSVNUrl = m_szSVNUrl.GetBuffer();
		TempProfile.szSVNUpdateFolder = m_szSVNUpdateFolder.GetBuffer();
		TempProfile.szPatchFolder = m_szPatchFolder.GetBuffer();
		TempProfile.szRMakeCmd = m_szRMakeCmd.GetBuffer();
		TempProfile.szCountryCode = m_szCountryCode.GetBuffer();
		TempProfile.szSVNUrlServer = m_szSVNUrlServer.GetBuffer();
		TempProfile.szManualPatchExe = m_szManualPatchExe.GetBuffer();
			
		CRegProfile::GetInstance().AddProfile( TempProfile );

		RefreshProfileList();
		SetCurProfile( CRegProfile::GetInstance().GetProfileCount() - 1 );

		char szString[ 1024 ];
		sprintf_s( szString, 1024, "Profile %s �߰� �Ǿ����ϴ�", Dlg.m_szProfileName.GetBuffer() );
		MessageBox( szString );
	}
}

void CPatchBuilderDlg::OnBnClickedModifyProfile()	// ������ ����
{
	int nCurSel = m_SelectProfile.GetCurSel();
	if( nCurSel == CB_ERR )
	{
		MessageBox( "���õ� Profile�� �����ϴ�." );
		return;
	}

	UpdateData( TRUE );

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nCurSel );
	pProfile->szSVNUrl = m_szSVNUrl.GetBuffer();
	pProfile->szSVNUpdateFolder = m_szSVNUpdateFolder.GetBuffer();
	pProfile->szPatchFolder = m_szPatchFolder.GetBuffer();
	pProfile->szRMakeCmd = m_szRMakeCmd.GetBuffer();
	pProfile->szCountryCode = m_szCountryCode.GetBuffer();
	pProfile->szSVNUrlServer = m_szSVNUrlServer.GetBuffer();
	pProfile->szManualPatchExe = m_szManualPatchExe.GetBuffer();

	CRegProfile::GetInstance().SaveProfile();

	char szString[ 1024 ];
	sprintf_s( szString, 1024, "Profile %s ���� �Ǿ����ϴ�", pProfile->szProfileName.c_str() );
	MessageBox( szString );
}

void CPatchBuilderDlg::OnBnClickedDeleteProfile()	//������ ���� 
{
	int nCurSel = m_SelectProfile.GetCurSel();
	if( nCurSel == CB_ERR )
	{
		MessageBox( "���õ� Profile�� �����ϴ�." );
		return;
	}

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nCurSel );
	std::string szDelName = pProfile->szProfileName;
	CRegProfile::GetInstance().DeleteProfile( nCurSel );
	RefreshProfileList();
	if( CRegProfile::GetInstance().GetProfileCount() > 0 )
	{
		SetCurProfile( 0 );
	}

	char szString[ 1024 ];
	sprintf_s( szString, 1024, "Profile %s ���� �Ǿ����ϴ�", szDelName.c_str() );
	MessageBox( szString );
}

void CPatchBuilderDlg::OnCbnSelchangeSelectProfile()
{
	int nCurSel = m_SelectProfile.GetCurSel();
	
	if( nCurSel == CB_ERR )
		return;

	SetCurProfile( nCurSel );
}

void CPatchBuilderDlg::GetModifyList()
{
	LogWnd::TraceLog( "GetModifyList" );
	SetWorkStatus( "Log �м����Դϴ�" );

	char szString[ 1024 ];
	std::string szOutput;
	int nStartRevision, nEndRevision;
	nStartRevision = m_nStartRevision + 1;
	nEndRevision = m_nLastRevision;
	if( nStartRevision > nEndRevision )
	{
		nStartRevision = nEndRevision;
	}
#ifdef DH_SELF_TEST
	sprintf_s( szString, 1024, "svn log %s -r %d:%d -v --xml --username hicom15 --password 6326", m_szSVNUrl.GetBuffer(), nStartRevision, nEndRevision );
#else // DH_SELF_TEST
	sprintf_s( szString, 1024, "svn log %s -r %d:%d -v --xml --username build --password b@0625", m_szSVNUrl.GetBuffer(), nStartRevision, nEndRevision );
#endif // DH_SELF_TEST
	LogWnd::TraceLog( "Command:%s", szString );
	ExecCommand( szString, &szOutput );

	int nMainFolderLength = m_szMainFolder.GetLength();
	m_vecModifyFile.clear();
	m_vecDeleteFile.clear();

#ifdef _ROLLBACK_PATCH
	m_vecRollDeleteFile.clear();
	m_vecRestoreFile.clear();
#endif // _ROLLBACK_PATCH

	TiXmlDocument Document;
	Document.Parse( szOutput.c_str(), 0, TIXML_ENCODING_UTF8 );
	if ( !Document.Error() )
	{
		TiXmlElement* pRoot = Document.FirstChildElement( "log" );
		if( pRoot )
		{
			TiXmlElement* pLogEntryElement = pRoot->FirstChildElement( "logentry" );
			while( pLogEntryElement )
			{
				TiXmlElement *pPathsElement = pLogEntryElement->FirstChildElement( "paths" );
				if( pPathsElement )
				{
					TiXmlElement *pPath = pPathsElement->FirstChildElement( "path" );
					while( pPath )
					{
						const char *pAction = pPath->Attribute( "action" );
						if( pAction )
						{
							const char *pFileName = pPath->GetText() + nMainFolderLength;
							if( pFileName[ 0 ] && strstr( pPath->GetText(), m_szMainFolder.GetBuffer() ) && strstr( pPath->GetText(), "." ) )	// ��� Ȯ�� �� ���� �α׿� ���� �ɷ���
							{
								//���������� ���� 
								pFileName++;
								std::vector< std::string >::iterator idModi = std::find( m_vecModifyFile.begin(), m_vecModifyFile.end(), pFileName );
								std::vector< std::string >::iterator itDelete = std::find( m_vecDeleteFile.begin(), m_vecDeleteFile.end(), pFileName );
#ifdef _ROLLBACK_PATCH
								std::vector< std::string >::iterator itRollDelete = std::find( m_vecRollDeleteFile.begin() ,m_vecRollDeleteFile.end() , pFileName ) ; 
								std::vector< std::string >::iterator itRestore = std::find( m_vecRestoreFile.begin() , m_vecRestoreFile.end() , pFileName );
								
								// 'A' �߰����Ȱ��� ������ �ؾߵǰ� M , D �����̳� ������ �Ȱ��� ������ �߱⿡ ������ �ִ´�.
								switch(pAction[0])
								{
									case 'A': 
										if ( itRollDelete == m_vecRollDeleteFile.end() ) 
										{
											m_vecRollDeleteFile.push_back(pFileName);
										}
										break; 
									case 'M':
									case 'R':
									case 'D':
										//�߰����� ������ ������ �����Ǿ� �ϱ⶧���� �������Ͽ� ���� �ʴ´�. 
										if( itRollDelete != m_vecRollDeleteFile.end() )
										{
											break; 
										}

										if( itRestore == m_vecRestoreFile.end())
										{
											m_vecRestoreFile.push_back(pFileName);
										}
										break; 
									default:
										ASSERT( 0 && "�߸��� �׼��ڵ�" );
										break;
								}
#endif // _ROLLBACK_PATCH
								switch( pAction[ 0 ] )
								{
									case 'A':
									case 'M':
									case 'R':
										if( idModi == m_vecModifyFile.end() )
										{
											m_vecModifyFile.push_back( pFileName );
										}

										//���� ����Ʈ ���� �ٽ� �����̳� �߰��Ѱ��� ����Ʈ���� ��������. 
										if( itDelete != m_vecDeleteFile.end() )
										{
											m_vecDeleteFile.erase( itDelete );
										}
										break;
									case 'D':
										//����� ��� ����Ʈ ���� �׼��� ������ 
										if( idModi != m_vecModifyFile.end() )
										{
											m_vecModifyFile.erase( idModi );
										}
										if( itDelete == m_vecDeleteFile.end() )
										{
											m_vecDeleteFile.push_back( pFileName );
										}
										break;
									default:
										ASSERT( 0 && "�߸��� �׼��ڵ�" );
										break;
								}
							}
						}

						TiXmlNode *pPathNode =pPathsElement->IterateChildren( pPath );
					
						if( pPathNode )
							pPath = pPathNode->ToElement();
						else
							break;
					}
				}

				TiXmlNode *pLogEntryNode = pRoot->IterateChildren( pLogEntryElement );
				
				if( pLogEntryNode )
					pLogEntryElement = pLogEntryNode->ToElement();
				else
					break;
			}
		}
	}
	else
	{
		MessageBox( "Log �� ������ �� �����ϴ�" );
	}

	int i;
	std::sort( m_vecDeleteFile.begin(), m_vecDeleteFile.end() );
	std::sort( m_vecModifyFile.begin(), m_vecModifyFile.end() );
#ifdef _ROLLBACK_PATCH
	std::sort( m_vecRollDeleteFile.begin(),	m_vecRollDeleteFile.end() );
	std::sort( m_vecRestoreFile.begin(),	m_vecRestoreFile.end() );
#endif // _ROLLBACK_PATCH
	for( i = 0; i < ( int )m_vecDeleteFile.size(); i++ )
	{
		std::replace( m_vecDeleteFile[ i ].begin(), m_vecDeleteFile[ i ].end(), '/', '\\' );
	}
	for( i = 0; i < ( int )m_vecModifyFile.size(); i++ )
	{
		std::replace( m_vecModifyFile[ i ].begin(), m_vecModifyFile[ i ].end(), '/', '\\' );
	}

#ifdef _ROLLBACK_PATCH
	for( i = 0; i < ( int )m_vecRollDeleteFile.size(); i++ )
	{
		std::replace( m_vecRollDeleteFile[ i ].begin(), m_vecRollDeleteFile[ i ].end(), '/', '\\' );
	}

	for( i = 0; i < ( int )m_vecRestoreFile.size(); i++ )
	{
		std::replace( m_vecRestoreFile[ i ].begin(), m_vecRestoreFile[ i ].end(), '/', '\\' );
	}
#endif // _ROLLBACK_PATCH
}

void CPatchBuilderDlg::UpdateLastRevision()
{
	LogWnd::TraceLog( "UpdateLastRevision" );
	SetWorkStatus( "Update �������Դϴ�" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "svn";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;
	
#ifdef DH_SELF_TEST
	sprintf_s( szString, 1024, "update %s -r %d --username hicom15 --password 6326", m_szSVNUpdateFolder.GetBuffer(), m_nLastRevision );
#else // DH_SELF_TEST
	sprintf_s( szString, 1024, "update %s -r %d --username build --password b@0625", m_szSVNUpdateFolder.GetBuffer(), m_nLastRevision );
#endif // DH_SELF_TEST
	LogWnd::TraceLog( "Command:%s", szString );

	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}
}

BOOL CPatchBuilderDlg::CopyBuild( bool bDirectCopy )
{
	LogWnd::TraceLog( "CopyBuild" );
	SetWorkStatus( "���� ����Ÿ �з����Դϴ�" );

	char szString[ 1024 ], szCopyFolder[ _MAX_PATH ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "cmd.exe";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

	if( bDirectCopy )
	{
		sprintf_s( szCopyFolder, _MAX_PATH, "%s\\%08d\\Server", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	}
	else
	{
		GetCurrentDirectory( _MAX_PATH, szCopyFolder );
	}

	sprintf_s( szString, 1024, "/c %s %s %s\\LoginServer /SL %s", m_szRMakeCmd.GetBuffer(), m_szSVNUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}
	
	sprintf_s( szString, 1024, "%s\\LoginServer\\%s",szCopyFolder , RMAKE_RESULT_NAME );	
	if( !FileExistName(szString))
	{
		goto ErrorMsg;
	}

	sprintf_s( szString, 1024, "/c %s %s %s\\MasterServer /SM %s", m_szRMakeCmd.GetBuffer(), m_szSVNUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\MasterServer\\rmakeresult.txt",szCopyFolder );
	if( !FileExistName(szString))
	{
		goto ErrorMsg;
	}

	char szSourceFile[ _MAX_PATH ], szDestFile[ _MAX_PATH ];
	sprintf_s( szSourceFile, _MAX_PATH, "%s\\ResourceRevision.txt", szCopyFolder );
	sprintf_s( szDestFile, _MAX_PATH, "%s\\VillageServer\\ResourceRevision.txt", szCopyFolder );
	if( CopyFile( szSourceFile, szDestFile, FALSE ) == 0 )
	{
		char szPath[ _MAX_PATH ];
		_GetPath( szPath, szDestFile );
		CreateFolder( szPath );
		CopyFile( szSourceFile, szDestFile, FALSE );
	}

	sprintf_s( szString, 1024, "/c %s %s %s\\VillageServer /SV %s /zip", m_szRMakeCmd.GetBuffer(), m_szSVNUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\VillageServer\\%s",szCopyFolder , RMAKE_RESULT_NAME );
	if( !FileExistName(szString))
	{
		goto ErrorMsg;
	}

	sprintf_s( szSourceFile, _MAX_PATH, "%s\\ResourceRevision.txt", szCopyFolder );
	sprintf_s( szDestFile, _MAX_PATH, "%s\\GameServer\\ResourceRevision.txt", szCopyFolder );
	if( CopyFile( szSourceFile, szDestFile, FALSE ) == 0 )
	{
		char szPath[ _MAX_PATH ];
		_GetPath( szPath, szDestFile );
		CreateFolder( szPath );
		CopyFile( szSourceFile, szDestFile, FALSE );
	}

	sprintf_s( szString, 1024, "/c %s %s %s\\GameServer /SG %s /zip", m_szRMakeCmd.GetBuffer(), m_szSVNUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\GameServer\\%s",szCopyFolder , RMAKE_RESULT_NAME);
	if( !FileExistName(szString))
	{
		goto ErrorMsg;
	}

	return TRUE; 

ErrorMsg:
	sprintf_s( szString , 1024 , "%s\\%s", m_szModuleRoot , RMAKE_RESULT_NAME );
	FILE *fp;
	fopen_s( &fp, szString , "w" );
	fprintf(fp , " ");
	fclose(fp);

	return FALSE; 
}

void CPatchBuilderDlg::CreateDeleteList()
{
	LogWnd::TraceLog( "CreateDeleteList" );
	char szString[ 1024 ];
	FILE *fp;

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.txt", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
	fopen_s( &fp, szString, "wt" );
	for( int i = 0; i < ( int )m_vecDeleteFile.size(); i++ )
	{
		WCHAR wszBuffer[ _MAX_PATH ];
		char szTemp[ _MAX_PATH ];
		MultiByteToWideChar( CP_UTF8, 0, m_vecDeleteFile[ i ].c_str(), -1, wszBuffer, _MAX_PATH );
		WideCharToMultiByte( CP_ACP, 0, wszBuffer, -1, szTemp, _MAX_PATH, NULL, NULL );

		sprintf_s( szString, 1024, "D %s\n", szTemp );
		fputs( szString, fp );
	}

	int bFindLauncher = false;
	for( int i = 0; i < ( int )m_vecModifyFile.size(); i++ )
	{
		if( _stricmp( m_vecModifyFile[ i ].c_str(), "DNLauncher.exe" ) == 0 )
		{
			m_vecModifyFile.erase( m_vecModifyFile.begin() + i );
			bFindLauncher = true;
			break;
		}
	}
	if( bFindLauncher )
	{
		m_vecModifyFile.push_back( "DNLauncher.exe" );
	}

	for( int i = 0; i < ( int )m_vecModifyFile.size(); i++ )
	{
		if( IsPatchFile( m_vecModifyFile[ i ].c_str() ) )
		{
			sprintf_s( szString, 1024, "P %s\n", m_vecModifyFile[ i ].c_str() );
		}
		else
		{
			sprintf_s( szString, 1024, "C %s\n", m_vecModifyFile[ i ].c_str() );
			fputs( szString, fp );
		}
	}
	// �ƹ��͵� ������ 0����Ʈ¥�� ������ �����Ǽ�.. �ٿ�ε� �ȵɼ��� �־. ���� �ϳ� �־��ش�.
	fputs( " ", fp );
	fclose( fp );
}

void CPatchBuilderDlg::CreatePatch()
{
	LogWnd::TraceLog( "CreatePatch" );
	CEtPackingFile FileSystem;
	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
	FileSystem.NewFileSystem( szString );
	std::sort( m_vecModifyFile.begin(), m_vecModifyFile.end() );
	for( int i = 0; i < ( int )m_vecModifyFile.size(); i++ )
	{
		char szPath[ _MAX_PATH ], szFileName[ _MAX_PATH ];
		_GetPath( szPath, m_vecModifyFile[ i ].c_str() );
		sprintf_s( szFileName, _MAX_PATH, "%s\\%s", m_szSVNUpdateFolder.GetBuffer(), m_vecModifyFile[ i ].c_str() );

		FileSystem.ChangeDir( "\\" );
		if( szPath[ 0 ] )
		{
			FileSystem.ChangeDir( szPath );
		}
		FileSystem.AddFile( szFileName );

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "��ġ���� �������Դϴ� %d%%", ( int )( i * 100 / m_vecModifyFile.size() ) );
		SetWorkStatus( szMessage );
	}
	FileSystem.CloseFileSystem();
}

void CPatchBuilderDlg::CopyServerPatch( bool bDirectCopy )
{
	LogWnd::TraceLog( "CopyServerPatch" );
	SetWorkStatus( "���� ����Ÿ �������Դϴ�" );

	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Server", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );

	if( m_szSVNUrlServer.GetLength() )
	{
		SHELLEXECUTEINFO ShellExecInfo;
		memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
		ShellExecInfo.hwnd = GetSafeHwnd();
		ShellExecInfo.lpFile = "svn";
		ShellExecInfo.nShow = SW_SHOW;
		ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		ShellExecInfo.lpVerb = __TEXT( "open" );
		ShellExecInfo.lpParameters = szString;
#ifdef DH_SELF_TEST
		sprintf_s( szString, 1024, "export -r %d %s %s\\%08d\\Server --force --username hicom15 --password 6326", m_nLastRevision, m_szSVNUrlServer.GetBuffer(), m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
#else // DH_SELF_TEST
		sprintf_s( szString, 1024, "export -r %d %s %s\\%08d\\Server --force --username build --password b@0625", m_nLastRevision, m_szSVNUrlServer.GetBuffer(), m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
#endif // DH_SELF_TEST
	
		LogWnd::TraceLog( "Command:%s", szString );
		if( ShellExecuteEx( &ShellExecInfo ) )
		{
			::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
		}
	}
}

void CPatchBuilderDlg::LoadPatchInfo()
{
	LogWnd::TraceLog( "LoadPatchInfo" );
	FILE *fp;
	char szString[ 1024 ] =  {0,};
	DWORD dwsize = 0; 
	char arg1[1024]={0,} , arg2[1024]={0,};
	m_SkipNumber.clear();

	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szPatchFolder.GetBuffer() );
	
	if( !fopen_s( &fp, szString, "rt" ) )
	{
		fseek(fp , 0 , SEEK_END );
		dwsize = ftell(fp);
		fseek(fp , 0 , SEEK_SET);

		char *buffer = new char[dwsize+ 3 ];
		ZeroMemory(buffer , dwsize+3);
		fread(buffer, sizeof(char) , dwsize , fp );

		char *szToken , *nextToken ,delimiters[] = "\r\n";
		szToken = strtok_s(buffer , delimiters , &nextToken );
		
		while( szToken != NULL )
		{
			if(sscanf_s(szToken , "%s %s" ,&arg1 , sizeof(arg1) ,&arg2 , sizeof(arg2)))			 
			{
				_strlwr_s( arg1 );
				//���������� ���� ����
				if( strcmp(arg1 , "version") == 0 )
				{
					int a = 0 ; 
				}
				//skipnum �� �о�´�.
				else if( strcmp(arg1 , "skipnum") == 0 )
				{
					int version = atol(arg2);
					m_SkipNumber.push_back(version);
				}
			}
			szToken = strtok_s( NULL, delimiters  ,&nextToken );
		}
		
		delete [] buffer;
		fclose(fp);
	}
}

void CPatchBuilderDlg::UpdatePatchInfo()
{
	LogWnd::TraceLog( "UpdatePatchInfo" );
#ifdef _ROLLBACK_PATCH
	//��ŵ�� ��ȣ�� ��ġ�� �̻��� ���� ��ġ������ �����Ѵ�.
	std::vector<std::string > m_PatchInfoData;

	FILE *fp;
	char szString[ 1024 ] =  {0,};
	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szPatchFolder.GetBuffer() );
	std::sort(m_SkipNumber.begin() , m_SkipNumber.end() );

	if( !fopen_s( &fp, szString, "wt" ) )
	{
		//���� ���� ù���ο� �ְ�
		sprintf_s( szString, _MAX_PATH, "version %d\n", m_nCurrentVersion );
		fputs( szString, fp );

		for(int i = 0; i < ( int )m_SkipNumber.size(); i++ )
		{
			sprintf_s( szString, 1024, "SkipNum %d\n", m_SkipNumber[i] );
			fputs( szString, fp );
		}
		fclose( fp );
	}
	else 
	{
		if( !m_bAutoStart )
		{
			AfxMessageBox(_T("PatchInfoServer.cfg ���� ���� ����!!"));
		}
	}
#else // _ROLLBACK_PATCH
	FILE *fp;
	char szString[ 1024 ] =  {0 ,};

	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szPatchFolder.GetBuffer() );
	fopen_s( &fp, szString, "wt" );
	sprintf_s( szString, _MAX_PATH, "%d", m_nCurrentVersion );
	fwrite(szString , strlen(szString)+1 , 1 , fp );
//	fputs( szString, fp );
	fclose( fp );
#endif // _ROLLBACK_PATCH
}

void CPatchBuilderDlg::CleanupDir()
{
	LogWnd::TraceLog( "CleanupDir" );
	char szString[ _MAX_PATH ], szCurDirectory[ _MAX_PATH ];

	GetCurrentDirectory( _MAX_PATH, szCurDirectory );
	sprintf_s( szString, _MAX_PATH, "%s\\LoginServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\MasterServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\VillageServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\GameServer", szCurDirectory );
	DeleteFolder( szString );
}

//Ǯ������ ���鶧 ����Ѵ� .. 
//�ش� UpdateFolder �� �ִ� ������ ������ mapdata , resource , etc ������ Ǯ�� �Ѵ�. 
void CPatchBuilderDlg::BuildFullVersion()
{
	LogWnd::TraceLog( "BuildFullVersion" );
	std::vector< std::string > vecResult;
	FindFileListInDirectory( m_szSVNUpdateFolder.GetBuffer(), "*.*", vecResult, false, true, true );

	int i, nMainFolderLength, nCurrentFileSystem = 0;
	CEtPackingFile *pFileSystem = NULL;
	char szString[ _MAX_PATH ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull\\Resource%02d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, nCurrentFileSystem );
	pFileSystem = new CEtPackingFile();
	pFileSystem->NewFileSystem( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	nMainFolderLength = m_szSVNUpdateFolder.GetLength();
	for( i = 0; i < ( int )vecResult.size(); i++ )
	{
		char szPath[ _MAX_PATH ];
		_GetPath( szPath, vecResult[ i ].c_str() + nMainFolderLength + 1 );

		pFileSystem->ChangeDir( "\\" );
		if( IsPatchFile( szPath ) )
		{
			pFileSystem->ChangeDir( szPath );
			pFileSystem->AddFile( vecResult[ i ].c_str() );
			// ���� �ϳ��� 512�ް� �̻� �Ǹ� ���� ���Ϸ� �����Ѵ�.
			if( pFileSystem->GetFileSystemSize() > ( DWORD )DEFAULT_PACKING_FILE_SIZE )
			{
				char szNewFileSystem[ _MAX_PATH ];
				pFileSystem->CloseFileSystem();
				SAFE_DELETE( pFileSystem );
				pFileSystem = new CEtPackingFile();
				nCurrentFileSystem++;
				sprintf_s( szNewFileSystem, _MAX_PATH, "%s\\%08d\\ClientFull\\Resource%02d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, nCurrentFileSystem );
				pFileSystem->NewFileSystem( szNewFileSystem );
			}
		}
		else
		{
			char szFileName[ _MAX_PATH ], szFullName[ _MAX_PATH ];
			if( strlen( szPath ) <= 1 )
			{
				_GetFullFileName( szFileName, vecResult[ i ].c_str() );
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, szFileName );
			}
			else 
			{
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, szPath );
				CreateFolder( szFullName );
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, vecResult[ i ].c_str() + nMainFolderLength + 1 );
			}
			CopyFile( vecResult[ i ].c_str(), szFullName, FALSE );
		}

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "Full Version �������Դϴ� %d%%", ( int )( i * 100 / vecResult.size() ) );
		SetWorkStatus( szMessage );
	}
	pFileSystem->CloseFileSystem();
	SAFE_DELETE( pFileSystem );

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull\\Version.cfg", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	FILE *fp;
	fopen_s( &fp, szString, "wt" );
	sprintf_s( szString, _MAX_PATH, "version %d\n", m_nCurrentVersion );
	fputs( szString, fp );
	fclose( fp );
}

void CPatchBuilderDlg::BuildVersion1()
{
	LogWnd::TraceLog( "BuildVersion1" );
	std::string szPatchFolder;
	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );

	bool bDirectCopy = true;
	UpdateLastRevision();
	CopyServerPatch( bDirectCopy );
	CopyBuild( bDirectCopy );
	UpdatePatchInfo();
	BuildFullVersion();
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, 0, m_nLastRevision );
	CreateDirectory( szString, NULL );
}

void CPatchBuilderDlg::SetWorkStatus( const char *pString )
{
	SetDlgItemText( IDC_WORK_STATUS , pString );
}

void CPatchBuilderDlg::BuildPatch()
{
	LogWnd::TraceLog( "BuildPatch Start!" );
	CTime PatchStartTime, PatchEndTime;
	PatchStartTime = CTime::GetCurrentTime();

	std::string szPatchFolder;
	char szString[ 1024 ];
	//��ġ ������ 1�϶� �� �ѹ���ġ�� ������ �ʴ´�.. 
	if( m_nCurrentVersion == 1 )
	{
		BuildVersion1();
	}
	else
	{
		SVNCleanUp();
		// ��������Ÿ ī�Ǹ� ���� �� �ش޶�� �ؼ� true�� �ٲ��..
		bool bDirectCopy = true;
		GetModifyList();
		if( m_vecModifyFile.empty() && m_vecDeleteFile.empty() )
		{
			MessageBox( "��ġ�� ������ �����ϴ�" );
			return;
		}

		SaveListLog( "ModifyList.txt" );

		sprintf_s( szString, _MAX_PATH, "%s\\%08d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
		CreateDirectory( szString, NULL );

		if( m_bUseRTPatch )
		{
			MakeTempFiles();
		}
#ifdef _ROLLBACK_PATCH
		else
		{
			if( m_bRollBack )
				RollBackPatch();
		}
#endif // _ROLLBACK_PATCH
	
		//������ ���������� �ش� ������ ������Ʈ �Ѵ�. SVN UpdateFolder
		UpdateLastRevision();

#ifdef _ROLLBACK_PATCH
		if( m_bUseRTPatch )
		{
			if( m_bRollBack )
				RollBackRTPatch();
		}
#endif // _ROLLBACK_PATCH

		if( m_bCopyServerData )
		{
			CopyServerPatch( bDirectCopy );
			if(!CopyBuild( bDirectCopy ) ) 
			{
				if(!m_bAutoStart )
				{
					sprintf_s( szString, 1024, "RmakeCmd Fail ", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
					MessageBox( szString );
				}
				PostMessage(WM_QUIT);
				return;
			}
		}

		CreateDeleteList();

		if( m_bUseRTPatch )
		{
			CString strPakFileName;
			strPakFileName.Format( "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
			CString strOldFilePath;
			strOldFilePath.Format( "%s\\%08d\\Backup", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );

			CreateRTPatch( m_vecModifyFile, strPakFileName, m_szSVNUpdateFolder, strOldFilePath, "��ġ���� �������Դϴ� %d%%" );
		}
		else
		{
			CreatePatch();
		}

		UpdatePatchInfo();
	
		if( m_bMakeFullVersion )
		{
			BuildFullVersion();
		}
		sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nStartRevision, m_nLastRevision );
		CreateDirectory( szString, NULL );

#ifdef MANUAL_PATCH
		if( m_bManualPatch )
		{
			ManualPatch();
		}
#endif // MANUAL_PATCH
		// MD5 ���� ����
		CString strFilePath;
		strFilePath.Format( "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );

		CString strChecksum = CMD5Checksum::GetMD5( strFilePath );

		CString strMD5FileName = strFilePath + ".MD5";
		FILE* stream = fopen( strMD5FileName.GetString(), "w+" );

		if( stream )
		{
			fseek( stream, 0L, SEEK_SET );
			fprintf_s( stream, strChecksum.GetString() );
			fprintf_s( stream, "\n" );
			fclose( stream );
		}
	}

	CleanupDir();
	if( !m_bAutoStart )
	{
		sprintf_s( szString, 1024, "%s\\%08d ������ ��ġ ���� ����!!", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
		MessageBox( szString );
	}

	LogWnd::TraceLog( "BuildPatch End!" );

	PatchEndTime = CTime::GetCurrentTime();
	CTimeSpan PatchDurationTime = PatchEndTime - PatchStartTime;

	TCHAR szTotal[2048] = _T("");
	ZeroMemory( szTotal, sizeof(TCHAR) * 2048 );
	if( PatchDurationTime.GetHours() > 0 )
		_sntprintf_s( szTotal, 2048, _T( "%02d�ð� %02d �� %02d ��" ), PatchDurationTime.GetHours(), PatchDurationTime.GetMinutes(), PatchDurationTime.GetSeconds() );
	else
	{
		if( PatchDurationTime.GetMinutes() > 0 )
			_sntprintf_s( szTotal, 2048, _T( "%02d �� %02d ��" ), PatchDurationTime.GetMinutes(), PatchDurationTime.GetSeconds() );
		else
			_sntprintf_s( szTotal, 2048, _T( "%02d ��" ), PatchDurationTime.GetSeconds() );
	}

	if( m_bUseRTPatch )
		LogWnd::TraceLog( "UseRTPatch (O) - Patch Duration : %s", szTotal );
	else
		LogWnd::TraceLog( "UseRTPatch (X) - Patch Duration : %s", szTotal );

	PostMessage( WM_QUIT );
}

void CPatchBuilderDlg::SVNCleanUp()
{
	LogWnd::TraceLog( "SVNCleanUp" );
	SetWorkStatus( "SVN���� �������Դϴ�" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "svn";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

#ifdef DH_SELF_TEST
	sprintf_s( szString, 1024, "cleanup %s --username hicom15 --password 6326", m_szSVNUpdateFolder.GetBuffer() );
#else // DH_SELF_TEST
	sprintf_s( szString, 1024, "cleanup %s --username build --password b@0625", m_szSVNUpdateFolder.GetBuffer() );
#endif // DH_SELF_TEST
	LogWnd::TraceLog( "Command:%s", szString );

	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}
}

UINT AFX_CDECL _BuildPatchThread( LPVOID Param )
{
	CPatchBuilderDlg *pMainDlg = ( CPatchBuilderDlg * )AfxGetApp()->m_pMainWnd;
	if ( pMainDlg )
	{
		pMainDlg->BuildPatch();
	}

	return 0;
}

void CPatchBuilderDlg::ManualPatch()
{
	LogWnd::TraceLog( "ManualPatch" );
#ifdef MANUAL_PATCH
	USES_CONVERSION;

	//������ 1�ΰ��� ������ �ȵ��. 
	if(m_nCurrentVersion == 1)			
		return;

	int nManualPatchTargetStartVer = 0;
	int nManualPatchTargetEndVer = 0;
	if( m_nManualPatchTargetStartVer > 0 && m_nManualPatchTargetEndVer > 0 && m_nManualPatchTargetEndVer > m_nManualPatchTargetStartVer )
	{
		nManualPatchTargetStartVer = m_nManualPatchTargetStartVer;
		nManualPatchTargetEndVer = m_nManualPatchTargetEndVer;
	}
	else
	{
		nManualPatchTargetStartVer = m_nCurrentVersion - 1;
		nManualPatchTargetEndVer = m_nCurrentVersion;
	}

	LogWnd::TraceLog( "ManualPatch StartVer:%d, EndVer:%d", nManualPatchTargetStartVer, nManualPatchTargetEndVer );

	m_pDnAttachFile = new CDnAttachFile;

	//���� ������ �ؽ�Ʈ���� �鰡�� ���� ������ ������ ������ �����. 
	//����ġ �ϴºκ��� �ѹ�������� �ۿ´�.
	CStringA szTxt , szPak; 
	CStringA szPatchFolder;

	szPatchFolder.Format( "%s\\%08d\\" ,m_szPatchFolder.GetBuffer(), m_nCurrentVersion );

	CStringA szManualPatchFolder, szPatchExecutePath;
	szManualPatchFolder.Format( "%s%s", szPatchFolder.GetBuffer(), "ManualPatch" );

	//���� ����� 
	CreateFolder( szManualPatchFolder.GetBuffer() );
	szPatchExecutePath.Format( "%s\\DNUpdater_%dto%d.exe", szManualPatchFolder.GetBuffer(), nManualPatchTargetStartVer, nManualPatchTargetEndVer );

	//����ġ�� ��� ���ϰ� �̾Ƴ� ����
	if( !m_bAutoStart )
	{
		MessageBox( m_szManualPatchExe.GetBuffer() );
		MessageBox( szPatchExecutePath.GetBuffer() );
	}

	LogWnd::TraceLog( "--- %s, %s", m_szManualPatchExe.GetBuffer(), szPatchExecutePath.GetBuffer() );

	if( !m_pDnAttachFile->Create( m_szManualPatchExe.GetBuffer(), szPatchExecutePath.GetBuffer() ) )
	{
		if( !m_bAutoStart )
		{
			LogWnd::TraceLog( "���� ������ġ �б� ���� %s, %s", m_szManualPatchExe.GetBuffer(), szPatchExecutePath.GetBuffer() );
			MessageBox( "���� ������ġ �б� ����" );
			SAFE_DELETE( m_pDnAttachFile );
			return; 
		}
	}

	//���������� ó�� �����Ѵ�. 
	m_pDnAttachFile->WriteVersion( nManualPatchTargetStartVer, nManualPatchTargetEndVer );

	szTxt.Format( "Patch%08d.txt", m_nCurrentVersion );
	szPak.Format( "Patch%08d.pak", m_nCurrentVersion );

	char szMessage[ 1024 ];
	sprintf_s( szMessage, 1024, "���� ��ġ�� ���� ���Դϴ�." );
	SetWorkStatus( szMessage );

	if( !m_pDnAttachFile->AttachFile( szPatchFolder.GetBuffer(), szTxt.GetBuffer() ) )
	{
		if( !m_bAutoStart )
		{
			LogWnd::TraceLog( "��ġ���� ���� ���� %s, %s", szPatchFolder.GetBuffer(), szTxt.GetBuffer() );
			MessageBox( "��ġ���� ���� ����" );
			SAFE_DELETE( m_pDnAttachFile );
			return; 
		}
	}

	m_pDnAttachFile->AttachFile( szPatchFolder.GetBuffer(), szPak.GetBuffer() );
	m_pDnAttachFile->Close();

	sprintf_s( szMessage, 1024, "���� ��ġ ���� �Ϸ�!!" );
	SetWorkStatus( szMessage );

	SAFE_DELETE( m_pDnAttachFile );
#endif // MANUAL_PATCH
}

void CPatchBuilderDlg::OnBnClickedBuild()
{
	UpdateData( TRUE );

	if( m_nStartRevision >= m_nLastRevision )
	{
		MessageBox( "Revision ��ȣ�� ����� �Է����ּ���.." );
		return;
	}
	if( m_nCurrentVersion <= 0 )
	{
		MessageBox( "�߸��� ���� ���� �Դϴ�." );
		return;
	}

	GetDlgItem( ID_BUILD )->EnableWindow( FALSE );

	CleanupDir();

	if( m_szSVNUrl[ m_szSVNUrl.GetLength() - 1 ] == '\\' )
	{
		m_szSVNUrl.Delete( m_szSVNUrl.GetLength() - 1 );
	}
	if( m_szSVNUpdateFolder[ m_szSVNUpdateFolder.GetLength() - 1 ] == '\\' )
	{
		m_szSVNUpdateFolder.Delete( m_szSVNUpdateFolder.GetLength() - 1 );
	}

	AfxBeginThread( _BuildPatchThread, GetSafeHwnd() );
}

void CPatchBuilderDlg::OnBnClickedExit()
{
	OnCancel();
}

//SVN ������ �������� ������
BOOL CPatchBuilderDlg::CheckSVNInfo( int nProfileIndex )
{
	char szString[ 1024 ];
	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nProfileIndex );

#ifdef DH_SELF_TEST
	sprintf_s( szString, 1024, "svn info %s -r HEAD --xml --username hicom15 --password 6326", pProfile->szSVNUrl.c_str() );
#else // DH_SELF_TEST
	sprintf_s( szString, 1024, "svn info %s -r HEAD --xml --username build --password b@0625", pProfile->szSVNUrl.c_str() );
#endif // DH_SELF_TEST

	std::string szOutput;
	if( ExecCommand( szString, &szOutput ) )
	{
		TiXmlDocument Document;
		Document.Parse( szOutput.c_str(), 0, TIXML_ENCODING_UTF8 );
		if ( !Document.Error() )
		{
			Document.FirstChildElement( "info" )->FirstChildElement( "entry" )->FirstChildElement( "commit" )->QueryIntAttribute( "revision", &m_nLastRevision );
			std::string szURL = Document.FirstChildElement( "info" )->FirstChildElement( "entry" )->FirstChildElement( "url" )->GetText();
			std::string szRoot = Document.FirstChildElement( "info" )->FirstChildElement( "entry" )->FirstChildElement( "repository" )->FirstChildElement( "root" )->GetText();
			m_szMainFolder = szURL.c_str() + strlen( szRoot.c_str() );
		}
		else
		{
			MessageBox( szOutput.c_str() );
			return FALSE;
		}
	}
	else
	{
		MessageBox( "svn info ��� ���� ����! svn Ŀ�ǵ���� �ν��� �Ǿ� �ִ��� Ȯ�� ���ּ���" );
		return FALSE;
	}

	return TRUE;
}

void CPatchBuilderDlg::CheckPatchFolder()
{
	std::vector< std::string > szVecResult;
	std::string szLastPatchFolder;

	szLastPatchFolder = m_szPatchFolder.GetBuffer();

	CreateFolder( szLastPatchFolder.c_str() );
	_FindFolder( szLastPatchFolder.c_str(), szVecResult, false, NULL );
	m_nCurrentVersion = 1;
	if( szVecResult.empty() )
	{
		return;
	}

	bool bDeleteFolder = false;
	int i;
	std::sort( szVecResult.begin(), szVecResult.end() );
	for( i = ( int )szVecResult.size() - 1; i >= 0; i-- )
	{
		szLastPatchFolder = m_szPatchFolder.GetBuffer();
		szLastPatchFolder += "\\";
		szLastPatchFolder += szVecResult[ i ];

		std::vector< std::string > szVecRevision;
		_FindFolder( szLastPatchFolder.c_str(), szVecRevision, false, NULL );
		std::sort( szVecRevision.begin(), szVecRevision.end() );
		if( !szVecRevision.empty() )
		{
			const char *pFindPtr = strchr( szVecRevision[ 0 ].c_str(), '-' );
			if( pFindPtr )
			{
				if( bDeleteFolder )
				{
					m_nCurrentVersion = atoi( szVecResult[ i ].c_str() );
					UpdatePatchInfo();
				}
				m_nStartRevision = atoi( pFindPtr + 1 );
				m_nCurrentVersion = atoi( szVecResult[ i ].c_str() ) +1;
				return;
			}
		}

		char szString[ 1024 ];
		sprintf_s( szString, 1024, "%s ������ �߸��� ���� �Դϴ�. ������?", szLastPatchFolder.c_str() );
		if(!m_bAutoStart)
		{
			if( MessageBox( szString, NULL, MB_YESNO ) == IDYES )
			{
				DeleteFolder( szLastPatchFolder.c_str() );
				bDeleteFolder = true;
			}
		}
		else 
		{
			DeleteFolder( szLastPatchFolder.c_str() );
			bDeleteFolder = true;
		}
		
	}
}

void CPatchBuilderDlg::RollPacking()
{
	CEtPackingFile FileSystem;
	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH , "%s\\%08d" , m_szRollPatchFolder.GetBuffer() , m_nCurrentVersion+1 );
	CreateDirectory(szString , NULL );

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.pak", 	m_szRollPatchFolder.GetBuffer(),m_nCurrentVersion+1 ,m_nCurrentVersion+1 );

	FileSystem.NewFileSystem( szString );

	std::sort( m_vecRestoreFile.begin(), m_vecRestoreFile.end() );

	for( int i = 0; i < ( int )m_vecRestoreFile.size(); i++ )
	{
		char szPath[ _MAX_PATH ], szFileName[ _MAX_PATH ];
		_GetPath( szPath, m_vecRestoreFile[ i ].c_str() );
		sprintf_s( szFileName, _MAX_PATH, "%s\\%s", m_szRollTempFolder.GetBuffer(), m_vecRestoreFile[ i ].c_str() );

		FileSystem.ChangeDir( "\\" );

		if( szPath[ 0 ] )
		{
			FileSystem.ChangeDir( szPath );
		}
		FileSystem.AddFile( szFileName );

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "�ѹ� ������ �������Դϴ� %d%%", ( int )( i * 100 / m_vecRestoreFile.size() ) );
		SetWorkStatus( szMessage );
	}
	FileSystem.CloseFileSystem();
}

void CPatchBuilderDlg::RollSaveVersion()
{
	FILE *fp;
	char szString[ 1024 ] =  {0,};
	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szRollPatchFolder.GetBuffer() );

	//��������� ��ŵ�Ѵ�.
	m_SkipNumber.push_back(m_nCurrentVersion);
	std::sort(m_SkipNumber.begin() , m_SkipNumber.end());

	if( !fopen_s( &fp, szString, "wt" ) )
	{
		//���� ���� ù���ο� �ְ�
		sprintf_s( szString, _MAX_PATH, "version %d\n", m_nCurrentVersion+1 );
		fputs( szString, fp );

		for(int i = 0; i < ( int )m_SkipNumber.size(); i++ )
		{
			sprintf_s( szString, 1024, "SkipNum %d\n", m_SkipNumber[i] );
			fputs( szString, fp );
		}
		fclose( fp );
	}
	
	//����� ��ŵ�ѹ� �� ��������� �������� ����
	for(int i = 0; i < (int)m_SkipNumber.size() ;i++)
	{
		if( m_SkipNumber[i] == m_nCurrentVersion )
		{
			m_SkipNumber.erase(m_SkipNumber.begin()+i);
			break;
		}
	}
}

void CPatchBuilderDlg::RollBackCopyFile()
{	
	char szMessage[1024]={0,};
	char szString[1024]={0,};
	std::string SrcFile , DstFile;
	std::string szRollPatchFolder , szRollSvnUpdateFolder ;
	std::string szRollPatchTempFolder;

	//�ش������� �ѹ� �������� 
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\RollBack", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );
	m_szRollPatchFolder = szString;

	//�ѹ� ������ ���� ���� ���� ���� ������ �޾ƿ´�.
	sprintf_s( szString , _MAX_PATH , "%s\\Temp", m_szRollPatchFolder.GetBuffer() );
	CreateDirectory(szString , NULL );
	m_szRollTempFolder = szString;  

	szRollPatchTempFolder = m_szRollTempFolder.GetBuffer() ; 
	szRollSvnUpdateFolder = m_szSVNUpdateFolder.GetBuffer();
	szRollPatchFolder = m_szRollPatchFolder.GetBuffer(); 

	SHELLEXECUTEINFO ShellExecInfo;
	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "svn";
	ShellExecInfo.nShow = SW_HIDE;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

	for(int i =0 ; i < (int)m_vecRestoreFile.size() ; ++i )
	{
		SrcFile = szRollSvnUpdateFolder  +	"\\" +m_vecRestoreFile[i];
		DstFile = szRollPatchTempFolder  +	"\\" +m_vecRestoreFile[i];

		std::string StrChange , SVNPath; 
		StrChange = m_vecRestoreFile[i];

		char szPath[1024] = {0 , };
		_GetPath( szPath, DstFile.c_str() );
		CreateFolder(szPath);

		//���� �����͸� ���� �����ʿ� ī���Ѵ�.
		if( CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE ) == 0 )
		{
			CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE );
		}
	}
}

void CPatchBuilderDlg::RollCreateDeleteList()
{
	int i;
	char szString[ 1024 ];
	FILE *fp;

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.txt", 	m_szRollPatchFolder.GetBuffer(),m_nCurrentVersion+1 ,m_nCurrentVersion+1 );

	fopen_s( &fp, szString, "wt" );
	for( i = 0; i < ( int )m_vecRollDeleteFile.size(); i++ )
	{
		WCHAR wszBuffer[ _MAX_PATH ];
		char szTemp[ _MAX_PATH ];
		MultiByteToWideChar( CP_UTF8, 0, m_vecRollDeleteFile[ i ].c_str(), -1, wszBuffer, _MAX_PATH );
		WideCharToMultiByte( CP_ACP, 0, wszBuffer, -1, szTemp, _MAX_PATH, NULL, NULL );

		sprintf_s( szString, 1024, "D %s\n", szTemp );
		fputs( szString, fp );
	}

	int bFindLauncher = false;
	for( i = 0; i < ( int )m_vecRestoreFile.size(); i++ )
	{
		if( _stricmp( m_vecRestoreFile[ i ].c_str(), "DNLauncher.exe" ) == 0 )
		{
			m_vecRestoreFile.erase( m_vecRestoreFile.begin() + i );
			bFindLauncher = true;
			break;
		}
	}
	if( bFindLauncher )
	{
		m_vecRestoreFile.push_back( "DNLauncher.exe" );
	}

	for( i = 0; i < ( int )m_vecRestoreFile.size(); i++ )
	{
		if( IsPatchFile( m_vecRestoreFile[ i ].c_str() ) )
		{
			sprintf_s( szString, 1024, "P %s\n", m_vecRestoreFile[ i ].c_str() );
		}
		else
		{
			sprintf_s( szString, 1024, "C %s\n", m_vecRestoreFile[ i ].c_str() );
			fputs( szString, fp );
		}
	}
	// �ƹ��͵� ������ 0����Ʈ¥�� ������ �����Ǽ�.. �ٿ�ε� �ȵɼ��� �־. ���� �ϳ� �־��ش�.
	fputs( " ", fp );
	fclose( fp );
}

void CPatchBuilderDlg::RollBackPatch()
{
	LogWnd::TraceLog( "RollBackPatch" );

	RollBackCopyFile();	
	RollPacking();
	RollCreateDeleteList();

	DeleteFolder(m_szRollTempFolder.GetBuffer());

	char szString[1024]={0,};
	//������ ���
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szRollPatchFolder.GetBuffer(), m_nCurrentVersion+1 ,m_nStartRevision, m_nStartRevision );
	CreateDirectory( szString, NULL );
	
	//�ѹ������� ������ ��ŵ�Ǵ� ������ �����Ѵ�.. 
	RollSaveVersion();
	return ;
}

void CPatchBuilderDlg::RollBackRTPatch()
{
	char szString[1024];
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\RollBack", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );
	m_szRollPatchFolder = szString;
	sprintf_s( szString, _MAX_PATH , "%s\\%08d", m_szRollPatchFolder.GetBuffer(), m_nCurrentVersion+1 );
	CreateDirectory( szString, NULL );
	CString strPakFileName;
	strPakFileName.Format( "%s\\%08d\\Patch%08d.pak", m_szRollPatchFolder.GetBuffer(), m_nCurrentVersion + 1, m_nCurrentVersion + 1 );
	CString strNewFilePath;
	strNewFilePath.Format( "%s\\%08d\\Backup", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );

	CreateRTPatch( m_vecRestoreFile, strPakFileName, strNewFilePath, m_szSVNUpdateFolder, "�ѹ� ������ �������Դϴ� %d%%" );
	RollCreateDeleteList();

	//������ ���
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szRollPatchFolder.GetBuffer(), m_nCurrentVersion + 1, m_nStartRevision, m_nStartRevision );
	CreateDirectory( szString, NULL );

	//�ѹ������� ������ ��ŵ�Ǵ� ������ �����Ѵ�.. 
	RollSaveVersion();
	return ;
}

BOOL CPatchBuilderDlg::FileExistName(char *Path)
{
	if( ::GetFileAttributes( Path ) == 0xFFFFFFFF )
		return FALSE;

	return TRUE; 
}

void CPatchBuilderDlg::GetModuleRoot()
{
	GetModuleFileNameA(NULL, m_szModuleRoot, MAX_PATH);
	int i=0 ; 

	for( i = ( lstrlenA( m_szModuleRoot) - 1 ); i >= 0; --i )
	{
		if( m_szModuleRoot[i] == '\\' || m_szModuleRoot[i] == '/' )
		{
			m_szModuleRoot[i] = '\0';
			break;
		}
	}

	if( i < 0 )
	{
		i = 0;
		m_szModuleRoot[i] = '\0';
	}
}

void CPatchBuilderDlg::OnBnClickedIdcManualFolderBrowse2()
{
	CFileDialog FileDlg( TRUE, "*.exe", "*.*" );
	if( FileDlg.DoModal() == IDOK )
	{
		m_szManualPatchExe = FileDlg.m_ofn.lpstrFile;
		UpdateData( FALSE );
	}
}

void CPatchBuilderDlg::MakeTempFiles()
{
	LogWnd::TraceLog( "MakeTempFiles" );
	SetWorkStatus( "Backup Files ������Դϴ�" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "svn";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

#ifdef DH_SELF_TEST
	sprintf_s( szString, 1024, "update %s -r %d --username hicom15 --password 6326", m_szSVNUpdateFolder.GetBuffer(), m_nStartRevision );
#else // DH_SELF_TEST
	sprintf_s( szString, 1024, "update %s -r %d --username build --password b@0625", m_szSVNUpdateFolder.GetBuffer(), m_nStartRevision );
#endif // DH_SELF_TEST
	LogWnd::TraceLog( "Command:%s", szString );

	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	char szMessage[1024]={0,};
	std::string SrcFile , DstFile;
	std::string szP4UpdateFolder;
	std::string szPatchBackupFolder;

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Backup", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );

	szPatchBackupFolder = szString; 
	szP4UpdateFolder = m_szSVNUpdateFolder.GetBuffer();

	for( int i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		SrcFile = szP4UpdateFolder + "\\" + m_vecRestoreFile[i];
		DstFile = szPatchBackupFolder + "\\" + m_vecRestoreFile[i];

		std::string StrChange;
		StrChange = m_vecRestoreFile[i];

		char szPath[1024] = {0 , };
		_GetPath( szPath, DstFile.c_str() );
		CreateFolder( szPath );

		//���� �����͸� ���� �����ʿ� ī���Ѵ�.
		if( CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE ) == 0 )
		{
			CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE );
		}
	}
}

void CPatchBuilderDlg::CreateRTPatch( std::vector<std::string>& vecFileList, LPCTSTR strPakFileName, 
									   LPCTSTR strNewFilePath, LPCTSTR strOldFilePath, LPCTSTR strMessage )
{
	LogWnd::TraceLog( "CreateRTPatch" );
	char szMessage[1024];
	sprintf_s( szMessage, 1024, strMessage, 0 );
	SetWorkStatus( szMessage );

	DeleteFolder( "RTPatchFileTemp" );
	CreateFolder( "RTPatchFileTemp" );

	char szString[1024];
	CEtPackingFile FileSystem;
	FileSystem.NewFileSystem( strPakFileName );
	std::sort( vecFileList.begin(), vecFileList.end() );

	for( int i=0; i<(int)vecFileList.size(); i++ )
	{
		char szPath[_MAX_PATH], szNewFile[_MAX_PATH], szOldFile[_MAX_PATH];
		char szFullFileName[1024];

		_GetPath( szPath,,2 vecFileList[i].c_str() );
		_GetFullFileName( szFullFileName,256, vecFileList[i].c_str() );
		sprintf_s( szNewFile, _MAX_PATH, "%s\\%s", strNewFilePath, vecFileList[i].c_str() );
		sprintf_s( szOldFile, _MAX_PATH, "%s\\%s", strOldFilePath, vecFileList[i].c_str() );

		// 1. OldFile ���ٸ� ADD�� ������ ó�� ��
		BOOL bOldFileExist = FileExistName( szOldFile );
		if( !FileExistName( szNewFile ) )	// NewFile�� ���ٸ� Erroró��
		{
			LogWnd::TraceLog( "%s Patch File is not Exist!!!", szNewFile );
			ASSERT( "Patch File is not Exist!!!" );
			FileSystem.CloseFileSystem();
			DeleteFile( strPakFileName );
			break;
		}

		// 2. patchbld ���� rtp������ �����.
		SHELLEXECUTEINFO ShellExecInfo;

		memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
		ShellExecInfo.hwnd = GetSafeHwnd();
		ShellExecInfo.lpFile = "patchbld";
		ShellExecInfo.nShow = SW_HIDE;
		ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		ShellExecInfo.lpVerb = __TEXT( "open" );
		ShellExecInfo.lpParameters = szString;

		if( bOldFileExist )
			sprintf_s( szString, 1024, "/O:RTPatchFileTemp\\TEMP.RTP \"%s\" \"%s\"", szOldFile, szNewFile );
		else
			sprintf_s( szString, 1024, "/O:RTPatchFileTemp\\TEMP.RTP \"%s\"", szNewFile );

//		LogWnd::TraceLog( "Command:%s", szString );

		if( ShellExecuteEx( &ShellExecInfo ) )
		{
			::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
		}

		// 3. ������ RTP���ϸ��� ���� ���ϸ����� ���� (Ȯ���� .rtp�� ����)
		char szRename[_MAX_PATH];
		sprintf_s( szRename, _MAX_PATH, "RTPatchFileTemp\\%s.rtp", szFullFileName );
		rename( "RTPatchFileTemp\\TEMP.RTP", szRename );

		FileSystem.ChangeDir( "\\" );
		if( szPath[0] )
		{
			FileSystem.ChangeDir( szPath );
		}

		// 4. RTP������ ��ŷ���� �ý��ۿ� �߰�
		FileSystem.AddFile( szRename );
		DeleteFile( szRename );

		sprintf_s( szMessage, 1024, strMessage, ( int )( i * 100 / m_vecModifyFile.size() ) );
		SetWorkStatus( szMessage );
	}

	FileSystem.CloseFileSystem();

	// 5. ����
	DeleteFolder( "RTPatchFileTemp" );
}

void CPatchBuilderDlg::SaveListLog( LPCTSTR strFileName )
{
	char szString[ _MAX_PATH ], szCurDirectory[ _MAX_PATH ];
	GetCurrentDirectory( _MAX_PATH, szCurDirectory );

	FILE *fp;
	sprintf_s( szString, _MAX_PATH, "%s\\%s", szCurDirectory, strFileName );
	fopen_s( &fp, szString, "wt" );

	fputs( "\n< ModifyListFile >\n", fp );
	std::vector<std::string>::iterator iter = m_vecModifyFile.begin();
	for( ; iter != m_vecModifyFile.end(); iter++ )
	{
		sprintf_s( szString, _MAX_PATH, "%s\n", (*iter).c_str() );
		fputs( szString, fp );
	}

	fputs( "\n< DeleteFileList >\n", fp );
	iter = m_vecDeleteFile.begin();
	for( ; iter != m_vecDeleteFile.end(); iter++ )
	{
		sprintf_s( szString, _MAX_PATH, "%s\n", (*iter).c_str() );
		fputs( szString, fp );
	}

	fputs( "\n< RestoreFileList >\n", fp );
	iter = m_vecRestoreFile.begin();
	for( ; iter != m_vecRestoreFile.end(); iter++ )
	{
		sprintf_s( szString, _MAX_PATH, "%s\n", (*iter).c_str() );
		fputs( szString, fp );
	}

	fputs( "\n< RollDeletFileList >\n", fp );
	iter = m_vecRollDeleteFile.begin();
	for( ; iter != m_vecRollDeleteFile.end(); iter++ )
	{
		sprintf_s( szString, _MAX_PATH, "%s\n", (*iter).c_str() );
		fputs( szString, fp );
	}

	fclose( fp );
}

