#include "StdAfx.h"
#include "DnCommonTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnNPCActor.h"
#include "DnLocalPlayerActor.h"
#include "DnNpcDlg.h"
#include "NpcSendPacket.h"
#include "DnInterfaceDlgID.h"
#include "DnChatTabDlg.h"
#include "DnTableDB.h"
#include "DnCutSceneTask.h"
#include "DnGameTask.h"
#include "DnVillageTask.h"
#include "EtUIMan.h"
#include "DnMainFrame.h"
#include "DnPartyTask.h"
#include "DnLoadingTask.h"
#include "DnMainMenuDlg.h"
#include "GameSendPacket.h"
#include "TaskManager.h"
#include "DnNpcTalkCamera.h"
#include "EtBenchMark.h"
#include <MMSystem.h>
#include "DnCutSceneActorProcessor.h"
#include "DnGuildTask.h"
#include "DnWorldNpcProp.h"
#include "DnItemTask.h"
#include "DnWorldDataManager.h"
#include "DnWorldData.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"
#include "DnWorldSound.h"
#include "DnQuestTask.h"
#include "DnActorClassDefine.h"
#include "SyncTimer.h"
#include "DnSystemDlg.h"
#include "DnPVPLadderRankBoardDlg.h"
#include "EtSoundEngine.h"
#include "EtSoundChannelGroup.h"
#include "GameOption.h"
#include "TimeSet.h"

#include "DnMainDlg.h"
#include "DnMiniSiteChecker.h"
#ifdef PRE_ADD_QUICK_PVP
#include "PvPSendPacket.h"
#include "DnAcceptRequestDlg.h"
#endif
#include "DnNestInfoTask.h"

#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
#include "DnLifeSkillCookingTask.h"
#endif

#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
#include "DnCashShopTask.h"
#endif

#ifdef PRE_ADD_GAMEQUIT_REWARD
#include "DnInvenTabDlg.h"
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef PRE_ADD_MAINQUEST_UI
#include "DnMainQuestDlg.h"
#endif

extern bool g_bSkipScene;
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#define SKIP_FADE_DIALOG 4444
CDnCommonTask::CDnCommonTask() : CTaskListener(true)
{
	m_pCutSceneTask = NULL;
	m_pFadeProcess = NULL;
	m_nQuestCutSceneID = -1;
	m_bControlQuestScript = false;
	m_nQuestCutSceneNpcID = -1;
	m_bQuestCutSceneAutoFadeIn = true;
	m_bSkipCutScene = false;

#ifdef _KR
	m_nPlayTimeNoticeCount = 0;
	m_dwSelectCharTime = 0;
#endif //_KR

	m_tServerCloseTime = 0;
	m_nProcessedRemainTime = 0;
	m_nCurrentTalkNpcID = 0;

	m_bForceOpenRecompense = false;
	m_nSelectedCompleteQuestIndex = 0;
	
	m_bCheatCutScene = false;
	m_bRequestNpcTalk = false;
	m_bMinimizeFlag = false;
	m_bCheckAttendanceFirst = false;

	m_cPCBangGrade = PCBang::Grade::None;
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	m_bQuickPvPRequesting = false;
#endif
#ifdef PRE_FIX_63975
	m_bHavePcBangRentalItem = false;
#endif
#ifdef PRE_ADD_WEEKLYEVENT
	m_tWeeklyEventRefreshTick = 0;
	m_nCurrentEventWeek = 0;
#endif
#ifdef PRE_REMOVE_MINISITE
	m_pMiniSiteChecker = NULL;
#endif
}

CDnCommonTask::~CDnCommonTask()
{
	SAFE_RELEASE_SPTR( m_hTalkNpcCamera );
	SAFE_DELETE( m_pFadeProcess );
	SAFE_DELETE(m_pMiniSiteChecker);

	m_mVariableData.clear();
}

bool CDnCommonTask::Initialize()
{
#ifdef PRE_REMOVE_MINISITE
#else
	m_pMiniSiteChecker = new CDnMiniSiteChecker;
#endif
	return true;
}

//LOCAL_TIME s_PrevTestTime = 0;
void CDnCommonTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsSyncComplete() && !m_VecInitTimeProcessList.empty() ) {
		for( DWORD i=0; i<m_VecInitTimeProcessList.size(); i++ ) {
			InitTimeProcessStruct *pStruct = &m_VecInitTimeProcessList[i];
			ExecuteInitTimeProcess( pStruct );
		}
		SAFE_DELETE_VEC( m_VecInitTimeProcessList );
	}

	///// 컷신이 끝났는지 조회
	if( m_pCutSceneTask ) {
		bool bEnd = m_pCutSceneTask->IsPlayEnd();
		if( bEnd ) EndCutScene();
	}

	DWORD dwScore=0;
	CEtBenchMark::GetInstance().GetResult(&dwScore);
	if( dwScore != 0 ) {
		wchar_t wzStr[255]={0,};
		WCHAR wszDevice[MAX_DEVICE_IDENTIFIER_STRING] = {0,};		

		D3DADAPTER_IDENTIFIER9 did;
		GetEtDevice()->GetAdapterIdentifier( &did);

		MultiByteToWideChar(CP_ACP, 0, did.Description, -1, wszDevice, MAX_DEVICE_IDENTIFIER_STRING );
		wsprintf(wzStr, L"BenchMark Score : %d [%s]", dwScore, wszDevice );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wzStr, false );
		CEtBenchMark::GetInstance().Reset();
	}

	if (m_pMiniSiteChecker)
		m_pMiniSiteChecker->Process(fDelta);

#if defined (_KRAZ) || defined (_WORK)
	ProcessPlayTimeNotice();
#endif // defined (_KRAZ) || defined (_WORK)
	ProcessServerClose();

	if( CDnMainFrame::IsActive() ) {
		RECT rcRect;
		HWND hwnd = CDnMainFrame::GetInstance().GetHWnd();
		if( GetClientRect( hwnd, &rcRect ) ) {
			if( ( ( rcRect.right - rcRect.left <= 0 ) || ( rcRect.bottom - rcRect.top <= 0 ) ) || g_bSkipScene  ) {
				if( m_bMinimizeFlag == false ) {
					if( CEtSoundEngine::IsActive() ) {
						for( DWORD i=0; i<CEtSoundEngine::GetInstance().GetChannelGroupCount(); i++ ) {
							CEtSoundEngine::GetInstance().SetMute( CEtSoundEngine::GetInstance().GetChannelGroup(i)->GetName(), true );
						}
					}
					m_bMinimizeFlag = true;
				}
			}
			else {
				if( m_bMinimizeFlag ) {
					if( CGameOption::IsActive() )
						CGameOption::GetInstance().ApplySound();
					m_bMinimizeFlag = false;
				}
			}
		}
	}

#ifdef PRE_ADD_QUICK_PVP
	m_AcceptRequestQuickPvPInvite.Process(fDelta);
#endif

#ifdef PRE_ADD_WEEKLYEVENT
	ProcessTimeEvent( LocalTime, fDelta );
#endif

#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	GetTableDB().Process( fDelta );
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
	// Process Interface
//	GetInterface().Process( LocalTime, fDelta );

	/*
	if ( m_pCutSceneTask && Play == m_eCutSceneState ) //m_pCutSceneTask
	{
		bool b = m_pCutSceneTask->IsPlayEnd();
		if ( b )
		{
			OutputDebug( "CutScene 종료! 다이얼로그 페이드 아웃!\n" );

			// fade out
			GetInterface().FadeDialog( 0x00000000, 0xff000000, 1.0f, this );
			m_eCutSceneState = End;
		}
	}
	*/

	/*
	if( s_PrevTestTime > LocalTime ) s_PrevTestTime = LocalTime;
	if( LocalTime - s_PrevTestTime > 1000 ) {
		s_PrevTestTime = LocalTime;
		OutputDebug( "CDnActor : %d ( %d )\n", CDnActor::GetItemCount(), CDnActor::GetProcessCount() );
		OutputDebug( "CDnEtcObject : %d ( %d )\n", CDnEtcObject::GetItemCount(), CDnEtcObject::GetProcessCount() );
		OutputDebug( "CDnWeapon : %d ( %d )\n", CDnWeapon::GetItemCount(), CDnWeapon::GetProcessCount() );
		OutputDebug( "CEtParticle : %d\n", CEtParticle::GetItemCount() );
		OutputDebug( "CEtEffectObject : %d\n", CEtEffectObject::GetItemCount() );
	}
	*/
}

void CDnCommonTask::OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize )
{
	if( CDnLoadingTask::IsActive() ) {
		if( CDnLoadingTask::GetInstance().InsertLoadPacket( this, nMainCmd, nSubCmd, (void*)pData, nSize ) ) return;
	}

	bool bProcessDispatch = false;
	switch( nMainCmd ) {
		case SC_CHAR:		OnRecvCharMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_NPC:		OnRecvNpcMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_CHAT:		OnRecvChatMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_QUEST:		OnRecvQuestMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_TRIGGER:	OnRecvTriggerMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_SYSTEM:		OnRecvSystemMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
		case SC_ETC:		OnRecvEtcMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
#ifdef PRE_ADD_QUICK_PVP
		case SC_PVP:		OnRecvPvPMessage( nSubCmd, pData, nSize ), bProcessDispatch = true; break;
#endif // PRE_ADD_QUICK_PVP
	}
	if( bProcessDispatch ) {
		WriteLog( 1, ", Info, CDnCommonTask::OnDispatchMessage End : nMainCmd(%d), nSubCmd(%d), nSize(%d)\n", nMainCmd, nSubCmd, nSize );
	}
}

void CDnCommonTask::OnRecvCharMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChar::SC_ENTERNPC: OnRecvCharNpcEntered( (SCEnterNpc *)pData ); break;
		case eChar::SC_LEAVENPC: OnRecvCharNpcLeaved( (SCLeaveNpc *)pData ); break;
		case eChar::SC_ENTERPROPNPC: OnRecvCharPropNpcEntered( (SCEnterPropNpc *)pData ); break;
		case eChar::SC_LEAVEPROPNPC: OnRecvCharPropNpcLeaved( (SCLeavePropNpc *)pData ); break;
		case eChar::SC_PCBANG: OnRecvCharPCBang( (SCPCBang *)pData ); break;
		case eChar::SC_MOD_COMMONVARIABLE: OnRecvCharCommonVariable( (SCModCommonVariable*)pData ); break;
		case eChar::SC_ATTENDANCEEVENT: OnRecvCheckAttedanceResult( (SCAttendanceEvent*)pData ); break;
	}
}

void CDnCommonTask::OnRecvNpcMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) // 타입에따른 Npc의 사운드 재생을 위한 패킷 구분.
	{
	case eNpc::SC_SHOWWAREHOUSE:
	case eNpc::SC_OPENCOMPOUND_EMBLEM:
	case eNpc::SC_OPENUPGRADE_JEWEL:
	case eNpc::SC_OPENUPGRADE_ITEM:		
	case eNpc::SC_OPENCOMPOUND_ITEM:		
	case eNpc::SC_OPENGUILDMGRBOX:			
	case eNpc::SC_OPEN_FARMWAREHOUSE:		
	case eNpc::SC_OPENCOMPOUND2_ITEM:	
	case eNpc::SC_OPEN_GLYPH_LIFT:
		{
			DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
			DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

			if ( hNpc ) {
				CDnNPCActor* pActor = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
				if ( pActor ) {
					pActor->OnSoundPlay( CDnNPCActor::OpenShop );
				}
			}
		}
	}

	switch( nSubCmd ) {
		case eNpc::SC_NPCTALK: OnRecvNpcTalkMsg( (SCNpcTalk*)pData );	break;
		case eNpc::SC_SHOWWAREHOUSE: OnRecvShowWareHouse(pData); break;
		case eNpc::SC_OPENCOMPOUND_EMBLEM: OnRecvShowCompoundEmblem(pData); break;
		case eNpc::SC_OPENUPGRADE_JEWEL: OnRecvShowUpgradeJewel(pData); break;
		case eNpc::SC_OPENMAILBOX:	OnRecvShowMailBox(pData); break;
		case eNpc::SC_OPENDISJOINT_ITEM:	OnRecvShowDisjointItem(pData); break;
		case eNpc::SC_OPENUPGRADE_ITEM:	OnRecvShowUpgradeItem( (SCOpenUpgradeItem*)pData ); break;
		case eNpc::SC_OPENCOMPOUND_ITEM:	OnRecvShowCompoundItem( (SCOpenCompoundItem*)pData ); break;
		case eNpc::SC_OPENGUILDMGRBOX:	OnRecvShowGuildMgrBox( (SCOpenGuildMgrBox*)pData ); break;
		case eNpc::SC_OPEN_DARKLAIRRANKBOARD:	OnRecvDarkLairRankBoard( (SCOpenDarkLairRankBoard*)pData ); break;
		case eNpc::SC_OPEN_PVPLADDERRANKBOARD: OnRecvPVPLadderRankBoard( (SCOpenPvPLadderRankBoard*)pData ); break;
		case eNpc::SC_OPEN_FARMWAREHOUSE :	OnRecvShowFarmWareHouse( pData );	break;
		case eNpc::SC_OPEN_MOVIEBROWSER : OnRecvOpenMovieBrowser(pData); break;
		case eNpc::SC_OPEN_BROWSER : OnRecvOpenBrowser((SCOpenBrowser*)pData); break;
		case eNpc::SC_OPENCOMPOUND2_ITEM:	OnRecvShowCompound2Item( (SCOpenCompound2Item*)pData ); break;
		case eNpc::SC_OPEN_GLYPH_LIFT:		OnRecvShowGlyphLift(pData);	break;
		case eNpc::SC_OPEN_INVENTORY: OnRecvShowInventory(pData); break;
#ifdef PRE_ADD_EXCHANGE_ENCHANT
		case eNpc::SC_OPEN_EXCHANGE_ENCHANT: OnRecvShowExchangeEnchant(pData); break;
#endif
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
		case eNpc::SC_OPEN_TEXTURE_DIALOG: OnRecvShowTextureDIalog((SCOpenTextureDialog*)pData); break;
		case eNpc::SC_CLOSE_TEXTURE_DIALOG: OnRecvCloseTextureDialog((SCCloseTextureDialog*)pData); break;
#endif
		case eNpc::SC_OPEN_CHANGEJOB_DIALOG: OnRecvChangeJobDialog((SCOpenChangeJobDialog*)pData); break;

	}
}

void CDnCommonTask::OnRecvQuestMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eQuest::SC_PLAYCUTSCENE: OnRecvQuestPlayCutScene( (SCPlayCutScene*)pData ); break;
		case eQuest::SC_COMPLETE_CUTSCENE: OnRecvQuestCompleteCutScene( (SCCompleteCutScene*)pData ); break;
		case eQuest::SC_SKIP_CUTSCENE: OnRecvQuestSkipCutScene( (SCSkipCutScene*)pData ); break;
		case eQuest::SC_SKIPALL_CUTSCENE: OnRecvQuestSkipAllCutScene( pData ); break;
	}
}

void CDnCommonTask::OnRecvChatMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eChat::SC_CHATMSG: OnRecvChatChatMsg( (SCChat *)pData, nSize ); break;
		case eChat::SC_WORLDSYSTEMMSG: OnRecvChatWorldSystemMsg( (SCWorldSystemMsg *)pData, nSize ); break;
		case eChat::SC_NOTICE: OnRecvChatNoticeMsg((SCNotice*)pData, nSize); break;
		case eChat::SC_GUILDCHAT: OnRecvChatGuildChat( (SCGuildChat *)pData, nSize ); break;
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
		case eChat::SC_DOORS_GUILDCHAT: OnRecvChatDoorsGuildChat((SCDoorsGuildChat*)pData, nSize); break;
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
		case eChat::SC_CANCELNOTICE: OnRecvChatNoticeCancelMsg(); break;
		case eChat::SC_SERVERMSG: OnRecvChatServerMsg( (SCChatServerMsg *)pData, nSize ); break;
	}
}

void CDnCommonTask::OnRecvTriggerMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eTrigger::SC_TRIGGER_CALLACTION:	OnRecvTriggerCallAction( (SCTriggerCallAction*)pData ); break;
		case eTrigger::SC_CHANGE_MY_BGM:		OnRecvTriggerChangeMyBGM( (SCChangeMyBGM*)pData ); break;
		case eTrigger::SC_RADIO_IMAGE:			OnRecvTriggerRadioImage( (SCRadioImage*)pData ); break;
		case eTrigger::SC_FILE_TABLE_BGM_OFF:	OnRecvTriggerFileTableBGMOff( (SCFileTableBGMOff*)pData );	break;
		case eTrigger::SC_TRIGGER_FORCE_ENABLE_RIDE:	OnRecvTriggerForceEnableRide( (SCForceEnableRide*)pData );	break;
	}
}

void CDnCommonTask::OnRecvSystemMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd ) {
		case eSystem::SC_SERVICECLOSE: OnRecvServiceClose((SCServiceClose*)pData); break;
	}
}

void CDnCommonTask::OnRecvChatChatMsg( SCChat *pPacket, int nSize )
{
	_ASSERT(pPacket);

	switch( pPacket->nRet )
	{
	case ERROR_CHAT_USERNOTFOUND:
		{
			GetInterface().AddChatMessage( pPacket->eType, pPacket->wszFromCharacterName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 660) );
			GetInterface().DelPrivateName( pPacket->wszFromCharacterName );
			return;
		}
		break;
	case ERROR_CHAT_DONTSAYYOURSELF:
		{
			GetInterface().AddChatMessage( pPacket->eType, pPacket->wszFromCharacterName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 661) );
			GetInterface().DelPrivateName( pPacket->wszFromCharacterName );
			return;
		}
		break;
#ifdef PRE_ADD_DOORS
	case ERROR_CHAT_MOBILESENDED:
		{
			GetInterface().AddChatMessage( pPacket->eType, pPacket->wszFromCharacterName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6484) );
			GetInterface().DelPrivateName( pPacket->wszFromCharacterName );
			return;
		}
#endif		//#ifdef PRE_ADD_DOORS
	}

	WCHAR wszChatMsg[CHATLENMAX] = {0,};

	memcpy(wszChatMsg, pPacket->wszChatMsg, pPacket->nLen * 2);

	GetInterface().AddChatMessage( pPacket->eType, pPacket->wszFromCharacterName, wszChatMsg );
}

void CDnCommonTask::OnRecvChatWorldSystemMsg( SCWorldSystemMsg *pPacket, int nSize )
{
	//GetInterface().AddWorldSystemMessage( pPacket->cType, pPacket->wszFromCharacterName, pPacket->nID, pPacket->nValue );
	GetInterface().AddWorldSystemMessage( pPacket );
}

void CDnCommonTask::OnRecvChatNoticeMsg(SCNotice * pPacket, int nSize)
{
	if (pPacket->nLen >= CHATLENMAX) return;

	WCHAR szMsg[CHATLENMAX];
	memset(szMsg, 0, sizeof(szMsg));
	_wcscpy(szMsg, _countof(szMsg), pPacket->wszNoticeMsg, pPacket->nLen);

	GetInterface().AddNoticeMessage(szMsg, pPacket->nShowSec);
}

void CDnCommonTask::OnRecvChatGuildChat(SCGuildChat *pPacket, int nSize)
{
	if( pPacket->nLen >= CHATLENMAX ) return;

	WCHAR szMsg[CHATLENMAX];
	memset(szMsg, 0, sizeof(szMsg));
	_wcscpy(szMsg, _countof(szMsg), pPacket->wszChatMsg, (int)wcslen(pPacket->wszChatMsg));

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	GetGuildTask().OnRecvGuildChat(szMsg, pPacket->nCharacterDBID, false);
#else
	GetGuildTask().OnRecvGuildChat(szMsg, pPacket->nCharacterDBID);
#endif
}

#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
void CDnCommonTask::OnRecvChatDoorsGuildChat(SCDoorsGuildChat *pPacket, int nSize)
{
	if( pPacket->nLen >= CHATLENMAX ) return;

	WCHAR szMsg[CHATLENMAX];
	memset(szMsg, 0, sizeof(szMsg));
	_wcscpy(szMsg, _countof(szMsg), pPacket->wszChatMsg, (int)wcslen(pPacket->wszChatMsg));

	GetGuildTask().OnRecvGuildChat(szMsg, pPacket->biCharacterDBID, true);
}
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT

void CDnCommonTask::OnRecvChatNoticeCancelMsg()
{
	GetInterface().CloseSlideCaptionMessage( false );
}

void CDnCommonTask::OnRecvChatServerMsg( SCChatServerMsg *pPacket, int nSize )
{
	GetInterface().AddServerMessage( pPacket->cType, pPacket->cStringType, pPacket->nMsgIndex, pPacket->cCaption, pPacket->cFadeTime );
}

// Rotha : 현재 퀘스트는 스크립트를 따라가는것이 정상이나 , 현재 내가 가지고 있는 퀘스트중에 완료된 퀘스트를 따로 모아서
// 예외가 되는 상황입니다 , 스크립트가 아닌 프로그램적으로 퀘스트를 구성하게 됩니다.
bool CDnCommonTask::OpenCompleteNpcTalk(int nNpcIndex)
{
	CDnQuestTask *pTask = (CDnQuestTask*)CTaskManager::GetInstance().GetTask( "QuestTask" );
	if( !pTask ) 
		return false;

	m_nVecCompleteQuest.clear();

	const TQuestGroup* pQuestGroup = pTask->GetQuestGroup();
	if ( !pQuestGroup )
		return false;

	for ( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		int nQuestIdx = pQuestGroup->Quest[i].nQuestID;
		int nJournalIdx = (int)pQuestGroup->Quest[i].cQuestJournal;

		if ( pQuestGroup->Quest[i].nQuestID > 0 )
		{
			Journal* pJournal = g_DataManager.GetJournalData(pQuestGroup->Quest[i].nQuestID);
			if ( pJournal )
			{
				for ( int j = 0 ; j < (int)pJournal->JounalPages.size() ; j++ )
				{
					if ( pJournal->JounalPages[j].nJournalPageIndex == nJournalIdx &&
						pJournal->JounalPages[j].nStateMarkType == CDnNPCActor::CrossReQuestComplete)
					{
						m_nVecCompleteQuest.push_back(pQuestGroup->Quest[i].nQuestID);
					}
				}
			}
		}
	}
	
	CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
	if( pNpcDlg )
	{
		std::vector<TALK_ANSWER> nAnswerList;

		for(int i=0 ;i<(int)m_nVecCompleteQuest.size();i++)
		{
			Journal* pJournal = g_DataManager.GetJournalData(m_nVecCompleteQuest[i]);
			
			if(pJournal)
			{
				TALK_ANSWER Temp;
				memset(&Temp,0,sizeof(TALK_ANSWER));
				
				Temp.szAnswer =pJournal->wszQuestTitle;
				nAnswerList.push_back(Temp);
			}
		}

		CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
		if( pNpcDlg )
		{
			if(m_nVecCompleteQuest.empty()) // 완료시킬 퀘가 없다. 메세지를 띄워주자
				pNpcDlg->SetNpcTalk( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200007 ),"GQ_normal_end.dds", 0 ); // 완료가능 퀘스트 존재시
			else
				pNpcDlg->SetNpcTalk( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200006 ),"GQ_normal_end.dds", 0 ); // 완료가능 퀘스트 없을시

			pNpcDlg->SetAnswer( nAnswerList, false );

			if( !pNpcDlg->IsShow() )
			{
				GetInterface().OpenNpcDialog(this);
			}

			m_bForceOpenRecompense = true;
			m_nCurrentTalkNpcID = nNpcIndex;

			return true;
		}

	}

	return false;
}

bool CDnCommonTask::OpenCompleteRewardMsg()
{
	CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
	if( !pNpcDlg ) 
		return false;

	if(m_bForceOpenRecompense)
	{
		int nSelectedLineIndex = pNpcDlg->GetAnswerLineIndex();
		TQuestRecompense questRecompense;

		if((nSelectedLineIndex == -1) || ((int)m_nVecCompleteQuest.size() < nSelectedLineIndex))
			return false;

		bool bResult = g_DataManager.GetQuestRecompenseByQuestIndex( m_nVecCompleteQuest[nSelectedLineIndex], questRecompense );
		if  ( !bResult )
		{
			WriteLog( 1, ", Error, can't find recompense table : %d ", m_nVecCompleteQuest[nSelectedLineIndex] );
			return false;
		}

		std::vector<TALK_ANSWER> nAnswerList;
		TALK_ANSWER Temp;
		memset(&Temp,0,sizeof(TALK_ANSWER));

		Temp.szAnswer = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200009 ); // 클릭꾸
		nAnswerList.push_back(Temp);

		pNpcDlg->SetRecompense( questRecompense, true );
		pNpcDlg->SetNpcTalk( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200008 ), "", 0 ); // UISTRING 작업해야함
		pNpcDlg->SetAnswer( nAnswerList, false );

		m_nSelectedCompleteQuestIndex = m_nVecCompleteQuest[nSelectedLineIndex];
		m_bForceOpenRecompense = false;
	}
	else
	{
		if(m_nSelectedCompleteQuestIndex )
		SendShortCutQuest(m_nSelectedCompleteQuestIndex);
		if(GetInterface().IsOpenBlind())
		{
			GetInterface().CloseNpcDialog();
			CDnLocalPlayerActor::LockInput(false);
		}
	}

	return true;
}

bool CDnCommonTask::IsHaveCompletedQuest()
{
	CDnQuestTask *pTask = (CDnQuestTask*)CTaskManager::GetInstance().GetTask( "QuestTask" );
	if( !pTask ) 
		return false;

	const TQuestGroup* pQuestGroup = pTask->GetQuestGroup();
	if ( !pQuestGroup )
		return false;

	for ( int i = 0 ; i < MAX_PLAY_QUEST ; i++ )
	{
		int nQuestIdx = pQuestGroup->Quest[i].nQuestID;
		int nJournalIdx = (int)pQuestGroup->Quest[i].cQuestJournal;

		if ( pQuestGroup->Quest[i].nQuestID > 0 )
		{
			Journal* pJournal = g_DataManager.GetJournalData(pQuestGroup->Quest[i].nQuestID);
			if ( pJournal )
			{
				for ( int j = 0 ; j < (int)pJournal->JounalPages.size() ; j++ )
				{
					if ( pJournal->JounalPages[j].nJournalPageIndex == nJournalIdx &&
						pJournal->JounalPages[j].nStateMarkType == CDnNPCActor::CrossReQuestComplete)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
};

void CDnCommonTask::OnRecvNpcTalkMsg( SCNpcTalk *pPacket )
{
	SetNpcTalkRequestWait( false );
	//#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	//	GetInterface().ClearNpcTalkTextureWindow();// 대화가 넘어가면 강제로 텍스쳐를 닫는다.
	//#endif
	DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(pPacket->nNpcUniqueID);
	CDnNPCActor*  pNpc = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
	if(pNpc && pNpc->IsNpcActor())
	{
		m_nCurrentTalkNpcID = pNpc->GetNpcData().nNpcID;

#ifdef PRE_ADD_STAMPSYSTEM
		CDnMainDlg * pMainDlg = GetInterface().GetMainBarDialog();
		if( pMainDlg )
			pMainDlg->NpcTalk( m_nCurrentTalkNpcID );
#endif // PRE_ADD_STAMPSYSTEM
	}

	if( pPacket->bIsEnd == true ) {
		GetInterface().CloseNpcDialog();
		EndNpcTalk( false );
#ifdef PRE_ADD_MAINQUEST_UI
		if(GetInterface().IsOpenMainQuestDlg())
			GetInterface().CloseMainQuestDlg();
#endif
		// Npc 가 싱크가 안맞거나 틀려졌을경우에 멈추게 된다. 멈추는것 보단 풀리는게 좋을듯하여 일단 이렇게 처리.
		if( pPacket->nNpcUniqueID == 0xFFFFFFFF ) {
			CDnLocalPlayerActor::LockInput(false);
		}
		return;
	}

	if(!hNpc)
	{
#ifndef _FINAL_BUILD
		std::wstring wszLog = FormatW(L"talk error [%d] ", pPacket->nNpcUniqueID);
		GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"", wszLog.c_str());
#endif
		if( GetInterface().GetBlindDialog() && !GetInterface().GetBlindDialog()->IsShow() ) {
			CDnLocalPlayerActor::LockInput(false);
		}

		GetInterface().CloseNpcDialog();
//		CDnLocalPlayerActor::SetTakeNpcUID(0xffffffff);
//		CDnLocalPlayerActor::LockInput(false);
#ifdef PRE_ADD_MAINQUEST_UI
		GetInterface().CloseMainQuestDlg();
#endif
		EndNpcTalk();
		return;
	}

	struct __StaticParamCallbackImpl : public StaticParamCallback
	{
		virtual void OnAction(std::string& szActionName)
		{
			if ( hNpc )
				hNpc->SetActionQueue(szActionName.c_str());
		}

		virtual void OnSound(std::string& szActionName) 
		{
			int nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( szActionName.c_str(), true, false );
			if ( nSoundIdx > -1 )
			{
				EtSoundChannelHandle hScriptSoundChannel = CEtSoundEngine::GetInstance().PlaySound__("3D", nSoundIdx );

				if( !hNpc )
					return;

				CDnNPCActor * pNpcActor = (CDnNPCActor *)(hNpc.GetPointer());
				if( NULL == pNpcActor )
					return;

				pNpcActor->RemoveScriptSoundChannel();
				pNpcActor->SetScriptSoundChannel( hScriptSoundChannel );
			}

		}

		DnActorHandle hNpc;
	};
	
	__StaticParamCallbackImpl sCallback;
	sCallback.hNpc = hNpc;

	CDnLocalPlayerActor::LockInput(true);
	std::wstring wszIndex;
	std::wstring wszTarget;

	bool bResult_Index = false;
	bool bResult_File  = false;
	bResult_Index = g_DataManager.GetTalkIndexName(pPacket->dwIndexHashCode, wszIndex);
	bResult_File  = g_DataManager.GetTalkFileName(pPacket->dwTargetHashCode, wszTarget);

	if( !bResult_Index )
	{
#ifndef _FINAL_BUILD
		GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"wszIndex", L"GetTalkIndexName Error!!");
#endif 
		GetInterface().CloseNpcDialog();
#ifdef PRE_ADD_MAINQUEST_UI
		GetInterface().CloseMainQuestDlg();
#endif
		EndNpcTalk();
	}

	if( !bResult_File )
	{
#ifndef _FINAL_BUILD
		GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"wszIndex", L"GetTalkFileName Error!!");
#endif
		GetInterface().CloseNpcDialog();
#ifdef PRE_ADD_MAINQUEST_UI
		GetInterface().CloseMainQuestDlg();
#endif
		EndNpcTalk();
	}

#ifndef _FINAL_BUILD
	std::wstring __wszIndex  = FormatW(L"wszIndex  [%s:%u], ", wszIndex.c_str(), pPacket->dwIndexHashCode);
	std::wstring __wszTarget = FormatW(L"wszTarget [%s:%u], ", wszTarget.c_str(), pPacket->dwTargetHashCode);

	GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"wszIndex", __wszIndex.c_str());
	GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"wszTarget", __wszTarget.c_str());
#endif

	std::vector<TalkParam> TalkParamList;
	if ( pPacket->nCount > 0 )
	{
		for ( int i = 0 ; i < pPacket->nCount ; i++ )
			TalkParamList.push_back( pPacket->TalkParamArray[i] );
	}

	TALK_PARAGRAPH talk_para;
	if ( g_DataManager.GetTalkParagraph(wszIndex, wszTarget, TalkParamList, talk_para, (StaticParamCallback*)&sCallback) == false )
	{
#ifndef _FINAL_BUILD
		std::wstring wszLog = FormatW(L"GetTalkParagraph Fail [%s:%s], ", wszIndex.c_str(), wszTarget.c_str());
		GetInterface().AddChatMessage(CHATTYPE_NORMAL, L"", wszLog.c_str());
#endif 
		GetInterface().CloseNpcDialog();
#ifdef PRE_ADD_MAINQUEST_UI
		GetInterface().CloseMainQuestDlg();
#endif
		//CDnLocalPlayerActor::SetTakeNpcUID(0xffffffff);
		//CDnLocalPlayerActor::LockInput(false);
		EndNpcTalk();
		return;
	}

	// 퀘스트 대화인지 구분한다.
	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo(wszTarget);
	bool bQuestTalk = false;
	if(pQuestInfo) bQuestTalk = true;

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	// 일반대화 & 퀘스트 대화인지 체크.
	bool bIsNpcTalk = true;
	if( wcscmp(talk_para.szIndex.c_str() , L"001") == 0 || wcscmp(talk_para.szIndex.c_str() , L"002") == 0 ) {
		bIsNpcTalk = false;
	}
#endif

#ifdef PRE_ADD_MAINQUEST_UI
	// 리뉴얼된 메인 퀘스트인지 체크
	if( OpenRenewalMainQuestUI(pQuestInfo, talk_para, pPacket->nNpcUniqueID) )
		return;
#endif

	CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
	if( pNpcDlg )
	{
		pNpcDlg->SetNpcTalk( talk_para.Question.szQuestion.c_str(), talk_para.szImageName, talk_para.nItemIndex );
#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
		pNpcDlg->SetAnswer( talk_para.Answers, bIsNpcTalk );
#else
		pNpcDlg->SetAnswer( talk_para.Answers, bQuestTalk );
#endif

		if( !pNpcDlg->IsShow() )
		{
			GetInterface().OpenNpcDialog(this);
		}
	}
	
	CDnLocalPlayerActor::SetTakeNpcUID(pPacket->nNpcUniqueID);
	
	OutputDebug("OnTRecvNpcTalkMsg\n");

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	GetInterface().ShowQuestReturnButton(true);
#endif
}

void CDnCommonTask::OnRecvShowWareHouse(char* pData )
{
	SCShowWareHouse* pPacket = reinterpret_cast<SCShowWareHouse*>(pData);
	GetInterface().OpenStorageDialog( pPacket->iItemID );
}

void CDnCommonTask::OnRecvShowCompoundEmblem( char* pData )
{
	//GetInterface().ShowEmblemDialog(true);
	GetInterface().OpenPlateMainDialog();
}

void CDnCommonTask::OnRecvShowUpgradeJewel( char* pData )
{
	// 업그레이드쥬얼은 현재 사용하지 않는다.
	//GetInterface().ShowJewelCompoundDialog(true);
}

void CDnCommonTask::OnRecvShowMailBox(char* pData)
{
	GetInterface().OpenMailDialog(false, _T(""), _T(""));
}

void CDnCommonTask::OnRecvShowFarmWareHouse( char * pData )
{
	GetInterface().ShowLifeStorageDialog( true );
}

void CDnCommonTask::OnRecvOpenMovieBrowser(char* pData)
{
	// note by kalliste : 일단 NPC Movie Browser를 호출하는 부분이 중국으로 한정되어 있어 url이나 type을 네트워크로 보내는 대신 클라이언트에서 셋팅하는 것으로 결정.
	float browserWidth = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserWidth);
	float browserHeight = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserHeight);

#ifdef _CH
	CDnInterface::GetInstance().OpenBrowser(std::string("http://act.dn.sdo.com/dntv/program.html"), browserWidth, browserHeight, CDnInterface::eBPT_CENTER, eGBT_NPCBROWSER);
#else
	CDnInterface::GetInstance().MessageBox(L"Movie Browser OpenCall Error"); // uistring by kalliste
#endif // _CH
}

void CDnCommonTask::OnRecvOpenBrowser(SCOpenBrowser* pData)
{
	CDnInterface::GetInstance().OpenBrowser(std::string(pData->url), float(pData->nWidth), float(pData->nHeight), CDnInterface::eBPT_CENTER, eGBT_NPCBROWSER);
}

void CDnCommonTask::OnRecvShowDisjointItem(char* pData)
{
	GetInterface().OpenItemDisjointDialog();
}

void CDnCommonTask::OnRecvShowUpgradeItem( SCOpenUpgradeItem* pPacket )
{
	GetInterface().OpenItemUpgradeDialog( pPacket->iItemID );
}

#ifdef PRE_ADD_EXCHANGE_ENCHANT
void CDnCommonTask::OnRecvShowExchangeEnchant(char* pData)
{
	GetInterface().OpenItemUpgradeExchangeDialog();
}
#endif

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
void CDnCommonTask::OnRecvShowTextureDIalog(SCOpenTextureDialog* pPacket)
{
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( pPacket->nFileIndex );
	if( szFileName == NULL ) return;

	int nTime = pPacket->nTime;
	if( nTime <= 0 ) nTime = INT_MAX;
	EtTextureHandle hTexture = EternityEngine::LoadTexture( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str() );
	int nResult = GetInterface().DrawTextureWindow( hTexture, pPacket->fX, pPacket->fY, pPacket->nPosition, (float)nTime, pPacket->nDialogIndex, pPacket->bAutoCloseDialog, NULL);	
	
	SetNpcTalkRequestWait(false);
}

void CDnCommonTask::OnRecvCloseTextureDialog(SCCloseTextureDialog* pPacket )
{	
	GetInterface().CloseTextureWindow(pPacket->nDialogID , true);
}
#endif // PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG

void CDnCommonTask::SendWindowState(short sWinState)
{
	CSWindowState packet;
	packet.sWinState = sWinState;

	CClientSessionManager::GetInstance().SendPacket(CS_ETC, eEtc::eCSEtc::CS_WINDOW_STATE, (char*)&packet, sizeof(CSWindowState));
}

void CDnCommonTask::OnRecvShowCompoundItem( SCOpenCompoundItem* pPacket )
{
	GetInterface().OpenItemCompoundDialog( pPacket->nCompoundShopID );
}

void CDnCommonTask::OnRecvShowCompound2Item( SCOpenCompound2Item* pPacket )
{
	GetInterface().OpenItemCompound2Dialog( pPacket->nCompoundGroupID, pPacket->iItemID );
}

void CDnCommonTask::OnRecvShowGuildMgrBox( SCOpenGuildMgrBox* pPacket )
{
	GetInterface().ShowGuildMgrBox( pPacket->nGuildMgrNo );
}

void CDnCommonTask::OnRecvCharNpcEntered( SCEnterNpc *pPacket )
{
	if( CDnLoadingTask::IsActive() ) {
		CDnLoadingTask::GetInstance().InsertLoadObject( 
			OnLoadRecvCharNpcEnteredCallback, 
			OnLoadRecvCharNpcEnteredUniqueID,
			OnCheckLoadingPacket, 
			NULL, 
			(void*)this, (void*)pPacket, sizeof(SCEnterNpc), m_LocalTime, CDnLoadingTask::CreateNpc );
	}

#if defined(PRE_OPT_CLIENT_QUEST_SCRIPT)
	g_DataManager.LoadNpcTalkData( pPacket->nNpcID );
	g_DataManager.LoadNpcQuestData( pPacket->nNpcID );
#endif	// #if defined(PRE_OPT_CLIENT_QUEST_SCRIPT)

	CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
	if( pMainMenuDlg ) pMainMenuDlg->RefreshZoneMapNPCActor();
}

bool __stdcall CDnCommonTask::OnLoadRecvCharNpcEnteredCallback( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime )
{
	SCEnterNpc *pPacket = (SCEnterNpc *)pParam;
	DnActorHandle hActor = CreateNpcActor( pPacket->nNpcObjectID, pPacket->nNpcID, pPacket->Position, pPacket->fRotate );

	// 필드일경우엔 GameTask 리스트에 알려줘야한다.
	if( hActor ) {
		CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask( "GameTask" ));
		if( pGameTask ) {
			pGameTask->InsertNpcList( hActor );
		}
	}

	// 유니크 카운트 클라이언트에도 동일하게 적용. 
	CDnActor::s_dwUniqueCount = pPacket->nNpcObjectID + 1;

	return true;
}

int __stdcall CDnCommonTask::OnLoadRecvCharNpcEnteredUniqueID( void *pParam, int nSize )
{
	return ((SCEnterNpc *)pParam)->nNpcObjectID;
}

void CDnCommonTask::OnRecvCharNpcLeaved( SCLeaveNpc *pPacket )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nObjectID );
	if( !hActor ) return;
	bool bSoftAppear = true;
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pTask && !pTask->IsSyncComplete() ) bSoftAppear = false;
	hActor->SetSoftAppear( bSoftAppear );
	hActor->SetDestroy();

	// 필드일경우엔 GameTask 리스트에 알려줘야한다.
	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask( "GameTask" ));
	if( pGameTask ) {
		pGameTask->RemoveNpcList( hActor );
	}

	CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
	if( pMainMenuDlg ) pMainMenuDlg->RefreshZoneMapNPCActor();
}

void CDnCommonTask::OnRecvCharPropNpcEntered( SCEnterPropNpc *pPacket )
{
	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance().FindPropFromCreateUniqueID( pPacket->nPropUniqueID, &pVecList );
	if( pVecList.empty() ) return;
	if( pVecList[0]->GetClassID() != PTE_Npc ) return;

	CDnWorldNpcProp *pProp = (CDnWorldNpcProp *)pVecList[0];
	DnActorHandle hActor = pProp->GetActorHandle();
	if( !hActor ) return;
	CDnNPCActor *pNpc = dynamic_cast<CDnNPCActor *>(hActor.GetPointer());
	if( !pNpc ) return;

	pNpc->EnableOperator( true );
	pNpc->SetUniqueID( pPacket->nNpcObjectID );
}

void CDnCommonTask::OnRecvCharPropNpcLeaved( SCLeavePropNpc *pPacket )
{
	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance().FindPropFromCreateUniqueID( pPacket->nPropUniqueID, &pVecList );
	if( pVecList.empty() ) return;
	if( pVecList[0]->GetClassID() != PTE_Npc ) return;

	CDnWorldNpcProp *pProp = (CDnWorldNpcProp *)pVecList[0];
	DnActorHandle hActor = pProp->GetActorHandle();
	if( !hActor ) return;
	CDnNPCActor *pNpc = dynamic_cast<CDnNPCActor *>(hActor.GetPointer());
	if( !pNpc ) return;

	pNpc->EnableOperator( false );
	pNpc->SetUniqueID( 0 );
}

void CDnCommonTask::OnRecvCharPCBang( SCPCBang *pPacket )
{
	if( !CDnInterface::IsActive() ) return;
	if( !CDnItemTask::IsActive() ) return;

#if defined(PRE_ADD_SHUTDOWN_CHILD) && defined(PRE_ADD_SELECTIVE_SHUTDOWN)
	CDnBridgeTask * pBridgeTask = static_cast<CDnBridgeTask *>( CTaskManager::GetInstance().GetTask("BridgeTask") );
	if( pBridgeTask ){
#if defined(_KRAZ)
		pBridgeTask->SetSelectiveShutDown( pPacket->Shutdown.nHour );
#else	// #if defined(_KRAZ)
		pBridgeTask->SetSelectiveShutDown( pPacket->nShutdownTime );
#endif	// #if defined(_KRAZ)
	}
#endif

	m_cPCBangGrade = pPacket->cPCBangGrade;

	if( CDnNestInfoTask::IsActive() ) 
		CDnNestInfoTask::GetInstance().SetPCBangNestCountInfo();

	if( pPacket->cPCBangGrade == PCBang::Grade::None )
		return;

	// PC방 혜택 알림. PC방이라면 혜택은 항상 알려준다.
	int nPCBangExp = 0, nPCBangMaxFTG = 0;
	int nPCBangRebirthCoin = 0, nPCBangClearBox = 0;
	bool bPCBangAppellation = false, bPCBangNestClearCount = false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPCCAFE );
	int nNumItem = pSox->GetItemCount();
	int nItemID = 0;
	for( int nItem = 0; nItem < nNumItem; ++nItem )
	{
		nItemID = pSox->GetItemID( nItem );
		int nType = pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();

		int nValue = 0;
		switch (pPacket->cPCBangGrade)
		{
		case PCBang::Grade::Premium:
			nValue = pSox->GetFieldFromLablePtr( nItemID, "_PremiumPCParam1" )->GetInteger();
			break;

		case PCBang::Grade::Gold:
			nValue = pSox->GetFieldFromLablePtr( nItemID, "_GoldPCParam1" )->GetInteger();
			break;

		case PCBang::Grade::Silver:
			nValue = pSox->GetFieldFromLablePtr( nItemID, "_SilverPCParam1" )->GetInteger();
			break;

		case PCBang::Grade::Red:
			nValue = pSox->GetFieldFromLablePtr( nItemID, "_RedPCParam1" )->GetInteger();
			break;

		default:
			nValue = pSox->GetFieldFromLablePtr( nItemID, "_PCParam1" )->GetInteger();
			break;
		}

		switch (nType)
		{
		case PCBang::Type::Exp:
			nPCBangExp = nValue;
			break;

		case PCBang::Type::Fatigue:
			nPCBangMaxFTG = nValue;
			CDnItemTask::GetInstance().SetMaxFatigue(CDnItemTask::ePCBANGFTG, nPCBangMaxFTG);
			break;

		case PCBang::Type::RebirthCoin:
			nPCBangRebirthCoin = nValue;
			break;

		case PCBang::Type::Appellation:
			if (bPCBangAppellation) continue;
			if (nValue > 0)
				bPCBangAppellation = true;
			break;

		case PCBang::Type::ClearBox:
			nPCBangClearBox = nValue;
			break;

		case PCBang::Type::NestClearCount:
			if (bPCBangNestClearCount) continue;
			if (nValue > 0)
				bPCBangNestClearCount = true;
			break;
		}
	}

#if defined(_KR)

	WCHAR wszRemainTime[128] = {0,};
	WCHAR wszSubStr[128] = {0,};
	WCHAR wszMsg[256] = {0,};

	switch( pPacket->cAuthorizeType )
	{
	case AddressDesc_F:
		{
			// 6205 프리미엄 적용 PC방입니다. 만료일은 %s 입니다.
			int nYear = pPacket->nArgument/10000;
			int nMonth = (pPacket->nArgument%10000)/100;
			int nDay = pPacket->nArgument%100;
			swprintf_s( wszRemainTime, _countof(wszRemainTime), L"%d/%d/%d", nYear, nMonth, nDay );
			wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6205 ), wszRemainTime );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszMsg, false );
		}
		break;
	case AddressDesc_M:
	case AddressDesc_I:
		{
			if( (pPacket->nArgument/60/24) > 0 )
			{
				// 6204 PC방 정량제 잔여 시간이 %s 남았습니다.(시간이 많이 남아있을테니 일,시간 단위로 표시하겠다.)
				// 4640 %d 일
				// 4075 %d 시간
				int nDay = pPacket->nArgument / 60 / 24;
				int nHour = (pPacket->nArgument / 60) % 24;
				if( nDay > 0 && nHour > 0 ) {
					swprintf_s( wszRemainTime, _countof(wszRemainTime), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4640 ), nDay );
					swprintf_s( wszSubStr, _countof(wszSubStr), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), nHour );
					wcscat_s( wszRemainTime, L" " );
					wcscat_s( wszRemainTime, wszSubStr );
				}
				else if( nDay ) {
					swprintf_s( wszRemainTime, _countof(wszRemainTime), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4640 ), nDay );
				}
				else {
					// 0시간도 포함시켜서 처리한다.
					swprintf_s( wszRemainTime, _countof(wszRemainTime), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), nHour );
				}
			}
			else
			{
				// 4075 %d 시간
				// 1209 분
				int nHour = pPacket->nArgument / 60;
				int nMinute = pPacket->nArgument % 60;
				if( nHour > 0 && nMinute > 0 ) {
					swprintf_s( wszRemainTime, _countof(wszRemainTime), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), nHour );
					swprintf_s( wszSubStr, _countof(wszSubStr), L"%d %s", nMinute, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1209 ) );
					wcscat_s( wszRemainTime, L" " );
					wcscat_s( wszRemainTime, wszSubStr );
				}
				else if( nHour ) {
					swprintf_s( wszRemainTime, _countof(wszRemainTime), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), nHour );
				}
				else {
					// 0분도 포함시켜서 처리한다.
					swprintf_s( wszRemainTime, L"%d ", nMinute );
					wcscat_s( wszRemainTime, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1209 ) );
				}
			}
			wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6204 ), wszRemainTime );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszMsg, false );
			break;
		}
		break;

	default:
		break;
	}
#endif	// #if defined(_KR)

#if defined(_WORK) || defined(_KR) || defined(_CH) || defined(_TH)
	WCHAR wszStr[256] = {0,};
	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6202 ), nPCBangExp, nPCBangMaxFTG );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );

	memset(&wszStr, 0, sizeof(wszStr));
	if (bPCBangAppellation){
		wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6208 ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
	}
	
	memset(&wszStr, 0, sizeof(wszStr));
	if (nPCBangClearBox > 0){
		wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6209 ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
	}

	memset(&wszStr, 0, sizeof(wszStr));
	if (bPCBangNestClearCount){
		wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6210 ) );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
	}
#endif	// 
}

void CDnCommonTask::OnRecvQuestPlayCutScene( SCPlayCutScene *pPacket )
{
	// 재생 가능한지 일단 체크좀 해서 넘기자.
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCUTSCENE );
	if( pSox->IsExistItem( pPacket->nCutSceneTableID ) == false ) return;
	std::string szFileName = pSox->GetFieldFromLablePtr( pPacket->nCutSceneTableID, "_FileName" )->GetString();

	bool bFind = false;
	CFileNameString szFullPathName = CEtResourceMng::GetInstance().GetFullName( szFileName, &bFind );
	m_bControlQuestScript = ( pPacket->nNpcObjectID == -1 ) ? false : true;
	if( !bFind ) 
	{
		// Note 한기: 파일을 찾지 못했을 경우엔 곧바로 서버에 complete 보내준다.
//		if( -1 == m_nQuestCutSceneID )
		if( !m_bControlQuestScript )
			SendCompleteCutScene( CDnBridgeTask::GetInstance().GetSessionID() );
		return;
	}

	// 게임일 때와 마을일 때로 나누자.
	// 게임에서도 Quest Script 에서 콜되는 경우 Trigger 에서 하는거와는 틀리게 돌아가기 때문에 ( 게임에서도 퀘스트로 처리시 혼자만 컷신 플레이 된다 등 )
	// 이런식으로 서버에서 퀘스트로 콜했는지 트리거로 콜햇는지 구분해서 처리하도록 합니다.
	// 변수명도 모호해서 Quest 용 CutScene 이라고 명확하게 변경해줍니다.
//	if( CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeEnum::MapTypeVillage )
	if( !m_bControlQuestScript )
	{
		if( CDnPartyTask::GetInstance().IsSyncComplete() ) 
		{
			// 게임 진행중인 상태. 페이드 끝나고 컷신 재생 시작.
			GetInterface().FadeDialog( 0x00000000, 0xff000000, 1.0f, this );

			char szTemp[16];
			sprintf_s( szTemp, "%d", pPacket->nCutSceneTableID );

			SAFE_DELETE( m_pFadeProcess );
			m_pFadeProcess = new InitTimeProcessStruct;
			m_pFadeProcess->Type = ITP_PlayCutScene;
			m_pFadeProcess->szVecParam.push_back( szTemp );
			sprintf_s( szTemp, "%d", pPacket->nQuestIndex );
			m_pFadeProcess->szVecParam.push_back( szTemp );
			sprintf_s( szTemp, "%d", pPacket->nQuestStep );
			m_pFadeProcess->szVecParam.push_back( szTemp );
			sprintf_s( szTemp, "%d", pPacket->dwQuestPlayerUniqueID );
			m_pFadeProcess->szVecParam.push_back( szTemp );
		}
		else 
		{
			// 맵 이동 하자마자 컷신 나오는 경우. 게임 시작되었을 때 바로 재생.
			char szTemp[4][16];
			sprintf_s( szTemp[0], "%d", pPacket->nCutSceneTableID );
			sprintf_s( szTemp[1], "%d", pPacket->nQuestIndex );
			sprintf_s( szTemp[2], "%d", pPacket->nQuestStep );
			sprintf_s( szTemp[3], "%d", pPacket->dwQuestPlayerUniqueID );

			AddInitTimeProcess( ITP_PlayCutScene, std::string(szTemp[0]), std::string(szTemp[1]), std::string(szTemp[2]), std::string(szTemp[3]) );
		}
	}
	else
	{
		// 마을에서 재생할 때는 페이드 아웃 시켜준다.
		GetInterface().FadeDialog( 0x00000000, 0xff000000, 1.0f, this );
		m_nQuestCutSceneID = pPacket->nCutSceneTableID;
		m_nQuestCutSceneNpcID = pPacket->nNpcObjectID;
		m_bQuestCutSceneAutoFadeIn = pPacket->bQuestAutoFadeIn;
	}
	m_bSkipCutScene = false;

	OutputDebug( "CutScene 시작! 다이얼로그 페이드 아웃!\n" );
}

void CDnCommonTask::OnRecvQuestCompleteCutScene( SCCompleteCutScene *pPacket )
{
	CDnVillageTask* pVillageTask = (CDnVillageTask*)(CTaskManager::GetInstance().GetTask("VillageTask"));
	if ( pVillageTask ) pVillageTask->EnableTaskProcess(true);

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if ( pGameTask ) pGameTask->EnableTaskProcess(true);

	if( pPacket->bFadeIn )
		GetInterface().FadeDialog( 0xff000000, 0x00000000, 1.0f, this );
}

void CDnCommonTask::OnRecvQuestSkipCutScene( SCSkipCutScene *pPacket )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );
	if( !hActor ) return;

	WCHAR wszStr[256];
	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100302 ), hActor->GetName() );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );

}

void CDnCommonTask::OnRecvQuestSkipAllCutScene( char *pPacket )
{
	SkipCutScene();
}

void CDnCommonTask::SkipCutScene()
{
	if( !m_pCutSceneTask ) return;
	m_bSkipCutScene = true;

	CDnCutSceneTask *pTask = (CDnCutSceneTask *)CTaskManager::GetInstance().GetTask( "CutSceneTask" );
	if( pTask ) pTask->FadeOutSkip();
}

void CDnCommonTask::OnRecvTriggerCallAction( SCTriggerCallAction *pPacket )
{
	if( !CDnWorld::IsActive() ) return;
	SectorIndex Index( 0, 0 );
	if( CDnWorld::GetInstance().GetGridX() ) {
		Index.nX = pPacket->wSectorIndex % CDnWorld::GetInstance().GetGridX();
		Index.nY = pPacket->wSectorIndex / CDnWorld::GetInstance().GetGridX();
	}
	CDnWorld::GetInstance().CallActionTrigger( Index, pPacket->wObjectIndex, pPacket->nRandomSeed );
}

void CDnCommonTask::OnRecvTriggerChangeMyBGM( SCChangeMyBGM *pPacket )
{
	if( !CDnWorld::IsActive() ) return;
	if( CDnActor::s_hLocalActor->GetUniqueID() != pPacket->nSessionID )
		return;

	CDnWorld::GetInstance().ChangeBattleBGM( CDnWorld::BattleBGMEnum::NoneBattle, 0.f, 15.f, 0.f, 0.f );

	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;
	const char *szFileName = CDnTableDB::GetInstance().GetFileName( pPacket->nBGM );
	if( szFileName == NULL ) return;

	float fTargetVolume = 1.f;
	for( DWORD i=0; i<pGrid->GetActiveSectorCount(); i++ ) {
		CDnWorldSector *pSector = (CDnWorldSector *)pGrid->GetActiveSector(i);
		if( !pSector ) continue;
		CDnWorldSound *pSound = (CDnWorldSound *)pSector->GetSoundInfo();
		if( !pSound ) continue;
		pSound->ChangeBGM( szFileName );
		pSound->FadeVolumeBGM( 1.f, CGlobalInfo::GetInstance().m_fFadeDelta );
	}

	CDnWorldEnvironment *pEnvi = CDnWorld::GetInstance().GetEnvironment();
	if( !pEnvi ) return;
	pEnvi->SetEnviBGMRatio( 0.0f );
}

void CDnCommonTask::OnRecvTriggerRadioImage( SCRadioImage *pPacket )
{
	DnActorHandle hActor;

	if( CDnActor::s_hLocalActor->GetUniqueID() == pPacket->nSessionID )
		hActor = CDnActor::s_hLocalActor;
	else
		hActor = CDnActor::FindActorFromUniqueID( pPacket->nSessionID );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TFILE );

	EtTextureHandle hTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( pSox->GetFieldFromLablePtr( pPacket->nFileID, "_FileName" )->GetString() ).c_str(), RT_TEXTURE );

	hActor->SetHeadIcon( hTexture, GetTickCount(), pPacket->nTime, true );
}

void CDnCommonTask::OnRecvTriggerFileTableBGMOff( SCFileTableBGMOff * pPacket )
{
	if( !CDnWorld::IsActive() ) return;
	if( CDnActor::s_hLocalActor->GetUniqueID() != pPacket->nSessionID )
		return;

	CDnWorldGrid *pGrid = (CDnWorldGrid *)CDnWorld::GetInstance().GetGrid();
	if( !pGrid ) return;

	for( DWORD i=0; i<pGrid->GetActiveSectorCount(); i++ ) {
		CDnWorldSector *pSector = (CDnWorldSector *)pGrid->GetActiveSector(i);
		if( !pSector ) continue;
		CDnWorldSound *pSound = (CDnWorldSound *)pSector->GetSoundInfo();
		if( !pSound ) continue;
		pSound->FadeVolumeBGM( 0.f, CGlobalInfo::GetInstance().m_fFadeDelta );
	}
}

void CDnCommonTask::OnRecvTriggerForceEnableRide( SCForceEnableRide * pPacket )
{
	if(!CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalPlayerActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());

	if( pLocalPlayerActor->GetUniqueID() != pPacket->nSessionID )
		return;

	pLocalPlayerActor->SetForceEnableRide( pPacket->bForceEnableRide );
}

void CDnCommonTask::OnRecvServiceClose(SCServiceClose * pPacket)
{
	if( CSyncTimer::IsActive() && CSyncTimer::GetInstance().IsStarted() == false )
		CSyncTimer::GetInstance().SetServerTime( pPacket->_tNow );
	m_tServerCloseTime = pPacket->_tCloseTime;
	m_nProcessedRemainTime = 0;
}

void CDnCommonTask::ProcessServerClose()
{
	if( m_tServerCloseTime == 0 ) return;
	if( CSyncTimer::IsActive() == false ) return;

	__time64_t tRemainTime = m_tServerCloseTime - CSyncTimer::GetInstance().GetCurTime();
	if( tRemainTime > 0 )
	{
		// 60초 남았을때 한번, 10초 이하부터 쭉 출력
		if( tRemainTime == 60 || tRemainTime <= 10 )
		{
			int nRemainTime = (int)tRemainTime;
			if( nRemainTime != m_nProcessedRemainTime )
			{
				WCHAR wszMsg[256] = {0,};
				swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 114058 ), nRemainTime );
				GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3, wszMsg, textcolor::HOTPINK, 4.0f );
				m_nProcessedRemainTime = nRemainTime;
			}
		}
	}
}

void CDnCommonTask::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 	
	{
	case NPC_DIALOG:
		{
			if( nCommand == EVENT_TEXTBOX_SELECTION )
			{
				// Rotha - 프로그램적으로 구성한 Answer에서 인덱스를 선택할경우 저장되어있는 퀘스트 벡터를 기반으로 Reward창을 띄우고 메세지를 보내게 됩니다.
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );
				if(pSox && pSox->IsExistItem(GetCurrentTalkNpcID()))
				{
					int nNpcType = pSox->GetFieldFromLablePtr(GetCurrentTalkNpcID(),"_NpcJobIndex")->GetInteger();
					if(nNpcType == CDnNPCActor::emJobType::typeCompleteQuest)
					{
						bool bOpenReward = OpenCompleteRewardMsg();

						if(bOpenReward)
							return;
					}
				}

				std::wstring szLinkIndex;
				std::wstring szLinkTarget;
				bool bResult(false);


				CDnNpcDlg *pNpcDlg = GetInterface().GetNpcDialog();
				if( pNpcDlg )
				{
					bResult = pNpcDlg->GetAnswerIndex(szLinkIndex, szLinkTarget, false);
					if (pNpcDlg->OnAnswer(GetCurrentTalkNpcID()) == true)
						return;
				}

				if (SendNpcTalkToNextStep(bResult == false, szLinkIndex, szLinkTarget) == false)
					return;
			}
		}
		break;
	case SKIP_FADE_DIALOG:
		if( nCommand == EVENT_FADE_COMPLETE ) {
			if( m_bSkipCutScene ) { // 스킵할경우..
				GetInterface().FadeDialog( 0xff000000, 0xff000000, 0.f, NULL, true );
				EndCutScene();
				break;
			}
		}
		break;
	case FADE_DIALOG:
		if( nCommand == EVENT_FADE_COMPLETE ) {
			if( m_pFadeProcess ) {
				ExecuteInitTimeProcess( m_pFadeProcess );
				SAFE_DELETE( m_pFadeProcess );
			}

			if( m_bControlQuestScript ) //-1 != m_nQuestCutSceneID )
			{
				PlayCutScene( m_nQuestCutSceneID );
			}
		}
		break;

#ifdef PRE_ADD_QUICK_PVP
	case ACCEPT_REQUEST_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
			if (CDnCashShopTask::IsActive() && CDnCashShopTask::GetInstance().IsOpenCashShop())
			{
				SendQuickPvPResult(false, m_CurrentAcceptRequestCache.m_SenderSessionID);
				OutputDebug( "IsOpenCashShop() - SendQuickPvPResult(false, m_CurrentAcceptRequestCache.m_SenderSessionID);\n" );

				if (m_CurrentAcceptRequestCache.m_SenderSessionID == m_AcceptRequestQuickPvPInvite.GetCurrentInfo().m_SenderSessionID)
					m_AcceptRequestQuickPvPInvite.PopAcceptRequestInfo();

				m_CurrentAcceptRequestCache.Clear();
				GetInterface().CloseAcceptRequestDialog();
				break;
			}
#endif
			if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
			{
				OutputDebug( "ID_OK() - SendQuickPvPResult(false, m_CurrentAcceptRequestCache.m_SenderSessionID);\n" );
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
				GetInterface().DisableCashShopMenuDlg(true);
#endif
				SendQuickPvPResult(true, m_CurrentAcceptRequestCache.m_SenderSessionID);
#ifdef PRE_MOD_INTEG_SYSTEM_STATE
				m_AcceptRequestQuickPvPInvite.ClearList();
#else
				m_AcceptRequestQuickPvPInvite.ClearInfoList();
#endif
			}
			else if( (strcmp( pControl->GetControlName(), "ID_REJECT" ) == 0) )
			{
				OutputDebug( "ID_REJECT() - SendQuickPvPResult(false, m_CurrentAcceptRequestCache.m_SenderSessionID);\n" );
				SendQuickPvPResult(false, m_CurrentAcceptRequestCache.m_SenderSessionID);

				if (m_CurrentAcceptRequestCache.m_SenderSessionID == m_AcceptRequestQuickPvPInvite.GetCurrentInfo().m_SenderSessionID)
					m_AcceptRequestQuickPvPInvite.PopAcceptRequestInfo();
			}

			m_CurrentAcceptRequestCache.Clear();
			GetInterface().CloseAcceptRequestDialog();
		}
		break;
#endif // PRE_ADD_QUICK_PVP

#ifdef PRE_ADD_SELECTIVE_SHUTDOWN
	case MESSAGEBOX_10:
		if( nCommand == EVENT_BUTTON_CLICKED ) 
		{
			if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 ) 
			{
				if( CDnLoadingTask::IsActive() ) 
					CDnLoadingTask::GetInstance().EnableBackgroundLoading( false, true );
				CTaskManager::GetInstance().RemoveAllTask( false );
			}
		}
		break;
#endif

#ifdef PRE_ADD_MAINQUEST_UI
	case MAINQUEST_DIALOG:
		{
			if( nCommand == EVENT_TEXTBOX_SELECTION || nCommand == EVENT_BUTTON_CLICKED )
			{
				// Rotha - 프로그램적으로 구성한 Answer에서 인덱스를 선택할경우 저장되어있는 퀘스트 벡터를 기반으로 Reward창을 띄우고 메세지를 보내게 됩니다.
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNPC );
				if(pSox && pSox->IsExistItem(GetCurrentTalkNpcID()))
				{
					int nNpcType = pSox->GetFieldFromLablePtr(GetCurrentTalkNpcID(),"_NpcJobIndex")->GetInteger();
					if( nNpcType == CDnNPCActor::emJobType::typeCompleteQuest) // 모험자 게시판에서 완료
					{
						if(OpenCompleteRewardMsg())
							return;
					}
				}

				std::wstring szLinkIndex;
				std::wstring szLinkTarget;
				bool bResult(false);
				
				CDnMainQuestDlg* pMainQuestDlg = GetInterface().GetMainQuestDlg();
				if(pMainQuestDlg)
				{
					bResult = pMainQuestDlg->GetAnswerIndex(szLinkIndex, szLinkTarget, nCommand, false);
					if( pMainQuestDlg->OnAnswer(GetCurrentTalkNpcID()) == true )
						return;
				}

				if( !SendNpcTalkToNextStep(bResult == false, szLinkIndex, szLinkTarget) )
					return;
			}
		}
		break;
#endif // PRE_ADD_MAINQUEST_UI

#ifdef PRE_ADD_GAMEQUIT_REWARD
	case 7957:
		CEtUIDialog * pDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pDlg )
		{
			CDnInvenTabDlg * pInvenDlg = (CDnInvenTabDlg *)pDlg;										
			if( !pInvenDlg->IsShow() )
				GetInterface().GetMainMenuDialog()->ToggleShowDialog( CDnMainMenuDlg::INVENTORY_DIALOG );

			// #78444 [게임 종료 시 상품 지급] 게임 종료 시 출력되는 팝업 메시지에서 [선물받기] 버튼을 누르면 캐시 탭이 아닌 제스쳐 탭이 선택된 채로 소지품 창이 출력됩니다.
			CEtUIRadioButton * pRBtn = pInvenDlg->GetCashTab();
			if( pRBtn )
				pInvenDlg->SetCheckedTab( pRBtn->GetTabID() );
		}
		break;
#endif // PRE_ADD_GAMEQUIT_REWARD
	}
}

bool CDnCommonTask::SendNpcTalkToNextStep(bool bCloseNpcDlg, const std::wstring& szIndex, const std::wstring& szTarget)
{
	if (bCloseNpcDlg || szIndex.empty() || szTarget.empty())
	{
#ifdef PRE_ADD_MAINQUEST_UI
		if(GetInterface().IsOpenMainQuestDlg())
			GetInterface().CloseMainQuestDlg();
#endif
		GetInterface().CloseNpcDialog();
		return false;
	}

	if( !IsRequestNpcTalk() )
	{
		UINT nNpcUID = CDnLocalPlayerActor::GetTakeNpcUID();
		SendNpcTalk(nNpcUID, szIndex, szTarget);
		SetNpcTalkRequestWait();
	}

	return true;
}

void CDnCommonTask::AddInitTimeProcess( InitTimeProcessEnum Type, std::string szParam1, std::string szParam2, std::string szParam3, std::string szParam4 )
{
	InitTimeProcessStruct Struct;
	Struct.Type = Type;

	if( !szParam1.empty() ) Struct.szVecParam.push_back( szParam1 );
	if( !szParam2.empty() ) Struct.szVecParam.push_back( szParam2 );
	if( !szParam3.empty() ) Struct.szVecParam.push_back( szParam3 );
	if( !szParam4.empty() ) Struct.szVecParam.push_back( szParam4 );

	m_VecInitTimeProcessList.push_back( Struct );
}

void CDnCommonTask::ResetInitTimeProcess()
{
	SAFE_DELETE_VEC( m_VecInitTimeProcessList );
}


void CDnCommonTask::PlayCutScene( int nCutSceneTableID, int nQuestIndex/* = -1*/, int nQuestStep/* = -1*/, DWORD dwQuestPlayerUniqueID/* = UINT_MAX*/ )
{
#if defined(PRE_ADD_SHUTDOWN_CHILD) && defined(PRE_ADD_SELECTIVE_SHUTDOWN)
	// 선택적셧다운제로 종료메세지박스가 떠있는 경우 컷씬을 무시하고 종료메세지박스를 띄운다.
	CDnBridgeTask * pTask = static_cast< CDnBridgeTask * >( CTaskManager::GetInstance().GetTask("BridgeTask") );
	if( pTask->IsShowMsgSelectiveShutDown() )
	{
		// 접속이 끊긴것처럼 처리해야해서 MESSAGEBOX_10 으로 콜백 처리하면 된다.
		GetInterface().ServerMessageBox( ERROR_NEXONAUTH_SHUTDOWNED_4, MB_OK, MESSAGEBOX_10, this, true, false, false, false );
		return;
	}
#endif

	CTaskManager* pTaskManager = CTaskManager::GetInstancePtr();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TCUTSCENE );
	if( pSox->IsExistItem( nCutSceneTableID ) == false ) return;
	std::string szFileName = pSox->GetFieldFromLablePtr( nCutSceneTableID, "_FileName" )->GetString();

	bool bFind = false;
	CFileNameString szFullPathName = CEtResourceMng::GetInstance().GetFullName( szFileName, &bFind );
	if( !bFind ) return;

	CDnCutSceneTask* pNewCutSceneTask = new CDnCutSceneTask;
	pNewCutSceneTask->Initialize( szFullPathName.c_str(), nQuestIndex, nQuestStep, ( m_bControlQuestScript ) ? m_bQuestCutSceneAutoFadeIn : true, dwQuestPlayerUniqueID );

	if( m_bCheatCutScene )
		pNewCutSceneTask->FromCheat();

	CDnVillageTask* pVillageTask = (CDnVillageTask*)(CTaskManager::GetInstance().GetTask("VillageTask"));
	if ( pVillageTask )
		pVillageTask->EnableTaskProcess(false);

	CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if ( pGameTask )
		pGameTask->EnableTaskProcess(false);

	pNewCutSceneTask->EnableTaskProcess( true );
	pNewCutSceneTask->SetTaskName( "CutSceneTask" );
	pTaskManager->AddTask( pNewCutSceneTask, "CutSceneTask", -1, false );

	// 플레이 시작!
	pNewCutSceneTask->StartPlay();
	m_pCutSceneTask = pNewCutSceneTask;

	if( !IsPlayedCutScene( nCutSceneTableID ) )
		m_nVecPlayedCutSceneList.push_back( nCutSceneTableID );

}

void CDnCommonTask::EndCutScene()
{
	if( !m_pCutSceneTask ) return;
	m_pCutSceneTask->Finalize();
//	CTaskManager::GetInstance().RemoveTask("CutSceneTask");
	m_pCutSceneTask->DestroyTask( true );
	m_pCutSceneTask = NULL;

	// 마을에서 재생중일 때는 끝났다고 패킷 안보내도 됨
	if( !m_bControlQuestScript )
//	if( -1 == m_nQuestCutSceneID )
		SendCompleteCutScene( CDnBridgeTask::GetInstance().GetSessionID() );
	else
	{
		m_bControlQuestScript = false;
		m_nQuestCutSceneID = -1;
		
		// VillageTask 복구
		CDnVillageTask* pVillageTask = (CDnVillageTask*)(CTaskManager::GetInstance().GetTask("VillageTask"));
		CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
		if( pVillageTask || pGameTask ) {
			if( pVillageTask ) pVillageTask->EnableTaskProcess(true);
			if( pGameTask ) pGameTask->EnableTaskProcess(true);

			DnActorHandle nNpc = CDnActor::FindActorFromUniqueID( m_nQuestCutSceneNpcID );
			if( nNpc ) {
				// npc 타입의 액터인지 한 번 더 검사. 치트로 컷신 실행했을 경우 m_nQuestCutSceneNpcID 가 0이 되어 뻑나는 경우가 있음.
				if( nNpc->GetActorType() == CDnActorState::Npc ) {
					CDnNPCActor* pNPC = dynamic_cast<CDnNPCActor*>(nNpc.GetPointer());
					CDnNPCActor::NpcData& data = pNPC->GetNpcData();

					// 사용자 정의에 의해 NPCTALK를 할때 NpcTalkStringGroup에 해당 스트링을 추가하세요!
					if( !IsRequestNpcTalk() )
					{
						SendNpcTalk(m_nQuestCutSceneNpcID, NpcTalkStringGroup[NPCTALK_COMPLETECUTSCENE], data.wszTalkFileName);
						SetNpcTalkRequestWait();
					}
				}
			}
		}

		CDnPlayerActor::HideAnotherPlayers( false, 1.f );
		if( m_bQuestCutSceneAutoFadeIn )
			GetInterface().FadeDialog( 0xff000000, 0x00000000, 1.0f, this );
		m_bQuestCutSceneAutoFadeIn = true;
	}
	m_bSkipCutScene = false;

	m_bCheatCutScene = false;
}

void CDnCommonTask::ExecuteInitTimeProcess( InitTimeProcessStruct *pStruct )
{
	switch( pStruct->Type ) {
		case ITP_PlayCutScene:
			{
				int nCutSceneIndex = atoi( pStruct->szVecParam[0].c_str() );

				int nQuestIndex = -1;
				int nQuestStep = -1;
				DWORD dwQuestPlayerUniqueID = UINT_MAX;
				if( 1 < (int)pStruct->szVecParam.size() )
				{
					nQuestIndex = atoi( pStruct->szVecParam[1].c_str() );
					nQuestStep = atoi( pStruct->szVecParam[2].c_str() );
					dwQuestPlayerUniqueID = (DWORD)atoi( pStruct->szVecParam[3].c_str() );
				}

				PlayCutScene( nCutSceneIndex, nQuestIndex, nQuestStep, dwQuestPlayerUniqueID );
			}
			break;
	}
}

bool __stdcall CDnCommonTask::OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize )
{
	if( pThis != dynamic_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask( "CommonTask" )) ) return false;

	switch( nMainCmd ) {
		case SC_CHAR:
			switch( nSubCmd ) {
				case eChar::SC_ENTERNPC:
					if( ((SCEnterNpc*)pParam)->nNpcObjectID == nUniqueID ) return true;
					break;
				case eChar::SC_LEAVENPC:
					if( ((SCLeaveNpc*)pParam)->nObjectID == nUniqueID ) return true;
					break;
			}
		case SC_NPC:
			switch( nSubCmd ) {
				case eNpc::SC_NPCTALK:
					if(((SCNpcTalk*)pParam)->nNpcUniqueID == nUniqueID ) return true;
					break;
			}
			break;
	}

	return false;
}


bool CDnCommonTask::IsPlayedCutScene( int nCutSceneTableID )
{
	if( std::find( m_nVecPlayedCutSceneList.begin(), m_nVecPlayedCutSceneList.end(), nCutSceneTableID ) == m_nVecPlayedCutSceneList.end() ) return false;

	return true;
}

void CDnCommonTask::ResetPlayedCutScene()
{
	SAFE_DELETE_VEC( m_nVecPlayedCutSceneList );
}

void CDnCommonTask::BeginNpcTalk( DnActorHandle hNpc )
{
	// 카메라 변경
	if( !hNpc ) return;

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	DnCameraHandle hCurrentCam = CDnCamera::GetActiveCamera();
	if(hCurrentCam)
	{
		if( hCurrentCam->GetCameraType() != CDnCamera::CameraTypeEnum::NpcTalkCamera )
		{
			m_hTalkNpcCamera = (new CDnNpcTalkCamera( hCurrentCam, hNpc ))->GetMySmartPtr();
			CDnCamera::SetActiveCamera( m_hTalkNpcCamera );
		}
	}
	else
	{
		m_hTalkNpcCamera = (new CDnNpcTalkCamera( CDnCamera::GetActiveCamera(), hNpc ))->GetMySmartPtr();
		CDnCamera::SetActiveCamera( m_hTalkNpcCamera );
	}
#else
	m_hTalkNpcCamera = (new CDnNpcTalkCamera( CDnCamera::GetActiveCamera(), hNpc ))->GetMySmartPtr();
	CDnCamera::SetActiveCamera( m_hTalkNpcCamera );
#endif // PRE_ADD_QUEST_BACK_DIALOG_BUTTON

	EtVector3 vVec = *hNpc->GetPosition() - *CDnActor::s_hLocalActor->GetPosition();
	vVec.y = 0.f;
	EtVec3Normalize( &vVec, &vVec );
	CDnActor::s_hLocalActor->CmdLook( EtVec3toVec2( vVec ), false );

	CDnNPCActor* pActor = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
	if ( pActor )
		pActor->OnSoundPlay( CDnNPCActor::BeginTalk );
}

void CDnCommonTask::EndNpcTalk( bool bPlaySound )
{
#ifdef PRE_ADD_NPCTALK_FORCE_ACTION_ONEND
	UINT nNpcUID = CDnLocalPlayerActor::GetTakeNpcUID();
	DnActorHandle hTakeNpc = CDnActor::FindActorFromUniqueID(nNpcUID);
	if (hTakeNpc)
	{
		// Academic NPC 만 일단 예외처리가 필요하다고 해서 적용. 이후 액터가 많아지고 액션도 다양화되면 테이블화 필요.
		int actorTableID = hTakeNpc->GetActorTableID();
		if (actorTableID == 1570 || actorTableID == 70025 )
		{
			std::string& currentAction = hTakeNpc->GetCurrentPlayAction();
			if (currentAction.compare("Talk_idle") == 0)
			{
				hTakeNpc->SetActionQueue("Talk_end");
			}
			else if (currentAction.compare("Talk") == 0)
			{
				CDnNPCActor* pActor = static_cast<CDnNPCActor*>(hTakeNpc.GetPointer());
				if (pActor)
					pActor->SetForceActionOnFinishCurrentAction("Talk_end");
			}
		}
	}
#endif

#if !defined(PRE_FIX_QUEST_GUILD_BOARD)
	if( CTaskManager::GetInstance().GetTask( "GameTask" ) ) {
#endif

#ifdef PRE_ADD_NPCTALK_FORCE_ACTION_ONEND
#else
		UINT nNpcUID = CDnLocalPlayerActor::GetTakeNpcUID();
#endif
		SendNpcTalkEnd( nNpcUID );
#if !defined(PRE_FIX_QUEST_GUILD_BOARD)
	}
#endif

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	GetInterface().ClearNpcTalkTextureWindow();// 대화가 종료되면 텍스쳐를 강제로 닫는다.
#endif // PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG

	bool bEndTalk = false;

	if( m_hTalkNpcCamera )
	{
		if( ((CDnNpcTalkCamera*)m_hTalkNpcCamera.GetPointer())->GetWorkState() != CDnNpcTalkCamera::EndTalk ) 
		{
#ifdef PRE_ADD_MAINQUEST_UI
			// MainQuestDlg는 카메라를 변경하지 않는다.
			CDnMainQuestDlg* pMainQuestDlg = GetInterface().GetMainQuestDlg();	
			if(pMainQuestDlg && pMainQuestDlg->IsOpenMainQuestDlg() == false)
#endif // PRE_ADD_MAINQUEST_UI
			{
				((CDnNpcTalkCamera*)m_hTalkNpcCamera.GetPointer())->SetWorkState( CDnNpcTalkCamera::EndTalk );
				bEndTalk = true;
			}
		}
	}

	if( bPlaySound )
	{
		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);

		if ( hNpc ) {
			CDnNPCActor* pActor = dynamic_cast<CDnNPCActor*>(hNpc.GetPointer());
			if ( pActor ) {
				pActor->OnSoundPlay( bEndTalk ? CDnNPCActor::FinishTalk : CDnNPCActor::CloseShop );
			}
		}
	}

	GetInterface().ShowDungeonOpenNoticeDialog( true );
}

#if defined (_KRAZ) || defined (_WORK)
void CDnCommonTask::ProcessPlayTimeNotice()
{
	if( !CTaskManager::GetInstance().GetTask( "GameTask" ) && !CTaskManager::GetInstance().GetTask( "VillageTask" ) ) return;
	DWORD dwPlayTime = ( timeGetTime() - m_dwSelectCharTime ) / 1000;

	if( dwPlayTime / 3600 > (DWORD)m_nPlayTimeNoticeCount ) {
		m_nPlayTimeNoticeCount++;
		WCHAR wszTemp[512] = { 0, };
		wsprintf( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101004 ), m_nPlayTimeNoticeCount );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );

		GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption6, wszTemp, textcolor::WHITE, 5.0f ); // #59492 한국 선택적 셧다운제 - "게임 이용시간이 "%d" 시간 경과 했습니다."

		if( m_nPlayTimeNoticeCount % 3 == 0 ) {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101005 ), false );
		}
		if( m_nPlayTimeNoticeCount % 2 == 0 ) {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101007 ), false );
		}
		else {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101006 ), false );
		}
	}
}

void CDnCommonTask::ResetPlayTimeNotice()
{
	m_dwSelectCharTime = timeGetTime();
	m_nPlayTimeNoticeCount = 0;

	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101001 ), false );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101002 ), false );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 101003 ), false );
}
#endif // defined (_KRAZ) || defined (_WORK)


bool CDnCommonTask::IsPCBangClearBox()
{
	bool bClearBox = false;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPCCAFE );
	int nNumItem = pSox->GetItemCount();
	int nItemID = 0;
	for( int nItem = 0; nItem < nNumItem; ++nItem )
	{
		nItemID = pSox->GetItemID( nItem );
		int nType = pSox->GetFieldFromLablePtr( nItemID, "_Type" )->GetInteger();

		switch (nType)
		{
		case PCBang::Type::ClearBox:
			{
				switch (m_cPCBangGrade)
				{
				case PCBang::Grade::Normal:
					bClearBox = pSox->GetFieldFromLablePtr( nItemID, "_PCParam1" )->GetInteger() ? true : false;
					break;

				case PCBang::Grade::Premium:
					bClearBox = pSox->GetFieldFromLablePtr( nItemID, "_PremiumPCParam1" )->GetInteger() ? true : false;
					break;

				case PCBang::Grade::Gold:
					bClearBox = pSox->GetFieldFromLablePtr( nItemID, "_GoldPCParam1" )->GetInteger() ? true : false;
					break;

				case PCBang::Grade::Silver:
					bClearBox = pSox->GetFieldFromLablePtr( nItemID, "_SilverPCParam1" )->GetInteger() ? true : false;
					break;

				case PCBang::Grade::Red:
					bClearBox = pSox->GetFieldFromLablePtr( nItemID, "_RedPCParam1" )->GetInteger() ? true : false;
					break;
				}
			}
			break;
		}
	}

	return bClearBox;
}

bool CDnCommonTask::CheckAndInsertDungeonOpenNotice( int nMapIndex, DungeonOpenNoticeTypeEnum EnterMapType )
{
	DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TMAP );

	for( DWORD j=0; j<m_VecDungeonOpenNoticeList.size(); j++ ) {
		if( m_VecDungeonOpenNoticeList[j].nMapID == nMapIndex ) {
			return false;
		}
	}

	CDnWorld::MapTypeEnum CheckMapType;
	switch( EnterMapType ) {
		case OpenWorldMap: CheckMapType = CDnWorld::MapTypeWorldMap; break;
		case OpenDungeon: 
		case OpenAbyss: CheckMapType = CDnWorld::MapTypeDungeon; break;
	}
	if( pMapSox->GetFieldFromLablePtr( nMapIndex, "_MapType" )->GetInteger() != CheckMapType ) return false;
	if( pMapSox->GetFieldFromLablePtr( nMapIndex, "_IsBattleField" )->GetInteger() == 1 ) return false;
	if( pMapSox->GetFieldFromLablePtr( nMapIndex, "_IncludeBuild" )->GetInteger() != 1 ) return false;

	// 같은 이름이 있을경우도 체크해서 제외 ( 다크레어의 경우 맵 3개모두 입장 condition 이 있기때문에 체크되어버린다. )
	int nMapNameID = pMapSox->GetFieldFromLablePtr( nMapIndex, "_MapNameID" )->GetInteger();
	for( DWORD j=0; j<m_VecDungeonOpenNoticeList.size(); j++ ) {
		if( m_VecDungeonOpenNoticeList[j].nMapNameID == nMapNameID && m_VecDungeonOpenNoticeList[j].Type == EnterMapType ) {
			return false;
		}
	}
	if( nMapNameID <= 0 ) return false;

	DungeonOpenNoticeStruct Struct;
	Struct.Type = EnterMapType;
	Struct.nMapID = nMapIndex;
	Struct.nMapNameID = nMapNameID;
	m_VecDungeonOpenNoticeList.push_back( Struct );

	return true;
}

void CDnCommonTask::CheckDungeonOpenNotice( int nLevel, int nQuestID )
{
	DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TMAP );
	DNTableFileFormat* pDungeonEnterSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
	DNTableFileFormat* pGateSox = GetDNTable( CDnTableDB::TMAPGATE );

	if( nLevel != -1 && nQuestID == -1 ) {
		// 일단 월드존 체크
		std::vector<int> nVecList;
		pGateSox->GetItemIDListFromField( "_PermitPlayerLevel", nLevel, nVecList );
		if( !nVecList.empty() ) {
			for( DWORD i=0; i<nVecList.size(); i++ ) {
				int nMapIndex = pGateSox->GetFieldFromLablePtr( nVecList[i], "_MapIndex" )->GetInteger();
				int nGateNo = pGateSox->GetFieldFromLablePtr( nVecList[i], "_GateIndex" )->GetInteger();
				CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nMapIndex );
				if( !pWorldData ) continue;
				CDnWorldData::GateData *pGate = pWorldData->GetGateDataFromGateNo( nGateNo );
				if( !pGate ) continue;
				CheckAndInsertDungeonOpenNotice( pGate->nMapIndex, OpenWorldMap );
			}
		}

		// 던전 체크
		nVecList.clear();
		pDungeonEnterSox->GetItemIDListFromField( "_LvlMin", nLevel, nVecList );
		if( !nVecList.empty() ) {
			for( DWORD i=0; i<nVecList.size(); i++ ) {
				std::vector<int> nVecMapList;
				pMapSox->GetItemIDListFromField( "_EnterConditionTableID", nVecList[i], nVecMapList );
				if( !nVecMapList.empty() ) {
					for( DWORD j=0; j<nVecMapList.size(); j++ ) {
						CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nVecMapList[j] );
						if( !pWorldData ) continue;
						CheckAndInsertDungeonOpenNotice( pWorldData->GetMapIndex(), OpenDungeon );
					}
				}
			}
		}

		// 어비스 던전 체크
		nVecList.clear();
		pDungeonEnterSox->GetItemIDListFromField( "_AbyssLvlMin", nLevel, nVecList );
		if( !nVecList.empty() ) {
			for( DWORD i=0; i<nVecList.size(); i++ ) {
				int nAbyssQuestID = pDungeonEnterSox->GetFieldFromLablePtr( nVecList[i], "_AbyssQuestID" )->GetInteger();
				if( !CDnQuestTask::GetInstance().IsClearQuest( nAbyssQuestID ) ) continue;
				std::vector<int> nVecMapList;
				pMapSox->GetItemIDListFromField( "_EnterConditionTableID", nVecList[i], nVecMapList );
				if( !nVecMapList.empty() ) {
					for( DWORD j=0; j<nVecMapList.size(); j++ ) {
						CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nVecMapList[j] );
						if( !pWorldData ) continue;
						CheckAndInsertDungeonOpenNotice( pWorldData->GetMapIndex(), OpenDungeon );
					}
				}
			}
		}
	}
	else if( nLevel != -1 && nQuestID != -1 ) {
		// 어비스 던전만 체크하면 된다. 
		std::vector<int> nVecList;
		pDungeonEnterSox->GetItemIDListFromField( "_AbyssQuestID", nQuestID, nVecList );
		if( !nVecList.empty() ) {
			for( DWORD i=0; i<nVecList.size(); i++ ) {
				int nLevelMin = pDungeonEnterSox->GetFieldFromLablePtr( nVecList[i], "_AbyssLvlMin" )->GetInteger();
				int nLevelMax = pDungeonEnterSox->GetFieldFromLablePtr( nVecList[i], "_AbyssLvlMax" )->GetInteger();
				if( nLevel < nLevelMin || nLevel > nLevelMax ) continue;

				std::vector<int> nVecMapList;
				pMapSox->GetItemIDListFromField( "_EnterConditionTableID", nVecList[i], nVecMapList );
				if( !nVecMapList.empty() ) {
					for( DWORD j=0; j<nVecMapList.size(); j++ ) 
					{
						CDnWorldData *pWorldData = CDnWorldDataManager::GetInstance().GetWorldData( nVecMapList[j] );
						if( !pWorldData ) continue;
						
						CheckAndInsertDungeonOpenNotice( pWorldData->GetMapIndex(), OpenAbyss );
					}
				}
			}
		}
	}
}

void CDnCommonTask::ClearDungeonOpenNotice()
{
	SAFE_DELETE_VEC( m_VecDungeonOpenNoticeList );
}

bool CDnCommonTask::OnCloseTask()
{
	if( !CDnInterface::IsActive() ) return false;
	if( !GetInterface().GetMainMenuDialog() ) return false;
	if( !GetInterface().GetMainMenuDialog()->GetSystemDlg() ) return false;
	if( !CTaskManager::GetInstance().GetTask( "GameTask" ) &&
		!CTaskManager::GetInstance().GetTask( "VillageTask" ) &&
		!CTaskManager::GetInstance().GetTask( "PVPLobbyTask" ) ) return false;

#ifdef PRE_ADD_GAMEQUIT_REWARD
	GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( false );
#else
	GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton();
#endif

	return true;
}

void CDnCommonTask::OnRecvEtcMessage( int nSubCmd, char *pData, int nSize )
{
	switch( nSubCmd )
	{
		case eEtc::eSCEtc::SC_DARKLAIR_RANK_BOARD: OnRecvOpenDarkLairRankBoard( (SCDarkLairRankBoard *)pData ); break;
		case eEtc::eSCEtc::SC_PVPLADDER_RANK_BOARD: OnRecvOpenPVPLadderRankBoard((SCPvPLadderRankBoard*)pData ); break;
#ifdef PRE_ADD_PVP_RANKING
		case eEtc::eSCEtc::SC_PVP_RANK_BOARD: OnRecvPvPRankBoardColosseum( pData ); break;			
		case eEtc::eSCEtc::SC_PVP_RANK_LIST: OnRecvPvPRankList( pData ); break;
		case eEtc::eSCEtc::SC_PVP_RANK_INFO: OnRecvPvPRankInfo( pData ); break;

		case eEtc::eSCEtc::SC_PVP_LADDER_RANK_BOARD: OnRecvPvPRankBoardLadder( pData ); break;
		case eEtc::eSCEtc::SC_PVP_LADDER_RANK_LIST: OnRecvPvPRankLadderList( pData ); break;
		case eEtc::eSCEtc::SC_PVP_LADDER_RANK_INFO: OnRecvPvPRankLadderInfo( pData ); break;
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_GAMEQUIT_REWARD
		case eEtc::eSCEtc::SC_GAMEQUIT_REWARD_RES: OnRecvGiftReceivingMessage( pData ); break;
		case eEtc::eSCEtc::SC_GAMEQUIT_REWARDCHECK_RES:	OnRecvLogOutMessage( pData ); break;
#endif // PRE_ADD_GAMEQUIT_REWARD
#ifdef PRE_DRAGONBUFF
		case eEtc::eSCEtc::SC_WORLDBUFFMSG: OnRecvWorldBuffMsg( (SCWorldBuffMsg*)pData ); break;
#endif

	}
}

#ifdef PRE_ADD_QUICK_PVP
void CDnCommonTask::OnRecvPvPMessage(int nSubCmd, char* pData, int nSize)
{
	switch(nSubCmd)
	{
	case ePvP::eSCPvP::SC_QUICKPVP_INVITE: OnRecvQuickPvPInvite((SCQuickPvPInvite*)pData); break;
	case ePvP::eSCPvP::SC_QUICKPVP_RESULT: OnRecvQuickPvPResult((SCQuickPvPResult*)pData); break;
	}
}
#endif

void CDnCommonTask::OnRecvDarkLairRankBoard( SCOpenDarkLairRankBoard* pPacket )
{
	_ASSERT(pPacket);
	
	CSDarkLairRankBoard TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.iMapIndex			= pPacket->iMapIndex;;
	TxPacket.cPartyUserCount	= pPacket->cPlayerCount;

	GetInterface().RequestDarkLairRankInfo();
	CClientSessionManager::GetInstance().SendPacket( CS_ETC, eEtc::eCSEtc::CS_DARKLAIR_RANK_BOARD, (char*)&TxPacket, int(sizeof(TxPacket)) );
}

void CDnCommonTask::OnRecvOpenDarkLairRankBoard( SCDarkLairRankBoard* pPacket )
{
	_ASSERT(pPacket);

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
	if( pPacket->iRet == ERROR_NONE )
	{
#endif
		TDLRankHistoryPartyInfo BsetScore;
		TDLRankHistoryPartyInfo HistoryScore[DarkLair::Rank::SelectRankBoardTop];

		memcpy( &BsetScore, &pPacket->sBestHistory, sizeof(TDLRankHistoryPartyInfo) );
		memcpy( HistoryScore, pPacket->sHistoryTop, sizeof(TDLRankHistoryPartyInfo) * DarkLair::Rank::SelectRankBoardTop );

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
		GetInterface().SetDarkRareRankBoardMapIndex( pPacket->iMapIndex );
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)

		GetInterface().OpenDarkRareRankBoardDialog( &BsetScore, HistoryScore );

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)	
	}
	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
		GetInterface().CloseBlind();
	}
#endif

}

void CDnCommonTask::OnRecvPVPLadderRankBoard( SCOpenPvPLadderRankBoard* pPacket )
{
 	_ASSERT(pPacket);

	CSPvPLadderRankBoard TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );
	TxPacket.MatchType = pPacket->MatchType;

	GetInterface().GetPVPLadderRankBoardDlg()->SetPVPLadderRankMatchType(pPacket->MatchType);
	GetInterface().RequestPVPLadderRankInfo();	
	CClientSessionManager::GetInstance().SendPacket( CS_ETC, eEtc::eCSEtc::CS_PVPLADDER_RANK_BOARD, (char*)&TxPacket, int(sizeof(TxPacket)) );
}

void CDnCommonTask::OnRecvOpenPVPLadderRankBoard( SCPvPLadderRankBoard* pPacket )
{
	_ASSERT(pPacket);

	GetInterface().OpenPVPLadderRankBoardDialog(pPacket);
}

#ifdef PRE_ADD_QUICK_PVP
void CDnCommonTask::OnRecvQuickPvPInvite(SCQuickPvPInvite* pData)
{
	_ASSERT(pData);

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID(pData->nSenderSessionID);
	if (!hActor)
		return;

#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	if (CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking())
	{
		SendQuickPvPResult(false, pData->nSenderSessionID);
		return;
	}
#endif

	CDnQuickPvPAcceptRequestInfo info;
	info.m_SenderSessionID = pData->nSenderSessionID;
	info.m_szOtherSideName = hActor->GetName();

	m_AcceptRequestQuickPvPInvite.AddAcceptRequestInfo(info);
}

void CDnCommonTask::OnRecvQuickPvPResult(SCQuickPvPResult* pData)
{
	_ASSERT(pData);

	int nStringCode = 0;

	switch(pData->nResult)
	{
	case ERROR_PVP_QUICK_USERNOTFOUND : nStringCode = 1131; break;
	case ERROR_PVP_QUICK_FAIL : nStringCode = 1132; break;
	case ERROR_PVP_QUICK_CANTINVITE : nStringCode = 1133; break;
	case ERROR_PVP_QUICK_DENY : nStringCode = 1134; break;
	case ERROR_PVP_QUICK_MY_LEVEL : nStringCode = 1135; break;
	case ERROR_PVP_QUICK_OTHER_LEVEL : nStringCode = 1136; break;
	}

	if (nStringCode > 0)	
	{
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
		if (pData->nResult == ERROR_PVP_QUICK_FAIL)
		{
			std::wstring serverMsg;
			CDnInterface::GetInstance().GetServerMessage(serverMsg, nStringCode);
			CDnInterface::GetInstance().AddChatMessage( CHATTYPE_SYSTEM, L"", serverMsg.c_str(), false );
		}
		else
		{
			CDnInterface::GetInstance().ServerMessageBox(nStringCode);
		}
#else
		CDnInterface::GetInstance().ServerMessageBox(nStringCode);
#endif
	}

#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	SetRequestingQuickPvP(false);
	CDnInterface::GetInstance().DisableCashShopMenuDlg(false);
#endif
}
#endif

void CDnCommonTask::OnRecvCharCommonVariable( SCModCommonVariable* pPacket )
{
	_ASSERT( pPacket );
	
	std::map<CommonVariable::Type::eCode, CommonVariable::Data>::iterator itor = m_mVariableData.find( pPacket->Data.Type );
	if(itor != m_mVariableData.end()) 
		m_mVariableData.erase(itor);

	m_mVariableData.insert( std::make_pair( pPacket->Data.Type, pPacket->Data ) );

	if( pPacket->Data.Type == CommonVariable::Type::AttendanceEvent )
	{
		CDnMainDlg* pMainDlg = GetInterface().GetMainBarDialog();
		if( pMainDlg )
			pMainDlg->ShowCheckButton( true );
	}

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	if( pPacket->Data.Type == CommonVariable::Type::NotifyCompound )
	{	
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg )
		{
			int nItemID = static_cast<int>( pPacket->Data.biValue & 0xffffffff );
			int nItemOption = static_cast<int>( pPacket->Data.biValue >> 32 );

			if( nItemID > 0 && nItemOption >= 0 ) // 옵션은 없는애들도 있습니다.
			{
				pMainMenuDlg->SetItemCompoundNotify( nItemID , nItemOption );
			}
			else
			{
				pMainMenuDlg->ResetItemCompoundNotify();
			}
			
		}
	}
#endif

}

bool CDnCommonTask::HasVariableData(CommonVariable::Type::eCode type) const
{
	std::map<CommonVariable::Type::eCode, CommonVariable::Data>::const_iterator iter = m_mVariableData.find(type);
	return (iter != m_mVariableData.end());
}

void CDnCommonTask::OnRecvCheckAttedanceResult( SCAttendanceEvent *pPacket )
{
	_ASSERT( pPacket );

	m_bCheckAttendanceFirst = pPacket->bCheckAttendanceFirst;
}

void CDnCommonTask::OnRecvShowGlyphLift( char* pData )
{
	GetInterface().OpenGlyphLiftDialog();
}

void CDnCommonTask::OnRecvShowInventory( char* pData )
{
	GetInterface().OpenInvenDialog();
}

const char* CDnCommonTask::GetMiniSiteURL() const
{
	if (m_pMiniSiteChecker)
		return m_pMiniSiteChecker->GetMiniSiteURL().c_str();

	return NULL;
}

void CDnCommonTask::GetMiniSiteTooltip(std::wstring& tooltip)
{
	if (m_pMiniSiteChecker)
		tooltip = m_pMiniSiteChecker->GetMiniSiteTooltip();
}

#ifdef PRE_ADD_QUICK_PVP
float CDnCommonTask::GetQuickPvPShortestAcceptTime() const
{
	return m_AcceptRequestQuickPvPInvite.GetShortestAcceptTime();
}

void CDnCommonTask::OpenQuickPvPAcceptRequestDialog()
{
	if (GetInterface().IsOpenBlind())
	{
#ifdef PRE_MOD_INTEG_SYSTEM_STATE
		m_AcceptRequestQuickPvPInvite.TerminateList();
		m_AcceptRequestQuickPvPInvite.ClearList();
#else
		m_AcceptRequestQuickPvPInvite.ClearInfoList();
#endif
		GetInterface().ClearAcceptReqDlg();
		return;
	}

	if (m_AcceptRequestQuickPvPInvite.IsListEmpty() == false)
	{
		const CDnQuickPvPAcceptRequestInfo& info = m_AcceptRequestQuickPvPInvite.GetCurrentInfo();

		std::wstring str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120151), info.m_szOtherSideName.c_str());	// UISTRING : [%s] 님이 결투를 신청하였습니다. 신청에 응하시겠습니까?
		GetInterface().OpenAcceptRequestDialog(str.c_str(), _REQUEST_ACCEPT_TOTALTIME, info.m_fAcceptTime, true, CDnAcceptRequestDlg::eType02, ACCEPT_REQUEST_DIALOG, this);

		m_CurrentAcceptRequestCache = info;
	}
}
#endif



#ifdef PRE_ADD_PVP_RANKING
/*struct TPvPRankingDetail : public TPvPRanking
{
	INT64	biClassRank;
	INT64	biChangedClassRank;
	INT64	biSubClassRank;	
	INT64	biChangedSubClassRank;
};*/
// 내정보 - 콜로세움.
void CDnCommonTask::OnRecvPvPRankBoardColosseum( char * pData )
{
	SCPvPRankBoard * pPacket = (SCPvPRankBoard *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetInfoMyRankColosseum( &pPacket->MyRanking );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

// 내정보 - 레더.
void CDnCommonTask::OnRecvPvPRankBoardLadder( char * pData )
{
	SCPvPLadderRankBoard2 * pPacket = (SCPvPLadderRankBoard2 *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetInfoMyRankLadder( &pPacket->MyRanking );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

// 캐릭터명 검색정보 - 콜로세움.
void CDnCommonTask::OnRecvPvPRankInfo( char * pData )
{
	SCPvPRankInfo * pPacket = (SCPvPRankInfo *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetInfoColosseum( &pPacket->RankingInfo );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

// 캐릭터명 검색정보 - 레더.
void CDnCommonTask::OnRecvPvPRankLadderInfo( char * pData )
{
	SCPvPLadderRankInfo * pPacket = (SCPvPLadderRankInfo *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetInfoLadder( &pPacket->RankingInfo );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

// Rank List - 콜로세움.
void CDnCommonTask::OnRecvPvPRankList( char * pData )
{
	SCPvPRankList * pPacket = (SCPvPRankList *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetListColosseum( pPacket );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

// Rank List - 레더.
void CDnCommonTask::OnRecvPvPRankLadderList( char * pData )
{
	SCPvPLadderRankList * pPacket = (SCPvPLadderRankList *)pData;

	if( pPacket->iRet == ERROR_NONE )
	{
		GetInterface().SetListLadder( pPacket );
	}

	else
	{
		GetInterface().ServerMessageBox(pPacket->iRet);
	}

}

#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_WEEKLYEVENT
void CDnCommonTask::ProcessTimeEvent( LOCAL_TIME LocalTime, float fDelta )
{
	const int WEEKLY_EVENT_REFRESH_TICK = 5000;

	if( ( LocalTime - m_tWeeklyEventRefreshTick ) > WEEKLY_EVENT_REFRESH_TICK )
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TWEEKLYEVENT );

		if( !pSox )
		{
			return;
		}
		const __time64_t tCurTime = CSyncTimer::GetInstance().GetCurTime(); 
		DBTIMESTAMP DbTime;  
		CTimeSet::ConvertTimeT64ToDbTimeStamp_LC( tCurTime, &DbTime );
		CTimeSet cTime;
		cTime.Set(DbTime);

		const int nTimeSet_Week = cTime.GetDayOfWeek();
		const int nTiemSet_Hour = cTime.GetHour();

		bool bNotice = false;
		bool bExist = false;

		for( int i=0; i<pSox->GetItemCount(); ++i ) // START VALIDATE
		{
			int nTableID = pSox->GetItemID( i );
			int nDayOfWeek = pSox->GetFieldFromLablePtr( nTableID , "_DayOfWeek" )->GetInteger();
			if( nDayOfWeek == nTimeSet_Week )
			{
				int nStartTime = pSox->GetFieldFromLablePtr( nTableID , "_StartTime" )->GetInteger();
				int nEndTime = pSox->GetFieldFromLablePtr( nTableID , "_EndTime" )->GetInteger();

				if( nStartTime <= nTiemSet_Hour && nTiemSet_Hour < nEndTime )
				{
					if( m_nCurrentEventWeek != nDayOfWeek )
					{
						m_nCurrentEventWeek = nDayOfWeek;
						bNotice = true;
					}

					bExist = true;
					break; // BREAK;
				}
			}
		}

		if( bExist ) // FOUND RESULT
		{
			GetInterface().RefreshWeeklyEventMark( true , bNotice );
		}
		else
		{
			GetInterface().RefreshWeeklyEventMark( false , false );
		}

		m_tWeeklyEventRefreshTick = LocalTime;
	}
}
#endif


#ifdef PRE_ADD_GAMEQUIT_REWARD

// 접속종료시 전송.
void CDnCommonTask::SendLogOutMessage()
{
	FUNC_LOG();

	CClientSessionManager::GetInstance().SendPacket( CS_ETC, eEtc::eCSEtc::CS_GAMEQUIT_REWARDCHECK_REQ, NULL, 0 );

}

// 접속종료 수신.
void CDnCommonTask::OnRecvLogOutMessage( char * pData )
{
	SCGameQuitRewardCheckRes * pPacket = (SCGameQuitRewardCheckRes *)pData;
	
	bool bShow = true;
	if( pPacket->eRewardType == GameQuitReward::RewardType::eType::None )
		bShow = false;

#if defined(PRE_ADD_DWC)
	if(CDnDWCTask::IsActive())
	{
		if(GetDWCTask().IsDWCChar() == false)
		{
			if( GetInterface().ShowGameQuitRewardDlg( bShow, pPacket->eRewardType ) == true )
				bShow = true;
		}
		else
			bShow = false;
	}

	if( bShow == false )
	{
		//GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
		CEtUIDialog * pDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
		if( pDlg )
		{
			CDnSystemDlg * pSystemDlg = (CDnSystemDlg *)pDlg;
			int strId = pSystemDlg->IsGameQuitCharSelectStrIdx();
			if( strId == 0 )
				GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
			else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strId ), MB_YESNO, CDnSystemDlg::MESSAGEBOX_CHAR, pSystemDlg );
		}
	}

#else // #else PRE_ADD_DWC

	if( GetInterface().ShowGameQuitRewardDlg( bShow, pPacket->eRewardType ) == true )
		bShow = true;

	if( bShow == false )
	{
		//GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
		CEtUIDialog * pDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
		if( pDlg )
		{
			CDnSystemDlg * pSystemDlg = (CDnSystemDlg *)pDlg;
			int strId = pSystemDlg->IsGameQuitCharSelectStrIdx();
			if( strId == 0 )
				GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
			else
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strId ), MB_YESNO, CDnSystemDlg::MESSAGEBOX_CHAR, pSystemDlg );
		}
	}
#endif // #endif PRE_ADD_DWC
}

// 선물받기요청.
void CDnCommonTask::SendGiftReceiving( int type )
{
	CSGameQuitRewardReq packet;
	packet.eRewardType = (GameQuitReward::RewardType::eType)type;
	CClientSessionManager::GetInstance().SendPacket( CS_ETC, eEtc::eCSEtc::CS_GAMEQUIT_REWARD_REQ, (char*)&packet, sizeof(CSGameQuitRewardReq) );
}

// 선물받기 수신.
void CDnCommonTask::OnRecvGiftReceivingMessage( char * pData )
{
	SCGameQuitRewardRes * pPacket = (SCGameQuitRewardRes *)pData;
	if( pPacket->nRet == ERROR_NONE )
	{	
		GetInterface().MessageBox( 7957, MB_OK, 7957, this );		

		CTask * pTask = CTaskManager::GetInstance().GetTask( "BridgeTask" );
		if( pTask )
		{
			CDnBridgeTask * pBridgeTask = (CDnBridgeTask *)pTask;
			pBridgeTask->TakeNewbieReward();
		}
		
		GetInterface().RecvReward();
	}
	else
	{
		GetInterface().ServerMessageBox( pPacket->nRet );
	}
}


#endif // PRE_ADD_GAMEQUIT_REWARD


#ifdef PRE_DRAGONBUFF
void CDnCommonTask::OnRecvWorldBuffMsg( SCWorldBuffMsg *pData )
{
	if( !CDnActor::s_hLocalActor ) return;

	if( pData )
	{
		GetInterface().ShowCaptionDialog( CDnInterface::typeCaption3, FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9168),  pData->wszCharacterName ).c_str() , textcolor::HOTPINK, 10.0f );
		
		std::string strTargetEffectName;
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TNAMEDITEM );
		if( pSox )
		{
			for( int i=0; i<pSox->GetItemCount(); ++i )
			{
				int nIndex = pSox->GetItemID( i );
				int nItemIndex = pSox->GetFieldFromLablePtr( nIndex, "_NamedItem_ID" )->GetInteger();
				if( nItemIndex == pData->nItemID )
				{
					strTargetEffectName = pSox->GetFieldFromLablePtr( nIndex , "_Target_Effect" )->GetString();
					break;
				}
			}
		}

		if( strTargetEffectName.empty() == false )
		{
			DNVector(DnActorHandle) hVecList;
			CDnActor::ScanActor( *CDnActor::s_hLocalActor->GetPosition(), 1000.f, hVecList );

			for( DWORD i=0; i<hVecList.size(); i++ )
			{
				if( hVecList[i] && hVecList[i]->IsPlayerActor() )
				{
					CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hVecList[i].GetPointer());
					DnEtcHandle hHandle = CDnInCodeResource::GetInstance().CreatePlayerCommonEffect();
					if( hHandle )
					{
						hHandle->SetActionQueue( strTargetEffectName.c_str() );
						DnActorHandle hActor = pPlayer->GetActorHandle();
						if( pPlayer->IsVehicleMode() && pPlayer->GetMyVehicleActor() )
							hActor = pPlayer->GetMyVehicleActor()->GetActorHandle();

						if( hActor )
						{
							EtcObjectSignalStruct *pResult = hActor->TSmartPtrSignalImp<DnEtcHandle, EtcObjectSignalStruct>::InsertSignalHandle( -1, -1, hHandle );
							hHandle->SetParentActor( hActor );

							if( pResult )
							{
								sprintf_s( pResult->szBoneName, pPlayer->IsVehicleMode() ? "~Dummy_Seat" : "Bip01" );
								pResult->bLinkObject = true;
							}
						}				
					}
				}
			}
		}
	}
}
#endif // PRE_DRAGONBUFF

#ifdef PRE_ADD_MAINQUEST_UI
void CDnCommonTask::SetNpcTalkCamera(CDnNpcTalkCamera::WorkState eType)
{
	if( m_hTalkNpcCamera )
	{
		//if( ((CDnNpcTalkCamera*)m_hTalkNpcCamera.GetPointer())->GetWorkState() != CDnNpcTalkCamera::EndTalk ) 
		{
			((CDnNpcTalkCamera*)m_hTalkNpcCamera.GetPointer())->SetWorkState( eType );
		}
	}
}

bool CDnCommonTask::OpenRenewalMainQuestUI(QuestInfo* pQuestInfo, TALK_PARAGRAPH talk_para, UINT nNpcUniqueID)
{
	if(pQuestInfo)
	{
		// Renewal Main Quest UI
		if( pQuestInfo->cQuestType == QuestType_MainQuest && pQuestInfo->cUIStyle == QuestRenewal::UIStyle::Renewal )
		{
			CDnMainQuestDlg*  pMainQuestDlg = GetInterface().GetMainQuestDlg();
			if(pMainQuestDlg) pMainQuestDlg->SetMainQuestParagraph(talk_para);

			// MainQuest UI Open
			if(GetInterface().IsOpenMainQuestDlg() == false)
				GetInterface().ShowMainQuestDlg(this);

			// NPC ID 저장
			CDnLocalPlayerActor::SetTakeNpcUID(nNpcUniqueID);

			// 돌아가기 버튼 OFF
#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
			GetInterface().ShowQuestReturnButton(false);
#endif
			OutputDebug("OnTRecvNpcTalkMsg\n");

			return true;
		}
	}

	return false;
}

#endif // PRE_ADD_MAINQUEST_UI

void CDnCommonTask::OnRecvChangeJobDialog(SCOpenChangeJobDialog* pPacket)
{
	GetInterface().ShowJobChangeUI();
	GetInterface().AddChatMessage(eChatType::CHATTYPE_NORMAL,L"",L"CDnCommonTask::OnRecvChangeJobDialog");
}