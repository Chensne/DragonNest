#pragma once

#include "Task.h"
#include "MessageListener.h"


#ifdef PRE_PRIVATECHAT_CHANNEL

class CDnChannelChatTask : public CTask, public CTaskListener, public CSingleton<CDnChannelChatTask>
{
public:
	CDnChannelChatTask();
	virtual ~CDnChannelChatTask();

public:
	bool Initialize();
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

private:
	void OnRecvChannelInfo( PrivateChatChannel::SCPrivateChannleInfo* pPacket );
	void OnRecvChannelJoinResult( PrivateChatChannel::SCPrivateChatChannleJoinResult* pPacket );
	void OnRecvChannelOutResult( PrivateChatChannel::SCPrivateChatChannleOutResult* pPacket );
	void OnRecvChannelAdd( PrivateChatChannel::SCPrivateChatChannleAdd* pPacket );
	void OnRecvChannelDel( PrivateChatChannel::SCPrivateChatChannleDel* pPacket );
	void OnRecvChannelMod( PrivateChatChannel::SCPrivateChatChannleMod* pPacket );
	void OnRecvChannelKick( PrivateChatChannel::SCPrivateChatChannleKick* pPacket );
	void OnRecvChannelResult( PrivateChatChannel::SCPrivateChatChannleResult* pPacket );
	void OnRecvChannelInviteResult( PrivateChatChannel::SCPrivateChatChannleInviteResult* pPacket );
	void OnRecvChannelKickResult( PrivateChatChannel::CSPrivateChatChannleKickResult* pPacket );

	void SortChannelMemberInfo();

public:
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	void SendChannelAdd( const WCHAR* pChannelName, int nChannelPassword );
#else // PRE_ADD_PRIVATECHAT_CHANNEL
	void SendChannelAdd( const WCHAR* pChannelName );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
	void SendChannelJoin( const WCHAR* pChannelName, int nChannelPassword );
	void SendChannelOut();
	void SendChannelInvite( const WCHAR* pInvitePlayerName );
	void SendChannelMod( int nModType, int nPassword, INT64 biCharacterDBID = 0 );
	void SendChannelKick( const WCHAR* pKickPlayerName );

public:
	bool IsJoinChannel() { return m_bJoinChannel; }
	bool IsChannelMaster() { return m_bChannelMaster; }
	std::wstring GetChannelName() { return m_strChannelName; }
	std::vector<PrivateChatChannel::TMemberInfo>& GetChannelMemeberInfoList() { return m_vecChannelMemberInfo; }
	PrivateChatChannel::TMemberInfo* GetChannelMemberInfoByDBID( INT64 nCharacterDBID );
	PrivateChatChannel::TMemberInfo* GetChannelMemberInfoByName( const WCHAR* pstrCharacterName );

private:
	bool			m_bJoinChannel;
	bool			m_bChannelMaster;
	std::wstring	m_strChannelName;
	std::vector<PrivateChatChannel::TMemberInfo>	m_vecChannelMemberInfo;
};

#define GetChannelChatTask()		CDnChannelChatTask::GetInstance()

#endif // PRE_PRIVATECHAT_CHANNEL
