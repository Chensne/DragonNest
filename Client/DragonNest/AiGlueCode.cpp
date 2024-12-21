#include "Stdafx.h"
/*
#include "MAAiScript.h"
#include "MAMovementBase.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"

#include "AiGlueCode.h"


//--------------------------------------------------------------------------
void cppGlue_Log(const char* szMsg)
{
	TCHAR szString[512];
	MultiByteToWideChar( CP_ACP, 0, szMsg, -1, szString, 512 );

	LogWnd::Log(9, szString);
}

void cppGlue_FindWalk(DWORD nUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	
	// 어슬렁 거린다.
	EtVector3 *pvPos = hActor->GetPosition();
	EtVector3 vTemp;
	vTemp = *pvPos;
	vTemp.y = 0.f;
	vTemp.x += cos( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );
	vTemp.z += sin( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );

	CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor *>(hActor.GetPointer());
	if( pMonster ) {
		SAABox Box = *pMonster->GetGenerationArea();
		Box.Max.y = 1000000.f;
		Box.Min.y = -1000000.f;
		if( !Box.IsInside( vTemp ) ) {
			vTemp.x = Box.Min.x + ( rand()%(int)( Box.Max.x - Box.Min.x ) );
			vTemp.z = Box.Min.z + ( rand()%(int)( Box.Max.z - Box.Min.z ) );
			vTemp.y = CDnWorld::GetInstance().GetHeight( vTemp );
		}
	}

	hActor->CmdMove( vTemp, "Walk_Front", -1, 8.f );
}

int cppGlue_IsCurrentAction(DWORD nUID, const char* szAction)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nUID);
	if ( !pScript ) return -1;

	DnActorHandle hActor = pScript->GetActor();
	std::string strAction = hActor->GetCurrentAction();
	if ( strAction == szAction )
		return 1;

	return -1;
}


const char* cppGlue_GetCurrentAction(DWORD nUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nUID);
	if ( !pScript ) return NULL;

	DnActorHandle hActor = pScript->GetActor();
	std::string strAction = hActor->GetCurrentAction();

	return strAction.c_str();
}

int cppGlue_FindTarget(DWORD nUID, float fDistance)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nUID);
	if ( !pScript ) return -1;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();
	hTarget.Identity();
	pScript->SetTarget(hTarget);

	EtVector3 *pvPos = hActor->GetPosition();

	// 찾아본다.
	if( !hTarget ) 
	{
		std::vector<DnActorHandle> hVecList;
		int nCount = CDnActor::ScanActor( *pvPos, fDistance, hVecList );
		if( nCount > 1 ) {
			std::vector<DnActorHandle> hVecTargetResult;
			for( DWORD i=0; i<hVecList.size(); i++ ) {
				if( hVecList[i]->GetTeam() == hActor->GetTeam() ) continue;
				if( hVecList[i]->IsDie() ) continue;
				hVecTargetResult.push_back( hVecList[i] );
			}

			if( !hVecTargetResult.empty() )
			{
				hTarget = hVecTargetResult[ rand()%hVecTargetResult.size() ];
				if( hTarget->IsMove() )
					hActor->CmdStop( "Stand", 0, 6.f );
			}
		}
	}

	// 찾았으면 
	if( hTarget ) 
	{
		// 죽었으면 
		if( hTarget->IsDie() ) 
		{
			// 타겟을 널로 만들고 세팅
			hTarget.Identity();
			pScript->SetTarget(hTarget);
			return -1;
		}

		pScript->SetTarget(hTarget);
		return hTarget->GetUniqueID();
	}
	else
	{
		// 못찾은거다.
		hTarget.Identity();
		pScript->SetTarget(hTarget);
		return -1;
	}
}

void cppGlue_ChaseTarget(DWORD nMyUID, DWORD nTargetUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();
	EtVector3 *pvPos = hActor->GetPosition();

	// 따라간다.
	hActor->CmdMove( hTarget, 100.f, "Move_Front", -1, 6.f );
	hActor->CmdLook( hTarget );

}

float cppGlue_TargetDistance(DWORD nMyUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return 10000.0f;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();

	if ( !hTarget )
		return 10000.0f;

	// 거리를 구해서
	EtVector3 *pvPos = hActor->GetPosition();
	float fLength = EtVec3Length( &( *pvPos - *hTarget->GetPosition() ) );
	return fLength;
}

float cppGlue_UnitSize(DWORD nMyUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return 0.0f;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();

	if ( !hActor )
		return 0.0f;
	if ( !hTarget )
		return 0.0f;


	float fUnitSize = (float)( hActor->GetUnitSize() + hTarget->GetUnitSize() );

	return fUnitSize;
}

void cppGlue_Tracing(DWORD nMyUID, float fDist, const char* szAction)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();

	if ( !hTarget )
		return;

	hActor->CmdMove( hTarget, fDist, szAction, 0, 6.f);

}

void cppGlue_LookTarget(DWORD nMyUID)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	DnActorHandle hTarget = pScript->GetTarget();
	if ( !hTarget )
		return;

	hActor->CmdLook(hTarget);
}


void cppGlue_CmdAction(DWORD nMyUID, const char* szAction)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	if ( !hActor )
		return;

	cppGlue_Log(szAction);
	
	hActor->CmdAction(szAction, 0 , 0.f);
}


DWORD cppGlue_GetCurTime()
{
	return GetTickCount();
}

void cppGlue_FinishDying(DWORD nMyUID, float fDieDelta)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	hActor->SetDieDelta(fDieDelta);

}

void cppGlud_Rotate(DWORD nMyUID, float fAngle)
{
	MAAiScript* pScript = MAAiScript::FindMonsterScriptAI(nMyUID);
	if ( !pScript ) return;

	DnActorHandle hActor = pScript->GetActor();
	hActor->GetCross()->RotateYaw(fAngle);
	LogWnd::Log(2,_T("회전!!!"));
}



MAAiScript::MonsterScriptAIMap	MAAiScript::ms_MonsterScriptAIMap;
*/
/*
int g_nMyUID = 2;
int g_nTargetUID = -1;
void	lua_OnStateStand();
void	lua_OnStateFindTarget();
void	lua_OnStateChase();
void	lua_OnStateSnarl();
void	lua_OnStateAttak();	
*/

/*
bool
MAAiScript::AddMonsterScriptAI(DWORD nUID, MAAiScript* pAI)
{
	MonsterScriptAIMapIt it = ms_MonsterScriptAIMap.find(nUID);
	if ( it == ms_MonsterScriptAIMap.end() )
	{
		ms_MonsterScriptAIMap.insert(std::pair<DWORD, MAAiScript*>(nUID, pAI));
		return true;
	}

	return false;
}

bool	
MAAiScript::RemoveMonsterScriptAI(DWORD nUID)
{
	MonsterScriptAIMapIt it = ms_MonsterScriptAIMap.find(nUID);
	if ( it != ms_MonsterScriptAIMap.end() )
	{
		ms_MonsterScriptAIMap.erase(it);
		return true;
	}
	return false;
}

MAAiScript*
MAAiScript::FindMonsterScriptAI(DWORD nUID)
{
	MonsterScriptAIMapIt it = ms_MonsterScriptAIMap.find(nUID);
	if ( it != ms_MonsterScriptAIMap.end() )
	{
		MAAiScript* pAI = it->second;
		return pAI;
	}

	return NULL;
}

bool
MAAiScript::ReloadAllScript()
{
	MonsterScriptAIMapIt it = ms_MonsterScriptAIMap.begin();
	for ( it ;  it != ms_MonsterScriptAIMap.end() ; ++it )
	{
		MAAiScript* pAI = it->second;
		if ( pAI )
		{
			pAI->m_bLoadScript = false;
		}
	}
	return true;
}
//------------------------------------------------------------------------

MAAiScript::MAAiScript( DnActorHandle hActor, MAAiReceiver *pReceiver )
: MAAiBase( hActor, pReceiver )
{
	m_hTarget.Identity();
	m_nState  = 0 ;
	m_bLoadScript = false;
	m_pLua = NULL;
	m_szLuaFileName = "BasiliskAI.lua";
	m_nLastChangedTime = 0 ;
}

MAAiScript::~MAAiScript()
{
	MAAiScript::RemoveMonsterScriptAI(m_nUID);
	if ( m_pLua )
		lua_close(m_pLua);
}

bool
MAAiScript::_LoadScript(const char* szFileName)
{
	if ( m_pLua )
	{
		lua_close(m_pLua);
		m_pLua = NULL;
	}
	m_pLua = lua_open();
	luaopen_base(m_pLua);
	luaopen_math(m_pLua);


	lua_tinker::def(m_pLua, "cppGlue_Log", cppGlue_Log);
	lua_tinker::def(m_pLua, "cppGlue_FindWalk", cppGlue_FindWalk);
	lua_tinker::def(m_pLua, "cppGlue_IsCurrentAction", cppGlue_IsCurrentAction);

	lua_tinker::def(m_pLua, "cppGlue_FindTarget", cppGlue_FindTarget);
	lua_tinker::def(m_pLua, "cppGlue_ChaseTarget", cppGlue_ChaseTarget);
	lua_tinker::def(m_pLua, "cppGlue_TargetDistance", cppGlue_TargetDistance);
	lua_tinker::def(m_pLua, "cppGlue_UnitSize", cppGlue_UnitSize);
	lua_tinker::def(m_pLua, "cppGlue_Tracing", cppGlue_Tracing);
	lua_tinker::def(m_pLua, "cppGlue_LookTarget", cppGlue_LookTarget);

	lua_tinker::def(m_pLua, "cppGlue_CmdAction", cppGlue_CmdAction);
	lua_tinker::def(m_pLua, "cppGlue_GetCurTime", cppGlue_GetCurTime);

	lua_tinker::def(m_pLua, "cppGlue_FinishDying", cppGlue_FinishDying);
	lua_tinker::def(m_pLua, "cppGlud_Rotate", cppGlud_Rotate);

	lua_tinker::def(m_pLua, "cppGlue_GetCurrentAction", cppGlue_GetCurrentAction);



	lua_tinker::dofile(m_pLua, szFileName);
	m_nUID = m_hActor->GetUniqueID();
	lua_tinker::set<DWORD>(m_pLua, "g_nMyUID", m_nUID);	

	MAAiScript::AddMonsterScriptAI(m_nUID, this);

	return true;
}

bool MAAiScript::_PreScriptCall()
{
	bool bUseLua = true;

	if ( bUseLua && m_bLoadScript == false )
	{
		_LoadScript(m_szLuaFileName.c_str());
		m_bLoadScript = true;
	}

	return true;
}

void MAAiScript::OnFinishAction(const char* szPrevAction)
{
	return;
	if ( _PreScriptCall() )
	{
		lua_tinker::call<int>(m_pLua, "lua_OnAI");	
	}	
}

void MAAiScript::OnDie()
{
	if ( _PreScriptCall() )
	{
		lua_tinker::call<int>(m_pLua, "lua_OnDie");	
	}
}

void MAAiScript::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if ( _PreScriptCall() )
	{
		lua_tinker::call<int>(m_pLua, "lua_OnAI");	
	}
}

*/