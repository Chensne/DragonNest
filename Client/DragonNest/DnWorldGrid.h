#pragma once

#include "DnWorld.h"
#include "EtWorldGrid.h"

class CEtWorld;
class CDnWorldGrid : public CEtWorldGrid
{
public:
	CDnWorldGrid( CEtWorld *pWorld );
	virtual ~CDnWorldGrid();

protected:
	float m_fWorldPosConvX;
	float m_fWorldPosConvZ;


protected:
	virtual CEtWorldSector *AllocSector();

public:
	TileTypeEnum GetTileType( float fX, float fZ );

	void RefreshQualityLevel( int nLevel );

	virtual bool Initialize( const char *szName, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwTileSize );
	virtual CEtWorldSector *GetSector( float fX, float fZ, float *fResultX = NULL, float *fResultZ = NULL );
};