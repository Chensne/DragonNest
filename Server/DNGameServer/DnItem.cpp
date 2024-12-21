#include "StdAfx.h"
#include "DnItem.h"
#include "DnTableDB.h"
#include "DnActor.h"
#include "DnGameDataManager.h"
#include "DnDropItem.h"


CDnItem::CDnItem( CMultiRoom *pRoom )
: CMultiElement( pRoom )
{
	m_nSerialID = -1;
	m_nSeed = 0;
	m_ItemType = (eItemTypeEnum)-1;
	m_Rank = (eItemRank)-1;
	m_SkillApplyType = ItemSkillApplyType::None;

	m_bIsCashItem = false;
	m_nAmount = 0;
	m_nLevelLimit = -1;
	m_nMaxOverlapCount = 1;
	m_nOverlapCount = 1;
	m_nSkillID = 0;
	m_nSkillLevel = 0;
	memset( m_nItemTypeParam, 0, sizeof(m_nItemTypeParam) );
	m_bNeedAuthentication = false;
	m_bCanDestruction = true;
	m_bAuthentication = false;
	// m_bIdentity = false;
	m_dwColor = 0xFFFFFFFF;

	m_nEnchantTableID = 0;
	m_nEnchantID = 0;
	m_cEnchantLevel = 0;
	m_cPotentialIndex = 0;
	m_cOptionIndex = 0;
	m_cSealCount = 0;
	m_bSoulBound = false;

	m_nClassID = -1;

	m_pEnchant = NULL;
	m_pPotential = NULL;

	m_nDisjointType = 0;
	m_bCanDisjoint = false;
	m_nDisjointCost = 0;
	ZeroMemory( m_nDisjointDropTableID, sizeof(m_nDisjointDropTableID) );

#if defined( PRE_FIX_69709 )
	m_iEnchantDisjointDrop = 0;
#endif // #if defined( PRE_FIX_69709 )
	
	m_Reversion = ItemReversionEnum_Amount;
	m_nDescriptionStringID = 0;
	m_nNameStringID = 0;
}

CDnItem::~CDnItem()
{
	SAFE_DELETE( m_pEnchant );
	SAFE_DELETE( m_pPotential );
	SAFE_RELEASE_SPTR( m_hSkill );
}

bool CDnItem::Initialize( int nTableID, int nSeed, char cOptionIndex, char cEnchantLevel, char cPotentialIndex, char cSealCount, bool bSoulBound )
{
	m_nClassID = nTableID;
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"ItemTable.ext failed\r\n");
		return false;
	}
	if( pSox->IsExistItem( nTableID ) == false ) return false;

	m_nSeed = nSeed;
	m_cEnchantLevel = cEnchantLevel;
	m_cPotentialIndex = cPotentialIndex;
	m_cOptionIndex = cOptionIndex;
	m_bSoulBound = bSoulBound;
	m_cSealCount = cSealCount;

	m_nNameStringID = pSox->GetFieldFromLablePtr( nTableID, "_NameID" )->GetInteger();
	m_nDescriptionStringID = pSox->GetFieldFromLablePtr( nTableID, "_DescriptionID" )->GetInteger();
	m_ItemType = (eItemTypeEnum)pSox->GetFieldFromLablePtr( nTableID, "_Type" )->GetInteger();
	m_nItemTypeParam[0] = pSox->GetFieldFromLablePtr( nTableID, "_TypeParam1" )->GetInteger();
	m_nItemTypeParam[1] = pSox->GetFieldFromLablePtr( nTableID, "_TypeParam2" )->GetInteger();
	m_nLevelLimit = pSox->GetFieldFromLablePtr( nTableID, "_LevelLimit" )->GetInteger();
	m_Rank = (eItemRank)pSox->GetFieldFromLablePtr( nTableID, "_Rank" )->GetInteger();
	m_Reversion = (ItemReversionEnum)pSox->GetFieldFromLablePtr( nTableID, "_Reversion" )->GetInteger();
	m_bNeedAuthentication = ( pSox->GetFieldFromLablePtr( nTableID, "_IsAuthentication" )->GetInteger() == TRUE ) ? true : false;
	m_bCanDestruction = ( pSox->GetFieldFromLablePtr( nTableID, "_IsDestruction" )->GetInteger() == TRUE ) ? true : false;
	m_bIsCashItem = ( pSox->GetFieldFromLablePtr( nTableID, "_IsCash" )->GetInteger() == TRUE ) ? true : false;
	m_nAmount = pSox->GetFieldFromLablePtr( nTableID, "_Amount" )->GetInteger();
	m_nMaxOverlapCount = pSox->GetFieldFromLablePtr( nTableID, "_OverlapCount" )->GetInteger();
	m_nEnchantID = pSox->GetFieldFromLablePtr( nTableID, "_EnchantID" )->GetInteger();

	// 분해 관련 데이터 ///////////////////////////////////////////////////////
	m_nDisjointType = pSox->GetFieldFromLablePtr( nTableID, "_DisjointorType" )->GetInteger();
	m_nDisjointCost = pSox->GetFieldFromLablePtr( nTableID, "_Disjointamount" )->GetInteger();

	char acBuf[ 128 ];
	for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
	{
		sprintf_s( acBuf, sizeof(acBuf), "_DisjointDrop%d", i+1 );
		m_nDisjointDropTableID[ i ] = pSox->GetFieldFromLablePtr( nTableID, acBuf )->GetInteger();
	}

	for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
	{
		if( 0 != m_nDisjointDropTableID[ i ] )
		{
			m_bCanDisjoint = true;
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	// 사용 가능 직업
	DNTableCell Field;
	if( pSox->GetFieldFromLable( nTableID, "_NeedJobClass", Field ) == false ) return false;
	char *szPermitStr = Field.GetString();
	int nJobIndex;
	for( int i=0;; i++ ) {
		//char *pStr = _GetSubStrByCount( i, szPermitStr, ';' );
		std::string strValue = _GetSubStrByCountSafe( i, szPermitStr, ';' );
		if( strValue.size() == 0 ) break;
		nJobIndex = atoi(strValue.c_str());
		if( nJobIndex < 1 ) continue;
		m_nVecPermitJobList.push_back( nJobIndex );
	}

	// -1 인 시드는 PreInitialize 일 경우기 때문에 Random 을 돌리면 틀어진다.
	// 0 인 시드는 플레이어 디폴트 웨폰들 생성할때 쓴다. 근데 랜덤시드가 항상 같게 마춰져서 InitializePlayerActor쪽에서
	// 생성 후에 랜덤사용시(PvP StartPosition 셋팅등) 같게 나와 불편하므로 0일땐 백업해놨다가 다시 생성해준다.
	if( nSeed == -1 ) return true;
	CRandom::LockSeed( GetRoom() );
	_srand( GetRoom(), nSeed );

	char szLabel[32];

	// 기본 능력치 계산
	for( int i=0; i<10; i++ ) {
		sprintf_s( szLabel, "_State%d", i + 1 );
		int nStateType = pSox->GetFieldFromLablePtr( nTableID, szLabel )->GetInteger();
		sprintf_s( szLabel, "_State%d_GenProb", i + 1 );

		if( IsPossibleProb( pSox, szLabel ) ) {
			sprintf_s( szLabel, "_State%d_Min", i + 1 );
			char *szMin = pSox->GetFieldFromLablePtr( nTableID, szLabel )->GetString();
			sprintf_s( szLabel, "_State%d_Max", i + 1 );
			char *szMax = pSox->GetFieldFromLablePtr( nTableID, szLabel )->GetString();

			///////////////////////////
			CalcStateValue( GetRoom(), this, nStateType, szMin, szMax );
		}
		else DummyRandom();
	}
	CalcValueType();

	// Skill
	if( IsPossibleProb( pSox, "_Skill_GenProb" ) ) {
		m_nSkillID = pSox->GetFieldFromLablePtr( nTableID, "_SkillID" )->GetInteger();
		m_nSkillLevel = pSox->GetFieldFromLablePtr( nTableID, "_SkillLevel" )->GetInteger();
		m_SkillApplyType = (ItemSkillApplyType)pSox->GetFieldFromLablePtr( nTableID, "_SkillUsingType" )->GetInteger();
	}
	else DummyRandom();

	// 옵션 능력치 계산
	InitializeOption();
	// 인첸트 능력치 계산
	InitializeEnchant();
	// 포텐샬 능력치 계산
	InitializePotential();

	// 아이템 분해 시에 나올 드랍 아이템 리스트 미리 뽑아 놓음.
	_GenerationDropItem();

	CRandom::UnlockSeed( GetRoom() );

	return true;
}

void CDnItem::Process( LOCAL_TIME LocalTime, float fDelta )
{

}

bool CDnItem::IsPossibleProb( DNTableFileFormat* pSox, const char *szLabel )
{
	_fpreset();
	int nProb = (int)( pSox->GetFieldFromLablePtr( m_nClassID, szLabel )->GetFloat() * 100.f );
	if( _rand(GetRoom())%100 < nProb ) return true;
	return false;
}

int CDnItem::GetRandomStateNumber( CMultiRoom *pRoom, int nMin, int nMax )
{
	int nRandom = abs( _rand(pRoom) );
	if( nMin == nMax ) return nMin;

	int nValue = nMax - nMin;

	_fpreset();
	float fRatio = ( 1.f / (float)INT_MAX ) * (float)nRandom;
	return nMin + (int)( nValue * fRatio );
}

float CDnItem::GetRandomStateRatio( CMultiRoom *pRoom, float fMin, float fMax )
{
	int nRandom = abs( _rand(pRoom) );

	_fpreset();

	int nMin = (int)( fMin * 1000.f );
	int nMax = (int)( fMax * 1000.f );
	if( nMin == nMax ) return ( nMin * 0.001f );

	int nValue = nMax - nMin;

	float fRatio = ( 1.f / (float)INT_MAX ) * (float)nRandom;
	return ( nMin + (int)( nValue * fRatio ) ) * 0.001f;
}

void CDnItem::DummyRandom( int nCount )
{
	for( int i=0; i<nCount; i++ ) _rand(GetRoom());
}


eItemTypeEnum CDnItem::GetItemType( int nItemTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pSox ) return (eItemTypeEnum)-1;

	DNTableCell Field;
	if( pSox->GetFieldFromLable( nItemTableID, "_Type", Field ) == false ) 
		return (eItemTypeEnum)-1;

	return (eItemTypeEnum)Field.GetInteger();
}

eItemRank CDnItem::GetItemRank( int nItemTableID )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( !pSox ) return (eItemRank)-1;

	DNTableCell Field;
	if( pSox->GetFieldFromLable( nItemTableID, "_Rank", Field ) == false ) 
		return (eItemRank)-1;

	return (eItemRank)Field.GetInteger();
}

bool CDnItem::ActivateSkillEffect( DnActorHandle hActor )
{
	if( m_nSkillID < 1 ) return true;
	if( m_nSkillLevel < 1 ) return true;
	SAFE_RELEASE_SPTR( m_hSkill );

	bool bResult = hActor->UseItemSkill( m_nSkillID, m_nSkillLevel, m_SkillApplyType, m_nClassID );
	_ASSERT( bResult && "CDnItem::ActivateSkillEffect() : 아이템 스킬 사용 실패" );

	return true;
}

bool CDnItem::DeactivateSkillEffect()
{
	SAFE_RELEASE_SPTR( m_hSkill );
	return true;
}

CDnItem *CDnItem::CreateItem( CMultiRoom *pRoom, int nTableID, int nSeed )
{
	CDnItem *pItem = new IBoostPoolDnItem( pRoom );
	if( pItem->Initialize( nTableID, nSeed ) == false ) {
		SAFE_DELETE( pItem );
		return NULL;
	}
	return pItem;
}

CDnItem &CDnItem::operator = ( TItem &e )
{
	m_nSerialID = e.nSerial;
	m_nOverlapCount = e.wCount;
	m_cEnchantLevel = e.cLevel;
	m_cPotentialIndex = e.cPotential;
	m_cOptionIndex = e.cOption;
	m_bSoulBound = e.bSoulbound;
	m_cSealCount = e.cSealCount;

	return *this;
}

void CDnItem::InitializeEnchant()
{
	SAFE_DELETE( m_pEnchant );

#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
	DNTableFileFormat* pEnchantStateSox = GetDNTable( CDnTableDB::TENCHANT_STATE );
	DNTableFileFormat* pEnchantNeedItemSox = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
	DNTableFileFormat *pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	if (!pSox || !pPotentialSox || !pEnchantStateSox || !pEnchantNeedItemSox)
	{
		g_Log.Log( LogType::_FILELOG, L"EnchantMainTable.ext / PotentialTable.ext / EnchantMaxStateTable.ext / EnchantNeedItemTable.ext failed\r\n");
		return;
	}
#else
	DNTableFileFormat *pSox = GetDNTable( CDnTableDB::TENCHANT );
	DNTableFileFormat *pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	if( !pSox || !pPotentialSox ) 
	{
		g_Log.Log( LogType::_FILELOG, L"EnchantTable.ext / PotentialTable.ext failed\r\n");
		return;
	}
#endif

	// 인첸트 셋팅
	if( m_cEnchantLevel == 0 ) return;

	TEnchantLevelData *pData = g_pDataManager->GetEnchantLevelData( m_nEnchantID, m_cEnchantLevel );
	if( !pData ) return;
	int nResultItemID = pData->nItemID;
	if( nResultItemID == 0 ) return;
	m_nEnchantTableID = nResultItemID;
#ifdef PRE_FIX_MEMOPT_ENCHANT
	int enchantStateTableID = pData->nStateTableID;
	int enchantNeedItemTableID = pData->nNeedItemTableID;

	const TEnchantNeedItemData* pNeedItemData = g_pDataManager->GetEnchantNeedItemData(*pData);
	if (pNeedItemData == NULL)
		return;
#endif

	m_pEnchant = new CDnState;
	char szLabel[32];
	for( int i=0; i<10; i++ ) {
		sprintf_s( szLabel, "_State%d", i + 1 );
#ifdef PRE_FIX_MEMOPT_ENCHANT
		int nStateType = pEnchantStateSox->GetFieldFromLablePtr( enchantStateTableID, szLabel )->GetInteger();
#else
		int nStateType = pSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetInteger();
#endif
		if( nStateType == -1 ) break;

		sprintf_s( szLabel, "_State%dValue", i + 1 );
#ifdef PRE_FIX_MEMOPT_ENCHANT
		char *szValue = pEnchantStateSox->GetFieldFromLablePtr( enchantStateTableID, szLabel )->GetString();
		int value = atoi(szValue);
		int result = int(value * pNeedItemData->fUpStateRatio);

		CalcStateValue( GetRoom(), m_pEnchant, nStateType, (float)result, (float)result, result, result, false );
#else
		char *szValue = pSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetString();

		CalcStateValue( GetRoom(), m_pEnchant, nStateType, szValue, szValue, false );
#endif
	}

#if defined( PRE_FIX_69709 )
	m_iEnchantDisjointDrop = pSox->GetFieldFromLablePtr( m_nEnchantTableID, "_DisjointDrop" )->GetInteger();
#endif // #if defined( PRE_FIX_69709 )

	m_pEnchant->CalcValueType();
}

void CDnItem::InitializeOption()
{
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	if( m_ItemType != eItemTypeEnum::ITEMTYPE_WEAPON && m_ItemType != eItemTypeEnum::ITEMTYPE_PARTS && m_ItemType != eItemTypeEnum::ITEMTYPE_GLYPH && m_ItemType != eItemTypeEnum::ITEMTYPE_TALISMAN ) return;
#else
	if( m_ItemType != eItemTypeEnum::ITEMTYPE_WEAPON && m_ItemType != eItemTypeEnum::ITEMTYPE_PARTS && m_ItemType != eItemTypeEnum::ITEMTYPE_GLYPH ) return;
#endif
	if( m_nItemTypeParam[0] < 1 ) return;
	if( m_cOptionIndex <= 0 ) return;

	DNTableFileFormat* pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	TPotentialData *pPotentialData = g_pDataManager->GetPotentialData( m_nItemTypeParam[0] );
	if( !pPotentialData ) return;

	if( pPotentialData->pVecItemData.empty() ) return;
	if( m_cOptionIndex > (int)pPotentialData->pVecItemData.size() ) return;

	int nResultItemID = pPotentialData->pVecItemData[m_cOptionIndex-1]->nItemID;
	char szLabel[32];
	CDnState OptionState;
	for( int i=0; i<16; i++ ) {
		sprintf_s( szLabel, "_State%d", i + 1 );
		int nStateType = pPotentialSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetInteger();
		if( nStateType == -1 ) break;

		sprintf_s( szLabel, "_State%dValue", i + 1 );
		char *szValue = pPotentialSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetString();

		CalcStateValue( GetRoom(), &OptionState, nStateType, szValue, szValue, false );
	}
	OptionState.CalcValueType();

	*this += OptionState;
}

void CDnItem::InitializePotential()
{
	SAFE_DELETE( m_pPotential );

	if( m_ItemType != eItemTypeEnum::ITEMTYPE_WEAPON && m_ItemType != eItemTypeEnum::ITEMTYPE_PARTS && m_ItemType != eItemTypeEnum::ITEMTYPE_GLYPH) return;
	if( m_nItemTypeParam[1] < 1 ) return;
	if( m_cPotentialIndex <= 0 ) return;
	// 포텐셜 셋팅
	DNTableFileFormat* pPotentialSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	TPotentialData *pPotentialData = g_pDataManager->GetPotentialData( m_nItemTypeParam[1] );
	if( !pPotentialData ) return;

	if( pPotentialData->pVecItemData.empty() ) return;
	if( m_cPotentialIndex > (int)pPotentialData->pVecItemData.size() ) return;

	char szLabel[32];
	m_pPotential = new CDnState;
	int nResultItemID = pPotentialData->pVecItemData[m_cPotentialIndex-1]->nItemID;
	for( int i=0; i<16; i++ ) {
		sprintf_s( szLabel, "_State%d", i + 1 );
		int nStateType = pPotentialSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetInteger();
		if( nStateType == -1 ) break;

		sprintf_s( szLabel, "_State%dValue", i + 1 );
		char *szValue = pPotentialSox->GetFieldFromLablePtr( nResultItemID, szLabel )->GetString();

		CalcStateValue( GetRoom(), m_pPotential, nStateType, szValue, szValue, false );
	}
	m_pPotential->CalcValueType();
}

void CDnItem::CalcStateValue( CMultiRoom *pRoom, CDnState *pState, int nStateType, char *szMin, char *szMax, bool bRandomValue, bool bResetValue , float fRatio )
{
	int nMin, nMax;
	float fMin, fMax;
	nMin = (int)( (float)atof(szMin) * fRatio );
	nMax = (int)( (float)atof(szMax) * fRatio );
	fMin = (float)atof(szMin) * fRatio;
	fMax = (float)atof(szMax) * fRatio;

#ifdef PRE_FIX_ATOF_ROUNDOFF
	fMax += 0.0000001f;
	fMin += 0.0000001f;
#endif // #ifdef PRE_FIX_ATOF_ROUNDOFF

#ifdef PRE_FIX_MEMOPT_ENCHANT
	CalcStateValue(pRoom, pState, nStateType, fMin, fMax, nMin, nMax, bRandomValue, bResetValue);
#else
	switch( nStateType ) {
		case 0: pState->SetStrength( ( ( bResetValue ) ? 0 : pState->GetStrength() ) + ( + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ) ); break;
		case 1: pState->SetAgility( ( ( bResetValue ) ? 0 : pState->GetAgility() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 2: pState->SetIntelligence( ( ( bResetValue ) ? 0 : pState->GetIntelligence() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 3: pState->SetStamina( ( ( bResetValue ) ? 0 : pState->GetStamina() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 4: pState->SetAttackPMin( ( ( bResetValue ) ? 0 : pState->GetAttackPMin() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 5: pState->SetAttackPMax( ( ( bResetValue ) ? 0 : pState->GetAttackPMax() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 6: pState->SetAttackMMin( ( ( bResetValue ) ? 0 : pState->GetAttackMMin() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 7: pState->SetAttackMMax( ( ( bResetValue ) ? 0 : pState->GetAttackMMax() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 8: pState->SetDefenseP( ( ( bResetValue ) ? 0 : pState->GetDefenseP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 9: pState->SetDefenseM( ( ( bResetValue ) ? 0 : pState->GetDefenseM() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 10: pState->SetStiff( ( ( bResetValue ) ? 0 : pState->GetStiff() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 11: pState->SetStiffResistance( ( ( bResetValue ) ? 0 : pState->GetStiffResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 12: pState->SetCritical( ( ( bResetValue ) ? 0 : pState->GetCritical() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 13: pState->SetCriticalResistance( ( ( bResetValue ) ? 0 : pState->GetCriticalResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 14: pState->SetStun( ( ( bResetValue ) ? 0 : pState->GetStun() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 15: pState->SetStunResistance( ( ( bResetValue ) ? 0 : pState->GetStunResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 16: pState->SetElementAttack( ElementEnum::Fire, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Fire) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 17: pState->SetElementAttack( ElementEnum::Ice, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Ice) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 18: pState->SetElementAttack( ElementEnum::Light, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Light) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 19: pState->SetElementAttack( ElementEnum::Dark, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Dark) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 20: pState->SetElementDefense( ElementEnum::Fire, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Fire) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 21: pState->SetElementDefense( ElementEnum::Ice, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Ice) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 22: pState->SetElementDefense( ElementEnum::Light, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Light) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 23: pState->SetElementDefense( ElementEnum::Dark, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Dark) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 24: pState->SetMoveSpeed( ( ( bResetValue ) ? 0 : pState->GetMoveSpeed() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 25: pState->SetMaxHP( ( ( bResetValue ) ? 0 : pState->GetMaxHP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 26: pState->SetMaxSP( ( ( bResetValue ) ? 0 : pState->GetMaxSP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 27: pState->SetRecoverySP( ( ( bResetValue ) ? 0 : pState->GetRecoverySP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 28: pState->SetSuperAmmor( ( ( bResetValue ) ? 0 : pState->GetSuperAmmor() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 29: pState->SetFinalDamage( ( ( bResetValue ) ? 0 : pState->GetFinalDamage() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 30: pState->SetSafeZoneMoveSpeed( ( ( bResetValue ) ? 0 : pState->GetSafeZoneMoveSpeed() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
		case 31: pState->SetAddExp( ( ( bResetValue ) ? 0 : pState->GetAddExp() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;

		// Ratio
		case 50: pState->SetStrengthRatio( ( ( bResetValue ) ? 0 : pState->GetStrengthRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 51: pState->SetAgilityRatio( ( ( bResetValue ) ? 0 : pState->GetAgilityRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 52: pState->SetIntelligenceRatio( ( ( bResetValue ) ? 0 : pState->GetIntelligenceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 53: pState->SetStaminaRatio( ( ( bResetValue ) ? 0 : pState->GetStaminaRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 54: pState->SetAttackPMinRatio( ( ( bResetValue ) ? 0 : pState->GetAttackPMinRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 55: pState->SetAttackPMaxRatio( ( ( bResetValue ) ? 0 : pState->GetAttackPMaxRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 56: pState->SetAttackMMinRatio( ( ( bResetValue ) ? 0 : pState->GetAttackMMinRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 57: pState->SetAttackMMaxRatio( ( ( bResetValue ) ? 0 : pState->GetAttackMMaxRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 58: pState->SetDefensePRatio( ( ( bResetValue ) ? 0 : pState->GetDefensePRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 59: pState->SetDefenseMRatio( ( ( bResetValue ) ? 0 : pState->GetDefenseMRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 60: pState->SetStiffRatio( ( ( bResetValue ) ? 0 : pState->GetStiffRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 61: pState->SetStiffResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetStiffResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 62: pState->SetCriticalRatio( ( ( bResetValue ) ? 0 : pState->GetCriticalRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 63: pState->SetCriticalResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetCriticalResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 64: pState->SetStunRatio( ( ( bResetValue ) ? 0 : pState->GetStunRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 65: pState->SetStunResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetStunResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
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
		case 74: pState->SetMoveSpeedRatio( ( ( bResetValue ) ? 0 : pState->GetMoveSpeedRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 75: pState->SetMaxHPRatio( ( ( bResetValue ) ? 0 : pState->GetMaxHPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 76: pState->SetMaxSPRatio( ( ( bResetValue ) ? 0 : pState->GetMaxSPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 77: pState->SetRecoverySPRatio( ( ( bResetValue ) ? 0 : pState->GetRecoverySPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 78: pState->SetSuperAmmorRatio( ( ( bResetValue ) ? 0 : pState->GetSuperAmmorRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 79: pState->SetFinalDamageRatio( ( ( bResetValue ) ? 0 : pState->GetFinalDamageRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
		case 81: pState->SetSafeZoneMoveSpeedRatio( ( ( bResetValue ) ? 0 : pState->GetSafeZoneMoveSpeedRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;

		case 100:pState->SetSpirit( ( ( bResetValue ) ? 0 : pState->GetSpirit() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;

		default: 
			if( bRandomValue ) {
				_rand(pRoom);
			}
			break;
	}
#endif // PRE_FIX_MEMOPT_ENCHANT
}

#ifdef PRE_FIX_MEMOPT_ENCHANT
void CDnItem::CalcStateValue( CMultiRoom *pRoom, CDnState *pState, int nStateType, float fMin, float fMax, int nMin, int nMax, bool bRandomValue, bool bResetValue )
{
	switch( nStateType ) {
	case 0: pState->SetStrength( ( ( bResetValue ) ? 0 : pState->GetStrength() ) + ( + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ) ); break;
	case 1: pState->SetAgility( ( ( bResetValue ) ? 0 : pState->GetAgility() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 2: pState->SetIntelligence( ( ( bResetValue ) ? 0 : pState->GetIntelligence() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 3: pState->SetStamina( ( ( bResetValue ) ? 0 : pState->GetStamina() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 4: pState->SetAttackPMin( ( ( bResetValue ) ? 0 : pState->GetAttackPMin() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 5: pState->SetAttackPMax( ( ( bResetValue ) ? 0 : pState->GetAttackPMax() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 6: pState->SetAttackMMin( ( ( bResetValue ) ? 0 : pState->GetAttackMMin() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 7: pState->SetAttackMMax( ( ( bResetValue ) ? 0 : pState->GetAttackMMax() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 8: pState->SetDefenseP( ( ( bResetValue ) ? 0 : pState->GetDefenseP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 9: pState->SetDefenseM( ( ( bResetValue ) ? 0 : pState->GetDefenseM() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 10: pState->SetStiff( ( ( bResetValue ) ? 0 : pState->GetStiff() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 11: pState->SetStiffResistance( ( ( bResetValue ) ? 0 : pState->GetStiffResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 12: pState->SetCritical( ( ( bResetValue ) ? 0 : pState->GetCritical() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 13: pState->SetCriticalResistance( ( ( bResetValue ) ? 0 : pState->GetCriticalResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 14: pState->SetStun( ( ( bResetValue ) ? 0 : pState->GetStun() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 15: pState->SetStunResistance( ( ( bResetValue ) ? 0 : pState->GetStunResistance() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 16: pState->SetElementAttack( ElementEnum::Fire, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Fire) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 17: pState->SetElementAttack( ElementEnum::Ice, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Ice) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 18: pState->SetElementAttack( ElementEnum::Light, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Light) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 19: pState->SetElementAttack( ElementEnum::Dark, ( ( bResetValue ) ? 0 : pState->GetElementAttack(ElementEnum::Dark) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 20: pState->SetElementDefense( ElementEnum::Fire, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Fire) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 21: pState->SetElementDefense( ElementEnum::Ice, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Ice) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 22: pState->SetElementDefense( ElementEnum::Light, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Light) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 23: pState->SetElementDefense( ElementEnum::Dark, ( ( bResetValue ) ? 0 : pState->GetElementDefense(ElementEnum::Dark) ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 24: pState->SetMoveSpeed( ( ( bResetValue ) ? 0 : pState->GetMoveSpeed() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 25: pState->SetMaxHP( ( ( bResetValue ) ? 0 : pState->GetMaxHP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 26: pState->SetMaxSP( ( ( bResetValue ) ? 0 : pState->GetMaxSP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 27: pState->SetRecoverySP( ( ( bResetValue ) ? 0 : pState->GetRecoverySP() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 28: pState->SetSuperAmmor( ( ( bResetValue ) ? 0 : pState->GetSuperAmmor() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 29: pState->SetFinalDamage( ( ( bResetValue ) ? 0 : pState->GetFinalDamage() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 30: pState->SetSafeZoneMoveSpeed( ( ( bResetValue ) ? 0 : pState->GetSafeZoneMoveSpeed() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;
	case 31: pState->SetAddExp( ( ( bResetValue ) ? 0 : pState->GetAddExp() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;

	// Ratio
	case 50: pState->SetStrengthRatio( ( ( bResetValue ) ? 0 : pState->GetStrengthRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 51: pState->SetAgilityRatio( ( ( bResetValue ) ? 0 : pState->GetAgilityRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 52: pState->SetIntelligenceRatio( ( ( bResetValue ) ? 0 : pState->GetIntelligenceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 53: pState->SetStaminaRatio( ( ( bResetValue ) ? 0 : pState->GetStaminaRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 54: pState->SetAttackPMinRatio( ( ( bResetValue ) ? 0 : pState->GetAttackPMinRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 55: pState->SetAttackPMaxRatio( ( ( bResetValue ) ? 0 : pState->GetAttackPMaxRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 56: pState->SetAttackMMinRatio( ( ( bResetValue ) ? 0 : pState->GetAttackMMinRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 57: pState->SetAttackMMaxRatio( ( ( bResetValue ) ? 0 : pState->GetAttackMMaxRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 58: pState->SetDefensePRatio( ( ( bResetValue ) ? 0 : pState->GetDefensePRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 59: pState->SetDefenseMRatio( ( ( bResetValue ) ? 0 : pState->GetDefenseMRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 60: pState->SetStiffRatio( ( ( bResetValue ) ? 0 : pState->GetStiffRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 61: pState->SetStiffResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetStiffResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 62: pState->SetCriticalRatio( ( ( bResetValue ) ? 0 : pState->GetCriticalRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 63: pState->SetCriticalResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetCriticalResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 64: pState->SetStunRatio( ( ( bResetValue ) ? 0 : pState->GetStunRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 65: pState->SetStunResistanceRatio( ( ( bResetValue ) ? 0 : pState->GetStunResistanceRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
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
	case 74: pState->SetMoveSpeedRatio( ( ( bResetValue ) ? 0 : pState->GetMoveSpeedRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 75: pState->SetMaxHPRatio( ( ( bResetValue ) ? 0 : pState->GetMaxHPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 76: pState->SetMaxSPRatio( ( ( bResetValue ) ? 0 : pState->GetMaxSPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 77: pState->SetRecoverySPRatio( ( ( bResetValue ) ? 0 : pState->GetRecoverySPRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 78: pState->SetSuperAmmorRatio( ( ( bResetValue ) ? 0 : pState->GetSuperAmmorRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 79: pState->SetFinalDamageRatio( ( ( bResetValue ) ? 0 : pState->GetFinalDamageRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;
	case 81: pState->SetSafeZoneMoveSpeedRatio( ( ( bResetValue ) ? 0 : pState->GetSafeZoneMoveSpeedRatio() ) + ( ( bRandomValue ) ? GetRandomStateRatio( pRoom, fMin, fMax ) : fMin ) ); break;

	case 100:pState->SetSpirit( ( ( bResetValue ) ? 0 : pState->GetSpirit() ) + ( ( bRandomValue ) ? GetRandomStateNumber( pRoom, nMin, nMax ) : nMin ) ); break;

	default: 
		if( bRandomValue ) {
			_rand(pRoom);
		}
		break;
	}
}
#endif

// 분해시에 드랍될 아이템들 미리 생성해 둠
void CDnItem::_GenerationDropItem( void )
{
	// #28327 강화된 아이템이라면 강화 테이블의 Disjoint 라벨을 참조해서 아이템을 떨군다.
	if( 0 < m_cEnchantLevel )
	{
#ifdef PRE_FIX_MEMOPT_ENCHANT
		DNTableFileFormat* pEnchantTable = GetDNTable( CDnTableDB::TENCHANT_MAIN );
#else
		DNTableFileFormat* pEnchantTable = GetDNTable( CDnTableDB::TENCHANT );
#endif
		int iDropItemTableID = pEnchantTable->GetFieldFromLablePtr( m_nEnchantTableID, "_DisjointDrop" )->GetInteger();
		CDnDropItem::CalcDropItemList( GetRoom(), iDropItemTableID, m_VecDropItemList, false );
	}

	for( int i = 0; i < NUM_DISJOINT_DROP; ++i )
		CDnDropItem::CalcDropItemList( GetRoom(), m_nDisjointDropTableID[ i ], m_VecDropItemList, false );

	for( DWORD i = 0; i < m_VecDropItemList.size(); ++i )
	{
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList.at(i).nItemID, m_VecDropItemList.at(i).nEnchantID ) == false )
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		if( CDnDropItem::PreInitializeItem( GetRoom(), m_VecDropItemList.at(i).nItemID ) == false )
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		{
			m_VecDropItemList.erase( m_VecDropItemList.begin() + i );
			--i;

			OutputDebug( "[Error] CDnItem::GenerationDropItem Failed!! ItemID: %d\n", m_nClassID );
		}
	}

	// TODO: 인챈트 실패시 나오는 아이템도 셋팅
}

bool CDnItem::CanDisjoint()
{
	if( m_nDisjointType == 0 ) 
		return false;

#if defined(PRE_FIX_69709)
	if (m_nDisjointDropTableID[0] == 0 || ((m_cEnchantLevel > 0) && (m_iEnchantDisjointDrop == 0)))
		return false;
#endif //#if defined(PRE_FIX_69709)

	return m_bCanDisjoint; 
}

bool CDnItem::CanDisjointType( int nDisjointType )
{
	if( !( nDisjointType & m_nDisjointType ) ) return false;
	return true;
}

bool CDnItem::CanDisjointLevel( int iUserLevel )
{
	if( m_nLevelLimit >= static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Disjoint_ItemLimitLevel )) )
	{
		if( iUserLevel < static_cast<int>(CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::Disjoint_LimitUserLevel )) )
			return false;
	}

	return true;
}

bool CDnItem::IsPermitPlayer( const std::vector<int> &nVecJobList )
{
	if( m_nVecPermitJobList.size() == 0 ) return true;
	for( DWORD i=0; i<nVecJobList.size(); i++ ) {
		for( DWORD j=0; j<m_nVecPermitJobList.size(); j++ ) {
			if( nVecJobList[i] == m_nVecPermitJobList[j] ) return true;
		}
	}
	return false;
}

bool CDnItem::IsSoulbBound() const
{
	if (m_Reversion == ITEMREVERSION_BELONG)
		return m_bSoulBound;

	return true;
}

bool CDnItem::HasPrefixSkill(int &nSkillID, int &nSkillLevel)
{
	// 무기/방어구
	if( m_ItemType != eItemTypeEnum::ITEMTYPE_WEAPON && m_ItemType != eItemTypeEnum::ITEMTYPE_PARTS ) return false;
	//m_nItemTypeParam[1]은 잠재 능력.. m_nItemTypeParam[0] 값을 참조 한다..
	if( m_nItemTypeParam[0] < 1 ) return false;
	if( m_cOptionIndex <= 0 ) return false;

	TPotentialData *pPotential = g_pDataManager->GetPotentialData( m_nItemTypeParam[0] );
	TPotentialDataItem* pData = NULL;

	if (pPotential)
	{
		if( m_cOptionIndex <= (int)pPotential->pVecItemData.size() )
		{
			pData = pPotential->pVecItemData[m_cOptionIndex-1];
		}
	}

	if (pData == NULL)
		return false;

	int nSkillUsingType = pData->nSkillUsingType;

	if (CDnItem::ItemSkillApplyType::PrefixSkill != nSkillUsingType)
		return false;

	nSkillID = pData->nSkillID;
	nSkillLevel = 1;

	nSkillLevel += m_cEnchantLevel;

	return true;
}


bool CDnItem::HasLevelUpInfo(int &nSkillID, int &nSkillLevel, int &nSkillUsingType)
{
	//원래 아이템의 스킬레벨업 정보
	int nItemSkillLevelUpSkillID = m_nSkillID;
	int nItemSkillLevelUpSkillLevel = m_nSkillLevel;
	int nItemSkillLevelUpSkillUsingType = m_SkillApplyType;

	// 무기/방어구
	if( m_ItemType != eItemTypeEnum::ITEMTYPE_WEAPON && m_ItemType != eItemTypeEnum::ITEMTYPE_PARTS ) return false;

	//옵션 정보를 가지고 있는지 확인한다.
	int nOptionSkillID = 0;
	int nOptionSkillLevel = 0;
	int nOptionSkillUsingType = 0;

	//옵션값이 설정이 되어 있으면 옵션 값을 확인한다.
	if (m_nItemTypeParam[0] > 0 && m_cOptionIndex > 0)
	{
		TPotentialData *pPotential = g_pDataManager->GetPotentialData( m_nItemTypeParam[0] );
		if (pPotential)
		{
			if( m_cOptionIndex <= (int)pPotential->pVecItemData.size() )
			{
				TPotentialDataItem* pData = pPotential->pVecItemData[m_cOptionIndex-1];

				nOptionSkillID = pData->nSkillID;
				nOptionSkillLevel = pData->nSkillLevel;
				nOptionSkillUsingType = pData->nSkillUsingType;
			}
		}
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
