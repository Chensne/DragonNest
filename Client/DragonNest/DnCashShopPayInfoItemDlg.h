#pragma once
#include "EtUIDialog.h"
#include "DnCashShopDefine.h"

class CDnCashShopPayInfoItemDlg : public CEtUIDialog
{
public:
	CDnCashShopPayInfoItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopPayInfoItemDlg(void);

	virtual void	Initialize(bool bShow);
	virtual void	Show(bool bShow);

	void			SetInfo(const SCashShopItemInfo& data, const SCashShopCartItemInfo& cartInfo);
	CASHITEM_SN		GetSN() const { return m_SN; }

protected:
	virtual void	InitialUpdate();

private:
	CEtUIStatic*		m_pNameStatic;
	CEtUIStatic*		m_pPriceStatic;
	CEtUIStatic*		m_pInfoStatic;
	CASHITEM_SN			m_SN;
};