#pragma once

#include "Timer.h"
#include "EtQuadtree.h"

class CEtWorldWaterRiver;
class CEtWorldSector;

class CEtWorldWater
{
public:
	CEtWorldWater( CEtWorldSector *pSector );
	virtual ~CEtWorldWater();

	struct CustomParamStruct : public TBoostMemoryPool< CustomParamStruct >
	{
		std::string szName;
		int nType;
		union {
			int nInt;
			float fFloat;
			float fFloat4[4];
			char *szTexName;
		};
		CustomParamStruct() : nType( 0 ) { szTexName = NULL; }
		~CustomParamStruct() {
			if( nType == EPT_TEX ) SAFE_DELETEA( szTexName );
		}
	};
protected:
	CEtWorldSector *m_pSector;
	std::vector<CEtWorldWaterRiver *> m_pVecRiver;
	BYTE *m_pAlphaTable;
	CEtQuadtree<CEtWorldWaterRiver *> *m_pRiverQuadtree;

	bool m_bEnable;
	float m_fGridSize;
	int m_nAlphaTableWidth;
	int m_nAlphaTableHeight;
	std::string m_szShaderName;
	EtVector2 m_vUVRatio;
	float m_fWaterLevel;

	int m_nBorderReduction;
	int m_nGaussianValue;
	float m_fGaussianEpsilon;
	int m_nMinAlpha;
	int m_nMaxAlpha;
	float m_fRevisionAlpha;

	std::vector<CustomParamStruct *> m_pVecCustomParamList;
	EtVector2 m_vOffset;

public:
	virtual bool Load( const char *szFileName );
	virtual CEtWorldWaterRiver *AllocRiver();

	virtual bool Initialize();

	virtual void Render( LOCAL_TIME LocalTime ) {}

	float GetWaterLevel() { return m_fWaterLevel; }

	DWORD GetRiverCount();
	CEtWorldWaterRiver *GetRiverFromIndex( DWORD dwIndex );
	CEtWorldWaterRiver *GetRiverFromName( const char *szName );
	CEtWorldSector *GetSector() { return m_pSector; }

	bool IsEnableWater() { return m_bEnable; }
	bool GetHeight( float fX, float fZ, float *pfHeight );
	bool Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );
	void ScanRiver( EtVector2 &vPos, float fRadius, std::vector<CEtWorldWaterRiver *> *pVecResult );

	EtVector2 *GetOffset() { return &m_vOffset; }
};