
#pragma once

#if defined( PRE_PRIVATECHAT_CHANNEL )

class CDNUserSession;
class CDNPrivateChaChannel;
 
class CDNPrivateChatManager 
{
public:
	CDNPrivateChatManager(); 
	~CDNPrivateChatManager();	

	char GetWorldIndex(char cWorldSetID);
	bool AddPrivateChatChannel(char cWorldSetID, const TPrivateChatChannelInfo &ChannelInfo );
	CDNPrivateChaChannel* GetPrivateChannelInfo( char cWorldSetID, INT64 nChannelID ); // 채널 정보 얻어오기(속한 유저들까지)	

	INT64 GetPrivateChatChannelId( char cWorldSetID, WCHAR* wszChannelName );

	bool GetFirst() {return m_bFirst;}
	bool SetFirst( bool bFirst ) {return m_bFirst = bFirst;}

private:	
	CSyncLock m_Sync;
	bool m_bFirst;
	char m_cWorldNumber[PrivateChatChannel::Common::MAXWORLDCOUNT];	
	std::map<INT64,CDNPrivateChaChannel*> m_mChannelInfo[PrivateChatChannel::Common::MAXWORLDCOUNT];	// 채널아이디, 채널정보		
};


extern CDNPrivateChatManager* g_pPrivateChatChannelManager;

#endif // #if defined( pre_privatechat_channel )
