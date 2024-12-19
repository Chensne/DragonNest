

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "BasePartitionView.h"
#include "BaseMonitorView.h"
#include "BaseInformationView.h"
#include "BaseErrorLogView.h"
#include "MainFrm.h"

#define DNPRJSAFE	// DNPrjSafe.h 제외
#include "CriticalSection.h"
#include "DNPacket.h"
#include "DNServerDef.h"
#include "DNServerPacket.h"

IMPLEMENT_DYNCREATE(CBasePartitionView, CScrollView)


BEGIN_MESSAGE_MAP(CBasePartitionView, CScrollView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CBasePartitionView::CBasePartitionView()
{
	m_IsInitialized = FALSE;
	m_ConnectionID = 0;
	m_DoHideControlDialog = FALSE;
	
	m_pMonitorView = NULL;
	m_pInformationView = NULL;
	m_pErrorLogView = NULL;
	m_IsSized = FALSE;
	m_RefreshWorldInfoTick = 0;
	m_pCreateContext = NULL;
	m_NetConnection = NULL;

	for (int aIndex = 0 ; EV_SBT_CNT > aIndex ; ++aIndex)
		m_TabIconTick[aIndex] = 0;
}

CBasePartitionView::~CBasePartitionView()
{

}


#ifdef _DEBUG
void CBasePartitionView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CBasePartitionView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


void CBasePartitionView::ForceAllTabViewActivate()
{
	if (m_MainTabView.GetPageCount() > 0)
	{
		// COX3DTabViewContainer 잠김 방지
		int aActivePageIndex = m_MainTabView.GetActivePageIndex();
		for (int aIndex = 0 ; m_MainTabView.GetPageCount() > aIndex ; ++aIndex)
			m_MainTabView.SetActivePageIndex(aIndex);
		
		m_MainTabView.SetActivePageIndex((0 <= aActivePageIndex)?(aActivePageIndex):(0));
	}
}


// CBaseMonitorView 메시지 처리기입니다.


int CBasePartitionView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	CCreateContext *pCreateContext = static_cast<CCreateContext*>(lpCreateStruct->lpCreateParams);
	if (!pCreateContext)
		m_pCreateContext = pCreateContext;

	return 0;
}

void CBasePartitionView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	//if (m_pMonitorView && ::IsWindow(m_pMonitorView->GetSafeHwnd()))
		//m_pMonitorView->MoveWindow(GetFrameViewRect());
	

	m_IsSized = TRUE;
}

void CBasePartitionView::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_NetConnection)
		return;

	CNetSession* pSession = m_NetConnection->GetSession();
	
	switch(nIDEvent) 
	{
	case EV_TMD_REQUESTSERVICEINFO:
			OnRequestServiceInfo (pSession);
		break;

	case EV_TMD_REFRESHSERVICESTATUS:
			OnRefreshServiceStatus (pSession);
		break;
	default:
		break;
	}
	

	CScrollView::OnTimer(nIDEvent);
}

void CBasePartitionView::OnClose()
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

	CScrollView::OnClose();
}

void CBasePartitionView::OnDestroy()
{
	CScrollView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(EV_TMD_REQUESTSERVICEINFO);
	KillTimer(EV_TMD_REFRESHSERVICESTATUS);
}

void CBasePartitionView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CScrollView::OnPaint()을(를) 호출하지 마십시오.
	return;
}

void CBasePartitionView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{

	if (m_IsInitialized)
		return;

	InitUpdate ();

	CreateAllView();

	m_IsInitialized = TRUE;
}

//---------------------------------------------------------------------------------------------------
void CBasePartitionView::InitUpdate ()
{
	// 아이콘설정
	SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), TRUE);	// 큰아이콘
	SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME)), FALSE);	// 작은아이콘

	//CRect rt (0, 0, PDS_RIGHT, PDS_BOTTOM);
	CRect rt;
	GetClientRect(&rt);

	rt.bottom = PDS_BOTTOM;

	m_pPartitionDlg = new CPartitionDlg;

	if (m_pPartitionDlg->GetSafeHwnd() == 0)
		if (!m_pPartitionDlg->Create(rt, this))
			return;

	static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_CONNECTBUTTON))->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NETDSCN1)));
	static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_ERRORLOGBUTTON))->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ERRORLOG)));
	static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_REFRESHWORLDINFO))->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_REFRESHWORLDINFO)));
	static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_CHANGEPW))->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_PASSWORD)));

	
	// ToolTip
	m_ToolTip.Create(this);
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_TOTAL_USERCOUNT), _T("Count of total user, include user at login server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_TOTAL_SERVERCOUNT), _T("Count of total server, all type"));
	/*
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_LO_SERVERCOUNT), _T("Count of login server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_MA_SERVERCOUNT), _T("Count of master server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_LG_SERVERCOUNT), _T("Count of log server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_VI_SERVERCOUNT), _T("Count of village server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_GA_SERVERCOUNT), _T("Count of game server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_DB_SERVERCOUNT), _T("Count of middleware server"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_CA_SERVERCOUNT), _T("Count of cash server"));
	*/
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_CONNECTBUTTON), _T("Connection Status"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_ERRORLOGBUTTON), _T("Error Log Status"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_MDB_REFRESHWORLDINFO), _T("Refresh World Info"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_START_PROCESS), _T("Start Process"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_STOP_PROCESS), _T("Stop Process"));
	m_ToolTip.AddTool(m_pPartitionDlg->GetDlgItem(IDC_COMBO_PROCESSTYPE), _T("Process Type"));
	

	// Timer
	SetTimer(EV_TMD_REQUESTSERVICEINFO, g_TimerInterval[EV_TMD_REQUESTSERVICEINFO - EV_TIMERID_DEF], NULL);
	SetTimer(EV_TMD_REFRESHSERVICESTATUS, g_TimerInterval[EV_TMD_REFRESHSERVICESTATUS - EV_TIMERID_DEF], NULL);

}


void CBasePartitionView::CreateAllView ()
{

	CRect aRect;
	GetClientRect(&aRect);

	aRect.top += PDS_BOTTOM;

	// Image List
	{
		VERIFY(m_ImageList[EV_ILT_SMALL].Create(IDB_IMG_SMALL, 16, 11, RGB(255,0,255)));
		VERIFY(m_ImageList[EV_ILT_LARGE].Create(IDB_IMG_LARGE, 32, 11, RGB(255,0,255)));
	}
	
	if (!m_MainTabView.Create(this, aRect, 
		WS_VISIBLE | WS_CHILD | TCS_FOCUSNEVER | TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_HOTTRACK, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create tabview container m_MainTabView\n");
		return;
	}

	m_MainTabView.SetOffsetExternal(0);	// 탭 뷰와 부모 프레임 사이의 간격
	m_MainTabView.SetOffsetInternal(0);	// 탭 뷰와 자식 뷰와 사이의 간격
	m_MainTabView.SetImageList(&m_ImageList[EV_ILT_SMALL]);

	// Monitor ----------------------------------------------------------------------------------
	if (!m_MainTabView.AddPage(RUNTIME_CLASS(CBaseMonitorView), m_pCreateContext, _T("Monitor"), EV_IML_MONITOR))
	{
		TRACE0("Failed to add page m_MainTabView ←CBaseMonitorView\n");
		return;
	}

	INT nIndex = 0;
	
	nIndex = m_MainTabView.GetPageCount();	// 가장뒤에추가되므로인덱스가됨
	if(nIndex <= 0) return;

	m_pMonitorView = static_cast<CBaseMonitorView*>(m_MainTabView.GetPage(nIndex - 1));
	if (!m_pMonitorView)
		return;

	m_pMonitorView->Set(nIndex - 1);
	
	// Information ----------------------------------------------------------------------------------
	if (!m_MainTabView.AddPage(RUNTIME_CLASS(CBaseInformationView), m_pCreateContext, _T("Information"), EV_IML_INFORMATION)) {
		TRACE0("Failed to add page m_MainTabView ←CBaseInformationView\n");
		return;
	}

	nIndex = m_MainTabView.GetPageCount();	// 가장뒤에추가되므로인덱스가됨
	if(nIndex <= 0) return;

	m_pInformationView = static_cast<CBaseInformationView*>(m_MainTabView.GetPage(nIndex - 1));
	if (!m_pInformationView)
		return;

	m_pInformationView->Set(nIndex - 1);

	// Error Log ----------------------------------------------------------------------------------
	if (!m_MainTabView.AddPage(RUNTIME_CLASS(CBaseErrorLogView), m_pCreateContext, _T("Error Log"), EV_IML_ERRORLOG)) {
		TRACE0("Failed to add page m_MainTabView ←CBaseErrorLogView\n");
		return;
	}

	nIndex = m_MainTabView.GetPageCount();	// 가장뒤에추가되므로인덱스가됨
	if(nIndex <= 0) return;

	m_pErrorLogView = static_cast<CBaseErrorLogView*>(m_MainTabView.GetPage(nIndex - 1));
	if(!m_pErrorLogView) return;

	m_pErrorLogView->Set(nIndex - 1);

	m_MainTabView.SetActivePageIndex(0);

	CMainFrame* aMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();
	if(!aMainFrame) return;
	aMainFrame->ForceAllTabViewActivate();
}

DWORD CBasePartitionView::GetTickTerm(DWORD dwOldTick, DWORD dwCurTick)
{
	return((dwCurTick >= dwOldTick)?(dwCurTick - dwOldTick):(ULONG_MAX - dwOldTick + dwCurTick));
}

void CBasePartitionView::OnRequestServiceInfo (CNetSession* pSession)
{
	if (!pSession)
		return;

	// 서비스 구조요청 (예외사항대비)
	BOOL aDoReq = FALSE;

	{
		CServiceInfo::TP_LOCKAUTO AutoLock(pSession->GetServiceInfo()->GetLock());

		if (pSession->GetServiceInfo()->IsRecvTick(EV_SRT_STRUCT) && EV_REQUESTSERVICESTRUCT_TICKTERM < pSession->GetServiceInfo()->GetRecvTickTerm(EV_SRT_STRUCT))
		{
			pSession->GetServiceInfo()->SetRecvTick(EV_SRT_STRUCT);		// 필요없음?

			aDoReq = TRUE;
		}
	}
	
	if (aDoReq) 
	{
		CNetSession* pNetSession = m_NetConnection->GetSession();
		m_NetConnection->SendRequestServiceStruct(pNetSession, -1);
	}

	//------------------------------------------------------------------------------------------------------------------------------------------

	// 서비스실시간정보요청
	
	aDoReq = FALSE;
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(pSession->GetServiceInfo()->GetLock());

		if (pSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT) && !pSession->GetServiceInfo()->GetRecvMode(EV_SRT_REALTIME))
		{
			pSession->GetServiceInfo()->SetRecvMode(EV_SRT_REALTIME, TRUE);

			aDoReq = TRUE;
		}

	}

	if (aDoReq) 
	{
		CNetSession* pNetSession = m_NetConnection->GetSession();
		TSMRealTimeReq TxPacket;
		TxPacket.nContainetVersion = pSession->GetServiceInfo()->GetContainerVersion();
		pNetSession->SendData(MONITOR2MANAGER_REALTIME_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));
	}
}

void CBasePartitionView::OnRefreshServiceStatus (CNetSession* pSession)
{
	if (!pSession)
		return;
	//------------------------------------------------------------------------------------------------------------------------------------------
	// EDIT
	CServiceInfo::TP_LISTSERVAUTO aListServ;
	if (pSession->GetServiceInfo()->GetRecvMode(EV_SRT_STRUCT)) 
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(pSession->GetServiceInfo()->GetLock());

		pSession->GetServiceInfo()->GetServerList(EV_SMT_REAL, aListServ,  EV_WORLDID_ALL, FALSE);
	}

	INT aTotalUserCount = 0;
	INT aTotalServerCount = 0;
	INT aEachServerCount[EV_SVT_CNT] = { 0, };

	if (!aListServ.empty()) 
	{
		CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
		for (; aListServ.end() != aIt ; ++aIt) 
		{
			const SERVERINFO* aServerInfo = (&(*aIt));
			if (!aServerInfo) return;
			if(!aServerInfo->IsSet()) return;

			if (!CHECK_RANGE(aServerInfo->m_ServerType, EV_SVT_NONE + 1, EV_SVT_CNT - 1))
				BASE_CONTINUE;
		
			++aEachServerCount[aServerInfo->m_ServerType];
			++aTotalServerCount;

			switch(aServerInfo->m_ServerType) 
			{
			case EV_SVT_LO:	// Login
			case EV_SVT_VI:	// Village
			case EV_SVT_GA:	// Game
					aTotalUserCount += aServerInfo->m_CurUserCount;
				break;
			default:
				break;
			}
		}
	}

	m_pPartitionDlg->GetDlgItem(IDC_MDB_TOTAL_USERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aTotalUserCount));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_TOTAL_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aTotalServerCount));
	/*
	m_pPartitionDlg->GetDlgItem(IDC_MDB_LO_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_LO]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_MA_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_MA]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_LG_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_LG]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_VI_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_VI]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_GA_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_GA]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_DB_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_DB]));
	m_pPartitionDlg->GetDlgItem(IDC_MDB_CA_SERVERCOUNT)->SetWindowText(CVarArg<MAX_PATH>(_T("%d"), aEachServerCount[EV_SVT_CA]));
	*/

	//------------------------------------------------------------------------------------------------------------------------------------------
	// BUTTON(s)


	// CONNECTION
	CButton* pButtonConnection = static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_CONNECTBUTTON));
	if (!pSession->IsConnect())
	{
		if ((m_TabIconTick[EV_SBT_CONNECT]++) % 2) 
			pButtonConnection->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NETDSCN1)));
		else
		{
			pButtonConnection->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NETDSCN2)));
			pSession->Reconnect();
		}
	}
	else
		pButtonConnection->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NETCONN)));


	// ERROR LOG STATUS
	CButton* pButtonError = static_cast<CButton*>(m_pPartitionDlg->GetDlgItem(IDC_MDB_ERRORLOGBUTTON));
	if (m_pErrorLogView->IsAlert())
	{
		if ((m_TabIconTick[EV_SBT_ERRORLOG]++) % 2) 
			pButtonError->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ALERT)));
		else
			pButtonError->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ERRORLOG)));
	}
	else
		pButtonError->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ERRORLOG)));
}

CRect CBasePartitionView::GetFrameViewRect()
{
	CRect aRect;
	GetClientRect(&aRect);

	return aRect;
}

BOOL CBasePartitionView::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT_VALID(this);

	m_ToolTip.RelayEvent(pMsg);

	return CScrollView::PreTranslateMessage(pMsg);
}

void CBasePartitionView::Close()
{
	m_NetConnection->Close ();

	for (int i=0; i<m_MainTabView.GetPageCount(); i++)
		m_MainTabView.DeletePage(0);
}

void CBasePartitionView::Start (CNetConnection* pConnection)
{
	m_NetConnection = pConnection;

	m_pPartitionDlg->SetSession(pConnection->GetSession());
	m_pMonitorView->SetPartitionView (this);
	m_pErrorLogView->SetPartitionView (this);
}

void CBasePartitionView::SetConnectionID (ULONG nID)
{
	m_ConnectionID = nID;
}

ULONG CBasePartitionView::GetConnectionID ()
{
	return m_ConnectionID;
}

CNetConnection* CBasePartitionView::GetNetConnection ()
{
	return m_NetConnection;
}

CBaseMonitorView* CBasePartitionView::GetMoniterView ()
{
	return m_pMonitorView;
}

CBaseInformationView* CBasePartitionView::GetInformationView ()
{
	return m_pInformationView;
}

CBaseErrorLogView* CBasePartitionView::GetErrorLogView ()
{
	return m_pErrorLogView;
}

void CBasePartitionView::SetPageImageIndex(int pPageIndex, int pImageIndex)
{
	m_MainTabView.SetPageImageIndex(pPageIndex, pImageIndex);
}

int CBasePartitionView::GetActivePageIndex() const
{
	return(m_MainTabView.GetActivePageIndex());
}

BOOL CBasePartitionView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return CScrollView::PreCreateWindow(cs);
}

void CBasePartitionView::SetActiveErrorLog ()
{
	if (!m_IsSized || 0 >= m_MainTabView.GetPageCount())
		return;

	if (m_pErrorLogView)
		m_MainTabView.SetActivePageIndex(m_pErrorLogView->GetPageIndex());
}

void CBasePartitionView::Refreshworldinfo()
{
	CNetSession* pSession = m_NetConnection->GetSession();

	CServiceInfo* pServiceInfo = pSession->GetServiceInfo();

	BOOL aDoReq = FALSE;
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(pServiceInfo->GetLock());

		if (pServiceInfo->GetRecvMode(EV_SRT_STRUCT) && !pServiceInfo->GetRecvMode(EV_SRT_REALTIME) 
			&& EV_REFRESHWORLDVIEW_TICKTERM < GetTickTerm(m_RefreshWorldInfoTick, ::GetTickCount()))
		{
			pServiceInfo->SetRecvMode(EV_SRT_REALTIME, TRUE);

			m_RefreshWorldInfoTick = ::GetTickCount();

			aDoReq = TRUE;
		}
	}
	
	if (aDoReq) 
	{
		TSMRealTimeReq TxPacket;
		TxPacket.nContainetVersion = pSession->GetServiceInfo()->GetContainerVersion();
		pSession->SendData(MONITOR2MANAGER_REALTIME_REQ, 0, reinterpret_cast<CHAR*>(&TxPacket), sizeof(TxPacket));
	}
}

void CBasePartitionView::ShowReportStatus(int nPage, int nMaxPage)
{
	m_pPartitionDlg->SetDlgItemText(IDC_STATIC_REPORTSTATUS, CVarArg<MAX_PATH>(_T("Download : %d / %d"), nPage, nMaxPage));
}


BOOL CBasePartitionView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}

void CBasePartitionView::OnDraw(CDC* /*pDC*/)
{
	CDocument* pDoc = GetDocument();
	UNUSED_ALWAYS(pDoc);

	// TODO: 여기에 그리기 코드를 추가합니다.
}


void CBasePartitionView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

}