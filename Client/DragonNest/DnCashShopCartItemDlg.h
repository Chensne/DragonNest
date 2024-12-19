#pragma once
#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"

class CDnItem;
class CDnItemSlotButton;
class CDnCashShopCartItemDlg : public CDnCustomDlg
{
public:
	CDnCashShopCartItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopCartItemDlg(void);

	virtual void	Initialize(bool bShow);
	void			SetInfo(int itemDlgIdx);

protected:
	virtual void	InitCustomControl(CEtUIControl* pControl);

private:
	void			Clear();

	std::vector<CDnItemSlotButton*>		m_SlotBtnList;
	int									m_Index;
	CDnItem*							m_pItem[_MAX_INVEN_SLOT_PER_LINE];
};