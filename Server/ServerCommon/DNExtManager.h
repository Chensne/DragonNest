#pragma once

#include "DNDataManagerBase.h"
#include "DNTableFile.h"

#if defined(_LOGINSERVER)
#define FLAG_NICK_EQUAL		0
#define FLAG_NICK_INCLUDE	1

#define PROHIBITWORDMAX		64

struct ProhibitWordInfo
{
	unsigned int nFlag;
	wchar_t szNick[PROHIBITWORDMAX];
};

#endif	// _LOGINSERVER

enum GlobalWeightTableIndex		// village, game은 DnTableDB.h에 있기 때문에 serverdef보다는 이곳에 옮겨놨음
{
	CharacterSlotMax = 34,				// 34: 캐릭터 슬롯 개수제한
	CHARACTERNAME_MAXLENGTH = 1010,		//GlobalWeightTable에 있는 값을 가져오기때문에 인덱스 값임
	PVPCHANNEL_LIMITLEVEL = 1018,		//GlobalWeightTable에 있는 값을 가져오기때문에 인덱스 값임
	Login_Max_CreateCharCount = 1022,	// 계정당 최대생성 가능한 캐릭터 수량 (GlobalWeight 1022)
	Login_Base_CreateCharCount = 1023,	// 계정당 기본생성 캐릭터 수량 (GlobalWeight 1023)	
	BeginnerGuild_UserMax = 1026,		// 초보자길드 제한인원 1026
	BeginnerGuild_GiftMailID = 1027,	// 초보자길드 가입선물 1027
	MasterSystem_MasterAndPupilAllowLevelGap = 1038		// 1038	사제시스템_스승과제자레벨차이
};

enum GlobalWeightIntTableIndex		// village, game은 DnTableDB.h에 있기 때문에 serverdef보다는 이곳에 옮겨놨음
{
	ComebackTerm_Day = 10006,
	ComebackEffectItemID_Login = 10009,	
	SpecialBoxKeepPeriod = 10010,
	NewbieReConnectReward_Hour = 10011, 
};

class CDNExtManager
#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
: public CDNDataManagerBase
#endif	// #if defined(_LOGINSERVER) || defined(_MASTERSERVER)
{
private:
	typedef map<int, TMapInfo*> TMapInfoMap;
	TMapInfoMap m_pMapInfo;

	typedef map<int, TPCBangData*> TMapPCBangData;
	TMapPCBangData m_pPCBangData;

	typedef map<BYTE, TDefaultCreateData*> TMapDefaultCreate;	// first: ClassID
	TMapDefaultCreate m_pMapDefaultCreate;

	typedef map<int, int> TNewCharCreate;
	TNewCharCreate m_pMapNewCharCreate;
#if defined( PRE_ADD_DWC )
	typedef map<BYTE, TDWCCreateData*> TMapDWCCreate;	// first: JobCode
	TMapDWCCreate m_pMapDWCCreate;
#endif // #if defined( PRE_ADD_DWC )

	typedef map<int, TSkillData*> TMapSkillData;		// 기본적으로 unlock 된 스킬을 골라서 보유 스킬 리스트에 레벨 0으로 넣어줌.
	TMapSkillData m_pMapSkillData;

	map<int, vector<TSkillData*> > m_mapDefaultUnlockSkillsByClass;

	typedef map<BYTE, TCoinCountData*> TMapCoinCount;	// first: Level
	TMapCoinCount m_pMapCoinCount;

	typedef map<int, TLevelData*> TMapLevelData;
	TMapLevelData m_pLevelData;

#if defined(_LOGINSERVER)
	typedef std::map <unsigned int, std::wstring> TProhibitWord;
#if defined(PRE_ADD_MULTILANGUAGE)
	std::map <int, TProhibitWord> m_ProhibitWordList;
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	TProhibitWord m_ProhibitWordList;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
#endif	// #if defined(_LOGINSERVER)

	typedef map<int, TWeaponData*> TMapWeaponData;
	TMapWeaponData m_pWeaponData;

	typedef map<int, TPartData*> TMapPartData;
	TMapPartData m_pPartData;

	typedef map<UINT, TPvPGameStartConditionTable*>	TPvPGameStartConditionData;
	TPvPGameStartConditionData		m_PvPGameStartConditionTable;

	typedef map<UINT, TSchedule>	TScheduleData;
	TScheduleData					m_ScheduleTable;

	typedef map<UINT, TPvPGameModeTable> TPvPGameModeTableData;
	TPvPGameModeTableData			m_PvPGameModeTable;

	std::map<LadderSystem::MatchType::eCode,std::vector<int>>	m_MatchTypePvPGameModeID;
	typedef map<UINT, TPvPMapTable*>	TPvPMapTableData;
	TPvPMapTableData				m_PvPMapTable;

	typedef map<char, TGuildWarMapInfo> TGuildWarMapInfoData;
	TGuildWarMapInfoData			m_GuildWarMapInfo;

	typedef map<int, TMailTableData>		TMapMailTableData;
	TMapMailTableData		m_MailTableData;

	std::vector<TGuildWarRewardData *> m_pGuildWarRewardData;

	typedef map<int, TJobTableData>		TMapJobTableData;
	TMapJobTableData		m_JobTableData;

	typedef map<int, TCashCommodityData*> TMapCashCommodity;	// first: ItemSN
	TMapCashCommodity m_pMapCashCommodity;

	typedef map<int, TCashPackageData*> TMapCashPackage;	// first: ItemSN
	TMapCashPackage m_pMapCashPackage;

	typedef map<int, TCashLimitData*> TMapCashLimit;	// first: ItemSN
	TMapCashLimit m_pMapCashLimit;

	typedef map<int, TVehicleData> TMapVehicleData;	// first: ItemID
	TMapVehicleData m_MapVehicle;

	typedef map<int, TVehiclePartsData> TMapVehicleParts;	// first: ItemID
	TMapVehicleParts m_MapVehicleParts;

	typedef map<int, TItemData*> TMapItemData;
	TMapItemData m_pItemData;

	typedef map<int, TGlobalWeightData*> TMapGlobalWeight;	// first: ID
	TMapGlobalWeight m_pMapGlobalWeight;

	typedef map<int, TGlobalWeightIntData*> TMapGlobalWeightInt;	// first: ID
	TMapGlobalWeightInt m_pMapGlobalWeightInt;

	CSyncLock m_Sync;		//m_pMapCashCommodity와 m_pMapCashPackage보호
	bool LoadCashCommodity(bool bReload = false);
	bool LoadCashPackage(bool bReload = false);

	bool LoadCashLimit();
	bool LoadItemData();

	bool LoadPlayerLevel();
	bool LoadGlobalWeight();
	bool LoadGlobalWeightInt();

	// VehicleTable
	bool LoadVehicleData();
	// VehiclePartsTable
	bool LoadVehiclePartsData();
	bool LoadPvPGameStartConditionTable();
	bool LoadScheduleTable();
	bool LoadPvPGameModeTable();
	bool LoadGuildWarMapInfoTable();
	bool LoadPvPMapTable();
	bool LoadMailTableData();
	bool LoadGuildWarRewardData();	
	bool LoadJobTableData();

	unsigned int MemoryToRSHashKey(const wchar_t* pszData, long nSize);

	void FindExtFileList( const char *szFileName, std::vector<std::string> &szVecList );
	DNTableFileFormat* LoadExtTable( const char *szFileName );

public:
	struct PlayerLevelTableIndex{
		enum ePlayerLevelTableIndex {
			Strength,
			Agility,
			Intelligence,
			Stamina,
			Experience,
			SuperAmmor,
			SkillPoint,
			Fatigue,
			DeadDurabilityRatio,
			KillScore,
			AssistScore,
			AssistDecision,
			AggroPer,
			WeekFatigue,
			SPDecreaseRatio,
			Amount,
		};
	};

protected:
	struct LevelValue {
		union {
			int nValue[PlayerLevelTableIndex::Amount];
			float fValue[PlayerLevelTableIndex::Amount];
		};
	};
	std::map<int, LevelValue *> m_nMapTable;

public:
	CDNExtManager(void);
	~CDNExtManager(void);

	bool AllLoad();

	int GetValue( int nClassID, int nLevel, PlayerLevelTableIndex::ePlayerLevelTableIndex Index );
	float GetValueFloat( int nClassID, int nLevel, PlayerLevelTableIndex::ePlayerLevelTableIndex Index );

	TPvPGameStartConditionTable*	GetPvPGameStartConditionTable( const UINT uiMaxUser );
	TSchedule*						GetSchedule( int nSchedule );
	const TPvPGameModeTable*		GetPvPGameModeTableByMatchType( LadderSystem::MatchType::eCode MatchType );
	UINT							GetGuildWarMapInfoID(char cMonth);
	const TPvPMapTable*				GetPvPMapTable( const int nItemID );
	const TPvPGameModeTable*		GetPvPGameModeTable( const int nItemID );

	bool LoadMapInfo();

	//MapInfo
	int GetMapIndexByGateNo(int nMapIndex, int nGateNo, int nSelect=0);
	int GetGateNoByGateNo( int nMapIndex, int nGateNo, int nSelect=0);
	GlobalEnum::eMapTypeEnum GetMapType( int nMapIndex );
	const char * GetMapName(int nMapIdx);

#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
	void SetOnOffServerInfo(char cWorldSetID, bool bOnline);

	int GetWorldID();
	WCHAR* GetWorldName(int nWorldID);
	bool GetLocalIp(OUT std::vector<std::string>& Out);
#endif	// #if defined(_LOGINSERVER) || defined(_MASTERSERVER)

#if defined(_LOGINSERVER)
	void GetChannelListByMapIndex(char cWorldID, int nMapIndex, int nLastMapIndex, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList);	
	void GetServerList(TServerListData *ServerList, BYTE &cCount, bool DenyWorld=false);

	// DefaultCreateData
	bool LoadDefaultCreateData();
	bool LoadSkillData();
	TDefaultCreateData* GetDefaultCreateData(BYTE cClassID);
	TDefaultCreateData* GetDefaultCreateData_2(BYTE cClassID);
//	TDefaultCreateData* GetDefaultCreateData(BYTE cClassID,bool bIsDarkClass);
	int GetCreateDefaultItem(BYTE cClassID, TCreateCharacterItem *CreateItemArray);
	bool IsDefaultParts(BYTE cClassID, char cEquipType, int nPartID);
	bool IsDefaultHairColor(BYTE cClassID, DWORD dwColor);
	bool IsDefaultSkinColor(BYTE cClassID, DWORD dwColor);
	bool IsDefaultEyeColor(BYTE cClassID, DWORD dwColor);

	bool GetCreateDefaultPosition(BYTE cClassID, int &nPosX, int &nPosY);
	bool GetCreateDefaultMapInfo(BYTE cClassID, int &nMapIndex, int &nTutorialMapIndex, char &cTutorialGateNo);
	bool GetCreateDefaultSkill(BYTE cClassID, int *CreateSkillArray);
	bool GetCreateDefaultQuickSlot(BYTE cClassID, TQuickSlot *QuickSlotArray);
	bool GetCreateDefaultGesture(BYTE cClassID, int * CreateGestureArr);
	bool GetCreateDefaultUnlockSkill(BYTE cClassID, int *CreateUnlockSkillArray);
	//rlkt_dark
	bool GetCreateDarkSecondClassID(BYTE cClassID, int &ClassID);

	// DefaultCreateData
#if defined( PRE_ADD_DWC )
	bool LoadDWCCreateData();
	TDWCCreateData* GetDWCCreateData(BYTE cJobCode);

	int GetCreateDWCItem(BYTE cJobCode, TCreateCharacterItem *CreateItemArray);
	bool GetCreateDWCPosition(BYTE cJobCode, int &nPosX, int &nPosY);
	bool GetCreateDWCEquip(BYTE cJobCode, int * CreateEquipArray);
	bool GetCreateDWCSkill(BYTE cJobCode, int *CreateSkillArray);
	bool GetCreateDWCQuickSlot(BYTE cJobCode, TQuickSlot *QuickSlotArray);
	bool GetCreateDWCGesture(BYTE cJobCode, int * CreateGestureArr);
#endif // #if defined( PRE_ADD_DWC )

	// CoinCountByLevel
	bool LoadCoinCount();
	int GetRebirthCoin(BYTE cLevel, char cWorldID);
	int GetCashRebirthCoin(BYTE cLevel, char cWorldID);

	bool LoadLevelData();
	int GetFatigue(char cClass, char cLevel);

	// 금칙어
#if defined(PRE_ADD_MULTILANGUAGE)
	bool LoadProhibitWord(int nLanguage, const char * pszFilePath);
	TProhibitWord * GetLanguageSlangFilter(int nLanguage);
	bool CheckProhibitWord(int nLanguage, const wchar_t *pwszWord);
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	bool LoadProhibitWord();
	bool CheckProhibitWord(const wchar_t *pwszWord);
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	bool CheckSpecialCharacter(const wchar_t *pwszWord);

	bool LoadWeaponData();
	TWeaponData* GetWeaponData(int nWeaponIndex);

	bool LoadPartData();
	TPartData* GetPartData(int nPartIndex);

	int GetItemDurability( int nItemID );
#endif

	// PCBang
	bool LoadPCBangData();
	int GetPCBangDataCount();
	TPCBangData *GetPCBangData(int nPCBangType);

	int GetPCBangParam1(int nPCBangType, int nPCBangGrade);	// 데이터 1줄만 있는경우 (PCBang::Type::)Exp, Appellation, Fatigue, RebirthCoin, Mission
	int GetPCBangParam2(int nPCBangType, int nPCBangGrade);	// 데이터 1줄만 있는경우 (PCBang::Type::)Exp, Appellation, Fatigue, RebirthCoin, Mission
	int GetPCBangNestClearCount(int nPCBangGrade, int nMapID);
	bool GetPCBangClearBoxFlag(int nPCBangGrade);

	// Cash쪽
	bool ReLoadCashCommodity();
	bool ReLoadCashPackage();

	bool GetCashCommodityData(int nSN, TCashCommodityData &Data);
	int GetCashCommodityPrice(int nSN);
	bool IsReserveCommodity(int nSN);	// 적립금 가능 상품인지
	bool IsPresentCommodity(int nSN);	// 선물 가능 상품인지
	bool IsOnSaleCommodity(int nSN);	// 판매 가능 상품인지
	bool IsLimitCommodity(int nSN);		// 한정판매인지
	int GetCashCommodityPeriod(int nSN);
	int GetCashCommodityCount(int nSN);
	int GetCashCommodityReserve(int nSN);	// 구입시 주어지는 적립금
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int GetCashCommoditySeed(int nSN);	// 구입시 주어지는 시드
#endif
	bool GetCashCommodityName(int nSN, std::string &outStr);
	bool GetCashCommodityNameW(int nSN, std::wstring &outStr);
#if defined(_JP)
	bool GetCashCommodityJPSN(int nSN, std::string &outStr);
#endif	// #if defined(_JP)
	int GetCashCommodityItem0(int nSN);
	int GetCashCommodityVIPPoint(int nSN);	// vippoint얻기
	bool GetCashCommodityPay(int nSN);
	int GetCashCommodityPaySale(int nSN);
	int GetCashCommodityVIPAutomaticPaySalePrice(int nSN);	// VIP자동결제때 할인가격
	std::vector<int> m_VecProhibitSaleList;
	void SetProhibitSaleList();
#if defined(PRE_ADD_CASH_REFUND)
	bool GetCashCommodityNoRefund(int nID, int nSN); // 캐쉬 환불 불가능
#endif
#if defined(PRE_ADD_SALE_COUPON)
	bool GetCashCommodityUseCoupon(int nSN);
#endif

	bool GetCashPackageData(int nSN, TCashPackageData &Data);
	TCashLimitData *GetCashLimitData(int nSN);
	int GetCashBuyAbleCount(int nSN);
#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)
	int GetCashLimitedItemCount(int nSN);
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)

	TItemData* GetItemData(int nItemID);
	int GetItemMainType(int nItemID);		// 아이템 타입중에 eItemTypeEnum 인지
	int GetItemOverlapCount(int nItemID);
	bool IsCashItem(int nItemID);

	// GlobalWeight
	TGlobalWeightData *GetGlobalWeightData(int nID);
	float GetGlobalWeightValueFloat(int nID);
	int GetGlobalWeightValue(int nID);
	TGlobalWeightIntData *GetGlobalWeightIntData(int nID);
	float GetGlobalWeightIntValueFloat(int nID);
	int GetGlobalWeightIntValue(int nID);

	// VehicleTable
	TVehicleData *GetVehicleData(int nItemID);
	int GetVehicleClassID(int nItemID);

	// VehiclePartsTable
	TVehiclePartsData *GetVehiclePartsData(int nItemID);
	int GetVehiclePartsType(int nItemID);
	TMailTableData*		GetMailTableData( int iItemID );
	TGuildWarRewardData* GetGuildWarRewardData(char cType, char cClass);
	TJobTableData* GetJobTableData( int nJobTableID );

	time_t GetTimeForTextDate(std::string& strDate);  // "00/00/00" Text에서 time_t 얻어오기
	bool IsOnSaleDate(int nSN);

	void GetXMLAccountDBID(UINT nAccountDBID, std::wstring &wstrOut);
	void GetXMLAccountName(WCHAR *pAccountName, std::wstring &wstrOut);
	void GetXMLCharacterDBID(INT64 biCharacterDBID, std::wstring &wstrOut);
	void GetXMLCharacterName(WCHAR *pCharacterName, std::wstring &wstrOut);
	void GetXMLItems(int nItemTotalCount, TSpecialBoxItemInfo *Items, std::wstring &wstrOut);


	///
	bool LoadNewCharCreate();
};

extern CDNExtManager *g_pExtManager;