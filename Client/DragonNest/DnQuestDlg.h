#pragma once
#include "DnCustomDlg.h"
#include "DnDataManager.h"
#include <map>

class CDnQuestDescriptionDlg;
class CDnQuestRecompenseDlg;
class CEtUIQuestTreeCtl;
class CEtUIQuestTreeItem;
struct TQuest;

class CDnQuestDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		QUEST_DESCRIPTION_DIALOG,
		QUEST_RECOMPENSE_DIALOG,
		MESSAGEBOX_CONFIRM_REMOVEQUEST,
		MESSAGEBOX_ALERT_REMOVEQUEST
	};

	enum{
		LAST_SUB_QEUST_ID = 15036,	// 서브 퀘스트 마지막에 올 아이디값
	};

public:
	enum emQUEST_TYPE
	{
		typeMain,
		typeSub,
		typePeriod,
#ifdef PRE_ADD_REMOTE_QUEST
		typeRemote,
#endif // PRE_ADD_REMOTE_QUEST
	};

public:
	CDnQuestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnQuestDlg(void);

protected:
	CDnQuestDescriptionDlg *m_pDescriptionDlg;
	CDnQuestRecompenseDlg *m_pRecompenseDlg;

	CDnQuestTree *m_pTreeControl;
	CEtUIButton * m_pButtonTrace;
	CEtUIButton * m_pButtonDel;
	CEtUICheckBox *m_pQuestShowLevelCheckBox;
	CEtUICheckBox *m_pQuestNotifyPriority;

	emQUEST_TYPE m_emQuestType;
	int m_nQuestIndex;
	
	bool					m_bSaveNotifierToServerNeeded;

	typedef		CDnCustomDlg BaseClass;

	int m_nLocalLevel;

protected:
	CEtUIQuestTreeItem *FindJournal( int nChapterIndex, int nQuestIndex, int nMapIndex );
	CEtUIQuestTreeItem *FindJournalPage( int nChapterIndex, int nQuestIndex, int nJournalPageIndex );

	CEtUIQuestTreeItem *GetLatestJournalPage();
	static bool CompareSubQuestLevel( TQuest * pFirst, TQuest * pSecond );
	void SetQuestRecompenseDlg( int nQuestIndex );

public:
	CEtUIQuestTreeItem* Expand( int nChapterIndex, int nQuestIndex );
	void CollapseAll();
	void SetSelectItem( CEtUIQuestTreeItem *pItem );

protected:
	CEtUIQuestTreeItem *AddMainQuest( const TQuest* pQuest, const Journal* pJournal, DWORD dwType );
	CEtUIQuestTreeItem *AddSubQuest( const TQuest* pQuest, const Journal* pJournal, DWORD dwType );
#ifdef PRE_ADD_REMOTE_QUEST
	CEtUIQuestTreeItem* AddRemoteQuest( const TQuest* pQuest, const Journal* pJournal, DWORD dwType );
	CDnQuestTreeItem* GetRemoteQuestTreeItem( int nQuestIndex );
	CDnQuestTreeItem* SetRemoteQuest( int nQuestIndex, int nJournalPageIndex, DWORD dwType );
#endif // PRE_ADD_REMOTE_QUEST
	void RefreshProgQuest( const std::vector<TQuest*> &vecProgQuest );
	DWORD GetQuestColor( int nLevel );
	void SortSubQuest( std::vector<TQuest*> & vecProgQuest );
	void SetLevelCapRecompense( const TQuestRecompense & questRecompese );

public:
	void SetQuestType( emQUEST_TYPE emQuestType ) { m_emQuestType = emQuestType; }
	void AddQuest( const TQuest *pQuest );
	void DeleteQuest( const TQuest *pQuest );
	void AddJournal( const TQuest *pQuest, bool bAll = false );
	void CompleteQuest( const TQuest *pQuest );
	void RefreshJournal( const TQuest *pQuest );
	void Clear();
	int GetCurrentSelectedQuestIndex() { return m_nQuestIndex; }
	void OnTreeSelected();
	bool IsMainQuestDlg() { return (m_emQuestType == typeMain); }

public:
	void RefreshCompleteMainQuest();
	void RefreshCompleteSubQuest();
	void RefreshProgMainQuest();
	void RefreshProgSubQuest();
	void RefreshCompletePeriodQuest();
	void RefreshProgPeriodQuest();
#ifdef PRE_ADD_REMOTE_QUEST
	void RefreshCompleteRemoteQuest();
	void RefreshProgRemoteQuest();
#endif // PRE_ADD_REMOTE_QUEST
	void RefreshEnableNotifierCheck();
	void ImportNotifierToCheckBox();
#ifdef PRE_ADD_REMOTE_QUEST
	void RecieveRemoteQuest( int nQuestIndex, CDnQuestTree::eRemoteQuestState remoteQuestState );
#endif // PRE_ADD_REMOTE_QUEST
public:
	void InitialUpdate() override;
	void Initialize(const char *pFileName, bool bShow ) override;
	void Show( bool bShow ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) override;
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;

	CDnQuestTreeItem *FindMatchedQuestTreeItem( int nMapIndex );
};
