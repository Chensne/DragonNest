#include "StdAfx.h"
#include "DnChangeWeaponRangeBlow.h"
#include "DnWorld.h"


CDnChangeWeaponRangeBlow::CDnChangeWeaponRangeBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_065;

	SetValue( szValue );
	m_fValue = (float)atof(szValue);

	m_fIncreaseRange = 0;
}

CDnChangeWeaponRangeBlow::~CDnChangeWeaponRangeBlow(void)
{

}


void CDnChangeWeaponRangeBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
#ifdef _CLIENT
	if( (CDnWorld::MapTypeEnum::MapTypeVillage == CDnWorld::GetInstancePtr()->GetMapType()) )
		return;
#endif

	DnWeaponHandle hMainWeapon = m_hActor->GetWeapon( 0 );
	if( hMainWeapon && hMainWeapon != m_hAppliedWeapon )
	{
		hMainWeapon->SetWeaponLength( hMainWeapon->GetWeaponLength() + (int)m_fValue );
		m_hAppliedWeapon = hMainWeapon;
		m_fIncreaseRange = m_fValue;
	}
}

void CDnChangeWeaponRangeBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _CLIENT
	if( (CDnWorld::MapTypeEnum::MapTypeVillage == CDnWorld::GetInstancePtr()->GetMapType()) )
		return;
#endif

	DnWeaponHandle hMainWeapon = m_hActor->GetWeapon( 0 );
	if( hMainWeapon && hMainWeapon != m_hAppliedWeapon )
	{
		hMainWeapon->SetWeaponLength( hMainWeapon->GetWeaponLength() + (int)m_fValue );
		m_hAppliedWeapon = hMainWeapon;
		m_fIncreaseRange = m_fValue;
	}
}


void CDnChangeWeaponRangeBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangeWeaponRangeBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _CLIENT
	if( (CDnWorld::MapTypeEnum::MapTypeVillage == CDnWorld::GetInstancePtr()->GetMapType()) )
		return;
#endif

	DnWeaponHandle hMainWeapon = m_hActor->GetWeapon( 0 );
	if( hMainWeapon && hMainWeapon == m_hAppliedWeapon )
	{
		hMainWeapon->SetWeaponLength( hMainWeapon->GetWeaponLength() - (int)m_fIncreaseRange );
	}
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeWeaponRangeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };
	float fValue[2];

	fValue[0] = (float)atof( szOrigValue );
	fValue[1] = (float)atof( szAddValue );

	float fResultValue = fValue[0] + fValue[1];
	sprintf_s(szBuff, "%f", fResultValue);
	szNewValue = szBuff;
}

void CDnChangeWeaponRangeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };
	float fValue[2];

	fValue[0] = (float)atof( szOrigValue );
	fValue[1] = (float)atof( szAddValue );

	float fResultValue = fValue[0] - fValue[1];
	sprintf_s(szBuff, "%f", fResultValue);
	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
