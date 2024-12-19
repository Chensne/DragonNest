// DnMakeMD5Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DnMakeMD5.h"
#include "DnMakeMD5Dlg.h"
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


// CDnMakeMD5Dlg 대화 상자




CDnMakeMD5Dlg::CDnMakeMD5Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnMakeMD5Dlg::IDD, pParent)
	, m_szFilePath(_T(""))
	, m_szText(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDnMakeMD5Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_szFilePath);
	DDX_Text(pDX, IDC_STATIC_MSG, m_szText);
}

BEGIN_MESSAGE_MAP(CDnMakeMD5Dlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CDnMakeMD5Dlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDnMakeMD5Dlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CDnMakeMD5Dlg 메시지 처리기

BOOL CDnMakeMD5Dlg::OnInitDialog()
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
	m_szText = _T("[열기] 버튼을 눌러 MD5 를 생성할 파일을 로드 하거나,\n여기에 드래그 해주세요.\n[저장]버튼을 누르면 로드한 파일이름.md5 으로 생성됩니다.\nDnMakeMD5.exe [생성파일이름] 과 같이 커맨드 명령어도 사용할수 있습니다. " );
	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDnMakeMD5Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDnMakeMD5Dlg::OnPaint()
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
HCURSOR CDnMakeMD5Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDnMakeMD5Dlg::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
	CFileDialog Dlg(TRUE,_T("*.*"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("파일(*.*)|*.*||"),NULL);

	if ( Dlg.DoModal() == IDCANCEL )
		return;

	LoadFile(Dlg.GetPathName());


}

void CDnMakeMD5Dlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	if ( m_szFilePath.IsEmpty() )
	{
		AfxMessageBox(_T("파일을 로드해 주세요."));
		return;
	}

	SaveFile(m_szFilePath);

}

BOOL CDnMakeMD5Dlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->wParam)
	{
	case VK_ESCAPE:
		pMsg->wParam=NULL; // ESC 는 무시한다.
		break;
	case VK_RETURN:
		pMsg->wParam=NULL; // 엔터도 무시한다.
		break;
	}

	switch(pMsg->message)
	{
	case WM_DROPFILES:
		DropFile((HDROP)pMsg->wParam);
		break;
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void
CDnMakeMD5Dlg::DropFile(HDROP hDrop)
{
	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);	// 파일의 갯수를 구한다.

	if ( iCount != 1 )
	{
		AfxMessageBox(_T("하나의 파일만 드래그 하세요."));
		return;
	}

	TCHAR strPath[_MAX_PATH]={0,};

	DragQueryFile(hDrop,0,strPath,_MAX_PATH);

	FILE* fp = _tfopen(strPath, _T("rb"));			// 파일이 아니면 bFile 이 NULL 될것이다

	if ( fp )
	{
		fclose(fp);
		LoadFile(CString(strPath));
	}
}

bool
CDnMakeMD5Dlg::LoadFile(CString& str)
{
	m_szFilePath = str;
	m_szChecksum = CMD5Checksum::GetMD5(str);

	if ( GetSafeHwnd() != NULL )
	{
		m_szText = str;
		m_szText.Format(_T("%s\n\n[%s] \n클립보드에 저장 되었습니다."), str, m_szChecksum );

		HGLOBAL hGlobal = GlobalAlloc(GHND|GMEM_SHARE, m_szChecksum.GetLength() * sizeof(TCHAR)*2);
		PSTR pGlobal = (PSTR)GlobalLock(hGlobal);
		lstrcpy(pGlobal, TEXT(m_szChecksum.GetBuffer()));
		GlobalUnlock(hGlobal);
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hGlobal);
		CloseClipboard();
		UpdateData(FALSE);
	}

	return true;
}

bool 
CDnMakeMD5Dlg::SaveFile(CString str)
{
	if ( m_szChecksum.IsEmpty() )
	{
		AfxMessageBox(_T("MD5 가 생성 되어있지 않습니다."));
		return false;
	}

	str += _T(".md5");

	FILE* fp = NULL;

	if ( (fp = fopen(str.GetBuffer(), "w+")) != NULL )
	{
		_ftprintf(fp, _T("%s"), m_szChecksum.GetBuffer());
		fclose(fp);
	}

	if ( GetSafeHwnd() != NULL )
	{
		m_szText = str;
		m_szText.Format(_T("%s이 저장 되었습니다.\n\n[%s] \n"), str, m_szChecksum );
		UpdateData(FALSE);
	}

	return true;
}