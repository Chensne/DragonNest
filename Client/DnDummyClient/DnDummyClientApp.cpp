#include "stdafx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/app.h"

#include "wx/aui/aui.h"

#include "BugReporter.h"
#include "DnDummyClientApp.h"
#include "DnDummyClientFrame.h"
#include "DnSecure.h"

#include "DnServerApp.h"
#pragma comment( lib, "dbghelp.lib" )

IMPLEMENT_APP(DnDummyClientApp)

bool DnDummyClientApp::OnInit()
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); // 
//	_CrtSetBreakAlloc(18730);
//	_CrtSetBreakAlloc(17516);
//	_CrtSetBreakAlloc(982);


	BugReporter::ReportingServerInfo info;
	

	info.bSaveLocal = true;
	info.bUseAutoView = false;
	info.bUseConfirm = true;
#ifdef _ENG
	info.szAppName = L"DummyClient";
	info.szNoticeTitle = L"Error.";
	info.szNoticeMsg = L"이 오류 보고는 더미클라이언트의 품질을 향상 시키는데 큰 도움이 됩니다.\n\n전달해 주신 정보는 기밀로 간주되며 보안유지를 원칙으로 합니다.\n또 고객님의 개인정보는 포함 되어 있지 않습니다.";
#else
	info.szAppName = L"더미클라이언트";
	info.szNoticeTitle = L"알수 없는 오류로 인해 프로그램을 종료해야 합니다.\n불편을 끼쳐드려서 죄송합니다.";
	info.szNoticeMsg = L"이 오류 보고는 더미클라이언트의 품질을 향상 시키는데 큰 도움이 됩니다.\n\n전달해 주신 정보는 기밀로 간주되며 보안유지를 원칙으로 합니다.\n또 고객님의 개인정보는 포함 되어 있지 않습니다.";
#endif
	info.hInst = wxGetInstance();

	gs_BugReporter.SetReportingServerInfo(info);
#ifdef _DEBUG
	gs_BugReporter.SetBuildVersion(std::string("Debug"));
#else
	gs_BugReporter.SetBuildVersion(std::string("Release"));
#endif 

	gs_BugReporter.Enable( BugReporter::eMiniDumpNormal );


	CDNSecure::CreateInstance();
	
	srand( GetTickCount());
	
	g_ServerApp.Create(2, 1001,10000);

	wxFrame* frame = new DnDummyClientFrame(NULL,
		wxID_ANY,
		wxT("DragonNest Dummy Client"),
		wxDefaultPosition,
		wxSize(800, 600));
	SetTopWindow(frame);
	frame->Show();

	return true;
}



void DnDummyClientApp::CleanUp()
{
	g_ServerApp.Destroy();
}

