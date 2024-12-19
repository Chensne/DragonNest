#include "StdAfx.h"
#include "DNUserSkill.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#include "DNLogConnection.h"
#include "DNMissionSystem.h"
#include "DNDBConnectionManager.h"
#include "DNDBConnection.h"

extern TVillageConfig g_Config;

CDNUserSkill::CDNUserSkill(CDNUserSession *pUserObj)
: m_pUserSession(pUserObj)
{

	m_pSkillData = m_pUserSession->GetSkillData( true );
	for(DWORD i=0; i<DualSkill::Type::MAX; i++ )
		m_SkillList[i].clear();
}

CDNUserSkill::~CDNUserSkill(void)
{
	for(DWORD i=0; i<DualSkill::Type::MAX; i++ )
		m_SkillList[i].clear();
}

void CDNUserSkill::LoadUserData()
{
	int nUseSkillPoint = 0;

	int nOpendSkillPageCount = m_pUserSession->GetItem()->GetSkillPageCount();

	for(int nSkillCount =0 ; nSkillCount < nOpendSkillPageCount; nSkillCount++)
	{
		for (int i = 0; i < SKILLMAX; i++){
			if (m_pSkillData[nSkillCount].SkillList[i].nSkillID <= 0) continue;

			// 데이터에 없는 스킬 ID가 DB 에 저장되어있다면 읽어들이지 않는다.
			const TSkillData* pSkillData = g_pDataManager->GetSkillData( m_pSkillData[nSkillCount].SkillList[i].nSkillID );
			if( NULL == pSkillData )
				continue;

			m_SkillList[nSkillCount][m_pSkillData[nSkillCount].SkillList[i].nSkillID] = m_pSkillData[nSkillCount].SkillList[i];

			CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
			SkillInfo.iSkillID = m_pSkillData[nSkillCount].SkillList[i].nSkillID;
			SkillInfo.iSkillLevel = m_pSkillData[nSkillCount].SkillList[i].cSkillLevel;
			SkillInfo.bCurrentLock = m_pSkillData[nSkillCount].SkillList[i].bLock;

			m_vlPossessedSkill[nSkillCount].push_back( SkillInfo );


			if (m_pSkillData[nSkillCount].SkillList[i].nSkillID > 0){
				for (int j = 0; j < m_pSkillData[nSkillCount].SkillList[i].cSkillLevel; j++){
					nUseSkillPoint += g_pDataManager->GetNeedSkillPoint(m_pSkillData[nSkillCount].SkillList[i].nSkillID, j + 1);
				}
			}
		}

		// 문장스킬 체크
		const TItem *pItem = m_pUserSession->GetItem()->GetGlyph( GLYPH_SPECIALSKILL );
		if( pItem && pItem->nItemID ) {
			if( pItem->nItemID > 0 ) {
				TItemData *pItemData = g_pDataManager->GetItemData( pItem->nItemID );
				if( pItemData && (pItemData->nSkillID > 0) ) {
					TSkillData *pSkillData = g_pDataManager->GetSkillData( pItemData->nSkillID );
					TSkill Skill;
					Skill.nSkillID = pSkillData->nSkillID;
					Skill.cSkillLevel = 1;
					Skill.nCoolTime = 0;
					Skill.bLock = false;

					m_SkillList[nSkillCount][pItemData->nSkillID] = Skill;

					CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
					SkillInfo.iSkillID = pItemData->nSkillID;
					SkillInfo.iSkillLevel = Skill.cSkillLevel;
					SkillInfo.bCurrentLock = Skill.bLock;

					m_vlPossessedSkill[nSkillCount].push_back( SkillInfo );

				}
			}
		}
	}

#ifdef PRE_ADD_SP_REVISION

#ifdef _FINAL_BUILD
	// 보유 스킬 포인트 검증. 틀어져 있더라도 보정 후 클라에게 따로 패킷은 안보낸다. 
	// 여기 지나간 다음에 클라이언트에게 정보가 보내지기 때문에..
	CheckAndRevisionSkillPoint( false );
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION
}

void CDNUserSkill::SaveUserData(TUserData *pUserData)
{
	if (m_SkillList[GetCurrentSkillPage()].empty()) return;
	memset(&(pUserData->Skill[DualSkill::Type::Primary].SkillList), 0, sizeof(TSkill) * SKILLMAX);

	TMapSkillList::iterator iter;
	int nCount = 0;
	for (iter = m_SkillList[GetCurrentSkillPage()].begin(); iter != m_SkillList[GetCurrentSkillPage()].end(); ++iter){
		pUserData->Skill[DualSkill::Type::Primary].SkillList[nCount] = iter->second;
		pUserData->Skill[DualSkill::Type::Primary].SkillList[nCount].nCoolTime = 0;
		nCount++;
	}
}

// 스킬 트리로 바뀌면서 addskill 치트에서만 쓰이는 함수입니다..
void CDNUserSkill::AddSkill( int nSkillID, int iLevel/* = 0*/ )
{
	// 20080901 로그서버 스킬추가[획득]
	if ( nSkillID <= 0 )
	{
		m_pUserSession->SendAddSkill(nSkillID, ERROR_SKILL_ACQUIREFAIL);
		return;
	}

	// 획득한 스킬 또 획득할 순 없음.
	if( FindSkill( nSkillID ) )
	{
		m_pUserSession->SendAddSkill(nSkillID, ERROR_SKILL_ACQUIREFAIL);
		return;
	}

	TSkill Skill;
	memset(&Skill, 0, sizeof(TSkill));
	Skill.nSkillID = nSkillID;
	Skill.cSkillLevel = iLevel;
	//Skill.bLock = true;		// 최초에 스킬 얻었을 때는 언락 상태. (레벨 0 상태)
	Skill.bLock = false;

	m_SkillList[GetCurrentSkillPage()][nSkillID] = Skill;
	
	CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
	SkillInfo.iSkillID = nSkillID;
	SkillInfo.iSkillLevel = Skill.cSkillLevel;
	SkillInfo.bCurrentLock = Skill.bLock;

	m_vlPossessedSkill[GetCurrentSkillPage()].push_back( SkillInfo );

	if( m_pUserSession )
	{
		m_pUserSession->GetDBConnection()->QueryAddSkill(m_pUserSession, nSkillID, Skill.cSkillLevel, 0, 0, DBDNWorldDef::SkillChangeCode::GainByAdmin);
		m_pUserSession->SendAddSkill(nSkillID, ERROR_NONE);

		m_pUserSession->GetEventSystem()->OnEvent( EventSystem::OnSkillAdd, 1, EventSystem::SkillID, nSkillID );
	}
}
 
void CDNUserSkill::DelSkill(int nSkillID)
{
	if (nSkillID <= 0){
		m_pUserSession->SendDelSkill(nSkillID, ERROR_SKILL_DELETEFAIL);
		return;
	}

	if (m_SkillList[GetCurrentSkillPage()].empty()){
		m_pUserSession->SendDelSkill(nSkillID, ERROR_SKILL_ISNOT_YOURS);
		return;
	}

	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find(nSkillID);
	if (iter != m_SkillList[GetCurrentSkillPage()].end()){
		m_SkillList[GetCurrentSkillPage()].erase(iter);

		vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill[GetCurrentSkillPage()].begin();
		for( iter; m_vlPossessedSkill[GetCurrentSkillPage()].end() != iter; ++iter )
		{
			if( iter->iSkillID == nSkillID )
			{
				m_vlPossessedSkill[GetCurrentSkillPage()].erase( iter );
				break;
			}
		}

		m_pUserSession->SendDelSkill(nSkillID, ERROR_NONE);
		return;
	}

	if( m_pUserSession )
	{
		m_pUserSession->GetDBConnection()->QueryDelSkill(m_pUserSession, nSkillID, DBDNWorldDef::SkillChangeCode::DelByAdmin, false, GetCurrentSkillPage() );
		m_pUserSession->SendDelSkill(nSkillID, ERROR_SKILL_ISNOT_YOURS);
	}
}

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
int CDNUserSkill::SkillLevelUp( int nSkillID, int nUsePoint )
#else
void CDNUserSkill::SkillLevelUp( int nSkillID, int nUsePoint )
#endif
{
#ifdef PRE_ADD_SP_REVISION

	// #29463 이슈 관련 현재 보유 스킬 포인트와 스킬 소유 리스트가 다른 경우엔 여기서 보정해준다.
#ifdef _FINAL_BUILD
	CheckAndRevisionSkillPoint();
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION

	bool bReqSuccess = false;
	int nError = ERROR_NONE;

	bool bFinded = false;

	if (nSkillID <= 0)
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_GENERIC_UNKNOWNERROR;;
#else
		return;
#endif//#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)

	if (m_SkillList[GetCurrentSkillPage()].empty()) 
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_GENERIC_UNKNOWNERROR;;
#else
		return;
#endif//#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)		

	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find(nSkillID);
	if (iter != m_SkillList[GetCurrentSkillPage()].end())
		bFinded = true;
	else
	{
		_ASSERT( "스킬 리스트에서 스킬을 찾을 수 없습니다." );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_GENERIC_UNKNOWNERROR;;
#else
		return;
#endif //#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	}

	TSkill& SkillInfo = iter->second;

	DNTableFileFormat* pSkillLevelTable = GetDNTable( CDnTableDB::TSKILLLEVEL );

	vector<int> vlSkillLevelList;
	if( pSkillLevelTable->GetItemIDListFromField( "_SkillIndex", nSkillID, vlSkillLevelList ) == 0 ) 
	{
		_ASSERT( "스킬 테이블 데이터 오류" );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_GENERIC_UNKNOWNERROR;;
#else
		return;
#endif//#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)		
	}

#if defined(PRE_FIX_NEXTSKILLINFO)
	int nLevelUpCount = 1;	// 스킬레벨은 무조건 1씩 올리게 되어있음.
	
	SKILL_LEVEL_TABLE_IDS::iterator findIter;
	SKILL_LEVEL_INFO* pTableInfo = g_pDataManager->GetSkillLevelTableIDList(nSkillID, 0);

	int nNeedSkillPoint = 0;
	bool bValidPacket = false;
	if( NULL != pTableInfo )
	{
		int nSkillLevel = SkillInfo.cSkillLevel + nLevelUpCount;
		findIter = pTableInfo->_SkillLevelTableIDs.find(nSkillLevel);

		int SkillLevelTableID = -1;
		if (findIter != pTableInfo->_SkillLevelTableIDs.end())
			SkillLevelTableID = findIter->second;
	
		if( -1 != SkillLevelTableID )
		{
			nNeedSkillPoint = pSkillLevelTable->GetFieldFromLablePtr( SkillLevelTableID, "_NeedSkillPoint" )->GetInteger();
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
			if( nNeedSkillPoint > 0 )
#else
			if( nNeedSkillPoint == nUsePoint )
#endif
				bValidPacket = true;
		}
	}
	
	if( false == bValidPacket )
	{
		// 패킷의 데이터가 서버와 다르므로 에러로 처리해서 돌려보낸다.
		nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
		bFinded = false;
	}
#else
	int iSkillLevelTableID = -1;
	for( int i = 0; i < (int)vlSkillLevelList.size(); ++i )
	{
		int iNowLevel = pSkillLevelTable->GetFieldFromLablePtr( vlSkillLevelList.at(i), "_SkillLevel" )->GetInteger();
		if( iNowLevel == SkillInfo.cSkillLevel )
		{
			iSkillLevelTableID = vlSkillLevelList.at( i );
			break;
		}
	}

	if( -1 == iSkillLevelTableID )
	{
		_ASSERT( "스킬 테이블 데이터 오류" );
		return;
	}

	int nNeedSkillPoint = 0;
	int nLevelUpCount = 0;
	while( nUsePoint != nNeedSkillPoint )
	{
		++nLevelUpCount;
		nNeedSkillPoint += pSkillLevelTable->GetFieldFromLablePtr( iSkillLevelTableID+nLevelUpCount, "_NeedSkillPoint" )->GetInteger();

		if( nUsePoint < nNeedSkillPoint )
		{
			nError = ERROR_SKILL_LEVELUPFAIL_INVALID_SKILLPOINT;
			bFinded = false;
			break;
		}
	}
#endif // PRE_FIX_NEXTSKILLINFO

	// 해당 스킬을 갖고 있는지 체크
	// 스킬 안 갖고 있는 경우엔 초기 데이터가 뭔지 물어봐서 알아내자.
	//bool bFinded = FindSkill( nSkillID );

	if( bFinded )
	{	
		// 보유한 스킬 포인트가 사용하겠다고 날아온 포인트보다 작음
		int iAvailSPByJob = GetAvailSkillPointByJob( nSkillID );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		if( nNeedSkillPoint <= iAvailSPByJob )
#else
		if( nNeedSkillPoint <= iAvailSPByJob &&	nUsePoint == nNeedSkillPoint )
#endif
		{
			int iLevelFrom = GetSkillLevel( nSkillID );

			// 스킬 레벨 제한 체크
			TSkillData *pSkillData = g_pDataManager->GetSkillData(nSkillID);
			if (!pSkillData){
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
				return ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS;;
#else
				m_pUserSession->SendSkillLevelUp( nSkillID, 0, ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS );
				return;
#endif//#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)				
			}

			int iNewLevel = iLevelFrom + nLevelUpCount;
			if( iNewLevel <= pSkillData->nMaxLevel )
			{
				TSkillLevelData *pSkillLevel = g_pDataManager->GetSkillLevelData(nSkillID, iNewLevel);
				if (!pSkillLevel){
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
					return ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS;
#else
					m_pUserSession->SendSkillLevelUp( nSkillID, 0, ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS );
					return;
#endif
				}

				if( pSkillLevel->cLevelLimit <= (int)m_pUserSession->GetLevel() )
				{
					if( 0 < m_pUserSession->GetSkillPoint() )
					{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
						SetSkillLevel( nSkillID, iNewLevel, DBDNWorldDef::SkillChangeCode::Use, false );	// 스킬포인트 안에서 차감
#else
						SetSkillLevel( nSkillID, iNewLevel, DBDNWorldDef::SkillChangeCode::Use );	// 스킬포인트 안에서 차감
#endif //#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
						bReqSuccess = true;
					}
					else
					{
						// 스킬 레벨업 포인트 부족함. ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_SKILLPOINT 클라로 리턴
						nError = ERROR_SKILL_LEVELUPFAIL_INSUFFICIENCY_SKILLPOINT;
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
	else
	{
		// 해당 스킬 보유하고 있지 않음. ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS 클라로 리턴
		nError = ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS;
	}
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	return nError;
#else
	if (!bReqSuccess)
		m_pUserSession->SendSkillLevelUp( nSkillID, 0, nError );
#endif
}

bool CDNUserSkill::FindSkill(int nSkillID)
{
	if (nSkillID <= 0) return false;

	if (m_SkillList[GetCurrentSkillPage()].empty()) return false;

	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find(nSkillID);
	if (iter != m_SkillList[GetCurrentSkillPage()].end()){
		return true;
	}

	return false;
}

int CDNUserSkill::GetSkillLevel( int nSkillID )
{

	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( nSkillID );
	if( iter != m_SkillList[GetCurrentSkillPage()].end() ) {
		return iter->second.cSkillLevel;
	}
	return -1;
}

void CDNUserSkill::SetSkillLevel( int nSkillID, int nLevel, int nLogCode, bool bSendSkillLevelUp /*= true*/ )
{
	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( nSkillID );
	if( iter != m_SkillList[GetCurrentSkillPage()].end() ) {
		int nGap = nLevel - iter->second.cSkillLevel;
		iter->second.cSkillLevel = nLevel;
		
		//////////////////////////////////////////////////////////////////////////
		// 게임서버와 마찬가지로 레벨이 한꺼번에 변하는 경우도 감안.
		// 현재 1레벨씩만 올리게 되어있지만 게임서버와 동일하게 맞춰준다.
		int nNeedSkillPoint = 0;
		for( int i = 0; i < nGap; ++i )
			nNeedSkillPoint += g_pDataManager->GetNeedSkillPoint(nSkillID, nLevel-i);
		//////////////////////////////////////////////////////////////////////////

		if (nNeedSkillPoint > 0) m_pUserSession->ChangeSkillPoint(-nNeedSkillPoint, nSkillID, false, 0, GetCurrentSkillPage() );
		m_pUserSession->GetDBConnection()->QueryModSkillLevel( m_pUserSession, nSkillID, nLevel, iter->second.nCoolTime, -nNeedSkillPoint, nLogCode);	// db저장: 스킬포인트까지 같이 업데이트

		if( bSendSkillLevelUp )
			m_pUserSession->SendSkillLevelUp( nSkillID, nLevel, ERROR_NONE );

		vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill[GetCurrentSkillPage()].begin();
		for( iter; m_vlPossessedSkill[GetCurrentSkillPage()].end() != iter; ++iter )
		{
			if( iter->iSkillID == nSkillID )
			{
				iter->iSkillLevel = nLevel;
				break;
			}
		}

		m_pUserSession->GetEventSystem()->OnEvent( EventSystem::OnSkillLevelUp, 2, EventSystem::SkillID, nSkillID, EventSystem::SkillLevel, nLevel );
	}
}


// 스킬 언락. 레벨 0인 상태로 캐릭터의 보유 스킬에 추가.
void CDNUserSkill::UnLockSkill( int nSkillID )
{
	TSkill Skill;
	memset(&Skill, 0, sizeof(TSkill));
	Skill.nSkillID = nSkillID;
	Skill.cSkillLevel = 0;
	Skill.bLock = false;		// 락 변수는 필요 없을듯;;

	m_SkillList[GetCurrentSkillPage()][nSkillID] = Skill;

	// 따로 필요한 정보만 모아놓은 벡터에 추가.
	CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
	SkillInfo.iSkillID = nSkillID;
	SkillInfo.iSkillLevel = 0;
	SkillInfo.bCurrentLock = false;
	m_vlPossessedSkill[GetCurrentSkillPage()].push_back( SkillInfo );

	m_pUserSession->SendUnlockSkill( nSkillID, ERROR_NONE );
	m_pUserSession->GetDBConnection()->QueryAddSkill(m_pUserSession, nSkillID, SkillInfo.iSkillLevel, 0, DBDNWorldDef::SkillChangeCode::GainByBook, 0);

	// TODO: 스킬 획득 관련 미션 쪽 물어봐서 처리.
	//m_pUserSession->GetEventSystem()->OnEvent( EventSystem::OnSkillAdd, 1, EventSystem::SkillID, nSkillID );

	return;
}

// 마을에서 스킬 트레이너 NPC 로 돈으로 언락. 현재 획득까지 처리.
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
int CDNUserSkill::UnLockSkillByMoney( int nSkillID )
#else
void CDNUserSkill::UnLockSkillByMoney( int nSkillID )
#endif
{
#ifdef PRE_ADD_SP_REVISION

	// #29463 이슈 관련 현재 보유 스킬 포인트와 스킬 소유 리스트가 다른 경우엔 여기서 보정해준다.
#ifdef _FINAL_BUILD
	CheckAndRevisionSkillPoint();
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION

	if (nSkillID <= 0){
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIREFAIL;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIREFAIL );
		return;
#endif //#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	}

	// 보유하고 있는 돈으로 되는지 확인.
	TSkillData* pSkillData = g_pDataManager->GetSkillData( nSkillID );
	if (!pSkillData){
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIREFAIL;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIREFAIL );
		return;
#endif
	}

	// 같이 못배우게 한 스킬은 배울 수 없다.
	if( IsExclusiveSkill( nSkillID, pSkillData->nExclusiveID ) )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE );
		return;
#endif
	}

	// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
	bool bAlreadyGlobalSkillAcquired = false;
	if( HasSameGlobalIDSkill( pSkillData->nGlobalSkillGroup ) )
		bAlreadyGlobalSkillAcquired = true;

	INT64 iNowMoney = m_pUserSession->GetCoin();
	if( iNowMoney < pSkillData->nUnlockPrice )
	{
		//m_pUserSession->SendUnlockSkill( nSkillID, ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY );
		// 클라에서 메시지 핸들링 보여주는 것을 전부 획득에서 처리하므로..
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_UNLOCK_NOT_ENOUGH_MONEY );
		return;
#endif
	}

	bool bAlreadyPossess = this->FindSkill( nSkillID );
	if( bAlreadyPossess )
	{
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_SKILL_UNLOCK_ALREADY_OPEN;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_UNLOCK_ALREADY_OPEN );
		return;
#endif
	}

	// 그러면 획득 조건을 검사해서 획득함.
	int nRetCode = ERROR_NONE;
	CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();

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
	CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( GetPossessedSkillInfo() );
	TryAcquire.iCurrentCharLevel = m_pUserSession->GetLevel();
	TryAcquire.iTryAcquireSkillID = nSkillID;
	// 직업별로 정해져 있는 쓸 수 있는 SP 를 기준으로 한다.
	TryAcquire.iHasSkillPoint = GetAvailSkillPointByJob( nSkillID );

	pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

	bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
										 (CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
										 (CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
										 true == bAlreadyGlobalSkillAcquired;

	if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult || true == bIgnoreParentSkillCondition )
	{
		// 스킬 리스트에 스킬 추가.
		TSkill Skill;
		memset(&Skill, 0, sizeof(TSkill));
		Skill.nSkillID = nSkillID;
		Skill.cSkillLevel = 0;
		Skill.bLock = false;		// 락 변수는 필요 없을듯;;

		m_SkillList[GetCurrentSkillPage()][nSkillID] = Skill;

		// 따로 필요한 정보만 모아놓은 벡터에 추가.
		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
		SkillInfo.iSkillID = nSkillID;
		SkillInfo.iSkillLevel = 0;
		SkillInfo.bCurrentLock = false;

		m_vlPossessedSkill[GetCurrentSkillPage()].push_back( SkillInfo );

		// 돈 차감
		m_pUserSession->DelCoin( pSkillData->nUnlockPrice, DBDNWorldDef::CoinChangeCode::DoNotDBSave, 0 );
		
		// 언락 패킷 클라로 보냄.
#if !defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		m_pUserSession->SendUnlockSkill( nSkillID, ERROR_NONE );
#endif // #if !defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		m_pUserSession->GetDBConnection()->QueryAddSkill(m_pUserSession, nSkillID, SkillInfo.iSkillLevel, 0, DBDNWorldDef::SkillChangeCode::GainByBuy, pSkillData->nUnlockPrice);

		// TODO: 스킬 획득 관련 미션 쪽 물어봐서 처리.
		//m_pUserSession->GetEventSystem()->OnEvent( EventSystem::OnSkillAdd, 1, EventSystem::SkillID, nSkillID );

		SetSkillLevel(nSkillID, 1, DBDNWorldDef::SkillChangeCode::Use, false);	// 함수안에서 스킬포인트 수정

		// 획득 성공 결과를 최종 통보.CDnSkillTreeSystem::S_OUTPUT Output;CDnSkillTreeSystem::S_OUTPUT Output;
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return ERROR_NONE;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, ERROR_NONE );
#endif
	}
	else
	{
		switch( Output.eResult )
		{
				// 캐릭터 요구레벨이 모자람.
			case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
				nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
				break;

				// 선행(부모) 스킬이 없음.
			case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
				nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
				break;

				// 부모 스킬의 레벨이 충족되지 않음.
			case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
				nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
				break;

				// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
			case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
				nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
				break;

			default:
				nRetCode = ERROR_SKILL_UNLOCK_FAIL;
				break;
		}
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		return nRetCode;
#else
		m_pUserSession->SendAcquireSkill( nSkillID, nRetCode );
#endif
	}
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	return ERROR_NONE;
#endif
}

bool CDNUserSkill::UseSkillBook( int nItemID )
{
#ifdef PRE_ADD_SP_REVISION

	// #29463 이슈 관련 현재 보유 스킬 포인트와 스킬 소유 리스트가 다른 경우엔 여기서 보정해준다.
#ifdef _FINAL_BUILD
	CheckAndRevisionSkillPoint();
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION

	CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();

	// 스킬북 사용, 트레이너에게 돈 지불 시에 스킬 언락 및 획득까지 일괄 처리되도록 변경됨.
	// SP 가 부족하거나 선행 스킬 없거나, 선행 스킬의 레벨이 낮거나, 캐릭터의 레벨이 낮은 경우 전부 체크해서 통과 되었을 경우에
	// 스킬 언락 및 1렙으로 찍어준다.

	// 이미 언락한 스킬은 다시 언락할 수 없다. 
	// 언락된 상태인 레벨 0일 때도 스킬 소유로 할 것인지는 고민해보자.
	int iSkillIDToUnLock = pSkillTreeSystem->FindSkillBySkillBook( nItemID );
	if( 0 == iSkillIDToUnLock )
	{
		g_Log.Log( LogType::_ERROR, L"[UseSkillBook Fail!!!] Can't find SkillID that using %d SkillBook Item ID\n", iSkillIDToUnLock );
		_ASSERT( !"해당 스킬북 아이템 ID 로 설정된 스킬을 찾을 수 없음." );
		return false;
	}

	// 같이 못배우게 한 스킬은 배울 수 없다.
	const TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillIDToUnLock );
	if( IsExclusiveSkill( iSkillIDToUnLock, pSkillData->nExclusiveID ) )
	{
		m_pUserSession->SendAcquireSkill( iSkillIDToUnLock, ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE );
		return false;
	}

	// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
	bool bAlreadyGlobalSkillAcquired = false;
	if( HasSameGlobalIDSkill( pSkillData->nGlobalSkillGroup ) )
		bAlreadyGlobalSkillAcquired = true;

	bool bSuccess = false;
	bool bAlreadyPossess = this->FindSkill( iSkillIDToUnLock );
	int nRetCode = ERROR_NONE;
	int nResultSkillID = 0;
	if( false == bAlreadyPossess )
	{
		CDnSkillTreeSystem::S_OUTPUT Output;

		CDnSkillTreeSystem::S_TRY_UNLOCK CurrentSkillInfo;
		CurrentSkillInfo.iTryUnlockSkillID = iSkillIDToUnLock;
		CurrentSkillInfo.iCurrentCharLevel = m_pUserSession->GetLevel();
		CurrentSkillInfo.iSkillBookItemID = nItemID;

		// 스킬이 요구하는 직업이 히스토리에 있는지 적절히 찾아서 처리.
		TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillIDToUnLock );

		if (pSkillData == NULL) 
		{
			g_Log.Log( LogType::_ERROR, L"[UseSkillBook - GetSkillData Fail!!!] Can't find %d SkillID\n", iSkillIDToUnLock );
			return false;
		}

		CurrentSkillInfo.iJobID = -1;
		for( int i = 0; i < JOBMAX; ++i )
		{
			BYTE cJob = m_pUserSession->GetStatusData()->cJobArray[ i ];
			if( 0 < cJob )
			{
				if( cJob == pSkillData->nNeedJobID )
				{				
					CurrentSkillInfo.iJobID = pSkillData->nNeedJobID;
				}
			}
			else
				break;
		}

		if( -1 != CurrentSkillInfo.iJobID )
		{
			pSkillTreeSystem->TryUnLockSkill( CurrentSkillInfo, &Output );

			// 결과에 따라 클라이언트에 통보.
			if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult )
			{
				// 성공.
				nResultSkillID = iSkillIDToUnLock;

				// 그러면 획득 조건을 검사.
				CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
				CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( GetPossessedSkillInfo() );
				TryAcquire.iCurrentCharLevel = m_pUserSession->GetLevel();
				TryAcquire.iTryAcquireSkillID = nResultSkillID;
				// 직업별로 정해져 있는 쓸 수 있는 SP 를 기준으로 한다.
				TryAcquire.iHasSkillPoint = GetAvailSkillPointByJob( nResultSkillID );

				pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
				std::vector<int> nNeedSPValues;
				pSkillTreeSystem->GetNeedSPValuesByJob(nResultSkillID, nNeedSPValues);

				std::vector<int> jobHistory;
				GetJobHistory(m_pUserSession->GetStatusData()->cJobArray, jobHistory);

				bool bAvailableSPByJob = IsAvailableSPByJob(jobHistory, nNeedSPValues);

				if (bAvailableSPByJob == false)
					Output.eResult = CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE;
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP

				bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
													 (CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
													 (CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
													 true == bAlreadyGlobalSkillAcquired;

				if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult ||
					true == bIgnoreParentSkillCondition )
				{
					// 클라에 통보 후 유저객체에서 스킬정보 꺼내서 레벨 0으로 셋팅... 및 스킬 추가.
					UnLockSkill( nResultSkillID );  // SendUnlockSkill() 내부에서 호출되어 패킷 나감.
					bSuccess = true;

					SetSkillLevel( nResultSkillID, 1, DBDNWorldDef::SkillChangeCode::Use, false );
					
					// 획득 성공 결과를 최종 통보.
					m_pUserSession->SendAcquireSkill( nResultSkillID, ERROR_NONE );

				}
				else
				{
					switch( Output.eResult )
					{
							// 캐릭터 요구레벨이 모자람.
						case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
							nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
							break;

							// 선행(부모) 스킬이 없음.
						case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
							nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
							break;

							//// 부모 스킬이 락이 되어있음.
							//case CDnSkillTreeSystem::R_LOCKED_PARENTSKILL:
							//	break;

							// 부모 스킬의 레벨이 충족되지 않음.
						case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
							nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
							break;

							// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
						case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
							nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
							break;
					}

				}
			}
			else
			{
				nRetCode = ERROR_SKILL_UNLOCK_FAIL;
			}
		}
		else
		{
			nRetCode = ERROR_SKILL_UNLOCK_MISMATCH_JOB;
		}
	}
	else
	{
		// 이미 언락 한 상태임.
		nRetCode = ERROR_SKILL_UNLOCK_ALREADY_OPEN;
	}

	if( false == bSuccess )
	{
		//m_pUserSession->SendUnlockSkill( nResultSkillID, nRetCode );
		m_pUserSession->SendAcquireSkill( nResultSkillID, nRetCode );
	}

	return bSuccess;
}


// 스킬 획득. (레벨 0에서 1로 바꿈)
// 스킬 리셋 후 다시 획득시 이게 호출된다.
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
int CDNUserSkill::AcquireSkill( int nSkillID )
#else
void CDNUserSkill::AcquireSkill( int nSkillID )
#endif
{
#ifdef PRE_ADD_SP_REVISION

	// #29463 이슈 관련 현재 보유 스킬 포인트와 스킬 소유 리스트가 다른 경우엔 여기서 보정해준다.
#ifdef _FINAL_BUILD
	CheckAndRevisionSkillPoint();
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION

	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( nSkillID );
	int nRetCode = ERROR_NONE;
	if( iter != m_SkillList[GetCurrentSkillPage()].end() )
	{
		_ASSERT( false == iter->second.bLock );
		_ASSERT( 0 == iter->second.cSkillLevel );

		// 스킬 리셋 후 재획득 하는 경우에 상호 배타적인 스킬이 있는지 체크
		// 같이 못배우게 한 스킬은 배울 수 없다.
		TSkillData* pSkillData = g_pDataManager->GetSkillData( nSkillID );
		if( m_pUserSession->GetSkill()->IsExclusiveSkill( nSkillID, pSkillData->nExclusiveID ) )
		{
			// 기본적으로는 클라에서 막기 때문에 서버로 패킷을 보낸다면 핵이다.
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)			
			return ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE;
#else
			m_pUserSession->SendAcquireSkill( nSkillID, ERROR_SKILL_ACQUIRE_FAIL_EXCLUSIVE );
			return;
#endif // #if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
		}

		// #36858 글로벌 스킬로 서로 엮여 있다면 해당 그룹중에 하나만 배워도 부모 스킬 조건없이 배울 수 있다.
		bool bAlreadyGlobalSkillAcquired = false;
		if( HasSameGlobalIDSkill( pSkillData->nGlobalSkillGroup ) )
			bAlreadyGlobalSkillAcquired = true;

		CDnSkillTreeSystem* pSkillTreeSystem = g_pDataManager->GetSkillTreeSystem();
		CDnSkillTreeSystem::S_TRY_ACQUIRE TryAcquire( GetPossessedSkillInfo() );
		CDnSkillTreeSystem::S_OUTPUT Output;

		TryAcquire.iCurrentCharLevel = m_pUserSession->GetLevel();
		TryAcquire.iTryAcquireSkillID = nSkillID;
		// 직업별로 정해져 있는 쓸 수 있는 SP 를 기준으로 한다.
		TryAcquire.iHasSkillPoint = GetAvailSkillPointByJob( nSkillID );
		
		// 스킬이 요구하는 직업이 히스토리에 있는지 적절히 찾아서 처리.
		TryAcquire.iJobID = -1;
		if (pSkillData){
			for( int i = 0; i < JOBMAX; ++i )
			{
				BYTE cJob = m_pUserSession->GetStatusData()->cJobArray[ i ];
				if( 0 < cJob )
				{
					if( cJob == pSkillData->nNeedJobID )
					{				
						TryAcquire.iJobID = pSkillData->nNeedJobID;
					}
				}
				else
					break;
			}
		}

		if( -1 != TryAcquire.iJobID )
		{
			pSkillTreeSystem->TryAcquireSkill( TryAcquire, &Output );

			bool bIgnoreParentSkillCondition = ( (CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL == Output.eResult) ||
												 (CDnSkillTreeSystem::R_LOCKED_PARENTSKILL == Output.eResult) ||
												 (CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL == Output.eResult) ) &&
												 true == bAlreadyGlobalSkillAcquired;

			if( CDnSkillTreeSystem::R_SUCCESS == Output.eResult ||
				true == bIgnoreParentSkillCondition )
			{
				SetSkillLevel( nSkillID, 1, DBDNWorldDef::SkillChangeCode::Use, false );

				// TODO: 스킬 획득 관련 미션 쪽 물어봐서 처리.
				//m_pUserSession->GetEventSystem()->OnEvent( EventSystem::OnSkillAdd, 1, EventSystem::SkillID, nSkillID );
			}
			else
			{
				switch( Output.eResult )
				{
					// 캐릭터 요구레벨이 모자람.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_CHAR_LEVEL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_CHAR_LEVEL;
					break;

					// 선행(부모) 스킬이 없음.
				case CDnSkillTreeSystem::R_DONT_HAVE_PARENT_SKILL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_DONT_HAVE_PARENT_SKILL;
					break;

					//// 부모 스킬이 락이 되어있음.
					//case CDnSkillTreeSystem::R_LOCKED_PARENTSKILL:
					//	break;

					// 부모 스킬의 레벨이 충족되지 않음.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_PARENT_SKILL_LEVEL:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_PARENT_SKILL_LEVEL;
					break;

					// 스킬 포인트가 모자라서 스킬을 획득할 수 없음.
				case CDnSkillTreeSystem::R_NOT_ENOUGH_SKILLPOINT_TO_ACQUIRE:
					nRetCode = ERROR_SKILL_ACQUIRE_FAIL_NOT_ENOUGH_SKILLPOINT;
					break;
				}
			}
		}
		else
			nRetCode = ERROR_SKILL_ACQUIRE_FAIL_MISMATCH_JOB;
	}
	else
	{
		// 해당 스킬 보유하고 있지 않음. ERROR_SKILL_LEVELUPFAIL_ISNOT_YOURS 클라로 리턴
		nRetCode = ERROR_SKILL_ACQUIRE_FAIL_ISNOT_YOURS;
	}
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)			
	return nRetCode;
#else
	m_pUserSession->SendAcquireSkill( nSkillID, nRetCode );
#endif
}


// 치트키 전용 스킬 획득 함수..
void CDNUserSkill::CheatAcquireSkill( int nSkillID )
{
	TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( nSkillID );
	int nRetCode = ERROR_NONE;
	if( iter != m_SkillList[GetCurrentSkillPage()].end() )
	{
		SetSkillLevel( nSkillID, 1, DBDNWorldDef::SkillChangeCode::ModSkillLevelByAdmin, false );
		m_pUserSession->SendAcquireSkill( nSkillID, nRetCode );
	}
}

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
void CDNUserSkill::ReservationSkillList(CSReservationSkillListReq* pUnlockSkillByMoneyReq)
{
	if( pUnlockSkillByMoneyReq->nCount <= 0 )
		return;	

	SCReservationSkillListAck ReservationSkillAck;
	memset(&ReservationSkillAck, 0, sizeof(SCReservationSkillListAck));

	for( int i=0; i<pUnlockSkillByMoneyReq->nCount; ++i)
	{
		memcpy(&ReservationSkillAck.tReservationSkillAck[i].tReservationSkill, &pUnlockSkillByMoneyReq->tReservationSkill[i], sizeof(TReservationSkillReq));
		if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::UnLock)
		{			
			ReservationSkillAck.tReservationSkillAck[i].nResult = UnLockSkillByMoney(pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
		}
		else if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::Acquire)
		{		
			ReservationSkillAck.tReservationSkillAck[i].nResult = AcquireSkill(pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
		}
		else if(pUnlockSkillByMoneyReq->tReservationSkill[i].cType == ReservationSKillList::Type::LevelUp)
		{
			int nResult = ERROR_NONE;
			int nCurLevel = GetSkillLevel(pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
			if( nCurLevel >= pUnlockSkillByMoneyReq->tReservationSkill[i].nLevel)
			{
				nResult = ERROR_GENERIC_UNKNOWNERROR;			
			}
			else
			{
				for( int j=nCurLevel;j<pUnlockSkillByMoneyReq->tReservationSkill[i].nLevel;++j)
				{
					nResult = SkillLevelUp(pUnlockSkillByMoneyReq->tReservationSkill[i].nSkillID);
					if( nResult != ERROR_NONE)
						break;
				}
			}
			ReservationSkillAck.tReservationSkillAck[i].nResult = nResult;			
		}
		else
			break;
		++ReservationSkillAck.nCount;
		if(ReservationSkillAck.tReservationSkillAck[i].nResult != ERROR_NONE ) // 도중에 1개라도 에러나면 그냥 마무리..
			break;
	}
	int nLen = sizeof(SCReservationSkillListAck) - sizeof(ReservationSkillAck.tReservationSkillAck) + (sizeof(TReservationSkillAck)*ReservationSkillAck.nCount);
	m_pUserSession->AddSendData(SC_SKILL, eSkill::SC_RESERVATION_SKILL_LIST_ACK, (char*)&ReservationSkillAck, nLen);
}
#endif //#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)


void CDNUserSkill::ResetSkill( int nSkillPage )
{
	// 전직시에 모든 스킬 초기화.
	// 나머진 캐시템 갖고 해야함.

	ResetAllSkill( nSkillPage );
	m_pUserSession->GetDBConnection()->QueryResetSkill(m_pUserSession, nSkillPage);
}


void CDNUserSkill::WaitForUseSkillResetCashItemFromDBServer( vector<int>& vlSkillIDsToReset )
{
	m_vlWaitingSkillIDsToReset = vlSkillIDsToReset;
}

void CDNUserSkill::OnResponseSkillResetCashItemFromDBServer( bool bSuccess )
{
	if( bSuccess )
	{
		// 받아놓았던 스킬 아이디들을 리셋. 스킬 포인트는 DB에서 받자마자 처리해주므로 여기서 신경 안써도 됨.
		int aiDefaultSkills[ DEFAULTSKILLMAX ] = { 0 };
		g_pDataManager->GetCreateDefaultSkill( m_pUserSession->GetClassID(), aiDefaultSkills );

		int nCount = 0;
		for( int i = 0; i != (int)m_vlWaitingSkillIDsToReset.size(); ++i )
		{

			TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( m_vlWaitingSkillIDsToReset.at(i) );
			_ASSERT( m_SkillList[GetCurrentSkillPage()].end() != iter );
			if( m_SkillList[GetCurrentSkillPage()].end() == iter )
				continue;

			bool bDefaultSkill = false;
			for( int k = 0; k < DEFAULTSKILLMAX; ++k )
			{
				if( aiDefaultSkills[ k ] == iter->second.nSkillID )
				{
					bDefaultSkill = true;
					break;
				}
			}

			if( bDefaultSkill )
			{
				iter->second.cSkillLevel = 1;
			}
			else
				iter->second.cSkillLevel = 0;

			for( UINT k = 0; k < m_vlPossessedSkill[GetCurrentSkillPage()].size(); ++k )
			{
				if( iter->second.nSkillID == m_vlPossessedSkill[GetCurrentSkillPage()].at( k ).iSkillID )
					m_vlPossessedSkill[GetCurrentSkillPage()].at( k ).iSkillLevel = iter->second.cSkillLevel;
			}
			nCount++;
		}
	}
	
	m_vlWaitingSkillIDsToReset.clear();
}


void CDNUserSkill::GatherThisJobSkill( BYTE cJob, IN OUT vector<int>& vlResult )
{
	// 추후 속도에 문제가 된다면 따로 리스트를 구성해 사용하도록 한다.

	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill[GetCurrentSkillPage()].begin();
	for ( iter; iter != m_vlPossessedSkill[GetCurrentSkillPage()].end(); ++iter )
	{
		TSkillData* pSkillData = g_pDataManager->GetSkillData( iter->iSkillID );
		if (!pSkillData) continue;
		if( pSkillData->nNeedJobID == cJob )
		{
			// 획득까지 한 상태여야 실제 스킬 보유 상태임.
			if( 0 < iter->iSkillLevel )
				vlResult.push_back( iter->iSkillID );
		}
	}
}

int CDNUserSkill::GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel )
{
	if( nPrevLevel == nCurLevel ) return 0;

	int nSkillPoint = 0;
	for( int i=nPrevLevel+1; i<=nCurLevel; i++ ) {
		nSkillPoint += CPlayerLevelTable::GetInstance().GetValue( m_pUserSession->GetClassID(), i, CPlayerLevelTable::SkillPoint );
	}
	return nSkillPoint;
}


bool CDNUserSkill::IsExclusiveSkill( int nSkillID, int nExclusiveID )
{
	if( 0 != nExclusiveID )
	{
		TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].begin();
		for(iter; iter != m_SkillList[GetCurrentSkillPage()].end(); ++iter)
		{
			const TSkill& ExistSkillData = iter->second;
			if( ExistSkillData.nSkillID <= 0 )
				continue;

			if( nSkillID == ExistSkillData.nSkillID )
				continue;

			// 레벨 0 짜리로 리셋된 것은 얻은 것이 아니므로 비교대상이 아님.
			if( 0 == ExistSkillData.cSkillLevel )
				continue;

			const TSkillData* pSkillData = g_pDataManager->GetSkillData( ExistSkillData.nSkillID );
			if( 0 != pSkillData->nExclusiveID )
			{
				if( nExclusiveID == pSkillData->nExclusiveID )
					return true;
			}
		}
	}

	return false;
}

int CDNUserSkill::GetUsedSPByJob( int nJobID )
{
	int iResult = 0;

	for( int i = 0; i < (int)m_vlPossessedSkill[GetCurrentSkillPage()].size(); ++i )
	{
		int iSkillID = m_vlPossessedSkill[GetCurrentSkillPage()].at( i ).iSkillID;
		const TSkillData* pSkillData = g_pDataManager->GetSkillData( iSkillID );

		// 파이널 빌드일때만 NULL 체크. 테스트 단계에서는 죽도록..
#if defined(_FINAL_BUILD)
		if( NULL == pSkillData )
			continue;
#endif // #if defined(_FINAL_BUILD)

		if( pSkillData->nNeedJobID == nJobID )
		{
			int iLevel = m_vlPossessedSkill[GetCurrentSkillPage()].at( i ).iSkillLevel;
			int iSize = (int)pSkillData->vLevelDataList.size();
			for( int k = 0; k < min(iLevel, iSize); ++k )
				iResult += pSkillData->vLevelDataList.at( k ).nNeedSkillPoint;

			if (iLevel > iSize)
				g_Log.Log(LogType::_ERROR, L"GetUsedSPByJob - Wrong Skill Size(WORSETID:%d, CHARID:%d, iLevel:%d, iSize:%d)\r\n", m_pUserSession->GetWorldSetID(), m_pUserSession->GetCharacterDBID(), iLevel, iSize);
		}
	}
	return iResult;
}

int CDNUserSkill::GetAvailSkillPointByJob( int nSkillID )
{
	int iWholeSP = GetLevelUpSkillPoint( 1, m_pUserSession->GetLevel() );
	float fAvailSPRatioByJob = m_pUserSession->GetAvailSkillPointRatioByJob( nSkillID );
	const TSkillData* pSkillDataToLevelUp = g_pDataManager->GetSkillData( nSkillID );
	int iUsedSPThisJob = 0;
	if (pSkillDataToLevelUp)
		iUsedSPThisJob = GetUsedSPByJob( pSkillDataToLevelUp->nNeedJobID );

	// 전체 사용가능 SP 보다 직업 SP 가 남은 것이 많으면 전체 사용가능 SP 가 진짜이므로 해당 포인트로 리턴.
	int iAvailPoint = int(fAvailSPRatioByJob*iWholeSP)-iUsedSPThisJob;
	if( m_pUserSession->GetSkillPoint() < iAvailPoint )
		iAvailPoint = m_pUserSession->GetSkillPoint();

	return iAvailPoint;
}

void CDNUserSkill::OnAttachEquip( TItem *pItem )
{
	if( pItem->nItemID > 0 ) {
		TItemData *pItemData = g_pDataManager->GetItemData( pItem->nItemID );
		if( pItemData && (pItemData->nSkillID > 0) ) {
			if( !FindSkill( pItemData->nSkillID ) ) {
				TSkill Skill;
				memset(&Skill, 0, sizeof(TSkill));
				Skill.nSkillID = pItemData->nSkillID;
				Skill.cSkillLevel = 1;
				Skill.bLock = false;
				m_SkillList[GetCurrentSkillPage()][pItemData->nSkillID] = Skill;

				CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
				SkillInfo.iSkillID = pItemData->nSkillID;
				SkillInfo.iSkillLevel = Skill.cSkillLevel;
				SkillInfo.bCurrentLock = Skill.bLock;

				m_vlPossessedSkill[GetCurrentSkillPage()].push_back( SkillInfo );
			}
		}
	}
}

void CDNUserSkill::OnDetachEquip( TItem *pItem )
{
	if( pItem->nItemID > 0 ) {
		TItemData *pItemData = g_pDataManager->GetItemData( pItem->nItemID );
		if( pItemData && (pItemData->nSkillID > 0) ) {
			TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].find( pItemData->nSkillID );
			if (iter != m_SkillList[GetCurrentSkillPage()].end()){
				m_SkillList[GetCurrentSkillPage()].erase(iter);

				vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>::iterator iter = m_vlPossessedSkill[GetCurrentSkillPage()].begin();
				for( iter; m_vlPossessedSkill[GetCurrentSkillPage()].end() != iter; ++iter )
				{
					if( iter->iSkillID == pItemData->nSkillID )
					{
						m_vlPossessedSkill[GetCurrentSkillPage()].erase( iter );
						break;
					}
				}
			}
		}
	}
}


#ifdef PRE_ADD_SP_REVISION

#ifdef _FINAL_BUILD
void CDNUserSkill::CheckAndRevisionSkillPoint( bool bSendRevisionSPToClient/* = true*/ )
{
	if( NULL == m_pUserSession )
		return;

	// #29463 이슈 관련 현재 보유 스킬 포인트와 스킬 소유 리스트가 다른 경우엔 여기서 보정해준다.
	int nUseSkillPoint = 0;
	int nWholeSP = GetLevelUpSkillPoint( 1, m_pUserSession->GetLevel() );

	// 디폴트 스킬을 얻어와서 현재 디폴트 스킬들만 있고 1렙이라면 스킬을 하나도 찍은 게 없으므로 리셋할 수 없다.
	int aiDefaultSkills[ DEFAULTSKILLMAX ] = { 0 };
	g_pDataManager->GetCreateDefaultSkill( m_pUserSession->GetClassID(), aiDefaultSkills );

	int iNumSkill = (int)m_vlPossessedSkill[GetCurrentSkillPage()].size();
	for( int i = 0; i < iNumSkill; ++i )
	{
		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO& PossessedSkill = m_vlPossessedSkill[GetCurrentSkillPage()].at( i );

		// 디폴트 스킬이고 레벨 1이면 스킬 포인트 계산에서 뺀다.
		bool bDefaultSkill = false;
		bool bDefaultSkillOneLevel = false;
		for( int k = 0; k < DEFAULTSKILLMAX; ++k )
		{
			if( aiDefaultSkills[ k ] == 0 )
				break;

			if( PossessedSkill.iSkillID == aiDefaultSkills[ k ] )
			{
				bDefaultSkill = true;
				if( 1 == PossessedSkill.iSkillLevel )
				{
					bDefaultSkillOneLevel = true;
				}
				break;
			}
		}

		if( bDefaultSkill && bDefaultSkillOneLevel )
			continue;

		for( int k = 0; k < PossessedSkill.iSkillLevel; ++k )
		{
			// 디폴트 스킬은 1레벨의 스킬 포인트를 계산하지 않는다.
			// 테이블에 필요 SP 가 0으로 되어있지만 만에 하나 잘못 입력되었을 경우를 대비해 코드에서 막는다.
			if( bDefaultSkill && 0 == k )
				continue;

			nUseSkillPoint += g_pDataManager->GetNeedSkillPoint(PossessedSkill.iSkillID, k + 1);
		}
	}

	int nLeftSkillPoint = nWholeSP - nUseSkillPoint;

	// 현재 저장되어있는 스킬 포인트와 값이 다르다면 보정처리.
	int nNowSkillPoint = m_pUserSession->GetSkillPoint();
	
	// 만약 초과해서 이미 스킬을 찍은 상태라면 다음에 스킬리셋하고 스킬 획득할때 보정되도록 그냥 여기선 넘긴다.
	if( nLeftSkillPoint < 0 )
		return;

	if( nNowSkillPoint != nLeftSkillPoint )
	{
		m_pUserSession->SetSkillPoint( nLeftSkillPoint, GetCurrentSkillPage() );

#ifdef PRE_FIX_SP_REVISION_CHANGE_SP
		m_pUserSession->GetDBConnection()->QuerySetSkillPoint( m_pUserSession, nLeftSkillPoint, GetCurrentSkillPage() );
#else
		int nSkillPointDelta = nLeftSkillPoint - nNowSkillPoint;
		if( 0 < nSkillPointDelta )
		{
			m_pUserSession->GetDBConnection()->QueryIncreaseSkillPoint( m_pUserSession, nSkillPointDelta, DBDNWorldDef::SkillPointCode::Repair, GetCurrentSkillPage() );
		}
		else
		{
			// 감소 쿼리에도 감소될 스킬 포인트를 양수로 넘겨주면 DB 측에서 해당 포인트 차감.
			m_pUserSession->GetDBConnection()->QueryDecreaseSkillPoint( m_pUserSession,	-nSkillPointDelta, GetCurrentSkillPage() );
		}
#endif // #ifdef PRE_FIX_SP_REVISION_CHANGE_SP

		// 클라이언트에게도 스킬 포인트 패킷을 밀어준다.
		if( bSendRevisionSPToClient )
			m_pUserSession->SendPushSkillPoint( nLeftSkillPoint );
	}
}
#endif // #ifdef _FINAL_BUILD

#endif // #ifdef PRE_ADD_SP_REVISION

void CDNUserSkill::ResetAllSkill(int nSkillPage )
{
	// 기본 스킬을 제외하고 모든 보유 스킬을 0으로 리셋. 소모된 스킬 포인트 복구시킴.
	// 기본 스킬은 1렙으로.
	int aiDefaultSkills[ DEFAULTSKILLMAX ] = { 0 };
	g_pDataManager->GetCreateDefaultSkill( m_pUserSession->GetClassID(), aiDefaultSkills );

	TMapSkillList::iterator iter;
	int nCount = 0;

	if( nSkillPage < DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX )
		return;

	for (iter = m_SkillList[nSkillPage].begin(); iter != m_SkillList[nSkillPage].end(); ++iter)
	{
		bool bDefaultSkill = false;
		for( int i = 0; i < DEFAULTSKILLMAX; ++i )
		{
			if( aiDefaultSkills[ i ] == iter->second.nSkillID )
			{
				bDefaultSkill = true;
				break;
			}
		}

		if( bDefaultSkill )
		{
			iter->second.cSkillLevel = 1;
		}
		else
			iter->second.cSkillLevel = 0;

		for( int i = 0; i < (int)m_vlPossessedSkill[nSkillPage].size(); ++i )
		{
			if( iter->second.nSkillID == m_vlPossessedSkill[nSkillPage].at( i ).iSkillID )
				m_vlPossessedSkill[nSkillPage].at( i ).iSkillLevel = iter->second.cSkillLevel;
		}

		nCount++;
	}
}


#ifdef PRE_ADD_CHANGEJOB_CASHITEM
void CDNUserSkill::OnResponseChangeJobCode( USHORT wTotalSkillPoint )
{	
	for(int nSkillPage = DualSkill::Type::Primary; nSkillPage < DualSkill::Type::MAX ; nSkillPage++ )
	{
		ResetAllSkill( nSkillPage );
		m_pUserSession->SetSkillPoint( wTotalSkillPoint, nSkillPage );
	}
}
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

bool CDNUserSkill::HasSameGlobalIDSkill( int iGlobalSkillGroupID )
{
	bool bResult = false;

	if( 0 < iGlobalSkillGroupID )
	{
		TMapSkillList::iterator iter = m_SkillList[GetCurrentSkillPage()].begin();
		for(iter; iter != m_SkillList[GetCurrentSkillPage()].end(); ++iter)
		{
			const TSkillData* pExistSkillData = g_pDataManager->GetSkillData( iter->second.nSkillID );
			if( pExistSkillData )
			{
				if( pExistSkillData->nGlobalSkillGroup == iGlobalSkillGroupID )
				{
					bResult = true;
					break;
				}
			}
		}
	}

	return bResult;
}


void CDNUserSkill::ApplyExpendedSkillPage( int nSkillPage )
{
	if( nSkillPage == GetCurrentSkillPage() || nSkillPage <= DualSkill::Type::Primary || nSkillPage >= DualSkill::Type::MAX)
		return;

	int aiDefaultSkills[ DEFAULTSKILLMAX ] = { 0 };
	int addCount = 0;
	int nUseSkillPoint = 0;

	g_pDataManager->GetCreateDefaultSkill( m_pUserSession->GetClassID(), aiDefaultSkills );

	for(int i=0; i<DEFAULTSKILLMAX; i++ )
	{
		if( aiDefaultSkills[i] > 0 )
		{
			m_pSkillData[nSkillPage].SkillList[addCount].cSkillLevel = 1;
			m_pSkillData[nSkillPage].SkillList[addCount].nSkillID = aiDefaultSkills[i];
			m_pSkillData[nSkillPage].SkillList[addCount].nCoolTime = 0;
			addCount++;
		}
	}

	for (int i = 0; i < SKILLMAX; i++){
		if (m_pSkillData[nSkillPage].SkillList[i].nSkillID <= 0) continue;

		// 데이터에 없는 스킬 ID가 DB 에 저장되어있다면 읽어들이지 않는다.
		const TSkillData* pSkillData = g_pDataManager->GetSkillData( m_pSkillData[nSkillPage].SkillList[i].nSkillID );
		if( NULL == pSkillData )
			continue;

		m_SkillList[nSkillPage][m_pSkillData[nSkillPage].SkillList[i].nSkillID] = m_pSkillData[nSkillPage].SkillList[i];

		CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO SkillInfo;
		SkillInfo.iSkillID = m_pSkillData[nSkillPage].SkillList[i].nSkillID;
		SkillInfo.iSkillLevel = m_pSkillData[nSkillPage].SkillList[i].cSkillLevel;
		SkillInfo.bCurrentLock = m_pSkillData[nSkillPage].SkillList[i].bLock;

		m_vlPossessedSkill[nSkillPage].push_back( SkillInfo );


		if (m_pSkillData[nSkillPage].SkillList[i].nSkillID > 0){
			for (int j = 0; j < m_pSkillData[nSkillPage].SkillList[i].cSkillLevel; j++){
				nUseSkillPoint += g_pDataManager->GetNeedSkillPoint(m_pSkillData[nSkillPage].SkillList[i].nSkillID, j + 1);
			}
		}
	}

}


int CDNUserSkill::GetCurrentSkillPage()
{
	return m_pUserSession->GetSkillPage();
}

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
void CDNUserSkill::GetJobHistory(BYTE *cJobArray, vector<int>& jobHistory)
{
	for (int j = 0; j < JOBMAX; j++){
		if (cJobArray[j] <= 0) continue;
		
		jobHistory.push_back((int)cJobArray[j]);
	}
}

bool CDNUserSkill::IsAvailableSPByJob(vector<int>& jobHistory, vector<int>& needSPValues)
{
	bool isAvailableSPByJob = true;

	int nJobCount = (int)jobHistory.size();
	int nJobID = -1;

	for (int i = 0; i < nJobCount; ++i)
	{
		if (IsAvailableSPByJob(jobHistory, needSPValues, i) == false)
		{
			isAvailableSPByJob = false;
			break;
		}
	}

	return isAvailableSPByJob;
}

bool CDNUserSkill::IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex)
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
	int iUsedSkillPointInThisJob = GetUsedSPByJob( nJobID );

	//해당 JobID에 사용한 SP값이 확인용 값보다 커야 사용 가능..
	if (iUsedSkillPointInThisJob >= nNeedSPValue)
		isAvailableSPByJob = true;

	return isAvailableSPByJob;
}
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP