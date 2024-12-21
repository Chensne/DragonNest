
#include "Stdafx.h"
#include "MasterSystemCacheRepository.h"
#include "DNDBConnection.h"
#include "DNUserSession.h"
#include "DNMasterConnection.h"
#include "DNWorldUserState.h"

using namespace MasterSystem;

void CCacheRepository::ClearCache()
{
	DWORD dwCurTime = timeGetTime();
	if( dwCurTime-m_dwCacheTick < CACHECLEARTICK )
		return;

	ScopeLock<CSyncLock> Lock( m_Sync );

	m_dwCacheTick = dwCurTime;

	switch( m_uiCacheClearCount%5 )
	{
		case 0:	// [0]MasterList
		{
			for( MMasterListItor itor=m_mMasterList.begin() ; itor!=m_mMasterList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mMasterList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
		case 1:	// [1]PupilList
		{
			for( MPupilListItor itor=m_mPupilList.begin() ; itor!=m_mPupilList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mPupilList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
		case 2:	// [2]Master&Classmate List
		{
			for( MMasterClassmateListItor itor=m_mMasterClassmateList.begin() ; itor!=m_mMasterClassmateList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mMasterClassmateList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
		case 3:	// [3]ClassmateInfo
		{
			for( MClassmateInfoListItor itor=m_mClassmateList.begin() ; itor!=m_mClassmateList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mClassmateList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
		case 4:	// [4]MyMasterInfo
		{
			for( MMyMasterInfoListItor itor=m_mMyMasterList.begin() ; itor!=m_mMyMasterList.end() ; )
			{
				if( dwCurTime-(*itor).second.first > CACHECLEARTICK )
					m_mMyMasterList.erase( itor++ );
				else
					++itor;
			}
			break;
		}
	}

	++m_uiCacheClearCount;
}

void CCacheRepository::GetMasterList( CDNUserSession* pSession, UINT uiPage, BYTE cJob, BYTE cGender )
{
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMasterListItor itor = m_mMasterList.find( std::tr1::make_tuple(uiPage,cJob,cGender) );
		if( itor != m_mMasterList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMasterSystemMasterList( ERROR_NONE, (*itor).second.second );
				return;
			}
		}
	}

	// QUERY
	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( !pDBCon || !pDBCon->GetActive() )
	{
		pSession->SendMasterSystemMasterList( ERROR_GENERIC_DBCON_NOT_FOUND, std::vector<TMasterInfo>() );
		return;
	}

	pDBCon->QueryGetPageMasterCharacter( pSession, uiPage, cJob, cGender );
}

void CCacheRepository::SetMasterList( CDNUserSession* pSession, TAGetPageMasterCharacter* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMasterListItor itor = m_mMasterList.find( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cGenderCode) );
		if( itor != m_mMasterList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->MasterInfoList[i] );
		}
		else
		{
			std::vector<TMasterInfo> vMasterList;
			if( pData->cCount > 0 )
			{
				vMasterList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vMasterList.push_back( pData->MasterInfoList[i] );
			}

			m_mMasterList.insert( std::make_pair( std::tr1::make_tuple(pData->uiPage,pData->cJobCode,pData->cGenderCode),std::make_pair(timeGetTime(),vMasterList) ) );
		}

		GetMasterList( pSession, pData->uiPage, pData->cJobCode, pData->cGenderCode );
	}
	else
	{
		if( pSession )
			pSession->SendMasterSystemMasterList( pData->nRetCode, std::vector<TMasterInfo>() );
	}
}

void CCacheRepository::GetMasterCharacter( CDNUserSession* pSession, INT64 biCharacterDBID )
{
	// QUERY
	if( pSession == NULL )
		return;

	CDNDBConnection* pDBCon = pSession->GetDBConnection();
	if( !pDBCon || !pDBCon->GetActive() )
	{
		pSession->SendMasterSystemMasterCharacterInfo( ERROR_GENERIC_DBCON_NOT_FOUND );
		return;
	}

	pDBCon->QueryGetMasterCharacterType1( pSession, pSession->GetCharacterDBID() );
}

void CCacheRepository::SetMasterCharacter( CDNUserSession* pSession, TAGetMasterCharacterType1* pData )
{
	if( pSession )
		pSession->SendMasterSystemMasterCharacterInfo( pData->nRetCode, &pData->MasterCharacterInfo );

	if( pData->nRetCode == ERROR_NONE )
		GetPupilList( pSession, pData->biMasterCharacterDBID, true );
}

void CCacheRepository::GetMyMasterInfo( CDNUserSession* pSession, INT64 biPupilCharacterDBID, INT64 biMasterCharacterDBID, bool bRefresh )
{
	if( bRefresh == false )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMyMasterInfoListItor itor = m_mMyMasterList.find( std::make_pair(biPupilCharacterDBID,biMasterCharacterDBID) );
		if( itor != m_mMyMasterList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMasterSystemMyMasterInfo( ERROR_NONE, &(*itor).second.second );
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
		pSession->SendMasterSystemMyMasterInfo( ERROR_GENERIC_DBCON_NOT_FOUND );
		return;
	}

	pDBCon->QueryGetMasterCharacterType2( pSession, biMasterCharacterDBID, biPupilCharacterDBID );
}

void CCacheRepository::SetMyMasterInto( CDNUserSession* pSession, TAGetMasterCharacterType2* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMyMasterInfoListItor itor = m_mMyMasterList.find( std::make_pair(pData->biPupilCharacterDBID,pData->biMasterCharacterDBID) );
		if( itor != m_mMyMasterList.end() )
		{
			(*itor).second.first	= timeGetTime();
			(*itor).second.second	= pData->MasterInfo;
		}
		else
		{
			m_mMyMasterList.insert( std::make_pair( std::make_pair(pData->biPupilCharacterDBID,pData->biMasterCharacterDBID),std::make_pair(timeGetTime(),pData->MasterInfo) ) );
		}

		GetMyMasterInfo( pSession, pData->biPupilCharacterDBID, pData->biMasterCharacterDBID, false );
	}
	else
	{
		if( pSession )
			pSession->SendMasterSystemMyMasterInfo( pData->nRetCode );
	}
}

const TPupilInfo* CCacheRepository::GetPupilInfo( INT64 biMasterCharacterDBID, INT64 biPupilCharacterDBID )
{
	ScopeLock<CSyncLock> Lock( m_Sync );

	MPupilListItor itor = m_mPupilList.find( biMasterCharacterDBID );
	if( itor == m_mPupilList.end()  )
		return NULL;

	for( UINT i=0 ; i<(*itor).second.second.size() ; ++i )
	{
		if( (*itor).second.second[i].biCharacterDBID == biPupilCharacterDBID )
			return &(*itor).second.second[i];
	}

	return NULL;
}

void CCacheRepository::GetPupilList( CDNUserSession* pSession, INT64 biCharacterDBID, bool bRefresh )
{
	if( bRefresh == false )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MPupilListItor itor = m_mPupilList.find( biCharacterDBID );
		if( itor != m_mPupilList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMasterSystemPupilList( ERROR_NONE, biCharacterDBID, (*itor).second.second );
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
		pSession->SendMasterSystemMasterCharacterInfo( ERROR_GENERIC_DBCON_NOT_FOUND );
		return;
	}

	pDBCon->QueryGetPupilList( pSession, biCharacterDBID );
}

void CCacheRepository::SetPupilList( CDNUserSession* pSession, TAGetListPupil* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MPupilListItor itor = m_mPupilList.find( pData->biCharacterDBID );
		if( itor != m_mPupilList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->PupilInfoList[i] );
		}
		else
		{
			std::vector<TPupilInfo> vPupilList;
			if( pData->cCount > 0 )
			{
				vPupilList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vPupilList.push_back( pData->PupilInfoList[i] );
			}

			m_mPupilList.insert( std::make_pair( pData->biCharacterDBID,std::make_pair(timeGetTime(),vPupilList) ) );
		}

		GetPupilList( pSession, pData->biCharacterDBID, false );
	}
	else
	{
		if( pSession )
			pSession->SendMasterSystemPupilList( pData->nRetCode, pData->biCharacterDBID, std::vector<TPupilInfo>() );
	}
}

void CCacheRepository::GetMasterClassmateList( CDNUserSession* pSession, INT64 biCharacterDBID, bool bRefresh )
{
	if( bRefresh == false )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMasterClassmateListItor itor = m_mMasterClassmateList.find( biCharacterDBID );
		if( itor != m_mMasterClassmateList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMasterSystemMasterAndClassmateInfo( ERROR_NONE, (*itor).second.second );
				return;
			}
		}
	}

	// QUERY
	if( pSession == NULL )
		return;

	if( pSession->CheckDBConnection() == false )
	{
		pSession->SendMasterSystemMasterAndClassmateInfo( ERROR_GENERIC_DBCON_NOT_FOUND, std::vector<TMasterAndClassmateInfo>() );
		return;
	}

	pSession->GetDBConnection()->QueryGetMasterAndClassmate( pSession, biCharacterDBID );
}

void CCacheRepository::SetMasterClassmateList( CDNUserSession* pSession, TAGetListMyMasterAndClassmate* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MMasterClassmateListItor itor = m_mMasterClassmateList.find( pData->biCharacterDBID );
		if( itor != m_mMasterClassmateList.end() )
		{
			(*itor).second.first = timeGetTime();
			(*itor).second.second.clear();
			for( UINT i=0 ; i<pData->cCount ; ++i )
				(*itor).second.second.push_back( pData->MasterAndClassmateInfoList[i] );
		}
		else
		{
			std::vector<TMasterAndClassmateInfo> vList;
			if( pData->cCount > 0 )
			{
				vList.reserve( pData->cCount );
				for( UINT i=0 ; i<pData->cCount ; ++i )
					vList.push_back( pData->MasterAndClassmateInfoList[i] );
			}

			m_mMasterClassmateList.insert( std::make_pair( pData->biCharacterDBID,std::make_pair(timeGetTime(),vList) ) );
		}

		GetMasterClassmateList( pSession, pData->biCharacterDBID, false );
	}
	else
	{
		if( pSession )
			pSession->SendMasterSystemMasterAndClassmateInfo( pData->nRetCode, std::vector<TMasterAndClassmateInfo>() );
	}
}

void CCacheRepository::GetClassmateInfo( CDNUserSession* pSession, INT64 biClassmateCharacterDBID, bool bRefresh )
{
	if( bRefresh == false )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MClassmateInfoListItor itor = m_mClassmateList.find( biClassmateCharacterDBID );
		if( itor != m_mClassmateList.end()  )
		{
			if( timeGetTime()-(*itor).second.first < CACHETICK )
			{
				if( pSession )
					pSession->SendMasterSystemClassmateInfo( ERROR_NONE, &(*itor).second.second );
				return;
			}
		}
	}

	// QUERY
	if( pSession == NULL )
		return;

	if( pSession->CheckDBConnection() == false )
	{
		pSession->SendMasterSystemClassmateInfo( ERROR_GENERIC_DBCON_NOT_FOUND );
		return;
	}

	pSession->GetDBConnection()->QueryGetClassmateInfo( pSession, biClassmateCharacterDBID );
}

void CCacheRepository::SetClassmateInfo( CDNUserSession* pSession, TAGetMyClassmate* pData )
{
	ClearCache();

	if( pData->nRetCode == ERROR_NONE )
	{
		ScopeLock<CSyncLock> Lock( m_Sync );

		MClassmateInfoListItor itor = m_mClassmateList.find( pData->ClassmateInfo.biCharacterDBID );
		if( itor != m_mClassmateList.end() )
		{
			(*itor).second.first	= timeGetTime();
			(*itor).second.second	= pData->ClassmateInfo;
		}
		else
		{
			m_mClassmateList.insert( std::make_pair( pData->ClassmateInfo.biCharacterDBID,std::make_pair(timeGetTime(),pData->ClassmateInfo) ) );
		}

		GetClassmateInfo( pSession, pData->ClassmateInfo.biCharacterDBID, false );
	}
	else
	{
		if( pSession )
			pSession->SendMasterSystemClassmateInfo( pData->nRetCode );
	}
}

TCommunityLocation CCacheRepository::GetLocationInfo( INT64 biCharacterDBID )
{
	TCommunityLocation Location;
	g_pWorldUserState->GetUserState( biCharacterDBID, Location );

	return Location;
}

void CCacheRepository::SetRegisterMaster( CDNUserSession* pSession, TAAddMasterCharacter* pData )
{
	if( pSession )
		pSession->SendMasterSystemIntroductionOnOff( pData->nRetCode, true, pData->wszSelfIntroduction );
}

void CCacheRepository::SetRegisterMaster( CDNUserSession* pSession, TADelMasterCharacter* pData )
{
	if( pSession )
		pSession->SendMasterSystemIntroductionOnOff( pData->nRetCode, false, NULL );
}

