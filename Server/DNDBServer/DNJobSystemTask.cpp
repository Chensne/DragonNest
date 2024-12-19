#include "StdAfx.h"
#include "DNJobSystemTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "DNExtManager.h"
#include "Log.h"
#include "Util.h"

CDNJobSystemTask::CDNJobSystemTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNJobSystemTask::~CDNJobSystemTask(void)
{
}

void CDNJobSystemTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_ADD_JOB_RESERVE :
		{
			TQAddJobReserve* pPacket = reinterpret_cast<TQAddJobReserve*>(pData);
			TAAddJobReserve Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.cJobType = pPacket->cJobType;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryAddJobReserve(pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[QueryAddJobReserve:%d] WorldDB not found\r\n", pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );						
		}
		break;
	case QUERY_GET_JOB_RESERVE :  
		{
			TQGetJobReserve* pPacket = reinterpret_cast<TQGetJobReserve*>(pData);
			TAGetJobReserve Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.nJobSeq = pPacket->nJobSeq;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetJobReserve(pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[QueryGetJobReserve:%d] WorldDB not found\r\n", pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
		}
		break;
	}
}
