
#pragma once

#include "DNWorldUserState.h"

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

class CDNVillageWorldUserState:public CDNWorldUserState
{
public:
	
	CDNVillageWorldUserState():CDNWorldUserState()
	{
	}
	virtual ~CDNVillageWorldUserState()
	{
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
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::GetUserState( pName, biCharacterDBID, pWorldUserState );
	}

	virtual bool GetUserState( INT64 biCharacterDBID, sWorldUserState *pWorldUserState )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::GetUserState( biCharacterDBID, pWorldUserState );
	}

	virtual bool GetUserState( INT64 biCharacterDBID, TCommunityLocation& Location )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::GetUserState( biCharacterDBID, Location );
	}

	virtual int GetUserLocation(const WCHAR * pName, INT64 biCharacterDBID)
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::GetUserLocation( pName, biCharacterDBID );
	}

	virtual int GetUserCummunityState(const WCHAR * pName, INT64 biCharacterDBID)
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		return CDNWorldUserState::GetUserCummunityState( pName, biCharacterDBID );
	}

protected:
	
	CSyncLock m_Sync;
};

#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
