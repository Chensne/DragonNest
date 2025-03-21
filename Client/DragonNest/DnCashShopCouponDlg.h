#pragma once

#include "EtUIDialog.h"
#include "DnCashShopDefine.h"

class CDnCashShopCouponDlg : public CEtUIDialog
{
public:
	CDnCashShopCouponDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopCouponDlg();

	virtual void		Initialize(bool bShow);
	virtual void		Show(bool bShow);

protected:
	virtual void		InitialUpdate();
	virtual void		ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void		Process(float fElapsedTime);
	virtual bool		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

private:
	void				SendCouponNumber();

	CEtUIButton*					m_pOkBtn;
	CEtUIButton*					m_pCancelBtn;
	CEtUIIMEEditBox*				m_pNumberInputEditBox;
	CEtUIStatic*					m_pNumberStatic;
};
