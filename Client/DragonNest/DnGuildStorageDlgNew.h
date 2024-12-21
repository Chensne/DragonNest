#pragma once

#include "EtUIDialog.h"
#include "DnInven.h"
#include "DnInventoryDlg.h"

class CDnGuildStorageDlg;
class CDnMoneyInputDlg;
class CDnGuildMoneyInputDlg;
class CDnStoreConfirmExDlg;
class CDnGuildStorageHistoryDlg;
class CDnQuickSlotButton;
class CDnItem;

// 길드창고에서는 일반아이템만 저장 가능하기때문에 탭이 없다.
class CDnGuildStorageDlgNew : public CEtUIDialog, public CEtUICallback, public CDnInventoryDlg
{
public:
	CDnGuildStorageDlgNew( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildStorageDlgNew(void);

protected:
	CDnGuildStorageDlg *m_pGuildStorageDlg;

	CEtUIStatic *m_pStaticGold;
	CEtUIStatic *m_pStaticSilver;
	CEtUIStatic *m_pStaticBronze;

	CEtUIButton *m_pButtonMoneyIn;	// 입금
	CEtUIButton *m_pButtonMoneyOut;	// 출금

	CEtUIStatic *m_pStaticCount;
	CEtUIButton *m_pButtonHistory;

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

	emMONEY_CONFIRM_TYPE m_emMoneyType;

	CDnMoneyInputDlg *m_pMoneyInputDlg;
	CDnGuildMoneyInputDlg *m_pGuildMoneyInputDlg;
	CDnStoreConfirmExDlg *m_pSplitConfirmExDlg;
	CDnGuildStorageHistoryDlg *m_pHistoryDlg;

	int m_nItemCnt;

public:
	virtual void SetItem( MIInventoryItem *pItem );
	virtual void ResetSlot( MIInventoryItem *pItem );

	virtual void SetUseItemCnt( DWORD dwItemCnt );
	int GetUseItemCnt() { return m_nItemCnt; }

	virtual bool IsEmptySlot();
	virtual int GetEmptySlot();

	void MoveItemToStorage(CDnQuickSlotButton* pFromSlot, bool bItemSplit);
	void RequestMoveItem(eItemMoveType moveType, CDnItem* pItem, int itemCount);

	void OnRecvGetGuildStorageHistoryList( SCGetGuildWareHistory *pPacket );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};

