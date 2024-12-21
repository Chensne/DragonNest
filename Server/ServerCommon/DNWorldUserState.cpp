
#include "stdafx.h"
#include "DNWorldUserState.h"
#include "Log.h"

CDNWorldUserState * g_pWorldUserState;

CDNWorldUserState::CDNWorldUserState()
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	m_pGetOnlyCharNameUserState = &m_WorldUserStateList;
	m_pGetOnlyCharDBIDUserState = &m_WorldUserStateListByCharDBID;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
}

CDNWorldUserState::~CDNWorldUserState()
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );

	for ( TCharNameUserState::iterator ii = m_WorldUserStateList.begin(); ii != m_WorldUserStateList.end(); ii++)
		SAFE_DELETE((*ii).second);
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

	m_WorldUserStateList.clear();
	m_WorldUserStateListByCharDBID.clear();
}

bool CDNWorldUserState::AddUserState(const WCHAR * pName, INT64 biCharacterDBID, int nLocation, int nCummunity, int nChannelID, int nMapIdx)
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	
	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
		return false;

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	sWorldUserState State;
	memset( &State, 0, sizeof(sWorldUserState));
	sWorldUserState* pState = &State;
#else
	sWorldUserState * pState = new sWorldUserState;
	memset(pState, 0, sizeof(sWorldUserState));
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

	_wcscpy(pState->wszCharacterName, _countof(pState->wszCharacterName), pName, (int)wcslen(pName));
	pState->biCharacterDBID = biCharacterDBID;
	if (nLocation > 0)
		pState->nLocationState = nLocation;
	if (nCummunity > 0)
		pState->nCummunityState = nCummunity;
	if (nChannelID > 0)
		pState->nChannelID = nChannelID;
	if (nMapIdx > 0)
		pState->nMapIdx = nMapIdx;

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	m_WorldUserStateList[pState->wszCharacterName]	= State;
	m_WorldUserStateListByCharDBID[biCharacterDBID]	= State;
#else
	m_WorldUserStateList[pState->wszCharacterName]	= pState;
	m_WorldUserStateListByCharDBID[biCharacterDBID]	= pState;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	
	return true;
}

bool CDNWorldUserState::DelUserState(const WCHAR * pName, INT64 biCharacterDBID)
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	
	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

		TCharDBIDUserState::iterator itor = m_WorldUserStateListByCharDBID.find( pUser->biCharacterDBID );
		if( itor != m_WorldUserStateListByCharDBID.end() )
			m_WorldUserStateListByCharDBID.erase( itor );

		m_WorldUserStateList.erase(ii);

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
		SAFE_DELETE(pUser);
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		return true;
	}
	
	_DANGER_POINT();
	return false;
}

bool CDNWorldUserState::UpdateUserState(const WCHAR * pName, INT64 biChracterDBID, int nLocation, int nCummunity, int nChannelID, int nMapIdx)
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		if (nLocation > 0)
			pUser->nLocationState = nLocation;
		if (nCummunity > 0)
			pUser->nCummunityState = nCummunity;
		if (nChannelID > 0)
			pUser->nChannelID = nChannelID;
		if (nMapIdx > 0)
			pUser->nMapIdx = nMapIdx;
		
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		TCharDBIDUserState::iterator itor = m_WorldUserStateListByCharDBID.find( biChracterDBID );
		if( itor != m_WorldUserStateListByCharDBID.end() )
		{
			if (nLocation > 0)
				(*itor).second.nLocationState = nLocation;
			if (nCummunity > 0)
				(*itor).second.nCummunityState = nCummunity;
			if (nChannelID > 0)
				(*itor).second.nChannelID = nChannelID;
			if (nMapIdx > 0)
				(*itor).second.nMapIdx = nMapIdx;
		}

#endif / #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		return true;
	}
	return false;
}

bool CDNWorldUserState::GetUserState(const WCHAR * pName, INT64 biChracterDBID, sWorldUserState *pWorldUserState)
{
	DN_ASSERT(NULL != pName,			"Invalid!");
	DN_ASSERT(NULL != pWorldUserState,	"Invalid!");

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharNameUserState::iterator ii = m_pGetOnlyCharNameUserState->find(pName);
	if (ii != m_pGetOnlyCharNameUserState->end())
#else
	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		if (pUser->biCharacterDBID != biChracterDBID)
			return false;
		pWorldUserState->nChannelID = pUser->nChannelID;
		pWorldUserState->nMapIdx = pUser->nMapIdx;
		pWorldUserState->nLocationState = pUser->nLocationState;
		pWorldUserState->nCummunityState = pUser->nCummunityState;
		return true;
	}

	return false;
}

bool CDNWorldUserState::GetUserState( INT64 biCharacterDBID, sWorldUserState *pWorldUserState )
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharDBIDUserState::iterator ii = m_pGetOnlyCharDBIDUserState->find(biCharacterDBID);
	if (ii != m_pGetOnlyCharDBIDUserState->end())
#else
	TCharDBIDUserState::iterator ii = m_WorldUserStateListByCharDBID.find(biCharacterDBID);
	if (ii != m_WorldUserStateListByCharDBID.end())
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		pWorldUserState->nChannelID = pUser->nChannelID;
		pWorldUserState->nMapIdx = pUser->nMapIdx;
		pWorldUserState->nLocationState = pUser->nLocationState;
		pWorldUserState->nCummunityState = pUser->nCummunityState;
		return true;
	}

	return false;
}

bool CDNWorldUserState::GetUserState( INT64 biCharacterDBID, TCommunityLocation& Location )
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharDBIDUserState::iterator ii = m_pGetOnlyCharDBIDUserState->find(biCharacterDBID);
	if (ii != m_pGetOnlyCharDBIDUserState->end())
#else
	TCharDBIDUserState::iterator ii = m_WorldUserStateListByCharDBID.find(biCharacterDBID);
	if (ii != m_WorldUserStateListByCharDBID.end())
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		Location.cServerLocation	= pUser->nLocationState;
		Location.nChannelID			= pUser->nChannelID;
		Location.nMapIdx			= pUser->nMapIdx;
		return true;
	}

	return false;
}

int CDNWorldUserState::GetUserLocation(const WCHAR * pName, INT64 biChracterDBID)
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharNameUserState::iterator ii = m_pGetOnlyCharNameUserState->find(pName);
	if (ii != m_pGetOnlyCharNameUserState->end())
#else
	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		return pUser->nLocationState;
	}

	return _LOCATION_NONE;
}

int CDNWorldUserState::GetUserCummunityState(const WCHAR * pName, INT64 biChracterDBID)
{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
#else
	ScopeLock<CSyncLock> Lock( m_Sync );
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )

#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	TCharNameUserState::iterator ii = m_pGetOnlyCharNameUserState->find(pName);
	if (ii != m_pGetOnlyCharNameUserState->end())
#else
	TCharNameUserState::iterator ii = m_WorldUserStateList.find(pName);
	if (ii != m_WorldUserStateList.end())
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
	{
#if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		sWorldUserState * pUser = &(*ii).second;
#else
		sWorldUserState * pUser = (*ii).second;
#endif // #if defined( PRE_WORLDUSERSTATE_OPTIMIZE )
		return pUser->nCummunityState;
	}

	return _COMMUNITY_NONE;
}