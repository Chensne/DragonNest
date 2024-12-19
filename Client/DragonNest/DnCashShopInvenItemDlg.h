#pragma once
#include "DnCustomDlg.h"
#include "DnCashShopDefine.h"

#ifdef _CASHSHOP_UI

class CDnItemSlotButton;
class CDnCashShopInvenItemDlg : public CDnCustomDlg
{
public:
	CDnCashShopInvenItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCashShopInvenItemDlg(void);

	virtual void	Initialize(bool bShow);
	void			SetSlotIndex(int itemDlgIdx);

protected:
	virtual void	InitCustomControl(CEtUIControl* pControl);

private:
	std::vector<CDnItemSlotButton*>		m_SlotBtnList;
	int									m_Index;
};

#endif // _CASHSHOP_UI