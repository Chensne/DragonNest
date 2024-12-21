#pragma once
#include "DnCustomDlg.h"
class CDnItem;
class CDnMarketBuyListDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnMarketBuyListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnMarketBuyListDlg(void);

protected:
	CDnItem		*m_pItem;
	int				m_nMarketDBID;
	int				m_nGold;
	int				m_nSilver;
	int				m_nCooper;
	int				m_nOnePrice;
	int				m_nPetalPrice;
	bool			m_bPetalTrade;
	CDnItemSlotButton		*m_pItemSlotButton;

	static std::string m_sUIFileName;

	typedef CDnCustomDlg BaseClass;

public:
	void	Initialize(bool bShow) override;
	void	Show(bool bShow) override;

protected:
	void	InitialUpdate() override;
	void	InitCustomControl( CEtUIControl *pControl ) override;
	void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void Process( float fElapsedTime ) override;	
	
public:
	void ProcessAlpha( float fElapsedTime );
	void SetProperty( TMarketInfo &Info, float fAlpha );
	int GetMarketDBID() { return m_nMarketDBID; }
	wchar_t* GetItemName();
	LPCWSTR GetSellerName()	{ return CONTROL( Static, ID_SELLER )->GetText(); }
	void GetMoney( int &nGold, int &nSilver, int &nCooper ) { nGold = m_nGold; nSilver = m_nSilver; nCooper = m_nCooper; }
	int	 GetPetalPrice()	{ return m_nPetalPrice; }
	int  GetItemSound();
	bool IsPetalTrade()	{ return m_bPetalTrade; }
	void SetPetalUI();
	CDnItem * GetItem()	{ return m_pItem; }

	static void SetUIFileName( std::string sUIFileName )	{ m_sUIFileName = sUIFileName; }
};

