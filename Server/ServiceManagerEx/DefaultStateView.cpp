// DefaultStateView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "DefaultStateView.h"
#include "PropertyGrid.h"
#include "ServiceManager.h"

extern CServiceManager* g_pServiceManager;
extern TServiceManagerConfig g_Config;

// CDefaultStateView

IMPLEMENT_DYNCREATE(CDefaultStateView, CView)

CDefaultStateView::CDefaultStateView()
	: m_pProperty(NULL)
{

}

CDefaultStateView::~CDefaultStateView()
{
	delete m_pProperty;
}

BEGIN_MESSAGE_MAP(CDefaultStateView, CView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDefaultStateView 그리기입니다.

void CDefaultStateView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.
	UNUSED_ALWAYS(pDoc);
}


// CDefaultStateView 진단입니다.

#ifdef _DEBUG
void CDefaultStateView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CDefaultStateView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDefaultStateView 메시지 처리기입니다.

void CDefaultStateView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CRect rect;
	GetClientRect(&rect);

	m_pProperty = new CPropertyGrid();
	m_pProperty->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, rect, this, IDC_PROPERTYGRID_DEFAULT_STATE);

	const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"DefaultState");
	if (pViewConfig)
	{
		m_pProperty->SetTitleColor(RGB(0, 0, 0));
		m_pProperty->SetBackColor(pViewConfig->GetBgColor());
		m_pProperty->SetTextColor(pViewConfig->GetDefaultFontColor());
		m_pProperty->SetDisabledColor(pViewConfig->GetDefaultFontColor());
	}
	else
	{
		m_pProperty->SetTitleColor(RGB(0, 0, 0));
		m_pProperty->SetBackColor(RGB(0, 0, 0));
		m_pProperty->SetTextColor(RGB(192, 192, 192));
		m_pProperty->SetDisabledColor(RGB(192, 192, 192));
	}
}

void CDefaultStateView::RefreshView()
{
	if (!m_pProperty)
		return;

	m_pProperty->ResetContents();

	// service manager info
	HSECTION section = m_pProperty->AddSection(L"ServiceManager Information");
	m_pProperty->AddStringItem(section, L"Region", g_Config.wszRegion, false);
	m_pProperty->AddIntegerItem(section, L"NetLauncher Port", g_Config.nLauncherPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Service Port", g_Config.nServicePort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Service Patcher Port", g_Config.nServicePatcherPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"GSM Port", g_Config.nGSMPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Monitor Port", g_Config.nMonitorPort, L"", false);
	m_pProperty->AddStringItem(section, L"GSM IP", g_Config.wszGSMIP, false);
	m_pProperty->AddStringItem(section, L"GSM CodePage", g_Config.wszGSMCodePage, false);
	m_pProperty->AddStringItem(section, L"Patch Base URL", g_Config.wszPatchBaseURL, false);
	m_pProperty->AddStringItem(section, L"Patch URL", g_Config.wszPatchURL, false);
	m_pProperty->AddStringItem(section, L"Patch Dir", g_Config.wszPatchDir, false);
	m_pProperty->AddStringItem(section, L"Notice Filename", g_Config.wszNoticePath, false);
	m_pProperty->ExpandSection(section, true);

	// server info
	const TDefaultServerInfo& serverInfo = g_pServiceManager->GetDefaultServerInfo();
	section = m_pProperty->AddSection(L"Default Server Information");
	m_pProperty->AddIntegerItem(section, L"Socket Max", serverInfo.nSocketMax, L"", false);
	m_pProperty->AddIntegerItem(section, L"DB Port", serverInfo.nDBMiddleWareAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Login Master Port", serverInfo.nLoginMasterAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Login Client Port", serverInfo.nLoginClientAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Master Village Port", serverInfo.nMasterVillageAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Master Game Port", serverInfo.nMasterGameAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Village Client Port", serverInfo.nVillageClientAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Game Client UDP Port", serverInfo.nGameClientAcceptUDPPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Game Client TCP Port", serverInfo.nGameClientAcceptTCPPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Cash Port", serverInfo.nCashAcceptPort, L"", false);
	m_pProperty->AddIntegerItem(section, L"Log Port", serverInfo.nLogServerAcceptPort, L"", false);
	m_pProperty->ExpandSection(section, false);

	//Doors 서버 정보가 있을때만 출력해준다
	if(CDataManager::GetInstance()->GetDoorsPort() > 0)
	{
		section = m_pProperty->AddSection(L"Doors Server Information");
		m_pProperty->AddStringItem(section, L"DoorsServer IP", CDataManager::GetInstance()->GetDoorsIP(), false);
		m_pProperty->AddIntegerItem(section, L"DoorsServer Port", CDataManager::GetInstance()->GetDoorsPort(), L"", false);
		m_pProperty->ExpandSection(section, false);
	}

	if (!serverInfo.vDolbyAxonInfo.empty())
	{
		const TDolbyAxon& dolby = serverInfo.vDolbyAxonInfo[0];
		section = m_pProperty->AddSection(L"DolbyAxon Server Information");
		m_pProperty->AddIntegerItem(section, L"C Port", dolby.nDolbyCPort, L"", false);
		m_pProperty->AddIntegerItem(section, L"A Port", dolby.nDolbyAPort, L"", false);
		m_pProperty->AddStringItem(section, L"Public IP", dolby.wszPublicDolbyIP, false);
		m_pProperty->AddStringItem(section, L"Private IP", dolby.wszPrivateDolbyIP, false);
		m_pProperty->AddIntegerItem(section, L"World ID", dolby.nDolbyWorldID, L"", false);
		m_pProperty->ExpandSection(section, false);
	}

	// database info
	const TDefaultDatabaseInfo& dbInfo = g_pServiceManager->GetDefaultDatabaseInfo();
	section = m_pProperty->AddSection(L"MemberShip DataBase Information");
	m_pProperty->AddIntegerItem(section, L"Port", dbInfo.MemberShipDB.nPort, L"", false);
	m_pProperty->AddStringItem(section, L"IP", dbInfo.MemberShipDB.wszIP, false);
	m_pProperty->AddStringItem(section, L"Name", dbInfo.MemberShipDB.wszDBName, false);
	m_pProperty->AddStringItem(section, L"ID", dbInfo.MemberShipDB.wszDBID, false);
	m_pProperty->ExpandSection(section, false);

	for each (map<int, TSQLConnectionInfo>::value_type v in dbInfo.WorldDB)
	{
		const TSQLConnectionInfo info = v.second;
		section = m_pProperty->AddSection(L"World DataBase Information");
		m_pProperty->AddIntegerItem(section, L"Port", info.nPort, L"", false);
		m_pProperty->AddStringItem(section, L"IP", info.wszIP, false);
		m_pProperty->AddStringItem(section, L"Name", info.wszDBName, false);
		m_pProperty->AddStringItem(section, L"ID", info.wszDBID, false);
		m_pProperty->AddIntegerItem(section, L"World ID", v.first, L"", false);
		m_pProperty->ExpandSection(section, false);
	}

	section = m_pProperty->AddSection(L"Log DataBase Information");
	m_pProperty->AddIntegerItem(section, L"Port", dbInfo.ServerLogDB.nPort, L"", false);
	m_pProperty->AddStringItem(section, L"IP", dbInfo.ServerLogDB.wszIP, false);
	m_pProperty->AddStringItem(section, L"Name", dbInfo.ServerLogDB.wszDBName, false);
	m_pProperty->AddStringItem(section, L"ID", dbInfo.ServerLogDB.wszDBID, false);
	m_pProperty->ExpandSection(section, false);

	// service manager info ex
	const ConfigEx& config = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx();
	section = m_pProperty->AddSection(L"ServiceManagerEx Information");
	m_pProperty->AddIntegerItem(section, L"Log Count", (int)config.maxLogCount);
	m_pProperty->AddIntegerItem(section, L"Command History Count", (int)config.cmdHistoryCount);
	m_pProperty->AddIntegerItem(section, L"Log Days", (int)config.serverReportDays);
	m_pProperty->AddStringItem(section, L"Log Detail", (config.serverReportDetail) ? L"true" : L"false");
	m_pProperty->AddIntegerItem(section, L"Log Except Size", (int)config.exceptLogSize);
	m_pProperty->ExpandSection(section, true);
}

void CDefaultStateView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_pProperty)
		m_pProperty->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE);
}
