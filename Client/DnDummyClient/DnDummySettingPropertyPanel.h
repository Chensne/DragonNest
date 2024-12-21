#pragma once

class DnDummySettingPropertyPanel : public wxScrolledWindow
{
public:
	DnDummySettingPropertyPanel(wxWindow *pParent, wxWindowID id);
	virtual ~DnDummySettingPropertyPanel();

	struct SettingPropValues 
	{
		std::wstring szLoginServerIP;
		int			 nLoginServerPort;
		std::wstring szServerName;
		//int			 nGameServerPort;
		int			 nGameStartUserCnt;
		int			 nAutoConnectBeginIdx;
		int			 nAutoConnectEndIdx;
		int			 nAutoConnectCount;
		int			 nAutoConnectTime;
		int			 nUpdateTime;
		int			 nMapIndex;
		int			 nChannelIndex;
		int			 nBasePlayTime;
		int			 nRandomPlayTime;

	};
private:
	enum SettingProp
	{
		_CONNECT_COUNT = 0,
		_DISCONNECT_COUNT,
		_VILLAGE_COUNT,
		_GAMEPLAYER_COUNT,
		_GAMEROOM_COUNT,
		_TCP_SEND_COUNT,
		_TCP_RECV_COUNT,
		_TCP_SEND_BYTES,
		_TCP_RECV_BYTES,
		_UDP_SEND_COUNT,
		_UDP_RECV_COUNT,
		_UDP_SEND_BYTES,
		_UDP_RECV_BYTES,
		_LOGIN_SEVER_IP,
		_LOGIN_SEVER_PORT,
		_GAME_SEVER_IP,
		_GAME_SEVER_PORT,
		_SERVER_NAME,
		_GAME_START_USER_COUNT,
		_AUTO_CONNECT_BEGIN_IDX,
		_AUTO_CONNECT_END_IDX,
		_AUTO_CONNECT_COUNT,
		_AUTO_CONNECT_TIME,
		_UPDATE_TIME,
		_MAP_INDEX,
		_CHANNEL_INDEX,
		_USER_DATA,

		_PLAY_MODE,
		_MIN_PLAYTIME,
		_RANDOM_PLAYTIME,

		_COUNT
	};
	

	enum UserProp
	{
		User_State = 0,
		User_ID,
		User_Character,
		User_CurPos,
		User_CurMap,
		User_ConnectTime,
		User_PlayTime,
		User_COUNT
	};


public:
	void OnSettingPropertyChanged( wxPropertyGridEvent& PGEvent );
	void OnUserPropertyChanged( wxPropertyGridEvent& PGEvent );

	void UpdateSettings();
	SettingPropValues&	GetSettingPropertyData() { return m_Settings; }

	void RefreshSettingProperty();
private:
	wxPropertyGrid*	m_pSettingProperty;
	wxPGId			m_aSettingPGID[ _COUNT ];
	SettingPropValues m_Settings;

	wxPropertyGrid*	m_pUserProperty;
	wxPGId			m_aUserPGID[ User_COUNT ];
	
	

	DECLARE_EVENT_TABLE()
};

extern DnDummySettingPropertyPanel* g_SettingPropertyPanel;