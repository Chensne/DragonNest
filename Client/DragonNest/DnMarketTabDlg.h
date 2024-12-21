#pragma once
#include "EtUITabDialog.h"

class CDnMarketBuyDlg;
class CDnMarketSellDlg;
class CDnMarketAccountDlg;
class CDnMarketPriceDlg;
class CDnMarketToolTipDlg;
class CDnMarketPosPetal;
class CDnMarketTabDlg : public CEtUITabDialog, public CEtUICallback
{
public:
	enum {
		MARKET_BUY_DIALOG,
		MARKET_SELL_DIALOG,
		MARKET_ACCOUNT_DIALOG,
	};

public:
	CDnMarketTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketTabDlg(void);

protected:
	CEtUIRadioButton *m_pTabMarketBuy;
	CEtUIRadioButton *m_pTabMarketSell;
	CEtUIRadioButton *m_pTabMarketAccount;

	CEtUIStatic*	m_pPremiumBackGround;
	CEtUIStatic*	m_pPremiumText;

	CDnMarketBuyDlg *m_pMarketBuyDlg;
	CDnMarketSellDlg *m_pMarketSellDlg;
	CDnMarketAccountDlg *m_pMarketAccountDlg;

	CDnMarketToolTipDlg		*m_MarketToolTipDlg;
	CDnMarketPosPetal		*m_MarketPosPetal;

	typedef		CEtUITabDialog		BaseClass;

	SUICoord	m_SaveDlgCoord;
	SUICoord	m_SaveInvenDlgCoord;

	bool		m_bRestoreCoordNeeded;
	float		m_fMagicScale;
	int			m_nPrevTabID;
	
public:
	void InitialUpdate() override;
	void Initialize( bool bShow ) override;
	void Show( bool bShow ) override;
	void Process( float fElapsedTime ) override;
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) override;
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */ ) override; 

public:
	CDnMarketPriceDlg* GetPriceDialog();
	CDnMarketBuyDlg* GetBuyDialog();

	void UpdateBuyList( std::vector< TMarketInfo > &vecBuyList, int nTotalCount );
	void UpdateSellList( std::vector< TMyMarketInfo > &vecSellList );
	void UpdateCalculationList( std::vector< TMarketCalculationInfo > &vecCalculationList );
	void UpdatePetalBalance( int nPetalBalance );
	void UpdateMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice );

	void SetSellCount( short wSellingCount, short wClosingCount, short wWeeklyRegisterCount, short wRemainRegisterItemCount, bool bPremium );

	void RemoveBuyItem( int nMarketDBID );
	void RemoveSellItem( int nMarketDBID, int nRegisterItemCount );
	void RemoveCalculationItem( int nMarketDBID );
	void RemoveAllCalculationItem();

	void EnableChildDlg( bool bEnable );

	void SetMoneyToolTipValue( int nMoney, bool bPetal );

	float GetMagicScale() { return m_fMagicScale; }

	void ShowPremium( bool bShow );
	void ShowPosPetal( bool bShow );
	int  GetPetalBalance();

	void EnableMarketAccountControl( const char *szControlName, bool bEnable );
};
