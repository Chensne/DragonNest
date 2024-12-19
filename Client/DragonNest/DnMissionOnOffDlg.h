#pragma once

#include "EtUITabDialog.h"
#include "DnMissionTask.h"

class CDnMissionOnOffDlg : public CEtUIDialog
{
public:
	CDnMissionOnOffDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMissionOnOffDlg(void);

protected:
	CEtUIListBoxEx *m_pListBoxEx;
	CEtUITreeCtl *m_pTreeCtrl;
	CDnMissionTask::MainCategoryEnum m_MainCategory;
	std::string m_szLastSelectCatogory;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetMainCategory( CDnMissionTask::MainCategoryEnum Category );
	void RefreshSubCategory();
	void RefreshList( std::string &szCodeStr );
#ifdef PRE_MOD_MISSION_HELPER
	void SetLastSelectCatogory( std::string &szStr ) { m_szLastSelectCatogory = szStr; }
	void SelectMissionItemFromNotifier( int nMissionItemID );
#endif
};