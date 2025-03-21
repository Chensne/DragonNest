//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#ifndef REMOTE_STORAGE_H
#define REMOTE_STORAGE_H

#include "SpaceWar.h"
#include "GameEngine.h"

class ISteamUser;
class CSpaceWarClient;
class IRemoteStorageSync;
class CRemoteStorageScreen;

enum ERemoteStorageSyncMenuCommand
{
	k_EMenuCommandNone = 0,
	k_EMenuCommandProgress = 1,
	k_EMenuCommandSyncComplete = 2,
	k_EMenuCommandKeepClient = 3,
	k_EMenuCommandKeepServer = 4
};

//-----------------------------------------------------------------------------
// Purpose: Example of Steam Cloud
//-----------------------------------------------------------------------------
class CRemoteStorage
{
public:

	// Constructor
	CRemoteStorage( IGameEngine *pGameEngine );
	~CRemoteStorage();

	// call when user changes to this menu
	void Show();

	// Display the remote storage screen
	void Render();

	// A sync menu item has been selected
	void OnMenuSelection( ERemoteStorageSyncMenuCommand selection );

private:
	void CheckState();

	enum ERemoteStorageState
	{
		k_ERemoteStorageStateIdle,
		k_ERemoteStorageStateSyncToClient,
		k_ERemoteStorageStateDisplayMessage,
		k_ERemoteStorageStateSyncToServer,
	};

	IGameEngine *m_pGameEngine;
	ERemoteStorageState m_eState;
	IRemoteStorageSync *m_pRemoteStorageSync;
	CRemoteStorageScreen *m_pRemoteStorageScreen;
};


//-----------------------------------------------------------------------------
// Purpose: Screen where user can enter their custom messasge
//-----------------------------------------------------------------------------
class CRemoteStorageScreen
{
public:
	CRemoteStorageScreen( IGameEngine *pGameEngine );

	// call when user changes to this menu
	void Show();

	// Display the remote storage screen
	void Render();

	// Done showing this page?
	bool BFinished() { return m_bFinished; }

private:
	void GetFileStats();
	void LoadMessage();

	// Game engine
	IGameEngine *m_pGameEngine;

	// Display font
	HGAMEFONT m_hDisplayFont;

	// Steam User interface
	ISteamUser *m_pSteamUser;

	// Steam RemoteStorage interface
	ISteamRemoteStorage *m_pSteamRemoteStorage;

	// Greeting message
	char m_rgchGreeting[40];
	char m_rgchGreetingNext[40];

	bool m_bFinished;

	int32 m_nNumFilesInCloud;
	int32 m_nBytesQuota;
	int32 m_nAvailableBytes;
};



#endif
