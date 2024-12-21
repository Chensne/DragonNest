#pragma once
#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

#if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )

class CDnSkillTreeDlg;

class CDnReservationGoldNoDlg : public CEtUIDialog
{
public:
	CDnReservationGoldNoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnReservationGoldNoDlg(void);

protected:
	CEtUIStatic * m_pSPText;
	CEtUIStatic * m_pGoldText;
	CEtUIStatic * m_pSilverText;
	CEtUIStatic * m_pBronzeText;
	CDnSmartMoveCursor m_SmartMove;

	CDnSkillTreeDlg * m_pSkillTreeDlg;

	int m_nSP;

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

	void SetSkillTreeDlg( CDnSkillTreeDlg * pSkillTreeDlg );
	void SetInfo();
};

#endif	// #if defined( PRE_ADD_SKILL_LEVELUP_RESERVATION )