// DnMakeFullPatchDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "DnMakeFullPatch.h"
#include "DnMakeFullPatchDlg.h"
#include "MD5Checksum.h"
#include "zip.h"
#include "LogWnd.h"

#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDnPatcherDlg ��ȭ ����
BOOL PeekAndPump()
{
	static MSG msg;

	while(::PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!AfxGetApp()->PumpMessage())
		{
			::PostQuitMessage(0);
			return FALSE;
		}	
	}

	return TRUE;
}



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


// CDnMakeFullPatchDlg ��ȭ ����




CDnMakeFullPatchDlg::CDnMakeFullPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnMakeFullPatchDlg::IDD, pParent)
	, m_szEditPath(_T(""))
	, m_szStaticMsg(_T(""))
	, m_szEditOutFolder(_T(""))
{

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bSetInputFolder = false;
	m_bSetOutputFolder = false;
}

void CDnMakeFullPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_szEditPath);
	DDX_Text(pDX, IDC_EDIT_OUT_PATH, m_szEditOutFolder);
	DDX_Text(pDX, IDC_STATIC_MSG, m_szStaticMsg);
	DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_ProgressCurrent);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_ProgressTotal);
	DDX_Control(pDX, IDC_LIST_FILELIST, m_ListCtrlFileList);
	
}

BEGIN_MESSAGE_MAP(CDnMakeFullPatchDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SET_PATH, &CDnMakeFullPatchDlg::OnBnClickedButtonSetPath)
	ON_BN_CLICKED(IDC_BUTTON_BUILD, &CDnMakeFullPatchDlg::OnBnClickedButtonBuild)
	ON_BN_CLICKED(IDC_BUTTON_SET_OUT_PATH, &CDnMakeFullPatchDlg::OnBnClickedButtonSetOutPath)
END_MESSAGE_MAP()


// CDnMakeFullPatchDlg �޽��� ó����

BOOL CDnMakeFullPatchDlg::OnInitDialog()
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

	m_ListCtrlFileList.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );

	CRect rt;
	
	m_ListCtrlFileList.GetWindowRect(rt);
	m_ListCtrlFileList.InsertColumn(0, _T("�ε���"), 0, int(rt.Width()*0.07f));
	m_ListCtrlFileList.InsertColumn(1, _T("������"), 0, int(rt.Width()*0.13f));
	m_ListCtrlFileList.InsertColumn(2, _T("���"), 0, int(rt.Width()*0.80f));

	m_szEditPath = _T("�Է� ���� ���⸦ ���� �Է� ������ ������ �ּ���.");
	m_szEditOutFolder = _T("��� ���� ���⸦ ���� ��� ������ ������ �ּ���.");
	
	m_szStaticMsg = _T("������ ���� ���� �ʾҽ��ϴ�.");
	UpdateData(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CDnMakeFullPatchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDnMakeFullPatchDlg::OnPaint()
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
HCURSOR CDnMakeFullPatchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		SetWindowText(hwnd, _T("���� ����"));
		SetWindowPos(hwnd,NULL,10,10,0,0,SWP_NOSIZE | SWP_NOZORDER);
		break;
	case BFFM_VALIDATEFAILED:
		MessageBox(hwnd,_T("����Ʈ�� �Է��� ������ �������� �ʽ��ϴ�. �ٽ� �Է��� �ֽʽÿ�"),
			_T("�˸�"),MB_OK);
		return 1;
	}
	return 0;
}


BOOL BrowseFolder(HWND hWnd, TCHAR *szFolder)
{
	LPMALLOC pMalloc;
	LPITEMIDLIST pidl;
	BROWSEINFO bi;
	LPITEMIDLIST root;

	bi.hwndOwner = hWnd;

	//�� ��ǻ�͸� ��Ʈ�� �����
	SHGetSpecialFolderLocation(hWnd,CSIDL_DRIVES,&root);

	bi.pidlRoot = root;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = _T("������ �����ϼ���." );
	bi.ulFlags = BIF_EDITBOX | BIF_STATUSTEXT | BIF_VALIDATE;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = 0;

	pidl = SHBrowseForFolder(&bi);

	if (pidl == NULL) {
		return FALSE;
	}
	SHGetPathFromIDList(pidl, szFolder);

	if (SHGetMalloc(&pMalloc) != NOERROR) {
		return FALSE;
	}
	pMalloc->Free(pidl);
	pMalloc->Release();
	return TRUE;
}




void CDnMakeFullPatchDlg::OnBnClickedButtonSetPath()
{
	// TODO: Add your control notification handler code here
	TCHAR szPath[1024] = {0, } ;

	ZeroMemory( szPath,sizeof(szPath));
	if ( BrowseFolder(GetSafeHwnd(), szPath) == TRUE) 	
	{
		m_szEditPath = szPath;	
		UpdateData(FALSE);
		DropFolder(szPath);
		_UpdateFileList();

		m_bSetInputFolder = true;
	}

}



void CDnMakeFullPatchDlg::OnBnClickedButtonSetOutPath()
{
	// TODO: Add your control notification handler code here
	TCHAR szPath[1024] = {0, } ;

	ZeroMemory( szPath,sizeof(szPath));
	if ( BrowseFolder(GetSafeHwnd(), szPath) == TRUE) 	
	{
		m_szEditOutFolder = szPath;	
		m_szStaticMsg = _T("���� ��ư�� ���� ��ġ�� �����ϼ���.");
		UpdateData(FALSE);
		m_bSetOutputFolder = true;
	}

}

void CDnMakeFullPatchDlg::OnBnClickedButtonBuild()
{
	// TODO: Add your control notification handler code here
	if ( m_bSetOutputFolder && m_bSetOutputFolder )
	{
		Build();
	}
	else
	{
		AfxMessageBox(_T("��/��� ������ ������ �ּ���"));
	}
}

BOOL CDnMakeFullPatchDlg::PreTranslateMessage(MSG* pMsg)
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
/*
	switch(pMsg->message)
	{
	case WM_DROPFILES:
		DropFile((HDROP)pMsg->wParam);

		break;
	}
*/

	return CDialog::PreTranslateMessage(pMsg);
}

void 
CDnMakeFullPatchDlg::DropFile(HDROP hDrop)
{
	m_FileList.clear();

	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);	// ������ ������ ���Ѵ�.

	TCHAR strPath[_MAX_PATH]={0,};
	if ( iCount < 0 )
		return ;

	if ( iCount != 1 )
	{
		AfxMessageBox(_T("�ϳ��� ������ �巡�� ���ּ���."));
		return ;
	}
	

	for ( int i = 0 ; i < iCount ; i++ )
	{
		DragQueryFile(hDrop,i,strPath,_MAX_PATH);

		HANDLE	hFile = INVALID_HANDLE_VALUE;
		hFile = CreateFile(strPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL );

		// ������ ���
		if ( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle(hFile);
			AfxMessageBox(_T("�ϳ��� ������ �巡�� ���ּ���."));
			return ;
		}
		// ���� �ϰ�� 
		else
		{
			m_szEditPath = strPath;
			UpdateData(FALSE);
			DropFolder(strPath);
		}

	}

	m_szStaticMsg = _T("���� ��ư�� ���� ��ġ�� ���� �ϼ���.");
	UpdateData(FALSE);
	_UpdateFileList();
}

void
CDnMakeFullPatchDlg::DropFolder(const TCHAR* szPath)
{
	WIN32_FIND_DATA fd;
	HANDLE			hFD;

	LogWnd::TraceLog("DropFolder %s", szPath);
	hFD = FindFirstFile(szPath, &fd);
	if (hFD != INVALID_HANDLE_VALUE)
	{
		do	
		{
			if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
			{
				CString _szName(fd.cFileName);	
				if ( _szName != _T(".") &&  _szName != _T("..") )
				{
					TCHAR szDrive[MAX_PATH] = { 0, };
					TCHAR szDir[MAX_PATH] = { 0, };
					TCHAR szFileName[MAX_PATH] = { 0, };
					TCHAR szExt[MAX_PATH] = { 0, };

					_tsplitpath(szPath, szDrive, szDir, szFileName, szExt );
					CString szNewPath;
					szNewPath.Format(_T("%s%s%s\\*.*"), szDrive, szDir, _szName.GetBuffer());
					DropFolder(szNewPath.GetBuffer());

				}
			}
			else
			{
				TCHAR szDrive[MAX_PATH] = { 0, };
				TCHAR szDir[MAX_PATH] = { 0, };
				TCHAR szFileName[MAX_PATH] = { 0, };
				TCHAR szExt[MAX_PATH] = { 0, };

				_tsplitpath(szPath, szDrive, szDir, szFileName, szExt );
				CString szNewPath;
				szNewPath.Format(_T("%s%s%s"), szDrive, szDir, fd.cFileName );

				HANDLE	hFile = INVALID_HANDLE_VALUE;
				hFile = CreateFile(szNewPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,NULL );

				DWORD tmp = 0;
				DWORD nFileSize = 0;
				nFileSize = GetFileSize(hFile, &tmp);
				CloseHandle(hFile);


				FILELIST f;
				f.nFileSize = nFileSize;

				CString szTemp;
				szTemp = m_szEditPath;
				szTemp += _T("\\");
				int n = szNewPath.Find(szTemp.GetBuffer());
				if ( n >= 0 && n < szNewPath.GetLength() )
				{
					szNewPath.Delete(0, szTemp.GetLength());
				}

				strcpy(f.szFilePath, szNewPath.GetBuffer());
				LogWnd::TraceLog(_T("FileList Add %s"), szNewPath.GetBuffer());

				m_FileList.push_back(f);				

			}
		} while(FindNextFile(hFD, &fd));

		FindClose( hFD );
	}
}

void
CDnMakeFullPatchDlg::_UpdateFileList()
{
	m_ListCtrlFileList.DeleteAllItems();

	for ( int i = 0 ; i < (int)m_FileList.size() ; i++ )
	{
		CString szTemp;
		szTemp.Format(_T("%d"), i);
		FILELIST* pF = (FILELIST*)&(m_FileList[i]);
		m_ListCtrlFileList.InsertItem(i, szTemp.GetBuffer());
		szTemp.Format(_T("%d"), pF->nFileSize);

		TCHAR text[512] = {0,};
		LV_ITEM lvitem;
		strcpy(text, szTemp.GetBuffer());
		lvitem.pszText = text;
		lvitem.mask = LVIF_TEXT;
		lvitem.iSubItem = 1;
		lvitem.iItem = i;
		m_ListCtrlFileList.SetItem(&lvitem);

		
		strcpy(text, pF->szFilePath);
		lvitem.pszText = text;
		lvitem.mask = LVIF_TEXT;
		lvitem.iSubItem = 2;
		lvitem.iItem = i;
		m_ListCtrlFileList.SetItem(&lvitem);

	}
}



// d:\sample\sample
// d:/sample/sample
bool __AccessFolder( char *sSrcFullPath_ )
{
	char spath[1024+1];
	char *ps;
	char *pd;

	ZeroMemory( spath, sizeof(spath) );
	ps = sSrcFullPath_;
	pd = spath;

	int i;	
	for (i=0; i< (int) strlen(sSrcFullPath_); ++i)
	{
		char ch = *(ps);
		if ( ch == '\\' ||  ch == '/' )
		{
			if ( strlen( ps ) > 0 && *(ps-1) != ':')
			{
				::_mkdir( spath );
				if ( ::_access( spath, 0) == -1)
					return false;
				/*
				OutputDebugString(spath);
				OutputDebugString("\n");
				*/
			}
		}
		*(pd) = ch; 
		++pd;
		++ps;
	}

	return true;
}

void
CDnMakeFullPatchDlg::Build()
{
	LogWnd::TraceLog(_T("Build Begin...."));

	if ( ::_access( m_szEditOutFolder.GetBuffer(), 6) == -1 )
	{
		LogWnd::TraceLog(_T("����!!! m_szEditOutFolder[%s] ������ �����Ҽ� ����"), m_szEditOutFolder.GetBuffer());
		return;
	}

	if ( GetSafeHwnd() )
	{
		m_szStaticMsg = _T("Ǯ��ġ�� ����� ���Դϴ�. ���� �ɸ����� ��ٷ��ּ���.. ������ �ƴ�!");
		UpdateData(FALSE);

		PeekAndPump();
		m_ProgressCurrent.SetRange(0,100);
		m_ProgressTotal.SetRange(0,(int)m_FileList.size() );
	}



	for ( int i = 0 ; i < (int)m_FileList.size() ; i++ )
	{
		if ( GetSafeHwnd() )
		{
			m_ProgressTotal.SetPos(i);
			PeekAndPump();
		}

		CString szTemp;
		szTemp.Format(_T("%d"), i);
		FILELIST* pF = (FILELIST*)&(m_FileList[i]);

		CString szPath, szOnlyPath ;
		szPath = m_szEditPath;
		szPath += _T("\\");
		szOnlyPath = szPath;
		szPath += pF->szFilePath;

		CString szChecksum = CMD5Checksum::GetMD5(szPath);

		if ( GetSafeHwnd() )
		{
			m_ProgressCurrent.SetPos(50);
			PeekAndPump();
		}

		strcpy(pF->szChecksum, szChecksum.GetBuffer());

		CString szZipPath ;
		szZipPath = m_szEditOutFolder;
		szZipPath += _T("\\");
		szZipPath += pF->szFilePath;
		szZipPath += _T(".zip");

		TCHAR szDrive[MAX_PATH] = { 0, };
		TCHAR szDir[MAX_PATH] = { 0, };
		TCHAR szFileName[MAX_PATH] = { 0, };
		TCHAR szExt[MAX_PATH] = { 0, };
		_tsplitpath(szZipPath, szDrive, szDir, szFileName, szExt );
		CString szOnlyFolder;
		szOnlyFolder.Format(_T("%s%s"), szDrive, szDir);
		LogWnd::TraceLog(_T("���� ���� [%s]"), szOnlyFolder.GetBuffer());

		if ( __AccessFolder(szOnlyFolder.GetBuffer()) == false )
		{
			LogWnd::TraceLog(_T("����!!! ���� ���� ���� [%s]"), szOnlyFolder.GetBuffer());
			return;
		}


		DeleteFile(szZipPath);
		HZIP hz;
		hz = CreateZip( szZipPath, "" );
		ZipAdd(hz, szFileName, szPath );
		CloseZip(hz);

		HANDLE	hFile = INVALID_HANDLE_VALUE;
		hFile = CreateFile(szZipPath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,NULL );

		DWORD tmp = 0;
		DWORD nFileSize = 0;
		nFileSize = GetFileSize(hFile, &tmp);
		CloseHandle(hFile);

		pF->nCompressFileSize = nFileSize;

		LogWnd::TraceLog(_T("[%s] [%s]���� �Ϸ� "), pF->szFilePath, pF->szChecksum);

		if ( GetSafeHwnd() )
		{
			m_ProgressCurrent.SetPos(100);
			PeekAndPump();
		}
	}

	_BuildChecksumFile();
	if ( GetSafeHwnd() )
	{
		m_ProgressTotal.SetPos((int)m_FileList.size()+1);
		AfxMessageBox(_T("�Ϸ�Ǿ����ϴ�."));
	}
	LogWnd::TraceLog(_T("�Ϸ�Ǿ����ϴ� "));
}


void CDnMakeFullPatchDlg::_BuildChecksumFile()
{
	LogWnd::TraceLog(_T("_BuildChecksumFile ����"));
	TCHAR sfile[ 1024+1 ];

	if ( m_FileList.size() <= 0)
		return;

	_stprintf( sfile,_T("%s\\fullpatch_svr.cfg"), m_szEditOutFolder.GetBuffer() );

	FILE *fw;
	if (( fw =fopen(sfile,"w+")) != NULL )
	{
		for ( int i = 0 ; i < (int)m_FileList.size() ; i++ )
		{
			FILELIST* pF = (FILELIST*)&(m_FileList[i]);
			fprintf(fw,"%10ld|%10ld|%-32s|%s\n",pF->nFileSize,pF->nCompressFileSize, pF->szChecksum, pF->szFilePath);

			LogWnd::TraceLog(_T("%10ld|%10ld|%-32s|%s\n"),pF->nFileSize,pF->nCompressFileSize, pF->szChecksum, pF->szFilePath);
		}
		fclose(fw);
	}

	// ������� MD5::üũ�� ����
	CString sAccess;
	CString sCheckSum;

	sCheckSum = CMD5Checksum::GetMD5( sfile );

	// MD5 üũ������
	sprintf( sfile,"%s\\fullpatch_svr.md5", m_szEditOutFolder.GetBuffer());
	if ((fw = fopen( sfile,"w+")) != NULL)
	{
		fprintf(fw,"%s\n",sCheckSum.GetBuffer());
		fclose(fw);
	}

	LogWnd::TraceLog(_T("_BuildChecksumFile �Ϸ�"));
}