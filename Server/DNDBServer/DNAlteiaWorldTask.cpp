
#include "StdAfx.h"
#include "DNAlteiaWorldTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLWorld.h"
#include "Log.h"
#include "Util.h"

#if defined ( PRE_ALTEIAWORLD_EXPLORE )

CDNAlteiaWorldTask::CDNAlteiaWorldTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{
}

CDNAlteiaWorldTask::~CDNAlteiaWorldTask()
{
}

void CDNAlteiaWorldTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData) 
{
	switch (nSubCmd)
	{
	case QUERY_RESET_ALTEIAWORLDEVENT:
		{
			TQResetAlteiaWorldEvent* pAlteia = reinterpret_cast<TQResetAlteiaWorldEvent*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_RESET_ALTEIAWORLDEVENT:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}			

			int nRetCode = pWorldDB->QueryResetAlteiaWorldEvent(pAlteia);
			if (nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_RESET_ALTEIAWORLDEVENT:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, nRetCode);

		}
		break;
	case QUERY_ADD_ALTEIAWORLDPLAYRESULT:
		{
			TQADDAlteiaWorldPlayResult* pAlteia = reinterpret_cast<TQADDAlteiaWorldPlayResult*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_ADD_ALTEIAWORLDPLAYRESULT:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}			
			
			int nRetCode = pWorldDB->QueryAddAlteiaWorldPlayResult(pAlteia);
			if (nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_ADD_ALTEIAWORLDPLAYRESULT:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, nRetCode);
			
		}
		break;
	case QUERY_GET_ALTEIAWORLDINFO:
		{
			TQGetAlteiaWorldInfo* pAlteia = reinterpret_cast<TQGetAlteiaWorldInfo*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDINFO:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAGetAlteiaWorldInfo Alteia;
			memset(&Alteia, 0, sizeof(Alteia));
			
			Alteia.nAccountDBID = pAlteia->nAccountDBID;
			
			Alteia.nRetCode = pWorldDB->QueryGetAlteiaWorldInfo(pAlteia, &Alteia);
			if (Alteia.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDINFO:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, Alteia.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_GET_ALTEIAWORLDPRIVATEGOLDKEYRANK:
		{
			TQGetAlteiaWorldRankInfo* pAlteia = reinterpret_cast<TQGetAlteiaWorldRankInfo*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDPRIVATEGOLDKEYRANK:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAGetAlteiaWorldPrivateGoldKeyRank Alteia;
			memset(&Alteia, 0, sizeof(Alteia));

			Alteia.nAccountDBID = pAlteia->nAccountDBID;

			Alteia.nRetCode = pWorldDB->QueryGetAlteiaWorldPrivateGoldKeyRank(pAlteia, &Alteia);
			if (Alteia.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDPRIVATEGOLDKEYRANK:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, Alteia.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_GET_ALTEIAWORLDPRIVATEPLAYTIMERANK:
		{
			TQGetAlteiaWorldRankInfo* pAlteia = reinterpret_cast<TQGetAlteiaWorldRankInfo*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDPRIVATEPLAYTIMERANK:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAGetAlteiaWorldPrivatePlayTimeRank Alteia;
			memset(&Alteia, 0, sizeof(Alteia));

			Alteia.nAccountDBID = pAlteia->nAccountDBID;

			Alteia.nRetCode = pWorldDB->QueryGetAlteiaWorldPrivatePlayTimeRank(pAlteia, &Alteia);
			if (Alteia.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDPRIVATEPLAYTIMERANK:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, Alteia.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_GET_ALTEIAWORLDGUILDGOLDKEYRANK:
		{
			TQGetAlteiaWorldRankInfo* pAlteia = reinterpret_cast<TQGetAlteiaWorldRankInfo*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDGUILDGOLDKEYRANK:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAGetAlteiaWorldGuildGoldKeyRank Alteia;
			memset(&Alteia, 0, sizeof(Alteia));

			Alteia.nAccountDBID = pAlteia->nAccountDBID;

			Alteia.nRetCode = pWorldDB->QueryGetAlteiaWorldGuildGoldKeyRank(pAlteia, &Alteia);
			if (Alteia.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDGUILDGOLDKEYRANK:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, Alteia.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_GET_ALTEIAWORLDSENDTICKETLIST:
		{
			TQGetAlteiaWorldSendTicketList* pAlteia = reinterpret_cast<TQGetAlteiaWorldSendTicketList*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDSENDTICKETLIST:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAGetAlteiaWorldSendTicketList Alteia;
			memset(&Alteia, 0, sizeof(Alteia));

			Alteia.nAccountDBID = pAlteia->nAccountDBID;

			Alteia.nRetCode = pWorldDB->QueryGetAlteiaWorldSendTicketList(pAlteia, &Alteia);
			if (Alteia.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_GET_ALTEIAWORLDSENDTICKETLIST:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, Alteia.nRetCode);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_ADD_ALTEIAWORLDSENDTICKETLIST:
		{
			TQAddAlteiaWorldSendTicketList* pAlteia = reinterpret_cast<TQAddAlteiaWorldSendTicketList*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_ADD_ALTEIAWORLDSENDTICKETLIST:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			TAAddAlteiaWorldSendTicketList Alteia;
			memset(&Alteia, 0, sizeof(Alteia));

			Alteia.nAccountDBID = pAlteia->nAccountDBID;
			Alteia.biSendCharacterDBID = pAlteia->biSendCharacterDBID;
			_wcscpy( Alteia.wszSendCharacterName, _countof(Alteia.wszSendCharacterName), pAlteia->wszSendCharacterName, (int)wcslen(pAlteia->wszSendCharacterName) );
			
			Alteia.nRetCode = pWorldDB->QueryAddAlteiaWorldSendTicketList(pAlteia);			

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Alteia), sizeof(Alteia));
		}
		break;
	case QUERY_RESET_ALTEIAWORLDPLAYALTEIA:
		{
			TQResetAlteiaWorldPlayAlteia* pAlteia = reinterpret_cast<TQResetAlteiaWorldPlayAlteia*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_RESET_ALTEIAWORLDPLAYALTEIA:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			int nRetCode = pWorldDB->QueryResetAlteiaWorldPlayAlteia(pAlteia);
			if (nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, pAlteia->biCharacterDBID, 0, L"[CDBID:0] [QUERY_RESET_ALTEIAWORLDPLAYALTEIA:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, nRetCode);
		}
		break;
	case QUERY_ADD_ALTEIAWORLDEVENT:
		{
			TQAddAlteiaWorldEvent* pAlteia = reinterpret_cast<TQAddAlteiaWorldEvent*>(pData);

			CDNSQLWorld* pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pAlteia->cWorldSetID);
			if (!pWorldDB)
			{
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_ADD_ALTEIAWORLDEVENT:%d] pWorldDB not found\r\n", pAlteia->cWorldSetID);
				break;
			}

			int nRetCode = pWorldDB->QueryAddAlteiaWorldEvent(pAlteia);
			if (nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pAlteia->cWorldSetID, pAlteia->nAccountDBID, 0, 0, L"[CDBID:0] [QUERY_ADD_ALTEIAWORLDEVENT:%d] Query Error Ret:%d\r\n", pAlteia->cWorldSetID, nRetCode);
		}
		break;

	default:
		break;
	}
}
#endif // #if defined ( PRE_ALTEIAWORLD_EXPLORE )