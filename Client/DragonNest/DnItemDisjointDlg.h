#pragma once
#include "DnCustomDlg.h"

class CDnItem;
class CDnItemDisjointInfoDlg;
class CDnItemDisjointDlg : public CDnCustomDlg
{
	enum
	{
		ITEM_DISJOINT_ITEMSLOT_MAX = 4,
	};

public:
	CDnItemDisjointDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemDisjointDlg(void);

protected:
	std::vector<CDnItem*> m_vecItem;
	std::vector<CDnItemSlotButton*> m_vecSlotButton;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	CEtUIButton *m_pButtonDisjoint;
	CEtUIButton *m_pButtonCancel;


	std::vector<int> m_pItemCacheList;
	CEtUIListBoxEx *m_pResultListBox;
	// 교환창에 올린 아이템들처럼 마찬가지로
	// 분해슬롯에 올린 아이템들도 인벤창에서 비활성화되어 보이도록 기억해둔다.
	std::vector<CDnQuickSlotButton*> m_vecQuickSlotButton;

	bool CheckItemDuplication( CDnItem *pItem ) const;
	bool IsEmptySlot();
	int GetEmptySlot();

	// 올려진 아이템을 가지고 가격 판정 및..
	void CheckItemList() const;

	bool IsDisjoitableItem(CDnSlotButton* pDragButton, CDnItem* pItem) const;

	void Clear(bool bAll);
public:
	void SetItemInfo(int nItemID, int nItemLevel);
	void RemoveItemFromCache(int nItemID, int nItemLevel);

	// 인벤에서 우클릭 바로등록.
	void AddItemToList( CDnQuickSlotButton *pPressedButton );
	void AddDropResultToList(int nItemID, int nItemLevel, bool bRefreshList);
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};