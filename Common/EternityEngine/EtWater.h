#pragma once

class CEtWater
{
public:
	struct WaterResource
	{
		EtTextureHandle hWaterMap;
		float fWaterHeight;
		bool bIgnoreBake;
	};
public:
	CEtWater();
	virtual ~CEtWater();
	void Clear();

protected:
	int m_nWaterMapSize;

	EtDepthHandle m_hWaterMapDepth;

	EtSurface *m_pBackupRenderTarget;
	EtSurface *m_pBackupDepthTarget;

	EtMatrix	m_BackupCamMat;

	EtViewPort	m_BackupViewport;
	std::vector< WaterResource > m_WaterResources;

	float			m_fWaterBakeTime;
	EtMatrix		m_WaterBakeViewParam;
	EtMatrix		m_WaterBakeLastView;

	float			m_fAveDeltaTime[32];
	int				m_nDeltaIndex;
	bool			m_bOptimize;

public:
	void Optimize( bool bOptimize ) { m_bOptimize = bOptimize; }
	void Initialize(int nCount);
	void BeginWater( int index);
	void EndWater( int index );
	void Bake( CEtRenderStack *pRenderStack, float fElapsedTime );
	EtMatrix GetViewProjMat(int index);
	EtTextureHandle GetWaterMapTexture(int index) {
		if( index < 0 || index >= (int)m_WaterResources.size() ) return CEtTexture::Identity();
		return m_WaterResources[index].hWaterMap;
	}
	void SetWaterHeight( int index, float fHeight) {
		m_WaterResources[index].fWaterHeight = fHeight;
	}
	void SetWaterIgnoreBake( int index, bool bIgnoreBake ) {
		m_WaterResources[index].bIgnoreBake = bIgnoreBake;
	}
	bool GetWaterIgnoreBake( int index ) {
		if( index < 0 || index >= (int)m_WaterResources.size() ) return false;
		return m_WaterResources[index].bIgnoreBake;
	}
	int GetWaterCount() {return (int)m_WaterResources.size();}	

	EtMatrix* GetWaterBakeViewMat() { return &m_WaterBakeViewParam; } 

};

CEtWater *GetEtWater();
void DeleteWaterMap();
void CreateSimpleWaterMap( int nCount );
