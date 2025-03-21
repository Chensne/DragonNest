#pragma once

#include "EtUIDialog.h"

class CDnCashShopGiftReplyDlg : public CEtUIDialog
{
	enum
	{
		MESSAGEBOX_GIFT_CONFIRM,
	};

public:
	CDnCashShopGiftReplyDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopGiftReplyDlg() {}

	virtual void	Initialize(bool bShow);
	void			SetInfo(const WCHAR* pSenderName);

protected:
	virtual void	InitialUpdate();
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);

private:
	void			AddComboItem();

	CEtUIStatic*			m_pTitle;
	CEtUIComboBox*			m_pImpressionComboBox;
	CEtUILineIMEEditBox*	m_pReplyEditBox;

	CEtUIButton*			m_pOkBtn;
	CEtUIButton*			m_pCancelBtn;
};
