#pragma once
#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"

class CDnItem;
class CDnCashShopPayItemDlg : public CDnCustomDlg
{
public:
	CDnCashShopPayItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopPayItemDlg(void);

	virtual void	Initialize(bool bShow);
	void			SetInfo(eCashUnitType mode, const SCashShopItemInfo& data, const SCashShopCartItemInfo& cartData);

protected:
	virtual void	InitialUpdate();
	virtual void	InitCustomControl(CEtUIControl *pControl);
	void			ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);


private:
	void			SetCostumeMode();
	void			SetNormalMode();

	CEtUIStatic*		m_pBase;
	CDnItemSlotButton*	m_pSlotBtn;
	CEtUIStatic*		m_pNameStatic;
	CEtUIStatic*		m_pItemSlotBG;

	CEtUIStatic*		m_pPriceStatic;
	CEtUIStatic*		m_pCountStatic;
	CEtUIStatic*		m_pReserveStatic;

	CEtUIComboBox*		m_pAbilityComboBox;
	CEtUIComboBox*		m_pPeriodComboBox;

	CDnItem*			m_pItem;
	int					m_Index;

	eCashUnitType		m_Mode;
};