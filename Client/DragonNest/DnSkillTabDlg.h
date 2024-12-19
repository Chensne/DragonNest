#pragma once
#include "EtUITabDialog.h"
#include "DnSkill.h"

class CEtUIRadioButton;
class CDnSkillDlg;
class CDnSkillRadioMsgDlg;
class CDnSkillGestureDlg;

class CDnSkillTabDlg : public CEtUITabDialog
{
public:
	CDnSkillTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnSkillTabDlg(void);

protected:
	CEtUIRadioButton *m_pActiveSkillTabButton;
	CEtUIRadioButton *m_pPassiveSkillTabButton;
	CEtUIRadioButton *m_pRadioMsgSkillTabButton;
	CEtUIRadioButton *m_pGestureSkillTabButton;
	CDnSkillDlg *m_pActiveSkillDlg;
	CDnSkillDlg *m_pPassiveSkillDlg;
	CDnSkillRadioMsgDlg *m_pRadioMsgSkillDlg;
	CDnSkillGestureDlg *m_pGestureSkillDlg;

	CEtUIStatic *m_pStaticMoney;
	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetAllItem();

	void OnSkillLevelUp( bool bSuccessed );
	void UpdatePage();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};