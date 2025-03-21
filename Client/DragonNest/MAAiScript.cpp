#include "StdAFx.h"
#include "MAAiScript.h"
#include "DnMonsterActor.h"
#include "DnWorld.h"
#include "DnWeapon.h"
#include "lua_tinker.h"

std::vector<MAAiScript*>	MAAiScript::ms_VecAIScript;

const float g_fBendFrame = 8.0f;

void
MAAiScript::ReloadAllScript()
{
	int nCount = (int)ms_VecAIScript.size();
	for ( int i = 0 ; i < nCount ; i++ )
	{
		ms_VecAIScript[i]->LoadScript(ms_VecAIScript[i]->m_szSettingFile.c_str());
	}

}

MAAiScript::MAAiScript( DnActorHandle hActor, MAAiReceiver *pReceiver )
: MAAiBase( hActor, pReceiver )
{
	m_nNearTableCnt = 0;
	m_nLookTargetNearState = 0;
	m_fWanderingDistance = 1000.0f;
	m_nPatrolBaseTime = 5000;
	m_nPatrolRandTime = 3000;
	m_fApproachValue = 100.0f;
	ms_VecAIScript.push_back(this);
	m_nState = Normal;
}

MAAiScript::~MAAiScript()
{
	std::vector<MAAiScript*>::iterator it = std::find(ms_VecAIScript.begin(), ms_VecAIScript.end(), this);
	if ( it != ms_VecAIScript.end() )
	{
		ms_VecAIScript.erase(it);
	}
}


void MAAiScript::Process( LOCAL_TIME LocalTime, float fDelta )
{
	MAAiBase::Process( LocalTime, fDelta );

	std::string szAction = m_hActor->GetCurrentPlayAction();
	if ( szAction == ""  )	
	{
		//m_hActor->SetActionQueue("Stand");
		m_hActor->CmdStop( "Stand", 0, -1.f );
	}
	

	OnAI("", LocalTime);

}

void MAAiScript::OnFinishAction(const char* szPrevAction, LOCAL_TIME time)
{
	OnAI(szPrevAction, time);
}


void MAAiScript::OnDie()
{

}

bool MAAiScript::FindTarget()
{
	// 타겟이 없으면 타겟을 찾는다.
	if( !m_hTarget )
	{
		SetTarget( GetAggroTarget() );
		if( m_hTarget ) {
			
			if( m_hActor->IsMove() ) 
			{
				m_hActor->CmdStop( "Stand", 0, g_fBendFrame );
			}
			if( m_hActor->GetLookTarget() != m_hTarget )
				m_hActor->CmdLook(m_hTarget);
			return true;
		}
	}
	return false;

}

void MAAiScript::OnAI(const char* szPrevAction, LOCAL_TIME time)
{
	if( m_hActor->IsDie() ) return;
	
	EtVector3 *pvPos = m_hActor->GetPosition();
	
	bool bFoundTarget = FindTarget();

	if( m_hTarget ) {
		float fLength = EtVec3Length( &( *pvPos - *m_hTarget->GetPosition() ) );
		// 너무 멀면 타겟팅을 취소 하고 배회하도록 하자.
		if ( fLength > GetThreatRange() )
		{
			SetTarget( CDnActor::Identity() );
			m_hActor->CmdLook(m_hTarget);
			SetDelay( 0, 0 );
			return;
		}

		if( m_hTarget->IsDie() )
		{
			SetTarget( CDnActor::Identity() );
			m_hActor->CmdLook(m_hTarget);
			SetDelay( 0, 0 );
			return;
		}

		// 너무 가까우면 뒤로 물러난다. 
		if ( fLength <= 50.0f && m_hActor->IsMovable() /*&& !m_hActor->IsHit() && !m_hActor->IsAttack() && !m_hActor->IsDown()*/ )
		{
			if( m_hActor->GetLookTarget() != m_hTarget )
				m_hActor->CmdLook(m_hTarget);

			std::string szAction;
			int nBack = rand()%2;
			switch( nBack )
			{
				case 0: szAction = "Move_Back"; break;
				case 1: szAction = "Walk_Back"; break;
			}

			m_hActor->CmdAction( szAction.c_str(), 0, g_fBendFrame  );
			return;
		}

		// 추격중인데 5초 지나면. 
		if( m_nState == Assault && time - m_ChangeStateTime > 5000 )
		{
			m_nState = Normal;
			m_hActor->CmdStop( "Stand", 0, g_fBendFrame );
			return;
		}

		if ( m_hActor->IsStay() && m_hActor->IsMovable() )
		{
			if( m_hActor->GetLookTarget() != m_hTarget )
				m_hActor->CmdLook(m_hTarget);
			
			int state = GetTargetDistanceState();
			OnDistanceState(state, fLength, time);
		}
	}
	// 타겟이 없으면 어슬렁
	else
	{
		if( m_hActor->GetLookTarget() ) m_hActor->CmdLook(m_hTarget);
		if ( IsDelay(0) ) return;
		if ( !m_hActor->IsHit() && m_hActor->IsMove() ) m_hActor->CmdStop( "Stand", 0, g_fBendFrame );

		// 어슬렁 거린다.
		EtVector3 *pvPos = m_hActor->GetPosition();
		EtVector3 vTemp;
		vTemp = *pvPos;
		vTemp.y = 0.f;
		vTemp.x += cos( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );
		vTemp.z += sin( EtToRadian( rand()%360 ) ) * ( 300 + rand()%200 );

		CDnMonsterActor *pMonster = dynamic_cast<CDnMonsterActor *>(m_hActor.GetPointer());
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


		m_hActor->CmdMove( vTemp, "Walk_Front", -1, g_fBendFrame );
		if ( m_nPatrolBaseTime < 1 || m_nPatrolRandTime < 1)
		{
			SetDelay( 0, 5000 + rand()%3000 );
		}
		else
		{
			SetDelay( 0, m_nPatrolBaseTime + rand()%m_nPatrolRandTime );
		}
		
		//LogWnd::Log(4, _T("어슬렁"));
		
	}


}


int MAAiScript::GetTargetDistanceState()
{
	if( !m_hTarget )
	{
		return -1;
	}

	EtVector3 *pvPos = m_hActor->GetPosition();
	float fLength = EtVec3Length( &( *pvPos - *m_hTarget->GetPosition() ) );

	int nSize = (int)m_AITable.size();
	if ( nSize < 2 )
	{
		return -1;
	}

	if ( fLength >= 0.0f && fLength < m_AITable[0].fNearValue )
	{
		return 0;
	}

	for ( int i = 0 ; i < nSize-1 ; i++ )
	{
		if ( fLength >= m_AITable[i].fNearValue && fLength < m_AITable[i+1].fNearValue )
		{
			return i+1;
		}
	}

	return -1;
}

void MAAiScript::OnDistanceState(int state, float fLength, LOCAL_TIME time)
{
	switch(m_nState)
	{
	case Normal:		OnNormal(state, fLength, time);
		//LogWnd::Log(3, _T("OnNormal"));
		break;
	case Assault:		OnAssault(state, fLength, time);	
		//LogWnd::Log(3, _T("OnAssault"));
		break;
	}
		

}

void MAAiScript::OnAssault(int state, float fLength, LOCAL_TIME time)
{
	float fUnitSize = (float)( m_hActor->GetUnitSize() + m_hTarget->GetUnitSize() );

	if( time - m_ChangeStateTime < 5000 )
	{
		if( ( m_hActor->IsMove() || m_hActor->IsStay() ) && fLength < 100.f + fUnitSize ) 
		{
			m_hActor->CmdLook( m_hTarget, false );
			m_hActor->CmdAction( "Attack1", 0, g_fBendFrame );

			m_nState = Normal;
		}

		if( m_hActor->IsMove() && time - m_ChangeStateTime > 2000 ) 
		{
			if( fLength < 400.f )
			{						
				if( rand()%100 < 40 )
				{
					if( rand()%2 )
					{
						m_hActor->CmdAction( "Move_Left", 3+rand()%2, g_fBendFrame );
					}
					else 
					{
						m_hActor->CmdAction( "Move_Right", 3+rand()%2, g_fBendFrame );
					}

					if( m_hActor->GetLookTarget() != m_hTarget )
						m_hActor->CmdLook( m_hTarget );

					m_nState = Normal;
				}
			}
		}
	}
}

int MAAiScript::SelectAction(std::vector<ActionTable*>& AtArray, LOCAL_TIME time)
{
	if ( AtArray.size() < 1 )
	{
		return 0;
	}
/*
	int nResult = rand()%((int)AtArray.size());//(rand()%(m_AITable[state].nRateSum));
	LogWnd::Log(3,_T("선택갯수:%d 선택번호 : %d"), (int)AtArray.size(), nResult) ;
	AtArray[nResult]->nLastTime = time;

	// 쿨타임 가능한 녀석들 중에서 rand 한거기때문에 실제 인덱스가 필요해.
	return AtArray[nResult]->nIdx;
*/
	int nRateSum = 0;
	for ( int i = 0 ; i < (int)AtArray.size() ; i++ )
	{
		nRateSum += AtArray[i]->nRate;
	}

	int nRand = rand()%nRateSum;//(rand()%(m_AITable[state].nRateSum));

	int nCount = (int)AtArray.size();

	int nSum = 0;
	int nIndex = -1;
	for ( int i = 0 ; i < nCount ; i++  )
	{
		if ( nSum >= nRand )
		{
			nIndex = i;
			if ( nIndex > -1 )
			{
				//LogWnd::Log(2, _T("State:%d nIdex : %d rand:%d sum : %d"), state, nIndex, nRand , nSum );
				nIndex-=1;
				break;
			}
		}
		nSum += AtArray[i]->nRate;
	}

	if ( nIndex < 0 )
	{
		nIndex = nCount-1;
	}
	
	AtArray[nIndex]->nLastTime = time;
	
	return AtArray[nIndex]->nIdx;
}

int MAAiScript::SelectActionFromCooltime(std::vector<ActionTable>& AtArray, LOCAL_TIME time)
{
	std::vector<ActionTable*> UsableAT;

	for ( int i = 0 ; i < (int)AtArray.size() ; i++ )
	{
		bool bResult = IsPassedCoolTime(AtArray[i], time);
		if ( bResult )
		{
			UsableAT.push_back( &(AtArray[i]) );
		}
	}

	return SelectAction(UsableAT, time);
}

bool MAAiScript::IsPassedCoolTime(ActionTable& at, LOCAL_TIME time)
{
	if ( at.nLastTime == 0 )
		return true;

	if ( at.nLastTime + at.nCoolTime <= time )
		return true;

	return false;
}

void MAAiScript::OnNormal(int state, float fLength, LOCAL_TIME time)
{
	if ( state < 0 || state >= (int)m_AITable.size() || m_AITable.size() == 0 )
		return;

	if ( state == 0 )
	{
		int a = 0;
	}
	int nIndex = SelectActionFromCooltime(m_AITable[state].VecActionTable, time);

	int nLoop = rand()%m_AITable[state].VecActionTable[nIndex].nLoop;
	//nLoop++;

	
	TCHAR szString[512];
	MultiByteToWideChar( CP_ACP, 0, m_AITable[state].VecActionTable[nIndex].szActionName.c_str(), -1, szString, 512 );
	LogWnd::Log(2, _T("near : %d Distance : %.2f index : %d 반복:%d AC :%s"), state+1, fLength, nIndex,  nLoop , szString);
	
	std::string szAction = m_AITable[state].VecActionTable[nIndex].szActionName;

	if ( szAction == "Assault" )
	{
		m_nState = Assault;

		float fAttackDist = m_hTarget->GetUnitSize() + m_hActor->GetUnitSize() + ( ( m_hActor->GetWeapon() ) ? m_hActor->GetWeapon()->GetWeaponLength() : 0.f );
		if( EtVec3LengthSq( &( *m_hActor->GetPosition() - *m_hTarget->GetPosition() ) ) <= fAttackDist * fAttackDist ) return;
		m_hActor->CmdMove( m_hTarget, fAttackDist, "Move_Front", -1, g_fBendFrame );

		m_ChangeStateTime = time;
		if( m_hActor->GetLookTarget() != m_hTarget )
			m_hActor->CmdLook( m_hTarget );
		return;
	}


	if ( state+1 <= m_nLookTargetNearState )
	{
		if ( !m_hTarget->IsDie() )
		{
			if ( fLength > 50.0f )
			{
				if( m_hActor->GetLookTarget() != m_hTarget )
					m_hActor->CmdLook(m_hTarget);
			}
		}
	}


	if( strstr( szAction.c_str(), "Front" ) ) 
	{
		float fAttackDist = m_hTarget->GetUnitSize() + m_hActor->GetUnitSize() + ( ( m_hActor->GetWeapon() ) ? m_hActor->GetWeapon()->GetWeaponLength() : 0.f );
		if ( fLength <=  fAttackDist )
		{
			return;
		}
		m_hActor->CmdMove( m_hTarget, m_fApproachValue, szAction.c_str(), nLoop, g_fBendFrame);
	}
	else if( strstr( szAction.c_str(), "Attack" ) ) 
	{
//		OutputDebug( "Loop : %d\n", nLoop );
//		m_hActor->CmdLook( m_hTarget, false );
		m_hActor->CmdStop( szAction.c_str(), nLoop, g_fBendFrame );
		//m_hActor->CmdAction( szAction.c_str(), nLoop, g_fBendFrame );
	}
	else
	{
		m_hActor->CmdAction(szAction.c_str(), nLoop , g_fBendFrame);
	}

}

bool	
MAAiScript::LoadScript(const char* szFilename)
{

	std::string szFullName;
	szFullName = g_PathMng.GetFullPathName( szFilename ).c_str();
	m_szSettingFile = szFilename;

	lua_State* pLua =  lua_open();
	luaL_openlibs(pLua); 
	TCHAR szString[512];
	MultiByteToWideChar( CP_ACP, 0, szFullName.c_str(), -1, szString, 512 );
	LogWnd::Log(1, _T("LuaLoad:%s"), szString );
	if ( lua_tinker::dofile(pLua, szFullName.c_str()) != 0 )
	{
		lua_close(pLua);
		return false;
	}

	m_nNearTableCnt = lua_tinker::get<int>(pLua, "g_Lua_NearTableCount");
	m_nLookTargetNearState = lua_tinker::get<int>(pLua, "g_Lua_LookTargetNearState");
	m_fWanderingDistance = (float)lua_tinker::get<float>(pLua, "g_Lua_WanderingDistance");
	m_nPatrolBaseTime = lua_tinker::get<int>(pLua, "g_Lua_PatrolBaseTime");
	m_nPatrolRandTime = lua_tinker::get<int>(pLua, "g_Lua_PatrolRandTime");
	m_fApproachValue = lua_tinker::get<float>(pLua, "g_Lua_ApproachValue");
	if ( m_fApproachValue < 10.0f )
	{
		m_fApproachValue = 100.0f;
	}

	m_AITable.clear();

	for ( int i = 0 ; i < m_nNearTableCnt ; i++ )
	{
		AI_TABLE table;
		char buff[512] = "";
		ZeroMemory(buff, sizeof(char)*512);
		sprintf_s(buff, "g_Lua_NearValue%d", i+1 );
		table.fNearValue = (float)lua_tinker::get<float>(pLua, buff);

		ZeroMemory(buff, sizeof(char)*512);
		sprintf_s(buff, "g_Lua_Near%d", i+1 );
		lua_tinker::table neartable = lua_tinker::get<lua_tinker::table>(pLua, buff);
		table.nRateSum = 0;
		
		
		for ( int j = 0 ; j < 50 ; j++ )
		{
			ZeroMemory(buff, sizeof(char)*512);
			sprintf_s(buff, "%d", j+1 );
			lua_tinker::table t = neartable.get<lua_tinker::table>(buff);
			
			const char* szActionName = t.get<const char*>("action_name");
			if ( szActionName == NULL )
			{
				break;
			}
			std::string sz(szActionName);
			int nRate = t.get<int>("rate");
			int nLoop = t.get<int>("loop");
			int nCoolTime = t.get<int>("cooltime");

			ActionTable at;
			at.szActionName = sz;
			at.nIdx = (int)table.VecActionTable.size();
			at.nRate = nRate;
			at.nLoop = nLoop;
			at.nCoolTime = nCoolTime;
			table.VecActionTable.push_back(at);
			table.nRateSum += nRate;
		}

		m_AITable.push_back(table);
	}
	
	lua_close(pLua);
	return true;
}