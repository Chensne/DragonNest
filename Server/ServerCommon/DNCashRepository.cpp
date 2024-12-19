
#include "stdafx.h"
#include "DNCashRepository.h"
#include "DNCashItem.h"
#include "DNGameDataManager.h"
#include "TimeSet.h"
#if defined( PRE_ADD_VIP_FARM )
#include "DNUserSession.h"
#endif // #if defined( PRE_ADD_VIP_FARM )

CDNEffectRepository::CDNEffectRepository( CDNUserSession* pSession ):m_pSession(pSession)
{
}

CDNEffectRepository::~CDNEffectRepository()
{
	for( std::map<INT64,CDNEffectItem*>::iterator itor=m_mRepository.begin() ; itor!=m_mRepository.end() ; )
	{
		CDNEffectItem* pCashItem = (*itor).second;
		itor = m_mRepository.erase( itor );
		SAFE_DELETE( pCashItem );
	}
}

bool CDNEffectRepository::Add( INT64 biItemSerial, int iItemID, __time64_t tExpireDate /*=0*/ )
{
	TItemData* pItemData = g_pDataManager->GetItemData( iItemID );
	if( !pItemData )
	{
		_ASSERT(0);
		return false;
	}

	CDNEffectItem* pEffectItem = NULL;
 	switch( pItemData->nType )
	{
	case ITEMTYPE_INVENTORY_SLOT:
	case ITEMTYPE_INVENTORY_SLOT_EX:
		{
			pEffectItem = new CDNInventoryExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_GLYPH_SLOT:
	case ITEMTYPE_PERIOD_PLATE:
	case ITEMTYPE_PERIOD_PLATE_EX:
		{
			pEffectItem = new CDNGlyphExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_WAREHOUSE_SLOT:
	case ITEMTYPE_WAREHOUSE_SLOT_EX:
		{
			pEffectItem = new CDNWareHouseInventoryExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_GESTURE:
		{
			pEffectItem = new CDNGestureAddItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_GUILDWARE_SLOT:
		{
			pEffectItem = new CDNGuildWareSlotExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}	
		break;

	case ITEMTYPE_UNION_MEMBERSHIP:
		{
			pEffectItem = new CDNUnionMembershipItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

#if defined( PRE_ADD_VIP_FARM )
	case ITEMTYPE_FARM_VIP:
		{
			pEffectItem = new CDNFarmVipItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;
#endif // #if defined( PRE_ADD_VIP_FARM )

	case ITEMTYPE_PERIOD_APPELLATION:
		{
			pEffectItem = new CDNPeriodAppellationItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_EXPAND_SKILLPAGE:
		{
			pEffectItem = new CDNExpandSkillPageItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;
		
	case ITEMTYPE_SOURCE:
		{
			pEffectItem = new CDNSourceItem(m_pSession, biItemSerial, pItemData, tExpireDate);
		}
		break;

#if defined( PRE_ADD_NAMEDITEM_SYSTEM )
	case ITEMTYPE_GLOBAL_PARTY_BUFF:
		{
			pEffectItem = new CDNEffectSkillItem(m_pSession, biItemSerial, pItemData, tExpireDate);
		}
		break;
#endif
#if defined( PRE_ADD_BESTFRIEND )
	case ITEMTYPE_BESTFRIENDBUFFITEM:
		{
			pEffectItem = new CDNEffectBestFriendBufftem(m_pSession, biItemSerial, pItemData, tExpireDate);
		}
		break;
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	case ITEMTYPE_TOTALSKILLLEVEL_SLOT:
		{
			pEffectItem = new CDNEffectTotalLevelSkilltem(m_pSession, biItemSerial, pItemData, tExpireDate);
		}
		break;
#endif
#if defined( PRE_ADD_NEWCOMEBACK )
	case ITEMTYPE_COMEBACK_EFFECTITEM:
		{
			pEffectItem = new CDNEffectComebackItem(m_pSession, biItemSerial, pItemData, tExpireDate);
		}
		break;
#endif
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_PERIOD_TALISMAN_EX:
		{
			pEffectItem = new CDNTalismanExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;
#endif
#if defined(PRE_PERIOD_INVENTORY)
	case ITEMTYPE_PERIOD_INVENTORY:
		{
			pEffectItem = new CDNPeriodInventoryExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;

	case ITEMTYPE_PERIOD_WAREHOUSE:
		{
			pEffectItem = new CDNPeriodWarehouseExtendItem( m_pSession, biItemSerial, pItemData, tExpireDate );
		}
		break;
#endif	// #if defined(PRE_PERIOD_INVENTORY)
	}

	if( !pEffectItem )
	{
		_ASSERT(0);
		return false;
	}

	if( !pEffectItem->Add() )
	{
		_ASSERT(0);
		delete pEffectItem;
		return false;
	}

	std::pair<std::map<INT64,CDNEffectItem*>::iterator,bool> Ret = m_mRepository.insert( std::make_pair(biItemSerial,pEffectItem) );
	if( !Ret.second )
	{
		_ASSERT(0);
		delete pEffectItem;
		return false;
	}

	return true;
}

bool CDNEffectRepository::Remove( INT64 biItemSerial )
{
	std::map<INT64,CDNEffectItem*>::iterator itor = m_mRepository.find( biItemSerial );
	if( itor == m_mRepository.end() )
	{
		_DANGER_POINT();
		return false;
	}

	CDNEffectItem* pCashItem = (*itor).second;
	if( !pCashItem->Remove() )
	{
		_ASSERT(0);
		return false;
	}

	m_mRepository.erase( itor );
	delete pCashItem;
	return true;
}

#if defined (PRE_ADD_VIP_FARM)
void CDNEffectRepository::SendEffectItem()
{
	for each (std::map<INT64, CDNEffectItem*>::value_type v in m_mRepository)
	{
		const TItemData* pItemData = v.second->GetItemData();
		switch (pItemData->nType)
		{
#if defined (PRE_ADD_VIP_FARM)
			case ITEMTYPE_FARM_VIP:
				{
					m_pSession->SendEffectItemInfo( pItemData->nItemID, v.second->GetExpireDate() );
					break;
				}
				break;
#endif // #if defined (PRE_ADD_VIP_FARM)
		}
	}
}
bool CDNEffectRepository::bIsExpiredItem( int iItemType )
{
	for( std::map<INT64,CDNEffectItem*>::iterator itor=m_mRepository.begin() ; itor!=m_mRepository.end() ; ++itor )
	{
		const TItemData* pItemData = (*itor).second->GetItemData();
		if( pItemData && pItemData->nType == iItemType )
		{
			CTimeSet EndTime( (*itor).second->GetExpireDate(), true );
			CTimeSet CurTime;
			if( CurTime.GetTimeT64_LC() > EndTime.GetTimeT64_LC() )
				return true;

			return false;
		}
	}

	return true;
}
#endif // #if defined (PRE_ADD_VIP_FARM)

void CDNEffectRepository::DelGuildRewardItem()
{
	for( std::map<INT64,CDNEffectItem*>::iterator itor=m_mRepository.begin() ; itor!=m_mRepository.end() ;)
	{
		const TItemData* pItemData = (*itor).second->GetItemData();
		if( pItemData && pItemData->cReversion == ITEMREVERSION_GUILD )
		{
			// 길드귀속이면 삭제(현재 제스쳐만 있음 다른 아이템도 삭제가 필요하면 해당아이템 OnRemove에 코드 작성 필요)
			itor->second->Remove();
			CDNEffectItem* pCashItem = (*itor).second;
			itor = m_mRepository.erase( itor );
			SAFE_DELETE( pCashItem );
		}
		else
			 ++itor;
	}
}

#if defined( PRE_ADD_BESTFRIEND )
void CDNEffectRepository::DelEffectItemType( char cType )
{
	for( std::map<INT64,CDNEffectItem*>::iterator itor=m_mRepository.begin() ; itor!=m_mRepository.end() ;)
	{
		const TItemData* pItemData = (*itor).second->GetItemData();
		if( pItemData && pItemData->nType == cType )
		{			
			itor->second->Remove();
			CDNEffectItem* pCashItem = (*itor).second;
			itor = m_mRepository.erase( itor );
			SAFE_DELETE( pCashItem );
		}
		else
			++itor;
	}
}
#endif