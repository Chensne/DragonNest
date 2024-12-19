#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
#pragma once
#include "DnCustomDlg.h"
#include "EtUITabDialog.h"

//통합Event UI
class CDnQuestTabDlg;
class CDnNpcReputationTabDlg;

class CDnIntegrateQuestNReputationDlg : public CEtUITabDialog
{
public:
	enum WINDOW_TAB
	{
		QUEST_TAB = 0,
		REPUTATION_TAB = 1,
		TAB_COUNT
	};

	CDnIntegrateQuestNReputationDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnIntegrateQuestNReputationDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SelectTab(DWORD tabIndex) { m_SelectedTabIndex = tabIndex; }
	void SetEnableEventTab(bool bEnable, DWORD tabIndex);

	CDnQuestTabDlg* GetTimeEventDlg() { return m_pQuestDlg; }
	CDnNpcReputationTabDlg* GetAttendanceEventDlg() { return m_pReputationDlg; }

#ifdef PRE_ADD_REPUTATION_EXPOSURE
	void SetReputeNpcUnionPage(int nNpcID);
#endif

	//tabIndex 탭이 선택되어 져 있는지 확인..
	//다이얼로그 자체가 일단 보여지고 있는 상태(m_bShow == true)여야 한다.
	bool IsEventTabShow(DWORD tabIndex);

	void ShowQuestFromNotifier( int nChapterIndex, int nQuestIndex, bool bMainQuest );

	void SetCheckDailyTab();

protected:
	CEtUIRadioButton*	m_pButtonQuestDlg;			// 퀘스트
	CEtUIRadioButton*	m_pButtonReputationDlg;	// 호감도
	CEtUIStatic* m_pTitle;
	
	CDnQuestTabDlg* m_pQuestDlg;
	CDnNpcReputationTabDlg* m_pReputationDlg;

	DWORD m_SelectedTabIndex;
};

#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
