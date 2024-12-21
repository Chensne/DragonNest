#include "StdAfx.h"
#include "DnWorldSound.h"
#include "DnWorldSoundEnvi.h"
#include "EtSoundEngine.h"
#include "EtSoundChannel.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldSound::CDnWorldSound( CEtWorldSector *pSector )
: CEtWorldSound( pSector )
{
	m_nSoundIndex = -1;
}

CDnWorldSound::~CDnWorldSound()
{
	if( m_nSoundIndex != -1 ) {
		CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
		CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
		m_hChannel.Identity();
		m_nSoundIndex = -1;
	}
}

bool CDnWorldSound::Load( const char *szInfoFile )
{
	bool bResult = CEtWorldSound::Load( szInfoFile );
	if( bResult == false ) return false;

	if( !m_szBGM.empty() ) {
		m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( m_szBGM ).c_str(), false, true );
	}

	return bResult;
}

void CDnWorldSound::Play()
{
	if( m_nSoundIndex != -1 ) {
		m_hChannel = CEtSoundEngine::GetInstance().PlaySound( "BGM", m_nSoundIndex, true, true );
		if( m_hChannel ) {
			m_hChannel->SetVolume( m_fVolume );
			m_hChannel->Resume();
		}
	}

	for( DWORD i=0; i<m_pVecEnvi.size(); i++ ) {
		m_pVecEnvi[i]->Play();
	}
}

void CDnWorldSound::Stop()
{
	if( m_nSoundIndex != -1 )
	{
		if( m_hChannel )
		{
			CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
			m_hChannel.Identity();
		}
	}

	for( DWORD i=0; i<m_pVecEnvi.size(); i++ ) {
		m_pVecEnvi[i]->Stop();
	}
}

void CDnWorldSound::SetVolume( float fValue )
{
	if( m_hChannel ) {
		m_hChannel->SetVolume( fValue );
	}
}

CEtWorldSoundEnvi *CDnWorldSound::AllocSoundEnvi()
{
	return new CDnWorldSoundEnvi( this );
}


void CDnWorldSound::ChangeBGM( const char *szFileName )
{
	Stop();
	if( m_nSoundIndex != -1 ) {
		CEtSoundEngine::GetInstance().RemoveChannel( m_hChannel );
		CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIndex );
		m_hChannel.Identity();
		m_nSoundIndex = -1;
	}

	m_szBGM = std::string( szFileName );

	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, true );
	Play();
}

void CDnWorldSound::FadeVolumeBGM( const float fTargetVolume, const float fDelta )
{
	CEtSoundEngine::GetInstance().FadeVolume( "BGM", fTargetVolume, CGlobalInfo::GetInstance().m_fFadeDelta, false );
}