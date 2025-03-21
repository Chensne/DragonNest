#include "StdAfx.h"
#include "DnForceActionBlow.h"
#include "DnCantMoveBlow.h"
#include "DnCantAttackBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnForceActionBlow::CDnForceActionBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																					  m_pCantMoveBlow( new CDnCantMoveBlow(hActor, NULL) ),
																					  m_pCantAttackBlow( new CDnCantAttackBlow(hActor, NULL) ),
																					  m_StartTime( 0 ),
																					  m_fOriAddedHeight( 0.0f )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_084;
	SetValue( szValue );
	//m_nValue = atoi( szValue );
	m_fValue = (float)atof(szValue);
#ifndef _GAMESERVER
	m_pCantMoveBlow->UseTableDefinedGraphicEffect( false );
#endif

	// [2010/12/13 semozz]
	// 상태효과가 상태를 가지고 있는 경우 계속 유지를 위해서
	if (m_pCantMoveBlow)
		m_pCantMoveBlow->SetPermanent(true);
	if (m_pCantAttackBlow)
		m_pCantAttackBlow->SetPermanent(true);
}

CDnForceActionBlow::~CDnForceActionBlow(void)
{
	SAFE_DELETE( m_pCantMoveBlow );
	SAFE_DELETE( m_pCantAttackBlow );
}


void CDnForceActionBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_pCantMoveBlow->OnBegin( LocalTime, fDelta );
	m_pCantAttackBlow->OnBegin( LocalTime, fDelta );
	//m_hActor->SetStateEffect( m_hActor->GetStateEffect() | CDnActorState::Cant_AttackAction );

	m_StartTime = LocalTime;

	switch( (int)m_fValue )
	{
		case 0:
			SetState( STATE_BLOW::STATE_END );
			break;

		// 에어(공중에 떠서 지속)
		case 1:
			//m_fOriAddedHeight = m_hActor->GetAddHeight();
			
			// 공중에 뜨는 모션 실행
			break;

		// 다운(다운 상태에서 일어나지 못함)
		case 2:
			break;
	}

	OutputDebug( "CDnForceActionBlow::OnBegin, Value:%d \n", (int)m_fValue );
}


void CDnForceActionBlow::Process(LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

	switch( (int)m_fValue )
	{
		case 0:
			break;

			// 에어(공중에 떠서 지속)
		case 1:
			{
				//float fUp = 0.0f;
				//if( (m_hActor->GetAddHeight() - m_fOriAddedHeight) < 100.0f )
				//	fUp = float(LocalTime - m_StartTime) * 0.002f;

				//m_hActor->SetAddHeight( m_fOriAddedHeight + fUp );
				// 공중에 뜨는 모션 실행
			}
			break;

			// 다운(다운 상태에서 일어나지 못함)
		case 2:
			break;
	}
}


void CDnForceActionBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	m_pCantMoveBlow->OnEnd( LocalTime, fDelta );
	m_pCantAttackBlow->OnEnd( LocalTime, fDelta );
	//m_hActor->SetStateEffect( m_hActor->GetStateEffect() & ~CDnActorState::Cant_AttackAction );

	switch( (int)m_fValue )
	{
		case 0:
			m_hActor->SetActionQueue( "Down_SmallBounce" );
			m_hActor->SetDownRemainDelta( 3.0f );
			break;

		case 1:
			//m_hActor->SetActionQueue( "" );
			break;
	}
	
	// TODO: 원래대로 돌려놓음
	//m_hActor->SetAddHeight( m_fOriAddedHeight );

	OutputDebug( "CDnForceActionBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnForceActionBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnForceActionBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
