#pragma once
#include "DnCustomDlg.h"

#ifdef PRE_ADD_EXCHANGE_POTENTIAL

class CDnItem;
class CDnPotentialTransferInfoDlg;
class CDnPotentialTransferDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPotentialTransferDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialTransferDlg();

	enum
	{
		POTENTIAL_EXTRACTOR_ITEMID = 1073749635,
	};

protected:
	CDnItem *m_pItem1;
	CDnItemSlotButton *m_pItemSlot1;
	CDnQuickSlotButton *m_pButtonQuickSlot1;
	CDnItem *m_pItem2;
	CDnItemSlotButton *m_pItemSlot2;
	CDnQuickSlotButton *m_pButtonQuickSlot2;

	CDnItem *m_pTransItem;
	CDnItemSlotButton *m_pTransItemSlot;

	CEtUIStatic *m_pStaticText;
	CEtUIButton *m_pButtonInfo;
	CEtUIStatic *m_pStaticCount;
	CEtUIButton *m_pButtonDetail;
	CEtUIButton *m_pButtonApply;
	DWORD m_dwCountColor;
	int m_nSoundIdx;

	int m_nStep;

	CDnPotentialTransferInfoDlg *m_pInfoDlg;

	void OnChangeStep( int nStep );
	bool IsPotentialExtractItem( CDnSlotButton *pDragButton );
	bool IsPotentialableItem( CDnSlotButton *pDragButton );
	void RefreshPotentialTransfer();
	int GetPotentialTransferItemCount( int nMainType, int nSubType, int nItemLevel1, int nItemRank1, int nItemLevel2, int nItemRank2 );

public:
	void OnRButtonClick( CDnQuickSlotButton *pPressedButton );
	void OnRecvExchangePotential( int nResult );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};

#endif