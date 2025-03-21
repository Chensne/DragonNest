// MakeTorrent.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "MakeTorrent.h"
#include "MakeTorrentDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMakeTorrentApp

BEGIN_MESSAGE_MAP(CMakeTorrentApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMakeTorrentApp 생성

CMakeTorrentApp::CMakeTorrentApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
//	_CrtSetBreakAlloc( 152 );
}


// 유일한 CMakeTorrentApp 개체입니다.

CMakeTorrentApp theApp;


// CMakeTorrentApp 초기화

BOOL CMakeTorrentApp::InitInstance()
{
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

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	LPCWSTR cmdLine = GetCommandLine();
	int nArgNum = 0;
	LPWSTR* argv = CommandLineToArgvW( cmdLine, &nArgNum );

	bool bMake = false;
	if( argv && nArgNum > 1)
	{
		LoadConfig();
		
		if( nArgNum == 2 )
		{
			m_strMakeFileName = argv[1];
		}
		else if( nArgNum == 3 )
		{
			m_strWebSeeds = argv[2];
		}
		else if( nArgNum == 4 )
		{
			m_strTrackers = argv[3];
		}

		LocalFree( argv );
		return MakeTorrent( m_strMakeFileName.c_str(), m_strWebSeeds.c_str(), m_strTrackers.c_str() );
	}

	CMakeTorrentDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

void CMakeTorrentApp::LoadConfig()
{
	FILE *fp;
	fopen_s( &fp, "MakeTorrent.cfg", "rt" );
	if( fp == NULL ) return;

	char szTemp[1024];
	TCHAR szWTemp[1024];
	
	memset( szTemp, 0, sizeof(szTemp) );
	memset( szWTemp, 0, sizeof(szWTemp) );

	fread( szTemp, 1, 1024, fp );

	MultiByteToWideChar( CP_ACP, 0, szTemp, -1, szWTemp, sizeof(TCHAR)*1024 );
	std::wstring wstrTemp = szWTemp;

	std::vector<std::wstring> tokens;
	TokenizeW( wstrTemp, tokens, std::wstring(L"\r\n") );

	LPCTSTR strMakeFileName = L"makefilename:";
	LPCTSTR strWebSeeds = L"webseeds:";
	LPCTSTR strTrackers = L"trackers:";

	for( int i=0; i<(int)tokens.size(); i++ )
	{
		tokens[i].erase( remove_if( tokens[i].begin(), tokens[i].end(), iswspace ), tokens[i].end() );

		if( NULL != StrStr(tokens[i].c_str(), strMakeFileName ) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			m_strMakeFileName = tokens[i];
		}
		else if( NULL != StrStr(tokens[i].c_str(), strWebSeeds ) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			m_strWebSeeds = tokens[i];
		}
		else if( NULL != StrStr(tokens[i].c_str(), strTrackers) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			m_strTrackers = tokens[i];
		}
	}

	tokens.clear();
	fclose( fp );
}