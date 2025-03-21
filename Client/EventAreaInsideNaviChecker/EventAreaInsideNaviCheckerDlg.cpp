// EventAreaInsideNaviCheckerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EventAreaInsideNaviChecker.h"
#include "EventAreaInsideNaviCheckerDlg.h"
#include "EtWorld.h"
#include <process.h>
#include "EtResourceMng.h"
#include "DNTableFile.h"
#include "EtWorldGrid.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
//#include "NavMesh.h"
#include "navigationmesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEventAreaInsideNaviCheckerDlg dialog




CEventAreaInsideNaviCheckerDlg::CEventAreaInsideNaviCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventAreaInsideNaviCheckerDlg::IDD, pParent)
	, m_szFolder(_T(""))
	, m_bCheckEventAreaInsideNavi(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bWorking = false;
}

void CEventAreaInsideNaviCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_szFolder);
	DDX_Check(pDX, IDC_CHECK1, m_bCheckEventAreaInsideNavi);
	DDX_Control(pDX, IDC_TREE1, m_TreeCtrl);
	DDX_Check(pDX, IDC_CHECK4, m_bCheckOptionMonster);
	DDX_Check(pDX, IDC_CHECK5, m_bCheckOptionMonsterGroup);
	DDX_Check(pDX, IDC_CHECK7, m_bCheckOptionNpc);
	DDX_Check(pDX, IDC_CHECK8, m_bCheckOptionEtc);
}

BEGIN_MESSAGE_MAP(CEventAreaInsideNaviCheckerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDCANCEL, &CEventAreaInsideNaviCheckerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CEventAreaInsideNaviCheckerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CEventAreaInsideNaviCheckerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CEventAreaInsideNaviCheckerDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CEventAreaInsideNaviCheckerDlg message handlers

BOOL CEventAreaInsideNaviCheckerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_BITMAP1 );

	m_ImageList.Create( 16, 16, ILC_COLOR24|ILC_MASK, 4, 1 );
	m_ImageList.Add( &bitmap, RGB(0,255,0) );
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );

	m_bCheckEventAreaInsideNavi = TRUE;
	m_bCheckOptionMonster = TRUE;
	m_bCheckOptionMonsterGroup = TRUE;
	m_bCheckOptionEtc = TRUE;
	UpdateData( FALSE );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEventAreaInsideNaviCheckerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEventAreaInsideNaviCheckerDlg::OnPaint()
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
HCURSOR CEventAreaInsideNaviCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEventAreaInsideNaviCheckerDlg::OnBnClickedCancel()
{
	/*
	// TODO: Add your control notification handler code here
	OnCancel();
	*/
}

void CEventAreaInsideNaviCheckerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CEventAreaInsideNaviCheckerDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if( UpdateData(TRUE) )
	{
		CXTBrowseDialog dlg;
		dlg.SetTitle(_T("Select Resource Directory"));
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

void CEventAreaInsideNaviCheckerDlg::EnableControl( bool bEnable )
{
	GetDlgItem( IDC_EDIT1 )->EnableWindow( bEnable );
	GetDlgItem( IDC_CHECK1 )->EnableWindow( bEnable );
	GetDlgItem( IDC_BUTTON2 )->EnableWindow( bEnable );

	GetDlgItem( IDC_CHECK4 )->EnableWindow( bEnable );
	GetDlgItem( IDC_CHECK5 )->EnableWindow( bEnable );
	GetDlgItem( IDC_CHECK7 )->EnableWindow( bEnable );
	GetDlgItem( IDC_CHECK8 )->EnableWindow( bEnable );
}

bool g_bAreaNameFilter[4] = { false, };
void CEventAreaInsideNaviCheckerDlg::OnBnClickedButton2()
{
	m_TreeCtrl.EnableMultiSelect();
	m_TreeCtrl.DeleteAllItems();
	m_TreeCtrl.SetImageList( &m_ImageList, TVSIL_NORMAL );
	UpdateData( TRUE );
	g_bAreaNameFilter[0] = m_bCheckOptionMonster;
	g_bAreaNameFilter[1] = m_bCheckOptionMonsterGroup;
	g_bAreaNameFilter[2] = m_bCheckOptionNpc;
	g_bAreaNameFilter[3] = m_bCheckOptionEtc;
	m_hThreadHandle = (HANDLE)_beginthreadex( NULL, 65536, CheckStart, (void*)this, 0, &m_dwThreadID );
}

UINT __stdcall CEventAreaInsideNaviCheckerDlg::CheckStart( void *pParam )
{
	CEventAreaInsideNaviCheckerDlg *pDlg = (CEventAreaInsideNaviCheckerDlg *)pParam;
	pDlg->m_bWorking = true;
	pDlg->EnableControl( false );
	// Resource Path
	CEtResourceMng::CreateInstance();
	CEtResourceMng::GetInstance().Initialize();
	CEtResourceMng::GetInstance().AddResourcePath( (const char*)pDlg->m_szFolder.GetBuffer(), true );

	// 체크할 Map 들 MapTable 에서 얻어오고.
	DNTableFileFormat *pMapSox = new DNTableFileFormat( CEtResourceMng::GetInstance().GetFullName( "MapTable.ext" ).c_str() );
	int nCount = pMapSox->GetItemCount();
	std::vector<std::string> szVecGridList;
	char szLabel[32];
	for( int i=0; i<nCount; i++ ) {
		int nItemID = pMapSox->GetItemID(i);
		for( int j=0; j<10; j++ ) {
			sprintf_s( szLabel, "_ToolName%d", j + 1 );
			std::string szMapName = pMapSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();
			if( !szMapName.empty() ) {
				ToLowerA( szMapName );
				if( std::find( szVecGridList.begin(), szVecGridList.end(), szMapName ) == szVecGridList.end() ) {
					szVecGridList.push_back( szMapName );
				}
			}
		}
	}
	SAFE_DELETE( pMapSox );

	// 네비메쉬에 겹치는지에 대해 체크
	DNTableFileFormat *pMonsterSox = new DNTableFileFormat( CEtResourceMng::GetInstance().GetFullName( "MonsterTable.ext" ).c_str() );
	DNTableFileFormat *pMonsterCompoundSox = new DNTableFileFormat( CEtResourceMng::GetInstance().GetFullName( "MonsterCompoundTable.ext" ).c_str() );
	DNTableFileFormat *pNpcSox = new DNTableFileFormat( CEtResourceMng::GetInstance().GetFullName( "NpcTable.ext" ).c_str() );

	CEtWorld *pWorld = new CEtWorld;
	char szWorldPath[256];
	sprintf_s( szWorldPath, "%s\\MapData", pDlg->m_szFolder.GetBuffer() );
	for( DWORD i=0; i<szVecGridList.size(); i++ ) {
		HTREEITEM hGrid = pDlg->m_TreeCtrl.InsertItem( szVecGridList[i].c_str(), 2, 3 );
		bool bValid = true;
		if( pWorld->Initialize( szWorldPath, szVecGridList[i].c_str() ) == false ) {
			// 에러처리 해준다.
			pDlg->m_TreeCtrl.InsertItem( "Initialize Failed", 0, 1, hGrid );
			bValid = false;
			goto _INVALID_CHECK;
		}
		if( pWorld->ForceInitialize( CEtWorldSector::LSE_Control | CEtWorldSector::LSE_Navigation ) == false ) {
			// 에러처리 해준다.
			pDlg->m_TreeCtrl.InsertItem( "ForceInitialize Failed", 0, 1, hGrid );
			bValid = false;
			goto _INVALID_CHECK;
		}
		if( pDlg->m_bCheckEventAreaInsideNavi ) {
			std::vector<std::string> szVecResult;
			if( !pDlg->IsValidEventAreaInsideNaviMesh( pWorld, szVecResult ) ) {
				for( DWORD j=0; j<szVecResult.size(); j++ ) {
					pDlg->m_TreeCtrl.InsertItem( szVecResult[j].c_str(), 0, 1, hGrid );
				}
				bValid = false;
			}
		}
_INVALID_CHECK:
		if( bValid ) {
			pDlg->m_TreeCtrl.DeleteItem( hGrid );
		}
		else {
			pDlg->m_TreeCtrl.Expand( hGrid, TVE_EXPAND );
		}
		pWorld->Destroy();
	}
	// UnitArea 에서 지정된 Monster, SetMonster, MonsterCompound, Npc 등이 실제 테이블에 존제하는지 체크

	SAFE_DELETE( pWorld );
	// Finalize
	CEtResourceMng::DestroyInstance();
	pDlg->EnableControl( true );
	pDlg->m_bWorking = false;
	return 0;
}



bool CEventAreaInsideNaviCheckerDlg::IsValidEventAreaInsideNaviMesh( CEtWorld *pWorld, std::vector<std::string> &szVecResult )
{
	bool bValid = true;
	for( DWORD i=0; i<pWorld->GetGrid()->GetActiveSectorCount(); i++ ) {
		CEtWorldSector *pSector = pWorld->GetGrid()->GetActiveSector(i);
		if( pSector->GetNavMesh() == NULL ) {
			char szStr[256];
			sprintf_s( szStr, "Can't Find Navigation Mesh - (%d - %d)", pSector->GetIndex().nX, pSector->GetIndex().nY );
			if( std::find( szVecResult.begin(), szVecResult.end(), szStr ) == szVecResult.end() )
				szVecResult.push_back( szStr );
			bValid = false;
			continue;
		}
		// UnitArea
		int nControlID[2] = { 0, 5 };

		for( int c=0; c<2; c++ ) {
			CEtWorldEventControl *pControl = pSector->GetControlFromUniqueID(nControlID[c]);
			if( pControl == NULL ) continue;
			for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) {
				CEtWorldEventArea *pArea = pControl->GetAreaFromIndex(j);
				char szAreaName[256];
				sscanf_s( pArea->GetName(), "%s", szAreaName, 256 );
				strlwr( szAreaName );
				if( ( g_bAreaNameFilter[0] && stricmp( szAreaName, "monster" ) == NULL ) ||
					( g_bAreaNameFilter[1] && stricmp( szAreaName, "monstergroup" ) == NULL ) ||
					( g_bAreaNameFilter[2] && stricmp( szAreaName, "npc" ) == NULL ) ||
					( g_bAreaNameFilter[3] && stricmp( szAreaName, "monster" ) && stricmp( szAreaName, "monstergroup" ) && stricmp( szAreaName, "npc" ) ) ) 
//				if( stricmp( szAreaName, "monster" ) == NULL || stricmp( szAreaName, "monstergroup" ) == NULL ) 
				{
					pArea->CalcOBB();
					SOBB Obb = *pArea->GetOBB();

					EtVector3 vPos[4];
					vPos[0] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[1] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[2] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[3] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );

					bool bInside = true;
					for( int k=0; k<4; k++ ) {
//						vPos[k].y = pWorld->GetHeight( vPos[k].x, vPos[k].z );
						if( pSector->GetNavMesh()->FindCell( vPos[k] ) == NULL ) {
							bInside = false;
							break;
						}
					}
					if( bInside == false ) {
						/*
						if( strcmp( pWorld->GetGrid()->GetName(), "3-2a" ) == NULL ) {
							int asdf = 0;
							pArea->CalcOBB();
							SOBB Obb = *pArea->GetOBB();

							EtVector3 vPos[4];
							vPos[0] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
							vPos[1] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );
							vPos[2] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
							vPos[3] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );

							bool bInside = true;
							for( int k=0; k<4; k++ ) {
								if( pSector->GetNavMesh()->FindCell( vPos[k] ) == NULL ) {
									bInside = false;
									break;
								}
							}

						}
						*/

						char szStr[256];
						sprintf_s( szStr, "네비밖으로 찍힌 놈 - %s", pArea->GetName() );
						szVecResult.push_back( szStr );
						bValid = false;
					}
				}
			}
		}
	}
	return bValid;
}

bool CEventAreaInsideNaviCheckerDlg::IsValidUnitAreaTableID( CEtWorld *pWorld, std::vector<std::string> &szVecResult )
{
	/*
	bool bValid = true;
	for( DWORD i=0; i<pWorld->GetGrid()->GetActiveSectorCount(); i++ ) {
		CEtWorldSector *pSector = pWorld->GetGrid()->GetActiveSector(i);
		if( pSector->GetNavMesh() == NULL ) {
			char szStr[256];
			sprintf_s( szStr, "Can't Find Navigation Mesh - (%d - %d)", pSector->GetIndex().nX, pSector->GetIndex().nY );
			if( std::find( szVecResult.begin(), szVecResult.end(), szStr ) == szVecResult.end() )
				szVecResult.push_back( szStr );
			bValid = false;
			continue;
		}
		// UnitArea
		int nControlID[2] = { 0, 5 };

		for( int c=0; c<2; c++ ) {
			CEtWorldEventControl *pControl = pSector->GetControlFromUniqueID(nControlID[c]);
			if( pControl == NULL ) continue;
			for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) {
				CEtWorldEventArea *pArea = pControl->GetAreaFromIndex(j);
				char szAreaName[256];
				sscanf_s( pArea->GetName(), "%s", szAreaName, 256 );
				strlwr( szAreaName );
				if( stricmp( szAreaName, "monster" ) == NULL || stricmp( szAreaName, "monstergroup" ) == NULL ) {

					pArea->CalcOBB();
					SOBB Obb = *pArea->GetOBB();

					EtVector3 vPos[4];
					vPos[0] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[1] = Obb.Center + ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[2] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) + ( Obb.Axis[2] * Obb.Extent[2] );
					vPos[3] = Obb.Center - ( Obb.Axis[0] * Obb.Extent[0] ) - ( Obb.Axis[2] * Obb.Extent[2] );

					bool bInside = true;
					for( int k=0; k<4; k++ ) {
						if( pSector->GetNavMesh()->FindCell( vPos[k] ) == NULL ) {
							bInside = false;
							break;
						}
					}
					if( bInside == false ) {
						char szStr[256];
						sprintf_s( szStr, "네비밖으로 찍힌 놈 - %s", pArea->GetName() );
						szVecResult.push_back( szStr );
						bValid = false;
					}
				}
			}
		}
	}
	return bValid;
	*/
	return true;
}
