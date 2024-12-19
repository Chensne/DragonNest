#pragma once

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnCustomDlg.h"
#include "DnInven.h"
#include "DnInventoryDlg.h"
#include "DnUIDefine.h"

class CDnMoneyInputDlg;
class CDnQuickSlotButton;
class CDnItemSlotButton;
class CDnItem;
class CDnWorldServerStorageDlg;
class CDnWorldServerStorageConfirmDlg;
class CDnWorldServerStorageConfirmExDlg;
class CDnWorldServerStorageChargeConfirmDlg;
class CDnWorldServerStorageChargeConfirmExDlg;
class CDnWorldServerStorageDrawConfirmDlg;
class CDnWorldServerStorageDrawConfirmExDlg;
class CDnWorldServerStorageInventoryDlg : public CDnCustomDlg, public CEtUICallback, public CDnInventoryDlg
{
	enum
	{
		FREE_CONFIRM_DLG,
		FREE_CONFIRM_EX_DLG,
		CHARGE_CONFIRM_DLG,
		CHARGE_CONFIRM_EX_DLG,
		DRAW_CONFIRM_DLG,
		DRAW_CONFIRM_EX_DLG,
	};

public:
	CDnWorldServerStorageInventoryDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageInventoryDlg();

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Process(float fElapsedTime);
	virtual void OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0);

	virtual void SetItem(MIInventoryItem* pItem);
	virtual void ResetSlot(MIInventoryItem* pItem);
	virtual void SetUseItemCnt(DWORD dwItemCnt);

	void SetChargeType(Storage::WorldServerStorage::eChargeType type) { m_ChargeType = type; }
	const Storage::WorldServerStorage::eChargeType& GetChargeType() const { return m_ChargeType; }

	const Storage::WorldServerStorage::eInvenType& GetInvenType() const { return m_InvenType; }

	bool IsEmptySlot() const;
	int GetEmptySlot() const;

	void UpdateCoin();

	void MoveItemToStorage(CDnQuickSlotButton* pFromSlot, bool bForceMove);
	void MoveItemToInventory(CDnItemSlotButton* pFromSlot, UINT uMsg);

	void RequestMoveItem(eItemMoveType moveType, CDnItem* pItem, int itemCount);


protected:
	virtual void OnInitialUpdate();
	virtual void SortInventory();

	CDnWorldServerStorageDlg* m_pWorldServerStorageDlg;
	Storage::WorldServerStorage::eInvenType m_InvenType;

private:
	bool HasEnoughMoney() const;
	bool HasCoinItem(int nReqCount) const;	
	template <typename T>
	void OnConfirmUIMsg(int nID, T* pConfirmDlg, bool bForceOk = false);

	CEtUIStatic* m_pCountStatic;
	CEtUIStatic* m_pNoticeStatic;
	CDnItemSlotButton* m_pChargeItemSlot;
	CEtUIStatic* m_pChargeItemSlotStatic;
	CEtUIStatic* m_pNoticeOnNoChargeStatic;
	CDnItem* m_pChargeItem;

	Storage::WorldServerStorage::eChargeType m_ChargeType;

	CDnWorldServerStorageConfirmDlg* m_pConfirmDlg;
	CDnWorldServerStorageConfirmExDlg* m_pSplitConfirmDlg;

	CDnWorldServerStorageChargeConfirmDlg* m_pChargeConfirmDlg;
	CDnWorldServerStorageChargeConfirmExDlg* m_pChargeSplitConfirmDlg;

	CDnWorldServerStorageDrawConfirmDlg* m_pDrawConfirmDlg;
	CDnWorldServerStorageDrawConfirmExDlg* m_pDrawSplitConfirmDlg;
};


//////////////////////////////////////////////////////////////////////////

class CDnWorldServerStorageCashInventoryDlg : public CDnWorldServerStorageInventoryDlg
{
public:
	CDnWorldServerStorageCashInventoryDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);

	virtual void Show(bool bShow);

protected:
	virtual void OnInitialUpdate();
	virtual void SortInventory();
};


#endif // PRE_ADD_ACCOUNT_STORAGE