#include "StdAfx.h"
#include "DNSQLWorld.h"
#include "Util.h"
#include "Log.h"
#include "DNServiceConnection.h"
#include "TimeSet.h"
#include "DNExtManager.h"
#if !defined(_LOGINSERVER)
#include "DNExtManager.h"
#endif

#ifdef PRE_ADD_BEGINNERGUILD
#include "./EtStringManager/EtUIXML.h"
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

const WCHAR* g_szDBJobSystem[DBJOB_SYSTEM_MAX] = {
	{L"exec dbo.P_DelGuildWarOpeningPointRecord"},
	{L"exec dbo.P_AddGuildWarOpeningPointTotal;exec dbo.P_AddGuildWarOpeningPointDailyTotal;exec dbo.P_AddGuildWarOpeningPointGuildTotal;"},
};

CDNSQLWorld::CDNSQLWorld(void): CSQLConnection()
{
}

CDNSQLWorld::~CDNSQLWorld(void)
{
}

int CDNSQLWorld::QueryGetDatabaseVersion( TDatabaseVersion* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetDatabaseVersion" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[QueryGetDatabaseVersion] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDatabaseVersion}" );
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;			
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,	 pA->wszName,	sizeof(pA->wszName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->iVersion, sizeof(int),		 &cblen );
			CheckColumnCount(nNo, "P_GetDatabaseVersion");
			RetCode = SQLFetch(m_hstmt);
		}	
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryLoginCharacter( TQLoginCharacter* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_LoginCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_LoginCharacter2(%I64d,%d,N'%s')}", pQ->biCharacterDBID, pQ->uiSessionID, pQ->wszIP );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryLogoutCharacter( TQLogoutCharacter* pQ )
{
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	UINT uiCheckSum = 0;
	int nResult = QueryMakeCharacterCheckSum( pQ, CheckSumReason::LogoutCharacter, pQ->biCharacterDBID, uiCheckSum );
	if( ERROR_NONE != nResult )
	{
		return nResult;
	}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

	CQueryTimeLog QueryTimeLog( "P_LogoutCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	int nViewCashEquip = 0;
	memcpy(&nViewCashEquip, pQ->cViewCashEquipBitmap, sizeof(pQ->cViewCashEquipBitmap));

#if defined( PRE_ADD_CHARACTERCHECKSUM )
	if (CheckConnect() < 0){
		g_Log.Log(LogType::_ERROR, L"[QueryLogoutCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}	

	swprintf( m_wszQuery, L"{?=CALL dbo.P_LogoutCharacter(%I64d,%d,%d,?)}", pQ->biCharacterDBID, pQ->uiSessionID, nViewCashEquip );

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;
	nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nCheckSumSize = sizeof(uiCheckSum);
		int nNo = 1;
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, nCheckSumSize, 0, &uiCheckSum, nCheckSumSize, SqlLen.Get(nCheckSumSize));

		RetCode = SQLExecute (m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
		{
			g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, pQ->uiSessionID, "[QueryLogoutCharacter] Fail(result:%d)\r\n", nResult);
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
#else // #if defined( PRE_ADD_CHARACTERCHECKSUM )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_LogoutCharacter(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->uiSessionID, nViewCashEquip );
	return CommonReturnValueQuery(m_wszQuery);
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
}

int CDNSQLWorld::QuerySelectCharacter(int nWorldSetID, INT64 biCharacterDBID, UINT nLastServerType, OUT TASelectCharacter *pA, UINT nAccountDBID)
{
	int nRet = QueryGetCharacter(nWorldSetID, biCharacterDBID, nLastServerType, pA);		// Status, Mission, Appellation
	if (nRet != ERROR_NONE) return nRet;

#if defined( PRE_ADD_CHARACTERCHECKSUM )
	nRet = QueryGetCharacterCheckSum(biCharacterDBID, pA); // checksum
	if (nRet != ERROR_NONE) return nRet;
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

	nRet = QueryGetListJobChangeLog(biCharacterDBID, pA->UserData.Status.cJobArray);	// jobarray
	if (nRet != ERROR_NONE) return nRet;

	int nCount = 0;

	nRet = QueryGetListNotifier(biCharacterDBID, nCount, pA->UserData.Status.NotifierData);	// notifier
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListQuickSlot(biCharacterDBID, nCount, pA->UserData.Status.QuickSlot);	// quickslot
	if (nRet != ERROR_NONE) return nRet;

#if !defined(PRE_DELETE_DUNGEONCLEAR)
	nRet = QueryGetListDungeonClear(biCharacterDBID, nCount, pA->UserData.Status.DungeonClear);	// dungeonclear
	if (nRet != ERROR_NONE) return nRet;
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

	nRet = QueryGetListNestClearCount(biCharacterDBID, nCount, pA->UserData.Status.NestClear);	// nest clear
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListEquipmentAttributes(biCharacterDBID, pA->UserData.Status);	// equipdelaytime, equipremaintime
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetPvPScore(biCharacterDBID, &(pA->UserData.PvP));		// pvp
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListSkill(biCharacterDBID, nCount, pA->UserData.Skill);	// skill
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListAchieveMission(biCharacterDBID, pA->UserData.Mission.MissionAchieve);
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListAppellation(biCharacterDBID, pA->UserData.Appellation.Appellation);
	if (nRet != ERROR_NONE) return nRet;

#if defined(PRE_PERIOD_INVENTORY)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	std::wstring wstrItemLocationCode = L"1,2,3,4,7,8,19,20,21";	// 캐쉬인벤은 따로 로드한다
#else
	std::wstring wstrItemLocationCode = L"1,2,3,4,7,8,20,21";	// 캐쉬인벤은 따로 로드한다
#endif
#else	// #if defined(PRE_PERIOD_INVENTORY)
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	std::wstring wstrItemLocationCode = L"1,2,3,4,7,8,19";	// 캐쉬인벤은 따로 로드한다
#else
	std::wstring wstrItemLocationCode = L"1,2,3,4,7,8";	// 캐쉬인벤은 따로 로드한다
#endif
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	DBPacket::TMaterialItemInfo ItemList[nItemListMaxCount];
	memset(&ItemList, 0, sizeof(ItemList));
	nRet = QueryGetListMaterializedItem(biCharacterDBID, wstrItemLocationCode.c_str(), nItemListMaxCount, nCount, ItemList, nAccountDBID);

	if( nCount > nItemListMaxCount )
	{
		nCount = nItemListMaxCount;
		g_Log.Log(LogType::_ERROR, nWorldSetID, 0, biCharacterDBID, 0, L"[QueryGetListMaterializedItem] Ret=%d ItemCount Invalid=%d/%d\n", nRet, nCount, nItemListMaxCount);
	}

	pA->nItemListCount = nCount;
	memcpy(pA->ItemList, ItemList, sizeof(DBPacket::TMaterialItemInfo) * nCount);

	DBPacket::TItemIndexSerial IndexList[nItemListMaxCount] = {0,};
	nRet = QueryGetListItemLocationIndex(biCharacterDBID, nCount, IndexList);

	for (int i = 0; i < pA->nItemListCount; i++){
		for (int j = 0; j < nCount; j++){
			if (pA->ItemList[i].ItemInfo.Item.nSerial == IndexList[j].biItemSerial){
				pA->ItemList[i].ItemInfo.cSlotIndex = IndexList[j].cSlotIndex;
				break;
			}
		}
	}

	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListQuest(biCharacterDBID, nCount, pA->UserData.Quest.Quest);	// quest
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetCompletedQuests(biCharacterDBID, pA->UserData.Quest.CompleteQuest);	//completed quest
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetListDailyWeeklyMission(biCharacterDBID, pA->UserData);	// daily, weekly mission
	if (nRet != ERROR_NONE) return nRet;

	nRet = QueryGetConnectDuration(biCharacterDBID, pA->UserData.TimeEvent);	// timeevent
	if (nRet != ERROR_NONE) return nRet;

	std::vector<DBPacket::TVehicleInfo> VecVehicleList;
	VecVehicleList.clear();
	nRet = QueryGetPagePet(biCharacterDBID, 1, Vehicle::Slot::Max + 1, DBDNWorldDef::PetType::Vehicle, true, VecVehicleList);

	if(!VecVehicleList.empty()){
		for(int i = 0; i <(int)VecVehicleList.size(); i++){
			if(VecVehicleList[i].nPetIndex == Vehicle::Slot::Sundries){
				pA->VehicleEquip.dwPartsColor1 = VecVehicleList[i].dwPartsColor1;
				continue;
			}
			pA->VehicleEquip.Vehicle[VecVehicleList[i].nPetIndex] = VecVehicleList[i];
		}
	}

	if (nRet != ERROR_NONE) return nRet;

 	std::vector<DBPacket::TVehicleInfo> VecPetList;
 	VecPetList.clear();
	nRet = QueryGetPagePet(biCharacterDBID, 1, Pet::Slot::Max + 1, DBDNWorldDef::PetType::Pet, true, VecPetList);
 
	if(!VecPetList.empty())
	{
		for(int i = 0; i <(int)VecPetList.size(); i++)
		{
			if(VecPetList[i].nPetIndex == Pet::Slot::Sundries)
			{
				pA->PetEquip.dwPartsColor1 = VecPetList[i].dwPartsColor1;
				pA->PetEquip.dwPartsColor2 = VecPetList[i].dwPartsColor2;
				_wcscpy(pA->PetEquip.wszNickName, NAMELENMAX, VecPetList[i].wszNickName, NAMELENMAX);
				pA->PetEquip.nExp = VecPetList[i].nExp;
				pA->PetEquip.nSkillID1 = VecPetList[i].nSkillID1;
				pA->PetEquip.nSkillID2 = VecPetList[i].nSkillID2;
				pA->PetEquip.bSkillSlot = VecPetList[i].bSkillSlot;
				pA->PetEquip.tLastHungerModifyDate = VecPetList[i].tLastHungerModifyDate;
				pA->PetEquip.nCurrentSatiety = VecPetList[i].nCurrentSatiety;
				continue;
			}
			pA->PetEquip.Vehicle[VecPetList[i].nPetIndex] = VecPetList[i];
		}
	}

	if (nRet != ERROR_NONE) 
		return nRet;

	return ERROR_NONE;
}

int CDNSQLWorld::QueryMoveIntoNewServer(TQUpdateCharacter *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_MoveIntoNewServer" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryMoveIntoNewServer] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wQuickSlotTypeStr, wQuickSlotValueStr, wSkillIDStr, wSkillCoolTimeStr, wItemSerialStr, wItemCoolTimeStr, wEquipIndexStr, wEquipAttributeStr, wEquipValueStr;

	std::wstring wSkillIDStr2, wSkillCoolTimeStr2;

	for(int i = 0; i < QUICKSLOTMAX; i++){
		if(i > 0) wQuickSlotTypeStr.append(L",");
		wQuickSlotTypeStr.append(boost::lexical_cast<std::wstring>(pQ->QuickSlot[i].cType));
		if(i > 0) wQuickSlotValueStr.append(L",");
		wQuickSlotValueStr.append(boost::lexical_cast<std::wstring>(pQ->QuickSlot[i].nID));
	}
	for(int i = 0; i < SKILLMAX; i++){
		if(pQ->SkillCoolTime[i].nSkillID > 0){
			if(!wSkillCoolTimeStr.empty()){
				wSkillCoolTimeStr.append(L",");
				wSkillIDStr.append(L",");
			}
			wSkillCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->SkillCoolTime[i].nCoolTime));
			wSkillIDStr.append(boost::lexical_cast<std::wstring>(pQ->SkillCoolTime[i].nSkillID));
		}
	}
	for(int i = 0; i < SKILLMAX; i++){
		if(pQ->SkillCoolTime2[i].nSkillID > 0){
			if(!wSkillCoolTimeStr2.empty()){
				wSkillCoolTimeStr2.append(L",");
				wSkillIDStr2.append(L",");
			}
			wSkillCoolTimeStr2.append(boost::lexical_cast<std::wstring>(pQ->SkillCoolTime2[i].nCoolTime));
			wSkillIDStr2.append(boost::lexical_cast<std::wstring>(pQ->SkillCoolTime2[i].nSkillID));
		}
	}
	for(int i = 0; i < EQUIPMAX; i++){
		if(pQ->Equip[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->Equip[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->Equip[i].biItemSerial));
		}
	}
	for(int i = 0; i < INVENTORYMAX; i++){
		if(pQ->Inventory[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->Inventory[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->Inventory[i].biItemSerial));
		}
	}
	for(int i = 0; i < WAREHOUSEMAX; i++){
		if(pQ->Warehouse[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->Warehouse[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->Warehouse[i].biItemSerial));
		}
	}
	for(int i = 0; i < CASHEQUIPMAX; i++){
		if(pQ->CashEquip[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->CashEquip[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->CashEquip[i].biItemSerial));
		}
	}
	for(int i = 0; i < CASHINVENTORYDBMAX; i++){
		if(pQ->CashInventory[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->CashInventory[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->CashInventory[i].biItemSerial));
		}
	}

#if defined( PRE_ITEMBUFF_COOLTIME )	
	for(int i = 0; i < WAREHOUSEMAX; i++){
		if(pQ->ServerWare[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->ServerWare[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->ServerWare[i].biItemSerial));
		}
	}
	for(int i = 0; i < WAREHOUSEMAX; i++){
		if(pQ->ServerWareCash[i].biItemSerial > 0){
			if(!wItemCoolTimeStr.empty()){
				wItemCoolTimeStr.append(L",");
				wItemSerialStr.append(L",");
			}
			wItemCoolTimeStr.append(boost::lexical_cast<std::wstring>(pQ->ServerWareCash[i].nCoolTime));
			wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->ServerWareCash[i].biItemSerial));
		}
	}
#endif

	wEquipValueStr.append(boost::lexical_cast<std::wstring>(pQ->nGlyphDelayTime));
	wEquipIndexStr.append(boost::lexical_cast<std::wstring>(20));
	wEquipAttributeStr.append(boost::lexical_cast<std::wstring>(DBDNWorldDef::EquipmentAttributeCode::DelayTime));

	wEquipValueStr.append(L",");
	wEquipIndexStr.append(L",");
	wEquipAttributeStr.append(L",");

	wEquipValueStr.append(boost::lexical_cast<std::wstring>(pQ->nGlyphRemainTime));
	wEquipIndexStr.append(boost::lexical_cast<std::wstring>(20));
	wEquipAttributeStr.append(boost::lexical_cast<std::wstring>(DBDNWorldDef::EquipmentAttributeCode::RemainTime));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	if( _isnan(pQ->fRotate) != 0 )
		pQ->fRotate = 0.f;

	int nViewCashEquip = 0;
	memcpy(&nViewCashEquip, pQ->cViewCashEquipBitmap, sizeof(pQ->cViewCashEquipBitmap));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveIntoNewServer2(%I64d,%d,%d,%d,%d,%d,%d,%d,%f,N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',%d,0,%d)}", 
		pQ->biCharacterDBID, pQ->nMapIndex, pQ->nLastVillageMapIndex, pQ->nLastSubVillageMapIndex, pQ->cLastVillageGateNo, pQ->nPosX, pQ->nPosY, pQ->nPosZ, pQ->fRotate, 
		wQuickSlotTypeStr.c_str(), wQuickSlotValueStr.c_str(), wSkillIDStr.c_str(), wSkillCoolTimeStr.c_str(), wItemSerialStr.c_str(), wItemCoolTimeStr.c_str(),
		wEquipIndexStr.c_str(), wEquipAttributeStr.c_str(), wEquipValueStr.c_str(), wSkillIDStr2.c_str(), wSkillCoolTimeStr2.c_str(), nViewCashEquip, pQ->bExecuteScheduleedTask?1:0);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryMoveIntoNewZone(TQChangeStageUserData *pQ, OUT BYTE &cRebirthCoin, OUT BYTE &cPCBangRebirthCoin)
{
	CQueryTimeLog QueryTimeLog( "P_MoveIntoNewZone" );

	std::wstring wItemSerialStr, wItemDurStr;

	for(int i = 0; i < pQ->nCount; i++){
		if(pQ->ItemDurArr[i].biSerial <= 0) continue;
		if(!wItemSerialStr.empty()){
			wItemSerialStr.append(L",");
			wItemDurStr.append(L",");
		}
		wItemSerialStr.append(boost::lexical_cast<std::wstring>(pQ->ItemDurArr[i].biSerial));
		wItemDurStr.append(boost::lexical_cast<std::wstring>((short)(pQ->ItemDurArr[i].wDur)));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined( PRE_PARTY_DB )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveIntoNewZone2(%I64d,%d,%d,%I64d,%d,%I64d,%d,%d,N'%s',N'%s',%d,%d,%I64d,%I64d,%d)}", 
		pQ->biCharacterDBID, pQ->nChannelID, pQ->nMapID, pQ->PartyID, pQ->nExp, pQ->biCoin, pQ->cRebirthCoin, pQ->cPCBangRebirthCoin, wItemSerialStr.c_str(), wItemDurStr.c_str(),
		pQ->nDeathCount, pQ->cDifficult, pQ->biPickUpCoin, pQ->biPetItemSerial, pQ->nPetExp );
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveIntoNewZone(%I64d,%d,%d,%d,%d,%I64d,%d,%d,N'%s',N'%s',%d,%d,%I64d,%I64d,%d)}", 
		pQ->biCharacterDBID, pQ->nChannelID, pQ->nMapID, pQ->PartyID, pQ->nExp, pQ->biCoin, pQ->cRebirthCoin, pQ->cPCBangRebirthCoin, wItemSerialStr.c_str(), wItemDurStr.c_str(),
		pQ->nDeathCount, pQ->cDifficult, pQ->biPickUpCoin, pQ->biPetItemSerial, pQ->nPetExp );
#endif // #if defined( PRE_PARTY_DB )

	return CommonReturnValueQuery(m_wszQuery);
}

// 캐릭터 생성
#if defined(_LOGINSERVER)

int CDNSQLWorld::QueryAddCharacter(INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, char cAccountLevel, int nWorldID, WCHAR *pCharName, char cClass, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, 
								   int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, 
								   TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, bool bJoinBeginnerGuild, UINT &nGuildDBID, WCHAR* pwszIP )
{
	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[QueryAddCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	DWORD nSTick = timeGetTime();

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB, nLenParam = 0;
	CDNSqlLen dnSqlLen[2];

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#ifdef PRE_ADD_BEGINNERGUILD	
	WCHAR wszGuildTile[GUILDNAME_MAX*2+1];
	WCHAR wszGuildNotice[GUILDNOTICE_MAX*2+1];
	memset(wszGuildTile, 0, sizeof(wszGuildTile));
	memset(wszGuildNotice, 0, sizeof(wszGuildNotice));
	ConvertQuery(const_cast<WCHAR*>(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, BeginnerGuild::StringIndex::GuildTitle)), GUILDNAME_MAX, wszGuildTile, _countof(wszGuildTile));
	ConvertQuery(const_cast<WCHAR*>(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, BeginnerGuild::StringIndex::GuildNotice)), GUILDNOTICE_MAX, wszGuildNotice, _countof(wszGuildNotice));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddCharacter(%I64d,%d,N'%s',%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,?,%d,N'%s',N'%s',?)}", 
		biCharacterDBID, nAccountDBID, pAccountName, cAccountLevel, nWorldID, pCharName, cClass, cCharIndex, nEquipArray[EQUIP_BODY], nEquipArray[EQUIP_LEG], nEquipArray[EQUIP_HAND], nEquipArray[EQUIP_FOOT], 
		dwHairColor, dwEyeColor, dwSkinColor, nMapID, Pos.nX, Pos.nY, Pos.nZ, fRotate, nRebirthCoin, nPCBangRebirthCoin, g_pExtManager->GetGlobalWeightValue(BeginnerGuild_UserMax), 
		wszGuildTile, wszGuildNotice);
#else		//PRE_ADD_BEGINNERGUILD
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddCharacter(%I64d,%d,N'%s',%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d)}", 
		biCharacterDBID, nAccountDBID, pAccountName, cAccountLevel, nWorldID, pCharName, cClass, cCharIndex, nEquipArray[EQUIP_BODY], nEquipArray[EQUIP_LEG], nEquipArray[EQUIP_HAND], nEquipArray[EQUIP_FOOT], 
		dwHairColor, dwEyeColor, dwSkinColor, nMapID, Pos.nX, Pos.nY, Pos.nZ, fRotate, nRebirthCoin, nPCBangRebirthCoin);
#endif		//PRE_ADD_BEGINNERGUILD
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
#ifdef PRE_ADD_BEGINNERGUILD		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &bJoinBeginnerGuild, sizeof(bool), dnSqlLen[0].Get(sizeof(bool)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(UINT), 0, &nGuildDBID, sizeof(UINT), dnSqlLen[1].Get(sizeof(UINT)));
#endif		//#ifdef PRE_ADD_BEGINNERGUILD

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			// 아이템 생성
			TQAddMaterializedItem Item;
			wcsncpy( Item.wszIP, pwszIP, IPLENMAX );

			DWORD nStartTick = timeGetTime();
			for(int i = 0; i < EQUIPMAX; i++){
				if(nEquipArray[i] <= 0) continue;
				MakeAddMaterializedItem(nMapID, biCharacterDBID, i, DBDNWorldDef::ItemLocation::Equip, nEquipArray[i], 1, rand(), g_pExtManager->GetItemDurability(nEquipArray[i]), Item);
				if(QueryAddMaterializedItem(&Item) != ERROR_NONE) return ERROR_DB;
			}
			DWORD nEndTick = timeGetTime();
			if((nEndTick - nStartTick) > 3000)
				g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddCharacter(ITEMPOSITION_EQUIP)] [%s] time=%f \n", m_wszQuery,(float)((nEndTick - nStartTick)*0.001f));

			if(cCreateItemMax > 0){
				nStartTick = timeGetTime();
				for(int i = 0; i < cCreateItemMax; i++){
					MakeAddMaterializedItem(nMapID, biCharacterDBID, i, DBDNWorldDef::ItemLocation::Inventory, CreateItemArray[i].nItemID, CreateItemArray[i].cCount, 0, CreateItemArray[i].wDur, Item);
					if(QueryAddMaterializedItem(&Item) != ERROR_NONE) return ERROR_DB;
				}
				nEndTick = timeGetTime();
				if((nEndTick - nStartTick) > 3000)
					g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddCharacter(ITEMPOSITION_INVEN)] [%s] time=%f \n", m_wszQuery,(float)((nEndTick - nStartTick)*0.001f));
			}

			if (nResult == ERROR_NONE)
				nResult = QueryFirstUseCharacter(biCharacterDBID, nSkillArray, nUnlockSkillArray, QuickSlotArray);
		}
	}

	DWORD nETick = timeGetTime();
	if((nETick - nSTick) > 3000)
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddCharacter] Total time=%f \n", m_wszQuery,(float)((nETick - nSTick)*0.001f));

	return nResult;
}

#if defined( PRE_ADD_DWC )
int CDNSQLWorld::QueryAddDWCCharacter(INT64 biCharacterDBID, UINT nAccountDBID, WCHAR *pAccountName, int nWorldID, WCHAR *pCharName, char cClass, char cJobCode1, char cJobCode2, char cLevel, int nExp, char cCharIndex, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, 
						 int nMapID, TPosition &Pos, float fRotate, int nRebirthCoin, int nPCBangRebirthCoin, int *nEquipArray, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray, 
						 TCreateCharacterItem *CreateItemArray, BYTE cCreateItemMax, WCHAR* pwszIP, short nSkillPoint, int nGold )
{
	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[QueryAddDWCCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	DWORD nSTick = timeGetTime();

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen1;
	
	int nResult = ERROR_DB;
	int nDeleteWaitingTime = 0;
#if defined(_FINAL_BUILD)
	nDeleteWaitingTime = DELETECHAR_WAITTIME_MINUTE; // Final일때 DWC캐릭터의 즉시 삭제는 불가능 하다.
#endif // #if defined(_FINAL_BUILD)

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDWCCharacter(%I64d,%d,N'%s',%d,N'%s',%d,%d,?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,%d,%d,%d)}", 
		biCharacterDBID, nAccountDBID, pAccountName, nWorldID, pCharName, cClass, cJobCode1, cLevel, nExp, cCharIndex, nEquipArray[EQUIP_BODY], nEquipArray[EQUIP_LEG], nEquipArray[EQUIP_HAND], nEquipArray[EQUIP_FOOT], 
		dwHairColor, dwEyeColor, dwSkinColor, nMapID, Pos.nX, Pos.nY, Pos.nZ, fRotate, nRebirthCoin, nPCBangRebirthCoin, nDeleteWaitingTime, nGold, nSkillPoint );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cJobCode2, sizeof(char), SqlLen1.GetNull(cJobCode2 == 0, sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			// 아이템 생성
			TQAddMaterializedItem Item;
			wcsncpy( Item.wszIP, pwszIP, IPLENMAX );

			DWORD nStartTick = timeGetTime();
			for(int i = 0; i < EQUIPMAX; i++){
				if(nEquipArray[i] <= 0) continue;
				MakeAddMaterializedItem(nMapID, biCharacterDBID, i, DBDNWorldDef::ItemLocation::Equip, nEquipArray[i], 1, rand(), g_pExtManager->GetItemDurability(nEquipArray[i]), Item);
				Item.AddItem.bSoulbound = true;
				if(QueryAddMaterializedItem(&Item) != ERROR_NONE) return ERROR_DB;
			}
			DWORD nEndTick = timeGetTime();
			if((nEndTick - nStartTick) > 3000)
				g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddDWCCharacter(ITEMPOSITION_EQUIP)] [%s] time=%f \n", m_wszQuery,(float)((nEndTick - nStartTick)*0.001f));

			if(cCreateItemMax > 0){
				nStartTick = timeGetTime();
				for(int i = 0; i < cCreateItemMax; i++){
					MakeAddMaterializedItem(nMapID, biCharacterDBID, i, DBDNWorldDef::ItemLocation::Inventory, CreateItemArray[i].nItemID, CreateItemArray[i].cCount, 0, CreateItemArray[i].wDur, Item);
					Item.AddItem.bSoulbound = true;
					if(QueryAddMaterializedItem(&Item) != ERROR_NONE) return ERROR_DB;
				}
				nEndTick = timeGetTime();
				if((nEndTick - nStartTick) > 3000)
					g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddDWCCharacter(ITEMPOSITION_INVEN)] [%s] time=%f \n", m_wszQuery,(float)((nEndTick - nStartTick)*0.001f));
			}

			if (nResult == ERROR_NONE)
				nResult = QueryFirstUseCharacter(biCharacterDBID, nSkillArray, nUnlockSkillArray, QuickSlotArray);
		}
	}

	DWORD nETick = timeGetTime();
	if((nETick - nSTick) > 3000)
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, biCharacterDBID, 0, L"[Query Over Time:QueryAddDWCCharacter] Total time=%f \n", m_wszQuery,(float)((nETick - nSTick)*0.001f));

	return nResult;
}

int CDNSQLWorld::QueryGetDWCCharacterID(UINT nAccountDBID, OUT INT64& biDWCCharacterDBID, int nDeleteWaitingTime)
{
	CQueryTimeLog QueryTimeLog( "P_GetDWCCharacterID" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[QueryGetDWCCharacterID] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDWCCharacterID(%d,%d)}", nAccountDBID, nDeleteWaitingTime );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;			
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &biDWCCharacterDBID, sizeof(INT64), &cblen );
			CheckColumnCount(nNo, "P_GetDWCCharacterID");
			RetCode = SQLFetch(m_hstmt);
			if (RetCode == SQL_SUCCESS || RetCode == SQL_NO_DATA)
			{					
				nResult = ERROR_NONE;
			}
		}	
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}
#endif // #if defined( PRE_ADD_DWC )

#endif	// #if defined(_LOGINSERVER)

int CDNSQLWorld::QueryRollbackAddCharacter(INT64 biCharacterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_RollbackAddCharacter" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_RollbackAddCharacter(%I64d)}", biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryFirstUseCharacter(INT64 biCharacterDBID, int *nSkillArray, int *nUnlockSkillArray, TQuickSlot *QuickSlotArray)
{
	CQueryTimeLog QueryTimeLog( "P_FirstUseCharacter" );

	std::wstring wSkillIDStr, wSkillLevelStr, wTypeStr, wValueStr;
	for(int i = 0; i < DEFAULTSKILLMAX; i++){
		if( nSkillArray[i] > 0 ){
			if( !wSkillIDStr.empty() ) wSkillIDStr.append(L",");
			wSkillIDStr.append(boost::lexical_cast<std::wstring>(nSkillArray[i]));
			if( !wSkillLevelStr.empty() ) wSkillLevelStr.append(L",");
			wSkillLevelStr.append(boost::lexical_cast<std::wstring>(1));
		}
	}
	for(int i = 0; i < DEFAULTUNLOCKSKILLMAX; i++){
		if(nUnlockSkillArray[i] > 0){
			if( !wSkillIDStr.empty() ) wSkillIDStr.append(L",");
			wSkillIDStr.append(boost::lexical_cast<std::wstring>(nUnlockSkillArray[i]));
			if( !wSkillLevelStr.empty() ) wSkillLevelStr.append(L",");
			wSkillLevelStr.append(boost::lexical_cast<std::wstring>(0));
		}
	}
	for(int i = 0; i < DEFAULTQUICKSLOTMAX; i++){
		if(i > 0) wTypeStr.append(L",");
		wTypeStr.append(boost::lexical_cast<std::wstring>(QuickSlotArray[i].cType));
		if(i > 0) wValueStr.append(L",");
		wValueStr.append(boost::lexical_cast<std::wstring>(QuickSlotArray[i].nID));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_FirstUseCharacter(%I64d,N'%s',N'%s',N'%s',N'%s',%d,%d)}", biCharacterDBID, wSkillIDStr.c_str(), wSkillLevelStr.c_str(), wTypeStr.c_str(), wValueStr.c_str(), DNNotifier::RegisterCount::Total, MAX_PLAY_QUEST);

	return CommonReturnValueQuery(m_wszQuery);
}

// 캐릭터 삭제
int CDNSQLWorld::QueryDelCharacter(INT64 biCharacterDBID, OUT bool &bVillageFirstVisit, OUT TIMESTAMP_STRUCT& DeleteDate )
{
	CQueryTimeLog QueryTimeLog( "P_DelCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryDelCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	CDNSqlLen SqlLen;
	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_FIX_65655)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelCharacter(%I64d,?,?,0,2)}", biCharacterDBID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelCharacter(%I64d,?,?)}", biCharacterDBID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bVillageFirstVisit, sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &DeleteDate, sizeof(TIMESTAMP_STRUCT), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryReviveCharacter( INT64 biCharacterDBID )
{
	CQueryTimeLog QueryTimeLog( "P_ReviveCharacter" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ReviveCharacter(%I64d,%d)}", biCharacterDBID, DELETECHAR_WAITTIME_MINUTE );

	return CommonReturnValueQuery(m_wszQuery);
}

// 캐릭터 속성 조회
#if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
int CDNSQLWorld::QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList, INT64 biDWCCharacterDBID, BYTE cAccountLevel)
#else // #if defined( PRE_ADD_DWC )
int CDNSQLWorld::QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT std::map<INT64, TDBListCharData> &MapCharacterList)
#endif // #if defined( PRE_ADD_DWC )
#else	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
int CDNSQLWorld::QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT TDBListCharData *CharList, INT64 biDWCCharacterDBID, BYTE cAccountLevel)
#else // #if defined( PRE_ADD_DWC )
int CDNSQLWorld::QueryGetListCharacter(UINT nAccountDBID, int nWorldID, int nCharacterMaxCount, OUT TDBListCharData *CharList)
#endif // #if defined( PRE_ADD_DWC )
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
{
	CQueryTimeLog QueryTimeLog( "P_GetListCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, 0, 0, L"[QueryGetListCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[3];

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 10;
#if defined(PRE_CHARLIST_SORTING)
	nVersion = 11;
#endif	// #if defined(PRE_CHARLIST_SORTING)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListCharacter2(?,?,?,%d)}", nVersion);

#if !defined(PRE_MOD_SELECT_CHAR)
	int nCount = 0;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(UINT), 0, &nAccountDBID, sizeof(UINT), SqlLen[0].Get(sizeof(UINT)));
		int iDeleteWaitingTime = DELETECHAR_WAITTIME_MINUTE;
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iDeleteWaitingTime, sizeof(int), SqlLen[1].Get(sizeof(int)));
		SQLBindParameter(m_hstmt, 4, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nWorldID, sizeof(int), SqlLen[2].GetNull(nWorldID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TDBListCharData CharData;
			TIMESTAMP_STRUCT DeleteDate = {0, }, LastLoginDate = {0,}, CreateDate = {0,};
			memset(&CharData, 0, sizeof(CharData));
#if defined(PRE_MOD_SELECT_CHAR)
			char cCharIndex = 0;
#endif	// #if defined(PRE_MOD_SELECT_CHAR)

#if defined( PRE_ADD_DWC )
			WCHAR wszTempCharacterName[NAMELENMAX+DWCPREFIXLEN] = L"";
#endif // #if defined( PRE_ADD_DWC )

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &CharData.biCharacterDBID, sizeof(INT64), &cblen);
#if defined(PRE_MOD_SELECT_CHAR)
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cCharIndex, sizeof(char), &cblen);
#else	// #if defined(PRE_MOD_SELECT_CHAR)
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &CharData.cCharIndex, sizeof(char), &cblen);
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, wszTempCharacterName, sizeof(WCHAR) * (NAMELENMAX+DWCPREFIXLEN), &cblen);
#else // #if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, CharData.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);
#endif // #if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &CharData.cLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &CharData.cJob, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nLastVillageMapID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nLastSubVillageMapID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nMapID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nDefaultBody, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nDefaultLeg, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nDefaultHand, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.nDefaultFoot, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.dwHairColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.dwEyeColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CharData.dwSkinColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &CharData.bVillageFirstVisit, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_BINARY, &CharData.nChecksum, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &CharData.bDeleteFlag, sizeof(bool), &cblen);			
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &DeleteDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			if (nVersion >= 11){
				SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastLoginDate, sizeof(TIMESTAMP_STRUCT), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &CreateDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			}
			CheckColumnCount(nNo, "P_GetListCharacter");

			while(1)
			{
				memset(&CharData, 0, sizeof(CharData));
				memset(&DeleteDate, 0, sizeof(DeleteDate));
				memset(&LastLoginDate, 0, sizeof(LastLoginDate));
				memset(&CreateDate, 0, sizeof(CreateDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO)) break;				

				CTimeParamSet DeleteTime(&QueryTimeLog, CharData.DeleteDate, DeleteDate, nWorldID, nAccountDBID);
				if (!DeleteTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}
#if defined(PRE_CHARLIST_SORTING)
				CTimeParamSet LastLoginDate(&QueryTimeLog, CharData.LastLoginDate, LastLoginDate, nWorldID, nAccountDBID);
				if (!LastLoginDate.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}
				CTimeParamSet CreateDate(&QueryTimeLog, CharData.CreateDate, CreateDate, nWorldID, nAccountDBID);
				if (!CreateDate.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}
#endif	// #if defined(PRE_CHARLIST_SORTING)

#if defined( PRE_ADD_DWC )
				CharData.cAccountLevel = cAccountLevel;
				BYTE cPrefixOffset = 0;
				if(CharData.biCharacterDBID == biDWCCharacterDBID)
				{
					CharData.cAccountLevel = AccountLevel_DWC;
					cPrefixOffset = DWCPREFIXLEN;
				}
				_wcscpy(CharData.wszCharacterName, NAMELENMAX, wszTempCharacterName + cPrefixOffset, NAMELENMAX);
#endif // #if defined( PRE_ADD_DWC )

#if defined(PRE_MOD_SELECT_CHAR)
				CharData.cWorldID = nWorldID;
				MapCharacterList.insert(std::make_pair(CharData.biCharacterDBID, CharData));

				if ((int)MapCharacterList.size() >= nCharacterMaxCount){
					nResult = ERROR_NONE;
					break;
				}
#else	// #if defined(PRE_MOD_SELECT_CHAR)
				CharList[CharData.cCharIndex] = CharData;
				nCount++;

				if(nCount >= nCharacterMaxCount){
					nResult = ERROR_NONE;
					break;
				}
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy1(INT64 biCharacterDBID, WCHAR *pCharName, TAGetCharacterPartialy *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy1" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCharacterPartialy] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,?,N'%s',%d)}", 1, pCharName, 14);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(biCharacterDBID <= 0, sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &pA->biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &pA->cClass, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &pA->cLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &pA->cJob, sizeof(BYTE), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy1");
			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				nResult = ERROR_NONE;
				return nResult;
			}

			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy4(INT64 biCharacterDBID, WCHAR *pCharName, OUT BYTE &cClassID, OUT BYTE &cLevel)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy4" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCharacterPartialy] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,?,?,%d)}", 4, 14);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(biCharacterDBID <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(WCHAR)*NAMELENMAX, 0, pCharName, sizeof(WCHAR)*NAMELENMAX, SqlLen[sqlparam++].GetNull(pCharName == NULL, sizeof(WCHAR)*(pCharName ? (int)wcslen(pCharName) : 0 )) );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &cLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &cClassID, sizeof(BYTE), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy4");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy6(WCHAR *pCharName, OUT BYTE &cJob, OUT BYTE &cLevel)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy6" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryGetCharacterPartialy6] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,?,N'%s',%d)}", 6, pCharName, 14);


	INT64 biCharacterDBID = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(biCharacterDBID <= 0, sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &cLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_TINYINT, &cJob, sizeof(BYTE), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy6");
			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA)
			{
				// 여기는 record 가 있다고 가정하고 call 하는 sp 이기 때문에 NODATA 일때 result setting 하지 않는다.
			}
			else
			{
				CheckRetCode(RetCode, L"SQLFetch");
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy7(WCHAR *pCharName, OUT UINT &nAccountDBID, OUT INT64 &biCharacterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy7" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryGetCharacterPartialy7] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,?,N'%s',%d)}", 7, pCharName, 14);

	INT64 biCharDBID = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(biCharDBID <= 0, sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nAccountDBID, sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy7");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy8(INT64 biCharacterDBID, WCHAR *pCharName, int &nWorldID)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy8" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCharacterPartialy8] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,?,?,%d)}", 8, 14);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(biCharacterDBID <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(WCHAR)*NAMELENMAX, 0, pCharName, sizeof(WCHAR)*NAMELENMAX, SqlLen[sqlparam++].GetNull(pCharName == NULL, sizeof(WCHAR) * (pCharName ? (int)wcslen(pCharName) : 0) ));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nWorldID, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy8");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCharacterPartialy10(WCHAR *pCharName, OUT char* pAccountName)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPartialy10" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[P_GetCharacterPartialy10] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterPartialy(%d,0,N'%s',%d)}", 10, pCharName, 14);

	INT64 biCharDBID = 0;
	int nAccountID = 0;
	INT64 biCharacterDBID = 0;
	WCHAR wszAccountName[IDLENMAX];	// 계정이름
	memset(&wszAccountName, 0, sizeof(wszAccountName));

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nAccountID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, wszAccountName, sizeof(wszAccountName), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			CheckColumnCount(nNo, "P_GetCharacterPartialy10");
			RetCode = SQLFetch(m_hstmt);
			if( RetCode == SQL_NO_DATA )
			{
				SQLCloseCursor(m_hstmt);
				return nResult;
			}
			if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
			{
				nResult = ERROR_DB;
				SQLCloseCursor(m_hstmt);
				return nResult;
			}
			WideCharToMultiByte(CP_ACP, 0, wszAccountName, -1, pAccountName, IDLENMAX, NULL, NULL);			
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetListEquipmentAttributes(INT64 biCharacterDBID, OUT TCharacterStatus &CharStatus)
{
	CQueryTimeLog QueryTimeLog( "P_GetListEquipmentAttributes" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListEquipmentAttributes] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListEquipmentAttributes(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			char cType = 0, cIndex = 0;
			int nValue = 0;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cType, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nValue, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetListEquipmentAttributes");
			while(1)
			{
				cType = 0;
				cIndex = 0;
				nValue = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				switch(cType){
				case DBDNWorldDef::EquipmentAttributeCode::DelayTime:
					if(cIndex == 20)
						CharStatus.nGlyphDelayTime = nValue;
					break;

				case DBDNWorldDef::EquipmentAttributeCode::RemainTime:
					if(cIndex == 20)
						CharStatus.nGlyphRemainTime = nValue;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetEquipmentAttribute(INT64 biCharacterDBID, char cEquipTimeType, char cEquipAttributeCode, int nValue, OUT int &nEquipAttributeValue)
{
	CQueryTimeLog QueryTimeLog( "P_GetEquipmentAttribute" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetEquipmentAttribute] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetEquipmentAttribute(%I64d,%d,%d,?)}", biCharacterDBID, cEquipTimeType, cEquipAttributeCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nEquipAttributeValue, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetCharacterLevelList(UINT nAccountDBID, int nWorldID, std::vector<TChracterLevel> &VecLevelList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListCharacterLevel" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, nWorldID, nAccountDBID, 0, 0, L"[QueryGetCharacterLevelList] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[1];

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListCharacterLevel(%d,?)}", nAccountDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nWorldID, sizeof(int), SqlLen[0].Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){

			int nNo = 1;
			TChracterLevel Level;
			memset(&Level, 0, sizeof(TChracterLevel));
			
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &Level.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &Level.cLevel, sizeof(char), &cblen);
			CheckColumnCount(nNo, "P_GetListCharacterLevel");

			while(1)
			{
				memset(&Level, 0, sizeof(Level));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				VecLevelList.push_back(Level);
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

#if defined( PRE_ADD_CHARACTERCHECKSUM )
int CDNSQLWorld::QueryGetCharacterCheckSum(INT64 biCharacterDBID, OUT TASelectCharacter *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterCheckSum" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryGetCharacterCheckSum] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterCheckSum(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			TIMESTAMP_STRUCT LastLoginDate = { 0, };
			TIMESTAMP_STRUCT LastLogoutDate = { 0, };

			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastLoginDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastLogoutDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_BINARY, &pA->uiCheckSum, sizeof(pA->uiCheckSum), &cblen);

			CheckColumnCount(nNo, "P_GetCharacterCheckSum");
			RetCode = SQLFetch(m_hstmt);
			if( RetCode == SQL_NO_DATA )
			{
				SQLCloseCursor(m_hstmt);
				return nResult;
			}
			if( CheckRetCode(RetCode, L"SQLFetch") == ERROR_NONE )
			{
				CTimeParamSet LastLoginTime(&QueryTimeLog, pA->tLastLoginDate, LastLoginDate, 0, 0, biCharacterDBID);
				if (!LastLoginTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet LastLogoutTime(&QueryTimeLog, pA->tLastLogoutDate, LastLogoutDate, 0, 0, biCharacterDBID);
				if (!LastLogoutTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
			}			
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryMakeCharacterCheckSum(TQHeader * pHeader, BYTE cReason, INT64 biCharacterDBID, OUT UINT& uiCheckSum, INT64 biChangeCoin)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacterInfo4CheckSum" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, biCharacterDBID, 0, L"[QueryGetCharacterInfo4CheckSum:%d] CheckConnect Fail\r\n", cReason);
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	uiCheckSum = 0;
	BYTE cLevel = 0;
	int nExp = 0;
	INT64 biCoin = 0;
	INT64 biWarehouseCoin = 0;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacterInfo4CheckSum(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;			

			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cLevel, sizeof(SQL_TINYINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nExp, sizeof(SQL_INTEGER), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biCoin, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biWarehouseCoin, sizeof(INT64), &cblen);

			CheckColumnCount(nNo, "P_GetCharacterInfo4CheckSum");
			RetCode = SQLFetch(m_hstmt);
			if( RetCode == SQL_NO_DATA )
			{
				g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, biCharacterDBID, 0, L"[QueryGetCharacterInfo4CheckSum:%d] No Data\r\n", cReason);
				SQLCloseCursor(m_hstmt);
				return ERROR_DB;
			}
			if( CheckRetCode(RetCode, L"SQLFetch") != ERROR_NONE )
			{
				g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, biCharacterDBID, 0, L"[QueryGetCharacterInfo4CheckSum:%d] Error(result:%d)\r\n", cReason, nResult);
				SQLCloseCursor(m_hstmt);
				return ERROR_DB;
			}			
		}
	}
	SQLCloseCursor(m_hstmt);

	biCoin = biCoin + biChangeCoin;
	uiCheckSum = MakeCharacterCheckSum( biCharacterDBID, cLevel, nExp, biCoin, biWarehouseCoin );
	
	g_Log.Log(LogType::_NORMAL, pHeader->cWorldSetID, pHeader->nAccountDBID, biCharacterDBID, 0, L"Make Character CheckSum(reason:%d, cid:%I64d, level:%d, exp:%d, coin:%I64d, wcoin:%I64d, checksum:%x)\r\n", cReason, biCharacterDBID, cLevel, nExp, biCoin, biWarehouseCoin, uiCheckSum );

	return nResult;
}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
int CDNSQLWorld::QueryGetTotalSkillLevel(TQGetTotalLevelSkill* pQ, TAGetTotalLevelSkill* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetUnifiedLevel" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QUERY_GET_TOTALSKILLLEVEL] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetUnifiedLevel(%d)}", pQ->nAccountDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->nTotalSkillLevel, sizeof(int), &cblen);			
			CheckColumnCount(nNo, "P_GetUnifiedLevel");
			RetCode = SQLFetch(m_hstmt);
			if( RetCode == SQL_NO_DATA )
			{
				SQLCloseCursor(m_hstmt);
				return nResult;
			}
			if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
			{
				nResult = ERROR_DB;
				SQLCloseCursor(m_hstmt);
				return nResult;
			}			
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetTotalSkillList(TQGetTotalLevelSkill* pQ, TAGetTotalLevelSkill* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListUnifiedLevelSkill" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QUERY_GET_TOTALSKILLLEVEL] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListUnifiedLevelSkill(%I64d)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){

			int nNo = 1;			
			INT64 biCharacterDBID = 0;
			TotalLevelSkill::TTotalLevelSkillData TotalSkillData;			

			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &TotalSkillData.nSlotIndex, sizeof(int), &cblen);
			SQLBindCol( m_hstmt,nNo++, SQL_INTEGER, &TotalSkillData.nSkillID, sizeof(int), &cblen );
			
			CheckColumnCount(nNo, "P_GetListUnifiedLevelSkill");

			while(1)
			{
				memset(&TotalSkillData, 0, sizeof(TotalSkillData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;				
				
				if( TotalSkillData.nSlotIndex >= TotalLevelSkill::Common::MAXSLOTCOUNT )
				{
					continue;;
				}

				pA->TotalSkill[TotalSkillData.nSlotIndex] = TotalSkillData;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}
int CDNSQLWorld::QueryAddTotalSkill(TQAddTotalLevelSkill* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddUnifiedLevelSkill" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddUnifiedLevelSkill(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->nSlotIndex, pQ->nSkillID);

	return CommonReturnValueQuery(m_wszQuery);
}

#endif

#ifdef PRE_ADD_PRESET_SKILLTREE
int CDNSQLWorld::QueryGetSKillPresetIndexList(TQGetSKillPreSetList * pQ, TAGetSKillPreSetList * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListSkillPreset" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetSKillPresetList] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nCount = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListSkillPreset(%I64d)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			TSkillSetIndexData SetIndex;
			memset(&SetIndex, 0, sizeof(TSkillSetIndexData));

			SQLBindCol(m_hstmt, nNo++, SQL_C_STINYINT, &SetIndex.cIndex, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &SetIndex.wszSetName,	sizeof(WCHAR) * (SKILLPRESETNAMEMAX), &cblen);

			CheckColumnCount(nNo, "P_GetListSkillPreset");
			while(1)
			{
				memset(&SetIndex, 0, sizeof(TSkillSetIndexData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					pA->nRetCode = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				if (SetIndex.cIndex >= SKILLPRESETMAX)
				{
					pA->nRetCode = ERROR_DB;
					break;
				}

				pA->SKillIndex[SetIndex.cIndex] = SetIndex;
				nCount++;				
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return pA->nRetCode;
}

int CDNSQLWorld::QueryGetSKillPresetList(TQGetSKillPreSetList * pQ, BYTE cIndex, TSkillSetPartialData * pSkill, BYTE &cCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetListPresetSkills" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetSKillPresetList] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nRetCode = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPresetSkills(%I64d,%d)}", pQ->biCharacterDBID, cIndex);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRetCode, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			TSkillSetPartialData SKillData;
			memset(&SKillData, 0, sizeof(TSkillSetPartialData));

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &SKillData.nSkillID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_STINYINT, &SKillData.cLevel, sizeof(BYTE), &cblen);

			CheckColumnCount(nNo, "P_GetListPresetSkills");
			while(1)
			{
				memset(&SKillData, 0, sizeof(TSkillSetPartialData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nRetCode = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				if(cCount >= SKILLMAX)
				{
					_DANGER_POINT();
					break;
				}
				pSkill[cCount] = SKillData;
				++cCount;				
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nRetCode;
}

int CDNSQLWorld::QueryAddSkillPreset(TQAddSkillPreSet * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddSkillPreset" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryAddSkillPreset] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wstrSKillIDs, wstrSKillLevel;
	for (int i = 0; i < pQ->cCount; i++)
	{
		wstrSKillIDs.append(boost::lexical_cast<std::wstring>((int)(pQ->SKill[i].nSkillID)));
		wstrSKillLevel.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->SKill[i].cLevel)));

		if (i < pQ->cCount -1)
		{
			wstrSKillIDs.append(L",");
			wstrSKillLevel.append(L",");
		}
	}

	WCHAR wszPresetName[SKILLPRESETNAMEMAX * 2 + 1] = {0,};	
	ConvertQuery(pQ->wszName, SKILLPRESETNAMEMAX, wszPresetName, _countof(wszPresetName));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddSkillPreset(%I64d,%d,N'%s',N'%s',N'%s')}", pQ->biCharacterDBID, pQ->cIndex, pQ->wszName, wstrSKillIDs.c_str(), wstrSKillLevel.c_str());
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelSkillPreset(TQDelSkillPreSet * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_DelSkillPreset" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryDelSkillPreset] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelSkillPreset(%I64d,%d)}", pQ->biCharacterDBID, pQ->cIndex);
	return CommonReturnValueQuery(m_wszQuery);
}
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE

// 캐릭터 속성 변경
int CDNSQLWorld::QueryVisitFirstVillage(INT64 biCharacterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_VisitFirstVillage" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_VisitFirstVillage(%I64d)}", biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModCharacterLevel(INT64 biCharacterDBID, char cLevelChangeCode, BYTE cLevel, int nChannelID, int nMapID)
{
	CQueryTimeLog QueryTimeLog( "P_ModCharacterLevel" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModCharacterLevel(%I64d,%d,%d,%d,%d)}", biCharacterDBID, cLevelChangeCode, cLevel, nChannelID, nMapID);
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModCharacterExp(INT64 biCharacterDBID, char cExpChangeCode, int nExp, int nChannelID, int nMapID, INT64 nFKey)
{
	CQueryTimeLog QueryTimeLog( "P_ModCharacterExp" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModCharacterExp(%I64d,%d,%d,%d,%d,%I64d)}", biCharacterDBID, cExpChangeCode, nExp, nChannelID, nMapID, nFKey);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModCoin(INT64 biCharacterDBID, char cCoinChangeCode, INT64 biCoinChangeKey, INT64 biCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin)
{
	CQueryTimeLog QueryTimeLog( "P_ModCoin" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryModCoin] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModCoin(%I64d,%d,%I64d,%I64d,%d,%d,?)}", biCharacterDBID, cCoinChangeCode, biCoinChangeKey, biCoin, nChannelID, nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCurrentCoin, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryMidtermCoin(INT64 biCharacterDBID, INT64 biPickUpCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin)
{
	CQueryTimeLog QueryTimeLog( "P_StageMidtermCoin" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryMidtermCoin] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_StageMidtermCoin(%I64d,%I64d,%d,%d,?)}", biCharacterDBID, biPickUpCoin, nChannelID, nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCurrentCoin, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModWarehouseCoin(INT64 biCharacterDBID, char cCoinChangeCode, INT64 biCoin, int nChannelID, int nMapID, OUT INT64 &biCurrentCoin, OUT INT64 &biCurrentWarehouseCoin)
{
	CQueryTimeLog QueryTimeLog( "P_ModWarehouseCoin" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryModWarehouseCoin] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModWarehouseCoin(%I64d,%d,%I64d,%d,%d,?,?)}", biCharacterDBID, cCoinChangeCode, biCoin, nChannelID, nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCurrentCoin, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCurrentWarehouseCoin, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModCharacterStatus(INT64 biCharacterDBID, int nModFieldBit, DWORD dwHairColor, DWORD dwEyeColor, DWORD dwSkinColor, int nMapID, int nLastVillageMapID, int nLastDarkLairVillageMapID, 
										 char cLastVillageGateNo, TPosition &Pos, float fRotate, bool bViewCashWeapon1, bool bViewCashWeapon2, bool bLastLogoutDate, bool bLastLoginDate, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_ModCharacterStatus" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryModWarehouseCoin] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[3];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModCharacterStatus(%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d,?,?,?,%d)}", \
		biCharacterDBID, nModFieldBit, dwHairColor, dwEyeColor, dwSkinColor, nMapID, nLastVillageMapID, nLastDarkLairVillageMapID, cLastVillageGateNo, Pos.nX, Pos.nY, Pos.nZ, fRotate, bViewCashWeapon1, bViewCashWeapon2, cSkillPage);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &bLastLogoutDate, sizeof(bool), SqlLen[sqlparam++].GetNull(bLastLogoutDate == false, sizeof(bool)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &bLastLoginDate, sizeof(bool), SqlLen[sqlparam++].GetNull(bLastLoginDate == false, sizeof(bool)));
		// by leekh - 검토해봐야 할 코드
		TIMESTAMP_STRUCT* pLastDailyCheckDate = NULL;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, pLastDailyCheckDate, sizeof(TIMESTAMP_STRUCT), SqlLen[sqlparam++].GetNull(!pLastDailyCheckDate, SQL_TIMESTAMP_LEN));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryUpsCharacterAbility(INT64 biCharacterDBID, char cAbilityCode, int nAbilityPoint)
{
	CQueryTimeLog QueryTimeLog( "P_UpsCharacterAbility" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsCharacterAbility(%I64d,%d,%d)}", biCharacterDBID, cAbilityCode, nAbilityPoint);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryUpsEquipmentAttribute(INT64 biCharacterDBID, char cEquipTimeType, char cEquipAttributeCode, int nValue)
{
	CQueryTimeLog QueryTimeLog( "P_UpsEquipmentAttribute" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsEquipmentAttribute(%I64d,%d,%d,%d)}", biCharacterDBID, cEquipTimeType, cEquipAttributeCode, nValue);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModFatigue(TQFatigue *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModFatigue" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFatigue(%I64d,%d,%d,%d,%d)}",	pQ->biCharacterDBID, pQ->cFatigueTypeCode, pQ->nChangeFatigue, pQ->nChannelID, pQ->nMapID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModEventFatigue(TQEventFatigue *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModEventFatigue" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModEventFatigue(%I64d,%d,%d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->nChangeFatigue, pQ->bReset, pQ->cInitTime, pQ->nChannelID, pQ->nMapID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModRebirthCoin(INT64 biCharacterDBID, int nRebirthCoin, int nPCBangRebirthCoin)
{
	CQueryTimeLog QueryTimeLog( "P_ModRebirthCoin" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModRebirthCoin(%I64d,%d,%d)}", biCharacterDBID, nRebirthCoin, nPCBangRebirthCoin);

	return CommonReturnValueQuery(m_wszQuery);
}

// 캐쉬 부활 코인
int CDNSQLWorld::QueryAddCashRebirthCoin(INT64 biCharacterDBID, int nRebirthCount, int nRebirthPrice, int nRebirthCode, int nRebirthKey)
{
	CQueryTimeLog QueryTimeLog( "P_AddCashRebirthCoin" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddCashRebirthCoin(%I64d,%d,%d,%d,%I64d)}", biCharacterDBID, nRebirthCount, nRebirthPrice, nRebirthCode, nRebirthKey);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryUseCashRebirthCoin(INT64 biCharacterDBID, int nChannelID, int nMapID, const WCHAR* pwszIP )
{
	CQueryTimeLog QueryTimeLog( "P_UseCashRebirthCoin" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UseCashRebirthCoin(%I64d,%d,%d,N'%s')}", biCharacterDBID, nChannelID, nMapID, pwszIP );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddEffectItems( TQAddEffectItems* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddEffectItems" );
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[3];

	int nResult = ERROR_DB, sqlparam = 0;
	
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddEffectItems(%I64d,%d,%d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d,?)}"
		, pQ->biCharacterDBID, pQ->cItemLifeSpanRenewal, pQ->iItemID, pQ->Code, pQ->biFKey, pQ->iPrice, pQ->bEternityFlag?1:0
		, pQ->iItemLifeSpan, pQ->iProperty[0], pQ->iProperty[1], pQ->iProperty[2], pQ->iProperty[3], pQ->iProperty[4], pQ->biItemSerial);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iRemoveItemID, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->iRemoveItemID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListEffectItem( INT64 biCharacterDBID, std::vector<TEffectItemInfo>& vEffectItem )
{
	CQueryTimeLog QueryTimeLog( "P_GetListEffectItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListEffectItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListEffectItem(%I64d)}", biCharacterDBID );

	int iSQLResult	= ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TEffectItemInfo ItemInfo;
			memset( &ItemInfo, 0, sizeof(ItemInfo) );
			TIMESTAMP_STRUCT ExpireDate = {0,};

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iItemID,			sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_BIT,		&ItemInfo.bEternityFlag,	sizeof(bool),				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TIMESTAMP,	&ExpireDate,				sizeof(TIMESTAMP_STRUCT),	&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iProperty[0],		sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iProperty[1],		sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iProperty[2],		sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iProperty[3],		sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.iProperty[4],		sizeof(int), 				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT,	&ItemInfo.biItemSerial,		sizeof(INT64), 				&cblen );
			CheckColumnCount(nNo, "P_GetListEffectItem");
			while(1)
			{
				memset( &ItemInfo, 0, sizeof(ItemInfo) );
				memset( &ExpireDate, 0, sizeof(ExpireDate) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				if(!ItemInfo.bEternityFlag) 
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, ItemInfo.ExpireDate, ExpireDate, 0, 0, biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						iSQLResult = ERROR_DB;
						break;									
					}
				}

				vEffectItem.push_back( ItemInfo );
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryModAdditiveItem(TQModAdditiveItem* pQ, TAModAdditiveItem* pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModAdditiveItemID" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModAdditiveItemID(%I64d,%d,%I64d)}", pQ->biItemSerial, pQ->nAdditiveItemID, pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

#if defined (PRE_ADD_COSRANDMIX)
int CDNSQLWorld::QueryModRandomItem(TQModRandomItem* pQ, TAModRandomItem* pA)
{
	CQueryTimeLog QueryTimeLog( "P_MakeItemFromItems" );

	// 삭제할 아이템 시리얼 문자열 변환
	std::wstring wItemSerialStr;
	std::wstring wItemCountStr;
	std::wstring wItemFlagStr;
	if (pQ->biStuffSerialA > 0){
		wItemSerialStr.append(boost::lexical_cast<std::wstring>((INT64)(pQ->biStuffSerialA)));
		wItemCountStr.append(L"1");	// 1개
		wItemFlagStr.append(L"1");	// 1: 전체삭제
	}
	if (pQ->biStuffSerialB > 0){
		wItemSerialStr.append(L",");
		wItemSerialStr.append(boost::lexical_cast<std::wstring>((INT64)(pQ->biStuffSerialB)));
		wItemCountStr.append(L",1");	// 1개
		wItemFlagStr.append(L",1");		// 1: 전체삭제
	}
	if (pQ->biMixItemSerial > 0){
		wItemSerialStr.append(L",");
		wItemSerialStr.append(boost::lexical_cast<std::wstring>((INT64)(pQ->biMixItemSerial)));
		wItemCountStr.append(L",1");	// 1개
		if (pQ->wMixItemCount > 1)
			wItemFlagStr.append(L",0");	// 0: 부분삭제
		else
			wItemFlagStr.append(L",1");	// 1: 전체삭제
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_MakeItemFromItems(%I64d, N'%s',N'%s',N'%s',%I64d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,N'%s')}", 
		pQ->biCharacterDBID,
		wItemSerialStr.c_str(),
		wItemCountStr.c_str(),
		wItemFlagStr.c_str(),
		pQ->biFee,
		pQ->Code,
		pQ->RandomItem.nSerial,
		pQ->RandomItem.nItemID,
		pQ->RandomItem.wCount,
		(short)pQ->RandomItem.wDur,
		pQ->RandomItem.nRandomSeed,
		pQ->RandomItem.cLevel,
		pQ->RandomItem.cPotential,
		(pQ->RandomItem.bSoulbound?1:0),
		pQ->RandomItem.cSealCount,
		pQ->RandomItem.cOption,
		(pQ->RandomItem.bEternity?1:0),
		pQ->RandomItem.nLifespan,
		pQ->cItemLocationCode,
		pQ->nChannelID,
		pQ->nMapIndex,
		pQ->wszIP);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif


int CDNSQLWorld::QueryGetProfile(INT64 biCharacterDBID, TProfile& Profile)
{
	CQueryTimeLog QueryTimeLog( "P_GetProfile" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[P_GetProfile] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetProfile(%I64d)}", biCharacterDBID );

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Profile.cGender,		sizeof(BYTE),							  &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR,	&Profile.wszGreeting,	sizeof(WCHAR) *(PROFILEGREETINGLEN + 1), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT,		&Profile.bOpenPublic,	sizeof(bool),							  &cblen);
			CheckColumnCount(nNo, "P_GetProfile");
			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA)
				iSQLResult = ERROR_NONE;
			else
				CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QuerySetProfile(INT64 biCharacterDBID, TProfile& Profile)
{
	CQueryTimeLog QueryTimeLog( "P_UpsProfile" );

	WCHAR wszGreeting[PROFILEGREETINGLEN * 2 + 1] ={0,};
	ConvertQuery(Profile.wszGreeting, PROFILEGREETINGLEN, wszGreeting, _countof(wszGreeting));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_UpsProfile(%I64d,%d,N'%s',%d)}", biCharacterDBID, Profile.cGender, wszGreeting, Profile.bOpenPublic );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddAbuseLog(TQAddAbuseLog* pQ)
{
	CQueryTimeLog QueryTimeLog( "QueryAddAbuseLog" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddAbuseLog(%d,%I64d,N'%s')}", pQ->cAbuseCode, pQ->biCharacterDBID, pQ->wszBuf );

	return CommonReturnValueQuery(m_wszQuery);
}


int CDNSQLWorld::QueryAddAbuseMonitor( TQAddAbuseMonitor* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddAbuseMonitor" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddAbuseMonitor(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->iCount, pQ->iCount2 );
#else
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddAbuseMonitor(%I64d,%d)}", pQ->biCharacterDBID, pQ->iCount );
#endif

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelAbuseMonitor( TQDelAbuseMonitor* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelAbuseMonitor" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_ANTI_CHAT_SPAM)
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelAbuseMonitor(%I64d,%d)}", pQ->biCharacterDBID, pQ->cType );
#else
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelAbuseMonitor(%I64d)}", pQ->biCharacterDBID );
#endif

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetAbuseMonitor( TQGetAbuseMonitor* pQ, TAGetAbuseMonitor* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetAbuseMonitor" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetAbuseMonitor] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetAbuseMonitor(%I64d,?,?,?,?)}", pQ->biCharacterDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iAbuseCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iCallCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iPlayRestraintValue, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iDBResetRestraintValue, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetWholeAbuseMonitor(TQGetWholeAbuseMonitor * pQ, TAGetWholeAbuseMonitor * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListAbuseMonitorCount" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetListAbuseMonitorCount] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListAbuseMonitorCount(%d)}", pQ->nAccountDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TAbuseMonitor Abuse;
			memset(&Abuse, 0, sizeof(TAbuseMonitor));

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &Abuse.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Abuse.iAbuseCount, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Abuse.iCallCount, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Abuse.iPlayRestraintValue, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Abuse.iDBResetRestraintValue, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetListAbuseMonitorCount");
			while(1)
			{
				memset(&Abuse, 0, sizeof(Abuse));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->Abuse[pA->cCount] = Abuse;
				pA->cCount++;

				if( pA->cCount >= CHARCOUNTMAX ){
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}


// 아이템 
#if defined(_LOGINSERVER)
#include "DNAuthManager.h"
void CDNSQLWorld::MakeAddMaterializedItem(int nMapIndex, INT64 biCharacterDBID, int nSlotIndex, int nItemLocation, int nItemID, int nCount, int nRandomSeed, int nDur, OUT TQAddMaterializedItem &Item)
{
	memset(&Item, 0, sizeof(TQAddMaterializedItem));

	Item.biCharacterDBID = biCharacterDBID;
	Item.cItemLocationIndex = nSlotIndex;
	Item.cItemLocationCode = nItemLocation;
	Item.Code = DBDNWorldDef::AddMaterializedItem::CharacterCreate;
	Item.iMapIndex = nMapIndex;
	//Item.bEternityFlag = true;

	Item.AddItem.nItemID = nItemID;
	Item.AddItem.wCount = nCount;
	Item.AddItem.wDur = nDur;
	Item.AddItem.nRandomSeed = nRandomSeed;
	Item.AddItem.nSerial = MakeSerial(g_pAuthManager->GetServerID());
	Item.AddItem.bEternity = true;
}
#endif // #if defined(_LOGINSERVER)

int CDNSQLWorld::QueryAddMaterializedItem( TQAddMaterializedItem* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddMaterializedItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryAddMaterializedItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[7];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	std::wstring wIndexs, wItemIDs, wSerials;
	for(int i = 0; i < pQ->cPetCount; i++){
		if(i > 0) wIndexs.append(L",");
		wIndexs.append(boost::lexical_cast<std::wstring>(pQ->PetList[i].cSlotIndex));
		if(i > 0) wItemIDs.append(L",");
		wItemIDs.append(boost::lexical_cast<std::wstring>(pQ->PetList[i].nItemID));
		if(i > 0) wSerials.append(L",");
		wSerials.append(boost::lexical_cast<std::wstring>(pQ->PetList[i].biItemSerial));
	}

	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddMaterializedItem2(%I64d,%I64d,%d,?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d,%d,%d,?,%d,%d,?,%d,%d,N'%s',N'%s',N'%s',N'%s',?,%d,?,?,?,%d,%I64d)}",
		pQ->AddItem.nSerial, pQ->biCharacterDBID, pQ->Code,
		pQ->AddItem.nItemID, pQ->AddItem.wCount, (short)pQ->AddItem.wDur, pQ->AddItem.nRandomSeed, pQ->AddItem.cLevel, pQ->AddItem.cPotential,(pQ->AddItem.bSoulbound?1:0), pQ->AddItem.cSealCount, pQ->AddItem.cOption,
		pQ->biItemPrice,(pQ->AddItem.bEternity?1:0), pQ->AddItem.nLifespan,
		pQ->cItemLocationCode, pQ->cItemLocationIndex,
		pQ->iChannelID, pQ->iMapIndex, pQ->wszIP, wIndexs.c_str(), wItemIDs.c_str(), wSerials.c_str(), pQ->bStackableFlag, pQ->bMerge, pQ->biMergeTargetItemSerial);

	DWORD dwPetPartsColor1 = 0, dwPetPartsColor2 = 0;
	dwPetPartsColor1 = pQ->dwPetPartsColor1;
	dwPetPartsColor2 = pQ->dwPetPartsColor2;
	int nPetSkillID1 = 0, nPetSkillID2 = 0;
	nPetSkillID1 = pQ->nPetSkillID1;
	nPetSkillID2 = pQ->nPetSkillID2;

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		// @inbItemMaterializeFKey
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biFKey, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biFKey <= 0, sizeof(INT64)));
		// @inbSenderCharacterID
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biSenderCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biSenderCharacterDBID <= 0, sizeof(INT64)));
		// @inyPayMethodCode
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cPayMethodCode, sizeof(BYTE), SqlLen[sqlparam++].GetNull(pQ->cPayMethodCode <= 0, sizeof(BYTE)));
		// @intPetSkinColor
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(DWORD), 0, &dwPetPartsColor1, sizeof(DWORD), SqlLen[sqlparam++].GetNull(dwPetPartsColor1 <= 0, sizeof(DWORD)));
		// @intPetSkinColor2
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(DWORD), 0, &dwPetPartsColor2, sizeof(DWORD), SqlLen[sqlparam++].GetNull(dwPetPartsColor2 <= 0, sizeof(DWORD)));
		// @intPetSkillID1
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nPetSkillID1, sizeof(int), SqlLen[sqlparam++].GetNull(nPetSkillID1 <= 0, sizeof(int)));
		// @intPetSkillID2
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nPetSkillID2, sizeof(int), SqlLen[sqlparam++].GetNull(nPetSkillID2 <= 0, sizeof(int)));
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return nResult;
}

int CDNSQLWorld::QueryUseItem( TQUseItem* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_UseItem" );
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryUseItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int iSQLResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	CDNSqlLen SqlLen[2];
	swprintf( m_wszQuery, L"{?=CALL dbo.P_UseItem2(%I64d,%d,%d,%d,%d,N'%s',%d,?,?)}", pQ->biItemSerial, pQ->Code, pQ->nUsedItemCount, pQ->iChannelID, pQ->iMapID, pQ->wszIP, pQ->bAllFlag );

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int),	&cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iTargetLogItemID, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->iTargetLogItemID <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cTargetLogItemLevel, sizeof(BYTE), SqlLen[sqlparam++].GetNull(pQ->cTargetLogItemLevel <= 0, sizeof(BYTE)));
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}
	return iSQLResult;
}

int CDNSQLWorld::QueryResellItem( TQResellItem* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ResellItem" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

#ifdef PRE_MOD_SELL_SEALEDITEM
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ResellItem2(%I64d,%I64d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d)}"
		, pQ->biCharacterDBID, pQ->biItemSerial, pQ->nItemCount, pQ->biResellPrice, pQ->iNpcID, pQ->iChannelID, pQ->iMapID, pQ->wszIP, pQ->bAllFlag, pQ->bRebuyFlag, pQ->bBelongFlag );
#else		//#ifdef PRE_MOD_SELL_SEALEDITEM
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ResellItem2(%I64d,%I64d,%d,%I64d,%d,%d,%d,N'%s',%d,%d)}"
		, pQ->biCharacterDBID, pQ->biItemSerial, pQ->nItemCount, pQ->biResellPrice, pQ->iNpcID, pQ->iChannelID, pQ->iMapID, pQ->wszIP, pQ->bAllFlag, pQ->bRebuyFlag );
#endif		//#ifdef PRE_MOD_SELL_SEALEDITEM

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryEnchantItem( TQEnchantItem* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_EnchantItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryEnchantItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wstrItemSerial, wstrItemCount;
	for( UINT i=0 ; i<pQ->cMaterialCount ; ++i )
	{
		if( i > 0 )
		{
			wstrItemSerial.append(L",");
			wstrItemCount.append(L",");
		}
		
		wstrItemSerial.append( boost::lexical_cast<std::wstring>(pQ->MaterialArray[i].biSerial));
		wstrItemCount.append( boost::lexical_cast<std::wstring>(pQ->MaterialArray[i].wCount));
	}
	
	int iSQLResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	CDNSqlLen SqlLen[14];
#if defined(PRE_ADD_EXCHANGE_ENCHANT)
	swprintf( m_wszQuery, L"{?=CALL dbo.P_EnchantItem(%I64d,N'%s',N'%s',%I64d,%d,%d,%d,%d,%d,%d,%d,?,?,?,?,?,?,?,?,?,?,?,?,?,?,%I64d)}"
		, pQ->biItemSerial, wstrItemSerial.c_str(), wstrItemCount.c_str(), pQ->biCoin, pQ->bEnchantSuccessFlag?1:0, pQ->bItemDestroyFlag?1:0
		, pQ->cCurrentItemLevel, pQ->cCurrentItemPotential, pQ->iChannelID, pQ->iMapID, pQ->iNpcID, pQ->biExtractItemSerial );
#else
	swprintf( m_wszQuery, L"{?=CALL dbo.P_EnchantItem(%I64d,N'%s',N'%s',%I64d,%d,%d,%d,%d,%d,%d,%d,?,?,?,?,?,?,?,?,?,?,?,?,?,?)}"
		, pQ->biItemSerial, wstrItemSerial.c_str(), wstrItemCount.c_str(), pQ->biCoin, pQ->bEnchantSuccessFlag?1:0, pQ->bItemDestroyFlag?1:0
		, pQ->cCurrentItemLevel, pQ->cCurrentItemPotential, pQ->iChannelID, pQ->iMapID, pQ->iNpcID );
#endif

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->NewItem.nSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->NewItem.nSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->NewItem.nItemID, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->NewItem.nItemID <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &pQ->NewItem.wCount, sizeof(short), SqlLen[sqlparam++].GetNull(pQ->NewItem.wCount <= 0, sizeof(short)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(USHORT), 0, &pQ->NewItem.wDur, sizeof(USHORT), SqlLen[sqlparam++].GetNull(pQ->NewItem.wDur <= 0, sizeof(USHORT)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->NewItem.nRandomSeed, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->NewItem.nRandomSeed <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(char), 0, &pQ->NewItem.cLevel, sizeof(char), SqlLen[sqlparam++].GetNull(pQ->NewItem.cLevel < 0, sizeof(char)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pQ->NewItem.cPotential, sizeof(char), SqlLen[sqlparam++].GetNull(pQ->NewItem.cPotential <= 0, sizeof(char)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &pQ->NewItem.bSoulbound, sizeof(bool), SqlLen[sqlparam++].GetNull(pQ->NewItem.bSoulbound == false, sizeof(bool)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->NewItem.cSealCount, sizeof(BYTE), SqlLen[sqlparam++].GetNull(pQ->NewItem.cSealCount < 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->NewItem.cOption, sizeof(BYTE), SqlLen[sqlparam++].GetNull(pQ->NewItem.cOption < 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &pQ->NewItem.bEternity, sizeof(bool), SqlLen[sqlparam++].GetNull(pQ->NewItem.bEternity == false, sizeof(bool)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->NewItem.nLifespan, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->NewItem.nLifespan <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biCharacterDBID, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biCharacterDBID <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->nSupportItemID, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->nSupportItemID <= 0, sizeof(int)));
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryChangeItemLocation( TQChangeItemLocation* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ChangeItemLocation" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryChangeItemLocation] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	bool bVehicleParts = false;
	bVehicleParts = pQ->bVehicleParts;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ChangeItemLocation2(%I64d,%I64d,?,%d,%d,%d,%d,%d,%d,%d,?,?,%d,%I64d,%d,%d)}",
		pQ->biCharacterDBID, pQ->biItemSerial, pQ->iItemID, pQ->Code, pQ->cItemLocationIndex, pQ->nItemCount, pQ->iChannelID, pQ->iMapID, bVehicleParts, pQ->bMerge, pQ->biMergeTargetItemSerial, pQ->bMergeAll, pQ->nGoldFee);
#else
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ChangeItemLocation2(%I64d,%I64d,?,%d,%d,%d,%d,%d,%d,%d,?,?,%d,%I64d,%d)}",
		pQ->biCharacterDBID, pQ->biItemSerial, pQ->iItemID, pQ->Code, pQ->cItemLocationIndex, pQ->nItemCount, pQ->iChannelID, pQ->iMapID, bVehicleParts, pQ->bMerge, pQ->biMergeTargetItemSerial, pQ->bMergeAll);
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

	int iSQLResult = ERROR_DB, nParam = 0;
	CDNSqlLen SqlLen[4];

	INT64 biVehicleSerial = 0;
	char cVehicleSlotIndex = 0;
	biVehicleSerial = pQ->biVehicleSerial;
	cVehicleSlotIndex = pQ->cVehicleSlotIndex;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biNewItemSerial, sizeof(INT64), SqlLen[nParam++].GetNull(pQ->biNewItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biVehicleSerial, sizeof(INT64), SqlLen[nParam++].GetNull(biVehicleSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pQ->cVehicleSlotIndex, sizeof(char), SqlLen[nParam++].GetNull(cVehicleSlotIndex <= 0, sizeof(char)));

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QuerySwitchItemLocation(TQSwitchItemLocation *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_SwitchItemLocation" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SwitchItemLocation2(%I64d,%d,%d,%I64d,%d,%d,%I64d)}", 
		pQ->biCharacterDBID, pQ->SrcCode, pQ->cSrcSlotIndex, pQ->biSrcItemSerial, pQ->DestCode, pQ->cDestSlotIndex, pQ->biDestItemSerial);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModItemDurability( TQModItemDurability* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModItemDurability" );

	std::wstring wstrItemSerial,wstrDurability;
	for( int i=0 ; i<pQ->nCount ; ++i )
	{
		if( i > 0 )
		{
			wstrItemSerial.append(L",");
			wstrDurability.append(L",");
		}

		wstrItemSerial.append( boost::lexical_cast<std::wstring>(pQ->ItemDurArr[i].biSerial));
		wstrDurability.append( boost::lexical_cast<std::wstring>((short)(pQ->ItemDurArr[i].wDur)));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModItemDurability(%I64d,N'%s',N'%s',%I64d,%d,%d)}", 
		pQ->biCharacterDBID, wstrItemSerial.c_str(), wstrDurability.c_str(), pQ->biTax, pQ->iChannelID, pQ->iMapID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModMaterializedItem( TQModMaterializedItem* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModMaterializedItem" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModMaterializedItem(%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,0,0,%d,%d)}"
						, pQ->biItemSerial, pQ->iChannelID, pQ->iMapID, pQ->iModFieldBitmap, (short)pQ->nItemDurability, pQ->cItemLevel
						, pQ->cItemPotential, pQ->bSoudBoundFlag?1:0, pQ->cSealCount, pQ->cItemOption, pQ->nEnchantJewelID, pQ->cItemPotentialMoveCount);
#else	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModMaterializedItem(%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,0,0,%d)}"
						, pQ->biItemSerial, pQ->iChannelID, pQ->iMapID, pQ->iModFieldBitmap, (short)pQ->nItemDurability, pQ->cItemLevel
						, pQ->cItemPotential, pQ->bSoudBoundFlag?1:0, pQ->cSealCount, pQ->cItemOption, pQ->nEnchantJewelID );
#endif	// #if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListMaterializedItem( INT64 biCharacterDBID, std::wstring wstrItemLocationCode, int nMaxCount, int &nCount, DBPacket::TMaterialItemInfo *ItemList, UINT nAccountDBID )
{
	CQueryTimeLog QueryTimeLog( "P_GetListMaterializedItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListMaterializedItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 4;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMaterializedItem2(%I64d,N'%s',%d,%d)}", biCharacterDBID, wstrItemLocationCode.c_str(), nVersion, nAccountDBID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMaterializedItem2(%I64d,N'%s',%d)}", biCharacterDBID, wstrItemLocationCode.c_str(), nVersion);
#endif

	nCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TItemInfo ItemInfo = { 0, };
			BYTE cItemLocationCode = 0;
			TIMESTAMP_STRUCT MakeDate = { 0, }, ExpireDate = { 0, };

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&cItemLocationCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.cSlotIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &ItemInfo.Item.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &ItemInfo.Item.wCount,	sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &ItemInfo.Item.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.Item.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &ItemInfo.Item.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &ItemInfo.Item.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nLookItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nLifespan, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nNo, "P_GetListMaterializedItem");
			while(1)
			{
				memset(&ItemInfo, 0, sizeof(ItemInfo));
				memset(&MakeDate, 0, sizeof(MakeDate));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				cItemLocationCode = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				ItemList[nCount].Code = static_cast<DBDNWorldDef::ItemLocation::eCode>(cItemLocationCode);
				ItemList[nCount].ItemInfo = ItemInfo;

				if(!ItemInfo.Item.bEternity) 
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, ItemList[nCount].ItemInfo.Item.tExpireDate, ExpireDate, 0, 0, biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						iSQLResult = ERROR_DB;
						break;									
					}
				}
				else ItemList[nCount].ItemInfo.Item.tExpireDate = 0;

				nCount++;

				if( nCount >= nMaxCount ){
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryExchangeProperty(TQExchangeProperty *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ExchangeProperty" );

	std::wstring wSerialStr, wIDStr, wNewSerialStr, wCountStr;
	std::wstring wTargetSerialStr, wTargetIDStr, wTargetNewSerialStr, wTargetCountStr;

	for(int i = 0; i < EXCHANGEMAX; i++){
		if(pQ->ExchangeItem[i].biSerial > 0){
			if(!wSerialStr.empty()){
				wSerialStr.append(L",");
				wIDStr.append(L",");
				wNewSerialStr.append(L",");
				wCountStr.append(L",");
			}
			wSerialStr.append(boost::lexical_cast<std::wstring>(pQ->ExchangeItem[i].biSerial));
			wIDStr.append(boost::lexical_cast<std::wstring>(pQ->ExchangeItem[i].nItemID));
			wNewSerialStr.append(boost::lexical_cast<std::wstring>(pQ->ExchangeItem[i].biNewSerial));
			wCountStr.append(boost::lexical_cast<std::wstring>(pQ->ExchangeItem[i].wCount));
		}
		if(pQ->TargetExchangeItem[i].biSerial > 0){
			if(!wTargetSerialStr.empty()){
				wTargetSerialStr.append(L",");
				wTargetIDStr.append(L",");
				wTargetNewSerialStr.append(L",");
				wTargetCountStr.append(L",");
			}
			wTargetSerialStr.append(boost::lexical_cast<std::wstring>(pQ->TargetExchangeItem[i].biSerial));
			wTargetIDStr.append(boost::lexical_cast<std::wstring>(pQ->TargetExchangeItem[i].nItemID));
			wTargetNewSerialStr.append(boost::lexical_cast<std::wstring>(pQ->TargetExchangeItem[i].biNewSerial));
			wTargetCountStr.append(boost::lexical_cast<std::wstring>(pQ->TargetExchangeItem[i].wCount));
		}
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ExchangeProperty2(%I64d,N'%s',N'%s',N'%s',N'%s',%I64d,%d, %I64d,N'%s',N'%s',N'%s',N'%s',%I64d,%d, %d,%d)}", 
		pQ->biCharacterDBID, wSerialStr.c_str(), wIDStr.c_str(), wNewSerialStr.c_str(), wCountStr.c_str(), pQ->biExchangeCoin, pQ->nExchangeTax,
		pQ->biTargetCharacterDBID, wTargetSerialStr.c_str(), wTargetIDStr.c_str(), wTargetNewSerialStr.c_str(), wTargetCountStr.c_str(), 
		pQ->biTargetExchangeCoin, pQ->nTargetExchangeTax, pQ->nChannelID, pQ->nMapID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryTakeCashEquipmentOff(TQTakeCashEquipmentOff *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_TakeCashEquipmentOff" );

	std::wstring wstrIndex;
	for(int i = 0; i < pQ->cCashEquipTotalCount; i++){
		if(!wstrIndex.empty()){
			wstrIndex.append(L",");
		}
		wstrIndex.append(boost::lexical_cast<std::wstring>(pQ->cCashEquipIndexArray[i]));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_TakeCashEquipmentOff2(%I64d,N'%s')}", pQ->biCharacterDBID, wstrIndex.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryPutOnepieceOn(TQPutOnepieceOn *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_PutOnepieceOn" );

	std::wstring wstrIndex;
	for(int i = 0; i < pQ->cCashEquipTotalCount; i++){
		if(pQ->cSubCashEquipIndexArray[i] > 0){
			if(!wstrIndex.empty()){
				wstrIndex.append(L",");
			}
			wstrIndex.append(boost::lexical_cast<std::wstring>(pQ->cSubCashEquipIndexArray[i]));
		}		
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_PutOnepieceOn2(%I64d,%I64d,%d,N'%s')}", pQ->biCharacterDBID, pQ->biOnepieceItemSerial, pQ->cOnepieceCashEquipIndex, wstrIndex.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetPageMaterializedItem(TQGetPageMaterializedItem *pQ, TAGetPageMaterializedItem *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPageMaterializedItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetPageMaterializedItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 3;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageMaterializedItem2(%d,%d,%I64d,%d,%d)}", pQ->nPageNumber, pQ->nPageSize, pQ->biCharacterDBID, DBDNWorldDef::ItemLocation::CashInventory, nVersion);

	pA->cCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TItem CashItem = { 0, };
			int nSeq = 0;
			char cItemLocationCode = 0;
			TIMESTAMP_STRUCT MakeDate = { 0, }, ExpireDate = { 0, };

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nSeq, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&cItemLocationCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &CashItem.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CashItem.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &CashItem.wCount,	sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &CashItem.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CashItem.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,	&CashItem.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&CashItem.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&CashItem.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &CashItem.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&CashItem.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&CashItem.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &CashItem.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CashItem.nLookItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &CashItem.nLifespan, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &CashItem.bExpireComplete, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&CashItem.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nNo, "P_GetPageMaterializedItem");
			while(1)
			{
				memset(&CashItem, 0, sizeof(TItem));
				memset(&MakeDate, 0, sizeof(MakeDate));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				nSeq = 0;
				cItemLocationCode = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->CashItem[pA->cCount] = CashItem;
				if(!CashItem.bEternity)
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, pA->CashItem[pA->cCount].tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						iSQLResult = ERROR_DB;
						break;									
					}
				}
				else 
					pA->CashItem[pA->cCount].tExpireDate = 0;

				pA->cCount++;

				memset(&CashItem, 0x00, sizeof(CashItem));

				if(pA->cCount >= pQ->nPageSize){
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryGetCountMaterializedItem(INT64 biCharacterDBID, int nItemLocationCode, OUT int &nTotalCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountMaterializedItem" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountMaterializeditem] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountMaterializedItem2(%I64d,%d)}", biCharacterDBID, nItemLocationCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nTotalCount, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetCountMaterializedItem");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetPagePet(INT64 biCharacterDBID, int nPageNumber, int nPageSize, int nPetType, bool bEquipFlag, std::vector<DBPacket::TVehicleInfo> &VecItemList)
{
	CQueryTimeLog QueryTimeLog( "P_GetPagePet" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetPagePet] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPagePet2(%d,%d,%I64d,%d,%d,7)}", nPageNumber, nPageSize, biCharacterDBID, nPetType, bEquipFlag);

	int nCount = 0;
	int nSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			DBPacket::TVehicleInfo Vehicle;
			memset(&Vehicle, 0, sizeof(Vehicle));
			TItem TempItem = { 0, };
			TIMESTAMP_STRUCT MakeDate = { 0, }, ExpireDate = { 0, };
			TIMESTAMP_STRUCT LastHungerDate = { 0, };
			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Vehicle.nPetIndex, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &Vehicle.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Vehicle.wCount,	sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &TempItem.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &TempItem.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,	&TempItem.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&TempItem.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&TempItem.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &TempItem.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&TempItem.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&TempItem.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &Vehicle.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.dwPartsColor1, sizeof(DWORD), &cblen);
			BYTE cPetSkillSlotNum = 0;
			if( nPetType > 1 ) // 펫
			{			
				SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.dwPartsColor2, sizeof(DWORD), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.nSkillID1, sizeof(int), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.nSkillID2, sizeof(int), &cblen);			
				SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, Vehicle.wszNickName, sizeof(Vehicle.wszNickName), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.nExp, sizeof(DWORD), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&cPetSkillSlotNum, sizeof(BYTE), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Vehicle.nCurrentSatiety, sizeof(int), &cblen);
				SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastHungerDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			}
			CheckColumnCount(nNo, "P_GetPagePet");
			while(1)
			{
				memset(&Vehicle, 0, sizeof(Vehicle));
				memset(&TempItem, 0, sizeof(TempItem));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				memset(&MakeDate, 0, sizeof(MakeDate));
				cPetSkillSlotNum = 0;
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				if(!Vehicle.bEternity)
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, Vehicle.tExpireDate, ExpireDate, 0, 0, biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						nSQLResult = ERROR_DB;
						break;									
					}
				}
				else 
					Vehicle.tExpireDate = 0;

				if( nPetType > 1 ) // 펫
				{				
					CTimeParamSet LastHungerTime(&QueryTimeLog, Vehicle.tLastHungerModifyDate, LastHungerDate, 0, 0, biCharacterDBID);
					if (!LastHungerTime.IsValid())
					{
						nSQLResult = ERROR_DB;
						break;									
					}
				}
				
				if( cPetSkillSlotNum > 0) // 0이면 스킬 1개, 1이면 확장된거.
					Vehicle.bSkillSlot = true;

				VecItemList.push_back(Vehicle);
				nCount++;

				if(nCount >= nPageSize){
					nSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nSQLResult;
}

int CDNSQLWorld::QueryGetCountPet(INT64 biCharacterDBID, int nPetType, OUT int &nTotalCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountPet" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountPet] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountPet2(%I64d,%d,%d)}", biCharacterDBID, nPetType, false);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nTotalCount, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetCountPet");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryModPetSkinColor(INT64 biBodySerial, DWORD dwSkinColor, DWORD dwSkinColor2)
{
	CQueryTimeLog QueryTimeLog( "P_ModPetSkinColor" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPetSkinColor(%I64d,%d,%d)}", biBodySerial, dwSkinColor, dwSkinColor2);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelPetEquipment(INT64 biBodySerial, INT64 biPartsSerial, WCHAR *pIp)
{
	CQueryTimeLog QueryTimeLog( "P_DelPetEquipment" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelPetEquipment(%I64d,%I64d,N'%s')}", biBodySerial, biPartsSerial, pIp);

	return CommonReturnValueQuery(m_wszQuery);
}

// 펫 이름 변경.
int CDNSQLWorld::QueryChangePetName(INT64 petSerial, const wchar_t* pName)
{
	CQueryTimeLog QueryTimeLog("P_ModPetName");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPetName(%I64d,N'%s')}", petSerial, pName);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryItemExpireByCheat(TQItemExpire* pQ)
{
	CQueryTimeLog QueryTimeLog("P_ExpireItemForCheat");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ExpireItemForCheat(%I64d, %I64d)}", pQ->biCharacterDBID, pQ->biSerial);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModItemExpireDate(TQModItemExpireDate* pQ)
{
	CQueryTimeLog QueryTimeLog("P_ModItemExpireDate");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModItemExpireDate(%I64d,%d,%I64d )}", pQ->biItemSerial , pQ->nMin, pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModPetExp(TQModPetExp* pQ)
{
	CQueryTimeLog QueryTiemLog("P_ModPetExp");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPetExp(%I64d,%d)}", pQ->biPetSerial, pQ->nExp);
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModPetSkill(TQModPetSkill *pQ)
{
	CQueryTimeLog QueryTiemLog("P_ModPetSkill");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPetSkill(%I64d,%d,%d)}", pQ->biItemSerial, pQ->cSkillSlot, pQ->nSkillID);
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModPetSkillExpand(INT64 biItemSerial)
{
	CQueryTimeLog QueryTiemLog("P_ModPetSkillExpand");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPetSkillExpand(%I64d)}", biItemSerial);
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModDegreeOfHunger(TQModDegreeOfHunger* pQ)
{
	CQueryTimeLog QueryTiemLog("P_ModDegreeOfHunger");

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModDegreeOfHunger(%I64d,%d)}", pQ->biBodySerial, pQ->nDegreeOfHunger);
	return CommonReturnValueQuery(m_wszQuery);
}

// 우편
int CDNSQLWorld::QueryGetCountSendMail(INT64 biCharacterDBID, OUT int &nPremiumMailCount, OUT int &nBasicMailCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountSendMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountSendMain] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountSendMail(?,?,?)}");

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nPremiumMailCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nBasicMailCount, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QuerySendMail( TQSendMail* pQ, TASendMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_SendMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biSenderCharacterDBID, 0, L"[QuerySendMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wstrItemSerial, wstrNewItemSerial, wstrItemCount;
	for( UINT i=0 ; i<pQ->cAttachItemCount ; ++i )
	{
		if( i > 0 )
		{
			wstrItemSerial.append(L",");
			wstrNewItemSerial.append(L",");
			wstrItemCount.append(L",");
		}

		wstrItemSerial.append( boost::lexical_cast<std::wstring>(pQ->AttachItemArr[i].Item.nSerial));
		wstrNewItemSerial.append( boost::lexical_cast<std::wstring>(pQ->AttachItemArr[i].biNewSerial));
		wstrItemCount.append( boost::lexical_cast<std::wstring>(pQ->AttachItemArr[i].Item.wCount));
	}

	pA->nAccountDBID = pQ->nAccountDBID;

	WCHAR wszReceiver[MAILNAMELENMAX * 2 + 1] = {0,}, wszTitle[MAILTITLELENMAX * 2 + 1] = {0,}, wszText[MAILTEXTLENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszReceiverCharacterName, MAILNAMELENMAX, wszReceiver, _countof(wszReceiver));
	ConvertQuery(pQ->wszSubject, MAILTITLELENMAX, wszTitle, _countof(wszTitle));
	ConvertQuery(pQ->wszContent, MAILTEXTLENMAX, wszText, _countof(wszText));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_SendMail2(?,N'%s',?,N'%s',N'%s',?,N'%s',N'%s',N'%s',?,?,?, ?,?,?,?,?,?)}",
		wszReceiver, wszTitle, wszText, wstrItemSerial.c_str(), wstrNewItemSerial.c_str(), wstrItemCount.c_str());

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;

		INT64 biTax = pQ->nTax;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biSenderCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pQ->Code, sizeof(char), SqlLen2.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biAttachCoin, sizeof(INT64), SqlLen3.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biTax, sizeof(INT64), SqlLen4.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iChannelID, sizeof(int), SqlLen5.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iMapID, sizeof(int), SqlLen6.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biReceiverCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverAccountDBID, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverTotalMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverNotReadMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiver7DaysLeftMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nBasicMailCount, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QuerySendSystemMail( TQSendSystemMail* pQ, TASendSystemMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_SendSystemMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QuerySendSystemMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->nAccountDBID = pQ->nAccountDBID;

	WCHAR wszTitle[MAILTITLELENMAX * 2 + 1] = {0,}, wszText[MAILTEXTLENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszSubject, MAILTITLELENMAX, wszTitle, _countof(wszTitle));
	ConvertQuery(pQ->wszContent, MAILTEXTLENMAX, wszText, _countof(wszText));

	WCHAR wszSenderName[MAILNAMELENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszSenderName, MAILNAMELENMAX, wszSenderName, _countof(wszSenderName));

	if(pQ->cAttachItemCount > MAILATTACHITEMMAX) pQ->cAttachItemCount = MAILATTACHITEMMAX;
	TItem sAttachItem[MAILATTACHITEMMAX] = {0,};
	memcpy_s( sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount), pQ->sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount) );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_SendSystemMail(N'%s',?,?,?,N'%s',N'%s',?,\
							%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
						    %I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
						    %I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
						    %I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
						    %I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d, \
						    %d,%d,?,?,?,?,%d)}",
							wszSenderName,
							wszTitle, wszText,
						    sAttachItem[0].nSerial, sAttachItem[0].nItemID, sAttachItem[0].wCount, (short)sAttachItem[0].wDur, sAttachItem[0].nRandomSeed, sAttachItem[0].cLevel, sAttachItem[0].cPotential, 
							sAttachItem[0].bSoulbound?1:0, sAttachItem[0].cSealCount, sAttachItem[0].cOption, sAttachItem[0].bEternity?1:0, sAttachItem[0].nLifespan,
						    sAttachItem[1].nSerial, sAttachItem[1].nItemID, sAttachItem[1].wCount, (short)sAttachItem[1].wDur, sAttachItem[1].nRandomSeed, sAttachItem[1].cLevel, sAttachItem[1].cPotential,
							sAttachItem[1].bSoulbound?1:0, sAttachItem[1].cSealCount, sAttachItem[1].cOption, sAttachItem[1].bEternity?1:0, sAttachItem[1].nLifespan,
						    sAttachItem[2].nSerial, sAttachItem[2].nItemID, sAttachItem[2].wCount, (short)sAttachItem[2].wDur, sAttachItem[2].nRandomSeed, sAttachItem[2].cLevel, sAttachItem[2].cPotential,
							sAttachItem[2].bSoulbound?1:0, sAttachItem[2].cSealCount, sAttachItem[2].cOption, sAttachItem[2].bEternity?1:0, sAttachItem[2].nLifespan,
						    sAttachItem[3].nSerial, sAttachItem[3].nItemID, sAttachItem[3].wCount, (short)sAttachItem[3].wDur, sAttachItem[3].nRandomSeed, sAttachItem[3].cLevel, sAttachItem[3].cPotential,
							sAttachItem[3].bSoulbound?1:0, sAttachItem[3].cSealCount, sAttachItem[3].cOption, sAttachItem[3].bEternity?1:0, sAttachItem[3].nLifespan,
						    sAttachItem[4].nSerial, sAttachItem[4].nItemID, sAttachItem[4].wCount, (short)sAttachItem[4].wDur, sAttachItem[4].nRandomSeed, sAttachItem[4].cLevel, sAttachItem[4].cPotential,
							sAttachItem[4].bSoulbound?1:0, sAttachItem[4].cSealCount, sAttachItem[4].cOption, sAttachItem[4].bEternity?1:0, sAttachItem[4].nLifespan,
						    pQ->iChannelID, pQ->iMapID, pQ->bNewFlag);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biReceiverCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pQ->Code, sizeof(char), SqlLen2.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biFKey, sizeof(INT64), SqlLen3.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biAttachCoin, sizeof(INT64), SqlLen4.Get(sizeof(INT64)));

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iTotalMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iNotReadMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->i7DaysLeftMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nMailDBID, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

#ifdef PRE_ADD_JOINGUILD_SUPPORT
int CDNSQLWorld::QueryGetGuildSupportRewardInfo(TQGuildSupportRewardInfo * pQ, TAGuildSupportRewardInfo * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildMaxLevelRewardInfo" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, pQ->biCharacterDBID, 0, L"[QueryGetGuildSupportRewardInfo] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildMaxLevelRewardInfo(%I64d)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &pA->bWasGuildSupportRewardFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->cJoinGuildLevel, sizeof(BYTE), &cblen);

			CheckColumnCount(nNo, "P_GetCountPet");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QuerySendGuildMail( TQSendGuildMail* pQ, TASendGuildMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_SendGuildItemMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QuerySendSystemMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->nAccountDBID = pQ->nAccountDBID;

	WCHAR wszTitle[MAILTITLELENMAX * 2 + 1] = {0,}, wszText[MAILTEXTLENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszSubject, MAILTITLELENMAX, wszTitle, _countof(wszTitle));
	ConvertQuery(pQ->wszContent, MAILTEXTLENMAX, wszText, _countof(wszText));

	WCHAR wszSenderName[MAILNAMELENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszSenderName, MAILNAMELENMAX, wszSenderName, _countof(wszSenderName));

	if(pQ->cAttachItemCount > MAILATTACHITEMMAX) pQ->cAttachItemCount = MAILATTACHITEMMAX;
	TItem sAttachItem[MAILATTACHITEMMAX] = {0,};
	memcpy_s( sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount), pQ->sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount) );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_SendGuildItemMail(N'%s',?,?,?,N'%s',N'%s',?,\
						   ?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?, \
						   ?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?, \
						   ?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?, \
						   ?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?, \
						   ?,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?, \
						   ?,?,?,?,?,?,%d)}",
						   wszSenderName,
						   wszTitle, wszText,
						   sAttachItem[0].nItemID, sAttachItem[0].wCount, (short)sAttachItem[0].wDur, sAttachItem[0].nRandomSeed, sAttachItem[0].cLevel, sAttachItem[0].cPotential, sAttachItem[0].bSoulbound?1:0, sAttachItem[0].cSealCount, sAttachItem[0].cOption, sAttachItem[0].bEternity?1:0,
						   sAttachItem[1].nItemID, sAttachItem[1].wCount, (short)sAttachItem[1].wDur, sAttachItem[1].nRandomSeed, sAttachItem[1].cLevel, sAttachItem[1].cPotential, sAttachItem[1].bSoulbound?1:0, sAttachItem[1].cSealCount, sAttachItem[1].cOption, sAttachItem[1].bEternity?1:0,
						   sAttachItem[2].nItemID, sAttachItem[2].wCount, (short)sAttachItem[2].wDur, sAttachItem[2].nRandomSeed, sAttachItem[2].cLevel, sAttachItem[2].cPotential, sAttachItem[2].bSoulbound?1:0, sAttachItem[2].cSealCount, sAttachItem[2].cOption, sAttachItem[2].bEternity?1:0,
						   sAttachItem[3].nItemID, sAttachItem[3].wCount, (short)sAttachItem[3].wDur, sAttachItem[3].nRandomSeed, sAttachItem[3].cLevel, sAttachItem[3].cPotential, sAttachItem[3].bSoulbound?1:0, sAttachItem[3].cSealCount, sAttachItem[3].cOption, sAttachItem[3].bEternity?1:0,
						   sAttachItem[4].nItemID, sAttachItem[4].wCount, (short)sAttachItem[4].wDur, sAttachItem[4].nRandomSeed, sAttachItem[4].cLevel, sAttachItem[4].cPotential, sAttachItem[4].bSoulbound?1:0, sAttachItem[4].cSealCount, sAttachItem[4].cOption, sAttachItem[4].bEternity?1:0,
						   pQ->bNewFlag);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen501[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen502[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen503[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen504[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen505[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen506[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen507[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen508[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen509[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen510[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen511[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen512[MAILATTACHITEMMAX];
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biReceiverCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biAchivementCharacterDBID, sizeof(INT64), SqlLen2.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biFKey, sizeof(INT64), SqlLen3.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biAttachCoin, sizeof(INT64), SqlLen4.Get(sizeof(INT64)));
		for(int iIndex = 0 ; MAILATTACHITEMMAX > iIndex ; ++iIndex) {
			SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &sAttachItem[iIndex].nSerial, sizeof(INT64), SqlLen501[iIndex].GetNull(0 == sAttachItem[iIndex].nSerial, sizeof(INT64)));
			SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &sAttachItem[iIndex].nLifespan, sizeof(int), SqlLen512[iIndex].GetNull(0 == sAttachItem[iIndex].nLifespan, sizeof(int)));
		}
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iChannelID, sizeof(int), SqlLen6.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iMapID, sizeof(int), SqlLen7.Get(sizeof(int)));

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iTotalMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iNotReadMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->i7DaysLeftMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nMailDBID, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

int CDNSQLWorld::QueryGetCountReceiveMail( INT64 biCharacterDBID, OUT int &nTotalCount, OUT int &nNotReadCount, OUT int &n7DaysLeftCount )
{
	CQueryTimeLog QueryTimeLog( "P_GetCountReceiveMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountReceiveMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetCountReceiveMail(?,?,?,?)}");

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int),	0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int),	0, &nTotalCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int),	0, &nNotReadCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int),	0, &n7DaysLeftCount, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetPageReceiveMail( TQGetPageReceiveMail* pQ, TAGetPageReceiveMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPageReceiveMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biReceiverCharacterDBID, 0, L"[QueryGetPageReceiveMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->nAccountDBID		= pQ->nAccountDBID;
	pA->cMailCount			= 0;

	int aMailPageSize		= MAILPAGEMAX;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageReceiveMail(?,?,?,?,?,?,?,%d)}",15);

	int nCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iPageNumber, sizeof(int), SqlLen1.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &aMailPageSize, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biReceiverCharacterDBID, sizeof(INT64), SqlLen3.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iTotalMailCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iNotReadMailCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->i7DaysLeftMailCount, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iBasicMailCount, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMailBoxInfo Mail;
			memset(&Mail, 0, sizeof(TMailBoxInfo));

			int	iRemainDay = 0, iIndex = 0;
			bool bAttachFlag = false;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&iIndex,					sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&Mail.nMailDBID,			sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR,		Mail.wszFromCharacterName,	sizeof(Mail.wszFromCharacterName),	&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR,		Mail.wszTitle,				sizeof(Mail.wszTitle),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&iRemainDay,				sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,			&Mail.cReadType,			sizeof(char),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,		&Mail.cDeliveryType,		sizeof(char),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,		&Mail.cMailType,			sizeof(char),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,			&bAttachFlag,				sizeof(bool),						&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,			&Mail.bNewFlag,				sizeof(bool),						&cblen );
			CheckColumnCount(nColNo, "P_GetPageReceiveMail");
			while(1)
			{
				memset(&Mail, 0, sizeof(Mail));
				iRemainDay = 0;
				iIndex = 0;
				bAttachFlag = false;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->sMail[pA->cMailCount] = Mail;
				if(bAttachFlag) pA->sMail[pA->cMailCount].cAttachType = MailType::AttachItem;
				pA->sMail[pA->cMailCount].cRemainDay = iRemainDay;
				pA->sMail[pA->cMailCount].wIndex = iIndex;

				pA->cMailCount++;

				if( pA->cMailCount >= _countof(pA->sMail) ){ 
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryReadMail( TQReadMail* pQ, TAReadMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ReadMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryReadMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->nAccountDBID = pQ->nAccountDBID;
	pA->cAttachItemCount = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 16;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ReadMail(?,?,?,?,?,?,?,?,%d,?)}", nVersion);

	int nCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		TIMESTAMP_STRUCT SendDate = {0,};
		bool bCoinReceiveFlag = false;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->iMailID, sizeof(int), SqlLen1.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pA->wszContent), 0, pA->wszContent, sizeof(pA->wszContent), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &SendDate, sizeof(TIMESTAMP_STRUCT), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biAttachCoin, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bCoinReceiveFlag, sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iNotReadMailCount, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cDeliveryType, sizeof(char), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bNewFlag, sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->cMailType, sizeof(BYTE), &cblen);

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TItem Item = { 0, };
			TIMESTAMP_STRUCT	ExpireDate = {0, };
			bool bReceiveFlag = false;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,	&bReceiveFlag, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &Item.nSerial, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &Item.nItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Item.wCount, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Item.wDur, sizeof(USHORT), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Item.nRandomSeed, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Item.nCoolTime, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&Item.cLevel, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&Item.cPotential, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,	&Item.bSoulbound, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&Item.cSealCount, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&Item.cOption, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,	&Item.bEternity, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&Item.cPotentialMoveCount, sizeof(char), &cblen );

			CheckColumnCount(nColNo, "P_ReadMail");
			while(1)
			{
				memset(&Item, 0, sizeof(TItem));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				bReceiveFlag = false;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					CTimeParamSet SendTime(&QueryTimeLog, pA->SendDate, SendDate, pQ->cWorldSetID, pQ->nAccountDBID);
					if (!SendTime.IsValid())
					{
						iSQLResult = ERROR_DB;
						break;									
					}
					if(bCoinReceiveFlag) pA->biAttachCoin = 0;

					if(iSQLResult == ERROR_DB)
						iSQLResult = ERROR_NONE;
						
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				if(!bReceiveFlag){
					pA->sAttachItem[pA->cAttachItemCount] = Item;
					if(!Item.bEternity)
					{
						CTimeParamSet ExpireTime(&QueryTimeLog, pA->sAttachItem[pA->cAttachItemCount].tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID);
						if (!ExpireTime.IsValid())
						{
							iSQLResult = ERROR_DB;
							break;									
						}
					}
					else
						pA->sAttachItem[pA->cAttachItemCount].tExpireDate = 0;
				}

				pA->cAttachItemCount++;

				if( pA->cAttachItemCount >= _countof(pA->sAttachItem) ){  
					CTimeParamSet SendTime(&QueryTimeLog, pA->SendDate, SendDate);
					if (!SendTime.IsValid()) break;					
					if(bCoinReceiveFlag) pA->biAttachCoin = 0;
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryTakeMailAttachCoin(int nMailDBID, int nChannelID, int nMapID, OUT INT64 &biCoin, UINT nGuildDBID, OUT bool &bAddGuildWare)
{
	CQueryTimeLog QueryTimeLog( "P_TakeMailAttachCoin" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, L"[QueryTakeMailAttachCoin] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_TakeMailAttachCoin(?,?,?,?,%d)}", nGuildDBID);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMailDBID, sizeof(int), SqlLen1.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCoin, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bAddGuildWare, sizeof(bool), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryTakeMailAttachItem(int nMailDBID, INT64 biSerial, int nChannelID, int nMapID, const WCHAR* pwszIP, OUT TItemInfo &ItemInfo, UINT nGuildDBID, OUT bool &bAddGuildWare)
{
	CQueryTimeLog QueryTimeLog( "P_TakeMailAttachItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, L"[QueryTakeMailAttachItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 3;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_TakeMailAttachItem2(%d,%I64d,%d,%d,N'%s',%d,%d,?)}", nMailDBID, biSerial, nChannelID, nMapID, pwszIP, nVersion, nGuildDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bAddGuildWare, sizeof(bool), &cblen );

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TIMESTAMP_STRUCT ExpireDate = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &ItemInfo.Item.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&ItemInfo.Item.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&ItemInfo.Item.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&ItemInfo.Item.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&ItemInfo.Item.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&ItemInfo.Item.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&ItemInfo.Item.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&ItemInfo.Item.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT,	&ItemInfo.Item.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&ItemInfo.Item.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&ItemInfo.Item.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT,	&ItemInfo.Item.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&ItemInfo.Item.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_TakeMailAttachItem");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");

			if(!ItemInfo.Item.bEternity) 
			{
				CTimeParamSet ExpireTime(&QueryTimeLog, ItemInfo.Item.tExpireDate, ExpireDate);
				if (!ExpireTime.IsValid())
					iSQLResult = ERROR_DB;
			}
			else
				ItemInfo.Item.tExpireDate = 0;
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryDelMail( TQDelMail* pQ, TADelMail* pA )
{
	CQueryTimeLog QueryTimeLog( "P_DelMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryDelMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	wstring wMailDBStr;
	for(int i = 0; i < MAILPAGEMAX; i++){
		if(pQ->iMailIDArray[i] > 0){
			if(!wMailDBStr.empty()){
				wMailDBStr.append(L",");
			}
			wMailDBStr.append(boost::lexical_cast<std::wstring>(pQ->iMailIDArray[i]));
		}
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelMail(?,N'%s',?)}", wMailDBStr.c_str());

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biReceiverCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iTotalMailCount, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetListMailAttachment(TQGetListMailAttachment *pQ, TAGetListMailAttachment *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListMailAttachment" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListMailAttachment] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->cCount	= 0;
	int nResult = 0;

	std::wstring wMailIDStr;
	for(int i = 0; i < MAILPAGEMAX; i++){
		if(pQ->nMailDBIDArray[i] <= 0) continue;
		if(!wMailIDStr.empty()) wMailIDStr.append(L",");
		wMailIDStr.append(boost::lexical_cast<std::wstring>(pQ->nMailDBIDArray[i]));
	}

	int nVersion = 8;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListMailAttachment(%I64d,N'%s',%d)}", pQ->biCharacterDBID, wMailIDStr.c_str(), nVersion);

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			DBPacket::TAttachMail AttachMail;
			memset(&AttachMail, 0, sizeof(AttachMail));
			int nItemCount = 0;
			BYTE cMailType = 0;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&AttachMail.nMailDBID, sizeof(int),	&cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &AttachMail.biAttachCoin,	sizeof(INT64), &cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&nItemCount, sizeof(int), &cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT,	&cMailType, sizeof(BYTE), &cblen);
			CheckColumnCount(nColNo, "P_GetListMailAttachment");
			while(1)
			{
				memset(&AttachMail, 0, sizeof(AttachMail));
				nItemCount = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->MailAttachArray[pA->cCount] = AttachMail;
				pA->MailAttachArray[pA->cCount].cItemAttachCount = nItemCount;
#ifdef PRE_ADD_JOINGUILD_SUPPORT
				pA->MailAttachArray[pA->cCount].cMailType = cMailType;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT
				pA->cCount++;

				if( pA->cCount >= _countof(pA->MailAttachArray) ){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetListMailAttachmentByMail(int nMailDBID, DBPacket::TAttachMail &MailList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListMailAttachmentByMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, L"[QueryGetListMailAttachmentByMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nCount = 0;
	int nResult = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 4;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListMailAttachmentByMail(%d,?,%d)}", nMailDBID, nVersion);

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &MailList.biAttachCoin, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TItem Item = { 0, };
			TIMESTAMP_STRUCT ExpireDate = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Item.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Item.nItemID, sizeof(int),	&cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Item.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Item.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Item.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Item.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&Item.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&Item.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT,	&Item.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&Item.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&Item.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT,	&Item.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT,	&Item.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_GetListMailAttachmentByMail");
			while(1)
			{
				memset(&Item, 0, sizeof(Item));
				memset(&ExpireDate, 0, sizeof(ExpireDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				MailList.Item[nCount] = Item;
				nCount++;

				if( nCount >= MAILATTACHITEMMAX ){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

#ifdef PRE_ADD_BEGINNERGUILD
int CDNSQLWorld::QueryGetWillSendMail(TQGetWillMails * pQ, TAGetWillMails * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetServerSendMails" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetWillSendMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetServerSendMails(%I64d)}", pQ->biCharacterDBID);

	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE)
	{
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			WillSendMail::TWillSendInfo WillMail;
			memset(&WillMail, 0, sizeof(WillSendMail::TWillSendInfo));

			SQLLEN cblen;
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&WillMail.nMailID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, WillMail.wszMemo, sizeof(WillMail.wszMemo), &cblen);

			CheckColumnCount(nColNo, "P_GetListMyTrade");
			while(1)
			{
				memset(&WillMail, 0, sizeof(WillMail));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->Mails[pA->cCount] = WillMail;
				pA->cCount++;

				if(pA->cCount >= _countof(pA->Mails))
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddWillSendMail(INT64 biChatacterDBID, int nMailID, const WCHAR * pMemo)
{
	CQueryTimeLog QueryTimeLog( "P_AddServerSendMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biChatacterDBID, 0, L"[QueryAddWillSendMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddServerSendMail(%I64d,%d,N'%s')}", biChatacterDBID, nMailID, pMemo);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryDelWillSendMail(INT64 biChatacterDBID, int nMailID)
{
	CQueryTimeLog QueryTimeLog( "P_DelServerSendMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biChatacterDBID, 0, L"[QueryDelWillSendMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelServerSendMail(%I64d,%d)}", biChatacterDBID, nMailID);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}
#endif		//#ifdef PRE_ADD_BEGINNERGUILD



#if defined(PRE_ADD_CADGE_CASH)
int CDNSQLWorld::QuerySendWishMail(TQSendWishMail *pQ, TASendWishMail *pA)
{
	CQueryTimeLog QueryTimeLog( "P_SendWishMail" );
	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biSenderCharacterDBID, 0, L"[QuerySendMail] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wstrPackageSN, wstrItemSN, wstrItemID, wstrItemOption;
	for (int i = 0; i < pQ->cWishListCount; ++i)
	{
		if (i > 0)
		{
			wstrPackageSN.append(L",");
			wstrItemSN.append(L",");
			wstrItemID.append(L",");
			wstrItemOption.append(L",");
		}

		wstrPackageSN.append(boost::lexical_cast<std::wstring>(pQ->nPackageSN));
		wstrItemSN.append(boost::lexical_cast<std::wstring>(pQ->WishList[i].nItemSN));
		wstrItemID.append(boost::lexical_cast<std::wstring>(pQ->WishList[i].nItemID));
		BYTE cOption = pQ->WishList[i].cOption;
		wstrItemOption.append(boost::lexical_cast<std::wstring>(static_cast<int>(cOption)));
	}

	WCHAR wszReceiver[MAILNAMELENMAX * 2 + 1] = {0,}, wszTitle[MAILTITLELENMAX * 2 + 1] = {0,}, wszText[MAILTEXTLENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszReceiverCharacterName, MAILNAMELENMAX, wszReceiver, _countof(wszReceiver));
	ConvertQuery(pQ->wszSubject, MAILTITLELENMAX, wszTitle, _countof(wszTitle));
	ConvertQuery(pQ->wszContent, MAILTEXTLENMAX, wszText, _countof(wszText));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	bool bMailFlag = false;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_SendWishMail(%I64d,N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',N'%s',%d,%d,?,?,?,?,?)}", 
		pQ->biSenderCharacterDBID, wszReceiver, wszTitle, wszText, wstrPackageSN.c_str(), wstrItemSN.c_str(), wstrItemID.c_str(), wstrItemOption.c_str(), pQ->nWishLimitCount, bMailFlag);

	int nSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;

		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biReceiverCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverAccountDBID, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverTotalMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiverNotReadMailCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nReceiver7DaysLeftMailCount, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return nSQLResult;
}

int CDNSQLWorld::QueryReadWishMail(TQReadWishMail *pQ, TAReadWishMail *pA)
{
	CQueryTimeLog QueryTimeLog("P_ReadWishMail");

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QUERY_READWISHMAIL] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ReadWishMail(%I64d,?,?,?,?,?,?,?)}", pQ->nMailID);

	int nCount = 0;
	int nSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		TIMESTAMP_STRUCT SendDate = {0,};

		INT64 biSenderCharacterDBID = 0, biReceiverCharacterDBID = 0;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSQLResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biSenderCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biReceiverCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pA->wszContent), 0, pA->wszContent, sizeof(pA->wszContent), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &SendDate, sizeof(TIMESTAMP_STRUCT), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biPurchaseOrderID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nNotReadMailCount, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bNewFlag, sizeof(bool), &cblen);	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nPackageSN = 0;
			DBPacket::TItemSNIDOption Item = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nPackageSN, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Item.nItemSN, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Item.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Item.cOption, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_ReadWishMail");

			while(1)
			{
				memset(&Item, 0, sizeof(DBPacket::TItemSNIDOption));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					CTimeParamSet SendTime(&QueryTimeLog, pA->SendDate, SendDate, pQ->cWorldSetID, pQ->nAccountDBID);
					if (!SendTime.IsValid())
					{
						nSQLResult = ERROR_DB;
						break;									
					}

					if (nSQLResult == ERROR_DB)
						nSQLResult = ERROR_NONE;

					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				pA->nPackageSN = nPackageSN;
				pA->WishList[pA->cWishListCount] = Item;
				pA->cWishListCount++;

				if (pA->cWishListCount >= _countof(pA->WishList)){  
					CTimeParamSet SendTime(&QueryTimeLog, pA->SendDate, SendDate);
					if (!SendTime.IsValid()) break;

					nSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nSQLResult;
}

int CDNSQLWorld::QueryModWishProducts(int nMailDBID, INT64 biPurchaseOrderID)
{
	CQueryTimeLog QueryTimeLog( "P_ModWishProducts" );

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QUERY_MODWISHPRODUCTS] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModWishProducts(%I64d,%I64d)}", nMailDBID, biPurchaseOrderID);

	return CommonReturnValueQuery(m_wszQuery);	
}

#endif	// #if defined(PRE_ADD_CADGE_CASH)

int CDNSQLWorld::QueryGetListMyTrade(TQGetListMyTrade *pQ, TAGetListMyTrade *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListMyTrade" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListMyTrade] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 15;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListMyTrade(%I64d,?,?,?,?,%d)}", pQ->biCharacterDBID, nVersion);

	pA->cMarketCount = 0;

	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE)
	{
		SQLLEN cblen;
		int nPrmNo = 1;
		int nSellingCount = 0, nExpiredCount = 0, nWeeklyRegister = 0, nRegisterCount = 0;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSellingCount, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nExpiredCount, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nWeeklyRegister, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRegisterCount, sizeof(int), &cblen );

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMyMarketInfo Market;
			memset(&Market, 0, sizeof(TMyMarketInfo));

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Market.nMarketDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cOption,	sizeof(char), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nItemPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cSellType, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nRemainTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Market.bPremiumTrade, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPayMethodCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cItemPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_GetListMyTrade");
			while(1)
			{
				memset(&Market, 0, sizeof(Market));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					pA->wSellingCount = nSellingCount;
					pA->wExpiredCount = nExpiredCount;
					pA->wWeeklyRegisterCount = nWeeklyRegister;
					pA->wRegisterItemCount = nRegisterCount;
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->MarketInfo[pA->cMarketCount] = Market;
				pA->cMarketCount++;

				if( pA->cMarketCount >= _countof(pA->MarketInfo) ){
					pA->wSellingCount = nSellingCount;
					pA->wExpiredCount = nExpiredCount;
					pA->wWeeklyRegisterCount = nWeeklyRegister;
					pA->wRegisterItemCount = nRegisterCount;
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCountMyTrade(INT64 biCharacterDBID, OUT int &nSellingCount, OUT int &nExpiredCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountMyTrade" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountMyTrade] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountMyTrade(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSellingCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nExpiredCount, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryAddTrade(TQAddTrade *pQ, TAAddTrade *pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddTrade" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddTrade] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	WCHAR wszText[(EXTDATANAMELENMAX * 2) + 1];
	memset(wszText, 0, sizeof(wszText));
	ConvertQuery(pQ->wszItemName, EXTDATANAMELENMAX, wszText, _countof(wszText));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddTrade2(%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%I64d,%d,?,%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}",
		pQ->biCharacterDBID,			// 캐릭터 ID
		pQ->wItemRegisterMaxCount,		// 등록할 수 있는 최대 개수(기간 제한 없이 현재 판매 중 상태인 거래 수)
		pQ->wRegisterMaxCount,			// 등록할 수 있는 최대 횟수(토요일 AM 04:00 기준 일주일간 등록할 수 있는 최대 횟수)
		pQ->nItemID,					// 등록할 아이템 ID
		wszText,						// 아이템 이름
		pQ->cItemGrade,					// 0=노멀, 1=매직, 2=레어, 3=에픽, 4=유니크, 5=히로익
		pQ->cLimitLevel,				// 아이템을 사용하기 위한 최소 캐릭터 레벨
		pQ->cJob,						// 직업코드
		pQ->bCash,						// 0=일반 아이템, 1=캐쉬 아이템
		pQ->nExchangeCode,				// 아이템이 속한 카테고리 ID
		pQ->nSerial,					// 등록할 아이템 고유 번호
		pQ->wCount,						// 등록할 아이템 수량
										// 아이템의 일부 수량만 등록하는 경우 새 ItemSerial을 입력하고 그렇지 않으면 NULL을 입력합니다.
		pQ->cLevel,						// 등록할 아이템 레벨
		pQ->cMethodCode,				// 1=게임머니, 3=페탈
		pQ->nPrice,						// 판매 가격
		pQ->nUnitPrice,					// 판매 단가
		pQ->nRegisterTax,				// 거래소 등록 수수료
		pQ->nSellTax,					// 거래 성립 수수료
		pQ->cSellPeriod,				// 판매 기간(시간)
		pQ->bPremiumTrade,				// 0=프리미엄 아님, 1=프리미엄
		pQ->nMapID						// 맵 ID
										// 거래소 거래 ID
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biNewSerial, sizeof(INT64), SqlLen.GetNull(pQ->biNewSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nMarketDBID, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");

		pA->bPremiumTrade = pQ->bPremiumTrade;		// 서버에서 준 값을 되돌려 준다.(굳이 서버에서 또 아이템을 찾지 않도록...)
#if defined(PRE_ADD_PETALTRADE)
		pA->cMethodCode = pQ->cMethodCode;
#endif
	}
	return nResult;
}

// 무인상점
int CDNSQLWorld::QueryGetPageTrade(TQGetPageTrade *pQ, TAGetPageTrade *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPageTrade" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetPageTrade] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 아이템 카테고리 문자열 변환
	std::wstring wItemExchangeStr;
	if(pQ->cItemExchange[0] >= 0)		// 전체검색인지 확인
	{
		for(int i = 0; i < ITEMCATAGORY_MAX; i++)
		{
			if(pQ->cItemExchange[i] > 0)
			{
				if(!wItemExchangeStr.empty()) wItemExchangeStr.append(L",");
				wItemExchangeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cItemExchange[i])));
			}
		}
	}

	// 직업코드 문자열 변환
	std::wstring wJobCodeStr;
	if(pQ->cJobCode[0] > 0)		// 전체검색인지 확인
	{
		wJobCodeStr.append(boost::lexical_cast<std::wstring>((BYTE)(0)));	// 기본 검색코드 0 입력
		for(int i = 0; i < ITEMJOB_MAX; i++)
		{
			if(pQ->cJobCode[i] > 0)
			{
				if(!wJobCodeStr.empty()) wJobCodeStr.append(L",");
				wJobCodeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cJobCode[i])));
			}
		}
	}

	// 아이템 등급 문자열 변환
	std::wstring wItemGradeStr;
	for(int i = 0; i <(ITEMRANK_MAX); i++)
	{
		if(pQ->cItemGrade[i] != -1)
		{
			if(!wItemGradeStr.empty()) wItemGradeStr.append(L",");
			wItemGradeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cItemGrade[i])));
		}
	}

	if(pQ->wPageNum <= 0) pQ->wPageNum = 1;
	
	// 아이템 이름 변환
	WCHAR wszSearchItemName[SEARCHLENMAX*2+1];	
	memset( wszSearchItemName, 0, sizeof(wszSearchItemName) );
	if( wcslen(pQ->wszSearchItemName) )
		ConvertQuery( pQ->wszSearchItemName, SEARCHLENMAX, wszSearchItemName, _countof(wszSearchItemName) );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 9;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageTradeNew(%d,%d,N'%s',N'%s',%d,%d,N'%s',N'%s',?,?,%d,?,%d)}",
		(pQ->wPageNum / 10) + 1, MARKETMAX, wItemExchangeStr.c_str(), wszSearchItemName, pQ->cMinLevel, pQ->cMaxLevel, wJobCodeStr.c_str(), wItemGradeStr.c_str(), pQ->cSortType, nVersion);

	pA->cMarketCount = 0;
	pA->nMarketTotalCount = -1;

	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	CDNSqlLen SqlLen;
	if(CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE)
	{
		SQLLEN cblen;
		int nPrmNo = 1;

		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->bCash, sizeof(BYTE), SqlLen1.GetNull(true, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cMethodCode, sizeof(BYTE), SqlLen2.GetNull(pQ->cMethodCode <= 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nMarketTotalCount, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMarketInfo Market;
			memset(&Market, 0, sizeof(TMarketInfo));

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Market.nMarketDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Market.wszSellerName, sizeof(Market.wszSellerName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cOption,	sizeof(char), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nUnitPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Market.bPremiumTrade, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cMethodCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_GetPageTrade");
			while(1)
			{
				memset(&Market, 0, sizeof(Market));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->MarketInfo[pA->cMarketCount] = Market;
				pA->cMarketCount++;

				if( pA->cMarketCount >= _countof(pA->MarketInfo) ){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}


int CDNSQLWorld::QueryGetCountTrade(TQGetPageTrade *pQ, OUT int &nTotalCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountTrade" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetCountTrade] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	// 아이템 카테고리 문자열 변환
	std::wstring wItemExchangeStr;
	for(int i = 0; i <(ITEMCATAGORY_MAX); i++){
		if(pQ->cItemExchange[i] > 0){
			if(!wItemExchangeStr.empty()) wItemExchangeStr.append(L",");
			wItemExchangeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cItemExchange[i])));
		}
	}

	// 직업코드 문자열 변환
	std::wstring wJobCodeStr;
	if(pQ->cJobCode[0] > 0)		// 전체검색인지 확인
	{
		wJobCodeStr.append(boost::lexical_cast<std::wstring>((BYTE)(0)));	// 기본 검색코드 0 입력
		for(int i = 0; i <(ITEMJOB_MAX); i++){
			if(pQ->cJobCode[i] > 0){
				if(!wJobCodeStr.empty()) wJobCodeStr.append(L",");
				wJobCodeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cJobCode[i])));
			}
		}
	}

	// 아이템 등급 문자열 변환
	std::wstring wItemGradeStr;
	for(int i = 0; i <(ITEMRANK_MAX); i++){
		if(pQ->cItemGrade[i] != -1){
			if(!wItemGradeStr.empty()) wItemGradeStr.append(L",");
			wItemGradeStr.append(boost::lexical_cast<std::wstring>((BYTE)(pQ->cItemGrade[i])));
		}
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetCountTradeNew( N'%s',N'%s',%d,%d,N'%s',N'%s',%d,%d,?)}", 
		wItemExchangeStr.c_str(), pQ->wszSearchItemName, pQ->cMinLevel,	pQ->cMaxLevel, wJobCodeStr.c_str(), wItemGradeStr.c_str(), pQ->bCash, pQ->cMethodCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nTotalCount, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryCancelTrade(TQCancelTrade *pQ, TACancelTrade *pA)
{
	CQueryTimeLog QueryTimeLog( "P_CancelTrade" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryCancelTrade] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nItemLocation = (pQ->bCashItem == true) ? DBDNWorldDef::ItemLocation::CashInventory : DBDNWorldDef::ItemLocation::Inventory;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_CancelTrade2(%I64d,%d,%d,%d,%d,?,?,?,?,?,?,?,?,?,?,?)}",
		pQ->biCharacterDBID, pQ->nMarketDBID, nItemLocation, pQ->nChannelID, pQ->nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->Item.nSerial, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->Item.nItemID, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(char), 0, &pA->Item.wCount, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &pA->Item.wDur, sizeof(USHORT), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->Item.nRandomSeed, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cLevel, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cPotential, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cOption, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cSealCount, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nPrice, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(int), 0, &pA->Item.cPotentialMoveCount, sizeof(char), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	return nResult;
}

int CDNSQLWorld::QueryBuyTradeItem(TQBuyTradeItem *pQ, TABuyTradeItem *pA)
{
	CQueryTimeLog QueryTimeLog( "P_BuyTradeItem" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryBuyTradeItem] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nItemLocation = (pQ->bCashItem == true) ? DBDNWorldDef::ItemLocation::CashInventory : DBDNWorldDef::ItemLocation::Inventory;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_BuyTradeItem2(%I64d,%d,%d,%d,%d,?,?,?,?,?,?,?,?,?,?,?,N'%s',?,?,?)}",
		pQ->biCharacterDBID, pQ->nMarketDBID, nItemLocation, pQ->nChannelID, pQ->nMapID, pQ->wszIP);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->Item.nSerial, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->Item.nItemID, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(char), 0, &pA->Item.wCount, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &pA->Item.wDur, sizeof(USHORT), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->Item.nRandomSeed, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cLevel, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cPotential, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cOption, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->Item.cSealCount, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nPrice, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biSellerCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cPayMethodCode, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(UINT), 0, &pA->nSellerADBID, sizeof(UINT), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(UINT), 0, &pA->Item.cPotentialMoveCount, sizeof(char), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListTradeForCalculation(TQGetListTradeForCalculation *pQ, TAGetListTradeForCalculation *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListTradeForCalculation" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListTradeForCalculation] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	int nVersion = 9;
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListTradeForCalculation(%I64d, %d)}", pQ->biCharacterDBID, nVersion);

	pA->cMarketCount = 0;

	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE)
	{
		SQLLEN cblen;
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMarketCalculationInfo Market;
			memset(&Market, 0, sizeof(TMarketCalculationInfo));
			TIMESTAMP_STRUCT BuyDate = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Market.nMarketDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cOption,	sizeof(char), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nUnitPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &BuyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPayMethodCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_GetListTradeForCalculation");
			while(1)
			{
				memset(&Market, 0, sizeof(Market));
				memset(&BuyDate, 0, sizeof(BuyDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->MarketInfo[pA->cMarketCount] = Market;
				CTimeParamSet BuyTime(&QueryTimeLog, pA->MarketInfo[pA->cMarketCount].tBuyDate, BuyDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!BuyTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}
				pA->cMarketCount++;

				if( pA->cMarketCount >= _countof(pA->MarketInfo) ){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetCountTradeForCalculation(INT64 biCharacterDBID, OUT int &nCalculationCount)
{
	CQueryTimeLog QueryTimeLog( "P_GetCountTradeForCalculation" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCountTradeForCalculation] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountTradeForCalculation(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nCalculationCount, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryTradeCalculate(TQTradeCalculate *pQ, TATradeCalculate *pA)
{
	CQueryTimeLog QueryTimeLog( "P_TradeCalculate" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryTradeCalculate] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_PETALTRADE)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_TradeCalculate(%I64d,%d,%d,%d,?,?)}", pQ->biCharacterDBID, pQ->nMarketDBID, pQ->nChannelID, pQ->nMapID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_TradeCalculate(%I64d,%d,%d,%d,?)}", pQ->biCharacterDBID, pQ->nMarketDBID, pQ->nChannelID, pQ->nMapID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCalculatePrice, sizeof(int), &cblen);
#if defined(PRE_ADD_PETALTRADE)
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cPayMethodCode, sizeof(char), &cblen);
#endif
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryTradeCalculateAll(TQTradeCalculateAll *pQ, TATradeCalculateAll *pA)
{
	CQueryTimeLog QueryTimeLog( "P_TradeCalculateAll" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryTradeCalculateAll] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_PETALTRADE)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_TradeCalculateAll(%I64d,%d,%d,?,?)}", pQ->biCharacterDBID, pQ->nChannelID, pQ->nMapID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_TradeCalculateAll(%I64d,%d,%d,?)}", pQ->biCharacterDBID, pQ->nChannelID, pQ->nMapID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCalculatePrice, sizeof(int), &cblen);
#if defined(PRE_ADD_PETALTRADE)
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCalculatePetal, sizeof(int), &cblen);
#endif
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetTradeItemID(int nMarketDBID, TAGetTradeItemID* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetTradeItemID" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryGetTradeItemID] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_PETALTRADE)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetTradeItemID(%d,?,?,?)}", nMarketDBID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetTradeItemID(%d,?)}", nMarketDBID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nMarketItemID, sizeof(int), &cblen);
#if defined(PRE_ADD_PETALTRADE)
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cMethodCode, sizeof(char), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nPrice, sizeof(int), &cblen);
#endif
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetTradePrice(TQGetTradePrice* pQ, TAGetTradePrice* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetTradePrice" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryGetTradePrice] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMarketPrice(%d,%d,%d)}", 
			pQ->nItemID,		// 아이템 ID
			pQ->cLevel,			// 아이템 강화레벨
			pQ->cOption			// 아이템 옵션
			);


	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMarketPrice Market;
			memset(&Market, 0, sizeof(TMarketPrice));

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPayMethodCode, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nAvgPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nMinPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nMaxPrice, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetTradePrice");
			while(1)
			{
				memset(&Market, 0, sizeof(Market));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				Market.bFlag = true;
				if(Market.cPayMethodCode == 1)
					pA->ItemPrices[0] = Market;
				else if(Market.cPayMethodCode == 3)
					pA->ItemPrices[1] = Market;
				else
					_DANGER_POINT();
			}

		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)
int CDNSQLWorld::QueryGetListMiniTrade(TQGetListMiniTrade *pQ, TAGetListMiniTrade *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListMiniTrade" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetListMiniTrade] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMiniTrade(%d,%I64d,%d)}", pQ->nItemID, pQ->biCharacterDBID, pQ->cPayMethodCode);

	pA->cMarketCount = 0;
	pA->nMarketTotalCount = -1;

	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	CDNSqlLen SqlLen;
	if(CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE)
	{
		SQLLEN cblen;
		int nPrmNo = 1;

		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TMarketInfo Market;
			memset(&Market, 0, sizeof(TMarketInfo));

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER,	&Market.nMarketDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&Market.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cOption,	sizeof(char), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Market.nUnitPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Market.bPremiumTrade, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cMethodCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Market.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nColNo, "P_GetListMiniTrade");
			while(1)
			{
				memset(&Market, 0, sizeof(Market));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->MarketInfo[pA->cMarketCount] = Market;
				pA->cMarketCount++;

				if( pA->cMarketCount >= _countof(pA->MarketInfo) ){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
#endif	// #if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM)

// PvP
#if defined (PRE_MOD_PVP_DBSP)
int CDNSQLWorld::QueryModPvPScore( TQUpdatePvPData* pQ, TAUpdatePvPData* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModPVPScore2" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryModPvPScore] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	UINT uiWinKO = 0;
	UINT uiLoseKO = 0;
	std::wstring wClassCodeStr;
	std::wstring wWinKOStr;
	std::wstring wLoseKOStr;

	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		uiWinKO = pQ->PvP.uiKOClassCount[i-1];
		uiLoseKO = pQ->PvP.uiKObyClassCount[i-1];

		if (uiWinKO || uiLoseKO)
		{
			if(!wClassCodeStr.empty()) wClassCodeStr.append(L",");
			if(!wWinKOStr.empty()) wWinKOStr.append(L",");
			if(!wLoseKOStr.empty()) wLoseKOStr.append(L",");

			wClassCodeStr.append(boost::lexical_cast<std::wstring>((BYTE)(i)));
			wWinKOStr.append(boost::lexical_cast<std::wstring>((UINT)(uiWinKO)));
			wLoseKOStr.append(boost::lexical_cast<std::wstring>((UINT)(uiLoseKO)));
		}
	}
	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModPVPScore2(%I64d,%d,%d,%d,%d,%d,%d,%d,%I64d,%I64d,%I64d,N'%s',N'%s',N'%s')}", 
		pQ->biCharacterDBID, 
		pQ->PvP.uiWin, 
		pQ->PvP.uiLose, 
		pQ->PvP.uiDraw, 
		pQ->PvP.uiGiveUpCount, 
		pQ->PvP.cLevel, 
		pQ->PvP.uiXP, 
		pQ->PvP.uiPlayTimeSec,
		pQ->PvP.biTotalKillPoint, 
		pQ->PvP.biTotalAssistPoint, 
		pQ->PvP.biTotalSupportPoint,
		wClassCodeStr.c_str(),
		wWinKOStr.c_str(),
		wLoseKOStr.c_str());

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetPvPScore( INT64 biCharacterDBID, TPvPGroup *pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPVPScore2" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetPvPScore] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPVPScore2(%I64d,?,?,?,?,?,?,?,?,?,?,?,?)}", biCharacterDBID );

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		int nNo = 1;

		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiWin, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiLose, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiDraw, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiGiveUpCount, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cLevel, sizeof(char), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiXP, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiPlayTimeSec, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biTotalKillPoint, sizeof(INT64), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biTotalAssistPoint, sizeof(INT64), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biTotalSupportPoint, sizeof(INT64), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nExpAbsoluteRank, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_FLOAT, SQL_FLOAT, sizeof(float), 0, &pA->fExpRateRank, sizeof(float), &cblen );

	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{

			int nCol = 1;

			char cClass = 0;
			int nWinKO = 0;
			int nLoseKO = 0;

			SQLBindCol( m_hstmt, nCol++, SQL_TINYINT, &cClass, sizeof(char), &cblen);
			SQLBindCol( m_hstmt, nCol++, SQL_INTEGER, &nWinKO, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nCol++, SQL_INTEGER, &nLoseKO, sizeof(int), &cblen );
			
			CheckColumnCount(nCol, "P_GetPVPScore2");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				if (CLASS_WARRIER <= cClass && cClass < CLASS_MAX)
				{
					pA->uiKOClassCount[cClass-1] = nWinKO;
					pA->uiKObyClassCount[cClass-1] = nLoseKO;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}


int CDNSQLWorld::QueryAddPvPResultLog( TQAddPvPResultLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPResultLog2" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryAddPvPResultLog] Check Connect Fail\r\n");
		return ERROR_DB;
	}


	UINT uiWinKO = 0;
	UINT uiLoseKO = 0;
	std::wstring wKOStr;

	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		uiWinKO = pQ->iVsKOWin[i-1];
		uiLoseKO = pQ->iVsKOLose[i-1];

		if (uiWinKO || uiLoseKO)
		{
			if(!wKOStr.empty()) 
				wKOStr.append(L",");
			
			wKOStr.append(boost::lexical_cast<std::wstring>((UINT)(uiWinKO)));
			wKOStr.append(L",");
			wKOStr.append(boost::lexical_cast<std::wstring>((UINT)(uiLoseKO)));
		}
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddPVPResultLog2(%I64d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d,N'%s',%d,%d)}", 
		pQ->biRoomID1, 
		pQ->iRoomID2, 
		pQ->biCharacterDBID, 
		pQ->TeamCode, 
		pQ->bBreakIntoFlag, 
		pQ->ResultCode, 
		pQ->iPlayTimeSec,
		pQ->iPvPKillPoint, 
		pQ->iPvPAssistPoint, 
		pQ->iPvPSupportPoint, 
		wKOStr.c_str(), pQ->nOccupationCount, pQ->cOccupationWinType);
	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

#else // #if defined (PRE_MOD_PVP_DBSP)

int CDNSQLWorld::QueryModPvPScore( TQUpdatePvPData* pQ, TAUpdatePvPData* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModPVPScore" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryModPvPScore] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->nAccountDBID		= pQ->nAccountDBID;

	std::wstring wszQuery;
	wszQuery += L"{?=CALL dbo.P_ModPVPScore(%I64d,%d,%d,%d,%d,%d,%d,%d,%I64d,%I64d,%I64d";
	WCHAR wszClassCount[100];
	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		memset (wszClassCount, 0x00, sizeof(wszClassCount));
		swprintf(wszClassCount, L",%d,%d", pQ->PvP.uiKOClassCount[i-1], pQ->PvP.uiKObyClassCount[i-1]);
		wszQuery += wszClassCount;
	}
	wszQuery += L")}";

	swprintf( m_wszQuery, wszQuery.c_str(), pQ->biCharacterDBID, pQ->PvP.uiWin, pQ->PvP.uiLose, 
		pQ->PvP.uiDraw, pQ->PvP.uiGiveUpCount, pQ->PvP.cLevel, pQ->PvP.uiXP, pQ->PvP.uiPlayTimeSec, 
		pQ->PvP.biTotalKillPoint, pQ->PvP.biTotalAssistPoint, pQ->PvP.biTotalSupportPoint);

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetPvPScore( INT64 biCharacterDBID, TPvPGroup *pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPVPScore" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetPvPScore] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 0;
#if defined( PRE_ADD_ACADEMIC )
	nVersion = 1;
#endif	// #if defined( PRE_ADD_ACADEMIC )
#ifdef PRE_MOD_PVPRANK
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPVPScore(%I64d,%d,?,?)}", biCharacterDBID, nVersion );
#else		//#ifdef PRE_MOD_PVPRANK
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPVPScore(%I64d,%d)}", biCharacterDBID, nVersion );
#endif		//#ifdef PRE_MOD_PVPRANK

	int nCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
#ifdef PRE_MOD_PVPRANK		
		RetCode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nExpAbsoluteRank, sizeof(int), &cblen );	
		RetCode = SQLBindParameter( m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_FLOAT, SQL_FLOAT, sizeof(int), 0, &pA->fExpRateRank, sizeof(float), &cblen );	
#endif		//#ifdef PRE_MOD_PVPRANK
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiWin, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiLose, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiDraw, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiGiveUpCount, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pA->cLevel, sizeof(BYTE), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiXP, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiPlayTimeSec, sizeof(UINT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pA->biTotalKillPoint, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pA->biTotalAssistPoint, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pA->biTotalSupportPoint, sizeof(INT64), &cblen );
			for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
			{
				SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiKOClassCount[i - 1], sizeof(UINT), &cblen );
				SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->uiKObyClassCount[i - 1], sizeof(UINT), &cblen );
			}
			CheckColumnCount(nNo, "P_GetPVPScore");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}


int CDNSQLWorld::QueryAddPvPResultLog( TQAddPvPResultLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPResultLog" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );


	std::wstring wszQuery;
	wszQuery += L"{CALL dbo.P_AddPVPResultLog(%I64d,%d,%I64d,%d,%d,%d,%d,%d,%d,%d";
	WCHAR wszClassScore[100];
	for (int i=CLASS_WARRIER; i<CLASS_MAX; i++)
	{
		memset (wszClassScore, 0x00, sizeof(wszClassScore));
		swprintf(wszClassScore, L",%d,%d", pQ->iVsKOWin[i-1], pQ->iVsKOLose[i-1]);
		wszQuery += wszClassScore;
	}
	wszQuery.append(L",");
	wszQuery.append(boost::lexical_cast<std::wstring>(pQ->nOccupationCount));
	wszQuery.append(L",");
	wszQuery.append(boost::lexical_cast<std::wstring>(pQ->cOccupationWinType));
	wszQuery += L")}";

	swprintf( m_wszQuery, wszQuery.c_str(), pQ->biRoomID1, pQ->iRoomID2, pQ->biCharacterDBID, pQ->TeamCode, pQ->bBreakIntoFlag, pQ->ResultCode, pQ->iPlayTimeSec,
		pQ->iPvPKillPoint, pQ->iPvPAssistPoint, pQ->iPvPSupportPoint);

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

#endif // #if defined (PRE_MOD_PVP_DBSP)

int CDNSQLWorld::QueryAddPVPGhoulScores(TQAddPVPGhoulScores* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPGhoulScores" );	

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPGhoulScores(%I64d,%d,%d,%d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->nGhoulWin, pQ->nHumanWin, pQ->nTimeOver, pQ->nGhoulKill,
		pQ->nHumanKill, pQ->nHolyWaterUse);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetPVPGhoulScores(INT64 biCharacterDBID, TAGetPVPGhoulScores* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPVPGhoulScores" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetPVPGhoulScores] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPVPGhoulScores(%I64d,?,?,?,?,?,?,?)}", biCharacterDBID );

	int nCount = 0;
	int nResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &nResult, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nPlayCount, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nGhoulWin, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nHumanWin, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nTimeOver, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nGhoulKill, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nHumanKill, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->nHolyWaterUse, sizeof(int),	&cblen );
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}
	return nResult;
}

#if defined(PRE_ADD_PVP_TOURNAMENT)
int CDNSQLWorld::QueryAddPVPTournamentResult(TQAddPVPTournamentResult* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPTournamentResult" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, 0, 0, 0, L"[P_AddPVPTournamentResult] Check Connect Fail\r\n");
		return ERROR_DB;
	}
	
	std::wstring wCharacterIDStr;
	std::wstring wFinalRouldStr;
	std::wstring wWinStr;

	for (int i=0; i<pQ->nTotalCount; i++)
	{
		if( pQ->biCharacterDBID[i] > 0)
		{				
			if(!wCharacterIDStr.empty()) 
				wCharacterIDStr.append(L",");

			wCharacterIDStr.append(boost::lexical_cast<std::wstring>((INT64)(pQ->biCharacterDBID[i])));

			if(!wFinalRouldStr.empty()) 
				wFinalRouldStr.append(L",");

			wFinalRouldStr.append(boost::lexical_cast<std::wstring>((int)(pQ->cTournamentStep[i])));

			if(!wWinStr.empty()) 
				wWinStr.append(L",");

			wWinStr.append(boost::lexical_cast<std::wstring>((int)(pQ->bWin[i])));
		}

	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

#if defined( PRE_PVP_GAMBLEROOM )
	swprintf(m_wszQuery, L"{CALL dbo.P_AddPVPTournamentResult(%d,N'%s',N'%s',N'%s', %I64d)}", 
		pQ->nTotalCount, wCharacterIDStr.c_str(), wFinalRouldStr.c_str(), wWinStr.c_str(), pQ->nGambleDBID);
#else
	swprintf(m_wszQuery, L"{CALL dbo.P_AddPVPTournamentResult(%d,N'%s',N'%s',N'%s')}", 
		pQ->nTotalCount, wCharacterIDStr.c_str(), wFinalRouldStr.c_str(), wWinStr.c_str());
#endif

	return (CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}
#endif //#if defined(PRE_ADD_PVP_TOURNAMENT)

int CDNSQLWorld::QueryAddPvPLadderResult( TQAddPvPLadderResult* pQ, TAAddPvPLadderResult* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPLadderResult" );

	std::string strKillDeathCount;

	// JobCode
	for( UINT i=0 ; i<pQ->cKillDeathCount ; ++i )
	{
		strKillDeathCount.append( boost::lexical_cast<std::string>(static_cast<int>(pQ->KillDeathCounts[i].cJobCode)) );
		if( i+1 == pQ->cKillDeathCount )
			strKillDeathCount.append( "|" );
		else
			strKillDeathCount.append( "," );
	}
	// KillCount
	for( UINT i=0 ; i<pQ->cKillDeathCount ; ++i )
	{
		strKillDeathCount.append( boost::lexical_cast<std::string>(pQ->KillDeathCounts[i].nKillCount) );
		if( i+1 == pQ->cKillDeathCount )
			strKillDeathCount.append( "|" );
		else
			strKillDeathCount.append( "," );
	}
	// DeathCount
	for( UINT i=0 ; i<pQ->cKillDeathCount ; ++i )
	{
		strKillDeathCount.append( boost::lexical_cast<std::string>(pQ->KillDeathCounts[i].nDeathCount) );
		if( i+1 != pQ->cKillDeathCount )
			strKillDeathCount.append( "," );
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPLadderResult(%I64d,%d,%d,%d,%d,'%S',%d)}", pQ->biCharacterDBID, pQ->cPvPLadderCode, pQ->iPvPLadderGradePoint, pQ->iHiddenPvPLadderGradePoint
																									, pQ->cResult, strKillDeathCount.c_str(), pQ->cVersusCharacterJobCode );
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListPvPLadderScore( TQGetListPvPLadderScore* pQ, TAGetListPvPLadderScore* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPVPLadderScore" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListPvPLadderScore] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListPVPLadderScore(%I64d,?,?,?)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = ERROR_DB;
		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->Data.iPvPLadderPoint, sizeof(int), &cblen );
		RetCode = SQLBindParameter( m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->Data.cMaxKillJobCode, sizeof(BYTE), &cblen );
		RetCode = SQLBindParameter( m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->Data.cMaxDeathJobCode, sizeof(BYTE), &cblen );

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TPvPLadderMatchTypeScore Data;
			memset( &Data, 0, sizeof(Data) );
			
			TIMESTAMP_STRUCT LastPvPLadderScoreDate = {0, };
			memset( &LastPvPLadderScoreDate, 0, sizeof(LastPvPLadderScoreDate));

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cPvPLadderCode, sizeof(BYTE), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iPvPLadderGradePoint, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iHiddenPvPLadderGradePoint, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iWin, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLose, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iDraw, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nTodayWin, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nTodayLose, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nTodayDraw, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nConsecutiveWin, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nConsecutiveLose, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TYPE_TIMESTAMP, &LastPvPLadderScoreDate, sizeof(TIMESTAMP_STRUCT), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT, &Data.nWeeklyCount, sizeof(short), &cblen );
			CheckColumnCount(nNo, "P_GetListPVPLadderScore");
			int iCount = 0;

			while(1)
			{
				memset( &Data, 0, sizeof(Data) );
				memset( &LastPvPLadderScoreDate, 0, sizeof(LastPvPLadderScoreDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)	
				{
					iSQLResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				if( pA->Data.cLadderTypeCount >= _countof(pA->Data.LadderScore) )				
					break;			

				pA->Data.LadderScore[pA->Data.cLadderTypeCount] = Data;
				CTimeParamSet ScoreTime(&QueryTimeLog, pA->Data.LadderScore[pA->Data.cLadderTypeCount++].tLasePvPLadderScoreDate, LastPvPLadderScoreDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!ScoreTime.IsValid())
				{
					iSQLResult = ERROR_DB;
					break;
				}
			}
			SQLCloseCursor(m_hstmt);
			return iSQLResult;
		}
	}

	return ERROR_DB;
}

int CDNSQLWorld::QueryGetListPvPLadderScoreByJob( TQGetListPvPLadderScoreByJob* pQ, TAGetListPvPLadderScoreByJob* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPVPLadderScoreByJob" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListPvPLadderScoreByJob] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListPVPLadderScoreByJob(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = 1;
		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TPvPLadderJobScore Data;
			memset( &Data, 0, sizeof(Data) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cJobCode, sizeof(BYTE),	&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iWin, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLose, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iDraw, sizeof(int), &cblen );
			CheckColumnCount(nNo, "P_GetListPVPLadderScoreByJob");

			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Data.LadderScoreByJob[pA->Data.cJobCount++] = Data;

				if( pA->Data.cJobCount >= _countof(pA->Data.LadderScoreByJob) )
				{
					SQLCloseCursor(m_hstmt);
					break;
				}
			}

			return iSQLResult;
		}
	}

	return ERROR_DB;
}

int CDNSQLWorld::QueryInitPvPLadderGradePoint( TQInitPvPLadderGradePoint* pQ, TAInitPvPLadderGradePoint* pA )
{
	CQueryTimeLog QueryTimeLog( "P_InitPVPLadderGradePoint" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_InitPVPLadderGradePoint(%I64d,%d,%d,%d,'%S')}", pQ->biCharacterDBID, pQ->cPvPLadderCode, pQ->iPvPLadderPoint, pQ->iPvPLadderGradePoint, pQ->szIP );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryUsePvPLadderPoint( TQUsePvPLadderPoint* pQ, TAUsePvPLadderPoint* pA )
{
	CQueryTimeLog QueryTimeLog( "P_UsePVPLadderPoint" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryUsePvPLadderPoint] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	CDNSqlLen SqlLen;
	int iSQLResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_UsePVPLadderPoint(%I64d,%d,%d,'%S',?)}", pQ->biCharacterDBID, pQ->iPvPLadderPoint, pQ->iMapID, pQ->szIP );

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &iSQLResult, sizeof(int),	&cblen );
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &pA->iPvPLadderPointAfter, sizeof(int),	&cblen );
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;	
}

int CDNSQLWorld::QueryAddPvPLadderCUCount( TQAddPvPLadderCUCount* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPLadderCUCount" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPLadderCUCount(%d,%d)}", pQ->cPvPLadderCode, pQ->iCUCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModPvPLadderScoresForCheat( TQModPvPLadderScoresForCheat* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModPVPLadderScoresForCheat" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPVPLadderScoresForCheat(%I64d,%d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->cPvPLadderCode, pQ->iPvPLadderPoint, pQ->iPvPLadderGradePoint, pQ->iHiddenPvPLadderGradePoint );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListPvPLadderRanking( TQGetListPvPLadderRanking* pQ, TAGetListPvPLadderRanking* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPVPLadderRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListPvPLadderRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));
	
	pA->MyRanking.biCharacterDBID = pQ->biCharacterDBID;

	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListPVPLadderRanking(%I64d,%d,?,?,?,?,?,?,?)}", pQ->biCharacterDBID, static_cast<int>(pQ->cPvPLadderCode) );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = ERROR_DB;

		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->MyRanking.iRank, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->MyRanking.iPvPLadderGradePoint, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pA->MyRanking.wszCharName), 0, pA->MyRanking.wszCharName,	sizeof(pA->MyRanking.wszCharName), &cblen );
		SQLBindParameter( m_hstmt, 5, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->MyRanking.cJobCode, sizeof(BYTE), &cblen );
		SQLBindParameter( m_hstmt, 6, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->MyRanking.iWin, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, 7, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->MyRanking.iLose, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, 8, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->MyRanking.iDraw, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TPvPLadderRanking Data;
			memset( &Data, 0, sizeof(Data) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iRank, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &Data.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iPvPLadderGradePoint, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, Data.wszCharName, sizeof(Data.wszCharName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cJobCode, sizeof(BYTE), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iWin, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLose, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iDraw, sizeof(int), &cblen );
			CheckColumnCount(nNo, "P_GetListPVPLadderRanking");
			int iCount = 0;

			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Top[iCount++] = Data;

				if( iCount >= _countof(pA->Top) )
				{
					SQLCloseCursor(m_hstmt);
					break;
				}
			}

			return iSQLResult;
		}
	}

	return ERROR_DB;
}

#ifdef PRE_MOD_PVPRANK
int CDNSQLWorld::QuerySetPvPRankCriteria(UINT nThresholdExp)
{
	CQueryTimeLog QueryTimeLog( "P_SavePVPRankCriteria" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SavePVPRankCriteria(%d)}", nThresholdExp);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryCalcPvPRank()
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPTopRanker" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPTopRanker}");

	return CommonReturnValueQuery(m_wszQuery);
}
#endif		//#ifdef PRE_MOD_PVPRANK

#if defined(PRE_ADD_PVP_RANKING)
int CDNSQLWorld::QueryGetPvPRankInfo(TQHeader* pQ, INT64 biCharacterDBID, WCHAR* wszCharName, TPvPRankingDetail * pRanking)
{									   
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPVPRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, biCharacterDBID, 0, L"[P_GetCharacterPVPRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;
	int nResult = ERROR_DB;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetCharacterPVPRanking(?,?)}");

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1, SqlLen2;

		int nNameLen = wszCharName ? (int)wcslen(wszCharName) : 0;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );				
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.GetNull(biCharacterDBID <= 0, sizeof(INT64)));		
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(WCHAR)*NAMELENMAX, 0, wszCharName, sizeof(WCHAR)*NAMELENMAX, SqlLen2.GetNull(wszCharName == NULL, sizeof(WCHAR)*nNameLen) );

		RetCode = SQLExecute( m_hstmt );

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;

			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &pRanking->wszCharName, sizeof(pRanking->wszCharName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pRanking->cJobCode, sizeof(pRanking->cJobCode), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &pRanking->wszGuildName, sizeof(pRanking->wszGuildName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->uiExp, sizeof(pRanking->uiExp), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iKill, sizeof(pRanking->iKill), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iDeath, sizeof(pRanking->iDeath), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iPvPRank, sizeof(pRanking->iPvPRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_FLOAT, &pRanking->fPvPRaito, sizeof(pRanking->fPvPRaito), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biRank, sizeof(pRanking->biRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biClassRank, sizeof(pRanking->biClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biSubClassRank, sizeof(pRanking->biSubClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedRank, sizeof(pRanking->iChangedRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedClassRank, sizeof(pRanking->iChangedClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedSubClassRank, sizeof(pRanking->iChangedSubClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iLevel, sizeof(pRanking->iLevel), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pRanking->cPvPLevel, sizeof(pRanking->cPvPLevel), &cblen );
			CheckColumnCount(nNo, "P_GetCharacterPVPRanking");

			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				nResult = ERROR_NONE;
				return nResult;
			}

			CheckRetCode(RetCode, L"SQLFetch");
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetPvPRankList(TQGetPvPRankList * pQ, TAGetPvPRankList * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPagePVPRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPagePVPRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;
	int nResult = ERROR_DB;
	int nPrmNo = 1;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));


	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPagePVPRanking(%d, %d,?,?,?,?)}", pQ->iPage, RankingSystem::RANKINGMAX );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		CDNSqlLen SqlLen1, SqlLen2, SqlLen3;
		pA->biRankingTotalCount = -1;

		int nGuildNameLen = (int)wcslen(pQ->wszGuildName);
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cClassCode, sizeof(BYTE), SqlLen1.GetNull(pQ->cClassCode <= 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cSubClassCode, sizeof(BYTE), SqlLen2.GetNull(pQ->cSubClassCode <= 0, sizeof(BYTE)));
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pQ->wszGuildName), 0, pQ->wszGuildName, sizeof(pQ->wszGuildName), SqlLen3.GetNull(nGuildNameLen <= 0, sizeof(WCHAR)*nGuildNameLen) );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biRankingTotalCount, sizeof(INT64), &cblen);

		RetCode = SQLExecute( m_hstmt );

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TPvPRanking Data;
			memset( &Data, 0, sizeof(Data) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &Data.wszCharName, sizeof(Data.wszCharName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cJobCode, sizeof(Data.cJobCode), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &Data.wszGuildName, sizeof(Data.wszGuildName), &cblen );				
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.uiExp, sizeof(Data.uiExp), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iKill, sizeof(Data.iKill), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iDeath, sizeof(Data.iDeath), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iPvPRank, sizeof(Data.iPvPRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_FLOAT, &Data.fPvPRaito, sizeof(Data.fPvPRaito), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &Data.biRank, sizeof(Data.biRank), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iChangedRank, sizeof(Data.iChangedRank), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLevel, sizeof(Data.iLevel), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cPvPLevel, sizeof(Data.cPvPLevel), &cblen );

			CheckColumnCount(nNo, "P_GetPagePVPRanking");
			pA->cRankingCount = 0;

			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RankingInfo[pA->cRankingCount++] = Data;

				if( pA->cRankingCount >= _countof(pA->RankingInfo) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetPvPLadderRankInfo(TQHeader* pQ, INT64 biCharacterDBID, WCHAR* wszCharName, BYTE cPvPLadderCode, TPvPLadderRankingDetail * pRanking)
{									   
	CQueryTimeLog QueryTimeLog( "P_GetCharacterPVPLadderRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, biCharacterDBID, 0, L"[P_GetCharacterPVPLadderRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;
	int nResult = ERROR_DB;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetCharacterPVPLadderRanking(?,?,%d)}", cPvPLadderCode);

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1, SqlLen2;

		int nNameLen = wszCharName ? (int)wcslen(wszCharName) : 0;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );				
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.GetNull(biCharacterDBID <= 0, sizeof(INT64)));		
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(WCHAR)*NAMELENMAX, 0, wszCharName, sizeof(WCHAR)*NAMELENMAX, SqlLen2.GetNull(wszCharName == NULL, sizeof(WCHAR)*nNameLen) );

		RetCode = SQLExecute( m_hstmt );

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;

			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &pRanking->wszCharName, sizeof(pRanking->wszCharName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pRanking->cJobCode, sizeof(pRanking->cJobCode), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &pRanking->wszGuildName, sizeof(pRanking->wszGuildName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iPvPLadderGradePoint, sizeof(pRanking->iPvPLadderGradePoint), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iWin, sizeof(pRanking->iWin), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iLose, sizeof(pRanking->iLose), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iDraw, sizeof(pRanking->iDraw), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->uiExp, sizeof(pRanking->uiExp), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iPvPRank, sizeof(pRanking->iPvPRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_FLOAT, &pRanking->fPvPRaito, sizeof(pRanking->fPvPRaito), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biRank, sizeof(pRanking->biRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biClassRank, sizeof(pRanking->biClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &pRanking->biSubClassRank, sizeof(pRanking->biSubClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedRank, sizeof(pRanking->iChangedRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedClassRank, sizeof(pRanking->iChangedClassRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iChangedSubClassRank, sizeof(pRanking->iChangedSubClassRank), &cblen );				
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pRanking->iLevel, sizeof(pRanking->iLevel), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pRanking->cPvPLevel, sizeof(pRanking->cPvPLevel), &cblen );
			CheckColumnCount(nNo, "P_GetCharacterPVPLadderRanking");

			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				nResult = ERROR_NONE;
				return nResult;
			}

			CheckRetCode(RetCode, L"SQLFetch");
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetPvPLadderRankList(TQGetPvPLadderRankList * pQ, TAGetPvPLadderRankList * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPagePVPLadderRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPagePVPLadderRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;
	int nResult = ERROR_DB;
	int nPrmNo = 1;

	memset( &m_wszQuery, 0, sizeof(m_wszQuery));


	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPagePVPLadderRanking(%d,%d,%d,?,?,?,?)}", pQ->iPage, RankingSystem::RANKINGMAX, pQ->cPvPLadderCode );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		CDNSqlLen SqlLen1, SqlLen2, SqlLen3;
		pA->biRankingTotalCount = -1;

		int nGuildNameLen = (int)wcslen(pQ->wszGuildName);
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cClassCode, sizeof(BYTE), SqlLen1.GetNull(pQ->cClassCode <= 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pQ->cSubClassCode, sizeof(BYTE), SqlLen2.GetNull(pQ->cSubClassCode <= 0, sizeof(BYTE)));
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pQ->wszGuildName), 0, pQ->wszGuildName, sizeof(pQ->wszGuildName), SqlLen3.GetNull(nGuildNameLen <= 0, sizeof(WCHAR)*nGuildNameLen) );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biRankingTotalCount, sizeof(INT64), &cblen);

		RetCode = SQLExecute( m_hstmt );

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TPvPLadderRanking2 Data;
			memset( &Data, 0, sizeof(Data) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &Data.wszCharName, sizeof(Data.wszCharName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cJobCode, sizeof(Data.cJobCode), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR, &Data.wszGuildName, sizeof(Data.wszGuildName), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iPvPLadderGradePoint, sizeof(Data.iPvPLadderGradePoint), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iWin, sizeof(Data.iWin), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLose, sizeof(Data.iLose), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iDraw, sizeof(Data.iDraw), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.uiExp, sizeof(Data.uiExp), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iPvPRank, sizeof(Data.iPvPRank), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_FLOAT, &Data.fPvPRaito, sizeof(Data.fPvPRaito), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &Data.biRank, sizeof(Data.biRank), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iChangedRank, sizeof(Data.iChangedRank), &cblen );			
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &Data.iLevel, sizeof(Data.iLevel), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &Data.cPvPLevel, sizeof(Data.cPvPLevel), &cblen );

			CheckColumnCount(nNo, "P_GetPagePVPLadderRanking");
			pA->cRankingCount = 0;

			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RankingInfo[pA->cRankingCount++] = Data;

				if( pA->cRankingCount >= _countof(pA->RankingInfo) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAggregatePvPRank()
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPRanking" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPRanking}");

	return CommonReturnValueQuery(m_wszQuery);
};

int CDNSQLWorld::QueryAggregatePvPLadderRank()
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPLadderRanking" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPVPLadderRanking}");

	return CommonReturnValueQuery(m_wszQuery);
};

#endif //#if defined(PRE_ADD_PVP_RANKING)

#if defined( PRE_WORLDCOMBINE_PVP )
int CDNSQLWorld::QueryAddWorldPvPRoom(TQAddWorldPvPRoom* pQ, int &nRoomID)
{
	CQueryTimeLog QueryTimeLog( "P_AddTotalPvPRoom" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, 0, 0, 0, L"[P_AddTotalPvPRoom] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddTotalPvPRoom(%d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", 
		pQ->TMissionRoom.cModeID, pQ->cWorldSetID, pQ->nServerID, pQ->nRoomID, pQ->wszRoomName, pQ->TMissionRoom.cMaxPlayers, pQ->TMissionRoom.nMapID, pQ->bBreakIntoFlag,
		pQ->TMissionRoom.bDropItem, pQ->bRegulationFlag, pQ->bObserverFlag, pQ->bRandomFlag, pQ->TMissionRoom.nMinLevel, pQ->TMissionRoom.nMaxLevel, pQ->TMissionRoom.nRoomPW, pQ->TMissionRoom.nChannelType, pQ->TMissionRoom.nPlayTime, pQ->TMissionRoom.nWinCondition );
	

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRoomID, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
	
};

int CDNSQLWorld::QueryAddWorldPvPRoomMember(TQAddWorldPvPRoomMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddTotalPvPRoomMember" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddTotalPvPRoomMember(%d,%I64d,%d,%d)}", pQ->nWorldPvPRoomDBIndex, pQ->biCharacterDBID, pQ->bObserverFlag, pQ->cMaxPlayers);

	return CommonReturnValueQuery(m_wszQuery);

};

int CDNSQLWorld::QueryDelWorldPvPRoomMember(TQDelWorldPvPRoomMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_DelTotalPvPRoomMember" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelTotalPvPRoomMember(%d,%I64d)}", pQ->nWorldPvPRoomDBIndex, pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);

};

int CDNSQLWorld::QueryDelWorldPvPRoomForServer( int nServerID )
{
	CQueryTimeLog QueryTimeLog( "P_DelTotalPvPRoomForServer" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelTotalPvPRoomForServer(%d)}", nServerID);

	return CommonReturnValueQuery(m_wszQuery);

};

int CDNSQLWorld::QueryDelWorldPvPRoom( int nWorldPvPRoomIndex )
{
	CQueryTimeLog QueryTimeLog( "P_DelTotalPvPRoom" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelTotalPvPRoom(%d)}", nWorldPvPRoomIndex);

	return CommonReturnValueQuery(m_wszQuery);

};

int CDNSQLWorld::QueryGetListWorldPvPRoom(int nCombineWorldID, TAGetListWorldPvPRoom *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListTotalPvPRoom" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, nCombineWorldID, 0, 0, 0, L"[P_GetListTotalPvPRoom] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListTotalPvPRoom(%d)}", nCombineWorldID );	
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TWorldPvPRoomDBData Data;

			int nColNo = 1;
			int nValue = 0;

			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nDBRoomID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nWorldID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nServerID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nRoomID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cModeID, sizeof(byte), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Data.wszRoomName, sizeof(WCHAR)*_countof(Data.wszRoomName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cMaxPlayers, sizeof(byte), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nMapID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bBreakIntoFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bDropItem, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bRegulationFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bObserverFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bRandomFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cMinLevel, sizeof(byte), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cMaxLevel, sizeof(byte), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nRoomMemberCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nPassWord, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cChannelType, sizeof(byte), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nPlayTimeSec, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nWinCondition, sizeof(int), &cblen );
			CheckColumnCount(nColNo, "P_GetListTotalPvPRoom");
			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->WorldPvPRoomData[pA->nCount] = Data;
				pA->nCount++;

				if( pA->nCount >= WorldPvPMissionRoom::Common::MaxDBDataCount)
				{
					nResult = ERROR_NONE;
					break;
				}				
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	

};
#endif

//#########################################################################################################################################
// DarkLair
//#########################################################################################################################################

int CDNSQLWorld::_QueryAddDarkLairRanking( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddDarkLairRanking" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[_QueryAddDarkLairRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 기본설정
	pA->cPartyUserCount = pQ->cPartyUserCount;

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;
	bool		boError = false;

	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	std::wstring wstrCharacterDBID,wstrUpdate,wstrLevel,wstrJob;
	for( UINT i=0 ; i<pQ->cPartyUserCount ; ++i )
	{
		if( i>0 )
		{
			wstrCharacterDBID.append( L"," );
			wstrUpdate.append( L"," );
			wstrLevel.append( L"," );
			wstrJob.append( L"," );
		}
		wstrCharacterDBID.append( boost::lexical_cast<std::wstring>(pQ->sUserData[i].i64CharacterDBID) );
		wstrUpdate.append( boost::lexical_cast<std::wstring>((pQ->sUserData[i].bUpdate ? 1 : 0)) );
		wstrLevel.append( boost::lexical_cast<std::wstring>(pQ->sUserData[i].unLevel) );
		wstrJob.append( boost::lexical_cast<std::wstring>(pQ->sUserData[i].iJobIndex) );
	}

	WCHAR wszPartyName[PARTYNAMELENMAX*2+1] = {0,};
	ConvertQuery(pQ->wszPartyName, PARTYNAMELENMAX, wszPartyName, _countof(wszPartyName));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDarkLairRanking(%d,%d,%d,%d,N'%s','%s','%s','%s','%s',%d,?)}", 
		pQ->iMapIndex, static_cast<int>(pQ->cPartyUserCount), pQ->unPlayRound, pQ->uiPlaySec, wszPartyName, 
		wstrCharacterDBID.c_str(), wstrUpdate.c_str(), wstrLevel.c_str(), wstrJob.c_str(), DarkLair::Rank::SelectTop );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = 1;
		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iRank, sizeof(int), &cblen );
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			INT64	i64Rank;
			WCHAR	wszPartyName[PARTYNAMELENMAX+1];
			short	nPlayRound;
			int		iPlayTime;
			WCHAR	wszCharacterNames[(NAMELENMAX+1)*DarkLair::Common::MaxPlayer];
			WCHAR	wszLevels[MAX_PATH];
			WCHAR	wszJobs[MAX_PATH];

			memset( wszPartyName, 0, sizeof(wszPartyName) );
			memset( wszCharacterNames, 0, sizeof(wszCharacterNames) );
			memset( wszLevels, 0, sizeof(wszLevels) );
			memset( wszJobs, 0, sizeof(wszJobs) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT,	&i64Rank,			sizeof(INT64),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszPartyName,		sizeof(WCHAR) * PARTYNAMELENMAX,	&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT,	&nPlayRound,		sizeof(short),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&iPlayTime,			sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszCharacterNames,	sizeof(wszCharacterNames),			&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszLevels,			sizeof(wszLevels),					&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszJobs,			sizeof(wszJobs),					&cblen );
			CheckColumnCount(nNo, "P_AddDarkLairRanking");
			int iCount = 0;

			while(1)
			{
				memset( wszPartyName, 0, sizeof(wszPartyName) );
				memset( wszCharacterNames, 0, sizeof(wszCharacterNames) );
				memset( wszLevels, 0, sizeof(wszLevels) );
				memset( wszJobs, 0, sizeof(wszJobs) );
				i64Rank = 0;				
				nPlayRound = 0;
				iPlayTime = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->sHistoryTop[iCount].Set( nPlayRound, iPlayTime, wszPartyName );

				// CharacterName
				std::wstring				wstrCharacterNamesString(wszCharacterNames);
				std::vector<std::wstring>	vCharacterNameSplit;
				boost::algorithm::split( vCharacterNameSplit, wstrCharacterNamesString, boost::algorithm::is_any_of(",") );

				// Level
				std::wstring				wstrLevelsString(wszLevels);
				std::vector<std::wstring>	vLevelSplit;
				boost::algorithm::split( vLevelSplit, wstrLevelsString, boost::algorithm::is_any_of(",") );

				// Job
				std::wstring				wstrJobsString(wszJobs);
				std::vector<std::wstring>	vJobSplit;
				boost::algorithm::split( vJobSplit, wstrJobsString, boost::algorithm::is_any_of(",") );

				for( UINT i=0 ; i<pA->cPartyUserCount ; ++i )
				{
					if( vLevelSplit.size() <= i || vJobSplit.size() <= i || vCharacterNameSplit.size() <= i )
						continue;

					// 방어코드
					boost::trim( vLevelSplit[i] );
					boost::trim( vJobSplit[i] );
					USHORT	unLevel = (vLevelSplit[i].size() > 0) ? boost::lexical_cast<USHORT>(vLevelSplit[i]) : 0;
					int		iJob	= (vJobSplit[i].size() > 0) ? boost::lexical_cast<int>(vJobSplit[i]) : 0;

					pA->sHistoryTop[iCount].AddUserData( unLevel, iJob, const_cast<WCHAR*>(vCharacterNameSplit[i].c_str()) );
				}

				++iCount;
				if( iCount >= DarkLair::Rank::SelectTop )
				{
					SQLCloseCursor(m_hstmt);
					break;
				}
			}

			return iSQLResult;
		}
	}
	
	return ERROR_DB;
}

int CDNSQLWorld::QueryGetDarkLairPersonalBest( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA, int iIdx )
{
	CQueryTimeLog QueryTimeLog( "P_GetDarkLairPersonalBest" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetDarkLairPersonalBest] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;
	bool		boError = false;

	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDarkLairPersonalBest(%I64d,%d,%d,?,?,?,?)}", 
		pQ->sUserData[iIdx].i64CharacterDBID, pQ->iMapIndex, static_cast<int>(pQ->cPartyUserCount) );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = 1;
		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER,	sizeof(int),					0, &iSQLResult,								sizeof(int),					&cblen );	
		RetCode = SQLBindParameter( m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER,	sizeof(int),					0, &pA->sBestUserData[iIdx].iRank,			sizeof(int),					&cblen );
		RetCode = SQLBindParameter( m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_WCHAR,	SQL_WVARCHAR,	sizeof(WCHAR)*PARTYNAMELENMAX,	0, pA->sBestUserData[iIdx].wszPartyName,	sizeof(WCHAR)*PARTYNAMELENMAX,	&cblen );
		RetCode = SQLBindParameter( m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_SMALLINT,	SQL_SMALLINT,	sizeof(short),					0, &pA->sBestUserData[iIdx].unPlayRound,	sizeof(short),					&cblen );
		RetCode = SQLBindParameter( m_hstmt, 5, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER,	sizeof(int),					0, &pA->sBestUserData[iIdx].uiPlaySec,		sizeof(int),					&cblen );

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			WCHAR	wszCharacterName[NAMELENMAX];
			short	nLevel;
			int		iJob;

			memset( wszCharacterName, 0, sizeof(wszCharacterName) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszCharacterName,	sizeof(wszCharacterName),	&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT,	&nLevel,			sizeof(short),				&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&iJob,				sizeof(int),				&cblen );
			CheckColumnCount(nNo, "P_GetDarkLairPersonalBest");
			while(1)
			{
				memset( wszCharacterName, 0, sizeof(wszCharacterName) );
				nLevel = 0;
				iJob = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->sBestUserData[iIdx].AddUserData( nLevel, iJob, wszCharacterName );
			}
#if 0
			INT64	i64Rank;
			WCHAR	wszPartyName[PARTYNAMELENMAX+1];
			short	nPlayRound;
			int		iPlayTime;
			WCHAR	wszCharacterNames[NAMELENMAX*DarkLair::Common::MaxPlayer];
			WCHAR	wszLevels[MAX_PATH];
			WCHAR	wszJobs[MAX_PATH];

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT,	&i64Rank,			sizeof(INT64),						&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR,		wszPartyName,		sizeof(WCHAR) * PARTYNAMELENMAX,	&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_C_SSHORT,	&nPlayRound,		sizeof(short),						&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,		&iPlayTime,			sizeof(int),						&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR,		wszCharacterNames,	sizeof(wszCharacterNames),			&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR,		wszLevels,			sizeof(wszLevels),					&cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR,		wszJobs,			sizeof(wszJobs),					&cblen );

			int iCount = 0;

			while(1)
			{
				memset( wszPartyName, 0, sizeof(wszPartyName) );
				memset( wszCharacterNames, 0, sizeof(wszCharacterNames) );
				memset( wszLevels, 0, sizeof(wszLevels) );
				memset( wszJobs, 0, sizeof(wszJobs) );
				iPlayTime = 0;
				nPlayRound = 0;
				i64Rank = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->sHistoryTop[iCount].Set( nPlayRound, iPlayTime, wszPartyName );

				// CharacterName
				std::wstring				wstrCharacterNamesString(wszCharacterNames);
				std::vector<std::wstring>	vCharacterNameSplit;
				boost::algorithm::split( vCharacterNameSplit, wstrCharacterNamesString, boost::algorithm::is_any_of(",") );

				// Level
				std::wstring				wstrLevelsString(wszLevels);
				std::vector<std::wstring>	vLevelSplit;
				boost::algorithm::split( vLevelSplit, wstrLevelsString, boost::algorithm::is_any_of(",") );

				// Job
				std::wstring				wstrJobsString(wszJobs);
				std::vector<std::wstring>	vJobSplit;
				boost::algorithm::split( vJobSplit, wstrJobsString, boost::algorithm::is_any_of(",") );

				for( UINT i=0 ; i<pA->cPartyUserCount ; ++i )
				{
					pA->sHistoryTop[iCount].AddUserData( boost::lexical_cast<USHORT>(vLevelSplit[i]), boost::lexical_cast<int>(vJobSplit[i]), const_cast<WCHAR*>(vCharacterNameSplit[i].c_str()) );
				}

				++iCount;
				if( iCount >= DarkLair::Rank::SelectTop )
				{
					SQLCloseCursor(m_hstmt);
					break;
				}
			}
#endif

			return iSQLResult;
		}
	}

	return ERROR_DB;
}

int CDNSQLWorld::QueryGetListDarkLairTopRanker( TQGetDarkLairRankBoard* pQ, TAGetDarkLairRankBoard* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListDarkLairTopRanker" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListDarkLairTopRanker] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	// 쿼리
	SQLRETURN	RetCode;
	SQLLEN		cblen;
	bool		boError = false;

	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDarkLairTopRanker(%d,%d,%d)}", pQ->iMapIndex, pQ->cPartyUserCount, DarkLair::Rank::SelectRankBoardTop );

	RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		int iSQLResult = ERROR_DB;
		RetCode = SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int		iRank;
			WCHAR	wszPartyName[PARTYNAMELENMAX+1];
			short	nPlayRound;
			int		iPlayTime;
			WCHAR	wszCharacterNames[(NAMELENMAX+1)*DarkLair::Common::MaxPlayer];
			WCHAR	wszLevels[MAX_PATH];
			WCHAR	wszJobs[MAX_PATH];

			memset( wszPartyName, 0, sizeof(wszPartyName) );
			memset( wszCharacterNames, 0, sizeof(wszCharacterNames) );
			memset( wszLevels, 0, sizeof(wszLevels) );
			memset( wszJobs, 0, sizeof(wszJobs) );

			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&iRank,				sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszPartyName,		sizeof(WCHAR) * PARTYNAMELENMAX,	&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SSHORT,	&nPlayRound,		sizeof(short),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER,	&iPlayTime,			sizeof(int),						&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszCharacterNames,	sizeof(wszCharacterNames),			&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszLevels,			sizeof(wszLevels),					&cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_WCHAR,		wszJobs,			sizeof(wszJobs),					&cblen );
			CheckColumnCount(nNo, "P_GetListDarkLairTopRanker");
			int iCount = 0;

			while(1)
			{
				memset( wszPartyName, 0, sizeof(wszPartyName) );
				memset( wszCharacterNames, 0, sizeof(wszCharacterNames) );
				memset( wszLevels, 0, sizeof(wszLevels) );
				memset( wszJobs, 0, sizeof(wszJobs) );
				iRank = 0;
				nPlayRound = 0;
				iPlayTime = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{					
					SQLCloseCursor(m_hstmt);
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->sHistoryTop[iCount].Set( nPlayRound, iPlayTime, wszPartyName );

				// CharacterName
				std::wstring				wstrCharacterNamesString(wszCharacterNames);
				std::vector<std::wstring>	vCharacterNameSplit;
				boost::algorithm::split( vCharacterNameSplit, wstrCharacterNamesString, boost::algorithm::is_any_of(",") );

				// Level
				std::wstring				wstrLevelsString(wszLevels);
				std::vector<std::wstring>	vLevelSplit;
				boost::algorithm::split( vLevelSplit, wstrLevelsString, boost::algorithm::is_any_of(",") );

				// Job
				std::wstring				wstrJobsString(wszJobs);
				std::vector<std::wstring>	vJobSplit;
				boost::algorithm::split( vJobSplit, wstrJobsString, boost::algorithm::is_any_of(",") );

				for( UINT i=0 ; i<pA->cPartyUserCount ; ++i )
				{
					if( vLevelSplit.size() <= i || vJobSplit.size() <= i || vCharacterNameSplit.size() <= i )
						continue;

					// 방어코드
					boost::trim( vLevelSplit[i] );
					boost::trim( vJobSplit[i] );
					USHORT	unLevel = (vLevelSplit[i].size() > 0) ? boost::lexical_cast<USHORT>(vLevelSplit[i]) : 0;
					int		iJob	= (vJobSplit[i].size() > 0) ? boost::lexical_cast<int>(vJobSplit[i]) : 0;

					pA->sHistoryTop[iCount].AddUserData( unLevel, iJob, const_cast<WCHAR*>(vCharacterNameSplit[i].c_str()) );
				}

				++iCount;
				if( iCount >= _countof(pA->sHistoryTop) )
				{
					SQLCloseCursor(m_hstmt);
					break;
				}
			}

			return iSQLResult;
		}
	}

	return ERROR_DB;
}

int CDNSQLWorld::QueryUpdateDarkLairResult( TQUpdateDarkLairResult* pQ, TAUpdateDarkLairResult* pA )
{
	int iRetVal = _QueryAddDarkLairRanking( pQ, pA );
	if( iRetVal != ERROR_NONE )
		return iRetVal;

	for( int i=0 ; i<pQ->cPartyUserCount ; ++i )
	{
		pA->sBestUserData[i].i64CharacterDBID = pQ->sUserData[i].i64CharacterDBID;
		iRetVal = QueryGetDarkLairPersonalBest( pQ, pA, i );
		if( iRetVal != ERROR_NONE )
			return iRetVal;
	}

	return ERROR_NONE;


#if 0
	// 프로시져 완성되기 전까지는 가데이터를 보내준다.
	// 랭크
	pA->iRank = (rand()%10000)+1;

	// Top
	for( UINT i=0 ; i<DarkLair::Rank::SelectTop ; ++i )
	{
		WCHAR wszPartyName[PARTYNAMELENMAX];
		wsprintf( wszPartyName, L"랭킹 %d위 파티임다.", i+1 );

		pA->sHistoryTop[i].Set((rand()%20)+1,(rand()%1000)+100, wszPartyName );
		for( UINT j=0 ; j<pQ->cPartyUserCount ; ++j )
		{
			WCHAR wszCharName[NAMELENMAX];
			wsprintf( wszCharName, L"바보%d", rand()%100 );

			pA->sHistoryTop[i].AddUserData((rand()%CHARLEVELMAX)+1,(rand()%36)+11, wszCharName );
		}
	}

	// MyBest
	pA->cPartyUserCount = pQ->cPartyUserCount;
	for( UINT i=0 ; i<pQ->cPartyUserCount ; ++i )
	{
		pA->sBestUserData[i].i64CharacterDBID	= pQ->sUserData[i].i64CharacterDBID;
		pA->sBestUserData[i].iRank				= (rand()%1000)+1;

		WCHAR wszPartyName[PARTYNAMELENMAX];
		wsprintf( wszPartyName, L"[%d]이게 내 베스트기록이다.", i );

		pA->sBestUserData[i].Set((rand()%20)+1,(rand()%1000)+100, wszPartyName );
		for( UINT j=0 ; j<pQ->cPartyUserCount ; ++j )
		{
			WCHAR wszCharName[NAMELENMAX];
			wsprintf( wszCharName, L"똥추%d", rand()%100 );

			pA->sBestUserData[i].AddUserData((rand()%CHARLEVELMAX)+1,(rand()%36)+11, wszCharName );
		}
	}

	return ERROR_NONE;
#endif
}

#if defined(PRE_ADD_WORLD_EVENT)
#else
int CDNSQLWorld::QueryEventList(TQEventList * pQ, TAEventList * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListEventStatus" );

	if(CheckConnect() < 0)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryEventList] Check Connect Fail\r\n");
		pA->nRetCode = ERROR_DB;
		return ERROR_DB;
	}	

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{CALL dbo.P_GetListEventStatus(%d)}", pQ->cWorldSetID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);	
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TEventInfo Info;
			memset(&Info, 0, sizeof(Info));

			TIMESTAMP_STRUCT beginTime = {0, }, endTime = {0, };
			memset(&beginTime, 0, sizeof(TIMESTAMP_STRUCT));
			memset(&endTime, 0, sizeof(TIMESTAMP_STRUCT));
			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.EventID, sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.WroldID, sizeof(UINT), &cblen);			
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, Info.wszMapIndex, sizeof(WCHAR) * EVENTMAPSTRMAX, &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TYPE_TIMESTAMP, &beginTime, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TYPE_TIMESTAMP, &endTime, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.EventType[0], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.EventType[1], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.EventType[2], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.Att[0], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.Att[1], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.Att[2], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.Att[3], sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info.Att[4], sizeof(UINT), &cblen);
			CheckColumnCount(nNo, "P_GetListEventStatus");
			while(1)
			{
				memset(&Info, 0, sizeof(Info));
				memset(&beginTime, 0, sizeof(beginTime));
				memset(&endTime, 0, sizeof(endTime));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA || pA->cCount >= EVENTLISTMAX)
				{
					nResult = ERROR_NONE;
					break;					
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				CTimeParamSet BeginTime(&QueryTimeLog, Info._tBeginTime, beginTime, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!BeginTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}			

				CTimeParamSet EndTime(&QueryTimeLog, Info._tEndTime, endTime, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!EndTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}


				memcpy(&pA->EventList[pA->cCount], &Info, sizeof(Info));
				pA->cCount++;

				if(pA->cCount >= EVENTLISTMAX){
					nResult = ERROR_NONE;
					break;					
				}
			}
			SQLCloseCursor(m_hstmt);
			return nResult;
		}
	}
	return ERROR_NONE;
}
#endif //#if defined(PRE_ADD_WORLD_EVENT)

int CDNSQLWorld::QueryMWTest( TQMWTest* pQ )
{
	if(CheckConnect() < ERROR_NONE)
		return ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DBMWTest(%d,%d)}", pQ->cThreadID, pQ->iCount );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetListMissingItem(INT64 biCharacterDBID, char &cCount, TItem *MissingItemList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListMissingItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListMissingItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 4;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMissingItem2(%I64d,%d)}", biCharacterDBID, nVersion);

	cCount = 0;
	int nSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSQLResult, sizeof(int), &cblen);
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TItem Item = { 0, };
			TIMESTAMP_STRUCT ExpireDate = { 0, };

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &Item.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Item.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Item.wCount,	sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Item.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Item.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,	&Item.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Item.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Item.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &Item.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Item.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Item.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Item.nLifespan, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &Item.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&Item.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nNo, "P_GetListMissingItem");
			while(1)
			{
				memset(&Item, 0, sizeof(Item));
				memset(&ExpireDate, 0, sizeof(ExpireDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nSQLResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				MissingItemList[cCount] = Item;
				if(!Item.bEternity) 
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, MissingItemList[cCount].tExpireDate, ExpireDate, 0, 0, biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						nSQLResult = ERROR_DB;
						break;
					}
				}
				else MissingItemList[cCount].tExpireDate = 0;

				cCount++;

				if(cCount >= MISSINGITEMMAX){
					nSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nSQLResult;
}

int CDNSQLWorld::QueryRecoverMissingItem(INT64 biCharacterDBID, INT64 biItemSerial, char cItemLocationCode, BYTE cSlotIndex)
{
	CQueryTimeLog QueryTimeLog( "P_RecoverMissingItem" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_RecoverMissingItem2(%I64d,%I64d,%d,%d)}", biItemSerial, biCharacterDBID, cItemLocationCode, cSlotIndex);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryItemMaterializeFKey(INT64 biItemSerial, OUT BYTE &cItemCode, OUT INT64 &biFKey)
{
	CQueryTimeLog QueryTimeLog( "P_GetItemMaterializeFKey" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, L"[QueryItemMaterializeFKey] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetItemMaterializeFKey(%I64d,?,?)}", biItemSerial);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &cItemCode, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biFKey, sizeof(INT64), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QuerySaveItemLocationIndex(TQSaveItemLocationIndex *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_SaveItemLocationIndex" );

	std::wstring wstrIndex, wstrSerial;
	for(int i = 0; i < pQ->nTotalInvenWareCount; i++){
		if(pQ->SaveList[i].biItemSerial <= 0) continue;

		if(!wstrIndex.empty()){
			wstrIndex.append(L",");
			wstrSerial.append(L",");
		}
		wstrIndex.append(boost::lexical_cast<std::wstring>(pQ->SaveList[i].cSlotIndex));
		wstrSerial.append(boost::lexical_cast<std::wstring>(pQ->SaveList[i].biItemSerial));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SaveItemLocationIndex2(%I64d,'%s','%s')}", pQ->biCharacterDBID, wstrIndex.c_str(), wstrSerial.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListItemLocationIndex(INT64 biCharacterDBID, int &nCount, DBPacket::TItemIndexSerial *ItemIndexList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListItemLocationIndex" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListItemLocationIndex] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	nCount = 0;
	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListItemLocationIndex(%I64d)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			DBPacket::TItemIndexSerial IndexSerial = {0,};

			int nNo = 1;

			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &IndexSerial.biItemSerial, sizeof(INT64), &cblen);		
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &IndexSerial.cSlotIndex, sizeof(BYTE), &cblen);		

			CheckColumnCount(nNo, "P_GetListItemLocationIndex");

			while(1)
			{				
				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				ItemIndexList[nCount] = IndexSerial;
				nCount++;

				if(nCount >= nItemListMaxCount){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryDeleteExpireItem(INT64 biCharacterDBID, std::list<INT64> &DelExpireitemList)
{
	CQueryTimeLog QueryTimeLog( "P_ExpireMaterializedItem2" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryDeleteExpireItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	int nVersion = 1;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ExpireMaterializedItem2(%I64d, %d)}", biCharacterDBID, nVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			INT64 biItemSerial = 0;
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biItemSerial, sizeof(INT64), &cblen);		
			CheckColumnCount(nNo, "P_ExpireMaterializedItem2");
			while(1)
			{				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				DelExpireitemList.push_back(biItemSerial);
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryDelCashItem(TQDelCashItem *pQ, TADelCashItem *pA)
{
	CQueryTimeLog QueryTimeLog( "P_DelCashIItem" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_DelCashIItem] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelCashIItem(%I64d,%d,%d,?)}", pQ->biItemSerial, pQ->wCount, pQ->nWatingTime);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		TIMESTAMP_STRUCT ExpireDate = {0,};

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");

		CTimeParamSet ExpireTime(&QueryTimeLog, pA->tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, 0);
		if (!ExpireTime.IsValid()) return ERROR_DB;
	}

	return nResult;
}

int CDNSQLWorld::QueryRecoverCashItem(TQRecoverCashItem *pQ, TARecoverCashItem *pA)
{
	CQueryTimeLog QueryTimeLog( "P_RecoverCashItem" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_RecoverCashItem] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_RecoverCashItem(%I64d,%d,?,?)}", pQ->biItemSerial, pQ->wCount);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		TIMESTAMP_STRUCT ExpireDate = {0,};

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bEternity, sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");

		CTimeParamSet ExpireTime(&QueryTimeLog, pA->tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, 0);
		if (!ExpireTime.IsValid()) return ERROR_DB;
	}

	return nResult;
}

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
int CDNSQLWorld::QueryCheckNamedItemCount(TQCheckNamedItemCount *pQ, TACheckNamedItemCount *pA)
{
	
	CQueryTimeLog QueryTimeLog( "P_GetCountMaterializedItem" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryCheckNamedItemCount] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountMaterializedItem(%d,?)}", pQ->nItemID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);				
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCount, sizeof(int), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}	

	return nResult;	
}
#endif

#if defined (PRE_ADD_EXCHANGE_POTENTIAL)
int CDNSQLWorld::QueryMovePotential(TQMovePotential * pQ, TAMovePotential * pA)
{
	CQueryTimeLog QueryTimeLog( "P_MoveItemPotential" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryMovePotential] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveItemPotential(%I64d, %I64d, %d, %I64d, %d, %d, %d)}", pQ->biCharacterDBID, pQ->biExtractSerial, pQ->cExtractPotential, \
		pQ->biInjectSerial, pQ->cInjectPotential, pQ->cMoveableLimitCount, pQ->nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}	

	return nResult;	
}
#endif		//#if defined (PRE_ADD_EXCHANGE_POTENTIAL)

#if defined (PRE_ADD_LIMITED_SHOP)
int CDNSQLWorld::QueryGetDailyLimitedShopItem(TQGetLimitedShopItem * pQ, TAGetLimitedShopItem * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListDailyItemTradeRecord" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, pQ->biCharacterDBID, 0, L"[QueryGetLimitedShopItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

#if defined( PRE_FIX_74404 )
	int nVersion = 2;
#endif

#if defined( PRE_FIX_74404 )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDailyItemTradeRecord(%I64d, %d)}", pQ->biCharacterDBID, nVersion);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDailyItemTradeRecord(%I64d)}", pQ->biCharacterDBID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			int nItemID = 0;
			int nBuyCount = 0;	
			int nShopID = 0;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nItemID, sizeof(int), &cblen);		
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nBuyCount, sizeof(int), &cblen);
#if defined( PRE_FIX_74404 )
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nShopID, sizeof(int), &cblen);
#endif
			CheckColumnCount(nNo, "P_GetListDailyItemTradeRecord");
			while(1)
			{				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;
				if(nBuyCount<=0)
					continue;
				pA->ItemData[pA->nCount].nItemID = nItemID;
				pA->ItemData[pA->nCount].nBuyCount = nBuyCount;
				pA->ItemData[pA->nCount].nResetCycle = LimitedShop::LimitedItemType::Day;
#if defined( PRE_FIX_74404 )
				pA->ItemData[pA->nCount].nShopID = nShopID;
#endif
				pA->nCount++;
				if(pA->nCount >= INVENTORYMAX + WAREHOUSEMAX)
					break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}
int CDNSQLWorld::QueryGetWeeklyLimitedShopItem(TQGetLimitedShopItem * pQ, TAGetLimitedShopItem * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListWeeklyItemTradeRecord" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, pQ->biCharacterDBID, 0, L"[QueryGetLimitedShopItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined( PRE_FIX_74404 )
	int nVersion = 2;
#endif

#if defined( PRE_FIX_74404 )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListWeeklyItemTradeRecord(%I64d,%d)}", pQ->biCharacterDBID, nVersion);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListWeeklyItemTradeRecord(%I64d)}", pQ->biCharacterDBID);
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;
			int nItemID = 0;
			int nBuyCount = 0;		
			int nShopID = 0;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nItemID, sizeof(int), &cblen);		
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nBuyCount, sizeof(int), &cblen);
#if defined( PRE_FIX_74404 )
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nShopID, sizeof(int), &cblen);
#endif
			CheckColumnCount(nNo, "P_GetListWeeklyItemTradeRecord");
			while(1)
			{				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;
				if(nBuyCount<=0)
					continue;
				pA->ItemData[pA->nCount].nItemID = nItemID;
				pA->ItemData[pA->nCount].nBuyCount = nBuyCount;
				pA->ItemData[pA->nCount].nResetCycle = LimitedShop::LimitedItemType::Week;
#if defined( PRE_FIX_74404 )
				pA->ItemData[pA->nCount].nShopID = nShopID;
#endif		//#if defined( PRE_FIX_74404 )
				pA->nCount++;
				if(pA->nCount >= INVENTORYMAX + WAREHOUSEMAX)
					break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}
int CDNSQLWorld::QueryAdddailyLimitedShopItem(TQAddLimitedShopItem * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddDailyItemTradeRecord" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined( PRE_FIX_74404 )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDailyItemTradeRecord(%I64d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->ItemData.nItemID, pQ->nBuyCount, pQ->ItemData.nShopID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDailyItemTradeRecord(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->ItemData.nItemID, pQ->nBuyCount);
#endif

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryAddWeeklyLimitedShopItem(TQAddLimitedShopItem * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddWeeklyItemTradeRecord" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
#if defined( PRE_FIX_74404 )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddWeeklyItemTradeRecord(%I64d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->ItemData.nItemID, pQ->nBuyCount, pQ->ItemData.nShopID);
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddWeeklyItemTradeRecord(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->ItemData.nItemID, pQ->nBuyCount);
#endif

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryResetdailyLimitedShopItem(TQResetLimitedShopItem * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_InitDailyItemTradeRecord" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_InitDailyItemTradeRecord(%I64d)}", pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryResetWeeklyLimitedShopItem(TQResetLimitedShopItem * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_InitWeeklyItemTradeRecord" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_InitWeeklyItemTradeRecord(%I64d)}", pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif

int CDNSQLWorld::QueryDelEffectItem(TQDelEffectItem * pQ)
{
	CQueryTimeLog QueryTimeLog( "P_DelEffectItemBySkill" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelEffectItemBySkill(%I64d,%I64d)}", pQ->biCharacterDBID, pQ->nItemSerial);

	return CommonReturnValueQuery(m_wszQuery);
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
int CDNSQLWorld::QueryModTalismanSlotOpenFlag(TQModTalismanSlotOpenFlag *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModTalismanSlotBitmap" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModTalismanSlotBitmap(%I64d,%d)}", pQ->biCharacterDBID, pQ->nTalismanOpenflag);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif

// 미션
int CDNSQLWorld::QueryGetListDailyWeeklyMission(INT64 biCharacterDBID, TUserData &UserData)
{
	CQueryTimeLog QueryTimeLog( "P_GetListDailyWeeklyMission" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListDailyWeeklyMission] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDailyWeeklyMission(%I64d)}", biCharacterDBID);

	int nDailyCount = 0, nWeeklyCount = 0, nGuildWarCount = 0, nPCBangCount = 0, nGuildCommonCount = 0, nWeekendEventCount = 0, nRepeatCount = 0, nMonthlyCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			char cMissionType = 0, cIndex = 0;
			TDailyMission Mission = { 0, };

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cMissionType, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Mission.nMissionID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Mission.nCounter, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &Mission.bAchieve, sizeof(bool), &cblen);
			CheckColumnCount(nNo, "P_GetListDailyWeeklyMission");
			while(1)
			{
				memset(&Mission, 0, sizeof(Mission));
				cMissionType = 0;
				cIndex = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				switch(cMissionType){
				case DAILYMISSION_DAY:
					if(nDailyCount >= DAILYMISSIONMAX) continue;
					UserData.Mission.DailyMission[cIndex] = Mission;
					nDailyCount++;
					break;

				case DAILYMISSION_WEEK:
					if(nWeeklyCount >= WEEKLYMISSIONMAX) continue;
					UserData.Mission.WeeklyMission[cIndex] = Mission;
					nWeeklyCount++;
					break;

#if defined(PRE_ADD_MONTHLY_MISSION)
				case DAILYMISSION_MONTH:
					if(nMonthlyCount >= MONTHLYMISSIONMAX) continue;
					UserData.Mission.MonthlyMission[cIndex] = Mission;
					nMonthlyCount++;
					break;
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

				case DAILYMISSION_GUILDWAR:
					if(nGuildWarCount >= GUILDWARMISSIONMAX) continue;
					UserData.Mission.GuildWarMission[cIndex] = Mission;
					nGuildWarCount++;
					break;

				case DAILYMISSION_PCBANG:
					if(nPCBangCount >= PCBangMissionMax) continue;
					UserData.Mission.PCBangMission[cIndex] = Mission;
					nPCBangCount++;
					break;

				case DAILYMISSION_GUILDCOMMON:
					if(nGuildCommonCount >= GUILDCOMMONMISSIONMAX) continue;
					UserData.Mission.GuildCommonMission[cIndex] = Mission;
					nGuildCommonCount++;
					break;

				case DAILYMISSION_WEEKENDEVENT:
					if (nWeekendEventCount >= WEEKENDEVENTMISSIONMAX) continue;
					UserData.Mission.WeekendEventMission[cIndex] = Mission;
					nWeekendEventCount++;
					break;

				case DAILYMISSION_WEEKENDREPEAT:
					if(nRepeatCount >= WEEKENDREPEATMISSIONMAX) continue;
					UserData.Mission.WeekendRepeatMission[cIndex] = Mission;
					nRepeatCount++;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QuerySetDailyWeeklyMission(INT64 biCharacterDBID, char cDailyMissionType, int *nMissionIDs)
{
	CQueryTimeLog QueryTimeLog( "P_SetDailyWeeklyMission" );

	std::wstring wStr;

	int nMaxSize = 0;
	switch(cDailyMissionType)
	{
	case DAILYMISSION_DAY:
		nMaxSize = DAILYMISSIONMAX;
		break;

	case DAILYMISSION_WEEK:
		nMaxSize = WEEKLYMISSIONMAX;
		break;

#if defined(PRE_ADD_MONTHLY_MISSION)
	case DAILYMISSION_MONTH:
		nMaxSize = MONTHLYMISSIONMAX;
		break;
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)

	case DAILYMISSION_GUILDWAR:
		nMaxSize = GUILDWARMISSIONMAX;
		break;

	case DAILYMISSION_PCBANG:
		nMaxSize = PCBangMissionMax;
		break;

	case DAILYMISSION_GUILDCOMMON:
		nMaxSize = GUILDCOMMONMISSIONMAX;
		break;

	case DAILYMISSION_WEEKENDEVENT:
		nMaxSize = WEEKENDEVENTMISSIONMAX;
		break;

	case DAILYMISSION_WEEKENDREPEAT:
		nMaxSize = WEEKENDREPEATMISSIONMAX;
		break;
	}

	for(int i = 0; i < nMaxSize; i++){
		if(i > 0) wStr.append(L",");
		wStr.append(boost::lexical_cast<std::wstring>(nMissionIDs[i]));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetDailyWeeklyMission(%I64d,%d,N'%s')}", biCharacterDBID, cDailyMissionType, wStr.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModDailyWeeklyMissionDetail(INT64 biCharacterDBID, char cDailyMissionType, int nMissionIndex, TDailyMission &Mission, int nMissionScore)
{
	CQueryTimeLog QueryTimeLog( "P_ModDailyWeeklyMissionDetail" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryModDailyWeeklyMissionDetail] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	CDNSqlLen SqlLen;
	int iSQLResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModDailyWeeklyMissionDetail(%I64d,%d,%d,%d,%d,%d,?)}", 
		biCharacterDBID, cDailyMissionType, nMissionIndex, Mission.nMissionID, Mission.nCounter, Mission.bAchieve);//, nMissionScore);

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		RetCode = SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	 SQL_INTEGER, sizeof(int),	 0, &iSQLResult,			 sizeof(int),	&cblen );
		RetCode = SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMissionScore, sizeof(int), SqlLen.GetNull(nMissionScore <= 0, sizeof(int)));
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryUpsGainMissionBin(INT64 biCharacterDBID, char *GainMissionList)
{
	CQueryTimeLog QueryTimeLog( "P_UpsGainMissionBin" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryUpsGainMissionBin] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsGainMissionBin(%I64d,?)}", biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		cblen = SQL_LEN_DATA_AT_EXEC(MISSIONMAX_BITSIZE);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, MISSIONMAX_BITSIZE, 0, GainMissionList, MISSIONMAX_BITSIZE, &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA){
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA){
					if(pParam == GainMissionList)
						SQLPutData(m_hstmt, pParam, MISSIONMAX_BITSIZE);
				}
			}			
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryAddAchieveMission(INT64 biCharacterDBID, int nMissionID, char cMissionType, char *MissionAchieveList)
{
	CQueryTimeLog QueryTimeLog( "P_AddAchieveMission" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddAchieveMission] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddAchieveMission(%I64d,%d,%d,?)}", biCharacterDBID, nMissionID, cMissionType);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		cblen = SQL_LEN_DATA_AT_EXEC(MISSIONMAX_BITSIZE);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, MISSIONMAX_BITSIZE, 0, MissionAchieveList, MISSIONMAX_BITSIZE, &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA){
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA){
					if(pParam == MissionAchieveList)
						SQLPutData(m_hstmt, pParam, MISSIONMAX_BITSIZE);
				}
			}			
		}
	}
	return nResult;
}

int CDNSQLWorld::QueryDelAllMissionsForCheat(INT64 biChracterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_DelAllMissionsForCheat" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelAllMissionsForCheat(%I64d)}", biChracterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

// 퀘스트
void CDNSQLWorld::ConvertToBinary(int questID, OUT char CompleteQuest[])
{
	int nIndex = questID / 8;
	int nMaskType = questID % 8;

	if(nIndex >= COMPLETEQUEST_BITSIZE) 
		return; 	

	switch(nMaskType)
	{
	case 1: CompleteQuest[nIndex] |= 0x80; break;
	case 2: CompleteQuest[nIndex] |= 0x40; break;
	case 3: CompleteQuest[nIndex] |= 0x20; break;
	case 4: CompleteQuest[nIndex] |= 0x10; break;
	case 5: CompleteQuest[nIndex] |= 0x08; break;
	case 6: CompleteQuest[nIndex] |= 0x04; break;
	case 7: CompleteQuest[nIndex] |= 0x02; break;
	case 0: CompleteQuest[nIndex] |= 0x01; break;		
	}
}

int CDNSQLWorld::QueryGetCompletedQuests(INT64 biCharacterDBID, OUT char CompleteQuest[])
{
	CQueryTimeLog QueryTimeLog("P_GetListCompleteQuest");

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListCompleteQuest] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	BYTE selectClauseCode = 2;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListCompleteQuest(%I64d,%d)}", biCharacterDBID, selectClauseCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int index = 0;
			int questID;
	
			int nNo=1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &questID, sizeof(int), &cblen);
			CheckColumnCount(nNo, "P_GetListCompleteQuest");
			while(1) 
			{
				questID = 0;
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA) {
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				ConvertToBinary(questID, CompleteQuest);
				if(++index >= MAX_QUEST_INDEX)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetListQuest(INT64 biCharacterDBID, OUT int &nCount, OUT TQuest *QuestList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListQuest" );

	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListQuest] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListQuest(?)}");

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) {
			char cIndex = 0;
			TQuest Quest = { 0, };
			TIMESTAMP_STRUCT tsRegist = {0, };
	
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Quest.nQuestID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Quest.cQuestState, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &Quest.nQuestStep, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Quest.cQuestJournal, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_BINARY, &Quest.Extra, sizeof(Quest.Extra), &cblen);
			for(int i = 0; i < QUESTMEMOMAX; i++) {
				SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Quest.nMemo[i], sizeof(char), &cblen);
			}
			SQLBindCol(m_hstmt, nColNo++, SQL_TYPE_TIMESTAMP, &tsRegist, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListQuest");
			while(1) 
			{
				memset(&Quest, 0, sizeof(Quest));
				memset(&tsRegist, 0, sizeof(tsRegist));
				cIndex = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA) {
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				if( cIndex < 0 || cIndex >= MAX_PLAY_QUEST )
				{
					g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListQuest] Invalid cIndex=%d\n", static_cast<int>(cIndex) );
					nResult = ERROR_DB;
					break;
				}

				CTimeParamSet RegistTime(&QueryTimeLog, Quest.tRegistDate, tsRegist, 0, 0, biCharacterDBID);
				if (!RegistTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}

				QuestList[cIndex] = Quest;
				nCount++;

				if(nCount >= MAX_PLAY_QUEST) {
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddQuest(INT64 biCharacterDBID, char cQuestIndex, TQuest &Quest)
{
	CQueryTimeLog QueryTimeLog( "P_AddQuest" );

	int nQuestCode = 1;		// by leekh - 검토해봐야 할 코드
	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddQuest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddQuest(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7[QUESTMEMOMAX];
		CDNSqlLen SqlLen8;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cQuestIndex, sizeof(char), SqlLen2.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &Quest.nQuestID, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.cQuestState, sizeof(char), SqlLen4.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &Quest.nQuestStep, sizeof(USHORT), SqlLen5.Get(sizeof(USHORT)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.cQuestJournal, sizeof(char), SqlLen6.Get(sizeof(char)));
		cblen = SQL_LEN_DATA_AT_EXEC(ExtraSize);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, ExtraSize, 0, Quest.Extra, ExtraSize, &cblen);
		for(int i = 0; i < QUESTMEMOMAX; i++) {
			SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.nMemo[i], sizeof(char), SqlLen7[i].Get(sizeof(char)));
		}
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &nQuestCode, sizeof(char), SqlLen8.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) {
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA) {
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA) {
					if(pParam == Quest.Extra) {
						SQLPutData(m_hstmt, pParam, ExtraSize);
					}
				}
			}			
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryDelQuest(INT64 biCharacterDBID, int nQuestIndex, int nQuestID, bool bCompleteFlag, bool bRepeatFlag, char *CompleteQuestArray)
{
	CQueryTimeLog QueryTimeLog( "P_DelQuest" );

	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryDelQuest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelQuest(%I64d,%d,%d,%d,%d,?)}", biCharacterDBID, nQuestIndex, nQuestID, bCompleteFlag, bRepeatFlag);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		cblen = SQL_LEN_DATA_AT_EXEC(COMPLETEQUEST_BITSIZE);
		// by leekh - 검토해봐야 할 코드
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, COMPLETEQUEST_BITSIZE, 0, CompleteQuestArray, COMPLETEQUEST_BITSIZE, &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) {
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA) {
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA) {
					if(pParam == CompleteQuestArray) {
						SQLPutData(m_hstmt, pParam, COMPLETEQUEST_BITSIZE);
					}
				}
			}			
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryModQuest(INT64 biCharacterDBID, char cQuestIndex, int nModFieldBit, char cQuestStatusCode, TQuest &Quest)
{
	CQueryTimeLog QueryTimeLog( "P_ModQuest" );

	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryModQuest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModQuest(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;
		CDNSqlLen SqlLen8[QUESTMEMOMAX];

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cQuestIndex, sizeof(char), SqlLen2.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &Quest.nQuestID, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nModFieldBit, sizeof(int), SqlLen4.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.cQuestState, sizeof(char), SqlLen5.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &Quest.nQuestStep, sizeof(USHORT), SqlLen6.Get(sizeof(USHORT)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.cQuestJournal, sizeof(char), SqlLen7.Get(sizeof(char)));
		cblen = SQL_LEN_DATA_AT_EXEC(ExtraSize);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, ExtraSize, 0, Quest.Extra, ExtraSize, &cblen);
		for(int i = 0; i < QUESTMEMOMAX; i++) {
			SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &Quest.nMemo[i], sizeof(char), SqlLen8[i].Get(sizeof(char)));
		}

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA) {
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA) {
					if(pParam == Quest.Extra) {
						SQLPutData(m_hstmt, pParam, ExtraSize);
					}
				}
			}			
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryClearQuest(INT64 biCharacterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_ClearQuest" );

	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryClearQuest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ClearQuest(?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryAddCompleteQuest(INT64 biCharacterDBID, int nQuestID, int nQuestCode)
{
	CQueryTimeLog QueryTimeLog( "P_AddCompleteQuest" );

	if(CheckConnect() < ERROR_NONE) {
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddCompleteQuest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddCompleteQuest(%I64d,%d,%d)}", biCharacterDBID, nQuestID, nQuestCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

// 퀵슬롯
int CDNSQLWorld::QuerySetQuickSlot(INT64 biCharacterDBID, TQuickSlot *QuickSlotArray)
{
	CQueryTimeLog QueryTimeLog( "P_SetQuickSlot" );

	std::wstring wTypeStr, wValueStr;
	for(int i = 0; i < QUICKSLOTMAX; i++){
		if(i > 0) wTypeStr.append(L",");
		wTypeStr.append(boost::lexical_cast<std::wstring>(QuickSlotArray[i].cType));
		if(i > 0) wValueStr.append(L",");
		wValueStr.append(boost::lexical_cast<std::wstring>(QuickSlotArray[i].nID));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetQuickSlot(?,N'%s',N'%s')}", wTypeStr.c_str(), wValueStr.c_str());

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListQuickSlot(INT64 biCharacterDBID, OUT int &nCount, OUT TQuickSlot *QuickSlotList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListQuickSlot" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListQuickSlot] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListQuickSlot(?)}");

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			char cIndex = 0, cType = 0;
			INT64 nValue = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cType, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &nValue, sizeof(INT64), &cblen);
			CheckColumnCount(nColNo, "P_GetListQuickSlot");
			while(1)
			{
				cIndex = 0; 
				cType = 0;
				nValue = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;


				QuickSlotList[cIndex].cType = cType;
				QuickSlotList[cIndex].nID = nValue;
				nCount++;

				if(nCount >= QUICKSLOTMAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

// 칭호
int CDNSQLWorld::QueryAddAppellation(INT64 biCharacterDBID, int nAppellationID, char *AppellationArray)
{
	CQueryTimeLog QueryTimeLog( "P_AddAppellation" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddAppellation] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddAppellation(?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nAppellationID, sizeof(int), SqlLen2.Get(sizeof(int)));
		cblen = SQL_LEN_DATA_AT_EXEC(APPELLATIONMAX_BITSIZE);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, APPELLATIONMAX_BITSIZE, 0, AppellationArray, APPELLATIONMAX_BITSIZE, &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			SQLPOINTER pParam;

			while(RetCode == SQL_NEED_DATA){
				RetCode = SQLParamData(m_hstmt, &pParam);

				if(RetCode == SQL_NEED_DATA){
					if(pParam == AppellationArray){
						SQLPutData(m_hstmt, pParam, APPELLATIONMAX_BITSIZE);
					}
				}
			}			
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryChoiceAppellation(INT64 biCharacterDBID, int nAppellationID, char cAppellationKind)
{
	CQueryTimeLog QueryTimeLog( "P_ChoiceAppellation" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ChoiceAppellation(?,?,%d)}", cAppellationKind);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nAppellationID, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}
int CDNSQLWorld::QueryDeleteAppellation(INT64 biCharacterDBID, int nAppellationID)
{
	CQueryTimeLog QueryTimeLog( "P_DelAppellation" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;	

	//마지막 인자는 필요 없다고 해서 0으로 셋팅
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelAppellation(?,?,0)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nAppellationID, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListAchieveMission(INT64 biCharacterDBID, char *MissionAchieve)
{
	CQueryTimeLog QueryTimeLog( "P_GetListAchieveMission" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListAchieveMission] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	int nVersion = 2;	// version 2 = MissionID/int

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListAchieveMission(%I64d,%d)}", biCharacterDBID, nVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nMissionID = 0;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nMissionID, sizeof(int), &cblen);

			CheckColumnCount(nNo, "P_GetListAchieveMission");

			while(1)
			{
				nMissionID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				SetBitFlag(MissionAchieve, nMissionID, true);
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetListAppellation(INT64 biCharacterDBID, char *Appellation)
{
	CQueryTimeLog QueryTimeLog( "P_GetListAppellation" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListAppellation] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	int nVersion = 3;	// version 3 = AppellationID/int

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListAppellation(%I64d,%d)}", biCharacterDBID, nVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			int nAppellationID = 0;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nAppellationID, sizeof(int), &cblen);

			CheckColumnCount(nNo, "P_GetListAppellation");

			while(1)
			{
				nAppellationID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				SetBitFlag(Appellation, nAppellationID, true);
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

// 직업
int CDNSQLWorld::QueryModJobCode(INT64 biCharacterDBID, char cJob)
{
	CQueryTimeLog QueryTimeLog( "P_ModJobCode" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModJobCode(%I64d,%d)}", biCharacterDBID, cJob);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListJobChangeLog(INT64 biCharacterDBID, OUT BYTE *JobArray)
{
	CQueryTimeLog QueryTimeLog( "P_GetListJobChangeLog" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListJobChangeLog] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListJobChangeLog(?)}");

	int nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			char cJob = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cJob, sizeof(char), &cblen);
			CheckColumnCount(nColNo, "P_GetListJobChangeLog");
			while(1)
			{
				cJob = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				JobArray[nCount] = cJob;
				nCount++;

				if(nCount >= JOBMAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

// 스킬
int CDNSQLWorld::QueryAddSkill(INT64 biCharacterDBID, int nSkillID, char cSkillLevel, int nCoolTime, char cSkillChangeCode, INT64 biCoin, int nChannelID, int nMapID, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_AddSkill" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	if(cSkillPage < 0 || cSkillPage >= DualSkill::Type::MAX)
		return nResult;
	else
		cSkillPage++;	//DB저장값은 1부터 시작

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddSkill(?,?,?,?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;
		CDNSqlLen SqlLen8;
		CDNSqlLen SqlLen9;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSkillID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillLevel, sizeof(char), SqlLen3.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nCoolTime, sizeof(int), SqlLen4.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillChangeCode, sizeof(char), SqlLen5.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCoin, sizeof(INT64), SqlLen6.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen7.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen8.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillPage, sizeof(char), SqlLen9.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModSkillLevel(INT64 biCharacterDBID, int nSkillID, char cSkillLevel, int nCoolTime, int nUsedSkillPoint, char cSkillChangeCode, int nChannelID, int nMapID, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_ModSkillLevel" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	if( cSkillPage < 0 || cSkillPage >= DualSkill::Type::MAX)
		return nResult;
	else
		cSkillPage++;	//DB값은 1부터 시작
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModSkillLevel(?,?,?,?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;
		CDNSqlLen SqlLen8;
		CDNSqlLen SqlLen9;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSkillID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillLevel, sizeof(char), SqlLen3.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nCoolTime, sizeof(int), SqlLen4.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &nUsedSkillPoint, sizeof(USHORT), SqlLen5.Get(sizeof(USHORT)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillChangeCode, sizeof(char), SqlLen6.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen7.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen8.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillPage, sizeof(char), SqlLen9.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryIncreaseSkillPoint(INT64 biCharacterDBID, short nSkillPoint, int nChannelID, int nMapID, int nChangeCode, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_IncreaseSkillPoint" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	if( cSkillPage < 0 || cSkillPage >= 2)
		return nResult;
	else
		cSkillPage++;	//DB값은 1부터 시작
	swprintf(m_wszQuery, L"{?=CALL dbo.P_IncreaseSkillPoint(?,?,?,?,%d,%d)}", nChangeCode, cSkillPage);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &nSkillPoint, sizeof(USHORT), SqlLen2.Get(sizeof(USHORT)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen4.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryDecreaseSkillPoint( TQDecreaseSkillPoint* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DecreaseSkillPoint" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	if( pQ->cSkillPage < 0 || pQ->cSkillPage >= 2 )
		return ERROR_DB;
	else
		pQ->cSkillPage++;
	
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DecreaseSkillPoint(%I64d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->nDecreaseSkillPoint, pQ->iChannelID, pQ->iMapID, pQ->cSkillPage );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelSkill(INT64 biCharacterDBID, int nSkillID, char cSkillChangeCode, bool bSkillPointBack, int nChannelID, int nMapID, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_DelSkill" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	if(cSkillPage < 0 || cSkillPage >= DualSkill::Type::MAX)
		return nResult;
	else
		cSkillPage++;	//DB저장값은 1부터 시작
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelSkill(?,?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;
		CDNSqlLen SqlLen5;
		CDNSqlLen SqlLen6;
		CDNSqlLen SqlLen7;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSkillID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillChangeCode, sizeof(char), SqlLen3.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bSkillPointBack, sizeof(bool), SqlLen4.Get(sizeof(bool)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen5.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen6.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillPage, sizeof(char), SqlLen7.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryResetSkill(INT64 biCharacterDBID, int nChannelID, int nMapID, OUT USHORT &wSkillPoint, OUT USHORT &wTotalSkillPoint, char cSkillPage)
{
	CQueryTimeLog QueryTimeLog( "P_ResetSkill" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryResetSkill] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	if(cSkillPage < 0 || cSkillPage >= DualSkill::Type::MAX)
		return nResult;
	else
		cSkillPage++;	// DB값은 1부터 시작
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ResetSkill(?,?,?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nChannelID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &wSkillPoint, sizeof(USHORT), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &wTotalSkillPoint, sizeof(USHORT), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cSkillPage, sizeof(char), SqlLen4.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryResetSkillBySkillIDs( TQResetSkillBySkillIDS* pQ, TAResetSkillBySkillIDS* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ResetSkillBySkillIDs" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryResetSkillBySkillIDs] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	std::wstring wstrSkillIDs;
	for( int i=0 ; i<_countof(pQ->iSkillIDs) ; ++i )
	{
		if( pQ->iSkillIDs[i] > 0 )
		{
			if( !wstrSkillIDs.empty() ) 
				wstrSkillIDs.append(L",");
			wstrSkillIDs.append( boost::lexical_cast<std::wstring>(pQ->iSkillIDs[i]));
		}
	}

	if( pQ->cSkillPage < 0 || pQ->cSkillPage >= 2 )
		return nResult;
	else
		pQ->cSkillPage++;	// DB값은 1부터 시작
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ResetSkillBySkillIDs(%I64d,N'%s',%d,%d,?,?, %d)}", pQ->biCharacterDBID, wstrSkillIDs.c_str(), pQ->iChannelID, pQ->iMapID, pQ->cSkillPage );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,  SQL_INTEGER,  sizeof(int),	   0, &nResult,				 sizeof(int),	 &cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &pA->nSkillPoint,		 sizeof(USHORT), &cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(USHORT), 0, &pA->nTotalSkillPoint, sizeof(USHORT), &cblen );

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListSkill(INT64 biCharacterDBID, OUT int &nCount, OUT TSkillGroup *SkillGroup)
{
	CQueryTimeLog QueryTimeLog( "P_GetListSkill" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListSkill] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListSkill(?,?)}");

	int nSkillCount[DualSkill::Type::MAX] = {0};
	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		int nVersion = 2;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nVersion, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TSkill Skill = { 0, };

			int nColNo = 1;
			BYTE cSkillCode = 0;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cSkillCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Skill.nSkillID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Skill.cSkillLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Skill.nCoolTime, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetListSkill");
			while(1)
			{
				memset(&Skill, 0, sizeof(Skill));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				if(cSkillCode == 1)	//1차 스킬
				{				
					SkillGroup[DualSkill::Type::Primary].SkillList[nSkillCount[DualSkill::Type::Primary]] = Skill;
					nSkillCount[DualSkill::Type::Primary]++;

					if(nSkillCount[DualSkill::Type::Primary] >= SKILLMAX)
					{
						nResult = ERROR_NONE;
						break;
					}
				}
				else if( cSkillCode == 2 )	//2차 스킬
				{
					SkillGroup[DualSkill::Type::Secondary].SkillList[nSkillCount[DualSkill::Type::Secondary]] = Skill;
					nSkillCount[DualSkill::Type::Secondary]++;

					if(nSkillCount[DualSkill::Type::Secondary] >= SKILLMAX)
					{
						nResult = ERROR_NONE;
						break;
					}
				}
				else
				{
					//잘못된 값임
					return nResult;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryUseExpandSkillPage(INT64 biCharacterDBID, int *nSkillID, int nSkillPoint2)
{
	CQueryTimeLog QueryTimeLog( "P_PrepareSecondSkill" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[P_PrepareSecondSkill] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wSkillIDStr, wSkillLevelStr;
	for(int i = 0; i < DEFAULTSKILLMAX; i++){
		if( nSkillID[i] > 0 ){
			if( !wSkillIDStr.empty() ) 
				wSkillIDStr.append(L",");
			wSkillIDStr.append(boost::lexical_cast<std::wstring>(nSkillID[i]));
			if( !wSkillLevelStr.empty() ) 
				wSkillLevelStr.append(L",");
			wSkillLevelStr.append(boost::lexical_cast<std::wstring>(1));
		}
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	

	swprintf(m_wszQuery, L"{?=CALL dbo.P_PrepareSecondSkill(%I64d, N'%s', N'%s', %d)}", biCharacterDBID, wSkillIDStr.c_str(), wSkillLevelStr.c_str(), nSkillPoint2);

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		RetCode = SQLBindParameter (m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	// return값을 말함

		RetCode = SQLExecute (m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QuerySetSkillPoint( TQSetSkillPoint* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_SetSkillPoint" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	if( pQ->cSkillPage < 0 || pQ->cSkillPage >= 2 )
		return ERROR_DB;
	else
		pQ->cSkillPage++;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetSkillPoint(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->nSkillPoint, pQ->cSkillPage );

	return CommonReturnValueQuery(m_wszQuery);
}

// 알리미 
int CDNSQLWorld::QueryGetListNotifier(INT64 biCharacterDBID, OUT int &nCount, OUT DNNotifier::Data *NotifierList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListNotifier" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListNotifier] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListNotifier(?)}");

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			char cIndex = 0, cType = 0;
			int nID = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cType, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nID, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetListNotifier");
			while(1)
			{
				cIndex = 0;
				cType = 0;
				nID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				NotifierList[cIndex].eType = (DNNotifier::Type::eType)cType;
				NotifierList[cIndex].iIndex = nID;
				nCount++;

				if(nCount >= DNNotifier::RegisterCount::Total){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryModNotifier(INT64 biCharacterDBID, char cNotifyIndex, char cNotifyTargetType, int nNotifyTargetID)
{
	CQueryTimeLog QueryTimeLog( "P_ModNotifier" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModNotifier(?,?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;
		CDNSqlLen SqlLen4;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cNotifyIndex, sizeof(char), SqlLen2.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cNotifyTargetType, sizeof(char), SqlLen3.Get(sizeof(char)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nNotifyTargetID, sizeof(int), SqlLen4.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

#if !defined(PRE_DELETE_DUNGEONCLEAR)
// 던전 
int CDNSQLWorld::QueryUpsDungeonClear(INT64 biCharacterDBID, int nMapID, char cDifficultyCode)
{
	CQueryTimeLog QueryTimeLog( "P_UpsDungeonClear" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsDungeonClear(?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cDifficultyCode, sizeof(char), SqlLen3.Get(sizeof(char)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListDungeonClear(INT64 biCharacterDBID, OUT int &nCount, OUT TDungeonClearData *DungeonList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListDungeonClear" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListDungeonClear] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDungeonClear(?)}");

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TDungeonClearData DungeonClear = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &DungeonClear.nMapIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &DungeonClear.cType, sizeof(char), &cblen);
			CheckColumnCount(nColNo, "P_GetListDungeonClear");
			while(1)
			{
				memset(&DungeonClear, 0, sizeof(DungeonClear));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				DungeonList[nCount] = DungeonClear;
				nCount++;

				if(nCount >= DUNGEONCLEARMAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetDungeonClear(INT64 biCharacterDBID, int nMapID, OUT char &cDifficulty)
{
	CQueryTimeLog QueryTimeLog( "P_GetDungeonClear" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetDungeonClear] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDungeonClear(?,?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nMapID, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(char), 0, &cDifficulty, sizeof(char), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

// Nest
int CDNSQLWorld::QueryUpsNestClear(INT64 biCharacterDBID, int nMapID, char cClearType)
{
	CQueryTimeLog QueryTimeLog( "P_UpsNestClear" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsNestClear(%I64d,%d,%d)}", biCharacterDBID, nMapID, cClearType);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListNestClearCount(INT64 biCharacterDBID, OUT int &nCount, OUT TNestClearData *NestList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListNestClearCount" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListNestClearCount] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 5;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListNestClearCount(%I64d, %d)}", biCharacterDBID, nVersion);

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TNestClearData NestClear = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &NestClear.nMapIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &NestClear.cClearCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &NestClear.cPCBangClearCount, sizeof(char), &cblen);
			CheckColumnCount(nColNo, "P_GetListNestClearCount");
			while(1)
			{
				memset(&NestClear, 0, sizeof(NestClear));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				NestList[nCount] = NestClear;
				nCount++;

				if(nCount >= NESTCLEARMAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryInitNestClearCount(INT64 biCharacterDBID)
{
	CQueryTimeLog QueryTimeLog( "P_InitNestClearCount" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_InitNestClearCount(%I64d)}", biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

// 친구
int CDNSQLWorld::QueryGetListFriendGroup(INT64 biCharacterDBID, OUT int &nCount, OUT TDBFriendGroupData *GroupList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListFriendGroup" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListFriendGroup] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFriendGroup(?)}");

	nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TDBFriendGroupData GroupData = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GroupData.nGroupDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GroupData.wszGroupName, sizeof(WCHAR) * FRIEND_GROUP_NAMELENMAX, &cblen);
			CheckColumnCount(nColNo, "P_GetListFriendGroup");
			while(1)
			{
				memset(&GroupData, 0, sizeof(GroupData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				GroupList[nCount] = GroupData;
				nCount++;

				if(nCount >= FRIEND_MAXCOUNT){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddFriendGroup(INT64 biCharacterDBID, WCHAR *pGroupName, int nGroupCountLimit, OUT UINT &nGroupID)
{
	CQueryTimeLog QueryTimeLog( "P_AddFriendGroup" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddFriendGroup] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddFriendGroup(?,N'%s',?,?)}", pGroupName);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGroupCountLimit, sizeof(int), SqlLen2.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGroupID, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModFriendGroupName(INT64 biCharacterDBID, int nGroupID, WCHAR *pGroupName)
{
	CQueryTimeLog QueryTimeLog( "P_ModFriendGroupName" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFriendGroupName(?,?,N'%s')}", pGroupName);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGroupID, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryDelFriendGroup(INT64 biCharacterDBID, int nGroupID)
{
	CQueryTimeLog QueryTimeLog( "P_DelFriendGroup" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelFriendGroup(?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGroupID, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListFriend(INT64 biCharacterDBID, OUT BYTE &cCount, OUT TDBFriendData *FriendList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListFriend" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListQuickSlot] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFriend(?,2)}");

	cCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TDBFriendData FriendData;
			memset(&FriendData, 0, sizeof(TDBFriendData));

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &FriendData.nGroupDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, FriendData.wszGroupName, sizeof(WCHAR) * FRIEND_GROUP_NAMELENMAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &FriendData.nFriendAccountDBID, sizeof(UINT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &FriendData.biFriendCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, FriendData.wszFriendCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, FriendData.wszMemo, sizeof(WCHAR) * FRIEND_MEMO_LEN_MAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &FriendData.bMobileAuthentication, sizeof(bool), &cblen);
			CheckColumnCount(nColNo, "P_GetListFriend");
			while(1)
			{
				memset(&FriendData, 0, sizeof(FriendData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				FriendList[cCount] = FriendData;
				cCount++;				

				if(cCount >= FRIEND_MAXCOUNT + FRIEND_GROUP_MAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddFriend(INT64 biCharacterDBID, WCHAR *pFriendCharName, WCHAR *pFriendMemo, int nFriendGroupID, int nFriendCountLimit, OUT INT64 &biFriendCharDBID, OUT UINT &nFriendAccountDBID, OUT bool &bNeedDelIsolate)
{
	CQueryTimeLog QueryTimeLog( "P_AddFriend" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddFriend] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	WCHAR wszFriendCharName[NAMELENMAX*2+1];
	memset(wszFriendCharName, 0, sizeof(wszFriendCharName));
	ConvertQuery(pFriendCharName, NAMELENMAX, wszFriendCharName, _countof(wszFriendCharName));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddFriend(?,N'%s',N'%s',?,?,?,?,?)}", wszFriendCharName, pFriendMemo);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;
		CDNSqlLen SqlLen3;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nFriendGroupID, sizeof(int), SqlLen2.GetNull(nFriendGroupID <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nFriendCountLimit, sizeof(int), SqlLen3.Get(sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biFriendCharDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(UINT), 0, &nFriendAccountDBID, sizeof(UINT), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &bNeedDelIsolate, sizeof(bool), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModFriendMemo(INT64 biCharacterDBID, INT64 biFriendCharacterDBID, WCHAR *pMemo)
{
	CQueryTimeLog QueryTimeLog( "P_ModFriendMemo" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFriendMemo(?,?,N'%s')}", pMemo);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biFriendCharacterDBID, sizeof(INT64), SqlLen2.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModFriendAndFriendGroupMapping(INT64 biCharacterDBID, int nFriendCount, INT64 *biFriendCharacterDBIDs, int nGroupID)
{
	CQueryTimeLog QueryTimeLog( "P_ModFriendAndFriendGroupMapping" );

	std::wstring wFriend;
	for(int i = 0; i < nFriendCount; i++){
		if(i > 0) wFriend.append(L",");
		wFriend.append(boost::lexical_cast<std::wstring>(biFriendCharacterDBIDs[i]));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFriendAndFriendGroupMapping(?,N'%s',?)}", wFriend.c_str());

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;
		CDNSqlLen SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGroupID, sizeof(int), SqlLen2.GetNull(nGroupID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryDelFriend(INT64 biCharacterDBID, int nFriendCount, INT64 *biFriendCharacterDBIDs)
{
	CQueryTimeLog QueryTimeLog( "P_DelFriend" );

	std::wstring wFriend;
	for(int i = 0; i < nFriendCount; i++){
		if(i > 0) wFriend.append(L",");
		wFriend.append(boost::lexical_cast<std::wstring>(biFriendCharacterDBIDs[i]));
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelFriend(?,N'%s')}", wFriend.c_str());

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

// 차단
int CDNSQLWorld::QueryAddBlockedCharacter(INT64 biCharacterDBID, WCHAR *pBlockedCharName, OUT INT64 &biBlockedCharacterDBID, OUT bool &bNeedDelFriend, int nIsolateCountMax)
{
	CQueryTimeLog QueryTimeLog( "P_AddBlockedCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryAddBlockedCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	WCHAR wszBlockedCharName[(NAMELENMAX*2)+1];
	memset(wszBlockedCharName, 0, sizeof(wszBlockedCharName));
	ConvertQuery(pBlockedCharName, NAMELENMAX, wszBlockedCharName, _countof(wszBlockedCharName));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddBlockedCharacter(?,N'%s',?,?,?)}", wszBlockedCharName);

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1, SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biBlockedCharacterDBID, sizeof(INT64), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT,  sizeof(bool), 0, &bNeedDelFriend, sizeof(bool), &cblen );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nIsolateCountMax, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetListBlockedCharacter(INT64 biCharacterDBID, OUT BYTE &cCount, OUT TIsolateItem * pIsolateItems)
{
	CQueryTimeLog QueryTimeLog( "P_GetListBlockedCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListBlockedCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListBlockedCharacter(?)}");

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			INT64	biCharacterDBID = 0;
			WCHAR	wszCharacterName[NAMELENMAX];

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszCharacterName, sizeof(WCHAR)*NAMELENMAX, &cblen );
			CheckColumnCount(nColNo, "P_GetListBlockedCharacter");
			while(1)
			{
				memset(&wszCharacterName, 0, sizeof(wszCharacterName));
				biCharacterDBID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pIsolateItems[cCount].biIsolateCharacterDBID = biCharacterDBID;
				_wcscpy(pIsolateItems[cCount].wszIsolateName, _countof(pIsolateItems[cCount].wszIsolateName), wszCharacterName, (int)wcslen(wszCharacterName));
				cCount++;

				if(cCount >= ISOLATELISTMAX)
				{
					break;
				}
			}

		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryDelBlockedCharacter(INT64 biCharacterDBID, UINT uiCount, INT64 *biBlockedCharacterDBIDs)
{
	CQueryTimeLog QueryTimeLog( "P_DelBlockedCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryDelBlockedCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wstrDBIDs;
	for( UINT i=0 ; i<uiCount ; ++i )
	{
		if( i )
			wstrDBIDs.append( L"," );
		wstrDBIDs.append( boost::lexical_cast<std::wstring>(biBlockedCharacterDBIDs[i]) );
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelBlockedCharacter(?,N'%s')}", wstrDBIDs.c_str() );

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

// Equipment 
int CDNSQLWorld::QueryGetListEquipment(INT64 biCharacterDBID, int *EquipList, int *CashEquipList)
{
	CQueryTimeLog QueryTimeLog( "P_GetListEquipment" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetListEquipment] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListEquipment2(%I64d)}", biCharacterDBID);

	int nCount = 0, nCashCount = 0;
	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			char cItemLocation = 0, cIndex = 0;
			int iItemID = 0;
			int	iLookItemID = 0;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cItemLocation, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cIndex, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &iItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &iLookItemID, sizeof(int), &cblen );
			CheckColumnCount(nColNo, "P_GetListEquipment");
			while(1)
			{
				cItemLocation = 0;
				cIndex = 0;
				iItemID = 0;
				iLookItemID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				int nMaxSlot = 0;
				switch(cItemLocation)
				{
				case DBDNWorldDef::ItemLocation::Equip:
					{
						nMaxSlot = EQUIPMAX;
						if(cIndex >= 0 && cIndex < nMaxSlot)
						{
							//탈것이 cIndex = -1로 넘어온다. 스택 안깨먹게 //2hogi
							if(iLookItemID > 0)
								EquipList[cIndex] = iLookItemID;
							else
								EquipList[cIndex] = iItemID;
						}
						nCount++;

						if(nCount >= nMaxSlot){
							iSQLResult = ERROR_NONE;
							break;
						}
					}
					break;

				case DBDNWorldDef::ItemLocation::CashEquip:
					{
						nMaxSlot = CASHEQUIPMAX;
						if(cIndex >= 0 && cIndex < nMaxSlot)
						{
							//탈것이 cIndex = -1로 넘어온다. 스택 안깨먹게 //2hogi
							if(iLookItemID > 0)
								CashEquipList[cIndex] = iLookItemID;
							else
								CashEquipList[cIndex] = iItemID;
						}
						nCashCount++;

						if(nCashCount >= nMaxSlot){
							iSQLResult = ERROR_NONE;
							break;
						}
					}
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryUpsEquipment(INT64 biCharacterDBID, int nItemLocation, int nSlotIndex, INT64 biSerial)
{
	CQueryTimeLog QueryTimeLog( "P_UpsEquipment" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UpsEquipment(%I64d,%d,%d,%I64d)}", biCharacterDBID, nItemLocation, nSlotIndex, biSerial);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetCharacter(int nWorldSetID, INT64 biCharacterDBID, UINT nLastServeType, OUT TASelectCharacter *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetCharacter2" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int iVersion = 8;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCharacter2(%I64d,%d,%d)}", biCharacterDBID, nLastServeType, iVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			WCHAR wszLastMissionAchieveIDs[1024] = { 0, };
			TIMESTAMP_STRUCT DailyDate = { 0, }, WeeklyDate = { 0, }, MonthlyDate = { 0, }, FatigueDate = { 0, }, RebirthDate = { 0, }, LastDailyCheckDate = { 0, };	// by leekh - 검토해봐야 할코드
			TIMESTAMP_STRUCT EventFatigueDate = { 0, };
			TIMESTAMP_STRUCT LastConnectDate = { 0, };
			TIMESTAMP_STRUCT TimeEventDate = { 0, };
			TIMESTAMP_STRUCT LastStampDate = { 0, };
			int nViewCashEquipBitmap = 0;

			int nNo = 1;
			BYTE cCurrentSkillCode = 0;	// 나중에 이중스킬트리 하면 필요한거?
			USHORT wSkillPoint1 = 0, wSkillPoint2 = 0;
			INT64 biPrivateChatChannel = 0;
			BYTE cGuildJoinLevel = 0;
			bool bGuildMaxLevelReward = true;
			int nTalismanOpenFlag = 0;
#if defined( PRE_ADD_DWC )
			WCHAR wszTempCharacterName[ NAMELENMAX + DWCPREFIXLEN ] = L"";
#endif // #if defined( PRE_ADD_DWC )

			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cAccountLevel, sizeof(char), &cblen);
#if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszTempCharacterName, sizeof(WCHAR) * (NAMELENMAX + DWCPREFIXLEN), &cblen);
#else // #if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &pA->UserData.Status.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);
#endif // #if defined( PRE_ADD_DWC )
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &pA->UserData.Status.IsCheckFirstVillage, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cClass, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cJob, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nExp, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &pA->UserData.Status.nCoin, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &pA->UserData.Status.nWarehouseCoin, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.dwHairColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.dwEyeColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.dwSkinColor, sizeof(DWORD), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cLastVillageGateNo, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nLastSubVillageMapIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nLastVillageMapIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nMapIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nPosX, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nPosY, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nPosZ, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_FLOAT, &pA->UserData.Status.fRotate, sizeof(float), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wPCBangFatigue, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wFatigue, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wWeeklyFatigue, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &FatigueDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nDefaultBody, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nDefaultLeg, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nDefaultHand, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nDefaultFoot, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Mission.nMissionScore, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &wSkillPoint1, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &wSkillPoint2, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Appellation.nSelectAppellation, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Appellation.nCoverAppellation, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszLastMissionAchieveIDs, sizeof(wszLastMissionAchieveIDs), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cRebirthCoin, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->UserData.Status.cPCBangRebirthCoin, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &RebirthDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wCashRebirthCoin, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &DailyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &WeeklyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_BINARY, &pA->UserData.Mission.MissionGain, sizeof(char) * MISSIONMAX_BITSIZE, &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.wMarketRegisterCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nMarketSellSuccess, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nMarketSellFailure, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nMarketRegisterTax, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->UserData.Status.nMarketSellTax, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pA->GuildSelfView.GuildUID.nDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, pA->GuildSelfView.wszGuildName, sizeof(pA->GuildSelfView.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->GuildSelfView.btGuildRole, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wEventFatigue, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &EventFatigueDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->UserData.Status.wVIPFatigue, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastDailyCheckDate, sizeof(TIMESTAMP_STRUCT), &cblen);	// by leekh - 검토해봐야 할코드
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastConnectDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->GuildSelfView.wWarSchduleID, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pA->GuildSelfView.cTeamColorCode, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &TimeEventDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->GuildSelfView.wGuildMark, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->GuildSelfView.wGuildMarkBG, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &pA->GuildSelfView.wGuildMarkBorder, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cCurrentSkillCode, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nViewCashEquipBitmap, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &biPrivateChatChannel, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &cGuildJoinLevel, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &bGuildMaxLevelReward, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastStampDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &MonthlyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &nTalismanOpenFlag, sizeof(int), &cblen);

			CheckColumnCount(nNo, "P_GetCharacter2");
			RetCode = SQLFetch(m_hstmt);
			if( RetCode == SQL_NO_DATA )
			{
				SQLCloseCursor(m_hstmt);
				return nResult;
			}
			if(CheckRetCode(RetCode, L"SQLFetch") == ERROR_NONE){

				CTimeParamSet FatigueTime(&QueryTimeLog, pA->UserData.Status.tLastFatigueDate, FatigueDate, 0, 0, biCharacterDBID);
				if (!FatigueTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet RebirthTime(&QueryTimeLog, pA->UserData.Status.tLastRebirthCoinDate, RebirthDate, 0, 0, biCharacterDBID);
				if (!RebirthTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet DailyTime(&QueryTimeLog, pA->UserData.Mission.tDailyMissionDate, DailyDate, 0, 0, biCharacterDBID);
				if (!DailyTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet WeeklyTime(&QueryTimeLog, pA->UserData.Mission.tWeeklyMissionDate, WeeklyDate, 0, 0, biCharacterDBID);
				if (!WeeklyTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
#if defined(PRE_ADD_MONTHLY_MISSION)
				CTimeParamSet MonthlyTime(&QueryTimeLog, pA->UserData.Mission.tMonthlyMissionDate, MonthlyDate, 0, 0, biCharacterDBID);
				if (!MonthlyTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
#endif	// #if defined(PRE_ADD_MONTHLY_MISSION)
				CTimeParamSet EventFatigueTime(&QueryTimeLog, pA->UserData.Status.tLastEventFatigueDate, EventFatigueDate, 0, 0, biCharacterDBID);
				if (!EventFatigueTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet LastConnectTime(&QueryTimeLog, pA->UserData.Status.tLastConnectDate, LastConnectDate, 0, 0, biCharacterDBID);
				if (!LastConnectTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
				CTimeParamSet EventTime(&QueryTimeLog, pA->UserData.Status.tLastTimeEventDate, TimeEventDate, 0, 0, biCharacterDBID);
				if (!EventTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
#if defined( PRE_ADD_STAMPSYSTEM )
				CTimeParamSet StampTime(&QueryTimeLog, pA->UserData.Status.tLastStampDate, LastStampDate, 0, 0, biCharacterDBID);
				if (!StampTime.IsValid())
				{
					SQLCloseCursor(m_hstmt);
					return nResult;
				}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined( PRE_ADD_DWC )
				int nPrefixOffset = 0;
				if(pA->UserData.Status.cAccountLevel == AccountLevel_DWC)
				{
					nPrefixOffset = DWCPREFIXLEN;
				}
				_wcscpy(pA->UserData.Status.wszCharacterName, NAMELENMAX, wszTempCharacterName + nPrefixOffset, NAMELENMAX);
#endif // #if defined( PRE_ADD_DWC )

				pA->UserData.Status.cSkillPage = cCurrentSkillCode - 1;
				pA->UserData.Skill[DualSkill::Type::Primary].wSkillPoint = wSkillPoint1;
				pA->UserData.Skill[DualSkill::Type::Secondary].wSkillPoint = wSkillPoint2;

#if defined( PRE_PRIVATECHAT_CHANNEL )
				pA->UserData.Status.nPrivateChatChannel = biPrivateChatChannel;
#endif

#ifdef PRE_ADD_JOINGUILD_SUPPORT
				pA->cGuildJoinLevel = cGuildJoinLevel;
				pA->bWasRewardedGuildMaxLevel = bGuildMaxLevelReward;
#endif		//#ifdef PRE_ADD_JOINGUILD_SUPPORT

#if defined(PRE_ADD_TALISMAN_SYSTEM)
				pA->nTalismanOpenFlag = nTalismanOpenFlag;
#endif
				std::wstring wstrLastMissionIDs(wszLastMissionAchieveIDs);
				std::vector<std::wstring> vLastMissionSplit;
				boost::algorithm::split( vLastMissionSplit, wstrLastMissionIDs, boost::algorithm::is_any_of(",") );
				memcpy(pA->UserData.Status.cViewCashEquipBitmap, &nViewCashEquipBitmap, sizeof(char) * VIEWCASHEQUIPMAX_BITSIZE);
				
				for( int i=0 ; i<LASTMISSIONACHIEVEMAX ; ++i )
					pA->UserData.Mission.wLastMissionAchieve[i] = -1;
				
				if( wcslen(wszLastMissionAchieveIDs) == 0 )
					vLastMissionSplit.clear();

				if( !vLastMissionSplit.empty()){
					int nCount = (int)vLastMissionSplit.size();
					if(nCount > LASTMISSIONACHIEVEMAX) nCount = LASTMISSIONACHIEVEMAX;
					for(int i = 0; i < nCount; i++){
						pA->UserData.Mission.wLastMissionAchieve[i] = _wtoi(vLastMissionSplit[i].c_str());
					}
				}

				if(GUILDDBID_DEF != pA->GuildSelfView.GuildUID.nDBID) {
					pA->GuildSelfView.GuildUID.nWorldID = nWorldSetID;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryCreateGuild(TQCreateGuild *pQ, TACreateGuild *pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuild" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryCreateGuild] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuild(N'%s',N'%s',%I64d,%d,N'%s',%d,%d, ?, ?)}", 
		pQ->wszGuildName,		// 길드이름
		" ",					// 디폴트 공지내용
		pQ->nCharacterDBID,	// 길드마스터 캐릭터 ID
		pQ->cLevel,			// 길드마스터 캐릭터 레벨
		" ",					// 디폴트 길드마스터 자기소개
		pQ->nEstablishExpense,// 길드창설 비용
		pQ->nMapID			// 맵 ID
		);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		SQL_TIMESTAMP_STRUCT JoinDate; 
		tm tmJoinDate;
		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nGuildDBID, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, &JoinDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);
		
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) 
		{
			::memset(&tmJoinDate, 0, sizeof(tmJoinDate));
			tmJoinDate.tm_year	= JoinDate.year - DF_TM_YEAR_EPOCH;
			tmJoinDate.tm_mon	= JoinDate.month - 1;
			tmJoinDate.tm_mday	= JoinDate.day;
			tmJoinDate.tm_hour	= JoinDate.hour;
			tmJoinDate.tm_min	= JoinDate.minute;
			tmJoinDate.tm_sec	= JoinDate.second;
			pA->JoinDate		= CTimeSet::ConvertTmToTimeT64_LC(&tmJoinDate);
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryGetGuildInfo(UINT nGuildDBID, char cWorldSetID, TAGetGuildInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuild" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, cWorldSetID, 0, 0, 0, L"[QueryGetGuildInfo] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;

	int nVersion = 12;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuild(%d,%d)}", nGuildDBID, nVersion);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			SQL_TIMESTAMP_STRUCT OpenDate; 
			tm tmOpenDate;

			int nColNo = 1;
			
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &pA->Info.GuildView.wszGuildName, sizeof(pA->Info.GuildView.wszGuildName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->Info.wGuildLevel, sizeof(byte), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &pA->Info.wszGuildNotice, sizeof(WCHAR)*GUILDNOTICE_MAX, &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->Info.ui64GuildMoney, sizeof(INT64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &OpenDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);

			for(int i=GUILDROLE_TYPE_SUBMASTER ; i<GUILDROLE_TYPE_CNT; i++) 
			{
				SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.wGuildRoleAuth[i], sizeof(int), &cbLen);
				SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.nRoleMaxItem[i], sizeof(int), &cbLen);
				SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.nRoleMaxCoin[i], sizeof(int), &cbLen);
			}
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.wGuildWareSize, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.GuildView.wWarSchduleID, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->Info.GuildView.cTeamColorCode, sizeof(byte), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.GuildView.wGuildMarkBG, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.GuildView.wGuildMarkBorder, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.GuildView.wGuildMark, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.nGuildWarFinalCount, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.nGuildWarFinalWinCount, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.iTotalGuildExp, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.iDailyStagePoint, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->Info.iDailyMissionPoint, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &pA->Info.wGuildSize, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &pA->Info.wszGuildHomePage, sizeof(WCHAR)*GUILDHISTORYTEXT_MAX, &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->Info.cGuildType, sizeof(char), &cbLen);

			CheckColumnCount(nColNo, "P_GetGuild");
			while(1)
			{			
				memset(&OpenDate, 0, sizeof(OpenDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;
				
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				pA->Info.GuildView.GuildUID.nWorldID = cWorldSetID;
				pA->Info.GuildView.GuildUID.nDBID = nGuildDBID;

				::memset(&tmOpenDate, 0, sizeof(tmOpenDate));
				tmOpenDate.tm_year	= OpenDate.year - DF_TM_YEAR_EPOCH;
				tmOpenDate.tm_mon	= OpenDate.month - 1;
				tmOpenDate.tm_mday	= OpenDate.day;
				tmOpenDate.tm_hour	= OpenDate.hour;
				tmOpenDate.tm_min	= OpenDate.minute;
				tmOpenDate.tm_sec	= OpenDate.second;
				pA->Info.OpenDate = CTimeSet::ConvertTmToTimeT64_LC(&tmOpenDate);
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildMemberList(UINT nGuildDBID, char cWorldSetID, std::vector<TGuildMember>& vGuildMember)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildMember2" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, cWorldSetID, 0, 0, 0, L"[QueryGetGuildMemberList] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildMember2(%d)}", nGuildDBID);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TGuildMember GuildMember;
			SQL_TIMESTAMP_STRUCT JoinDate, LastLogoutDate, LastGuildWareUseDate; 
			tm tmJoinDate, tmLastLogoutDate, tmLastGuildWareUseDate;

			int nColNo = 1;

			// 계정 DBID
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.nAccountDBID, sizeof(int), &cbLen);

			// 계정이름(임시)
			WCHAR wszAccountName[NAMELENMAX];
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszAccountName, sizeof(wszAccountName), &cbLen);

			// 캐릭터 DBID
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildMember.nCharacterDBID, sizeof(INT64), &cbLen);

			// 캐릭터명
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildMember.wszCharacterName, sizeof(GuildMember.wszCharacterName), &cbLen);

			// 직업코드
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildMember.nJob, sizeof(BYTE), &cbLen);

			// 캐릭터 레벨
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildMember.cLevel, sizeof(BYTE), &cbLen);

			// 마지막 로그인 시간
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &LastLogoutDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);

			// 역할코드
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildMember.btGuildRole, sizeof(BYTE), &cbLen);

			// 자기소개
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildMember.wszGuildMemberIntroduce, sizeof(GuildMember.wszGuildMemberIntroduce), &cbLen);

			// 길드원 전체 포인트
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.iTotGuildCommonPoint, sizeof(int), &cbLen);

			// 길드원 사용 포인트
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.iUseGuildCommonPoint, sizeof(int), &cbLen);
			
			// 가입일자
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &JoinDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);
		
			// 꺼낸 아이템 횟수
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.TakeWareItemCount, sizeof(int), &cbLen);

			// 인출한 금액
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.WithdrawCoin, sizeof(int), &cbLen);

			// 창고를 마지막으로 사용한 일자
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &LastGuildWareUseDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildMember2");
			while(1)
			{
				memset(&GuildMember, 0, sizeof(GuildMember));
				memset(&wszAccountName, 0, sizeof(wszAccountName));
				memset(&JoinDate, 0, sizeof(JoinDate));
				memset(&LastLogoutDate, 0, sizeof(LastLogoutDate));
				memset(&LastGuildWareUseDate, 0, sizeof(LastGuildWareUseDate));				
				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				::memset(&tmJoinDate, 0, sizeof(tmJoinDate));
				tmJoinDate.tm_year = JoinDate.year - DF_TM_YEAR_EPOCH;
				tmJoinDate.tm_mon = JoinDate.month - 1;
				tmJoinDate.tm_mday = JoinDate.day;
				tmJoinDate.tm_hour = JoinDate.hour;
				tmJoinDate.tm_min = JoinDate.minute;
				tmJoinDate.tm_sec = JoinDate.second;
				GuildMember.JoinDate = CTimeSet::ConvertTmToTimeT64_LC(&tmJoinDate);

				::memset(&tmLastLogoutDate, 0, sizeof(tmLastLogoutDate));
				tmLastLogoutDate.tm_year = LastLogoutDate.year - DF_TM_YEAR_EPOCH;
				tmLastLogoutDate.tm_mon = LastLogoutDate.month - 1;
				tmLastLogoutDate.tm_mday = LastLogoutDate.day;
				tmLastLogoutDate.tm_hour = LastLogoutDate.hour;
				tmLastLogoutDate.tm_min = LastLogoutDate.minute;
				tmLastLogoutDate.tm_sec = LastLogoutDate.second;
				GuildMember.LastLogoutDate = CTimeSet::ConvertTmToTimeT64_LC(&tmLastLogoutDate);

				::memset(&tmLastGuildWareUseDate, 0, sizeof(tmLastGuildWareUseDate));
				tmLastGuildWareUseDate.tm_year = LastGuildWareUseDate.year - DF_TM_YEAR_EPOCH;
				tmLastGuildWareUseDate.tm_mon = LastGuildWareUseDate.month - 1;
				tmLastGuildWareUseDate.tm_mday = LastGuildWareUseDate.day;
				tmLastGuildWareUseDate.tm_hour = LastGuildWareUseDate.hour;
				tmLastGuildWareUseDate.tm_min = LastGuildWareUseDate.minute;
				tmLastGuildWareUseDate.tm_sec = LastGuildWareUseDate.second;
				GuildMember.LastWareUseDate = CTimeSet::ConvertTmToTimeT64_LC(&tmLastGuildWareUseDate);

				vGuildMember.push_back(GuildMember);				
				if(vGuildMember.size() >= GUILDSIZE_MAX)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddGuildMember(TQAddGuildMember *pQ, TAAddGuildMember *pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildMember" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryAddGuildMember] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildMember(%d,%I64d,%d,N'%s',%d,?)}", 
		pQ->nGuildDBID,		// 길드 ID
		pQ->nCharacterDBID,	// 캐릭터 DBID
		pQ->cLevel,			// 캐릭터레벨
		" ",					// 디폴트 자기소개
		pQ->wGuildSize		// 최대길드원 수
		);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		SQL_TIMESTAMP_STRUCT JoinDate; 
		tm tmJoinDate;

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, &JoinDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) {
			::memset(&tmJoinDate, 0, sizeof(tmJoinDate));
			tmJoinDate.tm_year = JoinDate.year - DF_TM_YEAR_EPOCH;
			tmJoinDate.tm_mon = JoinDate.month - 1;
			tmJoinDate.tm_mday = JoinDate.day;
			tmJoinDate.tm_hour = JoinDate.hour;
			tmJoinDate.tm_min = JoinDate.minute;	
			tmJoinDate.tm_sec = JoinDate.second;
			pA->JoinDate = CTimeSet::ConvertTmToTimeT64_LC(&tmJoinDate);
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryDelGuildMember(TQDelGuildMember *pQ, TADelGuildMember *pA)
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildMember" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelGuildMember(%d,%d,%d, %d)}", 
		pQ->nGuildDBID,			// 길드 ID
		pQ->nDelCharacterDBID,	// 캐릭터 아이디
		pQ->cLevel,				// 캐릭터 레벨
		pQ->bIsExiled				// true : 강제탈퇴(추방) false : 자진탈퇴
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDismissGuild(TQDismissGuild *pQ, TADismissGuild *pA)
{
	CQueryTimeLog QueryTimeLog( "P_CloseGuild" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_CloseGuild(%d)}", pQ->nGuildDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildInfoNotice(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModNoticeContent" );

	WCHAR wszText[(GUILDHISTORYTEXT_MAX * 2) + 1];
	memset(wszText, 0, sizeof(wszText));
	ConvertQuery(pQ->Text, GUILDHISTORYTEXT_MAX, wszText, _countof(wszText));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModNoticeContent(%d,N'%s')}", 
		pQ->nGuildDBID,	// 길드 DBID
		wszText				// 공지내용
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildMemberInfoIntroduce(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildMemberSelfIntroduction" );
	
	WCHAR wszText[GUILDHISTORYTEXT_MAX*2+1];
	memset(wszText, 0, sizeof(wszText));
	ConvertQuery(pQ->Text, GUILDHISTORYTEXT_MAX, wszText, _countof(wszText));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildMemberSelfIntroduction(%d, %d,N'%s')}", pQ->nGuildDBID, pQ->nChgCharacterDBID, wszText);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildMemberInfoRole(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildMemberRoleCode" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildMemberRoleCode(%d,%d,%d,%d)}", 
		pQ->nGuildDBID,						// 길드 DBID
		pQ->nChgCharacterDBID,				// 캐릭터 ID
		pQ->cLevel,							// 캐릭터 레벨
		static_cast<BYTE>(pQ->nInt1)	// 역할코드
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildMemberInfoCommonPoint(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildPoint" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nReqAccountDBID, pQ->nReqCharacterDBID, 0, L"[QueryChangeGuildMemberInfoCommonPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildPoint(%d,%d,%I64d,%d, ?, ?)}", 
		pQ->nGuildDBID,		// 길드 ID
		pQ->nInt1,				// 길드포인트 변경치(+/-모두 가능)
		pQ->nReqCharacterDBID,// 길드포인트를 사용한 캐릭터 ID
		pQ->cLevel			// 사용한 캐릭터의 레벨
		);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nInt2, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nInt1, sizeof(int), &cbLen);
		
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryChangeGuildInfoRoleAuth(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_SetGuildMemberRole" );

	pA->nInt3 = pQ->nInt3;
	pA->nInt4 = pQ->nInt4;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetGuildMemberRole(%d,%d,%d,%d,%d)}", 
		pQ->nGuildDBID,						// 길드 DBID
		static_cast<BYTE>(pQ->nInt1),	// 직급번호
		static_cast<INT>(pQ->nInt3),			// 아이템수령제한
		static_cast<INT>(pQ->nInt4),			// 게임머니 인출제한
		static_cast<int>(pQ->nInt2)	// 권한
		);

	return CommonReturnValueQuery(m_wszQuery);
}
#if defined( PRE_ADD_GUILD_EASYSYSTEM ) 
int CDNSQLWorld::QueryChangeGuildInfoHomePage(TQChangeGuildInfo *pQ, TAChangeGuildInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildHomepage" );

	WCHAR wszText[(GUILDHISTORYTEXT_MAX * 2) + 1];
	memset(wszText, 0, sizeof(wszText));
	ConvertQuery(pQ->Text, GUILDHISTORYTEXT_MAX, wszText, _countof(wszText));

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildHomepage(%d,N'%s')}", 
		pQ->nGuildDBID,	// 길드 DBID
		wszText				// 공지내용
		);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif

int CDNSQLWorld::QueryChangeGuildMemberInfoGuildMaster(TQChangeGuildMemberInfo *pQ, TAChangeGuildMemberInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ChangeGuildMaster" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ChangeGuildMaster(%d,%I64d,%I64d, %d)}", 
		pQ->nGuildDBID,			// 길드 DBID
		pQ->nChgCharacterDBID,	// 새로운 길드마스터의 캐릭터 DBID
		pQ->nReqCharacterDBID,	// 기존 길드마스터의 캐릭터ID
		GUILDROLE_TYPE_SUBMASTER	// 기존 길드마스터의 새 직급은 길드부장!
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildHistoryList(TQGetGuildHistoryList *pQ, TAGetGuildHistoryList *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPageGuildHistory" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryGetGuildHistoryList] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageGuildHistory(%d,%d,%d,%d, ?, ?)}", 
		pQ->biIndex,						// 시작번호
		pQ->biIndex+GUILDHISTORYLIST_MAX, // 끝 번호
		1,									// 정렬기준(내림차순)
		pQ->nGuildDBID					// 길드 ID
		);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		TGuildHistory GuildHistory;
		
		tm tmRegDate;
		
		INT64 nCharacterDBID;
		char cLevel;
		BYTE btRoleCode;
		int ItemID;
		BYTE ItemLevel;
		BYTE GuildLevel;
		short GuildDepotSize;
		SQL_TIMESTAMP_STRUCT RegDate; 
		WCHAR wszCharacterName[NAMELENMAX*2];
		WCHAR wszGuildNameBefore[GUILDNAME_MAX];
		WCHAR wszGuildNameAfter[GUILDNAME_MAX];

		CDNSqlLen SqlLen;
		BYTE cHistoryType = 0;

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(int), 0, &cHistoryType, sizeof(int), SqlLen.GetNull(true,sizeof(BYTE)) );
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nTotCount, sizeof(int), &cbLen);
		
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;

			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildHistory.btHistoryType, sizeof(BYTE), &cbLen);

			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &nCharacterDBID, sizeof(__int64), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszCharacterName, sizeof(wszCharacterName), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cLevel, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &btRoleCode, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ItemID, sizeof(int), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemLevel, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildLevel, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildDepotSize, sizeof(short), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &RegDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszGuildNameBefore, sizeof(wszGuildNameBefore), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszGuildNameAfter, sizeof(wszGuildNameAfter), &cbLen);
			CheckColumnCount(nColNo, "P_GetPageGuildHistory");
			pA->nCurCount = 0;
			pA->nCount = 0;
			while(1)
			{
				memset(&GuildHistory, 0, sizeof(GuildHistory));
				memset(&RegDate, 0, sizeof(RegDate));
				memset(&wszCharacterName, 0, sizeof(wszCharacterName));
				memset(&wszGuildNameBefore, 0, sizeof(wszGuildNameBefore));
				memset(&wszGuildNameAfter, 0, sizeof(wszGuildNameAfter));
				nCharacterDBID = 0;
				cLevel = 0;
				btRoleCode = 0;
				ItemID = 0;
				ItemLevel = 0;
				GuildLevel = 0;
				GuildDepotSize = 0;				

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				// 기본적으로캐릭터이름을 Text에저장한다.
				_wcscpy(GuildHistory.Text, _countof(GuildHistory.Text), wszCharacterName, (int)wcslen(wszCharacterName));
				
				// 히스토리 타입에 맞게 데이터 변환
				switch(GuildHistory.btHistoryType)
				{
				case GUILDHISTORY_TYPE_LEVELUP:		// 길드레벨 변경
					GuildHistory.nInt1 = static_cast<int>(GuildLevel);
					break;

				case GUILDHISTORY_TYPE_ROLECHNG:	// 직급변경
					GuildHistory.nInt1 = static_cast<int>(btRoleCode);
					break;

				case GUILDHISTORY_TYPE_MEMBLEVELUP:	// 길드원 레벨변경
					GuildHistory.nInt1 = static_cast<int>(cLevel);
					break;

				case GUILDHISTORY_TYPE_ITEMENCT:	// 길드원 고강화 성공
					{
						GuildHistory.nInt1 = ItemLevel;
						GuildHistory.nInt2 = ItemID;
					}
					break;
				case GUILDHISTORY_TYPE_RENAME:		// 길드명 변경
					{
						std::wstring wGuildName;
						wGuildName.append(wszGuildNameBefore);
						wGuildName.append(L",");
						wGuildName.append(wszGuildNameAfter);

						memset(GuildHistory.Text, 0, sizeof(GuildHistory.Text));
						_wcscpy(GuildHistory.Text, _countof(GuildHistory.Text), wGuildName.c_str(), (int)wcslen(wGuildName.c_str()));
					}
					break;

				case GUILDHISTORY_TYPE_WARESIZE:	// 길드창고 사이즈 변경
					GuildHistory.nInt1 = static_cast<int>(GuildDepotSize);
					break;
				}

				::memset(&tmRegDate, 0, sizeof(tmRegDate));
				tmRegDate.tm_year = RegDate.year - DF_TM_YEAR_EPOCH;
				tmRegDate.tm_mon = RegDate.month - 1;
				tmRegDate.tm_mday = RegDate.day;
				tmRegDate.tm_hour = RegDate.hour;
				tmRegDate.tm_min = RegDate.minute;
				tmRegDate.tm_sec = RegDate.second;

				GuildHistory.RegDate = CTimeSet::ConvertTmToTimeT64_LC(&tmRegDate);

				GuildHistory.biIndex = pA->nCurCount;

				if(pA->nCurCount < GUILDHISTORYLIST_MAX) 
				{
					pA->HistoryList[pA->nCurCount] = GuildHistory;
					++pA->nCount;	
				}

				++pA->nCurCount;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWareInfo(TQGetGuildWareInfo *pQ, TAGetGuildWareInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildDepotItem" );

	memset(pA, 0, sizeof(TAGetGuildWareInfo));
	pA->nGuildDBID = pQ->nGuildDBID;
	pA->cWorldSetID = pQ->cWorldSetID;
	pA->nErrNo = ERROR_DB;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWareInfo] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	short wSlotIndex;
	int nCount = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 11;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildDepotItem(%d,%d)}", pQ->nGuildDBID, nVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nErrNo, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TItemInfo ItemInfo = {0,};
			SQL_TIMESTAMP_STRUCT MakeDate = {0,}, ExpireDate = {0,}; 

			int nColNo = 1;

			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &wSlotIndex, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &ItemInfo.Item.nSerial, sizeof(INT64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ItemInfo.Item.nItemID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &ItemInfo.Item.wCount, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &ItemInfo.Item.wDur, sizeof(USHORT), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ItemInfo.Item.nRandomSeed, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ItemInfo.Item.nCoolTime, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemInfo.Item.cLevel, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemInfo.Item.cPotential, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &ItemInfo.Item.bSoulbound, sizeof(bool), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemInfo.Item.cOption, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemInfo.Item.cSealCount, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &ItemInfo.Item.bEternity, sizeof(bool), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &ItemInfo.Item.cPotentialMoveCount, sizeof(char), &cbLen);

			CheckColumnCount(nColNo, "P_GetListGuildDepotItem");
			while(1)
			{	
				memset(&ItemInfo, 0, sizeof(ItemInfo));
				memset(&MakeDate, 0, sizeof(MakeDate));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				wSlotIndex = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					pA->nErrNo = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				
				if(nCount >= GUILD_WAREHOUSE_MAX)
				{
					g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWareInfo][GuildID:%d] ItemCount >= GUILD_WAREHOUSE_MAX\r\n", pQ->nGuildDBID);
					pA->nErrNo = ERROR_NONE;
					break;
				}

				if( nCount >= pQ->wMaxCount)
					g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWareInfo][GuildID:%d] ItemCount[%d] >= GuildWareSize[%d]\r\n", pQ->nGuildDBID, nCount, pQ->wMaxCount);

				// 인덱스 변환
				ItemInfo.cSlotIndex = (BYTE)wSlotIndex;

				pA->ItemList[nCount] = ItemInfo;

				if(!ItemInfo.Item.bEternity) 
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, pA->ItemList[nCount].Item.tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, 0);
					if (!ExpireTime.IsValid())
					{
						pA->nErrNo = ERROR_DB;
						break;
					}
				}
				else 
					pA->ItemList[nCount].Item.tExpireDate = 0;

				nCount++;

			}
		}
	}

	pA->cItemListCount = nCount;

	SQLCloseCursor(m_hstmt);
	return pA->nErrNo;
}


int CDNSQLWorld::QuerySetGuildWareInfo(TQSetGuildWareInfo *pQ, TASetGuildWareInfo *pA)
{
	CQueryTimeLog QueryTimeLog( "P_SetGuildDepotSlot" );

	pA->nRetCode = ERROR_DB;
	pA->cWorldSetID = pQ->cWorldSetID;
	pA->nGuildDBID = pQ->nGuildDBID;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QuerySetGuildWareInfo] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wIndexs;
	std::wstring wSerials;
	
	for(int i=0; i<pQ->cSlotListCount; i++)
	{
		if(i>0) 
		{
			wSerials.append(L",");
			wIndexs.append(L",");
		}

		wIndexs.append(boost::lexical_cast<std::wstring>(pQ->WareSlotList[i].cSlotIndex));
		wSerials.append(boost::lexical_cast<std::wstring>(pQ->WareSlotList[i].nSerial));
	}
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetGuildDepotSlot(%d,N'%s',N'%s')}", pQ->nGuildDBID, wSerials.c_str(), wIndexs.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildWareHistoryList(TQGetGuildWareHistory* pQ, TAGetGuildWareHistory* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPageGuildDepotHistory" );

	pA->nAccountDBID	= pQ->nAccountDBID;
	pA->nCharacterDBID	= pQ->nCharacterDBID;
	pA->nGuildDBID		= pQ->nGuildDBID;
	pA->cWorldSetID		= pQ->cWorldSetID;
	pA->nErrNo			= ERROR_DB;
	pA->nCount			= 0;
	pA->nCurrCount		= 0;
	pA->nTotalCount		= 0;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryGetGuildWareHistoryList] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageGuildDepotHistory(%d,%d,%d,%d, ?, ?, ?)}", 
		pQ->nIndex,									// 시작번호
		pQ->nIndex+GUILD_WAREHOUSE_HISTORYLIST_MAX, // 끝번호
		1,											// 정렬기준(내림차순)
		pQ->nGuildDBID								// 길드 ID
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		TGuildWareHistory WareHistory;

		tm tmRegDate;
		SQL_TIMESTAMP_STRUCT RegDate;

		INT64 nCharacterDBID;
		BYTE cItemCoinFlag;
		BYTE cItemLevel;
		short wItemCount;
		int nItemID;
		int nGuildCoin;
		INT64 nGuildCoinAfter;

		bool bConFlag = false;
		bool bOutFlag = false;

		CDNSqlLen SqlLen[2];
	
		int nPrmNo = 1;
			
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nErrNo, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &bConFlag, sizeof(bool), SqlLen[0].GetNull(true, sizeof(bool)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &bOutFlag, sizeof(bool), SqlLen[1].GetNull(true, sizeof(bool)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nTotalCount, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;

			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &nCharacterDBID, sizeof(__int64), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, WareHistory.wszCharacterName, sizeof(WareHistory.wszCharacterName), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cItemCoinFlag, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &WareHistory.cInOut, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nItemID, sizeof(int), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &cItemLevel, sizeof(BYTE), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &wItemCount, sizeof(short), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nGuildCoin, sizeof(int), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &nGuildCoinAfter, sizeof(INT64), &cbLen);
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_TYPE_TIMESTAMP, &RegDate, sizeof(SQL_TIMESTAMP_STRUCT), &cbLen);
			CheckColumnCount(nColNo, "P_GetPageGuildDepotHistory");
			while(1)
			{
				memset(&WareHistory, 0, sizeof(WareHistory));
				memset(&RegDate, 0, sizeof(RegDate));
				nCharacterDBID = 0;
				cItemCoinFlag = 0;
				nItemID = 0;
				cItemLevel = 0;
				wItemCount = 0;
				nGuildCoin = 0;
				nGuildCoinAfter = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					pA->nErrNo = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				if(cItemCoinFlag == 0) // 아이템
				{
					WareHistory.wCount = wItemCount;
					WareHistory.nContent = nItemID;

				}
				else // 코인
				{
					WareHistory.wCount = 0;
					WareHistory.nContent = nGuildCoin;
				}

				::memset(&tmRegDate, 0, sizeof(tmRegDate));
				tmRegDate.tm_year = RegDate.year - DF_TM_YEAR_EPOCH;
				tmRegDate.tm_mon = RegDate.month - 1;
				tmRegDate.tm_mday = RegDate.day;
				tmRegDate.tm_hour = RegDate.hour;
				tmRegDate.tm_min = RegDate.minute;
				tmRegDate.tm_sec = RegDate.second;

				WareHistory.EventDate = CTimeSet::ConvertTmToTimeT64_LC(&tmRegDate);

				WareHistory.nIndex = pA->nCurrCount++;

				if(pA->nCount < GUILD_WAREHOUSE_HISTORYLIST_MAX)
				{
					pA->HistoryList[pA->nCount] = WareHistory;
					pA->nCount++;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return pA->nErrNo;
}

int CDNSQLWorld::QueryMoveItemInGuildWare(TQMoveItemInGuildWare *pQ, TAMoveItemInGuildWare *pA)
{
	CQueryTimeLog QueryTimeLog( "P_SplitAndMergeItemInGuildDepot" );

	pA->nRetCode		= ERROR_DB;
	pA->nAccountDBID	= pQ->nAccountDBID;
	pA->nCharacterDBID	= pQ->nCharacterDBID;
	pA->cWorldSetID		= pQ->cWorldSetID;
	pA->nGuildDBID		= pQ->nGuildDBID;
	pA->SrcItem			= pQ->SrcItem;
	pA->DestItem		= pQ->DestItem;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryMoveGuildWareToInven] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	CDNSqlLen SqlLen[2];
	int sqlparam = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SplitAndMergeItemInGuildDepot2(%d,%d,%I64d,?,?,%d,%d,%d,%d,?,?)}", 
		pQ->nGuildDBID,				// 길드 ID
		pQ->nItemID,				// 이동할 아이템 ID
		pQ->biItemSerial,			// 이동할 아이템의 고유번호
		pQ->wItemCount,				// @inbItemSerial 값에 해당하는 아이템의 변경 전 총 수량
		pQ->wSplitItemCount,		// @inbNewItemSerial 값에 해당하는 아이템 수량
		pQ->wMergeItemCount,		// @inbMergeTargetItemSerial 값에 해당하는 아이템의 변경 전 총 수량
		pQ->nMapIndex				// 맵인덱스
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nNo = 1;
		short wNewItemCount = 0, wMergeTargetNewItemCount = 0;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biNewItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biNewItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biMergeTargetItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biMergeTargetItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &wNewItemCount, sizeof(short), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &wMergeTargetNewItemCount, sizeof(short), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryMoveInvenToGuildWare(TQMoveInvenToGuildWare *pQ, TAMoveInvenToGuildWare *pA)
{
	CQueryTimeLog QueryTimeLog( "P_MoveItemIntoGuildDepot" );

	pA->nRetCode		= ERROR_DB;
	pA->nAccountDBID	= pQ->nAccountDBID;
	pA->nCharacterDBID	= pQ->nCharacterDBID;
	pA->cWorldSetID		= pQ->cWorldSetID;
	pA->nGuildDBID		= pQ->nGuildDBID;
	pA->InvenItem		= pQ->InvenItem;
	pA->GuildWareItem	= pQ->GuildWareItem;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryMoveGuildWareToInven] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	CDNSqlLen SqlLen[2];
	int sqlparam = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveItemIntoGuildDepot2(%I64d,%d,%d,%I64d,?,?,%d,%d,%d,%d)}", 
		pQ->nCharacterDBID,			// 캐릭터 ID
		pQ->nGuildDBID,				// 길드 ID
		pQ->nItemID,				// 이동할 아이템 ID
		pQ->biItemSerial,			// 이동할 아이템의 고유번호
		pQ->wItemCount,
		pQ->wMovingItemCount,
		pQ->wMergeItemCount,
		pQ->nMapIndex
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biNewItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biNewItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biMergeTargetItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biMergeTargetItemSerial <= 0, sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryMoveGuildWareToInven(TQMoveGuildWareToInven *pQ, TAMoveGuildWareToInven *pA)
{
	CQueryTimeLog QueryTimeLog( "P_MoveItemFromGuildDepot" );

	pA->nRetCode		= ERROR_DB;
	pA->nAccountDBID	= pQ->nAccountDBID;
	pA->nCharacterDBID	= pQ->nCharacterDBID;
	pA->cWorldSetID		= pQ->cWorldSetID;
	pA->nGuildDBID		= pQ->nGuildDBID;
	pA->GuildWareItem	= pQ->GuildWareItem;
	pA->InvenItem		= pQ->InvenItem;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryMoveGuildWareToInven] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	CDNSqlLen SqlLen[3];
	int sqlparam = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_MoveItemFromGuildDepot2(%I64d,%d,%d,%d,%I64d,?,?,%d,%d,%d,?,%d,?,?,?)}", 
		pQ->nCharacterDBID,			// 캐릭터 ID
		DBDNWorldDef::ItemLocation::Inventory,	// 인벤토리
		pQ->nGuildDBID,				// 길드 ID
		pQ->nItemID,				// 이동할 아이템 ID
		pQ->biItemSerial,			// 이동할 아이템의 고유번호
		pQ->wItemCount,				// @inbItemSerial 값에 해당하는 아이템의 변경 전 총 수량
		pQ->wMovingItemCount,		// 옮기는 아이템의 수량
		pQ->wMergeItemCount,		// @inbMergeItemSerial 값에 해당하는 아이템의 변경 전 총 수량
		pQ->nMapIndex
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {

		TIMESTAMP_STRUCT UseDate = {0,};
		short nGuildDepotItemCount = 0;

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biNewItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biNewItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biMergeTargetItemSerial, sizeof(INT64), SqlLen[sqlparam++].GetNull(pQ->biMergeTargetItemSerial <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->nMaxDailyTakeItemCount, sizeof(int), SqlLen[sqlparam++].GetNull(pQ->nMaxDailyTakeItemCount < 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nDailyTakeItemCount, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &UseDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(short), 0, &nGuildDepotItemCount, sizeof(short), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			CTimeParamSet UseTime(&QueryTimeLog, pA->tUseDate, UseDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID);
			if (!UseTime.IsValid()) return ERROR_DB;
		}
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryModGuildCoin(TQGuildWareHouseCoin *pQ, TAGuildWareHouseCoin *pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildCoin" );

	pA->nRetCode		= ERROR_DB;
	pA->nAccountDBID	= pQ->nAccountDBID;
	pA->nCharacterDBID	= pQ->nCharacterDBID;
	pA->cWorldSetID		= pQ->cWorldSetID;
	pA->nGuildDBID		= pQ->nGuildDBID;
	pA->nMoveCoin		= pQ->nMoveCoin;
	pA->bInOutFlag      = pQ->bInOutFlag;
	pA->cMoveType		= pQ->cMoveType;

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryModGuildCoin] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;
	CDNSqlLen SqlLen[2];
	int sqlparam = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildCoin(%d,%I64d,%d,%I64d,%d,'%S',?,?)}", 
		pQ->nGuildDBID,				// 길드 ID
		pQ->nCharacterDBID,			// 캐릭터 ID
		pQ->bInOutFlag,				// 0=입금, 1=출금
		pQ->nMoveCoin,				// 게임머니
		pQ->nMapIndex,				// 맵ID
		pQ->szIP					// IP주소
		);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {

		TIMESTAMP_STRUCT UseDate = {0,};

		int nNo = 1;

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biTotalCoin, sizeof(INT64), &cbLen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &UseDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
		
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			CTimeParamSet UseTime(&QueryTimeLog, pA->tUseDate, UseDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID);
			if (!UseTime.IsValid()) return ERROR_DB;
		}
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryAddGuildHistory(TQAddGuildHistory *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildHistory" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildHistory(%d,%d,%I64d,%d,%d)}", pQ->nGuildDBID, pQ->History.btHistoryType, pQ->History.biInt64, pQ->History.nInt2, pQ->History.nInt1);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildWareSize(TQChangeGuildWareSize * pQ, TAChangeGuildWareSize * pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildDepotSize" );

	if(pQ->wStorageSize > GUILD_WAREHOUSE_MAX)
		pQ->wStorageSize = GUILD_WAREHOUSE_MAX;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildDepotSize(%d,%d)}", 
		pQ->nGuildDBID,				// 길드 ID
		pQ->wStorageSize			// 길드 창고 크기
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryChangeGuildMark(TQChangeGuildMark * pQ, TAChangeGuildMark * pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildMark" );

	pA->nRetCode		= ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildMark(%d,%d,%d,%d)}", 
		pQ->nGuildDBID,			// 길드 ID
		pQ->wGuildMarkBG,		// 길드마크 배경
		pQ->wGuildMarkBorder,	// 길드마크 테두리
		pQ->wGuildMark			// 길드마크 문양
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModGuildName(TQChangeGuildName* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildName" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModGuildName(%d,N'%s')}", pQ->nGuildDBID, pQ->wszGuildName );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryEnrollGuildWar(TQEnrollGuildWar *pQ, TAEnrollGuildWar *pA)
{
	if(CheckConnect() < 0)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryEnrollGuildWar] Check Connect Fail\r\n");
		pA->nRetCode = ERROR_DB;
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cbLen;
	int nResult = ERROR_DB;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildByGuildWarApply(%d,%d, 0)}", pQ->wScheduleID, pQ->nGuildDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cTeamColorCode, sizeof(BYTE), &cbLen);
			CheckColumnCount(nColNo, "P_ModGuildByGuildWarApply");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarSchedule(TQGetGuildWarSchedule *pQ, TAGetGuildWarSchedule *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildWarSchedule" );

	if(CheckConnect() < 0)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarSchedule] Check Connect Fail\r\n");
		pA->nRetCode = ERROR_DB;
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cbLen;	

	pA->wScheduleID = 0;
	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildWarSchedule(?, ?,?, ?,?, ?,?, ?, ?,?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		TIMESTAMP_STRUCT tsWarBeginDate[GUILDWAR_STEP_END];
		memset(tsWarBeginDate, 0, sizeof(tsWarBeginDate));
		TIMESTAMP_STRUCT tsWarEndDate[GUILDWAR_STEP_END];
		memset(tsWarEndDate, 0, sizeof(tsWarEndDate));
		TIMESTAMP_STRUCT tsRegistDate = {0,};
		TIMESTAMP_STRUCT tsRewardExpireDate = {0,};
		int nNo = 1;
		// 결과
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);

		// 길드전 차수
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &pA->wScheduleID, sizeof(short), &cbLen);
	
		// 신청기간 시작일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarBeginDate[GUILDWAR_STEP_PREPARATION], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 신청기간 종료일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarEndDate[GUILDWAR_STEP_PREPARATION], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 예선기간 시작일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarBeginDate[GUILDWAR_STEP_TRIAL], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 예선기간 종료일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarEndDate[GUILDWAR_STEP_TRIAL], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 보상기간 시작일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarBeginDate[GUILDWAR_STEP_REWARD], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 보상기간 종료일
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsWarEndDate[GUILDWAR_STEP_REWARD], sizeof(TIMESTAMP_STRUCT), &cbLen);

		// 패배팀 가중치
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &pA->wWinersWeightRate, sizeof(short), &cbLen);

		// 본선 진행여부
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bFinalProgress, sizeof(bool), &cbLen);

		// 보상 만료일 지정
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &tsRewardExpireDate, sizeof(TIMESTAMP_STRUCT), &cbLen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{				
#if defined(PRE_FIX_75807)
			// 보상 완료만 들어가 있을수 있다.
			if( tsRewardExpireDate.year > 0)
			{
				CTimeParamSet ExpireTime(&QueryTimeLog, pA->tRewardExpireDate, tsRewardExpireDate, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!ExpireTime.IsValid()) return ERROR_DB;
			}			
#endif
			if( pA->wScheduleID == 0 ) // 요건 스케쥴이 안들어가 있는 상태..
				return pA->nRetCode;
			CTimeParamSet PreBeginTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_PREPARATION].tBeginTime, tsWarBeginDate[GUILDWAR_STEP_PREPARATION], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!PreBeginTime.IsValid()) return ERROR_DB;

			CTimeParamSet PreEndTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_PREPARATION].tEndTime, tsWarEndDate[GUILDWAR_STEP_PREPARATION], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!PreEndTime.IsValid()) return ERROR_DB;

			CTimeParamSet TrialBeginTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_TRIAL].tBeginTime, tsWarBeginDate[GUILDWAR_STEP_TRIAL], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!TrialBeginTime.IsValid()) return ERROR_DB;

			CTimeParamSet TrialEndTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_TRIAL].tEndTime, tsWarEndDate[GUILDWAR_STEP_TRIAL], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!TrialEndTime.IsValid()) return ERROR_DB;

			CTimeParamSet RewardBeginTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_REWARD].tBeginTime, tsWarBeginDate[GUILDWAR_STEP_REWARD], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!RewardBeginTime.IsValid()) return ERROR_DB;

			CTimeParamSet RewardEndTime(&QueryTimeLog, pA->EventInfo[GUILDWAR_STEP_REWARD].tEndTime, tsWarEndDate[GUILDWAR_STEP_REWARD], pQ->cWorldSetID, pQ->nAccountDBID);
			if (!RewardEndTime.IsValid()) return ERROR_DB;

#if defined(PRE_FIX_75807)
#else
			CTimeParamSet ExpireTime(&QueryTimeLog, pA->tRewardExpireDate, tsRewardExpireDate, pQ->cWorldSetID, pQ->nAccountDBID);
			if (!ExpireTime.IsValid()) return ERROR_DB;
#endif
		}
	}

	return pA->nRetCode;
}
int CDNSQLWorld::QueryGetGuildWarFinalSchedule(TQGetGuildWarFinalSchedule *pQ, TAGetGuildWarFinalSchedule *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarFinalSchedule" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarFinalSchedule] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarFinalSchedule(%d)}",pQ->wScheduleID);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		

		RetCode = SQLExecute(m_hstmt);
		int nIndex = GUILDWAR_FINALPART_FINAL;
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			char cMatchTypeCode = 0;			
			TIMESTAMP_STRUCT tsWarBeginDate;
			TIMESTAMP_STRUCT tsWarEndDate;

			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &cMatchTypeCode, sizeof(BYTE), &cbLen );
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TIMESTAMP,	&tsWarBeginDate, sizeof(TIMESTAMP_STRUCT), &cbLen );	
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TIMESTAMP,	&tsWarEndDate, sizeof(TIMESTAMP_STRUCT), &cbLen );		
			CheckColumnCount(nColNo, "P_GetListGuildWarFinalSchedule");
			while(1)
			{				
				memset(&tsWarBeginDate, 0, sizeof(tsWarBeginDate));
				memset(&tsWarEndDate, 0, sizeof(tsWarEndDate));
				cMatchTypeCode = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;				
				if( nIndex < GUILDWAR_FINALPART_MAX)
				{
					CTimeParamSet BeginTime(&QueryTimeLog, pA->GuildWarFinalSchedule[nIndex].tBeginTime, tsWarBeginDate, pQ->cWorldSetID, pQ->nAccountDBID);
					if (!BeginTime.IsValid())
					{
						nResult = ERROR_DB;
						break;
					}
					CTimeParamSet EndTime(&QueryTimeLog, pA->GuildWarFinalSchedule[nIndex].tEndTime, tsWarEndDate, pQ->cWorldSetID, pQ->nAccountDBID);
					if (!EndTime.IsValid())
					{
						nResult = ERROR_DB;
						break;
					}
				}				
				++nIndex;
				//if( nIndex >= GUILDWAR_FINALPART_MAX)
				//	break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddGuildWarPoint(TQAddGuildWarPointRecodrd *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarOpeningPointRecord" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarOpeningPointRecord(%I64d,%d,%d,%d,%d)}", pQ->biCharacterDBID, 
		pQ->nMissionID, pQ->nOpeningPoint, pQ->nGuildDBID, pQ->cTeamColorCode );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildWarPoint(TQGetGuildWarPoint *pQ, TAGetGuildWarPoint *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildWarOpeningPointRecord" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPoint] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildWarOpeningPointRecord('%c',?,?,?,?,?)}", pQ->cQueryType);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biDBID, sizeof(INT64), SqlLen.GetNull(pQ->biDBID <= 0, sizeof(INT64)));
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCharOpeningPoint, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nGuildOpeningPoint, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 5, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nBlueTeamPoint, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 6, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRedTeamPoint, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarPointPartTotal(TQGetGuildWarPointPartTotal *pQ, TAGetGuildWarPointPartTotal *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarOpeningPointTotal" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPointPartTotal] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarOpeningPointTotal(?)}");

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biCharacterDBID, sizeof(INT64), SqlLen.GetNull(pQ->biCharacterDBID <= 0, sizeof(INT64)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SGuildWarPointPartTotal GuildWarPointPartTotal;
			BYTE cTypeCode = 0;

			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointPartTotal.wRanking, sizeof(short), &cbLen );
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &cTypeCode, sizeof(BYTE), &cbLen );
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildWarPointPartTotal.biCharacterDBID, sizeof(__int64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointPartTotal.wszCharName, sizeof(GuildWarPointPartTotal.wszCharName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointPartTotal.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointPartTotal.wszGuildName, sizeof(GuildWarPointPartTotal.wszGuildName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointPartTotal.nPoint, sizeof(int), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarOpeningPointTotal");
			while(1)
			{
				memset(&GuildWarPointPartTotal, 0, sizeof(GuildWarPointPartTotal));
				cTypeCode = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				if( cTypeCode < GUILDWAR_RANKINGTYPE_MAX)
				memcpy(&pA->GuildWarPointPartTotal[cTypeCode], &GuildWarPointPartTotal, sizeof(GuildWarPointPartTotal));
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarPointGuildPartTotal(TQGetGuildWarPointGuildPartTotal *pQ, TAGetGuildWarPointGuildPartTotal *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarOpeningPointGuildTotalByMission" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPointGuildPartTotal] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 1;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarOpeningPointGuildTotalByMission(?)}");


	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->nGuildDBID, sizeof(int), SqlLen.GetNull(pQ->nGuildDBID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		//int nIndex = 0;
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SGuildWarPointPartTotal GuildWarPointPartTotal;
			BYTE cTypeCode = 0;

			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointPartTotal.wRanking, sizeof(short), &cbLen );
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &cTypeCode, sizeof(BYTE), &cbLen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointPartTotal.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointPartTotal.wszGuildName, sizeof(GuildWarPointPartTotal.wszGuildName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildWarPointPartTotal.biCharacterDBID, sizeof(__int64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointPartTotal.wszCharName, sizeof(GuildWarPointPartTotal.wszCharName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointPartTotal.nPoint, sizeof(int), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarOpeningPointGuildTotalByMission");
			while(1)
			{
				memset(&GuildWarPointPartTotal, 0, sizeof(GuildWarPointPartTotal));
				cTypeCode = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				if( cTypeCode < GUILDWAR_RANKINGTYPE_MAX)
				memcpy(&pA->GuildWarPointPartTotal[cTypeCode], &GuildWarPointPartTotal, sizeof(GuildWarPointPartTotal));
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarPointDaily(char cWorldSetID, UINT nAccountDBID, TAGetGuildWarPointDaily *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarOpeningPointDailyTotal" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, cWorldSetID, nAccountDBID, 0, 0, L"[QueryGetGuildWarPointDaily] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarOpeningPointDailyTotal}");

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		

		RetCode = SQLExecute(m_hstmt);
		int nIndex = 0;
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			TIMESTAMP_STRUCT tsTemp = {0,};
			WCHAR wszGuildName[GUILDNAME_MAX];	// 길드 이름
			SGuildWarPointDaily GuildWarPointDaily;

			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &tsTemp, sizeof(TIMESTAMP_STRUCT), &cbLen );			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointDaily.nTotalPoint, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildWarPointDaily.biCharacterDBID, sizeof(__int64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointDaily.wszCharName, sizeof(GuildWarPointDaily.wszCharName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointDaily.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszGuildName, sizeof(wszGuildName), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarOpeningPointDailyTotal");
			while(1)
			{
				memset(&GuildWarPointDaily, 0, sizeof(GuildWarPointDaily));
				memset(&tsTemp, 0, sizeof(tsTemp));
				memset(&wszGuildName, 0, sizeof(wszGuildName));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				if( nIndex < GUILDWAR_DAILY_AWARD_MAX )
					memcpy(&pA->sGuildWarPointDaily[nIndex], &GuildWarPointDaily, sizeof(GuildWarPointDaily));
				++nIndex;
				//if( nIndex >= GUILDWAR_DAILY_AWARD_MAX)
				//	break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;

}

int CDNSQLWorld::QueryGetGuildWarPointGuildTotal(TQGetGuildWarPointGuildTotal *pQ, TAGetGuildWarPointGuildTotal *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarOpeningPointGuildTotal" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPointGuildTotal] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 1;
	nVersion = 2;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarOpeningPointGuildTotal(%d, ?, %d)}",pQ->wScheduleID, nVersion);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	int nIndex = 0;
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pQ->nGuildDBID, sizeof(int), SqlLen.GetNull(pQ->nGuildDBID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SGuildWarPointGuildTotal GuildWarPointGuildTotal;

			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointGuildTotal.wRanking, sizeof(short), &cbLen );			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointGuildTotal.wszGuildName, sizeof(GuildWarPointGuildTotal.wszGuildName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildWarPointGuildTotal.biMasterDBID, sizeof(__int64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointGuildTotal.wszMasterName, sizeof(GuildWarPointGuildTotal.wszMasterName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &GuildWarPointGuildTotal.wGuildLevel, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointGuildTotal.wGuildMemberCount, sizeof(short), &cbLen );			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nOpeningPointSum, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nMaxMemberCount, sizeof(int), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarOpeningPointGuildTotal");
			while(1)
			{
				memset(&GuildWarPointGuildTotal, 0, sizeof(GuildWarPointGuildTotal));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				if( nIndex < GUILDWAR_FINALS_TEAM_MAX)
				memcpy(&pA->sGuildWarPointGuildTotal[nIndex], &GuildWarPointGuildTotal, sizeof(GuildWarPointGuildTotal));
				++nIndex;
				//if( nIndex >= GUILDWAR_FINALS_TEAM_MAX)
				//	break;
			}
		}
	}
	pA->nTotalCount = nIndex;
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarPointRunningTotal(TQGetGuildWarPointRunningTotal* pQ, TAGetGuildWarPointRunningTotal* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarOpeningPointGuildRunningTotal" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPointRunningTotal] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;
	CDNSqlLen SqlLen;

	int nVersion = 1;
	int nGuildID = 0;
	nVersion = 2;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarOpeningPointGuildRunningTotal(%d,?,%d)}", GUILDWAR_TRIAL_POINT_TEAM_MAX, nVersion); 

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	int nIndex = 0;
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGuildID, sizeof(int), SqlLen.GetNull(nGuildID <= 0, sizeof(int)));

		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SGuildWarPointGuildTotal GuildWarPointGuildTotal;

			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointGuildTotal.wRanking, sizeof(short), &cbLen );			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointGuildTotal.wszGuildName, sizeof(GuildWarPointGuildTotal.wszGuildName), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildWarPointGuildTotal.biMasterDBID, sizeof(__int64), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildWarPointGuildTotal.wszMasterName, sizeof(GuildWarPointGuildTotal.wszMasterName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &GuildWarPointGuildTotal.wGuildLevel, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT,	&GuildWarPointGuildTotal.wGuildMemberCount, sizeof(short), &cbLen );			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nOpeningPointSum, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildWarPointGuildTotal.nMaxMemberCount, sizeof(int), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarOpeningPointGuildRunningTotal");
			while(1)
			{
				memset(&GuildWarPointGuildTotal, 0, sizeof(GuildWarPointGuildTotal));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				if( nIndex < GUILDWAR_TRIAL_POINT_TEAM_MAX)
					memcpy(&pA->sGuildWarPointGuildTotal[nIndex], &GuildWarPointGuildTotal, sizeof(GuildWarPointGuildTotal));
				++nIndex;				
			}
		}
	}
	pA->nTotalCount = nIndex;
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarFinalRewardPoint(TAGetGuildWarPointFinalRewards* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarFinalResults" );	

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, L"[QueryGetGuildWarFinalRewardPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarFinalResults(%d,%d)}",0,0); // 0,0이면 최종 16강 길드 포인트 조회임

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	int nIndex = 0;
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		

		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;

			UINT nGuildDBID = 0;
			int nGuildPoint = 0;

			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nGuildDBID, sizeof(int), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nGuildPoint, sizeof(int), &cbLen);	
			CheckColumnCount(nColNo, "P_GetListGuildWarFinalResults");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)					
					break;				

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				if( nIndex < GUILDWAR_FINALS_TEAM_MAX)
				{
					pA->nGuildDBID[nIndex] = nGuildDBID;
					pA->nGuildPoint[nIndex] = nGuildPoint;
				}					
				++nIndex;			
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}


int CDNSQLWorld::QueryAddGuildWarFinalMatchList(TQAddGuildWarFinalMatchList *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarFinalResultsMatchList" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	std::string GuildIDStr, GuildSeqStr;

	for(int i=0; i<GUILDWAR_FINALS_TEAM_MAX; ++i)
	{		
		if( !pQ->nGuildDBID[i])
			continue;
		
		if( !GuildIDStr.empty() )
			GuildIDStr.append(",");		
		if( !GuildSeqStr.empty() )
			GuildSeqStr.append(",");
		GuildIDStr.append(boost::lexical_cast<std::string>(pQ->nGuildDBID[i]));
		GuildSeqStr.append(boost::lexical_cast<std::string>(i+1));				
	}
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarFinalResultsMatchList(%d,'%S','%S')}", pQ->wScheduleID, GuildIDStr.c_str(), GuildSeqStr.c_str());		

	return CommonReturnValueQuery(m_wszQuery);

}

int CDNSQLWorld::QueryAddGuildWarPopularityVote(TQAddGuildWarPopularityVote *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarPopularityVote" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarPopularityVote(%I64d,%d)}", pQ->biCharacterDBID, pQ->nGuildDBID);		
	return CommonReturnValueQuery(m_wszQuery);

}

int CDNSQLWorld::QueryGetGuildWarPopularityVote(TQGetGuildWarPopularityVote *pQ, TAGetGuildWarPopularityVote *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarPopularityVote" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPopularityVote] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cbLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarPopularityVote(?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen SqlLen;
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biCharacterDBID, sizeof(INT64), SqlLen.GetNull(pQ->biCharacterDBID <= 0, sizeof(INT64)));
		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;		
			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->wszGuildName, sizeof(pA->wszGuildName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nPopularityVoteCount, sizeof(int), &cbLen);			
			CheckColumnCount(nColNo, "P_GetListGuildWarPopularityVote");
			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				nResult = ERROR_NONE;
				return nResult;
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}	
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddGuildWarFinalResults(TQAddGuildWarFinalResults *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarFinalResults" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarFinalResults(%d,%d,%d,%d,%d)}", pQ->nGuildDBID, pQ->wScheduleID, pQ->cMatchSequence, pQ->cMatchTypeCode, pQ->cMatchResultCode);		
	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildWarFinalResults(TQGetGuildWarFinalResults *pQ, TAGetGuildWarFinalResults *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarFinalResults" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QUeryGetGuildWarFinalResults] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarFinalResults(%d,%d)}",pQ->wScheduleID,1);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	int nIndex = 0;
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		

		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SGuildFinalResultInfo GuildFinalResultinfo;		
			
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildFinalResultinfo.nGuildDBID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildFinalResultinfo.wszGuildName, sizeof(GuildFinalResultinfo.wszGuildName), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &GuildFinalResultinfo.cMatchSequence, sizeof(BYTE), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &GuildFinalResultinfo.cMatchTypeCode, sizeof(BYTE), &cbLen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &GuildFinalResultinfo.cMatchResultCode, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildFinalResultinfo.wGuildMark, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildFinalResultinfo.wGuildMarkBG, sizeof(short), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildFinalResultinfo.wGuildMarkBorder, sizeof(short), &cbLen);
			CheckColumnCount(nColNo, "P_GetListGuildWarFinalResults");
			while(1)
			{
				memset(&GuildFinalResultinfo, 0, sizeof(GuildFinalResultinfo));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO))
					break;
				//if( GuildFinalResultinfo.cMatchTypeCode == 0) //요건 16강..
				//	GuildFinalResultinfo.cMatchResultCode = 16;
				if( nIndex < GUILDWAR_FINALS_TEAM_MAX)
				memcpy(&pA->GuildFinalResultInfo[nIndex], &GuildFinalResultinfo, sizeof(GuildFinalResultinfo));
				++nIndex;
				//if( nIndex >= GUILDWAR_FINALS_TEAM_MAX)
				//	break;
			}
		}
	}
	
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetGuildWarPreWinGuild(TQGetGuildWarPreWinGuild *pQ, TAGetGuildWarPreWinGuild *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListGuildWarFinalResults" );

	if(CheckConnect() < 0) {
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarPreWinGuild] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarFinalResults(%d,%d)}",0,1); // 스케쥴 ID가 0이면 마지막 최종 차수의 우승 길드 정보를 준다.

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	int nIndex = 0;
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);		

		RetCode = SQLExecute(m_hstmt);		
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SSHORT, &pA->wScheduleID, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nGuildDBID, sizeof(int), &cbLen);		
			CheckColumnCount(nColNo, "P_GetListGuildWarFinalResults");
			RetCode = SQLFetch(m_hstmt);
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddGuildWarRewarForCharacter(TQAddGuildWarRewardCharacter *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarRewardForCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarRewardForCharacter(%d,%I64d,%d)}", pQ->wScheduleID, pQ->biCharacterDBID, pQ->cRewardType+1 );

	return CommonReturnValueQuery(m_wszQuery);

}

int CDNSQLWorld::QueryGetGuildWarRewarForCharacter(TQGetGuildWarRewardCharacter *pQ, TAGetGuildWarRewardCharacter *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildWarRewardForCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[P_GetGuildWarRewardForCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB, sqlparam = 0;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildWarRewardForCharacter(%d,%I64d,?,?,?)}", pQ->wScheduleID, pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_CHAR_PRESENT], sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_CHAR_MAIL], sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_CHAR_POINT], sizeof(bool), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryAddGuildWarRewarForGuild(TQAddGuildWarRewardGuild *pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildWarRewardForGuild" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildWarRewardForGuild(%d,%d,%d)}", pQ->wScheduleID, pQ->nGuildDBID, pQ->cRewardType+1 );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildWarRewarForGuild(TQGetGuildWarRewardGuild *pQ, TAGetGuildWarRewardGuild *pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildWarRewardForGuild" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetGuildWarRewarForGuild] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB, sqlparam = 0;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildWarRewardForGuild(%d,%d,?,?,?,?,?)}", pQ->wScheduleID, pQ->nGuildDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_GUILD_SKILL], sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_GUILD_PRESENT], sizeof(bool), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_GUILD_MAIL], sizeof(bool), &cblen);		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_GUILD_TRIAL_POINT], sizeof(bool), &cblen);		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_BIT, SQL_BIT, sizeof(bool), 0, &pA->bRewardResults[GUILDWAR_REWARD_GUILD_FINAL_POINT], sizeof(bool), &cblen);		

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryAddGuildWarPointQueue(TQAddGuildWarPointQueue* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_SendGuildWarRewardPoint" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SendGuildWarRewardPoint(%d,%I64d,0,'%s')}", 
		pQ->nGuildDBID, pQ->biAddPoint, pQ->wszIP);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddGuildWarSystemMailQueue(TQSendSystemMail* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_SendGuildWarRewardMail" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddGuildWarSystemMailQueue] Check Connect Fail\r\n");
		return ERROR_DB;
	}	

	WCHAR wszTitle[MAILTITLELENMAX * 2 + 1] = {0,}, wszText[MAILTEXTLENMAX * 2 + 1] = {0,};
	ConvertQuery(pQ->wszSubject, MAILTITLELENMAX, wszTitle, _countof(wszTitle));
	ConvertQuery(pQ->wszContent, MAILTEXTLENMAX, wszText, _countof(wszText));

	if(pQ->cAttachItemCount > MAILATTACHITEMMAX) pQ->cAttachItemCount = MAILATTACHITEMMAX;
	TItem sAttachItem[MAILATTACHITEMMAX] = {0,};
	memcpy_s( sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount), pQ->sAttachItem, sizeof(TItem)*(pQ->cAttachItemCount) );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf( m_wszQuery, L"{?=CALL dbo.P_SendGuildWarRewardMail(?,?,N'%s',N'%s',N'%s',?,? \
						   ,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,? \
						   ,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,? \
						   ,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,? \
						   ,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,? \
						   ,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,? \
						   ,%d)}"
						   , pQ->wszSenderName, wszTitle, wszText
						   , sAttachItem[0].nItemID, sAttachItem[0].wCount, (short)sAttachItem[0].wDur, sAttachItem[0].nRandomSeed, sAttachItem[0].cLevel, sAttachItem[0].cPotential, sAttachItem[0].bSoulbound?1:0, sAttachItem[0].cSealCount, sAttachItem[0].cOption, sAttachItem[0].bEternity?1:0 
						   , sAttachItem[1].nItemID, sAttachItem[1].wCount, (short)sAttachItem[1].wDur, sAttachItem[1].nRandomSeed, sAttachItem[1].cLevel, sAttachItem[1].cPotential, sAttachItem[1].bSoulbound?1:0, sAttachItem[1].cSealCount, sAttachItem[1].cOption, sAttachItem[1].bEternity?1:0
						   , sAttachItem[2].nItemID, sAttachItem[2].wCount, (short)sAttachItem[2].wDur, sAttachItem[2].nRandomSeed, sAttachItem[2].cLevel, sAttachItem[2].cPotential, sAttachItem[2].bSoulbound?1:0, sAttachItem[2].cSealCount, sAttachItem[2].cOption, sAttachItem[2].bEternity?1:0
						   , sAttachItem[3].nItemID, sAttachItem[3].wCount, (short)sAttachItem[3].wDur, sAttachItem[3].nRandomSeed, sAttachItem[3].cLevel, sAttachItem[3].cPotential, sAttachItem[3].bSoulbound?1:0, sAttachItem[3].cSealCount, sAttachItem[3].cOption, sAttachItem[3].bEternity?1:0
						   , sAttachItem[4].nItemID, sAttachItem[4].wCount, (short)sAttachItem[4].wDur, sAttachItem[4].nRandomSeed, sAttachItem[4].cLevel, sAttachItem[4].cPotential, sAttachItem[4].bSoulbound?1:0, sAttachItem[4].cSealCount, sAttachItem[4].cOption, sAttachItem[4].bEternity?1:0						   
						   , pQ->bNewFlag);

	int nResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nPrmNo = 1;
		CDNSqlLen aSqlLen1;
		CDNSqlLen aSqlLen2;
		CDNSqlLen aSqlLen3;
		CDNSqlLen aSqlLen4;
		CDNSqlLen aSqlLen501[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen502[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen503[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen504[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen505[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen506[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen507[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen508[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen509[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen510[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen511[MAILATTACHITEMMAX];
		CDNSqlLen aSqlLen512[MAILATTACHITEMMAX];				
		
		// 강제 형변환
		int nGuildDBID = (int)pQ->biReceiverCharacterDBID;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nGuildDBID, sizeof(int), aSqlLen1.Get(sizeof(int)));		
		TIMESTAMP_STRUCT* pTempDate = NULL;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, pTempDate, sizeof(TIMESTAMP_STRUCT), aSqlLen2.GetNull(true, SQL_TIMESTAMP_LEN));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biAttachCoin, sizeof(INT64), aSqlLen3.Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, pTempDate, sizeof(TIMESTAMP_STRUCT), aSqlLen4.GetNull(true, SQL_TIMESTAMP_LEN));
		for(int iIndex = 0 ; MAILATTACHITEMMAX > iIndex ; ++iIndex)
		{
			SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &sAttachItem[iIndex].nLifespan, sizeof(int), aSqlLen512[iIndex].Get(sizeof(int)));
		}		

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return nResult;
}

int CDNSQLWorld::QueryAddJobReserve(TQAddJobReserve* pQ, TAAddJobReserve* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddJobReserve" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddJobReserve] Check Connect Fail\r\n");
		return ERROR_DB;
	}
	if( pQ->cJobType >= DBJOB_SYSTEM_MAX )
		return ERROR_DB;

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddJobReserve(N'%s',?)}", g_szDBJobSystem[pQ->cJobType]);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nJobSeq, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryGetJobReserve(TQGetJobReserve* pQ, TAGetJobReserve* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetJobReserve" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetJobReserve] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetJobReserve(%d,?)}", pQ->nJobSeq);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TINYINT, SQL_TINYINT, sizeof(char), 0, &pA->cJobStatus, sizeof(char), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryGetListGuildWarItemTradeRecord(INT64 characterDBID, OUT TAGetListGuildWarItemTradeRecord& record)
{
	CQueryTimeLog QueryTimeLog("P_GetListGuildWarItemTradeRecord");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, characterDBID, 0, L"[QueryGetListGuildWarItemTradeRecord] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListGuildWarItemTradeRecord(?)}");

	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen;

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &result, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &characterDBID, sizeof(INT64), sqlLen.Get(sizeof(INT64)));

		retCode = SQLExecute(m_hstmt);
		if (CheckRetCode(retCode, L"SQLExecute") == ERROR_NONE)
		{
			int itemID;
			int count;

			int colNo = 1;
			SQLBindCol(m_hstmt, colNo++, SQL_INTEGER, &itemID, sizeof(int), &len);
			SQLBindCol(m_hstmt, colNo++, SQL_INTEGER, &count, sizeof(int), &len);
			CheckColumnCount(colNo, "P_GetListGuildWarItemTradeRecord");
			int itemIndex = 0;
			while (1)
			{
				retCode = SQLFetch(m_hstmt);
				if (retCode == SQL_NO_DATA)
				{
					result = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(retCode, L"SQLFetch") < ERROR_NONE) && (retCode != SQL_SUCCESS_WITH_INFO))
					break;

				record.itemCount[itemIndex].id = itemID;
				record.itemCount[itemIndex].count = count;

				if (++itemIndex >= INVENTORYMAX + WAREHOUSEMAX)
				{
					result = ERROR_NONE;
					break;
				}
			}

			record.count = itemIndex;
		}
	}

	SQLCloseCursor(m_hstmt);

	return result;
}

int CDNSQLWorld::QueryAddGuildWarItemTradeRecord(INT64 characterDBID, ItemCount& itemCount)
{
	CQueryTimeLog QueryTimeLog("P_AddGuildWarItemTradeRecord");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, characterDBID, 0, L"[QueryAddGuildWarItemTradeRecord] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildWarItemTradeRecord(?,?,?)}");

	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &result, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &characterDBID, sizeof(INT64), sqlLen[0].Get(sizeof(INT64)));
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &itemCount.id, sizeof(int), sqlLen[1].Get(sizeof(int)));
		SQLBindParameter(m_hstmt, 4, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &itemCount.count, sizeof(int), sqlLen[2].Get(sizeof(int)));

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");
	}
	return result;
}

int CDNSQLWorld::QueryModGuildExp(TQModGuildExp* pQ, TAModGuildExp& pA)
{
	CQueryTimeLog QueryTimeLog("P_ModGuildEXP");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryModGuildExp] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));
#if defined(PRE_ADD_GUILD_CONTRIBUTION)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildEXP(%d,%d,%d,%d,%d,?,?,%I64d,%d)}", 
		pQ->nGuildDBID,		// 길드ID
		pQ->cPointType,		// 길드 포인트 타입
		pQ->nPointValue,	// 길드 포인터 변경치
		pQ->nMaxPoint,		// 일일 길드포인트 최대치
		pQ->nLevel,			// 변경 후 길드 레벨
		pQ->biCharacterDBID,// 경험치를 습득한 캐릭터ID
		pQ->IsGuildLevelMax?1:0 //길드레벨이 최대치에 도달했는지 확인
		);
#else	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildEXP(%d,%d,%d,%d,%d,?,?)}", 
			pQ->nGuildDBID,		// 길드ID
			pQ->cPointType,		// 길드 포인트 타입
			pQ->nPointValue,	// 길드 포인터 변경치
			pQ->nMaxPoint,		// 일일 길드포인트 최대치
			pQ->nLevel			// 변경 후 길드 레벨
			);
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &result, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA.nTotalGuildExp, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA.nDailyPointValue, sizeof(int), &len);

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");
	}
	
	return result;
}

int CDNSQLWorld::QueryModGuildCheat(TQModGuildCheat* pQ, TAModGuildCheat& pA)
{
	CQueryTimeLog QueryTimeLog("P_ModGuildLevelForCheat");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryModGuildCheat] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildLevelForCheat(%d,%d,%d,%d)}", 
		pQ->nGuildDBID,		// 길드ID
		pQ->cCheatType,		// 치트 타입 1=레벨변경, 2=일일포인트 초기화
		pQ->nPoint,			// 변경할 길드 포인트
		pQ->cLevel			// 변경할 길드 레벨
		);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetGuildRewardItem(TQGetGuildRewardItem* pQ, TAGetGuildRewardItem* pA)
{
	CQueryTimeLog QueryTimeLog("P_GetGuildRewards");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetGuildRewardItem] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cbLen;

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildRewards(%d)}", pQ->nGuildDBID);
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		// 결과
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{	
			int nColNo = 1;
			TGuildRewardItem GuildRewardItem;
			TIMESTAMP_STRUCT RegisterDate;
			TIMESTAMP_STRUCT ExpireDate;
			memset(&RegisterDate, 0, sizeof(RegisterDate));
			memset(&ExpireDate, 0, sizeof(ExpireDate));
			memset(&GuildRewardItem, 0, sizeof(GuildRewardItem));
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildRewardItem.nItemID, sizeof(int), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &GuildRewardItem.bEternity, sizeof(bool), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &RegisterDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cbLen);
			CheckColumnCount(nColNo, "P_GetGuildRewards");
			while(1)
			{				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					result = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				CTimeParamSet ExpireTime(&QueryTimeLog, GuildRewardItem.m_tExpireDate, ExpireDate);
				if (!ExpireTime.IsValid())
				{
					result = ERROR_DB;
					break;
				}
				pA->RewardItemInfo[pA->nCount] = GuildRewardItem;
				pA->nCount++;
				if( pA->nCount >= GUILDREWARDEFFECT_TYPE_CNT )
				{
					result = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return result;
}

int CDNSQLWorld::QueryAddGuildRewardItem(TQAddGuildRewardItem* pQ, TAAddGuildRewardItem* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildReward" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryAddGuildRewardItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGuildReward(%d,%d,%d,%d,%d,%I64d,%d,%d,%d,?)}", 
		pQ->nGuildDBID, pQ->nItemID, pQ->bEnternityFlag, pQ->nLifeSpan, pQ->cItemBuyType, pQ->biCharacterDBID, pQ->nNeedGold, pQ->nMpaID, pQ->nRemoveItemID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		TIMESTAMP_STRUCT ExpireDate;
		memset(&ExpireDate, 0, sizeof(TIMESTAMP_STRUCT));
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;

		CTimeParamSet ExpireTime(&QueryTimeLog, pA->m_tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
		if (!ExpireTime.IsValid()) return ERROR_DB;
	}

	return ERROR_NONE;
}
int CDNSQLWorld::QueryChangeGuildSize(TQChangeGuildSize* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildMaxMemberCount" );	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGuildMaxMemberCount(%d, %d)}", pQ->nGuildDBID, pQ->nGuildSize);

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryDelGuildRewardItem(TQDelGuildEffectItem* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildRewards" );	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelGuildRewards(%d, %d)}", pQ->nGuildDBID, pQ->nItemID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModCharacterName(TQChangeCharacterName* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModCharacterName" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModCharacterName(%d,N'%s')}", pQ->nCharacterDBID, pQ->wszCharacterName );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListEtcPoint(TQGetListEtcPoint *pQ, TAGetListEtcPoint *pA)
{
	if(CheckConnect() < 0)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->nCharacterDBID, 0, L"[QueryGetListPoint] Check Connect Fail\r\n");
		pA->nRetCode = ERROR_DB;
		return ERROR_DB;
	}	

	SQLRETURN RetCode;
	SQLLEN cbLen;
	int nResult = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPoint(%I64d, N'')}",
		pQ->nCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		// 결과
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{	
			BYTE cPointType;
			INT64 biPoint;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_TINYINT, &cPointType, sizeof(BYTE), &cbLen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biPoint, sizeof(INT64), &cbLen);
			CheckColumnCount(nColNo, "P_GetListPoint");
			while(1)
			{
				cPointType = 0;
				biPoint = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				if(cPointType >= DBDNWorldDef::EtcPointCode::Max)
					break;
	
				pA->biEtcPoint[cPointType] = biPoint;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddEtcPoint( TQAddEtcPoint* pQ, TAAddEtcPoint *pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddPoint" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[P_AddPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPoint(%I64d,%d,%I64d,%d,N'%s',?)}", 
		pQ->biCharacterDBID, pQ->cType, pQ->biAddPoint, pQ->nMapID, pQ->wszIP);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biLeftPoint, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

int CDNSQLWorld::QueryUseEtcPoint( TQUseEtcPoint* pQ, TAUseEtcPoint *pA )
{
	CQueryTimeLog QueryTimeLog( "TQAUseEtcPoint" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[TQUseEtcPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_UsePoint(%I64d,%d,%I64d,%d,N'%s',?)}", 
		pQ->biCharacterDBID, pQ->cType, pQ->biUsePoint, pQ->nMapID, pQ->wszIP);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biLeftPoint, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}

#if defined( PRE_ADD_NEW_MONEY_SEED )
int CDNSQLWorld::QueryGetEtcPoint( INT64 biCharacterDBID, char cType, INT64 &nPoint )
{
	CQueryTimeLog QueryTimeLog( "P_GetPoint" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetEtcPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;	

	int nResult = ERROR_DB;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPoint(%I64d,%d,?)}", biCharacterDBID, cType);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &nPoint, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}
	return nResult;
}
#endif

#if !defined(_LOGINSERVER)
int CDNSQLWorld::CashItemDBQuery(int nAccountDBID, INT64 biCharacterDBID, int nMapID, int nChannelID, char cPaymentRules, TCashItemBase &BuyItem, int nPrice, INT64 biPurchaseOrderID, int nAddMaterializedItemCode, 
								 const WCHAR* pwszIP, INT64 biSenderCharacterDBID/* = 0*/, bool bGift/* = false*/, char cPayMethodCode/* = DBDNWorldDef::PayMethodCode::Cash*/)
{
	int nItemType = g_pExtManager->GetItemMainType(BuyItem.CashItem.nItemID);

	switch(nItemType){
	case ITEMTYPE_VIP:
		break;

	case ITEMTYPE_REBIRTH_COIN:
		{
			int nLogCode = DBDNWorldDef::CashRebirthCode::Cash;
			if(cPaymentRules == Cash::PaymentRules::Petal) nLogCode = DBDNWorldDef::CashRebirthCode::Petal;
#if defined( PRE_ADD_NEW_MONEY_SEED )
			if(cPaymentRules == Cash::PaymentRules::Seed) nLogCode = DBDNWorldDef::CashRebirthCode::Seed;
#endif

			int nRet = QueryAddCashRebirthCoin(biCharacterDBID, g_pExtManager->GetCashCommodityCount(BuyItem.nItemSN), g_pExtManager->GetCashCommodityPrice(BuyItem.nItemSN), nLogCode, BuyItem.CashItem.nItemID);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, 0, nAccountDBID, biCharacterDBID, 0, L"[QueryAddCashRebirthCoin] Ret:%d ItemSN:%d ItemID:%d ItemType:%d\r\n", nRet, BuyItem.nItemSN, BuyItem.CashItem.nItemID, nItemType);
			}
			//원인 파악 후 삭제
			if(g_pExtManager->GetCashCommodityCount(BuyItem.nItemSN) == 0){
				g_Log.Log(LogType::_ERROR, 0, nAccountDBID, biCharacterDBID, 0, L"[AddCashRebirthCoin] AddCashRebirthCoinCount Error ItemSN:%d ItemID:%d ItemType:%d\r\n", BuyItem.nItemSN, BuyItem.CashItem.nItemID, nItemType);
			}
			return nRet;
		}
		break;

	case ITEMTYPE_INVENTORY_SLOT:
	case ITEMTYPE_WAREHOUSE_SLOT:
	case ITEMTYPE_GESTURE:
	case ITEMTYPE_FARM_VIP:
	case ITEMTYPE_GLYPH_SLOT:
	case ITEMTYPE_PERIOD_PLATE:
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_PERIOD_TALISMAN_EX:
#endif
		{
			TQAddEffectItems Add;
			memset(&Add, 0, sizeof(TQAddEffectItems));

			Add.biItemSerial = BuyItem.CashItem.nSerial;
			Add.iItemID = BuyItem.CashItem.nItemID;
			Add.biCharacterDBID = biCharacterDBID;
			Add.cItemLifeSpanRenewal = DBDNWorldDef::EffectItemLifeSpanType::New;
			Add.Code = DBDNWorldDef::EffectItemGetCode::Cash;
			if(cPaymentRules == Cash::PaymentRules::Petal) Add.Code = DBDNWorldDef::EffectItemGetCode::Petal;
#if defined( PRE_ADD_NEW_MONEY_SEED )
			if(cPaymentRules == Cash::PaymentRules::Seed) Add.Code = DBDNWorldDef::EffectItemGetCode::Seed;
#endif
			Add.biFKey = biPurchaseOrderID;
			Add.iPrice = nPrice;

			Add.bEternityFlag = false;
			Add.iItemLifeSpan = 0;
			int nPeriod = g_pExtManager->GetCashCommodityPeriod(BuyItem.nItemSN);
			if(nPeriod <= 0){
				Add.bEternityFlag = true;
				Add.iItemLifeSpan = LIFESPANMAX;
			}
			else {
				Add.iItemLifeSpan = nPeriod * 24 * 60;
			}

			TItemData *pItemData = g_pExtManager->GetItemData(BuyItem.CashItem.nItemID);
			if(pItemData){
				Add.iProperty[0] = pItemData->nTypeParam[0];
				Add.iProperty[1] = pItemData->nTypeParam[1];
			}

			int nRet = QueryAddEffectItems(&Add);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, 0, nAccountDBID, biCharacterDBID, 0, L"[QueryAddEffectItems] Ret:%d ItemSN:%d ItemID:%d ItemType:%d\r\n", nRet, BuyItem.nItemSN, BuyItem.CashItem.nItemID, nItemType);
			}
			return nRet;
		}
		break;

	case ITEMTYPE_GUILDWARE_SLOT:
		break;

	default:
		{
			TQAddMaterializedItem Add;
			memset(&Add, 0, sizeof(TQAddMaterializedItem));

			wcsncpy( Add.wszIP, pwszIP, IPLENMAX );
			Add.AddItem = BuyItem.CashItem;
			Add.biCharacterDBID = biCharacterDBID;
			Add.Code = (DBDNWorldDef::AddMaterializedItem::eCode)nAddMaterializedItemCode;// DBDNWorldDef::AddMaterializedItem::CashBuy;
			Add.biFKey = biPurchaseOrderID;
			Add.biItemPrice = nPrice;
			
			Add.cItemLocationCode = DBDNWorldDef::ItemLocation::CashInventory;
			if(nItemType == ITEMTYPE_VEHICLE )
			{
				Add.cItemLocationCode = DBDNWorldDef::ItemLocation::VehicleInventory;
				Add.dwPetPartsColor1 = BuyItem.dwPartsColor1;
				if(BuyItem.VehiclePart1.nItemID > 0){
					Add.PetList[Add.cPetCount].cSlotIndex = Vehicle::Slot::Saddle;
					Add.PetList[Add.cPetCount].nItemID = BuyItem.VehiclePart1.nItemID;
					Add.PetList[Add.cPetCount].biItemSerial = BuyItem.VehiclePart1.nSerial;
					Add.cPetCount++;
				}

				if(BuyItem.VehiclePart2.nItemID > 0){
					Add.PetList[Add.cPetCount].cSlotIndex = Vehicle::Slot::Hair;
					Add.PetList[Add.cPetCount].nItemID = BuyItem.VehiclePart2.nItemID;
					Add.PetList[Add.cPetCount].biItemSerial = BuyItem.VehiclePart2.nSerial;
					Add.cPetCount++;
				}
			}
			else if(nItemType == ITEMTYPE_PET)
			{
				Add.cItemLocationCode = DBDNWorldDef::ItemLocation::PetInventory;

				Add.dwPetPartsColor1 = BuyItem.dwPartsColor1;
				Add.dwPetPartsColor2 = BuyItem.dwPartsColor2;
				if(BuyItem.VehiclePart1.nItemID > 0)
				{
					Add.PetList[Add.cPetCount].cSlotIndex = Vehicle::Slot::Saddle;
					Add.PetList[Add.cPetCount].nItemID = BuyItem.VehiclePart1.nItemID;
					Add.PetList[Add.cPetCount].biItemSerial = BuyItem.VehiclePart1.nSerial;
					Add.cPetCount++;
				}

				if(BuyItem.VehiclePart2.nItemID > 0)
				{
					Add.PetList[Add.cPetCount].cSlotIndex = Vehicle::Slot::Hair;
					Add.PetList[Add.cPetCount].nItemID = BuyItem.VehiclePart2.nItemID;
					Add.PetList[Add.cPetCount].biItemSerial = BuyItem.VehiclePart2.nSerial;
					Add.cPetCount++;
				}
				TVehicleData *pVehicleData = g_pExtManager->GetVehicleData(BuyItem.CashItem.nItemID);
				if( pVehicleData )
				{
					if( pVehicleData->nPetSkillID1 > 0 )
						Add.nPetSkillID1 = pVehicleData->nPetSkillID1;
					if( pVehicleData->nPetSkillID2 > 0 )
						Add.nPetSkillID2 = pVehicleData->nPetSkillID2;
				}
			}

			Add.cPayMethodCode = cPayMethodCode;	// DBDNWorldDef::PayMethodCode::Cash;
			if(cPaymentRules == Cash::PaymentRules::Petal)
				Add.cPayMethodCode = DBDNWorldDef::PayMethodCode::Petal;
#if defined( PRE_ADD_NEW_MONEY_SEED )
			else if(cPaymentRules == Cash::PaymentRules::Seed)
				Add.cPayMethodCode = DBDNWorldDef::PayMethodCode::SeedPoint;
#endif
			Add.iMapIndex = nMapID;
			Add.iChannelID = nChannelID;
			if(bGift)
				Add.biSenderCharacterDBID = biSenderCharacterDBID;

			int nRet = QueryAddMaterializedItem(&Add);
			if (nRet != ERROR_NONE){
				g_Log.Log(LogType::_ERROR, 0, nAccountDBID, biCharacterDBID, 0, L"[QueryAddMaterializedItem] Ret:%d ItemSN:%d ItemID:%d ItemType:%d\r\n", nRet, BuyItem.nItemSN, BuyItem.CashItem.nItemID, nItemType);
			}
			return nRet;
		}
		break;
	}

	return ERROR_NONE;
}
#endif	// #if !defined(_LOGINSERVER)

int CDNSQLWorld::QueryAddPartyStartLog( TQAddPartyStartLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPartyStartLog" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddPartyStartLog(%I64d)}", pQ->biPartyLogID );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddPartyEndLog( TQAddPartyEndLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPartyEndLog" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddPartyEndLog(%I64d)}", pQ->biPartyLogID );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddStageStartLog( TQAddStageStartLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddStageStartLog" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddStageStartLog(%I64d,%I64d,%d,%d,%d)}", pQ->biRoomLogID, pQ->biPartyLogID, pQ->cPartySize, pQ->iMapID, pQ->Code );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddStageClearLog( TQAddStageClearLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddStageClearLog" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

#if defined( PRE_ADD_36870 )
	swprintf(m_wszQuery, L"{CALL dbo.P_AddStageClearLog(%I64d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)}", 
		pQ->biRoomLogID, pQ->biCharacterDBID, pQ->iJobID, pQ->cCharacterLevel, pQ->bClearFlag, pQ->Code, pQ->iMaxComboCount, pQ->cBossKillCount,
		pQ->iRewardCharacterExp, pQ->iBonusCharacterExp, pQ->iClearPoint, pQ->iBonusClearPoint, pQ->iClearTimeSec, pQ->iClearRound );
#else
	swprintf(m_wszQuery, L"{CALL dbo.P_AddStageClearLog(%I64d,%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)}", 
		pQ->biRoomLogID, pQ->biCharacterDBID, pQ->iJobID, pQ->cCharacterLevel, pQ->bClearFlag, pQ->Code, pQ->iMaxComboCount, pQ->cBossKillCount,
		pQ->iRewardCharacterExp, pQ->iBonusCharacterExp, pQ->iClearPoint, pQ->iBonusClearPoint, pQ->iClearTimeSec );
#endif // #if defined( PRE_ADD_36870 )

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddStageRewardLog( TQAddStageRewardLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddStageRewardLog" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddStageRewardLog] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddStageRewardLog(%I64d,%I64d,%d,?,%d,%d)}", pQ->biRoomLogID, pQ->biCharacterDBID, pQ->Code, pQ->iRewardItemID, pQ->iRewardRet );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen1;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biRewardItemSerial, sizeof(INT64), SqlLen1.GetNull(pQ->iRewardRet!=ERROR_NONE,sizeof(INT64)));

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;

}

int CDNSQLWorld::QueryAddStageEndLog( TQAddStageEndLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddStageEndLog" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddStageEndLog(%I64d,%d)}", pQ->biRoomLogID, pQ->Code );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddPvPStartLog( TQAddPvPStartLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPStartLog" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{CALL dbo.P_AddPVPStartLog(%I64d,%d,%d,%d,%d,%d,%d,%d,%d,%I64d,%I64d)}", 
		pQ->biRoomID1, pQ->iRoomID2, pQ->cMaxPlayerCount, pQ->Code, pQ->iPvPModeDetailValue, 
		pQ->bBreakIntoFlag, pQ->bItemUseFlag, pQ->bSecretFlag, pQ->iMapID, pQ->biCreateRoomCharacterDBID, pQ->biRoomMasterCharacterDBID );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}


int CDNSQLWorld::QueryAddPvPEndLog( TQAddPvPEndLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPVPEndLog" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddPVPEndLog(%I64d,%d)}", pQ->biRoomID1, pQ->iRoomID2 );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

#if defined( PRE_ADD_58761 )
int CDNSQLWorld::QueryAddNestGateStartLog( TQAddNestGateStartLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddNestGateStartLog" );


	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddNestGateStartLog(%I64d,%d,%d)}", pQ->biRoomID, pQ->cPartySize, pQ->nGateID );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddNestGateEndLog( TQAddNestGateEndLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddNestGateEndtLog" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{CALL dbo.P_AddNestGateEndLog(%I64d,%d)}", pQ->biRoomID, pQ->cPartySize );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddNestGateClearLog( TQAddNestGateClearLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddNestGateClearLog" );


	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{CALL dbo.P_AddNestGateClearLog(%I64d,%d,%I64d,%d,%d)}", pQ->biRoomID, pQ->bClearFlag, pQ->biChracterDBID, pQ->cCharacterJob, pQ->cCharacterLevel );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}

int CDNSQLWorld::QueryAddNestDeathLog( TQNestDeathLog* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddNestDeathLog" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{CALL dbo.P_AddNestDeathLog(%I64d,%I64d,%d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->biRoomID, pQ->nMonterID, pQ->nSkillID, pQ->cCharacterLevel, pQ->cCharacterJob );

	return(CommonResultQuery(m_wszQuery) == ERROR_NONE) ? ERROR_NONE : ERROR_DB;
}
#endif

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

int CDNSQLWorld::QueryModNpcLocation( TQModNPCLocation* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModNPCLocation" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModNPCLocation(%d,%d)}", pQ->iNPCID, pQ->iMapID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModNpcFavor( TQModNPCFavor* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModNPCFavor" );

	std::string strNpcID,strFavorPoint,strMalicePoint;
	for( UINT i=0 ; i<pQ->cCount ; ++i )
	{
		if( i > 0)
		{
			strNpcID.append(",");
			strFavorPoint.append(",");
			strMalicePoint.append(",");
		}
		strNpcID.append( boost::lexical_cast<std::string>(pQ->UpdateArr[i].iNpcID) );
		strFavorPoint.append( boost::lexical_cast<std::string>(pQ->UpdateArr[i].iFavorPoint) );
		strMalicePoint.append( boost::lexical_cast<std::string>(pQ->UpdateArr[i].iMalicePoint) );
	}

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModNPCFavor(%I64d,'%S','%S','%S')}", pQ->biCharacterDBID, strNpcID.c_str(), strFavorPoint.c_str(), strMalicePoint.c_str() );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListNpcFavor( TQGetListNpcFavor* pQ, std::vector<TNpcReputation>& vReputation )
{
	CQueryTimeLog QueryTimeLog( "P_GetListNPCFavor" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListNpcFavor] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	//swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListNPCFavor(?,?)}");
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListNPCFavor(?)}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1,SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->biCharacterDBID, sizeof(INT64), SqlLen1.Get(sizeof(INT64)));
		//SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(INT64), 0, &pQ->iMapID, sizeof(int), SqlLen2.Get(sizeof(int)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TNpcReputation ReputationData;
			memset( &ReputationData, 0, sizeof(ReputationData) );

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ReputationData.iNpcID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ReputationData.iFavorPoint, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &ReputationData.iMalicePoint, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetListNPCFavor");
			while(1)
			{
				memset( &ReputationData, 0, sizeof(ReputationData) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				vReputation.push_back( ReputationData );
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )

// 사제 시스템
int CDNSQLWorld::QueryGetMasterPupilInfo( TQGetMasterPupilInfo* pQ, TAGetMasterPupilInfo* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTeacherPupilInfo" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetMasterPupilInfo] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetTeacherPupilInfo(%I64d,?,?,?,?,?,?)}", pQ->biCharacterDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		TIMESTAMP_STRUCT BlockDate;
		memset( &BlockDate, 0, sizeof(BlockDate) );
		char szBuf[500];
		WCHAR wszNameBuf[500];
		memset( szBuf, 0, sizeof(szBuf) );
		memset( wszNameBuf, 0, sizeof(wszNameBuf) );

		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iMasterCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iPupilCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &BlockDate, sizeof(TIMESTAMP_STRUCT), &cblen);
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_CHAR, SQL_VARCHAR, sizeof(szBuf), 0, szBuf, sizeof(szBuf), &cblen);
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iGraduateCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(wszNameBuf), 0, wszNameBuf, sizeof(wszNameBuf), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );

		CTimeParamSet BlockTime(&QueryTimeLog, pA->SimpleInfo.BlockDate, BlockDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
		if (!BlockTime.IsValid()) return ERROR_DB;
		pA->SimpleInfo.cCharacterDBIDCount	= 0;

		if( strlen(szBuf) && wcslen(wszNameBuf) )
		{
			std::string					strString(szBuf);
			std::wstring				wstrNameString(wszNameBuf);
			std::vector<std::string>	vSplit;
			std::vector<std::wstring>	vNameSplit;
			boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );
			boost::algorithm::split( vNameSplit, wstrNameString, boost::algorithm::is_any_of(L",") );

			pA->SimpleInfo.cCharacterDBIDCount = (vSplit.size() > _countof(pA->SimpleInfo.OppositeInfo)) ? _countof(pA->SimpleInfo.OppositeInfo) : static_cast<BYTE>(vSplit.size());
			for( UINT i=0 ; i<pA->SimpleInfo.cCharacterDBIDCount ; ++i )
				pA->SimpleInfo.OppositeInfo[i].CharacterDBID = _atoi64(vSplit[i].c_str());
			for( UINT i=0 ; i<pA->SimpleInfo.cCharacterDBIDCount ; ++i )
				_wcscpy( pA->SimpleInfo.OppositeInfo[i].wszCharName, _countof(pA->SimpleInfo.OppositeInfo[i].wszCharName), vNameSplit[i].c_str(), (int)wcslen(vNameSplit[i].c_str()) );
		}
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetMasterPupilInfo2( TQGetMasterPupilInfo* pQ, TAGetMasterPupilInfo* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTeacherPupilInfo2" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetMasterPupilInfo2] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetTeacherPupilInfo2(%I64d,?,?,?,?,?)}", pQ->biCharacterDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		TIMESTAMP_STRUCT BlockDate;
		memset( &BlockDate, 0, sizeof(BlockDate) );
		
		WCHAR wszBuf[500];		
		memset( wszBuf, 0, sizeof(wszBuf) );

		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iMasterCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iPupilCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &BlockDate, sizeof(TIMESTAMP_STRUCT), &cblen);		
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->SimpleInfo.iGraduateCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(wszBuf), 0, wszBuf, sizeof(wszBuf), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );

		CTimeParamSet BlockTime(&QueryTimeLog, pA->SimpleInfo.BlockDate, BlockDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
		if (!BlockTime.IsValid()) return ERROR_DB;

		pA->SimpleInfo.cCharacterDBIDCount	= 0;

		if( wcslen(wszBuf) )
		{			
			std::wstring				wstrString(wszBuf);			
			std::vector<std::wstring>	vInfoSplit;			

			boost::algorithm::split( vInfoSplit, wstrString, boost::algorithm::is_any_of(L"|") );

			pA->SimpleInfo.cCharacterDBIDCount = (vInfoSplit.size() > _countof(pA->SimpleInfo.OppositeInfo)) ? _countof(pA->SimpleInfo.OppositeInfo) : static_cast<BYTE>(vInfoSplit.size());
			for( UINT i=0 ; i<pA->SimpleInfo.cCharacterDBIDCount ; ++i )
			{
				std::vector<std::wstring>	vRowSplit;
				boost::algorithm::split( vRowSplit, vInfoSplit[i], boost::algorithm::is_any_of(L",") );
				if( vRowSplit.size() != 3) // 이런 안돼는데..캐릭터ID, 캐릭터이름, 호감도
					break;				
				pA->SimpleInfo.OppositeInfo[i].CharacterDBID = _wtoi64(vRowSplit[0].c_str());
				_wcscpy( pA->SimpleInfo.OppositeInfo[i].wszCharName, _countof(pA->SimpleInfo.OppositeInfo[i].wszCharName), vRowSplit[1].c_str(), (int)wcslen(vRowSplit[1].c_str()) );
				pA->SimpleInfo.OppositeInfo[i].nFavorPoint = _wtoi(vRowSplit[2].c_str());
			}				
		}
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryAddMasterCharacter( TQAddMasterCharacter* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddTeacherCharacter" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	WCHAR wszText[MasterSystem::Max::SelfIntrotuctionLen*2+1];
	memset(wszText, 0, sizeof(wszText));
	ConvertQuery(pQ->wszSelfIntroduction, MasterSystem::Max::SelfIntrotuctionLen, wszText, _countof(wszText));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddTeacherCharacter(%I64d,N'%s')}", pQ->biCharacterDBID, wszText );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelMasterCharacter( TQDelMasterCharacter* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelTeacherCharacter" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelTeacherCharacter(%I64d)}", pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetPageMasterCharacter( TQGetPageMasterCharacter* pQ, TAGetPageMasterCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPageTeacherCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetPageMasterCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageTeacherCharacter(%d,%d,?,?)}",(pQ->uiPage*MasterSystem::Max::MasterPageRowCount)+1,((pQ->uiPage+1)*MasterSystem::Max::MasterPageRowCount) );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1,SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cJobCode, sizeof(BYTE), SqlLen1.GetNull(pQ->cJobCode<= 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cGenderCode, sizeof(BYTE), SqlLen2.GetNull(pQ->cGenderCode<= 0, sizeof(BYTE)));

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TMasterInfo MasterInfo;
			memset( &MasterInfo, 0, sizeof(MasterInfo) );

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &MasterInfo.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, MasterInfo.wszCharName, sizeof(MasterInfo.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &MasterInfo.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &MasterInfo.cJobCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &MasterInfo.cGenderCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &MasterInfo.iRespectPoint, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, MasterInfo.wszSelfIntrodution, sizeof(MasterInfo.wszSelfIntrodution), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &MasterInfo.iGraduateCount, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetPageTeacherCharacter");
			while(1)
			{
				memset( &MasterInfo, 0, sizeof(MasterInfo) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;


				pA->MasterInfoList[pA->cCount] = MasterInfo;
				++pA->cCount;

				if( pA->cCount >= MasterSystem::Max::MasterPageRowCount )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

// 자신의 스승 정보
int CDNSQLWorld::QueryGetMasterCharacterType1( TQGetMasterCharacter* pQ, TAGetMasterCharacterType1* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTeacherCharacter1" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetMasterCharacterType1] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetTeacherCharacter(%d,%I64d,%I64d)}", DBDNWorldDef::GetMasterCharacterCode::MyInfo, pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &pA->MasterCharacterInfo.bMasterListShowFlag, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->MasterCharacterInfo.wszSelfIntroduction, sizeof(pA->MasterCharacterInfo.wszSelfIntroduction), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->MasterCharacterInfo.iGraduateCount, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->MasterCharacterInfo.iRespectPoint, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetTeacherCharacter1");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				nResult = ERROR_NONE;
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

// 내 스승 정보
int CDNSQLWorld::QueryGetMasterCharacterType2( TQGetMasterCharacter* pQ, TAGetMasterCharacterType2* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTeacherCharacter2" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetMasterCharacterType2] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetTeacherCharacter(%d,%I64d,%I64d)}", DBDNWorldDef::GetMasterCharacterCode::MyMasterInfo, pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->MasterInfo.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->MasterInfo.wszCharName, sizeof(pA->MasterInfo.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->MasterInfo.cJob, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->MasterInfo.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->MasterInfo.iRespectPoint, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->MasterInfo.iFavorPoint, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetTeacherCharacter2");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				nResult = ERROR_NONE;
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

// 임의의 특정 스승 정보
int CDNSQLWorld::QueryGetMasterCharacterType3( TQGetMasterCharacter* pQ, TAGetMasterCharacterType3* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTeacherCharacter3" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetMasterCharacterType3] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetTeacherCharacter(%d,%I64d,%I64d)}", DBDNWorldDef::GetMasterCharacterCode::OptionalMasterInfo, pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->wszCharName, sizeof(pA->wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->wszSelfIntroduction, sizeof(pA->wszSelfIntroduction), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cGenderCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cJob, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->iGraduateCount, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->iRespectPoint, sizeof(int), &cblen);
			CheckColumnCount(nColNo, "P_GetTeacherCharacter3");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				nResult = ERROR_NONE;
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetListPupil( TQGetListPupil* pQ, TAGetListPupil* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPupil" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListPupil] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPupil(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1,SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TIMESTAMP_STRUCT LastConnectDate;
			memset( &LastConnectDate, 0, sizeof(LastConnectDate) );

			TPupilInfo PupilInfo;
			memset( &PupilInfo, 0, sizeof(PupilInfo) );

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &PupilInfo.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, PupilInfo.wszCharName, sizeof(PupilInfo.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &PupilInfo.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &PupilInfo.cJob, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &PupilInfo.iFavorPoint, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &LastConnectDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListPupil");
			while(1)
			{
				memset( &PupilInfo, 0, sizeof(PupilInfo) );
				memset( &LastConnectDate, 0, sizeof(LastConnectDate) );				

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->PupilInfoList[pA->cCount]					= PupilInfo;
				CTimeParamSet LastConnectTime(&QueryTimeLog, pA->PupilInfoList[pA->cCount].tLastConnectDate, LastConnectDate, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!LastConnectTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}
				++pA->cCount;
				
				if( pA->cCount >= MasterSystem::Max::PupilCount )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryAddMasterAndPupil( TQAddMasterAndPupil* pQ, TAAddMasterAndPupil* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddTeacherAndPupil" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddMasterAndPupil] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	DBDNWorldDef::TransactorCode::eCode Code = pQ->Code;
	if( pQ->bIsDirectMenu == true )
		Code = (pQ->Code == DBDNWorldDef::TransactorCode::Master) ? DBDNWorldDef::TransactorCode::Pupil : DBDNWorldDef::TransactorCode::Master;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddTeacherAndPupil(%I64d,%I64d,%d,%d,%d,%d,%d,?,%d)}", 
		pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID, MasterSystem::Max::MasterCount, MasterSystem::Max::PupilCount, pQ->bCheckRegisterFlag, g_pExtManager->GetGlobalWeightValue(MasterSystem_MasterAndPupilAllowLevelGap), Code, pQ->bIsSkip );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->uiOppositeAccountDBID, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryDelMasterAndPupil( TQDelMasterAndPupil* pQ, TADelMasterAndPupil* pA )
{
	CQueryTimeLog QueryTimeLog( "P_DelTeacherAndPupil" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryDelMasterAndPupil] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	pA->biMasterCharacterDBID	= pQ->biMasterCharacterDBID;
	pA->biPupilCharacterDBID	= pQ->biPupilCharacterDBID;
	pA->Code					= pQ->Code;
	pA->iPenaltyRespectPoint	= pQ->iPenaltyRespectPoint;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelTeacherAndPupil(%I64d,%I64d,%d,%d,%d,%d)}", pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID, pQ->Code, pQ->iPenaltyDay, pQ->iPenaltyRespectPoint, pQ->bIsSkip );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryGetListMyMasterAndClassmate( TQGetListMyMasterAndClassmate* pQ, TAGetListMyMasterAndClassmate* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListMyTeacherAndClassmate" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListMyMasterAndClassmate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMyTeacherAndClassmate(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1,SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TIMESTAMP_STRUCT LastConnectDate;
			memset( &LastConnectDate, 0, sizeof(LastConnectDate) );

			TMasterAndClassmateInfo RecordInfo;
			memset( &RecordInfo, 0, sizeof(RecordInfo) );

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &RecordInfo.Code, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &RecordInfo.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, RecordInfo.wszCharName, sizeof(RecordInfo.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &LastConnectDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListMyTeacherAndClassmate");
			while(1)
			{
				memset( &RecordInfo, 0, sizeof(RecordInfo) );
				memset( &LastConnectDate, 0, sizeof(LastConnectDate) );				

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->MasterAndClassmateInfoList[pA->cCount]					= RecordInfo;
				CTimeParamSet LastConnectTime(&QueryTimeLog, pA->MasterAndClassmateInfoList[pA->cCount].tLastConnectDate, LastConnectDate, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!LastConnectTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}

				++pA->cCount;

				if( pA->cCount >= _countof(pA->MasterAndClassmateInfoList) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetMyClassmate( TQGetMyClassmate* pQ, TAGetMyClassmate* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetMyClassmate" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetMyClassmate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetMyClassmate(%I64d,%I64d)}", pQ->biCharacterDBID, pQ->biClassmateCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			WCHAR wszCharNameList[(NAMELENMAX+1)*MasterSystem::Max::MasterCount];
			memset( wszCharNameList, 0, sizeof(wszCharNameList) );
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->ClassmateInfo.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->ClassmateInfo.wszCharName, sizeof(pA->ClassmateInfo.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->ClassmateInfo.cJob, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->ClassmateInfo.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, wszCharNameList, sizeof(wszCharNameList), &cblen);
			CheckColumnCount(nColNo, "P_GetMyClassmate");
			while(1)
			{
				memset( wszCharNameList, 0, sizeof(wszCharNameList) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				std::wstring				wstrString(wszCharNameList);
				std::vector<std::wstring>	vSplit;

				boost::algorithm::split( vSplit, wstrString, boost::algorithm::is_any_of(L",") );

				pA->ClassmateInfo.cMasterCount = static_cast<BYTE>(vSplit.size());
				if( pA->ClassmateInfo.cMasterCount > MasterSystem::Max::MasterCount )
				{
					_ASSERT(0);
					pA->ClassmateInfo.cMasterCount = MasterSystem::Max::MasterCount;
				}

				for( UINT i=0 ; i<pA->ClassmateInfo.cMasterCount ; ++i )
					_wcscpy( pA->ClassmateInfo.wszMasterCharName[i], _countof(pA->ClassmateInfo.wszMasterCharName[i]), vSplit[i].c_str(), (int)wcslen(vSplit[i].c_str()) );
				
				nResult = ERROR_NONE;
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGraduate( TQGraduate* pQ, TAGraduate* pA )
{
	CQueryTimeLog QueryTimeLog( "P_Graduate" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGraduate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_Graduate(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;
		CDNSqlLen SqlLen1,SqlLen2;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;
			
			INT64 biCharacterDBID = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen );
			CheckColumnCount(nColNo, "P_Graduate");
			while(1)
			{
				biCharacterDBID = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->biMasterCharacterDBIDList[pA->cCount] = biCharacterDBID;
				++pA->cCount;

				if( pA->cCount >= _countof(pA->biMasterCharacterDBIDList) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryModRespectPoint( TQModRespectPoint* pQ, TAModRespectPoint* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModRespectPoint" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModRespectPoint(%I64d,%d)}", pQ->biCharacterDBID,pQ->iRespectPoint );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModMasterFavorPoint( TQModMasterFavorPoint* pQ, TAModMasterFavorPoint* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModTPFavorPoint" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModTPFavorPoint(%I64d,%I64d,%d)}", pQ->biMasterCharacterDBID, pQ->biPupilCharacterDBID, pQ->iFavorPoint );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetMasterAndFavorPoint( TQGetMasterAndFavorPoint* pQ, TAGetMasterAndFavorPoint* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListMyTeacher" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetMasterAndFavorPoint] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListMyTeacher(%I64d)}", pQ->biPupilCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TMasterAndFavorPoint Data;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &Data.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &Data.iFavorPoint, sizeof(int),	&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, Data.wszCharName,	sizeof(Data.wszCharName), &cblen );
			CheckColumnCount(nColNo, "P_GetListMyTeacher");
			while(1)
			{
				memset(&Data, 0, sizeof(Data));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->MasterAndFavorPoint[pA->cCount] = Data;
				++pA->cCount;

				if( pA->cCount >= _countof(pA->MasterAndFavorPoint) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetMasterSystemCountInfo( TQGetMasterSystemCountInfo* pQ, TAGetMasterSystemCountInfo* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetTPCCount" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetMasterSystemCountInfo] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetTPCCount(%I64d,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)}", pQ->biCharacterDBID );

	int iSQLResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN		cblen;
		CDNSqlLen	DNSqlLen[15];
		INT64		biCharacterDBID[15];

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	

		for( UINT i=0 ; i<15 ; ++i )
		{
			biCharacterDBID[i] = 0;
			if( i<PARTYCOUNTMAX-1 && i < pQ->cCount )
				biCharacterDBID[i] = pQ->biPartyCharacterDBID[i];

			SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID[i], sizeof(INT64), DNSqlLen[i].GetNull( biCharacterDBID[i] <= 0, sizeof(INT64)) );
		}

		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iMasterCount, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iPupilCount, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->iClassmateCount, sizeof(int), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return iSQLResult;
}

int CDNSQLWorld::QueryModGraduateCount( TQModMasterSystemGraduateCount* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModGraduateCount" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModGraduateCount(%I64d,%d)}", pQ->biCharacterDBID, pQ->iGraduateCount );

	return CommonReturnValueQuery(m_wszQuery);
}

#if defined( PRE_ADD_SECONDARY_SKILL )

int CDNSQLWorld::QueryAddSecondarySkill( TQAddSecondarySkill* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddSecondarySkill" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddSecondarySkill(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->iSecondarySkillID, pQ->Type );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelSecondarySkill( TQDelSecondarySkill* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelSecondarySkill" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelSecondarySkill(%I64d,%d)}", pQ->biCharacterDBID, pQ->iSecondarySkillID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetListSecondarySkill( TQGetListSecondarySkill* pQ, TAGetListSecondarySkill* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListSecondarySkill" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListSecondarySkill] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListSecondarySkill(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			int iSkillID,iExp;
			BYTE cType;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &iSkillID,	sizeof(int),	&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cType,		sizeof(char),	&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER, &iExp,		sizeof(int),	&cblen );
			CheckColumnCount(nColNo, "P_GetListSecondarySkill");
			while(1)
			{
				iSkillID = 0;
				iExp = 0;
				cType = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->SkillList[pA->cCount].iSkillID	= iSkillID;
				//pA->SkillList[pA->cCount].Type		= static_cast<SecondarySkill::Type::eType>(cType);
				pA->SkillList[pA->cCount].iExp		= iExp;
				++pA->cCount;

				if( pA->cCount >= _countof(pA->SkillList) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryModSecondarySkillExp( TQModSecondarySkillExp* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModSecondarySkillExp" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryModSecondarySkillExp] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModSecondarySkillExp(%I64d,%d,%d,?)}", pQ->biCharacterDBID, pQ->iSecondarySkillID, pQ->iSecondarySkillExp );

	int iSQLResult = ERROR_DB;
	int iExpAfter  = 0;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iExpAfter, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	// After 값 비교
	if( iSQLResult == ERROR_NONE )
	{
		if( pQ->iSecondarySkillExpAfter != iExpAfter )
			return ERROR_INVALIDPACKET;
	}

	return iSQLResult;
}

int CDNSQLWorld::QuerySetManufactureSkillRecipe( TQSetManufactureSkillRecipe* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_SetManufactureSkillRecipe" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SetManufactureSkillRecipe2(%I64d,%d,%I64d,%d)}", pQ->biCharacterDBID, pQ->LocationCode, pQ->biItemSerial, pQ->iSecondarySkillID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryExtractManufactureSkillRecipe( TQExtractManufactureSkillRecipe* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ExtractManufactureSkillRecipe" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ExtractManufactureSkillRecipe2(%I64d,%I64d,%d)}", pQ->biCharacterDBID, pQ->biItemSerial, pQ->LocationCode);

	return CommonReturnValueQuery(m_wszQuery);
}

int	CDNSQLWorld::QueryDelManufactureSkillRecipe( TQDelManufactureSkillRecipe* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelManufactureSkillRecipe" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelManufactureSkillRecipe(%I64d,%I64d,'%S')}", pQ->biCharacterDBID, pQ->biItemSerial, pQ->szIP );

	return CommonReturnValueQuery(m_wszQuery);
}

int	CDNSQLWorld::QueryGetListManufactureSkillRecipe( TQGetListManufactureSkillRecipe* pQ, TAGetListManufactureSkillRecipe* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListManufactureSkillRecipe" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListManufactureSkillRecipe] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListManufactureSkillRecipe(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TSecondarySkillRecipe Data;
			int iLifespan;
			bool bEternityFlag;
			TIMESTAMP_STRUCT ExpireDate = { 0, };

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&Data.iSkillID,			sizeof(int),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT,	&Data.biItemSerial,		sizeof(INT64),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&Data.iItemID,			sizeof(int),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT,	&Data.nExp,				sizeof(short),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,		&iLifespan,				sizeof(int),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,			&bEternityFlag,			sizeof(bool),				&cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP,	&ExpireDate,			sizeof(TIMESTAMP_STRUCT),	&cblen );
			CheckColumnCount(nColNo, "P_GetListManufactureSkillRecipe");
			while(1)
			{
				memset(&Data, 0, sizeof(Data));
				memset(&ExpireDate, 0, sizeof(ExpireDate));
				iLifespan = 0;
				bEternityFlag = false;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RecipeList[pA->cCount]				= Data;
				//pA->RecipeList[pA->cCount].tExpireDate	= _dbStamptoTime64_t(ExpireDate);
				++pA->cCount;

				if( pA->cCount >= _countof(pA->RecipeList) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryModManufactureSkillRecipe( TQModManufactureSkillRecipe* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModManufactureSkillRecipe" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryModManufactureSkillRecipe] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModManufactureSkillRecipe(%I64d,%I64d,%d,?)}", pQ->biCharacterDBID, pQ->biItemSerial, pQ->nDurability );

	int		iSQLResult			= ERROR_DB;
	short	nDurabilityAfter	= 0;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &nDurabilityAfter, sizeof(short), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	// After 값 비교
	if( iSQLResult == ERROR_NONE )
	{
		if( pQ->nDurabilityAfter != nDurabilityAfter )
			return ERROR_INVALIDPACKET;
	}

	return iSQLResult;
}

#endif // #if defined( PRE_ADD_SECONDARY_SKILL )


int CDNSQLWorld::QueryGetListFarm( TQGetListFarm* pQ, TAGetListFarm* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListFarm" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListFarm] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;
	CDNSqlLen	SqlLen;	

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFarm(?,4)}" );
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFarm(?,3)}" );
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &pQ->cEnableFlag, sizeof(bool), SqlLen.GetNull( pQ->cEnableFlag < 0, sizeof(bool) ) );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TFarmItem FarmItem;
			memset( &FarmItem, 0, sizeof(FarmItem) );

			int nColNo = 1;
#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
			BYTE cAttr = 0;
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&FarmItem.iFarmDBID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, FarmItem.wszFarmName, sizeof(FarmItem.wszFarmName), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&FarmItem.iFarmMapID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&FarmItem.iFarmMaxUser, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,	&FarmItem.bEnableFlag, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT,	&FarmItem.bStartActivate, sizeof(bool), &cblen );
#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cAttr, sizeof(BYTE), &cblen );
#endif // #if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
			CheckColumnCount(nColNo, "P_GetListFarm");
			while(1)
			{
				memset( &FarmItem, 0, sizeof(FarmItem) );
#if defined( PRE_ADD_VIP_FARM ) || defined( PRE_ADD_FARM_DOWNSCALE )
				cAttr = 0;
#endif

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

#if defined( PRE_ADD_FARM_DOWNSCALE )
				FarmItem.iAttr = cAttr;
#if defined( _WORK )
				// TestCode
				/*
				if( FarmItem.iFarmDBID == 1 )
					FarmItem.iAttr |= Farm::Attr::DownScale;
				*/
#endif // #if defined( _WORK )
#elif defined( PRE_ADD_VIP_FARM )
				FarmItem.Attr = static_cast<Farm::Attr::eType>(cAttr);
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

				pA->Farms[pA->cCount] = FarmItem;
				++pA->cCount;

				if( pA->cCount >= _countof(pA->Farms) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetListField( TQGetListField* pQ, TAGetListField* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListField" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListField] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListField(%d)}", pQ->iFarmDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TFarmField Record;
			memset( &Record, 0, sizeof(Record) );
			
			WCHAR wszAttachItemID[1024];
			WCHAR wszAttachItemCount[1024];

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Record.nFieldIndex, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &Record.biCharacterDBID, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, Record.wszCharName, sizeof(Record.wszCharName), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cLevel, sizeof(BYTE), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cJob, sizeof(BYTE), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iElapsedTimeSec, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemID, sizeof(wszAttachItemID), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemCount, sizeof(wszAttachItemCount), &cblen );
			CheckColumnCount(nColNo, "P_GetListField");
			while(1)
			{
				memset( &Record, 0, sizeof(Record) );
				memset( wszAttachItemID, 0, sizeof(wszAttachItemID) );
				memset( wszAttachItemCount, 0, sizeof(wszAttachItemCount) );
				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Fields[pA->cCount] = Record;
				// AttachItem
				if( wcslen(wszAttachItemID) > 0 )
				{
					std::wstring				wstrAttachItemID(wszAttachItemID);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemID, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iItemID = boost::lexical_cast<int>(vSplit[i]);
					}
				}
				if( wcslen(wszAttachItemCount) > 0 )
				{
					std::wstring				wstrAttachItemCount(wszAttachItemCount);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemCount, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iCount = boost::lexical_cast<int>(vSplit[i]);
					}
				}

				++pA->cCount;

				if( pA->cCount >= _countof(pA->Fields) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetListFieldForCharacter( TQGetListFieldForCharacter* pQ, TAGetListFieldForCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListFieldForCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListFieldForCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFieldForCharacter(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TFarmFieldForCharacter Record;
			memset( &Record, 0, sizeof(Record) );

			WCHAR wszAttachItemID[1024];
			WCHAR wszAttachItemCount[1024];

			TIMESTAMP_STRUCT UpdateDate = { 0, };

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Record.nFieldIndex, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iElapsedTimeSec, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &UpdateDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemID, sizeof(wszAttachItemID), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemCount, sizeof(wszAttachItemCount), &cblen );
			CheckColumnCount(nColNo, "P_GetListFieldForCharacter");
			while(1)
			{
				memset( &Record, 0, sizeof(Record) );
				memset( &UpdateDate, 0, sizeof(UpdateDate) );
				memset( wszAttachItemID, 0, sizeof(wszAttachItemID) );
				memset( wszAttachItemCount, 0, sizeof(wszAttachItemCount) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Fields[pA->cCount] = Record;
				CTimeParamSet UpdateTime(&QueryTimeLog, pA->Fields[pA->cCount].tUpdateDate, UpdateDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!UpdateTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}

				// AttachItem
				if( wcslen(wszAttachItemID) > 0 )
				{
					std::wstring				wstrAttachItemID(wszAttachItemID);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemID, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iItemID = boost::lexical_cast<int>(vSplit[i]);
					}
				}
				if( wcslen(wszAttachItemCount) > 0 )
				{
					std::wstring				wstrAttachItemCount(wszAttachItemCount);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemCount, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iCount = boost::lexical_cast<int>(vSplit[i]);
					}
				}

				++pA->cCount;

				if( pA->cCount >= _countof(pA->Fields) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryAddFieldForCharacter( TQAddField* pQ, TAAddField* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddFieldForCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	std::string strAttachItemID,strAttachItemCount;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if( pQ->iAttachItemID > 0 && pQ->AttachItems[0].bRemoveItem )
	{
		strAttachItemID.append(boost::lexical_cast<std::string>(pQ->iAttachItemID));
		strAttachItemCount.append(boost::lexical_cast<std::string>(1));
	}
	for(int i=0;i<pQ->cAttachCount;i++)
	{
		if( !pQ->AttachItems[i].bRemoveItem )
		{	
			if(i>0)
			{
				strAttachItemID.append(",");
				strAttachItemCount.append(",");
			}
			strAttachItemID.append(boost::lexical_cast<std::string>(pQ->AttachItems[i].biSerial));
			strAttachItemCount.append(boost::lexical_cast<std::string>(1));
		}
	}
#else
	if( pQ->iAttachItemID > 0 )
	{
		strAttachItemID.append(boost::lexical_cast<std::string>(pQ->iAttachItemID));
		strAttachItemCount.append(boost::lexical_cast<std::string>(1));
	}
#endif
	

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddFieldForCharacter(%I64d,%d,%d,%d,'%S','%S',%d)}", pQ->biCharacterDBID, pQ->nFieldIndex, pQ->iItemID, pQ->iElapsedTimeSec, strAttachItemID.c_str(), strAttachItemCount.c_str(), pQ->iMaxFieldCount );

	return CommonReturnValueQuery(m_wszQuery);	
}

int CDNSQLWorld::QueryDelFieldForCharacter( TQDelFieldForCharacter* pQ, TADelFieldForCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_DelFieldForCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelFieldForCharacter(%I64d,%d)}", pQ->biCharacterDBID, pQ->nFieldIndex );

	return CommonReturnValueQuery(m_wszQuery);	
}

int CDNSQLWorld::QueryAddFieldForCharacterAttachment( TQAddFieldAttachment* pQ, TAAddFieldAttachment* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddFieldForCharacterAttachment" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddFieldForCharacterAttachment(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->nFieldIndex, pQ->iAttachItemID );

	return CommonReturnValueQuery(m_wszQuery);	
}

int CDNSQLWorld::QueryModFieldForCharacterElapsedTime( TQModFieldForCharacterElapsedTime* pQ, TAModFieldForCharacterElapsedTime* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModFieldForCharacterElapsedTime" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	std::string strFieldIndex,strElapsedTimeSec;
	for( int i=0 ; i<pQ->cCount ; ++i )
	{
		if( !strFieldIndex.empty() )
			strFieldIndex.append(",");
		if( !strElapsedTimeSec.empty() )
			strElapsedTimeSec.append(",");
		strFieldIndex.append(boost::lexical_cast<std::string>(pQ->Updates[i].nFieldIndex));
		strElapsedTimeSec.append(boost::lexical_cast<std::string>(pQ->Updates[i].iElapsedTimeSec));
	}

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFieldForCharacterElapsedTime(%I64d,'%S','%S')}", pQ->biCharacterDBID, strFieldIndex.c_str(), strElapsedTimeSec.c_str() );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryHarvestForCharacter( TQHarvest* pQ, TAHarvest* pA )
{
	CQueryTimeLog QueryTimeLog( "P_HarvestForCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryHarvestForCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[Farm::Max::HARVESTITEM_COUNT];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_HarvestForCharacter2(%I64d,%d,	?,%d,%d,%d,%d,%d,%d,%d,	?,%d,%d,%d,%d,%d,%d,%d,	?,%d,%d,%d,%d,%d,%d,%d,	%d, '%S') }"
		, pQ->biCharacterDBID, pQ->nFieldIndex
		, pQ->HarvestItems[0].iItemID, pQ->HarvestItems[0].nItemCount, pQ->HarvestItems[0].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[0].cSealCount), static_cast<int>(pQ->HarvestItems[0].cItemOption), pQ->HarvestItems[0].bEternityFlag, pQ->HarvestItems[0].iLifeSpan
		, pQ->HarvestItems[1].iItemID, pQ->HarvestItems[1].nItemCount, pQ->HarvestItems[1].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[1].cSealCount), static_cast<int>(pQ->HarvestItems[1].cItemOption), pQ->HarvestItems[1].bEternityFlag, pQ->HarvestItems[1].iLifeSpan
		, pQ->HarvestItems[2].iItemID, pQ->HarvestItems[2].nItemCount, pQ->HarvestItems[2].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[2].cSealCount), static_cast<int>(pQ->HarvestItems[2].cItemOption), pQ->HarvestItems[2].bEternityFlag, pQ->HarvestItems[2].iLifeSpan
		, pQ->iMapID, pQ->szIP );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		for( int i=0 ;i<Farm::Max::HARVESTITEM_COUNT ; ++i )
		{
			SQLBindParameter(m_hstmt, 2+i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->HarvestItems[i].biItemSerial, sizeof(INT64), SqlLen[i].GetNull(pQ->HarvestItems[i].biItemSerial <= 0, sizeof(INT64)));
		}

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListFieldByCharacter( TQGetListFieldByCharacter* pQ, TAGetListFieldByCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListFieldByCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListFieldByCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListFieldByCharacter(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TFarmFieldPartial Record;
			memset( &Record, 0, sizeof(Record) );

			WCHAR wszAttachItemID[1024];
			WCHAR wszAttachItemCount[1024];
			memset( wszAttachItemID, 0, sizeof(wszAttachItemID) );
			memset( wszAttachItemCount, 0, sizeof(wszAttachItemCount) );

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iFarmDBID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Record.nFieldIndex, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.iElapsedTimeSec, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemID, sizeof(wszAttachItemID), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_WCHAR, wszAttachItemCount, sizeof(wszAttachItemCount), &cblen );
			CheckColumnCount(nColNo, "P_GetListFieldByCharacter");
			while(1)
			{
				memset( &Record, 0, sizeof(Record) );
				memset( wszAttachItemID, 0, sizeof(wszAttachItemID) );
				memset( wszAttachItemCount, 0, sizeof(wszAttachItemCount) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Fields[pA->cCount] = Record;
				// AttachItem
				if( wcslen(wszAttachItemID) > 0 )
				{
					std::wstring				wstrAttachItemID(wszAttachItemID);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemID, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iItemID = boost::lexical_cast<int>(vSplit[i]);
					}
				}
				if( wcslen(wszAttachItemCount) > 0 )
				{
					std::wstring				wstrAttachItemCount(wszAttachItemCount);
					std::vector<std::wstring>	vSplit;
					boost::algorithm::split( vSplit, wstrAttachItemCount, boost::algorithm::is_any_of(",") );

					for( UINT i=0 ; i<vSplit.size() ; ++i )
					{
						if( i >= Farm::Max::ATTACHITEM_KIND )
						{
							_ASSERT(0);
							break;
						}

						pA->Fields[pA->cCount].AttachItems[i].iCount = boost::lexical_cast<int>(vSplit[i]);
					}
				}
				++pA->cCount;

				if( pA->cCount >= _countof(pA->Fields) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryAddField( TQAddField* pQ, TAAddField* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddField" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	std::wstring wstrAttachItem;	
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if( pQ->iAttachItemID > 0 && pQ->AttachItems[0].bRemoveItem )
		wstrAttachItem.append(boost::lexical_cast<std::wstring>(pQ->iAttachItemID));
	for(int i=0;i<pQ->cAttachCount;i++)
	{
		if( !pQ->AttachItems[i].bRemoveItem )
		{	
			if(i>0)
				wstrAttachItem.append(L",");
			wstrAttachItem.append(boost::lexical_cast<std::wstring>(pQ->AttachItems[i].biSerial));
		}
	}
#else
	if( pQ->iAttachItemID > 0 )
		wstrAttachItem.append(boost::lexical_cast<std::wstring>(pQ->iAttachItemID));
#endif

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddField(%d,%d,%I64d,%d,%d,N'%s',%d)}", pQ->iFarmDBID, pQ->nFieldIndex, pQ->biCharacterDBID, pQ->iItemID, pQ->iElapsedTimeSec, wstrAttachItem.c_str(), pQ->iMaxFieldCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelField( TQDelField* pQ, TADelField* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddField" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelField(%d,%d)}", pQ->iFarmDBID, pQ->nFieldIndex );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddFieldAttachment( TQAddFieldAttachment* pQ, TAAddFieldAttachment* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddFieldAttachment" );

	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddFieldAttachment(%d,%d,%d)}", pQ->iFarmDBID, pQ->nFieldIndex, pQ->iAttachItemID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModFieldElapsedTime( TQModFieldElapsedTime* pQ, TAModFieldElapsedTime* pA )
{
	CQueryTimeLog QueryTimeLog( "P_ModFieldElapsedTime" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	std::string strFieldIndex,strElapsedTimeSec;
	for( int i=0 ; i<pQ->cCount ; ++i )
	{
		if( !strFieldIndex.empty() )
			strFieldIndex.append(",");
		if( !strElapsedTimeSec.empty() )
			strElapsedTimeSec.append(",");
		strFieldIndex.append(boost::lexical_cast<std::string>(pQ->Updates[i].nFieldIndex));
		strElapsedTimeSec.append(boost::lexical_cast<std::string>(pQ->Updates[i].iElapsedTimeSec));
	}

	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModFieldElapsedTime(%d,'%S','%S')}", pQ->iFarmDBID, strFieldIndex.c_str(), strElapsedTimeSec.c_str() );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryHarvest( TQHarvest* pQ, TAHarvest* pA )
{
	CQueryTimeLog QueryTimeLog( "P_Harvest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryHarvest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[Farm::Max::HARVESTITEM_COUNT];

	int nResult = ERROR_DB, sqlparam = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_Harvest2(%d,%d,%I64d,%I64d,	?,%d,%d,%d,%d,%d,%d,%d,	?,%d,%d,%d,%d,%d,%d,%d,	?,%d,%d,%d,%d,%d,%d,%d,	%d, '%S') }"
		, pQ->iFarmDBID, pQ->nFieldIndex, pQ->biCharacterDBID, pQ->biHarvestCharacterDBID
		, pQ->HarvestItems[0].iItemID, pQ->HarvestItems[0].nItemCount, pQ->HarvestItems[0].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[0].cSealCount), static_cast<int>(pQ->HarvestItems[0].cItemOption), pQ->HarvestItems[0].bEternityFlag, pQ->HarvestItems[0].iLifeSpan
		, pQ->HarvestItems[1].iItemID, pQ->HarvestItems[1].nItemCount, pQ->HarvestItems[1].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[1].cSealCount), static_cast<int>(pQ->HarvestItems[1].cItemOption), pQ->HarvestItems[1].bEternityFlag, pQ->HarvestItems[1].iLifeSpan
		, pQ->HarvestItems[2].iItemID, pQ->HarvestItems[2].nItemCount, pQ->HarvestItems[2].bSoulBoundFlag, static_cast<int>(pQ->HarvestItems[2].cSealCount), static_cast<int>(pQ->HarvestItems[2].cItemOption), pQ->HarvestItems[2].bEternityFlag, pQ->HarvestItems[2].iLifeSpan
		, pQ->iMapID, pQ->szIP );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		for( int i=0 ;i<Farm::Max::HARVESTITEM_COUNT ; ++i )
		{
			SQLBindParameter(m_hstmt, 2+i, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pQ->HarvestItems[i].biItemSerial, sizeof(INT64), SqlLen[i].GetNull(pQ->HarvestItems[i].biItemSerial <= 0, sizeof(INT64)));
		}

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListHarvestDepotItem( TQGetListHarvestDepotItem* pQ, TAGetListHarvestDepotItem* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListHarvestDepotItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListHarvestDepotItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListHarvestDepotItem2(%I64d,%I64d,%d)}", pQ->biCharacterDBID, pQ->biLastUniqueID, Farm::Max::HARVESTDEPOT_COUNT);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TIMESTAMP_STRUCT MakeDate = { 0, }, ExpireDate = { 0, };

			TFarmWareHouseItem Record;
			memset( &Record, 0, sizeof(Record) );

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &Record.biUniqueID, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT, &Record.nSerial, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.nItemID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Record.wCount, sizeof(short), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &Record.wDur, sizeof(USHORT), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.nRandomSeed, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&Record.nCoolTime, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cLevel, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cPotential, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT, &Record.bSoulbound, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cOption, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &Record.cSealCount, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol( m_hstmt, nColNo++, SQL_BIT, &Record.bEternity, sizeof(bool), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListHarvestDepotItem");
			while(1)
			{
				memset( &Record, 0, sizeof(Record) );
				memset( &MakeDate, 0, sizeof(MakeDate) );
				memset( &ExpireDate, 0, sizeof(ExpireDate) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Items[pA->cCount] = Record;
				if( !Record.bEternity)
				{

					CTimeParamSet ExpireTime(&QueryTimeLog, pA->Items[pA->cCount].tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID);
					if (!ExpireTime.IsValid())
					{
						nResult = ERROR_DB;
						break;
					}
				}
				else 
					pA->Items[pA->cCount].tExpireDate = 0;

				CTimeParamSet MakeTime(&QueryTimeLog, pA->Items[pA->cCount].tMaterializeDate, MakeDate, pQ->cWorldSetID, pQ->nAccountDBID);
				if (!MakeTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}
				++pA->cCount;

				if( pA->cCount >= _countof(pA->Items) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetCountHarvestDepotItem( TQGetCountHarvestDepotItem* pQ, TAGetCountHarvestDepotItem* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetCountHarvestDepotItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetCountHarvestDepotItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetCountHarvestDepotItem2(%I64d,%d)}", pQ->biCharacterDBID, Farm::Max::WAREHOUSE_DEFAULT_KEEPING_DAY );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->iCount = 0;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&pA->iCount, sizeof(int), &cblen );
			CheckColumnCount(nColNo, "P_GetCountHarvestDepotItem");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				// 해당 SP 는 RecordSet Count 만 반환하므로 바로 return 한다.
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryGetFieldCountByCharacter( TQGetFieldCountByCharacter* pQ, TAGetFieldCountByCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetFieldCountByCharacter" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetFieldCountByCharacter] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetFieldCountByCharacter(%I64d,?,?,?)}", pQ->biCharacterDBID );

	int iSQLResult = ERROR_DB;
	int iFieldCount = 0;
	int iFieldCountForCharacter = 0;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN		cblen;
		CDNSqlLen	SqlLen;
		int			iFarmID = 0;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iFarmID, sizeof(int), SqlLen.GetNull(true,sizeof(int)) );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iFieldCount, sizeof(int), &cblen );
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iFieldCountForCharacter, sizeof(int), &cblen );

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	if( iSQLResult == ERROR_NONE )
	{
		pA->iFieldCount = iFieldCount+iFieldCountForCharacter;
	}
	return iSQLResult;
}

int CDNSQLWorld::QueryGetFieldItemCount( TQGetFieldItemCount* pQ, TAGetFieldItemCount* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetFieldItemCount" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetFieldItemCount] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetFieldItemCount(%I64d,%d,?,?)}", pQ->biCharacterDBID, pQ->iItemID );

	int iSQLResult = ERROR_DB;
	int iItemCount = 0;
	int iItemCountForCharacter = 0;

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN		cblen;
		CDNSqlLen	SqlLen;
		int			iFarmID = 0;

		int nNo = 1;
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iItemCount, sizeof(int), &cblen );	
		SQLBindParameter( m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iItemCountForCharacter, sizeof(int), &cblen );	

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}
	if( iSQLResult == ERROR_NONE )
	{
		pA->iItemCount = iItemCount+iItemCountForCharacter;
	}
	return iSQLResult;
}

int CDNSQLWorld::QueryDeletePeriodQuestList(TQDeletePeriodQuest* pQ, TADeletePeriodQuest* pA)
{
	CQueryTimeLog QueryTimeLog( "P_DelAssignedPeriodicQuest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryDeletePeriodQuestList] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	std::wstring wQuest;

	for(int i=0; i<pQ->nQuestCount; i++)
	{
		if(i>0)
			wQuest.append(L",");
		
		wQuest.append(boost::lexical_cast<std::wstring>(pQ->nQuestIDs[i]));
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelAssignedPeriodicQuest(%I64d,%d,N'%s',?)}", 
		pQ->biCharacterDBID,		// 캐릭터 ID
		pQ->nPeriodQuestType,		// 삭제하는 주기형 퀘스트의 주기
		wQuest.c_str());

	SQL_TIMESTAMP_STRUCT AssignDate = {0, };
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &AssignDate, sizeof(TIMESTAMP_STRUCT), &cblen);

		RetCode = SQLExecute( m_hstmt );
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			CTimeParamSet AssignTime(&QueryTimeLog, pA->tAssignDate, AssignDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
			if (!AssignTime.IsValid()) return ERROR_DB;
		}
	}

	return nResult;
}

int CDNSQLWorld::QueryGetPeriodQuestDate(TQGetPeriodQuestDate* pQ, TAGetPeriodQuestDate* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListQuestAssignDate" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetPeriodQuestDate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListQuestAssignDate(%I64d)}", pQ->biCharacterDBID);

	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->nCount = 0;

			BYTE cPeriodType;
			TIMESTAMP_STRUCT AssginDate = { 0, };

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cPeriodType, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &AssginDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListQuestAssignDate");
			while(1)
			{
				memset(&AssginDate, 0, sizeof(AssginDate));
				cPeriodType = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				if( pA->nCount >= PERIODQUEST_RESET_MAX )	
					break;

				pA->PeriodQuestDate[pA->nCount].nPeriodType = cPeriodType;
				CTimeParamSet AssignTime(&QueryTimeLog, pA->PeriodQuestDate[pA->nCount].tAssignDate, AssginDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!AssignTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}
				pA->nCount++;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetListCompleteEventQuest(TQGetListCompleteEventQuest * pQ, TAGetListCompleteEventQuest * pA)
{
	CQueryTimeLog QueryTimeLog( "QueryGetListCompleteEventQuest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetPeriodQuestDate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListCompleteEventQuest(%I64d)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->nCount = 0;

			int nScheduleID;
			short wCompleteCount;
			
			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_INTEGER,	&nScheduleID, sizeof(int), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_SMALLINT, &wCompleteCount, sizeof(short), &cblen );
			CheckColumnCount(nColNo, "P_GetListCompleteEventQuest");
			while(1)
			{
				nScheduleID = 0;
				wCompleteCount = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				if( pA->nCount >= EVENTQUESTMAX )	
					break;

				pA->EventQuestList[pA->nCount].nScheduleID = nScheduleID;
				pA->EventQuestList[pA->nCount].wCompleteCount = wCompleteCount;
				
				pA->nCount++;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;

}


int CDNSQLWorld::QueryCompleteEventQuest(TQCompleteEventQuest* pQ, TACompleteEventQuest* pA)
{
	CQueryTimeLog QueryTimeLog( "QueryCompleteEventQuest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryCompleteEventQuest] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	TIMESTAMP_STRUCT _tExpireDate;
	CTimeSet TimeSet(pQ->tExpireDate, true);
	memcpy(&_tExpireDate, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT));

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_CompleteEventQuest(%I64d,%d,?,?)}", pQ->biCharacterDBID, pQ->nScheduleID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT,  SQL_C_TIMESTAMP, SQL_TIMESTAMP,	SQL_TIMESTAMP_LEN, 0, &_tExpireDate, sizeof(TIMESTAMP_STRUCT), SqlLen.Get(SQL_TIMESTAMP_LEN));
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_SSHORT, SQL_SMALLINT, sizeof(short), 0, &pA->wCompleteCount, sizeof(short), &cblen);	

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetWorldEventQuestCounter(TQGetWorldEventQuestCounter* pQ, TAGetWorldEventQuestCounter* pA)
{
	CQueryTimeLog QueryTimeLog( "QueryGetWorldEventQuestCounter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetWorldEventQuestCounter] Check Connect Fail\r\n");
		return ERROR_DB;
	}


	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	INT64 biCharacterDBID = 0;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGlobalEventEntryCount(?,%d,?)}", pQ->nScheduleID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &biCharacterDBID, sizeof(INT64), SqlLen.GetNull(biCharacterDBID <= 0, sizeof(INT64)));	
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCount, sizeof(int), &cblen);
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryModWorldEventQuestCounter(TQModWorldEventQuestCounter* pQ, TAModWorldEventQuestCounter* pA)
{
	CQueryTimeLog QueryTimeLog( "QueryModWorldEventQuestCounter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryModWorldEventQuestCounter] Check Connect Fail\r\n");
		return ERROR_DB;
	}


	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGlobalEventEntryCount(%I64d,%d,%d,?)}", pQ->biCharacterDBID, pQ->nScheduleID, pQ->nCounterDelta);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nCountAfter, sizeof(int), &cblen);
		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetConnectDuration(INT64 biCharacterDBID, TTimeEventGroup &TimeEventGroup)
{
	CQueryTimeLog QueryTimeLog("P_GetConnectDuration");

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryGetConnectDuration] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nSPVersion = 5;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetConnectDuration(%I64d,%d)}", biCharacterDBID, nSPVersion);

	int nCount = 0;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){
			TTimeEvent Event = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Event.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Event.nRemainTime, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Event.bCheckFlag, sizeof(bool), &cblen);
			CheckColumnCount(nColNo, "P_GetConnectDuration");
			while(1)
			{
				memset(&Event, 0, sizeof(Event));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA){
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) break;

				TimeEventGroup.Event[nCount] = Event;
				nCount++;

				if(nCount >= TIMEEVENTMAX){
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QuerySaveConnectDurationTime(TQSaveConnectDurationTime *pQ)
{
	CQueryTimeLog QueryTimeLog("P_SaveConnectDurationTime");

	std::wstring wItemIDStr, wRemainTimeStr, wCheckFlagStr;

	for(int i = 0; i < pQ->cCount; i++){
		if(pQ->Event[i].nItemID > 0){
			if(!wItemIDStr.empty()){
				wItemIDStr.append(L",");
				wRemainTimeStr.append(L",");
				wCheckFlagStr.append(L",");
			}
			wItemIDStr.append(boost::lexical_cast<std::wstring>(pQ->Event[i].nItemID));
			wRemainTimeStr.append(boost::lexical_cast<std::wstring>(pQ->Event[i].nRemainTime));
			wCheckFlagStr.append(boost::lexical_cast<std::wstring>(pQ->Event[i].bCheckFlag));
		}
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_SaveConnectDurationTime(%I64d,N'%s',N'%s',N'%s')}", pQ->biCharacterDBID, wItemIDStr.c_str(), wRemainTimeStr.c_str(), wCheckFlagStr.c_str());

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryModTimeEventDate( TQModTimeEventDate* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModTimeEventDate" );

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModTimeEventDate(%I64d)}", pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
int CDNSQLWorld::QueryChangeJobCode( INT64 biCharacterDBID, int nCannelID, int nMapID, int nFirstJobIDBefore, int nFirstJobIDAfter, 
								     int nSecondJobIDBefore, int nSecondJobIDAfter, int nJobCodeChangeCode, OUT USHORT& wSkillPoint, OUT USHORT& wTotalSkillPoint )
{
	CQueryTimeLog QueryTimeLog( "P_ChangeJobCode" );
	
	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log(LogType::_ERROR, 0, 0, biCharacterDBID, 0, L"[QueryChangeJobCodes] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen1, SqlLen2, SqlLen3, SqlLen4;

	int nResult = ERROR_DB;
	memset( &m_wszQuery, 0, sizeof(m_wszQuery) );

	int nSkillCode = 3;	//3번이면 1,2번 페이지 스킬 모두 리셋
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ChangeJobCode(%I64d,?,?,?,?,%d,%d,%d,?,?,%d,?)}", biCharacterDBID, nJobCodeChangeCode, nCannelID, nMapID, nSkillCode);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		
		int nPrmNo = 1;

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nFirstJobIDBefore, sizeof(int), SqlLen1.GetNull(nFirstJobIDBefore <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nFirstJobIDAfter, sizeof(int), SqlLen2.GetNull(nFirstJobIDAfter <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSecondJobIDBefore, sizeof(int), SqlLen3.GetNull(nSecondJobIDBefore <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nSecondJobIDAfter, sizeof(int), SqlLen4.GetNull(nSecondJobIDAfter <= 0, sizeof(int)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(short), 0, &wSkillPoint, sizeof(short), &cblen);	
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(short), 0, &wTotalSkillPoint, sizeof(short), &cblen);	
		USHORT wSkillPoint2 = 0;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_SMALLINT, SQL_SMALLINT, sizeof(short), 0, &wSkillPoint2, sizeof(short), &cblen);	
	
		RetCode = SQLExecute( m_hstmt );
		CheckRetCode(RetCode, L"SQLExecute");

	}

	return nResult;
}
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

int CDNSQLWorld::QueryGetListVariableReset( TQGetListVariableReset* pQ, TAGetListVariableReset* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPeriodicReset" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListVariableReset] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN	RetCode;
	SQLLEN		cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPeriodicReset(%I64d)}", pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){

		int nPrmNo = 1;
		SQLBindParameter( m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TIMESTAMP_STRUCT LastModifyDate = { 0, };

			BYTE cCode = 0;
			INT64 biValue = 0;

			int nColNo = 1;
			SQLBindCol( m_hstmt, nColNo++, SQL_TINYINT, &cCode, sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_C_SBIGINT,	&biValue, sizeof(INT64), &cblen );
			SQLBindCol( m_hstmt, nColNo++, SQL_TIMESTAMP, &LastModifyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListPeriodicReset");
			while(1)
			{
				memset(&LastModifyDate, 0, sizeof(LastModifyDate));
				cCode = 0;
				biValue = 0;

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->Data[pA->cCount].Type = static_cast<CommonVariable::Type::eCode>(cCode);
				pA->Data[pA->cCount].biValue = biValue;

				CTimeParamSet ModifyTime(&QueryTimeLog, pA->Data[pA->cCount].tLastModifyDate, LastModifyDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!ModifyTime.IsValid())
				{
					nResult = ERROR_DB;
					break;
				}
				++pA->cCount;

				if( pA->cCount >= _countof(pA->Data) )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryModVariableReset( TQModVariableReset* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModPeriodicReset" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryModVariableReset] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;

	int nResult = ERROR_DB, sqlparam = 0, nRestraintID = 0;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModPeriodicReset(%I64d,%d,%I64d,?)}", pQ->biCharacterDBID, pQ->Type, pQ->biValue );
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		TIMESTAMP_STRUCT tResetTime;
		CTimeSet TimeSet(pQ->tResetTime, true);
		memcpy(&tResetTime, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT));

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int),	&cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT,  SQL_C_TIMESTAMP, SQL_TIMESTAMP,	SQL_TIMESTAMP_LEN, 0, &tResetTime, sizeof(TIMESTAMP_STRUCT), SqlLen.Get(SQL_TIMESTAMP_LEN) );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	return nResult;
}

#if defined(PRE_ADD_HEARTBEAT_TO_SQL)
int CDNSQLWorld::QueryHeartbeat()
{
	CQueryTimeLog QueryTimeLog( "P_HeartBeat" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_HeartBeat}");

	return CommonReturnValueQuery(m_wszQuery);
}
#endif		//#if defined(PRE_ADD_HEARTBEAT_TO_SQL)

int CDNSQLWorld::QueryGetListRepurchaseItem( TQGetListRepurchaseItem* pQ, TAGetListRepurchaseItem* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetListRepurchaseItem" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetListRepurchaseItem] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 8;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListRepurchaseItem(%I64d, %d)}", pQ->biCharacterDBID, nVersion );

	int iSQLResult = ERROR_DB;
	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &iSQLResult, sizeof(int), &cblen );	
		RetCode = SQLExecute( m_hstmt );
		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			TRepurchaseItemInfo ItemInfo = { 0, };
			TIMESTAMP_STRUCT SellDate = { 0, }, MakeDate = { 0, }, ExpireDate = { 0, };
			BYTE cItemLocationCode = 0;
			BYTE cSlotIndex =0;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&cItemLocationCode, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&cSlotIndex, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &ItemInfo.Item.nSerial, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &ItemInfo.Item.wCount,	sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &ItemInfo.Item.wDur, sizeof(USHORT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nRandomSeed, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER,	&ItemInfo.Item.nCoolTime, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cLevel, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cPotential, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &ItemInfo.Item.bSoulbound, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cOption, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT,	&ItemInfo.Item.cSealCount, sizeof(char), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &MakeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &ItemInfo.Item.bEternity, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &ExpireDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.Item.nLookItemID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.iRepurchaseID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &SellDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &ItemInfo.iSellPrice, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &ItemInfo.Item.cPotentialMoveCount, sizeof(char), &cblen);

			CheckColumnCount(nNo, "P_GetListRepurchaseItem");
			while(1)
			{
				memset(&ItemInfo, 0, sizeof(ItemInfo));
				memset(&SellDate, 0, sizeof(SellDate));
				memset(&MakeDate, 0, sizeof(MakeDate));
				memset(&ExpireDate, 0, sizeof(ExpireDate));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					iSQLResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO) ) 
					break;

				pA->ItemList[pA->cCount] = ItemInfo;
				if (!ItemInfo.Item.bEternity)
				{
					CTimeParamSet ExpireTime(&QueryTimeLog, pA->ItemList[pA->cCount].Item.tExpireDate, ExpireDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
					if (!ExpireTime.IsValid())
					{
						iSQLResult = ERROR_DB;
						break;
					}
				}
				else
					pA->ItemList[pA->cCount].Item.tExpireDate = 0;

				CTimeParamSet SellTime(&QueryTimeLog, pA->ItemList[pA->cCount].tSellDate, SellDate, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID);
				if (!SellTime.IsValid()) return ERROR_DB;

				pA->cCount++;

				if( pA->cCount >= _countof(pA->ItemList) )
				{
					iSQLResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return iSQLResult;
}

int CDNSQLWorld::QueryGetPageGuildRecruit( TQGetGuildRecruit* pQ, TAGetGuildRecruit* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPageGuildRecruitment" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetPageGuildRecruit] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )	
	int nVersion = 20;
	if(pQ->cSortType <= 0)
		pQ->cSortType = GuildRecruitSystem::SortType::GuildLevelDesc;
#endif

	int nResult = ERROR_DB;	

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageGuildRecruitment(%d,%d,?,?,%d,?,N'%s',?)}",(pQ->uiPage*GuildRecruitSystem::Max::GuildRecruitRowCount)+1,((pQ->uiPage+1)*GuildRecruitSystem::Max::GuildRecruitRowCount), nVersion, pQ->wszGuildName );
#else
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageGuildRecruitment(%d,%d,?,?)}",(pQ->uiPage*GuildRecruitSystem::Max::GuildRecruitRowCount)+1,((pQ->uiPage+1)*GuildRecruitSystem::Max::GuildRecruitRowCount));
#endif

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;

#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		CDNSqlLen SqlLen1,SqlLen2,SqlLen3,SqlLen4;
#else
		CDNSqlLen SqlLen1,SqlLen2;
#endif

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cJobCode, sizeof(BYTE), SqlLen1.GetNull(pQ->cJobCode<= 0, sizeof(BYTE)));
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cLevel, sizeof(BYTE), SqlLen2.GetNull(pQ->cLevel<= 0, sizeof(BYTE)));
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cPurposeCode, sizeof(BYTE), SqlLen3.Get(sizeof(BYTE)));		
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_INPUT, SQL_C_TINYINT, SQL_C_TINYINT, sizeof(BYTE), 0, &pQ->cSortType, sizeof(BYTE), SqlLen4.Get(sizeof(BYTE)));
#endif

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TGuildRecruitInfo GuildRecruitInfo;
			memset( &GuildRecruitInfo, 0, sizeof(GuildRecruitInfo) );

			int nColNo = 1;
			
			GuildRecruitInfo.GuildUID.nWorldID = pQ->cWorldSetID;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildRecruitInfo.GuildUID.nDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildName, sizeof(GuildRecruitInfo.wszGuildName), &cblen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildLevel, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMark, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMarkBG, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMarkBorder, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildRecruitNotice, sizeof(WCHAR)*GUILDRECRUITINTRODUCE, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMemberPresentSize, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMemberMaxSize, sizeof(short), &cblen);
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildRecruitInfo.cPurposeCode, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildHomePage, sizeof(WCHAR)*GUILDHISTORYTEXT_MAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildMasterName, sizeof(WCHAR)*NAMELENMAX, &cblen);
#endif
			CheckColumnCount(nColNo, "P_GetPageGuildRecruitment");
			while(1)
			{
				memset( &GuildRecruitInfo, 0, sizeof(GuildRecruitInfo) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->GuildRecruitList[pA->cCount] = GuildRecruitInfo;
				++pA->cCount;

				if( pA->cCount >= GuildRecruitSystem::Max::GuildRecruitRowCount )
				{
					nResult = ERROR_NONE;
					break;
				}
			}			
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
int CDNSQLWorld::QueryGetPageGuildRecruitCharacter( TQGetGuildRecruitCharacter* pQ, TAGetGuildRecruitCharacter* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetPageGuildJoinCharactersForGuild" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetPageGuildRecruit] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageGuildJoinCharactersForGuild(%d,%d,%d)}", 1, 1*GuildRecruitSystem::Max::GuildRecruitCharacterRowCount, pQ->nGuildDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TGuildRecruitCharacter GuildRecruitCharacter;
			memset( &GuildRecruitCharacter, 0, sizeof(GuildRecruitCharacter) );

			int nColNo = 1;
			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildRecruitCharacter.nCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildRecruitCharacter.cLevel, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildRecruitCharacter.nJob, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitCharacter.wszCharacterName, sizeof(WCHAR)*NAMELENMAX, &cblen);
			CheckColumnCount(nColNo, "P_GetPageGuildJoinCharactersForGuild");
			while(1)
			{
				memset( &GuildRecruitCharacter, 0, sizeof(GuildRecruitCharacter) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;


				pA->GuildRecruitCharacterList[pA->cCount] = GuildRecruitCharacter;
				++pA->cCount;

				if( pA->cCount >= GuildRecruitSystem::Max::GuildRecruitCharacterRowCount )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
int CDNSQLWorld::QueryMyGuildRecruit( TQGetMyGuildRecruit* pQ, TAGetMyGuildRecruit* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildJoinCharactersForCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryMyGuildRecruit] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nVerSion = 13;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	nVerSion = 14;
#endif

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildJoinCharactersForCharacter(%d, %d)}", pQ->biCharacterDBID, nVerSion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			pA->cCount = 0;

			TGuildRecruitInfo GuildRecruitInfo;
			memset( &GuildRecruitInfo, 0, sizeof(GuildRecruitInfo) );

			int nColNo = 1;

			GuildRecruitInfo.GuildUID.nWorldID = pQ->cWorldSetID;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildRecruitInfo.GuildUID.nDBID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildName, sizeof(GuildRecruitInfo.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildLevel, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMark, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMarkBG, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMarkBorder, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildRecruitNotice, sizeof(WCHAR)*GUILDRECRUITINTRODUCE, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMemberPresentSize, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &GuildRecruitInfo.wGuildMemberMaxSize, sizeof(short), &cblen);
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildRecruitInfo.cPurposeCode, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildHomePage, sizeof(WCHAR)*GUILDHISTORYTEXT_MAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &GuildRecruitInfo.wszGuildMasterName, sizeof(WCHAR)*NAMELENMAX, &cblen);
#endif
			CheckColumnCount(nColNo, "P_GetGuildJoinCharactersForCharacter");
			while(1)
			{
				memset( &GuildRecruitInfo, 0, sizeof(GuildRecruitInfo) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;


				pA->GuildRecruitList[pA->cCount] = GuildRecruitInfo;
				++pA->cCount;

				if( pA->cCount >= GuildRecruitSystem::Max::MaxRequestCount )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
int CDNSQLWorld::QueryGuildRecruitRequestCount( TQGetGuildRecruitRequestCount* pQ, TAGetGuildRecruitRequestCount* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildJoinCharacterCount" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGuildRecruitRequestCount] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildJoinCharacterCount(%d)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;

			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cCount, sizeof(byte), &cblen);
			CheckColumnCount(nColNo, "P_GetGuildJoinCharacterCount");
			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA )
			{
				SQLCloseCursor(m_hstmt);
				return nResult;
			}				
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
int CDNSQLWorld::QueryRegisterInofGuildRecruit( TQRegisterInfoGuildRecruit* pQ, TARegisterInfoGuildRecruit* pA )
{
	CQueryTimeLog QueryTimeLog( "P_GetGuildRecruitment" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryRegisterInofGuildRecruit] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;	
	int nVersion = 5;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
	nVersion = 6;
#endif
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildRecruitment(%d, %d)}", pQ->nGuildDBID, nVersion);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nCount = 0;
			BYTE nClassCode = 0;
			int nColNo = 1;			

			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &pA->wszGuildIntroduction, sizeof(pA->wszGuildIntroduction), &cblen);	
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nMinLevel, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nMaxLevel, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &nClassCode, sizeof(byte), &cblen);
#if defined( PRE_ADD_GUILD_EASYSYSTEM )
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &pA->cPurposeCode, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &pA->bCheckHomePage, sizeof(bool), &cblen);			
#endif
			CheckColumnCount(nColNo, "P_GetGuildRecruitment");
			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					if( pA->cClassCode[0] == 0 )
					{
						pA->nMinLevel = 1;
						pA->nMaxLevel = 100;
					}
					nResult = ERROR_NONE;
					break;
				}
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->cClassCode[nCount++] = nClassCode;
				if( nCount >= CLASSKINDMAX )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryRegisterOnGuildRecruit( TQRegisterOnGuildRecruit* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildRecruitment" );

	std::wstring wJobCode;
	for(int i = 0; i < CLASSKINDMAX; i++){
		if( pQ->cClassCode[i] > 0 ){
			if( !wJobCode.empty() ) 
				wJobCode.append(L",");
			wJobCode.append(boost::lexical_cast<std::wstring>(pQ->cClassCode[i]));			
		}
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	WCHAR wszGuildIntroduction[GUILDRECRUITINTRODUCE * 2 + 1] ={0,};
	ConvertQuery(pQ->wszGuildIntroduction, GUILDRECRUITINTRODUCE, wszGuildIntroduction, _countof(wszGuildIntroduction));	
	
	#if defined( PRE_ADD_GUILD_EASYSYSTEM )	
		swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildRecruitment(%d, N'%s', N'%s',%d,%d,%d,%d)}", pQ->nGuildDBID, wszGuildIntroduction, wJobCode.c_str(), pQ->nMinLevel, pQ->nMaxLevel, pQ->cPurposeCode, pQ->bCheckHomePage?1:0 );
	#else
		swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildRecruitment(%d, N'%s', N'%s',%d,%d)}", pQ->nGuildDBID, wszGuildIntroduction, wJobCode.c_str(), pQ->nMinLevel, pQ->nMaxLevel);
	#endif

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryRegisterModGuildRecruit( TQRegisterModGuildRecruit* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModGuildRecruitment" );

	std::wstring wJobCode;
	for(int i = 0; i < CLASSKINDMAX; i++){
		if( pQ->cClassCode[i] > 0 ){
			if( !wJobCode.empty() ) 
				wJobCode.append(L",");
			wJobCode.append(boost::lexical_cast<std::wstring>(pQ->cClassCode[i]));
		}
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	WCHAR wszGuildIntroduction[GUILDRECRUITINTRODUCE * 2 + 1] ={0,};
	ConvertQuery(pQ->wszGuildIntroduction, GUILDRECRUITINTRODUCE, wszGuildIntroduction, _countof(wszGuildIntroduction));

	#if defined( PRE_ADD_GUILD_EASYSYSTEM )
		swprintf( m_wszQuery, L"{?=CALL dbo.P_ModGuildRecruitment(%d, N'%s', N'%s',%d,%d,%d,%d)}", pQ->nGuildDBID, wszGuildIntroduction, wJobCode.c_str(), pQ->nMinLevel, pQ->nMaxLevel, pQ->cPurposeCode, pQ->bCheckHomePage?1:0 );
	#else
		swprintf( m_wszQuery, L"{?=CALL dbo.P_ModGuildRecruitment(%d, N'%s', N'%s',%d,%d)}", pQ->nGuildDBID, wszGuildIntroduction, wJobCode.c_str(), pQ->nMinLevel, pQ->nMaxLevel);
	#endif

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryRegisterOffGuildRecruit( TQRegisterOffGuildRecruit* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildRecruitment" );
	
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelGuildRecruitment(%d)}", pQ->nGuildDBID);

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryGuildRecruitRequestOn( TQGuildRecruitRequestOn* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddGuildJoinCharacters" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGuildJoinCharacters(%d, %lld, %d, %d)}", pQ->nGuildDBID, pQ->biCharacterDBID, GuildRecruitSystem::Max::MaxRequestCount, GuildRecruitSystem::Max::GuildRecruitCharacterRowCount );

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryGuildRecruitRequestOff( TQGuildRecruitRequestOff* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildJoinCharacters" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelGuildJoinCharacters(%d, %d)}", pQ->nGuildDBID, pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryGuildRecruitAcceptOn( TQGuildRecruitAcceptOn* pQ, TAGuildRecruitAcceptOn* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AcceptGuildJoinCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGuildRecruitAcceptOn] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_AcceptGuildJoinCharacter(%d,%lld,?,?,?,?,?)}", pQ->nGuildDBID, pQ->biAcceptCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) 
	{
		SQL_TIMESTAMP_STRUCT JoinDate; 
		tm tmJoinDate;		

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);				
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nAddAccountDBID, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->cJob, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, 4, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->cLevel, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, 5, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(SQL_TIMESTAMP_STRUCT), 0, &JoinDate, sizeof(SQL_TIMESTAMP_STRUCT), &cblen);
		SQLBindParameter(m_hstmt, 6, SQL_PARAM_OUTPUT, SQL_BIT,	SQL_BIT, sizeof(bool), 0, &pA->bDelGuildRecruit, sizeof(bool), &cblen );				

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE) {
			::memset(&tmJoinDate, 0, sizeof(tmJoinDate));
			tmJoinDate.tm_year = JoinDate.year - DF_TM_YEAR_EPOCH;
			tmJoinDate.tm_mon = JoinDate.month - 1;
			tmJoinDate.tm_mday = JoinDate.day;
			tmJoinDate.tm_hour = JoinDate.hour;
			tmJoinDate.tm_min = JoinDate.minute;	
			tmJoinDate.tm_sec = JoinDate.second;
			pA->JoinDate = CTimeSet::ConvertTmToTimeT64_LC(&tmJoinDate);
		}
	}	

	SQLCloseCursor(m_hstmt);
	return nResult;	
}
int CDNSQLWorld::QueryGuildRecruitAcceptOff( TQGuildRecruitAcceptOff* pQ, TAGuildRecruitAcceptOff* pA )
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildJoinCharacters" );	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelGuildJoinCharacters(%d, %d)}", pQ->nGuildDBID, pQ->biDenyCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}
int CDNSQLWorld::QueryDelGuildRecruit( TQDelGuildRecruit* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelGuildJoinCharactersForCheat" );	
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelGuildJoinCharactersForCheat(%d, %d, %d)}", pQ->nDelType, pQ->nGuildDBID, pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
}

#if defined (PRE_ADD_DONATION)
int CDNSQLWorld::QueryDonate(TQDonate* pQ)
{
	CQueryTimeLog QueryTimeLog("P_Donate");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryDonate] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	TIMESTAMP_STRUCT DonationTime;
	CTimeSet TimeSet(pQ->tDonationTime, true);
	memcpy(&DonationTime, &TimeSet.GetDbTimeStamp(), sizeof(DonationTime));

	ZeroMemory(m_wszQuery, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_Donate(?, %I64d, %I64d)}", pQ->biCharacterDBID, pQ->nCoin);

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_INPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, SQL_TIMESTAMP_LEN, 0, &DonationTime, sizeof(DonationTime), SqlLen.Get(SQL_TIMESTAMP_LEN));

		RetCode = SQLExecute (m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryDonationRanking(TQDonationRanking* pQ, OUT TADonationRaking& Ack)
{
	CQueryTimeLog QueryTimeLog("P_GetListDonationRanking");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryDonationRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	ZeroMemory(m_wszQuery, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDonationRanking(%I64d, ?, ?)}", pQ->biCharacterDBID);

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &Ack.nMyCoin, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &Ack.nMyRanking, sizeof(int), &cblen);

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			Donation::Ranking Ranking;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Ranking.biCharacterID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Ranking.wszCharacterName, sizeof(Ranking.wszCharacterName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Ranking.btJobCode, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Ranking.wszGuildName, sizeof(Ranking.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Ranking.nCoin, sizeof(INT64), &cblen);
			CheckColumnCount(nColNo, "P_GetListDOnationRanking");

			for (BYTE i = 0; i < Donation::MAX_RANKING; ++i)
			{
				ZeroMemory(&Ranking, sizeof(Ranking));
				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) && (RetCode != SQL_SUCCESS_WITH_INFO))
					break;

				memcpy(&(Ack.Ranking[i]), &Ranking, sizeof(Ranking));
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryDonationTopRanker(TQDonationTopRanker* pQ, OUT TADonationTopRanker& Ack)
{
	CQueryTimeLog QueryTimeLog("P_GetListDonationRanker");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryDonationTopRanker] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	ZeroMemory(m_wszQuery, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDonationRanker}");

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Ack.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Ack.wszCharacterName, sizeof(Ack.wszCharacterName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Ack.nCoin, sizeof(INT64), &cblen);
			CheckColumnCount(nColNo, "P_GetListDonationRanker");
			RetCode = SQLFetch(m_hstmt);
			if (RetCode == SQL_NO_DATA)
			{
				SQLCloseCursor(m_hstmt);
				return ERROR_NONE;
			}

			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}
#endif // #if defined (PRE_ADD_DONATION)

#if defined( PRE_PARTY_DB )

int CDNSQLWorld::QueryAddParty( TQAddParty* pQ, TAAddParty* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddParty" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddParty] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	WCHAR wszPartyName[PARTYNAMELENMAX * 2 + 1] ={0,};
	ConvertQuery(pQ->Data.PartyData.wszPartyName, PARTYNAMELENMAX, wszPartyName, _countof(wszPartyName));

#if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?,%d)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount, pQ->Data.PartyData.bCheckComeBackParty );
#else	// #if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount );
#endif	// #if defined( PRE_ADD_NEWCOMEBACK )

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->Data.PartyData.PartyID, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}

int CDNSQLWorld::QueryAddPartyAndMemberGame( TQAddPartyAndMemberGame* pQ, TAAddPartyAndMemberGame* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddParty" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddParty] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	WCHAR wszPartyName[PARTYNAMELENMAX * 2 + 1] ={0,};
	ConvertQuery(pQ->Data.PartyData.wszPartyName, PARTYNAMELENMAX, wszPartyName, _countof(wszPartyName));

#if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?,%d)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount, pQ->Data.PartyData.bCheckComeBackParty );
#else	// #if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount );
#endif	// #if defined( PRE_ADD_NEWCOMEBACK )
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->Data.PartyData.PartyID, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}

int CDNSQLWorld::QueryAddPartyAndMemberVillage( TQAddPartyAndMemberVillage* pQ, TAAddPartyAndMemberVillage* pA )
{
	CQueryTimeLog QueryTimeLog( "P_AddParty" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddParty] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	WCHAR wszPartyName[PARTYNAMELENMAX * 2 + 1] ={0,};
	ConvertQuery(pQ->Data.PartyData.wszPartyName, PARTYNAMELENMAX, wszPartyName, _countof(wszPartyName));

#if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?,%d)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount, pQ->Data.PartyData.bCheckComeBackParty );
#else	// #if defined( PRE_ADD_NEWCOMEBACK )
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddParty(%d,%d,%d,%I64d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,?)}", pQ->Data.PartyData.iServerID, pQ->cWorldSetID, pQ->Data.PartyData.iRoomID, pQ->Data.PartyData.biLeaderCharacterDBID, pQ->Data.PartyData.LocationType, pQ->Data.PartyData.iLocationID, pQ->Data.PartyData.Type, 
		wszPartyName, pQ->Data.PartyData.nPartyMaxCount, pQ->Data.PartyData.iTargetMapIndex, pQ->Data.PartyData.TargetMapDifficulty, pQ->Data.PartyData.iBitFlag, pQ->Data.PartyData.iPassword, pQ->Data.PartyData.cMinLevel, pQ->Data.PartyData.LootRule, pQ->Data.PartyData.LootItemRank, pQ->Data.PartyData.iUpkeepCount );
#endif	// #if defined( PRE_ADD_NEWCOMEBACK )

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->Data.PartyData.PartyID, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}


int CDNSQLWorld::QueryModParty( TQModParty* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModParty" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	WCHAR wszPartyName[PARTYNAMELENMAX * 2 + 1] ={0,};
	ConvertQuery(pQ->PartyData.wszPartyName, PARTYNAMELENMAX, wszPartyName, _countof(wszPartyName));
	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModParty(%I64d,%d,%d,%d,%d,N'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d)}", pQ->PartyData.PartyID, pQ->PartyData.iServerID, pQ->PartyData.LocationType, pQ->PartyData.iLocationID, pQ->PartyData.Type,
		wszPartyName, pQ->PartyData.nPartyMaxCount, pQ->PartyData.iTargetMapIndex, pQ->PartyData.TargetMapDifficulty, pQ->PartyData.iBitFlag, pQ->PartyData.iPassword, pQ->PartyData.cMinLevel, pQ->PartyData.LootRule, pQ->PartyData.LootItemRank, pQ->PartyData.iUpkeepCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelParty( TQDelParty* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelParty" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelParty(%I64d)}", pQ->PartyID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryDelPartyForServer( TQDelPartyForServer* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_DelPartyForServer" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelPartyForServer(%d)}", pQ->iServerID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryJoinParty( TQJoinParty* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddParyJoin" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
#if defined( PRE_ADD_NEWCOMEBACK )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddParyJoin(%I64d,%I64d,%d,%d)}", pQ->PartyID, pQ->biCharacterDBID, pQ->iMaxUserCount, pQ->bCheckComeBackParty );
#else
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddParyJoin(%I64d,%I64d,%d)}", pQ->PartyID, pQ->biCharacterDBID, pQ->iMaxUserCount );
#endif

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryOutParty( TQOutParty* pQ, TAOutParty* pA )
{
	CQueryTimeLog QueryTimeLog( "P_DelPartyJoin" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryOutParty] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelPartyJoin(%I64d,%I64d,?)}", pQ->PartyID, pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->bIsDelParty, sizeof(BYTE), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}

#if defined( PRE_PARTY_DB )

int CDNSQLWorld::_QueryGetListParty( std::vector<Party::Data>& vData )
{
	SQLLEN cblen;
	int nResult = ERROR_DB;

	SQLRETURN RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			Party::Data Data;

			int nColNo = 1;
			bool bValue = 0;

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Data.PartyID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iServerID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iRoomID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.LocationType, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iLocationID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.Type, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Data.wszPartyName, sizeof(WCHAR)*_countof(Data.wszPartyName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &Data.nPartyMaxCount, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iTargetMapIndex, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.TargetMapDifficulty, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iBitFlag, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iPassword, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cMinLevel, sizeof(byte), &cblen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.LootRule, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.LootItemRank, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iUpkeepCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Data.biLeaderCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.iCurMemberCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &bValue, sizeof(byte), &cblen);
			CheckColumnCount(nColNo, "P_GetParties");
			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}
#if defined( PRE_ADD_NEWCOMEBACK )
				Data.bCheckComeBackParty = bValue;
#endif
				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				vData.push_back( Data );
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

#endif // #if defined( PRE_PARTY_DB )


#if defined( PRE_WORLDCOMBINE_PARTY )

int CDNSQLWorld::QueryGetListParty( int iWorldSetID, ePartyType PartyType, std::vector<Party::Data>& vData )
{
	CQueryTimeLog QueryTimeLog( "P_GetListPartyCodeParties" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetListParty] P_GetListPartyCodeParties Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 2;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPartyCodeParties(%d,%d,%d)}", PartyType, iWorldSetID, nVersion );

	return _QueryGetListParty( vData );
}

#endif // #if defined( PRE_WORLDCOMBINE_PARTY )

int CDNSQLWorld::QueryGetListParty( TQGetListParty* pQ, std::vector<Party::Data>& vData )
{
	CQueryTimeLog QueryTimeLog( "P_GetListParties" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListParty] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	int nVersion = 2;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListParties(%d,%d,%d)}", pQ->iExecptServerID, pQ->cWorldSetID, nVersion );

	return _QueryGetListParty( vData );
}

int CDNSQLWorld::QueryGetListPartyMember( TQGetListPartyMember* pQ, TAGetListPartyMember* pA, std::vector<Party::MemberData>& vData )
{
	CQueryTimeLog QueryTimeLog( "P_GetPartyJoinMembers" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListPartyMember] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	BYTE cPartyType = 0;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	int nVersion = 2;

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPartyJoinMembers(%I64d,?,%d)}", pQ->PartyID, nVersion );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE){
		int nPrmNo = 1;		

		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &cPartyType, sizeof(BYTE), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			Party::MemberData Data;

			int nColNo = 1;

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Data.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Data.wszCharName, sizeof(WCHAR)*_countof(Data.wszCharName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cLevel, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cJob, sizeof(byte), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bLeader, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &Data.bCheckComeback, sizeof(bool), &cblen);
			CheckColumnCount(nColNo, "P_GetPartyJoinMembers");
			while(1)
			{
				memset( &Data, 0, sizeof(Data) );

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
				{
					nResult = ERROR_NONE;
					break;
				}

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				vData.push_back( Data );
			}
		}
	}

	if( nResult == ERROR_NONE )
		pA->PartyType = static_cast<ePartyType>(cPartyType);

	SQLCloseCursor(m_hstmt);
	return nResult;	
}

int CDNSQLWorld::QueryModPartyLeader( TQModPartyLeader* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModPartyOwnerFlag" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModPartyOwnerFlag(%I64d,%I64d)}", pQ->PartyID, pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

#if defined( PRE_ADD_NEWCOMEBACK )
int CDNSQLWorld::QueryModPartyMemberComeback( TQModPartyMemberComeback* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ModReturnCharacter" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModReturnCharacter(%I64d,%I64d,%d)}", pQ->PartyID, pQ->biCharacterDBID, pQ->bComebackAppellation );

	return CommonReturnValueQuery(m_wszQuery);
}
#endif

#endif // #if defined( PRE_PARTY_DB )

#if defined( PRE_ADD_DOORS_PROJECT )
int CDNSQLWorld::QuerySaveCharacterAbility(TQSaveCharacterAbility* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_SaveCharacterAbility" );
	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

	string AbilityCodesStr, AbilityPointsStr;
	
	for (int i = 0; i < Doors::CharacterAbility::MaxType; i++)
	{
		if(!AbilityCodesStr.empty())
			AbilityCodesStr.append(",");
		if(!AbilityPointsStr.empty())
			AbilityPointsStr.append(",");

		AbilityCodesStr.append(boost::lexical_cast<std::string>(i));
		AbilityPointsStr.append(boost::lexical_cast<std::string>(pQ->nCharacterAblity[i]));
	}

	swprintf( m_wszQuery, L"{?=CALL dbo.P_SaveCharacterAbility(%I64d,N'%S',N'%S')}", pQ->biCharacterDBID, AbilityCodesStr.c_str(), AbilityPointsStr.c_str() );

	return CommonReturnValueQuery(m_wszQuery);
}
#endif // #if defined( PRE_ADD_DOORS_PROJECT )

#if defined (PRE_ADD_BESTFRIEND)
int CDNSQLWorld::QueryGetBestFriend(TQGetBestFriend* pQ, TAGetBestFriend* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetBestFriend" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetBestFriend] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetBestFriend(%I64d,?,?,?)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(WCHAR) * NAMELENMAX, 0, pA->Info.wszName, sizeof(WCHAR) * NAMELENMAX, &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->Info.cLevel, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->Info.cJob, sizeof(BYTE), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nColNo = 1;

			TIMESTAMP_STRUCT StartDate={0,}, EndDate={0,};

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->Info.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &StartDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->Info.wszMyMemo, sizeof(WCHAR) * BESTFRIENDMEMOMAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &pA->Info.bStatus, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_BIT, &pA->Info.bCancel, sizeof(bool), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &EndDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, pA->Info.wszFriendMemo, sizeof(WCHAR) * BESTFRIENDMEMOMAX, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &pA->Info.biItemSerial, sizeof(INT64), &cblen);
			CheckColumnCount(nColNo, "P_GetBestFriend");

			while(1)
			{
				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				CTimeParamSet StartTime(&QueryTimeLog, pA->Info.tStartDate, StartDate, 0, 0, pQ->biCharacterDBID);
				CTimeParamSet EndTime(&QueryTimeLog, pA->Info.tEndDate, EndDate, 0, 0, pQ->biCharacterDBID);

				if (!StartTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}
				if (!EndTime.IsValid())
				{
					nResult = ERROR_DB;
					break;									
				}

				nResult = ERROR_NONE;
				break;
			}
		}
	}	
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryRegistBestFriend(TQRegistBestFriend* pQ, TARegistBestFriend* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddBestFriend" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryRegistBestFriend] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddBestFriend(%I64d,%I64d)}", pQ->biFromCharacterDBID, pQ->biToCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryCancelBestFriend(TQCancelBestFriend* pQ, TACancelBestFriend* pA)
{
	CQueryTimeLog QueryTimeLog( "P_EndBestFriend" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryCancelBestFriend] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_EndBestFriend(%I64d,%d)}", pQ->biCharacterDBID, pQ->bCancel );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryCloseBestFriend(TQCloseBestFriend* pQ, TACloseBestFriend* pA)
{
	CQueryTimeLog QueryTimeLog( "P_DelBestFriend" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryCloseBestFriend] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DelBestFriend(%I64d)}", pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryEditBestFriendMemo(TQEditBestFriendMemo* pQ, TAEditBestFriendMemo* pA)
{
	CQueryTimeLog QueryTimeLog( "P_ModMyMemo" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryEditBestFriendMemo] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModMyMemo(%I64d,N'%s')}", pQ->biCharacterDBID, pQ->wszMemo );

	return CommonReturnValueQuery(m_wszQuery);
}
#endif // #if defined (PRE_ADD_BESTFRIEND)

#if defined( PRE_PRIVATECHAT_CHANNEL )
int CDNSQLWorld::QueyrGetPrivateChatChannelInfo(TQPrivateChatChannelInfo* pQ, std::vector<TPrivateChatChannelInfo>& vData)
{
	CQueryTimeLog QueryTimeLog( "P_GetListPrivateChannels" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetListPrivateChannels] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPrivateChannels}");
	
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{		
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TPrivateChatChannelInfo PrivateChatChannelInfo;			
			int nColNo = 1;		

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &PrivateChatChannelInfo.nPrivateChatChannelID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, PrivateChatChannelInfo.wszName, sizeof(WCHAR) * PrivateChatChannel::Common::MaxNameLen, &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &PrivateChatChannelInfo.biMasterCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &PrivateChatChannelInfo.nMemberCount, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &PrivateChatChannelInfo.nPassWord, sizeof(int), &cblen);			

			CheckColumnCount(nColNo, "P_GetListPrivateChannels");

			while(1)
			{
				memset(&PrivateChatChannelInfo, 0, sizeof(PrivateChatChannelInfo));
				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				vData.push_back(PrivateChatChannelInfo);				
			}
		}
	}	
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueyrGetPrivateChatChannelMember(TQPrivateChatChannelMember* pQ, TAPrivateChatChannelMember* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetListPrivateChannelMembers" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetListPrivateChannelMembers] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListPrivateChannelMembers(%I64d)}", pQ->nPrivateChatChannelID);

	TPrivateChatChannelMember Member;
	INT64 nPrivateChatChannelID;

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{		
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{			
			int nColNo = 1;		

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Member.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &nPrivateChatChannelID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Member.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);			

			CheckColumnCount(nColNo, "P_GetListPrivateChannelMembers");

			while(1)
			{				
				memset(&Member, 0, sizeof(Member));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}
				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;	
				pA->Member[pA->nCount] = Member;
				pA->nCount++;
				if(pA->nCount >= PrivateChatChannel::Common::MaxMemberCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}	
	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueyrAddPrivateChatChannel(TQAddPrivateChatChannel* pQ, TAAddPrivateChatChannel* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_AddPrivateChannel] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	WCHAR wszChannelName[PrivateChatChannel::Common::MaxNameLen * 2 + 1] ={0,};
	ConvertQuery(pQ->tPrivateChatChannel.wszName, PrivateChatChannel::Common::MaxNameLen, wszChannelName, _countof(wszChannelName));

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPrivateChannel(N'%s',%I64d,%d,?)}", wszChannelName, pQ->tPrivateChatChannel.biMasterCharacterDBID, pQ->tPrivateChatChannel.nPassWord);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->tPrivateChatChannel.nPrivateChatChannelID, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}	
	return nResult;
}

int CDNSQLWorld::QueyrAddPrivateChatChannelMember(TQAddPrivateChatMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_JoinPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrAddPrivateChatChannelMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_JoinPrivateChannel(%I64d,N'%s',%I64d,%d,%d)}", pQ->Member.biCharacterDBID, pQ->Member.wszCharacterName, pQ->nPrivateChatChannelID, pQ->nServerID, PrivateChatChannel::Common::MaxMemberCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrInvitePrivateChatChannelMember(TQInvitePrivateChatMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_JoinPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrAddPrivateChatChannelMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_JoinPrivateChannel(%I64d,N'%s',%I64d,%d,%d)}", pQ->Member.biCharacterDBID, pQ->Member.wszCharacterName, pQ->nPrivateChatChannelID, pQ->nServerID, PrivateChatChannel::Common::MaxMemberCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrEnterPrivateChatChannelMember(TQAddPrivateChatMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_EnterPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrAddPrivateChatChannelMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_EnterPrivateChannel(%I64d,%I64d,N'%s',%d,%d)}", pQ->Member.biCharacterDBID, pQ->nPrivateChatChannelID, pQ->Member.wszCharacterName, pQ->nServerID, PrivateChatChannel::Common::MaxMemberCount );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrDelPrivateChatChannelMember(TQDelPrivateChatMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_QuitPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrDelPrivateChatChannelMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_QuitPrivateChannel(%I64d)}", pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrExitPrivateChatChannelMember(TQDelPrivateChatMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ExitPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrExitPrivateChatChannelMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ExitPrivateChannel(%I64d,%I64d)}", pQ->biCharacterDBID, pQ->nPrivateChatChannelID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrModPrivateChatChannelInfo(TQModPrivateChatChannelInfo* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModPrivateChannel" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrModPrivateChatChannelInfo] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModPrivateChannel(%I64d,N'%s',%I64d,%d)}", pQ->nPrivateChatChannelID, pQ->wszChannelName, pQ->biCharacterDBID, pQ->nPassWord );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrModPrivateMemberServerID(TQModPrivateChatMemberServerID* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_ModPrivateChannelMemberServerID" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueyrModPrivateMemberServerID] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ModPrivateChannelMemberServerID(%I64d,%d)}", pQ->biCharacterDBID, pQ->nServerID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueyrDelPrivateMemberServerID(TQDelPrivateChatMemberServerID* pQ, std::list<TPrivateMemberDelServer> &MemberList)
{	
	CQueryTimeLog QueryTimeLog( "P_ExitPrivateChannelMemberForServer" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_ExitPrivateChannelMemberForServer] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_ExitPrivateChannelMemberForServer(%d)}", pQ->nServerID );

	INT64 biCharacterDBID = 0;	
	TPrivateMemberDelServer Member;

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{		
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );
		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{			
			int nColNo = 1;		

			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Member.biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Member.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);			
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &Member.nPrivateChatChannelID, sizeof(INT64), &cblen);	

			CheckColumnCount(nColNo, "P_ExitPrivateChannelMemberForServer");

			while(1)
			{
				memset(&Member, 0, sizeof(Member));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{					
					nResult = ERROR_NONE;
					break;
				}
				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				MemberList.push_back(Member);

			}
		}
	}	
	SQLCloseCursor(m_hstmt);
	return nResult;
}
#endif

#ifdef PRE_ADD_DOORS
int CDNSQLWorld::QueryCancelDoorsAuthentication(UINT nAccountDBID)
{
	CQueryTimeLog QueryTimeLog( "P_DRS_DelMobileAuthentication" );

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_DRS_DelMobileAuthentication(%d)}", nAccountDBID);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif		//#ifdef PRE_ADD_DOORS

#if defined( PRE_ADD_NEWCOMEBACK )
int CDNSQLWorld::QueryCheckCombackEffectItem( INT64 biCharacterDBID, int nItemID )
{
	CQueryTimeLog QueryTimeLog( "P_CheckEffectItemExpireDate" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_CheckEffectItemExpireDate(%I64d,%d)}", biCharacterDBID, nItemID);

	return CommonReturnValueQuery(m_wszQuery);
}
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )

int CDNSQLWorld::QueryResetAlteiaWorldEvent( TQResetAlteiaWorldEvent* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ResetAlteiaEvent" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ResetAlteiaEvent}");	

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddAlteiaWorldPlayResult( TQADDAlteiaWorldPlayResult* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddPlayAlteia" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddPlayAlteia(%I64d,%d,%d,%d)}", pQ->biCharacterDBID, pQ->nGoldKeyCount, pQ->dwPlayTime, pQ->nGuildID);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetAlteiaWorldInfo( TQGetAlteiaWorldInfo* pQ, TAGetAlteiaWorldInfo* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetPlayAlteia");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPlayAlteia] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPlayAlteia(%I64d)}", pQ->biCharacterDBID );

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;
		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TIMESTAMP_STRUCT GoldKeyDate, PlayTimeDate, LastPlayDate;
			INT64	biCharacterDBID;
			int nGuildID = 0;
			int nGuildGoldKeyCount = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nBestGoldKeyCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &GoldKeyDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->uiBestPlayTime, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &PlayTimeDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nDailyPlayCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &pA->nWeeklyPlayCount, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &LastPlayDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nGuildID, sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &nGuildGoldKeyCount, sizeof(int), &cblen );
			
			CheckColumnCount(nColNo, "P_GetPlayAlteia");
			RetCode = SQLFetch(m_hstmt);
			if (RetCode == SQL_NO_DATA)
			{
				SQLCloseCursor(m_hstmt);
				return ERROR_NONE;
			}

			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetAlteiaWorldPrivateGoldKeyRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldPrivateGoldKeyRank* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetPageAlteiaRanking");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPageAlteiaRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageAlteiaRanking(%d,%d)}", pQ->nMaxRank, pQ->eType );

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;		

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			AlteiaWorld::GoldKeyRankMemberInfo Member;
			INT64	biCharacterDBID;			

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nRankIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Member.cJob, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Member.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);	
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Member.wszGuildName, sizeof(Member.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nGoldKeyCount, sizeof(int), &cblen);
			
			CheckColumnCount(nColNo, "P_GetPageAlteiaRanking");


			while(1)
			{
				memset(&Member, 0, sizeof(Member));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					SQLCloseCursor(m_hstmt);
					return ERROR_NONE;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RankMemberInfo[pA->nCount] = Member;
				pA->nCount++;

				if(pA->nCount >= AlteiaWorld::Common::MaxRankCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetAlteiaWorldPrivatePlayTimeRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldPrivatePlayTimeRank* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetPageAlteiaRanking");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPageAlteiaRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageAlteiaRanking(%d,%d)}", pQ->nMaxRank, pQ->eType );

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;		

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			AlteiaWorld::PlayTimeRankMemberInfo Member;
			INT64	biCharacterDBID = 0;			

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nRankIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Member.cJob, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &biCharacterDBID, sizeof(INT64), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, Member.wszCharacterName, sizeof(WCHAR) * NAMELENMAX, &cblen);	
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Member.wszGuildName, sizeof(Member.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nPlaySec, sizeof(int), &cblen);

			CheckColumnCount(nColNo, "P_GetPageAlteiaRanking");


			while(1)
			{
				memset(&Member, 0, sizeof(Member));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					SQLCloseCursor(m_hstmt);
					return ERROR_NONE;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RankMemberInfo[pA->nCount] = Member;
				pA->nCount++;

				if(pA->nCount >= AlteiaWorld::Common::MaxRankCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetAlteiaWorldGuildGoldKeyRank( TQGetAlteiaWorldRankInfo* pQ, TAGetAlteiaWorldGuildGoldKeyRank* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetPageAlteiaRanking");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetPageAlteiaRanking] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetPageAlteiaRanking(%d,%d)}", pQ->nMaxRank, pQ->eType );

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;		

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			AlteiaWorld::GuildGoldKeyRankInfo Member;						

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nRankIndex, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nGuildID, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, &Member.wszGuildName, sizeof(Member.wszGuildName), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &Member.wGuildMark, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &Member.wGuildMarkBG, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_SMALLINT, &Member.wGuildMarkBorder, sizeof(short), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Member.nGoldKeyCount, sizeof(int), &cblen);
			
			CheckColumnCount(nColNo, "P_GetPageAlteiaRanking");


			while(1)
			{
				memset(&Member, 0, sizeof(Member));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					SQLCloseCursor(m_hstmt);
					return ERROR_NONE;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->RankMemberInfo[pA->nCount] = Member;
				pA->nCount++;

				if(pA->nCount >= AlteiaWorld::Common::MaxRankCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetAlteiaWorldSendTicketList( TQGetAlteiaWorldSendTicketList* pQ, TAGetAlteiaWorldSendTicketList* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetListAlteiaSendTickets");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[P_GetListAlteiaSendTickets] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListAlteiaSendTickets(%I64d,?,?)}", pQ->biCharacterDBID);

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;
		
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nSendTicketCount, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRecvTicketCount, sizeof(int), &cblen);

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{			
			TIMESTAMP_STRUCT SendDate;
			INT64	biCharacterDBID = 0;
			int nCount = 0;

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &biCharacterDBID, sizeof(int), &cblen);		
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &SendDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			CheckColumnCount(nColNo, "P_GetListAlteiaSendTickets");

			while(1)
			{
				biCharacterDBID = 0;

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					SQLCloseCursor(m_hstmt);
					return ERROR_NONE;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->biCharacterDBID[nCount] = biCharacterDBID;
				nCount++;

				if(nCount >= AlteiaWorld::Common::MaxSendCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddAlteiaWorldSendTicketList( TQAddAlteiaWorldSendTicketList* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddAlteiaSendTickets" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddAlteiaSendTickets(%I64d,%I64d,%d)}", pQ->biSendCharacterDBID, pQ->biCharacterDBID, pQ->nSendMaxCount);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryResetAlteiaWorldPlayAlteia( TQResetAlteiaWorldPlayAlteia* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_ResetDailyWeeklyPlayAlteia" );

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ResetDailyWeeklyPlayAlteia(%I64d,%d)}", pQ->biCharacterDBID, pQ->eType);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddAlteiaWorldEvent( TQAddAlteiaWorldEvent* pQ )
{	
	CQueryTimeLog QueryTimeLog( "P_AddAlteiaSchedule" );

	if(CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, 0, 0, 0, L"[QueryAddAlteiaWorldEvent] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen[2];

	int nResult = ERROR_DB, sqlparam = 0, nRestraintID = 0;
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddAlteiaSchedule(?,?)}");
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		TIMESTAMP_STRUCT tStartTime, tEndTime;
		CTimeSet StartTime(pQ->tAlteiaEventStartTime, true);
		memcpy(&tStartTime, &StartTime.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT));
		CTimeSet EndTime(pQ->tAlteiaEventEndTime, true);
		memcpy(&tEndTime, &EndTime.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT));

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int),	&cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT,  SQL_C_TIMESTAMP, SQL_TIMESTAMP,	SQL_TIMESTAMP_LEN, 0, &tStartTime, sizeof(TIMESTAMP_STRUCT), SqlLen[0].Get(SQL_TIMESTAMP_LEN) );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_INPUT,  SQL_C_TIMESTAMP, SQL_TIMESTAMP,	SQL_TIMESTAMP_LEN, 0, &tEndTime, sizeof(TIMESTAMP_STRUCT), SqlLen[1].Get(SQL_TIMESTAMP_LEN) );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
			return ERROR_DB;
	}

	return nResult;
}

#endif // #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined( PRE_ADD_STAMPSYSTEM )
int CDNSQLWorld::QueryGetListCompleteChallenges( TQGetListCompleteChallenges* pQ, TAGetListCompleteChallenges* pA )
{
	CQueryTimeLog QueryTimeLog("P_GetListCompleteChallenges");

	if (CheckConnect() < ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetListCompleteChallenges] Check Connect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));	
	swprintf( m_wszQuery, L"{?=CALL dbo.P_GetListCompleteChallenges(%I64d)}", pQ->biCharacterDBID);

	SQLRETURN RetCode;
	int nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLLEN cblen;
		CDNSqlLen SqlLen;		

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);			

		RetCode = SQLExecute (m_hstmt);
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			StampSystem::StampCompleteInfo Data;

			TIMESTAMP_STRUCT CompleteDate = { 0, };

			int nColNo = 1;
			SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &Data.cChallengeIndex, sizeof(BYTE), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &Data.nWeekDay, sizeof(int), &cblen);
			SQLBindCol(m_hstmt, nColNo++, SQL_TIMESTAMP, &CompleteDate, sizeof(TIMESTAMP_STRUCT), &cblen);

			CheckColumnCount(nColNo, "P_GetListCompleteChallenges");

			int nCount = 0;

			while(1)
			{
				memset(&Data, 0, sizeof(Data));

				RetCode = SQLFetch(m_hstmt);
				if (RetCode == SQL_NO_DATA)
				{
					SQLCloseCursor(m_hstmt);
					return ERROR_NONE;
				}

				if ((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) 
					break;

				pA->CompleteInfo[nCount] = Data;
				nCount++;

				if(nCount >= StampSystem::Common::MaxStampSlotCount)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
			CheckRetCode(RetCode, L"SQLFetch");
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryInitCompleteChallenge( TQInitCompleteChallenge* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_InitCompleteChallenge" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryInitCompleteChallenge] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_InitCompleteChallenge(%I64d)}", pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryAddCompleteChallenge( TQAddCompleteChallenge* pQ )
{
	CQueryTimeLog QueryTimeLog( "P_AddCompleteChallenge" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryAddCompleteChallenge] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddCompleteChallenge(%I64d,%d,%d)}", pQ->biCharacterDBID, pQ->cChallengeIndex, pQ->nWeekDay );

	return CommonReturnValueQuery(m_wszQuery);
}
#endif // #if defined( PRE_ADD_STAMPSYSTEM )

#if defined(PRE_ADD_CHNC2C)
int CDNSQLWorld::QueryGetGameMoney(TQGetGameMoney* pQ, TAGetGameMoney* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetGameMoney" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryGetGameMoney] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGameMoney(%I64d,?)}", pQ->biCharacterDBID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->biCoinBalance, sizeof(INT64), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}
	return nResult;
}

int CDNSQLWorld::QueryKeepGameMoney(TQKeepGameMoney* pQ)
{
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	UINT uiCheckSum = 0;
	int nResult = QueryMakeCharacterCheckSum( pQ, CheckSumReason::C2C_KeepGameMoney, pQ->biCharacterDBID, uiCheckSum, - pQ->biReduceCoin );
	if( ERROR_NONE != nResult )
	{
		return nResult;
	}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

	CQueryTimeLog QueryTimeLog( "P_KeepGameMoneyForTrade" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryKeepGameMoney] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );

#if defined( PRE_ADD_CHARACTERCHECKSUM )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_KeepGameMoneyForTrade('%S',%I64d,%I64d,?)}", pQ->szBookID, pQ->biCharacterDBID, pQ->biReduceCoin );

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;
	nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nCheckSumSize = sizeof(uiCheckSum);
		int nNo = 1;
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, nCheckSumSize, 0, &uiCheckSum, nCheckSumSize, SqlLen.Get(nCheckSumSize));

		RetCode = SQLExecute (m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
		{
			g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, "[QueryKeepGameMoney] Fail(result:%d)\r\n", nResult);
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
#else // #if defined( PRE_ADD_CHARACTERCHECKSUM )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_KeepGameMoneyForTrade('%S',%I64d,%I64d)}", pQ->szBookID, pQ->biCharacterDBID, pQ->biReduceCoin );

	return CommonReturnValueQuery(m_wszQuery);
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
}

int CDNSQLWorld::QueryTransferGameMoney(TQTransferGameMoney* pQ)
{
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	UINT uiCheckSum = 0;
	int nResult = QueryMakeCharacterCheckSum( pQ, CheckSumReason::C2C_TransferGameMoney, pQ->biCharacterDBID, uiCheckSum, pQ->biAddCoin );
	if( ERROR_NONE != nResult )
	{
		return nResult;
	}
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )

	CQueryTimeLog QueryTimeLog( "P_TransferGameMoneyForTrade" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryTransferGameMoney] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
#if defined( PRE_ADD_CHARACTERCHECKSUM )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_TransferGameMoneyForTrade('%S',%I64d,%I64d,?)}", pQ->szBookID, pQ->biAddCoin, pQ->biCharacterDBID);

	SQLRETURN RetCode;
	SQLLEN cblen;
	CDNSqlLen SqlLen;
	nResult = ERROR_DB;

	RetCode = SQLPrepare (m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nCheckSumSize = sizeof(uiCheckSum);
		int nNo = 1;
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter (m_hstmt, nNo++, SQL_PARAM_INPUT, SQL_BINARY, SQL_BINARY, nCheckSumSize, 0, &uiCheckSum, nCheckSumSize, SqlLen.Get(nCheckSumSize));

		RetCode = SQLExecute (m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") != ERROR_NONE)
		{
			g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, "[QueryTransferGameMoney] Fail(result:%d)\r\n", nResult);
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
#else // #if defined( PRE_ADD_CHARACTERCHECKSUM )
	swprintf( m_wszQuery, L"{?=CALL dbo.P_TransferGameMoneyForTrade('%S',%I64d,%I64d)}", pQ->szBookID, pQ->biAddCoin, pQ->biCharacterDBID);

	return CommonReturnValueQuery(m_wszQuery);
#endif // #if defined( PRE_ADD_CHARACTERCHECKSUM )
}
#endif //#if defined(PRE_ADD_CHNC2C)
#if defined(PRE_ADD_CP_RANK)
int CDNSQLWorld::QueryAddStageClearBest(TQAddStageClearBest* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddStageClearBest" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, pQ->biCharacterDBID, 0, L"[QueryAddStageClearBest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddStageClearBest(%d,%I64d,%d,%d,%d)}", pQ->nMapID, pQ->biCharacterDBID, pQ->Code, pQ->nClearPoint, pQ->nLimitLevel);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryInitStageClearBest(char cInitType)
{
	CQueryTimeLog QueryTimeLog( "P_InitStageCP" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, 0, 0, 0, 0, L"[QueryInitStageClearBest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_InitStageCP(%d)}", cInitType);

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryGetStageClearBest(TQGetStageClearBest* pQ, TAGetStageClearBest* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetStageClearBest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log(LogType::_ERROR, pQ->cWorldSetID, pQ->nAccountDBID, 0, 0, L"[QueryGetStageClearBest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;
	memset(&m_wszQuery, 0, sizeof(m_wszQuery));

	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetStageClearBest(%d,?,?,?,?,?,?,?,?)}", pQ->nMapID);

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->sLegendClearBest.biCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pA->sLegendClearBest.wszCharName), 0, pA->sLegendClearBest.wszCharName, sizeof(pA->sLegendClearBest.wszCharName), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->sLegendClearBest.cRank, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->sLegendClearBest.nClearPoint, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->sMonthlyClearBest.biCharacterDBID, sizeof(INT64), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_WCHAR, SQL_WVARCHAR, sizeof(pA->sMonthlyClearBest.wszCharName), 0, pA->sMonthlyClearBest.wszCharName, sizeof(pA->sMonthlyClearBest.wszCharName), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->sMonthlyClearBest.cRank, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->sMonthlyClearBest.nClearPoint, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}
	return nResult;
}

int CDNSQLWorld::QueryGetStageClearPersonalBest(TQGetStageClearPersonalBest* pQ, TAGetStageClearPersonalBest* pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetStageClearPersonalBest" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[QueryGetStageClearPersonalBest] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	int nResult = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetStageClearPersonalBest(%I64d,%d)}", pQ->biCharacterDBID, pQ->nMapID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter( m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			int nNo = 1;			
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pA->Code,	sizeof(char), &cblen );
			SQLBindCol( m_hstmt, nNo++, SQL_INTEGER, &pA->nClearPoint, sizeof(int), &cblen );
			CheckColumnCount(nNo, "P_GetStageClearPersonalBest");
			RetCode = SQLFetch(m_hstmt);
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;

}
#endif //#if defined(PRE_ADD_CP_RANK)

#if defined(PRE_ADD_DWC)
int CDNSQLWorld::QueryAddDWCTeam(TQCreateDWCTeam* pQ, TACreateDWCTeam* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddDWCTeam" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_AddDWCTeam] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDWCTeam(N'%s',%I64d,?)}", pQ->wszTeamName, pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nTeamID, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryAddDWCTeamMember(TQAddDWCTeamMember* pQ, TAAddDWCTeamMember* pA)
{
	CQueryTimeLog QueryTimeLog( "P_AddDWCTeamMember" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_AddDWCTeamMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddDWCTeamMember(%d,%I64d)}", pQ->nTeamID, pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}
	return pA->nRetCode;
}

int CDNSQLWorld::QueryDelDWCTeamMember(TQDelDWCTeamMember* pQ, TADelDWCTeamMember* pA)
{
	CQueryTimeLog QueryTimeLog( "P_DelDWCTeamMember" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_DelDWCTeamMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_DelDWCTeamMember(%d,%I64d)}", pQ->nTeamID, pQ->biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");
	}
	return pA->nRetCode;
}

int CDNSQLWorld::QueryGetDWCTeamInfo(INT64 biCharacterDBID, UINT* pTeamID, TDWCTeam* Info)
{
	CQueryTimeLog QueryTimeLog( "P_GetDWCTeamJoinCharacter" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_GetDWCTeamJoinCharacter] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nRet = ERROR_DB;
	(*pTeamID) = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDWCTeamJoinCharacter(%I64d)}", biCharacterDBID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRet, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;
			TIMESTAMP_STRUCT LastPlayDate = { 0, };

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, pTeamID, sizeof(UINT), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &Info->wszTeamName, sizeof(Info->wszTeamName), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalWin, sizeof(Info->nTotalWin), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalLose, sizeof(Info->nTotalLose), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalDraw, sizeof(Info->nTotalDraw), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nDWCPoint, sizeof(Info->nDWCPoint), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayWin, sizeof(Info->wTodayWin), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayLose, sizeof(Info->wTodayLose), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayDraw, sizeof(Info->wTodayDraw), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wConsecutiveWin, sizeof(Info->wConsecutiveWin), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wConsecutiveLose, sizeof(Info->wConsecutiveLose), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nWeeklyPlayCount, sizeof(Info->nWeeklyPlayCount), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastPlayDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nHiddenDWCPoint, sizeof(Info->nHiddenDWCPoint), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nDWCRank, sizeof(Info->nDWCRank), &cblen );

			CheckColumnCount(nNo, "P_GetDWCTeamJoinCharacter");

			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				return ERROR_NONE;
			}		

			CheckRetCode(RetCode, L"SQLFetch");

			CTimeParamSet LastPlayDateTime(&QueryTimeLog, Info->tLastPlayDate, LastPlayDate);
			if (!LastPlayDateTime.IsValid())
			{
				SQLCloseCursor(m_hstmt);
				return ERROR_DB;
			}
		}
	}
	SQLCloseCursor(m_hstmt);
	return nRet;
}

int CDNSQLWorld::QueryGetDWCTeamInfoByTeamID(UINT nTeamID, UINT* pTeamID, TDWCTeam* Info)
{
	CQueryTimeLog QueryTimeLog( "P_GetDWCTeam" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_GetDWCTeam] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nRet = ERROR_DB;
	(*pTeamID) = 0;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDWCTeam(%d)}", nTeamID );

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRet, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;
			TIMESTAMP_STRUCT LastPlayDate = { 0, };
			
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &Info->wszTeamName, sizeof(Info->wszTeamName), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalWin, sizeof(Info->nTotalWin), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalLose, sizeof(Info->nTotalLose), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nTotalDraw, sizeof(Info->nTotalDraw), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nDWCPoint, sizeof(Info->nDWCPoint), &cblen );			
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayWin, sizeof(Info->wTodayWin), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayLose, sizeof(Info->wTodayLose), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wTodayDraw, sizeof(Info->wTodayDraw), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wConsecutiveWin, sizeof(Info->wConsecutiveWin), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_SMALLINT, &Info->wConsecutiveLose, sizeof(Info->wConsecutiveLose), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nWeeklyPlayCount, sizeof(Info->nWeeklyPlayCount), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &LastPlayDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nHiddenDWCPoint, sizeof(Info->nHiddenDWCPoint), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &Info->nDWCRank, sizeof(Info->nDWCRank), &cblen );

			CheckColumnCount(nNo, "P_GetDWCTeam");

			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				return ERROR_NONE;
			}		

			CheckRetCode(RetCode, L"SQLFetch");

			CTimeParamSet LastPlayDateTime(&QueryTimeLog, Info->tLastPlayDate, LastPlayDate);
			if (!LastPlayDateTime.IsValid())
			{
				SQLCloseCursor(m_hstmt);
				return ERROR_DB;
			}
			(*pTeamID) = nTeamID;
		}
	}
	SQLCloseCursor(m_hstmt);
	return nRet;
}

int CDNSQLWorld::QueryGetDWCTeamMemberList(UINT nDWCTeamID, std::vector<TDWCTeamMember>& vTeamMember)
{
	CQueryTimeLog QueryTimeLog( "P_GetListDWCTeamMember" );

	if(CheckConnect() < 0) {
		g_Log.Log( LogType::_ERROR, L"[P_GetListDWCTeamMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListDWCTeamMember(%d)}", nDWCTeamID);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TDWCTeamMember DWCTeamMember;
			WCHAR wszTempCharacterName[ NAMELENMAX + DWCPREFIXLEN ] = L"";

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &DWCTeamMember.nAccountDBID, sizeof(DWCTeamMember.nAccountDBID), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_C_SBIGINT, &DWCTeamMember.biCharacterDBID, sizeof(DWCTeamMember.biCharacterDBID), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszTempCharacterName, sizeof(WCHAR) * (NAMELENMAX + DWCPREFIXLEN), &cbLen);			
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &DWCTeamMember.cJobCode, sizeof(DWCTeamMember.cJobCode), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_BIT, &DWCTeamMember.bTeamLeader, sizeof(DWCTeamMember.bTeamLeader), &cbLen);

			CheckColumnCount(nNo, "P_GetListDWCTeamMember");
			while(1)
			{
				memset(&DWCTeamMember, 0, sizeof(DWCTeamMember));
				memset(&wszTempCharacterName, 0, sizeof(wszTempCharacterName));
				
				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				_wcscpy(DWCTeamMember.wszCharacterName, NAMELENMAX, wszTempCharacterName + DWCPREFIXLEN, NAMELENMAX);
				
				vTeamMember.push_back(DWCTeamMember);
				if( vTeamMember.size() > DWC::DWC_MAX_MEMBERISZE )
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryAddDWCResult( TQAddPvPDWCResult* pQ, TAAddPvPDWCResult* pA )
{
	CQueryTimeLog QueryTimeLog( "QueryAddDWCResult" );

	if(CheckConnect() < 0) {
		g_Log.Log( LogType::_ERROR, L"[QueryAddDWCResult] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_ModDWCPVPResult(%d,%d,%d,%d,%d,?,?,?,?,?,?,?,?,?)}", pQ->nTeamID, pQ->cResult, pQ->nDWCGradePoint, pQ->nHiddenDWCGradePoint, pQ->nOppositeTeamID);

	int nResult = ERROR_DB;	

	SQLRETURN RetCode = SQLPrepare( m_hstmt, m_wszQuery, SQL_NTSL );
	if( CheckRetCode( RetCode, L"SQLPrepare" ) == ERROR_NONE )
	{
		SQLLEN cblen;

		int nNo = 1;
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cblen );

		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &pA->DWCScore.nTotalWin, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &pA->DWCScore.nTotalLose, sizeof(int), &cblen );
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_INTEGER,	SQL_INTEGER, sizeof(int), 0, &pA->DWCScore.nTotalDraw, sizeof(int), &cblen );		
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.wTodayWin, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.wTodayLose, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.wTodayDraw, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.wConsecutiveWin, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.wConsecutiveLose, sizeof(BYTE), &cblen);
		SQLBindParameter(m_hstmt, nNo++, SQL_PARAM_OUTPUT, SQL_TINYINT, SQL_TINYINT, sizeof(BYTE), 0, &pA->DWCScore.nWeeklyPlayCount, sizeof(BYTE), &cblen);

		RetCode = SQLExecute( m_hstmt );
		CheckRetCode( RetCode, L"SQLExecute" );
	}

	return nResult;
}

int CDNSQLWorld::QueryGetDWCRankList(TQGetDWCRankPage* pQ, TAGetDWCRankPage * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetPageDWCRanking" );

	if(CheckConnect() < 0) {
		g_Log.Log( LogType::_ERROR, L"[P_GetPageDWCRanking] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetPageDWCRanking(%d,%d,?)}", pQ->nPageNum, pQ->nPageSize);

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nTotalRankSize, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TDWCRankData RankData;
			memset(&RankData, 0, sizeof(TDWCRankData));

			WCHAR wszLeaderData[80];
			WCHAR wszMemberData[160];
			memset(wszLeaderData, 0, sizeof(wszLeaderData));
			memset(wszMemberData, 0, sizeof(wszMemberData));

			std::vector<std::wstring> tokens;
			std::vector<std::wstring> tokens1;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nRank, sizeof(RankData.nRank), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nTeamID, sizeof(RankData.nTeamID), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &RankData.wszTeamName, sizeof(WCHAR) * _countof(RankData.wszTeamName), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nWinCount, sizeof(RankData.nWinCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nLoseCount, sizeof(RankData.nLoseCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nDrawCount, sizeof(RankData.nDrawCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nDWCPoint, sizeof(RankData.nDWCPoint), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nChangedRanking, sizeof(RankData.nChangedRanking), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszLeaderData, sizeof(WCHAR) * _countof(wszLeaderData), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszMemberData, sizeof(WCHAR) * _countof(wszMemberData), &cbLen);

			CheckColumnCount(nNo, "P_GetListDWCTeamMember");
			while(1)
			{
				memset(&RankData, 0, sizeof(TDWCRankData));
				memset(wszLeaderData, 0, sizeof(wszLeaderData));
				memset(wszMemberData, 0, sizeof(wszMemberData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}
				
				tokens.clear();
				TokenizeW(wszLeaderData, tokens, L",");
				if (tokens.size() >= 2)
				{
					_wcscpy(RankData.LeaderData.wszMemberName, _countof(RankData.LeaderData.wszMemberName), tokens[0].c_str() + DWCPREFIXLEN, NAMELENMAX);
					RankData.LeaderData.cJobCode = static_cast<BYTE>(_wtoi(tokens[1].c_str()));
				}
				
				tokens1.clear();
				TokenizeW(wszMemberData, tokens1, L"|");
				for (int nCount = 0; nCount < static_cast<int>(tokens1.size()); nCount++)
				{
					tokens.clear();
					TokenizeW(tokens1[nCount], tokens, L",");

					_wcscpy(RankData.MemberData[nCount].wszMemberName, _countof(RankData.MemberData[nCount].wszMemberName), tokens[0].c_str() + DWCPREFIXLEN, NAMELENMAX);
					RankData.MemberData[nCount].cJobCode = static_cast<BYTE>(_wtoi(tokens[1].c_str()));
				}

				pA->RankList[pA->nPageSize] = RankData;
				pA->nPageSize++;
				if(pA->nPageSize >= DWC::Common::RankPageMaxSize)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetDWCFindRank(TQGetDWCFindRank* pQ, TAGetDWCFindRank * pA)
{
	CQueryTimeLog QueryTimeLog( "P_GetDWCRanking" );

	if(CheckConnect() < 0) {
		g_Log.Log( LogType::_ERROR, L"[P_GetDWCRanking] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;	// ODBC API 호출 반환값
	SQLLEN cbLen;

	std::wstring wstrFindTeamName;
	std::wstring wstrFindCharacterName;
	if (pQ->cFindType == DWC::FindRankType::TeamName)
		wstrFindTeamName = pQ->wszFindKey;
	if (pQ->cFindType == DWC::FindRankType::CharacterName)
		wstrFindCharacterName = pQ->wszFindKey;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDWCRanking(N'%s', N'%s')}", wstrFindTeamName.c_str(), wstrFindCharacterName.c_str());

	int nResult = ERROR_DB;
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			TDWCRankData RankData;
			memset(&RankData, 0, sizeof(TDWCRankData));

			WCHAR wszLeaderData[80];
			WCHAR wszMemberData[160];
			memset(wszLeaderData, 0, sizeof(wszLeaderData));
			memset(wszMemberData, 0, sizeof(wszMemberData));

			std::vector<std::wstring> tokens;
			std::vector<std::wstring> tokens1;

			int nNo = 1;
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nRank, sizeof(RankData.nRank), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nTeamID, sizeof(RankData.nTeamID), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &RankData.wszTeamName, sizeof(WCHAR) * _countof(RankData.wszTeamName), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nWinCount, sizeof(RankData.nWinCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nLoseCount, sizeof(RankData.nLoseCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nDrawCount, sizeof(RankData.nDrawCount), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nDWCPoint, sizeof(RankData.nDWCPoint), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &RankData.nChangedRanking, sizeof(RankData.nChangedRanking), &cbLen );
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszLeaderData, sizeof(WCHAR) * _countof(wszLeaderData), &cbLen);
			SQLBindCol(m_hstmt, nNo++, SQL_WCHAR, &wszMemberData, sizeof(WCHAR) * _countof(wszMemberData), &cbLen);

			CheckColumnCount(nNo, "P_GetListDWCTeamMember");
			while(1)
			{
				memset(&RankData, 0, sizeof(TDWCRankData));
				memset(wszLeaderData, 0, sizeof(wszLeaderData));
				memset(wszMemberData, 0, sizeof(wszMemberData));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				tokens.clear();
				TokenizeW(wszLeaderData, tokens, L",");
				if (tokens.size() >= 2)
				{
					_wcscpy(RankData.LeaderData.wszMemberName, _countof(RankData.LeaderData.wszMemberName), tokens[0].c_str() + DWCPREFIXLEN, NAMELENMAX);
					RankData.LeaderData.cJobCode = static_cast<BYTE>(_wtoi(tokens[1].c_str()));
				}

				tokens1.clear();
				TokenizeW(wszMemberData, tokens1, L"|");
				for (int nCount = 0; nCount < static_cast<int>(tokens1.size()); nCount++)
				{
					tokens.clear();
					TokenizeW(tokens1[nCount], tokens, L",");

					_wcscpy(RankData.MemberData[nCount].wszMemberName, _countof(RankData.MemberData[nCount].wszMemberName), tokens[0].c_str() + DWCPREFIXLEN, NAMELENMAX);
					RankData.MemberData[nCount].cJobCode = static_cast<BYTE>(_wtoi(tokens[1].c_str()));
				}

				pA->Rank = RankData;
				break;
			}
		}
	}

	SQLCloseCursor(m_hstmt);
	return nResult;
}

int CDNSQLWorld::QueryGetDWCChannelInfo(TDWCChannelInfo* pChannelInfo)
{
	CQueryTimeLog QueryTimeLog( "P_GetDWCChannel" );

	if(CheckConnect() < ERROR_NONE){
		g_Log.Log( LogType::_ERROR, L"[P_GetDWCChannel] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	int nRet = ERROR_DB;

	memset(&m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetDWCChannel}");

	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		int nPrmNo = 1;
		SQLBindParameter(m_hstmt, nPrmNo++, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nRet, sizeof(int), &cblen);

		RetCode = SQLExecute(m_hstmt);

		if( CheckRetCode( RetCode, L"SQLExecute" ) == ERROR_NONE )
		{
			int nNo = 1;
			TIMESTAMP_STRUCT StartDate={0,}, EndDate={0,};

			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pChannelInfo->nChannelID, sizeof(pChannelInfo->nChannelID), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &StartDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol(m_hstmt, nNo++, SQL_TIMESTAMP, &EndDate, sizeof(TIMESTAMP_STRUCT), &cblen);
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &pChannelInfo->cStatus, sizeof(pChannelInfo->cStatus), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pChannelInfo->nLadderTime[0], sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_INTEGER, &pChannelInfo->nLadderTime[1], sizeof(int), &cblen );
			SQLBindCol(m_hstmt, nNo++, SQL_TINYINT, &pChannelInfo->cLimitCharacterLevel, sizeof(BYTE), &cblen );

			CheckColumnCount(nNo, "P_GetDWCChannel");

			RetCode = SQLFetch(m_hstmt);
			if(RetCode == SQL_NO_DATA){
				SQLCloseCursor(m_hstmt);
				return ERROR_NONE;
			}		

			CheckRetCode(RetCode, L"SQLFetch");

			CTimeParamSet StartDateTime(&QueryTimeLog, pChannelInfo->tStartDate, StartDate);
			CTimeParamSet EndDateTime(&QueryTimeLog, pChannelInfo->tEndDate, EndDate);

			if (!StartDateTime.IsValid() || !EndDateTime.IsValid())
			{
				SQLCloseCursor(m_hstmt);
				return ERROR_DB;
			}
		}
	}
	SQLCloseCursor(m_hstmt);
	return nRet;
}

#endif	//#if defined(PRE_ADD_DWC)

#if defined(PRE_ADD_GUILD_CONTRIBUTION)
int CDNSQLWorld::QueryGetGuildContributionPoint(TQGetGuildContributionPoint* pQ, TAGetGuildContributionPoint& pA)
{
	CQueryTimeLog QueryTimeLog("P_GetGuildContributionPoint");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetGuildContributionPoint] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;

	int result = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetGuildContributionPoint(%I64d,?,?)}",  pQ->biCharacterDBID);
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];

		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &result, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA.nTotalGuildContributionPoint, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA.nWeeklyGuildContributionPoint, sizeof(int), &len);

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");
	}

	return result;
}

int CDNSQLWorld::QueryGetGuildContributionWeeklyRanking(TQGetGuildContributionRanking* pQ, TAGetGuildContributionRanking* pA)
{
	CQueryTimeLog QueryTimeLog("P_GetListWeeklyGuildContributionPointRanking");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[P_GetListWeeklyGuildContributionPointRanking] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cbLen;

	int nResult = ERROR_DB;
	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListWeeklyGuildContributionPointRanking(%d)}",  pQ->nGuildDBID);
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);

	if(CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE) {
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &cbLen);

		RetCode = SQLExecute(m_hstmt);
		if(CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE)
		{
			GuildContribution::TGuildContributionRankingData GuildMember;
			int nColNo = 1;
			// 리턴값 : 랭킹, 캐릭터 DBID, 캐릭터 이름, 직업코드, 공헌도 수치
			// 랭킹(int)
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.nRank, sizeof(int), &cbLen);
			// 캐릭터DBID(bigint)
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_C_SBIGINT, &GuildMember.biCharacterDBID, sizeof(INT64), &cbLen);
			// 캐릭터명(nvarchar)
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_WCHAR, GuildMember.wszCharacterName, sizeof(GuildMember.wszCharacterName), &cbLen);
			// 직업코드
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_TINYINT, &GuildMember.cJobCode, sizeof(BYTE), &cbLen);
			// 공헌도 수치(int)
			RetCode = SQLBindCol(m_hstmt, nColNo++, SQL_INTEGER, &GuildMember.nWeeklyGuildContributionPoint, sizeof(int), &cbLen);

			CheckColumnCount(nColNo, "P_GetListWeeklyGuildContributionPointRanking");
			while(1)
			{
				memset(&GuildMember, 0, sizeof(GuildMember));

				RetCode = SQLFetch(m_hstmt);
				if(RetCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(RetCode, L"SQLFetch") < ERROR_NONE) &&(RetCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				pA->ContributionRankingList[pA->nCount] = GuildMember;
				++pA->nCount;
				if(pA->nCount >= GUILDSIZE_MAX)
				{
					nResult = ERROR_NONE;
					break;
				}
			}
		}
	}
	SQLCloseCursor(m_hstmt);
	return nResult;
}
#endif	// #if defined(PRE_ADD_GUILD_CONTRIBUTION)

#if defined(PRE_ADD_EQUIPLOCK)
int CDNSQLWorld::QueryAddLockItem(TQLockItemInfo* pQ, TALockItemInfo *pA)
{
	CQueryTimeLog QueryTimeLog("P_AddLockItem");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryAddLockItem] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;
	pA->nRetCode = ERROR_DB;

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddLockItem(%I64d,%d,%d,%d,%I64d,?)}", pQ->biCharacterDBID, pQ->Code, pQ->cItemSlotIndex, pQ->nItemID, pQ->biItemSerial);
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];
		TIMESTAMP_STRUCT LockDate = {0, };
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &LockDate, sizeof(TIMESTAMP_STRUCT), &len);

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");

		CTimeParamSet LockTime(&QueryTimeLog, pA->tLockDate, LockDate, 0, 0, pQ->biCharacterDBID);
		if (!LockTime.IsValid())
		{
			return ERROR_DB;
		}
	}
	
	return pA->nRetCode;
}

int CDNSQLWorld::QueryRequestItemUnlock(TQUnLockRequsetItemInfo* pQ, TAUnLockRequestItemInfo *pA)
{
	CQueryTimeLog QueryTimeLog("P_RequestItemUnlock");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryRequestItemUnlock] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;
	pA->nRetCode = ERROR_DB;

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_RequestItemUnlock(%I64d,%d,%d,%d,%I64d,?, %d, ?)}", pQ->biCharacterDBID, pQ->Code, pQ->cItemSlotIndex, pQ->nItemID, pQ->biItemSerial, pQ->nUnLockWaitTime);
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];
		TIMESTAMP_STRUCT UnRequestLockDate = {0, };
		TIMESTAMP_STRUCT UnLockDate = {0, };
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &len);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &UnRequestLockDate, sizeof(TIMESTAMP_STRUCT), &len);
		SQLBindParameter(m_hstmt, 3, SQL_PARAM_OUTPUT, SQL_C_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), 0, &UnLockDate, sizeof(TIMESTAMP_STRUCT), &len);

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");

		CTimeParamSet LockTime(&QueryTimeLog, pA->tUnLockDate, UnLockDate, 0, 0, pQ->biCharacterDBID);
		if (!LockTime.IsValid())
		{
			return ERROR_DB;
		}
		CTimeParamSet UnLockRequestTime(&QueryTimeLog, pA->tUnLockRequestDate, UnRequestLockDate, 0, 0, pQ->biCharacterDBID);
		if (!UnLockRequestTime.IsValid())
		{
			return ERROR_DB;
		}
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryUnlockItem(TQLockItemInfo* pQ)
{
	CQueryTimeLog QueryTimeLog("P_UnlockItem");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryRequestItemUnlock] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN len;
	int nResult = ERROR_DB;

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_UnlockItem(%I64d,%d,%d,%d,%I64d)}", pQ->biCharacterDBID, pQ->Code, pQ->cItemSlotIndex, pQ->nItemID, pQ->biItemSerial);
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		CDNSqlLen sqlLen[3];
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &nResult, sizeof(int), &len);

		retCode = SQLExecute(m_hstmt);
		CheckRetCode(retCode, L"SQLExecute");
	}

	return nResult;
}

int CDNSQLWorld::QueryGetListLockedItems(INT64 biCharacterDBID, TAGetListLockedItems *pA)
{
	CQueryTimeLog QueryTimeLog("P_GetListLockedItems");
	
	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[QueryGetListLockedItems] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN retCode;
	SQLLEN cbLen;
	pA->nRetCode = ERROR_DB;

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_GetListLockedItems(%I64d)}", biCharacterDBID);
	retCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(retCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cbLen);

		retCode = SQLExecute(m_hstmt);
		if(CheckRetCode(retCode, L"SQLExecute") == ERROR_NONE)
		{
			TDBLockItemInfo LockItem;
			
			TIMESTAMP_STRUCT LockDate = {0,};
			TIMESTAMP_STRUCT UnLockRequestDate = {0,};
			TIMESTAMP_STRUCT UnLockDate = {0,};
			// 리턴값 : 아이템 종류(1 일반장비, 7 캐쉬장비), 장착 부위(슬롯인덱스), 해제일자, 잠금일자, 해제요청일
			
			int nNo = 1;
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &LockItem.cItemCode, sizeof(char), &cbLen );
			SQLBindCol( m_hstmt, nNo++, SQL_TINYINT, &LockItem.cItemSlotIndex, sizeof(char), &cbLen );
			SQLBindCol( m_hstmt, nNo++, SQL_TIMESTAMP, &UnLockRequestDate, sizeof(TIMESTAMP_STRUCT), &cbLen );
			SQLBindCol( m_hstmt, nNo++, SQL_TIMESTAMP, &LockDate, sizeof(TIMESTAMP_STRUCT), &cbLen );
			SQLBindCol( m_hstmt, nNo++, SQL_TIMESTAMP, &UnLockDate, sizeof(TIMESTAMP_STRUCT), &cbLen );
			SQLBindCol( m_hstmt, nNo++, SQL_C_SBIGINT, &LockItem.biItemSerial, sizeof(INT64), &cbLen );

			CheckColumnCount(nNo, "P_GetListLockedItems");
			while(1)
			{
				memset(&LockItem, 0, sizeof(LockItem));
				memset(&LockDate, 0, sizeof(LockDate));
				memset(&UnLockDate, 0, sizeof(UnLockDate));

				retCode = SQLFetch(m_hstmt);
				if(retCode == SQL_NO_DATA)
					break;

				if((CheckRetCode(retCode, L"SQLFetch") < ERROR_NONE) &&(retCode != SQL_SUCCESS_WITH_INFO)) {
					break;
				}

				CTimeParamSet UnLockTime(&QueryTimeLog, LockItem.ItemData.tUnLockDate, UnLockDate, 0, pA->nAccountDBID);
				if (!UnLockTime.IsValid())
				{
					pA->nRetCode = ERROR_DB;
					break;									
				}
				//UnLockDate가 설정되어있으면 잠금 해제 상태인것..
				if(LockItem.ItemData.tUnLockDate > 0)
				{
					LockItem.ItemData.eItemLockStatus = EquipItemLock::RequestUnLock;
					CTimeParamSet LockTime(&QueryTimeLog, LockItem.ItemData.tUnLockRequestDate, UnLockRequestDate, 0, pA->nAccountDBID);
					if (!LockTime.IsValid())
					{
						pA->nRetCode = ERROR_DB;
						break;									
					}
				}
				else
				{
					CTimeParamSet LockTime(&QueryTimeLog, LockItem.ItemData.tUnLockDate, LockDate, 0, pA->nAccountDBID);
					if (!LockTime.IsValid())
					{
						pA->nRetCode = ERROR_DB;
						break;									
					}
					LockItem.ItemData.eItemLockStatus = EquipItemLock::Lock;
				}

				pA->LockItem[pA->nCount] = LockItem;
				++pA->nCount;
				if(pA->nCount >= (EQUIPMAX+CASHEQUIPMAX))
				{
					pA->nRetCode = ERROR_NONE;
					break;
				}
			}
		}

	}
	SQLCloseCursor(m_hstmt);
	return pA->nRetCode;
}
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
#if defined( PRE_PVP_GAMBLEROOM )
int CDNSQLWorld::QueryAddGambleRoom(TQAddGambleRoom* pQ, TAAddGambleRoom *pA)
{
	CQueryTimeLog QueryTimeLog("P_AddGamble");

	if (CheckConnect() > ERROR_NONE)
	{
		g_Log.Log(LogType::_ERROR, 0, 0, 0, 0, L"[P_AddGamble] CheckConnect Failed.\r\n");
		return ERROR_DB;
	}

	SQLRETURN RetCode;
	SQLLEN cblen;
	pA->nRetCode = ERROR_DB;

	memset(m_wszQuery, 0, sizeof(m_wszQuery));
	swprintf(m_wszQuery, L"{?=CALL dbo.P_AddGamble(%d,%d,?)}", pQ->cGambleType, pQ->nGamblePrice );
	RetCode = SQLPrepare(m_hstmt, m_wszQuery, SQL_NTSL);
	if (CheckRetCode(RetCode, L"SQLPrepare") == ERROR_NONE)
	{
		SQLBindParameter(m_hstmt, 1, SQL_PARAM_OUTPUT, SQL_INTEGER, SQL_INTEGER, sizeof(int), 0, &pA->nRetCode, sizeof(int), &cblen);
		SQLBindParameter(m_hstmt, 2, SQL_PARAM_OUTPUT, SQL_C_SBIGINT, SQL_BIGINT, sizeof(INT64), 0, &pA->nGambleDBID, sizeof(INT64), &cblen);
		

		RetCode = SQLExecute(m_hstmt);
		CheckRetCode(RetCode, L"SQLExecute");		
		if (CheckRetCode(RetCode, L"SQLExecute") == ERROR_NONE){			
			return pA->nRetCode;
		}
	}

	return pA->nRetCode;
}

int CDNSQLWorld::QueryAddGambleRoomMember(TQAddGambleRoomMember* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGambleParticipantes" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, 0, 0, 0, 0, L"[QueryAddGambleRoomMember] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGambleParticipantes(%I64d, %I64d)}", pQ->nGambleDBID, pQ->biCharacterDBID );

	return CommonReturnValueQuery(m_wszQuery);
}

int CDNSQLWorld::QueryEndGambleRoom(TQEndGambleRoom* pQ)
{
	CQueryTimeLog QueryTimeLog( "P_AddGambleEnd" );

	if( CheckConnect() < ERROR_NONE )
	{
		g_Log.Log( LogType::_ERROR, 0, 0, 0, 0, L"[P_AddGambleEnd] CheckConnect Fail\r\n");
		return ERROR_DB;
	}

	memset( m_wszQuery, 0, sizeof(m_wszQuery) );
	swprintf( m_wszQuery, L"{?=CALL dbo.P_AddGambleEnd(%I64d,%I64d,%I64d,%d,%d)}", pQ->nGambleDBID, 
		pQ->Member[PvPGambleRoom::firstWinner].biCharacterDBID, pQ->Member[PvPGambleRoom::SecondWinner].biCharacterDBID, 
		pQ->Member[PvPGambleRoom::firstWinner].nPrice, pQ->Member[PvPGambleRoom::SecondWinner].nPrice );

	return CommonReturnValueQuery(m_wszQuery);
}

#endif
