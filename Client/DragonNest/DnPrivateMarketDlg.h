#pragma once
#include "DnCustomDlg.h"

class CDnMoneyInputDlg;
class CDnItem;
class CDnStoreConfirmExDlg;

class CDnPrivateMarketDlg : public CDnCustomDlg, public CEtUICallback
{
	enum
	{
		MONEY_INPUT_DIALOG,
		PRIVATE_MARKET_ITEMSLOT_MAX = 10,
		ITEM_ATTACH_CONFIRM_DIALOG
	};

public:
	CDnPrivateMarketDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPrivateMarketDlg(void);

protected:
	std::vector<CDnItemSlotButton*> m_vecItemSlotButton_You;

	// 자기아이템의 경우 원래는 인벤에 있는 CDnItem의 포인터를 받아서 썼는데,
	// 이게 창이 꺼지기전에 서버의 DeleteInventory가 호출되면서 위험한 포인터가 되버린다.
	// 지금까지 운좋게 별 문제 없었는데, 실제 릴리즈에서는 아주 가끔씩 뻑나는 원인이 되므로,
	// 이제부턴 인벤토리의 CDnItem 포인터를 받아 쓰는 일은 하지 않겠다.
	//
	// 상대방 아이템의 경우 이미 DnTradePrivateMarket태스크쪽에서 아이템을 올릴때마다 임시로 만들어쓰니
	// 따로 관리하지 않아도 된다.
	std::vector<CDnItem*> m_vecItem_Me;
	std::vector<CDnItemSlotButton*> m_vecItemSlotButton_Me;

	INT64 m_nTradeMoney;
	INT64 m_nTaxMoney;
	CEtUIStatic *m_pStaticGold_You;
	CEtUIStatic *m_pStaticSilver_You;
	CEtUIStatic *m_pStaticBronze_You;
	CEtUIStatic *m_pStaticGold_Me;
	CEtUIStatic *m_pStaticSilver_Me;
	CEtUIStatic *m_pStaticBronze_Me;
	
	CEtUIButton *m_pButtonRegist;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
	CEtUIButton *m_pButtonInputMoney;

	CEtUIStatic *m_pStaticBalloonMe;
	CEtUIStatic *m_pStaticChatMe;
	CEtUIStatic *m_pStaticBalloonYou;
	CEtUIStatic *m_pStaticChatYou;

	CEtUIStatic *m_pStaticNotice;

	CEtUIStatic *m_pStaticMe;
	CEtUIStatic *m_pStaticYou;

	CDnMoneyInputDlg *m_pMoneyInputDlg;

	CEtUIStatic *m_pStaticGold_Tax;
	CEtUIStatic *m_pStaticSilver_Tax;
	CEtUIStatic *m_pStaticBronze_Tax;

	float m_fBalloonMeShowTime;
	float m_fBalloonYouShowTime;

	bool m_bReadyMe;
	bool m_bReadyYou;
	bool m_bComfirm;

	// 상대방 이름 저장해두기
	WCHAR m_strYourName[NAMELENMAX];

	// 교환창에 올린 아이템들이 인벤창에서 비활성화되어 보이도록 기억해둔다.
	std::vector<CDnQuickSlotButton*> m_vecQuickSlotButton;

	CDnStoreConfirmExDlg*	m_pSplitConfirmExDlg;		// Note : 중첩아이템 확인창(개수 입력)

protected:
	void OnResetControl();
	bool CheckItemDuplication( CDnItem *pItem );
	DWORD GetGoldCoinColor( INT64 nGold );
	void SetNotice( const wchar_t *wszNotice, bool bUseTempColor = false, DWORD dwTempColor = 0xFFFFFFFF );
	void DoAddItemToMyTradeList(CDnQuickSlotButton* pFromBtn, int itemCount);
	void RefreshTax();

public:
	void OnUpdateMoney_Other( INT64 nMoney );
	void OnUpdateMoney_One( INT64 nMoney );

	void OnAddItem_Other( int nIndex, CDnItem *pItem );
	void OnDelItem_Other( int nIndex );

	void OnClickButtonRegist_Other();
	void OnClickButtonRegist_One();

	void OnClickButtonConfirm_Other();
	void OnClickButtonConfirm_One();

	//void OnClickButtonCancel_Other();
	//void OnClickButtonCancel_One();

	void AddChatMe( const wchar_t *wszChat );
	void AddChatYou( const wchar_t *wszChat );

	void SetNameMe( const wchar_t *wszName );
	void SetNameYou( const wchar_t *wszName );

	// 상대방 이름은 얻어올 필요가 있다. 근데 이름이 GetNameYou가 뭐냐..
	LPCWSTR GetNameYou() { return m_strYourName; }

	// 우클릭으로 아이템이 교환창으로 보내지도록 처리
	void AddItemToMyTradeList(CDnQuickSlotButton *pPressedButton, bool bItemSplit);
	bool IsTradableItem(CDnSlotButton* pSlotBtn, CDnItem* pItem);
	bool IsEmptySlot();
	int GetEmptySlot();
	CDnSlotButton* GetEmptySlotMe();
	void OnCancelTrade();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual void Process( float fElapsedTime );

private:
	bool CheckReversionItemTradable(const CDnItem* pItem);
};