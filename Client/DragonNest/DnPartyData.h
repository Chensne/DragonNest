#pragma once

#include "DnJobIconStatic.h"

#ifdef PRE_PARTY_DB
enum eTargetStageFilterType
{
	eTARGETFILTER_ALL,
	eTARGETFILTER_DUNGEON,
	eTARGETFILTER_NEST,
	eTARGETFILTER_ETC
};

enum eMakePartyTargetMapOption
{
	CURRENT_VILLAGE,
	WHOLE_VILLAGE,
};

#ifdef _WORK
#define MAX_PARTY_SEARCH_BTN_COOLTIME 0.f
#define MAX_PARTY_ONLY_SAME_VILLAGE_COOLTIME 0.f
#else
#define MAX_PARTY_SEARCH_BTN_COOLTIME 1.f
#define MAX_PARTY_ONLY_SAME_VILLAGE_COOLTIME 5.f
#endif

#define DEFAULT_PARTY_LIST_ALLOW_WORLDZONE 0

struct SPartyVillageIconData
{
	int nVillageMapID;
	CEtUIStatic* pIcon;

	SPartyVillageIconData() { nVillageMapID = 0; pIcon = NULL; }
	bool operator==(const SPartyVillageIconData& rhs) { return ((nVillageMapID == rhs.nVillageMapID) && (pIcon == rhs.pIcon)); }
};
#endif

#ifdef PRE_WORLDCOMBINE_PARTY
struct SWorldCombinePartyData
{
	int maxUser;
	bool bHasPartyBuff;
	std::wstring partyBuffTooltipString;

	SWorldCombinePartyData()
	{
		maxUser = 0;
		bHasPartyBuff = false;
	}

	bool IsEmpty() const { return (maxUser <= 0); }
};
#endif

struct SPartyInfoList
{
	wchar_t m_wszTemp[80];

	TPARTYID m_Number;

	CEtUIStatic *m_pSecret;
	bool m_bSecret;

	CEtUIStatic *m_pTitle;
	CEtUIStatic *m_pCount;
	CEtUIStatic *m_pBase;
	CEtUIStatic *m_pUserLevel;
	CEtUIStatic* m_pTargetStage;
	CEtUIStatic* m_pTargetDifficulty;
	CEtUIStatic* m_pVoice;
#ifdef PRE_PARTY_DB
	std::vector<SPartyVillageIconData> m_VillageIconList;
	CEtUIStatic* m_pBonusExpParty;
#endif
#ifdef PRE_WORLDCOMBINE_PARTY
	int m_nWorldCombinePartyID;
	CEtUIStatic* m_pWorldCombinePartyBonus;
#endif

#ifdef PRE_ADD_NEWCOMEBACK
	CEtUIStatic* m_pComebackUser;
#endif // PRE_ADD_NEWCOMEBACK

	SPartyInfoList();

	void InitInfoList();
	bool IsInsideItem( float fX, float fY );
#ifdef PRE_PARTY_DB
	#ifdef PRE_WORLDCOMBINE_PARTY
	void SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, 
					CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice, CEtUIStatic* pBonusExpParty,
					CEtUIStatic* pWorldCombinePartyBonus
				#ifdef PRE_ADD_NEWCOMEBACK
					,CEtUIStatic* pComebackUser
				#endif // PRE_ADD_NEWCOMEBACK
					);
	#else
	void SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, 
		CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice, CEtUIStatic* pBonusExpParty
	#ifdef PRE_ADD_NEWCOMEBACK
		,CEtUIStatic* pComebackUser
	#endif // PRE_ADD_NEWCOMEBACK
		);
	#endif // PRE_WORLDCOMBINE_PARTY
	void InitializeVillageIconControl(int nVillageMapID, CEtUIStatic* pVillageIcon);
	void ShowVillageIcon(int nVillageMapID);
#else
	void SetControl( CEtUIStatic* pBase, CEtUIStatic *pSecret, CEtUIStatic *pTitle, CEtUIStatic *pCount, CEtUIStatic *pUserLevel, CEtUIStatic* pStage, CEtUIStatic* pDifficulty, CEtUIStatic* pVoice
	#ifdef PRE_ADD_NEWCOMEBACK
		,CEtUIStatic* pComebackUser
	#endif // PRE_ADD_NEWCOMEBACK
		);
#endif

	void SetNumber( TPARTYID nNumber );
	TPARTYID GetNumber() const { return m_Number; }

	void SetSecret( bool bSecret );
	bool IsSecret() const { return m_bSecret; }

#if defined (PRE_PARTY_DB) && defined (_WORK)
	void SetDebugTitle( LPCWSTR wszTitle, INT64 sortPoint, ePartyType type );
#endif
#ifdef PRE_WORLDCOMBINE_PARTY
	void SetWorldCombineParty(bool bSet, const SWorldCombinePartyData& combineData);
	void SetWorldCombineBonus(bool bSet);
#endif
	void SetTitle( LPCWSTR wszTitle );
	void SetCount( int nCurCount, int nMaxCount );
	void SetWorldLevel( int nLevel );
#ifdef PRE_PARTY_DB
	void SetUserLevel(int nMinLevel);
	void SetBonusExpParty(bool bSet);
	bool IsEmpty() const;
#else
	void SetUserLevel( int nMinLevel, int nMaxLevel );
#endif

#ifdef PRE_ADD_NEWCOMEBACK
	void SetComebackUser( bool bShow );
#endif // PRE_ADD_NEWCOMEBACK

	void Enable(bool bEnable);
	bool IsEnable()	{ return m_pBase->IsEnable(); }
	void Clear();
	void SetTargetStage(const std::wstring& stage);
	void SetTargetDifficulty(TDUNGEONDIFFICULTY difficultyIdx);
	void SetVoiceChat(bool bVoiceChat);

	CEtUIControl*	GetControl()	{ return m_pBase; }
};

typedef std::vector<SPartyInfoList>		PARTYINFO_LIST_VEC;
typedef PARTYINFO_LIST_VEC::iterator	PARTYINFO_LIST_VEC_ITER;

struct SPartyMemberData
{
	wchar_t m_wszTemp[80];

	std::wstring name;
	int		nSlotIndex;
	int		nUserLevel;
	bool	bMaster;
	int		nClass;
	int		nWorldLevel;
	UINT	nSessionID;
	int		nFatigueValue;
	DWORD	dwNameColor;
#ifdef PRE_ADD_NEWCOMEBACK
	bool bComeback;
#endif // PRE_ADD_NEWCOMEBACK

	SPartyMemberData();
	virtual void InitInfoList();
	void SetSlotIndex(int nIdx);
	virtual void SetUserLevel(int nLevel);
	virtual void SetMaster(bool bMaster);
	virtual void SetJobClass(int nClass);
	void SetWorldLevel(int nLevel);
	void SetSessionID(UINT nRhsSessionID)	{ nSessionID = nRhsSessionID; }
	void SetFatigueValue(int nFatigue)	{ nFatigueValue = nFatigue; }

	virtual void SetName(const WCHAR* pNameString) { name = pNameString; }
#ifdef PRE_ADD_NEWCOMEBACK
	void SetComeback(bool bShow);
	bool GetComeback(){ return bComeback; }
#endif // PRE_ADD_NEWCOMEBACK

	const std::wstring& GetName() const { return name; }
	int GetSlotIndex() const { return nSlotIndex; }
	UINT	GetSessionID() const { return nSessionID; }
	int GetUserLevel() const { return nUserLevel; }
	int GetJobClass() const { return nClass; }
	int GetWorldLevel() { return nWorldLevel; }
	int	GetFatigueValue() const	{ return nFatigueValue; }
	bool IsMaster() const { return bMaster; }
	bool IsEmpty() const;
};

struct SPartyMemberNormalUIData : public SPartyMemberData
{
	CEtUIStatic* pUserLevel;
	CEtUIStatic* pMaster;
	CEtUIStatic* pClass;
	CEtUIStatic* pName;
	CEtUIStatic* pBase;
	CEtUIStatic* pState;
	CEtUIStatic* pFreeCoinIcon;
	CEtUIStatic* pFreeCoinCount;
	CEtUIStatic* pCashCoinIcon;
	CEtUIStatic* pCashCoinCount;
	CEtUIStatic* pStaticInvitation;
	CEtUIStatic* pTextInviting;
#ifdef PRE_ADD_NEWCOMEBACK
	CEtUIStatic* pComebackUser;	
#endif // PRE_ADD_NEWCOMEBACK

	SPartyMemberNormalUIData();
	virtual void InitInfoList();
	void SetUserLevel(int nLevel);
	void SetState(bool bAbsence);
	void SetJobClass(int nClass);
	void SetCoin(int freeCoin, int cashCoin);
	void SetName(const WCHAR* pNameString, bool bRed);
	void SetMaster(bool bMaster);
	void SetInvitationCtrl(CEtUIStatic* pStatic, CEtUIStatic* pTextStatic);
	void ShowInvitationCtrl(bool bShow);
#ifdef PRE_ADD_NEWCOMEBACK
	void SetComeback(bool bShow);
#endif // PRE_ADD_NEWCOMEBACK
	bool IsShowInvitationCtrl() const		{ if (pStaticInvitation) return pStaticInvitation->IsShow(); return false; }
	bool IsInsideItem(float fX, float fY);
	LPCWSTR GetJobClassString() const { return pClass->GetText(); }
	void Enable(bool bEnable);
	CEtUIControl* GetControl() const	{ return pBase; }
};

struct SPartyMemberRaidUIData : public SPartyMemberData
{
	CEtUIStatic* pUserLevel;
	CEtUIStatic* pMaster;
	CDnJobIconStatic* pJobIcon;
	CEtUIStatic* pName;
	CEtUIStatic* pBase;
	CEtUIStatic* pFreeCoinIcon;
	CEtUIStatic* pFreeCoinCount;
	CEtUIStatic* pCashCoinIcon;
	CEtUIStatic* pCashCoinCount;
	CEtUIStatic* pStaticInvitation;
	CEtUIStatic* pTextInviting;
#ifdef PRE_ADD_NEWCOMEBACK
	CEtUIStatic* pComebackUser;	
#endif // PRE_ADD_NEWCOMEBACK

	SPartyMemberRaidUIData();
	virtual void InitInfoList();
	void SetUserLevel(int nLevel);
	void SetJobClass(int nClass);
	void SetCoin(int freeCoin, int cashCoin);
	void SetName(const WCHAR* pNameString, bool bRed);
	void SetMaster(bool bMaster);
	void SetInvitationCtrl(CEtUIStatic* pStatic, CEtUIStatic* pTextStatic);
	void ShowInvitationCtrl(bool bShow);
#ifdef PRE_ADD_NEWCOMEBACK
	void SetComeback(bool bShow);
#endif // PRE_ADD_NEWCOMEBACK
	bool IsShowInvitationCtrl() const		{ if (pStaticInvitation) return pStaticInvitation->IsShow(); return false; }
	bool IsInsideItem(float fX, float fY);
	void Enable(bool bEnable);
	CEtUIControl* GetControl() const	{ return pBase; }
};

struct SPartyTargetMapInfo
{
	std::wstring	mapName;
	int				mapId;
	int				minPartyNum;
	int				maxPartyNum;
#ifdef PRE_PARTY_DB
	eTargetStageFilterType filterType;

	bool operator==(const SPartyTargetMapInfo& rhs) const
	{
		return (mapId == rhs.mapId);
	}

	const SPartyTargetMapInfo& operator=(const SPartyTargetMapInfo& rhs)
	{
		mapName = rhs.mapName;
		mapId = rhs.mapId;
		minPartyNum = rhs.minPartyNum;
		maxPartyNum = rhs.maxPartyNum;
		filterType = rhs.filterType;

		return *this;
	}
#endif

	bool operator<(const SPartyTargetMapInfo& rhs) const
	{
		return (_tcscmp(mapName.c_str(), rhs.mapName.c_str()) < 0);
	}

	bool IsValid() const
	{
		return (mapName.empty() == false && mapId >= 0);
	}
};

#define MAX_8RAID_GROUP			2
#define MAX_8RAID_GROUP_MEMBER	4

struct SRaidReGroupCtrlUnit
{
	UINT sessionId;

	CEtUIStatic* pBase;
	CEtUIStatic* pLevel;
	CEtUIStatic* pName;
	CDnJobIconStatic* pJobIcon;
	CEtUIStatic* pSelect;

	SRaidReGroupCtrlUnit();
	void InitUnit();
	void SetInfo(UINT sesId, const WCHAR* pszName, int level, int jobId);
	bool IsEmpty() const;
};

struct SRaidReGroupInfo
{
	CEtUIStatic* pTitle;
	std::vector<SRaidReGroupCtrlUnit> partyCtrlList;

	void InitInfo();
};

#ifdef PRE_PARTY_DB
struct SPartyCreateParam
{
	ePartyType PartyType;
	BYTE cPartyMemberMax;
	int nUserLvLimitMin;
	TPARTYITEMLOOTRULE ItemLootRule;
	TITEMRANK ItemLootRank;
	std::wstring wszPartyName;
	int iPassword;
	int nMapIdx;
	TDUNGEONDIFFICULTY Difficulty;
	BYTE cIsPartyDice;
	bool bAllowWorldZonePartyList;
	BYTE cUseVoice;
#ifdef _WORK
	bool bAddDummyParty;
#endif

	SPartyCreateParam()
	{
		PartyType = _NORMAL_PARTY;
		cPartyMemberMax = 4;
		nUserLvLimitMin = 1;
		ItemLootRule = ITEMLOOTRULE_INORDER;
		ItemLootRank = ITEMRANK_B;
		iPassword = 0;
		nMapIdx = 0;
		Difficulty = Dungeon::Difficulty::Easy;
		cIsPartyDice = 0;
		bAllowWorldZonePartyList = false;
		cUseVoice = 0;
#ifdef _WORK
		bAddDummyParty = false;
#endif
	}
};
#endif