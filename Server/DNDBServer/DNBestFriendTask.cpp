
#include "StdAfx.h"
#include "DNBestFriendTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLWorld.h"
#include "Log.h"
#include "Util.h"

#if defined (PRE_ADD_BESTFRIEND)

CDNBestFriendTask::CDNBestFriendTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{
}

CDNBestFriendTask::~CDNBestFriendTask()
{
}

void CDNBestFriendTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	switch (nSubCmd)
	{
	case QUERY_GET_BESTFRIEND:
		{

			TQGetBestFriend* pPacket = reinterpret_cast<TQGetBestFriend*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_GET_BESTFRIEND:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TAGetBestFriend BestFriend;
			ZeroMemory(&BestFriend, sizeof(BestFriend));

			BestFriend.nAccountDBID = pPacket->nAccountDBID;			
			BestFriend.bSend = pPacket->bSend;
			BestFriend.nRetCode = pWorldDB->QueryGetBestFriend(pPacket, &BestFriend);
			if (BestFriend.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_GET_BESTFRIEND:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, BestFriend.nRetCode);
		
			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&BestFriend), sizeof(BestFriend));

		}
		break;

	case QUERY_REGIST_BESTFRIEND:
		{
			
			TQRegistBestFriend* pPacket = reinterpret_cast<TQRegistBestFriend*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biFromCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_REGIST_BESTFRIEND:%d] pWorldDB not found\r\n", pPacket->biFromCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TARegistBestFriend BestFriend;
			ZeroMemory(&BestFriend, sizeof(BestFriend));

			BestFriend.nAccountDBID = pPacket->nAccountDBID;

			BestFriend.nFromAccountDBID = pPacket->nFromAccountDBID;
			BestFriend.biFromCharacterDBID = pPacket->biFromCharacterDBID;
			_wcscpy(BestFriend.wszFromName, _countof(BestFriend.wszFromName), pPacket->wszFromName, _countof(BestFriend.wszFromName));

			BestFriend.nToAccountDBID = pPacket->nToAccountDBID;
			_wcscpy(BestFriend.wszToName, _countof(BestFriend.wszToName), pPacket->wszToName, _countof(BestFriend.wszToName));
			BestFriend.nItemID = pPacket->nItemID;
			BestFriend.biRegistSerial = pPacket->biRegistSerial;
			
			BestFriend.nRetCode = pWorldDB->QueryRegistBestFriend(pPacket, &BestFriend);
			if (BestFriend.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biFromCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_REGIST_BESTFRIEND:%d] Query Error Ret:%d\r\n", pPacket->biFromCharacterDBID, pPacket->cWorldSetID, BestFriend.nRetCode);
			else
			{
				TQModVariableReset FromVariable;
				memset( &FromVariable, 0, sizeof(FromVariable) );
				FromVariable.cThreadID = pPacket->cThreadID;
				FromVariable.cWorldSetID = pPacket->cWorldSetID;
				FromVariable.biCharacterDBID = pPacket->biFromCharacterDBID;
				FromVariable.Type = CommonVariable::Type::BESTFRIEND_GIFTBOXID;
				FromVariable.biValue = pPacket->nItemID;

				CDNMessageTask* pTask = NULL;
				pTask = m_pConnection->GetMessageTask(MAINCMD_STATUS);
				if (pTask)
					pTask->OnRecvMessage( pPacket->cThreadID, MAINCMD_STATUS, QUERY_MOD_VARIABLERESET, reinterpret_cast<char*>(&FromVariable) );

				TQModVariableReset ToVariable;
				memset( &ToVariable, 0, sizeof(ToVariable) );
				ToVariable.cThreadID = pPacket->cThreadID;
				ToVariable.cWorldSetID = pPacket->cWorldSetID;
				ToVariable.biCharacterDBID = pPacket->biToCharacterDBID;
				ToVariable.Type = CommonVariable::Type::BESTFRIEND_GIFTBOXID;
				ToVariable.biValue = pPacket->nItemID;

				pTask = NULL;
				pTask = m_pConnection->GetMessageTask(MAINCMD_STATUS);
				if (pTask)
					pTask->OnRecvMessage( pPacket->cThreadID, MAINCMD_STATUS, QUERY_MOD_VARIABLERESET, reinterpret_cast<char*>(&ToVariable) );
				
			
				TQUseItem UseItem;
				memset( &UseItem, 0, sizeof(UseItem) );
				UseItem.cThreadID		= pPacket->cThreadID;
				UseItem.cWorldSetID		= pPacket->cWorldSetID;
				UseItem.nAccountDBID	= pPacket->nAccountDBID;
				UseItem.Code			= DBDNWorldDef::UseItem::Use;
				UseItem.biItemSerial	= pPacket->biRegistSerial;
				UseItem.nUsedItemCount	= 1;
				UseItem.iMapID			= pPacket->nMapID;
				_wcscpy( UseItem.wszIP, _countof(UseItem.wszIP), pPacket->wszIP, (int)wcslen(pPacket->wszIP) );
				pTask = NULL;
				pTask = m_pConnection->GetMessageTask(MAINCMD_ITEM);
				if (pTask)
					pTask->OnRecvMessage( pPacket->cThreadID, MAINCMD_ITEM, QUERY_USEITEM, reinterpret_cast<char*>(&UseItem) );
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&BestFriend), sizeof(BestFriend));
		}
		break;

	case QUERY_CANCEL_BESTFRIEND:
		{
			TQCancelBestFriend* pPacket = reinterpret_cast<TQCancelBestFriend*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_CANCEL_BESTFRIEND:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TACancelBestFriend BestFriend;
			ZeroMemory(&BestFriend, sizeof(BestFriend));

			BestFriend.nAccountDBID = pPacket->nAccountDBID;
			BestFriend.bCancel = pPacket->bCancel;
			BestFriend.nRetCode = pWorldDB->QueryCancelBestFriend(pPacket, &BestFriend);
			if (BestFriend.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_CANCEL_BESTFRIEND:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, BestFriend.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&BestFriend), sizeof(BestFriend));
		}
		break;

	case QUERY_CLOSE_BESTFRIEND:
		{
			TQCloseBestFriend* pPacket = reinterpret_cast<TQCloseBestFriend*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_CLOSE_BESTFRIEND:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TACloseBestFriend BestFriend;
			ZeroMemory(&BestFriend, sizeof(BestFriend));

			BestFriend.nAccountDBID = pPacket->nAccountDBID;
			BestFriend.nRetCode = pWorldDB->QueryCloseBestFriend(pPacket, &BestFriend);
			if (BestFriend.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_CLOSE_BESTFRIEND:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, BestFriend.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&BestFriend), sizeof(BestFriend));
		}
		break;

	case QUERY_EDIT_BESTFRIENDMEMO:
		{
			TQEditBestFriendMemo* pPacket = reinterpret_cast<TQEditBestFriendMemo*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_EDIT_BESTFRIENDMEMO:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TAEditBestFriendMemo BestFriend;
			ZeroMemory(&BestFriend, sizeof(BestFriend));

			BestFriend.nAccountDBID = pPacket->nAccountDBID;
			_wcscpy( BestFriend.wszMemo, _countof(BestFriend.wszMemo), pPacket->wszMemo, (int)wcslen(pPacket->wszMemo) );
			BestFriend.nRetCode = pWorldDB->QueryEditBestFriendMemo(pPacket, &BestFriend);
			if (BestFriend.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_EDIT_BESTFRIENDMEMO:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, BestFriend.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&BestFriend), sizeof(BestFriend));
		}
		break;
	default:
		break;
	}
}
#endif // #if defined (PRE_ADD_BESTFRIEND)