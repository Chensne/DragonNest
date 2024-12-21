#include "StdAfx.h"
#include "DNSkillTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNSkillTask::CDNSkillTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNSkillTask::~CDNSkillTask(void)
{
}

void CDNSkillTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
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
	case QUERY_ADDSKILL:
		{
			TQAddSkill *pUpdate = (TQAddSkill*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				if( pUpdate->biCurrentCoin > 0 && pUpdate->biPickUpCoin > 0 )
				{
					INT64 biTotalCoin = 0;
					nRet = pWorldDB->QueryMidtermCoin(pUpdate->biCharacterDBID, pUpdate->biPickUpCoin, pUpdate->nChannelID, pUpdate->nMapID, biTotalCoin);
					if( nRet != ERROR_NONE || biTotalCoin != pUpdate->biCurrentCoin )
					{
						g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QueryMidtermCoin:%d] Query Error Ret:%d DBCoint:%I64d ServerCoin:%I64d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet, biTotalCoin, pUpdate->biCurrentCoin);
						nRet = ERROR_DB;
						m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
						break;
					}
				}
				nRet = pWorldDB->QueryAddSkill(pUpdate->biCharacterDBID, pUpdate->nSkillID, pUpdate->cSkillLevel, pUpdate->nCoolTime, pUpdate->cSkillChangeCode, pUpdate->biCoin, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->cSkillPage);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDSKILL:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDSKILL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_MODSKILLLEVEL:
		{
			TQModSkillLevel *pUpdate = (TQModSkillLevel*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModSkillLevel(pUpdate->biCharacterDBID, pUpdate->nSkillID, pUpdate->cSkillLevel, pUpdate->nCoolTime, pUpdate->wUsedSkillPoint, pUpdate->cSkillChangeCode, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->cSkillPage);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MODSKILLLEVEL:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MODSKILLLEVEL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_INCREASESKILLPOINT:
		{
			TQIncreaseSkillPoint *pUpdate = (TQIncreaseSkillPoint*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryIncreaseSkillPoint(pUpdate->biCharacterDBID, pUpdate->wSkillPoint, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->cSkillPointChangeCode, pUpdate->cSkillPage);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_INCREASESKILLPOINT:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_INCREASESKILLPOINT:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;
	case QUERY_DECREASESKILLPOINT:
		{
			TQDecreaseSkillPoint *pUpdate = (TQDecreaseSkillPoint*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryDecreaseSkillPoint( pUpdate );
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DECREASESKILLPOINT:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DECREASESKILLPOINT:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_DELSKILL:
		{
			TQDelSkill *pUpdate = (TQDelSkill*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryDelSkill(pUpdate->biCharacterDBID, pUpdate->nSkillID, pUpdate->cSkillChangeCode, pUpdate->bSkillPointBack, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->cSkillPage);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELSKILL:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DELSKILL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_RESETSKILL:
		{
			TQResetSkill *pUpdate = (TQResetSkill*)pData;
			TAResetSkill Update;
			memset(&Update, 0, sizeof(TAResetSkill));

			Update.nAccountDBID = pUpdate->nAccountDBID;
			Update.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				Update.cSkillPage = pUpdate->cSkillPage;
				Update.nRetCode = pWorldDB->QueryResetSkill(pUpdate->biCharacterDBID, pUpdate->nChannelID, pUpdate->nMapID, Update.wSkillPoint, Update.wTotalSkillPoint, pUpdate->cSkillPage);
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_RESETSKILL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Update, sizeof(Update));
		}
		break;

	case QUERY_RESETSKILLBYSKILLIDS:
		{
			TQResetSkillBySkillIDS* pUpdate = reinterpret_cast<TQResetSkillBySkillIDS*>(pData);
			TAResetSkillBySkillIDS Update;
			memset( &Update, 0, sizeof(Update) );

			Update.nAccountDBID = pUpdate->nAccountDBID;
			Update.nRetCode		= ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB)
			{
				Update.cSkillPage = pUpdate->cSkillPage;
				Update.nRetCode = pWorldDB->QueryResetSkillBySkillIDs( pUpdate, &Update );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_RESETSKILLBYSKILLIDS:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Update, sizeof(Update));
			break;
		}
	case QUERY_USEEXPANDSKILLPAGE:
		{
			TQUseExpandSkillPage* pUpdate = reinterpret_cast<TQUseExpandSkillPage *>(pData);
			TAUseExpandSkillPage Update;
			memset( &Update, 0, sizeof(Update) );

			Update.nAccountDBID = pUpdate->nAccountDBID;
			Update.nRetCode	= ERROR_DB;
			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				Update.nRetCode = pWorldDB->QueryUseExpandSkillPage(pUpdate->biCharacterDBID, pUpdate->nSkillArray, pUpdate->wSkillPoint);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Update, sizeof(Update));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_USEEXPANDSKILLPAGE:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;
	case QUERY_SETSKILLPOINT:
		{
			TQSetSkillPoint *pUpdate = (TQSetSkillPoint*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QuerySetSkillPoint( pUpdate );
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SETSKILLPOINT:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// µðºñ¿¡¼­ ¹º°¡ ¿¡·¯°ªÀ» ¹ñ¾î³ÂÀ¸´Ï °Á ²÷¾î¹ö¸°´Ù
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SETSKILLPOINT:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;
	}
}