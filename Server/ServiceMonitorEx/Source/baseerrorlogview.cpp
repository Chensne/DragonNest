// BaseErrorLogView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "BaseErrorLogView.h"
#include "BasePartitionView.h"

#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CBaseErrorLogView, CBaseView)

CBaseErrorLogView::CBaseErrorLogView()
	: CBaseView(CBaseErrorLogView::IDD)
{
	m_IsInitialized = FALSE;

	m_IsClose = FALSE;

	for (int aIndex = 0 ; EV_ELT_CNT > aIndex ; ++aIndex) {
		m_OldGridListSubProc[aIndex] = NULL;
	}

	m_TabIconTick = 0;
	m_DoAlert = FALSE;

	m_Report.str(L"");
}

CBaseErrorLogView::~CBaseErrorLogView()
{
	m_IsClose = TRUE;
}

void CBaseErrorLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MONERRITELIST, m_GridList[EV_ELT_ITE]);
	DDX_Control(pDX, IDC_MONERRECPLIST, m_GridList[EV_ELT_ECP]);
}

BEGIN_MESSAGE_MAP(CBaseErrorLogView, CFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ERRITELISTCLEAR, &CBaseErrorLogView::OnBnClickedErrIteListClear)
	ON_BN_CLICKED(IDC_ERRECPLISTCLEAR, &CBaseErrorLogView::OnBnClickedErrEcpListClear)
	ON_WM_PAINT()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MONERRITELIST, OnCustomDrawErrIteList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MONERRECPLIST, OnCustomDrawErrEcpList)
END_MESSAGE_MAP()


// CBaseErrorLogView 진단입니다.

#ifdef _DEBUG
void CBaseErrorLogView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CBaseErrorLogView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CBaseErrorLogView::DrawTitle(CDC* pDC)
{
	if (!pDC)
		return;

	Graphics aGraphics((*pDC));
	aGraphics.SetSmoothingMode(SmoothingModeHighQuality);

	CRect aRect;
	GetClientRect(&aRect);

	// Draw Title
	{
		// Service Monitor Internal Error Log
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						aRect.left + EV_UIS_TITLELINE_PAD_X,
						aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
						aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X),
						EV_UIS_TITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					aRect.left + EV_UIS_TITLELINE_PAD_X,
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
					aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2 + 1) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X),
					EV_UIS_TITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X, 
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y - EV_UIS_TITLELINE_PAD_Y,
					aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X),
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y + EV_UIS_TITLELINE_HGT + EV_UIS_TITLELINE_PAD_Y
					);

				pDC->DrawText(_T("Service Monitor Internal Error Log"), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}

		// Service Exception Log
		{
			// Gradient Line
			{
				LinearGradientBrush aBrush(
					Rect(
						aRect.left + EV_UIS_TITLELINE_PAD_X,
						aRect.top + EV_UIS_ERRORLOGITEVIEW_HGT + (EV_UIS_ERRORLOGECPVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
						aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X),
						EV_UIS_TITLELINE_HGT
					),
					Color(100, 221, 221, 221),
					Color(255, 120, 120, 120),
					180.0f,
					FALSE
					);

				aGraphics.FillRectangle(
					&aBrush, 
					aRect.left + EV_UIS_TITLELINE_PAD_X,
					aRect.top + EV_UIS_ERRORLOGITEVIEW_HGT + (EV_UIS_ERRORLOGECPVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
					aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2 + 1) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X),
					EV_UIS_TITLELINE_HGT
					);
			}

			// Draw Text
			{
				LOGFONT aLogFont;
				::memset(&aLogFont, 0x00, sizeof(aLogFont));
				_tcsncpy_s(aLogFont.lfFaceName, _T("Arial"), LF_FACESIZE);
				aLogFont.lfCharSet = DEFAULT_CHARSET;
				aLogFont.lfHeight = 20;
				aLogFont.lfWeight = FW_EXTRABOLD;

				CFont aNewFont;
				VERIFY(aNewFont.CreateFontIndirect(&aLogFont));
				CFont* aOldFont = pDC->SelectObject(&aNewFont);

				COLORREF aOldTxColor = pDC->SetTextColor(RGB(255, 255, 255));
				int aOldBkMode = pDC->SetBkMode(TRANSPARENT);

				CRect aRect(
					EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X, 
					aRect.top + EV_UIS_ERRORLOGITEVIEW_HGT + (EV_UIS_ERRORLOGECPVIEW_PAD_Y * 2) - EV_UIS_TITLELINE_PAD_Y,
					aRect.right - ((EV_UIS_ERRORLOGVIEW_PAD_A * 2) + EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_TITLELINE_PAD_X + EV_UIS_TITLETEXT_PAD_X),
					aRect.top + EV_UIS_ERRORLOGITEVIEW_HGT + (EV_UIS_ERRORLOGECPVIEW_PAD_Y * 2) + EV_UIS_TITLELINE_HGT + EV_UIS_TITLELINE_PAD_Y
					);

				pDC->DrawText(_T("Service Exception Log"), &aRect, DT_LEFT | DT_VCENTER);

				pDC->SetTextColor(aOldTxColor);
				pDC->SetBkMode(aOldBkMode);

				pDC->SelectObject(aOldFont);
				aNewFont.DeleteObject();
			}
		}
	}

	
}

void CBaseErrorLogView::AddGridList(INT pErrorCode, LPCTSTR pMsg, BOOL pDoAlert)
{
	if(!pMsg) return;

	CListCtrl* aGridList = &m_GridList[EV_ELT_ITE];
	if(!aGridList) return;

	BeginWaitCursor();
	aGridList->LockWindowUpdate();

	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= aGridList->GetItemCount();
	aLvItem.iSubItem	= 0;

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Date
	SYSTEMTIME aSystemTime;
	::GetLocalTime(&aSystemTime);

	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%04d-%02d-%02d %02d:%02d:%02d"), 
		aSystemTime.wYear,
		aSystemTime.wMonth,
		aSystemTime.wDay,
		aSystemTime.wHour,
		aSystemTime.wMinute,
		aSystemTime.wSecond
		);
	aLvItem.pszText = sBuffer;

	int nRtnValue = aGridList->InsertItem(&aLvItem);
	if(-1 == nRtnValue) return;

	// Error Code
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"), pErrorCode);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Message
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), pMsg);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	aGridList->UnlockWindowUpdate();
	EndWaitCursor();

	// Grid List 의 가장 마지막 행까지 스크롤
	CRect aRect;
	aGridList->GetItemRect(aGridList->GetItemCount() - 1, &aRect, LVIR_BOUNDS);
	aGridList->Scroll(CSize(0, aRect.bottom));

	if (pDoAlert) 
	{
		m_DoAlert = TRUE;

		CMainFrame* pMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();
		if(pMainFrame)
			pMainFrame->SetAlert(TRUE);
	}
}

void CBaseErrorLogView::AddGridList(INT pServerID, CHAR pServerType, INT nWorldID[WORLDCOUNTMAX], EF_SERVERSTATE pServerState, INT pExceptionCode, LPCTSTR pDetail, BOOL pDoAlert)
{
	if (pServerID == EV_SERVERID_DEF)
		return;

	if (!CHECK_LIMIT(pServerState, EV_SVS_CNT))
		return;

	if(!nWorldID)
		return;

	if(!pDetail)
		return;

	CListCtrl* aGridList = &m_GridList[EV_ELT_ECP];

	if (!aGridList)
		return;

	BeginWaitCursor();
	//aGridList->LockWindowUpdate();

	LV_ITEM aLvItem;
	aLvItem.mask		= LVIF_TEXT | LVIF_PARAM;
	aLvItem.cchTextMax	= 0;
	aLvItem.lParam		= 0;
	aLvItem.iItem		= aGridList->GetItemCount();
	aLvItem.iSubItem	= 0;
	aLvItem.lParam		= pServerID;	// 추후 빠른 정보 검색을 위해 파라메터로 지정

	TCHAR sBuffer[MAX_PATH*2] = { _T('\0'), };

	// Date
	SYSTEMTIME aSystemTime;
	::GetLocalTime(&aSystemTime);

	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%04d-%02d-%02d %02d:%02d:%02d"), 
		aSystemTime.wYear,
		aSystemTime.wMonth,
		aSystemTime.wDay,
		aSystemTime.wHour,
		aSystemTime.wMinute,
		aSystemTime.wSecond
		);
	aLvItem.pszText = sBuffer;

	int nRtnValue = aGridList->InsertItem(&aLvItem);
	if(-1 == nRtnValue)
		return;

	// Server ID
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d"),  pServerID);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Server Type
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"),  (CHECK_LIMIT(pServerType, EV_SVT_CNT))?(g_ServerTypeFullName[pServerType]):(_T("N/A")));
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// World ID
	{
		if (EV_WORLDID_DEF != nWorldID[0]) {
			sBuffer[0] = _T('\0');
			for (int aIndex = 0 ; WORLDCOUNTMAX > aIndex ; ++aIndex) {
				if (EV_WORLDID_DEF == nWorldID[aIndex]) {
					break;
				}
				if (0 < aIndex) {
					_tcscat_s(sBuffer, _T(", "));
				}
				_tcscat_s(sBuffer, CVarArg<MAX_PATH>(_T("%d"), nWorldID[aIndex]));
			}
		}
		else {
			_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), _T("N/A"));
		}
		aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);
	}

	// Server State
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d (%s)"), pServerState, (CHECK_LIMIT(pServerState, EV_SVS_CNT)?(g_ServerStateName[pServerState]):(_T("N/A"))));
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Exception Code
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%d (%s)"), pExceptionCode, g_ExceptionReportName[pExceptionCode]);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	// Detail
	_sntprintf_s(sBuffer, _countof(sBuffer), _T("%s"), pDetail);
	aGridList->SetItemText(aLvItem.iItem, ++aLvItem.iSubItem, sBuffer);

	//aGridList->UnlockWindowUpdate();
	EndWaitCursor();
	
	// Grid List 의 가장 마지막 행까지 스크롤
	CRect aRect;
	aGridList->GetItemRect(aGridList->GetItemCount() - 1, &aRect, LVIR_BOUNDS);
	aGridList->Scroll(CSize(0, aRect.bottom));

	if (pDoAlert) {
		m_DoAlert = TRUE;

		CMainFrame* aMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();
		if (aMainFrame)
			aMainFrame->SetAlert(TRUE);
	}
}

void CBaseErrorLogView::ShowOperatingResult (INT nCmd, INT nRet)
{
	CString ResultText;

	if (nRet == ERROR_NONE)
		ResultText += CVarArg<MAX_PATH>(_T("Success [Command:%d]\n"), nCmd);
	else
		ResultText += CVarArg<MAX_PATH>(_T("Error [Command:%d Result:%d]\n"), nCmd, nRet);

	::AfxMessageBox(ResultText);
}


// CBaseErrorLogView 메시지 처리기입니다.

int CBaseErrorLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.

	return 0;
}

void CBaseErrorLogView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CRect aRect;
	GetClientRect(&aRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (m_IsInitialized) {
		m_LayoutManager.OnSize(cx,cy);

		// Draw Title
		{
			CClientDC aDC(this);

			DrawTitle(&aDC);
		}
	}
}

void CBaseErrorLogView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CRect aRect;
	GetClientRect(&aRect);

	if (!m_IsInitialized) {
		m_IsInitialized = TRUE;
		
		// Image List
		{
			VERIFY(m_ImageList.Create(IDB_IMG_SMALL, 16, 11, RGB(255,0,255)));
		}

		// Grid Control
		{
			// Service Monitor Internal Error Log
			{
				int aColNo = 0;

				CListCtrl* aGridList = &m_GridList[EV_ELT_ITE];
				if(!aGridList)
					return;

				aGridList->InsertColumn(aColNo++, _T("Date"), LVCFMT_LEFT, 140);
 				aGridList->InsertColumn(aColNo++, _T("Error Code"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Message"), LVCFMT_LEFT, 1300);

				aGridList->SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);


				aGridList->MoveWindow(
					aRect.left + EV_UIS_ERRORLOGVIEW_PAD_A,
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y,
					aRect.right - (EV_UIS_ERRORLOGVIEW_PAD_A * 2),
					EV_UIS_ERRORLOGITEVIEW_HGT,
					TRUE
					);
			}

			// Service Exception Log
			{
				int aColNo = 0;

				CListCtrl* aGridList = &m_GridList[EV_ELT_ECP];
				if(!aGridList)
					return;

				aGridList->InsertColumn(aColNo++, _T("Date"), LVCFMT_LEFT, 140);
				aGridList->InsertColumn(aColNo++, _T("Server ID"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Server Type"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("World ID"), LVCFMT_LEFT, 80);
				aGridList->InsertColumn(aColNo++, _T("Server State"), LVCFMT_LEFT, 100);
				aGridList->InsertColumn(aColNo++, _T("Exception Code"), LVCFMT_LEFT, 200);
				aGridList->InsertColumn(aColNo++, _T("Detail"), LVCFMT_LEFT, 840);

				aGridList->SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);

				aGridList->MoveWindow(
					aRect.left + EV_UIS_ERRORLOGVIEW_PAD_A,
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y + EV_UIS_ERRORLOGITEVIEW_HGT + EV_UIS_ERRORLOGECPVIEW_PAD_Y,
					aRect.right - (EV_UIS_ERRORLOGVIEW_PAD_A * 2),
					aRect.bottom - (aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y + EV_UIS_ERRORLOGITEVIEW_HGT + EV_UIS_ERRORLOGECPVIEW_PAD_Y + EV_UIS_ERRORLOGVIEW_PAD_A),
					TRUE
					);
			}

			// Button(s)
			{
				// Service Monitor Internal Error Log
				GetDlgItem(IDC_ERRITELISTCLEAR)->MoveWindow(
					aRect.right - (EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_ERRORLOGVIEW_PAD_A),
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y - (EV_UIS_ERRORLOGCLEARBUTTON_HGT + EV_UIS_ERRORLOGCLEARBUTTON_GAP),
					EV_UIS_ERRORLOGCLEARBUTTON_WTH,
					EV_UIS_ERRORLOGCLEARBUTTON_HGT,
					TRUE
					);

				// Service Exception Log
				GetDlgItem(IDC_ERRECPLISTCLEAR)->MoveWindow(
					aRect.right - (EV_UIS_ERRORLOGCLEARBUTTON_WTH + EV_UIS_ERRORLOGVIEW_PAD_A),
					aRect.top + EV_UIS_ERRORLOGITEVIEW_PAD_Y + EV_UIS_ERRORLOGITEVIEW_HGT + EV_UIS_ERRORLOGECPVIEW_PAD_Y - (EV_UIS_ERRORLOGCLEARBUTTON_HGT + EV_UIS_ERRORLOGCLEARBUTTON_GAP),
					EV_UIS_ERRORLOGCLEARBUTTON_WTH,
					EV_UIS_ERRORLOGCLEARBUTTON_HGT,
					TRUE
					);
			}
		}

		// Layout Manager
		{
			m_LayoutManager.Attach(this);
 			m_LayoutManager.TieChild(IDC_MONERRITELIST, OX_LMS_TOP | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
 			m_LayoutManager.TieChild(IDC_MONERRECPLIST, OX_LMS_TOP | OX_LMS_BOTTOM | OX_LMS_LEFT | OX_LMS_RIGHT, OX_LMT_SAME);
			m_LayoutManager.TieChild(IDC_ERRITELISTCLEAR, OX_LMS_RIGHT, OX_LMT_SAME);
			m_LayoutManager.TieChild(IDC_ERRECPLISTCLEAR, OX_LMS_RIGHT, OX_LMT_SAME);
		}

		SetTimer(EV_TMD_SHOWERRORLOG, g_TimerInterval[EV_TMD_SHOWERRORLOG - EV_TIMERID_DEF], NULL);

	}

}

void CBaseErrorLogView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnLButtonDown(nFlags, point);
}

BOOL CBaseErrorLogView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	cs.style &= ~(WS_HSCROLL | WS_VSCROLL);

	return CFormView::PreCreateWindow(cs);
}

void CBaseErrorLogView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_IsClose)
		return;

	switch(nIDEvent) 
	{
		case EV_TMD_SHOWERRORLOG:
		{
			
			if (m_DoAlert) {
				CBasePartitionView* pPartitionView = GetPartitionView();
				if (!pPartitionView)
					return;

				if (pPartitionView->GetActivePageIndex() == GetPageIndex()) {
					m_DoAlert = FALSE;
					pPartitionView->SetPageImageIndex(GetPageIndex(), EV_IML_ERRORLOG);
				}
				else {
					if ((++m_TabIconTick) % 2) {
						pPartitionView->SetPageImageIndex(GetPageIndex(), EV_IML_ALERT);
					}
					else {
						pPartitionView->SetPageImageIndex(GetPageIndex(), EV_IML_ERRORLOG);
					}
				}
			}
			
		}
		break;
		default:
		break;
	}

	CFormView::OnTimer(nIDEvent);
}

void CBaseErrorLogView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

	KillTimer(EV_TMD_SHOWERRORLOG);
}

void CBaseErrorLogView::OnBnClickedErrIteListClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (IDOK != ::AfxMessageBox(_T("Are you sure to clear list ?"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2)) {
		return;
	}

	ClearGridList(EV_ELT_ITE);
}

void CBaseErrorLogView::OnBnClickedErrEcpListClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (IDOK != ::AfxMessageBox(_T("Are you sure to clear list ?"), MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2)) {
		return;
	}

	ClearGridList(EV_ELT_ECP);
}

void CBaseErrorLogView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CBaseView::OnPaint()을(를) 호출하지 마십시오.

	DrawTitle(&dc);
}

void CBaseErrorLogView::OnCustomDrawErrIteList(NMHDR* pNMHDR, LRESULT* pResult)
{
	const LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lplvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_DODEFAULT;
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}   
}

void CBaseErrorLogView::OnCustomDrawErrEcpList(NMHDR* pNMHDR, LRESULT* pResult)
{
	const LPNMLVCUSTOMDRAW lplvcd = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lplvcd->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_DODEFAULT;

			TCHAR aItemText[64] = { _T('\0'), };

			CListCtrl* aGridList = &m_GridList[EV_ELT_ECP];
			if(!aGridList)
				return;

			// Exception Code
			{
				aGridList->GetItemText(static_cast<int>(lplvcd->nmcd.dwItemSpec), 5, aItemText, _countof(aItemText));
				INT aExceptionCode = static_cast<INT>(_ttoi(aItemText));

				if (!aExceptionCode) {
					lplvcd->clrText		= g_ServiceExceptionLogColor[0][0];
					lplvcd->clrTextBk	= g_ServiceExceptionLogColor[0][1];
				}
				else {
					lplvcd->clrText		= g_ServiceExceptionLogColor[1][0];
					lplvcd->clrTextBk	= g_ServiceExceptionLogColor[1][1];
				}
			}

			// Server State
			{
				aGridList->GetItemText(static_cast<int>(lplvcd->nmcd.dwItemSpec), 4, aItemText, _countof(aItemText));
				EF_SERVERSTATE aServerState = static_cast<EF_SERVERSTATE>(_ttoi(aItemText));

				switch(aServerState) {
				case  EV_SVS_TMNT:
					{
						lplvcd->clrText		= g_ServiceExceptionLogColor[2][0];
						lplvcd->clrTextBk	= g_ServiceExceptionLogColor[2][1];
					}
					break;
				}
			}
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}   
}

void CBaseErrorLogView::SetPartitionView(CBasePartitionView* pView)
{
	m_pPartitionView = pView;
}

CBasePartitionView* CBaseErrorLogView::GetPartitionView()
{
	return m_pPartitionView;
}

void CBaseErrorLogView::RecvReportData(const wchar_t* szReport)
{
	m_Report << szReport;
}

void CBaseErrorLogView::SaveToClipboard()
{
	const std::wstring&report = m_Report.str();

	if (!OpenClipboard())
		return;

	if (!EmptyClipboard())
		return;
	
	size_t size = report.length() * sizeof(wchar_t);

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size + sizeof(wchar_t));
	if (!hMem)
	{
		CloseClipboard();
	
		return;
	}

	wchar_t* pBuf = (wchar_t*)GlobalLock(hMem);
	::memcpy(pBuf, report.c_str(), size);
	GlobalUnlock(hMem);

	if (!::SetClipboardData(CF_UNICODETEXT, hMem))
	{
		CloseClipboard();
		return;
	}

	CloseClipboard();

	m_Report.str(L"");

	//::AfxMessageBox(_T("Ready to use clipboard!"), MB_ICONINFORMATION);


	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = L"notepad.exe";
	sei.hwnd = NULL;
	sei.lpVerb = L"open";
	sei.nShow = SW_SHOW;
	ShellExecuteEx(&sei);

	Sleep(100);
	
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	
	
}