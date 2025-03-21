// PatchBuilderP4Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "PatchBuilderP4.h"
#include "PatchBuilderP4Dlg.h"
#include "ExportBrowseFolder.h"
#include "NewProfile.h"
#include "RegProfile.h"
#include "SundriesFunc.h"
#include "tinyxml.h"
#include "EtFileSystem.h"
#include "MD5Checksum.h"
#include "DnAttachFile.h"
#include "shlwapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatchBuilderP4Dlg 대화 상자

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


CPatchBuilderP4Dlg::CPatchBuilderP4Dlg(CWnd* pParent /*=NULL*/)
: CDialog(CPatchBuilderP4Dlg::IDD, pParent)
, m_szP4Url( _T("") )
, m_szP4UrlFolder( _T("") )
, m_szP4UrlServer( _T("") )
, m_szP4UrlServerFolder( _T("") )
, m_szUpdateFolder( _T("") )
, m_szPatchFolder( _T("") )
, m_nLastRevision( 0 )
, m_nStartRevision( 0 )
, m_szRMakeCmd( _T("") )
, m_szCountryCode( _T("") )
, m_szP4Workspace( _T("") )
, m_bMakeFullVersion( FALSE )
, m_bCopyServerData( FALSE )
, m_bRollBack( FALSE )
, m_pDnAttachFile( NULL )
, m_szManualPatchExe( _T("") )
, m_bManualPatch( FALSE )
, m_bSetWorkspace( FALSE )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurrentVersion = 0;
	m_nDefaultProfile = 0;
	m_bAutoStart = FALSE;
}

void CPatchBuilderP4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_P4_URL, m_szP4Url);
	DDX_Text(pDX, IDC_P4_URL2, m_szP4UrlFolder);
	DDX_Text(pDX, IDC_P4_URL_SERVER, m_szP4UrlServer);
	DDX_Text(pDX, IDC_P4_URL_SERVER2, m_szP4UrlServerFolder);
	DDX_Text(pDX, IDC_UPDATE_FOLDER, m_szUpdateFolder);
	DDX_Text(pDX, IDC_PATCH_FOLDER, m_szPatchFolder);
	DDX_Control(pDX, IDC_SELECT_PROFILE, m_SelectProfile);
	DDX_Control(pDX, IDC_SELECT_WORKSPACE, m_SelectWorkspace);
	DDX_Text(pDX, IDC_CURRENT_REVISION, m_nLastRevision);
	DDX_Text(pDX, IDC_BASE_REVISION, m_nStartRevision);
	DDX_Text(pDX, IDC_RMAKE_FOLDER, m_szRMakeCmd);
	DDX_Text(pDX, IDC_COUNTRY_CODE, m_szCountryCode);
	DDX_Check(pDX, IDC_FULL_VERSION, m_bMakeFullVersion);
	DDX_Check(pDX, IDC_COPY_SERVER_DATA, m_bCopyServerData);
	DDX_Check(pDX, IDC_ROLLBACK, m_bRollBack);
	DDX_Text(pDX, IDC_MANUAL_FOLDER, m_szManualPatchExe);
	DDX_Check(pDX, IDC_MANUAL_PATCH, m_bManualPatch);
}

BEGIN_MESSAGE_MAP(CPatchBuilderP4Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PATCH_FOLDER_BROWSE, &CPatchBuilderP4Dlg::OnBnClickedPatchFolderBrowse)
	ON_BN_CLICKED(ID_ADD_PROFILE, &CPatchBuilderP4Dlg::OnBnClickedAddProfile)
	ON_BN_CLICKED(ID_MODIFY_PROFILE, &CPatchBuilderP4Dlg::OnBnClickedModifyProfile)
	ON_BN_CLICKED(ID_DELETE_PROFILE, &CPatchBuilderP4Dlg::OnBnClickedDeleteProfile)
	ON_CBN_SELCHANGE(IDC_SELECT_PROFILE, &CPatchBuilderP4Dlg::OnCbnSelchangeSelectProfile)
	ON_CBN_SELCHANGE(IDC_SELECT_WORKSPACE, &CPatchBuilderP4Dlg::OnCbnSelchangeSelectWorkspace)
	ON_BN_CLICKED(ID_BUILD, &CPatchBuilderP4Dlg::OnBnClickedBuild)
	ON_BN_CLICKED(IDC_RMAKE_FOLDER_BROWSE, &CPatchBuilderP4Dlg::OnBnClickedRmakeFolderBrowse)
	ON_BN_CLICKED(IDC_FULL_VERSION, &CPatchBuilderP4Dlg::OnBnClickedFullVersion)
	ON_BN_CLICKED(IDC_COPY_SERVER_DATA, &CPatchBuilderP4Dlg::OnBnClickedCopyServerData)
	ON_BN_CLICKED(ID_EXIT, &CPatchBuilderP4Dlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ROLLBACK, &CPatchBuilderP4Dlg::OnBnClickedRollback)
	ON_BN_CLICKED(IDC_MANUAL_PATCH, &CPatchBuilderP4Dlg::OnBnClickedManualPatch)
	ON_BN_CLICKED(IDC_IDC_MANUAL_FOLDER_BROWSE2, &CPatchBuilderP4Dlg::OnBnClickedIdcManualFolderBrowse2)
END_MESSAGE_MAP()


// CPatchBuilderP4Dlg 메시지 처리기

BOOL CPatchBuilderP4Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	GetModuleRoot();
	SetCurrentDirectoryA(m_szModuleRoot);
	DeleteFile(".\\rmakeresult.txt");

	RefreshProfileList();				//레지스트리에서 저장된 프로필 정보를 읽어옮
	SetCurProfile( m_nDefaultProfile );	//불러온 해당 값으로 셋팅 

	if( m_bAutoStart )
	{
		PostMessage( WM_COMMAND, MAKEWPARAM( ID_BUILD, 1 ) );
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPatchBuilderP4Dlg::OnPaint()
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
HCURSOR CPatchBuilderP4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPatchBuilderP4Dlg::OnBnClickedRmakeFolderBrowse()
{
	CFileDialog FileDlg( TRUE, "*.exe", "RMakeCmd.exe" );
	if( FileDlg.DoModal() == IDOK )
	{
		m_szRMakeCmd = FileDlg.m_ofn.lpstrFile;
		UpdateData( FALSE );
	}
}

void CPatchBuilderP4Dlg::OnBnClickedFullVersion()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeFullVersion", m_bMakeFullVersion );
}

void CPatchBuilderP4Dlg::OnBnClickedCopyServerData()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeServerData", m_bCopyServerData );
}

void CPatchBuilderP4Dlg::OnBnClickedRollback()
{
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "RollBack", m_bRollBack);
}

void CPatchBuilderP4Dlg::OnBnClickedManualPatch()
{
	// TODO: Add your control notification handler code here
	UpdateData( TRUE );
	SetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "ManualPatch", m_bManualPatch);
}

void CPatchBuilderP4Dlg::RefreshProfileList()
{
	m_SelectProfile.ResetContent();
	int i;
	for( i=0; i<CRegProfile::GetInstance().GetProfileCount(); i++ )
	{
		SProfile *pProfile = CRegProfile::GetInstance().GetProfile( i );
		m_SelectProfile.AddString( pProfile->szProfileName.c_str() );
	}
}

//레지에서 정보를 얻어온다
void CPatchBuilderP4Dlg::SetCurProfile( int nIndex )
{
	m_SelectProfile.SetCurSel( nIndex );

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nIndex );
	if( !pProfile )
	{
		return;
	}

	m_szP4Url = pProfile->szP4Url.c_str();
	m_szP4UrlFolder = pProfile->szP4UrlFolder.c_str();
	m_szP4UrlServer = pProfile->szP4UrlServer.c_str();
	m_szP4UrlServerFolder = pProfile->szP4UrlServerFolder.c_str();
	m_szP4Workspace = pProfile->szP4Workspace.c_str();
	m_szPatchFolder = pProfile->szPatchFolder.c_str();
	m_szRMakeCmd = pProfile->szRMakeCmd.c_str();
	m_szCountryCode = pProfile->szCountryCode.c_str();
	m_szManualPatchExe = pProfile->szManualPatchExe.c_str();

	CheckPatchFolder();
	CheckPerforceInfo( nIndex );
#ifdef _ROLLBACK_PATCH
	LoadPatchInfo();
#endif 
	UpdateData( FALSE );
}

//프로필중에 아무거나 선택하면 해당 저장된 정보에서 로드 한다. 
void CPatchBuilderP4Dlg::OnBnClickedAddProfile()
{
	CNewProfile Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		UpdateData( TRUE );

		SProfile TempProfile;
		TempProfile.szProfileName = Dlg.m_szProfileName.GetBuffer();
		TempProfile.szP4Url = m_szP4Url.GetBuffer();
		TempProfile.szP4UrlFolder = m_szP4UrlFolder.GetBuffer();
		TempProfile.szP4UrlServer = m_szP4UrlServer.GetBuffer();
		TempProfile.szP4UrlServerFolder = m_szP4UrlServerFolder.GetBuffer();
		TempProfile.szP4Workspace = m_szP4Workspace.GetBuffer();
		TempProfile.szPatchFolder = m_szPatchFolder.GetBuffer();
		TempProfile.szRMakeCmd = m_szRMakeCmd.GetBuffer();
		TempProfile.szCountryCode = m_szCountryCode.GetBuffer();
		TempProfile.szManualPatchExe = m_szManualPatchExe.GetBuffer();

		CRegProfile::GetInstance().AddProfile( TempProfile );

		RefreshProfileList();
		SetCurProfile( CRegProfile::GetInstance().GetProfileCount() - 1 );

		char szString[ 1024 ];
		sprintf_s( szString, 1024, "Profile %s 추가 되었습니다", Dlg.m_szProfileName.GetBuffer() );
		MessageBox( szString );
	}
}

//수정 한다 ㅋㅋ 
void CPatchBuilderP4Dlg::OnBnClickedModifyProfile()
{
	int nCurSel = m_SelectProfile.GetCurSel();
	if( nCurSel == CB_ERR )
	{
		MessageBox( "선택된 Profile이 없습니다." );
		return;
	}

	UpdateData( TRUE );

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nCurSel );
	pProfile->szP4Url = m_szP4Url.GetBuffer();
	pProfile->szP4UrlFolder = m_szP4UrlFolder.GetBuffer();
	pProfile->szP4UrlServer = m_szP4UrlServer.GetBuffer();
	pProfile->szP4UrlServerFolder = m_szP4UrlServerFolder.GetBuffer();
	pProfile->szP4Workspace = m_szP4Workspace.GetBuffer();
	pProfile->szPatchFolder = m_szPatchFolder.GetBuffer();
	pProfile->szRMakeCmd = m_szRMakeCmd.GetBuffer();
	pProfile->szCountryCode = m_szCountryCode.GetBuffer();
	pProfile->szManualPatchExe = m_szManualPatchExe.GetBuffer();

	CRegProfile::GetInstance().SaveProfile();

	char szString[ 1024 ];
	sprintf_s( szString, 1024, "Profile %s 변경 되었습니다", pProfile->szProfileName.c_str() );
	MessageBox( szString );
}

//프로필 삭제 
void CPatchBuilderP4Dlg::OnBnClickedDeleteProfile()
{
	int nCurSel = m_SelectProfile.GetCurSel();
	if( nCurSel == CB_ERR )
	{
		MessageBox( "선택된 Profile이 없습니다." );
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
	sprintf_s( szString, 1024, "Profile %s 삭제 되었습니다", szDelName.c_str() );
	MessageBox( szString );
}

void CPatchBuilderP4Dlg::OnCbnSelchangeSelectProfile()
{
	int nCurSel = m_SelectProfile.GetCurSel();
	if( nCurSel == CB_ERR )
	{
		return;
	}
	SetCurProfile( nCurSel );
}

void CPatchBuilderP4Dlg::OnCbnSelchangeSelectWorkspace()
{
	m_SelectWorkspace.GetLBText( m_SelectWorkspace.GetCurSel(), m_szP4Workspace );
	SetWorkspace( m_szP4Workspace );
}

void CPatchBuilderP4Dlg::GetModifyList()
{
	SetWorkStatus( "Log 분석중입니다" );

	char szString[ 1024 ];
	std::string szOutput;
	int nStartRevision, nEndRevision;
	nStartRevision = m_nStartRevision + 1;
	nEndRevision = m_nLastRevision;
	if( nStartRevision > nEndRevision )
	{
		nStartRevision = nEndRevision;
	}

	m_vecModifyFile.clear();
	m_vecDeleteFile.clear();

#ifdef _ROLLBACK_PATCH
	m_vecRollDeleteFile.clear();
	m_vecRestoreFile.clear();
#endif

	std::vector<int>::reverse_iterator iter = m_vecRevision.rbegin();

	CString strServerFolder = m_szP4UrlServerFolder.Left( m_szP4UrlServerFolder.GetLength() - 3 );	// "..." 삭제

	for( ; iter!=m_vecRevision.rend(); iter++ )
	{
		if( (*iter) <= m_nStartRevision )
			continue;

		sprintf_s( szString, 1024, "p4 describe %d", (*iter) );
		P4ExecCommand( szString, &szOutput );

		sprintf_s( szString, 1024, "%d - no such changelist.\r\n", (*iter) );
		if( StrStr( szOutput.c_str(), szString ) == NULL )
		{
			std::vector<std::string> tokens1;
			TokenizeA( szOutput, tokens1, "\r\n" );

			int nChangelistStart = 0;
			int nChangelistEnd = 0;

			for( int j=0; j<(int)tokens1.size(); j++ )
			{
				sprintf_s( szString, 1024, "Affected files ..." );
				if( StrStr( tokens1[j].c_str(), szString ) != NULL )
				{
					nChangelistStart = j+1;
				}
				sprintf_s( szString, 1024, "Differences ..." );
				if( StrStr( tokens1[j].c_str(), szString ) != NULL )
				{
					nChangelistEnd = j;
				}
			}

			if( nChangelistEnd == 0 )	// "Differences ..." 없을 경우 Affected files만 있는 경우임
				nChangelistEnd = (int)tokens1.size();

			if( nChangelistStart >= nChangelistEnd )
				break;

			std::vector<std::string> tokens2;
			for( int k=nChangelistStart; k<nChangelistEnd; k++ )
			{
				TokenizeA( tokens1[k], tokens2, " " );

				CString strTemp = tokens1[k].c_str();
				int nIndex = strTemp.ReverseFind( '/' );
				CString strFilePath = strTemp.Left( nIndex + 1 );
				strFilePath.Delete( 0, 4 );
				CString strFileName = strTemp.Right( strTemp.GetLength() - ( m_szP4UrlFolder.GetLength() + 1 ) );
				strFileName.Delete( strFileName.ReverseFind( '#' ), strFileName.GetLength() );

				if( strFilePath.Find( strServerFolder ) == -1 )	// 같은 리비전에 서버 변경사항도 있을 수 있기 때문에 제외
					SetChangelistInfo( tokens2[tokens2.size()-1].c_str(), strFileName.GetBuffer() );

				tokens2.clear();
			}

			tokens1.clear();
		}
	}

	int i;
	std::sort( m_vecDeleteFile.begin(), m_vecDeleteFile.end() );
	std::sort( m_vecModifyFile.begin(), m_vecModifyFile.end() );
#ifdef _ROLLBACK_PATCH
	std::sort( m_vecRollDeleteFile.begin(), m_vecRollDeleteFile.end() );
	std::sort( m_vecRestoreFile.begin(), m_vecRestoreFile.end() );
#endif

	for( i=0; i<(int)m_vecDeleteFile.size(); i++ )
		std::replace( m_vecDeleteFile[i].begin(), m_vecDeleteFile[i].end(), '/', '\\' );

	for( i=0; i<(int)m_vecModifyFile.size(); i++ )
		std::replace( m_vecModifyFile[i].begin(), m_vecModifyFile[i].end(), '/', '\\' );

#ifdef _ROLLBACK_PATCH
	for( i=0; i<(int)m_vecRollDeleteFile.size(); i++ )
		std::replace( m_vecRollDeleteFile[i].begin(), m_vecRollDeleteFile[i].end(), '/', '\\' );

	for( i=0; i<(int)m_vecRestoreFile.size(); i++ )
		std::replace( m_vecRestoreFile[i].begin(), m_vecRestoreFile[i].end(), '/', '\\' );
#endif 
}

void CPatchBuilderP4Dlg::SetChangelistInfo( const char* pSeparator, const char* pFileName )
{
	if( pSeparator == NULL || pFileName == NULL )
		return;

	if( strlen( pSeparator ) == 0 || strlen( pFileName ) == 0 )
		return;

	std::vector< std::string >::iterator idModi = std::find( m_vecModifyFile.begin(), m_vecModifyFile.end(), pFileName );
	std::vector< std::string >::iterator itDelete = std::find( m_vecDeleteFile.begin(), m_vecDeleteFile.end(), pFileName );

#ifdef _ROLLBACK_PATCH
	std::vector< std::string >::iterator itRollDelete = std::find( m_vecRollDeleteFile.begin() ,m_vecRollDeleteFile.end() , pFileName ) ; 
	std::vector< std::string >::iterator itRestore = std::find( m_vecRestoreFile.begin() , m_vecRestoreFile.end() , pFileName );

	// 'A' 추가가된것은 삭제를 해야되고 M , D 수정이나 삭제가 된것은 수정을 했기에 복구에 넣는다.
	if( StrStr( pSeparator, "add" ) != NULL || StrStr( pSeparator, "branch" ) != NULL )
	{
		if( itRollDelete == m_vecRollDeleteFile.end() ) 
			m_vecRollDeleteFile.push_back( pFileName );
	}
	else if( StrStr( pSeparator, "edit" ) != NULL || StrStr( pSeparator, "delete" ) != NULL || StrStr( pSeparator, "intergrate" ) != NULL )
	{
		//추가가된 파일은 무조건 삭제되야 하기때문에 복구파일에 들어가지 않는다. 
		if( itRollDelete == m_vecRollDeleteFile.end() && itRestore == m_vecRestoreFile.end() )
			m_vecRestoreFile.push_back(pFileName);
	}
	else
		ASSERT( 0 && "잘못된 액션코드" );
#endif

	if( StrStr( pSeparator, "add" ) != NULL || StrStr( pSeparator, "edit" ) != NULL || 
		StrStr( pSeparator, "branch" ) != NULL || StrStr( pSeparator, "intergrate" ) != NULL )
	{
		if( idModi == m_vecModifyFile.end() )
			m_vecModifyFile.push_back( pFileName );

		//삭제 리스트 에서 다시 수정이나 추가한것은 리스트에서 빼버린다. 
		if( itDelete != m_vecDeleteFile.end() )
			m_vecDeleteFile.erase( itDelete );
	}
	else if( StrStr( pSeparator, "delete" ) != NULL )
	{
		//저장된 모디 리스트 삭제 액션이 나오면 
		if( idModi != m_vecModifyFile.end() )
			m_vecModifyFile.erase( idModi );
		
		if( itDelete == m_vecDeleteFile.end() )
			m_vecDeleteFile.push_back( pFileName );
	}
	else
		ASSERT( 0 && "잘못된 액션코드" );
}

BOOL CPatchBuilderP4Dlg::SetWorkspace( LPCTSTR strWorkspace )
{
	if( strlen( strWorkspace ) == 0 )
		return FALSE;

	char szString[ 1024 ];
	std::string szOutput;

	// Set Workspace
	sprintf_s( szString, 1024, "p4 -s set P4CLIENT=%s", strWorkspace );
	P4ExecCommand( szString, &szOutput );

	sprintf_s( szString, 1024, "p4 -s info", strWorkspace );
	P4ExecCommand( szString, &szOutput );

	std::vector<std::string> tokens1;
	std::vector<std::string> tokens2;
	TokenizeA( szOutput, tokens1, "\r\n" );
	m_bSetWorkspace = FALSE;

	for( DWORD i=0; i<tokens1.size(); i++ )
	{
		TokenizeA( tokens1[i], tokens2, " " );
		for( DWORD j=0; j<tokens2.size(); j++ )
		{
			if( NULL != StrStr( tokens2[j].c_str(), "Client" ) && NULL != StrStr( tokens2[j+1].c_str(), "root:" ) )
			{
				m_szP4ClientRoot = tokens2[j+2].c_str();
				m_szUpdateFolder.Format( "%s\\Client", m_szP4ClientRoot );
				GetDlgItem( IDC_UPDATE_FOLDER )->SetWindowText( m_szUpdateFolder );
				m_bSetWorkspace = TRUE;
				break;
			}
		}

		tokens2.clear();
		if( m_bSetWorkspace )
			break;
	}

	tokens1.clear();

	return TRUE;
}

void CPatchBuilderP4Dlg::UpdateLastRevision()
{
	SetWorkStatus( "Update 실행중입니다" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "p4";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

	// Sync
//	sprintf_s( szString, 1024, "-s sync %s@%d", m_szP4UrlFolder.GetBuffer(), m_nLastRevision );
	sprintf_s( szString, 1024, "-s sync %s#head", m_szP4UrlFolder.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );

	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}
}

BOOL CPatchBuilderP4Dlg::CopyBuild( bool bDirectCopy )
{
	SetWorkStatus( "서버 데이타 분류중입니다" );

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

	sprintf_s( szString, 1024, "/c %s %s %s\\LoginServer /SL %s", m_szRMakeCmd.GetBuffer(), m_szUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\LoginServer\\%s", szCopyFolder, RMAKE_RESULT_NAME );	
	if( !FileExistName( szString) )
	{
		goto ErrorMsg;
	}

	sprintf_s( szString, 1024, "/c %s %s %s\\MasterServer /SM %s", m_szRMakeCmd.GetBuffer(), m_szUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\MasterServer\\rmakeresult.txt",szCopyFolder );
	if( !FileExistName( szString ) )
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

	sprintf_s( szString, 1024, "/c %s %s %s\\VillageServer /SV %s /zip", m_szRMakeCmd.GetBuffer(), m_szUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\VillageServer\\%s",szCopyFolder , RMAKE_RESULT_NAME );
	if( !FileExistName( szString ) )
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

	sprintf_s( szString, 1024, "/c %s %s %s\\GameServer /SG %s /zip", m_szRMakeCmd.GetBuffer(), m_szUpdateFolder.GetBuffer(), szCopyFolder, m_szCountryCode.GetBuffer() );
	LogWnd::TraceLog( "Command:%s", szString );
	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	sprintf_s( szString, 1024, "%s\\GameServer\\%s",szCopyFolder , RMAKE_RESULT_NAME);
	if( !FileExistName( szString ) )
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

void CPatchBuilderP4Dlg::CreateDeleteList()
{
	int i;
	char szString[ 1024 ];
	FILE *fp;

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.txt", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
	fopen_s( &fp, szString, "wt" );
	for( i = 0; i < ( int )m_vecDeleteFile.size(); i++ )
	{
		WCHAR wszBuffer[ _MAX_PATH ];
		char szTemp[ _MAX_PATH ];
		MultiByteToWideChar( CP_UTF8, 0, m_vecDeleteFile[i].c_str(), -1, wszBuffer, _MAX_PATH );
		WideCharToMultiByte( CP_ACP, 0, wszBuffer, -1, szTemp, _MAX_PATH, NULL, NULL );

		sprintf_s( szString, 1024, "D %s\n", szTemp );
		fputs( szString, fp );
	}

	int bFindLauncher = false;
	for( i=0; i<(int)m_vecModifyFile.size(); i++ )
	{
		if( _stricmp( m_vecModifyFile[i].c_str(), "DNLauncher.exe" ) == 0 )
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

	for( i=0; i<(int)m_vecModifyFile.size(); i++ )
	{
		if( !IsPatchFile( m_vecModifyFile[i].c_str() ) )
		{
			sprintf_s( szString, 1024, "C %s\n", m_vecModifyFile[i].c_str() );
			fputs( szString, fp );
		}
	}
	// 아무것도 없으면 0바이트짜리 파일이 생성되서.. 다운로드 안될수도 있어서. 더미 하나 넣어준다.
	fputs( " ", fp );
	fclose( fp );
}

void CPatchBuilderP4Dlg::CreatePatch()
{
	int i;
	CEtPackingFile FileSystem;
	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
	FileSystem.NewFileSystem( szString );
	std::sort( m_vecModifyFile.begin(), m_vecModifyFile.end() );
	for( i=0; i<(int)m_vecModifyFile.size(); i++ )
	{
		char szPath[ _MAX_PATH ], szFileName[ _MAX_PATH ];
		_GetPath( szPath, m_vecModifyFile[i].c_str() );
		sprintf_s( szFileName, _MAX_PATH, "%s\\%s", m_szUpdateFolder.GetBuffer(), m_vecModifyFile[i].c_str() );

		FileSystem.ChangeDir( "\\" );
		if( szPath[ 0 ] )
		{
			FileSystem.ChangeDir( szPath );
		}
		FileSystem.AddFile( szFileName );

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "패치파일 생성중입니다 %d%%", ( int )( i * 100 / m_vecModifyFile.size() ) );
		SetWorkStatus( szMessage );
	}
	FileSystem.CloseFileSystem();
}

void CPatchBuilderP4Dlg::CopyServerPatch( bool bDirectCopy )
{
	SetWorkStatus( "서버 데이타 복사중입니다" );

	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Server", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );

	if( m_szP4UrlServer.GetLength() )
	{
		SHELLEXECUTEINFO ShellExecInfo;
		memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
		ShellExecInfo.hwnd = GetSafeHwnd();
		ShellExecInfo.lpFile = "p4";
		ShellExecInfo.nShow = SW_SHOW;
		ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		ShellExecInfo.lpVerb = __TEXT( "open" );
		ShellExecInfo.lpParameters = szString;
		
//		sprintf_s( szString, 1024, "-s sync %s@%d", m_szP4UrlServerFolder.GetBuffer(), m_nLastRevision );
		sprintf_s( szString, 1024, "-s sync %s#head", m_szP4UrlServerFolder.GetBuffer() );
		LogWnd::TraceLog( "Command:%s", szString );

		if( ShellExecuteEx( &ShellExecInfo ) )
		{
			::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
		}

		memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
		ShellExecInfo.hwnd = GetSafeHwnd();
		ShellExecInfo.lpFile = "xcopy";
		ShellExecInfo.nShow = SW_SHOW;
		ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
		ShellExecInfo.lpVerb = __TEXT( "open" );
		ShellExecInfo.lpParameters = szString;

		sprintf_s( szString, 1024, "%s\\Server %s\\%08d\\Server /E /I /K /O /X /Y", m_szP4ClientRoot.GetBuffer(), m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
		LogWnd::TraceLog( "Command:%s", szString );

		if( ShellExecuteEx( &ShellExecInfo ) )
		{
			::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
		}
	}
}

void CPatchBuilderP4Dlg::LoadPatchInfo()
{
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
				//서버버전은 읽지 않음
				if( strcmp(arg1 , "version") == 0 )
				{
					int a = 0 ; 
				}
				//skipnum 만 읽어온다.
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

void CPatchBuilderP4Dlg::UpdatePatchInfo()
{
#ifdef _ROLLBACK_PATCH
	//스킵의 번호는 패치후 이상이 생긴 패치버젼을 저장한다.
	std::vector<std::string > m_PatchInfoData;

	FILE *fp;
	char szString[ 1024 ] =  {0,};
	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szPatchFolder.GetBuffer() );
	std::sort(m_SkipNumber.begin() , m_SkipNumber.end() );

	if( !fopen_s( &fp, szString, "wt" ) )
	{
		//서버 버전 첫라인에 넣고
		sprintf_s( szString, _MAX_PATH, "version %d\n", m_nCurrentVersion );
		fputs( szString, fp );

		for( int i=0; i<(int)m_SkipNumber.size(); i++ )
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
			AfxMessageBox(_T("PatchInfoServer.cfg 버전 저장 실패!!"));
		}
	}
#else 
	FILE *fp;
	char szString[ 1024 ] =  {0 ,};

	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szPatchFolder.GetBuffer() );
	fopen_s( &fp, szString, "wt" );
	sprintf_s( szString, _MAX_PATH, "%d", m_nCurrentVersion );
	fwrite(szString , strlen(szString)+1 , 1 , fp );
	//	fputs( szString, fp );
	fclose( fp );
#endif 
}

void CPatchBuilderP4Dlg::CleanupDir()
{
	char szString[ _MAX_PATH ], szCurDirectory[ _MAX_PATH ];

	GetCurrentDirectory( _MAX_PATH, szCurDirectory );
//	sprintf_s( szString, _MAX_PATH, "%s\\Client", szCurDirectory );
//	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\LoginServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\MasterServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\VillageServer", szCurDirectory );
	DeleteFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\GameServer", szCurDirectory );
	DeleteFolder( szString );
}

//풀버젼을 만들때 사용한다 .. 
//해당 UpdateFolder 에 있는 정보를 가지고 mapdata , resource , etc 파일을 풀팩 한다. 
void CPatchBuilderP4Dlg::BuildFullVersion()
{
	std::vector< std::string > vecResult;
	FindFileListInDirectory( m_szUpdateFolder.GetBuffer(), "*.*", vecResult, false, true, true );

	int i, nMainFolderLength, nCurrentFileSystem = 0;
	CEtPackingFile *pFileSystem = NULL;
	char szString[ _MAX_PATH ];

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateFolder( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull\\Resource%02d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, nCurrentFileSystem );
	pFileSystem = new CEtPackingFile();
	pFileSystem->NewFileSystem( szString );
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\ClientFull", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	nMainFolderLength = m_szUpdateFolder.GetLength();
	for( i=0; i<(int)vecResult.size(); i++ )
	{
		char szPath[ _MAX_PATH ];
		_GetPath( szPath, vecResult[i].c_str() + nMainFolderLength + 1 );

		pFileSystem->ChangeDir( "\\" );
		if( IsPatchFile( szPath ) )
		{
			pFileSystem->ChangeDir( szPath );
			pFileSystem->AddFile( vecResult[i].c_str() );
			// 파일 하나가 512메가 이상 되면 다음 파일로 압축한다.
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
				_GetFullFileName( szFileName, vecResult[i].c_str() );
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, szFileName );
			}
			else 
			{
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, szPath );
				CreateFolder( szFullName );
				sprintf_s( szFullName, _MAX_PATH, "%s\\%s", szString, vecResult[i].c_str() + nMainFolderLength + 1 );
			}
			CopyFile( vecResult[i].c_str(), szFullName, FALSE );
		}

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "Full Version 생성중입니다 %d%%", ( int )( i * 100 / vecResult.size() ) );
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

void CPatchBuilderP4Dlg::BuildVersion1()
{
	LogWnd::TraceLog( _T( "BuildVersion1 Start!") );

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

void CPatchBuilderP4Dlg::SetWorkStatus( const char *pString )
{
	SetDlgItemText( IDC_WORK_STATUS , pString );
}

void CPatchBuilderP4Dlg::BuildPatch()
{
	std::string szPatchFolder;
	char szString[ 1024 ];

	LogWnd::TraceLog( _T( "BuildPatch Start!") );

	//패치 버젼이 1일때 는 롤백패치는 만들지 않는다.. 
	if( m_nCurrentVersion == 1 )
	{
		BuildVersion1();
	}
	else
	{
		// 서버데이타 카피를 전부 다 해달라고 해서 true로 바꿨다..
		bool bDirectCopy = true;
		GetModifyList();
		if( m_vecModifyFile.empty() && m_vecDeleteFile.empty() )
		{
			MessageBox( "패치할 내용이 없습니다" );
			return;
		}

		sprintf_s( szString, _MAX_PATH, "%s\\%08d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
		CreateDirectory( szString, NULL );

#ifdef _USE_RTPATCH
		MakeTempFiles();
#else // _USE_RTPATCH
#ifdef _ROLLBACK_PATCH
		if( m_bRollBack )
			RollBackPatch();
#endif 
#endif // _USE_RTPATCH

		//마지막 리비젼까지 해당 폴더로 업데이트 한다. P4 UpdateFolder
		UpdateLastRevision();

#ifdef _USE_RTPATCH	// Update받고 나서 Rollback Patch 만든다.
#ifdef _ROLLBACK_PATCH
	if( m_bRollBack )
		RollBackPatch();
#endif // _ROLLBACK_PATCH
#endif // _USE_RTPATCH

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
#ifdef _USE_RTPATCH
		CString strPakFileName;
		strPakFileName.Format( "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );
		CString strOldFilePath;
		strOldFilePath.Format( "%s\\%08d\\Backup", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );

		CreateRTPatch( m_vecModifyFile, strPakFileName, m_szUpdateFolder, strOldFilePath, "패치파일 생성중입니다 %d%%" );
#else // _USE_RTPATCH
		CreatePatch();
#endif // _USE_RTPATCH
		UpdatePatchInfo();

		if( m_bMakeFullVersion )
			BuildFullVersion();

		sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nStartRevision, m_nLastRevision );
		CreateDirectory( szString, NULL );

#ifdef MANUAL_PATCH
		if(m_bManualPatch )
			ManualPatch();
#endif
		// MD5 파일 생성
		CString strFilePath;
		strFilePath.Format( "%s\\%08d\\Patch%08d.pak", m_szPatchFolder.GetBuffer(), m_nCurrentVersion, m_nCurrentVersion );

		CString strChecksum = CMD5Checksum::GetMD5( strFilePath );

		CString strMD5FileName = strFilePath + ".MD5";
		FILE* stream = NULL;
		fopen_s( &stream, strMD5FileName.GetString(), "w+" );

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
		sprintf_s( szString, 1024, "%s\\%08d 폴더에 패치 생성 성공!!", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
		MessageBox( szString );
	}
	PostMessage( WM_QUIT );
}

UINT AFX_CDECL _BuildPatchThread( LPVOID Param )
{
	CPatchBuilderP4Dlg *pMainDlg = ( CPatchBuilderP4Dlg * )AfxGetApp()->m_pMainWnd;

	if( pMainDlg )
		pMainDlg->BuildPatch();

	return 0;
}

void CPatchBuilderP4Dlg::ManualPatch()
{
#ifdef MANUAL_PATCH
	USES_CONVERSION;

	//버젼이 1인것은 만들지 안들다. 
	if(m_nCurrentVersion == 1)			
		return;

	m_pDnAttachFile = new CDnAttachFile;

	//팩의 순서는 텍스트먼저 들가고 다음 데이터 순으로 파일을 만든다. 
	//어태치 하는부분은 롤백버젼에서 퍼온다.
	CStringA szTxt , szPak; 
	CStringA szPatchFolder;

	szPatchFolder.Format( "%s\\%08d\\" ,m_szPatchFolder.GetBuffer() , m_nCurrentVersion );

	CStringA szManualPatchFolder , szPatchExecutePath ;
	szManualPatchFolder.Format( "%s%s" , szPatchFolder.GetBuffer() , "ManualPatch" );

	//폴더 만들고 
	CreateFolder(szManualPatchFolder.GetBuffer() );
	szPatchExecutePath.Format("%s\\DNUpdater_%dto%d.exe" , szManualPatchFolder.GetBuffer() , m_nCurrentVersion-1 , m_nCurrentVersion);

	//어태치할 모듈 파일과 뽑아낼 파일 
	if(!m_pDnAttachFile->Create( m_szManualPatchExe.GetBuffer() , szPatchExecutePath.GetBuffer() ))
	{
		if(!m_bAutoStart)
		{
			MessageBox("원본 수동패치 읽기 실패");
			SAFE_DELETE(m_pDnAttachFile);
			return; 
		}
	}

	//버젼정보를 처음 저장한다. 
	m_pDnAttachFile->WriteVersion( m_nCurrentVersion-1 ,m_nCurrentVersion );

	szTxt.Format("Patch%08d.txt" , m_nCurrentVersion  );
	szPak.Format("Patch%08d.pak" , m_nCurrentVersion  );

	char szMessage[ 1024 ];
	sprintf_s( szMessage, 1024, "수동 패치를 생성 중입니다." );
	SetWorkStatus( szMessage );

	if( !m_pDnAttachFile->AttachFile( szPatchFolder.GetBuffer()  ,  szTxt.GetBuffer()))
	{
		if(!m_bAutoStart)
		{
			MessageBox("패치파일 병합 실패");
			SAFE_DELETE(m_pDnAttachFile);
			return; 
		}
	}

	m_pDnAttachFile->AttachFile( szPatchFolder.GetBuffer()  ,  szPak.GetBuffer());
	m_pDnAttachFile->Close();

	sprintf_s( szMessage, 1024, "수동 패치 생성 완료!!" );
	SetWorkStatus( szMessage );

	SAFE_DELETE(m_pDnAttachFile);
#endif // MANUAL_PATCH
}

void CPatchBuilderP4Dlg::OnBnClickedBuild()
{
	UpdateData( TRUE );

	if( m_nStartRevision >= m_nLastRevision )
	{
		MessageBox( "Revision 번호를 제대로 입력해주세요.." );
		return;
	}

	if( m_nCurrentVersion <= 0 )
	{
		MessageBox( "잘못된 빌드 버전 입니다." );
		return;
	}

	if( !m_bSetWorkspace )
	{
		MessageBox( "Workspace 설정이 잘못되었습니다." );
		return;
	}

	GetDlgItem( ID_BUILD )->EnableWindow( FALSE );

	CleanupDir();

	AfxBeginThread( _BuildPatchThread, GetSafeHwnd() );
}

void CPatchBuilderP4Dlg::OnBnClickedExit()
{
	OnCancel();
}

void CPatchBuilderP4Dlg::OnOK()
{
	return;
}

bool CPatchBuilderP4Dlg::P4ExecCommand( char *pCommand, std::string *pszOutput )
{
	LogWnd::TraceLog( "Command:%s", pCommand );

	ExecCommand( pCommand, pszOutput );

//	LogWnd::TraceLog( "Ouput:%s", pszOutput->c_str() );

	std::vector<std::string> tokens;
	TokenizeA( pszOutput->c_str(), tokens, "\r\n" );

	if( tokens.size() == 0 || NULL != StrStr( tokens[tokens.size()-1].c_str(), "exit: 0" ) )
		return true;

	return false;
}

BOOL CPatchBuilderP4Dlg::CheckPerforceInfo( int nProfileIndex )
{
	char szString[ 1024 ];
	std::string szOutput;

	SProfile *pProfile = CRegProfile::GetInstance().GetProfile( nProfileIndex );

	// Set Password
	sprintf_s( szString, 1024, "p4 -s set P4PASSWD=1wkatlfdur!" );
	if( !P4ExecCommand( szString, &szOutput ) )
	{
		MessageBox( szOutput.c_str() );
		return FALSE;
	}

	// Login
	sprintf_s( szString, 1024, "p4 -s -p %s -u eyeadmin login -s", pProfile->szP4Url.c_str() );
	if( !P4ExecCommand( szString, &szOutput ) )
	{
		MessageBox( szOutput.c_str() );
		return FALSE;
	}

	// Workspace
	sprintf_s( szString, 1024, "p4 -s workspaces" );
	if( !P4ExecCommand( szString, &szOutput ) )
	{
		MessageBox( szOutput.c_str() );
		return FALSE;
	}

	std::vector<std::string> tokens1;
	std::vector<std::string> tokens2;
	TokenizeA( szOutput, tokens1, "\r\n" );

	for( DWORD i=0; i<tokens1.size(); i++ )
	{
		TokenizeA( tokens1[i], tokens2, " " );
		for( DWORD j=0; j<tokens2.size(); j++ )
		{
			if( NULL != StrStr( tokens2[j].c_str(), "Client" ) )
			{
				m_SelectWorkspace.AddString( tokens2[j+1].c_str() );
				if( NULL != StrStr( tokens2[j+1].c_str(), pProfile->szP4Workspace.c_str() ) )
				{
					SetWorkspace( pProfile->szP4Workspace.c_str() );
					m_SelectWorkspace.SetCurSel( i );
				}
			}
		}
		tokens2.clear();
	}
	tokens1.clear();

	// Get Revision Info
	m_vecRevision.clear();

	sprintf_s( szString, 1024, "p4 -s changes %s", pProfile->szP4UrlFolder.c_str() );
	if( !P4ExecCommand( szString, &szOutput ) )
	{
		MessageBox( szOutput.c_str() );
		return FALSE;
	}
	
	TokenizeA( szOutput, tokens1, " " );
	for( DWORD i=0; i<tokens1.size(); i++ )
	{
		if( NULL != StrStr( tokens1[i].c_str(), "Change" ) )
		{
			m_vecRevision.push_back( atoi( tokens1[i+1].c_str() ) );
			if( m_nLastRevision == 0 )
				m_nLastRevision = atoi( tokens1[i+1].c_str() );
		}
	}

	tokens1.clear();

	return TRUE;
}

void CPatchBuilderP4Dlg::CheckPatchFolder()
{
	std::vector< std::string > szVecResult;
	std::string szLastPatchFolder;

	szLastPatchFolder = m_szPatchFolder.GetBuffer();

	CreateFolder( szLastPatchFolder.c_str() );
	_FindFolder( szLastPatchFolder.c_str(), szVecResult, false, NULL );
	m_nCurrentVersion = 1;
	GetDlgItem( IDC_BASE_REVISION )->EnableWindow( false );
	GetDlgItem( IDC_FULL_VERSION )->EnableWindow( false );
	GetDlgItem( IDC_COPY_SERVER_DATA )->EnableWindow( false );

	if( szVecResult.empty() )
		return;

	bool bDeleteFolder = false;
	int i;
	std::sort( szVecResult.begin(), szVecResult.end() );
	for( i=(int)szVecResult.size()-1; i>=0; i-- )
	{
		szLastPatchFolder = m_szPatchFolder.GetBuffer();
		szLastPatchFolder += "\\";
		szLastPatchFolder += szVecResult[i];

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
					m_nCurrentVersion = atoi( szVecResult[i].c_str() );
					UpdatePatchInfo();
				}
				m_nStartRevision = atoi( pFindPtr + 1 );
				m_nCurrentVersion = atoi( szVecResult[i].c_str() ) + 1;

				if( m_nCurrentVersion > 1 )
				{
					GetDlgItem( IDC_BASE_REVISION )->EnableWindow( true );
					GetDlgItem( IDC_FULL_VERSION )->EnableWindow( true );
					GetDlgItem( IDC_COPY_SERVER_DATA )->EnableWindow( true );
				}
				return;
			}
		}

		char szString[ 1024 ];
		sprintf_s( szString, 1024, "%s 폴더는 잘못된 폴더 입니다. 지울까요?", szLastPatchFolder.c_str() );
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

void CPatchBuilderP4Dlg::RollPacking()
{
	int i;
	CEtPackingFile FileSystem;
	char szString[ 1024 ];

	sprintf_s( szString, _MAX_PATH , "%s\\%08d" , m_szRollPatchFolder.GetBuffer() , m_nCurrentVersion+1 );
	CreateDirectory(szString , NULL );

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.pak", 	m_szRollPatchFolder.GetBuffer(),m_nCurrentVersion+1 ,m_nCurrentVersion+1 );

	FileSystem.NewFileSystem( szString );

	std::sort( m_vecRestoreFile.begin(), m_vecRestoreFile.end() );

	for( i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		char szPath[ _MAX_PATH ], szFileName[ _MAX_PATH ];
		_GetPath( szPath, m_vecRestoreFile[i].c_str() );
		sprintf_s( szFileName, _MAX_PATH, "%s\\%s", m_szRollTempFolder.GetBuffer(), m_vecRestoreFile[i].c_str() );

		FileSystem.ChangeDir( "\\" );

		if( szPath[ 0 ] )
		{
			FileSystem.ChangeDir( szPath );
		}
		FileSystem.AddFile( szFileName );

		char szMessage[ 1024 ];
		sprintf_s( szMessage, 1024, "롤백 파일을 생성중입니다 %d%%", ( int )( i * 100 / m_vecRestoreFile.size() ) );
		SetWorkStatus( szMessage );
	}
	FileSystem.CloseFileSystem();
} 

void CPatchBuilderP4Dlg::RollSaveVersion()
{
	FILE *fp;
	char szString[ 1024 ] =  {0,};
	sprintf_s( szString, _MAX_PATH, "%s\\PatchInfoServer.cfg", m_szRollPatchFolder.GetBuffer() );

	//현재버젼을 스킵한다.
	m_SkipNumber.push_back(m_nCurrentVersion);
	std::sort(m_SkipNumber.begin() , m_SkipNumber.end());

	if( !fopen_s( &fp, szString, "wt" ) )
	{
		//서버 버젼 첫라인에 넣고
		sprintf_s( szString, _MAX_PATH, "version %d\n", m_nCurrentVersion+1 );
		fputs( szString, fp );

		for( int i=0; i<(int)m_SkipNumber.size(); i++ )
		{
			sprintf_s( szString, 1024, "SkipNum %d\n", m_SkipNumber[i] );
			fputs( szString, fp );
		}
		fclose( fp );
	}

	//저장된 스킵넘버 와 현재버젼이 같은것은 삭제
	for( int i=0; i<(int)m_SkipNumber.size(); i++ )
	{
		if( m_SkipNumber[i] == m_nCurrentVersion )
		{
			m_SkipNumber.erase(m_SkipNumber.begin()+i);
			break;
		}
	}
}

void CPatchBuilderP4Dlg::RollBackCopyFile()
{
	SetWorkStatus( "RollBack Files 백업중입니다" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "p4";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

	sprintf_s( szString, 1024, "-s sync %s@%d", m_szP4UrlFolder.GetBuffer(), m_nStartRevision );
	LogWnd::TraceLog( "Command:%s", szString );

	if( ShellExecuteEx( &ShellExecInfo ) )
	{
		::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
	}

	char szMessage[1024]={0,};
	std::string SrcFile , DstFile;
	std::string szRollP4UpdateFolder;
	std::string szRollPatchTempFolder;

	//해당폴더에 롤백 폴더생성 
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\RollBack", m_szPatchFolder.GetBuffer(), m_nCurrentVersion );
	CreateDirectory( szString, NULL );
	m_szRollPatchFolder = szString;

	//롤백 폴더에 템프 파일 생성 여기 파일을 받아온다.
	sprintf_s( szString , _MAX_PATH , "%s\\Temp", m_szRollPatchFolder.GetBuffer() );
	CreateDirectory( szString , NULL );
	m_szRollTempFolder = szString;  

	szRollPatchTempFolder = m_szRollTempFolder.GetBuffer(); 
	szRollP4UpdateFolder = m_szUpdateFolder.GetBuffer();

	for( int i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		SrcFile = szRollP4UpdateFolder + "\\" + m_vecRestoreFile[i];
		DstFile = szRollPatchTempFolder + "\\" + m_vecRestoreFile[i];

		std::string StrChange;
		StrChange = m_vecRestoreFile[i];

		char szPath[1024] = {0 , };
		_GetPath( szPath, DstFile.c_str() );
		CreateFolder( szPath );

		//원본 데이터를 먼저 템프쪽에 카피한다.
		if( CopyFile( SrcFile.c_str(), DstFile.c_str(), FALSE ) == 0 )
		{
			CopyFile( SrcFile.c_str(), DstFile.c_str(), FALSE );
		}
	}
}

void CPatchBuilderP4Dlg::RollCreateDeleteList()
{
	int i;
	char szString[ 1024 ];
	FILE *fp;

	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\Patch%08d.txt", 	m_szRollPatchFolder.GetBuffer(),m_nCurrentVersion+1 ,m_nCurrentVersion+1 );

	fopen_s( &fp, szString, "wt" );
	for( i=0; i<(int)m_vecRollDeleteFile.size(); i++ )
	{
		WCHAR wszBuffer[ _MAX_PATH ];
		char szTemp[ _MAX_PATH ];
		MultiByteToWideChar( CP_UTF8, 0, m_vecRollDeleteFile[i].c_str(), -1, wszBuffer, _MAX_PATH );
		WideCharToMultiByte( CP_ACP, 0, wszBuffer, -1, szTemp, _MAX_PATH, NULL, NULL );

		sprintf_s( szString, 1024, "D %s\n", szTemp );
		fputs( szString, fp );
	}

	int bFindLauncher = false;
	for( i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		if( _stricmp( m_vecRestoreFile[i].c_str(), "DNLauncher.exe" ) == 0 )
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

	for( i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		if( IsPatchFile( m_vecRestoreFile[i].c_str() ) )
		{
			sprintf_s( szString, 1024, "P %s\n", m_vecRestoreFile[i].c_str() );
		}
		else
		{
			sprintf_s( szString, 1024, "C %s\n", m_vecRestoreFile[i].c_str() );
			fputs( szString, fp );
		}
	}
	// 아무것도 없으면 0바이트짜리 파일이 생성되서.. 다운로드 안될수도 있어서. 더미 하나 넣어준다.
	fputs( " ", fp );
	fclose( fp );
}

void CPatchBuilderP4Dlg::RollBackPatch()
{
#ifdef _USE_RTPATCH
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

	CreateRTPatch( m_vecRestoreFile, strPakFileName, strNewFilePath, m_szUpdateFolder, "롤백 파일을 생성중입니다 %d%%" );
#else // _USE_RTPATCH
	RollBackCopyFile();
	RollPacking();
#endif // _USE_RTPATCH
	RollCreateDeleteList();

#ifndef _USE_RTPATCH
	DeleteFolder( m_szRollTempFolder.GetBuffer() );
	char szString[1024]={0,};
#endif // _USE_RTPATCH

	//리비전 기록
	sprintf_s( szString, _MAX_PATH, "%s\\%08d\\%d-%d", m_szRollPatchFolder.GetBuffer(), m_nCurrentVersion + 1, m_nStartRevision, m_nStartRevision );
	CreateDirectory( szString, NULL );

	//롤백폴더에 버젼과 스킵되는 버젼을 저장한다.. 
	RollSaveVersion();
	return ;
}

BOOL CPatchBuilderP4Dlg::FileExistName( char *Path )
{
	if( ::GetFileAttributes( Path ) == 0xFFFFFFFF )
	{
		return FALSE;
	}
	return TRUE; 
}

void CPatchBuilderP4Dlg::GetModuleRoot()
{
	GetModuleFileNameA(NULL, m_szModuleRoot, MAX_PATH);
	int i=0 ; 

	for( i=( lstrlenA( m_szModuleRoot ) - 1 ); i>=0; --i )
	{
		if( m_szModuleRoot[i] == '\\' || m_szModuleRoot[i] == '/' )
		{
			m_szModuleRoot[i] = '\0';
			break;
		}
	}
	if( i < 0 )
	{
		i=0;
		m_szModuleRoot[i] = '\0';
	}
}

void CPatchBuilderP4Dlg::OnBnClickedIdcManualFolderBrowse2()
{
	CFileDialog FileDlg( TRUE, "*.exe", "*.*" );
	if( FileDlg.DoModal() == IDOK )
	{
		m_szManualPatchExe = FileDlg.m_ofn.lpstrFile;
		UpdateData( FALSE );
	}
}

void CPatchBuilderP4Dlg::OnBnClickedPatchFolderBrowse()
{
	CFolderDialog Dlg;

	if( Dlg.DoModal() == IDOK )
	{
		m_szPatchFolder = Dlg.GetFolderPath();
		UpdateData( FALSE );
	}
}

#ifdef _USE_RTPATCH

void CPatchBuilderP4Dlg::MakeTempFiles()
{
	SetWorkStatus( "Backup Files 백업중입니다" );

	char szString[ 1024 ];
	SHELLEXECUTEINFO ShellExecInfo;

	memset( &ShellExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
	ShellExecInfo.cbSize = sizeof( SHELLEXECUTEINFO );
	ShellExecInfo.hwnd = GetSafeHwnd();
	ShellExecInfo.lpFile = "p4";
	ShellExecInfo.nShow = SW_SHOW;
	ShellExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	ShellExecInfo.lpVerb = __TEXT( "open" );
	ShellExecInfo.lpParameters = szString;

	sprintf_s( szString, 1024, "-s sync %s@%d", m_szP4UrlFolder.GetBuffer(), m_nStartRevision );
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
	szP4UpdateFolder = m_szUpdateFolder.GetBuffer();

	for( int i=0; i<(int)m_vecRestoreFile.size(); i++ )
	{
		SrcFile = szP4UpdateFolder + "\\" + m_vecRestoreFile[i];
		DstFile = szPatchBackupFolder + "\\" + m_vecRestoreFile[i];

		std::string StrChange;
		StrChange = m_vecRestoreFile[i];

		char szPath[1024] = {0 , };
		_GetPath( szPath, DstFile.c_str() );
		CreateFolder( szPath );

		//원본 데이터를 먼저 템프쪽에 카피한다.
		if( CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE ) == 0 )
		{
			CopyFile( SrcFile.c_str() , DstFile.c_str() , FALSE );
		}
	}
}

void CPatchBuilderP4Dlg::CreateRTPatch( std::vector<std::string>& vecFileList, LPCTSTR strPakFileName, 
									   LPCTSTR strNewFilePath, LPCTSTR strOldFilePath, LPCTSTR strMessage )
{
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

		_GetPath( szPath, vecFileList[i].c_str() );
		_GetFullFileName( szFullFileName, vecFileList[i].c_str() );
		sprintf_s( szNewFile, _MAX_PATH, "%s\\%s", strNewFilePath, vecFileList[i].c_str() );
		sprintf_s( szOldFile, _MAX_PATH, "%s\\%s", strOldFilePath, vecFileList[i].c_str() );

		// 1. OldFile 없다면 ADD된 것으로 처리 함
		BOOL bOldFileExist = FileExistName( szOldFile );
		if( !FileExistName( szNewFile ) )	// NewFile이 없다면 Error처리
		{
			ASSERT( "Patch File is not Exist!!!" );
			break;
		}

		// 2. patchbld 실행 rtp파일을 만든다.
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

		LogWnd::TraceLog( "Command:%s", szString );

		if( ShellExecuteEx( &ShellExecInfo ) )
		{
			::WaitForSingleObject( ShellExecInfo.hProcess, INFINITE );
		}

		// 3. 생성된 RTP파일명을 원본 파일명으로 변경 (확장자 .rtp는 유지)
		char szRename[_MAX_PATH];
		sprintf_s( szRename, _MAX_PATH, "RTPatchFileTemp\\%s.rtp", szFullFileName );
		rename( "RTPatchFileTemp\\TEMP.RTP", szRename );



		FileSystem.ChangeDir( "\\" );
		if( szPath[0] )
		{
			FileSystem.ChangeDir( szPath );
		}

		// 4. RTP파일을 패킹파일 시스템에 추가
		FileSystem.AddFile( szRename );
		
		sprintf_s( szMessage, 1024, strMessage, ( int )( i * 100 / m_vecModifyFile.size() ) );
		SetWorkStatus( szMessage );
	}

	FileSystem.CloseFileSystem();
	
	// 5. 정리
	DeleteFolder( "RTPatchFileTemp" );
}

#endif // _USE_RTPATCH