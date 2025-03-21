#pragma once

#include "EtTerrain.h"

#define TERRAIN_BLEND_THRESH_HOLD	0.7f

class CEtCliffTerrain : public CEtTerrain
{
public:
	CEtCliffTerrain(void);
	virtual ~CEtCliffTerrain(void);

protected:

public:
	virtual void GenerateTexureCoord();
	DWORD CalcBlendWeight( EtVector3 *pNormal );
	float CalcAddOffsetX( EtVector3 *pFaceNormal );
	float CalcAddOffsetZ( EtVector3 *pFaceNormal );
};


class CEtDetailCliffTerrain : public CEtCliffTerrain
{
public:
	CEtDetailCliffTerrain(void);
	virtual ~CEtDetailCliffTerrain(void);

protected:
	void CreateMaterial( const char *pEffectName );

public:

};

class CEtLowDetailTerrain : public CEtTerrain
{
public:
	CEtLowDetailTerrain(void);
	virtual ~CEtLowDetailTerrain(void);

protected:
	EtTextureHandle m_hEntireMap;

public:
	void Clear();
	void InitializeTerrain();
	void BakeLowDetailMap();

};

class CEtTerrainOpti : public CEtCliffTerrain
{
public:
	CEtTerrainOpti(void);
	virtual ~CEtTerrainOpti(void);

protected:
	bool m_bProcessLayer;
	int m_nLayerConvertTable[ 4 ];

public:
	virtual void CheckLayerCount();
	virtual bool SetTexture( int nTexIndex, const char *pTexName );
	virtual void GenerateTexureCoord();

};


class CEtLowSplatTerrain : public CEtCliffTerrain
{
public:
	CEtLowSplatTerrain( void );
	virtual ~CEtLowSplatTerrain( void );

protected:

public:
	virtual void CreateMaterial( const char* pEffectName );
	virtual bool SetTexture( int nTexIndex, const char *pTexName );
	virtual void SetTextureDistance( int nTexLayer, float fDistance );

	virtual void GenerateVertexBuffer( void );
};
