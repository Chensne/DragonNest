#include "StdAfx.h"
#include "DnUISound.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnUISound::CDnUISound()
{
}

CDnUISound::~CDnUISound()
{
}

void CDnUISound::Initialize()
{
	CEtUISound::Initialize();

	m_uiSoundIndex[UI_WINDOW_OPEN] = CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10019 ), false, false );
	m_uiSoundIndex[UI_WINDOW_CLOSE] = CEtSoundEngine::GetInstance().LoadSound( CDnTableDB::GetInstance().GetFileName( 10020 ), false, false );
}