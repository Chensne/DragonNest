#pragma once
#include "EtUITabDialog.h"

class MIInventoryItem;

class CDnStorageDlg : public CEtUITabDialog
{
public:
	CDnStorageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnStorageDlg(void);

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

#if defined( PRE_PERIOD_INVENTORY )
	void EnablePeriodStorage( const bool bEnable, const __time64_t tTime = 0 );
	void SetPeriodStorageTooltip( const bool bEnable, const __time64_t tTime );
	bool IsPeriodStorageEnable();
#endif	//	#if defined( PRE_PERIOD_INVENTORY )

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
