#pragma once

#if defined( PRE_ALTEIAWORLD_EXPLORE )

#include "Task.h"
#include "MessageListener.h"

class CDnAlteaTask : public CTask, public CTaskListener, public CSingleton<CDnAlteaTask>
{
public:
	enum
	{
		E_MAPTYPE_WORLDMAP = 2,
		E_MAPTYPE_DUNGEON = 3,

		E_DICE_MAXNUMBER = 6,
		E_DICE_UP_NUMBER = 5,
	};

public:
	CDnAlteaTask();
	virtual ~CDnAlteaTask();

	bool Initialize();
	void Finalize();

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );
	virtual void OnRecvAlteaMessage( int nSubCmd, char *pData, int nSize );

	void OnRecvJoinInfo( AlteiaWorld::SCAlteiaWorldJoinInfo * pPacket );
	void OnRecvPrivateGoldKeyRankingInfo( AlteiaWorld::SCAlteiaWorldPrivateGoldKeyRankingInfo * pPacket );
	void OnRecvPrivatePlayTimeRankingInfo( AlteiaWorld::SCAlteiaWorldPrivatePlayTimeRankingInfo * pPacket );
	void OnRecvGuildGoldKeyRankingInfo( AlteiaWorld::SCAlteiaWorldGuildGoldKeyRankingInfo * pPacket );
	void OnRecvSendTicketInfo( AlteiaWorld::SCAlteiaWorldSendTicketInfo * pPacket );
	void OnRecvJoinResult( AlteiaWorld::SCAlteiaWorldJoinResult * pPacket );
	void OnRecvSendTicket( AlteiaWorld::SCAlteiaWorldSendTicket * pPacket );
	void OnRecvSendTicketResult( AlteiaWorld::SCAlteiaWorldSendTicketResult * pPacket );
	void OnRecvDiceResult( AlteiaWorld::SCAlteiaWorldDiceResult * pPacket );

	void RequestAlteaInfo( const AlteiaWorld::Info::eInfoType eType );
	void RequestAlteaJoin();
	void RequestAlteaSendTicket( const WCHAR * wszCharacterName );
	void RequestAlteaDice();
	void RequestAlteaQuitInfo();
	void RequestAlteaNextMap();

public:
	void RefreshTabDlg();
	void RefreshCheckDlg();
	void RefreshPersonalGoldKeyRankDlg();
	void RefreshPersonalPlayTimeRankDlg();
	void RefreshGuildGoldKeyRankDlg();
	void RefreshInviteDlg();

	bool IsAlteaWorldMap( const int nMapIndex );

protected:
	bool m_bRequestWait;

	int m_nDailyClearCount;
	int m_nWeeklyClearCount;
	int m_nTicketCount;

	int m_nMyBestGoldKeyCount;
	UINT m_nPlaySec;

	int m_nSendTicketCount;

	std::wstring m_wszSendUserName;

	std::vector<AlteiaWorld::GoldKeyRankMemberInfo> m_vPersonalGoldKey;
	std::vector<AlteiaWorld::PlayTimeRankMemberInfo> m_vPersonalPlayTime;
	std::vector<AlteiaWorld::GuildGoldKeyRankInfo> m_vGuildGoldKey;

	std::vector<INT64> m_vSendUser;
	std::vector<int> m_vAlteaWorldMap;
};

#define GetAlteaTask()	CDnAlteaTask::GetInstance()

#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )