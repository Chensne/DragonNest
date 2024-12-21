#include "StdAfx.h"
#include "EtSoundEngine.h"
#include "EtSoundChannelGroup.h"
#include "EtSoundChannel.h"
#include "SundriesFunc.h"
#include "EtLoader.h"
#include <process.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSoundEngine *CEtSoundEngine::s_pCurrentCallbackPtr = NULL;
CSyncLock CEtSoundEngine::s_FMODFuncLock;
bool CEtSoundEngine::s_bFMODFuncLock = false;
CSyncLock CEtSoundEngine::s_ChannelLock;
bool CEtSoundEngine::s_bChannelLock = false;

CEtSoundEngine::CEtSoundEngine()
{
	m_pSystem = NULL;
	m_nSoundRefCount = 0;
	m_bEnable = true;
	m_dwMainThreadID = 0;
	m_nTotalSoundSize = 0;

	s_pCurrentCallbackPtr = this;

	m_fMaxVolumeRate = 0.6f;	// #28730 전체적으로 소리 크다해서 60%로 낮춥니다.
}

CEtSoundEngine::~CEtSoundEngine()
{
	Finalize();
}

bool CEtSoundEngine::Initialize( OutputTypeEnum Type )
{
	ScopeLock<CSyncLock> Lock(s_FMODFuncLock);
	m_dwMainThreadID = GetCurrentThreadId();
	FMOD_RESULT Result;
	Result = FMOD::System_Create( &m_pSystem );
	if( Result != FMOD_OK ) return false;
	unsigned int nVersion;
	Result = m_pSystem->getVersion( &nVersion );
	if( Result != FMOD_OK ) return false;
	if( nVersion < FMOD_VERSION ) {
		char szStr[256];
		sprintf_s( szStr, "You are using an old version of FMOD %08x.  This program requires %08x", nVersion, FMOD_VERSION );
		MessageBox( NULL, szStr, "Error", MB_OK );
		return false;
	}
	int nNumDriver;
	int nNumChannel = 0;
	Result = m_pSystem->getNumDrivers( &nNumDriver );
	if( nNumDriver == 0 ) {
		FMOD_API_CHECK( m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND ) );
	}
	else {

		// 스피커 모드 셋팅
		FMOD_SPEAKERMODE SpeakerMode;
		FMOD_API_CHECK( m_pSystem->getDriverCaps( 0, NULL, NULL, NULL, &SpeakerMode ));
		Result = m_pSystem->setSpeakerMode( SpeakerMode );
		if( Result != FMOD_OK ) {
			FMOD_API_CHECK( m_pSystem->setSpeakerMode( FMOD_SPEAKERMODE_STEREO ));

		}
		FMOD_OUTPUTTYPE OutputType = FMOD_OUTPUTTYPE_AUTODETECT;
		switch( Type ) {
			case AutoDetected: OutputType = FMOD_OUTPUTTYPE_AUTODETECT; break;
			case DirectSound: OutputType = FMOD_OUTPUTTYPE_DSOUND; break;
			case WindowsMM: OutputType = FMOD_OUTPUTTYPE_WINMM; break;
			case OpenAL: OutputType = FMOD_OUTPUTTYPE_OPENAL; break;
			case WASAPI: OutputType = FMOD_OUTPUTTYPE_WASAPI; break;
		}
		Result = m_pSystem->setOutput( OutputType );
		if( Result != FMOD_OK ) {
			FMOD_API_CHECK( m_pSystem->setOutput( FMOD_OUTPUTTYPE_AUTODETECT ));
		}

		// 체널 셋팅
		int n2DChannel, n3DChannel, nTotalChannel;
		Result = m_pSystem->getHardwareChannels( &n2DChannel, &n3DChannel, &nTotalChannel );
		Result = m_pSystem->setHardwareChannels( n2DChannel, n2DChannel, n3DChannel, n3DChannel );
		Result = m_pSystem->setSoftwareChannels( 64 );
		nNumChannel = n2DChannel + 64;
	}
	
	Result = m_pSystem->init( nNumChannel, FMOD_INIT_NORMAL, NULL );
	if( Type == WASAPI ) {		
		if( Result == FMOD_ERR_OUTPUT_INIT ) {		
			FMOD_API_CHECK( m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND ) );
			Result = m_pSystem->init( nNumChannel, FMOD_INIT_NORMAL, NULL );	// 이런 상황이 안나오게 하는법이 없을지..?
		}
		else if( Result != FMOD_OK ) {
			FMOD_API_CHECK( Result );
			FMOD_SPEAKERMODE SpeakerMode;
			FMOD_API_CHECK( m_pSystem->getDriverCaps( 0, NULL, NULL, NULL, &SpeakerMode ));
			Result = m_pSystem->setSpeakerMode( SpeakerMode );
			Result = m_pSystem->init( nNumChannel, FMOD_INIT_WASAPI_EXCLUSIVE, NULL );		// WASAPI 실패시엔 독점모드로 시도해본다. (외부 사운드 중지됨)
		}
	}
	
	FMOD_API_CHECK( Result );

	if( Result != FMOD_OK ) {	// 그래도 안되는 경우엔 NoSound로 해준다. 종료할때 크래쉬 나는거 방지.
		m_pSystem->setOutput( FMOD_OUTPUTTYPE_NOSOUND );
		Result = m_pSystem->init( nNumChannel, FMOD_INIT_NORMAL, NULL );
		return false;
	}

	// 3D Setting
	FMOD_API_CHECK( m_pSystem->set3DSettings( 1.f, 100.f, 1.f ));

	CreateChannelGroup( "NULL" );

	return true;
}

bool CEtSoundEngine::ReInitialize()
{
	Finalize();
	return Initialize();
}

void CEtSoundEngine::LockFMODFunc()
{
	s_FMODFuncLock.Lock();
	s_bFMODFuncLock = true;
}

void CEtSoundEngine::UnLockFMODFunc()
{
	s_FMODFuncLock.UnLock();
	s_bFMODFuncLock = false;
}

void CEtSoundEngine::UnLockExceptionFMODFunc()
{
	if( s_bFMODFuncLock ) UnLockFMODFunc();
}

void CEtSoundEngine::LockChannel()
{
	s_ChannelLock.Lock();
	s_bChannelLock = true;
}

void CEtSoundEngine::UnLockChannel()
{
	s_ChannelLock.UnLock();
	s_bChannelLock = false;
}

void CEtSoundEngine::UnLockExceptionChannel()
{
	if( s_bChannelLock ) UnLockChannel();
}

bool CEtSoundEngine::_Finalize()
{
	if( m_pSystem == NULL ) return true;

	ScopeLock<CSyncLock> Lock(m_LoadLock);
	LockChannel();

	m_dwMainThreadID = 0;
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		m_pVecChannelGroupList[i]->RemoveAllChannel();
	}
	Process(0.f);

	SAFE_DELETE_PVEC( m_pVecChannelGroupList );

	for( DWORD i=0; i<m_pVecSoundList.size(); i++ ) {
		if( m_pVecSoundList[i]->szFileName.empty() ) continue;
		SAFE_DELETE( m_pVecSoundList[i] );
		m_pVecSoundList.erase( m_pVecSoundList.begin() + i );
		i--;
	}

	LockFMODFunc();
	FMOD_API_CHECK( m_pSystem->close() );
	FMOD_API_CHECK( m_pSystem->release() );
	UnLockFMODFunc();
	m_pSystem = NULL;

	UnLockChannel();
	return true;
}
bool CEtSoundEngine::Finalize()
{
	__try {
		_Finalize();
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
		return false;
	}
	return true;
}

void CEtSoundEngine::Process( float fDelta )
{
	__try {
		_Process( fDelta );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

void CEtSoundEngine::_Process( float fDelta )
{
	ScopeLock<CSyncLock> Lock(m_LoadLock);
	if ( !m_bEnable ) return; 
	if( !m_pSystem ) return;

	s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pSystem->update());
	s_FMODFuncLock.UnLock();

	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		m_pVecChannelGroupList[i]->Process( fDelta );
	}
	ProcessRemoveSoundQueue();
}

int CEtSoundEngine::CreateChannelGroup( const char *szChannelName, int nChannelIndex )
{
	LockChannel();
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szChannelName ) == NULL ) {
			UnLockChannel();
			return -1;
		}
		if( m_pVecChannelGroupList[i]->GetIndex() == nChannelIndex ) {
			UnLockChannel();
			return -1;
		}
	}
	int nRefCount = 0;
	while(1) {
		bool bExistChannel = false;
		for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
			if( m_pVecChannelGroupList[i]->GetIndex() == nRefCount ) {
				bExistChannel = true;
				break;
			}
		}
		if( bExistChannel == false ) break;

		nRefCount++;
		continue;
	}

	if( nChannelIndex == -1 ) nChannelIndex = nRefCount;

	CEtSoundChannelGroup *pChannel = new CEtSoundChannelGroup( this, szChannelName, nChannelIndex );

	m_pVecChannelGroupList.push_back( pChannel );

	UnLockChannel();
	return nChannelIndex;
}

void CEtSoundEngine::RemoveChannelGroup( const char *szChannelName )
{
	ScopeLock<CSyncLock> Lock(s_ChannelLock);
	CEtSoundChannelGroup *pChannel = NULL;
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szChannelName ) == NULL ) {
			SAFE_DELETE( m_pVecChannelGroupList[i] );
			m_pVecChannelGroupList.erase( m_pVecChannelGroupList.begin() + i );
//			pChannel = m_pVecChannelGroupList[i];
			break;
		}
	}
//	if( pChannel == NULL ) return;
}

CEtSoundChannelGroup *CEtSoundEngine::GetChannelGroup( const char *szChannelName )
{
	ScopeLock<CSyncLock> Lock(s_ChannelLock);
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szChannelName ) == NULL ) return m_pVecChannelGroupList[i];
	}
	return NULL;
}

CEtSoundChannelGroup *CEtSoundEngine::GetChannelGroup( int nChannelIndex )
{
	ScopeLock<CSyncLock> Lock(s_ChannelLock);
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		if( m_pVecChannelGroupList[i]->GetIndex() == nChannelIndex ) return m_pVecChannelGroupList[i];
	}
	return NULL;
}

int CEtSoundEngine::LoadSound( const char *szFileName, bool b3DSound, bool bStream )
{
	__try {
		return _LoadSound( szFileName, b3DSound, bStream );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
		return -1;
	}
}
int CEtSoundEngine::_LoadSound( const char *szFileName, bool b3DSound, bool bStream )
{
	if( !szFileName ) return -1;
	m_LoadLock.Lock();
	std::pair< MapSearchIter, MapSearchIter > range =  m_szMapSearch.equal_range( szFileName );
	for ( MapSearchIter itSearch = range.first; itSearch != range.second; ++itSearch ) {
		if( itSearch->second->b3D == b3DSound ) {
			itSearch->second->nRefCount++;
			m_LoadLock.UnLock();
			return itSearch->second->nIndex;
		}
	}
	m_LoadLock.UnLock();

	bool bValid = false;
	CResMngStream Stream( szFileName );
	if( Stream.IsValid() == false ) return -1;

	SoundStruct *pStruct = new SoundStruct;
	pStruct->szFileName = szFileName;
	m_LoadLock.Lock();
	pStruct->nIndex = m_nSoundRefCount++;
	m_LoadLock.UnLock();
	pStruct->nRefCount = 1;
	pStruct->b3D = b3DSound;
	pStruct->nSize = Stream.Size();

	pStruct->pData = new char[Stream.Size()];
	Stream.Read( pStruct->pData, Stream.Size() );

	FMOD_RESULT Result;
	FMOD_MODE Mode;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	if( b3DSound ) Mode = FMOD_3D | FMOD_OPENMEMORY | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_SOFTWARE;
	else Mode = FMOD_DEFAULT | FMOD_OPENMEMORY | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_SOFTWARE;
#else
	if( b3DSound ) Mode = FMOD_3D | FMOD_OPENMEMORY;
	else Mode = FMOD_DEFAULT | FMOD_OPENMEMORY;
#endif

	FMOD_CREATESOUNDEXINFO Info;
	memset( &Info, 0, sizeof(FMOD_CREATESOUNDEXINFO) );
	Info.length = Stream.Size();
	Info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

	// 이부분 락이 필요없어보이지만 FMOD 쪽 thread 검증이 안된관계로 일단 둡니다. ( 여기가 가장 오래 걸립니다. 빼면좋을듯. )
	if( bStream ) {
		s_FMODFuncLock.Lock();
		Result = m_pSystem->createStream( pStruct->pData, Mode, &Info, &pStruct->pSound );
		s_FMODFuncLock.UnLock();
	}
	else {
		s_FMODFuncLock.Lock();
		Result = m_pSystem->createSound( pStruct->pData, Mode, &Info, &pStruct->pSound );
		s_FMODFuncLock.UnLock();
		SAFE_DELETEA( pStruct->pData );
	}

	if( Result != FMOD_OK ) {
		SAFE_DELETE( pStruct );
		return -1;
	}
	pStruct->pSound->getFormat( &pStruct->Type, &pStruct->Format, &pStruct->nChannel, &pStruct->nBit );
	m_nTotalSoundSize += pStruct->nSize;
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	if( pStruct->Type == FMOD_SOUND_TYPE_MPEG ) {
		s_FMODFuncLock.Lock();
		pStruct->pSound->getMode( &Mode );
		Mode &= ~( FMOD_CREATECOMPRESSEDSAMPLE | FMOD_SOFTWARE );
		Result = pStruct->pSound->setMode( Mode );
		s_FMODFuncLock.UnLock();
	}
#endif

	m_LoadLock.Lock();
	m_pVecSoundList.push_back( pStruct );
	m_nMapSearch.insert( make_pair( pStruct->nIndex, pStruct ) );
	m_szMapSearch.insert( make_pair( pStruct->szFileName, pStruct ) );
	m_LoadLock.UnLock();

	return pStruct->nIndex;

}

void CEtSoundEngine::RemoveSound( int nSoundIndex )
{
	__try {
		return _RemoveSound( nSoundIndex );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

void CEtSoundEngine::_RemoveSound( int nSoundIndex )
{
	ScopeLock<CSyncLock> Lock(m_LoadLock);

	std::map<int, SoundStruct *>::iterator it = m_nMapSearch.find( nSoundIndex );
	if( it != m_nMapSearch.end() ) {
		it->second->nRefCount--;
		if( it->second->nRefCount <= 0 ) {
			if( m_dwMainThreadID != GetCurrentThreadId() ) {
				if( std::find( m_nVecRemoveSoundQueue.begin(), m_nVecRemoveSoundQueue.end(), nSoundIndex ) == m_nVecRemoveSoundQueue.end() ) {
					m_nVecRemoveSoundQueue.push_back( nSoundIndex );
				}
				return;
			}

			std::pair<  MapSearchIter, MapSearchIter > range =  m_szMapSearch.equal_range( it->second->szFileName );
			for ( MapSearchIter itSearch = range.first; itSearch != range.second; ++itSearch ) {
				if( itSearch->second == it->second ) {
					m_szMapSearch.erase( itSearch );
					break;
				}
			}
			m_nMapSearch.erase( it );

			for( DWORD i=0; i<m_pVecSoundList.size(); i++ ) {
				if( m_pVecSoundList[i]->nIndex == nSoundIndex ) {
					m_pVecSoundList[i]->nRefCount--;
					if( m_pVecSoundList[i]->nRefCount <= 0 ) {
						SoundStruct *pStruct = m_pVecSoundList[i];
						m_nTotalSoundSize -= pStruct->nSize;
						SAFE_DELETE( pStruct );
						m_pVecSoundList.erase( m_pVecSoundList.begin() + i );
					}
					return;
				}
			}
			for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
				CEtSoundChannelGroup *pGroup = m_pVecChannelGroupList[i];
				pGroup->CheckSoundIndexAndRemoveChannel( nSoundIndex );
			}
		}
	}
}

void CEtSoundEngine::ProcessRemoveSoundQueue()
{
	for( DWORD i=0; i<m_nVecRemoveSoundQueue.size(); i++ ) {
		SoundStruct *pStruct = FindSoundStruct( m_nVecRemoveSoundQueue[i] );
		if( !pStruct ) continue;
		if( pStruct->nRefCount > 0 ) continue;
		RemoveSound( m_nVecRemoveSoundQueue[i] );
	}
	m_nVecRemoveSoundQueue.clear();
}

void CEtSoundEngine::GetCPUUsage( float &fDSP, float &fStream, float &fUpdate, float &fTotal )
{
	ScopeLock<CSyncLock> Lock(s_FMODFuncLock);
	FMOD_API_CHECK( m_pSystem->getCPUUsage( &fDSP, &fStream, NULL, &fUpdate, &fTotal ));
}

int CEtSoundEngine::GetUsingChannelCount()
{
	ScopeLock<CSyncLock> Lock(s_FMODFuncLock);
	int nCount = 0;
	m_pSystem->getChannelsPlaying( &nCount );
	return nCount;
}

EtSoundChannelHandle CEtSoundEngine::PlaySound__( const char *szGroupName, int nSoundIndex, bool bLoop, bool bPause, bool b3DSound )
{
	return PlaySound(szGroupName, nSoundIndex, bLoop, bPause, b3DSound);
}

EtSoundChannelHandle CEtSoundEngine::PlaySound( const char *szGroupName, int nSoundIndex, bool bLoop, bool bPause, bool b3DSound )
{
	if( m_dwMainThreadID != GetCurrentThreadId() ) return CEtSoundChannel::Identity();
	if ( !m_bEnable )
	{
		return CEtSoundChannel::Identity();
	}

	CEtSoundChannelGroup *pGroup = NULL;
	if( szGroupName == NULL ) pGroup = GetChannelGroup( "NULL" );
	else pGroup = GetChannelGroup( szGroupName );
	if( pGroup == NULL ) return CEtSoundChannel::Identity();
	SoundStruct *pStruct = FindSoundStruct( nSoundIndex );
	if( pStruct == NULL ) return CEtSoundChannel::Identity();
	if( pStruct->pSound == NULL ) return CEtSoundChannel::Identity();
	if( pStruct->szFileName.empty() ) return CEtSoundChannel::Identity();
	FMOD::Channel *pChannel;

	FMOD_RESULT Result;
	s_FMODFuncLock.Lock();
	Result = m_pSystem->playSound( FMOD_CHANNEL_FREE, pStruct->pSound, bPause, &pChannel );
	s_FMODFuncLock.UnLock();
	if( Result != FMOD_OK ) return CEtSoundChannel::Identity();

	s_FMODFuncLock.Lock();
	FMOD_API_CHECK( pChannel->setChannelGroup( pGroup->GetGroup() ));
	s_FMODFuncLock.UnLock();

	FMOD_MODE Mode = 0;
	if( bLoop ) Mode |= FMOD_LOOP_NORMAL;
	else Mode |= FMOD_LOOP_OFF;

	if (pStruct->b3D && b3DSound)
		Mode |= FMOD_3D;
	else
		Mode |= FMOD_2D;

	s_FMODFuncLock.Lock();
	FMOD_API_CHECK( pChannel->setMode( Mode ));
	FMOD_API_CHECK( pChannel->setLoopCount( ( bLoop == true ) ? -1 : 0 ));
	// SetVolume 을 해주지 않으면 ChannelGroup 에 먹혀있는 Volume값이 적용되지 않는다.
	float fVolume = pGroup->GetVolume();
	FMOD_API_CHECK( pChannel->setVolume( fVolume ));
	if( bLoop ) {
		pChannel->setPriority(0);
	}
	s_FMODFuncLock.UnLock();

	ScopeLock<CSyncLock> Lock2(s_ChannelLock);
	return pGroup->InsertChannel( nSoundIndex, pChannel );
}

CEtSoundEngine::SoundStruct *CEtSoundEngine::FindSoundStruct( const char *szFileName )
{
	if( !szFileName ) return NULL;
	ScopeLock<CSyncLock> Lock(m_LoadLock);
	std::multimap<std::string, SoundStruct *>::iterator it = m_szMapSearch.find( szFileName );
	if( it != m_szMapSearch.end() ) {
		return it->second;
	}
	return NULL;
}

CEtSoundEngine::SoundStruct *CEtSoundEngine::FindSoundStruct( int nIndex )
{
	ScopeLock<CSyncLock> Lock(m_LoadLock);
	std::map<int, SoundStruct *>::iterator it = m_nMapSearch.find( nIndex );
	if( it != m_nMapSearch.end() ) {
		return it->second;
	}
	return NULL;
}

void CEtSoundEngine::RemoveChannel( EtSoundChannelHandle hChannel )
{
	ScopeLock<CSyncLock> Lock(s_ChannelLock);
	if( hChannel )
		hChannel->GetGroup()->RemoveChannel( hChannel->GetIndex() );
}

void CEtSoundEngine::AddSoundRef( int nIndex )
{
	ScopeLock<CSyncLock> Lock(m_LoadLock);
	SoundStruct *pStruct = FindSoundStruct( nIndex );
	if( pStruct ) pStruct->nRefCount++;
}
void CEtSoundEngine::SetListener( EtVector3 &vPos, EtVector3 &vLook, EtVector3 &vUp )
{
	__try {
		return _SetListener( vPos, vLook, vUp );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

EtVector3 CEtSoundEngine::GetListenerPos()
{
	__try {
		return _GetListenerPos();
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
	return EtVector3( 0.f, 0.f, 0.f );
}

void CEtSoundEngine::_SetListener( EtVector3 &vPos, EtVector3 &vLook, EtVector3 &vUp )
{
	if( !m_pSystem ) return;
	FMOD_VECTOR vPosF = { vPos.x, vPos.y, vPos.z };
	FMOD_VECTOR vLookF = { vLook.x, vLook.y, vLook.z };
	FMOD_VECTOR vUpF = { vUp.x, vUp.y, vUp.z };
//	s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pSystem->set3DListenerAttributes( 0, &vPosF, NULL, &vLookF, &vUpF ));
//	s_FMODFuncLock.UnLock();
}

EtVector3 CEtSoundEngine::_GetListenerPos()
{
	if( !m_pSystem ) return EtVector3( 0.f, 0.f, 0.f );

	FMOD_VECTOR vPos;
//	s_FMODFuncLock.Lock();
	FMOD_API_CHECK( m_pSystem->get3DListenerAttributes( 0, &vPos, NULL, NULL, NULL ));
//	s_FMODFuncLock.UnLock();

	return EtVector3( vPos.x, vPos.y, vPos.z );
}

void CEtSoundEngine::FadeVolume( const char *szGroupName, float fVolume, float fDelta, bool bPauseResume )
{
	__try {
		return _FadeVolume( szGroupName, fVolume, fDelta, bPauseResume );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}
void CEtSoundEngine::SetVolume( const char *szGroupName, float fVolume )
{
	__try {
		return _SetVolume( szGroupName, fVolume );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

void CEtSoundEngine::_FadeVolume( const char *szGroupName, float fVolume, float fDelta, bool bPauseResume )
{
	LockChannel();
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) {
		if( szGroupName && strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) continue;
		m_pVecChannelGroupList[i]->SetVolume( fVolume, fDelta );
		if( bPauseResume == true ) m_pVecChannelGroupList[i]->Pause();
		else m_pVecChannelGroupList[i]->Resume();
	}
	UnLockChannel();
}

void CEtSoundEngine::_SetVolume( const char *szGroupName, float fVolume )
{
	LockChannel();
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) 
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) 
			continue;

		m_pVecChannelGroupList[i]->SetVolume( fVolume );
		break;
	}
	UnLockChannel();
}

float CEtSoundEngine::GetVolume( const char *szGroupName )
{
	LockChannel();
	float fValue = 0.f;
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ )
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL )
			continue;

		fValue = m_pVecChannelGroupList[i]->GetVolume();
	}

	UnLockChannel();
	return fValue;
}


void CEtSoundEngine::SetMute( const char *szGroupName, bool bMute )
{
	__try {
		return _SetMute( szGroupName, bMute );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

bool CEtSoundEngine::IsMute( const char *szGroupName )
{
	__try {
		return _IsMute( szGroupName );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
	return false;
}

void CEtSoundEngine::_SetMute( const char *szGroupName, bool bMute )
{
	LockChannel();
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) 
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) 
			continue;

		m_pVecChannelGroupList[i]->SetMute( bMute );
		break;
	}
	UnLockChannel();
}

bool CEtSoundEngine::_IsMute( const char *szGroupName )
{
	LockChannel();
	bool bResult = false;
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) 
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) 
			continue;

		bResult = m_pVecChannelGroupList[i]->IsMute();
	}
	UnLockChannel();

	return bResult;
}

void CEtSoundEngine::SetMasterVolume( const char *szGroupName, float fVolume )
{
	__try {
		return _SetMasterVolume( szGroupName, fVolume );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
}

float CEtSoundEngine::GetMasterVolume( const char *szGroupName )
{
	__try {
		return _GetMasterVolume( szGroupName );
	}
	__except(EXCEPTION_EXECUTE_HANDLER) {
		UnLockExceptionFMODFunc();
	}
	return 0.f;
}

void CEtSoundEngine::_SetMasterVolume( const char *szGroupName, float fVolume )
{
	LockChannel();
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) 
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) 
			continue;

		m_pVecChannelGroupList[i]->SetMasterVolume( fVolume * m_fMaxVolumeRate );
		break;
	}
	UnLockChannel();
}

float CEtSoundEngine::_GetMasterVolume( const char *szGroupName )
{
	LockChannel();
	float fValue = 0.f;
	for( DWORD i=0; i<m_pVecChannelGroupList.size(); i++ ) 
	{
		if( strcmp( m_pVecChannelGroupList[i]->GetName(), szGroupName ) != NULL ) 
			continue;

		fValue = m_pVecChannelGroupList[i]->GetMasterVolume() / m_fMaxVolumeRate;
	}
	UnLockChannel();

	return fValue;
}

int CEtSoundEngine::GetSoundTypeCount( FMOD_SOUND_TYPE Type )
{
	int nCount = 0;
	for( DWORD i=0; i<m_pVecSoundList.size(); i++ ) {
		if( m_pVecSoundList[i]->Type == Type ) nCount++;
	}
	return nCount;
}

int CEtSoundEngine::GetSoundFormatCount( FMOD_SOUND_FORMAT Format )
{
	int nCount = 0;
	for( DWORD i=0; i<m_pVecSoundList.size(); i++ ) {
		if( m_pVecSoundList[i]->Format == Format ) nCount++;
	}
	return nCount;
}

CEtSoundEngine::SoundStruct *CEtSoundEngine::GetSoundStruct( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_pVecSoundList.size() ) return NULL;

	return m_pVecSoundList[nIndex];
}