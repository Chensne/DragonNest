// MakeManualPatchDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MakeManualPatch.h"
#include "MakeManualPatchDlg.h"
#include "DnAttachFile.h"
#include "MD5Checksum.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMakeManualPatchDlg 대화 상자




CMakeManualPatchDlg::CMakeManualPatchDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMakeManualPatchDlg::IDD, pParent)
, m_nManualPatchStartVersion( 0 )
, m_nManualPatchEndVersion( 0 )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeManualPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeManualPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PAK, &CMakeManualPatchDlg::OnBnClickedButtonSelectPak)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_TXT, &CMakeManualPatchDlg::OnBnClickedButtonSelectTxt)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_EXE, &CMakeManualPatchDlg::OnBnClickedButtonSelectExe)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_MD5, &CMakeManualPatchDlg::OnBnClickedButtonMakeMd5)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_MANUALPATCH, &CMakeManualPatchDlg::OnBnClickedButtonMakeManualpatch)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMakeManualPatchDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CMakeManualPatchDlg 메시지 처리기

BOOL CMakeManualPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMakeManualPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMakeManualPatchDlg::OnPaint()
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
HCURSOR CMakeManualPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMakeManualPatchDlg::OnBnClickedButtonSelectPak()
{
	CFileDialog dlg( TRUE,  NULL, NULL, NULL, "pak File(*.pak)|*.pak||", NULL );

	if( dlg.DoModal() == IDOK )
	{
		m_strPakFilePath = dlg.GetPathName();
		GetDlgItem( IDC_EDIT_SELECT_PAK )->SetWindowText( m_strPakFilePath );
	}
}

void CMakeManualPatchDlg::OnBnClickedButtonSelectTxt()
{
	CFileDialog dlg( TRUE,  NULL, NULL, NULL, "txt File(*.txt)|*.txt||", NULL );

	if( dlg.DoModal() == IDOK )
	{
		m_strTxtFilePath = dlg.GetPathName();
		GetDlgItem( IDC_EDIT_SELECT_TXT )->SetWindowText( m_strTxtFilePath );
	}
}

void CMakeManualPatchDlg::OnBnClickedButtonSelectExe()
{
	CFileDialog dlg( TRUE,  NULL, NULL, NULL, "exe File(*.exe)|*.exe||", NULL );

	if( dlg.DoModal() == IDOK )
	{
		m_strExeFilePath = dlg.GetPathName();
		GetDlgItem( IDC_EDIT_SELECT_EXE )->SetWindowText( m_strExeFilePath );
	}
}

const char* _GetPath(char* pBuffer, const char* pPath)
{
	strcpy_s( pBuffer, (int)strlen(pPath) + 1, pPath );
	int i = (int ) strlen(pPath) - 1;

	while (i >= 0) {
		if (pPath[i] == '\\' || pPath[i] == '/') {
			pBuffer[i + 1] = 0;
			return pBuffer;
		}
		i--;
	}

	pBuffer[ 0 ] = 0;

	return pBuffer;
}

const char *_GetFullFileName( char *pBuffer, const char *pString )
{
	int i = (int ) strlen(pString) - 1;

	int nSize = i;
	while (i >= 0) {
		if ( pString[i] == '\\' || pString[i] == '/') {
			memcpy( pBuffer, pString + i + 1, nSize - i );
			pBuffer[nSize - i] = 0;
			return pBuffer;
		}

		i--;
	}
	strcpy_s( pBuffer, (int)strlen(pString)+1, pString );

	return pBuffer;
}

void CMakeManualPatchDlg::OnBnClickedButtonMakeMd5()
{
	if( m_strPakFilePath.GetLength() == 0 )
		return;

	CString strChecksum = CMD5Checksum::GetMD5( m_strPakFilePath );

	FILE *stream = NULL;
	CString strMD5FileName = m_strPakFilePath + ".MD5";

	if( fopen_s( &stream, strMD5FileName.GetString() , "w+" ) != 0 )
	{
		MessageBox( "파일을 만들 수 없습니다." );
		return;
	}

	fseek( stream, 0L, SEEK_SET );
	fprintf( stream, strChecksum.GetString() );
	fprintf( stream, "\n" );
	fclose( stream );

	AfxMessageBox( "Make MD5 Complete!", MB_OK );
}

void CMakeManualPatchDlg::OnBnClickedButtonMakeManualpatch()
{
	if( m_strExeFilePath.GetLength() == 0 || m_strPakFilePath.GetLength() == 0 || m_strTxtFilePath.GetLength() == 0 )
	{
		MessageBox( "입력 정보가 올바르지 않습니다." );
		return;
	}

	UpdateData( TRUE );
	m_nManualPatchStartVersion = GetDlgItemInt( IDC_EDIT_START_VER );
	m_nManualPatchEndVersion = GetDlgItemInt( IDC_EDIT_END_VER );

	if( m_nManualPatchStartVersion == 0 || m_nManualPatchEndVersion == 0 )
	{
		MessageBox( "버전 정보가 올바르지 않습니다." );
		return;
	}

	if( m_nManualPatchStartVersion >= m_nManualPatchEndVersion )
	{
		MessageBox( "버전 정보가 올바르지 않습니다." );
		return;
	}

	USES_CONVERSION;

	CDnAttachFile* pDnAttachFile = new CDnAttachFile;

	char szPath[ _MAX_PATH ];
	char szFileName[ _MAX_FNAME ];
	_GetPath( szPath, m_strPakFilePath.GetBuffer() );

	CString szPatchExecutePath;
	szPatchExecutePath.Format( "%sDNUpdater_%dto%d.exe", szPath, m_nManualPatchStartVersion, m_nManualPatchEndVersion );

	if( !pDnAttachFile->Create( m_strExeFilePath.GetBuffer(), szPatchExecutePath.GetBuffer() ) )
	{
		MessageBox( "원본 수동패치 읽기 실패" );
		SAFE_DELETE( pDnAttachFile );
		return;
	}

	//버젼정보를 처음 저장한다. 
	pDnAttachFile->WriteVersion( m_nManualPatchStartVersion, m_nManualPatchEndVersion );

	_GetPath( szPath, m_strTxtFilePath.GetBuffer() );
	_GetFullFileName( szFileName, m_strTxtFilePath.GetBuffer() );
	if( !pDnAttachFile->AttachFile( szPath, szFileName ) )
	{
		MessageBox( "패치파일 병합 실패" );
		SAFE_DELETE( pDnAttachFile );
		return; 
	}

	_GetPath( szPath, m_strPakFilePath.GetBuffer() );
	_GetFullFileName( szFileName, m_strPakFilePath.GetBuffer() );
	if( !pDnAttachFile->AttachFile( szPath, szFileName ) )
	{
		MessageBox( "패치파일 병합 실패" );
		SAFE_DELETE( pDnAttachFile );
		return; 
	}

	pDnAttachFile->Close();
	SAFE_DELETE( pDnAttachFile );

	AfxMessageBox( "Make ManualPatch Complete!", MB_OK );
}

void CMakeManualPatchDlg::OnBnClickedButtonClear()
{
	m_strPakFilePath = "";
	GetDlgItem( IDC_EDIT_SELECT_PAK )->SetWindowText( m_strPakFilePath );
	m_strTxtFilePath = "";
	GetDlgItem( IDC_EDIT_SELECT_TXT )->SetWindowText( m_strTxtFilePath );
	m_strExeFilePath = "";
	GetDlgItem( IDC_EDIT_SELECT_EXE )->SetWindowText( m_strExeFilePath );

	m_nManualPatchStartVersion = 0;
	m_nManualPatchEndVersion = 0;

	GetDlgItem( IDC_EDIT_START_VER )->SetWindowText( "" );
	GetDlgItem( IDC_EDIT_END_VER )->SetWindowText( "" );
}
