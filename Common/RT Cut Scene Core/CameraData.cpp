#include "StdAfx.h"
#include "CameraData.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CCameraData::CCameraData(void)
{
}

CCameraData::~CCameraData(void)
{
}

void CCameraData::LoadResource( CStream *pStream )
{
	pStream->Read( &m_Header, sizeof( SCameraFileHeader ) );
	pStream->Seek( CAMERA_HEADER_RESERVED, SEEK_CUR );
	m_vecFOV.resize( m_Header.nFOVKeyCount );
	m_vecPosition.resize( m_Header.nPositionKeyCount );
	m_vecRotation.resize( m_Header.nRotationKeyCount );
	if( m_Header.nFOVKeyCount )
	{
		pStream->Read( &m_vecFOV[ 0 ], m_Header.nFOVKeyCount * sizeof( SCameraFOVKey ) );
	}
	if( m_Header.nPositionKeyCount )
	{
		pStream->Read( &m_vecPosition[ 0 ], m_Header.nPositionKeyCount * sizeof( SCameraPositionKey ) );
	}
	if( m_Header.nRotationKeyCount )
	{
		pStream->Read( &m_vecRotation[ 0 ], m_Header.nRotationKeyCount * sizeof( SCameraRotationKey ) );
	}
}


void CCameraData::Clear( void )
{
	ZeroMemory( &m_Header, sizeof(SCameraFileHeader) );
	m_vecFOV.clear();
	m_vecPosition.clear();
	m_vecRotation.clear();
}
