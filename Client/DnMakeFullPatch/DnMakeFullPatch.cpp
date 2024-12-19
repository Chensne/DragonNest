// DnMakeFullPatch.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "DnMakeFullPatch.h"
#include "DnMakeFullPatchDlg.h"
#include "LogWnd.h"
#include "BugReporter.h"

#pragma comment( lib, "dbghelp.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDnMakeFullPatchApp

BEGIN_MESSAGE_MAP(CDnMakeFullPatchApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDnMakeFullPatchApp 생성

CDnMakeFullPatchApp::CDnMakeFullPatchApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CDnMakeFullPatchApp 개체입니다.

CDnMakeFullPatchApp theApp;




void TokenizeA( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters /* = " " */ )
{
	// 맨 첫 글자가 구분자인 경우 무시
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	// 구분자가 아닌 첫 글자를 찾는다
	std::string::size_type pos = str.find_first_of( delimiters, lastPos );

	while( std::string::npos != pos || std::string::npos != lastPos )
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back( str.substr( lastPos, pos - lastPos ) );
		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of( delimiters, pos );
		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of( delimiters, lastPos );
	}
}

// CDnMakeFullPatchApp 초기화

BOOL CDnMakeFullPatchApp::InitInstance()
{
	BugReporter::ReportingServerInfo info;
	info.szServerURL = "http://192.168.0.20/bugreport/bugprocess.aspx";
	gs_BugReporter.SetReportingServerInfo(info);
	gs_BugReporter.Enable();

	LogWnd::CreateLog();

	CString szCmdLine;

	szCmdLine = m_lpCmdLine;
	if ( szCmdLine.IsEmpty() == FALSE )
	{
		CDnMakeFullPatchDlg dlg;

		std::vector<std::string>	tokens;
		TokenizeA( std::string(szCmdLine.GetBuffer()), tokens, std::string(" ") );
		if ( tokens.size() != 2 )
			return FALSE;

		dlg.m_szEditPath = tokens[0].c_str();
		dlg.m_szEditOutFolder = tokens[1].c_str();

		dlg.DropFolder(tokens[0].c_str());
		dlg.Build();
		return TRUE;
	}


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

	CDnMakeFullPatchDlg dlg;
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
