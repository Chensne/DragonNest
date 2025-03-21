#pragma once
#include "DnCustomDlg.h"
#include "DnItemTask.h"

class CDnItem;

class CDnPlateMixViewDlg : public CDnCustomDlg
{
public:
	CDnPlateMixViewDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPlateMixViewDlg(void);

protected:
	CEtUIStatic *m_pStaticProb;

	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlotButton;
	CEtUIStatic *m_pStaticItemQ;

public:
	void SetInfo( int nProb, int nItemID );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};