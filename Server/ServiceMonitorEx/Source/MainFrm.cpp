// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "MainFrm.h"
#include "BasePartitionView.h"
#include "BaseMonitorView.h"
#include "BaseInformationView.h"
#include "BaseErrorLogView.h"
#include "ConfigMngr.h"
#include "NetConnection.h"

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"
#include "ConnectionInfoMngr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_POPUP_RESTORE, &CMainFrame::OnPopupRestore)
	ON_COMMAND(ID_POPUP_MINIMIZE, &CMainFrame::OnPopupMinimize)
	ON_COMMAND(ID_POPUP_EXIT, &CMainFrame::OnPopupExit)
	ON_COMMAND(ID_POPUP_ABOUT, &CMainFrame::OnPopupAbout)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_TOTALSTATUSBAR, &CMainFrame::OnViewTotalStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOTALSTATUSBAR, &CMainFrame::OnUpdateViewTotalStatusBar)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_COMMAND(ID_SETTING_CONNECTION, &CMainFrame::OnSettingConnection)
	ON_UPDATE_COMMAND_UI(ID_SETTING_CONNECTION, &CMainFrame::OnUpdateSettingConnection)
	ON_COMMAND(ID_CONTROL_CONNECTIONFROMFILE, &CMainFrame::OnConnectionFromFile)
	ON_COMMAND(ID_SETTING_CONFIG, &CMainFrame::OnSettingConfig)
	ON_UPDATE_COMMAND_UI(ID_SETTING_CONFIG, &CMainFrame::OnUpdateSettingConfig)
	ON_COMMAND(IDC_MDB_ERRORLOGBUTTON, &CMainFrame::OnButtonErrorLog)
	ON_UPDATE_COMMAND_UI(IDC_MDB_ERRORLOGBUTTON, &CMainFrame::OnUpdateButtonErrorLog)
	ON_COMMAND(IDC_MDB_REFRESHWORLDINFO, &CMainFrame::OnButtonRefreshWorldInfo)
	ON_UPDATE_COMMAND_UI(IDC_MDB_REFRESHWORLDINFO, &CMainFrame::OnUpdateButtonRefreshWorldInfo)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here

	CServiceMonitorApp* aWinApp = static_cast<CServiceMonitorApp*>(::AfxGetApp());
	ASSERT(NULL != aWinApp);
	aWinApp->SetMainFrame(this);

	m_DoAlert = FALSE;
	for (int aIndex = 0 ; EV_SBT_CNT > aIndex ; ++aIndex) {
		m_TabIconTick[aIndex] = 0;
	}
}

CMainFrame::~CMainFrame()
{
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style &= ~FWS_ADDTOTITLE;	// 현재 뷰와 연결된 도큐멘트 이름을 프레임 타이틀바에 출력하지 않음
	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


void CMainFrame::ToggleConnectionDialog()
{
	if (!::IsWindow(m_ConnectionDialog.GetSafeHwnd())) {
		m_ConnectionDialog.Create(IDD_CNNTDLG, this);
	}

	m_ConnectionDialog.ShowWindow((!m_ConnectionDialog.IsWindowVisible())?(SW_NORMAL):(SW_HIDE));
}


void CMainFrame::ShowConnectionDialog(int nCmdShow)
{
	if (!::IsWindow(m_ConnectionDialog.GetSafeHwnd())) {
		m_ConnectionDialog.Create(IDD_CNNTDLG, this);
	}

	m_ConnectionDialog.ShowWindow(nCmdShow);
}

void CMainFrame::ShowControlDialog(int nCmdShow)
{

}




DWORD CMainFrame::GetTickTerm(DWORD dwOldTick, DWORD dwCurTick)
{
	return((dwCurTick >= dwOldTick)?(dwCurTick - dwOldTick):(ULONG_MAX - dwOldTick + dwCurTick));
}

int CMainFrame::GetActivePageIndex() const
{
	return(m_PartitionTabView.GetActivePageIndex());
}

void CMainFrame::SetPageImageIndex(int pPageIndex, int pImageIndex)
{
	m_PartitionTabView.SetPageImageIndex(pPageIndex, pImageIndex);
}

void CMainFrame::ForceAllTabViewActivate()
{
	if (m_PartitionTabView.GetPageCount() > 0)
	{
		// COX3DTabViewContainer 잠김 방지
		int aActivePageIndex = m_PartitionTabView.GetActivePageIndex();
		for (int aIndex = 0 ; m_PartitionTabView.GetPageCount() > aIndex ; ++aIndex)
			m_PartitionTabView.SetActivePageIndex(aIndex);

		m_PartitionTabView.SetActivePageIndex((0 <= aActivePageIndex)?(aActivePageIndex):(0));
	}	

}
// CMainFrame message handlers




int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	CCreateContext *aCreateContext = static_cast<CCreateContext*>(lpCreateStruct->lpCreateParams);
	ASSERT(NULL != aCreateContext);

	CRect aRect;
	GetClientRect(&aRect);

	// 추후 윈도우 위치상태 저장이 들어가게 되면 여기에서 처리
	CenterWindow();


	// 아이콘 설정
	{
		SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);	// 큰 아이콘
		SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);	// 작은 아이콘
	}
	// Window Position
	{

		/*
			SetWindowPlacement()

			- flags
				> WPF_ASYNCWINDOWPLACEMENT : 
				> WPF_SETMINPOSITION : 아이콘화된 창의 x, y 좌표가 ptMaxPosition 멤버에 지정되어 있음.
				> WPF_RESTORETOMAXIMIZED : 아이콘화되기 전 상태에 관계없이 화면복귀 시 최대화 되어야 함.

			- UINT showCmd
				> SW_HIDE : 창을 숨기고 다른 창을 활성화시킨다.
				> SW_MINIMIZE : 창을 아이콘화 시킨다. 이 때 다른 창이 활성화 된다.
				> SW_RESTORE : 창을 활성화시킨다. 그 전에 아이콘 또는 최대 상태였다면 원래 상태대로 복원된다.
				> SW_SHOW : 이전 위치, 크기대로 보이게 하고 활성화 시킨다.
				> SW_SHOWMAXIMIZED : 활성화하고 최대화 시킨다.
				> SW_SHOWMINIMIZED : 활성화하고 최소화 시킨다.
				> SW_SHOWNA : 창을 현재 상태대로 활성화 시킨다.
				> SW_SHOWNOACTIVATE : 창을 가장 최근 상태로 활성화 시킨다.
				> SW_SHOWNORMAL : 이는 SW_RESTORE와 같음.
		*/

		if (0 != CConfigMngr::GetInstancePtr()->m_WindowPlacement.rcNormalPosition.top ||
			0 != CConfigMngr::GetInstancePtr()->m_WindowPlacement.rcNormalPosition.left ||
			0 != CConfigMngr::GetInstancePtr()->m_WindowPlacement.rcNormalPosition.bottom ||
			0 != CConfigMngr::GetInstancePtr()->m_WindowPlacement.rcNormalPosition.right
			)
		{
			SetWindowPlacement(&CConfigMngr::GetInstancePtr()->m_WindowPlacement);
		}

		if (SW_MINIMIZE == CConfigMngr::GetInstancePtr()->m_WindowPlacement.showCmd ||
			SW_SHOWMINIMIZED == CConfigMngr::GetInstancePtr()->m_WindowPlacement.showCmd ||
			SW_FORCEMINIMIZE == CConfigMngr::GetInstancePtr()->m_WindowPlacement.showCmd
			)
		{
			ShowWindow(SW_RESTORE);
		}

		if (SW_MAXIMIZE == CConfigMngr::GetInstancePtr()->m_WindowPlacement.showCmd ||
			SW_SHOWMAXIMIZED == CConfigMngr::GetInstancePtr()->m_WindowPlacement.showCmd
			)
		{
			ShowWindow(SW_SHOWMAXIMIZED);
//			CFrameWnd::ActivateFrame(SW_SHOWMAXIMIZED);
		}

		
	}

	// Open Connection Dialog
	OnSettingConnection();

	return 0;
}

CBasePartitionView* CMainFrame::AddPartition (CNetConnection* pConnection, ULONG uIpAddress)
{
	CCreateContext aCreateContext;
	::memset(&aCreateContext, 0, sizeof(aCreateContext));
	aCreateContext.m_pNewViewClass = RUNTIME_CLASS(CMainFrame);

	SOCKADDR_IN stSockAddrIn;
	stSockAddrIn.sin_family = AF_INET;
	stSockAddrIn.sin_addr.s_addr = uIpAddress;

	WCHAR wszAddress[20];
	_swprintf(wszAddress, L"%S", inet_ntoa(stSockAddrIn.sin_addr));

	if (!m_PartitionTabView.AddPage(RUNTIME_CLASS(CBasePartitionView), &aCreateContext, wszAddress, EV_IML_PARTITION))
	{
		TRACE0("Failed to add page m_MainTabView ←CBasePartitionView\n");
		return NULL;		// fail to create
	}

	INT nIndex = m_PartitionTabView.GetPageCount();
	if(nIndex <= 0) return NULL;

	CBasePartitionView* pPartitionView = static_cast<CBasePartitionView*>(m_PartitionTabView.GetPage(nIndex - 1));
	ASSERT(NULL != pPartitionView);

	pPartitionView->Set(nIndex - 1);
	pPartitionView->Start (pConnection);

	// App에 파티션과 세션 등록
	CServiceMonitorApp* aWinApp = static_cast<CServiceMonitorApp*>(::AfxGetApp());
	aWinApp->SetPartitionView(uIpAddress, pPartitionView);

	return pPartitionView;
}

void CMainFrame::RemovePartition (CBasePartitionView* pPartition)
{
	ULONG nConID = pPartition->GetConnectionID();
	CNetConnectionMgr::GetInstancePtr()->RemoveConnection(nConID);

	pPartition->Close ();

	// 페이지 인덱스가 자동정렬되지 않아 일단 막는다.
 	m_PartitionTabView.DeletePage (pPartition->GetPageIndex());
	
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpCreateStruct, CCreateContext* /*pContext*/)
{
	// 본 메서드를 오버라이딩 하면 기본 뷰 (CMainView) 를 생성 않함

	CCreateContext *aCreateContext = static_cast<CCreateContext*>(lpCreateStruct->lpCreateParams);
	ASSERT(NULL != aCreateContext);

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_PartitionTabView.Create(this, aRect, 
		WS_VISIBLE | WS_CHILD | TCS_FOCUSNEVER | TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_HOTTRACK,
		AFX_IDW_PANE_FIRST
		))
	{
		TRACE0("Failed to create tabview container m_MainTabView\n");
		return -1;		// fail to create
	}
	m_PartitionTabView.SetOffsetExternal(0);	// 탭 뷰와 부모 프레임 사이의 간격
	m_PartitionTabView.SetOffsetInternal(0);	// 탭 뷰와 자식 뷰와 사이의 간격
	m_PartitionTabView.SetActivePageIndex(0);

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if(SIZE_MINIMIZED == nType)
		ShowWindow(SW_MINIMIZE);
	
}

void CMainFrame::OnPopupRestore()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	SetForegroundWindow();
	PostMessage(WM_SYSCOMMAND, SC_RESTORE);
	ShowWindow(SW_SHOW);
}

void CMainFrame::OnPopupMinimize()
{
	ShowWindow(SW_SHOWMINIMIZED);
}

void CMainFrame::OnPopupExit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	PostMessage(WM_CLOSE);
}

void CMainFrame::OnPopupAbout()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	PostMessage(WM_COMMAND, ID_APP_ABOUT);
}

void CMainFrame::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (IDYES != ::AfxMessageBox(_T("Are you sure to quit ?"), MB_ICONQUESTION | MB_YESNO)) {
		return;
	}

	// Save Config
	{
		{
			
		}

		// Window Position
		{
			GetWindowPlacement(&CConfigMngr::GetInstancePtr()->m_WindowPlacement);

			CConfigMngr::GetInstancePtr()->Flush();
		}
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnViewTotalStatusBar()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

}

void CMainFrame::OnUpdateViewTotalStatusBar(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CMainFrame::OnSettingConnection()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	ToggleConnectionDialog();
}

void CMainFrame::OnConnectionFromFile()
{
	CConnectionInfoMngr::GetInstancePtr()->ConnectToServerManager();
}

void CMainFrame::OnUpdateSettingConnection(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	pCmdUI->SetCheck(::IsWindow(m_ConnectionDialog.GetSafeHwnd()) && m_ConnectionDialog.IsWindowVisible());
}

void CMainFrame::OnSettingConfig()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	
}

void CMainFrame::OnUpdateSettingConfig(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	pCmdUI->Enable(FALSE);	// 현재 미지원
//	pCmdUI->SetCheck(::IsWindow(m_ControlDialog.GetSafeHwnd()) && m_ControlDialog.IsWindowVisible());		// 테스트 - 삭제되어야 함 !!!
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	KillTimer(EV_TMD_REFRESHSERVICESTATUS);
}

void CMainFrame::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CFrameWnd::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (lpwndpos->flags & SWP_SHOWWINDOW) {
		// 윈도우 보임 시 통지

		ForceAllTabViewActivate();	// COX3DTabViewContainer 잠김 방지
	}

	if (lpwndpos->flags & SWP_HIDEWINDOW) {
		// 윈도우 숨김 시 통지

		
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFrameWnd::PreTranslateMessage(pMsg);
}


void CMainFrame::OnButtonErrorLog()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

}

void CMainFrame::OnUpdateButtonErrorLog(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.

	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnButtonRefreshWorldInfo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CMainFrame::OnUpdateButtonRefreshWorldInfo(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}
