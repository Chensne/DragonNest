#pragma once

#include "fmod.hpp"
#include "Timer.h"
#include "Singleton.h"
#include "EtSoundChannel.h"

class CEtSoundChannelGroup;
class CEtSoundChannel;
class CEtSoundFile;
class CEtSoundEngine : public CSingleton<CEtSoundEngine> {
public:
	CEtSoundEngine();
	virtual ~CEtSoundEngine();

	struct SoundStruct {
		FMOD::Sound *pSound;
		std::string szFileName;
		bool b3D;
		int nIndex;
		int nRefCount;
		int nSize;

		char *pData;

		FMOD_SOUND_TYPE Type;
		FMOD_SOUND_FORMAT Format;
		int nChannel;
		int nBit;
		SoundStruct() {
			pSound = NULL;
			b3D = false;
			nIndex = -1;
			nRefCount = 0;
			pData = NULL;
			Type = FMOD_SOUND_TYPE_UNKNOWN;
			Format = FMOD_SOUND_FORMAT_NONE;
			nBit = 0;
			nChannel = 0;
			nSize = 0;
		}
		~SoundStruct() {
			if( pSound ) {
				s_FMODFuncLock.Lock();
				FMOD_API_CHECK( pSound->release());
				s_FMODFuncLock.UnLock();
			}
			SAFE_DELETEA( pData );
		}
	};
	struct ChannelStruct {
		int nIndex;
		FMOD::Channel *pChannel;
	};

	enum OutputTypeEnum {
		AutoDetected = 0,
		DirectSound,
		WindowsMM,
		OpenAL,
		WASAPI,			// Vista 이상에서 사용 권장
	};

	static CEtSoundEngine *s_pCurrentCallbackPtr;
	CSyncLock m_LoadLock;
	static CSyncLock s_ChannelLock;
	static bool s_bChannelLock;
	static CSyncLock s_FMODFuncLock;
	static bool s_bFMODFuncLock;

	int m_nTotalSoundSize;
	
	typedef std::multimap< std::string, SoundStruct *>::iterator MapSearchIter;

protected:
	FMOD::System *m_pSystem;
	std::vector<CEtSoundChannelGroup *> m_pVecChannelGroupList;
	std::vector<SoundStruct *> m_pVecSoundList;
	std::map<int, SoundStruct *> m_nMapSearch;
	std::multimap<std::string, SoundStruct *> m_szMapSearch;
	int m_nSoundRefCount;
	bool m_bEnable;

	std::vector<int> m_nVecRemoveSoundQueue;
	DWORD m_dwMainThreadID;

	float m_fMaxVolumeRate;

protected:
	void ProcessRemoveSoundQueue();
	void LockFMODFunc();
	void UnLockFMODFunc();
	void UnLockExceptionFMODFunc();

	void LockChannel();
	void UnLockChannel();
	void UnLockExceptionChannel();

protected:
	bool _Finalize();
	void _Process( float fDelta );
	int _LoadSound( const char *szFileName, bool b3DSound, bool bStream );
	void _RemoveSound( int nSoundIndex );
	void _FadeVolume( const char *szGroupName, float fVolume, float fDelta, bool bPauseResume );
	void _SetVolume( const char *szGroupName, float fVolume );
	void _SetMute( const char *szGroupName, bool bMute );
	bool _IsMute( const char *szGroupName );
	void _SetMasterVolume( const char *szGroupName, float fVolume );
	float _GetMasterVolume( const char *szGroupName );
	void _SetListener( EtVector3 &vPos, EtVector3 &vLook, EtVector3 &vUp );
	EtVector3 _GetListenerPos();

public:
	bool Initialize( OutputTypeEnum Type = AutoDetected );
	bool ReInitialize();
	bool Finalize();

	void Process( float fDelta );

	FMOD::System *GetFMODSystem() { return m_pSystem; }

	int CreateChannelGroup( const char *szChannelName, int nChannelIndex = -1 );
	void RemoveChannelGroup( const char *szChannelName );
	CEtSoundChannelGroup *GetChannelGroup( const char *szChannelName );
	CEtSoundChannelGroup *GetChannelGroup( int nChannelIndex );
	DWORD GetChannelGroupCount() { return (DWORD)m_pVecChannelGroupList.size(); }

	int LoadSound( const char *szFileName, bool b3DSound, bool bStream );
	void RemoveSound( int nSoundIndex );
	EtSoundChannelHandle PlaySound( const char *szGroupName, int nSoundIndex, bool bLoop = false, bool bPause = false, bool b3DSound = true );
	// 하는일은 없음.. mmsystem 디파인 때문에..
	EtSoundChannelHandle PlaySound__( const char *szGroupName, int nSoundIndex, bool bLoop = false, bool bPause = false, bool b3DSound = true );

	void RemoveChannel( EtSoundChannelHandle hChannel );

	void AddSoundRef( int nIndex );

	void SetListener( EtVector3 &vPos, EtVector3 &vLook, EtVector3 &vUp );
	EtVector3 GetListenerPos();

	void GetCPUUsage( float &fDSP, float &fStream, float &fUpdate, float &fTotal );
	int GetUsingChannelCount();

	void FadeVolume( const char *szGroupName, float fVolume, float fDelta, bool bPauseResume );
	void SetEnable( bool bEnable ) { m_bEnable = bEnable; }

	void SetVolume( const char *szGroupName, float fVolume );
	float GetVolume( const char *szGroupName );
	void SetMute( const char *szGroupName, bool bMute );
	bool IsMute( const char *szGroupName );

	void SetMasterVolume( const char *szGroupName, float fVolume );
	float GetMasterVolume( const char *szGroupName );

	// Note 한기: 사운드 길이를 알아야 해서 protected 에서 public 으로 뺍니다. 
	SoundStruct *FindSoundStruct( const char *szFileName );
	SoundStruct *FindSoundStruct( int nIndex );
	SoundStruct *GetSoundStruct( int nIndex );
	int GetSoundStructCount() { return (int)m_pVecSoundList.size(); }

	int GetTotalSoundSize() { return m_nTotalSoundSize; }
	int GetSoundTypeCount( FMOD_SOUND_TYPE Type );
	int GetSoundFormatCount( FMOD_SOUND_FORMAT Format );
};
