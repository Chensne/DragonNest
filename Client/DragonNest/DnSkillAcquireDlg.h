#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnSkillAcquireDlg : public CDnCustomDlg
{
public:
	CDnSkillAcquireDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnSkillAcquireDlg(void);

protected:
	CDnSkillSlotButton *m_pSkillSlotButton;
	CEtUIStatic *m_pStaticSkillName;
	CEtUIStatic *m_pStaticSkillSP;

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

public:
	void SetSkill( MIInventoryItem *pSkill );

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void Show( bool bShow );
};
