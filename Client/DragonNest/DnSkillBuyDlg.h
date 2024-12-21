#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnSkillBuyDlg : public CDnCustomDlg
{
public:
	CDnSkillBuyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL );
	virtual ~CDnSkillBuyDlg(void);

protected:
	CDnSkillSlotButton *m_pSkillSlotButton;
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticSP;
	CEtUIStatic *m_pStaticCoin;
	CEtUIStatic *m_pStaticItemCount;

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

public:
	void SetSkillInfo( MIInventoryItem *pSkill, MIInventoryItem *pNeedItem, const wchar_t *wszPrice );

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
};
