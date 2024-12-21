#pragma once

#include "EtUITabDialog.h"
#include "DnMissionTask.h"

class CDnMissionListItem;

class CDnMissionGuildCommonDlg : public CEtUIDialog
{
public:
	CDnMissionGuildCommonDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMissionGuildCommonDlg(void);

protected:
	CEtUIListBoxEx *m_pListBoxEx;
	CEtUITreeCtl *m_pTreeCtrl;
	CDnMissionTask::DailyMissionTypeEnum m_LastSelectType;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void RefreshSubCategory();
	void RefreshList( CDnMissionTask::DailyMissionTypeEnum Type );
	void SetCategory( CDnMissionTask::DailyMissionTypeEnum Type ) { m_LastSelectType = Type; }
	void SelectMissionItemFromNotifier( int nMissionItemID );
};