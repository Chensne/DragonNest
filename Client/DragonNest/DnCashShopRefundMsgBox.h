#pragma once

#include "EtUIDialog.h"

#ifdef PRE_ADD_CASHSHOP_REFUND_CL

class CDnCashShopRefundMsgBox : public CEtUIDialog
{
public:
	CDnCashShopRefundMsgBox(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopRefundMsgBox(void);

	void SetMsgBoxText(CDnSlotButton* pSlotBtn, const std::wstring& itemName, int refundCash);

	virtual void	InitialUpdate();
	virtual void	Initialize(bool bShow);

	virtual void	Show(bool bShow);

protected:
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

	CEtUIButton*	m_pOKBtn;
	CEtUIButton*	m_pCancelBtn;
	CEtUIStatic*	m_pItemNameStatic;
	CEtUIStatic*	m_pRefundCashStatic;
	CDnSlotButton*	m_pSlotBtn;
};

#endif // PRE_ADD_CASHSHOP_REFUND_CL