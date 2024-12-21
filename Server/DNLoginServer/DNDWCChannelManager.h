#pragma once

#if defined( PRE_ADD_DWC )

class CDNDWCChannelManager
{
public:
	CDNDWCChannelManager();
	~CDNDWCChannelManager();

	bool Init();
	void UpdateInfo();

	bool CheckValidDate();
	void GetChannelInfo( OUT TDWCChannelInfo& rChannelInfo );

private:
	TDWCChannelInfo m_ChannelInfo;
};

extern CDNDWCChannelManager* g_pDWCChannelManager;

#endif // #if defined( PRE_ADD_DWC )