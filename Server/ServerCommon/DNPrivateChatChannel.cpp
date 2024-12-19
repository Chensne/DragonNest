#include "Stdafx.h"
#include "DNUserSession.h"
#include "DNMasterConnection.h"
#include "DNPrivateChatChannel.h"
#if defined(_VILLAGESERVER)
#include "DNUserSessionManager.h"
#endif

#if defined( PRE_PRIVATECHAT_CHANNEL )

CDNPrivateChaChannel::CDNPrivateChaChannel()
{
	m_bLoad = false;
	m_ChannelInfo.nPrivateChatChannelID = 0;
	memset(m_ChannelInfo.wszName, 0, sizeof(WCHAR)*PrivateChatChannel::Common::MaxNameLen);
	m_ChannelInfo.biMasterCharacterDBID = 0;
	m_ChannelInfo.nPassWord = 0;
	m_ChannelInfo.nMemberCount = 0;

	m_mJoinMembers.clear();
}
CDNPrivateChaChannel::~CDNPrivateChaChannel() 
{
};

void CDNPrivateChaChannel::SetPrivateChannelInfo( const TPrivateChatChannelInfo &ChannelInfo )  
{
	m_ChannelInfo.nPrivateChatChannelID = ChannelInfo.nPrivateChatChannelID;
	_wcscpy(m_ChannelInfo.wszName, _countof(m_ChannelInfo.wszName), ChannelInfo.wszName, (int)wcslen(ChannelInfo.wszName));
	m_ChannelInfo.biMasterCharacterDBID = ChannelInfo.biMasterCharacterDBID;
	m_ChannelInfo.nPassWord = ChannelInfo.nPassWord;
}

void CDNPrivateChaChannel::AddPrivateChannelMember(const TPrivateChatChannelMember &Member )
{
	ScopeLock <CSyncLock> Lock(m_Sync);			
	
	if(Member.bMaster)
	{
		m_ChannelInfo.biMasterCharacterDBID = Member.biCharacterDBID;
	}

#if defined( _VILLAGESERVER )	
	CDNUserSession* pSession = NULL;
	for(std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.begin(); itor != m_mJoinMembers.end();itor++)
	{
		pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( itor->second.nAccountDBID );	
		if(pSession)
			pSession->SendPrivateChatChannelMemberAdd( ERROR_NONE, Member );
		
	}
#endif
	m_mJoinMembers.insert(std::make_pair(Member.biCharacterDBID, Member));
}

void CDNPrivateChaChannel::DelPrivateChannelMember(  PrivateChatChannel::Common::eModType eType, INT64 biCharacterDBID )
{
	ScopeLock <CSyncLock> Lock(m_Sync);
	TPrivateChatChannelMember Member;
	memset(&Member, 0, sizeof(Member));

	std::map<INT64,TPrivateChatChannelMember>::iterator ii = m_mJoinMembers.find(biCharacterDBID);
	if(ii != m_mJoinMembers.end() )
	{
		Member = ii->second;
		m_mJoinMembers.erase(ii);
	}
	else
	{
		g_Log.Log(LogType::_ERROR, L"DelPrivateChannelMember m_mJoinMembers not Exist Member\r\n");		
		return;
	}

#if defined( _VILLAGESERVER )	
	CDNUserSession* pSession = NULL;

	int nSubCmd = ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_DEL;

	if(eType == PrivateChatChannel::Common::KickMember)
	{
		nSubCmd = ePrivateChatChannel::SC_PRIVATECHAT_CHANNEL_KICK;
	}

	for(std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.begin(); itor != m_mJoinMembers.end();itor++)
	{
		pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( itor->second.nAccountDBID );
		if(pSession)
		{
			if(eType == PrivateChatChannel::Common::KickMember)
				pSession->SendPrivateChatChannelMemberKick(ERROR_NONE, Member.biCharacterDBID, Member.wszCharacterName);
			else
				pSession->SendPrivateChatChannelMemberDel(ERROR_NONE, Member.biCharacterDBID, Member.wszCharacterName);
		}

	}
#endif
}

void CDNPrivateChaChannel::ModPrivateChannelInfo( PrivateChatChannel::Common::eModType eType, int nPassWord, INT64 biMasterCharacterID )
{	
	ScopeLock <CSyncLock> Lock(m_Sync);
	WCHAR wszName[NAMELENMAX];
	memset(wszName, 0, sizeof(wszName));

	if( eType == PrivateChatChannel::Common::ChangeMaster )
	{
		std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(m_ChannelInfo.biMasterCharacterDBID);
		if( itor != m_mJoinMembers.end() )
		{
			itor->second.bMaster = false;
		}
		if(biMasterCharacterID == 0)
			m_ChannelInfo.biMasterCharacterDBID = biMasterCharacterID;
		else
		{
			itor = m_mJoinMembers.find(biMasterCharacterID);
			if( itor != m_mJoinMembers.end() )
			{
				itor->second.bMaster = true;
				m_ChannelInfo.biMasterCharacterDBID = biMasterCharacterID;				
			}
		}		
	}
	else if( eType == PrivateChatChannel::Common::ChangePassWord )
	{
		m_ChannelInfo.nPassWord = nPassWord;
	}

	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(biMasterCharacterID);
	if( itor != m_mJoinMembers.end() )
	{
		_wcscpy(wszName, _countof(wszName), itor->second.wszCharacterName, (int)wcslen(itor->second.wszCharacterName));
	}
	

#if defined( _VILLAGESERVER )	
	CDNUserSession* pSession = NULL;	
	for(std::map<INT64,TPrivateChatChannelMember>::iterator itor1 = m_mJoinMembers.begin(); itor1 != m_mJoinMembers.end();itor1++)
	{
		pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( itor1->second.nAccountDBID );
		if(pSession)
		{
			pSession->SendPrivateChatChannelInfoMod( ERROR_NONE, eType, wszName, biMasterCharacterID );
		}
	}	
#endif
}

void CDNPrivateChaChannel::ModPrivateChannelMemberName( INT64 biCharacterDBID, WCHAR* wszName )
{	
	if(!wszName)
		return;
	ScopeLock <CSyncLock> Lock(m_Sync);
	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(biCharacterDBID);
	if( itor != m_mJoinMembers.end() )
	{
		_wcscpy(itor->second.wszCharacterName, _countof(itor->second.wszCharacterName), wszName, (int)wcslen(wszName));
	}
	else
		return;


#if defined( _VILLAGESERVER )	
	CDNUserSession* pSession = NULL;
	for(std::map<INT64,TPrivateChatChannelMember>::iterator itor1 = m_mJoinMembers.begin(); itor1 != m_mJoinMembers.end();itor1++)
	{
		pSession = g_pUserSessionManager->FindUserSessionByAccountDBID( itor1->second.nAccountDBID );	
		if(pSession)
			pSession->SendPrivateChatChannelMemberAdd( ERROR_NONE, itor->second );

	}
#endif
}

void CDNPrivateChaChannel::GetPrivateChannelMember( std::list<TPrivateChatChannelMember> &MemberList )
{	
	if(m_mJoinMembers.empty())
		return;
	ScopeLock <CSyncLock> Lock(m_Sync);
	for (std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.begin(); itor != m_mJoinMembers.end(); itor++)
	{
		MemberList.push_back((*itor).second);
	}	
}

TPrivateChatChannelMember CDNPrivateChaChannel::GetPrivateChannelMember( INT64 biCharacterDBID )
{	
	TPrivateChatChannelMember Member;
	memset(&Member, 0, sizeof(Member));

	ScopeLock <CSyncLock> Lock(m_Sync);
	if(m_mJoinMembers.empty())
		return Member;
	
	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(biCharacterDBID);
	if(itor != m_mJoinMembers.end())
	{
		Member = itor->second;
	}
	return Member;	
}

bool CDNPrivateChaChannel::CheckPrivateChannelMember( INT64 biCharacterDBID )
{	
	if(biCharacterDBID <= 0)
		return false;
	ScopeLock <CSyncLock> Lock(m_Sync);
	if(m_mJoinMembers.empty())
		return false;

	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(biCharacterDBID);
	if(itor != m_mJoinMembers.end())
	{
		return true;
	}
	return false;	
}

void CDNPrivateChaChannel::SetPrivateChannelMemberAccountDBID(UINT nAccountDBID, INT64 biCharacterDBID)
{	
	ScopeLock <CSyncLock> Lock(m_Sync);
	if(m_mJoinMembers.empty())
		return;

	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(biCharacterDBID);
	if(itor != m_mJoinMembers.end())
	{
		(*itor).second.nAccountDBID = nAccountDBID;
	}	
}

TPrivateChatChannelMember CDNPrivateChaChannel::GetPrivateChannelMember( WCHAR* wszName )
{	
	TPrivateChatChannelMember Member;
	memset(&Member, 0, sizeof(Member));

	ScopeLock <CSyncLock> Lock(m_Sync);
	if(m_mJoinMembers.empty())
		return Member;

	for (std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.begin(); itor != m_mJoinMembers.end(); itor++)
	{
		if (wcscmp(itor->second.wszCharacterName, wszName) == 0)
		{
			Member = itor->second;
			break;
		}
	}	
	return Member;	
}

void CDNPrivateChaChannel::GetMasterCharacterName(WCHAR *MasterName)
{		
	ScopeLock <CSyncLock> Lock(m_Sync);
	std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.find(m_ChannelInfo.biMasterCharacterDBID);
	
	if( itor != m_mJoinMembers.end() )
	{			
		_wcscpy(MasterName, NAMELENMAX, itor->second.wszCharacterName, (int)wcslen(itor->second.wszCharacterName));
	}	
}

INT64 CDNPrivateChaChannel::GetNextMasterCharacterDBID()
{	
	ScopeLock <CSyncLock> Lock(m_Sync);

	if(m_mJoinMembers.empty())
		return 0;
	INT64 biMasterCharacterDBID = 0;
	time_t tJoinDate = 0;
	
	for (std::map<INT64,TPrivateChatChannelMember>::iterator itor = m_mJoinMembers.begin(); itor != m_mJoinMembers.end(); itor++)
	{
		if( !itor->second.bMaster && (tJoinDate == 0 || tJoinDate > itor->second.tJoinDate) )
		{
			biMasterCharacterDBID = itor->second.biCharacterDBID;
			tJoinDate = itor->second.tJoinDate;
		}
	}	
	return biMasterCharacterDBID;
}
#endif