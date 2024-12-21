#pragma once

#include "DnCustomDlg.h"


class CDnItem;

class CDnPetAddSkillDlg : public CDnCustomDlg
{
public:
	CDnPetAddSkillDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetAddSkillDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetSkillItem( CDnItem* pItem ) { m_pSkillItem = pItem; }

protected:
	CDnItemSlotButton*	m_pItemSlot;
	CEtUIButton*		m_pButtonOK;
	CEtUIStatic*		m_pStatic;

	CDnItem*			m_pSkillItem;
	CDnItem*			m_pSkillItemSlot;
};

