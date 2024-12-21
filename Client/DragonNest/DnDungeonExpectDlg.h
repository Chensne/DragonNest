#pragma once
#include "EtUITabDialog.h"
#include "DnDungeonEnterDlg.h"


class CDnDungeonExpectRewardDlg;
class CDnDungeonExpectMissionDlg;
class CDnDungeonExpectQuestDlg;

class CDnDungeonExpectDlg : public CEtUITabDialog
{
public:
	CDnDungeonExpectDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDungeonExpectDlg();

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

public:
	void UpdateDungeonExpectReward( int nMapIndex, int nDifficult );
	int GetMapIndex() { return m_nMapIndex; }
	int GetDiffcult() { return m_nDifficult; }

protected:
	CEtUIRadioButton*			m_pExpectRewardTab;
	CEtUIRadioButton*			m_pExpectQuestTab;
	CEtUIRadioButton*			m_pExpectMissionTab;

	CDnDungeonExpectRewardDlg*	m_pDnDungeonExpectRewardDlg;
	CDnDungeonExpectMissionDlg*	m_pDnDungeonExpectMissionDlg;
	CDnDungeonExpectQuestDlg*	m_pDnDungeonExpectQuestDlg;

	int m_nMapIndex;
	int m_nDifficult;
};

