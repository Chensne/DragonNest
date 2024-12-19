#pragma once

#include "DnCustomDlg.h"

#ifdef PRE_ADD_PET_EXTEND_PERIOD
class CDnBuyConfirmPetExtendPeriodDlg;
#endif // PRE_ADD_PET_EXTEND_PERIOD
class CDnItem;


#ifdef PRE_ADD_PET_EXTEND_PERIOD
class CDnPetExtendPeriodDlg : public CDnCustomDlg, public CEtUICallback
#else // PRE_ADD_PET_EXTEND_PERIOD
class CDnPetExtendPeriodDlg : public CDnCustomDlg
#endif // PRE_ADD_PET_EXTEND_PERIOD
{
public:
	CDnPetExtendPeriodDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPetExtendPeriodDlg();

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#endif // PRE_ADD_PET_EXTEND_PERIOD

public:
	void SetExtendPeriodItem( CDnItem* pItem ) { m_pExtendPeriodItem = pItem; }
	void SetPetItem( CDnSlotButton* pPressedButton );

private:
	void CheckPetItem();

protected:
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	enum{ BUYCONFIRM_PETEXTENDPERIODITEM = 0 };
	CDnBuyConfirmPetExtendPeriodDlg* m_pDnBuyConfirmPetExtendPeriodDlg;
#endif // PRE_ADD_PET_EXTEND_PERIOD

	CDnItemSlotButton*	m_pItemSlot;
	CEtUIButton*		m_pButtonOK;
	CDnSlotButton*		m_pButtonSlot;
	CEtUIStatic*		m_pStatic;

	CDnItem*			m_pExtendPeriodItem;
	CDnItem*			m_pPetItem;
	int					m_nExtendPeriod;
};

