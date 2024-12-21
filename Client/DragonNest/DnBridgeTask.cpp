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
	m_bShowMsgSelectiveShutDown = false ; // �������˴ٿ��� �޼����� ������ִ��� ����.
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
					GetInterface().MessageBox(120270); // Mid: �ݷδ�ȸ �Ⱓ�� �ƴմϴ�. �ش� ĳ���ʹ� ����� �� �����ϴ�.
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
	//��ȣ�� ���� cWithoutLoading ���� 0����ũ�� �ε���� �������ּ���.
	FUNC_LOG();
	if( pPacket->nRet != 0 ) {
		GetInterface().MessageBox( MESSAGEBOX_8, MB_OK, MESSAGEBOX_8, this );
		DestroyTask( true );
		return;
	}
//blondymarry start
// ��Ʈ����Ʈ����
	int nPrevChannelAtt = pPacket->nChannelAttr;
	m_ChannelAtt =  pPacket->nChannelAttr;

	/*
	if(IsPVPVillage())
		CGlobalInfo::GetInstance().SetPVPArea(true);//PVP����
	else
		CGlobalInfo::GetInstance().SetPVPArea(false);//PVP����
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

	m_bInvalidDisconnect = true; // �������� �ǳʶ��� �ʵ��� �Ѵ�.

#if defined(PRE_MOD_DUPLICATION_LOGIN_MES)
	if (pPacket->bDisConnectByClient)
		CClientSessionManager::GetInstance().DisConnectServer( true );
#endif	// #if defined(PRE_MOD_DUPLICATION_LOGIN_MES)

	// ������ �����ó�� ó���ؾ��ؼ� MESSAGEBOX_10 ���� �ݹ� ó���ϸ� �ȴ�.
	GetInterface().ServerMessageBox( pPacket->nRetCode, MB_OK, MESSAGEBOX_10, this, true, false, false, true );
}

void CDnBridgeTask::OnRecvZeroPopulation(SCZeroPopulation * pPacket)
{
	//���ľ� ����
	m_bZeroPopulation = pPacket->bZeroPopulation;
}

#ifdef PRE_ADD_VIP
void CDnBridgeTask::OnRecvVIPAutoPay(SCVIPAutomaticPay* pData)
{
	if (pData->bAutomaticPay == false)
		GetInterface().MessageBox(2020007, MB_OK);	// �ڵ������� ��ҵǾ����ϴ�.
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
					pLoginTask->SetTaskName( "LoginTask" );	// DnInterface���� Task�̸��� ����ϱ� ���� ������ �̸� �־�Ӵϴ�.
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
			//blondy pvp������ �� �׳� �޼��� �ڽ����� ok������ ���� ���� �ش޶�׿�
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
		// ������������ ������ �� ������ ������ �޼���. �̰Ͷ����� ���忡�� ������ ���� ���� ���ӾȵǸ� ƨ���.
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
							else { //�Ϲ� ����(������)��
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
							
							// ��� ������ �� �� �־ VillageTask üũ�Ͽ� �ı���Ų��.
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

							// ���⼭ ������ ���� ������ ������ �ִٰ�
							// EquipData �� ���� �׶� ���������� �����ϰ� �ٲ�.
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

							if( GetChannelAttribute() & eChannelAttribute::CHANNEL_ATT_PVPLOBBY ) { //�κ������� ���� �½�ũ�� �и� 
								//pvp�������� �κ�� ���� ��� 								
								if( pVillageTask ){
									pVillageTask->DestroyTask( true );
									pVillageTask->FinalizeStage();
								}

								CDnPVPLobbyVillageTask *pPVPVillageTask = new CDnPVPLobbyVillageTask;
								pPVPVillageTask->SetTaskName( "PVPLobbyTask" );
								// PvP�������� �κ�� �̵��� FirstConnect �÷��װ��� false�� �����մϴ� - robust
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
								//�Ϲ� �������� �Ϲݸ����� ���� ��� 
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
								//PVP�κ񿡼� PVP������ ���� ��� 	
								if( pPVPLobbyTask )
								{
									pPVPLobbyTask->DestroyTask( true );
									pPVPLobbyTask->FinalizeStage();
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
									CDnPVPLobbyVillageTask::ReleasePreloadPlayer();
#endif

									CDnVillageTask *pNewVillageTask = new CDnVillageTask;
									// Note �ѱ�: FirstConnect �÷��� true �� �ϸ� ��ų����Ʈ ����� �� �ͼ� 
									// �ٲ�����ϴ�.. ��ξ��� ����غ����� ��.
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
							// �̰� �־�� ĳ���� ����â���� �̵��ϸ鼭 npc���� ���� �ɾ� ���� ����ε�Dlg�� ���� �� �ִ�.
							// Fade���� ����ε带 ������, ������ ���̵�+�ƽŽ��۶��� ����ε尡 ����������.
							// �׷��� �̷��� ������ ó���Ѵ�.
							GetInterface().CloseBlind();

							// �� ��ȣ �ٽ� �ʱ�ȭ.
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
	// Default Ÿ��ũ�� ��Ͻ��ѳ��. Party, Item
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

	//1. World���� �����ϰ�...
	CDnWorld::GetInstance().SetDragonNestType(pPacket->Type);
	
	//2. ��Ȱ ī��Ʈ ���� ����..
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
void CDnBridgeTask::SetSelectiveShutDown( int nShutDownTime ) // �������˴ٿ��� ����ð�.
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