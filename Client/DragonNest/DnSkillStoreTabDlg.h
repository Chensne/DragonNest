#pragma once
#include "EtUITabDialog.h"
#include "DnSkill.h"

class CDnSkillStoreDlg;

class CDnSkillStoreTabDlg : public CEtUITabDialog
{
public:
	CDnSkillStoreTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSkillStoreTabDlg(void);

protected:
	CEtUIRadioButton *m_pActiveSkillTabButton;
	CEtUIRadioButton *m_pPassiveSkillTabButton;
	CDnSkillStoreDlg *m_pActiveSkillStoreDlg;
	CDnSkillStoreDlg *m_pPassiveSkillStoreDlg;

protected:
	void UpdateSkillTabButton();

public:
	void SetItem( int nSlotIndex, MIInventoryItem *pItem );
	void ResetAllItem();

	void OnRefreshSlot();
	void OnRefreshDialog();

public:
	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};