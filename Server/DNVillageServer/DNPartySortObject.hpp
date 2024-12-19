
#pragma once

#include "DNParty.h"
#include "DNGameDataManager.h"
#include "DnCommonUtil.h"
#include <./boost/algorithm/string.hpp>

extern TVillageConfig g_Config;

#if defined( PRE_PARTY_DB )

namespace Party
{
	class SortObject
	{
	public:

		bool operator()( const CDNParty* lhs, const CDNParty* rhs )
		{
			if( lhs->GetSortPoint() <= rhs->GetSortPoint() )
				return false;

			return true;
		}	
	};

	class CalcSortPoint
	{
	public:

		CalcSortPoint( const CSPartyListInfo* pPacket, int iMapIndex ):m_pPacket(pPacket),m_iMapIndex(iMapIndex){}

		void CalcPoint( const CDNParty* pParty )
		{
#if defined( PRE_WORLDCOMBINE_PARTY )
			if( pParty->bIsWorldCombieParty() )
			{
				const_cast<CDNParty*>(pParty)->SetSortPoint( LLONG_MAX );
				return;
			}
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
			INT64 biPoint = 0;

			// ## �˻����� ���� ����
			if( wcslen(m_pPacket->wszSearchWord)>0 )
			{
				std::wstring				wstrString(m_pPacket->wszSearchWord);
				std::vector<std::wstring>	vSplit;

#if defined( PRE_FIX_61545 )
				WCHAR seps[] = { m_pPacket->cSeperator, 0 };
				boost::algorithm::split( vSplit, wstrString, boost::algorithm::is_any_of(seps) );
#else
				boost::algorithm::split( vSplit, wstrString, boost::algorithm::is_any_of(L" ") );
#endif // #if defined( PRE_FIX_61545 )

				std::wstring wstrPartyName = pParty->GetPartyName();
				std::transform( wstrPartyName .begin(), wstrPartyName .end(), wstrPartyName.begin(), towlower ); 

				std::wstring wstrMapName;
				const TMapInfo* pMapInfo = g_pDataManager->GetMapInfo( pParty->GetTargetMapIndex() );
				if( pMapInfo && pMapInfo->wstrMapName.empty() == false )
				{
					wstrMapName = pMapInfo->wstrMapName;
					std::transform( wstrMapName .begin(), wstrMapName .end(), wstrMapName.begin(), towlower ); 
				}

				const WCHAR* pwszDifficulty = CommonUtil::GetDungeonDifficultyString( pParty->GetDifficulty() );
				std::wstring wstrDifficulty;
				if( pwszDifficulty )
				{
					wstrDifficulty = pwszDifficulty;
					std::transform( wstrDifficulty.begin(), wstrDifficulty.end(), wstrDifficulty.begin(), towlower ); 
				}

				for( UINT i=0 ; i<vSplit.size() ; ++i )
				{
					if( i >= Party::Max::WordSpacing )
						break;

					// ������
					if( wcsstr( wstrPartyName.c_str(), vSplit[i].c_str() ) )
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::SearchWordTitle );

					// ��ǥ ��������
					if( wstrMapName.empty() == false )
					{
						if( wcsstr( wstrMapName.c_str(), vSplit[i].c_str() ) )
							biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::SearchWordStage );
					}

					// ���̵�
					if( wstrDifficulty.empty() == false  )
					{
						if( wcsstr( wstrDifficulty.c_str(), vSplit[i].c_str() ) )
							biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::SearchWordDifficulty );
					}
				}
			}

			// ## ���� ��Ƽ ����
			if( pParty->bIsPasswordParty() )
				biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PublicParty );
			else
				biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PrivateParty );

			// ## �������� ���̵�
			switch( pParty->GetDifficulty() )
			{
				case Dungeon::Difficulty::Abyss:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyAbyss );
					break;
				}
				case Dungeon::Difficulty::Master:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyMaster );
					break;
				}
				case Dungeon::Difficulty::Hard:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyHard );
					break;
				}
				case Dungeon::Difficulty::Normal:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNormal );
					break;
				}
				case Dungeon::Difficulty::Easy:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyEasy );
					break;
				}
				default:
				{
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNone );
					break;
				}
			}

			// ## �������� ���̵�(��������)
			if( m_pPacket->OrderType == Party::OrderType::TargetMapDifficultyAsc )
			{
				switch( pParty->GetDifficulty() )
				{
					case Dungeon::Difficulty::Abyss:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyAbyssOrderbyAsc );
						break;
					}
					case Dungeon::Difficulty::Master:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyMasterOrderbyAsc );
						break;
					}
					case Dungeon::Difficulty::Hard:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyHardOrderbyAsc );
						break;
					}
					case Dungeon::Difficulty::Normal:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNormalOrderbyAsc );
						break;
					}
					case Dungeon::Difficulty::Easy:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyEasyOrderbyAsc );
						break;
					}
					default:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNoneOrderbyAsc );
						break;
					}
				}				
			}

			// ## �������� ���̵�(��������)
			if( m_pPacket->OrderType == Party::OrderType::TargetMapDifficultyDesc )
			{
				switch( pParty->GetDifficulty() )
				{
					case Dungeon::Difficulty::Abyss:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyAbyssOrderbyDesc );
						break;
					}
					case Dungeon::Difficulty::Master:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyMasterOrderbyDesc );
						break;
					}
					case Dungeon::Difficulty::Hard:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyHardOrderbyDesc );
						break;
					}
					case Dungeon::Difficulty::Normal:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNormalOrderbyDesc );
						break;
					}
					case Dungeon::Difficulty::Easy:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyEasyOrderbyDesc );
						break;
					}
					default:
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::DifficultyNoneOrderbyDesc );
						break;
					}
				}				
			}

			biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::LevelLimit )*pParty->GetMinLevel();

			if( m_pPacket->OrderType == Party::OrderType::LevelAsc )
			{
				biPoint -= (g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::LevelLimitOrderbyAsc )*pParty->GetMinLevel());
			}
			else if( m_pPacket->OrderType == Party::OrderType::LevelDesc )
			{
				biPoint += (g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::LevelLimitOrderbyDesc )*pParty->GetMinLevel());
			}

			// ## ���� ��ġ
			if( pParty->bIsInVillage() == true )
			{
				int iMapIndex = pParty->GetPartyMapIndex();

				if( iMapIndex == m_iMapIndex )
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::SameVillage );

				if( iMapIndex == 5 )	// ��������
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::ManaRidge );
				else if( iMapIndex == 1 )	// ������ Ÿ��
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PrairieTown );
				else if( iMapIndex == 8 ) // ĳ���� ����
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::CataractGateWay );
				else if( iMapIndex == 11 ) // ����Ʈ���̺�
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::SaintHaven );
				else if( iMapIndex == 15 ) // �δ�������
					biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::LotusMash );
			}

			int iMemberCount = 0;
			if( pParty->GetServerID() != g_Config.nManagedID )
			{
				iMemberCount = const_cast<CDNParty*>(pParty)->GetCurMemberCount();
			}
			else
			{
				iMemberCount = pParty->GetMemberCount();
			}

			// ## �ο���
			biPoint += (g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PartyMemberCount )*iMemberCount);

			// ## �ο���(��������)
			if( m_pPacket->OrderType == Party::OrderType::MemberCountAsc )
			{
				biPoint += (g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PartyMemberCountOrderbyAsc )*iMemberCount);
			}
			// ## �ο���(��������)
			else if( m_pPacket->OrderType == Party::OrderType::MemberCountDesc )
			{
				biPoint += (g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::PartyMemberCountOrderbyDesc )*iMemberCount);
			}

			if( pParty->GetTargetMapIndex() > 0 )
			{
				// ## ��ǥ ��������(�⺻��)
				biPoint += ((g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::TargetStage )*g_pDataManager->GetDungeonSortWeight( pParty->GetTargetMapIndex() )));

				// ## ��ǥ ��������(��������)
				if( m_pPacket->OrderType == Party::OrderType::TargetMapAsc )
				{
					biPoint += ((g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::TargetStageAsc )*g_pDataManager->GetDungeonSortWeight( pParty->GetTargetMapIndex() )));
				}
				// ## ��ǥ ��������(��������)
				else if( m_pPacket->OrderType == Party::OrderType::TargetMapDesc )
				{
					biPoint += ((g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::TargetStageDesc )*g_pDataManager->GetDungeonSortWeight( pParty->GetTargetMapIndex() )));
				}

				const TMapInfo* pMapInfo = g_pDataManager->GetMapInfo( pParty->GetTargetMapIndex() );
				if( pMapInfo && pMapInfo->MapType == GlobalEnum::MAP_DUNGEON )
				{
					// ## ���/�׽�Ʈ/��������/��Ÿ ��� ��ư ����ġ
					if( (pMapInfo->MapSubType == GlobalEnum::MAPSUB_NEST || pMapInfo->MapSubType == GlobalEnum::MAPSUB_NESTNORMAL) && m_pPacket->StageOrderType == Party::StageOrderType::TargetMapNestDesc )
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::NestToggle );
					}
					else if( pMapInfo->MapSubType == GlobalEnum::MAPSUB_NONE && m_pPacket->StageOrderType == Party::StageOrderType::TargetMapStageDesc )
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::StageToggle );
					}
					else if( (pMapInfo->MapSubType == GlobalEnum::MAPSUB_DARKLAIR || pMapInfo->MapSubType == GlobalEnum::MAPSUB_EVENT || pMapInfo->MapSubType == GlobalEnum::MAPSUB_CHAOSFIELD )
						&& m_pPacket->StageOrderType == Party::StageOrderType::TargetMapEtcDesc )
					{
						biPoint += g_pDataManager->GetPartySortWeight( Party::SortWeightIndex::EtcToggle ); 
					}
				}
			}

			const_cast<CDNParty*>(pParty)->SetSortPoint( biPoint );
		}

	private:

		const CSPartyListInfo* m_pPacket;
		const int m_iMapIndex;
	};
}

#endif // #if defined( PRE_PARTY_DB )
