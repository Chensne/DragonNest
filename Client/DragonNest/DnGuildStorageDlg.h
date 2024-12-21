#pragma once
#include "EtUITabDialog.h"

class MIInventoryItem;

class CDnGuildStorageDlg : public CEtUITabDialog
{
public:
	CDnGuildStorageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildStorageDlg(void);

protected:
	std::vector<CEtUIRadioButton*> m_vecTabButton;

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetSlot( int nSlotIndex );
	void SetUseItemCnt( DWORD dwItemCnt );

	bool IsEmptySlot();
	int GetEmptySlot();

	void PrevPage();
	void NextPage();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};