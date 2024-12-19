
#include "Stdafx.h"
#include "GrowingArea.h"
#include "DNFarmGameRoom.h"
#include "DnFarmGameTask.h"
#include "DNUserSession.h"
#include "GrowingAreaStateNone.h"
#include "GrowingAreaStatePlanting.h"
#include "GrowingAreaStateGrowing.h"
#include "GrowingAreaStateCompleted.h"
#include "GrowingAreaStateHarvesting.h"
#include "GrowingAreaStateHarvested.h"
#include "DNDBConnection.h"
#include "DNGameDataManager.h"
#include "DnDropItem.h"
#include "SecondarySkillRepository.h"
#include "SecondarySkill.h"
#include "DNWorldUserState.h"
#include "DNMasterConnectionManager.h"
#include "DNFarmUserSession.h"
#include "DNGameDataManager.h"
#include "DNMissionSystem.h"
#include "TimeSet.h"
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
#include "DnPlayerActor.h"
#include "DnStateBlow.h"
#endif

CGrowingArea::CGrowingArea( CDNFarmGameRoom* pGameRoom, const int iIndex, SOBB* pOBB, bool bPrivate/*=false*/, INT64 biCharacterDBID/*=0*/ )
:m_pFarmGameRoom(pGameRoom),m_iIndex(iIndex),m_OBB(pOBB?*pOBB:SOBB())
,m_pState(new CFarmAreaStateNone(this)),m_dwStateElapsedTick(0),m_pSeed(NULL)
{
	_ASSERT( dynamic_cast<CDnFarmGameTask*>(m_pFarmGameRoom->GetGameTask()) );
	m_pFarmGameTask = reinterpret_cast<CDnFarmGameTask*>(m_pFarmGameRoom->GetGameTask());
	m_lChangeStateLock	= 0;
	m_uiRandSeed		= 0;
	m_bPrivateArea				= bPrivate;
	m_biPrivateCharacterDBID	= biCharacterDBID;
}

CGrowingArea::~CGrowingArea()
{
	SAFE_DELETE( m_pState );
	SAFE_DELETE( m_pSeed );
}

void CGrowingArea::Process( float fDelta )
{
	m_dwStateElapsedTick += static_cast<DWORD>(fDelta*1000);
		
	_DeleteProcess();
	m_pState->Process( fDelta );
}

void CGrowingArea::_DeleteProcess()
{
	if( m_vDeleteState.empty() )
		return;

	for( UINT i=0 ; i<m_vDeleteState.size() ; ++i )
		delete m_vDeleteState[i];
	m_vDeleteState.clear();
}

bool CGrowingArea::CheckPlantItem( CDNUserSession* pSession, CSFarmPlant* pPacket, const TItem*& pItem, const TItem*& pAttachItem )
{
	pItem = pAttachItem = NULL;

	// 아이템 검사
	pItem = pSession->GetItem()->GetInventory( pPacket->cSeedInvenIndex );
	if( pItem == NULL )
		return false;

	const TItemData* pItemData = g_pDataManager->GetItemData( pItem->nItemID );
	if( pItemData == NULL )
		return false;

	if( pItemData->nType != ITEMTYPE_SEED )
		return false;

	TFarmCultivateTableData* pCultivateTable = g_pDataManager->GetFarmCultivateTableData( pItem->nItemID );
	if( pCultivateTable == NULL )
		return false;

	if( pPacket->cCount > 0 )
	{
		int iItemCount = 0;
		for( int i=0 ; i<pPacket->cCount ; ++i )
		{
			iItemCount += pPacket->AttachItems[i].cCount;
		}

		// 필요개수 체크
		if( iItemCount != pCultivateTable->iCatalystCount )
			return false;

		// 모든 아이템의 시리얼이 틀려야한다.
		std::map<INT64,int> mDuplicate;
		for( int i=0 ; i<pPacket->cCount ; ++i )
		{
			mDuplicate.insert( std::make_pair(pPacket->AttachItems[i].biSerial,0) );
		}
		if( mDuplicate.size() != pPacket->cCount )
			return false;
	}

	// AttachItem
	int iOrgItemID = 0;

	for( int i=0 ; i<pPacket->cCount ; ++i )
	{
		pAttachItem = pSession->GetItem()->GetCashInventory( pPacket->AttachItems[i].biSerial );
		if( pAttachItem == NULL )
			return false;

		if( i == 0 )
		{
			iOrgItemID = pAttachItem->nItemID;

			const TItemData* pAttachItemData = g_pDataManager->GetItemData( pAttachItem->nItemID );
			if( pAttachItemData == NULL )
				return false;

			switch( pAttachItemData->nType )
			{
				case ITEMTYPE_GROWING_BOOST:	// 성장촉진제
				{
					break;
				}
				default:
				{
					return false;
				}
			}
		}
		else
		{
			// 모든 아이템은 같은 아이템ID 여야한다.
			if( iOrgItemID != pAttachItem->nItemID )
				return false;
		}
	}

	// 완료일때는 START 일때 미리 설정해 둔 정보와 같은지 비교해본다.
	if( pPacket->ActionType == Farm::ActionType::COMPLETE )
	{
		if( m_pSeed->GetSeedItemID() != pItem->nItemID )
			return false;
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		int nTotalLevel = pSession->GetintTotalLevelSkillEffect(TotalLevelSkill::Common::GrowingBoost);
		if( g_pDataManager->GetItemMainType(nTotalLevel) != ITEMTYPE_GROWING_BOOST )
		{
			nTotalLevel = 0;
		}		
		if( m_pSeed->GetFirstAttachItemID() != nTotalLevel )
		{
			if( m_pSeed->GetFirstAttachItemID() != (pAttachItem ? pAttachItem->nItemID : 0) )
				return false;
		}		
#else
		if( m_pSeed->GetFirstAttachItemID() != (pAttachItem ? pAttachItem->nItemID : 0) )
			return false;
#endif
	}

	// 보조스킬 검사
	CSecondarySkill* pSecondarySkill = pSession->GetSecondarySkillRepository()->Get( SecondarySkill::SubType::CultivationSkill );
	if( pSecondarySkill == NULL )
		return false;

	if( pCultivateTable->RequiredSkillGrade > pSecondarySkill->GetGrade() )
		return false;
	if( pCultivateTable->RequiredSkillGrade == pSecondarySkill->GetGrade() )
	{
		if( pCultivateTable->iRequiredSkillLevel > pSecondarySkill->GetLevel() )
			return false;
	}

	return true;
}

bool CGrowingArea::CheckHarvestItem( CDNUserSession* pSession, CSFarmHarvest* pPacket )
{
	// 아이템 검사
	const TItem* pItem = pSession->GetItem()->GetInventory( pPacket->cNeedItemInvenIndex );
	if( pItem == NULL )
		return false;

	TFarmCultivateTableData* pCultivateTable = g_pDataManager->GetFarmCultivateTableData( m_pSeed->GetSeedItemID() );
	if( pCultivateTable == NULL )
		return false;

	if( pItem->nItemID != pCultivateTable->iHarvestNeedItemID )
		return false;

	return true;
}

bool CGrowingArea::CheckAddWaterItem( CDNUserSession* pSession, const CSFarmAddWater* pPacket, const TItem*& pItem )
{
	pItem = NULL;

	// 아이템 검사
	switch( pPacket->cInvenType )
	{
		case ITEMPOSITION_INVEN:
		{
			pItem = pSession->GetItem()->GetInventory( pPacket->cWaterItemInvenIndex );
			break;
		}
		case ITEMPOSITION_CASHINVEN:
		{
			pItem = pSession->GetItem()->GetCashInventory( pPacket->biWaterItemItemSerial );
			break;
		}
		default:
		{
			g_Log.Log(LogType::_FARM, pSession, L"CGrowingArea::CheckAddWaterItem Invalid pPacket->cInvenType\n");
			return false;
		}
	}
	if( pItem == NULL )
	{
		g_Log.Log(LogType::_FARM, pSession, L"CGrowingArea::CheckAddWaterItem Invalid pItem\n");
		return false;
	}

	const TItemData* pItemData = g_pDataManager->GetItemData( pItem->nItemID );
	if( pItemData == NULL )
	{
		g_Log.Log(LogType::_FARM, pSession, L"CGrowingArea::CheckAddWaterItem Invalid pItemData\n");
		return false;
	}

	switch( pItemData->nType )
	{
		case ITEMTYPE_WATER:
		case ITEMTYPE_WATERBOTTLE:
		{
			break;
		}
		default:
		{
			g_Log.Log(LogType::_FARM, pSession, L"CGrowingArea::CheckAddWaterItem Invalid pItemData->nType\n");
			return false;
		}
	}

	return true;
}

void CGrowingArea::ProcessStartPlant( CDNUserSession* pSession, CSFarmPlant* pPacket )
{
	do 
	{
#if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_FARM_DOWNSCALE )
		if( GetFarmGameRoom()->GetAttr()&Farm::Attr::Vip )
#else
		if( GetFarmGameRoom()->GetAttr() == Farm::Attr::Vip )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		{
			if( pSession->bIsFarmVip() == false )
			{
				pSession->SendFarmPlantSeed( ERROR_FARM_INVALID_VIP, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
				return;
			}
		}
#endif // #if defined( PRE_ADD_VIP_FARM )

#if defined( PRE_ADD_FARM_DOWNSCALE )
		if( GetFarmGameRoom()->GetAttr()&Farm::Attr::DownScale && bIsPrivateArea() == false )
		{
			pSession->SendFarmPlantSeed( ERROR_FARM_CANT_START_PLANTSTATE_DOWNSCALEFARM, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
			return;
		}
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		if( GetFarmGameRoom()->GetAttr()&Farm::Attr::GuildChampion && bIsPrivateArea() == false )
		{
			TGuildUID GuildUID = g_pMasterConnectionManager->GetPreWinGuildUID( pSession->GetWorldSetID() );
			if( GuildUID.IsSet() == false  )
			{
				pSession->SendFarmPlantSeed( ERROR_GENERIC_MASTERCON_NOT_FOUND, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
				return;
			}

			if( GuildUID != pSession->GetGuildUID() )
			{
				pSession->SendFarmPlantSeed( ERROR_FARM_INVALUD_CHAMPIONGUILD, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
				return;
			}
		}
		if( CanStartPlantSeed( pSession ) == false )
			break;

		if( static_cast<CDNFarmUserSession*>(pSession)->CheckActiveFieldCount() == false )
		{
			pSession->SendFarmPlantSeed( ERROR_FARM_CANT_START_PLANTSTATE_MAXFIELDCOUNT, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
			return;
		}

		if( bIsPrivateArea() == true )
		{
			if( GetFarmGameTask()->CheckActivePrivateFieldCount( pSession ) == false )
			{
				pSession->SendFarmPlantSeed( ERROR_FARM_CANT_START_PLANTSTATE_MAXPRIVATEFIELDCOUNT, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
				return;
			}
		}
		// 아이템 검사
		const TItem* pItem;
		const TItem* pAttachItem;
		if( CheckPlantItem( pSession, pPacket, pItem, pAttachItem ) == false )
			break;

		m_pSeed = new CPlantSeed( pItem->nItemID, 0, pSession->GetCharacterDBID(), pSession->GetCharacterName() );
		if( m_pSeed == NULL || m_pSeed->bIsValid() == false )
			break;
		if( pAttachItem )
		{
			if( m_pSeed->CanAttach( pAttachItem, true ) == false )
			{
				SAFE_DELETE( m_pSeed );
				break;
			}

			m_pSeed->AttachItem( pAttachItem->nItemID );
		}

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		int nTotalLevel = pSession->GetintTotalLevelSkillEffect(TotalLevelSkill::Common::GrowingBoost);
		if( g_pDataManager->GetItemMainType(nTotalLevel) == ITEMTYPE_GROWING_BOOST )
		{
			m_pSeed->AttachItem( nTotalLevel );
		}		
#endif
		if( GetFarmGameRoom()->GetAttr()&Farm::Attr::GuildChampion && bIsPrivateArea() == false )
		{
			// 나중에 성장촉진제가 들어
			TFarmCultivateTableData* pCultivateData = g_pDataManager->GetFarmCultivateTableData( pItem->nItemID );
			if( pCultivateData )
			{
				__time64_t FinalStartTime = g_pMasterConnectionManager->GetGuildWarFinalStatTime( pSession->GetWorldSetID() );
				if( FinalStartTime > 0 )
				{
					CTimeSet CurTime;
					__time64_t tCurTime = CurTime.GetTimeT64_LC();
					if( tCurTime+m_pSeed->GetRequiredTimeSec() > FinalStartTime )
					{
						SAFE_DELETE( m_pSeed );
						pSession->SendFarmPlantSeed( ERROR_FARM_CANT_START_PLANTSTATE_GUILDCHAMPION_TIMELIMIT, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
						return;
					}
				}
			}
		}
		ChangeState( Farm::AreaState::PLANTING );
		return;
	}while( false );
	
	// 에러
	pSession->SendFarmPlantSeed( ERROR_FARM_CANT_START_PLANTSTATE, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
}

void CGrowingArea::ProcessCancelPlant( CDNUserSession* pSession, CSFarmPlant* pPacket )
{
	if( CanCancelPlantSeed( pSession ) == false )
	{
		pSession->SendFarmPlantSeed( ERROR_FARM_CANT_CANCEL_PLANTSTATE, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
		return;
	}

	ChangeState( Farm::AreaState::NONE );
}

void CGrowingArea::QueryCompletePlant( CDNUserSession* pSession, CSFarmPlant* pPacket )
{
	do 
	{
		if( CanCompletePlantSeed( pSession->GetCharacterDBID() ) == false )
			break;

		// 아이템 검사
		const TItem* pItem			= NULL;
		const TItem* pAttachItem	= NULL;
		if( CheckPlantItem( pSession, pPacket, pItem, pAttachItem ) == false )
			break;

		CDNDBConnection* pDBCon = NULL;
		BYTE cThreadID;
		GetFarmGameRoom()->CopyDBConnectionInfo( pDBCon, cThreadID );

		CSecondarySkill* pSecondarySkill = pSession->GetSecondarySkillRepository()->Get( SecondarySkill::SubType::CultivationSkill );
		if( pSecondarySkill == NULL )
			break;
		TSecondarySkillLevelTableData* pTableData = g_pDataManager->GetSecondarySkillLevelTableData( pSecondarySkill->GetSkillID(), pSecondarySkill->GetGrade(), pSecondarySkill->GetLevel() );
		if( pTableData == NULL )
			break;

#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		for(int i=0;i<pPacket->cCount;i++)
		{
			pPacket->AttachItems[i].bRemoveItem = true;
		}

		int nTotalLevel = pSession->GetintTotalLevelSkillEffect(TotalLevelSkill::Common::GrowingBoost);
		if( g_pDataManager->GetItemMainType(nTotalLevel) == ITEMTYPE_GROWING_BOOST )
		{
			pPacket->AttachItems[pPacket->cCount].biSerial = nTotalLevel;
			pPacket->AttachItems[pPacket->cCount].cCount = 1;
			pPacket->AttachItems[pPacket->cCount].bRemoveItem = false;
			pPacket->cCount++;			
		}
#endif

		static_cast<CFarmAreaStatePlanting*>(m_pState)->SetQueryFlag( true );
		if( bIsPrivateArea() == true )
		{
			pDBCon->QueryAddFieldForCharacter( cThreadID, GetFarmGameRoom()->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), GetFarmGameRoom()->GetRoomID(), this, pTableData->iParam,
											   pPacket->cSeedInvenIndex, pItem->nSerial, pPacket, pSession->GetMapIndex(), pSession->GetIpW() );
		}
		else
		{
			pDBCon->QueryAddField( cThreadID, pSession, this, pTableData->iParam, pPacket->cSeedInvenIndex, pItem->nSerial, pPacket );
		}
		return;
	}while( false );

	pSession->SendFarmPlantSeed( ERROR_FARM_CANT_COMPLETE_PLANTSTATE, pPacket->ActionType, pPacket->iAreaIndex, 0, 0 );
}

void CGrowingArea::ProcessStartHarvest( CDNUserSession* pSession, CSFarmHarvest* pPacket )
{
	do 
	{
		if( CanStartHarvest( pSession ) == false )
			break;

		// 아이템 검사
		if( CheckHarvestItem( pSession, pPacket ) == false )
			break;

		ChangeState( Farm::AreaState::HARVESTING, pSession );
		return;
	}while( false );

	// 에러
	pSession->SendFarmHarvest( ERROR_FARM_CANT_START_HARVESTSTATE, pPacket->ActionType, pPacket->iAreaIndex );
}

void CGrowingArea::ProcessCancelHarvest( CDNUserSession* pSession, CSFarmHarvest* pPacket )
{
	if( CanCancelHarvest( pSession ) == false )
	{
		pSession->SendFarmHarvest( ERROR_FARM_CANT_CANCEL_HARVESTSTATE, pPacket->ActionType, pPacket->iAreaIndex );
		return;
	}

	ChangeState( Farm::AreaState::COMPLETED );
}

#if defined( PRE_ADD_FARM_DOWNSCALE )
void CGrowingArea::QueryCompleteHarvest( CDNUserSession* pSession, CSFarmHarvest* pPacket, bool bAuthHarvest/*=false*/ )
#else
void CGrowingArea::QueryCompleteHarvest( CDNUserSession* pSession, CSFarmHarvest* pPacket )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
{
	do 
	{
#if defined( PRE_ADD_FARM_DOWNSCALE )
		if( bAuthHarvest == true )
		{
			_ASSERT( pSession );
			pSession = NULL;	// 자동 수확일 때 pSession 이 있을 수가 없다. 만약의 사태(?)를 대비해 강제 NULL 처리
		}
		else
		{
			if( pSession == NULL )
				break;
		}

		if( pSession )
		{
			if( CanCompleteHarvest( pSession->GetCharacterDBID() ) == false )
				break;

			// 아이템 검사
			if( CheckHarvestItem( pSession, pPacket ) == false )
				break;
		}
#else
		if( CanCompleteHarvest( pSession->GetCharacterDBID() ) == false )
			break;

		// 아이템 검사
		if( CheckHarvestItem( pSession, pPacket ) == false )
			break;
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

		CDNDBConnection* pDBCon = NULL;
		BYTE cThreadID;
		GetFarmGameRoom()->CopyDBConnectionInfo( pDBCon, cThreadID );

		// 아이템 생성
		TFarmCultivateTableData* pCultivateTable = g_pDataManager->GetFarmCultivateTableData( m_pSeed->GetSeedItemID() );
		if( pCultivateTable == NULL )
			break;		

		int iHarvestCount	= 1;
		int iRandVal = 0;
		if( pSession )
			iRandVal		= (_roomrand(pSession->GetGameRoom())%100)+1;
		else
		{
			CMtRandom Random;
			Random.srand( timeGetTime() );		
			iRandVal		= ((Random.rand())%100)+1;
		}		
		int iVal			= pCultivateTable->iHarvestDropRate[0];

		for( int i=1 ; i<_countof(pCultivateTable->iHarvestDropRate) ; ++i )
		{
			if( iVal < iRandVal )
				++iHarvestCount;
			else
				break;
			iVal += pCultivateTable->iHarvestDropRate[i];
		}

		std::vector<TItem> vItem;

		// 여기서 아이템 계산해서 새루 추가해준다.
		for( int i=0 ; i<iHarvestCount ; ++i )
		{
			int iResultItemID		= 0;
			int iReulstItemCount	= 0;
			int iLevel				= 1;
			int iDepth				= ITEMDROP_DEPTH;

#if defined( PRE_ADD_FARM_DOWNSCALE )
			CDNUserItem::CalcDropItems( pSession, pCultivateTable->iHarvestDropTableID, iResultItemID, iReulstItemCount, iLevel, iDepth );			
#else
			pSession->GetItem()->CalcDropItems( pCultivateTable->iHarvestDropTableID, iResultItemID, iReulstItemCount, iLevel, iDepth );			
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )

			TItem ResultItem;
			if( CDNUserItem::MakeItemStruct( iResultItemID, ResultItem ) == false )
				continue;

			GetPlantSeed()->PushResultItem( ResultItem.nItemID );
			vItem.push_back( ResultItem );

			TItemData *pItemData = g_pDataManager->GetItemData(ResultItem.nItemID);
			if( pSession && pItemData )
				pSession->GetEventSystem()->OnEvent( EventSystem::OnItemGain, 2, EventSystem::ItemType, pItemData->nType, EventSystem::ItemCount, pSession->GetItem()->GetInventoryItemCountByType(pItemData->nType));
		}

		// 오잉?? 생성할 아이템이 없삼?
		if( vItem.empty() )
			break;

		if( m_pState->GetState() == Farm::AreaState::HARVESTING )
			static_cast<CFarmAreaStateHarvesting*>(m_pState)->SetQueryFlag( true );

		UINT		uiAccountDBID	= pSession ? pSession->GetAccountDBID() : 0;
		INT64		biCharacterDBID	= pSession ? pSession->GetCharacterDBID() : 0;
		int			iMapIndex		= pSession ? pSession->GetMapIndex() : 0;
		const char* pszIP			= pSession ? pSession->GetIp() : "";

		if( bIsPrivateArea() == true )
		{
			pDBCon->QueryHarvestForCharacter( cThreadID, GetFarmGameRoom()->GetWorldSetID(), uiAccountDBID, GetFarmGameRoom()->GetRoomID(), GetIndex(), 
											  GetPlantSeed()->GetOwnerCharacterDBID(), biCharacterDBID, vItem, iMapIndex, pszIP );
		}
		else
		{
			pDBCon->QueryHarvest( cThreadID, GetFarmGameRoom()->GetWorldSetID(), uiAccountDBID, GetFarmGameRoom()->GetFarmIndex(), GetFarmGameRoom()->GetRoomID(), GetIndex(), 
								  GetPlantSeed()->GetOwnerCharacterDBID(), biCharacterDBID, vItem, iMapIndex, pszIP );
		}
		return;
	}while( false );

	if( pSession )
		pSession->SendFarmHarvest( ERROR_FARM_CANT_COMPLETE_HARVESTSTATE, pPacket->ActionType, pPacket->iAreaIndex );
}

void CGrowingArea::ProcessStartAddWater( CDNUserSession* pSession, const CSFarmAddWater* pPacket )
{
	do 
	{
#if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_FARM_DOWNSCALE )
		if( GetFarmGameRoom()->GetAttr()&Farm::Attr::Vip )
#else
		if( GetFarmGameRoom()->GetAttr() == Farm::Attr::Vip )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
		{
			if( pSession->bIsFarmVip() == false )
			{
				pSession->SendFarmAddWater( ERROR_FARM_INVALID_VIP, pPacket->ActionType, pPacket->iAreaIndex );
				return;
			}
		}
#endif // #if defined( PRE_ADD_VIP_FARM )

		// 아이템 검사
		const TItem* pItem = NULL;
		if( CheckAddWaterItem( pSession, pPacket, pItem ) == false )
			break;

		if( CanStartAddWater( pSession, pItem ) == false )
			break;

		CFarmAreaStateGrowing* pGrowingState = static_cast<CFarmAreaStateGrowing*>(GetStatePtr());
		pGrowingState->StartAddWater( pSession, pPacket  );
		return;
	}while( false );

	// 에러
	pSession->SendFarmAddWater( ERROR_FARM_CANT_START_ADDWATERSTATE, pPacket->ActionType, pPacket->iAreaIndex );
}

void CGrowingArea::ProcessCancelAddWater( CDNUserSession* pSession, const CSFarmAddWater* pPacket )
{
	if( CanCancelAddWater( pSession ) == false )
	{
		pSession->SendFarmAddWater( ERROR_FARM_CANT_CANCEL_ADDWATERSTATE, pPacket->ActionType, pPacket->iAreaIndex );
		return;
	}

	CFarmAreaStateGrowing* pGrowingState = static_cast<CFarmAreaStateGrowing*>(GetStatePtr());
	pGrowingState->CancelAddWater( pSession, pPacket );	
}

void CGrowingArea::QueryCompleteAddWater( CDNUserSession* pSession, const CSFarmAddWater* pPacket )
{
	do 
	{
		if( CanCompleteAddWater( pSession ) == false )
			break;

		// 아이템 검사
		const TItem* pItem = NULL;
		if( CheckAddWaterItem( pSession, pPacket, pItem ) == false )
			break;

		CDNDBConnection* pDBCon = NULL;
		BYTE cThreadID;
		GetFarmGameRoom()->CopyDBConnectionInfo( pDBCon, cThreadID );

		static_cast<CFarmAreaStateGrowing*>(m_pState)->QueryAddWater();
		if( bIsPrivateArea() == true )
		{
			pDBCon->QueryAddFieldForCharacterAttachment( cThreadID, pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), GetFarmGameRoom()->GetRoomID(),
		   												 GetIndex(), pItem->nItemID, pPacket->cInvenType, pPacket->cWaterItemInvenIndex, pItem->nSerial, pSession->GetMapIndex(), pSession->GetIpW() );
		}
		else
		{
			pDBCon->QueryAddFieldAttachment( cThreadID, pSession, GetFarmGameRoom()->GetFarmIndex(), GetFarmGameRoom()->GetRoomID(), GetIndex(), pItem->nItemID, pPacket->cInvenType, pPacket->cWaterItemInvenIndex, pItem->nSerial );
		}
		return;
	}while( false );

	pSession->SendFarmAddWater( ERROR_FARM_CANT_COMPLETE_ADDWATERSTATE, pPacket->ActionType, pPacket->iAreaIndex );
}

void CGrowingArea::ChangeState( Farm::AreaState::eState State, CDNUserSession* pSession/*=NULL*/ )
{
	// 이미 같은 State 이면 스킵한다.
	if( m_pState->GetState() == State )
		return;

	IFarmAreaState* pChangeState = NULL;

	switch( State )
	{
		case Farm::AreaState::NONE:
		{
			pChangeState = new CFarmAreaStateNone( this );
			break;
		}
		case Farm::AreaState::PLANTING:
		{
			pChangeState = new CFarmAreaStatePlanting( this );
			break;
		}
		case Farm::AreaState::GROWING:
		{
			pChangeState = new CFarmAreaStateGrowing( this );
			break;
		}
		case Farm::AreaState::COMPLETED:
		{
			pChangeState = new CFarmAreaStateCompleted( this );
			break;
		}
		case Farm::AreaState::HARVESTING:
		{
			pChangeState = new CFarmAreaStateHarvesting( this );

			_ASSERT( pSession );
			static_cast<CFarmAreaStateHarvesting*>(pChangeState)->SetHarvestInfo( pSession );
			break;
		}
		case Farm::AreaState::HARVESTED:
		{
			pChangeState = new CFarmAreaStateHarvested( this );
			static_cast<CFarmAreaStateHarvested*>(pChangeState)->SetHarvestedInfo( pSession );
			break;
		}
		default:
		{
			_ASSERT(0);
			return;
		}
	}

	IFarmAreaState* pDeleteState = m_pState;

	// ChangeState 동기화
	if( InterlockedIncrement( &m_lChangeStateLock ) >= 2 )
	{
		_ASSERT(0);
		SAFE_DELETE( pChangeState );
		return;
	}

	m_pState = pChangeState;
	pDeleteState->EndProcess();
	m_pState->BeginProcess();

	InterlockedDecrement( &m_lChangeStateLock );

	// 삭제할 State 객체
	m_vDeleteState.push_back( pDeleteState );

	// Delta 초기화
	m_dwStateElapsedTick = 0;
}

// bResetUpdateTick : 개인농장구역 DB 상에서 ElapsedTick 이 지난경우 Setting
void CGrowingArea::InitializeSeed( const TFarmField* pPacket, bool bResetUpdateTick/*=false*/ )
{
	_ASSERT( m_pSeed == NULL );

	m_pSeed = new CPlantSeed( pPacket->iItemID, pPacket->iElapsedTimeSec*1000, pPacket->biCharacterDBID, const_cast<WCHAR*>(pPacket->wszCharName) );
	for( int i=0 ;i<_countof(pPacket->AttachItems) ; ++i )
	{
		if( pPacket->AttachItems[i].iItemID > 0 )
		{
			m_pSeed->ForceAttachItem(pPacket->AttachItems[i].iItemID,pPacket->AttachItems[i].iCount);
		}
	}
	m_pSeed->CalcAttachItem();
	m_pSeed->SetDBCreateFlag( true );
	if( bResetUpdateTick == true )
		m_pSeed->ResetUpdateTick();

	IFarmAreaState* pChangeState = NULL;

	if( m_pSeed->bIsValid() == false )
	{
		_ASSERT(0);
		SAFE_DELETE( m_pSeed );
		pChangeState = new CFarmAreaStateNone( this );
	}
	else if( m_pSeed->bIsComplete() && bResetUpdateTick == false )
	{
		pChangeState = new CFarmAreaStateCompleted( this );
	}
	else
	{
		pChangeState = new CFarmAreaStateGrowing( this );
	}

	delete m_pState;
	m_pState = pChangeState;
}

void CGrowingArea::DestroySeed()
{
	// 씨앗 파괴 DB
	if( m_pSeed )
	{
		if( m_pSeed->bIsDBCreate() == true )
		{
			if( bIsPrivateArea() == true )
				GetFarmGameRoom()->GetDBConnection()->QueryDelFieldForCharacter( GetFarmGameRoom()->GetDBThreadID(), GetFarmGameRoom()->GetWorldSetID(), GetOwnerCharacterDBID(), GetFarmGameRoom()->GetRoomID(), GetIndex() );
			else
				GetFarmGameRoom()->GetDBConnection()->QueryDelField( GetFarmGameRoom()->GetDBThreadID(), GetFarmGameRoom()->GetWorldSetID(), GetFarmGameRoom()->GetFarmIndex(), GetFarmGameRoom()->GetRoomID(), GetIndex() );
		}

		// FieldCount 동기화
		CDNUserSession* pSession = GetUserSession( m_pSeed->GetOwnerCharacterDBID() );
		if( pSession )
			pSession->GetDBConnection()->QueryGetFieldCountByCharacter( GetFarmGameRoom()->GetDBThreadID(), GetFarmGameRoom()->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), true );
		else
		{
			TCommunityLocation Location;
			if( g_pWorldUserState->GetUserState( m_pSeed->GetOwnerCharacterDBID(), Location ) == true )
			{
				if( Location.cServerLocation == _LOCATION_GAME )
				{
					switch( g_pDataManager->GetMapSubType( Location.nMapIdx ) )
					{
						case GlobalEnum::eMapSubTypeEnum::MAPSUB_FARM:
						{
							g_pMasterConnectionManager->SendFarmSync( GetFarmGameRoom()->GetWorldSetID(), m_pSeed->GetOwnerCharacterDBID(), Farm::ServerSyncType::FIELDCOUNT );
							break;
						}
					}
				}
			}
		}
	}

	SAFE_DELETE( m_pSeed );
	SendAreaInfo();
}

void CGrowingArea::MakeAreaInfo( TFarmAreaInfo& Info )
{
	memset( &Info, 0, sizeof(Info) );

	Info.iAreaIndex		= GetIndex();
	Info.iRandSeed		= GetRandSeed();
	Info.State			= GetState();
	Info.iElapsedTick	= GetElapsedStateTick();
	if( m_pSeed )
	{
		Info.iElapsedTick = m_pSeed->GetElapsedTimeSec()*1000;

		Info.iItemID = m_pSeed->GetSeedItemID();
		m_pSeed->MakeAttachItemInfo( Info );
		_wcscpy( Info.wszCharName, _countof(Info.wszCharName), m_pSeed->GetOwnerCharacterName(), (int)wcslen(m_pSeed->GetOwnerCharacterName()) );
	}
}

void CGrowingArea::SendAreaInfo()
{
	SCAreaInfo TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	MakeAreaInfo( TxPacket.AreaInfo );

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.AreaInfo.AttachItems)+TxPacket.AreaInfo.cAttachCount*sizeof(TxPacket.AreaInfo.AttachItems[0]);
	if( bIsPrivateArea() == true )
	{
		CDNUserSession* pSession = GetUserSession( GetOwnerCharacterDBID() );
		if( pSession )
		{
			_wcscpy( TxPacket.AreaInfo.wszCharName, NAMELENMAX, pSession->GetCharacterName(), static_cast<int>(wcslen(pSession->GetCharacterName())) );
			pSession->AddSendData( SC_FARM, eFarm::SC_AREAINFO, reinterpret_cast<char*>(&TxPacket), iSize );
		}
	}
	else
		GetFarmGameRoom()->BroadCast( SC_FARM, eFarm::SC_AREAINFO, reinterpret_cast<char*>(&TxPacket), iSize );
}

bool CGrowingArea::CanStartPlantSeed( CDNUserSession* pSession )
{ 
	if( m_pState->CanStartPlantSeed() == false )
	{
		g_Log.Log(LogType::_FARM, pSession, L"CGrowingArea::CanStartPlantSeed() State=%d", m_pState->GetState() );
		return false;
	}

	// 식물재배 스킬 검사
	if( pSession->GetSecondarySkillRepository()->Get( SecondarySkill::SubType::CultivationSkill ) == NULL )
		return false;

	return true;
}

bool CGrowingArea::CanCompletePlantSeed( INT64 biCharacterDBID )
{
	// Owner 확인
	if( biCharacterDBID != GetOwnerCharacterDBID() )
		return false;

	return m_pState->CanCompletePlantSeed(); 
}

bool CGrowingArea::CanCancelPlantSeed( CDNUserSession* pSession )
{ 
	// Owner 확인
	if( pSession->GetCharacterDBID() != GetOwnerCharacterDBID() )
		return false;

	return m_pState->CanCancelPlantSeed(); 
}

bool CGrowingArea::CanStartHarvest( CDNUserSession* pSession )
{ 
	if( m_pState->CanStartHarvest() == false )
		return false;

	// 식물재배 스킬 검사
	if( pSession->GetSecondarySkillRepository()->Get( SecondarySkill::SubType::CultivationSkill ) == NULL )
		return false;

	// 범위 검사	
	float fDistSq = EtVec3LengthSq( &( *pSession->GetActorHandle()->GetPosition() - m_OBB.Center ) );
	if( fDistSq > 40000.0f)
		return false;

	return true;
}

bool CGrowingArea::CanCancelHarvest( CDNUserSession* pSession )
{
	if( m_pState->CanCancelHarvest() == false )
		return false;

	_ASSERT( dynamic_cast<CFarmAreaStateHarvesting*>(m_pState) );

	// HarvestringUser ValidCheck
	if( static_cast<CFarmAreaStateHarvesting*>(m_pState)->GetHarvestingCharacterDBID() != pSession->GetCharacterDBID() )
		return false;

	return true;
}

bool CGrowingArea::CanStartAddWater( CDNUserSession* pSession, const TItem* pItem )
{ 
	if( m_pState->CanStartAddWater() == false )
	{
		m_pState->RejectReasonLog(L"CanStartAddWater", pSession);
		return false;
	}

	// AttachItem 개수 검사
	if( m_pSeed->CanAttach( pItem, false ) == false )
	{
		m_pState->RejectReasonLog(L"CanAttach", pSession);
		return false;
	}


	// 식물재배 스킬 검사
	if( pSession->GetSecondarySkillRepository()->Get( SecondarySkill::SubType::CultivationSkill ) == NULL )
	{
		m_pState->RejectReasonLog(L"CultivationSkill is NULL", pSession);
		return false;
	}

	return true;
}

bool CGrowingArea::CanCancelAddWater( CDNUserSession* pSession )
{
	if( m_pState->CanCancelAddWater( pSession ) == false )
		return false;

	return true;
}

bool CGrowingArea::CanCompleteAddWater( CDNUserSession* pSession )
{
	return m_pState->CanCompleteAddWater( pSession );
}

bool CGrowingArea::CanCompleteHarvest( INT64 biCharacterDBID )
{
	if( m_pState->CanCompleteHarvest() == false )
		return false;

	_ASSERT( dynamic_cast<CFarmAreaStateHarvesting*>(m_pState) );

	// HarvestringUser ValidCheck
	if( static_cast<CFarmAreaStateHarvesting*>(m_pState)->GetHarvestingCharacterDBID() != biCharacterDBID )
		return false;

	return true;
}

CDNUserSession*	CGrowingArea::GetUserSession( INT64 biCharacterDBID )
{
	return m_pFarmGameRoom->GetUserSessionByCharDBID( biCharacterDBID );
}

UINT CGrowingArea::GetRandSeed()
{
	if( GetPlantSeed() == NULL )
	{
		m_uiRandSeed = 0;
		return m_uiRandSeed;
	}

	if( m_uiRandSeed == 0 )
	{
		m_uiRandSeed = _roomrand( GetFarmGameRoom() );
	}

	return m_uiRandSeed;
}


