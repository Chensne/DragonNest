#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_EXCHANGE_ENCHANT

class CDnItemUpgradeExchangeDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum
	{
		NOITEMS_IN_USERSLOT = -1,
		STUFF_ITEMSLOT_MAX = 5,
		SOURCE_SLOT_INDEX = 0,
		TARGET_SLOT_INDEX = 1,
		USERITEM_SLOT_COUNT = 2,
		ITEM_UPGRADE_EXCHANGE_CONFIRM_DIALOG,
		MAX_STUFFITEM = 2,
	};

	enum eERR_ITEM_UPGRADE_EXC
	{
		eEIU_NONE,
		eEIU_GENERAL,
		eEIU_CANT_EXCHANGE_OTHER_CLASS,
		eEIU_DOESNOTMATCH_RANK,
		eEIU_DOESNOTMATCH_LEVELLIMIT,
		eEIU_EMPTY_SOURCE_OR_TARGET_SLOT,
		eEIU_INVALID_SOURCE_OR_TARGET_ITEM,
		eEIU_SAME_OR_HIGH_TARGET_ENCHANT_LEVEL,
	};

	struct SNeedStuffItems
	{
		int itemId;
		int count;

		SNeedStuffItems()
		{
			itemId = ITEMCLSID_NONE;
			count = 0;
		}
	};

	CDnItemUpgradeExchangeDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnItemUpgradeExchangeDlg (void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);

	void AddAttachItem(CDnSlotButton* pFromButton);
	void OnComplete();

private:
	bool AddAttachItem(CDnSlotButton* pFromSlot, CDnItemSlotButton* pAttachSlot);
	bool ReleaseItemSlotBtn(CDnItemSlotButton* pReleaseSlotBtn, int slotIdxIfSlotChange = NOITEMS_IN_USERSLOT);
	void HandleError(eERR_ITEM_UPGRADE_EXC code);

	bool IsFullSourceAndTargetItemSlot() const;
	eERR_ITEM_UPGRADE_EXC IsItemValidExchangeCondition(const CDnItem& source, const CDnItem& target);
	eERR_ITEM_UPGRADE_EXC IsSlotValidExchangeCondition(ITEM_SLOT_TYPE existItemSlotType, const CDnItem& item);

	void GetNeedStuffs(int& nNeedCoin, std::vector<SNeedStuffItems>& needStuffItems, const CDnItem& source) const;
	void SetStuffItemSlot(int slotIndex, int itemId, int count, int curItemCount);
	bool CheckAndSetNeedItemInfo(const std::vector<SNeedStuffItems>& needStuffItems);
	void CheckUpgradeItem();
	bool CheckAttachItemWithOtherItemSlot(const CDnItem* pFromItem, ITEM_SLOT_TYPE type);

	void ClearAllStuffItemSlot();
	void ClearStuffItemSlot(int slotIndex);
	void ClearAllSourceAndTargetSlot();

	CDnItemSlotButton* m_pTargetSlot;
	CDnItemSlotButton* m_pSourceSlot;
	CDnSlotButton* m_pInvenSlotCache[USERITEM_SLOT_COUNT];

	CDnItemSlotButton *m_pStuffItemSlots[STUFF_ITEMSLOT_MAX];

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
};

#endif // PRE_ADD_EXCHANGE_ENCHANT