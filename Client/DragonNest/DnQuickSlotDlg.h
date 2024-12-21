#pragma once

#include "DnCustomDlg.h"
#include "DnCustomControlCommon.h"

class CDnSlotButton;
class CDnQuickSlotButton;
class MIInventoryItem;

class CDnQuickSlotDlg : public CDnCustomDlg
{
public:
	CDnQuickSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnQuickSlotDlg(void);

protected:
	std::vector< CDnQuickSlotButton* > m_vecSlotButton;

protected:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	void ResetSlot( int nIndex );
	int FindQuickSlotItem( MIInventoryItem *pItem );

	// 스킬을 슬롯에 넣을때 소리, 집을때 소리
	int m_nSkillLaydownSoundIndex;
	int m_nSkillDragSoundIndex;
	

public:
	bool InitSlot( int nIndex, MIInventoryItem *pItem );
	bool SetSlot( int nIndex, MIInventoryItem *pItem );
	bool SetEventSlot( MIInventoryItem *pItem ); // 이벤트 슬롯에 등록 <클라에서만 기억하는 소모성 슬롯>
	int FindEmptyQuickSlotIndex();
	int FindSkillQuickSlotIndex( int nSkillID );
	int FindSkillQuickSlotIndexAndReplace(int nSkillID, int nSkillIDReplace, bool bRevert=false); //rlkt_add

	void ResetAllSlot();
	void OnRefreshSlot();
	void EnableQuickSlot(bool bEnable);
	void GetItemList(std::vector<int> &vecList); // 퀵슬롯에 등록된 아이템을 얻어온다.
	void ResetSlotID(int nSlotID) { this->ResetSlot(nSlotID); };
public:
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
};