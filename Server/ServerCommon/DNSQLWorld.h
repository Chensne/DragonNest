#pragma once

/* -----------------------------------------
	Login, DB, Cash 가 같이 쓰는 파일임
----------------------------------------- */

#include "SQLConnection.h"

class CDNSQLWorld: public CSQLConnection
{
public:
	CDNSQLWorld(void);
	virtual ~CDNSQLWorld(void);

	int QueryGetDatabaseVersion( TDatabaseVersion* pA );

	int QueryLoginCharacter( TQLoginCharacter* pQ );
	int QueryLogoutCharacter( TQLogoutCharacter* pQ );

	int QuerySelectCharacter(int nWorldSetID, INT64 biCharacterDBID, UINT nLastServerType, OUT TASelectCharacter *pA, UINT nAccountDBID = 0);
	int QueryMoveIntoNewServer(TQUpdateCharacter *pQ);	// 서버 장비 이동(쿨 타임, 퀵 슬롯, 캐릭터 위치 저장) - P_MoveIntoNewServer
	int QueryMoveIntoNewZone(TQChangeStageUserData *pQ, OUT BYTE &cRebirthCoin, OUT BYTE &cPCBangRebirthCoin);		// Zone 이동(획득 경험치, 획득 코인, 사용한 일반/PC방 부활 코인) - P_MoveIntoNewZone
	// 캐릭터 생성
#if defined(_LOGINSERVER)
	int QueryAddCharacter(INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, char cAccountLevel, int nWorldID, WCHAR *pCharName, char cClass, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, 
		int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, 
		TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, bool bJoinBeginnerGuild, UINT &nGuildDBID, WCHAR* pwszIP );	// 캐릭터 추가(기본 속성) - P_AddCharacter, P_FirstUseCharacter, P_AddMaterializedItem
#if defined( PRE_ADD_DWC )
	int QueryAddDWCCharacter(INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, int nWorldID, WCHAR *pCharName, char cClass, char cJobCode1, char cJobCode2, char cLevel, int nExp, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, 
		int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, 
		TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, WCHAR* pwszIP, short nSkillPoint, int nGold );	// DWC 캐릭터 추가 - P_AddDWCCharacter, P_FirstUseCharacter, P_AddMaterializedItem
	int QueryGetDWCCharacterID(UINT nAccountDBID, OUT INT64& biDWCCharacterDBID, int nDeleteWaitingTime);
#endif // #if defined( PRE_ADD_DWC )
#endif	// #if defined(_LOGINSERVER)
	int QueryRollbackAddCharacter(INT64 biCharacterDBID);	// 캐릭터 추가 롤백 - P_RollbackAddCharacter
	int QueryFirstUseCharacter(INT64 biCharacterDBID, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray);	// 캐릭터 부가 속성 세팅(캐릭터 첫 사용시 호출) - P_FirstUseCharacter
	// 캐릭터 삭제
	int QueryDelCharacter(INT64 biCharacterDBID, OUT bool &bVillageFirstVisit, OUT TIMESTAMP_STRUCT& DeleteDate );	// 캐릭터 삭제 - P_DelCharacter
	int QueryReviveCharacter( INT64 biCharacterDBID );
	// 캐릭터 속성 조회
#if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList, INT64 biDWCCharacterDBID, BYTE cAccountLevel);	// 캐릭터 목록 - P_GetListCharacter
#else // #if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList);	// 캐릭터 목록 - P_GetListCharacter
#endif // #if defined( PRE_ADD_DWC )
#else	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT TDBListCharData *CharList, INT64 biDWCCharacterDBID, BYTE cAccountLevel);	// 캐릭터 목록 - P_GetListCharacter
#else // #if defined( PRE_ADD_DWC )
	int QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT TDBListCharData *CharList);	// 캐릭터 목록 - P_GetListCharacter
#endif // #if defined( PRE_ADD_DWC )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	int QueryGetCharacter(int nWorldSetID, INT64 biCharacterDBID, UINT nLastServerType, OUT TASelectCharacter *pA);	// 캐릭터 조회 - P_GetCharacter
	int QueryGetCharacterPartialy1(INT64 biCharacterDBID, WCHAR *pCharName, TAGetCharacterPartialy *pA);	// select 코드 1번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 이름 or 캐릭터 ID) - P_GetCharacterPartialy
	int QueryGetCharacterPartialy4(INT64 biCharacterDBID, WCHAR *pCharName, OUT BYTE &cClassID, OUT BYTE &cLevel);	// select 코드 4번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 이름 or 캐릭터 ID) - P_GetCharacterPartialy
	int QueryGetCharacterPartialy6(WCHAR *pCharName, OUT BYTE &cJob, OUT BYTE &cLevel);	// select 코드 6번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 이름 or 캐릭터 ID) - P_GetCharacterPartialy
	int QueryGetCharacterPartialy7(WCHAR *pCharName, OUT UINT &nAccountDBID, OUT INT64 &biCharacterDBID);	// select 코드 7번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 이름) - P_GetCharacterPartialy
	int QueryGetCharacterPartialy8(INT64 biCharacterDBID, WCHAR *pCharName, int &nWorldID);	// select 코드 8번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 ID) - P_GetCharacterPartialy
	int QueryGetCharacterPartialy10(WCHAR *pCharName, OUT char* pAccountName);	// select 코드 10번인경우 : 캐릭터 속성 일부 조회(by 캐릭터 ID) - P_GetCharacterPartialy
	int QueryGetListEquipmentAttributes(INT64 biCharacterDBID, OUT TCharacterStatus &CharStatus);	// Equip 추가 속성 목록 - P_GetListEquipmentAttributes
	int QueryGetEquipmentAttribute(INT64 biCharacterDBID, char cEquipTimeType, char cEquipAttributeCode, int nValue, OUT int &nEquipAttributeValue);	// Equip 추가 속성 조회(equipdelaytime, equipremaintime) - P_GetEquipmentAttribute 
	int QueryGetCharacterLevelList(UINT nAccountDBID, int nWorldID, std::vector<TChracterLevel> &VecLevelList);
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	int QueryGetCharacterCheckSum(INT64 biCharacterDBID, OUT TASelectCharacter *pA);
	int QueryMakeCharacterCheckSum(TQHeader * pHeader, BYTE cReason, INT64 biCharacterDBID, OUT UINT& uiCheckSum, INT64 biChangeCoin = 0);
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	int QueryGetTotalSkillLevel(TQGetTotalLevelSkill* pQ, TAGetTotalLevelSkill* pA);
	int QueryGetTotalSkillList(TQGetTotalLevelSkill* pQ, TAGetTotalLevelSkill* pA);
	int QueryAddTotalSkill(TQAddTotalLevelSkill* pQ);
#endif
#ifdef PRE_ADD_PRESET_SKILLTREE
	int QueryGetSKillPresetIndexList(TQGetSKillPreSetList * pQ, TAGetSKillPreSetList * pA);
	int QueryGetSKillPresetList(TQGetSKillPreSetList * pQ, BYTE cIndex, TSkillSetPartialData * pSkill, BYTE &cCount);
	int QueryAddSkillPreset(TQAddSkillPreSet * pQ);
	int QueryDelSkillPreset(TQDelSkillPreSet * pQ);
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
	// 캐릭터 속성 변경
	int QueryVisitFirstVillage(INT64 biCharacterDBID);	// 캐릭터의 첫 빌리지 방문 기록 - P_VisitFirstVillage
	int QueryModCharacterLevel(INT64 biCharacterDBID, char cLevelChangeCode, BYTE cLevel, int nChannelID, int nMapID);	// 캐릭터 레벨 변경 - P_ModCharacterLevel
	int QueryModCharacterExp(INT64 biCharacterDBID, char cExpChangeCode, int nExp, int nChannelID, int nMapID, INT64 nFKey);	// 캐릭터 경험치 변경 - P_ModCharacterExp
	int QueryModCoin(INT64 biCharacterDBID, char cCoinChangeCode, INT64 biCoinChangeKey, INT64 biCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin);	// 코인 증가 or 코인 직접 수정 - P_ModCoin
	int QueryMidtermCoin(INT64 biCharacterDBID, INT64 biPickUpCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin);	// 스테이지 골드 사용시 픽업코인 중간정산
	int QueryModWarehouseCoin(INT64 biCharacterDBID, char cCoinChangeCode, INT64 biCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin, OUT INT64 &biCurrentWarehouseCoin);	// 창고 코인 입/출금 - P_ModWarehouseCoin

	int QueryModCharacterStatus(INT64 biCharacterDBID, int nModFieldBit, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, int nMapID, int nLastVillageMapID, int nLastDarkLairVillageMapID, char cLastVillageGateNo, 
		TPosition &Pos, float fRotate, bool bViewCashWeapon1, bool bViewCashWeapon2, bool bLastLogoutDate, bool bLastLoginDate, char cSkillPage);	// 캐릭터 Status 변경 - P_ModCharacterStatus
	int QueryUpsCharacterAbility(INT64 biCharacterDBID, char cAbilityCode, int nAbilityPoint);	// 캐릭터 추가 능력치(영구적용아이템) UPSERT - P_UpsCharacterAbility
	int QueryUpsEquipmentAttribute(INT64 biCharacterDBID, char cEquipTimeType, char cEquipAttributeCode, int nValue);	// Equip 추가 속성 UPSERT - P_UpsEquipmentAttribute
	int QueryModFatigue(TQFatigue *pQ);	// 피로도 증가 - P_ModFatigue
	int QueryModEventFatigue(TQEventFatigue *pQ);	// 이벤트 피로도 변경 - P_ModEventFatigue
	int QueryModRebirthCoin(INT64 biCharacterDBID, int nRebirthCoin, int nPCBangRebirthCoin);	// P_ModRebirthCoin	

	// 캐쉬 부활 코인
	int QueryAddCashRebirthCoin(INT64 biCharacterDBID, int nRebirthCount, int nRebirthPrice, int nRebirthCode, int nRebirthKey);	// 캐쉬 부활 코인 충전(Cash Server가 호출) - P_AddCashRebirthCoin
	int QueryUseCashRebirthCoin(INT64 biCharacterDBID, int nChannelID, int nMapID, const WCHAR* pwszIP );	// 캐쉬 부활 코인 사용 - P_UseRebirthCoin
	int QueryAddEffectItems( TQAddEffectItems* pQ );
	int QueryGetListEffectItem( INT64 biCharacterDBID, std::vector<TEffectItemInfo>& vEffectItem );
	int QueryGetProfile(INT64 biCharacterDBID, TProfile& Profile);
	int QuerySetProfile(INT64 biCharacterDBID, TProfile& Profile);
	int QueryAddAbuseLog(TQAddAbuseLog* pQ);
	int QueryAddAbuseMonitor( TQAddAbuseMonitor* pQ );
	int QueryDelAbuseMonitor( TQDelAbuseMonitor* pQ );
	int QueryGetAbuseMonitor( TQGetAbuseMonitor* pQ, TAGetAbuseMonitor* pA );
	int QueryGetWholeAbuseMonitor(TQGetWholeAbuseMonitor * pQ, TAGetWholeAbuseMonitor * pA);

	// 아이템
#if defined(_LOGINSERVER)
	void MakeAddMaterializedItem(int nMapIndex, INT64 biCharacterDBID, int nSlotIndex, int nItemLocation, int nItemID, int nCount, int nRandomSeed, int nDur, OUT TQAddMaterializedItem &Item);
#endif //#if defined(_LOGINSERVER)
	int QueryAddMaterializedItem( TQAddMaterializedItem* pQ );		// 아이템 획득 - P_AddMaterializedItem
	int QueryUseItem( TQUseItem* pQ );		// 아이템 소멸(소모성 아이템 사용/버리기/분해) - P_UseItem
	int QueryResellItem( TQResellItem* pQ );	// 아이템 되팔기 - P_ResellItem
	int QueryEnchantItem( TQEnchantItem* pQ );	// 아이템 강화 - P_EnchantItem
	int QueryChangeItemLocation( TQChangeItemLocation* pQ/*, TAChangeItemLocation* pA*/ );	// 아이템 위치 변경 - P_ChangeItemLocation
	int QuerySwitchItemLocation(TQSwitchItemLocation *pQ);	// 두 아이템의 위치 바꾸기 - P_SwitchItemLocation
	int QueryModItemDurability( TQModItemDurability* pQ );	// 내구도 일괄 변경 - P_ModItemDurability
	int QueryModMaterializedItem( TQModMaterializedItem* pQ );	// 아이템 속성 변경 - P_ModMaterializedItem
	int QueryGetListMaterializedItem( INT64 biCharacterDBID, std::wstring wstrItemLocationCode, int nMaxCount, int &nCount, DBPacket::TMaterialItemInfo *ItemList, UINT nAccountDBID = 0 );	// 소유한 아이템 목록 - P_GetListMaterializedItem
	int QueryExchangeProperty(TQExchangeProperty *pQ);	// 아이템/코인 교환 - P_ExchangeProperty
	int QueryTakeCashEquipmentOff(TQTakeCashEquipmentOff *pQ);	// 17.캐쉬 이큅먼트 일괄 탈착 - P_TakeCashEquipmentOff
	int QueryPutOnepieceOn(TQPutOnepieceOn *pQ);	// 18.한벌옷 장착 - P_PutOnepieceOn
	int QueryGetPageMaterializedItem(TQGetPageMaterializedItem *pQ, TAGetPageMaterializedItem *pA);	// 11.소유한 아이템 목록 페이징(공간 제약 없는 종류) - P_GetPageMaterializedItem
	int QueryGetCountMaterializedItem(INT64 biCharacterDBID, int nItemLocationCode, OUT int &nTotalCount);	// 12.소유한 아이템 목록 페이징용 카운트 - P_GetCountMaterializedItem
	int QueryGetPagePet(INT64 biCharacterDBID, int nPageNumber, int nPageSize, int nPetType, bool bEquipFlag, std::vector<DBPacket::TVehicleInfo> &VecItemList);	// 15.소유한 탈것 & 펫 목록 페이징 - P_GetPagePet
	int QueryGetCountPet(INT64 biCharacterDBID, int nPetType, OUT int &nTotalCount);	// 16.소유한 탈것 & 펫 카운트 - P_GetCountPet
	int QueryDelPetEquipment(INT64 biBodySerial, INT64 biPartsSerial, WCHAR *pIp);	// 25.탈것&펫에 장착한 파츠 삭제(버리기) - P_DelPetEquipment

	int QueryModPetSkinColor(INT64 biBodySerial, DWORD dwSkinColor, DWORD dwSkinColor2);	// 24.탈것&펫 색상 변경 - P_ModPetSkinColor 색상2 추가.
	int QueryModItemExpireDate(TQModItemExpireDate* pQ);
	int QueryModPetExp(TQModPetExp* pQ);
	int QueryItemExpireByCheat(TQItemExpire* pQ);
	int QueryChangePetName(INT64 petSerial, const wchar_t* pName);
	int QueryModPetSkill(TQModPetSkill *pQ);
	int QueryModPetSkillExpand(INT64 biItemSerial);
	int QueryModDegreeOfHunger(TQModDegreeOfHunger* pQ);
	int QueryModAdditiveItem(TQModAdditiveItem* pQ, TAModAdditiveItem* pA);
#if defined (PRE_ADD_COSRANDMIX)
	int QueryModRandomItem(TQModRandomItem* pQ, TAModRandomItem* pA);
#endif
	int QueryGetListMissingItem(INT64 biCharacterDBID, char &cCount, TItem *MissingItemList);	// 26.유실 아이템 목록 - P_GetListMissingItem
	int QueryRecoverMissingItem(INT64 biCharacterDBID, INT64 biItemSerial, char cItemLocationCode, BYTE cSlotIndex);	// 27.유실 아이템 복구 - P_RecoverMissingItem
	int QueryItemMaterializeFKey(INT64 biItemSerial, OUT BYTE &cItemCode, OUT INT64 &biFKey);		// 28.ItemMaterializeFKey 조회 - P_GetItemMaterializeFKey
	int QuerySaveItemLocationIndex(TQSaveItemLocationIndex *pQ);	// P_SaveItemLocationIndex
	int QueryGetListItemLocationIndex(INT64 biCharacterDBID, int &nCount, DBPacket::TItemIndexSerial *ItemIndexList);
	int QueryDeleteExpireItem(INT64 biCharacterDBID, std::list<INT64> &DelExpireitemList);
	int QueryDelCashItem(TQDelCashItem *pQ, TADelCashItem *pA);
	int QueryRecoverCashItem(TQRecoverCashItem *pQ, TARecoverCashItem *pA);
#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	int QueryCheckNamedItemCount(TQCheckNamedItemCount *pQ, TACheckNamedItemCount *pA);
#endif
#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
	int QueryMovePotential(TQMovePotential * pQ, TAMovePotential * pA);
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)

#if defined( PRE_ADD_LIMITED_SHOP )
	int QueryGetDailyLimitedShopItem(TQGetLimitedShopItem * pQ, TAGetLimitedShopItem * pA);
	int QueryGetWeeklyLimitedShopItem(TQGetLimitedShopItem * pQ, TAGetLimitedShopItem * pA);
	int QueryAdddailyLimitedShopItem(TQAddLimitedShopItem * pQ);
	int QueryAddWeeklyLimitedShopItem(TQAddLimitedShopItem * pQ);
	int QueryResetdailyLimitedShopItem(TQResetLimitedShopItem * pQ);
	int QueryResetWeeklyLimitedShopItem(TQResetLimitedShopItem * pQ);
#endif
	int QueryDelEffectItem(TQDelEffectItem * pQ);
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	int QueryModTalismanSlotOpenFlag(TQModTalismanSlotOpenFlag *pQ);
#endif

	// 미션
	int QueryGetListDailyWeeklyMission(INT64 biCharacterDBID, TUserData &UserData);		// 일일, 주간 미션 목록 - P_GetListDailyWeeklyMission
	int QuerySetDailyWeeklyMission(INT64 biCharacterDBID, char cDailyMissionType, int *nMissionIDs);	// 일일, 주간 미션 재설정 - P_SetDailyWeeklyMission
	int QueryModDailyWeeklyMissionDetail(INT64 biCharacterDBID, char cDailyMissionType, int nMissionIndex, TDailyMission &Mission, int nMissionScore);	// 일일, 주간 미션 세부 속성 변경 - P_ModDailyWeeklyMissionDetail
	int QueryUpsGainMissionBin(INT64 biCharacterDBID, char *GainMissionList);	// 획득 가능한 미션 비트맵 저장 - P_UpsGainMissionBin
	int QueryAddAchieveMission(INT64 biCharacterDBID, int nMissionID, char cMissionType, char *MissionAchieveList);	// 미션 달성 - P_AddAchieveMission
	int QueryDelAllMissionsForCheat(INT64 biChracterDBID);

	// 퀘스트
private:
	void ConvertToBinary(int questID, OUT char CompleteQuest[]);

public:
	int QueryGetCompletedQuests(INT64 biCharacterDBID, OUT char CompleteQuest[]);
	int QueryGetListQuest(INT64 biCharacterDBID, OUT int &nCount, OUT TQuest *QuestList);	// 퀘스트 목록 - P_GetListQuest
	int QueryAddQuest(INT64 biCharacterDBID, char cQuestIndex, TQuest &Quest);	// 퀘스트 추가 - P_AddQuest
	int QueryDelQuest(INT64 biCharacterDBID, int nQuestIndex, int nQuestID, bool bCompleteFlag, bool bRepeatFlag, char *CompleteQuestArray);	// 퀘스트 삭제 - P_DelQuest
	int QueryModQuest(INT64 biCharacterDBID, char cQuestIndex, int nModFieldBit, char cQuestStatusCode, TQuest &Quest);	// 퀘스트 수정 - P_ModQuest
	int QueryClearQuest(INT64 biCharacterDBID);	// 퀘스트 초기화 - P_ClearQuest
	int QueryAddCompleteQuest(INT64 biCharacterDBID, int nQuestID, int nQuestCode);

	// 퀵슬롯
	int QuerySetQuickSlot(INT64 biCharacterDBID, TQuickSlot *QuickSlotArray);	// 캐릭터 퀵 슬롯 MERGE - P_SetQuickSlot
	int QueryGetListQuickSlot(INT64 biCharacterDBID, OUT int &nCount, OUT TQuickSlot *QuickSlotList);	// 퀵 슬롯 목록 - P_GetListQuickSlot
	// 칭호
	int QueryAddAppellation(INT64 biCharacterDBID, int nAppellationID, char *AppellationArray);	// 칭호 추가 - P_AddAppellation
	int QueryChoiceAppellation(INT64 biCharacterDBID, int nAppellationID, char cAppellationKind);	// 대표 칭호 선택 - P_ChoiceAppellation
	int QueryDeleteAppellation(INT64 biCharacterDBID, int nAppellationID);	// 칭호 삭제
	int QueryGetListAchieveMission(INT64 biCharacterDBID, char *MissionAchieve);	// P_GetListAchieveMission
	int QueryGetListAppellation(INT64 biCharacterDBID, char *Appellation);	// P_GetListAppellation
	// 직업
	int QueryModJobCode(INT64 biCharacterDBID, char cJob);	// 전직 - P_ModJobCode
	int QueryGetListJobChangeLog(INT64 biCharacterDBID, OUT BYTE *JobArray);	// 직업 이력 - P_GetListJobChangeLog
	// 스킬
		
	int QueryAddSkill(INT64 biCharacterDBID, int nSkillID, char cSkillLevel, int nCoolTime, char cSkillChangeCode, INT64 biCoin, int nChannelID, int nMapID, char cSkillPage);	// 스킬 추가 - P_AddSkill
	int QueryIncreaseSkillPoint(INT64 biCharacterDBID, short nSkillPoint, int nChannelID, int nMapID, int nChangeCode, char cSkillPage);	// 스킬 포인트 증가 - P_IncreaseSkillPoint
	int QueryDelSkill(INT64 biCharacterDBID, int nSkillID, char cSkillChangeCode, bool bSkillPointBack, int nChannelID, int nMapID, char cSkillPage);	// 스킬 삭제 - P_DelSkill
	int QueryModSkillLevel(INT64 biCharacterDBID, int nSkillID, char cSkillLevel, int nCoolTime, int nUsedSkillPoint, char cSkillChangeCode, int nChannelID, int nMapID, char cSkillPage);	// 스킬 레벨 변경 - P_ModSkillLevel
	int QueryResetSkill(INT64 biCharacterDBID, int nChannelID, int nMapID, OUT USHORT &wSkillPoint, OUT USHORT &wTotalSkillPoint, char cSkillPage);	// 스킬 리셋 - P_ResetSkill
	int QueryGetListSkill(INT64 biCharacterDBID, OUT int &nCount, OUT TSkillGroup *SkillGroup);	// 스킬 목록 - P_GetListSkill
	int QueryUseExpandSkillPage(INT64 biCharacterDBID, int *nSkillID, int nSkillPoint2);	// 스킬트리확장 아이템 사용시 스킬과 스킬포인트 저장

	int QueryDecreaseSkillPoint( TQDecreaseSkillPoint* pQ );
	int QueryResetSkillBySkillIDs( TQResetSkillBySkillIDS* pQ, TAResetSkillBySkillIDS* pA );
	int QuerySetSkillPoint( TQSetSkillPoint* pQ );

	// 알리미 
	int QueryGetListNotifier(INT64 biCharacterDBID, OUT int &nCount, OUT DNNotifier::Data *NotifierList);	// 알리미 목록 - P_GetListNotifier
	int QueryModNotifier(INT64 biCharacterDBID, char cNotifyIndex, char cNotifyTargetType, int nNotifyTargetID);	// 알리미 수정 - P_ModNotifier
#if !defined(PRE_DELETE_DUNGEONCLEAR)
	// 던전
	int QueryUpsDungeonClear(INT64 biCharacterDBID, int nMapID, char cDifficultyCode);	// 던전 클리어 상태 UPSERT - P_UpsDungeonClear
	int QueryGetListDungeonClear(INT64 biCharacterDBID, OUT int &nCount, OUT TDungeonClearData *DungeonList);	// 캐릭터의 던전 클리어 목록 - P_GetListDungeonClear
	int QueryGetDungeonClear(INT64 biCharacterDBID, int nMapID, OUT char &cDifficulty);	// 특정 캐릭터가 특정 던전을 클리어한 난이도 조회 - P_GetDungeonClear
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)
	// Nest
	int QueryUpsNestClear(INT64 biCharacterDBID, int nMapID, char cClearType);	// NEST 클리어 횟수 UPSERT - P_UpsNestClear
	int QueryGetListNestClearCount(INT64 biCharacterDBID, OUT int &nCount, OUT TNestClearData *NestList);	// NEST 주간 클리어 횟수 조회 - P_GetListNestClearCount
	int QueryInitNestClearCount(INT64 biCharacterDBID);	// NEST 주간 클리어 횟수 초기화 - P_InitNestClearCount
	// 친구
	int QueryGetListFriendGroup(INT64 biCharacterDBID, OUT int &nCount, OUT TDBFriendGroupData *GroupList);	// 친구 그룹 목록 - P_GetListFriendGroup
	int QueryAddFriendGroup(INT64 biCharacterDBID, WCHAR *pGroupName, int nGroupCountLimit, OUT UINT &nGroupID);	// 친구 그룹 추가 - P_AddFriendGroup
	int QueryModFriendGroupName(INT64 biCharacterDBID, int nGroupID, WCHAR *pGroupName);	// 친구 그룹 이름 수정 - P_ModFriendGroupName
	int QueryDelFriendGroup(INT64 biCharacterDBID, int nGroupID);	// 친구 그룹 삭제 - P_DelFriendGroup
	int QueryGetListFriend(INT64 biCharacterDBID, OUT BYTE &cCount, OUT TDBFriendData *FriendList);	// 친구 목록 - P_GetListFriend
	int QueryAddFriend(INT64 biCharacterDBID, WCHAR *pFriendCharName, WCHAR *pFriendMemo, int nFriendGroupID, int nFriendCountLimit, OUT INT64 &biFriendCharDBID, OUT UINT &nFriendAccountDBID, OUT bool &bNeedDelIsolate);	// 친구 추가 - P_AddFriend
	int QueryModFriendMemo(INT64 biCharacterDBID, INT64 biFriendCharacterDBID, WCHAR *pMemo);	// 친구 메모 수정 - P_ModFriendMemo
	int QueryModFriendAndFriendGroupMapping(INT64 biCharacterDBID, int nFriendCount, INT64 *biFriendCharacterDBIDs, int nGroupID);	// 친구와 친구 그룹 매핑 수정 - P_ModFriendAndFriendGroupMapping
	int QueryDelFriend(INT64 biCharacterDBID, int nFriendCount, INT64 *biFriendCharacterDBIDs);	// 친구 삭제 - P_DelFriend
	// 차단
	int QueryAddBlockedCharacter(INT64 biCharacterDBID, WCHAR *pBlockedCharName, OUT INT64 &biBlockedCharacterDBID, OUT bool &bNeedDelFriend, int nIsolateCountMax);	// 캐릭터 차단 - P_AddBlockedCharacter
	int QueryGetListBlockedCharacter(INT64 biCharacterDBID, OUT BYTE &cCount, OUT TIsolateItem * pIsolateItems);	// 캐릭터 차단 목록 - P_GetListBlockedCharacter
	int QueryDelBlockedCharacter(INT64 biCharacterDBID, UINT uiCount, INT64 *biBlockedCharacterDBIDs);	// 캐릭터 차단 해지 - P_DelBlockedCharacter
	// Equipment 
	int QueryGetListEquipment(INT64 biCharacterDBID, int *EquipList, int *CashEquipList);	// Equipment의 아이템 목록 - P_GetListEquipment
	int QueryUpsEquipment(INT64 biCharacterDBID, int nItemLocation, int nSlotIndex, INT64 biSerial);

	// 우편
	int QueryGetCountSendMail(INT64 biCharacterDBID, OUT int &nPremiumMailCount, OUT int &nBasicMailCount);	// 보낸 메일 카운트 - P_GetCountSendMail
	int QuerySendMail( TQSendMail* pQ, TASendMail* pA );	// 메일 발송 - P_SendMail
	int QuerySendSystemMail( TQSendSystemMail* pQ, TASendSystemMail* pA );	// 시스템 메일 발송 - P_SendSystemMail
#ifdef PRE_ADD_JOINGUILD_SUPPORT
	int QueryGetGuildSupportRewardInfo(TQGuildSupportRewardInfo * pQ, TAGuildSupportRewardInfo * pA);
	int QuerySendGuildMail(TQSendGuildMail * pQ, TASendGuildMail * pA);
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
	int QueryGetCountReceiveMail( INT64 biCharacterDBID, OUT int &nTotalCount, OUT int &nNotReadCount, OUT int &n7DaysLeftCount );	// 받은 메일 카운트 - P_GetCountReceiveMail
	int QueryGetPageReceiveMail( TQGetPageReceiveMail* pQ, TAGetPageReceiveMail* pA );	// 받은 메일 목록 페이징 - P_GetPageReceiveMailc
	int QueryReadMail( TQReadMail* pQ, TAReadMail* pA );	// 메일 읽기 - P_ReadMail
	int QueryTakeMailAttachCoin(int nMailDBID, int nChannelID, int nMapID, OUT INT64 &biCoin, UINT nGuildDBID, OUT bool &bAddGuildWare);	// 첨부 코인 받기 - P_TakeMailAttachCoin
	int QueryTakeMailAttachItem(int nMailDBID, INT64 biSerial, int nChannelID, int nMapID, const WCHAR* pwszIP, OUT TItemInfo &ItemInfo, UINT nGuildDBID, OUT bool &bAddGuildWare);	// 첨부 아이템 받기 - P_TakeMailAttachItem
	int QueryDelMail( TQDelMail* pQ, TADelMail* pA );	// 메일 삭제 - P_DelMail
	int QueryGetListMailAttachment(TQGetListMailAttachment *pQ, TAGetListMailAttachment *pA);	// 메일 목록에 포함된 첨부 코인 및 아이템 수량 조회 - P_GetLisMailAttachment
	int QueryGetListMailAttachmentByMail(int nMailDBID, DBPacket::TAttachMail &MailList);	// P_GetListMailAttachmentByMail

#ifdef PRE_ADD_BEGINNERGUILD
	int QueryGetWillSendMail(TQGetWillMails * pQ, TAGetWillMails * pA);
	int QueryAddWillSendMail(INT64 biChatacterDBID, int nMailID, const WCHAR * pMemo);
	int QueryDelWillSendMail(INT64 biChatacterDBID, int nMailID);
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

#if defined(PRE_ADD_CADGE_CASH)
	int QuerySendWishMail(TQSendWishMail *pQ, TASendWishMail *pA);	// 조르기 우편 발송 - P_SendWishMail
	int QueryReadWishMail(TQReadWishMail *pQ, TAReadWishMail *pA);	// 조르기 우편 읽기 - P_ReadWishMail
	int QueryModWishProducts(int nMailDBID, INT64 biPurchaseOrderID);	// 조르기 선물 지급 상태 수정 (아이템 구입 주문 ID 입력) - P_ModWishProducts
#endif	// #if defined(PRE_ADD_CADGE_CASH)

	// 무인상점
	int QueryGetPageTrade(TQGetPageTrade *pQ, TAGetPageTrade *pA);	// 등록된 아이템 목록 페이징 - P_GetPageTrade
	int QueryGetCountTrade(TQGetPageTrade *pQ, OUT int &nTotalCount);	// 등록된 아이템 카운트 - P_GetCountTrade
	int QueryGetListMyTrade(TQGetListMyTrade *pQ, TAGetListMyTrade *pA);	// 내가 등록한 판매중 아이템 목록 - P_GetListMyTrade
	int QueryGetCountMyTrade(INT64 biCharacterDBID, OUT int &nSellingCount, OUT int &nExpiredCount);	// 내가 등록한 판매중 아이템 카운트 - P_GetCountMyTrade
	int QueryAddTrade(TQAddTrade *pQ, TAAddTrade *pA);	// 아이템 등록 - P_AddTrade
	int QueryCancelTrade(TQCancelTrade *pQ, TACancelTrade *pA);	// 아이템 등록 취소 - P_CancelTrade
	int QueryBuyTradeItem(TQBuyTradeItem *pQ, TABuyTradeItem *pA);	// 아이템 구입 - P_BuyTradeItem
	int QueryGetListTradeForCalculation(TQGetListTradeForCalculation *pQ, TAGetListTradeForCalculation *pA);	// 아이템 정산 목록 - P_GetListTradeForCalculation
	int QueryGetCountTradeForCalculation(INT64 biCharacterDBID, OUT int &nCalculationCount);	// 아이템 정산 카운트 - P_GetCountTradeForCalculation
	int QueryTradeCalculate(TQTradeCalculate *pQ, TATradeCalculate *pA);	// 개별 정산 - P_TradeCalculate
	int QueryTradeCalculateAll(TQTradeCalculateAll *pQ, TATradeCalculateAll *pA);	// 전체 정산 - P_TradeCalculateAll
	int QueryGetTradeItemID(int nMarketDBID, TAGetTradeItemID* pA);
	int QueryGetTradePrice(TQGetTradePrice* pQ, TAGetTradePrice* pA);
#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
	int QueryGetListMiniTrade(TQGetListMiniTrade *pQ, TAGetListMiniTrade *pA);		// P_GetListMiniTrade
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

	// PvP
	int QueryModPvPScore( TQUpdatePvPData* pQ, TAUpdatePvPData* pA );
	int QueryGetPvPScore( INT64 biCharacterDBID, TPvPGroup *pA );
	int QueryAddPVPGhoulScores(TQAddPVPGhoulScores* pQ);
	int QueryGetPVPGhoulScores(INT64 biCharacterDBID, TAGetPVPGhoulScores* pA);
#if defined(PRE_ADD_PVP_TOURNAMENT)
	int QueryAddPVPTournamentResult(TQAddPVPTournamentResult* pQ);
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)
	int QueryAddPvPLadderResult( TQAddPvPLadderResult* pQ, TAAddPvPLadderResult* pA );
	int QueryGetListPvPLadderScore( TQGetListPvPLadderScore* pQ, TAGetListPvPLadderScore* pA );
	int QueryGetListPvPLadderScoreByJob( TQGetListPvPLadderScoreByJob* pQ, TAGetListPvPLadderScoreByJob* pA );
	int QueryInitPvPLadderGradePoint( TQInitPvPLadderGradePoint* pQ, TAInitPvPLadderGradePoint* pA );
	int QueryUsePvPLadderPoint( TQUsePvPLadderPoint* pQ, TAUsePvPLadderPoint* pA );
	int QueryAddPvPLadderCUCount( TQAddPvPLadderCUCount* pQ );
	int QueryModPvPLadderScoresForCheat( TQModPvPLadderScoresForCheat* pQ );
	int QueryGetListPvPLadderRanking( TQGetListPvPLadderRanking* pQ, TAGetListPvPLadderRanking* pA );
#ifdef PRE_MOD_PVPRANK
	int QuerySetPvPRankCriteria(UINT nThresholdExp);
	int QueryCalcPvPRank();
#endif		//#ifdef PRE_MOD_PVPRANK
#if defined(PRE_ADD_PVP_RANKING)
	int QueryGetPvPRankInfo(TQHeader* pQ, INT64 biCharacterDBID, WCHAR* wszCharName, TPvPRankingDetail * pRanking);
	int QueryGetPvPRankList(TQGetPvPRankList * pQ, TAGetPvPRankList * pA);
	int QueryGetPvPLadderRankInfo(TQHeader* pQ, INT64 biCharacterDBID, WCHAR* wszCharName, BYTE cPvPLadderCode, TPvPLadderRankingDetail * pRanking);
	int QueryGetPvPLadderRankList(TQGetPvPLadderRankList * pQ, TAGetPvPLadderRankList * pA);
	int QueryAggregatePvPRank();
	int QueryAggregatePvPLadderRank();
#endif

#if defined( PRE_WORLDCOMBINE_PVP )
	int QueryAddWorldPvPRoom(TQAddWorldPvPRoom* pQ, int &nRoomID);
	int QueryAddWorldPvPRoomMember(TQAddWorldPvPRoomMember* pQ);
	int QueryDelWorldPvPRoom( int nWorldPvPRoomIndex );
	int QueryDelWorldPvPRoomMember(TQDelWorldPvPRoomMember* pQ);
	int QueryDelWorldPvPRoomForServer(int nServerID);
	int QueryGetListWorldPvPRoom(int nCombineWorldID, TAGetListWorldPvPRoom *pA);	
#endif
	
	// Test
	int QueryMWTest( TQMWTest* pQ );
	//Event
#if defined(PRE_ADD_WORLD_EVENT)
#else
	int QueryEventList(TQEventList * pQ, TAEventList * pA);
#endif //#if defined(PRE_ADD_WORLD_EVENT)

	//Guild
	int QueryCreateGuild(TQCreateGuild *pQ, TACreateGuild *pA);
	int QueryDismissGuild(TQDismissGuild *pQ, TADismissGuild *pA);
	int QueryAddGuildMember(TQAddGuildMember *pQ, TAAddGuildMember *pA);
	int QueryDelGuildMember(TQDelGuildMember *pQ, TADelGuildMember *pA);
	int QueryChangeGuildInfoNotice(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA);
	int QueryChangeGuildInfoRoleAuth(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA);
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	int QueryChangeGuildInfoHomePage(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA);
#endif
	int QueryChangeGuildMemberInfoIntroduce(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA);
	int QueryChangeGuildMemberInfoRole(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA);
	int QueryChangeGuildMemberInfoGuildMaster(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA);
	int QueryChangeGuildMemberInfoCommonPoint(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA);
	int QueryAddGuildHistory(TQAddGuildHistory *pQ);
	int QueryGetGuildHistoryList(TQGetGuildHistoryList *pQ, TAGetGuildHistoryList *pA);
	int QueryGetGuildInfo(UINT nGuildDBID, char cWorldID, TAGetGuildInfo *pA);
	int QueryGetGuildMemberList(UINT nGuildDBID, char cWorldID, std::vector<TGuildMember>& GuildMemberList);
	int QueryGetGuildWareInfo(TQGetGuildWareInfo *pQ, TAGetGuildWareInfo *pA);
	int QuerySetGuildWareInfo(TQSetGuildWareInfo *pQ, TASetGuildWareInfo *pA);
	int QueryGetGuildWareHistoryList(TQGetGuildWareHistory* pQ, TAGetGuildWareHistory* pA);
	int QueryMoveItemInGuildWare(TQMoveItemInGuildWare *pQ, TAMoveItemInGuildWare *pA);
	int QueryMoveInvenToGuildWare(TQMoveInvenToGuildWare *pQ, TAMoveInvenToGuildWare *pA);
	int QueryMoveGuildWareToInven(TQMoveGuildWareToInven *pQ, TAMoveGuildWareToInven *pA);
	int QueryModGuildCoin(TQGuildWareHouseCoin *pQ, TAGuildWareHouseCoin *pA);
	int QueryChangeGuildWareSize(TQChangeGuildWareSize * pQ, TAChangeGuildWareSize * pA);
	int QueryChangeGuildMark(TQChangeGuildMark * pQ, TAChangeGuildMark * pA);
	int QueryModGuildName(TQChangeGuildName* pQ);

	int QueryEnrollGuildWar(TQEnrollGuildWar *pQ, TAEnrollGuildWar *pA);
	int QueryGetGuildWarSchedule(TQGetGuildWarSchedule *pQ, TAGetGuildWarSchedule *pA);
	int QueryGetGuildWarFinalSchedule(TQGetGuildWarFinalSchedule *pQ, TAGetGuildWarFinalSchedule *pA);
	int QueryAddGuildWarPoint(TQAddGuildWarPointRecodrd *pQ);
	int QueryGetGuildWarPoint(TQGetGuildWarPoint *pQ, TAGetGuildWarPoint *pA);
	int QueryGetGuildWarPointPartTotal(TQGetGuildWarPointPartTotal *pQ, TAGetGuildWarPointPartTotal *pA);
	int QueryGetGuildWarPointGuildPartTotal(TQGetGuildWarPointGuildPartTotal *pQ, TAGetGuildWarPointGuildPartTotal *pA);
	int QueryGetGuildWarPointDaily(char cWorldSetID, UINT nAccountDBID, TAGetGuildWarPointDaily *pA);
	int QueryGetGuildWarPointGuildTotal(TQGetGuildWarPointGuildTotal *pQ, TAGetGuildWarPointGuildTotal *pA);
	int QueryAddGuildWarFinalMatchList(TQAddGuildWarFinalMatchList *pQ);
	int QueryAddGuildWarPopularityVote(TQAddGuildWarPopularityVote *pQ);
	int QueryGetGuildWarPopularityVote(TQGetGuildWarPopularityVote *pQ, TAGetGuildWarPopularityVote *pA);
	int QueryAddGuildWarFinalResults(TQAddGuildWarFinalResults *pQ);
	int QueryGetGuildWarFinalResults(TQGetGuildWarFinalResults *pQ, TAGetGuildWarFinalResults *pA);
	int QueryGetGuildWarPreWinGuild(TQGetGuildWarPreWinGuild *pQ, TAGetGuildWarPreWinGuild *pA);
	int QueryAddGuildWarRewarForCharacter(TQAddGuildWarRewardCharacter *pQ);
	int QueryGetGuildWarRewarForCharacter(TQGetGuildWarRewardCharacter *pQ, TAGetGuildWarRewardCharacter *pA);
	int QueryAddGuildWarRewarForGuild(TQAddGuildWarRewardGuild *pQ);
	int QueryGetGuildWarRewarForGuild(TQGetGuildWarRewardGuild *pQ, TAGetGuildWarRewardGuild *pA);
	int QueryAddGuildWarPointQueue(TQAddGuildWarPointQueue* pQ);
	int QueryAddGuildWarSystemMailQueue(TQSendSystemMail* pQ);
	int QueryAddJobReserve(TQAddJobReserve* pQ, TAAddJobReserve* pA);
	int QueryGetJobReserve(TQGetJobReserve* pQ, TAGetJobReserve* pA);
	int QueryGetGuildWarPointRunningTotal(TQGetGuildWarPointRunningTotal* pQ, TAGetGuildWarPointRunningTotal* pA );	
	int QueryGetGuildWarFinalRewardPoint(TAGetGuildWarPointFinalRewards* pA);
	int QueryGetListGuildWarItemTradeRecord(INT64 characterDBID, OUT TAGetListGuildWarItemTradeRecord& record);
	int QueryAddGuildWarItemTradeRecord(INT64 characterDBID, ItemCount& itemCount);

	int QueryModGuildExp(TQModGuildExp* pQ, TAModGuildExp& pA);
	int QueryModGuildCheat(TQModGuildCheat* pQ, TAModGuildCheat& pA);
	int QueryGetGuildRewardItem(TQGetGuildRewardItem* pQ, TAGetGuildRewardItem* pA);
	int QueryAddGuildRewardItem(TQAddGuildRewardItem* pQ, TAAddGuildRewardItem* pA);
	int QueryChangeGuildSize(TQChangeGuildSize* pQ);
	int QueryDelGuildRewardItem(TQDelGuildEffectItem* pQ);
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
	int QueryGetGuildContributionPoint(TQGetGuildContributionPoint* pQ, TAGetGuildContributionPoint& pA);
	int QueryGetGuildContributionWeeklyRanking(TQGetGuildContributionRanking* pQ, TAGetGuildContributionRanking* pA);
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	int QueryModCharacterName(TQChangeCharacterName* pQ);
	int QueryGetListEtcPoint(TQGetListEtcPoint *pQ, TAGetListEtcPoint *pA);
	int QueryAddEtcPoint( TQAddEtcPoint* pQ, TAAddEtcPoint *pA );
	int QueryUseEtcPoint( TQUseEtcPoint* pQ, TAUseEtcPoint *pA );
#if defined( PRE_ADD_NEW_MONEY_SEED )
	int QueryGetEtcPoint( INT64 biCharacterDBID, char cType, INT64 &nPoint );
#endif

#if !defined(_LOGINSERVER)
	// 아이템 처리
	int CashItemDBQuery(int nAccountDBID, INT64 biCharacterDBID, int nMapID, int nChannelID, char cPaymentRules, TCashItemBase &BuyItem, int nPrice, INT64 biPurchaseOrderID, int nAddMaterializedItemCode, 
		const WCHAR* pwszIP, INT64 biSenderCharacterDBID = 0, bool bGift = false, char cPayMethodCode = DBDNWorldDef::PayMethodCode::Cash);
#endif	// #if !defined(_LOGINSERVER)

	//Log
	int QueryAddPartyStartLog( TQAddPartyStartLog* pQ );
	int QueryAddPartyEndLog( TQAddPartyEndLog* pQ );
	int QueryAddStageStartLog( TQAddStageStartLog* pQ );
	int QueryAddStageClearLog( TQAddStageClearLog* pQ );
	int QueryAddStageRewardLog( TQAddStageRewardLog* pQ );
	int QueryAddStageEndLog( TQAddStageEndLog* pQ );
	int QueryAddPvPStartLog( TQAddPvPStartLog* pQ );
	int QueryAddPvPResultLog( TQAddPvPResultLog* pQ );
	int QueryAddPvPEndLog( TQAddPvPEndLog* pQ );
#if defined( PRE_ADD_58761 )
	int QueryAddNestGateStartLog( TQAddNestGateStartLog* pQ );
	int QueryAddNestGateEndLog( TQAddNestGateEndLog* pQ );
	int QueryAddNestGateClearLog( TQAddNestGateClearLog* pQ );	
	int QueryAddNestDeathLog( TQNestDeathLog* pQ );	
#endif

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	// NPC 호감도
	int QueryModNpcLocation( TQModNPCLocation* pQ );
	int QueryModNpcFavor( TQModNPCFavor* pQ );
	int QueryGetListNpcFavor( TQGetListNpcFavor* pQ, std::vector<TNpcReputation>& vReputation );
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

	// 사제 시스템
	int QueryGetMasterPupilInfo( TQGetMasterPupilInfo* pQ, TAGetMasterPupilInfo* pA );
	int QueryGetMasterPupilInfo2( TQGetMasterPupilInfo* pQ, TAGetMasterPupilInfo* pA );
	int QueryAddMasterCharacter( TQAddMasterCharacter* pQ );
	int QueryDelMasterCharacter( TQDelMasterCharacter* pQ );
	int QueryGetPageMasterCharacter( TQGetPageMasterCharacter* pQ, TAGetPageMasterCharacter* pA );
	int QueryGetMasterCharacterType1( TQGetMasterCharacter* pQ, TAGetMasterCharacterType1* pA );	// 자신의 스승 정보
	int QueryGetMasterCharacterType2( TQGetMasterCharacter* pQ, TAGetMasterCharacterType2* pA );	// 내 스승 정보
	int QueryGetMasterCharacterType3( TQGetMasterCharacter* pQ, TAGetMasterCharacterType3* pA );	// 임의의 특정 스승 정보
	int QueryGetListPupil( TQGetListPupil* pQ, TAGetListPupil* pA );
	int QueryAddMasterAndPupil( TQAddMasterAndPupil* pQ, TAAddMasterAndPupil* pA );
	int QueryDelMasterAndPupil( TQDelMasterAndPupil* pQ, TADelMasterAndPupil* pA );
	int QueryGetListMyMasterAndClassmate( TQGetListMyMasterAndClassmate* pQ, TAGetListMyMasterAndClassmate* pA );
	int QueryGetMyClassmate( TQGetMyClassmate* pQ, TAGetMyClassmate* pA );
	int QueryGraduate( TQGraduate* pQ, TAGraduate* pA );
	int QueryModRespectPoint( TQModRespectPoint* pQ, TAModRespectPoint* pA );
	int QueryModMasterFavorPoint( TQModMasterFavorPoint* pQ, TAModMasterFavorPoint* pA );
	int QueryGetMasterAndFavorPoint( TQGetMasterAndFavorPoint* pQ, TAGetMasterAndFavorPoint* pA );
	int QueryGetMasterSystemCountInfo( TQGetMasterSystemCountInfo* pQ, TAGetMasterSystemCountInfo* pA );
	int QueryModGraduateCount( TQModMasterSystemGraduateCount* pQ );

	// DarkLair
	int QueryUpdateDarkLairResult( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA );
	int QueryGetDarkLairPersonalBest( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA, int iIdx );
	int QueryGetListDarkLairTopRanker( TQGetDarkLairRankBoard* pQ, TAGetDarkLairRankBoard* pA );

#if defined( PRE_ADD_SECONDARY_SKILL )
	// 보조 스킬
	int QueryAddSecondarySkill( TQAddSecondarySkill* pQ );
	int QueryDelSecondarySkill( TQDelSecondarySkill* pQ );
	int QueryGetListSecondarySkill( TQGetListSecondarySkill* pQ, TAGetListSecondarySkill* pA );
	int QueryModSecondarySkillExp( TQModSecondarySkillExp* pQ );
	int QuerySetManufactureSkillRecipe( TQSetManufactureSkillRecipe* pQ );
	int QueryExtractManufactureSkillRecipe( TQExtractManufactureSkillRecipe* pQ );
	int	QueryDelManufactureSkillRecipe( TQDelManufactureSkillRecipe* pQ );
	int	QueryGetListManufactureSkillRecipe( TQGetListManufactureSkillRecipe* pQ, TAGetListManufactureSkillRecipe* pA );
	int QueryModManufactureSkillRecipe( TQModManufactureSkillRecipe* pQ );
#endif // #if defined( PRE_ADD_SECONDARY_SKILL )

	// 농장
	int QueryGetListFarm( TQGetListFarm* pQ, TAGetListFarm* pA );
	int QueryGetListField( TQGetListField* pQ, TAGetListField* pA );
	int QueryGetListFieldByCharacter( TQGetListFieldByCharacter* pQ, TAGetListFieldByCharacter* pA );
	int QueryAddField( TQAddField* pQ, TAAddField* pA );
	int QueryDelField( TQDelField* pQ, TADelField* pA );
	int QueryAddFieldAttachment( TQAddFieldAttachment* pQ, TAAddFieldAttachment* pA );
	int QueryModFieldElapsedTime( TQModFieldElapsedTime* pQ, TAModFieldElapsedTime* pA );
	int QueryHarvest( TQHarvest* pQ, TAHarvest* pA );
	int QueryGetListHarvestDepotItem( TQGetListHarvestDepotItem* pQ, TAGetListHarvestDepotItem* pA );
	int QueryGetCountHarvestDepotItem( TQGetCountHarvestDepotItem* pQ, TAGetCountHarvestDepotItem* pA );
	int QueryGetFieldCountByCharacter( TQGetFieldCountByCharacter* pQ, TAGetFieldCountByCharacter* pA );
	int QueryGetFieldItemCount( TQGetFieldItemCount* pQ, TAGetFieldItemCount* pA );

	int QueryGetListFieldForCharacter( TQGetListFieldForCharacter* pQ, TAGetListFieldForCharacter* pA );
	int QueryAddFieldForCharacter( TQAddField* pQ, TAAddField* pA );
	int QueryDelFieldForCharacter( TQDelFieldForCharacter* pQ, TADelFieldForCharacter* pA );
	int QueryAddFieldForCharacterAttachment( TQAddFieldAttachment* pQ, TAAddFieldAttachment* pA );
	int QueryModFieldForCharacterElapsedTime( TQModFieldForCharacterElapsedTime* pQ, TAModFieldForCharacterElapsedTime* pA );
	int QueryHarvestForCharacter( TQHarvest* pQ, TAHarvest* pA );

	int QueryDeletePeriodQuestList( TQDeletePeriodQuest* pQ, TADeletePeriodQuest* pA );
	int QueryGetPeriodQuestDate( TQGetPeriodQuestDate* pQ, TAGetPeriodQuestDate* pA );
	int QueryGetListCompleteEventQuest(TQGetListCompleteEventQuest * pQ, TAGetListCompleteEventQuest * pA);
	int QueryCompleteEventQuest(TQCompleteEventQuest* pQ, TACompleteEventQuest* pA);
	int QueryGetWorldEventQuestCounter(TQGetWorldEventQuestCounter* pQ, TAGetWorldEventQuestCounter* pA);
	int QueryModWorldEventQuestCounter(TQModWorldEventQuestCounter* pQ, TAModWorldEventQuestCounter* pA);

	int QueryGetConnectDuration(INT64 biCharacterDBID, TTimeEventGroup &TimeEventGroup);
	int QuerySaveConnectDurationTime(TQSaveConnectDurationTime *pQ);
	int QueryModTimeEventDate( TQModTimeEventDate* pQ );

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	int QueryChangeJobCode( INT64 biCharacterDBID, int nCannelID, int nMapID, int nFirstJobIDBefore, int nFirstJobIDAfter, 
							int nSecondJobIDBefore, int nSecondJobIDAfter, int nJobCodeChangeCode, OUT USHORT& wSkillPoint, OUT USHORT& wTotalSkillPoint ); // 전직 아이템 사용 - P_ChangeJobCode
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

	int QueryGetListVariableReset( TQGetListVariableReset* pQ, TAGetListVariableReset* pA );
	int QueryModVariableReset( TQModVariableReset* pQ );

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
	int QueryHeartbeat();
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL)

	int QueryGetListRepurchaseItem( TQGetListRepurchaseItem* pQ, TAGetListRepurchaseItem* pA );

	int QueryGetPageGuildRecruit( TQGetGuildRecruit* pQ, TAGetGuildRecruit* pA );
	int QueryGetPageGuildRecruitCharacter( TQGetGuildRecruitCharacter* pQ, TAGetGuildRecruitCharacter* pA );
	int QueryMyGuildRecruit( TQGetMyGuildRecruit* pQ, TAGetMyGuildRecruit* pA );
	int QueryGuildRecruitRequestCount( TQGetGuildRecruitRequestCount* pQ, TAGetGuildRecruitRequestCount* pA );
	int QueryRegisterInofGuildRecruit( TQRegisterInfoGuildRecruit* pQ, TARegisterInfoGuildRecruit* pA );
	int QueryRegisterOnGuildRecruit( TQRegisterOnGuildRecruit* pQ );
	int QueryRegisterModGuildRecruit( TQRegisterModGuildRecruit* pQ );
	int QueryRegisterOffGuildRecruit( TQRegisterOffGuildRecruit* pQ );
	int QueryGuildRecruitRequestOn( TQGuildRecruitRequestOn* pQ );
	int QueryGuildRecruitRequestOff( TQGuildRecruitRequestOff* pQ );
	int QueryGuildRecruitAcceptOn( TQGuildRecruitAcceptOn* pQ, TAGuildRecruitAcceptOn* pA );
	int QueryGuildRecruitAcceptOff( TQGuildRecruitAcceptOff* pQ, TAGuildRecruitAcceptOff* pA );
	int QueryDelGuildRecruit( TQDelGuildRecruit* pQ );

#if defined (PRE_ADD_DONATION)
	int QueryDonate(TQDonate* pQ);
	int QueryDonationRanking(TQDonationRanking* pQ, OUT TADonationRaking& Ack);
	int QueryDonationTopRanker(TQDonationTopRanker* pQ, OUT TADonationTopRanker& Ack);
#endif // #if defined (PRE_ADD_DONATION)

#if defined( PRE_PARTY_DB )
	int QueryAddParty( TQAddParty* pQ, TAAddParty* pA );
	int QueryAddPartyAndMemberGame( TQAddPartyAndMemberGame* pQ, TAAddPartyAndMemberGame* pA );
	int QueryAddPartyAndMemberVillage( TQAddPartyAndMemberVillage* pQ, TAAddPartyAndMemberVillage* pA );
	int QueryModParty( TQModParty* pQ );
	int QueryDelParty( TQDelParty* pQ );
	int QueryDelPartyForServer( TQDelPartyForServer* pQ );
	int QueryJoinParty( TQJoinParty* pQ );
	int QueryOutParty( TQOutParty* pQ, TAOutParty* pA );
	int QueryGetListParty( TQGetListParty* pQ, std::vector<Party::Data>& vData );
#if defined( PRE_WORLDCOMBINE_PARTY )
	int QueryGetListParty( int iWorldSetID, ePartyType PartyType, std::vector<Party::Data>& vData );
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
	int QueryGetListPartyMember( TQGetListPartyMember* pQ, TAGetListPartyMember* pA, std::vector<Party::MemberData>& vData );
	int QueryModPartyLeader( TQModPartyLeader* pQ );	
#if defined( PRE_ADD_NEWCOMEBACK )
	int QueryModPartyMemberComeback( TQModPartyMemberComeback* pQ );
#endif
#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_ADD_DOORS_PROJECT )
	int QuerySaveCharacterAbility(TQSaveCharacterAbility* pQ);
#endif // #if defined( PRE_ADD_DOORS_PROJECT )
#if defined (PRE_ADD_BESTFRIEND)
	int QueryGetBestFriend(TQGetBestFriend* pQ, TAGetBestFriend* pA);
	int QueryRegistBestFriend(TQRegistBestFriend* pQ, TARegistBestFriend* pA);
	int QueryCancelBestFriend(TQCancelBestFriend* pQ, TACancelBestFriend* pA);
	int QueryCloseBestFriend(TQCloseBestFriend* pQ, TACloseBestFriend* pA);
	int QueryEditBestFriendMemo(TQEditBestFriendMemo* pQ, TAEditBestFriendMemo* pA);
#endif
#if defined( PRE_PRIVATECHAT_CHANNEL )
	int QueyrGetPrivateChatChannelInfo(TQPrivateChatChannelInfo* pQ, std::vector<TPrivateChatChannelInfo>& vData);
	int QueyrGetPrivateChatChannelMember(TQPrivateChatChannelMember* pQ, TAPrivateChatChannelMember* pA);
	int QueyrAddPrivateChatChannel(TQAddPrivateChatChannel* pQ, TAAddPrivateChatChannel* pA);
	int QueyrAddPrivateChatChannelMember(TQAddPrivateChatMember* pQ);
	int QueyrInvitePrivateChatChannelMember(TQInvitePrivateChatMember* pQ);
	int QueyrEnterPrivateChatChannelMember(TQAddPrivateChatMember* pQ);
	int QueyrDelPrivateChatChannelMember(TQDelPrivateChatMember* pQ);
	int QueyrExitPrivateChatChannelMember(TQDelPrivateChatMember* pQ);
	int QueyrModPrivateChatChannelInfo(TQModPrivateChatChannelInfo* pQ);
	int QueyrModPrivateMemberServerID(TQModPrivateChatMemberServerID* pQ);
	int QueyrDelPrivateMemberServerID(TQDelPrivateChatMemberServerID* pQ, std::list<TPrivateMemberDelServer> &Member);
#endif

#ifdef PRE_ADD_DOORS
	int QueryCancelDoorsAuthentication(UINT nAccountDBID);
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_ADD_NEWCOMEBACK )
	int QueryCheckCombackEffectItem( INT64 biCharacterDBID, int nItemID );
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	int QueryResetAlteiaWorldEvent( TQResetAlteiaWorldEvent* pQ );
	int QueryAddAlteiaWorldPlayResult( TQADDAlteiaWorldPlayResult* pQ );
	int QueryGetAlteiaWorldInfo( TQGetAlteiaWorldInfo* pQ, TAGetAlteiaWorldInfo* pA );
	int QueryGetAlteiaWorldPrivateGoldKeyRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldPrivateGoldKeyRank* pA );
	int QueryGetAlteiaWorldPrivatePlayTimeRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldPrivatePlayTimeRank* pA );
	int QueryGetAlteiaWorldGuildGoldKeyRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldGuildGoldKeyRank* pA );
	int QueryGetAlteiaWorldSendTicketList( TQGetAlteiaWorldSendTicketList* pQ, TAGetAlteiaWorldSendTicketList* pA );
	int QueryAddAlteiaWorldSendTicketList( TQAddAlteiaWorldSendTicketList* pQ );
	int QueryResetAlteiaWorldPlayAlteia( TQResetAlteiaWorldPlayAlteia* pQ );
	int QueryAddAlteiaWorldEvent( TQAddAlteiaWorldEvent* pQ );
#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
	int QueryGetListCompleteChallenges( TQGetListCompleteChallenges* pQ, TAGetListCompleteChallenges* pA );
	int QueryInitCompleteChallenge( TQInitCompleteChallenge* pQ );
	int QueryAddCompleteChallenge( TQAddCompleteChallenge* pQ );
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_CHNC2C)
	int QueryGetGameMoney(TQGetGameMoney* pQ, TAGetGameMoney* pA);
	int QueryKeepGameMoney(TQKeepGameMoney* pQ);
	int QueryTransferGameMoney(TQTransferGameMoney* pQ);
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_CP_RANK)
	int QueryAddStageClearBest(TQAddStageClearBest* pQ);
	int QueryGetStageClearBest(TQGetStageClearBest* pQ, TAGetStageClearBest* pA);
	int QueryGetStageClearPersonalBest(TQGetStageClearPersonalBest* pQ, TAGetStageClearPersonalBest* pA);
	int QueryInitStageClearBest(char cInitType);
#endif //#if defined(PRE_ADD_CP_RANK)

#if defined(PRE_ADD_DWC)
	int QueryAddDWCTeam(TQCreateDWCTeam* pQ, TACreateDWCTeam* pA);
	int QueryAddDWCTeamMember(TQAddDWCTeamMember* pQ, TAAddDWCTeamMember* pA);
	int QueryDelDWCTeamMember(TQDelDWCTeamMember* pQ, TADelDWCTeamMember* pA);
	//int QueryGetDWCTeamInfo(TQGetDWCTeamInfo* pQ, TAGetDWCTeamInfo* pA);	
	int QueryGetDWCTeamInfo(INT64 biCharacterDBID, UINT* pTeamID, TDWCTeam* Info);
	int QueryGetDWCTeamInfoByTeamID(UINT nTeamID, UINT* pTeamID, TDWCTeam* Info);
	int QueryGetDWCTeamMemberList(UINT nDWCTeamID, std::vector<TDWCTeamMember>& vTeamMember);
	int QueryAddDWCResult( TQAddPvPDWCResult* pQ, TAAddPvPDWCResult* pA );
	int QueryGetDWCRankList(TQGetDWCRankPage* pQ, TAGetDWCRankPage * pA);
	int QueryGetDWCFindRank(TQGetDWCFindRank* pQ, TAGetDWCFindRank * pA);
	int QueryGetDWCChannelInfo(TDWCChannelInfo* pChannelInfo);
#endif

#if defined(PRE_ADD_EQUIPLOCK)
	int QueryAddLockItem(TQLockItemInfo* pQ, TALockItemInfo *pA);
	int QueryRequestItemUnlock(TQUnLockRequsetItemInfo* pQ, TAUnLockRequestItemInfo *pA);
	int QueryUnlockItem(TQLockItemInfo* pQ);
	int QueryGetListLockedItems(INT64 biCharacterDBID, TAGetListLockedItems *pA);
#endif	// #if defined(PRE_ADD_EQUIPLOCK)

#if defined( PRE_PVP_GAMBLEROOM )
	int QueryAddGambleRoom(TQAddGambleRoom* pQ, TAAddGambleRoom *pA);
	int QueryAddGambleRoomMember(TQAddGambleRoomMember* pQ);
	int QueryEndGambleRoom(TQEndGambleRoom* pQ);
#endif

private:
	int _QueryAddDarkLairRanking( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA );

#if defined( PRE_PARTY_DB )
	int _QueryGetListParty( std::vector<Party::Data>& vData );
#endif // #if defined( PRE_PARTY_DB )
};
