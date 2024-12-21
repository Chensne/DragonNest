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
		ITEMTYPE_WEAPON = 0,		// ����/���ݺ�����
		ITEMTYPE_PARTS = 1,			// ��
		ITEMTYPE_NORMAL = 2,		// ��Ÿ
		ITEMTYPE_SKILL = 3,			// 
		ITEMTYPE_JEWEL = 4,			// ����
		ITEMTYPE_PLATE = 5,			// ����
		ITEMTYPE_INSTANT = 6,		//
		ITEMTYPE_QUEST = 7,			// ����Ʈ
		ITEMTYPE_RANDOM = 8,		// ����������
		ITEMTYPE_HAIRDYE = 9,
		ITEMTYPE_SKILLBOOK = 10,	// ��ų ��
		ITEMTYPE_GLYPH = 38,			// ����
		ITEMTYPE_POTENTIAL_JEWEL = 43,	// �����
		ITEMTYPE_PETALTOKEN = 49,	// ��Ż
		ITEMTYPE_RECIPE = 50,		// ������
		ITEMTYPE_SEED = 52,			// ����
		ITEMTYPE_CROP = 64,			// ���۹�
		ITEMTYPE_COOKING = 65,		// �丮
		ITEMTYPE_FISH = 66,			// ���깰
	};

	enum eWeaponType
	{
		WEAPON_SWORD,			// ���
		WEAPON_GAUNTLET,		// ��Ʋ��
		WEAPON_AXE,				// ����
		WEAPON_HAMMER,			// �ظ�
		WEAPON_SMALLBOW,		// �ұ�
		WEAPON_BIGBOW,			// ���
		WEAPON_CROSSBOW,		// ����
		WEAPON_STAFF,			// ������
		WEAPON_BOOK,			// ������
		WEAPON_ORB,				// ������
		WEAPON_PUPPET,			// �ּ�����
		WEAPON_MACE,			// ���̽�
		WEAPON_FLAIL,			// ������
		WEAPON_WAND,			// �ϵ�
		WEAPON_SHIELD,			// ����
		WEAPON_ARROW,			// ȭ��
	};

	enum eEquipType
	{
		EQUIP_FACE,				// ��
		EQUIP_HAIR,				// �Ӹ�
		EQUIP_HELMET,			// ���
		EQUIP_BODY,				// ����
		EQUIP_LEG,				// ����
		EQUIP_HAND,				// ��
		EQUIP_FOOT,				// ��
		EQUIP_NECKLACE,
		EQUIP_EARRING,
		EQUIP_RING1,
		EQUIP_RING2,
		EQUIP_CREST,
		EQUIP_WEAPON1,			// ����
		EQUIP_WEAPON2,
	};

	enum eCount {		
		MAX_ITEM_COUNT_FOR_PAGE = 5,
		PAGE_COUNT_FOR_REQUEST = 10,
		PAGE_COUNT_FOR_SCREEN = 10,	// �Һ� ����
	};

	enum eSortType {
		MARKETSORT_ITEM_ASC = 0,	// �����ۿ�������
		MARKETSORT_ITEM_DESC,		// �����۳�������
		MARKETSORT_LEVEL_ASC,		// ������������
		MARKETSORT_LEVEL_DESC,		// ������������
		MARKETSORT_ONEPRICE_ASC,	// ���簡�ݿ�������
		MARKETSORT_ONEPRICE_DESC,	// ���簡�ݳ�������
		MARKETSORT_ITEMGRADE_ASC,	// ��ȭ��������
		MARKETSORT_ITEMGRADE_DESC,	// ��ȭ��������
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