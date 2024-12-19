#include "StdAfx.h"
#include "EtActionCoreMng.h"
#include "EtActionSignal.h"
#include "EtResourceMng.h"
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
#include <MMSystem.h>
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

#pragma warning(disable:4996)
CEtActionCoreMng g_ActionCoreMng;


CEtActionCoreMng::CEtActionCoreMng()
{
	m_bUseDynamic = true;
#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	m_bProcessOptimize = false;
	m_fOptimizeCheckTime = 0.0f;
#endif
}

CEtActionCoreMng::~CEtActionCoreMng()
{
	Finalize();
}

bool CEtActionCoreMng::Initialize( bool bUseDynamic )
{
	m_bUseDynamic = bUseDynamic;
	return true;
}

void CEtActionCoreMng::Finalize()
{
	ScopeLock<CSyncLock> Lock(m_SyncLock);

	while( !m_szMapContainerList.empty() ) {
		FlushWaitDelete();
		std::map<std::string, ActionContainerStruct*>::iterator it;
		while( !m_szMapContainerList.empty() ) {
			it = m_szMapContainerList.begin();
			RemoveAction( it->second->szFileName.c_str(), NULL );
		}
		FlushWaitDelete();
	}
}

bool CEtActionCoreMng::IsActionObject( const char *szFullPathName )
{
	char szPath[512] = { 0, };
	char szFileName[256] = { 0, };
	char szExt[16] = { 0, };
	char szBuffer[512] = { 0, };

	_GetPath( szPath, _countof(szPath), szFullPathName );
	_GetFileName( szFileName, _countof(szFileName), szFullPathName );
	_GetExt( szExt, _countof(szExt), szFullPathName );

	//sprintf_s( szBuffer, "%s%s.act", szPath, szFileName, szExt );
	sprintf_s( szBuffer, "%s%s.act", szPath, szFileName);	// code analysis

	CResMngStream Stream( szBuffer );
	if( !Stream.IsValid() ) return false;
	return true;
}

bool Compare_Order( CEtActionSignal *a, CEtActionSignal *b )
{
	if( a->GetOrder() < b->GetOrder() ) return true;
	else if( a->GetOrder() > b->GetOrder() ) return false;
	if( a->GetStartFrame() < b->GetStartFrame() ) return true;
	return false;
}

std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadAction( const char *szFileName, CEtActionBase *pAction )
{
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;

	ActionHeader Header;
	memset( &Header, 0, sizeof(Header) );
	CResMngStream Stream( szFileName );

	if( !Stream.IsValid() ) return NULL;

	bool bMulti = false;
	if( Stream.Size() < sizeof(ActionHeader) )
	{
		bMulti = true;
	}
	else
	{
		Stream.Read( &Header, sizeof(Header) );
		if( strcmp( Header.szHeaderString, "Eternity Action File" ) != NULL ) {
			if( strstr( Header.szHeaderString, ".act" ) ) {
				bMulti = true;
			}
		}
	}
	Stream.Seek( 0, SEEK_SET );

	if( bMulti ) return LoadActionMulti( &Stream, pAction );

	return LoadActionBasic( &Stream, pAction );
}

std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::GetAction( const char *szFileName, CEtActionBase *pAction )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	std::map<std::string, ActionContainerStruct*>::iterator it = m_szMapContainerList.find( szFileName );
	if( it != m_szMapContainerList.end() ) {
		it->second->nRefCount++;
		pAction->SetSearchMap( &it->second->MapSearch );
		return &it->second->pVecActionElementList;
	}
	if( m_bUseDynamic ) {
		for( DWORD i=0; i<m_pVecWaitDeleteList.size(); i++ ) {
			ActionContainerStruct *pStruct = m_pVecWaitDeleteList[i];
			if( _stricmp( pStruct->szFileName.c_str(), szFileName ) == NULL ) {
				m_pVecWaitDeleteList.erase( m_pVecWaitDeleteList.begin() + i );
				pStruct->nRefCount++;
				m_szMapContainerList.insert( make_pair( szFileName, pStruct ) );
				pAction->SetSearchMap( &pStruct->MapSearch );

				return &pStruct->pVecActionElementList;
			}
		}
	}
	return NULL;
}

std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionMulti( CStream *pStream, CEtActionBase *pAction )
{
	char *szFileName = (char *)pStream->GetName();
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;

	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);

	int nSize = pStream->Size();
	char *pBuffer = new char[ nSize + 1 ];

	memset( pBuffer, 0, nSize + 1 );
	pStream->Read( pBuffer, nSize );
	std::vector<char *> pVecStreamList;
	std::vector<int> nVecSizeList;

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	std::vector<std::string> szVecFileNameList;
	std::vector<DWORD> nVecFileOffset;
	std::vector<DWORD> nActionVersion;
#endif

	int nTotalSize = sizeof(ActionHeader);

	char *pToken = strtok( pBuffer, "\n\r" );
	ActionHeader Header;
	int nActionCount = 0;
	char *pTemp;
	while( pToken ) {
		CResMngStream *pStream = new CResMngStream( pToken );
		if( !pStream->IsValid() ) {
			SAFE_DELETE( pStream );
			pToken = strtok( NULL, "\n\r" );
			continue;
		}
		//RLKT ADDON !?!?!
		pStream->Read( &Header, sizeof(Header) );
		if( strcmp( Header.szHeaderString, "Eternity Action File" ) != NULL ) {
			SAFE_DELETE( pStream );
			pToken = strtok( NULL, "\n\r" );
			continue;
		}
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		szVecFileNameList.push_back( pToken );
		ToLowerA( szVecFileNameList[szVecFileNameList.size()-1] );
		nVecFileOffset.push_back( nActionCount );
		nActionVersion.push_back( Header.nVersion );
#endif
		nActionCount += Header.nActionCount;

		int nCurSize = pStream->Size() - sizeof(ActionHeader);
		nTotalSize += nCurSize;

		pTemp = new char[nCurSize];
		pStream->Read( pTemp, nCurSize );

		pVecStreamList.push_back( pTemp );
		nVecSizeList.push_back( nCurSize );

		SAFE_DELETE( pStream );
		pToken = strtok( NULL, "\n\r" );
	}
	SAFE_DELETEA( pBuffer );

	pBuffer = new char[nTotalSize];
	CMemoryStream MergeStream( pBuffer, nTotalSize );


	Header.nActionCount = nActionCount;
	MergeStream.Write( &Header, sizeof(ActionHeader) );

	for( DWORD i=0; i<pVecStreamList.size(); i++ ) {
		MergeStream.Write( pVecStreamList[i], nVecSizeList[i] );
	}
	SAFE_DELETE_AVEC( pVecStreamList );

	MergeStream.Seek( 0, SEEK_SET );
	MergeStream.SetName( szFileName );
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	std::vector<CEtActionBase::ActionElementStruct *> *pResult = LoadActionBasicMultiVersion( &MergeStream, pAction, &szVecFileNameList, &nVecFileOffset, &nActionVersion );
#else
	std::vector<CEtActionBase::ActionElementStruct *> *pResult = LoadActionBasic( &MergeStream, pAction );
#endif
	SAFE_DELETEA( pBuffer );

	return pResult;
}

std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionMulti( const char *szFileName, CEtActionBase *pAction )
{
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;

	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() ) return NULL;
	return LoadActionMulti( &Stream, pAction );
}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionBasic( CStream *pStream, CEtActionBase *pAction, std::vector<std::string> *szVecNameList, std::vector<DWORD> *nVecOffsetList )
#else
std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionBasic( CStream *pStream, CEtActionBase *pAction )
#endif
{
	char *szFileName = (char *)pStream->GetName();
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);


	ActionHeader Header;
	memset( &Header, 0, sizeof(Header) );

	if( !pStream->IsValid() ) return NULL;

	pStream->Read( &Header, sizeof(Header) );
	// 헤더 체크
	if( strcmp( Header.szHeaderString, "Eternity Action File" ) != NULL 
		|| Header.nVersion > 5 //act 5
		|| Header.nVersion < 1) { //act 1
		return NULL;
	}
	
	char szFileNameTemp[256];
	_GetFullFileName( szFileNameTemp, _countof(szFileNameTemp), szFileName );
	_strlwr( szFileNameTemp );


	// 로딩
	int nSignalCount;
	ActionContainerStruct *pContainer = new ActionContainerStruct;
	CEtActionBase::ActionElementStruct *pStruct;
	pContainer->nRefCount = 1;
	pContainer->pCheckPreSignalFunc = pAction->m_pCheckPreSignalFunc;
	pContainer->pCheckPostSignalFunc = pAction->m_pCheckPostSignalFunc;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	pContainer->pCheckInitSignalFunc = pAction->m_pCheckInitSignalFunc;
#endif
	pContainer->szFileName = szFileName;
	pContainer->szVecUsingElementList = pAction->GetUsingElementList();
	pAction->GetUsingElementList().clear();

	for( int i=0; i<Header.nActionCount; i++ ) {
		pStruct = new CEtActionBase::ActionElementStruct;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( szVecNameList && nVecOffsetList ) {
			std::string szTempName;
			for( int j=(int)nVecOffsetList->size()-1; j>=0; j-- ) {
				if( i >= (int)(*nVecOffsetList)[j] ) {
					pStruct->szParentFileName = (*szVecNameList)[j];
					
					break;
				}
			}
		}
		else {
			pStruct->szParentFileName = szFileNameTemp;
		}
#endif

		ReadStdString( pStruct->szName, pStream );

		ReadStdString( pStruct->szLinkAniName, pStream );
		pStream->Read( &pStruct->dwLength, sizeof(DWORD) );

	 	ReadStdString( pStruct->szNextActionName, pStream );

		// #33608 액션툴에서 next 액션 문자열이 none 이라면 비워준다.
		if( stricmp( pStruct->szNextActionName.c_str(), "none" ) == 0 )
			pStruct->szNextActionName.clear();

		pStream->Read( &pStruct->dwBlendFrame, sizeof(DWORD) );
		pStream->Read( &pStruct->dwNextActionFrame, sizeof(DWORD) );
		
#ifdef _ADD_ACT2
		if (Header.nVersion >= 2)
		{
			pStream->Read( &pStruct->dwUnkAct2, sizeof(DWORD) );
		}
#endif

#ifdef _ADD_ACT3
		if (Header.nVersion >= 3)
		{
			pStream->Read( &pStruct->dwUnkAct3, sizeof(DWORD) );
		}
#endif

		//rlkt_act4
		if (Header.nVersion >= 4)
		{
			pStream->Read(&pStruct->bUnkAct4, sizeof(bool));
		}

		//rlkt_act5
		if (Header.nVersion >= 5)
		{
			pStream->Read(&pStruct->dwUnkAct5, sizeof(DWORD));
		}

		pStream->Read( &nSignalCount, sizeof(int) );

		for( int j=0; j<nSignalCount; j++ ) 
		{
			CEtActionSignal *pSignal = new CEtActionSignal;
			pSignal->SetSignalListArrayIndex( j );

			if( pSignal->LoadSignal( pStream ) == false ) {
				SAFE_DELETE( pSignal );
				continue;
			}
			if( pAction->IsIgnoreSignal( pSignal->GetSignalIndex() ) == true ) {
				SAFE_DELETE( pSignal );
				continue;
			}

			pStruct->pVecSignalList.push_back( pSignal );
		}
		std::sort( pStruct->pVecSignalList.begin(), pStruct->pVecSignalList.end(), Compare_Order );

		// Search 용 Map 매칭 시켜준다.
		////////////////////////////////////////////////////////////////////////////////
		std::map<int, std::vector<CEtActionSignal *>>::iterator itInner;
		for( DWORD j=0; j<pStruct->pVecSignalList.size(); j++ ) {
			itInner = pStruct->MapSearch.find( pStruct->pVecSignalList[j]->GetSignalIndex() );
			if( itInner != pStruct->MapSearch.end() ) {
				itInner->second.push_back( pStruct->pVecSignalList[j] );
				std::sort( itInner->second.begin(), itInner->second.end(), Compare_Order );
			}
			else {
				std::vector<CEtActionSignal *> VecList;
				VecList.push_back( pStruct->pVecSignalList[j] );
				pStruct->MapSearch.insert( make_pair( pStruct->pVecSignalList[j]->GetSignalIndex(), VecList ) );
			}
		}
		////////////////////////////////////////////////////////////////////////////////
		for( DWORD j=0; j<pStruct->pVecSignalList.size(); j++ ) {
			int nIndex = pStruct->pVecSignalList[j]->GetSignalListArrayIndex();
			pStruct->MapSearchByArrayIndex.insert( make_pair( nIndex, pStruct->pVecSignalList[j] ) );
		}
		////////////////////////////////////////////////////////////////////////////////

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
		// 최초 로드시 시간 설정
		pStruct->dwLastUsedTime = timeGetTime();
		pStruct->bCheckPostSignal = false;
#endif

		pContainer->pVecActionElementList.push_back( pStruct );
	}
	// Map Search 
	for( DWORD i=0; i<pContainer->pVecActionElementList.size(); i++ ) {
		pContainer->MapSearch.insert( make_pair( pContainer->pVecActionElementList[i]->szName.c_str(), (int)i ) );
	}

	pAction->SetSearchMap( &pContainer->MapSearch );
	m_szMapContainerList.insert( make_pair( szFileName, pContainer ) );

	// 새로 읽혀진 파일일 경우 CheckPreSignal 호출해서 미리 읽을것들을 읽어준다.
	for( DWORD i=0; i<pContainer->pVecActionElementList.size(); i++ ) {
		CEtActionBase::ActionElementStruct *pElement = pContainer->pVecActionElementList[i];
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		bool bCallPreSignal = true;
		if( !pContainer->szVecUsingElementList.empty() && std::find( pContainer->szVecUsingElementList.begin(), pContainer->szVecUsingElementList.end(), pElement->szParentFileName ) == pContainer->szVecUsingElementList.end() ) {
			bCallPreSignal = false;
		}
		pElement->bUsing = bCallPreSignal;
#endif

		for( DWORD j=0; j<pElement->pVecSignalList.size(); j++ ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
			pContainer->pCheckInitSignalFunc( pElement, i, pElement->pVecSignalList[j], j, pAction );
			if( bCallPreSignal ) pContainer->pCheckPreSignalFunc( pContainer->pVecActionElementList[i], i, pContainer->pVecActionElementList[i]->pVecSignalList[j], j, pAction );
#else
			pContainer->pCheckPreSignalFunc( pContainer->pVecActionElementList[i], i, pContainer->pVecActionElementList[i]->pVecSignalList[j], j, pAction );
#endif
		}
	}
	return &pContainer->pVecActionElementList;
}


//KAT ! 2016
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionBasicMultiVersion( CStream *pStream, CEtActionBase *pAction, std::vector<std::string> *szVecNameList, std::vector<DWORD> *nVecOffsetList, std::vector<DWORD> *nActionVersion )
#else
std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionBasic( CStream *pStream, CEtActionBase *pAction )
#endif
{
	char *szFileName = (char *)pStream->GetName();
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);


	ActionHeader Header;
	memset( &Header, 0, sizeof(Header) );

	if( !pStream->IsValid() ) return NULL;

	pStream->Read( &Header, sizeof(Header) );
	// 헤더 체크
	if( strcmp( Header.szHeaderString, "Eternity Action File" ) != NULL 
		|| Header.nVersion >  5 //act 5
		|| Header.nVersion < 1) { //act 1
		return NULL;
	}
	
	char szFileNameTemp[256];
	_GetFullFileName( szFileNameTemp, _countof(szFileNameTemp), szFileName );
	_strlwr( szFileNameTemp );

	OutputDebug("[M-NORMAL] Load Act %d , Name: %s AC: %d",Header.nVersion,szFileNameTemp,Header.nActionCount);


	// 로딩

	int nSignalCount;
	ActionContainerStruct *pContainer = new ActionContainerStruct;
	CEtActionBase::ActionElementStruct *pStruct;
	pContainer->nRefCount = 1;
	pContainer->pCheckPreSignalFunc = pAction->m_pCheckPreSignalFunc;
	pContainer->pCheckPostSignalFunc = pAction->m_pCheckPostSignalFunc;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	pContainer->pCheckInitSignalFunc = pAction->m_pCheckInitSignalFunc;
#endif
	pContainer->szFileName = szFileName;
	pContainer->szVecUsingElementList = pAction->GetUsingElementList();
	pAction->GetUsingElementList().clear();

	for( int i=0; i<Header.nActionCount; i++ ) {
		pStruct = new CEtActionBase::ActionElementStruct;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( szVecNameList && nVecOffsetList ) {
			std::string szTempName;
			for( int j=(int)nVecOffsetList->size()-1; j>=0; j-- ) {
				if( i >= (int)(*nVecOffsetList)[j] ) {
					pStruct->szParentFileName = (*szVecNameList)[j];
					
				//	OutputDebug("[MULTI] Load Act Orig: %d New: %d , Name: %s AC: %d ",Header.nVersion,(int)(*nActionVersion)[j],pStruct->szParentFileName.c_str(),Header.nActionCount);
					Header.nVersion = (int)(*nActionVersion)[j];

					break;
				}
			}
		}
		else {
			//	OutputDebug("[ELSE] Load Act %d , Name: %s AC: %d",Header.nVersion,szFileNameTemp,Header.nActionCount);

			pStruct->szParentFileName = szFileNameTemp;
		}
#endif

		ReadStdString( pStruct->szName, pStream );

		ReadStdString( pStruct->szLinkAniName, pStream );
		pStream->Read( &pStruct->dwLength, sizeof(DWORD) );

	 	ReadStdString( pStruct->szNextActionName, pStream );

		// #33608 액션툴에서 next 액션 문자열이 none 이라면 비워준다.
		if( stricmp( pStruct->szNextActionName.c_str(), "none" ) == 0 )
			pStruct->szNextActionName.clear();

		pStream->Read( &pStruct->dwBlendFrame, sizeof(DWORD) );
		pStream->Read( &pStruct->dwNextActionFrame, sizeof(DWORD) );
		
#ifdef _ADD_ACT2
		if (Header.nVersion >= 2)
		{
			pStream->Read( &pStruct->dwUnkAct2, sizeof(DWORD) );
		}
#endif

#ifdef _ADD_ACT3
		if (Header.nVersion >= 3)
		{
			pStream->Read( &pStruct->dwUnkAct3, sizeof(DWORD) );
		}
#endif
		//rlkt_act4
		if (Header.nVersion >= 4)
		{
			pStream->Read(&pStruct->bUnkAct4, sizeof(bool));
		}

		//rlkt_act5
		if (Header.nVersion >= 5)
		{
			pStream->Read(&pStruct->dwUnkAct5, sizeof(DWORD));
		}

		pStream->Read( &nSignalCount, sizeof(int) );

		for( int j=0; j<nSignalCount; j++ ) 
		{
			CEtActionSignal *pSignal = new CEtActionSignal;
			pSignal->SetSignalListArrayIndex( j );

			if( pSignal->LoadSignal( pStream ) == false ) {
				SAFE_DELETE( pSignal );
				continue;
			}
			if( pAction->IsIgnoreSignal( pSignal->GetSignalIndex() ) == true ) {
				SAFE_DELETE( pSignal );
				continue;
			}

			pStruct->pVecSignalList.push_back( pSignal );
		}
		std::sort( pStruct->pVecSignalList.begin(), pStruct->pVecSignalList.end(), Compare_Order );

		// Search 용 Map 매칭 시켜준다.
		////////////////////////////////////////////////////////////////////////////////
		std::map<int, std::vector<CEtActionSignal *>>::iterator itInner;
		for( DWORD j=0; j<pStruct->pVecSignalList.size(); j++ ) {
			itInner = pStruct->MapSearch.find( pStruct->pVecSignalList[j]->GetSignalIndex() );
			if( itInner != pStruct->MapSearch.end() ) {
				itInner->second.push_back( pStruct->pVecSignalList[j] );
				std::sort( itInner->second.begin(), itInner->second.end(), Compare_Order );
			}
			else {
				std::vector<CEtActionSignal *> VecList;
				VecList.push_back( pStruct->pVecSignalList[j] );
				pStruct->MapSearch.insert( make_pair( pStruct->pVecSignalList[j]->GetSignalIndex(), VecList ) );
			}
		}
		////////////////////////////////////////////////////////////////////////////////
		for( DWORD j=0; j<pStruct->pVecSignalList.size(); j++ ) {
			int nIndex = pStruct->pVecSignalList[j]->GetSignalListArrayIndex();
			pStruct->MapSearchByArrayIndex.insert( make_pair( nIndex, pStruct->pVecSignalList[j] ) );
		}
		////////////////////////////////////////////////////////////////////////////////

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
		// 최초 로드시 시간 설정
		pStruct->dwLastUsedTime = timeGetTime();
		pStruct->bCheckPostSignal = false;
#endif

		pContainer->pVecActionElementList.push_back( pStruct );
	}
	// Map Search 
	for( DWORD i=0; i<pContainer->pVecActionElementList.size(); i++ ) {
		pContainer->MapSearch.insert( make_pair( pContainer->pVecActionElementList[i]->szName.c_str(), (int)i ) );
	}

	pAction->SetSearchMap( &pContainer->MapSearch );
	m_szMapContainerList.insert( make_pair( szFileName, pContainer ) );

	// 새로 읽혀진 파일일 경우 CheckPreSignal 호출해서 미리 읽을것들을 읽어준다.
	for( DWORD i=0; i<pContainer->pVecActionElementList.size(); i++ ) {
		CEtActionBase::ActionElementStruct *pElement = pContainer->pVecActionElementList[i];
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		bool bCallPreSignal = true;
		if( !pContainer->szVecUsingElementList.empty() && std::find( pContainer->szVecUsingElementList.begin(), pContainer->szVecUsingElementList.end(), pElement->szParentFileName ) == pContainer->szVecUsingElementList.end() ) {
			bCallPreSignal = false;
		}
		pElement->bUsing = bCallPreSignal;
#endif

		for( DWORD j=0; j<pElement->pVecSignalList.size(); j++ ) {
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
			pContainer->pCheckInitSignalFunc( pElement, i, pElement->pVecSignalList[j], j, pAction );
			if( bCallPreSignal ) pContainer->pCheckPreSignalFunc( pContainer->pVecActionElementList[i], i, pContainer->pVecActionElementList[i]->pVecSignalList[j], j, pAction );
#else
			pContainer->pCheckPreSignalFunc( pContainer->pVecActionElementList[i], i, pContainer->pVecActionElementList[i]->pVecSignalList[j], j, pAction );
#endif
		}
	}
	return &pContainer->pVecActionElementList;
}



std::vector<CEtActionBase::ActionElementStruct *> *CEtActionCoreMng::LoadActionBasic( const char *szFileName, CEtActionBase *pAction )
{
	std::vector<CEtActionBase::ActionElementStruct *> *pExistAction = GetAction( szFileName, pAction );
	if( pExistAction ) return pExistAction;

	CResMngStream Stream( szFileName );
	if( !Stream.IsValid() ) return NULL;
	return LoadActionBasic( &Stream, pAction );
}

void CEtActionCoreMng::RemoveAction( const char *szFileName, CEtActionBase *pAction )
{
	if( !m_bUseDynamic ) return;
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);

	if( szFileName == NULL ) return;
	std::map<std::string, ActionContainerStruct*>::iterator it = m_szMapContainerList.find( szFileName );
	if( it == m_szMapContainerList.end() ) return;
	if( it->second->nRefCount > 0 ) it->second->nRefCount--;
	if( m_bUseDynamic && it->second->nRefCount == 0 ) {
		m_pVecWaitDeleteList.push_back( it->second );

		m_szMapContainerList.erase( it );
	}
}

void CEtActionCoreMng::ReleaseContainer( ActionContainerStruct *pStruct )
{
	for( DWORD j=0; j<pStruct->pVecActionElementList.size(); j++ ) {
		for( DWORD k=0; k<pStruct->pVecActionElementList[j]->pVecSignalList.size(); k++ ) {
			pStruct->pCheckPostSignalFunc( pStruct->pVecActionElementList[j], j, pStruct->pVecActionElementList[j]->pVecSignalList[k], k, NULL );
		}
		SAFE_DELETE_PVEC( pStruct->pVecActionElementList[j]->pVecSignalList );
	}
	SAFE_DELETE_PVEC( pStruct->pVecActionElementList );
	SAFE_DELETE( pStruct );
}

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
void CEtActionCoreMng::ProcessOptimize( float fDelta )
{
	if( !m_bProcessOptimize )
		return;

	m_fOptimizeCheckTime += fDelta;
	if( m_fOptimizeCheckTime < 3.0f ) return;
	m_fOptimizeCheckTime -= 3.0f;

	const DWORD dwOptimizeTime = 120000;

	DWORD dwCurrTime = timeGetTime();
	std::map<std::string, ActionContainerStruct*>::iterator it;
	for (it = m_szMapContainerList.begin(); it != m_szMapContainerList.end(); it++)
	{
		ActionContainerStruct *pStruct = it->second;
		for( DWORD j=0; j<pStruct->pVecActionElementList.size(); j++ )
		{
			if( pStruct->pVecActionElementList[j]->bCheckPostSignal ) continue;
			if( pStruct->pVecActionElementList[j]->dwLastUsedTime + dwOptimizeTime < dwCurrTime )
			{
				for( DWORD k=0; k<pStruct->pVecActionElementList[j]->pVecSignalList.size(); k++ ) {
					if( IsOptimizeSignal( pStruct->pVecActionElementList[j]->pVecSignalList[k]->GetSignalIndex() ) )
						pStruct->pCheckPostSignalFunc( pStruct->pVecActionElementList[j], j, pStruct->pVecActionElementList[j]->pVecSignalList[k], k, NULL );
				}
				pStruct->pVecActionElementList[j]->bCheckPostSignal = true;
			}
		}
	}

	// WaitDeleteList에 들어있는 애들도 해줘야한다.
	std::vector<ActionContainerStruct*>::iterator iter = m_pVecWaitDeleteList.begin();
	for( ; iter != m_pVecWaitDeleteList.end(); iter++ )
	{
		ActionContainerStruct *pStruct = *iter;
		for( DWORD j=0; j<pStruct->pVecActionElementList.size(); j++ )
		{
			if( pStruct->pVecActionElementList[j]->bCheckPostSignal ) continue;
			if( pStruct->pVecActionElementList[j]->dwLastUsedTime + dwOptimizeTime < dwCurrTime )
			{
				for( DWORD k=0; k<pStruct->pVecActionElementList[j]->pVecSignalList.size(); k++ ) {
					if( IsOptimizeSignal( pStruct->pVecActionElementList[j]->pVecSignalList[k]->GetSignalIndex() ) )
						pStruct->pCheckPostSignalFunc( pStruct->pVecActionElementList[j], j, pStruct->pVecActionElementList[j]->pVecSignalList[k], k, NULL );
				}
				pStruct->pVecActionElementList[j]->bCheckPostSignal = true;
			}
		}
	}
}

bool CEtActionCoreMng::IsOptimizeSignal( int nSignalIndex )
{
	for( int i = 0; i < (int)m_vecOptimizeSignalIndex.size(); ++i )
	{
		if( m_vecOptimizeSignalIndex[i] == nSignalIndex )
			return true;
	}
	return false;
}
#endif

void CEtActionCoreMng::FlushWaitDelete()
{
	while( m_pVecWaitDeleteList.size() ) {
		ActionContainerStruct *pStruct = m_pVecWaitDeleteList[0];
		ReleaseContainer( pStruct );
		m_pVecWaitDeleteList.erase( m_pVecWaitDeleteList.begin() );
	}
}

CEtActionCoreMng::ActionContainerStruct *CEtActionCoreMng::GetContainer( const char *szFileName )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	std::map<std::string, ActionContainerStruct*>::iterator it = m_szMapContainerList.find( szFileName );
	if( it != m_szMapContainerList.end() ) return it->second;

	return NULL;
}

//#if defined(_RDEBUG) || defined(_DEBUG)
void CEtActionCoreMng::ReleaseAllContainer()
{
	FlushWaitDelete();
	std::map<std::string, ActionContainerStruct*>::iterator ii;
	for (ii = m_szMapContainerList.begin(); ii != m_szMapContainerList.end(); ii++)
	{
		ReleaseContainer((*ii).second);
	}
	m_szMapContainerList.clear();
}
//#endif		//#if defined(_RDEBUG) || defined(_DEBUG)
#pragma warning(default:4996)