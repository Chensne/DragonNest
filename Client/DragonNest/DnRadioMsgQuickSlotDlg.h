#pragma once

#include "DnCustomDlg.h"
#include "DnCustomControlCommon.h"

class CDnSlotButton;
class CDnRadioMsgButton;
class MIInventoryItem;

// 라디오메세지 퀵슬롯 다이얼로그
class CDnRadioMsgQuickSlotDlg : public CDnCustomDlg
{
public:
	CDnRadioMsgQuickSlotDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRadioMsgQuickSlotDlg(void);

protected:
	std::vector< CDnRadioMsgButton* > m_vecSlotButton;

protected:
	bool SetSlot( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	void ResetSlot( int nIndex );
	int FindRadioMsgSlotItem( int nRadioMsgID );

	// 기본 퀵슬롯과 달리, 다이얼로그가 하나뿐이라, MainDlg에 안만들고 직접 처리할 수 있다.
	void ChangeQuickSlotButton( CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton );
	bool SetSlot( int nIndex, int nRadioMsgID );

	// 스킬을 슬롯에 넣을때 소리, 집을때 소리
	int m_nSkillLaydownSoundIndex;
	int m_nSkillDragSoundIndex;

public:
	bool InitSlot( int nIndex, int nRadioMsgID );

public:
	virtual void Initialize(bool bShow);
	virtual void InitCustomControl( CEtUIControl *pControl );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};