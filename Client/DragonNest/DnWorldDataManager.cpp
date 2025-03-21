#include "StdAfx.h"
#include "DnWorldDataManager.h"
#include "DnWorldData.h"
#include "DnTableDB.h"
#include "DnWorld.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldDataManager::CDnWorldDataManager()
{
}

CDnWorldDataManager::~CDnWorldDataManager()
{

	// 기존 맵 데이터가 std::map<int, std::vector<CDnWorldData*>> 이 형식으로 맵 아이디마다 월드데이터 포인터 벡터를 물고있는 형태인데
	// std::map<std::string, int>::iterator it = MapLoaded.find( szStr ); 를 사용해서 중복되는 경우 "넣어주지 않는방식" 으로 사용하고 있었습니다.
	// 맵데이터가 중복되어서 설정이 되는 경우 제대로 동작하지 않기때문에 이 부분을 개선하다보니 
	// CDnWorldDate* 를 컨테이너 내부에서 동시에 물고있는 케이스가 생겨서( 중복 생성을 방지하다보니 ) 해제할때 컨테이너 내부에서 delete된 포인터를 또 지워서 문제가 발생합니다.
	// 원론적으로 수정하면 Shared_ptr을 사용하거나 중복을 허용해야 하는데 컨테이너 전체에 적용해야하는것이라 퍼포먼스 문제도 있고 안하는게 좋을것 같아서
	// 일단은 CDnWorldDate* 포인터 객체 관리자로 컨트롤 하도록 설정합니다.
	// 따로 관리되는 객체가 있어서 좀 번거로운데.. 나중에 m_MapList가 인덱싱을 사용해서 참조하도록 하는게 좋을것 같습니다.
#ifdef PRE_ADD_RENEW_RANDOM_MAP
	if( m_MapWorldDataPtr.empty() == false )
	{
		std::map< std::string, CDnWorldData* >::iterator it;
		for( it = m_MapWorldDataPtr.begin(); it != m_MapWorldDataPtr.end(); it++ )
			SAFE_DELETE( it->second );

		m_MapWorldDataPtr.clear();
	}

	std::map<int, std::vector<CDnWorldData*>>::iterator it;
	for( it = m_MapList.begin(); it != m_MapList.end(); it++ ) 
		it->second.clear();
#else
	std::map<int, std::vector<CDnWorldData*>>::iterator it;
	for( it = m_MapList.begin(); it != m_MapList.end(); it++ ) {
		SAFE_DELETE_PVEC( it->second );
	}
#endif

	SAFE_DELETE_VEC( m_MapList );
}

bool CDnWorldDataManager::Initialize( const char *szWorldFolder )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );

	int nCount = pSox->GetItemCount();
	char szLabel[64];
	std::map<std::string, int> MapLoaded;

	std::string szStr;
	int nWorldLevel(0);

	for( int i=0; i<nCount; i++ ) {
		int nItemID = pSox->GetItemID(i);

		std::vector<CDnWorldData *> pVecList;
		bool bValid = false;

		for( int j=0; j<10; j++ ) {
			pVecList.push_back( NULL );

			sprintf_s( szLabel, "_ToolName%d", j + 1 );
			szStr = pSox->GetFieldFromLablePtr( nItemID, szLabel )->GetString();
			if( szStr.empty() ) continue;

			std::map<std::string, int>::iterator it = MapLoaded.find( szStr );
			if( it != MapLoaded.end() ) 
			{
				it->second++;

#ifdef PRE_ADD_RENEW_RANDOM_MAP
				std::map< std::string, CDnWorldData* >::iterator world_it = m_MapWorldDataPtr.find( szStr );
				if( world_it != m_MapWorldDataPtr.end() )
				{
					pVecList[j] = world_it->second;
				}
#endif
				continue;
			}

			MapLoaded.insert( make_pair( szStr, 1 ) );
			CDnWorldData *pData = new CDnWorldData( nItemID );
			if( pData->Initialize( szWorldFolder, szStr.c_str() ) == false ) {
				SAFE_DELETE( pData );
				continue;
			}

			// Note : 맵테이블에 있는 정보 입력
			//
			pData->SetMapName( std::wstring(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( nItemID, "_MapNameID" )->GetInteger() )) );
			pData->SetMiniMapName( szStr );
//			pData->SetWorldLevel( pSox->GetFieldFromLablePtr( nItemID, "_WorldLevel" )->GetInteger() );

			bValid = true;
			pVecList[j] = pData;

#ifdef PRE_ADD_RENEW_RANDOM_MAP
			m_MapWorldDataPtr.insert( make_pair( szStr, pData ) );
#endif
		}

		CDnWorld::MapTypeEnum mapType = (CDnWorld::MapTypeEnum)pSox->GetFieldFromLablePtr(nItemID, "_MapType")->GetInteger();
		if (mapType == CDnWorld::MapTypeDungeon)
		{
			int mapNameId = pSox->GetFieldFromLablePtr( nItemID, "_MapNameID" )->GetInteger();
			char* pMapAreaString = pSox->GetFieldFromLablePtr(nItemID, "_MapArea")->GetString();
			int dungeonEnterTableID = pSox->GetFieldFromLablePtr(nItemID, "_EnterConditionTableID")->GetInteger();
			bool bIncludeBuild = pSox->GetFieldFromLablePtr(nItemID, "_IncludeBuild")->GetInteger() ? true : false;
			int mapAreaIdx = atoi(pMapAreaString);
			if (mapNameId != 0 && mapAreaIdx != 0 && dungeonEnterTableID != 0 && bIncludeBuild)
			{
				SMapAreaInfoUnit unit;
				unit.mapId = nItemID;
				unit.mapNameId = mapNameId;

				std::map<int, std::vector<SMapAreaInfoUnit> >::iterator iter = m_MapAreaList.find(mapAreaIdx);

				bool bNewInsert = true;
				if (iter != m_MapAreaList.end())
				{
					std::vector<SMapAreaInfoUnit>& mapNameList = (*iter).second;
					std::vector<SMapAreaInfoUnit>::iterator mapNameListIter = std::find(mapNameList.begin(), mapNameList.end(), unit);
					if (mapNameListIter == mapNameList.end())
						mapNameList.push_back(unit);
					bNewInsert = false;
				}

				if (bNewInsert)
				{
					std::vector<SMapAreaInfoUnit> mapNameList;
					mapNameList.push_back(unit);

					m_MapAreaList.insert(std::make_pair(mapAreaIdx, mapNameList));
				}
			}

			if (mapNameId != 0)
			{
				std::map<int, std::vector<int> >::iterator mgnIter = m_MapGroupByName.find(mapNameId);
				if (mgnIter != m_MapGroupByName.end())
				{
					std::vector<int>& mapGroup = (*mgnIter).second;
					mapGroup.push_back(nItemID);
				}
				else
				{
					std::vector<int> mapGroup;
					mapGroup.push_back(nItemID);
					m_MapGroupByName.insert(std::make_pair(mapNameId, mapGroup));
				}
			}
		}

		if( !bValid ) continue;
		m_MapList.insert( make_pair( nItemID, pVecList ) );
	}

#ifdef PRE_PARTY_DB
	DNTableFileFormat* pGateTable = GetDNTable(CDnTableDB::TMAPGATE);

	nCount = pGateTable->GetItemCount();
	int i = 0;
	for (; i<nCount; i++)
	{
		int nItemID = pGateTable->GetItemID(i);
		DNTableFileFormat::Cell* pCell = pGateTable->GetFieldFromLablePtr(nItemID, "_MapIndex");
		if (pCell != NULL)
		{
			int mapIdx = pCell->GetInteger();
			if (mapIdx > 0)
			{
				DNTableFileFormat::Cell* pMapTypeCell = pSox->GetFieldFromLablePtr(mapIdx, "_MapType");
				if (pMapTypeCell == NULL)
					continue;

				CDnWorld::MapTypeEnum mapType = (CDnWorld::MapTypeEnum)pMapTypeCell->GetInteger();
				if (mapType == CDnWorld::MapTypeVillage)
					m_VillageMapIndexList.push_back(mapIdx);
			}
		}
	}
#endif

	return true;
}


CDnWorldData *CDnWorldDataManager::GetWorldData( int nMapIndex, int nArrayIndex )
{
	std::map<int, std::vector<CDnWorldData*>>::iterator it = m_MapList.find( nMapIndex );
	if( it == m_MapList.end() ) return NULL;
	if( nArrayIndex < 0 || nArrayIndex >= (int)it->second.size() ) return NULL;
	return it->second[nArrayIndex];
}

const std::vector<CDnWorldDataManager::SMapAreaInfoUnit>* CDnWorldDataManager::GetMapAreaList(int areaToFind) const
{
	std::map<int, std::vector<SMapAreaInfoUnit> >::const_iterator iter = m_MapAreaList.find(areaToFind);
	if (iter != m_MapAreaList.end())
	{
		const std::vector<SMapAreaInfoUnit>& mapNameIdList = (*iter).second;
		return &mapNameIdList;
	}

	return NULL;
}

const std::vector<int>* CDnWorldDataManager::GetMapGroupByName(int mapNamId) const
{
	std::map<int, std::vector<int> >::const_iterator iter = m_MapGroupByName.find(mapNamId);
	if (iter != m_MapGroupByName.end())
	{
		const std::vector<int>& mapIdList = (*iter).second;
		return &mapIdList;
	}

	return NULL;
}

#ifdef PRE_PARTY_DB
const std::vector<int>& CDnWorldDataManager::GetVillageMapIndexList() const
{
	return m_VillageMapIndexList;
}
#endif