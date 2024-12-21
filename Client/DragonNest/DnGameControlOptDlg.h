#pragma once
#include "DnOptionTabDlg.h"

class CDnGameControlComboDlg;
class CDnGameControlKeyDlg;
class CDnGameControlQuickSlotDlg;
class CDnGameControlUIDlg;
class CDnGameControlMouseDlg;
class CDnGameControlAttackDlg;
class CDnGameControlLooktDlg;
class CDnGameControlSideOptDlg;

class CDnGameControlOptDlg : public CDnOptionTabDlg
{
public:
	enum {
		CONTROL_BASEKEY = 0,
		CONTROL_QUICKSLOT,
		CONTROL_UI,
		CONTROL_MOUSE,
		CONTROL_PAD,
		CONTROL_COUNT,
	};
public:
	CDnGameControlOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGameControlOptDlg();

protected:
	CEtUIRadioButton *m_pTabButtonBaseKey;
	CEtUIRadioButton *m_pTabButtonQuickSlot;
	CEtUIRadioButton *m_pTabButtonUI;
	CEtUIRadioButton *m_pTabButtonMouse;
	CEtUIRadioButton *m_pTabButtonAttack;
	CEtUIRadioButton *m_pTabButtonLookt;
	CEtUIRadioButton *m_pTabButtonSideOpt;
	CEtUIRadioButton *m_pTabButtonPad;

	int	m_iComboIndex;
	CDnGameControlComboDlg * m_pGameControlComboDlg;

	CDnGameControlKeyDlg *m_pGameControlKeyDlg;
	CDnGameControlQuickSlotDlg *m_pGameControlQuickSlotDlg;
	CDnGameControlUIDlg *m_pGameControlUIDlg;
	CDnGameControlMouseDlg *m_pGameControlMouseDlg;
	CDnGameControlAttackDlg * m_pGameControlAttackDlg;
	CDnGameControlLooktDlg * m_pGameControlLooktDlg;
	CDnGameControlSideOptDlg * m_pGameControlSideOptDlg;

	void ConfigPad();

private:
	typedef CDnOptionTabDlg BaseClass;

public:

	virtual void ProcessCombo(const int index);
	
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};