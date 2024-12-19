#pragma once
#include "EtUIDialog.h"
#include "DnCustomDlg.h"

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
class CDnMarketToolTipDlg;
class CDnMarketDirectBuyDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	enum 
	{
		SLOT_MAX = 5 ,
		LIST_MAX = 50,
		MESSAGEBOX_DIRECT_BUY_ITEM = 5001,
	};

	CDnMarketDirectBuyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMarketDirectBuyDlg();

	virtual void	Initialize( bool bShow );
	virtual void	InitialUpdate();
	virtual void	ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void	OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	virtual bool	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void	Show( bool bShow );
	
	void			ResetData();
	void			ResetUI();
	void			SendOpenMarketPacket(int nitemID);
	void			RecvOpenMarketPacket(SCMarketMiniList* pPacket);
	int				ComputeMaxPage(int nVectorSize);
	const CDnItem*	GetPrevSelectItem()  { return m_pPrevSelectedItem; }
protected:
	CEtUIButton* m_pItemBuyOKButton;
	CEtUIButton* m_pItemBuyCancelButton;

	CEtUIButton* m_pPrevButton;
	CEtUIButton* m_pNextButton;
	CEtUIStatic* m_pStaticPageNumber;
	
	CEtUIStatic* m_pStaticPremiumItem[SLOT_MAX];
	CEtUIStatic* m_pStaticSelect[SLOT_MAX];
	CDnItemSlotButton* m_pItemSlotButton[SLOT_MAX];
	CEtUIStatic* m_pStaticItemName[SLOT_MAX];

	CEtUIStatic* m_pStaticGold[SLOT_MAX];
	CEtUIStatic* m_pStaticSilver[SLOT_MAX];
	CEtUIStatic* m_pStaticBronze[SLOT_MAX];
	CEtUIStatic* m_pStaticToolTipArea[SLOT_MAX];
	CEtUIButton* m_pButtonReScan;
	CEtUIStatic* m_pStaticMoneyBackBase[SLOT_MAX];
	int			 m_nMoneyBackBaseName[SLOT_MAX];

#ifdef PRE_ADD_PETALTRADE
	CEtUIStatic*	m_pStaticPetal[SLOT_MAX];
	CEtUIStatic*	m_pStaticPetalBase[SLOT_MAX];
	CEtUICheckBox*	m_pCheckBoxGold;
	CEtUICheckBox*	m_pCheckBoxPetal;
	CEtUIStatic*	m_pStaticPetalMessage;
	CEtUIStatic*	m_pStaticMyPetal;
	CEtUIButton*	m_pButtonSearch;
	int				m_nCurrentMyPetal;
#endif //PRE_ADD_PETALTRADE
	float			m_fElapseTime;
	bool			m_bIsClickSearchButton;

	int			m_nMaxPageNumber;
	int			m_nCurrentPageNumber;
	int			m_nSelectedItemIndex;
	int			m_nBuyItemDBID;
	CDnItem*	m_pPrevSelectedItem;
	std::vector<TMarketInfo> m_vecDirectMarketList;

	CDnMarketToolTipDlg*	m_pItemPriceTooltip;
	int						m_nOnePrice;
	bool					m_bPetalItem;
	int						m_nMouseOverIndex;

protected:
	void PrevPage();
	void NextPage();
	void SetCurrentItemPage(int nPageNum);
	void Process(float fElapsedTime) override;
	void ProcessItemBuy();
	void RefreshItemPage();
#ifdef PRE_ADD_PETALTRADE
	void ReSearchMarketItem();
#endif

public:
#ifdef PRE_ADD_PETALTRADE
	void SetMyPetal(int nPetal) { m_nCurrentMyPetal = nPetal; }
#endif

};

#endif // PRE_ADD_DIRECT_BUY_UPGRADEITEM