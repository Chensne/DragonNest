#include "StdAfx.h"
#include "DNFarmTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNFarmTask::CDNFarmTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNFarmTask::~CDNFarmTask(void)
{
}

void CDNFarmTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
		case QUERY_GETLIST_FARM:
		{
			TQGetListFarm* pPacket = reinterpret_cast<TQGetListFarm*>(pData);

			TAGetListFarm Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListFarm( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetListFarm:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.Farms)+(Packet.cCount*sizeof(Packet.Farms[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
		case QUERY_GETLIST_FIELD:
		{
			TQGetListField* pPacket = reinterpret_cast<TQGetListField*>(pData);

			TAGetListField Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.iFarmDBID	= pPacket->iFarmDBID;
			Packet.iRoomID		= pPacket->iRoomID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListField( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetListField:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.Fields)+(Packet.cCount*sizeof(Packet.Fields[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
		case QUERY_GETLIST_FIELD_FORCHARACTER:
		{
			TQGetListFieldForCharacter* pPacket = reinterpret_cast<TQGetListFieldForCharacter*>(pData);

			TAGetListFieldForCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.iRoomID			= pPacket->iRoomID;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListFieldForCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID,pPacket->biCharacterDBID, 0, L"[ADBID:%d] [QueryGetListFieldForCharacter:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.Fields)+(Packet.cCount*sizeof(Packet.Fields[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
		case QUERY_GETLIST_FIELD_BYCHARACTER:
		{
			TQGetListFieldByCharacter* pPacket = reinterpret_cast<TQGetListFieldByCharacter*>(pData);

			TAGetListFieldByCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.bRefreshGate = pPacket->bRefreshGate;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListFieldByCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetListFieldByCharacter:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
			{
				TQGetListFieldForCharacter pSubQ;
				pSubQ.nAccountDBID		= pPacket->nAccountDBID;
				pSubQ.cWorldSetID		= pPacket->cWorldSetID;
				pSubQ.biCharacterDBID	= pPacket->biCharacterDBID;
				
				TAGetListFieldForCharacter pSubA;
				memset( &pSubA, 0, sizeof(pSubA) );

				if( pWorldDB )
					nRet = pWorldDB->QueryGetListFieldForCharacter( &pSubQ, &pSubA );

				if( nRet == ERROR_NONE )
				{
					for( int i=0 ; i<pSubA.cCount ; ++i )
					{
						Packet.Fields[Packet.cCount].nFieldIndex		= pSubA.Fields[i].nFieldIndex;
						Packet.Fields[Packet.cCount].iItemID			= pSubA.Fields[i].iItemID;
						Packet.Fields[Packet.cCount].iElapsedTimeSec	= pSubA.Fields[i].iElapsedTimeSec;
						memcpy( Packet.Fields[Packet.cCount].AttachItems, pSubA.Fields[i].AttachItems, sizeof(Packet.Fields[Packet.cCount].AttachItems) );

						++Packet.cCount;
						if( Packet.cCount >= _countof(Packet.Fields) )
							break;
					}
				}
			}
			int iSize = sizeof(Packet)-sizeof(Packet.Fields)+(Packet.cCount*sizeof(Packet.Fields[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
		case QUERY_ADD_FIELD:
		case QUERY_ADD_FIELD_FORCHARACTER:
		{
			TQAddField* pPacket = reinterpret_cast<TQAddField*>(pData);

			TAAddField Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode				= ERROR_DB;
			Packet.nAccountDBID			= pPacket->nAccountDBID;
			Packet.iRoomID				= pPacket->iRoomID;
			Packet.nFieldIndex			= pPacket->nFieldIndex;
			Packet.biCharacterDBID		= pPacket->biCharacterDBID;
			Packet.cInvenIndex			= pPacket->cInvenIndex;
			Packet.biSeedItemSerial		= pPacket->biSeedItemSerial;
			Packet.cAttachCount			= pPacket->cAttachCount;
			for( int i=0 ; i<Packet.cAttachCount ; ++i )
				Packet.AttachItems[i] = pPacket->AttachItems[i];

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
			{
				if( nSubCmd == QUERY_ADD_FIELD )
				{
					Packet.nRetCode = pWorldDB->QueryAddField( pPacket, &Packet );
				}
				else
				{
					Packet.nRetCode = pWorldDB->QueryAddFieldForCharacter( pPacket, &Packet );
				}
			}
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryAddField:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );

			if( Packet.nRetCode == ERROR_NONE )
			{
				// 씨앗 아이템 제거
				TQUseItem UseItem;
				memset( &UseItem, 0, sizeof(UseItem) );

				UseItem.cThreadID		= pPacket->cThreadID;
				UseItem.cWorldSetID		= pPacket->cWorldSetID;
				UseItem.nAccountDBID	= pPacket->nAccountDBID;
				UseItem.Code			= DBDNWorldDef::UseItem::Use;
				UseItem.biItemSerial	= pPacket->biSeedItemSerial;
				UseItem.nUsedItemCount	= 1;
				UseItem.iMapID			= pPacket->iMapID;
				_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pPacket->wszIP, (int)wcslen(pPacket->wszIP) );

				CDNMessageTask* pTask = m_pConnection->GetMessageTask(MAINCMD_ITEM);
				if (pTask)
					pTask->OnRecvMessage(pPacket->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem));

				// Attach 아이템 제거
				for( int i=0 ; i<pPacket->cAttachCount ; ++i )
				{
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
					if( !pPacket->AttachItems[i].bRemoveItem )
						continue;
#endif
					UseItem.biItemSerial	= pPacket->AttachItems[i].biSerial;
					UseItem.nUsedItemCount	= pPacket->AttachItems[i].cCount;

					CDNMessageTask* pTask = m_pConnection->GetMessageTask(MAINCMD_ITEM);
					if (pTask)
						pTask->OnRecvMessage(pPacket->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem));
				}
			}

			break;
		}
		case QUERY_DEL_FIELD:
		{
			TQDelField* pPacket = reinterpret_cast<TQDelField*>(pData);

			TADelField Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.iRoomID		= pPacket->iRoomID;
			Packet.nFieldIndex	= pPacket->nFieldIndex;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryDelField( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryDelField:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_DEL_FIELD_FORCHARACTER:
		{
			TQDelFieldForCharacter* pPacket = reinterpret_cast<TQDelFieldForCharacter*>(pData);

			TADelFieldForCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.iRoomID			= pPacket->iRoomID;
			Packet.nFieldIndex		= pPacket->nFieldIndex;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryDelFieldForCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryDelFieldForCharacter:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_ADD_FIELD_ATTACHMENT:
		case QUERY_ADD_FIELD_FORCHARACTER_ATTACHMENT:
		{
			TQAddFieldAttachment* pPacket = reinterpret_cast<TQAddFieldAttachment*>(pData);

			TAAddFieldAttachment Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode				= ERROR_DB;
			Packet.nAccountDBID			= pPacket->nAccountDBID;
			Packet.iRoomID				= pPacket->iRoomID;
			Packet.biCharacterDBID		= pPacket->biCharacterDBID;
			Packet.nFieldIndex			= pPacket->nFieldIndex;
			Packet.iAttachItemID		= pPacket->iAttachItemID;
			Packet.cInvenType			= pPacket->cInvenType;
			Packet.cInvenIndex			= pPacket->cInvenIndex;
			Packet.biAttachItemSerial	= pPacket->biAttachItemSerial;
#if defined( PRE_ADD_VIP_FARM )
			Packet.bVirtualAttach		= pPacket->bVirtualAttach;
#endif // #if defined( PRE_ADD_VIP_FARM )

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
			{
				if( nSubCmd == QUERY_ADD_FIELD_ATTACHMENT )
					Packet.nRetCode = pWorldDB->QueryAddFieldAttachment( pPacket, &Packet );
				else
					Packet.nRetCode = pWorldDB->QueryAddFieldForCharacterAttachment( pPacket, &Packet );
			}
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryAddFieldAttachment:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );

#if defined( PRE_ADD_VIP_FARM )
			if( Packet.nRetCode == ERROR_NONE && pPacket->bVirtualAttach == false )
#else
			if( Packet.nRetCode == ERROR_NONE )
#endif // #if defined( PRE_ADD_VIP_FARM )
			{
				// Attach 아이템 제거
				TQUseItem UseItem;
				memset( &UseItem, 0, sizeof(UseItem) );

				UseItem.cThreadID		= pPacket->cThreadID;
				UseItem.cWorldSetID		= pPacket->cWorldSetID;
				UseItem.nAccountDBID	= pPacket->nAccountDBID;
				UseItem.Code			= DBDNWorldDef::UseItem::Use;
				UseItem.biItemSerial	= pPacket->biAttachItemSerial;
				UseItem.nUsedItemCount	= 1;
				UseItem.iMapID			= pPacket->iMapID;
				_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pPacket->wszIP, (int)wcslen(pPacket->wszIP) );

				CDNMessageTask* pTask = m_pConnection->GetMessageTask(MAINCMD_ITEM);
				if (pTask)
					pTask->OnRecvMessage(pPacket->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem));
			}
			break;
		}
		case QUERY_MOD_FIELD_ELAPSEDTIME:
		{
			TQModFieldElapsedTime* pPacket = reinterpret_cast<TQModFieldElapsedTime*>(pData);

			TAModFieldElapsedTime Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryModFieldElapsedTime( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryModFieldElapsedTime:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			// 게임서버에서 해당 Cmd 따로 처리 안함
			//m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_MOD_FIELD_FORCHARACTER_ELAPSEDTIME:
		{
			TQModFieldForCharacterElapsedTime* pPacket = reinterpret_cast<TQModFieldForCharacterElapsedTime*>(pData);

			TAModFieldForCharacterElapsedTime Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryModFieldForCharacterElapsedTime( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%d] [QueryModFieldForCharacterElapsedTime:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			// 게임서버에서 해당 Cmd 따로 처리 안함
			//m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_HARVEST:
		case QUERY_HARVEST_FORCHARACTER:
		{
			TQHarvest* pPacket = reinterpret_cast<TQHarvest*>(pData);

			TAHarvest Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.iRoomID			= pPacket->iRoomID;
			Packet.nFieldIndex		= pPacket->nFieldIndex;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
			{
				if( nSubCmd == QUERY_HARVEST )
					Packet.nRetCode = pWorldDB->QueryHarvest( pPacket, &Packet );
				else
					Packet.nRetCode = pWorldDB->QueryHarvestForCharacter( pPacket, &Packet );
			}
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryHarvest:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_GETLIST_HARVESTDEPOTITEM:
		{
			TQGetListHarvestDepotItem* pPacket = reinterpret_cast<TQGetListHarvestDepotItem*>(pData);

			TAGetListHarvestDepotItem Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.biLastUniqueID	= pPacket->biLastUniqueID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListHarvestDepotItem( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetListHarvestDepotItem:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.Items)+(Packet.cCount*sizeof(Packet.Items[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
		case QUERY_GETCOUNT_HARVESTDEPOTITEM:
		{
			TQGetCountHarvestDepotItem* pPacket = reinterpret_cast<TQGetCountHarvestDepotItem*>(pData);

			TAGetCountHarvestDepotItem Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetCountHarvestDepotItem( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetCountHarvestDepotItem:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_GETFIELDCOUNT_BYCHARACTER:
		{
			TQGetFieldCountByCharacter* pPacket = reinterpret_cast<TQGetFieldCountByCharacter*>(pData);

			TAGetFieldCountByCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.bIsSend		= pPacket->bIsSend;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetFieldCountByCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QueryGetFieldCountByCharacter:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
		case QUERY_GET_FIELDITEMCOUNT:
		{
			TQGetFieldItemCount* pPacket = reinterpret_cast<TQGetFieldItemCount*>(pData);

			TAGetFieldItemCount Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.iRoomID		= pPacket->iRoomID;
			Packet.iAreaIndex	= pPacket->iAreaIndex;
			Packet.nSessionID = pPacket->nSessionID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetFieldItemCount( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%d] [QueryGetFieldItemCount:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}

	default:
		_DANGER_POINT();		//처리하지 않는 메세지가 왔음
	}
}

