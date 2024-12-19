
#pragma once

#ifdef _USE_VOICECHAT
#include "./VoiceChat/VoiceChatServer.h"

class CDNVoiceChat : public CDNVoiceChatBase
{
public:
	CDNVoiceChat();
	~CDNVoiceChat();

	bool Initialize(int nIndex, const char * pGameName, const char * pAudioServerIP, unsigned short nPort);

	//VoiceChannel
	bool CreateVoiceChannel(UINT nVoiceChannelID, int nHearingRange = 10000);
	bool DestroyVoiceChannel(UINT nVoiceChannelID);

	bool JoinChannel(UINT nChannelID, UINT nUserAccountDBID, const char * pUserName, const char * pUserIP, bool bUsePremium, int &nJoinRet);
	void LeaveChannel(UINT nChannelID, UINT nUserAccountDBID);

	//Mute
	bool MuteOneToOne(UINT nUserAccountDBID, UINT nMuteUserID, bool bIsMute);
	//bool MuteOneToMany(UINT nUserAccountDBID, const UINT * pMuterUserIDArr, const BYTE * pMuteFlagArr, const UINT nLen);

	//Complaint
	bool ComplaintRequest(UINT nComplainerUserID, UINT nHasComplainee, UINT nComplaineeUserID, const char * pCategory, const char * pSubject, const char * pMsg);

	//UserPos : Spartial Channel Only
	bool SetUserPos(UINT nPlayerID, int nX, int nY, int nZ, USHORT nRotation);

	//ICE update
	//void DoUpdate(ULONG nCurTick);

	//Human Readable Error
	void VoiceErrorLog(ICE_ERROR err);

	//Get Channel & User Status & property
	bool IsChannelSpartial(UINT nVoiceChannelID);
	bool GetChannelHearingRange(UINT nVoiceChannelID, int * pRange);
	bool IsUserTalking(UINT nUserID, BYTE * pTalking);

	bool GetChannelProperty(UINT nVoiceChannelID, ICE_CHAN_PROPERTY Ice_Prop, void * pVal, unsigned int * pSize);
	bool GetUserStatus(UINT nUserID, ICE_player_status * pStatus);

	//Set Channel Property
	bool SetChannelProperty(UINT nChannelID, const ICE_CHAN_PROPERTY Ice_Prop, const void * pVal, const unsigned int nLen);

private:
	int m_nVoiceChatIndex;
	char m_szGameName[64];
	char m_szAudioServerIP[IPLENMAX];
	USHORT m_nAudioServerPort;
	ULONG m_nPreTick;

	CSyncLock m_Sync;
};

extern CDNVoiceChat * g_pVoiceChat;

#endif