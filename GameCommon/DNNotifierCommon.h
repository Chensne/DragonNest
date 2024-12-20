#pragma once

#pragma pack(push, 1)

namespace DNNotifier
{
	// 알림이 타입
	struct Type
	{
		enum eType
		{
			MainQuest		= 0,
			SubQuest,
#if defined(PRE_MOD_MISSION_HELPER)
			NormalMission,
#endif
			DailyMission,
			WeeklyMission,
#if defined(PRE_MOD_MISSION_HELPER)
			GuildWarMission,
			PCBangMission,
			GuildCommonMission,
			WeekendEventMission,
			WeekendRepeatMission,
#endif
#if defined(PRE_ADD_MONTHLY_MISSION)
			MonthlyMission,
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
			Max,
		};
	};

	// 최대 등록 개수
	struct RegisterCount
	{
		enum eRegisterCount
		{
			MainQuest		= 1,
			SubQuest		= 3,
#if defined(PRE_MOD_MISSION_HELPER)
			Mission			= 2,			
#else
			DailyMission	= 1,
			WeeklyMission	= 1,
#endif

			TotalQuest		= MainQuest + SubQuest,
#if defined(PRE_MOD_MISSION_HELPER)
			TotalMission	= Mission,
#else
			TotalMission	= DailyMission + WeeklyMission,
#endif
			Total			= TotalQuest + TotalMission,
		};
	};

	struct Data
	{
	public:

		Type::eType eType;
		int			iIndex;

		Data()
		{
			Clear();
		}

		void Clear()
		{
			eType	= Type::Max;
			iIndex	= -1;
		}

		bool Register( const Type::eType _eType, const int _iIndex )
		{
			eType	= _eType;
			iIndex	= _iIndex;
			return true;
		}

		bool IsEmpty()
		{
			return (eType == Type::Max);
		}

		bool IsEqual( const Type::eType _eType, const int _iIndex )
		{
			return (eType == _eType && iIndex == _iIndex);
		}
	};

	struct Repository
	{
	private:

		Data data[RegisterCount::Total];

	public:

		void Clear()
		{
			for( UINT i=0 ; i<RegisterCount::Total ; ++i )
				data[i].Clear();
		}

		void Register( const UINT uiIndex, Data& _data )
		{
			if( uiIndex < RegisterCount::Total )
				data[uiIndex] = _data;
		}

		bool RegisterMainQuest( const int iIndex )
		{
			// 중복검사
			if( data[0].IsEqual( Type::MainQuest, iIndex ) )
				return false;
			return data[0].Register( Type::MainQuest, iIndex );
		}

		bool RegisterSubQuest( const int iSlotIndex, const int iIndex )
		{
			if( iSlotIndex < RegisterCount::MainQuest || iSlotIndex >= RegisterCount::TotalQuest )
				return false;

			// 중복검사
			if( data[iSlotIndex].IsEqual( Type::SubQuest, iIndex ) )
				return false;
			return data[iSlotIndex].Register( Type::SubQuest, iIndex );
		}

#if defined(PRE_MOD_MISSION_HELPER)
		bool RegisterMission( const int iSlotIndex, const int iIndex, const Type::eType eType )
		{
			if( iSlotIndex < RegisterCount::TotalQuest -1 || iSlotIndex >= RegisterCount::Total )
				return false;

			// 중복검사
			if( data[iSlotIndex].IsEqual( eType, iIndex ) )
				return false;
			return data[iSlotIndex].Register( eType, iIndex );
		}
#else
		bool RegisterDailyMission( const int iIndex )
		{
			Data* pData = &data[ RegisterCount::TotalQuest ];
			// 중복검사
			if( pData->IsEqual( Type::DailyMission, iIndex ) )
				return false;
			return pData->Register( Type::DailyMission, iIndex );
		}

		bool RegisterWeeklyMission( const int iIndex )
		{
			Data* pData = &data[ RegisterCount::TotalQuest + RegisterCount::DailyMission ];
			// 중복검사
			if( pData->IsEqual( Type::WeeklyMission, iIndex ) )
				return false;
			return pData->Register( Type::WeeklyMission, iIndex );
		}
#endif

		const Data& GetData( const UINT uiSlotIndex )
		{
			return data[uiSlotIndex];
		}
	};
};

#pragma pack(pop)
