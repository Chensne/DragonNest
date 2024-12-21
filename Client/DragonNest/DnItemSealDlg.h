#pragma once
#include "DnCustomDlg.h"

class CDnItem;

class CDnItemSealDlg : public CDnCustomDlg
{
public:
	CDnItemSealDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemSealDlg(void);

protected:
	CDnItem *m_pItem;
	CDnItemSlotButton *m_pItemSlotButton;
	CDnQuickSlotButton *m_pQuickSlotButton;

	CEtUIStatic *m_pStaticSeal;
	CEtUIStatic *m_pStaticRandomSeal;
	CEtUIStatic *m_pStaticItemName;
	CEtUIStatic *m_pStaticTextAccount;
	CEtUIButton *m_pButtonDetail;
	CEtUIButton *m_pButtonOK;

	bool IsSealableItem( CDnSlotButton *pDragButton );
	void CheckSealItem();
	int m_nTotalSealNeeds;
	int m_nSealSoundIdx;

	int m_nSealTypeParam;
	std::wstring m_wszSealItemName;
	INT64 m_biItemSerial;

public:

	void SetSealItem( CDnItem *pSealItem );
	void OnRecvSealItem( int nResult, int nSlotIndex );

	// 우클릭 등록
	void SetSealItem( CDnQuickSlotButton *pPressedButton );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
