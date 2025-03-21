#include "StdAfx.h"
#include "DnBoneCrushBlow.h"
#include "DnSkill.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnBoneCrushBlow::CDnBoneCrushBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_161;
	SetValue( szValue );
	
	m_nAddDamage = atoi( szValue );
	m_bOnDamaged = false;

#ifdef _GAMESERVER
	
	AddCallBackType( SB_ONCALCDAMAGE );
#endif
}

CDnBoneCrushBlow::~CDnBoneCrushBlow(void)
{

}

void CDnBoneCrushBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}


void CDnBoneCrushBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

#if defined(_GAMESERVER)
#if defined(PRE_FIX_61382)
	DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
	//#30953 1타 2킬 상황 막음. - 액터가 죽었으면 데미지 처리 안되도록..
	if (m_bOnDamaged && hActor && !hActor->IsDie())
	{
		m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), m_nAddDamage);
		m_bOnDamaged = false;

		// 죽었나 체크
		if( hActor->IsDie() )
			hActor->Die( m_ParentSkillInfo.hSkillUser	);

		OutputDebug("%s Damage :: %d\n", __FUNCTION__, m_nAddDamage);
	}
#else
	//#30953 1타 2킬 상황 막음. - 액터가 죽었으면 데미지 처리 안되도록..
	if (m_bOnDamaged && m_hActor && !m_hActor->IsDie())
	{
		m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), m_nAddDamage);
		m_bOnDamaged = false;

		// 죽었나 체크
		if( m_hActor->GetHP() <= 0.f )
			m_hActor->Die( m_ParentSkillInfo.hSkillUser	);

		OutputDebug("%s Damage :: %d\n", __FUNCTION__, m_nAddDamage);
	}
#endif // PRE_FIX_61382
#endif // _GAMESERVER
}


void CDnBoneCrushBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(_GAMESERVER)
float CDnBoneCrushBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	//여기 들어 오면 맞은거.. 플래그 설정 해놓고.. Process에서 추가 데미지 요청하고 리셋..
	m_bOnDamaged = true;

	return 0.0f;
}
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnBoneCrushBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	int iValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	iValue[0] = atoi( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	iValue[1] = atoi( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int iResultValue = iValue[0] + iValue[1];

	sprintf_s(szBuff, "%d", iResultValue);

	szNewValue = szBuff;
}

void CDnBoneCrushBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	int iValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	iValue[0] = atoi( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	iValue[1] = atoi( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	int iResultValue = iValue[0] - iValue[1];

	sprintf_s(szBuff, "%d", iResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW