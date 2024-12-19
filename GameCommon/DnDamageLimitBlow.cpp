#include "StdAfx.h"
#include "DnDamageLimitBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnDamageLimitBlow::CDnDamageLimitBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_274;
	SetValue( szValue );
	m_fValue = (float)atof(szValue);

#ifdef _GAMESERVER
	m_fDamageLimit = 0;
	m_nSkillIndex = 0;
	m_fSumDamage = 0;
	m_tSkillUseDelay = 0;
	m_bOrderUseSkill = false;

	std::string str = szValue;
	std::vector<std::string> tokens;
	TokenizeA( str, tokens, ";" );

	if( tokens.size() == 2 )
	{
		m_fDamageLimit = (float)atof( tokens[0].c_str() );
		m_nSkillIndex = atoi( tokens[1].c_str() );
	}

	AddCallBackType( SB_ONCALCDAMAGE );
#endif
}

CDnDamageLimitBlow::~CDnDamageLimitBlow()
{

}


void CDnDamageLimitBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::OnBegin( LocalTime, fDelta );
}


void CDnDamageLimitBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	if( m_bOrderUseSkill == true )
	{
		if( LocalTime - m_tSkillUseDelay > 500 )
		{
			if( m_hActor->IsProcessSkill() )
				m_hActor->CancelUsingSkill();

			CDnSkill::UsingResult eResult = m_hActor->UseSkill( m_nSkillIndex, false );
			if( eResult == CDnSkill::UsingResult::Success )
				SetState(STATE_BLOW::STATE_END);

			m_tSkillUseDelay = LocalTime;
		}
	}
#endif

}


void CDnDamageLimitBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::OnEnd( LocalTime , fDelta );
}

#if defined(_GAMESERVER)

bool CDnDamageLimitBlow::CanBegin( void )
{
	DnSkillHandle hSkill = m_hActor->FindSkill( m_nSkillIndex );
	if( !hSkill )
		return false;

	if( (m_hActor && m_hActor->IsMonsterActor()) == false )
		return false;

	return true;
}

float CDnDamageLimitBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if( m_fSumDamage > m_hActor->GetMaxHP() * m_fDamageLimit )
	{
		m_bOrderUseSkill = true;
		return -fOriginalDamage;
	}
	else
	{
		m_fSumDamage += fOriginalDamage;
	}

	return 0.f;
}

#endif