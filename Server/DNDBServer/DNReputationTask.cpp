#include "StdAfx.h"
#include "DNReputationTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNReputationTask::CDNReputationTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNReputationTask::~CDNReputationTask(void)
{
}

#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
void CDNReputationTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch( nSubCmd )
	{
	case QUERY_MOD_NPCLOCATION:
		{
			TQModNPCLocation* pPacket = reinterpret_cast<TQModNPCLocation*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				nRet = pWorldDB->QueryModNpcLocation( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_NPCLOCATION:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( nRet != ERROR_NONE )
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_NPCLOCATION:%d] (Ret:%d)\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID, nRet);
			break;
		}

	case QUERY_MOD_NPCFAVOR:
		{
			TQModNPCFavor* pPacket = reinterpret_cast<TQModNPCFavor*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				nRet = pWorldDB->QueryModNpcFavor( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_NPCFAVOR:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( nRet != ERROR_NONE )
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_NPCFAVOR:%d] (Ret:%d)\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID, nRet);
			break;
		}

	case QUERT_GET_LISTNPCFAVOR:
		{
			TQGetListNpcFavor* pPacket = reinterpret_cast<TQGetListNpcFavor*>(pData);
			TAGetListNpcFavor Packet;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			Packet.nRetCode		= ERROR_DB;
			Packet.bLastPage	= true;
			Packet.cCount		= 0;

			std::vector<TNpcReputation> vReputation;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				nRet = pWorldDB->QueryGetListNpcFavor( pPacket, vReputation );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERT_GET_LISTNPCFAVOR:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( nRet != ERROR_NONE )
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERT_GET_LISTNPCFAVOR:%d] (Ret:%d)\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID, nRet);

			int iSendCount = 0;
			do 
			{
				if( vReputation.size()-iSendCount > _countof(Packet.ReputationArr) )
				{
					Packet.bLastPage	= false;
					Packet.cCount		= _countof(Packet.ReputationArr);
				}
				else
				{
					Packet.bLastPage	= true;
					Packet.cCount		= static_cast<BYTE>(vReputation.size()-iSendCount);
				}

				// 패킷만들기~
				for( UINT i=0 ; i<Packet.cCount ; ++i )
					Packet.ReputationArr[i] = vReputation[iSendCount+i];

				iSendCount += Packet.cCount;

				int iSize = sizeof(Packet)-sizeof(Packet.ReputationArr)+Packet.cCount*sizeof(Packet.ReputationArr[0]);
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			} while( iSendCount < (int)vReputation.size() );

			break;
		}
	}
}
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
