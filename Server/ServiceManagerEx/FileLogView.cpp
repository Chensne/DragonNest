// FileLogView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "FileLogView.h"
#include "LogList.h"

// CFileLogView

IMPLEMENT_DYNCREATE(CFileLogView, CScrollView)

CFileLogView::CFileLogView()
	: m_pList(NULL)
{
}

CFileLogView::~CFileLogView()
{
	delete m_pList;
}

BEGIN_MESSAGE_MAP(CFileLogView, CScrollView)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CFileLogView 그리기입니다.

void CFileLogView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	RECT rect;
	GetClientRect(&rect);

	m_pList = new CLogList(MENU_FILE_LOG);
	m_pList->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(0, 0, rect.right, rect.bottom), this, IDC_LIST_FILELOG);
	m_pList->SetExtendedStyle(LVS_EX_FULLROWSELECT);

	const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"Log");
	if (pViewConfig)
		m_pList->SetBkColor(pViewConfig->GetBgColor());
	else
		m_pList->SetBkColor(RGB(0, 0, 0));
	
#if defined (USE_FONT_BOLD)
	CFont* pFont = m_pList->GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	m_pList->SetFont(pFont);
#endif //#if defined (USE_FONT_BOLD)

	m_pList->InsertColumn(0, L"Date", LVCFMT_LEFT, 200);
	m_pList->InsertColumn(1, L"Type", LVCFMT_LEFT, 150);
	m_pList->InsertColumn(2, L"Log", LVCFMT_LEFT, rect.right - 200 - 150 - 20);
}

void CFileLogView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	UNUSED_ALWAYS(pDoc);
}

// CFileLogView 진단입니다.

#ifdef _DEBUG
void CFileLogView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CFileLogView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFileLogView 메시지 처리기입니다.

void CFileLogView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pList)
		m_pList->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE);
}

bool CFileLogView::OpenFile()
{
	return (m_pList) ? m_pList->OpenFile() : false;
}

void CFileLogView::InsertLog(LogInfo& log)
{
	if (m_pList)
		m_pList->InsertLog(log);
}
