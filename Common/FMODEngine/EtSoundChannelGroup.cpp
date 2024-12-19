#include "StdAfx.h"
#include "EtSoundChannelGroup.h"
#include "EtSoundChannel.h"
#include "SundriesFunc.h"
#include "EtSoundEngine.h"
#include "fmod.hpp"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CEtSoundChannelGroup::CEtSoundChannelGroup( CEtSoundEngine *pEngine, const char *szName, int nIndex )
{
	m_szName = szName;
	m_nIndex = nIndex;
	m_nChannelRefCount = 0;
	m_pGroup = NULL;
	m_pEngine = pEngine;
	m_fTargetVolume = 0.f;
	m_fCurrentVolume = 0.f;
	m_fCurrentDelta = 0.f;

	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pEngine->GetFMODSystem()->createChannelGroup( szName, &m_pGroup ) );
	FMOD_API_CHECK( m_pGroup->getVolume( &m_fTargetVolume ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	m_fCurrentVolume = m_fTargetVolume;
	m_fVolumeDelta = 0.f;

	m_fMasterVolume = 1.f;
}

CEtSoundChannelGroup::~CEtSoundChannelGroup()
{
	RemoveAllChannel();
	if( m_pGroup ) {
		CEtSoundEngine::s_FMODFuncLock.Lock();
		FMOD_API_CHECK( m_pGroup->release() );		// 이곳에서 가끔 크래쉬... WinAmp 에서 Output Device 를 
																		// DirectSound 로 세팅하는 경우 종종 충돌이 생기는듯....
		CEtSoundEngine::s_FMODFuncLock.UnLock();

		m_pGroup = NULL;
	}
}

const char *CEtSoundChannelGroup::GetName()
{
	return m_szName.c_str();
}

int CEtSoundChannelGroup::GetIndex()
{
	return m_nIndex;
} 

EtSoundChannelHandle CEtSoundChannelGroup::InsertChannel( int nSoundIndex, FMOD::Channel *pChannel )
{
	ScopeLock<CSyncLock> Lock(m_InsertLock);
	EtSoundChannelHandle hChannel = (new CEtSoundChannel( m_nChannelRefCount, nSoundIndex, this ))->GetMySmartPtr();
	hChannel->SetChannel( pChannel );
	m_hVecChannelList.push_back( hChannel );
	m_nChannelRefCount++;

	hChannel->SetCallback( ChannelStopCallbackFunc );
	return hChannel;
}

void CEtSoundChannelGroup::RemoveChannel( int nChannelIndex )
{
	ScopeLock<CSyncLock> Lock(m_InsertLock);
	for( DWORD i=0; i<m_hVecChannelList.size(); i++ ) {
		if( !m_hVecChannelList[i] ) {
			m_hVecChannelList.erase( m_hVecChannelList.begin() + i );
			i--;
			continue;
		}
		if( m_hVecChannelList[i]->GetIndex() == nChannelIndex ) {
			SAFE_RELEASE_SPTR( m_hVecChannelList[i] );
			m_hVecChannelList.erase( m_hVecChannelList.begin() + i );
			break;
		}
	}
}


void CEtSoundChannelGroup::RemoveAllChannel()
{
	CEtSoundEngine::s_FMODFuncLock.Lock();
	m_pGroup->stop();
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	SAFE_RELEASE_SPTRVEC( m_hVecChannelList );
}

EtSoundChannelHandle CEtSoundChannelGroup::GetChannel( int nChannelIndex )
{
	for( DWORD i=0; i<m_hVecChannelList.size(); i++ ) {
		if( m_hVecChannelList[i]->GetIndex() == nChannelIndex ) return m_hVecChannelList[i];
	}
	return CEtSoundChannel::Identity();
}

void CEtSoundChannelGroup::Process( float fDelta )
{
	if( m_fCurrentDelta > 0.f ) {
		m_fCurrentDelta -= fDelta;
		if( m_fCurrentDelta <= 0.f ) m_fCurrentDelta = 0.f;
		float fRatio = 1.f - ( 1.f / m_fVolumeDelta * m_fCurrentDelta );
		float fVolume = m_fCurrentVolume + ( ( m_fTargetVolume - m_fCurrentVolume ) * fRatio );
		CEtSoundEngine::s_FMODFuncLock.Lock();
		FMOD_API_CHECK(  m_pGroup->setVolume( fVolume * ( IsMute() ? 0.f : m_fMasterVolume ) ) );
		CEtSoundEngine::s_FMODFuncLock.UnLock();
	}
}

FMOD_RESULT F_CALLBACK CEtSoundChannelGroup::ChannelStopCallbackFunc( FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2 )
{
#pragma warning(disable:4312)
	if( type == FMOD_CHANNEL_CALLBACKTYPE_END ) {
		void *pUserData;
		FMOD_API_CHECK(  ((FMOD::Channel *)channel)->getUserData( &pUserData ) );
		int nIndex = *(int*)pUserData;

		EtSoundChannelHandle hChannel = CEtSoundChannel::GetSmartPtr( nIndex );

		if( hChannel && hChannel->GetGroup() ) {
			hChannel->SetRollOff( 0 );
			hChannel->SetChannel( NULL );
			hChannel->GetGroup()->RemoveChannel( hChannel->GetIndex() );
		}
		return FMOD_OK;
	}
	return FMOD_OK;
#pragma warning(default:4312)
}

void CEtSoundChannelGroup::SetVolume( float fValue, float fDelta )
{
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK(  m_pGroup->getVolume( &m_fCurrentVolume ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	m_fCurrentVolume *= ( m_fMasterVolume <= 0.f ) ? 0.f : ( 1.f / m_fMasterVolume );
	m_fCurrentDelta = fDelta;
	m_fVolumeDelta = fDelta;
	m_fTargetVolume = fValue;
	if( fDelta == 0.f ) {
		CEtSoundEngine::s_FMODFuncLock.Lock();
		FMOD_API_CHECK( m_pGroup->setVolume( fValue * ( IsMute() ? 0.f : m_fMasterVolume ) ) );
		CEtSoundEngine::s_FMODFuncLock.UnLock();
	}
}

float CEtSoundChannelGroup::GetVolume()
{
	return m_fTargetVolume;
}

void CEtSoundChannelGroup::CheckSoundIndexAndRemoveChannel( int nSoundIndex )
{
	ScopeLock<CSyncLock> Lock(m_InsertLock);
	for( DWORD i=0; i<m_hVecChannelList.size(); i++ ) {
		if( m_hVecChannelList[i]->GetSoundIndex() == nSoundIndex ) {
			SAFE_RELEASE_SPTR( m_hVecChannelList[i] );
			m_hVecChannelList.erase( m_hVecChannelList.begin() + i );
			i--;
		}
	}
}


void CEtSoundChannelGroup::Pause()
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	FMOD_API_CHECK(  m_pGroup->setPaused( true ) );
}

void CEtSoundChannelGroup::Resume()
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	FMOD_API_CHECK( m_pGroup->setPaused( false ));
}

void CEtSoundChannelGroup::Stop()
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	FMOD_API_CHECK( m_pGroup->stop());
}

void CEtSoundChannelGroup::Play()
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	FMOD_API_CHECK( m_pGroup->setPaused( false ));
}

void CEtSoundChannelGroup::SetMute( bool bMute )
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	FMOD_API_CHECK( m_pGroup->setMute( bMute ));
	if( bMute ) {
		FMOD_API_CHECK( m_pGroup->setVolume( 0.f ));
	}
	else FMOD_API_CHECK( m_pGroup->setVolume( m_fTargetVolume * m_fMasterVolume ));
}

bool CEtSoundChannelGroup::IsMute()
{
	ScopeLock<CSyncLock> Lock( CEtSoundEngine::s_FMODFuncLock );
	bool bMute;
	FMOD_API_CHECK( m_pGroup->getMute( &bMute ));
	return bMute;
}

void CEtSoundChannelGroup::SetMasterVolume( float fValue )
{
	m_fMasterVolume = fValue;

	CEtSoundEngine::s_FMODFuncLock.Lock();
	if( IsMute() ) {
		FMOD_API_CHECK( m_pGroup->setVolume( 0.f ));
	}
	else FMOD_API_CHECK( m_pGroup->setVolume( m_fTargetVolume * m_fMasterVolume ));
	CEtSoundEngine::s_FMODFuncLock.UnLock();
//	FMOD_API_CHECK( m_pGroup->setVolume( m_fTargetVolume * m_fMasterVolume ));
}

float CEtSoundChannelGroup::GetMasterVolume()
{
	return m_fMasterVolume;
}