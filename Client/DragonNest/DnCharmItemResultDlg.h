#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;

class CDnCharmItemResultDlg : public CDnCustomDlg
{
public:
	CDnCharmItemResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharmItemResultDlg(void);

protected:
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticItemName;

	CDnSmartMoveCursor m_SmartMove;

#if defined(PRE_FIX_43986)
	int m_nSoundIndex;
#endif // PRE_FIX_43986

public:
	void SetResultItem( int nItemID, int nCount, int nPeriod = 0, INT64 nGold = 0 );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};