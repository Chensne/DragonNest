#include "StdAfx.h"
#include "DnMPBurstBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnMPBurstBlow::CDnMPBurstBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_223;
	SetValue( szValue );

#if defined(_GAMESERVER)
	m_fBurstRatio = 0.f;
	m_fBurstConditionLimit = 0.f;

	SetBlowInfo();
#endif
}


CDnMPBurstBlow::~CDnMPBurstBlow(void)
{

}

#if defined(_GAMESERVER)
void CDnMPBurstBlow::SetBlowInfo()
{
	std::string str = m_StateBlow.szValue;

	std::vector<std::string> tokens;
	std::string delimiters = ";";
	TokenizeA(str, tokens, delimiters);

	if(!tokens.empty())
	{
		switch(tokens.size())
		{
		case 1:
			m_fBurstRatio = (float)atof(tokens[0].c_str());
			break;

		case 2:
			m_fBurstRatio = (float)atof(tokens[0].c_str());
			m_fBurstConditionLimit = (float)atof(tokens[1].c_str());
			break;
		default:
			break;
		}
	}
}
#endif


void CDnMPBurstBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#if defined(_GAMESERVER)
	//#30953 1타 2킬 상황 막음. - 액터가 죽었으면 데미지 처리 안되도록..
#if defined(PRE_FIX_61382)
	DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
	if (0 != m_fBurstRatio &&
		hActor &&
		hActor->IsDie() == false)
	{
		float fSPRatio = 0.f;
		fSPRatio = (float)m_hActor->GetSP() / (float)m_hActor->GetMaxSP();

		if(m_fBurstConditionLimit == 0.f || ( m_fBurstConditionLimit != 0.f && m_fBurstConditionLimit < fSPRatio ) )
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), static_cast<int>(m_fBurstRatio * m_hActor->GetSP()) );
			m_hActor->UseMP( -(int)(m_fBurstRatio * (float)m_hActor->GetSP()) );

			// 죽었나 체크
			if( hActor->IsDie() )
				hActor->Die( m_ParentSkillInfo.hSkillUser	);
		}
	}
#else
	if (0 != m_fBurstRatio && !m_hActor->IsDie())
	{
		float fSPRatio = 0.f;
		fSPRatio = (float)m_hActor->GetSP() / (float)m_hActor->GetMaxSP();

		if(m_fBurstConditionLimit == 0.f || ( m_fBurstConditionLimit != 0.f && m_fBurstConditionLimit < fSPRatio ) )
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), static_cast<int>(m_fBurstRatio * m_hActor->GetSP()) );
			m_hActor->UseMP( -(int)(m_fBurstRatio * (float)m_hActor->GetSP()) );

			// 죽었나 체크
			if( m_hActor->GetHP() <= 0.f )
				m_hActor->Die( m_ParentSkillInfo.hSkillUser	);
		}
	}
#endif // PRE_FIX_61382
#endif // _GAMESERVER
}

void CDnMPBurstBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnMPBurstBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMPBurstBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}

void CDnMPBurstBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
