
#include "Stdafx.h"
#include "DnLadderMatchingSystem.h"
#include "DNLadderRoom.h"
#include "DNLadderRoomRepository.h"
#include "DNMasterConnection.h"
#include "DNLadderSystemManager.h"
#include "DNGameDataManager.h"
#if defined(PRE_ADD_DWC)
#include "DNDWCSystem.h"
#endif

using namespace LadderSystem;

CMatchingSystem::CMatchingSystem( CRoomRepository* pRepository )
:m_pRoomRepository( pRepository )
,m_dwLastProcessTick(timeGetTime())
{
	_ASSERT( static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_MinSampleCount )) >= 2 );
}

void CMatchingSystem::Process( DWORD dwCurTick )
{
	// ProcessTick �˻�
	if( dwCurTick-m_dwLastProcessTick < 1000 )
		return;
	m_dwLastProcessTick = dwCurTick;
	// RandomSeed ����
	m_Random.srand( dwCurTick );

	// RoomState::WaitMatching List �����
	std::map<MatchType::eCode,std::vector<CRoom*>> mMatchingRoom;
	_CreateRoomList( mMatchingRoom, RoomState::WaitMatching, 1000 );

	for( std::map<MatchType::eCode,std::vector<CRoom*>>::iterator itor=mMatchingRoom.begin() ; itor!=mMatchingRoom.end() ; ++itor )
	{
		std::map<int,std::list<CRoom*>> mListUp;
		std::vector<int> vData;

		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			CRoom* pLadderRoom = (*itor).second[i];

			if( pLadderRoom->GetRoomState() != RoomState::WaitMatching )
				continue;

#if defined(PRE_ADD_DWC)
			//��Ī�ð� ����Ǹ� ť���� ������
			if( pLadderRoom->GetMatchType() == LadderSystem::MatchType::_3vs3_DWC || pLadderRoom->GetMatchType() == LadderSystem::MatchType::_3vs3_DWC_PRACTICE )			
			{
				if( !g_pDWCTeamManager || !g_pDWCTeamManager->CheckDWCMatchTime(pLadderRoom->GetMatchType()) )
				{
					pLadderRoom->SendLadderMatching( ERROR_DWC_LADDER_MATCH_CLOSED, true );
					pLadderRoom->ChangeRoomState( RoomState::WaitUser );					
					continue;
				}
			}
#endif

			bool bForceMatching = false;

			if( pLadderRoom->bIsForceMatching() )
			{
				for( UINT j=0 ; j<(*itor).second.size() ; ++j )
				{
					CRoom* pOpponentLadderRoom = (*itor).second[j];				
					if( pLadderRoom == pOpponentLadderRoom || pOpponentLadderRoom->GetRoomState() != RoomState::WaitMatching )
						continue;

					// ��Ī��Ŵ
					pLadderRoom->ChangeRoomState( RoomState::Matched );
					pLadderRoom->SetOpponentRoomIndex( pOpponentLadderRoom->GetRoomIndex() );
					pOpponentLadderRoom->ChangeRoomState( RoomState::Matched );
					pOpponentLadderRoom->SetOpponentRoomIndex( pLadderRoom->GetRoomIndex() );

					bForceMatching = true;
					break;
				}
			}		

			// �����ð� ���� ��Ī
			if( bForceMatching == false )
			{
				if( pLadderRoom->GetRoomStateElapsedTick() >= static_cast<DWORD>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_ForceMatchingSec ))*1000 )
				{
					for( UINT j=0 ; j<(*itor).second.size() ; ++j )
					{
						CRoom* pOpponentLadderRoom = (*itor).second[j];				
						if( pLadderRoom == pOpponentLadderRoom || pOpponentLadderRoom->GetRoomState() != RoomState::WaitMatching || pOpponentLadderRoom->GetRoomStateElapsedTick() < static_cast<DWORD>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_ForceMatchingSec ))*1000 )
							continue;

						// ��Ī��Ŵ
						pLadderRoom->ChangeRoomState( RoomState::Matched );
						pLadderRoom->SetOpponentRoomIndex( pOpponentLadderRoom->GetRoomIndex() );
						pOpponentLadderRoom->ChangeRoomState( RoomState::Matched );
						pOpponentLadderRoom->SetOpponentRoomIndex( pLadderRoom->GetRoomIndex() );

						bForceMatching = true;
						break;
					}
				}
			}

			if( bForceMatching == false )
			{
				pLadderRoom->GetMatchingSection( vData );

				for( UINT i=0 ; i<vData.size() ; ++i )
				{
					std::map<int,std::list<CRoom*>>::iterator itor = mListUp.find( vData[i] );
					if( itor == mListUp.end() )
					{
						std::list<CRoom*> Temp;
						Temp.push_back( pLadderRoom );
						mListUp.insert( std::make_pair(vData[i],Temp) );
					}
					else
					{
						(*itor).second.push_back( pLadderRoom );
					}
				}
			}
		}

		for( std::map<int,std::list<CRoom*>>::iterator LUitor=mListUp.begin() ; LUitor!=mListUp.end() ; ++LUitor )
		{
			if( (*LUitor).second.size() < static_cast<size_t>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_MinSampleCount )) )
				continue;

			do
			{
				// ���� WaitMatching �� ������ ���� Ű�� �ȴ�.
				std::list<CRoom*>::iterator LItor = (*LUitor).second.begin();
				CRoom* pLadderRoom = (*LItor);
				(*LUitor).second.remove( pLadderRoom );

				if( pLadderRoom->GetRoomState() == RoomState::Matched )
					continue;

				// �����ϰ� �ϳ��� �� ����
				size_t RandVal = m_Random.rand()%(*LUitor).second.size();
				LItor = (*LUitor).second.begin();
				std::advance( LItor, RandVal );
				CRoom* pOpponentLadderRoom = (*LItor);
				(*LUitor).second.remove( pOpponentLadderRoom );

				// ��Ī��Ŵ
				pLadderRoom->ChangeRoomState( RoomState::Matched );
				pLadderRoom->SetOpponentRoomIndex( pOpponentLadderRoom->GetRoomIndex() );
				pOpponentLadderRoom->ChangeRoomState( RoomState::Matched );
				pOpponentLadderRoom->SetOpponentRoomIndex( pLadderRoom->GetRoomIndex() );

			}while((*LUitor).second.size() >= 2 );
		}
	}

	// RoomState::Matched List �����
	mMatchingRoom.clear();
	_CreateRoomList( mMatchingRoom, (RoomState::Matched|RoomState::Starting), 1000 );

	for( std::map<MatchType::eCode,std::vector<CRoom*>>::iterator itor=mMatchingRoom.begin() ; itor!=mMatchingRoom.end() ; ++itor )
	{
		std::map<CRoom*,bool> mSyncingRoom;

		// Match ��Ű�� �� �������� ���� Ȯ��
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			CRoom* pLadderRoom = (*itor).second[i];

			if( mSyncingRoom.find(pLadderRoom) != mSyncingRoom.end() )
				continue;

			bool bCheck = false;
			CRoom* pOpponentLadderRoom = m_pRoomRepository->GetRoomPtr( pLadderRoom->GetOpponentRoomIndex() );
			if( pOpponentLadderRoom )
			{
				bCheck = pOpponentLadderRoom->bIsValidOpponentRoom( pLadderRoom, static_cast<int>(RoomState::Matched|RoomState::Starting) );
			}

			if( bCheck == false )
			{
				pLadderRoom->ChangeRoomState( RoomState::WaitUser );
				continue;
			}
		
			if( pLadderRoom->GetStartMsgCount() > 0 )
				pLadderRoom->SetStartMsgCount( pLadderRoom->GetStartMsgCount()-1 );

			// ���� LadderRoom �� RoomState �� ����� �� �����Ƿ� �ѹ��� üũ�Ѵ�.
			if( !(pLadderRoom->GetRoomState()&RoomState::Starting) )
				continue;

			if( pLadderRoom->GetStartMsgCount() > 0 || pOpponentLadderRoom->GetStartMsgCount() > 0 )
				continue;

			// ���� ��������� ����
			if( g_pMasterConnection && g_pMasterConnection->GetActive() )
			{
				mSyncingRoom[pLadderRoom] = true;
				mSyncingRoom[pOpponentLadderRoom] = true;

				int iMapIndex = g_pDataManager->GetRandomLadderMapIndex( pLadderRoom->GetMatchType() );
				if( iMapIndex < 0 )
				{
					_ASSERT(0);
					pLadderRoom->ChangeRoomState( RoomState::WaitUser );
					continue;
				}
				g_pMasterConnection->SendReqLadderGameID( pLadderRoom, pOpponentLadderRoom, CManager::GetInstance().GetChannelID(), timeGetTime(), iMapIndex );
			}
		}
	}
}

void CMatchingSystem::_CreateRoomList( std::map<MatchType::eCode,std::vector<CRoom*>>& mMatchingRoom, int State, DWORD dwElapsedTick/*=0*/ )
{
	const std::map<INT64,CRoom*> RoomList = m_pRoomRepository->GetRoomList();

	for( std::map<INT64,CRoom*>::const_iterator itor=RoomList.begin() ; itor!=RoomList.end() ; ++itor )
	{
		CRoom* pRoom = (*itor).second;

		if( pRoom->GetRoomState()&State && pRoom->GetRoomStateElapsedTick() >= dwElapsedTick )
		{
			if( State&RoomState::WaitMatching )
			{
				if( pRoom->bIsMatchingReady() == false )
					continue;
			}
			std::map<MatchType::eCode,std::vector<CRoom*>>::iterator itor = mMatchingRoom.find( pRoom->GetMatchType() );
			if( itor == mMatchingRoom.end() )
			{
				std::vector<CRoom*> vRoom;
				vRoom.push_back( pRoom );
				mMatchingRoom.insert( std::make_pair(pRoom->GetMatchType(),vRoom) );
			}
			else
			{
				(*itor).second.push_back( pRoom );
			}
		}
	}
}

void CMatchingSystem::AddMatchingTime( MatchType::eCode Type, int iSec )
{
	std::map<MatchType::eCode,std::pair<INT64,int>>::iterator itor = m_mAvgMatchingTime.find( Type );
	if( itor == m_mAvgMatchingTime.end() )
	{
		m_mAvgMatchingTime.insert( std::make_pair( Type, std::make_pair(iSec,1) ) );
	}
	else
	{
		(*itor).second.first += iSec;
		++(*itor).second.second;
	}

#if defined( _WORK )
	std::cout << "[Ladder] MatchType:" << Type << " ��Ī�ð�:" << iSec << "�� �ɸ�" << std::endl;
	std::cout << "[Ladder] MatchType:" << Type << " ��ո�Ī�ð�:" << GetAvgMatchingTimeSec(Type) << "��" << std::endl;
#endif // #if defined( _WORK )
}

int CMatchingSystem::GetAvgMatchingTimeSec( MatchType::eCode Type )
{
	std::map<MatchType::eCode,std::pair<INT64,int>>::iterator itor = m_mAvgMatchingTime.find( Type );
	if( itor != m_mAvgMatchingTime.end() )
		return static_cast<int>((*itor).second.first/(*itor).second.second);

	return -1;
}

