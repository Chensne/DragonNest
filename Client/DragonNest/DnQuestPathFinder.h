#pragma once

const int MAX_WEIGHT = static_cast<unsigned int>(-1)/2;

struct PathInfo
{
	int nGateIndex;
	int nMapIndex;
};

typedef std::deque<PathInfo>	PathResult;


class CDnQuestPathFinder
{
public:
	CDnQuestPathFinder();
	virtual ~CDnQuestPathFinder();

	bool Init();
	bool GetPathOneWay( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result, bool bInverseResult = false );
	bool GetPath( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result, bool bBothWay = false );
	bool GetPathDijkstra( int nStartMapIndex, int nEndMapIndex, OUT PathResult& path_result);
	// 두맵이 게이트를 통해서 링크 되어있는가? 몇번 게이트를 통해 나갈수 있는지 알아낸다. 불가능 하면 -1
	bool IsLinkedMap(int nCurrentMapIndex, int nTargeMapIndex, OUT int& nLinkedGate);

// #77323, #77324 드래곤원정대 퀘스트 알림이 오류현상.
	void GetGatePath( int nStartMapIndex, int nEndMapIndex, bool bInverseResult, OUT std::vector< int > & beforeMap, OUT PathResult & path_result );
//

private:
	// 인접행렬의 배열인덱스로 실제 맵인덱스를 얻는다.
	inline int GetMapIndex(int nArrayIndex) { return m_AdjMatrixMapIndex[nArrayIndex]; }

private:

	struct GateInfo 
	{
		int nGateIndex;				// 게이트인댁스 1~8번 게이트
		int nLinkedMapIndex;		// 이게이트로 나가면 몇번 맵인가.
		//int nLinkedStartIndex;		// 이게이트로 나가면 몇번 스타트포지션인가.
	};

	struct MapNode 
	{
		MapNode()
		{
			nMapIndex = nAdjMatrixIndex = 0;
		}
		int	nMapIndex;
		int nAdjMatrixIndex;
		std::vector<GateInfo> gates;

		inline bool IsLinked(int nMapIndex, OUT int& nLinkedGate)
		{
			nLinkedGate = -1;
			for ( size_t i = 0 ; i < gates.size() ; i++ )
			{
				if ( gates[i].nLinkedMapIndex == nMapIndex )
				{
					nLinkedGate = gates[i].nGateIndex ;
					return true;
				}
			}

			return false;
		}
	};

	// 가중치값 인접 행렬
	std::vector< std::vector<int> > m_AdjMatrixWeight;
	// 가중치값 인접 행렬과 쌍으로 있는 실제 게임 맵 인덱스
	std::vector<int>				m_AdjMatrixMapIndex;	
	// 각 게이트 정보를 가지고 잇는 맵 
	// key - mapindex
	std::map<int, MapNode>			m_MapNode;
};