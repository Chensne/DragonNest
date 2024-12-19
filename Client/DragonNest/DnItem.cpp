#include "StdAfx.h"
#include "DnItem.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnProtocol.h"
#include "DnDropItem.h"
#include "DnParts.h"
#include "DnUIString.h"
#include "DnCharStatusDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnSkillTask.h"
#include "DnItemTask.h"
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#include "DnVehicleTask.h"
#include "SyncTimer.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

std::map<INT64, CDnItem *> CDnItem::s_nMapSerialSearch;
CSyncLock CDnItem::s_SerialSearchLock;

int CDnItem::s_nStateValueMatchUIStringIndex[] = {
	2140, 2142, 2141, 2143, // Strength ~ Stamina
	5066, 5065,				// PAttack Min, Max
	5068, 5067,				// MAttack Min, Max
	5020, 5062,				// PDefense, MDefense
	5021, 5024, 5023, 5026, 5022, 5025, // Stiff ~ StunResis
	5029, 5030, 5027, 5028, // Element Attack
	5033, 5034, 5031, 5032, // Element Defense
	24, 5063, 5064, 2182, 2279, 2280,
	5074,					// Spirit
	5076,					// MoveSpeed_Safe  
							// Size 32 
							5018, 5019,			    // PAtk min-max , MAtk min-max
													// Size 34 

};

int CDnItem::GetStateValueMatchUIStringIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= 34)
		return 0;

	return CDnItem::s_nStateValueMatchUIStringIndex[nIndex];
}

bool CDnItem::IsEffectSkillItem(int nItemType, bool bCheckVisible)
{
	switch (nItemType)
	{
	case ITEMTYPE_SOURCE:
	{
		return (bCheckVisible == false);
	}
	break;
	case ITEMTYPE_GLOBAL_PARTY_BUFF:
	case ITEMTYPE_BESTFRIENDBUFFITEM:
	case ITEMTYPE_DIRECT_PARTYBUFF:
	case ITEMTYPE_TRANSFORMPOTION:
		return true;
	default:
		break;
	}

	return false;
}

bool CDnItem::IsVisualChangeItem(int nItemType)
{
	switch (nItemType)
	{
	case ITEMTYPE_HAIRDYE:
	case ITEMTYPE_EYEDYE:
	case ITEMTYPE_SKINDYE:
	case ITEMTYPE_FACIAL:
	case ITEMTYPE_HAIRDRESS:
	case ITEMTYPE_PARTS:
	case ITEMTYPE_WEAPON:
		return true;
	}

	return false;
}


CDnItem::CDnItem()
	: MIInventoryItem(MIInventoryItem::Item)
{
	m_nSerialID = -1;
	m_nSeed = 0;
	m_ItemType = (eItemTypeEnum)-1;
	m_Rank = (eItemRank)-1;
	m_SkillApplyType = (ItemSkillApplyType)None;

	m_bIsCashItem = false;

#ifdef PRE_ADD_CASHREMOVE	
	m_tExpireTime = 0; // 삭제대기시간.
	m_bIsCashRemoveItem = false; // 삭제된캐쉬아이템 여부.
#endif

	m_nAmount = 0;
	m_nNeedBuyLadderPoint = 0;
	m_nNeedBuyGuildWarPoint = 0;
	m_nNeedBuyUnionPoint = 0;
	m_nSellAmount = 0;
	m_nLevelLimit = -1;
	m_nMaxOverlapCount = 1;
	m_nOverlapCount = 1;
	m_nClassID = -1;
	m_bNeedAuthentication = false;
	m_bCanDestruction = true;

	m_nSkillID = 0;
	m_nSkillLevel = 0;

	m_nRootSoundIndex = -1;
	m_nDragSoundIndex = -1;
	m_nUseSoundIndex = -1;

	m_fCoolTime = 0.0f;
	m_fDelayTime = 0.0f;
	m_fElapsedDelayTime = 0.0f;

	m_bNewGain = false;

	//	m_dwColor = 0xFFFFFFFF;
	m_bAuthentication = false;
	m_bIdentity = false;

	m_nEnchantTableID = 0;
	m_nEnchantID = 0;
	m_cEnchantLevel = 0;
	m_cMaxEnchantLevel = 0;
	m_cPotentialIndex = 0;
	m_cOptionIndex = 0;
	m_cSealCount = 0;
	m_nSealID = 0;

	m_pEnchant = NULL;
	m_pPotential = NULL;

	m_bCanDisjoint = false;
	m_nDisjointType = 0;
	m_nDisjointCost = 0;
	ZeroMemory(m_nDisjointDropTableID, sizeof(m_nDisjointDropTableID));
#if defined(PRE_FIX_69709)
	m_nEnchantDisjointDrop = 0;
#endif
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	m_nOptionSkillID = 0;
	m_nOptionSkillLevel = 0;
	m_nOptionSkillUsingType = 0;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE

	m_Reversion = ItemReversionEnum::NoReversion;
	m_bEnableCostumeMix = false;
	m_bSoulBound = false;
	m_nAllowedGameType = 0;
	m_nExchangeType = 0;
	m_nNeedBuyItem = 0;
	m_nNeedBuyItemCount = 0;
	m_nNeedPvpRank = 0;

	memset(m_nTypeParam, 0, sizeof(m_nTypeParam));

	//blondy 
	//m_IsDisabled= false;
	//blondy end

	m_bShowAlarmDialog = false;

	m_bEternity = true;
	m_ExpireDate = 0;
	m_nApplicableValue = 0;

	m_nCashTradeCount = 0;

	m_bInfoItem = false;
	m_nLookItemID = ITEMCLSID_NONE;
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	m_cPotentialMoveCount = 0;
#endif
#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	m_nCategoryType = 0;
#endif
#if defined(PRE_ADD_SERVER_WAREHOUSE)
	m_bWstorage = false;
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	m_bUseLimited = false;
#endif
	m_nDragonGemType = 0;
}

CDnItem::~CDnItem()
{
	RemoveSerialSearchMap(this);
	SAFE_DELETE(m_pEnchant);
	SAFE_DELETE(m_pPotential);
	SAFE_RELEASE_SPTR(m_hSkill);

	if (m_nRootSoundIndex != -1) {
		CEtSoundEngine::GetInstance().RemoveSound(m_nRootSoundIndex);
	}
	if (m_nDragSoundIndex != -1) {
		CEtSoundEngine::GetInstance().RemoveSound(m_nDragSoundIndex);
	}
	if (m_nUseSoundIndex != -1) {
		CEtSoundEngine::GetInstance().RemoveSound(m_nUseSoundIndex);
	}
}

bool CDnItem::Initialize(int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound, int nLookItemID)
{
	m_nClassID = nTableID;
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return false;
	if (!pSox->IsExistItem(nTableID)) return false;

	m_nSeed = nSeed;
	m_cEnchantLevel = cEnchantLevel;
	m_cPotentialIndex = cPotentialIndex;
	m_cOptionIndex = cOptionIndex;
	m_bSoulBound = bSoulBound;
	m_cSealCount = cSealCount;
	m_szName = GetItemFullName(nTableID, m_cOptionIndex);
	int nNameID = pSox->GetFieldFromLablePtr(nTableID, "_DescriptionID")->GetInteger();
	char *szParam = pSox->GetFieldFromLablePtr(nTableID, "_DescriptionIDParam")->GetString();
	MakeUIStringUseVariableParam(m_szDescription, nNameID, szParam);
	m_ItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr(nTableID, "_Type")->GetInteger();
	m_nTypeParam[0] = pSox->GetFieldFromLablePtr(nTableID, "_TypeParam1")->GetInteger();
	m_nTypeParam[1] = pSox->GetFieldFromLablePtr(nTableID, "_TypeParam2")->GetInteger();
	m_nExchangeType = pSox->GetFieldFromLablePtr(nTableID, "_ExchangeType")->GetInteger();
	m_nLevelLimit = pSox->GetFieldFromLablePtr(nTableID, "_LevelLimit")->GetInteger();
	m_Rank = (eItemRank)pSox->GetFieldFromLablePtr(nTableID, "_Rank")->GetInteger();
	m_Reversion = (ItemReversionEnum)pSox->GetFieldFromLablePtr(nTableID, "_Reversion")->GetInteger();
	m_bNeedAuthentication = (pSox->GetFieldFromLablePtr(nTableID, "_IsAuthentication")->GetInteger() == TRUE) ? true : false;
	m_bCanDestruction = (pSox->GetFieldFromLablePtr(nTableID, "_IsDestruction")->GetInteger() == TRUE) ? true : false;
	m_bIsCashItem = (pSox->GetFieldFromLablePtr(nTableID, "_IsCash")->GetInteger() == TRUE) ? true : false;
	m_nAmount = pSox->GetFieldFromLablePtr(nTableID, "_Amount")->GetInteger();
	m_nSellAmount = pSox->GetFieldFromLablePtr(nTableID, "_SellAmount")->GetInteger();
	m_nMaxOverlapCount = pSox->GetFieldFromLablePtr(nTableID, "_OverlapCount")->GetInteger();
	m_bShowAlarmDialog = (pSox->GetFieldFromLablePtr(nTableID, "_ShowAlarmDialog")->GetInteger() == TRUE) ? true : false;
	m_nAllowedGameType = pSox->GetFieldFromLablePtr(nTableID, "_AllowMapTypes")->GetInteger();
	SetLookItemID(nLookItemID, false);

	m_nNeedBuyItem = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyItem")->GetInteger();
	m_nNeedBuyItemCount = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyItemCount")->GetInteger();
	m_nNeedPvpRank = pSox->GetFieldFromLablePtr(nTableID, "_NeedPvPRank")->GetInteger();
	m_nNeedBuyLadderPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyLadderPoint")->GetInteger();
	m_nNeedBuyGuildWarPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedGuildFestPoint")->GetInteger();
	m_nNeedBuyUnionPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedUnionPoint")->GetInteger();
	m_nSealID = pSox->GetFieldFromLablePtr(nTableID, "_SealID")->GetInteger();
	// 인챈트테이블
	m_nEnchantID = pSox->GetFieldFromLablePtr(nTableID, "_EnchantID")->GetInteger();
	m_nApplicableValue = pSox->GetFieldFromLablePtr(nTableID, "_ApplicableValue")->GetInteger();

	//Dragon Gem
	m_nDragonGemType = pSox->GetFieldFromLablePtr(nTableID, "_dragonjeweltype")->GetInteger();

	// 분해 관련 데이터 ///////////////////////////////////////////////////////
	m_nDisjointType = pSox->GetFieldFromLablePtr(nTableID, "_DisjointorType")->GetInteger();
	m_nDisjointCost = pSox->GetFieldFromLablePtr(nTableID, "_Disjointamount")->GetInteger();
	m_bEnableCostumeMix = (pSox->GetFieldFromLablePtr(nTableID, "_Compose")->GetInteger()) ? true : false;

	m_nCashTradeCount = pSox->GetFieldFromLablePtr(nTableID, "_CashTradeCount")->GetInteger();
#ifdef PRE_ADD_ITEMCAT_TOOLTIP_INFO
	DNTableCell* pItemCatCell = pSox->GetFieldFromLablePtr(nTableID, "_ItemCategoryType");
	if (pItemCatCell)
		m_nCategoryType = pItemCatCell->GetInteger();
	else
		_ASSERT(0);
#endif
#if defined(PRE_ADD_SERVER_WAREHOUSE)	
	m_bWstorage = (pSox->GetFieldFromLablePtr(nTableID, "_AbleWStorage")->GetInteger()) ? true : false;
#endif	
	char acBuf[128];
	for (int i = 0; i < NUM_DISJOINT_DROP; ++i)
	{
		sprintf_s(acBuf, sizeof(acBuf), "_DisjointDrop%d", i + 1);
		m_nDisjointDropTableID[i] = pSox->GetFieldFromLablePtr(nTableID, acBuf)->GetInteger();
	}

	for (int i = 0; i < NUM_DISJOINT_DROP; ++i)
	{
		if (0 != m_nDisjointDropTableID[i])
		{
			m_bCanDisjoint = true;
			break;
		}
	}

	int idForIcon = nTableID;
	if (m_nLookItemID != ITEMCLSID_NONE)
		idForIcon = m_nLookItemID;

	// 아이콘 이미지 셋
	const DNTableCell* pField = pSox->GetFieldFromLablePtr(idForIcon, "_IconImageIndex");
	if (pField != NULL)
		SetIconImageIndex(pField->GetInteger());

	//////////////////////////////////////////////////////////////////////////

	// 사용 가능 직업
	char *szPermitStr = pSox->GetFieldFromLablePtr(nTableID, "_NeedJobClass")->GetString();
	int nJobIndex;
	for (int i = 0;; i++) {
		//char *pStr = _GetSubStrByCount( i, szPermitStr, ';' );
		std::string strValue = _GetSubStrByCountSafe(i, szPermitStr, ';');
		if (strValue.size() == 0) break;
		nJobIndex = atoi(strValue.c_str());
		if (nJobIndex < 1) continue;
		m_nVecPermitJobList.push_back(nJobIndex);
	}

	// -1 인 시드는 PreInitialize 일 경우기 때문에 Random 을 돌리면 틀어진다.
	if (nSeed == -1) return true;
	GetRandom().LockSeed();
	_srand(nSeed);

	char szLabel[32];

	// 기본 능력치 계산
	for (int i = 0; i<10; i++) {
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetInteger();
		sprintf_s(szLabel, "_State%d_GenProb", i + 1);

		if (IsPossibleProb(pSox, szLabel)) {
			sprintf_s(szLabel, "_State%d_Min", i + 1);
			char *szMin = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetString();
			sprintf_s(szLabel, "_State%d_Max", i + 1);
			char *szMax = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetString();

			///////////////////////////
			CalcStateValue(this, nStateType, szMin, szMax);
		}
		else DummyRandom();
	}
	CalcValueType();

	// Skill
	if (IsPossibleProb(pSox, "_Skill_GenProb")) {
		m_nSkillID = pSox->GetFieldFromLablePtr(nTableID, "_SkillID")->GetInteger();
		m_nSkillLevel = pSox->GetFieldFromLablePtr(nTableID, "_SkillLevel")->GetInteger();
		m_SkillApplyType = (ItemSkillApplyType)pSox->GetFieldFromLablePtr(nTableID, "_SkillUsingType")->GetInteger();

		// 스킬 딜레이를 아이템 쿨타임으로 적용 시킴..
		DNTableFileFormat*  pSkillLevelTable = GetDNTable(CDnTableDB::TSKILLLEVEL);

		vector<int> vlSkillLevelList;
		//if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", m_nSkillID, vlSkillLevelList ) != 0 ) 
		GetSkillTask().GetSkillLevelList(m_nSkillID, vlSkillLevelList, CDnSkill::PVE);		// pve 기준으로 처리하면 된다.
		if (false == vlSkillLevelList.empty())
		{
			int iSkillLevelTableID = -1;
			for (int i = 0; i < (int)vlSkillLevelList.size(); ++i)
			{
				int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr(vlSkillLevelList.at(i), "_SkillLevel")->GetInteger();
				if (iNowLevel == m_nSkillLevel)
				{
					iSkillLevelTableID = vlSkillLevelList.at(i);
					break;
				}
			}

			m_fDelayTime = pSkillLevelTable->GetFieldFromLablePtr(iSkillLevelTableID, "_DelayTime")->GetInteger() / 1000.f;
		}

		// 아이템을 쓰는 순간 UI 에서 쓰는 클리핑 쿨 타임 셋팅해줌.	
	}
	else DummyRandom();

	// 옵션(기본포텐샬 능력치) 계산
	InitializeOption();
	// 인첸트 능력치 계산
	InitializeEnchant();
	// 포텐샬 능력치 계산
	InitializePotential();

	// 사운드
	const char *szFileName;
#ifdef PRE_FIX_MEMOPT_EXT
	szFileName = CommonUtil::GetFileNameFromFileEXT(pSox, nTableID, "_RootSound");
#else
	szFileName = pSox->GetFieldFromLablePtr(nTableID, "_RootSound")->GetString();
#endif
	if (szFileName && strlen(szFileName) > 0) {
		m_nRootSoundIndex = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
	}

#ifdef PRE_FIX_MEMOPT_EXT
	szFileName = CommonUtil::GetFileNameFromFileEXT(pSox, nTableID, "_DragSound");
#else
	szFileName = pSox->GetFieldFromLablePtr(nTableID, "_DragSound")->GetString();
#endif
	if (szFileName && strlen(szFileName) > 0) {
		m_nDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
	}

#ifdef PRE_FIX_MEMOPT_EXT
	szFileName = CommonUtil::GetFileNameFromFileEXT(pSox, nTableID, "_UseSound");
#else
	szFileName = pSox->GetFieldFromLablePtr(nTableID, "_UseSound")->GetString();
#endif
	if (szFileName && strlen(szFileName) > 0) {
		m_nUseSoundIndex = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
	}

	// 스킬이 붙어있는 아이템이라면 사용 가능 체크를 위해 스킬 생성..
	if (ApplySkill == m_SkillApplyType)
	{
		m_hSkill = CDnSkill::CreateSkill(CDnActor::s_hLocalActor, m_nSkillID, m_nSkillLevel);

		if (m_hSkill)
			m_hSkill->SetCreatedByItem(true);
	}

	GetRandom().UnlockSeed();
	return true;
}


// 기본정보만을 보기 위해 반드시 필요한 정보들로 구성된 초기화 함수. 현재 가챠에서 사용됨.
bool CDnItem::InitializeForInformation(int nTableID, int nSeed, char cOptionIndex)
{
	m_nClassID = nTableID;
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return false;
	if (!pSox->IsExistItem(nTableID)) return false;

	m_nSeed = nSeed;
	//m_cEnchantLevel = cEnchantLevel;
	//m_cPotentialIndex = cPotentialIndex;
	m_cOptionIndex = cOptionIndex;
	//m_bSoulBound = bSoulBound;
	//m_cSealCount = cSealCount;
	m_szName = GetItemFullName(nTableID);
	int nNameID = pSox->GetFieldFromLablePtr(nTableID, "_DescriptionID")->GetInteger();
	char *szParam = pSox->GetFieldFromLablePtr(nTableID, "_DescriptionIDParam")->GetString();
	MakeUIStringUseVariableParam(m_szDescription, nNameID, szParam);
	m_ItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr(nTableID, "_Type")->GetInteger();
	m_nTypeParam[0] = pSox->GetFieldFromLablePtr(nTableID, "_TypeParam1")->GetInteger();
	m_nTypeParam[1] = pSox->GetFieldFromLablePtr(nTableID, "_TypeParam2")->GetInteger();
	m_nLevelLimit = pSox->GetFieldFromLablePtr(nTableID, "_LevelLimit")->GetInteger();
	m_Rank = (eItemRank)pSox->GetFieldFromLablePtr(nTableID, "_Rank")->GetInteger();
	m_Reversion = (ItemReversionEnum)pSox->GetFieldFromLablePtr(nTableID, "_Reversion")->GetInteger();
	m_bNeedAuthentication = (pSox->GetFieldFromLablePtr(nTableID, "_IsAuthentication")->GetInteger() == TRUE) ? true : false;
	m_bCanDestruction = (pSox->GetFieldFromLablePtr(nTableID, "_IsDestruction")->GetInteger() == TRUE) ? true : false;
	m_bIsCashItem = (pSox->GetFieldFromLablePtr(nTableID, "_IsCash")->GetInteger() == TRUE) ? true : false;
	m_nAmount = pSox->GetFieldFromLablePtr(nTableID, "_Amount")->GetInteger();
	m_nSellAmount = pSox->GetFieldFromLablePtr(nTableID, "_SellAmount")->GetInteger();
	m_nMaxOverlapCount = pSox->GetFieldFromLablePtr(nTableID, "_OverlapCount")->GetInteger();
	m_bShowAlarmDialog = (pSox->GetFieldFromLablePtr(nTableID, "_ShowAlarmDialog")->GetInteger() == TRUE) ? true : false;
	m_nAllowedGameType = pSox->GetFieldFromLablePtr(nTableID, "_AllowMapTypes")->GetInteger();

	m_nNeedBuyItem = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyItem")->GetInteger();
	m_nNeedBuyItemCount = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyItemCount")->GetInteger();
	m_nNeedPvpRank = pSox->GetFieldFromLablePtr(nTableID, "_NeedPvPRank")->GetInteger();
	m_nNeedBuyLadderPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyLadderPoint")->GetInteger();
	m_nNeedBuyGuildWarPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedBuyLadderPoint")->GetInteger();
	m_nNeedBuyUnionPoint = pSox->GetFieldFromLablePtr(nTableID, "_NeedUnionPoint")->GetInteger();
	m_nSealID = pSox->GetFieldFromLablePtr(nTableID, "_SealID")->GetInteger();
	// 인챈트테이블
	//m_nEnchantID = pSox->GetFieldFromLablePtr( nTableID, "_EnchantID" )->GetInteger();

	// 분해 관련 데이터 ///////////////////////////////////////////////////////
	//m_nDisjointType = pSox->GetFieldFromLablePtr( nTableID, "_DisjointorType" )->GetInteger();
	//m_nDisjointCost = pSox->GetFieldFromLablePtr( nTableID, "_Disjointamount" )->GetInteger();

	//char acBuf[ 128 ];
	//for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
	//{
	//	sprintf_s( acBuf, sizeof(acBuf), "_DisjointDrop%d", i+1 );
	//	m_nDisjointDropTableID[ i ] = pSox->GetFieldFromLablePtr( nTableID, acBuf )->GetInteger();
	//}

	//for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
	//{
	//	if( 0 != m_nDisjointDropTableID[ i ] )
	//	{
	//		m_bCanDisjoint = true;
	//		break;
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////

	//// 사용 가능 직업
	//DNTableCell Field;
	//if( pSox->GetFieldFromLable( nTableID, "_NeedJobClass", Field ) == false ) return false;
	//char *szPermitStr = Field.GetString();
	//int nJobIndex;
	//for( int i=0;; i++ ) {
	//	//char *pStr = _GetSubStrByCount( i, szPermitStr, ';' );
	//	std::string strValue = _GetSubStrByCountSafe( i, szPermitStr, ';' );
	//	if( strValue.size() == 0 ) break;
	//	nJobIndex = atoi(strValue.c_str());
	//	if( nJobIndex < 1 ) continue;
	//	m_nVecPermitJobList.push_back( nJobIndex );
	//}

	// -1 인 시드는 PreInitialize 일 경우기 때문에 Random 을 돌리면 틀어진다.
	if (nSeed == -1) return true;
	if (nSeed == 0) GetRandom().LockSeed();
	_srand(nSeed);

	char szLabel[32];

	// 기본 능력치 계산
	for (int i = 0; i<10; i++) {
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetInteger();
		sprintf_s(szLabel, "_State%d_GenProb", i + 1);

		if (IsPossibleProb(pSox, szLabel)) {
			sprintf_s(szLabel, "_State%d_Min", i + 1);
			char *szMin = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetString();
			sprintf_s(szLabel, "_State%d_Max", i + 1);
			char *szMax = pSox->GetFieldFromLablePtr(nTableID, szLabel)->GetString();

			///////////////////////////
			CalcStateValue(this, nStateType, szMin, szMax);
		}
		else DummyRandom();
	}
	CalcValueType();

	// Skill
	if (IsPossibleProb(pSox, "_Skill_GenProb"))
	{
		//m_nSkillID = pSox->GetFieldFromLablePtr( nTableID, "_SkillID" )->GetInteger();
		//m_nSkillLevel = pSox->GetFieldFromLablePtr( nTableID, "_SkillLevel" )->GetInteger();
		//m_SkillApplyType = (ItemSkillApplyType)pSox->GetFieldFromLablePtr( nTableID, "_SkillUsingType" )->GetInteger();

		//// 스킬 딜레이를 아이템 쿨타임으로 적용 시킴..
		//DNTableFileFormat*  pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		//DNTableFileFormat*  pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

		//vector<int> vlSkillLevelList;
		//if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", m_nSkillID, vlSkillLevelList ) != 0 ) 
		//{
		//	int iSkillLevelTableID = -1;
		//	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
		//	{
		//		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		//		if( iNowLevel == m_nSkillLevel )
		//		{
		//			iSkillLevelTableID = vlSkillLevelList.at( i );
		//			break;
		//		}
		//	}

		//	m_fDelayTime = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_DelayTime" )->GetInteger() / 1000.f;
		//}

		// 아이템을 쓰는 순간 UI 에서 쓰는 클리핑 쿨 타임 셋팅해줌.	
	}
	else DummyRandom();

	// 옵션(기본포텐샬 능력치) 계산
	InitializeOption();
	//// 인첸트, 포텐샬 능력치 계산
	//InitializeEnchant();

	//// 아이콘 이미지 셋
	//SetIconImageIndex( pSox->GetFieldFromLablePtr( nTableID, "_IconImageIndex" )->GetInteger() );

	//// 사운드
	//const char *szFileName;
	//szFileName = pSox->GetFieldFromLablePtr( nTableID, "_RootSound" )->GetString();
	//if( szFileName && strlen( szFileName ) > 0 ) {
	//	m_nRootSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	//}
	//szFileName = pSox->GetFieldFromLablePtr( nTableID, "_DragSound" )->GetString();
	//if( szFileName && strlen( szFileName ) > 0 ) {
	//	m_nDragSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	//}
	//szFileName = pSox->GetFieldFromLablePtr( nTableID, "_UseSound" )->GetString();
	//if( szFileName && strlen( szFileName ) > 0 ) {
	//	m_nUseSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
	//}

	//// 스킬이 붙어있는 아이템이라면 사용 가능 체크를 위해 스킬 생성..
	//if( ApplySkill == m_SkillApplyType )
	//	m_hSkill = CDnSkill::CreateSkill( CDnActor::s_hLocalActor, m_nSkillID, m_nSkillLevel );

	if (nSeed == 0) GetRandom().UnlockSeed();
	return true;
}


bool CDnItem::IsPossibleProb(DNTableFileFormat* pSox, const char *szLabel)
{
	_fpreset();
	int nProb = (int)(pSox->GetFieldFromLablePtr(m_nClassID, szLabel)->GetFloat() * 100.f);
	if (_rand() % 100 < nProb) return true;
	return false;
}

int CDnItem::GetRandomStateNumber(int nMin, int nMax)
{
	int nRandom = _rand();
	if (nMin == nMax) return nMin;

	int nValue = nMax - nMin;

	_fpreset();
	float fRatio = (1.f / (float)INT_MAX) * (float)nRandom;
	return nMin + (int)(nValue * fRatio);
}

float CDnItem::GetRandomStateRatio(float fMin, float fMax)
{
	int nRandom = _rand();

	_fpreset();

	int nMin = (int)(fMin * 1000.f);
	int nMax = (int)(fMax * 1000.f);
	if (nMin == nMax) return (nMin * 0.001f);

	int nValue = nMax - nMin;

	float fRatio = (1.f / (float)INT_MAX) * (float)nRandom;
	return (nMin + (int)(nValue * fRatio)) * 0.001f;
}

void CDnItem::DummyRandom(int nCount)
{
	for (int i = 0; i<nCount; i++) _rand();
}


eItemTypeEnum CDnItem::GetItemType(int nItemTableID)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return (eItemTypeEnum)-1;

	if (pSox->IsExistItem(nItemTableID) == false) return (eItemTypeEnum)-1;

	return (eItemTypeEnum)pSox->GetFieldFromLablePtr(nItemTableID, "_Type")->GetInteger();
}


tstring CDnItem::GetItemName(int nItemTableID)
{
	tstring szName;

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox || !pSox->IsExistItem(nItemTableID)) return szName;

	szName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(nItemTableID, "_NameID")->GetInteger());

	return szName;
}

tstring CDnItem::GetItemFullName(int nItemTableID, char cOptionIndex)
{
	tstring szName;

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox || !pSox->IsExistItem(nItemTableID)) return szName;

	int nNameID = pSox->GetFieldFromLablePtr(nItemTableID, "_NameID")->GetInteger();
	char *szParam = pSox->GetFieldFromLablePtr(nItemTableID, "_NameIDParam")->GetString();
	MakeUIStringUseVariableParam(szName, nNameID, szParam);

	if (cOptionIndex > 0) {
		int nOptionTableID = GetOptionTableID(nItemTableID, cOptionIndex);
		if (nOptionTableID > 0) {
			DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
			int nPrefixNameID = pPotentialSox->GetFieldFromLablePtr(nOptionTableID, "_PrefixNameID")->GetInteger();
			if (nPrefixNameID > 0) {
				szName += L" (";
				szName += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nPrefixNameID);
				szName += L")";
			}
		}
	}

	return szName;
}

int CDnItem::GetItemTypeParam(int nItemTableID, int nIndex)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return 0;
	if (pSox->IsExistItem(nItemTableID) == false) return 0;
#ifdef PRE_MOD_GACHA_SYSTEM
	if (nIndex < 0 || nIndex > 2) return 0;
#else
	if (nIndex < 0 || nIndex > 1) return 0;
#endif

	char szLabel[32] = { 0, };
	sprintf_s(szLabel, "_TypeParam%d", nIndex + 1);
	return pSox->GetFieldFromLablePtr(nItemTableID, szLabel)->GetInteger();
}

bool CDnItem::IsPermitPlayer(const int nItemID, const std::vector<int> &nVecJobList)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);

	if (NULL == pSox)
		return false;

	std::vector<int> vPermitJobList;
	char *szPermitStr = pSox->GetFieldFromLablePtr(nItemID, "_NeedJobClass")->GetString();
	for (int i = 0;; i++)
	{
		std::string strValue = _GetSubStrByCountSafe(i, szPermitStr, ';');
		if (strValue.size() == 0)
			break;

		int nJobIndex = atoi(strValue.c_str());

		if (nJobIndex < 1)
			continue;

		vPermitJobList.push_back(nJobIndex);
	}

	if (vPermitJobList.size() == 0)
		return true;

	for (DWORD itr = 0; itr < nVecJobList.size(); ++itr)
	{
		for (DWORD jtr = 0; jtr < vPermitJobList.size(); ++jtr)
		{
			if (nVecJobList[itr] == vPermitJobList[jtr])
				return true;
		}
	}
	return false;
}

bool CDnItem::IsNeedJob() const
{
	if (m_nVecPermitJobList.size() == 0) return false;
	return true;
}

bool CDnItem::IsPermitPlayer(const std::vector<int> &nVecJobList) const
{
	if (m_nVecPermitJobList.size() == 0) return true;
	for (DWORD i = 0; i<nVecJobList.size(); i++) {
		for (DWORD j = 0; j<m_nVecPermitJobList.size(); j++) {
			if (nVecJobList[i] == m_nVecPermitJobList[j]) return true;
		}
	}
	return false;
}

int CDnItem::GetNeedJob(int nIndex) const
{
	_ASSERT(0 <= nIndex && nIndex < (int)m_nVecPermitJobList.size());
	return m_nVecPermitJobList[nIndex];
}

void CDnItem::BeginCoolTime(void)
{
	m_fCoolTime = (m_fDelayTime == 0.0f) ? 0.0f : 1.0f;
	m_fElapsedDelayTime = m_fDelayTime;
}



bool CDnItem::ActivateSkillEffect(DnActorHandle hActor, bool bPickUp)
{
	if (m_nSkillID < 1) return true;
	if (m_nSkillLevel < 1) return true;
	SAFE_RELEASE_SPTR(m_hSkill);

	bool bResult = hActor->UseItemSkill(m_nSkillID, m_nSkillLevel, m_SkillApplyType, m_nClassID);
	assert(bResult && "CDnItem::ActivateSkillEffect() : 아이템 스킬 사용 실패");

	// Note: 픽업 아이템은 픽업 메시지가 가기 때문에 따로 패킷을 다른 클라한테 보내지 않아도 된다.
	if (bPickUp == false &&
		hActor == CDnActor::s_hLocalActor &&
		ItemSkillApplyType::ApplySkill == m_SkillApplyType)
	{
		// Note: ApplySkill 아이템인 경우 아이템 사용으로 스킬 패킷을 쏴준다.
		// 아이템 사용 패킷은 사용한 클라이언트와 게임서버끼리만 통신하므로 나머지 클라이언트들의 
		// 겉보기 연출을 위해 스킬 사용 패킷을 추가로 브로드캐스팅 요구를 한다.
		BYTE pBuffer[128];
		CPacketCompressStream Stream(pBuffer, 128);

		// 아이템 스킬은 필요없으므로 기본 값을 보내줌.
		bool bUseApplySkillItem = true;
		bool abSignalSkillCheck[3] = { false };

		int nEnchatSkillID = 0;
		bool bAutoUseFromServer = false;

		Stream.Write(&m_nSkillID, sizeof(int));
		Stream.Write(&m_nSkillLevel, sizeof(char));
		Stream.Write(&bUseApplySkillItem, sizeof(bool));
		Stream.Write(abSignalSkillCheck, sizeof(abSignalSkillCheck));
		Stream.Write(&EtVec3toVec2(*hActor->GetMoveVectorZ()), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT);
		Stream.Write(&EtVec3toVec2(*hActor->GetLookDir()), sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT);
		Stream.Write(&bAutoUseFromServer, sizeof(bool));
		Stream.Write(&nEnchatSkillID, sizeof(int));
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
		Stream.Write(hActor->GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT);
#endif

		hActor->Send(eActor::CS_USESKILL, &Stream);
	}

	// 쿨 타임 시작
	BeginCoolTime();

	//m_hSkill = CDnSkill::CreateSkill( hActor, m_nSkillID, m_nSkillLevel );
	//m_hSkill->OnBegin( 0, 0.f );
	return true;
}

bool CDnItem::DeactivateSkillEffect()
{
	return true;
}

CDnItem *CDnItem::CreateItem(int nTableID, int nSeed, bool bSoulBound, int nLookItemID /*= ITEMCLSID_NONE*/)
{
	CDnItem *pItem = new CDnItem;
	//bool Initialize( int nTableID, int nSeed, char cOptionIndex = 0, char cEnchantLevel = 0, char cPotentialIndex = 0, char cSealCount = 0, bool bSoulBound = false);
	if (pItem->Initialize(nTableID, nSeed, 0, 0, 0, 0, bSoulBound, nLookItemID) == false) {
		SAFE_DELETE(pItem);
		return NULL;
	}
	return pItem;
}

MIInventoryItem::ItemConditionEnum CDnItem::GetItemCondition(void)
{
	if (!CDnActor::s_hLocalActor)
		return ItemConditionEnum::Unusable;

	CDnLocalPlayerActor *pLocalPlayer = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

	if (CDnActor::s_hLocalActor->IsDie())
		return ItemConditionEnum::Unusable;

	ItemConditionEnum eResult = Usable;

	if (pLocalPlayer->IsSwapSingleSkin())
	{
		if (IsVisualChangeItem(m_ItemType) == true)
			return Unusable;
#ifdef PRE_ADD_TRANSFORM_POTION
		if (IsQuickSlotItem() == true && pLocalPlayer->IsVillageTransformMode() == false)
			return Unusable;
#endif
	}

	// 근원 아이템은 아무데서나 사용 가능.
	if (ITEMTYPE_SOURCE == m_ItemType)
		return Usable;

	if (ITEMTYPE_DIRECT_PARTYBUFF == m_ItemType)
		return Usable;

	if (ITEMTYPE_GLOBAL_PARTY_BUFF == m_ItemType)
		return Usable;

	if (ITEMTYPE_TRANSFORMPOTION == m_ItemType || ITEMTYPE_HEAD_SCALE_POTION == m_ItemType)
	{
		if (pLocalPlayer->IsUsableItem(this))
			return Usable;
	}

	if (ITEMTYPE_PET_FOOD == m_ItemType && pLocalPlayer->IsSummonPet())
		return Usable;

	// 스킬을 쓰는 아이템이라면 해당 스킬의 사용 가능 여부를 확인한다.
	if (ApplySkill == m_SkillApplyType && m_hSkill && m_ItemType != ITEMTYPE_GLYPH)
	{
		if ((CDnActor::s_hLocalActor != m_hSkill->GetActor()))
			m_hSkill->SetHasActor(CDnActor::s_hLocalActor);

		eResult = m_hSkill->GetItemCondition();
		if (eResult == ItemConditionEnum::Usable) {
			if (CDnActor::s_hLocalActor && !(pLocalPlayer->IsUsableItem(this)))
				eResult = ItemConditionEnum::Unusable;
		}
	}
	// 귀환 아이템은 피븝피에서만 사용 가능 하다.
	if (ITEMTYPE_RETURN_HOME == m_ItemType)
	{
		eResult = ItemConditionEnum::Unusable;

		CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
		if (pGameTask && GameTaskType::PvP == pGameTask->GetGameTaskType())
		{
			CDnPvPGameTask * pPvPGameTask = (CDnPvPGameTask *)pGameTask;
			if (PvPCommon::GameMode::PvP_GuildWar == pPvPGameTask->GetGameMode())
				eResult = ItemConditionEnum::Usable;
		}
	}
	// 마을 이동 아이템은 사용할 수 있는지 여부가 맵 테이블마다 있음
	if (ITEMTYPE_FREE_PASS == m_ItemType ||
		ITEMTYPE_UNLIMITED_FREE_PASS == m_ItemType)
	{
		eResult = ItemConditionEnum::Unusable;

		DNTableFileFormat*  pMapTable = GetTableDB().GetTable(CDnTableDB::TMAP);
		bool bAllowWarpVillage = pMapTable->GetFieldFromLablePtr(CGlobalInfo::GetInstance().m_nCurrentMapIndex, "_AllowFreePass")->GetInteger() == 1 ? true : false;
		if (bAllowWarpVillage)
		{
			eResult = ItemConditionEnum::Usable;
		}
	}

	if (m_ItemType == ITEMTYPE_VEHICLE || m_ItemType == ITEMTYPE_VEHICLE_SHARE)
	{
		if (pLocalPlayer->IsCanVehicleMode() == false)
			eResult = ItemConditionEnum::Unusable;
	}
	else if (m_ItemType == ITEMTYPE_PET && CDnVehicleTask::IsActive())
	{
		TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial(GetSerialID());
		if (pPetCompact)
		{
			const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();
			if ((!pLocalPlayer->IsInPetSummonableMap()) ||
				(!pPetCompact->Vehicle[Vehicle::Slot::Body].bEternity && pNowTime >= pPetCompact->Vehicle[Vehicle::Slot::Body].tExpireDate))	// 만료기간, 소환가능 지역 체크
				eResult = ItemConditionEnum::Unusable;
		}
	}

	return eResult;
}

void CDnItem::ProcessCoolTime(LOCAL_TIME LocalTime, float fDelta)
{
	if (m_fElapsedDelayTime > 0.0f)
	{
		m_fElapsedDelayTime -= fDelta;
		if (m_fElapsedDelayTime < 0.0f)
		{
			m_fElapsedDelayTime = 0.0f;
			m_fCoolTime = 0.0f;
		}
		else
			m_fCoolTime = (m_fElapsedDelayTime / m_fDelayTime);
	}
}

void CDnItem::PlayInstantUseSound()
{
	int nSoundIndex = GetUseSoundIndex();
	if (nSoundIndex != -1)
	{
		CEtSoundEngine::GetInstance().PlaySound("2D", GetUseSoundIndex());
	}
}

void CDnItem::SetNewGain(bool bNew, bool bExistItem)
{
	if (bNew)
	{
		if (GetItemType() == ITEMTYPE_WEAPON ||
			GetItemType() == ITEMTYPE_PARTS ||
			GetItemType() == ITEMTYPE_PLATE ||
			GetItemType() == ITEMTYPE_HAIRDYE ||
			GetItemType() == ITEMTYPE_SKINDYE ||
			GetItemType() == ITEMTYPE_EYEDYE ||
			GetItemType() == ITEMTYPE_FACIAL ||
			GetItemType() == ITEMTYPE_HAIRDRESS ||
			GetItemType() == ITEMTYPE_PREMIUM_POST ||
			GetItemType() == ITEMTYPE_SEAL ||
			GetItemType() == ITEMTYPE_RESET_SKILL ||
			GetItemType() == ITEMTYPE_WORLDMSG ||
			GetItemType() == ITEMTYPE_ENCHANT_BREAKGAURD ||
			GetItemType() == ITEMTYPE_ENCHANT_SHIELD ||
			GetItemType() == ITEMTYPE_REWARDITEM_IDENTIFY ||
			GetItemType() == ITEMTYPE_GLYPH ||
			GetItemType() == ITEMTYPE_COSTUMEMIX ||
			GetItemType() == ITEMTYPE_NPC_VOICEPLAYER ||
			GetItemType() == ITEMTYPE_VOICEFONT ||
			GetItemType() == ITEMTYPE_VEHICLE ||
			GetItemType() == ITEMTYPE_VEHICLE_SHARE ||
			GetItemType() == ITEMTYPE_VEHICLEPARTS ||
			GetItemType() == ITEMTYPE_PET ||
			GetItemType() == ITEMTYPE_PETPARTS ||
			GetItemType() == ITEMTYPE_CHARNAME ||
			GetItemType() == ITEMTYPE_REMOTE_ENCHANT ||
			GetItemType() == ITEMTYPE_REPAIR_EQUIPITEM ||
			GetItemType() == ITEMTYPE_SURROUNDCHAT ||
			GetItemType() == ITEMTYPE_GUILDMARK ||
			GetItemType() == ITEMTYPE_GACHACOIN ||
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
			GetItemType() == ITEMTYPE_POTENTIAL_EXTRACTOR ||
#endif
			GetItemType() == ITEMTYPE_REMOTE_ITEMCOMPOUND ||
			GetItemType() == ITEMTYPE_REMOTE_WAREHOUSE)
		{
			m_bNewGain = bNew;
		}

		// 보옥(중첩이 되므로 별도로 체크)
		else if (GetItemType() == ITEMTYPE_JEWEL ||
			GetItemType() == ITEMTYPE_POTENTIAL_JEWEL ||
			GetItemType() == ITEMTYPE_ENCHANT_JEWEL ||
			GetItemType() == ITEMTYPE_CHARM ||
#if defined( PRE_ADD_EASYGAMECASH )
			GetItemType() == ITEMTYPE_ALLGIVECHARM ||
#endif
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
			GetItemType() == ITEMTYPE_CHARMRANDOM ||
#endif
			GetItemType() == ITEMTYPE_SOURCE ||
			GetItemType() == ITEMTYPE_GLOBAL_PARTY_BUFF ||
			GetItemType() == ITEMTYPE_TRANSFORMPOTION ||
			GetItemType() == ITEMTYPE_HEAD_SCALE_POTION ||
			GetItemType() == ITEMTYPE_PVPEXPUP)
		{
			if (!bExistItem)
				m_bNewGain = bNew;
		}

		// 물약
		else if (IsQuickSlotItem() && !bExistItem)
			m_bNewGain = bNew;
	}
	else
		m_bNewGain = bNew;
}

bool CDnItem::IsQuickSlotItem()
{
	switch (GetItemType())
	{
	case ITEMTYPE_NORMAL:
	case ITEMTYPE_TRANSFORMPOTION:
	case ITEMTYPE_HEAD_SCALE_POTION:
	case ITEMTYPE_COOKING:
		break;
	case ITEMTYPE_RETURN_HOME:
		return true;
	case ITEMTYPE_GLOBAL_PARTY_BUFF:
	{
		if (GetItemRank() == ITEMRANK_SSS)
			return false;
	}
	break;
	default:
		return false;
	}
	if (GetSkillID() <= 0)
	{
		return false;
	}

	return true;
}

CDnItem &CDnItem::operator = (TItem &e)
{
	SetSerialID(e.nSerial);
	m_nOverlapCount = e.wCount;
	m_fElapsedDelayTime = e.nCoolTime / 1000.f;
	//	m_dwColor = e.dwColor;
	//m_bAuthentication = e.bSoulbound;
	// m_bIdentity = e.bIdentify;
	m_cEnchantLevel = e.cLevel;
	m_cPotentialIndex = e.cPotential;
	m_cOptionIndex = e.cOption;
	m_bSoulBound = e.bSoulbound;
	m_cSealCount = e.cSealCount;
	SetLookItemID(e.nLookItemID, true);
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	m_cPotentialMoveCount = e.cPotentialMoveCount;
#endif

	return *this;
}

void CDnItem::InitializeEnchant()
{
	if (m_nEnchantID == 0) return;

	SAFE_DELETE(m_pEnchant);

#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TENCHANT_MAIN);
	DNTableFileFormat*  pEnchantNeedItemSox = GetDNTable(CDnTableDB::TENCHANT_NEEDITEM);
	DNTableFileFormat*  pEnchantStateSox = GetDNTable(CDnTableDB::TENCHANT_STATE);
	DNTableFileFormat*  pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	if (!pSox || !pPotentialSox || !pEnchantNeedItemSox || !pEnchantStateSox)
		return;
#else
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TENCHANT);
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	if (!pSox) return;
#else // PRE_ADD_REDUCE_TABLE_RESOURCE
	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	if (!pSox || !pPotentialSox) return;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
#endif

	// MaxEnchantLevel은 미리 이렇게 구한다.
	std::vector<int> nVecItemID;
	pSox->GetItemIDListFromField("_EnchantID", m_nEnchantID, nVecItemID);
	for (DWORD i = 0; i<nVecItemID.size(); i++) {
#ifdef PRE_FIX_MEMOPT_ENCHANT
		int curEnchantLevel = pSox->GetFieldFromLablePtr(nVecItemID[i], "_EnchantLevel")->GetInteger();
		int curRatioTableID = pSox->GetFieldFromLablePtr(nVecItemID[i], "_NeedItemTableID")->GetInteger();
		float curRatio = pEnchantNeedItemSox->GetFieldFromLablePtr(curRatioTableID, "_EnchantRatio")->GetFloat();

		if (m_cMaxEnchantLevel < curEnchantLevel && curRatio > 0.f)
			m_cMaxEnchantLevel = curEnchantLevel;
#else
		if (m_cMaxEnchantLevel < pSox->GetFieldFromLablePtr(nVecItemID[i], "_EnchantLevel")->GetInteger() && pSox->GetFieldFromLablePtr(nVecItemID[i], "_EnchantRatio")->GetFloat() > 0.0f)
			m_cMaxEnchantLevel = pSox->GetFieldFromLablePtr(nVecItemID[i], "_EnchantLevel")->GetInteger();
#endif
	}

	// 인첸트 셋팅
	if (m_cEnchantLevel == 0) return;

	int nResultItemID = 0;
	for (DWORD i = 0; i<nVecItemID.size(); i++) {
		if (pSox->GetFieldFromLablePtr(nVecItemID[i], "_EnchantLevel")->GetInteger() == m_cEnchantLevel) {
			nResultItemID = nVecItemID[i];
			break;
		}
	}
	if (nResultItemID == 0) return;

	m_nEnchantTableID = nResultItemID;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	int enchantStateTableID = pSox->GetFieldFromLablePtr(nResultItemID, "_StateID")->GetInteger();
	int resultNeedItemTableID = pSox->GetFieldFromLablePtr(nResultItemID, "_NeedItemTableID")->GetInteger();
#endif
	char szLabel[32];
	m_pEnchant = new CDnState;
	for (int i = 0; i<10; i++) {
#ifdef PRE_FIX_MEMOPT_ENCHANT
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pEnchantStateSox->GetFieldFromLablePtr(enchantStateTableID, szLabel)->GetInteger();
		if (nStateType == -1) break;

		sprintf_s(szLabel, "_State%dValue", i + 1);
		char *szValue = pEnchantStateSox->GetFieldFromLablePtr(enchantStateTableID, szLabel)->GetString();

		int nValue = atoi(szValue);
		float stateRatio = pEnchantNeedItemSox->GetFieldFromLablePtr(resultNeedItemTableID, "_UpStateRatio")->GetFloat();
		int resultStateValue = int(nValue * stateRatio);

		CalcStateValue(m_pEnchant, nStateType, (float)resultStateValue, (float)resultStateValue, resultStateValue, resultStateValue, false);
#else
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetInteger();
		if (nStateType == -1) break;

		sprintf_s(szLabel, "_State%dValue", i + 1);
		char *szValue = pSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetString();

		CalcStateValue(m_pEnchant, nStateType, szValue, szValue, false);
#endif
	}
#if defined( PRE_FIX_69709 )
	m_nEnchantDisjointDrop = pSox->GetFieldFromLablePtr(m_nEnchantTableID, "_DisjointDrop")->GetInteger();
#endif // #if defined( PRE_FIX_69709 )
	m_pEnchant->CalcValueType();
}

int CDnItem::GetOptionTableID(int nItemTableID, char cOptionIndex)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return 0;
	if (!pSox->IsExistItem(nItemTableID)) return 0;

	int nTypeParam = pSox->GetFieldFromLablePtr(nItemTableID, "_TypeParam1")->GetInteger();
	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	std::vector<int> nVecItemID;
	pPotentialSox->GetItemIDListFromField("_PotentialID", nTypeParam, nVecItemID);

	if (cOptionIndex > (int)nVecItemID.size()) return 0;
	return nVecItemID[cOptionIndex - 1];
}

void CDnItem::InitializeOption()
{
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS && m_ItemType != ITEMTYPE_GLYPH && m_ItemType != ITEMTYPE_TALISMAN) return;
#else
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS && m_ItemType != ITEMTYPE_GLYPH) return;
#endif

#ifdef PRE_MOD_GACHA_SYSTEM
	DNTableFileFormat* pItemSox = GetDNTable(CDnTableDB::TITEM);
	if (!pItemSox)
		return;

	int nTypeParam3 = GetItemTypeParam(m_nClassID, 2);
	//	note by kalliste : 2013 가차 개편으로 _TypeParam3에도 옵션 값이 설정되게 되어 두개의 값이 겹치는 일이 없게끔 셋팅되도록 논의되었다.
	if (m_nTypeParam[0] > 0 && nTypeParam3 > 0)
	{
		_ASSERT(0);
		return;
	}

	int nCurrentTypeParam = 0;
	if (nTypeParam3 > 0)
		nCurrentTypeParam = nTypeParam3;
	else if (m_nTypeParam[0] > 0)
		nCurrentTypeParam = m_nTypeParam[0];
	else
		return;
#else
	if (m_nTypeParam[0] < 1) return;
#endif
	if (m_cOptionIndex <= 0) return;

	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	std::vector<int> nVecItemID;
#ifdef PRE_MOD_GACHA_SYSTEM
	pPotentialSox->GetItemIDListFromField("_PotentialID", nCurrentTypeParam, nVecItemID);
#else
	pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[0], nVecItemID);
#endif

	if (m_cOptionIndex > (int)nVecItemID.size()) return;

	int nResultItemID = nVecItemID[m_cOptionIndex - 1];
	if (nResultItemID == 0) return;


	char szLabel[32];
	CDnState OptionState;
	for (int i = 0; i<16; i++) {
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetInteger();
		if (nStateType == -1) break;

		sprintf_s(szLabel, "_State%dValue", i + 1);
		char *szValue = pPotentialSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetString();

		CalcStateValue(&OptionState, nStateType, szValue, szValue, false);
	}
	OptionState.CalcValueType();
	*this += OptionState;
}

void CDnItem::InitializePotential()
{
	std::vector<int> nVecItemID;
	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);

#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	if (m_cOptionIndex > 0)
	{
		nVecItemID.clear();
		pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[0], nVecItemID);

		if (m_cOptionIndex <= (int)nVecItemID.size())
		{
			int nResultItemID = nVecItemID[m_cOptionIndex - 1];

			m_nOptionSkillID = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillID")->GetInteger();
			m_nOptionSkillLevel = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillLevel")->GetInteger();
			m_nOptionSkillUsingType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillUsingType")->GetInteger();
		}
	}
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE

	SAFE_DELETE(m_pPotential);
	// 포텐셜 셋팅
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS && m_ItemType != ITEMTYPE_GLYPH) return;
	if (m_nTypeParam[1] < 1) return;
	if (m_cPotentialIndex <= 0) return;

	pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[1], nVecItemID);

	if (m_cPotentialIndex > (int)nVecItemID.size()) return;
	int nResultItemID = nVecItemID[m_cPotentialIndex - 1];
	m_pPotential = new CDnState;
	char szLabel[32];

	for (int i = 0; i<16; i++) {
		sprintf_s(szLabel, "_State%d", i + 1);
		int nStateType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetInteger();
		if (nStateType == -1) break;

		sprintf_s(szLabel, "_State%dValue", i + 1);
		char *szValue = pPotentialSox->GetFieldFromLablePtr(nResultItemID, szLabel)->GetString();

		CalcStateValue(m_pPotential, nStateType, szValue, szValue, false);
	}

	m_pPotential->CalcValueType();


}

void CDnItem::CalcStateValue(CDnState *pState, int nStateType, char *szMin, char *szMax, bool bRandomValue, bool bResetValue, float fRatio)
{
	int nMin, nMax;
	float fMin, fMax;
	nMin = (int)((float)atof(szMin) * fRatio);
	nMax = (int)((float)atof(szMax) * fRatio);
	fMin = (float)atof(szMin) * fRatio;
	fMax = (float)atof(szMax) * fRatio;

#ifdef PRE_FIX_ATOF_ROUNDOFF
	fMax += 0.0000001f;
	fMin += 0.0000001f;
#endif // #ifdef PRE_FIX_ATOF_ROUNDOFF

#ifdef PRE_FIX_MEMOPT_ENCHANT
	CalcStateValue(pState, nStateType, fMin, fMax, nMin, nMax, bRandomValue, bResetValue);
#else
	switch (nStateType) {
		// Absolute
	case 0: pState->SetStrength(((bResetValue) ? 0 : pState->GetStrength()) + (+((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin))); break;
	case 1: pState->SetAgility(((bResetValue) ? 0 : pState->GetAgility()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 2: pState->SetIntelligence(((bResetValue) ? 0 : pState->GetIntelligence()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 3: pState->SetStamina(((bResetValue) ? 0 : pState->GetStamina()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 4: pState->SetAttackPMin(((bResetValue) ? 0 : pState->GetAttackPMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 5: pState->SetAttackPMax(((bResetValue) ? 0 : pState->GetAttackPMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 6: pState->SetAttackMMin(((bResetValue) ? 0 : pState->GetAttackMMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 7: pState->SetAttackMMax(((bResetValue) ? 0 : pState->GetAttackMMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 8: pState->SetDefenseP(((bResetValue) ? 0 : pState->GetDefenseP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 9: pState->SetDefenseM(((bResetValue) ? 0 : pState->GetDefenseM()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 10: pState->SetStiff(((bResetValue) ? 0 : pState->GetStiff()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 11: pState->SetStiffResistance(((bResetValue) ? 0 : pState->GetStiffResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 12: pState->SetCritical(((bResetValue) ? 0 : pState->GetCritical()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 13: pState->SetCriticalResistance(((bResetValue) ? 0 : pState->GetCriticalResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 14: pState->SetStun(((bResetValue) ? 0 : pState->GetStun()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 15: pState->SetStunResistance(((bResetValue) ? 0 : pState->GetStunResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 16: pState->SetElementAttack(ElementEnum::Fire, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Fire)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 17: pState->SetElementAttack(ElementEnum::Ice, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Ice)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 18: pState->SetElementAttack(ElementEnum::Light, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Light)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 19: pState->SetElementAttack(ElementEnum::Dark, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Dark)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 20: pState->SetElementDefense(ElementEnum::Fire, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Fire)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 21: pState->SetElementDefense(ElementEnum::Ice, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Ice)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 22: pState->SetElementDefense(ElementEnum::Light, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Light)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 23: pState->SetElementDefense(ElementEnum::Dark, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Dark)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 24: pState->SetMoveSpeed(((bResetValue) ? 0 : pState->GetMoveSpeed()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 25: pState->SetMaxHP(((bResetValue) ? 0 : pState->GetMaxHP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 26: pState->SetMaxSP(((bResetValue) ? 0 : pState->GetMaxSP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 27: pState->SetRecoverySP(((bResetValue) ? 0 : pState->GetRecoverySP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 28: pState->SetSuperAmmor(((bResetValue) ? 0 : pState->GetSuperAmmor()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 29: pState->SetFinalDamage(((bResetValue) ? 0 : pState->GetFinalDamage()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 30: pState->SetSafeZoneMoveSpeed(((bResetValue) ? 0 : pState->GetSafeZoneMoveSpeed()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 31: pState->SetAddExp(((bResetValue) ? 0 : pState->GetAddExp()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
		//Case 32 increase P attack min,max
		//Case 33 increase M attack min,max
	case 32:
	{
		pState->SetAttackPMin(((bResetValue) ? 0 : pState->GetAttackPMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin));
		pState->SetAttackPMax(((bResetValue) ? 0 : pState->GetAttackPMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin));
	}
	break;
	case 33:
	{
		pState->SetAttackMMin(((bResetValue) ? 0 : pState->GetAttackMMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin));
		pState->SetAttackMMax(((bResetValue) ? 0 : pState->GetAttackMMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin));
	}
	break;

	// Ratio
	case 50: pState->SetStrengthRatio(((bResetValue) ? 0 : pState->GetStrengthRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 51: pState->SetAgilityRatio(((bResetValue) ? 0 : pState->GetAgilityRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 52: pState->SetIntelligenceRatio(((bResetValue) ? 0 : pState->GetIntelligenceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 53: pState->SetStaminaRatio(((bResetValue) ? 0 : pState->GetStaminaRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 54: pState->SetAttackPMinRatio(((bResetValue) ? 0 : pState->GetAttackPMinRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 55: pState->SetAttackPMaxRatio(((bResetValue) ? 0 : pState->GetAttackPMaxRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 56: pState->SetAttackMMinRatio(((bResetValue) ? 0 : pState->GetAttackMMinRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 57: pState->SetAttackMMaxRatio(((bResetValue) ? 0 : pState->GetAttackMMaxRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 58: pState->SetDefensePRatio(((bResetValue) ? 0 : pState->GetDefensePRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 59: pState->SetDefenseMRatio(((bResetValue) ? 0 : pState->GetDefenseMRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 60: pState->SetStiffRatio(((bResetValue) ? 0 : pState->GetStiffRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 61: pState->SetStiffResistanceRatio(((bResetValue) ? 0 : pState->GetStiffResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 62: pState->SetCriticalRatio(((bResetValue) ? 0 : pState->GetCriticalRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 63: pState->SetCriticalResistanceRatio(((bResetValue) ? 0 : pState->GetCriticalResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 64: pState->SetStunRatio(((bResetValue) ? 0 : pState->GetStunRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 65: pState->SetStunResistanceRatio(((bResetValue) ? 0 : pState->GetStunResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
		/*
		case 66: m_fElementAttackRatio[ElementEnum::Fire] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 67: m_fElementAttackRatio[ElementEnum::Ice] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 68: m_fElementAttackRatio[ElementEnum::Light] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 69: m_fElementAttackRatio[ElementEnum::Dark] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 70: m_fElementDefenseRatio[ElementEnum::Fire] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 71: m_fElementDefenseRatio[ElementEnum::Ice] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 72: m_fElementDefenseRatio[ElementEnum::Light] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 73: m_fElementDefenseRatio[ElementEnum::Dark] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		*/
	case 74: pState->SetMoveSpeedRatio(((bResetValue) ? 0 : pState->GetMoveSpeedRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 75: pState->SetMaxHPRatio(((bResetValue) ? 0 : pState->GetMaxHPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 76: pState->SetMaxSPRatio(((bResetValue) ? 0 : pState->GetMaxSPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 77: pState->SetRecoverySPRatio(((bResetValue) ? 0 : pState->GetRecoverySPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 78: pState->SetSuperAmmorRatio(((bResetValue) ? 0 : pState->GetSuperAmmorRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 79: pState->SetFinalDamageRatio(((bResetValue) ? 0 : pState->GetFinalDamageRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 81: pState->SetSafeZoneMoveSpeedRatio(((bResetValue) ? 0 : pState->GetSafeZoneMoveSpeedRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;

	case 100:pState->SetSpirit(((bResetValue) ? 0 : pState->GetSpirit()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;


	default:
		if (bRandomValue) DummyRandom();
		break;
	}
#endif // PRE_FIX_MEMOPT_ENCHANT
}

#ifdef PRE_FIX_MEMOPT_ENCHANT
void CDnItem::CalcStateValue(CDnState *pState, int nStateType, float fMin, float fMax, int nMin, int nMax, bool bRandomValue, bool bResetValue)
{
	switch (nStateType) {
		// Absolute
	case 0: pState->SetStrength(((bResetValue) ? 0 : pState->GetStrength()) + (+((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin))); break;
	case 1: pState->SetAgility(((bResetValue) ? 0 : pState->GetAgility()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 2: pState->SetIntelligence(((bResetValue) ? 0 : pState->GetIntelligence()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 3: pState->SetStamina(((bResetValue) ? 0 : pState->GetStamina()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 4: pState->SetAttackPMin(((bResetValue) ? 0 : pState->GetAttackPMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 5: pState->SetAttackPMax(((bResetValue) ? 0 : pState->GetAttackPMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 6: pState->SetAttackMMin(((bResetValue) ? 0 : pState->GetAttackMMin()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 7: pState->SetAttackMMax(((bResetValue) ? 0 : pState->GetAttackMMax()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 8: pState->SetDefenseP(((bResetValue) ? 0 : pState->GetDefenseP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 9: pState->SetDefenseM(((bResetValue) ? 0 : pState->GetDefenseM()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 10: pState->SetStiff(((bResetValue) ? 0 : pState->GetStiff()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 11: pState->SetStiffResistance(((bResetValue) ? 0 : pState->GetStiffResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 12: pState->SetCritical(((bResetValue) ? 0 : pState->GetCritical()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 13: pState->SetCriticalResistance(((bResetValue) ? 0 : pState->GetCriticalResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 14: pState->SetStun(((bResetValue) ? 0 : pState->GetStun()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 15: pState->SetStunResistance(((bResetValue) ? 0 : pState->GetStunResistance()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 16: pState->SetElementAttack(ElementEnum::Fire, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Fire)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 17: pState->SetElementAttack(ElementEnum::Ice, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Ice)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 18: pState->SetElementAttack(ElementEnum::Light, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Light)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 19: pState->SetElementAttack(ElementEnum::Dark, ((bResetValue) ? 0 : pState->GetElementAttack(ElementEnum::Dark)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 20: pState->SetElementDefense(ElementEnum::Fire, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Fire)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 21: pState->SetElementDefense(ElementEnum::Ice, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Ice)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 22: pState->SetElementDefense(ElementEnum::Light, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Light)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 23: pState->SetElementDefense(ElementEnum::Dark, ((bResetValue) ? 0 : pState->GetElementDefense(ElementEnum::Dark)) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 24: pState->SetMoveSpeed(((bResetValue) ? 0 : pState->GetMoveSpeed()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 25: pState->SetMaxHP(((bResetValue) ? 0 : pState->GetMaxHP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 26: pState->SetMaxSP(((bResetValue) ? 0 : pState->GetMaxSP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 27: pState->SetRecoverySP(((bResetValue) ? 0 : pState->GetRecoverySP()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 28: pState->SetSuperAmmor(((bResetValue) ? 0 : pState->GetSuperAmmor()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 29: pState->SetFinalDamage(((bResetValue) ? 0 : pState->GetFinalDamage()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 30: pState->SetSafeZoneMoveSpeed(((bResetValue) ? 0 : pState->GetSafeZoneMoveSpeed()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;
	case 31: pState->SetAddExp(((bResetValue) ? 0 : pState->GetAddExp()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;

		// Ratio
	case 50: pState->SetStrengthRatio(((bResetValue) ? 0 : pState->GetStrengthRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 51: pState->SetAgilityRatio(((bResetValue) ? 0 : pState->GetAgilityRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 52: pState->SetIntelligenceRatio(((bResetValue) ? 0 : pState->GetIntelligenceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 53: pState->SetStaminaRatio(((bResetValue) ? 0 : pState->GetStaminaRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 54: pState->SetAttackPMinRatio(((bResetValue) ? 0 : pState->GetAttackPMinRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 55: pState->SetAttackPMaxRatio(((bResetValue) ? 0 : pState->GetAttackPMaxRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 56: pState->SetAttackMMinRatio(((bResetValue) ? 0 : pState->GetAttackMMinRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 57: pState->SetAttackMMaxRatio(((bResetValue) ? 0 : pState->GetAttackMMaxRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 58: pState->SetDefensePRatio(((bResetValue) ? 0 : pState->GetDefensePRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 59: pState->SetDefenseMRatio(((bResetValue) ? 0 : pState->GetDefenseMRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 60: pState->SetStiffRatio(((bResetValue) ? 0 : pState->GetStiffRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 61: pState->SetStiffResistanceRatio(((bResetValue) ? 0 : pState->GetStiffResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 62: pState->SetCriticalRatio(((bResetValue) ? 0 : pState->GetCriticalRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 63: pState->SetCriticalResistanceRatio(((bResetValue) ? 0 : pState->GetCriticalResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 64: pState->SetStunRatio(((bResetValue) ? 0 : pState->GetStunRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 65: pState->SetStunResistanceRatio(((bResetValue) ? 0 : pState->GetStunResistanceRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
		/*
		case 66: m_fElementAttackRatio[ElementEnum::Fire] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 67: m_fElementAttackRatio[ElementEnum::Ice] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 68: m_fElementAttackRatio[ElementEnum::Light] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 69: m_fElementAttackRatio[ElementEnum::Dark] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 70: m_fElementDefenseRatio[ElementEnum::Fire] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 71: m_fElementDefenseRatio[ElementEnum::Ice] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 72: m_fElementDefenseRatio[ElementEnum::Light] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		case 73: m_fElementDefenseRatio[ElementEnum::Dark] = GetRandomStateRatio( fMin, fMax ) : fMin; break;
		*/
	case 74: pState->SetMoveSpeedRatio(((bResetValue) ? 0 : pState->GetMoveSpeedRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 75: pState->SetMaxHPRatio(((bResetValue) ? 0 : pState->GetMaxHPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 76: pState->SetMaxSPRatio(((bResetValue) ? 0 : pState->GetMaxSPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 77: pState->SetRecoverySPRatio(((bResetValue) ? 0 : pState->GetRecoverySPRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 78: pState->SetSuperAmmorRatio(((bResetValue) ? 0 : pState->GetSuperAmmorRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 79: pState->SetFinalDamageRatio(((bResetValue) ? 0 : pState->GetFinalDamageRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;
	case 81: pState->SetSafeZoneMoveSpeedRatio(((bResetValue) ? 0 : pState->GetSafeZoneMoveSpeedRatio()) + ((bRandomValue) ? GetRandomStateRatio(fMin, fMax) : fMin)); break;

	case 100:pState->SetSpirit(((bResetValue) ? 0 : pState->GetSpirit()) + ((bRandomValue) ? GetRandomStateNumber(nMin, nMax) : nMin)); break;

	default:
		if (bRandomValue) DummyRandom();
		break;
	}
}
#endif

void CDnItem::GetTItemInfo(TItemInfo &Info)
{
	// 이건 다른 창에 등록하는 복사용아이템이긴 하지만, cSlotIndex 역시 필요하다.
	// 패킷요청시 이 복사용아이템의 정보를 사용할때도 있으니..
	Info.cSlotIndex = m_nSlotIndex;
	Info.Item.nItemID = m_nClassID;
	Info.Item.wCount = m_nOverlapCount;
	Info.Item.cLevel = m_cEnchantLevel;
	Info.Item.cPotential = m_cPotentialIndex;
	Info.Item.cOption = m_cOptionIndex;
	Info.Item.cSealCount = m_cSealCount;
	Info.Item.bSoulbound = (m_Reversion == ITEMREVERSION_BELONG) ? m_bSoulBound : true;
	Info.Item.nSerial = m_nSerialID;
	Info.Item.bEternity = m_bEternity;
	Info.Item.nLookItemID = m_nLookItemID;
	Info.Item.tExpireDate = m_ExpireDate;
	// 내구도 재설정. 위에서는 맥스치를 알 수 없으니 여기서 이렇게 맥스치로 설정한다.
	if (m_ItemType == ITEMTYPE_WEAPON)
	{
		CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(this);
		if (pWeapon)
			Info.Item.wDur = pWeapon->GetDurability();
	}
	else if (m_ItemType == ITEMTYPE_PARTS)
	{
		CDnParts *pParts = dynamic_cast<CDnParts *>(this);
		if (pParts)
			Info.Item.wDur = pParts->GetDurability();
	}
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	Info.Item.cPotentialMoveCount = m_cPotentialMoveCount;
#endif
}

//void CDnItem::GenerationDropItem( void )
//{
//	for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
//		CDnDropItem::CalcDropItemList( m_nDisjointDropTableID[ 2 ], m_VecDropItemList );
//
//	for( DWORD i = 0; i < m_VecDropItemList.size(); ++i )
//	{
//		if( CDnDropItem::PreInitializeItem( m_VecDropItemList.at(i).nItemID ) == false )
//		{
//			m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
//			--i;
//
//			OutputDebug( "[Error] CDnItem::GenerationDropItem Failed!! ItemID: %d\n", m_nClassID );
//		}
//	}
//}
//
//
//bool CDnItem::Disjoint( void )
//{
//	bool bResult = true;
//
//	// 아이템 분해기 프랍의 정확한 위치가 필요할 듯. 
//	// 일단 지금은 자신의 플레이어의 위치로 하면 될까나.
//	EtVector3 vPos = *CDnActor::s_hLocalActor->GetPosition();
//	
//	for( DWORD i = 0; i < m_VecDropItemList.size(); ++i )
//	{
//		int nRotate =  (int)( ( ( m_VecDropItemList[i].nSeed % 360 ) / (float)m_VecDropItemList.size() ) * i );
//		CDnDropItem::DropItem( vPos, m_VecDropItemList[i].dwUniqueID, m_VecDropItemList[i].nItemID, 
//									 m_VecDropItemList[i].nSeed, m_VecDropItemList[i].nCount, nRotate );
//	}
//
//	SAFE_DELETE_VEC( m_VecDropItemList );
//
//	return bResult;
//}

int CDnItem::GetTotalSealCount(int nSealItemTypeParam)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TSEALCOUNT);
	if (!pSox)
	{
		_ASSERT(0);
		return 0;
	}

	const BYTE SEALENCHANTMAX = 16;
	for (int i = 0; i < (int)pSox->GetItemCount(); i++)
	{
		int nItemID = pSox->GetItemID(i);
		int nType1 = pSox->GetFieldFromLablePtr(nItemID, "_Type1")->GetInteger();
		if (nType1 != nSealItemTypeParam) continue;

		int nType2 = pSox->GetFieldFromLablePtr(nItemID, "_Type2")->GetInteger();
		if (nType2 != m_nSealID) continue;

		if (SEALENCHANTMAX <= m_cEnchantLevel) continue;

		char szLabel[32];
		sprintf_s(szLabel, _countof(szLabel), "_Count%d", m_cEnchantLevel);
		return pSox->GetFieldFromLablePtr(nItemID, szLabel)->GetInteger();
	}

	return 0;
}

bool CDnItem::GetCharmItemKeyID(std::vector<int> &vecKeyID)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TCHARMITEMKEY);
	if (!pSox)
	{
		_ASSERT(0);
		return false;
	}

	const BYTE CHARMKEYMAX = 5;
	for (int i = 1; i <= (int)pSox->GetItemCount(); ++i)
	{
		int nItemID = pSox->GetFieldFromLablePtr(i, "_CharmItemID")->GetInteger();
		if (nItemID == 0) continue;
		if (m_nClassID != nItemID) continue;

		// 해당되는 CharmItemID를 찾으면,
		int nKeyID = 0;
		char szLabel[32];
		for (int j = 1; j <= CHARMKEYMAX; ++j)
		{
			sprintf_s(szLabel, _countof(szLabel), "_Key%d", j);
			nKeyID = pSox->GetFieldFromLablePtr(i, szLabel)->GetInteger();
			if (nKeyID)
			{
				DNTableFileFormat* pItemSox = GetDNTable(CDnTableDB::TITEM);
				if (pItemSox && pItemSox->IsExistItem(nKeyID))
					vecKeyID.push_back(nKeyID);
			}
		}
		return true;
	}
	return false;
}

bool CDnItem::MakeItemInfo(int nItemID, int nCount, TItem &ItemInfo)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return false;
	if (!pSox->IsExistItem(nItemID)) return false;

	memset(&ItemInfo, 0, sizeof(TItem));

	ItemInfo.nItemID = nItemID;
	ItemInfo.wCount = (short)nCount;
	// 인벤리스트 들어올때보니 일반템은 false로 귀속템은 귀속일 경우 true로 들어온다.
	int nReversionType = pSox->GetFieldFromLablePtr(nItemID, "_Reversion")->GetInteger();
	ItemInfo.bSoulbound = (nReversionType == NoReversion) ? false : true;
	if (nReversionType == Trade)
		ItemInfo.cSealCount = pSox->GetFieldFromLablePtr(nItemID, "_CashTradeCount")->GetInteger();
	else
		ItemInfo.cSealCount = pSox->GetFieldFromLablePtr(nItemID, "_SealCount")->GetInteger();

	eItemTypeEnum Type = (eItemTypeEnum)pSox->GetFieldFromLablePtr(nItemID, "_Type")->GetInteger();
	switch (Type) {
	case ITEMTYPE_WEAPON:
	{
		DNTableFileFormat* pWeaponSox = GetDNTable(CDnTableDB::TWEAPON);
		if (!pWeaponSox->IsExistItem(nItemID)) return false;
		ItemInfo.wDur = pWeaponSox->GetFieldFromLablePtr(nItemID, "_Durability")->GetInteger();
	}
	break;
	case ITEMTYPE_PARTS:
	{
		DNTableFileFormat* pPartsSox = GetDNTable(CDnTableDB::TPARTS);
		if (!pPartsSox->IsExistItem(nItemID)) return false;
		ItemInfo.wDur = pPartsSox->GetFieldFromLablePtr(nItemID, "_Durability")->GetInteger();
	}
	break;
	}
	return true;
}

bool CDnItem::MakeItemInfo(int nItemID, int nCount, TItemInfo &ItemInfo)
{
	memset(&ItemInfo, 0, sizeof(TItemInfo));

	return MakeItemInfo(nItemID, nCount, ItemInfo.Item);
}

void CDnItem::MakeItemDurationString(int iDurationTimeByMS, /*IN OUT*/ wstring &strDurationTime)
{
	int iSecond = 1000;
	int iMinute = iSecond * 60;
	int iHour = iMinute * 60;
	int iDay = iHour * 24;

	// 24시간으로 나누어 일자 표시
	int iDurationDays = iDurationTimeByMS / iDay;

	// 시간으로 나누어 시간 표시
	int iDurationHours = (iDurationTimeByMS / iHour) % 24;

	// 분으로 나누어 분 표시
	int iDurationMinutes = (iDurationTimeByMS / iMinute) % 60;

	// 초로 나누어 초 표시
	int iDurationSeconds = (iDurationTimeByMS / iSecond) % 60;

	WCHAR wcBuffer[32] = { 0 };
	if (0 < iDurationDays)
	{
		_itow_s(iDurationDays, wcBuffer, sizeof(wcBuffer) / sizeof(WCHAR), 10);
		strDurationTime.append(wcBuffer);
		strDurationTime.append(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5154));
		strDurationTime.append(L" ");
	}

	if (0 < iDurationHours)
	{
		_itow_s(iDurationHours, wcBuffer, sizeof(wcBuffer) / sizeof(WCHAR), 10);
		strDurationTime.append(wcBuffer);
		strDurationTime.append(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5155));
		strDurationTime.append(L" ");
	}

	if (0 < iDurationMinutes)
	{
		_itow_s(iDurationMinutes, wcBuffer, sizeof(wcBuffer) / sizeof(WCHAR), 10);
		strDurationTime.append(wcBuffer);
		strDurationTime.append(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5156));
		strDurationTime.append(L" ");

	}

	if (0 < iDurationSeconds)
	{
		_itow_s(iDurationSeconds, wcBuffer, sizeof(wcBuffer) / sizeof(WCHAR), 10);
		strDurationTime.append(wcBuffer);
		strDurationTime.append(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5157));
		strDurationTime.append(L" ");
	}
}

void CDnItem::InsertSerialSearchMap(CDnItem *pItem)
{
	ScopeLock<CSyncLock> Lock(s_SerialSearchLock);
	if (pItem->GetSerialID() == -1) return;

	s_nMapSerialSearch[pItem->GetSerialID()] = pItem;
}

void CDnItem::RemoveSerialSearchMap(CDnItem *pItem)
{
	ScopeLock<CSyncLock> Lock(s_SerialSearchLock);
	if (pItem->GetSerialID() == -1) return;

	std::map<INT64, CDnItem *>::iterator it = s_nMapSerialSearch.find(pItem->GetSerialID());
	if (it != s_nMapSerialSearch.end()) s_nMapSerialSearch.erase(it);
}

void CDnItem::SetSerialID(INT64 nSerial)
{
	RemoveSerialSearchMap(this);
	m_nSerialID = nSerial;
	InsertSerialSearchMap(this);
}

CDnItem *CDnItem::FindItemFromSerialID(INT64 nSerialID)
{
	ScopeLock<CSyncLock> Lock(s_SerialSearchLock);
	std::map<INT64, CDnItem *>::iterator it = s_nMapSerialSearch.find(nSerialID);
	if (it != s_nMapSerialSearch.end()) return it->second;
	return NULL;
}

bool CDnItem::CanDisjointType(int nDisjointType)
{
	if (!(m_nDisjointType & nDisjointType)) return false;
	return true;
}

bool CDnItem::CanDisjoint(void)
{
	if (m_nDisjointType == 0) return false;
#if defined( PRE_FIX_69709)
	if (m_nDisjointDropTableID[0] == 0 || ((m_cEnchantLevel > 0) && (m_nEnchantDisjointDrop == 0)))
		return false;
#endif //#if defined( PRE_FIX_69709)
	return m_bCanDisjoint;
}

bool CDnItem::IsSoulbBound() const
{
	if (m_Reversion == ITEMREVERSION_BELONG)
		return m_bSoulBound;

	return true;
}

bool CDnItem::IsCashItem(int nTableID)
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return false;
	if (pSox->IsExistItem(nTableID) == false) return false;

	return (pSox->GetFieldFromLablePtr(nTableID, "_IsCash")->GetInteger()) ? true : false;
}

bool CDnItem::IsEternityItem() const
{
	return m_bEternity;
}

void CDnItem::SetEternityItem(bool bEternity)
{
	m_bEternity = bEternity;
}

const time_t* CDnItem::GetExpireDate() const
{
#ifdef PRE_ADD_CASHREMOVE
	if (m_bEternity && false == m_bIsCashRemoveItem)
#else
	if (m_bEternity)
#endif	// #ifdef PRE_ADD_CASHREMOVE
		return NULL;

	return &m_ExpireDate;
};

void CDnItem::SetExpireDate(const time_t& date)
{
	m_ExpireDate = date;
}

int CDnItem::GetClickedEquipPage()
{
	switch (GetItemType()) {
	case ITEMTYPE_GLYPH:	return CDnCharStatusDlg::EquipPagePlate;
#ifdef PRE_ADD_TALISMAN_SYSTEM
	case ITEMTYPE_TALISMAN: return CDnCharStatusDlg::EquipPageTalisman;
#endif
	case ITEMTYPE_VEHICLEEFFECT:
	case ITEMTYPE_VEHICLE:
	case ITEMTYPE_VEHICLEPARTS:
	case ITEMTYPE_VEHICLEHAIRCOLOR:
	case ITEMTYPE_VEHICLE_SHARE:
		return CDnCharStatusDlg::EquipPageVehicle;

	case ITEMTYPE_PET:
	case ITEMTYPE_PETPARTS:
	case ITEMTYPE_PET_FOOD:
		return CDnCharStatusDlg::EquipPagePet;

	default:
		if (IsCashItem()) return CDnCharStatusDlg::EquipPageCash;
		else return CDnCharStatusDlg::EquipPageNormal;
		break;
	}
	return CDnCharStatusDlg::EquipPageNormal;
}

void CDnItem::SetInfoItem(bool bInfoItem)
{
	m_bInfoItem = bInfoItem;
}

bool CDnItem::HasPrefixSkill()
{
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	if (CDnItem::ItemSkillApplyType::PrefixSkill != m_nOptionSkillUsingType)
		return false;

	return true;
#else // PRE_ADD_REDUCE_TABLE_RESOURCE
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS) return false;
	if (m_nTypeParam[0] < 1) return false;
	if (m_cOptionIndex <= 0) return false;

	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	std::vector<int> nVecItemID;
	pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[0], nVecItemID);

	if (m_cOptionIndex > (int)nVecItemID.size()) return false;
	int nResultItemID = nVecItemID[m_cOptionIndex - 1];

	//스킬 레벨업 아이템 때문에 접두사 스킬 등록에 문제가 발생...
	int nSkillUsingType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillUsingType")->GetInteger();

	if (CDnItem::ItemSkillApplyType::PrefixSkill != nSkillUsingType)
		return false;

	return true;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
}

bool CDnItem::HasPrefixSkill(int &nSkillID, int &nSkillLevel)
{
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
	if (CDnItem::ItemSkillApplyType::PrefixSkill != m_nOptionSkillUsingType)
		return false;

	nSkillID = m_nOptionSkillID;
	nSkillLevel = 1; //접미사 스킬 레벨은 항상 1로 설정. 테이블의 SkillLevel값은 접미사의 등급 정보로 사용됨.
	nSkillLevel += m_cEnchantLevel;

	return true;
#else // PRE_ADD_REDUCE_TABLE_RESOURCE
	// 무기/방어구
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS) return false;
	//m_nTypeParam[1]은 잠재 능력.. m_nTypeParam[0] 값을 참조 한다..
	if (m_nTypeParam[0] < 1) return false;
	if (m_cOptionIndex <= 0) return false;

	DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
	std::vector<int> nVecItemID;
	pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[0], nVecItemID);

	if (m_cOptionIndex > (int)nVecItemID.size()) return false;
	int nResultItemID = nVecItemID[m_cOptionIndex - 1];

	//스킬 레벨업 아이템 때문에 접두사 스킬 등록에 문제가 발생...
	int nSkillUsingType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillUsingType")->GetInteger();

	if (CDnItem::ItemSkillApplyType::PrefixSkill != nSkillUsingType)
		return false;

	nSkillID = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillID")->GetInteger();

	nSkillLevel = 1; //접미사 스킬 레벨은 항상 1로 설정. 테이블의 SkillLevel값은 접미사의 등급 정보로 사용됨.

	nSkillLevel += m_cEnchantLevel;

	return true;
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
}

#ifdef PRE_ADD_DMIX_DESIGNNAME
void CDnItem::MakeLookItemDescription(DNTableFileFormat* pTableFormat)
{
	if (pTableFormat == NULL)
		return;

	m_szLookItemDescriptionCache.clear();
	const DNTableCell* pDescField = pTableFormat->GetFieldFromLablePtr(GetLookItemID(), "_DescriptionID");
	if (pDescField == NULL)
		return;

	int nNameID = pDescField->GetInteger();

	const DNTableCell* pDescIDField = pTableFormat->GetFieldFromLablePtr(GetLookItemID(), "_DescriptionIDParam");
	if (pDescIDField == NULL)
		return;

	char *szParam = pDescIDField->GetString();

	MakeUIStringUseVariableParam(m_szLookItemDescriptionCache, nNameID, szParam);
}
#endif // PRE_ADD_DMIX_DESIGNNAME

void CDnItem::SetLookItemID(int nLookItemID, bool bChangeIconIdx)
{
	if (nLookItemID == 0 ||
		(m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS))
	{
		m_nLookItemID = 0;
		return;
	}

	m_nLookItemID = nLookItemID;

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
	if (!pSox) return;

	const DNTableCell* pField = pSox->GetFieldFromLablePtr(m_nLookItemID, "_IconImageIndex");
	if (pField == NULL)
		return;

	SetIconImageIndex(pField->GetInteger());

#ifdef PRE_ADD_DMIX_DESIGNNAME
	MakeLookItemDescription(pSox);
#endif

}

bool CDnItem::HasLevelUpInfo(int &nSkillID, int &nSkillLevel, int &nSkillUsingType)
{
	//원래 아이템의 스킬레벨업 정보
	int nItemSkillLevelUpSkillID = m_nSkillID;
	int nItemSkillLevelUpSkillLevel = m_nSkillLevel;
	int nItemSkillLevelUpSkillUsingType = m_SkillApplyType;

	// 무기/방어구
	if (m_ItemType != ITEMTYPE_WEAPON && m_ItemType != ITEMTYPE_PARTS) return false;

	//옵션 정보를 가지고 있는지 확인한다.
	int nOptionSkillID = 0;
	int nOptionSkillLevel = 0;
	int nOptionSkillUsingType = 0;

	//m_nItemTypeParam[1]은 잠재 능력.. m_nItemTypeParam[0] 값을 참조 한다..
	//if( m_nTypeParam[0] < 1 ) return false;
	//if( m_cOptionIndex <= 0 ) return false;

	//옵션값이 설정이 되어 있으면 옵션 값을 확인한다.
	if (m_nTypeParam[0] > 0 && m_cOptionIndex > 0)
	{
#ifdef PRE_ADD_REDUCE_TABLE_RESOURCE
		nOptionSkillID = m_nOptionSkillID;
		nOptionSkillLevel = m_nOptionSkillLevel;
		nOptionSkillUsingType = m_nOptionSkillUsingType;
#else // PRE_ADD_REDUCE_TABLE_RESOURCE
		DNTableFileFormat* pPotentialSox = GetDNTable(CDnTableDB::TPOTENTIAL);
		std::vector<int> nVecItemID;
		pPotentialSox->GetItemIDListFromField("_PotentialID", m_nTypeParam[0], nVecItemID);

		if (m_cOptionIndex <= (int)nVecItemID.size())
		{
			int nResultItemID = nVecItemID[m_cOptionIndex - 1];

			nOptionSkillID = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillID")->GetInteger();
			nOptionSkillLevel = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillLevel")->GetInteger();
			nOptionSkillUsingType = pPotentialSox->GetFieldFromLablePtr(nResultItemID, "_SkillUsingType")->GetInteger();
		}
#endif // PRE_ADD_REDUCE_TABLE_RESOURCE
	}

	//옵션 정보가 스킬레벨업 정보이면 옵션 정보로 갱신한다.
	if (nOptionSkillUsingType == CDnItem::ItemSkillApplyType::SkillLevelUp)
	{
		nSkillID = nOptionSkillID;
		nSkillLevel = nOptionSkillLevel;
		nSkillUsingType = nOptionSkillUsingType;
	}
	else if (nItemSkillLevelUpSkillUsingType == CDnItem::ItemSkillApplyType::SkillLevelUp)
	{
		nSkillID = nItemSkillLevelUpSkillID;
		nSkillLevel = nItemSkillLevelUpSkillLevel;
		nSkillUsingType = nItemSkillLevelUpSkillUsingType;
	}

	return (nSkillUsingType == CDnItem::ItemSkillApplyType::SkillLevelUp);
}

int CDnItem::GetSkillLevelID(void)
{
	if (m_hSkill)
		return m_hSkill->GetSkillLevelID();

	return 0;
}

const TCHAR* CDnItem::GetSkillDescription(void)
{
	const TCHAR* pDescription = NULL;
	if (m_hSkill)
	{
		pDescription = m_hSkill->GetDescription();
	}

	return pDescription;
}

int CDnItem::GetSkillSEDurationTime(void)
{
	int iDurationTime = 0;

	if (m_hSkill)
	{
		if (0 < m_hSkill->GetStateEffectCount())
		{
			iDurationTime = m_hSkill->GetStateEffectFromIndex(0)->nDurationTime;
		}
	}

	return iDurationTime;
}


int CDnItem::GetSkillEffectIconIndex(bool bBuff)
{
	if (m_hSkill)
	{
#ifdef PRE_ADD_SKILL_ADDTIONAL_BUFF
		return bBuff ? m_hSkill->GetBuffIconImageIndex() : m_hSkill->GetAdditionalIconImageIndex();
#else
		return m_hSkill->GetBuffIconImageIndex();
#endif
	}

	return -1;
}

void CDnItem::ChangeToSkillIconIndex(void)
{
	if (m_hSkill)
		m_nIconImageIndex = m_hSkill->GetIconImageIndex();

	// 봉인 아이콘 표시 할 필요 없으므로 해제.
	m_Reversion = NoReversion;
}

#ifdef PRE_ADD_DMIX_DESIGNNAME
TCHAR* CDnItem::GetName() const
{
	if (GetLookItemID() != ITEMCLSID_NONE)
	{
		DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
		if (!pSox || !pSox->IsExistItem(GetLookItemID())) return NULL;

		return (TCHAR*)GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr(GetLookItemID(), "_NameID")->GetInteger());
	}
	return (TCHAR*)m_szName.c_str();
}

const TCHAR* CDnItem::GetDescription()
{
	if (GetLookItemID() != ITEMCLSID_NONE)
	{
		if (m_szLookItemDescriptionCache.empty())
		{
			DNTableFileFormat* pTableFormat = GetDNTable(CDnTableDB::TITEM);
			MakeLookItemDescription(pTableFormat);
		}

		return m_szLookItemDescriptionCache.c_str();
	}

	return m_szDescription.c_str();
}
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
bool CDnItem::IsPotentialExtractableItem()
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPOTENTIAL_TRANS);
	if (!pSox)
	{
		_ASSERT(0);
		return false;
	}

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if (IsPcCafeRentItem())
		return false;
#endif

	int nItemSubType = -1;
	if (m_ItemType == ITEMTYPE_WEAPON)
	{
		CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(this);
		if (pWeapon) nItemSubType = pWeapon->GetEquipType();
	}
	else if (m_ItemType == ITEMTYPE_PARTS)
	{
		CDnParts *pParts = dynamic_cast<CDnParts *>(this);
		if (pParts) nItemSubType = pParts->GetPartsType();

		// 두번째 반지슬롯에 끼려고 할때 SubType을 Ring2로 바꾸는 코드때문에 이렇게 처리한다.
		if (nItemSubType == CDnParts::Ring2) nItemSubType = CDnParts::Ring;
	}

	if (nItemSubType == -1)
		return false;

	for (int i = 0; i < (int)pSox->GetItemCount(); i++)
	{
		int nItemID = pSox->GetItemID(i);
		int nLevel = pSox->GetFieldFromLablePtr(nItemID, "_FromLevel")->GetInteger();
		int nRank = pSox->GetFieldFromLablePtr(nItemID, "_FromGrade")->GetInteger();
		int nMainType = pSox->GetFieldFromLablePtr(nItemID, "_MainType")->GetInteger();
		int nSubType = pSox->GetFieldFromLablePtr(nItemID, "_SubType")->GetInteger();

		if (nLevel == m_nLevelLimit && nRank == m_Rank && nMainType == m_ItemType && nSubType == nItemSubType)
			return true;
	}

	return false;
}
#endif

#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
bool CDnItem::IsPcCafeRentItem()
{
	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPCBANGRENTALITEM);
	if (!pSox)
	{
		_ASSERT(0);
		return false;
	}

	for (int i = 0; i < (int)pSox->GetItemCount(); i++)
	{
		int nIdx = pSox->GetItemID(i);
		int nItemID = pSox->GetFieldFromLablePtr(nIdx, "_ItemID")->GetInteger();
		if (m_nClassID == nItemID)
			return true;
	}
	return false;
}
#endif
#ifdef PRE_ADD_SERVER_WAREHOUSE
bool CDnItem::IsTradable(bool bIsWStorage) const
{
	bool bTradable = true;

	if (IsEternityItem() == false)
		return false;

	switch (GetReversion())
	{
	case ITEMREVERSION_BELONG:
	{
		if (IsSoulbBound())
		{
			bTradable = false;
			if (bIsWStorage)
			{
				if (IsWstorage())
					bTradable = true;
			}
		}
	}
	break;

	case ITEMREVERSION_TRADE:
	{
		int count = GetSealCount();
		if (count == 0)
		{
			if (bIsWStorage)
				bTradable = true;
			else
				bTradable = false;
		}
	}
	break;
	}

	return bTradable;
}
#endif