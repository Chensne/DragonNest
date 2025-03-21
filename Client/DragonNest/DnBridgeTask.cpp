#include "StdAfx.h"
#include "DnBridgeTask.h"
#include "ClientSessionManager.h"
#include "DnInterface.h"
#include "DnInterfaceDlgID.h"
#include "DnMessageBox.h"
#include "SystemSendPacket.h"
#include "DnVillageTask.h"
#include "DnGameTask.h"
#include "DnPvPGameTask.h"
#include "DnDLGameTask.h"
#include "TaskManager.h"
#include "GameSendPacket.h"
#include "DnMainFrame.h"
#include "DnLocalPlayerActor.h"
#include "DnVillageTask.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#include "DnQuestTask.h"
#include "DnCommonTask.h"
#include "DnFriendTask.h"
#include "DnGuildTask.h"
#include "DnSkillTask.h"
#include "DnTradeTask.h"
#include "DnNestInfoTask.h"
#include "DnMasterTask.h"
#include "DNIsolate.h"
#include "DnMissionTask.h"
#include "DnAppellationTask.h"
#include "DnGameOptionTask.h"
#include "DnTimeEventTask.h"
//blondymarry start
#include "DnPVPLobbyVillageTask.h"
//blondymarry end
#include "GameOption.h"
#include "DnRadioMsgTask.h"
#include "DNGestureTask.h"
#include "DNVoiceChatTask.h"
#include "DnGameTipTask.h"
#include "DnRestraintTask.h"
#include "DnCashShopTask.h"
#include "DnVehicleTask.h"
#include "DnChatRoomTask.h"
#include "DnFarmGameTask.h"
#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif //_HSHIELD
#include "DnLifeSkillFishingTask.h"
#ifdef PRE_ADD_COOKING_SYSTEM
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_COOKING_SYSTEM
#include "DnLifeSkillPlantTask.h"
#include "DnGuildWarTask.h"
#include "DnPetTask.h"

#ifdef PRE_ADD_DONATION
#include "DnContributionTask.h"
#endif

#include "DnGameControlTask.h"

#if defined( PRE_ADD_REVENGE )
#include "DnRevengeTask.h"
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
#include "DnChannelChatTask.h"
#endif // PRE_PRIVATECHAT_CHANNEL
#include "DnWorld.h"

#ifdef PRE_ADD_GAMEQUIT_REWARD
#include "SyncTimer.h"
#endif // PRE_ADD_GAMEQUIT_REWARD

#if defined( PRE_ALTEIAWORLD_EXPLORE )
#include "DnAlteaTask.h"
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined(_KRAZ) && defined(_AUTH) && defined(_FINAL_BUILD)
#include "DnKRAZService.h"
#endif
#include "GlobalValue.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnBridgeTask::CDnBridgeTask() : CTaskListener(true)
{
	m_BridgeState		= eBridgeState::BridgeUnknown;
	m_nSessionID		= 0;
	m_nVillageMapIndex	= 0;
	m_nEnteredGateIndex = 0;
	m_nStageRandomSeed	= 0;
	m_nStageConstructionLevel = 0;
	m_nCurrentCharLevel = 0;
//	m_cReqGameIDType	= REQINFO_TYPE_SINGLE;
	m_nGameTaskType		= GameTaskType::Normal;
	m_vPartyStruct.reserve( PvPCommon::Common::MaxPlayer );
//blondymarry start
	m_ChannelAtt = 0;
//blondymarry end
	m_cGateSelectIndex = 0;

	memset( m_szLoginServerVersion, 0, sizeof(m_szLoginServerVersion) );
	memset( m_szVillageServerVersion, 0, sizeof(m_szVillageServerVersion) );
	memset( m_szGameServerVersion, 0, sizeof(m_szGameServerVersion) );

	m_nAccountDBID = 0;
	m_biCertifyingKey = 0;
	m_bZeroPopulation = false;
	m_bInvalidDisconnect = false;

#if defined( PRE_ADD_FARM_DOWNSCALE )
	m_iAttr = Farm::Attr::None;
#elif defined( PRE_ADD_VIP_FARM )
	m_Attr = Farm::Attr::None;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

	m_bIsGuildWarSystem = false;
	m_nPreviousMapIndexForPvP = -1;

#ifdef PRE_ADD_COMEBACK
	m_bComebackUser = false;
	m_bComeBackMessage = true;
#endif

#ifdef PRE_ADD_STAMPSYSTEM
	m_bShowStampDlg = false;
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_GAMEQUIT_REWARD
	m_bNewbieReward = false;
	m_bLoginSequence = false;
	m_timeAccountReg = 0;
	m_timeRewardCrrTime = 0;
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_DOORS
	m_bIsDoorsMobileAuthUser = false;
#endif

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	m_eSelectedChannelFromVillageAccessMode = PvPCommon::RoomType::max;
	m_bEnterLadderFromVillageAccessMode = false;
	m_eGameStartedGradeChannel = PvPCommon::RoomType::max;
	m_iGameStartedLadderChannel = LadderSystem::MatchType::eCode::None;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
#endif // #ifdef PRE_ADD_PVP_VILLAGE_ACCESS

#if defined(PRE_ADD_SHUTDOWN_CHILD) && defined(PRE_ADD_SELECTIVE_SHUTDOWN)
	m_bShowMsgSelectiveShutDown = false ; // 선택적셧다운제 메세지가 띄워져있는지 여부.
#endif
}

CDnBridgeTask::~CDnBridgeTask()
{
}

void CDnBridgeTask::ConnectResult( bool bRet, eBridgeState eState, char cReqGameIDType, int nGameTaskType )
{
#ifdef PRE_ADD_STAMPSYSTEM
	GetInterface().AddStamp();
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_CASHSHOPBANNER_RENEWAL
	if( !(eState == eBridgeState::LoginToVillage || eState == eBridgeState::GameToVillage || eState ==  eBridgeState::VillageToVillage) )
		GetInterface().ShowIngameBanner( false );
#endif // PRE_ADD_CASHSHOPBANNER_RENEWAL

	switch(eState)
	{
	case eBridgeState::GameToLogin:
	case eBridgeState::VillageToLogin:
		{
			if (bRet)
			{
				SendReconnectLogin( m_nSessionID, m_nAccountDBID, m_biCertifyingKey );
				SetBridgeState( eBridgeState::BridgeUnknown );
				ResetCertifyingKey();
				break;
			}
			GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
			break;
		}
	case eBridgeState::LoginToVillage:
		{
			if (bRet)
			{
				m_BridgeState = eBridgeState::LoginToVillage;
				SendConnectVillage( m_nSessionID, m_nAccountDBID, m_biCertifyingKey );
				ResetCertifyingKey();
				break;
			}
			GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
			break;
		}

	case eBridgeState::GameToVillage:
		{
			if (bRet)
			{
				m_BridgeState = eBridgeState::GameToVillage;
				SendConnectVillage( m_nSessionID, m_nAccountDBID, m_biCertifyingKey );
				ResetCertifyingKey();
				break;
			}
			GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
			break;
		}

	case eBridgeState::VillageToVillage:
		{
			if (bRet)
			{
				m_BridgeState = eBridgeState::VillageToVillage;
				SendConnectVillage( m_nSessionID, m_nAccountDBID, m_biCertifyingKey );
				ResetCertifyingKey();
				break;
			}
			GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
			break;
		}

	case eBridgeState::VillageToGame:
		{
			if (bRet)
			{
				GetInterface().ShowGateQuestionDialog( false );

//				m_cReqGameIDType = cReqGameIDType;
				m_nGameTaskType = nGameTaskType;
				switch( m_BridgeState ) 
				{
				case eBridgeState::LoginToGame:
					break;
				default:
					m_BridgeState = eBridgeState::VillageToGame;
					break;
				}
				break;
			}

			GetInterface().MessageBox( MESSAGEBOX_10, MB_OK, MESSAGEBOX_10, this );
			break;
		}
	}
}

void CDnBridgeTask::OnConnectTcp()
{
	FUNC_LOG();
}

void CDnBridgeTask::OnDisconnectTcp( bool bValidDisconnect )
{
	if( !bValidDisconnect ) {
		CDnLoadingTask::GetInstance().EnableBackgroundLoading( true );
		CDnLoadingTask::GetInstance().ResetStateMachine();
		m_bInvalidDisconnect = true;
	}
	FUNC_LOG();
}

void CDnBridgeTask::OnDisconnectUdp( bool bValidDisconnect )
{
	FUNC_LOG();
}

void CDnBridgeTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	bool bProcessDispatch = false;
	switch( nMainCmd ) 
	{
		case SC_LOGIN:	OnRecvLoginMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;	break;
		case SC_SYSTEM: OnRecvSystemMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;	break;
		case SC_ROOM:	OnRecvRoomMessage( nSubCmd, pData, nSize ), bProcessDispatch = true;		break;
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CDnBridgeTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}

#ifdef _HSHIELD
	HShieldSetup::OnDispatchMessage(nMainCmd, nSubCmd, pData, nSize);
#endif //_HSHIELD
}


void CDnBridgeTask::OnRecvLoginMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eLogin::SC_CHECKLOGIN: OnRecvLoginCheckLogin( (SCCheckLogin*)pData ); break;
		case eLogin::SC_TUTORIALMAPINFO: OnRecvTutorialMapInfo( (SCTutorialMapInfo*)pData ); break;
		case eLogin::SC_CHECKVERSION: OnRecvLoginCheckVersion( (SCCheckVersion*)pData ); break;
	}
}

void CDnBridgeTask::OnRecvSystemMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eSystem::SC_VILLAGEINFO: OnRecvSystemVillageInfo( (SCVillageInfo*)pData ); break;
		case eSystem::SC_CONNECTVILLAGE: OnRecvSystemConnectVillage( (SCConnectVillage *)pData ); break;
		case eSystem::SC_GAMEINFO: OnRecvSystemGameInfo( (SCGameInfo *)pData ); break;
		case eSystem::SC_CONNECTED_RESULT: OnRecvSystemConnectedResult( (SCConnectedResult *)pData ); break;
		case eSystem::SC_RECONNECT_REQ: OnRecvSystemReconnectReq(); break;
		case eSystem::SC_TCP_CONNECT_REQ: OnRecvSystemGameTcpConnectReq(); break;
		case eSystem::SC_RECONNECTLOGIN: OnRecvSystemReconnectLogin( (SCReconnectLogin *)pData ); break;
		case eSystem::SC_MOVE_PVPVILLAGETOLOBBY: OnRecvSystemMoveToLobby(); break;
		//case eSystem::SC_VOICECHATINFO: OnRecvSystemVoiceChatInfo( (SCVoiceChatInfo *)pData ); break;		
		//case eSystem::SC_CHAR_COMM_OPTION: OnRecvSystemCharCommOption( (SCCharCommOption*) pData ); break;
		case eSystem::SC_DETACHCONNECTION_MSG: OnRecvDetachMessage((SCDetachMessage*)pData); break;
		case eSystem::SC_ZEROPOPULATION: OnRecvZeroPopulation((SCZeroPopulation*)pData); break;
#ifdef PRE_ADD_VIP
		case eSystem::SC_VIPAUTOMATICPAY: OnRecvVIPAutoPay((SCVIPAutomaticPay*)pData); break;
#endif
		case eSystem::SC_TCP_PING: OnRecvTcpPing( reinterpret_cast<SCTCPPing*>(pData) ); break;
		case eSystem::SC_TCP_FIELD_PING: OnRecvFieldPing( reinterpret_cast<SCFieldPing*>(pData) ); break;
	}
}

//blondy
void CDnBridgeTask::OnRecvSystemMoveToLobby()
{
    SendMovePvPVillageToPvPLobby();
}
//blondy end

void CDnBridgeTask::OnRecvRoomMessage( int iSubCmd, char* pData, int iSize )
{
	switch( iSubCmd )
	{
		//case eRoom::SC_SYNC_MEMBERINFO:			_OnRecvRoomMemberInfo( pData );			break;
		//case eRoom::SC_SYNC_MEMBERTEAM:			_OnRecvRoomMemberTeam( pData );			break;
		case eRoom::SC_SYNC_MEMBERDEFAULTPARTS:	_OnRecvRoomMemberDefaultParts( pData );	break;
		case eRoom::SC_SYNC_MEMBEREQUIP:		_OnRecvRoomMemberEquip( pData );		break;
		case eRoom::SC_SYNC_MEMBERSKILL:		_OnRecvRoomMemberSkill( pData );		break;
		case eRoom::SC_START_DRAGONNEST:		_OnRecvRoomStartDragonNest( pData );	break;
	}
}
		
void CDnBridgeTask::OnRecvLoginCheckLogin( SCCheckLogin *pPacket )
{
	FUNC_LOG();
	m_nSessionID = pPacket->nSessionID;
	memcpy_s( dynamic_cast<TGameOptions*>( CGameOption::GetInstancePtr() ), sizeof( TGameOptions ), &pPacket->GameOption, sizeof( TGameOptions ) );

#ifdef PRE_ADD_COMEBACK
	SetComeback( pPacket->bComebackUser );
#endif

#ifdef PRE_ADD_GAMEQUIT_REWARD
	SetGameQuitReward( pPacket->bReConnectNewbieReward, pPacket->_tAccountRegistTime, CSyncTimer::GetInstance().GetCurTime() );
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_DOORS
	SetDoorsMobileAuthUser(pPacket->bMobileAuthentication);
#endif

#if defined(_KRAZ) && defined(_AUTH) && defined(_HSHIELD)
	if( pPacket->nRet == ERROR_NONE )
	{
		DnKRAZService* pDnKrazService = static_cast<DnKRAZService*>( g_pServiceSetup );
		if( pDnKrazService )
			pDnKrazService->HShieldSetMyID( pPacket->wszAccountName );
	}
#endif
}

void CDnBridgeTask::OnRecvLoginCheckVersion( SCCheckVersion *pPacket )
{
	WCHAR wszDBVersion[128] = {0,};
	std::string szDBVersion;
	swprintf_s( wszDBVersion, _countof(wszDBVersion), L"%s:%d - %s:%d",
		pPacket->MembershipDBVersion.wszName, pPacket->MembershipDBVersion.iVersion, pPacket->WorldDBVersion.wszName, pPacket->WorldDBVersion.iVersion );
	ToMultiString( wszDBVersion, szDBVersion );
	sprintf_s( m_szLoginServerVersion, _countof(m_szLoginServerVersion), "%s - %s", pPacket->szServerVersion, szDBVersion.c_str() );
}

void CDnBridgeTask::OnRecvTutorialMapInfo( SCTutorialMapInfo *pPacket )
{
	FUNC_LOG();
	m_nVillageMapIndex = pPacket->nMapIndex;
	m_nEnteredGateIndex = pPacket->cGateNo;

	m_BridgeState = eBridgeState::LoginToGame;
}

void CDnBridgeTask::OnRecvSystemVillageInfo( SCVillageInfo *pPacket )
{
	FUNC_LOG();
	if( pPacket->nRet != ERROR_NONE ) {
		
		//blondy
		switch( pPacket->nRet ) {
			case ERROR_PVP_CANTMOVECHANNEL_PARTYSTATE:
				GetInterface().MessageBox( MESSAGEBOX_47, MB_OK, MESSAGEBOX_47);
				CDnLocalPlayerActor::LockInput( false );			
				break;
#ifdef PRE_ADD_DWC
			case ERROR_LOGIN_DWC_WRONG_DATE:
				{
					GetInterface().MessageBox(120270); // Mid: 콜로대회 기간이 아닙니다. 해당 캐릭터는 사용할 수 없습니다.
				}
				break;
#endif // PRE_ADD_DWC
			default:
				GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
				CDnLocalPlayerActor::LockInput( false );
				break;
		}
		return;
	}

	m_nAccountDBID = pPacket->nAccountDBID;
	m_biCertifyingKey = pPacket->biCertifyingKey;

	if( CTaskManager::GetInstance().GetTask( "LoginTask" ) ) {
		CClientSessionManager::GetInstance().ConnectVillageServer( pPacket->szVillageIp, pPacket->wVillagePort, eBridgeState::LoginToVillage);
	}
	else if( CTaskManager::GetInstance().GetTask( "GameTask" ) ) {
		CClientSessionManager::GetInstance().ConnectVillageServer( pPacket->szVillageIp, pPacket->wVillagePort, eBridgeState::GameToVillage);
	}
	else if( CTaskManager::GetInstance().GetTask( "VillageTask" ) || CTaskManager::GetInstance().GetTask( "PVPLobbyTask" )) {
		CClientSessionManager::GetInstance().ConnectVillageServer( pPacket->szVillageIp, pPacket->wVillagePort, eBridgeState::VillageToVillage);
	}
}

void CDnBridgeTask::OnRecvSystemConnectVillage( SCConnectVillage *pPacket )
{
	//지호씨 여기 cWithoutLoading 값이 0보다크면 로드없이 진행해주세요.
	FUNC_LOG();
	if( pPacket->nRet != 0 ) {
		GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
		DestroyTask( true );
		return;
	}
//blondymarry start
// 어트리뷰트저장
	int nPrevChannelAtt = pPacket->nChannelAttr;
	m_ChannelAtt =  pPacket->nChannelAttr;

	/*
	if(IsPVPVillage())
		CGlobalInfo::GetInstance().SetPVPArea(true);//PVP셋팅
	else
		CGlobalInfo::GetInstance().SetPVPArea(false);//PVP셋팅
	*/

//blondymarry end

	memcpy( m_szVillageServerVersion, pPacket->szServerVersion, SERVERVERSIONMAX );

	if( pPacket->cWithoutLoading ) {
		CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
		if( pVillageTask )
		{
			bool bChannelMove = true;
			if( pVillageTask->Initialize( false, bChannelMove ) == false ) {
				ErrorLog( "Can't Create Village Task" );
				SAFE_DELETE( pVillageTask );
				return;
			}
			return;
		}
	}

	GetInterface().FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, this );
	CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
}

void CDnBridgeTask::OnRecvSystemConnectedResult( SCConnectedResult* pPacket )
{
	FUNC_LOG();
	memcpy( m_szGameServerVersion, pPacket->szServerVersion, SERVERVERSIONMAX );

	GetInterface().FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, this );
	CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
#ifdef PRE_MOD_INDUCE_TCPCONNECT
	ResetCertifyingKey();
#endif		//#ifdef PRE_MOD_INDUCE_TCPCONNECT
}

void CDnBridgeTask::OnRecvSystemReconnectReq()
{
	FUNC_LOG();
}

void CDnBridgeTask::OnRecvSystemGameTcpConnectReq()
{
	FUNC_LOG();
	SendGameTcpConnect(GetSessionID(), m_nAccountDBID, m_biCertifyingKey);
#ifdef PRE_MOD_INDUCE_TCPCONNECT
	SendGameTcpConnectReq();
#else		//#ifdef PRE_MOD_INDUCE_TCPCONNECT
	ResetCertifyingKey();
#endif		//#ifdef PRE_MOD_INDUCE_TCPCONNECT
}

void CDnBridgeTask::OnRecvSystemGameInfo( SCGameInfo *pPacket )
{
	FUNC_LOG();
	if( pPacket->nRet != ERROR_NONE ) {
		GetInterface().ServerMessageBox( pPacket->nRet, MB_OK, MESSAGEBOX_8, this );
		return;
	}

	m_nAccountDBID = pPacket->nAccountDBID;
	m_biCertifyingKey = pPacket->biCertifyingKey;

	CClientSessionManager::GetInstance().ConnectGameServer( pPacket->nGameServerIP, pPacket->nGameServerPort, pPacket->nGameServerTcpPort, m_nSessionID, eBridgeState::VillageToGame, pPacket->cReqGameIDType, pPacket->GameTaskType );
}

void CDnBridgeTask::OnRecvDetachMessage(SCDetachMessage * pPacket)
{
#if defined(PRE_ADD_SHUTDOWN_CHILD) && defined(PRE_ADD_SELECTIVE_SHUTDOWN)
	m_bShowMsgSelectiveShutDown = true;
#endif

	m_bInvalidDisconnect = true; // 렌더링을 건너뛰지 않도록 한다.

#if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
	if (pPacket->bDisConnectByClient)
		CClientSessionManager::GetInstance().DisConnectServer( true );
#endif	// #if defined(PRE_MOD_DUPLICATION_LOGIN_MES)

	// 접속이 끊긴것처럼 처리해야해서 MESSAGEBOX_10 으로 콜백 처리하면 된다.
	GetInterface().ServerMessageBox( pPacket->nRetCode, MB_OK, MESSAGEBOX_10, this, true, false, false, true );
}

void CDnBridgeTask::OnRecvZeroPopulation(SCZeroPopulation * pPacket)
{
	//준후쒸 여귀
	m_bZeroPopulation = pPacket->bZeroPopulation;
}

#ifdef PRE_ADD_VIP
void CDnBridgeTask::OnRecvVIPAutoPay(SCVIPAutomaticPay* pData)
{
	if (pData->bAutomaticPay == false)
		GetInterface().MessageBox(2020007, MB_OK);	// 자동결제가 취소되었습니다.
}
#endif // PRE_ADD_VIP

void CDnBridgeTask::OnRecvFieldPing( SCFieldPing* pData )
{
	GetInterface().SetLatency(pData->dwTick);
	//rlkt_ok!
	//GetInterface().SendChatMessage(eChatType::CHATTYPE_NORMAL,L"Server",FormatW(L"Ping: %d ms",pData->dwTick));
}

void CDnBridgeTask::OnRecvTcpPing( SCTCPPing* pData )
{
	SendTcpPing( pData->dwTick );
}


void CDnBridgeTask::OnRecvSystemReconnectLogin( SCReconnectLogin *pPacket )
{
	FUNC_LOG();
	switch( pPacket->nRet ) {
		case 0:
			{
				if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ||
					CTaskManager::GetInstance().GetTask( "GameTask" ) ) {

					if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );

					CTask *pTask = NULL;
					pTask = CTaskManager::GetInstance().GetTask( "VillageTask" );
					if( pTask ) {
						((CDnVillageTask*)pTask)->FinalizeStage();
						pTask->DestroyTask( true );
					}

					pTask = CTaskManager::GetInstance().GetTask( "GameTask" );
					if( pTask ) {
						((CDnGameTask*)pTask)->FinalizeStage();
						pTask->DestroyTask( true );
					}
					//blondymarry start					
					pTask = CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
					if( pTask ) {
						((CDnPVPLobbyVillageTask*)pTask)->FinalizeStage();
						pTask->DestroyTask( true );
					}
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
					CDnPVPLobbyVillageTask::ReleasePreloadPlayer();
#endif
					//blondymarry end
					
					for( std::vector< const char* >::iterator it = m_TaskNameList.begin(); it != m_TaskNameList.end(); ++it) {
						pTask = CTaskManager::GetInstance().GetTask( *it );
						if( pTask ) pTask->DestroyTask( true );
					}
					m_TaskNameList.clear();

					SetBridgeState( ( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) ? eBridgeState::VillageToLogin : eBridgeState::GameToLogin );

					CDnLoginTask *pLoginTask = new CDnLoginTask;
					pLoginTask->SetTaskName( "LoginTask" );	// DnInterface에서 Task이름을 기억하기 위해 강제로 미리 넣어둡니다.
#ifdef PRE_MOD_SELECT_CHAR
					pLoginTask->PreInitialize( true );
#else // PRE_MOD_SELECT_CHAR
					pLoginTask->PreInitialize();
#endif // PRE_MOD_SELECT_CHAR
					pLoginTask->Initialize( CDnLoginTask::CharSelect );
					CTaskManager::GetInstance().AddTask( pLoginTask, "LoginTask", -1, false );

					m_nAccountDBID = pPacket->nAccountDBID;
					m_biCertifyingKey = pPacket->biCertifyingKey;
					/*SendReconnectLogin( GetSessionID(), pPacket->nAccountDBID, pPacket->biCertifyingKey );
					SetBridgeState( eBridgeState::BridgeUnknown );*/
				}
				else {
//					GetInterface().FadeDialog( 0xff000000, 0x00000000, CGlobalInfo::GetInstance().m_fFadeDelta, this );
//					CEtSoundEngine::GetInstance().FadeVolume( NULL, 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
				}
			}
			break;
		default:
			//blondy pvp게임은 또 그냥 메세지 박스에서 ok누르면 게임 종료 해달라네요
			CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

			if( CTaskManager::GetInstance().GetTask( "VillageTask" ) || 
				( pTask && pTask->GetGameTaskType() != GameTaskType::PvP ) ) {
				GetInterface().MessageBox( MESSAGEBOX_9, MB_OK, MESSAGEBOX_9, NULL );

				CDnLocalPlayerActor::LockInput(false);
				GetInterface().FadeDialog( 0xff000000, 0x00000000, CGlobalInfo::GetInstance().m_fFadeDelta, NULL );				
			}
			else {
				GetInterface().FadeDialog( 0xFFFFFFFF, 0x00FFFFFF, 5.f, NULL , true );
				GetInterface().MessageBox( MESSAGEBOX_9, MB_OK, MESSAGEBOX_9, this );
			}
			//blondy end
			break;
	}
}

//void CDnBridgeTask::OnRecvSystemCharCommOption( SCCharCommOption *pPacket )
//{
//	GetInterface().SetDirectDialogPermission( pPacket->nSessionID, pPacket->cCommunityOption );
//}

void CDnBridgeTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) {
		// 빌리지서버에 접속할 수 없을때 나오는 메세지. 이것때문에 월드에서 마을로 갈때 마을 접속안되면 튕긴다.
		case MESSAGEBOX_8:
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
				if( pGameTask ) {
					GetInterface().FadeDialog( 0xff000000, 0x00000000, CGlobalInfo::GetInstance().m_fFadeDelta, NULL );
					CEtSoundEngine::GetInstance().FadeVolume( NULL, 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
				}
			}
			break;
		case MESSAGEBOX_9:
		case MESSAGEBOX_10:
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
				{
					if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
					CTaskManager::GetInstance().RemoveAllTask( false );
				}
			}
			break;
		case FADE_DIALOG:                 
			if( nCommand == EVENT_FADE_COMPLETE ) {
				if( m_bInvalidDisconnect ) {
					if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
					CTaskManager::GetInstance().RemoveAllTask( false );
					break;
				}

#ifdef PRE_ADD_STAMPSYSTEM
				GetInterface().ChangeWorldMap();
#endif // PRE_ADD_STAMPSYSTEM

				switch( m_BridgeState ) {
					case eBridgeState::LoginToVillage:
						{
							InitializeDefaultTask();
							if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
							CTask *pLoginTask = CTaskManager::GetInstance().GetTask( "LoginTask" );
							pLoginTask->DestroyTask( true );

							CDnVillageTask *pVillageTask = new CDnVillageTask;
							if( pVillageTask->Initialize() == false ) {
								ErrorLog( "Can't Create Village Task" );
								SAFE_DELETE( pVillageTask );
								return;
							}						

							CTaskManager::GetInstance().AddTask( pVillageTask, "VillageTask", -1, false );

							DebugLog("eBridgeState::LoginToVillage");

							//FUNC_LOG();

#ifdef PRE_ADD_STAMPSYSTEM
							SetStampDlg( true );
#endif // PRE_ADD_STAMPSYSTEM

						}
						break;
					case eBridgeState::GameToVillage:
						{
							if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
							CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
							pGameTask->DestroyTask( true );
							pGameTask->FinalizeStage();

							//blondy
							if( GetChannelAttribute() & eChannelAttribute::CHANNEL_ATT_PVPLOBBY ) {
								CDnPVPLobbyVillageTask *pPVPVillageTask = new CDnPVPLobbyVillageTask;
								pPVPVillageTask->SetTaskName( "PVPLobbyTask" );
								if( pPVPVillageTask->Initialize(false) == false ) {
									ErrorLog( "Can't Create PVPLobby Task" );
									SAFE_DELETE( pPVPVillageTask );
									return;
								}							

								CTaskManager::GetInstance().AddTask( pPVPVillageTask, "PVPLobbyTask", -1, false );
								DebugLog("eBridgeState::VillageToPVPVillage");
							}
							else { //일반 마을(투기장)임
								CDnVillageTask *pVillageTask = new CDnVillageTask;
								if( pVillageTask->Initialize( false ) == false ) {
									ErrorLog( "Can't Create Village Task" );
								SAFE_DELETE( pVillageTask );
								return;
								}

								CTaskManager::GetInstance().AddTask( pVillageTask, "VillageTask", -1, false );
							}
							//blondy end
							if( CDnItemTask::IsActive() ) CDnItemTask::GetInstance().ResetRequestWait();
						}
						break;
					case eBridgeState::VillageToGame:
						{
							CDnGameTask* pGameTask = NULL;
							CDnVillageTask *pVillageTask = NULL;
							CDnPVPLobbyVillageTask *pPVPLobbyVillageTask = NULL;

							if( CDnInterface::IsActive() ) GetInterface().CloseAllMainMenuDialog();
							if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
							
							// 운영자 난입을 할 수 있어서 VillageTask 체크하여 파괴시킨다.
							pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
							if( pVillageTask ){
								pVillageTask->DestroyTask( true );
#ifdef PRE_ADD_RELOAD_ACTFILE
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
								pVillageTask->ReleasePreloadPlayer();
#endif 
#endif 
								pVillageTask->FinalizeStage();
							}

							pPVPLobbyVillageTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
							if( pPVPLobbyVillageTask ){
								pPVPLobbyVillageTask->DestroyTask( true );

								pPVPLobbyVillageTask->FinalizeStage();
							}								

							switch( m_nGameTaskType ) {
								case GameTaskType::Normal: pGameTask = new CDnGameTask; break;
								case GameTaskType::PvP: pGameTask = new CDnPvPGameTask; break;
								case GameTaskType::DarkLair: pGameTask = new CDnDLGameTask; break;
								case GameTaskType::Farm: pGameTask = new CDnFarmGameTask; break;
							}
								
							if( !pGameTask || pGameTask->Initialize() == false ) 
							{
								OutputDebug( "Can't Create Game Task" );
								SAFE_DELETE( pGameTask );
								return;
							}
							CTaskManager::GetInstance().AddTask( pGameTask, "GameTask", -1, false );
							pGameTask->SetBridgeState( eBridgeState::VillageToGame );

							// 여기서 빌리지 관련 정보만 가지고 있다가
							// EquipData 가 오면 그때 스테이지를 생성하게 바꿈.
							if( pVillageTask ){
								m_nVillageMapIndex = pVillageTask->GetVillageMapIndex();
								m_nEnteredGateIndex = pVillageTask->GetEnteredGateIndex();
							}
							if( pPVPLobbyVillageTask ){
								m_nVillageMapIndex = pPVPLobbyVillageTask->GetVillageMapIndex();
								m_nEnteredGateIndex = pPVPLobbyVillageTask->GetEnteredGateIndex();
							}
							m_nStageConstructionLevel = 0;

							if( CDnItemTask::IsActive() ) CDnItemTask::GetInstance().ResetRequestWait();
							OutputDebug( "VillageToGame\n" );
						}
						break;
					case eBridgeState::VillageToVillage:
						{
							if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
							CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
							CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
							if( CDnInterface::IsActive() ) GetInterface().CloseAllMainMenuDialog();

							if( GetChannelAttribute() & eChannelAttribute::CHANNEL_ATT_PVPLOBBY ) { //로비인지를 보고 태스크를 분리 
								//pvp마을에서 로비로 가는 경우 								
								if( pVillageTask ){
									pVillageTask->DestroyTask( true );
									pVillageTask->FinalizeStage();
								}

								CDnPVPLobbyVillageTask *pPVPVillageTask = new CDnPVPLobbyVillageTask;
								pPVPVillageTask->SetTaskName( "PVPLobbyTask" );
								// PvP마을에서 로비로 이동시 FirstConnect 플래그값을 false로 변경합니다 - robust
								if( pPVPVillageTask->Initialize(false) == false ) {
									ErrorLog( "Can't Create PVPLobby Task" );
									SAFE_DELETE( pPVPVillageTask );
									return;
								}							

								CTaskManager::GetInstance().AddTask( pPVPVillageTask, "PVPLobbyTask", -1, false );

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
								CDnLoadingTask::GetInstance().Initialize( pPVPVillageTask, 0 );
								CDnPVPLobbyVillageTask::InitializePreloadPlayer();
#endif
								DebugLog("eBridgeState::VillageToPVPVillage");

							}
							else {
								//일반 마을에서 일반마을로 가는 경우 
								if( pVillageTask )
								{
#ifdef PRE_ADD_RELOAD_ACTFILE
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
									pVillageTask->ReleasePreloadPlayer();
#endif 
#endif 
									pVillageTask->FinalizeStage();
									if( pVillageTask->Initialize( false ) == false ) {
									ErrorLog( "Can't Create Village Task" );
									SAFE_DELETE( pVillageTask );
									return;
									}
								}
								//PVP로비에서 PVP마을로 가는 경우 	
								if( pPVPLobbyTask )
								{
									pPVPLobbyTask->DestroyTask( true );
									pPVPLobbyTask->FinalizeStage();
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
									CDnPVPLobbyVillageTask::ReleasePreloadPlayer();
#endif

									CDnVillageTask *pNewVillageTask = new CDnVillageTask;
									// Note 한기: FirstConnect 플래그 true 로 하면 스킬리스트 등등이 또 와서 
									// 바꿔놓습니다.. 경민씨랑 얘기해봐야할 듯.
									if( pNewVillageTask->Initialize( false ) == false ) {
										ErrorLog( "Can't Create Village Task" );
										SAFE_DELETE( pVillageTask );
										return;
									}

									CTaskManager::GetInstance().AddTask( pNewVillageTask, "VillageTask", -1, false );
								}
								
							}
//blondymarry end
						}
						break;
					case eBridgeState::LoginToGame:
						{
							if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
							InitializeDefaultTask();

							CTask *pLoginTask = CTaskManager::GetInstance().GetTask( "LoginTask" );
							pLoginTask->DestroyTask( true );

							CDnGameTask *pGameTask = new CDnGameTask;
							if( pGameTask->Initialize() == false ) {
								OutputDebug( "Can't Create Game Task" );
								SAFE_DELETE( pGameTask );
								return;
							}
							CTaskManager::GetInstance().AddTask( pGameTask, "GameTask", -1, false );

							m_nStageConstructionLevel = 0;

							pGameTask->SetBridgeState( eBridgeState::LoginToGame );
						}
						break;
					case eBridgeState::GameToLogin:
					case eBridgeState::VillageToLogin:
						{
							// 이게 있어야 캐릭터 선택창으로 이동하면서 npc한테 말을 걸어 나온 블라인드Dlg가 닫힐 수 있다.
							// Fade에서 블라인드를 닫으면, 열리는 페이드+컷신시작때도 블라인드가 닫혀버린다.
							// 그래서 이렇게 별도로 처리한다.
							GetInterface().CloseBlind();

							// 맵 번호 다시 초기화.
							CGlobalInfo::GetInstance().m_nCurrentMapIndex = -1;

							SendReconnectLogin();
						}
						break;					
					case eBridgeState::Exit:
						if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
						CTaskManager::GetInstance().RemoveAllTask( false );
						m_BridgeState = eBridgeState::BridgeUnknown;
						break;
				}
			}
			break;
	}
}

#define REGISTER_TASK( Name ) CDn##Name *p##Name = new CDn##Name; p##Name->Initialize(); \
													CTaskManager::GetInstance().AddTask( p##Name, AddTaskName( #Name ), -1, false ); 

void CDnBridgeTask::InitializeDefaultTask()
{
	// Default 타스크들 등록시켜논다. Party, Item
	REGISTER_TASK( PartyTask );
	REGISTER_TASK( ItemTask );
	REGISTER_TASK( SkillTask );
	REGISTER_TASK( CommonTask );
	REGISTER_TASK( QuestTask );
	REGISTER_TASK( FriendTask );	
	REGISTER_TASK( GestureTask );
#ifdef _USE_VOICECHAT
	REGISTER_TASK( VoiceChatTask );
#endif
	REGISTER_TASK( GuildTask );
	REGISTER_TASK( IsolateTask );
	REGISTER_TASK( TradeTask );
	REGISTER_TASK( MissionTask );
	REGISTER_TASK( AppellationTask );
	REGISTER_TASK( GameOptionTask );
	REGISTER_TASK( RadioMsgTask );
	REGISTER_TASK( GameTipTask );
	REGISTER_TASK( RestraintTask );
	REGISTER_TASK( CashShopTask );
	REGISTER_TASK( VehicleTask );
	REGISTER_TASK( ChatRoomTask );
	REGISTER_TASK( NestInfoTask );
	REGISTER_TASK( MasterTask );
	REGISTER_TASK( LifeSkillFishingTask );
#ifdef PRE_ADD_COOKING_SYSTEM
	REGISTER_TASK( LifeSkillCookingTask );
#endif // PRE_ADD_COOKING_SYSTEM
	REGISTER_TASK( LifeSkillPlantTask );
	REGISTER_TASK( GuildWarTask );
	REGISTER_TASK( TimeEventTask );
	REGISTER_TASK( PetTask );

#ifdef PRE_ADD_DONATION
	REGISTER_TASK( ContributionTask );
#endif
	REGISTER_TASK( GameControlTask );
#if defined(PRE_ADD_REVENGE)
	REGISTER_TASK( RevengeTask );
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	REGISTER_TASK( ChannelChatTask );
#endif // PRE_PRIVATECHAT_CHANNEL

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	REGISTER_TASK( AlteaTask );
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	while( !CDnMainFrame::GetInstance().IsFinishPreloadPlayer() ) { Sleep(1); }
#endif
#ifdef PRE_ADD_SHUTDOWN_CHILD
	m_ShutDownSystem.Initialize();
#endif
}

/*
void CDnBridgeTask::_OnRecvRoomMemberInfo( char* pData )
{
	m_vPartyStruct.clear();

	SCROOM_SYNC_MEMBERINFO* pPacket = reinterpret_cast<SCROOM_SYNC_MEMBERINFO*>(pData);

	for( int i=0; i<pPacket->nCount; ++i ) 
	{
		CDnPartyTask::PartyStruct Struct;
		Struct.Assign( pPacket->Member[i] );

		m_vPartyStruct.push_back( Struct );
	}
}
*/

/*
void CDnBridgeTask::_OnRecvRoomMemberTeam( char* pData )
{
	SCROOM_SYNC_MEMBERTEAM* pPacket = reinterpret_cast<SCROOM_SYNC_MEMBERTEAM*>(pData);
	if( m_vPartyStruct.empty() || m_vPartyStruct.size() != pPacket->nCount )
		assert(0);

	for( int i=0 ; i<pPacket->nCount ; ++i )
		m_vPartyStruct[i].usTeam = pPacket->usTeam[i];
}
*/

void CDnBridgeTask::_OnRecvRoomMemberDefaultParts( char* pData )
{
	SCROOM_SYNC_MEMBERDEFAULTPARTS* pPacket = reinterpret_cast<SCROOM_SYNC_MEMBERDEFAULTPARTS*>(pData);
	if( m_vPartyStruct.empty() || m_vPartyStruct.size() != pPacket->nCount )
		assert(0);

	for( int i=0 ; i<pPacket->nCount ; ++i )
		memcpy( m_vPartyStruct[i].nDefaultPartsIndex, &pPacket->iDefaultPartsIndex[i*4], sizeof(int)*4 );
}

void CDnBridgeTask::_OnRecvRoomMemberEquip( char* pData )
{
	SCROOM_SYNC_MEMBEREQUIP* pPacket = reinterpret_cast<SCROOM_SYNC_MEMBEREQUIP*>(pData);
	if( m_vPartyStruct.empty() || m_vPartyStruct.size() != pPacket->nCount )
		assert(0);

	TPartyMemberEquip MemberEquip[PvPCommon::Common::MaxPlayer];
	memset( &MemberEquip, 0, sizeof(MemberEquip) );
	int iOffset = sizeof(short);
	int iSize	= 0;

	for( int i=0 ; i<pPacket->nCount ; ++i ) 
	{
		char cCount = *((char*)pPacket + iOffset);
		iSize = ( cCount * sizeof(TItemInfo) ) + sizeof(char);
		memcpy( &MemberEquip[i], (char*)pPacket + iOffset, iSize );
		iOffset += iSize;
	}

	for( int i=0 ; i<pPacket->nCount ; ++i )
	{
		CDnPartyTask::PartyStruct* pStruct = &m_vPartyStruct[i];
		for( int j=0 ; j< MemberEquip[i].cCount ; ++j ) 
		{
			if( MemberEquip[i].EquipArray[j].cSlotIndex < EQUIP_WEAPON1 )
				pStruct->VecParts.push_back( MemberEquip[i].EquipArray[j] );
			else 
				pStruct->Weapon[ MemberEquip[i].EquipArray[j].cSlotIndex - EQUIP_WEAPON1 ] = MemberEquip[i].EquipArray[j];
		}
	}
}

void CDnBridgeTask::_OnRecvRoomMemberSkill( char* pData )
{
	SCROOM_SYNC_MEMBERSKILL* pPacket = reinterpret_cast<SCROOM_SYNC_MEMBERSKILL*>(pData);
	if( m_vPartyStruct.empty() || m_vPartyStruct.size() != pPacket->nCount )
		assert(0);

	TPartyMemberSkill MemberSkill[PvPCommon::Common::MaxPlayer];
	memset( &MemberSkill, 0, sizeof(MemberSkill) );
	int iOffset = sizeof(short);
	int iSize	= 0;

	for( int i=0 ; i<pPacket->nCount ; ++i ) 
	{
		char cCount = *((char*)pPacket + iOffset);
		iSize = ( cCount * sizeof(TSkill) ) + sizeof(char);
		memcpy( &MemberSkill[i], (char*)pPacket + iOffset, iSize );
		iOffset += iSize;
	}

	for( int i=0 ; i<pPacket->nCount ; ++i )
	{
		CDnPartyTask::PartyStruct* pStruct = &m_vPartyStruct[i];
		for( int j=0 ; j< MemberSkill[i].cCount ; ++j ) 
			pStruct->VecSkill.push_back( MemberSkill[i].SkillArray[j] );
	}
}


const char *CDnBridgeTask::GetCurrentServerVersion()
{
	if( CTaskManager::GetInstance().GetTask( "LoginTask" ) ) return m_szLoginServerVersion;
	else if( CTaskManager::GetInstance().GetTask( "GameTask" ) ) return m_szGameServerVersion;
	else if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) return m_szVillageServerVersion;

	return "";
}

void CDnBridgeTask::SetPVPGameStatus( std::wstring RoomName , int nObjectiveCount , int nGameModeTableID , int nTimeSec , byte cMaxPlayer , bool bPVPShowHp, bool bDisablePVPRegulation)
{
	m_sBackUpRoomStatus.wszPVPRoomName   =  RoomName;
	m_sBackUpRoomStatus.m_nObjectiveCount = nObjectiveCount;
	m_sBackUpRoomStatus.m_nGameModeTableID = nGameModeTableID;
	m_sBackUpRoomStatus.m_nTimeSec = nTimeSec;
	m_sBackUpRoomStatus.m_cMaxPlayer = cMaxPlayer;
	m_sBackUpRoomStatus.m_bPVPShowHp = bPVPShowHp;
	m_sBackUpRoomStatus.m_bDisablePVPRegulation = bDisablePVPRegulation;
}

void CDnBridgeTask::ResetPVPGameStatus()
{
	m_sBackUpRoomStatus.wszPVPRoomName =  L"";
	m_sBackUpRoomStatus.m_nObjectiveCount = 0;
	m_sBackUpRoomStatus.m_nGameModeTableID = 0;
	m_sBackUpRoomStatus.m_nTimeSec = 0;
	m_sBackUpRoomStatus.m_cMaxPlayer = 0;
	m_sBackUpRoomStatus.m_bPVPShowHp = false;
	m_sBackUpRoomStatus.m_bDisablePVPRegulation = false;
}

void CDnBridgeTask::ResetCertifyingKey()
{
//	m_nAccountDBID = 0;
	m_biCertifyingKey = 0;
}


void CDnBridgeTask::_OnRecvRoomStartDragonNest( char* pData )
{
	SCStartDragonNest* pPacket = reinterpret_cast<SCStartDragonNest*>(pData);

	//1. World정보 설정하고...
	CDnWorld::GetInstance().SetDragonNestType(pPacket->Type);
	
	//2. 부활 카운트 정보 설정..
	CDnItemTask::GetInstance().SetUsableRebirthCoin(pPacket->nRebirthCount);

}

bool CDnBridgeTask::IsPvPGameMode( int eMode )
{
	CDnPvPGameTask* pGameTask = (CDnPvPGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
	{
		if( pGameTask && pGameTask->GetGameMode() == eMode ) 
			return true;
	}

	return false;
}

#ifdef PRE_ADD_SHUTDOWN_CHILD
void CDnBridgeTask::ActivateShutDownSystem(USHORT age)
{
	m_ShutDownSystem.Activate(age);
}

void CDnBridgeTask::ProcessShutDownSystem(float fElpasedTime)
{
	m_ShutDownSystem.Process(fElpasedTime);
}

#ifdef _WORK
void CDnBridgeTask::TestShutDownSystem(int startHour, int endHour)
{
	m_ShutDownSystem.TestShutDownMsg(startHour, endHour);
}
#endif // _WORK

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
void CDnBridgeTask::SetSelectiveShutDown( int nShutDownTime ) // 선택적셧다운제 종료시간.
{
	m_ShutDownSystem.SetSelectiveShutDown( nShutDownTime );	
}
#endif // PRE_ADD_SELECTIVE_SHUTDOWN


#endif


#ifdef PRE_ADD_COMEBACK
void CDnBridgeTask::SetComeback( bool bComeback ){
	m_bComebackUser = bComeback;
}
bool CDnBridgeTask::GetComeback(){
	return m_bComebackUser;
}
void CDnBridgeTask::EndComebackMessage(){
	m_bComeBackMessage = false;
}
bool CDnBridgeTask::GetComebackMessage(){
	return m_bComeBackMessage;
}
#endif // PRE_ADD_COMEBACK

#ifdef PRE_ADD_PVP_TOURNAMENT
CDnPVPTournamentDataMgr& CDnBridgeTask::GetPVPTournamentDataMgr()
{
	return m_PVPTournamentDataMgr;
}

bool CDnBridgeTask::IsLocalActorSessionID(UINT sessionID) const
{
	return (m_nSessionID == sessionID);
}
#endif

#ifdef PRE_ADD_GAMEQUIT_REWARD	
void CDnBridgeTask::EndNewbieRewardMessage(){
	
	m_bNewbieRewardMessage = false;

	GetInterface().ResetCharacterListGameQuitRewardComboDlg();
}
#endif // PRE_ADD_GAMEQUIT_REWARD