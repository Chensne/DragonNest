#pragma once

#include "Singleton.h"

class CDnWorldData;
class CDnWorldDataManager : public CSingleton<CDnWorldDataManager>
{
public:
	struct SMapAreaInfoUnit
	{
		int mapId;
		int mapNameId;

		SMapAreaInfoUnit() { mapId = 0; mapNameId = 0; }
		bool operator==(const SMapAreaInfoUnit& unit) const	{ return unit.mapNameId == mapNameId; }
	};

	CDnWorldDataManager();
	virtual ~CDnWorldDataManager();

protected:
	std::map<int, std::vector<CDnWorldData *>> m_MapList;
	std::map<int, std::vector<SMapAreaInfoUnit> > m_MapAreaList;	// key : _MapArea, list : _MapNameID
	std::map<int, std::vector<int> > m_MapGroupByName;	// key : _MapName, list : _ID
#ifdef PRE_ADD_RENEW_RANDOM_MAP
	std::map< std::string, CDnWorldData* > m_MapWorldDataPtr;
#endif
#ifdef PRE_PARTY_DB
	std::vector<int> m_VillageMapIndexList;
#endif

public:
	bool Initialize( const char *szWorldFolder );

	CDnWorldData *GetWorldData( int nMapIndex, int nArrayIndex = 0 );
	const std::vector<SMapAreaInfoUnit>* GetMapAreaList(int areaToFind) const;
	const std::vector<int>* GetMapGroupByName(int mapNamId) const;
#ifdef PRE_PARTY_DB
	const std::vector<int>& GetVillageMapIndexList() const;
#endif
};