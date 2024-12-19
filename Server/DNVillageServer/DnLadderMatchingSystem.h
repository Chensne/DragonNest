
#pragma once

namespace LadderSystem
{
	class CRoomRepository;
	class CRoom;

	class CMatchingSystem
	{
		public:
			
			CMatchingSystem( CRoomRepository* pRepository );

			void Process( DWORD dwCurTick );
			void AddMatchingTime( MatchType::eCode Type, int iSec );
			int  GetAvgMatchingTimeSec( MatchType::eCode Type );

		private:

			void _CreateRoomList( std::map<MatchType::eCode,std::vector<CRoom*>>& mMatchingRoom, int State, DWORD dwElapsedTick=0 );

			CRoomRepository*	m_pRoomRepository;
			DWORD				m_dwLastProcessTick;
			CMtRandom			m_Random;
			std::map<MatchType::eCode,std::pair<INT64,int>>	m_mAvgMatchingTime;
	};
};
