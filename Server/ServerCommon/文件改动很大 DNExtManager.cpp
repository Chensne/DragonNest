#include "StdAfx.h"
#include "DNExtManager.h"
#include "DNTableFile.h"
#include "Util.h"
#include "Log.h"
#include "EtResourceMng.h"
#include "./boost/lexical_cast.hpp"
#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
#include "XMLParser.h"
#endif	// #if defined(_LOGINSERVER) || defined(_MASTERSERVER)

#if defined(_DBSERVER)
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
extern TDBConfig g_Config;

#elif defined(_CASHSERVER)
#include "./EtStringManager/EtUIXML.h"
#include "DNSQLManager.h"
#include "DNSQLMembership.h"
#if defined(PRE_ADD_LIMITED_CASHITEM)
#include "DNLimitedCashItemRepository.h"
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)

extern TCashConfig g_Config;

#elif defined(_MASTERSERVER)
extern TMasterConfig g_Config;

#elif defined(_LOGINSERVER)
#if defined(PRE_ADD_MULTILANGUAGE)
#include "MemPool.h"
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
extern TLoginConfig g_Config;

#endif

CDNExtManager *g_pExtManager = NULL;

CDNExtManager::CDNExtManager(void)
{
	m_pMapInfo.clear();
	m_pPCBangData.clear();
	m_pMapDefaultCreate.clear();
#if defined( PRE_ADD_DWC )
	m_pMapDWCCreate.clear();
#endif // #if defined( PRE_ADD_DWC )
	m_pMapSkillData.clear();
	m_mapDefaultUnlockSkillsByClass.clear();
	m_pMapCoinCount.clear();
	m_pLevelData.clear();
	m_pWeaponData.clear();
	m_pPartData.clear();
	m_PvPGameStartConditionTable.clear();
	m_ScheduleTable.clear();
	m_PvPGameModeTable.clear();
	m_MatchTypePvPGameModeID.clear();
	m_PvPMapTable.clear();
	m_GuildWarMapInfo.clear();
	m_MailTableData.clear();
	m_pGuildWarRewardData.clear();
	m_JobTableData.clear();
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		m_pMapCashCommodity.clear();
		m_pMapCashPackage.clear();
	}

	m_pMapCashLimit.clear();
	m_pItemData.clear();
	m_pMapGlobalWeight.clear();
	m_pMapGlobalWeightInt.clear();
	m_VecProhibitSaleList.clear();
	m_MapVehicle.clear();
	m_MapVehicleParts.clear();
}

bool CDNExtManager::AllLoad()
{
	int iLoadFailCount = 0;

	if (LoadGlobalWeight() == false)
		++iLoadFailCount;

	if (LoadGlobalWeightInt() == false)
		++iLoadFailCount;

#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
	if (LoadMapInfo() == false)
		++iLoadFailCount;

	if (LoadChannelInfo() == false)
		++iLoadFailCount;

	if (LoadPCBangData() == false)
		++iLoadFailCount;

#endif	// _MASTERSERVER

#if defined(_LOGINSERVER)
	if (LoadDefaultCreateData() == false)
		++iLoadFailCount;

#if defined( PRE_ADD_DWC )
	if(LoadDWCCreateData() == false)
		++iLoadFailCount;
#endif // #if defined( PRE_ADD_DWC )

	if (LoadSkillData() == false)
		++iLoadFailCount;

	if (LoadCoinCount() == false)
		++iLoadFailCount;

#if defined(PRE_ADD_MULTILANGUAGE)
	CLfhHeap::GetInstance()->InitPool();

	std::string strFilePath;
	for (int i = 0; i < MultiLanguage::SupportLanguage::NationMax; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			strFilePath.clear();
			strFilePath = g_Config.szResourcePath;
			strFilePath.append("\\Resource");
			if (g_Config.szResourceNation.size() > 0 && j == 0)
				strFilePath.append(g_Config.szResourceNation);
			strFilePath.append("\\UIString\\ProhibitWord");
			if (i != 0)		//일단 0번은 디폴트
				strFilePath.append(MultiLanguage::NationString[i]);
			strFilePath.append(".xml");
			if (LoadProhibitWord(i, strFilePath.c_str()))
			{
				g_Log.Log(LogType::_FILELOG, L"%S Loaded\r\n", strFilePath.c_str());
				break;
			}
		}
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	CLfhHeap::GetInstance()->InitPool();
	if (LoadProhibitWord() == false)
		++iLoadFailCount;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
	if (LoadLevelData() == false)
		++iLoadFailCount;

	if (LoadWeaponData() == false)
		++iLoadFailCount;

	if (LoadPartData() == false)
		++iLoadFailCount;

#elif defined(_MASTERSERVER)
	if (LoadPvPGameStartConditionTable() == false)
		++iLoadFailCount;

	if (LoadScheduleTable() == false)
		++iLoadFailCount;

	if (LoadPvPGameModeTable() == false)
		++iLoadFailCount;
	if (LoadGuildWarMapInfoTable() == false)
		++iLoadFailCount;

	if (LoadPvPMapTable() == false)
		++iLoadFailCount;
#if defined(_WORK)
	// WorldID 세팅하기
#if defined( PRE_WORLDCOMBINE_PARTY )
	if( g_Config.nWorldSetID == 0 )
		g_Config.nWorldSetID = GetWorldID();
#else
	g_Config.nWorldSetID = GetWorldID();
#endif // #if defined( PRE_WORLDCOMBINE_PARTY )
#endif
#endif	// _MASTERSERVER

#if defined(_DBSERVER) || defined(_CASHSERVER)
	if (LoadCashCommodity() == false)
		++iLoadFailCount;

	if (LoadCashPackage() == false)
		++iLoadFailCount;

	if (LoadCashLimit() == false)
		++iLoadFailCount;

	if (LoadItemData() == false)
		++iLoadFailCount;

	if (LoadPlayerLevel() == false)
		++iLoadFailCount;

	if (LoadVehicleData() == false)
		++iLoadFailCount;

	if (LoadVehiclePartsData() == false)
		++iLoadFailCount;
	if (LoadMailTableData() == false)
		++iLoadFailCount;

	if (LoadGuildWarRewardData() == false)
		++iLoadFailCount;

	if (LoadJobTableData() == false)
		++iLoadFailCount;

#endif	// #if defined(_DBSERVER) || defined(_CASHSERVER)

	if( iLoadFailCount != 0 )
		return false;

	return true;
}

CDNExtManager::~CDNExtManager(void)
{
#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
	SAFE_DELETE_PVEC( m_pPCBangData );

	for (TMapInfoMap::iterator iterMap = m_pMapInfo.begin(); iterMap != m_pMapInfo.end(); ++iterMap){
		SAFE_DELETE(iterMap->second);
	}
	m_pMapInfo.clear();
#endif	// #if defined(_LOGINSERVER) || defined(_MASTERSERVER)

#if defined(_LOGINSERVER)
	for (TMapDefaultCreate::iterator iter = m_pMapDefaultCreate.begin(); iter != m_pMapDefaultCreate.end(); ++iter){
		SAFE_DELETE(iter->second);
	}
	m_pMapDefaultCreate.clear();

#if defined( PRE_ADD_DWC )
	for(TMapDWCCreate::iterator iter = m_pMapDWCCreate.begin(); iter != m_pMapDWCCreate.end(); ++ iter){
		SAFE_DELETE(iter->second);
	}
#endif // #if defined( PRE_ADD_DWC )

	for( TMapSkillData::iterator iter = m_pMapSkillData.begin(); iter != m_pMapSkillData.end(); ++iter ) {
		SAFE_DELETE( iter->second );
	}
	m_pMapSkillData.clear();

#if defined(PRE_ADD_MULTILANGUAGE)
	for (std::map <int, TProhibitWord>::iterator ii = m_ProhibitWordList.begin(); ii != m_ProhibitWordList.end(); ii++)
		(*ii).second.clear();
	m_ProhibitWordList.clear();
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	m_ProhibitWordList.clear();
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

#elif defined(_MASTERSERVER)

	for( TPvPGameStartConditionData::iterator iterGameStartConditionTable = m_PvPGameStartConditionTable.begin() ; iterGameStartConditionTable != m_PvPGameStartConditionTable.end() ; ++iterGameStartConditionTable )
	{
		SAFE_DELETE( iterGameStartConditionTable->second );
	}
	m_PvPGameStartConditionTable.clear();
	SAFE_DELETE_PMAP( TPvPMapTableData, m_PvPMapTable );
#endif

	SAFE_DELETE_PMAP(TMapGlobalWeight, m_pMapGlobalWeight);
	SAFE_DELETE_PMAP(TMapGlobalWeightInt, m_pMapGlobalWeightInt);

#if defined(_DBSERVER) || defined(_CASHSERVER)
	{
		ScopeLock <CSyncLock> Lock(m_Sync);

		SAFE_DELETE_PMAP(TMapCashCommodity, m_pMapCashCommodity);
		SAFE_DELETE_PMAP(TMapCashPackage, m_pMapCashPackage);
	}

	SAFE_DELETE_PMAP(TMapCashLimit, m_pMapCashLimit);
	SAFE_DELETE_PMAP( TMapItemData, m_pItemData );	

	typedef std::map<int, LevelValue *> TMapPlayerLevelTable;
	SAFE_DELETEA_PMAP( TMapPlayerLevelTable, m_nMapTable );
	SAFE_DELETE_PVEC(m_pGuildWarRewardData);
#endif	// #if defined(_DBSERVER) || defined(_CASHSERVER)

	for (map<int, vector<TSkillData*> >::iterator ii = m_mapDefaultUnlockSkillsByClass.begin(); ii != m_mapDefaultUnlockSkillsByClass.end(); ii++)
	{
		for (vector<TSkillData*>::iterator ih = (*ii).second.begin(); ih != (*ii).second.end(); ih++)
			SAFE_DELETE((*ih));
		(*ii).second.clear();
	}
	m_mapDefaultUnlockSkillsByClass.clear();

	SAFE_DELETE_PMAP(TMapCoinCount, m_pMapCoinCount);
	SAFE_DELETE_PMAP(TMapLevelData, m_pLevelData);
	SAFE_DELETE_PMAP(TMapWeaponData, m_pWeaponData);
	SAFE_DELETE_PMAP(TMapPartData, m_pPartData);
}

void CDNExtManager::FindExtFileList( const char *szFileName, std::vector<std::string> &szVecList )
{
	szVecList.clear();
	bool bExistFile = false;
	CFileNameString szFullName = szFileName;
	CFileStream Stream( szFileName );
	if( !Stream.IsValid() ) return;
	Stream.Close();

	char szPath[256] = { 0, };
	szVecList.push_back( szFullName.c_str() );
	_GetPath( szPath, _countof(szPath), szFullName.c_str() );
	std::vector<CFileNameString> szVecTemp;
	FindFileListInDirectory( szPath, "*.dnt", szVecTemp );

	char szTemp1[256];
	char szTemp2[256];
	_GetFileName( szTemp1, _countof(szTemp1), szFileName );
	_strlwr( szTemp1 );
	int nLength = (int)strlen(szTemp1);
	szTemp1[nLength] = '_';
	szTemp1[nLength+1] = 0;

	char szTemp3[256];
	for( DWORD i=0; i<szVecTemp.size(); i++ ) {
		_GetFileName( szTemp2, _countof(szTemp2), szVecTemp[i].c_str() );
		_strlwr( szTemp2 );

		if( strstr( szTemp2, szTemp1 ) && strcmp( szTemp2, szTemp1 ) ) {
			sprintf_s( szTemp3, "%s%s.dnt", szPath, szTemp2 );
			szVecList.push_back( szTemp3 );
		}
	}
}

DNTableFileFormat* CDNExtManager::LoadExtTable( const char *szFileName )
{
	char buf[_MAX_PATH] = { 0, };
	bool bValid = true;
	std::vector<CFileNameString> szVecList;

	sprintf_s( buf, "%s*.dnt", szFileName );

	DNTableFileFormat* pSox = new DNTableFileFormat();
	if (!pSox){
		g_Log.Log(LogType::_FILELOG, L"ext(%S) failed\r\n", buf );
		return NULL;
	}

	CEtResourceMng::GetInstance().FindFileListAll_IgnoreExistFile( "ext", buf, szVecList );
	for( DWORD i=0; i<szVecList.size(); i++ ) {
		if (pSox->Load(szVecList[i].c_str(), false) == false)
		{
			g_Log.Log(LogType::_FILELOG, L"%S failed\r\n", szVecList[i].c_str());
			bValid = false;
			break;
		}
	}
	if( !bValid )
	{
		if (szVecList.size() == 1){
			g_Log.Log(LogType::_FILELOG, L"%S failed\r\n", buf );
			SAFE_DELETE(pSox);
			return NULL;
		}
	}

	if (pSox->GetItemCount() <= 0){
		g_Log.Log(LogType::_FILELOG, L"%S Count(%d)\r\n", pSox->GetItemCount(), buf );
		SAFE_DELETE(pSox);
		return NULL;
	}
	return pSox;
}

bool CDNExtManager::LoadPlayerLevel()
{
	DNTableFileFormat *pSox = LoadExtTable( "PlayerLevelTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PlayerLevelTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	char *szLabelValue[PlayerLevelTableIndex::Amount] = {
		"_Strength",
		"_Agility",
		"_Intelligence",
		"_Stamina",
		"_Experience",
		"_SuperAmmor",
		"_SkillPoint",
		"_Fatigue",
		"_DeadDurabilityRatio",
		"_KillScore",
		"_Assistscore",
		"_Assistdecision",
		"_Aggroper",
		"_WeekFatigue",
		"_SKillUsageRatio",
	};

	for( int i=0; i<pSox->GetItemCount(); i++ ) {
		int nItemID = pSox->GetItemID(i);
		if( nItemID % PLAYER_MAX_LEVEL == 1 ) {
			LevelValue *pValue = new LevelValue[PLAYER_MAX_LEVEL];
			for( DWORD j=0; j<PLAYER_MAX_LEVEL; j++ ) {
				nItemID = pSox->GetItemID(i+j);
				for( int k=0; k<PlayerLevelTableIndex::Amount; k++ ) {
					switch( k ) {
						case PlayerLevelTableIndex::DeadDurabilityRatio:
						case PlayerLevelTableIndex::AggroPer:
							pValue[j].nValue[k] = (int)( ( pSox->GetFieldFromLablePtr( nItemID, szLabelValue[k] )->GetFloat() + 0.0001f ) * 100.f );
							break;
						case PlayerLevelTableIndex::SPDecreaseRatio:
							pValue[j].fValue[k] = pSox->GetFieldFromLablePtr( nItemID, szLabelValue[k] )->GetFloat();
							break;
						default:
							pValue[j].nValue[k] = pSox->GetFieldFromLablePtr( nItemID, szLabelValue[k] )->GetInteger();
							break;
					}
				}
			}
			int nJobID = ( pSox->GetItemID(i) / PLAYER_MAX_LEVEL ) + 1;
			m_nMapTable.insert( make_pair( nJobID, pValue ) );
			i += ( PLAYER_MAX_LEVEL - 1 ); // For 넘어가면서 i++ 시키기땜에 하나 빼서 한다.
		}
	}
	SAFE_DELETE( pSox );

	return true;
}

int CDNExtManager::GetValue( int nClassID, int nLevel, PlayerLevelTableIndex::ePlayerLevelTableIndex Index )
{
	std::map<int, LevelValue *>::iterator it = m_nMapTable.find( nClassID );
	if( it == m_nMapTable.end() ) return -1;
	return it->second[nLevel-1].nValue[Index];
}

float CDNExtManager::GetValueFloat( int nClassID, int nLevel, PlayerLevelTableIndex::ePlayerLevelTableIndex Index )
{
	std::map<int, LevelValue *>::iterator it = m_nMapTable.find( nClassID );
	if( it == m_nMapTable.end() ) return -1.f;
	return it->second[nLevel-1].fValue[Index];
}

// GlobalWeight
bool CDNExtManager::LoadGlobalWeight()
{
	DNTableFileFormat *pSox = LoadExtTable( "GlobalWeightTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"GlobalWeightTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _nID;
		int _Value;
	};

	TempFieldNum sFieldNum;
	sFieldNum._nID = pSox->GetFieldNum("ID");
	sFieldNum._Value = pSox->GetFieldNum("_Value");

	//##################################################################
	// Load
	//##################################################################

	char *pStr = NULL, Dest[256] = {0,};

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TGlobalWeightData *pData = new TGlobalWeightData;
		memset(pData, 0, sizeof(TGlobalWeightData));

		pData->nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(pData->nID);
		pData->fValue = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Value)->GetFloat();

		std::pair<TMapGlobalWeight::iterator,bool> Ret = m_pMapGlobalWeight.insert(make_pair(pData->nID, pData));
		if (Ret.second == false) delete pData;
	}

	SAFE_DELETE( pSox );

	return true;
}

bool CDNExtManager::LoadGlobalWeightInt()
{
	DNTableFileFormat *pSox = LoadExtTable( "GlobalWeightIntTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"GlobalWeightIntTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _nID;
		int _Value;
	};

	TempFieldNum sFieldNum;
	sFieldNum._nID = pSox->GetFieldNum("ID");
	sFieldNum._Value = pSox->GetFieldNum("_Value");

	//##################################################################
	// Load
	//##################################################################

	char *pStr = NULL, Dest[256] = {0,};

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TGlobalWeightIntData *pData = new TGlobalWeightIntData;
		memset(pData, 0, sizeof(TGlobalWeightIntData));

		pData->nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(pData->nID);
		pData->nValue = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Value)->GetInteger();

		std::pair<TMapGlobalWeightInt::iterator,bool> Ret = m_pMapGlobalWeightInt.insert(make_pair(pData->nID, pData));
		if (Ret.second == false) delete pData;
	}

	SAFE_DELETE( pSox );

	return true;
}

// GlobalWeight
TGlobalWeightData *CDNExtManager::GetGlobalWeightData(int nID)
{
	if (nID <= 0) return NULL;

	TMapGlobalWeight::iterator iter = m_pMapGlobalWeight.find(nID);
	if (iter == m_pMapGlobalWeight.end()) return NULL;

	return iter->second;
}

float CDNExtManager::GetGlobalWeightValueFloat(int nID)
{
	if (nID <= 0) return 0.f;

	TGlobalWeightData *pData = GetGlobalWeightData(nID);
	if (!pData) return 0.f;

	return pData->fValue;
}

int CDNExtManager::GetGlobalWeightValue(int nID)
{
	if (nID <= 0) return 0;

	TGlobalWeightData *pData = GetGlobalWeightData(nID);
	if (!pData) return 0;

	return (int)pData->fValue;
}

TGlobalWeightIntData * CDNExtManager::GetGlobalWeightIntData(int nID)
{
	if (nID <= 0) return NULL;

	TMapGlobalWeightInt::iterator iter = m_pMapGlobalWeightInt.find(nID);
	if (iter == m_pMapGlobalWeightInt.end()) return NULL;

	return iter->second;
}

float CDNExtManager::GetGlobalWeightIntValueFloat(int nID)
{
	if (nID <= 0) return 0.f;

	TGlobalWeightIntData *pData = GetGlobalWeightIntData(nID);
	if (!pData) return 0.f;

	return (float)pData->nValue;
}

int CDNExtManager::GetGlobalWeightIntValue(int nID)
{
	if (nID <= 0) return 0;

	TGlobalWeightIntData *pData = GetGlobalWeightIntData(nID);
	if (!pData) return 0;

	return pData->nValue;
}

// Cash
bool CDNExtManager::ReLoadCashCommodity()
{
	return LoadCashCommodity(true);
}

bool CDNExtManager::ReLoadCashPackage()
{
	return LoadCashPackage(true);
}

bool CDNExtManager::LoadCashCommodity(bool bReload)
{
	DNTableFileFormat *pSox = LoadExtTable( "CashCommodity" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"CashCommodity failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TMapCashCommodity BackupMapCashCommodity;
	if (bReload)
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		BackupMapCashCommodity = m_pMapCashCommodity;
		m_pMapCashCommodity.clear();
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SN;
#if defined(_JP)
		int _SNJPN;
#endif	// #if defined(_JP)
		int _Category;
		int _SubCategory;
		int _NameID;
		std::vector<int> _ItemID;
		std::vector<int> _LinkSN;
		int _Period;
		int _Price;
		int _Count;
		int _Priority;
		int _OnSale;
		int _OnSaleReal;
		int _State;
		int _Limit;
		int _ReserveGive;
		int _Reserve;
		int _validity;
		int _ReserveAble;
		int _PresentAble;
		int _ItemSort;
		int _PriceFix;
		int _CartAble;
		int _VIPSell;
		int _VIPLevel;
		int _Pay;
		int _PaySale;
		int _VIPPoint;
#if defined(PRE_ADD_CASH_REFUND)
		int _NoRefund;
#endif
#if defined(PRE_ADD_SALE_COUPON)
		int _UseCoupon;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
		int _SeedAble;
		int _Seed;
		int _SeedGive;
#endif
	};

	TempFieldNum sFieldNum;
	sFieldNum._SN = pSox->GetFieldNum("_SN");
#if defined(_JP)
	sFieldNum._SNJPN = pSox->GetFieldNum("_SNJPN");
#endif	// #if defined(_JP)
	sFieldNum._Category = pSox->GetFieldNum("_Category");
	sFieldNum._SubCategory = pSox->GetFieldNum("_SubCategory");
	sFieldNum._NameID = pSox->GetFieldNum("_NameID");
	sFieldNum._ItemID.reserve(COMMODITYITEMMAX);
	for (int j = 0; j < COMMODITYITEMMAX; ++j){
		sprintf_s(szTemp, "_ItemID%02d", j + 1);
		sFieldNum._ItemID.push_back(pSox->GetFieldNum(szTemp));
	}
	sFieldNum._LinkSN.reserve(COMMODITYLINKMAX);
	for (int j = 0; j < COMMODITYLINKMAX; ++j){
		sprintf_s(szTemp, "_LinkSN%02d", j + 1);
		sFieldNum._LinkSN.push_back(pSox->GetFieldNum(szTemp));
	}
	sFieldNum._Period = pSox->GetFieldNum("_Period");
	sFieldNum._Price = pSox->GetFieldNum("_Price");
	sFieldNum._Count = pSox->GetFieldNum("_Count");
	sFieldNum._Priority = pSox->GetFieldNum("_Priority");
	sFieldNum._OnSale = pSox->GetFieldNum("_OnSale");
	sFieldNum._OnSaleReal = pSox->GetFieldNum("_OnSaleREAL");
	sFieldNum._State = pSox->GetFieldNum("_State");
	sFieldNum._Limit = pSox->GetFieldNum("_Limit");
	sFieldNum._ReserveGive = pSox->GetFieldNum("_ReserveGive");
	sFieldNum._Reserve = pSox->GetFieldNum("_Reserve");
	sFieldNum._validity = pSox->GetFieldNum("_validity");
	sFieldNum._ReserveAble = pSox->GetFieldNum("_ReserveAble");
	sFieldNum._PresentAble = pSox->GetFieldNum("_PresentAble");
	sFieldNum._ItemSort = pSox->GetFieldNum("_ItemSort");
	sFieldNum._PriceFix = pSox->GetFieldNum("_PriceFix");
	sFieldNum._CartAble = pSox->GetFieldNum("_CartAble");
	sFieldNum._VIPSell = pSox->GetFieldNum("_VIPSell");
	sFieldNum._VIPLevel = pSox->GetFieldNum("_VIPLevel");
	sFieldNum._Pay = pSox->GetFieldNum("_Pay");
	sFieldNum._PaySale = pSox->GetFieldNum("_PaySale");
	sFieldNum._VIPPoint = pSox->GetFieldNum("_VIPPoint");
#if defined(PRE_ADD_CASH_REFUND)
	sFieldNum._NoRefund = pSox->GetFieldNum("_NoRefund");
#endif
#if defined(PRE_ADD_SALE_COUPON)
	sFieldNum._UseCoupon = pSox->GetFieldNum("_UseCoupon");
#endif //#if defined(PRE_ADD_SALE_COUPON)
#if defined( PRE_ADD_NEW_MONEY_SEED )
	sFieldNum._SeedAble = pSox->GetFieldNum("_SeedAble");
	sFieldNum._Seed = pSox->GetFieldNum("_Seed");
	sFieldNum._SeedGive = pSox->GetFieldNum("_SeedGive");
#endif

	//##################################################################
	// Load
	//##################################################################

	char *pStr = NULL, Dest[256] = {0,};
	int nJob	= 0;

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TCashCommodityData *pCashData = new TCashCommodityData;
		memset(pCashData, 0, sizeof(TCashCommodityData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pCashData->nSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SN)->GetInteger();
#if defined(_JP)
		pCashData->strJPSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SNJPN)->GetString();
#endif	// #if defined(_JP)
		pCashData->cCategory = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Category)->GetInteger();
		pCashData->nSubCategory = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SubCategory)->GetInteger();
#if defined(_CASHSERVER)
		int nNameID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._NameID)->GetInteger();
		if( nNameID > 0 )
		{
			std::wstring wStr = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nNameID);
	        	ToMultiString(wStr, pCashData->strName);
		}
#endif	// #if defined(_CASHSERVER)

		for (int j = 0; j < COMMODITYITEMMAX; j++){
			pCashData->nItemID[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemID[j])->GetInteger();
		}

		for (int j = 0; j < COMMODITYLINKMAX; j++){
			pCashData->nLinkSN[j] = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._LinkSN[j])->GetInteger();
		}

		pCashData->wPeriod = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Period)->GetInteger();
		pCashData->nPrice = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Price)->GetInteger();
		pCashData->nCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Count)->GetInteger();
		pCashData->nPriority = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Priority)->GetInteger();
		pCashData->bOnSale = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._OnSale)->GetInteger() ? true : false;
		pCashData->bOnSaleReal = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._OnSaleReal)->GetInteger() ? true : false;
		pCashData->cState = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._State)->GetInteger();
		pCashData->bLimit = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Limit)->GetInteger() ? true : false;
		pCashData->bReserveGive = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReserveGive)->GetInteger() ? true : false;
		pCashData->nReserve = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Reserve)->GetInteger();
		pCashData->nValidity = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._validity)->GetInteger();
		pCashData->bReserveAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ReserveAble)->GetInteger() ? true : false;
		pCashData->bPresentAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PresentAble)->GetInteger() ? true : false;
		pCashData->cItemSort = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ItemSort)->GetInteger();
		pCashData->nPriceFix = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PriceFix)->GetInteger();
		pCashData->bCartAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CartAble)->GetInteger() ? true : false;
		pCashData->bVIPSell = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPSell)->GetInteger() ? true : false;
		pCashData->nVIPLevel = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPLevel)->GetInteger();
		pCashData->bAutomaticPay = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Pay)->GetInteger() ? true : false;
		pCashData->nAutomaticPaySale = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._PaySale)->GetInteger();
		pCashData->nVIPPoint = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VIPPoint)->GetInteger();
#if defined(PRE_ADD_CASH_REFUND)
		pCashData->bNoRefund = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._NoRefund)->GetInteger() ? true : false;
#endif
#if defined(PRE_ADD_SALE_COUPON)
		pCashData->bUseCoupon = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._UseCoupon)->GetInteger() ? true : false;
#endif
#if defined( PRE_ADD_NEW_MONEY_SEED )
		pCashData->bSeedAble = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SeedAble)->GetInteger() ? true : false;
		pCashData->bSeedGive = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SeedGive)->GetInteger() ? true : false;
		if( pCashData->bSeedGive )
			pCashData->nSeed = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._Seed)->GetInteger();
#endif
		ScopeLock <CSyncLock> Lock(m_Sync);
		std::pair<TMapCashCommodity::iterator,bool> Ret = m_pMapCashCommodity.insert(make_pair(pCashData->nSN, pCashData));
		if (Ret.second == false) delete pCashData;
	}

	SAFE_DELETE( pSox );

	if (bReload)
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		SAFE_DELETE_PMAP(TMapCashCommodity, BackupMapCashCommodity);
	}
	return true;
}

bool CDNExtManager::GetCashCommodityData(int nSN, TCashCommodityData &Data)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
	{
		Data = *(*iter).second;
		return true;
	}
	return false;
}

int CDNExtManager::GetCashCommodityPrice(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nPrice;		//-1은 꽁짜
	return 0;
}

bool CDNExtManager::IsReserveCommodity(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->bReserveAble;
	return false;
}

bool CDNExtManager::IsPresentCommodity(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->bPresentAble;
	return false;
}

bool CDNExtManager::IsOnSaleCommodity(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->bOnSaleReal;
	return false;
}

bool CDNExtManager::IsLimitCommodity(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->bLimit;

	return false;
}

int CDNExtManager::GetCashCommodityPeriod(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->wPeriod;	// -1은 무한
	return 0;
}

int CDNExtManager::GetCashCommodityCount(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nCount;
	return 0;
}

int CDNExtManager::GetCashCommodityReserve(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nReserve;
	return 0;
}

#if defined( PRE_ADD_NEW_MONEY_SEED )
int CDNExtManager::GetCashCommoditySeed(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nSeed;
	return 0;
}
#endif


bool CDNExtManager::GetCashCommodityName(int nSN, std::string &outStr)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
	{
		outStr = (*iter).second->strName;
		return true;
	}
	return false;
}

bool CDNExtManager::GetCashCommodityNameW(int nSN, std::wstring &outStr)
{
	std::string strItemName;
	GetCashCommodityName(nSN, strItemName);

	USES_CONVERSION;
	outStr = A2CW(strItemName.c_str());

	return true;
}

#if defined(_JP)
bool CDNExtManager::GetCashCommodityJPSN(int nSN, std::string &outStr)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
	{
		outStr = (*iter).second->strJPSN.c_str();
		return true;
	}
	return false;
}
#endif	// #if defined(_JP)

int CDNExtManager::GetCashCommodityItem0(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nItemID[0];
	return 0;
}

int CDNExtManager::GetCashCommodityVIPPoint(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nVIPPoint;
	return 0;
}

bool CDNExtManager::GetCashCommodityPay(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->bAutomaticPay;
	return 0;
}

int CDNExtManager::GetCashCommodityPaySale(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nAutomaticPaySale;
	return 0;
}

int CDNExtManager::GetCashCommodityVIPAutomaticPaySalePrice(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		return (*iter).second->nPrice - (int)((*iter).second->nPrice * (*iter).second->nAutomaticPaySale / 100);
	return 0;
}

void CDNExtManager::SetProhibitSaleList()
{
#if defined(_CASHSERVER) || defined(_DBSERVER)
#if defined(_CASHSERVER)
	CDNSQLMembership *pMembershipDB = g_pSQLManager->FindMembershipDB(0);
#elif defined(_DBSERVER)
	CDNSQLMembership *pMembershipDB = g_SQLConnectionManager.FindMembershipDB(0);
#endif	// _CASHSERVER, _DBSERVER
	if (!pMembershipDB) return;

	m_VecProhibitSaleList.clear();
	pMembershipDB->QueryGetListBanOfSale(g_pExtManager->m_VecProhibitSaleList);

	ReLoadCashCommodity();
#endif	// #if defined(_CASHSERVER) || defined(_DBSERVER)

	if (m_VecProhibitSaleList.empty()) return;

	ScopeLock <CSyncLock> Lock(m_Sync);

	TCashCommodityData *pCashData = NULL;
	for (int i = 0; i < (int)m_VecProhibitSaleList.size(); i++)
	{
		TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(m_VecProhibitSaleList[i]);
		if (m_pMapCashCommodity.end() != iter)
			(*iter).second->bOnSaleReal = false;
	}
}

#if defined(PRE_ADD_CASH_REFUND)
bool CDNExtManager::GetCashCommodityNoRefund(int nID, int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TCashCommodityData *pCashData = NULL;
	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		pCashData = (*iter).second;
	
	bool bResult; 
	if (!pCashData)
	{
		bResult = true;		// 불가능..
		return bResult;
	}

	bResult = pCashData->bNoRefund;
	if( !bResult && nID > 0) // 환불 가능일때 타입으로 한번 더 확인하자..기획팀 실수 방지..
	{
		BYTE bType = GetItemMainType(nID);
		switch(bType)
		{
		case ITEMTYPE_REBIRTH_COIN :
		case ITEMTYPE_INVENTORY_SLOT :
		case ITEMTYPE_WAREHOUSE_SLOT :
		case ITEMTYPE_GESTURE :
		case ITEMTYPE_GUILDWARE_SLOT :
		case ITEMTYPE_FARM_VIP:
		case ITEMTYPE_GLYPH_SLOT:
		case ITEMTYPE_PERIOD_PLATE:
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMTYPE_PERIOD_TALISMAN_EX:
#endif
			bResult = true;
			break;
		}
	}	
	return bResult;
}
#endif

#if defined(PRE_ADD_SALE_COUPON)
bool CDNExtManager::GetCashCommodityUseCoupon(int nSN)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TCashCommodityData *pCashData = NULL;
	TMapCashCommodity::iterator iter = m_pMapCashCommodity.find(nSN);
	if (m_pMapCashCommodity.end() != iter)
		pCashData = (*iter).second;

	if (!pCashData)		
		return false;
	return pCashData->bUseCoupon;
}
#endif

bool CDNExtManager::LoadCashPackage(bool bReload)
{
	DNTableFileFormat *pSox = LoadExtTable( "CashPackageTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"CashPackageTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TMapCashPackage BackupMapCashPackage;
	if (bReload)
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		BackupMapCashPackage = m_pMapCashPackage;
		m_pMapCashPackage.clear();
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SN;
		std::vector<int> _CommodityID;
	};

	TempFieldNum sFieldNum;
	sFieldNum._SN = pSox->GetFieldNum("_SN");
	sFieldNum._CommodityID.reserve(PACKAGEITEMMAX);
	for (int j = 0; j < PACKAGEITEMMAX; ++j){
		sprintf_s(szTemp, "_CommodityID%02d", j);
		sFieldNum._CommodityID.push_back(pSox->GetFieldNum(szTemp));
	}

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++){
		TCashPackageData *pCashData = new TCashPackageData;
		memset(pCashData, 0, sizeof(TCashPackageData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);

		pCashData->nSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SN)->GetInteger();

		for (int j = 0; j < PACKAGEITEMMAX; j++){
			int nItemSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._CommodityID[j])->GetInteger();
			if (nItemSN > 0)
				pCashData->nVecCommoditySN.push_back(nItemSN);
		}

		ScopeLock <CSyncLock> Lock(m_Sync);
		std::pair<TMapCashPackage::iterator,bool> Ret = m_pMapCashPackage.insert(make_pair(pCashData->nSN, pCashData));
		if (Ret.second == false) delete pCashData;
	}

	if (bReload)
	{
		ScopeLock <CSyncLock> Lock(m_Sync);
		SAFE_DELETE_PMAP(TMapCashPackage, BackupMapCashPackage);
	}

	SAFE_DELETE( pSox );
	return true;
}

bool CDNExtManager::GetCashPackageData(int nSN, TCashPackageData &Data)
{
	ScopeLock <CSyncLock> Lock(m_Sync);

	TMapCashPackage::iterator iter = m_pMapCashPackage.find(nSN);
	if (iter != m_pMapCashPackage.end()) 
	{
		Data = *(*iter).second;
		return true;
	}
	return false;
}

bool CDNExtManager::LoadCashLimit()
{
	DNTableFileFormat *pSox = LoadExtTable( "CashLimitTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"CashLimitTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	char szTemp[MAX_PATH] = { 0, };

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _SN;
		int _StartDate;
		int _EndDate;
		int _BuyAbleCount;
#if defined(PRE_ADD_LIMITED_CASHITEM)
		int _SellCount;
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)
	};

	TempFieldNum sFieldNum;
	sFieldNum._SN = pSox->GetFieldNum("_SN");
	sFieldNum._StartDate = pSox->GetFieldNum("_StartDate");
	sFieldNum._EndDate = pSox->GetFieldNum("_EndDate");
	sFieldNum._BuyAbleCount = pSox->GetFieldNum("_BuyAbleCount");
#if defined(PRE_ADD_LIMITED_CASHITEM)
	sFieldNum._SellCount = pSox->GetFieldNum("_Sellcount");
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)

	//##################################################################
	// Load
	//##################################################################
	for (int i = 0; i < pSox->GetItemCount(); i++){
		TCashLimitData *pCashData = new TCashLimitData;
		memset(pCashData, 0, sizeof(TCashLimitData));

		int nID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nID);		

		pCashData->nSN = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SN)->GetInteger();

		std::string strStartDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._StartDate)->GetString();
		std::string strEndDate = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._EndDate)->GetString();
		pCashData->tStartDate = GetTimeForTextDate(strStartDate);
		pCashData->tEndDate = GetTimeForTextDate(strEndDate);

		pCashData->nBuyAbleCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._BuyAbleCount)->GetInteger();
#if defined(PRE_ADD_LIMITED_CASHITEM)
		pCashData->nLimitedSellCount = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SellCount)->GetInteger();
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM)

		std::pair<TMapCashLimit::iterator,bool> Ret = m_pMapCashLimit.insert(make_pair(pCashData->nSN, pCashData));
		if (Ret.second == false) delete pCashData;
	}

	SAFE_DELETE( pSox );

	return true;
}

time_t CDNExtManager::GetTimeForTextDate(std::string& strDate)
{
	std::vector<std::string> tokens;
	TokenizeA(strDate, tokens, "/");
	if( tokens.size() != 3 ) // 00/00/00 Format
		return 0;

	struct tm _tm={0,};
#if defined(_US)	 // USA mm/dd/yy 월/일/년
	_tm.tm_mon = atoi(tokens[0].c_str())-1;	
	_tm.tm_mday = atoi(tokens[1].c_str());	
	_tm.tm_year = atoi(tokens[2].c_str());
	if( _tm.tm_year < 2000 )
		_tm.tm_year += 2000;
	_tm.tm_year -= 1900;
#elif defined(_RU) // RUS dd/mm/yy 일/월/년
	_tm.tm_mday = atoi(tokens[0].c_str());
	_tm.tm_mon = atoi(tokens[1].c_str())-1;
	_tm.tm_year = atoi(tokens[2].c_str());
	if( _tm.tm_year < 2000 )
		_tm.tm_year += 2000;
	_tm.tm_year -= 1900;
#else	// yy/mm/dd 년/월/일
	_tm.tm_year = atoi(tokens[0].c_str());
	if( _tm.tm_year < 2000 )
		_tm.tm_year += 2000;
	_tm.tm_year -= 1900;
	_tm.tm_mon = atoi(tokens[1].c_str())-1;
	_tm.tm_mday = atoi(tokens[2].c_str());
#endif
	
	return mktime(&_tm);
}

bool CDNExtManager::IsOnSaleDate(int nSN)
{
	if (IsLimitCommodity(nSN) == false) return true;	// 한정판매 아님

	TCashLimitData *pCash = GetCashLimitData(nSN);
	if (pCash) 
	{
		time_t now;
		now = time(NULL);
		if( pCash->tStartDate > 0 && pCash->tEndDate > 0 )
		{
			struct tm tm_now, tm_end;			
			localtime_s(&tm_now, &now);	
			localtime_s(&tm_end, &pCash->tEndDate);
			if( tm_now.tm_year > tm_end.tm_year )
				return false;
			// 365일 기준임..tm_yday
			if( tm_now.tm_year == tm_end.tm_year && tm_now.tm_yday > tm_end.tm_yday )
				return false;

			return true;
		}
	}	
	return true; // CashLimitTable에 없으면 그냥 통과하자 혹시나 테이블에 추가안해서 판매가 안될까봐.
}

TCashLimitData *CDNExtManager::GetCashLimitData(int nSN)
{
	TMapCashLimit::iterator iter = m_pMapCashLimit.find(nSN);
	if (iter != m_pMapCashLimit.end()) return iter->second;
	return NULL;
}

int CDNExtManager::GetCashBuyAbleCount(int nSN)
{
	TCashLimitData *pCash = GetCashLimitData(nSN);
	if (pCash) return pCash->nBuyAbleCount;
	return 0;
}

#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)
int CDNExtManager::GetCashLimitedItemCount(int nSN)
{
	TCashLimitData *pCash = GetCashLimitData(nSN);
	if (pCash)
	{
		int nLimitMax = 0;
		if (g_pLimitedCashItemRepository && g_pLimitedCashItemRepository->GetChangedLimitedItemMax(nSN, nLimitMax))
			return nLimitMax;
		return pCash->nLimitedSellCount;
	}
	return 0;
}
#endif		//#if defined(PRE_ADD_LIMITED_CASHITEM) && defined(_CASHSERVER)

bool CDNExtManager::LoadItemData()
{
	DNTableFileFormat *pSox = LoadExtTable( "ItemTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"ItemTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _NameID;
		int _NameIDParam;
		int	_Type;
		int	_TypeParam1;
		int	_TypeParam2;
		int _LevelLimit;
		int _Rank;
		int _Reversion;
		int _IsCash;
		int _IsAuthentication;
		int _IsDestruction;
		int _Amount;
		int _SellAmount;
		int _OverlapCount;
		int _SkillID;
		int _SkillLevel;
		int _SkillUsingType;
		int _AllowMapTypes;
		int _NeedJobClass;
		int _EnchantID;
		int	_SealCount;
		int _NeedBuyItem;
		int _NeedBuyItemCount;
		int _NeedPvPRank;
	};

	TempFieldNum sFieldNum;
	sFieldNum._NameID			= pSox->GetFieldNum( "_NameID" );
	sFieldNum._NameIDParam		= pSox->GetFieldNum( "_NameIDParam" );
	sFieldNum._Type				= pSox->GetFieldNum( "_Type" );
	sFieldNum._TypeParam1		= pSox->GetFieldNum( "_TypeParam1" );
	sFieldNum._TypeParam2		= pSox->GetFieldNum( "_TypeParam2" );
	sFieldNum._LevelLimit		= pSox->GetFieldNum( "_LevelLimit" );
	sFieldNum._Rank				= pSox->GetFieldNum( "_Rank" );
	sFieldNum._Reversion		= pSox->GetFieldNum( "_Reversion" );
	sFieldNum._IsCash			= pSox->GetFieldNum( "_IsCash" );
	sFieldNum._IsAuthentication = pSox->GetFieldNum( "_IsAuthentication" );
	sFieldNum._IsDestruction	= pSox->GetFieldNum( "_IsDestruction" );
	sFieldNum._Amount			= pSox->GetFieldNum( "_Amount" );
	sFieldNum._SellAmount		= pSox->GetFieldNum( "_SellAmount" );
	sFieldNum._OverlapCount		= pSox->GetFieldNum( "_OverlapCount" );
	sFieldNum._SkillID			= pSox->GetFieldNum( "_SkillID" );
	sFieldNum._SkillLevel		= pSox->GetFieldNum( "_SkillLevel" );
	sFieldNum._SkillUsingType	= pSox->GetFieldNum( "_SkillUsingType" );
	sFieldNum._AllowMapTypes	= pSox->GetFieldNum( "_AllowMapTypes" );
	sFieldNum._NeedJobClass		= pSox->GetFieldNum( "_NeedJobClass" );
	sFieldNum._EnchantID		= pSox->GetFieldNum( "_EnchantID" );
	sFieldNum._SealCount		= pSox->GetFieldNum( "_SealCount" );
	sFieldNum._NeedBuyItem		= pSox->GetFieldNum( "_NeedBuyItem" );
	sFieldNum._NeedBuyItemCount	= pSox->GetFieldNum( "_NeedBuyItemCount" );
	sFieldNum._NeedPvPRank		= pSox->GetFieldNum( "_NeedPvPRank" );

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); i++)
	{
		TItemData* pItemData = new TItemData;
		pItemData->Reset();

		pItemData->nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( pItemData->nItemID );

		pItemData->nType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();
		pItemData->nTypeParam[0] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TypeParam1 )->GetInteger();
		pItemData->nTypeParam[1] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._TypeParam2 )->GetInteger();
		pItemData->cLevelLimit = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._LevelLimit )->GetInteger();
		pItemData->cRank = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Rank )->GetInteger();
		pItemData->cReversion = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Reversion )->GetInteger();
		pItemData->IsCash = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsCash )->GetInteger() ? true : false;
		pItemData->IsAuthentication = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsAuthentication )->GetInteger() ? true : false;
		pItemData->IsDestruction = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._IsDestruction )->GetInteger() ? true : false;
		pItemData->nAmount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Amount )->GetInteger();
		pItemData->nSellAmount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SellAmount )->GetInteger();
		pItemData->nOverlapCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._OverlapCount )->GetInteger();
		pItemData->nSkillID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillID )->GetInteger();
		pItemData->cSkillLevel = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillLevel )->GetInteger();
		pItemData->cSkillUsingType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SkillUsingType )->GetInteger();
		pItemData->nAllowMapType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._AllowMapTypes )->GetInteger();
		pItemData->nNeedBuyItemID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyItem )->GetInteger();
		pItemData->nNeedBuyItemCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedBuyItemCount )->GetInteger();
		pItemData->nNeedPvPRank = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedPvPRank )->GetInteger();

		std::vector<std::string> JobTokens;

		char *pStr = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._NeedJobClass )->GetString();
		TokenizeA(pStr, JobTokens, ";");

		for (int j = 0; j < (int)JobTokens.size(); j++){
			int iJob = atoi(JobTokens[j].c_str());
			if( iJob > 0 )
				pItemData->nNeedJobClassList.push_back(iJob);
		}

		pItemData->nEnchantID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._EnchantID)->GetInteger();
		pItemData->cSealCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SealCount )->GetInteger();

		std::pair<TMapItemData::iterator,bool> Ret = m_pItemData.insert(make_pair(pItemData->nItemID, pItemData));
		if( Ret.second == false )
			delete pItemData;
	}

	SAFE_DELETE( pSox );

	return true;
}

TItemData* CDNExtManager::GetItemData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapItemData::iterator iter = m_pItemData.find(nItemID);
	if (iter != m_pItemData.end()){
		return iter->second;
	}

	return NULL;
}

int CDNExtManager::GetItemMainType(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nType;
}

int CDNExtManager::GetItemOverlapCount(int nItemID)
{
	if (nItemID <= 0) return 0;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return 0;

	return pItemData->nOverlapCount;
}

bool CDNExtManager::IsCashItem(int nItemID)
{
	if (nItemID <= 0) return false;

	TItemData *pItemData = GetItemData(nItemID);
	if (!pItemData) return false;

	return pItemData->IsCash;
}

// VehicleTable
bool CDNExtManager::LoadVehicleData()
{
	DNTableFileFormat *pSox = LoadExtTable( "vehicletable" );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"vehicletable failed\r\n" );
		SAFE_DELETE(pSox);
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"vehicletable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _VehicleClassID;
		int _VehicleActorID;
		int _DefaultBodyParts;
		int _DefaultPartsA;
		int _DefaultPartsB;
		int _VehicleSummonTime;
		int _VehicleDefaultSpeed;
		int _nPetLevelTypeID;
		int _nPetSkillID1;
		int _nPetSkillID2;
		int _nRange;
	};

	TempFieldNum sFieldNum;
	sFieldNum._VehicleClassID = pSox->GetFieldNum("_VehicleClassID");
	sFieldNum._VehicleActorID = pSox->GetFieldNum("_VehicleActorID");
	sFieldNum._DefaultBodyParts = pSox->GetFieldNum("_DefaultBodyParts");
	sFieldNum._DefaultPartsA = pSox->GetFieldNum("_DefaultPartsA");
	sFieldNum._DefaultPartsB = pSox->GetFieldNum("_DefaultPartsB");
	sFieldNum._VehicleSummonTime = pSox->GetFieldNum("_VehicleSummonTime");
	sFieldNum._VehicleDefaultSpeed = pSox->GetFieldNum("_VehicleDefaultSpeed");
	sFieldNum._nPetLevelTypeID = pSox->GetFieldNum("_PetLevelTypeID");
	sFieldNum._nPetSkillID1 = pSox->GetFieldNum("_PetSKILLID1");
	sFieldNum._nPetSkillID2 = pSox->GetFieldNum("_PetSKILLID2");
	sFieldNum._nRange = pSox->GetFieldNum("_Range");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		TVehicleData Data = {0, };
		Data.nItemID = nItemID;
		Data.nVehicleClassID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleClassID)->GetInteger();
		Data.nVehicleActorID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleActorID)->GetInteger();
		Data.nDefaultBody = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultBodyParts)->GetInteger();
		Data.nDefaultParts1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultPartsA)->GetInteger();
		Data.nDefaultParts2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._DefaultPartsB)->GetInteger();
		Data.nVehicleSummonTime = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleSummonTime)->GetInteger();
		Data.nVehicleDefaultSpeed = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleDefaultSpeed)->GetInteger();
		Data.nPetLevelTypeID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetLevelTypeID)->GetInteger();
		Data.nPetSkillID1 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetSkillID1)->GetInteger();
		Data.nPetSkillID2 = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nPetSkillID2)->GetInteger();
		Data.nRange = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._nRange)->GetInteger();

		m_MapVehicle.insert(make_pair(Data.nItemID, Data));
	}

	SAFE_DELETE(pSox);
	return true;
}

TVehicleData *CDNExtManager::GetVehicleData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapVehicleData::iterator iter = m_MapVehicle.find(nItemID);
	if (iter == m_MapVehicle.end()) return NULL;

	return &(iter->second);
}

int CDNExtManager::GetVehicleClassID(int nItemID)
{
	TVehicleData *pVehicle = GetVehicleData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nVehicleClassID;
}

// VehiclePartsTable
bool CDNExtManager::LoadVehiclePartsData()
{
	DNTableFileFormat *pSox = LoadExtTable( "vehiclepartstable" );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"vehiclepartstable failed\r\n" );
		SAFE_DELETE(pSox);
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"vehiclepartstable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _VehicleClassID;
		int _VehiclePartsType;
		int _SkinName;
		int _AniName;
		int _ActName;
		int _SkinColor;
	};

	TempFieldNum sFieldNum;
	sFieldNum._VehicleClassID = pSox->GetFieldNum("_VehicleClassID");
	sFieldNum._VehiclePartsType = pSox->GetFieldNum("_VehiclePartsType");
	sFieldNum._SkinName = pSox->GetFieldNum("_SkinName");
	sFieldNum._AniName = pSox->GetFieldNum("_AniName");
	sFieldNum._ActName = pSox->GetFieldNum("_ActName");
	sFieldNum._SkinColor = pSox->GetFieldNum("_SkinColor");

	//##################################################################
	// Load
	//##################################################################

	for (int i = 0; i < pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetItemID(i);
		int nIdx = pSox->GetIDXprimary(nItemID);

		//TVehiclePartsData Data = {0, };
		//2010.10.7 haling STL 보호 위해 생성자로 변환
		TVehiclePartsData Data;
		Data.nItemID = nItemID;
		Data.nVehicleClassID = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehicleClassID)->GetInteger();
		Data.nVehiclePartsType = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._VehiclePartsType)->GetInteger() + 1;
		Data.strSkinName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkinName)->GetString();
		Data.strAniName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._AniName)->GetString();
		Data.strActName = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._ActName)->GetString();
		Data.nSkinColor = pSox->GetFieldFromLablePtr(nIdx, sFieldNum._SkinColor)->GetInteger();

		m_MapVehicleParts.insert(make_pair(Data.nItemID, Data));
	}

	SAFE_DELETE(pSox);
	return true;
}

TVehiclePartsData *CDNExtManager::GetVehiclePartsData(int nItemID)
{
	if (nItemID <= 0) return NULL;

	TMapVehicleParts::iterator iter = m_MapVehicleParts.find(nItemID);
	if (iter == m_MapVehicleParts.end()) return NULL;

	return &(iter->second);
}

int CDNExtManager::GetVehiclePartsType(int nItemID)
{
	TVehiclePartsData *pVehicle = GetVehiclePartsData(nItemID);
	if (!pVehicle) return 0;

	return pVehicle->nVehiclePartsType;
}

bool CDNExtManager::LoadMailTableData()
{
	DNTableFileFormat *pSox = LoadExtTable( "MailTable" );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"MailTable failed\r\n" );
		SAFE_DELETE(pSox);
		return false;
	}


	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"MailTable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int MailType;
		int MailSender;
		int MailTitle;
		int MailText;
		int MailPresentItem[MAILATTACHITEMMAX];
		int Count[MAILATTACHITEMMAX];
		int	MailPresentMoney;
		int IsCash;
	};

	TempFieldNum sFieldNum;
	sFieldNum.MailType			= pSox->GetFieldNum( "_MailType" );
	sFieldNum.MailSender		= pSox->GetFieldNum( "_MailSender" );
	sFieldNum.MailTitle			= pSox->GetFieldNum( "_MailTitle" );
	sFieldNum.MailText			= pSox->GetFieldNum( "_MailText" );
	sFieldNum.MailPresentMoney	= pSox->GetFieldNum( "_MailPresentMoney" );
	sFieldNum.IsCash			= pSox->GetFieldNum( "_IsCash" );
	for( int i=1 ; i<=MAILATTACHITEMMAX ; ++i )
	{
		CHAR szBuf[MAX_PATH];

		sprintf( szBuf, "_MailPresentItem%d", i );
		sFieldNum.MailPresentItem[i-1] = pSox->GetFieldNum( szBuf );
		sprintf( szBuf, "_Count%d", i );
		sFieldNum.Count[i-1] = pSox->GetFieldNum( szBuf );
	}

	//##################################################################
	// Load
	//##################################################################


	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TMailTableData Data;
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary( nItemID );

		Data.Code					= static_cast<DBDNWorldDef::MailTypeCode::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailType )->GetInteger());
		Data.nSenderUIStringIndex	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailSender )->GetInteger();
		Data.nTitleUIStringIndex	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailTitle )->GetInteger();
		Data.nTextUIStringIndex		= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailText )->GetInteger();
		Data.IsCash					= pSox->GetFieldFromLablePtr( iIdx, sFieldNum.IsCash )->GetInteger() ? true : false;

		for( int i=0 ; i<MAILATTACHITEMMAX ; ++i )
		{
			if (Data.IsCash){
				Data.ItemSNArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentItem[i] )->GetInteger();
			}
			else{
				Data.ItemIDArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentItem[i] )->GetInteger();
				if( Data.ItemIDArr[i] <= 0 )
					continue;
				//_ASSERT( GetItemData( Data.ItemIDArr[i] ) );	 -- by robust
				if( GetItemData( Data.ItemIDArr[i] ) == NULL )
				{
					WCHAR wszBuf[MAX_PATH];
					wsprintf( wszBuf, L"MailTable ID:%d ItemID:%d not found\r\n", nItemID, Data.ItemIDArr[i] );
					g_Log.Log( LogType::_FILELOG, wszBuf );
					SAFE_DELETE(pSox);
					return false;
				}
				Data.ItemCountArr[i] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Count[i] )->GetInteger();
			}
		}

		Data.nPresentMoney = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MailPresentMoney )->GetInteger();

		m_MailTableData.insert( std::make_pair(nItemID,Data) );
	}

	SAFE_DELETE(pSox);
	return true;
}

TMailTableData* CDNExtManager::GetMailTableData( int iItemID )
{
	TMapMailTableData::iterator itor = m_MailTableData.find( iItemID );
	if( itor != m_MailTableData.end() )
		return &itor->second;

	return NULL;
}

bool CDNExtManager::LoadGuildWarRewardData()
{
	DNTableFileFormat *pSox = LoadExtTable( "guildwarmailtable" );
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"guildwarmailtable failed\r\n" );
		SAFE_DELETE(pSox);
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"guildwarmailtable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}	

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{		
		int	_ResultType;
		int	_ClassID;
		int	_MailID1;
		int	_MailID2;
		int _GuildFestPoint;
		int _GuildPoint;
	};

	TempFieldNum sFieldNum;	
	sFieldNum._ResultType = pSox->GetFieldNum("_ResultType");
	sFieldNum._ClassID = pSox->GetFieldNum("_ClassID");
	sFieldNum._MailID1 = pSox->GetFieldNum("_MailID1");
	sFieldNum._MailID2 = pSox->GetFieldNum("_MailID2");
	sFieldNum._GuildFestPoint = pSox->GetFieldNum("_GuildFestPoint");
	sFieldNum._GuildPoint = pSox->GetFieldNum("_GuildPoint");

	//##################################################################
	// Load
	//##################################################################

	for( int i=0; i<pSox->GetItemCount(); i++ ) 
	{
		int nItemID = pSox->GetItemID(i);
		int iIdx = pSox->GetIDXprimary( nItemID );

		TGuildWarRewardData* Data = new TGuildWarRewardData;
		memset( Data, 0, sizeof(TGuildWarRewardData) );
		Data->cType = (char)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ResultType )->GetInteger();
		Data->cClass = (char)pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ClassID )->GetInteger();
		Data->nMailID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MailID1 )->GetInteger();
		Data->nPresentID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MailID2 )->GetInteger();		
		Data->nGuildFestivalPoint = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GuildFestPoint)->GetInteger();		
		Data->nGuildPoint = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GuildPoint)->GetInteger();		

		m_pGuildWarRewardData.push_back(Data);
	}

	SAFE_DELETE(pSox);
	return true;
}

TGuildWarRewardData* CDNExtManager::GetGuildWarRewardData(char cType, char cClass)
{
	std::vector<TGuildWarRewardData*>::iterator iter;
	for( iter=m_pGuildWarRewardData.begin(); iter!=m_pGuildWarRewardData.end(); ++iter)
	{
		TGuildWarRewardData* pRewardData = (*iter);
		if( pRewardData->cType == cType && pRewardData->cClass == cClass )
			return pRewardData;
	}
	return NULL;
}

bool CDNExtManager::LoadJobTableData()
{
	DNTableFileFormat *pSox = LoadExtTable( "JobTable" );	
	if( !pSox )
	{
		g_Log.Log( LogType::_FILELOG, L"JobTable failed\r\n" );
		SAFE_DELETE(pSox);
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"JobTable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _JobNumber;
		int _ParentJob;
		int _MaxSPJob[ 5 ];
		int _Class;
	};

	TempFieldNum sFieldNum;
	sFieldNum._JobNumber	= pSox->GetFieldNum( "_JobNumber" );
	sFieldNum._ParentJob	= pSox->GetFieldNum( "_ParentJob" );

	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < 5; ++i )
	{
		sprintf_s( acBuffer, "_MaxSPJob%d", i );
		sFieldNum._MaxSPJob[ i ] = pSox->GetFieldNum( acBuffer );
	}

	sFieldNum._Class	= pSox->GetFieldNum( "_Class" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		TJobTableData Data;
		int nItemID = pSox->GetItemID(i);
		int iIdx	= pSox->GetIDXprimary( nItemID );

		Data.cJobNumber	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._JobNumber )->GetInteger();
		Data.cParentJob	= pSox->GetFieldFromLablePtr( iIdx, sFieldNum._ParentJob )->GetInteger();
		for( int k = 0; k < 5; ++k )
			Data.afMaxUsingSP[ k ] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._MaxSPJob[ k ] )->GetFloat();

		Data.cClass = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Class )->GetInteger();

		m_JobTableData.insert( std::make_pair(nItemID, Data) );

		if( nItemID >= JOB_KIND_MAX )
		{
			g_Log.Log( LogType::_FILELOG, L"MaxJob OverFlow %d/%d\r\n", nItemID, JOB_KIND_MAX );
			SAFE_DELETE(pSox);
			return false;
		}
	}

	SAFE_DELETE(pSox);
	return true;
}

TJobTableData* CDNExtManager::GetJobTableData( int nJobTableID )
{
	TMapJobTableData::iterator itor = m_JobTableData.find( nJobTableID );
	if( itor != m_JobTableData.end() )
	{
		TJobTableData * pTJobTableData = &(itor->second);
		if( pTJobTableData->cJobNumber >= 0 )		// 기본직업은 차수가 0 부터 시작이라 수정.
			return pTJobTableData;
	}

	return NULL;
}

bool CDNExtManager::LoadMapInfo()
{
	DNTableFileFormat *pSox = LoadExtTable( "MapTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"MapTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}
	
	TMapInfo *pMapInfo = NULL;
	char szTemp[256];
	char szLabel[64];
	memset(&szTemp, 0, sizeof(szTemp));

	for (int i = 0; i < pSox->GetItemCount(); i++){
		int nItemID = pSox->GetItemID(i);

		pMapInfo = new TMapInfo;

		pMapInfo->nMapID = nItemID;
		for( int j=0; j<10; j++ ) {
			sprintf_s( szLabel, "_ToolName%d", j + 1 );
			_strcpy(pMapInfo->szMapName[j], _countof(pMapInfo->szMapName[j]), pSox->GetFieldFromLablePtr(nItemID, szLabel)->GetString(), (int)strlen(pSox->GetFieldFromLablePtr(nItemID, szLabel)->GetString()));
		}

		pMapInfo->MapType = (GlobalEnum::eMapTypeEnum)pSox->GetFieldFromLablePtr(nItemID, "_MapType")->GetInteger();

		for (int j = 0; j < WORLDMAP_GATECOUNT; j++){
			sprintf_s(szTemp, "_Gate%d_MapIndex_txt", j + 1);
			char* pszGateMapIndexs = pSox->GetFieldFromLablePtr( nItemID, szTemp )->GetString();

			sprintf_s(szTemp, "_Gate%d_StartGate_txt", j + 1);
			char* pszGateStartIndexs = pSox->GetFieldFromLablePtr(nItemID, szTemp)->GetString();

			std::vector<string> vGateMapIndexs;
			TokenizeA(pszGateMapIndexs, vGateMapIndexs, ";");

			std::vector<string> vGateStartIndexs;
			TokenizeA(pszGateStartIndexs, vGateStartIndexs, ";");

			for (int k=0; k<vGateMapIndexs.size(); k++)
			{
				if (k < WORLDMAP_GATECOUNT)
				{
					pMapInfo->GateMapIndexs[j][k] = (USHORT)atoi(vGateMapIndexs[k].c_str());
					pMapInfo->GateStartIndexs[j][k] = (BYTE)atoi(vGateStartIndexs[k].c_str());
					pMapInfo->GateMapCount[j] = k+1;
				}
			}
		}
		m_pMapInfo.insert(make_pair(pMapInfo->nMapID, pMapInfo));
	}

	SAFE_DELETE(pSox);
	return true;
}

int CDNExtManager::GetMapIndexByGateNo(int nMapIndex, int nGateNo, int nSelect)
{
	if (nMapIndex < 0) return -1;
	if (nGateNo <= 0) return -1;
	if (nSelect < 0) return -1;

	TMapInfoMap::iterator iter = m_pMapInfo.find(nMapIndex);
	if (iter != m_pMapInfo.end())
	{
		if (nSelect < iter->second->GateMapCount[nGateNo-1])
			return iter->second->GateMapIndexs[nGateNo-1][nSelect];
	}

	return -1;
}

int CDNExtManager::GetGateNoByGateNo( int nMapIndex, int nGateNo, int nSelect )
{
	if (nMapIndex < 0) return -1;
	if (nGateNo < 0) return -1;

	TMapInfoMap::iterator iter = m_pMapInfo.find(nMapIndex);
	if (iter != m_pMapInfo.end()){
		if (nSelect < iter->second->GateMapCount[nGateNo-1])
			return iter->second->GateStartIndexs[nGateNo-1][nSelect];
	}

	return -1;
}

GlobalEnum::eMapTypeEnum CDNExtManager::GetMapType(int nMapIndex)
{
	if (nMapIndex < 0) 
		return GlobalEnum::MAP_UNKNOWN;

	TMapInfoMap::iterator iter = m_pMapInfo.find(nMapIndex);
	if (iter != m_pMapInfo.end()){
		return iter->second->MapType;
	}

	return GlobalEnum::MAP_UNKNOWN;
}

const char * CDNExtManager::GetMapName(int nMapIdx)
{
	if (nMapIdx < 0) return NULL;

	TMapInfoMap::iterator iter = m_pMapInfo.find(nMapIdx);
	if (iter != m_pMapInfo.end()){
		return iter->second->szMapName[0];
	}
	return NULL;
}

#if defined(_LOGINSERVER) || defined(_MASTERSERVER)
void CDNExtManager::SetOnOffServerInfo(char cWorldSetID, bool bOnline)
{
	if (m_pServerInfoList.empty()) return;

	for (TMapServerInfo::iterator iter = m_pServerInfoList.begin(); iter != m_pServerInfoList.end(); ++iter){
		if (iter->second->nWorldSetID == cWorldSetID){
			iter->second->bOnline = bOnline;
		}
	}
}

int CDNExtManager::GetWorldID()
{
	if (m_pServerInfoList.empty()) return -1;

	std::vector<std::string> IPList;
	bool bResult = GetLocalIp(IPList);
	if ( !bResult )
		return -1;
		
	TMapServerInfo::iterator iter;
	for (iter = m_pServerInfoList.begin(); iter != m_pServerInfoList.end(); ++iter)
	{
		for ( size_t i = 0 ; i < IPList.size() ; i++ )
		{
			if (strcmp(iter->second->vOwnedVillageList[0].szIP, IPList[i].c_str()) == 0) 
			{
				return iter->second->cWorldID;
			}
		}
	}

	_ASSERT_EXPR(0, L"채널정보에 아이피가 등록되어 있지 않습니다. 정홍철을 찾아주세요!");

	return -1;
}

WCHAR* CDNExtManager::GetWorldName(int nWorldSetID)
{
	if (m_pServerInfoList.empty()) return NULL;

	TMapServerInfo::iterator iter;
	for (iter = m_pServerInfoList.begin(); iter != m_pServerInfoList.end(); ++iter){
		if (iter->second->cWorldID == nWorldSetID){
			return iter->second->wszWorldName;
		}
	}

	return NULL;
}

// 삭제 예정 !!! 20090907
/*
char* CDNExtManager::GetLocalIp(OUT in_addr* pInAddr)
{
	WSADATA WsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (0 > iResult) {
		DN_RETURN(NULL);
	}

	char Name[256] = { 0, };

	gethostname(Name, sizeof(Name));
	PHOSTENT host = gethostbyname(Name);
	if (host) {
		if (pInAddr) {
			memcpy(pInAddr, host->h_addr_list[0], sizeof(in_addr));
		}
		return inet_ntoa(*(struct in_addr*)*host->h_addr_list);
	}

	WSACleanup();

	return NULL;
}
*/

bool CDNExtManager::GetLocalIp(OUT std::vector<std::string>& Out)
{
	char Name[256] = { 0, };
	char addr[16] = {0,};

	gethostname(Name,sizeof(Name));
	struct hostent* he = gethostbyname(Name);
	struct in_addr iaddr;
	memset(&iaddr,0,sizeof(iaddr));

	for(int i =0; he->h_addr_list[i] !=0; i++)
	{
		if(he->h_addrtype!=AF_INET)	  continue;
		memcpy(&iaddr,he->h_addr_list[i],sizeof(iaddr));
		_strcpy(addr, _countof(addr), inet_ntoa(iaddr), (int)strlen(inet_ntoa(iaddr)));
		Out.push_back(std::string(addr));
	}

	return Out.size() ? true : false;
}

#endif	// #if defined(_LOGINSERVER) || defined(_MASTERSERVER)

#if defined(_LOGINSERVER)
void CDNExtManager::GetChannelListByMapIndex(char cWorldID, int nMapIndex, int nLastMapIndex, sChannelInfo *ChannelArray, BYTE &cCount, TMapChannel *ChannelList)
{
	const TServerInfo *pServerInfo = GetServerInfo(cWorldID);
	if (!pServerInfo) return;
	if (pServerInfo->vOwnedVillageList.empty()) return;

	int nSearchMapIndex = nMapIndex;
	if (nMapIndex == 0) nSearchMapIndex = 1;
	else if (GetMapType(nMapIndex) != GlobalEnum::MAP_VILLAGE)
		nSearchMapIndex = nLastMapIndex;

	cCount = 0;
	TChannelInfoEx ChannelInfo;
	ChannelList->clear();

	for (int i = 0; i < (int)pServerInfo->vOwnedVillageList.size(); i++){
		if (pServerInfo->vOwnedVillageList[i].vOwnedChannelList.empty()) continue;

		for (int j = 0; j < (int)pServerInfo->vOwnedVillageList[i].vOwnedChannelList.size(); j++){
			if (pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j].nMapIdx != nSearchMapIndex) continue;

			memset(&ChannelInfo, 0, sizeof(TChannelInfoEx));
			ChannelInfo.Channel = pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j];
			ChannelInfo.cVillageID = pServerInfo->vOwnedVillageList[i].cVillageID;
			_strcpy(ChannelInfo.szIP, _countof(ChannelInfo.szIP), pServerInfo->vOwnedVillageList[i].szIP, (int)strlen(pServerInfo->vOwnedVillageList[i].szIP));
			ChannelInfo.nPort = pServerInfo->vOwnedVillageList[i].nPort;
			ChannelList->insert(make_pair(ChannelInfo.Channel.nChannelID, ChannelInfo));

			ChannelArray[cCount].nChannelID = pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j].nChannelID;
			ChannelArray[cCount].nMapIdx = pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j].nMapIdx;
			ChannelArray[cCount].nMaxUserCount = pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j].nChannelMaxUser;
			ChannelArray[cCount].nChannelAttribute = pServerInfo->vOwnedVillageList[i].vOwnedChannelList[j].nAttribute;
			_strcpy(ChannelArray[cCount].szIP, _countof(ChannelArray[cCount].szIP), pServerInfo->vOwnedVillageList[i].szIP, (int)strlen(pServerInfo->vOwnedVillageList[i].szIP));
			ChannelArray[cCount].nPort = pServerInfo->vOwnedVillageList[i].nPort;

			cCount++;
		}
	}
}

void CDNExtManager::GetServerList(TServerListData *ServerList, BYTE &cCount, bool bDenyWorld)
{
	if (m_pMapInfo.empty()) return;

	cCount = 0;
	TMapServerInfo::iterator iter;
	for (iter = m_pServerInfoList.begin(); iter != m_pServerInfoList.end(); ++iter)
	{
#if !defined(PRE_MOD_SELECT_CHAR)
		if (!iter->second->bOnline) continue;	// 일단 online인 애들만 뿌려주자
#endif	// #if defined(PRE_MOD_SELECT_CHAR)
#if defined(_TW)
		if( bDenyWorld && g_Config.nDenyWorld == iter->second->cWorldID ) 
			continue;
#endif //#if defined(_TW)
#if defined(PRE_IDN_PVP)
		if( bDenyWorld && g_Config.nPvPWorldID == iter->second->cWorldID )
			continue;
#endif // #if defined(PRE_IDN_PVP)
		ServerList[cCount].cWorldID = iter->second->cWorldID;
		wcsncpy(ServerList[cCount].wszServerName, iter->second->wszWorldName, WORLDNAMELENMAX);
		ServerList[cCount].bOnline = iter->second->bOnline;
		ServerList[cCount].bOnTop = (*iter).second->bOnTop;
		cCount++;
	}
}
#endif

#if defined(_LOGINSERVER)

DWORD ConvertD3DCOLORToR10G10B10( DWORD dwSource, float fIntensity )
{
	int nR = (int)(fIntensity * LOBYTE(HIWORD(dwSource)));
	int nG = (int)(fIntensity * HIBYTE(LOWORD(dwSource)));
	int nB = (int)(fIntensity * LOBYTE(LOWORD(dwSource)));
	ASSERT( nR >= 0 && nR < 1024);		// 1024 (4.0f) 를 넘는 경우는 없을거라 생각한다.
	ASSERT( nG >= 0 && nG < 1024);
	ASSERT( nB >= 0 && nB < 1024);
	return (DWORD)((nR<<20)|(nG<<10)|nB);
}

bool CDNExtManager::LoadDefaultCreateData()
{
	DNTableFileFormat *pSox = LoadExtTable( "DefaultCreateTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"DefaultCreateTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TDefaultCreateData *pDefaultCreate = NULL;
	char szLabel[64] = { 0, };
	int nIndex = 0;

	for (int i = 0; i < pSox->GetItemCount(); i++){
		pDefaultCreate = new TDefaultCreateData;
		memset(pDefaultCreate, 0, sizeof(TDefaultCreateData));

		nIndex = pSox->GetItemID(i);
		pDefaultCreate->cClassID = pSox->GetFieldFromLablePtr(nIndex, "_ClassID")->GetInteger();

		// Default Position
		pDefaultCreate->nCreateMapIndex = pSox->GetFieldFromLablePtr(nIndex, "_CreateMapIndex")->GetInteger();
		for (int j = 0; j < DEFAULTPOSITIONMAX; j++){
			sprintf_s(szLabel, "_CreateMapStartPosition_%02d_x", j + 1);
			pDefaultCreate->nCreateMapStartPositionX[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_CreateMapStartPosition_%02d_y", j + 1);
			pDefaultCreate->nCreateMapStartPositionY[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_CreateMapStartRadius_%02d", j + 1);
			pDefaultCreate->nCreateMapStartRadius[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		pDefaultCreate->nCreateTutorialMapIndex = pSox->GetFieldFromLablePtr(nIndex, "_CreateTutorialMapIndex")->GetInteger();
		pDefaultCreate->nCreateTutorialGateNo = pSox->GetFieldFromLablePtr(nIndex, "_CreateTutorialGateIndex")->GetInteger();

		// Default Parts
		pDefaultCreate->nDefaultBody = pSox->GetFieldFromLablePtr(nIndex, "_DefaultBody")->GetInteger();
		pDefaultCreate->nDefaultLeg = pSox->GetFieldFromLablePtr(nIndex, "_DefaultLeg")->GetInteger();
		pDefaultCreate->nDefaultHand = pSox->GetFieldFromLablePtr(nIndex, "_DefaultHand")->GetInteger();
		pDefaultCreate->nDefaultFoot = pSox->GetFieldFromLablePtr(nIndex, "_DefaultFoot")->GetInteger();

		for (int j = 0; j < DEFAULTPARTSMAX; j++){
			sprintf_s(szLabel, "_Face%d", j + 1);
			pDefaultCreate->nFace[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Hair%d", j + 1);
			pDefaultCreate->nHair[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Helmet%d", j + 1);
			pDefaultCreate->nHelmet[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Body%d", j + 1);
			pDefaultCreate->nBody[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Leg%d", j + 1);
			pDefaultCreate->nLeg[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Hand%d", j + 1);
			pDefaultCreate->nHand[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_Foot%d", j + 1);
			pDefaultCreate->nFoot[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		pDefaultCreate->nWeapon[0] = pSox->GetFieldFromLablePtr(nIndex, "_Weapon1")->GetInteger();
		pDefaultCreate->nWeapon[1] = pSox->GetFieldFromLablePtr(nIndex, "_Weapon2")->GetInteger();

		// Default Item
		for (int j = 0; j < DEFAULTITEMMAX; j++){
			sprintf_s(szLabel, "_DefaultItem%d", j + 1);
			pDefaultCreate->nDefaultItemID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_DefaultItem%dNumber", j + 1);
			pDefaultCreate->nDefaultItemCount[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		// Default Skill
		for (int j = 0; j < DEFAULTSKILLMAX; j++){
			sprintf_s(szLabel, "_DefaultSkill%d", j + 1);
			pDefaultCreate->nDefaultSkillID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

//----------------------------------------------------------------------
//[debug]  @_@
#if 0
       int nDEFAULTQUICKSLOTMAX = 60;  //멕경굶栗都  
	   printf("멕경굶栗都  DEFAULTQUICKSLOTMAX = 60  \n");
#else
       int nDEFAULTQUICKSLOTMAX = 50;  //일경굶栗都
	   printf("멕경굶栗都  DEFAULTQUICKSLOTMAX = 50  \n");
#endif
//----------------------------------------------------------------------

		// Default Quickslot
		for (int j = 0; j < nDEFAULTQUICKSLOTMAX; j++){     //for (int j = 0; j < DEFAULTQUICKSLOTMAX; j++){
			sprintf_s(szLabel, "_QuickSlot%dType", j + 1);
			pDefaultCreate->DefaultQuickSlot[j].cType = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_QuickSlot%dIndex", j + 1);
			pDefaultCreate->DefaultQuickSlot[j].nID = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		//Default Gesture
		for (int j = 0; j < DEFAULTGESTUREMAX; j++)
		{
			sprintf_s(szLabel, "_DefaultGesture%d", j + 1);
			pDefaultCreate->nDefaultGestureID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		//Default HairColor
		for( int j=0; j<DEFAULTHAIRCOLORMAX; j++ )
		{
			int nR = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dR", j + 1 ).c_str() )->GetInteger();
			int nG = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dG", j + 1 ).c_str() )->GetInteger();
			int nB = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dB", j + 1 ).c_str() )->GetInteger();
			DWORD dwColor = ((DWORD)((((255)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));

			pDefaultCreate->dwHairColor[j] = ConvertD3DCOLORToR10G10B10( dwColor, 1.1f );
		}

		//Default SkinColor
		for( int j=0; j<DEFAULTHAIRCOLORMAX; j++ )
		{
			int nA = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dA", j + 1 ).c_str() )->GetFloat() * 255);
			int nR = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dR", j + 1 ).c_str() )->GetFloat() * 255);
			int nG = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dG", j + 1 ).c_str() )->GetFloat() * 255);
			int nB = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dB", j + 1 ).c_str() )->GetFloat() * 255);

			pDefaultCreate->dwSkinColor[j] = ((DWORD)((((nA)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));
		}

		//Default EyeColor
		for( int j=0; j<DEFAULTHAIRCOLORMAX; j++ )
		{
			int nR = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dR", j + 1 ).c_str() )->GetInteger();
			int nG = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dG", j + 1 ).c_str() )->GetInteger();
			int nB = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dB", j + 1 ).c_str() )->GetInteger();

			DWORD dwColor = ((DWORD)((((255)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));

			pDefaultCreate->dwEyeColor[j] = ConvertD3DCOLORToR10G10B10( dwColor, 1.0f );
		}

		m_pMapDefaultCreate.insert(make_pair(pDefaultCreate->cClassID, pDefaultCreate));
	}

	SAFE_DELETE(pSox);

	return true;
}

bool CDNExtManager::LoadSkillData()
{
	DNTableFileFormat *pSox = LoadExtTable( "SkillTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"SkillTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	DNTableFileFormat *pJobTable = LoadExtTable( "JobTable" );
	if( !pJobTable ) 
	{
		g_Log.Log( LogType::_FILELOG, L"JobTable failed\r\n");
		SAFE_DELETE(pJobTable);
		SAFE_DELETE(pSox);
		return false;
	}
	
	// 직업 아이디로 부모 직업 아이디를 찾음.
	map<int, int> mapRootJob;
	for( int i = 0; i < pJobTable->GetItemCount(); ++i )
	{
		int iItemID = pJobTable->GetItemID( i );
		int iJobDeep = pJobTable->GetFieldFromLablePtr( iItemID, "_JobNumber" )->GetInteger();
		if( 0 < iJobDeep )
		{
			int iJobID = iItemID;
			int iRootJobDefine = pJobTable->GetFieldFromLablePtr( iItemID, "_BaseClass" )->GetInteger();

			// JobTable에 정의되어있는대로,
			//0	PLAYER_WARRIOR	 
			//1	PLAYER_ARCHER	 
			//-1	FREE	 
			//2	PLAYER_SOCERESS	 
			//3	PLAYER_CLERIC	 
			//9	PLAYER_ANCESTOR	 
			int iRootJobID = -1;
			switch( iRootJobDefine )
			{
				case 0:
					iRootJobID = 1;
					break;

				case 1:
					iRootJobID = 2;
					break;

				case 2:
					iRootJobID = 3;
					break;

				case 3:
					iRootJobID = 4;
					break;
			}

			if( -1 != iRootJobID )
			{
				mapRootJob.insert( make_pair(iJobID, iRootJobID) );
			}
		}
	}

	// Note: 디폴트 락 스킬을 찾아서 넣어주는 것이기 때문에 관련되어 필요한 정보만 읽어두면 된다.
	TSkillData* pSkillData = NULL;
	int nIndex = 0;
	for( int i = 0;i < pSox->GetItemCount(); ++i )
	{
		pSkillData = new TSkillData;
		//memset( pSkillData, 0, sizeof(TSkillData) );

		nIndex = pSox->GetItemID( i );
		pSkillData->nSkillID = nIndex;
		pSkillData->nNeedJobID = pSox->GetFieldFromLablePtr( nIndex, "_NeedJob" )->GetInteger();
		pSkillData->bDefaultLocked = (pSox->GetFieldFromLablePtr( nIndex, "_Lock" )->GetInteger() == 1) ? true : false;

		if( 0 < pSkillData->nSkillID && false == pSkillData->bDefaultLocked && pSkillData->nNeedJobID != 0 )
		{
			int iRootJobID = 0;
			if( pSkillData->nNeedJobID < 5 )
			{
				iRootJobID = pSkillData->nNeedJobID;
			}
			else
			{
				map<int, int>::iterator iter = mapRootJob.find( pSkillData->nNeedJobID );
				_ASSERT( mapRootJob.end() != iter );
				continue;
			}

			TDefaultCreateData* pDefaultCreateInfo = m_pMapDefaultCreate[ iRootJobID ];
			for( int i = 0; i < DEFAULTSKILLMAX; ++i )
			{
				int iDefaultSkillID = pDefaultCreateInfo->nDefaultSkillID[ i ];
				if( 0 == iDefaultSkillID )
					break;

				_ASSERT( iDefaultSkillID != pSkillData->nSkillID && "디폴트로 주는 스킬 중에 디폴트 언락 스킬로 설정되어있는 것이 있으면 안됨." );
			}

			_ASSERT( (int)m_mapDefaultUnlockSkillsByClass[ iRootJobID ].size() < DEFAULTUNLOCKSKILLMAX && "디폴트 언락 스킬 최대 갯수 초과." );
			m_mapDefaultUnlockSkillsByClass[ iRootJobID ].push_back( pSkillData );
		}
	}

	SAFE_DELETE( pSox );
	SAFE_DELETE( pJobTable );

	return true;
}


TDefaultCreateData* CDNExtManager::GetDefaultCreateData(BYTE cClassID)
{
	if (cClassID > CLASSKINDMAX) return NULL;

	TMapDefaultCreate::iterator iter = m_pMapDefaultCreate.find(cClassID);
	if (iter != m_pMapDefaultCreate.end()){
		return iter->second;
	}

	return NULL;
}

int CDNExtManager::GetCreateDefaultItem(BYTE cClassID, TCreateCharacterItem *CreateItemArray)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return -1;

	int nCount = 0;
	for (int i = 0; i < DEFAULTITEMMAX; i++){
		if (pDefaultCreate->nDefaultItemID[i] <= 0) continue;
		CreateItemArray[nCount].nItemID = pDefaultCreate->nDefaultItemID[i];
		CreateItemArray[nCount].cCount = pDefaultCreate->nDefaultItemCount[i];
		CreateItemArray[nCount].wDur = GetItemDurability(pDefaultCreate->nDefaultItemID[i]);

		nCount++;
	}

	return nCount;
}

bool CDNExtManager::IsDefaultParts(BYTE cClassID, char cEquipType, int nPartID)
{
	if (nPartID < 0) return false;

	TDefaultCreateData *pDefaultCreate = NULL;
	if (!(pDefaultCreate = GetDefaultCreateData(cClassID))) return false;

	switch (cEquipType)
	{
	case EQUIP_FACE:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nFace[i] == nPartID) return true;
		}
		break;
		
	case EQUIP_HAIR:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nHair[i] == nPartID) return true;
		}
		break;
		
	case EQUIP_HELMET:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nHelmet[i] == nPartID) return true;
		}
		break;

	case EQUIP_BODY:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nBody[i] == nPartID) return true;
		}
		break;

	case EQUIP_LEG:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nLeg[i] == nPartID) return true;
		}
		break;

	case EQUIP_HAND:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nHand[i] == nPartID) return true;
		}
		break;

	case EQUIP_FOOT:
		for (int i = 0; i < DEFAULTPARTSMAX; i++){
			if (pDefaultCreate->nFoot[i] == nPartID) return true;
		}
		break;

	case EQUIP_WEAPON1: if (pDefaultCreate->nWeapon[0] == nPartID) return true;
	case EQUIP_WEAPON2: if (pDefaultCreate->nWeapon[1] == nPartID) return true;

	default: 
		return false;
	}

	return false;
}

bool CDNExtManager::IsDefaultHairColor( BYTE cClassID, DWORD dwColor )
{
	TDefaultCreateData *pDefaultCreate = NULL;
	if (!(pDefaultCreate = GetDefaultCreateData(cClassID))) return false;

	for( int i=0; i<DEFAULTHAIRCOLORMAX; i++ ) {
		if( pDefaultCreate->dwHairColor[i] == dwColor ) return true;
	}

	return false;
}

bool CDNExtManager::IsDefaultSkinColor( BYTE cClassID, DWORD dwColor )
{
	TDefaultCreateData *pDefaultCreate = NULL;
	if (!(pDefaultCreate = GetDefaultCreateData(cClassID))) return false;

	for( int i=0; i<DEFAULTSKINCOLORMAX; i++ ) {
		if( pDefaultCreate->dwSkinColor[i] == dwColor ) return true;
	}

	return false;
}

bool CDNExtManager::IsDefaultEyeColor( BYTE cClassID, DWORD dwColor )
{
	TDefaultCreateData *pDefaultCreate = NULL;
	if (!(pDefaultCreate = GetDefaultCreateData(cClassID))) return false;

	for( int i=0; i<DEFAULTEYECOLORMAX; i++ ) {
		if( pDefaultCreate->dwEyeColor[i] == dwColor ) return true;
	}

	return false;
}

bool CDNExtManager::GetCreateDefaultPosition(BYTE cClassID, int &nPosX, int &nPosY)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return false;

	std::vector<int> nVecList;
	for( int i=0; i<DEFAULTPARTSMAX; i++ ) {
		if( pDefaultCreate->nCreateMapStartPositionX[i] == 0 && pDefaultCreate->nCreateMapStartPositionY[i] == 0 ) continue;
		nVecList.push_back(i);
	}
	if( nVecList.empty() ) {
		nPosX = 0;
		nPosY = 0;
		return true;
	}

	int nIndex = (int)(rand()%nVecList.size());
	int nRadius = pDefaultCreate->nCreateMapStartRadius[nVecList[nIndex]];
	if( nRadius == 0 ) nRadius = 1;
	nPosX = pDefaultCreate->nCreateMapStartPositionX[ nVecList[nIndex] ] - ( ( nRadius / 2 ) + (rand()%nRadius) );
	nPosY = pDefaultCreate->nCreateMapStartPositionY[ nVecList[nIndex] ] - ( ( nRadius / 2 ) + (rand()%nRadius) );

	nPosX *= 1000;
	nPosY *= 1000;

	return true;
}

bool CDNExtManager::GetCreateDefaultMapInfo(BYTE cClassID, int &nMapIndex, int &nTutorialMapIndex, char &cTutorialGateNo)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return false;

	nMapIndex = pDefaultCreate->nCreateMapIndex;
	nTutorialMapIndex = pDefaultCreate->nCreateTutorialMapIndex;
	cTutorialGateNo = pDefaultCreate->nCreateTutorialGateNo;

	return true;
}

bool CDNExtManager::GetCreateDefaultSkill(BYTE cClassID, int *CreateSkillArray)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return false;

	memcpy(CreateSkillArray, pDefaultCreate->nDefaultSkillID, sizeof(int) * DEFAULTSKILLMAX);
	return true;
}

bool CDNExtManager::GetCreateDefaultQuickSlot(BYTE cClassID, TQuickSlot *QuickSlotArray)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return false;

	memcpy(QuickSlotArray, pDefaultCreate->DefaultQuickSlot, sizeof(TQuickSlot) * DEFAULTQUICKSLOTMAX);
	return true;
}

bool CDNExtManager::GetCreateDefaultGesture(BYTE cClassID, int * CreateGestureArr)
{
	TDefaultCreateData *pDefaultCreate = GetDefaultCreateData(cClassID);
	if (!pDefaultCreate) return false;

	memcpy(CreateGestureArr, pDefaultCreate->nDefaultGestureID, sizeof(int) * DEFAULTGESTUREMAX);
	return true;
}

bool CDNExtManager::GetCreateDefaultUnlockSkill(BYTE cClassID, int *CreateUnlockSkillArray)
{
	if (m_mapDefaultUnlockSkillsByClass.empty()) return false;

	map<int, vector<TSkillData*> >::iterator iter = m_mapDefaultUnlockSkillsByClass.find(cClassID);
	if (iter == m_mapDefaultUnlockSkillsByClass.end()) return false;
	if (iter->second.empty()) return false;

	int nSize = (int)iter->second.size();
	if (nSize > DEFAULTUNLOCKSKILLMAX) nSize = DEFAULTUNLOCKSKILLMAX;
	for (int i = 0; i < nSize; i++){
		CreateUnlockSkillArray[i] = iter->second[i]->nSkillID;
	}

	return true;
}

// DWCCreateData
#if defined( PRE_ADD_DWC )
bool CDNExtManager::LoadDWCCreateData()
{
	DNTableFileFormat *pSox = LoadExtTable( "DWCCreateTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"DWCCreateTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TDWCCreateData *pDWCCreate = NULL;
	char szLabel[64] = { 0, };
	int nIndex = 0;

	for (int i = 0; i < pSox->GetItemCount(); i++){
		pDWCCreate = new TDWCCreateData;
		memset(pDWCCreate, 0, sizeof(TDWCCreateData));

		nIndex = pSox->GetItemID(i);
		pDWCCreate->cClassID = pSox->GetFieldFromLablePtr(nIndex, "_ClassID")->GetInteger();
		pDWCCreate->cJobCode1 = pSox->GetFieldFromLablePtr(nIndex, "_Jobcode1")->GetInteger();
		pDWCCreate->cJobCode2 = pSox->GetFieldFromLablePtr(nIndex, "_Jobcode2")->GetInteger();

		BYTE cJobCode = pDWCCreate->cClassID;
		if( pDWCCreate->cJobCode1 )
			cJobCode = pDWCCreate->cJobCode1;
		if( pDWCCreate->cJobCode2 )
			cJobCode = pDWCCreate->cJobCode2;

		pDWCCreate->cLevel = pSox->GetFieldFromLablePtr(nIndex, "_Level")->GetInteger();
		pDWCCreate->nExp = pSox->GetFieldFromLablePtr(nIndex, "_Experience")->GetInteger();

		// Default Position
		pDWCCreate->nCreateMapIndex = pSox->GetFieldFromLablePtr(nIndex, "_CreateMapIndex")->GetInteger();
		for (int j = 0; j < DEFAULTPOSITIONMAX; j++){
			sprintf_s(szLabel, "_CreateMapStartPosition_%02d_x", j + 1);
			pDWCCreate->nCreateMapStartPositionX[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_CreateMapStartPosition_%02d_y", j + 1);
			pDWCCreate->nCreateMapStartPositionY[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_CreateMapStartRadius_%02d", j + 1);
			pDWCCreate->nCreateMapStartRadius[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		// Default Parts
		pDWCCreate->nDefaultBody = pSox->GetFieldFromLablePtr(nIndex, "_DefaultBody")->GetInteger();
		pDWCCreate->nDefaultLeg = pSox->GetFieldFromLablePtr(nIndex, "_DefaultLeg")->GetInteger();
		pDWCCreate->nDefaultHand = pSox->GetFieldFromLablePtr(nIndex, "_DefaultHand")->GetInteger();
		pDWCCreate->nDefaultFoot = pSox->GetFieldFromLablePtr(nIndex, "_DefaultFoot")->GetInteger();

		pDWCCreate->nEquipArray[EQUIP_FACE]		= pSox->GetFieldFromLablePtr(nIndex, "_Face1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_HAIR]		= pSox->GetFieldFromLablePtr(nIndex, "_Hair1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_HELMET]	= pSox->GetFieldFromLablePtr(nIndex, "_Helmet1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_BODY]		= pSox->GetFieldFromLablePtr(nIndex, "_Body1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_LEG]		= pSox->GetFieldFromLablePtr(nIndex, "_Leg1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_HAND]		= pSox->GetFieldFromLablePtr(nIndex, "_Hand1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_FOOT]		= pSox->GetFieldFromLablePtr(nIndex, "_Foot1")->GetInteger();
		//pDWCCreate->nEquipArray[EQUIP_NECKLACE] = pSox->GetFieldFromLablePtr(nIndex, "_Necklace1")->GetInteger();
		//pDWCCreate->nEquipArray[EQUIP_EARRING]	= pSox->GetFieldFromLablePtr(nIndex, "_Earring1")->GetInteger();
		//pDWCCreate->nEquipArray[EQUIP_RING1]	= pSox->GetFieldFromLablePtr(nIndex, "_Ring1")->GetInteger();
		//pDWCCreate->nEquipArray[EQUIP_RING2]	= pSox->GetFieldFromLablePtr(nIndex, "_Ring2")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_WEAPON1]	= pSox->GetFieldFromLablePtr(nIndex, "_Weapon1")->GetInteger();
		pDWCCreate->nEquipArray[EQUIP_WEAPON2]	= pSox->GetFieldFromLablePtr(nIndex, "_Weapon2")->GetInteger();

		pDWCCreate->nSkillPoint = static_cast<short>(pSox->GetFieldFromLablePtr(nIndex, "_SkillPoint")->GetInteger());
		pDWCCreate->nGold = pSox->GetFieldFromLablePtr(nIndex, "_Gold")->GetInteger();

		// Default Item
		for (int j = 0; j < DEFAULTITEMMAX; j++){
			sprintf_s(szLabel, "_DefaultItem%d", j + 1);
			pDWCCreate->nDefaultItemID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_DefaultItem%dNumber", j + 1);
			pDWCCreate->nDefaultItemCount[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		// Default Skill
		for (int j = 0; j < DEFAULTSKILLMAX; j++){
			sprintf_s(szLabel, "_DefaultSkill%d", j + 1);
			pDWCCreate->nDefaultSkillID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		//----------------------------------------------------------------------
		//[debug]  @_@
#if 0
		int nDEFAULTQUICKSLOTMAX = 60;  //멕경굶栗都  
		printf("멕경굶栗都  DEFAULTQUICKSLOTMAX = 60  \n");
#else
		int nDEFAULTQUICKSLOTMAX = 50;  //일경굶栗都
		printf("멕경굶栗都  DEFAULTQUICKSLOTMAX = 50  \n");
#endif
		//----------------------------------------------------------------------

		// Default Quickslot
		for (int j = 0; j < nDEFAULTQUICKSLOTMAX; j++){
			sprintf_s(szLabel, "_QuickSlot%dType", j + 1);
			pDWCCreate->DefaultQuickSlot[j].cType = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_QuickSlot%dIndex", j + 1);
			pDWCCreate->DefaultQuickSlot[j].nID = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		//Default Gesture
		for (int j = 0; j < DEFAULTGESTUREMAX; j++)
		{
			sprintf_s(szLabel, "_DefaultGesture%d", j + 1);
			pDWCCreate->nDefaultGestureID[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}

		//Default HairColor
		for( int j=0; j<1; j++ )
		{
			int nR = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dR", j + 1 ).c_str() )->GetInteger();
			int nG = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dG", j + 1 ).c_str() )->GetInteger();
			int nB = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_HairColor%dB", j + 1 ).c_str() )->GetInteger();
			DWORD dwColor = ((DWORD)((((255)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));

			pDWCCreate->dwHairColor = ConvertD3DCOLORToR10G10B10( dwColor, 1.1f );
		}

		//Default SkinColor
		for( int j=0; j<1; j++ )
		{
			int nA = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dA", j + 1 ).c_str() )->GetFloat() * 255);
			int nR = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dR", j + 1 ).c_str() )->GetFloat() * 255);
			int nG = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dG", j + 1 ).c_str() )->GetFloat() * 255);
			int nB = (int)(pSox->GetFieldFromLablePtr( nIndex, FormatA( "_SkinColor%dB", j + 1 ).c_str() )->GetFloat() * 255);

			pDWCCreate->dwSkinColor = ((DWORD)((((nA)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));
		}

		//Default EyeColor
		for( int j=0; j<1; j++ )
		{
			int nR = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dR", j + 1 ).c_str() )->GetInteger();
			int nG = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dG", j + 1 ).c_str() )->GetInteger();
			int nB = pSox->GetFieldFromLablePtr( nIndex, FormatA( "_EyeColor%dB", j + 1 ).c_str() )->GetInteger();

			DWORD dwColor = ((DWORD)((((255)&0xff)<<24)|(((nR)&0xff)<<16)|(((nG)&0xff)<<8)|((nB)&0xff)));

			pDWCCreate->dwEyeColor = ConvertD3DCOLORToR10G10B10( dwColor, 1.0f );
		}

		m_pMapDWCCreate.insert(make_pair(cJobCode, pDWCCreate));
	}

	SAFE_DELETE(pSox);

	return true;
}

TDWCCreateData* CDNExtManager::GetDWCCreateData(BYTE cJobCode)
{
	TMapDWCCreate::iterator iter = m_pMapDWCCreate.find(cJobCode);
	if( m_pMapDWCCreate.end() != iter)
		return iter->second;

	return NULL;
}

int CDNExtManager::GetCreateDWCItem(BYTE cJobCode, TCreateCharacterItem *CreateItemArray)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return -1;

	int nCount = 0;
	for (int i = 0; i < DEFAULTITEMMAX; i++){
		if (pDWCCreate->nDefaultItemID[i] <= 0) continue;
		CreateItemArray[nCount].nItemID = pDWCCreate->nDefaultItemID[i];
		CreateItemArray[nCount].cCount = pDWCCreate->nDefaultItemCount[i];
		CreateItemArray[nCount].wDur = GetItemDurability(pDWCCreate->nDefaultItemID[i]);

		nCount++;
	}

	return nCount;
}

bool CDNExtManager::GetCreateDWCPosition(BYTE cJobCode, int &nPosX, int &nPosY)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return false;

	std::vector<int> nVecList;
	for( int i=0; i<DEFAULTPARTSMAX; i++ ) {
		if( pDWCCreate->nCreateMapStartPositionX[i] == 0 && pDWCCreate->nCreateMapStartPositionY[i] == 0 ) continue;
		nVecList.push_back(i);
	}
	if( nVecList.empty() ) {
		nPosX = 0;
		nPosY = 0;
		return true;
	}

	int nIndex = (int)(rand()%nVecList.size());
	int nRadius = pDWCCreate->nCreateMapStartRadius[nVecList[nIndex]];
	if( nRadius == 0 ) nRadius = 1;
	nPosX = pDWCCreate->nCreateMapStartPositionX[ nVecList[nIndex] ] - ( ( nRadius / 2 ) + (rand()%nRadius) );
	nPosY = pDWCCreate->nCreateMapStartPositionY[ nVecList[nIndex] ] - ( ( nRadius / 2 ) + (rand()%nRadius) );

	nPosX *= 1000;
	nPosY *= 1000;

	return true;
}

bool CDNExtManager::GetCreateDWCEquip(BYTE cJobCode, int * CreateEquipArray)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return false;

	memcpy(CreateEquipArray, pDWCCreate->nEquipArray, sizeof(int) * EQUIPMAX);
	return true;
}

bool CDNExtManager::GetCreateDWCSkill(BYTE cJobCode, int *CreateSkillArray)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return false;

	memcpy(CreateSkillArray, pDWCCreate->nDefaultSkillID, sizeof(int) * DEFAULTSKILLMAX);
	return true;
}

bool CDNExtManager::GetCreateDWCQuickSlot(BYTE cJobCode, TQuickSlot *QuickSlotArray)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return false;

	memcpy(QuickSlotArray, pDWCCreate->DefaultQuickSlot, sizeof(TQuickSlot) * DEFAULTQUICKSLOTMAX);
	return true;
}

bool CDNExtManager::GetCreateDWCGesture(BYTE cJobCode, int * CreateGestureArr)
{
	TDWCCreateData *pDWCCreate = GetDWCCreateData(cJobCode);
	if (!pDWCCreate) return false;

	memcpy(CreateGestureArr, pDWCCreate->nDefaultGestureID, sizeof(int) * DEFAULTGESTUREMAX);
	return true;
}
#endif // #if defined( PRE_ADD_DWC )

// CoinCountByLevel
bool CDNExtManager::LoadCoinCount()
{
	DNTableFileFormat *pSox = LoadExtTable( "CoinTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"CoinTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TCoinCountData *pCoinCount = NULL;
	char szLabel[64] = { 0, };
	int nIndex = 0;

	for (int i = 0; i < pSox->GetItemCount(); i++){
		pCoinCount = new TCoinCountData;
		memset(pCoinCount, 0, sizeof(TCoinCountData));

		nIndex = pSox->GetItemID(i);
		pCoinCount->cLevel = nIndex;

		// CoinCount
		for (int j = 0; j < 50; j++){
			sprintf_s(szLabel, "_World%dCoin", j + 1);
			pCoinCount->nRebirthCoin[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
			sprintf_s(szLabel, "_World%dCashCoin", j + 1);
			pCoinCount->nCashRebirthCoin[j] = pSox->GetFieldFromLablePtr(nIndex, szLabel)->GetInteger();
		}
		pCoinCount->nRebirthCoinLimit = pSox->GetFieldFromLablePtr(nIndex, "_CoinLimit")->GetInteger();
		pCoinCount->nCashRebirthCoinLimit = pSox->GetFieldFromLablePtr(nIndex, "_CashCoinLimit")->GetInteger();

		m_pMapCoinCount.insert(make_pair(pCoinCount->cLevel, pCoinCount));
	}

	SAFE_DELETE(pSox);

	return true;
}

int CDNExtManager::GetRebirthCoin(BYTE cLevel, char cWorldID)
{
	if ((cLevel <= 0) || (cLevel > CHARLEVELMAX)) return -1;
	if ((cWorldID <= 0) || (cWorldID > WORLDCOUNTMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nRebirthCoin[cWorldID - 1];
	}

	return -1;
}

int CDNExtManager::GetCashRebirthCoin(BYTE cLevel, char cWorldID)
{
	if ((cLevel <= 0) || (cLevel > CHARLEVELMAX)) return -1;
	if ((cWorldID <= 0) || (cWorldID > WORLDCOUNTMAX)) return -1;

	TMapCoinCount::iterator iter = m_pMapCoinCount.find(cLevel);
	if (iter != m_pMapCoinCount.end()){
		return iter->second->nCashRebirthCoin[cWorldID - 1];
	}

	return -1;
}


//---------------------------------------------------------------------------------
// LevelData (PlayerLevelTable - TLevelData)
//---------------------------------------------------------------------------------
bool CDNExtManager::LoadLevelData()
{
	DNTableFileFormat *pSox = LoadExtTable( "PlayerLevelTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PlayerLevelTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TLevelData *pLevelData = NULL;
	char szTemp[256];
	memset(&szTemp, 0, sizeof(szTemp));

	int nIndex = 0;
	for (int i = 0; i < pSox->GetItemCount(); i++){
		pLevelData = new TLevelData;
		memset(pLevelData, 0, sizeof(TLevelData));

		pLevelData->nIndex = pSox->GetItemID(i);
		pLevelData->wStrength = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Strength")->GetInteger();
		pLevelData->wAgility = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Agility")->GetInteger();
		pLevelData->wIntelligence = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Intelligence")->GetInteger();
		pLevelData->wStamina = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Stamina")->GetInteger();
		pLevelData->nExperience = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Experience")->GetInteger();
		pLevelData->nFatigue = pSox->GetFieldFromLablePtr(pLevelData->nIndex, "_Fatigue")->GetInteger();

		m_pLevelData[pLevelData->nIndex] = pLevelData;
	}

	SAFE_DELETE(pSox);
	return true;
}

int CDNExtManager::GetFatigue(char cClass, char cLevel)
{
	if ((cClass <= 0) || (cClass > CLASSKINDMAX)) return 0;
	if ((cLevel <= 0) || (cLevel > CHARLEVELMAX)) return 0;

	int nValue = ((cClass - 1) * CHARLEVELMAX) + cLevel;
	if (nValue > (int)m_pLevelData.size()) return 0;

	return m_pLevelData[nValue]->nFatigue;
}

// 금칙어
#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNExtManager::LoadProhibitWord(int nLanguage, const char * pszFilePath)
{
	if (pszFilePath == NULL)
		return false;

	CXMLParser parser;
	if (parser.Open(pszFilePath) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"%S failed\r\n", pszFilePath);
		return false;
	}

	TProhibitWord * pProhibit = GetLanguageSlangFilter(nLanguage);
	if (pProhibit == NULL)
	{
		TProhibitWord mProhibit;
		m_ProhibitWordList.insert(std::make_pair(nLanguage, mProhibit));

		pProhibit = GetLanguageSlangFilter(nLanguage);
		if (pProhibit == NULL)
			return false;
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNExtManager::LoadProhibitWord()
{
	char buf[_MAX_PATH] = { 0, };
	sprintf_s( buf, "%s/Resource/UIString/ProhibitWord.xml", g_Config.szResourcePath.c_str() );

	CXMLParser parser;

	if (parser.Open(buf) == false)
	{
		g_Log.Log(LogType::_FILELOG, L"ProhibitWord.xml failed\r\n");
		return false;
	}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)	

	if (parser.FirstChildElement("ProhibitWord", true) == true)
	{
		if (parser.FirstChildElement("Account", true) == true)
		{
			if (parser.FirstChildElement("AccountWord", true) == true)
			{
				do {
					if (parser.GetText())
					{
						wstring wszStr = parser.GetText();
						std::transform( wszStr.begin(), wszStr.end(), wszStr.begin(), towlower );
						unsigned int nHashKey = MemoryToRSHashKey(wszStr.c_str(), (long)wszStr.length());
#if defined(PRE_ADD_MULTILANGUAGE)
						pProhibit->insert(make_pair(nHashKey, wszStr));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
						m_ProhibitWordList.insert(make_pair(nHashKey, wszStr));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					}
				} while (parser.NextSiblingElement("AccountWord"));

				parser.GoParent();
			} 

			parser.GoParent();
		}

#if defined(_KR) || defined(_KRAZ)
		if (parser.FirstChildElement("Chat", true) == true)
		{
			if (parser.FirstChildElement("ChatWord", true) == true)
			{
				do {
					if (parser.GetText())
					{
						wstring wszStr = parser.GetText();
						std::transform( wszStr.begin(), wszStr.end(), wszStr.begin(), towlower );
						unsigned int nHashKey = MemoryToRSHashKey(wszStr.c_str(), (long)wszStr.length());
#if defined(PRE_ADD_MULTILANGUAGE)
						pProhibit->insert(make_pair(nHashKey, wszStr));
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
						m_ProhibitWordList.insert(make_pair(nHashKey, wszStr));
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
					}
				} while (parser.NextSiblingElement("ChatWord"));

				parser.GoParent();
			} 

			parser.GoParent();
		}
#endif
		return true;
	}
	return false;
}

#if defined(PRE_ADD_MULTILANGUAGE)
CDNExtManager::TProhibitWord * CDNExtManager::GetLanguageSlangFilter(int nLanguage)
{
	TProhibitWord * pList = NULL;
	std::map <int, TProhibitWord>::iterator ii = m_ProhibitWordList.find(nLanguage);
	if (ii != m_ProhibitWordList.end())
		return &(*ii).second;
	return NULL;
}

bool CDNExtManager::CheckProhibitWord(int nLanguage, const wchar_t *pwszWord)
{
	TProhibitWord * pProhibit = GetLanguageSlangFilter(nLanguage);
	if (pProhibit)
	{
		std::wstring::size_type index;

		std::wstring strTemp(pwszWord);
		std::transform ( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

		for( std::map<unsigned int, std::wstring>::iterator itor = pProhibit->begin(); itor != pProhibit->end(); itor++ )
		{
			index = strTemp.find( itor->second );
			if( index != string::npos )		
				return true;
		}
	}
	return false;
}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
bool CDNExtManager::CheckProhibitWord(const wchar_t *pwszWord)
{
	std::wstring::size_type index;

	std::wstring strTemp(pwszWord);
	std::transform ( strTemp.begin(), strTemp.end(), strTemp.begin(), towlower );

	for( std::map<unsigned int, std::wstring>::iterator itor = m_ProhibitWordList.begin(); itor != m_ProhibitWordList.end(); itor++ )
	{
		index = strTemp.find( itor->second );
		if( index != string::npos )
			return true;
	}
	return false;
}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

bool CDNExtManager::CheckSpecialCharacter(const wchar_t *pwszWord)
{
	wstring strSrc = pwszWord;

	std::wstring::size_type index;
	index = strSrc.find_first_of( L"~!@#$%^&*+|:?><,.;[]{}()\n\t\v\b\r\a\\\?\'\" " );

	if( index != string::npos )
	{
		return true;
	}

	return false;
}

unsigned int CDNExtManager::MemoryToRSHashKey(const wchar_t *pszData, long nSize)
{
	unsigned int nB			= 378551;
	unsigned int nA			= 63689;
	unsigned int nHashKey	= 0;

	for (int i = 0; i < nSize; i++)
	{
		nHashKey = nHashKey * nA + pszData[i];
		nA = nA * nB;
	}

	return (nHashKey & 0x7FFFFFFF);
}

//---------------------------------------------------------------------------------
// Weapon (WeaponTable - TWeaponData)
//---------------------------------------------------------------------------------
bool CDNExtManager::LoadWeaponData()
{
	DNTableFileFormat *pSox = LoadExtTable( "WeaponTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"WeaponTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TWeaponData *pWeaponData = NULL;
	char szTemp[256];
	memset(&szTemp, 0, sizeof(szTemp));

	for (int i = 0; i < pSox->GetItemCount(); i++){
		pWeaponData = new TWeaponData;
		memset(pWeaponData, 0, sizeof(TWeaponData));

		pWeaponData->nWeaponIndex = pSox->GetItemID(i);
		pWeaponData->cEquipType = pSox->GetFieldFromLablePtr(pWeaponData->nWeaponIndex, "_EquipType")->GetInteger();
		pWeaponData->nLength = pSox->GetFieldFromLablePtr(pWeaponData->nWeaponIndex, "_Length")->GetInteger();
		pWeaponData->nDurability = pSox->GetFieldFromLablePtr(pWeaponData->nWeaponIndex, "_Durability")->GetInteger();
		pWeaponData->nDurabilityRepairCoin = pSox->GetFieldFromLablePtr(pWeaponData->nWeaponIndex, "_DurabilityRepairCoin")->GetInteger();
		
		std::pair<TMapWeaponData::iterator,bool> Ret = m_pWeaponData.insert(make_pair(pWeaponData->nWeaponIndex, pWeaponData));
		if( Ret.second == false )
			delete pWeaponData;
	}

	SAFE_DELETE(pSox);
	return true;
}

TWeaponData* CDNExtManager::GetWeaponData(int nWeaponIndex)
{
	if (m_pWeaponData.empty()) return NULL;

	TMapWeaponData::iterator iter = m_pWeaponData.find(nWeaponIndex);
	if (iter != m_pWeaponData.end()){
		return iter->second;
	}

	return NULL;
}

//---------------------------------------------------------------------------------
// Part (PartsTable - TPartData)
//---------------------------------------------------------------------------------
bool CDNExtManager::LoadPartData()
{
	DNTableFileFormat *pSox = LoadExtTable( "PartsTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PartsTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	TPartData *pPartData = NULL;
	char szTemp[256];

	memset(&szTemp, 0, sizeof(szTemp));

	for (int i = 0; i < pSox->GetItemCount(); i++){
		pPartData = new TPartData;
		memset(pPartData, 0, sizeof(TPartData));

		pPartData->nPartIndex = pSox->GetItemID(i);
		pPartData->nParts = pSox->GetFieldFromLablePtr(pPartData->nPartIndex, "_Parts")->GetInteger();
		pPartData->nDurability = pSox->GetFieldFromLablePtr(pPartData->nPartIndex, "_Durability")->GetInteger();
		pPartData->nDurabilityRepairCoin = pSox->GetFieldFromLablePtr(pPartData->nPartIndex, "_DurabilityRepairCoin")->GetInteger();

		std::pair<TMapPartData::iterator,bool> Ret = m_pPartData.insert(make_pair(pPartData->nPartIndex, pPartData));
		if( Ret.second == false )
			delete pPartData;
	}
	SAFE_DELETE(pSox);
	return true;
}

TPartData* CDNExtManager::GetPartData(int nPartIndex)
{
	if (m_pPartData.empty()) return NULL;

	TMapPartData::iterator iter = m_pPartData.find(nPartIndex);
	if (iter != m_pPartData.end()){
		return iter->second;
	}

	return NULL;
}


int CDNExtManager::GetItemDurability( int nItemID )
{
	// 내구도 구해서 셋팅한다.
	int nDurability = 0;

	TWeaponData *pWeapon = GetWeaponData( nItemID );
	if( pWeapon ) nDurability = pWeapon->nDurability;

	TPartData *pParts = GetPartData( nItemID );
	if( pParts ) nDurability = pParts->nDurability;

	return nDurability;
}

#elif defined(_MASTERSERVER)

//---------------------------------------------------------------------------------
// PvPGameStartConditionTable (PvPGameStartConditiontable - TPvPGameStartConditionTable)
//---------------------------------------------------------------------------------

bool CDNExtManager::LoadPvPGameStartConditionTable()
{	
	DNTableFileFormat *pSox = LoadExtTable( "PvPGameStartConditionTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PvPGameStartConditionTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iMaxPlayerNum = pSox->GetFieldFromLablePtr( nItemID, "PlayerNumber" )->GetInteger();
		if( iMaxPlayerNum <= 0 )
			continue;

		_ASSERT( iMaxPlayerNum <= PvPCommon::Common::MaxPlayer );

		TPvPGameStartConditionTable* pPvPGameStartConditionTable = new TPvPGameStartConditionTable;

		pPvPGameStartConditionTable->uiMinTeamPlayerNum		= pSox->GetFieldFromLablePtr( nItemID, "Min_TeamPlayerNum_Needed" )->GetInteger();
		pPvPGameStartConditionTable->uiMaxTeamPlayerDiff	= pSox->GetFieldFromLablePtr( nItemID, "Max_TeamPlayerNum_Difference" )->GetInteger();

		std::pair<TPvPGameStartConditionData::iterator,bool> Ret = m_PvPGameStartConditionTable.insert( make_pair( iMaxPlayerNum, pPvPGameStartConditionTable ) );
		if( Ret.second == false )
			delete pPvPGameStartConditionTable;
	}

	SAFE_DELETE(pSox);
	return true;
}

bool CDNExtManager::LoadPvPGameModeTable()
{
	DNTableFileFormat *pSox = LoadExtTable( "pvpgamemodetable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"pvpgamemodetable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		char szBuf[MAX_PATH];

		int nItemID = pSox->GetItemID(i);

		TPvPGameModeTable Data;
		memset( &Data, 0, sizeof(Data) );

		Data.nItemID	= nItemID;
		Data.uiGameMode = pSox->GetFieldFromLablePtr( nItemID, "GamemodeID" )->GetInteger();

		bool bSkip = true;
		switch( Data.uiGameMode )
		{
			case PvPCommon::GameMode::PvP_Respawn:
			case PvPCommon::GameMode::PvP_Round:
			case PvPCommon::GameMode::PvP_Captain:
			case PvPCommon::GameMode::PvP_IndividualRespawn:
			case PvPCommon::GameMode::PvP_Zombie_Survival:
			case PvPCommon::GameMode::PvP_GuildWar:
#if defined(PRE_ADD_DWC)
			case PvPCommon::GameMode::PvP_AllKill:
#endif
			{
				bSkip = false;
				break;
			}
			default:
			{
				break;
			}

		}

		if( bSkip == true )
			continue;

		// 승리조건
		for( int j=1 ; j<=5 ; ++j )
		{
			sprintf_s( szBuf, "WinCondition_%d", j );
			int iWinCondition = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			if( iWinCondition == 0 )
				break;
			_ASSERT( iWinCondition > 0 );

			Data.vWinCondition.push_back( iWinCondition );
		}
		// 플레이시간
		for( int j=1 ; j<= 5 ; ++j )
		{
			sprintf_s( szBuf, "PlayTime_%d", j );
			int iSec = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			if( iSec == 0 )
				break;
			_ASSERT( iSec > 0 );

			Data.vPlayTimeSec.push_back( iSec );
		}

		// LadderMatchType
		Data.LadderMatchType = static_cast<LadderSystem::MatchType::eCode>(pSox->GetFieldFromLablePtr( nItemID, "LadderType" )->GetInteger());
		// 부활대기시간
		Data.uiRespawnTimeSec = pSox->GetFieldFromLablePtr( nItemID, "Respawn_Time" )->GetInteger();
		// 부활무적시간 
		Data.uiRespawnNoDamageTimeSec = pSox->GetFieldFromLablePtr( nItemID, "Respawn_Nodamage_Time" )->GetInteger();
		// 부활 시 회복되는 HP%
		Data.uiRespawnHPPercent = pSox->GetFieldFromLablePtr( nItemID, "Respawn_HP_Percent" )->GetInteger();
		// 부활 시 회복되는 MP%
		Data.uiRespawnMPPercent = pSox->GetFieldFromLablePtr( nItemID, "Respawn_MP_Percent" )->GetInteger();
		// 승리 보너스 점수
		Data.uiWinXPPerRound = pSox->GetFieldFromLablePtr( nItemID, "WinXP_PerRound" )->GetInteger();
		// 패배 보너스 점수
		Data.uiLoseXPPerRound = pSox->GetFieldFromLablePtr( nItemID, "LoseXP_PerRound" )->GetInteger();
		// 적KO시드롭아이템
		Data.uiItemDropTableID = pSox->GetFieldFromLablePtr( nItemID, "KOItemDropTableID" )->GetInteger();
		// 레벨보정시스템ON/OFF
		Data.bIsLevelRegulation = ( pSox->GetFieldFromLablePtr( nItemID, "StartRegulation" )->GetInteger() == 1 ) ? true : false;
		// 릴리즈버전에서노출
		Data.bIsReleaseShow = ( pSox->GetFieldFromLablePtr( nItemID, "ReleaseShow" )->GetInteger() == 1 ) ? true : false;
		// 메달지급점수
		Data.uiMedalExp = pSox->GetFieldFromLablePtr( nItemID, "MedalExp" )->GetInteger();
		_ASSERT( Data.uiMedalExp );
		// 인원 수 최소 제한
		Data.uiNumOfPlayersMin = pSox->GetFieldFromLablePtr( nItemID, "NumOfPlayers_Min" )->GetInteger();
		// 인원 수 최대 제한
		Data.uiNumOfPlayersMax = pSox->GetFieldFromLablePtr( nItemID, "NumOfPlayers_Max" )->GetInteger();
		// 승리시 얻는 PvPExp 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			sprintf_s( szBuf, "VictoryExp_%d", j );
			int iExp = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			Data.vVictoryExp.push_back( iExp );
		}

		// 승리시 얻는 PvPExp 보너스 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			sprintf_s( szBuf, "VictoryBonusRate_%d", j );
			int iExp = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			Data.vVictoryBonusRate.push_back( iExp );
		}

		// 패배시 얻는 PvPExp 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			sprintf_s( szBuf, "DefeatExp_%d", j );
			int iExp = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			Data.vDefeatExp.push_back( iExp );
		}

		// 패배시 얻는 PvPExp 보너스 점수
		for( int j=1 ; j<= 5 ; ++j )
		{
			sprintf_s( szBuf, "DefeatBonusRate_%d", j );
			int iExp = pSox->GetFieldFromLablePtr( nItemID, szBuf )->GetInteger();
			Data.vDefeatBonusRate.push_back( iExp );
		}

		std::pair<TPvPGameModeTableData::iterator,bool> Ret = m_PvPGameModeTable.insert( std::make_pair(nItemID,Data) );
		_ASSERT( Ret.second );

		if( Data.LadderMatchType >= LadderSystem::MatchType::_1vs1 )
		{
			std::map<LadderSystem::MatchType::eCode,std::vector<int>>::iterator itor = m_MatchTypePvPGameModeID.find( Data.LadderMatchType );
			if( itor == m_MatchTypePvPGameModeID.end() )
			{
				std::vector<int> vData;
				vData.push_back( nItemID );
				m_MatchTypePvPGameModeID.insert( std::make_pair(Data.LadderMatchType,vData) );
			}
			else
			{
				(*itor).second.push_back( nItemID );
			}
		}
	}

	SAFE_DELETE(pSox);
	return true;
}

//---------------------------------------------------------------------------------
// PvPMapTable (PvPMaptable - TPvPMapTable)
//---------------------------------------------------------------------------------
bool CDNExtManager::LoadPvPMapTable()
{
	DNTableFileFormat *pSox = LoadExtTable( "PvPMapTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMapTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	if( pSox->GetItemCount() <= 0 )
	{
		g_Log.Log( LogType::_FILELOG, L"PvPMapTable Count(%d)\r\n", pSox->GetItemCount() );
		SAFE_DELETE(pSox);
		return false;
	}

	char szTemp[MAX_PATH];

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int					MapTableID;
		int					GameModeType;
		std::vector<int>	GameModeTableID;
		std::vector<int>	NumOfPlayersOption;
		int					Allow_Breakin_PlayingGame;
		int					ItemUsageType;
		int					AllowItemDrop;
		int					IsGuildBattleGround;
		int					ReleaseShow;
		int					LadderType;
	};

	TempFieldNum sFieldNum;
	sFieldNum.MapTableID				= pSox->GetFieldNum( "MapTableID" );
	sFieldNum.GameModeType				= pSox->GetFieldNum( "GameModeType" );
	sFieldNum.Allow_Breakin_PlayingGame	= pSox->GetFieldNum( "Allow_Breakin_PlayingGame" );
	sFieldNum.ItemUsageType				= pSox->GetFieldNum( "ItemUsageType" );
	sFieldNum.AllowItemDrop				= pSox->GetFieldNum( "AllowItemDrop" );
	sFieldNum.IsGuildBattleGround		= pSox->GetFieldNum( "IsGuildBattleGround" );
	sFieldNum.ReleaseShow				= pSox->GetFieldNum( "ReleaseShow" );
	sFieldNum.LadderType				= pSox->GetFieldNum( "_LadderType" );

	sFieldNum.GameModeTableID.reserve(10);
	for( int j=1 ; j<=10 ; ++j )
	{
		sprintf_s( szTemp, "GameModeTableID_%d", j );
		sFieldNum.GameModeTableID.push_back( pSox->GetFieldNum(szTemp) );
	}

	sFieldNum.NumOfPlayersOption.reserve(5);
	for( int j=1 ; j<= 5 ; ++j )
	{
		sprintf_s( szTemp, "NumOfPlayersOption%d", j );
		sFieldNum.NumOfPlayersOption.push_back( pSox->GetFieldNum(szTemp) );
	}

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		int iIdx = pSox->GetIDXprimary(nItemID);

		int nMapTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.MapTableID )->GetInteger();
		if( nMapTableID == 0 )
			continue;

		//		_ASSERT( nMapTableID == nItemID );

		TPvPMapTable* pPvPMapTable = new TPvPMapTable;
		pPvPMapTable->vGameModeTableID.reserve( 10 );
		pPvPMapTable->vNumOfPlayerOption.reserve( 5 );

		// GameType
		pPvPMapTable->uiGameType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GameModeType )->GetInteger();
		_ASSERT( pPvPMapTable->uiGameType < PvPCommon::GameType::Max );

		// GameMode
		for( int j=1 ; j<=10 ; ++j )
		{
			int iTableID = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.GameModeTableID[j-1] )->GetInteger();
			if( iTableID == 0 )
				break;

			pPvPMapTable->vGameModeTableID.push_back( iTableID );
		}
		_ASSERT( pPvPMapTable->vGameModeTableID.size() );

		// 인원수
		for( int j=1 ; j<= 5 ; ++j )
		{
			int nCount = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.NumOfPlayersOption[j-1] )->GetInteger();
			if( nCount <= 0 )
				break;

			_ASSERT( nCount <= PvPCommon::Common::MaxPlayer );
			pPvPMapTable->vNumOfPlayerOption.push_back( nCount );
		}
		_ASSERT( pPvPMapTable->vNumOfPlayerOption.size() );

		// 난입가능Flag
		pPvPMapTable->bIsBreakInto = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.Allow_Breakin_PlayingGame )->GetInteger() == 0 ) ? false : true;

		// PvPCommon::ItemUsageType
		int nItemUsageType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ItemUsageType )->GetInteger();
		_ASSERT( nItemUsageType >= 0 && nItemUsageType < PvPCommon::ItemUsageType::Max );

		// 아이템드롭 Flag
		pPvPMapTable->bIsAllowItemDrop = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.AllowItemDrop )->GetInteger() == 0 ) ? false : true;

		// 길드전에서 사용할수 있는지 Flag
		pPvPMapTable->bIsGuildBattleGround = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.IsGuildBattleGround )->GetInteger() == 0 ) ? false : true;

		// 릴리즈버전에서노출
		pPvPMapTable->bIsReleaseShow = ( pSox->GetFieldFromLablePtr( iIdx, sFieldNum.ReleaseShow )->GetInteger() == 0 ) ? false : true;

		pPvPMapTable->MatchType = static_cast<LadderSystem::MatchType::eCode>(pSox->GetFieldFromLablePtr( iIdx, sFieldNum.LadderType )->GetInteger() );

		std::pair<TPvPMapTableData::iterator,bool> Ret = m_PvPMapTable.insert( make_pair( nMapTableID, pPvPMapTable ) );
		if( Ret.second == false )
			delete pPvPMapTable;
	}

	SAFE_DELETE(pSox);
	return true;
}

const TPvPMapTable* CDNExtManager::GetPvPMapTable( const int nItemID )
{
	TPvPMapTableData::iterator itor = m_PvPMapTable.find( nItemID );
	if( itor != m_PvPMapTable.end() )
		return itor->second;

	return NULL;
}

bool CDNExtManager::LoadGuildWarMapInfoTable()
{
	DNTableFileFormat *pSox = LoadExtTable( "pvpguildwarmapmaketable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"pvpguildwarmapmaketable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		TGuildWarMapInfo GuildWarMapInfo;

		GuildWarMapInfo.uiMapTableID = pSox->GetFieldFromLablePtr( nItemID, "_PvPMapTableID" )->GetInteger();
		GuildWarMapInfo.cGuildWarUseMonth = pSox->GetFieldFromLablePtr( nItemID, "_GuildWarUseMonth" )->GetInteger();		

		m_GuildWarMapInfo.insert( make_pair( GuildWarMapInfo.cGuildWarUseMonth, GuildWarMapInfo ) );
	}

	SAFE_DELETE(pSox);
	return true;
}

UINT CDNExtManager::GetGuildWarMapInfoID(char cMonth)
{
	TGuildWarMapInfoData::iterator itor = m_GuildWarMapInfo.find( cMonth );
	if( itor != m_GuildWarMapInfo.end() )
		return itor->second.uiMapTableID;

	return 0;
}

const TPvPGameModeTable* CDNExtManager::GetPvPGameModeTable( const int nItemID )
{
	TPvPGameModeTableData::iterator itor = m_PvPGameModeTable.find( nItemID );
	if( itor != m_PvPGameModeTable.end() )
		return &(itor)->second;

	return NULL;
}

bool CDNExtManager::LoadScheduleTable()
{
	DNTableFileFormat *pSox = LoadExtTable( "ScheduleTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"ScheduleTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int nItemID = pSox->GetItemID(i);

		TSchedule Schedule;

		Schedule.cHour = pSox->GetFieldFromLablePtr( nItemID, "_Hour" )->GetInteger();
		Schedule.cMinute = pSox->GetFieldFromLablePtr( nItemID, "_Minute" )->GetInteger();
		Schedule.nCycle = pSox->GetFieldFromLablePtr( nItemID, "_Cycle" )->GetInteger();
		
		m_ScheduleTable.insert( make_pair( nItemID, Schedule ) );
	}

	SAFE_DELETE(pSox);
	return true;
}

TPvPGameStartConditionTable* CDNExtManager::GetPvPGameStartConditionTable( const UINT uiMaxUser )
{
	TPvPGameStartConditionData::iterator itor = m_PvPGameStartConditionTable.find( uiMaxUser );
	if( itor != m_PvPGameStartConditionTable.end() )
		return itor->second;

	return NULL;
}

const TPvPGameModeTable* CDNExtManager::GetPvPGameModeTableByMatchType( LadderSystem::MatchType::eCode MatchType )
{
	std::map<LadderSystem::MatchType::eCode,std::vector<int>>::iterator itor = m_MatchTypePvPGameModeID.find( MatchType );
	if( itor == m_MatchTypePvPGameModeID.end() )
	{
		_ASSERT(0);
		return NULL;
	}

	if ((*itor).second.size() <= 0)
	{
		_ASSERT(0);
		return NULL;
	}

	::srand( timeGetTime() );
	int iRandIndex = static_cast<int>(::rand()%(*itor).second.size());

	TPvPGameModeTableData::iterator itor2 = m_PvPGameModeTable.find( (*itor).second[iRandIndex] );
	if( itor2 == m_PvPGameModeTable.end() )
	{
		_ASSERT(0);
		return NULL;
	}

	return &(*itor2).second;
}

TSchedule* CDNExtManager::GetSchedule( int nSchedule )
{
	TScheduleData::iterator itor = m_ScheduleTable.find( nSchedule );
	if( itor != m_ScheduleTable.end() )
	{
		return &(itor->second);
	}

	return NULL;
}

#endif

bool CDNExtManager::LoadPCBangData()
{
	DNTableFileFormat *pSox = LoadExtTable( "PCCafeTable" );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"PCCafeTable failed\r\n");
		SAFE_DELETE(pSox);
		return false;
	}

	//##################################################################
	// FieldNum 미리 색인
	//##################################################################

	struct TempFieldNum
	{
		int _Type;
		int _PCParam[PCParamMax];
		int _PremiumPCParam[PCParamMax];
		int _GoldPCParam[PCParamMax];
		int _SilverPCParam[PCParamMax];
		int _RedPCParam[PCParamMax];
	};

	TempFieldNum sFieldNum;
	sFieldNum._Type	= pSox->GetFieldNum( "_Type" );
	sFieldNum._PCParam[0] = pSox->GetFieldNum( "_PCParam1" );
	sFieldNum._PCParam[1] = pSox->GetFieldNum( "_PCParam2" );
	sFieldNum._PremiumPCParam[0] = pSox->GetFieldNum( "_PremiumPCParam1" );
	sFieldNum._PremiumPCParam[1] = pSox->GetFieldNum( "_PremiumPCParam2" );
	sFieldNum._GoldPCParam[0] = pSox->GetFieldNum( "_GoldPCParam1" );
	sFieldNum._GoldPCParam[1] = pSox->GetFieldNum( "_GoldPCParam2" );
	sFieldNum._SilverPCParam[0] = pSox->GetFieldNum( "_SilverPCParam1" );
	sFieldNum._SilverPCParam[1] = pSox->GetFieldNum( "_SilverPCParam2" );
	sFieldNum._RedPCParam[0] = pSox->GetFieldNum( "_RedPCParam1" );
	sFieldNum._RedPCParam[1] = pSox->GetFieldNum( "_RedPCParam1" );

	//##################################################################
	// Load
	//##################################################################

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		int iIdx = pSox->GetIDXprimary( pSox->GetItemID(i) );
		int nType = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._Type )->GetInteger();

		TPCBangData *pPCBangData = GetPCBangData(nType);
		if (!pPCBangData){
			pPCBangData = new TPCBangData;
			pPCBangData->cType = nType;
			m_pPCBangData.insert(make_pair(nType, pPCBangData));
		}

		TPCBangParam Normal = {0,}, Premium = {0,}, Gold = {0,}, Silver = {0,}, Red = {0,};
		for (int j = 0; j < PCParamMax; j++){
			Normal.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PCParam[j] )->GetInteger();
			Premium.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._PremiumPCParam[j] )->GetInteger();
			Gold.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._GoldPCParam[j] )->GetInteger();
			Silver.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._SilverPCParam[j] )->GetInteger();
			Red.nParam[j] = pSox->GetFieldFromLablePtr( iIdx, sFieldNum._RedPCParam[j] )->GetInteger();
		}
	}

	SAFE_DELETE(pSox);
	return true;
}

int CDNExtManager::GetPCBangDataCount()
{
	return (int)m_pPCBangData.size();
}

TPCBangData *CDNExtManager::GetPCBangData(int nPCBangType)
{
	if (m_pPCBangData.empty()) return NULL;

	TMapPCBangData::iterator iter = m_pPCBangData.find(nPCBangType);
	if (iter == m_pPCBangData.end()) return NULL;

	return iter->second;
}

int CDNExtManager::GetPCBangParam1(int nPCBangType, int nPCBangGrade)
{
	TPCBangData *pPCBangData = GetPCBangData(nPCBangType);
	if (!pPCBangData) return 0;

	int nValue = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:	// 피씨방이든 아니든 기본값은 넣어줘야함(일반에서 피씨방 갔을때 값 얻어와야함)
	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty())
				nValue = pPCBangData->VecPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty())
				nValue = pPCBangData->VecPremiumPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty())
				nValue = pPCBangData->VecGoldPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty())
				nValue = pPCBangData->VecSilverPCBangParam[0].nParam[0];
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty())
				nValue = pPCBangData->VecRedPCBangParam[0].nParam[0];
		}
		break;
	}

	return nValue;
}

int CDNExtManager::GetPCBangParam2(int nPCBangType, int nPCBangGrade)
{
	TPCBangData *pPCBangData = GetPCBangData(nPCBangType);
	if (!pPCBangData) return 0;

	int nValue = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:	// 피씨방이든 아니든 기본값은 넣어줘야함(일반에서 피씨방 갔을때 값 얻어와야함)
	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty())
				nValue = pPCBangData->VecPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty())
				nValue = pPCBangData->VecPremiumPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty())
				nValue = pPCBangData->VecGoldPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty())
				nValue = pPCBangData->VecSilverPCBangParam[0].nParam[1];
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty())
				nValue = pPCBangData->VecRedPCBangParam[0].nParam[1];
		}
		break;
	}

	return nValue;
}

int CDNExtManager::GetPCBangNestClearCount(int nPCBangGrade, int nMapID)
{
	TPCBangData *pPCBangData = GetPCBangData(PCBang::Type::NestClearCount);
	if (!pPCBangData) return 0;

	int nClearCount = 0;
	switch (nPCBangGrade)
	{
	case PCBang::Grade::None:
		break;

	case PCBang::Grade::Normal:
		{
			if (!pPCBangData->VecPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecPCBangParam.size(); i++){
					if (pPCBangData->VecPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Premium:
		{
			if (!pPCBangData->VecPremiumPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecPremiumPCBangParam.size(); i++){
					if (pPCBangData->VecPremiumPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecPremiumPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Gold:
		{
			if (!pPCBangData->VecGoldPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecGoldPCBangParam.size(); i++){
					if (pPCBangData->VecGoldPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecGoldPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Silver:
		{
			if (!pPCBangData->VecSilverPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecSilverPCBangParam.size(); i++){
					if (pPCBangData->VecSilverPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecSilverPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;

	case PCBang::Grade::Red:
		{
			if (!pPCBangData->VecRedPCBangParam.empty()){
				for (int i = 0; i < (int)pPCBangData->VecRedPCBangParam.size(); i++){
					if (pPCBangData->VecRedPCBangParam[i].nParam[0] == nMapID){
						nClearCount = pPCBangData->VecRedPCBangParam[i].nParam[1];
						break;
					}
				}
			}
		}
		break;
	}

	return nClearCount;
}

bool CDNExtManager::GetPCBangClearBoxFlag(int nPCBangGrade)
{
	if (nPCBangGrade == PCBang::Grade::None) return false;

	int nValue = GetPCBangParam1(PCBang::Type::ClearBox, nPCBangGrade);
	return (nValue == 1) ? true : false;
}

void CDNExtManager::GetXMLAccountDBID(UINT nAccountDBID, std::wstring &wstrOut)
{
	if (nAccountDBID <= 0) return;
	wstrOut = FormatW(L"<root><Account AccountID=\"%d\"/></root>", nAccountDBID);
}

void CDNExtManager::GetXMLAccountName(WCHAR *pAccountName, std::wstring &wstrOut)
{
	if (!pAccountName) return;
	wstrOut = FormatW(L"<root><Account AccountName=\"%s\"/></root>", pAccountName);
}

void CDNExtManager::GetXMLCharacterDBID(INT64 biCharacterDBID, std::wstring &wstrOut)
{
	if (biCharacterDBID <= 0) return;
	wstrOut = FormatW(L"<root><Character CharacterID=\"%I64d\"/></root>", biCharacterDBID);
}

void CDNExtManager::GetXMLCharacterName(WCHAR *pCharacterName, std::wstring &wstrOut)
{
	if (!pCharacterName) return;
	wstrOut = FormatW(L"<root><Character CharacterName=\"%s\"/></root>", pCharacterName);
}

void CDNExtManager::GetXMLItems(int nItemTotalCount, TSpecialBoxItemInfo *Items, std::wstring &wstrOut)
{
	if (nItemTotalCount <= 0) return;

	std::wstring wstrItem;

	wstrOut.append(L"<root>");
	for (int i = 0; i < nItemTotalCount; i++){
		wstrItem = FormatW(L"<Item ProductFlag=\"%d\" ItemID=\"%d\" ItemCount=\"%d\" ItemDurability=\"%d\" RandomSeed=\"%d\" ItemLevel=\"%d\" ItemPotential=\"%d\" SoulBoundFlag=\"%d\" SealCount=\"%d\" ItemOption=\"%d\" ItemLifespan=\"%d\" EternityFlag=\"%d\"/>",
			Items[i].bCashItem, Items[i].RewardItem.nItemID, Items[i].RewardItem.wCount, (short)Items[i].RewardItem.wDur, Items[i].RewardItem.nRandomSeed, Items[i].RewardItem.cLevel, Items[i].RewardItem.cPotential, 
			Items[i].RewardItem.bSoulbound, Items[i].RewardItem.cSealCount, Items[i].RewardItem.cOption, Items[i].RewardItem.nLifespan, Items[i].RewardItem.bEternity);

		wstrOut.append(wstrItem.c_str());
	}
	wstrOut.append(L"</root>");
}

extern CDNExtManager* g_pExtManager;