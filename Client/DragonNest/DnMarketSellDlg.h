#pragma once
#include "DnCustomDlg.h"

class CDnMarketPriceDlg;
class CDnItem;
class CDnMarketListDlg;

class CDnMarketSellDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnMarketSellDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMarketSellDlg(void);

protected:
	struct MySellItem
	{
		std::wstring wszItemName;
		TMyMarketInfo MyMarket;
	};
	enum eSortType {
		ST_NONE = -1,
		ST_NAME,
		ST_PRICE,
		ST_REMAINTIME,		
	};

	enum {
		MESSAGEBOX_RECALL_ITEM = 4054,
	};

	enum eCount {		
		MAX_ITEM_COUNT_FOR_PAGE = 5,
		PAGE_COUNT_FOR_SCREEN = 10,	// 불변 예정
	};

	CDnMarketPriceDlg						*m_pMarketPriceDlg;
	std::vector< MySellItem >					m_vecSellList;

	short m_wSellingCount;
	short m_wClosingCount;
	short m_wWeeklyRegisterCount;
	short m_wRemainRegisterItemCount;

	eSortType	m_lastSortType;
	bool			m_lastAscending;


	struct SellListSorter {
		eSortType m_sortType;
		bool	m_bAscending;

		enum {
			MAGIC_LARGE_NUMBER = 1000000,
		};

		SellListSorter( eSortType sortType, bool bAscending ) : m_sortType( sortType ), m_bAscending( bAscending )  { ; }

		bool operator () ( MySellItem &lhs, MySellItem &rhs )		// implementation of Less Operator
		{
			bool bLess = false;
			switch( m_sortType )
			{
			case ST_NAME:
				bLess =  ( lhs.wszItemName < rhs.wszItemName ) ? m_bAscending : !m_bAscending;
				break;
			case ST_PRICE:
				bLess = ( lhs.MyMarket.nItemPrice < rhs.MyMarket.nItemPrice ) ? m_bAscending : !m_bAscending;
				break;
			case ST_REMAINTIME:
				bLess = ( lhs.MyMarket.nRemainTime + lhs.MyMarket.cSellType * MAGIC_LARGE_NUMBER  <				// 팔린아템은 마지막으로 오도록..
					rhs.MyMarket.nRemainTime + rhs.MyMarket.cSellType * MAGIC_LARGE_NUMBER ) ? m_bAscending : !m_bAscending;
				break;
			}
			return bLess;
		}
	};

	int		m_nPage;

	bool m_bRefresh;
	bool m_bPremium;
	vector<int>	 m_vPremiumEnableNumList;

	bool m_bRequestListNeeded;	
	int		m_nRecallDBID;
	std::set< int > m_IsNewPage;
	typedef CDnCustomDlg	BaseClass;

	CEtUIButton * m_pButtonAllRecall;
	bool m_bAllRecall;
	bool m_bEnableRecallRequest;
	int	 m_nRecallCount;

public:
	CDnMarketPriceDlg* GetPriceDialog() { return m_pMarketPriceDlg; }

private:
	void UpdateListItems();
	void RefreshCount();
	void RefreshPageControls();
	int GetTotalPage();
	int GetStartPage();
	void ProcessRecall();
	void ProcessAllRecall();
	void SearchPremium();

public:
	void Initialize( bool bShow ) override;
	void InitialUpdate() override;
	void InitCustomControl( CEtUIControl *pControl ) override;
	void Show( bool bShow ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void Process( float fElapsedTime ) override;
	
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 ) override;

public:
	void OnRecvUpdateList( std::vector< TMyMarketInfo > &vecSellList );
	void OnRecvRemoveItem( int nMarketDBID, int nRegisterItemCount );
	void SetCount( short wSellingCount, short wClosingCount, short wRegisteredCountPerDay, short wCurrentRegisterEnableCount, bool bPremium );
	void RefreshRequestNeed(); 
	void EnableListBoxControl( bool bEnable );
	void OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice );
};
