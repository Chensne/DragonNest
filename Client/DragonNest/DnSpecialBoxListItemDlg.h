#pragma once
#include "DnCustomDlg.h"
#include "DnItem.h"
#include "DNPacket.h"

#ifdef PRE_SPECIALBOX

class CDnSpecialBoxListItemDlg : public CDnCustomDlg
{
protected:
	CDnItemSlotButton*	m_pItemSlotButton;
	CEtUIStatic *m_pStaticItemName;
	CDnItem *m_pItem;

public:
	CDnSpecialBoxListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSpecialBoxListItemDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

public:
	void SetRewardItemInfo( TSpecialBoxItemInfo *pItem );
	CDnItem *GetItem();
};

#endif // PRE_SPECIALBOX
