// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "EtEffect2Tool.h"

#include "ParticlePaneView.h"
#include "FXPaneView.h"
#include "KeyframePropPaneView.h"
#include "PropertyPaneView.h"
#include "KeyControlPaneView.h"
#include "SequenceView.h"
#include "InputDevice.h"
#include "PreviewPaneView.h"
#include "MainFrm.h"
#include "../../Common/BugReporter/BugReporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CInputDevice g_InputDevice;
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CXTPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CXTPFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("도구 모음을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	  sizeof(indicators)/sizeof(UINT)))
	//{
	//	TRACE0("상태 표시줄을 만들지 못했습니다.\n");
	//	return -1;      // 만들지 못했습니다.
	//}

	//// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);
	/*BugReporter::ReportingServerInfo info;
	info.szServerURL = "http://192.168.0.20/bugreport/bugprocess.aspx";
	char buff[512] = {0,};
	gs_BugReporter.SetBuildVersion(std::string(buff));
	gs_BugReporter.SetReportingServerInfo(info);
	gs_BugReporter.Enable();*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!InitCommandBars())
		return -1;

	CXTPPaintManager::SetTheme(xtpThemeOffice2003);


	CXTPCommandBars* pCommandBars = GetCommandBars();

	pCommandBars->SetMenu(_T("Menu Bar"), IDR_MAINFRAME);

	CXTPToolBar* pStandardBar = (CXTPToolBar*)pCommandBars->Add(_T("Standard"), xtpBarTop);
	if (!pStandardBar ||
		!pStandardBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	// Load the previous state for command bars.
	LoadCommandBars(_T("CommandBars"));


	m_paneManager.InstallDockingPanes(this);

	CreatePanes();

	m_paneManager.SetTheme(xtpPaneThemeOffice);

	int nIDIcons[] = {1, 2, 3, 4, 5, 6};
	m_paneManager.SetIcons(IDB_BITMAP_ICONS, nIDIcons, 7, RGB(255, 0, 255));

	XTPPaintManager()->RefreshMetrics();

	CInputDevice::GetInstance().Initialize( m_hWnd, CInputDevice::MOUSE );

	return 0;
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
void CMainFrame::CreatePanes()
{
	CXTPDockingPane *pane0 = CreatePane(RUNTIME_CLASS(CSequenceView), _T("Frame Control"), xtpPaneDockBottom);
	CXTPDockingPane *pane1 = CreatePane(RUNTIME_CLASS(CParticlePaneView), _T("Particle Edit"), xtpPaneDockLeft);	
	CXTPDockingPane *pane2 = CreatePane(RUNTIME_CLASS(CFXPaneView), _T("FX Edit"), xtpPaneDockLeft);
	CXTPDockingPane *pane3 = CreatePane(RUNTIME_CLASS(CPropertyPaneView), _T("Properties"), xtpPaneDockBottom, pane1, 300, 300);
	CXTPDockingPane *pane4 = CreatePane(RUNTIME_CLASS(CKeyframePropPaneView), _T("Curve Properties"), xtpPaneDockBottom, pane1);
	CXTPDockingPane *pane5 = CreatePane(RUNTIME_CLASS(CKeyControlPaneView), _T("Keyframe Control"), xtpPaneDockBottom );	
	CXTPDockingPane *pane6 = CreatePane(RUNTIME_CLASS(CPreviewPaneView), _T("Preview List"), xtpPaneDockLeft);

	m_vecPanes.push_back(pane0);
	m_vecPanes.push_back(pane1);
	m_vecPanes.push_back(pane2);
	m_vecPanes.push_back(pane3);
	m_vecPanes.push_back(pane4);
	m_vecPanes.push_back(pane5);
	m_vecPanes.push_back(pane6);

	m_paneManager.AttachPane( pane5, pane0);
	m_paneManager.AttachPane( pane2, pane1);
	m_paneManager.AttachPane( pane6, pane2);
	m_paneManager.AttachPane( pane4, pane3);
	m_paneManager.ShowPane( pane1 );
	m_paneManager.ShowPane( pane3 );
	m_paneManager.ShowPane( pane0 );
}

void CMainFrame::ShowPane( int nIndex )
{
	m_paneManager.ShowPane( m_vecPanes[nIndex] );
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


CXTPDockingPane* CMainFrame::CreatePane(CRuntimeClass* pNewViewClass, CString strFormat, XTPDockingPaneDirection direction, CXTPDockingPane* pNeighbour, int width, int height)
{
	int nID = ++m_nCount;

	CXTPDockingPane* pwndPane = m_paneManager.CreatePane(
		nID, CRect(0, 0,width, height), direction, pNeighbour);

	CString strTitle;
	strTitle.Format(strFormat, nID);
	pwndPane->SetTitle(strTitle);
	pwndPane->SetIconID(nID % 6 + 1);

	CFrameWnd* pFrame = new CFrameWnd;

	CCreateContext context;
	context.m_pNewViewClass = pNewViewClass;
	context.m_pCurrentDoc = NULL;//GetActiveView()->GetDocument();

	pFrame->Create(NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, NULL, 0, &context);
	pFrame->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	pFrame->SendMessageToDescendants(WM_INITIALUPDATE, 0, 0, TRUE, TRUE);

	m_mapPanes.SetAt(nID, pFrame);

	return pwndPane;
}

