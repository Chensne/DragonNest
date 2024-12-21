#pragma once

#include "fmod.hpp"
#include "EtSoundChannel.h"

class CEtSoundEngine;
class CEtSoundChannelGroup
{
public:
	CEtSoundChannelGroup( CEtSoundEngine *pEngine, const char *szName, int nIndex = -1 );
	~CEtSoundChannelGroup();

protected:
	std::vector<EtSoundChannelHandle> m_hVecChannelList;
	FMOD::ChannelGroup *m_pGroup;
	CEtSoundEngine *m_pEngine;
	std::string m_szName;
	int m_nIndex;
	int m_nChannelRefCount;
	float m_fTargetVolume;
	float m_fCurrentVolume;
	float m_fCurrentDelta;
	float m_fVolumeDelta;

	float m_fMasterVolume;

	CSyncLock m_InsertLock;
public:
	const char *GetName();
	int GetIndex();
	CEtSoundEngine *GetEngine() { return m_pEngine; }

	FMOD::ChannelGroup *GetGroup() { return m_pGroup; }
	EtSoundChannelHandle InsertChannel( int nSoundIndex, FMOD::Channel *pChannel );
	void RemoveChannel( int nChannelIndex );
	EtSoundChannelHandle GetChannel( int nChannelIndex );

	void SetMasterVolume( float fValue );
	float GetMasterVolume();

	void SetVolume( float fValue, float fDelta = 0.f );
	float GetVolume();

	void SetMute( bool bMute );
	bool IsMute();

	void Pause();
	void Resume();

	void Stop();
	void Play();

	void Process( float fDelta );
	static FMOD_RESULT F_CALLBACK ChannelStopCallbackFunc( FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2 );

	void CheckSoundIndexAndRemoveChannel( int nSoundIndex );

	void RemoveAllChannel();

};