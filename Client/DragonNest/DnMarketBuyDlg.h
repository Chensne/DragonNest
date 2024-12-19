#pragma once
#include "EtUIDialog.h"
class CDnItem;
class CDnMarketBuyListDlg;
class CDnMarketPriceTooltipDlg;

class CDnMarketBuyDlg : public CEtUIDialog, public CEtUICallback
{
public:
	enum eJob {
		JOB_NONE,
		JOB_WARRIER = 1,
		JOB_ARCHER,
		JOB_SOCERESS,
		JOB_CLERIC,

		JOB_SWORDMASTER = 11,
		JOB_MERCENARY = 12,
		JOB_BOWMASTER = 14,
		JOB_ACROBAT = 15,
		JOB_ELEMENTALLORD = 17,
		JOB_FORCEUSER = 18,
		JOB_PALADIN = 20,
		JOB_PRIEST = 22,

		JOB_MAX = 13
	};

	enum {
		ALL = -1,
	};

	enum eItemTypeEnum {
		ITEMTYPE_FARM = -3,
		ITEMTYPE_ETC = -2,
		ITEMTYPE_WEAPON = 0,		// 무기/공격보조구
		ITEMTYPE_PARTS = 1,			// 방어구
		ITEMTYPE_NORMAL = 2,		// 기타
		ITEMTYPE_SKILL = 3,			// 
		ITEMTYPE_JEWEL = 4,			// 보옥
		ITEMTYPE_PLATE = 5,			// 문장
		ITEMTYPE_INSTANT = 6,		//
		ITEMTYPE_QUEST = 7,			// 퀘스트
		ITEMTYPE_RANDOM = 8,		// 랜덤아이템
		ITEMTYPE_HAIRDYE = 9,
		ITEMTYPE_SKILLBOOK = 10,	// 스킬 북
		ITEMTYPE_GLYPH = 38,			// 문장
		ITEMTYPE_POTENTIAL_JEWEL = 43,	// 잠재력
		ITEMTYPE_PETALTOKEN = 49,	// 페탈
		ITEMTYPE_RECIPE = 50,		// 레시피
		ITEMTYPE_SEED = 52,			// 씨앗
		ITEMTYPE_CROP = 64,			// 농작물
		ITEMTYPE_COOKING = 65,		// 요리
		ITEMTYPE_FISH = 66,			// 수산물
	};

	enum eWeaponType
	{
		WEAPON_SWORD,			// 대검
		WEAPON_GAUNTLET,		// 건틀렛
		WEAPON_AXE,				// 도끼
		WEAPON_HAMMER,			// 해머
		WEAPON_SMALLBOW,		// 소궁
		WEAPON_BIGBOW,			// 대궁
		WEAPON_CROSSBOW,		// 석궁
		WEAPON_STAFF,			// 스테프
		WEAPON_BOOK,			// 마법서
		WEAPON_ORB,				// 수정구
		WEAPON_PUPPET,			// 주술인형
		WEAPON_MACE,			// 메이스
		WEAPON_FLAIL,			// 프레일
		WEAPON_WAND,			// 완드
		WEAPON_SHIELD,			// 방패
		WEAPON_ARROW,			// 화살
	};

	enum eEquipType
	{
		EQUIP_FACE,				// 얼굴
		EQUIP_HAIR,				// 머리
		EQUIP_HELMET,			// 헬멧
		EQUIP_BODY,				// 상의
		EQUIP_LEG,				// 하의
		EQUIP_HAND,				// 손
		EQUIP_FOOT,				// 발
		EQUIP_NECKLACE,
		EQUIP_EARRING,
		EQUIP_RING1,
		EQUIP_RING2,
		EQUIP_CREST,
		EQUIP_WEAPON1,			// 무기
		EQUIP_WEAPON2,
	};

	enum eCount {		
		MAX_ITEM_COUNT_FOR_PAGE = 5,
		PAGE_COUNT_FOR_REQUEST = 10,
		PAGE_COUNT_FOR_SCREEN = 10,	// 불변 예정
	};

	enum eSortType {
		MARKETSORT_ITEM_ASC = 0,	// 아이템오름차순
		MARKETSORT_ITEM_DESC,		// 아이템내림차순
		MARKETSORT_LEVEL_ASC,		// 레벨오름차순
		MARKETSORT_LEVEL_DESC,		// 레벨내림차순
		MARKETSORT_ONEPRICE_ASC,	// 개당가격오름차순
		MARKETSORT_ONEPRICE_DESC,	// 개당가격내림차순
		MARKETSORT_ITEMGRADE_ASC,	// 강화오름차순
		MARKETSORT_ITEMGRADE_DESC,	// 강화내림차순
	};

	enum {
		MESSAGEBOX_LEVEL_ERROR = 3549,
		MESSAGEBOX_BUY_ITEM = 4050,
		MESSAGEBOX_PETAL_BUY_ITEM = 4161,
	};

public:
	CDnMarketBuyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMarketBuyDlg(void);

protected:
	struct NODE  {
		int m_nStringID;
		int m_Job;
		int m_MainType;
		int m_DetailType;
		int m_CashDetailType;
		int m_ExcahngeItemID;
		int m_GroupID;
		int m_ExchangeType;
		NODE( int nStringID, int Job = JOB_NONE, int MainType = ALL, int DetailType = ALL, int CashDetailType = ALL, int ExchangeItemID = ALL, int GroupID = ALL, int ExchangeType = ALL ) 
			: m_nStringID( nStringID )
			, m_Job( Job )
			, m_MainType( MainType )
			, m_DetailType( DetailType )
			, m_CashDetailType( CashDetailType )
			, m_ExcahngeItemID( ExchangeItemID )
			, m_GroupID( GroupID )
			, m_ExchangeType( ExchangeType )
		{}
	};

	struct SearchInfo {
		int nPageIndex;
		int nReqPageIndex;
		char cMinLevel;
		char cMaxLevel;
		std::vector<char> cItemGradeArray;
		char cJob;
		short nMainType;
		char cDetailType;
		std::wstring pwszSearch1;
		std::wstring pwszSearch2;
		std::wstring pwszSearch3;
		char cSortType;
		int nExchangeItemID;
		bool bLowJobGroup;
		std::wstring pwszSearch;
	} m_LastSearchInfo;

	std::vector< NODE *>				m_pNodes;
	std::vector< NODE >				m_Nodes;
	int											m_nTotalItemCount;
	std::deque< std::pair<int, char> >						m_RequestedKeys;
	std::pair<int, char>											m_LastBuyKey;
	std::map< std::pair<int, char >, std::vector< TMarketInfo >	>	m_CachedBuyList;
	std::set< std::pair<int, char > > m_IsNewKey;
	int											m_nBuyItemDBID;

	CDnMarketPriceTooltipDlg * m_pMarketPriceTooltipDlg;

	typedef CEtUIDialog BaseClass;
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	CEtUIListBox* m_pSearchItemListBox;
	CEtUIStatic* m_pSearchItemListBoxBoard;
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE

private:
	void SetDefaultState();	
	void RequestMarketBuyList();
	void RefreshPageControls();
	int GetTotalPage();
	int GetStartPage();
	int GetReqStartPage();
	void UpdateListItems();
	void RefreshCount();
	std::pair<int, char> GetLastReqKey() { return std::make_pair( m_LastSearchInfo.nReqPageIndex, m_LastSearchInfo.cSortType ); }
	std::pair<int, char> GetLastKey() { return std::make_pair( m_LastSearchInfo.nPageIndex, m_LastSearchInfo.cSortType ); }
	void MakeEmptyKey( std::pair<int, char> &Key ) { Key.first = -1; }
	bool IsEmptyKey( std::pair<int, char> &Key ) { return (Key.first == -1);}  
	void ProcessBuy();
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	void OnSetSearchItemList( const std::wstring& strText );
	void OnSearchItem();
#endif // PRE_ADD_ITEMNAME_AUTOCOMPLETE

public:
	void InitialUpdate() override;
	void Initialize( bool bShow ) override;
	void Show( bool bShow ) override;
	void Process( float fElapsedTime ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
	void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg ) override;

public:
	void OnRecvUpdateList( std::vector< TMarketInfo > &vecBuyList, int nTotalCount );	
	void OnRecvRemoveItem( int nMarketDBID );
	void RefreshRequestNeed() { SetDefaultState(); }
	void SetSearchItem( CDnItem *pItem );
	void OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice );
};