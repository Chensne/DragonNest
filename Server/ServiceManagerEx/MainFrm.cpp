// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "ServiceManagerEx.h"

#include "MainFrm.h"
#include "ServiceManagerExDoc.h"
#include "Log.h"

#include "FileLogView.h"

#include "CommandListDlg.h"
#include "PreparePatchDlg.h"
#include "SendMailDlg.h"
#include "ServerViewType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_MENU_PATCH_URL, &CMainFrame::OnMenuPatchUrl)
	ON_COMMAND(ID_MENU_LOG, &CMainFrame::OnMenuLog)
	ON_COMMAND(ID_EXCEPTION_REPORT_TO_EMAIL, &CMainFrame::OnMenuExceptionReportToEMail)
	ON_COMMAND(ID_EXCEPTION_REPORT_TO_CLIPBOARD, &CMainFrame::OnMenuExceptionReportToClipboard)
	ON_COMMAND(ID_MENU_WORLD_VIEW, &CMainFrame::OnMenuWorldView)
	ON_COMMAND(ID_MENU_LAUNCHER_VIEW, &CMainFrame::OnMenuLauncherView)
	ON_COMMAND(ID_MENU_LOAD_CONFIG, &CMainFrame::OnMenuLoadConfig)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
	: m_pFileLogView(NULL)
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}

	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
	/*
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	*/

	CMenu* pViewMenu = GetMenu()->GetSubMenu(2);
	pViewMenu->CheckMenuRadioItem(ID_MENU_WORLD_VIEW, ID_MENU_LAUNCHER_VIEW, ID_MENU_WORLD_VIEW, MF_BYCOMMAND);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	const FrameConfig* pConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetFrameConfig(L"MainFrame");
	if (pConfig)
	{
		cs.cx = pConfig->size.x;
		cs.cy = pConfig->size.y;
	}
	else
	{
		cs.cx = 800;
		cs.cy = 600;
	}

	return TRUE;
}

// CMainFrame ����

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

// CMainFrame �޽��� ó����
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	CCreateContext context;
	context.m_pNewViewClass = RUNTIME_CLASS(CFileLogView);
	m_pFileLogView = (CFileLogView*)CreateView(&context);
	m_pFileLogView->ShowWindow(SW_HIDE);

	RECT rect;
	GetClientRect(&rect);

	m_LogSplit.CreateSplit(this, rect, pContext);

	int paneId = m_LogSplit.IdFromRowCol(0, 0);
	m_MainSplit.CreateSplit(&m_LogSplit, paneId, rect, pContext);

	paneId = m_MainSplit.IdFromRowCol(0, 1);
	m_StateSplit.CreateSplit(&m_MainSplit, paneId, rect, pContext);

	return TRUE;
}

void CMainFrame::OnMenuPatchUrl()
{
	CPreparePatchDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnMenuLog()
{
	if (m_pFileLogView->IsWindowVisible())
	{
		m_MainSplit.ShowWnd(true);
		m_LogSplit.ShowWnd(true);
		m_pFileLogView->ShowWindow(SW_HIDE);
		SetWindowText(L"ServiceManagerEx");
	}
	else
	{
		if (m_pFileLogView->OpenFile() == false)
			return;

		m_MainSplit.ShowWnd(false);
		m_LogSplit.ShowWnd(false);

		RECT rect;
		GetClientRect(&rect);
		m_pFileLogView->SetWindowPos(NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE);
		m_pFileLogView->ShowWindow(SW_SHOWNORMAL);
	}
}

void CMainFrame::OnMenuCommand()
{
	CCommandListDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	const std::wstring& command = dlg.GetCommand();
	m_LogSplit.SetCommand(command);
}

void CMainFrame::OnMenuWorldView()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMenu* pViewMenu = GetMenu()->GetSubMenu(2);
	pViewMenu->CheckMenuRadioItem(ID_MENU_WORLD_VIEW, ID_MENU_LAUNCHER_VIEW, ID_MENU_WORLD_VIEW, MF_BYCOMMAND);

	m_MainSplit.SetManagerViewType(Logical);
}

void CMainFrame::OnMenuLauncherView()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMenu* pViewMenu = GetMenu()->GetSubMenu(2);
	pViewMenu->CheckMenuRadioItem(ID_MENU_WORLD_VIEW, ID_MENU_LAUNCHER_VIEW, ID_MENU_LAUNCHER_VIEW, MF_BYCOMMAND);

	m_MainSplit.SetManagerViewType(Physical);
}

void CMainFrame::OnMenuExceptionReportToEMail()
{
	CSendMailDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnMenuExceptionReportToClipboard()
{
	((CServiceManagerExApp*)AfxGetApp())->ReportExceptionToClipboard();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if (m_pFileLogView && m_pFileLogView->IsWindowVisible())
	{
		RECT rect;
		GetClientRect(&rect);
		m_pFileLogView->SetWindowPos(NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE);
	}
}

void CMainFrame::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (IDYES != ::AfxMessageBox(L"Are you sure to quit?", MB_ICONQUESTION | MB_YESNO))
		return;

	CFrameWnd::OnClose();
}

void CMainFrame::BuildView()
{
	m_MainSplit.BuildView();
}

void CMainFrame::RefreshView()
{
	m_MainSplit.RefreshView();
}

void CMainFrame::RefreshStateView()
{
	m_StateSplit.RefreshView();
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CFrameWnd::OnSysCommand(nID, lParam);

	if (nID == SC_RESTORE || nID == SC_MAXIMIZE)
	{
		RECT rect;
		GetClientRect(&rect);
		m_LogSplit.ResizeView(rect);
		m_MainSplit.ResizeView(rect);
		m_StateSplit.ResizeView(rect);
	}
}

void CMainFrame::OnMenuLoadConfig()
{
	((CServiceManagerExApp*)::AfxGetApp())->ReloadConfigEx();
	g_Log.Log(LogType::_NORMAL, L"ServiceManagerEx config loaded.\n");
	m_StateSplit.RefreshView();
	m_MainSplit.RefreshView();
}
