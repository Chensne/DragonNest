#include "StdAfx.h"
#include "DNStatusTask.h"
#include "DNConnection.h"
#include "DNSQLConnectionManager.h"
#include "DNSQLMembership.h"
#include "DNSQLWorld.h"
#include "Util.h"
#include "Log.h"

CDNStatusTask::CDNStatusTask(CDNConnection* pConnection)
: CDNMessageTask(pConnection)
{

}

CDNStatusTask::~CDNStatusTask(void)
{
}

void CDNStatusTask::OnRecvMessage(int nThreadID, int nMainCmd, int nSubCmd, char* pData)
{
	CDNSQLMembership *pMembershipDB = NULL;
	CDNSQLWorld *pWorldDB = NULL;
	int nRet = ERROR_DB;

	TQHeader *pHeader = (TQHeader*)pData;
	if( pHeader->nAccountDBID > 0)
	{
		// 여기를 해제하는것 보단 Save 패킷만 예외처리해서 다른걸 막는게 더 좋을듯..
		if( nSubCmd != QUERY_LASTUPDATEUSERDATA && g_pSPErrorCheckManager->bIsError(pHeader->nAccountDBID) == true )
		{
			//g_Log.Log(LogType::_ERROR, pHeader->cWorldSetID, pHeader->nAccountDBID, 0, 0, L"[SP_ERRORCHECK] Main:%d, Sub:%d\r\n", nMainCmd, nSubCmd);
			return;
		}
	}

	switch (nSubCmd)
	{
	case QUERY_SELECTCHARACTER:
		{
			TQSelectCharacter *pSelect = (TQSelectCharacter*)pData;

			TASelectCharacter Select;
			memset(&Select, 0, sizeof(TASelectCharacter));

			Select.nAccountDBID = pSelect->nAccountDBID;
			Select.biCharacterDBID = pSelect->biCharacterDBID;
#if !defined( PRE_PARTY_DB )
			Select.PartyID = pSelect->PartyID;
#endif
			Select.nRetCode = ERROR_DB;

			for( UINT i=0 ; i<DNNotifier::RegisterCount::Total ; ++i )
				Select.UserData.Status.NotifierData[i].Clear();

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pSelect->cWorldSetID);
			if (pWorldDB){
				// EffectItem
				std::vector<TEffectItemInfo> vEffectInfo;
				nRet = pWorldDB->QueryGetListEffectItem( pSelect->biCharacterDBID, vEffectInfo );
				if( nRet == ERROR_NONE )
				{
					for( UINT i=0 ; i<vEffectInfo.size() ; ++i )
					{
						TAEffectItem TxPacket;
						TxPacket.nRetCode = ERROR_NONE;
						TxPacket.nAccountDBID = pSelect->nAccountDBID;
						TxPacket.iOffset = i;
						TxPacket.ItemInfo = vEffectInfo[i];

						m_pConnection->AddSendData( nMainCmd, QUERY_GETLISTEFFECTITEM, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket) );
					}

#if defined(PRE_ADD_SERVER_WAREHOUSE)
					Select.nRetCode = pWorldDB->QuerySelectCharacter(pSelect->cWorldSetID, pSelect->biCharacterDBID, pSelect->nLastServerType, &Select, pSelect->nAccountDBID);
#else
					Select.nRetCode = pWorldDB->QuerySelectCharacter(pSelect->cWorldSetID, pSelect->biCharacterDBID, pSelect->nLastServerType, &Select, 0);
#endif
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pSelect->cWorldSetID, pSelect->nAccountDBID, pSelect->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_SELECTCHARACTER:%d] pWorldDB not found\r\n", pSelect->nAccountDBID, pSelect->biCharacterDBID, pSelect->cWorldSetID);
			}

			if (Select.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pSelect->cWorldSetID, pSelect->nAccountDBID, pSelect->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_SELECTCHARACTER:%d] Result:%d\r\n", pSelect->nAccountDBID, pSelect->biCharacterDBID, pSelect->cWorldSetID, Select.nRetCode);
			else
			{
				pMembershipDB = g_SQLConnectionManager.FindMembershipDB( nThreadID );
				if (pMembershipDB )
				{
					// M/W 에서는 ResetDate 사용할 일이 엄당~
					__time64_t tResetdate = -1;
					if( pMembershipDB->QueryGetSecondAuthStatus( pSelect->nAccountDBID, Select.bIsSetSecondAuthPW, Select.bIsSetSecondAuthLock, tResetdate ) != ERROR_NONE )
						Select.nRetCode = ERROR_DB;
					if( pMembershipDB->QueryGetKeySettingOption( pSelect->nAccountDBID, &Select.KeySetting ) != ERROR_NONE )
						Select.nRetCode = ERROR_DB;
					if( pMembershipDB->QueryGetPadSettingOption( pSelect->nAccountDBID, &Select.PadSetting ) != ERROR_NONE )
						Select.nRetCode = ERROR_DB;

#if defined(PRE_ADD_VIP)
					pMembershipDB->QueryGetVIPPoint(pSelect->biCharacterDBID, Select.nVIPTotalPoint, Select.tVIPEndDate, Select.bAutoPay);
#endif	// #if defined(PRE_ADD_VIP)
#if defined(_CH)
					pMembershipDB->QueryCheckIntroducedAccount(pSelect->nAccountDBID, Select.bIntroducer);
#endif	// #if defined(_CH)

					int nRestraintCount = 0;					
					std::vector<TRestraintForAccountAndCharacter> VecAccountRestraint;

					for (int nQueryRestraint = 0; nQueryRestraint < 2; nQueryRestraint++)
					{
						if (nQueryRestraint == 0)
							pMembershipDB->QueryGetListRestraintForAccount(pSelect->nAccountDBID, VecAccountRestraint);
						else
							pMembershipDB->QueryGetListRestraintForCharacter(pSelect->biCharacterDBID, VecAccountRestraint);

						if (VecAccountRestraint.empty() == false)
						{
							for (std::vector<TRestraintForAccountAndCharacter>::iterator RestraintItor = VecAccountRestraint.begin(); RestraintItor != VecAccountRestraint.end(); RestraintItor++)
							{
								Select.RestraintData.Restraint[nRestraintCount].nRestraintType = (*RestraintItor).cLevelCode;
								Select.RestraintData.Restraint[nRestraintCount].nRestraintKey = (*RestraintItor).iReasonID;
								_wcscpy(Select.RestraintData.Restraint[nRestraintCount].wszRestraintReason, RESTRAINTREASONMAX, \
									(*RestraintItor).wszRestraintReason, RESTRAINTREASONMAX);
								Select.RestraintData.Restraint[nRestraintCount]._tBegineTime = (*RestraintItor)._tStartDate;
								Select.RestraintData.Restraint[nRestraintCount]._tEndTime = (*RestraintItor)._tEndDate;
								nRestraintCount++;
								if (nRestraintCount >= RESTRAINTMAX)
									break;
							}
						}					
					}
				}
				else
					g_Log.Log(LogType::_ERROR, pSelect->cWorldSetID, pSelect->nAccountDBID, pSelect->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_SELECTCHARACTER:%d] AccountDB not found\r\n", pSelect->nAccountDBID, pSelect->biCharacterDBID, pSelect->cWorldSetID );
			}

			int nSelectLen = sizeof(TASelectCharacter) - sizeof(Select.ItemList) + (sizeof(DBPacket::TMaterialItemInfo) * Select.nItemListCount);
			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Select, nSelectLen);

			// 유저데이터가 세팅된 뒤에 캐쉬템이 보내져야한다
			if (pMembershipDB){
				TAGetListGiveFailItem FailItemList;
				memset(&FailItemList, 0, sizeof(TAGetListGiveFailItem));
				FailItemList.nAccountDBID = pSelect->nAccountDBID;

				nRet = pMembershipDB->QueryGetListGiveFailItem(pSelect->biCharacterDBID, &FailItemList);
				if ((nRet == ERROR_NONE) && (FailItemList.nCount > 0)){
					FailItemList.nRetCode = nRet;
					int nLen = sizeof(TAGetListGiveFailItem) - sizeof(FailItemList.CashFailItem) + (sizeof(DBPacket::TCashFailItem) * FailItemList.nCount);
					m_pConnection->AddSendData(MAINCMD_CASH, QUERY_GETLISTGIVEFAILITEM, (char*)&FailItemList, nLen);
				}
			}
			
			if (pWorldDB)
			{
				TAOwnChracterLevel LevelList;
				memset(&LevelList, 0, sizeof(TAOwnChracterLevel));

				LevelList.nAccountDBID = pSelect->nAccountDBID;
				LevelList.nRetCode = ERROR_DB;

				std::map<int, int> MapWorldUserCount;
				if (pMembershipDB)
					pMembershipDB->QueryGetCharacterCount(pSelect->wszAccountName, MapWorldUserCount);

				if (!MapWorldUserCount.empty()){
					nRet = ERROR_DB;

					std::vector<TChracterLevel> VecLevelList;
					VecLevelList.clear();
					for (std::map<int, int>::iterator iter = MapWorldUserCount.begin(); iter != MapWorldUserCount.end(); iter++){
						LevelList.nRetCode = pWorldDB->QueryGetCharacterLevelList(pSelect->nAccountDBID, iter->first, VecLevelList);
					}

					if (!VecLevelList.empty()){
						LevelList.cCount = (int)VecLevelList.size();
						if (LevelList.cCount > CHARCOUNTMAX)
							LevelList.cCount = CHARCOUNTMAX;

						int nCount = 0;
						for (int i = 0; i < LevelList.cCount; i++){
							LevelList.Level[nCount] = VecLevelList[i];
							nCount++;
						}
					}
				}

				if ((LevelList.nRetCode == ERROR_NONE) && (LevelList.cCount > 0)){
					int nLen = sizeof(TAOwnChracterLevel) - sizeof(LevelList.Level) + (sizeof(TChracterLevel) * LevelList.cCount);
					m_pConnection->AddSendData(MAINCMD_STATUS, QUERY_OWNCHRACTERLEVEL, (char*)&LevelList, nLen);
				}

				TAMissingItemList Missing;
				memset(&Missing, 0, sizeof(TAMissingItemList));

				Missing.nAccountDBID = pSelect->nAccountDBID;
				Missing.nRetCode = pWorldDB->QueryGetListMissingItem(pSelect->biCharacterDBID, Missing.cCount, Missing.MissingList);
				if ((Missing.nRetCode == ERROR_NONE) && (Missing.cCount > 0))
					m_pConnection->AddSendData(MAINCMD_ITEM, QUERY_MISSINGITEMLIST, (char*)&Missing, sizeof(TAMissingItemList));

#if defined(PRE_ADD_EQUIPLOCK)
				// 장비정보 들어간 이후에 장비 잠금 정보가 들어가도록  처리	
				TAGetListLockedItems LockedItemList;
				memset(&LockedItemList, 0, sizeof(LockedItemList));

				LockedItemList.nAccountDBID = pSelect->nAccountDBID;
				LockedItemList.nRetCode = pWorldDB->QueryGetListLockedItems(pSelect->biCharacterDBID, &LockedItemList);
				if(ERROR_NONE != LockedItemList.nRetCode)
					g_Log.Log(LogType::_ERROR, pSelect->cWorldSetID, pSelect->nAccountDBID, pSelect->biCharacterDBID, 0, L"[QUERY_GET_LIST_LOCKEDITEMS] query error (ret:%d)\r\n", LockedItemList.nRetCode);

				m_pConnection->AddSendData(MAINCMD_ITEM, QUERY_GET_LIST_LOCKEDITEMS, reinterpret_cast<char*>(&LockedItemList), sizeof(TAGetListLockedItems) - sizeof(TDBLockItemInfo) * (EQUIPMAX+CASHEQUIPMAX - LockedItemList.nCount));
#endif	// #if defined(PRE_ADD_EQUIPLOCK)
			}
		}
		break;

	case QUERY_UPDATEUSERDATA:
	case QUERY_CHANGESERVERUSERDATA:
	case QUERY_LASTUPDATEUSERDATA:
		{
			TQUpdateCharacter *pUpdate = (TQUpdateCharacter*)pData;

			TAUpdateCharacter Update;
			memset(&Update, 0, sizeof(TAUpdateCharacter));

			Update.nAccountDBID = pUpdate->nAccountDBID;
			Update.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				Update.nRetCode = pWorldDB->QueryMoveIntoNewServer(pUpdate);
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_UPDATE:%d] pWorldDB not found (Cmd:%d)\r\n", pUpdate->nAccountDBID, pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nSubCmd);
			}

			if (Update.nRetCode != ERROR_NONE)
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[ADBID:%u CDBID:%lld] [QUERY_UPDATE:%d] DB Error (Cmd:%d) Result:%d\r\n", pUpdate->nAccountDBID, pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nSubCmd, Update.nRetCode);

			switch (nSubCmd)
			{
			case QUERY_LASTUPDATEUSERDATA:
			case QUERY_CHANGESERVERUSERDATA:	// 서버 바꿀때만 결과값을 알려주면 된다.
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Update, sizeof(TAUpdateCharacter));
			}
			if( nSubCmd == QUERY_LASTUPDATEUSERDATA)
			{
				// 여기서 혹시나 존재하는넘은 지워줍니다.
				g_pSPErrorCheckManager->Del(pUpdate->nAccountDBID);
			}
		}
		break;

	case QUERY_CHANGESTAGEUSERDATA:		// 스테이지 나갈때마다 정보저장
		{
			TQChangeStageUserData *pUpdate = (TQChangeStageUserData*)pData;

			TAChangeStageUserData Update;
			memset(&Update, 0, sizeof(TAChangeStageUserData));
			Update.nAccountDBID = pUpdate->nAccountDBID;
			Update.nRetCode = ERROR_DB;
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				Update.nRetCode = pWorldDB->QueryMoveIntoNewZone(pUpdate, Update.cRebirthCoin, Update.cPCBangRebirthCoin);

				if (Update.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_CHANGESTAGEUSERDATA:%d] Result:%d\r\n", pUpdate->nAccountDBID, pUpdate->cWorldSetID, Update.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_CHANGESTAGEUSERDATA:%d] pWorldDB not found\r\n", pUpdate->nAccountDBID, pUpdate->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Update, sizeof(TAChangeStageUserData));
		}
		break;

	case QUERY_GETCHARACTERPARTIALYBYNAME:
		{
			TQGetCharacterPartialyByName *pStatus = (TQGetCharacterPartialyByName*)pData;

			TAGetCharacterPartialy Status;
			memset( &Status, 0, sizeof(Status) );
			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.cReqType = pStatus->cReqType;
			Status.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB){
				Status.nRetCode = pWorldDB->QueryGetCharacterPartialy1(0, pStatus->wszName, &Status);

				if (Status.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETCHARACTERPARTIALYBYNAME:%d] Result:%d\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, 0, 0, L"[ADBID:%u] [QUERY_GETCHARACTERPARTIALYBYNAME:%d] pWorldDB not found\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(TAGetCharacterPartialy));
		}
		break;

	case QUERY_GETCHARACTERPARTIALYBYDBID:
		{
			TQGetCharacterPartialyByDBID *pStatus = (TQGetCharacterPartialyByDBID*)pData;

			TAGetCharacterPartialy Status;
			memset( &Status, 0, sizeof(Status) );

			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.cReqType = pStatus->cReqType;
			Status.nRetCode = ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB){
				Status.nRetCode = pWorldDB->QueryGetCharacterPartialy1(pStatus->biCharacterDBID, NULL, &Status);
				if (Status.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETCHARACTERPARTIALYBYDBID:%d] Result:%d\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
			}
			else{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETCHARACTERPARTIALYBYDBID:%d] pWorldDB not found\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID);
			}

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(TAGetCharacterPartialy));
		}
		break;

	case QUERY_LEVEL:
		{
			TQLevel *pUpdate = (TQLevel*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModCharacterLevel(pUpdate->biCharacterDBID, pUpdate->cChangeCode, pUpdate->cLevel, pUpdate->nChannelID, pUpdate->nMapID);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_LEVEL:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
				else
				{
					TQGetMasterAndFavorPoint TxPacket;
					memset( &TxPacket, 0, sizeof(TxPacket) );

					TxPacket.cThreadID				= pUpdate->cThreadID;
					TxPacket.cWorldSetID				= pUpdate->cWorldSetID;
					TxPacket.nAccountDBID			= pUpdate->nAccountDBID;
					TxPacket.cLevel					= pUpdate->cLevel;
					TxPacket.biPupilCharacterDBID	= pUpdate->biCharacterDBID;
					_wcscpy( TxPacket.wszPupilCharName, _countof(TxPacket.wszPupilCharName), pUpdate->wszCharName, (int)wcslen(pUpdate->wszCharName) );

					CDNMessageTask* pTask = m_pConnection->GetMessageTask(MAINCMD_MASTERSYSTEM);
					if (pTask)
						pTask->OnRecvMessage(pUpdate->cThreadID, MAINCMD_MASTERSYSTEM, QUERY_GET_MASTERANDFAVORPOINT, reinterpret_cast<char*>(&TxPacket));
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_LEVEL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_EXP:
		{
			TQExp *pUpdate = (TQExp*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB)
			{
				nRet = pWorldDB->QueryModCharacterExp(pUpdate->biCharacterDBID, pUpdate->cChangeCode, pUpdate->nExp, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->biFKey);
				if (nRet != ERROR_NONE){
#if !defined( _FINAL_BUILD )
					// 변경 후 경험치가 변경 전 경험치보다 작습니다.
					if( nRet == 103289 )
						break;
#endif // #if !defined( _FINAL_BUILD )
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_LEVEL:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_LEVEL:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_COIN:
		{
			TQCoin *pCoin = (TQCoin*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pCoin->cWorldSetID);
			if (pWorldDB){
				INT64 biTotalCoin = 0;
				nRet = pWorldDB->QueryModCoin(pCoin->biCharacterDBID, pCoin->cCoinChangeCode, pCoin->biChangeKey, pCoin->nChangeCoin, pCoin->nChannelID, pCoin->nMapID, biTotalCoin);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pCoin->cWorldSetID, pCoin->nAccountDBID, pCoin->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_COIN:%d] Query Error Ret:%d\r\n", pCoin->biCharacterDBID, pCoin->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pCoin->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pCoin->cWorldSetID, pCoin->nAccountDBID, pCoin->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_COIN:%d] pWorldDB not found\r\n", pCoin->biCharacterDBID, pCoin->cWorldSetID);
			}
		}
		break;

	case QUERY_WAREHOUSECOIN:
		{
			TQWarehouseCoin *pUpdate = (TQWarehouseCoin*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				INT64 biCurrentCoin = 0, biCurrentWarehouseCoin = 0;
				nRet = pWorldDB->QueryModWarehouseCoin(pUpdate->biCharacterDBID, pUpdate->cCoinChangeCode, pUpdate->nChangeCoin, pUpdate->nChannelID, pUpdate->nMapID, biCurrentCoin, biCurrentWarehouseCoin);
				/*
				if (biCurrentCoin != pUpdate->nTotalCoin){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, 0, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_WAREHOUSECOIN:%d] (%d:%d)\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, biCurrentCoin, pUpdate->nTotalCoin);
				}
				*/
				if (biCurrentWarehouseCoin != pUpdate->nTotalWarehouseCoin){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_WAREHOUSECOIN:%d] (%d:%d)\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, biCurrentWarehouseCoin, pUpdate->nTotalWarehouseCoin);
				}
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_WAREHOUSECOIN:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_WAREHOUSECOIN:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_MAPINFO:
		{
			TQMapInfo *pUpdate = (TQMapInfo*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				TPosition Pos = { 0, };
				nRet = pWorldDB->QueryModCharacterStatus(pUpdate->biCharacterDBID, 120, 0, 0, 0, pUpdate->nMapIndex, pUpdate->nLastVillageMapIndex, pUpdate->nLastSubVillageMapIndex, pUpdate->cLastVillageGateNo, Pos, 0, 0, 0, false, false, 0);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MAPINFO:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MAPINFO:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_CASHREBIRTHCOIN:
		{
			TQCashRebirthCoin *pUpdate = (TQCashRebirthCoin*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUseCashRebirthCoin(pUpdate->biCharacterDBID, pUpdate->nChannelID, pUpdate->nMapID, pUpdate->wszIP );

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_REBIRTHCASHCOIN:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_REBIRTHCASHCOIN:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_ADDCASHREBIRTHCOIN:
		{
			TQAddCashRebirthCoin *pUpdate = (TQAddCashRebirthCoin*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryAddCashRebirthCoin(pUpdate->biCharacterDBID, pUpdate->nRebirthCount, pUpdate->nRebirthPrice, pUpdate->nRebirthCode, pUpdate->nRebirthKey);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDCASHREBIRTHCOIN:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ADDCASHREBIRTHCOIN:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_FATIGUE:
		{
			TQFatigue *pUpdate = (TQFatigue*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModFatigue(pUpdate);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_FATIGUE(%d):%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cFatigueTypeCode, pUpdate->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_FATIGUE(%d):%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cFatigueTypeCode, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_EVENTFATIGUE:
		{
			TQEventFatigue *pUpdate = (TQEventFatigue*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModEventFatigue(pUpdate);

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_EVENTFATIGUE:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_EVENTFATIGUE:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_JOB:
		{
			TQJob *pUpdate = (TQJob*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModJobCode(pUpdate->biCharacterDBID, pUpdate->cJob);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_JOB:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_JOB:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_GLYPHDELAYTIME:
		{
			TQGlyphAttributeTime *pUpdate = (TQGlyphAttributeTime*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsEquipmentAttribute(pUpdate->biCharacterDBID, 20, DBDNWorldDef::EquipmentAttributeCode::DelayTime, pUpdate->nTime);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GLYPHDELAYTIME:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GLYPHDELAYTIME:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}

		}
		break;

	case QUERY_GLYPHREMAINTIME:
		{
			TQGlyphAttributeTime *pUpdate = (TQGlyphAttributeTime*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsEquipmentAttribute(pUpdate->biCharacterDBID, 20, DBDNWorldDef::EquipmentAttributeCode::RemainTime, pUpdate->nTime);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GLYPHREMAINTIME:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GLYPHREMAINTIME:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}

		}
		break;

	case QUEST_NOTIFIER:
		{
			TQNotifier *pUpdate = (TQNotifier*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB)
			{
				nRet = pWorldDB->QueryModNotifier(pUpdate->biCharacterDBID, pUpdate->cNotifierSlot, pUpdate->cNotifierType, pUpdate->nID);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUEST_NOTIFIER:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUEST_NOTIFIER:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

#if !defined(PRE_DELETE_DUNGEONCLEAR)
	case QUERY_DUNGEONCLEAR:
		{
			TQDungeonClear *pUpdate = (TQDungeonClear*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsDungeonClear(pUpdate->biCharacterDBID, pUpdate->nMapIndex, pUpdate->cType);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DUNGEONCLEAR:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_DUNGEONCLEAR:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;
#endif	// #if !defined(PRE_DELETE_DUNGEONCLEAR)

	case QUERY_ETERNITYITEM:
		{
			TQEternityItem *pUpdate = (TQEternityItem*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsCharacterAbility(pUpdate->biCharacterDBID, pUpdate->cEternityCode, pUpdate->nEternityValue);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ETERNITYITEM:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_ETERNITYITEM:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_CHECKFIRSTVILLAGE:
		{
			TQCheckFirstVillage *pUpdate = (TQCheckFirstVillage*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryVisitFirstVillage(pUpdate->biCharacterDBID);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHECKFIRSTVILLAGE:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHECKFIRSTVILLAGE:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_COLOR:
		{
			TQColor *pUpdate = (TQColor*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				TPosition Pos = { 0, };

				switch (pUpdate->cItemType){
				case ITEMTYPE_HAIRDYE:
					nRet = pWorldDB->QueryModCharacterStatus(pUpdate->biCharacterDBID, 1, pUpdate->dwColor, 0, 0, 0, 0, 0, 0, Pos, 0, 0, 0, false, false, 0);
					break;

				case ITEMTYPE_EYEDYE:
					nRet = pWorldDB->QueryModCharacterStatus(pUpdate->biCharacterDBID, 2, 0, pUpdate->dwColor, 0, 0, 0, 0, 0, Pos, 0, 0, 0, false, false, 0);
					break;

				case ITEMTYPE_SKINDYE:
					nRet = pWorldDB->QueryModCharacterStatus(pUpdate->biCharacterDBID, 4, 0, 0, pUpdate->dwColor, 0, 0, 0, 0, Pos, 0, 0, 0, false, false, 0);
					break;
				}

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_COLOR:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);

					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_COLOR:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}

#ifdef PRE_FIX_63822
			TAColor packet;
			memset(&packet, 0, sizeof(TAColor));

			packet.nAccountDBID = pUpdate->nAccountDBID;
			packet.nRetCode = nRet;

			m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&packet), sizeof(TAColor));
#endif		//#ifdef PRE_FIX_63822
		}
		break;

	case QUERY_REBIRTHCOIN:
		{
			TQRebirthCoin *pUpdate = (TQRebirthCoin*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModRebirthCoin(pUpdate->biCharacterDBID, pUpdate->cRebirthCoin, pUpdate->cPCBangRebirthCoin);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_REBIRTHCOIN:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_REBIRTHCOIN:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_NESTCLEARCOUNT:
		{
			TQNestClearCount *pUpdate = (TQNestClearCount*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryUpsNestClear(pUpdate->biCharacterDBID, pUpdate->nMapIndex, pUpdate->cClearType);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_NESTCLEARCOUNT:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_NESTCLEARCOUNT:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_INITNESTCLEARCOUNT:
		{
			TQInitNestClearCount *pUpdate = (TQInitNestClearCount*)pData;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pUpdate->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryInitNestClearCount(pUpdate->biCharacterDBID);
				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_INITNESTCLEARCOUNT:%d] Query Error Ret:%d\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID, nRet);
					m_pConnection->QueryResultError(pUpdate->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pUpdate->cWorldSetID, pUpdate->nAccountDBID, pUpdate->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_INITNESTCLEARCOUNT:%d] pWorldDB not found\r\n", pUpdate->biCharacterDBID, pUpdate->cWorldSetID);
			}
		}
		break;

	case QUERY_CHANGECHARACTERNAME:		// 캐릭터명 변경
		{
			TQChangeCharacterName *pStatus = reinterpret_cast<TQChangeCharacterName *>(pData);

			pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);

			if (pMembershipDB && pWorldDB)
			{
				TAChangeCharacterName Status;				
				Status.nAccountDBID = pStatus->nAccountDBID;
				Status.cWorldSetID = pStatus->cWorldSetID;			
				Status.nCharacterDBID = pStatus->nCharacterDBID;
				Status.biItemSerial = pStatus->biItemSerial;
				_wcscpy(Status.wszOriginName, _countof(Status.wszOriginName), pStatus->wszOriginName, (int)wcslen(pStatus->wszOriginName));
				_wcscpy(Status.wszCharacterName, _countof(Status.wszCharacterName), pStatus->wszCharacterName, (int)wcslen(pStatus->wszCharacterName));

				nRet = pMembershipDB->QueryModCharacterName(pStatus, &Status);
				switch (nRet)
				{
				case ERROR_NONE:
					{
						nRet = pWorldDB->QueryModCharacterName(pStatus);
						switch (nRet)
						{
						case ERROR_NONE:
							break;

						default:
							g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->nCharacterDBID, 0, L"[QUERY_CHANGECHARACTERNAME(World):%d Result:%d] Query Error\r\n", pStatus->cWorldSetID, nRet);
							break;
						}
					}
					break;

				case 101116: // 이미 존재하는 캐릭터 이름입니다.
					break;

				default:
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->nCharacterDBID, 0, L"[QUERY_CHANGECHARACTERNAME(Member):%d Result:%d] Query Error\r\n", pStatus->cWorldSetID, nRet);
					break;
				}

				Status.nRetCode = nRet;
				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Status), sizeof(TAChangeCharacterName));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->nCharacterDBID, 0, L"[QUERY_CHANGECHARACTERNAME:%d] Member & Char DB not found\r\n", pStatus->cWorldSetID);
			}
		}
		break;

	case QUERY_GETLIST_ETCPOINT:
		{
			TQGetListEtcPoint *pStatus = reinterpret_cast<TQGetListEtcPoint *>(pData);

			TAGetListEtcPoint Status;
			memset( &Status, 0, sizeof(Status) );
			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.nCharacterDBID = pStatus->nCharacterDBID;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB)
			{
				Status.nRetCode = pWorldDB->QueryGetListEtcPoint(pStatus, &Status);
				if (Status.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->nCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETLISTPOINT:%d] Result:%d\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, reinterpret_cast<char*>(&Status), sizeof(TAGetListEtcPoint));
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->nCharacterDBID, 0, L"[ADBID:%u] [QUERY_GETLISTPOINT:%d] pWorldDB not found\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID);
			}
			
		}
		break;
	case QUERY_ADD_ETCPOINT:
		{
			TQAddEtcPoint* pStatus = reinterpret_cast<TQAddEtcPoint*>(pData);
			TAAddEtcPoint Status;
			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.nRetCode		= ERROR_DB;
			Status.cType		= pStatus->cType;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB)
			{
				Status.nRetCode = pWorldDB->QueryAddEtcPoint( pStatus, &Status );
				if (Status.nRetCode != ERROR_NONE)
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_ADD_ETCPOINT:%d] Result:%d\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
			}
			else
				g_Log.Log( LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%d] [QUERY_ADD_ETCPOINT:%d] WorldDB not found\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(Status));
		}	
		break;
	case QUERY_USE_ETCPOINT:
		{
			TQUseEtcPoint* pStatus = reinterpret_cast<TQUseEtcPoint*>(pData);
			TAUseEtcPoint Status;
			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.nRetCode		= ERROR_DB;
			Status.cType		= pStatus->cType;

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB)
			{
				Status.nRetCode = pWorldDB->QueryUseEtcPoint( pStatus, &Status );
				if (Status.nRetCode != ERROR_NONE)
				{
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%u] [QUERY_USE_ETCPOINT:%d] Result:%d\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
					m_pConnection->QueryResultError(pStatus->nAccountDBID, Status.nRetCode, nMainCmd, nSubCmd);
				}
			}
			else
				g_Log.Log( LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[ADBID:%d] [QUERY_USE_ETCPOINT:%d] WorldDB not found\r\n", pStatus->nAccountDBID, pStatus->cWorldSetID);

			m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(Status));
		}	
		break;

	case QUERY_MOD_TIMEEVENT_DATE:
		{
			TQModTimeEventDate *pStatus = reinterpret_cast<TQModTimeEventDate *>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB(nThreadID, pStatus->cWorldSetID);
			if (pWorldDB){
				nRet = pWorldDB->QueryModTimeEventDate( pStatus );

				if (nRet != ERROR_NONE){
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MOD_TIMEEVENT_DATE] Query Error Ret:%d\r\n", pStatus->biCharacterDBID, nRet);
					m_pConnection->QueryResultError(pStatus->nAccountDBID, nRet, nMainCmd, nSubCmd);	// 디비에서 뭔가 에러값을 뱉어냈으니 걍 끊어버린다
				}
			}
			else{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MOD_TIMEEVENT_DATE:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);
			}

			break;
		}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
		case QUERY_CHANGEJOBCODE:
		{
			TQChangeJobCode* pStatus = reinterpret_cast<TQChangeJobCode*>(pData);
			TAChangeJobCode Status;
			memset( &Status, 0, sizeof(Status) );

			Status.nAccountDBID = pStatus->nAccountDBID;
			Status.nRetCode		= ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );
			if( pWorldDB )
			{
				Status.nRetCode = pWorldDB->QueryChangeJobCode( pStatus->biCharacterDBID, pStatus->nChannelID, pStatus->nMapID, pStatus->nFirstJobIDBefore, pStatus->nFirstJobIDAfter, 
					pStatus->nSecondJobIDBefore, pStatus->nSecondJobIDAfter, 2, Status.wSkillPoint, Status.wTotalSkillPoint );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHANGEJOBCODE:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);
			}

			m_pConnection->AddSendData( nMainCmd, nSubCmd, (char*)&Status, sizeof(Status) );
		}
		break;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

		case QUERY_GETLIST_VARIABLERESET:
		{
			TQGetListVariableReset* pStatus = reinterpret_cast<TQGetListVariableReset*>(pData);
			TAGetListVariableReset Status;
			memset( &Status, 0, sizeof(Status) );

			Status.nAccountDBID	= pStatus->nAccountDBID;
			Status.nRetCode	= ERROR_DB;

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );
			if( pWorldDB )
			{
				Status.nRetCode = pWorldDB->QueryGetListVariableReset( pStatus, &Status );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_GETLIST_VARIABLERESET:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);
			}

			int iSize = sizeof(Status)-sizeof(Status.Data)+Status.cCount*sizeof(Status.Data[0]);
			m_pConnection->AddSendData( nMainCmd, nSubCmd, reinterpret_cast<char*>(&Status), iSize );
			break;
		}
		case QUERY_MOD_VARIABLERESET:
		{
			TQModVariableReset* pStatus = reinterpret_cast<TQModVariableReset*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );

			int nRetCode = ERROR_DB;

			if( pWorldDB )
			{
				nRetCode = pWorldDB->QueryModVariableReset( pStatus );
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_MOD_VARIABLERESET:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);
			}

			// 응답이 필요없는 패킷인데 에러인경우 Disconnect 처리가 필요한 경우 호출
			if( nRetCode != ERROR_NONE )
				m_pConnection->QueryResultError( pStatus->nAccountDBID, nRetCode, nMainCmd, nSubCmd );
			break;
		}

		case QUERY_CHANGE_SKILLPAGE:
		{
			TQChangeSkillPage* pStatus = reinterpret_cast<TQChangeSkillPage*>(pData);

			pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );

			int nRetCode = ERROR_DB;
			TPosition Pos = { 0, };

			if( pWorldDB )
			{				
				pStatus->cSkillPage++;	//디비는 1부터 시작이라서
				nRetCode = pWorldDB->QueryModCharacterStatus(pStatus->biCharacterDBID, 65536, 0, 0, 0, 0, 0, 0, 0, Pos, 0, 0, 0, false, false, pStatus->cSkillPage);
			}
			else
			{
				g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_CHANGE_SKILLPAGE:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);
			}

			// 응답이 필요없는 패킷인데 에러인경우 Disconnect 처리가 필요한 경우 호출
			if( nRetCode != ERROR_NONE )
				m_pConnection->QueryResultError( pStatus->nAccountDBID, nRetCode, nMainCmd, nSubCmd );

		}
			break;
#if defined(PRE_ADD_DOORS_PROJECT)
		case QUERY_SAVE_CHARACTER_ABILITY :
			{
				TQSaveCharacterAbility* pStatus = reinterpret_cast<TQSaveCharacterAbility*>(pData);
				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );

				int nRetCode = ERROR_DB;		

				if( pWorldDB )
					nRetCode = pWorldDB->QuerySaveCharacterAbility(pStatus);				
				else				
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%lld] [QUERY_SAVE_CHARACTER_ABILITY:%d] pWorldDB not found\r\n", pStatus->biCharacterDBID, pStatus->cWorldSetID);				
			}
			break;
#endif
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
		case QUERY_GET_TOTALSKILLLEVEL :
			{
				TQGetTotalLevelSkill* pStatus = reinterpret_cast<TQGetTotalLevelSkill*>(pData);
				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );

				int nRetCode = ERROR_DB;

				TAGetTotalLevelSkill Status;
				memset( &Status, 0, sizeof(Status) );

				Status.nAccountDBID	= pStatus->nAccountDBID;				

				if( pWorldDB )
					nRetCode = pWorldDB->QueryGetTotalSkillLevel(pStatus, &Status);
				else				
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QUERY_GET_TOTALSKILLLEVEL:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, nRetCode);
				if( nRetCode == ERROR_NONE )
				{
					nRetCode = pWorldDB->QueryGetTotalSkillList(pStatus, &Status);
				}

				m_pConnection->AddSendData( nMainCmd, nSubCmd, (char*)&Status, sizeof(Status) );
			}
			break;
		case QUERY_ADD_TOTALSKILLLEVEL :
			{
				TQAddTotalLevelSkill* pStatus = reinterpret_cast<TQAddTotalLevelSkill*>(pData);
				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );

				int nRetCode = ERROR_DB;				

				if( pWorldDB )
					nRetCode = pWorldDB->QueryAddTotalSkill(pStatus);
				else				
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QUERY_GET_TOTALSKILLLEVEL:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, nRetCode);				
			}
			break;
#endif

#ifdef PRE_ADD_PRESET_SKILLTREE
		case QUERY_GET_SKILLPRESET_LIST:
			{
				TQGetSKillPreSetList * pStatus = (TQGetSKillPreSetList*)pData;
				TAGetSKillPreSetList Status;
				memset(&Status, 0, sizeof(TAGetSKillPreSetList));

				Status.nAccountDBID = pStatus->nAccountDBID;
				Status.nRetCode = ERROR_DB;
				int nTotalCount = 0;

				for (int i = 0; i < SKILLPRESETMAX; i++)
					Status.SKillIndex[i].cIndex = i;

				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );
				if (pWorldDB)
				{
					if (pWorldDB->QueryGetSKillPresetIndexList(pStatus, &Status) != ERROR_NONE)
						g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QueryGetSKillPresetIndexList:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
					else
					{
						for (int i = 0; i < SKILLPRESETMAX; i++)
						{
							if (wcslen(Status.SKillIndex[i].wszSetName) <= 0) continue;
							int nRet = pWorldDB->QueryGetSKillPresetList(pStatus, Status.SKillIndex[i].cIndex, &Status.SKills[nTotalCount], Status.SKillIndex[i].cCount);
							if (nRet != ERROR_NONE)
							{
								g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QueryGetSKillPresetList:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
								Status.nRetCode = nRet;
								nTotalCount = 0;
								break;
							}

							nTotalCount += Status.SKillIndex[i].cCount;
							if (nTotalCount >= SKILLMAX * SKILLPRESETMAX)
							{
								_DANGER_POINT();
								break;
							}
						}
					}	
				}
				else
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QUERY_GET_SKILLPRESET_LIST:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);

				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(Status) - sizeof(Status.SKills) + (sizeof(TSkillSetPartialData) * nTotalCount));
			}
			break;

		case QUERY_ADD_SKILLPRESET:
			{
				TQAddSkillPreSet * pStatus = (TQAddSkillPreSet*)pData;

				TAAddSkillPreSet Status;
				memset(&Status, 0, sizeof(TAAddSkillPreSet));

				Status.nRetCode = ERROR_DB;
				Status.nAccountDBID = pStatus->nAccountDBID;

				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );
				if (pWorldDB)
				{
					Status.nRetCode = pWorldDB->QueryAddSkillPreset(pStatus);
				}
				else
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QUERY_ADD_SKILLPRESET:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(Status));
			}
			break;

		case QUERY_DEL_SKILLPRESET:
			{
				TQDelSkillPreSet * pStatus = (TQDelSkillPreSet*)pData;

				TADelSkillPreSet Status;
				memset(&Status, 0, sizeof(TADelSkillPreSet));

				Status.nRetCode = ERROR_DB;
				Status.nAccountDBID = pStatus->nAccountDBID;

				pWorldDB = g_SQLConnectionManager.FindWorldDB( nThreadID, pStatus->cWorldSetID );
				if (pWorldDB)
				{
					Status.nRetCode = pWorldDB->QueryDelSkillPreset(pStatus);
				}
				else
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, pStatus->biCharacterDBID, 0, L"[CDBID:%d] [QUERY_ADD_SKILLPRESET:%d] Query Error Ret:%d\r\n",pStatus->nAccountDBID, pStatus->cWorldSetID, Status.nRetCode);
				m_pConnection->AddSendData(nMainCmd, nSubCmd, (char*)&Status, sizeof(Status));
			}
			break;
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
#if defined( PRE_ADD_NEWCOMEBACK )
		case QUERY_MOD_COMEBACKFLAG:				// 인증정보 세팅 (서버간 이동 직전) (GA, VI 사용)
			{
				TQModComebackFlag* pStatus = reinterpret_cast<TQModComebackFlag*>(pData);
				
				pMembershipDB = g_SQLConnectionManager.FindMembershipDB(nThreadID);
				if (pMembershipDB){
					int nRetCode = pMembershipDB->QueryModComebackFlag( pStatus->nAccountDBID, pStatus->bRewardComeback );
					if (ERROR_NONE != nRetCode) {
						g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_COMEBACKFLAG] Query Error Result:%d\r\n", pStatus->nAccountDBID, nRetCode);
					}
				}
				else{
					g_Log.Log(LogType::_ERROR, pStatus->cWorldSetID, pStatus->nAccountDBID, 0, 0, L"[ADBID:%d] [QUERY_MOD_COMEBACKFLAG] Query not found\r\n", pStatus->nAccountDBID);
				}
			}
			break;
#endif
	}
}
