#pragma once

#include "DnCustomDlg.h"
#include "DnCustomControlCommon.h"

class CDnSlotButton;
class CDnQuickSlotButton;
class MIInventoryItem;

class CDnGuildWarSkillSlotDlg : public CDnCustomDlg
{
public:
	CDnGuildWarSkillSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarSkillSlotDlg();

protected:
	std::vector< CDnQuickSlotButton* > m_vecSlotButton;

protected:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	void ResetSlot( int nIndex );
	int FindQuickSlotItem( MIInventoryItem *pItem );

	// 스킬을 슬롯에 넣을때 소리, 집을때 소리
	int m_nSkillLaydownSoundIndex;
	int m_nSkillDragSoundIndex;

	bool m_bFixedMode;

public:
	bool InitSlot( int nIndex, MIInventoryItem *pItem );
	bool SetSlot( int nIndex, MIInventoryItem *pItem );
	void SwapQuickSlot( int nFromIndex, int nToIndex );
	int FindEmptyQuickSlotIndex();

	void ResetAllSlot();
	void OnRefreshSlot();
	void EnableQuickSlot(bool bEnable);

	void UseGuildWarSkill( int nIndex );
	void SetFixedMode( bool bFixedMode ) { m_bFixedMode = bFixedMode; }

public:
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
};
