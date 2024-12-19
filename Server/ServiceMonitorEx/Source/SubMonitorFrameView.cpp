// SubMonitorFrameView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "BasePartitionView.h"
#include "ServiceInfo.h"
#include "SubMonitorFrameView.h"
#include "SubMonitorWorldView.h"
#include "ConfigMngr.h"
#include <MMSystem.h>	// PlaySound



// CSubMonitorFrameView

IMPLEMENT_DYNCREATE(CSubMonitorFrameView, CFormView)

CSubMonitorFrameView::CSubMonitorFrameView()
	: CFormView(CSubMonitorFrameView::IDD)
{
	m_IsInitialized = FALSE;
	m_IsSized = FALSE;
	m_ServiceInfoUpdateNo = EV_SVCINFOUPD_DEF;
	m_PlayAlert = FALSE;
	m_hAccel = NULL;
	m_pPartitionView = NULL;

	DWORD aRetVal = m_Lock.Open();
	if (NOERROR != aRetVal) {
		return;
	}

	if (!::IsWindow(m_ControlDialog.GetSafeHwnd())) 
		m_ControlDialog.Create(IDD_CTRLDLG, this);

	
}

CSubMonitorFrameView::~CSubMonitorFrameView()
{
	if (::IsWindow(GetSafeHwnd())) {
		if (m_hAccel) {
			
			m_hAccel = NULL;
		}
	}

	m_Lock.Close();

	::PlaySound(NULL, ::AfxGetInstanceHandle(), SND_ASYNC);
}

void CSubMonitorFrameView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSubMonitorFrameView, CFormView)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_PICKALLCURVIEWSERVER, &CSubMonitorFrameView::OnPickAllCurViewServer)
	ON_COMMAND(ID_REFRESHWORLDINFO, &CSubMonitorFrameView::OnRefsCurWorldView)
END_MESSAGE_MAP()


// CSubMonitorFrameView 진단입니다.

#ifdef _DEBUG
void CSubMonitorFrameView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSubMonitorFrameView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


BOOL CSubMonitorFrameView::RebuildWorldView()
{
	if (!m_pPartitionView)
		return FALSE;

	CServiceInfo::TP_LISTWRLDAUTO aListWrld;
	{
		CServiceInfo::TP_LOCKAUTO AutoLock(m_pServiceInfo->GetLock());

		m_pServiceInfo->GetWorldList(EV_SMT_REAL, aListWrld, FALSE);
	}

	if (aListWrld.empty()) {
		return FALSE;
	}

	ClearAllWorldView();

	AddWorldView(EV_WORLDID_ALL);	// 전체 정보 뷰는 기본 추가

	CServiceInfo::TP_LISTWRLDAUTO_CTR aIt = aListWrld.begin();
	for (; aListWrld.end() != aIt ; ++aIt) {
		const WORLDINFO* aWorldInfo = (&(*aIt));
		if (!aWorldInfo) continue;
		if(!aWorldInfo->IsSet()) continue;

		AddWorldView(aWorldInfo->m_WorldID);
	}

	// COX3DTabViewContainer 잠김 방지
	for (int aIndex = 0 ; m_SubTabView.GetPageCount() > aIndex ; ++aIndex)
		m_SubTabView.SetActivePageIndex(aIndex);
	m_SubTabView.SetActivePageIndex(0);

	m_pPartitionView->ForceAllTabViewActivate();

	return TRUE;
}

void CSubMonitorFrameView::AddWorldView(INT nWorldID)
{
	TCHAR aWorldName[MAX_PATH] = { _T('\0'), };

	if (EV_WORLDID_ALL == nWorldID) {
		_sntprintf_s(aWorldName, _countof(aWorldName), _T("All Servers"));
	}
	else {
		_sntprintf_s(aWorldName, _countof(aWorldName), _T("World %d"), nWorldID);
	}

	CCreateContext aCreateContext;
	::memset(&aCreateContext, 0, sizeof(aCreateContext));
	aCreateContext.m_pNewViewClass = RUNTIME_CLASS(CSubMonitorWorldView);

	if (!m_SubTabView.AddPage(RUNTIME_CLASS(CSubMonitorWorldView), &aCreateContext, aWorldName, 1)) {
		TRACE0("Failed to add page m_SubTabView ← CSubMonitorFrameView\n");
		return;		// fail to create
	}

	INT aIndex = m_SubTabView.GetPageCount();	// 가장 뒤에 추가되므로 인덱스가 됨
	if (aIndex <= 0 )
		return;


	CSubMonitorWorldView* aSubMonitorWorldView = static_cast<CSubMonitorWorldView*>(m_SubTabView.GetPage(aIndex - 1));
	if (!aSubMonitorWorldView)
		return;
	

	aSubMonitorWorldView->Set(nWorldID, aIndex - 1);
	aSubMonitorWorldView->SetBaseInfo(m_pPartitionView, this, m_pServiceInfo);

	m_pPartitionView->GetPartitionDlg()->AddProcessType(nWorldID);

}

void CSubMonitorFrameView::ClearAllWorldView()
{
	int aPageCount = m_SubTabView.GetPageCount();

	for (int aIndex = 0 ; aPageCount > aIndex ; ++aIndex) {
		m_SubTabView.DeletePage(0);	// 페이지 삭제 시 인덱스 번호가 앞으로 밀리므로 계속 0 번 인덱스를 삭제
	}
}

void CSubMonitorFrameView::SetBaseInfo(CBasePartitionView* pView)
{
	m_pPartitionView = pView;
	m_pServiceInfo = pView->GetServiceInfo();

	// 세션 세팅
	m_ControlDialog.SetSession (m_pPartitionView->GetNetConnection()->GetSession());

}

int CSubMonitorFrameView::GetActivePageIndex() const
{
	return(m_SubTabView.GetActivePageIndex());
}

void CSubMonitorFrameView::SetPageImageIndex(int pPageIndex, int pImageIndex)
{
	m_SubTabView.SetPageImageIndex(pPageIndex, pImageIndex);
}


// CSubMonitorFrameView 메시지 처리기입니다.

void CSubMonitorFrameView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_IsInitialized) {
		m_IsInitialized = TRUE;

		// Image List
		{
			VERIFY(m_ImageList[EV_ILT_SMALL].Create(IDB_IMG_SMALL, 16, 11, RGB(255,0,255)));
			VERIFY(m_ImageList[EV_ILT_LARGE].Create(IDB_IMG_LARGE, 32, 11, RGB(255,0,255)));
		}

		// Tab View
		{
			if (!m_SubTabView.Create(this, aRect, 
				WS_VISIBLE | WS_CHILD | TCS_FOCUSNEVER | TCS_MULTILINE | TCS_FIXEDWIDTH | TCS_HOTTRACK,
				AFX_IDW_PANE_FIRST
				))
			{
				TRACE0("Failed to create tabview container m_SubTabView\n");
				return;		// fail to create
			}

			m_SubTabView.SetOffsetExternal(0);	// 탭 뷰와 부모 프레임 사이의 간격
			m_SubTabView.SetOffsetInternal(0);	// 탭 뷰와 자식 뷰와 사이의 간격
			m_SubTabView.SetImageList(&m_ImageList[EV_ILT_SMALL]);

			// COX3DTabViewContainer 잠김 방지
			for (int aIndex = 0 ; m_SubTabView.GetPageCount() > aIndex ; ++aIndex) {
				m_SubTabView.SetActivePageIndex(aIndex);
			}
			m_SubTabView.SetActivePageIndex(0);

		}

	
		SetTimer(EV_TMD_CHECKSERVERSTRUCT, g_TimerInterval[EV_TMD_CHECKSERVERSTRUCT - EV_TIMERID_DEF], NULL);
		SetTimer(EV_TMD_CHECKSERVERSTATE, g_TimerInterval[EV_TMD_CHECKSERVERSTATE - EV_TIMERID_DEF], NULL);

		m_hAccel = ::LoadAccelerators(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINACCEL));
		ASSERT(NULL != m_hAccel);
	}
	
}

BOOL CSubMonitorFrameView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (m_hAccel) {
		if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
			return TRUE;
		}
	}
	return CScrollView::PreTranslateMessage(pMsg);
}


void CSubMonitorFrameView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect aRect;
	GetClientRect(&aRect);

	if (m_IsInitialized) 
	{
		m_SubTabView.MoveWindow(&aRect, TRUE);

		if (!m_IsSized)
			m_IsSized = TRUE;
	}
}

void CSubMonitorFrameView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(EV_TMD_CHECKSERVERSTRUCT);
	KillTimer(EV_TMD_CHECKSERVERSTATE);
}

void CSubMonitorFrameView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch(nIDEvent) {
	case EV_TMD_CHECKSERVERSTRUCT:
		{
			LONG64 aServiceInfoUpdateNo = m_pServiceInfo->GetServiceInfoUpdateNo();
			if (GetServiceInfoUpdateNo() != aServiceInfoUpdateNo) {
				BOOL aRetVal = RebuildWorldView();
				if (aRetVal) {
					SetServiceInfoUpdateNo(aServiceInfoUpdateNo);
				}
			}
		}
		break;

	case EV_TMD_CHECKSERVERSTATE:
		{
			CServiceInfo::TP_LISTSERVAUTO aListServ;
			{
				CServiceInfo::TP_LOCKAUTO AutoLock(m_pServiceInfo->GetLock());

				m_pServiceInfo->GetServerList(EV_SMT_REAL, aListServ, EV_WORLDID_ALL, FALSE);
			}

			BOOL aIsServerTerminated = FALSE;

			CServiceInfo::TP_LISTSERVAUTO_CTR aIt = aListServ.begin();
			for (; aListServ.end() != aIt ; ++aIt) {
				const SERVERINFO* aServerInfo = static_cast<const SERVERINFO*>(&(*aIt));
				if (!aServerInfo) return;
				if(!aServerInfo->IsSet()) return;

				if (EV_SVS_TMNT == aServerInfo->m_ServerState) {
					aIsServerTerminated = TRUE;
				}
			}

			if (aIsServerTerminated) {
				if (!m_PlayAlert) {
					m_PlayAlert = TRUE;
					if (!CConfigMngr::GetInstancePtr()->m_WavFile.IsEmpty()) {
						::PlaySound(CConfigMngr::GetInstancePtr()->m_WavFile, ::AfxGetInstanceHandle(), SND_FILENAME | SND_ASYNC | SND_LOOP);
					}
				}
			}
			else {
				if (m_PlayAlert) {
					m_PlayAlert = FALSE;
					::PlaySound(NULL, ::AfxGetInstanceHandle(), SND_ASYNC);
				}
			}
		}
		break;

	default:
		break;
	}


	CFormView::OnTimer(nIDEvent);
}

void CSubMonitorFrameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch(nChar) {
	case VK_ESCAPE:
		{
			if (0 >= m_SubTabView.GetPageCount()) {
				break;
			}

			CSubMonitorWorldView* aSubMonitorWorldView = static_cast<CSubMonitorWorldView*>(m_SubTabView.GetActivePage());
			if (!aSubMonitorWorldView) {
				break;
			}
			
			aSubMonitorWorldView->PickServerAll(FALSE);
		}
		break;
	}

	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSubMonitorFrameView::OnPickAllCurViewServer()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	if (0 >= m_SubTabView.GetPageCount()) {
		return;
	}

	CSubMonitorWorldView* aSubMonitorWorldView = static_cast<CSubMonitorWorldView*>(m_SubTabView.GetActivePage());
	if (!aSubMonitorWorldView) {
		return;
	}
	
	aSubMonitorWorldView->PickServerAll(TRUE);
}

void CSubMonitorFrameView::OnRefsCurWorldView()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 월드 정보 갱신
	m_pPartitionView->GetPartitionDlg()->OnBnClickedMdbRefreshworldinfo();
}
