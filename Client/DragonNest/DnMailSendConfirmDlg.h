#pragma once

#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

class CDnMailSendConfirmDlg : public CEtUIDialog
{
public:
	CDnMailSendConfirmDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallBack = NULL);
	virtual ~CDnMailSendConfirmDlg() {}

	virtual void	Initialize(bool bShow);
	virtual void	InitialUpdate();

	void			Show(bool bShow, bool bPremium, const std::wstring& receiverName);
	virtual void	Show(bool bShow);
	void			SetTax(const TAX_TYPE& amount);

private:
	CEtUIStatic*	m_pStaticConfirm;
	CEtUIStatic*	m_pStaticQuick;

	CEtUIStatic*	m_pCoinG;
	CEtUIStatic*	m_pCoinS;
	CEtUIStatic*	m_pCoinB;

	CEtUIButton*	m_pButtonSend;
	CEtUIButton*	m_pButtonCancel;

	CDnSmartMoveCursor m_SmartMove;
};
