#pragma once

#include "EtUIDialog.h"

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

class CDnSkillTreeDlg;
class CDnReservationGoldDlg;
class CDnReservationGoldNoDlg;

class CDnSkillTreeButtonDlg : public CEtUIDialog
{
public:
	CDnSkillTreeButtonDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL, bool bAutoCursor = false );
	virtual ~CDnSkillTreeButtonDlg();

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

protected:
	CEtUIButton * m_pResetButton;
	CEtUIButton * m_pApplyButton;

	CDnSkillTreeDlg * m_pSkillTreeDlg;
	CDnReservationGoldDlg * m_pReservationGoldDlg;
	CDnReservationGoldNoDlg * m_pReservationGlodNoDlg;

public:
	void SetButtonEnable( const bool bEnable );
	void SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg );
};

#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )