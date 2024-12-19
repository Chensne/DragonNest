#pragma once
#include "DnCustomDlg.h"
#include "DnDungeonEnterDlg.h"
#include "DnDataManager.h"


class CDnDungeonExpectQuestRewardDlg;

class CDnDungeonExpectQuestDlg : public CDnCustomDlg
{
public:
	CDnDungeonExpectQuestDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDungeonExpectQuestDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:
	int LoadQuestInCurrentMap( int nMapIndex, int nDifficult );

private:
	void SetQuestList( std::vector<TQuest*>& vecQuestList, int nMapIndex, int nDifficult );
	bool CheckAvailableQuest( JournalPage* pJournalPage, int nMapIndex );

protected:
	CEtUIListBoxEx* m_pQuestListBox;
	CDnDungeonExpectQuestRewardDlg*	m_pDnDungeonExpectQuestRewardDlg;
};

class CDnDungeonExpectQuestItemDlg : public CDnCustomDlg
{
public:
	CDnDungeonExpectQuestItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDungeonExpectQuestItemDlg();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();

public:
	void SetQuestIndex( int nQuestIndex ) { m_nQuestIndex = nQuestIndex; }
	int GetQuestIndex() { return m_nQuestIndex; }
	void SetInfo( bool bMainQuest, const wchar_t *szTitle, const wchar_t *szDest, std::vector< boost::tuple<std::wstring, int, int> > &vecGetList, std::wstring szProgress );

protected:
	CEtUIStatic* m_pStaticMainTitle;
	CEtUIStatic* m_pStaticSubTitle;
	CEtUIStatic* m_pStaticMainIcon;
	CEtUIStatic* m_pStaticSubIcon;
	CEtUIStatic* m_pStaticMissinoIcon;
	CEtUIStatic* m_pStaticSelect;
	CEtUITextBox* m_pTextBoxDescription;

	int	m_nQuestIndex;
};

class CDnDungeonExpectQuestRewardDlg : public CDnCustomDlg
{
public:
	CDnDungeonExpectQuestRewardDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnDungeonExpectQuestRewardDlg();

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );

protected:
	void DeleteAllItem();

public:
	void SetReward( TQuestRecompense& table );
	void SetItemOption( const int nTableID, const int nCount, TItemInfo & itemInfo );

protected:
	CEtUIStatic*	m_pStaticBoard1;
	CEtUIStatic*	m_pStaticBottom1;
	CEtUIStatic*	m_pStaticBoard2;
	CEtUIStatic*	m_pStaticBottom2;
	CEtUIStatic*	m_pStaticBoard3;
	CEtUIStatic*	m_pStaticBottom3;
	CEtUIStatic*	m_pStaticGold;
	CEtUIStatic*	m_pStaticSilver;
	CEtUIStatic*	m_pStaticBronze;
	CEtUIStatic*	m_pStaticExp;

	std::vector<CDnItemSlotButton*> m_vecItemSlotButton;
};

