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
	info.szNoticeMsg = L"�� ���� ����� ����Ŭ���̾�Ʈ�� ǰ���� ��� ��Ű�µ� ū ������ �˴ϴ�.\n\n������ �ֽ� ������ ��з� ���ֵǸ� ���������� ��Ģ���� �մϴ�.\n�� ������ ���������� ���� �Ǿ� ���� �ʽ��ϴ�.";
#else
	info.szAppName = L"����Ŭ���̾�Ʈ";
	info.szNoticeTitle = L"�˼� ���� ������ ���� ���α׷��� �����ؾ� �մϴ�.\n������ ���ĵ���� �˼��մϴ�.";
	info.szNoticeMsg = L"�� ���� ����� ����Ŭ���̾�Ʈ�� ǰ���� ��� ��Ű�µ� ū ������ �˴ϴ�.\n\n������ �ֽ� ������ ��з� ���ֵǸ� ���������� ��Ģ���� �մϴ�.\n�� ������ ���������� ���� �Ǿ� ���� �ʽ��ϴ�.";
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

