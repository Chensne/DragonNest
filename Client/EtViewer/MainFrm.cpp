// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "EtViewer.h"

#include "MainFrm.h"
#include "PaneDefine.h"
#include "InputDevice.h"
#include "GlobalValue.h"
#include "PropertiesDlg.h"
#include "EtResourceMng.h"
#include "BonePropDlg.h"
#include "LinkObjectDlg.h"
#include "RenderBase.h"
#include "UserMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CInputDevice g_InputDevice;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_PANE_WORKSPACEPANE, &CMainFrame::OnUpdatePaneWorkspacepane)
	ON_UPDATE_COMMAND_UI(ID_PANE_PROPERTIESPANE, &CMainFrame::OnUpdatePanePropertiespane)
	ON_UPDATE_COMMAND_UI(ID_PANE_ANIMATIONLISTPANE, &CMainFrame::OnUpdatePaneAnimationlistpane)
	ON_UPDATE_COMMAND_UI(ID_BONEPROPERTIES, &CMainFrame::OnUpdateBoneProperties)
	ON_COMMAND(ID_PANE_WORKSPACEPANE, &CMainFrame::OnPaneWorkspacepane)
	ON_COMMAND(ID_PANE_PROPERTIESPANE, &CMainFrame::OnPanePropertiespane)
	ON_COMMAND(ID_PANE_ANIMATIONLISTPANE, &CMainFrame::OnPaneAnimationlistpane)
	ON_COMMAND(ID_FULLSCREEN, &CMainFrame::OnFullscreen)
	ON_COMMAND(ID_BONEPROPERTIES, &CMainFrame::OnBoneProperties)
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, &CMainFrame::OnUpdateFullscreen)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SHADERFOLDER, &CMainFrame::OnShaderfolder)
	ON_COMMAND(ID_TOGGLEGRID, &CMainFrame::OnToggleGrid)
	ON_COMMAND(ID_LESSPRIORITY, &CMainFrame::OnLesspriority)
	ON_UPDATE_COMMAND_UI(ID_LESSPRIORITY, &CMainFrame::OnUpdateLesspriority)
	ON_UPDATE_COMMAND_UI(ID_LINKOBJECT, &CMainFrame::OnUpdateLinkObject)
	ON_COMMAND(ID_LINKOBJECT, &CMainFrame::OnLinkObject)
	ON_UPDATE_COMMAND_UI(ID_UNLINKOBJECT, &CMainFrame::OnUpdateUnlinkObject)
	ON_COMMAND(ID_UNLINKOBJECT, &CMainFrame::OnUnlinkObject)
	ON_COMMAND(ID_CAPTURE, &CMainFrame::OnCapture)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_PROG,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_dwStyle = 0;
	m_dwExStyle = 0;
	m_pResMng = new CEtResourceMng( false );
}

CMainFrame::~CMainFrame()
{
	SAFE_DELETE( m_pResMng );
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	if (!InitCommandBars()) return -1;

	// Progress
	int iIndex = m_wndStatusBar.GetPaneCount();
	if (!m_wndProgCtrl.Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
		CRect(0,0,0,0), &m_wndStatusBar, 0))
	{
		TRACE0("Failed to create edit control.\n");
		return 0;
	}

	// add the indicator to the status bar.
	m_wndStatusBar.AddIndicator(ID_INDICATOR_PROG, iIndex);

	// Initialize the pane info and add the control.
	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_PROG);
	ASSERT (nIndex != -1);

	m_wndStatusBar.SetPaneWidth(nIndex, 100);
	m_wndStatusBar.SetPaneStyle(nIndex, m_wndStatusBar.GetPaneStyle(nIndex) | SBPS_NOBORDERS);
	m_wndStatusBar.AddControl(&m_wndProgCtrl, ID_INDICATOR_PROG, FALSE);

	// initialize progress control.
	m_wndProgCtrl.SetRange (0, 100);
	m_wndProgCtrl.SetPos(0);
	m_wndProgCtrl.SetStep(1);

	AfxInitRichEdit();

	// Init Thema
	XTPPaintManager()->SetTheme( xtpThemeNativeWinXP ); 

	XTP_COMMANDBARS_ICONSINFO* pIconsInfo = XTPPaintManager()->GetIconsInfo();
	pIconsInfo->bUseDisabledIcons = TRUE;

	CXTPCommandBars* pCommandBars = GetCommandBars();
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME); 

	CXTPToolBar* pCommandBar;

	//	XTPImageManager()->SetMaskColor(RGB(0, 255, 0));
	if (!(pCommandBar = (CXTPToolBar*)pCommandBars->Add(_T("Standard"), xtpBarTop)) ||
		!pCommandBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	if (!(pCommandBar = (CXTPToolBar*)pCommandBars->Add(_T("Show/Hide Window"), xtpBarBottom )) ||
		!pCommandBar->LoadToolBar(IDR_PANEVIEW))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	pCommandBars->Remove( pCommandBar );
	if (!(pCommandBar = (CXTPToolBar*)pCommandBars->Add(_T("Etc Bar"), xtpBarBottom )) ||
		!pCommandBar->LoadToolBar(IDR_ETCBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
	pCommandBars->Remove( pCommandBar );


	XTPDockingPaneManager()->InstallDockingPanes(this);
	XTPDockingPaneManager()->SetTheme( xtpPaneThemeOffice ); //NativeWinXP
	m_paneManager.SetThemedFloatingFrames( TRUE );

	/*
	m_paneManager.SetCustomTheme(new CDockingPaneAdobeTheme);
	m_paneManager.SetDockingContext(new CDockingPaneAdobeContext);
	m_paneManager.SetDefaultPaneOptions(xtpPaneNoCloseable);
	*/

	int nCount = sizeof(g_PaneList) / sizeof(PaneCreateStruct);
	int *nIconList = new int[nCount];
	for( int i=0; i<nCount; i++ ) {
		CXTPDockingPane *pDockPane = NULL;
		nIconList[i] = g_PaneList[i].nPaneID;
		if( g_PaneList[i].nDockPaneID != -1 ) pDockPane = g_PaneList[g_PaneList[i].nDockPaneID].pThis;
		g_PaneList[i].pThis = CreatePane( g_PaneList[i].nPaneID, g_PaneList[i].pClass, CString(g_PaneList[i].szName), g_PaneList[i].Direction, pDockPane, &CRect( g_PaneList[i].nLeft, g_PaneList[i].nTop, g_PaneList[i].nRight, g_PaneList[i].nBottom ) );
		if( g_PaneList[i].nAttackPaneID != -1 ) {
			XTPDockingPaneManager()->AttachPane( g_PaneList[i].pThis, g_PaneList[g_PaneList[i].nAttackPaneID].pThis );
			XTPDockingPaneManager()->ShowPane( g_PaneList[g_PaneList[i].nAttackPaneID].pThis );
		}
	}
	XTPDockingPaneManager()->SetIcons( IDB_PANEICON, nIconList, nCount, RGB( 0, 255, 0 ) );
	delete []nIconList;

	for( int i=0; i<nCount; i++ ) {
		if( g_PaneList[i].bShow == FALSE )
			XTPDockingPaneManager()->ClosePane( g_PaneList[i].pThis );
	}

	// Etc Initialize
	LoadCommandBars(_T("EtViewerCommandBars"));

	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	if (layoutNormal.Load(_T("EtViewerLayout")))
		m_paneManager.SetLayout(&layoutNormal);

	CInputDevice::GetInstance().Initialize( m_hWnd, CInputDevice::MOUSE );

	return 0;
}

void CMainFrame::UpdateProgress( int nPer )
{
	m_wndProgCtrl.SetPos(nPer);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CXTPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CXTPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CXTPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기




CXTPDockingPane* CMainFrame::CreatePane( int nID, CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour, CRect *pRect ) 
{
	CRect Rect;
	if( pRect == NULL ) Rect = CRect( 0, 0, 200, 200 );
	else Rect = *pRect;
	CXTPDockingPane* pwndPane = m_paneManager.CreatePane(
		nID, Rect, direction, pNeighbour);

	CString strTitle;
	strTitle.Format(strFormat, nID);
	pwndPane->SetTitle(strTitle);

	CFrameWnd* pFrame = new CFrameWnd;

	CCreateContext context;
	context.m_pNewViewClass = pNewViewClass;
	context.m_pCurrentDoc = NULL;

	DWORD dwStyle;
	dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	pFrame->Create(NULL, NULL, dwStyle, CRect(0, 0, 0, 0), this, NULL, 0, &context);
	pFrame->ModifyStyleEx( WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_OVERLAPPEDWINDOW, 0 );
	pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);


	m_mapPanes.SetAt(nID, pFrame);

	return pwndPane;
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		// get a pointer to the docking pane being shown.
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			CWnd* pWnd = NULL;
			if (m_mapPanes.Lookup(pPane->GetID(), pWnd))
			{
				pPane->Attach(pWnd);
			}
		}	
		return TRUE; // handled
	}

	if (wParam == XTP_DPN_CLOSEPANE)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		CWnd* pWnd =NULL;

		if (!m_mapPanes.Lookup(pPane->GetID(), pWnd))
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

CWnd *CMainFrame::GetPaneFrame( int nID )
{
	if( XTPDockingPaneManager() == NULL ) return NULL;
	if( XTPDockingPaneManager()->GetSite() == NULL ) return NULL;
	CXTPDockingPane* pPane = XTPDockingPaneManager()->FindPane( nID );
	if( pPane == NULL ) return NULL;

	CWnd *pWnd = NULL;
	if (!m_mapPanes.Lookup(pPane->GetID(), pWnd) ) return NULL;

	CFrameWnd* pFrame = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);
	return (CWnd*)pFrame->GetWindow( GW_CHILD );
	//	return pWnd;
}

int CMainFrame::GetFocusPaneID()
{
	int nCount = sizeof(g_PaneList) / sizeof(PaneCreateStruct);
	for( int i=0; i<nCount; i++ ) {
		CXTPDockingPane* pPane = XTPDockingPaneManager()->FindPane( g_PaneList[i].nPaneID );
		if( pPane->IsFocus() ) return g_PaneList[i].nPaneID;
	}
	return -1;
}

void CMainFrame::OnDestroy()
{
	CXTPFrameWnd::OnDestroy();
}

void CMainFrame::SetFocus( int nID )
{
	CXTPDockingPane* pPane = XTPDockingPaneManager()->FindPane( nID );
	XTPDockingPaneManager()->ShowPane( pPane );
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SaveCommandBars(_T("EtViewerCommandBars"));

	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.Save(_T("EtViewerLayout"));


	CObjectBase *pBase = CGlobalValue::GetInstance().GetRootObject();
	if( pBase->IsChildModify() == true ) {
		CString szBuf;
		pBase->GetChildModifyString( szBuf );
		if( MessageBox( szBuf, "변경된 Skin값이 있습니다. 저장하시겠습니까?", MB_YESNO ) == IDYES )
			CGlobalValue::GetInstance().SaveSkinObject( pBase );
	}
	CXTPFrameWnd::OnClose();
}

void CMainFrame::OnUpdatePaneWorkspacepane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( !XTPDockingPaneManager()->IsPaneClosed( WORKSPACE_PANE ) );
}

void CMainFrame::OnUpdatePanePropertiespane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( !XTPDockingPaneManager()->IsPaneClosed( PROP_SHADER_PANE ) );
}

void CMainFrame::OnUpdatePaneAnimationlistpane(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck( !XTPDockingPaneManager()->IsPaneClosed( ANI_PANE ) );
}

void CMainFrame::OnPaneWorkspacepane()
{
	if( XTPDockingPaneManager()->IsPaneClosed( WORKSPACE_PANE ) == TRUE )
		XTPDockingPaneManager()->ShowPane( WORKSPACE_PANE );
	else XTPDockingPaneManager()->ClosePane( WORKSPACE_PANE );
}

void CMainFrame::OnPanePropertiespane()
{
	if( XTPDockingPaneManager()->IsPaneClosed( PROP_SHADER_PANE ) == TRUE )
		XTPDockingPaneManager()->ShowPane( PROP_SHADER_PANE );
	else XTPDockingPaneManager()->ClosePane( PROP_SHADER_PANE );
}

void CMainFrame::OnPaneAnimationlistpane()
{
	if( XTPDockingPaneManager()->IsPaneClosed( ANI_PANE ) == TRUE )
		XTPDockingPaneManager()->ShowPane( ANI_PANE );
	else XTPDockingPaneManager()->ClosePane( ANI_PANE );
}

void CMainFrame::OnFullscreen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if( CGlobalValue::GetInstance().IsFullScreen() == true ) {
		DWORD dwStyle = GetStyle();
		DWORD dwExStyle = GetExStyle();

		ModifyStyle( dwStyle, m_dwStyle );
		ModifyStyleEx( dwExStyle, m_dwExStyle );
		SetWindowPos( NULL, m_rcPrevRect.left, m_rcPrevRect.top, m_rcPrevRect.Width(), m_rcPrevRect.Height(), SWP_FRAMECHANGED );

		CXTPCommandBars* pCommandBars = GetCommandBars();
		CXTPToolBar *pToolBar = pCommandBars->GetToolBar( IDR_MAINFRAME );
		CXTPMenuBar* pMenuBar = pCommandBars->GetMenuBar();

		pMenuBar->SetVisible( TRUE );
		pToolBar->SetVisible( TRUE );
		m_wndStatusBar.ShowWindow( SW_SHOW );

		XTPDockingPaneManager()->ShowPane( WORKSPACE_PANE );
		XTPDockingPaneManager()->ShowPane( PROP_SHADER_PANE );
		XTPDockingPaneManager()->ShowPane( ANI_PANE );


	}
	else {
		XTPDockingPaneManager()->ClosePane( WORKSPACE_PANE );
		XTPDockingPaneManager()->ClosePane( PROP_SHADER_PANE );
		XTPDockingPaneManager()->ClosePane( ANI_PANE );

		m_dwStyle = GetStyle();
		m_dwExStyle = GetExStyle();
		GetWindowRect( &m_rcPrevRect );

		ModifyStyle( WS_BORDER | WS_CAPTION | WS_THICKFRAME | WS_POPUP, 0 );
		ModifyStyleEx( WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME, WS_EX_TOPMOST );
		long lWidth = GetSystemMetrics( SM_CXSCREEN ) + GetSystemMetrics( SM_CXFRAME );
		long lHeight = GetSystemMetrics( SM_CYSCREEN ) + GetSystemMetrics( SM_CYFRAME );

		CXTPCommandBars* pCommandBars = GetCommandBars();
		CXTPToolBar *pToolBar = pCommandBars->GetToolBar( IDR_MAINFRAME );
		CXTPMenuBar* pMenuBar = pCommandBars->GetMenuBar();

		pMenuBar->SetVisible( FALSE );
		pToolBar->SetVisible( FALSE );
		m_wndStatusBar.ShowWindow( SW_HIDE );


		SetWindowPos( NULL, 0, 0, lWidth, lHeight, 0 );
	}
	CGlobalValue::GetInstance().SetFullScreen( !CGlobalValue::GetInstance().IsFullScreen() );
}

void CMainFrame::OnUpdateFullscreen(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( CGlobalValue::GetInstance().IsFullScreen() );
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CXTPFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnShaderfolder()
{
	// TODO: Add your command handler code here
	CPropertiesDlg Dlg;
	Dlg.DoModal();
}


void CMainFrame::OnBoneProperties()
{
	CBonePropDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnUpdateBoneProperties( CCmdUI *pCmdUI )
{
	int nCount = CGlobalValue::GetInstance().GetObjectCount( CObjectBase::SKIN );
	pCmdUI->Enable( nCount > 0 );
}

void CMainFrame::OnLesspriority()
{
	// TODO: Add your command handler code here
	CRenderBase::GetInstance().ChangePriority( !CRenderBase::GetInstance().IsLessPriority() );
}

void CMainFrame::OnUpdateLesspriority(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck( CRenderBase::GetInstance().IsLessPriority() );
}

void CMainFrame::OnToggleGrid()
{
	// TODO: Add your command handler code here
	CRenderBase::GetInstance().ShowGrid( !CRenderBase::GetInstance().IsShowGrid() );
	CRenderBase::GetInstance().ShowAxis( !CRenderBase::GetInstance().IsShowAxis() );

	CWnd *pWnd = GetPaneWnd( PROP_SHADER_PANE );
	if( pWnd ) pWnd->SendMessage( UM_SHADERPROP_REFRESH );
}


void CMainFrame::OnLinkObject()
{
	CLinkObjectDlg Dlg;
	Dlg.DoModal();
}

void CMainFrame::OnUpdateLinkObject(CCmdUI *pCmdUI)
{
	bool bEnable = ( CGlobalValue::GetInstance().GetObjectCount( CObjectBase::SKIN ) < 2 ) ? false : true;
	pCmdUI->Enable( bEnable );
}

void CMainFrame::OnUnlinkObject()
{
	CObjectBase *pRoot = CGlobalValue::GetInstance().GetOneObject( CObjectBase::SCENE );
	for( DWORD i=0; i<pRoot->GetChildCount(); i++ ) {
		CObjectSkin *pSkin = dynamic_cast<CObjectSkin *>(pRoot->GetChild(i));
		if( !pSkin ) continue;
		EtAniObjectHandle hHandle = pSkin->GetObjectHandle();
		if( !hHandle ) continue;
		hHandle->SetParent( CEtObject::Identity(), -1 );
	}
}

void CMainFrame::OnUpdateUnlinkObject(CCmdUI *pCmdUI)
{
	bool bEnable = ( CGlobalValue::GetInstance().GetObjectCount( CObjectBase::SKIN ) < 2 ) ? false : true;
	pCmdUI->Enable( bEnable );
}

void CMainFrame::OnCapture()
{
	// TODO: Add your command handler code here
	EternityEngine::CaptureScreen( "Viewer" );
}
