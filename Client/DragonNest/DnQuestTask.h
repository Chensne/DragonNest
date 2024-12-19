#pragma once
#include "Task.h"
#include "MessageListener.h"
#include "DNUserData.h"
#include "DNPacket.h"
#include "DnDataManager.h"
#include "DnQuestPathFinder.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "ReputationSystemRepository.h"
#include "DnNpcReaction.h"
#endif // PRE_ADD_NPC_REPUTATION_SYSTEM

class CDnQuestTabDlg;
class CDnInvenTabDlg;

class CDnQuestTask : public CTask, public CTaskListener, public CEtUICallback, public CSingleton<CDnQuestTask>
{
public:

	CDnQuestTask();
	virtual ~CDnQuestTask();
	
	bool Initialize();
	virtual void Process( LOCAL_TIME LocalTime, float fDelta );

protected:
	void OnRecvQuestMessage( int nSubCmd, char *pData, int nSize );
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	void OnRecvReputationMessage( int nSubCmd, char* pData, int nSize );
#endif // PRE_ADD_NPC_REPUTATION_SYSTEM

public:

	// DisPatchMassages
	virtual void OnDispatchMessage( int nMainCmd, int nSubCmd, char *pData, int nSize );

	void OnRecvQuestInfo( SCQuestInfo *pPacket );
	void OnRecvQuestCompleteInfo( SCQuestCompleteInfo *pPacket );
	void OnRecvRefreshQuest( SCRefreshQuest *pPacket );
	void OnRecvCompleteQuest( SCCompleteQuest *pPacket );
	void OnRecvMarkingCompleteQuest(SCMarkingCompleteQuest* pPacket );
	void OnRecvRemoveQuest( SCRemoveQuest *pPacket );
	void OnRecvQuestReward( SCQuestReward *pPacket );
	void OnRecvQuestResult(SCQuestResult * pPacket);
	void OnRecvPeriodQuest(SCAssignPeriodQuest* pPacket );
	void OnRecvNoticePeriodQuest(SCPeriodQuestNotice* pPacket);
	void OnRecvScorePeriodQuest(SCScorePeriodQuest *pPacket);

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CReputationSystemRepository* GetReputationRepository( void ) { return m_pReputationRepos; }; 	// 일단 questtask 에 두지만 평판 관련해서 양이 많아지면 따로 task 로 빼는 것이 바람직..
	void ClearNpcReaction( void ) { m_NpcReaction.Clear(); };

	bool CheckAndCalcStoreBenefit( int iType, /*IN OUT*/ int& iNeedCoin ); 	// 상점 혜택 처리. 보상 처리된다면 true 를 리턴한다.
	int GetStoreBenefitValue( int iType );

	virtual void OnRecvReputationList( SCReputationList* pPacket );
	virtual void OnRecvReputationModify( SCModReputation* pPacket );
	virtual void OnRecvReputationOpenGiveNpcPresent( SCOpenGiveNpcPresent* pPacket );
	virtual void OnRecvRequestSendNpcPresentSelect( SCRequestSendSelectedPresent* pPacket );
	virtual void OnRecvShowNpcEffect( SCShowNpcEffect* pPacket );
	virtual void OnRecvUnionPoint(SCUnionPoint* pPacket);
	virtual void OnRecvUnionMembershipList(SCUnionMembershipList* pData);
	virtual void OnRecvUnionBuyMembership(SCBuyUnionMembershipResult* pData);
#ifdef PRE_ADD_NEW_MONEY_SEED
	virtual void OnRecvSeedPoint( SCSendSeedPoint* pPacket );
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_FIX_71455_REPUTE_RENEW
	void RefreshRepUnionMembershipMark();
#endif // PRE_FIX_71455_REPUTE_RENEW
#endif // PRE_ADD_NPC_REPUTATION_SYSTEM

	// Quest Fuction
	void LoadQuestTalk();

	bool UpdateQuestInfo(int nQuestSlot, TQuest quest);
	void SetCompleteQuestFlag(int nQuestIndex, bool bFlag);

	void ProcessRefreshQuest();
	void ProcessRefreshQuest( SCRefreshQuest *pPacket );

	int GetQuestCount();
	void GetPlayQuest( std::vector<TQuest*> &vecProgQuest );
	void GetPlayMainQuest( OUT std::vector<TQuest*> &vecProgMainQuest );
	void GetCompleteMainQuest( OUT std::vector<Journal*> &vecCompleteMainQuest );
	void GetPlaySubQuest( OUT std::vector<TQuest*> &vecProgSubQuest );
	void GetCompleteSubQuest( OUT std::vector<Journal*> &vecCompleteSubQuest );
	void GetPlayQuest( const EnumQuestType eQuestType, OUT std::vector<TQuest*> &vecProgQuest );
	void GetCompleteQuest( const EnumQuestType eQuestType, OUT std::vector<Journal*> &vecCompleteQuest );

	const TQuestGroup* GetQuestGroup()		{ return &m_Quest; }
	const TQuest* GetQuest( int nIndex );
	const TQuest* GetQuestIndex( int nQuestIndex );

	int FindPlayingQuest(int nQuestID);
	int FindQuest(int nQuestID);
	int FindEmptyIndex();

	void CancelQuest(int nQuestIndex);
	void ClearMainQuestFromQuestID(int nQuestID);
	bool ClearQuestInfo(int nQuestSlot);
	bool IsClearQuest(int nQuestIndex);

	void SetRefreshNpcQuestState(bool bDoSet) { m_bIsRefreshNpcQuestState = bDoSet; }
	bool IsRefreshNpcQuestState() const { return m_bIsRefreshNpcQuestState; }

	void SetTraceQuestID( int nTraceQuestID )	{ m_nTraceQuestID = nTraceQuestID; }
	int  GetTraceQuestID()	{ return m_nTraceQuestID; }

	bool GetCurJournalCountingInfo(IN int nQuestIndex, OUT char& cCurJournalIndex, OUT std::vector<TCount>& Counting);
	void GetCurJournalCountingInfoString( int nQuestIndex, int nJournalPageIndex, OUT std::vector< boost::tuple<std::wstring, int, int> > &vecGetList, OUT wstring &szProgressString );
	int IsAllCompleteCounting(int QuestID);
#ifdef PRE_ADD_REMOTE_QUEST
	void OnRecvGainRemoteQuest( SCGainRemoteQuest* pPacket );
	void OnRecvCompleteRemoteQuest( SCCompleteRemoteQuest* pPacket );
	void OnRecvRemoveRemoteQuest( SCRemoveRemoteQuest* pPacket );
	void SendAcceptRemoteQuest( int nQuestIndex );
	void SendCompleteRemoteQuest( int nQuestIndex, int nRewardIndex, bool bSelectedArray[], bool bSelectedCashArray[] );
	struct stRemoteQuestAsk
	{
		int nQuestIndex;
		CDnQuestTree::eRemoteQuestState	remoteQuestState;
		stRemoteQuestAsk() : nQuestIndex( 0 ), remoteQuestState( CDnQuestTree::REMOTEQUEST_NONE ) {}
	};
	void AddRemoteQuestAskList( int nQuestIndex, CDnQuestTree::eRemoteQuestState remoteQuestState );
	void DeleteRemoteQuestAskList( int nQuestIndex );
	bool IsExistRemoteQuestAskList( int nQuestIndex, CDnQuestTree::eRemoteQuestState remoteQuestState );
	std::vector<stRemoteQuestAsk>& GetRemoteQuestAskList() { return m_vecRemoteQuestAskList; }
	void CancelRemoteQuest( int nQuestIndex );
	void GetRecompenseRemoteQuest( const EnumQuestType eQuestType, std::vector<TQuest*> &vecProgQuest );
#endif // PRE_ADD_REMOTE_QUEST

	// Notifier
	struct QuestNotifierInfo 
	{
		QuestNotifierInfo()
		{
			nQuestIndex = nJournalStep = nCurrentMapIndex = nTargetMapIndex = 0;
			vTargetPos.x = vTargetPos.y = 0.0f;
		}

		int nQuestIndex;
		int nJournalStep;			
		int nCurrentMapIndex;		
		int nTargetMapIndex;		
		D3DXVECTOR2 vTargetPos;		
		PathResult	pathResult;		
	};

	void ClearNotifierData( UINT uiIndex )
	{
		if( uiIndex >= DNNotifier::RegisterCount::TotalQuest )
			return;

		m_NotifierData[uiIndex].Clear();
		m_NotifierInfo[uiIndex].nQuestIndex = 0;
	}
	
	void UpdateQuestNotifierInfo(bool bForceUpdate = false );
	void RefreshQuestNotifierInfo();
	
	bool AddQuestNotifier( DNNotifier::Type::eType QuestType, int nQuestIndex );
	bool RemoveQuestNotifier( DNNotifier::Type::eType QuestType, int nQuestIndex );
	void RefreshNotifier( DNNotifier::Data *pData ) { memcpy( m_NotifierData, pData, sizeof( m_NotifierData) ); }
	void OnUpdateNotifier( bool bSaveToServer );

	Journal *GetNotifierJournal( int nIndex );
	JournalPage* GetNotifierJournalPage( int nIndex );
	QuestNotifierInfo GetNotifierInfo(int nIndex ) { return m_NotifierInfo[ nIndex ]; }

	CDnQuestPathFinder* GetPathFinder() {return &m_QuestPathFinder;}
	QuestNotifierInfo* GetQuestNotifierInfoByQuestID(int nQuestID);
	QuestNotifierInfo* GetQuestNotifierInfoByQuestIDForce(int nQuestID);

	int GetRegisteredSubQuestCount();
	bool IsRegisterQuestNotifier( DNNotifier::Type::eType QuestType, int nQuestIndex );
	bool IsMainQuestNotifierExist() { return m_NotifierData[0].iIndex >= 0 ;}
	bool IsSubQuestNotifierFull();

	// ETC
	void SetQuestDialog( CDnQuestTabDlg *pDialog ) { m_pQuestDialog = pDialog; }
	void SetInvenDialog( CDnInvenTabDlg *pDialog ) { m_pInvenDialog = pDialog; }

	CDnQuestTabDlg *GetQuestDlg(){ return m_pQuestDialog; }

private:
	TQuestGroup	m_Quest;
	CDnQuestPathFinder	m_QuestPathFinder;

	LOCAL_TIME m_nLastUpdateTime;	
	bool m_bIsRefreshNpcQuestState;
	std::vector<SCRefreshQuest> m_VecRefreshQuestList;
	
	QuestNotifierInfo   m_TraceNotifierInfo;
	QuestNotifierInfo	m_NotifierInfo[DNNotifier::RegisterCount::TotalQuest];
	DNNotifier::Data	m_NotifierData[DNNotifier::RegisterCount::TotalQuest];

	CDnQuestTabDlg *m_pQuestDialog;
	CDnInvenTabDlg *m_pInvenDialog;

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	CReputationSystemRepository* m_pReputationRepos;
	CDnNpcReaction m_NpcReaction;
	int m_iReputeUpSoundIndex;
	EtSoundChannelHandle m_hReputeUpSound;
#endif // PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef PRE_ADD_REMOTE_QUEST
	std::vector<stRemoteQuestAsk> m_vecRemoteQuestAskList;
#endif // PRE_ADD_REMOTE_QUEST

	UINT m_nTraceQuestID;
};

#define GetQuestTask()		CDnQuestTask::GetInstance()
