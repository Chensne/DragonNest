#pragma once
#include "EtUITabDialog.h"
#include "DnInven.h"
#include "DnCustomControlCommon.h"

class MIInventoryItem;
class CEtUIRadioButton;

class CDnInvenDlg : public CEtUITabDialog
{
public:
	CDnInvenDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnInvenDlg(void);

protected:
	std::vector<CEtUIRadioButton*> m_vecTabButton;
#ifdef PRE_ADD_INVEN_EXTENSION
	ITEM_SLOT_TYPE m_SlotType;
#endif

#if defined(PRE_ADD_AUTOUNPACK) || defined(PRE_ADD_CASHREMOVE)
	std::vector<class CDnInvenSlotDlg *> m_pVecInvenSlotDlg; // 인벤에 속한 모든 슬롯창 - 자동열기에 등록한 아이템이 인벤에 존재하면 모두가져오기위함.
#endif

public:
	void SetItem( MIInventoryItem *pItem );
	void ResetSlot( int nSlotIndex );
	void SetSlotType( ITEM_SLOT_TYPE slotType );

	void SetUseItemCnt( DWORD dwItemCnt );

	bool IsEmptySlot();
	int GetEmptySlot();
	int GetEmptySlotCount();

	void ShowPage( int nPageNum, bool bShow );

	void ReleaseNewGain();
	int GetRegisteredItemCount();

	void DisablePressedButtonSplitMode();

	void PrevPage();
	void NextPage();

#if defined( PRE_PERIOD_INVENTORY )
	void EnablePeriodInven( const bool bEnable, const __time64_t tTime = 0 );
	void SetPeriodInvenTooltip( const bool bEnable, const __time64_t tTime );
	bool IsPeriodIvenEnable();
#endif	//	#if defined( PRE_PERIOD_INVENTORY )

#ifdef PRE_ADD_AUTOUNPACK
	std::vector<CDnInvenSlotDlg *>& GetInvenSlotDlgList() { return m_pVecInvenSlotDlg; }
#endif


public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
