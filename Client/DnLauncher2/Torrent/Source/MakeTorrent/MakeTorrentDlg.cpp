// MakeTorrentDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MakeTorrent.h"
#include "MakeTorrentDlg.h"
#include "SHFolderDialog.h"


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


// CMakeTorrentDlg 대화 상자




CMakeTorrentDlg::CMakeTorrentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMakeTorrentDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMakeTorrentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMakeTorrentDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_FILE, &CMakeTorrentDlg::OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_PATH, &CMakeTorrentDlg::OnBnClickedButtonPath)
	ON_BN_CLICKED(IDC_BUTTON_MAKE, &CMakeTorrentDlg::OnBnClickedButtonMake)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CMakeTorrentDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CMakeTorrentDlg 메시지 처리기

BOOL CMakeTorrentDlg::OnInitDialog()
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

void CMakeTorrentDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMakeTorrentDlg::OnPaint()
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
HCURSOR CMakeTorrentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMakeTorrentDlg::OnBnClickedButtonFile()
{
	CString strFilter = L"Select Files (*.*)|*.*||";
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, strFilter );
	if( IDOK == dlg.DoModal() )
	{
		m_strFileName = dlg.GetPathName();
		GetDlgItem( IDC_STATIC_FILE )->SetWindowText( m_strFileName );
	}
}

void CMakeTorrentDlg::OnBnClickedButtonPath()
{
	if( CSHFolderDialog::BrowseForFolder( m_strFileName, _T("Choose a Folder:"), m_hWnd, 0, 0, NULL ) == IDOK )
	{
		GetDlgItem( IDC_STATIC_FILE )->SetWindowText( m_strFileName );
	}
}

void CMakeTorrentDlg::OnBnClickedButtonMake()
{
	if( m_strFileName.GetLength() == 0 )
		return;

	CString strWebSeeds, strTrackers;
	GetDlgItem( IDC_EDIT_WEBSEEDS )->GetWindowText( strWebSeeds );
	GetDlgItem( IDC_EDIT_TRACKERS )->GetWindowText( strTrackers );

	BOOL bMake = MakeTorrent( m_strFileName, strWebSeeds, strTrackers );

	if( bMake )
	{
		AfxMessageBox( L"Success!" );
	}
	else
	{
		AfxMessageBox( L"Fail!" );
	}
}

void CMakeTorrentDlg::OnBnClickedButtonReset()
{
	m_strFileName = L"";
	GetDlgItem( IDC_STATIC_FILE )->SetWindowText( m_strFileName );
	GetDlgItem( IDC_EDIT_WEBSEEDS )->SetWindowText( L"" );
	GetDlgItem( IDC_EDIT_TRACKERS )->SetWindowText( L"" );
}
