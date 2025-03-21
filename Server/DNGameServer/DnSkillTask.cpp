#include "StdAfx.h"
#include "DnSkillTask.h"
#include "DnProtocol.h"
#include "DNUserSession.h"
#include "DnActor.h"
#include "DNLogConnection.h"
#include "DNMissionSystem.h"
#include "DNDBConnectionManager.h"
#include "DnGameDataManager.h"
#include "DnPlayerActor.h"
#include "DNDBConnection.h"

CDnSkillTask::CDnSkillTask( CDNGameRoom* pRoom ) : CTask( pRoom ), 
												   CMultiSingleton<CDnSkillTask, MAX_SESSION_COUNT>( pRoom )
{
	m_listWaitSkillLevelup.clear();
}

CDnSkillTask::~CDnSkillTask(void)
{
	list<S_SKILL_LEVELUP_INFO*>::iterator iter = m_listWaitSkillLevelup.begin();
	for( iter; iter != m_listWaitSkillLevelup.end(); ++iter )
		delete (*iter);
	m_listWaitSkillLevelup.clear();
}

bool CDnSkillTask::Initialize( void )
{
	//m_SkillTreeSystem.InitializeTable();

	return true;
}

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
int CDnSkillTask::UnLockSkillByMoney( CDNUserSession* pSession, int nSkillID )
#else
bool CDnSkillTask::UnLockSkillByMoney( CDNUserSession* pSession, int nSkillID )
#endif
{
	CDnPlayerActor * pPlayerActor = pSession->GetPlayerActor();

	_ASSERT( pPlayerActor );

	if( nSkillID <= 0 )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIREFAIL;
#else
		pSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIREFAIL );
		return false;
#endif
	}

	TSkillData * pSkillData = g_pDataManager->GetSkillData( nSkillID );
	if( NULL == pSkillData )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIREFAIL;
#else
		pSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIREFAIL );
		return false;
#endif
	}

	if( pPlayerActor->IsExclusiveSkill( nSkillID, pSkillData->nExclusiveID ) )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE;
#else
		pSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE );
		return false;
#endif
	}

	bool bAlreadyGlobalSkillAcquired = false;
	if( pPlayerActor->HasSameGlobalIDSkill( pSkillData->nSkillID ) )
		bAlreadyGlobalSkillAcquired = true;

	const INT64 iNowMoney = pSession->GetCoin();
	if( iNowMoney < pSkillData->nUnlockPrice )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY;
#else
		pSession->SendAcquireSkill( nSkillID, ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY );
		return false;
#endif
	}

	DnSkillHandle hSkill = pPlayerActor->FindSkill( nSkillID );
	if( NULL != hSkill.GetPointer() )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_UNLOCK_ALREADY_OPEN;
#else
		pSession->SendAcquireSkill( nSkillID, ERROR_SKILL_UNLOCK_ALREADY_OPEN );
		return false;
#endif
	}

	int nRetCode = ERROR_NONE;
	CDnSkillTreeSystem * pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();

#ifdef PRE_ADD_ONLY_SKILLBOOK
	if( pSkillTreeSystem )
	{
		bool bExistSkill = false;
		bool bNeedSkillBook = pSkillTreeSystem->IsNeedSkillBook( nSkillID, bExistSkill );
		if( bExistSkill && bNeedSkillBook )
		{
			return ERROR_SKILL_UNLOCK_FAIL;
		}
	}
#endif // PRE_ADD_ONLY_SKILLBOOK

	CDnSkillTreeSystem::S_OUTPUT Output;
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( pPlayerActor->GetPossessedSkillInfo() );
	TryAcquire.iCurrentCharLevel = pSession->GetLevel();
	TryAcquire.iTryAcquireSkillID = nSkillID;
	TryAcquire.iHasSkillPoint = pPlayerActor->GetAvailSkillPointByJob( nSkillID );

	pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	std::vector<int> nNeedSPValues;
	pSkillTreeSystem->GetNeedSPValuesByJob( nSkillID, nNeedSPValues );

	std::vector<int> nJobHistorys;
	pPlayerActor->GetJobHistory( nJobHistorys );

	const bool bAvailableSPByJob = IsAvailableSPByJob( nJobHistorys, nNeedSPValues, pPlayerActor );
	if( false == bAvailableSPByJob )
		Output.eResult = CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE;
#endif	// #if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)

	bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
										(CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
										(CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
										true == bAlreadyGlobalSkillAcquired;

	if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult || true == bIgnoreParentSkillCondition )
	{
		TSkill Skill;
		SecureZeroMemory( &Skill, sizeof(TSkill) );
		Skill.nSkillID = nSkillID;
		Skill.cSkillLevel = 0;
		Skill.bLock = false;

		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
		SkillInfo.iSkillID = nSkillID;
		SkillInfo.iSkillLevel = 0;
		SkillInfo.bCurrentLock = false;

		// 언락 및 획득.
		pPlayerActor->UnLockSkill( nSkillID, pSkillData->nUnlockPrice );
		pPlayerActor->AcquireSkill( nSkillID );

		// 클라로 결과 보내줌.
#if !defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		pSession->SendUnlockSkill( nSkillID, ERROR_NONE );
		pSession->SendAcquireSkill( nSkillID, ERROR_NONE );
#endif // #if !defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)		

#if defined( _GAMESERVER )
		g_Log.Log(LogType::_ERROR, pSession, L"SkillTask152 PrevCoin=%I64d ChangeGold=%d PickUpCoin=%I64d \r\n", pSession->GetCoin(), pSkillData->nUnlockPrice, pSession->GetPickUpCoin());
#endif
		pSession->DelCoin( pSkillData->nUnlockPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0 );
	}
	else
	{
		switch( Output.eResult )
		{
		case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
			nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
			break;

		case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
			nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
			break;

		case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
			nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
			break;

		case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
			nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
			break;

		default:
			nRetCode = ERROR_SKILL_UNLOCK_FAIL;
			break;
		}
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return nRetCode;
	}
	return ERROR_NONE;
#else
		pSession->SendAcquireSkill( nSkillID, nRetCode );
	}
	return true;
#endif
}

bool CDnSkillTask::UseSkillBook( CDNUserSession* pSession, int nItemID )
{
	_ASSERT( pSession->GetPlayerActor() );
	CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
	if (!pSkillTreeSystem){
		pSession->SendAcquireSkill( 0, ERROR_SKILL_UNLOCK_FAIL );
		return false;
	}

	// 이미 소유하고 있는 스킬인지 체크
	int iSkillIDToUnLock = pSkillTreeSystem->FindSkillBySkillBook( nItemID );

	// 서로 못배우게 한 스킬은 못배우게 처리.
	DnActorHandle hActor = pSession->GetActorHandle();
	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
	TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillIDToUnLock );
	if( pPlayerActor->IsExclusiveSkill( iSkillIDToUnLock, pSkillData->nExclusiveID ) )
	{
		pSession->SendAcquireSkill( 0, ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE );
		return false;
	}

	DnSkillHandle hAlreadyHasSkill = pSession->GetActorHandle()->FindSkill( iSkillIDToUnLock );
	int nRetCode = ERROR_NONE;
	int nResultSkillID = 0;
	if( !hAlreadyHasSkill )
	{
		CDnSkillTreeSystem::S_OUTPUT Output;

		//CDnSkillTreeSystem::S_CURRENT_USERSKILL_INFO CurrentSkillInfo( pPlayerActor->GetPossessedSkillInfo() );
		CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo;
		CurrentSkillInfo.iTryUnlockSkillID = iSkillIDToUnLock;
		CurrentSkillInfo.iCurrentCharLevel = pPlayerActor->GetLevel();
		CurrentSkillInfo.iSkillBookItemID = nItemID;

		DNTableFileFormat* pSkillTable = GetDNTable( CDnTableDB::TSKILL );
		int iNeedJob = pSkillTable->GetFieldFromLablePtr( iSkillIDToUnLock, "_NeedJob" )->GetInteger();
		if( pSession->GetPlayerActor()->IsPassJob( iNeedJob ) )
		{
			//CurrentSkillInfo.iJobID = iNeedJob;

			pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			CDnPlayerActor * pPlayerActor = pSession->GetPlayerActor();
			if( NULL != pPlayerActor )
			{
				std::vector<int> nNeedSPValues;
				pSkillTreeSystem->GetNeedSPValuesByJob(iSkillIDToUnLock, nNeedSPValues);

				std::vector<int> jobHistory;
				pPlayerActor->GetJobHistory( jobHistory );

				bool bAvailableSPByJob = IsAvailableSPByJob(jobHistory, nNeedSPValues, pPlayerActor);

				if (bAvailableSPByJob == false)
					Output.eResult = CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE;
			}
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

			// 결과에 따라 클라이언트에 통보.
			if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult )
			{
				// 성공.
				nResultSkillID = iSkillIDToUnLock;

				// 언락 스킬에 있는지 확인
				_ASSERT( pPlayerActor );

				if( pPlayerActor )
				{
					nRetCode = pPlayerActor->CanAcquireSkillIfUnlock( nResultSkillID );
					if( ERROR_NONE == nRetCode )
					{
						// 언락 및 획득.
						pPlayerActor->UnLockSkill( nResultSkillID );
						pPlayerActor->AcquireSkill( nResultSkillID );

						// 클라로 결과 보내줌.
						pSession->SendUnlockSkill( nResultSkillID, ERROR_NONE );
						pSession->SendAcquireSkill( nResultSkillID, nRetCode );

						return true;
					}
				}
			}
			else if ( CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE == Output.eResult )
				nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
			else
				nRetCode = ERROR_SKILL_UNLOCK_FAIL;
		}
		else
		{
			nRetCode = ERROR_SKILL_UNLOCK_MISMATCH_JOB;
		}
	}
	else
	{
		nRetCode = ERROR_SKILL_UNLOCK_ALREADY_OPEN;
	}

	pSession->SendAcquireSkill( nResultSkillID, nRetCode );
	
	return false;
}


void CDnSkillTask::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pBaseRoom && static_cast<CDNGameRoom*>(m_pBaseRoom)->GetRoomState() != _GAME_STATE_PLAY )
		return;

	if( !m_listWaitSkillLevelup.empty() )
	{
		list<S_SKILL_LEVELUP_INFO*>::iterator iter = m_listWaitSkillLevelup.begin();
		for( iter; iter != m_listWaitSkillLevelup.end(); )
		{
			S_SKILL_LEVELUP_INFO* pInfo = *iter;
			if( pInfo->hActor && pInfo->hSkill )
			{
				// 액티브 스킬인 경우 스킬 사용중이 아닌 경우.
				// 오토 패시브 스킬인 경우 발동 중이 아닐 경우에 스킬 레벨업 패킷을 처리해서 클라로 통보.
				if( (/*(pInfo->hSkill->GetSkillType() == CDnSkill::Active) &&*/ (pInfo->hActor->GetProcessSkill() != pInfo->hSkill)) ||
					((pInfo->hSkill->GetSkillType() == CDnSkill::AutoPassive) && (false == pInfo->hActor->IsProcessingAutoPassive(pInfo->hSkill->GetClassID()))) )
				{
					SkillLevelUp( pInfo->pSession, pInfo->nSkillID, pInfo->cUseSkillPoint );
					delete (*iter);
					iter = m_listWaitSkillLevelup.erase( iter );
					continue;
				}
			}
			else
			{
				// 사라진 액터 객체는 리스트에서 제거.
				delete (*iter);
				iter = m_listWaitSkillLevelup.erase( iter );
				continue;
			}
			 
			++iter;
		}
	}
}

int CDnSkillTask::OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char* pData, int nLen )
{
	switch( nMainCmd )
	{
		case CS_SKILL: 
			return OnRecvSkillMessage( pSession, nSubCmd, pData, nLen );
	}

	return ERROR_NONE;
}

int CDnSkillTask::OnRecvSkillMessage( CDNUserSession* pSession, int nSubCmd, char *pData, int nLen )
{
	switch( nSubCmd )
	{
		case eSkill::CS_UNLOCKSKILL_BY_MONEY_REQ:
			return OnRecvUnlockSkillByMoneyReq( pSession, (CSUnlockSkillByMoneyReq*)pData );
		case eSkill::CS_SKILLLEVELUP_REQ:
			return OnRecvSkillLevelUpReq( pSession, (CSSkillLevelUpReq*)pData );

		case eSkill::CS_ACQUIRESKILL_REQ:
			return OnRecvAcquireSkillReq( pSession, (CSAcquireSkillReq*)pData );			
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		case eSkill::CS_RESERVATION_SKILL_LIST_REQ:
			return OnRecvReservationSkillListReq( pSession, (CSReservationSkillListReq*)pData );
#endif
#ifdef PRE_ADD_PRESET_SKILLTREE
		case eSkill::CS_LIST_SKILLSET:
			{
				if (nLen != 0)
					return ERROR_INVALIDPACKET;

				return OnRecvSkillSetList( pSession );
			}
			break;

		case eSkill::CS_SAVE_SKILLSET:
			{
				CSSaveSkillSet * pPacket = (CSSaveSkillSet*)pData;

				if (sizeof(CSSaveSkillSet) - sizeof(pPacket->SKills) + (sizeof(TSkillSetPartialData) * pPacket->cCount) != nLen)
					return ERROR_INVALIDPACKET;

				return OnRecvSaveSkillSet( pSession, pPacket );
			}
			break;

		case eSkill::CS_DELETE_SKILLSET:
			{
				if (sizeof(CSDeleteSkillSet) != nLen)
					return ERROR_INVALIDPACKET;

				return OnRecvDeleteSkillSet( pSession, (CSDeleteSkillSet*)pData );
			}
			break;
#endif		//#ifdef PRE_ADD_PRESET_SKILLTREE
		default:
			return ERROR_UNKNOWN_HEADER;
	}
}

int CDnSkillTask::OnRecvSkillLevelUpReq( CDNUserSession* pSession, CSSkillLevelUpReq* pPacket )
{
	return SkillLevelUp(pSession, pPacket->nSkillID, pPacket->cUseSkillPoint);	
}

int CDnSkillTask::OnRecvAcquireSkillReq( CDNUserSession* pSession, CSAcquireSkillReq* pPacket )
{
	return AcquireSkill(pSession, pPacket->nSkillID);	
}

int CDnSkillTask::OnRecvUnlockSkillByMoneyReq( CDNUserSession* pSession, CSUnlockSkillByMoneyReq* pPacket )
{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	if( ERROR_NONE != UnLockSkillByMoney( pSession, pPacket->nSkillID ) )
#else
	if( false == UnLockSkillByMoney( pSession, pPacket->nSkillID ) )
#endif
		return ERROR_SKILL_UNLOCK_FAIL;

	return ERROR_NONE;
}

int CDnSkillTask::AcquireSkill(CDNUserSession* pSession, int nSkillID)
{
	// 언락 스킬에 있는지 확인
	CDnPlayerActor* pPlayerActor = pSession->GetPlayerActor();
	_ASSERT( pPlayerActor );

	if( pPlayerActor )
	{
		// #26902 임시 스킬이 추가가 된 상태. 클라에서 UI 막았지만 패킷 온 경우에도 처리.
		if( pPlayerActor->IsTempSkillAdded() )
			return ERROR_GENERIC_INVALIDREQUEST;

		int nRetCode = pPlayerActor->AcquireSkill( nSkillID );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return nRetCode;
#else
		pSession->SendAcquireSkill( nSkillID, nRetCode );
#endif
	}
	return ERROR_GENERIC_INVALIDREQUEST;
}

int CDnSkillTask::SkillLevelUp(CDNUserSession* pSession, int nSkillID, BYTE cUseSkillPoint)
{
	// TODO: 추후에 게임서버에서 유저 데이터 DB 에 쓰는 방식이 달라지면 여기도 바꿔줘야 한다.
	// 현재는 스킬 목록을 클라이언트 종료될 때 DB에 게임서버에서 알아서 써줌.
	int nError = -1;
	char cLevel = 0;

	// 해당 스킬을 갖고 있는지 체크
	// 스킬 안 갖고 있는 경우엔 초기 데이터가 뭔지 물어봐서 알아내자.
	DnActorHandle hActor = pSession->GetActorHandle();
	if( !hActor )
		return ERROR_GENERIC_INVALIDREQUEST;

	// 임시 스킬이 추가가 된 상태. 클라에서 UI 막았지만 패킷 온 경우에도 처리.
	if( pSession->GetPlayerActor() &&
		pSession->GetPlayerActor()->IsTempSkillAdded() )
		return ERROR_GENERIC_INVALIDREQUEST;

	DnSkillHandle hFindedSkill = hActor->FindSkill( nSkillID );

	if( hFindedSkill )
	{
		// 빌리지 서버에는 장비로 추가된 스킬이 아예 없을 것. 따라서 레벨업 불가능.
		if( hFindedSkill->IsEquipItemSkill() )
			nError = ERROR_SKILL_LEVELUPFAIL_EQUIPITEMSKILL;
		else
		// 오라/토글링 땐 레벨업 불가능.
		if( hFindedSkill->IsToggleOn() || hFindedSkill->IsAuraOn() )
			nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
		else
		{
			// 즉시 사용 스킬, 혹은 오토 패시브 스킬이 시전 중일 때는 레벨업을 끝날 때 까지 기다렸다가 처리..
			CDnSkill::SkillTypeEnum eSkillType = hFindedSkill->GetSkillType();
			if( (/*(eSkillType == CDnSkill::Active) &&*/ (hFindedSkill == hActor->GetProcessSkill())) ||
				((eSkillType == CDnSkill::AutoPassive) && (hActor->IsProcessingAutoPassive(nSkillID))) )
			{
				// 같은 스킬의 레벨업 요청에 연속으로 왔을 땐 무시한다.
				list<S_SKILL_LEVELUP_INFO*>::iterator iter = m_listWaitSkillLevelup.begin();
				for( iter; m_listWaitSkillLevelup.end() != iter; ++iter )
				{
					if( (*iter)->nSkillID == nSkillID )
						return ERROR_NONE;
				}

				S_SKILL_LEVELUP_INFO* pSkillLevelupInfo = new S_SKILL_LEVELUP_INFO;
				pSkillLevelupInfo->hActor = hActor;
				pSkillLevelupInfo->hSkill = hFindedSkill;
				pSkillLevelupInfo->nSkillID = nSkillID;
				pSkillLevelupInfo->cUseSkillPoint = cUseSkillPoint;
				pSkillLevelupInfo->pSession = pSession;

				m_listWaitSkillLevelup.push_back( pSkillLevelupInfo );

				return ERROR_NONE;
			}

			int iLevelFrom = hFindedSkill->GetLevel();
			
			//실제 레벨을 계산한다.
			int nLevelUpValue = hFindedSkill->GetLevelUpValue();
			iLevelFrom -= nLevelUpValue;

			DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

			vector<int> vlSkillLevelList;
			//if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", pPacket->nSkillID, vlSkillLevelList ) == 0 ) 
			GetSkillLevelList( nSkillID, vlSkillLevelList, CDnSkill::PVE );
			if( vlSkillLevelList.empty() )
			{
				_ASSERT( "스킬 테이블 데이터 오류" );
				nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
			}

#if defined(PRE_FIX_NEXTSKILLINFO)
			int nLevelUpCount = 1;	// 스킬레벨은 무조건 1씩 올리게 되어있음.

			SKILL_LEVEL_TABLE_IDS::iterator findIter;
			SKILL_LEVEL_INFO* pTableInfo = g_pDataManager->GetSkillLevelTableIDList(hFindedSkill->GetClassID(), hFindedSkill->GetSelectedLevelDataType());

			int nNeedSkillPoint = 0;
			bool bValidPacket = false;
			if( NULL != pTableInfo )
			{
				int nSkillLevel = iLevelFrom + nLevelUpCount;
				findIter = pTableInfo->_SkillLevelTableIDs.find(nSkillLevel);

				int SkillLevelTableID = -1;
				if (findIter != pTableInfo->_SkillLevelTableIDs.end())
					SkillLevelTableID = findIter->second;

				if( -1 != SkillLevelTableID )
				{
					nNeedSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( SkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
					if( nNeedSkillPoint > 0)
#else
					if( nNeedSkillPoint == cUseSkillPoint )
#endif // #if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
						bValidPacket = true;
				}
			}

			if( false == bValidPacket )
			{
				// 패킷의 데이터가 서버와 다르므로 에러로 처리해서 돌려보낸다.
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
				return ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
#else
				pSession->SendSkillLevelUp(nSkillID, cLevel, ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT);	// 날려주는 함수가 이미 있어서 요걸로 바꿔놨어요~
				return ERROR_NONE;
#endif //#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
			}
#else
			int iSkillLevelTableID = -1;
			for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
			{
				int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
				if( iNowLevel == iLevelFrom )
				{
					iSkillLevelTableID = vlSkillLevelList.at( i );
					break;
				}
			}

			int nNeedSkillPoint = 0;
			int nLevelUpCount = 0;
			while( cUseSkillPoint != nNeedSkillPoint && 0 < iSkillLevelTableID )
			{
				++nLevelUpCount;
				nNeedSkillPoint += pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+nLevelUpCount, "_NeedSkillPoint" )->GetInteger();

				if( cUseSkillPoint < nNeedSkillPoint )
				{
					// Note: 서버와 클라의 테이블 값이 다름. 해킹일 가능성이 있습니다.
					nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
					break;
				}
			}

#endif // PRE_FIX_NEXTSKILLINFO

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
			bool bAvailableSPByJob = false;

			std::vector<int> nNeedSPValues;
			GetNeedSPValuesByJob(nSkillID, nNeedSPValues);
			
			std::vector<int> jobHistory;
			CDnPlayerActor* pPlayerActor = pSession->GetPlayerActor();
			if (pPlayerActor)
				pPlayerActor->GetJobHistory(jobHistory);

			bAvailableSPByJob = IsAvailableSPByJob(jobHistory, nNeedSPValues, pPlayerActor);
			
			if (bAvailableSPByJob == false)
				nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
			


			int iAvailSPByJob = pSession->GetPlayerActor()->GetAvailSkillPointByJob( nSkillID );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
			if( nNeedSkillPoint <= iAvailSPByJob )
#else
			if( nNeedSkillPoint <= iAvailSPByJob && nNeedSkillPoint == cUseSkillPoint )
#endif
			{
				// 스킬 레벨 제한 체크 및 남은 SP 체크
#if defined(PRE_FIX_NEXTSKILLINFO)
				
				int iLevelTableIDToUp = -1;
				int iMaxLevelTableID = -1;
				int nSkillLevelUp = iLevelFrom + nLevelUpCount;

				//레벨업 할 스킬 테이블 ID
				findIter = pTableInfo->_SkillLevelTableIDs.find(nSkillLevelUp);
				if (findIter != pTableInfo->_SkillLevelTableIDs.end())
					iLevelTableIDToUp = findIter->second;

				//MaxLevel 테이블
				findIter = pTableInfo->_SkillLevelTableIDs.find(pTableInfo->_MaxLevel);
				if (findIter != pTableInfo->_SkillLevelTableIDs.end())
					iMaxLevelTableID = findIter->second;

				if (nSkillLevelUp <= pTableInfo->_MaxLevel)
#else
				int iLevelTableIDToUp = hFindedSkill->GetSkillLevelID() + nLevelUpCount;
				int iMaxLevelTableID = hFindedSkill->GetSkillLevelID() - (hFindedSkill->GetLevel()-1) + hFindedSkill->GetMaxLevel()-1;
				if( iLevelTableIDToUp <= iMaxLevelTableID )
#endif // PRE_FIX_NEXTSKILLINFO
				{
					int iLevelLimit = pSkillLevelTable->GetFieldFromLablePtr( iLevelTableIDToUp, "_LevelLimit" )->GetInteger();

					if( iLevelLimit <= hActor->GetLevel() )
					{
						// HP, SP 올려주는 패시브 스킬은 삭제되면 원래 스킬 없는 상태의 순수 HP SP 로 현재 HP/SP가
						// 클리핑 되게 때문에 미리 받아뒀다가 셋팅해준다..
						INT64 iHP = hActor->GetHP();
						int iSP = hActor->GetSP();

						float fElapsedDelayTime = hFindedSkill->GetElapsedDelayTime();
						bool bRemoveSuccess = hActor->RemoveSkill( nSkillID );
						//_ASSERT( bRemoveSuccess && "CDnSkillTask::OnRecvSkillLevelUpReq() -> 액터가 보유하지 않은 스킬을 레벨업 하려 합니다." );

						if( bRemoveSuccess )
						{
							if( 0 < pSession->GetSkillPoint() /*&&
								pPacket->cUseSkillPoint <= pUserData->Attribute.wSkillPoint*/ )
							{
								int iNewLevel = iLevelFrom + nLevelUpCount;
								hActor->AddSkill( nSkillID, iNewLevel );
								DnSkillHandle hNewSkill = hActor->FindSkill( nSkillID );
								hNewSkill->SetElapsedDelayTime( fElapsedDelayTime );

								hActor->SetHP( iHP );
								hActor->SetSP( iSP );

								pSession->ChangeSkillPoint(-nNeedSkillPoint, nSkillID, false, 0 );
								pSession->GetDBConnection()->QueryModSkillLevel( pSession, nSkillID, iNewLevel, (int)( fElapsedDelayTime * 1000.f ), -nNeedSkillPoint, DBDNWorldDef::SkillChangeCode::Use);	// db저장: 스킬포인트까지 같이 업데이트
								
								cLevel = (char)iNewLevel;

								CDnPlayerActor * pPlayerActor = (CDnPlayerActor *)hActor.GetPointer();
								pPlayerActor->ReplacementGlyph( hNewSkill );

								pSession->GetEventSystem()->OnEvent( EventSystem::OnSkillLevelUp, 2, EventSystem::SkillID, nSkillID, EventSystem::SkillLevel, cLevel );
								nError = ERROR_NONE;
							}
							else
							{
								// 스킬 레벨업 포인트 부족함. ERROR_SK_LEVELUP_NOT_ENOUGH_POINT 클라로 리턴
								nError = ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_SKILLPOINT;
							}
						}
					}
					else
					{
						//TODO: 캐릭터의 레벨이 스킬의 제한 레벨 보다 작습니다..
						nError = ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_CHARACTERLEVEL;
					}
				}
				else
				{
					//TODO: 최대로 정해져 있는 스킬 레벨 이상을 올리려고 함..
					nError = ERROR_SKILL_LEVELUPFAIL_ALREADY_MAX;
				}
			}
			else
			{
				// TODO: 실제 사용 스킬포인트보다 보유 포인트보다 적거나 테이블의 데이터와 다름.
				nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
			}
		}
	}
	else
	{
		// 해당 스킬 보유하고 있지 않음. ERROR_SK_LEVELUP_NOT_EXIST_FAIL 클라로 리턴
		nError = ERROR_SKILL_ISNOT_YOURS;
	}
	
#if !defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	pSession->SendSkillLevelUp(nSkillID, cLevel, nError);	// 날려주는 함수가 이미 있어서 요걸로 바꿔놨어요~
#endif
	
	//hActor->ReplacementSkill( pPacket->nSkillID, cLevel );

	if (nError != ERROR_NONE)
		return ERROR_NONE;

	// Note: 패시브 버프 스킬로 인한 스탯 변화를 위해서 다른 플레이어들에게도 알려줘야 한다.
	DWORD dwNumPartyMember = pSession->GetGameRoom()->GetUserCount();
	if( 0 < nSkillID && 1 < dwNumPartyMember )
	{
		int iPartyMemberIndex = -1;
		for (DWORD i = 0; i < dwNumPartyMember; i++)
		{
			CDNGameRoom::PartyStruct *pStruct = pSession->GetGameRoom()->GetPartyData(i);
			if( pStruct == NULL ) continue;
			if( pStruct->pSession == pSession )
				iPartyMemberIndex = i;
		}
		
		_ASSERT( -1 != iPartyMemberIndex );
		if( -1 != iPartyMemberIndex )
		{
			for (DWORD i = 0; i < dwNumPartyMember; i++)
			{
				CDNGameRoom::PartyStruct *pStruct = pSession->GetGameRoom()->GetPartyData(i);
				if( pStruct == NULL ) continue;
				if( pStruct->pSession != pSession )
					pStruct->pSession->SendOtherPlayerSkillLevelUp( iPartyMemberIndex, nSkillID, cLevel );
			}
		}
	}
	return ERROR_NONE;
}

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
int CDnSkillTask::OnRecvReservationSkillListReq( CDNUserSession* pSession, CSReservationSkillListReq* pUnlockSkillByMoneyReq )
{
	if( pUnlockSkillByMoneyReq->nCount <= 0 )
		return ERROR_GENERIC_UNKNOWNERROR;	

	SCReservationSkillListAck ReservationSkillAck;
	memset(&ReservationSkillAck, 0, sizeof(SCReservationSkillListAck));

	for( int i=0; i<pUnlockSkillByMoneyReq->nCount; ++i)
	{
		memcpy(&ReservationSkillAck.tReservationSkillAck[i].tReservationSkill, &pUnlockSkillByMoneyReq->tReservationSkill[i], sizeof(TReservationSkillReq));
		if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::UnLock)
		{			
			ReservationSkillAck.tReservationSkillAck[i].nResult = UnLockSkillByMoney( pSession, pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
		}
		else if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::Acquire)
		{		
			ReservationSkillAck.tReservationSkillAck[i].nResult = AcquireSkill(pSession, pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
		}
		else if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::LevelUp)
		{
			int nResult = ERROR_NONE;
			DnActorHandle hActor = pSession->GetActorHandle();
			if( hActor )
			{
				DnSkillHandle hFindedSkill = hActor->FindSkill( pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID );
				if( hFindedSkill )
				{
					int nCurLevel = hFindedSkill->GetLevel();

					//실제 레벨을 계산한다.
					int nLevelUpValue = hFindedSkill->GetLevelUpValue();
					nCurLevel -= nLevelUpValue;				
					if( nCurLevel >= pUnlockSkillByMoneyReq->tReservationSkill[i].nLevel)
					{
						nResult = ERROR_GENERIC_UNKNOWNERROR;
					}
					else
					{
						for( int j=nCurLevel;j<pUnlockSkillByMoneyReq->tReservationSkill[i].nLevel;++j)
						{
							nResult = SkillLevelUp(pSession, pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID, 0);
							if( nResult != ERROR_NONE)
								break;
						}
					}
				}
				else
					nResult = ERROR_GENERIC_UNKNOWNERROR;			
			}
			else
				nResult = ERROR_GENERIC_UNKNOWNERROR;
			ReservationSkillAck.tReservationSkillAck[i].nResult = nResult;			
		}
		else
			break;
		++ReservationSkillAck.nCount;
		if(ReservationSkillAck.tReservationSkillAck[i].nResult != ERROR_NONE ) // 도중에 1개라도 에러나면 그냥 마무리..
			break;
	}
	int nLen = sizeof(SCReservationSkillListAck) - sizeof(ReservationSkillAck.tReservationSkillAck) + (sizeof(TReservationSkillAck)*ReservationSkillAck.nCount);
	pSession->AddSendData(SC_SKILL, eSkill::SC_RESERVATION_SKILL_LIST_ACK, (char*)&ReservationSkillAck, nLen);
	return ERROR_NONE;
}
#endif

#if defined(PRE_ADD_PRESET_SKILLTREE)
int CDnSkillTask::OnRecvSkillSetList( CDNUserSession* pSession )
{
	if (pSession->GetDBConnection())
		pSession->GetDBConnection()->QueryGetSkillPresetList(pSession);

	return ERROR_NONE;
}

int CDnSkillTask::OnRecvSaveSkillSet( CDNUserSession* pSession, CSSaveSkillSet *pPacket )
{
	if (wcslen(pPacket->wszSetName) <= 0)
		return ERROR_GENERIC_UNKNOWNERROR;

	if (pPacket->cIndex < 0 || pPacket->cIndex >= SKILLPRESETMAX)
		return ERROR_GENERIC_UNKNOWNERROR;

	if (pSession->GetDBConnection())
		pSession->GetDBConnection()->QueryAddSkillPreset(pSession, pPacket);

	return ERROR_NONE;
}

int CDnSkillTask::OnRecvDeleteSkillSet( CDNUserSession* pSession, CSDeleteSkillSet *pPacket )
{
	if (pSession->GetDBConnection())
		pSession->GetDBConnection()->QueryDelSkillPreset(pSession, pPacket->cIndex);

	return ERROR_NONE;
}
#endif	// #if defined(PRE_ADD_PRESET_SKILLTREE)

void CDnSkillTask::GetSkillLevelList( int iSkillID, vector<int>& vlSkillLevelList, int iLevelDataType )
{
	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", iSkillID, vlSkillLevelList );

	// pve, pvp 대상인지 확인하여 걸러냄.
	vector<int>::iterator iterLevelList = vlSkillLevelList.begin();
	for( iterLevelList; iterLevelList != vlSkillLevelList.end(); )
	{
		int iSkillLevelTableID = *iterLevelList;
		int iApplyType = pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID, "_ApplyType" )->GetInteger();
		if( iApplyType != iLevelDataType )
			iterLevelList = vlSkillLevelList.erase( iterLevelList );
		else
			++iterLevelList;
	}
}

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
bool CDnSkillTask::IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, CDnPlayerActor * pPlayerActor)
{
	bool isAvailableSPByJob = true;

	int nJobCount = (int)jobHistory.size();
	int nJobID = -1;

	for (int i = 0; i < nJobCount; ++i)
	{
		if (IsAvailableSPByJob(jobHistory, needSPValues, i, pPlayerActor) == false)
		{
			isAvailableSPByJob = false;
			break;
		}
	}

	return isAvailableSPByJob;
}

bool CDnSkillTask::IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex, CDnPlayerActor * pPlayerActor)
{
	bool isAvailableSPByJob = false;

	int nJobCount = (int)jobHistory.size();

	int nNeedSPValue = 0;
	int nSPValueCount = (int)needSPValues.size();
	if (nIndex >= 0 && nIndex < nSPValueCount)
		nNeedSPValue = needSPValues[nIndex];

	//전직이 되지 않은 경우??
	if (nIndex >= nJobCount)
	{
		if (nNeedSPValue <= 0)
			return true;
		else
			return false;
	}

	int nJobID = -1;

	nJobID = jobHistory[nIndex];
	int iUsedSkillPointInThisJob = pPlayerActor->GetUsedSkillPointInThisJob( nJobID );

	//해당 JobID에 사용한 SP값이 확인용 값보다 커야 사용 가능..
	if (iUsedSkillPointInThisJob >= nNeedSPValue)
		isAvailableSPByJob = true;

	return isAvailableSPByJob;
}

void CDnSkillTask::GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues)
{
	CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
	if (pSkillTreeSystem)
		pSkillTreeSystem->GetNeedSPValuesByJob(nSkillID, nNeedSPValues);
}
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP