#pragma once

#include "./Common.h"
#include "Timer.h"
class CEtWorldGrid;
class CEtWorldProp;
class CEtWorldEventControl;
class CEtWorldEventArea;
class CEtWorldSound;
class CEtTrigger;
class CEtWorldWater;
class CEtWorldDecal;
class NavigationMesh;
class CUnionValueProperty;
class NavigationCell;

#include "EtOctree.h"
#include "EtQuadtree.h"

class CEtWorldSector {
public:
	CEtWorldSector();
	virtual ~CEtWorldSector();

	enum LoadSectorEnum {
		LSE_Terrain = 0x0001,
		LSE_Prop = 0x0002,
		LSE_Control = 0x0004,
		LSE_Sound = 0x0008,
		LSE_Attribute = 0x0010,
		LSE_Navigation = 0x0020,
		LSE_Trigger = 0x0040,
		LSE_Water = 0x0080,
		LSE_Decal = 0x0100,
		LSE_GenCollisionHeight = 0x0200,

		LSE_All = 0xFFFF,
	};

	// Default Process
	virtual bool Initialize( CEtWorldGrid *pParentGrid, SectorIndex Index );
	virtual bool Load( int nBlockIndex = -1, bool bThreadLoad = true, int nLoadSectorEnum = 0xFFFFFFFF );
	virtual void Free();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta ) {}

	// HeightField Info Func
	EtVector3 *GetOffset() { return &m_Offset; }
	float GetHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );
	float GetHeightToWorld( float fX, float fZ, EtVector3 *pNormal = NULL );
	bool GetWaterHeight( float fX, float fZ, float *pfResult );

	bool Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );
	bool PickWater( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );

	void SetTileSize( float fValue );
	__forceinline float GetTileSize() { return m_fTileSize; }

	__forceinline DWORD GetTileWidthCount() { return m_nTileWidthCount; }
	__forceinline DWORD GetTileHeightCount() { return m_nTileHeightCount; }

	float GetHeightMultiply() { return m_fHeightMultiply; }
	SectorIndex GetIndex() { return m_Index; }
	CEtWorldGrid *GetParentGrid() { return m_pParentGrid; }

	void SetLoadFailed( bool bValue ) { m_bLoadFailed = bValue; }

	// Prop Func
	CEtWorldProp *AddProp( const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale, void *pCustomParam );

	DWORD GetPropCount();
	CEtWorldProp *GetPropFromIndex( DWORD dwIndex );
	int AddPropCreateUniqueCount();
	CEtWorldProp *GetPropFromCreateUniqueID( DWORD dwUniqueID );

	void InsertProp( CEtWorldProp *pProp );
	bool DeleteProp( CEtWorldProp *pProp );
	__inline bool IsLoaded() {
		if( m_bLoadFailed ) return true;
		return m_bLoaded; 
	}
	virtual CEtWorldProp *AllocProp( int nClass );

	// Area Control Func
	virtual CEtWorldEventControl *AllocControl();
	void InsertControl( CEtWorldEventControl *pControl );
	void RemoveControl( int nUniqueID );

	DWORD GetControlCount();
	CEtWorldEventControl *GetControlFromIndex( DWORD dwIndex );
	CEtWorldEventControl *GetControlFromUniqueID( int nUniqueID );

	int AddEventAreaCreateUniqueCount();
	CEtWorldEventArea *GetEventAreaFromCreateUniqueID( int nValue );

	// Sound Func
	virtual CEtWorldSound *AllocSound();
	CEtWorldSound *GetSoundInfo() { return m_pSoundInfo; }

	// Trigger Func
	virtual CEtTrigger *AllocTrigger();
	CEtTrigger *GetTrigger() { return m_pTrigger; }

	// Attribute Func
	int GetAttributeBlockSize() { return m_nAttributeBlockSize; }
	char GetAttribute( float fX, float fZ );

	// Navigation Mesh Func
	virtual NavigationMesh *GetNavMesh() { return m_pNaviMesh; }

	// Water Func
	virtual CEtWorldWater *AllocWater();
	CEtWorldWater *GetWater() { return m_pWater; }

	// Decal Func
	virtual CEtWorldDecal *AllocDecal();
	void InsertDecal( CEtWorldDecal *pDecal );
	void DeleteDecal( CEtWorldDecal *pDecal );
	DWORD GetDecalCount();
	CEtWorldDecal *GetDecalFromIndex( DWORD dwIndex );

	void ScanProp( EtVector3 &vPos, float fRadius, DNVector(CEtWorldProp*) *pVecResult );
	void ScanDecal( EtVector2 &vPos, float fRadius, std::vector<CEtWorldDecal *> *pVecResult );
	void ScanEventArea( EtVector2 &vPos, float fRadius, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SAABox &Box, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SOBB &Box, std::vector<CEtWorldEventArea *> *pVecResult );

	float GetCollisionHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );

protected:
	CEtWorldGrid *m_pParentGrid;
	EtTerrainHandle m_Handle;
	SectorIndex m_Index;
	EtVector3 m_Offset;	// Center Offset
	float m_fTileSize;
	float m_fHeightMultiply;
	int m_nBlockCount;
	bool m_bLoaded;
	bool m_bLoadFailed;
	int m_nAttributeBlockSize;
	int m_nPropCreateUniqueCount;
	int m_nEventAreaCreateUniqueCount;
	int m_nTileWidthCount;
	int m_nTileHeightCount;

	short *m_pHeight;
	int		m_nHeightSize;
	char *m_pAttribute;
	int		m_nAttributeSize;
	short *m_pCollisionHeight;

	CEtOctree<CEtWorldProp *> *m_pPropOctree;
	CEtQuadtree<CEtWorldDecal *> *m_pDecalQuadtree;
	CEtQuadtree<CEtWorldEventArea *> *m_pEventAreaQuadtree;
	std::vector<CEtWorldProp *> m_pVecPropList;
	std::vector<CEtWorldDecal *> m_pVecDecalList;
	std::vector<CEtWorldEventControl *> m_pVecAreaControl;
	CEtWorldSound *m_pSoundInfo;
	CEtTrigger *m_pTrigger;
	CEtWorldWater *m_pWater;
	NavigationMesh* m_pNaviMesh;

	float m_fCenterHeight;
	float m_fRadius;

	std::vector<std::string> m_vecIgnorePropFolder;
	std::string m_szSectorPath;

protected:
	virtual int GetPropClassID( const char *szFileName ) { return 0; }

	EtVector3 GetHeightNormal( int nCellX, int nCellZ );
	void CalcTileCount();

	virtual bool LoadTerrain( const char *szSectorPath, int nBlockIndex = -1, bool bThreadLoad = true );
	virtual bool LoadProp( const char *szSectorPath, int nBlockIndex = -1, bool bThreadLoad = true );
	virtual bool LoadControlArea( const char *szSectorPath );
	virtual bool LoadSound( const char *szSectorPath );
	virtual bool LoadAttribute( const char *szSectorPath );
	virtual bool LoadNavigationMesh( const char *szSectorPath );
	virtual bool LoadTrigger( const char *szSectorPath );
	virtual bool LoadWater( const char *szSectorPath );
	virtual bool LoadDecal( const char *szSectorPath );

	virtual bool LoadSectorSize( const char *szSectorPath );
	// 실제 클라이언트쪽에선 요 함수 상속받아서 생성한다. 이 클레스에선 엔진 관련 오브젝트를 생성하지 안습!!
	virtual void InitializeTerrain( DWORD *pAlphaTable, std::vector<int> &nVecBlockType, std::vector< std::vector<std::string> > &szVecLayerTexture, std::vector< std::vector<float> > &fVecLayerTextureDistance, std::vector< std::vector<float> > &fVecLayerTextureRotation, char *szGrassTexture, char *pGrassTable, float *fGrassWidth, float *fGrassHeightMin, float *fGrassHeightMax, float fShakeMin, float fShakeMax ) {}

	bool GenerationCollisionHeight( short *pCollisionHeight );

	float GetHeightFromArray( float fX, float fZ, EtVector3 *pNormal, int nTileScale, short *pHeight );

	// Prop
	void LoadIgnorePropFolder( const char *szSectorPath );
	bool CheckIgnoreProp( const char *szPropName );

public:
	float GetHeightCenter() { return m_fCenterHeight; }
	float GetRadius() { return m_fRadius; }
};
