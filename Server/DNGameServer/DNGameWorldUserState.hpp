
#pragma once

#include "DNWorldUserState.h"

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

const DWORD SNAPSHOT_UPDATE_TICK = (60*1000);

class CDNGameWorldUserState:public CDNWorldUserState
{
public:

	CDNGameWorldUserState():CDNWorldUserState()
	{
		m_dwSnapShotTick = 0;
		m_pGetOnlyCharNameUserState = &m_SnapShotWorldUserStateList;
		m_pGetOnlyCharDBIDUserState = &m_SnapShotWorldUserStateListByCharDBID;
	}
	virtual ~CDNGameWorldUserState()
	{
	}

	virtual void SnapShotUserState()
	{
		DWORD dwTick = timeGetTime();
		if( dwTick-m_dwSnapShotTick < SNAPSHOT_UPDATE_TICK )
			return;
		m_dwSnapShotTick = dwTick;

		ScopeLock<CSyncLock> Lock( m_Sync );
		ScopeLock<CSyncLock> SnapShotLock( m_SnapShotSync );

		m_SnapShotWorldUserStateList = m_WorldUserStateList;
		m_SnapShotWorldUserStateListByCharDBID = m_WorldUserStateListByCharDBID;
	}

	virtual bool AddUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocation, int nCummunity, int nChannelID, int nMapIdx)
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::AddUserState( pName, biCharacterDBID, nLocation, nCummunity, nChannelID, nMapIdx );
	}
	virtual bool DelUserState(const WCHAR * pName, INT64 biCharacterDBID)
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::DelUserState( pName, biCharacterDBID );
	}

	virtual bool UpdateUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocation = -1, int nCummunity = -1, int nChannelID = -1, int nMapIdx = -1)
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::UpdateUserState( pName, biCharacterDBID, nLocation, nCummunity, nChannelID, nMapIdx );
	}

	virtual bool GetUserState(const WCHAR * pName, INT64 biCharacterDBID, sWorldUserState *pWorldUserState)
	{
		ScopeLock<CSyncLock> Lock( m_SnapShotSync );
		return CDNWorldUserState::GetUserState( pName, biCharacterDBID, pWorldUserState );
	}

	virtual bool GetUserState( INT64 biCharacterDBID, sWorldUserState *pWorldUserState )
	{
		ScopeLock<CSyncLock> Lock( m_SnapShotSync );
		return CDNWorldUserState::GetUserState( biCharacterDBID, pWorldUserState );
	}

	virtual bool GetUserState( INT64 biCharacterDBID, TCommunityLocation& Location )
	{
		ScopeLock<CSyncLock> Lock( m_SnapShotSync );
		return CDNWorldUserState::GetUserState( biCharacterDBID, Location );
	}

	virtual int GetUserLocation(const WCHAR * pName, INT64 biCharacterDBID)
	{
		ScopeLock<CSyncLock> Lock( m_SnapShotSync );
		return CDNWorldUserState::GetUserLocation( pName, biCharacterDBID );
	}

	virtual int GetUserCummunityState(const WCHAR * pName, INT64 biCharacterDBID)
	{
		ScopeLock<CSyncLock> Lock( m_SnapShotSync );
		return CDNWorldUserState::GetUserCummunityState( pName, biCharacterDBID );
	}

protected:

	CSyncLock m_SnapShotSync;
	CSyncLock m_Sync;
	CDNWorldUserState::TCharNameUserState m_SnapShotWorldUserStateList;
	CDNWorldUserState::TCharDBIDUserState m_SnapShotWorldUserStateListByCharDBID;
	DWORD m_dwSnapShotTick;
};

#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
