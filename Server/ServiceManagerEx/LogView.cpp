// LogView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "LogView.h"
#include "LogList.h"
#include "SMConfig.h"
#include "Commander.h"

// CLogView
class CLogViewDisplayer;

IMPLEMENT_DYNCREATE(CLogView, CScrollView)

BEGIN_MESSAGE_MAP(CLogView, CScrollView)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CLogView::CLogView()
	: m_pList(NULL), m_pCmdLine(NULL), m_pCmdBtn(NULL)
{

}

CLogView::~CLogView()
{
	SAFE_DELETE(m_pList);
	SAFE_DELETE(m_pCmdLine);
	SAFE_DELETE(m_pCmdBtn);
	m_Logs.clear();
}

// CLogView �׸����Դϴ�.

void CLogView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: �� ���� ��ü ũ�⸦ ����մϴ�.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	RECT rect;
	GetClientRect(&rect);

	m_pList = new CLogList(MENU_LOG);
	m_pList->Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, CRect(0, 0, rect.right, rect.bottom - 20), this, IDC_LIST_LOG);
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

	FlushLog();

	m_pCmdLine = new CCommander();
	m_pCmdLine->Create(WS_CHILD | WS_BORDER | WS_VISIBLE, CRect(0, rect.bottom - 20, rect.right - 100, rect.bottom), this, IDC_EDIT_COMMAND);
	m_pCmdLine->ModifyStyle(0, ES_WANTRETURN);

	m_pCmdBtn = new CButton();
	m_pCmdBtn->Create(L"Command", WS_CHILD | WS_VISIBLE, CRect(rect.right - 100, rect.bottom - 20, rect.right, rect.bottom), this, IDC_BUTTON_COMMAND);
}

void CLogView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
	UNUSED_ALWAYS(pDoc);
}

// CLogView �����Դϴ�.

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLogView �޽��� ó�����Դϴ�.

void CLogView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_pList)
		m_pList->SetWindowPos(NULL, 0, 0, cx, cy - 20, SWP_NOMOVE);

	if (m_pCmdLine)
		m_pCmdLine->SetWindowPos(NULL, 0, cy - 20, cx - 100, 20, 0);

	if (m_pCmdBtn)
		m_pCmdBtn->SetWindowPos(NULL, cx - 100, cy - 20, 0, 0, SWP_NOSIZE);
}

BOOL CLogView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch (wParam)
	{
	case IDC_BUTTON_COMMAND:
		m_pCmdLine->OnCommand();
		break;
	}

	return CScrollView::OnCommand(wParam, lParam);
}

void CLogView::InsertLog(LogInfo& log)
{
	if (!m_pList)
	{
		m_Logs.push_back(log);
		return;
	}

	const ConfigEx& config = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx();
	size_t count = m_pList->GetItemCount();
	if (count >= config.maxLogCount)
	{
		for (size_t i = 0; i <= count - config.maxLogCount; ++i)
			m_pList->Pop();
	}

	m_pList->InsertLog(log);
}

void CLogView::FlushLog()
{
	for (vector<LogInfo>::iterator it = m_Logs.begin(); it != m_Logs.end(); ++it)
	{
		InsertLog(*it);
	}
	m_Logs.clear();
}

void CLogView::SetCommand(const wstring& command)
{
	m_pCmdLine->SetCommand(command);
	m_pCmdLine->SetFocus();
}