
#include "Stdafx.h"
#include "DNGuildRecruitCacheRepository.h"
#include "DNDBConnection.h"
#include "DNUserSession.h"
#include "DNMasterConnection.h"


using namespace GuildRecruitSystem;

void CCacheRepository::ClearCache()
{
	DWORD dwCurTime = timeGetTime();
	if( dwCurTime-m_dwCacheTick < CACHECLEARTICK )
		return;

	ScopeLock<CSyncLock> Lock( m_Sync );

	m_dwCacheTick = dwCurTime;

	switch( m_uiCacheClearCount%5 )
	{
	case 0:	// [0]MGuildInfoList
		{
			for( MMGuildRecruitListItor itor=m_mGuildRecruitList.begin() ; itor!=m_mGuildRecruitList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mGuildRecruitList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
	case 1:	// [1]MGuildMemberList
		{
			for( MGuildRecruitCharacterListItor itor=m_mGuildRecruitCharacterList.begin() ; itor!=m_mGuildRecruitCharacterList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mGuildRecruitCharacterList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
	case 2:	// [2]MRequestGuildList
		{
			for( MMyGuildRecruitListItor itor=m_mMyGuildRecruitList.begin() ; itor!=m_mMyGuildRecruitList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mMyGuildRecruitList.erase( itor++ );
				else
					++itor;
			}
			break;
		}	
	}

	++m_uiCacheClearCount;
}

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
void CCacheRepository::GetGuildRecruitList( CDNUserSession* pSession, UINT uiPage, BYTE cJob, BYTE cLevel, BYTE cPurposeCode, WCHAR* wszGuildName, BYTE cSortType )
#else
void CCacheRepository::GetGuildRecruitList( CDNUserSession* pSession, UINT uiPage, BYTE cJob, BYTE cLevel )
#endif
{
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		WCHAR GuildName[GUILDNAME_MAX] = {0,};
		_wcscpy(GuildName, _countof(GuildName), wszGuildName, (int)wcslen(wszGuildName));				
		MMGuildRecruitListItor itor = m_mGuildRecruitList.find( std::tr1::make_tuple(uiPage,cJob,cLevel,cPurposeCode, cSortType, GuildName[0]) );
#else
		MMGuildRecruitListItor itor = m_mGuildRecruitList.find( std::tr1::make_tuple(uiPage,cJob,cLevel) );
#endif
		if( itor != m_mGuildRecruitList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendGuildRecruitList( ERROR_NONE, uiPage, (*itor).second.second );
				return;
			}
		}
	}
	
	// QUERY
	if( pSession == NULL )
		return;

	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( !pDBCon || !pDBCon->GetActive() )
	{
		pSession->SendGuildRecruitList( ERROR_GENERIC_DBCON_NOT_FOUND, 0, std::vector<TGuildRecruitInfo>() );
		return;
	}

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	pDBCon->QueryGetPageGuildRecruit( pSession, uiPage, cJob, cLevel, cPurposeCode, wszGuildName, cSortType );
#else
	pDBCon->QueryGetPageGuildRecruit( pSession, uiPage, cJob, cLevel, 0, NULL, 0 );
#endif
}

void CCacheRepository::SetGuildRecruitList( CDNUserSession* pSession, TAGetGuildRecruit* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		WCHAR GuildName[GUILDNAME_MAX] = {0,};
		_wcscpy(GuildName, _countof(GuildName), pData->wszGuildName, (int)wcslen(pData->wszGuildName));		
		MMGuildRecruitListItor itor = m_mGuildRecruitList.find( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cLevel,pData->cPurposeCode,pData->cSortType, GuildName[0]) );
#else
		MMGuildRecruitListItor itor = m_mGuildRecruitList.find( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cLevel) );
#endif
		if( itor != m_mGuildRecruitList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->GuildRecruitList[i] );
		}
		else
		{
			std::vector<TGuildRecruitInfo> vGuildInfoList;
			if( pData->cCount > 0 )
			{
				vGuildInfoList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vGuildInfoList.push_back( pData->GuildRecruitList[i] );
			}

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			m_mGuildRecruitList.insert( std::make_pair( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cLevel,pData->cPurposeCode,pData->cSortType,pData->wszGuildName[0]),std::make_pair(timeGetTime(),vGuildInfoList) ) );
#else
			m_mGuildRecruitList.insert( std::make_pair( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cLevel),std::make_pair(timeGetTime(),vGuildInfoList) ) );
#endif
		}

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		GetGuildRecruitList( pSession, pData->uiPage, pData->cJobCode, pData->cLevel, pData->cPurposeCode, pData->wszGuildName, pData->cSortType );
#else
		GetGuildRecruitList( pSession, pData->uiPage, pData->cJobCode, pData->cLevel );
#endif
	}
	else
	{
		if( pSession )
			pSession->SendGuildRecruitList( pData->nRetCode, 0, std::vector<TGuildRecruitInfo>() );
	}
}

void CCacheRepository::GetGuildRecruitCharacter( CDNUserSession* pSession, TGuildUID GuildUID )
{
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		
		MGuildRecruitCharacterListItor itor = m_mGuildRecruitCharacterList.find( GuildUID );
		
		if( itor != m_mGuildRecruitCharacterList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendGuildRecruitCharacter( ERROR_NONE, (*itor).second.second );
				return;
			}
		}
	}

	// QUERY
	if( pSession == NULL )
		return;

	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( !pDBCon || !pDBCon->GetActive() )
	{
		pSession->SendGuildRecruitCharacter( ERROR_GENERIC_DBCON_NOT_FOUND, std::vector<TGuildRecruitCharacter>() );
		return;
	}

	pDBCon->QueryGetGuildRecruitCharacter( pSession, GuildUID.nDBID );
}

void CCacheRepository::SetGuildRecruitCharacter( CDNUserSession* pSession, TAGetGuildRecruitCharacter* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		TGuildUID GuildUID(pData->cWorldSetID, pData->nGuildDBID);

		MGuildRecruitCharacterListItor itor = m_mGuildRecruitCharacterList.find( GuildUID ) ;
		if( itor != m_mGuildRecruitCharacterList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->GuildRecruitCharacterList[i] );
		}
		else
		{
			std::vector<TGuildRecruitCharacter> vGuildCharacterList;
			if( pData->cCount > 0 )
			{
				vGuildCharacterList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vGuildCharacterList.push_back( pData->GuildRecruitCharacterList[i] );
			}
			m_mGuildRecruitCharacterList.insert( std::make_pair(GuildUID, std::make_pair(timeGetTime(),vGuildCharacterList)) );
		}

		GetGuildRecruitCharacter( pSession, GuildUID );
	}
	else
	{
		if( pSession )
			pSession->SendGuildRecruitCharacter( pData->nRetCode, std::vector<TGuildRecruitCharacter>() );
	}
}

void CCacheRepository::DelGuildRecruitCharacter( TGuildUID GuildUID )
{
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MGuildRecruitCharacterListItor itor = m_mGuildRecruitCharacterList.find( GuildUID ) ;
		if( itor != m_mGuildRecruitCharacterList.end()  )
		{
			m_mGuildRecruitCharacterList.erase(itor);
		}
	}	
}

void CCacheRepository::GetMyGuildRecruit( CDNUserSession* pSession )
{	
	{
		ScopeLock<CSyncLock> Lock( m_Sync );
		
		if( pSession == NULL )
			return;

		MMyGuildRecruitListItor itor = m_mMyGuildRecruitList.find( pSession->GetCharacterDBID() );
		if( itor != m_mMyGuildRecruitList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMyGuildRecruit( ERROR_NONE, (*itor).second.second );
				return;
			}
		}
	}

	// QUERY
	if( pSession == NULL )
		return;

	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( !pDBCon || !pDBCon->GetActive() )
	{
		pSession->SendMyGuildRecruit( ERROR_GENERIC_DBCON_NOT_FOUND, std::vector<TGuildRecruitInfo>() );
		return;
	}

	pDBCon->QueryGetMyGuildRecruit( pSession );
}

void CCacheRepository::SetMyGuildRecruit( CDNUserSession* pSession, TAGetMyGuildRecruit* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		if( pSession == NULL )
			return;

		MMyGuildRecruitListItor itor = m_mMyGuildRecruitList.find( pSession->GetCharacterDBID() );
		if( itor != m_mMyGuildRecruitList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->GuildRecruitList[i] );
		}
		else
		{
			std::vector<TGuildRecruitInfo> vGuildRecruitList;
			if( pData->cCount > 0 )
			{
				vGuildRecruitList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vGuildRecruitList.push_back( pData->GuildRecruitList[i] );
			}
			m_mMyGuildRecruitList.insert( std::make_pair(pSession->GetCharacterDBID(), std::make_pair(timeGetTime(),vGuildRecruitList)) );
		}

		GetMyGuildRecruit( pSession);
	}
	else
	{
		if( pSession )
			pSession->SendMyGuildRecruit( pData->nRetCode, std::vector<TGuildRecruitInfo>() );
	}
}
void CCacheRepository::DelMyGuildRecruit( CDNUserSession* pSession )
{	
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		if( pSession == NULL )
			return;

		MMyGuildRecruitListItor itor = m_mMyGuildRecruitList.find( pSession->GetCharacterDBID() );
		if( itor != m_mMyGuildRecruitList.end()  )
		{
			m_mMyGuildRecruitList.erase(itor);
		}
	}	
}