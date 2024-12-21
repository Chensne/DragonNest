#include "StdAfx.h"
#include "EtUISound.h"
#include "EtSoundEngine.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtUISound::CEtUISound(void)
{
	for( int i=0; i<UI_SOUND_MAX; i++ ) {
		m_uiSoundIndex[i] = -1;
	}
}

CEtUISound::~CEtUISound(void)
{
}

void CEtUISound::Initialize()
{
}

void CEtUISound::Finalize()
{
	for( int i=0; i<UI_SOUND_MAX; i++ ) {
		if( m_uiSoundIndex[i] == -1 ) continue;
		CEtSoundEngine::GetInstance().RemoveSound( m_uiSoundIndex[i] );
	}
}

void CEtUISound::Play( emUI_SOUND uiSound )
{
	if( uiSound == UI_SOUND_NONE )
		return;
	if( m_uiSoundIndex[uiSound] == -1 ) return;

	ASSERT((uiSound<UI_SOUND_MAX)&&"CEtUISound::Play");
	CEtSoundEngine::GetInstance().PlaySound( "2D", m_uiSoundIndex[uiSound] );
}