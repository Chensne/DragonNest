
#include "StdAfx.h"
#include "DnFarmGameTask.h"
#include "DnActor.h"
#include "DNUserSession.h"
#include "EtWorldSector.h"
#include "EtWorldEventControl.h"
#include "GrowingArea.h"
#include "GrowingAreaStateGrowing.h"
#include "DNDBConnection.h"
#include "FishingArea.h"
#include "DNGameDataManager.h"
#include "DNFarmUserSession.h"
#include "DNFishingStatus.h"
#if defined( PRE_ADD_VIP_FARM )
#include "DNFarmGameRoom.h"
#endif // #if defined( PRE_ADD_VIP_FARM )
#include "TimeSet.h"


CDnFarmGameTask::CDnFarmGameTask( CDNGameRoom* pRoom )
:CDnGameTask( pRoom ),m_bPause(false)
{
	m_nGameTaskType = GameTaskType::Farm;
	m_vUpdateAreaIndex.reserve( Farm::Max::FIELDCOUNT );
	m_vUpdateElapsedTimeSec.reserve( Farm::Max::FIELDCOUNT );
}

CDnFarmGameTask::~CDnFarmGameTask()
{
	for( std::map<int,CGrowingArea*>::iterator itor=m_mGrowingArea.begin() ; itor!=m_mGrowingArea.end() ; ++itor )
	{
		SAFE_DELETE( (*itor).second );
	}
	m_mGrowingArea.clear();
}

bool CDnFarmGameTask::InitializeStage( int nCurrentMapIndex, int nGateIndex, TDUNGEONDIFFICULTY StageDifficulty, int nRandomSeed, bool bContinueStage, bool bDirectConnect, int nGateSelect )
{
	bool bResult = CDnGameTask::InitializeStage( nCurrentMapIndex, nGateIndex, StageDifficulty, nRandomSeed, bContinueStage, bDirectConnect, nGateSelect );
	if( !bResult ) 
		return false;

	InitializeGrowingArea();
	InitializeFishingArea();
	return true;
}

void CDnFarmGameTask::InitializeGrowingArea()
{
	CEtWorldEventControl*	pControl	= NULL;
	CEtWorldSector*			pSector		= NULL;
	CEtWorldEventArea*		pArea		= NULL;

	std::vector<CEtWorldEventArea*> vRespawnArea;

	for( DWORD i=0; i<m_pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
		pControl = pSector->GetControlFromUniqueID( ETE_FarmGrowingArea );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			pArea = pControl->GetAreaFromIndex(j);
			if( pArea )
			{
				std::string					strString(pArea->GetName());
				std::vector<std::string>	vSplit;
		
				boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of("-") );

				_ASSERT( vSplit.size() == 2 );
				if( vSplit.size() < 2 )
					continue;

				boost::trim( vSplit[1] );
				int iAreaIndex = boost::lexical_cast<int>( vSplit[1] );

				FarmGrowingAreaStruct* pData = reinterpret_cast<FarmGrowingAreaStruct*>(pArea->GetData());
				if( pData == NULL )
				{
					_ASSERT(0);
					continue;
				}

				if( pData->nFarmType == Farm::AreaType::Private )
				{
					if( bIsPrivateArea( iAreaIndex ) == true )
					{
						_ASSERT(0);
						continue;
					}
					
					m_mPrivateGrowingAreaInfo.insert( std::make_pair(iAreaIndex,pArea->GetOBB()) );
					continue;
				}
				if( GetGrowingArea( iAreaIndex) )
				{
					_ASSERT(0);
					continue;
				}

				CGrowingArea* pGrowingArea = new CGrowingArea( reinterpret_cast<CDNFarmGameRoom*>(GetRoom()), iAreaIndex, pArea->GetOBB() );
				_ASSERT(pGrowingArea);
				if( pGrowingArea == NULL )
					continue;

				std::pair<std::map<int,CGrowingArea*>::iterator,bool> Ret = m_mGrowingArea.insert( std::make_pair(iAreaIndex,pGrowingArea) );
				if( Ret.second == false )
				{
					_ASSERT(0);
				}
			}
		}
	}
}

void CDnFarmGameTask::InitializeSeed( TAGetListField* pPacket )
{
	if( pPacket->nRetCode != ERROR_NONE )
	{
		_ASSERT(0);
		return;
	}

	for( int i=0 ; i<pPacket->cCount ; ++i )
	{
		CGrowingArea* pArea = GetGrowingArea( pPacket->Fields[i].nFieldIndex );
		if( pArea == NULL )
		{
			_ASSERT(0);
			continue;
		}
		
		pArea->InitializeSeed( &pPacket->Fields[i] );
	}
}

void CDnFarmGameTask::InitializeFishingArea()
{
	std::vector <TFishingAreaTableData> vFishingArea;
	g_pDataManager->GetFishingAreaTableDataByMapID(GetMapTableID(), vFishingArea);

	CEtWorldEventControl*	pControl	= NULL;
	CEtWorldSector*			pSector		= NULL;
	CEtWorldEventArea*		pArea		= NULL;

	for( DWORD i=0; i<m_pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
	{
		pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
		pControl = pSector->GetControlFromUniqueID( ETE_FarmFishingArea );
		if( !pControl ) 
			continue;

		for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
		{
			pArea = pControl->GetAreaFromIndex(j);
			if( pArea )
			{
				int nFishingPointID = -1;
				int nFishingAreaID = -1;
				std::vector <TFishingAreaTableData>::iterator ii;
				for (ii = vFishingArea.begin(); ii != vFishingArea.end(); ii++)
				{
					if (pArea->GetCreateUniqueID() == (*ii).nMatchedFishingAreaID)
					{
						nFishingPointID = (*ii).nFishingPointID;
						nFishingAreaID = (*ii).nMatchedFishingAreaID;
						break;
					}
				}

				if (nFishingPointID == -1)
					continue;

				TFishingPointTableData * pFishingPoint = g_pDataManager->GetFishingPointTableData(nFishingPointID);
				if (pFishingPoint == NULL)
				{
					_ASSERT(0);
					continue;
				}

				if (GetFishingArea(nFishingAreaID))
				{
					_ASSERT_EXPR(0, L"Check Fishing Area Index Duplicated");
					continue;
				}

				CFishingArea * pFishingArea = new CFishingArea(reinterpret_cast<CDNFarmGameRoom*>(GetRoom()), nFishingAreaID, pArea->GetOBB(), pFishingPoint);
				_ASSERT(pFishingArea);
				if(pFishingArea == NULL)
					continue;

				std::pair<std::map<int,CFishingArea*>::iterator,bool> Ret = m_mFishingArea.insert(std::make_pair(nFishingAreaID, pFishingArea));
				if(Ret.second == false)
				{
					_ASSERT(0);
				}
			}
		}
	}
}

bool CDnFarmGameTask::OnInitializeBreakIntoActor( CDNUserSession* pSession, const int iVectorIndex )
{
	if( !CDnGameTask::OnInitializeBreakIntoActor( pSession, iVectorIndex ) )
		return false;

	return true;
}

void CDnFarmGameTask::OnInitializeStartPosition( CDNUserSession* pSession, const int iPartyIndex )
{
	DnActorHandle hActor = pSession->GetActorHandle();
	
	OnInitializeRespawnPosition( hActor, true );
}

void CDnFarmGameTask::OnInitializeRespawnPosition( DnActorHandle hActor, const bool bIsStartPosition/*=false*/ )
{
	if( hActor )
	{
		CEtWorldEventControl*	pControl	= NULL;
		CEtWorldSector*			pSector		= NULL;
		CEtWorldEventArea*		pArea		= NULL;

		std::vector<CEtWorldEventArea*> vRespawnArea;

		if( bIsStartPosition )
		{
			for( DWORD i=0; i<m_pWorld->GetGrid()->GetActiveSectorCount(); i++ ) 
			{
				pSector	 = m_pWorld->GetGrid()->GetActiveSector(i);
				pControl = pSector->GetControlFromUniqueID( ETE_EventArea );
				if( !pControl ) 
					continue;

				for( DWORD j=0; j<pControl->GetAreaCount(); j++ ) 
				{
					pArea = pControl->GetAreaFromIndex(j);
					if( pArea )
					{
						if( strstr( pArea->GetName(), Farm::StartPositionStr.c_str() ) == pArea->GetName() )
						{
							PvPRespawnAreaStruct* pStruct = reinterpret_cast<PvPRespawnAreaStruct*>(pArea->GetData());
							if( pStruct  )
								vRespawnArea.push_back( pArea );
						}
					}
				}
			}
		}

		if( !vRespawnArea.empty() )
		{
			_srand( GetRoom(), hActor->GetSessionID() );
			CEtWorldEventArea* pRespawnArea = vRespawnArea[_rand(GetRoom())%vRespawnArea.size()];

			EtVector3	vRandPos	= GetGenerationRandomPosition( pRespawnArea->GetOBB() );
			float		fRotate		= pRespawnArea->GetRotate();
			EtVector2	vStartPos( vRandPos.x,vRandPos.z );

			// 리스폰 좌표/LOOK 설정
			hActor->SetPosition( EtVector3( vStartPos.x, m_pWorld->GetHeight( vStartPos.x, vStartPos.y ), vStartPos.y ) );

			EtVector2 vDir;
			vDir.x = sin( EtToRadian( fRotate ) );
			vDir.y = cos( EtToRadian( fRotate ) );
			hActor->Look( vDir );

			if( hActor->GetObjectHandle() )
				hActor->GetObjectHandle()->Update( *hActor->GetMatEx() );
		}
	}
}

int CDnFarmGameTask::OnDispatchMessage(CDNUserSession * pSession, int nMainCmd, int nSubCmd, char *pData, int nLen)
{
	CGameServerSession::OnDispatchMessage( pSession, nMainCmd, nSubCmd, pData, nLen );

	switch( nMainCmd )
	{
		case CS_FARM:	
			return OnRecvFarmMessage( pSession, nSubCmd, pData, nLen );
			// 일단 농장 태스크에 묶는다.
		case CS_FISHING:
			return OnRecvFishingMessage( pSession, nSubCmd, pData, nLen );
	}

	return ERROR_NONE;
}

int CDnFarmGameTask::OnRecvFarmMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen)
{
	switch( nSubCmd )
	{
		case eFarm::CS_PLANT:
		{
			CSFarmPlant* pPacket = reinterpret_cast<CSFarmPlant*>(pData);

			if( sizeof(CSFarmPlant)-sizeof(pPacket->AttachItems)+(pPacket->cCount*sizeof(pPacket->AttachItems[0])) != nLen )
				return ERROR_INVALIDPACKET;
			CGrowingArea* pArea = GetGrowingArea( pPacket->iAreaIndex, pSession );
			if( pArea == NULL )
			{
				_ASSERT(0);
				return ERROR_INVALIDPACKET;
			}

			switch( pPacket->ActionType )
			{
				case Farm::ActionType::START:
				{
					pArea->ProcessStartPlant( pSession, pPacket );
					break;
				}
				case Farm::ActionType::CANCEL:
				{
					pArea->ProcessCancelPlant( pSession, pPacket );
					break;
				}
				case Farm::ActionType::COMPLETE:
				{
					pArea->QueryCompletePlant( pSession, pPacket );
					break;
				}
				default:
				{
					_ASSERT(0);
					return ERROR_INVALIDPACKET;
				}
			}

			return ERROR_NONE;
		}
		case eFarm::CS_HARVEST:
		{
			if( sizeof(CSFarmHarvest) != nLen )
				return ERROR_INVALIDPACKET;

			CSFarmHarvest* pPacket = reinterpret_cast<CSFarmHarvest*>(pData);
			CGrowingArea* pArea = GetGrowingArea( pPacket->iAreaIndex, pSession );
			if( pArea == NULL )
			{
				_ASSERT(0);
				return ERROR_INVALIDPACKET;
			}

			switch( pPacket->ActionType )
			{
				case Farm::ActionType::START:
				{
					pArea->ProcessStartHarvest( pSession, pPacket );
					break;
				}
				case Farm::ActionType::CANCEL:
				{
					pArea->ProcessCancelHarvest( pSession, pPacket );
					break;
				}
				case Farm::ActionType::COMPLETE:
				{
					pArea->QueryCompleteHarvest( pSession, pPacket );
					break;
				}
				default:
				{
					_ASSERT(0);
					return ERROR_INVALIDPACKET;
				}
			}

			return ERROR_NONE;
		}
		case eFarm::CS_ADD_WATER:
		{
#if defined( PRE_REMOVE_FARM_WATER )
			return ERROR_INVALIDPACKET;
#endif // #if defined( PRE_REMOVE_FARM_WATER )
			if( sizeof(CSFarmAddWater) != nLen )
				return ERROR_INVALIDPACKET;

			const CSFarmAddWater* pPacket = reinterpret_cast<CSFarmAddWater*>(pData);
			CGrowingArea* pArea = GetGrowingArea( pPacket->iAreaIndex, pSession );
			if( pArea == NULL )
			{
				_ASSERT(0);
				return ERROR_INVALIDPACKET;
			}

			switch( pPacket->ActionType )
			{
				case Farm::ActionType::START:
				{
					pArea->ProcessStartAddWater( pSession, pPacket );
					break;
				}
				case Farm::ActionType::CANCEL:
				{
					pArea->ProcessCancelAddWater( pSession, pPacket );
					break;
				}
				case Farm::ActionType::COMPLETE:
				{
					pArea->QueryCompleteAddWater( pSession, pPacket );
					break;
				}
				default:
				{
					_ASSERT(0);
					return ERROR_INVALIDPACKET;
				}
			}
				
			return ERROR_NONE;
		}
		case eFarm::CS_WAREHOUSE_LIST:
		{
			if( 0 != nLen )
				return ERROR_INVALIDPACKET;

			GetRoom()->GetDBConnection()->QueryGetListHarvestDepotItem( GetRoom()->GetDBThreadID(), GetRoom()->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), static_cast<CDNFarmUserSession*>(pSession)->GetLastWareHouseItemUniqueID() );
			return ERROR_NONE;
		}
		case eFarm::CS_TAKE_WAREHOUSE_ITEM:
		{
			if( sizeof(CSFarmTakeWareHouseItem) != nLen )
				return ERROR_INVALIDPACKET;

			const CSFarmTakeWareHouseItem* pPacket = reinterpret_cast<CSFarmTakeWareHouseItem*>(pData);

			const TItem* pItem = static_cast<CDNFarmUserSession*>(pSession)->GetFarmWareHouseItem( pPacket->biUniqueID );
			if( pItem == NULL )
			{
				pSession->SendFarmTakeWareHouseItem( ERROR_ITEM_NOTFOUND, pPacket->biUniqueID );
				return ERROR_NONE;
			}

			int iEmptyIndex = pSession->GetItem()->FindBlankInventorySlot();
			if( iEmptyIndex == -1 )
			{
				pSession->SendFarmTakeWareHouseItem( ERROR_ITEM_INVENTORY_NOTENOUGH, pPacket->biUniqueID );
				return ERROR_NONE;
			}

			pSession->GetItem()->CreateInvenWholeItemByIndex( iEmptyIndex, *const_cast<TItem*>(pItem) );
			pSession->SendFarmTakeWareHouseItem( ERROR_NONE, pPacket->biUniqueID );
			pSession->GetDBConnection()->QueryChangeItemLocation( pSession, pItem->nSerial, 0, pItem->nItemID, DBDNWorldDef::ItemLocation::Inventory, iEmptyIndex, pItem->wCount, false, 0, false );
			pSession->GetDBConnection()->QueryGetCountHarvestDepotItem( pSession );
			static_cast<CDNFarmUserSession*>(pSession)->DeleteFarmWareHouseItem( pPacket->biUniqueID );
			return ERROR_NONE;
		}
	}

	return ERROR_UNKNOWN_HEADER;
}

int CDnFarmGameTask::OnRecvFishingMessage(CDNUserSession * pSession, int nSubCmd, char *pData, int nLen)
{
	switch (nSubCmd)
	{
	case eFishing::CS_READYFISHING:
		{
			if (sizeof(CSFishingReady) != nLen)
				return ERROR_INVALIDPACKET;

			//낚시 가능한 영역에 있는지 확인하고 인벤에 낚시도구가 있는지 확인후 다 있으면, 시작시켜준다.
			if (static_cast<CDNFarmUserSession*>(pSession)->GetIsFishing())
				return ERROR_NONE;

#if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_FARM_DOWNSCALE )
			if( static_cast<CDNFarmGameRoom*>(GetRoom())->GetAttr()&Farm::Attr::Vip )
#else
			if( static_cast<CDNFarmGameRoom*>(GetRoom())->GetAttr() == Farm::Attr::Vip )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
			{
				if( pSession->bIsFarmVip() == false )
				{
					pSession->SendFishingReadyResult(ERROR_FARM_INVALID_VIP);
					return ERROR_NONE;
				}
			}
#endif // #if defined( PRE_ADD_VIP_FARM )

			CSFishingReady * pPacket = (CSFishingReady*)pData;
			CFishingArea * pFishing = GetFishingArea(pSession->GetActorHandle()->GetPosition());
			if (pFishing)
			{
#ifdef PRE_ADD_CASHFISHINGITEM
				TFishingMeritInfo FishingMerit;
				memset(&FishingMerit, 0, sizeof(TFishingMeritInfo));
				int nRet = pFishing->CheckFishingRequirement(static_cast<CDNFarmUserSession*>(pSession), pPacket->ToolInfo, FishingMerit);
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
				INT64 nBaitSerial = 0;
				int nRet = pFishing->CheckFishingRequirement(static_cast<CDNFarmUserSession*>(pSession), pPacket->nFishingRodInvenIdx, pPacket->nFishingBaitInvenIdx, nBaitSerial);
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
				if (nRet != ERROR_NONE)
				{
					pSession->SendFishingReadyResult(nRet);
					return ERROR_NONE;
				}

				//기본환인완료 낚시패턴을 얻어서 낚시 상태를 세션에 추가
				if (static_cast<CDNFarmUserSession*>(pSession)->SetFishingStatus(pFishing))
				{
					pSession->SendFishingReadyResult(ERROR_NONE);
					return ERROR_NONE;
				}
			}

			pSession->SendFishingReadyResult(ERROR_FISHING_FAIL);
			return ERROR_NONE;
		}

	case eFishing::CS_CASTBAIT:
		{
			if (sizeof(CSCastBait) != nLen)
				return ERROR_INVALIDPACKET;

			if (static_cast<CDNFarmUserSession*>(pSession)->GetIsFishing())
				return ERROR_NONE;

#if defined( PRE_ADD_VIP_FARM )
#if defined( PRE_ADD_FARM_DOWNSCALE )
			if( static_cast<CDNFarmGameRoom*>(GetRoom())->GetAttr()&Farm::Attr::Vip )
#else
			if( static_cast<CDNFarmGameRoom*>(GetRoom())->GetAttr() == Farm::Attr::Vip )
#endif // #if defined( PRE_ADD_FARM_DOWNSCALE )
			{
				if( pSession->bIsFarmVip() == false )
				{
					pSession->SendFishingCastBaitResult(ERROR_FARM_INVALID_VIP);
					return ERROR_NONE;
				}
			}
#endif // #if defined( PRE_ADD_VIP_FARM )

			CSCastBait * pPacket = (CSCastBait*)pData;
			CDNFishingStatus * pStatus = static_cast<CDNFarmUserSession*>(pSession)->GetFishingStatus();
			if (pStatus)
			{
				CFishingArea * pArea = pStatus->GetFishingArea();
				if (pArea)
				{
#ifdef PRE_ADD_CASHFISHINGITEM
					TFishingMeritInfo FishingMerit;
					memset(&FishingMerit, 0, sizeof(TFishingMeritInfo));
					int nRet = pArea->CheckFishingRequirement(static_cast<CDNFarmUserSession*>(pSession), pPacket->ToolInfo, FishingMerit);
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
					INT64 nBaitSerial = 0;
					int nRet = pArea->CheckFishingRequirement(static_cast<CDNFarmUserSession*>(pSession), pPacket->nFishingRodInvenIdx, pPacket->nFishingBaitInvenIdx, nBaitSerial);
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
					if (nRet != ERROR_NONE)
					{
						pSession->SendFishingCastBaitResult(nRet);
						return ERROR_NONE;
					}

					//정상이라면 시작!
#ifdef PRE_ADD_CASHFISHINGITEM
					if (pStatus->StartFishing(pPacket->eCastType, FishingMerit))
#else		//#ifdef PRE_ADD_CASHFISHINGITEM
					if (pStatus->StartFishing(pPacket->eCastType, pPacket->nFishingBaitInvenIdx, nBaitSerial))
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
						return ERROR_NONE;
				}
			}

			pSession->SendFishingCastBaitResult(ERROR_FISHING_CASTBAIT_FAIL);
			return ERROR_NONE;
		}

	case eFishing::CS_STOPFISHING:
		{
			if (0 != nLen)
				return ERROR_INVALIDPACKET;

			if (static_cast<CDNFarmUserSession*>(pSession)->GetIsFishing() == false)
				return ERROR_NONE;
			
			CDNFishingStatus * pStatus = static_cast<CDNFarmUserSession*>(pSession)->GetFishingStatus();
			if (pStatus)
			{
				if (pStatus->StopFishing())
					return ERROR_NONE;
			}
			
			pSession->SendFishingStopFishingResult(ERROR_FISHING_STOPFISHING_FAIL);
			return ERROR_NONE;
		}

	case eFishing::CS_PULLINGROD:
		{
			if (sizeof(CSPullingRod) != nLen)
				return ERROR_INVALIDPACKET;

			if (static_cast<CDNFarmUserSession*>(pSession)->GetIsFishing() == false)
				return ERROR_NONE;

			CSPullingRod * pPacket = (CSPullingRod*)pData;
			CDNFishingStatus * pStatus = static_cast<CDNFarmUserSession*>(pSession)->GetFishingStatus();
			if (pStatus)
				pStatus->FishingControl(pPacket->ePullingControl);
			return ERROR_NONE;
		}

	case eFishing::CS_FISHINGREWARD:
		{
			if (nLen != 0)
				return ERROR_INVALIDPACKET;

			CDNFishingStatus * pStatus = static_cast<CDNFarmUserSession*>(pSession)->GetFishingStatus();
			if (pStatus)
				pStatus->CheckAndRewardItem();		

			return ERROR_NONE;
		}
	}
	return ERROR_UNKNOWN_HEADER;
}

void CDnFarmGameTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnGameTask::Process( LocalTime, fDelta );
	ProcessGrowingArea( fDelta );
	//ProcessFishingArea(fDelta);
}

void CDnFarmGameTask::ProcessGrowingArea( float fDelta )
{
	if( m_bPause )
		return;

	m_vUpdateAreaIndex.clear();
	m_vUpdateElapsedTimeSec.clear();

	for( std::map<int,CGrowingArea*>::iterator itor=m_mGrowingArea.begin() ; itor!=m_mGrowingArea.end() ; ++itor )
	{
		CGrowingArea* pArea = (*itor).second;

		pArea->Process( fDelta );
		
		// DB Update 검사
		if( pArea->CanDBUpdate() )
		{
			m_vUpdateAreaIndex.push_back( pArea->GetIndex() );
			m_vUpdateElapsedTimeSec.push_back( pArea->GetPlantSeed()->GetElapsedTimeSec() );
		}
	}

	if( m_vUpdateAreaIndex.size() )
	{
		GetRoom()->GetDBConnection()->QueryModFieldElapsedTime( GetRoom()->GetDBThreadID(), GetRoom()->GetWorldSetID(), 0, GetRoom()->GetFarmIndex(), m_vUpdateAreaIndex, m_vUpdateElapsedTimeSec );
	}

	for( std::map<INT64,std::vector<CGrowingArea*>>::iterator itor=m_mPrivateGrowingArea.begin() ; itor!=m_mPrivateGrowingArea.end() ; ++itor )
	{
		m_vUpdateAreaIndex.clear();
		m_vUpdateElapsedTimeSec.clear();

		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			CGrowingArea* pArea = (*itor).second[i];
			pArea->Process( fDelta );

			// DB Update 검사
			if( pArea->CanDBUpdate() )
			{
				m_vUpdateAreaIndex.push_back( pArea->GetIndex() );
				m_vUpdateElapsedTimeSec.push_back( pArea->GetPlantSeed()->GetElapsedTimeSec() );
			}
		}

		if( m_vUpdateAreaIndex.size() )
			GetRoom()->GetDBConnection()->QueryModFieldForCharacterElapsedTime( GetRoom()->GetDBThreadID(), GetRoom()->GetWorldSetID(), 0, (*itor).first, m_vUpdateAreaIndex, m_vUpdateElapsedTimeSec );
	}
}

CGrowingArea* CDnFarmGameTask::GetGrowingArea( const int iIndex, CDNUserSession* pSession/*=NULL*/ )
{
	if( bIsPrivateArea(iIndex) == true )
	{
		if( pSession == NULL )
			return NULL;

		std::map<INT64,std::vector<CGrowingArea*>>::iterator itor= m_mPrivateGrowingArea.find( pSession->GetCharacterDBID() );
		if( itor == m_mPrivateGrowingArea.end() )
			return NULL;

		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			if( (*itor).second[i]->GetIndex() == iIndex )
				return (*itor).second[i];
		}
		return NULL;
	}

	std::map<int,CGrowingArea*>::iterator itor = m_mGrowingArea.find( iIndex );
	if( itor != m_mGrowingArea.end() )
		return (*itor).second;

	return NULL;
}

CFishingArea * CDnFarmGameTask::GetFishingArea(int nIdx)
{
	std::map <int, CFishingArea*>::iterator ii = m_mFishingArea.find(nIdx);
	return ii != m_mFishingArea.end() ? (*ii).second : NULL;
}

CFishingArea * CDnFarmGameTask::GetFishingArea(EtVector3 * pPosition)
{
	CFishingArea * pFishingArea = NULL;
	std::map <int, CFishingArea*>::iterator ii;
	for (ii = m_mFishingArea.begin(); ii != m_mFishingArea.end(); ii++)
	{
		if ((*ii).second->CheckInside(*pPosition))
		{
			pFishingArea = (*ii).second;
			break;
		}
	}

	return pFishingArea;
}

CFishingArea * CDnFarmGameTask::GetFishingArea(CDNUserSession * pSession)
{
	if (!pSession || !pSession->GetActorHandle())
		return NULL;

	std::map <int, CFishingArea*>::iterator ii;
	for (ii = m_mFishingArea.begin(); ii != m_mFishingArea.end(); ii++)
	{
		if ((*ii).second->CheckInside(*pSession->GetActorHandle()->GetPosition()))
			return (*ii).second;
	}
	return NULL;
}

void CDnFarmGameTask::SyncArea( CDNUserSession* pBreakIntoGameSession )
{
	if( pBreakIntoGameSession == NULL )
	{
		_ASSERT(0);
		return;
	}

	SCAreaInfoList TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	for( std::map<int,CGrowingArea*>::iterator itor=m_mGrowingArea.begin() ; itor!=m_mGrowingArea.end() ; ++itor )
	{
		CGrowingArea*	pArea = (*itor).second;
		CPlantSeed*		pSeed = pArea->GetPlantSeed();
		if( pSeed == NULL )
			continue;

		pArea->MakeAreaInfo( TxPacket.AreaInfos[TxPacket.unCount] );
		++TxPacket.unCount;
	}

	std::map<INT64,std::vector<CGrowingArea*>>::iterator itor = m_mPrivateGrowingArea.find( pBreakIntoGameSession->GetCharacterDBID() );
	if( itor != m_mPrivateGrowingArea.end() )
	{
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			CGrowingArea*	pArea = (*itor).second[i];
			CPlantSeed*		pSeed = pArea->GetPlantSeed();
			if( pSeed == NULL )
				continue;

			pArea->MakeAreaInfo( TxPacket.AreaInfos[TxPacket.unCount] );
			// 개인 농작물은 이름정보가 없어서 강제로 넣어준다.
			_wcscpy( TxPacket.AreaInfos[TxPacket.unCount].wszCharName, NAMELENMAX, pBreakIntoGameSession->GetCharacterName(), static_cast<int>(wcslen(pBreakIntoGameSession->GetCharacterName())));
			++TxPacket.unCount;
		}
	}

	int iSize = sizeof(TxPacket)-sizeof(TxPacket.AreaInfos)+TxPacket.unCount*sizeof(TxPacket.AreaInfos[0]);
	pBreakIntoGameSession->AddSendData( SC_FARM, eFarm::SC_AREAINFO_LIST, reinterpret_cast<char*>(&TxPacket), iSize );
}

void CDnFarmGameTask::SyncFishing( CDNUserSession* pBreakIntoGameSession )
{
	if( pBreakIntoGameSession == NULL )
	{
		_ASSERT(0);
		return;
	}

	SCFishingPlayer TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	for( UINT i=0 ; i<GetUserCount() ; ++i )
	{
		CDNFarmUserSession* pSession = static_cast<CDNFarmUserSession*>(GetUserData(i));
		if( pSession && pSession->GetIsFishingReward() )
		{
			TxPacket.nSessionID = pSession->GetSessionID();
#ifdef PRE_ADD_CASHFISHINGITEM
			TxPacket.nRodItemID = pSession->GetFishingRodItemID();
#endif		//#ifdef PRE_ADD_CASHFISHINGITEM
			pBreakIntoGameSession->AddSendData( SC_FISHING, eFishing::SC_FISHINGPLAYER, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
		}
	}
}

bool CDnFarmGameTask::UpdateCharacterName( MAChangeCharacterName* pPacket )
{
	bool bBroadcast = false;
	for( std::map<int,CGrowingArea*>::iterator itor=m_mGrowingArea.begin() ; itor!=m_mGrowingArea.end() ; ++itor )
	{
		CGrowingArea* pArea = (*itor).second;
		if (!pArea)
			continue;

		CPlantSeed* pSeed = pArea->GetPlantSeed();
		if (!pSeed)
			continue;

		if (pSeed->UpdateOwnerCharacterName(pPacket))
			bBroadcast = true;
	}

	return bBroadcast;
}

void CDnFarmGameTask::InitializePrivateGrowingArea( TAGetListFieldForCharacter* pPacket )
{
	if( pPacket->nRetCode != ERROR_NONE )
		return;

	std::map<INT64,std::vector<CGrowingArea*>>::iterator itor = m_mPrivateGrowingArea.find( pPacket->biCharacterDBID );
	if( itor != m_mPrivateGrowingArea.end() )
	{
		_DANGER_POINT();

		// 개인구역은 중복 될 수 없다. 이 경우 강제로 날린다.
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			SAFE_DELETE( (*itor).second[i] );
		}
		m_mPrivateGrowingArea.erase( itor );
	}

	m_mPrivateGrowingArea.insert( std::make_pair(pPacket->biCharacterDBID, std::vector<CGrowingArea*>()) );
	std::map<INT64,std::vector<CGrowingArea*>>::iterator PrivateItor = m_mPrivateGrowingArea.find( pPacket->biCharacterDBID );
	if( m_mPrivateGrowingArea.find( pPacket->biCharacterDBID ) == m_mPrivateGrowingArea.end() )
	{
		_DANGER_POINT();
		return;
	}

	// PrivateArea 설정
	for( std::map<int,SOBB*>::iterator itor=m_mPrivateGrowingAreaInfo.begin() ; itor!=m_mPrivateGrowingAreaInfo.end() ; ++itor )
	{
		CGrowingArea* pGrowingArea = new CGrowingArea( reinterpret_cast<CDNFarmGameRoom*>(GetRoom()), (*itor).first, (*itor).second, true, pPacket->biCharacterDBID );
		_ASSERT(pGrowingArea);
		if( pGrowingArea == NULL )
			continue;

		(*PrivateItor).second.push_back( pGrowingArea );
	}
	
	// Seed 설정
	for( int i=0 ; i<pPacket->cCount ; ++i )
	{
		CTimeSet CurTime;
		CTimeSet UpdateTime( pPacket->Fields[i].tUpdateDate, true );
		INT64 biElapsedSec = CurTime-UpdateTime;
		
		TFarmField Field;
		memset( &Field, 0, sizeof(Field) );

		Field.nFieldIndex		= pPacket->Fields[i].nFieldIndex;
		Field.biCharacterDBID	= pPacket->biCharacterDBID;
		Field.iItemID			= pPacket->Fields[i].iItemID;
		Field.iElapsedTimeSec	= pPacket->Fields[i].iElapsedTimeSec;
		memcpy( Field.AttachItems, pPacket->Fields[i].AttachItems, sizeof(pPacket->Fields[i].AttachItems) );

		// 시간 Update
		if( biElapsedSec > 0 )
		{
			Field.iElapsedTimeSec += static_cast<int>(biElapsedSec);
		}

		for( UINT j=0 ; j<(*PrivateItor).second.size() ; ++j )
		{
			if( (*PrivateItor).second[j]->GetIndex() == Field.nFieldIndex )
			{
				(*PrivateItor).second[j]->InitializeSeed( &Field, biElapsedSec > 0 ? true : false );
			}
		}
	}
}

void CDnFarmGameTask::OnLeaveUser( UINT uiSessionID )
{
	CDNUserSession* pSession = GetRoom()->GetUserSession( uiSessionID );
	if( pSession == NULL )
		return;

	std::map<INT64,std::vector<CGrowingArea*>>::iterator itor = m_mPrivateGrowingArea.find( pSession->GetCharacterDBID() );
	if( itor != m_mPrivateGrowingArea.end() )
	{
		// ElapsedUpdate
		std::vector<int> vUpdateAreaIndex;
		std::vector<int> vUpdateElapsedTimeSec;

		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
		{
			CGrowingArea* pArea = (*itor).second[i];

			if( pArea && pArea->GetPlantSeed() )
			{
				vUpdateAreaIndex.push_back( pArea->GetIndex() );
				vUpdateElapsedTimeSec.push_back( pArea->GetPlantSeed()->GetElapsedTimeSec() );
			}
		}

		if( vUpdateAreaIndex.size() )
			GetRoom()->GetDBConnection()->QueryModFieldForCharacterElapsedTime( GetRoom()->GetDBThreadID(), GetRoom()->GetWorldSetID(), 0, (*itor).first, vUpdateAreaIndex, vUpdateElapsedTimeSec );

		// Delete
		for( UINT i=0 ; i<(*itor).second.size() ; ++i )
			SAFE_DELETE( (*itor).second[i] );
		m_mPrivateGrowingArea.erase( itor );
	}	
}

void CDnFarmGameTask::OnStartGuildWarFinal()
{
	for( std::map<int,CGrowingArea*>::iterator itor=m_mGrowingArea.begin() ; itor!=m_mGrowingArea.end() ; ++itor )
	{
		CGrowingArea* pArea = (*itor).second;

		if( pArea->GetState() == Farm::AreaState::GROWING )
		{
			CPlantSeed* pSeed = pArea->GetPlantSeed();
			if( pSeed )
				pSeed->ForceComplete();
		}
		else if( pArea->GetState() == Farm::AreaState::COMPLETED )
		{
			pArea->QueryCompleteHarvest( NULL, NULL, true );
		}
	}
}

bool CDnFarmGameTask::bIsPrivateArea( int iAreaIndex )
{
	std::map<int,SOBB*>::iterator itor = m_mPrivateGrowingAreaInfo.find( iAreaIndex );
	if( itor == m_mPrivateGrowingAreaInfo.end() )
		return false;

	return true;
}

bool CDnFarmGameTask::CheckActivePrivateFieldCount( CDNUserSession* pSession )
{
	std::map<INT64,std::vector<CGrowingArea*>>::iterator itor = m_mPrivateGrowingArea.find( pSession->GetCharacterDBID() );
	if( itor == m_mPrivateGrowingArea.end() )
		return false;

	int iPrivateSeedCount = 0;
	for( UINT i=0 ; i<(*itor).second.size() ; ++i )
	{
		if( (*itor).second[i]->GetPlantSeed() )
			++iPrivateSeedCount;
	}

	int iAvailCount = Farm::Max::PRIVATEPLANTCOUNT+pSession->GetItem()->GetExtendPrivateFarmFieldCount();

	return iAvailCount > iPrivateSeedCount;
}

