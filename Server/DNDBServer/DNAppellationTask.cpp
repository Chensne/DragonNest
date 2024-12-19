#include "StdAfx.h"
#include "DNAppellationTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNAppellationTask::CDNAppellationTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNAppellationTask::~CDNAppellationTask(void)
{
}

void CDNAppellationTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_ADDAPPELLATION:
		{
			TQAddAppellation *pUpdate = (TQAddAppellation*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryAddAppellation(pUpdate->biCharacterDBID, pUpdate->nAppellationID, pUpdate->Appellation);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDAPPELLATION:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDAPPELLATION:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_SELECTAPPELLATION:
		{
			TQSelectAppellation *pUpdate = (TQSelectAppellation*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryChoiceAppellation(pUpdate->biCharacterDBID, pUpdate->nSelectAppellation, pUpdate->cAppellationKind);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SELECTAPPELLATION:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SELECTAPPELLATION:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;
	case QUERY_DELETEAPPELLATION:
		{	
			TQDelAppellation *pDelete = (TQDelAppellation*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pDelete->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryDeleteAppellation(pDelete->biCharacterDBID, pDelete->nDelAppellation);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pDelete->cWorldSetID, 0, pDelete->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELETEAPPELLATION:%d] Query Error Ret:%d\r\n", pDelete->biCharacterDBID, pDelete->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pDelete->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pDelete->cWorldSetID, 0, pDelete->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELETEAPPELLATION:%d] pWorldDB not found\r\n", pDelete->biCharacterDBID, pDelete->cWorldSetID);
			}
		}
		break;
	}
}