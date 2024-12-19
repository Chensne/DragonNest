// ServiceManagerEx.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "MainFrm.h"
#include "ServiceManagerExDoc.h"
#include "ServiceManagerExView.h"

#include "ServiceServer.h"
#include "Log.h"
#include "DataManager.h"
#include "DNBAM.h"
#include "IniFile.h"
#include "Version.h"

#include "MainSplit.h"
#include "LogViewDisplayer.h"
#include "LogBuilder.h"
#include "LogSplit.h"
#include "Scheduler.h"
#include "ServerReporter.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern TServiceManagerConfig g_Config;
extern CServiceServer * g_pServiceServer;

extern CLog g_Log;
extern CLog s_CountLog;
extern CLog g_ExceptionReportLog;
extern CLog g_DBDelayLog;
extern CLog s_ExcuteLog;
extern CLog g_GameDelayLog;
extern CLog g_VillageDelayLog;
extern CLog g_DBErrorLog;
extern CLog g_DBSystemErrorLog;
extern CLog g_MonitorLog;
extern CLog g_FileLog;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CServiceManagerExApp

BEGIN_MESSAGE_MAP(CServiceManagerExApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CServiceManagerExApp::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// CServiceManagerExApp 생성

CServiceManagerExApp::CServiceManagerExApp()
	: m_pLogDisplayer(NULL), m_hMutex(NULL)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CServiceManagerExApp::~CServiceManagerExApp()
{

}

// 유일한 CServiceManagerExApp 개체입니다.

CServiceManagerExApp theApp;

int CServiceManagerExApp::ExitInstance(){
	//로그 출력부터 일단 막고 시작
	g_Log.SetDisplayer(NULL);
	
	m_GSMServer.Close();
	if(g_pServiceServer)
		g_pServiceServer->Close();
	
	SAFE_DELETE(g_pServiceServer);
	SAFE_DELETE(m_pLogDisplayer);

	m_Scheduler.Clear();
	m_ConfigEx.Reset();
	::ReleaseMutex(m_hMutex);
	return CWinApp::ExitInstance();
}

// CServiceManagerExApp 초기화
BOOL CServiceManagerExApp::InitInstance()
{
	m_hMutex = ::CreateMutex(NULL, TRUE, L"ServiceManagerEx");
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		::AfxMessageBox(L"ServiceManagerEx is already running.");
		return FALSE;
	}

	SetMiniDump();
	LoadConfigEx();
	CLogBuilder::Initialize();

	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.
	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CServiceManagerExDoc),
		RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
		RUNTIME_CLASS(CServiceManagerExView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	if (!InitServiceManager())
		return FALSE;

	Sleep(1000);

	if (!CreateDisplayer())
		return FALSE;

	BuildSchedule();

	BuildView();
	RefreshStateView();

	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.
	m_pMainWnd->SetWindowTextW(L"ServiceManagerEx");
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	//  SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	return TRUE;
}

bool CServiceManagerExApp::LoadConfig()
{
	wstring wszFileName = L"./Config/DNServiceManager.ini";			// 한국
	if (!g_IniFile.Open(wszFileName.c_str())){
		g_Log.Log(LogType::_FILELOG, L"%s File not Found!!\n", wszFileName.c_str());
		return false;
	}
	memset(&g_Config, 0, sizeof(TServiceManagerConfig));

	g_IniFile.GetValue(L"Region", L"RegionInfo", g_Config.wszRegion);

	g_IniFile.GetValue(L"Connection", L"NetLauncherPort", &g_Config.nLauncherPort);
	g_IniFile.GetValue(L"Connection", L"ServicePort", &g_Config.nServicePort);
	g_IniFile.GetValue(L"Connection", L"ServicePatcherPort", &g_Config.nServicePatcherPort);
	g_IniFile.GetValue(L"Connection", L"GSMPort", &g_Config.nGSMPort);
	g_IniFile.GetValue(L"Connection", L"MonitorPort", &g_Config.nMonitorPort);

	g_IniFile.GetValue(L"GSMInfo", L"IP", g_Config.wszGSMIP, IPLENMAX);
	g_IniFile.GetValue(L"GSMInfo", L"CodePage", g_Config.wszGSMCodePage);
	g_IniFile.GetValue(L"PatchInfo", L"BaseUrl", g_Config.wszPatchBaseURL);
	g_IniFile.GetValue(L"PatchInfo", L"PatchUrl", g_Config.wszPatchURL);
	g_IniFile.GetValue(L"PatchInfo", L"PatchDir", g_Config.wszPatchDir);
	g_IniFile.GetValue(L"ReserveNoticeInfo", L"FileName", g_Config.wszNoticePath);
	strcpy_s(g_Config.szVersion, szServiceManagerVersion);

	g_Log.Log(LogType::_FILELOG, L"## LauncherPort:%d, ServicePort:%d GsmPort:%d ESMPort:%d MonitorPort:%d\r\n", g_Config.nLauncherPort, g_Config.nServicePort, g_Config.nGSMPort, g_Config.nServicePatcherPort, g_Config.nMonitorPort);
	g_Log.Log(LogType::_FILELOG, L"## ServiceManager Version : %S\r\n", szServiceManagerVersion);
	if (_access("./System", 0) == -1)
	{
		mkdir("./System");
	}

	return true;
}

void CServiceManagerExApp::LoadConfigEx()
{
	m_ConfigEx.SetDefault();
	m_ConfigEx.LoadConfig();
}

void CServiceManagerExApp::ReloadConfigEx()
{
	LoadConfigEx();
	ClearSchedule();
	BuildSchedule();
}

bool CServiceManagerExApp::CreateDisplayer()
{
	CLogSplit& logSplit = ((CMainFrame*)GetMainWnd())->GetLogSplit();
	m_pLogDisplayer = new CLogViewDisplayer(&logSplit);
	g_Log.SetDisplayer(m_pLogDisplayer);

	return true;
}

bool CServiceManagerExApp::InitServiceManager()
{
	//SetMiniDump();

	//ASSERT(m_pLogDisplayer);
	g_Log.Init(L"ServiceManager", LOGTYPE_CRT_FILE_DAY);
	s_CountLog.Init(L"LogCount", LOGTYPE_FILE_DAY);
	g_ExceptionReportLog.Init(L"ExceptionReport", LOGTYPE_FILE_DAY);
	g_DBDelayLog.Init(L"DBDelay", LOGTYPE_FILE_DAY);
	s_ExcuteLog.Init(L"Excute", LOGTYPE_FILE_DAY);
	g_GameDelayLog.Init(L"GameDelay", LOGTYPE_FILE_DAY);
	g_VillageDelayLog.Init(L"VillageDelay", LOGTYPE_FILE_DAY);
	g_DBErrorLog.Init(L"DBError", LOGTYPE_FILE_DAY);
	g_DBSystemErrorLog.Init(L"DBSystemError", LOGTYPE_FILE_DAY);
	g_MonitorLog.Init(L"MonitorLog", LOGTYPE_FILE_DAY);
	g_FileLog.Init(L"ServiceManager", LOGTYPE_FILE_DAY);

	if (LoadConfig() == false)
		return false;
	
	int nSocketCount = 1000;
	if (!_wcsicmp(g_Config.wszRegion, L"KR") || !_wcsicmp(g_Config.wszRegion, L"KOR") || !_wcsicmp(g_Config.wszRegion, L"DEV"))
		setlocale(LC_ALL, "Korean");
	else if (!_wcsicmp(g_Config.wszRegion, L"CH") || !_wcsicmp(g_Config.wszRegion, L"CHN"))
	{
		setlocale(LC_ALL, "chinese-simplified");
		nSocketCount = 3000;
	}
	else if (!_wcsicmp(g_Config.wszRegion, L"JP") || !_wcsicmp(g_Config.wszRegion, L"JPN"))
		setlocale(LC_ALL, "japanese");
	else if (!_wcsicmp(g_Config.wszRegion, L"US") || !_wcsicmp(g_Config.wszRegion, L"USA"))
		setlocale(LC_ALL, "us");

//#if defined(_KR)
//	g_pBAM = new CDNBAM;
//	if (!g_pBAM) return;
//#endif	// #if defined(_KR)

#ifdef _UNICODE
	g_pServiceManager = new CServiceManager(this);
#else
	char szIP[IPLENMAX], szCode[256];
	WideCharToMultiByte(CP_ACP, 0, g_Config.wszGSMIP, -1, szIP, IPLENMAX, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, g_Config.wszGSMCodePage, -1, szCode, IPLENMAX, NULL, NULL);

	g_pServiceManager = new CServiceManager(szIP, szCode);
#endif
	if (!g_pServiceManager)
		return false;

	//g_pServiceManager->InitializeCommand();

	g_pServiceServer = new CServiceServer();
	if (!g_pServiceServer)
		return false;

	if (!g_pServiceServer->Initialize(nSocketCount, g_Config.nLauncherPort, g_Config.nServicePort, g_Config.nServicePatcherPort, g_Config.nMonitorPort))
	{
		ErrorMessage(L"Service server initialize failed.");
		return false;
	}

	if (!m_GSMServer.Open(g_Config.nGSMPort))
	{
		ErrorMessage(L"Already used GSM port. Program will exit.");
		return false;
	}

	return true;
}

void CServiceManagerExApp::BuildView()
{
	((CMainFrame*)GetMainWnd())->BuildView();
}

void CServiceManagerExApp::RefreshView()
{
	((CMainFrame*)GetMainWnd())->RefreshView();
}

void CServiceManagerExApp::RefreshStateView()
{
	((CMainFrame*)GetMainWnd())->RefreshStateView();
}

BOOL CServiceManagerExApp::OnIdle(LONG lCount)
{
	m_Scheduler.Update();

	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.Update();

	::Sleep(1);

	return TRUE;
}

void CServiceManagerExApp::OnPatchStart()
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnPatchStart();
}

void CServiceManagerExApp::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnPatchProgress(id, key, progress, progressMax);
}

void CServiceManagerExApp::OnPatchEnd(bool succeeded)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnPatchEnd(succeeded);
}

void CServiceManagerExApp::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnUnzipProgress(id, filename, progress, progressMax);
}

void CServiceManagerExApp::OnWorldMaxUser(int id, int maxUser)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnWorldMaxUser(id, maxUser);
}

void CServiceManagerExApp::OnPatchFail(int id, const wchar_t* msg)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnPatchFail(id, msg);
}
void CServiceManagerExApp::OnPatchCompleted(int id)
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnPatchCompleted(id);
}
void CServiceManagerExApp::OnCommandPatch()
{
	CMainSplit& mainSplit = ((CMainFrame*)GetMainWnd())->GetMainSplit();
	mainSplit.OnCommandPatch();
}

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CServiceManagerExApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CServiceManagerExApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinApp::PreTranslateMessage(pMsg);
}

bool CServiceManagerExApp::ReportExceptionToClipboard()
{
	g_Log.Log(LogType::_NORMAL, L"Starting report build to clipboard.\n");

	ServerReport::CServerReporter reporter;

	wstring report;
	reporter.Report(m_ConfigEx.serverReportDays, m_ConfigEx.serverReportDetail, report);

	if (!SaveToClipboard(report))
		return false;

	g_Log.Log(LogType::_NORMAL, L"Copy to Clipboard completed.\n");
	return true;
}

bool CServiceManagerExApp::ReportExceptionToBuffer(size_t days, OUT wstring& buffer)
{
	g_Log.Log(LogType::_NORMAL, L"Starting report build to buffer.\n");

	ServerReport::CServerReporter reporter;
	reporter.Report(days, m_ConfigEx.serverReportDetail, buffer);

	g_Log.Log(LogType::_NORMAL, L"Buffer set completed.\n");
	return true;
}

bool CServiceManagerExApp::ReportExceptionToFile(const wstring& filename)
{
	g_Log.Log(LogType::_NORMAL, L"Starting report build to file.\n");

	ServerReport::CServerReporter reporter;

	wstring report;
	reporter.Report(m_ConfigEx.serverReportDays, m_ConfigEx.serverReportDetail, report);

	HANDLE file = 0;
	try
	{
		file = CreateFile(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			throw;

		DWORD written_size;
		if (!WriteFile(file, report.c_str(), (DWORD)report.size(), &written_size, NULL))
			throw;

		CloseHandle(file);
	}
	catch (...)
	{
		CloseHandle(file);
		g_Log.Log(LogType::_ERROR, L"Copy to file failed.\n");
		return false;
	}

	g_Log.Log(LogType::_NORMAL, L"Copy to file completed.\n");
	return true;
}

bool CServiceManagerExApp::SaveToClipboard(const wstring& report)
{
	if (!OpenClipboard(NULL))
	{
		g_Log.Log(LogType::_ERROR, L"Copy to Clipboard failed. (open)\n");
		return false;
	}

	if (!EmptyClipboard())
	{
		g_Log.Log(LogType::_ERROR, L"Copy to Clipboard failed. (empty)\n");
		return false;
	}

	size_t size = report.length() * sizeof(wchar_t);

	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size + sizeof(wchar_t));
	if (!hMem)
	{
		CloseClipboard();
		g_Log.Log(LogType::_ERROR, L"Copy to Clipboard failed. (alloc)\n");
		return false;
	}

	wchar_t* pBuf = (wchar_t*)GlobalLock(hMem);
	::memcpy(pBuf, report.c_str(), size);
	GlobalUnlock(hMem);

	if (!::SetClipboardData(CF_UNICODETEXT, hMem))
	{
		GlobalFree(hMem);
		CloseClipboard();
		g_Log.Log(LogType::_ERROR, L"Copy to Clipboard failed. (set)\n");
		return false;
	}

	GlobalFree(hMem);
	CloseClipboard();

	return true;
}

void CServiceManagerExApp::BuildSchedule()
{
	CTime curTime = CTime::GetCurrentTime();

	for each (const ScheduleJob* pScheduleJob in m_ConfigEx.schedules)
	{
		SimpleScheduler::CScheduleObj* pScheduleObj = SimpleScheduler::CScheduler::GenerateScheduleObj(this, curTime, pScheduleJob);
		if (!pScheduleObj)
			continue;

		m_Scheduler.Register(pScheduleObj);
	}
}

void CServiceManagerExApp::ClearSchedule()
{
	m_Scheduler.Clear();
}
