#include "StdAfx.h"
#include "DNFriendTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Util.h"
#include "Log.h"

CDNFriendTask::CDNFriendTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNFriendTask::~CDNFriendTask(void)
{
}

void CDNFriendTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_FRIENDLIST:
		{
			TQFriendList * pPacket = (TQFriendList*)pData;

			TAFriendList packet;
			memset(&packet, 0, sizeof(TAFriendList));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryGetListFriend(pPacket->biCharacterDBID, packet.cCount, packet.FriendData);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_FRIENDLIST:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, \
				sizeof(TAFriendList) - ((FRIEND_MAXCOUNT + FRIEND_GROUP_MAX - packet.cCount) * sizeof(TDBFriendData)));
		}
		break;

	case QUERY_ADDGROUP:
		{
			TQAddGroup * pPacket = (TQAddGroup*)pData;
			TAAddGroup packet;
			memset(&packet, 0, sizeof(TAAddGroup));

			packet.nAccountDBID = pPacket->nAccountDBID;
			_wcscpy(packet.wszGroupName, _countof(packet.wszGroupName), pPacket->wszGroupName, (int)wcslen(pPacket->wszGroupName));
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryAddFriendGroup(pPacket->biCharacterDBID, pPacket->wszGroupName, pPacket->nGroupCountLimit, packet.nGroupDBID);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_ADDGROUP:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_MODGROUPNAME:
		{
			TQModGroupName * pPacket = (TQModGroupName*)pData;
			TAModGroupName packet;
			memset(&packet, 0, sizeof(TAModGroupName));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nGroupDBID = pPacket->nGroupDBID;
			_wcscpy(packet.wszGroupName, FRIEND_GROUP_NAMELENMAX, pPacket->wszGroupName, FRIEND_GROUP_NAMELENMAX);
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryModFriendGroupName(pPacket->biCharacterDBID, pPacket->nGroupDBID, pPacket->wszGroupName);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_MODGROUPNAME:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_DELGROUP:
		{
			TQDelGroup * pPacket = (TQDelGroup*)pData;
			TADelGroup packet;
			memset(&packet, 0, sizeof(TADelGroup));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nGroupDBID = pPacket->nGroupDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryDelFriendGroup(pPacket->biCharacterDBID, pPacket->nGroupDBID);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_DELGROUP:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_MODFRIENDANDGROUPMAPPING:
		{
			TQModFriendAndGroupMapping * pPacket = (TQModFriendAndGroupMapping*)pData;
			TAModFriendAndGroupMapping packet;
			memset(&packet, 0, sizeof(TAModFriendAndGroupMapping));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nGroupDBID = pPacket->nGroupDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryModFriendAndFriendGroupMapping(pPacket->biCharacterDBID, pPacket->cCount, pPacket->biFriendCharacterDBIDs, pPacket->nGroupDBID);
				if (packet.nRetCode == ERROR_NONE)
				{
					packet.cCount = pPacket->cCount;
					memcpy(packet.biFriendCharacterDBIDs, pPacket->biFriendCharacterDBIDs, (sizeof(packet.biFriendCharacterDBIDs)/sizeof(*packet.biFriendCharacterDBIDs)) * packet.cCount);
				}
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_MODFRIENDANDGROUPMAPPING:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet) - sizeof(packet.biFriendCharacterDBIDs) + (sizeof(INT64) * packet.cCount));
		}
		break;

	case QUERY_ADDFRIEND:
		{
			TQAddFriend * pPacket = (TQAddFriend*)pData;
			TAAddFriend packet;
			memset(&packet, 0, sizeof(TAAddFriend));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nGroupDBID = pPacket->nGroupDBID;
			_wcscpy(packet.wszFriendName, _countof(packet.wszFriendName), pPacket->wszFriendName, (int)wcslen(pPacket->wszFriendName));
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				bool bNeedDelIsolate = false;
				packet.nRetCode = pWorldDB->QueryAddFriend(pPacket->biCharacterDBID, pPacket->wszFriendName, L"", pPacket->nGroupDBID, pPacket->nFriendCountLimit, packet.biFriendCharacterDBID, packet.nFriendAccountDBID, bNeedDelIsolate);
				if (packet.nRetCode == ERROR_NONE)
					packet.cNeedDelIsolate = bNeedDelIsolate == true ? 1 : 0;
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_ADDFRIEND:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_DELFRIEND:
		{
			TQDelFriend * pPacket = (TQDelFriend*)pData;
			TADelFriend packet;
			memset(&packet, 0, sizeof(TADelFriend));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.cCount = pPacket->cCount;
			memcpy(packet.biFriendCharacterDBIDs, pPacket->biFriendCharacterDBIDs, sizeof(packet.biFriendCharacterDBIDs[0]) * packet.cCount);
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				packet.nRetCode = pWorldDB->QueryDelFriend(pPacket->biCharacterDBID, pPacket->cCount, pPacket->biFriendCharacterDBIDs);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_ADDFRIEND:%d] pWorldDB not found\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, pPacket->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet) - sizeof(packet.biFriendCharacterDBIDs) + sizeof(packet.biFriendCharacterDBIDs[0]) * packet.cCount );
		}
		break;

	case QUERY_MODFRIENDMEMO:
		{
			//메모는 안써요.
		}
		break;
	}
}