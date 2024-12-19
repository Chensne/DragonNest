#pragma once
#include "DnCustomDlg.h"
#include "DnMoneyInputDlg.h"
#include "DnSlotButton.h"

class CDnItem;
class CDnItemSlotButton;
class CDnQuickSlotButton;

class CDnStoreConfirmExDlg;

class CDnMarketPriceDlg : public CDnCustomDlg, public CEtUICallback
{
	enum {
		ITEM_ATTACH_CONFIRM_DIALOG,
	};
	enum {
		MESSAGEBOX_REGIST_ITEM = 4053
	};

public:
	CDnMarketPriceDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketPriceDlg(void);

protected:
	CDnMoneyControl		m_MoneyControl;

	CDnItem				*m_pItem;
	CDnSlotButton		*m_pSlotButton;

	bool				m_bPremium;

	TAX_TYPE			m_nRegistTax;

	CDnStoreConfirmExDlg*		m_pSplitConfirmExDlg;

	D3DCOLOR			m_TaxColor[ 3 ];

	typedef	CDnCustomDlg		BaseClass;

private:
	void CancelMarketItem();
	void CalcRegistrationFee();
	void ProcessRegister();
	void RequestMarketPrice();

public:
	bool PickUpMarketItem( CDnQuickSlotButton *pPressedButton, bool bSplit = false);
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg ) override;

	void SetPremium( bool bPremium );

public:
	void InitialUpdate() override;
	void Initialize( bool bShow ) override;
	void Show( bool bShow ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void Process( float fElapsedTime ) override;

	void OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice );
	void ResetMarketPrice();
};
