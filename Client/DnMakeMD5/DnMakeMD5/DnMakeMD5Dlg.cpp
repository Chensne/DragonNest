// DnMakeMD5Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DnMakeMD5.h"
#include "DnMakeMD5Dlg.h"
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


// CDnMakeMD5Dlg ��ȭ ����




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


// CDnMakeMD5Dlg �޽��� ó����

BOOL CDnMakeMD5Dlg::OnInitDialog()
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
	m_szText = _T("[����] ��ư�� ���� MD5 �� ������ ������ �ε� �ϰų�,\n���⿡ �巡�� ���ּ���.\n[����]��ư�� ������ �ε��� �����̸�.md5 ���� �����˴ϴ�.\nDnMakeMD5.exe [���������̸�] �� ���� Ŀ�ǵ� ��ɾ ����Ҽ� �ֽ��ϴ�. " );
	UpdateData(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDnMakeMD5Dlg::OnPaint()
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
HCURSOR CDnMakeMD5Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDnMakeMD5Dlg::OnBnClickedButtonOpen()
{
	// TODO: Add your control notification handler code here
	CFileDialog Dlg(TRUE,_T("*.*"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("����(*.*)|*.*||"),NULL);

	if ( Dlg.DoModal() == IDCANCEL )
		return;

	LoadFile(Dlg.GetPathName());


}

void CDnMakeMD5Dlg::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	if ( m_szFilePath.IsEmpty() )
	{
		AfxMessageBox(_T("������ �ε��� �ּ���."));
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
		pMsg->wParam=NULL; // ESC �� �����Ѵ�.
		break;
	case VK_RETURN:
		pMsg->wParam=NULL; // ���͵� �����Ѵ�.
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
	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);	// ������ ������ ���Ѵ�.

	if ( iCount != 1 )
	{
		AfxMessageBox(_T("�ϳ��� ���ϸ� �巡�� �ϼ���."));
		return;
	}

	TCHAR strPath[_MAX_PATH]={0,};

	DragQueryFile(hDrop,0,strPath,_MAX_PATH);

	FILE* fp = _tfopen(strPath, _T("rb"));			// ������ �ƴϸ� bFile �� NULL �ɰ��̴�

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
		m_szText.Format(_T("%s\n\n[%s] \nŬ�����忡 ���� �Ǿ����ϴ�."), str, m_szChecksum );

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
		AfxMessageBox(_T("MD5 �� ���� �Ǿ����� �ʽ��ϴ�."));
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
		m_szText.Format(_T("%s�� ���� �Ǿ����ϴ�.\n\n[%s] \n"), str, m_szChecksum );
		UpdateData(FALSE);
	}

	return true;
}