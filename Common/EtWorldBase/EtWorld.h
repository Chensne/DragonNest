#pragma once

#include "EtWorldGrid.h"
#include "./Common.h"
#include "Timer.h"

class CEtWorldProp;
class CUnionValueProperty;
class CEtWorld {
public:
	CEtWorld();
	virtual ~CEtWorld();

	static int s_nWorldVersion;
	static char *s_szWorldHeaderString;
	static bool s_bIgnoreInvalidFile;

protected:
	CEtWorldGrid *m_pGrid;

	std::string m_szCurrentGrid;
	CEtWorldGrid *m_pCurrentGrid;

	EtVector3 m_vPrevPosition;
	EtVector3 m_vPosition;
	EtVector3 m_vDirection;
	float m_fDirectionLength;

	std::string m_szWorldFolder;

	struct InfoFile_Header {
		char szStr[32];
		int nVersion;
	};

protected:
	virtual CEtWorldGrid *AllocGrid();

public:
	virtual bool Initialize( const char *szWorldFolder, const char *szGridName );
	virtual void Destroy();

	virtual bool ForceInitialize( int nLoadSectorEnum = 0xFFFFFDFF ); // LSE_All ^ LSE_GenCollisionHeight
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	float GetHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );
	float GetHeight( EtVector3 &vPos, EtVector3 *pNormal = NULL, int nTileScale = 1 );


	float GetCollisionHeight( float fX, float fZ, EtVector3 *pNormal = NULL, int nTileScale = 1 );
	float GetCollisionHeight( EtVector3 &vPos, EtVector3 *pNormal = NULL, int nTileScale = 1 );

	DWORD GetCurrentTileSize();

	bool GetWaterHeight( float fX, float fZ, float *pfHeight );
	bool GetWaterHeight( EtVector3 &vPos, float *pfHeight );

	char GetAttribute( float fX, float fZ );
	char GetAttribute( EtVector3 &vPos );
	int GetAttributeBlockSize( float fX, float fZ );

	NavigationMesh *GetNavMesh( EtVector3 &vPos );

	bool Pick( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );
	bool PickWater( EtVector3 &vOrig, EtVector3 &vDir, EtVector3 &vPickPos );

	// 위치를 업뎃해줘야 로드가 자동으로 된다!! 업뎃 함수들!!
	void Update( EtVector3 &vPos, EtVector3 &vDir, float fDirLength );

	const EtVector3 *GetUpdatePrevPosition() { return &m_vPrevPosition; }
	const EtVector3 *GetUpdatePosition() { return &m_vPosition; }
	const EtVector3 *GetUpdateDirection() { return &m_vDirection; }
	float GetUpdateDirectionLength() { return m_fDirectionLength; }
	CEtWorldSector *GetSector( float fX, float fZ );
	void CalcWorldBasePos( float fX, float fZ, float &fResultX, float &fResultZ );

	CEtWorldGrid *FindGrid( const char *szGridName, CEtWorldGrid *pGrid = NULL );
	const char *GetWorldFolder() { return m_szWorldFolder.c_str(); }

	DWORD GetGridX();
	DWORD GetGridY();
	DWORD GetGridWidth();
	DWORD GetGridHeight();
	DWORD GetGridTileSize();
	CEtWorldGrid *GetGrid() { return m_pGrid; }

	void ScanProp( EtVector3 &vPos, float fRadius, DNVector(CEtWorldProp*) *pVecResult );
	void ScanDecal( EtVector2 &vPos, float fRadius, std::vector<CEtWorldDecal *> *pVecResult );
	void ScanEventArea( EtVector2 &vPos, float fRadius, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SAABox &Box, std::vector<CEtWorldEventArea *> *pVecResult );
	void ScanEventArea( SOBB &Box, std::vector<CEtWorldEventArea *> *pVecResult );

	void FindPropFromCreateUniqueID( DWORD dwUniqueID, std::vector<CEtWorldProp *> *pVecResult );
	// pCustomParam 는 포인터 alloc 해서 넘겨줘야하구 delete 는 객체가 지워질때 해준다.
	CEtWorldProp *AddProp( const char *szPropName, EtVector3 &vPos, EtVector3 &vRotate, EtVector3 &vScale, void *pCustomParam );

	int FindEventControlFromUniqueID( int nUniqueID, std::vector<CEtWorldEventControl *> *pVecList );
	int FindEventAreaFromName( int nUniqueID, const char *szAreaName, std::vector<CEtWorldEventArea *> *pVecList );
	int FindEventAreaFromSubStr( int nUniqueID, const char *szStr, std::vector<CEtWorldEventArea *> *pVecList );
	int FindEventAreaFromCreateUniqueID( int nCreateUniqueID, std::vector<CEtWorldEventArea *> *pVecList );

	void OnTriggerEventCallback( const char *szFuncName, LOCAL_TIME LocalTime, float fDelta, bool bProcessQueue = true );
	void EnableTriggerEventCallback( bool bEnable );
	void InsertTriggerEventStore( const char *szValueStr, int nValue );
	void EnableTriggerObject( const char *szTriggerName, bool bEnable );

	void CalcWorldSize( float &fCenter, float &fSize );
	void CalcWorldSize( EtVector2 &vSize );

	static void SetIgnoreInvalidFile( bool bValue ) { s_bIgnoreInvalidFile = bValue; }

//#ifdef PRE_ADD_FILTEREVENT
	virtual void SetSceneAbsoluteColor( bool bMonochrome, EtVector3 & vColor, float fVolume ){}
//#endif PRE_ADD_FILTEREVENT

};
