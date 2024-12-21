#include "StdAfx.h"
#include "CameraData.h"

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
		pStream->Read( &m_vecPosition[ 0 ], m_Header.nPositionKeyCount * sizeof( SCameraPostionKey ) );
	}
	if( m_Header.nRotationKeyCount )
	{
		pStream->Read( &m_vecRotation[ 0 ], m_Header.nRotationKeyCount * sizeof( SCameraRotationKey ) );
	}
}
