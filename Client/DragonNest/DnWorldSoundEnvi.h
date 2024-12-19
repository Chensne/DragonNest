#pragma once

#include "EtWorldSoundEnvi.h"
#include "EtSoundChannel.h"

class CEtWorldSound;
class CDnWorldSoundEnvi : public CEtWorldSoundEnvi
{
public:
	CDnWorldSoundEnvi( CEtWorldSound *pSound );
	virtual ~CDnWorldSoundEnvi();

protected:
	int m_nSoundIndex;
	EtSoundChannelHandle m_hChannel;

public:
	virtual bool Load( CStream *pStream );
	virtual void Play();
	virtual void Stop();
};