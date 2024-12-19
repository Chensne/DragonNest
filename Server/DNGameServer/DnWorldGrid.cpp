#include "StdAfx.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"

CDnWorldGrid::CDnWorldGrid( CEtWorld *pWorld )
: CEtWorldGrid( pWorld )
{
	m_fWorldPosConvX = 0.f;
	m_fWorldPosConvZ = 0.f;
}

CDnWorldGrid::~CDnWorldGrid()
{
}

CEtWorldSector *CDnWorldGrid::AllocSector()
{
	return new CDnWorldSector;
}

bool CDnWorldGrid::Initialize( const char *szName, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwTileSize )
{
	bool bResult = CEtWorldGrid::Initialize( szName, dwX, dwY, dwWidth, dwHeight, dwTileSize );
	if( !bResult ) return false;


	m_fWorldPosConvX = ( GetGridWidth() * 100.f ) / 2.f;
	m_fWorldPosConvZ = ( GetGridHeight() * 100.f ) / 2.f;

	return true;
}


CEtWorldSector *CDnWorldGrid::GetSector( float fX, float fZ, float *fResultX, float *fResultZ )
{
	if( fResultX ) *fResultX = fX + m_fWorldPosConvX;
	if( fResultZ ) *fResultZ = fZ + m_fWorldPosConvZ;
	return m_ppSector[0][0];
}