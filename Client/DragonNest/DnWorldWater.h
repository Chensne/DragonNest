#pragma once

#include "EtWorldWater.h"

class CEtWorldSector;
class CDnWorldWater : public CEtWorldWater {
public:
	CDnWorldWater( CEtWorldSector *pSector );
	virtual ~CDnWorldWater();

protected:
	EtTextureHandle m_hAlphaTexture;
	CEtMeshStream *m_pMesh;
	EtMaterialHandle m_hMaterial;
	std::vector< SCustomParam > m_VecCustomParam;
	std::map<std::string, EtTextureHandle> m_MapUsingTexture;
	int		m_nRenderUniqueID;
	int			m_nScale;

protected:
	void GenerationVertex();
	void GenerationIndex();
	void GenerationNormal();
	void GenerationTexCoord();
	void GenerationShader();
	void GenerationTexture();
	void CreateWater();

	void RefreshWaterInfo();
public:
	virtual bool Load( const char *szFileName );
	virtual CEtWorldWaterRiver *AllocRiver();

	virtual void Render( LOCAL_TIME LocalTime );

	int GetWaterIndex( void *pWater );
};