#pragma once

#include "EtUIDialog.h"

#if defined( PRE_ADD_PRESET_SKILLTREE )

class CDnSkillTreeDlg;

class CDnSkillTreePreSetDlg : public CEtUIDialog
{
public:
	CDnSkillTreePreSetDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL, bool bAutoCursor = false );
	virtual ~CDnSkillTreePreSetDlg();

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

protected:
	CEtUIButton * m_pSaveButton;
	CEtUIButton * m_pDelButton;

	CDnSkillTreeDlg * m_pSkillTreeDlg;

public:
	void Enable( const bool bEnable );
	void SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg );
	void RefreshButtonShow( const bool bSave_Show, const bool bSave_Enable, const bool bDel_Show, const bool bDel_Enable );
};

#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )