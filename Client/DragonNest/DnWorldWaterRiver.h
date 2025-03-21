#pragma once
#include "EtWorldWaterRiver.h"

class CDnWorldWaterRiver : public CEtWorldWaterRiver
{
public:
	CDnWorldWaterRiver( CEtWorldWater *pWater );
	virtual ~CDnWorldWaterRiver();

protected:
	EtTextureHandle m_hAlphaTexture;
	CEtMeshStream *m_pMesh;
	EtMaterialHandle m_hMaterial;
	std::vector< SCustomParam > m_VecCustomParam;
	std::map<std::string, EtTextureHandle> m_MapUsingTexture;

protected:
	void GenerationVertex();
	void GenerationIndex();
	void GenerationNormal();
	void GenerationTexCoord();
	void GenerationShader();
	void GenerationTexture();
	void CreateRiver();

	EtVector3 GetSectorOffset();

public:
	virtual bool Load( CStream *pStream );
	void Render( LOCAL_TIME LocalTime );

	void RefreshWaterInfo();
};