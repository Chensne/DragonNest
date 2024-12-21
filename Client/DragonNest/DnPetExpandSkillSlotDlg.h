#pragma once

#include "DnCustomDlg.h"


class CDnItem;

class CDnPetExpandSkillSlotDlg : public CDnCustomDlg
{
public:
	CDnPetExpandSkillSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetExpandSkillSlotDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

public:
	void SetExpandSkillSlotItem( CDnItem* pItem ) { m_pExpandSkillSlotItem = pItem; }

protected:
	CDnItemSlotButton*	m_pItemSlot;
	CEtUIButton*		m_pButtonOK;
	CEtUIStatic*		m_pStatic;

	CDnItem*			m_pExpandSkillSlotItem;
	CDnItem*			m_pExpandSkillSlotItemSlot;
};

