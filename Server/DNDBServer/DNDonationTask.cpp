
#include "StdAfx.h"
#include "DNDonationTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLWorld.h"
#include "Log.h"

#if defined (PRE_ADD_DONATION)

CDNDonationTask::CDNDonationTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{
}

CDNDonationTask::~CDNDonationTask()
{
}

void CDNDonationTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	switch (nSubCmd)
	{
	case QUERY_DONATE:
		{
			TQDonate* pPacket = reinterpret_cast<TQDonate*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_DONATE:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TADonate Ack;
			ZeroMemory(&Ack, sizeof(Ack));
			Ack.nAccountDBID = pPacket->nAccountDBID;
			Ack.nRetCode = pWorldDB->QueryDonate(pPacket);
			if (Ack.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_DONATE:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, Ack.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Ack), sizeof(Ack));
		}
		break;

	case QUERY_DONATION_RANKING:
		{
			TQDonationRanking* pPacket = reinterpret_cast<TQDonationRanking*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_DONATION_RANKING:%d] pWorldDB not found\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID);
				break;
			}

			TADonationRaking Ack;
			ZeroMemory(&Ack, sizeof(Ack));
			Ack.nAccountDBID = pPacket->nAccountDBID;
			Ack.nRetCode = pWorldDB->QueryDonationRanking(pPacket, Ack);
			if (Ack.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, pPacket->biCharacterDBID, 0, L"[CDBID:%I64d] [QUERY_DONATION_RANKING:%d] Query Error Ret:%d\r\n", pPacket->biCharacterDBID, pPacket->cWorldSetID, Ack.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Ack), sizeof(Ack));
		}
		break;

	case QUERY_DONATION_TOP_RANKER:
		{
			TQDonationTopRanker* pPacket = reinterpret_cast<TQDonationTopRanker*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, 0, 0, L"[QUERY_DONATION_TOP_RANKER:%d] pWorldDB not found\r\n", pPacket->cWorldSetID);
				break;
			}

			TADonationTopRanker Ack;
			ZeroMemory(&Ack, sizeof(Ack));
			Ack.cWorldID = pPacket->cWorldSetID;
			Ack.nRetCode = pWorldDB->QueryDonationTopRanker(pPacket, Ack);
			if (Ack.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, 0, 0, 0, L"[QUERY_DONATION_TOP_RANKER:%d] Query Error Ret:%d\r\n", pPacket->cWorldSetID, Ack.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Ack), sizeof(Ack));
		}
		break;

	default:
		break;
	}
}
#endif // #if defined (PRE_ADD_DONATION)