#include "StdAfx.h"
#include "DnWorldSoundEnvi.h"
#include "EtSoundEngine.h"
#include "EtSoundChannel.h"
#include "EtWorldSector.h"
#include "EtWorldSound.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldSoundEnvi::CDnWorldSoundEnvi( CEtWorldSound *pSound )
: CEtWorldSoundEnvi( pSound )
{
	m_nSoundIndex = -1;
}

CDnWorldSoundEnvi::~CDnWorldSoundEnvi()
{
	if( m_nSoundIndex != -1 ) {
		if( m_hChannel ) CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
		CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
		m_hChannel.Identity();
		m_nSoundIndex = -1;
	}
}

bool CDnWorldSoundEnvi::Load( CStream *pStream )
{
	bool bResult = CEtWorldSoundEnvi::Load( pStream );
	if( bResult == false ) return false;

	if( !m_szFileName.empty() ) {
		m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( m_szFileName ).c_str(), true, m_bStream );
	}
	return bResult;
}

void CDnWorldSoundEnvi::Play()
{
	if( m_nSoundIndex == -1 ) return;

	CEtWorldSector *pParentSector = m_pSound->GetSector();
	EtVector3 vOffset = *pParentSector->GetOffset();
	vOffset.x -= ( pParentSector->GetTileWidthCount() * pParentSector->GetTileSize() ) / 2.f;
	vOffset.z -= ( pParentSector->GetTileHeightCount() * pParentSector->GetTileSize() ) / 2.f;
	vOffset.y = 0.f;


	m_hChannel = CEtSoundEngine::GetInstance().PlaySound( "3D", m_nSoundIndex, true, true );
	if( m_hChannel ) {
		m_hChannel->SetVolume( m_fVolume );
		m_hChannel->SetPosition( m_vPosition + vOffset );
		m_hChannel->SetRollOff( 3, 0.f, 1.f, GetRange() * GetRollOff(), 1.f, GetRange(), 0.f );
		m_hChannel->Resume();
	}
}

void CDnWorldSoundEnvi::Stop()
{
	if( m_nSoundIndex == -1 ) return;
	if( !m_hChannel ) return;
	CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
	m_hChannel.Identity();
}
