#include "stdafx.h"
#include "NxFileSystemTool.h"
#include "NxFileSystemToolDlg.h"
#include "NewFolderDlg.h"
#include "ExportBrowseFolder.h"
#include "WorkDialog.h"
#include "BuildDivisionPacking.h"
#include "BuildDivisionPackingCapacity.h"
#include "LogWnd.h"
#include "SundriesFunc.h"
#include "SHFolderDialog.h"

#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable:4311)

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
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



// CNxFileSystemToolDlg 대화 상자


CNxFileSystemToolDlg::CNxFileSystemToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNxFileSystemToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iOverwriteType = -1;
}

void 
CNxFileSystemToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FILE_LIST, m_FileList);
	DDX_Control(pDX, IDC_NAVI , m_Edit);
}

BEGIN_MESSAGE_MAP(CNxFileSystemToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_COMMAND(ID_LOADSYSTEM, OnLoadsystem)
	ON_COMMAND(ID_NEWSYSTEM, OnNewsystem)
	ON_COMMAND(ID_FILESYSTEM_CLOSE, OnFilesystemClose)
	ON_COMMAND(ID_INSERT_FILE, OnInsertFile)
	ON_MESSAGE(WM_COMPLETE_MSG, OnCompleteMsg)
	ON_MESSAGE(WM_COMPLETE_OPTIMIZE, OnComplete_Optimize)
	ON_MESSAGE(WM_COMPLETE_EXPORT, OnComplete_Export)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_COMMAND(ID_REMOVE, OnRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnNMDblclkFileList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FILE_LIST, OnLvnEndlabeleditFileList)
	ON_COMMAND(ID_IPAC_EXIT, OnIpacExit)
	ON_COMMAND(ID_OPTIMIZE, OnOptimize)
	ON_COMMAND(ID_RENAME, OnRename)
	ON_COMMAND(ID_EXPORT, OnExport)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_COMMAND(ID_MENU_DIV_PACKING, &CNxFileSystemToolDlg::OnMenuDivPacking)
	ON_COMMAND(ID_MENU_DIV_PACKING_CAPACITY, &CNxFileSystemToolDlg::OnMenuDivPackingCapacity)
	ON_COMMAND(ID_PATCH, &CNxFileSystemToolDlg::OnPatch)
	ON_MESSAGE(WM_COMPLETE_PATCH, OnCompletePatch)
	ON_COMMAND(ID_COMPARE, &CNxFileSystemToolDlg::OnCompare)
	ON_COMMAND(ID_INSERT_FOLDER, &CNxFileSystemToolDlg::OnInsertFolder)
END_MESSAGE_MAP()


// CNxFileSystemToolDlg 메시지 처리기


BOOL 
CNxFileSystemToolDlg::OnInitDialog()
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
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_FileList.DragAcceptFiles();
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	RECT Rect;
	m_FileList.GetWindowRect(&Rect);

	const int COLUMN_WIDTH_GAB = 4;
	const int SIZECOLUMN_WIDTH = 300;
	const int SIZECOLUMN_WIDTH2 = 80;

	m_FileList.InsertColumn(0,_T("이름"),LVCFMT_LEFT,SIZECOLUMN_WIDTH);
	m_FileList.InsertColumn(1,_T("크기"),LVCFMT_LEFT,SIZECOLUMN_WIDTH2);
	m_FileList.InsertColumn(2,_T("원본크기"),LVCFMT_LEFT,SIZECOLUMN_WIDTH2);
	m_FileList.InsertColumn(3,_T("설명"),LVCFMT_LEFT,SIZECOLUMN_WIDTH);

	m_ImageList.Create(IDB_FILE_LIST_ICON,16,2,RGB(0,255,0));
	m_FileList.SetImageList(&m_ImageList,LVSIL_SMALL);
	
	m_bWorking = false;	// 작업중인지 아닌지.. 초기화

	m_FileList.SetFocus();

//	LogWnd::CreateLog();


	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void
CNxFileSystemToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void 
CNxFileSystemToolDlg::OnPaint() 
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
HCURSOR
CNxFileSystemToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL
CNxFileSystemToolDlg::PreTranslateMessage(MSG* pMsg)
{

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
		if(pMsg->hwnd == m_FileList.GetSafeHwnd()) 
			DropFile((HDROP)pMsg->wParam);
		break;
	case WM_KEYDOWN:
		if(pMsg->hwnd == m_FileList.GetSafeHwnd() && pMsg->wParam == VK_DELETE) 
			OnRemove();
		break;
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void 
CNxFileSystemToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	const int SIZECOLUMN_WIDTH = 300;
	const int SIZECOLUMN_WIDTH2 = 60;

	CRect rt;
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_FileList.GetSafeHwnd() != NULL )
	{
		m_FileList.SetWindowPos(NULL,10,35,cx-22,cy-48,SWP_NOZORDER);
		//Client 영역의 크기를 구한다. 
		m_FileList.GetClientRect(&rt);

	}
}

static int
CALLBACK FileListItemSortFunc(LPARAM lParam1,LPARAM lParam2,LPARAM /*lParamSort*/)
{
	SNxFileInfo* pFH1 = (SNxFileInfo*)lParam1;
	SNxFileInfo* pFH2 = (SNxFileInfo*)lParam2;

	CString strItem1;
	if(pFH1->nFileType == NFS_DIR)
	{
		strItem1.Format(_T("!%s"),pFH1->szFileName);
	}
	else
	{
		strItem1 = pFH1->szFileName;
	}

	CString strItem2;
	if(pFH2->nFileType == NFS_DIR)
	{
		strItem2.Format(_T("!%s"),pFH2->szFileName);
	}
	else
	{
		strItem2 = pFH2->szFileName;
	}

	return _tcscmp(strItem1,strItem2);
}
void
CNxFileSystemToolDlg::UpdateFileList()
{
	m_FileList.DeleteAllItems();

	int i;
	CString strFileSize;
	CString OriFileSize;

	std::vector< SPackingFileInfo * > vecFileInfo;
	std::vector< std::string > vecDirList;
		 
	g_FileSystem.GetFileInfoListCurDir( vecFileInfo );
	g_FileSystem.GetDirListCurDir( vecDirList );
	m_vecFileInfo.resize( vecFileInfo.size() + vecDirList.size() );
	for( i = 0; i < ( int )vecFileInfo.size(); i++ )
	{
		_GetFullFileName( m_vecFileInfo[ i ].szFileName, _countof(m_vecFileInfo[ i ].szFileName), vecFileInfo[ i ]->szFileName );
		m_vecFileInfo[ i ].dwCompressSize = vecFileInfo[ i ]->dwCompressSize;
		m_vecFileInfo[ i ].dwOriginalSize = vecFileInfo[ i ]->dwOriginalSize;
		m_vecFileInfo[ i ].nFileType = NFS_FILE;
	}
	for( i = 0; i < ( int )vecDirList.size(); i++ )
	{
		strcpy_s( m_vecFileInfo[ vecFileInfo.size() + i ].szFileName, _MAX_PATH, vecDirList[ i ].c_str() );
		m_vecFileInfo[ vecFileInfo.size() + i ].nFileType = NFS_DIR;
	}
	for( i = 0; i < ( int )m_vecFileInfo.size(); i++ )
	{
		m_FileList.InsertItem( i, m_vecFileInfo[ i ].szFileName, 1 );
		m_FileList.SetItemData( i, ( DWORD )( &m_vecFileInfo[ i ] ) );

		if( m_vecFileInfo[ i ].nFileType == NFS_FILE )
		{
			strFileSize.Format(_T("%dKB"),( m_vecFileInfo[ i ].dwCompressSize + 999 ) / 1000 );
			m_FileList.SetItemText(i,1,LPCTSTR(strFileSize));

			OriFileSize.Format(_T("%dKB"),( m_vecFileInfo[ i ].dwOriginalSize + 999 ) / 1000 );
			m_FileList.SetItemText(i,2,LPCTSTR(OriFileSize));
		}
	}
	m_FileList.SortItems(FileListItemSortFunc,NULL);

	this->SetDlgItemText(IDC_NAVI,g_FileSystem.GetCurDir());
}


void
CNxFileSystemToolDlg::OnLoadsystem()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return ;
	}

	CFileDialog Dlg(TRUE,_T(".pak"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("파일(*.pak)|*.pak||"),NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!g_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) 
	{
		// 파일 시스템 쓰기로 안열리면 읽기로라도 연다..
		if( !g_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()), true )) 
		{
			AfxMessageBox(_T("파일시스템을 열지 못하였거나 잘못된 파일 입니다."));
			return;
		}
	}

	m_strPath = Dlg.GetPathName();

	UpdateFileList();
}


void 
CNxFileSystemToolDlg::OnNewsystem()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return ;
	}

	CFileDialog Dlg(FALSE,_T(".Nfs"),_T("NewSystem.pak"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("파일(*.pak)|*.pak||"),NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!g_FileSystem.NewFileSystem(LPCTSTR(Dlg.GetPathName()))) 
	{
		AfxMessageBox(_T("새로운 파일시스템 생성에 실패 하였습니다."));
		return;
	}
	if(!g_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) 
	{
		AfxMessageBox(_T("새파일 시스템을 생성하였지만 로드하지 못하였습니다."));
		return;
	}

	m_strPath = Dlg.GetPathName();
	UpdateFileList();
}

void 
CNxFileSystemToolDlg::OnFilesystemClose()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return ;
	}

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_FileSystem.CloseFileSystem();

	m_strPath.Empty();

	UpdateFileList();
}

BOOL 
CNxFileSystemToolDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SAFE_DELETE (g_pAddThread);

	return CDialog::DestroyWindow();
}


LRESULT 
CNxFileSystemToolDlg::OnCompleteMsg(WPARAM wParam, LPARAM lParam)
{
	SAFE_DELETE (g_pAddThread);
	m_bWorking = false;
	::PostMessage(g_pWorking->GetSafeHwnd(),WM_DESTROY,0,0);
	SAFE_DELETE(g_pWorking);
	UpdateFileList();
	return TRUE;
}

void CNxFileSystemToolDlg::OnInsertFile()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	CFileDialog Dlg(TRUE);
	if(Dlg.DoModal() == IDCANCEL) 
		return;

	TCHAR strName[_MAX_FNAME];


	g_FileSystem.GetNameOnly(strName,Dlg.GetPathName().GetBuffer());
/*	if(g_FileSystem.CheckNameExist(strName))
	{
		if(AfxMessageBox(_T("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!   "),MB_YESNO) == IDNO) return;

		g_FileSystem.Remove(strName);
	}*/

	// 안내 박스.. 띄우기..
	DrawWorking();

	AddFileInfo add;
	add.FileName = LPCTSTR( Dlg.GetPathName() );
	add.FilePath = g_FileSystem.GetCurDir();

	g_AddFileList.push_back(add);

	// 파일 추가 쓰레스 시작!
	g_pAddThread = new CFileImportThread(this->GetSafeHwnd());
	g_pAddThread->Start();
	m_bWorking = true;
}

void CNxFileSystemToolDlg::OnInsertFolder()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}

	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	CString strSelectedPath;
	if( !CSHFolderDialog::BrowseForFolder( strSelectedPath, _T("Choose a Folder:"), m_hWnd, 0, 0, NULL ) == IDOK )
	{
		return;
	}

	SearchFolderAndAddFile( strSelectedPath.GetBuffer() );

	// 안내 박스.. 띄우기..
	DrawWorking();

	// 파일 추가 쓰레스 시작!
	g_pAddThread = new CFileImportThread(this->GetSafeHwnd());
	g_pAddThread->Start();
	m_bWorking = true;
}

void CNxFileSystemToolDlg::SearchFolderAndAddFile( const char *szFolderName )
{
	char acCommonBuffer[1024];
	sprintf(acCommonBuffer, szFolderName);
	strcat(acCommonBuffer, "/*.*");

	HANDLE hSearchFile;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	// 폴더안에 있는 파일을 찾아 순회한다.
	hSearchFile = FindFirstFile(acCommonBuffer, &wfd);
	if (hSearchFile == INVALID_HANDLE_VALUE)
		return;

	std::string strSubFolder;
	std::string::size_type szLength;
	if( m_strRootFolder.length() == 0 )
	{
		std::string strTemp = szFolderName;
		szLength = strTemp.rfind( "\\" );
		strTemp.erase( szLength );
		m_strRootFolder = strTemp;
	}

	std::string strFolderName = szFolderName;
	strFolderName.erase( 0, m_strRootFolder.length() );
	strSubFolder = strFolderName + "\\";

	// 뭔가 찾았다면,
	while (bResult)
	{
		// 파일 순서는 항상 영어이름 순이다. 폴더와 파일 구분 없다.
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// 모든 폴더에는 항상 . 과 .. 폴더가 있다. 이건 그냥 넘기고, 다른 폴더라면,
			if (wfd.cFileName[0] != '.')
			{
				// 재귀호출로 처리한다.
				sprintf(acCommonBuffer, "%s\\%s", szFolderName, wfd.cFileName);
				SearchFolderAndAddFile(acCommonBuffer);
			}
		}
		else
		{
			if (_stricmp(wfd.cFileName, "Thumbs.db"))
			{
				// 파일명 조합
				sprintf(acCommonBuffer, "%s\\%s", szFolderName, wfd.cFileName);

				// 파일 크기가 0이 아닌 경우에만 추가한다. 파일크기 0은 의미없는 파일이다.
				HANDLE hFile = CreateFile(acCommonBuffer, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (GetFileSize(hFile, NULL) != 0)
				{
					AddFileInfo add;
					add.FileName = acCommonBuffer;
					add.FilePath = strSubFolder;

					g_AddFileList.push_back(add);
				}
				CloseHandle(hFile);
			}
		}

		bResult = FindNextFile(hSearchFile, &wfd);
	}
	FindClose(hSearchFile);
}

void CNxFileSystemToolDlg::OnNewFolder()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}

	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	CNewFolderDlg Dlg;
	if(Dlg.DoModal() == IDCANCEL) return;
	if(Dlg.m_strFolderName.IsEmpty()) return;

//	g_FileSystem.AddDirWithChange(LPCTSTR(Dlg.m_strFolderName));

	UpdateFileList();
}

void 
CNxFileSystemToolDlg::OnRemove()
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}

	
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	if(!m_FileList.GetSelectedCount()) return;

	if(AfxMessageBox(_T("선택된 파일 혹은 폴더를 삭제하시겠습니까?"), MB_YESNO) == IDYES)
	{
		CStringArray strRemove;

		POSITION Pos = m_FileList.GetFirstSelectedItemPosition();
		while(Pos)
		{
			int iItem = m_FileList.GetNextSelectedItem(Pos);
			SNxFileInfo* NxFileInfo = (SNxFileInfo*)m_FileList.GetItemData(iItem);

			strRemove.Add(NxFileInfo->szFileName);
		}

		for(int i=0;i<strRemove.GetSize();i++) g_FileSystem.Remove(strRemove[i]);

		UpdateFileList();
	}
}

void 
CNxFileSystemToolDlg::OnNMDblclkFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}


	if(m_FileList.GetSelectedCount() != 1) return;

	NMITEMACTIVATE* pNMItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if(pNMItemActivate->iItem >= 0)
	{
		SNxFileInfo *pFH = ( SNxFileInfo * )m_FileList.GetItemData(pNMItemActivate->iItem);

		if( pFH->nFileType == NFS_DIR)
		{
			g_FileSystem.ChangeDir(pFH->szFileName);

			UpdateFileList();
		}
	}

	*pResult = 0;
}

void 
CNxFileSystemToolDlg::OnLvnEndlabeleditFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}


	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if(!pDispInfo->item.pszText) return;

	SNxFileInfo *pFH = ( SNxFileInfo * )m_FileList.GetItemData(pDispInfo->item.iItem);
	if(!g_FileSystem.Rename(pFH->szFileName,pDispInfo->item.pszText))
	{
		AfxMessageBox(_T("선택된 파일 혹은 폴더의 이름을 변경할수 없습니다!"));
		return;
	}

	m_FileList.SetItemText(pDispInfo->item.iItem,0,pDispInfo->item.pszText);
	*pResult = 0;
}


void
CNxFileSystemToolDlg::DropFile(HDROP hDrop)
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}


	g_AddFileList.clear();

	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	if ( AfxMessageBox(_T("드래그한 파일및 폴더를 파일시스템에 저장하시겠습니까?") , MB_YESNO) == IDNO )
		return;

	// 안내 박스.. 띄우기..
	DrawWorking();


	/* 
	UINT DragQueryFile( HDROP hDrop, UINT iFile, LPTSTR lpszFile, UINT cch );

	iFile의 값이 0xFFFFFFFF일 경우에는 리턴값으로 몇 개의 파일이 드롭되었는지를 정수형으로 반환
	또, iFile값이 0과 드롭된 전체 파일수사이의 값이고, lpszFile이 NULL인 경우에는 null을 포함하지 
	않는 필요한 캐릭터 크기를 리턴

	hDrop	 : 드롭파일 지시자
	iFile    : 파일 쿼리에 대한 인덱스를 나타내며, 이 값에 의해 필요한 쿼리가 변합니다.
	lpszFile : 패스명을 받아드릴 버퍼(스트링형)
	cch		 : 버퍼의 크기

	*/

	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);	// 파일의 갯수를 구한다.

	TCHAR strPath[_MAX_PATH];

	for(int i=0;i<iCount;i++)
	{
		DragQueryFile(hDrop,i,strPath,_MAX_PATH);

		FILE* bFile = _tfopen(strPath, _T("rb"));			// 파일이 아니면 bFile 이 NULL 될것이다

		if(bFile)	// 즉, 파일이면 일단 닫고!
			fclose(bFile);

		TCHAR strName[_MAX_FNAME];
		g_FileSystem.GetNameOnly(strName,strPath);	// 경로명 다 빼고 파일이름만 얻어낸다.

		if(bFile)
		{
			// 같은 이름의 파일이 존재할경우..
/*			if(g_FileSystem.CheckNameExist(strName))	
			{
				if(m_iOverwriteType < 0) 
					m_iOverwriteType = AfxMessageBox(_T("동일한 이름의 파일이 이미 존재합니다.\n\n 모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n  모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!") ,MB_YESNO);
				
				if(m_iOverwriteType == IDNO) 
					continue;

				g_FileSystem.Remove(strName);
			}*/

            // 파일 추가..	
			AddFileInfo add;
			add.FileName = strPath;
			add.FilePath = g_FileSystem.GetCurDir();
			g_AddFileList.push_back(add);
			
		}
		else
		{
			if(!DropFolder(strPath))
			{
				CString strError;
				strError.Format(_T("폴더를 추가하는데 실패하였습니다.\n\n%s"),strPath);
				AfxMessageBox(strError);
			}
		}
	}

	m_iOverwriteType = -1;

	DragFinish(hDrop);
	// 파일 추가 쓰레스 시작!
	g_pAddThread = new CFileImportThread(this->GetSafeHwnd());
	g_pAddThread->Start();
	m_bWorking = true;
}

BOOL
CNxFileSystemToolDlg::DropFolder(const TCHAR* strPath)
{
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return FALSE;
	}

	char *pFindPtr = strstr( ( char * )strPath, ".svn" );
	if( pFindPtr )
	{
		if( _stricmp( pFindPtr, ".svn" ) == 0 )
		{
			return FALSE;
		}
	}
	
	TCHAR strOldPath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,strOldPath);	// 현재 이 프로그램이 실행중인 디렉토리를 알아낸다.
	SetCurrentDirectory(strPath);				// 드롭할 폴더로 이동.?? 

	TCHAR strName[_MAX_FNAME];
	g_FileSystem.GetNameOnly(strName,strPath);

/*	if(!g_FileSystem.CheckNameExist(strName))
	{
		g_FileSystem.AddDir(strName);
	}*/

	if(!g_FileSystem.ChangeDir(strName))
		return FALSE;

	CFileFind FileFind;
	if(FileFind.FindFile())
	{
		for(BOOL bNext=TRUE;bNext;)
		{
			bNext = FileFind.FindNextFile();
			CString strP = FileFind.GetFilePath();
			CString strN = FileFind.GetFileName();
			if(strN == "." || strN == "..") continue;

			if(FileFind.IsDirectory())
			{
				DropFolder(LPCTSTR(strP));
			}
			else
			{
/*				if(g_FileSystem.CheckNameExist(strN))
				{
					if(m_iOverwriteType < 0)
						m_iOverwriteType = AfxMessageBox(_T("동일한 이름의 파일이 이미 존재합니다.\n\n모든 파일을 새로운 파일로 덮어쓰시려면 '예' 를\n\n모든 기존의 파일을 보존하시려면 '아니오' 를 눌러주십시오!"),MB_YESNO);
					
					if(m_iOverwriteType == IDNO) 
						continue;

					g_FileSystem.Remove(strN);
				}*/
				
				// 파일 추가..
				AddFileInfo add;
				add.FileName = strP;
				add.FilePath = g_FileSystem.GetCurDir();
				g_AddFileList.push_back(add);
			}
		}
	}

	FileFind.Close();

	g_FileSystem.ChangeDir(_T(".."));

	SetCurrentDirectory(strOldPath);

	return TRUE;
}
void
CNxFileSystemToolDlg::OnIpacExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다.\n\n조금만 기다려주시면 안될까요..? \n\n흑흑..."));
		return;
	}

	PostQuitMessage(0);
}

void 
CNxFileSystemToolDlg::OnOptimize()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다.\n\n조금만 기다려주시면 안될까요..? \n\n흑흑..."));
		return;
	}
	
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}
	// 안내 박스.. 띄우기..
	DrawWorking();

	// 파일 추가 쓰레스 시작!
	g_pOptimizeThread = new COptimizeThread(this->GetSafeHwnd() , m_strPath);
	//OptimizeThread->Start();
	g_pOptimizeThread->Run();
	m_bWorking = true;
	
}

LRESULT		
CNxFileSystemToolDlg::OnComplete_Optimize(WPARAM wParam, LPARAM lParam)
{
	SAFE_DELETE (g_pOptimizeThread);
	g_FileSystem.ChangeDir(_T("/"));
	m_bWorking = false;
	::PostMessage(g_pWorking->GetSafeHwnd(),WM_DESTROY,0,0);
	SAFE_DELETE(g_pWorking);
	AfxMessageBox(_T("최적화 작업을 성공했습니다.\n\n오래기달렷슈~"));
	UpdateFileList();

	return TRUE;
}

LRESULT
CNxFileSystemToolDlg::OnComplete_Export(WPARAM wParam, LPARAM lParam)
{

	SAFE_DELETE (g_pExportThread);
	::PostMessage(g_pWorking->GetSafeHwnd(),WM_DESTROY,0,0);
	SAFE_DELETE(g_pWorking);
	AfxMessageBox(_T("파일 추출 작업이 완료 되었습니다."));
	UpdateFileList();
	m_bWorking = false;
	return TRUE;
}


void 
CNxFileSystemToolDlg::OnRename()
{
	;
}


using namespace std;
typedef list<SNxFileInfo>		 		CValueFileHandleList;
typedef list<SNxFileInfo>::iterator	 CValueFileHandleList_It;

void
CNxFileSystemToolDlg::OnExport()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if ( m_bWorking )
	{
		AfxMessageBox(_T("아직 작업이 진행중입니다."));
		return;
	}

	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다."));
		return;
	}

	if(!m_FileList.GetSelectedCount()) 
		return;

	if ( AfxMessageBox(_T("선택된 파일 혹은 폴더를 외부로 추출하시겠습니까?"), MB_YESNO) == IDNO)
		return;

	TCHAR strOldPath[_MAX_PATH]= { 0 , };
//	GetCurrentDirectory(_MAX_PATH,strOldPath);	// 현재 디렉토리를 알아낸다.

	if ( ExportBrowseFolder(GetSafeHwnd(),strOldPath) == FALSE )
		return;

	CString	CurPath;
	CString ExportPath;

	CurPath = strOldPath;
	ExportPath = CurPath + _T("\\Exports");

	// 선택한 경로 + Exports 폴더 만들기
	CreateDirectory(ExportPath,NULL);

	// 익스포트 할 폴더로 이동
	SetCurrentDirectory(ExportPath);	
	
	// 안내 박스.. 띄우기..
	DrawWorking();

	g_ExportList.clear();

	POSITION Pos = m_FileList.GetFirstSelectedItemPosition();

	CValueFileHandleList OriginalValue;

	while(Pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(Pos);
		SNxFileInfo *pFH = ( SNxFileInfo * )m_FileList.GetItemData(iItem);
		
		if( *(pFH->szFileName) == _T('.') )
			continue;
		
		SNxFileInfo FH = *pFH;	// Value Copy
		OriginalValue.push_back(FH);
	}

	CValueFileHandleList_It tempit=OriginalValue.begin();

	for(tempit ; tempit!=OriginalValue.end() ; ++tempit )
	{
		SNxFileInfo FH = (SNxFileInfo)(*tempit);

		ExportInfo add;
		add.FileType = FH.nFileType;
		add.Name = FH.szFileName;
		add.FilePath = g_FileSystem.GetCurDir();		
		g_ExportList.push_back(add);

		if ( FH.nFileType == NFS_DIR )
		{
			tstring name = FH.szFileName;
			FolderExport(name);
			g_FileSystem.ChangeDir(_T(".."));
		}
	}

	// 파일 로그를 남겨 보아요~
	ExportInfoList_It Export_It = g_ExportList.begin();
	//@Log.LogInfo(L"###### 타입 0 번이면 폴더 1번이면 파일 ######");
	for (Export_It ; Export_It != g_ExportList.end() ; ++Export_It )
	{
		tstring name = (*Export_It).Name;
		tstring FilePath = (*Export_It).FilePath;
		int FileType = (*Export_It).FileType;
		//@Log.LogInfo(L"이름:[%s]    ,     경로:[%s]   , 파일타입: [%d] ",name.c_str(),FilePath.c_str(),FileType);
	}

	// 목록을 다 뽑았으니 쓰레드를 시작해볼까!!
	g_pExportThread = new CFileExportThread(this->GetSafeHwnd() , ExportPath);
	g_pExportThread->Start();
	m_bWorking = true;
	
	OriginalValue.clear();

}

void CNxFileSystemToolDlg::FolderExport(tstring FolderName)
{
	if ( g_FileSystem.ChangeDir(FolderName.c_str()) == FALSE)
	{
		return;
	}
	
	std::vector< SPackingFileInfo * > vecFileInfo;
	std::vector< std::string > vecDirList;

	g_FileSystem.GetDirListCurDir( vecDirList );
	g_FileSystem.GetFileInfoListCurDir( vecFileInfo );

	int i;
	for( i = 0; i < ( int )vecFileInfo.size(); i++ )
	{
		if( vecFileInfo[ i ]->dwCompressSize > 0 )
		{
			ExportInfo add;
			add.FileType = NFS_FILE;
			add.Name = vecFileInfo[ i ]->szFileName;
			add.FilePath = g_FileSystem.GetCurDir();
			g_ExportList.push_back(add);
		}
	}

	for( i = 0; i < ( int )vecDirList.size(); i++ )
	{
		if( _stricmp( vecDirList[ i ].c_str(), "." ) == 0 )
		{
			continue;
		}
		if( _stricmp( vecDirList[ i ].c_str(), ".." ) == 0 )
		{
			continue;
		}
		FolderExport( vecDirList[ i ].c_str() );
	}
	g_FileSystem.ChangeDir( ".." );
}

void
CNxFileSystemToolDlg::OnAbout()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void 
CNxFileSystemToolDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Path;
	this->GetDlgItemText(IDC_NAVI,Path);
    if( g_FileSystem.ChangeDir((LPTSTR)(LPCTSTR)Path) )
	{
		UpdateFileList();
	}

}

void CNxFileSystemToolDlg::OnMenuDivPacking()
{
	// TODO: Add your command handler code here
	BuildDivisionPacking dlg;
	dlg.DoModal();
}

void CNxFileSystemToolDlg::OnMenuDivPackingCapacity()
{
	// TODO: Add your command handler code here
	BuildDivisionPackingCapacity dlg;
	dlg.DoModal();
}


void CNxFileSystemToolDlg::OnPatch()
{
	// TODO: Add your command handler code here
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox(_T("파일시스템이 생성되어 있지 않습니다.\n원본패킹파일을 로드해주세요."));
		return;
	}

	CFileDialog Dlg(TRUE,_T(".pak"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("파일(*.pak)|*.pak||"),NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	DrawWorking();

	// 파일 패치 쓰레드 시작!
	g_pPatchThread = new PatchThread(this->GetSafeHwnd(), std::string(LPCTSTR(Dlg.GetPathName())));
	g_pPatchThread->Start();
	m_bWorking = true;


}

LRESULT CNxFileSystemToolDlg::OnCompletePatch(WPARAM wParam, LPARAM lParam)
{
	m_bWorking = false;
	SAFE_DELETE(g_pPatchThread);
	//AfxMessageBox(_T("패치 완료!!"));
	
	if ( g_pWorking )
	{
		::PostMessage(g_pWorking->GetSafeHwnd(),WM_DESTROY,0,0);
		SAFE_DELETE(g_pWorking);
	}

	g_FileSystem.ChangeDir(_T("/"));
	UpdateFileList();

	return 0;
}
void CNxFileSystemToolDlg::OnCompare()
{
	char szFileList[ 10000 ];
	TCHAR szFilter[] = _T( "DragonNest Packing File (*.pak)|*.pak|All Files (*.*)|*.*||" );
	CFileDialog FileDlg( TRUE, _T("pak"), _T("*.pak"), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_ENABLESIZING , szFilter, this );
	memset( szFileList, 0, 10000 );
	FileDlg.GetOFN().lpstrFile = szFileList;
	FileDlg.GetOFN().nMaxFile = 10000;
	FileDlg.m_ofn.lpstrTitle = "Open Source File";

	if( FileDlg.DoModal() != IDOK )
	{
		return;
	}

	std::vector< std::string > vecSour, vecDest;
	POSITION Pos = FileDlg.GetStartPosition();
	while( Pos )
	{
		CString szFileName = FileDlg.GetNextPathName( Pos );
		vecSour.push_back( szFileName.GetBuffer() );
	}

	FileDlg.m_ofn.lpstrTitle = "Open Destination File";
	if( FileDlg.DoModal() != IDOK )
	{
		return;
	}

	Pos = FileDlg.GetStartPosition();
	while( Pos )
	{
		CString szFileName = FileDlg.GetNextPathName( Pos );
		vecDest.push_back( szFileName.GetBuffer() );
	}

	std::vector< SPackingFileInfo > vecSourFileInfo, vecDestFileInfo;
	int i, j;
	for( i = 0; i < ( int )vecSour.size(); i++ )
	{
		CEtPackingFile FileSystem;
		if( !FileSystem.OpenFileSystem( vecSour[ i ].c_str(), true ) )
		{
			continue;
		}
		int nFileCount = FileSystem.GetFileCount();
		for( j = 0; j < nFileCount; j++ )
		{
			SPackingFileInfo *pFileInfo = FileSystem.GetFileInfo( j );
			if( pFileInfo->dwCompressSize == 0 )
			{
				continue;
			}
			vecSourFileInfo.push_back( *pFileInfo );
		}
		FileSystem.CloseFileSystem();
	}

	for( i = 0; i < ( int )vecDest.size(); i++ )
	{
		CEtPackingFile FileSystem;
		if( !FileSystem.OpenFileSystem( vecDest[ i ].c_str(), true ) )
		{
			continue;
		}
		int nFileCount = FileSystem.GetFileCount();
		for( j = 0; j < nFileCount; j++ )
		{
			SPackingFileInfo *pFileInfo = FileSystem.GetFileInfo( j );
			if( pFileInfo->dwCompressSize == 0 )
			{
				continue;
			}
			vecDestFileInfo.push_back( *pFileInfo );
		}
		FileSystem.CloseFileSystem();
	}

	std::vector< char > vecCheckField;
	std::vector< SPackingFileInfo > vecSourExist, vecDestExist, vecDiff;

	vecCheckField.resize( vecDestFileInfo.size() );
	memset( &vecCheckField[ 0 ], 0, vecCheckField.size() );
	for( i = 0; i < ( int )vecSourFileInfo.size(); i++ )
	{
		for( j = 0; j < ( int )vecDestFileInfo.size(); j++ )
		{
			if( _stricmp( vecSourFileInfo[ i ].szFileName, vecDestFileInfo[ j ].szFileName ) == 0 )
			{
				vecCheckField[ j ] = 1;
				if( vecSourFileInfo[ i ].dwChecksum != vecDestFileInfo[ j ].dwChecksum )
				{
					vecDiff.push_back( vecSourFileInfo[ i ] );
				}
				break;
			}
		}
		if( j == ( int )vecDestFileInfo.size() )
		{
			vecSourExist.push_back( vecSourFileInfo[ i ] );
		}
	}
	for( i = 0; i < ( int )vecCheckField.size(); i++ )
	{
		if( vecCheckField[ i ] == 0 )
		{
			vecDestExist.push_back( vecDestFileInfo[ i ] );
		}
	}

	if( vecDestExist.size() + vecSourExist.size() + vecDiff.size() == 0 )
	{
		MessageBox( "File Is Identical" );
		return;
	}

	MessageBox( "File Is Different" );
	return;
}

