#include "StdAfx.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include <utility>
using namespace std;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtWorldEventControl::CEtWorldEventControl( CEtWorldSector *pSector )
{
	m_pSector = pSector;
	m_nUniqueID = -1;
}

CEtWorldEventControl::~CEtWorldEventControl()
{
	SAFE_DELETE_PVEC( m_pVecList );
	SAFE_DELETE_VEC( m_nMapCreateUniqueIDSearch );
}

bool CEtWorldEventControl::Initialize()
{
	return true;
}

bool CEtWorldEventControl::Load( CStream *pStream )
{
	int nCount;
	pStream->Read( &m_nUniqueID, sizeof(int) );
	pStream->Read( &nCount, sizeof(int) );
	for( int i=0; i<nCount; i++ ) {
		CEtWorldEventArea *pArea = AllocArea();
		if( pArea->Load( pStream ) == false ) {
			SAFE_DELETE( pArea );
			continue;
		}
		m_nMapCreateUniqueIDSearch.insert( make_pair( pArea->GetCreateUniqueID(), pArea ) );
		m_pVecList.push_back( pArea );
	}

	return true;
}

void CEtWorldEventControl::Process( EtVector3 &vPos, bool bIgnoreY )
{
	m_dwVecCheckList.clear();
	CEtWorldEventArea *pArea;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		pArea = m_pVecList[i];
		if( !pArea->IsActive() ) continue;
		if( pArea->CheckArea( vPos, bIgnoreY ) == true )
			m_dwVecCheckList.push_back( i );
	}
}

DWORD CEtWorldEventControl::GetAreaCount()
{
	return (DWORD)m_pVecList.size();
}

CEtWorldEventArea *CEtWorldEventControl::GetAreaFromIndex( DWORD dwIndex )
{
	return m_pVecList[dwIndex];
}

CEtWorldEventArea *CEtWorldEventControl::GetAreaFromName( const char *szName, int nIndex )
{
	int nOffset = 0;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( strcmp( m_pVecList[i]->GetName(), szName ) == NULL ) {
			if( nOffset == nIndex ) return m_pVecList[i];
			nOffset++;
		}
	}
	return NULL;
}

CEtWorldEventArea *CEtWorldEventControl::GetAreaFromCreateUniqueID( int nUniqueID )
{
	std::map<int, CEtWorldEventArea *>::iterator it = m_nMapCreateUniqueIDSearch.find( nUniqueID );
	if( it != m_nMapCreateUniqueIDSearch.end() ) return it->second;
	return NULL;
}

int CEtWorldEventControl::FindAreaFromName( const char *szName, std::vector<CEtWorldEventArea *> *pVecList )
{
	int nCount = 0;
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( strcmp( m_pVecList[i]->GetName(), szName ) == NULL ) {
			if( pVecList ) pVecList->push_back( m_pVecList[i] );
			nCount++;
		}
	}
	return nCount;
}

DWORD CEtWorldEventControl::GetCheckAreaCount()
{
	return (DWORD)m_dwVecCheckList.size();
}

CEtWorldEventArea *CEtWorldEventControl::GetCheckArea( DWORD dwIndex )
{
	return m_pVecList[ m_dwVecCheckList[dwIndex] ];
}

int CEtWorldEventControl::FindCheckAreaFromSubStr( const char *szStr, std::vector<CEtWorldEventArea *> *pVecList )
{
	CEtWorldEventArea *pArea;
	int nCount = 0;
	for( DWORD i=0; i<m_dwVecCheckList.size(); i++ ) {
		pArea = GetCheckArea(i);
		if( strstr( pArea->GetName(), szStr ) ) {
			if( pVecList ) pVecList->push_back( pArea );
			nCount++;
		}
	}
	return nCount;
}

CEtWorldEventArea *CEtWorldEventControl::AllocArea()
{
	return new CEtWorldEventArea( this );
}

CEtWorldEventArea *CEtWorldEventControl::InsertArea( EtVector3 &vMin, EtVector3 &vMax, const char *szName )
{
	CEtWorldEventArea *pArea = AllocArea();
	pArea->SetName( szName );
	pArea->SetMin( vMin );
	pArea->SetMax( vMax );

	m_pVecList.push_back( pArea );
	m_nMapCreateUniqueIDSearch.insert( make_pair( pArea->GetCreateUniqueID(), pArea ) );
	return pArea;
}

void CEtWorldEventControl::RemoveArea( const char *szName )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( strcmp( szName, m_pVecList[i]->GetName() ) == NULL ) {
			m_nMapCreateUniqueIDSearch.erase( m_pVecList[i]->GetCreateUniqueID() );
			SAFE_DELETE( m_pVecList[i] );
			m_pVecList.erase( m_pVecList.begin() + i );
		}
	}
}

void CEtWorldEventControl::RemoveArea( DWORD dwIndex )
{
	if( dwIndex >= m_pVecList.size() ) return;

	m_nMapCreateUniqueIDSearch.erase( m_pVecList[dwIndex]->GetCreateUniqueID() );
	SAFE_DELETE( m_pVecList[dwIndex] );
	m_pVecList.erase( m_pVecList.begin() + dwIndex );
}

void CEtWorldEventControl::RemoveAreaFromCreateUniqueID( int nUniqueID )
{
	for( DWORD i=0; i<m_pVecList.size(); i++ ) {
		if( nUniqueID == m_pVecList[i]->GetCreateUniqueID() ) {
			m_nMapCreateUniqueIDSearch.erase( m_pVecList[i]->GetCreateUniqueID() );
			SAFE_DELETE( m_pVecList[i] );
			m_pVecList.erase( m_pVecList.begin() + i );
		}
	}
}