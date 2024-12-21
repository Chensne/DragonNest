
#pragma once

namespace LadderSystem
{
	class CRoomRepository;
	class CMatchingSystem;
	class CRoom;

	class CManager:public CSingleton<CManager>
	{
		public:

			CManager();
			~CManager();

			void Process( DWORD dwCurTick );

			void	SetChannelID( USHORT unChannelID );
			USHORT	GetChannelID();
			int		GetAvgMatchingTimeSec( MatchType::eCode MatchType );

			CRoomRepository*	GetRoomRepositoryPtr(){ return m_pRoomRepository; }
			CMatchingSystem*	GetMatchingSystemPtr();
			CRoom*				GetRoomPtr( const WCHAR* pwszCharName );
			CRoom*				GetRoomPtr( INT64 biRoomIndex );

			//
			bool		bIsValidPairRoom( INT64 biRoomIndex, INT64 biRoomIndex2 );
			void		ChangeRoomState( INT64 biRoomIndex, RoomState::eCode State, RoomStateReason::eCode Reason=RoomStateReason::ERROR_NONE );
			void		InsertPlayingList( CRoom* pRoom );
			void		DeletePlayingList( CRoom* pRoom );

			// EventHandler
			int			OnEnterChannel( CDNUserSession* pSession, MatchType::eCode MatchType );
			int			OnJoinRoom( CDNUserSession* pSession, CRoom* pRoom );
			int			OnLeaveChannel( CDNUserSession* pSession );
			int			OnMatching( CDNUserSession* pSession, bool bIsCancel );
			void		OnDisconnectUser( INT64 biCharDBID, WCHAR* pwszCharName );
			bool		OnReconnect( CDNUserSession* pSession );
			int			OnEnterObserver( CDNUserSession* pSession, CS_OBSERVER* pPacket );
			int			OnInviteUser( CDNUserSession* pInviteSession, const WCHAR* pwszCharName );
			void		OnInviteUserConfirm( const VIMALadderInviteConfirm* pPacket );
			int			OnKickOut( CDNUserSession* pSession, INT64 biCharacterDBID );

		private:

			CRoomRepository*	m_pRoomRepository;
			USHORT				m_unChannelID;		// 콜로세움 로비 채널ID
	};
};

