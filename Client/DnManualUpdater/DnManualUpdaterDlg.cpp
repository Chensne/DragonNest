// DnManualUpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DnManualUpdater.h"
#include "DnManualUpdaterDlg.h"
#include "DnAttachFile.h"
#include "EtPackingStream.h"
#include "EtFileSystem.h"
#include "SundriesFunc.h"
#include <new.h>
#include "VarArg.h"
#include "wininet.h"

#pragma comment ( lib, "Wininet.lib" )


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#include "atlconv.h"

// CDnManualUpdaterDlg dialog

CDnManualUpdaterDlg::CDnManualUpdaterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CDnManualUpdaterDlg::IDD, pParent)
, m_strVersion(_T(""))
, m_strState(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bTerminateThread = false; 
	m_bUserCancel = false; 
	m_bCompleted = false; 
	m_bFinishThread = false;
	LogWnd::CreateLog(true, LogWnd::LOG_TARGET_FILE);

}

CDnManualUpdaterDlg::~CDnManualUpdaterDlg()
{
	
	LogWnd::DestroyLog();
	delete m_pDnAttachFile;

}

void CDnManualUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ctlProgressTotal);
	DDX_Control(pDX, IDC_PROGRESS2, m_ctlProgressFile);

	DDX_Text(pDX, IDC_STATE_VERSION, m_strVersion);
	DDX_Text(pDX, IDC_STATE_INFO, m_strState);
}

BEGIN_MESSAGE_MAP(CDnManualUpdaterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATE_VERSION, &CDnManualUpdaterDlg::OnStnClickedStateVersion)
	ON_BN_CLICKED(IDCANCEL, &CDnManualUpdaterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CDnManualUpdaterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


UINT AFX_CDECL _BuildPatchThread( LPVOID Param )
{
	CDnManualUpdaterDlg *pMainDlg = ( CDnManualUpdaterDlg * )AfxGetApp()->m_pMainWnd;
	if ( pMainDlg )
	{
		pMainDlg->UpdateMain();
	}

	pMainDlg->m_bFinishThread = true;
	return 0;
}

bool CDnManualUpdaterDlg::InitializePatchUpdate()
{
	if( !g_bSilenceMode ) {
		GetDlgItem( IDOK )->EnableWindow( FALSE );
		GetDlgItem( IDCANCEL )->EnableWindow( TRUE );
	}
	USES_CONVERSION;

	// TODO: Add extra initialization here
	TCHAR szStr[_MAX_PATH]={0,};
	TCHAR szDownRoot[1024]={0,};
	m_pDnAttachFile = new CDnAttachFile; 

	m_PackStep = emOutPack;


	if( PathFileExistsA( "dnlauncher.exe" )  == FALSE )
		//	if( NULL == fopen("dnlauncher.exe" , "r") ) 
	{
		AfxMessageBox(STR_DRAGON_NEST_RUN_IN_FOLDER);
		CDialog::OnOK();
		return false; 
	}

	m_strState = STR_CONFIRM_PATCH_VER;

#ifdef PRE_DELETE_VERSION
	DeletePakFile(_T("Version.cfg"));
	CDialog::OnOK();
	return false;
#endif 


	//��ġ ����°�... ���� �������� ���ͼ� ��ġ�� �����. 
#ifdef PRE_ADD_MAKE_PAK
	char ModuleFileName[_MAX_FNAME] = {0,} ;
	char OutFileName[1024] = "DNUpdater_5to6.exe";
	sprintf_s( OutFileName , sizeof(OutFileName) , "DNUpdater_%dto%d.exe" , 6 , 7);
	strcpy_s( ModuleFileName  ,_MAX_FNAME , "DnManualUpdater.exe" );
	//������ ���� ����κп� ����.
	m_pDnAttachFile->Create( ModuleFileName  , OutFileName );
	//�߰��ϰ� ���� ������ ������ AttachFile ��� ȣ���Ѵ�.
	//���� ������ �ؽ�Ʈ���� �鰡�� ���� ������ ������ ������ �����. 
	//����ġ �ϴºκ��� �ѹ�������� �ۿ´�.
	m_pDnAttachFile->WriteVersion( 0 ,6);

	for(int i = 0 ; i < 6 ; i++)
	{
		CStringA txt , Pak; 
		txt.Format("Patch%08d.txt" , i+1);
		Pak.Format("Patch%08d.pak" , i+1);

		m_pDnAttachFile->AttachFile(  W2A( m_szClientPath.GetBuffer())  ,  txt.GetBuffer()  );
		m_pDnAttachFile->AttachFile(  W2A( m_szClientPath.GetBuffer())  ,  Pak.GetBuffer()  );
	}

	//////////////////////////////////////////////////////////////////////////
	m_pDnAttachFile->Close();
	AfxMessageBox(_T("������ġ�� ��������ϴ�."));
	CDialog::OnOK();
	return true;
#endif 

	//////////////////////////////////////////////////////////////////////////
	// ���ҽ� ����ȭ �Ǵ°ž� ���߿� �׽�Ʈ
	//	ResourceOptimize();	
	//////////////////////////////////////////////////////////////////////////

	char str[1024]={0,};

	GetModulePath();
	GetModuleFileName();

	//������ ����

	if ( ! m_pDnAttachFile->Open(W2A(m_szModuleName)) )
	{
		if( g_bSilenceMode ) {
			exit(-1);
		}
		AfxMessageBox( STR_FAILED_PATCH );
		CDialog::OnOK();
		return false; 
	}

	//ù����ü�� �������� �������� �д´�.
	SAttachFileHeader header;
	if( !m_pDnAttachFile->ReadHeader( &header))
	{
		if( g_bSilenceMode ) {
			exit(-2);
		}
		AfxMessageBox( STR_NOT_READ_DATA );
		CDialog::OnOK();
		return false;
	}

	m_iPrevVersion = atoi(header.FileName);
	m_iLastVersion = header.FileSize;

	int nVersion = -1;

#ifdef USE_SERVER_VERSION_CHECK
	// ���� ���� üũ
	if( !g_bPassCheckVersion )
	{
		nVersion = CheckServerVersion();
		if( nVersion < m_iLastVersion )
		{
			if( g_bSilenceMode )
				exit(-4);
			
			CString Str; 
			Str.Format(_T("%s Server Ver.%d Manaual Ver.%d") , STR_MANUALPATCH_NOT_MATCH_SERVER_VERSION, nVersion, m_iLastVersion );
			MessageBox( Str , _T("Error"), MB_ICONINFORMATION );

			CDialog::OnOK();
			return false; 
		}
	}
#endif // USE_SERVER_VERSION_CHECK

	//Ŭ���̾�Ʈ ������ �о�´�. 
	nVersion = LoadVersion();

	if(nVersion == -1)
	{
		if( g_bSilenceMode ) {
			exit(-3);
		}
		//AfxMessageBox( STR_CLIENT_NOT_READ_VERSION );
		CDialog::OnOK();
		return false; 
	}

	if( nVersion != m_iPrevVersion ) 
	{
		if( g_bSilenceMode ) 
			exit(-4);

		CString Str; 
		Str.Format(_T("%s Client Ver.%d Manaual Ver.%d") , STR_MANUALPATCH_NOT_MATCH_VERSION, nVersion, m_iPrevVersion );
		MessageBox( Str , _T("Error"), MB_ICONINFORMATION );

		CDialog::OnOK();
		return false; 
	}

	//������Ʈ ���� ����
	swprintf_s(szDownRoot , _countof(szDownRoot) , _T("%s%s") ,m_szClientPath ,  _T(UPDATER_TEMP_LOCAL_DIR) );
	//������ ������ ���� �����ְ� 
	DeleteFolder(W2A(szDownRoot));
	CreateFolder(W2A(szDownRoot));
	//���� ������ ������ �� ���� �����ش�. 
	m_dwTotalSize =  GetFileSize( m_szModulePath ) -  m_pDnAttachFile->GetExeModuleSize();
	m_dwNowTotalsize = 0 ; 
	m_dwFileSize = 0 ; 
	m_dwNowFileSize =0 ; 

	if( !g_bSilenceMode ) {
		m_ctlProgressFile.SetRange32( 0 , m_dwTotalSize );
		m_ctlProgressFile.SetPos(0);

		m_ctlProgressTotal.SetRange32( 0 , (m_iLastVersion -  m_iPrevVersion)*2 );
		m_ctlProgressTotal.SetPos(0);
		m_strVersion.Format(_T("Version %d -> %d") , m_iPrevVersion , m_iLastVersion );
		UpdateData(FALSE);

		SetTimer(1,1 ,NULL );
	}

	m_thread = AfxBeginThread( _BuildPatchThread , GetSafeHwnd() );
	m_hThread = m_thread->m_hThread;

	return true;
}

#ifdef USE_SERVER_VERSION_CHECK

int CDnManualUpdaterDlg::CheckServerVersion()
{
	int nServerVersion = -1;

	CString strVersionUrl;
#if defined (_THA)
	strVersionUrl = L"http://auto-dgn.asiasoft.co.th/real/Client/";
#elif defined (_IDN)
	strVersionUrl = L"http://patch.gemscool.com/dragonnest/live/";
#endif

	// Ŭ���̾�Ʈ ��� ����
	TCHAR szCurDir[ _MAX_PATH ]={0,};
	GetCurrentDirectory( _MAX_PATH, szCurDir );

	CString strFilePath;
	strFilePath = szCurDir;
	int nLast = strFilePath.GetLength();
	if ( strFilePath[nLast] != _T('\\') || strFilePath[nLast] != _T('/') )
		strFilePath += _T('\\');

	strFilePath += SERVER_VERSION_NAME;

	HRESULT hr = E_FAIL;
	for( int i=0; i<RETRY_MAX_COUNT; i++ )
	{
		CString strVersionUrlFile = strVersionUrl + SERVER_VERSION_NAME;
		BOOL bResult = DeleteUrlCacheEntry( strVersionUrlFile.GetBuffer() );	// ������ ���� ���ϱ� PatchInfoServer.cfg �� �ٿ�ε�
		hr = DownloadToFile( strVersionUrlFile.GetBuffer(), strFilePath.GetBuffer() );
		if( hr == S_OK )
			break;
	}

	if( hr == S_OK )
	{
		nServerVersion = LoadVersion( strFilePath );
		DeleteFile( strFilePath );
	}

	return nServerVersion;
}

#endif // USE_SERVER_VERSION_CHECK

HRESULT CDnManualUpdaterDlg::DownloadToFile( LPCTSTR strDownloadUrl, LPCTSTR strSaveFilePath )
{
	HINTERNET hInternetSession;
	HINTERNET hHttpConnection;
	// ���� ����
	LogWnd::TraceLog( L"Open Internet Session" );
	hInternetSession = InternetOpen( NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );

	if( hInternetSession == NULL )
	{
		LogWnd::Log( LogLevel::Error, L"Internet Session Open Failed!" );
		return S_FALSE;
	}

	// Http ����
	LogWnd::TraceLog( L"Open Http Connection" );
	CString strAddHeader;
	strAddHeader.Format( L"Cache-Control:no-cache\nPragma:no-cache" );

	hHttpConnection = InternetOpenUrl( hInternetSession, strDownloadUrl, strAddHeader, strAddHeader.GetLength(), 
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0 );

	if( hHttpConnection == NULL )
	{
		LogWnd::Log( LogLevel::Error, L"Http Connection Open Failed!" );
		InternetCloseHandle( hInternetSession );
		return S_FALSE;
	}

	TCHAR szBuffer[MAX_PATH]={0,};
	DWORD dwBufferSize = MAX_PATH;
	DWORD dwIndex = 0;

	if( HttpQueryInfo( hHttpConnection, HTTP_QUERY_STATUS_CODE, (LPVOID)&szBuffer, &dwBufferSize, &dwIndex ) )
	{
		CString strReply = szBuffer;
		if( strReply != L"200" )
		{
			InternetCloseHandle( hHttpConnection );
			InternetCloseHandle( hInternetSession );
			return S_FALSE;
		}
	}

	// Save File
	CFile file;
	CFileException e;
	UINT uiFileModeFlag = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone;

	if( !file.Open( strSaveFilePath, uiFileModeFlag, &e ) )
	{
		LogWnd::Log( LogLevel::Error, L"Download File Open Failed!" );
		InternetCloseHandle( hHttpConnection );
		InternetCloseHandle( hInternetSession );
		return S_FALSE;
	}

	LogWnd::TraceLog( L"Download File Start" );

	DWORD dwRead, dwSize;
	char szReadBuf[32768];	// 32KiB

	BOOL bError = FALSE;

	do 
	{
		if( !InternetQueryDataAvailable( hHttpConnection, &dwSize, 0, 0 ) )
		{
			LogWnd::Log( LogLevel::Error, L"InternetQueryDataAvailable : data not available!" );
			bError = TRUE;	// error "data not available!"
			break;
		}

		if( !InternetReadFile( hHttpConnection, szReadBuf, dwSize, &dwRead ) )
		{
			LogWnd::Log( LogLevel::Error, L"InternetReadFile : reading file fail!" );
			bError = TRUE;	// error "reading file fail!"
			break;
		}

		if( dwRead )
		{
			file.Write( szReadBuf, dwRead );
		}
	} while( dwRead != 0 );

	InternetCloseHandle( hHttpConnection );
	InternetCloseHandle( hInternetSession );
	file.Close();

	if( bError )
		return S_FALSE;

	LogWnd::TraceLog( L"Download File Success" );

	return S_OK;
}

int CDnManualUpdaterDlg::LoadVersion( CString& szFilePath )
{
	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	int nVersion = 0;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD tmp = 0;
	DWORD nFileSize = 0;
	nFileSize = ::GetFileSize( hFile, &tmp );
	nFileSize++;

	// 7 byte �̸��������� Ŭ���̾�Ʈ version.cfg ������ ������ ���ο������ ��ü�Ѵ�.
	if( nFileSize < 5 )
	{
		AfxMessageBox( _T("Error the old version.cfg.") );
		CloseHandle(hFile);
		return -1; 
	}

	buffer = new char[nFileSize+1];
	if( buffer == NULL )
		return -1;

	ZeroMemory( buffer, nFileSize+1 );

	DWORD nReadSize = 0;
	ReadFile( hFile, buffer, nFileSize, &nReadSize, NULL );
	CloseHandle( hFile );	

	char arg1[256]={0,}, arg2[256]={0,};
	char *szToken = NULL, *nextToken = NULL;
	char delimiters[] = "\r\n";

	szToken = strtok_s( buffer, delimiters, &nextToken );

	while( szToken != NULL )
	{
		if( sscanf_s( szToken, "%s %s", &arg1, sizeof(arg1), &arg2, sizeof(arg2) ) )
		{
			_strlwr_s( arg1 );
			// version ���� Ŭ�� �����̸� ����
			if( _stricmp( arg1, "version" ) == 0 )
				nVersion = atol( arg2 );
		}
		szToken = strtok_s( NULL, delimiters, &nextToken );
	}
	SAFE_DELETE_ARRAY( buffer );

	return nVersion;
}

// CDnManualUpdaterDlg message handlers
BOOL CDnManualUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	USES_CONVERSION;

	//return 0; 
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitializePatchUpdate();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDnManualUpdaterDlg::ResourceOptimize()
{

// 	
	std::vector< CEtPackingFile * > vecPackingFile;
	char szTemp[ __MAX_PATH ] = {0,};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString szFindPackingFile;

	szFindPackingFile =  m_szClientPath;
	szFindPackingFile += _T("Resource*.pak");

	hFind = FindFirstFile( szFindPackingFile.GetBuffer() , &FindFileData );

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			CString szPackingFile;
			szPackingFile = m_szClientPath;
			szPackingFile += FindFileData.cFileName;


			CEtPackingFile *pPackingFile = new CEtPackingFile();

			char szTemp[ __MAX_PATH ] = {0,};

			WideCharToMultiByte( CP_ACP, 0, szPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
	
			if( pPackingFile->OpenFileSystem( szTemp ) )
			{
				vecPackingFile.push_back( pPackingFile );
			}
					
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}
	FindClose( hFind );

	for( int i = 0; i < ( int )vecPackingFile.size(); i++ )
	{
		if( vecPackingFile[ i ]->OptimizeFileSystem("Optimize") ) 
		{
			vecPackingFile[ i ]->CloseFileSystem() ;
			CStringA szResouceName;
			szResouceName.Format("Resource%2d.pak", i );

			if( DeleteFileA(szResouceName.GetBuffer()))
			{
				rename("Optimize" , szResouceName.GetBuffer() );
			}
		}
	}
}

void CDnManualUpdaterDlg::OnTimer(UINT nIDEvent)
{
	if(nIDEvent == 1 )
	{	
		
		if( m_PackStep == emOutPack ) 
		{
			if( m_dwNowTotalsize < m_dwTotalSize )
			{
				m_dwNowFileSize = m_pDnAttachFile->GetCurDetachSize();
			}
		}
		
		m_ctlProgressFile.SetPos( m_dwNowFileSize);
		m_ctlProgressTotal.SetPos( m_dwNowTotalsize );
		
		UpdateData(FALSE);
	}
	else if(nIDEvent == 2)
	{
		static int cnt = 1;
		cnt = ++cnt % 3;
		if( cnt )
		{
			m_strState = STR_PATCH_COMPLETE;
		}
		else 
		{
			m_strState ="";
		}
	
		UpdateData(FALSE);
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CDnManualUpdaterDlg::DeletePakFile(TCHAR *pFileName)
{
	//��γ� .. ��.. 3�� �ҷ����µ��� 2�ʸԳ׽�
	char szTemp[ _MAX_PATH ] = {0,};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	USES_CONVERSION;
	hFind = FindFirstFile( _T(".\\Resource*.pak") , &FindFileData );

	CEtPackingFile *pPackingFile = new CEtPackingFile();
	CEtFileHandle *FileHandle = NULL;
	

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			if( pPackingFile->OpenFileSystem( CVarArgA<MAX_PATH>(".\\%s", T2CA(FindFileData.cFileName) ) ) )
			{
				pPackingFile->Remove(W2A(pFileName));
				break; 
			}
			else
			{
				delete pPackingFile;
				break; 
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}
	FindClose( hFind );		
	pPackingFile->CloseFileSystem();
	delete pPackingFile;
	
//	AfxMessageBox(_T("Delete complete Version.cfg"));
}

int CDnManualUpdaterDlg::GetFileSize(TCHAR *pFileName)
{
	HANDLE hFile = CreateFile(	pFileName,   
		GENERIC_READ,              // open for reading 
		FILE_SHARE_READ,           // share for reading 
		NULL,                      // no security 
		OPEN_EXISTING,             // existing file only 
		FILE_ATTRIBUTE_NORMAL,     // normal file 
		NULL );    

	if( hFile == INVALID_HANDLE_VALUE ) return 0;

	DWORD fileSize = ::GetFileSize( hFile, NULL );
	if( fileSize == 0xFFFFFFFF ) return 0;

	CloseHandle( hFile );
	
	return fileSize;
}


void CDnManualUpdaterDlg::GetModulePath()
{
	TCHAR szStr[256]={0,};
	GetCurrentDirectory( _MAX_PATH, szStr );
	m_szClientPath = szStr; 
	m_szClientPath += _T("\\");
	m_szClientUpdatePath = m_szClientPath;
	m_szClientUpdatePath += _T(UPDATER_TEMP_LOCAL_DIR);
	m_szClientUpdatePath += "\\";
	
}


void CDnManualUpdaterDlg::GetModuleFileName()
{
	TCHAR szCurFileName[MAX_PATH];
	::GetModuleFileName(NULL, szCurFileName, MAX_PATH);
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_tsplitpath(szCurFileName, szDrive, szDir, szFileName, szExt );
	m_szModuleName.Format(_T("%s%s"), szFileName, szExt);
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CDnManualUpdaterDlg::OnPaint()
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
HCURSOR CDnManualUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool CDnManualUpdaterDlg::LoadPatchList( std::vector< std::string > &vecDeleteList, std::vector< std::string > &vecPatchList, std::vector< std::string > &vecCopyList )
{
	HANDLE hFile = CreateFile( m_szPatchListFile.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::TraceLog( _T("%s File Not Found"), m_szPatchListFile.GetBuffer() );
		return false;
	}
	char cPatchCode, szString[ 1024 ], *pFindPtr;
	int nFileSize = ::GetFileSize( hFile, NULL );
	char *pBuffer = new char [ nFileSize + 3 ]; // ���� ���� ������� pBuffer + 2 �� �Ŀ� pBuffer �� �����Ⱚ�̿��� strchr ���� ���� �� �ִ�.
	memset( pBuffer, 0, nFileSize + 3 );

	char *pBufferBackup = pBuffer;
	DWORD dwReadSize;
	ReadFile( hFile, pBuffer, nFileSize, &dwReadSize, NULL );
	while( 1 )
	{
		cPatchCode = *pBuffer;
		pBuffer += 2;
		pFindPtr = strchr( pBuffer, 0x0d );
		if( pFindPtr == NULL )
		{
			break;
		}
		if( ( int )( pBuffer - pBufferBackup ) > ( int )dwReadSize )
		{
			break;
		}
		*pFindPtr = 0;
		strcpy_s( szString, 1024, pBuffer );
		int nStrLength = ( int )strlen( szString );
		if( ( szString[ nStrLength - 1 ] == 0x0d ) || ( ( szString[ nStrLength - 1 ] == 0x0a ) ) )
		{
			szString[ nStrLength - 1 ] = 0;
		}
		pBuffer += nStrLength + 1;
		if( *pBuffer == 0x0a )
		{
			pBuffer++;
		}
		switch( cPatchCode )
		{
		case 'D':
			vecDeleteList.push_back( szString );
			break;
		case 'C':
			vecCopyList.push_back( szString );
			break;
		case 'P':
			vecPatchList.push_back( szString );
			break;
		default:
			assert( 0 && "Invalid Patch Code" );
			delete [] pBufferBackup;
			return false;
		}
	}
	CloseHandle( hFile );
	delete [] pBufferBackup;

	return true;
}

void CDnManualUpdaterDlg::UpdateMain()
{
	Sleep(1000);
	//���� ��
	//��ŷ�� ��������� Ǭ��. 
	bool bExit = true; 
	SAttachFileHeader header;
	//�׻� ¦���� �̷������. 
	SetCurrentDirectory(m_szClientPath.GetBuffer());

	while(m_pDnAttachFile->ReadHeader( &header ))
	{
		if(m_bUserCancel)
		{
			m_pDnAttachFile->Close();
		}

		if( !g_bSilenceMode ) m_strState = header.FileName;
		m_dwFileSize = header.FileSize;
		m_dwNowFileSize = 0;

		if( !g_bSilenceMode )  {
			m_ctlProgressFile.SetRange32(0 , header.FileSize );
			m_ctlProgressFile.SetPos(0);
		}

		m_pDnAttachFile->Detach( ".\\update" , &header);

		if( StrStr(m_strState , _T(".pak")))
		{
			m_dwNowTotalsize++;
		}

		if( m_bTerminateThread )
		{
			return;
		}

	}

	m_pDnAttachFile->Close(); 
	
	m_strState = STR_INSTALL_PATCH_DATA;
	m_PackStep = emApplyPack;

	if( !CheckPackFile() ) 
	{
		if( g_bSilenceMode ) {
			exit(-5);
		}
		AfxMessageBox( STR_FAILED_PATCH );
		CDialog::OnOK();
		return;
	}

	SaveVersion(m_iLastVersion);
	
	if( !g_bSilenceMode ) {
		m_strState = STR_PATCH_COMPLETE;
		m_ctlProgressFile.SetRange32(0,1);
		m_ctlProgressFile.SetPos(1);
		m_ctlProgressTotal.SetRange32(0,1);
		m_ctlProgressTotal.SetPos(1);

		KillTimer( 1 );

		SetTimer(2 , 800 , NULL );
	}
	TCHAR szDownRoot[1024]={0,};
	swprintf_s(szDownRoot , _countof(szDownRoot) , _T("%s%s") ,m_szClientPath ,  _T(UPDATER_TEMP_LOCAL_DIR) );
	//������ ������ ���� �����ְ� 
	USES_CONVERSION;
	DeleteFolder(W2A(szDownRoot));

	if( !g_bSilenceMode ) {
		GetDlgItem( IDOK )->EnableWindow( TRUE );
		GetDlgItem( IDCANCEL )->EnableWindow( FALSE );
	}
}

int CDnManualUpdaterDlg::LoadVersionOld()
{
	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	int version = -1 ;
	
	SetCurrentDirectory(m_szClientPath.GetBuffer());
	
	hFile = CreateFile( _T(".\\Version.cfg") , GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD tmp = 0;
	DWORD nFileSize = 0;
	nFileSize = ::GetFileSize(hFile, &tmp);
	nFileSize++;

	//7 byte �̸��������� Ŭ���̾�Ʈ version.cfg ������ ������ ���ο������ ��ü�Ѵ�.

	if( nFileSize < 5 )
	{
		AfxMessageBox(_T("Error the old version.cfg. "));
		return -1; 
	}

	buffer = new char[nFileSize];
	ZeroMemory(buffer, nFileSize);

	DWORD nReadSize = 0;
	ReadFile(hFile, buffer, nFileSize, &nReadSize, NULL );
	CloseHandle(hFile);

	char arg1[256]={0,} , arg2[256]={0,};

	char *szToken , *nextToken ,delimiters[] = "\r\n";
	szToken = strtok_s(buffer , delimiters , &nextToken );

	while( szToken != NULL )
	{
		if(sscanf_s(szToken , "%s %s" ,&arg1 , sizeof(arg1) ,&arg2 , sizeof(arg2)))			 
		{
			_strlwr_s( arg1 );
			//version ���� Ŭ�� �����̸� ����
			if(_stricmp(arg1 , "version") == 0 )
			{
				version = atol(arg2); 
			}
		}
		szToken = strtok_s( NULL, delimiters  ,&nextToken );
	}
	SAFE_DELETE(buffer);

	return version;
}

int CDnManualUpdaterDlg::LoadVersion()
{
	
	CString szFindPackingFile;

	szFindPackingFile =  m_szClientPath;
	szFindPackingFile += _T("Resource00.pak");

	CEtPackingFile *pPackingFile = new CEtPackingFile();
	CEtFileHandle *FileHandle = NULL;

	char StrVersionName[32]= "\\version.cfg";
	char szTemp[ __MAX_PATH ] = {0,};
	char *Buff = NULL ; 
	int nFileSize = 0 ; 
	int Version = 0 ; 

	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		FileHandle = pPackingFile->OpenFile(StrVersionName);

		if( FileHandle ) 
		{
			nFileSize = FileHandle->GetFileContext()->dwAllocSize;
			Buff = new char[nFileSize];
			FileHandle->Read(Buff ,  nFileSize );
		}
		else 
		{
			Version = LoadVersionOld();

			if( Version == -1)
			{
				pPackingFile->CloseFile(FileHandle);
				pPackingFile->CloseFileSystem();
				
				AfxMessageBox(STR_CLIENT_NOT_READ_VERSION);
				return -1;
			}
			else 
			{
				//�Ѿȿ� ������ ������ ������ �־�����. 
				char strVersion[256]={0,};
				sprintf_s( strVersion , sizeof(strVersion) , "version %d" , Version );
				char StrVersionName[32]= "\\version.cfg";
				pPackingFile->AddFile(StrVersionName , strVersion  , sizeof(strVersion) );
				pPackingFile->CloseFile(FileHandle);		
				pPackingFile->CloseFileSystem();
				return Version; 
			}
		}
	}
	else 
	{
		AfxMessageBox( STR_NOT_READ_DATA );
		delete pPackingFile;
		return -1;
	}

	char arg1[256]={0,} , arg2[256]={0,};
	char *szToken , *nextToken ,delimiters[] = "\r\n";
	
	szToken = strtok_s(Buff  , delimiters , &nextToken );

	while( szToken != NULL )
	{
		if(sscanf_s(szToken , "%s %s" ,&arg1 , sizeof(arg1) ,&arg2 , sizeof(arg2)))			 
		{
			_strlwr_s( arg1 );
			//version ���� Ŭ�� �����̸� ����
			if(_stricmp(arg1 , "version") == 0 )
			{
				Version = atol(arg2); 
				break; 
			}
		}
		szToken = strtok_s( NULL, delimiters  ,&nextToken );
	}
	
	delete Buff;
	pPackingFile->CloseFile(FileHandle);
	pPackingFile->CloseFileSystem();
	SAFE_DELETE(pPackingFile);
	
	return Version;
}

BOOL CDnManualUpdaterDlg::SaveVersionOld(int Version)
{

	CString szFilePath;

	szFilePath =  m_szClientPath;
	szFilePath += _T("Version.cfg");

	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	//���̺��Ҷ��� ������ �׻� ����� ���̺���.
	DeleteFile(szFilePath.GetBuffer());

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL,NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	char szVersion[ 256 ]= {0,};
	DWORD dwWriteSize;

	sprintf_s( szVersion, 256, "Version %d\r\n", Version );

	WriteFile( hFile, szVersion,(int)strlen(szVersion)+1 , &dwWriteSize, NULL );
	CloseHandle(hFile);
	
	return TRUE; 

}


BOOL CDnManualUpdaterDlg::SaveVersion(int Version)
{
	CString szFindPackingFile;

	szFindPackingFile =  m_szClientPath;
	szFindPackingFile += _T("Resource00.pak");
	
	CEtPackingFile *pPackingFile = new CEtPackingFile();
	char strVersion[256]={0,};
	sprintf_s( strVersion , sizeof(strVersion) , "version %d" ,Version );
	
	USES_CONVERSION;

	char szTemp[ __MAX_PATH ] = {0,};
	char StrVersionName[32]= "\\version.cfg";
	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );

	//��¿�� ����. 512M �Ѿ�°��߿� ��� �߰�����. 
	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		pPackingFile->Remove(StrVersionName);
		pPackingFile->AddFile(StrVersionName , strVersion  , sizeof(strVersion) );
	}
	else 
	{
		MessageBox(_T("Version can not be saved") , _T("Error") , MB_OK);
		return FALSE;
	}
	pPackingFile->CloseFileSystem();
	delete pPackingFile;

	SaveVersionOld(Version);
	

	return TRUE;
}

bool CDnManualUpdaterDlg::CheckPackFile()
{
	//�� ���丮�� pak ������ �ִ��� �����Ѵ�.
	char szTemp[ __MAX_PATH ] = {0,};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString szFindPakFile;
	szFindPakFile = m_szClientUpdatePath;
	szFindPakFile += _T("Patch*.pak");

	hFind = FindFirstFile( szFindPakFile.GetBuffer() , &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{		
			CString szFileName = FindFileData.cFileName;
			szFileName.Replace(_T(".pak") ,  _T(""));
			szFileName.Replace(_T("Patch") , _T(""));
			int nVersion = _wtoi( szFileName.GetBuffer() ) ;

			m_szPatchFile.Format( _T( "%sPatch%08d.pak" ), m_szClientUpdatePath.GetBuffer(), nVersion );
			m_szPatchListFile.Format( _T( "%sPatch%08d.txt" ) , m_szClientUpdatePath.GetBuffer(), nVersion );
			
			//�ѵ� ������ �����Ű��
			if( !FilePacking())
			{
				return FALSE;
			}
			m_dwNowTotalsize++;
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}
	FindClose( hFind );
	m_bCompleted = true; 
	return TRUE;
}

int CDnManualUpdaterDlg::FilePacking()
{

	LogWnd::TraceLog( _T( "apply pak 1"));

	std::vector< std::string > vecDeleteList, vecPatchList, vecCopyList;

	//�ؽ�Ʈ �����͸� ���� �ҷ��;� �Ѵ�. 
	if( !LoadPatchList( vecDeleteList, vecPatchList, vecCopyList ) )
	{
		LogWnd::TraceLog( _T( "apply pak _LoadPatchList failed"));
		return FALSE;
	}

	LogWnd::TraceLog( _T( "apply pak 2"));
	//�� ���丮�� ���ҽ����� ����� �ִ�ã�� �׾ȿ� ������ �����´�.
	std::vector< CEtPackingFile * > vecPackingFile;
	char szTemp[ __MAX_PATH ] = {0,};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString szFindPackingFile;

	szFindPackingFile =  m_szClientPath;
	szFindPackingFile += _T("Resource*.pak");

	hFind = FindFirstFile( szFindPackingFile.GetBuffer() , &FindFileData );
	

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			CString szPackingFile;
			szPackingFile = m_szClientPath;
			szPackingFile += FindFileData.cFileName;

			CEtPackingFile *pPackingFile = new CEtPackingFile();	

			char szTemp[ __MAX_PATH ] = {0,};
			
			WideCharToMultiByte( CP_ACP, 0, szPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
			if( pPackingFile->OpenFileSystem( szTemp ) )
			{
				vecPackingFile.push_back( pPackingFile );
			}
			else
			{
				if( pPackingFile->OpenFileSystem( szTemp ) )
				{
					vecPackingFile.push_back( pPackingFile );
				}
				else
				{
					delete pPackingFile;
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
		if( m_bTerminateThread ) {
			for( int m = 0; m < ( int )vecPackingFile.size(); m++ )
				vecPackingFile[ m ]->CloseFileSystem();
			return FALSE;
		}
	}
	FindClose( hFind );

	if( vecPackingFile.empty() )
	{
		LogWnd::TraceLog( _T( "*.pak File Not Found!!!"));
		return 0;
	}

	LogWnd::TraceLog( _T( "apply pak 3"));

	int i, j;
	int nRet = 1;
	CEtPackingFile PatchSource;

	WideCharToMultiByte( CP_ACP, 0, m_szPatchFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
	PatchSource.OpenFileSystem(  szTemp , true );


	m_dwFileSize = ( int )vecDeleteList.size() + PatchSource.GetFileCount();
	m_dwNowFileSize = 0; 

	if( m_dwFileSize == 0 )
	{
		AfxMessageBox(_T("FileSize 0"));
	}
	if( !g_bSilenceMode ) {
		m_ctlProgressFile.SetRange32(0 , m_dwFileSize );
		m_ctlProgressFile.SetPos(0);
	}

	for( i = 0; i < ( int )vecDeleteList.size(); i++ )
	{
		char szPath[ __MAX_PATH+100 ], szFileName[ __MAX_PATH+100 ];
		_GetPath( szPath, __MAX_PATH+100, vecDeleteList[ i ].c_str() );
		_GetFullFileName( szFileName, __MAX_PATH+100, vecDeleteList[ i ].c_str() );
		bool bIsNormalFile = true; 

		for( j = 0; j < ( int )vecPackingFile.size(); j++ )
		{
			vecPackingFile[ j ]->ChangeDir( "\\" );

			if( szPath[ 0 ] )
			{
				vecPackingFile[ j ]->ChangeDir( szPath );
			}
			if( vecPackingFile[ j ]->Remove( szFileName ) )
			{
				// ���� ������ ���� ���Ϸ� �Ѿ��.
				m_dwNowFileSize++;
				//OnPatch( m_dwNowFileSize, nTotalPatchCount, "" );
				bIsNormalFile = false; 
				break;
			}
		}
		
		if( bIsNormalFile ) 
		{
			//�Ϲ� ���丮 ���� ���� �߰�. 
			char CheckExe[256]={0 ,};
			_GetExt( CheckExe, 256, vecDeleteList[i].c_str() );
			if( CheckExe == '\0')
			{
				m_dwNowFileSize++;
				DeleteFolder(vecDeleteList[i].c_str());
			}
			else
			{
				DeleteFileA(CVarArgA<MAX_PATH>(".\\%s", vecDeleteList[ i ].c_str()));
				m_dwNowFileSize++;
			}
		}


		if( m_bTerminateThread ) {
			for( int m = 0; m < ( int )vecPackingFile.size(); m++ )
				vecPackingFile[ m ]->CloseFileSystem();
			return FALSE;
		}
	}

	int nPatchFileCount = PatchSource.GetFileCount();
	for( i = 0; i < nPatchFileCount; i++ )
	{
		bool bSkip = false;
		SPackingFileInfo *pFileInfo = PatchSource.GetFileInfo( i );
		if( pFileInfo == NULL )
		{
			continue;
		}
		for( j = 0; j < ( int )vecCopyList.size(); j++ )
		{
			// �����̸� �տ�.. \ �پ� �־ �װ� ������ �̸����� ���ؾ� ����� �ȴ�.
			if( _stricmp( vecCopyList[ j ].c_str(), pFileInfo->szFileName + 1 ) == 0 )
			{
				bSkip = true;
				break;
			}
		}
		if( bSkip )
		{
			continue;
		}
		CEtFileHandle *pFileHandle = PatchSource.OpenOnly( pFileInfo->szFileName );
		if( pFileHandle == NULL )
		{
			continue;
		}
		bool bPatch = false;
		for( j = 0; j < ( int )vecPackingFile.size(); j++ )
		{
			int nFindIndex = vecPackingFile[ j ]->FindFile( pFileInfo->szFileName );
			if( nFindIndex != -1 )
			{
				SPackingFileInfo *pFindFileInfo = vecPackingFile[ j ]->GetFileInfo( nFindIndex );
				// ���� ���� �̸��� ã������ ���ڸ��� �о� ���� �� ������ ����� �ٸ� ���� ã�ƺ���.
				if( pFindFileInfo->dwAllocSize >= pFileInfo->dwCompressSize )
				{
					vecPackingFile[ j ]->PatchFile( pFileHandle );
					bPatch = true;
				}
				else
				{
					vecPackingFile[ j ]->Remove( pFileInfo->szFileName );
				}
				break;
			}
		}
		//���� �ѵ� ������ ������ ���ҽ� �ѿ��� ���� ������ ã�´�. 
		if( bPatch == false )
		{
			for( j = 0; j < ( int )vecPackingFile.size(); j++ )
			{
				int nFindIndex = vecPackingFile[ j ]->FindSuitableEmptySpace( pFileHandle->GetFileContext()->dwCompressSize );
				if( nFindIndex != -1 )
				{
					vecPackingFile[ j ]->PatchFile( pFileHandle );
					bPatch = true;
					break;
				}
			}
		}
		// ��ġ�� ��ġ�� ã�� �������� ���� ������ ��ġ�� ã�Ƽ� �־��ش�
		if( bPatch == false )
		{
			int nBestIndex = -1;
			DWORD dwMinSize = ULONG_MAX;
			for( j = 0; j < ( int )vecPackingFile.size(); j++ )
			{
				DWORD dwPackingFileSize = vecPackingFile[ j ]->GetFileSystemSize();
				// 512�ް� ���ٴ� �۾ƾ� �о� ���� �� �ִ�
				if( ( dwPackingFileSize < dwMinSize ) && ( dwPackingFileSize < ( DWORD )DEFAULT_PACKING_FILE_SIZE ) )
				{
					dwMinSize = dwPackingFileSize;
					nBestIndex = j;
				}
			}
			if( nBestIndex == -1 )
			{
				// ��ġ�� ������ ������ ��ã������ ���ο� ��ġ������ �����.
				int nStartFileIndex = ( int )vecPackingFile.size();
				while( 1 )
				{
					CString szNewPackingFile;
					szNewPackingFile.Format( _T( "%sResource%02d.pak" ), m_szClientPath.GetBuffer(), nStartFileIndex );
					hFind = FindFirstFile( szNewPackingFile.GetBuffer() , &FindFileData );
					if( hFind == INVALID_HANDLE_VALUE )
					{
						WideCharToMultiByte( CP_ACP, 0, szNewPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
						CEtPackingFile *pNewPackingFile = new CEtPackingFile();
						pNewPackingFile->NewFileSystem( szTemp );
						vecPackingFile.push_back( pNewPackingFile );
						nBestIndex = ( int )vecPackingFile.size() - 1;
						break;
					}
					nStartFileIndex++;
					FindClose( hFind );
				}
			}
			if( nBestIndex != -1 )
			{
				vecPackingFile[ nBestIndex ]->PatchFile( pFileHandle );
			}
		}
		PatchSource.CloseFile( pFileHandle );
		m_dwNowFileSize++;
		//		OnPatch( m_dwNowFileSize, nTotalPatchCount, "" );
		if( m_bTerminateThread ) {
			for( int m = 0; m < ( int )vecPackingFile.size(); m++ )
				vecPackingFile[ m ]->CloseFileSystem();
			return FALSE;
		}
	}

	for( i = 0; i < ( int )vecPackingFile.size(); i++ )
	{
		vecPackingFile[ i ]->CloseFileSystem();
		delete vecPackingFile[ i ];
	}

	vecPackingFile.clear();


	LogWnd::TraceLog( _T( "apply pak 4"));

	//ī�� ������ �� ����Ÿ�� ��Ʈ�� ������.

	for( i = 0; i < ( int )vecCopyList.size(); i++ )
	{
		char szAddCopyListName[_MAX_PATH]={0,};
		sprintf_s(szAddCopyListName , "\\%s" , vecCopyList[ i ].c_str() );

		CEtFileHandle *pFileHandle = PatchSource.OpenFile( szAddCopyListName );
		if( pFileHandle )
		{
			char szPath[ __MAX_PATH ] = {0,};
			char szCurDir[ __MAX_PATH ] = {0,};

			int nSize1 = ( int )strlen( szPath );
			int nSize2 = ( int )strlen( vecCopyList[ i ].c_str() );

			if ( nSize1 > _MAX_PATH || nSize2 > _MAX_PATH )
			{
				LogWnd::TraceLog( _T( "nSize1 > _MAX_PATH || nSize2 > _MAX_PATH "));
			}

			_GetPath( szPath, __MAX_PATH, vecCopyList[ i ].c_str() );
			/*		
			
			if( strstr( vecCopyList[ i ].c_str(), "DnLauncher.exe" ) )
			{
				MoveFile( _T( "DnLauncher.exe" ), _T( "DnLauncher.tmp" ) );
				Sleep(100);
				DeleteFile(_T( "DnLauncher.tmp" ));
				Sleep(200);
				LogWnd::TraceLog( _T( "apply pak 4-1"));
				nRet = 2;
			}
			*/

			if( szPath[ 0 ] )
			{
				CreateFolder( szPath );
				GetCurrentDirectoryA( __MAX_PATH, szCurDir );

				char szTemp[ __MAX_PATH ] = {0,} ;
				strcpy_s( szTemp, __MAX_PATH, szCurDir );
				strcat_s( szTemp, __MAX_PATH, "\\" );
				strcat_s( szTemp, __MAX_PATH, szPath );
				SetCurrentDirectoryA( szTemp );
				LogWnd::TraceLog( _T( "apply pak 4-2"));
			}
			pFileHandle->ExportFile();
			if( szPath[ 0 ] )
			{
				SetCurrentDirectoryA( szCurDir );
			}
			PatchSource.CloseFile( pFileHandle );

			m_dwNowFileSize++;
			//OnPatch( m_dwNowFileSize, nTotalPatchCount, "" );
		}
	}

	PatchSource.CloseFileSystem();




	LogWnd::TraceLog( _T( "apply pak 5"));

	DeleteFile( m_szPatchFile.GetBuffer() );
	DeleteFile( m_szPatchListFile.GetBuffer() );
	LogWnd::TraceLog( _T( "apply pak 6"));

	// ��ġ �׷��� 100�ۼ�Ʈ �������
	m_dwNowFileSize = m_dwFileSize; 

	return TRUE;
}

BOOL CDnManualUpdaterDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_F4 ) 
	{
		OnCancel();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDnManualUpdaterDlg::OnCancel()
{
	if( !m_bCompleted )
	{
		if( AfxMessageBox( STR_NOT_YET_PATCH_CANCEL , MB_YESNO|MB_ICONWARNING) == IDYES )
		{
			KillTimer(4);
			m_bUserCancel = TRUE; 
			m_bTerminateThread = true; 

			if( m_hThread != INVALID_HANDLE_VALUE )
			{
				::WaitForSingleObject( m_hThread, INFINITE );
			}
		}
	}
	else 
	{
		KillTimer(4);
	}

	CDialog::OnCancel();

}

void CDnManualUpdaterDlg::OnOK()
{
	if( !m_bCompleted)
	{
		if( MessageBox(STR_NOT_YET_PATCH_CANCEL , STR_CONFIRM , MB_YESNO | MB_ICONWARNING) == IDYES )
		{
			KillTimer( 4 );
			m_bUserCancel = TRUE;
			m_bTerminateThread = true; 
			
			if( m_hThread == INVALID_HANDLE_VALUE );
			else
			{
				(::WaitForSingleObject( m_hThread, INFINITE ) == WAIT_OBJECT_0 );
			}
		}
	}
	else 
	{
		KillTimer(4);
	}

	CDialog::OnOK();
}

void CDnManualUpdaterDlg::OnStnClickedStateVersion()
{
	// TODO: Add your control notification handler code here
}

void CDnManualUpdaterDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDnManualUpdaterDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
