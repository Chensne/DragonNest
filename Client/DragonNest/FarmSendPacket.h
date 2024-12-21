
#pragma once

#include "DNPacket.h"
#include "ClientSessionManager.h"
#include "ClientSocket.h"


namespace Farm
{
	namespace Send
	{
		inline void SendFarmPlant( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cSeedInvenIndex, const BYTE cCount, const INT64 * pAttachItems, const BYTE * pCount )
		{
			CSFarmPlant packet;
			memset( &packet, 0, sizeof(CSFarmPlant) );

			packet.iAreaIndex = iAreaIndex;
			packet.ActionType = ActionType;
			packet.cSeedInvenIndex = cSeedInvenIndex;
			packet.cCount = cCount;
			if( pAttachItems )
			{
				for( int itr = 0; itr < cCount; ++itr )
				{
					packet.AttachItems[itr].biSerial = pAttachItems[itr];
					packet.AttachItems[itr].cCount = pCount[itr];
				}
			}

			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_PLANT, (char*)&packet, sizeof(packet) - sizeof(packet.AttachItems) + (sizeof(TFarmAttachItemSerial)*cCount) );
		}

		inline void SendFarmHarvest( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cNeedItemInvenIndex )
		{
			CSFarmHarvest packet;
			memset( &packet, 0, sizeof(CSFarmHarvest) );

			packet.iAreaIndex = iAreaIndex;
			packet.ActionType = ActionType;
			packet.cNeedItemInvenIndex = cNeedItemInvenIndex;

			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_HARVEST, (char*)&packet, sizeof(packet) );
		}

		inline void SendFarmAddWater( const int iAreaIndex, const Farm::ActionType::eType ActionType, const BYTE cInvenType, const BYTE cWaterItemInvenIndex, const INT64 biWaterItemItemSerial )
		{
			CSFarmAddWater packet;
			memset( &packet, 0, sizeof(CSFarmAddWater) );

			packet.iAreaIndex = iAreaIndex;
			packet.ActionType = ActionType;
			packet.cInvenType = cInvenType;
			packet.cWaterItemInvenIndex = cWaterItemInvenIndex;
			packet.biWaterItemItemSerial = biWaterItemItemSerial;

			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_ADD_WATER, (char*)&packet, sizeof(packet) );
		}

		inline void SendFarmWareHouseList()
		{
			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_WAREHOUSE_LIST, NULL, 0 );
		}

		inline void SendFarmTakeWareHouseItem( INT64 biUniqueID )
		{
			CSFarmTakeWareHouseItem packet;
			memset( &packet, 0, sizeof(CSFarmTakeWareHouseItem) );

			packet.biUniqueID = biUniqueID;

			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_TAKE_WAREHOUSE_ITEM, (char*)&packet, sizeof(packet) );
		}

		inline void SendReqFarmInfo()
		{
			CClientSessionManager::GetInstance().SendPacket(CS_FARM, eFarm::CS_FARMINFO, NULL, 0);
			FUNC_LOG();
		}
	}
}


