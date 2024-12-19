#include "StdAfx.h"
#include "DNGuildRecruitSystemTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"
#include "Util.h"

CDNGuildRecruitSystemTask::CDNGuildRecruitSystemTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNGuildRecruitSystemTask::~CDNGuildRecruitSystemTask(void)
{
}

void CDNGuildRecruitSystemTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch( nSubCmd )
	{
	case QUERY_GET_PAGEGUILDRECRUIT:
		{
			TQGetGuildRecruit* pPacket = reinterpret_cast<TQGetGuildRecruit*>(pData);

			TAGetGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.cJobCode		= pPacket->cJobCode;
			Packet.cLevel		= pPacket->cLevel;
			Packet.uiPage		= pPacket->uiPage;
#if defined( PRE_ADD_GUILD_EASYSYSTEM )		
			_wcscpy( Packet.wszGuildName, _countof(Packet.wszGuildName), pPacket->wszGuildName, (int)wcslen(pPacket->wszGuildName) );
#endif

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGetPageGuildRecruit( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_PAGEGUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.GuildRecruitList)+(Packet.cCount*sizeof(Packet.GuildRecruitList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );			
		}	
		break;
	case QUERY_GET_PAGEGUILDRECRUITCHARACTER:
		{
			TQGetGuildRecruitCharacter* pPacket = reinterpret_cast<TQGetGuildRecruitCharacter*>(pData);

			TAGetGuildRecruitCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;	
			Packet.nGuildDBID	= pPacket->nGuildDBID;
			Packet.cWorldSetID	= pPacket->cWorldSetID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGetPageGuildRecruitCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_PAGEGUILDRECRUITCHARACTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.GuildRecruitCharacterList)+(Packet.cCount*sizeof(Packet.GuildRecruitCharacterList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
		}
		break;
	case QUERY_GET_MYGUILDRECRUIT:
		{
			TQGetMyGuildRecruit* pPacket = reinterpret_cast<TQGetMyGuildRecruit*>(pData);

			TAGetMyGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryMyGuildRecruit( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_MYGUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.GuildRecruitList)+(Packet.cCount*sizeof(Packet.GuildRecruitList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );		
		}
		break;
	case QUERY_GET_GUILDRECRUIT_REQUESTCOUNT:
		{
			TQGetGuildRecruitRequestCount* pPacket = reinterpret_cast<TQGetGuildRecruitRequestCount*>(pData);

			TAGetGuildRecruitRequestCount Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGuildRecruitRequestCount( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_GUILDRECRUIT_REQUESTCOUNT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );		
		}
		break;
	case QUERY_REGISTERINFO_GUILDRECRUIT:
		{
			TQRegisterInfoGuildRecruit* pPacket = reinterpret_cast<TQRegisterInfoGuildRecruit*>(pData);

			TARegisterInfoGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryRegisterInofGuildRecruit( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTERINFO_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_REGISTERON_GUILDRECRUIT:
		{
			TQRegisterOnGuildRecruit* pPacket = reinterpret_cast<TQRegisterOnGuildRecruit*>(pData);

			TARegisterOnGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryRegisterOnGuildRecruit( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTERON_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_REGISTERMOD_GUILDRECRUIT:
		{
			TQRegisterModGuildRecruit* pPacket = reinterpret_cast<TQRegisterModGuildRecruit*>(pData);

			TARegisterModGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryRegisterModGuildRecruit( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTERMOD_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_REGISTEROFF_GUILDRECRUIT:
		{
			TQRegisterOffGuildRecruit* pPacket = reinterpret_cast<TQRegisterOffGuildRecruit*>(pData);

			TARegisterOffGuildRecruit Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryRegisterOffGuildRecruit( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTEROFF_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_REQUESTON_GUILDRECRUIT:
		{
			TQGuildRecruitRequestOn* pPacket = reinterpret_cast<TQGuildRecruitRequestOn*>(pData);

			TAGuildRecruitRequestOn Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGuildRecruitRequestOn( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REQUESTON_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_REQUESTOFF_GUILDRECRUIT:
		{
			TQGuildRecruitRequestOff* pPacket = reinterpret_cast<TQGuildRecruitRequestOff*>(pData);

			TAGuildRecruitRequestOff Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGuildRecruitRequestOff( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REQUESTOFF_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_ACCEPTON_GUILDRECRUIT:
		{
			TQGuildRecruitAcceptOn* pPacket = reinterpret_cast<TQGuildRecruitAcceptOn*>(pData);

			TAGuildRecruitAcceptOn Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.biAddCharacterDBID = pPacket->biAcceptCharacterDBID;
			_wcscpy( Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), pPacket->wszToCharacterName, (int)wcslen(pPacket->wszToCharacterName) );

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGuildRecruitAcceptOn( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_ACCEPTON_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_ACCEPTOFF_GUILDRECRUIT:
		{
			TQGuildRecruitAcceptOff* pPacket = reinterpret_cast<TQGuildRecruitAcceptOff*>(pData);

			TAGuildRecruitAcceptOff Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.biDenyCharacterDBID	= pPacket->biDenyCharacterDBID;
			_wcscpy( Packet.wszToCharacterName, _countof(Packet.wszToCharacterName), pPacket->wszToCharacterName, (int)wcslen(pPacket->wszToCharacterName) );

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGuildRecruitAcceptOff( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_ACCEPTOFF_GUILDRECRUIT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );	
		}
		break;
	case QUERY_DELETE_GUILDRECRUIT:
		{
			TQDelGuildRecruit *pPacket = reinterpret_cast<TQDelGuildRecruit *>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
			{
				int nRetCode = 0;
				nRetCode = pWorldDB->QueryDelGuildRecruit(pPacket);				

				if (ERROR_NONE != nRetCode)
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[QUERY_DELETE_GUILDRECRUIT] query error (ret:%d)\r\n", nRetCode);
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[QUERY_DELETE_GUILDRECRUIT] worldDB not found\r\n");
		}
		break;
	}
}