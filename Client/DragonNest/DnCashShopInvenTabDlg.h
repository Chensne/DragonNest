#pragma once

#include "EtUIDialog.h"

class CDnCashShopInvenTabDlg : public CEtUIDialog
{
public:
	CDnCashShopInvenTabDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopInvenTabDlg(void);

	virtual void	Initialize(bool bShow);
	virtual void	Show(bool bShow);
	virtual void	Render(float fElapsedTime);

protected:
	virtual void	ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void	InitialUpdate();

	void	SetPositionCashShopInven( bool bCashShop );
	UIAllignHoriType	m_OrigHoriType;
	UIAllignVertType	m_OrigVertType;
};