#include "StdAfx.h"
#include "DnPuppetBlow.h"
#if defined(_GAMESERVER)
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnMonsterActor.h"
#ifdef PRE_ADD_DECREASE_EFFECT
#include "DnStateBlow.h"
#endif // PRE_ADD_DECREASE_EFFECT
#endif // _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPuppetBlow::CDnPuppetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_247;

	SetValue( szValue );
	//m_fValue = (float)atof(szValue);
	m_fLimitRange = -1.0f;

	std::string str = szValue;
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	TokenizeA(str, tokens, delimiters);
	if (tokens.size() >= 2)
	{
		m_fValue = (float)atof(tokens[0].c_str());	//몬스터 ID
		m_DamageChangeRateArg = tokens[1];	//데미지 비율

		if (tokens.size() >= 3)
			m_fLimitRange = (float)atof(tokens[2].c_str());
	}
	else
	{
// 		m_fValue = 33501.0f;
// 		m_DamageChangeRateArg = "1.2";
		OutputDebug("%s Invalidvalue.... %s\n", __FUNCTION__, szValue);
	}

#if defined(_GAMESERVER)
	m_vDeltaPos = EtVector3(1.0f, 0.0f, 1.0f);
#endif // _GAMESERVER
}

CDnPuppetBlow::~CDnPuppetBlow(void)
{
}

#ifdef _GAMESERVER
bool CDnPuppetBlow::CanBegin( void )
{
	//꼭두각시 상태효과는 낙인 상태효과가 있을때만 적용 가능하다.
	if (m_hActor && m_hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_246) == true)
		return true;
	else
		return false;
}
#endif

void CDnPuppetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	SummonMonsterStruct Struct;
	memset(&Struct, 0, sizeof(Struct));
	Struct.MonsterID = (int)m_fValue;
	Struct.vPosition = &m_vDeltaPos;

	//꼭두각시 소환체 지속시간은 꼭두각시 상태효과 지속시관으로 설정
	Struct.nLifeTime = (int)(m_StateBlow.fDurationTime*1000.0f);

	//여기서 팀 설정을 0으로 해야 RequestSummonMonster함수에서 Owner의 팀으로 설정됨.
	//Struct.nTeam = m_hActor->GetTeam();

	Struct.bCopySummonerState = true;
	Struct.fHPAdjustValue = 1.0f;

	Struct.bSuicideWhenSummonerDie = TRUE;	//소환한 액터가 죽을때 같이 사라지도록 설정.
	Struct.bFollowSummonerStage = FALSE;	//소호한 액터 따라 다니기 설정

	CDnGameTask* pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstancePtr(GetRoom())->GetTask( "GameTask" ));
	
	if( pGameTask )
		m_hSummonMonster = pGameTask->RequestSummonMonsterBySkill( m_hActor, m_ParentSkillInfo.hSkillUser, &Struct );

	//물리/마법 데미지 변경 상태효과 추가 한다..
	if (m_hSummonMonster)
	{
		int nDurationTime = (int)(m_StateBlow.fDurationTime * 1000.0f);
		const char* szParam = m_DamageChangeRateArg.c_str();

		m_hSummonMonster->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_134, nDurationTime, szParam);
		m_hSummonMonster->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_135, nDurationTime, szParam);


		//위 pGameTask->RequestSummonMonsterBySkill에서 호출 되지만 일단 여기에 남겨 놓는다.
		m_hSummonMonster->SetPuppetSummonMonster(true);
	}
#ifdef PRE_ADD_DECREASE_EFFECT
		if( m_hSummonMonster && m_bShowReduce )
		{
			m_hSummonMonster->SendAddSEFail( CDnStateBlow::ADD_DECREASE_EFFECT_BY_IMMUNE, m_StateBlow.emBlowIndex );
			m_bShowReduce = false;
		}
#endif // PRE_ADD_DECREASE_EFFECT
#endif // _GAMESERVER

	OutputDebug( "%s\n", __FUNCTION__ );
}

void CDnPuppetBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

#if defined(_GAMESERVER)
	if (m_fLimitRange >= 0.0f)
	{
		if (m_hActor && m_hSummonMonster)
		{
#if defined(PRE_FIX_61438)
			SSphere actorSphere;
			SSphere summonActorSphere;

			//드래곤의 경우 바운딩 정보가 엄청 크게 잡혀 있는 경우가 있음.
			//실제 액터 테이블의 수치 값으로 변경..
			m_hActor->GetBoundingSphere(actorSphere, true);
			m_hSummonMonster->GetBoundingSphere(summonActorSphere, true);
			
			EtVector2 vVec;
			vVec.x = summonActorSphere.Center.x - actorSphere.Center.x;
			vVec.y = summonActorSphere.Center.z - actorSphere.Center.z;

			float fLength = EtVec2Length(&vVec);
			if (summonActorSphere.fRadius + actorSphere.fRadius + m_fLimitRange < fLength)
			{
				SetState(STATE_BLOW::STATE_END);

				//클라이언트로 삭제 패킷 보내야함..
				m_hActor->CmdRemoveStateEffectFromID(GetBlowID());
			}

#else
			EtVector3 ownerActorPos = *(m_hActor->GetPosition());
			EtVector3 puppetActorPos = *(m_hSummonMonster->GetPosition());

			EtVector2 vVec;
			vVec.x = puppetActorPos.x - ownerActorPos.x;
			vVec.y = puppetActorPos.z - ownerActorPos.z;

			float fLength = EtVec2Length(&vVec);

			//설정 범위를 벗어 나면 상태효과 종료..
			if( m_fLimitRange < fLength )
			{
				SetState( STATE_BLOW::STATE_END );
				
				//클라이언트로 삭제 패킷 보내야함..
				m_hActor->CmdRemoveStateEffectFromID(GetBlowID());
			}
#endif // PRE_FIX_61438
		}
	}
#endif // _GAMESERVER

}

void CDnPuppetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(_GAMESERVER)
	if (m_hSummonMonster)
		m_hSummonMonster->CmdSuicide(false, false);
#endif // _GAMESERVER

	OutputDebug( "%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPuppetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnPuppetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW