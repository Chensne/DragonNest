#include "StdAfx.h"
#include "DNQuestTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNQuestTask::CDNQuestTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNQuestTask::~CDNQuestTask(void)
{
}

void CDNQuestTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	TQHeader *pHeader = (TQHeader*)pData;
	if( pHeader->nAccountDBID > 0)
	{
		if( g_pSPErrorCheckManager->bIsError(pHeader->nAccountDBID) == true )
		{
			//g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, 0, 0, L"[SP_ERRORCHECK] Main:%d, Sub:%d\r\n", nMainCmd, nSubCmd);
			return;
		}
	}

	switch (nSubCmd)
	{
		// Quest
	case QUERY_ADDQUEST:
		{
			TQAddQuest *pQuest = (TQAddQuest*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryAddQuest(pQuest->biCharacterDBID, pQuest->cIdx, pQuest->Quest);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDQUEST:%d] Query Error Ret:%d\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pQuest->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDQUEST:%d] pWorldDB not found\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID);
			}
		}
		break;

	case QUERY_DELQUEST:
		{
			TQDelQuest *pQuest = (TQDelQuest*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryDelQuest(pQuest->biCharacterDBID, pQuest->cIdx, pQuest->nQuestID, pQuest->bComplete, pQuest->bRepeat, pQuest->CompleteQuest);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELQUEST:%d] Query Error Ret:%d\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pQuest->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELQUEST:%d] pWorldDB not found\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID);
			}
		}
		break;

	case QUERY_MODQUEST:
		{
			TQModQuest *pQuest = (TQModQuest*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				// 현재는 저장Bit 를 나누기가 뭐해서..우선은 모든 비트속성 다 저장한다.(나중에 빌리지서버,게임서버 쪽에서 바뀐거만 컨트롤해서 조절해줘야 한다.)
				nRet = pWorldDB->QueryModQuest(pQuest->biCharacterDBID, pQuest->cIndex, 0xFFFFFFFF, pQuest->cQuestStatusCode, pQuest->Quest);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MODQUEST:%d] Query Error Ret:%d\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pQuest->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MODQUEST:%d] pWorldDB not found\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID);
			}
		}
		break;

	case QUERY_CLEARQUEST:		// 퀘스트 초기화
		{
			TQClearQuest *pQuest = reinterpret_cast<TQClearQuest*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				nRet = pWorldDB->QueryClearQuest(pQuest->biCharacterDBID);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CLEARQUEST:%d] Query Error Ret:%d\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pQuest->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else {
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CLEARQUEST:%d] pWorldDB not found\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID);
			}
		}
		break;

	case QUERY_FORCECOMPLETEQUEST:			// 퀘스트 강제 완료
		{
			TQForceCompleteQuest* pQuest = reinterpret_cast<TQForceCompleteQuest*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				nRet = pWorldDB->QueryAddCompleteQuest(pQuest->biCharacterDBID, pQuest->nQuestID, pQuest->nQuestCode);
				if (nRet != ERROR_NONE) {
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_FORCECOMPLETEQUEST QueryAddCompleteQuest:%d] Query Error Ret:%d\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pQuest->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
					break;
				}
			}
			else {
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_FORCECOMPLETEQUEST:%d] pWorldDB not found\r\n", pQuest->biCharacterDBID, pQuest->cWorldSetID);
			}
		}
		break;

	case QUERY_DELETE_CHARACTER_PERIODQUEST:
		{
			TQDeletePeriodQuest *pQuest = reinterpret_cast<TQDeletePeriodQuest*>(pData);

			TADeletePeriodQuest Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldID = pQuest->cWorldSetID;
			Quest.nAccountDBID = pQuest->nAccountDBID;
			Quest.biCharacterDBID = pQuest->biCharacterDBID;
			Quest.nPeriodQuestType = pQuest->nPeriodQuestType;
			Quest.nQuestCount = pQuest->nQuestCount;
			memcpy (Quest.nQuestIDs, pQuest->nQuestIDs, sizeof(int)*pQuest->nQuestCount);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryDeletePeriodQuestList(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_DELETE_CHARACTER_PERIODQUEST:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_DELETE_CHARACTER_PERIODQUEST:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			int nLen = sizeof(TADeletePeriodQuest) - sizeof(Quest.nQuestIDs) + (sizeof(int)*Quest.nQuestCount);
			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, nLen);
		}
		break;

	case QUERY_GET_CHARACTER_PERIODQUESTDATE:
		{
			TQGetPeriodQuestDate *pQuest = reinterpret_cast<TQGetPeriodQuestDate*>(pData);

			TAGetPeriodQuestDate Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldID = pQuest->cWorldSetID;
			Quest.nAccountDBID = pQuest->nAccountDBID;
			Quest.biCharacterDBID = pQuest->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryGetPeriodQuestDate(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_GET_CHARACTER_PERIODQUESTDATE:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_GET_CHARACTER_PERIODQUESTDATE:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			int nLen = sizeof(TAGetPeriodQuestDate) - sizeof(Quest.PeriodQuestDate) + (sizeof(TPeriodQuestDate)*Quest.nCount);
			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, nLen);
		}
		break;

	case QUERY_GET_LISTCOMPLETE_EVENTQUEST:
		{
			TQGetListCompleteEventQuest *pQuest = reinterpret_cast<TQGetListCompleteEventQuest*>(pData);

			TAGetListCompleteEventQuest Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldID = pQuest->cWorldSetID;
			Quest.nAccountDBID = pQuest->nAccountDBID;
			Quest.biCharacterDBID = pQuest->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryGetListCompleteEventQuest(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_GET_LISTCOMPLETE_EVENTQUEST:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_GET_LISTCOMPLETE_EVENTQUEST:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			int nLen = sizeof(TAGetListCompleteEventQuest) - sizeof(Quest.EventQuestList) + (sizeof(TCompleteEventQuest)*Quest.nCount);
			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, nLen);
		}
		break;

	case QUERY_COMPLETE_EVENTQUEST:
		{
			TQCompleteEventQuest *pQuest = reinterpret_cast<TQCompleteEventQuest*>(pData);

			TACompleteEventQuest Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldID = pQuest->cWorldSetID;
			Quest.nAccountDBID = pQuest->nAccountDBID;
			Quest.biCharacterDBID = pQuest->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryCompleteEventQuest(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_COMPLETE_EVENTQUEST:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_COMPLETE_EVENTQUEST:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, sizeof(TACompleteEventQuest));
		}
		break;

	case QUERY_GET_WORLDEVENTQUESTCOUNTER:
		{
			TQGetWorldEventQuestCounter *pQuest = reinterpret_cast<TQGetWorldEventQuestCounter*>(pData);

			TAGetWorldEventQuestCounter Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldSetID = pQuest->cWorldSetID;
			Quest.nScheduleID = pQuest->nScheduleID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryGetWorldEventQuestCounter(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, 0, 0, L"[QUERY_GET_WORLDEVENTQUESTCOUNTER:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, 0, 0, L"[QUERY_GET_WORLDEVENTQUESTCOUNTER:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, sizeof(TAGetWorldEventQuestCounter));
		}
		break;

	case QUERY_MOD_WORLDEVENTQUESTCOUNTER:
		{
			TQModWorldEventQuestCounter *pQuest = reinterpret_cast<TQModWorldEventQuestCounter*>(pData);

			TAModWorldEventQuestCounter Quest;
			memset(&Quest, 0, sizeof(Quest));

			Quest.nRetCode = ERROR_DB;
			Quest.cWorldSetID = pQuest->cWorldSetID;
			Quest.nScheduleID = pQuest->nScheduleID;
			Quest.nCounterDelta = pQuest->nCounterDelta;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pQuest->cWorldSetID);
			if (pWorldDB)
			{
				Quest.nRetCode = pWorldDB->QueryModWorldEventQuestCounter(pQuest, &Quest);
				if (Quest.nRetCode != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_MOD_WORLDEVENTQUESTCOUNTER:%d] Query Error Ret:%d\r\n", pQuest->cWorldSetID, Quest.nRetCode);
					m_pConnection->QueryResultError(pQuest->nAccountDBID, Quest.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pQuest->cWorldSetID, pQuest->nAccountDBID, pQuest->biCharacterDBID, 0, L"[QUERY_MOD_WORLDEVENTQUESTCOUNTER:%d] WorldDB not found\r\n", pQuest->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Quest, sizeof(TAModWorldEventQuestCounter));
		}
		break;
	}
}