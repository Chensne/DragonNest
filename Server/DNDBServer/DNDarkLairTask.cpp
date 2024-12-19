#include "StdAfx.h"
#include "DNDarkLairTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNDarkLairTask::CDNDarkLairTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNDarkLairTask::~CDNDarkLairTask(void)
{
}

void CDNDarkLairTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_UPDATE_DARKLAIR_RESULT:
		{
			TQUpdateDarkLairResult* pDarkLair = reinterpret_cast<TQUpdateDarkLairResult*>(pData);
			TAUpdateDarkLairResult DarkLair;
			memset( &DarkLair, 0, sizeof(DarkLair) );

			DarkLair.nRetCode			= ERROR_DB;
			DarkLair.iRoomID			= pDarkLair->iRoomID;
			DarkLair.iProtectionKey	= pDarkLair->iProtectionKey;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pDarkLair->cWorldSetID );
			if( pWorldDB )
			{
				DarkLair.nRetCode = pWorldDB->QueryUpdateDarkLairResult( pDarkLair, &DarkLair);
			}

			if( DarkLair.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pDarkLair->cWorldSetID, pDarkLair->nAccountDBID, 0, 0, L"[QUERY_UPDATE_DARKLAIR_RESULT:%d] (Ret:%d)\r\n", pDarkLair->cWorldSetID, DarkLair.nRetCode);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&DarkLair), sizeof(DarkLair) );
			break;
		}
	case QUERY_GET_DARKLAIR_RANK_BOARD:
		{
			TQGetDarkLairRankBoard* pDarkLair = reinterpret_cast<TQGetDarkLairRankBoard*>(pData);
			TAGetDarkLairRankBoard DarkLair;
			memset( &DarkLair, 0, sizeof(DarkLair) );

			DarkLair.nRetCode			= ERROR_DB;
			DarkLair.nAccountDBID		= pDarkLair->nAccountDBID;
			DarkLair.cPartyUserCount	= pDarkLair->cPartyUserCount;
#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
			DarkLair.iMapIndex			= pDarkLair->iMapIndex;
#endif

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pDarkLair->cWorldSetID );
			if( pWorldDB )
			{
				do 
				{
					// 개인베스트 얻기
					TQUpdateDarkLairResult pTempQ;
					memset( &pTempQ, 0, sizeof(pTempQ) );

					pTempQ.sUserData[0].i64CharacterDBID	= pDarkLair->biCharacterDBID;
					pTempQ.iMapIndex						= pDarkLair->iMapIndex;
					pTempQ.cPartyUserCount					= pDarkLair->cPartyUserCount;

					TAUpdateDarkLairResult pTempA;
					memset( &pTempA, 0, sizeof(pTempA) );

					DarkLair.nRetCode = pWorldDB->QueryGetDarkLairPersonalBest( &pTempQ, &pTempA, 0 );
					if( DarkLair.nRetCode != ERROR_NONE )
						break;
					DarkLair.sBestHistory	= pTempA.sBestUserData[0];

					// 다크레어 Rank 얻기
					DarkLair.nRetCode = pWorldDB->QueryGetListDarkLairTopRanker( pDarkLair, &DarkLair );
				}while( false );
			}

			if( DarkLair.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pDarkLair->cWorldSetID, pDarkLair->nAccountDBID, pDarkLair->biCharacterDBID, 0, L"[QUERY_GET_DARKLAIR_RANK_BOARD:%d] (Ret:%d)\r\n", pDarkLair->cWorldSetID, DarkLair.nRetCode);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&DarkLair), sizeof(DarkLair) );
			break;
		}
	}
}