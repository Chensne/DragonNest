#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_DRAGON_GEM
#define DRAGON_GEM_SLOT_MAX 4
class CDnDragonGemTooltipDlg : public CDnCustomDlg
{
public:
	CDnDragonGemTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDragonGemTooltipDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void InitCustomControl(CEtUIControl *pControl);

protected:	
	CEtUIStatic*	m_pJewelSlot[DRAGON_GEM_SLOT_MAX][DRAGON_GEM_SLOT_MAX];
	CDnItemSlotButton* m_pItemSlot[DRAGON_GEM_SLOT_MAX];
	CEtUITextBox*	m_pTextBox;
	CEtUIStatic*	m_pStaticBtnCover[DRAGON_GEM_SLOT_MAX];
	CEtUIStatic*	m_pStaticBase;
	CEtUIStatic*	m_pStaticEquip;
	CEtUIStatic*	m_pStaticVertialLines[2];
	std::vector<CDnItemSlotButton*> m_vSlotButtonList;

public:
	void ShowDragonGemToolTip(CDnItem* pItem);
	void CloseDragonGemToolTip();
	void InitTooltipDragonGem(int nJewelTypeID);
	void ResetSlots();
};

#endif // PRE_ADD_DRAGON_GEM