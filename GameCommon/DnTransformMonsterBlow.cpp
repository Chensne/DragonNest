#include "StdAfx.h"
#include "DnTransformMonsterBlow.h"
#include "DnMonsterActor.h"


CDnTransformMonsterBlow::CDnTransformMonsterBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_277;

	m_nTransformActorID = 0;
	m_nOriginalActorID = 0;

	SetValue( szValue );

	std::string str = szValue;
	std::vector<std::string> tokens;

	TokenizeA( str, tokens,  ";" );
	
	if( tokens.size() > 0 )
		m_nTransformActorID = atoi( tokens[0].c_str() );

#if defined(_GAMESERVER)
	m_bOrderUseSkill = false;
	m_nTransformSkillID = 0;

	if( tokens.size() > 1 )
		m_nTransformSkillID = atoi( tokens[1].c_str() );
#endif

}

CDnTransformMonsterBlow::~CDnTransformMonsterBlow(void)
{

}

void CDnTransformMonsterBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	if(m_hActor && m_hActor->IsMonsterActor() )
	{
		CDnMonsterActor *pMonster = static_cast<CDnMonsterActor*>(m_hActor.GetPointer());
		m_nOriginalActorID = pMonster->GetActorTableID();
		pMonster->SwapActor( m_nTransformActorID ); 
#if defined(_GAMESERVER)
		m_bOrderUseSkill = true;
#endif
	}
#endif

}

void CDnTransformMonsterBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
#if defined(_GAMESERVER)
	if( m_bOrderUseSkill == true )
	{
		if( m_nTransformSkillID > 0 )
		{
			if( m_hActor )
				m_hActor->UseSkill( m_nTransformSkillID, false );
		}
		m_bOrderUseSkill = false;
	}
#endif
#endif
}

void CDnTransformMonsterBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// 몬스터는 AI및 기타 여러가지 상황이 있기때문에 
	// 변신되는 상황을 AI 에서 관리하도록 합니다.
	// 종료될때 돌아오지 않도록 설정합니다.
	// 이 부분은 앞으로 변경될 수 있습니다.
}
