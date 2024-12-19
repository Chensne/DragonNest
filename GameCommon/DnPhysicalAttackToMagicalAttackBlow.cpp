#include "StdAfx.h"
#include "DnPhysicalAttackToMagicalAttackBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnPhysicalAttackToMagicalAttackBlow::CDnPhysicalAttackToMagicalAttackBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_180;
	SetValue( szValue );
}

CDnPhysicalAttackToMagicalAttackBlow::~CDnPhysicalAttackToMagicalAttackBlow(void)
{

}



void CDnPhysicalAttackToMagicalAttackBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}



void CDnPhysicalAttackToMagicalAttackBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnPhysicalAttackToMagicalAttackBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// 매지컬 브리즈 상태효과가 종료될 때 바뀌어서 들어갔던 상태효과들 원래대로 돌려놓고.
	DNVector( DnBlowHandle ) vlBlows;
	m_hActor->GetAllAppliedStateBlow( vlBlows );
	for( int i = 0; i < (int)vlBlows.size(); ++i )
	{
		DnBlowHandle hBlow = vlBlows.at( i );
		if( hBlow->IsChangedBlowIndex() )
		{
			hBlow->RestoreOriginalBlowIndex();
		}
	}
	
	// 모든 능력치 다시 갱신.
	m_hActor->RefreshState();

	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPhysicalAttackToMagicalAttackBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnPhysicalAttackToMagicalAttackBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


