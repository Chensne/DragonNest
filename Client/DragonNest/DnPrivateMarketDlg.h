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

	// �ڱ�������� ��� ������ �κ��� �ִ� CDnItem�� �����͸� �޾Ƽ� ��µ�,
	// �̰� â�� ���������� ������ DeleteInventory�� ȣ��Ǹ鼭 ������ �����Ͱ� �ǹ�����.
	// ���ݱ��� ������ �� ���� �����µ�, ���� ��������� ���� ������ ������ ������ �ǹǷ�,
	// �������� �κ��丮�� CDnItem �����͸� �޾� ���� ���� ���� �ʰڴ�.
	//
	// ���� �������� ��� �̹� DnTradePrivateMarket�½�ũ�ʿ��� �������� �ø������� �ӽ÷� ������
	// ���� �������� �ʾƵ� �ȴ�.
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

	// ���� �̸� �����صα�
	WCHAR m_strYourName[NAMELENMAX];

	// ��ȯâ�� �ø� �����۵��� �κ�â���� ��Ȱ��ȭ�Ǿ� ���̵��� ����صд�.
	std::vector<CDnQuickSlotButton*> m_vecQuickSlotButton;

	CDnStoreConfirmExDlg*	m_pSplitConfirmExDlg;		// Note : ��ø������ Ȯ��â(���� �Է�)

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

	// ���� �̸��� ���� �ʿ䰡 �ִ�. �ٵ� �̸��� GetNameYou�� ����..
	LPCWSTR GetNameYou() { return m_strYourName; }

	// ��Ŭ������ �������� ��ȯâ���� ���������� ó��
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