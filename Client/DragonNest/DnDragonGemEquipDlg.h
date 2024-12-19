#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_DRAGON_GEM

class CDnDragonGemEquipDlg : public CDnCustomDlg
{
public:
	CDnDragonGemEquipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnDragonGemEquipDlg(void);
	void OnRecvDragonGemEquip(int nResult, int nSlotIndex);


	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void InitCustomControl(CEtUIControl *pControl);

protected:
	CDnItemSlotButton* m_pItemEquipSlot;
	CEtUITextBox* m_pDescription;
	CEtUIStatic* m_pMessage;

	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlot;
	CDnQuickSlotButton *m_pButtonQuickSlot;
};

#endif // PRE_ADD_DRAGON_GEM