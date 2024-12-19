// DuplicateFinderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DuplicateFinder.h"
#include "DuplicateFinderDlg.h"
#include "SundriesFunc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CXTResizeDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CXTResizeDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CXTResizeDialog)
END_MESSAGE_MAP()


// CDuplicateFinderDlg dialog




CDuplicateFinderDlg::CDuplicateFinderDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDuplicateFinderDlg::IDD, pParent)
	, m_szFolder(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDuplicateFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ListCtrl);
	DDX_Text(pDX, IDC_EDIT2, m_szFolder);
}

BEGIN_MESSAGE_MAP(CDuplicateFinderDlg, CXTResizeDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CDuplicateFinderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CDuplicateFinderDlg::OnBnClickedButton1)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CDuplicateFinderDlg message handlers

BOOL CDuplicateFinderDlg::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize( IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	SetResize( IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT );
	SetResize( IDC_EDIT2, SZ_TOP_LEFT, SZ_TOP_RIGHT );
	SetResize( IDC_BUTTON1, SZ_TOP_RIGHT, SZ_TOP_RIGHT );
	SetResize( IDC_LIST2, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT );
	LoadPlacement( _T("CDuplicateFinderDlg") );
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rcRect;
	m_ListCtrl.GetWindowRect( &rcRect );
	m_ListCtrl.AddColumn( "Location", rcRect.Width() - 110 );

	m_ListCtrl.AddColumn( "Size", 90, LVCFMT_RIGHT );


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDuplicateFinderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CXTResizeDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDuplicateFinderDlg::OnPaint()
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
		CXTResizeDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDuplicateFinderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void FindFileListInDirectoryXXX( const char *szFolderName, const char *szExt, std::vector<std::string> &szVecResult, std::vector<int> &nVecFileSize, bool bIncludeFolder, bool bSearchSubDir, bool bIncludeFolderName )
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char szFindFile[512];

	sprintf_s( szFindFile, "%s\\%s", szFolderName, szExt );

	hFind = FindFirstFile( szFindFile, &FindFileData );
	while( hFind != INVALID_HANDLE_VALUE ) {
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) ) {
			if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) || bIncludeFolder == true ) {
				if( bIncludeFolderName )
				{
					char szFullName[ _MAX_PATH ];
					sprintf_s( szFullName, "%s\\%s", szFolderName, FindFileData.cFileName );
					szVecResult.push_back( szFullName );
					nVecFileSize.push_back( FindFileData.nFileSizeLow );
				}
				else
				{
					szVecResult.push_back( FindFileData.cFileName );
					nVecFileSize.push_back( FindFileData.nFileSizeLow );
				}
			}
			if( ( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && bSearchSubDir == true ) {
				if( strcmp( FindFileData.cFileName, "." ) && strcmp( FindFileData.cFileName, ".." ) ) {
					char szFolder[512] = { 0, };
					sprintf_s( szFolder, "%s\\%s", szFolderName, FindFileData.cFileName );
					FindFileListInDirectoryXXX( szFolder, szExt, szVecResult, nVecFileSize, bIncludeFolder, bSearchSubDir, bIncludeFolderName );
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;

	}

	FindClose( hFind );
}

std::string NumToDecimalStr( int nValue )
{
	char szTemp[128];
	std::string szTemp2;
	itoa( nValue, szTemp, 10 );

	szTemp2 = szTemp;
	int nOffset = 0;
	for( int k=szTemp2.size()-1; k>=0; k-- ) {
		nOffset++;
		if( nOffset % 3 == 0 && k != 0 ) {
			szTemp2.insert( szTemp2.begin() + k, ',' );
			nOffset = 0;
		}
	}
	return szTemp2;
}

int GetMin( std::vector<int> &nVecList )
{
	int nValue = INT_MAX;
	for( DWORD i=0; i<nVecList.size(); i++ ) {
		if( nVecList[i] < nValue ) nValue =  nVecList[i];
	}
	return nValue;
}

int GetMax( std::vector<int> &nVecList )
{
	int nValue = INT_MIN;
	for( DWORD i=0; i<nVecList.size(); i++ ) {
		if( nVecList[i] > nValue ) nValue =  nVecList[i];
	}
	return nValue;
}

void CDuplicateFinderDlg::OnBnClickedOk()
{
	m_ListCtrl.DeleteAllItems();
	// TODO: Add your control notification handler code here
	std::vector<std::string> szVecList;
	std::vector<int> nVecSizeList;

	struct FileInfoStruct {
		std::string szFileName;
		long lSize;
	};
	std::map<std::string,std::vector<FileInfoStruct>> szMapSearch;
	char szFileName[_MAX_PATH];
	std::vector<std::string> szVecResult;

	FindFileListInDirectoryXXX( m_szFolder.GetBuffer(), "*.*", szVecList, nVecSizeList, false, true, true );

	std::map<std::string,std::vector<FileInfoStruct>>::iterator it;
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		_GetFullFileName( szFileName, szVecList[i].c_str() );
		strlwr( szFileName );
		if( strcmp( szFileName, "." ) == NULL || strcmp( szFileName, ".." ) == NULL ) continue;
		if( strstr( szVecList[i].c_str(), "\\Grid\\" ) ) continue;
		if( strstr( szVecList[i].c_str(), "\\MapData_" ) ) continue;
		if( strstr( szVecList[i].c_str(), "\\Resource_" ) ) continue;
		it = szMapSearch.find( szFileName );
		if( it == szMapSearch.end() ) {
			std::vector<FileInfoStruct> VecTemp;
			FileInfoStruct Struct;
			Struct.szFileName = szVecList[i];
			Struct.lSize = nVecSizeList[i];
			VecTemp.push_back( Struct );
			szMapSearch.insert( make_pair( szFileName, VecTemp ) );
		}
		else {
			FileInfoStruct Struct;
			Struct.szFileName = szVecList[i];
			Struct.lSize = nVecSizeList[i];
			it->second.push_back( Struct );
		}
	}

	char szExt[128];
	std::string szTemp;
	int nCount = 0;
	int nDelCount = 0;
	int nTotalSize = 0;
	int nMinSize = 0;
	int nMaxSize = 0;
	std::vector<int> nVecSizeTemp;
	std::map<std::string, int> szMapExtList;
	std::map<std::string, int>::iterator it2;
	for( it = szMapSearch.begin(); it != szMapSearch.end(); it++ ) {
		if( it->second.size() > 1 ) {
			nVecSizeTemp.clear();
			for( DWORD i=0; i<it->second.size(); i++ ) {
				int nItemID = m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), it->second[i].szFileName.c_str() );
				szTemp = NumToDecimalStr( it->second[i].lSize );
				m_ListCtrl.SetItemText( nItemID, 1, szTemp.c_str() );
				nTotalSize += it->second[i].lSize;
				nVecSizeTemp.push_back( it->second[i].lSize );
			}
			nMinSize += GetMin( nVecSizeTemp );
			nMaxSize += GetMax( nVecSizeTemp );

			_GetExt( szExt, it->first.c_str() );
			it2 = szMapExtList.find( szExt );
			if( it2 == szMapExtList.end() ) {
				szMapExtList.insert( make_pair( szExt, 1 ) );
			}
			else {
				it2->second++;
			}

			int nItemID = m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), "---------------------------------------------------------------------------------------------------------------------------------------" );
			m_ListCtrl.SetItemText( nItemID, 1, "-------------------------------------" );
			nCount++;
			nDelCount += ( it->second.size() - 1 );
		}
	}

	char szStr[256];
	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), "-------------- 확장자 별 중복 항목 수----------------" );
	for( it2 = szMapExtList.begin(); it2 != szMapExtList.end(); it2++ ) {
		sprintf_s( szStr, "%s   :   %d 개", it2->first.c_str(), it2->second );
		m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szStr );
	}

	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), "---------------------------------------------------------------------------------------------------------------------------------------" );

	sprintf_s( szStr, "중복 파일 항목 수 : %d       삭제가 필요한 파일 수 : %d", nCount, nDelCount );
	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szStr );

	sprintf_s( szStr, "중복 파일 총 용량 : %s", NumToDecimalStr(nTotalSize).c_str() );
	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szStr );
	sprintf_s( szStr, "삭제시 줄게 될 최소 예상 용량 : %s", NumToDecimalStr(nMinSize).c_str() );
	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szStr );
	sprintf_s( szStr, "삭제시 줄게 될 최대 예상 용량 : %s", NumToDecimalStr(nMaxSize).c_str() );
	m_ListCtrl.InsertItem( m_ListCtrl.GetItemCount(), szStr );	

	m_ListCtrl.UpdateData( FALSE );

//	OnOK();
}

void CDuplicateFinderDlg::OnBnClickedButton1()
{
	if( UpdateData(TRUE) )
	{
		CXTBrowseDialog dlg;
		dlg.SetTitle(_T("Select Search Directory"));
		if (!m_szFolder.IsEmpty())
		{
			TCHAR path[_MAX_PATH];
			STRCPY_S(path, _MAX_PATH, m_szFolder);
			dlg.SetSelPath(path);
		}
		if (dlg.DoModal() == IDOK)
		{
			m_szFolder = dlg.GetSelPath();
			UpdateData(FALSE);
		}
	}
}

void CDuplicateFinderDlg::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: Add your message handler code here
	SavePlacement( _T("CDuplicateFinderDlg") );
}
