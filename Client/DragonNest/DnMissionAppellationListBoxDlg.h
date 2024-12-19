#pragma once
#include "EtUIDialog.h"
#include "DnMissionTask.h"

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

class CDnMissionAppellationListBoxDlg : public CEtUIDialog
{
public:
	CDnMissionAppellationListBoxDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMissionAppellationListBoxDlg(void);

protected:
	enum
	{
		E_APPELLATION_HIDE = 0,
		E_APPELLATION_SHOW = 1
	};
	CEtUIComboBox*	m_pComboBox;
	CEtUIListBoxEx* m_pListBoxEx;
	CEtUIButton*	m_pAppellationButton;

	std::vector<CDnMissionTask::MissionInfoStruct*> m_vecMissionInfoList;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	void RefreshInfo();
	CDnMissionTask::MissionInfoStruct* CreateMission(int nArrayIndex);
};

#endif

