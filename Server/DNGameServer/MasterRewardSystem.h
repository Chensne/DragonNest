
#pragma once

namespace MasterSystem
{
	class CRewardSystem:public TBoostMemoryPool<CRewardSystem>
	{
		public:

			struct TCountInfo
			{
				TCountInfo()
				{
					Clear();
				}

				TCountInfo( int iMaster, int iPupil, int iClassmate )
				{
					iMasterCount	= iMaster;
					iPupilCount		= iPupil;
					iClassmateCount	= iClassmate;
				}

				bool bIsAllZero()
				{
					return (iMasterCount == 0 && iPupilCount == 0 && iClassmateCount == 0);
				}

				bool operator == ( const TCountInfo& rhs )
				{
					return (iMasterCount == rhs.iMasterCount && iPupilCount == rhs.iPupilCount && iClassmateCount == rhs.iClassmateCount);
				}

				bool operator != ( const TCountInfo& rhs )
				{
					return (iMasterCount != rhs.iMasterCount || iPupilCount != rhs.iPupilCount || iClassmateCount != rhs.iClassmateCount);
				}

				void Clear()
				{
					iMasterCount	= 0;
					iPupilCount		= 0;
					iClassmateCount	= 0;
				}

				int iMasterCount;
				int iPupilCount;
				int iClassmateCount;
			};

			CRewardSystem( CDNGameRoom* pGameRoom );

			void				Update( CDNUserSession* pSession, TAGetMasterSystemCountInfo* pPacket );
			void				RequestRefresh();
			void				RequestDungeonClear( CDNUserSession* pSession );
			void				GetCountInfo( CDNUserSession* pSession, TCountInfo& CountInfo );
			const TCountInfo*	GetCountInfo( CDNUserSession* pSession );
			int					GetMasterSystemAddExp( CDNUserSession* pSession, float fExp, bool bStageClear);
			void				UpdateFavor( CDNUserSession* pSession, INT64 biPupilCharacterDBID, int nAddFavor);						

			bool	bIsDurabilityReward( CDNUserSession* pSession );			// 내구도 보상 받냐?
			bool	bIsFatigueReward( CDNUserSession* pSession );				// 피로도 보상 받냐?
			float	GetExpRewardRate( CDNUserSession* pSession );				// 경험치 보상 얼마받냐?
			float	GetTreasureBoxRewardRate( CDNUserSession* pSession );		// 보상상자 보상 얼마받냐?

		private:

			CDNGameRoom* m_pGameRoom;
			std::map<INT64,TCountInfo> m_mCountRepository;
	};
}
