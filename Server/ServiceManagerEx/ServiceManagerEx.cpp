// ServiceManagerEx.cpp : 览侩 橇肺弊伐俊 措茄 努贰胶 悼累阑 沥狼钦聪促.
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

// 览侩 橇肺弊伐 沥焊俊 荤侩登绰 CAboutDlg 措拳 惑磊涝聪促.
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 措拳 惑磊 单捞磐涝聪促.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 瘤盔涝聪促.

	// 备泅涝聪促.
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
	// 钎霖 颇老阑 扁檬肺 窍绰 巩辑 疙飞涝聪促.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 钎霖 牢尖 汲沥 疙飞涝聪促.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

// CServiceManagerExApp 积己

CServiceManagerExApp::CServiceManagerExApp()
	: m_pLogDisplayer(NULL), m_hMutex(NULL)
{
	// TODO: 咯扁俊 积己 内靛甫 眠啊钦聪促.
	// InitInstance俊 葛电 吝夸茄 檬扁拳 累诀阑 硅摹钦聪促.
}

CServiceManagerExApp::~CServiceManagerExApp()
{

}

// 蜡老茄 CServiceManagerExApp 俺眉涝聪促.

CServiceManagerExApp theApp;

int CServiceManagerExApp::ExitInstance(){
	//肺弊 免仿何磐 老窜 阜绊 矫累
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

// CServiceManagerExApp 檬扁拳
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

	// 览侩 橇肺弊伐 概聪其胶飘啊 ComCtl32.dll 滚傈 6 捞惑阑 荤侩窍咯 厚林倔 胶鸥老阑
	// 荤侩窍档废 瘤沥窍绰 版快, Windows XP 惑俊辑 馆靛矫 InitCommonControlsEx()啊 鞘夸钦聪促. 
	// InitCommonControlsEx()甫 荤侩窍瘤 臼栏搁 芒阑 父甸 荐 绝嚼聪促.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 览侩 橇肺弊伐俊辑 荤侩且 葛电 傍侩 牧飘费 努贰胶甫 器窃窍档废
	// 捞 亲格阑 汲沥窍绞矫坷.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE 扼捞宏矾府甫 檬扁拳钦聪促.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 钎霖 檬扁拳
	// 捞甸 扁瓷阑 荤侩窍瘤 臼绊 弥辆 角青 颇老狼 农扁甫 临捞妨搁
	// 酒贰俊辑 鞘夸 绝绰 漂沥 檬扁拳
	// 风凭阑 力芭秦具 钦聪促.
	// 秦寸 汲沥捞 历厘等 饭瘤胶飘府 虐甫 函版窍绞矫坷.
	// TODO: 捞 巩磊凯阑 雀荤 肚绰 炼流狼 捞抚苞 鞍篮
	// 利例茄 郴侩栏肺 荐沥秦具 钦聪促.
	SetRegistryKey(_T("肺拿 览侩 橇肺弊伐 付过荤俊辑 积己等 览侩 橇肺弊伐"));
	LoadStdProfileSettings(4);  // MRU甫 器窃窍咯 钎霖 INI 颇老 可记阑 肺靛钦聪促.
	// 览侩 橇肺弊伐狼 巩辑 袍敲复阑 殿废钦聪促. 巩辑 袍敲复篮
	//  巩辑, 橇饭烙 芒 棺 轰 荤捞狼 楷搬 开且阑 钦聪促.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CServiceManagerExDoc),
		RUNTIME_CLASS(CMainFrame),       // 林 SDI 橇饭烙 芒涝聪促.
		RUNTIME_CLASS(CServiceManagerExView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	
	// 钎霖 夹 疙飞, DDE, 颇老 凯扁俊 措茄 疙飞临阑 备巩 盒籍钦聪促.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 疙飞临俊 瘤沥等 疙飞阑 叼胶菩摹钦聪促.
	// 览侩 橇肺弊伐捞 /RegServer, /Register, /Unregserver 肚绰 /Unregister肺 矫累等 版快 FALSE甫 馆券钦聪促.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	if (!InitServiceManager())
		return FALSE;

	//Sleep(1000);

	if (!CreateDisplayer())
		return FALSE;

	BuildSchedule();

	BuildView();
	RefreshStateView();

	// 芒 窍唱父 檬扁拳登菌栏骨肺 捞甫 钎矫窍绊 诀单捞飘钦聪促.
	m_pMainWnd->SetWindowTextW(L"ServiceManagerEx");
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 立固荤啊 乐阑 版快俊父 DragAcceptFiles甫 龋免钦聪促.
	//  SDI 览侩 橇肺弊伐俊辑绰 ProcessShellCommand 饶俊 捞矾茄 龋免捞 惯积秦具 钦聪促.

	return TRUE;
}

bool CServiceManagerExApp::LoadConfig()
{
	wstring wszFileName = L"./Config/DNServiceManager.ini";			// 茄惫
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

// 措拳 惑磊甫 角青窍扁 困茄 览侩 橇肺弊伐 疙飞涝聪促.
void CServiceManagerExApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CServiceManagerExApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 咯扁俊 漂荐拳等 内靛甫 眠啊 棺/肚绰 扁夯 努贰胶甫 龋免钦聪促.

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
