#include "StdAfx.h"
#include "EtTriggerObject.h"
#include "EtTriggerElement.h"
#include "PerfCheck.h"
#include <MMSystem.h>
#include <map>
using namespace std;

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

int CEtTriggerObject::s_nDummySize = 512;

CEtTriggerObject::CEtTriggerObject( CEtTrigger *pTrigger )
{
	m_pTrigger = pTrigger;
	m_nTick = 0;
	m_LastProcessTime = 0;
	m_bRepeat = false;
	m_bEnable = true;
	m_nCreateIndex = -1;
	m_nRandomSeed = 0;
#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
	m_dTime = 0.f;
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
}

CEtTriggerObject::~CEtTriggerObject()
{
	SAFE_DELETE_PVEC( m_pVecCondition );
	SAFE_DELETE_PVEC( m_pVecAction );
	SAFE_DELETE_PVEC( m_pVecEvent );

	SAFE_DELETE_VEC( m_nVecAndConditionList );
	for( DWORD i=0; i<m_nVecOrConditionList.size(); i++ ) {
		SAFE_DELETE_VEC( m_nVecOrConditionList[i] );
	}
}

bool CEtTriggerObject::Load( CStream *pStream )
{
	int nCount;

	ReadStdString( m_szCategory, pStream );
	ReadStdString( m_szTriggerName, pStream );

	pStream->Read( &m_bRepeat, sizeof(bool) );
#if defined( PRE_TRIGGER_TEST )
	m_bRepeat = false;
#endif // #if defined( PRE_TRIGGER_TEST )
	pStream->Seek( s_nDummySize, SEEK_CUR );
	pStream->Read( &nCount, sizeof(int) );


	for( int i=0; i<nCount; i++ ) {
		CEtTriggerElement *pElement = AllocTriggerElement();
		if( pElement->Load( pStream ) == false ) {
#ifdef PRE_FIX_62845
		m_pTrigger->SetErrLuaName(pElement->GetFileName());
#endif
			SAFE_DELETE( pElement );
			continue;
		}
		m_pVecCondition.push_back( pElement );
	}

	pStream->Read( &nCount, sizeof(int) );
	for( int i=0; i<nCount; i++ ) {
		CEtTriggerElement *pElement = AllocTriggerElement();
		if( pElement->Load( pStream ) == false ) {
#ifdef PRE_FIX_62845
			m_pTrigger->SetErrLuaName(pElement->GetFileName());
#endif
			SAFE_DELETE( pElement );
			continue;
		}
		m_pVecAction.push_back( pElement );
	}

	pStream->Read( &nCount, sizeof(int) );
	for( int i=0; i<nCount; i++ ) {
		CEtTriggerElement *pElement = AllocTriggerElement();
		if( pElement->Load( pStream ) == false ) {
#ifdef PRE_FIX_62845
			m_pTrigger->SetErrLuaName(pElement->GetFileName());
#endif
			SAFE_DELETE( pElement );
			continue;
		}
		m_pVecEvent.push_back( pElement );
	}

	ReadStdString( m_szDescription, pStream );
	CalcOperatorList();
	return true;
}

void CEtTriggerObject::CalcOperatorList()
{
	// Condition 오퍼레이션 나누기
	int nMaxOffset = 0;
	CEtTriggerElement *pElement;

	for( DWORD i=0; i<m_pVecCondition.size(); i++ ) {
		pElement = m_pVecCondition[i];
		if( pElement->GetOperatorType() == CEtTriggerElement::Or ) {
			if( pElement->GetOperatorIndex() > nMaxOffset )
				nMaxOffset = pElement->GetOperatorIndex();
		}
	}
	m_nVecOrConditionList.resize( nMaxOffset + 1 );

	for( DWORD i=0; i<m_pVecCondition.size(); i++ ) {
		pElement = m_pVecCondition[i];
		switch( pElement->GetOperatorType() ) {
			case CEtTriggerElement::And:
				m_nVecAndConditionList.push_back(i);
				break;
			case CEtTriggerElement::Or:
				m_nVecOrConditionList[pElement->GetOperatorIndex()].push_back(i);
				break;
		}
	}

	// Action Operation
	for( DWORD i=0; i<m_pVecAction.size(); i++ ) {
		pElement = m_pVecAction[i];
		if( pElement->GetOperatorType() == CEtTriggerElement::Random ) {
			if( pElement->GetOperatorIndex() > nMaxOffset )
				nMaxOffset = pElement->GetOperatorIndex();
		}
	}

	m_nVecRandomActionList.resize( nMaxOffset + 1 );

	for( DWORD i=0; i<m_pVecAction.size(); i++ ) {
		pElement = m_pVecAction[i];
		switch( pElement->GetOperatorType() ) {
			case CEtTriggerElement::And:
				m_nVecAndActionList.push_back(i);
				break;
			case CEtTriggerElement::Random:
				m_nVecRandomActionList[pElement->GetOperatorIndex()].push_back(i);
				break;
		}
	}

}

CEtTriggerElement *CEtTriggerObject::AllocTriggerElement()
{
	return new IBoostPoolEtTriggerElement( this, NULL );
}

CEtTriggerElement *CEtTriggerObject::GetEventFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_pVecEvent.size() ) return NULL;
	return m_pVecEvent[dwIndex];
}

CEtTriggerElement *CEtTriggerObject::GetConditionFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_pVecCondition.size() ) return NULL;
	return m_pVecCondition[dwIndex];
}

CEtTriggerElement *CEtTriggerObject::GetActionFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_pVecAction.size() ) return NULL;
	return m_pVecAction[dwIndex];
}

bool CEtTriggerObject::Process( LOCAL_TIME LocalTime, float fDelta )
{
#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
	QueryPerformanceFrequency(&liFrequency);
	QueryPerformanceCounter(&liStartTime);	
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )

	m_LastProcessTime = LocalTime;

	bool bValidAction = true;
	/*
	for( DWORD i=0; i<m_pVecCondition.size(); i++ ) {
		if( m_pVecCondition[i]->CallLuaFunction() == false ) {
			bValidAction = false;
			break;
		}
	}
	*/
	for( DWORD i=0; i<m_nVecAndConditionList.size(); i++ ) {
		if( m_pVecCondition[m_nVecAndConditionList[i]]->CallLuaFunction() == false ) {
			bValidAction = false;
			break;
		}		
	}
	if( bValidAction ) {
		bool bValidOrCondition = false;
		for( DWORD i=0; i<m_nVecOrConditionList.size(); i++ ) {
			bool bValid = true;
			for( DWORD j=0; j<m_nVecOrConditionList[i].size(); j++ ) {
				if( m_pVecCondition[m_nVecOrConditionList[i][j]]->CallLuaFunction() == false ) {
					bValid = false;
					break;
				}
			}
			if( bValid ) {
				bValidOrCondition = true;
				break;
			}
		}
		if( !bValidOrCondition ) bValidAction = false;
	}

#if defined( PRE_TRIGGER_TEST )
	bValidAction = true;
#endif // #if defined( PRE_TRIGGER_TEST )

	if( bValidAction == false ) {
		ResetTriggerEventStore();
		return false;
	}

	SetEnable( false );

	if( !m_pVecAction.empty() ) {
		m_pVecAction[0]->SetLastExcuteTime( LocalTime );
//		for( DWORD i=0; i<m_pVecAction.size(); i++ )
//			m_pVecAction[i]->SetLastExcuteTime( LocalTime );
	}

	m_nRandomSeed = timeGetTime();
	m_pTrigger->OnCallTriggerAction( this );
	m_pTrigger->InsertActionQueue( this );

#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
	QueryPerformanceCounter(&liCurTime);
	m_dTime += 1000 * ( liCurTime.QuadPart - liStartTime.QuadPart ) / static_cast<double>( liFrequency.QuadPart );
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )

	return true;
}

bool CEtTriggerObject::IsCanProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( !IsEnable() ) return false;
	if( m_pTrigger->IsProcessActionQueue( this ) ) return false;
	if( m_LastProcessTime > LocalTime ) LocalTime = m_LastProcessTime;
	if( (int)( LocalTime - m_LastProcessTime ) >= m_nTick ) return true;
	return false;
}

bool CEtTriggerObject::ProcessQueue( LOCAL_TIME LocalTime, float fDelta )
{
#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
	QueryPerformanceFrequency(&liFrequency);
	QueryPerformanceCounter(&liStartTime);	
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )

	CEtTriggerElement *pElement;
	DWORD i;
	for( i=0; i<m_pVecAction.size(); i++ ) {
		pElement = m_pVecAction[i];
		if( !pElement->IsEnable() ) continue;

		// 랜덤일 경우는 첫번재껏만으로 판단해서 나머지 같은 그룹 엘리먼트들 속성을 잠시 바꿔준다.
		if( pElement->GetOperatorType() == CEtTriggerElement::Random ) {
			// 랜덤 실패했으면
			m_pTrigger->RandomSeed( GetRandomSeed() + i );
			if( m_pTrigger->Random()%100 > pElement->GetOperatorValue(0) ) {
				for( DWORD j=0; j<m_nVecRandomActionList[pElement->GetOperatorIndex()].size(); j++ ) {
					m_pVecAction[ m_nVecRandomActionList[pElement->GetOperatorIndex()][j] ]->SetEnable( false );
				}
				continue;
			}
			// 성공햇으면
			else {
				for( DWORD j=1; j<m_nVecRandomActionList[pElement->GetOperatorIndex()].size(); j++ ) {
					m_pVecAction[ m_nVecRandomActionList[pElement->GetOperatorIndex()][j] ]->SetOperatorType( CEtTriggerElement::And );
				}
			}
		}

		if( pElement->CallLuaFunction() == true ) {
			pElement->SetEnable( false );
			if( i + 1 < m_pVecAction.size() ) {
				m_pVecAction[i+1]->SetLastExcuteTime( LocalTime );
			}
		}
		else break;
	}

	if( i == m_pVecAction.size() ) {
		for( i=0; i<m_pVecAction.size(); i++ ) {
			m_pVecAction[i]->SetEnable( true );
		}
		SetEnable( true );
		ResetTriggerEventStore();

#if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )
		QueryPerformanceCounter(&liCurTime);
		m_dTime += 1000 * ( liCurTime.QuadPart - liStartTime.QuadPart ) / static_cast<double>( liFrequency.QuadPart );
#endif // #if defined( PRE_TRIGGER_TEST ) || defined( PRE_TRIGGER_UNITTEST_LOG )

		return true;
	}
	return false;
}

void CEtTriggerObject::InsertEventValueStore( const char *szValueStr, int nValue )
{
	std::map<std::string,int>::iterator it = m_MapEventValueStore.find( szValueStr );
	if( it == m_MapEventValueStore.end() )
		m_MapEventValueStore.insert( make_pair( szValueStr, nValue ) );
	else it->second = nValue;
}

int CEtTriggerObject::GetEventValueStore( const char *szValueStr ) 
{
	std::map<std::string,int>::iterator it = m_MapEventValueStore.find( szValueStr );
	if( it == m_MapEventValueStore.end() ) return -1;

	return it->second;
}


void CEtTriggerObject::ResetTriggerEventStore()
{
	std::map<std::string,int>::iterator it = m_MapEventValueStore.begin();
	for( ; it != m_MapEventValueStore.end(); it++ ) {
		it->second = -1;
	}
}