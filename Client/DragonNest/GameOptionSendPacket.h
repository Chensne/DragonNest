
#pragma once

#include "GameClientSession.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"
#include "DNPacket.h"
#include "DnCommonUtil.h"

inline void SendSetGameOption(TGameOptions &Option)
{
	CSGameOptionUpdate packet;
	memset( &packet, 0, sizeof(CSGameOptionUpdate) );

	memcpy_s(&packet.Option, sizeof(TGameOptions), &Option, sizeof(TGameOptions));

	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_UPDATEOPTION, (char*)&packet, sizeof(CSGameOptionUpdate) );
}

inline void SendCharCommOption( UINT nSessionID )
{
	FUNC_LOG();
	CSGameOptionReqComm CharCommOpt;
	memset( &CharCommOpt, 0, sizeof(CSGameOptionReqComm) );

	CharCommOpt.nSessionID = nSessionID;
	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_REQCOMMOPTION, (char*)&CharCommOpt, sizeof(CSGameOptionReqComm) );
}

inline void SendQuestNotifier( DNNotifier::Data (&data)[DNNotifier::RegisterCount::TotalQuest] )
{
	CSGameOptionQuestNotifier Notifier;
	memcpy_s( &Notifier.data, sizeof(Notifier.data), data, sizeof(data) );

	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_QUEST_NOTIFIER, (char*)&Notifier, sizeof(CSGameOptionQuestNotifier) );
}

inline void SendMissionNotifier( DNNotifier::Data (&data)[DNNotifier::RegisterCount::TotalMission] )
{
	CSGameOptionMissionNotifier Notifier;
	memcpy_s( &Notifier.data, sizeof(Notifier.data), data, sizeof(data) );

	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_MISSION_NOTIFIER, (char*)&Notifier, sizeof(CSGameOptionMissionNotifier) );
}

inline void SendKeySetting( bool bDefault, TKeySetting *pKeySetting )
{
	CSGameOptionUpdateKeySetting TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.bIsDefault = bDefault;

	int iSize = sizeof(TxPacket);
	if( !TxPacket.bIsDefault )
		memcpy( &TxPacket.sKeySetting, pKeySetting, sizeof(TKeySetting) );
	else
		iSize = sizeof(TxPacket)-sizeof(TxPacket.sKeySetting);

	CClientSessionManager::GetInstance().SendPacket( CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_UPDATE_KEYSETTING, reinterpret_cast<char*>(&TxPacket), iSize );
}

inline void SendPadSetting( bool bDefault, TPadSetting * pPadSetting )
{
	SCGameOptionSelectPadSetting TxPacket;

	TxPacket.bIsDefault = bDefault;

	int iSize = sizeof(TxPacket);
	if( !TxPacket.bIsDefault )
		memcpy( &TxPacket.sPadSetting, pPadSetting, sizeof(TPadSetting) );
	else
		iSize = sizeof(TxPacket) - sizeof(TxPacket.sPadSetting);

	CClientSessionManager::GetInstance().SendPacket( CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_UPDATE_PADSETTING, reinterpret_cast<char*>(&TxPacket), iSize );
}

inline void SendSecondAuthPassword(int nOldSeed, int nOldValue[], int nNewSeed, int nNewValue[])
{
	CSGameOptionUpdateSecondAuthPassword TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.nOldSeed = nOldSeed;
	memcpy( TxPacket.nOldValue, nOldValue, sizeof(TxPacket.nOldValue) );
	TxPacket.nNewSeed = nNewSeed;
	memcpy( TxPacket.nNewValue, nNewValue, sizeof(TxPacket.nNewValue) );

	CClientSessionManager::GetInstance().SendPacket( CS_GAMEOPTION, eGameOption::CS_UPDATE_SECONDAUTH_PASSWORD, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

inline void SendSecondAuthLock(bool IsLock , int nSeed, int nValue[])
{
	CSGameOptionUpdateSecondAuthLock TxPacket; 
	memset( &TxPacket , 0 , sizeof(TxPacket));

	TxPacket.bIsLock = IsLock; 
	TxPacket.nSeed = nSeed;
	memcpy( TxPacket.nValue, nValue, sizeof(TxPacket.nValue) );


	CClientSessionManager::GetInstance().SendPacket( CS_GAMEOPTION, eGameOption::CS_UPDATE_SECONDAUTH_LOCK, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
};


inline void SendSecondAuthNotifyDate()
{
}


inline void SendSecondAuthInit( int nSeed, int nValue[] )
{
	CSGameOptionInitSecondAuth TxPacket; 
	memset( &TxPacket , 0 , sizeof(TxPacket));

	TxPacket.nSeed = nSeed;
	memcpy( TxPacket.nValue, nValue, sizeof(TxPacket.nValue) );

	CClientSessionManager::GetInstance().SendPacket( CS_GAMEOPTION, eGameOption::CS_INIT_SECONDAUTH, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}



inline void SendSecondAuthValidate( int nSeed, int nValue[], int authCheckType )
{
	CSGameOptionValidateSecondAuth TxPacket;
	memset(&TxPacket, 0, sizeof(TxPacket));

	TxPacket.nSeed = nSeed;
	memcpy( TxPacket.nValue, nValue, sizeof(TxPacket.nValue) );

	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_INIT_SECONDAUTH, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket));
}

inline void SendSetProfile( BYTE cGender, const WCHAR *pPR, bool bOpenPublic )
{
	CSGameOptionSetProfile Profile;
	memset( &Profile, 0, sizeof(Profile) );

	Profile.sProfile.cGender = cGender;
	_wcscpy(Profile.sProfile.wszGreeting, _countof(Profile.sProfile.wszGreeting), pPR, (int)wcslen(pPR));
	Profile.sProfile.bOpenPublic = bOpenPublic;
	
	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_SET_PROFILE, reinterpret_cast<char*>(&Profile), sizeof(Profile));
}

inline void SendGetProfile()
{
	CClientSessionManager::GetInstance().SendPacket(CS_GAMEOPTION, eGameOption::CS_GAMEOPTION_GET_PROFILE, NULL, 0);
}

#ifdef PRE_ADD_DOORS
inline void SendGetDoorsAuthKey()
{
	CClientSessionManager::GetInstance().SendPacket(CS_DOORS, eDoors::CS_GET_AUTHKEY, NULL, 0);
}

inline void SendCancelDoorsAuthKey()
{
	CClientSessionManager::GetInstance().SendPacket(CS_DOORS, eDoors::CS_CANCEL_AUTH, NULL, 0);
}
#endif	// #ifdef PRE_ADD_DOORS