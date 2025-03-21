#pragma once
#include "EtUIDialog.h"
#include "MIInventoryItem.h"
#include "DnCustomControlCommon.h"
#include "DnCashShopDefine.h"

class CEtUITextBox;
class CDnSlotButton;
class CDnItem;
class CDnState;
#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
class CDnQuickSlotButton;
#endif
#ifdef PRE_ADD_DRAGON_GEM
class CDnDragonGemTooltipDlg;
#endif

enum emTOOLTIP_STATIC
{
	STATIC_INVALID = -1,
	SKILL_DISTANCE,
	SKILL_DESCRIPTION,
	SKILL_NEXT_DESCRIPTION,

	ITEM_DURABLE,
	ITEM_UNDESTRUCTION,
	ITEM_ATTACK,	// 사용안함
	ITEM_DEFENSE,	// 사용안함

	ITEM_STRENGTH,
	ITEM_AGILITY,
	ITEM_INTELLIGENCE,
	ITEM_STAMINA,
	ITEM_SPIRIT,

	ITEM_STIFFNESS_P,
	ITEM_STIFFNESS_R,
	ITEM_STUN_P,
	ITEM_STUN_R,
	ITEM_CRITICAL_P,
	ITEM_CRITICAL_R,

	ITEM_FIRE_A,
	ITEM_ICE_A,
	ITEM_LIGHT_A,
	ITEM_DARK_A,

	ITEM_FIRE_R,
	ITEM_ICE_R,
	ITEM_LIGHT_R,
	ITEM_DARK_R,

	ITEM_MOVE,
	ITEM_HP,
	ITEM_SP,

	ITEM_JUDGMENT,

	ITEM_COOLTIME,
	ITEM_SAFEZONEMOVE,

	ITEM_MAXHP,
	ITEM_MAXSP,

	ITEM_ATTACK_P,
	ITEM_ATTACK_M,
	ITEM_DEFENSE_P,
	ITEM_DEFENSE_M,

	ITEM_RECOVERSP,

	ITEM_ADDEXP,
};

class CDnTooltipDlg : public CEtUIDialog
{
public:
	enum emDIALOG_POS
	{
		MOUSE_LEFT = 0x0001,
		MOUSE_TOP = 0x0002,
		MOUSE_RIGHT = 0x0004,
		MOUSE_BOTTOM = 0x0008,

		MOUSE_LT = MOUSE_LEFT|MOUSE_TOP,
		MOUSE_RT = MOUSE_RIGHT|MOUSE_TOP,
		MOUSE_LB = MOUSE_LEFT|MOUSE_BOTTOM,
		MOUSE_RB = MOUSE_RIGHT|MOUSE_BOTTOM,

		MOUSE_DEFAULT = MOUSE_RIGHT|MOUSE_BOTTOM,

		LINE_MAX = 25,
	};

	enum emToolTipType
	{
		TOOLTIP_NORMAL,
		TOOLTIP_NAMELINK,
		TOOLTIP_MANUALCONTROL,
	};

#define MAX_STATE_COMPARE_UNIT_COUNT 4
#define MAX_RING_PARTS_COUNT		 2
	enum eStateCompareType
	{
		eSCT_VALUE,
		eSCT_RANGE,
	};

	enum eStateCompareCategory
	{
		eSCC_NORMAL,
		eSCC_ENCHANT,
		eSCC_POTENTIAL,
	};

	template <typename T>
	struct SStateCompareUnit
	{
		T values[MAX_STATE_COMPARE_UNIT_COUNT];

		void Clear()
		{
			memset(values, 0, sizeof(T) * MAX_STATE_COMPARE_UNIT_COUNT);
		}

		SStateCompareUnit(T value0, T value1)
		{
			Clear();
			values[0] = value0;
			values[1] = value1;
		}

		SStateCompareUnit(T value0, T value1, T value2, T value3)
		{
			values[0] = value0;
			values[1] = value1;
			values[2] = value2;
			values[3] = value3;
		}

		SStateCompareUnit()
		{
			Clear();
		}
	};

public:
	CDnTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTooltipDlg(void);

protected:
	CEtUITextBox *m_pTextBox;
	SUICoord m_TextBoxDefaultCoord;
	CEtUITextBox *m_pTextBox2;
	CEtUITextBox *m_pTextBox3;
	DnSlotButtonHandle m_hSlotButton;
	ITEM_SLOT_TYPE m_itemSlotType;
	MIInventoryItem::InvenItemTypeEnum m_SrcItemType;
	CEtUIStatic *m_pStaticName;

	CEtUIStatic *m_pLineVertical2;
	CEtUIStatic *m_pLineVertical3;
	SUICoord m_uiDefaultCoord;

	// 바탕 텍스처 바꾼다.
	CEtUIStatic *m_pStaticBase;
	CEtUIStatic *m_pStaticBaseEquip;

	CEtUIStatic *m_pStaticLine[LINE_MAX];
	SUICoord m_LineBaseCoord;
	int m_nCurLineIndex;

	DWORD m_dwMousePos;

	CDnTooltipDlg* m_pCompareTooltipDlg;
	CDnTooltipDlg* m_pCompareTooltipDlg2;

	MIInventoryItem *m_pSrcItem;
	MIInventoryItem *m_pDestItem;
	MIInventoryItem *m_pDestItem2;

	MIInventoryItem *m_pOverlapCompareItem;

	bool m_bTooltipSwitch;

	CDnItem *m_pTargetDiffItem;

	float m_fUpdateOrigPosX;
	float m_fUpdateOrigPosY;

	CEtUIStatic *m_pStaticPointBox;

//#ifdef PRE_ADD_MAINQUEST_UI
//	bool m_bShowMainQuestToolTip;
//#endif

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	#define OVERLAP_SETITEM_MAX  10 // Overlap SetItemID는 10개까지만 저장된다
	bool m_bSwitchOverlapToolTip;	// 셋트 아이템 전용 툴팁이 IsShow인지 체크
#endif

#ifdef PRE_ADD_DRAGON_GEM
	CDnDragonGemTooltipDlg*	m_pDragonGemTooltipDlg;
#endif

#ifdef PRE_ADD_48682
	// 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화.
	enum EAllowMapTypes
	{
		EAllowMapTypes_None       = 0,   // "모든맵에서 사용불가"
		EAllowMapTypes_Village    = 1,   // 마을
		EAllowMapTypes_WorldMap   = 2,   // 월드맵
		EAllowMapTypes_Dungeon    = 4,   // 던전
		EAllowMapTypes_PVP        = 8,   // PVP
		EAllowMapTypes_DarkLair   = 16,  // 다크레어
		EAllowMapTypes_Nest       = 32,  // 네스트
		EAllowMapTypes_SDragonNest= 64,  // 씨드래곤 네스트
		EAllowMapTypes_Guild      = 128, // 길드전
		EAllowMapTypes_NestHell   = 256, // 네스트
		EAllowMapTypes_GDragonNest= 512, // 그린드래곤 네스트    
	};// 현재 11개.
	#define EAllowMapTypesSIZE 11
	std::wstring m_strAllowZone; // "\n[사용가능지역]\n"
	std::vector< int > m_vAllowMapBitFlag; // EAllowMapTypes 값 배열. 
	std::map< UINT, std::wstring > m_mapStrMapType; // EAllowMapTypes 와 매칭되는 텍스트 map.	

#endif	

#ifdef PRE_ADD_CASHREMOVE
	// 캐시제거대기아이템의 시간툴팁처리용.
	__time64_t m_tBegin;
	INT64 m_nCashRemoveSerial;
#endif

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
	int m_nStoreSaleItemPeriod; // 기간 일수.
#endif // PRE_ADD_COMBINEDSHOP_PERIOD

#ifdef PRE_ADD_COMPARETOOLTIP
	// #69087
	bool m_bShiftPress;
	int m_nShowRingSlotIndex;
	bool m_bTwoRing;
	struct STempData
	{		
		float fX;
		float fY;
		bool bAutoCompare;		
		STempData(): fX(0.0f), fY(0.0f), bAutoCompare(true){}
		void SetData( float _fx, float _fy, bool _bAuto ){
			fX = _fx;	fY = _fy;	bAutoCompare = _bAuto;
		}
	};
	STempData m_ringCompare;
#endif // PRE_ADD_COMPARETOOLTIP

protected:
	void SetTooltipText( MIInventoryItem *pItem, bool bCompare, bool bNameLink = false );
	void SetItemText( MIInventoryItem *pInvenItem, bool bCompare, bool bNameLink = false);
	void SetItemStateText( CDnState *pState, DWORD dwFontColor, LPCWSTR szPrefix = NULL, std::vector<CDnItem*>* pCompareItemList = NULL, eStateCompareCategory cat = eSCC_NORMAL);

	CDnItem* GetCompareDestItem(CDnItem* pSrcItem) const;
	void GetCompareDestItem_Ring(std::vector<CDnItem*>& vResults, CDnItem* pSrcItem) const;
	template <typename T>
	void SetStateCompareText(eStateCompareType type, std::wstring& szPreText, const SStateCompareUnit<T>& unit, bool bPercent = false, int floatUnit = -1);
	void GetStateCompareTextUnit(std::wstring& out, eStateCompareType type, float diffMin, float diffMax, bool bPercent, int floatUnit) const;
	void GetStateCompareTextUnit(std::wstring& out, eStateCompareType type, int diffMin, int diffMax, bool bPercent, int floatUnit) const;
	bool IsCompareTextUnitOverLine(const std::wstring& compareText, const WCHAR* preText);

	void SetStateText( LPCWSTR szText, DWORD dwFontColor, LPCWSTR szPrefix );	// SetItemStateText함수에서 부르는 텍스트 출력 부분.
	void SetItemSetText( CDnItem *pItem, DWORD dwFontColor, bool bNameLink = false );	// SetItemText함수에서 부르는 셋트아이템 관련 부분.

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	void SetOnlySetItemText( CDnItem *pItem, DWORD dwFontColor ); // 툴팁에 세트아이템 효과만 적는다
	void GetOverlapID(bool bIsOverlap, int nOverLapID, std::vector<int>& vIDList); // 현재 아이템의 Overlap아이디를 불러온다 (Overlap아이템이 아니면 기존의 SetItemID를 불러옴)
	void ComputeEquipedSetItemCount(int nMouseOverSetItemID, std::vector<int> vEquipedItemList, int& nMaxCount); // 착용중인 셋트 아이템 갯수를 구한다
	void Update(bool bCompare);
	void UpdateCompareToolTip();
	MIInventoryItem* GetToolTipOverItem(bool bCompare);
	bool IsCashPartsItem(MIInventoryItem* pItem);
#endif

	void SetCharmItemText( CDnItem *pItem );
	void SetPackageItemText( CDnItem *pItem );
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	void SetRandomCharmItemText( CDnItem* pItem );
#endif 
	void SetSkillText( MIInventoryItem *pInvenItem );
	void SetGuildWarSkillText( MIInventoryItem *pInvenItem );
	void SetGestureText( int nGestureID );
	void SetLifeSkillText( int nLifeSkillID );
	wstring GetItemName( CDnItem *pItem );
	void SetAppellationItemText( CDnItem *pItem );

	void SetCashShopItemText(MIInventoryItem *pInvenItem, bool bCompare, bool bCashRewardItem = false);
	void SetItemText_ExchangeAttribute(CDnItem* pItem, bool bNameLink = false);
	bool SetItemText_CashAttribute(CDnItem* pItem, const SCashShopItemInfo& info);
	void SetItemText_ItemAttribute(CDnItem* pItem, bool bCompare);
	bool SetItemText_CashItemAbility(CDnItem* pItem, const SCashShopItemInfo& info);
	void SetItemText_CashItemDescription(CDnItem* pItem, const SCashShopItemInfo& info);
	void SetItemText_CostumeDesignMix(CDnItem* pItem);

	void SetCombinedShop_PurchaseGold( int nMoney, bool bNoBenefit = false );
#ifdef PRE_ADD_NEW_MONEY_SEED
	void SetCombinedShop_PurchaseSeed( int nSeed );
#endif // PRE_ADD_NEW_MONEY_SEED
	void SetCombinedShop_PurchaseItem( int nItemID, int nCount );
	void SetCombinedShop_PurchaseLadderPoint( int nPoint );
	void SetCombinedShop_PurchaseLimitLadderGrade( int nGrade );
	void SetCombinedShop_PurchaseGuildPoint( int nItemID, int nPoint );
	void SetCombinedShop_PurchaseUnionPoint( int eType, int nPoint );
#if defined( PRE_ADD_LIMITED_SHOP )
#if defined( PRE_FIX_74404 )
	void SetCombinedShop_PurchaseLimitedShopItem( int nShopID, int nItemID, int nLimitCount, int nShopLimitReset );
#else // #if defined( PRE_FIX_74404 )
	void SetCombinedShop_PurchaseLimitedShopItem( int nItemID, int nLimitCount, int nShopLimitReset );
#endif // #if defined( PRE_FIX_74404 )
#endif

	void SetCombinedShop_PurchaseLimitJob( int nJobID);
	void SetCombinedShop_PurchaseLimitLevel( int nLevel );
	void SetCombinedShop_PurchaseLimitPvPRank( int nRank );
	void SetCombinedShop_PurchaseLimitGuildLevel( int nLevel );

	void SetItemText_CombinedShop( CDnItem * pItem, int nTabID );
	bool GetEnableJob( int nNeedJobID, int nMyJobID );

	bool SetItemText_NeedJob(const CDnItem* pItem);
	bool SetItemText_DetailType(const CDnItem* pItem);

	bool SetItemText_Period(const CDnItem* pItem);

#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
	bool SetCashItemText_StorePeriod(const CDnItem* pItem);
	void SetCashItemText_StoreReversion(const CDnItem* pItem);
#endif

	bool SetItemText_UnableDisjointOrEnchant(CDnItem* pItem);
	bool SetItemText_Reversion(const CDnItem* pItem);
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
	bool SetItemText_UnableConstumeMix( const CDnItem* pItem );
	bool SetItemText_UnableDesignMix( const CDnItem* pItem );
#endif 
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	bool SetItemText_CashPackageItemContentsDesc(CDnItem* pItem, const SCashShopItemInfo& info);
#endif

#if defined(PRE_ADD_SECONDARY_SKILL)
	bool SetItemText_SecondarySkill( SecondarySkill::SubType::eType eSecondarySkillSubType, const CDnItem* pItem );
#endif	//#if defined(PRE_ADD_SECONDARY_SKILL)
#ifdef PRE_ADD_SALE_COUPON
	bool SetItemText_CouponSaleItemList( const CDnItem* pItem );
#endif // PRE_ADD_SALE_COUPON

	void ShowCompareLight( bool bShow );
#ifdef PRE_ADD_COMPARETOOLTIP
	// Ring 의 경우 PartType이 테이블에서 지정되는데 항상 Ring, CashRing 이다. 
	// Shift 를 눌러 반지비교시에 두번째슬롯에 장착된 반지가 지정되는데 PartType이 Ring, CashRing 으로 지정되는 문제가 있어서 강제로 바꿀수 있도록한다.
	bool ShowCompareTooltip( bool bCash, eItemTypeEnum nType, int nSlotIndex, MIInventoryItem *&pDestItem, CDnTooltipDlg *pDialog, int forcePartsType=-1, bool bNameLink = false );
#else
	bool ShowCompareTooltip( bool bCash, eItemTypeEnum nType, int nSlotIndex, MIInventoryItem *&pDestItem, CDnTooltipDlg *pDialog, bool bNameLink = false);
#endif // PRE_ADD_COMPARETOOLTIP
	void HideCompareTooltip( CDnTooltipDlg *pDialog );

	// 개행처리를 안에다 같이 수행하는걸로 할까 하다가, 개행과 라인텍스처가 항상 같이 다니지 않게 할수도 있어서 따로 두었다.
	void ClearDivisionLine();
	void AddDivisionLine( int nYOffset = 6, bool bSmallWidth = false );	// YOffset값은 현재 라인Height인 15에서 적절한 값을 직접 넣은 것이다. 라인Height바뀌면 바꿀 것.
	void RemoveLastDivisionLine();

	void SetItemPrefixSystemInfo(CDnItem *pItem, int nSkillID, int nSkillLevel);

	void SetSkillLevelUpItemInfo(CDnItem *pItem);

#ifdef PRE_ADD_CASHREMOVE
	// 인벤토리 캐시삭제탭에 속한 아이템의 툴팁에 삭제대기시간 출력.
	void SetRemoveCashItemExpireTime( CDnItem * pItem );
#endif

	void UpdateSourceItemDescAndDurationTime( CDnItem * pItem );
	void SetItemText_RepurchaseSellPrice(CDnItem* pItem);

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	void SetNotifyCompoundItemText( MIInventoryItem *pInvenItem );
#endif

#ifdef PRE_ADD_48682
	// 툴팁개선 - 아이템 사용가능지역 텍스트 시스템화.
	void SetItemText_AllowMapType( CDnItem * pItem );
#endif

#if defined(PRE_ADD_63603)
	void SetInteractionInfo(MIInventoryItem *pInvenItem);
#endif // PRE_ADD_63603

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	void SetItemText_CashRefund( CDnItem * pItem ); // 캐시샾에서 청약철회 약관 출력.
#endif // PRE_ADD_CASHSHOP_ACTOZ

#ifdef PRE_ADD_DRAGON_GEM
	// protected function
#endif // PRE_ADD_DRAGON_GEM


public:
	void ShowTooltip(CDnSlotButton *pControl, float fX, float fY, bool bAutoCompare = true, CDnItem *pTargetDiffItem = NULL);
	void ShowTooltip(CDnItem* pSrcItem, MIInventoryItem::InvenItemTypeEnum itemType, ITEM_SLOT_TYPE slotType, float fX, float fY, bool bNameLink, bool bAutoCompare = true, CDnItem *pTargetDiffItem = NULL );
	void SetTargetDiffItem( CDnItem* pItem );
	void HideTooltip();
	void UpdateTooltip( float fX, float fY );
	void UpdateTooltipNextPage();
	void RefreshTooltip( CEtUIControl *pControl );

#ifdef PRE_FIX_SKILL_TOOLTIP_REFRESH
	void RefreshTooltip( CDnSkill *pSkill );
#endif
	void OnChangeTooltip();

	void SetToolTipSwitch(bool bEnable) { m_bTooltipSwitch = bEnable;  }
	void ResetToolTipSwitch();

	void ClearText();
	//void RefreshTooltip( CEtUIControl *pControl );
	//CDnSlotButton *GetSlotButton() { return m_pInControl; }
	bool SetItemText_CashItem_SkillLevelupInfo(CDnItem *pItem);

#ifdef PRE_ADD_PARTSITEM_TOOLTIP_INFO
	void SetEnchantEnableInfo();
	void SetCompoundEnableInfo();
	void SetPotentialEnableInfo();
#endif

#ifdef PRE_MOD_CASHSHOP_NOETERNITY_TOOLTIP
	bool IsCashRewardItem(const MIInventoryItem* pItem, bool bDontCareSN) const;
#endif

#ifdef PRE_ADD_COMPARETOOLTIP
	// #69087	
	void PressShift( bool b );
#endif // PRE_ADD_COMPARETOOLTIP

#ifdef PRE_ADD_OVERLAP_SETEFFECT
	const bool IsShowOverlapToolTip() { return m_bSwitchOverlapToolTip; }
	void ChangeOverlapSetItemToolTip(CDnTooltipDlg* pToolTip, bool bIsCompare, bool bIsKeyPush);
	void ChangeCompareOverlapSetItemToolTip(bool bIsKeyPush);

	void PressAlt(bool bPush);
#endif

#ifdef PRE_ADD_DRAGON_GEM
	// public function
	void ShowDragonGemToolTip();
	void CloseDragonGemToolTip();
#endif // PRE_ADD_DRAGON_GEM

//#ifdef PRE_ADD_MAINQUEST_UI
//	void ShowMainQuestTooltip(CDnSlotButton *pControl, float fX, float fY, bool bAutoCompare = true, CDnItem *pTargetDiffItem = NULL);
//	void HideMainQuestTooltip();
//	const bool IsShowMainQuestTooltip() { return m_bShowMainQuestToolTip; }
//#endif

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );

	static emTOOLTIP_STATIC GetToolTipStaticByStateType(int stateType);
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void SetMode(emToolTipType type) { m_Type = type; }

	void SetItemText_LockState(CDnItem* pItem);

private:
	emToolTipType m_Type;
};

template <typename T>
void CDnTooltipDlg::SetStateCompareText(eStateCompareType type, std::wstring& szPreText, const SStateCompareUnit<T>& unit, bool bPercent, int floatUnit)
{
	if (type == eSCT_RANGE)
	{
		T diffMin = unit.values[1] - unit.values[0];
		T diffMax = unit.values[3] - unit.values[2];

		if (int(diffMin * 100) == 0 && int(diffMax * 100) == 0)
			return;

		if (diffMin != 0 || diffMax != 0)
		{
			std::wstring szCompareTemp;
			GetStateCompareTextUnit(szCompareTemp, type, diffMin, diffMax, bPercent, floatUnit);

			DWORD compareColor = textcolor::TOOLTIP_STAT_UPDOWN;
			if (diffMin > 0 && diffMax > 0)
				compareColor = textcolor::TOOLTIP_STAT_UP;
			else if (diffMin < 0 && diffMax < 0)
				compareColor = textcolor::TOOLTIP_STAT_DOWN;

			if (IsCompareTextUnitOverLine(szCompareTemp, szPreText.c_str()))
				m_pTextBox->AddText(szCompareTemp.c_str(), compareColor, UITEXT_RIGHT);
			else
				m_pTextBox->AppendText(szCompareTemp.c_str(), compareColor);

			szPreText += szCompareTemp;
		}
	}
	else if (type == eSCT_VALUE)
	{
		T diff = unit.values[1] - unit.values[0];

		if (int(diff * 100) == 0)
			return;

		if (diff != 0)
		{
			std::wstring tempDiff;
			GetStateCompareTextUnit(tempDiff, type, diff, unit.values[0], bPercent, floatUnit);

			DWORD compareColor = textcolor::TOOLTIP_STAT_UPDOWN;
			if (diff > 0)
				compareColor = textcolor::TOOLTIP_STAT_UP;
			else if (diff < 0)
				compareColor = textcolor::TOOLTIP_STAT_DOWN;

			if (IsCompareTextUnitOverLine(tempDiff, szPreText.c_str()))
				m_pTextBox->AddText(tempDiff.c_str(), compareColor, UITEXT_RIGHT);
			else
				m_pTextBox->AppendText(tempDiff.c_str(), compareColor);

			szPreText += tempDiff;
		}
	}
}