#include "StdAfx.h"
#include "EtWorldSound.h"
#include "EtWorldSoundEnvi.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtWorldSound::CEtWorldSound( CEtWorldSector *pSector )
{
	m_pSector = pSector;
	m_fVolume = 1.f;
}

CEtWorldSound::~CEtWorldSound()
{
	SAFE_DELETE_PVEC( m_pVecEnvi );
}

bool CEtWorldSound::Load( const char *szInfoFile )
{
	CResMngStream Stream( szInfoFile );
	if( !Stream.IsValid() ) return false;

	m_szBGM.clear();
	ReadStdString( m_szBGM, &Stream );
	Stream.Read( &m_fVolume, sizeof(float) );

	int nCount;
	Stream.Read( &nCount, sizeof(int) );
	CEtWorldSoundEnvi *pEnvi;
	for( int i=0; i<nCount; i++ ) {
		pEnvi = AllocSoundEnvi();
		if( pEnvi->Load( &Stream ) == false ) {
			SAFE_DELETE( pEnvi );
			continue;
		}
		m_pVecEnvi.push_back( pEnvi );
	}

	return true;
}


CEtWorldSoundEnvi *CEtWorldSound::AllocSoundEnvi()
{
	return new CEtWorldSoundEnvi( this );
}

void CEtWorldSound::InsertSoundEnvi( CEtWorldSoundEnvi *pEnvi )
{
	m_pVecEnvi.push_back( pEnvi );
}

void CEtWorldSound::DeleteSoundEnvi( CEtWorldSoundEnvi *pEnvi )
{
	for( DWORD i=0; i<m_pVecEnvi.size(); i++ ) {
		if( m_pVecEnvi[i] == pEnvi ) {
			SAFE_DELETE( m_pVecEnvi[i] );
			m_pVecEnvi.erase( m_pVecEnvi.begin() + i );
			i--;
			break;
		}
	}
}

DWORD CEtWorldSound::GetEnviCount()
{
	return (DWORD)m_pVecEnvi.size();
}

CEtWorldSoundEnvi *CEtWorldSound::GetEnviFromIndex( DWORD dwIndex )
{
	if( dwIndex >= m_pVecEnvi.size() ) return NULL;
	return m_pVecEnvi[dwIndex];
}

CEtWorldSoundEnvi *CEtWorldSound::GetEnviFromName( const char *szName )
{
	for( DWORD i=0; i<m_pVecEnvi.size(); i++ ) {
		if( strcmp( szName, m_pVecEnvi[i]->GetName() ) == NULL ) return m_pVecEnvi[i];
	}
	return NULL;
}