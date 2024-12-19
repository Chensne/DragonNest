#pragma once

#include "./Common.h"
#include "Timer.h"

class CEtWorldSector;
class CEtWorld;
class CEtWorldDecal;
class CEtWorldProp;
class CEtWorldEventControl;
class CEtWorldEventArea;
class CUnionValueProperty;
class NavigationCell;
class NavigationMesh;

class CEtWorldGrid {
public:
	CEtWorldGrid( CEtWorld *pWorld );
	virtual ~CEtWorldGrid();

protected:
	std::string m_szName;
	CEtWorld *m_pWorld;

	DWORD m_dwGridX;
	DWORD m_dwGridY;
	DWORD m_dwGridWidth;
	DWORD m_dwGridHeight;
	DWORD m_dwTileSize;
	std::vector< std::vector<CEtWorldSector *> > m_ppSector;
	CEtWorldSector *m_pProcessSector;

	CEtWorldGrid *m_pParent;
	std::vector<CEtWorldGrid *> m_pVecChild;

	std::vector<SectorIndex> m_VecActiveSector;

	struct InfoFile_Header {
		char szParentName[64];
		DWORD dwGridX;
		DWORD dwGridY;
		DWORD dwGridWidth;
		DWORD dwGridHeight;
		DWORD dwTileSize;
	};

	float m_fSectorCalcValueX;
	float m_fSectorCalcValueY;

protected:
	virtual CEtWorldSector *AllocSector();
	void FreeSector();

public: // 서버에서 PreLoad 하기위해.
	bool LoadSector( SectorIndex Index, bool bThreadLoad = true, int nLoadSectorEnum = 0xFFFFFFFF );
	void FreeSector( SectorIndex Index );

public:
	virtual bool Initialize( const char *szName, DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, DWORD dwTileSize );
	virtual bool LoadGrid( const char *szName );
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	CEtWorld *GetWorld() { return m_pWorld; }

	DWORD GetGridX() { return m_dwGridX; }
	DWORD GetGridY() { return m_dwGridY; }
	// 단위 미터임 
	DWORD GetGridWidth() { return m_dwGridWidth; }
	DWORD GetGridHeight() { return m_dwGridHeight; }
	// 단위 cm임
	DWORD GetTileSize() { return m_dwTileSize; }

	CEtWorldSector *GetSector( SectorIndex Index );
	float GetHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );
	bool GetWaterHeight( float fX, float fZ, float *pfHeight );
	char GetAttribute( float fX, float fZ );
	int GetAttributeBlockSize( float fX, float fZ );

	float GetCollisionHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );

	NavigationMesh *GetNavMesh( EtVector3 &vPos );

	void CalcWorldBasePos( float fX, float fZ, float &fResultX, float &fResultZ );

	bool Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );
	bool PickWater( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );

	DWORD GetActiveSectorCount();
	CEtWorldSector *GetActiveSector( DWORD dwIndex );
	virtual CEtWorldSector *GetSector( float fX, float fZ, float *fResultX = NULL, float *fResultZ = NULL );
	CEtWorldSector *GetProcessSector();

	void SetName( const char *szName );
	const char *GetName();
	void SetParent( CEtWorldGrid *pGrid );
	CEtWorldGrid *GetParent() { return m_pParent; }
	DWORD GetChildCount();
	CEtWorldGrid *GetChildFromIndex( DWORD dwIndex );
	void InsertChild( CEtWorldGrid *pGrid );
	void RemoveChild( CEtWorldGrid *pGrid );

	void ScanProp( EtVector3 &vPos, float fRadius, DNVector(CEtWorldProp*) *pVecResult );
	void ScanDecal( EtVector2 &vPos, float fRadius, std::vector<CEtWorldDecal *> *pVecResult );
	void ScanEventArea( EtVector2 &vPos, float fRadius, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SAABox &Box, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SOBB &Box, std::vector<CEtWorldEventArea *> *pVecResult );

	CEtWorldProp *AddProp( const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale, void *pCustomParam );
	int FindPropFromCreateUniqueID( DWORD dwUniqueID, std::vector<CEtWorldProp *> *pVecResult );

	int FindEventControlFromUniqueID( int nUniqueID, std::vector<CEtWorldEventControl *> *pVecList );
	int FindEventAreaFromName( int nUniqueID, const char *szAreaName, std::vector<CEtWorldEventArea *> *pVecList );
	int FindEventAreaFromSubStr( int nUniqueID, const char *szStr, std::vector<CEtWorldEventArea *> *pVecList );
	int FindEventAreaFromCreateUniqueID( int nCreateUniqueID, std::vector<CEtWorldEventArea *> *pVecList );

};
