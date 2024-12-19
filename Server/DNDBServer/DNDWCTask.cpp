#include "StdAfx.h"

#include "DNDWCTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Log.h"

#if defined( PRE_ADD_DWC )

CDNDWCTask::CDNDWCTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{
}

CDNDWCTask::~CDNDWCTask(void)
{
}

void CDNDWCTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_CREATE_DWC_TEAM:
		{
			TQCreateDWCTeam *pPacket = (TQCreateDWCTeam*)pData;
			TACreateDWCTeam Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			_wcscpy(Packet.wszTeamName, _countof(Packet.wszTeamName), pPacket->wszTeamName, (int)wcslen(pPacket->wszTeamName));

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{
				nRet = pWorldDB->QueryAddDWCTeam(pPacket, &Packet);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Packet, sizeof(Packet));

				switch (nRet)
				{
				case ERROR_NONE:
				case ERROR_DB_DWC_PERMIT_FAIL:
				case ERROR_DB_DWC_NAME_ALREADYEXIST:
				case ERROR_DB_DWC_ALREADY_HASTEAM:
				case ERROR_DB_DWC_USER_NOT_FOUND:
					break;
				default:
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_ADD_DWC_TEAM:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, nRet);
				}
			}
			else 
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_ADD_DWC_TEAM] pWorldDB not found\r\n", pPacket->nAccountDBID);
		}
		break;
	case QUERY_ADD_DWC_TEAMMEMBER:
		{
			TQAddDWCTeamMember *pPacket = (TQAddDWCTeamMember*)pData;
			TAAddDWCTeamMember Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			Packet.biCharacterDBID = pPacket->biCharacterDBID;
			Packet.nTeamID = pPacket->nTeamID;
			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{
				nRet = pWorldDB->QueryAddDWCTeamMember(pPacket, &Packet);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Packet, sizeof(Packet));

				switch(nRet)
				{
				case ERROR_NONE:
				case ERROR_DB_DWC_PERMIT_FAIL:
				case ERROR_DB_DWC_ALREADY_HASTEAM:
					break;
				default:
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_ADD_DWC_TEAMMEMBER:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, nRet);					
				}				
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_ADD_DWC_TEAMMEMBER] pWorldDB not found\r\n", pPacket->nAccountDBID);
		}
		break;
	case QUERY_DWC_INVITE_MEMBER:
		{
			TQDWCInviteMember *pPacket = (TQDWCInviteMember*)pData;
			TADWCInviteMember Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			Packet.biCharacterDBID = pPacket->biCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{				
				Packet.nRetCode = pWorldDB->QueryGetDWCTeamInfoByTeamID(pPacket->nTeamID, &Packet.nTeamID, &Packet.Info);
				if(Packet.nRetCode != ERROR_NONE )
				{
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_DWC_INVITE_MEMBER:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, Packet.nRetCode);
					break;
				}

				if( Packet.nTeamID <= 0 )	//팀 정보가 없음
				{
					m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) - sizeof(Packet.Info) - sizeof(Packet.MemberList));
					break;
				}

				if(pPacket->bNeedMembList) {
					std::vector<TDWCTeamMember> vTeamMember;
					vTeamMember.clear();

					Packet.nRetCode = pWorldDB->QueryGetDWCTeamMemberList(Packet.nTeamID, vTeamMember);
					if(Packet.nRetCode != ERROR_NONE || vTeamMember.size() == 0 )
					{
						g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_DWC_INVITE_MEMBER:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, Packet.nRetCode);
						break;
					}

					for(std::vector<TDWCTeamMember>::iterator itor = vTeamMember.begin(); itor != vTeamMember.end(); itor++)
						Packet.MemberList[Packet.cMemberCount++] = *itor;
				}

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet) - sizeof(Packet.MemberList) + (sizeof(Packet.MemberList[0])*Packet.cMemberCount) );
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_DWC_INVITE_MEMBER] pWorldDB not found\r\n", pPacket->nAccountDBID);
		}
		break;
	case QUERY_DEL_DWC_TEAMMEMBER:
		{
			TQDelDWCTeamMember *pPacket = (TQDelDWCTeamMember*)pData;
			TADelDWCTeamMember Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			Packet.nTeamID = pPacket->nTeamID;
			
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{
				nRet = pWorldDB->QueryDelDWCTeamMember(pPacket, &Packet);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Packet, sizeof(Packet));

				switch(nRet)
				{
				case ERROR_NONE:
				case ERROR_DB_DWC_USER_NOT_FOUND:
					break;
				default:
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_DEL_DWC_TEAMMEMBER:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, nRet);					
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_DEL_DWC_TEAMMEMBER] pWorldDB not found\r\n", pPacket->nAccountDBID);
		}
		break;
	case QUERY_GET_DWC_TEAMINFO:
		{
			TQGetDWCTeamInfo *pPacket = (TQGetDWCTeamInfo*)pData;
			TAGetDWCTeamInfo Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{				
				//Packet.nRetCode = pWorldDB->QueryGetDWCTeamInfo(pPacket, &Packet);
				Packet.nRetCode = pWorldDB->QueryGetDWCTeamInfo(pPacket->biCharacterDBID, &Packet.nTeamID, &Packet.Info);				
				if(Packet.nRetCode != ERROR_NONE )
				{
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_GET_DWC_TEAMINFO:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, Packet.nRetCode);
					break;
				}

				if( Packet.nTeamID <= 0 )	//팀에 가입 안되어 있는 상태
					m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet)-sizeof(TDWCTeam));
				else
					m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet));
				
				if(Packet.nTeamID >= 0 && pPacket->bNeedMembList) {
					TAGetDWCTeamMember DWCTeamMember;
					memset(&DWCTeamMember, 0, sizeof(DWCTeamMember));
					
					std::vector<TDWCTeamMember> vTeamMember;
					vTeamMember.clear();

					DWCTeamMember.nRetCode = pWorldDB->QueryGetDWCTeamMemberList(Packet.nTeamID, vTeamMember);
					DWCTeamMember.nAccountDBID = pPacket->nAccountDBID;
 					DWCTeamMember.nTeamID = Packet.nTeamID;
 					
 					for(std::vector<TDWCTeamMember>::iterator itor = vTeamMember.begin(); itor != vTeamMember.end(); itor++)
 						DWCTeamMember.MemberList[DWCTeamMember.cCount++] = *itor;

					int nSize = (sizeof(TAGetDWCTeamMember) - sizeof(DWCTeamMember.MemberList)) + (sizeof(DWCTeamMember.MemberList[0]) * DWCTeamMember.cCount);
 					m_pConnection->AddSendData(nMainCmd, QUERY_GET_DWC_TEAMMEMBER, reinterpret_cast<char*>(&DWCTeamMember), nSize);

					if(DWCTeamMember.nRetCode != ERROR_NONE )
					{
						g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_GET_DWC_TEAMMEMBER:%d] Query Error\r\n", pPacket->nAccountDBID, pPacket->biCharacterDBID, DWCTeamMember.nRetCode);
						break;
					}
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, pPacket->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GET_DWC_TEAMINFO] pWorldDB not found\r\n", pPacket->nAccountDBID);
		}
		break;
	case QUERY_ADD_DWC_RESULT:
		{
			TQAddPvPDWCResult *pPacket = (TQAddPvPDWCResult*)pData;
			TAAddPvPDWCResult Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nAccountDBID = pPacket->nAccountDBID;
			Packet.nRoomID = pPacket->nRoomID;
			Packet.nTeamID = pPacket->nTeamID;
			Packet.DWCScore.nDWCPoint = pPacket->nDWCGradePoint;
			Packet.DWCScore.nHiddenDWCPoint = pPacket->nHiddenDWCGradePoint;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{				
				Packet.nRetCode = pWorldDB->QueryAddDWCResult(pPacket, &Packet);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Packet, sizeof(Packet));

				if(Packet.nRetCode != ERROR_NONE )
				{
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[RoomID:%n TeamID:%u] [QUERY_ADD_DWC_RESULT:%d] Query Error\r\n", pPacket->nRoomID, pPacket->nTeamID, nRet);
					break;
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[RoomID:%n TeamID:%u] [QUERY_ADD_DWC_RESULT] pWorldDB not found\r\n", pPacket->nRoomID, pPacket->nTeamID);
		}
		break;
	case QUERY_GET_DWC_SCORELIST:
		{
			TQGetListDWCScore *pPacket = (TQGetListDWCScore*)pData;
			TAGetListDWCScore Packet;
			memset(&Packet, 0, sizeof(Packet));

			Packet.nRetCode = ERROR_DB;
			Packet.nRoomID = pPacket->nRoomID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
			{				
				Packet.nRetCode = pWorldDB->QueryGetDWCTeamInfo(pPacket->biATeamCharacterDBID, &Packet.nATeamID, &Packet.ATeamInfo);
				if(Packet.nRetCode != ERROR_NONE )
				{
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[RoomID:%n CharacterDBID:%lld] [QUERY_GET_DWC_SCORELIST:%d] Query Error\r\n", pPacket->nRoomID, pPacket->biATeamCharacterDBID, Packet.nRetCode);
					break;
				}

				Packet.nRetCode = pWorldDB->QueryGetDWCTeamInfo(pPacket->biBTeamCharacterDBID, &Packet.nBTeamID, &Packet.BTeamInfo);
				if(Packet.nRetCode != ERROR_NONE )
				{
					g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[RoomID:%n CharacterDBID:%lld] [QUERY_GET_DWC_SCORELIST:%d] Query Error\r\n", pPacket->nRoomID, pPacket->biATeamCharacterDBID, Packet.nRetCode);
					break;
				}
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Packet), sizeof(Packet));
			}
			else
				g_Log.Log(LogType::_ERROR, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[RoomID:%n ACharacterDBID:%lld, BCharacterDBID:%lld] [QUERY_GET_DWC_SCORELIST] pWorldDB not found\r\n", pPacket->nRoomID, pPacket->biATeamCharacterDBID, pPacket->biATeamCharacterDBID);
		}
		break;
	case QUERY_GET_DWC_RANKLIST:
		{
			TQGetDWCRankPage * pPacket = (TQGetDWCRankPage*)pData;
			TAGetDWCRankPage packet;
			memset(&packet, 0, sizeof(TAGetDWCRankPage));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
				nRet = pWorldDB->QueryGetDWCRankList(pPacket, &packet);

			packet.nRetCode = nRet;
			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(packet) - sizeof(packet.RankList) + (sizeof(TDWCRankData) * packet.nPageSize));
		}
		break;
	case QUERY_GET_DWC_FINDRANK:
		{
			TQGetDWCFindRank* pPacket = (TQGetDWCFindRank*)pData;
			TAGetDWCFindRank packet;
			memset(&packet, 0, sizeof(TQGetDWCFindRank));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if (pWorldDB)
				nRet = pWorldDB->QueryGetDWCFindRank(pPacket, &packet);

			packet.nRetCode = nRet;
			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(packet));
		}
		break;
	case QUERY_GET_DWC_CHANNELINFO:
		{
			TQGetDWCChannelInfo * pPacket = (TQGetDWCChannelInfo*)pData;

			TAGetDWCChannelInfo packet;
			memset(&packet, 0, sizeof(TAGetDWCChannelInfo));

			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if(pWorldDB)
				packet.nRetCode = pWorldDB->QueryGetDWCChannelInfo(&packet.ChannelInfo);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(packet));
		}
		break;
	}	
}

#endif // #if defined( PRE_ADD_DWC )