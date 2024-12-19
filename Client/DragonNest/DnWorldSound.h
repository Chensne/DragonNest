#pragma once

#include "EtWorldSound.h"
#include "EtSoundChannel.h"
class CEtWorldSector;
class CDnWorldSound : public CEtWorldSound
{
public:
	CDnWorldSound( CEtWorldSector *pSector );
	virtual ~CDnWorldSound();

protected:
	int m_nSoundIndex;
	EtSoundChannelHandle m_hChannel;

public:
	virtual bool Load( const char *szInfoFile );
	virtual void Play();
	virtual void Stop();
	virtual void SetVolume( float fValue );

	void ChangeBGM( const char *szFileName );

	void FadeVolumeBGM( const float fTargetVolume, const float fDelta );

	virtual CEtWorldSoundEnvi *AllocSoundEnvi();
};