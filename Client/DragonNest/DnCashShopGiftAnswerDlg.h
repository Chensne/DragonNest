#pragma once

#include "EtUIDialog.h"

class CDnCashShopGiftAnswerDlg : public CEtUIDialog
{
public:
	CDnCashShopGiftAnswerDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopGiftAnswerDlg() {}

	virtual void	Initialize(bool bShow);
	void			SetInfo(const WCHAR* pSenderName, int comboBoxIndex, const WCHAR* pMsg);

protected:
	virtual void	InitialUpdate();
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);

private:
	void			GetComboItem();

	CEtUIStatic*		m_pTitle;
	CEtUITextBox*		m_pTextBox;
	CEtUIButton*		m_pCancelBtn;
};