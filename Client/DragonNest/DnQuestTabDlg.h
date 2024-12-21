#pragma once

#include "EtUITabDialog.h"
#include "DnQuestTask.h"
#include "DnSmartMoveCursor.h"

class CEtUIRadioButton;
class CDnQuestDlg;
struct TQuest;

class CDnQuestTabDlg : public CEtUITabDialog
{
public:
	CDnQuestTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnQuestTabDlg(void);

protected:
	CDnQuestDlg* m_pMainQuestDlg;
	CDnQuestDlg* m_pSubQuestDlg;

	CEtUIRadioButton* m_pMainTabButton;
	CEtUIRadioButton* m_pSubTabButton;

	CDnQuestDlg* m_pPeriodQuestDlg;
	CEtUIRadioButton* m_pPeriodTabButton;
#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP
	CEtUIStatic *m_pStaticQuestCount;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
	CDnQuestDlg* m_pRemoteQuestDlg;
	CEtUIRadioButton* m_pRemoteTabButton;
#endif // PRE_ADD_REMOTE_QUEST

	CDnSmartMoveCursorEx m_SmartMoveEx;

protected:
	CDnQuestDlg* GetQuestDialog( EnumQuestType emQuestType );
	int m_nLocalLevel;

public:
	void AddQuest( const TQuest *pQuest );
	void DeleteQuest( const TQuest *pQuest );
	void AddJournal( const TQuest *pQuest, bool bAll = false );
	void CompleteQuest( const TQuest *pQuest );
	void RefreshQuest();
	void RefreshJournal( const TQuest *pQuest );
	void RefreshSummaryInfo();
	void ShowQuestFromNotifier( int nChapterIndex, int nQuestIndex,  bool bMainQuest );
	void RefreshQuestNotifierCheck();
#ifdef PRE_ADD_REMOTE_QUEST
	void RecieveRemoteQuest( int nQuestIndex, CDnQuestTree::eRemoteQuestState remoteQuestState );
#endif // PRE_ADD_REMOTE_QUEST

	void SetCheckDailyTab();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};