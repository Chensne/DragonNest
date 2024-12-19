#include "stdafx.h"
#include "DnDummyClientFrame.h"
#include "DnDummyInfoListCtrl.h"
#include "DnDummySettingPropertyPanel.h"
#include "ResourceID.h"
#include "DnServerApp.h"
#include "BugReporter.h"

wxTextCtrl*		g_pLogTextCtrl = NULL;


void Log(const TCHAR* szString)
{
	if ( !g_pLogTextCtrl )
		return;

	if ( g_pLogTextCtrl )
	{
		g_pLogTextCtrl->AppendText(wxString(szString));
	}

}
void ClearLog()
{
	if ( !g_pLogTextCtrl )
		return;

	if ( g_pLogTextCtrl )
	{
		g_pLogTextCtrl->Clear();
	}
}

BEGIN_EVENT_TABLE(DnDummyClientFrame, wxFrame)
	EVT_MENU( ResID::CONNECT,		DnDummyClientFrame::OnMenuConnect )
	EVT_MENU( ResID::DISCONNECT,	DnDummyClientFrame::OnMenuDisconnect )
	EVT_MENU( ResID::AUTO_CONNECT,	DnDummyClientFrame::OnMenuAutoConnect )
	EVT_MENU( ResID::AUTO_DISCONNECT,	DnDummyClientFrame::OnMenuStopAutoConnect )
	EVT_MENU( ResID::CAPTURE_INFO, DnDummyClientFrame::OnMenuSaveInfomation )
	EVT_MENU( ResID::RETRY_CONNECT, DnDummyClientFrame::OnMenuRetryConnect )
	
	
	
	EVT_TIMER(wxID_ANY, DnDummyClientFrame::OnTimer)
	EVT_IDLE( DnDummyClientFrame::OnIdle )
	EVT_CLOSE(DnDummyClientFrame::OnCloseWindow)
END_EVENT_TABLE()

DnDummyClientFrame::DnDummyClientFrame(wxWindow* parent,
										 wxWindowID id,
										 const wxString& title,
										 const wxPoint& pos,
										 const wxSize& size,
										 long style)
										 : wxFrame(parent, id, title, pos, size, style), m_Timer(this)
{

	m_pLogTextCtrl = NULL;
	m_pDummyInfoListCtrl = NULL;
	m_pDummySettingPropertyPanel = NULL;
	m_bAutoConnect = false;
	m_nCurAutoIndex = 0;
	m_nAutoConnectBegin = 0;
	m_nAutoConnectEnd = 0;
	m_nLastUpdateTime = 0;
	_CreateWindow();

	gs_BugReporter.GetReportingServerInfo().hWnd  = (HWND)this->GetHWND();

}

DnDummyClientFrame::~DnDummyClientFrame()
{
	
	//SAFE_DELETE(tb1);

	g_DummyClientMgr.AllDisconnect();

	m_Timer.Stop();
	m_AuiManager.UnInit();

}

void DnDummyClientFrame::DoUpdate()
{
	m_AuiManager.Update();
}



bool 
DnDummyClientFrame::_CreateWindow()
{
	//_MakeDummyFile();
	//_MakeDummyNexonFile();
	
	// aui 등록
	m_AuiManager.SetManagedWindow(this);

	// create a menu bar
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(wxID_SEPARATOR);
	menuFile->Append(wxID_EXIT, _T("종료하기"));

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, _T("메뉴"));

	SetMenuBar(menuBar);

	// 상태바 생성
	CreateStatusBar();
	GetStatusBar()->SetStatusText(_T("준비"));

	SetMinSize(wxSize(1400,1015));
	SetSize(1400,1015);


	// 툴바
	tb1 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_NODIVIDER|wxTB_FLAT|wxTB_TEXT  );//		wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT);

	tb1->AddTool( ResID::CONNECT, wxT("Connect"),	wxBITMAP(connect));
	tb1->AddTool( ResID::DISCONNECT, wxT("Disconnect"), wxBITMAP(disconnect));
	tb1->AddTool( ResID::AUTO_CONNECT, wxT("AutoConnect"),	wxBITMAP(connect));
	tb1->AddTool( ResID::AUTO_DISCONNECT, wxT("AutoDisConnect"),	wxBITMAP(disconnect));
	tb1->AddTool( ResID::CAPTURE_INFO, wxT("Save Infomation"),	wxBITMAP(filesave));
	tb1->AddTool( ResID::RETRY_CONNECT, wxT("Retry Connect"),	wxBITMAP(connect));

	tb1->Realize();

	// 툴바 
	m_AuiManager.AddPane(tb1, wxAuiPaneInfo().
		Name(wxT("tb2")).Caption(wxT("Toolbar 2")).
		ToolbarPane().Top().
		LeftDockable(false).RightDockable(false));



	// 더미 프로퍼티
	m_pDummySettingPropertyPanel = new DnDummySettingPropertyPanel(this, wxID_ANY);

#ifdef _ENG
	m_AuiManager.AddPane(m_pDummySettingPropertyPanel, wxAuiPaneInfo().
		Name(wxT("DummySetting")).Caption(wxT("DummyClient Setting")).
		Left().CloseButton(false).BestSize(250,800).MaxSize(250,800));
#else
	m_AuiManager.AddPane(m_pDummySettingPropertyPanel, wxAuiPaneInfo().
		Name(wxT("DummySetting")).Caption(wxT("더미클라이언트세팅")).
		Left().CloseButton(false).BestSize(250,800).MaxSize(250,800));
#endif
	wxAuiPaneInfo& infoSet = m_AuiManager.GetPane(wxT("DummySetting"));


	// 더미 리스트 컨트롤
	m_pDummyInfoListCtrl = new DnDummyInfoListCtrl(this, ResID::USER_LIST_CTRL, wxDefaultPosition, wxDefaultSize );

	m_AuiManager.AddPane(m_pDummyInfoListCtrl, wxAuiPaneInfo().
		Name(wxT("DummyInfo")).Caption(wxT("더미클라이언트리스트")).
		Right().Top().CloseButton(false).CenterPane());
	wxAuiPaneInfo& info = m_AuiManager.GetPane(wxT("DummyInfo"));

	m_pLogTextCtrl = new wxTextCtrl(this,wxID_ANY, _T(""), wxPoint(0,0), wxSize(150,90),
		wxNO_BORDER | wxTE_MULTILINE);

	m_pLogTextCtrl->SetEditable(false);
	g_pLogTextCtrl = m_pLogTextCtrl;

	// 로그 컨트롤
	m_AuiManager.AddPane(m_pLogTextCtrl, wxAuiPaneInfo().
		Name(wxT("test10")).Caption(wxT("로그패널")).
		Bottom().CloseButton(false));

	wxString str;
	str = _T("Initialize Dummy Client.\n");
	AddLog( str.GetData() );

	m_Timer.Start(UI_UPDATE_TIME);
	DoUpdate();

	return true;
}




void 
DnDummyClientFrame::AddLog(const TCHAR* szString)
{
	m_pLogTextCtrl->AppendText(wxString(szString));
}

void 
DnDummyClientFrame::OnCloseWindow( wxCloseEvent& event )
{
	g_DummyClientMgr.AllDisconnect();
	wxFrame::OnCloseWindow(event);
}

void 
DnDummyClientFrame::OnMenuConnect( wxCommandEvent& MenuEvent )
{
	std::vector<int> SelectedIndex;
	g_pDummyInfoListCtrl->GetSelectedIndex(SelectedIndex);

	bool bIgnore = false;
	for ( int i = 0 ; i < (int)SelectedIndex.size() ; i++ )
	{
		DnDummyClient* pClient = g_DummyClientMgr.GetDummyClient(SelectedIndex[i]);
		
		if ( !pClient )
			continue;

		bool bResult = pClient->ConnectToLoginServer(g_SettingPropertyPanel->GetSettingPropertyData().szLoginServerIP.c_str(), 
			g_SettingPropertyPanel->GetSettingPropertyData().nLoginServerPort);

		/*
		if ( bResult == false && bIgnore == false )
		{
			if ( wxMessageBox( _T("접속에 실패 하였습니다. 접속을 취소 하겠습니까?"),wxMessageBoxCaptionStr, wxYES_NO) == wxYES ) 
			{
				bIgnore = false;
				return;
			}
			else
			{
				bIgnore = true;
				
			}
		}
		*/

	}
	
}

void
DnDummyClientFrame::OnMenuDisconnect( wxCommandEvent& MenuEvent )
{
	std::vector<int> SelectedIndex;
	g_pDummyInfoListCtrl->GetSelectedIndex(SelectedIndex);


	for ( int i = 0 ; i < (int)SelectedIndex.size() ; i++ )
	{
		DnDummyClient* pClient = g_DummyClientMgr.GetDummyClient(SelectedIndex[i]);

		if ( !pClient )
			continue;

		pClient->Disconnect();

	}
	
}

void 
DnDummyClientFrame::OnMenuAutoConnect( wxCommandEvent& MenuEvent )
{
	m_bAutoConnect = true;

	m_nAutoConnectBegin = g_SettingPropertyPanel->GetSettingPropertyData().nAutoConnectBeginIdx;
	m_nAutoConnectEnd  = g_SettingPropertyPanel->GetSettingPropertyData().nAutoConnectEndIdx;
	m_nCurAutoIndex = m_nAutoConnectBegin;
	m_nLastAutoConnectTime = 0;
}

void
DnDummyClientFrame::OnMenuStopAutoConnect( wxCommandEvent& MenuEvent )
{
	m_bAutoConnect = false;
}

void
DnDummyClientFrame::OnMenuSaveInfomation( wxCommandEvent& MenuEvent )
{

	int nTCPSendCnt = NxServerApp::m_nTotalSendCnt / (UI_UPDATE_TIME/1000);
	int nTCPSendBytes = NxServerApp::m_nTotalSendBytes/ (UI_UPDATE_TIME/1000);
	int nTCPRecvCnt = NxServerApp::m_nTotalRecvCnt/ (UI_UPDATE_TIME/1000);
	int nTCPRecvBytes = NxServerApp::m_nTotalRecvBytes/ (UI_UPDATE_TIME/1000);

	int nUDPSendCnt = g_DummyClientMgr.m_nTotalSendCnt/ (UI_UPDATE_TIME/1000);
	int nUDPSendBytes = g_DummyClientMgr.m_nTotalSendBytes/ (UI_UPDATE_TIME/1000);
	int nUDPRecvCnt = g_DummyClientMgr.m_nTotalRecvCnt/ (UI_UPDATE_TIME/1000);
	int nUDPRecvBytes = g_DummyClientMgr.m_nTotalRecvBytes/ (UI_UPDATE_TIME/1000);

	std::wstring szLog;
	szLog.reserve(32 * 1024);

	time_t curTime;
	time(&curTime);
	std::wstring szTime = GetTimeString(curTime);

	szLog += L"-----------------------------------------------------------------------------------------\n";
	szLog += szTime.c_str();
	szLog += L"\n";
	szLog += FormatW(L"TCP Send Count : %d\n", nTCPSendCnt );
	szLog += FormatW(L"TCP Recv Count : %d\n", nTCPRecvCnt );
	szLog += FormatW(L"TCP Send Bytes : %d\n", nTCPSendBytes );
	szLog += FormatW(L"TCP Recv Bytes : %d\n", nTCPRecvBytes );
	szLog += FormatW(L"UDP Send Count : %d\n", nUDPSendCnt );
	szLog += FormatW(L"UDP Recv Count : %d\n", nUDPRecvCnt );
	szLog += FormatW(L"UDP Send Bytes : %d\n", nUDPSendBytes );
	szLog += FormatW(L"UDP Recv Bytes : %d\n", nUDPRecvBytes );
	szLog += L"-----------------------------------------------------------------------------------------\n";
	
	for ( int i = 0 ; i < _countof(gs_Column) ; i++ )
	{
		szLog += gs_Column[i];
		if ( i < _countof(gs_Column) -1 )
			szLog += L",";
		
	}
	szLog += L"\n";
	g_DummyClientMgr.DumpCurrentInfo(szLog);


	WCHAR szCurDir[ _MAX_PATH ] = {0,};
	GetCurrentDirectoryW( _MAX_PATH, szCurDir );
	std::wstring _szCurDir;
	_szCurDir = szCurDir; 
	_szCurDir += L"\\Dummy_";
	_szCurDir += szTime;
	_szCurDir += L".txt";

	FILE* pFile = _wfopen(_szCurDir.c_str(), L"w");

	int nSize = szLog.size()*2;

	std::string _szLog;
	_szLog.reserve(nSize);

	
	char* pBuffer = new char[nSize];
	ZeroMemory(pBuffer, nSize);
	WideCharToMultiByte( CP_ACP, 0, szLog.c_str(), -1, pBuffer, nSize, NULL, NULL );

	fwrite(pBuffer, 1, nSize, pFile );
	fclose(pFile);


	delete[] pBuffer;
}

void
DnDummyClientFrame::OnMenuRetryConnect( wxCommandEvent& MenuEvent )
{
	for ( int i = 0 ; i < g_DummyClientMgr.GetDummyClientSize() ; i++ )
	{
		DnDummyClient* pClient = g_DummyClientMgr.GetDummyClient(i);

		if ( !pClient )
			continue;

		if ( pClient->IsConnected() )
			continue;

		if ( pClient->GetProperty().nConnectionState == StateInfo::_DISCONNECTED )
		{
			bool bResult = pClient->ConnectToLoginServer(g_SettingPropertyPanel->GetSettingPropertyData().szLoginServerIP.c_str(), 
				g_SettingPropertyPanel->GetSettingPropertyData().nLoginServerPort);
		}
	}
}


void
DnDummyClientFrame::OnTimer(wxTimerEvent& event)
{
	g_DummyClientMgr.UpdateDummyInfo();

	
	//g_DummyClientMgr.Update();
	g_SettingPropertyPanel->RefreshSettingProperty();

	if ( m_bAutoConnect )
	{
		DWORD nCurTime = GetTickCount();
		if ( m_nLastAutoConnectTime > 0 )
		{
			DWORD n = g_SettingPropertyPanel->GetSettingPropertyData().nAutoConnectTime;
			if ( nCurTime < m_nLastAutoConnectTime+n )
			{
				return;
			}
		}

		m_nLastAutoConnectTime = nCurTime;
		wxString str1 = wxString::Format(_T("시간 : %d \n" ), m_nLastAutoConnectTime);
		AddLog(str1.c_str());

		int nDelta = g_SettingPropertyPanel->GetSettingPropertyData().nAutoConnectCount;

		int nCnt = 0 ;
		for ( int i = m_nCurAutoIndex; i <= m_nAutoConnectEnd ; i++ )
		{
			DnDummyClient* pClient = g_DummyClientMgr.GetDummyClient(i);

			if ( !pClient )
				continue;

			if ( pClient->IsConnected() )
				continue;

			wxString str = wxString::Format(_T("더미 자동 접속 시도 Idx : %d \n" ), i);
			AddLog(str.c_str());
			bool bResult = pClient->ConnectToLoginServer(g_SettingPropertyPanel->GetSettingPropertyData().szLoginServerIP.c_str(), 
				g_SettingPropertyPanel->GetSettingPropertyData().nLoginServerPort);
/*		
			if ( bResult == false  )
			{
				//if ( wxMessageBox( _T("접속에 실패 하였습니다. 접속을 취소 하겠습니까?"),wxMessageBoxCaptionStr, wxYES_NO) == wxYES ) 
				{
					m_bAutoConnect = false;
					return;
				}
			}
*/
			nCnt++;
			m_nCurAutoIndex++;

			if ( nCnt >= nDelta )
			{
				if( m_nCurAutoIndex > m_nAutoConnectEnd )
				{
					m_nCurAutoIndex = m_nAutoConnectBegin;
				}
				return;
			}
			if( m_nCurAutoIndex > m_nAutoConnectEnd )
			{
				m_nCurAutoIndex = m_nAutoConnectBegin;
				return;
			}
		}
		m_nCurAutoIndex = m_nAutoConnectBegin;
	}
}

void DnDummyClientFrame::OnIdle( wxIdleEvent& IdleEvent )
{
	g_DummyClientMgr.Update();

	DWORD nCurTime = GetTickCount();

	DWORD nDelta = nCurTime - m_nLastUpdateTime;
	
	IdleEvent.RequestMore();

	if ( nDelta >= g_SettingPropertyPanel->GetSettingPropertyData().nUpdateTime )
	{
		IdleEvent.RequestMore();
		m_nLastUpdateTime = nCurTime;
	}
	else
	{
		int a = 0;
	}
}

void 
DnDummyClientFrame::_MakeDummyFile()
{
	FILE* fp = fopen("DummyUserData.xml", "wt");

	fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone = \"yes\"?>\n");
	fprintf(fp, "<dummy_user_data>\n");

	for ( int i = 1 ; i <= 5000 ; i++ )
	{
		fprintf(fp, "	<user id = \"nextome_%d\"  passwd = \"1111\" character = \"archer\" extra = \"\"/>\n", i);
	}
	
	fprintf(fp, "</dummy_user_data>");

	fclose(fp);
}


void 
DnDummyClientFrame::_MakeDummyNexonFile(int nSplit)
{
	struct __User
	{
		std::string szID;
		std::string szPasswd;
		std::string szPassport;
	};

	int nMakeUserCnt = 5000;
	std::vector<__User> userList;
	userList.reserve(nMakeUserCnt);
	

	char _szId[256] = {0,};
	char _szPasswd[256] = {0,};
	char _szPassport[2048] = {0,};

	FILE* fp = fopen("passport.log", "rt");

	while (1)
	{
		int nResult = fscanf(fp,"%s %s %s", _szId, _szPasswd, _szPassport);
		if ( nResult < 1 )
			break;

		std::string szID = _szId;
		std::string szPasswd = _szPasswd;
		std::string szPassport = _szPassport;
		RemoveStringA(szID, std::string(","));
		RemoveStringA(szPasswd, std::string(","));

		__User user;
		user.szID = szID;
		user.szPasswd = szPasswd;
		user.szPassport = szPassport;
		userList.push_back(user);
	}
	fclose(fp);

	if (nSplit <= 0)
	{
		fp = fopen("Nexon_DummyList.xml", "wt");

		fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone = \"yes\"?>\n");
		fprintf(fp, "<dummy_user_data>\n");

		for ( int i = 0 ; i < userList.size() ; i++ )
		{
			std::string szID = userList[i].szID;
			std::string szPasswd = userList[i].szPasswd;
			std::string szPassport = userList[i].szPassport;

			fprintf(fp, "	<user id = \"%s\"  passwd = \"%s\" character = \"archer\"	extra = \"%s\" />\n", szID.c_str(), szPasswd.c_str(), szPassport.c_str() );
		}

		fprintf(fp, "</dummy_user_data>");

		fclose(fp);
	}
	else
	{
		int nSplitCount = (int)userList.size()/nSplit;
		for (int j = 0; j < nSplitCount; j++)
		{
			char szXmlName[128];
			sprintf(szXmlName, "NexonDummy%d.xml", j+1);
			fp = fopen(szXmlName, "wt");

			fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone = \"yes\"?>\n");
			fprintf(fp, "<dummy_user_data>\n");

			int nLimit = j == 0 ? nSplit : (nSplit * (j+1));
			for (int i = j*nSplit; i < nLimit; i++)
			{
				std::string szID = userList[i].szID;
				std::string szPasswd = userList[i].szPasswd;
				std::string szPassport = userList[i].szPassport;

				fprintf(fp, "	<user id = \"%s\"  passwd = \"%s\" character = \"archer\"	extra = \"%s\" />\n", szID.c_str(), szPasswd.c_str(), szPassport.c_str() );
			}

			fprintf(fp, "</dummy_user_data>");

			fclose(fp);
		}
		/*fp = fopen("Nexon_DummyList.xml", "wt");

		fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone = \"yes\"?>\n");
		fprintf(fp, "<dummy_user_data>\n");

		for ( int i = 0 ; i < userList.size() ; i++ )
		{
			std::string szID = userList[i].szID;
			std::string szPasswd = userList[i].szPasswd;
			std::string szPassport = userList[i].szPassport;

			fprintf(fp, "	<user id = \"%s\"  passwd = \"%s\" character = \"archer\"	extra = \"%s\" />\n", szID.c_str(), szPasswd.c_str(), szPassport.c_str() );
		}

		fprintf(fp, "</dummy_user_data>");

		fclose(fp);*/
	}
}

