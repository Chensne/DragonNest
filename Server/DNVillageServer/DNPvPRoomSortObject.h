
#pragma once

#include "DNPvPRoom.h"
#include "DNUserSession.h"

namespace PvPUserSort
{
	class OrderByPvPLevelAsc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. PvPLevel �������� Sort
				if( lhs->GetPvPData()->cLevel >= rhs->GetPvPData()->cLevel )
					return false;
				return true;
			}
	};

	class OrderByPvPLevelDesc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. PvPLevel �������� Sort
				if( lhs->GetPvPData()->cLevel <= rhs->GetPvPData()->cLevel )
					return false;
				return true;
			}
	};

	class OrderByJobAsc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. Job �������� Sort
				if( lhs->GetUserJob() >= rhs->GetUserJob() )
					return false;
				return true;
			}
	};

	class OrderByJobDesc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. Job �������� Sort
				if( lhs->GetUserJob() <= rhs->GetUserJob() )
					return false;
				return true;
			}
	};

	class OrderByLevelAsc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. Level �������� Sort
				if( lhs->GetLevel() >= rhs->GetLevel() )
					return false;
				return true;
			}
	};

	class OrderByLevelDesc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. Level �������� Sort
				if( lhs->GetLevel() <= rhs->GetLevel() )
					return false;
				return true;
			}
	};

	class OrderByCharNameAsc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. CharName �������� Sort
				if( wcscmp( lhs->GetCharacterName(), rhs->GetCharacterName() ) >= 0 )
					return false;
				return true;
			}
	};

	class OrderByCharNameDesc
	{
		public:		
			bool operator()( CDNUserSession* lhs, CDNUserSession* rhs )
			{
				// 1. CharName �������� Sort
				if( wcscmp( lhs->GetCharacterName(), rhs->GetCharacterName() ) <= 0 )
					return false;
				return true;
			}
	};
}

namespace PvPRoomSort
{
	class RoomIndexOrderByAsc
	{
	public:
		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )
		{
			// �ý��� ��������� ������ �̺�Ʈ�뺸�� �켱!
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;			
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;			
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);

		}

	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;			
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif
			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2. RoomIndex ������������ Sort
			if( lhs->GetIndex() > rhs->GetIndex() )
				return false;

			return true;
		}
	};

	class RoomIndexOrderByDesc
	{
	public:
		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif
			// �ý��� ��������� ������ �̺�Ʈ�뺸�� �켱!
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);
		}
	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;	
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif
			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2.RoomIndex ������������ Sort
			if( lhs->GetIndex() < rhs->GetIndex() )
				return false;

			return true;
		}
	};

	class NumOfPlayerOrderByAsc
	{
	public:
		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif	// �ý��� ��������� ������ �̺�Ʈ�뺸�� �켱!
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);
		}
	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;	
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif
			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2. ���ִ��ο� ������������ Sort
			if( lhs->GetMaxUser() > rhs->GetMaxUser() )
				return false;
			// 3. �����ο� ������������ Sort
			else if( lhs->GetMaxUser() == rhs->GetMaxUser() )
			{
				if( lhs->GetCurUser() < rhs->GetCurUser() )
					return false;
				// 4. RoomIndex ������������ Sort
				else if( lhs->GetCurUser() == rhs->GetCurUser() )
				{
					if( lhs->GetIndex() > rhs->GetIndex() )
						return false;
				}
			}

			return true;
		}
	};

	class NumOfPlayerOrderByDesc
	{
	public:
		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )		
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);
		}
	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;	
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif
			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2. ���ִ��ο� ������������ Sort
			if( lhs->GetMaxUser() < rhs->GetMaxUser() )
				return false;
			// 3. �����ο� ������������ Sort
			else if( lhs->GetMaxUser() == rhs->GetMaxUser() )
			{
				if( lhs->GetCurUser() < rhs->GetCurUser() )
					return false;
				// 4. RoomIndex ������������ Sort
				else if( lhs->GetCurUser() == rhs->GetCurUser() )
				{
					if( lhs->GetIndex() > rhs->GetIndex() )
						return false;
				}
			}
			return true;
		}
	};

	class GameModeOrder
	{
	public:
		GameModeOrder( const UINT uiGameMode ):m_uiGameMode(uiGameMode)
		{
		}

		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);
		}
	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;		
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif
			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2. ���͵� GameMode �� Sort
			if( lhs->GetGameMode() == m_uiGameMode && rhs->GetGameMode() != m_uiGameMode )
				return true;
			else if( lhs->GetGameMode() != m_uiGameMode && rhs->GetGameMode() == m_uiGameMode )
				return false;
			else
			{
				// 3.GameMode ������������ Sort
				if( lhs->GetGameMode() > rhs->GetGameMode() )
					return false;
				else if( lhs->GetGameMode() == rhs->GetGameMode() )
				{
					// 4. RoomIndex ������������ Sort
					if( lhs->GetIndex() > rhs->GetIndex() )
						return false;
				}
			}

			return true;
		}

	private:
		UINT m_uiGameMode;
	};

	class MapOrder
	{
	public:
		MapOrder( const UINT uiMapIndex ):m_uiMapIndex(uiMapIndex)
		{
		}

		bool operator()( CDNPvPRoom* lhs, CDNPvPRoom* rhs )
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() > 0 || rhs->GetWorldPvPRoomType() > 0)
				return CompareCondition(lhs, rhs);
#endif
#if defined( PRE_PVP_GAMBLEROOM )
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() > 0)
				return CompareCondition(lhs, rhs);
			if( lhs->GetGambleRoomType() > 0 && rhs->GetGambleRoomType() == 0)
				return true;
			if( lhs->GetGambleRoomType() == 0 && rhs->GetGambleRoomType() > 0)
				return false;
#endif
			if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() == 0 && lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == false &&
				lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == false)
				return CompareCondition(lhs, rhs);

			if( lhs->IsExtendObserver() == false && rhs->IsExtendObserver() == true )
				return false;
			else if( lhs->IsExtendObserver() == true && rhs->IsExtendObserver() == false )
				return true;
			else if( lhs->IsGuildWarSystem() == false && rhs->IsGuildWarSystem() == true )
				return false;
			else if( lhs->IsGuildWarSystem() == true && rhs->IsGuildWarSystem() == false )
				return true;
			else if( lhs->GetEventRoomIndex() == 0 && rhs->GetEventRoomIndex() > 0 )
				return false;

			return CompareCondition(lhs, rhs);
		}
	private:
		bool CompareCondition(CDNPvPRoom* lhs, CDNPvPRoom* rhs)
		{
#if defined( PRE_WORLDCOMBINE_PVP )
			if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom)
				return true;			
			else if( lhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::MissionRoom && rhs->GetWorldPvPRoomType() == WorldPvPMissionRoom::Common::GMRoom)
				return false;			
			else if( lhs->GetWorldPvPRoomType() == 0 && rhs->GetWorldPvPRoomType() > 0)
				return false;	
			else if( lhs->GetWorldPvPRoomType() > 0 && rhs->GetWorldPvPRoomType() == 0)
				return true;			
#endif

			// 1. �������� ���η� Sort
			if( lhs->bIsEmptySlot() && !rhs->bIsEmptySlot() )
				return true;
			else if( !lhs->bIsEmptySlot() && rhs->bIsEmptySlot() )
				return false;

			// 2. ���͵� MapIndex �� Sort
			if( lhs->GetMapIndex() == m_uiMapIndex && rhs->GetMapIndex() != m_uiMapIndex )
				return true;
			else if( lhs->GetMapIndex() != m_uiMapIndex && rhs->GetMapIndex() == m_uiMapIndex )
				return false;
			else
			{
				// 3.MapIndex ������������ Sort
				if( lhs->GetMapIndex() > rhs->GetMapIndex() )
					return false;
				else if( lhs->GetMapIndex() == rhs->GetMapIndex() )
				{
					// 4. RoomIndex ������������ Sort
					if( lhs->GetIndex() > rhs->GetIndex() )
						return false;
				}
			}

			return true;
		}
	private:
		UINT m_uiMapIndex;
	};
}
