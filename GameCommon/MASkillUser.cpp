#include "StdAfx.h"
#include "MASkillUser.h"
#include "DnSkill.h"
#include "IDnSkillUsableChecker.h"
#include "IDnSkillProcessor.h"
#include "InputReceiver.h"
#include "DnProjectile.h"
#include "DnPlayerActor.h"
#include "DnChangeProjectileProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnBlow.h"
#include "DnSkillTask.h"
#include "DnStateBlow.h"
#ifndef _GAMESERVER
#include "DnCantUseActiveSkillBlow.h"
#include "DnWorld.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#else
#include "DNUserSession.h"
#include "DnApplySEWhenTargetNormalHitProcessor.h"
#endif

#if defined(_GAMESERVER)
#include "DnProbabilityChecker.h"
#endif

#include "DnCreateBlow.h"

#include "SmartPtrDef.h"

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#include "TotalLevelSkillSystem.h"
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MASkillUser::MASkillUser()
//: m_vSkillTargetPos( 0.f, 0.f, 0.f )
{
	m_LocalTime = 0;

	m_pActor = NULL;
	m_bIsValidActor = true;

	//m_fAdjustCoolTime = 1.0f;
#ifdef _GAMESERVER
	m_bIgnoreCoolTime = false;
#else
	m_bSkillExecutedThisFrame = false;
#endif

	m_fCoolTimeDeltaAdjustValue = 1.0f;

	//ZeroMemory( m_aEternityItemApplyStat, sizeof(m_aEternityItemApplyStat) );
}

MASkillUser::~MASkillUser()
{
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) {
		if( m_vbSelfAllocList[i] == true ) SAFE_RELEASE_SPTR( m_vlhSkillList[i] );
	}
	SAFE_DELETE_VEC( m_vlhSkillList );
	SAFE_DELETE_VEC( m_vbSelfAllocList );
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		S_SKILL_OBJECT& SkillObject = m_vlSkillObjects.at( i );
		if( true == SkillObject.bSelfAlloc )
			SAFE_RELEASE_SPTR( SkillObject.hSkill );
	}
	SAFE_DELETE_VEC( m_vlSkillObjects );
#endif // #ifndef PRE_FIX_SKILLLIST

#if defined(_GAMESERVER)
	m_PrefixSkillCoolTimeManager.InitList();

	//방어용 스킬 초기화
	PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemDefenceSkills.begin();
	PREFIX_SYSTEM_SKILL_LIST::iterator endIter = m_prefixSystemDefenceSkills.end();
	for (; iter != endIter; ++iter)
	{
		if (iter->second.hSkill)
			SAFE_RELEASE_SPTR(iter->second.hSkill);
	}
	SAFE_DELETE_MAP(m_prefixSystemDefenceSkills);

	//공격용 스킬 초기화
	iter = m_prefixSystemOffenceSkills.begin();
	endIter = m_prefixSystemOffenceSkills.end();
	for (; iter != endIter; ++iter)
	{
		if (iter->second.hSkill)
			SAFE_RELEASE_SPTR(iter->second.hSkill);
	}
	SAFE_DELETE_MAP(m_prefixSystemOffenceSkills);
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
#if defined(_GAMESERVER)
	{
		//접두사 방어용 스킬
		PREFIX_SKILL_INFO::iterator iter = m_PrefixDefenceSkills.begin();
		PREFIX_SKILL_INFO::iterator endIter = m_PrefixDefenceSkills.end();
		for (; iter != endIter; ++iter)
		{
			if(iter->second)
				SAFE_DELETE(iter->second);
		}
		m_PrefixDefenceSkills.clear();

		//접두사 공격용 스킬
		iter = m_PrefixOffenceSkills.begin();
		endIter = m_PrefixOffenceSkills.end();
		for (; iter != endIter; ++iter)
		{
			if(iter->second)
				SAFE_DELETE(iter->second);
		}
		m_PrefixOffenceSkills.clear();

		//사용중인 접두사 공격용 스킬 리스트
		m_ProcessPrefixOffenceSkills.clear();
	}
#endif // _GAMESERVER
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
}

bool MASkillUser::IsValidActor()
{
	if( m_pActor ) return true;
	else {
		if( !m_bIsValidActor ) return false;
		m_pActor = dynamic_cast<CDnActor*>(this);
		if( !m_pActor ) m_bIsValidActor = false;
	}
	return true;
}

void MASkillUser::OnAddSkill( DnSkillHandle hSkill, bool isInitialize/* = false*/ )
{
	// 패시브 이면서 버프고 상태효과 Self 로 붙어있는 스킬은 곧바로 적용시켜 줌..
	if( !IsValidActor() ) return;
	bool bPassiveBuff = false;

	bPassiveBuff = ApplyPassiveSkill( hSkill, isInitialize );
#ifdef _GAMESERVER
	if( CDnSkill::AutoPassive == hSkill->GetSkillType() )
		m_listAutoPassiveSkills.push_back( hSkill );
#else
	const set<string>& setUseAction = hSkill->GetUseActionSet();
	set<string>::const_iterator iter = setUseAction.begin();
	for( iter; iter != setUseAction.end(); ++iter )
		m_setUseActionNames.insert( *iter );
#endif

	if( bPassiveBuff )
		m_vlhSelfPassiveBlowSkill.push_back( hSkill );
}

bool MASkillUser::AddSkill( int nSkillTableID, int nLevel/* = 1*/, int iSkillLevelApplyType/* = CDnSkill::PVE*/ )
{
	if( IsExistSkill( nSkillTableID, nLevel ) )
		return false;

	if( !IsValidActor() ) 
		return false;

#ifdef _GAMESERVER
	if( m_pActor == NULL || m_pActor->GetRoom() == NULL ) 
		return false;
#endif

	//레벨업 정보가 있으면 추가 한다.
	int nLevelUp = GetSkillLevelUpValue(nSkillTableID);
	nLevel += nLevelUp;

	DnSkillHandle hSkill = CDnSkill::CreateSkill( m_pActor->GetMySmartPtr(), nSkillTableID, nLevel );

	//레벨업을 시도 했는데 스킬 생성이 안됐다면
	if (nLevelUp != 0 && !hSkill)
	{
		OutputDebug("스킬 (%d) 레벨 (%d) 생성 실패.. 스킬 레벨 테이블 확인 요망!!!!!!\n", nSkillTableID, nLevel);
		_ASSERT( hSkill && "스킬 레벨업 시도 실패.. 레벨 테이블 확인" );

		//다시 원래 레벨로 생성 시도..
		nLevel -= nLevelUp;
		hSkill = CDnSkill::CreateSkill(m_pActor->GetMySmartPtr(), nSkillTableID, nLevel);

		nLevelUp = 0;
	}

	if( !hSkill ) 
		return false;

	//추가된 레벨업 정보를 설정 한다.
	hSkill->SetLevelUpValue(nLevelUp);

	// 스킬레벨 테이블 적용타입 셋팅
	if( m_pActor->IsPlayerActor() )
		hSkill->SelectLevelDataType( iSkillLevelApplyType );

#ifndef PRE_FIX_SKILLLIST
	m_vlhSkillList.push_back( hSkill );
	m_vbSelfAllocList.push_back( true );
#else
	m_vlSkillObjects.push_back( S_SKILL_OBJECT(hSkill, true) );
#endif // #ifndef PRE_FIX_SKILLLIST

	// 토글, 오라 스킬은 따로 리스트에 담아둠
	CDnSkill::DurationTypeEnum eDurationType = hSkill->GetDurationType();
	switch(eDurationType)
	{
	case CDnSkill::ActiveToggle:
	case CDnSkill::TimeToggle:
	case CDnSkill::ActiveToggleForSummon:
		m_vlhToggleSkills.push_back(hSkill);
	case CDnSkill::Aura:
		m_vlhAuraSkills.push_back(hSkill);
	}

	if( hSkill->GetActor() )
		hSkill->SetHasActor( m_pActor->GetMySmartPtr() );

	OnAddSkill( hSkill );
	return true;
}

#if defined(PRE_FIX_62052)
void MASkillUser::ApplyGlobalSkillCoolTime(DnSkillHandle hSkill)
{
	int nGlobalSkillGroupID = hSkill ? hSkill->GetGlobalSkillGroupID() : 0;
	//GlobalSkillGroupID가 설정된 스킬이 추가 될때 기존에 같은 ID를 사용하느 스킬의 쿨타임을 추가 되는 스킬에도 설정 해준다.

	if (nGlobalSkillGroupID != 0)
	{
		for( DWORD i = 0; i < GetSkillCount(); ++i )
		{
			// m_vlGroupedSkillIDs 의 값들은 테이블에서 그냥 끄집어 낸 것이기 때문에
			// 실제 스킬을 습득했는지 여부가 확인되어야 한다.
			DnSkillHandle hExistSkill = GetSkillFromIndex( i );
			float fElapsedDelayTime = hExistSkill->GetElapsedDelayTime();

			if( hExistSkill && 
				hExistSkill->GetGlobalSkillGroupID() == nGlobalSkillGroupID &&
				fElapsedDelayTime > 0.0f)
			{
				DnSkillHandle hOrigSkill = FindSkill(hExistSkill->GetAnotherGlobalSkillID());

				hSkill->OnAnotherGlobalSkillBeginCoolTime( hOrigSkill );
				hSkill->SetElapsedDelayTime(fElapsedDelayTime);
			}
		}
	}
}
#endif // PRE_FIX_62052

bool MASkillUser::AddSkill( DnSkillHandle hSkill )
{
#ifdef _GAMESERVER
	// 겜서버에서는 몬스터에서만 스킬 객체로 직접 addskill 합니다.
	// 플레이어 액터에게는 스킬 트리 관리가 중간에 들어가기 때문에 서버에서 가상함수로 
	// playeractor 가 오버라이드하여 사용하지 않는 이 함수 사용하면 안됩니다.
	// 추후에 사용하게 된다면 기억하기 위해서 assert 걸어놓습니다.
	//_ASSERT( IsValidActor() );
	//_ASSERT( m_pActor->IsMonsterActor() );
#endif

	if( IsExistSkill( hSkill->GetClassID(), hSkill->GetLevel() ) ) return false;

#if defined(PRE_FIX_62052)
	ApplyGlobalSkillCoolTime(hSkill);
#endif // PRE_FIX_62052

#ifndef PRE_FIX_SKILLLIST
	m_vlhSkillList.push_back( hSkill );
	m_vbSelfAllocList.push_back( false );
#else
	m_vlSkillObjects.push_back( S_SKILL_OBJECT(hSkill, false) );
#endif // #ifndef PRE_FIX_SKILLLIST

	// 토글, 오라 스킬은 따로 리스트에 담아둠
	CDnSkill::DurationTypeEnum eDurationType = hSkill->GetDurationType();
	switch(eDurationType)
	{
	case CDnSkill::ActiveToggle:
	case CDnSkill::TimeToggle:
	case CDnSkill::ActiveToggleForSummon:
		m_vlhToggleSkills.push_back(hSkill);
	case CDnSkill::Aura:
		m_vlhAuraSkills.push_back(hSkill);
	}

	OnAddSkill( hSkill );

	return true;
}

bool MASkillUser::bIsPassiveSkill( DnBlowHandle hBlow )
{
	int iNumSkill = (int)m_vlhSelfPassiveBlowSkill.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at( iSkill );
		_ASSERT( CDnSkill::Passive == hSkill->GetSkillType() );
		const CDnSkill::SkillInfo* pSkillInfo = hBlow->GetParentSkillInfo();
		if( pSkillInfo && (pSkillInfo->iSkillID == hSkill->GetClassID()) )
			return true;

		//if( CDnSkill::Passive == hSkill->GetSkillType() )
		//{
		//	int iNumStateEffect = hSkill->GetStateEffectCount();
		//	for( int i = 0; i < iNumStateEffect; ++i )
		//	{
		//		CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
		//		if( CDnSkill::StateEffectApplyType::ApplySelf == pSE->ApplyType )
		//		{
		//			if( CDnSkill::DurationTypeEnum::Buff == hSkill->GetDurationType() )
		//			{
		//				if( hBlow->GetBlowIndex() == pSE->nID )
		//					return true;
		//			}
		//		}
		//	}
		//}
	}

	return false;
}

bool MASkillUser::ApplyPassiveSkill( DnSkillHandle hSkill, bool isInitialize/* = false*/ )
{
	bool bPassiveBuf = false;

	// #24143 자기자신의 스킬들을 초기화할땐 CDnSkillTask 쪽에서 실제 획득한 스킬리스트를 기준으로 AddSkill() 함수를 
	// 호출해주기 때문에 문제 없지만 다른 플레이어의 경우엔 그냥 서버에서 밀어주는 스킬리스트(리셋된 레벨 0짜리 스킬 포함)
	// 그대로 AddSkill() 함수를 호출해주기 때문에 리셋된 패시브 스킬도 적용시켜줘버리는 문제가 있다. 
	// 따라서 여기서 획득 상태가 아니면 패시브 버프 스킬을 발동시켜주지 않도록 한다. (hp/mp 최대치 증가 스킬등등..)
	// 게임서버쪽에서는 클라이언트와 다르게 각 CDnPlayerActor 객체들이 실제 보유한 스킬만 갖고 있다.
	// 클라이언트쪽에서는 LocalPlayerActor 인 경우에 리셋된 스킬의 정보도 필요하므로 PlayerActor 를 따로 구분 안하고 스킬리스트 모두 넣어준다..
#ifndef _GAMESERVER
	if( false == hSkill->IsAcquired() )
		return false;
#endif // #ifndef _GAMESERVER

	if( CDnSkill::Passive == hSkill->GetSkillType() )
	{

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
		bool isTotalLevelSkill = false;
		if (m_pActor && m_pActor->IsPlayerActor())
		{
			CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_pActor);
			CDnTotalLevelSkillSystem* pTotalLevelSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem() : NULL;
			
			if (pTotalLevelSystem)
				isTotalLevelSkill = pTotalLevelSystem->IsTotalLevelSkill(hSkill->GetClassID());
		}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

		int iNumStateEffect = hSkill->GetStateEffectCount();
		for( int i = 0; i < iNumStateEffect; ++i )
		{
			CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( i );
			if( CDnSkill::StateEffectApplyType::ApplySelf == pSE->ApplyType )
			{
				if( CDnSkill::DurationTypeEnum::Buff == hSkill->GetDurationType() )
				{
					bPassiveBuf = true;
					if( hSkill->IsSatisfyWeapon() )
					{
						int iBlowID = -1;
						DnBlowHandle hBlow;

#ifdef _GAMESERVER
						if(m_pActor)
						{
							iBlowID = m_pActor->CDnActor::CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pSE->nID, -1, pSE->szValue.c_str(), true ); // Duration Time이 -1 이면 무한 적용임
							OnApplyPassiveSkillBlow( iBlowID );

							hBlow = m_pActor->GetStateBlowFromID(iBlowID);
						}
#else
						if(m_pActor)
							hBlow = m_pActor->CDnActor::CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pSE->nID, -1, pSE->szValue.c_str(), true ); // Duration Time이 -1 이면 무한 적용임
#endif

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
						if (isTotalLevelSkill == true && isInitialize == true && hBlow)
							hBlow->FromSourceItem();
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

						hSkill->SetAppliedPassiveBlows( true );
						
#ifndef _FINAL_BUILD
						char szTemp[256] = { 0, };
						WideCharToMultiByte( CP_ACP, 0, hSkill->GetName(), -1, szTemp, _countof(szTemp), NULL, NULL );
						OutputDebug( "[패시브 버프 스킬 적용됨: %d] \"%s\" 상태효과Index: %d, Value:%s\n", hSkill->GetClassID(), szTemp, pSE->nID, pSE->szValue.c_str() );
#endif // #ifndef _FINAL_BUILD
					}
#ifndef _FINAL_BUILD
					else
					{
						char szTemp[256] = { 0, };
						WideCharToMultiByte( CP_ACP, 0, hSkill->GetName(), -1, szTemp, _countof(szTemp), NULL, NULL );
						OutputDebug( "[패시브 버프 스킬 적용안됨: %d] \"%s\" 스킬에 지정된 필요 무기 타입이 맞지 않습니다.\n", hSkill->GetClassID(), szTemp );
					}
#endif // #ifndef _FINAL_BUILD
				}
#ifndef _FINAL_BUILD
				else
				{
					char szTemp[256] = { 0, };
					WideCharToMultiByte( CP_ACP, 0, hSkill->GetName(), -1, szTemp, _countof(szTemp), NULL, NULL );
					OutputDebug( "[스킬 데이터 확인 요망: %d] \"%s\" 패시브 버프 스킬로 생각되나 Buff 로 설정되어있지 않아서 적용 안됨\n", hSkill->GetClassID(), szTemp );
				}
#endif // #ifndef _FINAL_BUILD
			}
		}
	}

	return bPassiveBuf;
}
 
void MASkillUser::ApplyPassiveSkills( void )
{
	if( !IsValidActor() ) return;

	int iNumSkill = (int)m_vlhSelfPassiveBlowSkill.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at( iSkill );
		ApplyPassiveSkill(hSkill);
	}
}

void MASkillUser::OnRemoveSkill( DnSkillHandle hSkill )
{
#ifndef _GAMESERVER
	const set<string>& setUseAction = hSkill->GetUseActionSet();
	set<string>::const_iterator iter = setUseAction.begin();
	for( iter; iter != setUseAction.end(); ++iter )
		m_setUseActionNames.erase( *iter );
#endif
}

bool MASkillUser::RemoveSkillAll()
{
	if( !IsValidActor() )
		return false;

#ifndef PRE_FIX_SKILLLIST
	int nSkillSize = (int)m_vlhSkillList.size();

	for( int i=0 ; i<nSkillSize ; i++ )
	{
		DnSkillHandle hSkill = m_vlhSkillList.back();
		if( !hSkill )
			continue;

		bool bRet = RemoveSkill( hSkill->GetClassID() );
		_ASSERT( bRet );
	}
#else

	int nSkillSize = (int)m_vlSkillObjects.size();

	for( int i=0 ; i<nSkillSize ; i++ )
	{
		S_SKILL_OBJECT& SkillObject = m_vlSkillObjects.back();
		DnSkillHandle hSkill = SkillObject.hSkill;
		if( !hSkill )
			continue;

		bool bRet = RemoveSkill( hSkill->GetClassID() );
		_ASSERT( bRet );
	}
#endif // #ifndef PRE_FIX_SKILLLIST

	return true;
}

bool MASkillUser::EndStateEffectSkill(int nSkillTableID)
{ 
	DNVector(DnBlowHandle) vlBlows;
	m_pActor->GetAllAppliedStateBlow(vlBlows);

	for (int i = 0; i < (int)vlBlows.size(); ++i)
	{
		DnBlowHandle hBlow = vlBlows.at(i);
		const CDnSkill::SkillInfo * skillInfo = hBlow->GetParentSkillInfo();
		if (skillInfo->iSkillID == nSkillTableID)
		{
			hBlow->OnEnd(0, 0);
			m_pActor->OnEndStateBlow(hBlow);
			m_pActor->RemoveStateBlowFromID(hBlow->GetBlowID()); //? need test.
		}
	}
/*
	DnSkillHandle hSkill = FindSkill(nSkillTableID);
	if (m_pActor && m_pActor->GetStateBlow())
	{
		std::vector<int> vecRemoveBlowID;
		DNVector(DnBlowHandle) vlBlows;
		m_pActor->GetAllAppliedStateBlowBySkillID(nSkillTableID, vlBlows);

		int iStateEffectCount = hSkill->GetStateEffectCount();
		for (int iStateEffect = 0; iStateEffect < iStateEffectCount; ++iStateEffect)
		{
			const CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex(iStateEffect);
			if (pSE && CDnSkill::StateEffectApplyType::ApplySelf == pSE->ApplyType)
			{
				for (DWORD BLOW_COUNT = 0; BLOW_COUNT < vlBlows.size(); BLOW_COUNT++)
				{
					if (vlBlows[BLOW_COUNT] && (vlBlows[BLOW_COUNT]->GetBlowIndex() == (STATE_BLOW::emBLOW_INDEX)pSE->nID))
					{
							vecRemoveBlowID.push_back(vlBlows[BLOW_COUNT]->GetBlowID());
					}
				}
			}
		}

		for (DWORD Index = 0; Index < vecRemoveBlowID.size(); Index++)
		{
			m_pActor->GetStateBlow()->RemoveImediatlyStateEffectFromID(vecRemoveBlowID[Index]);
		}
	}
*/
	return true;
}

bool MASkillUser::RemoveSkill( int nSkillTableID )
{
	if( !IsValidActor() ) return false;

	int iNumPassiveBlowSkill = (int)m_vlhSelfPassiveBlowSkill.size();
	for( int iPassiveBlowSkill = 0; iPassiveBlowSkill < iNumPassiveBlowSkill; ++iPassiveBlowSkill )
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at( iPassiveBlowSkill );
		if( hSkill && hSkill->GetClassID() == nSkillTableID )
		{
			if( m_pActor && m_pActor->GetStateBlow() )
			{
				std::vector<int> vecRemoveBlowID;
				DNVector(DnBlowHandle) vlBlows;
				m_pActor->GetAllAppliedStateBlowBySkillID( nSkillTableID , vlBlows );

				int iStateEffectCount = hSkill->GetStateEffectCount();
				for( int iStateEffect = 0; iStateEffect < iStateEffectCount; ++iStateEffect )
				{
					const CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( iStateEffect );
					if( pSE && CDnSkill::StateEffectApplyType::ApplySelf == pSE->ApplyType )
					{
						for( DWORD BLOW_COUNT=0; BLOW_COUNT < vlBlows.size(); BLOW_COUNT++ )
						{
							if( vlBlows[BLOW_COUNT] && ( vlBlows[BLOW_COUNT]->GetBlowIndex() == (STATE_BLOW::emBLOW_INDEX)pSE->nID ) )
							{
								vecRemoveBlowID.push_back( vlBlows[BLOW_COUNT]->GetBlowID() );
							}
						}
					}
				}		

				for( DWORD Index=0; Index<vecRemoveBlowID.size(); Index++)
				{
					m_pActor->GetStateBlow()->RemoveImediatlyStateEffectFromID( vecRemoveBlowID[Index] );
				}
			}

			// 2009.03.16 김밥
			m_vlhSelfPassiveBlowSkill.erase( m_vlhSelfPassiveBlowSkill.begin()+iPassiveBlowSkill );
			break;
		}
	}

#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	//통합레벨 스킬인 경우 아랫쪽 루틴을 탈 필요가 없음...
	if (m_pActor && m_pActor->IsPlayerActor())
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(m_pActor);
		CDnTotalLevelSkillSystem *pTotalLevelSystem = pPlayerActor ? pPlayerActor->GetTotalLevelSkillSystem(): NULL;

		if (pTotalLevelSystem && pTotalLevelSystem->IsTotalLevelSkill(nSkillTableID))
			return true;
	}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

#ifdef _GAMESERVER
	list<DnSkillHandle>::iterator iter = m_listAutoPassiveSkills.begin();
	for( iter; m_listAutoPassiveSkills.end() != iter; ++iter )
	{
		if( (*iter)->GetClassID() == nSkillTableID )
		{
			m_listAutoPassiveSkills.erase( iter );
			break;
		}
	}
#endif

#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) {
		if( m_vlhSkillList[i]->GetClassID() == nSkillTableID ) {
			
			// 토글이나 오라 스킬이라면 별도의 리스트에서 삭제
			DnSkillHandle hSkill = m_vlhSkillList.at(i);
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		S_SKILL_OBJECT& SkillObject = m_vlSkillObjects.at( i );
		if( SkillObject.hSkill && SkillObject.hSkill->GetClassID() ==  nSkillTableID )
		{
			// 토글이나 오라 스킬이라면 별도의 리스트에서 삭제
			DnSkillHandle hSkill = SkillObject.hSkill;
#endif // #ifndef PRE_FIX_SKILLLIST
			if( CDnSkill::ActiveToggle == hSkill->GetDurationType() ||
				CDnSkill::TimeToggle == hSkill->GetDurationType() 	|| 
				CDnSkill::ActiveToggleForSummon == hSkill->GetDurationType()
				)
			{
				DNVector(DnSkillHandle)::iterator Localiter = find( m_vlhToggleSkills.begin(), m_vlhToggleSkills.end(), hSkill );
				m_vlhToggleSkills.erase( Localiter );
			}
			else
			if( CDnSkill::Aura == hSkill->GetDurationType() )
			{
				if (m_hAuraSkill && m_hAuraSkill->GetClassID() == nSkillTableID && m_hAuraSkill->IsAuraOn())
				{
					if (m_hAuraSkill == m_hProcessSkill)
						m_hProcessSkill.Identity();

					m_hAuraSkill->EnableAura( false );
					OnSkillAura(m_hAuraSkill, false);
					m_hAuraSkill.Identity();
				}

				DNVector(DnSkillHandle)::iterator Localiter = find( m_vlhAuraSkills.begin(), m_vlhAuraSkills.end(), hSkill );
				m_vlhAuraSkills.erase( Localiter );
			}

#ifndef PRE_FIX_SKILLLIST
			OnRemoveSkill( m_vlhSkillList[i] );
			if( m_vbSelfAllocList[i] == true ) SAFE_RELEASE_SPTR( m_vlhSkillList[i] );

			m_vlhSkillList.erase( m_vlhSkillList.begin() + i );
			m_vbSelfAllocList.erase( m_vbSelfAllocList.begin() + i );
#else
			OnRemoveSkill( SkillObject.hSkill );
			if( SkillObject.bSelfAlloc )
				SAFE_RELEASE_SPTR( SkillObject.hSkill );
			m_vlSkillObjects.erase( m_vlSkillObjects.begin()+i );
#endif // #ifndef PRE_FIX_SKILLLIST
			return true;
		}
	}
	return true;
}

#ifndef _GAMESERVER
bool MASkillUser::ReplacementSkill( int nSkillTableID, int nLevel )
{
	if( !IsValidActor() ) return false;

	DnSkillHandle hSkill = CDnSkill::CreateSkill( m_pActor->GetMySmartPtr(), nSkillTableID, nLevel );
	if( !hSkill ) return false;
	
	return ReplacementSkill( hSkill );
}


bool MASkillUser::ReplacementSkill( DnSkillHandle hNewSkill )
{
#ifndef PRE_FIX_SKILLLIST
	int iSkillID = hNewSkill->GetClassID();
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) 
	{
		if( m_vlhSkillList[ i ]->GetClassID() == iSkillID ) 
		{
			// 패시브 블로우 스킬인가. 그렇다면 기존 상태효과 모두 없애고 새로 적용시켜 준다.
			DNVector(DnSkillHandle)::iterator iter = find( m_vlhSelfPassiveBlowSkill.begin(), m_vlhSelfPassiveBlowSkill.end(), m_vlhSkillList[ i ] );
			if( m_vlhSelfPassiveBlowSkill.end() != iter )
			{
				// 적용되어 있는 상태효과를 제거한다.
				// 부모스킬이 일치하는 상태효과를 찾아내서 삭제한다.
				if( !IsValidActor() ) continue;

				// HP, SP 올려주는 패시브 스킬은 삭제되면 원래 스킬 없는 상태의 순수 HP SP 로 현재 HP/SP가
				// 클리핑 되게 때문에 미리 받아뒀다가 셋팅해준다..
				INT64 iHP = m_pActor->GetHP();
				int iSP = m_pActor->GetSP();

				DNVector(DnBlowHandle) vlhAppliedSE;
				DNVector(DnBlowHandle) vlhSEToRemove;
				m_pActor->GetAllAppliedStateBlow( vlhAppliedSE );
				int iNumAppliedSE = (int)vlhAppliedSE.size();
				for( int iSE = 0; iSE < iNumAppliedSE; ++iSE )
				{
					DnBlowHandle hAppliedBlow = vlhAppliedSE.at( iSE );
					const CDnSkill::SkillInfo* pSkillInfo = hAppliedBlow->GetParentSkillInfo();
					if( pSkillInfo->iSkillID == (*iter)->GetClassID() )
						vlhSEToRemove.push_back( hAppliedBlow );
				}

				int iNumSEToRemove = (int)vlhSEToRemove.size();
				for( int iSE = 0; iSE < iNumSEToRemove; ++iSE )
				{
					DnBlowHandle hBlow = vlhSEToRemove.at(iSE);
					int iID = hBlow->GetBlowID();
					OutputDebug( "[패시브 버프 스킬 삭제됨: %d] %s 상태효과Index: %d, Value:%s\n", 
								 (*iter)->GetClassID(), (*iter)->GetName(), hBlow->GetBlowIndex(), hBlow->GetValue() );
					m_pActor->RemoveStateBlowFromID( iID );
				}

				m_vlhSelfPassiveBlowSkill.erase( iter );

				ApplyPassiveSkill( hNewSkill );
				m_vlhSelfPassiveBlowSkill.push_back( hNewSkill );

				m_pActor->SetHP( iHP );
				m_pActor->SetSP( iSP );
			}

			if (m_hAuraSkill && m_hAuraSkill->GetClassID() == iSkillID && m_hAuraSkill->IsAuraOn())
			{
				if (m_hAuraSkill == m_hProcessSkill)
					m_hProcessSkill.Identity();

				m_hAuraSkill->EnableAura( false );
				OnSkillAura(m_hAuraSkill, false);
				m_hAuraSkill.Identity();
			}

			if( m_vbSelfAllocList[ i ] )
			{
				// 자신이 생성한 스킬은 자신이 지워야 하므로.
				DnSkillHandle hSkillToRelease = m_vlhSkillList[ i ];

				// 오라, 토글 스킬 리스트에서도 검색해서 대체 시킴
				DNVector(DnSkillHandle)::iterator iterToggle = find( m_vlhToggleSkills.begin(), m_vlhToggleSkills.end(), hSkillToRelease );
				if( m_vlhToggleSkills.end() != iterToggle )
					*iterToggle = hNewSkill;

				DNVector(DnSkillHandle)::iterator iterAura = find( m_vlhAuraSkills.begin(), m_vlhAuraSkills.end(), hSkillToRelease );
				if( m_vlhAuraSkills.end() != iterAura )
					*iterAura = hNewSkill;

				OnRemoveSkill( hSkillToRelease );
				SAFE_RELEASE_SPTR( hSkillToRelease );

			}
			else
			{
				// 스킬 태스크에서 생성한 스킬 객체는 여기서 지우지 않는다.
				DnSkillHandle hPrevLevelSkill = m_vlhSkillList[ i ];

				// 오라, 토글 스킬 리스트에서도 검색해서 대체 시킴
				DNVector(DnSkillHandle)::iterator iterToggle = find( m_vlhToggleSkills.begin(), m_vlhToggleSkills.end(), hPrevLevelSkill );
				if( m_vlhToggleSkills.end() != iterToggle )
					*iterToggle = hNewSkill;

				DNVector(DnSkillHandle)::iterator iterAura = find( m_vlhAuraSkills.begin(), m_vlhAuraSkills.end(), hPrevLevelSkill );
				if( m_vlhAuraSkills.end() != iterAura )
					*iterAura = hNewSkill;
			}

			OnReplacementSkill( m_vlhSkillList[ i ], hNewSkill );
			m_vlhSkillList[ i ] = hNewSkill;

			return true;
		}
	}

	return false;
#else
	int iSkillID = hNewSkill->GetClassID();
	for( DWORD i = 0; i < m_vlSkillObjects.size(); i++ ) 
	{
		S_SKILL_OBJECT& SkillObject = m_vlSkillObjects.at( i );
		DnSkillHandle hSkill = SkillObject.hSkill;
		if( hSkill->GetClassID() == iSkillID ) 
		{
			// 패시브 블로우 스킬인가. 그렇다면 기존 상태효과 모두 없애고 새로 적용시켜 준다.
			DNVector(DnSkillHandle)::iterator iter = find( m_vlhSelfPassiveBlowSkill.begin(), m_vlhSelfPassiveBlowSkill.end(), hSkill );
			if( m_vlhSelfPassiveBlowSkill.end() != iter )
			{
				// 적용되어 있는 상태효과를 제거한다.
				// 부모스킬이 일치하는 상태효과를 찾아내서 삭제한다.
				if( !IsValidActor() ) continue;

				// HP, SP 올려주는 패시브 스킬은 삭제되면 원래 스킬 없는 상태의 순수 HP SP 로 현재 HP/SP가
				// 클리핑 되게 때문에 미리 받아뒀다가 셋팅해준다..
				INT64 iHP = m_pActor->GetHP();
				int iSP = m_pActor->GetSP();

				DNVector(DnBlowHandle) vlhAppliedSE;
				DNVector(DnBlowHandle) vlhSEToRemove;
				m_pActor->GetAllAppliedStateBlow( vlhAppliedSE );
				int iNumAppliedSE = (int)vlhAppliedSE.size();
				for( int iSE = 0; iSE < iNumAppliedSE; ++iSE )
				{
					DnBlowHandle hAppliedBlow = vlhAppliedSE.at( iSE );
					const CDnSkill::SkillInfo* pSkillInfo = hAppliedBlow->GetParentSkillInfo();
					if( pSkillInfo->iSkillID == (*iter)->GetClassID() )
						vlhSEToRemove.push_back( hAppliedBlow );
				}

				int iNumSEToRemove = (int)vlhSEToRemove.size();
				for( int iSE = 0; iSE < iNumSEToRemove; ++iSE )
				{
					DnBlowHandle hBlow = vlhSEToRemove.at(iSE);
					int iID = hBlow->GetBlowID();
					OutputDebug( "[패시브 버프 스킬 삭제됨: %d] %s 상태효과Index: %d, Value:%s\n", 
								 (*iter)->GetClassID(), (*iter)->GetName(), hBlow->GetBlowIndex(), hBlow->GetValue() );
					m_pActor->RemoveStateBlowFromID( iID );
				}

				m_vlhSelfPassiveBlowSkill.erase( iter );

				ApplyPassiveSkill( hNewSkill );
				m_vlhSelfPassiveBlowSkill.push_back( hNewSkill );

				m_pActor->SetHP( iHP );
				m_pActor->SetSP( iSP );
			}

			if (m_hAuraSkill && m_hAuraSkill->GetClassID() == iSkillID && m_hAuraSkill->IsAuraOn())
			{
				if (m_hAuraSkill == m_hProcessSkill)
					m_hProcessSkill.Identity();

				m_hAuraSkill->EnableAura( false );
				OnSkillAura(m_hAuraSkill, false);
				m_hAuraSkill.Identity();
			}

			if( true == SkillObject.bSelfAlloc )
			{
				// 자신이 생성한 스킬은 자신이 지워야 하므로.
				DnSkillHandle hSkillToRelease = hSkill;

				// 오라, 토글 스킬 리스트에서도 검색해서 대체 시킴
				DNVector(DnSkillHandle)::iterator iterToggle = find( m_vlhToggleSkills.begin(), m_vlhToggleSkills.end(), hSkillToRelease );
				if( m_vlhToggleSkills.end() != iterToggle )
					*iterToggle = hNewSkill;

				DNVector(DnSkillHandle)::iterator iterAura = find( m_vlhAuraSkills.begin(), m_vlhAuraSkills.end(), hSkillToRelease );
				if( m_vlhAuraSkills.end() != iterAura )
					*iterAura = hNewSkill;

				OnRemoveSkill( hSkillToRelease );
				SAFE_RELEASE_SPTR( hSkillToRelease );

			}
			else
			{
				// 스킬 태스크에서 생성한 스킬 객체는 여기서 지우지 않는다.
				DnSkillHandle hPrevLevelSkill = hSkill;

				// 오라, 토글 스킬 리스트에서도 검색해서 대체 시킴
				DNVector(DnSkillHandle)::iterator iterToggle = find( m_vlhToggleSkills.begin(), m_vlhToggleSkills.end(), hPrevLevelSkill );
				if( m_vlhToggleSkills.end() != iterToggle )
					*iterToggle = hNewSkill;

				DNVector(DnSkillHandle)::iterator iterAura = find( m_vlhAuraSkills.begin(), m_vlhAuraSkills.end(), hPrevLevelSkill );
				if( m_vlhAuraSkills.end() != iterAura )
					*iterAura = hNewSkill;
			}

			OnReplacementSkill( hSkill, hNewSkill );
			SkillObject.hSkill = hNewSkill;

			return true;
		}
	}

	return false;
#endif // #ifndef PRE_FIX_SKILLLIST
}
#endif

bool MASkillUser::IsExistSkill( int nSkillTableID, int nLevel )
{
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) {
		if( m_vlhSkillList[i] && ( m_vlhSkillList[i]->GetClassID() == nSkillTableID ) ) 
		{
			if( nLevel == -1 ) return true;
			if( m_vlhSkillList[i]->GetLevel() == nLevel ) return true;
		}
	}
	return false;
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		S_SKILL_OBJECT& SkillObject = m_vlSkillObjects.at( i );
		DnSkillHandle hSkill = SkillObject.hSkill;
		if( hSkill)
		{
			if( hSkill->GetClassID() == nSkillTableID )
			{
				if( -1 == nLevel )
					return true;

				if( hSkill->GetLevel() == nLevel )
					return true;
			}
		}
	}

	return false;
#endif // #ifndef PRE_FIX_SKILLLIST
}

bool MASkillUser::IsSelfAllocSkill( int nSkillTableID )
{
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) {
		if( m_vlhSkillList[i]->GetClassID() == nSkillTableID ) 
			return m_vbSelfAllocList[i];
	}
	return false;
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		if( m_vlSkillObjects.at( i ).hSkill->GetClassID() == nSkillTableID )
			return m_vlSkillObjects.at( i ).bSelfAlloc;
	}
	return false;
#endif // #ifndef PRE_FIX_SKILLLIST
}

void MASkillUser::SetSkillLevel( int nSkillTableID, int nValue )
{
	if( !IsSelfAllocSkill( nSkillTableID ) ) return;

	DnSkillHandle hSkill = FindSkill( nSkillTableID );
	if( !hSkill ) return;
	if( hSkill->GetLevel() == nValue ) return;

	float fElapsedDelayTime = hSkill->GetElapsedDelayTime();
	RemoveSkill( nSkillTableID );
	AddSkill( nSkillTableID, nValue );
	hSkill = FindSkill( nSkillTableID );
	hSkill->SetElapsedDelayTime( fElapsedDelayTime );
}

DnSkillHandle MASkillUser::FindSkill( int nSkillTableID )
{
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i=0; i<m_vlhSkillList.size(); i++ ) 
	{
		if( m_vlhSkillList[i]->GetClassID() == nSkillTableID ) 
			return m_vlhSkillList[i];
	}
	return CDnSkill::Identity();
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		if( m_vlSkillObjects.at( i ).hSkill &&
			m_vlSkillObjects.at( i ).hSkill->GetClassID() == nSkillTableID )
			return m_vlSkillObjects.at( i ).hSkill;
	}

	return CDnSkill::Identity();
#endif // #ifndef PRE_FIX_SKILLLIST
}

DnSkillHandle MASkillUser::GetProcessSkill()
{
	return m_hProcessSkill;
}


CDnSkill::UsingResult MASkillUser::CanExecuteSkill( DnSkillHandle hSkill )
{
	_ASSERT( hSkill && "MASkillUser::CanExecuteSkill() -> Invalid hSkill" );

#ifndef _GAMESERVER
	// 같은 프레임에 액티브 스킬 여러개 사용 불가.
	if( m_hProcessSkill && 
		m_hProcessSkill->GetSkillType() == CDnSkill::Active && 
		hSkill->GetSkillType() == CDnSkill::Active )
		if( m_bSkillExecutedThisFrame )
			return CDnSkill::UsingResult::Failed;
#endif


// [2011/02/22 semozz]
// 애니메이션 전환 가능 체크는 클라이언트만 하면 될듯...
#if !defined(_GAMESERVER)
	if (!hSkill->CheckAnimation())
		return CDnSkill::UsingResult::Failed;
#endif // _GAMESERVER

#if !defined(_GAMESERVER)
	if( hSkill->GetGlyphActiveSkillDisable() )
		return CDnSkill::UsingResult::Failed;
#endif	// _GAMESERVER

	return hSkill->CanExecute();
}


#ifdef _GAMESERVER
bool MASkillUser::ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta )
#else
bool MASkillUser::ExecuteSkill( DnSkillHandle hSkill, LOCAL_TIME LocalTime, float fDelta, 
							    bool bCheckValid /*= true*/, bool bAutoUsedFromServer /*= false*/, bool bSendPacketToServer /*= true */ )
#endif
{
	_ASSERT( hSkill && "Invalid hSkill" );

	if( !IsValidActor() ) return false;

	if( m_hProcessSkill )
	{
		// #25154 오라 스킬은 오라를 껐을 때 onend 된다.
		if( false == m_hProcessSkill->IsAuraOn() )
			m_hProcessSkill->OnEnd( m_LocalTime, 0 );

		m_hProcessSkill.Identity();
	}

	m_hProcessSkill = hSkill;

	switch( m_hProcessSkill->GetDurationType() ) {
		case CDnSkill::Instantly:
		case CDnSkill::Buff:
		case CDnSkill::Debuff:
		case CDnSkill::SummonOnOff:
		case CDnSkill::StanceChange:
			// 마을에서는 SP 안 깍이도록
#ifndef _GAMESERVER
			if( CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
#endif
				m_pActor->UseMP( -m_hProcessSkill->GetDecreaseMP() );			// Compile Error. Moved by kalliste
				//m_pActor->SetSP( m_pActor->GetSP() - m_hProcessSkill->GetDecreaseMP() );
			
			// Excute 함수는 OnBegin 으로 이름이 바뀌었다.
			//m_hProcessSkill->Execute( LocalTime, fDelta );
			m_hProcessSkill->OnBegin( LocalTime, fDelta );

			// 글로벌 쿨타임이 설정되어있다면 같은 그룹의 스킬들도 쿨타임 시작..
			if( m_hProcessSkill && 0 < m_hProcessSkill->GetGlobalSkillGroupID() )
			{
				for( DWORD i = 0; i < GetSkillCount(); ++i )
				{
					// m_vlGroupedSkillIDs 의 값들은 테이블에서 그냥 끄집어 낸 것이기 때문에
					// 실제 스킬을 습득했는지 여부가 확인되어야 한다.
					DnSkillHandle hExistSkill = GetSkillFromIndex( i );
					if( hExistSkill && 
						hExistSkill != m_hProcessSkill &&
						hExistSkill->GetGlobalSkillGroupID() == m_hProcessSkill->GetGlobalSkillGroupID() )
					{
						// 글로벌 쿨타임으로 같이 도는 스킬들은 사용된 스킬의 쿨타임으로 바꿔준다.
						hExistSkill->ResetCoolTime();
						hExistSkill->OnAnotherGlobalSkillBeginCoolTime( hSkill );
						hExistSkill->OnBeginCoolTime();
					}
				}
			}
			break;

		case CDnSkill::TimeToggle:
		case CDnSkill::ActiveToggle:
		case CDnSkill::ActiveToggleForSummon:
			{
				bool bToggleEnable = !hSkill->IsToggleOn();
				hSkill->EnableToggle(bToggleEnable);
				OnSkillToggle(hSkill, bToggleEnable);

#ifdef _GAMESERVER
				// NOTE: 토글이 켜질 당시의 공격력을 저장해둔다. 추후에 공격력 말고 다른 것들이 추가될 수도 있다.
				if( hSkill && hSkill->IsToggleOn() )
				{
					m_ActorStateSnapshotForToggleProjectile.ResetState();
					hSkill->CheckAndAddSelfStateEffect();
					static_cast<CDnActor*>(this)->GetStateBlow()->Process( 0, 0.0f );
					CDnActor* pActor = static_cast<CDnActor*>(this);

					m_ActorStateSnapshotForToggleProjectile = *(static_cast<CDnState*>(pActor));
					hSkill->CheckAndRemoveInstantApplySelfStateEffect();
				}
#endif
			}
			break;;

		case CDnSkill::Aura:
			{
				// 오라 스킬들끼리 토글링 해줌. 다른 오라 스킬이 켜지면 꺼지고, 현재 시전중인 오라 스킬이면 꺼준다.
				// 오라 스킬들을 먼저 종료시키고 새로 켜지는 오라를 실행해야 새로 실행될 오라에서 self blow 큐에 쌓인 것들이 
				// 클리어 되지 않고 제대로 적용된다.
				bool bProcessingAuraOff = false;		// 같은 오라 스킬이 execute 되면 오라가 꺼지는 것임.
				int iNumAuraSkill = (int)m_vlhAuraSkills.size();
				for( int iAuraSkill = 0; iAuraSkill < iNumAuraSkill; ++iAuraSkill )
				{
					DnSkillHandle hAuraSkill = m_vlhAuraSkills.at( iAuraSkill );
					_ASSERT( hAuraSkill->GetDurationType() == CDnSkill::Aura );
					if( hAuraSkill->IsAuraOn() )
					{
						if( m_hProcessSkill == hAuraSkill )
							bProcessingAuraOff = true;

						hAuraSkill->EnableAura( false );
						OnSkillAura( hAuraSkill, false );
						break;
					}
				}

				// 다른 오라스킬을 사용해서 기존의 오라가 꺼지고 새로 발동됨.
				if( false == bProcessingAuraOff )
				{
					for( int iAuraSkill = 0; iAuraSkill < iNumAuraSkill; ++iAuraSkill )
					{
						DnSkillHandle hAuraSkill = m_vlhAuraSkills.at( iAuraSkill );
						if( hAuraSkill == m_hProcessSkill )
						{
							m_hProcessSkill->EnableAura( true );
							OnSkillAura( m_hProcessSkill, true );
							break;

							// Note 한기: m_hProcessSkill 스마트 포인터는 오라 스킬 사용하는 액션이 재생되는 동안은 유효해야
							// 게임 서버에서 CDnPlayerActor::CmdStop() 쪽에서 걸러지기 때문에 겜 서버에서 해당 액션 시그널이 끝까지 
							// 처리됨. 따라서 CDnActor::OnChangeAction 쪽에서 ProcessSkill 을 Identity 시킴.
						}
					}
				}
				else
				{
					// 켜져 있던 오라가 꺼짐.
					m_hAuraSkill.Identity();
					m_hProcessSkill.Identity();
				}
			}
			break;
	}

#ifndef _GAMESERVER
	m_bSkillExecutedThisFrame = true;
#endif

	return true;
}

#ifdef _GAMESERVER
void MASkillUser::UseAutoPassiveSkill( LOCAL_TIME LocalTime, float fDelta )
{
	list<DnSkillHandle>::iterator iter = m_listAutoPassiveSkills.begin();
	for( iter; m_listAutoPassiveSkills.end() != iter; ++iter )
	{
		DnSkillHandle hSkill = *iter;
		if( CDnSkill::UsingResult::Success == CanExecuteSkill( hSkill ) )
		{
			BYTE pBuffer[128] = {0,};
			CPacketCompressStream Stream( pBuffer, 128 );
			int iSkillID = hSkill->GetClassID();
			int iEnchantSkillID = hSkill->GetEnchantedSkillID();
			char cLevel = hSkill->GetLevel();
			EtVector2 vLook, vZVec;
			bool bUseApplySkillItem = false;
			char cUseSignalSkill = -1;
			bool bAutoUseFromServer = true;
			bool abSignalSkillCheck[ 3 ] = { false };

			vLook = EtVec3toVec2( *m_pActor->GetLookDir() );
			vZVec = EtVec3toVec2( *m_pActor->GetMoveVectorZ() );

			Stream.Write( &iSkillID, sizeof(int) );
			Stream.Write( &cLevel, sizeof(char) );
			Stream.Write( &bUseApplySkillItem, sizeof(bool) );
			Stream.Write( abSignalSkillCheck, sizeof(abSignalSkillCheck) );
			Stream.Write( &vZVec, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Write( &vLook, sizeof(EtVector2), CPacketCompressStream::VECTOR2_SHORT );
			Stream.Write( &bAutoUseFromServer, sizeof(bool) );
			Stream.Write( &iEnchantSkillID, sizeof(int) );
#ifdef PRE_ADD_POSITION_SYNC_BY_SKILL_USAGE
			Stream.Write( m_pActor->GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
#endif
			
			m_pActor->Send( eActor::SC_USESKILL, &Stream );

			ExecuteSkill( hSkill, LocalTime, fDelta );

			// 발동된 리스트에 넣어둔다. process 돌려야 하기 때문에,
			m_listProcessAutoPassiveSkills.push_back( hSkill );

			// 사용된 오토 패시브 스킬은 맨 뒤에 붙인다. 실행 우선순위를 낮춤.
			m_listAutoPassiveSkills.erase( iter );
			m_listAutoPassiveSkills.push_back( hSkill );

			// ProcessSkill 은 아니므로 초기화 시킴. ProcessSkill 이 있으면 CmdStop 이 먹지 않는다.
			m_hProcessSkill.Identity();
			break;
		}
	}
}


bool MASkillUser::IsProcessingAutoPassive( int iSkillID )
{
	list<DnSkillHandle>::iterator iter = m_listProcessAutoPassiveSkills.begin();
	for( iter; iter != m_listProcessAutoPassiveSkills.end(); ++iter )
	{
		DnSkillHandle hAutoPassive = *iter;
		if( hAutoPassive->GetClassID() == iSkillID )
			return true;
	}

	return false;
}




void MASkillUser::EndAutoPassiveSkill( LOCAL_TIME LocalTime, float fDelta )
{
	list<DnSkillHandle>::iterator iter = m_listProcessAutoPassiveSkills.begin();
	for( iter; iter != m_listProcessAutoPassiveSkills.end(); ++iter )
	{
		DnSkillHandle hAutoPassive = *iter;
		hAutoPassive->OnEnd( LocalTime, fDelta );
	}
}
#endif

bool MASkillUser::HavePassiveSkill(int iSkillID)
{
	int iNumSkill = (int)m_vlhSelfPassiveBlowSkill.size();
	for (int iSkill = 0; iSkill < iNumSkill; ++iSkill)
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at(iSkill);
		if (iSkillID == hSkill->GetClassID())
			return true;
	}

	return false;
}


void MASkillUser::OnSkillProcess( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_hAuraSkill )
	{
		//게임 서버에서만 판단해서 서버쪽 종료 시키고/클라이언트로 패킷 전송...
#if defined(_GAMESERVER)
		if( m_hAuraSkill->IsFinished() )
		{
			if (m_pActor)
				m_pActor->CmdFinishAuraSkill(m_hAuraSkill->GetClassID());
			
			// [2011/03/22 semozz]
			// 현재 스킬이 오라 스킬과 같은 스킬이면 현재 스킬도 초기화.
			if (m_hProcessSkill == m_hAuraSkill)
				m_hProcessSkill.Identity();

			OnSkillAura( m_hAuraSkill, false );
		}
		else
#endif // _GAMESERVER
			m_hAuraSkill->ProcessExecute( LocalTime, fDelta );

	}

#ifdef _GAMESERVER
	if( m_pActor && m_pActor->IsPlayerActor() )
	{
		list<DnSkillHandle>::iterator iter = m_listProcessAutoPassiveSkills.begin();
		for( iter; m_listProcessAutoPassiveSkills.end() != iter;  )
		{
			DnSkillHandle hSkill = *iter;

			//스킬 핸들 유효성 체크 추가
			if (!hSkill)
			{
				iter = m_listProcessAutoPassiveSkills.erase( iter );
			}
			else
			{
				if( hSkill->IsFinished() )
				{
					iter = m_listProcessAutoPassiveSkills.erase( iter );
				}
				else
				{
					hSkill->ProcessExecute( LocalTime, fDelta );
					++iter;
				}
			}
		}

		m_PrefixSkillCoolTimeManager.Process(LocalTime, fDelta);

		iter = m_listProcessPreFixDefenceSkills.begin();
		for( iter; m_listProcessPreFixDefenceSkills.end() != iter;  )
		{
			DnSkillHandle hSkill = *iter;

			//스킬 핸들 유효성 체크 추가
			if (!hSkill)
			{
				iter = m_listProcessPreFixDefenceSkills.erase( iter );
			}
			else
			{
				if( hSkill->IsFinished() )
				{
					hSkill->OnEnd( LocalTime, fDelta );
					iter = m_listProcessPreFixDefenceSkills.erase( iter );
				}
				else
				{
					hSkill->ProcessExecute( LocalTime, fDelta );
					++iter;
				}
			}
		}

		iter = m_listProcessPreFixOffenceSkills.begin();
		for( iter; m_listProcessPreFixOffenceSkills.end() != iter;  )
		{
			DnSkillHandle hSkill = *iter;
			if (!hSkill)
			{
				iter = m_listProcessPreFixOffenceSkills.erase( iter );
			}
			else
			{
				if( hSkill->IsFinished() )
				{
					hSkill->OnEnd( LocalTime, fDelta );
					iter = m_listProcessPreFixOffenceSkills.erase( iter );
				}
				else
				{
					hSkill->ProcessExecute( LocalTime, fDelta );
					++iter;
				}
			}
		}

		if (!m_prefixSystemDefenceSkills.empty())
		{
			PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemDefenceSkills.begin();
			PREFIX_SYSTEM_SKILL_LIST::iterator enditer = m_prefixSystemDefenceSkills.end();

			for (; iter != enditer; ++iter)
			{
				if (iter->second.hSkill)
					iter->second.hSkill->Process(LocalTime, fDelta);
			}
		}

		if (!m_prefixSystemOffenceSkills.empty())
		{
			PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemOffenceSkills.begin();
			PREFIX_SYSTEM_SKILL_LIST::iterator enditer = m_prefixSystemOffenceSkills.end();

			for (; iter != enditer; ++iter)
			{
				if (iter->second.hSkill)
					iter->second.hSkill->Process(LocalTime, fDelta);
			}
		}

	}
#else
	m_bSkillExecutedThisFrame = false;
#endif
	
	if( m_hProcessSkill ) 
	{
		if( m_hProcessSkill->IsFinished() )
		{
			m_hProcessSkill->OnEnd( LocalTime, fDelta );
			m_hProcessSkill.Identity();
		}
		else
			m_hProcessSkill->ProcessExecute( LocalTime, fDelta );
	}

	// 쿨타임 감소 상태효과가 있다면 델타값에 영향을 줄 값을 계산한다.
	if( m_pActor && m_pActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_096 ) )
	{
		DNVector(DnBlowHandle) vlhCoolTimeBlows;
		m_pActor->GetStateBlow()->GetStateBlowFromBlowIndex( STATE_BLOW::BLOW_096, vlhCoolTimeBlows );

		// 쿨타임 감소 상태효과는 - 기준이기때문에 중첩되면 빼준다.
		float fCoolTimeDeltaAdjust = 1.0f;
		int iNumBlow = (int)vlhCoolTimeBlows.size();
		for( int i = 0; i < iNumBlow; ++i )
		{
			fCoolTimeDeltaAdjust -= (1.0f - vlhCoolTimeBlows.at( i )->GetFloatValue());
		}

		// #40670 스피릿 부스트도 쿨타임 감소 상태효과를 사용하게 됨.
		// 중첩시 WeightTable 에서 제한을 받게 된다.
		float fCoolTimeAccelMax = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CoolTimeAccellSE_Max );
		if( fCoolTimeDeltaAdjust < 1.0f - fCoolTimeAccelMax )
			fCoolTimeDeltaAdjust = 1.0f - fCoolTimeAccelMax;

		// 1.0f 을 정상수치 기준으로 상태효과 인자값을 전체 쿨타임에 그대로 적용한 결과값이 현재 최종 쿨타임 기준이므로
		// 매 프레임 처리해주는 델타 값에 비율의 역수를 곱한다.
		// 결과로 나온 값은 각 스킬에서 루프를 돌 때 액터에서 얻어와서 직접 적용한다. 2415의 타임 엑셀러레이션 스킬만 제외하고.
		m_fCoolTimeDeltaAdjustValue = 1.0f / fCoolTimeDeltaAdjust;
	}
	else
		m_fCoolTimeDeltaAdjustValue = 1.0f;
	


#ifdef _GAMESERVER
#ifndef PRE_FIX_SKILLLIST

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	// 쿨타임 처리. 추후에 쿨 타임 뿐만 아니라 기타등등의 여러가지 지속적인 Processing 을 하게 될 것임.
	if( m_pActor && m_pActor->IsPlayerActor() /*&& m_pActor->IsAllowCallSkillProcess( fDelta )*/ )
	{
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

		for( DWORD i = 0; i < m_vlhSkillList.size(); i++ ) 
		{
			if( m_vlhSkillList.at( i ) )
				m_vlhSkillList[i]->Process( LocalTime, fDelta );
		}

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	}
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

#else

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	if( m_pActor && m_pActor->IsPlayerActor() /*&& m_pActor->IsAllowCallSkillProcess( fDelta )*/ )
	{
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

		PROFILE_TIME_TEST_BLOCK_START( "OnSkillProcess() - SkillHandleLoop" );
		// 쿨타임 처리. 추후에 쿨 타임 뿐만 아니라 기타등등의 여러가지 지속적인 Processing 을 하게 될 것임.
		for( DWORD i = 0; i < m_vlSkillObjects.size(); i++ ) 
		{
			// 클라이언트인 경우엔 스킬 객체 관리를 하는 SkillTask 가 먼저 소멸되어 valid 체크를 해야 함.
			if( m_vlSkillObjects.at( i ).hSkill )
				m_vlSkillObjects.at( i ).hSkill->Process( LocalTime, fDelta );
		}
		PROFILE_TIME_TEST_BLOCK_END();

#ifdef PRE_FIX_GAMESERVER_PERFOMANCE
	}
#endif // #ifdef PRE_FIX_GAMESERVER_PERFOMANCE

#endif // #ifndef PRE_FIX_SKILLLIST
#else
#ifndef PRE_FIX_SKILLLIST
	for( DWORD i = 0; i < m_vlhSkillList.size(); i++ ) 
	{
		if( m_vlhSkillList.at( i ) )
			m_vlhSkillList[i]->Process( LocalTime, fDelta );
	}
#else
	// 쿨타임 처리. 추후에 쿨 타임 뿐만 아니라 기타등등의 여러가지 지속적인 Processing 을 하게 될 것임.
	for( DWORD i = 0; i < m_vlSkillObjects.size(); i++ ) 
	{
		// 클라이언트인 경우엔 스킬 객체 관리를 하는 SkillTask 가 먼저 소멸되어 valid 체크를 해야 함.
		if( m_vlSkillObjects.at( i ).hSkill )
			m_vlSkillObjects.at( i ).hSkill->Process( LocalTime, fDelta );
	}
#endif // #ifndef PRE_FIX_SKILLLIST
#endif // #ifdef _GAMESERVER

	// 아이템에 붙은 스킬이 있다면 Process
	if( m_hItemSkill )
	{
		if( m_hItemSkill->IsFinished() )
		{
			m_hItemSkill->OnEnd( LocalTime, fDelta );

			deque<DnSkillHandle>::iterator iter = find( m_dqhItemSkillList.begin(), m_dqhItemSkillList.end(), m_hItemSkill );
			_ASSERT( m_dqhItemSkillList.end() != iter );
			m_dqhItemSkillList.erase( iter );

			SAFE_RELEASE_SPTR( m_hItemSkill );
		}
		else
			m_hItemSkill->ProcessExecute( LocalTime, fDelta );
	}

	for( DWORD i = 0; i < m_dqhItemSkillList.size(); ++i )
	{
		m_dqhItemSkillList.at( i )->Process( LocalTime, fDelta );
	}

	// 종료 예약된 스킬들 종료 시킴.
	int iNumReservedFinishSkill = (int)m_vlhReservedFinishSkill.size();
	for( int iSkill = 0; iSkill < iNumReservedFinishSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhReservedFinishSkill.at( iSkill );
		if( hSkill == m_hProcessSkill )
		{
			m_hProcessSkill->OnEnd( LocalTime, fDelta );
			m_hProcessSkill.Identity();
		}
		else
		if( hSkill == m_hToggleSkill )
		{
			_ASSERT( m_hToggleSkill->IsToggleOn() );
			if( m_hToggleSkill->IsToggleOn() )
				OnSkillToggle( m_hToggleSkill, false );
		}
		else
		if( hSkill == m_hAuraSkill )
		{
			_ASSERT( m_hAuraSkill->IsAuraOn() );
			if( m_hAuraSkill->IsAuraOn() )
				OnSkillAura( m_hAuraSkill, false );
		}
	}
	
	if( false == m_vlhReservedFinishSkill.empty() )
		m_vlhReservedFinishSkill.clear();
}



// 이 함수는 몬스터든 플레이어든 무기든 OnSignal 에서 Projectile 이 걸리면 항상 호출된다!!
void MASkillUser::OnSkillProjectile( CDnProjectile *pProjectile )
{
	if( !IsValidActor() ) 
		return;

	// Projectile 의 겉모습을 실제로 바꿔준다.
	bool bClientSide = false;

#if defined( _GAMESERVER )
	if( m_pActor->GetClassID() <= CDnActor::Reserved6 && m_pActor->IsPlayerActor() && ((CDnPlayerActor*)m_pActor)->IsLocalActor() )
		bClientSide = true;
#endif // #if defined( _GAMESERVER )
	
	if( m_hToggleSkill && m_hToggleSkill->IsToggleOn() )
	{
		// 현재 토글 Enable 될 상태인 스킬을 처리하는 것임.
		_ASSERT( m_hToggleSkill->IsToggleOn() );

		CDnChangeProjectileProcessor* pChangeProj = static_cast<CDnChangeProjectileProcessor*>(m_hToggleSkill->GetProcessor( IDnSkillProcessor::CHANGE_PROJECTILE ));
		if( pChangeProj )
		{
			int iCheckProjectileID = ( bClientSide ) ? pChangeProj->GetSourceWeaponID() : pChangeProj->GetChangeWeaponID();
			if( pProjectile->GetClassID() == iCheckProjectileID )
			{
				if( m_hToggleSkill->GetDurationType() == CDnSkill::ActiveToggle )
				{
					if( !(m_hToggleSkill->GetDecreaseMP() > 0 && m_pActor->GetSP() < m_hToggleSkill->GetDecreaseMP()) )
					{
						// 마을에서는 SP 안 깍이도록
#ifndef _GAMESERVER
						if( CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
#endif
							m_pActor->UseMP( -m_hToggleSkill->GetDecreaseMP() );
							//m_pActor->SetSP( m_pActor->GetSP() - m_hToggleSkill->GetDecreaseMP() );

						if( bClientSide )
						{
							int iWeaponLength = pProjectile->GetWeaponLength() - pProjectile->GetOriginalWeaponLength();
							*(CDnWeapon*)pProjectile = *pChangeProj->GetProjectile();
							pProjectile->SetWeaponLength( iWeaponLength + pProjectile->GetOriginalWeaponLength() );
						}

						OnSkillToggleProjectile( m_hToggleSkill, pProjectile );

						if( m_pActor->GetSP() < m_hToggleSkill->GetDecreaseMP() ) 
						{	
							OnSkillToggle( m_hToggleSkill, false );
						}
					}
				}
			}
		}
		else
		{
			// ChangeAction 이라고 지호씨가 추가한 거 있음
			CDnChangeActionStrProcessor* pChangeAction = static_cast<CDnChangeActionStrProcessor*>(m_hToggleSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ));
			if( pChangeAction )
			{
				if( m_hToggleSkill->GetDurationType() == CDnSkill::ActiveToggle )
				{
					if( !(m_hToggleSkill->GetDecreaseMP() > 0 && m_pActor->GetSP() < m_hToggleSkill->GetDecreaseMP()) )
					{
						// 마을에서는 SP 안 깍이도록
#ifndef _GAMESERVER
						if( CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
#endif
							m_pActor->UseMP( -m_hToggleSkill->GetDecreaseMP() );
							//m_pActor->SetSP( m_pActor->GetSP() - m_hToggleSkill->GetDecreaseMP() );

						OnSkillToggleProjectile( m_hToggleSkill, pProjectile );

						if( m_pActor->GetSP() < m_hToggleSkill->GetDecreaseMP() ) 
						{	
							OnSkillToggle( m_hToggleSkill, false );
						}
					}
				}
			}

		}
	}

#ifdef _GAMESERVER
	if( pProjectile && !bClientSide )
	{
		// Note 한기: 직접 데미지 계산에 필요한 ActorState 를 Projectile 에 실어서 날리는 방법으로 변경.
		// 토글 스킬이 아닌 프로젝타일을 쏘는 스킬인 경우엔 여기서 공격력 적용시켜줌
		if( !m_hToggleSkill && m_hProcessSkill )
		{
			static_cast<CDnActor*>(this)->GetStateBlow()->Process( 0, 0.0f );
			CDnActor* pActor = static_cast<CDnActor*>(this);

			// 체인 스킬 상태효과를 쓰는 발사체는 
			// CDnState 복사가 매우 빈번하게 일어날 수 있으므로 shaared_ptr 사용.
			boost::shared_ptr<CDnState> pActorStateSnapshot = boost::shared_ptr<CDnState>(new CDnState);
			*pActorStateSnapshot = *(static_cast<CDnState*>(pActor));

			// 스킬인 경우에만 여기서 
			pProjectile->FromSkill( true );
			pProjectile->SetShooterStateSnapshot( pActorStateSnapshot );


#if defined(PRE_FIX_65287)
			float fFinalDamageRate = 0.0f;
			if (pActor && pActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
			{
				DNVector(DnBlowHandle) vlhBlows;
				pActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
				int iNumBlow = (int)vlhBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					fFinalDamageRate += vlhBlows[i]->GetFloatValue();
				}
			}

			pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287
		}
		else if( m_hToggleSkill && m_hToggleSkill->IsToggleOn() )
		{
			// 현재 토글 스킬은 사용되지 않지만 추후 어찌될지 몰라 코드 남겨둠. 2011.03.15
			// 토글을 켠 당시의 능력치로 프로젝타일에 셋팅해 줌.
			boost::shared_ptr<CDnState> pActorStatesnapshot = boost::shared_ptr<CDnState>( new CDnState );
			*pActorStatesnapshot = m_ActorStateSnapshotForToggleProjectile;
			
			pProjectile->SetShooterStateSnapshot( pActorStatesnapshot );
			pProjectile->FromSkill( true );

#if defined(PRE_FIX_65287)
			CDnActor* pActor = static_cast<CDnActor*>(this);

			float fFinalDamageRate = 0.0f;
			if (pActor && pActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_050))
			{
				DNVector(DnBlowHandle) vlhBlows;
				pActor->GatherAppliedStateBlowByBlowIndex( STATE_BLOW::BLOW_050, vlhBlows );
				int iNumBlow = (int)vlhBlows.size();
				for( int i = 0; i < iNumBlow; ++i )
				{
					fFinalDamageRate += vlhBlows[i]->GetFloatValue();
				}
			}

			pProjectile->SetShooterFinalDamageRate(fFinalDamageRate);
#endif // PRE_FIX_65287
		}
		else if( m_pActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_121 ) || m_pActor->IsAppliedThisStateBlow( STATE_BLOW::BLOW_129 ) )
		{
			// #30362 스탠드 액션 변경 상태효과에서 발사체를 쏘는 경우에도 스킬 발사체로 처리 함.
			pProjectile->FromSkill( true );
		}
	}
#endif

}

void MASkillUser::OnSkillToggleProjectile( DnSkillHandle hSkill, CDnProjectile *pProjectile )
{
}

void MASkillUser::OnSkillToggle( DnSkillHandle hSkill, bool bEnable )
{
	if( !IsValidActor() ) return;

	if( bEnable )
	{
		switch( hSkill->GetDurationType() )
		{
			case CDnSkill::ActiveToggle:
			case CDnSkill::ActiveToggleForSummon:
				// 마을에서는 SP 안 깍이도록
#ifndef _GAMESERVER
				if( CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
#endif
					// 토글링할땐 깍이지 않는다. 쓸 때 깍임
					//m_pActor->SetSP( m_pActor->GetSP() - hSkill->GetDecreaseSP() );
				hSkill->OnBegin( m_LocalTime, 0.0f );
				break;

			case CDnSkill::TimeToggle:
				hSkill->OnBegin( m_LocalTime, 0.0f );
				break;
		}

		SkillToggle(hSkill, true);
	}
	else
	{
		SkillToggle(hSkill, false);

		hSkill->OnEnd( m_LocalTime, 0.0f );
		hSkill->EnableToggle( false );
	}

	//if( bEnable ) {
	//	switch( hSkill->GetDurationType() ) {
	//		case CDnSkillBase::TimeToggle:
	//			pActor->SetSP( pActor->GetSP() - hSkill->GetDecreaseSP() );
	//			hSkill->Execute( m_LocalTime, 0.f );
	//			break;
	//		case CDnSkillBase::ActiveToggle:
	//			hSkill->Execute( m_LocalTime, 0.f );
	//			break;
	//		case CDnSkillBase::Aura:
	//			pActor->SetSP( pActor->GetSP() - hSkill->GetDecreaseSP() );
	//			hSkill->Execute( m_LocalTime, 0.f );
	//			break;
	//	}
	//}
	//else {
	//	hSkill->Finish( m_LocalTime, 0.f );
	//}
}


void MASkillUser::OnSkillAura( DnSkillHandle hSkill, bool bEnable )
{
	if( !IsValidActor() ) return;

	if( bEnable )
	{
		switch( hSkill->GetDurationType() )
		{
			case CDnSkill::Aura:
				// 마을에서는 SP 안 깍이도록
#ifndef _GAMESERVER
				if( CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
#endif
					m_pActor->UseMP( -hSkill->GetDecreaseMP() );
					//m_pActor->SetSP( m_pActor->GetSP() - hSkill->GetDecreaseMP() );
				hSkill->OnBegin( m_LocalTime, 0.0f );
				break;
		}

		m_hAuraSkill = hSkill;
	}
	else
	{
		hSkill->OnEnd( m_LocalTime, 0.0f );
		hSkill->EnableAura( false );

		if( m_hAuraSkill == hSkill )
			m_hAuraSkill.Identity();
	}
}



void MASkillUser::ResetToggleSkill()
{
	//if( m_hToggleSkill )
	//	m_hToggleSkill.Identity();

	//int iNumToggle = (int)m_vlhToggleSkills.size();
	//for( int i = 0; i < iNumToggle; ++i )
	//{
	//	if( m_vlhToggleSkills.at(i)->IsToggleOn() )
	//		OnSkillToggle( m_vlhToggleSkills.at(i), false );
	//}
}



void MASkillUser::ResetAuraSkill()
{
	//if( m_hAuraSkill )
	//	m_hAuraSkill.Identity();

	//int iNumAura = (int)m_vlhAuraSkills.size();
	//for( int i = 0; i < iNumAura; ++i )
	//{
	//	if( m_vlhAuraSkills.at(i)->IsAuraOn() )
	//		OnSkillAura( m_vlhAuraSkills.at(i), false );
	//}
}


bool MASkillUser::IsProcessSkill()
{
	if( !m_hProcessSkill ) return false;
	return !m_hProcessSkill->IsFinished();

	return true;
}

DWORD MASkillUser::GetSkillCount()
{
#ifndef PRE_FIX_SKILLLIST
	return (DWORD)m_vlhSkillList.size();
#else
	return (DWORD)m_vlSkillObjects.size();
#endif // #ifndef PRE_FIX_SKILLLIST
}

DnSkillHandle MASkillUser::GetSkillFromIndex( DWORD dwIndex )
{
#ifndef PRE_FIX_SKILLLIST
	if( dwIndex < 0 || dwIndex >= m_vlhSkillList.size() ) return CDnSkill::Identity();
	return m_vlhSkillList[dwIndex];
#else
	if( dwIndex < 0 || dwIndex >= m_vlSkillObjects.size() ) 
		return CDnSkill::Identity();
		
	return m_vlSkillObjects.at( dwIndex ).hSkill;
#endif // #ifndef PRE_FIX_SKILLLIST
}

CDnSkill::UsingResult MASkillUser::UseSkill( int nSkillTableID, bool bCheckValid, bool bAutoUseFromServer/* = false*/, int nLuaSkillIndex/*=-1*/ )
{
	CDnSkill::UsingResult eResult = CDnSkill::UsingResult::Failed;

	DnSkillHandle hSkill = FindSkill( nSkillTableID );
	if( hSkill ) 
	{
		if( m_pActor && m_pActor->IsPlayerActor() )
		{
			if( hSkill->GetSkillType() == CDnSkill::SkillTypeEnum::EnchantPassive )
				return CDnSkill::UsingResult::Hack;
		}

#ifdef _GAMESERVER
		// 서버의 쿨타임 오차를 감안해서 0.5초의 여유를 두고 있지만,
		// 존이동할때나 기타 0.5초 이상 클라이언트와 벌어지는 다른 새로운 경우들이 생길 수 있으므로
		// 최종 스킬 사용한 타임 스탬프를 찍어두어 데이터에 지정된 스킬의 쿨타임보타 간격이 크다면 
		// 서버의 스킬 객체에 저장되어있는 쿨타임을 초기화 시켜주도록 한다. (#19737)
		hSkill->UpdateSkillCoolTimeExactly();
#endif

		eResult = CanExecuteSkill( hSkill );
		if( !bCheckValid || CDnSkill::UsingResult::Success == eResult )
		{
#if defined(_CLIENT)
			//스킬 사용이 성공이라도 소환 몬스터가 있다면 소환 몬스터를 소환 해제만 하고 실제 스킬 사용은 안됨..
			if (hSkill->GetDurationType() == CDnSkill::DurationTypeEnum::SummonOnOff)
			{
				if (hSkill->SummonMonsterOff())
					return eResult;
			}
#endif // _CLIENT
#ifdef _GAMESERVER
			ExecuteSkill( hSkill, m_LocalTime, 0.f );
#else
			ExecuteSkill( hSkill, m_LocalTime, 0.f, bCheckValid, bAutoUseFromServer );
#endif
		}
#if defined(_CLIENT)
		else
		{
			// 스킬 사용이 실패 했더라도 소환 몬스터가 있다면 소환 몬스터 소환 해제 시킴.
			if (hSkill->GetDurationType() == CDnSkill::DurationTypeEnum::SummonOnOff)
				hSkill->SummonMonsterOff();
		}
#endif // _CLIENT
	}

	return eResult;
}



bool MASkillUser::UseItemSkill( int nSkillTableID, int nSkillLevelTableID, CDnItem::ItemSkillApplyType ItemSkillApplyType, int nItemID/* = -1*/ )
{
	if( !IsValidActor() ) return false;
	bool bResult = true;

	switch( ItemSkillApplyType )
	{
		case CDnItem::ApplySkill:
			{
				// 여기서 생성된 아이템 스킬은 OnProcessSkill 함수에서 Finish 체크를 하여 리스트에서 알아서 삭제해준다.
				DnSkillHandle hSkill = CDnSkill::CreateSkill( m_pActor->GetMySmartPtr(), nSkillTableID, nSkillLevelTableID );

				if( !hSkill ) {
					bResult = false;
					break;
				}
					
				// 아이템에 붙은 스킬로 셋팅해줌.
				hSkill->AsItemSkill();
				
#if defined(_GAMESERVER)
				hSkill->SetItemID(nItemID);
#endif // _GAMESERVER

				m_dqhItemSkillList.push_back( hSkill );

				ExecuteSkill( hSkill, m_LocalTime, 0.f );
				m_hItemSkill = hSkill;
			}
			break;

			// 삭제 예정.
		case CDnItem::ApplyStateBlow:
			{
				//// 여기에 들어오는 Instant 아이템인 경우 CDnItemTask 에서 아이템 생성하고 곧바로 삭제함.
				//DnSkillHandle hSkill = CDnSkill::CreateSkill( m_pActor->GetMySmartPtr(), nSkillTableID, nSkillLevelTableID );
				//
				//if( !hSkill )
				//	bResult = false;

				//hSkill->AsItemSkill();

				//// 강제로 상태효과 동기를 맞춰주는 함수.
				//hSkill->OnBeginForceSync( m_LocalTime, 0.0f );
				//hSkill->Release();
			}
			break;
	}

	return bResult;
}



void MASkillUser::ResetSkillCoolTime( void )
{
#ifndef PRE_FIX_SKILLLIST
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );
		if( hSkill )
			hSkill->ResetCoolTime();
	}
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		if (hSkill)
			hSkill->ResetCoolTime();
	}
#endif // #ifndef PRE_FIX_SKILLLIST

#if defined(_GAMESERVER)
	m_listProcessPreFixDefenceSkills.clear();
	m_listProcessPreFixOffenceSkills.clear();
	
	if (!m_prefixSystemDefenceSkills.empty())
	{
		PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemDefenceSkills.begin();
		PREFIX_SYSTEM_SKILL_LIST::iterator enditer = m_prefixSystemDefenceSkills.end();

		for (; iter != enditer; ++iter)
		{
			if (iter->second.hSkill)
				iter->second.hSkill->ResetCoolTime();
		}
	}

	if (!m_prefixSystemOffenceSkills.empty())
	{
		PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemOffenceSkills.begin();
		PREFIX_SYSTEM_SKILL_LIST::iterator enditer = m_prefixSystemOffenceSkills.end();

		for (; iter != enditer; ++iter)
		{
			if (iter->second.hSkill)
				iter->second.hSkill->ResetCoolTime();
		}
	}

	m_PrefixSkillCoolTimeManager.InitList();
#endif // _GAMESERVER
}

void MASkillUser::ResetPvPSkillCoolTime()
{
#ifndef PRE_FIX_SKILLLIST
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );
		if( hSkill->GetSkillType() == CDnSkill::SkillTypeEnum::Active )
		{
			hSkill->OnBeginCoolTime();
		}
	}
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		if( hSkill->GetSkillType() == CDnSkill::SkillTypeEnum::Active )
		{
			hSkill->OnBeginCoolTime();
		}
	}
#endif // #ifndef PRE_FIX_SKILLLIST
}

void MASkillUser::ResetLadderSkillCoolTime()
{
#ifndef PRE_FIX_SKILLLIST
	ResetSkillCoolTime();
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );
		hSkill->SetElapsedDelayTime( hSkill->GetDelayTime()/2 );
	}
#else
	ResetSkillCoolTime();
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		hSkill->SetElapsedDelayTime( hSkill->GetDelayTime()/2 );
	}
#endif // #ifndef PRE_FIX_SKILLLIST
}

#ifdef _GAMESERVER
void MASkillUser::IgnoreSkillCoolTime( void )
{
#ifndef PRE_FIX_SKILLLIST
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int iSkill = 0; iSkill < iNumSkill; ++iSkill )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( iSkill );
		hSkill->IgnoreCoolTime();
	}

	m_bIgnoreCoolTime = true;
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		hSkill->IgnoreCoolTime();
	}

	m_bIgnoreCoolTime = true;
#endif // #ifndef PRE_FIX_SKILLLIST
}
#endif


void MASkillUser::ReserveFinishSkill( DnSkillHandle hSkill )
{
	if( hSkill )
	{
		m_vlhReservedFinishSkill.push_back( hSkill );
	}
}


void MASkillUser::CancelUsingSkill( void )
{
	if( m_hProcessSkill )
	{
		m_hProcessSkill->OnEnd( 0, 0.0f );
		m_hProcessSkill.Identity();
	}
};


void MASkillUser::OnAttachWeapon( DnWeaponHandle hWeapon, int iChangedWeaponIndex )
{
	// 패시브 스킬들 중에 사용가능 무기를 정의한 것들이 있나 체크.
	// 다른 무기를 차고 있어서 패시브 상태효과들이 적용된 상태가 아니라면 활성화 시켜준다.
	int iNumSelfBlowPassiveSkills = (int)m_vlhSelfPassiveBlowSkill.size();
	for( int i = 0; i < iNumSelfBlowPassiveSkills; ++i )
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at( i );
		if( false == hSkill->IsAppliedPassiveBlows() )
		{
			// 내부에서 사용가능 무기 체크 함.
			ApplyPassiveSkill( hSkill );
		}
	}

	// rangechecker 있는 스킬들은 무기 찼을 때 업데이트 해준다.
#ifndef _GAMESERVER

#ifndef PRE_FIX_SKILLLIST
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int i = 0; i < iNumSkill; ++i )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( i );
		hSkill->UpdateRangeChecker( hSkill->GetSelectedLevelDataType() );
	}
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		hSkill->UpdateRangeChecker( hSkill->GetSelectedLevelDataType() );
	}
#endif // #ifndef PRE_FIX_SKILLLIST

#endif
}

void MASkillUser::OnDetachWeapon( DnWeaponHandle hWeapon, int iDetachedWeaponIndex )
{
	// 패시브 스킬들 중에 사용가능 무기를 정의한 것들이 있나 체크.
	// 요구하는 무기와 맞지 않는다면 패시브 스킬에 딸린 상태효과들을 제거한다.
	int iNumSelfBlowPassiveSkills = (int)m_vlhSelfPassiveBlowSkill.size();
	for( int i = 0; i < iNumSelfBlowPassiveSkills; ++i )
	{
		DnSkillHandle hSkill = m_vlhSelfPassiveBlowSkill.at( i );
		if( hSkill->IsAppliedPassiveBlows() )
		{
			if( false == hSkill->IsSatisfyWeapon() )
			{
				int iNumStateBlow = hSkill->GetStateEffectCount();
				for( int iBlow = 0; iBlow < iNumStateBlow; ++iBlow )
				{
					const CDnSkill::StateEffectStruct* pSE = hSkill->GetStateEffectFromIndex( iBlow );

					DNVector(DnBlowHandle) vlAppliedBlows;
					m_pActor->GatherAppliedStateBlowByBlowIndex( (STATE_BLOW::emBLOW_INDEX)pSE->nID, vlAppliedBlows );

					int iNumAppliedBlow = (int)vlAppliedBlows.size();
					for( int iAppliedBlow = 0; iAppliedBlow < iNumAppliedBlow; ++iAppliedBlow )
					{
						DnBlowHandle hBlow = vlAppliedBlows.at( iAppliedBlow );
						if( hBlow->GetParentSkillInfo()->iSkillID == hSkill->GetClassID() )
						{
							// 현시점에서 곧바로 삭제
							//m_pActor->GetStateBlow()->RemoveImediatlyStateEffectFromID( hBlow->GetBlowID() );
							//RebirthBlow에서 OnBegin에서 타고 들어와서 삭제가 되는 경우가 발생.
							//즉시 지우지 않고 지움 리스트에 추가 하도록 수정
							m_pActor->GetStateBlow()->RemoveStateBlowFromID(hBlow->GetBlowID());
						}
					}
				}

				hSkill->SetAppliedPassiveBlows( false );
			}
		}
	}

	// rangechecker 있는 스킬들은 무기 찼을 때 업데이트 해준다.
#ifndef _GAMESERVER
#ifndef PRE_FIX_SKILLLIST
	int iNumSkill = (int)m_vlhSkillList.size();
	for( int i = 0; i < iNumSkill; ++i )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( i );
		hSkill->UpdateRangeChecker( hSkill->GetSelectedLevelDataType() );
	}
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		hSkill->UpdateRangeChecker( hSkill->GetSelectedLevelDataType() );
	}
#endif // #ifndef PRE_FIX_SKILLLIST
#endif
}

#ifdef _GAMESERVER
void MASkillUser::SelectSkillLevelDataType( int iLevelDataType, bool bPlayerSummonedMonster/* = false*/ )
{
#ifndef PRE_FIX_SKILLLIST
	for( int i = 0; i < (int)m_vlhSkillList.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlhSkillList.at( i );
		hSkill->SelectLevelDataType( iLevelDataType, bPlayerSummonedMonster );
	}
#else
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		DnSkillHandle hSkill = m_vlSkillObjects.at( i ).hSkill;
		hSkill->SelectLevelDataType( iLevelDataType, bPlayerSummonedMonster );
	}
#endif // #ifndef PRE_FIX_SKILLLIST
}

int MASkillUser::GetSelectedSkillLevelDataType( void )
{
#ifndef PRE_FIX_SKILLLIST
	if( m_vlhSkillList.empty() )
		return CDnSkill::NUM_SKILLLEVEL_APPLY_TYPE;
	else
		return m_vlhSkillList.front()->GetSelectedLevelDataType();
#else
	if( m_vlSkillObjects.empty() )
		return CDnSkill::NUM_SKILLLEVEL_APPLY_TYPE;
	else
		return m_vlSkillObjects.front().hSkill->GetSelectedLevelDataType();
#endif // #ifndef PRE_FIX_SKILLLIST
}
#endif

#ifndef _GAMESERVER
bool MASkillUser::IsUsingSkillAction( const char *szAction )
{
	if( m_setUseActionNames.find( szAction ) != m_setUseActionNames.end() ) return true;
	return false;
}
#endif


void MASkillUser::AddStateBlowIDToRemove(int blowID)
{
	m_vlStateBlowIDToRemove.insert(std::map<int, int>::value_type(blowID, blowID));
}

void MASkillUser::InitStateBlowIDToRemove()
{
	m_vlStateBlowIDToRemove.clear();
}

#ifdef _GAMESERVER
void MASkillUser::RemoveApplySEWhenTargetNormalHitProcessor( IDnSkillProcessor* pProcessor )
{
	DNVector( IDnSkillProcessor* )::iterator iter = find( m_vlpApplySEWhenNormalHitProcessor.begin(), m_vlpApplySEWhenNormalHitProcessor.end(), pProcessor );
	if( m_vlpApplySEWhenNormalHitProcessor.end() != iter )
		m_vlpApplySEWhenNormalHitProcessor.erase( iter );
}
#endif


#if defined(_GAMESERVER)
bool MASkillUser::AddPreFixSystemDefenceSkill(int slotIndex, DnSkillHandle hSkill)
{
	if (!hSkill)
	{
		RemovePreFixSystemDefenceSkill(slotIndex);
		return false;
	}

	PREFIX_SYSTEM_SKILLINFO skillInfo;
	skillInfo.slotIndex = slotIndex;
	skillInfo.hSkill = hSkill;
	skillInfo.hSkill->SetItemPrefixSkill();
	
	//해당 스킬의 확률값을 가져온다.
	CDnProbabilityChecker* pProbabilityChecker = static_cast<CDnProbabilityChecker*>(hSkill->GetChecker(IDnSkillUsableChecker::PROB_CHECKER));
	skillInfo.fRatio = pProbabilityChecker ? pProbabilityChecker->GetProbability() : 0.0f;
	
	bool bResult = m_prefixSystemDefenceSkills.insert(PREFIX_SYSTEM_SKILL_LIST::value_type(slotIndex, skillInfo)).second;

// 	if (bResult)
// 		RefreshPrefixDefenceSkills();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	AddPrefixDefenceSkill(hSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

	return bResult;
}

void MASkillUser::RemovePreFixSystemDefenceSkill(int slotIndex)
{
	PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemDefenceSkills.find(slotIndex);
	if (iter != m_prefixSystemDefenceSkills.end())
	{
		DnSkillHandle hSkill = iter->second.hSkill;
		
#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
		RemovePrefixDefenceSkill(hSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

		RemoveProcessPrefixDefenceSkill(hSkill);
				
		SAFE_RELEASE_SPTR(hSkill);
		m_prefixSystemDefenceSkills.erase(iter);

		//RefreshPrefixDefenceSkills();
	}
}

void MASkillUser::RemoveProcessPrefixDefenceSkill(DnSkillHandle hSkill)
{
	list<DnSkillHandle>::iterator iter = m_listProcessPreFixDefenceSkills.begin();
	for( ; iter != m_listProcessPreFixDefenceSkills.end(); )
	{
		DnSkillHandle hProcessPrefixDefenceSkill = *iter;
		if (hProcessPrefixDefenceSkill == hSkill)
		{
			iter = m_listProcessPreFixDefenceSkills.erase(iter);
			continue;
		}

		++iter;
	}
}


void MASkillUser::RefreshPrefixDefenceSkills()
{
	PREFIX_SYSTEM_SKILL_COLLECTION sameSkillList;
	//1. 슬롯별로 저장된 스킬들을 같은 스킬끼리 수집한다.
	GroupingPrefixDefenceSkillsBySameSkillID(m_prefixSystemDefenceSkills, sameSkillList);

	PREFIX_SYSTEM_SKILL_LIST tempCandiateDefenceSkills;
	//2. 같은 스킬들 확률을 더해서 확률을 결정하고, 같은 스킬중 레벨이 높은 녀석을 선택한다.
	CalculateProbabilitySkill(tempCandiateDefenceSkills, sameSkillList);

	//3. 스킬ID별로 레벨이 높은 녀석의 리스트에서 스킬의 우선순위별로 그룹핑한다.
	GroupingSkillByPriority(m_PrefixSystemCandiateDefenceSkills, tempCandiateDefenceSkills);
}

bool MASkillUser::AddPreFixSystemOffenceSkill(int slotIndex, DnSkillHandle hSkill)
{
	if (!hSkill)
	{
		RemovePreFixSystemOffenceSkill(slotIndex);
		return false;
	}

	PREFIX_SYSTEM_SKILLINFO skillInfo;
	skillInfo.slotIndex = slotIndex;
	skillInfo.hSkill = hSkill;
	skillInfo.hSkill->SetItemPrefixSkill();

	//해당 스킬의 확률값을 가져온다.
	CDnProbabilityChecker* pProbabilityChecker = static_cast<CDnProbabilityChecker*>(hSkill->GetChecker(IDnSkillUsableChecker::PROB_CHECKER));
	skillInfo.fRatio = pProbabilityChecker ? pProbabilityChecker->GetProbability() : 0.0f;

	bool bResult = m_prefixSystemOffenceSkills.insert(PREFIX_SYSTEM_SKILL_LIST::value_type(slotIndex, skillInfo)).second;

// 	if (bResult)
// 		RefreshPrefixOffenceSkills();

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
	AddPrefixOffenceSkill(hSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

	return bResult;
}

void MASkillUser::RemovePreFixSystemOffenceSkill(int slotIndex)
{
	PREFIX_SYSTEM_SKILL_LIST::iterator iter = m_prefixSystemOffenceSkills.begin(); 
	
	for( ; iter != m_prefixSystemOffenceSkills.end(); )
	{
		if( iter->first == slotIndex )
		{
			DnSkillHandle hSkill = iter->second.hSkill;
			hSkill->OnEnd( 0, 0.f );

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
			RemovePrefixOffenceSkill(hSkill);
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

			RemoveProcessPrefixOffenceSkill(hSkill);

			SAFE_RELEASE_SPTR(hSkill);
			iter = m_prefixSystemOffenceSkills.erase(iter);
			continue;
		}

		 ++iter;
	}
}

void MASkillUser::RemoveProcessPrefixOffenceSkill(DnSkillHandle hSkill)
{
	list<DnSkillHandle>::iterator iter = m_listProcessPreFixOffenceSkills.begin();
	for( ; iter != m_listProcessPreFixOffenceSkills.end(); )
	{
		DnSkillHandle hProcessPrefixOffenceSkill = *iter;
		if (hProcessPrefixOffenceSkill == hSkill)
		{
			iter = m_listProcessPreFixOffenceSkills.erase(iter);
			continue;
		}

		++iter;
	}
}

void MASkillUser::RefreshPrefixOffenceSkills(float fHitDamageProb)
{
	PREFIX_SYSTEM_SKILL_COLLECTION sameSkillList;
	//1. 슬롯별로 저장된 스킬들을 같은 스킬끼리 수집한다.
	GroupingPrefixDefenceSkillsBySameSkillID(m_prefixSystemOffenceSkills, sameSkillList);

	PREFIX_SYSTEM_SKILL_LIST tempCandiateOffenceSkills;
	//2. 같은 스킬들 확률을 더해서 확률을 결정하고, 같은 스킬중 레벨이 높은 녀석을 선택한다.
	CalculateProbabilitySkill(tempCandiateOffenceSkills, sameSkillList);

	//3. 스킬ID별로 레벨이 높은 녀석의 리스트에서 스킬의 우선순위별로 그룹핑한다.
	GroupingSkillByPriority(m_PrefixSystemCandiateOffenceSkills, tempCandiateOffenceSkills, fHitDamageProb);
}

void MASkillUser::ProcessPrefixDefenceSkill(DnActorHandle hHitter)
{
	//접두어 시스템 방어용 스킬 등록되 되어 있지 않다면	
	if (m_prefixSystemDefenceSkills.empty())
		return;

	OutputDebug("접두어 방어용 스킬 시작-----------------------------!!\n");

	//접두어 시스템 방어용 스킬중 선택..
	RefreshPrefixDefenceSkills();

	// 최종 리스트에 담긴 스킬중에서 우선순위가 가장 높은 리스트에 저장된 스킬만 사용한다.
	if (m_PrefixSystemCandiateDefenceSkills.empty())
	{
		OutputDebug("접두어 방어용 스킬 선택된거 없음.-----------------------------!!\n");
		return;
	}

	PREFIX_SYSTEM_SKILL_COLLECTION::iterator selectIter = m_PrefixSystemCandiateDefenceSkills.begin();
	PRESIX_SYSTEM_SKILLS &skills = selectIter->second;

	PRESIX_SYSTEM_SKILLS::iterator iter = skills.begin();
	PRESIX_SYSTEM_SKILLS::iterator endIter = skills.end();

	for (; iter != endIter; ++iter)
	{
		DnSkillHandle hSkill = (*iter).hSkill;
		if (!hSkill /*|| CDnSkill::UsingResult::Success != hSkill->CanExecute()*/)
			continue;

		/*
		//쿨타임 남아 있으면 건너뜀.
		if (hSkill->GetCoolTime() != 0.0f && hSkill->GetElapsedDelayTime() > 0.0f)
			continue;
		*/

		OutputDebug("접두어 방어용 스킬(%d) 시작....\n", hSkill->GetClassID());

		//Skill 쿨타임 시작?
		hSkill->OnBeginCoolTime();
		m_PrefixSkillCoolTimeManager.AddCoolTime(hSkill);

		int nStateEffectCount = hSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( DWORD k = 0; k < hSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = hSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				hApplyActor = m_pActor->GetActorHandle();
				break;

			case CDnSkill::ApplyTarget: // 타겟한테
				hApplyActor = hHitter;
				break;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				if( m_pActor->GetTeam() == hHitter->GetTeam() )
					continue;
				else
					hApplyActor = hHitter;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				if( m_pActor->GetTeam() != hHitter->GetTeam() )
					continue;
				else
					hApplyActor = hHitter;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);
			
			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}

		//사용된 방어 스킬은 리스트에 담아 놓는다..
		m_listProcessPreFixDefenceSkills.push_back(hSkill);
	}

	OutputDebug("접두어 방어용 스킬 끝  -----------------------------!!\n");
}

void MASkillUser::ProcessPrefixOffenceSkill(float fHitDamageProb)
{
	//접두어 시스템 방어용 스킬 등록되 되어 있지 않다면	
	if (m_prefixSystemOffenceSkills.empty())
		return;

	//접두어 시스템 공격용 스킬중 선택..
	RefreshPrefixOffenceSkills(fHitDamageProb);

	// 최종 리스트에 담긴 스킬중에서 우선순위가 가장 높은 리스트에 저장된 스킬만 사용한다.
	if (m_PrefixSystemCandiateOffenceSkills.empty())
		return;

	PREFIX_SYSTEM_SKILL_COLLECTION::iterator selectIter = m_PrefixSystemCandiateOffenceSkills.begin();
	PRESIX_SYSTEM_SKILLS &skills = selectIter->second;

	PRESIX_SYSTEM_SKILLS::iterator iter = skills.begin();
	PRESIX_SYSTEM_SKILLS::iterator endIter = skills.end();

	for (; iter != endIter; ++iter)
	{
		DnSkillHandle hSkill = (*iter).hSkill;
		if (!hSkill/* || CDnSkill::UsingResult::Success != hSkill->CanExecute()*/)
			continue;

		//Skill 쿨타임 시작?
		hSkill->OnBeginCoolTime();
		m_PrefixSkillCoolTimeManager.AddCoolTime(hSkill);

		int nStateEffectCount = hSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( DWORD k = 0; k < hSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = hSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				hApplyActor = m_pActor->GetActorHandle();
				break;

			case CDnSkill::ApplyTarget: // 타겟한테
				continue;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				continue;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				continue;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);

			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}

		//사용된 방어 스킬은 리스트에 담아 놓는다..
		m_listProcessPreFixOffenceSkills.push_back(hSkill);
	}
}

void MASkillUser::ApplyPrefixOffenceSkillToTarget(DnActorHandle hTarget)
{
	// 최종 리스트에 담긴 스킬중에서 우선순위가 가장 높은 리스트에 저장된 스킬만 사용한다.
	if (m_listProcessPreFixOffenceSkills.empty())
		return;

	list<DnSkillHandle>::iterator iter = m_listProcessPreFixOffenceSkills.begin();
	list<DnSkillHandle>::iterator endIter = m_listProcessPreFixOffenceSkills.end();
	
	for (; iter != endIter; ++iter)
	{
		DnSkillHandle hSkill = (*iter);
		if (!hSkill)
			continue;

		int nStateEffectCount = hSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( DWORD k = 0; k < hSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = hSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				continue;
			case CDnSkill::ApplyTarget: // 타겟한테
				hApplyActor = hTarget;
				break;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				if( m_pActor->GetTeam() == hTarget->GetTeam() )
					continue;
				else
					hApplyActor = hTarget;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				if( m_pActor->GetTeam() != hTarget->GetTeam() )
					continue;
				else
					hApplyActor = hTarget;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;
			
			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);

			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}
	}
}

void MASkillUser::CalculateProbabilitySkill(PREFIX_SYSTEM_SKILL_LIST &candidateSkills,
											PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList)
{
	//후보 스킬 리스트 초기화
	candidateSkills.clear();

	PREFIX_SYSTEM_SKILL_COLLECTION::iterator iter = sameSkillList.begin();
	PREFIX_SYSTEM_SKILL_COLLECTION::iterator endIter = sameSkillList.end();

	for (; iter != endIter; ++iter)
	{
		//같은 스킬ID를 가진 스킬들 확률을 합산한다.
		PRESIX_SYSTEM_SKILLS::iterator skillIter = iter->second.begin();
		PRESIX_SYSTEM_SKILLS::iterator skillEndIter = iter->second.end();

		float fProbability = 0.0f;
		DnSkillHandle hSelectedSkill;
		int nSlotIndex = -1;

		for (; skillIter != skillEndIter; ++skillIter)
		{
			if (skillIter->hSkill)
			{
				fProbability += skillIter->fRatio;
				
				// 해당 스킬
				if (!hSelectedSkill)
				{
					hSelectedSkill = skillIter->hSkill;
					nSlotIndex = skillIter->slotIndex;
				}
				else
				{
					if (hSelectedSkill->GetLevel() < skillIter->hSkill->GetLevel())
					{
						hSelectedSkill = skillIter->hSkill;
						nSlotIndex = skillIter->slotIndex;
					}
				}
			}
		}

		// 선택된 스킬이 있으면 최종 스킬 후보 리스트에 추가한다.
		if (hSelectedSkill && -1 != nSlotIndex && 0.0f != fProbability)
		{
			int nSkillID = hSelectedSkill->GetClassID();
			
			PREFIX_SYSTEM_SKILLINFO skillInfo;
			skillInfo.fRatio = fProbability;
			skillInfo.hSkill = hSelectedSkill;
			skillInfo.slotIndex = nSlotIndex;

			OutputDebug("같은 스킬(%d) 합산 최종 확률 (%f) 슬롯 Index(%d)로 결정됨!!!!!\n", skillInfo.hSkill->GetClassID(), skillInfo.fRatio, skillInfo.slotIndex);

			candidateSkills.insert(PREFIX_SYSTEM_SKILL_LIST::value_type(nSkillID, skillInfo));
		}
	}
}

void MASkillUser::GroupingPrefixDefenceSkillsBySameSkillID(PREFIX_SYSTEM_SKILL_LIST &skillList, PREFIX_SYSTEM_SKILL_COLLECTION &sameSkillList)
{
	PREFIX_SYSTEM_SKILL_LIST::iterator iter = skillList.begin();
	PREFIX_SYSTEM_SKILL_LIST::iterator endIter = skillList.end();

	for (; iter != endIter; ++iter)
	{
		int nSkillID = -1;
		int nSlotIndex = iter->first;	//슬롯 인덱스

		PREFIX_SYSTEM_SKILLINFO skillInfo = iter->second;

		if (!skillInfo.hSkill)
			continue;

		nSkillID = skillInfo.hSkill->GetClassID();

		PREFIX_SYSTEM_SKILL_COLLECTION::iterator findIter = sameSkillList.find(nSkillID);
		
		if (findIter != sameSkillList.end())
		{
			//이미 같은 스킬ID를 가진 스킬이 리스트에 있다면
			
			
			skillInfo.slotIndex = nSlotIndex;
			
			findIter->second.push_back(skillInfo);
		}
		else
		{
			//아직 스킬 등록이 되어 있지 않다면 리스트를 만들어 등록..
			PRESIX_SYSTEM_SKILLS skillList;
			
			skillInfo.slotIndex = nSlotIndex;
			skillList.push_back(skillInfo);

			sameSkillList.insert(PREFIX_SYSTEM_SKILL_COLLECTION::value_type(nSkillID, skillList));
		}
	}
}

void MASkillUser::GroupingSkillByPriority(PREFIX_SYSTEM_SKILL_COLLECTION& prefixSystemSkills, PREFIX_SYSTEM_SKILL_LIST& tempCandiateSkills, float fHitDamageProb/* = 1.0f*/)
{
	prefixSystemSkills.clear();

	PREFIX_SYSTEM_SKILL_LIST::iterator iter = tempCandiateSkills.begin();
	PREFIX_SYSTEM_SKILL_LIST::iterator endIter = tempCandiateSkills.end();

	//대표 스킬 리스트를 순회 하면서 우선 순위별로 그룹핑을 한다.
	for (; iter != endIter; ++iter)
	{
		int nPriority = 0;

		PREFIX_SYSTEM_SKILLINFO skillInfo = iter->second;

		if (!skillInfo.hSkill)
			continue;

		nPriority = skillInfo.hSkill->GetPrefixSystemPriority();
		
		//스킬 쿨타임 확인...
		//if (skillInfo.hSkill->GetCoolTime() > 0.0f && skillInfo.hSkill->GetElapsedDelayTime() > 0.0f)
		if (m_PrefixSkillCoolTimeManager.IsCoolTime(skillInfo.hSkill->GetClassID()))
		{
			OutputDebug("스킬(%d) 쿨타임 중.........!!!!\n", skillInfo.hSkill->GetClassID());
			continue;
		}

		//확률계산 해서 통과된 스킬만 리스트에 추가한다...
		bool bExecuteable = rand() % 10000 <= (iter->second.fRatio * fHitDamageProb);
		if (!bExecuteable)
		{
			OutputDebug("스킬(%d) 확률에서 걸러짐...!!!!\n", skillInfo.hSkill->GetClassID());
			continue;
		}

		OutputDebug("우선순위 (%d)에 스킬(%d) 등록됨!!!!\n", nPriority, skillInfo.hSkill->GetClassID());

		PREFIX_SYSTEM_SKILL_COLLECTION::iterator findIter = prefixSystemSkills.find(nPriority);
		if (findIter != prefixSystemSkills.end())
		{
			//이미 우선순위 정보가 등록 되어 있다면 
			//우선순위 리스트에 스킬 정보를 추가한다.
			findIter->second.push_back(skillInfo);
		}
		else
		{
			//아직 우선순위가 등록이 되어 있지 않다면..
			//스킬 저장할 리스트 만들고
			//스킬 정보 설정 해서 리스트에 추가한다.
			PRESIX_SYSTEM_SKILLS skillList;
			skillList.push_back(skillInfo);

			prefixSystemSkills.insert(PREFIX_SYSTEM_SKILL_COLLECTION::value_type(nPriority, skillList));
		}
	}

}

void MASkillUser::EndPrefixSystemSkill( LOCAL_TIME LocalTime, float fDelta )
{
	list<DnSkillHandle>::iterator iter = m_listProcessPreFixDefenceSkills.begin();
	for( iter; iter != m_listProcessPreFixDefenceSkills.end(); ++iter )
	{
		DnSkillHandle hAutoPassive = *iter;
		hAutoPassive->OnEnd( LocalTime, fDelta );
	}

	iter = m_listProcessPreFixOffenceSkills.begin();
	for( iter; iter != m_listProcessPreFixOffenceSkills.end(); ++iter )
	{
		DnSkillHandle hAutoPassive = *iter;
		hAutoPassive->OnEnd( LocalTime, fDelta );
	}
}

void MASkillUser::InitPrefixOffenceSkills()
{
	m_PrefixSystemCandiateOffenceSkills.clear();
}

bool MASkillUser::IsPrefixTriggerSkill()
{
	if (!m_hProcessSkill)
		return false;
	
	return m_hProcessSkill->IsPrefixTriggerSkill();
}

#endif // _GAMESERVER


void MASkillUser::UpdateSkillLevelUpInfo()
{
	std::map<int, SkillLevelUpInfo>::iterator iter = m_SkillLevelUpInfoList.begin();
	std::map<int, SkillLevelUpInfo>::iterator endIter = m_SkillLevelUpInfoList.end();

	m_SkillLevelUpInfo.clear();

	for (; iter != endIter; ++iter)
	{
		SkillLevelUpInfo& skillLevelUpInfo = iter->second;

		//스킬 ID / 레벨업 값이 정상이라면
		if (skillLevelUpInfo.nSkillID != 0 && skillLevelUpInfo.nLevelUp != 0)
		{
			//스킬 레벨업 정보 리스트에서 같은 스킬 아이디를 찾는다.
			std::map<int, int>::iterator findIter = m_SkillLevelUpInfo.find(skillLevelUpInfo.nSkillID);
			if (findIter != m_SkillLevelUpInfo.end())
			{
				// 기존 레벨업 값이 더 크다면 건너뜀.
				if (findIter->second > skillLevelUpInfo.nLevelUp)
					continue;
				
				// 기존 레벨업 값이 크지 않으면 리스트에서 제거 한다..
				m_SkillLevelUpInfo.erase(findIter);
			}

			// 새로운 스킬 레벨업 정보를 저장한다.
			m_SkillLevelUpInfo.insert(std::make_pair(skillLevelUpInfo.nSkillID, skillLevelUpInfo.nLevelUp));
		}
	}
}

void MASkillUser::AddSkillLevelUpInfo(int nSlotIndex, int nSkillID, int nLevelUp)
{
	DnSkillHandle hSkill = FindSkill(nSkillID);
	if( hSkill )
	{
		if (hSkill->GetElapsedDelayTime() > 0.0f || hSkill->IsToggleOn())
			return;
	}

	SkillLevelUpInfo skillLevelUpInfo;
	skillLevelUpInfo.nSkillID = nSkillID;
	skillLevelUpInfo.nLevelUp = nLevelUp;

	m_SkillLevelUpInfoList.insert(std::make_pair(nSlotIndex, skillLevelUpInfo));

	//스킬 레벨업 정보 갱신..
	UpdateSkillLevelUpInfo();

	int nSkillLevelUp = GetSkillLevelUpValue(nSkillID);
	SkillLevelUp(nSkillID, nSkillLevelUp);
}

void MASkillUser::RemoveSkillLevelUpInfo(int nSlotIndex)
{
	std::map<int, SkillLevelUpInfo>::iterator findIter = m_SkillLevelUpInfoList.find(nSlotIndex);
	if (findIter != m_SkillLevelUpInfoList.end())
	{
		//지워질 레벨업 아이템에 적용된 스킬 ID를 받아 놓는다.
		int nSkillID = findIter->second.nSkillID;

		DnSkillHandle hSkill = FindSkill(nSkillID);
		if( hSkill )
		{
			if (hSkill->GetElapsedDelayTime() > 0.0f || hSkill->IsToggleOn())
				return;
		}

		//레벨업 정보를 리스트에서 지우고
		m_SkillLevelUpInfoList.erase(findIter);

		//레벨업 정보 갱신
		UpdateSkillLevelUpInfo();

		//레벨업 정보가 없어지므로 
		int nLevelUp = GetSkillLevelUpValue(nSkillID);
		SkillLevelUp(nSkillID, nLevelUp);
	}
}

int MASkillUser::GetSkillLevelUpValue(int nSkillID)
{
	int nLevelUpValue = 0;

	int nLevelUpValueByNormalItem = 0;
	std::map<int, int>::iterator findNormalIter = m_SkillLevelUpInfo.find(nSkillID);
	if (findNormalIter != m_SkillLevelUpInfo.end())
		nLevelUpValueByNormalItem = findNormalIter->second;

	int nLevelUpValueByCashItem = 0;
	std::map<int, int>::iterator findCashIter = m_CashSkillLevelUpInfo.find(nSkillID);
	if (findCashIter != m_CashSkillLevelUpInfo.end())
		nLevelUpValueByCashItem = findCashIter->second;

	//캐시아이템의 레벨업 제한 값은 1
	if (nLevelUpValueByCashItem > 1)
		nLevelUpValueByCashItem = 1;

	//레벨업 아이템은 중복 되지 않고, 수차가 높은 값을 적용 하도록 한다.
	nLevelUpValue = max(nLevelUpValueByNormalItem, nLevelUpValueByCashItem);

	return nLevelUpValue;
}

bool MASkillUser::ExistSkillLevelUpValue()
{
	if( m_SkillLevelUpInfo.empty() && m_CashSkillLevelUpInfo.empty() )
		return false;

	return true;
}

void MASkillUser::SkillLevelUp(int nSkillID, int nLevelUp)
{
	//기존 스킬을 찾아서 스킬이 있고, 초기화 된 스킬이 아니면 레벨업 시킨다.
	DnSkillHandle hSkill = FindSkill(nSkillID);
	if (!hSkill)
		return;

	int nCurentSkillLevel = hSkill->GetLevel();
	int nCurLevlUpValue = hSkill->GetLevelUpValue();

	bool bChangeLevel = true;

	if (nCurentSkillLevel == 0)
		bChangeLevel = false;

	//새로 레벨업 할 수치와 현재 레벨업이 적용된 수치가 같으면 스킬을 변경할 필요가 없음.
	if (nCurLevlUpValue == nLevelUp)
		bChangeLevel = false;

	if (bChangeLevel)
	{
		//SetSkillLevel내부 AddSkill에서 원래 레벨에 지금의 증가치가 계산되어 스킬이 생성됨..
		int nOrigLevel = nCurentSkillLevel - nCurLevlUpValue;
		ChangeSkillLevelUp(nSkillID, nOrigLevel);
	}
#if defined(_GAMESERVER)
#else
	else
		hSkill->DisableSkillByItemMove(false);
#endif // _GAMESERVER
}

void MASkillUser::ChangeSkillLevelUp(int nSkillID, int nOrigLevel)
{
	RemoveSkill(nSkillID);
	AddSkill(nSkillID, nOrigLevel);
}



//	std::map<int, SkillLevelUpInfo> m_CashSkillLevelUpInfoList;

void MASkillUser::AddSkillLevelUpInfoByCashItem(int nSlotIndex, int nSkillID, int nLevelUp)
{
	SkillLevelUpInfo skillLevelUpInfo;
	skillLevelUpInfo.nSkillID = nSkillID;
	skillLevelUpInfo.nLevelUp = nLevelUp;

	m_CashSkillLevelUpInfoList.insert(std::make_pair(nSlotIndex, skillLevelUpInfo));

	//스킬 레벨업 정보 갱신..
	UpdateSkillLevelUpInfoByCashItem();

	int nSkillLevelUp = GetSkillLevelUpValue(nSkillID);
	SkillLevelUp(nSkillID, nSkillLevelUp);
}

void MASkillUser::UpdateSkillLevelUpInfoByCashItem()
{
	std::map<int, SkillLevelUpInfo>::iterator iter = m_CashSkillLevelUpInfoList.begin();
	std::map<int, SkillLevelUpInfo>::iterator endIter = m_CashSkillLevelUpInfoList.end();

	m_CashSkillLevelUpInfo.clear();

	for (; iter != endIter; ++iter)
	{
		SkillLevelUpInfo& skillLevelUpInfo = iter->second;

		//스킬 ID / 레벨업 값이 정상이라면
		if (skillLevelUpInfo.nSkillID != 0 && skillLevelUpInfo.nLevelUp != 0)
		{
			//스킬 레벨업 정보 리스트에서 같은 스킬 아이디를 찾는다.
			std::map<int, int>::iterator findIter = m_CashSkillLevelUpInfo.find(skillLevelUpInfo.nSkillID);
			if (findIter != m_CashSkillLevelUpInfo.end())
			{
				// 기존 레벨업 값이 더 크다면 건너뜀.
				if (findIter->second > skillLevelUpInfo.nLevelUp)
					continue;

				// 기존 레벨업 값이 크지 않으면 리스트에서 제거 한다..
				m_CashSkillLevelUpInfo.erase(findIter);
			}

			// 새로운 스킬 레벨업 정보를 저장한다.
			m_CashSkillLevelUpInfo.insert(std::make_pair(skillLevelUpInfo.nSkillID, skillLevelUpInfo.nLevelUp));
		}
	}
}

void MASkillUser::RemoveSkillLevelUpInfoByCashItem(int nSlotIndex)
{
	std::map<int, SkillLevelUpInfo>::iterator findIter = m_CashSkillLevelUpInfoList.find(nSlotIndex);
	if (findIter != m_CashSkillLevelUpInfoList.end())
	{
		//지워질 레벨업 아이템에 적용된 스킬 ID를 받아 놓는다.
		int nSkillID = findIter->second.nSkillID;

		//레벨업 정보를 리스트에서 지우고
		m_CashSkillLevelUpInfoList.erase(findIter);

		//레벨업 정보 갱신
		UpdateSkillLevelUpInfoByCashItem();

		//레벨업 정보가 없어지므로 
		int nLevelUp = GetSkillLevelUpValue(nSkillID);
		SkillLevelUp(nSkillID, nLevelUp);
	}
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
#if defined(_GAMESERVER)
void MASkillUser::AddPrefixDefenceSkill(DnSkillHandle hSkill)
{
	if (!hSkill)
		return;
	
	//이 스킬의 접두사 스킬 타입(행운/파괴....)
	int nPrefixSkillType = hSkill->GetPrefixSkillType();
	PREFIX_SKILL_INFO::iterator findIter = m_PrefixDefenceSkills.find(nPrefixSkillType);

	//기존 카테고리가 있다면
	if (findIter != m_PrefixDefenceSkills.end())
	{
		//기존 접두사 스킬 관리 객체에 스킬을 추가 하고, 정보를 갱신한다.
		findIter->second->AddSkill(hSkill);
	}
	else
	{
		//아직 등록된 접두사 스킬이 없다면 새로 만들어서 등록 한다.
		CDnPrefixSkill* pPrefixSkill = new CDnPrefixSkill(nPrefixSkillType);
		pPrefixSkill->AddSkill(hSkill);

		m_PrefixDefenceSkills.insert(make_pair(nPrefixSkillType, pPrefixSkill));
	}
}

void MASkillUser::RemovePrefixDefenceSkill(DnSkillHandle hSkill)
{
	//이 스킬의 접두사 스킬 타입(행운/파괴....)
	int nPrefixSkillType = hSkill->GetPrefixSkillType();
	PREFIX_SKILL_INFO::iterator findIter = m_PrefixDefenceSkills.find(nPrefixSkillType);

	//기존 카테고리가 있다면
	if (findIter != m_PrefixDefenceSkills.end())
	{
		//기존 접두사 스킬 관리 객체에서 스킬을 제거 한다.
		findIter->second->RemoveSkill(hSkill);

		//등록된 스킬이 없으면 리스트에서 제거 한다.
		if (findIter->second->GetSkillCount() == 0)
		{
			SAFE_DELETE(findIter->second);
			m_PrefixDefenceSkills.erase(findIter);
		}
	}
}

void MASkillUser::ProcessPrefixDefenceSkill_New(DnActorHandle hHitter)
{
	//접두어 시스템 방어용 스킬 등록되 되어 있지 않다면	
	if (m_PrefixDefenceSkills.empty())
		return;

	OutputDebug("접두어 방어용 스킬 시작-----------------------------!!\n");

	//접두사 스킬 발동 확률 계산..
	PREFIX_SKILL_INFO::iterator iter = m_PrefixDefenceSkills.begin();
	PREFIX_SKILL_INFO::iterator endIter = m_PrefixDefenceSkills.end();
	for (; iter != endIter; ++iter)
	{
		CDnPrefixSkill* pPrefixSkill = iter->second;
		if (!pPrefixSkill)
			continue;

		//접두사 스킬이 쿨타임 중인지 확인한다..
		if (m_PrefixSkillCoolTimeManager.IsCoolTime(pPrefixSkill->GetSkillType()))
		{
			OutputDebug("접두사 방어 스킬(%d) 쿨타임 중!!!!!!!!!!\n", pPrefixSkill->GetSkillType());
			continue;
		}

		//접두사 스킬 발동 확률 확인.
		bool bExecuteable = rand() % 10000 <= pPrefixSkill->GetProbability();
		if (!bExecuteable)
		{
			OutputDebug("접두사 방어 스킬(%d) 확률에서 걸러짐...!!!!\n", pPrefixSkill->GetSkillType());
			continue;
		}

		//대표 스킬
		DnSkillHandle hSkill = pPrefixSkill->GetSkillHandle();
		if (!hSkill)
			continue;

		hSkill->OnBeginCoolTime();
		//Skill 쿨타임 시작?
		m_PrefixSkillCoolTimeManager.AddCoolTime(pPrefixSkill);
		
		int nStateEffectCount = pPrefixSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( int k = 0; k < pPrefixSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = pPrefixSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				hApplyActor = m_pActor->GetActorHandle();
				break;

			case CDnSkill::ApplyTarget: // 타겟한테
				hApplyActor = hHitter;
				break;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				if( m_pActor->GetTeam() == hHitter->GetTeam() )
					continue;
				else
					hApplyActor = hHitter;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				if( m_pActor->GetTeam() != hHitter->GetTeam() )
					continue;
				else
					hApplyActor = hHitter;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);
			
			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				OutputDebug("접두사 방어 스킬(%d) 상태효과 (%d)-(%s)\n", pPrefixSkill->GetSkillType(), pLocalStruct->nID, pLocalStruct->szValue.c_str());

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}

		//사용된 방어 스킬은 리스트에 담아 놓는다..
		m_listProcessPreFixDefenceSkills.push_back(hSkill);
	}

	OutputDebug("접두어 방어용 스킬 끝  -----------------------------!!\n");
}


void MASkillUser::AddPrefixOffenceSkill(DnSkillHandle hSkill)
{
	if (!hSkill)
		return;

	//이 스킬의 접두사 스킬 타입(행운/파괴....)
	int nPrefixSkillType = hSkill->GetPrefixSkillType();
	PREFIX_SKILL_INFO::iterator findIter = m_PrefixOffenceSkills.find(nPrefixSkillType);

	//기존 카테고리가 있다면
	if (findIter != m_PrefixOffenceSkills.end())
	{
		//기존 접두사 스킬 관리 객체에 스킬을 추가 하고, 정보를 갱신한다.
		findIter->second->AddSkill(hSkill);
	}
	else
	{
		//아직 등록된 접두사 스킬이 없다면 새로 만들어서 등록 한다.
		CDnPrefixSkill* pPrefixSkill = new CDnPrefixSkill(nPrefixSkillType);
		pPrefixSkill->AddSkill(hSkill);

		m_PrefixOffenceSkills.insert(make_pair(nPrefixSkillType, pPrefixSkill));
	}
}

void MASkillUser::RemovePrefixOffenceSkill(DnSkillHandle hSkill)
{
	//이 스킬의 접두사 스킬 타입(행운/파괴....)
	int nPrefixSkillType = hSkill->GetPrefixSkillType();
	PREFIX_SKILL_INFO::iterator findIter = m_PrefixOffenceSkills.find(nPrefixSkillType);

	//기존 카테고리가 있다면
	if (findIter != m_PrefixOffenceSkills.end())
	{
		//기존 접두사 스킬 관리 객체에서 스킬을 제거 한다.
		findIter->second->RemoveSkill(hSkill);

		//등록된 스킬이 없으면 리스트에서 제거 한다.
		if (findIter->second->GetSkillCount() == 0)
		{
			RemoveProcessPrefixOffenceSkill(findIter->second);

			SAFE_DELETE(findIter->second);
			m_PrefixOffenceSkills.erase(findIter);
		}
	}
}

//덤프 관련해서(한 프레임에서 동작해서 이럴일은 없을듯 한데..) 공격용 접미사 스킬이 제거 될때
//실행되고 있는 접미사 스킬 리스트에 같은 스킬이 있으면 제거 하도록한다.
void MASkillUser::RemoveProcessPrefixOffenceSkill(CDnPrefixSkill* pRemovePrefixSkill)
{
	if (NULL == pRemovePrefixSkill)
		return;

	list<CDnPrefixSkill*>::iterator iter = m_ProcessPrefixOffenceSkills.begin();
	
	for (; iter != m_ProcessPrefixOffenceSkills.end(); )
	{
		CDnPrefixSkill* pPrefixSkill = (*iter);
		if (pPrefixSkill && pPrefixSkill == pRemovePrefixSkill)
		{
			iter = m_ProcessPrefixOffenceSkills.erase(iter);
			continue;
		}

		++iter;
	}
}

void MASkillUser::ProcessPrefixOffenceSkill_New()
{
	//접두어 시스템 방어용 스킬 등록되 되어 있지 않다면	
	if (m_PrefixOffenceSkills.empty())
		return;

	//접두사 스킬 발동 전에 리스트를 비운다.
	m_ProcessPrefixOffenceSkills.clear();

	//접두사 스킬 발동 확률 계산..
	PREFIX_SKILL_INFO::iterator iter = m_PrefixOffenceSkills.begin();
	PREFIX_SKILL_INFO::iterator endIter = m_PrefixOffenceSkills.end();
	for (; iter != endIter; ++iter)
	{
		CDnPrefixSkill* pPrefixSkill = iter->second;
		if (!pPrefixSkill)
			continue;

		//접두사 스킬이 쿨타임 중인지 확인한다..(무기/방어구 구분이 된다??)
		if (m_PrefixSkillCoolTimeManager.IsCoolTime(pPrefixSkill->GetSkillType()))
		{
			OutputDebug("접두사 공격 스킬(%d) 쿨타임 중!!!!!!!!!!\n", pPrefixSkill->GetSkillType());
			continue;
		}

		//접두사 스킬 발동 확률 확인.
		bool bExecuteable = rand() % 10000 <= pPrefixSkill->GetProbability();
		if (!bExecuteable)
		{
			OutputDebug("접두사 공격 스킬(%d) 확률에서 걸러짐...!!!!\n", pPrefixSkill->GetSkillType());
			continue;
		}

		//대표 스킬
		DnSkillHandle hSkill = pPrefixSkill->GetSkillHandle();
		if (!hSkill)
			continue;

		//Skill 쿨타임 시작?
		hSkill->OnBeginCoolTime();
		m_PrefixSkillCoolTimeManager.AddCoolTime(pPrefixSkill);

		int nStateEffectCount = pPrefixSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( int k = 0; k < pPrefixSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = pPrefixSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				hApplyActor = m_pActor->GetActorHandle();
				break;

			case CDnSkill::ApplyTarget: // 타겟한테
				continue;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				continue;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				continue;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);

			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				//
				OutputDebug("접두사 공격 스킬(%d) 상태효과 자신한테 적용 (%d)-(%s)\n", pPrefixSkill->GetSkillType(), pLocalStruct->nID, pLocalStruct->szValue.c_str());

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}

		//사용된 방어 스킬은 리스트에 담아 놓는다..
		m_listProcessPreFixOffenceSkills.push_back(hSkill);

		//지금 발동된 공격용 접두사 스킬을 담아 놓는다.
		m_ProcessPrefixOffenceSkills.push_back(pPrefixSkill);
	}
}

void MASkillUser::ApplyPrefixOffenceSkillToTarget_New(DnActorHandle hTarget)
{
	// 최종 리스트에 담긴 스킬중에서 우선순위가 가장 높은 리스트에 저장된 스킬만 사용한다.
	if (m_ProcessPrefixOffenceSkills.empty())
		return;

	list<CDnPrefixSkill*>::iterator iter = m_ProcessPrefixOffenceSkills.begin();
	list<CDnPrefixSkill*>::iterator endIter = m_ProcessPrefixOffenceSkills.end();

	for (; iter != endIter; ++iter)
	{
		CDnPrefixSkill* pPrefixSkill = (*iter);
		if (!pPrefixSkill)
			continue;

		DnSkillHandle hSkill = pPrefixSkill->GetSkillHandle();
		if (!hSkill)
			continue;

		int nStateEffectCount = pPrefixSkill->GetStateEffectCount();
		// 해당 스킬의 상태 효과별 적용 액터를 결정해서 중복 처리 확인하고,
		// 액터별로 상태 효과 적용을 해야 할듯 한데...
		for( int k = 0; k < pPrefixSkill->GetStateEffectCount(); k++ ) 
		{
			CDnSkill::StateEffectStruct *pLocalStruct = pPrefixSkill->GetStateEffectFromIndex(k);

			DnActorHandle hApplyActor;

			// 상태 효과 적용 타겟 타입에 따라 적용될 액터를 결정한다.
			switch( pLocalStruct->ApplyType )
			{
			case CDnSkill::ApplySelf:	// 자신한테
				continue;
			case CDnSkill::ApplyTarget: // 타겟한테
				hApplyActor = hTarget;
				break;

			case CDnSkill::ApplyEnemy:	// 적한테.. (같은 편이 아니면 나를 때린 놈이 적이다..)
				if( m_pActor->GetTeam() == hTarget->GetTeam() )
					continue;
				else
					hApplyActor = hTarget;
				break;

			case CDnSkill::ApplyFriend: // 같은 편.. (같은 편이면 나를 때린 놈이 같은 편..?)
				if( m_pActor->GetTeam() != hTarget->GetTeam() )
					continue;
				else
					hApplyActor = hTarget;
				break;
			}

			// 상태효과중 지속 시간이 0이고 자신에게 거는 상태 효과는
			// 스킬의 m_vlApplyNoPacketStateEffectList에 담아 놓고 스킬 끝날때
			// 지워 지도록 한다...[2011/01/13 semozz]
			bool isSelfPermentStateEffect = false;
			if (CDnSkill::ApplySelf == pLocalStruct->ApplyType && 
				(0 == pLocalStruct->nDurationTime ||
				-1 == pLocalStruct->nDurationTime))
			{
				pLocalStruct->nDurationTime = -1;
				isSelfPermentStateEffect = true;
			}

			CDnStateBlow *pStateBlow = hApplyActor->GetStateBlow();
			CDnSkill::CanApply eResult = CDnSkill::CanApply::Apply;

			if (pStateBlow)
				eResult = pStateBlow->CanApplySkillStateEffect(hSkill->GetInfo(), *pLocalStruct);

			if (CDnSkill::CanApply::Fail != eResult)
			{
				hApplyActor->RemoveResetStateBlow();

				int nBlowID = hApplyActor->CmdAddStateEffect( hSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pLocalStruct->nID, 
					pLocalStruct->nDurationTime, pLocalStruct->szValue.c_str(), false, false );

				OutputDebug("접두사 공격 스킬(%d) 상태효과 상대에 적용 (%d)-(%s)\n", pPrefixSkill->GetSkillType(), pLocalStruct->nID, pLocalStruct->szValue.c_str());

				if (isSelfPermentStateEffect)
					hSkill->AddPrefixBlow(hApplyActor, nBlowID);
			}

			hApplyActor->InitStateBlowIDToRemove();
		}
	}

	//덤프 관련 의심 코드 정리
	//적용하고 리스트를 비운다..
	m_ProcessPrefixOffenceSkills.clear();
}

#endif // _GAMESERVER
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


void MASkillUser::SkillToggle(DnSkillHandle hSkill, bool isOn)
{
	if (!hSkill)	return;

#if defined(_CLIENT)
	CDnSkill::DurationTypeEnum eDurationType = hSkill->GetDurationType();
	if (eDurationType == CDnSkill::DurationTypeEnum::SummonOnOff)
	{
		hSkill->SummonMonsterOff();
	}
#endif // _CLIENT

	int skillID = hSkill->GetClassID();
	TOGGLESKILLLIST::iterator findIter = m_ToggleOnSkillList.find(skillID);
	
	if (isOn)
	{
		//이미 스킬 등록되어 있음..
		if (findIter != m_ToggleOnSkillList.end())
			return;

		m_ToggleOnSkillList.insert(std::make_pair(skillID, hSkill));
	}
	else
	{
		//등록된 스킬이 없다면
		if (findIter == m_ToggleOnSkillList.end())
			return;

		m_ToggleOnSkillList.erase(findIter);
	}
}


void MASkillUser::SummonOnOffSkillInit(DWORD dwSummonMonsterID)
{
	DNVector(DnSkillHandle) summonOnOffSkills;
#if defined(PRE_FIX_SKILLLIST)
	for( DWORD i=0; i<m_vlSkillObjects.size(); i++ ) 
	{
		DnSkillHandle hSkill = m_vlSkillObjects[i].hSkill;
		if (hSkill && hSkill->GetDurationType() == CDnSkill::DurationTypeEnum::SummonOnOff)
			summonOnOffSkills.push_back(hSkill);
	}
#endif // PRE_FIX_SKILLLIST

	for (DWORD i =0; i < summonOnOffSkills.size(); ++i)
	{
		DnSkillHandle hSkill = summonOnOffSkills[i];
		if (hSkill && hSkill->GetSummonMonsterID() == dwSummonMonsterID)
			hSkill->EnableToggle(false);
	}
}


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
void MASkillUser::UpdateGlobalCoolTime(float fRate)
{
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		if(m_vlSkillObjects.at( i ).hSkill)
			m_vlSkillObjects.at( i ).hSkill->UpdateGlobalCoolTime(fRate);
	}
}

void MASkillUser::ResetGlobalCoolTime(float fRate)
{
	for( DWORD i = 0; i < m_vlSkillObjects.size(); ++i )
	{
		if(m_vlSkillObjects.at( i ).hSkill)
			m_vlSkillObjects.at( i ).hSkill->ResetGlobalCoolTime(fRate);
	}
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL