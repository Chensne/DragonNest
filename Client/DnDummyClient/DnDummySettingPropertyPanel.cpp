#include "stdafx.h"
#include "DnDummySettingPropertyPanel.h"
#include "ResourceID.h"
#include "DnDummyClientMgr.h"
#include "DnDummyClientFrame.h"
#include "DnServerApp.h"
#include "IniHelper.h"

CIniHelper g_IniHelper;
const char* g_szIniFile = "DnDummyClient.ini";

DnDummySettingPropertyPanel* g_SettingPropertyPanel = NULL;
// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DnDummySettingPropertyPanel, wxScrolledWindow)
	EVT_PG_CHANGED( ResID::SETTING_PROPERTY, DnDummySettingPropertyPanel::OnSettingPropertyChanged )
	EVT_PG_CHANGED( ResID::USER_PROPERTY, DnDummySettingPropertyPanel::OnUserPropertyChanged )
END_EVENT_TABLE()

DnDummySettingPropertyPanel::DnDummySettingPropertyPanel(wxWindow *pParent, wxWindowID id)
:wxScrolledWindow(pParent, id)
{
	char szCurDir[ _MAX_PATH ] = {0,};
	GetCurrentDirectoryA( _MAX_PATH, szCurDir );
	std::string szFile;
	szFile = szCurDir;
	szFile += "\\";
	szFile += g_szIniFile;
	g_IniHelper.SetFile(szFile.c_str());

	std::string szIP = g_IniHelper.Read("SETTING", "ip", "192.168.0.21");
	std::wstring wszIP;
	ToWideString(szIP, wszIP);
	int nPort = g_IniHelper.Read("SETTING", "port", 14300);
	std::string szServerName = g_IniHelper.Read("SETTING", "ServerName", "nextome");
	std::wstring wszServerName;
	ToWideString(szServerName, wszServerName);

	int nGameStartUserCnt = g_IniHelper.Read("SETTING", "nGameStartUserCnt", 4);
	int nAutoConnectBeginIdx = g_IniHelper.Read("SETTING", "nAutoConnectBeginIdx", 0);
	int nAutoConnectEndIdx = g_IniHelper.Read("SETTING", "nAutoConnectEndIdx", 499);
	int nAutoConnectCount = g_IniHelper.Read("SETTING", "nAutoConnectCount", 10);
	int nAutoConnectTime = g_IniHelper.Read("SETTING", "nAutoConnectTime", 10000);
	int nUpdateTime = g_IniHelper.Read("SETTING", "update_time", 50);
	int nMapIndex =	g_IniHelper.Read("SETTING", "nMapIndex", 204);
	int nChannelIndex =	g_IniHelper.Read("SETTING", "nChannelIndex", 1);
	int nBasePlayTime =	g_IniHelper.Read("SETTING", "nBasePlayTime", 30000);
	int nRandomPlayTime =	g_IniHelper.Read("SETTING", "nRandomPlayTime", 30000);

	g_SettingPropertyPanel = this;
	SetScrollRate( 5, 5 );
	wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );
	SetSizer(pMainSizer);
	
	m_pSettingProperty = new wxPropertyGrid( this, ResID::SETTING_PROPERTY, wxDefaultPosition, wxSize(500,700), wxPG_BOLD_MODIFIED | wxPG_TOOLTIPS | wxTAB_TRAVERSAL | wxEXPAND );
	pMainSizer->Add(m_pSettingProperty, 1, wxALL|wxALIGN_LEFT|wxEXPAND, 2 );

#ifdef _ENG
	m_pSettingProperty->AppendCategory( wxT("Dummy Data"));
	m_aSettingPGID[ _USER_DATA ]	= m_pSettingProperty->Append( new wxFileProperty( wxT("Dummy DataFile"), wxPG_LABEL, wxT("Need Dummy File.") ) );

	m_pSettingProperty->AppendCategory( wxT("Status"));
	m_aSettingPGID[ _CONNECT_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("Connecting"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _DISCONNECT_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("Waiting"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _VILLAGE_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("VilageDummy Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _GAMEPLAYER_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("GameDummy Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _GAMEROOM_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("GameRoom Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_SEND_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Send Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_RECV_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Recv Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_SEND_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Send Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_RECV_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Recv Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_SEND_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Send Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_RECV_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Recv Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_SEND_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Send Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_RECV_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Recv Bytes"), wxPG_LABEL, 0 ) );

	m_pSettingProperty->AppendCategory( wxT("Server Setting"));
	m_aSettingPGID[ _LOGIN_SEVER_IP ]	= m_pSettingProperty->Append( new wxStringProperty( wxT("Login Server IP"), wxPG_LABEL, wszIP.c_str() ) );
	m_aSettingPGID[ _LOGIN_SEVER_PORT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("Login Server Port"), wxPG_LABEL, nPort ) );
	m_aSettingPGID[ _SERVER_NAME ]	= m_pSettingProperty->Append( new wxStringProperty( wxT("ServerName"), wxPG_LABEL, wszServerName.c_str() ) );
	m_aSettingPGID[_GAME_START_USER_COUNT]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("���ӽ����ּ��ο�"), wxPG_LABEL, nGameStartUserCnt ) );
	m_aSettingPGID[_AUTO_CONNECT_BEGIN_IDX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����ӽ����ε���"), wxPG_LABEL, nAutoConnectBeginIdx ) );
	m_aSettingPGID[_AUTO_CONNECT_END_IDX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����ӳ��ε���"), wxPG_LABEL, nAutoConnectEndIdx ) );
	m_aSettingPGID[_AUTO_CONNECT_COUNT]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ѹ��� �����Ҽ� "), wxPG_LABEL, nAutoConnectCount ) );
	m_aSettingPGID[_AUTO_CONNECT_TIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����������ð�"), wxPG_LABEL, nAutoConnectTime ) );
	m_aSettingPGID[_UPDATE_TIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("������Ʈ Ÿ��"), wxPG_LABEL, nUpdateTime ) );
	m_aSettingPGID[_MAP_INDEX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("���ε���"), wxPG_LABEL, nMapIndex ) );
	m_aSettingPGID[_CHANNEL_INDEX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("ä���ε���"), wxPG_LABEL, nChannelIndex ) );

	m_pSettingProperty->AppendCategory( wxT("��Ÿ ����"));
	m_aSettingPGID[_PLAY_MODE]	= m_pSettingProperty->Append( new wxEnumProperty( wxT("������"), wxPG_LABEL, gs_PlayMode ) );

	m_aSettingPGID[_MIN_PLAYTIME]		= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ּҰ��ӽð�"), wxPG_LABEL, nBasePlayTime ) );
	m_aSettingPGID[_RANDOM_PLAYTIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�߰��������ӽð�"), wxPG_LABEL, nRandomPlayTime ) );
#else
	m_pSettingProperty->AppendCategory( wxT("���� ������"));
	m_aSettingPGID[ _USER_DATA ]	= m_pSettingProperty->Append( new wxFileProperty( wxT("���� ����������"), wxPG_LABEL, wxT("�������ε����ּ���.") ) );

	m_pSettingProperty->AppendCategory( wxT("����"));
	m_aSettingPGID[ _CONNECT_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("������"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _DISCONNECT_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�����"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _VILLAGE_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("������ �ִ� ������"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _GAMEPLAYER_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�������� ������"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _GAMEROOM_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("���ӷ� ��"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_SEND_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Send Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_RECV_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Recv Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_SEND_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Send Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _TCP_RECV_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("TCP Recv Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_SEND_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Send Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_RECV_COUNT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Recv Count"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_SEND_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Send Bytes"), wxPG_LABEL, 0 ) );
	m_aSettingPGID[ _UDP_RECV_BYTES ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("UDP Recv Bytes"), wxPG_LABEL, 0 ) );

	m_pSettingProperty->AppendCategory( wxT("���� ����"));
	m_aSettingPGID[ _LOGIN_SEVER_IP ]	= m_pSettingProperty->Append( new wxStringProperty( wxT("Login Server IP"), wxPG_LABEL, wszIP.c_str() ) );
	m_aSettingPGID[ _LOGIN_SEVER_PORT ]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("Login Server Port"), wxPG_LABEL, nPort ) );
	m_aSettingPGID[ _SERVER_NAME ]	= m_pSettingProperty->Append( new wxStringProperty( wxT("ServerName"), wxPG_LABEL, wszServerName.c_str() ) );
	m_aSettingPGID[_GAME_START_USER_COUNT]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("���ӽ����ּ��ο�"), wxPG_LABEL, nGameStartUserCnt ) );
	m_aSettingPGID[_AUTO_CONNECT_BEGIN_IDX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����ӽ����ε���"), wxPG_LABEL, nAutoConnectBeginIdx ) );
	m_aSettingPGID[_AUTO_CONNECT_END_IDX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����ӳ��ε���"), wxPG_LABEL, nAutoConnectEndIdx ) );
	m_aSettingPGID[_AUTO_CONNECT_COUNT]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ѹ��� �����Ҽ� "), wxPG_LABEL, nAutoConnectCount ) );
	m_aSettingPGID[_AUTO_CONNECT_TIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ڵ����������ð�"), wxPG_LABEL, nAutoConnectTime ) );
	m_aSettingPGID[_UPDATE_TIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("������Ʈ Ÿ��"), wxPG_LABEL, nUpdateTime ) );
	m_aSettingPGID[_MAP_INDEX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("���ε���"), wxPG_LABEL, nMapIndex ) );
	m_aSettingPGID[_CHANNEL_INDEX]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("ä���ε���"), wxPG_LABEL, nChannelIndex ) );
	
	m_pSettingProperty->AppendCategory( wxT("��Ÿ ����"));
	m_aSettingPGID[_PLAY_MODE]	= m_pSettingProperty->Append( new wxEnumProperty( wxT("������"), wxPG_LABEL, gs_PlayMode ) );
	
	m_aSettingPGID[_MIN_PLAYTIME]		= m_pSettingProperty->Append( new wxUIntProperty( wxT("�ּҰ��ӽð�"), wxPG_LABEL, nBasePlayTime ) );
	m_aSettingPGID[_RANDOM_PLAYTIME]	= m_pSettingProperty->Append( new wxUIntProperty( wxT("�߰��������ӽð�"), wxPG_LABEL, nRandomPlayTime ) );
#endif
	


	
	/*
	m_pUserProperty = new wxPropertyGrid( this, ResID::USER_PROPERTY, wxDefaultPosition, wxSize(500,300), wxPG_BOLD_MODIFIED | wxPG_TOOLTIPS | wxTAB_TRAVERSAL | wxEXPAND );
	pMainSizer->Add(m_pUserProperty, 1, wxALL|wxALIGN_LEFT|wxEXPAND, 2 );
	m_pUserProperty->AppendCategory( wxT("��������"));
	
	m_aUserPGID[ User_State ]		= m_pUserProperty->Append( new wxStringProperty( wxT("����"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_ID ]			= m_pUserProperty->Append( new wxStringProperty( wxT("���̵�"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_Character ]	= m_pUserProperty->Append( new wxStringProperty( wxT("ĳ����"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_CurPos ]		= m_pUserProperty->Append( new wxStringProperty( wxT("������ġ"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_CurMap ]		= m_pUserProperty->Append( new wxStringProperty( wxT("�����"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_ConnectTime ]	= m_pUserProperty->Append( new wxStringProperty( wxT("���ӽð�"), wxPG_LABEL, wxT("") ) );
	m_aUserPGID[ User_PlayTime ]	= m_pUserProperty->Append( new wxStringProperty( wxT("�÷��̽ð�"), wxPG_LABEL, wxT("") ) );
	*/
	UpdateSettings();
}

DnDummySettingPropertyPanel::~DnDummySettingPropertyPanel()
{
	g_SettingPropertyPanel = NULL;

}


void DnDummySettingPropertyPanel::OnSettingPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	   wxPGProperty* id = PGEvent.GetProperty();
	   wxString szName = PGEvent.GetPropertyName();

	   if ( szName == m_aSettingPGID[ _USER_DATA ]->GetName() )
	   {
		   wxVariant value = PGEvent.GetPropertyValue();
		   wxString szFileName = value.GetString();

		   std::vector<DnDummyClient::CreateInfo>	out;

		   std::string szFile;
		   ToMultiString(std::wstring(szFileName.GetData()), szFile);
		   bool bResult  = LoadDummyClientFile(szFile.c_str(), out);

		   wxString str;
		   
		   if ( bResult )
		   {
			   str = wxString::Format(_T("���� ������ ���� �ε� ���� : %s"), szFileName.GetData());
		   }
		   else
		   {
			   str = wxString::Format(_T("���� ������ ���� �ε� ���� : %s"), szFileName.GetData());
		   }

		   Log(str.GetData());

	   }

	   UpdateSettings();
	   
}

void DnDummySettingPropertyPanel::OnUserPropertyChanged( wxPropertyGridEvent& PGEvent )
{
	
}

void DnDummySettingPropertyPanel::UpdateSettings()
{
	m_Settings.szLoginServerIP = m_aSettingPGID[_LOGIN_SEVER_IP]->GetValue().GetString();
	m_Settings.nLoginServerPort = m_aSettingPGID[_LOGIN_SEVER_PORT]->GetValue().GetInteger();
	m_Settings.szServerName = m_aSettingPGID[_SERVER_NAME]->GetValue().GetString();

	m_Settings.nGameStartUserCnt = m_aSettingPGID[_GAME_START_USER_COUNT]->GetValue().GetInteger();

	m_Settings.nAutoConnectBeginIdx = m_aSettingPGID[_AUTO_CONNECT_BEGIN_IDX]->GetValue().GetInteger();
	m_Settings.nAutoConnectEndIdx = m_aSettingPGID[_AUTO_CONNECT_END_IDX]->GetValue().GetInteger();
	m_Settings.nAutoConnectCount = m_aSettingPGID[_AUTO_CONNECT_COUNT]->GetValue().GetInteger();
	m_Settings.nAutoConnectTime = m_aSettingPGID[_AUTO_CONNECT_TIME]->GetValue().GetInteger();
	m_Settings.nUpdateTime = m_aSettingPGID[_UPDATE_TIME]->GetValue().GetInteger();
	m_Settings.nMapIndex = m_aSettingPGID[_MAP_INDEX]->GetValue().GetInteger();
	m_Settings.nChannelIndex = m_aSettingPGID[_CHANNEL_INDEX]->GetValue().GetInteger();
	m_Settings.nBasePlayTime = m_aSettingPGID[_MIN_PLAYTIME]->GetValue().GetInteger();
	m_Settings.nRandomPlayTime = m_aSettingPGID[_RANDOM_PLAYTIME]->GetValue().GetInteger();
	int nMode = m_aSettingPGID[_PLAY_MODE]->GetValue().GetInteger();
	g_DummyClientMgr.SetMode(nMode);
	
	std::string szIP;
	std::string szServerName;

	ToMultiString(m_Settings.szLoginServerIP, szIP);
	ToMultiString(m_Settings.szServerName, szServerName);
	char szCurDir[ _MAX_PATH ] = {0,};
	GetCurrentDirectoryA( _MAX_PATH, szCurDir );
	std::string szFile;
	szFile = szCurDir;
	szFile += "\\";
	szFile += g_szIniFile;
	g_IniHelper.SetFile(szFile.c_str());
	g_IniHelper.Write("SETTING", "ip",						szIP.c_str());
	g_IniHelper.Write("SETTING", "port",					m_Settings.nLoginServerPort);
	g_IniHelper.Write("SETTING", "nGameStartUserCnt",		m_Settings.nGameStartUserCnt);
	g_IniHelper.Write("SETTING", "nAutoConnectBeginIdx",	m_Settings.nAutoConnectBeginIdx);
	g_IniHelper.Write("SETTING", "nAutoConnectEndIdx",		m_Settings.nAutoConnectEndIdx);
	g_IniHelper.Write("SETTING", "nAutoConnectCount",		m_Settings.nAutoConnectCount);
	g_IniHelper.Write("SETTING", "nAutoConnectTime",		m_Settings.nAutoConnectTime);
	g_IniHelper.Write("SETTING", "update_time",				m_Settings.nUpdateTime);
	g_IniHelper.Write("SETTING", "ServerName",				szServerName.c_str());
	g_IniHelper.Write("SETTING", "nMapIndex",				m_Settings.nMapIndex);
	g_IniHelper.Write("SETTING", "nChannelIndex",			m_Settings.nChannelIndex);
	g_IniHelper.Write("SETTING", "nBasePlayTime",			m_Settings.nBasePlayTime);
	g_IniHelper.Write("SETTING", "nRandomPlayTime",			m_Settings.nRandomPlayTime);


	g_IniHelper.Flush();
	
	DWORD dw = GetLastError();



}

void DnDummySettingPropertyPanel::RefreshSettingProperty()
{

	int nConnected = 0;
	int nDisConnected = 0;
	int nVillageCnt, nGameRoomCnt, nGamePlayCnt;

	g_DummyClientMgr.GetClientCntInfo(nConnected, nDisConnected, nVillageCnt, nGameRoomCnt, nGamePlayCnt);


	m_aSettingPGID[_CONNECT_COUNT]->SetValue(wxVariant((long)nConnected));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_CONNECT_COUNT]);
	m_aSettingPGID[_DISCONNECT_COUNT]->SetValue(wxVariant((long)nDisConnected));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_DISCONNECT_COUNT]);

	m_aSettingPGID[_VILLAGE_COUNT]->SetValue(wxVariant((long)nVillageCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_VILLAGE_COUNT]);
	m_aSettingPGID[_GAMEPLAYER_COUNT]->SetValue(wxVariant((long)nGamePlayCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_GAMEPLAYER_COUNT]);
	m_aSettingPGID[_GAMEROOM_COUNT]->SetValue(wxVariant((long)nGameRoomCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_GAMEROOM_COUNT]);
	
	int nTCPSendCnt = NxServerApp::m_nTotalSendCnt / (UI_UPDATE_TIME/1000);
	int nTCPSendBytes = NxServerApp::m_nTotalSendBytes/ (UI_UPDATE_TIME/1000);
	int nTCPRecvCnt = NxServerApp::m_nTotalRecvCnt/ (UI_UPDATE_TIME/1000);
	int nTCPRecvBytes = NxServerApp::m_nTotalRecvBytes/ (UI_UPDATE_TIME/1000);

	int nUDPSendCnt = g_DummyClientMgr.m_nTotalSendCnt/ (UI_UPDATE_TIME/1000);
	int nUDPSendBytes = g_DummyClientMgr.m_nTotalSendBytes/ (UI_UPDATE_TIME/1000);
	int nUDPRecvCnt = g_DummyClientMgr.m_nTotalRecvCnt/ (UI_UPDATE_TIME/1000);
	int nUDPRecvBytes = g_DummyClientMgr.m_nTotalRecvBytes/ (UI_UPDATE_TIME/1000);

	
	m_aSettingPGID[_TCP_SEND_COUNT]->SetValue(wxVariant((long)nTCPSendCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_TCP_SEND_COUNT]);

	m_aSettingPGID[_TCP_RECV_COUNT]->SetValue(wxVariant((long)nTCPRecvCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_TCP_RECV_COUNT]);

	m_aSettingPGID[_TCP_SEND_BYTES]->SetValue(wxVariant((long)nTCPSendBytes));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_TCP_SEND_BYTES]);

	m_aSettingPGID[_TCP_RECV_BYTES]->SetValue(wxVariant((long)nTCPRecvBytes));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_TCP_RECV_BYTES]);



	m_aSettingPGID[_UDP_SEND_COUNT]->SetValue(wxVariant((long)nUDPSendCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_UDP_SEND_COUNT]);

	m_aSettingPGID[_UDP_RECV_COUNT]->SetValue(wxVariant((long)nUDPRecvCnt));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_UDP_RECV_COUNT]);

	m_aSettingPGID[_UDP_SEND_BYTES]->SetValue(wxVariant((long)nUDPSendBytes));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_UDP_SEND_BYTES]);

	m_aSettingPGID[_UDP_RECV_BYTES]->SetValue(wxVariant((long)nUDPRecvBytes));
	m_pSettingProperty->RefreshProperty(m_aSettingPGID[_UDP_RECV_BYTES]);


	

	
	g_DummyClientMgr.ClearIOInfo();
	NxServerApp::ClearIOInfo();
}