#include "StdAfx.h"
#include "DnShieldBlow.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnShieldBlow::CDnShieldBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_055;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	m_iDurability = (int)m_fValue;

	m_bCalcDamage = false;

#if defined(_GAMESERVER)
	AddCallBackType( SB_AFTERONDEFENSEATTACK );
	AddCallBackType( SB_ONCALCDAMAGE );
#endif // _GAMESERVER
}

CDnShieldBlow::~CDnShieldBlow(void)
{

}



void CDnShieldBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnBlockBlow::OnBegin, Value:%d \n", (int)m_fValue );
}



void CDnShieldBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	if( m_iDurability <= 0 )
	{
		// 클라에게 상태효과 제거하란 패킷만 보내고 서버에서는 순리대로 blow 를 해제시킨다.
		m_hActor->CmdRemoveStateEffect( m_StateBlow.emBlowIndex, false );
		SetState( STATE_BLOW::STATE_END );
	}
#endif
}


void CDnShieldBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnBlockBlow::OnEnd\n");
}


#ifdef _GAMESERVER
bool CDnShieldBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
/*	//  #25797 히트 시그널에 bIgnoreParring 이 켜져 있으면 발동하지 않는다.
	if( HitParam.bIgnoreParring )
		return false;

#if defined(PRE_FIX_BLOCK_CONDITION)
	//블럭의 발동이 fDamage가 0이라도 적이라면 블럭 발동 가능 하도록..
	//아군인 경우 기존 처럼 fDamage 가0이면 블럭 되지 않도록 함.
	if (IsCanBlock(hHitter, m_hActor, HitParam) == false)
		return false;
#else
	// hit percent 가 0% 일 경우 발동하지 않는 것으로 됨. (#21175)
	if( 0.0f == HitParam.fDamage )
		return false;
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	// 피격자가 디버프 (Freezing / FrameStop) 상태효과가 적용되어 있는 경우는 블럭 발동 되지 않도록..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166
*/
	bool bResult = false;

	//여기서 데미지 보정 계산은 안되야 한다..
	m_bCalcDamage = false;
	int fDamage = (int)m_hActor->PreCalcDamage(hHitter, HitParam);

	//////////////////////////////////////////////////////////////////////////
	//#37215관련..
	//내구도가 데미지보다 많을경우 여기서 방어 성공하면 내구도를 데미지만큼 감소
	//내구도가 데미지보다 적을 경우 여기서 방어 실패..
	//방어 실패때문에 OnCalcDamage가 호출 됨..
	//방어 성공 하면 OnCalcDamage는 호출 안됨..
	//////////////////////////////////////////////////////////////////////////
	if( m_iDurability >= fDamage )
	{
		m_iDurability -= fDamage;

		//맞는 동작 하지 않도록한다.
		HitParam.szActionName.assign( "" );	

		bResult = true;
	}
	else
	{
		//여기에 들어 오게 되면 OnCalcDamage로 들어 가게 되고
		//내구도 만큼 데미지 보정을 해줘야 한다.
		m_bCalcDamage = true;
	}
	
	return bResult;
}

float CDnShieldBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	//OnDefenseAttack에서도 계산이 되므로 실제 데미지 계산에서만 데미지 보정 처리가 되로록한다.
	if (m_bCalcDamage == false)
		return 0.0f;

	//데미지 보정 계산
	//남은 내구도 만큼 데미지 보정하고, 내구도는 0으로.
	float fDelta = (float)m_iDurability;
	m_iDurability = 0;

	return -fDelta;
}
#endif


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnShieldBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnShieldBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
