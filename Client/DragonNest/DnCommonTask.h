#pragma once

#include "Task.h"
#include "MessageListener.h"
#include "DNPacket.h"
#ifdef PRE_ADD_QUICK_PVP
#include "DnAcceptRequestInfo.h"
#include "DnAcceptRequestInviteInfo.h"
#include "DnAcceptRequestDataManager.h"
#endif
#ifdef PRE_ADD_MAINQUEST_UI
#include "DnNpcTalkCamera.h"
#include "DnDataManager.h"
#endif

class CDnCutSceneTask;
class CDnMiniSiteChecker;


//class CDnCommonTask : public CTask, public CClientTcpSession, public CClientUdpSession, public CEtUICallback
class CDnCommonTask : public CTask, public CTaskListener, public CEtUICallback
{
public:
	CDnCommonTask();
	virtual ~CDnCommonTask();

	bool Initialize();

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

	enum InitTimeProcessEnum {
		ITP_PlayCutScene,
//		ITP_OpenDungeonLevel,
	};

	struct InitTimeProcessStruct {
		InitTimeProcessEnum Type;
		std::vector<std::string> szVecParam;
	};

	enum DungeonOpenNoticeTypeEnum {
		OpenWorldMap,
		OpenDungeon,
		OpenAbyss,
	};
	struct DungeonOpenNoticeStruct {
		DungeonOpenNoticeTypeEnum Type;
		int nMapID;
		int nMapNameID;
	};

protected:
	void OnRecvCharMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvNpcMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvQuestMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvChatMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvTriggerMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvSystemMessage( int nSubCmd, char *pData, int nSize );
	void OnRecvEtcMessage( int nSubCmd, char *pData, int nSize );
#ifdef PRE_ADD_QUICK_PVP
	void OnRecvPvPMessage( int nSubCmd, char *pData, int nSize );
#endif
	// Loading 관련 함수들
	static bool __stdcall OnLoadRecvCharNpcEnteredCallback( void *pThis, void *pParam, int nSize, LOCAL_TIME LocalTime );
	static int __stdcall OnLoadRecvCharNpcEnteredUniqueID( void *pParam, int nSize );
	static bool __stdcall OnCheckLoadingPacket( void *pThis, UINT nUniqueID, int nMainCmd, int nSubCmd, void *pParam, int nSize );

#if defined (_KRAZ) || defined (_WORK)
	void ProcessPlayTimeNotice();
#endif // #if defined (_KRAZ) || defined (_WORK)

#ifdef PRE_ADD_MAINQUEST_UI
	bool OpenRenewalMainQuestUI(QuestInfo* pQuestInfo, TALK_PARAGRAPH talk_para, UINT nNpcUniqueID);
#endif

	void ProcessServerClose();

public:
	virtual void OnConnectTcp() {}
	virtual void OnDisconnectTcp( bool bValidDisconnect ) {}

	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	bool SendNpcTalkToNextStep(bool bCloseNpcDlg, const std::wstring& szIndex, const std::wstring& szTarget);

	virtual void OnRecvCharNpcEntered( SCEnterNpc *pPacket );
	virtual void OnRecvCharNpcLeaved( SCLeaveNpc *pPacket );

	virtual void OnRecvCharPropNpcEntered( SCEnterPropNpc *pPacket );
	virtual void OnRecvCharPropNpcLeaved( SCLeavePropNpc *pPacket );

	bool OpenCompleteNpcTalk(int nNpcIndex);
	bool OpenCompleteRewardMsg();
	bool IsHaveCompletedQuest();

	void OnRecvCharPCBang( SCPCBang *pPacket );
	void OnRecvNpcTalkMsg( SCNpcTalk *pPacket );
	void OnRecvShowWareHouse( char* pData );
	void OnRecvShowCompoundEmblem( char* pData );
	void OnRecvShowUpgradeJewel( char* pData );
	void OnRecvShowMailBox( char* pData);
	void OnRecvShowDisjointItem(char* pData);
	void OnRecvShowUpgradeItem( SCOpenUpgradeItem* pPacket );
	void SendWindowState(short sWinState);
	void OnRecvShowCompoundItem( SCOpenCompoundItem* pPacket );
	void OnRecvShowCompound2Item( SCOpenCompound2Item* pPacket );
	void OnRecvShowGuildMgrBox( SCOpenGuildMgrBox* pPacket );
	void OnRecvShowGlyphLift( char* pData );
	void OnRecvShowInventory( char* pData );
#ifdef PRE_ADD_EXCHANGE_ENCHANT
	void OnRecvShowExchangeEnchant(char* pData);
#endif
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	void OnRecvShowTextureDIalog( SCOpenTextureDialog* pPacket );
	void OnRecvCloseTextureDialog(SCCloseTextureDialog* pPacket );
#endif // PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG

	virtual void OnRecvChatChatMsg( SCChat *pPacket, int nSize );
	virtual void OnRecvChatWorldSystemMsg( SCWorldSystemMsg *pPacket, int nSize );
	virtual void OnRecvChatNoticeMsg(SCNotice * pPacket, int nSize);
	virtual void OnRecvChatGuildChat(SCGuildChat *pPacket, int nSize);
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	virtual void OnRecvChatDoorsGuildChat(SCDoorsGuildChat *pPacket, int nSize);
#endif		//#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	virtual void OnRecvChatNoticeCancelMsg();
	virtual void OnRecvChatServerMsg( SCChatServerMsg *pPacket, int nSize );

	virtual void OnRecvQuestPlayCutScene( SCPlayCutScene *pPacket );
	virtual void OnRecvQuestCompleteCutScene( SCCompleteCutScene *pPacket );
	virtual void OnRecvQuestSkipCutScene( SCSkipCutScene *pPacket );
	virtual void OnRecvQuestSkipAllCutScene( char *pPacket );

	virtual void OnRecvTriggerCallAction( SCTriggerCallAction *pPacket );
	virtual void OnRecvTriggerChangeMyBGM( SCChangeMyBGM *pPacket );
	virtual void OnRecvTriggerRadioImage( SCRadioImage *pPacket );
	virtual void OnRecvTriggerFileTableBGMOff( SCFileTableBGMOff * pPacket );
	virtual void OnRecvTriggerForceEnableRide( SCForceEnableRide * pPacket );

	//rlkt_test
	virtual void OnRecvChangeJobDialog(SCOpenChangeJobDialog* pPacket);

	void OnRecvServiceClose(SCServiceClose * pPacket);

	void OnRecvShowFarmWareHouse( char * pData );
	void OnRecvOpenMovieBrowser(char* pData);
	void OnRecvOpenBrowser(SCOpenBrowser* pData);
	const char* GetMiniSiteURL() const;
	void GetMiniSiteTooltip(std::wstring& tooltip);

	void OnRecvCharCommonVariable( SCModCommonVariable* pPacket );
	std::map<CommonVariable::Type::eCode, CommonVariable::Data>& GetVariableData() { return m_mVariableData; }
	bool HasVariableData(CommonVariable::Type::eCode type) const;

	bool GetCheckAttendanceFirst() { return m_bCheckAttendanceFirst; }
	void SetCheckAttendanceFirst( bool bCheckAttendanceFirst ) { m_bCheckAttendanceFirst = bCheckAttendanceFirst; }
	void OnRecvCheckAttedanceResult( SCAttendanceEvent *pPacket );

	void AddInitTimeProcess( InitTimeProcessEnum Type, std::string szParam1 = std::string(""), std::string szParam2 = std::string(""), std::string szParam3 = std::string(""), std::string szParam4 = std::string("") );
	void ResetInitTimeProcess();
	void ExecuteInitTimeProcess( InitTimeProcessStruct *pStruct );
	bool IsExistInitTimeProcess() { return !m_VecInitTimeProcessList.empty(); }

#ifdef PRE_ADD_WEEKLYEVENT
	void ProcessTimeEvent( LOCAL_TIME LocalTime, float fDelta );
#endif

	void PlayCutScene( int nCutSceneTableID, int nQuestIndex = -1, int nQuestStep = -1, DWORD dwQuestPlayerUniqueID = UINT_MAX );
	void EndCutScene();
	void SkipCutScene();
	bool IsControlQuestScriptCutScene() { return m_bControlQuestScript; }

	bool IsPlayedCutScene( int nCutSceneTableId );
	void ResetPlayedCutScene();

	void BeginNpcTalk( DnActorHandle hNpc );
	void EndNpcTalk( bool bPlaySound = true );

	void CheckDungeonOpenNotice( int nLevel, int nQuestID ); // True 면 퀘스트 완료시, False 면 레벨업시 체크합니다.
	void ClearDungeonOpenNotice();
//	void ShowDungeonOpenNotice();

	DWORD GetDungeonOpenNoticeCount() { return (DWORD)m_VecDungeonOpenNoticeList.size(); }
	DungeonOpenNoticeStruct *GetDungeonOpenNoticeData( DWORD dwIndex ) { return &m_VecDungeonOpenNoticeList[dwIndex]; }
	bool CheckAndInsertDungeonOpenNotice( int nMapIndex, DungeonOpenNoticeTypeEnum EnterMapType );

	virtual bool OnCloseTask();
	void SetNpcTalkRequestWait(bool bEnable = true ) { m_bRequestNpcTalk = bEnable;  }
	bool IsRequestNpcTalk() { return m_bRequestNpcTalk; }
	bool IsPcBang(){ return m_cPCBangGrade != PCBang::Grade::None; }
	char GetPCBangGrade() { return m_cPCBangGrade; }
	bool IsPCBangClearBox();
#ifdef PRE_FIX_63975
	bool IsHavePcBangRentalItem(){ return m_bHavePcBangRentalItem; }
	void NotifyRecvPcBangRentalItem(){ m_bHavePcBangRentalItem = true; }
#endif

#if defined (_KRAZ) || defined (_WORK)
	void ResetPlayTimeNotice();
#endif // defined (_KRAZ) || defined (_WORK)

	void UsedCutSceneCheat( void ) { m_bCheatCutScene = true; };
	
	int GetCurrentTalkNpcID(){return m_nCurrentTalkNpcID;}

	void OnRecvDarkLairRankBoard( SCOpenDarkLairRankBoard* pPacket );
	void OnRecvOpenDarkLairRankBoard( SCDarkLairRankBoard* pPacket );
	void OnRecvPVPLadderRankBoard( SCOpenPvPLadderRankBoard* pPacket );
	void OnRecvOpenPVPLadderRankBoard( SCPvPLadderRankBoard* pPacket );

#ifdef PRE_ADD_QUICK_PVP
	void OnRecvQuickPvPInvite(SCQuickPvPInvite* pData);
	void OnRecvQuickPvPResult(SCQuickPvPResult* pData);
	float GetQuickPvPShortestAcceptTime() const;
	void OpenQuickPvPAcceptRequestDialog();
	#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	bool IsRequestingQuickPvP() const { return m_bQuickPvPRequesting; }
	void SetRequestingQuickPvP(bool bSet) { m_bQuickPvPRequesting = bSet; }
	#endif
#endif


#ifdef PRE_ADD_PVP_RANKING
	void OnRecvPvPRankBoardColosseum( char * pData ); // 내정보 - 콜로세움.
	void OnRecvPvPRankBoardLadder( char * pData );    // 내정보 - 레더.

	void OnRecvPvPRankInfo( char * pData );		  // 캐릭터명 검색정보 - 콜로세움.
	void OnRecvPvPRankLadderInfo( char * pData ); // 캐릭터명 검색정보 - 레더.
	
	void OnRecvPvPRankList( char * pData );		   // Rank List - 콜로세움.	
	void OnRecvPvPRankLadderList( char * pData );  // Rank List - 레더.	
	
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_GAMEQUIT_REWARD
	void SendLogOutMessage();				 // 접속종료시 전송.
	void OnRecvLogOutMessage( char * pData );// 접속종료 수신.

	void SendGiftReceiving( int type );				 // 선물받기요청.
	void OnRecvGiftReceivingMessage( char * pData ); // 선물받기 수신.
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_DRAGONBUFF
	void OnRecvWorldBuffMsg( SCWorldBuffMsg *pData );
#endif

#ifdef PRE_ADD_MAINQUEST_UI
	void SetNpcTalkCamera(CDnNpcTalkCamera::WorkState eType);
	DnCameraHandle GetNpcTalkCamera() { return m_hTalkNpcCamera; }
#endif

private:
	char m_cPCBangGrade;
	CDnCutSceneTask* m_pCutSceneTask;

	std::vector<InitTimeProcessStruct> m_VecInitTimeProcessList;
	InitTimeProcessStruct *m_pFadeProcess;

	std::vector<int> m_nVecPlayedCutSceneList;

	bool m_bControlQuestScript;
	bool m_bQuestCutSceneAutoFadeIn;
	int m_nQuestCutSceneID;
	bool m_bSkipCutScene;
	UINT m_nQuestCutSceneNpcID;
	DnCameraHandle m_hTalkNpcCamera;
	bool m_bRequestNpcTalk;

	std::vector<DungeonOpenNoticeStruct> m_VecDungeonOpenNoticeList;
	int m_nCurrentTalkNpcID;  // 방금 말건 NPC의 아이디를 얻습니다.

	std::vector<int> m_nVecCompleteQuest;
	bool m_bForceOpenRecompense;
	int m_nSelectedCompleteQuestIndex;

#if defined (_KRAZ) || defined (_WORK)
	DWORD m_dwSelectCharTime;
	int m_nPlayTimeNoticeCount;
#endif // defined (_KRAZ) || defined (_WORK)

	__time64_t m_tServerCloseTime;
	int m_nProcessedRemainTime;

	// 컷신 치트키로 컷신을 보는 건가.
	bool m_bCheatCutScene;
	bool m_bMinimizeFlag;

	std::map<CommonVariable::Type::eCode, CommonVariable::Data> m_mVariableData;

	bool m_bCheckAttendanceFirst;
	CDnMiniSiteChecker* m_pMiniSiteChecker;
#ifdef PRE_ADD_QUICK_PVP
	CDnAcceptRequestDataManager<CDnQuickPvPAcceptRequestInfo> m_AcceptRequestQuickPvPInvite;
	CDnQuickPvPAcceptRequestInfo m_CurrentAcceptRequestCache;
	#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	bool m_bQuickPvPRequesting; // temp by kalliste : SystemState 기능이 추가되면 SystemState로 대체할 것.
	#endif
#endif

#ifdef PRE_FIX_63975
	bool m_bHavePcBangRentalItem;
#endif

#ifdef PRE_ADD_WEEKLYEVENT
	int m_nCurrentEventWeek;
	LOCAL_TIME m_tWeeklyEventRefreshTick;
#endif
};
