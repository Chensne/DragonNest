#pragma once
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;
class CDnItemSlotButton;

class CDnItemCompoundMixDlg : public CDnCustomDlg
{
public:
	CDnItemCompoundMixDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnItemCompoundMixDlg(void);

	enum
	{
		NUM_NEED_ITEM = 5,
	};

protected:
	CEtUIStatic *m_pGold;
	CEtUIStatic *m_pSilver;
	CEtUIStatic *m_pBronze;
	DWORD m_dwColorGold;
	DWORD m_dwColorSilver;
	DWORD m_dwColorBronze;

	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;

	CEtUIStatic *m_pProb;

	// 재료템 슬롯
	CDnItem *m_pItem[NUM_NEED_ITEM];
	CDnItemSlotButton *m_pItemSlotButton[NUM_NEED_ITEM];

	// 슬롯 설정
	void SetItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount );

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

protected:

public:
	// 확률 설정
	void SetProb( int nProb );

	// 가격 설정
	void SetPrice( int nPrice );

	// 재료 리스트 설정
	void SetCompoundInfo( int *pItemID, int *pItemCount );

	// OK 버튼 설정
	void SetOK( bool bOK );

	void EnableButton( bool bEnable );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};