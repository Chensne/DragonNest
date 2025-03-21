#pragma once

#include "EtWorldSector.h"
#include "IResourcePathFinder.h"

class CDnCutSceneWorldProp;

class CDnCutSceneWorldSector : public CEtWorldSector
{
private:
	vector<CDnCutSceneWorldProp*>		m_vlpProcessProp;
	IResourcePathFinder*				m_pResPathFinder;
	
public:
	CDnCutSceneWorldSector(void);
	virtual ~CDnCutSceneWorldSector(void);

	// from CEtWorldSector
	void InitializeTerrain( DWORD *pAlphaTable, std::vector<int> &nVecBlockType, std::vector< std::vector<std::string> > &szVecLayerTexture, 
							std::vector< std::vector<float> > &fVecLayerTextureDistance, std::vector< std::vector<float> > &fVecLayerTextureRotation, 
							char *szGrassTexture, char *pGrassTable, float *fGrassWidth, float *fGrassHeightMin, float *fGrassHeightMax, float fShakeMin, float fShakeMax );

	virtual CEtWorldProp *AllocProp( int nClass );
	virtual CEtWorldSound *AllocSound();
	virtual int GetPropClassID( const char* szFileName );
	virtual void Free( void );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	////

	void InsertProcessProp( CDnCutSceneWorldProp* pProp );
	void DeleteProcessProp( CDnCutSceneWorldProp* pProp );

	bool CheckBakedLightMap();
	void BakeLightMap( int nLightMapSize, int nBlurSize );

	void SetResPathFinder( IResourcePathFinder* pResPathFinder ) { m_pResPathFinder = pResPathFinder; };
	
	int GetNumActionProp( void ) { return (int)m_vlpProcessProp.size(); };
	CDnCutSceneWorldProp* GetActionProp( int iIndex ) { return (iIndex < (int)m_vlpProcessProp.size()) ? m_vlpProcessProp.at(iIndex) : NULL; };
};
