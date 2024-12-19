
#include "Stdafx.h"
#include "DNLadderRoom.h"
#include "DNUserSessionManager.h"
#include "DNAuthManager.h"
#include "DNLadderSystemManager.h"
#include "DnLadderMatchingSystem.h"

extern TVillageConfig g_Config;

using namespace LadderSystem;

CRoom::CRoom( const INT64 biIndex, const MatchType::eCode MatchType, CDNUserSession* pSession )
:m_biRoomIndex(biIndex),m_biOpponentRoomIndex(0),m_MatchType(MatchType),m_RoomState(RoomState::None),m_dwRoomStateTick(timeGetTime())
#if defined(PRE_ADD_DWC)
,m_uiDWCTeamID(0), m_iHiddenDWCGradePoint(0)
#endif
{
	m_vUserInfo.reserve( MatchType );
	m_vUserInfo.push_back( SUserInfo(pSession,MatchType) );
	m_biLeaderCharDBID	= pSession->GetCharacterDBID();
	ClearGameServerInfo();

	m_iAvgHiddenGradePoint		= 0;
	m_dwUpdateProcessTick		= m_dwRoomStateTick;
	m_bForceMatching			= false;
	m_iGameModeTableID = 0;
	m_iStartMsgCount = 0;
}

void CRoom::Process( DWORD dwCurTick )
{
	if( dwCurTick-m_dwUpdateProcessTick < 500 )
		return;
	m_dwUpdateProcessTick = dwCurTick;

	switch( m_RoomState )
	{
		case RoomState::WaitMatching:
		{
			if( bIsMatchingReady() == false )
			{
				_ASSERT( m_vUserInfo.empty() == false );

#if defined(PRE_ADD_DWC)
				if(m_MatchType == MatchType::_3vs3_DWC || m_MatchType == MatchType::_3vs3_DWC_PRACTICE)
				{
					m_iAvgHiddenGradePoint = m_iHiddenDWCGradePoint;
#if defined( _WORK )
					std::cout << "[DWC Ladder] RoomIndex:" << m_biRoomIndex << " 매칭준비완료!!" << std::endl;
#endif // #if defined( _WORK )
					break;
				}
#endif	//#if defined(PRE_ADD_DWC)

				int		iSum	= 0;
				bool	bCheck	= true;

				for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
				{
					CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
					if( pSession == NULL )
					{
						bCheck = false;
						break;
					}
					// 직업 체크 우서 안하기로 함 주석 처리.
#if 0
					if( m_MatchType == MatchType::_1vs1 )
					{
						// 직업별 정보 받았는지 체크
						if( pSession->GetPvPLadderScoreInfoPtrByJob()->bInit == false  )
						{
							bCheck = false;
							break;
						}
					}
#endif

					iSum += pSession->GetPvPLadderScoreInfoPtr()->GetHiddenGradePoint( m_MatchType );
				}

				if( bCheck == true && m_vUserInfo.empty() == false )
				{
					m_iAvgHiddenGradePoint = iSum / static_cast<int>(m_vUserInfo.size());
#if defined( _WORK )
					std::cout << "[Ladder] RoomIndex:" << m_biRoomIndex << " 매칭준비완료!!" << std::endl;
#endif // #if defined( _WORK )
				}
			}
			break;
		}
	}
}

bool CRoom::JoinUser( CDNUserSession* pJoinSession )
{
	// 기존유저에게 JoinUser 정보 보냄
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
		if( pSession == NULL )
			continue;

		pSession->SendLadderNotifyJoinUser( pJoinSession->GetCharacterDBID(), pJoinSession->GetCharacterName(), pJoinSession->GetPvPLadderScoreInfoPtr()->GetGradePoint(m_MatchType), pJoinSession->GetUserJob() );
	}

	m_vUserInfo.push_back( SUserInfo(pJoinSession,m_MatchType) );

	// JoinUser 에게 Room정보 보냄
	RefreshUserInfo( pJoinSession );

	// JoinUser 에게 방장 정보 보냄
	const WCHAR* pLeaderName = GetCharName( GetLeaderCharDBID() );
	if( pLeaderName )
		pJoinSession->SendLadderNotifyLeader( pLeaderName );
	return true;
}

// 나를 제외한 유저의 정보를 갱신해서 보내준다.
void CRoom::RefreshUserInfo( CDNUserSession* pSession )
{
	SC_REFRESH_USERINFO TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		if( TxPacket.cCount >= _countof(TxPacket.sUserInfoArr) )
		{
			_ASSERT(0);
			break;
		}

		if( m_vUserInfo[i].biCharDBID == pSession->GetCharacterDBID() )
			continue;
		TxPacket.sUserInfoArr[TxPacket.cCount].biCharDBID	= m_vUserInfo[i].biCharDBID;
		TxPacket.sUserInfoArr[TxPacket.cCount].cJob			= m_vUserInfo[i].cJob;
		TxPacket.sUserInfoArr[TxPacket.cCount].iGradePoint	= m_vUserInfo[i].iGradePoint;
		_wcscpy( TxPacket.sUserInfoArr[TxPacket.cCount].wszCharName, _countof(TxPacket.sUserInfoArr[TxPacket.cCount].wszCharName), m_vUserInfo[i].wszCharName, (int)wcslen(m_vUserInfo[i].wszCharName) );
		++TxPacket.cCount;
	}

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.sUserInfoArr)+TxPacket.cCount*sizeof(TxPacket.sUserInfoArr[0]);
	pSession->AddSendData( SC_PVP, ePvP::SC_LADDER_REFRESH_USERINFO, reinterpret_cast<char*>(&TxPacket), iSize );

#if defined( _WORK )
	for( int i=0 ; i<TxPacket.cCount ; ++i )
	{
		WCHAR wszBuf[MAX_PATH];
		wsprintf( wszBuf, L"[LADDER] 유저정보 CharName:%s CharDBID:%I64d GradePoint:%d", TxPacket.sUserInfoArr[i].wszCharName, TxPacket.sUserInfoArr[i].biCharDBID, TxPacket.sUserInfoArr[i].iGradePoint );
		pSession->SendDebugChat( wszBuf );
	}
#endif // #if defined( _WORK )
}

void CRoom::UpdateUserInfo( CDNUserSession* pSession )
{
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		if( m_vUserInfo[i].biCharDBID == pSession->GetCharacterDBID() )
		{
			m_vUserInfo[i].iGradePoint = pSession->GetPvPLadderScoreInfoPtr()->GetGradePoint( m_MatchType );
			break;
		}
	}

	// Refresh
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
		if( pSession == NULL )
			continue;

		RefreshUserInfo( pSession );
	}
}

bool CRoom::OutUser( INT64 biCharDBID, const WCHAR* pwszCharName, Reason::eCode Type )
{
	bool bIsOut = false;

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		if( m_vUserInfo[i].biCharDBID == biCharDBID )
		{
			bIsOut = true;
			m_vUserInfo.erase( m_vUserInfo.begin()+i );
			break;
		}
	}

	if( bIsOut == false )
		return false;

	if( m_biLeaderCharDBID == biCharDBID )
		m_biLeaderCharDBID = 0;

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
		if( pSession == NULL )
			continue;
		
		pSession->SendLadderNotifyLeaveUser( biCharDBID, pwszCharName, Type );
	}
	if( GetRoomState()&(RoomState::WaitMatching|RoomState::Matched|RoomState::Starting) )
		ChangeRoomState( RoomState::WaitUser );

	return true;
}

void CRoom::AdjustNewLeader()
{
	bool bAdjust = true;

	if( GetLeaderCharDBID() > 0 )
		bAdjust = false;

	if( bAdjust == true )
	{
		if( m_vUserInfo.empty() )
			return;

		m_biLeaderCharDBID = (*m_vUserInfo.begin()).biCharDBID;

		for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
		{
			CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
			if( pSession == NULL )
				continue;

			pSession->SendLadderNotifyLeader( (*m_vUserInfo.begin()).wszCharName );
		}
	}

	// RoomState 변경
	switch( m_RoomState )
	{
		case RoomState::WaitMatching:	// 매칭대기중
		case RoomState::Matched:		// 매치됨
		{
			ChangeRoomState( RoomState::WaitUser );
			break;
		}
	}
}

void CRoom::ChangeRoomState( RoomState::eCode State, RoomStateReason::eCode Reason/*=RoomStateReason::ERROR_NONE*/ )
{
	if( m_RoomState == State )
		return;

	RoomState::eCode	PrevRoomState = m_RoomState;
	DWORD				dwElapsedTick = GetRoomStateElapsedTick();

	m_RoomState			= State;
	m_dwRoomStateTick	= timeGetTime();

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pSession = g_pUserSessionManager->FindUserSessionByName( m_vUserInfo[i].wszCharName );
		if( pSession == NULL )
			continue;

		pSession->SendLadderNotifyRoomState( m_RoomState, Reason );
	}

	switch( PrevRoomState )
	{
		case RoomState::Playing:
		{
			CManager::GetInstance().DeletePlayingList( this );
			break;
		}
	}

	switch( m_RoomState )
	{
		case RoomState::WaitUser:
		{
			ClearOpponentRoomIndex();
			break;
		}
		case RoomState::Matched:
		{
			m_bForceMatching = false;
			SetStartMsgCount( LadderSystem::Common::StartMsgCount+1 );
			
			CMatchingSystem* pMatchingSystem = CManager::GetInstance().GetMatchingSystemPtr();
			if( pMatchingSystem )
				pMatchingSystem->AddMatchingTime( m_MatchType, dwElapsedTick/1000 );
			break;
		}
		case RoomState::Playing:
		{
			CManager::GetInstance().InsertPlayingList( this );
			break;
		}
		case RoomState::WaitMatching:
		{
			m_iAvgHiddenGradePoint = 0;
			SendMatchingAvgSec();
			break;
		}
	}
}

bool CRoom::bIsValidOpponentRoom( CRoom* pLadderRoom, int CheckState/*=RoomState::None*/ )
{
	if( pLadderRoom == NULL )
		return false;

	if( CheckState > RoomState::None )
	{
		if( (GetRoomState()&CheckState) == false )
			return false;
	}

	if( GetOpponentRoomIndex() == pLadderRoom->GetRoomIndex() )
		return true;

	return false;
}

bool CRoom::bIsValidUser( CDNUserSession* pSession )
{
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		if( m_vUserInfo[i].biCharDBID == pSession->GetCharacterDBID() )
			return true;
	}

	return false;
}

void CRoom::SendPvPGameModeTableID( int iTableID, bool bSet/*=true*/ )
{
	if( bSet )
		m_iGameModeTableID = iTableID;

	SC_NOTIFY_GAMEMODE_TABLEID TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iTableID = iTableID;

	BroadCast( SC_PVP, ePvP::SC_LADDER_NOTIFY_GAMEMODE_TABLEID, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CRoom::SendAllRoomID( USHORT wGameID, int nRoomID )
{
	m_iGameServerID	= wGameID;
	m_iGameRoomID	= nRoomID;

	if( g_pUserSessionManager )
	{
		for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
		{
			CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
			if( pUserSession == NULL )
			{
				_DANGER_POINT();
				continue;
			}
			pUserSession->m_GameTaskType	= GameTaskType::PvP;
			pUserSession->m_cReqGameIDType	= REQINFO_TYPE_LADDER;
			pUserSession->SetGameID(wGameID);
			pUserSession->SetRoomID(nRoomID);
		}
	}
	else
		_DANGER_POINT();

}

void CRoom::SendAllReadyToGame( ULONG nIP, USHORT nPort, USHORT nTcpPort )
{
	if( g_pUserSessionManager == NULL )
	{
		_DANGER_POINT();
		return;
	}

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
		if( pUserSession == NULL )
		{
			_DANGER_POINT();
			continue;
		}

		pUserSession->m_nGameServerIP = nIP;
		pUserSession->m_nGameServerPort = nPort;
		pUserSession->m_nGameServerTcpPort = nTcpPort;
		pUserSession->m_eUserState = STATE_READYTOGAME;
		pUserSession->m_biCertifyingKey = g_pAuthManager->GetCertifyingKey();
		DN_ASSERT(0 != pUserSession->m_biCertifyingKey,	"Invalid!");	// 인증키가 0 이 생성되면 않됨 !!! (없음 의미)

		g_pAuthManager->QueryStoreAuth(SERVERTYPE_VILLAGE, pUserSession);
	}
}

void CRoom::SetStartMsgCount( int iCount )
{
	m_iStartMsgCount = iCount;

	if( m_iStartMsgCount == Common::StartMsgCount )
		ChangeRoomState( RoomState::Starting );

	if( m_iStartMsgCount > 0 && m_iStartMsgCount <= Common::StartMsgCount )
		SendStartMsgCount();
}

void CRoom::SendStartMsgCount()
{
	SCPVP_STARTMSG TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cSec = static_cast<BYTE>(m_iStartMsgCount);

	BroadCast( SC_PVP, ePvP::SC_STARTMSG, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CRoom::SendMatchingAvgSec()
{
	LadderSystem::SC_MATCHING_AVGSEC TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iSec = CManager::GetInstance().GetAvgMatchingTimeSec( m_MatchType );

	BroadCast( SC_PVP, ePvP::SC_LADDER_MATCHING_AVGSEC, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}

void CRoom::BroadCast( int iMainCmd, int iSubCmd, char* pData, int iLen )
{
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
		if( pUserSession == NULL )
			continue;

		pUserSession->Send( iMainCmd, iSubCmd, pData, iLen );
	}
}

void CRoom::ClearGameServerInfo()
{
	m_iGameServerID = 0;
	m_iGameRoomID = 0;
}

void CRoom::SendChat( eChatType eType, int cLen, const WCHAR *pwszCharacterName, const WCHAR *pwszChatMsg, int nRet/* = ERROR_NONE*/ )
{
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
		if( pUserSession == NULL )
			continue;

		pUserSession->SendChat( eType, cLen, pwszCharacterName, pwszChatMsg, NULL, nRet );
	}
}

void CRoom::GetMatchingSection( std::vector<int>& vData )
{
	vData.clear();

#if !defined( _FINAL_BUILD )
	if( bIsMatchingReady() == false )
	{
		_ASSERT(0);
		return;
	}
#endif // #if !defined( _FINAL_BUILD )

	int iGap = static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_IncreaseHiddenPointPerSec )*(GetRoomStateElapsedTick()/1000));
	
	int iMin = m_iAvgHiddenGradePoint-iGap;
	int iMax = m_iAvgHiddenGradePoint+iGap;

	if( iMin < LadderSystem::Stats::MinGradePoint )
		iMin = LadderSystem::Stats::MinGradePoint;
	if( iMax < LadderSystem::Stats::MinGradePoint )
		iMax = LadderSystem::Stats::MinGradePoint;

	int iStart	= iMin/static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_SectionRange ));
	int iEnd	= iMax/static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PvPLadder_SectionRange ));
	for( int i=iStart ; i<=iEnd ; ++i )
		vData.push_back(i);
}

bool CRoom::bIsFullUser()
{
	BOOST_STATIC_ASSERT( LadderSystem::MatchType::_1vs1 == 1 );
	BOOST_STATIC_ASSERT( LadderSystem::MatchType::_2vs2 == 2 );
	BOOST_STATIC_ASSERT( LadderSystem::MatchType::_3vs3 == 3 );
	BOOST_STATIC_ASSERT( LadderSystem::MatchType::_4vs4 == 4 );	

#if defined(PRE_ADD_DWC)
	return m_vUserInfo.size() == LadderSystem::GetNeedTeamCount(m_MatchType);
#else
	return m_vUserInfo.size() == m_MatchType;
#endif
}

bool CRoom::bIsAllConnectUser()
{
	_ASSERT( bIsFullUser() == true );

	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
		if( pUserSession == NULL )
			return false;
		if( pUserSession->bIsLadderUser() == false )
			return false;
	}

	return true;
}

const WCHAR* CRoom::GetCharName( INT64 biCharDBID )
{
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		if( m_vUserInfo[i].biCharDBID == biCharDBID )
			return m_vUserInfo[i].wszCharName;
	}

	return NULL;
}

int CRoom::GetAvgGradePoint()
{
	if( m_vUserInfo.empty() )
	{
		_ASSERT(0);
		return 0;
	}

	int iSum = 0;
	int iCount = 0;
	for( UINT i=0 ; i<m_vUserInfo.size() ; ++i )
	{
		CDNUserSession* pUserSession = g_pUserSessionManager->FindUserSessionByAccountDBID( m_vUserInfo[i].uiAccountDBID );
		if( pUserSession )
		{
			iSum += pUserSession->GetPvPLadderScoreInfoPtr()->GetGradePoint( m_MatchType );
			++iCount;
		}
	}

	if( iCount <= 0 )
	{
		_ASSERT(0);
		return 0;
	}

	return iSum/iCount;
}

void CRoom::AddInviteUser( const WCHAR* pwszCharName )
{
	std::map<std::wstring,DWORD>::iterator itor = m_mInviteUser.find( pwszCharName );
	if( itor == m_mInviteUser.end() )
		m_mInviteUser.insert( std::make_pair(pwszCharName,timeGetTime()) );
	else
		(*itor).second = timeGetTime();
}

bool CRoom::bIsInviteUser( const WCHAR* pwszCharName )
{
	std::map<std::wstring,DWORD>::iterator itor = m_mInviteUser.find( pwszCharName );
	if( itor == m_mInviteUser.end() )
		return false;

	if( timeGetTime()-(*itor).second > LadderSystem::Common::InviteValidTick )
		return false;

	return true;
}

void CRoom::DelInviteUser( const WCHAR* pwszCharName )
{
	m_mInviteUser.erase( pwszCharName );
}

bool CRoom::bIsInviting()
{
	for( std::map<std::wstring,DWORD>::iterator itor=m_mInviteUser.begin() ; itor!=m_mInviteUser.end() ; ++itor )
	{
		if( bIsInviteUser( (*itor).first.c_str() ) == true )
			return true;
	}

	return false;
}

#if defined(PRE_ADD_DWC)
void CRoom::SetDWCInfo(UINT nTeamID, int nHiddenDWCPoint)
{
	m_uiDWCTeamID = nTeamID;
	m_iHiddenDWCGradePoint = nHiddenDWCPoint;
}

void CRoom::SendLadderMatching(int nRet, bool bIsCancel)
{
	LadderSystem::SC_LADDER_MATCHING TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iRet		= nRet;
	TxPacket.bIsCancel	= bIsCancel;

	BroadCast( SC_PVP, ePvP::SC_LADDER_MATCHING, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
}
#endif
