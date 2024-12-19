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
	// ��ȯâ�� �ø� �����۵�ó�� ����������
	// ���ؽ��Կ� �ø� �����۵鵵 �κ�â���� ��Ȱ��ȭ�Ǿ� ���̵��� ����صд�.
	std::vector<CDnQuickSlotButton*> m_vecQuickSlotButton;

	bool CheckItemDuplication( CDnItem *pItem ) const;
	bool IsEmptySlot();
	int GetEmptySlot();

	// �÷��� �������� ������ ���� ���� ��..
	void CheckItemList() const;

	bool IsDisjoitableItem(CDnSlotButton* pDragButton, CDnItem* pItem) const;

	void Clear(bool bAll);
public:
	void SetItemInfo(int nItemID, int nItemLevel);
	void RemoveItemFromCache(int nItemID, int nItemLevel);

	// �κ����� ��Ŭ�� �ٷε��.
	void AddItemToList( CDnQuickSlotButton *pPressedButton );
	void AddDropResultToList(int nItemID, int nItemLevel, bool bRefreshList);
public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};