#include "StdAfx.h"
#include "DnPartyAddDamageBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnPartyAddDamageBlow::CDnPartyAddDamageBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_279;
	SetValue( szValue );

#ifdef _GAMESERVER
	m_nAddDamage = atoi( szValue );
	AddCallBackType( SB_ONCALCDAMAGE );
#endif
}

CDnPartyAddDamageBlow::~CDnPartyAddDamageBlow(void)
{

}

void CDnPartyAddDamageBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}


void CDnPartyAddDamageBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);
}


void CDnPartyAddDamageBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(_GAMESERVER)
float CDnPartyAddDamageBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if( m_hActor && m_hActor->IsDie() == false )
	{
		if( m_nAddDamage > 0 && fOriginalDamage > 0 )
		{
			char buffer[65];
			_itoa_s(m_nAddDamage, buffer, 65, 10 );

			m_sDamageActorSkillInfo.hSkillUser = HitParam.hHitter;
			m_hActor->CmdAddStateEffect( &m_sDamageActorSkillInfo, STATE_BLOW::BLOW_177, 0, buffer );
		}
	}

	return 0.0f;
}
#endif // _GAMESERVER
