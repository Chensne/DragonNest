#pragma once

class MIInventoryItem;

class CDnInventoryDlg
{
public:
	CDnInventoryDlg(void);
	virtual ~CDnInventoryDlg(void);

protected:
	static int s_nCurrentAmount;

public:
	static void SetSelectAmount( int nValue ) { s_nCurrentAmount = nValue; }
	static int GetCurrentAmount() { return s_nCurrentAmount; }

public:
	virtual void SetItem( MIInventoryItem *pItem ) = 0;
	virtual void ResetSlot( MIInventoryItem *pItem ) = 0;

	virtual void SetUseItemCnt( DWORD dwItemCnt ) = 0;
	virtual void SetUseQuestItemCnt( DWORD dwItemCnt ) {}

	virtual bool IsEmptySlot() { return false; }
	virtual int GetEmptySlot() { return -1; }
};
