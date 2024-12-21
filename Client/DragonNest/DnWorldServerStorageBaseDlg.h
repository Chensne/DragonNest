#pragma once

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnCustomDlg.h"
#include "DnInven.h"
#include "DnInventoryDlg.h"

class CDnMoneyInputDlg;
class CDnQuickSlotButton;
class CDnItemSlotButton;
class CDnItem;
class CDnStoreConfirmExDlg;
class CDnWorldServerStorageBaseDlg : public CDnCustomDlg, public CEtUICallback, public CDnInventoryDlg
{
	enum emMONEY_CONFIRM_TYPE
	{
		MONEY_CONFIRM_IN,
		MONEY_CONFIRM_OUT,
	};

	enum
	{
		MONEY_INPUT_DIALOG,
		ITEM_ATTACH_CONFIRM_DIALOG
	};

public:
	CDnWorldServerStorageBaseDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageBaseDlg();

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Process(float fElapsedTime);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0);

	virtual void SetItem(MIInventoryItem* pItem);
	virtual void ResetSlot(MIInventoryItem* pItem);
	virtual void SetUseItemCnt(DWORD dwItemCnt);

	bool IsEmptySlot() const;
	int GetEmptySlot() const;

	void MoveItemToStorage(const CDnQuickSlotButton* pFromSlot, bool bItemSplit);
	void RequestMoveItem(eItemMoveType moveType, const CDnItem* pItem, int itemCount);

private:
	CEtUIStatic* m_pCountStatic;
	CEtUIStatic* m_pNoticeStatic;
	CDnItemSlotButton* m_pChargeItemSlot;
	CEtUIStatic* m_pChargeItemSlotStatic;
	CEtUIStatic* m_pNoticeOnNoChargeStatic;

	CDnStoreConfirmExDlg* m_pSplitConfirmExDlg;
};

#endif // PRE_ADD_ACCOUNT_STORAGE