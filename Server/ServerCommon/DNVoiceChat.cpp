
#include "Stdafx.h"

#ifdef _USE_VOICECHAT
#include "DNVoiceChat.h"
#include "Log.h"

CDNVoiceChat * g_pVoiceChat;

CDNVoiceChat::CDNVoiceChat()
{
	m_nVoiceChatIndex = 0;
	memset(m_szGameName, 0, sizeof(m_szGameName));
	memset(m_szAudioServerIP, 0, sizeof(m_szAudioServerIP));
	m_nAudioServerPort = 0;
	m_nPreTick = 0;
	m_pICE = NULL;
}

CDNVoiceChat::~CDNVoiceChat()
{
}

bool CDNVoiceChat::Initialize(int nIndex, const char * pGameName, const char * pAudioServerIP, unsigned short nPort)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::Initialize(pGameName, pAudioServerIP, nPort);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}

	m_nVoiceChatIndex = nIndex;
	_strcpy(m_szGameName, _countof(m_szGameName), pGameName, (int)strlen(pGameName));
	_strcpy(m_szAudioServerIP, _countof(m_szAudioServerIP), pAudioServerIP, (int)strlen(pAudioServerIP));
	m_nAudioServerPort = nPort;
	return true;
}

bool CDNVoiceChat::CreateVoiceChannel(UINT nVoiceChannelID, int nHearingRange)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::CreateVoiceChannel(nVoiceChannelID, nHearingRange);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

bool CDNVoiceChat::DestroyVoiceChannel(UINT nVoiceChannelID)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::DestroyVoiceChannel(nVoiceChannelID);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

bool CDNVoiceChat::JoinChannel(UINT nChannelID, UINT nUserAccountDBID, const char * pUserName, const char * pUserIP, bool bUsePremium, int &nJoinRet)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::IceConnect(nUserAccountDBID, pUserName, pUserIP);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}

	if (bUsePremium)
	{
		IceRet = SetPremium(nUserAccountDBID);
		if (IceRet != ICE_ERROR_NONE)
			VoiceErrorLog(IceRet);
	}

	IceRet = CDNVoiceChatBase::ListenJoin(nChannelID, nUserAccountDBID);
	if (IceRet != ICE_ERROR_NONE)
		VoiceErrorLog(IceRet);
	else
		nJoinRet = _VOICEJOINTYPE_LISTENONLY;
	
	IceRet = CDNVoiceChatBase::TalkJoin(nChannelID, nUserAccountDBID);
	if (IceRet != ICE_ERROR_NONE)
		VoiceErrorLog(IceRet);
	else
		nJoinRet = nJoinRet == _VOICEJOINTYPE_NONE ? _VOICEJOINTYPE_TALKONLY : _VOICEJOINTYPE_TALK_N_LISTEN;
	return true;
}

void CDNVoiceChat::LeaveChannel(UINT nChannelID, UINT nUserAccountDBID)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	VoiceErrorLog(CDNVoiceChatBase::TalkLeave(nChannelID, nUserAccountDBID));
	VoiceErrorLog(CDNVoiceChatBase::ListenLeave(nChannelID, nUserAccountDBID));
	VoiceErrorLog(CDNVoiceChatBase::IceDisconnect(nUserAccountDBID));
}

bool CDNVoiceChat::MuteOneToOne(UINT nUserAccountDBID, UINT nMuteUserID, bool bIsMute)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::MuteOneToOne(nUserAccountDBID, nMuteUserID, bIsMute);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

//bool CDNVoiceChat::MuteOneToMany(UINT nUserAccountDBID, const UINT * pMuterUserIDArr, const BYTE * pMuteFlagArr, const UINT nLen)
//{
//	ScopeLock <CSyncLock> lock(m_Sync);
//	if (!pMuteFlagArr || !pMuterUserIDArr) return false;
//	ICE_ERROR IceRet = CDNVoiceChatBase::MuteOneToMany(nUserAccountDBID, pMuterUserIDArr, pMuteFlagArr, nLen);
//	if (IceRet != ICE_ERROR_NONE)
//	{
//		VoiceErrorLog(IceRet);
//		return false;
//	}
//	return true;
//}

bool CDNVoiceChat::ComplaintRequest(UINT nComplainerUserID, UINT nHasComplainee, UINT nComplaineeUserID, const char * pCategory, const char * pSubject, const char * pMsg)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	if (!pCategory || !pSubject || !pMsg) return false;
	ICE_ERROR IceRet = CDNVoiceChatBase::ComplaintRequest(nComplainerUserID, nHasComplainee, nComplaineeUserID, pCategory, pSubject, pMsg);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

bool CDNVoiceChat::SetUserPos(UINT nPlayerID, int nX, int nY, int nZ, USHORT nRotation)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	ICE_ERROR IceRet = CDNVoiceChatBase::SetUserPos(nPlayerID, nX, nY, nZ, nRotation);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

//void CDNVoiceChat::DoUpdate(ULONG nCurTick)
//{
//	//The libICE function should be called every 100ms
//	ScopeLock <CSyncLock> lock(m_Sync);
//	if (m_nPreTick == 0) m_nPreTick = nCurTick;
//	if (nCurTick > m_nPreTick + 500)		//we don't need fast update
//	{
//		if (m_pICE)
//			ICE_null(m_pICE);
//		m_nPreTick = nCurTick;
//	}
//}

void CDNVoiceChat::VoiceErrorLog(ICE_ERROR err)
{
	if (err == ICE_ERROR_NONE) return;
	g_Log.Log( LogType::_ERROR, L"VoiceChat Error[%S]\n", IceErrorString(err) );

}

bool CDNVoiceChat::IsChannelSpartial(UINT nVoiceChannelID)
{
	int nProperty;
	unsigned int nSize = sizeof(int);
	if (GetChannelProperty(nVoiceChannelID, ICE_CHAN_SPATIAL, &nProperty, &nSize))
		return nProperty == 0 ? false : true;
	return false;
}

bool CDNVoiceChat::GetChannelHearingRange(UINT nVoiceChannelID, int * pRange)
{
	unsigned int nSize = sizeof(int);
	return GetChannelProperty(nVoiceChannelID, ICE_CHAN_HEARING_RANGE, pRange, &nSize);
}

bool CDNVoiceChat::IsUserTalking(UINT nUserID, BYTE * pTalking)
{
	//note : non zero is talking, indicative volume 0 - 255 to out value
	ICE_player_status Status;
	memset(&Status, 0, sizeof(ICE_player_status));

	if (GetUserStatus(nUserID, &Status))
	{
		*pTalking = Status.talking;
		return true;
	}
	return false;
}

bool CDNVoiceChat::GetChannelProperty(UINT nVoiceChannelID, ICE_CHAN_PROPERTY Ice_Prop, void * pVal, unsigned int * pSize)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	if (!pVal || !pSize) return false;
	ICE_ERROR IceRet = CDNVoiceChatBase::GetChannelProperty(nVoiceChannelID, Ice_Prop, pVal, pSize);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

bool CDNVoiceChat::SetChannelProperty(UINT nChannelID, const ICE_CHAN_PROPERTY Ice_Prop, const void * pVal, const unsigned int nLen)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	if (!pVal) return false;
	ICE_ERROR IceRet = CDNVoiceChatBase::SetChannelProperty(nChannelID, Ice_Prop, pVal, nLen);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

bool CDNVoiceChat::GetUserStatus(UINT nUserID, ICE_player_status * pStatus)
{
	ScopeLock <CSyncLock> lock(m_Sync);
	if (!pStatus) return false;
	ICE_ERROR IceRet = CDNVoiceChatBase::GetUserStatus(nUserID, pStatus);
	if (IceRet != ICE_ERROR_NONE)
	{
		VoiceErrorLog(IceRet);
		return false;
	}
	return true;
}

#endif