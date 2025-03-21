
#pragma once

#include <./Boost/tuple/tuple.hpp>

namespace LadderSystem
{
	const int MAX_GRADEPOINT_RANGE = 6;

	// Stats
	class CStats:public TBoostMemoryPool<CStats>
	{
		public:

			CStats( CDNUserSession* pSession, MatchType::eCode MatchType );

			bool	bIsInit(){ return m_bInit; }
			void	SetVSJobCode( int iJob ){ m_iVSJobCode=iJob; };
			BYTE	GetVSJobCode(){ return static_cast<BYTE>(m_iVSJobCode); }
			int		GetGradePoint(){ return m_iGradePoint; }
			int		GetHiddenGradePoint(){ return m_iHiddenGradePoint; }
			
			void	ConvertKillResult( std::vector<LadderKillResult>& vData );

			void	OnRecvLadderScore( const TAGetListPvPLadderScore* pPacket );
			void	OnDie( DnActorHandle hActor, DnActorHandle hHitter );
			void	OnKill( DnActorHandle hActor, DnActorHandle hDieActor );

			struct SKillDeathCount
			{
			public:
				int iKillCount;
				int iDeathCount;
				SKillDeathCount():iKillCount(0),iDeathCount(0){}
				SKillDeathCount( int KillCount, int DeathCount ):iKillCount(KillCount),iDeathCount(DeathCount){}
			};

		private:

			const INT64						m_biCharacterDBID;
			const MatchType::eCode			m_MatchType;
			bool							m_bInit;
			int								m_iGradePoint;			// 평점
			int								m_iHiddenGradePoint;	// 가평점
			MatchResult::eCode				m_MatchResult;			// 매치결과
			std::map<int,SKillDeathCount>	m_mKillDeathCount;		// <Job/SKillDeathCount>
			int								m_iVSJobCode;			// MatchType::_1vs1 일경우에만 쓰임
	};

#if defined(PRE_ADD_DWC)
	struct SResult
	{
		int iAddGradePoint;
		int iResultGradePoint;

		SResult():iAddGradePoint(0),iResultGradePoint(0){}
		SResult( int iAdd, int iResult ):iAddGradePoint(iAdd),iResultGradePoint(iResult){}
	};
#endif

	// Repository
	class CStatsRepository:public TBoostMemoryPool<CStatsRepository>
	{
		public:
		
			CStatsRepository( CDNGameRoom* pGameRoom );
			~CStatsRepository();
			bool bInitialize();
			bool QueryUpdateResult( CDNUserSession* pSession, UINT uiWinTeam, PvPCommon::QueryUpdatePvPDataType::eCode Type );

			void OnDie( DnActorHandle hActor, DnActorHandle hHitter );
			void OnRecvLadderScore( INT64 biCharacterDBID, int iTeam, const TAGetListPvPLadderScore* pPacket );
			void OnSetPlayState();

			bool	bIsValidLadderTeam( int iTeam );
			int		GetAddGradePoint( INT64 biCharacterDBID );
			int		GetResultGradePoint( INT64 biCharacterDBID );

#if defined(PRE_ADD_DWC)
#else
			struct SResult
			{
				int iAddGradePoint;
				int iResultGradePoint;

				SResult():iAddGradePoint(0),iResultGradePoint(0){}
				SResult( int iAdd, int iResult ):iAddGradePoint(iAdd),iResultGradePoint(iResult){}
			};
#endif

		private:

			CDNGameRoom*					m_pGameRoom;
			MatchType::eCode				m_MatchType;
			std::map<INT64,CStats*>			m_mRepository;
			std::map<int,std::vector<int>>	m_mGradePointSum;
			int								m_iTeamGradePointAvg[PvPCommon::TeamIndex::Max];
			std::map<INT64,SResult>			m_mResult;

			//#############################################
			// 평점 계산용
			//#############################################

			boost::tuple<float,float,float> S_Repository[MAX_GRADEPOINT_RANGE];
			boost::tuple<float,float,float> HiddenS_Repository[MAX_GRADEPOINT_RANGE];
			boost::tuple<float,float,float,float> K_Repository[MAX_GRADEPOINT_RANGE];
			boost::tuple<float,float,float,float> HiddenK_Repository[MAX_GRADEPOINT_RANGE];

	};

#if defined(PRE_ADD_DWC)
	class CDWCStats
	{
	public:
		CDWCStats() : m_nTeamID(0), m_MatchType(LadderSystem::MatchType::None), m_iGradePoint(0), m_iHiddenGradePoint(0),
			m_MatchResult(MatchResult::None), m_nOppositeTeamID(0) {}

		void SetMatchType(MatchType::eCode MatchType);
		void OnRecvDWCScore(UINT nTeamID, TDWCTeam* Info, UINT nOppositeTeamID);
		UINT GetTeamID(){ return m_nTeamID; }
		UINT GetOppositeTeamID(){ return m_nOppositeTeamID; }
		int GetGradePoint(){ return m_iGradePoint; }
		int GetHiddenGradePoint(){ return m_iHiddenGradePoint; }
		void OnRecvLadderScore( const TAGetListPvPLadderScore* pPacket );

	private:

		UINT m_nTeamID;
		UINT m_nOppositeTeamID;		// 상대편 팀 ID
		int m_iGradePoint;			// 평점
		int m_iHiddenGradePoint;	// 가평점
		MatchType::eCode m_MatchType;
		MatchResult::eCode	m_MatchResult; // 매치결과
		bool m_bInit;
	};

	class CDWCStatsRepository:public TBoostMemoryPool<CDWCStatsRepository>
	{
	public:
		CDWCStatsRepository( CDNGameRoom* pGameRoom );

		bool bInitialize();
 		bool QueryUpdateResult( UINT uiWinTeam, PvPCommon::QueryUpdatePvPDataType::eCode Type );
		void OnRecvDWCScore(UINT nATeamID, TDWCTeam* ATeamInfo, UINT nBTeamID, TDWCTeam* BTeamInfo);

		bool bIsValidLadderTeam( int iTeam );
		int GetAddGradePoint( int iTeam );
		int GetResultGradePoint( int iTeam );		

	private:
		CDNGameRoom* m_pGameRoom;
		MatchType::eCode m_MatchType;		
		CDWCStats m_DWCStat[2];	//팀이 두개니까 두개
		SResult m_Result[2];

		//#############################################
		// 평점 계산용
		//#############################################

		boost::tuple<float,float,float> S_Repository[MAX_GRADEPOINT_RANGE];
		boost::tuple<float,float,float> HiddenS_Repository[MAX_GRADEPOINT_RANGE];
		boost::tuple<float,float,float,float> K_Repository[MAX_GRADEPOINT_RANGE];
		boost::tuple<float,float,float,float> HiddenK_Repository[MAX_GRADEPOINT_RANGE];
	};	
#endif

	// Factory
	class CStatsFactory
	{
		public:

			static CStatsRepository* CreateRepository( CDNGameRoom* pGameRoom );
#if defined(PRE_ADD_DWC)
			static CDWCStatsRepository* CreateDWCRepository( CDNGameRoom* pGameRoom );
#endif			
	};
};

