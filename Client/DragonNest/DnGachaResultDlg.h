#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

#ifdef PRE_ADD_GACHA_JAPAN

class CDnItem;

class CDnGachaResultDlg : public CDnCustomDlg
{
private:
	CDnItemSlotButton* m_pResultItemSlot;
	CEtUITextBox* m_pResultItemTextBox;
	CDnSmartMoveCursor m_SmartMove;

protected:

public:
	CDnGachaResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaResultDlg(void);

public:
	void Initialize( bool bShow );
	void InitialUpdate( void );
	void InitCustomControl( CEtUIControl *pControl );
	void Show( bool bShow );

	void SetItem( CDnItem *pItem );
	CDnItem *GetItem( void ) { return (CDnItem*)m_pResultItemSlot->GetItem(); }
};

#endif // PRE_ADD_GACHA_JAPAN