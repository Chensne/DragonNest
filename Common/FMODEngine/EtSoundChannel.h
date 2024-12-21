#pragma once

#include "fmod.hpp"
#include "SmartPtr.h"

#if defined(_DEBUG) || defined(_RDEBUG)
#define FMOD_API_CHECK( func )	do{ FMOD_RESULT result = func; if( result != FMOD_OK) { OutputDebug("Fmod Api Error %s = %d\n", #func, result ); ASSERT( false && "Fmod Error" ); } } while ( false )
#else
#define FMOD_API_CHECK( func ) func;
#endif 

class CEtSoundChannelGroup;

class CEtSoundChannel;
typedef CSmartPtr< CEtSoundChannel > EtSoundChannelHandle;

class CEtSoundChannel : public CSmartPtrBase< CEtSoundChannel >
{
public:
	CEtSoundChannel( int nIndex, int nSoundIndex, CEtSoundChannelGroup *pGroup );
	~CEtSoundChannel();

protected:
	int m_nIndex;
	int m_nSoundIndex;
	CEtSoundChannelGroup *m_pGroup;
	FMOD::Channel *m_pChannel;
	FMOD_VECTOR *m_pCustomRollOffArray;
	std::vector<FMOD::DSP *> m_pVecDSPList;
	int m_nSmartPtrIndex;

public:
	void SetChannel( FMOD::Channel *pChannel );
	FMOD::Channel *GetChannel() { return m_pChannel; }
	int GetIndex() { return m_nIndex; }
	int GetSoundIndex() { return m_nSoundIndex; }

	void SetCallback( FMOD_RESULT (_stdcall *Func)(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2) );
	void SetCallback( FMOD_RESULT (_stdcall *Func)(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2), void *pValue );

	CEtSoundChannelGroup *GetGroup() { return m_pGroup; }

	bool IsPlay();
	void Pause();
	void Resume();
	bool IsPause();
	void SetLoopCount( int nCount );
	int GetLoopCount();

	void SetPriority( int nValue );

	void SetVolume( float fValue );
	float GetVolume();

	void SetPosition( EtVector3 &vPos );
	EtVector3 GetPosition();
	void SetRollOff( int nCount, ... );

	bool AddDSP( int nIndex );
	void RemoveDSP( int nIndex );
	void SetDSPParameter( int nIndex, int nParamIndex, float fValue );
};