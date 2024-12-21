#include "StdAfx.h"
#include "EtWorldSoundEnvi.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtWorldSoundEnvi::CEtWorldSoundEnvi( CEtWorldSound *pSound )
: m_vPosition( 0.f, 0.f, 0.f )
{
	m_pSound = pSound;
	m_fRange = 0.f;
	m_fVolume = 1.f;
	m_fRollOff = 0.8f;
	m_bStream = false;
}

CEtWorldSoundEnvi::~CEtWorldSoundEnvi()
{
}

bool CEtWorldSoundEnvi::Load( CStream *pStream )
{
	ReadStdString( m_szName, pStream );
	ReadStdString( m_szFileName, pStream );
	pStream->Read( &m_vPosition, sizeof(EtVector3) );
	pStream->Read( &m_fRange, sizeof(float) );
	pStream->Read( &m_fRollOff, sizeof(float) );
	pStream->Read( &m_fVolume, sizeof(float) );
	pStream->Read( &m_bStream, sizeof(bool) );
	return true;
}