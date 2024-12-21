#pragma once
#include "DnCustomDlg.h"
#include "DnItemSlotButton.h"

#ifdef PRE_SPECIALBOX

class CDnSpecialBoxConfirmDlg : public CDnCustomDlg
{

protected:
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticItemName;
	CEtUIStatic *m_pStaticAsk;

	int m_nRewardID;

public:
	CDnSpecialBoxConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnSpecialBoxConfirmDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetItemInfo( int nRewardID, CDnItem *pItem );

};

#endif // PRE_SPECIALBOX