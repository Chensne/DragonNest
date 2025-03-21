#include "StdAfx.h"
#include "DNActozCommonDBTask.h"
#include "DNSQLActozCommon.h"
#include "DNSQLConnectionManager.h"

#if defined(_KRAZ) && defined(_FINAL_BUILD)

CDNActozCommonDBTask::CDNActozCommonDBTask(CDNConnection *pConnection)
: CDNMessageTask(pConnection)
{
}

CDNActozCommonDBTask::~CDNActozCommonDBTask(void)
{
}

void CDNActozCommonDBTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLActozCommon *pActozCommon = NULL;

	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_ACTOZ_UPDATECHARACTERINFO:
		{
			TQActozUpdateCharacterInfo *pPacket = (TQActozUpdateCharacterInfo*)pData;

			pActozCommon = g_SQLConnectionManager.FindActozCommonDB(nThreadID);
			if (pActozCommon){
				nRet = pActozCommon->QueryUpdateCharacterInfo(pPacket->cUpdateType, pPacket->biCharacterDBID, pPacket->cJob, pPacket->nLevel, pPacket->nExp, pPacket->biCoin, pPacket->szIp);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, 
						L"[ADBID:%u] [QUERY_UPDATECHARACTERINFO:%d] Result:%d\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID, nRet);
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, 
					L"[ADBID:%u] [QUERY_UPDATECHARACTERINFO:%d] pActozCommon not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			}
		}
		break;

	case QUERY_ACTOZ_UPDATECHARACTERNAME:
		{
			TQActozUpdateCharacterName *pPacket = (TQActozUpdateCharacterName*)pData;

			pActozCommon = g_SQLConnectionManager.FindActozCommonDB(nThreadID);
			if (pActozCommon){
				nRet = pActozCommon->QueryUpdateCharacterName(pPacket->biCharacterDBID, pPacket->szCharacterName);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, 
						L"[ADBID:%u] [QUERY_UPDATECHARACTERINFO:%d] Result:%d\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID, nRet);
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, 
					L"[ADBID:%u] [QUERY_UPDATECHARACTERINFO:%d] pActozCommon not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			}
		}
		break;
	}
}

#endif	// #if defined(_KRAZ)