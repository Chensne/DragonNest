#include "StdAfx.h"
#include "DNEtcTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "TimeSet.h"
#include "Log.h"
#if defined( PRE_FIX_67546 )
#include "DNIocpManager.h"
#endif
extern TDBConfig g_Config;

CDNEtcTask::CDNEtcTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNEtcTask::~CDNEtcTask(void)
{
}

void CDNEtcTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	switch (nSubCmd)
	{
	case QUERY_UPDATEQUERYQUEUE:
		{
			// 주의!!! - 실제 DB 작업 수행은 본 메서드를 재귀호출 하여 이루어지며 재귀호출 시 본 패킷 (MAINCMD_ETC / QUERY_UPDATEQUERYQUEUE) 은 절대 포함되어선 않됨 !!!

			TDBQueryQueue* pEtc = reinterpret_cast<TDBQueryQueue*>(pData);
			DN_ASSERT(NULL != pEtc,			"Invalid!");
			DN_ASSERT(pEtc->CheckIntegrity(),	"Invalid!");

			if (DBQUERYAUTOCHKSUM != pEtc->m_nCheckSum) {		// 20091022 패킷 처리 시 무결성 체크를 위해 임시 추가 ??? (b4nfter)
				DN_BREAK;
			}

			for (int nCurPtr = 0 ; pEtc->m_nSize > nCurPtr ; ) {
				TDBQueryData* pDBQueryData = pEtc->At(nCurPtr);
				DN_ASSERT(NULL != pDBQueryData,	"Check!");

				if (MAINCMD_ETC == pDBQueryData->cMainCmd && QUERY_UPDATEQUERYQUEUE == pDBQueryData->cSubCmd) {
					DN_ASSERT(0,	"Invalid!");	// 해당 메인명령은 본 DB 누적작업 등록용 이므로 사용할 이유가 없으며 사용 시 무한재귀에 빠질 수도 있음 ?
				}

				m_pConnection->DBMessageProcess(reinterpret_cast<char*>(pDBQueryData), nThreadID);		// P.S.> 이 부분의 함수 재귀호출 처리를 위해 TDBQueryData 는 DNTPacket 와 동일한 구조를 가지도록 의도적으로 설계되었음 (차후 변경 필요 시 이와같은 설계 의도에 영향을 미치지 않도록 주의할 것 !!!)

				nCurPtr += (sizeof(DNTPacketHeader) + pDBQueryData->iLen);
			}
		}
		break;

	case QUERY_EVENTLIST:
		{
			TQEventList * pEtc = (TQEventList*)pData;
			TAEventList Etc;
			memset(&Etc, 0, sizeof(Etc));

			Etc.cWorldSetID = pEtc->cWorldSetID;
			Etc.nRetCode = ERROR_DB;
#if defined(PRE_ADD_WORLD_EVENT)
			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB)
			{				
				Etc.nRetCode = pMembershipDB->QueryEventList(pEtc, &Etc);
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_EVENTLIST:%d] pMembershipDB Error\r\n", pEtc->cWorldSetID);
#else //#if defined(PRE_ADD_WORLD_EVENT)
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB)
			{
				Etc.nRetCode = pWorldDB->QueryEventList(pEtc, &Etc);
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_EVENTLIST:%d] pWorldDB Error\r\n", pEtc->cWorldSetID);
#endif // #if defined(PRE_ADD_WORLD_EVENT)

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Etc, sizeof(Etc) - sizeof(Etc.EventList) + (Etc.cCount * sizeof(TEventInfo)));
		}
		break;

#ifdef PRE_MOD_RESTRICT_IDENTITY_IP
	case QUERY_GET_SIMPLECONFIG:
		{
			TQSimpleConfig * pPacket = (TQSimpleConfig*)pData;
			TASimpleConfig packet;
			memset(&packet, 0, sizeof(TASimpleConfig));

			packet.nRetCode = ERROR_DB;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB)
			{
				pMembershipDB->QueryGetSimpleConfigValue(&packet);
			}
			else
				g_Log.Log(LogType::_RESTRICTIP, pPacket->cWorldSetID, pPacket->nAccountDBID, 0, 0, L"[QUERY_GET_SIMPLECONFIG] pMembershipDB Error\r\n");

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet) - sizeof(packet.Configs) + (packet.cCount * sizeof(SimpleConfig::ConfigData)));
		}
		break;
#endif		//#ifdef PRE_MOD_RESTRICT_IDENTITY_IP

#ifdef PRE_ADD_SEETMAINTENANCEFLAG
	case QUERY_MAINTENANCEFLAG:
		{
			TQMaintenanceInfo * pPacket = reinterpret_cast<TQMaintenanceInfo*>(pData);

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB)
			{
				pMembershipDB->QuerySetMaintenanceFlag(pPacket->nFlag);
			}
		}
		break;
#endif		//#ifdef PRE_ADD_SEETMAINTENANCEFLAG

	case QUERY_GET_DBSID:
		{
			TAGetDBSID packet;
			memset(&packet, 0, sizeof(TAGetDBSID));
			packet.nServerID = g_Config.nManagedID;

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_GETGAMEOPTION:
		{
			TQGetGameOption * pOption = (TQGetGameOption*)pData;

			TAGetGameOption packet;
			memset(&packet, 0, sizeof(packet));

			packet.nAccountDBID = pOption->nAccountDBID;
			packet.biCharacterDBID = pOption->biCharacterDBID;
			packet.nRetCode = ERROR_DB;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB){
				packet.nRetCode = pMembershipDB->QueryGetGameOption(pOption->nAccountDBID, packet.Option);
			}
			else
				g_Log.Log(LogType::_ERROR, pOption->cWorldSetID, pOption->nAccountDBID, pOption->biCharacterDBID, 0, L"[QUERY_GETGAMEOPTION:%d] pMembershipDB Error\r\n", pOption->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet));
		}
		break;

	case QUERY_SETGAMEOPTION:
		{
			TQSetGameOption * pOption = (TQSetGameOption*)pData;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB){
				pMembershipDB->QueryModGameOption(pOption->nAccountDBID, pOption->Option);
			}
			else
				g_Log.Log(LogType::_ERROR, pOption->cWorldSetID, pOption->nAccountDBID, pOption->biCharacterDBID, 0, L"[QUERY_GETGAMEOPTION:%d] pMembershipDB Error\r\n", pOption->cWorldSetID);
		}
		break;

	case QUERY_RESTRAINT:
		{
			TQRestraint * pRestraint = (TQRestraint*)pData;

			TARestraint packet;
			memset(&packet, 0, sizeof(TARestraint));

			packet.nAccountDBID = pRestraint->nAccountDBID;
			packet.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pRestraint->cWorldSetID);
			if (pWorldDB){

			}
			else
				g_Log.Log(LogType::_ERROR, pRestraint->cWorldSetID, pRestraint->nAccountDBID, 0, 0, L"[QUERY_RESTRAINT:%d] pWorldDB Error\r\n", pRestraint->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&packet, sizeof(packet) - sizeof(packet.restraint) + (packet.cCount * sizeof(TRestraint)));
		}
		break;

	case QUERY_SETRESTRAINT:
		{
			TQSetRestraint * pAddRestraint = (TQSetRestraint*)pData;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
			if (pMembershipDB)
			{
				TIMESTAMP_STRUCT _tStartTime, _tEndTime;

				// 시작시각
				CTimeSet TimeSet;
				memcpy( &_tStartTime, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));
				// 끝시간
				TimeSet.AddSecond( ONEDAYSEC * 30 );
				memcpy( &_tEndTime, &TimeSet.GetDbTimeStamp(), sizeof(TIMESTAMP_STRUCT ));

#ifdef PRE_ADD_ABUSE_ACCOUNT_RESTRAINT
				int nRetCode = pMembershipDB->QueryAddRestraint((DBDNWorldDef::RestraintTargetCode::eCode)pAddRestraint->cTargetCode, pAddRestraint->nAccountDBID, \
					pAddRestraint->biCharacterDBID, pAddRestraint->nDolisReasonCode, (DBDNWorldDef::RestraintTypeCode::eCode)pAddRestraint->cRestraintCode, pAddRestraint->wszRestraintreasonForDolis, pAddRestraint->wszRestraintreason, _tStartTime, _tEndTime);
#else		//#ifdef PRE_ADD_ABUSE_ACCOUNT_RESTRAINT
				//ReasonID 110번은 상수로 박혀 있으며, 이쪽 번호 변경시 황시연씨에게 문의 바람.
				int nRetCode = pMembershipDB->QueryAddRestraint((DBDNWorldDef::RestraintTargetCode::eCode)pAddRestraint->cTargetCode, pAddRestraint->nAccountDBID, \
					pAddRestraint->biCharacterDBID, 110, (DBDNWorldDef::RestraintTypeCode::eCode)pAddRestraint->cRestraintCode, pAddRestraint->wszRestraintreasonForDolis, pAddRestraint->wszRestraintreason, _tStartTime, _tEndTime);
#endif		//#ifdef PRE_ADD_ABUSE_ACCOUNT_RESTRAINT

				if (nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pAddRestraint->cWorldSetID, pAddRestraint->nAccountDBID, 0, 0, L"[QUERY_SETRESTRAINT:%d] Error[Code:%d]\r\n", pAddRestraint->cWorldSetID, nRetCode);
			}
			else
				g_Log.Log(LogType::_ERROR, pAddRestraint->cWorldSetID, pAddRestraint->nAccountDBID, 0, 0, L"[QUERY_SETRESTRAINT:%d] pMembershipDB Error\r\n", pAddRestraint->cWorldSetID);
		}
		break;

	case QUERY_GET_KEYSETTING_OPTION:
		{
			TQGetKeySettingOption* pEtc = reinterpret_cast<TQGetKeySettingOption*>(pData);

			TAGetKeySettingOption Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
			if (pMembershipDB)
				Etc.nRetCode = pMembershipDB->QueryGetKeySettingOption( pEtc->nAccountDBID, &Etc.sKeySetting );

			if( Etc.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_GET_KEYSETTING_OPTION:%d] (Ret:%d)\r\n", pEtc->cWorldSetID, Etc.nRetCode);

			break;
		}

	case QUERY_MOD_KEYSETTING_OPTION:
		{
			TQModKeySettingOption* pEtc = reinterpret_cast<TQModKeySettingOption*>(pData);

			TAModKeySettingOption Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
			if (pMembershipDB)
				Etc.nRetCode = pMembershipDB->QueryModKeySettingOption(pEtc, &Etc);

			if( Etc.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_MOD_KEYSETTING_OPTION:%d] (Ret:%d)\r\n", pEtc->cWorldSetID, Etc.nRetCode);

			break;
		}

	case QUERY_GET_PADSETTING_OPTION:
		{
			TQGetPadSettingOption* pEtc = reinterpret_cast<TQGetPadSettingOption*>(pData);

			TAGetPadSettingOption Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode	= ERROR_DB;
			Etc.nAccountDBID	= pEtc->nAccountDBID;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
			if (pMembershipDB)
				Etc.nRetCode = pMembershipDB->QueryGetPadSettingOption( pEtc->nAccountDBID, &Etc.sPadSetting );

			if( Etc.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_GET_PADSETTING_OPTION:%d] (Ret:%d)\r\n", pEtc->cWorldSetID, Etc.nRetCode);

			break;
		}

	case QUERY_MOD_PADSETTING_OPTION:
		{
			TQModPadSettingOption* pEtc = reinterpret_cast<TQModPadSettingOption*>(pData);

			TAModPadSettingOption Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
			if (pMembershipDB)
				Etc.nRetCode = pMembershipDB->QueryModPadSettingOption(pEtc, &Etc);

			if( Etc.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_MOD_PADSETTING_OPTION:%d] (Ret:%d)\r\n", pEtc->cWorldSetID, Etc.nRetCode);

			break;
		}

	case QUERY_SET_PROFILE:
		{
			TQSetProfile *pEtc = (TQSetProfile*)pData;

			TASetProfile Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			// 서버에서 전송한 데이터를 그대로 반환 - 성공하면 서버에서 캐릭터 메모리로 적용한다.
			Etc.sProfile.cGender = pEtc->sProfile.cGender;
			_wcscpy(Etc.sProfile.wszGreeting, _countof(Etc.sProfile.wszGreeting), pEtc->sProfile.wszGreeting, (int)wcslen(pEtc->sProfile.wszGreeting));
			Etc.sProfile.bOpenPublic = pEtc->sProfile.bOpenPublic;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB)
				Etc.nRetCode = pWorldDB->QuerySetProfile(pEtc->biCharacterDBID, pEtc->sProfile);

			if( Etc.nRetCode != ERROR_NONE )
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SET_PROFILE:%d] (Ret:%d)\r\n", pEtc->biCharacterDBID, pEtc->cWorldSetID, Etc.nRetCode);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc) );
		}
		break;

	case QUERY_GET_PROFILE:
		{
			TQGetProfile *pEtc = (TQGetProfile*)pData;

			TAGetProfile Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode		= ERROR_DB;
			Etc.nAccountDBID	= pEtc->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryGetProfile(pEtc->biCharacterDBID, Etc.sProfile);
				Etc.nRetCode = nRet;
				if( nRet == 103212 )	nRet = ERROR_NONE;	// 103212는 프로필이 설정되지 않은 경우로 DB에러는 아니다. - 쓸데없는 로그를 찍지 않기 위해~
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GET_PROFILE:%d] (Ret:%d)\r\n", pEtc->biCharacterDBID, pEtc->cWorldSetID, nRet);
				}
			}
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc) );
		}
		break;

	case QUERY_ADD_ABUSELOG:
		{	
			TQAddAbuseLog *pEtc = (TQAddAbuseLog*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryAddAbuseLog(pEtc);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADD_ABUSELOG:%d] (Ret:%d)\r\n", pEtc->biCharacterDBID, pEtc->cWorldSetID, nRet);
				}
			}
		}
		break;

	case QUERY_ADD_ABUSEMONITOR:
		{
			TQAddAbuseMonitor* pEtc = reinterpret_cast<TQAddAbuseMonitor*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if( pWorldDB )
				pWorldDB->QueryAddAbuseMonitor( pEtc );
			break;
		}

	case QUERY_DEL_ABUSEMONITOR:
		{
			TQDelAbuseMonitor* pEtc = reinterpret_cast<TQDelAbuseMonitor*>(pData);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if( pWorldDB )
				pWorldDB->QueryDelAbuseMonitor( pEtc );
			break;
		}

	case QUERY_GET_ABUSEMONITOR:
		{
			TQGetAbuseMonitor* pEtc = reinterpret_cast<TQGetAbuseMonitor*>(pData);

			TAGetAbuseMonitor Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if( pWorldDB )
				Etc.nRetCode = pWorldDB->QueryGetAbuseMonitor( pEtc, &Etc );

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc) );
			break;
		}

	case QUERY_GET_WHOLE_ABUSEMONITOR:
		{
			TQGetWholeAbuseMonitor * pPacket = (TQGetWholeAbuseMonitor*)pData;

			TAGetWholeAbuseMonitor packet;
			memset(&packet, 0, sizeof(TAGetWholeAbuseMonitor));

			packet.nAccountDBID = pPacket->nAccountDBID;
			packet.nRetCode = ERROR_DB;
#if defined(PRE_ADD_MULTILANGUAGE)
			packet.cSelectedLang = pPacket->cSelectedLang;
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pPacket->cWorldSetID);
			if( pWorldDB )
				packet.nRetCode = pWorldDB->QueryGetWholeAbuseMonitor(pPacket, &packet);

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAGetWholeAbuseMonitor) - sizeof(packet.Abuse) + (sizeof(TAbuseMonitor) * packet.cCount) );
			break;
		}

	case QUERY_VALIDATE_SECONDAUTH:
		{
			TQValidateSecondAuth* pEtc = reinterpret_cast<TQValidateSecondAuth*>(pData);

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );

			TAValidateSecondAuth Etc;
			memset( &Etc, 0, sizeof(Etc) );

			Etc.nAccountDBID		= pEtc->nAccountDBID;
			Etc.nAuthCheckType	= pEtc->nAuthCheckType;
			Etc.nRetCode			= ERROR_DB;

			if( pMembershipDB )
				Etc.nRetCode = pMembershipDB->QueryValidataSecondAuthPassphrase( pEtc->nAccountDBID, pEtc->wszPW, Etc.cFailCount );

			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc) );
			break;
		}
	case QUERY_LOGOUT:
		{
			TQLogout *pUpdate = (TQLogout*)pData;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB){
#if defined (_KR)
				nRet = pMembershipDB->QueryLogout(pUpdate->nAccountDBID, pUpdate->nSessionID, pUpdate->szMachineID);
#else
				nRet = pMembershipDB->QueryLogout(pUpdate->nAccountDBID, pUpdate->nSessionID);
#endif
				if (nRet != ERROR_NONE){
					g_Log.Log( LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, 0, pUpdate->nSessionID, L"[ADBID:%u] [QUERY_LOGOUT:%d] (Ret:%d)\r\n", pUpdate->nAccountDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, 0, pUpdate->nSessionID, L"[ADBID:%u] [QUERY_LOGOUT:%d] pMembershipDB not found\r\n", pUpdate->nAccountDBID, pUpdate->cWorldSetID);
		}
		break;

	case QUERY_LOGINCHARACTER:
		{
			TQLoginCharacter* pEtc = reinterpret_cast<TQLoginCharacter*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pEtc->cWorldSetID );
			if (pWorldDB)
			{
				pWorldDB->QueryLoginCharacter( pEtc );
			}
			else
			{
				g_Log.Log (LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, pEtc->uiSessionID, L"[QUERY_LOGINCHARACTER:%d] pWorldDB Error\r\n", pEtc->cWorldSetID );
			}
			break;
		}
	case QUERY_LOGOUTCHARACTER:
		{
			TQLogoutCharacter* pEtc = reinterpret_cast<TQLogoutCharacter*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pEtc->cWorldSetID );
			if (pWorldDB)
			{
				pWorldDB->QueryLogoutCharacter( pEtc );
			}
			else
			{
				if( pEtc->cWorldSetID > 0 )
					g_Log.Log (LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, pEtc->uiSessionID, L"[QUERY_LOGOUTCHARACTER:%d] pWorldDB Error\r\n", pEtc->cWorldSetID );
			}
			break;
		}

	case QUERY_UPDATECONNECTINGTIME:
		{
			TQSaveConnectDurationTime *pEtc = (TQSaveConnectDurationTime*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB)
				pWorldDB->QuerySaveConnectDurationTime(pEtc);
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, 0, L"[QUERY_UPDATECONNECTINGTIME:%d] pWorldDB Error\r\n", pEtc->cWorldSetID);

		}
		break;

	case QUERY_MODLASTCONNECTDATE:
		{
			TQHeader *pEtc = (TQHeader*)pData;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB){
				nRet = pMembershipDB->QueryModLastConnectDate(pEtc->nAccountDBID);
				if (nRet != ERROR_NONE){
					g_Log.Log( LogType::_ERROR, 0, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_MODLASTCONNECTDATE:%d] (Ret:%d)\r\n", pEtc->nAccountDBID, pEtc->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pEtc->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else
				g_Log.Log(LogType::_ERROR, 0, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_MODLASTCONNECTDATE:%d] pMembershipDB not found\r\n", pEtc->nAccountDBID, pEtc->cWorldSetID);
		}
		break;
#if defined( PRE_FIX_67546 )
	case QUERY_ADD_CHANNELCOUNT:
		{
			TQAddThreadCount *pEtc = (TQAddThreadCount*)pData;
			if( g_pIocpManager )
			{
				TAAddThreadCount Etc;
				memset(&Etc, 0, sizeof(Etc));
				g_pIocpManager->AddConnectionCount( pEtc->cChoiceThreadID );
				int nCount[THREADMAX];
				g_pIocpManager->GetConnectionCount( nCount );
				memcpy( Etc.nConnectionCount, nCount, sizeof(Etc.nConnectionCount) );
				int nConnectionCount = g_pIocpManager->GetVillageGameConnectionCount();
				for(int i=0;i<nConnectionCount;i++)
				{
					CDNConnection* pCon = g_pIocpManager->GetVillageGameConnection(i);
					if( pCon && pCon->GetActive() )
					{
						pCon->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc) );						
					}
				}
			}
		}
		break;
#endif
#if defined( PRE_ADD_GAMEQUIT_REWARD )
	case QUERY_MOD_NEWBIE_REWARDFLAG:
		{
			TQModNewbieRewardFlag *pEtc = (TQModNewbieRewardFlag*)pData;
			TAHeader Etc;
			memset(&Etc, 0, sizeof(Etc));
			
			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if(pMembershipDB)
			{
				Etc.nRetCode = pMembershipDB->QueryModNewbieRewardFlag(pEtc->nAccountDBID, pEtc->bRewardFlag);
			}
			else
				g_Log.Log(LogType::_GAMEQUITREWARD, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_MOD_NEWBIE_REWARDFLAG:%d] pMembershipDB not found\r\n", pEtc->nAccountDBID, pEtc->bRewardFlag);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc));
		}
		break;
#endif	// #if defined( PRE_ADD_GAMEQUIT_REWARD )
#if defined(PRE_ADD_CP_RANK)
	case QUERY_ADD_STAGE_CLEAR_BEST :
		{
			TQAddStageClearBest* pEtc = (TQAddStageClearBest*)pData;
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if (pWorldDB)			
				nRet = pWorldDB->QueryAddStageClearBest(pEtc);			
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, pEtc->biCharacterDBID, 0, L"[QUERY_ADD_STAGE_CLEAR_BEST:%d] pWorldDB Error\r\n", pEtc->cWorldSetID);

			if( nRet != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[QUERY_ADD_STAGE_CLEAR_BEST] Error[Code:%d]\r\n", nRet);
		}
		break;
	case QUERY_GET_STAGE_CLEAR_BEST :
		{
			TQGetStageClearBest *pEtc = (TQGetStageClearBest*)pData;
			TAGetStageClearBest Etc;
			memset(&Etc, 0, sizeof(Etc));

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;			

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if(pWorldDB)
			{
				Etc.nRetCode = pWorldDB->QueryGetStageClearBest(pEtc, &Etc);
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GET_STAGE_CLEAR_BEST] pWorldDB not found\r\n", pEtc->nAccountDBID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc));
		}
		break;

	case QUERY_GET_STAGE_PERSONAL_BEST :
		{
			TQGetStageClearPersonalBest *pEtc = (TQGetStageClearPersonalBest*)pData;
			TAGetStageClearPersonalBest Etc;
			memset(&Etc, 0, sizeof(Etc));

			Etc.nRetCode = ERROR_DB;
			Etc.nAccountDBID = pEtc->nAccountDBID;				

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if(pWorldDB)
			{
				Etc.nRetCode = pWorldDB->QueryGetStageClearPersonalBest(pEtc, &Etc);
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GET_STAGE_PERSONAL_BEST] pWorldDB not found\r\n", pEtc->nAccountDBID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Etc), sizeof(Etc));
		}
		break;

	case QUERY_INIT_STAGE_BEST :
		{
			TQInitStageCP *pEtc = (TQInitStageCP*)pData;
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pEtc->cWorldSetID);
			if(pWorldDB)
			{
				pWorldDB->QueryInitStageClearBest(pEtc->cInitType);
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, pEtc->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_INIT_STAGE_BEST] pWorldDB not found\r\n", pEtc->nAccountDBID);
		}
		break;
#endif //#if defined(PRE_ADD_CP_RANK)

	case QUERY_MODCHARACTERSLOTCOUNT:
		{
			TQModCharacterSlotCount *pEtc = (TQModCharacterSlotCount*)pData;

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			if (pMembershipDB)
			{				
				char cMaxCount = 0;
				nRet = pMembershipDB->QueryModCharacterSlotCount(pEtc->biCharacterDBID, 1, cMaxCount);	// 34: 캐릭터 슬롯 개수제한
			}
			else
				g_Log.Log(LogType::_ERROR, pEtc->cWorldSetID, 0, 0, 0, L"[QUERY_MODCHARACTERSLOTCOUNT:%d] pMembershipDB Error\r\n", pEtc->cWorldSetID);
		}
		break;
	}
}