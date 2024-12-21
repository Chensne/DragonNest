#pragma once

#ifdef _USE_VOICECHAT

#include "./DolbyAxon/IceLib64/ICE.h"
//const _ICE_HEARING_PROFILE_POINT HearingPoint[] = { {0.0f, 1.0f}, {0.015f, 1.0f}, {0.3f, 0.9f}, {0.5f, 0.5f}, {0.65f, 0.25f}, {1.0f, 0.2f}, {0, 0} };
const _ICE_HEARING_PROFILE_POINT HearingPoint[] = { {0.0f, 1.0f}, {0.15f, 0.4f}, {0.3f, 0.3f}, {0.5f, 0.25f}, {0.65f, 0.2f}, {1.0f, 0.2f} };

class CDNVoiceChatBase
{
public :
	CDNVoiceChatBase();
	virtual ~CDNVoiceChatBase();
	ICE_ERROR Initialize(const char * pGameName, const char * pAudioServerIP, unsigned short nPort);
	void Finalize();

	ICE_ERROR CreateVoiceChannel(UINT nVoiceChannelID, int nHearingRange = 10000);
	ICE_ERROR DestroyVoiceChannel(UINT nChannelID);	

	ICE_ERROR IceConnect(UINT nUserID, const char * pUserName, const char * pUserIP);
	ICE_ERROR IceDisconnect(UINT nUserID);

	ICE_ERROR SetPremium(UINT nUserID);
	ICE_ERROR ListenJoin(UINT nChannelID, UINT nUserID);
	ICE_ERROR ListenLeave(UINT nChannelID, UINT nUserID);
	ICE_ERROR TalkJoin(UINT nChannelID, UINT nUserID);
	ICE_ERROR TalkLeave(UINT nChannelID, UINT nUserID);

	ICE_ERROR MuteOneToOne(UINT nUserID, UINT nMuteUserID, bool bIsMute);
	//ICE_ERROR MuteOneToMany(UINT nUserID, const UINT * pMuterUserIDArr, const BYTE * pMuteFlagArr, const UINT nLen);

	ICE_ERROR ComplaintRequest(UINT nComplainerUserID, UINT nHasComplainee, UINT nComplaineeUserID, const char * pCategory, const char * pSubject, const char * pMsg);

	ICE_ERROR SetUserPos(UINT nPlayerID, int nX, int nY, int nZ, USHORT nRotation);

	ICE_ERROR SetChannelProperty(UINT nChannelID, const ICE_CHAN_PROPERTY Ice_Prop, const void * pVal, const unsigned int nLen);

	ICE_ERROR GetChannelProperty(UINT nVoiceChannelID, ICE_CHAN_PROPERTY Ice_Prop, void * pVal, unsigned int * pSize);
	ICE_ERROR GetUserStatus(UINT nUserID, ICE_player_status * pStatus);

	const char * IceErrorString(const ICE_ERROR err) { return ICE_error_str(err); }
	//virtual void DoUpdate(ULONG nCurTick) = 0;

protected:
	ICE * m_pICE;	
};
#endif