#include "StdAfx.h"
#include "DnInventory.h"
#include "DnItemTask.h"
#include "DnInventoryDlg.h"
#include <bitset>
#include "ItemSendPacket.h"
#include "DnWeapon.h"
#include "DnParts.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "DnPlayerState.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnInventory::CDnInventory(void)
	: m_nUsableSlotCount(0)
	, m_pInvenDlg(NULL)
	, m_pItemTask(NULL)
	, m_bSortRequesting(false)
#if defined(PRE_PERIOD_INVENTORY)
	, m_bSortPeriod( false )
#endif	// #if defined(PRE_PERIOD_INVENTORY)
{
	memset( m_SortSlotInfo, 0, sizeof(m_SortSlotInfo) );
}

CDnInventory::~CDnInventory(void)
{
	ClearInventory();
}

void CDnInventory::ClearInventory()
{
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		SAFE_DELETE( iter->second );
	}

	m_mapInventory.clear();
}

CDnItem* CDnInventory::GetItem( int nSlotIndex )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( iter != m_mapInventory.end() )
	{
		return iter->second;
	}

	return NULL;
}

void CDnInventory::CreateItem( TItemInfo &itemInfo )
{
	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, m_pInvenDlg is NULL!" );
		return;
	}

	if( !m_pItemTask )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, m_pItemTask is NULL!" );
		return;
	}

	CDnItem *pItem = m_pItemTask->CreateItem( itemInfo );

	if( pItem == NULL )
	{
		CDebugSet::ToLogFile( "CDnInventory::CreateItem, 아이템(%d) 생성에 실패!", itemInfo.Item.nItemID );
		return;
	}

	m_mapInventory[pItem->GetSlotIndex()] = pItem;
	m_pInvenDlg->SetItem( pItem );
}

int CDnInventory::FindItemList( int nItemTableID, char cOption, std::vector<CDnItem *> &pVecResultList )
{
	CDnItem *pItem(NULL);

	int nCount = 0;
	pVecResultList.clear();
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;

		if( pItem && (pItem->GetClassID() == nItemTableID) )
		{
			if( cOption != -1 && pItem->GetOptionIndex() != cOption ) continue;
			pVecResultList.push_back( pItem );
			nCount += pItem->GetOverlapCount();
		}
	}
	return nCount;
}

CDnItem* CDnInventory::FindItem( int nItemTableID, char cOption )
{
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;

		if( pItem && (pItem->GetClassID() == nItemTableID) )
		{
			if( cOption != -1 && pItem->GetOptionIndex() != cOption ) continue;
			return pItem;
		}
	}

	return NULL;
}

int CDnInventory::FindItemSlotIndex( int nItemTableID )
{
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;

		if( pItem && (pItem->GetClassID() == nItemTableID) )
		{
			return iter->first;
		}
	}

	return -1;
}

CDnItem* CDnInventory::FindItemFromSerialID( INT64 nSerialID )
{
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		if( pItem->GetSerialID() == nSerialID ) return pItem;
	}

	return NULL;
}

void CDnInventory::FindItemFromItemType( eItemTypeEnum Type, std::vector<CDnItem *> &pVecResult )
{
	CDnItem *pItem;
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		if( pItem->GetItemType() == Type ) {
			pVecResult.push_back( pItem );
		}
	}
}

void CDnInventory::FindItemFromItemType( eItemTypeEnum Type, std::vector<CDnItem *> &pVecItem, std::vector<int> &pVecIndex )
{
	CDnItem *pItem;
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		if( pItem->GetItemType() == Type ) {
			pVecItem.push_back( pItem );
			pVecIndex.push_back( iter->first );
		}
	}
}

int CDnInventory::GetItemCount( int nItemTableID, char cOption )
{
	int nItemCount(0);
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;

		if( pItem && (pItem->GetClassID() == nItemTableID) )
		{
			if( cOption != -1 && pItem->GetOptionIndex() != cOption ) continue;
			nItemCount += pItem->GetOverlapCount();
		}
	}

	return nItemCount;
}

void CDnInventory::BeginSameSkillCoolTime( const CDnItem *pItem, int nSkillID )
{
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();

	for( iter; iter != iterEnd; ++iter )
	{
		if( iter->second == pItem )
			continue;

		if( iter->second->GetSkillID() == nSkillID )
			iter->second->BeginCoolTime();
	}
}

void CDnInventory::RefreshInventory()
{
	if( !m_pInvenDlg )
	{
		CDebugSet::ToLogFile( "CDnInventory::RefreshInventory, m_pInvenDlg is NULL!" );
		return;
	}

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		m_pInvenDlg->ResetSlot( iter->second );
		m_pInvenDlg->SetItem( iter->second );

#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
		iter->second->SetUseLimit(false);
#endif
	}
}

void CDnInventory::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// 맵 순환하는 거... 최적화의 여지가 있음..
	// 쿨 타임 계산
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();

	for( iter; iter != iterEnd; ++iter )
	{
		iter->second->ProcessCoolTime( LocalTime, fDelta );
	}
}

int CDnInventory::ScanItemFromID( int nItemTableID, std::vector<CDnItem *> *pVecResult )
{
	//ASSERT(pVecResult&&"CDnInventory::ScanItemFromID, pVecResult is NULL!");

	int nCount(0);
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;

		if( pItem->GetClassID() == nItemTableID )
		{
			if( pVecResult )
				pVecResult->push_back( pItem );

			nCount += pItem->GetOverlapCount();
		}
	}

	return nCount;
}

bool CDnInventory::RequestUseItem( int nSlotIndex, char cType )
{
	// 쿨타임이 남아있으면 아이템을 사용할 수 없다.
	INVENTORY_MAP_ITER iter = m_mapInventory.find( nSlotIndex );
	if( m_mapInventory.end() != iter )
	{
#if defined( _FINAL_BUILD )
		if( iter->second->GetCoolTime() == 0.0f )
#else
		if( CGlobalValue::GetInstance().m_bIgnoreCoolTime == true || iter->second->GetCoolTime() == 0.0f )
#endif // #if defined( _FINAL_BUILD )
		{
			switch( cType )
			{
			case ITEMPOSITION_INVEN:		SendUseItem( cType, nSlotIndex, iter->second->GetSerialID() );	break;
			case ITEMPOSITION_CASHINVEN:	SendUseItem( cType, 0, iter->second->GetSerialID() );	break;
			}
			return true;
		}
	}

	return false;
}

void CDnInventory::SetUsableSlotCount( int nCount )
{ 
	if (m_pInvenDlg)
	{
		m_nUsableSlotCount = nCount;
		m_pInvenDlg->SetUseItemCnt( nCount );
	}
}

int CDnInventory::GetRepairInvenPrice()
{
	float fTotalPrice = 0.f;
	CDnItem *pItem(NULL);

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		if( pItem->GetItemType() == ITEMTYPE_WEAPON )
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon *>(pItem);
			if( pWeapon )
				fTotalPrice += pWeapon->GetRepairPrice();
		}
		else if( pItem->GetItemType() == ITEMTYPE_PARTS )
		{
			CDnParts *pParts = dynamic_cast<CDnParts *>(pItem);
			if( pParts )
				fTotalPrice += pParts->GetRepairPrice();
		}
	}
	if( fTotalPrice != 0.f && fTotalPrice < 1.f ) fTotalPrice = 1.f;
	return (int)fTotalPrice;
}
//blondy
/*
bool CDnInventory::IsDisableItem( int iGameMode )
{
	CDnItem *pItem(NULL);
	int iAllowedGameType = 0;

	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
	{
		pItem = iter->second;
		iAllowedGameType = pItem->GetAllowGameType( pItem->GetClassID() );
		pItem->SetDisable( iAllowedGameType|iGameMode?true:false);		
	}
}
*/
//blondy end

//CDnItem *CDnInventory::GetItemFromSerialID( INT64 nSerialID )
//{
//	CDnItem *pItem(NULL);
//
//	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
//	for( ; iter != m_mapInventory.end(); ++iter )
//	{
//		pItem = iter->second;
//		if( !pItem ) continue;
//
//		if( pItem->GetSerialID() == nSerialID )
//			return pItem;
//	}
//
//	return NULL;
//}

int CDnInventory::FindFirstEmptyIndex()
{
	if( m_mapInventory.empty() ) return 0;

	// 제일 마지막에 있는 슬롯 인덱스를 찾고,
	int nLastCashInvenSlotIndex = 0;
	std::map<int,CDnItem*>::reverse_iterator riter = m_mapInventory.rbegin();
	if( riter != m_mapInventory.rend() )
		nLastCashInvenSlotIndex = riter->first;

	// 슬롯개수의 최대치
	int nMaxNumSlot = nLastCashInvenSlotIndex+1;	

	// 임시변수 최대치를 그만큼 늘려둔 후 초기화
	if( (int)m_vecTempCheck.size() < nMaxNumSlot )
		m_vecTempCheck.resize( nMaxNumSlot );
	for( int i = 0; i < (int)m_vecTempCheck.size(); ++i )
		m_vecTempCheck[i] = false;

	// 인벤토리 뒤지면서 아이템 있는 곳 체크
	CDnItem *pItem(NULL);
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
		m_vecTempCheck[iter->first] = true;

	// 마지막으로 제일 앞에 있는 인덱스 찾는다.
	for( int i = 0; i < nMaxNumSlot; ++i )
	{
		if( m_vecTempCheck[i] == false )
			return i;
	}

	// 못찾으면 사이에 빈슬롯없이 인벤토리가 꽉차있다는 것이므로 맥스인덱스+1을 넘겨준다.
	return nMaxNumSlot;
}

static bool IsSubWeapon( CDnWeapon::EquipTypeEnum emType )
{
	switch( emType )
	{
	case CDnWeapon::Sword:
	case CDnWeapon::Axe:
	case CDnWeapon::Hammer:
	case CDnWeapon::SmallBow:
	case CDnWeapon::BigBow:
	case CDnWeapon::CrossBow:
	case CDnWeapon::Staff:
	case CDnWeapon::Mace:
	case CDnWeapon::Flail:
	case CDnWeapon::Wand:		
	case CDnWeapon::Cannon:
	case CDnWeapon::BubbleGun:	
	case CDnWeapon::Chakram:
	case CDnWeapon::Fan:
	case CDnWeapon::Scimiter:
	case CDnWeapon::Dagger:
	case CDnWeapon::Spear:
	case CDnWeapon::KnuckleGear:
		return false;
	case CDnWeapon::Shield:
	case CDnWeapon::Arrow:
	case CDnWeapon::Book:
	case CDnWeapon::Orb:
	case CDnWeapon::Puppet:
	case CDnWeapon::Gauntlet:	
	case CDnWeapon::Glove:		
	case CDnWeapon::Charm:
	case CDnWeapon::Crook:
	case CDnWeapon::Bracelet:
	case CDnWeapon::Claw:
		return true;
	default:
		CDebugSet::ToLogFile( "CDnInventory::IsSubWeapon, default case(%d)", emType );
		ASSERT(0&&"CDnInventory::IsSubWeapon");
		break;
	}
	return false;
}

static bool CompareInvenItem( CDnItem *s1, CDnItem *s2 )
{
	if( !CDnActor::s_hLocalActor ) return false;

	// 둘다 잡템의 경우 - 유일하게 아이템 타입 순서에 따라 정렬되지 않아야 되는거라 위에서 따로 체크한다.
	if( s1->GetItemType() == ITEMTYPE_NORMAL && s1->GetSkillID() == 0 &&
		s2->GetItemType() == ITEMTYPE_NORMAL && s2->GetSkillID() == 0 )
	{
		// 아이템이 같은 종류고 중첩이 된다면, 중첩 수량 높은 것을 앞으로 보낸다.
		if( (s1->GetClassID() == s2->GetClassID()) && (s1->GetMaxOverlapCount() > 1) )
		{
			if( s1->GetOverlapCount() < s2->GetOverlapCount() ) return false;
			else if( s1->GetOverlapCount() > s2->GetOverlapCount() ) return true;
		}

		// ID순서대로 정렬.
		if( s1->GetClassID() < s2->GetClassID() ) return true;
		else if( s1->GetClassID() > s2->GetClassID() ) return false;
		return false;
	}
	// 잡템은 다른 어떤 타입보다도 뒤로 간다.
	else if( s1->GetItemType() == ITEMTYPE_NORMAL && s1->GetSkillID() == 0 ) return false;
	else if( s2->GetItemType() == ITEMTYPE_NORMAL && s2->GetSkillID() == 0 ) return true;

	// 기본적으로 아이템 타입 순서에 따라 정렬
	if( s1->GetItemType() < s2->GetItemType() ) return true;
	else if( s1->GetItemType() > s2->GetItemType() ) return false;

	// 무기, 방어구는 세부타입에 따라서도 정렬. 타입이 같을테니 s1만 검사해도 된다.
	if( s1->GetItemType() == ITEMTYPE_WEAPON )
	{
 		CDnWeapon *pWeapon1 = dynamic_cast<CDnWeapon *>(s1);
 		CDnWeapon *pWeapon2 = dynamic_cast<CDnWeapon *>(s2);

		// 무기의 경우 장비타입이 아니라, 무기인지 서브무기인지만 구분하고, 아래로 넘긴다.
		// s1이 서브무기고, s2가 서브무기가 아닐때,
		if( IsSubWeapon(pWeapon1->GetEquipType()) && !IsSubWeapon(pWeapon2->GetEquipType()) )
			return false;
		if( IsSubWeapon(pWeapon2->GetEquipType()) && !IsSubWeapon(pWeapon1->GetEquipType()) )
			return true;
	}
	else if( s1->GetItemType() == ITEMTYPE_PARTS )
	{
		CDnParts *pParts1 = dynamic_cast<CDnParts *>(s1);
		CDnParts *pParts2 = dynamic_cast<CDnParts *>(s2);
		if( pParts1->GetPartsType() < pParts2->GetPartsType() ) return true;
		else if( pParts1->GetPartsType() > pParts2->GetPartsType() ) return false;
	}

	// 무기는 무기인지 서브무기인지 구분 후, 방어구는 세부 타입까지 같을 경우,
	// 그리고 스킬북은 직업구분 처리를 해야하므로 여기에 포함된다.
	if( s1->GetItemType() == ITEMTYPE_WEAPON || s1->GetItemType() == ITEMTYPE_PARTS || s1->GetItemType() == ITEMTYPE_SKILLBOOK )
	{
		// 먼저 직업제한이 없는 것을 앞으로,
		if( !s1->IsNeedJob() && s2->IsNeedJob() ) return true;
		else if( s1->IsNeedJob() && !s2->IsNeedJob() ) return false;

		// 둘다 필요직업이 있다면 제일 앞에 있는 잡아이디 하나를 골라
		if( s1->IsNeedJob() && s2->IsNeedJob() )
		{
			// Job을 클래스ID로 바꾼다.(1~4)
			int nS1NeedClassID = CDnPlayerState::GetJobToBaseClassID(s1->GetNeedJob(0));
			int nS2NeedClassID = CDnPlayerState::GetJobToBaseClassID(s2->GetNeedJob(0));

			// 두 아이템의 필요 최상위 부모 클래스가 다를 경우
			if( nS1NeedClassID != nS2NeedClassID )
			{
				// 자신계열 먼저, 나머지들 뒤로.
				if( nS1NeedClassID == CDnActor::s_hLocalActor->GetClassID() ) return true;
				else if( nS2NeedClassID == CDnActor::s_hLocalActor->GetClassID() ) return false;
				else if( nS1NeedClassID < nS2NeedClassID ) return true;
				else if( nS1NeedClassID > nS2NeedClassID ) return false;
			}
		}

		// 나머지 제한레벨, 등급 등에 대한 검사는 Weapon과 Parts에만 처리.
		if( s1->GetItemType() != ITEMTYPE_SKILLBOOK )
		{
			// 직업도 같으면, 제한레벨 낮은 순서대로
			if( s1->GetLevelLimit() < s2->GetLevelLimit() ) return true;
			else if( s1->GetLevelLimit() > s2->GetLevelLimit() ) return false;

			// 등급은 높은 것
			if( s1->GetItemRank() < s2->GetItemRank() ) return false;
			else if( s1->GetItemRank() > s2->GetItemRank() ) return true;

			// 강화 높은 것.
			if( s1->GetEnchantLevel() < s2->GetEnchantLevel() ) return false;
			else if( s1->GetEnchantLevel() > s2->GetEnchantLevel() ) return true;

			// 내구도 높은 것. 타입이 같을테니 s1만 검사해도 된다.
			if( s1->GetItemType() == ITEMTYPE_WEAPON )
			{
				CDnWeapon *pWeapon1 = dynamic_cast<CDnWeapon *>(s1);
				CDnWeapon *pWeapon2 = dynamic_cast<CDnWeapon *>(s2);
				float fRate1 = (float)pWeapon1->GetDurability() / pWeapon1->GetMaxDurability();
				float fRate2 = (float)pWeapon2->GetDurability() / pWeapon2->GetMaxDurability();
				if( fRate1 < fRate2 ) return false;
				else if( fRate1 > fRate2 ) return true;
			}
			else if( s1->GetItemType() == ITEMTYPE_PARTS )
			{
				CDnParts *pParts1 = dynamic_cast<CDnParts *>(s1);
				CDnParts *pParts2 = dynamic_cast<CDnParts *>(s2);
				float fRate1 = (float)pParts1->GetDurability() / pParts1->GetMaxDurability();
				float fRate2 = (float)pParts2->GetDurability() / pParts2->GetMaxDurability();
				if( fRate1 < fRate2 ) return false;
				else if( fRate1 > fRate2 ) return true;
			}
		}
	}

	//농장 관련
	if(s1->GetItemType() == ITEMTYPE_SEED && s2->GetItemType() == ITEMTYPE_SEED)
	{
		DNTableFileFormat* pSoxFile = GetDNTable( CDnTableDB::TFARMCULTIVATE );
			
		int nGrade_s1 = pSoxFile->GetFieldFromLablePtr( s1->GetClassID(), "_SecondarySkillClass" )->GetInteger();
		int nLevel_s1 = pSoxFile->GetFieldFromLablePtr( s1->GetClassID(), "_SecondarySkillLevel" )->GetInteger();

		int nGrade_s2 = pSoxFile->GetFieldFromLablePtr( s2->GetClassID(), "_SecondarySkillClass" )->GetInteger();
		int nLevel_s2 = pSoxFile->GetFieldFromLablePtr( s2->GetClassID(), "_SecondarySkillLevel" )->GetInteger();

		if( nGrade_s1 > nGrade_s2 )
			return false;
		else if( nGrade_s1 == nGrade_s2 )
		{
			if( nLevel_s1 > nLevel_s2 )
				return false;
			else
				return true;
		}
		else
			return true;
	}

	// 아이템이 같은 종류고 중첩이 된다면, 중첩 수량 높은 것을 앞으로 보낸다.
	if( (s1->GetClassID() == s2->GetClassID()) && (s1->GetMaxOverlapCount() > 1) )
	{
		if( s1->GetOverlapCount() < s2->GetOverlapCount() ) return false;
		else if( s1->GetOverlapCount() > s2->GetOverlapCount() ) return true;
	}

	// 장비류의 모든 조건이 같다면, 소비템을 비롯해 보옥, 플레이트, 잡템 모두 ID순서대로 정렬.
	if( s1->GetClassID() < s2->GetClassID() ) return true;
	else if( s1->GetClassID() > s2->GetClassID() ) return false;

	return false;
}

bool CDnInventory::PrepareSort( bool bPeriod )
{
	// 정렬 요청중이라면,
	if( m_bSortRequesting )
	{
		ASSERT(0&&"정렬 결과를 받기전에 두번 정렬하지 않습니다.");
		return false;	// 이미 정렬했다는 메세지 뜰텐데, 거의 안일어날 예외상황이니 그냥 둔다.
	}

	int nSortStartIndex = 0;

#if defined(PRE_PERIOD_INVENTORY)
	m_bSortPeriod = bPeriod;
	nSortStartIndex = m_bSortPeriod ? INVENTORYMAX : 0;
	m_mapNotSortItem.clear();
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	m_vecSortItem.clear();

	// 맵 순환
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();

	for( iter; iter != iterEnd; ++iter )
	{
#if defined(PRE_PERIOD_INVENTORY)
		if( ( false == m_bSortPeriod && INVENTORYMAX <= iter->first ) 
		 || ( true == m_bSortPeriod && INVENTORYMAX > iter->first ) )
		{
			m_mapNotSortItem.insert( std::make_pair( iter->first, iter->second ) );
			continue;
		}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

		m_vecSortItem.push_back(iter->second);
	}

	// 아이템이 한개도 없으면,
	if( (int)m_vecSortItem.size() == 0 )
		return false;

	std::sort( m_vecSortItem.begin(), m_vecSortItem.end(), CompareInvenItem );

	bool bChanged = false;
	for( BYTE i = 0; i < (BYTE)m_vecSortItem.size(); ++i )
	{
		m_SortSlotInfo[i].cNew = i;

		m_SortSlotInfo[i].cCurrent = m_vecSortItem[i]->GetSlotIndex();

		if( m_SortSlotInfo[i].cNew != m_SortSlotInfo[i].cCurrent - nSortStartIndex )
			bChanged = true;
	}

	// 정렬 후 슬롯번호가 하나도 바뀌지 않았다면,
	if( bChanged == false )
		return false;

	m_bSortRequesting = true;
	return true;
}

bool CDnInventory::Sort( bool bServerSucceed )
{
	// 정렬 요청중이 아니라면,
	if( m_bSortRequesting == false )
	{
		ASSERT(0&&"왜 정렬요청도 안했는데 정렬이 수행되나.");
		return false;
	}

	if( !bServerSucceed )
	{
		m_bSortRequesting = false;
		return false;
	}

	// 요청 후 인벤 바뀌었는지 검증
	for( BYTE i = 0; i < (BYTE)m_vecSortItem.size(); ++i )
	{
		int nSlotIndex = m_vecSortItem[i]->GetSlotIndex();
		if( m_mapInventory.count( nSlotIndex ) != 0 &&  m_mapInventory[ nSlotIndex ] != m_vecSortItem[ i ] )		// Key가 없는 경우 디폴트로 NULL 이 들어가서 크래시 나는것 수정.
		{
			m_bSortRequesting = false;
			return false;
		}
	}

	// 인벤토리 초기화. 슬롯도 초기화.
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	for( ; iter != m_mapInventory.end(); ++iter )
		m_pInvenDlg->ResetSlot( iter->second );
	m_mapInventory.clear();

	int nSortStartIndex = 0;
#if defined(PRE_PERIOD_INVENTORY)
	nSortStartIndex = m_bSortPeriod ? INVENTORYMAX : 0;
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	// 맵을 교환정보에 맞게 변환
	for( BYTE i = 0; i < (BYTE)m_vecSortItem.size(); ++i )
	{
		m_vecSortItem[i]->SetSlotIndex( nSortStartIndex + i );
		m_mapInventory[nSortStartIndex + i] = m_vecSortItem[i];

		m_pInvenDlg->SetItem(m_vecSortItem[i]);
	}

#if defined(PRE_PERIOD_INVENTORY)
	iter = m_mapNotSortItem.begin();
	for( ; iter != m_mapNotSortItem.end(); ++iter )
	{
		iter->second->SetSlotIndex( iter->first );
		m_mapInventory[iter->first] = iter->second;
		m_pInvenDlg->SetItem( iter->second );
	}
#endif	// #if defined(PRE_PERIOD_INVENTORY)

	m_bSortRequesting = false;
	return true;
}

CDnInventory::INVENTORY_MAP& CDnInventory::GetInventoryItemList()
{
	return m_mapInventory;
}

static bool CompareCashInvenItem( CDnItem *s1, CDnItem *s2 )
{
	if( !CDnActor::s_hLocalActor ) return false;

	// 기본적으로 Weapon, Parts가 제일 뒤로 가도록
	if( s1->GetItemType() == ITEMTYPE_WEAPON && s2->GetItemType() == ITEMTYPE_PARTS ) return true;
	else if( s1->GetItemType() == ITEMTYPE_PARTS && s2->GetItemType() == ITEMTYPE_WEAPON ) return false;
	else if( s1->GetItemType() != ITEMTYPE_WEAPON && s2->GetItemType() == ITEMTYPE_WEAPON ) return true;
	else if( s1->GetItemType() == ITEMTYPE_WEAPON && s2->GetItemType() != ITEMTYPE_WEAPON ) return false;
	else if( s1->GetItemType() != ITEMTYPE_PARTS && s2->GetItemType() == ITEMTYPE_PARTS ) return true;
	else if( s1->GetItemType() == ITEMTYPE_PARTS && s2->GetItemType() != ITEMTYPE_PARTS ) return false;

	// 무기, 방어구는 세부타입에 따라서도 정렬. 타입이 같을테니 s1만 검사해도 된다.
	if( s1->GetItemType() == ITEMTYPE_WEAPON )
	{
		CDnWeapon *pWeapon1 = dynamic_cast<CDnWeapon *>(s1);
		CDnWeapon *pWeapon2 = dynamic_cast<CDnWeapon *>(s2);

		// 무기의 경우 장비타입이 아니라, 무기인지 서브무기인지만 구분하고, 아래로 넘긴다.
		// s1이 서브무기고, s2가 서브무기가 아닐때,
		if( IsSubWeapon(pWeapon1->GetEquipType()) && !IsSubWeapon(pWeapon2->GetEquipType()) )
			return false;
		if( IsSubWeapon(pWeapon2->GetEquipType()) && !IsSubWeapon(pWeapon1->GetEquipType()) )
			return true;
	}
	else if( s1->GetItemType() == ITEMTYPE_PARTS )
	{
		CDnParts *pParts1 = dynamic_cast<CDnParts *>(s1);
		CDnParts *pParts2 = dynamic_cast<CDnParts *>(s2);
		if( pParts1->GetPartsType() < pParts2->GetPartsType() ) return true;
		else if( pParts1->GetPartsType() > pParts2->GetPartsType() ) return false;
	}

	// 무기는 무기인지 서브무기인지 구분 후, 방어구는 세부 타입까지 같을 경우,
	// 그리고 스킬북은 직업구분 처리를 해야하므로 여기에 포함된다.
	if( s1->GetItemType() == ITEMTYPE_WEAPON || s1->GetItemType() == ITEMTYPE_PARTS || s1->GetItemType() == ITEMTYPE_SKILLBOOK )
	{
		// 먼저 직업제한이 없는 것을 앞으로,
		if( !s1->IsNeedJob() && s2->IsNeedJob() ) return true;
		else if( s1->IsNeedJob() && !s2->IsNeedJob() ) return false;

		// 둘다 필요직업이 있다면 제일 앞에 있는 잡아이디 하나를 골라
		if( s1->IsNeedJob() && s2->IsNeedJob() )
		{
			// Job을 클래스ID로 바꾼다.(1~4)
			int nS1NeedClassID = CDnPlayerState::GetJobToBaseClassID(s1->GetNeedJob(0));
			int nS2NeedClassID = CDnPlayerState::GetJobToBaseClassID(s2->GetNeedJob(0));

			// 두 아이템의 필요 최상위 부모 클래스가 다를 경우
			if( nS1NeedClassID != nS2NeedClassID )
			{
				// 자신계열 먼저, 나머지들 뒤로.
				if( nS1NeedClassID == CDnActor::s_hLocalActor->GetClassID() ) return true;
				else if( nS2NeedClassID == CDnActor::s_hLocalActor->GetClassID() ) return false;
				else if( nS1NeedClassID < nS2NeedClassID ) return true;
				else if( nS1NeedClassID > nS2NeedClassID ) return false;
			}
		}

		// 나머지 제한레벨, 등급 등에 대한 검사는 Weapon과 Parts에만 처리.
		if( s1->GetItemType() != ITEMTYPE_SKILLBOOK )
		{
			// 직업도 같으면, 제한레벨 낮은 순서대로
			if( s1->GetLevelLimit() < s2->GetLevelLimit() ) return true;
			else if( s1->GetLevelLimit() > s2->GetLevelLimit() ) return false;

			// 등급은 높은 것
			if( s1->GetItemRank() < s2->GetItemRank() ) return false;
			else if( s1->GetItemRank() > s2->GetItemRank() ) return true;
		}
	}

	// 장비류의 모든 조건이 같다면, 소비템을 비롯해 보옥, 플레이트, 잡템 모두 ID순서대로 정렬.
	if( s1->GetClassID() < s2->GetClassID() ) return true;
	else if( s1->GetClassID() > s2->GetClassID() ) return false;

	// 아이템이 같은 종류고 기간이 남아있다면, 기간 적게 남은 것을 앞으로 보낸다.
	if( s1->GetClassID() == s2->GetClassID() )
	{
		if( s1->IsEternityItem() == true && s2->IsEternityItem() == false ) return false;
		else if( s1->IsEternityItem() == false && s2->IsEternityItem() == true ) return true;
		else if( s1->IsEternityItem() == false && s2->IsEternityItem() == false )
		{
			if( s1->GetExpireDate() < s2->GetExpireDate() ) return true;
			else if( s1->GetExpireDate() > s2->GetExpireDate() ) return false;
		}
	}

	// 아이템이 같은 종류고 기간 남은 것도 동일하면서 중첩이 된다면, 중첩 수량 높은 것을 앞으로 보낸다.
	if( (s1->GetClassID() == s2->GetClassID()) && (s1->GetMaxOverlapCount() > 1) )
	{
		if( s1->GetOverlapCount() < s2->GetOverlapCount() ) return false;
		else if( s1->GetOverlapCount() > s2->GetOverlapCount() ) return true;
	}

	return false;
}

bool CDnInventory::SortCashInventory()
{
	std::vector<CDnItem*> vecSortItem;
	vecSortItem.clear();

	// 아이템이 한개도 없으면,
	if( (int)m_mapInventory.size() == 0 )
		return false;

	// 맵 순환
	INVENTORY_MAP_ITER iter = m_mapInventory.begin();
	INVENTORY_MAP_ITER iterEnd = m_mapInventory.end();
	vecSortItem.reserve(m_mapInventory.size());
	for( iter; iter != iterEnd; ++iter )
		vecSortItem.push_back(iter->second);

	std::sort( vecSortItem.begin(), vecSortItem.end(), CompareCashInvenItem );

	bool bChanged = false;
	for( int i = 0; i < (int)vecSortItem.size(); ++i )
	{
		if( vecSortItem[i]->GetSlotIndex() != i )
		{
			bChanged = true;
			break;
		}
	}

	// 정렬 후 슬롯번호가 하나도 바뀌지 않았다면,
	if( bChanged == false )
		return false;

	// 뒤에서부터 전부 다 뺀 후(이렇게 빼야 뒤에 남는 페이지 없이 새로 갱신할 수 있다.)
	std::map<int,CDnItem*>::reverse_iterator riter = m_mapInventory.rbegin();
	while( riter != m_mapInventory.rend() )
	{
		INVENTORY_MAP_ITER iter2 = m_mapInventory.find( riter->first );
		if( iter2 != m_mapInventory.end() )
		{
			m_pInvenDlg->ResetSlot( iter2->second );
			m_mapInventory.erase( iter2 );
		}
	}

	// 정렬된 결과로 재구성
	for( int i = 0; i < (int)vecSortItem.size(); ++i )
	{
		vecSortItem[i]->SetSlotIndex(i);
		m_mapInventory[i] = vecSortItem[i];
		m_pInvenDlg->SetItem(vecSortItem[i]);
	}

	return true;
}