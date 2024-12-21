#pragma once

#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"
#include "DnInterface.h"

class CDnItem;
class CDnWorldServerStorageChargeConfirmDlg : public CDnCustomDlg
{
public:
	CDnWorldServerStorageChargeConfirmDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageChargeConfirmDlg(void);

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual void InitCustomControl(CEtUIControl *pControl);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

	virtual void EnableButtons(bool bEnable);
	virtual void Reset();
	virtual int GetSlotItemCount() const;
	int GetRealItemCount() const;

	void SetItemWithSlotButton(CDnSlotButton* pFromSlot, bool bRenderCountMinusOne);
	virtual bool SetItem(CDnItem* pItem, int slotCount);

	CDnItem* GetItem();
	bool IsFromCashInventory() const;
	bool IsFromCashStorage() const;
	bool IsRenderCountMinusOne() const { return m_bRenderCountMinusOne; }

protected:
	CDnItemSlotButton* m_pItemSlotButton;
	CDnSlotButton* m_pFromSlotBtnCache;

	CEtUIStatic* m_pItemName;

	CEtUIButton* m_pOKButton;
	CEtUIButton* m_pCancelButton;

	bool m_bRenderCountMinusOne;

	CDnSmartMoveCursor m_SmartMove;
};

//////////////////////////////////////////////////////////////////////////

class CDnWorldServerStorageChargeConfirmExDlg : public CDnWorldServerStorageChargeConfirmDlg
{
public:
	CDnWorldServerStorageChargeConfirmExDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageChargeConfirmExDlg()  {}

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();

	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);

	virtual void EnableButtons(bool bEnable);
	virtual void Reset();

	virtual bool SetItem(CDnItem* pItem, int slotCount);
	virtual void Show(bool bShow);

	void UpdateAmount();
	int GetSlotItemCount() const;
	void SetItemCountForce(int count);

protected:
	CEtUIButton* m_pItemUp;
	CEtUIButton* m_pItemDown;

	CEtUIButton* m_pButtonMin;
	CEtUIButton* m_pButtonMax;

	CEtUIEditBox* m_pEditBoxItemCount;
};