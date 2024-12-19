#include "StdAfx.h"
#include "EtSoundEngine.h"
#include "EtSoundChannel.h"
#include "EtSoundChannelGroup.h"
DECL_SMART_PTR_STATIC( CEtSoundChannel, 256 )

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSoundChannel::CEtSoundChannel( int nIndex, int nSoundIndex, CEtSoundChannelGroup *pGroup )
{
	m_pGroup = pGroup;
	m_nIndex = nIndex;
	m_nSoundIndex = nSoundIndex;
	m_pCustomRollOffArray = NULL;
	m_pChannel = NULL;

	m_nSmartPtrIndex = GetMyIndex();
}

CEtSoundChannel::~CEtSoundChannel()
{
	if( m_pChannel ) {
		SetCallback( NULL );
		SetRollOff( 0 );
		if( IsPlay() || !IsPause() ) {
			CEtSoundEngine::s_FMODFuncLock.Lock();
			m_pChannel->stop();
			CEtSoundEngine::s_FMODFuncLock.UnLock();
		}

	}
	SAFE_DELETEA( m_pCustomRollOffArray );
}

void CEtSoundChannel::SetChannel( FMOD::Channel *pChannel )
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	m_pChannel = pChannel;
	if( m_pChannel ) FMOD_API_CHECK( m_pChannel->setUserData( (void*)&m_nSmartPtrIndex ) );
}

void CEtSoundChannel::SetLoopCount( int nCount )
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	m_pChannel->setLoopCount( nCount );
}

int CEtSoundChannel::GetLoopCount()
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	int nLoopCount = 0;
	m_pChannel->getLoopCount( &nLoopCount );
	return nLoopCount;
}

bool CEtSoundChannel::IsPlay()
{
	bool bPlay;

	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_RESULT fr = m_pChannel->isPlaying( &bPlay );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( fr != FMOD_ERR_CHANNEL_STOLEN && fr != FMOD_ERR_INVALID_HANDLE  ) {
		FMOD_API_CHECK( fr );
	}
	return bPlay;
}

bool CEtSoundChannel::IsPause()
{
	bool bPause;
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_RESULT fr = m_pChannel->getPaused( &bPause );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( fr != FMOD_ERR_CHANNEL_STOLEN && fr != FMOD_ERR_INVALID_HANDLE ) {
		FMOD_API_CHECK( fr );
	}
	return bPause;
}

void CEtSoundChannel::Pause()
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	FMOD_API_CHECK( m_pChannel->setPaused( true ) );
}

void CEtSoundChannel::Resume()
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	FMOD_API_CHECK( m_pChannel->setPaused( false ) );
}

void CEtSoundChannel::SetVolume( float fValue )
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	FMOD_API_CHECK( m_pChannel->setVolume( fValue ) );
}

float CEtSoundChannel::GetVolume()
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	float fValue;
	FMOD_API_CHECK( m_pChannel->getVolume( &fValue ) );
	return fValue;
}

void CEtSoundChannel::SetPosition( EtVector3 &vPos )
{
	FMOD_VECTOR Pos = { vPos.x, vPos.y, vPos.z };
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_RESULT fr = m_pChannel->set3DAttributes( &Pos, NULL );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( fr != FMOD_ERR_CHANNEL_STOLEN && fr != FMOD_ERR_INVALID_HANDLE  ) {
		FMOD_API_CHECK( fr );
	}
}

EtVector3 CEtSoundChannel::GetPosition()
{
	FMOD_VECTOR Pos;
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pChannel->get3DAttributes( &Pos, NULL ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	return EtVector3( Pos.x, Pos.y, Pos.z );
}

void CEtSoundChannel::SetCallback( FMOD_RESULT (_stdcall *Func)(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)  )
{
#pragma warning(disable:4311)	
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_RESULT fr = m_pChannel->setCallback( Func );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( fr != FMOD_ERR_CHANNEL_STOLEN && fr != FMOD_ERR_INVALID_HANDLE  ) {
		FMOD_API_CHECK( fr );
	}
#pragma warning(default:4311)
}

void CEtSoundChannel::SetCallback( FMOD_RESULT (__stdcall *Func)(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2), void *pValue )
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	FMOD_API_CHECK( m_pChannel->setUserData( pValue ) );
	FMOD_API_CHECK( m_pChannel->setCallback( Func ) );
}

void CEtSoundChannel::SetRollOff( int nCount, ... )
{
	FMOD_MODE Mode;
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pChannel->getMode( &Mode ) );
	if( ( Mode & FMOD_3D ) && ( Mode & FMOD_3D_CUSTOMROLLOFF ) ) {
		Mode &= ~FMOD_3D_CUSTOMROLLOFF;
		FMOD_API_CHECK( m_pChannel->setMode( Mode ) );
	}
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	SAFE_DELETEA( m_pCustomRollOffArray );
	if( nCount <= 0 ) return;
	m_pCustomRollOffArray = new FMOD_VECTOR[nCount];

	va_list list;
	va_start( list, nCount );

	for( int i=0; i<nCount; i++ ) {
		m_pCustomRollOffArray[i].x = (float)va_arg( list, double );
		m_pCustomRollOffArray[i].y = (float)va_arg( list, double );
		m_pCustomRollOffArray[i].z = 0.f;
	}
	va_end( list );


	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pChannel->getMode( &Mode ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( !( Mode & FMOD_3D ) ) return;
	if( !( Mode & FMOD_3D_CUSTOMROLLOFF ) ) Mode |= FMOD_3D_CUSTOMROLLOFF;

	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pChannel->setMode( Mode ) );
	FMOD_API_CHECK( m_pChannel->set3DCustomRolloff( m_pCustomRollOffArray, nCount ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	
}

bool CEtSoundChannel::AddDSP( int nIndex )
{
	FMOD_DSP_TYPE Type;
	FMOD::DSP *pDSP;
	for( DWORD i=0; i<m_pVecDSPList.size(); i++ ) {
		pDSP = m_pVecDSPList[i];
		FMOD_API_CHECK( pDSP->getType( &Type ) );
		if( (int)Type == nIndex ) return true;
	}

	FMOD::System *pSystem = m_pGroup->GetEngine()->GetFMODSystem();
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_RESULT result = pSystem->createDSPByType( (FMOD_DSP_TYPE)nIndex, &pDSP );
	CEtSoundEngine::s_FMODFuncLock.UnLock();
	if( result != FMOD_OK ) return false;

	m_pVecDSPList.push_back( pDSP );
	CEtSoundEngine::s_FMODFuncLock.Lock();
	FMOD_API_CHECK( pDSP->setActive( true ));
	FMOD_API_CHECK( m_pChannel->addDSP( pDSP, NULL ) );
	CEtSoundEngine::s_FMODFuncLock.UnLock();

	return true;
}

void CEtSoundChannel::RemoveDSP( int nIndex )
{
	FMOD_DSP_TYPE Type;
	FMOD::DSP *pDSP;
	for( DWORD i=0; i<m_pVecDSPList.size(); i++ ) {
		pDSP = m_pVecDSPList[i];
		FMOD_API_CHECK( pDSP->getType( &Type ) );
		if( (int)Type == nIndex ) {
			CEtSoundEngine::s_FMODFuncLock.Lock();
			FMOD_API_CHECK( pDSP->setActive( false ) );
			FMOD_API_CHECK( pDSP->release() );
			CEtSoundEngine::s_FMODFuncLock.UnLock();
			m_pVecDSPList.erase( m_pVecDSPList.begin() + i );
			return;
		}
	}
}

void CEtSoundChannel::SetDSPParameter( int nIndex, int nParamIndex, float fValue )
{
	ScopeLock<CSyncLock> Lock(CEtSoundEngine::s_FMODFuncLock);
	FMOD_DSP_TYPE Type;
	FMOD::DSP *pDSP = NULL;
	for( DWORD i=0; i<m_pVecDSPList.size(); i++ ) {
		pDSP = m_pVecDSPList[i];
		FMOD_API_CHECK( pDSP->getType( &Type ));
		if( (int)Type == nIndex ) {
			FMOD_API_CHECK( pDSP->setParameter( nParamIndex, fValue ) );
		}
	}
}
void CEtSoundChannel::SetPriority( int nValue )
{
	CEtSoundEngine::s_FMODFuncLock.Lock();
	m_pChannel->setPriority(0);
	CEtSoundEngine::s_FMODFuncLock.UnLock();
}