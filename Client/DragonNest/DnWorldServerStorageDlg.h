#pragma once

#include "EtUITabDialog.h"
#include "DnCustomControlCommon.h"

#ifdef PRE_ADD_SERVER_WAREHOUSE

class MIInventoryItem;
class CDnWorldServerStorageDlg : public CEtUITabDialog
{
public:
	CDnWorldServerStorageDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnWorldServerStorageDlg();

protected:
	std::vector<CEtUIRadioButton*> m_vecTabButton;

public:
	void SetItem(MIInventoryItem *pItem);
	void ResetSlot(int nSlotIndex);
	void SetSlotType(ITEM_SLOT_TYPE type);

	void SetUseItemCnt(DWORD dwItemCnt);

	bool IsEmptySlot();
	int GetEmptySlot();

	void PrevPage();
	void NextPage();

	virtual void Initialize(bool bShow);
	virtual void InitialUpdate();
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // PRE_ADD_ACCOUNT_STORAGE