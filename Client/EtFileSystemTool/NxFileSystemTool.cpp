
#include "stdafx.h"
#include "NxFileSystemTool.h"
#include "NxFileSystemToolDlg.h"
#include "FileIOThread.h"
#include ".\NxFileSystemTool.h"
#include "StringUtil.h"
#include "BugReporter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"dbghelp.lib")

/*
#ifdef _DEBUG
#pragma comment( lib, "zlib_MTd.lib" )
#pragma comment( lib, "EtFileSystem_MTd.lib" )
#else
#pragma comment( lib, "zlib_MT.lib" )
#pragma comment( lib, "EtFileSystem_MT.lib" )
#endif
*/

// CNxFileSystemToolApp

BEGIN_MESSAGE_MAP(CNxFileSystemToolApp, CWinApp)
	
END_MESSAGE_MAP()


// CNxFileSystemToolApp 생성

CNxFileSystemToolApp::CNxFileSystemToolApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); // 
	_CrtMemDumpAllObjectsSince(0); 
	//_CrtSetBreakAlloc(15359);


}


// 유일한 CNxFileSystemToolApp 개체입니다.

CNxFileSystemToolApp theApp;


// CNxFileSystemToolApp 초기화

BOOL CNxFileSystemToolApp::InitInstance()
{
	BugReporter::ReportingServerInfo info;
	gs_BugReporter.SetReportingServerInfo(info);
	gs_BugReporter.Enable();


	CString szCmdLine;
	szCmdLine = m_lpCmdLine;
	if ( szCmdLine.IsEmpty() == FALSE )
	{
		tstring szAgrv = m_lpCmdLine;
		std::vector<tstring>	tokens;

#ifdef _UNICODE
		TokenizeW( szAgrv, tokens, _T(" "));
#else
		TokenizeA( szAgrv, tokens, _T(" "));
#endif 
		bool bResult = false;
		std::string str;

#ifdef _UNICODE
		
		ToMultiString(tokens[0], str);
#else
		str = tokens[0];
#endif

		// 파라미터
		// -capa_pak Data 50 패킹할폴더 아웃풋폴더
		// -folder_pak 패킹정보xml 아웃풋폴더
		if ( str == "-capacity_pack")
		{
			return _CapaPacking(tokens);

		}
		else if ( str == "-folder_pack" )
		{
			return _FolderPacking(tokens);
		}
	}


	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성한 응용 프로그램"));

	CNxFileSystemToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 대화 상자가 확인을 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 대화 상자가 취소를 눌러 없어지는 경우 처리할
		// 코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고
	// 응용 프로그램을 끝낼 수 있도록 FALSE를 반환합니다.
	return FALSE;
}


// 용량별 패킹
BOOL	
CNxFileSystemToolApp::_CapaPacking(std::vector<std::string>& tokens)
{
	if ( tokens.size() != 5 )
		return FALSE;
	
	int nCapa = atoi(tokens[2].c_str());
	if ( nCapa < 1 || nCapa > 2000 )
	{
		nCapa = 50;
	}

	DIVPackingCapacityInfo info;
	info.m_szBaseName = tokens[1].c_str();
	info.m_InputPath = tokens[3].c_str();
	info.m_OutputPath = tokens[4].c_str();
	info.nDivCapacity = nCapa;
	g_pDivPackingCapaThread = new CDivPackingCapacityThread(NULL, info);
	g_pDivPackingCapaThread->Start();

	/*
	if  ( ::WaitForSingleObject( g_pDivPackingThread->m_hHandleThread, INFINITE ) == WAIT_OBJECT_0 )
	{
	delete g_pDivPackingThread;
	return TRUE;

	}*/

	if ( g_pDivPackingCapaThread->WaitForTerminate() )
	{
		delete g_pDivPackingCapaThread;
		return TRUE;
	}
	return FALSE;

}

// 폴더로 패킹
BOOL
CNxFileSystemToolApp::_FolderPacking(std::vector<std::string>& tokens)
{
	if ( tokens.size() != 3 )
		return FALSE;

	bool bResult = false;
	std::vector<_PackingInfo> packingInfo;

	std::string str = tokens[1];

	bResult = ParseSettingFile(str.c_str(), packingInfo);

	if ( bResult == false )
		return FALSE;

	g_pDivPackingThread = new CDivPackingThread(NULL, CString(tokens[1].c_str()), packingInfo);
	g_pDivPackingThread->Start();

	/*
	if  ( ::WaitForSingleObject( g_pDivPackingThread->m_hHandleThread, INFINITE ) == WAIT_OBJECT_0 )
	{
	delete g_pDivPackingThread;
	return TRUE;

	}*/

	if ( g_pDivPackingThread->WaitForTerminate() )
	{
		delete g_pDivPackingThread;
		return TRUE;
	}
	return FALSE;
}