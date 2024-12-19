
#include "stdafx.h"
#include "MAAiSkill.h"
#include "DnMonsterActor.h"
#include "DnSkill.h"
#include "DnBlow.h"
#include "MAAiChecker.h"
#include "MAAiScript.h"
#include "MAScanner.h"
#include "DnStateBlow.h"
#if defined( PRE_FIX_67719 )
#include "DNAggroSystem.h"
#endif


#if defined(PRE_ADD_64990)
void MonsterSkillAI::CheckSkillTable(lua_State* pLua, vector<string>& skillTableName)
{
	string tableName = "g_Lua_Skill";
	lua_tinker::table legacySkillTable = lua_tinker::get<lua_tinker::table>(pLua, tableName.c_str());
	if( legacySkillTable.m_obj->m_pointer )
		skillTableName.push_back(tableName);
	else
		skillTableName.push_back("");

	tableName = "g_Lua_Skill_PvE";
	lua_tinker::table SkillTable_PVE = lua_tinker::get<lua_tinker::table>(pLua, tableName.c_str());
	if( SkillTable_PVE.m_obj->m_pointer )
		skillTableName.push_back(tableName);
	else
		skillTableName.push_back("");


	tableName = "g_Lua_Skill_PvP";
	lua_tinker::table SkillTable_PVP = lua_tinker::get<lua_tinker::table>(pLua, tableName.c_str());
	if( SkillTable_PVP.m_obj->m_pointer )
		skillTableName.push_back(tableName);
	else
		skillTableName.push_back("");
}

bool MonsterSkillAI::LoadSkillTable(lua_State* pLua, OUT std::vector<MonsterSkillTable>&	_SkillTable, string& tableName)
{
	// 스킬 테이블이 있다면.
	lua_tinker::table SkillTable = lua_tinker::get<lua_tinker::table>(pLua, tableName.c_str());
	{
		bool bHave = false;
		lua_tinker::table t = SkillTable.get<lua_tinker::table>(1);
		int __nSkillIndex = t.get<int>("skill_index");

		if ( __nSkillIndex == 0 )
		{
			bHave = false;
		}
		else
		{
			bHave = true;
		}

		if ( bHave )
		{
			for ( int j = 1 ; j <= SkillTable.getSize() ; j++ )
			{
				lua_tinker::table LocalT = SkillTable.get<lua_tinker::table>(j);

				int nSkillIndex= LocalT.get<int>("skill_index");
				if ( nSkillIndex == 0 )
					return false;

				MonsterSkillTable _skilltable;

				_skilltable.nSkillIndex		= nSkillIndex;
				_skilltable.iLuaSkillIndex	= (j-1);
				_skilltable.nCooltime		= LocalT.get<int>("cooltime");
				_skilltable.nRangeMin		= LocalT.get<int>("rangemin");
				_skilltable.nRangeMax		= LocalT.get<int>("rangemax");
				_skilltable.nTargetType		= LocalT.get<int>("target");
				_skilltable.nCancelLook		= LocalT.get<int>("cancellook");
				_skilltable.iPriority		= LocalT.get<int>("priority");
#if defined( PRE_FIX_67719 )
				_skilltable.fRandomTarget	= LocalT.get<float>("randomtarget");
				const char* pszRandomTarget = LocalT.get<const char*>("randomtarget");
				if( pszRandomTarget )
				{
					std::string strString(pszRandomTarget);
					std::vector<std::string> vSplit;

					boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

					if( 1 < vSplit.size() )
					{						
						_skilltable.fRandomTarget = (float)atof(vSplit[0].c_str());
						_skilltable.nIgnoreAggroTarget = atoi(vSplit[1].c_str());
					}
				}
#endif				
#if defined( PRE_FIX_68096 )
				if( LocalT.get<const char*>("globalcooltime") )
				{
					_skilltable.iGlobalCoolTimeIndex[0] = LocalT.get<int>("globalcooltime")-1;
					_ASSERT( _skilltable.iGlobalCoolTimeIndex[0] >= 0 );
				}
				
				for( int i=1 ; i<ActionTableCommon::eCommon::MaxGlobalCoolTime ; ++i )
				{
					char szBuf[MAX_PATH];
					sprintf( szBuf, "globalcooltime%d", i+1 );
					_skilltable.iGlobalCoolTimeIndex[i] = LocalT.get<int>(szBuf)-1;					
					if(_skilltable.iGlobalCoolTimeIndex[i] == -1)
						break;						
				}
#else
				if( LocalT.get<const char*>("globalcooltime") )
				{
					_skilltable.iGlobalCoolTimeIndex = LocalT.get<int>("globalcooltime")-1;
					_ASSERT( _skilltable.iGlobalCoolTimeIndex >= 0 );
				}
#endif

				_skilltable.iNextLuaSkillIndex = -1;
				if( LocalT.get<const char*>("next_lua_skill_index") )
					_skilltable.iNextLuaSkillIndex = LocalT.get<int>("next_lua_skill_index");

#if defined (PRE_MOD_AIMULTITARGET)
				_skilltable.nMultipleTarget			= 0;
#else
				_skilltable.bMultipleTarget			= false;
#endif
				_skilltable.iMaxMultipleTargetCount	= 0;
				_skilltable.bIsExceptCannonPlayer	= false;

				const char* pAnyMultipleTarget = LocalT.get<const char*>("anymultipletarget");
				if( pAnyMultipleTarget )
					_skilltable.bIsAnyMultipleTarget = true;

				const char* pTemp = LocalT.get<const char*>( _skilltable.bIsAnyMultipleTarget ? "anymultipletarget" : "multipletarget" );
				if( pTemp )
				{
					std::string					strString( pTemp );
					std::vector<std::string>	vSplit;
					boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

#if defined (PRE_MOD_AIMULTITARGET)
					_skilltable.nMultipleTarget = boost::lexical_cast<int>(vSplit[0]);

					if( vSplit.size() > 1 )
						_skilltable.iMaxMultipleTargetCount = boost::lexical_cast<int>(vSplit[1]);

					if( vSplit.size() > 2 )
					{
						if( stricmp( vSplit[2].c_str(), "exceptcannon" ) == 0 )
							_skilltable.bIsExceptCannonPlayer = true;
					}

					if (vSplit.size() > 3)
						_skilltable.nSummonerTarget = boost::lexical_cast<int>(vSplit[3]);	

#else // #if defined (PRE_MOD_AIMULTITARGET)
					_skilltable.bMultipleTarget = boost::lexical_cast<int>(vSplit[0]) ? true : false;

					if( vSplit.size() == 2 )
					{
						_skilltable.iMaxMultipleTargetCount = boost::lexical_cast<int>(vSplit[1]);
					}
					else if( vSplit.size() == 3 )
					{
						if( stricmp( vSplit[2].c_str(), "exceptcannon" ) == 0 )
							_skilltable.bIsExceptCannonPlayer = true;
					}
#endif // #if defined (PRE_MOD_AIMULTITARGET)
				}

				// LoadChekcer
				_skilltable.pMAAiCheckerManager = new CMAAiSkillCheckerManager();
				if( !_skilltable.pMAAiCheckerManager->bLoadChecker( LocalT ) )
				{
					delete _skilltable.pMAAiCheckerManager;
					return false;
				}

				_SkillTable.push_back(_skilltable);
			}
		}
	}

	return true;
}
#endif // PRE_ADD_64990

bool MonsterSkillAI::LoadSkillTable(lua_State* pLua, OUT std::vector<MonsterSkillTable>&	_SkillTable)
{
//	bool bHave = false;

	// 스킬 테이블이 있다면.
	lua_tinker::table SkillTable = lua_tinker::get<lua_tinker::table>(pLua, "g_Lua_Skill");
	{
		bool bHave = false;
		lua_tinker::table t = SkillTable.get<lua_tinker::table>(1);
		int __nSkillIndex = t.get<int>("skill_index");

		if ( __nSkillIndex == 0 )
		{
			bHave = false;
		}
		else
		{
			bHave = true;
		}

		if ( bHave )
		{
			for ( int j = 1 ; j <= SkillTable.getSize() ; j++ )
			{
				lua_tinker::table LocalT = SkillTable.get<lua_tinker::table>(j);

				int nSkillIndex= LocalT.get<int>("skill_index");
				if ( nSkillIndex == 0 )
					return false;

				MonsterSkillTable _skilltable;

				_skilltable.nSkillIndex		= nSkillIndex;
				_skilltable.iLuaSkillIndex	= (j-1);
				_skilltable.nCooltime		= LocalT.get<int>("cooltime");
				_skilltable.nRangeMin		= LocalT.get<int>("rangemin");
				_skilltable.nRangeMax		= LocalT.get<int>("rangemax");
				_skilltable.nTargetType		= LocalT.get<int>("target");
				_skilltable.nCancelLook		= LocalT.get<int>("cancellook");
				_skilltable.iPriority		= LocalT.get<int>("priority");
#if defined( PRE_FIX_67719 )
				_skilltable.fRandomTarget	= LocalT.get<float>("randomtarget");
#endif

#if defined( PRE_FIX_68096 )
				if( LocalT.get<const char*>("globalcooltime") )
				{
					_skilltable.iGlobalCoolTimeIndex[0] = LocalT.get<int>("globalcooltime")-1;
					_ASSERT( _skilltable.iGlobalCoolTimeIndex[0] >= 0 );
				}				

				for( int i=1 ; i<ActionTableCommon::eCommon::MaxGlobalCoolTime ; ++i )
				{
					char szBuf[MAX_PATH];
					sprintf( szBuf, "globalcooltime%d", i+1 );
					_skilltable.iGlobalCoolTimeIndex[i] = LocalT.get<int>(szBuf)-1;					
					if(_skilltable.iGlobalCoolTimeIndex[i] == -1)
						break;						
				}
#else
				if( LocalT.get<const char*>("globalcooltime") )
				{
					_skilltable.iGlobalCoolTimeIndex = LocalT.get<int>("globalcooltime")-1;
					_ASSERT( _skilltable.iGlobalCoolTimeIndex >= 0 );
				}
#endif

				_skilltable.iNextLuaSkillIndex = -1;
				if( LocalT.get<const char*>("next_lua_skill_index") )
					_skilltable.iNextLuaSkillIndex = LocalT.get<int>("next_lua_skill_index");

#if defined (PRE_MOD_AIMULTITARGET)
				_skilltable.nMultipleTarget			= 0;
#else
				_skilltable.bMultipleTarget			= false;
#endif
				_skilltable.iMaxMultipleTargetCount	= 0;
				_skilltable.bIsExceptCannonPlayer	= false;

				const char* pAnyMultipleTarget = LocalT.get<const char*>("anymultipletarget");
				if( pAnyMultipleTarget )
					_skilltable.bIsAnyMultipleTarget = true;

				const char* pTemp = LocalT.get<const char*>( _skilltable.bIsAnyMultipleTarget ? "anymultipletarget" : "multipletarget" );
				if( pTemp )
				{
					std::string					strString( pTemp );
					std::vector<std::string>	vSplit;
					boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

#if defined (PRE_MOD_AIMULTITARGET)
					_skilltable.nMultipleTarget = boost::lexical_cast<int>(vSplit[0]);

					if( vSplit.size() > 1 )
						_skilltable.iMaxMultipleTargetCount = boost::lexical_cast<int>(vSplit[1]);

					if( vSplit.size() > 2 )
					{
						if( stricmp( vSplit[2].c_str(), "exceptcannon" ) == 0 )
							_skilltable.bIsExceptCannonPlayer = true;
					}

					if (vSplit.size() > 3)
						_skilltable.nSummonerTarget = boost::lexical_cast<int>(vSplit[3]);	

#else // #if defined (PRE_MOD_AIMULTITARGET)
					_skilltable.bMultipleTarget = boost::lexical_cast<int>(vSplit[0]) ? true : false;

					if( vSplit.size() == 2 )
					{
						_skilltable.iMaxMultipleTargetCount = boost::lexical_cast<int>(vSplit[1]);
					}
					else if( vSplit.size() == 3 )
					{
						if( stricmp( vSplit[2].c_str(), "exceptcannon" ) == 0 )
							_skilltable.bIsExceptCannonPlayer = true;
					}
#endif // #if defined (PRE_MOD_AIMULTITARGET)
				}

				// LoadChekcer
				_skilltable.pMAAiCheckerManager = new CMAAiSkillCheckerManager();
				if( !_skilltable.pMAAiCheckerManager->bLoadChecker( LocalT ) )
				{
					delete _skilltable.pMAAiCheckerManager;
					return false;
				}

				_SkillTable.push_back(_skilltable);
			}
		}
	}

	return true;
}

void MonsterSkillAI::Initialize(std::vector<MonsterSkillTable>* pSkilltable, DnActorHandle hActor)
{
	m_pSkillTable	= pSkilltable;
	m_hActor		= hActor;
	if( m_hActor )
		m_pMonsterActor = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
	m_CoolTime.Clear();

	m_mSkillTable.clear();
	for( UINT i=0 ; i<m_pSkillTable->size() ; ++i )
	{
		MonsterSkillTable& table = m_pSkillTable->at(i);
		m_mSkillTable.insert( std::make_pair(table.nSkillIndex,i) );
	}

	DN_ASSERT( m_pMonsterActor != NULL, "m_pMonsterActor == NULL 이 될 수 없음!!!" );
}

void MonsterSkillAI::Process( LOCAL_TIME LocalTime, float fDelta )
{
}

bool MonsterSkillAI::ScanActor(float fMinDist, float fMaxDist, int nTargetType, OUT DNVector(DnActorHandle)& ActorList)
{
	switch( nTargetType )
	{
		case MonsterSkillTable::SAMETEAM:			// 같은팀
		{
			GetMAScanner().Scan( MAScanner::eType::MonsterSkillSameTeam, m_hActor, fMinDist, fMaxDist, ActorList );
			break;
		}
		case MonsterSkillTable::OPPONENTTEAM:		// 상대팀
		{
			GetMAScanner().Scan( MAScanner::eType::MonsterSkillOpponentTeam, m_hActor, fMinDist, fMaxDist, ActorList );
			break;
		}
		case MonsterSkillTable::SAMETEAM_EXPECTME:   // 나를 제외한 같은팀
		{
			GetMAScanner().Scan( MAScanner::eType::MonsterSkillSameTeamExpectMe, m_hActor, fMinDist, fMaxDist, ActorList );
			break;
		}
		default:
		{
			_DANGER_POINT();
			break;
		}
	}

	return !ActorList.empty();
}

void MonsterSkillAI::AddWaitOrderCount( DnActorHandle hActor, int iSkillID )
{
	if( !hActor || !hActor->IsMonsterActor() )
		return;
	CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(hActor.GetPointer());
	pMonster->AddWaitOrderCount( iSkillID );
}

void MonsterSkillAI::DelWaitOrderCount( DnActorHandle hActor, int iSkillID )
{
	if( !hActor || !hActor->IsMonsterActor() )
		return;
	CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(hActor.GetPointer());
	pMonster->DelWaitOrderCount( iSkillID );
}

bool MonsterSkillAI::UseSkill( int iArrayIndex/*=-1*/ )
{
	m_iNextLuaSkillIndex = -1;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 현재 MP소모 스킬 사용불가로 적용되어 있는 스킬 효과에 추가로 몬스터에게 다른 효과를 적용 부탁드립니다.

	// 몬스터에게 적용되는 효과
	// 해당 시간동안 액션툴에서 STATE값이 Attack으로 되어 있는 액션을 제한한다.
	// 스킬을 사용하는 경우엔 CDnMonsterActor::CmdAction() 말고 이쪽으로 호출되고 스킬 내부에선 
	// SetActionQueue() 로 액션을 셋팅하므로 CDnMonsterActor::CmdAction() 과 이곳도 막아야 합니다.
	// 루틴 다시 추가 합니다. ㅠㅠ -한기.
	if( m_hActor && m_hActor->GetStateBlow() )
	{
		if( m_hActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_078 ) )
			return false;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	DWORD dwTime = timeGetTime();

	DNVector(_UsableSkill) UsableSkill;

	for ( int i = 0 ; i < (int)m_pSkillTable->size() ; i++ )
	{
		if( iArrayIndex >= 0 )
		{
			if( i != iArrayIndex )
				continue;
		}

		MonsterSkillTable& table = m_pSkillTable->at(i);

		// 스킬이 있는지 체크
		DnSkillHandle hSkill =  m_hActor->FindSkill(table.nSkillIndex);
		if ( !hSkill )
			continue;

		// 스킬이 사용가능한지.
		if ( hSkill->GetCoolTime() > 0.0f )
			continue;

		// 아직 쿨타임이 남았다면 
		if( m_CoolTime.Update( table.nSkillIndex, dwTime ) )
		{
			//g_Log.LogA( "SKILL:%d 쿨타임중입니다.\r\n", table.nSkillIndex );
			continue;
		}

		/*
			ArrayIndex 가 할당된 스킬은 rate 체크를 하지 않는다.
		*/
		bool bIgnoreRate	= false;
		bool bAnnounceSkill = table.pMAAiCheckerManager->bIsAnnounceSkill();
		if( iArrayIndex >= 0 || bAnnounceSkill == true )
			bIgnoreRate = true;
		table.pMAAiCheckerManager->IgnoreRateChecker( bIgnoreRate );

		if( !_bIsActorChecker(table) )
			continue;

		if( m_hActor->IsLimitAction( hSkill ) )
			continue;

		DnActorHandle hTargetActor; 
		if ( TargetCheck(i, hSkill, hTargetActor) == false )
		{
			continue;
		}
		else
		{
			_UsableSkill _us;
			_us.nTableIdx		= i ;
			_us.nSkillID		= table.nSkillIndex;
			_us.hTargetActor	= hTargetActor;
			UsableSkill.push_back(_us);

			if( bAnnounceSkill )
				break;
		}
	}

	if ( UsableSkill.empty() )
	{
		return false;
	}
	else
	{
		int iPriorityIdx = -1;
		int iPriorityVal = 0;
		for( UINT i=0 ; i<UsableSkill.size() ; ++i )
		{
			const MonsterSkillTable& table = m_pSkillTable->at(UsableSkill[i].nTableIdx);
			if( table.iPriority > 0 && iPriorityVal < table.iPriority )
			{
				iPriorityIdx = i;
				iPriorityVal = table.iPriority;
			}
		}

		_UsableSkill us;
		if( iPriorityIdx >= 0 )
		{
			us = UsableSkill[iPriorityIdx];
		}
		else
		{
			us = UsableSkill[_rand(m_hActor->GetRoom())%UsableSkill.size()];
		}

		MonsterSkillTable& table = m_pSkillTable->at(us.nTableIdx);
		if ( table.nCancelLook == 1 )
		{
			//if ( m_hActor->GetLookTarget() )
			{
				m_hActor->ResetMove();
				m_hActor->ResetLook();
				m_hActor->CmdLook(CDnActor::Identity(), false);

			}
		}
		else
		{
			if( m_pMonsterActor->GetAggroTarget() != us.hTargetActor 
#ifdef PRE_ADD_AURA_FOR_MONSTER
				&& table.nTargetType != MonsterSkillTable::_TargetType::SAMETEAM // 같은편에게 쓰는 스킬은 어그로를 바꾸지않는다. Ex> 오라형 스킬.
#endif
				) 
			{
				m_pMonsterActor->GetAIBase()->ChangeTarget( us.hTargetActor );
			}

			/*
			if ( m_hActor->GetLookTarget() != us.hTargetActor )
			{
				m_hActor->CmdLook(us.hTargetActor);
				
				// 아군에게 사용하는 스킬인 경우 타겟을 설정해준다.
				if( m_hActor->GetTeam() == us.hTargetActor->GetTeam() )
				{
					CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
					pMonster->GetAIBase()->ChangeTarget( us.hTargetActor );
				}
			}
			*/
		}

#if defined(PRE_FIX_64312)
		if (table.pMAAiCheckerManager)
		{
			std::vector<CMAAiActorChecker*>& checkerList = table.pMAAiCheckerManager->GetActorCheckerList();

			std::vector<CMAAiActorChecker*>::iterator iter = checkerList.begin();
			std::vector<CMAAiActorChecker*>::iterator endIter = checkerList.end();

			int nMasterSkillID = -1;

			for (; iter != endIter; ++iter)
			{
				CMAAiActorChecker* pChecker = *iter;
				if (pChecker)
				{
					CMAAiActorSkillToggleChecker* pSkillToggleChecker = dynamic_cast<CMAAiActorSkillToggleChecker*>(pChecker);
					if (pSkillToggleChecker)
					{
						nMasterSkillID = pSkillToggleChecker->GetSkillID();
					}
					else
					{
						CMAAiActorPassiveSkillChecker* pPassiveSkillChecker = dynamic_cast<CMAAiActorPassiveSkillChecker*>(pChecker);
						if (pPassiveSkillChecker)
						{
							nMasterSkillID = pPassiveSkillChecker->GetSkillID();
						}
					}
					
				}
			}

			if (nMasterSkillID != -1)
			{
				DnSkillHandle hMonsterSkill = m_hActor->FindSkill(us.nSkillID);
				DnSkillHandle hMasterBaseSkill;
				DnSkillHandle hMonsterExSkill;

				DnActorHandle hMasterActor = m_pMonsterActor->GetSummonerPlayerActor();
				if (hMasterActor)
					hMasterBaseSkill = hMasterActor->FindSkill(nMasterSkillID);
				
				if (hMasterBaseSkill)
					hMonsterExSkill = hMasterBaseSkill->GetSummonMonsterEnchangeSkill();

				if (hMonsterSkill && hMonsterExSkill)
				{
					//클라이언트로 패킷 보내서 스킬 적용..
					m_pMonsterActor->SendApplySummonMonsterExSkill(hMonsterSkill->GetClassID(), hMonsterSkill->GetLevel(), hMonsterSkill->GetSelectedLevelDataType(), 
																	hMasterActor->GetUniqueID(), hMasterBaseSkill->GetClassID());
					hMonsterSkill->ApplySummonMonsterEnchantSkill(hMonsterExSkill);
				}
			}
		}
#endif // PRE_FIX_64312

		CDnSkill::UsingResult eResult = m_hActor->UseSkill(us.nSkillID,true,false,m_pSkillTable->at(us.nTableIdx).iLuaSkillIndex );
		if ( CDnSkill::UsingResult::Success == eResult )
		{
			DelWaitOrderCount( m_hActor, us.nSkillID );
			m_CoolTime.AddCoolTime( table.nSkillIndex, dwTime+table.nCooltime );
#if defined( PRE_FIX_68096 )
			for( int i=0 ; i<ActionTableCommon::eCommon::MaxGlobalCoolTime ; ++i )
			{
				if( table.iGlobalCoolTimeIndex[i] >= 0 )
				{
					if( table.iGlobalCoolTimeIndex[i] < static_cast<int>(static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->GetScriptData().m_AIGlobalCoolTime.size()) )
						static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->m_GlobalCoolTime.AddCoolTime( table.iGlobalCoolTimeIndex[i], dwTime+static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->GetScriptData().m_AIGlobalCoolTime[table.iGlobalCoolTimeIndex[i]].dwCoolTime );
					else
						_ASSERT(0);
				}
			}
#else
			if( table.iGlobalCoolTimeIndex >= 0 )
			{
				if( table.iGlobalCoolTimeIndex < static_cast<int>(static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->GetScriptData().m_AIGlobalCoolTime.size()) )
					static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->m_GlobalCoolTime.AddCoolTime( table.iGlobalCoolTimeIndex, dwTime+static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase())->GetScriptData().m_AIGlobalCoolTime[table.iGlobalCoolTimeIndex].dwCoolTime );
				else
					_ASSERT(0);
			}
#endif

			//g_Log.LogA( "SKILL:%d 사용!!!\r\n", table.nSkillIndex );

			MAAiScript* pMAAiScript = static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase());
			
			DN_ASSERT( pMAAiScript != NULL, "pMAAiScript == NULL" );
			DN_ASSERT( m_pSkillTable->size() > static_cast<size_t>(us.nTableIdx), "m_pSkillTable.size() > us.nTableIdx" );

			pMAAiScript->m_cMultipleTarget.SetMultipleTargetSkill( &m_pSkillTable->at(us.nTableIdx) );
			// 다음실행할 스킬 인덱스 기억
			m_iNextLuaSkillIndex = m_pSkillTable->at(us.nTableIdx).iNextLuaSkillIndex;

#if defined( PRE_ADD_ACADEMIC )
			DnSkillHandle hSkill =  m_hActor->FindSkill(us.nSkillID);
			// 플레이어MP 소모
			if( hSkill && hSkill->GetSummonerDecreaseSP() && m_hActor->IsMonsterActor() )
			{
				CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
				DnActorHandle hSummoner = pMonster->GetSummonerPlayerActor();
				if( hSummoner )
				{
					int iSP = hSummoner->GetSP()-hSkill->GetSummonerDecreaseSP();
					if( iSP < 0 )
						iSP = 0;
					hSummoner->CmdRefreshHPSP( hSummoner->GetHP(), iSP );
				}
			}
#endif // #if defined( PRE_ADD_ACADEMIC )

			return true;
		}
	}

	return false;
}

bool MonsterSkillAI::_bIsActorChecker( const MonsterSkillTable& table )
{
	return table.pMAAiCheckerManager->bIsActorChecker( m_hActor, table.iLuaSkillIndex );
}

void MonsterSkillAI::_bIsTargetActorChecker( MonsterSkillTable& table, DNVector(DnActorHandle)& SelectedList )
{
	for( DNVector(DnActorHandle)::iterator itor=SelectedList.begin() ; itor!=SelectedList.end() ; )
	{
		bool bErase = true;
		if( table.pMAAiCheckerManager->bIsTargetActorChecker( m_hActor, *itor ) )
			bErase = false;
	
		if( bErase == true )
			itor = SelectedList.erase( itor );
		else
			++itor;
	}
}

bool MonsterSkillAI::TargetCheck(int nSkillTableID, DnSkillHandle hSkill, OUT DnActorHandle& hTargetActor)
{
	if ( !m_pSkillTable || nSkillTableID >= (int)m_pSkillTable->size() )
		return false;

	hTargetActor = CDnActor::Identity();
	MonsterSkillTable& table = m_pSkillTable->at(nSkillTableID);

	DNVector(DnActorHandle)	ScanList;

	// SelfPercent
	switch(table.nTargetType)
	{
		case MonsterSkillTable::SELF:
		{
			hTargetActor = m_hActor;
			return true;
		}
		case MonsterSkillTable::OPPONENTTEAM:
		{
			hTargetActor =  m_pMonsterActor->GetAggroTarget();
			if( !hTargetActor )
				return false;
#if defined( PRE_FIX_67719 )
			if( table.fRandomTarget > 0.f )
			{				
				if( !_SelectRandomTarget( table ) && table.nIgnoreAggroTarget == 1 )
					return false;
				else
					hTargetActor =  m_pMonsterActor->GetAggroTarget();
				if( !hTargetActor )
					return false;
			}			
#endif			
			// 거리 체크
			if( !MAAiScript::bIsTargetDistance( m_hActor, hTargetActor, table.nRangeMin, table.nRangeMax ) )
				return false;

			ScanList.push_back( hTargetActor );
			break;
		}
		case MonsterSkillTable::SAMETEAM:
		{
			ScanActor((float)table.nRangeMin, (float)table.nRangeMax, table.nTargetType, ScanList );
			break;
		}
		case MonsterSkillTable::SAMETEAM_EXPECTME:
		{
			ScanActor((float)table.nRangeMin, (float)table.nRangeMax, table.nTargetType, ScanList );
			break;
		}
	}

	// TargetActor 체크
	_bIsTargetActorChecker( table, ScanList );

	if( !ScanList.empty() )
	{
		hTargetActor = ScanList[_rand(m_hActor->GetRoom())%ScanList.size()];
		return true;
	}

	return false;
}

#if defined( PRE_FIX_67719 )
bool MonsterSkillAI::_SelectRandomTarget( MonsterSkillTable& table )
{
	if( table.fRandomTarget == 0.f )
		return false;

	if(!m_pMonsterActor)
		return false;

	DNVector(DnActorHandle) vTarget;
	MAAiScript* pMAAiScript = static_cast<MAAiScript*>(m_pMonsterActor->GetAIBase());	
	
	if(!pMAAiScript)
		return false;

	pMAAiScript->GetTargetDistance( table.nRangeMin, table.nRangeMax, vTarget );
	if( vTarget.empty() )
		return false;

	DnActorHandle hPrevTarget = pMAAiScript->GetTarget();

	size_t uiRand = _rand(m_hActor->GetRoom())%vTarget.size();

	if( !table.pMAAiCheckerManager->bIsTargetActorChecker( m_hActor, vTarget[uiRand] ) )
		return false;

	pMAAiScript->ChangeTarget( vTarget[uiRand] );

	if( table.fRandomTarget > 0.f )
	{
		CDNAggroSystem* pAggroSystem = m_hActor->GetAggroSystem();
		CDNAggroSystem::AggroStruct* pPrevStruct = pAggroSystem->GetAggroStruct( hPrevTarget );
		if( pPrevStruct )
		{
			CDNAggroSystem::AggroStruct* pStruct = pAggroSystem->GetAggroStruct( vTarget[uiRand] );
			if( pStruct )
			{
				pStruct->iAggro = static_cast<int>(pPrevStruct->iAggro * table.fRandomTarget);
				return true;
			}
		}
	}
	return false;
}
#endif