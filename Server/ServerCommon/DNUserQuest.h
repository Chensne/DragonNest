#pragma once
#include "DNEventSystem.h"
/*---------------------------------------------------------------------------------------
									CDNUserQuest

	- 유저 캐릭터 퀘스트쪽 담당하는 클래스
	- VillageServer, GameServer가 공통으로 가지고 있는 클래스 이므로 Item과 마찬가지로 
	  if defined 떡칠(?)함 ㅋㅋㅋ
---------------------------------------------------------------------------------------*/


class CDNUserSession;
class CDNUserSendManager;
class CDNQuest;

class CDNUserQuest:public TBoostMemoryPool<CDNUserQuest>
{

private:
	CDNUserSession*	m_pSession;

	TQuestGroup* m_pQuest;

	TQuestReward m_QuestReward;
	int	m_RewardQuestID;
	int	m_RewardCheck;		// 퀘스트 보상 지급 순서가 사용자가 지정한 번호와 맞는지 체크하며 대상은 퀘스트 메모의 특정 인덱스(QUESTMEMOREWARDCHECK)

	CSSelectQuestReward m_PacketReward;
	std::set<int> m_CompletedPeriodQuests;

#if defined(PRE_ADD_REMOTE_QUEST)
	std::vector<int> m_nVecRemoteQuestGainList[EventSystem::EventTypeEnum_Amount];
	std::vector<int> m_nVecRemoteQuestAcceptWaitList;	//일단 퀘ID 로 넣어보자
#endif

public:
	CDNUserQuest(CDNUserSession *pSession);
	virtual ~CDNUserQuest();

	UINT GetUserObjID();

	bool IsClearQuest(int nQuestID);
	void SetCompleteQuestFlag(int nQuestID, bool bFlag);

	int	AddHuntingQuest(int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);

	int	AddQuest(int nQuestID, bool bSendErrorMsg = false);
	void OnCancelQuest(int nQuestID);
	int	CompleteQuest(int nQuestID, bool bDelPlayList, bool bRepeat);
	int	MarkingCompleteQuest(int nQuestID);
	int	ForceCompleteQuest(int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat);

	int	RewardAfterCompletingQuest(int nQuestID, bool bDelPlayList);
	void ResetRewardFlag();

	char* GetQuestCompleteBinary() { return(m_pQuest->CompleteQuest); }

	int	GetPlayingQuestCount();

	short HasQuest(int nQuestID);

	int	SetQuestStepAndJournalStep(int nQuestID, short nQuestStep, int nJournalStep, bool bFromScript);

	int	SetQuestStep(int nQuestID, short nQuestStep);
	short GetQuestStep(int nQuestID);
	int	SetJournalStep(int nQuestID, int nJournalStep);
	int	GetJournalStep(int nQuestID);

	int	SetQuestMemo(int nQuestID, char nMemoIndex, TP_QUESTMEMO nVal);
	TP_QUESTMEMO GetQuestMemo(int nQuestID, char nMemoIndex);
	int	IncQuestMemo(int nQuestID, char nMemoIndex);
	int	DecQuestMemo(int nQuestID, char nMemoIndex);

	int	SetCountingInfo(int nQuestID, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt);
	int	ClearCountingInfo(int nQuestID);

	int	IsAllCompleteCounting(int nQuestID);

#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
	void OnCounting(int nCountingType, int nCountItemID, int nItemCount=0);
#else
	void OnCounting(int nCountingType, int nCountItemID);
#endif//#if defined(PRE_FIX_QUEST_ITEM_ONCOUNT)
	void OnKillMonster(int nMonsterID);
	void OnDieMonster(int nMonsterID);
	void OnAddItem(int nItemID, short wCount);
	void OnAddSymbolItem(int nItemID);
	void OnAddQuestItem(int nItemID);
	void OnStageClear(int nMapIndex);
	void OnAddItemEx(int nItemID, short wCount);

	// 상태가 변경된 퀘스트정보를 클라이언트에게 갱신 시켜준다.
	int	SendQuestRefresh(int nRefreshType, int nQuestID);

	// 퀘스트 알리미 리프레쉬
	void OnSetActiveQuest(int nQuestID);

	// 퀘스트 보상을 세팅한다.
	void SetReward(TQuestReward& Reward, int nQuestID, int nRewardCheck);

	// 보상패킷을 임시로 저장한다.
	void SavePacketReward(CSSelectQuestReward* pPacket);

	// 보상데이터를 테이블에서 뽑아 임시저장한다.
	bool PreparePacketReward(int nQuestID);

	bool IsRewardItem();
	bool IsRewardCashItem();

	void OnSelectRewardEx(bool bSelectedArray[], bool bSelectedCashArray[]);
	void OnPeriodEvent(int nQuestID, bool bFlag);

	// For CheatCommand
	void SendAllQuestRefresh();
	void ClearAllQuestInfo();
	void DumpQuest(std::vector<TQuest>& output);
	int	FindPlayingQuest(int nQuestID);

	void AddCompletedPeriodQuest(int questID);
	void RemoveCompletedPeriodQuest(int questID);
	bool IsCompletedPeriodQuest(int questID) const;	

#if defined(PRE_ADD_REMOTE_QUEST)

	void RefreshRemoteQuest(int nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList);
	void GetAcceptWaitRemoteQuest(int *nAcceptWaitRemoteQuestCount, int *AcceptWaitRemoteQuestList);
	
	bool OnEvent( EventSystem::EventTypeEnum Event );

	void CheckRemoteQuestGain(EventSystem::EventTypeEnum Event);
	bool CheckRemotePreQuestCondition( TRemoteQuestData *pData );
	void CheckAcceptWaitRemoteQuestList();

	bool RemoveRemoteQuestFromAcceptWaitList(int nQuestID);
	bool AddRemoteQuestToAcceptWaitList(int nQuestID);
	void AcceptRemoteQuest(int nQuestID);
	void CancelRemoteQuest(int nQuestID);
	void CompleteRemoteQuest(int nQuestID, CSSelectQuestReward* pPacket);

	int FindRecompenseQuestIndex(int nQuestID);
	bool SetQuestToRecompenseState(int nQuestID);

#endif	//#if defined(PRE_ADD_REMOTE_QUEST)


private:
	int	_RemoveQuest(int nQuestID, bool bComplete, bool bRepeat);

	// 이미 플레이 중인 퀘스트인지 찾는다. 있으면 그 인덱스를 리턴한다. 없으면 -1
	int	_FindPlayingQuest(int nQuestID);
	// 플레이 퀘스트 슬롯의 빈곳을 찾는다. 없으면 -1
	int	_FindEmptyPlayQuestIndex();

	// 진행중인 서브 퀘스트중에 가장 높은 퀘스트 스탭을 찾는다. return QuestID; 
	TQuest _FindHighestQStepSubQuest();
#if defined(PRE_ADD_REMOTE_QUEST)
	void _ResetRemoteQuest();
	void _RefreshRemoteQuestList();
	// 완료 후 보상대기중인 원격퀘스트인지 찾는다. 있으면 그 인덱스를 리턴한다. 없으면 -1	
	int _FindRecompenseQuestIndex(int nQuestID);
	// 수락 대기중인 원격퀘스트인지 찾는다. 있으면 그 인덱스를 리턴한다. 없으면 -1	
	int _FindAcceptWaitQuest(int nQuestID);
	// 획득 가능한 원격퀘 인지 검증
	bool _CheckRemoteQuestGainValidation(int nQuestID, CDNQuest* pQuest);
#endif

	int _AddItemByQuestReward( const TQuestReward::_ITEMSET* pItemSet );
};