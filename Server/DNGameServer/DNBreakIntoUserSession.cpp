
#include "StdAfx.h"
#include "DNBreakIntoUserSession.h"
#include "DNGameRoom.h"
#include "DnGameTask.h"
#include "DnItemTask.h"
#include "DnPartyTask.h"
#include "GameSendPacket.h"
#include "PvPGameMode.h"
#include "MasterRewardSystem.h"
#include "DNMasterConnectionManager.h"
#include "DNWorldUserState.h"
#include "DNPvPGameRoom.h"

CDNBreakIntoUserSession::CDNBreakIntoUserSession( UINT uiUID, CDNRUDPGameServer* pServer, CDNGameRoom* pRoom )
: CDNUserSession( uiUID, pServer, pRoom ), m_bBreakIntoSession(true)
{
	_SetGameRoomState( _GAME_STATE_READY2CONNECT );
	m_uiNextGameRoomStateTick = pRoom->GetGameTick();
}

CDNBreakIntoUserSession::~CDNBreakIntoUserSession()
{

}

#if defined(PRE_ADD_MULTILANGUAGE)
void CDNBreakIntoUserSession::PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, char cSelectedLanguage, TMemberVoiceInfo * pInfo/* = NULL*/)
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
void CDNBreakIntoUserSession::PreInitializeUser(WCHAR * wszAccountName, UINT nAccountDBID, UINT nSessionID, INT64 biCharacterDBID, int iTeam, int nWorldID, int nVillageID, bool bTutorial, bool bAdult, char cPCBangGrade, TMemberVoiceInfo * pInfo/* = NULL*/)
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
{
#if defined(PRE_ADD_MULTILANGUAGE)
	CDNUserSession::PreInitializeUser( wszAccountName, nAccountDBID, nSessionID, biCharacterDBID, iTeam, nWorldID, nVillageID, bTutorial, bAdult, cPCBangGrade, cSelectedLanguage, pInfo );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CDNUserSession::PreInitializeUser( wszAccountName, nAccountDBID, nSessionID, biCharacterDBID, iTeam, nWorldID, nVillageID, bTutorial, bAdult, cPCBangGrade, pInfo );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

	bool bLessTeam = false;
	// PvP 방이고 PvP이벤트방일때 난입은 적은팀으로 무조건 설정한다.

	//rlkt 2016
	/*	if( GetGameRoom() && GetGameRoom()->bIsPvPRoom() )
	{
		if( static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetEventRoomIndex() > 0 )
			bLessTeam = true;
*/		if( static_cast<CDNPvPGameRoom*>(GetGameRoom())->GetRoomOptionBit()&PvPCommon::RoomOption::RandomTeam )
			bLessTeam = true;
//	}

	if( bLessTeam == true && iTeam != PvPCommon::Team::Observer )
	{
		int iATeam = 0;
		int iBTeam = 0;
		for( UINT i=0 ; i<GetGameRoom()->GetUserCount() ; ++i )
		{
			CDNUserSession* pSession = GetGameRoom()->GetUserData(i);
			if( pSession->GetTeam() == PvPCommon::Team::A )
				++iATeam;
			else if( pSession->GetTeam() == PvPCommon::Team::B )
				++iBTeam;
		}
		int iBreakIntoATeam = 0;
		int iBreakIntoBTeam = 0;
		GetGameRoom()->GetBreakIntoUserTeamCount( iBreakIntoATeam, iBreakIntoBTeam );
		iATeam += iBreakIntoATeam;
		iBTeam += iBreakIntoBTeam;

		SetTeam( iATeam > iBTeam ? PvPCommon::Team::B : PvPCommon::Team::A );
	}
}

bool CDNBreakIntoUserSession::BreakIntoProcess()
{
	ULONG uiCurTick	= m_pGameRoom->GetGameTick();
	unsigned long iCurTick2 = timeGetTime();

	switch( m_uiGameRoomState )
	{
		case _GAME_STATE_READY2CONNECT:
		{
			if( IsConnected() || (uiCurTick > m_uiNextGameRoomStateTick+WAIT_FOR_ANOTHER_USER_TIME_LIMIT) )
			{
				_SetGameRoomState( _GAME_STATE_CONNECT2CHECKAUTH );
				m_uiNextGameRoomStateTick	= timeGetTime();
			}
			break;
		}
		case _GAME_STATE_CONNECT2CHECKAUTH:
			{
				if (IsCertified())
				{
					_SetGameRoomState( _GAME_STATE_CONNECT2LOAD );
					m_uiNextGameRoomStateTick	= 0;
					break;
				}

				if(CHECKAUTHLIMITTERM < GetTickTerm(m_uiNextGameRoomStateTick, iCurTick2))
				{
					DetachConnection(L"_GAME_STATE_CONNECT2CHECKAUTH TimeOver");		// 추가 구현 필요 ??? (다음 상태로 실패처리를 표현하는 것이 아님 ???)
					break;
				}
			}
			break;

		case _GAME_STATE_CONNECT2LOAD:
		{
			if( m_uiNextGameRoomStateTick == 0 )
			{
				//if( !IsConnected() )
				//	DetachConnection();

				InitialUser();
				m_uiNextGameRoomStateTick = uiCurTick + WAIT_FOR_LOAD_TIME_LIMIT;
			}

			if( GetState() == SESSION_STATE_LOADED )
			{
				SendConnectedResult();

#if defined(_HSHIELD)
				if (GetAccountLevel() != AccountLevel_Developer){	// 내부 디버깅용(?)
					SendMakeRequest();	// CRC 요청
				}
#elif defined(_GPK)
				SendGPKCode();
				SendGPKAuthData();
#endif	// _HSHIELD

				_SetGameRoomState( _GAME_STATE_LOAD2SYNC );
				m_uiNextGameRoomStateTick	= uiCurTick + WAIT_FOR_LOAD_TIME_LIMIT;
			}
			break;
		}
		case _GAME_STATE_LOAD2SYNC:
		{
			if( GetState() == SESSION_STATE_READY_TO_SYNC )
			{
				CDnGameTask* pGameTask = m_pGameRoom->GetGameTask();
				CDnItemTask* pItemTask = m_pGameRoom->GetItemTask();
				if( !pGameTask || !pItemTask )
					DN_RETURN(false);

				if( !m_pGameRoom->AddPartyStruct( this, false ) )
				{
					DetachConnection( L"PartyMax" );
					return false;
				}
				if( m_pGameRoom->GetMasterRewardSystem() )
					m_pGameRoom->GetMasterRewardSystem()->RequestRefresh();

				pItemTask->InitializePlayerItem( this );
				pGameTask->OnInitializeBreakIntoActor( this, 0 );
				m_pGameRoom->CheckDiePlayer( this );

				//파티초대인경우 파티데이타의 동기를 맞춰준다.
				//SendBreakIntoUser 하지전에 인덱스를 맞춰줘야함!
				int nIdx = GetGameRoom()->AdjustBreakintoUser(GetCharacterName(), GetSessionID(), ERROR_NONE, false);
				if (nIdx >= 0)
				{
					SetPartyMemberIndex(nIdx);
					GetGameRoom()->SetPartyMemberIndex(nIdx, GetSessionID());
				}
				else if (GetGameRoom()->bIsOccupationMode())
				{
					//점령전인경우에는 마스터에서 인덱스 세팅해온다 아 미치도록 구질하다 흑
					int nPvPTeamSlotIndex = GetPartyMemberIndex();
					if (nPvPTeamSlotIndex >= 0)
					{
						if (GetGameRoom()->SetPartyMemberIndex(nPvPTeamSlotIndex, GetSessionID(), GetTeam()) == false)
							_DANGER_POINT();		//아 꼬였따아아아아아아아앙아ㅏ아아아아아
					}
					else
					{
						if (GetTeam() != PvPCommon::Team::Observer)
							_DANGER_POINT();		//옵저버가 아닌데 인덱스가 없다! 이럼 안데지!
					}
				}
				m_pGameRoom->SendBreakIntoUser( this );						// 기존 유저에게 난입유저 정보 보냄
				m_pGameRoom->SetSync2SyncStateTemplateMethod( 0, this );	// 난입 유저에게 기존유저 정보 보냄

				_SetGameRoomState( _GAME_STATE_SYNC2SYNC );
				m_uiNextGameRoomStateTick = uiCurTick + GOGO_SING_TO_PLAY_FOR_WAIT_TIME_LIMIT;
			}
			break;
		}
		case _GAME_STATE_SYNC2SYNC:
		{
			_SetGameRoomState( _GAME_STATE_SYNC2PLAY );
			break;
		}
		case _GAME_STATE_SYNC2PLAY:
		{
			// 난입성공
			if( GetState() == SESSION_STATE_READY_TO_PLAY )
			{
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
				m_pGameRoom->SendCompleteBreakIntoUser(this);
#endif
				SetMapIndex( m_pGameRoom->GetGameTask()->GetMapTableID() );
				m_pGameRoom->SyncMonster( this );
				m_pGameRoom->SyncNpc( this );
				m_pGameRoom->SyncProp( this );
				m_pGameRoom->OnSync2Sync( this );
				m_pGameRoom->SyncPlayer( this );
				m_pGameRoom->SyncGate( this );
#if defined( PRE_PARTY_DB )
				if (m_pGameRoom->GetGameType() == REQINFO_TYPE_PARTY)
				{
					m_pGameRoom->AddPartyMemberDB( this );
				}
#endif
#if defined( PRE_WORLDCOMBINE_PVP )
				if( m_pGameRoom->bIsWorldPvPRoom() )
				{
					m_pGameRoom->AddDBWorldPvPRoomMember( this );
				}
#endif
				// 난입에 의한 유저위치 동기화
				if( g_pMasterConnectionManager )
				{
					DNVector(std::wstring) vUserName;
					vUserName.push_back( GetCharacterName() );
					g_pMasterConnectionManager->SendWorldUserState( GetWorldSetID(), vUserName, GetMapIndex() );
				}
				if( g_pWorldUserState )
					g_pWorldUserState->UpdateUserState( GetCharacterName(), GetCharacterDBID(), _LOCATION_GAME, _COMMUNITY_NONE, -1, GetMapIndex() );

				SetSessionState( SESSION_STATE_GAME_PLAY );
				SetSecurityUpdateFlag( true );
				_SetGameRoomState( _GAME_STATE_PLAY );
				FlushPacketQueue();
				m_pGameRoom->OnRequestSyncStartMsg( this );

				// 난입성공알림
				m_pGameRoom->OnSuccessBreakInto( this );

				m_pGameRoom->SendPosition( this );
				m_pGameRoom->SendDropItemList( this );
				if( m_hActor )
				{
					m_hActor->InsertOctreeNode();
					m_hActor->SetOctreeUpdate( true );
				}

				if (GetGameRoom()->GetPartyIndex() > 0)
					m_pSession->SendJoinParty(ERROR_NONE, GetGameRoom()->GetPartyIndex(), GetGameRoom()->GetPartyName() );
				
				if( CDnPartyTask::IsActive(GetGameRoom()) )
					CDnPartyTask::GetInstance(GetGameRoom()).UpdateGateInfo();
				GetGameRoom()->SendRefreshParty(GetSessionID(), GetProfile()->bOpenPublic == true ? GetProfile() : NULL);
				GetGameRoom()->UpdateAppliedEventValue();
				GetGameRoom()->SendChatRoomInfo( this );
				CDnWorld *pWorld = GetGameRoom()->GetWorld();
				if( pWorld && pWorld->GetDragonNestType() != eDragonNestType::None )
				{
					CDNGameRoom::PartyStruct* pStruct = GetGameRoom()->GetPartyData( this );
					if( pStruct )
					{
						SendStartDragonNest( pWorld->GetDragonNestType(), pStruct->nUsableRebirthCoin );						
					}
				}
				m_bBreakIntoSession = false;			//<--요거 브레이스맨 하단에 위치시켜주세요~
				return true;
			}
			break;
		}
	}

	return false;
}
