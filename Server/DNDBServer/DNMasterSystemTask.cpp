#include "StdAfx.h"
#include "DNMasterSystemTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

CDNMasterSystemTask::CDNMasterSystemTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNMasterSystemTask::~CDNMasterSystemTask(void)
{
}

void CDNMasterSystemTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch( nSubCmd )
	{
	case QUERY_GET_SIMPLEINFO:
		{
			TQGetMasterPupilInfo* pPacket = reinterpret_cast<TQGetMasterPupilInfo*>(pData);

			TAGetMasterPupilInfo Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.bClientSend	= pPacket->bClientSend;
			Packet.EventCode	= pPacket->EventCode;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGetMasterPupilInfo2( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_SIMPLEINFO:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.SimpleInfo.OppositeInfo)+(Packet.SimpleInfo.cCharacterDBIDCount*sizeof(Packet.SimpleInfo.OppositeInfo[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
	case QUERY_GET_PAGEMASTERCHARACTER:
		{
			TQGetPageMasterCharacter* pPacket = reinterpret_cast<TQGetPageMasterCharacter*>(pData);

			TAGetPageMasterCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.uiPage		= pPacket->uiPage;
			Packet.cJobCode		= pPacket->cJobCode;
			Packet.cGenderCode	= pPacket->cGenderCode;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				Packet.nRetCode = pWorldDB->QueryGetPageMasterCharacter( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_PAGEMASTERCHARACTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.MasterInfoList)+(Packet.cCount*sizeof(Packet.MasterInfoList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
	case QUERY_GET_MASTERCHARACTER_TYPE1:
	case QUERY_GET_MASTERCHARACTER_TYPE2:
		{
			TQGetMasterCharacter* pPacket = reinterpret_cast<TQGetMasterCharacter*>(pData);

			switch( pPacket->Code )
			{
			case DBDNWorldDef::GetMasterCharacterCode::MyInfo:
				{
					TAGetMasterCharacterType1 Packet;
					memset( &Packet, 0, sizeof(Packet) );

					Packet.nRetCode					= ERROR_DB;
					Packet.nAccountDBID				= pPacket->nAccountDBID;
					Packet.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;

					pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
					if( pWorldDB )
						Packet.nRetCode = pWorldDB->QueryGetMasterCharacterType1( pPacket, &Packet );
					else
						g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_MASTERCHARACTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

					m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
					break;
				}
			case DBDNWorldDef::GetMasterCharacterCode::MyMasterInfo:
				{
					TAGetMasterCharacterType2 Packet;
					memset( &Packet, 0, sizeof(Packet) );

					Packet.nRetCode					= ERROR_DB;
					Packet.nAccountDBID				= pPacket->nAccountDBID;
					Packet.biPupilCharacterDBID		= pPacket->biPupilCharacterDBID;
					Packet.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;

					pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
					if( pWorldDB )
						Packet.nRetCode = pWorldDB->QueryGetMasterCharacterType2( pPacket, &Packet );
					else
						g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_MASTERCHARACTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

					m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
					break;
				}
			default:
				{
					_DANGER_POINT();
					break;
				}
			}

			break;
		}
	case QUERY_GET_PUPILLIST:
		{
			TQGetListPupil* pPacket = reinterpret_cast<TQGetListPupil*>(pData);

			TAGetListPupil Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListPupil( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_PUPILLIST:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.PupilInfoList)+Packet.cCount*sizeof(Packet.PupilInfoList[0]);
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
	case QUERY_REGISTER_MASTER:
		{
			TQAddMasterCharacter* pPacket = reinterpret_cast<TQAddMasterCharacter*>(pData);

			TAAddMasterCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;
			_wcscpy( Packet.wszSelfIntroduction, _countof(Packet.wszSelfIntroduction), pPacket->wszSelfIntroduction, (int)wcslen(pPacket->wszSelfIntroduction) );

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryAddMasterCharacter( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTER_MASTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_REGISTERCANCEL_MASTER:
		{
			TQDelMasterCharacter* pPacket = reinterpret_cast<TQDelMasterCharacter*>(pData);

			TADelMasterCharacter Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryDelMasterCharacter( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_REGISTERCANCEL_MASTER:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_JOIN_MASTERSYSTEM:
		{
			TQAddMasterAndPupil* pPacket = reinterpret_cast<TQAddMasterAndPupil*>(pData);

			TAAddMasterAndPupil Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode					= ERROR_DB;
			Packet.nAccountDBID				= pPacket->nAccountDBID;
			Packet.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;
			Packet.biPupilCharacterDBID		= pPacket->biPupilCharacterDBID;
			Packet.bIsDirectMenu			= pPacket->bIsDirectMenu;
			Packet.Code						= pPacket->Code;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryAddMasterAndPupil( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_JOIN_MASTERSYSTEM:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
			{
				TQGetMasterPupilInfo Request;
				memset( &Request, 0, sizeof(Request) );

				Request.cThreadID		= pPacket->cThreadID;
				Request.cWorldSetID		= pPacket->cWorldSetID;
				Request.nAccountDBID	= pPacket->nAccountDBID;
				Request.biCharacterDBID	= (pPacket->Code == DBDNWorldDef::TransactorCode::Master) ? pPacket->biMasterCharacterDBID : pPacket->biPupilCharacterDBID;
				Request.bClientSend		= true;
				Request.EventCode		= MasterSystem::EventType::Join;

				OnRecvMessage( pPacket->cThreadID, MAINCMD_MASTERSYSTEM, QUERY_GET_SIMPLEINFO, reinterpret_cast<char*>(&Request) );
			}

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_LEAVE_MASTERSYSTEM:
		{
			TQDelMasterAndPupil* pPacket = reinterpret_cast<TQDelMasterAndPupil*>(pData);

			TADelMasterAndPupil Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode					= ERROR_DB;
			Packet.nAccountDBID				= pPacket->nAccountDBID;
			Packet.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;
			Packet.biPupilCharacterDBID		= pPacket->biPupilCharacterDBID;
			Packet.Code						= pPacket->Code;
			Packet.iPenaltyRespectPoint		= pPacket->iPenaltyRespectPoint;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryDelMasterAndPupil( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_LEAVE_MASTERSYSTEM:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
			{
				TQGetMasterPupilInfo Request;
				memset( &Request, 0, sizeof(Request) );

				Request.cThreadID		= pPacket->cThreadID;
				Request.cWorldSetID		= pPacket->cWorldSetID;
				Request.nAccountDBID	= pPacket->nAccountDBID;
				Request.biCharacterDBID	= (pPacket->Code == DBDNWorldDef::TransactorCode::Master) ? pPacket->biMasterCharacterDBID : pPacket->biPupilCharacterDBID;
				Request.bClientSend		= true;

				OnRecvMessage( pPacket->cThreadID, MAINCMD_MASTERSYSTEM, QUERY_GET_SIMPLEINFO, reinterpret_cast<char*>(&Request) );
			}

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_GET_MASTERANDCLASSMATE:
		{
			TQGetListMyMasterAndClassmate* pPacket = reinterpret_cast<TQGetListMyMasterAndClassmate*>(pData);

			TAGetListMyMasterAndClassmate Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetListMyMasterAndClassmate( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_MASTERANDCLASSMATE:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.MasterAndClassmateInfoList)+(Packet.cCount*sizeof(Packet.MasterAndClassmateInfoList[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break;
		}
	case QUERY_GET_CLASSMATEINFO:
		{
			TQGetMyClassmate* pPacket = reinterpret_cast<TQGetMyClassmate*>(pData);

			TAGetMyClassmate Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetMyClassmate( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_CLASSMATEINFO:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			int iSize = sizeof(Packet)-sizeof(Packet.ClassmateInfo.wszMasterCharName)+(Packet.ClassmateInfo.cMasterCount*sizeof(Packet.ClassmateInfo.wszMasterCharName[0]));
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			break; 
		}
	case QUERY_GET_MASTERANDFAVORPOINT:
		{
			TQGetMasterAndFavorPoint* pPacket = reinterpret_cast<TQGetMasterAndFavorPoint*>(pData);

			TAGetMasterAndFavorPoint Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode				= ERROR_DB;
			Packet.nAccountDBID			= pPacket->nAccountDBID;
			Packet.cLevel				= pPacket->cLevel;
			Packet.cWorldSetID				= pPacket->cWorldSetID;
			Packet.biPupilCharacterDBID	= pPacket->biPupilCharacterDBID;
			_wcscpy( Packet.wszPupilCharName, _countof(Packet.wszPupilCharName), pPacket->wszPupilCharName, (int)wcslen(pPacket->wszPupilCharName) );

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetMasterAndFavorPoint( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_MASTERANDFAVORPOINT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			// 에러가 없고 스승이 있는 경우에만 패킷을 보낸다.
			if( Packet.nRetCode == ERROR_NONE && Packet.cCount > 0 )
			{
				int iSize = sizeof(Packet)-sizeof(Packet.MasterAndFavorPoint)+(Packet.cCount*sizeof(Packet.MasterAndFavorPoint[0]));
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			}
			break;
		}
	case QUERY_GRADUATE:
		{
			TQGraduate* pPacket = reinterpret_cast<TQGraduate*>(pData);

			TAGraduate Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode		= ERROR_DB;
			Packet.nAccountDBID	= pPacket->nAccountDBID;
			Packet.cWorldSetID		= pPacket->cWorldSetID;
			_wcscpy( Packet.wszPupilCharName, _countof(Packet.wszPupilCharName), pPacket->wszPupilCharName, (int)wcslen(pPacket->wszPupilCharName) );

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGraduate( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_GRADUATE:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
			{
				// 제자에게 SimpleInfo Refresh
				TQGetMasterPupilInfo Request;
				memset( &Request, 0, sizeof(Request) );

				Request.cThreadID		= pPacket->cThreadID;
				Request.cWorldSetID		= pPacket->cWorldSetID;
				Request.nAccountDBID	= pPacket->nAccountDBID;
				Request.biCharacterDBID	= pPacket->biCharacterDBID;
				Request.bClientSend		= true;
				Request.EventCode		= MasterSystem::EventType::Graduate;

				OnRecvMessage( pPacket->cThreadID, MAINCMD_MASTERSYSTEM, QUERY_GET_SIMPLEINFO, reinterpret_cast<char*>(&Request) );

				int iSize = sizeof(Packet)-sizeof(Packet.biMasterCharacterDBIDList)+Packet.cCount*sizeof(Packet.biMasterCharacterDBIDList[0]);
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), iSize );
			}
			break;
		}
	case QUERY_GET_COUNTINFO:
		{
			TQGetMasterSystemCountInfo* pPacket = reinterpret_cast<TQGetMasterSystemCountInfo*>(pData);

			TAGetMasterSystemCountInfo Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.biCharacterDBID	= pPacket->biCharacterDBID;
			Packet.bClientSend		= pPacket->bClientSend;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryGetMasterSystemCountInfo( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_GET_COUNTINFO:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_MOD_RESPECTPOINT:
		{
			TQModRespectPoint* pPacket = reinterpret_cast<TQModRespectPoint*>(pData);

			TAModRespectPoint Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode			= ERROR_DB;
			Packet.nAccountDBID		= pPacket->nAccountDBID;
			Packet.iRespectPoint	= pPacket->iRespectPoint;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryModRespectPoint( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_RESPECTPOINT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_MOD_FAVORPOINT:
		{
			TQModMasterFavorPoint* pPacket = reinterpret_cast<TQModMasterFavorPoint*>(pData);

			TAModMasterFavorPoint Packet;
			memset( &Packet, 0, sizeof(Packet) );

			Packet.nRetCode					= ERROR_DB;
			Packet.nAccountDBID				= pPacket->nAccountDBID;
			Packet.biMasterCharacterDBID	= pPacket->biMasterCharacterDBID;
			Packet.biPupilCharacterDBID		= pPacket->biPupilCharacterDBID;
			Packet.iFavorPoint				= pPacket->iFavorPoint;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				Packet.nRetCode = pWorldDB->QueryModMasterFavorPoint( pPacket, &Packet );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_FAVORPOINT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);

			if( Packet.nRetCode == ERROR_NONE )
				m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) );
			break;
		}
	case QUERY_MOD_GRADUATECOUNT:
		{
			TQModMasterSystemGraduateCount* pPacket = reinterpret_cast<TQModMasterSystemGraduateCount*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pPacket->cWorldSetID );
			if( pWorldDB )
				pWorldDB->QueryModGraduateCount( pPacket );
			else
				g_Log.Log( LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_GRADUATECOUNT:%d] WorldDB not found\r\n", pPacket->nAccountDBID, pPacket->cWorldSetID);
			break;
		}
	}
}
