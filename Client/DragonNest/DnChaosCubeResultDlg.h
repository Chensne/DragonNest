#pragma once

#ifdef PRE_ADD_CHAOSCUBE


#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;

class CDnChaosCubeResultDlg : public CDnCustomDlg
{
public:
	CDnChaosCubeResultDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChaosCubeResultDlg(void);

protected:
	
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticItemName;

	CDnSmartMoveCursor m_SmartMove;

#if defined(PRE_FIX_43986)
	int m_nSoundIndex;
#endif // PRE_FIX_43986

public:

	void SetResultItem( int nItemID, int nCount );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};

#endif