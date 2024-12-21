#include "stdafx.h"
#include "DnQuestPathFinder.h"
#include "DnTableDB.h"
#include "DnWorld.h"
#include "EtWorldEventArea.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 


CDnQuestPathFinder::CDnQuestPathFinder()
{

}

CDnQuestPathFinder::~CDnQuestPathFinder()
{

}

bool CDnQuestPathFinder::Init()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox ) return false;

	DNTableFileFormat* pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONMAP );
	if( !pDungeonSox ) return false;

	char szBuffer[1024]={0,};
	m_AdjMatrixMapIndex.reserve(pSox->GetItemCount()+pDungeonSox->GetItemCount());	
	
	for( int i=0; i<pSox->GetItemCount(); i++ ) 
	{
		int nTableID = pSox->GetItemID(i);

		MapNode mapNode;
		mapNode.nMapIndex = nTableID;

		for ( int j = 1; j <= DNWORLD_GATE_COUNT ; j++ )
		{
			std::vector<int> vecMapIndex, vecStartGate;
			CDnWorld::GetGateMapIndex( nTableID, j - 1, vecMapIndex );
			CDnWorld::GetGateStartGate( nTableID, j - 1, vecStartGate );

			for( DWORD itr = 0; itr < vecMapIndex.size(); ++itr )
			{
				// 맵 연결이 이상하다면 무시
				if ( vecMapIndex.empty() || vecStartGate.empty() )
					continue;

				int nMapIndex = vecMapIndex[itr];
				int nStartPositionIndex = vecStartGate[itr];

				GateInfo gate;
				gate.nGateIndex = j;
				gate.nLinkedMapIndex   = nMapIndex;
				/*gate.nLinkedStartIndex = nStartPositionIndex;*/
				mapNode.gates.push_back(gate);
			}
		}
		
		// 맵번호가 올바르고 게이트로 연결 되어있는 녀석들만 추가
		if ( nTableID > 0 )
		{
			mapNode.nAdjMatrixIndex = (int)m_AdjMatrixMapIndex.size();
			m_MapNode.insert( make_pair( nTableID, mapNode) );
			m_AdjMatrixMapIndex.push_back(nTableID);
		}
	}

	// 던전맵도 일반맵으로 그냥 처리해버린다.. fucking -_-+
	for( int i=0; i<pDungeonSox->GetItemCount(); i++ ) 
	{
		int nDungeonTableID = pDungeonSox->GetItemID(i);

		MapNode mapNode;
		mapNode.nMapIndex = nDungeonTableID;

		for ( int j = 1; j <= 5 ; j++ )
		{
			sprintf_s( szBuffer, "_MapIndex%d", j );
			int nMapIndex = pDungeonSox->GetFieldFromLablePtr( nDungeonTableID, szBuffer )->GetInteger();

			sprintf_s( szBuffer, "_Map%d_StartGate", j );
			int  nStartPositionIndex = pDungeonSox->GetFieldFromLablePtr( nDungeonTableID, szBuffer )->GetInteger();

			// 맵 연결이 이상하다면 무시
			if ( nMapIndex <= 0 || nStartPositionIndex <= 0)
				continue;

			GateInfo gate;
			gate.nGateIndex = j;
			gate.nLinkedMapIndex   = nMapIndex;
			/*gate.nLinkedStartIndex = nStartPositionIndex;*/
			mapNode.gates.push_back(gate);
		}

		// 맵번호가 올바르고 게이트로 연결 되어있는 녀석들만 추가
		if ( nDungeonTableID > 0 )
		{
			mapNode.nAdjMatrixIndex = (int)m_AdjMatrixMapIndex.size();
			m_MapNode.insert( make_pair( nDungeonTableID, mapNode) );
			m_AdjMatrixMapIndex.push_back(nDungeonTableID);
		}

	}

	m_AdjMatrixWeight.resize(m_AdjMatrixMapIndex.size());
	for ( size_t i = 0 ; i < m_AdjMatrixMapIndex.size() ; i++ ) {
		m_AdjMatrixWeight[i].resize(m_AdjMatrixMapIndex.size());
	}

	int nMapCount = (int)m_AdjMatrixMapIndex.size();

	// 여기서 인접 가중치 행렬 세팅	
	for ( int i = 0 ; i < nMapCount ; i++ )
	{
		for ( int j = 0 ; j < nMapCount ; j++ )
		{
			if (GetMapIndex(i) == GetMapIndex(j) )
			{
				m_AdjMatrixWeight[i][j] = 0;
			}
			else
			{
				int nGateIdx = -1;
				bool bResult = IsLinkedMap(GetMapIndex(i), GetMapIndex(j), nGateIdx);
				if ( bResult )
					m_AdjMatrixWeight[i][j] = 1;
				else
					m_AdjMatrixWeight[i][j] = MAX_WEIGHT;
			}
		}
	}
	return true;
}

bool CDnQuestPathFinder::IsLinkedMap(int nCurrentMapIndex, int nTargeMapIndex, OUT int& nLinkedGate)
{
	std::map<int, MapNode>::iterator it = m_MapNode.find(nCurrentMapIndex);
	if ( it == m_MapNode.end() )
		return false;

	MapNode mapNode = it->second;

	bool bResult = mapNode.IsLinked(nTargeMapIndex, nLinkedGate);
	return bResult;
}

bool CDnQuestPathFinder::GetPathOneWay( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result, bool bInverseResult)
{
	path_result.clear();

	if( m_MapNode.count(nStartMapIndex) == 0 || m_MapNode.count(nEndMapIndex) == 0 ) {
		return false;
	}

	int nCurIndex = 0;
	int nNextIndex = 1;
	static std::vector< int > vecMapIndices[ 2 ];
	static std::set< int > usedMapIndices;
	static std::vector< int > beforeMap;	
	usedMapIndices.clear();
	vecMapIndices[0].clear();
	vecMapIndices[1].clear();
	beforeMap.resize( m_AdjMatrixMapIndex.size() );	
	std::fill( beforeMap.begin(), beforeMap.end(), -1);

	vecMapIndices[ nCurIndex ].push_back( nStartMapIndex );
	usedMapIndices.insert( nStartMapIndex );

	const int nMaxStep = 16;
	for( int nStep = 0; nStep < nMaxStep; nStep++ ) {
		vecMapIndices[ nNextIndex ].clear();
		for each( int nMapIndex in vecMapIndices[ nCurIndex ] ) {

			int nAdjIndex = 0;
			MapNode mapNode;
			if( m_MapNode.find(nMapIndex) != m_MapNode.end() ) {
				nAdjIndex = m_MapNode[nMapIndex].nAdjMatrixIndex;
				mapNode = m_MapNode[nMapIndex];
			}
			for each( GateInfo gate in mapNode.gates ) {

				if( mapNode.nMapIndex == CGlobalInfo::GetInstance().m_nCurrentMapIndex ) {
					char szLabel[64]={0,};
					sprintf_s( szLabel, "Gate %d", gate.nGateIndex );
					std::vector<CEtWorldEventArea *> VecArea;
					CDnWorld::GetInstance().FindEventAreaFromName( ETE_EventArea, szLabel, &VecArea );
					if( VecArea.empty() ) {
						continue;
					}
				}			
								
				if( gate.nLinkedMapIndex == nEndMapIndex )				
				{
					if( m_MapNode.find(nEndMapIndex) != m_MapNode.end() ) {
						beforeMap[ m_MapNode[ nEndMapIndex ].nAdjMatrixIndex ] = nAdjIndex;
					}

					GetGatePath( nStartMapIndex, nEndMapIndex, bInverseResult, beforeMap, path_result );
					return true;
				}

				// #77323, #77324 드래곤원정대 퀘스트 알림이 오류현상.
				// : 랜덤맵의 경우 목적지 검색을 랜덤맵으로 묶인 맵들까지로 확대한다.
				else
				{
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
					if( pSox && pSox->IsExistItem(gate.nLinkedMapIndex) )
					{
						DNTableCell * pCell = pSox->GetFieldFromLablePtr( gate.nLinkedMapIndex, "_MapSubType" );
						if( pCell && pCell->GetInteger() == CDnWorld::MapSubTypeEnum::MapSubTypeFellowship )
						{
							char strCell[32]={0,};
							for( int k=1; k<11; ++k )
							{
								sprintf_s( strCell, 32, "_ToolNameIndex%d", k );
								pCell = pSox->GetFieldFromLablePtr( gate.nLinkedMapIndex, strCell );
								if( pCell && pCell->GetInteger() == nEndMapIndex )
								{
									if( m_MapNode.find(nEndMapIndex) != m_MapNode.end() )
										beforeMap[ m_MapNode[ nEndMapIndex ].nAdjMatrixIndex ] = nAdjIndex;

									GetGatePath( nStartMapIndex, nEndMapIndex, bInverseResult, beforeMap, path_result );
									return true;
								}
							}
						}
					}					
				}
				//

				if( usedMapIndices.count( gate.nLinkedMapIndex ) == 0 ) {
					vecMapIndices[ nNextIndex ].push_back( gate.nLinkedMapIndex );
					usedMapIndices.insert( gate.nLinkedMapIndex );

					if( m_MapNode.find(gate.nLinkedMapIndex) != m_MapNode.end() ) {
						beforeMap[ m_MapNode[ gate.nLinkedMapIndex ].nAdjMatrixIndex ] = nAdjIndex;
					}
				}
			}
		}
		if( vecMapIndices[ nNextIndex ].empty() ) {
			break;
		}
		std::swap( nCurIndex, nNextIndex );
	}
	return false;
}

bool CDnQuestPathFinder::GetPath( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result, bool bBothWay )
{
	if( bBothWay ) {
		PathResult result, resultInverse;
		bool bResult = GetPathOneWay( nStartMapIndex, nEndMapIndex, result );
		bool bResultInverse = GetPathOneWay( nEndMapIndex, nStartMapIndex, resultInverse, true);
		if( bResult && bResultInverse ) {
			if( result.size() < resultInverse.size() )	{
				path_result = result;
			}
			else {
				path_result = resultInverse;
			}
			return true;
		}
		else if ( bResult ) {
			path_result = result;
			return true;
		}
		else if ( bResultInverse ) {
			path_result = resultInverse;
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return GetPathOneWay( nStartMapIndex, nEndMapIndex, path_result );
	}
	return false;
}

// Dijkstra algorithm 참고.
bool CDnQuestPathFinder::GetPathDijkstra( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result)
{
	path_result.clear();
	
	std::map<int, MapNode>::iterator it = m_MapNode.find(nStartMapIndex);
	if ( it == m_MapNode.end() )
		return false;

	MapNode mapNode = it->second;
	int startIndex = mapNode.nAdjMatrixIndex;

	it = m_MapNode.find(nEndMapIndex);
	if ( it == m_MapNode.end() )
		return false;

	mapNode = it->second;
	int endIndex = mapNode.nAdjMatrixIndex;

	int ArraySize = (int)m_AdjMatrixMapIndex.size();
	int i,j,minIndex=-1,min;
	int path_cnt=0;
	
	static std::vector<int>	vecPath;
	static std::vector<bool>	vecUsed;
	static std::vector<int>	vecDistance;
	static std::vector<int>	vecVia;

	vecPath.resize(ArraySize);
	vecUsed.resize(ArraySize);
	vecDistance.resize(ArraySize);
	vecVia.resize(ArraySize);

	// 초기화
	for( i = 0 ; i < ArraySize ; i++ )
	{
		vecUsed[i] = false;
		vecDistance[i] = MAX_WEIGHT;
		vecVia[i] = startIndex;
		vecPath[i] = 0;
	}

	vecDistance[startIndex]=0;	

	DWORD dwA = GetTickCount();
	// 찾기
	for( i = 0 ; i < ArraySize ; i++ )
	{
		min = MAX_WEIGHT;		
		for( j = 0 ; j < ArraySize ; j++ )
		{
			if( vecUsed[j] == false && vecDistance[j] < min )
			{
				minIndex = j;
				min = vecDistance[j];
			}
		}		
		vecUsed[minIndex] = true;
		if( min == MAX_WEIGHT )
			break;		

		for( j = 0 ; j < ArraySize ; j++ )
		{
			if( m_AdjMatrixWeight[minIndex][j] == MAX_WEIGHT )
				continue;
			if( vecDistance[j] > vecDistance[minIndex]+m_AdjMatrixWeight[minIndex][j] )
			{
				vecDistance[j] = vecDistance[minIndex]+m_AdjMatrixWeight[minIndex][j];
				vecVia[j]=minIndex;
			}
		}
	}
	// 최단 경로
	minIndex = endIndex;
	while(true)
	{
		vecPath[path_cnt] = minIndex;
		
		path_cnt++;

		if ( path_cnt >= (int)ArraySize )return false;

		if ( minIndex == startIndex )
			break;

		minIndex = vecVia[minIndex];		
	}

	static std::vector<int> MapStep;
	MapStep.clear();
	 
	for( i = path_cnt-1 ; i>=0 ; i-- )
	{
		MapStep.push_back(GetMapIndex(vecPath[i]));
	}

	i = 0 ;
	for ( i = 0 ; i < (int)MapStep.size() ; i++ )
	{
		PathInfo pathInfo;
		pathInfo.nMapIndex = MapStep[i];
		pathInfo.nGateIndex = -1;

		if ( i+1 < (int)MapStep.size() )
		{
			int nLinkedGate = -1;
			bool bResult = IsLinkedMap(MapStep[i], MapStep[i+1], nLinkedGate );
			if ( bResult )
				pathInfo.nGateIndex = nLinkedGate;
		}

		path_result.push_back(pathInfo);
	}

	return true;
}

// #77323, #77324 드래곤원정대 퀘스트 알림이 오류현상.
void CDnQuestPathFinder::GetGatePath( int nStartMapIndex, int nEndMapIndex, bool bInverseResult, OUT std::vector< int > & beforeMap, OUT PathResult & path_result )
{
	int nPathMapIndex = nEndMapIndex;
	int nPathBeforeIndex = -1;
	while( true ) {
		PathInfo path;
		path.nMapIndex = nPathMapIndex;
		path.nGateIndex = -1;

		int nLinkedGate = -1;
		bool bResult = IsLinkedMap(nPathMapIndex, nPathBeforeIndex, nLinkedGate );
		if ( bResult )
			path.nGateIndex = nLinkedGate;

		if( bInverseResult ) {
			path_result.push_back( path );
		}
		else {
			path_result.push_front( path );							
		}

		if( nPathMapIndex == nStartMapIndex ) {
			break;
		}
		nPathBeforeIndex = nPathMapIndex;
		nPathMapIndex = m_AdjMatrixMapIndex[ beforeMap[ m_MapNode[ nPathMapIndex ].nAdjMatrixIndex ] ];
	}	
}
//