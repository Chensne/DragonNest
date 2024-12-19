// MakeManualPatchDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MakeManualPatch.h"
#include "MakeManualPatchDlg.h"
#include "DnAttachFile.h"
#include "MD5Checksum.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CMakeManualPatchDlg ��ȭ ����




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


// CMakeManualPatchDlg �޽��� ó����

BOOL CMakeManualPatchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMakeManualPatchDlg::OnPaint()
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

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
		MessageBox( "������ ���� �� �����ϴ�." );
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
		MessageBox( "�Է� ������ �ùٸ��� �ʽ��ϴ�." );
		return;
	}

	UpdateData( TRUE );
	m_nManualPatchStartVersion = GetDlgItemInt( IDC_EDIT_START_VER );
	m_nManualPatchEndVersion = GetDlgItemInt( IDC_EDIT_END_VER );

	if( m_nManualPatchStartVersion == 0 || m_nManualPatchEndVersion == 0 )
	{
		MessageBox( "���� ������ �ùٸ��� �ʽ��ϴ�." );
		return;
	}

	if( m_nManualPatchStartVersion >= m_nManualPatchEndVersion )
	{
		MessageBox( "���� ������ �ùٸ��� �ʽ��ϴ�." );
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
		MessageBox( "���� ������ġ �б� ����" );
		SAFE_DELETE( pDnAttachFile );
		return;
	}

	//���������� ó�� �����Ѵ�. 
	pDnAttachFile->WriteVersion( m_nManualPatchStartVersion, m_nManualPatchEndVersion );

	_GetPath( szPath, m_strTxtFilePath.GetBuffer() );
	_GetFullFileName( szFileName, m_strTxtFilePath.GetBuffer() );
	if( !pDnAttachFile->AttachFile( szPath, szFileName ) )
	{
		MessageBox( "��ġ���� ���� ����" );
		SAFE_DELETE( pDnAttachFile );
		return; 
	}

	_GetPath( szPath, m_strPakFilePath.GetBuffer() );
	_GetFullFileName( szFileName, m_strPakFilePath.GetBuffer() );
	if( !pDnAttachFile->AttachFile( szPath, szFileName ) )
	{
		MessageBox( "��ġ���� ���� ����" );
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
