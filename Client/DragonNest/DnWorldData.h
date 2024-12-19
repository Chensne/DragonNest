#pragma once

#include "EtWorldEventArea.h"

class CEtWorldSector;
class CDnWorldData 
{
public:
	CDnWorldData( int nMapIndex );
	~CDnWorldData();

	struct NpcData {
		int nNpcID;
		EtVector2 vPos;
		float fRotate;
		std::wstring strName;
		int nJobType;
		int nUnitSize;
	};

	struct GateData {
		EtVector2 vPos;
		int nGateNo;
		int nMapIndex;
		int nMapType;
		int nLevel;
		std::wstring strName;
		std::vector<GateData> vecGateData;
	};

	struct PlantData {
		int nAreaIndex;
		EtVector2 vPos;
	};

protected:
	std::vector<NpcData *> m_pVecNpcData;
	std::vector<GateData *> m_pVecGateData;
	std::vector<PlantData *> m_pVecPlantData;

	int m_nMapIndex;
	std::string m_strMiniMapName;
	std::wstring m_strMapName;
	int m_nGridWidth;
	int m_nGridHeight;

protected:
	void LoadControl( CEtWorldSector *pSector );
	void LoadProp( CEtWorldSector *pSector );

public:
	bool Initialize( const char *szWorldFolder, const char *szGridName );
	void LoadNpcTalk();

	DWORD GetNpcCount();
	NpcData *GetNpcData( DWORD dwIndex );

	DWORD GetGateCount();
	GateData *GetGateData( DWORD dwIndex );
	GateData *GetGateDataFromGateNo( int nGateNo );

	DWORD GetPlantCount();
	PlantData * GetPlantData( DWORD dwIndex );

	int GetMapIndex() { return m_nMapIndex; }

	void SetMiniMapName( std::string &strMiniMapName )		{ m_strMiniMapName = strMiniMapName; }
	void GetMiniMapName( std::string &strMiniMapName )		{ strMiniMapName = m_strMiniMapName; }

	void SetMapName( std::wstring &strMapName )		{ m_strMapName = strMapName; }
	void GetMapName( std::wstring &strMapName )		{ strMapName = m_strMapName; }

	int GetGridWidth() { return m_nGridWidth; }
	int GetGridHeight() { return m_nGridHeight; }
};