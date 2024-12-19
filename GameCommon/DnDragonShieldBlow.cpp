#include "StdAfx.h"
#include "DnDragonShieldBlow.h"
#include "DnStateBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDragonShieldBlow::CDnDragonShieldBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_280;
	SetValue( szValue );
	m_fValue = (float)atof(szValue);

#if defined( _GAMESERVER )

	m_nSubSkillIndex = 0;
	m_fActivateCooltime = 0.f;
	m_nRestartCooltime = 0;
	m_nProbablity = 0;

	std::vector<string> vlTokens;
	std::string strValue = szValue;
	TokenizeA( strValue, vlTokens, ";" );

	if( vlTokens.size() == 4 )
	{
		m_nSubSkillIndex = (int)atoi( vlTokens[0].c_str() );
		m_nRestartCooltime = (int)atoi( vlTokens[1].c_str() );
		m_nProbablity = (int)atoi( vlTokens[2].c_str() );
		m_strEffectName = vlTokens[3];
	}

	AddCallBackType( SB_ONCALCDAMAGE );

	CDnSkill::CreateSkillInfo( m_nSubSkillIndex, 1, m_sSubSkillInfo, m_vecSubSkillEffectList );
	m_sSubSkillInfo.hSkillUser = m_hActor;
#endif
}

CDnDragonShieldBlow::~CDnDragonShieldBlow(void)
{

}


#ifdef _GAMESERVER

void CDnDragonShieldBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
}


void CDnDragonShieldBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_fActivateCooltime > 0 )
		m_fActivateCooltime -= fDelta;

	CDnBlow::Process( LocalTime, fDelta );
}

void CDnDragonShieldBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
}

float CDnDragonShieldBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if( m_fActivateCooltime <= 0 )
	{
		if( (_rand(GetRoom())%100) <= m_nProbablity )
		{
			m_fActivateCooltime = float(m_nRestartCooltime);
			if( m_hActor && m_hActor->IsDie() == false )
			{
				for( int i = 0; i < (int)m_vecSubSkillEffectList.size(); ++i )
				{
					m_hActor->CmdAddStateEffect( 
						&m_sSubSkillInfo, 
						(STATE_BLOW::emBLOW_INDEX)m_vecSubSkillEffectList[i].nID,
						m_vecSubSkillEffectList[i].nDurationTime, 
						m_vecSubSkillEffectList[i].szValue.c_str() );
				}

				m_hActor->CmdAddStateEffect( NULL , STATE_BLOW::BLOW_100 , 0 , FormatA( "%d;%s", 1, m_strEffectName.c_str() ).c_str() ); // 이펙트를 위한 용도
			}
			return -fOriginalDamage;
		}
	}

	return 0.f;
}

#endif