// DnEndingBannerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DnEndingBanner.h"
#include "DnEndingBannerDlg.h"
#include "DnUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDnEndingBannerDlg 대화 상자

CDnEndingBannerDlg::CDnEndingBannerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnEndingBannerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwWidth = 0;
	m_dwHeight = 0;

	int i = 0;
	for (; i < HTMLVIEW_MAX; ++i)
	{
		m_pDnHtmlView[i] = NULL;
	}
}

void CDnEndingBannerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDnEndingBannerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CDnEndingBannerDlg 메시지 처리기

BOOL CDnEndingBannerDlg::InitSkin()
{
 	CenterWindow();

	return TRUE;
}

BOOL CDnEndingBannerDlg::InitWebPage()
{
	CCreateContext pContext;
	CWnd* pFrameWnd = this;

	pContext.m_pCurrentDoc = NULL;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CDnHtmlView);

	m_pDnHtmlView[HTMLVIEW_MAIN] = (CDnHtmlView*)((CFrameWnd*)pFrameWnd)->CreateView( &pContext );
	if (m_pDnHtmlView[HTMLVIEW_MAIN] == NULL)
		return FALSE;
	m_pDnHtmlView[HTMLVIEW_MAIN]->ShowWindow(SW_SHOW);
	m_pDnHtmlView[HTMLVIEW_MAIN]->MoveWindow(m_CtrlPosData.GetCtrlRect(eDPI_MainWebHtml));
	m_pDnHtmlView[HTMLVIEW_MAIN]->Navigate(MAIN_URL);
	m_pDnHtmlView[HTMLVIEW_MAIN]->SetOwner(this);
	m_pDnHtmlView[HTMLVIEW_MAIN]->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

	return TRUE;
}

#define ID_TIMER 1

BOOL CDnEndingBannerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	this->SetWindowText(_S(IDS_STRING_TITLE));

	InitSkin();

	InitWebPage();

	MoveWindow(m_CtrlPosData.GetCtrlRect(eDPI_MainWebHtml));

	SetTimer(ID_TIMER, 10000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDnEndingBannerDlg::OnPaint()
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
HCURSOR CDnEndingBannerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDnEndingBannerDlg::OnEraseBkgnd( CDC* pDC )
{
	BitBlt(pDC->m_hDC, 0, 0, m_dwWidth, m_dwHeight, m_dcBkGrnd, 0, 0, SRCCOPY);
	return FALSE;
}

void CDnEndingBannerDlg::OnDestroy()
{
}

void CDnEndingBannerDlg::OnClose()
{
	CDialog::OnCancel();
}

void CDnEndingBannerDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ID_TIMER)
		CDialog::OnCancel();
}

BOOL CDnEndingBannerDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
		return true;

	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
		return true;

	return CDialog::PreTranslateMessage(pMsg);
}
